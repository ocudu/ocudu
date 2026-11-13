// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/* DIFI (IEEE-ISTO Std 4900, VITA 49.2) TRX driver for Amarisoft LTE/NR components.
 *
 * Carries baseband over UDP as DIFI data and context packets, so an Amarisoft UE can connect directly to the DIFI
 * radio of an ocudu gNB. The driver is self-contained: it depends only on the C and C++ standard libraries and on the
 * Amarisoft trx_driver.h API.
 *
 * The gNB's ru_sdr.device_args name the ports from the gNB's point of view, so they cross over here:
 *
 *     gNB  device_args: tx_addr=127.0.0.1,tx_port=4991,rx_addr=127.0.0.1,rx_port=4992
 *     UE   args:        rx_port=4991,tx_addr=127.0.0.1,tx_port=4992
 *
 * Each antenna (channel) of the RF port travels as a DIFI stream of its own, on its own UDP port, with stream
 * identifier stream_id + channel. Ports are listed per channel as rx_port0, rx_port1, ..., the same way the gNB lists
 * them; a plain rx_port is channel 0. For 2x2:
 *
 *     gNB  device_args: tx_port0=4991,tx_port1=4993,rx_port0=4992,rx_port1=4994
 *     UE   args:        rx_port0=4991,rx_port1=4993,tx_port0=4992,tx_port1=4994
 *
 * Design notes:
 *
 *  - Receive is sample-locked. trx_read_func2() blocks until the peer's datagrams arrive and returns what it has, so
 *    the peer's stream is this radio's clock, as a hardware radio's read blocks on its converter. Pacing to a local
 *    wall clock instead would give the two ends independent clocks that drift apart.
 *
 *  - Silence is transmitted, not skipped. The application marks buffers it knows are empty
 *    (TRX_WRITE_MD_FLAG_PADDING, samples == NULL). DIFI is a continuous timestamped stream, so omitting such a span
 *    would leave a hole in it.
 *
 *  - Float to int16 conversion saturates, since an out-of-range cast to int16_t wraps.
 *
 *  - The header is big-endian. The 16-bit IQ payload is big-endian too by default, per VITA 49.2. Some DIFI tooling
 *    serialises the payload in host byte order instead, which iq_byte_order=little selects. Both ends must agree; the
 *    ocudu gNB takes the same key in its ru_sdr device_args.
 */

#include <arpa/inet.h>
#include <cinttypes>
#include <emmintrin.h>
#include <errno.h>
#include <math.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <vector>

extern "C" {
#include <trx_driver.h>
}

/* ---- DIFI packet layout ---------------------------------------------------- */

/* Data packet header: 7 32-bit words before the payload. */
#define DIFI_DATA_HEADER_SIZE 28
/* The DIFI standard flow context packet is a fixed 108 bytes. */
#define DIFI_CONTEXT_SIZE 108
/* Packet type, in bits [31:28] of the header word. */
#define DIFI_PKT_TYPE_SHIFT 28
#define DIFI_PKT_TYPE_MASK 0xfU
#define DIFI_PKT_TYPE_DATA 0x1U
/* Packet count, in bits [19:16] of the header word. VITA packet counters are 4-bit. */
#define DIFI_PKT_COUNT_SHIFT 16
#define DIFI_PKT_COUNT_MASK 0xfU
#define DIFI_PKT_MOD 16
/* Header bits that never vary, per packet type: packet type, class ID present, TSI = UTC and TSF = real time
 * (picoseconds). Data and context packets declare the same epoch. */
#define DIFI_DATA_STATIC_BITS 0x18600000U
#define DIFI_CTX_STATIC_BITS 0x49600000U
/* DIFI Consortium OUI, in the upper word of the class ID. */
#define DIFI_OUI 0x6a621eU
#define DIFI_OUI_SHIFT 32
/* Pad bit count of the class ID field: the bits of zero padding at the end of the payload, in its top five bits. */
#define DIFI_PAD_BITS_SHIFT 59
#define DIFI_PAD_BITS_MASK 0x1fU
#define BITS_PER_BYTE 8
/* Packet class code, in the lower word of the class ID: 1 for a standard flow context packet, 0 for a data packet. */
#define DIFI_CTX_CLASS_CODE 1U
/* Context indicator field 0: the fields present in the standard flow context packet. */
#define DIFI_CTX_CIF0 0xfbb98000U
/* Reference point identifier for the standard flow context packet. */
#define DIFI_REF_POINT 0x64U
/* Timestamp adjustment advertised in the context packet: 10 us, in femtoseconds. */
#define DIFI_TIMESTAMP_ADJ_FS 10000000000ULL
/* Default state and event indicator word. */
#define DIFI_STATE_AND_EVENTS 0x9ff00000U
/* Data packet payload format words: 8- and 16-bit signed cartesian, link-efficient packing. */
#define DIFI_PAYLOAD_FMT_8BIT 0xa00003c700000000ULL
#define DIFI_PAYLOAD_FMT_16BIT 0xa00007cf00000000ULL
/* Frequencies and rates are 64-bit fixed point with 20 fractional bits; gains are 16-bit with 7. */
#define DIFI_FREQ_RADIX (1 << 20)
#define DIFI_GAIN_RADIX 128.0
#define DIFI_GAIN_MASK 0xffffU

/* Byte offsets of the fields in a data or context packet. */
#define DIFI_OFFSET_HEADER 0
#define DIFI_OFFSET_STREAM_ID 4
#define DIFI_OFFSET_CLASS_ID 8
#define DIFI_OFFSET_INT_TS 16
#define DIFI_OFFSET_FRAC_TS 20
/* Byte offsets of the fields that follow the prologue in a context packet. */
#define DIFI_CTX_OFFSET_CIF0 28
#define DIFI_CTX_OFFSET_REF_POINT 32
#define DIFI_CTX_OFFSET_BANDWIDTH 36
#define DIFI_CTX_OFFSET_IF_REF_FREQ 44
#define DIFI_CTX_OFFSET_RF_REF_FREQ 52
#define DIFI_CTX_OFFSET_IF_BAND_OFFSET 60
#define DIFI_CTX_OFFSET_REF_LEVEL 68
#define DIFI_CTX_OFFSET_GAIN 72
#define DIFI_CTX_OFFSET_SAMPLE_RATE 76
#define DIFI_CTX_OFFSET_TS_ADJ 84
#define DIFI_CTX_OFFSET_TS_CAL_TIME 92
#define DIFI_CTX_OFFSET_STATE_EVENTS 96
#define DIFI_CTX_OFFSET_PAYLOAD_FMT 100

#define PICO_PER_SEC 1000000000000ULL
#define NANO_PER_SEC 1000000000LL
#define NANO_PER_MICRO 1000
#define NANO_PER_MILLI 1000000LL
#define HZ_PER_MHZ 1000000
#define BYTES_PER_MB (1024 * 1024)

#define MAX_UDP_PAYLOAD 65507
#define MAX_UDP_PORT 65535

/* Most channels (antennas) on the one supported RF port, as many as the gNB carries per cell. */
#define DIFI_MAX_CHANNELS 8
/* How long a read waits for another channel to catch up with channel 0. The peer sends every channel of a buffer back
 * to back, so its data trails by microseconds; the bound only matters for a channel whose transmitter has stopped,
 * which then costs this much once before it is drained without waiting. */
#define DIFI_CHANNEL_WAIT_MS 5

/* Full scale of a 16-bit IQ component. */
#define INT16_FULL_SCALE 32768.0
/* Sample rates the Amarisoft application accepts: n x 1.92 MHz, with n = 2^a * 3^b * 5^c. */
#define BASE_SAMPLE_RATE_HZ 1920000
#define MAX_SAMPLE_RATE_MULTIPLIER 64
/* Margin of the sample rate over the bandwidth the application asks for, the same as Amarisoft's reference drivers. */
#define SAMPLE_RATE_MARGIN 1.15

/* Defaults: the ocudu gNB's DIFI radio defaults with the ports crossed over, so a local link needs no arguments. */
#define DEFAULT_RX_ADDR "0.0.0.0"
#define DEFAULT_TX_ADDR "127.0.0.1"
#define DEFAULT_RX_PORT 4991
#define DEFAULT_TX_PORT 4992
#define DEFAULT_BIT_DEPTH 16
#define DEFAULT_PKT_SAMPLES 1920
#define DEFAULT_CTX_INTERVAL 100
#define DEFAULT_RX_TIMEOUT_MS 1000
#define DEFAULT_RCVBUF_MB 16
/* SO_RCVBUF takes an int byte count, so 2048 MB would overflow it. */
#define MAX_RCVBUF_MB 2047

/* ---- Big-endian helpers --------------------------------------------------- */

static inline void pack_u32(uint8_t* p, uint32_t v)
{
  p[0] = (uint8_t)(v >> 24);
  p[1] = (uint8_t)(v >> 16);
  p[2] = (uint8_t)(v >> 8);
  p[3] = (uint8_t)v;
}

