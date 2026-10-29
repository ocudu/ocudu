// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/pdsch/pdsch_antenna_ports_mapping.h"
#include "ocudu/ran/pdsch/pdsch_constants.h"
#include "ocudu/support/math/pow2_utils.h"
#include <algorithm>
#include <array>

using namespace ocudu;

/// Creates the DM-RS port mask of a table entry.
template <typename... Ports>
static dmrs_port_mask dmrs_ports(Ports... ports)
{
  dmrs_port_mask mask(DMRS_MAX_NPORTS);
  (mask.set(ports), ...);
  return mask;
}

/// The following entries are taken from Table 7.3.1.2.2-1, TS 38.212, which is applicable when dmrs-Type=1,
/// maxLength=1.
static constexpr uint32_t antenna_port_mapping_dmrs_type1_max_length1_table_entries = 12;
static const std::array<pdsch_antenna_ports_mapping, antenna_port_mapping_dmrs_type1_max_length1_table_entries>
    antenna_port_mapping_dmrs_type1_max_length1 = {{
        // clang-format off
        {1, dmrs_ports(0), 1},
        {1, dmrs_ports(1), 1},
        {1, dmrs_ports(0, 1), 1},
        {2, dmrs_ports(0), 1},
        {2, dmrs_ports(1), 1},
        {2, dmrs_ports(2), 1},
        {2, dmrs_ports(3), 1},
        {2, dmrs_ports(0, 1), 1},
        {2, dmrs_ports(2, 3), 1},
        {2, dmrs_ports(0, 1, 2), 1},
        {2, dmrs_ports(0, 1, 2, 3), 1},
        {2, dmrs_ports(0, 2), 1},
        // clang-format on
    }};

/// The following entries are taken from Table 7.3.1.2.2-2, TS 38.212, which is applicable when dmrs-Type=1, maxLength=2
/// and only codeword=0 is enabled.
static constexpr uint32_t antenna_port_mapping_dmrs_type1_max_length2_one_cw_table_entries = 31;
static const std::array<pdsch_antenna_ports_mapping, antenna_port_mapping_dmrs_type1_max_length2_one_cw_table_entries>
    antenna_port_mapping_dmrs_type1_max_length2_one_cw = {{
        // clang-format off
        {1, dmrs_ports(0), 1},
        {1, dmrs_ports(1), 1},
        {1, dmrs_ports(0, 1), 1},
        {2, dmrs_ports(0), 1},
        {2, dmrs_ports(1), 1},
        {2, dmrs_ports(2), 1},
        {2, dmrs_ports(3), 1},
        {2, dmrs_ports(0, 1), 1},
        {2, dmrs_ports(2, 3), 1},
        {2, dmrs_ports(0, 1, 2), 1},
        {2, dmrs_ports(0, 1, 2, 3), 1},
        {2, dmrs_ports(0, 2), 1},
        {2, dmrs_ports(0), 2},
        {2, dmrs_ports(1), 2},
        {2, dmrs_ports(2), 2},
        {2, dmrs_ports(3), 2},
        {2, dmrs_ports(4), 2},
        {2, dmrs_ports(5), 2},
        {2, dmrs_ports(6), 2},
        {2, dmrs_ports(7), 2},
        {2, dmrs_ports(0, 1), 2},
        {2, dmrs_ports(2, 3), 2},
        {2, dmrs_ports(4, 5), 2},
        {2, dmrs_ports(6, 7), 2},
        {2, dmrs_ports(0, 4), 2},
        {2, dmrs_ports(2, 6), 2},
        {2, dmrs_ports(0, 1, 4), 2},
        {2, dmrs_ports(2, 3, 6), 2},
        {2, dmrs_ports(0, 1, 4, 5), 2},
        {2, dmrs_ports(2, 3, 6, 7), 2},
        {2, dmrs_ports(0, 2, 4, 6), 2},
        // clang-format on
    }};

