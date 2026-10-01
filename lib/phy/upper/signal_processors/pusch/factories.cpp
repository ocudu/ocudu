// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/upper/signal_processors/pusch/factories.h"
#include "dmrs_pusch_estimator_impl.h"
#include "ocudu/adt/format.h"

using namespace ocudu;

namespace {

class dmrs_pusch_estimator_factory_sw : public dmrs_pusch_estimator_factory
{
public:
  dmrs_pusch_estimator_factory_sw(std::shared_ptr<pseudo_random_generator_factory>     prg_factory_,
                                  std::shared_ptr<low_papr_sequence_generator_factory> low_papr_gen_factory_,
                                  std::shared_ptr<port_channel_estimator_factory>      ch_estimator_factory_,
                                  task_executor&                                       executor_,
                                  unsigned                                             nof_rx_ports_,
                                  port_channel_estimator_fd_smoothing_strategy         fd_smoothing_strategy_,
                                  port_channel_estimator_td_interpolation_strategy     td_interpolation_strategy_,
                                  bool                                                 compensate_cfo_) :
    prg_factory(std::move(prg_factory_)),
    low_papr_gen_factory(std::move(low_papr_gen_factory_)),
    ch_estimator_factory(std::move(ch_estimator_factory_)),
    executor(executor_),
    nof_rx_ports(nof_rx_ports_),
    fd_smoothing_strategy(fd_smoothing_strategy_),
    td_interpolation_strategy(td_interpolation_strategy_),
    compensate_cfo(compensate_cfo_)
  {
    ocudu_assert(prg_factory, "Invalid PRG factory.");
    ocudu_assert(low_papr_gen_factory, "Invalid low-PAPR generator factory.");
    ocudu_assert(ch_estimator_factory, "Invalid channel estimator factory.");
  }

  std::unique_ptr<dmrs_pusch_estimator> create() override
  {
    std::vector<std::unique_ptr<port_channel_estimator>> estimators;
    for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
      estimators.push_back(
          ch_estimator_factory->create(fd_smoothing_strategy, td_interpolation_strategy, compensate_cfo));
    }
    return std::make_unique<dmrs_pusch_estimator_impl>(
        prg_factory->create(), low_papr_gen_factory->create(), std::move(estimators), executor);
  }

private:
  std::shared_ptr<pseudo_random_generator_factory>     prg_factory;
  std::shared_ptr<low_papr_sequence_generator_factory> low_papr_gen_factory;
  std::shared_ptr<port_channel_estimator_factory>      ch_estimator_factory;
  task_executor&                                       executor;
  unsigned                                             nof_rx_ports;
  port_channel_estimator_fd_smoothing_strategy         fd_smoothing_strategy;
  port_channel_estimator_td_interpolation_strategy     td_interpolation_strategy;
  bool                                                 compensate_cfo;
};

} // namespace

std::shared_ptr<dmrs_pusch_estimator_factory> ocudu::create_dmrs_pusch_estimator_factory_sw(
    std::shared_ptr<pseudo_random_generator_factory>     prg_factory,
    std::shared_ptr<low_papr_sequence_generator_factory> low_papr_sequence_gen_factory,
    std::shared_ptr<port_channel_estimator_factory>      ch_estimator_factory,
    task_executor&                                       executor,
    unsigned                                             nof_rx_ports,
    port_channel_estimator_fd_smoothing_strategy         fd_smoothing_strategy,
    port_channel_estimator_td_interpolation_strategy     td_interpolation_strategy,
    bool                                                 compensate_cfo)
{
  return std::make_shared<dmrs_pusch_estimator_factory_sw>(std::move(prg_factory),
                                                           std::move(low_papr_sequence_gen_factory),
                                                           std::move(ch_estimator_factory),
                                                           executor,
                                                           nof_rx_ports,
                                                           fd_smoothing_strategy,
                                                           td_interpolation_strategy,
                                                           compensate_cfo);
}