static inline void pack_u64(uint8_t* p, uint64_t v)
{
  pack_u32(p, (uint32_t)(v >> 32));
  pack_u32(p + 4, (uint32_t)v);
}

static inline void pack_u16(uint8_t* p, uint16_t v)
{
  p[0] = (uint8_t)(v >> 8);
  p[1] = (uint8_t)v;
}

static inline uint16_t unpack_u16(const uint8_t* p)
{
  return (uint16_t)(((uint16_t)p[0] << 8) | (uint16_t)p[1]);
}

static inline uint32_t unpack_u32(const uint8_t* p)
{
  return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static inline uint64_t unpack_u64(const uint8_t* p)
{
  return ((uint64_t)unpack_u32(p) << 32) | unpack_u32(p + 4);
}

/* ---- Timestamp conversion ------------------------------------------------- */

/* Sample ticks to DIFI integer seconds plus fractional picoseconds.
 *
 * The obvious rem * PICO_PER_SEC / rate overflows: rem can approach the sample rate, so the product
 * reaches 1e20 against a 1.8e19 ceiling. Splitting PICO_PER_SEC into its quotient and remainder over
 * the rate keeps every intermediate inside 64 bits and stays exact. */
static void difi_ticks_to_time(uint64_t ticks, uint64_t rate, uint32_t* full_secs, uint64_t* frac_ps)
{
  if (rate == 0) {
    *full_secs = 0;
    *frac_ps   = 0;
    return;
  }
  const uint64_t q   = PICO_PER_SEC / rate;
  const uint64_t r   = PICO_PER_SEC % rate;
  const uint64_t rem = ticks % rate;

  *full_secs = (uint32_t)(ticks / rate);
  *frac_ps   = rem * q + (rem * r) / rate;
}

/* Inverse, rounding to nearest so a value round-trips exactly. __int128 is used because
 * frac_ps * rate reaches 1e20 for high sample rates. */
static uint64_t difi_time_to_ticks(uint32_t full_secs, uint64_t frac_ps, uint64_t rate)
{
  if (rate == 0)
    return 0;
  const unsigned __int128 scaled = (unsigned __int128)frac_ps * rate + (PICO_PER_SEC / 2);
  return (uint64_t)full_secs * rate + (uint64_t)(scaled / PICO_PER_SEC);
}

/* ---- Driver state --------------------------------------------------------- */

/* One transmit channel: a DIFI stream of its own, with its own destination and packet counters. */
struct DifiTxChannel {
  int                port;
  int                fd;
  struct sockaddr_in dest;
  double             gain;
  int64_t            freq;
  uint8_t            data_pkt_n;
  uint8_t            ctx_pkt_n;
  int                pkts_since_ctx;
  uint64_t           send_failures;
};

/* One receive channel. Channel 0 drives each read; the others are filled to match it. */
struct DifiRxChannel {
  int    port;
  int    fd;
  double gain;
  /* Samples from a datagram that ran past the end of the caller's buffer. The caller's count need not be a multiple
   * of the peer's packet size, so a datagram can straddle the boundary. The remainder is kept for the next read,
   * which asks for exactly what follows; dropping it would leave a hole that makes every later read start
   * non-contiguous. */
  std::vector<float> carry;
  uint64_t           carry_ts;
  /* Whether a data packet has arrived since start, kept for the diagnostics. */
  bool active;
  /* Whether a read waits for this channel. Set by each data packet and cleared when a wait times out, so a channel
   * whose transmitter has stopped costs one wait, not one per read. */
  bool waiting;
  /* Spans that had to be zero-filled, and datagrams dropped as already behind the buffer. */
  uint64_t gaps;
  uint64_t stale;
};

struct TRXDifiState {
  /* Configuration. */
  std::string rx_addr = DEFAULT_RX_ADDR;
  std::string tx_addr = DEFAULT_TX_ADDR;
  /* Port of each channel, -1 where none was given. */
  int rx_ports[DIFI_MAX_CHANNELS];
  int tx_ports[DIFI_MAX_CHANNELS];
  /* Stream identifier of channel 0; channel c uses stream_id + c. */
  uint32_t stream_id = 0;
  /* IQ component size in bits, 8 or 16. */
  int bit_depth = DEFAULT_BIT_DEPTH;
  /* IQ samples per transmitted data packet. */
  int pkt_samples = DEFAULT_PKT_SAMPLES;
  /* Scaling between the application's floats, around +/-1.0, and full-scale int16. */
  double tx_scale = INT16_MAX;
  double rx_scale = 1.0 / INT16_FULL_SCALE;
  /* Data packets between context packets. */
  int ctx_interval = DEFAULT_CTX_INTERVAL;
  /* How long a blocking read waits before reporting nothing. */
  int rx_timeout_ms = DEFAULT_RX_TIMEOUT_MS;
  /* Requested SO_RCVBUF in MB; the kernel caps it at net.core.rmem_max. */
  int rcvbuf_mb = DEFAULT_RCVBUF_MB;
  /* 16-bit IQ payload byte order: big (VITA 49.2, default) or little. */
  bool iq_big_endian = true;
  /* Sample rate the driver negotiates, in Hz; 0 derives it from the bandwidth. */
  int64_t forced_srate = 0;

  /* Runtime. */
  uint64_t                   sample_rate = 0;
  std::vector<DifiTxChannel> tx;
  std::vector<DifiRxChannel> rx;
  bool                       started = false;

  /* Receive reassembly, shared by every channel, since the peer stamps all of them from one clock. */
  bool ts_offset_valid = false;
  /* Offset from the peer's timestamps to the local sample timeline. */
  int64_t ts_offset = 0;
  /* Local timeline position expected next. */
  uint64_t             next_rx_ts = 0;
  std::vector<uint8_t> rx_buf;
  std::vector<uint8_t> tx_pkt;

  /* Statistics of channel 0. */
  uint64_t rx_overflows      = 0;
  uint64_t rx_non_contiguous = 0;

  /* Timing, logged once a second when timing_log is set, to tell whether stalls come from the peer or from the
   * application. A read that blocks long is waiting on the peer's stream; a shrinking transmit advance means the
   * application is late handing over its buffers. */
  bool     timing_log             = false;
  int64_t  timing_period_start_ns = 0;
  int64_t  max_read_wait_ns       = 0;
  uint64_t nof_long_read_waits    = 0;
  /* Samples between a transmit buffer and the latest received sample. */
  int64_t min_tx_advance  = 0;
  bool    have_tx_advance = false;
  /* Longest gaps between successive read and write calls: a pause of the application shows in both. */
  int64_t last_read_ns     = 0;
  int64_t last_write_ns    = 0;
  int64_t max_read_gap_ns  = 0;
  int64_t max_write_gap_ns = 0;
};

static void difi_log(const char* fmt, ...)
{
  va_list ap;
  fprintf(stderr, "DIFI TRX: ");
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fprintf(stderr, "\n");
}

static int64_t monotonic_ns(void)
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (int64_t)ts.tv_sec * NANO_PER_SEC + ts.tv_nsec;
}

/* Time left until an absolute monotonic deadline, as a poll() timeout in whole milliseconds rounded up. */
static int difi_poll_timeout_ms(int64_t deadline_ns)
{
  const int64_t remaining_ns = deadline_ns - monotonic_ns();
  if (remaining_ns <= 0)
    return 0;
  return (int)((remaining_ns + NANO_PER_MILLI - 1) / NANO_PER_MILLI);
}

/* A read that blocks for longer than this counts as a long wait in the timing log. */
#define DIFI_LONG_READ_WAIT_NS 1000000LL
#define DIFI_TIMING_PERIOD_NS NANO_PER_SEC
#define MICRO_PER_SEC 1e6

/* Logs and resets the timing statistics once a period. Called from the read path. */
static void difi_report_timing(TRXDifiState* s, int64_t now_ns)
{
  if (s->timing_period_start_ns == 0) {
    s->timing_period_start_ns = now_ns;
    return;
  }
  if (now_ns - s->timing_period_start_ns < DIFI_TIMING_PERIOD_NS)
    return;
  const double us_per_sample = MICRO_PER_SEC / (double)s->sample_rate;
  /* Wall-clock time, so the line can be matched against the peer's own once-a-second timing. */
  char         wall[16];
  const time_t now_s = time(NULL);
  struct tm    tm_now;
  localtime_r(&now_s, &tm_now);
  strftime(wall, sizeof(wall), "%H:%M:%S", &tm_now);
  if (s->have_tx_advance) {
    fprintf(stderr,
            "DIFI TRX timing %s: max_read_wait=%" PRId64 "us long_read_waits=%" PRIu64
            " min_tx_advance=%.0fus max_read_gap=%" PRId64 "us max_write_gap=%" PRId64 "us\n",
            wall,
            s->max_read_wait_ns / NANO_PER_MICRO,
            s->nof_long_read_waits,
            (double)s->min_tx_advance * us_per_sample,
            s->max_read_gap_ns / NANO_PER_MICRO,
            s->max_write_gap_ns / NANO_PER_MICRO);
  } else {
    fprintf(stderr,
            "DIFI TRX timing %s: max_read_wait=%" PRId64 "us long_read_waits=%" PRIu64
            " min_tx_advance=n/a max_read_gap=%" PRId64 "us max_write_gap=%" PRId64 "us\n",
            wall,
            s->max_read_wait_ns / NANO_PER_MICRO,
            s->nof_long_read_waits,
            s->max_read_gap_ns / NANO_PER_MICRO,
            s->max_write_gap_ns / NANO_PER_MICRO);
  }
  s->max_read_gap_ns        = 0;
  s->max_write_gap_ns       = 0;
  s->timing_period_start_ns = now_ns;
  s->max_read_wait_ns       = 0;
  s->nof_long_read_waits    = 0;
  s->have_tx_advance        = false;
}

