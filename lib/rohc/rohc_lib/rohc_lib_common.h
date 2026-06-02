// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/rohc/rohc_config.h"
#include <rohc/rohc.h>
#include <rohc/rohc_comp.h>
#include <rohc/rohc_decomp.h>

namespace ocudu::rohc {

/// The maximum size of a ROHC packet. This implementation-specific parameter shall be larger than the maximum supported
/// PDCP SDU size (see TS 38.323, Sec. 4.3.1) \c pdcp_max_sdu_size and equals the \c network_gateway_udp_max_len.
static constexpr uint32_t rohc_max_packet_size = 9100;

/// Threshold for CID type. Large CIDs shall be used if maximum CID > 15. Otherwise small CIDs shall be used.
/// See TS 38.323 Sec. 5.7.3.
static constexpr uint16_t rohc_cid_type_threshold = 15;

static constexpr rohc_cid_type_t rohc_lib_get_cid_type(uint16_t max_cid)
{
  return max_cid > rohc_cid_type_threshold ? rohc_cid_type_t::ROHC_LARGE_CID : rohc_cid_type_t::ROHC_SMALL_CID;
}

static constexpr rohc_profile_t to_rohc_profile_t(rohc_profile profile)
{
  switch (profile) {
    case rohc_profile::profile0x0001:
      return rohc_profile_t::ROHC_PROFILE_RTP;
    case rohc_profile::profile0x0002:
      return rohc_profile_t::ROHC_PROFILE_UDP;
    case rohc_profile::profile0x0003:
      return rohc_profile_t::ROHC_PROFILE_ESP;
    case rohc_profile::profile0x0004:
      return rohc_profile_t::ROHC_PROFILE_IP;
    case rohc_profile::profile0x0006:
      return rohc_profile_t::ROHC_PROFILE_TCP;
    case rohc_profile::profile0x0101:
      return rohc_profile_t::ROHCv2_PROFILE_IP_UDP_RTP;
    case rohc_profile::profile0x0102:
      return rohc_profile_t::ROHCv2_PROFILE_IP_UDP;
    case rohc_profile::profile0x0103:
      return rohc_profile_t::ROHCv2_PROFILE_IP_ESP;
    case rohc_profile::profile0x0104:
      return rohc_profile_t::ROHCv2_PROFILE_IP;
  }
  // We should never get here.
  return rohc_profile_t::ROHC_PROFILE_MAX;
}

} // namespace ocudu::rohc
