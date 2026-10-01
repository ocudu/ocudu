// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/phy/generic_functions/transform_precoding/transform_precoding_factories.h"
#include "ocudu/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "ocudu/phy/upper/channel_processors/pucch/pucch_demodulator.h"
#include "ocudu/phy/upper/channel_processors/pucch/pucch_detector.h"
#include "ocudu/phy/upper/channel_processors/pucch/pucch_processor.h"
#include "ocudu/phy/upper/channel_processors/uci/factories.h"
#include "ocudu/phy/upper/equalization/equalization_factories.h"
#include "ocudu/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "ocudu/phy/upper/signal_processors/pucch/factories.h"

namespace ocudu {

class pucch_demodulator_factory
{
public:
  virtual ~pucch_demodulator_factory()                = default;
  virtual std::unique_ptr<pucch_demodulator> create() = 0;
};

std::shared_ptr<pucch_demodulator_factory>
create_pucch_demodulator_factory_sw(std::shared_ptr<channel_equalizer_factory>       equalizer_factory,
                                    std::shared_ptr<demodulation_mapper_factory>     demodulation_factory,
                                    std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                    std::shared_ptr<transform_precoder_factory>      precoder_factory);

class pucch_detector_factory
{
public:
  virtual ~pucch_detector_factory()                = default;
  virtual std::unique_ptr<pucch_detector> create() = 0;
};

std::shared_ptr<pucch_detector_factory>
create_pucch_detector_factory_sw(std::shared_ptr<low_papr_sequence_collection_factory> lpcf,
                                 std::shared_ptr<pseudo_random_generator_factory>      prgf,
                                 std::shared_ptr<channel_equalizer_factory>            eqzrf,
                                 std::shared_ptr<dft_processor_factory>                dftf);

class pucch_processor_factory
{
public:
  virtual ~pucch_processor_factory()                              = default;
  virtual std::unique_ptr<pucch_processor>     create()           = 0;
  virtual std::unique_ptr<pucch_pdu_validator> create_validator() = 0;
  virtual std::unique_ptr<pucch_processor>     create(ocudulog::basic_logger& logger);
};

std::shared_ptr<pucch_processor_factory>
create_pucch_processor_factory_sw(std::shared_ptr<dmrs_pucch_estimator_factory>        dmrs_factory,
                                  std::shared_ptr<pucch_detector_factory>              detector_factory,
                                  std::shared_ptr<pucch_demodulator_factory>           demodulator_factory,
                                  std::shared_ptr<uci_decoder_factory>                 decoder_factory,
                                  const channel_estimate::channel_estimate_dimensions& channel_estimate_dimensions);

std::shared_ptr<pucch_processor_factory>
create_pucch_processor_pool_factory(std::shared_ptr<pucch_processor_factory> factory, unsigned nof_concurrent_threads);

} // namespace ocudu