/* ---- Argument parsing ----------------------------------------------------- */

/* Parses a per-channel port key: "<prefix>" alone is channel 0, "<prefix><k>" is channel k. Returns the
 * channel, -1 if the key is not one of these, or -2 if the channel number is out of range. */
static int difi_port_key_channel(const std::string& key, const char* prefix)
{
  const size_t len = strlen(prefix);
  if (key.compare(0, len, prefix) != 0)
    return -1;
  if (key.size() == len)
    return 0;
  const std::string digits = key.substr(len);
  if (digits.size() != 1 || digits[0] < '0' || digits[0] > '9')
    return -1;
  const int channel = digits[0] - '0';
  return (channel < DIFI_MAX_CHANNELS) ? channel : -2;
}

/* Number of channels with a port, which must be numbered 0 to n-1 without holes. Returns -1 otherwise. */
static int difi_count_ports(const int* ports, const char* name)
{
  int count = 0;
  while (count < DIFI_MAX_CHANNELS && ports[count] >= 0)
    count++;
  for (int i = count; i < DIFI_MAX_CHANNELS; i++) {
    if (ports[i] >= 0) {
      difi_log("%s%d is given but %s%d is not; channels must be numbered from 0 without gaps", name, i, name, count);
      return -1;
    }
  }
  return count;
}

/* Accepts the same comma separated key=value form as ocudu's DIFI radio, so both ends of a link can
 * be configured in the same idiom. Unknown keys are rejected rather than ignored: a silently dropped
 * key is how a link ends up half-configured and pointing at the wrong port. */
static int difi_parse_args(TRXDifiState* s, const char* args)
{
  if (!args || !*args)
    return 0;

  std::string in(args);
  size_t      pos = 0;
  while (pos < in.size()) {
    size_t comma = in.find(',', pos);
    if (comma == std::string::npos)
      comma = in.size();
    std::string tok = in.substr(pos, comma - pos);
    pos             = comma + 1;
    if (tok.empty())
      continue;

    size_t eq = tok.find('=');
    if (eq == std::string::npos) {
      difi_log("argument '%s' is malformed, expected key=value", tok.c_str());
      return -1;
    }
    std::string key = tok.substr(0, eq);
    std::string val = tok.substr(eq + 1);

    const int rx_channel = difi_port_key_channel(key, "rx_port");
    const int tx_channel = difi_port_key_channel(key, "tx_port");
    if (rx_channel == -2 || tx_channel == -2) {
      difi_log("'%s' names a channel beyond the %d supported", key.c_str(), DIFI_MAX_CHANNELS);
      return -1;
    }

    if (key == "rx_addr") {
      s->rx_addr = val;
    } else if (key == "tx_addr") {
      s->tx_addr = val;
    } else if (rx_channel >= 0) {
      s->rx_ports[rx_channel] = atoi(val.c_str());
    } else if (tx_channel >= 0) {
      s->tx_ports[tx_channel] = atoi(val.c_str());
    } else if (key == "stream_id") {
      s->stream_id = (uint32_t)strtoul(val.c_str(), NULL, 0);
    } else if (key == "bit_depth") {
      s->bit_depth = atoi(val.c_str());
    } else if (key == "pkt_samples") {
      s->pkt_samples = atoi(val.c_str());
    } else if (key == "tx_scale") {
      s->tx_scale = atof(val.c_str());
    } else if (key == "rx_scale") {
      s->rx_scale = atof(val.c_str());
    } else if (key == "ctx_interval") {
      s->ctx_interval = atoi(val.c_str());
    } else if (key == "iq_byte_order") {
      if (val == "big") {
        s->iq_big_endian = true;
      } else if (val == "little") {
        s->iq_big_endian = false;
      } else {
        difi_log("iq_byte_order must be 'big' or 'little', got '%s'", val.c_str());
        return -1;
      }
    } else if (key == "rcvbuf_mb") {
      s->rcvbuf_mb = atoi(val.c_str());
    } else if (key == "rx_timeout_ms") {
      s->rx_timeout_ms = atoi(val.c_str());
    } else if (key == "timing_log") {
      s->timing_log = atoi(val.c_str()) != 0;
    } else if (key == "srate") {
      s->forced_srate = (int64_t)atof(val.c_str());
    } else {
      difi_log("unknown argument key '%s'", key.c_str());
      return -1;
    }
  }

  for (int i = 0; i < DIFI_MAX_CHANNELS; i++) {
    if ((s->rx_ports[i] >= 0 && (s->rx_ports[i] < 1 || s->rx_ports[i] > MAX_UDP_PORT)) ||
        (s->tx_ports[i] >= 0 && (s->tx_ports[i] < 1 || s->tx_ports[i] > MAX_UDP_PORT))) {
      difi_log("channel %d has a port outside [1, %d]", i, MAX_UDP_PORT);
      return -1;
    }
  }
  if (difi_count_ports(s->rx_ports, "rx_port") < 0 || difi_count_ports(s->tx_ports, "tx_port") < 0)
    return -1;

  if (s->bit_depth != 8 && s->bit_depth != 16) {
    difi_log("bit_depth must be 8 or 16, got %d", s->bit_depth);
    return -1;
  }
  if (s->rcvbuf_mb < 1 || s->rcvbuf_mb > MAX_RCVBUF_MB) {
    difi_log("rcvbuf_mb must be in [1, %d], got %d", MAX_RCVBUF_MB, s->rcvbuf_mb);
    return -1;
  }
  if (s->pkt_samples <= 0) {
    difi_log("pkt_samples must be positive");
    return -1;
  }
  if (s->rx_timeout_ms <= 0) {
    difi_log("rx_timeout_ms must be positive, got %d", s->rx_timeout_ms);
    return -1;
  }
  /* A datagram must fit a UDP payload. The transmit path pads the payload to whole 32-bit words, so the bound is the
   * largest whole-word payload. It is divided rather than the size multiplied, so a large pkt_samples cannot
   * overflow. */
  const int bytes_per_sample = (s->bit_depth == 8) ? 2 : 4;
  const int max_payload_bytes =
      (int)(((MAX_UDP_PAYLOAD - DIFI_DATA_HEADER_SIZE) / sizeof(uint32_t)) * sizeof(uint32_t));
  if (s->pkt_samples > max_payload_bytes / bytes_per_sample) {
    difi_log("pkt_samples %d exceeds the maximum UDP payload at %d-bit depth", s->pkt_samples, s->bit_depth);
    return -1;
  }
  return 0;
}

/* ---- Sockets -------------------------------------------------------------- */

static int difi_open_tx_socket(TRXDifiState* s, DifiTxChannel* ch)
{
  ch->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (ch->fd < 0) {
    difi_log("cannot create transmit socket: %s", strerror(errno));
    return -1;
  }
  memset(&ch->dest, 0, sizeof(ch->dest));
  ch->dest.sin_family = AF_INET;
  ch->dest.sin_port   = htons((uint16_t)ch->port);
  if (inet_pton(AF_INET, s->tx_addr.c_str(), &ch->dest.sin_addr) != 1) {
    difi_log("invalid tx_addr '%s'", s->tx_addr.c_str());
    return -1;
  }
  return 0;
}

static int difi_open_rx_socket(TRXDifiState* s, DifiRxChannel* ch)
{
  ch->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (ch->fd < 0) {
    difi_log("cannot create receive socket: %s", strerror(errno));
    return -1;
  }

  /* The receive buffer is the only slack when the application stalls while the peer keeps streaming. The time it
   * covers shrinks as the rate grows: 16 MB is about 350 ms at 11.52 Msps but only 8 ms at 491.52 Msps, so it is
   * configurable with rcvbuf_mb. The kernel caps the request at net.core.rmem_max, so the granted size is logged. */
  int rcvbuf = s->rcvbuf_mb * BYTES_PER_MB;
  if (setsockopt(ch->fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf)) < 0)
    difi_log("warning: cannot set receive buffer size: %s", strerror(errno));
  int       granted     = 0;
  socklen_t granted_len = sizeof(granted);
  if (getsockopt(ch->fd, SOL_SOCKET, SO_RCVBUF, &granted, &granted_len) == 0)
    difi_log("receive buffer: requested %d MB, granted %d MB", s->rcvbuf_mb, granted / BYTES_PER_MB);

  struct sockaddr_in bind_addr;
  memset(&bind_addr, 0, sizeof(bind_addr));
  bind_addr.sin_family = AF_INET;
  bind_addr.sin_port   = htons((uint16_t)ch->port);
  if (inet_pton(AF_INET, s->rx_addr.c_str(), &bind_addr.sin_addr) != 1) {
    difi_log("invalid rx_addr '%s'", s->rx_addr.c_str());
    return -1;
  }
  if (bind(ch->fd, (struct sockaddr*)&bind_addr, sizeof(bind_addr)) < 0) {
    difi_log("cannot bind %s:%d: %s", s->rx_addr.c_str(), ch->port, strerror(errno));
    return -1;
  }
  return 0;
}

