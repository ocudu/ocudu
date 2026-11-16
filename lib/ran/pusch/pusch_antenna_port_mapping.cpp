// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/format.h"
#include "ocudu/adt/interval.h"
#include "ocudu/ran/pusch/pusch_antenna_ports_mapping.h"
#include "ocudu/ran/pusch/pusch_constants.h"
#include <algorithm>
#include <array>

using namespace ocudu;

// Current range of supported number of layers.
static constexpr interval<unsigned, true> nof_layers_range(1, pusch_constants::MAX_NOF_LAYERS);

/// Creates the DM-RS port mask of a table entry.
template <typename... Ports>
static dmrs_port_mask dmrs_ports(Ports... ports)
{
  dmrs_port_mask mask(DMRS_MAX_NPORTS);
  (mask.set(ports), ...);
  return mask;
}

/// The following entries are taken from Table 7.3.1.1.2-6, TS 38.212, which is applicable when transform precoder is
/// enabled, dmrs-Type=1, maxLength=1.
static const std::array<pusch_antenna_ports_mapping, 4> antenna_port_mapping_tp_dmrs_type1_max_length1 = {{
    {2, dmrs_ports(0), 1},
    {2, dmrs_ports(1), 1},
    {2, dmrs_ports(2), 1},
    {2, dmrs_ports(3), 1},
}};

/// The following entries are taken from Table 7.3.1.1.2-8, TS 38.212, which is applicable when transform precoder is
/// disabled, dmrs-Type=1, maxLength=1, rank=1.
static const std::array<pusch_antenna_ports_mapping, 6> antenna_port_mapping_dmrs_type1_max_length1_rank1 = {{
    {1, dmrs_ports(0), 1},
    {1, dmrs_ports(1), 1},
    {2, dmrs_ports(0), 1},
    {2, dmrs_ports(1), 1},
    {2, dmrs_ports(2), 1},
    {2, dmrs_ports(3), 1},
}};

/// The following entries are taken from Table 7.3.1.1.2-9, TS 38.212, which is applicable when transform precoder is
/// disabled, dmrs-Type=1, maxLength=1, rank=2.
static const std::array<pusch_antenna_ports_mapping, 4> antenna_port_mapping_dmrs_type1_max_length1_rank2 = {{
    {1, dmrs_ports(0, 1), 1},
    {2, dmrs_ports(0, 1), 1},
    {2, dmrs_ports(2, 3), 1},
    {2, dmrs_ports(0, 2), 1},
}};

/// The following entries are taken from Table 7.3.1.1.2-10, TS 38.212, which is applicable when transform precoder is
/// disabled, dmrs-Type=1, maxLength=1, rank=3.
static const std::array<pusch_antenna_ports_mapping, 1> antenna_port_mapping_dmrs_type1_max_length1_rank3 = {{
    {2, dmrs_ports(0, 1, 2), 1},
}};

/// The following entries are taken from Table 7.3.1.1.2-11, TS 38.212, which is applicable when transform precoder is
/// disabled, dmrs-Type=1, maxLength=1, rank=4.
static const std::array<pusch_antenna_ports_mapping, 1> antenna_port_mapping_dmrs_type1_max_length1_rank4 = {{
    {2, dmrs_ports(0, 1, 2, 3), 1},
}};

/// The following entries are taken from Table 7.3.1.1.2-7, TS 38.212, which is applicable when transform precoder is
/// enabled, dmrs-Type=1, maxLength=2.
static const std::array<pusch_antenna_ports_mapping, 12> antenna_port_mapping_tp_dmrs_type1_max_length2 = {{
    {2, dmrs_ports(0), 1},
    {2, dmrs_ports(1), 1},
    {2, dmrs_ports(2), 1},
    {2, dmrs_ports(3), 1},
    {2, dmrs_ports(0), 2},
    {2, dmrs_ports(1), 2},
    {2, dmrs_ports(2), 2},
    {2, dmrs_ports(3), 2},
    {2, dmrs_ports(4), 2},
    {2, dmrs_ports(5), 2},
    {2, dmrs_ports(6), 2},
    {2, dmrs_ports(7), 2},
}};

/// The following entries are taken from Table 7.3.1.1.2-12, TS 38.212, which is applicable when transform precoder is
/// disabled, dmrs-Type=1, maxLength=2, rank=1.
static const std::array<pusch_antenna_ports_mapping, 14> antenna_port_mapping_dmrs_type1_max_length2_rank1 = {{
    {1, dmrs_ports(0), 1},
    {1, dmrs_ports(1), 1},
    {2, dmrs_ports(0), 1},
    {2, dmrs_ports(1), 1},
    {2, dmrs_ports(2), 1},
    {2, dmrs_ports(3), 1},
    {2, dmrs_ports(0), 2},
    {2, dmrs_ports(1), 2},
    {2, dmrs_ports(2), 2},
    {2, dmrs_ports(3), 2},
    {2, dmrs_ports(4), 2},
    {2, dmrs_ports(5), 2},
    {2, dmrs_ports(6), 2},
    {2, dmrs_ports(7), 2},
}};

