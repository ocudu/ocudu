// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "uci_test_utils.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/pdcch/coreset.h"
#include "ocudu/scheduler/config/sched_cell_config_helpers.h"
#include "ocudu/scheduler/config/scheduler_expert_config.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include "ocudu/scheduler/result/pdcch_info.h"
#include <type_traits>

using namespace ocudu;

pucch_info test_helpers::make_common_pucch_info(const bwp_configuration* bwp_cfg,
                                                unsigned                 pci,
                                                pucch_format             format,
                                                prb_interval             prbs,
                                                prb_interval             second_hop_prbs,
                                                ofdm_symbol_range        symbols,
                                                uint8_t                  initial_cyclic_shift,
                                                uint8_t                  time_domain_occ)
{
  pucch_info pucch_test{
      .crnti     = to_rnti(0x4601),
      .bwp_cfg   = bwp_cfg,
      .resources = {.prbs = prbs, .symbols = symbols, .second_hop_prbs = second_hop_prbs},
      .uci_bits  = {.harq_ack_nof_bits = 1U},
  };

  if (format == pucch_format::FORMAT_0) {
    auto& format_0                = pucch_test.format_params.emplace<pucch_format_0>();
    format_0.initial_cyclic_shift = initial_cyclic_shift;
    format_0.group_hopping        = pucch_group_hopping::NEITHER;
    format_0.n_id_hopping         = pci;
  } else if (format == pucch_format::FORMAT_1) {
    auto& format_1                = pucch_test.format_params.emplace<pucch_format_1>();
    format_1.initial_cyclic_shift = initial_cyclic_shift;
    format_1.time_domain_occ      = time_domain_occ;
    format_1.group_hopping        = pucch_group_hopping::NEITHER;
    format_1.n_id_hopping         = pci;
    format_1.slot_repetition      = pucch_repetition_tx_slot::no_multi_slot;
  } else {
    ocudu_assertion_failure("Invalid PUCCH format");
  }

  return pucch_test;
}

pucch_info test_helpers::make_ded_pucch_info(const cell_configuration& cell_cfg,
                                             const pucch_resource&     res,
                                             pucch_uci_bits            uci_bits,
                                             max_pucch_code_rate       max_code_rate)
{
  pucch_info info{.crnti    = to_rnti(0x4601),
                  .bwp_cfg  = &cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params,
                  .uci_bits = uci_bits};

  unsigned nof_prbs = 1;
  if (const auto* format_2_3 = std::get_if<pucch_format_2_3_cfg>(&res.format_params)) {
    nof_prbs = format_2_3->nof_prbs;
  }

  info.resources = pucch_resources{
      .prbs            = prb_interval::start_and_len(res.starting_prb, nof_prbs),
      .symbols         = ofdm_symbol_range::start_and_len(res.starting_sym_idx, res.nof_symbols),
      .second_hop_prbs = res.second_hop_prb.has_value() ? prb_interval::start_and_len(*res.second_hop_prb, nof_prbs)
                                                        : prb_interval{0U, 0U},
  };

  switch (res.format) {
    case pucch_format::FORMAT_0: {
      const auto& f0 = std::get<pucch_format_0_cfg>(res.format_params);
      info.format_params.emplace<pucch_format_0>(pucch_format_0{
          .group_hopping        = pucch_group_hopping::NEITHER,
          .n_id_hopping         = cell_cfg.params.pci,
          .initial_cyclic_shift = f0.initial_cyclic_shift,
      });
    } break;
    case pucch_format::FORMAT_1: {
      const auto& f1 = std::get<pucch_format_1_cfg>(res.format_params);
      info.format_params.emplace<pucch_format_1>(pucch_format_1{
          .group_hopping        = pucch_group_hopping::NEITHER,
          .n_id_hopping         = cell_cfg.params.pci,
          .initial_cyclic_shift = f1.initial_cyclic_shift,
          .time_domain_occ      = f1.time_domain_occ,
          .slot_repetition      = pucch_repetition_tx_slot::no_multi_slot,
      });
    } break;
    case pucch_format::FORMAT_2: {
      info.format_params.emplace<pucch_format_2>(pucch_format_2{
          .n_id_scrambling   = cell_cfg.params.pci,
          .n_id_0_scrambling = cell_cfg.params.pci,
          .max_code_rate     = max_code_rate,
      });
    } break;
    case pucch_format::FORMAT_3: {
      info.format_params.emplace<pucch_format_3>(pucch_format_3{
          .group_hopping     = pucch_group_hopping::NEITHER,
          .n_id_hopping      = cell_cfg.params.pci,
          .slot_repetition   = pucch_repetition_tx_slot::no_multi_slot,
          .n_id_scrambling   = cell_cfg.params.pci,
          .pi_2_bpsk         = false,
          .max_code_rate     = max_code_rate,
          .additional_dmrs   = false,
          .n_id_0_scrambling = cell_cfg.params.pci,
      });
    } break;
    case pucch_format::FORMAT_4: {
      const auto& f4 = std::get<pucch_format_4_cfg>(res.format_params);
      info.format_params.emplace<pucch_format_4>(pucch_format_4{
          .group_hopping     = pucch_group_hopping::NEITHER,
          .n_id_hopping      = cell_cfg.params.pci,
          .slot_repetition   = pucch_repetition_tx_slot::no_multi_slot,
          .n_id_scrambling   = cell_cfg.params.pci,
          .pi_2_bpsk         = false,
          .max_code_rate     = max_code_rate,
          .orthog_seq_idx    = static_cast<uint8_t>(f4.occ_index),
          .n_sf_pucch_f4     = static_cast<pucch_format_4_sf>(f4.occ_length),
          .additional_dmrs   = false,
          .n_id_0_scrambling = cell_cfg.params.pci,
      });
    } break;
    default:
      ocudu_assertion_failure("Invalid PUCCH format");
      break;
  }

  return info;
}

