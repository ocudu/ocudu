// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_ue/du_ue.h"
#include "../du_ue/du_ue_manager_repository.h"
#include "procedure_logger.h"
#include "ocudu/du/du_high/du_manager/du_manager_mem_resources.h"
#include "ocudu/f1ap/du/f1ap_du_ue_config.h"
#include "ocudu/mac/mac_ue_configurator.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu {
namespace odu {

class du_procedure_metrics_collector;

struct du_ue_creation_request {
  du_ue_index_t   ue_index;
  du_cell_index_t pcell_index;
  /// \brief Currently allocated TC-RNTI for the UE in the MAC. In case the UE does not have a TC-RNTI (e.g. UE created
  /// by upper layers during Handover), the value should be INVALID_RNTI.
  rnti_t tc_rnti;
  /// \brief UL-CCCH message received from the UE in Msg3. Empty if the UE is created by upper layers.
  byte_buffer ul_ccch_msg;
  /// \brief If present, it represents the slot at which the UL-CCCH message was received in the PUSCH. Absent, when
  /// the UE is created by command from upper layers.
  std::optional<slot_point> slot_rx;
};

/// \brief Handles the creation of a UE and respective bearers in the DU UE manager, MAC, F1.
///  \startuml
///    participant DM
///    participant MAC
///    participant F1
///    Note over MAC: UL CCCH subPDU decoded
///    MAC-->>DM: UL CCCH PDU Indication
///    Note over DM: Allocate UE index
///    Note over DM: Buffer UL CCCH Indication
///    DM->>F1: UE create
///    F1-->>DM: UE create completed
///    Note over DM: Create RLC SRB0 (Factory)
///    DM->>MAC: UE create
///    MAC-->>DM: UE create completed
///    Note over DM: Create UE in UE manager
///    DM->>MAC: Notify UL CCCH PDU
///    MAC->>F1: SRB0 - Notify UL CCCH PDU
///  \enduml
class ue_creation_procedure
{
public:
  ue_creation_procedure(const du_ue_creation_request&   req_,
                        du_ue_manager_repository&       ue_mng_,
                        const du_manager_params&        du_params_,
                        const du_manager_mem_resources& du_mem_resources_,
                        du_ran_resource_manager&        cell_res_alloc_,
                        du_procedure_metrics_collector& metrics_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "UE Create"; }

private:
  /// Creates a UE object in the DU UE manager.
  expected<du_ue*, std::string> create_du_ue_context() const;

  /// Remove UE from DU Manager UE repository.
  async_task<void> clear_ue();

  /// Setups DU manager resources used by DU UE being created.
  bool setup_du_ue_resources();

  /// Creates UE object in F1.
  f1ap_ue_creation_response create_f1ap_ue();

  /// Creates SRB0 and SRB1 in RLC.
  void create_rlc_srbs();

  async_task<mac_ue_create_response> create_mac_ue();

  void connect_layer_bearers();

  const du_ue_creation_request    req;
  du_ue_manager_repository&       ue_mng;
  const du_manager_params&        du_params;
  const du_manager_mem_resources& du_mem_resources;
  du_ran_resource_manager&        du_res_alloc;
  du_procedure_metrics_collector& metrics;
  ue_procedure_logger             proc_logger;

  du_ue*                        ue_ctx                  = nullptr;
  expected<du_ue*, std::string> ue_ctx_creation_outcome = nullptr;
  mac_ue_create_response        mac_resp{};
  f1ap_ue_creation_response     f1ap_resp{};

  std::chrono::steady_clock::time_point start_tp;
};

} // namespace odu
} // namespace ocudu
