// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Precoding factories.

#pragma once

#include "ocudu/phy/generic_functions/precoding/channel_precoder.h"

namespace ocudu {

/// Factory that builds channel precoder objects.
class channel_precoder_factory
{
public:
  /// Default destructor.
  virtual ~channel_precoder_factory() = default;

  /// Creates and returns a channel precoder object.
  virtual std::unique_ptr<channel_precoder> create() = 0;
};

/// \brief Creates and returns a channel precoder factory.
///
/// \param[in] precoder_type A string indicating the precoder implementation to create. Values: generic, auto, avx2.
/// \return A channel precoder factory for the chosen precoder implementation.
std::shared_ptr<channel_precoder_factory> create_channel_precoder_factory(const std::string& precoder_type);

} // namespace ocudu
