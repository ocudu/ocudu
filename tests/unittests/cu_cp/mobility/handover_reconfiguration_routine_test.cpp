// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/cu_cp/routines/mobility/handover_reconfiguration_routine.h"
#include "mobility_test_helpers.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/rrc/rrc_types.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/async/coroutine.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

class handover_reconfiguration_routine_test : public mobility_test
{
protected:
  handover_reconfiguration_routine_test() {}

  void create_ues(bool procedure_outcome, unsigned transaction_id_)
  {
    cu_cp_ue_index_t source_ue_index = get_ue_manager()->add_ue(source_du_index);
    ASSERT_NE(source_ue_index, cu_cp_ue_index_t::invalid);
    ASSERT_FALSE(get_ue_manager()->ue_admission_limit_reached());
    ASSERT_TRUE(get_ue_manager()->update_ue_context(
        source_ue_index, int_to_gnb_du_id(0), source_pci, source_rnti, cu_cp_du_cell_index_t::min));
    get_ue_manager()->set_plmn(source_ue_index, plmn_identity::test_value());
    source_ue = get_ue_manager()->find_ue(source_ue_index);
    ASSERT_NE(source_ue, nullptr);
    source_rrc_ue.set_transaction_id(transaction_id_);
    source_ue->set_rrc_ue(source_rrc_ue);

    cu_cp_ue_index_t target_ue_index = get_ue_manager()->add_ue(target_du_index);
    ASSERT_NE(target_ue_index, cu_cp_ue_index_t::invalid);
    ASSERT_FALSE(get_ue_manager()->ue_admission_limit_reached());
    ASSERT_TRUE(get_ue_manager()->update_ue_context(
        target_ue_index, int_to_gnb_du_id(0), target_pci, target_rnti, cu_cp_du_cell_index_t::min));
    get_ue_manager()->set_plmn(target_ue_index, plmn_identity::test_value());
    target_ue = get_ue_manager()->find_ue(target_ue_index);
    ASSERT_NE(target_ue, nullptr);
    source_f1ap_ue_ctxt_mng.set_ue_context_modification_outcome(
        {procedure_outcome,
         {},
         {},
         {},
         make_byte_buffer(
             "5c04c00604b0701f00811a0f020001273b8c01692f30004d25e24040008c8040a26418d6d8d76006e08040000101000083446a48"
             "d802692f1200000464e35b63224f8060664abff0124e9106e28dc61b8e372c6fbf56c70eb00442c0680182c4602c020521004930"
             "b2a0003fe00000000060dc2108000780594008300000020698101450a000e3890000246aac90838002081840a1839389142c60d1"
             "c3c81100000850400800b50401000850501800b50502000850602800b506030c88500038c8b500040c88501048c5048143141405"
             "8c50581831418068c50681c3141c0780904020024120880905204963028811d159e26f2681d2083c5df81821c00000038ffd294a"
             "5294f2816000021976000000000000000000088ad5450047001800082000e21005c400e0202108001c4200b8401c080441000388"
             "401708038180842000710802e18070401104000e21005c300080000008218081018201c1a0001c71000000080100020180020240"
             "088029800008c48089c7001800")
             .value()});
    target_ue->set_rrc_ue(target_rrc_ue);
  }

