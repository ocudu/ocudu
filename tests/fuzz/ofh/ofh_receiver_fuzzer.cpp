// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// AFL++/libFuzzer harness for the Open Fronthaul receive path of a sector.
///
/// The harness wires the receiver and the uplink request handler of a sector to shared context repositories, as
/// create_ofh_sector() does, and replaces only the Ethernet receiver, the executors and the PHY-facing notifiers:
///
///   uplink requests (PHY)  -> uplink request handler -> uplink, PRACH and C-Plane context repositories
///   fuzz Ethernet frames   -> message receiver -> VLAN / eCPRI / U-Plane decoders -> data flows -> repositories
///   OTA symbols (timing)   -> Rx window checker, closed Rx window handler -> uplink symbol and PRACH notifications
///
/// The downlink side of the transmitter is left out: its Ethernet frame pools take most of the time to build a sector,
/// and nothing of it is reachable from the received frames.
///
/// The decoders alone accept every value their fields can encode. What a field may index depends on the contexts the
/// PHY has stored and on the configuration, so the input describes the configuration and a sequence of uplink requests,
/// received frames and OTA symbol advances, and the harness replays it in order on a single thread.
///
/// See tests/fuzz/README.md for the input layout, build instructions and run commands.

#include "lib/ofh/receiver/ofh_receiver_factories.h"
#include "lib/ofh/support/context_repository_helpers.h"
#include "lib/ofh/transmitter/ofh_data_flow_cplane_scheduling_commands_impl.h"
#include "lib/ofh/transmitter/ofh_data_flow_cplane_scheduling_commands_metrics_decorator.h"
#include "lib/ofh/transmitter/ofh_uplink_request_handler_impl.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ofh/ecpri/ecpri_factories.h"
#include "ocudu/ofh/ethernet/ethernet_controller.h"
#include "ocudu/ofh/ethernet/ethernet_factories.h"
#include "ocudu/ofh/ethernet/ethernet_frame_notifier.h"
#include "ocudu/ofh/ethernet/ethernet_frame_pool.h"
#include "ocudu/ofh/ethernet/ethernet_properties.h"
#include "ocudu/ofh/ethernet/ethernet_receiver.h"
#include "ocudu/ofh/ethernet/ethernet_receiver_metrics_collector.h"
#include "ocudu/ofh/ethernet/ethernet_unique_buffer.h"
#include "ocudu/ofh/ofh_controller.h"
#include "ocudu/ofh/ofh_error_notifier.h"
#include "ocudu/ofh/ofh_sector_config.h"
#include "ocudu/ofh/ofh_uplane_rx_symbol_notifier.h"
#include "ocudu/ofh/receiver/ofh_receiver.h"
#include "ocudu/ofh/serdes/ofh_serdes_factories.h"
#include "ocudu/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "ocudu/phy/support/prach_buffer.h"
#include "ocudu/phy/support/prach_buffer_context.h"
#include "ocudu/phy/support/resource_grid.h"
#include "ocudu/phy/support/resource_grid_context.h"
#include "ocudu/phy/support/resource_grid_pool.h"
#include "ocudu/phy/support/shared_prach_buffer.h"
#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/phy/support/support_factories.h"
#include "ocudu/ran/prach/prach_frequency_mapping.h"
#include "ocudu/ran/prach/prach_preamble_information.h"
#include "ocudu/ran/resource_block.h"
#include "ocudu/support/executors/inline_task_executor.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <vector>

using namespace ocudu;
using namespace ofh;

namespace {

// ---------------------------------------------------------------------------
// Fixed sector parameters
// ---------------------------------------------------------------------------

/// MAC address of the DU, the destination of the received frames.
constexpr ether::mac_address du_mac = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};
/// MAC address of the RU, the source of the received frames.
constexpr ether::mac_address ru_mac = {0x02, 0x00, 0x00, 0x00, 0x00, 0x02};

/// eAxC values of the uplink data, one per antenna.
constexpr std::array<unsigned, 4> ul_eaxc = {0, 1, 2, 3};
/// eAxC values of the PRACH, one per antenna.
constexpr std::array<unsigned, 4> prach_eaxc = {4, 5, 6, 7};

/// Cell bandwidths selectable by the input, all valid for 15 kHz and 30 kHz subcarrier spacing.
constexpr std::array<bs_channel_bandwidth, 4> cell_bandwidths = {bs_channel_bandwidth::MHz5,
                                                                 bs_channel_bandwidth::MHz10,
                                                                 bs_channel_bandwidth::MHz20,
                                                                 bs_channel_bandwidth::MHz40};
