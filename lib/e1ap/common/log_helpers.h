// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "e1ap_logger.h"
#include "ocudu/e1ap/common/e1ap_message.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/cu_up_types.h"
#include "ocudu/support/format/prefixed_logger.h"

namespace ocudu {

/// \brief Helper for logging Rx/Tx E1AP PDUs for the CU-CP and CU-UP.
template <typename UeIndex>
void log_e1ap_pdu(ocudulog::basic_logger&       logger,
                  bool                          is_rx,
                  const std::optional<UeIndex>& ue_id,
                  const e1ap_message&           e1ap_msg,
                  bool                          json_enabled);

/// \brief Helper for logging Rx/Tx E1AP PDUs for the CU-CP and CU-UP with prefixed logger.
template <typename UeIndex>
void log_e1ap_pdu(e1ap_logger&                  logger,
                  bool                          is_rx,
                  const std::optional<UeIndex>& ue_id,
                  const e1ap_message&           e1ap_msg,
                  bool                          json_enabled);

extern template void log_e1ap_pdu<cu_cp_ue_index_t>(ocudulog::basic_logger&                logger,
                                                    bool                                   is_rx,
                                                    const std::optional<cu_cp_ue_index_t>& ue_id,
                                                    const e1ap_message&                    e1ap_msg,
                                                    bool                                   json_enabled);
extern template void log_e1ap_pdu<cu_up_ue_index_t>(ocudulog::basic_logger&                logger,
                                                    bool                                   is_rx,
                                                    const std::optional<cu_up_ue_index_t>& ue_id,
                                                    const e1ap_message&                    e1ap_msg,
                                                    bool                                   json_enabled);

extern template void log_e1ap_pdu<cu_up_ue_index_t>(e1ap_logger&                           logger,
                                                    bool                                   is_rx,
                                                    const std::optional<cu_up_ue_index_t>& ue_id,
                                                    const e1ap_message&                    e1ap_msg,
                                                    bool                                   json_enabled);

} // namespace ocudu
