// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/config/cell_config_builder_params.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

cell_config_builder_params& cell_config_builder_params::auto_derive_params()
{
  // Auto-derive band if not derived yet.
  if (dl_carrier.band == nr_band::invalid) {
    dl_carrier.band = band_helper::get_band_from_dl_arfcn(dl_carrier.arfcn_f_ref);
  } else {
    auto err = band_helper::is_dl_arfcn_valid_given_band(
        dl_carrier.band, dl_carrier.arfcn_f_ref, scs_common, dl_carrier.carrier_bw);
    report_error_if_not(err.has_value(),
                        "DL ARFCN {} is not valid for band {}. Cause: {}\n",
                        dl_carrier.arfcn_f_ref,
                        static_cast<unsigned>(dl_carrier.band),
                        err.error());
  }

  // Auto-derive SSB SCS.
  if (not scs_ssb.has_value()) {
    scs_ssb = band_helper::get_most_suitable_ssb_scs(dl_carrier.band, scs_common);
  }

  // Auto-derive offset_to_pointA, k_ssb and coreset0 index.
  if (not cs0_index.has_value() or not offset_to_point_a.has_value() or not k_ssb.has_value()) {
    if (offset_to_point_a.has_value() or k_ssb.has_value()) {
      report_error("The user either sets {controlResourceSetZero, offsetToPointA, kSSB} or just "
                   "{controlResourceSetZero}, or none of them.\n");
    }
    const unsigned nof_crbs =
        band_helper::get_n_rbs_from_bw(dl_carrier.carrier_bw, scs_common, band_helper::get_freq_range(dl_carrier.band));
    ocudu_assert(nof_crbs > 0, "Invalid builder params");

    std::optional<band_helper::ssb_coreset0_freq_location> ssb_freq_loc;
    if (cs0_index.has_value()) {
      ssb_freq_loc = band_helper::get_ssb_coreset0_freq_location_for_cset0_idx(
          dl_carrier.arfcn_f_ref, dl_carrier.band, nof_crbs, scs_common, *scs_ssb, ss0_index, *cs0_index);
    } else {
      ssb_freq_loc = band_helper::get_ssb_coreset0_freq_location(
          dl_carrier.arfcn_f_ref, dl_carrier.band, nof_crbs, scs_common, *scs_ssb, ss0_index, max_coreset0_duration);
    }
    report_error_if_not(
        ssb_freq_loc.has_value(), "Unable to derive a valid SSB pointA and k_SSB for cell id ({}).\n", pci);
    offset_to_point_a = ssb_freq_loc->offset_to_point_A;
    k_ssb             = ssb_freq_loc->k_ssb;
    cs0_index         = ssb_freq_loc->coreset0_idx;
  }

  return *this;
}