/// RU operating bandwidth when the input asks for one larger than the cell bandwidth.
constexpr bs_channel_bandwidth wide_ru_bandwidth = bs_channel_bandwidth::MHz50;

/// Bit widths that the configuration validator accepts for BFP compression.
constexpr std::array<unsigned, 5> bfp_widths = {8, 9, 12, 14, 16};

/// PRACH formats the PHY can request in FR1.
constexpr std::array<prach_format_type, 11> prach_formats = {prach_format_type::zero,
                                                             prach_format_type::one,
                                                             prach_format_type::two,
                                                             prach_format_type::three,
                                                             prach_format_type::A1,
                                                             prach_format_type::A2,
                                                             prach_format_type::A3,
                                                             prach_format_type::B1,
                                                             prach_format_type::B4,
                                                             prach_format_type::C0,
                                                             prach_format_type::C2};

/// MTU of the Ethernet link, which sizes the buffers of the uplink C-Plane frame pool.
constexpr units::bytes mtu_size(9000);
/// Reception window, in symbols, as in the OFH integration test.
constexpr rx_window_timing_parameters rx_window = {1, 28};

/// Number of resource grids and of PRACH buffers of each preamble length.
constexpr unsigned nof_buffers = 4;
/// Maximum number of PRACH time-domain occasions of a short preamble buffer.
constexpr unsigned max_nof_prach_td_occasions = 7;
/// \brief Processing delay of the PHY, in slots.
///
/// The PHY requests an uplink slot at most this many slots ahead of the OTA time. It also sizes the context
/// repositories, whose construction dominates the time of an input.
constexpr unsigned max_processing_delay_slots = 2;
/// Maximum number of operations replayed per input, which keeps each input fast.
constexpr unsigned max_nof_operations = 256;

// ---------------------------------------------------------------------------
// Input encoding
// ---------------------------------------------------------------------------

/// Reads the input front to back, and returns zeros once it is exhausted.
class input_reader
{
public:
  explicit input_reader(span<const uint8_t> data_) : data(data_) {}

  bool empty() const { return data.empty(); }

  uint8_t read_u8()
  {
    if (data.empty()) {
      return 0;
    }
    uint8_t value = data.front();
    data          = data.last(data.size() - 1);
    return value;
  }

  uint16_t read_u16()
  {
    uint16_t high = read_u8();
    return (high << 8U) | read_u8();
  }

  span<const uint8_t> read_bytes(size_t nof_bytes)
  {
    span<const uint8_t> bytes = data.first(std::min(nof_bytes, data.size()));
    data                      = data.last(data.size() - bytes.size());
    return bytes;
  }

private:
  span<const uint8_t> data;
};

/// Operations of the input, selected by the low bits of the operation byte.
enum class operation : uint8_t {
  uplink_slot,
  prach_occasion,
  uplane_frame,
  raw_frame,
  advance_symbols,
  nof_operations
};

/// Selects the slot that the harness writes into the header of a U-Plane message.
enum class slot_override : uint8_t { none, last_uplink_slot, last_prach_slot, ota_slot };

/// Decodes a compression configuration byte. See tests/fuzz/README.md for the bit layout.
ru_compression_params decode_compression(uint8_t byte)
{
  if ((byte & 0x80U) != 0) {
    return {compression_type::BFP, bfp_widths[(byte & 0x0fU) % bfp_widths.size()]};
  }
  return {compression_type::none, MIN_IQ_WIDTH + (byte & 0x0fU) % (MAX_IQ_WIDTH - MIN_IQ_WIDTH + 1)};
}

