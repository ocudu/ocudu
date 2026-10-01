// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/pdcch/coreset.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "ocudu/ran/ssb/ssb_properties.h"

using namespace ocudu;

coreset_configuration::coreset_configuration(nr_band                 band_,
                                             subcarrier_spacing      ssb_scs_,
                                             subcarrier_spacing      pdcch_scs_,
                                             coreset0_index          coreset0_index_,
                                             ssb_subcarrier_offset   k_ssb_,
                                             ssb_offset_to_pointA    offset_to_point_a_,
                                             pci_t                   pci_,
                                             std::optional<unsigned> pdcch_dmrs_scrambling_id_) :
  id(to_coreset_id(0)),
  precoder_granurality(precoder_granularity_type::same_as_reg_bundle),
  pdcch_dmrs_scrambling_id(pdcch_dmrs_scrambling_id_),
  coreset0_idx(coreset0_index_)
{
  const pdcch_type0_css_coreset_description desc =
      pdcch_type0_css_coreset_get(band_, ssb_scs_, pdcch_scs_, coreset0_index_.value(), k_ssb_.value());
  ocudu_assert(desc.pattern != PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern, "Invalid CORESET#0 index value");

  dur                = desc.nof_symb_coreset;
  const int rb_start = pdcch_scs_ == subcarrier_spacing::kHz15
                           ? static_cast<int>(offset_to_point_a_.value()) - desc.offset
                           : static_cast<int>(offset_to_point_a_.value() / 2) - desc.offset;
  ocudu_assert(rb_start >= 0, "Coreset#0 CRB starts before pointA.");

  coreset0_rbs = {static_cast<unsigned>(rb_start), static_cast<unsigned>(rb_start) + desc.nof_rb_coreset};
  // CORESET#0 parameters as per TS 38.211 Section 7.3.2.2.
  interleaved = interleaved_mapping_type{6, 2, pci_};
}

coreset_configuration::coreset_configuration(coreset_id                              id_,
                                             freq_resource_bitmap                    freq_domain_resources_,
                                             unsigned                                duration_,
                                             std::optional<interleaved_mapping_type> interleaved_,
                                             precoder_granularity_type               precoder_granurality_,
                                             std::optional<unsigned>                 pdcch_dmrs_scrambling_id_) :
  id(id_),
  dur(duration_),
  interleaved(std::move(interleaved_)),
  precoder_granurality(precoder_granurality_),
  pdcch_dmrs_scrambling_id(pdcch_dmrs_scrambling_id_),
  other_coreset_freq_resources(freq_domain_resources_)
{
  ocudu_assert(id != to_coreset_id(0), "Invalid CORESET#0 ID for non-CORESET#0 constructor");
}
