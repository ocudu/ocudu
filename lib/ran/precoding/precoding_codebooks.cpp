// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/precoding/precoding_codebooks.h"
#include "ocudu/adt/interval.h"

using namespace ocudu;

precoding_weight_matrix ocudu::make_single_port()
{
  return make_one_layer_one_port(1, 0);
}

precoding_weight_matrix ocudu::make_one_layer_one_port(unsigned nof_ports, unsigned selected_i_port)
{
  interval<unsigned, false> selected_i_port_range(0, nof_ports);
  ocudu_assert(selected_i_port_range.contains(selected_i_port),
               "The given port identifier (i.e., {}) is out of the valid range {}",
               selected_i_port,
               selected_i_port_range);

  precoding_weight_matrix result(1, nof_ports);

  // Set weights per port.
  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    cf_t port_weight = (i_port == selected_i_port) ? 1.0F : 0.0F;
    result.set_coefficient(port_weight, 0, i_port);
  }

  return result;
}

precoding_weight_matrix ocudu::make_one_layer_all_ports(unsigned nof_ports)
{
  interval<unsigned, true> nof_ports_range(1, precoding_constants::MAX_NOF_PORTS);
  ocudu_assert(nof_ports_range.contains(nof_ports),
               "The number of ports (i.e., {}) is out of the valid range {}.",
               nof_ports,
               nof_ports_range);

  precoding_weight_matrix result(1, nof_ports);

  // Set normalized weights per port.
  cf_t weight = {1.0F / std::sqrt(static_cast<float>(nof_ports)), 0.0F};
  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    result.set_coefficient(weight, 0, i_port);
  }

  return result;
}

precoding_weight_matrix ocudu::make_identity(unsigned nof_streams)
{
  static constexpr interval<unsigned, true> nof_streams_range(1, precoding_constants::MAX_NOF_LAYERS);

  ocudu_assert(nof_streams_range.contains(nof_streams),
               "The number of streams (i.e., {}) is out of the valid range {}.",
               nof_streams,
               nof_streams_range);

  precoding_weight_matrix result(nof_streams, nof_streams);

  cf_t normalised_weight = 1.0F / std::sqrt(static_cast<float>(nof_streams));

  // Set weights per port.
  for (unsigned i_layer = 0; i_layer != nof_streams; ++i_layer) {
    for (unsigned i_port = 0; i_port != nof_streams; ++i_port) {
      cf_t weight = (i_layer == i_port) ? normalised_weight : 0.0F;
      result.set_coefficient(weight, i_layer, i_port);
    }
  }
  return result;
}
