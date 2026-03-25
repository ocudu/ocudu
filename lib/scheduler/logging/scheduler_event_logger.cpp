// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "scheduler_event_logger.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/type_list_buffer.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/csi_report/csi_report_formatters.h"
#include "ocudu/ran/pusch/pusch_tpmi_formatter.h"
#include "ocudu/support/format/custom_formattable.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"

using namespace ocudu;

namespace {

using sel = scheduler_event_logger;

struct cell_creation_event {
  du_cell_index_t cell_index;
};

/// Storage type of events taking place during a slot.
using slot_event_buffer = type_list_buffer_stream<cell_creation_event,
                                                  sel::prach_event,
                                                  rach_indication_message,
                                                  sel::ue_creation_event,
                                                  sel::ue_reconf_event,
                                                  sched_ue_delete_message,
                                                  sel::ue_cfg_applied_event,
                                                  sel::ue_deactivation_event,
                                                  sel::error_indication_event,
                                                  sr_event,
                                                  csi_report_event,
                                                  sel::bsr_event,
                                                  harq_ack_event,
                                                  sel::crc_event,
                                                  dl_mac_ce_indication,
                                                  dl_buffer_state_indication_message,
                                                  sel::phr_event,
                                                  sel::srs_indication_event,
                                                  sel::slice_reconfiguration_event>;

/// Sentinel return type used by format_info_level to signal that an event has no info-level formatter.
struct no_info_formatter {};

/// Helper function to format a given event in info level.
template <typename FormatContext, typename Event>
auto format_info_level(FormatContext& ctx, const Event& ev, bool first)
{
  const char* separator = first ? " " : ", ";
  if constexpr (std::is_same_v<Event, cell_creation_event>) {
    fmt::format_to(ctx.out(), "{}Cell creation idx={}", separator, fmt::underlying(ev.cell_index));
  } else if constexpr (std::is_same_v<Event, sel::prach_event>) {
    fmt::format_to(ctx.out(),
                   "{}prach({}={} preamble={} tc-rnti={})",
                   separator,
                   ev.is_msga ? "msgb-rnti" : "ra-rnti",
                   ev.ra_rnti,
                   ev.preamble_id,
                   ev.tc_rnti);
  } else if constexpr (std::is_same_v<Event, rach_indication_message>) {
    fmt::format_to(ctx.out(), "{}RACH Ind slot_rx={}", separator, ev.slot_rx);
  } else if constexpr (std::is_same_v<Event, sel::error_indication_event>) {
    fmt::format_to(ctx.out(), "{}ErrorIndication slot={}", separator, ev.sl_tx);
  } else {
    // Signal via return that there is no formatter for this event type.
    return no_info_formatter{};
  }
}

/// Metafunction to check if event type has an info level formatter.
template <typename Event>
constexpr bool has_info_formatter()
{
  using return_t =
      decltype(format_info_level(std::declval<fmt::format_context&>(), std::declval<const Event&>(), true));
  return not std::is_same_v<return_t, no_info_formatter>;
}

/// Helper function to format a given event in debug level.
template <typename FormatContext, typename Event>
void format_debug_level(FormatContext& ctx, const Event& ev)
{
  if constexpr (std::is_same_v<Event, cell_creation_event>) {
    fmt::format_to(ctx.out(), "\n- Cell creation: idx={}", fmt::underlying(ev.cell_index));
  } else if constexpr (std::is_same_v<Event, sel::prach_event>) {
    fmt::format_to(ctx.out(),
                   "\n- PRACH: slot={} preamble={} {}={} temp_crnti={} ta_cmd={}",
                   ev.slot_rx,
                   ev.preamble_id,
                   ev.is_msga ? "msgb-rnti" : "ra-rnti",
                   ev.ra_rnti,
                   ev.tc_rnti,
                   ev.ta);
  } else if constexpr (std::is_same_v<Event, rach_indication_message>) {
    fmt::format_to(ctx.out(), "\n- RACH ind: slot_rx={} PRACHs: ", ev.slot_rx);
    unsigned count = 0;
    for (unsigned i = 0, sz = ev.occasions.size(); i != sz; ++i) {
      for (unsigned j = 0, e = ev.occasions[i].preambles.size(); j != e; ++j) {
        const auto& preamble = ev.occasions[i].preambles[j];
        fmt::format_to(ctx.out(),
                       "{}{}: preamble={} tc-rnti={} freq_idx={} start_symbol={} TA={}s",
                       count == 0 ? "" : ", ",
                       count,
                       preamble.preamble_id,
                       preamble.tc_rnti,
                       ev.occasions[i].frequency_index,
                       ev.occasions[i].start_symbol,
                       preamble.time_advance.to_seconds());
        count++;
      }
    }
  } else if constexpr (std::is_same_v<Event, sel::ue_creation_event>) {
    fmt::format_to(ctx.out(), "\n- UE creation: ue={} rnti={}", fmt::underlying(ev.ue_index), ev.rnti);
  } else if constexpr (std::is_same_v<Event, sel::ue_reconf_event>) {
    fmt::format_to(ctx.out(), "\n- UE reconfiguration: ue={} rnti={}", fmt::underlying(ev.ue_index), ev.rnti);
  } else if constexpr (std::is_same_v<Event, sched_ue_delete_message>) {
    fmt::format_to(ctx.out(), "\n- UE removal: ue={} rnti={}", fmt::underlying(ev.ue_index), ev.crnti);
  } else if constexpr (std::is_same_v<Event, sel::ue_cfg_applied_event>) {
    fmt::format_to(ctx.out(), "\n- UE dedicated config applied: ue={} rnti={}", fmt::underlying(ev.ue_index), ev.rnti);
  } else if constexpr (std::is_same_v<Event, sel::ue_deactivation_event>) {
    fmt::format_to(ctx.out(), "\n- UE deactivation: ue={} rnti={}", fmt::underlying(ev.ue_index), ev.rnti);
  } else if constexpr (std::is_same_v<Event, sel::error_indication_event>) {
    fmt::format_to(ctx.out(),
                   "\n- ErrorIndication: slot={}{}{}{}",
                   ev.sl_tx,
                   ev.outcome.pdcch_discarded ? ", PDCCH discarded" : "",
                   ev.outcome.pdsch_discarded ? ", PDSCH discarded" : "",
                   ev.outcome.pusch_and_pucch_discarded ? ", PUSCH and PUCCH discarded" : "");
  } else if constexpr (std::is_same_v<Event, sr_event>) {
    fmt::format_to(ctx.out(), "\n- SR: ue={} rnti={}", fmt::underlying(ev.ue_index), ev.rnti);
  } else if constexpr (std::is_same_v<Event, csi_report_event>) {
    fmt::format_to(ctx.out(), "\n- CSI: ue={} rnti={}: slot_rx={}", fmt::underlying(ev.ue_index), ev.rnti, ev.sl_rx);
    if (not ev.csi.first_tb_wideband_cqi.has_value() and not ev.csi.ri.has_value() and not ev.csi.pmi.has_value()) {
      fmt::format_to(ctx.out(), " invalid");
      return;
    }
    if (ev.csi.first_tb_wideband_cqi.has_value()) {
      fmt::format_to(ctx.out(), " cqi={}", *ev.csi.first_tb_wideband_cqi);
    }
    if (ev.csi.ri.has_value()) {
      fmt::format_to(ctx.out(), " ri={}", ev.csi.ri.value());
    }
    if (ev.csi.pmi.has_value()) {
      fmt::format_to(ctx.out(), " {}", *ev.csi.pmi);
    }
  } else if constexpr (std::is_same_v<Event, sel::bsr_event>) {
    fmt::format_to(ctx.out(),
                   "\n- BSR: ue={} rnti={} type=\"{}\" report={{",
                   fmt::underlying(ev.ue_index),
                   ev.rnti,
                   to_string(ev.type));
    if (ev.type == bsr_format::LONG_BSR or ev.type == bsr_format::LONG_TRUNC_BSR or ev.reported_lcgs.full()) {
      std::array<int, MAX_NOF_LCGS> report;
      std::fill(report.begin(), report.end(), -1);
      for (unsigned i = 0, sz = ev.reported_lcgs.size(); i != sz; ++i) {
        report[ev.reported_lcgs[i].lcg_id] = ev.reported_lcgs[i].nof_bytes;
      }
      for (unsigned i = 0; i != MAX_NOF_LCGS; ++i) {
        fmt::format_to(ctx.out(), "{}", i == 0 ? "" : ", ");
        if (report[i] < 0) {
          fmt::format_to(ctx.out(), "-");
        } else {
          fmt::format_to(ctx.out(), "{}", report[i]);
        }
      }
    } else {
      for (unsigned i = 0, sz = ev.reported_lcgs.size(); i != sz; ++i) {
        fmt::format_to(ctx.out(),
                       "{}{}: {}",
                       i == 0 ? "" : " ",
                       fmt::underlying(ev.reported_lcgs[i].lcg_id),
                       ev.reported_lcgs[i].nof_bytes);
      }
    }
    fmt::format_to(ctx.out(), "}} pending_bytes={}", ev.tot_ul_pending_bytes);
  } else if constexpr (std::is_same_v<Event, harq_ack_event>) {
    fmt::format_to(ctx.out(),
                   "\n- HARQ-ACK: ue={} rnti={} slot_rx={} h_id={} ack={}",
                   fmt::underlying(ev.ue_index),
                   ev.rnti,
                   ev.sl_ack_rx,
                   fmt::underlying(ev.h_id),
                   (unsigned)ev.ack);
    if (ev.ack == mac_harq_ack_report_status::ack) {
      fmt::format_to(ctx.out(), " tbs={}", ev.tbs);
    }
  } else if constexpr (std::is_same_v<Event, sel::crc_event>) {
    if (ev.ul_sinr_db.has_value()) {
      fmt::format_to(ctx.out(),
                     "\n- CRC: ue={} rnti={} rx_slot={} h_id={} crc={} sinr={:.3}dB",
                     fmt::underlying(ev.ue_index),
                     ev.rnti,
                     ev.sl_rx,
                     fmt::underlying(ev.h_id),
                     ev.crc,
                     ev.ul_sinr_db.value());
    } else {
      fmt::format_to(ctx.out(),
                     "\n- CRC: ue={} rnti={} rx_slot={} h_id={} crc={} sinr=N/A",
                     fmt::underlying(ev.ue_index),
                     ev.rnti,
                     ev.sl_rx,
                     fmt::underlying(ev.h_id),
                     ev.crc);
    }
  } else if constexpr (std::is_same_v<Event, dl_mac_ce_indication>) {
    fmt::format_to(ctx.out(), "\n- MAC CE: ue={} lcid={}", fmt::underlying(ev.ue_index), ev.ce_lcid.value());
  } else if constexpr (std::is_same_v<Event, dl_buffer_state_indication_message>) {
    fmt::format_to(ctx.out(),
                   "\n- RLC Buffer State: ue={} lcid={} pending_bytes={}",
                   fmt::underlying(ev.ue_index),
                   fmt::underlying(ev.lcid),
                   ev.bs);
  } else if constexpr (std::is_same_v<Event, sel::phr_event>) {
    fmt::format_to(ctx.out(), "\n- PHR: ue={} rnti={} ph={}dB", fmt::underlying(ev.ue_index), ev.rnti, ev.ph);
    if (ev.p_cmax.has_value()) {
      fmt::format_to(ctx.out(), " p_cmax={}dBm", ev.p_cmax.value());
    }
  } else if constexpr (std::is_same_v<Event, sel::srs_indication_event>) {
    fmt::format_to(ctx.out(), "\n- SRS: ue={} rnti={}", fmt::underlying(ev.ue_index), ev.rnti);
    if (ev.tpmi_info.has_value()) {
      fmt::format_to(ctx.out(), " tpmi_info=[{:;}]", ev.tpmi_info.value());
    }
  } else if constexpr (std::is_same_v<Event, sel::slice_reconfiguration_event>) {
    fmt::format_to(ctx.out(), "\n- Slice Reconfig: cell={}", fmt::underlying(ev.cell_index));
  } else {
    report_fatal_error("Detected event with no formatter");
  }
}

} // namespace

