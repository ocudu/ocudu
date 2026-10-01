// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_coding/channel_coding_factories.h"
#include "ocudu/phy/upper/channel_processors/uci/uci_decoder.h"
#include <memory>

namespace ocudu {

class uci_decoder_factory
{
public:
  virtual ~uci_decoder_factory()                = default;
  virtual std::unique_ptr<uci_decoder> create() = 0;
};

std::shared_ptr<uci_decoder_factory>
create_uci_decoder_factory_generic(std::shared_ptr<short_block_detector_factory> decoder_factory,
                                   std::shared_ptr<polar_factory>                polar_factory,
                                   std::shared_ptr<crc_calculator_factory>       crc_calc_factory);

} // namespace ocudu
