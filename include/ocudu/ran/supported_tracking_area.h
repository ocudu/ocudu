// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/plmn_identity.h"
#include "ocudu/ran/s_nssai.h"
#include "ocudu/ran/tac.h"
#include <optional>
#include <string_view>
#include <vector>

namespace ocudu::ocucp {

struct plmn_item {
  plmn_identity          plmn_id;
  std::vector<s_nssai_t> slice_support_list;
};

/// \brief Satellite RAT type of an NTN tracking area, as per TS 23.501, Section 5.4.10.
///
/// The AMF derives from it the RAT type of the UEs served in the tracking area, e.g. to extend the NAS timers of a
/// satellite access.
enum class satellite_rat_type { nr_leo, nr_meo, nr_geo, nr_othersat };

/// Returns the configuration name of a satellite RAT type.
inline const char* to_string(satellite_rat_type rat)
{
  switch (rat) {
    case satellite_rat_type::nr_leo:
      return "nr_leo";
    case satellite_rat_type::nr_meo:
      return "nr_meo";
    case satellite_rat_type::nr_geo:
      return "nr_geo";
    case satellite_rat_type::nr_othersat:
      return "nr_othersat";
  }
  return "invalid";
}

/// Parses the configuration name of a satellite RAT type. Returns an empty optional for an unknown name.
inline std::optional<satellite_rat_type> satellite_rat_type_from_string(std::string_view str)
{
  for (satellite_rat_type rat : {satellite_rat_type::nr_leo,
                                 satellite_rat_type::nr_meo,
                                 satellite_rat_type::nr_geo,
                                 satellite_rat_type::nr_othersat}) {
    if (str == to_string(rat)) {
      return rat;
    }
  }
  return std::nullopt;
}

struct supported_tracking_area {
  tac_t                  tac;
  std::vector<plmn_item> plmn_list;
  /// Satellite RAT type of the tracking area. Left empty for a terrestrial tracking area.
  std::optional<satellite_rat_type> satellite_rat;
};

} // namespace ocudu::ocucp
