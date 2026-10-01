// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/generic_functions/dft_processor_ci16.h"
#include <memory>

namespace ocudu {

/// \brief Creates a DFT processor for 16-bit complex integer samples.
///
/// The DFT processor is templated to reduce interfaces overhead and this method isolates the factory source file from
/// including processor specific intrinsics headers.
///
/// \param dft_size DFT size
/// \param direction DFT direction.
/// \return A DFT processor if combination of DFT size and direction is implemented, otherwise nullptr.
std::unique_ptr<dft_processor_ci16> create_dft_processor_ci16_avx2(unsigned                      dft_size,
                                                                   dft_processor_ci16::direction direction);

} // namespace ocudu
