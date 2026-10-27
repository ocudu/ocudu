// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/format.h"
#include "ocudu/scheduler/config/serving_cell_config_builder.h"

using namespace ocudu;

/// Build the default PUCCH-PowerControl configuration.
static pucch_power_control build_pucch_power_control()
{
  pucch_power_control pw_ctrl;
  pw_ctrl.delta_pucch_f0 = 0;
  pw_ctrl.delta_pucch_f1 = 0;
  pw_ctrl.delta_pucch_f2 = 0;
  pw_ctrl.delta_pucch_f3 = 0;
  pw_ctrl.delta_pucch_f4 = 0;

  auto& pucch_pw_set = pw_ctrl.p0_set.emplace_back();
  pucch_pw_set.id    = 0U;
  pucch_pw_set.value = 0;

  return pw_ctrl;
}

/// Appends a cell resource to the UE's PUCCH resource list and returns its UE resource ID, i.e. its list position.
static unsigned add_ue_res(static_vector<pucch_res_id_t, MAX_NOF_UE_PUCCH_RESOURCES>& res_list, pucch_res_id_t cell_res)
{
  res_list.push_back(cell_res);
  return res_list.size() - 1;
}

unsigned config_helpers::ue_csi_res_id(const pucch_resource_builder_params& res_params)
{
  // The UE resource list holds, in this order: the Resource Set ID 0 resources, the SR resource, the Resource Set ID 1
  // resources and the CSI resource. See \c build_pucch_config.
  const unsigned nof_set_0_res = res_params.res_set_size.value();
  const unsigned nof_sr_res    = 1U;
  const unsigned nof_set_1_res = res_params.res_set_size.value();
  return nof_set_0_res + nof_sr_res + nof_set_1_res;
}

config_helpers::ue_pucch_res_layout config_helpers::make_ue_pucch_res_layout(const cell_pucch_resources& cell_resources,
                                                                             const ue_rrm_config&        ue_rrm_cfg)
{
  const auto& ue_periodic_csi_report_cfg = ue_rrm_cfg.csi.periodic_report;
  const auto& harq_res                   = cell_resources.get_harq(ue_rrm_cfg.pucch.res_set_cfg_id);
  const auto& sr_res                     = cell_resources.get_sr(ue_rrm_cfg.pucch.sr_res_id);

  ue_pucch_res_layout layout;
  auto&               res_list  = layout.res_list;
  auto&               res_set_0 = layout.res_set_0;
  auto&               res_set_1 = layout.res_set_1;

  // Add Resource Set ID 0 resources for HARQ-ACK.
  for (pucch_res_id_t cell_res : harq_res.set_0) {
    // Add resource to both the PUCCH resource list and Resource Set ID 0.
    res_set_0.push_back(add_ue_res(res_list, cell_res));
  }

  // Add SR resource to the PUCCH resource list.
  layout.sr_res_id = add_ue_res(res_list, sr_res.res_id);

  // Add Resource Set ID 1 resources for HARQ-ACK.
  for (pucch_res_id_t cell_res : harq_res.set_1) {
    // Add resource to both the PUCCH resource list and Resource Set ID 1.
    res_set_1.push_back(add_ue_res(res_list, cell_res));
  }

  const cell_pucch_resources::res_with_alt* csi_res = nullptr;
  if (ue_periodic_csi_report_cfg.has_value()) {
    csi_res = &cell_resources.get_csi(ue_periodic_csi_report_cfg->res_id);
    // Add CSI resource to the PUCCH resource list.
    layout.csi_res_id = add_ue_res(res_list, csi_res->res_id);
    ocudu_assert(*layout.csi_res_id == 2U * harq_res.set_0.size() + 1U,
                 "CSI resource at UE resource ID {} differs from the ID used by the CSI report",
                 *layout.csi_res_id);
  }

  if (sr_res.alt_res_id.has_value()) {
    // [Implementation-defined] We build the PUCCH-Config of all UEs in a way that will prevent the scheduling of
    // multiple PUCCH transmissions per UE per slot, since not all UEs are capable of that. To achieve this, we make
    // sure that there is always an option to schedule different UCI types on overlapping symbols, so that the UE will
    // multiplex the different UCI types on a single PUCCH.
    //
    // This is done differently depending on the configured PUCCH formats:
    // - F1 and F2/F3/F4: since we configure F1 resources to always take all available symbols in the slot, all F2/F3/F4
    //   resources will overlap in symbols with F1 resources.
    // - F0 and F3/F4: for now we do not allow this combination. TODO: figure out this case.
    // - F0 and F2: we reserve two resources in each Resource Set to be used for HARQ-ACK when CSI or SR is scheduled.
    //
    // The diagram below illustrates the configuration of the Resource Sets for this last case:
    //
    //  +--------+--------+--------+--------+--------+    +--------+--------+--------+--------+--------+
    //  | R_0    | ...    | R_N-1  | SR_F0  | CSI_F0 |    | R_0    | ...    | R_N-1  | SR_F2  | CSI_F2 |
    //  +--------+--------+--------+--------+--------+    +--------+--------+--------+--------+--------+
    //             PUCCH Resource Set ID 0                           PUCCH Resource Set ID 1
    //
    // Where:
    // - R_i are regular PUCCH resources for HARQ-ACK.
    // - SR_F0 is the same F0 resource in the PUCCH resource list as the SR resource.
    // - CSI_F0 is a F0 resource in Resource Set ID 0 that overlaps in symbols with the CSI (F2) resource.
    // - SR_F2 is a F2 resource in Resource Set ID 1 that overlaps in symbols with the SR (F0) resource.
    // - CSI_F2 is the same F2 resource in the PUCCH resource list as the CSI resource.
    res_set_0.push_back(layout.sr_res_id);
    res_set_1.push_back(add_ue_res(res_list, *sr_res.alt_res_id));

    if (csi_res != nullptr) {
      res_set_0.push_back(add_ue_res(res_list, *csi_res->alt_res_id));
      res_set_1.push_back(*layout.csi_res_id);
    }
  }

  return layout;
}

