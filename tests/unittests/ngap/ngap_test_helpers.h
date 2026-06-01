// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "test_helpers.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace ocudu::ocucp {

/// Fixture class for NGAP.
class ngap_test : public ::testing::Test
{
protected:
  class test_ue
  {
  public:
    test_ue(cu_cp_ue_index_t ue_index_) : ue_index(ue_index_), rrc_ue_handler(ue_index_) {}

    cu_cp_ue_index_t           ue_index = cu_cp_ue_index_t::invalid;
    std::optional<amf_ue_id_t> amf_ue_id;
    std::optional<ran_ue_id_t> ran_ue_id;

    dummy_rrc_ngap_message_handler rrc_ue_handler;
  };

  ngap_test();
  ~ngap_test() override;

  /// \brief Helper method to successfully run NG setup in NGAP.
  bool run_ng_setup();

  /// \brief Helper method to successfully create UE instance in NGAP and inject an InitialUEMessage.
  cu_cp_ue_index_t create_ue(rnti_t rnti = rnti_t::MIN_CRNTI);

  /// \brief Helper method to successfully create UE instance in NGAP.
  cu_cp_ue_index_t create_ue_without_init_ue_message(rnti_t rnti);

  /// \brief Helper method to successfully run DL NAS transport in NGAP.
  void run_dl_nas_transport(cu_cp_ue_index_t ue_index);

  /// \brief Helper method to successfully run UL NAS transport in NGAP.
  void run_ul_nas_transport(cu_cp_ue_index_t ue_index);

  /// \brief Helper method to successfully run Initial Context Setup in NGAP.
  void run_initial_context_setup(cu_cp_ue_index_t ue_index);

  /// \brief Helper method to enable security for a UE in NGAP.
  bool enable_ue_security(cu_cp_ue_index_t ue_index);

  /// \brief Helper method to successfully run PDU Session Resource Setup in NGAP
  void run_pdu_session_resource_setup(cu_cp_ue_index_t ue_index, pdu_session_id_t pdu_session_id);

  // Manually add existing PDU sessions to UP manager
  void add_pdu_session_to_up_manager(cu_cp_ue_index_t        ue_index,
                                     pdu_session_id_t        pdu_session_id,
                                     pdu_session_type_t      pdu_session_type,
                                     up_transport_layer_info ul_ngu_up_tnl_info,
                                     drb_id_t                drb_id,
                                     qos_flow_id_t           qos_flow_id);

  /// \brief Manually tick timers.
  void tick();

  ocudulog::basic_logger& ngap_logger = ocudulog::fetch_basic_logger("NGAP");
  ocudulog::basic_logger& test_logger = ocudulog::fetch_basic_logger("TEST");

  std::unordered_map<cu_cp_ue_index_t, test_ue> test_ues;

  timer_manager       timers;
  manual_task_worker  ctrl_worker{128};
  cu_cp_configuration cu_cp_cfg;

  ue_manager                      ue_mng{cu_cp_cfg};
  dummy_n2_gateway                n2_gw;
  dummy_ngap_message_handler      dummy_amf;
  dummy_ngap_cu_cp_notifier       cu_cp_notifier{ue_mng};
  std::unique_ptr<ngap_interface> ngap;
};

} // namespace ocudu::ocucp
