// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/rlc/rlc_rx_am_entity.h"
#include "lib/rlc/rlc_rx_tm_entity.h"
#include "lib/rlc/rlc_rx_um_entity.h"
#include "lib/rlc/rlc_tx_am_entity.h"
#include "lib/rlc/rlc_tx_tm_entity.h"
#include "lib/rlc/rlc_tx_um_entity.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <cstdio>
#include <fstream>
#include <getopt.h>

using namespace ocudu;

namespace {

/// Mocking class of the surrounding layers invoked by the RLC AM Rx entity.
class rlc_rx_am_test_frame : public rlc_rx_upper_layer_data_notifier,
                             public rlc_tx_am_status_handler,
                             public rlc_tx_am_status_notifier,
                             public rlc_metrics_notifier
{
public:
  std::unique_ptr<rlc_bearer_metrics_collector> metrics_collector;

  rlc_rx_am_test_frame(task_executor& ue_executor) :
    metrics_collector(std::make_unique<rlc_bearer_metrics_collector>(gnb_du_id_t{},
                                                                     du_ue_index_t{},
                                                                     rb_id_t{},
                                                                     timer_duration{1},
                                                                     this,
                                                                     ue_executor))
  {
  }

  // rlc_rx_upper_layer_data_notifier interface
  void on_new_sdu(byte_buffer_chain sdu) override {}

  // rlc_tx_am_status_handler interface
  void on_status_pdu(rlc_am_status_pdu status_) override {}
  // rlc_tx_am_status_notifier interface
  void on_status_report_changed() override {}
  // rlc_metrics_notifier
  void report_metrics(const rlc_metrics& metrics) override {}
};

/// Mocking class of the surrounding layers invoked by the RLC TM/UM Rx entity.
class rlc_rx_tm_um_test_frame : public rlc_rx_upper_layer_data_notifier, public rlc_metrics_notifier
{
public:
  std::unique_ptr<rlc_bearer_metrics_collector> metrics_collector;

  rlc_rx_tm_um_test_frame(task_executor& ue_executor) :
    metrics_collector(std::make_unique<rlc_bearer_metrics_collector>(gnb_du_id_t{},
                                                                     du_ue_index_t{},
                                                                     rb_id_t{},
                                                                     timer_duration{1},
                                                                     this,
                                                                     ue_executor))
  {
  }

  // rlc_rx_upper_layer_data_notifier interface
  void on_new_sdu(byte_buffer_chain sdu) override {}