static int difi_open_sockets(TRXDifiState* s)
{
  for (DifiTxChannel& ch : s->tx) {
    if (difi_open_tx_socket(s, &ch) < 0)
      return -1;
  }
  for (DifiRxChannel& ch : s->rx) {
    if (difi_open_rx_socket(s, &ch) < 0)
      return -1;
  }
  return 0;
}

static void difi_close_sockets(TRXDifiState* s)
{
  for (DifiTxChannel& ch : s->tx) {
    if (ch.fd >= 0) {
      close(ch.fd);
      ch.fd = -1;
    }
  }
  for (DifiRxChannel& ch : s->rx) {
    if (ch.fd >= 0) {
      close(ch.fd);
      ch.fd = -1;
    }
  }
}

/* ---- Packet construction -------------------------------------------------- */

/* Saturating conversion from the application's float to a 16-bit IQ component. A plain cast of an out-of-range value
 * would wrap instead, turning the loudest samples into large values of the wrong sign. */
static inline int16_t float_to_i16(float v, double scale)
{
  double scaled = (double)v * scale;
  if (scaled > INT16_MAX)
    return INT16_MAX;
  if (scaled < INT16_MIN)
    return INT16_MIN;
  return (int16_t)lrint(scaled);
}

/* Writes and reads one 16-bit value in the given byte order, for the scalar tails of the SIMD loops. */
static inline void pack_u16_order(uint8_t* p, uint16_t v, bool big_endian)
{
  if (big_endian) {
    pack_u16(p, v);
    return;
  }
  p[0] = (uint8_t)v;
  p[1] = (uint8_t)(v >> 8);
}

static inline uint16_t unpack_u16_order(const uint8_t* p, bool big_endian)
{
  return big_endian ? unpack_u16(p) : (uint16_t)(p[0] | ((uint16_t)p[1] << 8));
}

/* Swaps the two bytes of every 16-bit lane. */
static inline __m128i bswap_epi16(__m128i v)
{
  return _mm_or_si128(_mm_slli_epi16(v, 8), _mm_srli_epi16(v, 8));
}

/* Converts interleaved float IQ to 16-bit wire samples in the given byte order, four IQ samples per step.
 *
 * Same result as float_to_i16 per component: the clamp keeps every value inside int32 before
 * _mm_cvtps_epi32, which rounds to nearest-even like lrint, and _mm_packs_epi32 saturates to int16.
 * Scaling is done in float rather than double, which can move a result by one LSB only when the
 * scaled value lands within float precision of a .5 boundary. A per-sample scalar loop does not keep up with
 * 491.52 Msps on one core. */
static void pack_iq16(uint8_t* dst, const float* in, int nof_samples, double scale, bool big_endian)
{
  const __m128 vscale = _mm_set1_ps((float)scale);
  const __m128 vmax   = _mm_set1_ps((float)INT16_MAX);
  const __m128 vmin   = _mm_set1_ps((float)INT16_MIN);
  int          i      = 0;
  for (; i + 4 <= nof_samples; i += 4) {
    __m128 lo            = _mm_mul_ps(_mm_loadu_ps(in + 2 * i), vscale);
    __m128 hi            = _mm_mul_ps(_mm_loadu_ps(in + 2 * i + 4), vscale);
    lo                   = _mm_max_ps(_mm_min_ps(lo, vmax), vmin);
    hi                   = _mm_max_ps(_mm_min_ps(hi, vmax), vmin);
    const __m128i packed = _mm_packs_epi32(_mm_cvtps_epi32(lo), _mm_cvtps_epi32(hi));
    _mm_storeu_si128((__m128i*)(dst + (size_t)i * 4), big_endian ? bswap_epi16(packed) : packed);
  }
  for (; i < nof_samples; i++) {
    pack_u16_order(dst + (size_t)i * 4 + 0, (uint16_t)float_to_i16(in[2 * i + 0], scale), big_endian);
    pack_u16_order(dst + (size_t)i * 4 + 2, (uint16_t)float_to_i16(in[2 * i + 1], scale), big_endian);
  }
}

/* Converts 16-bit wire samples in the given byte order to interleaved float IQ, four IQ samples per step. */
static void unpack_iq16(float* out, const uint8_t* src, int nof_samples, double scale, bool big_endian)
{
  const __m128 vscale = _mm_set1_ps((float)scale);
  int          i      = 0;
  for (; i + 4 <= nof_samples; i += 4) {
    const __m128i raw = _mm_loadu_si128((const __m128i*)(src + (size_t)i * 4));
    const __m128i v   = big_endian ? bswap_epi16(raw) : raw;
    /* Sign-extend each int16 to int32 by placing it in the upper half and shifting back down. */
    const __m128i lo = _mm_srai_epi32(_mm_unpacklo_epi16(v, v), 16);
    const __m128i hi = _mm_srai_epi32(_mm_unpackhi_epi16(v, v), 16);
    _mm_storeu_ps(out + 2 * i, _mm_mul_ps(_mm_cvtepi32_ps(lo), vscale));
    _mm_storeu_ps(out + 2 * i + 4, _mm_mul_ps(_mm_cvtepi32_ps(hi), vscale));
  }
  for (; i < nof_samples; i++) {
    out[2 * i + 0] = (float)((double)(int16_t)unpack_u16_order(src + (size_t)i * 4 + 0, big_endian) * scale);
    out[2 * i + 1] = (float)((double)(int16_t)unpack_u16_order(src + (size_t)i * 4 + 2, big_endian) * scale);
  }
}

static void difi_send_context(TRXDifiState* s, DifiTxChannel* ch, int channel, uint32_t full_secs, uint64_t frac_ps)
{
  uint8_t buf[DIFI_CONTEXT_SIZE];
  memset(buf, 0, sizeof(buf));

  pack_u32(buf + DIFI_OFFSET_HEADER,
           DIFI_CTX_STATIC_BITS | ((uint32_t)(ch->ctx_pkt_n & DIFI_PKT_COUNT_MASK) << DIFI_PKT_COUNT_SHIFT) |
               (uint32_t)(DIFI_CONTEXT_SIZE / sizeof(uint32_t)));
  pack_u32(buf + DIFI_OFFSET_STREAM_ID, s->stream_id + (uint32_t)channel);
  pack_u64(buf + DIFI_OFFSET_CLASS_ID, ((uint64_t)DIFI_OUI << DIFI_OUI_SHIFT) | DIFI_CTX_CLASS_CODE);
  pack_u32(buf + DIFI_OFFSET_INT_TS, full_secs);
  pack_u64(buf + DIFI_OFFSET_FRAC_TS, frac_ps);

  /* CIF0 declares which of the following fields are present; a receiver ignores any field it does not declare. */
  pack_u32(buf + DIFI_CTX_OFFSET_CIF0, DIFI_CTX_CIF0);
  pack_u32(buf + DIFI_CTX_OFFSET_REF_POINT, DIFI_REF_POINT);
  /* Bandwidth is reported equal to the sample rate; the IF fields are zero because the stream is at baseband. */
  pack_u64(buf + DIFI_CTX_OFFSET_BANDWIDTH, (uint64_t)((double)s->sample_rate * DIFI_FREQ_RADIX));
  pack_u64(buf + DIFI_CTX_OFFSET_IF_REF_FREQ, 0);
  pack_u64(buf + DIFI_CTX_OFFSET_RF_REF_FREQ, (uint64_t)((int64_t)((double)ch->freq * DIFI_FREQ_RADIX)));
  pack_u64(buf + DIFI_CTX_OFFSET_IF_BAND_OFFSET, 0);
  pack_u32(buf + DIFI_CTX_OFFSET_REF_LEVEL, 0);
  /* Gain word: two 16-bit fields, IF stage in the upper half and RF stage in the lower half. */
  pack_u32(buf + DIFI_CTX_OFFSET_GAIN, (uint32_t)((int16_t)lrint(ch->gain * DIFI_GAIN_RADIX)) & DIFI_GAIN_MASK);
  pack_u64(buf + DIFI_CTX_OFFSET_SAMPLE_RATE, (uint64_t)((double)s->sample_rate * DIFI_FREQ_RADIX));
  pack_u64(buf + DIFI_CTX_OFFSET_TS_ADJ, DIFI_TIMESTAMP_ADJ_FS);
  pack_u32(buf + DIFI_CTX_OFFSET_TS_CAL_TIME, full_secs);
  pack_u32(buf + DIFI_CTX_OFFSET_STATE_EVENTS, DIFI_STATE_AND_EVENTS);
  pack_u64(buf + DIFI_CTX_OFFSET_PAYLOAD_FMT, s->bit_depth == 8 ? DIFI_PAYLOAD_FMT_8BIT : DIFI_PAYLOAD_FMT_16BIT);

  if (sendto(ch->fd, buf, sizeof(buf), 0, (struct sockaddr*)&ch->dest, sizeof(ch->dest)) < 0) {
    /* Not fatal: a lost context packet costs the receiver metadata it will get again. */
    if (errno != ECONNREFUSED)
      difi_log("context packet send failed: %s", strerror(errno));
  }
  ch->ctx_pkt_n = (uint8_t)((ch->ctx_pkt_n + 1) % DIFI_PKT_MOD);
}

