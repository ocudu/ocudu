// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_cell_group_config_pool.h"
#include "cell_configuration.h"
#include "ocudu/ran/bwp/bwp_id.h"
#include "ocudu/scheduler/config/time_domain_resource_helper.h"
#include "ocudu/scheduler/config/ue_bwp_config.h"
#include "ocudu/scheduler/scheduler_configurator.h"

using namespace ocudu;

static pdsch_time_domain_mapper make_pdsch_td_mapper(const ran_cell_config& cfg)
{
  pdsch_time_domain_builder_params params;
  params.cp      = cfg.dl_cfg_common.init_dl_bwp.generic_params.cp;
  params.tdd_cfg = cfg.tdd_cfg;

  pdsch_time_domain_builder_params::explicit_resources explicit_res;
  explicit_res.pdsch_td_res_list = cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list;
  params.params                  = std::move(explicit_res);

  return pdsch_time_domain_mapper(params);
}

static pusch_time_domain_mapper make_pusch_td_mapper(const ran_cell_config& cfg)
{
  pusch_time_domain_builder_params params;
  params.cp      = cfg.ul_cfg_common.init_ul_bwp.generic_params.cp;
  params.tdd_cfg = cfg.tdd_cfg;

  pusch_time_domain_builder_params::explicit_resources explicit_res;
  explicit_res.pusch_td_res_list =
      cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common.has_value()
          ? cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common->pusch_td_alloc_list
          : time_domain_resource_helper::generate_dedicated_pusch_td_res_list(
                cfg.tdd_cfg, cfg.ul_cfg_common.init_ul_bwp.generic_params.cp, cfg.init_bwp.pusch.min_k2);
  params.params = std::move(explicit_res);

  return pusch_time_domain_mapper(params);
}

static pucch_time_domain_mapper make_pucch_td_mapper(const ran_cell_config& cfg)
{
  pucch_time_domain_builder_params params;
  params.tdd_cfg = cfg.tdd_cfg;

  pucch_time_domain_builder_params::explicit_resources explicit_res;
  const auto                                           dl_data_to_ul_ack =
      time_domain_resource_helper::generate_k1_candidates(cfg.tdd_cfg, cfg.init_bwp.pucch.min_k1);
  explicit_res.k1_candidates.assign(dl_data_to_ul_ack.begin(), dl_data_to_ul_ack.end());
  params.params = std::move(explicit_res);

  return pucch_time_domain_mapper(params);
}

bwp_config_pool::bwp_config_pool(const ran_cell_config&     cell_ran_cfg,
                                 bwp_id_t                   bwpid,
                                 const cell_bwp_res_config& bwp_ded_res) :
  bwp_id(bwpid),
  bwp_dl_cmn(cell_ran_cfg.dl_cfg_common.init_dl_bwp),
  bwp_ul_cmn(cell_ran_cfg.ul_cfg_common.init_ul_bwp),
  pdsch_td_mapper(make_pdsch_td_mapper(cell_ran_cfg)),
  pusch_td_mapper(make_pusch_td_mapper(cell_ran_cfg)),
  pucch_td_mapper(make_pucch_td_mapper(cell_ran_cfg)),
  pdcch_pool(cell_ran_cfg.pci, bwp_dl_cmn.generic_params, bwp_dl_cmn.pdcch_common, bwp_ded_res.dl),
  common_bwp_cfg{bwp_id,
                 sched_bwp_dl_config{bwp_dl_cmn, nullptr, pdcch_pool.init_cfg(), pdsch_td_mapper},
                 sched_bwp_ul_config{bwp_ul_cmn, std::nullopt, std::nullopt, pusch_td_mapper, pucch_td_mapper}}
{
}

