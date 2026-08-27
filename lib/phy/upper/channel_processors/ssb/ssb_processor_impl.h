// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/ssb/pbch_encoder.h"
#include "ocudu/phy/upper/channel_processors/ssb/pbch_modulator.h"
#include "ocudu/phy/upper/channel_processors/ssb/ssb_processor.h"
#include "ocudu/phy/upper/signal_processors/ssb/dmrs_pbch_processor.h"
#include "ocudu/phy/upper/signal_processors/ssb/pss_processor.h"
#include "ocudu/phy/upper/signal_processors/ssb/sss_processor.h"
#include "ocudu/support/math/math_utils.h"
#include "fmt/format.h"

namespace ocudu {

struct ssb_processor_config {
  std::unique_ptr<pbch_encoder>        encoder;
  std::unique_ptr<pbch_modulator>      modulator;
  std::unique_ptr<dmrs_pbch_processor> dmrs;
  std::unique_ptr<pss_processor>       pss;
  std::unique_ptr<sss_processor>       sss;
};

/// SSB processor implementation.
class ssb_processor_impl : public ssb_processor
{
  std::unique_ptr<pbch_encoder>        encoder;
  std::unique_ptr<pbch_modulator>      modulator;
  std::unique_ptr<dmrs_pbch_processor> dmrs;
  std::unique_ptr<pss_processor>       pss;
  std::unique_ptr<sss_processor>       sss;

public:
  ssb_processor_impl(ssb_processor_config config) :
    encoder(std::move(config.encoder)),
    modulator(std::move(config.modulator)),
    dmrs(std::move(config.dmrs)),
    pss(std::move(config.pss)),
    sss(std::move(config.sss))
  {
    // Do nothing
  }

  void process(resource_grid_writer& grid, const pdu_t& pdu) override;
};

} // namespace ocudu
