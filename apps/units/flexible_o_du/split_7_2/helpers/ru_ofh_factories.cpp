// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ru_ofh_factories.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_o_du/split_helpers/flexible_o_du_configs.h"
#include "ru_ofh_config_translator.h"
#include "ocudu/adt/format.h"
#include "ocudu/ru/ofh/ru_ofh_factory.h"

using namespace ocudu;

std::unique_ptr<radio_unit> ocudu::create_ofh_radio_unit(const ru_ofh_unit_config&            ru_cfg,
                                                         const flexible_o_du_ru_config&       ru_config,
                                                         const flexible_o_du_ru_dependencies& ru_dependencies,
                                                         bool                                 uses_dpdk)
{
  auto& ofh_exec_map = ru_dependencies.workers.get_ofh_ru_executor_mapper();

  ru_ofh_dependencies dependencies;

  dependencies.logger             = &ocudulog::fetch_basic_logger("OFH");
  dependencies.rt_timing_executor = &ofh_exec_map.timing_executor();
  dependencies.timing_notifier    = &ru_dependencies.timing_notifier;
  dependencies.rx_symbol_notifier = &ru_dependencies.symbol_notifier;
  dependencies.error_notifier     = &ru_dependencies.error_notifier;

  // Configure sector.
  dependencies.sector_dependencies.reserve(ru_cfg.cells.size());
  for (unsigned i = 0, e = ru_cfg.cells.size(); i != e; ++i) {
    // The sector executor mapper provides the txrx, uplink and downlink executors.
    dependencies.sector_dependencies.emplace_back(
        ofh::sector_dependencies{.logger = dependencies.logger, .exec_mapper = ofh_exec_map[i]});
  }

  return create_ofh_ru(generate_ru_ofh_config(ru_cfg, ru_config.cells, ru_config.max_processing_delay, uses_dpdk),
                       std::move(dependencies));
}
