// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/config/cell_configuration.h"
#include "scheduler_test_doubles.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "tests/unittests/scheduler/test_utils/indication_generators.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include "ocudu/scheduler/result/sched_result.h"
#include "ocudu/scheduler/scheduler_factory.h"
#include "ocudu/support/benchmark_utils.h"
#include <getopt.h>

using namespace ocudu;

struct bench_params {
  unsigned nof_repetitions = 100;
};

static void usage(const char* prog, const bench_params& params)
{
  fmt::print("Usage: {} [-R repetitions] [-s silent]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", params.nof_repetitions);
  fmt::print("\t-h Show this message\n");
}

static void parse_args(int argc, char** argv, bench_params& params)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:h")) != -1) {
    switch (opt) {
      case 'R':
        params.nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'h':
      default:
        usage(argv[0], params);
        std::exit(0);
    }
  }
}

static std::unique_ptr<benchmarker> bm;

void benchmark_sib_scheduling()
{
  sched_cfg_dummy_notifier       cfg_notif;
  sched_dummy_metric_notifier    metric_notif;
  std::unique_ptr<mac_scheduler> sch =
      create_scheduler(scheduler_config{config_helpers::make_default_scheduler_expert_config(), cfg_notif});

  // Add Cell.
  scheduler_expert_config                  sched_cfg = config_helpers::make_default_scheduler_expert_config();
  sched_cell_configuration_request_message cell_cfg_msg =
      sched_config_helper::make_default_sched_cell_configuration_request();
  test_helpers::test_sched_config_manager cfg_mng{sched_cfg};
  cfg_mng.add_cell(cell_cfg_msg);
  sch->handle_cell_configuration_request(cell_cfg_msg);

  auto& logger = ocudulog::fetch_basic_logger("SCHED", true);

  // Run benchmark.
  slot_point_extended sl_tx{subcarrier_spacing::kHz15, 0};
  bm->new_measure("SSB+SIB scheduling", 1, [&sch, &sl_tx, &logger]() mutable {
    logger.set_context(sl_tx.sfn(), sl_tx.slot_index());
    sch->slot_indication(sl_tx, to_du_cell_index(0));
    ++sl_tx;
  });
}

void benchmark_rach_scheduling()
{
  sched_cfg_dummy_notifier       cfg_notif;
  sched_dummy_metric_notifier    metric_notif;
  std::unique_ptr<mac_scheduler> sch =
      create_scheduler(scheduler_config{config_helpers::make_default_scheduler_expert_config(), cfg_notif});

  // Add Cell.
  scheduler_expert_config                  sched_cfg = config_helpers::make_default_scheduler_expert_config();
  sched_cell_configuration_request_message cell_cfg_msg =
      sched_config_helper::make_default_sched_cell_configuration_request();
  test_helpers::test_sched_config_manager cfg_mng{sched_cfg};
  const cell_configuration&               cell_cfg = *cfg_mng.add_cell(cell_cfg_msg);
  sch->handle_cell_configuration_request(cell_cfg_msg);

  auto&                   logger = ocudulog::fetch_basic_logger("SCHED", true);
  slot_point_extended     sl_tx{subcarrier_spacing::kHz15, 0};
  rach_indication_message rach_ind = test_helper::create_rach_indication(
      cell_cfg, sl_tx.without_hyper_sfn() - 4, {test_helper::create_preamble(0, to_rnti(0x4601))});

  // Run benchmark.
  bm->new_measure("SSB+SIB+RACH scheduling", 1, [&sch, &sl_tx, &rach_ind, &logger]() mutable {
    logger.set_context(sl_tx.sfn(), sl_tx.slot_index());
    if (sl_tx.slot_index() != 0) {
      // Avoid slots with SIB1, otherwise there might not be space in PDCCH.
      sch->handle_rach_indication(rach_ind);
    }
    const sched_result* res = &sch->slot_indication(sl_tx, to_du_cell_index(0));

    // ack msg3s.
    if (not res->ul.puschs.empty()) {
      ul_crc_indication crc;
      crc.cell_index = to_du_cell_index(0);
      crc.sl_rx      = sl_tx.without_hyper_sfn() - 4;
      for (const ul_sched_info& ulinfo : res->ul.puschs) {
        crc.crcs.push_back(ul_crc_pdu_indication{
            ulinfo.pusch_cfg.rnti, INVALID_DU_UE_INDEX, to_harq_id(ulinfo.pusch_cfg.harq_id), true});
      }
      sch->handle_crc_indication(crc);
    }

    // update slot.
    ++sl_tx;
    rach_ind.slot_rx++;
    rach_ind.occasions.back().preambles.back().tc_rnti =
        to_rnti(1 + (unsigned)rach_ind.occasions.back().preambles.back().tc_rnti);
  });
}

int main(int argc, char** argv)
{
  ocudulog::fetch_basic_logger("MAC", true).set_level(ocudulog::basic_levels::warning);
  ocudulog::fetch_basic_logger("SCHED", true).set_level(ocudulog::basic_levels::warning);

  ocudulog::init();

  bench_params params{};
  parse_args(argc, argv, params);

  bm = std::make_unique<benchmarker>("scheduler", params.nof_repetitions);

  benchmark_sib_scheduling();
  benchmark_rach_scheduling();

  // Output results.
  bm->print_percentiles_time();
}