pucch_config config_helpers::build_pucch_config(const cell_pucch_rrm_config& cell_resources,
                                                const ue_rrm_config&         ue_rrm_cfg)
{
  const ue_pucch_res_layout layout = make_ue_pucch_res_layout(cell_resources.res, ue_rrm_cfg);

  const auto   max_code_rate = cell_resources.max_code_rate_234;
  pucch_config cfg{
      .format_1_common_param = pucch_common_all_formats{},
      .format_2_common_param = pucch_common_all_formats{.max_c_rate = max_code_rate, .simultaneous_harq_ack_csi = true},
      .format_3_common_param = pucch_common_all_formats{.max_c_rate = max_code_rate, .simultaneous_harq_ack_csi = true},
      .format_4_common_param = pucch_common_all_formats{.max_c_rate = max_code_rate, .simultaneous_harq_ack_csi = true},
      .dl_data_to_ul_ack     = cell_resources.dl_data_to_ul_ack,
      .pucch_pw_control      = build_pucch_power_control(),
  };

  for (pucch_res_id_t cell_res_id : layout.res_list) {
    cfg.pucch_res_list.push_back(cell_resources.res.get(cell_res_id));
  }
  auto& res_set_0 = cfg.pucch_res_set.emplace_back();
  res_set_0.id    = pucch_res_set_id::set_0;
  res_set_0.resources.assign(layout.res_set_0.begin(), layout.res_set_0.end());
  auto& res_set_1 = cfg.pucch_res_set.emplace_back();
  res_set_1.id    = pucch_res_set_id::set_1;
  res_set_1.resources.assign(layout.res_set_1.begin(), layout.res_set_1.end());
  cfg.sr_res_list.emplace_back(scheduling_request_resource_config{.sr_res_id    = 1,
                                                                  .sr_id        = uint_to_sched_req_id(0),
                                                                  .period       = cell_resources.sr_period,
                                                                  .offset       = ue_rrm_cfg.pucch.sr_offset,
                                                                  .pucch_res_id = layout.sr_res_id});

  return cfg;
}