/* ---- TRX callbacks -------------------------------------------------------- */

/* Sends count samples of one channel, or silence when in is NULL, as that channel's DIFI stream.
 * Returns false if a send failed, in which case the rest of the buffer is dropped. */
static bool difi_send_channel(TRXDifiState* s, int channel, trx_timestamp_t timestamp, const float* in, int count)
{
  DifiTxChannel* ch               = &s->tx[(size_t)channel];
  const bool     padding          = (in == NULL);
  const int      bytes_per_sample = (s->bit_depth == 8) ? 2 : 4;

  /* tx_pkt is sized for a full pkt_samples packet in trx_difi_start, so this path never allocates. */
  for (int offset = 0; offset < count;) {
    const int    chunk         = (count - offset < s->pkt_samples) ? (count - offset) : s->pkt_samples;
    const size_t payload_bytes = (size_t)chunk * bytes_per_sample;
    /* The header counts the packet in 32-bit words. An odd number of 8-bit samples leaves half a word, which is
     * zero-padded as the gNB does, so the declared size matches the bytes sent. */
    const size_t padded_bytes = (payload_bytes + sizeof(uint32_t) - 1) & ~(sizeof(uint32_t) - 1);
    const size_t total        = DIFI_DATA_HEADER_SIZE + padded_bytes;
    uint8_t*     buf          = s->tx_pkt.data();

    /* Each fragment carries the absolute time of its own first sample, so a receiver can place it without
     * reference to the fragment that preceded it.
     *
     * The timestamp is translated back into the peer's epoch, undoing the offset latched when the peer's first
     * packet arrived. The peer derives the slot of a received buffer from its timestamp, so the uplink must be
     * stamped on the peer's own timeline for it to land in the slot it was transmitted for. */
    uint32_t      full_secs   = 0;
    uint64_t      frac_ps     = 0;
    const int64_t local_tx_ts = (int64_t)timestamp + (int64_t)offset;
    const int64_t peer_tx_ts  = s->ts_offset_valid ? (local_tx_ts - s->ts_offset) : local_tx_ts;
    difi_ticks_to_time((peer_tx_ts > 0) ? (uint64_t)peer_tx_ts : 0, s->sample_rate, &full_secs, &frac_ps);

    if (ch->pkts_since_ctx >= s->ctx_interval) {
      difi_send_context(s, ch, channel, full_secs, frac_ps);
      ch->pkts_since_ctx = 0;
    }

    memset(buf, 0, DIFI_DATA_HEADER_SIZE);
    pack_u32(buf + DIFI_OFFSET_HEADER,
             DIFI_DATA_STATIC_BITS | ((uint32_t)(ch->data_pkt_n & DIFI_PKT_COUNT_MASK) << DIFI_PKT_COUNT_SHIFT) |
                 (uint32_t)(total / sizeof(uint32_t)));
    pack_u32(buf + DIFI_OFFSET_STREAM_ID, s->stream_id + (uint32_t)channel);
    /* The pad bit count tells the receiver that the padding is not a sample, so its timeline stays aligned. */
    const uint64_t pad_bits = (uint64_t)(padded_bytes - payload_bytes) * BITS_PER_BYTE;
    pack_u64(buf + DIFI_OFFSET_CLASS_ID, ((uint64_t)DIFI_OUI << DIFI_OUI_SHIFT) | (pad_bits << DIFI_PAD_BITS_SHIFT));
    pack_u32(buf + DIFI_OFFSET_INT_TS, full_secs);
    pack_u64(buf + DIFI_OFFSET_FRAC_TS, frac_ps);

    /* Payload: interleaved I then Q. Silence is all-zero bytes at either depth, so it is written directly rather
     * than converted sample by sample. */
    uint8_t* payload = buf + DIFI_DATA_HEADER_SIZE;
    memset(payload + payload_bytes, 0, padded_bytes - payload_bytes);
    if (padding) {
      memset(payload, 0, payload_bytes);
    } else if (s->bit_depth == 16) {
      pack_iq16(payload, in + 2 * offset, chunk, s->tx_scale, s->iq_big_endian);
    } else {
      for (int i = 0; i < chunk; i++) {
        const int16_t re           = float_to_i16(in[2 * (offset + i) + 0], s->tx_scale);
        const int16_t im           = float_to_i16(in[2 * (offset + i) + 1], s->tx_scale);
        payload[(size_t)i * 2 + 0] = (uint8_t)(int8_t)(re >> 8);
        payload[(size_t)i * 2 + 1] = (uint8_t)(int8_t)(im >> 8);
      }
    }

    if (sendto(ch->fd, buf, total, 0, (struct sockaddr*)&ch->dest, sizeof(ch->dest)) < 0) {
      /* ECONNREFUSED just means nothing is listening yet; it is normal before the peer starts
       * and would otherwise flood the log on the baseband path. */
      if (errno != ECONNREFUSED)
        difi_log("data packet send failed: %s", strerror(errno));
      ch->send_failures++;
      return false;
    }

    ch->data_pkt_n = (uint8_t)((ch->data_pkt_n + 1) % DIFI_PKT_MOD);
    ch->pkts_since_ctx++;
    offset += chunk;
  }
  return true;
}

static void trx_difi_write2(TRXState*         s1,
                            trx_timestamp_t   timestamp,
                            const void**      samples,
                            int               count,
                            int               tx_port_index,
                            TRXWriteMetadata* md)
{
  TRXDifiState* s = (TRXDifiState*)s1->opaque;
  (void)tx_port_index;

  if (count <= 0 || !s->started)
    return;

  /* Padding means the application knows there is no signal here and has passed no buffer. The span is still
   * transmitted, as silence, so the stream stays continuous. */
  const bool padding = (samples == NULL) || (md && (md->flags & TRX_WRITE_MD_FLAG_PADDING));

  if (s->timing_log) {
    const int64_t now_ns = monotonic_ns();
    if (s->last_write_ns != 0 && now_ns - s->last_write_ns > s->max_write_gap_ns)
      s->max_write_gap_ns = now_ns - s->last_write_ns;
    s->last_write_ns      = now_ns;
    const int64_t advance = (int64_t)timestamp - (int64_t)s->next_rx_ts;
    if (!s->have_tx_advance || advance < s->min_tx_advance)
      s->min_tx_advance = advance;
    s->have_tx_advance = true;
  }

  /* Every channel is stamped with the same timestamps, so the peer can line the antennas up. */
  for (size_t channel = 0; channel < s->tx.size(); channel++) {
    const float* in = padding ? NULL : (const float*)samples[channel];
    if (!difi_send_channel(s, (int)channel, timestamp, in, count) && md)
      md->underflow = 1;
  }
}

/* Unpacks nof_samples wire samples into interleaved float IQ. */
static void difi_unpack(TRXDifiState* s, float* out, const uint8_t* payload, int nof_samples)
{
  if (s->bit_depth == 16) {
    unpack_iq16(out, payload, nof_samples, s->rx_scale, s->iq_big_endian);
    return;
  }
  for (int i = 0; i < nof_samples; i++) {
    const int16_t re = (int16_t)((int16_t)(int8_t)payload[(size_t)i * 2 + 0] << 8);
    const int16_t im = (int16_t)((int16_t)(int8_t)payload[(size_t)i * 2 + 1] << 8);
    out[2 * i + 0]   = (float)((double)re * s->rx_scale);
    out[2 * i + 1]   = (float)((double)im * s->rx_scale);
  }
}

/* Places a block of samples starting at local_ts into the window [start_ts, start_ts + count) of a
 * channel other than 0, *pos samples of which are written. A gap ahead of the block is zero-filled;
 * samples already behind *pos are dropped; whatever runs past the window is kept in the channel carry.
 * src is wire samples when from_wire, otherwise interleaved float IQ (a previous carry). */