/// The following entries are taken from Table 7.3.1.1.2-13, TS 38.212, which is applicable when transform precoder is
/// disabled, dmrs-Type=1, maxLength=2, rank=2.
static const std::array<pusch_antenna_ports_mapping, 10> antenna_port_mapping_dmrs_type1_max_length2_rank2 = {{
    {1, dmrs_ports(0, 1), 1},
    {2, dmrs_ports(0, 1), 1},
    {2, dmrs_ports(2, 3), 1},
    {2, dmrs_ports(0, 2), 1},
    {2, dmrs_ports(0, 1), 2},
    {2, dmrs_ports(2, 3), 2},
    {2, dmrs_ports(4, 5), 2},
    {2, dmrs_ports(6, 7), 2},
    {2, dmrs_ports(0, 4), 2},
    {2, dmrs_ports(2, 6), 2},
}};

/// The following entries are taken from Table 7.3.1.1.2-14, TS 38.212, which is applicable when transform precoder is
/// disabled, dmrs-Type=1, maxLength=2, rank=3.
static const std::array<pusch_antenna_ports_mapping, 3> antenna_port_mapping_dmrs_type1_max_length2_rank3 = {{
    {2, dmrs_ports(0, 1, 2), 1},
    {2, dmrs_ports(0, 1, 4), 2},
    {2, dmrs_ports(2, 3, 6), 2},
}};

/// The following entries are taken from Table 7.3.1.1.2-15, TS 38.212, which is applicable when transform precoder is
/// disabled, dmrs-Type=1, maxLength=2, rank=4.
static const std::array<pusch_antenna_ports_mapping, 4> antenna_port_mapping_dmrs_type1_max_length2_rank4 = {{
    {2, dmrs_ports(0, 1, 2, 3), 1},
    {2, dmrs_ports(0, 1, 4, 5), 2},
    {2, dmrs_ports(2, 3, 6, 7), 2},
    {2, dmrs_ports(0, 2, 4, 6), 2},
}};

span<const pusch_antenna_ports_mapping> ocudu::get_pusch_antenna_port_mapping_table(bool             transform_precoder,
                                                                                    dmrs_config_type dmrs_cfg_type,
                                                                                    dmrs_max_length  dmrs_max_len,
                                                                                    unsigned         nof_layers)
{
  if (dmrs_cfg_type != dmrs_config_type::type1) {
    // The tables for DM-RS type 2 are not supported.
    return {};
  }
  const bool max_length2 = dmrs_max_len == dmrs_max_length::len2;

  if (transform_precoder) {
    // Transform precoding is only possible with one layer.
    if (nof_layers != 1) {
      return {};
    }
    return max_length2 ? span<const pusch_antenna_ports_mapping>(antenna_port_mapping_tp_dmrs_type1_max_length2)
                       : span<const pusch_antenna_ports_mapping>(antenna_port_mapping_tp_dmrs_type1_max_length1);
  }

  switch (nof_layers) {
    case 1:
      return max_length2 ? span<const pusch_antenna_ports_mapping>(antenna_port_mapping_dmrs_type1_max_length2_rank1)
                         : span<const pusch_antenna_ports_mapping>(antenna_port_mapping_dmrs_type1_max_length1_rank1);
    case 2:
      return max_length2 ? span<const pusch_antenna_ports_mapping>(antenna_port_mapping_dmrs_type1_max_length2_rank2)
                         : span<const pusch_antenna_ports_mapping>(antenna_port_mapping_dmrs_type1_max_length1_rank2);
    case 3:
      return max_length2 ? span<const pusch_antenna_ports_mapping>(antenna_port_mapping_dmrs_type1_max_length2_rank3)
                         : span<const pusch_antenna_ports_mapping>(antenna_port_mapping_dmrs_type1_max_length1_rank3);
    case 4:
      return max_length2 ? span<const pusch_antenna_ports_mapping>(antenna_port_mapping_dmrs_type1_max_length2_rank4)
                         : span<const pusch_antenna_ports_mapping>(antenna_port_mapping_dmrs_type1_max_length1_rank4);
    default:
      return {};
  }
}

std::optional<unsigned> ocudu::find_pusch_antenna_port_mapping_row_index(bool             transform_precoder,
                                                                         dmrs_config_type dmrs_cfg_type,
                                                                         dmrs_max_length  dmrs_max_len,
                                                                         const pusch_antenna_ports_mapping& mapping)
{
  ocudu_assert(mapping.dmrs_ports.size() == DMRS_MAX_NPORTS,
               "The DM-RS ports mask has {} bits instead of {}.",
               mapping.dmrs_ports.size(),
               DMRS_MAX_NPORTS);

  const auto mapping_table =
      get_pusch_antenna_port_mapping_table(transform_precoder, dmrs_cfg_type, dmrs_max_len, mapping.dmrs_ports.count());
  const auto* it = std::find(mapping_table.begin(), mapping_table.end(), mapping);
  if (it == mapping_table.end()) {
    return std::nullopt;
  }
  return static_cast<unsigned>(std::distance(mapping_table.begin(), it));
}

