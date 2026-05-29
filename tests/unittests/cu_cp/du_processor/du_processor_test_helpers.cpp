// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_processor_test_helpers.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/support/async/async_task_scheduler.h"
#include <memory>

using namespace ocudu;
using namespace ocucp;

namespace {

class dummy_task_sched final : public async_task_scheduler
{
public:
  bool schedule(async_task<void> task) override { return task_sched.schedule(std::move(task)); }

private:
  fifo_async_task_scheduler task_sched{32};
};

struct dummy_cu_cp_ue_admission_controller : public cu_cp_ue_admission_controller {
  bool request_ue_setup() const override { return true; }
};

struct dummy_cu_cp_measurement_handler : public cu_cp_measurement_handler {
  std::optional<rrc_meas_cfg>
  handle_measurement_config_request(cu_cp_ue_index_t                   ue_index,
                                    nr_cell_identity                   nci,
                                    const std::optional<rrc_meas_cfg>& current_meas_config = std::nullopt,
                                    bool                               cond_meas           = false,
                                    span<const pci_t>                  candidate_pcis      = {}) override
  {
    return std::nullopt;
  }
  void handle_measurement_report(const cu_cp_ue_index_t ue_index, const rrc_meas_results& meas_results) override {}
};

struct dummy_cu_cp_ue_removal_handler : public cu_cp_ue_removal_handler {
  async_task<void> handle_ue_removal_request(cu_cp_ue_index_t ue_index) override { return launch_no_op_task(); }
  void             handle_pending_ue_task_cancellation(cu_cp_ue_index_t ue_index) override {}
};

struct dummy_cu_cp_rrc_ue_interface : public cu_cp_rrc_ue_interface {
  bool handle_ue_plmn_selected(cu_cp_ue_index_t ue_index, const plmn_identity& plmn) override { return true; }
  rrc_ue_reestablishment_context_response
  handle_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti, cu_cp_ue_index_t ue_index) override
  {
    return {};
  }
  async_task<bool> handle_rrc_reestablishment_context_modification_required(cu_cp_ue_index_t ue_index) override
  {
    return launch_no_op_task(true);
  }
  void             handle_rrc_reestablishment_failure(const cu_cp_ue_context_release_request& request) override {}
  void             handle_rrc_reestablishment_complete(cu_cp_ue_index_t old_ue_index) override {}
  async_task<bool> handle_ue_context_transfer(cu_cp_ue_index_t ue_index, cu_cp_ue_index_t old_ue_index) override
  {
    return launch_no_op_task(true);
  }
  async_task<void> handle_ue_context_release(const cu_cp_ue_context_release_request& request) override
  {
    return launch_no_op_task();
  }
  async_task<rrc_resume_request_response> handle_rrc_resume_request(const rrc_resume_request& request) override
  {
    return launch_no_op_task(rrc_resume_request_response{.success = true});
  }
  void handle_ran_paging_required(cu_cp_ue_index_t ue_index) override {}
};

struct dummy_cu_cp_du_event_handler : public cu_cp_du_event_handler {
public:
  dummy_cu_cp_du_event_handler(ue_manager& ue_mng_) : ue_mng(ue_mng_) {}

  void handle_rrc_ue_creation(cu_cp_ue_index_t ue_index, rrc_ue_interface& rrc_ue) override
  {
    ue_mng.get_rrc_ue_cu_cp_adapter(ue_index).connect_cu_cp(rrc_ue_handler,
                                                            ue_rem_handler,
                                                            ue_admission_handler,
                                                            ue_mng.find_ue(ue_index)->get_up_resource_manager(),
                                                            meas_handler);
  }
  byte_buffer handle_target_cell_sib1_required(cu_cp_du_index_t du_index, nr_cell_global_id_t cgi) override
  {
    return {};
  }
  async_task<void> handle_transaction_info_loss(const ue_transaction_info_loss_event& ev) override
  {
    return launch_no_op_task();
  }

private:
  ue_manager&                         ue_mng;
  dummy_cu_cp_ue_admission_controller ue_admission_handler;
  dummy_cu_cp_measurement_handler     meas_handler;
  dummy_cu_cp_ue_removal_handler      ue_rem_handler;
  dummy_cu_cp_rrc_ue_interface        rrc_ue_handler;
};

} // namespace

du_processor_test::du_processor_test() :
  cu_cp_cfg([this]() {
    cu_cp_configuration cucfg     = config_helpers::make_default_cu_cp_config();
    cucfg.services.timers         = &timers;
    cucfg.services.cu_cp_executor = &ctrl_worker;
    cu_cp_cfg.ngap.ngaps.push_back(
        cu_cp_configuration::ngap_config{nullptr, {{7, {{plmn_identity::test_value(), {{slice_service_type{1}}}}}}}});

    return cucfg;
  }()),
  common_task_sched(std::make_unique<dummy_task_sched>()),

  du_cfg_mgr{cu_cp_cfg.node.gnb_id, config_helpers::get_supported_plmns(cu_cp_cfg.ngap.ngaps)}
{
  test_logger.set_level(ocudulog::basic_levels::debug);
  cu_cp_logger.set_level(ocudulog::basic_levels::debug);
  ocudulog::init();

  // Create and start DU processor.
  du_processor_config_t du_cfg = {uint_to_cu_cp_du_index(0),
                                  cu_cp_cfg,
                                  ocudulog::fetch_basic_logger("CU-CP"),
                                  &du_conn_notifier,
                                  du_cfg_mgr.create_du_handler()};
  du_processor_obj =
      create_du_processor(std::move(du_cfg), cu_cp_notifier, f1ap_pdu_notifier, *common_task_sched, ue_mng);

  cu_cp_event_handler = std::make_unique<dummy_cu_cp_du_event_handler>(ue_mng);
  cu_cp_notifier.attach_handler(&*cu_cp_event_handler, nullptr, du_processor_obj.get());
}

du_processor_test::~du_processor_test()
{
  // Flush logger after each test.
  ocudulog::flush();
}
