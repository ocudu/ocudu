// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include <cstdint>
#include <optional>

namespace ocudu {

struct five_g_s_tmsi_t {
  five_g_s_tmsi_t() = default;

  five_g_s_tmsi_t(const bounded_bitset<48>& five_g_s_tmsi_) : five_g_s_tmsi(five_g_s_tmsi_)
  {
    ocudu_assert(five_g_s_tmsi_.size() == 48, "Invalid size for 5G-S-TMSI ({})", five_g_s_tmsi_.size());
  }

  five_g_s_tmsi_t(uint64_t amf_set_id, uint64_t amf_pointer, uint64_t five_g_tmsi)
  {
    five_g_s_tmsi.emplace();
    five_g_s_tmsi->resize(48);
    five_g_s_tmsi->from_uint64((amf_set_id << 38U) + (amf_pointer << 32U) + five_g_tmsi);
  }

  uint16_t get_amf_set_id() const
  {
    ocudu_assert(five_g_s_tmsi.has_value(), "five_g_s_tmsi is not set");
    return five_g_s_tmsi.value().to_uint64() >> 38U;
  }

  uint8_t get_amf_pointer() const
  {
    ocudu_assert(five_g_s_tmsi.has_value(), "five_g_s_tmsi is not set");
    return (five_g_s_tmsi.value().to_uint64() & 0x3f00000000) >> 32U;
  }

  uint32_t get_five_g_tmsi() const
  {
    ocudu_assert(five_g_s_tmsi.has_value(), "five_g_s_tmsi is not set");
    return (five_g_s_tmsi.value().to_uint64() & 0xffffffff);
  }

  uint64_t to_number() const { return five_g_s_tmsi->to_uint64(); }

private:
  std::optional<bounded_bitset<48>> five_g_s_tmsi;
};

} // namespace ocudu
