// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/upper/equalization/equalization_factories.h"
#include "channel_equalizer_generic_impl.h"
#include <memory>

using namespace ocudu;

namespace {

class channel_equalizer_generic_factory : public channel_equalizer_factory
{
public:
  channel_equalizer_generic_factory(channel_equalizer_algorithm_type type_) : type(type_) {}

  std::unique_ptr<channel_equalizer> create() override
  {
    return std::make_unique<channel_equalizer_generic_impl>(type);
  }

private:
  channel_equalizer_algorithm_type type;
};

} // namespace

std::shared_ptr<channel_equalizer_factory>
ocudu::create_channel_equalizer_generic_factory(channel_equalizer_algorithm_type type)
{
  return std::make_shared<channel_equalizer_generic_factory>(type);
}