sched_bwp_config bwp_config_pool::add_ded_cfg(const bwp_downlink_dedicated* dl_ded,
                                              const bwp_uplink_dedicated*   ul_ded,
                                              const ue_bwp_config&          ue_bwp_cfg)
{
  config_ptr<bwp_downlink_dedicated> dl_ptr;
  if (dl_ded != nullptr) {
    dl_ptr = dl_ded_config_pool.create(*dl_ded);
  }
  // The UL dedicated config (PUCCH/SR/SRS) is high-cardinality (differs per UE), so it is owned by value here rather
  // than interned in a shared pool that would grow unbounded.
  std::optional<bwp_uplink_dedicated> ul_owned;
  if (ul_ded != nullptr) {
    ul_owned = *ul_ded;
  }

  return sched_bwp_config{
      bwp_id,
      sched_bwp_dl_config{bwp_dl_cmn,
                          dl_ptr.get(),
                          dl_ptr.has_value() and dl_ptr->pdcch_cfg.has_value() ? pdcch_pool.ded_cfgs()[0]
                                                                               : pdcch_pool.init_cfg(),
                          pdsch_td_mapper},
      sched_bwp_ul_config{bwp_ul_cmn, std::move(ul_owned), ue_bwp_cfg.ul, pusch_td_mapper, pucch_td_mapper}};
}

static std::vector<std::unique_ptr<bwp_config_pool>>
make_cell_bwp_pools(const sched_cell_configuration_request_message& req)
{
  std::vector<std::unique_ptr<bwp_config_pool>> bwps;
  // Note: Create a pool for a single BWP for now.
  bwps.push_back(std::make_unique<bwp_config_pool>(req.ran, to_bwp_id(0), make_cell_bwp_res_config(req.ran)));
  return bwps;
}

du_cell_config_pool::du_cell_config_pool(const scheduler_expert_config&                  sched_cfg_,
                                         const sched_cell_configuration_request_message& req) :
  cell_bwps(make_cell_bwp_pools(req)), cell_cfg_inst(sched_cfg_, req, cell_bwps[0]->cell_cfg())
{
}

ue_cell_res_config& du_cell_config_pool::acquire_ue_cell_cfg()
{
  // Reuse a slot whose references have all been dropped (ref_cnt == 0), else grow the pool. Both happen on the
  // (non-RT) config path, so the allocation on growth is fine; the pool plateaus at the peak concurrent UE count.
  for (ue_cell_res_config& pooled_cfg : ue_cell_cfg_pool) {
    if (pooled_cfg.is_unreferenced()) {
      pooled_cfg.clear();
      return pooled_cfg;
    }
  }
  return ue_cell_cfg_pool.emplace_back();
}

ue_cell_config_ptr du_cell_config_pool::update_ue(const ue_cell_config& ue_cell)
{
  ue_cell_res_config& ret = acquire_ue_cell_cfg();
  ret.cell_index          = ue_cell.serv_cell_cfg.cell_index;
  if (ue_cell.serv_cell_cfg.ul_config.has_value() and
      ue_cell.serv_cell_cfg.ul_config.value().pusch_serv_cell_cfg.has_value()) {
    ret.pusch_serv_cell_cfg.emplace(
        pusch_serv_cell_pool.create(ue_cell.serv_cell_cfg.ul_config.value().pusch_serv_cell_cfg.value()));
  }
  if (ue_cell.serv_cell_cfg.pdsch_serv_cell_cfg.has_value()) {
    ret.pdsch_serv_cell_cfg = pdsch_serv_cell_pool.create(ue_cell.serv_cell_cfg.pdsch_serv_cell_cfg.value());
  }
  if (ue_cell.serv_cell_cfg.csi_meas_cfg.has_value()) {
    ret.csi_meas_cfg = ue_cell.serv_cell_cfg.csi_meas_cfg.value();
  }
  ret.tag_id = ue_cell.serv_cell_cfg.tag_id;

  // TODO: Support more than one BWP.
  add_bwp(ret,
          ue_cell.serv_cell_cfg.init_dl_bwp,
          ue_cell.serv_cell_cfg.ul_config.has_value() ? &ue_cell.serv_cell_cfg.ul_config->init_ul_bwp : nullptr,
          ue_cell.bwps[0]);

  // Hand out an intrusive_ptr to the pool-owned slot. Dropping the last reference (possibly on the RT thread) only
  // decrements ref_cnt; the slot is reused on a later acquire, never freed off the config path.
  return ue_cell_config_ptr{&ret};
}

