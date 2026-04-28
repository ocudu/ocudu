// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_manager_test_helpers.h"
#include "ocudu/du/du_high/du_qos_config_helpers.h"
#include "ocudu/mac/config/mac_cell_group_config_factory.h"
#include "ocudu/mac/config/mac_config_helpers.h"
#include "ocudu/rlc/rlc_srb_config_factory.h"
#include "ocudu/scheduler/config/ran_cell_config_helper.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"

using namespace ocudu;
using namespace odu;

dummy_ue_resource_configurator_factory::dummy_ue_resource_configurator_factory()
{
  next_context_update_result.srbs.emplace(srb_id_t::srb1);
  auto& new_srb   = next_context_update_result.srbs[srb_id_t::srb1];
  new_srb.srb_id  = srb_id_t::srb1;
  new_srb.rlc_cfg = make_default_srb_rlc_config();
  new_srb.mac_cfg = make_default_srb_mac_lc_config(LCID_SRB1);
  next_context_update_result.cell_group.cells.emplace(
      SERVING_PCELL_IDX, config_helpers::make_default_ue_cell_config(config_helpers::make_default_ran_cell_config()));
  next_context_update_result.cell_group.mcg_cfg = config_helpers::make_initial_mac_cell_group_config();
  next_context_update_result.cell_group.pcg_cfg = {}; // TODO
}

dummy_ue_resource_configurator_factory::dummy_resource_updater::dummy_resource_updater(
    dummy_ue_resource_configurator_factory& parent_,
    du_ue_index_t                           ue_index_) :
  ue_index(ue_index_), parent(parent_)
{
}
dummy_ue_resource_configurator_factory::dummy_resource_updater::~dummy_resource_updater()
{
  parent.ue_resource_pool.erase(ue_index);
}

du_ue_resource_update_response dummy_ue_resource_configurator_factory::dummy_resource_updater::update(
    du_cell_index_t                       pcell_index,
    const f1ap_ue_context_update_request& upd_req,
    const du_ue_resource_config*          reestablished_context,
    const ue_capability_summary*          reestablished_ue_caps)
{
  parent.ue_resource_pool[ue_index] = parent.next_context_update_result;
  return parent.next_config_resp;
}

const du_ue_resource_config& dummy_ue_resource_configurator_factory::dummy_resource_updater::get()
{
  return parent.ue_resource_pool[ue_index];
}

const std::optional<ue_capability_summary>&
dummy_ue_resource_configurator_factory::dummy_resource_updater::ue_capabilities() const
{
  return parent.next_ue_caps;
}

expected<ue_ran_resource_configurator, std::string>
dummy_ue_resource_configurator_factory::create_ue_resource_configurator(du_ue_index_t   ue_index,
                                                                        du_cell_index_t pcell_index,
                                                                        bool            has_tc_rnti)
{
  if (ue_resource_pool.count(ue_index) > 0) {
    return make_unexpected(std::string("Duplicate UE index"));
  }
  last_ue_index = ue_index;
  last_ue_pcell = pcell_index;
  ue_resource_pool.emplace(ue_index, du_ue_resource_config{});
  ue_resource_pool[ue_index].cell_group.cells.emplace(
      SERVING_PCELL_IDX, config_helpers::make_default_ue_cell_config(config_helpers::make_default_ran_cell_config()));
  ue_resource_pool[ue_index].cell_group.cells.at(SERVING_PCELL_IDX).serv_cell_cfg.cell_index = pcell_index;
  return ue_ran_resource_configurator{std::make_unique<dummy_resource_updater>(*this, ue_index)};
}

f1ap_ue_context_update_request
ocudu::odu::create_f1ap_ue_context_update_request(du_ue_index_t                   ue_idx,
                                                  std::initializer_list<srb_id_t> srbs_to_addmod,
                                                  std::initializer_list<drb_id_t> drbs_to_add,
                                                  std::initializer_list<drb_id_t> drbs_to_mod)
{
  f1ap_ue_context_update_request req = {};

  req.ue_index             = ue_idx;
  req.full_config_required = false;

  for (srb_id_t srb_id : srbs_to_addmod) {
    req.srbs_to_setup.emplace_back();
    req.srbs_to_setup.back() = srb_id;
  }

  for (drb_id_t drb_id : drbs_to_add) {
    auto& drb_to_setup                     = req.drbs_to_setup.emplace_back();
    drb_to_setup.drb_id                    = drb_id;
    drb_to_setup.mode                      = rlc_mode::am;
    drb_to_setup.qos_info.drb_qos.qos_desc = non_dyn_5qi_descriptor{uint_to_five_qi(9)};
    drb_to_setup.uluptnl_info_list.resize(1);
    drb_to_setup.uluptnl_info_list[0].gtp_teid   = int_to_gtpu_teid(0);
    drb_to_setup.uluptnl_info_list[0].tp_address = transport_layer_address::create_from_string("127.0.0.1");
  }

  for (drb_id_t drb_id : drbs_to_mod) {
    req.drbs_to_mod.emplace_back();
    req.drbs_to_mod.back().drb_id = drb_id;
    req.drbs_to_mod.back().uluptnl_info_list.resize(1);
    req.drbs_to_mod.back().uluptnl_info_list[0].gtp_teid   = int_to_gtpu_teid(0);
    req.drbs_to_mod.back().uluptnl_info_list[0].tp_address = transport_layer_address::create_from_string("127.0.0.1");
  }

  return req;
}

du_manager_test_bench::du_manager_test_bench(span<const du_cell_config> cells) :
  du_cells(cells.begin(), cells.end()),
  worker(128),
  du_mng_exec(worker),
  ue_exec_mapper(worker),
  cell_exec_mapper(worker),
  params{{"ocudu", (gnb_du_id_t)1, 1, du_cells},
         {timers, du_mng_exec, ue_exec_mapper, cell_exec_mapper},
         {f1ap, f1ap, f1ap},
         {f1u_teid_allocator, f1u_gw},
         {mac, f1ap, f1ap, rlc_pcap},
         {mac}},
  cell_mng(params),
  logger(ocudulog::fetch_basic_logger("DU-MNG"))
{
  logger.set_level(ocudulog::basic_levels::debug);

  ocudulog::init();

  params.ran.qos = config_helpers::make_default_du_qos_config_list(/* warn_on_drop */ true, 1000);

  // Add cells to cell manager.
  for (const auto& cell : du_cells) {
    cell_mng.add_cell(cell);
  }
}
