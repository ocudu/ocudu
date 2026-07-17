// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rar_pdu_assembler.h"
#include "ocudu/mac/mac_cell_rach_handler.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/scheduler/result/pdsch_info.h"

using namespace ocudu;

namespace {

// See TS 38.321, Section 6.2.3.
constexpr unsigned MAC_RAR_SUBHEADER_AND_PAYLOAD_LENGTH = 8;
// See TS 38.321, Section 6.2.3a.
constexpr unsigned MAC_SUCCESSRAR_SUBHEADER_AND_PAYLOAD_LENGTH = 12;
// See TS 38.321, Section 6.2.2. The Backoff Indicator subPDU only has a subheader, no payload.
constexpr unsigned MAC_BI_SUBHEADER_LENGTH = 1;

bool is_success_rar(const rar_ul_grant& grant)
{
  return std::holds_alternative<rar_ul_grant::two_step_success_info>(grant.type);
}

unsigned rar_subpdu_length(const rar_ul_grant& grant)
{
  return is_success_rar(grant) ? MAC_SUCCESSRAR_SUBHEADER_AND_PAYLOAD_LENGTH : MAC_RAR_SUBHEADER_AND_PAYLOAD_LENGTH;
}

/// Encoder of RAR PDUs.
class rar_pdu_encoder
{
public:
  rar_pdu_encoder(const rar_information& rar_info_, mac_cell_rach_handler& rach_handler_) :
    rar_info(rar_info_), rach_handler(rach_handler_)
  {
  }

  void encode(span<uint8_t> output_buf);

private:
  /// Encodes RAR UL Grant into provided payload as per TS 38.321 6.1.5 and 6.2.
  void encode_rar_subpdu(const rar_ul_grant& grant, bool is_last_subpdu);

  /// \brief Encodes RAR subPDU subheader as per TS 38.321 6.1.5 and 6.2.2.
  void encode_rapid_subheader(uint16_t rapid, bool is_last_subpdu);

  /// \brief Encodes Backoff Indicator (BI) subheader as per TS 38.321 6.1.5 and 6.2.2. This subPDU has no payload.
  void encode_bi_subheader(uint8_t backoff_indicator, bool is_last_subpdu);

  /// Encodes RAR UL Grant into provided payload as per TS 38.321 6.2.3. and TS 38.213 8.2.
  void encode_rar_grant_payload(const rar_ul_grant& grant);

  /// \brief Encodes successRAR MAC subheader as per TS 38.321 6.1.5a and Figure 6.1.5a-3.
  void encode_successrar_subheader(bool is_last_subpdu);

  /// \brief Encodes successRAR payload as per TS 38.321 6.2.3a and Figure 6.2.3a-2.
  void encode_successrar_payload(const rar_ul_grant& grant);

  const rar_information& rar_info;
  mac_cell_rach_handler& rach_handler;
  uint8_t*               ptr = nullptr;
};

} // namespace

void rar_pdu_encoder::encode(span<uint8_t> output_buf)
{
  unsigned expected_len = rar_info.backoff_indicator.has_value() ? MAC_BI_SUBHEADER_LENGTH : 0;
  for (const rar_ul_grant& grant : rar_info.grants) {
    expected_len += rar_subpdu_length(grant);
  }
  ocudu_assert(output_buf.size() >= expected_len, "Output buffer is too small to fit encoded RAR");
  ptr = output_buf.data();

  if (rar_info.backoff_indicator.has_value()) {
    encode_bi_subheader(*rar_info.backoff_indicator, rar_info.grants.empty());
  }
  for (unsigned i = 0; i != rar_info.grants.size(); ++i) {
    encode_rar_subpdu(rar_info.grants[i], i == rar_info.grants.size() - 1);
  }
  ocudu_sanity_check(ptr <= output_buf.data() + output_buf.size(), "Encoded RAR PDU length differs from expected");

  // Pad with zeros.
  std::fill(ptr, output_buf.data() + output_buf.size(), 0);
}

void rar_pdu_encoder::encode_rar_subpdu(const rar_ul_grant& grant, bool is_last_subpdu)
{
  if (is_success_rar(grant)) {
    encode_successrar_subheader(is_last_subpdu);
    encode_successrar_payload(grant);
    return;
  }

  // Encode subheader.
  encode_rapid_subheader(grant.rapid, is_last_subpdu);

  // Encode payload.
  encode_rar_grant_payload(grant);
}

void rar_pdu_encoder::encode_rapid_subheader(uint16_t rapid, bool is_last_subpdu)
{
  static constexpr unsigned RAPID_FLAG = 1;

  // write E/T/RAPID MAC subheader.
  *ptr = (uint8_t)((not is_last_subpdu ? 1U : 0U) << 7U) | (RAPID_FLAG << 6U) | (static_cast<uint8_t>(rapid) & 0x3fU);
  ++ptr;
}

void rar_pdu_encoder::encode_bi_subheader(uint8_t backoff_indicator, bool is_last_subpdu)
{
  static constexpr unsigned BI_FLAG = 0;

  // write E/T/R/R/BI MAC subheader. The two R bits are reserved and set to 0.
  *ptr = static_cast<uint8_t>((not is_last_subpdu ? 1U : 0U) << 7U) | (BI_FLAG << 6U) | (backoff_indicator & 0xfU);
  ++ptr;
}

