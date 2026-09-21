// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/precoding/precoding_codebook_configuration.h"
#include "ocudu/ran/precoding/precoding_codebook_type2_helpers.h"
#include <gtest/gtest.h>

using namespace ocudu;

// Verify that every codebook identifier maps onto a configuration that maps back onto the same identifier. It guards
// the arithmetic in to_pmi_codebook_identifier() against the order of the codebook configuration list.
TEST(precoding_codebook_configuration_test, every_identifier_round_trips)
{
  for (unsigned id = 0, id_end = pmi_codebook_id::max() + 1; id != id_end; ++id) {
    const pmi_codebook_config& codebook = to_pmi_codebook_config(id);

    ASSERT_EQ(to_pmi_codebook_identifier(codebook).value(), id)
        << "Codebook " << to_string(codebook) << " does not map back onto its identifier.";
  }
}

// Verify that the Type II identifiers cover the codebook configurations that a DU cell can be configured with.
TEST(precoding_codebook_configuration_test, typeII_identifiers_cover_the_supported_configurations)
{
  unsigned nof_typeII_codebooks = 0;

  for (pmi_codebook_single_panel_config n1_n2 :
       {pmi_codebook_single_panel_config::two_one, pmi_codebook_single_panel_config::four_one}) {
    // As per TS38.214 Section 5.2.2.2.3, the number of beams cannot exceed N1 * N2, which is two for the (2, 1) panel
    // topology and four for the (4, 1) one.
    unsigned max_nof_beams = (n1_n2 == pmi_codebook_single_panel_config::two_one) ? 2 : max_nof_typeII_beams;

    for (unsigned nof_beams = 2; nof_beams <= max_nof_beams; ++nof_beams) {
      for (pmi_codebook_typeII_phase_size phase_size :
           {pmi_codebook_typeII_phase_size::qpsk, pmi_codebook_typeII_phase_size::psk8}) {
        const pmi_codebook_config codebook = pmi_codebook_typeII{n1_n2, nof_beams, phase_size, false};

        pmi_codebook_id id = to_pmi_codebook_identifier(codebook);
        ASSERT_EQ(to_pmi_codebook_identifier(to_pmi_codebook_config(id)).value(), id.value())
            << "Codebook " << to_string(codebook) << " does not map onto its own configuration.";

        ++nof_typeII_codebooks;
      }
    }
  }

  ASSERT_EQ(nof_typeII_codebooks, 8);
}

// Verify that the Type II codebooks report at most two layers, as per TS38.214 Section 5.2.2.2.3.
TEST(precoding_codebook_configuration_test, typeII_codebooks_report_at_most_two_layers)
{
  for (unsigned id = 0, id_end = pmi_codebook_id::max() + 1; id != id_end; ++id) {
    const pmi_codebook_config& codebook = to_pmi_codebook_config(id);
    if (!std::holds_alternative<pmi_codebook_typeII>(codebook)) {
      continue;
    }

    ASSERT_LE(get_precoding_codebook_max_rank(codebook), max_nof_typeII_layers);
    ASSERT_GE(get_precoding_codebook_antenna_ports(codebook), 4);
  }
}
