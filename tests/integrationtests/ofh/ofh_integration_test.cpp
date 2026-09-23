// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ethernet/ethernet_rx_buffer_pool.h"
#include "ofh_integration_test_config.h"
#include "ofh_integration_test_non_rt_ru_factory.h"
#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/circular_map.h"
#include "ocudu/adt/format.h"
#include "ocudu/ofh/ecpri/ecpri_constants.h"
#include "ocudu/ofh/ethernet/ethernet_controller.h"
#include "ocudu/ofh/ethernet/ethernet_frame_notifier.h"
#include "ocudu/ofh/ethernet/ethernet_receiver.h"
#include "ocudu/ofh/ethernet/ethernet_receiver_metrics_collector.h"
#include "ocudu/ofh/ethernet/ethernet_transmitter.h"
#include "ocudu/ofh/ethernet/ethernet_transmitter_metrics_collector.h"
#include "ocudu/phy/support/resource_grid_context.h"
#include "ocudu/phy/support/resource_grid_writer.h"
#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/phy/support/support_factories.h"
#include "ocudu/ru/ofh/ru_ofh_configuration.h"
#include "ocudu/ru/ofh/ru_ofh_executor_mapper_factory.h"
#include "ocudu/ru/ofh/ru_ofh_factory.h"
#include "ocudu/ru/ru_controller.h"
#include "ocudu/ru/ru_downlink_plane.h"
#include "ocudu/ru/ru_error_notifier.h"
#include "ocudu/ru/ru_timing_notifier.h"
#include "ocudu/ru/ru_uplink_plane.h"
#include "ocudu/support/executors/task_execution_manager.h"
#include "ocudu/support/executors/task_executor.h"
#include "fmt/std.h"
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <mutex>
#include <net/if.h>
#include <netinet/ether.h>
#include <random>
#include <sys/ioctl.h>

using namespace ocudu;
using namespace ofh;
using namespace std::chrono_literals;

/// Random generator.
static std::mt19937 rgen(0);

/// Transmission window parameters expressed in symbols, given the 30kHz scs.
unsigned T1a_max_cp_dl = 13; // 470us.
unsigned T1a_min_cp_dl = 8;  // 258us.
unsigned T1a_max_cp_ul = 8;  // 300us.
unsigned T1a_min_cp_ul = 8;  // 285us.
unsigned T1a_max_up    = 9;  // 350us.
unsigned T1a_min_up    = 2;  // 50us.
/// Reception window parameters expressed in symbols, given the 30kHz scs.
unsigned Ta4_min = 1;  // 35us.
unsigned Ta4_max = 28; // 1ms.

static const tdd_ul_dl_pattern tdd_pattern_7d2u{10, 7, 0, 2, 0};
static const tdd_ul_dl_pattern tdd_pattern_6d3u{10, 6, 0, 3, 0};
static tdd_ul_dl_pattern       tdd_pattern = tdd_pattern_7d2u;

static const unsigned vlan_tag               = 9;
static const unsigned processing_delay_slots = 6;
static unsigned       nof_antennas_dl        = 4;
static unsigned       nof_antennas_ul        = 2;

static std::atomic<unsigned> nof_malformed_packets{0};
static std::atomic<unsigned> nof_missing_dl_packets{0};

namespace {

/// Dummy Radio Unit error notifier.
class dummy_ru_error_notifier : public ru_error_notifier
{
public:
  void on_late_downlink_message(const ru_error_context& context) override {}
  void on_late_uplink_message(const ru_error_context& context) override {}
  void on_late_prach_message(const ru_error_context& context) override {}
};
} // namespace

static test::test_parameters test_params;

namespace {

class dummy_frame_notifier : public ether::frame_notifier
{
  // See interface for documentation.
  void on_new_frame(ether::unique_rx_buffer buffer) override {}
};
dummy_frame_notifier dummy_notifier;

/// Test Ethernet receiver interface.
class test_ether_receiver : public ether::receiver, public ether::receiver_operation_controller
{
public:
  test_ether_receiver(ocudulog::basic_logger& logger_) : logger(logger_), notifier(dummy_notifier) {}
  virtual ~test_ether_receiver() = default;

  receiver_operation_controller& get_operation_controller() override { return *this; }

  void start(ether::frame_notifier& notifier_) override
  {
    notifier = std::ref(notifier_);
    logger.debug("Test Ethernet receiver started");
  }

  void stop() override
  {
    stop_requested.store(true, std::memory_order_relaxed);

    while (is_running.load(std::memory_order_relaxed)) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  // See interface for documentation.
  ether::receiver_metrics_collector* get_metrics_collector() override { return nullptr; }

  virtual void push_new_data(span<const uint8_t> frame) = 0;

protected:
  ocudulog::basic_logger&                       logger;
  std::reference_wrapper<ether::frame_notifier> notifier;
  std::atomic<bool>                             is_running{false};
  std::atomic<bool>                             stop_requested{false};
};

/// Dummy Ethernet receiver that receives data from RU emulator and pushes them to the OFH receiver without using real
/// Ethernet interface.
class dummy_eth_receiver : public test_ether_receiver
{
public:
  dummy_eth_receiver(ocudulog::basic_logger& logger_, ether::ethernet_rx_buffer_pool& pool) :
    test_ether_receiver(logger_), buffer_pool(pool)
  {
  }