/// Decodes the configuration header of the input into a sector configuration. See tests/fuzz/README.md.
sector_configuration decode_sector_configuration(input_reader& input)
{
  uint8_t flags      = input.read_u8();
  uint8_t ul_compr   = input.read_u8();
  uint8_t prach_comp = input.read_u8();
  uint8_t misc       = input.read_u8();

  sector_configuration cfg;
  cfg.sector_id                       = 0;
  cfg.interface                       = "fuzz";
  cfg.is_promiscuous_mode_enabled     = false;
  cfg.is_link_status_check_enabled    = false;
  cfg.mtu_size                        = mtu_size;
  cfg.mac_dst_address                 = ru_mac;
  cfg.mac_src_address                 = du_mac;
  cfg.tx_window_timing_params         = {13, 8, 8, 8, 9, 2};
  cfg.rx_window_timing_params         = rx_window;
  cfg.cp                              = cyclic_prefix::NORMAL;
  cfg.is_prach_control_plane_enabled  = (flags & 0x01U) != 0;
  cfg.ignore_prach_start_symbol       = (flags & 0x02U) != 0;
  cfg.ignore_ecpri_payload_size_field = (flags & 0x04U) != 0;
  cfg.ignore_ecpri_seq_id_field       = (flags & 0x08U) != 0;
  cfg.are_metrics_enabled             = (flags & 0x10U) != 0;
  // The flag selects the dynamic compression header, the configuration flag enables the static one.
  cfg.is_uplink_static_compr_hdr_enabled   = (flags & 0x20U) == 0;
  cfg.is_downlink_static_compr_hdr_enabled = true;
  cfg.scs                      = (flags & 0x40U) != 0 ? subcarrier_spacing::kHz30 : subcarrier_spacing::kHz15;
  cfg.bw                       = cell_bandwidths[(misc >> 2U) & 0x03U];
  cfg.ru_operating_bw          = (flags & 0x80U) != 0 ? wide_ru_bandwidth : cfg.bw;
  cfg.ul_compression_params    = decode_compression(ul_compr);
  cfg.prach_compression_params = decode_compression(prach_comp);
  cfg.iq_scaling               = 1.0F;
  if ((misc & 0x10U) != 0) {
    cfg.ul_fs_offset = misc >> 5U;
  }
  cfg.max_processing_delay_slots = max_processing_delay_slots;
  cfg.nof_antennas_ul            = (misc & 0x03U) + 1;
  cfg.uses_dpdk                  = false;

  cfg.ul_eaxc.assign(ul_eaxc.begin(), ul_eaxc.begin() + cfg.nof_antennas_ul);
  cfg.prach_eaxc.assign(prach_eaxc.begin(), prach_eaxc.begin() + cfg.nof_antennas_ul);

  // The downlink path is not built and no downlink C-Plane message is sent, so the downlink parameters only fill the
  // configuration. They take the uplink values, as in the default RU configuration.
  cfg.dl_compression_params = cfg.ul_compression_params;
  cfg.dl_eaxc.assign(ul_eaxc.begin(), ul_eaxc.begin() + cfg.nof_antennas_ul);

  return cfg;
}

// ---------------------------------------------------------------------------
// Test doubles
// ---------------------------------------------------------------------------

/// Error notifier that ignores the late request notifications.
class null_error_notifier : public error_notifier
{
public:
  void on_late_downlink_message(const error_context& /*context*/) override {}
  void on_late_uplink_message(const error_context& /*context*/) override {}
  void on_late_prach_message(const error_context& /*context*/) override {}
};

/// Received Ethernet frame. The frame is held in its own allocation so that reads past its end are reported by ASan.
class fuzz_rx_buffer : public ether::rx_buffer
{
public:
  explicit fuzz_rx_buffer(std::vector<uint8_t> frame_) : frame(std::move(frame_)) {}

  span<const uint8_t> data() const override { return frame; }

private:
  std::vector<uint8_t> frame;
};

/// Ethernet receiver delivering the frames of the input to the frame notifier the OFH receiver starts it with.
class fuzz_eth_receiver : public ether::receiver,
                          public ether::receiver_operation_controller,
                          private ether::receiver_metrics_collector
{
public:
  ether::receiver_operation_controller& get_operation_controller() override { return *this; }
  ether::receiver_metrics_collector*    get_metrics_collector() override { return this; }

  void start(ether::frame_notifier& notifier_) override { notifier = &notifier_; }
  void stop() override { notifier = nullptr; }

  /// Delivers a frame to the OFH receiver.
  void receive(std::vector<uint8_t> frame)
  {
    if (notifier != nullptr) {
      notifier->on_new_frame(ether::unique_rx_buffer(fuzz_rx_buffer(std::move(frame))));
    }
  }

private:
  void collect_metrics(ether::receiver_metrics& /*metric*/) override {}

  ether::frame_notifier* notifier = nullptr;
};

/// \brief Reports a broken invariant and aborts, so that the fuzzer records the input as a crash.
[[noreturn]] void report_invariant_violation(const char* what, slot_point slot)
{
  std::fprintf(stderr, "OFH receiver fuzzer: %s for slot %u\n", what, slot.count());
  // Write the pending log entries, which are lost on abort, for when the input is replayed with OFH_FUZZ_DEBUG_LOG.
  ocudulog::flush();
  std::abort();
}

