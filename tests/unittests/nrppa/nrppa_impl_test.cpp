// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/nrppa/nrppa_impl.h"
#include "tests/test_doubles/nrppa/nrppa_test_message_validators.h"
#include "tests/test_doubles/nrppa/nrppa_test_messages.h"
#include "ocudu/support/async/async_no_op_task.h"
#include "ocudu/support/async/fifo_async_task_scheduler.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// \brief Fake CU-CP notifier whose on_new_nrppa_ue() can be told to return nullptr, simulating a CU-CP UE that no
/// longer exists when a DL NRPPA message arrives for it.
class test_nrppa_cu_cp_notifier : public nrppa_cu_cp_notifier
{
public:
  nrppa_cu_cp_ue_notifier* on_new_nrppa_ue(cu_cp_ue_index_t ue_index) override { return ue_notifier; }

  void on_ul_nrppa_pdu(const byte_buffer&                                nrppa_pdu,
                       std::variant<cu_cp_ue_index_t, cu_cp_amf_index_t> ue_or_amf_index) override
  {
    ul_pdu_sent       = true;
    last_ul_nrppa_pdu = nrppa_pdu.copy();
  }

  async_task<trp_information_cu_cp_response_t>
  on_trp_information_request(const trp_information_request_t& request) override
  {
    return launch_no_op_task(trp_information_cu_cp_response_t{});
  }

  nrppa_cu_cp_ue_notifier* ue_notifier = nullptr;
  bool                     ul_pdu_sent = false;
  byte_buffer              last_ul_nrppa_pdu;
};

static asn1::nrppa::nr_ppa_pdu_c unpack_nrppa_pdu(const byte_buffer& pdu)
{
  asn1::nrppa::nr_ppa_pdu_c nrppa_pdu;
  asn1::cbit_ref            bref(pdu);
  report_fatal_error_if_not(nrppa_pdu.unpack(bref) == asn1::OCUDUASN_SUCCESS, "Failed to unpack NRPPa-PDU");
  return nrppa_pdu;
}

/// Fixture class for the NRPPA implementation.
class nrppa_impl_test : public ::testing::Test
{
protected:
  nrppa_impl_test()
  {
    logger.set_level(ocudulog::basic_levels::debug);
    ocudulog::init();
  }

  ~nrppa_impl_test()
  {
    // Flush logger after each test.
    ocudulog::flush();
  }

  ocudulog::basic_logger&   logger = ocudulog::fetch_basic_logger("NRPPA");
  timer_manager             timer_mng;
  manual_task_worker        ctrl_worker{128};
  fifo_async_task_scheduler task_sched{32};
  test_nrppa_cu_cp_notifier cu_cp_notifier;
  nrppa_impl                nrppa{{}, cu_cp_notifier, task_sched, timer_mng, ctrl_worker};
  cu_cp_ue_index_t          ue_index = uint_to_ue_index(0);
};

TEST_F(nrppa_impl_test, when_malformed_pdu_is_received_then_it_is_dropped)
{
  byte_buffer garbage_pdu = make_byte_buffer("ffffffffff").value();

  nrppa.get_nrppa_message_handler().handle_new_nrppa_pdu(garbage_pdu,
                                                         std::variant<cu_cp_ue_index_t, cu_cp_amf_index_t>{ue_index});

  ASSERT_FALSE(cu_cp_notifier.ul_pdu_sent);
}

TEST_F(nrppa_impl_test, when_positioning_information_request_for_unknown_ue_is_received_then_failure_is_sent)
{
  cu_cp_notifier.ue_notifier = nullptr;

  nrppa.get_nrppa_message_handler().handle_new_nrppa_pdu(generate_valid_positioning_information_request(),
                                                         std::variant<cu_cp_ue_index_t, cu_cp_amf_index_t>{ue_index});

  ASSERT_TRUE(cu_cp_notifier.ul_pdu_sent);
  ASSERT_TRUE(
      test_helpers::is_valid_nrppa_positioning_information_failure(unpack_nrppa_pdu(cu_cp_notifier.last_ul_nrppa_pdu)));
}

TEST_F(nrppa_impl_test, when_positioning_activation_request_for_unknown_ue_is_received_then_failure_is_sent)
{
  cu_cp_notifier.ue_notifier = nullptr;

  nrppa.get_nrppa_message_handler().handle_new_nrppa_pdu(generate_valid_positioning_activation_request(),
                                                         std::variant<cu_cp_ue_index_t, cu_cp_amf_index_t>{ue_index});

  ASSERT_TRUE(cu_cp_notifier.ul_pdu_sent);
  ASSERT_TRUE(
      test_helpers::is_valid_nrppa_positioning_activation_failure(unpack_nrppa_pdu(cu_cp_notifier.last_ul_nrppa_pdu)));
}

TEST_F(nrppa_impl_test, when_e_cid_meas_initiation_request_for_unknown_ue_is_received_then_failure_is_sent)
{
  cu_cp_notifier.ue_notifier = nullptr;

  nrppa.get_nrppa_message_handler().handle_new_nrppa_pdu(
      generate_valid_nrppa_e_cid_measurement_initiation_request(uint_to_lmf_ue_meas_id(1)),
      std::variant<cu_cp_ue_index_t, cu_cp_amf_index_t>{ue_index});

  ASSERT_TRUE(cu_cp_notifier.ul_pdu_sent);
  ASSERT_TRUE(test_helpers::is_valid_e_cid_meas_initiation_failure(unpack_nrppa_pdu(cu_cp_notifier.last_ul_nrppa_pdu)));
}
