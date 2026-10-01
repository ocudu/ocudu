// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_processor.h"

namespace ocudu {

/// Implements a parameter validator for \ref pusch_processor_impl.
class pusch_processor_validator_impl : public pusch_pdu_validator
{
public:
  /// \brief Constructs PUSCH processor validator.
  ///
  /// It requires channel estimate dimensions to limit the bandwidth, slot duration, number of receive ports and
  /// transmit layers.
  ///
  /// \param[in] ce_dims_ Provides the channel estimates dimensions.
  explicit pusch_processor_validator_impl(const pusch_processor::channel_size& ce_dims_);

  // See interface for documentation.
  error_type<std::string> is_valid(const pusch_processor::pdu_t& pdu) const override;

private:
  pusch_processor::channel_size ce_dims;
};

} // namespace ocudu
