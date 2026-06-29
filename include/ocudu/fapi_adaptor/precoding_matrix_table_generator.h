// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi_adaptor/precoding_matrix_mapper.h"
#include "ocudu/fapi_adaptor/precoding_matrix_repository.h"
#include <memory>

namespace ocudu {
namespace fapi_adaptor {

/// Generates the precoding matrix mapper and precoding matrix repository for the given number of antenna ports.
std::pair<std::unique_ptr<precoding_matrix_mapper>, std::unique_ptr<precoding_matrix_repository>>
generate_precoding_matrix_tables(const pmi_codebook_config& codebook_config, unsigned sector_id);

} // namespace fapi_adaptor
} // namespace ocudu
