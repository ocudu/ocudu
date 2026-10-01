// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pdsch/factories.h"
#include "ocudu/phy/upper/channel_processors/pdsch/pdsch_processor.h"

namespace ocudu {

/// Implements a parameter validator for \ref pdsch_processor_impl.
class pdsch_processor_validator_impl : public pdsch_pdu_validator
{
public:
  // See interface for documentation.
  error_type<std::string> is_valid(const pdsch_processor::pdu_t& pdu) const override;
};

} // namespace ocudu