  void start_procedure()
  {
    rrc_reconfiguration_procedure_request request;
    rrc_recfg_v1530_ies                   recfg_v1530_ies;
    recfg_v1530_ies.master_cell_group =
        make_byte_buffer(
            "5c00b001117aec701061e0007c0204683c080004120981950001ff000000000306e10840003c02ca00418000001034c080a2850007"
            "1c4800081aab2420e0008206102860e4e2450b183470f204400002140002002d40004002140406002d4040800214080a002d4080c0"
            "02140c0e002d40c1032214041201410050050481601414060050581a01418070050681e0141c080c5040220241481258c0a1c47456"
            "789bc9a074820f177e060870000000e3ff4a5294a53ca0580000865d8000000000000000000315aa8a008e003000104001c4200b88"
            "01c040421000388401708038100882000710802e10070301084000e21005c300e0802208001c4200b8600100000010430102030403"
            "83400038e20000001002000403000404801100530000118801138e0030")
            .value();
    recfg_v1530_ies.ded_sib1_delivery =
        make_byte_buffer("d20405c04100100800003800000cd8601584268000800119008144c831adb186cdc1008000020200010688d490484"
                         "c109a4bc48000011938d6d8c893e0181992affc0493a441b8a37186e38dcb1befd5b1c3a8021603400c162da08a")
            .value();
    request.non_crit_ext = recfg_v1530_ies;

    e1ap_bearer_context_modification_request target_bearer_context_modification_request;

    t = launch_async<handover_reconfiguration_routine>(request,
                                                       target_bearer_context_modification_request,
                                                       target_ue->get_ue_index(),
                                                       *source_ue,
                                                       source_f1ap_ue_ctxt_mng,
                                                       cu_cp_handler,
                                                       test_logger);
    t_launcher.emplace(t);
  }

  void set_sub_procedure_outcome(bool outcome)
  {
    source_f1ap_ue_ctxt_mng.set_ue_context_modification_outcome(ue_context_outcome_t{outcome, {}, {}, {}});
  }

  bool procedure_ready() const { return t.ready(); }

  bool get_result() { return t.get(); }

  bool check_transaction_id(unsigned transaction_id) { return cu_cp_handler.last_transaction_id == transaction_id; }

  const f1ap_ue_context_modification_request& get_source_f1ap_ctxt_mod_request()
  {
    return source_f1ap_ue_ctxt_mng.get_ctxt_mod_request();
  }

private:
  // source UE parameters.
  cu_cp_du_index_t              source_du_index = uint_to_cu_cp_du_index(0);
  pci_t                         source_pci      = 1;
  rnti_t                        source_rnti     = to_rnti(0x4601);
  dummy_rrc_ue                  source_rrc_ue;
  dummy_f1ap_ue_context_manager source_f1ap_ue_ctxt_mng;
  cu_cp_ue*                     source_ue = nullptr;

  // target UE parameters.
  cu_cp_du_index_t target_du_index = uint_to_cu_cp_du_index(1);
  pci_t            target_pci      = 2;
  rnti_t           target_rnti     = to_rnti(0x4601);
  dummy_rrc_ue     target_rrc_ue;
  cu_cp_ue*        target_ue = nullptr;

  async_task<bool>                        t;
  std::optional<lazy_task_launcher<bool>> t_launcher;
};

TEST_F(handover_reconfiguration_routine_test, when_reconfiguration_successful_then_return_true)
{
  unsigned test_transaction_id = 99;

  // Test Preamble.
  create_ues(true, test_transaction_id);

  set_sub_procedure_outcome(true);

  // it should be ready immediately
  start_procedure();

  ASSERT_TRUE(procedure_ready());

  // Reconfiguration complete was received.
  ASSERT_TRUE(get_result());

  ASSERT_TRUE(check_transaction_id(test_transaction_id));

  // Make sure that the source UP context was not modified.
  f1ap_ue_context_modification_request context_mod = get_source_f1ap_ctxt_mod_request();
  ASSERT_TRUE(context_mod.drbs_to_be_setup_mod_list.empty());
  ASSERT_TRUE(context_mod.drbs_to_be_released_list.empty());
}

TEST_F(handover_reconfiguration_routine_test, when_ue_context_mod_unsuccessful_then_return_false)
{
  unsigned test_transaction_id = 35;

  // Test Preamble.
  create_ues(false, test_transaction_id);

  set_sub_procedure_outcome(false);

  // it should be ready immediately
  start_procedure();

  ASSERT_TRUE(procedure_ready());

  // Reconfiguration complete was received.
  ASSERT_FALSE(get_result());
}