  void push_new_data(span<const uint8_t> frame) override
  {
    if (stop_requested.load(std::memory_order_relaxed)) {
      return;
    }
    is_running.store(true, std::memory_order::memory_order_relaxed);

    auto exp_buffer = buffer_pool.reserve();
    if (!exp_buffer.has_value()) {
      logger.warning("Dummy Ethernet receiver: no buffer is available for receiving a packet");
      is_running.store(false, std::memory_order::memory_order_relaxed);
      return;
    }
    ether::ethernet_rx_buffer_impl buffer = std::move(exp_buffer.value());
    std::memcpy(buffer.storage().data(), frame.data(), frame.size());
    buffer.resize(frame.size());

    notifier.get().on_new_frame(ether::unique_rx_buffer(std::move(buffer)));
    is_running.store(false, std::memory_order::memory_order_relaxed);
  }

private:
  ether::ethernet_rx_buffer_pool& buffer_pool;
};

/// Ethernet receiver using loopback ('lo') interface.
class lo_eth_receiver : public test_ether_receiver
{
public:
  lo_eth_receiver(ocudulog::basic_logger& logger_) : test_ether_receiver(logger_) { init_loopback_connection(); }

  // See interface for documentation.
  void push_new_data(span<const uint8_t> frame) override
  {
    if (::sendto(socket_fd,
                 frame.data(),
                 frame.size(),
                 0,
                 reinterpret_cast<::sockaddr*>(&socket_address),
                 sizeof(socket_address)) < 0) {
      fmt::print("sendto failed to transmit {} bytes", frame.size());
    }
  }

private:
  void init_loopback_connection()
  {
    socket_fd = ::socket(AF_PACKET, SOCK_RAW | SOCK_NONBLOCK, IPPROTO_RAW);
    if (socket_fd < 0) {
      report_error("Unable to open raw socket for Ethernet gateway: {}", ::strerror(errno));
    }

    // Get the index of loopback interface.
    ::ifreq if_idx = {};
    ::strncpy(if_idx.ifr_name, "lo", IFNAMSIZ - 1);
    if (::ioctl(socket_fd, SIOCGIFINDEX, &if_idx) < 0) {
      report_error("Unable to get index for loopback interface");
    }

    // Prepare the socket address used by sendto.
    socket_address             = {};
    socket_address.sll_ifindex = if_idx.ifr_ifindex;
    socket_address.sll_halen   = ETH_ALEN;
  }

  /// Ethernet structures.
  int         socket_fd = -1;
  sockaddr_ll socket_address;
};

/// Dummy RU notifier class for symbol events.
class dummy_rx_symbol_notifier : public ru_uplink_plane_rx_symbol_notifier
{
public:
  // See interface for documentation.
  void on_new_uplink_symbol(const ru_uplink_rx_symbol_context& context,
                            const shared_resource_grid&        grid,
                            bool                               is_valid) override
  {
    ocudu_assert(grid, "Invalid grid.");
  }

  // See interface for documentation.
  void on_new_prach_window_data(const prach_buffer_context& context, shared_prach_buffer buffer) override {}
};

/// RU emulator class responsible for generating uplink packets with random IQ data.
class test_ru_emulator
{
  /// Ethernet packet size, set to the value used by OFH implementation.
  static constexpr unsigned ethernet_frame_size = 9000;

  /// Helper structure used internally to group OFH header parameters.
  struct header_parameters {
    uint8_t  port;
    unsigned payload_size;
    unsigned start_prb;
    unsigned nof_prbs;
  };

public:
  /// Constructor.
  test_ru_emulator(ocudulog::basic_logger& logger_,
                   task_executor&          executor_,
                   test_ether_receiver&    receiver_,
                   ru_compression_params   compr_params_,
                   unsigned                nof_prb_) :
    logger(logger_), executor(executor_), receiver(receiver_), compr_params(compr_params_), nof_prb(nof_prb_)
  {
    ul_eaxc.assign(test_params.ul_port_id.begin(), test_params.ul_port_id.end());
    prepare_test_data();

    for (unsigned K = 0; K != MAX_SUPPORTED_EAXC_ID_VALUE; ++K) {
      seq_counters.emplace(K, 0);
    }
  }

  /// Generates UL packets with random IQ data for the specified slot and sends to an ethernet receiver.
  void send_uplink_data(slot_point slot)
  {
    if (!executor.execute([this, slot]() { send_uplink(slot); })) {
      logger.warning("Failed to dispatch uplink task");
    }
  }

private:
  void send_uplink(slot_point slot)
  {
    for (unsigned symbol = 0; symbol != get_nsymb_per_slot(cyclic_prefix::NORMAL); ++symbol) {
      // Prepare symbol for all eAxC.
      for (unsigned eaxc_id = 0, end = ul_eaxc.size(); eaxc_id != end; ++eaxc_id) {
        auto& frames = test_data[eaxc_id];
        for (auto& frame : frames) {
          set_header_parameters(frame, slot, symbol, ul_eaxc[eaxc_id]);
        }
      }
      // Send symbol.
      for (const auto& frames : test_data) {
        send(frames);
      }
    }
    logger.info("RU sent UL in slot {}", slot);
  }

  /// Sends byte arrays to the loopback ethernet interface.
  void send(const std::vector<std::vector<uint8_t>>& frames)
  {
    for (const auto& frame : frames) {
      receiver.push_new_data(frame);
    }
  }

