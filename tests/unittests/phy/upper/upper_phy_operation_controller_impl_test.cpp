// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../../../../lib/phy/upper/upper_phy_operation_controller_impl.h"
#include "ocudu/phy/upper/upper_phy_timing_context.h"
#include <gtest/gtest.h>
#include <vector>

using namespace ocudu;

namespace {

/// Records every forwarded slot boundary event.
class timing_notifier_spy : public upper_phy_timing_notifier
{
public:
  void on_tti_boundary(const upper_phy_timing_context& context) override { slots.push_back(context.slot); }

  std::vector<slot_point_extended> slots;
};

upper_phy_timing_context make_context(uint32_t count)
{
  upper_phy_timing_context context;
  context.slot = slot_point_extended(subcarrier_spacing::kHz30, count);
  return context;
}

TEST(UpperPhyOperationControllerImplTest, StartStopWithoutNotifierIsNoOp)
{
  upper_phy_operation_controller_impl controller;

  controller.start();
  controller.stop();
  controller.get_timing_notifier_proxy().on_tti_boundary(make_context(10));
}

TEST(UpperPhyOperationControllerImplTest, ForwardsByDefault)
{
  upper_phy_operation_controller_impl controller;
  timing_notifier_spy                 spy;
  controller.connect_timing_notifier(spy);

  controller.get_timing_notifier_proxy().on_tti_boundary(make_context(10));

  ASSERT_EQ(spy.slots.size(), 1U);
  EXPECT_EQ(spy.slots.front(), slot_point_extended(subcarrier_spacing::kHz30, 10));
}

TEST(UpperPhyOperationControllerImplTest, StopSuppressesForwarding)
{
  upper_phy_operation_controller_impl controller;
  timing_notifier_spy                 spy;
  controller.connect_timing_notifier(spy);

  controller.stop();
  controller.get_timing_notifier_proxy().on_tti_boundary(make_context(11));

  EXPECT_TRUE(spy.slots.empty());
}

TEST(UpperPhyOperationControllerImplTest, StartResumesForwarding)
{
  upper_phy_operation_controller_impl controller;
  timing_notifier_spy                 spy;
  controller.connect_timing_notifier(spy);

  controller.stop();
  controller.get_timing_notifier_proxy().on_tti_boundary(make_context(20));
  controller.start();
  controller.get_timing_notifier_proxy().on_tti_boundary(make_context(21));

  ASSERT_EQ(spy.slots.size(), 1U);
  EXPECT_EQ(spy.slots.front(), slot_point_extended(subcarrier_spacing::kHz30, 21));
}

TEST(UpperPhyOperationControllerImplTest, RestartAfterStoppedPeriodForwardsTheNewSlot)
{
  upper_phy_operation_controller_impl controller;
  timing_notifier_spy                 spy;
  controller.connect_timing_notifier(spy);

  // Cell running: one slot forwarded.
  controller.get_timing_notifier_proxy().on_tti_boundary(make_context(100));

  // Cell stopped: slots keep arriving from the timing handler but none are forwarded.
  controller.stop();
  for (uint32_t count = 101; count != 150; ++count) {
    controller.get_timing_notifier_proxy().on_tti_boundary(make_context(count));
  }

  // Cell restarted after the stopped period: the next slot is forwarded as-is. The receiver
  // observes the jump relative to the last slot forwarded before the stop.
  controller.start();
  controller.get_timing_notifier_proxy().on_tti_boundary(make_context(150));

  ASSERT_EQ(spy.slots.size(), 2U);
  EXPECT_EQ(spy.slots.front(), slot_point_extended(subcarrier_spacing::kHz30, 100));
  EXPECT_EQ(spy.slots.back(), slot_point_extended(subcarrier_spacing::kHz30, 150));
}

} // namespace