unsigned ocudu::get_pusch_precoding_info_row_index(unsigned                                      nof_layers,
                                                   unsigned                                      max_rank,
                                                   srs_resource_configuration::one_two_four_enum nof_srs_ports,
                                                   unsigned                                      tpmi)
{
  static constexpr interval<unsigned, true> max_rank_range(1, 4);
  ocudu_assert(
      max_rank_range.contains(max_rank), "Maximum rank (i.e., {}) is out of the range {}.", max_rank, max_rank_range);
  ocudu_assert(nof_layers_range.contains(nof_layers),
               "The number of layers (i.e., {}) is out of the range {}.",
               nof_layers,
               nof_layers_range);
  ocudu_assert(nof_layers <= max_rank,
               "The number of layers (i.e., {}) must not exceed the maximum rank (i.e., {}).",
               nof_layers,
               max_rank);

  // TS38.212 Tables 7.3.1.1.2-2 and 7.3.1.1.2-3.
  if (nof_srs_ports == srs_resource_configuration::one_two_four_enum::four) {
    // Direct TPMI to row index for maximum rank 1, TS38.212 Table 7.3.1.1.2-2.
    if (max_rank == 1) {
      ocudu_assert(tpmi < 28, "PUSCH TPMI, (i.e., {}) must be lower than 28.", tpmi);
      return tpmi;
    }

    // TPMI to row indices for 1 layer, TS38.212 Table 7.3.1.1.2-2.
    if (nof_layers == 1) {
      ocudu_assert(tpmi < 28, "PUSCH TPMI, (i.e., {}) must be lower than 28.", tpmi);

      // TPMI [0, 3] to row index [0, 3]
      if (tpmi < 4) {
        return tpmi;
      }

      // TPMI [4, 11] to row index [12, 19]
      if (tpmi < 12) {
        return tpmi + 8;
      }

      // TPMI [12, 27] to row index [32, 47]
      return tpmi + 20;
    }

    // TPMI to row indices for 2 layers, TS38.212 Table 7.3.1.1.2-2.
    if (nof_layers == 2) {
      ocudu_assert(tpmi < 22, "PUSCH TPMI, (i.e., {}) must be lower than 22.", tpmi);

      // TPMI [0, 5] to row index [4, 9]
      if (tpmi < 6) {
        return 4 + tpmi;
      }

      // TPMI [6, 13] to row index [20, 27]
      if (tpmi < 14) {
        return 14 + tpmi;
      }

      // TPMI [14, 21] to row index [48, 55]
      return 34 + tpmi;
    }

    // For 3 layers.
    if (nof_layers == 3) {
      ocudu_assert(tpmi < 7, "PUSCH TPMI, (i.e., {}) must be lower than 7.", tpmi);
      // TPMI [0, 0] to row index [10, 10]
      if (tpmi == 0) {
        return 10;
      }

      // TPMI [1, 2] to row index [28, 29]
      if (tpmi < 3) {
        return 27 + tpmi;
      }

      // TPMI [3, 6] to row index [56, 59]
      return 53 + tpmi;
    }

    // For 4 layers.
    if (nof_layers == 4) {
      ocudu_assert(tpmi < 5, "PUSCH TPMI, (i.e., {}) must be lower than 5.", tpmi);
      // TPMI [0, 0] to row index [11, 11]
      if (tpmi == 0) {
        return 11;
      }

      // TPMI [1, 2] to row index [30, 31]
      if (tpmi < 3) {
        return 29 + tpmi;
      }

      // TPMI [3, 4] to row index [60, 61]
      return 57 + tpmi;
    }
  }

  // TS38.212 Tables 7.3.1.1.2-4 and 7.3.1.1.2-5.
  if (nof_srs_ports == srs_resource_configuration::one_two_four_enum::two) {
    // Direct TPMI to row index for maximum rank 1.
    if (max_rank == 1) {
      ocudu_assert(tpmi < 7, "PUSCH TPMI, (i.e., {}) must be lower than 9.", tpmi);
      return tpmi;
    }

    if (nof_layers == 2) {
      ocudu_assert(tpmi < 3, "PUSCH TPMI, (i.e., {}) must be lower than 3.", tpmi);

      // TPMI 0 to row index 2
      if (tpmi == 0) {
        return tpmi + 2;
      }

      // TPMI [1, 2] to row index [7, 8]
      return tpmi + 6;
    }

    ocudu_assert(tpmi < 7, "PUSCH TPMI, (i.e., {}) must be lower than 9.", tpmi);

    // TPMI [0, 1] to row index [0, 1]
    if (tpmi < 2) {
      return tpmi;
    }

    // TPMI [2, 5] to row index [3, 6]
    if (tpmi < 6) {
      return tpmi + 1;
    }
  }

  return tpmi;
}
