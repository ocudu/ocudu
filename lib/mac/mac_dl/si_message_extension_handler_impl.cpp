// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "bcch_dl_sch_encoder.h"
#include "ocudu/adt/spsc_queue.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/units.h"
#include <algorithm>

using namespace ocudu;

/// Payload of zeros sent to when an error occurs.
static const std::vector<uint8_t> zeros_payload(MAX_BCCH_DL_SCH_PDU_SIZE, 0);

namespace {

/// Handles slot-aligned PDU updates for a single SI message, queuing incoming updates and applying them at the
/// correct slot boundary.
class si_message_extension_handler_impl : public si_message_extension_handler
{
  using time_point = std::chrono::system_clock::time_point;
  struct si_pdu_update {
    /// Slot at which this update becomes active. If std::nullopt, it becomes active immediately.
    std::optional<slot_point> slot;
    units::bytes              len;
    bcch_dl_sch_buffer        pdu_buffer;
  };

public:
  explicit si_message_extension_handler_impl(const mac_cell_sys_info_config& req) :
    logger(ocudulog::fetch_basic_logger("MAC"))
  {
    cur_si_msg.resize(req.si_messages.size());

    // An SI grant states which SI messages it carries, rather than the position it holds in the SI epoch it was
    // scheduled with, which does not survive a warning going on and off air.
    for (const si_message_scheduling_config& si_msg : req.si_sched_cfg.si_messages) {
      si_msg_sibs.push_back(si_msg.sibs);
    }

    /// Min si_period is 8 frames (80 ms), with size of 128, we can enqueue SIB19 PDUs for the next 10s.
    static constexpr unsigned max_nof_msgs = 128;
    si_msg_queues.reserve(req.si_messages.size());
    for (unsigned i = 0; i != req.si_messages.size(); ++i) {
      si_msg_queues.emplace_back(std::make_unique<si_msg_queue_type>(max_nof_msgs));
    }
  }

  // See interface for documentation.
  span<const uint8_t> get_pdu(slot_point_extended sl_tx_ext, const sib_information& si_info) override
  {
    ocudu_assert(si_info.pdsch_cfg.codewords.size() == 1, "SIB grants always carry exactly one codeword");
    const unsigned   tbs   = si_info.pdsch_cfg.codewords[0].tb_size_bytes.value();
    const slot_point sl_tx = sl_tx_ext.without_hyper_sfn();

    const auto sibs_it = std::find(si_msg_sibs.begin(), si_msg_sibs.end(), si_info.sibs);
    if (sibs_it == si_msg_sibs.end()) {
      // The SI message this grant carries is not one of the cell.
      return span<const uint8_t>();
    }
    const unsigned idx = std::distance(si_msg_sibs.begin(), sibs_it);

    if (idx >= si_msg_queues.size()) {
      // si_message_handler does not hold a msg queue for the given SI message.
      return span<const uint8_t>();
    }

    if (si_msg_queues[idx]->empty()) {
      // si_message_handler does not hold any PDUs for the given SI message.
      return span<const uint8_t>();
    }

    // Check if we need to move to the SI next version.
    const std::optional<slot_point>& front_slot = si_msg_queues[idx]->front()->slot;
    if (not front_slot.has_value() or sl_tx >= *front_slot) {
      // Pop the current SI PDU.
      if (not si_msg_queues[idx]->try_pop(cur_si_msg[idx])) {
        logger.warning("SI-message idx={} try_pop failed despite non-empty queue", idx);
        return span<const uint8_t>();
      }
    }

    if (cur_si_msg[idx].len.value() == 0) {
      logger.warning("SI-message extension idx={} tbs={} not yet initialized.", idx, tbs);
      return span<const uint8_t>();
    }
    ocudu_assert(cur_si_msg[idx].pdu_buffer, "SI-message idx={} has null PDU buffer after dequeue", idx);

    if (cur_si_msg[idx].len.value() > tbs) {
      logger.warning("Failed to encode SI-message extension idx={}. Cause: "
                     "PDSCH TB size {} is smaller than the "
                     "SI-message length {}",
                     idx,
                     tbs,
                     cur_si_msg[idx].len.value());
      return span<const uint8_t>{zeros_payload}.first(tbs);
    }

    return span<const uint8_t>(cur_si_msg[idx].pdu_buffer->data(), tbs);
  }

  // See interface for documentation.
  bool enqueue_si_pdu_updates(const mac_cell_sys_info_pdu_update& req) override
  {
    if (req.si_msg_idx >= si_msg_queues.size()) {
      return false;
    }

    for (unsigned idx = 0, e = req.si_messages.size(); idx != e; ++idx) {
      std::optional<slot_point> tx_slot;
      if (req.slot.has_value()) {
        tx_slot = *req.slot + idx * req.si_slot_period.value_or(0);
      }
      const byte_buffer& pdu = req.si_messages[idx];
      logger.debug("New SIB{} PDU enqueued for tx_slot: {}, si_msg_idx: {} size: {}",
                   req.sib_idx,
                   tx_slot.has_value() ? fmt::to_string(*tx_slot) : "asap",
                   req.si_msg_idx,
                   static_cast<unsigned>(pdu.length()));
      si_pdu_update sib_pdu_update{
          tx_slot, units::bytes{static_cast<unsigned>(pdu.length())}, make_linear_bcch_dl_sch_buffer(pdu)};
      if (!si_msg_queues[req.si_msg_idx]->try_push(sib_pdu_update)) {
        return false;
      }
    }

    return true;
  }

private:
  ocudulog::basic_logger& logger;

  using si_msg_queue_type = concurrent_queue<si_pdu_update,
                                             concurrent_queue_policy::lockfree_spsc,
                                             concurrent_queue_wait_policy::non_blocking>;
  std::vector<std::unique_ptr<si_msg_queue_type>> si_msg_queues;
  std::vector<si_pdu_update>                      cur_si_msg;

  // SIBs carried by each SI message of the cell, in the order the queues are held in.
  std::vector<sib_type_set> si_msg_sibs;
};
} // namespace

std::unique_ptr<si_message_extension_handler>
ocudu::create_si_message_extension_handler(const mac_cell_sys_info_config& req)
{
  return std::make_unique<si_message_extension_handler_impl>(req);
}
