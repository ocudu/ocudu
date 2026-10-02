// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "si_test_helpers.h"
#include "ocudu/support/rtsan.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocudu::test_helpers;

namespace {

/// SIB whose SI message the PDU updates of these tests target.
constexpr sib_type updated_sib = sib_type::sib2;

/// Slots between two consecutive SI occasions of that SI message.
constexpr unsigned si_slot_period = 20;

} // namespace

/// \brief Tests the SI message extension handler through the assembler that owns it.
///
/// The handler is reached by serving the SI grants of an SI message that PDU updates were enqueued for, which is how
/// an NTN cell refreshes its SIB19 between SI modification windows.
class si_message_extension_handler_test : public ::testing::Test
{
public:
  si_message_extension_handler_test() : bench(make_sys_info_cfg()) {}

  static mac_cell_sys_info_config make_sys_info_cfg()
  {
    mac_cell_sys_info_config cfg;
    cfg.sib1 = make_random_pdu();
    cfg.si_messages.push_back(make_random_segmented_pdu(100, 1));
    cfg.si_sched_cfg.si_messages.emplace_back().sibs = sib_type_set{updated_sib};
    return cfg;
  }

  /// Enqueues one PDU per SI occasion, the first at the current slot and the rest one period apart.
  [[nodiscard]] bool enqueue_updates(span<const byte_buffer> pdus)
  {
    mac_cell_sys_info_pdu_update req;
    req.sib_idx        = updated_sib;
    req.slot           = bench.current_slot.without_hyper_sfn();
    req.si_slot_period = si_slot_period;
    req.si_messages    = pdus;
    return bench.push_si_pdu_updates(req);
  }

  /// \brief Serves the SI grant of the updated SI message, as the cell does at every SI occasion.
  ///
  /// The real-time sanitizer is enabled for the call: this is the cell real-time path, so an allocation or a release
  /// inside it is the defect these tests cover, not merely a slow path.
  span<const uint8_t> serve_si_grant(units::bytes tbs)
  {
    const sib_information si_info = make_sib_pdu(0, 0, tbs, sib_type_set{updated_sib});

    OCUDU_RTSAN_SCOPED_ENABLER;
    return bench.assembler.encode_si_pdu(bench.current_slot, si_info);
  }

  si_bench bench;
};

TEST_F(si_message_extension_handler_test, when_updates_are_enqueued_then_every_occasion_carries_its_own_pdu)
{
  constexpr unsigned       nof_updates = 8;
  std::vector<byte_buffer> pdus;
  for (unsigned i = 0; i != nof_updates; ++i) {
    pdus.push_back(make_random_pdu());
  }
  ASSERT_TRUE(enqueue_updates(pdus));

  for (unsigned i = 0; i != nof_updates; ++i) {
    const units::bytes        tbs      = units::bytes{static_cast<unsigned>(pdus[i].length())} + units::bytes{4};
    const span<const uint8_t> pdu      = serve_si_grant(tbs);
    const byte_buffer         expected = make_pdu_with_padding(pdus[i], tbs);
    ASSERT_EQ(expected, pdu) << fmt::format("Occasion {} carried the wrong SI-message payload", i);

    bench.current_slot += si_slot_period;
  }
}

TEST_F(si_message_extension_handler_test, when_updates_outnumber_the_pool_then_the_broadcast_buffers_are_reused)
{
  // Many more updates than the buffer pool holds, enqueued one at a time as an NTN cell refreshes its SIB19. A
  // buffer that the cell stopped broadcasting but never returned to the pool would exhaust it well before the end
  // of this loop, and enqueueing would start to fail.
  constexpr unsigned nof_cycles = 400;
  for (unsigned i = 0; i != nof_cycles; ++i) {
    const byte_buffer pdu = make_random_pdu();
    ASSERT_TRUE(enqueue_updates(span<const byte_buffer>(&pdu, 1))) << fmt::format("Update {} was rejected", i);

    const units::bytes        tbs    = units::bytes{static_cast<unsigned>(pdu.length())} + units::bytes{4};
    const span<const uint8_t> served = serve_si_grant(tbs);
    ASSERT_EQ(make_pdu_with_padding(pdu, tbs), served) << fmt::format("Cycle {} carried the wrong payload", i);

    bench.current_slot += si_slot_period;
  }
}