/// The following entries are taken from Table 7.3.1.2.2-2, TS 38.212, which is applicable when dmrs-Type=1, maxLength=2
/// and both codeword=0 and codeword=1 are enabled.
static constexpr uint32_t antenna_port_mapping_dmrs_type1_max_length2_two_cw_table_entries = 4;
static const std::array<pdsch_antenna_ports_mapping, antenna_port_mapping_dmrs_type1_max_length2_two_cw_table_entries>
    antenna_port_mapping_dmrs_type1_max_length2_two_cw = {{
        // clang-format off
        {2, dmrs_ports(0, 1, 2, 3, 4), 2},
        {2, dmrs_ports(0, 1, 2, 3, 4, 6), 2},
        {2, dmrs_ports(0, 1, 2, 3, 4, 5, 6), 2},
        {2, dmrs_ports(0, 1, 2, 3, 4, 5, 6, 7), 2},
        // clang-format on
    }};

/// The following entries are taken from Table 7.3.1.2.2-3, TS 38.212, which is applicable when dmrs-Type=2, maxLength=1
/// and only codeword=0 is enabled.
static constexpr uint32_t antenna_port_mapping_dmrs_type2_max_length1_one_cw_table_entries = 24;
static const std::array<pdsch_antenna_ports_mapping, antenna_port_mapping_dmrs_type2_max_length1_one_cw_table_entries>
    antenna_port_mapping_dmrs_type2_max_length1_one_cw = {{
        // clang-format off
        {1, dmrs_ports(0), 1},
        {1, dmrs_ports(1), 1},
        {1, dmrs_ports(0, 1), 1},
        {2, dmrs_ports(0), 1},
        {2, dmrs_ports(1), 1},
        {2, dmrs_ports(2), 1},
        {2, dmrs_ports(3), 1},
        {2, dmrs_ports(0, 1), 1},
        {2, dmrs_ports(2, 3), 1},
        {2, dmrs_ports(0, 1, 2), 1},
        {2, dmrs_ports(0, 1, 2, 3), 1},
        {3, dmrs_ports(0), 1},
        {3, dmrs_ports(1), 1},
        {3, dmrs_ports(2), 1},
        {3, dmrs_ports(3), 1},
        {3, dmrs_ports(4), 1},
        {3, dmrs_ports(5), 1},
        {3, dmrs_ports(0, 1), 1},
        {3, dmrs_ports(2, 3), 1},
        {3, dmrs_ports(4, 5), 1},
        {3, dmrs_ports(0, 1, 2), 1},
        {3, dmrs_ports(3, 4, 5), 1},
        {3, dmrs_ports(0, 1, 2, 3), 1},
        {2, dmrs_ports(0, 2), 1},
        // clang-format on
    }};

/// The following entries are taken from Table 7.3.1.2.2-3, TS 38.212, which is applicable when dmrs-Type=2, maxLength=1
/// and both codeword=0 and codeword=1 are enabled.
static constexpr uint32_t antenna_port_mapping_dmrs_type2_max_length1_two_cw_table_entries = 2;
static const std::array<pdsch_antenna_ports_mapping, antenna_port_mapping_dmrs_type2_max_length1_two_cw_table_entries>
    antenna_port_mapping_dmrs_type2_max_length1_two_cw = {{
        // clang-format off
        {3, dmrs_ports(0, 1, 2, 3, 4), 1},
        {3, dmrs_ports(0, 1, 2, 3, 4, 5), 1},
        // clang-format on
    }};

