// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pws_sib_encoder.h"
#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include <algorithm>

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

// Splits msg_bytes into segments and encodes each as a SibT. Returns an ordered vector of packed buffers.
// dataCodingScheme is set on segment 0 only (TS 38.331 section 6.3.1, condition Segment1).
template <typename SibT, typename DCSSetFn>
static expected<std::vector<byte_buffer>> encode_segmented(uint16_t                    msg_id,
                                                           uint16_t                    serial_num,
                                                           const std::vector<uint8_t>& msg_bytes,
                                                           uint32_t                    max_segment_size,
                                                           DCSSetFn                    set_dcs)
{
  const size_t total_bytes = msg_bytes.size();
  if (total_bytes == 0 || max_segment_size == 0) {
    return make_unexpected(default_error_t{});
  }
  // warningMessageSegmentNumber is INTEGER (0..63), so at most 64 segments (TS 38.331 section 6.3.1).
  const size_t n_segments = (total_bytes + max_segment_size - 1) / max_segment_size;
  if (n_segments > 64) {
    return make_unexpected(default_error_t{});
  }

  std::vector<byte_buffer> result;
  result.reserve(n_segments);

  for (size_t i = 0; i < n_segments; ++i) {
    SibT sib;
    sib.msg_id.from_number(msg_id);
    sib.serial_num.from_number(serial_num);
    sib.warning_msg_segment_num        = static_cast<uint8_t>(i);
    sib.warning_msg_segment_type.value = (i == n_segments - 1) ? SibT::warning_msg_segment_type_opts::last_segment
                                                               : SibT::warning_msg_segment_type_opts::not_last_segment;
    if (i == 0) {
      set_dcs(sib);
    }
    const size_t offset     = i * max_segment_size;
    const size_t chunk_size = std::min(static_cast<size_t>(max_segment_size), total_bytes - offset);
    sib.warning_msg_segment.from_bytes(ocudu::span<const uint8_t>(msg_bytes.data() + offset, chunk_size));

    auto buf = pack_sib(sib);
    if (!buf) {
      return make_unexpected(default_error_t{});
    }
    result.push_back(std::move(*buf));
  }

  return result;
}

expected<std::vector<byte_buffer>> encode_sib7(uint16_t           msg_id,
                                               uint16_t           serial_num,
                                               uint8_t            data_coding_scheme,
                                               const byte_buffer& warning_msg_contents,
                                               uint32_t           max_segment_size)
{
  const std::vector<uint8_t> msg_bytes(warning_msg_contents.begin(), warning_msg_contents.end());
  return encode_segmented<sib7_s>(msg_id, serial_num, msg_bytes, max_segment_size, [data_coding_scheme](sib7_s& s) {
    s.data_coding_scheme_present = true;
    s.data_coding_scheme.from_number(data_coding_scheme);
  });
}

expected<std::vector<byte_buffer>> encode_sib8(uint16_t           msg_id,
                                               uint16_t           serial_num,
                                               uint8_t            data_coding_scheme,
                                               const byte_buffer& warning_msg_contents,
                                               uint32_t           max_segment_size)
{
  const std::vector<uint8_t> msg_bytes(warning_msg_contents.begin(), warning_msg_contents.end());
  return encode_segmented<sib8_s>(msg_id, serial_num, msg_bytes, max_segment_size, [data_coding_scheme](sib8_s& s) {
    s.data_coding_scheme_present = true;
    s.data_coding_scheme.from_number(data_coding_scheme);
  });
}

} // namespace ocudu::ocucp
