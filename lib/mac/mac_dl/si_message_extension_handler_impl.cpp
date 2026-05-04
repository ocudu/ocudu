// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "sib_pdu_assembler.h"
#include "ocudu/adt/ring_buffer.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/units.h"

using namespace ocudu;

/// Max SI Message PDU size. This value is implementation-defined.
static constexpr unsigned MAX_BCCH_DL_SCH_PDU_SIZE = 2048;

/// Payload of zeros sent to when an error occurs.
static const std::vector<uint8_t> zeros_payload(MAX_BCCH_DL_SCH_PDU_SIZE, 0);

static std::shared_ptr<const std::vector<uint8_t>> make_linear_buffer(const byte_buffer& pdu)
{
  // Note: We overallocate the SI message buffer to account for padding.
  // Note: resizing after this point invalidates the spans passed to lower layers.
  auto vec = std::make_shared<std::vector<uint8_t>>(MAX_BCCH_DL_SCH_PDU_SIZE, 0);
  copy_segments(pdu, span<uint8_t>(vec->data(), vec->size()));
  return vec;
}

namespace {

/// Handles slot-aligned PDU updates for a single SI message, queuing incoming updates and applying them at the
/// correct slot boundary.
class si_message_extension_handler_impl : public sib_pdu_assembler::message_handler
{
  using time_point         = std::chrono::system_clock::time_point;
  using bcch_dl_sch_buffer = std::shared_ptr<const std::vector<uint8_t>>;
  struct si_pdu_update {
    slot_point         slot;
    units::bytes       len;
    bcch_dl_sch_buffer pdu_buffer;
  };

public:
  explicit si_message_extension_handler_impl(const mac_cell_sys_info_config& req) :
    logger(ocudulog::fetch_basic_logger("MAC"))
  {
    cur_si_msg.resize(req.si_messages.size());
    si_msg_queues.resize(req.si_messages.size());
  }

  // See interface for documentation.
  si_version_type update(si_version_type si_version, const byte_buffer& pdu) override { return 0; }

  // See interface for documentation.
  span<const uint8_t> get_pdu(slot_point_extended sl_tx_ext, const sib_information& si_info) override
  {
    const unsigned   idx   = si_info.si_msg_index.value();
    const unsigned   tbs   = si_info.pdsch_cfg.codewords[0].tb_size_bytes.value();
    const slot_point sl_tx = sl_tx_ext.without_hyper_sfn();

    if (idx >= si_msg_queues.size()) {
      // si_message_handler does not hold a msg queue for the given si_msg_index.
      return span<const uint8_t>();
    }

    if (si_msg_queues[idx].empty()) {
      // si_message_handler does not hold any PDUs for the given si_msg_index.
      return span<const uint8_t>();
    }

    // Check if we need to move to the SI next version.
    if (sl_tx >= si_msg_queues[idx].begin()->slot) {
      // Update the current SI version.
      cur_si_msg[idx] = std::move(*si_msg_queues[idx].begin());
      // Pop the current SI PDU.
      si_msg_queues[idx].pop();
    }

    if (cur_si_msg[idx].len.value() == 0) {
      logger.warning("SI-message extension idx={} tbs={} not yet initialized.", idx, tbs);
      return span<const uint8_t>();
    }

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
      slot_point   tx_slot = req.slot + idx * req.si_slot_period.value_or(0);
      byte_buffer& pdu     = req.si_messages[idx];
      logger.debug("New SIB{} PDU enqueued for tx_slot: {}, si_msg_idx: {} size: {}",
                   req.sib_idx,
                   tx_slot,
                   req.si_msg_idx,
                   static_cast<unsigned>(pdu.length()));
      si_pdu_update sib_pdu_update{tx_slot, units::bytes{static_cast<unsigned>(pdu.length())}, make_linear_buffer(pdu)};
      if (!si_msg_queues[req.si_msg_idx].try_push(sib_pdu_update)) {
        return false;
      }
    }

    return true;
  }

private:
  ocudulog::basic_logger& logger;

  /// Min si_period is 8 frames (80 ms), with size of 128, we can enqueue SIB19 PDUs for the next 10s.
  static constexpr unsigned                                    max_nof_msgs = 128;
  std::vector<static_ring_buffer<si_pdu_update, max_nof_msgs>> si_msg_queues;
  std::vector<si_pdu_update>                                   cur_si_msg;
};
} // namespace

std::unique_ptr<sib_pdu_assembler::message_handler>
ocudu::create_si_message_extension_handler(const mac_cell_sys_info_config& req)
{
  return std::make_unique<si_message_extension_handler_impl>(req);
}