/// The following entries are taken from Table 7.3.1.2.2-4, TS 38.212, which is applicable when dmrs-Type=2, maxLength=2
/// and only codeword=0 is enabled.
static const uint32_t antenna_port_mapping_dmrs_type2_max_length2_one_cw_table_entries = 58;
static const std::array<pdsch_antenna_ports_mapping, antenna_port_mapping_dmrs_type2_max_length2_one_cw_table_entries>
    antenna_port_mapping_dmrs_type2_max_length2_one_cw = {{
        // clang-format off
        {1, dmrs_ports(0), 1},
        {1, dmrs_ports(1), 1},
        {1, dmrs_ports(0, 1), 1},
        {2, dmrs_ports(0), 1},
        {2, dmrs_ports(1), 1},
        {2, dmrs_ports(2), 1},
        {2, dmrs_ports(3), 1},
        {2, dmrs_ports(0, 1), 1},
        {2, dmrs_ports(2, 3), 1},
        {2, dmrs_ports(0, 1, 2), 1},
        {2, dmrs_ports(0, 1, 2, 3), 1},
        {3, dmrs_ports(0), 1},
        {3, dmrs_ports(1), 1},
        {3, dmrs_ports(2), 1},
        {3, dmrs_ports(3), 1},
        {3, dmrs_ports(4), 1},
        {3, dmrs_ports(5), 1},
        {3, dmrs_ports(0, 1), 1},
        {3, dmrs_ports(2, 3), 1},
        {3, dmrs_ports(4, 5), 1},
        {3, dmrs_ports(0, 1, 2), 1},
        {3, dmrs_ports(3, 4, 5), 1},
        {3, dmrs_ports(0, 1, 2, 3), 1},
        {2, dmrs_ports(0, 2), 1},
        {3, dmrs_ports(0), 2},
        {3, dmrs_ports(1), 2},
        {3, dmrs_ports(2), 2},
        {3, dmrs_ports(3), 2},
        {3, dmrs_ports(4), 2},
        {3, dmrs_ports(5), 2},
        {3, dmrs_ports(6), 2},
        {3, dmrs_ports(7), 2},
        {3, dmrs_ports(8), 2},
        {3, dmrs_ports(9), 2},
        {3, dmrs_ports(10), 2},
        {3, dmrs_ports(11), 2},
        {3, dmrs_ports(0, 1), 2},
        {3, dmrs_ports(2, 3), 2},
        {3, dmrs_ports(4, 5), 2},
        {3, dmrs_ports(6, 7), 2},
        {3, dmrs_ports(8, 9), 2},
        {3, dmrs_ports(10, 11), 2},
        {3, dmrs_ports(0, 1, 6), 2},
        {3, dmrs_ports(2, 3, 8), 2},
        {3, dmrs_ports(4, 5, 10), 2},
        {3, dmrs_ports(0, 1, 6, 7), 2},
        {3, dmrs_ports(2, 3, 8, 9), 2},
        {3, dmrs_ports(4, 5, 10, 11), 2},
        {1, dmrs_ports(0), 2},
        {1, dmrs_ports(1), 2},
        {1, dmrs_ports(6), 2},
        {1, dmrs_ports(7), 2},
        {1, dmrs_ports(0, 1), 2},
        {1, dmrs_ports(6, 7), 2},
        {2, dmrs_ports(0, 1), 2},
        {2, dmrs_ports(2, 3), 2},
        {2, dmrs_ports(6, 7), 2},
        {2, dmrs_ports(8, 9), 2},
        // clang-format on
    }};

/// The following entries are taken from Table 7.3.1.2.2-4, TS 38.212, which is applicable when dmrs-Type=2, maxLength=2
/// and both codeword=0 and codeword=1 are enabled.
static constexpr uint32_t antenna_port_mapping_dmrs_type2_max_length2_two_cw_table_entries = 6;
static const std::array<pdsch_antenna_ports_mapping, antenna_port_mapping_dmrs_type2_max_length2_two_cw_table_entries>
    antenna_port_mapping_dmrs_type2_max_length2_two_cw = {{
        // clang-format off
        {3, dmrs_ports(0, 1, 2, 3, 4), 1},
        {3, dmrs_ports(0, 1, 2, 3, 4, 5), 1},
        {2, dmrs_ports(0, 1, 2, 3, 6), 2},
        {2, dmrs_ports(0, 1, 2, 3, 6, 8), 2},
        {2, dmrs_ports(0, 1, 2, 3, 6, 7, 8), 2},
        {2, dmrs_ports(0, 1, 2, 3, 6, 7, 8, 9), 2},
        // clang-format on
    }};

