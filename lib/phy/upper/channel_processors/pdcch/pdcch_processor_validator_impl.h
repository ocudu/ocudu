// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pdcch/pdcch_processor.h"
#include "ocudu/ran/pdcch/pdcch_constants.h"

namespace ocudu {

/// Implements a parameter validator for \ref pdcch_processor_impl.
class pdcch_processor_validator_impl : public pdcch_pdu_validator
{
public:
  // See interface for documentation.
  error_type<std::string> is_valid(const pdcch_processor::pdu_t& pdu) const override;
};

} // namespace ocudu
