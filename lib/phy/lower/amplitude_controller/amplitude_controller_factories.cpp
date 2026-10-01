// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/lower/amplitude_controller/amplitude_controller_factories.h"
#include "amplitude_controller_clipping_impl.h"
#include "amplitude_controller_scaling_impl.h"
#include <memory>

using namespace ocudu;

namespace {

class amplitude_controller_clipping_factory : public amplitude_controller_factory
{
public:
  std::unique_ptr<amplitude_controller> create() override
  {
    return std::make_unique<amplitude_controller_clipping_impl>(amplitude_controller_config.enable_clipping,
                                                                amplitude_controller_config.input_gain_dB,
                                                                amplitude_controller_config.full_scale_lin,
                                                                amplitude_controller_config.ceiling_dBFS);
  }

  explicit amplitude_controller_clipping_factory(const amplitude_controller_clipping_config& config) :
    amplitude_controller_config(config)
  {
    ocudu_assert(
        config.ceiling_dBFS <= 0.0F, "The amplitude ceiling ({} dBFS) cannot be above Full Scale", config.ceiling_dBFS);
    ocudu_assert(config.full_scale_lin > 0.0F, "The Full Scale amplitude ({}) must be positive", config.full_scale_lin);
  }

private:
  amplitude_controller_clipping_config amplitude_controller_config;
};

class amplitude_controller_scaling_factory : public amplitude_controller_factory
{
public:
  std::unique_ptr<amplitude_controller> create() override
  {
    return std::make_unique<amplitude_controller_scaling_impl>(gain_dB);
  }

  explicit amplitude_controller_scaling_factory(float gain_dB_) : gain_dB(gain_dB_) {}

private:
  float gain_dB;
};

} // namespace

std::shared_ptr<amplitude_controller_factory>
ocudu::create_amplitude_controller_clipping_factory(const amplitude_controller_clipping_config& config)
{
  return std::make_shared<amplitude_controller_clipping_factory>(config);
}

std::shared_ptr<amplitude_controller_factory> ocudu::create_amplitude_controller_scaling_factory(float gain_dB_)
{
  return std::make_shared<amplitude_controller_scaling_factory>(gain_dB_);
}