/// \brief PHY-facing notifier checking that every notification refers to a slot the PHY has requested.
///
/// The receiver must only notify what it holds a context for, so a notification for a slot that was never requested
/// means that a context was created or indexed from received data.
class checking_rx_symbol_notifier : public uplane_rx_symbol_notifier
{
public:
  void add_uplink_slot(slot_point slot) { uplink_slots.insert(slot.count()); }
  void add_prach_slot(slot_point slot) { prach_slots.insert(slot.count()); }

  void on_new_uplink_symbol(const uplane_rx_symbol_context& context,
                            shared_resource_grid /*grid*/,
                            bool /*is_valid*/) override
  {
    if (uplink_slots.count(context.slot.count()) == 0) {
      report_invariant_violation("uplink symbol notified without an uplink request", context.slot);
    }
  }

  void on_new_prach_window_data(const prach_buffer_context& context, shared_prach_buffer /*buffer*/) override
  {
    if (prach_slots.count(context.slot.count()) == 0) {
      report_invariant_violation("PRACH window notified without a PRACH request", context.slot);
    }
  }

private:
  std::set<uint32_t> uplink_slots;
  std::set<uint32_t> prach_slots;
};

// ---------------------------------------------------------------------------
// Sector under test
// ---------------------------------------------------------------------------

/// \brief Returns the Ethernet frame pool of the uplink C-Plane messages, emptied.
///
/// The pool allocates its buffers for every symbol of ten slots and every supported eAxC, which takes longer than the
/// rest of the receive path to build, so it is built once and emptied for every input instead. Emptying it releases
/// every buffer and resets its expired frame counter, which leaves it as a new pool.
std::shared_ptr<ether::eth_frame_pool> get_empty_ul_cplane_frame_pool(slot_symbol_point start)
{
  static std::shared_ptr<ether::eth_frame_pool> pool = std::make_shared<ether::eth_frame_pool>(
      ocudulog::fetch_basic_logger("OFH"), mtu_size, 1, message_type::control_plane, data_direction::uplink);

  for (unsigned i = 0, e = ether::eth_frame_pool::pool_size_in_slots() * start.get_nof_symbols(); i != e; ++i) {
    static_vector<ether::scoped_frame_buffer, ether::MAX_TX_BURST_SIZE> burst;
    while (!pool->enqueue_pending_into_burst(start + i, burst)) {
      burst.clear();
    }
  }
  pool->clear_slot(start.get_slot(), 0);

  return pool;
}

/// Generates the receiver configuration of the given sector configuration, as create_ofh_sector() does.
receiver_config generate_receiver_config(const sector_configuration& cfg)
{
  return {.sector     = cfg.sector_id,
          .scs        = cfg.scs,
          .cp         = cfg.cp,
          .prach_eaxc = {cfg.prach_eaxc.begin(), cfg.prach_eaxc.begin() + cfg.nof_antennas_ul},
          .ul_eaxc    = {cfg.ul_eaxc.begin(), cfg.ul_eaxc.begin() + cfg.nof_antennas_ul},
          // In reception, the destination and source addresses are swapped.
          .mac_dst_address                    = cfg.mac_src_address,
          .mac_src_address                    = cfg.mac_dst_address,
          .rx_timing_params                   = cfg.rx_window_timing_params,
          .ru_operating_bw                    = cfg.ru_operating_bw,
          .ul_compression_params              = cfg.ul_compression_params,
          .prach_compression_params           = cfg.prach_compression_params,
          .is_uplink_static_compr_hdr_enabled = cfg.is_uplink_static_compr_hdr_enabled,
          .ul_fs_offset                       = cfg.ul_fs_offset,
          .is_prach_control_plane_enabled     = cfg.is_prach_control_plane_enabled,
          .ignore_prach_start_symbol          = cfg.ignore_prach_start_symbol,
          .ignore_ecpri_payload_size_field    = cfg.ignore_ecpri_payload_size_field,
          .ignore_ecpri_seq_id_field          = cfg.ignore_ecpri_seq_id_field,
          .are_metrics_enabled                = cfg.are_metrics_enabled,
          .log_unreceived_ru_frames           = cfg.log_unreceived_ru_frames};
}

