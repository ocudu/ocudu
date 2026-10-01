// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_test_mode_config.h"
#include "ocudu/mac/mac_cell_control_information_handler.h"
#include "ocudu/mac/mac_pdu_handler.h"
#include "ocudu/ran/harq_id.h"
#include "ocudu/scheduler/scheduler_configurator.h"

namespace ocudu {

struct pucch_info;
struct ul_sched_info;

namespace odu {

/// Create dummy PDU with BSR.
expected<mac_rx_data_indication>
create_test_pdu_with_bsr(du_cell_index_t cell_index, slot_point sl_rx, rnti_t test_rnti, harq_id_t harq_id);

/// Create dummy PDU with rrcSetupComplete.
expected<mac_rx_data_indication> create_test_pdu_with_rrc_setup_complete(du_cell_index_t cell_index,
                                                                         slot_point      sl_rx,
                                                                         rnti_t          test_rnti,
                                                                         harq_id_t       harq_id);

/// Setters for UCI PDUs based on config values.
mac_uci_pdu create_uci_pdu(const pucch_info& pucch, const du_test_mode_config::test_mode_ue_config& test_ue_cfg);
mac_uci_pdu create_uci_pdu(const ul_sched_info& pusch, const du_test_mode_config::test_mode_ue_config& test_ue_cfg);

/// Check whether a PUCCH grant and MAC UCI PDU match in C-RNTI, format and expected info.
bool pucch_info_and_uci_ind_match(const pucch_info& pucch, const mac_uci_pdu& uci_ind);

} // namespace odu
} // namespace ocudu
