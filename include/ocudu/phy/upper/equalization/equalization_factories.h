// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Channel equalization factories.

#pragma once

#include "ocudu/phy/upper/equalization/channel_equalizer.h"
#include "ocudu/phy/upper/equalization/channel_equalizer_algorithm_type.h"
#include <memory>

namespace ocudu {

/// Describes a channel equalizer factory.
class channel_equalizer_factory
{
public:
  /// Default destructor.
  virtual ~channel_equalizer_factory() = default;

  /// \brief Creates a channel equalizer.
  /// \return A unique pointer to a channel equalizer instance.
  virtual std::unique_ptr<channel_equalizer> create() = 0;
};

/// \brief Creates a generic channel equalizer factory.
/// \param[in] type Selects the channel equalizer algorithm type.
/// \return A generic channel equalizer factory.
std::shared_ptr<channel_equalizer_factory>
create_channel_equalizer_generic_factory(channel_equalizer_algorithm_type type = channel_equalizer_algorithm_type::zf);

} // namespace ocudu