/// \brief Creates the uplink C-Plane data flow, as the transmitter factory does.
///
/// The uplink request handler stores the C-Plane contexts that the receiver filters the U-Plane messages against
/// through this data flow.
std::unique_ptr<data_flow_cplane_scheduling_commands>
create_ul_cplane_data_flow(const sector_configuration&                       cfg,
                           std::shared_ptr<ether::eth_frame_pool>            frame_pool,
                           std::shared_ptr<uplink_cplane_context_repository> ul_cp_repo,
                           std::shared_ptr<uplink_cplane_context_repository> prach_cp_repo)
{
  const data_flow_cplane_scheduling_commands_impl_config config = {
      .sector                = cfg.sector_id,
      .ru_nof_prbs           = static_cast<unsigned>(get_max_Nprb(cfg.ru_operating_bw, cfg.scs, frequency_range::FR1)),
      .cp                    = cfg.cp,
      .dl_compr_params       = cfg.dl_compression_params,
      .ul_compr_params       = cfg.ul_compression_params,
      .prach_compr_params    = cfg.prach_compression_params,
      .c_plane_prach_fft_len = cfg.c_plane_prach_fft_len};

  const ether::vlan_frame_params ether_params = {.mac_dst_address = cfg.mac_dst_address,
                                                 .mac_src_address = cfg.mac_src_address,
                                                 .eth_type        = ether::ECPRI_ETH_TYPE};

  data_flow_cplane_scheduling_commands_impl_dependencies dependencies = {
      .logger                    = &ocudulog::fetch_basic_logger("OFH"),
      .ul_cplane_context_repo    = std::move(ul_cp_repo),
      .prach_cplane_context_repo = std::move(prach_cp_repo),
      .frame_pool                = std::move(frame_pool),
      .eth_builder               = ether::create_frame_builder(ether_params),
      .ecpri_builder             = ecpri::create_ecpri_packet_builder(),
      .cp_builder                = cfg.is_uplink_static_compr_hdr_enabled
                                       ? create_ofh_control_plane_static_compression_message_builder()
                                       : create_ofh_control_plane_dynamic_compression_message_builder()};

  auto data_flow = std::make_unique<data_flow_cplane_scheduling_commands_impl>(config, std::move(dependencies));
  if (!cfg.are_metrics_enabled) {
    return data_flow;
  }
  return std::make_unique<data_flow_cplane_metrics_decorator>(std::move(data_flow));
}

/// \brief Uplink buffers of the PHY, for one number of antennas and one cell bandwidth.
///
/// Creating the buffers takes longer than replaying most inputs, so they are created once and reused by every input
/// with the same configuration. An input releases every buffer it requested before the next input starts, and the
/// contents of a buffer do not change how the receiver handles a message.
struct phy_buffer_pools {
  std::unique_ptr<resource_grid_pool> grids;
  std::unique_ptr<prach_buffer_pool>  long_prach;
  std::unique_ptr<prach_buffer_pool>  short_prach;
};

/// Returns the uplink buffers for the given number of antennas and number of PRBs.
phy_buffer_pools& get_phy_buffer_pools(unsigned nof_antennas, unsigned nof_prbs)
{
  static std::map<std::pair<unsigned, unsigned>, phy_buffer_pools> pools;

  phy_buffer_pools& entry = pools[{nof_antennas, nof_prbs}];
  if (entry.grids) {
    return entry;
  }

  std::shared_ptr<resource_grid_factory>      factory = create_resource_grid_factory();
  std::vector<std::unique_ptr<resource_grid>> grids;
  std::vector<std::unique_ptr<prach_buffer>>  long_prach;
  std::vector<std::unique_ptr<prach_buffer>>  short_prach;
  for (unsigned i = 0; i != nof_buffers; ++i) {
    grids.push_back(factory->create(nof_antennas, MAX_NSYMB_PER_SLOT, nof_prbs * NOF_SUBCARRIERS_PER_RB));
    long_prach.push_back(create_prach_buffer_long(nof_antennas, 1));
    short_prach.push_back(create_prach_buffer_short(nof_antennas, max_nof_prach_td_occasions, 1));
  }
  entry.grids       = create_generic_resource_grid_pool(std::move(grids));
  entry.long_prach  = std::make_unique<prach_buffer_pool>(long_prach);
  entry.short_prach = std::make_unique<prach_buffer_pool>(short_prach);
  return entry;
}

