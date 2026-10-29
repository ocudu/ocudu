// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/dmrs/dmrs.h"
#include <optional>

namespace ocudu {

/// Used to configure downlink demodulation reference signals for PDSCH.
/// \remark See TS 38.331, DMRS-DownlinkConfig.
struct dmrs_downlink_config {
  /// DMRS type to be used for DL. Mapping to NR RRC: type 2 sets the field, and type 1 leaves it absent.
  dmrs_config_type type{dmrs_config_type::type1};
  /// Position for additional DM-RS in DL, see Tables 7.4.1.1.2-3 and 7.4.1.1.2-4 in TS 38.211. If the field is absent,
  /// the UE applies the value pos2.
  dmrs_additional_positions additional_positions{dmrs_additional_positions::pos2};
  /// The maximum number of OFDM symbols for DL front loaded DMRS. Mapping to NR RRC: len2 sets the field, and len1
  /// leaves it absent. If set to len2, the UE determines the actual number of DM-RS symbols by the associated DCI.
  dmrs_max_length max_length{dmrs_max_length::len1};
  /// DL DMRS scrambling initialization (see TS 38.211, clause 7.4.1.1.1).
  /// When the field is absent the UE applies the value Physical cell ID (physCellId) configured for this serving cell.
  std::optional<uint16_t> scrambling_id0;
  std::optional<uint16_t> scrambling_id1;
  // TODO: Remaining

  bool operator==(const dmrs_downlink_config& rhs) const
  {
    return type == rhs.type && additional_positions == rhs.additional_positions && max_length == rhs.max_length &&
           scrambling_id0 == rhs.scrambling_id0 && scrambling_id1 == rhs.scrambling_id1;
  }
  bool operator!=(const dmrs_downlink_config& rhs) const { return !(rhs == *this); }
};

} // namespace ocudu
