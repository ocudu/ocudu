// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_low_config.h"
#include "ocudu/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc_factory.h"
#include "ocudu/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec_factory.h"

namespace ocudu {

/// O-RAN DU low unit HAL dependencies.
struct o_du_low_hal_dependencies {
  std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> hw_encoder_factory = nullptr;
  std::shared_ptr<hal::hw_accelerator_pusch_dec_factory> hw_decoder_factory = nullptr;
};

/// \brief Initializes the HAL depencies of the DU low unit.
/// \param[out] hal_config Struct defining the DU low HAL configuration
/// \return  Struct containing the DU low unit dependencies.
o_du_low_hal_dependencies make_du_low_hal_dependencies(const std::optional<du_low_unit_hal_config>& hal_config);

} // namespace ocudu
