// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcch_type0_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "ocudu/ran/pdcch/pdcch_type0_css_occasions.h"

using namespace ocudu;

slot_point ocudu::precompute_type0_pdcch_css_n0(search_space0_index       searchspace0,
                                                uint8_t                   coreset0,
                                                const cell_configuration& cell_cfg,
                                                subcarrier_spacing        scs_common,
                                                unsigned                  ssb_index)
{
  bool is_fr2 = band_helper::get_freq_range(cell_cfg.band()) == frequency_range::FR2;

  const pdcch_type0_css_coreset_description coreset0_param =
      pdcch_type0_css_coreset_get(cell_cfg.band(),
                                  cell_cfg.params.ssb_cfg.scs,
                                  scs_common,
                                  coreset0,
                                  static_cast<uint8_t>(cell_cfg.params.ssb_cfg.k_ssb.value()));

  ocudu_assert(coreset0_param.pattern == ssb_coreset0_mplex_pattern::mplx_pattern1,
               "SS/PBCH and CORESET multiplexing pattern not supported.");

  // Get Coreset0 num of symbols from Coreset0 config.
  const unsigned nof_symb_coreset0 = coreset0_param.nof_rb_coreset;

  const pdcch_type0_css_occasion_pattern1_description ss0_config_occasion_param =
      pdcch_type0_css_occasions_get_pattern1(pdcch_type0_css_occasion_pattern1_configuration{
          .is_fr2 = is_fr2, .ss0_index = searchspace0, .nof_symb_coreset = nof_symb_coreset0});

  const auto pdcch_slot = get_type0_pdcch_css_n0(
      static_cast<unsigned>(ss0_config_occasion_param.offset), ss0_config_occasion_param.M, scs_common, ssb_index);

  report_fatal_error_if_not(cell_cfg.is_dl_enabled(pdcch_slot), "PDCCH slot is not DL enabled.");

  return pdcch_slot;
}

slot_point ocudu::precompute_type0_pdcch_css_n0_plus_1(search_space0_index       searchspace0,
                                                       uint8_t                   coreset0,
                                                       const cell_configuration& cell_cfg,
                                                       subcarrier_spacing        scs_common,
                                                       unsigned                  ssb_index)
{
  const auto pdcch_slot = precompute_type0_pdcch_css_n0(searchspace0, coreset0, cell_cfg, scs_common, ssb_index) + 1;
  report_fatal_error_if_not(cell_cfg.is_dl_enabled(pdcch_slot), "PDCCH slot is not DL enabled.");
  return pdcch_slot;
}