void rar_pdu_encoder::encode_rar_grant_payload(const rar_ul_grant& grant)
{
  // Encode TA (12 bits).
  // high 7 bits of TA go into first octet.
  *ptr = ((grant.ta >> 5U) & 0x7fU);
  ++ptr;
  *ptr = ((grant.ta & 0x1fU) << 3U);

  // Encode UL grant (27 bits) as per TS 38.213 Table 8.2-1.
  // encode Frequency hopping flag (1 bit).
  *ptr |= ((grant.freq_hop_flag ? 1U : 0U) << 2U);
  // encode PUSCH frequency resource allocation (14 bits).
  *ptr |= (grant.freq_resource_assignment >> (14U - 2U) & 0x3U); // first 2 bits.
  ++ptr;
  *ptr = ((grant.freq_resource_assignment >> (14U - 2U - 8U)) & 0xffU); // middle 8 bits.
  ++ptr;
  *ptr = (grant.freq_resource_assignment & 0xfU) << 4U; // last 4 bits.
  // encode PUSCH time resource allocation (4 bits).
  *ptr |= (grant.time_resource_assignment & 0xfU);
  ++ptr;
  // encode MCS (4 bits).
  *ptr = (grant.mcs.value() & 0xfU) << 4U;
  // encode TPC command for PUSCH (3 bits).
  *ptr |= (grant.tpc & 0x7U) << 1U;
  // encode CSI request (1 bit).
  *ptr |= grant.csi_req ? 1U : 0U;
  ++ptr;

  // Encode Temporary C-RNTI (2 Octets).
  *ptr = (to_value(grant.temp_crnti) >> 8U) & 0xffU;
  ++ptr;
  *ptr = to_value(grant.temp_crnti) & 0xffU;
  ++ptr;
}

void rar_pdu_encoder::encode_successrar_subheader(bool is_last_subpdu)
{
  // write E/T1/T2/S/R/R/R/R MAC subheader, as per Figure 6.1.5a-3. T1=0 indicates the T2 field is present (as
  // opposed to RAPID); T2=1 indicates the S field is present (as opposed to a Backoff Indicator); S=0, as no MAC
  // subPDU(s) for MAC SDU are appended after this successRAR yet.
  static constexpr unsigned T1_FLAG = 0;
  static constexpr unsigned T2_FLAG = 1;
  static constexpr unsigned S_FLAG  = 0;
  *ptr =
      static_cast<uint8_t>((not is_last_subpdu ? 1U : 0U) << 7U) | (T1_FLAG << 6U) | (T2_FLAG << 5U) | (S_FLAG << 4U);
  ++ptr;
}

void rar_pdu_encoder::encode_successrar_payload(const rar_ul_grant& grant)
{
  const auto& info = std::get<rar_ul_grant::two_step_success_info>(grant.type);

  // Encode UE Contention Resolution Identity (48 bits / 6 octets). Decoded from the MsgA PUSCH CCCH SDU by the MAC
  // UL PDU executor; zero-filled if not yet available, since a real UE will reject a mismatching successRAR anyway.
  std::optional<ue_con_res_id_t> con_res_id = rach_handler.resolve_msga_con_res_id(grant.temp_crnti);
  if (not con_res_id.has_value()) {
    ocudulog::fetch_basic_logger("MAC").warning(
        "rapid={}: successRAR encoded with a zero-filled UE Contention Resolution Identity. Cause: MsgA CCCH SDU "
        "not yet decoded",
        grant.rapid);
    // Set zero-filled UEConRes.
    con_res_id = ue_con_res_id_t{};
  }
  for (uint8_t byte : *con_res_id) {
    *ptr = byte;
    ++ptr;
  }

  // Encode R(1) + ChannelAccess-CPext(2, reserved: shared spectrum channel access is not supported) + TPC(2) +
  // HARQ Feedback Timing Indicator(3).
  *ptr = static_cast<uint8_t>((grant.tpc & 0x3U) << 3U) | (info.harq_feedback_timing_indicator & 0x7U);
  ++ptr;

  // Encode PUCCH Resource Indicator (4 bits) + high 4 bits of TA.
  *ptr = static_cast<uint8_t>((info.pucch_resource_indicator & 0xfU) << 4U) | ((grant.ta >> 8U) & 0xfU);
  ++ptr;

  // Encode low 8 bits of TA.
  *ptr = grant.ta & 0xffU;
  ++ptr;

  // Encode C-RNTI (2 octets).
  *ptr = (to_value(grant.temp_crnti) >> 8U) & 0xffU;
  ++ptr;
  *ptr = to_value(grant.temp_crnti) & 0xffU;
  ++ptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

rar_pdu_assembler::rar_pdu_assembler(ticking_ring_buffer_pool& pdu_pool_, mac_cell_rach_handler& rach_handler_) :
  pdu_pool(pdu_pool_), rach_handler(rach_handler_)
{
}

span<const uint8_t> rar_pdu_assembler::encode_rar_pdu(const rar_information& rar)
{
  ocudu_assert(not rar.grants.empty() or rar.backoff_indicator.has_value(),
               "Cannot encode RAR without UL grants or a Backoff Indicator");
  ocudu_assert(rar.pdsch_cfg.codewords.size() == 1, "RAR grants always carry exactly one codeword");

  // Fetch PDU buffer where RAR grant payload is going to be encoded.
  span<uint8_t> pdu_bytes = pdu_pool.allocate_buffer(rar.pdsch_cfg.codewords[0].tb_size_bytes.value());

  // Encode RAR PDU.
  rar_pdu_encoder pdu_encoder{rar, rach_handler};
  pdu_encoder.encode(pdu_bytes);

  return pdu_bytes;
}
