// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "sib_pdu_assembler.h"
#include "ocudu/asn1/rrc_nr/bcch_dl_sch_msg.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/slot_point_extended.h"
#include "ocudu/support/units.h"

using namespace ocudu;

/// Max SI Message PDU size. This value is implementation-defined.
static constexpr unsigned MAX_BCCH_DL_SCH_PDU_SIZE = 2048;

/// Payload of zeros sent to lower layers when an error occurs.
static const std::vector<uint8_t> zeros_payload(MAX_BCCH_DL_SCH_PDU_SIZE, 0);

/// Helper to convert byte buffer into a linear array buffer (lower layers work with linearized buffers).
static std::shared_ptr<std::vector<uint8_t>> make_linear_buffer(const byte_buffer& pdu)
{
  // Note: We overallocate the SI message buffer to account for padding.
  // Note: After this point, resizing the vector is not possible as it would invalidate the spans passed to lower
  // layers.
  auto vec = std::make_shared<std::vector<uint8_t>>(MAX_BCCH_DL_SCH_PDU_SIZE, 0);
  copy_segments(pdu, span<uint8_t>(vec->data(), vec->size()));
  return vec;
}

/// Handler of SIB1 buffer generation.
class sib_pdu_assembler::sib1_assembler
{
public:
  /// Encoder for static BCCH-DL-SCH SIB1 buffers.
  class static_msg_encoder final : public sib_pdu_assembler::bcch_dl_sch_msg_encoder
  {
  public:
    static_msg_encoder(const byte_buffer& buffer) : len(buffer.length()), current_payload(MAX_BCCH_DL_SCH_PDU_SIZE, 0)
    {
      copy_segments(buffer, current_payload);
    }

    expected<span<const uint8_t>, units::bytes> encode(slot_point_extended /*sl_tx*/, units::bytes tbs) override
    {
      if (OCUDU_LIKELY(tbs >= len)) {
        return span<const uint8_t>(current_payload.data(), tbs.value());
      }
      return make_unexpected(len);
    }

  private:
    units::bytes         len;
    std::vector<uint8_t> current_payload;
  };

  /// BCCH-DL-SCH SIB1 Buffer that gets hyperSFN auto-updated, when eDRX is enabled.
  class hypersfn_msg_encoder final : public sib_pdu_assembler::bcch_dl_sch_msg_encoder
  {
  public:
    hypersfn_msg_encoder(const byte_buffer& buffer) : current_payload(MAX_BCCH_DL_SCH_PDU_SIZE, 0)
    {
      // Unpack initial SIB1.
      {
        asn1::cbit_ref bref{buffer};
        auto           err = current_unpacked.unpack(bref);
        report_fatal_error_if_not(err == asn1::OCUDUASN_SUCCESS, "Failed to unpack SIB1 for HyperSFN-aware encoder.");
        report_fatal_error_if_not(current_unpacked.msg.type().value ==
                                          asn1::rrc_nr::bcch_dl_sch_msg_type_c::types_opts::c1 and
                                      current_unpacked.msg.c1().type().value ==
                                          asn1::rrc_nr::bcch_dl_sch_msg_type_c::c1_c_::types_opts::sib_type1,
                                  "SIB1 message is not of expected type for HyperSFN-aware encoder.");
      }

      // Ensure HyperSFN is encoded in SIB1 and its value matches the member current_hyper_sfn.
      auto& sib1msg                                                        = current_unpacked.msg.c1().sib_type1();
      sib1msg.non_crit_ext_present                                         = true;
      sib1msg.non_crit_ext.non_crit_ext_present                            = true;
      sib1msg.non_crit_ext.non_crit_ext.non_crit_ext_present               = true;
      sib1msg.non_crit_ext.non_crit_ext.non_crit_ext.hyper_sfn_r17_present = true;
      build_bcch_dl_sch_payload(0);
    }

    expected<span<const uint8_t>, units::bytes> encode(slot_point_extended sl_tx, units::bytes tbs) override
    {
      if (OCUDU_UNLIKELY(tbs < current_len)) {
        return make_unexpected(current_len);
      }

      const uint32_t new_hyper_sfn = sl_tx.hyper_sfn();
      if (current_hyper_sfn != new_hyper_sfn) {
        // HyperSFN has changed, re-encode payload.
        build_bcch_dl_sch_payload(new_hyper_sfn);
      }
      return span<const uint8_t>(current_payload.data(), tbs.value());
    }

