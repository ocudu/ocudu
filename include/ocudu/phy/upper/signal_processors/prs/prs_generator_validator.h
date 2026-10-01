// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/expected.h"
#include <string>

namespace ocudu {

struct prs_generator_configuration;

/// \brief PRS generator validator.
class prs_generator_validator
{
public:
  /// Default destructor.
  virtual ~prs_generator_validator() = default;

  /// \brief Validates PRS generator configuration parameters.
  /// \return A success if the parameters contained in \c config are supported, an error message otherwise.
  virtual error_type<std::string> is_valid(const prs_generator_configuration& config) const = 0;
};

} // namespace ocudu