bool ocudu::pucch_info_match(const pucch_info& expected, const pucch_info& test)
{
  bool is_equal =
      expected.crnti == test.crnti && *expected.bwp_cfg == *test.bwp_cfg && expected.format() == test.format();
  is_equal = is_equal && expected.resources.prbs == test.resources.prbs &&
             expected.resources.symbols == test.resources.symbols &&
             expected.resources.second_hop_prbs == test.resources.second_hop_prbs;

  if (not is_equal) {
    return false;
  }

  switch (expected.format()) {
    case pucch_format::FORMAT_0: {
      const auto& expected_f = std::get<pucch_format_0>(expected.format_params);
      const auto& test_f     = std::get<pucch_format_0>(test.format_params);
      is_equal               = is_equal && expected_f.group_hopping == test_f.group_hopping &&
                 expected_f.n_id_hopping == test_f.n_id_hopping &&
                 expected_f.initial_cyclic_shift == test_f.initial_cyclic_shift &&
                 expected.uci_bits.sr_bits == test.uci_bits.sr_bits &&
                 expected.uci_bits.harq_ack_nof_bits == test.uci_bits.harq_ack_nof_bits;
    } break;
    case pucch_format::FORMAT_1: {
      const auto& expected_f = std::get<pucch_format_1>(expected.format_params);
      const auto& test_f     = std::get<pucch_format_1>(test.format_params);
      is_equal               = is_equal && expected_f.group_hopping == test_f.group_hopping &&
                 expected_f.n_id_hopping == test_f.n_id_hopping &&
                 expected_f.initial_cyclic_shift == test_f.initial_cyclic_shift &&
                 expected.uci_bits.sr_bits == test.uci_bits.sr_bits &&
                 expected.uci_bits.harq_ack_nof_bits == test.uci_bits.harq_ack_nof_bits &&
                 expected_f.slot_repetition == test_f.slot_repetition &&
                 expected_f.time_domain_occ == test_f.time_domain_occ;
    } break;
    case pucch_format::FORMAT_2: {
      const auto& expected_f = std::get<pucch_format_2>(expected.format_params);
      const auto& test_f     = std::get<pucch_format_2>(test.format_params);
      is_equal               = is_equal && expected_f.max_code_rate == test_f.max_code_rate &&
                 expected_f.n_id_scrambling == test_f.n_id_scrambling &&
                 expected_f.n_id_0_scrambling == test_f.n_id_0_scrambling &&
                 expected.uci_bits.sr_bits == test.uci_bits.sr_bits &&
                 expected.uci_bits.harq_ack_nof_bits == test.uci_bits.harq_ack_nof_bits &&
                 expected.uci_bits.csi_part1_nof_bits == test.uci_bits.csi_part1_nof_bits;
    } break;
    case pucch_format::FORMAT_3: {
      const auto& expected_f = std::get<pucch_format_3>(expected.format_params);
      const auto& test_f     = std::get<pucch_format_3>(test.format_params);
      is_equal               = is_equal && expected_f.group_hopping == test_f.group_hopping &&
                 expected_f.n_id_hopping == test_f.n_id_hopping && expected.uci_bits.sr_bits == test.uci_bits.sr_bits &&
                 expected.uci_bits.harq_ack_nof_bits == test.uci_bits.harq_ack_nof_bits &&
                 expected.uci_bits.csi_part1_nof_bits == test.uci_bits.csi_part1_nof_bits &&
                 expected_f.slot_repetition == test_f.slot_repetition &&
                 expected_f.n_id_scrambling == test_f.n_id_scrambling && expected_f.pi_2_bpsk == test_f.pi_2_bpsk &&
                 expected_f.max_code_rate == test_f.max_code_rate &&
                 expected_f.additional_dmrs == test_f.additional_dmrs &&
                 expected_f.n_id_0_scrambling == test_f.n_id_0_scrambling;
    } break;
    case pucch_format::FORMAT_4: {
      const auto& expected_f = std::get<pucch_format_4>(expected.format_params);
      const auto& test_f     = std::get<pucch_format_4>(test.format_params);
      is_equal =
          is_equal && expected_f.group_hopping == test_f.group_hopping &&
          expected_f.n_id_hopping == test_f.n_id_hopping && expected.uci_bits.sr_bits == test.uci_bits.sr_bits &&
          expected.uci_bits.harq_ack_nof_bits == test.uci_bits.harq_ack_nof_bits &&
          expected.uci_bits.csi_part1_nof_bits == test.uci_bits.csi_part1_nof_bits &&
          expected_f.slot_repetition == test_f.slot_repetition &&
          expected_f.n_id_scrambling == test_f.n_id_scrambling && expected_f.pi_2_bpsk == test_f.pi_2_bpsk &&
          expected_f.max_code_rate == test_f.max_code_rate && expected_f.orthog_seq_idx == test_f.orthog_seq_idx &&
          expected_f.n_sf_pucch_f4 == test_f.n_sf_pucch_f4 && expected_f.additional_dmrs == test_f.additional_dmrs &&
          expected_f.n_id_0_scrambling == test_f.n_id_0_scrambling;
    } break;
    default: {
      return false;
    };
  }

  return is_equal;
}