void du_cell_config_pool::add_bwp(ue_cell_res_config&           out,
                                  const bwp_downlink_dedicated& dl_bwp_ded,
                                  const bwp_uplink_dedicated*   ul_bwp_ded,
                                  const ue_bwp_config&          ue_bwp_cfg)
{
  sched_bwp_config bwp_cfg = cell_bwps[0]->add_ded_cfg(&dl_bwp_ded, ul_bwp_ded, ue_bwp_cfg);

  // Generate cell-wide lookups of CORESETs and SearchSpaces for this UE. These reference the (shared, pooled) PDCCH
  // config, so they stay valid after the BWP config is moved into the owned list below.
  for (const auto& cs : bwp_cfg.dl.pdcch().coresets()) {
    out.coresets.emplace(cs->id(), cs);
  }
  for (const auto& ss : bwp_cfg.dl.pdcch().search_spaces()) {
    out.search_spaces.emplace(ss->id(), &ss->cfg());
  }

  out.bwps.emplace(bwp_cfg.id, std::move(bwp_cfg));
}

// class du_cell_group_config_pool

cell_configuration& du_cell_group_config_pool::add_cell(const scheduler_expert_config&                  expert_cfg,
                                                        const sched_cell_configuration_request_message& cell_cfg_req)
{
  ocudu_assert(not cells.contains(cell_cfg_req.cell_index), "Cell already exists");
  return cells.emplace(cell_cfg_req.cell_index, std::make_unique<du_cell_config_pool>(expert_cfg, cell_cfg_req))
      ->cell_cfg();
}

void du_cell_group_config_pool::rem_cell(du_cell_index_t cell_index)
{
  // Note: This function assumes that all the UEs have been removed from this cell.
  cells.erase(cell_index);
}

ue_creation_params du_cell_group_config_pool::add_ue(const sched_ue_creation_request_message& ue_creation_req)
{
  // Create logical channel config.
  auto lc_ch_list =
      lc_ch_pool.create(ue_creation_req.cfg.lc_config_list.has_value() ? ue_creation_req.cfg.lc_config_list.value()
                                                                       : std::vector<logical_channel_config>{});

  // Create UE dedicated cell configs.
  slotted_id_vector<du_cell_index_t, ue_cell_config_ptr> cell_cfgs;
  if (ue_creation_req.cfg.cells.has_value()) {
    for (const auto& cell : ue_creation_req.cfg.cells.value()) {
      cell_cfgs.emplace(cell.serv_cell_cfg.cell_index, cells[cell.serv_cell_cfg.cell_index]->update_ue(cell));
    }
  }

  return ue_creation_params{ue_creation_req.cfg, lc_ch_list, cell_cfgs};
}

ue_reconfig_params du_cell_group_config_pool::reconf_ue(const sched_ue_reconfiguration_message& ue_reconf_req)
{
  std::optional<logical_channel_config_list_ptr> lc_ch_list;
  if (ue_reconf_req.cfg.lc_config_list.has_value()) {
    lc_ch_list = lc_ch_pool.create(ue_reconf_req.cfg.lc_config_list.value());
  }

  // Create UE dedicated cell configs.
  slotted_id_vector<du_cell_index_t, ue_cell_config_ptr> cell_cfgs;
  if (ue_reconf_req.cfg.cells.has_value()) {
    for (const auto& cell : ue_reconf_req.cfg.cells.value()) {
      cell_cfgs.emplace(cell.serv_cell_cfg.cell_index, cells[cell.serv_cell_cfg.cell_index]->update_ue(cell));
    }
  }

  return ue_reconfig_params{ue_reconf_req.cfg, lc_ch_list, cell_cfgs};
}
