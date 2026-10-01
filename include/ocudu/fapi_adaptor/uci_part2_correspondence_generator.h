// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi_adaptor/uci_part2_correspondence_mapper.h"
#include "ocudu/fapi_adaptor/uci_part2_correspondence_repository.h"

namespace ocudu {
namespace fapi_adaptor {

/// Generates the UCI Part2 correspondence mapper and repository for the given number of CSI-RS resources.
std::pair<std::unique_ptr<uci_part2_correspondence_mapper>, std::unique_ptr<uci_part2_correspondence_repository>>
generate_uci_part2_correspondence(unsigned nof_csi_rs_resources);

} // namespace fapi_adaptor
} // namespace ocudu
