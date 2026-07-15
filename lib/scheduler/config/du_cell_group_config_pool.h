// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cell_configuration.h"
#include "logical_channel_config_pool.h"
#include "pdcch_config_pool.h"
#include "sched_config_params.h"
#include "time_domain_mapper.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/scheduler/config/bwp_configuration.h"
#include "ocudu/scheduler/config/serving_cell_config.h"
#include "ocudu/scheduler/config/ue_bwp_config.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include <deque>
#include <memory>

namespace ocudu {

struct sched_cell_configuration_request_message;
struct sched_ue_creation_request_message;
struct sched_ue_reconfiguration_message;
struct serving_cell_config;

/// Pool of resources associated with a BWP config.
class bwp_config_pool
{
public:
  bwp_config_pool(const ran_cell_config& cell_ran_cfg, bwp_id_t bwpid, const cell_bwp_res_config& bwp_ded_res);
  bwp_config_pool(const bwp_config_pool&)            = delete;
  bwp_config_pool& operator=(const bwp_config_pool&) = delete;

  const bwp_downlink_common& dl_common() const { return bwp_dl_cmn; }

  // Builds the UE-dedicated BWP config.
  sched_bwp_config add_ded_cfg(const bwp_downlink_dedicated* dl_ded,
                               const bwp_uplink_dedicated*   ul_ded,
                               const ue_bwp_config&          ue_bwp_cfg);

  const sched_bwp_config& cell_cfg() const { return common_bwp_cfg; }

private:
  /// \brief Retrieve the DL TD resource mapper to use for a UE with the given dedicated DL BWP config.
  const dl_time_domain_mapper& get_dl_td_mapper(const bwp_downlink_dedicated* dl_ded);

  const bwp_id_t                             bwp_id;
  const bwp_downlink_common                  bwp_dl_cmn;
  const bwp_uplink_common                    bwp_ul_cmn;
  dl_time_domain_builder_params              dl_td_params;
  dl_time_domain_mapper                      dl_td_mapper;
  ul_time_domain_mapper                      ul_td_mapper;
  pdcch_config_pool                          pdcch_pool;
  sched_bwp_config                           common_bwp_cfg;
  config_object_pool<bwp_downlink_dedicated> dl_ded_config_pool;
  config_object_pool<dl_time_domain_mapper>  dl_ded_td_mapper_pool;
};

class du_cell_config_pool
{
public:
  du_cell_config_pool(const scheduler_expert_config& sched_cfg_, const sched_cell_configuration_request_message& req);
  du_cell_config_pool(const du_cell_config_pool&)            = delete;
  du_cell_config_pool& operator=(const du_cell_config_pool&) = delete;

  const cell_configuration& cell_cfg() const { return cell_cfg_inst; }
  cell_configuration&       cell_cfg() { return cell_cfg_inst; }

  // Builds a reference-counted UE-dedicated cell config.
  ue_cell_config_ptr update_ue(const ue_cell_config& ue_cell);

private:
  void add_bwp(ue_cell_res_config&           out,
               const bwp_downlink_dedicated& dl_bwp_ded,
               const bwp_uplink_dedicated*   ul_bwp_ded,
               const ue_bwp_config&          ue_bwp_cfg);

  /// \brief Returns a reusable ue_cell_res_config slot from the pool, growing the pool if none is free.
  /// The returned slot has its payload cleared, ready to be populated.
  /// \note Called only from the (non-RT) config path.
  ue_cell_res_config& acquire_ue_cell_cfg();

  /// Pool of per-UE dedicated cell configs. Slots are reused once their reference count drops to zero; a reference
  /// being dropped on the RT scheduler thread only decrements the counter (never frees), so the pool only ever grows
  /// (to the peak number of concurrent UEs) and frees at cell teardown. A deque keeps element addresses stable for
  /// the intrusive_ptr handles.
  std::deque<ue_cell_res_config> ue_cell_cfg_pool;

  /// BWPs managed in this cell.
  std::vector<std::unique_ptr<bwp_config_pool>> cell_bwps;

  /// Cell common configuration.
  cell_configuration cell_cfg_inst;

  // Pools of UE-dedicated configurations.
  config_object_pool<pdsch_serving_cell_config> pdsch_serv_cell_pool;
  config_object_pool<pusch_serving_cell_config> pusch_serv_cell_pool;
};

/// Class responsible for managing the configurations of all the entities (cells, UEs, slices) in a cell group.
class du_cell_group_config_pool
{
public:
  /// Creates handles to the resources associated with common cell configuration.
  cell_configuration& add_cell(const scheduler_expert_config&                  expert_cfg,
                               const sched_cell_configuration_request_message& cell_cfg_req);

  /// Remove cell and respective resources.
  void rem_cell(du_cell_index_t cell_index);

  /// Creates handles to the resources associated with a specific UE during its creation.
  ue_creation_params add_ue(const sched_ue_creation_request_message& ue_creation_req);

  /// Creates handles to the resources associated with a specific UE during its reconfiguration.
  ue_reconfig_params reconf_ue(const sched_ue_reconfiguration_message& ue_reconf_req);

private:
  logical_channel_config_pool lc_ch_pool;

  slotted_id_vector<du_cell_index_t, std::unique_ptr<du_cell_config_pool>> cells;
};

} // namespace ocudu
