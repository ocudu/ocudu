// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/format.h"
#include "ocudu/adt/interval.h"
#include "ocudu/ran/pusch/pusch_antenna_ports_mapping.h"

using namespace ocudu;

// Current range of supported number of layers.
static constexpr interval<unsigned, true> nof_layers_range(1, 4);

unsigned ocudu::get_pusch_antenna_port_mapping_row_index(unsigned         nof_layers,
                                                         bool             transform_precoder,
                                                         dmrs_config_type dmrs_cfg_type,
                                                         dmrs_max_length  dmrs_max_len)
{
  ocudu_assert(nof_layers_range.contains(nof_layers),
               "The number of layers (i.e., {}) is out of the range {}.",
               nof_layers,
               nof_layers_range);
  ocudu_assert(dmrs_cfg_type == dmrs_config_type::type1, "Only DM-RS type 1 is supported.");
  ocudu_assert(dmrs_max_len == dmrs_max_length::len1, "Only DM-RS maximum length 1 is supported.");

  if (transform_precoder) {
    // PHY does not support DM-RS in ports other than 0, hence the selected antenna port value is chosen from Table
    // 7.3.1.1.2-6 in TS38.212 based on assumption of max. rank 1 and DM-RS max. length 1.
    ocudu_assert(nof_layers == 1, "The number of layers (i.e., {}) with transform precoding must be one.", nof_layers);
    return 0;
  }

  switch (nof_layers) {
    case 1:
      // PHY does not support nof. DM-RS CDM groups(s) without data other than 2, hence the selected antenna port value
      // is chosen from Table 7.3.1.1.2-8 in TS38.212 based on assumption of max. rank 1 and DM-RS max. length 1.
      return 2;
    case 2:
      // PHY does not support nof. DM-RS CDM groups(s) without data other than 2, hence the selected antenna port value
      // is chosen from Table 7.3.1.1.2-9 in TS38.212 based on assumption of max. rank 1 and DM-RS max. length 1.
      return 1;
    default:
      // The only value for three and four layers is zero in TS38.212 Table 7.3.1.1.2-10 for three layers and
      // Table 7.3.1.1.2-11 for four layers. Based on assumption of DM-RS type 1, and maximum length 1.
      return 0;
  }
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
