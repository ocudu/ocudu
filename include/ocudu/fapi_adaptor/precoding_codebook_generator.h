// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi_adaptor/precoding_codebook_repository.h"
#include "ocudu/fapi_adaptor/precoding_matrix_mapper.h"
#include "ocudu/ran/antenna_topology.h"
#include <memory>

namespace ocudu {
namespace fapi_adaptor {

/// \brief Generates the precoding matrix mapper and the precoding codebook repository.
///
/// The codebook gives the precoding that the user equipment reports. The topology gives the antenna array that
/// transmits it. Both must describe the same number of antenna ports.
std::pair<std::unique_ptr<precoding_matrix_mapper>, std::unique_ptr<precoding_codebook_repository>>
generate_precoding_codebooks(const pmi_codebook_config& codebook_config, antenna_topology topology, unsigned sector_id);

} // namespace fapi_adaptor
} // namespace ocudu
