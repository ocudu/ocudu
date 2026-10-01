// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/pdcp/pdcp_factory.h"
#include "pdcp_entity_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace ocudu;

std::unique_ptr<pdcp_entity> ocudu::create_pdcp_entity(pdcp_entity_creation_message& msg)
{
  return std::make_unique<pdcp_entity_impl>(msg.ue_index,
                                            msg.rb_id,
                                            msg.config,
                                            *msg.tx_lower,
                                            *msg.tx_upper_cn,
                                            *msg.rx_upper_dn,
                                            *msg.rx_upper_cn,
                                            msg.ue_dl_timer_factory,
                                            msg.ue_ul_timer_factory,
                                            msg.ue_ctrl_timer_factory,
                                            *msg.ue_dl_executor,
                                            *msg.ue_ul_executor,
                                            *msg.ue_ctrl_executor,
                                            *msg.crypto_executor,
                                            msg.max_nof_crypto_workers);
}
