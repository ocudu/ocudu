// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_integer.h"
#include "ocudu/ran/csi_report/csi_report_configuration.h"
#include "ocudu/ran/csi_report/csi_report_data.h"
#include "ocudu/ran/csi_report/csi_report_packed.h"
#include "ocudu/ran/csi_report/csi_report_size.h"
#include "ocudu/ran/precoding/precoding_codebook_configuration.h"

namespace ocudu {

/// Collects the RI, LI, wideband CQI, and CSI fields bit-widths.
struct ri_li_cqi_cri_sizes {
  unsigned ri;
  unsigned li;
  unsigned wideband_cqi_first_tb;
  unsigned wideband_cqi_second_tb;
  unsigned subband_diff_cqi_first_tb;
  unsigned subband_diff_cqi_second_tb;
  unsigned cri;
};

/// \brief Gets the bit-widths of the RI, LI, wideband CQI, and CRI fields.
///
/// The bit-width of each of the fields is given in TS38.214 Table 6.3.1.1.2-5.
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

/// Collects the CRI (or SSBRI), RSRP, and differential RSRP fields bit-widths.
struct cri_ssbri_rsrp_sizes {
  /// CSI Resource Indicator or the SS/PBCH Block resource indicator fields bit-width.
  unsigned cri;
  /// RSRP measurement field bit-width.
  unsigned rsrp;
  /// Differential RSRP fields bit-width.
  unsigned diff_rsrp;
};

/// \brief Gets the bit-widths of the CRI, SSBRI, RSRP, and differential RSRP fields bit-widths.
///
/// The bit-width of each of the fields is given in TS38.214 Table 6.3.1.1.2-6.
///
/// \param[in] nof_csi_rs_resources The number of CSI-RS resources in the corresponding resource set, parameter
///                                 \f$K_{s}^{CSI-RS}\f$ or \f$K_{s}^{SSB}\f$ for CSI-RS or SSB resource sets
///                                 respectively.
cri_ssbri_rsrp_sizes get_cri_ssbri_rsrp_sizes(unsigned nof_csi_rs_resources);

/// \brief Gets the CSI report size for quantities \e cri-RSRP and \e ssb-Index-RSRP.
///
/// The CSI report for these quantities is given is TS38.212 Table 6.3.1.1.2-8. The calculation is common for PUCCH and
/// PUSCH.
///
/// \param[in] nof_csi_rs_resources The number of CSI-RS resources in the corresponding resource set, parameter
///                                 \f$K_{s}^{CSI-RS}\f$ or \f$K_{s}^{SSB}\f$ for CSI-RS or SSB resource sets
///                                 respectively.
/// \param[in] nof_reported_rs      Number of reported Resource Sets.
csi_report_size
get_csi_report_size_cri_ssbri_rsrp(unsigned                                             nof_csi_rs_resources,
                                   bounded_integer<uint8_t, 1, csi_max_nof_reported_rs> nof_reported_rs);

/// Gets the PMI field bit-width.
unsigned csi_report_get_size_pmi(const pmi_codebook_config& codebook, csi_report_data::ri_type ri);

/// Unpacks the wideband CQI field from a packed CSI report.
csi_report_data::wideband_cqi_type csi_report_unpack_wideband_cqi(csi_report_packed packed);

/// Unpacks the PMI fields from a packed CSI report.
precoding_matrix_indicator csi_report_unpack_pmi(const csi_report_packed&   packed,
                                                 const pmi_codebook_config& codebook,
                                                 csi_report_data::ri_type   ri);

/// Unpacks RI as per TS38.212 Section 6.3.1.1.2. and TS38.214 Section 5.2.2.2.1.
csi_report_data::ri_type csi_report_unpack_ri(const csi_report_packed&   ri_packed,
                                              const ri_restriction_type& ri_restriction);

/// \brief Unpacks \e cri-RSRP and \e ssb-Index-RSRP CSI reports as per TS38.212 Table 6.3.1.1.2-8.
///
/// \param[in] packed               Packed CSI report.
/// \param[in] nof_csi_rs_resources The number of CSI-RS resources in the corresponding resource set, parameter
///                                 \f$K_{s}^{CSI-RS}\f$ or \f$K_{s}^{SSB}\f$ for CSI-RS or SSB resource sets
///                                 respectively.
/// \param[in] nof_reported_rs      Number of reported Resource Sets.
csi_report_data csi_report_unpack_cri_ssbri_rsrp(const csi_report_packed& packed,
                                                 unsigned                 nof_csi_rs_resources,
                                                 bounded_integer<uint8_t, 1, csi_max_nof_reported_rs> nof_reported_rs);

} // namespace ocudu
