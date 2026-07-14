// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_high_configuration.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/rlc/rlc_entity.h"
#include "ocudu/rlc/rlc_window_seg_pool_factory.h"
#include "ocudu/rlc/rlc_window_seg_pools.h"

namespace ocudu {

struct du_high_ue_simulator_config {
  rnti_t                            crnti;
  du_cell_index_t                   pcell_index;
  const odu::du_high_configuration& du_high_cfg;
};

class du_high_ue_simulator
{
public:
  explicit du_high_ue_simulator(const du_high_ue_simulator_config& cfg,
                                const odu::du_high_dependencies&   du_hi_deps,
                                task_executor&                     test_exec);
  ~du_high_ue_simulator();

  /// Called when a UE receives a DL PDU and processes it.
  void handle_dl_pdu(const mac_dl_data_result::dl_pdu& pdu);

  /// Retrieves RLC SDUs that are ready to be transmitted in the UL direction and generate the respective MAC PDU.
  std::optional<byte_buffer> build_next_ul_mac_pdu();

  void enqueue_ul_mac_sdu(lcid_t lcid, byte_buffer ul_mac_sdu);

  /// Get all received DL MAC SDUs.
  std::vector<std::pair<lcid_t, byte_buffer>> pop_pending_dl_mac_sdus();

private:
  class rlc_bearer_adapter;

  struct bearer_context {
    std::unique_ptr<rlc_bearer_adapter> adapter;
    std::unique_ptr<rlc_entity>         rlc;
  };

  du_high_ue_simulator_config cfg;
  task_executor&              test_exec;

  std::unique_ptr<rlc_srb_rx_window_seg_pool, rlc_pool_deleter> srb_rx_seg_pool =
      make_rlc_srb_rx_window_seg_pool(rlc_srb_rx_window_seg_pool_size);
  std::unique_ptr<rlc_srb_tx_window_seg_pool, rlc_pool_deleter> srb_tx_seg_pool =
      make_rlc_srb_tx_window_seg_pool(rlc_srb_tx_window_seg_pool_size);

  std::map<lcid_t, bearer_context> bearers;

  std::vector<byte_buffer> pdus_to_tx;
};

} // namespace ocudu