  // rlc_metrics_notifier
  void report_metrics(const rlc_metrics& metrics) override {}
};

/// Mocking class of the surrounding layers invoked by the RLC AM Tx entity.
class rlc_tx_am_test_frame : public rlc_tx_upper_layer_data_notifier,
                             public rlc_tx_upper_layer_control_notifier,
                             public rlc_tx_lower_layer_notifier,
                             public rlc_rx_am_status_provider,
                             public rlc_metrics_notifier
{
public:
  rlc_am_status_pdu status;
  bool              status_required = false;

  std::unique_ptr<rlc_bearer_metrics_collector> metrics_collector;

  rlc_tx_am_test_frame(rlc_am_sn_size sn_size_, task_executor& ue_executor) :
    status(sn_size_),
    metrics_collector(std::make_unique<rlc_bearer_metrics_collector>(gnb_du_id_t{},
                                                                     du_ue_index_t{},
                                                                     rb_id_t{},
                                                                     timer_duration{1},
                                                                     this,
                                                                     ue_executor))
  {
  }

  // rlc_tx_upper_layer_data_notifier interface
  void on_transmitted_sdu(uint32_t max_tx_pdcp_sn, uint32_t desired_buf_size) override {}
  void on_delivered_sdu(uint32_t max_deliv_pdcp_sn) override {}
  void on_retransmitted_sdu(uint32_t max_retx_pdcp_sn) override {}
  void on_delivered_retransmitted_sdu(uint32_t max_deliv_retx_pdcp_sn) override {}

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() override {}
  void on_max_retx() override {}

  // rlc_tx_buffer_state_update_notifier interface
  void on_buffer_state_update(const rlc_buffer_state& bs) override {}

  // rlc_rx_am_status_provider interface
  rlc_am_status_pdu& get_status_pdu() override { return status; }
  uint32_t           get_status_pdu_length() override { return status.get_packed_size(); }
  bool               status_report_required() override { return status_required; }

  // rlc_metrics_notifier
  void report_metrics(const rlc_metrics& metrics) override {}
};

/// Mocking class of the surrounding layers invoked by the RLC TM/UM Tx entity.
class rlc_tx_tm_um_test_frame : public rlc_tx_upper_layer_data_notifier,
                                public rlc_tx_upper_layer_control_notifier,
                                public rlc_tx_lower_layer_notifier,
                                public rlc_metrics_notifier
{
public:
  std::unique_ptr<rlc_bearer_metrics_collector> metrics_collector;

  rlc_tx_tm_um_test_frame(task_executor& ue_executor) :
    metrics_collector(std::make_unique<rlc_bearer_metrics_collector>(gnb_du_id_t{},
                                                                     du_ue_index_t{},
                                                                     rb_id_t{},
                                                                     timer_duration{1},
                                                                     this,
                                                                     ue_executor))
  {
  }

  // rlc_tx_upper_layer_data_notifier interface
  void on_transmitted_sdu(uint32_t max_tx_pdcp_sn, uint32_t desired_buf_size) override {}
  void on_delivered_sdu(uint32_t max_deliv_pdcp_sn) override {}
  void on_retransmitted_sdu(uint32_t max_retx_pdcp_sn) override {}
  void on_delivered_retransmitted_sdu(uint32_t max_deliv_retx_pdcp_sn) override {}

  // rlc_tx_upper_layer_control_notifier interface
  void on_protocol_failure() override {}
  void on_max_retx() override {}

  // rlc_tx_buffer_state_update_notifier interface
  void on_buffer_state_update(const rlc_buffer_state& bs) override {}

  // rlc_metrics_notifier
  void report_metrics(const rlc_metrics& metrics) override {}
};

struct bench_params {
  // AM 12-bit
  unsigned nof_rlc_rx_am_12 = 0;
  unsigned nof_rlc_tx_am_12 = 0;

  // AM 18-bit
  unsigned nof_rlc_rx_am_18 = 0;
  unsigned nof_rlc_tx_am_18 = 0;

  // TM
  unsigned nof_rlc_rx_tm = 0;
  unsigned nof_rlc_tx_tm = 0;

  // UM 6-bit
  unsigned nof_rlc_rx_um_6 = 0;
  unsigned nof_rlc_tx_um_6 = 0;