/// OFH sector built for one input, together with the state the harness keeps to replay the input.
class fuzz_sector
{
public:
  explicit fuzz_sector(const sector_configuration& cfg_) :
    cfg(cfg_),
    nof_symbols(get_nsymb_per_slot(cfg.cp)),
    nof_prbs(get_max_Nprb(cfg.bw, cfg.scs, frequency_range::FR1)),
    phy_pools(get_phy_buffer_pools(cfg.nof_antennas_ul, nof_prbs)),
    ota(slot_point(cfg.scs, 1, 0), 0, nof_symbols),
    last_uplink_slot(ota.get_slot()),
    last_prach_slot(ota.get_slot())
  {
    ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("OFH");

    // Repositories shared by the receiver and the uplink request handler, sized as create_ofh_sector() does.
    unsigned repository_size = calculate_repository_size(cfg.scs, cfg.max_processing_delay_slots * 4);
    auto     ul_cp_repo      = std::make_shared<uplink_cplane_context_repository>(repository_size);
    auto     prach_cp_repo   = std::make_shared<uplink_cplane_context_repository>(repository_size);
    auto     prach_repo      = std::make_shared<prach_context_repository>(repository_size);
    auto     ul_repo         = std::make_shared<uplink_context_repository>(repository_size);
    auto     notified_repo   = std::make_shared<uplink_notified_grid_symbol_repository>(repository_size);

    auto eth_rx_ptr = std::make_unique<fuzz_eth_receiver>();
    eth_rx          = eth_rx_ptr.get();
    rx              = create_receiver(generate_receiver_config(cfg),
                         logger,
                         executor,
                         std::move(eth_rx_ptr),
                         &rx_notifier,
                         prach_repo,
                         ul_repo,
                         ul_cp_repo,
                         prach_cp_repo,
                         notified_repo);

    frame_pool = get_empty_ul_cplane_frame_pool(ota);
    ul_handler = std::make_unique<uplink_request_handler_impl>(
        uplink_request_handler_impl_config{.sector                        = cfg.sector_id,
                                           .is_prach_cp_enabled           = cfg.is_prach_control_plane_enabled,
                                           .prach_eaxc                    = cfg.prach_eaxc,
                                           .ul_data_eaxc                  = cfg.ul_eaxc,
                                           .tdd_config                    = cfg.tdd_config,
                                           .cp                            = cfg.cp,
                                           .scs                           = cfg.scs,
                                           .tx_timing_params              = cfg.tx_window_timing_params,
                                           .enable_log_warnings_for_lates = cfg.enable_log_warnings_for_lates},
        uplink_request_handler_impl_dependencies{
            .logger               = logger,
            .err_notifier         = err_notifier,
            .ul_slot_repo         = ul_repo,
            .ul_prach_repo        = prach_repo,
            .notifier_symbol_repo = notified_repo,
            .data_flow            = create_ul_cplane_data_flow(cfg, frame_pool, ul_cp_repo, prach_cp_repo),
            .frame_pool           = frame_pool});

    rx->get_operation_controller().start();

    // Notify the first OTA symbol, as the timing manager does before any request.
    notify_symbol();
  }

  ~fuzz_sector() { rx->get_operation_controller().stop(); }

  /// Replays the operations of the input.
  void run(input_reader& input)
  {
    for (unsigned i = 0; i != max_nof_operations && !input.empty(); ++i) {
      uint8_t op = input.read_u8();
      switch (static_cast<operation>((op & 0x07U) % static_cast<uint8_t>(operation::nof_operations))) {
        case operation::uplink_slot:
          request_uplink_slot(input);
          break;
        case operation::prach_occasion:
          request_prach_occasion(input);
          break;
        case operation::uplane_frame:
          receive_uplane_frame(input);
          break;
        case operation::raw_frame:
          eth_rx->receive(to_vector(input.read_bytes(input.read_u16())));
          break;
        case operation::advance_symbols:
        default:
          advance(input.read_u8() + 1U);
          break;
      }
    }

    // Close every reception window that is still open, so that the pending contexts are notified.
    advance(cfg.max_processing_delay_slots * nof_symbols + 2 * nof_symbols + rx_window.sym_end + 1);
  }

private:
  static std::vector<uint8_t> to_vector(span<const uint8_t> bytes) { return {bytes.begin(), bytes.end()}; }

  /// \brief Notifies the current OTA symbol, as the timing manager does.
  ///
  /// The uplink C-Plane messages of the symbol are then taken from the frame pool and discarded, in place of the
  /// transmitter sending them to the RU.
  void notify_symbol()
  {
    slot_symbol_point_context context{.symbol_point = ota, .hfn = 0, .time_point = {}};
    if (ota_symbol_boundary_notifier* rx_ota_notifier = rx->get_ota_symbol_boundary_notifier()) {
      rx_ota_notifier->on_new_symbol(context);
    }
    ul_handler->get_ota_symbol_boundary_notifier().on_new_symbol(context);

    static_vector<ether::scoped_frame_buffer, ether::MAX_TX_BURST_SIZE> burst;
    while (!frame_pool->enqueue_pending_into_burst(ota, burst)) {
      burst.clear();
    }
  }