static cell_config_builder_params make_custom_cell_config_builder_params(const test_bench_params& params)
{
  cell_config_builder_params cfg_params{};
  cfg_params.scs_common             = params.tdd ? subcarrier_spacing::kHz30 : subcarrier_spacing::kHz15;
  cfg_params.dl_carrier.carrier_bw  = bs_channel_bandwidth::MHz10;
  cfg_params.dl_carrier.arfcn_f_ref = params.tdd ? 520000U : 365000U;
  cfg_params.dl_carrier.nof_ant     = params.nof_ul_dl_ports;

  if (params.tdd) {
    cfg_params.tdd_ul_dl_cfg_common = tdd_ul_dl_config_common{.ref_scs  = subcarrier_spacing::kHz30,
                                                              .pattern1 = {.dl_ul_tx_period_nof_slots = 10,
                                                                           .nof_dl_slots              = 6,
                                                                           .nof_dl_symbols            = 8,
                                                                           .nof_ul_slots              = 3,
                                                                           .nof_ul_symbols            = 0},
                                                              .pattern2 = std::nullopt};
  }

  return cfg_params;
}
static sched_cell_configuration_request_message
make_custom_sched_cell_configuration_request(const test_bench_params& params)
{
  sched_cell_configuration_request_message req = sched_config_helper::make_default_sched_cell_configuration_request(
      make_custom_cell_config_builder_params(params));

  if (params.pucch_res_common.has_value()) {
    req.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common = *params.pucch_res_common;
  } else {
    if (std::holds_alternative<pucch_f0_params>(params.pucch_ded_params.f0_or_f1_params)) {
      req.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common = 0U;
    } else {
      req.ran.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common = 11U;
    }
  }
  req.ran.init_bwp.pucch.resources = params.pucch_ded_params;
  req.ran.init_bwp.pucch.sr_period = params.sr_period;
  if (params.csi_period.has_value()) {
    req.ran.init_bwp.csi.value().csi_rs_period          = params.csi_period.value();
    req.ran.init_bwp.csi.value().csi_report_slot_offset = params.csi_offset;
  } else {
    req.ran.init_bwp.csi = std::nullopt;
  }

  return req;
}

static pdcch_dl_information make_default_dci(unsigned n_cces, const coreset_configuration* coreset_cfg)
{
  pdcch_dl_information dci{};
  dci.ctx.cces.ncce   = n_cces;
  dci.ctx.coreset_cfg = coreset_cfg;
  return dci;
}