  private:
    void build_bcch_dl_sch_payload(uint32_t hyper_sfn)
    {
      // Update HyperSFN.
      current_hyper_sfn = hyper_sfn;

      // Update HyperSFN in unpacked SIB1.
      current_unpacked.msg.c1().sib_type1().non_crit_ext.non_crit_ext.non_crit_ext.hyper_sfn_r17.from_number(hyper_sfn);

      // Re-encode.
      byte_buffer   buf;
      asn1::bit_ref bref{buf};
      auto          ret = current_unpacked.pack(bref);
      ocudu_assert(ret == asn1::OCUDUASN_SUCCESS, "Failed to pack SIB1 with updated HyperSFN.");
      size_t n = copy_segments(buf, current_payload);
      ocudu_assert(n <= current_payload.size(), "Encoded SIB1 payload exceeds maximum size.");
      current_len = units::bytes{static_cast<unsigned>(n)};
    }

    asn1::rrc_nr::bcch_dl_sch_msg_s current_unpacked;
    std::vector<uint8_t>            current_payload;
    units::bytes                    current_len{0};
    uint32_t                        current_hyper_sfn = 0;
  };

  std::shared_ptr<bcch_dl_sch_msg_encoder> handle_si_change_request(const byte_buffer& new_sib1, bool contains_hypersfn)
  {
    if (last_sib1 == new_sib1 and last_hypersfn_enabled == contains_hypersfn) {
      // No change in SIB1 payload. Reuse previous encoder.
      return last_encoder;
    }
    last_hypersfn_enabled = contains_hypersfn;
    last_sib1             = new_sib1.copy();

    // Create a new encoder.
    if (contains_hypersfn) {
      // Use HyperSFN-aware encoder.
      last_encoder = std::make_shared<hypersfn_msg_encoder>(new_sib1);
    } else {
      // eDRX not enabled, use static buffer.
      last_encoder = std::make_shared<static_msg_encoder>(new_sib1);
    }

    return last_encoder;
  }

private:
  bool last_hypersfn_enabled = false;

  /// Last BCCH-DL-SCH SIB1 payload received.
  byte_buffer last_sib1;

  /// Last generated SIB1 encoder.
  std::shared_ptr<bcch_dl_sch_msg_encoder> last_encoder;
};

sib_pdu_assembler::sib_pdu_assembler(const mac_cell_sys_info_config& req) :
  logger(ocudulog::fetch_basic_logger("MAC")), sib1_hdlr(std::make_unique<sib1_assembler>())
{
  // Version starts at 0.
  last_cfg_buffers.version = 0;
  save_buffers(0, req);
  // No need to go through pending buffer yet, as there are no race conditions at this point.
  current_buffers = last_cfg_buffers;

  // Create SI message extension handler if needed.
  message_ext_handler = create_si_message_extension_handler(req);
}

sib_pdu_assembler::~sib_pdu_assembler() = default;

void sib_pdu_assembler::handle_si_change_request(const mac_cell_sys_info_config& req)
{
  // Save new buffers that have changed.
  ocudu_assert(last_cfg_buffers.version != req.si_sched_cfg.version,
               "Version of the last SI message update is the same as the new one");
  save_buffers(req.si_sched_cfg.version, req);

  // Forward new version and buffers to SIB assembler RT path.
  pending.write_and_commit(last_cfg_buffers);
}

bool sib_pdu_assembler::enqueue_si_message_pdu_updates(const mac_cell_sys_info_pdu_update& req)
{
  if (message_ext_handler) {
    return message_ext_handler->enqueue_si_pdu_updates(req);
  }
  return false;
}