  /// Advances the OTA time by the given number of symbols.
  void advance(unsigned nof_advanced_symbols)
  {
    for (unsigned i = 0; i != nof_advanced_symbols; ++i) {
      ota += 1;
      notify_symbol();
    }
  }

  /// \brief Requests an uplink slot the given number of slots after the current OTA slot, as the PHY does.
  ///
  /// The request may be one slot further ahead than the processing delay allows, which reuses a repository entry
  /// that may still be in use.
  void request_uplink_slot(input_reader& input)
  {
    slot_point           slot = ota.get_slot() + input.read_u8() % (max_processing_delay_slots + 2);
    shared_resource_grid grid = phy_pools.grids->allocate_resource_grid(slot);
    if (!grid) {
      return;
    }

    rx_notifier.add_uplink_slot(slot);
    last_uplink_slot = slot;
    ul_handler->handle_new_uplink_slot({slot, cfg.sector_id}, grid);
  }

  /// Requests a PRACH occasion the given number of slots after the current OTA slot, with a PRACH configuration the
  /// PHY can produce.
  void request_prach_occasion(input_reader& input)
  {
    slot_point        slot      = ota.get_slot() + input.read_u8() % (max_processing_delay_slots + 2);
    prach_format_type format    = prach_formats[input.read_u8() % prach_formats.size()];
    uint8_t           start     = input.read_u8();
    uint8_t           rb_offset = input.read_u8();
    uint8_t           nof_td    = input.read_u8();

    bool                       is_long = is_long_preamble(format);
    prach_preamble_information preamble_info =
        is_long ? get_prach_preamble_long_info(format)
                : get_prach_preamble_short_info(format, to_ra_subcarrier_spacing(cfg.scs), true);
    unsigned nof_rb_ra = prach_frequency_mapping_get(preamble_info.scs, cfg.scs).nof_rb_ra;
    if (nof_rb_ra > nof_prbs) {
      return;
    }

    prach_buffer_context context = {};
    for (unsigned i = 0; i != cfg.nof_antennas_ul; ++i) {
      context.ports.push_back(i);
    }
    context.slot             = slot;
    context.nof_prb_ul_grid  = nof_prbs;
    context.rb_offset        = rb_offset % (nof_prbs - nof_rb_ra + 1);
    context.sector           = cfg.sector_id;
    context.format           = format;
    context.nof_fd_occasions = 1;
    context.pusch_scs        = cfg.scs;
    if (is_long) {
      // Long preambles start at the beginning of the slot in FR1, TS 38.211, table 6.3.3.2-2.
      context.start_symbol     = 0;
      context.nof_td_occasions = 1;
    } else {
      // The occasions of a short preamble must fit in the slot.
      unsigned max_nof_td      = std::min(max_nof_prach_td_occasions, nof_symbols / preamble_info.nof_symbols);
      context.nof_td_occasions = 1 + nof_td % max_nof_td;
      context.start_symbol     = start % (nof_symbols - context.nof_td_occasions * preamble_info.nof_symbols + 1);
    }

    shared_prach_buffer buffer = (is_long ? phy_pools.long_prach : phy_pools.short_prach)->get();
    if (!buffer) {
      return;
    }

    rx_notifier.add_prach_slot(slot);
    last_prach_slot = slot;
    ul_handler->handle_prach_occasion(context, std::move(buffer));
  }

  /// Writes the given slot into the header of an Open Fronthaul U-Plane message, O-RAN.WG4.CUS, clause 7.5.2.
  static void write_slot(span<uint8_t> message, slot_point slot)
  {
    unsigned slot_id = slot.subframe_slot_index();
    message[1]       = slot.sfn() & 0xffU;
    message[2]       = (slot.subframe_index() << 4U) | (slot_id >> 2U);
    message[3]       = ((slot_id & 0x03U) << 6U) | (message[3] & 0x3fU);
  }