namespace fmt {

/// Formatter of slot_event_list that supports both info and debug modes.
template <>
struct formatter<slot_event_buffer> {
  bool debug = false;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    auto it = ctx.begin();
    while (it != ctx.end() and *it != '}') {
      if (*it == 'd') {
        debug = true;
      }
      ++it;
    }
    return it;
  }

  template <typename FormatContext>
  auto format(const slot_event_buffer& obj, FormatContext& ctx) const
  {
    if (debug) {
      obj.for_each([&ctx](const auto& ev) mutable { format_debug_level(ctx, ev); });
    } else {
      obj.for_each([&ctx, first = true](const auto& ev) mutable {
        format_info_level(ctx, ev, first);
        first = false;
      });
    }
    return ctx.out();
  }
};

} // namespace fmt

/// Class that handles the pool of formattable slot events.
class scheduler_event_logger::event_buffer_writer
{
public:
  event_buffer_writer(mode_t mode_) :
    mode(mode_),
    cur_buffer(mode == mode_t::none ? slot_event_buffer{}
                                    : *slot_event_buffer::make(get_default_fallback_byte_buffer_segment_pool()))
  {
  }

  template <typename EventType>
  void push(const EventType& ev)
  {
    if (mode == mode_t::none or (mode == mode_t::info and not has_info_formatter<EventType>())) {
      return;
    }
    cur_buffer.push(ev);
  }

