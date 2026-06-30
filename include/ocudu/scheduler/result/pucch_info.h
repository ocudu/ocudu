// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/bwp/bwp_configuration.h"
#include "ocudu/ran/csi_report/csi_report_configuration.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/pucch/pucch_mapping.h"
#include "ocudu/ran/pucch/pucch_uci_bits.h"
#include "ocudu/ran/rnti.h"

namespace ocudu {

/// Info about PUCCH used resource.
struct pucch_info {
  /// Format 0 specific parameters for a PUCCH transmission.
  struct f0_config {
    /// \c pucch-GroupHopping, as per TS 38.331.
    pucch_group_hopping group_hopping;
    /// \f$n_{ID}\f$ as per Section 6.3.2.2.1, TS 38.211.
    unsigned n_id_hopping;
  };

  /// Format 1 specific parameters for a PUCCH transmission.
  struct f1_config {
    /// \c pucch-GroupHopping, as per TS 38.331.
    pucch_group_hopping group_hopping;
    /// \f$n_{ID}\f$ as per Section 6.3.2.2.1, TS 38.211.
    unsigned                 n_id_hopping;
    pucch_repetition_tx_slot slot_repetition;
  };

  /// Format 2 specific parameters for a PUCCH transmission.
  struct f2_config {
    /// \f$n_{ID}\f$ as per Section 6.3.2.5.1 and 6.3.2.6.1, TS 38.211.
    uint16_t n_id_scrambling;
    /// \f$N_{ID}^0\f$ as per TS 38.211, Section 6.4.1.3.2.1.
    uint16_t n_id_0_scrambling;
    uint8_t  nof_prbs;
  };

  /// Format 3 specific parameters for a PUCCH transmission.
  struct f3_config {
    /// \c pucch-GroupHopping, as per TS 38.331
    pucch_group_hopping group_hopping;
    /// \f$n_{ID}\f$ as per Section 6.3.2.2.1, TS 38.211.
    unsigned                 n_id_hopping;
    pucch_repetition_tx_slot slot_repetition;
    uint16_t                 n_id_scrambling;
    uint16_t                 n_id_0_scrambling;
    uint8_t                  nof_prbs;
  };

  /// Format 4 specific parameters for a PUCCH transmission.
  struct f4_config {
    /// \c pucch-GroupHopping, as per TS 38.331
    pucch_group_hopping group_hopping;
    /// \f$n_{ID}\f$ as per Section 6.3.2.2.1, TS 38.211.
    unsigned                 n_id_hopping;
    pucch_repetition_tx_slot slot_repetition;
    uint16_t                 n_id_scrambling;
    uint16_t                 n_id_0_scrambling;
  };

  rnti_t                                                              crnti;
  const bwp_configuration*                                            bwp_cfg;
  const pucch_resource*                                               res;
  std::variant<f0_config, f1_config, f2_config, f3_config, f4_config> format_params;
  pucch_uci_bits                                                      uci_bits;
  /// In case the PUCCH will contain CSI bits, this struct contains information how those bits are to be decoded.
  std::optional<csi_report_configuration> csi_rep_cfg;

  /// Returns the format of the PUCCH.
  constexpr pucch_format format() const
  {
    if (std::holds_alternative<f0_config>(format_params)) {
      return pucch_format::FORMAT_0;
    }
    if (std::holds_alternative<f1_config>(format_params)) {
      return pucch_format::FORMAT_1;
    }
    if (std::holds_alternative<f2_config>(format_params)) {
      return pucch_format::FORMAT_2;
    }
    if (std::holds_alternative<f3_config>(format_params)) {
      return pucch_format::FORMAT_3;
    }
    if (std::holds_alternative<f4_config>(format_params)) {
      return pucch_format::FORMAT_4;
    }
    return pucch_format::NOF_FORMATS;
  }

  /// Sets the format parameters to the appropiate type for the PUCCH format.
  void set_format(pucch_format format)
  {
    switch (format) {
      case pucch_format::FORMAT_0:
        format_params.emplace<f0_config>();
        break;
      case pucch_format::FORMAT_1:
        format_params.emplace<f1_config>();
        break;
      case pucch_format::FORMAT_2:
        format_params.emplace<f2_config>();
        break;
      case pucch_format::FORMAT_3:
        format_params.emplace<f3_config>();
        break;
      case pucch_format::FORMAT_4:
        format_params.emplace<f4_config>();
        break;
      default:
        ocudu_assertion_failure("Invalid PUCCH format");
    }
  }

  /// \brief Returns the PRB interval occupied by the PUCCH grant (first hop).
  ///
  /// \remark The number of PRBs used for a PUCCH Format 2/3 grant can be smaller than the number of PRBs configured for
  ///         the resource, depending on the payload size.
  prb_interval grant_prbs() const
  {
    switch (res->format()) {
      case pucch_format::FORMAT_2: {
        const auto& f2 = std::get<f2_config>(format_params);
        return prb_interval::start_and_len(res->starting_prb, f2.nof_prbs);
      } break;
      case pucch_format::FORMAT_3: {
        const auto& f3 = std::get<f3_config>(format_params);
        return prb_interval::start_and_len(res->starting_prb, f3.nof_prbs);
      } break;
      default:
        return res->prbs();
    }
  }
};

} // namespace ocudu
