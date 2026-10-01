// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "sib_pdu_assembler.h"
#include "ocudu/ocudulog/ocudulog.h"

using namespace ocudu;

/// Payload of zeros sent to lower layers when an error occurs.
static const std::vector<uint8_t> zeros_payload(MAX_BCCH_DL_SCH_PDU_SIZE, 0);

sib_pdu_assembler::sib_pdu_assembler() : logger(ocudulog::fetch_basic_logger("MAC")) {}

void sib_pdu_assembler::start_broadcast(std::shared_ptr<si_message_extension_handler> ext_handler_,
                                        const si_update_command&                      first_cmd,
                                        std::unique_ptr<pws_broadcast_end_notifier>   pws_end_notifier_)
{
  ocudu_assert(current.sib1 == nullptr, "The SI broadcast cannot be started more than once");
  ext_handler      = std::move(ext_handler_);
  pws_end_notifier = std::move(pws_end_notifier_);

  handle_si_update(first_cmd);

  // No need to go through the pending buffer, as no slot indications are processed yet and there are no race
  // conditions with the RT path at this point.
  current = pending.read();
}

void sib_pdu_assembler::handle_si_update(const si_update_command& cmd)
{
  pending.write_and_commit(si_encoder_snapshot{cmd.version, cmd.sib1, cmd.si_msgs});
}

void sib_pdu_assembler::handle_pws_si_update(const si_update_command& cmd)
{
  pending_pws.write_and_commit(si_encoder_snapshot{cmd.version, cmd.sib1, cmd.si_msgs});
}

void sib_pdu_assembler::handle_pws_broadcast_end()
{
  if (not pws_version_in_use.has_value()) {
    // No warning is active.
    return;
  }

  // The scheduler went back to stamping the grants with the epoch of the normal operation, which it only does once
  // every warning finished being broadcast.
  pws_end_notifier->on_pws_broadcast_end(pws_version_in_use.value());
  pws_version_in_use.reset();
}

const sib_pdu_assembler::si_encoder_snapshot& sib_pdu_assembler::select_snapshot(si_version_type version)
{
  if (version == current.version) {
    handle_pws_broadcast_end();
    return current;
  }
  if (version == current_pws.version) {
    pws_version_in_use = current_pws.version;
    return current_pws;
  }

  // The grant was scheduled with an SI epoch that is not held yet. Fetch it from the shared buffers.
  current = pending.read();
  if (version == current.version) {
    handle_pws_broadcast_end();
    return current;
  }
  current_pws = pending_pws.read();
  if (version == current_pws.version) {
    pws_version_in_use = current_pws.version;
    return current_pws;
  }

  logger.error("SI message version mismatch. Expected: {}, got: {}", version, current.version);
  // We force the version to avoid more than one error log message.
  current.version = version;
  return current;
}

span<const uint8_t> sib_pdu_assembler::encode_si_pdu(slot_point_extended sl_tx, const sib_information& si_info)
{
  ocudu_assert(si_info.pdsch_cfg.codewords.size() == 1, "SIB grants always carry exactly one codeword");
  const unsigned tbs = si_info.pdsch_cfg.codewords[0].tb_size_bytes.value();
  ocudu_assert(tbs <= MAX_BCCH_DL_SCH_PDU_SIZE, "BCCH-DL-SCH is too long. Revisit constant");

  const si_encoder_snapshot& snapshot = select_snapshot(si_info.version);

  if (si_info.si_indicator == sib_information::si_indicator_type::sib1) {
    if (not snapshot.sib1) {
      logger.error("Failed to encode SIB1 in PDSCH. Cause: No SIB1 was provided for the cell");
      return span<const uint8_t>{zeros_payload}.first(tbs);
    }
    auto payload = snapshot.sib1->encode(sl_tx, si_info);
    if (not payload.has_value()) {
      units::bytes sib1_len = payload.error();
      logger.warning(
          "Failed to encode SIB1 PDSCH. Cause: PDSCH TB size {} is smaller than the SIB1 length {}", tbs, sib1_len);
      return span<const uint8_t>{zeros_payload}.first(tbs);
    }
    return payload.value();
  }

  ocudu_assert(si_info.si_msg_index.has_value(), "Invalid SI message index");
  const unsigned idx = si_info.si_msg_index.value();
  if (idx >= snapshot.si_msgs.size() or not snapshot.si_msgs[idx]) {
    logger.error("Failed to encode SI-message in PDSCH. Cause: SI message index {} does not exist", idx);
    return span<const uint8_t>{zeros_payload}.first(tbs);
  }

  if (ext_handler) {
    auto si_pdu = ext_handler->get_pdu(sl_tx, si_info);
    if (!si_pdu.empty()) {
      return si_pdu;
    }
  }

  auto payload = snapshot.si_msgs[idx]->encode(sl_tx, si_info);
  if (not payload.has_value()) {
    units::bytes min_len = payload.error();
    logger.warning(
        "Failed to encode SI-message {} PDSCH. Cause: PDSCH TB size {} is smaller than the SI-message length {}",
        idx,
        tbs,
        min_len.value());
    return span<const uint8_t>{zeros_payload}.first(tbs);
  }
  return payload.value();
}
