// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_ue_resource_config.h"
#include "ocudu/du/du_high/du_qos_config.h"
#include "ocudu/du/du_high/du_srb_config.h"
#include "ocudu/f1ap/ue_context_management_configs.h"
#include <map>

namespace ocudu {
namespace odu {

struct du_ue_bearer_resource_update_request {
  span<const srb_id_t>           srbs_to_setup;
  span<const f1ap_drb_to_setup>  drbs_to_setup;
  span<const f1ap_drb_to_modify> drbs_to_mod;
  span<const drb_id_t>           drbs_to_rem;
};

struct du_ue_bearer_resource_update_response {
  std::vector<drb_id_t> drbs_failed_to_setup;
  std::vector<drb_id_t> drbs_failed_to_mod;
};

class du_bearer_resource_manager
{
public:
  du_bearer_resource_manager(const std::map<srb_id_t, du_srb_config>&  srbs,
                             const std::map<five_qi_t, du_qos_config>& qos,
                             ocudulog::basic_logger&                   logger);

  /// \brief Allocate bearer resources for a given UE. The resources are stored in the UE's DU UE resource config.
  /// \return true if allocation was successful.
  du_ue_bearer_resource_update_response update(du_ue_resource_config&                      ue_cfg,
                                               const du_ue_bearer_resource_update_request& request,
                                               const du_ue_resource_config*                reestablished_context);

private:
  /// \brief Returns the allowed UL HARQ mode a logical channel of this UE can keep, if any.
  ///
  /// The UL HARQ processes of a UE that does not support mode B all operate in mode A, whichever mode the cell
  /// configures. A restriction no process of this UE can meet is dropped, since a logical channel carrying it would
  /// be left out of every grant, as per \c allowedHARQ-mode, TS 38.331.
  std::optional<ul_harq_mode> supported_allowed_harq_mode(const du_ue_resource_config& ue_cfg,
                                                          std::optional<ul_harq_mode>  allowed_harq_mode,
                                                          lcid_t                       lcid) const;

  /// \brief Restricts the SRBs of the UE to UL HARQ mode A, when that restriction selects anything.
  ///
  /// An unrestricted SRB rides any grant, as per TS 38.321, Section 5.4.3.1.2, including the mode B grants of a data
  /// slice, where no HARQ retransmission recovers a corrupted PDU. Resolved on every update, as the UE only takes the
  /// mode B processes once it has reported the capability.
  void restrict_srbs_to_mode_a(du_ue_resource_config& ue_cfg) const;

  void                  setup_srbs(du_ue_resource_config& ue_cfg, const du_ue_bearer_resource_update_request& request);
  std::vector<drb_id_t> setup_drbs(du_ue_resource_config& ue_cfg, const du_ue_bearer_resource_update_request& request);
  std::vector<drb_id_t> modify_drbs(du_ue_resource_config& ue_cfg, const du_ue_bearer_resource_update_request& request);
  void                  rem_drbs(du_ue_resource_config& ue_cfg, const du_ue_bearer_resource_update_request& request);

  const std::map<srb_id_t, du_srb_config>&  srb_config;
  const std::map<five_qi_t, du_qos_config>& qos_config;
  ocudulog::basic_logger&                   logger;
};

} // namespace odu
} // namespace ocudu
