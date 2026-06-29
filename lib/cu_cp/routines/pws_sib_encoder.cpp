// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pws_sib_encoder.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"

using namespace asn1::rrc_nr;

namespace ocudu::ocucp {

template <typename SibT>
static expected<byte_buffer> pack_sib(const SibT& sib)
{
  byte_buffer   buf;
  asn1::bit_ref bref{buf};
  if (sib.pack(bref) != asn1::OCUDUASN_SUCCESS) {
    return make_unexpected(default_error_t{});
  }
  return buf;
}

expected<byte_buffer> encode_sib6(uint16_t msg_id, uint16_t serial_num, uint16_t warning_type)
{
  sib6_s sib6;
  sib6.msg_id.from_number(msg_id);
  sib6.serial_num.from_number(serial_num);
  sib6.warning_type.from_number(warning_type);
  return pack_sib(sib6);
}

expected<byte_buffer>
encode_sib7(uint16_t msg_id, uint16_t serial_num, uint8_t data_coding_scheme, const byte_buffer& warning_msg_contents)
{
  const std::vector<uint8_t> msg_bytes(warning_msg_contents.begin(), warning_msg_contents.end());
  sib7_s                     sib7;
  sib7.msg_id.from_number(msg_id);
  sib7.serial_num.from_number(serial_num);
  sib7.warning_msg_segment_num        = 0;
  sib7.warning_msg_segment_type.value = sib7_s::warning_msg_segment_type_opts::last_segment;
  sib7.data_coding_scheme_present     = true;
  sib7.data_coding_scheme.from_number(data_coding_scheme);
  sib7.warning_msg_segment.from_bytes(ocudu::span<const uint8_t>(msg_bytes));
  return pack_sib(sib7);
}

expected<byte_buffer>
encode_sib8(uint16_t msg_id, uint16_t serial_num, uint8_t data_coding_scheme, const byte_buffer& warning_msg_contents)
{
  const std::vector<uint8_t> msg_bytes(warning_msg_contents.begin(), warning_msg_contents.end());
  sib8_s                     sib8;
  sib8.msg_id.from_number(msg_id);
  sib8.serial_num.from_number(serial_num);
  sib8.warning_msg_segment_num        = 0;
  sib8.warning_msg_segment_type.value = sib8_s::warning_msg_segment_type_opts::last_segment;
  sib8.data_coding_scheme_present     = true;
  sib8.data_coding_scheme.from_number(data_coding_scheme);
  sib8.warning_msg_segment.from_bytes(ocudu::span<const uint8_t>(msg_bytes));
  return pack_sib(sib8);
}

} // namespace ocudu::ocucp
