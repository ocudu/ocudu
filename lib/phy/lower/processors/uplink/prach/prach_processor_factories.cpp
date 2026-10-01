// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/lower/processors/uplink/prach/prach_processor_factories.h"
#include "prach_processor_impl.h"
#include "prach_processor_worker.h"
#include "ocudu/phy/lower/modulation/modulation_factories.h"
#include "ocudu/phy/lower/sampling_rate.h"

using namespace ocudu;

namespace {

class prach_processor_factory_sw : public prach_processor_factory
{
  std::shared_ptr<ofdm_prach_demodulator_factory> ofdm_prach_factory;
  task_executor&                                  async_task_executor;
  sampling_rate                                   srate;
  unsigned                                        max_nof_ports;
  unsigned                                        max_nof_concurrent_requests;

public:
  prach_processor_factory_sw(std::shared_ptr<ofdm_prach_demodulator_factory> ofdm_prach_factory_,
                             task_executor&                                  async_task_executor_,
                             sampling_rate                                   srate_,
                             unsigned                                        max_nof_ports_,
                             unsigned                                        max_nof_concurrent_requests_) :
    ofdm_prach_factory(std::move(ofdm_prach_factory_)),
    async_task_executor(async_task_executor_),
    srate(srate_),
    max_nof_ports(max_nof_ports_),
    max_nof_concurrent_requests(max_nof_concurrent_requests_)
  {
    ocudu_assert(ofdm_prach_factory, "Invalid OFDM PRACH factory.");
    ocudu_assert(max_nof_concurrent_requests, "Zero is an invalid maximum number of concurrent reqs.");
  }

  std::unique_ptr<prach_processor> create() override
  {
    std::vector<std::unique_ptr<prach_processor_worker>> workers;
    workers.reserve(max_nof_concurrent_requests);
    for (unsigned count = 0; count != max_nof_concurrent_requests; ++count) {
      workers.push_back(std::make_unique<prach_processor_worker>(
          ofdm_prach_factory->create(), async_task_executor, srate, max_nof_ports));
    }

    return std::make_unique<prach_processor_impl>(std::move(workers));
  }
};

} // namespace

std::shared_ptr<prach_processor_factory>
ocudu::create_prach_processor_factory_sw(std::shared_ptr<ofdm_prach_demodulator_factory> ofdm_prach_factory,
                                         task_executor&                                  async_task_executor,
                                         sampling_rate                                   srate,
                                         unsigned                                        max_nof_ports,
                                         unsigned                                        max_nof_concurrent_requests)
{
  return std::make_shared<prach_processor_factory_sw>(
      std::move(ofdm_prach_factory), async_task_executor, srate, max_nof_ports, max_nof_concurrent_requests);
}
