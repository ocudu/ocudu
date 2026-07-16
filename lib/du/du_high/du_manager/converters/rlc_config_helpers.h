// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_ue/du_bearer.h"
#include "ocudu/du/du_high/du_manager/du_manager_mem_resources.h"
#include "ocudu/du/du_high/du_manager/du_manager_params.h"
#include "ocudu/f1ap/du/f1ap_du_ue_config.h"
#include "ocudu/mac/mac_lc_config.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/rlc/rlc_config.h"
#include "ocudu/rlc/rlc_factory.h"

namespace ocudu {
namespace odu {

struct rlc_window_segment_pools {
  rlc_drb_am_rx_window_seg_pool* drb_am_rx_pool = nullptr;
  rlc_drb_am_tx_window_seg_pool* drb_am_tx_pool = nullptr;
  rlc_drb_um_rx_window_seg_pool* drb_um_rx_pool = nullptr;
  rlc_srb_am_rx_window_seg_pool* srb_am_rx_pool = nullptr;
  rlc_srb_am_tx_window_seg_pool* srb_am_tx_pool = nullptr;
};

/// \brief Create configuration for RLC SRB entity.
rlc_entity_creation_message
make_rlc_entity_creation_message(gnb_du_id_t                                    du_id,
                                 du_ue_index_t                                  ue_index,
                                 du_cell_index_t                                pcell_index,
                                 du_ue_srb&                                     bearer,
                                 const rlc_config&                              rlc_cfg,
                                 const du_manager_params::service_params&       du_services,
                                 rlc_tx_upper_layer_control_notifier&           rlc_rlf_notifier,
                                 const du_manager_params::rlc_config_params&    rlc_params,
                                 const du_manager_mem_resources::rlc_resources& rlc_resources);

/// \brief Create configuration for RLC DRB entity.
rlc_entity_creation_message
make_rlc_entity_creation_message(gnb_du_id_t                                    du_id,
                                 du_ue_index_t                                  ue_index,
                                 du_cell_index_t                                pcell_index,
                                 du_ue_drb&                                     bearer,
                                 const rlc_config&                              rlc_cfg,
                                 const du_manager_params::service_params&       du_services,
                                 rlc_tx_upper_layer_control_notifier&           rlc_rlf_notifier,
                                 const du_manager_params::rlc_config_params&    rlc_params,
                                 const du_manager_mem_resources::rlc_resources& rlc_resources);

} // namespace odu
} // namespace ocudu
