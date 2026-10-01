// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/rlc/rlc_bearer_logger.h"
#include "rlc_stress_test_args.h"
#include "ocudu/pdcp/pdcp_rx.h"
#include "ocudu/ran/gnb_du_id.h"
#include <random>

namespace ocudu {
class pdcp_tx_upper_data_interface;

class stress_traffic_sink : public pdcp_rx_upper_data_notifier
{
  rlc_bearer_logger logger;

public:
  stress_traffic_sink(uint32_t ue_id, rb_id_t rb_id) : logger("TRAFF", {(gnb_du_id_t)0, ue_id, rb_id, "UL"}) {}

  // pdcp_rx_upper_data_notifier interface
  void on_new_sdu(byte_buffer pdu, bool integrity_verified) final;
};

class stress_traffic_source
{
  const stress_test_args& args;
  uint32_t                sdu_size = 0;
  uint8_t                 payload  = 0x0; // increment for each SDU byte

  std::mt19937                    rgen;
  std::uniform_int_distribution<> int_dist;

  pdcp_tx_upper_data_interface* pdcp_tx_upper = nullptr;

  rlc_bearer_logger logger;

public:
  explicit stress_traffic_source(const stress_test_args& args_, uint32_t ue_id, rb_id_t rb_id) :
    args(args_),
    rgen(args_.seed),
    int_dist(args_.min_sdu_size, args_.max_sdu_size),
    logger("TRAFF", {(gnb_du_id_t)0, ue_id, rb_id, "DL"})
  {
  }

  void set_pdcp_tx_upper(pdcp_tx_upper_data_interface* pdcp_tx_upper_) { this->pdcp_tx_upper = pdcp_tx_upper_; }

  void send_pdu();
};
} // namespace ocudu