void sib_pdu_assembler::save_buffers(si_version_type si_version, const mac_cell_sys_info_config& req)
{
  // Note: In case the SIB1/SI message does not change, the comparison between the respective byte_buffers should be
  // fast (as they will point to the same memory location). Avoid at all costs the operator== for the stored vectors
  // as they are overdimensioned to account for padding.

  // Generate SIB1 encoder.
  last_cfg_buffers.sib1 = sib1_hdlr->handle_si_change_request(req.sib1, req.sib1_contains_hypersfn);

  // Check if SI messages have changed.
  last_cfg_buffers.si_msg_buffers.resize(req.si_messages.size());
  for (unsigned i = 0, e = req.si_messages.size(); i != e; ++i) {
    if (last_si_messages.size() <= i) {
      last_si_messages.resize(i + 1);
    }
    if (req.si_messages[i] != last_si_messages[i]) {
      // Resize according to the number of message segments in the new SI message.
      last_si_messages[i].resize(req.si_messages[i].size());
      // Check if the request contains a segmented SI message.
      if (req.si_messages[i].size() > 1) {
        const auto& new_si_cfg = req.si_messages[i];

        // Copy the last configuration request (perform a shallow copy of each segment).
        auto& last_si_cfg_buf = last_si_messages[i];
        last_si_cfg_buf.resize(new_si_cfg.size());
        for (unsigned i_segment = 0, nof_segments = new_si_cfg.size(); i_segment != nof_segments; ++i_segment) {
          last_si_cfg_buf[i_segment] = new_si_cfg[i_segment].copy();
        }

        // Set the SI message size.
        size_t si_msg_len = new_si_cfg.front().length();
        ocudu_assert(std::all_of(last_si_cfg_buf.begin(),
                                 last_si_cfg_buf.end(),
                                 [si_msg_len](const byte_buffer& si_msg) { return si_msg.length() == si_msg_len; }),
                     "All segments of an SI message must have the same length.");
        last_cfg_buffers.si_msg_buffers[i].first = units::bytes{static_cast<unsigned>(si_msg_len)};

        // Copy the contents of the request into a linear buffer (deep copy each segment).
        auto& si_messsage_buf =
            last_cfg_buffers.si_msg_buffers[i].second.emplace<segmented_sib_list<bcch_dl_sch_buffer>>();
        for (unsigned i_segment = 0, nof_segments = new_si_cfg.size(); i_segment != nof_segments; ++i_segment) {
          si_messsage_buf.append_segment(make_linear_buffer(new_si_cfg[i_segment]));
        }

      } else {
        // Do the same for a configuration request carrying a single SI message segment.
        auto&       last_si_cfg_buf               = last_si_messages[i].front();
        const auto& new_si_cfg                    = req.si_messages[i].front();
        last_si_cfg_buf                           = new_si_cfg.copy();
        last_cfg_buffers.si_msg_buffers[i].first  = units::bytes{static_cast<unsigned>(new_si_cfg.length())};
        last_cfg_buffers.si_msg_buffers[i].second = make_linear_buffer(new_si_cfg);
      }
    }
  }

  // Bump version.
  last_cfg_buffers.version = si_version;
}

span<const uint8_t> sib_pdu_assembler::encode_si_pdu(slot_point_extended sl_tx, const sib_information& si_info)
{
  const unsigned tbs = si_info.pdsch_cfg.codewords[0].tb_size_bytes.value();
  ocudu_assert(tbs <= MAX_BCCH_DL_SCH_PDU_SIZE, "BCCH-DL-SCH is too long. Revisit constant");

  if (si_info.version != current_buffers.version) {
    // Current SI message version is too old. Fetch new version from shared buffer.
    current_buffers = pending.read();
    if (current_buffers.version != si_info.version) {
      // Versions do not match.
      logger.error("SI message version mismatch. Expected: {}, got: {}", si_info.version, current_buffers.version);
      // We force the version to avoid more than one warning.
      current_buffers.version = si_info.version;
    }
  }

  if (si_info.si_indicator == sib_information::si_indicator_type::sib1) {
    auto payload = current_buffers.sib1->encode(sl_tx, units::bytes{tbs});
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
  if (idx >= current_buffers.si_msg_buffers.size()) {
    logger.error("Failed to encode SI-message in PDSCH. Cause: SI message index {} does not exist", idx);
    return span<const uint8_t>{zeros_payload}.first(tbs);
  }

  if (message_ext_handler) {
    auto si_pdu = message_ext_handler->get_pdu(sl_tx, si_info);
    if (!si_pdu.empty()) {
      return si_pdu;
    }
  }

  if (current_buffers.si_msg_buffers[idx].first.value() > tbs) {
    logger.warning(
        "Failed to encode SI-message {} PDSCH. Cause: PDSCH TB size {} is smaller than the SI-message length {}",
        idx,
        tbs,
        current_buffers.si_msg_buffers[idx].first.value());
    return span<const uint8_t>{zeros_payload}.first(tbs);
  }

  // If the message is segmented, return the current segment.
  if (auto* segmented_msg =
          std::get_if<segmented_sib_list<bcch_dl_sch_buffer>>(&current_buffers.si_msg_buffers[idx].second)) {
    // If the SI message has not been scheduled previously within the repetition period, and has been previously
    // transmitted, advance to the next message segment.
    if (!si_info.is_repetition && (si_info.nof_txs > 0)) {
      segmented_msg->advance_current_segment();
    }
    return span<const uint8_t>(segmented_msg->get_current_segment()->data(), tbs);
  }

  return span<const uint8_t>(std::get<bcch_dl_sch_buffer>(current_buffers.si_msg_buffers[idx].second)->data(), tbs);
}
