// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/rlc/rlc_bearer_logger.h"
#include "rlc_stress_test_args.h"
#include "ocudu/adt/byte_buffer_chain.h"
#include "ocudu/ran/gnb_du_id.h"
#include "ocudu/rlc/rlc_tx.h"
#include <random>

namespace ocudu {
class rlc_rx_lower_layer_interface;

class mac_dummy : public rlc_tx_lower_layer_notifier
{
  const stress_test_args& args;
  rlc_bearer_logger       logger;

  std::mt19937                          rgen;
  std::uniform_real_distribution<float> real_dist;

  std::atomic<unsigned> bsr;

  rlc_tx_lower_layer_interface* rlc_tx_lower = nullptr;
  rlc_rx_lower_layer_interface* rlc_rx_lower = nullptr;

public:
  mac_dummy(const stress_test_args& args_, uint32_t ue_id, rb_id_t rb_id) :
    args(args_), logger("MAC", {(gnb_du_id_t)0, ue_id, rb_id, "DL"}), rgen(args_.seed), bsr(0)
  {
  }

  std::vector<byte_buffer_chain> pdu_rx_list;
  std::vector<byte_buffer_chain> run_tx_tti(uint32_t tti);
  void                           run_rx_tti();
  void                           push_rx_pdus(std::vector<byte_buffer_chain> list_pdus);

  // rlc_tx_lower_layer_notifier interface
  void on_buffer_state_update(const rlc_buffer_state& bs) final
  {
    this->bsr.store(bs.pending_bytes, std::memory_order_relaxed);
  }

  void set_rlc_tx_lower(rlc_tx_lower_layer_interface* rlc_tx_lower_) { this->rlc_tx_lower = rlc_tx_lower_; }
  void set_rlc_rx_lower(rlc_rx_lower_layer_interface* rlc_rx_lower_) { this->rlc_rx_lower = rlc_rx_lower_; }
};

} // namespace ocudu