span<const pdsch_antenna_ports_mapping> ocudu::get_pdsch_antenna_port_mapping_table(dmrs_config_type dmrs_cfg_type,
                                                                                    dmrs_max_length  dmrs_max_len,
                                                                                    bool are_both_cws_enabled)
{
  if (dmrs_cfg_type == dmrs_config_type::type1 and dmrs_max_len == dmrs_max_length::len1 and not are_both_cws_enabled) {
    return antenna_port_mapping_dmrs_type1_max_length1;
  }
  if (dmrs_cfg_type == dmrs_config_type::type1 and dmrs_max_len == dmrs_max_length::len1 and are_both_cws_enabled) {
    // DM-RS type 1 with a single front-load symbol supports up to four ports, i.e., a single codeword.
    return {};
  }
  if (dmrs_cfg_type == dmrs_config_type::type1 and dmrs_max_len == dmrs_max_length::len2 and not are_both_cws_enabled) {
    return antenna_port_mapping_dmrs_type1_max_length2_one_cw;
  }
  if (dmrs_cfg_type == dmrs_config_type::type1 and dmrs_max_len == dmrs_max_length::len2 and are_both_cws_enabled) {
    return antenna_port_mapping_dmrs_type1_max_length2_two_cw;
  }
  if (dmrs_cfg_type == dmrs_config_type::type2 and dmrs_max_len == dmrs_max_length::len1 and not are_both_cws_enabled) {
    return antenna_port_mapping_dmrs_type2_max_length1_one_cw;
  }
  if (dmrs_cfg_type == dmrs_config_type::type2 and dmrs_max_len == dmrs_max_length::len1 and are_both_cws_enabled) {
    return antenna_port_mapping_dmrs_type2_max_length1_two_cw;
  }
  if (dmrs_cfg_type == dmrs_config_type::type2 and dmrs_max_len == dmrs_max_length::len2 and not are_both_cws_enabled) {
    return antenna_port_mapping_dmrs_type2_max_length2_one_cw;
  }
  if (dmrs_cfg_type == dmrs_config_type::type2 and dmrs_max_len == dmrs_max_length::len2 and are_both_cws_enabled) {
    return antenna_port_mapping_dmrs_type2_max_length2_two_cw;
  }
  report_fatal_error("No PDSCH antenna ports mapping found for the given DMRS configuration.");
}

unsigned ocudu::get_pdsch_antenna_ports_field_size(dmrs_config_type dmrs_cfg_type, dmrs_max_length dmrs_max_len)
{
  // The field indexes the single codeword table, which holds the most rows.
  const auto mapping_table = get_pdsch_antenna_port_mapping_table(dmrs_cfg_type, dmrs_max_len, false);
  return log2_ceil(static_cast<unsigned>(mapping_table.size()));
}

std::optional<unsigned> ocudu::find_pdsch_antenna_port_mapping_row_index(dmrs_config_type dmrs_cfg_type,
                                                                         dmrs_max_length  dmrs_max_len,
                                                                         const pdsch_antenna_ports_mapping& mapping)
{
  // See TS 38.211 Table 7.3.1.3-1, the second codeword is enabled when there are more layers than a codeword carries.
  const bool  are_both_cws_enabled = mapping.dmrs_ports.count() > pdsch_constants::MAX_NOF_LAYERS_PER_CODEWORD;
  const auto  mapping_table = get_pdsch_antenna_port_mapping_table(dmrs_cfg_type, dmrs_max_len, are_both_cws_enabled);
  const auto* it            = std::find(mapping_table.begin(), mapping_table.end(), mapping);
  if (it == mapping_table.end()) {
    return std::nullopt;
  }
  return static_cast<unsigned>(std::distance(mapping_table.begin(), it));
}