  void set_header_parameters(span<uint8_t> frame, slot_point slot, unsigned symbol, unsigned eaxc)
  {
    // Real receiver sends VLAN parameters as part of a Ethernet frame.
    unsigned offset = (test_params.use_loopback_receiver) ? 4 : 0;

    // Set timestamp.
    uint8_t octet      = 0;
    frame[23 + offset] = uint8_t(slot.sfn());
    // Subframe index; offset: 4, 4 bits long.
    octet |= uint8_t(slot.subframe_index()) << 4u;
    // Four MSBs of the slot index within 1ms subframe; offset: 4, 6 bits long.
    octet |= uint8_t(slot.subframe_slot_index() >> 2u);
    frame[24 + offset] = octet;

    octet = 0;
    octet |= uint8_t(slot.subframe_slot_index() & 0x3) << 6u;
    octet |= uint8_t(symbol);
    frame[25 + offset] = octet;

    // Set sequence index.
    uint8_t& seq_id    = seq_counters[eaxc];
    frame[20 + offset] = seq_id++;
  }

  void initialize_header(span<uint8_t> frame, header_parameters params) const
  {
    // Doesn't include VLAN header, as the OFH receiver expects a NIC to strip it.
    static const uint8_t hdr_template_no_vlan[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0xae, 0xfe, 0x10, 0x00, 0x1d, 0xea, 0x00, 0x00, 0x00, 0x80,
                                                   0x10, 0xee, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x00};

    static const uint8_t hdr_template_vlan[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                0x81, 0x00, 0x00, 0x02, 0xae, 0xfe, 0x10, 0x00, 0x1d, 0xea, 0x00, 0x00,
                                                0x00, 0x80, 0x10, 0xee, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0x00};

    const uint8_t* hdr_template_ptr = (test_params.use_loopback_receiver) ? hdr_template_vlan : hdr_template_no_vlan;
    const size_t   hdr_template_size =
        (test_params.use_loopback_receiver) ? sizeof(hdr_template_vlan) : sizeof(hdr_template_no_vlan);
    unsigned offset = (test_params.use_loopback_receiver) ? 4 : 0;

    // Copy default header.
    std::memcpy(frame.data(), hdr_template_ptr, hdr_template_size);

    // Set VLAN tag.
    if (test_params.use_loopback_receiver) {
      // MSBs containing the PCP + VID.
      frame[14] = static_cast<uint8_t>(vlan_tag >> 8);
      // LSBs containing the VID.
      frame[15] = static_cast<uint8_t>(vlan_tag & 0xff);
    }

    // Set correct payload size.
    uint16_t payload_size = htons(params.payload_size);
    std::memcpy(&frame[16 + offset], &payload_size, sizeof(uint16_t));

    // Set port ID.
    frame[19 + offset] = params.port;

    // Set start PRB and number of PRBs.
    frame[27 + offset] = uint8_t(params.start_prb >> 8u) & 0x3;
    frame[28 + offset] = uint8_t(params.start_prb);
    frame[29 + offset] = uint8_t((params.nof_prbs == nof_prb) ? 0 : params.nof_prbs);

    // Set compression header.
    uint8_t octet = 0U;
    octet |= uint8_t(compr_params.data_width) << 4U;
    octet |= uint8_t(to_underlying(compr_params.type));
    frame[30 + offset] = octet;
  }

  static void fill_random_data(span<uint8_t> frame)
  {
    std::uniform_int_distribution<uint8_t> dist{0, 255};
    std::generate(frame.begin(), frame.end(), [&]() { return dist(rgen); });
  }

  void prepare_test_data()
  {
    units::bytes ecpri_iq_data_header_size(8);
    units::bytes ofh_header_size(10);
    units::bytes ether_header_size(14);
    if (test_params.use_loopback_receiver) {
      // VLAN parameters are added in case real Ethernet receiver is used.
      ether_header_size = units::bytes(18);
    }
    unsigned headers_size = (ether_header_size + ecpri_iq_data_header_size + ofh_header_size).value();

    prb_size = units::bits(compr_params.data_width * NOF_SUBCARRIERS_PER_RB * 2 +
                           (compr_params.type == compression_type::BFP ? 8 : 0))
                   .round_up_to_bytes();
    unsigned iq_data_size = nof_prb * prb_size.value();

    // It is assumed that maximum 2 packets required to send all data for antenna.
    unsigned              nof_frames = ((headers_size + iq_data_size) > ethernet_frame_size) ? 2u : 1u;
    std::vector<unsigned> nof_frame_prbs;
    if (nof_frames == 1) {
      nof_frame_prbs.push_back(nof_prb);
    } else {
      unsigned nof_prbs_first  = (ethernet_frame_size - headers_size) / prb_size.value();
      unsigned nof_prbs_second = nof_prb - nof_prbs_first;
      nof_frame_prbs.push_back(nof_prbs_first);
      nof_frame_prbs.push_back(nof_prbs_second);
    }

    /// Initializes ethernet packet headers for all antennas. Timestamp, sequence index and symbol index will be updated
    /// on every transmission.
    for (unsigned port = 0; port != nof_antennas_ul; ++port) {
      test_data.emplace_back();
      std::vector<std::vector<uint8_t>>& ether_frames = test_data.back();

      unsigned start_prb = 0;
      for (unsigned j = 0; j != nof_frames; ++j) {
        unsigned data_size = nof_frame_prbs[j] * prb_size.value();

        ether_frames.emplace_back();
        std::vector<uint8_t>& frame = ether_frames.back();
        frame.resize(headers_size + data_size);

        // Prepare header.
        span<uint8_t>     frame_header(frame.data(), headers_size);
        header_parameters params;
        params.port         = port;
        params.payload_size = data_size + ofh_header_size.value() + ecpri::ECPRI_COMMON_HEADER_SIZE.value();
        params.start_prb    = start_prb;
        params.nof_prbs     = nof_frame_prbs[j];
        initialize_header(frame_header, params);

        // Prepare IQ data.
        fill_random_data(span<uint8_t>(frame).last(data_size));

        start_prb += nof_frame_prbs[j];
      }
    }
  }

private:
  ocudulog::basic_logger&     logger;
  task_executor&              executor;
  test_ether_receiver&        receiver;
  const ru_compression_params compr_params;
  const unsigned              nof_prb;
  units::bytes                prb_size;
  /// Stores byte arrays for each antenna.
  std::vector<std::vector<std::vector<uint8_t>>>                     test_data;
  static_circular_map<uint8_t, uint8_t, MAX_SUPPORTED_EAXC_ID_VALUE> seq_counters;
  static_vector<unsigned, ofh::MAX_NOF_SUPPORTED_EAXC>               ul_eaxc;
};

/// \brief DU emulator that pushes resource grids to the OFH RU implementation.
///
/// Every slot notified by RU is processed in the DU emulator executor, until the configured number of test slots has
/// been processed.
class test_du_emulator
{
  /// Number of slots to wait after the OTA time of the last processed slot. This allows to finish uplink processing.
  static constexpr unsigned nof_rx_window_slots = 3;

public:
  test_du_emulator(ocudulog::basic_logger&    logger_,
                   task_executor&             executor_,
                   resource_grid_pool&        dl_rg_pool_,
                   resource_grid_pool&        ul_rg_pool_,
                   ru_downlink_plane_handler& dl_handler_,
                   ru_uplink_plane_handler&   ul_handler_) :
    logger(logger_),
    dl_rg_pool(dl_rg_pool_),
    ul_rg_pool(ul_rg_pool_),
    executor(executor_),
    dl_handler(dl_handler_),
    ul_handler(ul_handler_)
  {
  }

