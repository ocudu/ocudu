// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "rlc_stress_test_args.h"
#include "rlc_stress_test_f1.h"
#include "rlc_stress_test_mac.h"
#include "rlc_stress_test_rrc.h"
#include "rlc_stress_test_traffic.h"
#include "ocudu/pcap/rlc_pcap.h"
#include "ocudu/pdcp/pdcp_entity.h"
#include "ocudu/pdcp/pdcp_metrics.h"
#include "ocudu/rlc/rlc_entity.h"
#include "ocudu/rlc/rlc_metrics.h"
#include "ocudu/rlc/rlc_window_seg_pool_factory.h"
#include "ocudu/rlc/rlc_window_seg_pools.h"
#include "ocudu/support/executors/task_worker.h"
#include <condition_variable>
#include <mutex>
#include <unistd.h>

namespace ocudu {

class mock_pdcp_metrics_notifier : public pdcp_metrics_notifier
{
  void report_metrics(const pdcp_metrics_container& metrics) override { metrics_list.push_back(metrics); }

public:
  std::vector<pdcp_metrics_container> metrics_list;
};

/// \brief Stack emulator used to stress test the RLC TX/RX entities.
/// This emulator will try to mimic a real gNB stack as closely as possible.
/// To do this, it will contain two executors (threads) the PCell executor and a UE executor.
/// The PCell executor will emulate the MAC TX by pulling PDUs from RLC;
/// while the UE executor will emulate the MAC RX and PDCP RX and TX.
class stress_stack
{
public:
  stress_stack(const stress_test_args& args_, uint32_t id, rb_id_t rb_id);

  ~stress_stack() = default;

  void start();
  void wait_for_finish();

  void run_upper_tti(uint32_t tti);
  void run_lower_tti(uint32_t tti);
  void push_pdus(std::vector<byte_buffer_chain> list_pdus);

  void set_peer_stack(stress_stack* peer_stack_) { peer_stack = peer_stack_; }

  pdcp_metrics_container get_pdcp_metrics()
  {
    if (pdcp_metrics_notifier.metrics_list.empty())
      return {};
    auto m = pdcp_metrics_notifier.metrics_list.back();
    return m;
  }
  rlc_metrics get_rlc_metrics() { return rlc->get_metrics(); }

  // Mutex and condition variables for stopping workers
  std::mutex              mutex_pcell;
  std::mutex              mutex_ue;
  std::condition_variable cv_pcell;
  std::condition_variable cv_ue;
  std::atomic<bool>       stopping_pcell = {false};
  std::atomic<bool>       stopping_ue    = {false};

private:
  // Stack ID for logging
  uint16_t stack_id;

  // Arguments
  const stress_test_args& args;

  // Executors
  uint16_t                       task_worker_queue_size = 15000;
  std::string                    ue_name;
  std::string                    pcell_name;
  task_worker                    ue_worker;
  task_worker                    pcell_worker;
  std::unique_ptr<task_executor> ue_executor;
  std::unique_ptr<task_executor> pcell_executor;
  mock_pdcp_metrics_notifier     pdcp_metrics_notifier;

  // Security
  security::sec_128_as_config sec_cfg;

  // Peer stack
  stress_stack* peer_stack = nullptr;

  // Timers
  timer_manager timers;

  // MAC
  std::unique_ptr<mac_dummy> mac = nullptr;

  // RLC
  std::unique_ptr<rlc_entity> rlc = nullptr;

  // F1AP
  std::unique_ptr<f1ap_dummy> f1ap = nullptr;

  // PDCP
  std::unique_ptr<pdcp_entity> pdcp = nullptr;

  // RRC
  std::unique_ptr<rrc_dummy> rrc = nullptr;

  // Traffic generators
  std::unique_ptr<stress_traffic_sink>   traffic_sink   = nullptr;
  std::unique_ptr<stress_traffic_source> traffic_source = nullptr;

  // Logging
  ocudu::rlc_bearer_logger logger;

  // PCAP
  null_rlc_pcap pcap;

  // Window segment pools
  std::unique_ptr<rlc_drb_rx_window_seg_pool, rlc_pool_deleter> drb_rx_seg_pool =
      make_rlc_drb_rx_window_seg_pool(rlc_drb_rx_window_seg_pool_size);
  std::unique_ptr<rlc_drb_tx_window_seg_pool, rlc_pool_deleter> drb_tx_seg_pool =
      make_rlc_drb_tx_window_seg_pool(rlc_drb_tx_window_seg_pool_size);
  std::unique_ptr<rlc_srb_rx_window_seg_pool, rlc_pool_deleter> srb_rx_seg_pool =
      make_rlc_srb_rx_window_seg_pool(rlc_srb_rx_window_seg_pool_size);
  std::unique_ptr<rlc_srb_tx_window_seg_pool, rlc_pool_deleter> srb_tx_seg_pool =
      make_rlc_srb_tx_window_seg_pool(rlc_srb_tx_window_seg_pool_size);
};

} // namespace ocudu