  // UM 18-bit
  unsigned nof_rlc_rx_um_12 = 0;
  unsigned nof_rlc_tx_um_12 = 0;
};

void usage(const char* prog)
{
  fmt::print("Usage: {} [options]\n", prog);
  fmt::print("\t-h                  Show this message\n");
  fmt::print("\tWithout any option all counts default to 1. Specifying any option sets all others to 0.\n");
  fmt::print("\t--rx_am_12 <number> Number of RLC AM Rx 12-bit SN instances\n");
  fmt::print("\t--tx_am_12 <number> Number of RLC AM Tx 12-bit SN instances\n");
  fmt::print("\t--rx_am_18 <number> Number of RLC AM Rx 18-bit SN instances\n");
  fmt::print("\t--tx_am_18 <number> Number of RLC AM Tx 18-bit SN instances\n");
  fmt::print("\t--rx_tm    <number> Number of RLC TM Rx instances\n");
  fmt::print("\t--tx_tm    <number> Number of RLC TM Tx instances\n");
  fmt::print("\t--rx_um_6  <number> Number of RLC UM Rx 6-bit SN instances\n");
  fmt::print("\t--tx_um_6  <number> Number of RLC UM Tx 6-bit SN instances\n");
  fmt::print("\t--rx_um_12 <number> Number of RLC UM Rx 12-bit SN instances\n");
  fmt::print("\t--tx_um_12 <number> Number of RLC UM Tx 12-bit SN instances\n");
}

// clang-format off
const option long_options[] = {
    {"rx_am_12", required_argument, nullptr, 1},
    {"tx_am_12", required_argument, nullptr, 2},
    {"rx_am_18", required_argument, nullptr, 3},
    {"tx_am_18", required_argument, nullptr, 4},
    {"rx_tm",    required_argument, nullptr, 5},
    {"tx_tm",    required_argument, nullptr, 6},
    {"rx_um_6",  required_argument, nullptr, 7},
    {"tx_um_6",  required_argument, nullptr, 8},
    {"rx_um_12", required_argument, nullptr, 9},
    {"tx_um_12", required_argument, nullptr, 10},
    {nullptr, 0, nullptr, 0},
};
// clang-format on

void parse_args(int argc, char** argv, bench_params& params)
{
  bool any_set = false;
  int  opt     = 0;
  while ((opt = getopt_long(argc, argv, "h", long_options, nullptr)) != -1) {
    switch (opt) {
      case 1:
        params.nof_rlc_rx_am_12 = std::stoul(optarg);
        any_set                 = true;
        break;
      case 2:
        params.nof_rlc_tx_am_12 = std::stoul(optarg);
        any_set                 = true;
        break;
      case 3:
        params.nof_rlc_rx_am_18 = std::stoul(optarg);
        any_set                 = true;
        break;
      case 4:
        params.nof_rlc_tx_am_18 = std::stoul(optarg);
        any_set                 = true;
        break;
      case 5:
        params.nof_rlc_rx_tm = std::stoul(optarg);
        any_set              = true;
        break;
      case 6:
        params.nof_rlc_tx_tm = std::stoul(optarg);
        any_set              = true;
        break;
      case 7:
        params.nof_rlc_rx_um_6 = std::stoul(optarg);
        any_set                = true;
        break;
      case 8:
        params.nof_rlc_tx_um_6 = std::stoul(optarg);
        any_set                = true;
        break;
      case 9:
        params.nof_rlc_rx_um_12 = std::stoul(optarg);
        any_set                 = true;
        break;
      case 10:
        params.nof_rlc_tx_um_12 = std::stoul(optarg);
        any_set                 = true;
        break;
      case 'h':
      default:
        usage(argv[0]);
        std::exit(0);
    }
  }
  if (!any_set) {
    params = {.nof_rlc_rx_am_12 = 1,
              .nof_rlc_tx_am_12 = 1,
              .nof_rlc_rx_am_18 = 1,
              .nof_rlc_tx_am_18 = 1,
              .nof_rlc_rx_tm    = 1,
              .nof_rlc_tx_tm    = 1,
              .nof_rlc_rx_um_6  = 1,
              .nof_rlc_tx_um_6  = 1,
              .nof_rlc_rx_um_12 = 1,
              .nof_rlc_tx_um_12 = 1};
  }
}

void rlc_instances(const bench_params& params)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "Creating RLC instances");

  rlc_rx_am_config rx_am_12_cfg = {.sn_field_length   = rlc_am_sn_size::size12bits,
                                   .t_reassembly      = 20,
                                   .t_status_prohibit = 10,
                                   .max_sn_per_status = std::nullopt};

  rlc_rx_am_config rx_am_18_cfg = {.sn_field_length   = rlc_am_sn_size::size18bits,
                                   .t_reassembly      = 20,
                                   .t_status_prohibit = 10,
                                   .max_sn_per_status = std::nullopt};

  rlc_rx_tm_config rx_tm_cfg = {};

  rlc_rx_um_config rx_um_6_cfg = {.sn_field_length = rlc_um_sn_size::size6bits, .t_reassembly = 100};

  rlc_rx_um_config rx_um_12_cfg = {.sn_field_length = rlc_um_sn_size::size12bits, .t_reassembly = 100};

  rlc_tx_am_config tx_am_12_cfg = {.sn_field_length  = rlc_am_sn_size::size12bits,
                                   .pdcp_sn_len      = pdcp_sn_size::size12bits,
                                   .t_poll_retx      = 100,
                                   .max_retx_thresh  = 32,
                                   .poll_pdu         = 16,
                                   .poll_byte        = -1,
                                   .queue_size       = 16384,
                                   .queue_size_bytes = 4096 * (1500 + 7),
                                   .max_window       = 0};

  rlc_tx_am_config tx_am_18_cfg = {.sn_field_length  = rlc_am_sn_size::size18bits,
                                   .pdcp_sn_len      = pdcp_sn_size::size18bits,
                                   .t_poll_retx      = 100,
                                   .max_retx_thresh  = 32,
                                   .poll_pdu         = 16,
                                   .poll_byte        = -1,
                                   .queue_size       = 16384,
                                   .queue_size_bytes = 4096 * (1500 + 7),
                                   .max_window       = 0};

  rlc_tx_tm_config tx_tm_cfg = {.queue_size = 16384, .queue_size_bytes = 4096 * (1500 + 7)};

  rlc_tx_um_config tx_um_6_cfg = {
      .sn_field_length  = rlc_um_sn_size::size6bits,
      .pdcp_sn_len      = pdcp_sn_size::size12bits,
      .queue_size       = 16384,
      .queue_size_bytes = 4096 * (1500 + 7),
  };

  rlc_tx_um_config tx_um_12_cfg = {
      .sn_field_length  = rlc_um_sn_size::size12bits,
      .pdcp_sn_len      = pdcp_sn_size::size12bits,
      .queue_size       = 16384,
      .queue_size_bytes = 4096 * (1500 + 7),
  };

  timer_manager      timers;
  manual_task_worker pcell_worker{128};
  manual_task_worker ue_worker{128};

  null_rlc_pcap pcap;

  auto                                           rx_am_12_tester = std::make_unique<rlc_rx_am_test_frame>(ue_worker);
  std::vector<std::unique_ptr<rlc_rx_am_entity>> rx_am_12_instances;
  if (params.nof_rlc_rx_am_12 > 0) {
    rx_am_12_instances.reserve(params.nof_rlc_rx_am_12);
    for (unsigned i = 0; i < params.nof_rlc_rx_am_12; i++) {
      auto rx_am_12 = std::make_unique<rlc_rx_am_entity>(gnb_du_id_t::min,
                                                         du_ue_index_t::MIN_DU_UE_INDEX,
                                                         drb_id_t::drb1,
                                                         rx_am_12_cfg,
                                                         *rx_am_12_tester,
                                                         *rx_am_12_tester->metrics_collector,
                                                         pcap,
                                                         ue_worker,
                                                         timers);
      rx_am_12_instances.push_back(std::move(rx_am_12));
    }
  }

  auto                                           rx_am_18_tester = std::make_unique<rlc_rx_am_test_frame>(ue_worker);
  std::vector<std::unique_ptr<rlc_rx_am_entity>> rx_am_18_instances;
  if (params.nof_rlc_rx_am_18 > 0) {
    rx_am_18_instances.reserve(params.nof_rlc_rx_am_18);
    for (unsigned i = 0; i < params.nof_rlc_rx_am_18; i++) {
      auto rx_am_18 = std::make_unique<rlc_rx_am_entity>(gnb_du_id_t::min,
                                                         du_ue_index_t::MIN_DU_UE_INDEX,
                                                         drb_id_t::drb1,
                                                         rx_am_18_cfg,
                                                         *rx_am_18_tester,
                                                         *rx_am_18_tester->metrics_collector,
                                                         pcap,
                                                         ue_worker,
                                                         timers);
      rx_am_18_instances.push_back(std::move(rx_am_18));
    }
  }

  auto                                           rx_tm_tester = std::make_unique<rlc_rx_tm_um_test_frame>(ue_worker);
  std::vector<std::unique_ptr<rlc_rx_tm_entity>> rx_tm_instances;
  if (params.nof_rlc_rx_tm > 0) {
    rx_tm_instances.reserve(params.nof_rlc_rx_tm);
    for (unsigned i = 0; i < params.nof_rlc_rx_tm; i++) {
      auto rx_tm = std::make_unique<rlc_rx_tm_entity>(gnb_du_id_t::min,
                                                      du_ue_index_t::MIN_DU_UE_INDEX,
                                                      srb_id_t::srb1,
                                                      rx_tm_cfg,
                                                      *rx_tm_tester,
                                                      *rx_tm_tester->metrics_collector,
                                                      pcap,
                                                      ue_worker,
                                                      timers);
      rx_tm_instances.push_back(std::move(rx_tm));
    }
  }

  auto                                           rx_um_6_tester = std::make_unique<rlc_rx_tm_um_test_frame>(ue_worker);
  std::vector<std::unique_ptr<rlc_rx_um_entity>> rx_um_6_instances;
  if (params.nof_rlc_rx_um_6 > 0) {
    rx_um_6_instances.reserve(params.nof_rlc_rx_tm);
    for (unsigned i = 0; i < params.nof_rlc_rx_um_6; i++) {
      auto rx_um_6 = std::make_unique<rlc_rx_um_entity>(gnb_du_id_t::min,
                                                        du_ue_index_t::MIN_DU_UE_INDEX,
                                                        drb_id_t::drb1,
                                                        rx_um_6_cfg,
                                                        *rx_um_6_tester,
                                                        *rx_um_6_tester->metrics_collector,
                                                        pcap,
                                                        ue_worker,
                                                        timers);
      rx_um_6_instances.push_back(std::move(rx_um_6));
    }
  }

  auto                                           rx_um_12_tester = std::make_unique<rlc_rx_tm_um_test_frame>(ue_worker);
  std::vector<std::unique_ptr<rlc_rx_um_entity>> rx_um_12_instances;
  if (params.nof_rlc_rx_um_12 > 0) {
    rx_um_12_instances.reserve(params.nof_rlc_rx_tm);
    for (unsigned i = 0; i < params.nof_rlc_rx_um_12; i++) {
      auto rx_um_12 = std::make_unique<rlc_rx_um_entity>(gnb_du_id_t::min,
                                                         du_ue_index_t::MIN_DU_UE_INDEX,
                                                         drb_id_t::drb1,
                                                         rx_um_12_cfg,
                                                         *rx_um_12_tester,
                                                         *rx_um_12_tester->metrics_collector,
                                                         pcap,
                                                         ue_worker,
                                                         timers);
      rx_um_12_instances.push_back(std::move(rx_um_12));
    }
  }

  auto tx_am_12_tester = std::make_unique<rlc_tx_am_test_frame>(tx_am_12_cfg.sn_field_length, ue_worker);
  std::vector<std::unique_ptr<rlc_tx_am_entity>> tx_am_12_instances;
  if (params.nof_rlc_tx_am_12 > 0) {
    tx_am_12_instances.reserve(params.nof_rlc_tx_am_12);
    for (unsigned i = 0; i < params.nof_rlc_tx_am_12; i++) {
      auto tx_am_12 = std::make_unique<rlc_tx_am_entity>(gnb_du_id_t::min,
                                                         du_ue_index_t::MIN_DU_UE_INDEX,
                                                         drb_id_t::drb1,
                                                         tx_am_12_cfg,
                                                         *tx_am_12_tester,
                                                         *tx_am_12_tester,
                                                         *tx_am_12_tester,
                                                         *tx_am_12_tester->metrics_collector,
                                                         pcap,
                                                         pcell_worker,
                                                         ue_worker,
                                                         timers);
      tx_am_12->set_status_provider(tx_am_12_tester.get());
      tx_am_12_instances.push_back(std::move(tx_am_12));
    }
  }

  auto tx_am_18_tester = std::make_unique<rlc_tx_am_test_frame>(tx_am_18_cfg.sn_field_length, ue_worker);
  std::vector<std::unique_ptr<rlc_tx_am_entity>> tx_am_18_instances;
  if (params.nof_rlc_tx_am_18 > 0) {
    for (unsigned i = 0; i < params.nof_rlc_tx_am_18; i++) {
      auto tx_am_18 = std::make_unique<rlc_tx_am_entity>(gnb_du_id_t::min,
                                                         du_ue_index_t::MIN_DU_UE_INDEX,
                                                         drb_id_t::drb1,
                                                         tx_am_18_cfg,
                                                         *tx_am_18_tester,
                                                         *tx_am_18_tester,
                                                         *tx_am_18_tester,
                                                         *tx_am_18_tester->metrics_collector,
                                                         pcap,
                                                         pcell_worker,
                                                         ue_worker,
                                                         timers);
      tx_am_18->set_status_provider(tx_am_18_tester.get());
      tx_am_18_instances.push_back(std::move(tx_am_18));
    }
  }

  auto                                           tx_tm_tester = std::make_unique<rlc_tx_tm_um_test_frame>(ue_worker);
  std::vector<std::unique_ptr<rlc_tx_tm_entity>> tx_tm_instances;
  if (params.nof_rlc_tx_tm > 0) {
    for (unsigned i = 0; i < params.nof_rlc_tx_tm; i++) {
      auto tx_tm = std::make_unique<rlc_tx_tm_entity>(gnb_du_id_t::min,
                                                      du_ue_index_t::MIN_DU_UE_INDEX,
                                                      srb_id_t::srb1,
                                                      tx_tm_cfg,
                                                      *tx_tm_tester,
                                                      *tx_tm_tester,
                                                      *tx_tm_tester,
                                                      *tx_tm_tester->metrics_collector,
                                                      pcap,
                                                      pcell_worker,
                                                      ue_worker,
                                                      timers);
      tx_tm_instances.push_back(std::move(tx_tm));
    }
  }

  auto                                           tx_um_6_tester = std::make_unique<rlc_tx_tm_um_test_frame>(ue_worker);
  std::vector<std::unique_ptr<rlc_tx_um_entity>> tx_um_6_instances;
  if (params.nof_rlc_tx_um_6 > 0) {
    for (unsigned i = 0; i < params.nof_rlc_tx_um_6; i++) {
      auto tx_um_6 = std::make_unique<rlc_tx_um_entity>(gnb_du_id_t::min,
                                                        du_ue_index_t::MIN_DU_UE_INDEX,
                                                        drb_id_t::drb1,
                                                        tx_um_6_cfg,
                                                        *tx_um_6_tester,
                                                        *tx_um_6_tester,
                                                        *tx_um_6_tester,
                                                        *tx_um_6_tester->metrics_collector,
                                                        pcap,
                                                        pcell_worker,
                                                        ue_worker,
                                                        timers);
      tx_um_6_instances.push_back(std::move(tx_um_6));
    }
  }

  auto                                           tx_um_12_tester = std::make_unique<rlc_tx_tm_um_test_frame>(ue_worker);
  std::vector<std::unique_ptr<rlc_tx_um_entity>> tx_um_12_instances;
  if (params.nof_rlc_tx_um_12 > 0) {
    for (unsigned i = 0; i < params.nof_rlc_tx_um_12; i++) {
      auto tx_um_12 = std::make_unique<rlc_tx_um_entity>(gnb_du_id_t::min,
                                                         du_ue_index_t::MIN_DU_UE_INDEX,
                                                         drb_id_t::drb1,
                                                         tx_um_12_cfg,
                                                         *tx_um_12_tester,
                                                         *tx_um_12_tester,
                                                         *tx_um_12_tester,
                                                         *tx_um_12_tester->metrics_collector,
                                                         pcap,
                                                         pcell_worker,
                                                         ue_worker,
                                                         timers);
      tx_um_12_instances.push_back(std::move(tx_um_12));
    }
  }

  /// Virtual memory allocated by the process including untouched pages.
  long vm_size_kb = -1;
  /// Resident memory size covering physical memory pages mapped to the process.
  long vm_rss_kb = -1;
  if (std::ifstream proc_status("/proc/self/status"); proc_status.is_open()) {
    std::string line;
    while (std::getline(proc_status, line)) {
      if (line.rfind("VmSize:", 0) == 0) {
        std::sscanf(line.c_str(), "VmSize: %ld kB", &vm_size_kb);
      } else if (line.rfind("VmRSS:", 0) == 0) {
        std::sscanf(line.c_str(), "VmRSS: %ld kB", &vm_rss_kb);
      }
    }
  }
  fmt::print("Memory usage: vm_size_kb={} vm_rss_kb={}\n", vm_size_kb, vm_rss_kb);
}

} // namespace

int main(int argc, char** argv)
{
  ocudulog::fetch_basic_logger("RLC").set_level(ocudulog::basic_levels::warning);

  ocudulog::init();

  bench_params params{};
  parse_args(argc, argv, params);

  // Setup tiny size of byte buffer pool.
  init_byte_buffer_segment_pool(128);

  rlc_instances(params);

  ocudulog::flush();
}