test_bench::test_bench(const test_bench_params& params_) :
  params(params_),
  expert_cfg([]() {
    // Create default scheduler expert configuration and save it for the lifetime of the test bench.
    auto default_expert_cfg                            = config_helpers::make_default_scheduler_expert_config();
    default_expert_cfg.ue.min_pucch_pusch_prb_distance = 0U;
    return default_expert_cfg;
  }()),
  cfg_mng(make_custom_cell_config_builder_params(params), expert_cfg),
  cell_cfg(*cfg_mng.add_cell(make_custom_sched_cell_configuration_request(params))),
  ues(expert_cfg.ue),
  cell_ues(ues.add_cell(cell_cfg, nullptr)),
  pucch_builder(cell_cfg.expert_cfg.ue.max_pucchs_per_slot),
  res_grid(cell_cfg),
  k0(cell_cfg.params.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].k0),
  dci_info{make_default_dci(params.n_cces, &cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.value())},
  pucch_alloc{cell_cfg, params.max_pucchs_per_slot, params.max_ul_grants_per_slot},
  uci_alloc(cell_cfg, pucch_alloc),
  uci_sched{cell_cfg, uci_alloc, ues},
  sl_tx{to_numerology_value(cell_cfg.params.dl_cfg_common.init_dl_bwp.generic_params.scs), 0}
{
  pucch_builder.setup(cell_cfg.params);

  // Add main UE.
  add_ue();

  slot_indication(sl_tx);
}

const ue& test_bench::get_main_ue() const
{
  return get_ue(main_ue_idx);
}

const ue& test_bench::get_ue(du_ue_index_t ue_idx) const
{
  ocudu_assert(ues.contains(ue_idx), "User not found");
  return ues[ue_idx];
}

void test_bench::add_ue()
{
  sched_ue_creation_request_message ue_req =
      sched_config_helper::create_default_sched_ue_creation_request(cell_cfg.params);

  if (ue_ded_cfgs.empty()) {
    ue_req.ue_index = main_ue_idx;
    ue_req.crnti    = to_rnti(0x4601);
  } else {
    ue_req.ue_index =
        to_du_ue_index(static_cast<std::underlying_type_t<du_ue_index_t>>(ue_ded_cfgs.back()->ue_index) + 1);
    ue_req.crnti = to_rnti(static_cast<std::underlying_type_t<rnti_t>>(ue_ded_cfgs.back()->crnti) + 1);
  }

  const bool success = pucch_builder.add_build_new_ue_pucch_cfg(ue_req.cfg.cells->back());
  ocudu_assert(success, "UE PUCCH configuration couldn't be built");

  // TODO: rewrite this test, we should never modify the UE cell config like this in unittests.
  ue_req.cfg.cells->back().init_bwp().ul.pucch.sr_offset                                         = params.sr_offset;
  ue_req.cfg.cells->back().serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list[0].offset = params.sr_offset;
  if (params.csi_period.has_value()) {
    ue_req.cfg.cells->back().init_bwp().ul.periodic_csi_report.value().offset = params.csi_offset;
    auto& csi_report = std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
        ue_req.cfg.cells->back().serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list[0].report_cfg_type);
    csi_report.report_slot_offset = params.csi_offset;
  }

  const ue_configuration* ue_cfg = cfg_mng.add_ue(ue_req);
  ocudu_assert(ue_cfg != nullptr, "Failed to create UE configuration");
  ue_ded_cfgs.push_back(ue_cfg);
  ues.add_ue(*ue_ded_cfgs.back(), ue_req.starts_in_fallback, ue_req.ul_ccch_slot_rx, ue_req.cfra_enabled);
  uci_sched.add_ue(ues[ue_req.ue_index].get_pcell().cfg());

  last_added_ue_idx  = ue_req.ue_index;
  last_added_ue_rnti = ue_req.crnti;
}

void test_bench::slot_indication(slot_point slot_tx)
{
  pucch_alloc.slot_indication(slot_tx);
  mac_logger.set_context(slot_tx.sfn(), slot_tx.slot_index());
  test_logger.set_context(slot_tx.sfn(), slot_tx.slot_index());
  res_grid.slot_indication(slot_tx);
}

void test_bench::fill_all_grid(slot_point slot_tx)
{
  cell_slot_resource_allocator& pucch_slot_alloc = res_grid[slot_tx];
  pucch_slot_alloc.ul_res_grid.fill(grant_info{cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.scs,
                                               ofdm_symbol_range{0, 14},
                                               cell_cfg.params.ul_cfg_common.init_ul_bwp.generic_params.crbs});
}
