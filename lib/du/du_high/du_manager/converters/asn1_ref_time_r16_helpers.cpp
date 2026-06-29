// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "asn1_ref_time_r16_helpers.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace asn1::rrc_nr;

byte_buffer ocudu::odu::pack_ref_time_r16(std::chrono::system_clock::time_point time_point, bool is_local_clock)
{
  constexpr int64_t gps_epoch_offset_ns = 315964800LL * 1'000'000'000LL;

  int64_t total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(time_point.time_since_epoch()).count();
  int64_t gps_ns   = is_local_clock ? total_ns : (total_ns - gps_epoch_offset_ns);

  int64_t  total_10ns           = gps_ns / 10;
  uint32_t ref_ten_nano_seconds = static_cast<uint32_t>(total_10ns % 100000);
  int64_t  total_ms             = total_10ns / 100000;
  uint16_t ref_milli_seconds    = static_cast<uint16_t>(total_ms % 1000);
  int64_t  total_s              = total_ms / 1000;
  uint32_t ref_seconds          = static_cast<uint32_t>(total_s % 86400);
  uint32_t ref_days             = static_cast<uint32_t>(total_s / 86400);

  ref_time_r16_s rrc_ref_time;
  rrc_ref_time.ref_days_r16             = ref_days;
  rrc_ref_time.ref_seconds_r16          = ref_seconds;
  rrc_ref_time.ref_milli_seconds_r16    = ref_milli_seconds;
  rrc_ref_time.ref_ten_nano_seconds_r16 = ref_ten_nano_seconds;

  byte_buffer   buf;
  asn1::bit_ref bref{buf};
  if (rrc_ref_time.pack(bref) != asn1::OCUDUASN_SUCCESS) {
    ocudu_assertion_failure("Failed to PER-pack ReferenceTime-r16");
  }
  return buf;
}
