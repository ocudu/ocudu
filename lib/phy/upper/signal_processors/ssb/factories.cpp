// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/upper/signal_processors/ssb/factories.h"
#include "dmrs_pbch_processor_impl.h"
#include "pss_processor_impl.h"
#include "sss_processor_impl.h"

using namespace ocudu;

namespace {

class dmrs_pbch_processor_sw_factory : public dmrs_pbch_processor_factory
{
private:
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;

public:
  explicit dmrs_pbch_processor_sw_factory(std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    prg_factory(std::move(prg_factory_))
  {
    ocudu_assert(prg_factory, "Invalid PRG factory.");
  }

  std::unique_ptr<dmrs_pbch_processor> create() override
  {
    return std::make_unique<dmrs_pbch_processor_impl>(prg_factory->create());
  }
};

class pss_processor_factory_sw : public pss_processor_factory
{
public:
  std::unique_ptr<pss_processor> create() override { return std::make_unique<pss_processor_impl>(); }
};

class sss_processor_factory_sw : public sss_processor_factory
{
public:
  std::unique_ptr<sss_processor> create() override { return std::make_unique<sss_processor_impl>(); }
};

} // namespace

std::shared_ptr<dmrs_pbch_processor_factory>
ocudu::create_dmrs_pbch_processor_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<dmrs_pbch_processor_sw_factory>(std::move(prg_factory));
}

std::shared_ptr<pss_processor_factory> ocudu::create_pss_processor_factory_sw()
{
  return std::make_shared<pss_processor_factory_sw>();
}

std::shared_ptr<sss_processor_factory> ocudu::create_sss_processor_factory_sw()
{
  return std::make_shared<sss_processor_factory_sw>();
}