  bool empty() const { return mode == mode_t::none or cur_buffer.empty(); }

  /// Extract a formattable event list.
  slot_event_buffer pop_event_list()
  {
    if (empty()) {
      return slot_event_buffer();
    }

    // Start a new current buffer.
    auto ret   = std::move(cur_buffer);
    cur_buffer = *slot_event_buffer::make(get_default_fallback_byte_buffer_segment_pool());
    return ret;
  }

private:
  mode_t            mode = mode_t::none;
  slot_event_buffer cur_buffer;
};

scheduler_event_logger::scheduler_event_logger(du_cell_index_t cell_index_, pci_t pci_) :
  cell_index(cell_index_),
  pci(pci_),
  logger(ocudulog::fetch_basic_logger("SCHED")),
  mode(logger.debug.enabled() ? mode_t::debug : (logger.info.enabled() ? mode_t::info : mode_t::none)),
  slot_buffer_writer(std::make_unique<event_buffer_writer>(mode))
{
  slot_buffer_writer->push(cell_creation_event{cell_index});
}

scheduler_event_logger::~scheduler_event_logger() = default;

void scheduler_event_logger::log_impl()
{
  if (slot_buffer_writer->empty()) {
    return;
  }
  if (mode == mode_t::debug) {
    logger.debug("Processed slot events pci={}:{:d}", pci, slot_buffer_writer->pop_event_list());
  } else if (mode == mode_t::info) {
    logger.info("Processed slot events pci={}:{}", pci, slot_buffer_writer->pop_event_list());
  }
}