  /// \brief Receives a U-Plane message of the input in a valid Ethernet frame and eCPRI header.
  ///
  /// The harness writes the headers that the receiver filters on, so that the fuzzer mutates the Open Fronthaul
  /// message rather than the addresses. The raw frame operation covers the filters.
  void receive_uplane_frame(input_reader& input)
  {
    uint8_t eaxc_selector = input.read_u8();
    uint8_t flags         = input.read_u8();

    unsigned eaxc;
    if ((eaxc_selector & 0x80U) != 0) {
      eaxc = eaxc_selector & 0x7fU;
    } else {
      unsigned index = eaxc_selector % (2 * cfg.nof_antennas_ul);
      eaxc           = (index < cfg.nof_antennas_ul) ? cfg.ul_eaxc[index] : cfg.prach_eaxc[index - cfg.nof_antennas_ul];
    }

    uint8_t seq_id = ((flags & 0x04U) != 0) ? next_seq_id[eaxc % next_seq_id.size()]++ : input.read_u8();

    std::optional<uint16_t> ecpri_payload_size;
    if ((flags & 0x08U) != 0) {
      ecpri_payload_size = input.read_u16();
    }

    std::vector<uint8_t> message = to_vector(input.read_bytes(input.read_u16()));
    if (message.size() >= 4) {
      switch (static_cast<slot_override>(flags & 0x03U)) {
        case slot_override::last_uplink_slot:
          write_slot(message, last_uplink_slot);
          break;
        case slot_override::last_prach_slot:
          write_slot(message, last_prach_slot);
          break;
        case slot_override::ota_slot:
          write_slot(message, ota.get_slot());
          break;
        case slot_override::none:
        default:
          break;
      }
    }

    // eCPRI payload: PC_ID and SEQ_ID, then the Open Fronthaul message.
    uint16_t payload_size = ecpri_payload_size.value_or(4 + message.size());

    std::vector<uint8_t> frame;
    frame.insert(frame.end(), du_mac.begin(), du_mac.end());
    frame.insert(frame.end(), ru_mac.begin(), ru_mac.end());
    frame.push_back(ether::ECPRI_ETH_TYPE >> 8U);
    frame.push_back(ether::ECPRI_ETH_TYPE & 0xffU);
    // eCPRI common header: revision 1, last packet, IQ data message type.
    frame.push_back(0x10);
    frame.push_back(0x00);
    frame.push_back(payload_size >> 8U);
    frame.push_back(payload_size & 0xffU);
    frame.push_back(eaxc >> 8U);
    frame.push_back(eaxc & 0xffU);
    frame.push_back(seq_id);
    frame.push_back(0x80);
    frame.insert(frame.end(), message.begin(), message.end());
    // Pad to the minimum Ethernet frame size, as the NIC does.
    if (frame.size() < 64) {
      frame.resize(64, 0);
    }

    eth_rx->receive(std::move(frame));
  }

  const sector_configuration                   cfg;
  const unsigned                               nof_symbols;
  const unsigned                               nof_prbs;
  inline_task_executor                         executor;
  null_error_notifier                          err_notifier;
  checking_rx_symbol_notifier                  rx_notifier;
  phy_buffer_pools&                            phy_pools;
  fuzz_eth_receiver*                           eth_rx = nullptr;
  std::unique_ptr<receiver>                    rx;
  std::shared_ptr<ether::eth_frame_pool>       frame_pool;
  std::unique_ptr<uplink_request_handler_impl> ul_handler;
  slot_symbol_point                            ota;
  slot_point                                   last_uplink_slot;
  slot_point                                   last_prach_slot;
  std::array<uint8_t, 128>                     next_seq_id = {};
};

std::once_flag init_flag;

void ensure_logging()
{
  // Started here rather than in LLVMFuzzerInitialize so that the backend thread is created after AFL++ forks.
  std::call_once(init_flag, []() {
    ocudulog::init();
    std::atexit([]() { ocudulog::flush(); });
  });
}

} // namespace

// ---------------------------------------------------------------------------
// LLVMFuzzer interface
// ---------------------------------------------------------------------------

/// Route logs to /dev/null at debug level before any forking occurs.
///
/// The level stays at debug so that the code formatting the decoded fields for the log runs as well.
extern "C" int LLVMFuzzerInitialize(int* /*argc*/, char*** /*argv*/)
{
  if (std::getenv("OFH_FUZZ_DEBUG_LOG") != nullptr) {
    ocudulog::set_default_sink(ocudulog::fetch_stdout_sink());
  } else if (ocudulog::sink* null_sink = ocudulog::create_file_sink("/dev/null"); null_sink != nullptr) {
    ocudulog::set_default_sink(*null_sink);
  }
  ocudulog::fetch_basic_logger("OFH").set_level(ocudulog::basic_levels::debug);
  return 0;
}

/// Entry point called by AFL++ / libFuzzer for each mutated input.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  // A configuration header of four bytes, then the operations.
  if (size < 5) {
    return 0;
  }
  ensure_logging();

  input_reader input({data, size});
  fuzz_sector  harness(decode_sector_configuration(input));
  harness.run(input);

  return 0;
}
