// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "srs_validator_generic_impl.h"
#include "ocudu/phy/upper/signal_processors/srs/srs_estimator_configuration.h"
#include "ocudu/ran/resource_block.h"
#include "ocudu/ran/srs/srs_information.h"

using namespace ocudu;

error_type<std::string> srs_validator_generic_impl::is_valid(const srs_estimator_configuration& config) const
{
  // Check the SRS resource is valid.
  if (!config.resource.is_valid()) {
    return make_unexpected("Invalid SRS resource configuration.");
  }

  // Frequency hopping is not supported.
  if (config.resource.has_frequency_hopping()) {
    return make_unexpected("The SRS estimator does not support frequency hopping.");
  }

  // Sequence and group hopping is not supported.
  if (config.resource.hopping != srs_group_or_sequence_hopping::neither) {
    return make_unexpected("The SRS estimator does not support group or sequence hopping.");
  }

  // Invalid receive port list.
  if (config.ports.empty()) {
    return make_unexpected("Empty list of Rx ports for the SRS estimator.");
  }

  for (unsigned i_port = 0, i_port_end = config.ports.size(); i_port != i_port_end; ++i_port) {
    srs_information info = get_srs_information(config.resource, i_port);

    if (info.mapping_initial_subcarrier + (info.sequence_length - 1) * info.comb_size >=
        max_nof_prb * NOF_SUBCARRIERS_PER_RB) {
      return make_unexpected("SRS resource exceeds maximum bandwidth.");
    }
  }

  return default_success_t();
}
