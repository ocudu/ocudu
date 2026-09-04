// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/pdcch/pdcch_candidates.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/pdcch/aggregation_level.h"
#include "ocudu/ran/rnti.h"

using namespace ocudu;

static pdcch_candidate_list pdcch_candidates_get_lowest_cce(unsigned Y_p,
                                                            unsigned n_ci,
                                                            unsigned L,
                                                            unsigned nof_candidates,
                                                            unsigned nof_cce_coreset)
{
  // Assertions avoid zero division.
  ocudu_assert(nof_cce_coreset > 0, "Number of CORESET CCEs must be greater than zero.");
  ocudu_assert(L > 0, "The aggregation level must be greater than zero.");
  ocudu_assert(L <= nof_cce_coreset or nof_candidates == 0,
               "The aggregation level must be less than or equal to the number of CORESET CCEs.");

  pdcch_candidate_list candidates;

  for (unsigned candidate = 0; candidate != nof_candidates; ++candidate) {
    unsigned n_cce = L * ((Y_p + (candidate * nof_cce_coreset) / (L * nof_candidates) + n_ci) % (nof_cce_coreset / L));
    candidates.push_back(n_cce);
  }

  return candidates;
}

static unsigned pdcch_candidates_ue_ss_get_Y_p(unsigned A_p, unsigned D, unsigned rnti, unsigned n)
{
  if (n == 0) {
    return (A_p * rnti) % D;
  }

  return (A_p * pdcch_candidates_ue_ss_get_Y_p(A_p, D, rnti, n - 1)) % D;
}

pdcch_candidate_list
ocudu::pdcch_candidates_common_ss_get_lowest_cce(const pdcch_candidates_common_ss_configuration& config)
{
  unsigned Y_p  = 0;
  unsigned n_ci = 0;

  return pdcch_candidates_get_lowest_cce(
      Y_p, n_ci, to_nof_cces(config.al), config.nof_candidates, config.nof_cce_coreset);
}

pdcch_candidate_list ocudu::pdcch_candidates_ue_ss_get_lowest_cce(const pdcch_candidates_ue_ss_configuration& config)
{
  constexpr static unsigned                D          = 65537;
  constexpr static std::array<unsigned, 3> A_p_values = {39827, 39829, 39839};
  constexpr static unsigned                n_ci       = 0;

  const unsigned A_p = A_p_values[static_cast<size_t>(config.cs_id) % A_p_values.size()];
  const unsigned Y_p = pdcch_candidates_ue_ss_get_Y_p(A_p, D, to_underlying(config.rnti), config.slot_index);

  return pdcch_candidates_get_lowest_cce(
      Y_p, n_ci, to_nof_cces(config.al), config.nof_candidates, config.nof_cce_coreset);
}
