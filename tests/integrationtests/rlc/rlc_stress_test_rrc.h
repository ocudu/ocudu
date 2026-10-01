// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/rlc/rlc_bearer_logger.h"
#include "ocudu/pdcp/pdcp_rx.h"
#include "ocudu/pdcp/pdcp_tx.h"

namespace ocudu {
class rrc_dummy final : public pdcp_rx_upper_control_notifier, public pdcp_tx_upper_control_notifier
{
  rlc_bearer_logger logger;

public:
  explicit rrc_dummy(uint32_t id) : logger("RRC", {gnb_du_id_t::min, id, drb_id_t::drb1, "DL/UL"}) {}

  // PDCP -> RRC
  void on_integrity_failure() override {}
  void on_protocol_failure() override {}
  void on_max_count_reached() override {}
  void on_resume_required() override {}
};

} // namespace ocudu
