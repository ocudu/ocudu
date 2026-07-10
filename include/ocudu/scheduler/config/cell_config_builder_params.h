// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/carrier_configuration.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/pdcch/search_space.h"
#include "ocudu/ran/ssb/ssb_properties.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"

namespace ocudu {

/// \brief Main cell parameters from which other cell parameters (e.g. coreset0, BWP RBs) will be derived.
/// \remark Only fields that may affect many different fields in du_cell_config (e.g. number of PRBs) should be added
/// in this struct.
struct cell_config_builder_params {
  /// Physical Cell Identity.
  pci_t pci = 1;
  /// subCarrierSpacingCommon, as per \c MIB, TS 38.331.
  subcarrier_spacing scs_common = subcarrier_spacing::kHz15;
  /// DL carrier configuration.
  /// \remark If nr_band is invalid, an appropriate band is derived from the provided DL ARFCN.
  /// \remark UL carrier is automatically derived.
  carrier_configuration dl_carrier{bs_channel_bandwidth::MHz10, 365000, nr_band::invalid, 1};
  /// offsetToPointA, as per TS 38.211, Section 4.4.4.2; \ref ssb_offset_to_pointA. If not specified, a valid offset
  /// is derived.
  std::optional<ssb_offset_to_pointA> offset_to_point_a;
  /// This is \c controlResourceSetZero, as per TS38.213, Section 13. If not specified, a valid coreset0 is derived.
  std::optional<coreset0_index> cs0_index;
  /// Maximum CORESET#0 duration in OFDM symbols to consider when deriving CORESET#0 index.
  uint8_t max_coreset0_duration = 2;
  /// This is \c searchSpaceZero, as per TS38.213, Section 13.
  search_space0_index ss0_index = 0;
  /// \brief k_ssb or SSB SubcarrierOffest, as per TS38.211 Section 7.4.3.1. Possible values: {0, ..., 23}. If not
  /// specified, a valid k_ssb is derived.
  std::optional<ssb_subcarrier_offset> k_ssb;
  /// Whether to enable CSI-RS in the cell.
  bool csi_rs_enabled = true;
  /// \brief Minimum k1 value used in the generation of the UE "dl-DataToUl-Ack", as per TS38.213, 9.1.2.1.
  /// Possible values: {1, ..., 15}.
  uint8_t min_k1 = 4;
  /// \brief Minimum k2 value used in the generation of the UE PUSCH time-domain resources. The value of min_k2
  /// should be equal or lower than min_k1. Otherwise, the scheduler is forced to pick higher k1 values, as it cannot
  /// allocate PUCCHs in slots where there is an PUSCH with an already assigned DAI.
  /// Possible values: {1, ..., 32}.
  uint8_t min_k2 = 4;
  /// Defines the TDD DL-UL pattern and periodicity. If no value is set, the cell is in FDD mode.
  std::optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common;
  /// Maximum number of DL layers.
  std::optional<unsigned> max_nof_layers;
  /// SSB subcarrier spacing.
  std::optional<subcarrier_spacing> scs_ssb;
  /// Whether UL frequency shift of 7.5 kHz is enabled.
  bool freq_shift_7p5khz = false;

  /// \brief Helper builder method to auto-derive dependent parameters, namely band, SSB SCS, nof DL layers,
  /// offset-to-pointA, k_SSB and coreset0 index.
  cell_config_builder_params& auto_derive_params()
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

    // Auto-derive nof layers if not provided.
    if (not max_nof_layers.has_value()) {
      max_nof_layers = dl_carrier.nof_ant;
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
      const unsigned nof_crbs = band_helper::get_n_rbs_from_bw(
          dl_carrier.carrier_bw, scs_common, band_helper::get_freq_range(dl_carrier.band));
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
};

} // namespace ocudu