static void difi_place_block(TRXDifiState*  s,
                             DifiRxChannel* ch,
                             float*         out,
                             uint64_t       start_ts,
                             int            count,
                             int*           pos,
                             int64_t        local_ts,
                             const void*    src,
                             int            nof_block,
                             bool           from_wire)
{
  const int     bytes_per_sample = (s->bit_depth == 8) ? 2 : 4;
  const int64_t expected         = (int64_t)start_ts + *pos;

  int skip = 0;
  if (local_ts < expected) {
    const int64_t stale = expected - local_ts;
    if (stale >= nof_block) {
      ch->stale++;
      if (!from_wire)
        ch->carry.clear();
      return;
    }
    skip = (int)stale;
  } else if (local_ts > expected) {
    /* Datagrams were lost. Zero-fill the hole so what did arrive stays where it belongs. */
    const int64_t gap     = local_ts - expected;
    const int     nof_gap = (gap < count - *pos) ? (int)gap : count - *pos;
    memset(out + 2 * *pos, 0, (size_t)nof_gap * 2 * sizeof(float));
    *pos += nof_gap;
    ch->gaps++;
  }

  const int nof_avail = nof_block - skip;
  const int nof_write = (nof_avail < count - *pos) ? nof_avail : count - *pos;
  if (from_wire) {
    const uint8_t* payload = (const uint8_t*)src + (size_t)skip * bytes_per_sample;
    difi_unpack(s, out + 2 * *pos, payload, nof_write);
  } else {
    memcpy(out + 2 * *pos, (const float*)src + 2 * skip, (size_t)nof_write * 2 * sizeof(float));
  }
  *pos += nof_write;

  const int nof_tail = nof_avail - nof_write;
  if (nof_tail <= 0) {
    if (!from_wire)
      ch->carry.clear();
    return;
  }
  /* Hold the remainder for the next call, see DifiRxChannel::carry. */
  const uint64_t tail_ts = (uint64_t)(local_ts + skip + nof_write);
  if (from_wire) {
    ch->carry.resize((size_t)nof_tail * 2);
    difi_unpack(s, ch->carry.data(), (const uint8_t*)src + (size_t)(skip + nof_write) * bytes_per_sample, nof_tail);
  } else {
    ch->carry.erase(ch->carry.begin(), ch->carry.begin() + (size_t)(skip + nof_write) * 2);
  }
  ch->carry_ts = tail_ts;
}

/* Number of samples a data packet carries: its payload, less the padding its class ID declares, in whole samples. */
static int difi_nof_samples(const uint8_t* buf, size_t len, int bytes_per_sample)
{
  const uint64_t pad_bits     = (unpack_u64(buf + DIFI_OFFSET_CLASS_ID) >> DIFI_PAD_BITS_SHIFT) & DIFI_PAD_BITS_MASK;
  const uint64_t payload_bits = (uint64_t)(len - DIFI_DATA_HEADER_SIZE) * BITS_PER_BYTE;
  if (pad_bits >= payload_bits)
    return 0;
  return (int)((payload_bits - pad_bits) / ((uint64_t)bytes_per_sample * BITS_PER_BYTE));
}

/* Fills channel `channel` (not 0) for the window [start_ts, start_ts + count) that channel 0 settled on.
 * Waits briefly for data that trails channel 0, unless the channel has never received anything, and
 * zero-fills whatever is still missing so every channel returns the same span. */
static void difi_fill_channel(TRXDifiState* s, int channel, float* out, uint64_t start_ts, int count)
{
  DifiRxChannel* ch               = &s->rx[(size_t)channel];
  const int      bytes_per_sample = (s->bit_depth == 8) ? 2 : 4;
  const uint32_t stream_id        = s->stream_id + (uint32_t)channel;
  int            pos              = 0;

  if (!ch->carry.empty()) {
    const int nof_carry = (int)(ch->carry.size() / 2);
    difi_place_block(s, ch, out, start_ts, count, &pos, (int64_t)ch->carry_ts, ch->carry.data(), nof_carry, false);
  }

  /* One deadline bounds the whole fill, including datagrams that are rejected, so stray traffic cannot hold the read.
   * Draining what is already queued takes microseconds, well inside it. */
  const int64_t deadline_ns = monotonic_ns() + DIFI_CHANNEL_WAIT_MS * NANO_PER_MILLI;
  while (pos < count) {
    const int timeout_ms = difi_poll_timeout_ms(deadline_ns);
    if (timeout_ms == 0)
      break;
    struct pollfd pfd;
    pfd.fd       = ch->fd;
    pfd.events   = POLLIN;
    const int pr = poll(&pfd, 1, ch->waiting ? timeout_ms : 0);
    if (pr < 0 && errno == EINTR)
      continue;
    if (pr == 0 && ch->waiting)
      ch->waiting = false;
    if (pr <= 0)
      break;

    const ssize_t n = recv(ch->fd, s->rx_buf.data(), s->rx_buf.size(), 0);
    if (n <= 0)
      break;
    const size_t len = (size_t)n;
    if (len < DIFI_DATA_HEADER_SIZE)
      continue;

    const uint8_t* buf = s->rx_buf.data();
    if (((unpack_u32(buf + DIFI_OFFSET_HEADER) >> DIFI_PKT_TYPE_SHIFT) & DIFI_PKT_TYPE_MASK) != DIFI_PKT_TYPE_DATA)
      continue;
    if (unpack_u32(buf + DIFI_OFFSET_STREAM_ID) != stream_id)
      continue;
    const int nof_pkt = difi_nof_samples(buf, len, bytes_per_sample);
    if (nof_pkt <= 0)
      continue;
    ch->active  = true;
    ch->waiting = true;

    /* Channel 0 latched the peer's epoch, and the peer stamps every channel from the same clock. */
    const uint64_t pkt_ts =
        difi_time_to_ticks(unpack_u32(buf + DIFI_OFFSET_INT_TS), unpack_u64(buf + DIFI_OFFSET_FRAC_TS), s->sample_rate);
    const int64_t local_ts = (int64_t)pkt_ts + s->ts_offset;
    difi_place_block(s, ch, out, start_ts, count, &pos, local_ts, buf + DIFI_DATA_HEADER_SIZE, nof_pkt, true);
  }

  if (pos < count) {
    memset(out + 2 * pos, 0, (size_t)(count - pos) * 2 * sizeof(float));
    /* A channel without a transmitter is silence, not a stream with holes in it. */
    if (ch->active)
      ch->gaps++;
  }
}

