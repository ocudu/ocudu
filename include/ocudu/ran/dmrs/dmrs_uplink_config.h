// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/dmrs/dmrs.h"
#include "ocudu/ran/ptrs.h"
#include <optional>

namespace ocudu {

/// Used to configure uplink demodulation reference signals for PUSCH.
/// \remark See TS 38.331, DMRS-UplinkConfig.
struct dmrs_uplink_config {
  /// \brief DMRS related parameters for Cyclic Prefix OFDM.
  struct transform_precoder_disabled {
    /// UL DMRS scrambling initialization for CP-OFDM. When the field is absent the UE applies the value Physical cell
    /// ID. See TS 38.211, clause 6.4.1.1.1.1.
    std::optional<uint16_t> scrambling_id0;
    std::optional<uint16_t> scrambling_id1;

    bool operator==(const transform_precoder_disabled& rhs) const
    {
      return scrambling_id0 == rhs.scrambling_id0 && scrambling_id1 == rhs.scrambling_id1;
    }
    bool operator!=(const transform_precoder_disabled& rhs) const { return !(rhs == *this); }
  };

  /// \brief DMRS related parameters for DFT-s-OFDM (Transform Precoding).
  struct transform_precoder_enabled {
    /// Values {0..1007}.
    std::optional<uint16_t> n_pusch_id;
    /// For DMRS transmission with transform precoder the NW may configure group hopping by the cell-specific parameter
    /// groupHoppingEnabledTransformPrecoding in PUSCH-ConfigCommon. In this case, the NW may include this UE specific
    /// field to disable group hopping for PUSCH transmission except for Msg3.
    /// Override the configuration in PUSCH-ConfigCommon.
    /// Mapping to NR RRC: If true, the field is present in NR RRC indicating value disabled to UE. If false, the field
    /// is absent in NR RRC and the UE uses the same hopping mode as for Msg3.
    bool is_seq_grp_hopping_disabled{false};
    /// Determines if sequence hopping is enabled for DMRS transmission with transform precoder for PUSCH transmission
    /// other than Msg3 (sequence hopping is always disabled for Msg3).
    /// Mapping to NR RRC: If true, the field is present in NR RRC indicating value enabled to UE. If false, the field
    /// is absent in NR RRC and the UE uses the same hopping mode as for Msg3.
    /// Note: The network does not configure simultaneous group hopping and sequence hopping.
    bool is_seq_hopping_enabled{false};

    bool operator==(const transform_precoder_enabled& rhs) const
    {
      return n_pusch_id == rhs.n_pusch_id && is_seq_grp_hopping_disabled == rhs.is_seq_grp_hopping_disabled &&
             is_seq_hopping_enabled == rhs.is_seq_hopping_enabled;
    }
    bool operator!=(const transform_precoder_enabled& rhs) const { return !(rhs == *this); }
  };

  /// Selection of the DMRS type to be used for UL.
  /// Mapping to NR RRC: If true, the field is present in NR RRC indicating value type2 to UE. If false, the field
  /// is absent in NR RRC and the UE uses DMRS type 1.
  bool is_dmrs_type2{false};
  /// Position for additional DM-RS in UL (see TS 38.211, clause 6.4.1.1.3). If the field is not set, the UE applies the
  /// value pos2.
  dmrs_additional_positions additional_positions{dmrs_additional_positions::pos2};
  /// Configures uplink PTRS.
  std::optional<ptrs_uplink_config> ptrs;
  /// The maximum number of OFDM symbols for UL front loaded DMRS.
  /// Mapping to NR RRC: If true, the field is present in NR RRC indicating value len2 to UE. If false, the field
  /// is absent in NR RRC and the UE applies value len1.
  /// If set to len2, the UE determines the actual number of DM-RS symbols by the associated DCI.
  bool                                       is_max_length_len2{false};
  std::optional<transform_precoder_disabled> trans_precoder_disabled;
  std::optional<transform_precoder_enabled>  trans_precoder_enabled;
  // TODO: Remaining

  bool operator==(const dmrs_uplink_config& rhs) const
  {
    return is_dmrs_type2 == rhs.is_dmrs_type2 && additional_positions == rhs.additional_positions && ptrs == rhs.ptrs &&
           is_max_length_len2 == rhs.is_max_length_len2 && trans_precoder_disabled == rhs.trans_precoder_disabled &&
           trans_precoder_enabled == rhs.trans_precoder_enabled;
  }
  bool operator!=(const dmrs_uplink_config& rhs) const { return !(rhs == *this); }
};

} // namespace ocudu
