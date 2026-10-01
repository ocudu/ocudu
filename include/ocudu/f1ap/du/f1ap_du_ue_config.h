// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/du/f1c_bearer.h"
#include "ocudu/f1ap/du/f1c_rx_sdu_notifier.h"
#include "ocudu/f1ap/f1ap_ue_id_types.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/ran/rnti.h"
#include <vector>

namespace ocudu {
namespace odu {

/// \brief F1-C bearer to Add or Modify in UE F1 context.
struct f1c_bearer_to_addmod {
  srb_id_t             srb_id;
  f1c_rx_sdu_notifier* rx_sdu_notifier;
};

/// \brief F1c bearer Added or Modified in UE F1 context.
struct f1c_bearer_addmodded {
  srb_id_t    srb_id;
  f1c_bearer* bearer;
};

/// \brief Request sent to the DU F1AP to create a new UE F1AP context.
struct f1ap_ue_creation_request {
  du_ue_index_t                     ue_index;
  du_cell_index_t                   pcell_index;
  rnti_t                            c_rnti;
  byte_buffer                       du_cu_rrc_container;
  std::vector<f1c_bearer_to_addmod> f1c_bearers_to_add;
};

/// \brief Response from the DU F1AP to the request to create a new UE.
struct f1ap_ue_creation_response {
  bool                     result     = false;
  gnb_du_ue_f1ap_id_t      f1ap_ue_id = gnb_du_ue_f1ap_id_t::invalid;
  std::vector<f1c_bearer*> f1c_bearers_added;
};

/// \brief Request sent to the DU F1AP to update the configuration of an existing UE F1AP context.
struct f1ap_ue_configuration_request {
  du_ue_index_t                     ue_index;
  std::vector<f1c_bearer_to_addmod> f1c_bearers_to_add;
};

/// \brief Response from the DU F1AP to the request to update the configuration of an existing UE.
struct f1ap_ue_configuration_response {
  std::vector<f1c_bearer_addmodded> f1c_bearers_added;
};

} // namespace odu
} // namespace ocudu
