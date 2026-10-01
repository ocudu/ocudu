// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../config/ue_configuration.h"
#include "ocudu/ran/pdcch/search_space.h"
#include "ocudu/ran/resource_allocation/resource_allocation_frequency.h"
#include "ocudu/scheduler/config/bwp_configuration.h"
#include "ocudu/scheduler/result/dci_info.h"

namespace ocudu {

class dl_harq_process_handle;
class ul_harq_process_handle;

/// Builds DCI f1_0 for SI-RNTI used in SIBs.
void build_dci_f1_0_si_rnti(dci_dl_info&               dci,
                            const bwp_downlink_common& init_dl_bwp,
                            crb_interval               crbs,
                            unsigned                   time_resource,
                            sch_mcs_index              mcs_index,
                            uint8_t                    si_indicator);

/// Builds DCI f1_0 for P-RNTI used in PCCHs.
void build_dci_f1_0_p_rnti(dci_dl_info&               dci,
                           const bwp_downlink_common& init_dl_bwp,
                           crb_interval               crbs,
                           unsigned                   time_resource,
                           sch_mcs_index              mcs_index);

/// Builds DCI f1_0 with only short messages for P-RNTI used in PCCHs.
void build_dci_f1_0_p_rnti(dci_dl_info& dci, const bwp_downlink_common& init_dl_bwp, unsigned short_messages);

/// Builds DCI f1_0 for RA-RNTI used in RAR.
void build_dci_f1_0_ra_rnti(dci_dl_info&               dci,
                            const bwp_downlink_common& init_dl_bwp,
                            crb_interval               crbs,
                            unsigned                   time_resource,
                            sch_mcs_index              mcs_index);

/// \brief Builds DCI f1_0 for MsgB-RNTI used in successRAR/fallbackRAR.
/// \param msgb_resp_window_ms Configured msgB-ResponseWindow duration, in msec.
/// \param prach_sfn SFN where the UE transmitted PRACH.
///
/// As per TS38.213, Section 8.2A, the DCI carries the 2 LSBs of \c prach_sfn if \c msgb_resp_window_ms is larger
/// than 10 msec; these bits are left at \c 0 otherwise.
void build_dci_f1_0_msgb_rnti(dci_dl_info&               dci,
                              const bwp_downlink_common& init_dl_bwp,
                              crb_interval               crbs,
                              unsigned                   time_resource,
                              sch_mcs_index              mcs_index,
                              unsigned                   msgb_resp_window_ms = 0,
                              unsigned                   prach_sfn           = 0);

/// Builds DCI f1_0 for TC-RNTI, used for instance in Msg4.
void build_dci_f1_0_tc_rnti(dci_dl_info&                  dci,
                            const bwp_downlink_common&    init_dl_bwp,
                            vrb_interval                  vrbs,
                            unsigned                      time_resource,
                            unsigned                      k1,
                            unsigned                      pucch_res_indicator,
                            sch_mcs_index                 mcs_index,
                            uint8_t                       rv,
                            const dl_harq_process_handle& h_dl);

/// Builds DCI f1_0 for C-RNTI.
void build_dci_f1_0_c_rnti(dci_dl_info&                  dci,
                           const search_space_info&      ss_info,
                           const bwp_downlink_common&    init_dl_bwp,
                           vrb_interval                  vrbs,
                           unsigned                      time_resource,
                           unsigned                      k1,
                           unsigned                      pucch_res_indicator,
                           unsigned                      dai,
                           sch_mcs_index                 mcs_index,
                           uint8_t                       rv,
                           const dl_harq_process_handle& h_dl);

/// Builds DCI f1_1 for C-RNTI.
void build_dci_f1_1_c_rnti(dci_dl_info&                  dci,
                           const ue_cell_configuration&  ue_cell_cfg,
                           search_space_id               ss_id,
                           vrb_interval                  vrbs,
                           unsigned                      time_resource,
                           unsigned                      k1,
                           unsigned                      pucch_res_indicator,
                           unsigned                      dai,
                           sch_mcs_index                 tb1_mcs_index,
                           uint8_t                       rv,
                           const dl_harq_process_handle& h_dl,
                           unsigned                      nof_layers,
                           uint8_t                       tpc,
                           bool                          enable_interleaving,
                           unsigned                      srs_request);

/// Builds DCI f0_0 for TC-RNTI, used in Msg3 retxs.
void build_dci_f0_0_tc_rnti(dci_ul_info&               dci,
                            const bwp_downlink_common& init_dl_bwp,
                            const bwp_configuration&   ul_bwp,
                            const crb_interval&        crbs,
                            unsigned                   time_resource,
                            sch_mcs_index              mcs_index,
                            uint8_t                    rv);

/// Builds DCI f0_0 for C-RNTI.
void build_dci_f0_0_c_rnti(dci_ul_info&                  dci,
                           const search_space_info&      ss_info,
                           const bwp_uplink_common&      init_ul_bwp,
                           const vrb_interval&           vrbs,
                           unsigned                      time_resource,
                           sch_mcs_index                 mcs_index,
                           uint8_t                       rv,
                           const ul_harq_process_handle& h_ul,
                           uint8_t                       tpc_command);

/// Builds DCI f0_1 for C-RNTI.
void build_dci_f0_1_c_rnti(dci_ul_info&                  dci,
                           const ue_cell_configuration&  ue_cell_cfg,
                           search_space_id               ss_id,
                           const vrb_interval&           vrbs,
                           unsigned                      time_resource,
                           sch_mcs_index                 mcs_index,
                           uint8_t                       rv,
                           const ul_harq_process_handle& h_ul,
                           unsigned                      total_dai,
                           unsigned                      nof_layers,
                           unsigned                      tpmi,
                           uint8_t                       tpc_command,
                           std::optional<bool>           csi_request,
                           unsigned                      srs_request);

} // namespace ocudu
