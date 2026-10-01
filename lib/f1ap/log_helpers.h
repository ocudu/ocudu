// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/gnb_du_id.h"

namespace ocudu {

/// \brief Helper for logging Rx/Tx F1AP PDUs for the CU-CP and DU.
template <typename UeIndex>
void log_f1ap_pdu(ocudulog::basic_logger&       logger,
                  bool                          is_rx,
                  gnb_du_id_t                   du_id,
                  const std::optional<UeIndex>& ue_id,
                  const f1ap_message&           msg,
                  bool                          json_enabled);

extern template void log_f1ap_pdu<cu_cp_ue_index_t>(ocudulog::basic_logger&                logger,
                                                    bool                                   is_rx,
                                                    gnb_du_id_t                            du_id,
                                                    const std::optional<cu_cp_ue_index_t>& ue_id,
                                                    const f1ap_message&                    msg,
                                                    bool                                   json_enabled);
extern template void log_f1ap_pdu<du_ue_index_t>(ocudulog::basic_logger&             logger,
                                                 bool                                is_rx,
                                                 gnb_du_id_t                         du_id,
                                                 const std::optional<du_ue_index_t>& ue_id,
                                                 const f1ap_message&                 msg,
                                                 bool                                json_enabled);

} // namespace ocudu
