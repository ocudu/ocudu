// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../test_utils/config_generators.h"
#include "../test_utils/sched_random_utils.h"
#include "lib/scheduler/config/du_cell_group_config_pool.h"
#include "lib/scheduler/logging/scheduler_metrics_handler.h"
#include "lib/scheduler/ue_context/ue.h"
#include "lib/scheduler/ue_context/ue_repository.h"
#include "ocudu/scheduler/config/logical_channel_config_factory.h"
#include "ocudu/scheduler/config/scheduler_expert_config_factory.h"
#include <gtest/gtest.h>

using namespace ocudu;

class ue_harq_link_adaptation_test : public ::testing::Test
{
protected:
  ue_harq_link_adaptation_test() :
    cfg_mng([]() {
      // Set nof. DL ports to 4.
      cell_config_builder_params params{};
      params.dl_carrier.nof_ant = 4;
      return params;
    }()),
    logger(ocudulog::fetch_basic_logger("SCHED", true)),
    ues(sched_cfg.ue)
  {
    logger.set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    // Create cell.
    const sched_cell_configuration_request_message sched_cell_cfg_req = cfg_mng.get_default_cell_config_request();
    cell_cfg                                                          = cfg_mng.add_cell(sched_cell_cfg_req);
    ues.add_cell(*cell_cfg, nullptr);
    next_slot = test_helper::generate_random_slot_point(cell_cfg->params.dl_cfg_common.init_dl_bwp.generic_params.scs);

    // Create UE.
    sched_ue_creation_request_message ue_creation_req = cfg_mng.get_default_ue_config_request();
    ue_creation_req.ue_index                          = to_du_ue_index(0);
    ue_creation_req.crnti = to_rnti(0x4601 + static_cast<unsigned>(ue_creation_req.ue_index));
    ue_creation_req.cfg.lc_config_list->clear();
    for (const lcid_t lcid : std::array<lcid_t, 3>{uint_to_lcid(1), uint_to_lcid(2), uint_to_lcid(4)}) {
      ue_creation_req.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(lcid));
    }
    ue_ded_cfg = cfg_mng.add_ue(ue_creation_req);
    report_error_if_not(ue_ded_cfg != nullptr, "Failed to create UE configuration");
    ues.add_ue(
        *ue_ded_cfg, ue_creation_req.starts_in_fallback, ue_creation_req.ul_ccch_slot_rx, ue_creation_req.cfra_enabled);
    ue_ptr = &ues[ue_creation_req.ue_index];
    ue_cc  = &ue_ptr->get_cell(SERVING_PCELL_IDX);
  }

  void run_slot()
  {
    ++next_slot;
    ue_ptr->slot_indication(next_slot);
  }

  dl_harq_process_handle handle_harq_newtx(unsigned k1 = 4)
  {
    const search_space_info& ss = ue_cc->cfg().search_space(to_search_space_id(2));

    dl_harq_process_handle h_dl = ue_cc->harqs.alloc_dl_harq(next_slot, k1, 4, 0).value();

    // Create dummy PDSCH grant.
    const pdsch_codeword    cw{sch_mcs_description{modulation_scheme::QAM256, 0.9},
                            sch_mcs_index{5},
                            pdsch_mcs_table::qam64,
                            0,
                            units::bytes{128}};
    const pdsch_information pdsch{ue_ptr->crnti,
                                  &ss.bwp->dl.cfg(),
                                  &ss.coreset->cfg(),
                                  vrb_alloc{vrb_interval{0, 5}},
                                  ss.pdsch_time_domain_list[0].symbols,
                                  {cw},
                                  {},
                                  ue_cc->cfg().cell_cfg_common.params.pci,
                                  2,
                                  vrb_to_prb::mapping_type::non_interleaved,
                                  search_space_set_type::ue_specific,
                                  dci_dl_format::f1_1,
                                  h_dl.id(),
                                  std::nullopt};
    const dl_msg_alloc      ue_pdsch{
        pdsch,
             {{dl_msg_tb_info{{dl_msg_lc_info{lcid_dl_sch_t{lcid_t::LCID_SRB1}, cw.tb_size_bytes.value() - 4, {}}}}}},
             {ue_ptr->ue_index}};

    dl_harq_alloc_context ctxt{dci_dl_rnti_config_type::c_rnti_f1_1, pdsch.codewords[0].mcs_index, std::nullopt, 15};
    h_dl.save_grant_params(ctxt, ue_pdsch);

    return h_dl;
  }

  const scheduler_expert_config           sched_cfg = config_helpers::make_default_scheduler_expert_config();
  test_helpers::test_sched_config_manager cfg_mng;
  const cell_configuration*               cell_cfg   = nullptr;
  const ue_configuration*                 ue_ded_cfg = nullptr;

  ocudulog::basic_logger& logger;

  ue_repository ues;
  ue*           ue_ptr = nullptr;
  ue_cell*      ue_cc  = nullptr;

  slot_point next_slot;
};

TEST_F(ue_harq_link_adaptation_test, harq_not_retx_when_cqi_drops_below_threshold)
{
  // Action: UE reports CQI value of 15.
  csi_report_data csi{};
  csi.first_tb_wideband_cqi = cqi_value{15};
  csi.valid                 = true;
  ue_cc->handle_csi_report(csi);

  dl_harq_process_handle h = handle_harq_newtx();

  while (h.uci_slot() != next_slot) {
    // Run until ACK slot.
    run_slot();
  }

  // Action: UE reports CQI value of 10 (CQI during new Tx was 15).
  csi.first_tb_wideband_cqi = cqi_value{10};
  ue_cc->handle_csi_report(csi);

  // Action: NACK the HARQ.
  bool ack_result = h.dl_ack_info(mac_harq_ack_report_status::nack, std::nullopt);
  report_fatal_error_if_not(ack_result, "dl_ack_info failed");

  // Result: There should not be retx for HARQ.
  ASSERT_EQ(ue_cc->harqs.find_pending_dl_retx(), std::nullopt) << "HARQ must not be retransmitted due to drop in CQI";
}

TEST_F(ue_harq_link_adaptation_test, harq_not_retx_when_ri_drops_below_threshold)
{
  // Action: UE reports RI value of 2.
  csi_report_data csi{};
  csi.first_tb_wideband_cqi = cqi_value{15};
  csi.ri                    = 2;
  csi.valid                 = true;
  ue_cc->handle_csi_report(csi);

  dl_harq_process_handle h = handle_harq_newtx();

  while (h.uci_slot() != next_slot) {
    // Run until ACK slot.
    run_slot();
  }

  // Action: UE reports RI value of 1.
  csi.ri = 1;
  ue_cc->handle_csi_report(csi);

  // Action: NACK the HARQ.
  bool ack_result = h.dl_ack_info(mac_harq_ack_report_status::nack, std::nullopt);
  report_fatal_error_if_not(ack_result, "dl_ack_info failed");

  // Result: There should not be retx for HARQ.
  ASSERT_EQ(ue_cc->harqs.find_pending_dl_retx(), std::nullopt) << "HARQ must not be retransmitted due to drop in RI";
}