  /// \brief Handles a new TTI boundary notified by the RU.
  ///
  /// \note this method is called from the RU timing thread.
  void handle_tti_boundary(slot_point slot)
  {
    // If we arrived at the end of the test, wait for the processing delay, as the TTI boundary leads the OTA time by
    // the processing delay.
    if (nof_dispatched_slots == test_params.nof_test_slots) {
      if (!is_test_finished() && (last_slot + processing_delay_slots + nof_rx_window_slots <= slot)) {
        test_finished.store(true, std::memory_order_relaxed);
      }
      return;
    }

    if (nof_dispatched_slots == 0) {
      fmt::print("Initial slot set to {}\n", slot);
    }
    ++nof_dispatched_slots;
    last_slot = slot;

    if (!executor.execute([this, slot]() { process_slot(slot); })) {
      logger.warning("Failed to dispatch DU emulator task for slot {}", slot);
    }
  }

  bool is_test_finished() const { return test_finished.load(std::memory_order_relaxed); }

private:
  void process_slot(slot_point slot)
  {
    // Max attempts of allocating resource grid from the pool.
    static constexpr unsigned rg_alloc_max_attempts = 10;

    unsigned slot_id    = slot.slot_index() % tdd_pattern.dl_ul_tx_period_nof_slots;
    bool     is_dl_slot = (slot_id < tdd_pattern.nof_dl_slots);
    bool     is_ul_slot = (slot_id >= tdd_pattern.dl_ul_tx_period_nof_slots - tdd_pattern.nof_ul_slots);

    ocudu_assert(!(is_dl_slot && is_ul_slot), "Invalid slot type: both DL and UL can not be set simultaneously");

    int alloc_attempts = rg_alloc_max_attempts;
    // Push downlink data.
    if (is_dl_slot) {
      resource_grid_context context{slot, 0};
      shared_resource_grid  dl_grid;
      while (!dl_grid && alloc_attempts--) {
        dl_grid = dl_rg_pool.allocate_resource_grid(slot);
        if (!dl_grid) {
          std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
      }

      if (dl_grid) {
        dl_handler.handle_dl_data(context, dl_grid);
        logger.info("DU emulator pushed DL data in slot {}", slot);
      } else {
        logger.warning("No resource grid is available for processing DL slot");
      }
    }

    // Request uplink data.
    if (is_ul_slot) {
      resource_grid_context context{slot, 0};
      shared_resource_grid  ul_grid;
      while (!ul_grid && alloc_attempts--) {
        ul_grid = ul_rg_pool.allocate_resource_grid(slot);
        if (!ul_grid) {
          std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
      }

      if (ul_grid) {
        ul_handler.handle_new_uplink_slot(context, ul_grid);
        logger.info("DU emulator requested UL data in slot {}", slot);
      } else {
        logger.warning("No resource grid is available for processing UL slot");
      }
    }
  }

  ocudulog::basic_logger&    logger;
  resource_grid_pool&        dl_rg_pool;
  resource_grid_pool&        ul_rg_pool;
  task_executor&             executor;
  ru_downlink_plane_handler& dl_handler;
  ru_uplink_plane_handler&   ul_handler;

  /// Number of slots dispatched for processing.
  unsigned nof_dispatched_slots = 0;
  /// Last slot dispatched for processing.
  slot_point        last_slot;
  std::atomic<bool> test_finished{false};
};

/// Dummy RU notifier class for timing events that forwards the TTI boundaries to the DU emulator.
class dummy_timing_notifier : public ru_timing_notifier
{
public:
  /// Connects the DU emulator.
  void connect_du(test_du_emulator& du_emulator_) { du_emulator = &du_emulator_; }

  // See interface for documentation.
  void on_tti_boundary(const tti_boundary_context& slot_context) override
  {
    if (du_emulator != nullptr) {
      du_emulator->handle_tti_boundary(slot_context.slot.without_hyper_sfn());
    }
  }

  // See interface for documentation.
  void on_ul_half_slot_boundary(slot_point slot) override {}

  // See interface for documentation.
  void on_ul_full_slot_boundary(slot_point slot) override {}

private:
  test_du_emulator* du_emulator = nullptr;
};

/// Ethernet transmitter gateway that analyzes incoming packets and checks integrity of the DL packets, as well as asks
/// RU emulator for UL traffic generation.
class test_gateway : public ether::transmitter
{
public:
  test_gateway() :
    scs(test_params.scs),
    nof_symbols(get_nsymb_per_slot(cyclic_prefix::NORMAL)),
    seq_counter_initialized(MAX_SUPPORTED_EAXC_ID_VALUE)
  {
    for (unsigned K = 0; K != MAX_SUPPORTED_EAXC_ID_VALUE; ++K) {
      seq_counters.insert(K, 0);
    }
  }

  void connect_ru(test_ru_emulator* ru_emulator_) { ru_emulator = ru_emulator_; }

  // See interface for documentation.
  void send(span<span<const uint8_t>> frames) override
  {
    slot_point sent_ul_slot = {};
    for (auto frame : frames) {
      // For DL messages check seq id and make sure packets for all antennas were transmitted.
      if (peek_direction(frame) == data_direction::downlink) {
        if (peek_message_type(frame) == ecpri::message_type::iq_data) {
          check_and_update_sequence_id(frame);
        }
        continue;
      }
      // For UL message ask the RU emulator to send UP packets to the loopback interface.
      ocudu_assert(ru_emulator != nullptr, "RU emulator uninitialized");
      slot_point slot = peek_slot_point(frame);
      if (slot != sent_ul_slot) {
        ru_emulator->send_uplink_data(slot);
        sent_ul_slot = slot;
      }
    }
  }

  // See interface for documentation.
  ether::transmitter_metrics_collector* get_metrics_collector() override { return nullptr; }

private:
  void check_and_update_sequence_id(span<const uint8_t> message)
  {
    // Retrieve eAxC and SeqID from codified message.
    unsigned seq_id = message[24];
    unsigned eaxc   = (unsigned(message[22]) << 8u | message[23]);

    ocudu_assert(eaxc < MAX_SUPPORTED_EAXC_ID_VALUE, "Invalid eAxC={} detected", eaxc);

    if (!seq_counter_initialized.test(eaxc)) {
      seq_counter_initialized.set(eaxc);
      seq_counters[eaxc] = seq_id + 1;
      return;
    }

    uint8_t& expected_seq_id = seq_counters[eaxc];
    if (seq_id == expected_seq_id) {
      expected_seq_id++;
      return;
    }

    nof_missing_dl_packets++;
    expected_seq_id = seq_id + 1;
  }

  static data_direction peek_direction(span<const uint8_t> message)
  {
    if (message.size() < 27) {
      nof_malformed_packets++;
      return data_direction::downlink;
    }
    // Filter index is codified in the byte 26, bit 7.
    unsigned direction = (message[26] & 0x80) >> 7u;
    return (direction == 1) ? data_direction::downlink : data_direction::uplink;
  }

  static ecpri::message_type peek_message_type(span<const uint8_t> message)
  {
    return static_cast<ecpri::message_type>(message[19]);
  }

  slot_point peek_slot_point(span<const uint8_t> message) const
  {
    // Slot is codified in the bytes 27-29 of the Ethernet packet.
    if (message.size() < 30) {
      return slot_point{};
    }

    uint8_t  frame             = message[27];
    uint8_t  subframe_and_slot = message[28];
    uint8_t  subframe          = subframe_and_slot >> 4;
    unsigned slot_id           = 0;
    slot_id |= (subframe_and_slot & 0x0f) << 2;

    uint8_t slot_and_symbol = message[29];
    slot_id |= slot_and_symbol >> 6;

    return {to_numerology_value(scs), frame, subframe, slot_id};
  }

private:
  const subcarrier_spacing                                           scs;
  const unsigned                                                     nof_symbols;
  static_circular_map<uint8_t, uint8_t, MAX_SUPPORTED_EAXC_ID_VALUE> seq_counters;
  bounded_bitset<MAX_SUPPORTED_EAXC_ID_VALUE>                        seq_counter_initialized;
  test_ru_emulator*                                                  ru_emulator;
};

/// Manages the workers of the test application and OFH RU.
struct worker_manager {
  static constexpr uint32_t task_worker_queue_size = 2048;
  /// DU emulator queue size.
  static constexpr uint32_t du_sim_queue_size = 8;

  worker_manager() { create_ofh_executors(); }

  void stop() { exec_mng.stop(); }

  void create_ofh_executors()
  {
    using namespace execution_config_helper;

    // Timing executor.
    {
      const std::string name      = "ru_timing";
      const std::string exec_name = "ru_timing_exec";

      const single_worker ru_worker{name,
                                    {exec_name, concurrent_queue_policy::lockfree_spsc, 4},
                                    std::chrono::microseconds{1},
                                    os_thread_realtime_priority::max() - 0};
      if (!exec_mng.add_execution_context(create_execution_context(ru_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
      }
      ru_timing_exec = exec_mng.executors().at(exec_name);
    }

    // Executor for the Open Fronthaul User and Control messages codification.
    {
      unsigned          nof_workers = (test_params.is_downlink_parallelized) ? std::max(nof_antennas_dl / 2U, 1U) : 1U;
      const std::string name        = "ru_dl";
      const std::string exec_name   = "ru_dl_exec";

      const worker_pool ru_pool{name,
                                nof_workers,
                                {{exec_name, concurrent_queue_policy::locking_mpmc, task_worker_queue_size}},
                                std::chrono::microseconds(0),
                                os_thread_realtime_priority::max() - 5,
                                {}};
      if (!exec_mng.add_execution_context(create_execution_context(ru_pool))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_pool.name);
      }
      ru_dl_exec = exec_mng.executors().at(exec_name);
    }

    // Executor for Open Fronthaul messages transmission.
    {
      const std::string name      = "ru_txrx";
      const std::string exec_name = "ru_txrx_exec";

      const single_worker ru_worker{name,
                                    {exec_name, concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
                                    std::chrono::microseconds{5},
                                    os_thread_realtime_priority::max() - 1};
      if (!exec_mng.add_execution_context(create_execution_context(ru_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
      }
      ru_tx_exec = exec_mng.executors().at(exec_name);
    }

    // Executor for Open Fronthaul messages reception.
    {
      const std::string name      = "ru_rx";
      const std::string exec_name = "ru_rx_exec";

      const single_worker ru_worker{name,
                                    {exec_name, concurrent_queue_policy::lockfree_mpmc, task_worker_queue_size},
                                    std::chrono::microseconds{15},
                                    os_thread_realtime_priority::max() - 5};
      if (!exec_mng.add_execution_context(create_execution_context(ru_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_worker.name);
      }
      ru_rx_exec = exec_mng.executors().at(exec_name);
    }

    // Executor for DU emulator responsible for resource grids generation.
    {
      const std::string name      = "du_sim";
      const std::string exec_name = "du_sim_exec";

      const single_worker du_sim_worker{name,
                                        {exec_name, concurrent_queue_policy::locking_mpmc, du_sim_queue_size},
                                        std::nullopt,
                                        os_thread_realtime_priority::max() - 10};
      if (!exec_mng.add_execution_context(create_execution_context(du_sim_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", du_sim_worker.name);
      }
      test_du_sim_exec = exec_mng.executors().at(exec_name);
    }

    // Executor for RU emulator responsible for rx packets generation.
    {
      const std::string name      = "ru_sim";
      const std::string exec_name = "ru_sim_exec";

      const single_worker ru_sim_worker{name,
                                        {exec_name, concurrent_queue_policy::lockfree_spsc, task_worker_queue_size},
                                        std::chrono::microseconds{5},
                                        os_thread_realtime_priority::max() - 2};
      if (!exec_mng.add_execution_context(create_execution_context(ru_sim_worker))) {
        report_fatal_error("Failed to instantiate {} execution context", ru_sim_worker.name);
      }
      test_ru_sim_exec = exec_mng.executors().at(exec_name);
    }
  }

  task_execution_manager                  exec_mng;
  task_executor*                          ru_timing_exec = nullptr;
  task_executor*                          ru_dl_exec;
  task_executor*                          ru_tx_exec;
  task_executor*                          ru_rx_exec;
  task_executor*                          test_du_sim_exec;
  task_executor*                          test_ru_sim_exec;
  std::unique_ptr<ru_ofh_executor_mapper> ofh_exec_mapper;
};
} // namespace

static void configure_ofh_sector(ofh::sector_configuration& sector_cfg)
{
  // Default IQ data scaling to be applied prior to downlink data compression.
  const float iq_scaling = 0.9f;
  // Downlink processing time in microseconds.
  const std::chrono::microseconds dl_processing_time = 400us;

  sector_cfg.max_processing_delay_slots = processing_delay_slots;
  sector_cfg.dl_processing_time         = dl_processing_time;
  sector_cfg.uses_dpdk                  = false;
  sector_cfg.sector_id                  = 0;

  std::chrono::duration<double, std::nano> symbol_duration(
      (1e6 / (get_nsymb_per_slot(cyclic_prefix::NORMAL) * get_nof_slots_per_subframe(test_params.scs))));

  sector_cfg.interface                       = "lo";
  sector_cfg.mac_src_address                 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  sector_cfg.mac_dst_address                 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  sector_cfg.mtu_size                        = test_params.mtu;
  sector_cfg.vlan_cfg_cp                     = ether::vlan_parameters{.tci_vid = vlan_tag};
  sector_cfg.vlan_cfg_up                     = ether::vlan_parameters{.tci_vid = vlan_tag};
  sector_cfg.scs                             = test_params.scs;
  sector_cfg.bw                              = test_params.channel_bw_mhz;
  sector_cfg.ru_operating_bw                 = sector_cfg.bw;
  sector_cfg.cp                              = cyclic_prefix::NORMAL;
  sector_cfg.is_prach_control_plane_enabled  = test_params.is_prach_control_plane_enabled;
  sector_cfg.ignore_ecpri_payload_size_field = test_params.ignore_ecpri_payload_size_field;
  sector_cfg.tx_window_timing_params         = {
      T1a_max_cp_dl, T1a_min_cp_dl, T1a_max_cp_ul, T1a_min_cp_ul, T1a_max_up, T1a_min_up};
  sector_cfg.rx_window_timing_params = {Ta4_min, Ta4_max};

  // Configure compression
  sector_cfg.dl_compression_params                = test_params.data_compr_params;
  sector_cfg.ul_compression_params                = test_params.data_compr_params;
  sector_cfg.prach_compression_params             = test_params.prach_compr_params;
  sector_cfg.iq_scaling                           = iq_scaling;
  sector_cfg.is_downlink_static_compr_hdr_enabled = test_params.is_downlink_static_comp_hdr_enabled;
  sector_cfg.is_uplink_static_compr_hdr_enabled   = test_params.is_uplink_static_comp_hdr_enabled;

  // Configure eAxCs.
  sector_cfg.prach_eaxc.assign(test_params.prach_port_id.begin(), test_params.prach_port_id.end());
  sector_cfg.dl_eaxc.assign(test_params.dl_port_id.begin(), test_params.dl_port_id.end());
  sector_cfg.ul_eaxc.assign(test_params.ul_port_id.begin(), test_params.ul_port_id.end());
  sector_cfg.nof_antennas_ul = nof_antennas_ul;

  // Configure downlink beamforming, the configuration validation guarantees a valid antenna topology.
  if (test_params.beamforming_cfg.enable) {
    sector_cfg.dl_beamforming = transmitter_beamforming_config{
        .topology         = *test::get_dl_antenna_topology(nof_antennas_dl),
        .bfw_compr_params = test_params.beamforming_cfg.bfw_compr_params,
    };
  }
}

static ru_ofh_configuration generate_ru_config()
{
  ru_ofh_configuration ru_cfg;

  ru_cfg.gps_Alpha = 0;
  ru_cfg.gps_Beta  = 0;

  ofh::sector_configuration& sector_cfg = ru_cfg.sector_configs.emplace_back();
  configure_ofh_sector(sector_cfg);

  return ru_cfg;
}

static ru_ofh_dependencies generate_ru_dependencies(ocudulog::basic_logger&             logger,
                                                    worker_manager&                     workers,
                                                    ru_timing_notifier*                 timing_notifier,
                                                    ru_uplink_plane_rx_symbol_notifier* rx_symbol_notifier,
                                                    test_gateway*&                      tx_gateway,
                                                    test_ether_receiver*&               eth_receiver,
                                                    ether::ethernet_rx_buffer_pool&     buffer_pool,
                                                    ru_error_notifier&                  error_notifier)
{
  ru_ofh_dependencies dependencies;
  dependencies.logger             = &logger;
  dependencies.timing_notifier    = timing_notifier;
  dependencies.rx_symbol_notifier = rx_symbol_notifier;
  dependencies.rt_timing_executor = workers.ru_timing_exec;
  dependencies.error_notifier     = &error_notifier;

  // Build the sector executor mapper that owns the per-eAxC serialization strands.
  ru_ofh_executor_mapper_config exec_mapper_cfg;
  exec_mapper_cfg.dl_eaxc_per_sector = {test_params.dl_port_id};
  exec_mapper_cfg.downlink_executor  = workers.ru_dl_exec;
  exec_mapper_cfg.uplink_executor    = workers.ru_rx_exec;
  exec_mapper_cfg.txrx_executors     = {workers.ru_tx_exec};
  exec_mapper_cfg.timing_executor    = workers.ru_timing_exec;
  workers.ofh_exec_mapper            = create_ofh_ru_executor_mapper(exec_mapper_cfg);

  // Configure Ethernet gateway.
  auto gateway = std::make_unique<test_gateway>();
  tx_gateway   = gateway.get();

  // Configure Ethernet receiver.
  auto dummy_receiver = std::make_unique<dummy_eth_receiver>(logger, buffer_pool);
  eth_receiver        = dummy_receiver.get();

  dependencies.sector_dependencies.emplace_back(ofh::sector_dependencies{
      .logger          = &logger,
      .exec_mapper     = workers.ofh_exec_mapper->get_sector_mapper(0),
      .eth_transmitter = std::move(gateway),
      .eth_receiver    = std::move(dummy_receiver),
  });

  return dependencies;
}

static std::unique_ptr<resource_grid_pool>
create_dl_resource_grid_pool(std::shared_ptr<resource_grid_factory> rg_factory, unsigned nof_prb)
{
  std::uniform_real_distribution<float>       dist(-1.0, +1.0);
  std::vector<std::unique_ptr<resource_grid>> dl_resource_grids;

  // Create resource grids according to TDD pattern.
  for (unsigned rg_id = 0, e = processing_delay_slots * tdd_pattern.nof_dl_slots; rg_id != e; rg_id++) {
    dl_resource_grids.push_back(
        rg_factory->create(nof_antennas_dl, MAX_NSYMB_PER_SLOT, nof_prb * NOF_SUBCARRIERS_PER_RB));
    resource_grid_writer& rg_writer = dl_resource_grids.back()->get_writer();

    // Pre-generate random downlink data.
    for (unsigned sym = 0; sym != get_nsymb_per_slot(cyclic_prefix::NORMAL); ++sym) {
      for (unsigned port = 0; port != nof_antennas_dl; ++port) {
        std::vector<cf_t> test_data(nof_prb * NOF_SUBCARRIERS_PER_RB);
        std::generate(test_data.begin(), test_data.end(), [&]() { return cf_t{dist(rgen), dist(rgen)}; });
        rg_writer.put(port, sym, 0, test_data);
      }
    }
  }
  return create_generic_resource_grid_pool(std::move(dl_resource_grids));
}

static std::unique_ptr<resource_grid_pool>
create_ul_resource_grid_pool(std::shared_ptr<resource_grid_factory> rg_factory, unsigned nof_prb)
{
  std::vector<std::unique_ptr<resource_grid>> ul_resource_grids;
  for (unsigned rg_id = 0, e = processing_delay_slots * tdd_pattern.nof_ul_slots; rg_id != e; rg_id++) {
    ul_resource_grids.push_back(
        rg_factory->create(nof_antennas_ul, MAX_NSYMB_PER_SLOT, nof_prb * NOF_SUBCARRIERS_PER_RB));
  }
  return create_generic_resource_grid_pool(std::move(ul_resource_grids));
}

int main(int argc, char** argv)
{
  static constexpr unsigned            BUFFER_SIZE = 9600;
  std::unique_ptr<test_ether_receiver> eth_receiver_ptr;

  auto parsed_params = test::parse_test_configuration(argc, argv);
  if (!parsed_params.has_value()) {
    return parsed_params.error();
  }
  test_params     = std::move(*parsed_params);
  tdd_pattern     = (test_params.tdd_pattern_str == "6d3u") ? tdd_pattern_6d3u : tdd_pattern_7d2u;
  nof_antennas_dl = test_params.dl_port_id.size();
  nof_antennas_ul = test_params.ul_port_id.size();

  // Set up logging.
  const std::string& log_filename = test_params.logger_cfg.filename;
  ocudulog::sink*    log_sink =
      (log_filename == "stdout") ? ocudulog::create_stdout_sink() : ocudulog::create_file_sink(log_filename);
  if (log_sink == nullptr) {
    report_error("Could not create application main log sink.\n");
  }
  ocudulog::set_default_sink(*log_sink);
  ocudulog::init();

  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("OFH_TEST", false);
  logger.set_level(test_params.logger_cfg.level);
  ocudulog::fetch_basic_logger("PHY").set_level(ocudulog::basic_levels::error);

  unsigned nof_prb = get_max_Nprb(test_params.channel_bw_mhz, test_params.scs, frequency_range::FR1);

  // Set up resources used by the DU emulator.
  std::shared_ptr<resource_grid_factory> rg_factory = create_resource_grid_factory();
  report_fatal_error_if_not(rg_factory, "Invalid factory");

  auto dl_rg_pool = create_dl_resource_grid_pool(rg_factory, nof_prb);
  auto ul_rg_pool = create_ul_resource_grid_pool(rg_factory, nof_prb);

  ether::ethernet_rx_buffer_pool buffer_pool(BUFFER_SIZE);
  worker_manager                 workers;
  dummy_rx_symbol_notifier       rx_symbol_notifier;
  dummy_timing_notifier          timing_notifier;
  test_gateway*                  tx_gateway;
  test_ether_receiver*           eth_receiver;
  dummy_ru_error_notifier        error_notifier;

  ru_ofh_configuration ru_cfg  = generate_ru_config();
  ru_ofh_dependencies  ru_deps = generate_ru_dependencies(
      logger, workers, &timing_notifier, &rx_symbol_notifier, tx_gateway, eth_receiver, buffer_pool, error_notifier);

  if (test_params.use_loopback_receiver) {
    ru_deps.sector_dependencies[0].eth_receiver.reset();
    eth_receiver_ptr = std::make_unique<lo_eth_receiver>(logger);
    eth_receiver     = eth_receiver_ptr.get();
  }
  std::unique_ptr<radio_unit> ru_object = test_params.is_non_realtime
                                              ? test::create_non_rt_ofh_ru(ru_cfg, std::move(ru_deps))
                                              : create_ofh_ru(ru_cfg, std::move(ru_deps));

  // Get RU downlink plane handler.
  auto& ru_dl_handler = ru_object->get_downlink_plane_handler();
  auto& ru_ul_handler = ru_object->get_uplink_plane_handler();

  // Create RU emulator instance.
  test_ru_emulator ru_emulator(
      logger, *workers.test_ru_sim_exec, *eth_receiver, test_params.data_compr_params, nof_prb);

  // Create DU emulator instance.
  test_du_emulator du_emulator(
      logger, *workers.test_du_sim_exec, *dl_rg_pool, *ul_rg_pool, ru_dl_handler, ru_ul_handler);

  // Connect Ethernet gateway to the RU emulator and the RU timing notifications to the DU emulator.
  tx_gateway->connect_ru(&ru_emulator);
  timing_notifier.connect_du(du_emulator);

  // Start the RU, it notifies the DU emulator through the TTI boundary notifications.
  fmt::print("Starting RU...\n");
  ru_object->get_controller().get_operation_controller().start();
  fmt::print("Running the test...\n");

  // Wait until test is finished.
  while (!du_emulator.is_test_finished()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  fmt::print("DU emulator stopped\n");

  fmt::print("Stopping the RU...\n");
  ru_object->get_controller().get_operation_controller().stop();
  fmt::print("RU stopped successfully.\n");

  workers.stop();
  ocudulog::flush();

  fmt::print("Test finished, nof_missing_dl_packets={}, nof_malformed_packets={}\n",
             nof_missing_dl_packets,
             nof_malformed_packets);

  return 0;
}
