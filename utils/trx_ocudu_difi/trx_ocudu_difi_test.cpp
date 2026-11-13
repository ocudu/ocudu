// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <iostream>
#include <set>
#include <string>
#include <vector>

extern "C" {
#include <trx_driver.h>
}

#define TESTASSERT(COND)                                                                                               \
  do {                                                                                                                 \
    if (!(COND)) {                                                                                                     \
      std::cerr << "Error in " << __FILE__ << ":" << __LINE__ << ":" << ": unmet condition '" << #COND << "'"          \
                << std::endl;                                                                                          \
      std::abort();                                                                                                    \
    }                                                                                                                  \
  } while (false)

namespace {

using driver_init_func = int (*)(TRXState*);

/// Application side of one driver instance: the arguments it is given and the parameters it is asked for.
struct test_app {
  std::string           args;
  std::set<std::string> param_names;
};

char* get_param_string(void* app_opaque, const char* prop_name)
{
  auto* app = static_cast<test_app*>(app_opaque);
  app->param_names.insert(prop_name);
  if (std::string(prop_name) == "args") {
    return ::strdup(app->args.c_str());
  }
  return nullptr;
}

int get_param_double(void* app_opaque, double* /* unused */, const char* prop_name)
{
  static_cast<test_app*>(app_opaque)->param_names.insert(prop_name);
  return -1;
}

TRXState make_state(test_app& app)
{
  TRXState state             = {};
  state.trx_api_version      = TRX_API_VERSION;
  state.app_opaque           = &app;
  state.trx_get_param_string = &get_param_string;
  state.trx_get_param_double = &get_param_double;
  return state;
}

/// Loads the driver, initialises it without arguments and checks the callbacks it sets.
void test_init(driver_init_func driver_init)
{
  test_app app;
  TRXState state = make_state(app);
  TESTASSERT(driver_init(&state) == 0);
  std::cout << "Driver initialised successfully." << std::endl;

  TESTASSERT(app.param_names.count("args"));
  std::cout << "Driver parsed parameters." << std::endl;

  TESTASSERT(state.trx_get_sample_rate_func);
  TESTASSERT(state.trx_start_func2);
  TESTASSERT(state.trx_write_func == nullptr);
  TESTASSERT(state.trx_read_func == nullptr);
  TESTASSERT(state.trx_write_func2);
  TESTASSERT(state.trx_read_func2);
  TESTASSERT(state.trx_set_tx_gain_func);
  TESTASSERT(state.trx_set_rx_gain_func);
  TESTASSERT(state.trx_get_tx_gain_func);
  TESTASSERT(state.trx_get_rx_gain_func);
  TESTASSERT(state.trx_get_tx_samples_per_packet_func);
  TESTASSERT(state.trx_get_stats);
  TESTASSERT(state.trx_dump_info);
  TESTASSERT(state.trx_stop_func);
  TESTASSERT(state.trx_end_func);
  std::cout << "Driver set callbacks." << std::endl;

  state.trx_end_func(&state);
}

/// Checks that the driver rejects an unknown argument rather than ignoring it.
void test_unknown_argument(driver_init_func driver_init)
{
  test_app app{"rx_port=5000,unknown_key=1", {}};
  TRXState state = make_state(app);
  TESTASSERT(driver_init(&state) != 0);
  std::cout << "Driver rejected an unknown argument." << std::endl;
}

/// Checks that the driver rejects out-of-range values of known arguments.
void test_invalid_argument(driver_init_func driver_init)
{
  // A receive timeout of zero would end every read before the socket is polled.
  test_app timeout_app{"rx_timeout_ms=0", {}};
  TRXState timeout_state = make_state(timeout_app);
  TESTASSERT(driver_init(&timeout_state) != 0);

  // At 8-bit depth this packet size fits a UDP payload only before padding to a whole 32-bit word.
  test_app size_app{"bit_depth=8,pkt_samples=32739", {}};
  TRXState size_state = make_state(size_app);
  TESTASSERT(driver_init(&size_state) != 0);

  test_app largest_app{"bit_depth=8,pkt_samples=32738", {}};
  TRXState largest_state = make_state(largest_app);
  TESTASSERT(driver_init(&largest_state) == 0);
  largest_state.trx_end_func(&largest_state);
  std::cout << "Driver rejected invalid argument values." << std::endl;
}

/// Starts a single-channel driver instance at the given sample rate.
void start(TRXState& state, int sample_rate_hz)
{
  int64_t     freq      = 0;
  double      gain      = 0;
  int         bandwidth = 0;
  int         nof_ch    = 1;
  TRXFraction rate      = {sample_rate_hz, 1};

  TRXDriverParams2 params      = {};
  params.rx_channel_count      = 1;
  params.tx_channel_count      = 1;
  params.rx_freq               = &freq;
  params.tx_freq               = &freq;
  params.rx_gain               = &gain;
  params.tx_gain               = &gain;
  params.rx_bandwidth          = &bandwidth;
  params.tx_bandwidth          = &bandwidth;
  params.rf_port_count         = 1;
  params.sample_rate           = &rate;
  params.tx_port_channel_count = &nof_ch;
  params.rx_port_channel_count = &nof_ch;
  TESTASSERT(state.trx_start_func2(&state, &params) == 0);
}

/// A pair of driver instances wired to each other over the loopback interface.
struct loopback_pair {
  test_app tx_app;
  test_app rx_app;
  TRXState tx_state;
  TRXState rx_state;
};

/// Initialises and starts a pair of driver instances sharing the given extra arguments.
void start_pair(loopback_pair& pair, driver_init_func driver_init, const std::string& extra_args, int sample_rate_hz)
{
  static constexpr unsigned first_port  = 47991;
  static constexpr unsigned second_port = 47992;

  const std::string common = ",tx_addr=127.0.0.1,rx_addr=127.0.0.1" + extra_args;
  pair.tx_app   = {"rx_port=" + std::to_string(first_port) + ",tx_port=" + std::to_string(second_port) + common, {}};
  pair.rx_app   = {"rx_port=" + std::to_string(second_port) + ",tx_port=" + std::to_string(first_port) + common, {}};
  pair.tx_state = make_state(pair.tx_app);
  pair.rx_state = make_state(pair.rx_app);
  TESTASSERT(driver_init(&pair.tx_state) == 0);
  TESTASSERT(driver_init(&pair.rx_state) == 0);
  start(pair.tx_state, sample_rate_hz);
  start(pair.rx_state, sample_rate_hz);
}

/// Stops and releases both instances of a pair.
void stop_pair(loopback_pair& pair)
{
  pair.tx_state.trx_stop_func(&pair.tx_state);
  pair.rx_state.trx_stop_func(&pair.rx_state);
  pair.tx_state.trx_end_func(&pair.tx_state);
  pair.rx_state.trx_end_func(&pair.rx_state);
}

/// Reads once from the receiving instance and returns the number of samples and their timestamp.
int read_once(loopback_pair& pair, std::vector<float>& buffer, trx_timestamp_t& timestamp)
{
  void*           rx_samples[] = {buffer.data()};
  TRXReadMetadata rx_metadata  = {};
  return pair.rx_state.trx_read_func2(
      &pair.rx_state, &timestamp, rx_samples, static_cast<int>(buffer.size() / 2), 0, &rx_metadata);
}

/// Sends two packets of an odd number of 8-bit samples, which each leave half a 32-bit word to pad, and checks they
/// arrive intact and back to back, without the padding showing up as a sample.
void test_loopback_8bit_odd(driver_init_func driver_init)
{
  static constexpr int   sample_rate_hz = 1920000;
  static constexpr int   nof_samples    = 3;
  static constexpr float amplitude      = 0.5F;
  // One step of an 8-bit IQ component, relative to the full scale of 1.0.
  static constexpr float tolerance = 1.0F / 128.0F;

  loopback_pair pair;
  start_pair(pair, driver_init, ",bit_depth=8,pkt_samples=" + std::to_string(nof_samples), sample_rate_hz);

  std::vector<float> tx_buffer    = {amplitude, -amplitude, amplitude / 2, 0.0F, -amplitude, amplitude / 4};
  const void*        tx_samples[] = {tx_buffer.data()};
  TRXWriteMetadata   tx_metadata  = {};
  pair.tx_state.trx_write_func2(&pair.tx_state, 0, tx_samples, nof_samples, 0, &tx_metadata);
  pair.tx_state.trx_write_func2(&pair.tx_state, nof_samples, tx_samples, nof_samples, 0, &tx_metadata);

  // Both packets fill one read only if neither one's padding was taken for a sample.
  std::vector<float> rx_buffer(2 * 2 * nof_samples);
  trx_timestamp_t    timestamp = 0;
  TESTASSERT(read_once(pair, rx_buffer, timestamp) == 2 * nof_samples);
  TESTASSERT(timestamp == 0);
  for (unsigned i = 0, e = tx_buffer.size(); i != e; ++i) {
    TESTASSERT(std::abs(rx_buffer[i] - tx_buffer[i]) <= tolerance);
    TESTASSERT(std::abs(rx_buffer[e + i] - tx_buffer[i]) <= tolerance);
  }
  std::cout << "Driver looped back an odd number of 8-bit samples." << std::endl;

  stop_pair(pair);
}

/// Sends two packets with a gap between them and checks that the second one starts the next read rather than being
/// lost.
void test_loopback_gap(driver_init_func driver_init)
{
  static constexpr int   sample_rate_hz = 1920000;
  static constexpr int   pkt_samples    = 480;
  static constexpr float amplitude      = 0.5F;

  loopback_pair pair;
  start_pair(pair, driver_init, ",pkt_samples=" + std::to_string(pkt_samples), sample_rate_hz);

  std::vector<float> tx_buffer(2 * pkt_samples, amplitude);
  const void*        tx_samples[] = {tx_buffer.data()};
  TRXWriteMetadata   tx_metadata  = {};
  pair.tx_state.trx_write_func2(&pair.tx_state, 0, tx_samples, pkt_samples, 0, &tx_metadata);
  pair.tx_state.trx_write_func2(&pair.tx_state, 2 * pkt_samples, tx_samples, pkt_samples, 0, &tx_metadata);

  // A read wide enough for both packets stops at the gap.
  std::vector<float> rx_buffer(2 * 2 * pkt_samples);
  trx_timestamp_t    timestamp = 0;
  TESTASSERT(read_once(pair, rx_buffer, timestamp) == pkt_samples);
  TESTASSERT(timestamp == 0);

  // The packet after the gap is returned by the next read, at its own timestamp.
  TESTASSERT(read_once(pair, rx_buffer, timestamp) == pkt_samples);
  TESTASSERT(timestamp == 2 * pkt_samples);
  TESTASSERT(rx_buffer[0] != 0.0F);
  std::cout << "Driver kept the packet after a gap." << std::endl;

  stop_pair(pair);
}

/// Sends a buffer from one driver instance to another over the loopback interface and checks it arrives intact.
void test_loopback(driver_init_func driver_init)
{
  static constexpr int   sample_rate_hz = 1920000;
  static constexpr int   pkt_samples    = 480;
  static constexpr int   nof_samples    = 1920;
  static constexpr float amplitude      = 0.5F;
  // One step of a 16-bit IQ component, relative to the full scale of 1.0.
  static constexpr float tolerance = 1.0F / 32768.0F;

  const std::string common = ",tx_addr=127.0.0.1,rx_addr=127.0.0.1,pkt_samples=" + std::to_string(pkt_samples);
  test_app          tx_app{"rx_port=47991,tx_port=47992" + common, {}};
  test_app          rx_app{"rx_port=47992,tx_port=47991" + common, {}};
  TRXState          tx_state = make_state(tx_app);
  TRXState          rx_state = make_state(rx_app);
  TESTASSERT(driver_init(&tx_state) == 0);
  TESTASSERT(driver_init(&rx_state) == 0);
  start(tx_state, sample_rate_hz);
  start(rx_state, sample_rate_hz);

  // A ramp, so that a misplaced or reordered sample shows as a mismatch.
  std::vector<float> tx_buffer(2 * nof_samples);
  for (int i = 0; i != nof_samples; ++i) {
    tx_buffer[2 * i]     = amplitude * static_cast<float>(i) / nof_samples;
    tx_buffer[2 * i + 1] = -amplitude * static_cast<float>(i) / nof_samples;
  }
  const void*      tx_samples[] = {tx_buffer.data()};
  TRXWriteMetadata tx_md        = {};
  tx_state.trx_write_func2(&tx_state, 0, tx_samples, nof_samples, 0, &tx_md);

  std::vector<float> rx_buffer(2 * nof_samples);
  int                nof_received = 0;
  while (nof_received < nof_samples) {
    void*           rx_samples[] = {rx_buffer.data() + 2 * nof_received};
    trx_timestamp_t timestamp    = 0;
    TRXReadMetadata rx_md        = {};
    const int       nof_read =
        rx_state.trx_read_func2(&rx_state, &timestamp, rx_samples, nof_samples - nof_received, 0, &rx_md);
    TESTASSERT(nof_read > 0);
    TESTASSERT(timestamp == static_cast<trx_timestamp_t>(nof_received));
    nof_received += nof_read;
  }

  for (int i = 0; i != 2 * nof_samples; ++i) {
    TESTASSERT(std::abs(rx_buffer[i] - tx_buffer[i]) <= tolerance);
  }
  std::cout << "Driver looped back " << nof_samples << " samples." << std::endl;

  tx_state.trx_stop_func(&tx_state);
  rx_state.trx_stop_func(&rx_state);
  tx_state.trx_end_func(&tx_state);
  rx_state.trx_end_func(&rx_state);
}

} // namespace

int main(int argc, char** argv)
{
  if (argc != 2) {
    std::cerr << "Expected usage: " << argv[0] << " library.so" << std::endl;
    return EXIT_FAILURE;
  }

  void* handle = ::dlopen(argv[1], RTLD_NOW);
  if (handle == nullptr) {
    std::cerr << "Error: " << ::dlerror() << std::endl;
    return EXIT_FAILURE;
  }

  // Clear any existing error.
  ::dlerror();
  auto        driver_init = reinterpret_cast<driver_init_func>(::dlsym(handle, "trx_driver_init"));
  const char* error       = ::dlerror();
  if (error != nullptr) {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "Dynamic library loaded." << std::endl;

  test_init(driver_init);
  test_unknown_argument(driver_init);
  test_invalid_argument(driver_init);
  test_loopback(driver_init);
  test_loopback_8bit_odd(driver_init);
  test_loopback_gap(driver_init);

  ::dlclose(handle);
  std::cout << "Successful." << std::endl;
  return EXIT_SUCCESS;
}
