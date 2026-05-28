// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "e1ap_cu_cp_test_helpers.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp_factory.h"
#include "ocudu/support/async/async_test_utils.h"

using namespace ocudu;
using namespace ocucp;

e1ap_cu_cp_test::e1ap_cu_cp_test() :
  cu_cp_cfg([this]() {
    cu_cp_configuration cucfg     = config_helpers::make_default_cu_cp_config();
    cucfg.services.timers         = &timers;
    cucfg.services.cu_cp_executor = &ctrl_worker;
    cucfg.admission.max_nof_ues   = 1024 * 4;
    return cucfg;
  }())
{
  test_logger.set_level(ocudulog::basic_levels::debug);
  e1ap_logger.set_level(ocudulog::basic_levels::debug);
  ocudulog::init();

  // We enable Json logging by default for the purpose of testing.
  e1ap_configuration tmp;
  tmp.proc_timeout     = std::chrono::milliseconds(10000);
  tmp.json_log_enabled = true;

  e1ap = create_e1ap(tmp,
                     cu_cp_cu_up_index_t::min,
                     e1ap_pdu_notifier,
                     cu_up_processor_notifier,
                     cu_cp_notifier,
                     timers,
                     ctrl_worker,
                     cu_cp_cfg.admission.max_nof_ues);
}

e1ap_cu_cp_test::~e1ap_cu_cp_test()
{
  // flush logger after each test
  ocudulog::flush();
}

void e1ap_cu_cp_test::run_bearer_context_setup(cu_cp_ue_index_t ue_index, gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
{
  e1ap_bearer_context_setup_request req = generate_bearer_context_setup_request(ue_index);

  // Start procedure in CU-CP.
  async_task<e1ap_bearer_context_setup_response>         t = e1ap->handle_bearer_context_setup_request(req);
  lazy_task_launcher<e1ap_bearer_context_setup_response> t_launcher(t);

  ASSERT_FALSE(t.ready());

  test_ues.emplace(ue_index, test_ue{ue_index});
  test_ue& u         = test_ues[ue_index];
  u.ue_index         = ue_index;
  u.cu_cp_ue_e1ap_id = int_to_gnb_cu_cp_ue_e1ap_id(
      this->e1ap_pdu_notifier.last_e1ap_msg.pdu.init_msg().value.bearer_context_setup_request()->gnb_cu_cp_ue_e1ap_id);
  u.cu_up_ue_e1ap_id = cu_up_ue_e1ap_id;

  // Handle response from CU-UP.
  e1ap_message bearer_context_setup_response =
      generate_bearer_context_setup_response(u.cu_cp_ue_e1ap_id.value(), cu_up_ue_e1ap_id);
  test_logger.info("Injecting BearerContextSetupResponse");
  e1ap->handle_message(bearer_context_setup_response);

  ocudu_assert(t.ready(), "The procedure should have completed by now");
}

e1ap_cu_cp_test::test_ue& e1ap_cu_cp_test::create_ue()
{
  cu_cp_ue_index_t ue_index = ue_mng.add_ue(cu_cp_du_index_t::min);
  report_fatal_error_if_not(ue_index != cu_cp_ue_index_t::invalid, "Failed to create UE");
  if (ue_mng.ue_admission_limit_reached()) {
    ue_mng.remove_ue(ue_index);
    report_fatal_error("Failed to create UE. UE not servable");
  }
  ue_mng.set_plmn(ue_index, plmn_identity::test_value());
  auto request = generate_bearer_context_setup_request(ue_index);

  run_bearer_context_setup(request.ue_index,
                           int_to_gnb_cu_up_ue_e1ap_id(test_rng::uniform_int<uint64_t>(
                               gnb_cu_up_ue_e1ap_id_to_uint(gnb_cu_up_ue_e1ap_id_t::min),
                               gnb_cu_up_ue_e1ap_id_to_uint(gnb_cu_up_ue_e1ap_id_t::max) - 1)));

  return test_ues[request.ue_index];
}

void e1ap_cu_cp_test::tick()
{
  timers.tick();
  ctrl_worker.run_pending_tasks();
}
