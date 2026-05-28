// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "ngap_test_helpers.h"
#include "ocudu/cu_cp/inter_cu_handover_messages.h"
#include "ocudu/ran/cause/ngap_cause.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>
#include <variant>

using namespace ocudu;
using namespace ocucp;

class ngap_path_switch_procedure_test : public ngap_test
{
protected:
  cu_cp_ue_index_t create_ue()
  {
    cu_cp_ue_index_t ue_index = ue_mng.add_ue(cu_cp_du_index_t::min);
    if (ue_index == cu_cp_ue_index_t::invalid) {
      test_logger.error("Failed to create UE");
      return cu_cp_ue_index_t::invalid;
    }
    if (ue_mng.ue_admission_limit_reached()) {
      test_logger.error("Failed to create UE. UE not servable");
      ue_mng.remove_ue(ue_index);
      return cu_cp_ue_index_t::invalid;
    }
    if (not ue_mng.update_ue_context(
            ue_index, int_to_gnb_du_id(0), MIN_PCI, rnti_t::MIN_CRNTI, ocudu::cu_cp_du_cell_index_t::min)) {
      test_logger.error("Failed to update UE context with pci={} rnti={} pcell_index={}",
                        MIN_PCI,
                        rnti_t::MIN_CRNTI,
                        cu_cp_du_cell_index_t::min);
      return cu_cp_ue_index_t::invalid;
    }

    return ue_index;
  }

  static cu_cp_path_switch_request generate_path_switch_request(cu_cp_ue_index_t ue_index)
  {
    cu_cp_path_switch_request request;
    request.ue_index                          = ue_index;
    request.source_amf_ue_ngap_id             = 0;
    request.user_location_info.nr_cgi.plmn_id = plmn_identity::test_value();
    request.user_location_info.nr_cgi.nci     = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
    request.user_location_info.tai.plmn_id    = plmn_identity::test_value();
    request.user_location_info.tai.tac        = 7;
    request.supported_enc_algos               = {true, true, true};
    request.supported_int_algos               = {true, true, true};

    cu_cp_pdu_session_res_to_be_switched_dl_item item;
    item.pdu_session_id = pdu_session_id_t::min;
    item.dl_ngu_up_tnl_info =
        up_transport_layer_info{transport_layer_address::create_from_string("127.0.0.1"), int_to_gtpu_teid(1)};
    item.qos_flow_accepted_list.push_back(qos_flow_id_t::min);
    request.pdu_session_res_to_be_switched_dl_list.push_back(item);

    return request;
  }
};

/// Test Path Switch procedure timeout.
TEST_F(ngap_path_switch_procedure_test,
       when_path_switch_request_ack_not_received_within_timeout_then_path_switch_procedure_fails)
{
  // Test preparation: Create UE.
  cu_cp_ue_index_t ue_index = create_ue();

  // Action 1: Launch Path Switch Procedure.
  test_logger.info("Launch path switch procedure...");
  async_task<cu_cp_path_switch_response> t =
      ngap->handle_path_switch_request_required(generate_path_switch_request(ue_index));
  lazy_task_launcher<cu_cp_path_switch_response> t_launcher(t);

  // Status: Fail Path Switch procedure (AMF doesn't respond).
  for (unsigned msec_elapsed = 0; msec_elapsed < 5000; ++msec_elapsed) {
    ASSERT_FALSE(t.ready());
    this->tick();
  }

  // Check that path switch procedure failed due to timeout.
  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(std::holds_alternative<cu_cp_path_switch_request_failure>(t.get()));
}

/// Test unsuccessful Path Switch Request procedure.
TEST_F(ngap_path_switch_procedure_test, when_path_switch_request_failure_received_then_path_switch_procedure_fails)
{
  // Test preparation: Create UE.
  cu_cp_ue_index_t ue_index = create_ue();

  // Action 1: Launch Path Switch Procedure.
  test_logger.info("Launch path switch procedure...");
  async_task<cu_cp_path_switch_response> t =
      ngap->handle_path_switch_request_required(generate_path_switch_request(ue_index));
  lazy_task_launcher<cu_cp_path_switch_response> t_launcher(t);

  // Inject Path Switch Request Failure.
  ngap_message path_switch_request_failure =
      generate_path_switch_request_failure(amf_ue_id_t::min, ran_ue_id_t::min, ngap_cause_radio_network_t::unspecified);
  test_logger.info("Injecting Path Switch Request Failure");
  ngap->handle_message(path_switch_request_failure);

  // Check that path switch procedure failed.
  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(std::holds_alternative<cu_cp_path_switch_request_failure>(t.get()));
}

/// Test successful Path Switch Request procedure.
TEST_F(ngap_path_switch_procedure_test, when_path_switch_request_ack_received_then_path_switch_procedure_succeeds)
{
  // Test preparation: Create UE.
  cu_cp_ue_index_t ue_index = create_ue();

  // Action 1: Launch Path Switch Procedure.
  test_logger.info("Launch path switch procedure...");
  async_task<cu_cp_path_switch_response> t =
      ngap->handle_path_switch_request_required(generate_path_switch_request(ue_index));
  lazy_task_launcher<cu_cp_path_switch_response> t_launcher(t);

  // Inject Path Switch Request Ack.
  ngap_message path_switch_request_ack = generate_path_switch_request_ack(amf_ue_id_t::min, ran_ue_id_t::min);
  test_logger.info("Injecting Path Switch Request Ack");
  ngap->handle_message(path_switch_request_ack);

  // Check that path switch procedure succeeded.
  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(std::holds_alternative<cu_cp_path_switch_request_ack>(t.get()));
}
