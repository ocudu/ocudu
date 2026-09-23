// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/csi_report/csi_report_data.h"
#include "ocudu/ran/uci/uci_part2_size_description.h"
#include "ocudu/support/units.h"

namespace ocudu {

/// Collects the parameters that describe the CSI report size transmitted in PUCCH and PUSCH.
struct csi_report_size {
  /// \brief CSI Part 1 number of bits.
  ///
  /// It is the total number of CSI Part 1 reports described in TS38.212 Table 6.3.2.1.2-6.
  units::bits part1_size;

  /// \brief CSI Part 1 correspondence to CSI Part 2 size.
  ///
  /// It provides the necessary information for calculating the CSI Part 2 reports payload size from the CSI Part 1
  /// decoded payload. The CSI Part 2 payload is described in TS38.212 Table 6.3.2.1.2-7.
  uci_part2_size_description part2_correspondence;

  /// \brief CSI Part 2 minimum payload size.
  ///
  /// Minimum number of CSI Part 2 payload bits given \c part2_correspondence. It is calculated by
  /// adding the minimum CSI Part 2 report sizes for all entries in \c part2_correpondence.
  units::bits part2_min_size;

  /// \brief CSI Part 2 maximum payload size.
  ///
  /// Maximum number of CSI Part 2 payload bits given \c part2_correspondence. It is calculated by
  /// adding the maximum CSI Part 2 report sizes for all entries in \c part2_correpondence.
  units::bits part2_max_size;
};

/// Collects the RI, LI, wideband CQI, and CSI fields bit-widths.
struct ri_li_cqi_cri_sizes {
  unsigned ri;
  unsigned li;
  unsigned wideband_cqi_first_tb;
  unsigned wideband_cqi_second_tb;
  unsigned subband_diff_cqi_first_tb;
  unsigned subband_diff_cqi_second_tb;
  unsigned cri;
  unsigned nof_wideband_amplitudes;
};

/// \brief Gets the bit-widths of the RI, LI, wideband CQI, and CRI fields.
///
/// The bit-width of each of the fields is given in TS 38.214 Table 6.3.1.1.2-5.
///
/// \param[in] pmi_codebook         PMI codebook configuration.
/// \param[in] ri_restriction       The number of allowed rank indicator values, parameter \f$n_{RI}\f$.
/// \param[in] ri                   The value of the rank, parameter \f$\nu\f$.
/// \param[in] nof_csi_rs_resources The number of CSI-RS resources in the corresponding resource set, parameter
///                                 \f$K_{s}^{CSI-RS}\f$.
ri_li_cqi_cri_sizes get_ri_li_cqi_cri_sizes(const pmi_codebook_config& pmi_codebook,
                                            const ri_restriction_type& ri_restriction,
                                            csi_report_data::ri_type   ri,
                                            unsigned                   nof_csi_rs_resources);

/// \brief Gets the PMI field bit-width.
///
/// \param[in] codebook       PMI codebook configuration.
/// \param[in] ri             The value of the rank.
/// \param[in] nof_amplitudes Number of non-zero wideband amplitude coefficients \f$M_l\f$ for each of the reported
///                           layers. It is only required by the Type II codebook.
///
/// \return Size in bits of the reported Precoding Matrix Indicator.
unsigned csi_report_get_size_pmi(const pmi_codebook_config&   codebook,
                                 csi_report_data::ri_type     ri,
                                 const typeII_nof_amplitudes& nof_amplitudes = {});

} // namespace ocudu