void scheduler_event_logger::enqueue_impl(const prach_event& rach_ev)
{
  slot_buffer_writer->push(rach_ev);
}

void scheduler_event_logger::enqueue_impl(const rach_indication_message& rach_ind)
{
  slot_buffer_writer->push(rach_ind);
}

void scheduler_event_logger::enqueue_impl(const ue_creation_event& ue_request)
{
  slot_buffer_writer->push(ue_request);
}

void scheduler_event_logger::enqueue_impl(const ue_reconf_event& ue_request)
{
  slot_buffer_writer->push(ue_request);
}

void scheduler_event_logger::enqueue_impl(const sched_ue_delete_message& ue_request)
{
  slot_buffer_writer->push(ue_request);
}

void scheduler_event_logger::enqueue_impl(const ue_cfg_applied_event& ev)
{
  slot_buffer_writer->push(ev);
}

void scheduler_event_logger::enqueue_impl(const ue_deactivation_event& req)
{
  slot_buffer_writer->push(req);
}

void scheduler_event_logger::enqueue_impl(const error_indication_event& err_ind)
{
  slot_buffer_writer->push(err_ind);
}

void scheduler_event_logger::enqueue_impl(const sr_event& sr)
{
  slot_buffer_writer->push(sr);
}

void scheduler_event_logger::enqueue_impl(const csi_report_event& csi)
{
  slot_buffer_writer->push(csi);
}

void scheduler_event_logger::enqueue_impl(const bsr_event& bsr)
{
  slot_buffer_writer->push(bsr);
}

void scheduler_event_logger::enqueue_impl(const harq_ack_event& harq_ev)
{
  slot_buffer_writer->push(harq_ev);
}

void scheduler_event_logger::enqueue_impl(const crc_event& crc_ev)
{
  slot_buffer_writer->push(crc_ev);
}

void scheduler_event_logger::enqueue_impl(const dl_mac_ce_indication& mac_ce)
{
  slot_buffer_writer->push(mac_ce);
}

void scheduler_event_logger::enqueue_impl(const dl_buffer_state_indication_message& bs)
{
  slot_buffer_writer->push(bs);
}

void scheduler_event_logger::enqueue_impl(const phr_event& phr_ev)
{
  slot_buffer_writer->push(phr_ev);
}

void scheduler_event_logger::enqueue_impl(const srs_indication_event& srs_ev)
{
  slot_buffer_writer->push(srs_ev);
}

void scheduler_event_logger::enqueue_impl(const slice_reconfiguration_event& slice_reconf_ev)
{
  slot_buffer_writer->push(slice_reconf_ev);
}