static int trx_difi_read2(TRXState*        s1,
                          trx_timestamp_t* ptimestamp,
                          void**           samples,
                          int              count,
                          int              rx_port_index,
                          TRXReadMetadata* md)
{
  TRXDifiState* s = (TRXDifiState*)s1->opaque;
  (void)rx_port_index;

  if (md) {
    md->flags      = 0;
    md->overflow   = 0;
    md->agc_change = 0;
  }
  if (count <= 0 || !s->started)
    return 0;

  DifiRxChannel* ch0 = &s->rx[0];
  float*         out = (float*)samples[0];
  if (s->timing_log) {
    const int64_t now_ns = monotonic_ns();
    if (s->last_read_ns != 0 && now_ns - s->last_read_ns > s->max_read_gap_ns)
      s->max_read_gap_ns = now_ns - s->last_read_ns;
    s->last_read_ns = now_ns;
  }
  const int bytes_per_sample = (s->bit_depth == 8) ? 2 : 4;
  int       filled           = 0;
  bool      have_start       = false;
  uint64_t  start_ts         = 0;

  /* Continue with the tail of the datagram that overran the previous call, before touching the
   * socket, so the stream stays contiguous across the buffer boundary. */
  if (!ch0->carry.empty()) {
    const int nof_carry = (int)(ch0->carry.size() / 2);
    const int nof_write = (nof_carry < count) ? nof_carry : count;
    memcpy(out, ch0->carry.data(), (size_t)nof_write * 2 * sizeof(float));
    /* The carry can hold a whole datagram that did not follow on from the previous read, see below. */
    if (ch0->carry_ts != s->next_rx_ts) {
      if (md) {
        md->flags |= TRX_READ_MD_OVERFLOW;
        md->overflow = 1;
      }
      s->rx_overflows++;
    }
    start_ts   = ch0->carry_ts;
    have_start = true;
    filled     = nof_write;
    if (nof_write < nof_carry) {
      ch0->carry.erase(ch0->carry.begin(), ch0->carry.begin() + (size_t)nof_write * 2);
      ch0->carry_ts += (uint64_t)nof_write;
    } else {
      ch0->carry.clear();
    }
  }

  /* Sample-locked: block for the peer's data rather than following a local clock. The API allows a short return, so
   * the buffer need not be filled, and a gap shows as a jump in the timestamp returned by the next call rather than
   * as zeros.
   *
   * One deadline bounds the whole read, including datagrams that are rejected, so stray traffic cannot hold it past
   * rx_timeout_ms. */
  const int64_t deadline_ns = monotonic_ns() + (int64_t)s->rx_timeout_ms * NANO_PER_MILLI;
  while (filled < count) {
    const int remaining_ms = difi_poll_timeout_ms(deadline_ns);
    if (remaining_ms == 0)
      break;
    struct pollfd pfd;
    pfd.fd     = ch0->fd;
    pfd.events = POLLIN;
    /* Once samples are in hand, only drain what is already queued: waiting longer would add
     * latency for no benefit, since the caller can simply ask again. */
    const int     timeout       = (filled == 0) ? remaining_ms : 0;
    const int64_t wait_start_ns = (s->timing_log && filled == 0) ? monotonic_ns() : 0;
    const int     pr            = poll(&pfd, 1, timeout);
    if (wait_start_ns != 0) {
      const int64_t now_ns = monotonic_ns();
      const int64_t waited = now_ns - wait_start_ns;
      if (waited > s->max_read_wait_ns)
        s->max_read_wait_ns = waited;
      if (waited > DIFI_LONG_READ_WAIT_NS)
        s->nof_long_read_waits++;
      difi_report_timing(s, now_ns);
    }
    if (pr < 0) {
      if (errno == EINTR)
        continue;
      difi_log("poll failed: %s", strerror(errno));
      break;
    }
    if (pr == 0)
      break;

    const ssize_t n = recv(ch0->fd, s->rx_buf.data(), s->rx_buf.size(), 0);
    if (n <= 0)
      break;
    const size_t len = (size_t)n;
    if (len < DIFI_DATA_HEADER_SIZE)
      continue;

    const uint8_t* buf    = s->rx_buf.data();
    const uint32_t header = unpack_u32(buf + DIFI_OFFSET_HEADER);
    /* Context and unknown packet types carry no samples. */
    if (((header >> DIFI_PKT_TYPE_SHIFT) & DIFI_PKT_TYPE_MASK) != DIFI_PKT_TYPE_DATA)
      continue;
    if (unpack_u32(buf + DIFI_OFFSET_STREAM_ID) != s->stream_id)
      continue;
    ch0->active = true;

    const uint64_t pkt_ts =
        difi_time_to_ticks(unpack_u32(buf + DIFI_OFFSET_INT_TS), unpack_u64(buf + DIFI_OFFSET_FRAC_TS), s->sample_rate);
    const int nof_pkt = difi_nof_samples(buf, len, bytes_per_sample);
    if (nof_pkt <= 0)
      continue;

    /* Lock onto whatever epoch the peer uses by mapping its first packet to where this stream's
     * timeline currently sits. Without this the two sides differ by an arbitrary offset and every
     * packet looks misplaced. */
    if (!s->ts_offset_valid) {
      s->ts_offset       = (int64_t)s->next_rx_ts - (int64_t)pkt_ts;
      s->ts_offset_valid = true;
    }
    const int64_t local_ts = (int64_t)pkt_ts + s->ts_offset;

    if (!have_start) {
      /* First packet of this call sets the timestamp we report. A jump relative to what the
       * previous call ended on is a genuine discontinuity: report it and let the application
       * resynchronise, exactly as it would after a hardware overflow. */
      if (local_ts != (int64_t)s->next_rx_ts) {
        if (md) {
          md->flags |= TRX_READ_MD_OVERFLOW;
          md->overflow = 1;
        }
        s->rx_overflows++;
      }
      start_ts   = (uint64_t)local_ts;
      have_start = true;
    } else if (local_ts != (int64_t)(start_ts + (uint64_t)filled)) {
      /* Non-contiguous with what is already in the buffer. Stop here and return. A packet ahead of the buffer is kept
       * as the carry, so the next call starts cleanly at it rather than us inventing filler; one behind it is a
       * duplicate or late datagram and is dropped. */
      s->rx_non_contiguous++;
      if (local_ts > (int64_t)(start_ts + (uint64_t)filled)) {
        ch0->carry.resize((size_t)nof_pkt * 2);
        difi_unpack(s, ch0->carry.data(), buf + DIFI_DATA_HEADER_SIZE, nof_pkt);
        ch0->carry_ts = (uint64_t)local_ts;
      }
      break;
    }

    const int      nof_write = (nof_pkt < count - filled) ? nof_pkt : count - filled;
    const uint8_t* payload   = buf + DIFI_DATA_HEADER_SIZE;
    difi_unpack(s, out + 2 * filled, payload, nof_write);
    filled += nof_write;

    if (nof_write < nof_pkt) {
      /* Hold the remainder for the next call, see DifiRxChannel::carry. */
      const int nof_tail = nof_pkt - nof_write;
      ch0->carry.resize((size_t)nof_tail * 2);
      difi_unpack(s, ch0->carry.data(), payload + (size_t)nof_write * bytes_per_sample, nof_tail);
      ch0->carry_ts = (uint64_t)local_ts + (uint64_t)nof_write;
      break;
    }
  }

  if (!have_start) {
    /* Nothing arrived within the timeout. Report no samples and let the application decide whether that is a
     * stall. */
    *ptimestamp = (trx_timestamp_t)s->next_rx_ts;
    return 0;
  }

  s->next_rx_ts = start_ts + (uint64_t)filled;
  *ptimestamp   = (trx_timestamp_t)start_ts;

  /* Every other channel covers the span channel 0 settled on, so the application gets all antennas for
   * one stretch of time. */
  for (size_t channel = 1; channel < s->rx.size(); channel++)
    difi_fill_channel(s, (int)channel, (float*)samples[channel], start_ts, filled);
  return filled;
}

static int trx_difi_start(TRXState* s1, const TRXDriverParams2* p)
{
  TRXDifiState* s = (TRXDifiState*)s1->opaque;

  if (p->rf_port_count != 1) {
    difi_log("exactly one RF port is supported, got %d", p->rf_port_count);
    return -1;
  }

  /* Every channel needs a port of its own, and a port with no channel is a configuration mistake. */
  const int nof_rx_ports = difi_count_ports(s->rx_ports, "rx_port");
  const int nof_tx_ports = difi_count_ports(s->tx_ports, "tx_port");
  if (p->rx_channel_count != nof_rx_ports || p->tx_channel_count != nof_tx_ports) {
    difi_log("the cell has %d rx and %d tx channels, but %d rx_port and %d tx_port are configured",
             p->rx_channel_count,
             p->tx_channel_count,
             nof_rx_ports,
             nof_tx_ports);
    return -1;
  }

  const TRXFraction rate = p->sample_rate[0];
  if (rate.den <= 0) {
    difi_log("invalid sample rate denominator %d", rate.den);
    return -1;
  }
  s->sample_rate = (uint64_t)(rate.num / rate.den);
  if (s->sample_rate == 0) {
    difi_log("sample rate resolved to zero");
    return -1;
  }

  s->tx.assign((size_t)p->tx_channel_count, DifiTxChannel());
  for (int i = 0; i < p->tx_channel_count; i++) {
    DifiTxChannel& ch = s->tx[(size_t)i];
    ch.port           = s->tx_ports[i];
    ch.fd             = -1;
    ch.gain           = p->tx_gain ? p->tx_gain[i] : 0.0;
    ch.freq           = p->tx_freq ? p->tx_freq[i] : 0;
    ch.data_pkt_n     = 0;
    ch.ctx_pkt_n      = 0;
    /* Start due, so a context packet leads the stream and the receiver has metadata from the outset. */
    ch.pkts_since_ctx = s->ctx_interval;
    ch.send_failures  = 0;
  }

  /* Size every buffer the I/O path uses here, once, so trx_difi_write2 and trx_difi_read2 never allocate:
   * a full transmit packet, and a carried receive tail per channel, which is never longer than one
   * datagram (the most samples fit at 8-bit depth, 2 bytes each). */
  s->rx.assign((size_t)p->rx_channel_count, DifiRxChannel());
  for (int i = 0; i < p->rx_channel_count; i++) {
    DifiRxChannel& ch = s->rx[(size_t)i];
    ch.port           = s->rx_ports[i];
    ch.fd             = -1;
    ch.gain           = p->rx_gain ? p->rx_gain[i] : 0.0;
    ch.carry.reserve((size_t)2 * ((MAX_UDP_PAYLOAD - DIFI_DATA_HEADER_SIZE) / 2));
    ch.carry_ts = 0;
    ch.active   = false;
    ch.waiting  = false;
    ch.gaps     = 0;
    ch.stale    = 0;
  }

  s->rx_buf.assign(MAX_UDP_PAYLOAD, 0);
  s->tx_pkt.assign(DIFI_DATA_HEADER_SIZE + (size_t)s->pkt_samples * 4, 0);
  s->ts_offset_valid   = false;
  s->ts_offset         = 0;
  s->next_rx_ts        = 0;
  s->rx_overflows      = 0;
  s->rx_non_contiguous = 0;

  if (difi_open_sockets(s) < 0) {
    difi_close_sockets(s);
    return -1;
  }

  s->started = true;
  for (size_t i = 0; i < s->rx.size(); i++)
    difi_log("rx channel %zu: %s:%d, stream %u", i, s->rx_addr.c_str(), s->rx[i].port, s->stream_id + (uint32_t)i);
  for (size_t i = 0; i < s->tx.size(); i++)
    difi_log("tx channel %zu: %s:%d, stream %u", i, s->tx_addr.c_str(), s->tx[i].port, s->stream_id + (uint32_t)i);
  difi_log("started: %.2f Msps, %zu rx / %zu tx channels, %d-bit %s-endian IQ, %d samples/packet",
           (double)s->sample_rate / HZ_PER_MHZ,
           s->rx.size(),
           s->tx.size(),
           s->bit_depth,
           s->iq_big_endian ? "big" : "little",
           s->pkt_samples);
  difi_log("scaling: tx float x %.1f -> int16 (saturating), rx int16 x %g -> float", s->tx_scale, s->rx_scale);
  return 0;
}

static uint64_t difi_tx_failures(const TRXDifiState* s)
{
  uint64_t total = 0;
  for (const DifiTxChannel& ch : s->tx)
    total += ch.send_failures;
  return total;
}

static void trx_difi_stop(TRXState* s1)
{
  TRXDifiState* s = (TRXDifiState*)s1->opaque;
  if (!s->started)
    return;
  difi_close_sockets(s);
  s->started = false;
  difi_log("stopped: rx discontinuities %" PRIu64 ", non-contiguous datagrams %" PRIu64 ", tx send failures %" PRIu64,
           s->rx_overflows,
           s->rx_non_contiguous,
           difi_tx_failures(s));
  for (size_t i = 1; i < s->rx.size(); i++)
    difi_log("rx channel %zu: gaps %" PRIu64 ", stale datagrams %" PRIu64 "%s",
             i,
             s->rx[i].gaps,
             s->rx[i].stale,
             s->rx[i].active ? "" : ", never received");
}

static void trx_difi_end(TRXState* s1)
{
  TRXDifiState* s = (TRXDifiState*)s1->opaque;
  if (!s)
    return;
  difi_close_sockets(s);
  delete s;
  s1->opaque = NULL;
}

static void trx_difi_set_tx_gain(TRXState* s1, double gain, int channel_num)
{
  TRXDifiState* s = (TRXDifiState*)s1->opaque;
  if (channel_num < 0 || (size_t)channel_num >= s->tx.size())
    return;
  s->tx[(size_t)channel_num].gain = gain;
  /* Gain is metadata on a digital IF link, not something applied to the samples: scaling them here
   * would silently change the numbers the peer receives. It is advertised in the next context
   * packet, which is where a DIFI receiver looks for it. */
  s->tx[(size_t)channel_num].pkts_since_ctx = s->ctx_interval;
}

static void trx_difi_set_rx_gain(TRXState* s1, double gain, int channel_num)
{
  TRXDifiState* s = (TRXDifiState*)s1->opaque;
  if (channel_num < 0 || (size_t)channel_num >= s->rx.size())
    return;
  s->rx[(size_t)channel_num].gain = gain;
}

static void trx_difi_get_tx_gain(TRXState* s1, double* gain, int channel_num)
{
  TRXDifiState* s = (TRXDifiState*)s1->opaque;
  *gain           = (channel_num >= 0 && (size_t)channel_num < s->tx.size()) ? s->tx[(size_t)channel_num].gain : 0.0;
}

static void trx_difi_get_rx_gain(TRXState* s1, double* gain, int channel_num)
{
  TRXDifiState* s = (TRXDifiState*)s1->opaque;
  *gain           = (channel_num >= 0 && (size_t)channel_num < s->rx.size()) ? s->rx[(size_t)channel_num].gain : 0.0;
}

/* Whether n is of the form 2^a * 3^b * 5^c, as the application's interpolator requires. */
static bool is_allowed_rate_num(int n)
{
  if (n <= 0)
    return false;
  while ((n % 2) == 0)
    n /= 2;
  while ((n % 3) == 0)
    n /= 3;
  while ((n % 5) == 0)
    n /= 5;
  return n == 1;
}

static int trx_difi_get_sample_rate(TRXState* s1, TRXFraction* psample_rate, int* psample_rate_num, int sample_rate_min)
{
  TRXDifiState* s = (TRXDifiState*)s1->opaque;

  /* An explicit srate pins the rate, which matters when the peer's rate is fixed by its own
   * configuration and the two must agree exactly. */
  if (s->forced_srate > 0) {
    const int n = (int)(s->forced_srate / BASE_SAMPLE_RATE_HZ);
    if ((int64_t)n * BASE_SAMPLE_RATE_HZ != s->forced_srate || !is_allowed_rate_num(n)) {
      difi_log("srate %" PRId64 " is not n x 1.92 MHz with n = 2^a*3^b*5^c", s->forced_srate);
      return -1;
    }
    if (s->forced_srate < sample_rate_min) {
      difi_log(
          "srate %" PRId64 " is below the %d required by the configured bandwidth", s->forced_srate, sample_rate_min);
      return -1;
    }
    *psample_rate_num = n;
    psample_rate->num = (int)s->forced_srate;
    psample_rate->den = 1;
    return 0;
  }

  /* Otherwise pick the smallest allowed rate that covers the requested bandwidth with margin. */
  const int needed = (int)lround((double)sample_rate_min * SAMPLE_RATE_MARGIN);
  for (int n = 1; n <= MAX_SAMPLE_RATE_MULTIPLIER; n++) {
    if (!is_allowed_rate_num(n))
      continue;
    const int rate = n * BASE_SAMPLE_RATE_HZ;
    if (rate >= needed) {
      *psample_rate_num = n;
      psample_rate->num = rate;
      psample_rate->den = 1;
      return 0;
    }
  }
  return -1;
}

static int trx_difi_get_tx_samples_per_packet(TRXState* s1)
{
  TRXDifiState* s = (TRXDifiState*)s1->opaque;
  return s->pkt_samples;
}

static int trx_difi_get_stats(TRXState* s1, TRXStatistics* m)
{
  TRXDifiState* s = (TRXDifiState*)s1->opaque;
  memset(m, 0, sizeof(*m));
  m->rx_overflow_count  = s->rx_overflows;
  m->tx_underflow_count = difi_tx_failures(s);
  return 0;
}

static void trx_difi_dump_info(TRXState* s1, trx_printf_cb cb, void* opaque)
{
  TRXDifiState* s = (TRXDifiState*)s1->opaque;
  cb(opaque,
     "DIFI: %.2f Msps, %zu rx / %zu tx channels from stream %u, %d-bit\n",
     (double)s->sample_rate / HZ_PER_MHZ,
     s->rx.size(),
     s->tx.size(),
     s->stream_id,
     s->bit_depth);
  for (size_t i = 0; i < s->rx.size(); i++)
    cb(opaque, "DIFI: rx channel %zu on %s:%d\n", i, s->rx_addr.c_str(), s->rx[i].port);
  for (size_t i = 0; i < s->tx.size(); i++)
    cb(opaque, "DIFI: tx channel %zu to %s:%d\n", i, s->tx_addr.c_str(), s->tx[i].port);
  cb(opaque,
     "DIFI: rx discontinuities=%" PRIu64 " non-contiguous=%" PRIu64 " tx failures=%" PRIu64 "\n",
     s->rx_overflows,
     s->rx_non_contiguous,
     difi_tx_failures(s));
}

/* ---- Entry point ---------------------------------------------------------- */

extern "C" int trx_driver_init(TRXState* s1)
{
  if (s1->trx_api_version != TRX_API_VERSION) {
    fprintf(stderr,
            "ABI mismatch between the application and the DIFI TRX driver "
            "(application=%d, driver=%d)\n",
            s1->trx_api_version,
            TRX_API_VERSION);
    return -1;
  }

  TRXDifiState* s = new TRXDifiState();
  for (int i = 0; i < DIFI_MAX_CHANNELS; i++) {
    s->rx_ports[i] = -1;
    s->tx_ports[i] = -1;
  }
  s->rx_ports[0] = DEFAULT_RX_PORT;
  s->tx_ports[0] = DEFAULT_TX_PORT;

  char*     args = trx_get_param_string(s1, "args");
  const int rc   = difi_parse_args(s, args ? args : "");
  free(args);
  if (rc < 0) {
    delete s;
    return -1;
  }

  s1->opaque                             = s;
  s1->trx_end_func                       = trx_difi_end;
  s1->trx_write_func2                    = trx_difi_write2;
  s1->trx_read_func2                     = trx_difi_read2;
  s1->trx_start_func2                    = trx_difi_start;
  s1->trx_set_tx_gain_func               = trx_difi_set_tx_gain;
  s1->trx_set_rx_gain_func               = trx_difi_set_rx_gain;
  s1->trx_get_sample_rate_func           = trx_difi_get_sample_rate;
  s1->trx_get_tx_samples_per_packet_func = trx_difi_get_tx_samples_per_packet;
  s1->trx_get_stats                      = trx_difi_get_stats;
  s1->trx_dump_info                      = trx_difi_dump_info;

  s1->trx_get_tx_gain_func = trx_difi_get_tx_gain;
  s1->trx_get_rx_gain_func = trx_difi_get_rx_gain;
  s1->trx_stop_func        = trx_difi_stop;

  return 0;
}
