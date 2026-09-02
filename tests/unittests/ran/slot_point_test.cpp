// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/slot_point.h"
#include "fmt/format.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST(slot_point_test, slot_type)
{
  // TEST: constructors
  slot_point slot1;
  ASSERT_TRUE(not slot1.valid());
  slot_point slot2{0, 1, 5};
  ASSERT_TRUE(slot2.valid() and slot2.numerology() == 0 and slot2.slot_index() == 5 and slot2.slot_index() == 5 and
              slot2.sfn() == 1);
  slot_point slot3{slot2};
  ASSERT_TRUE(slot3 == slot2);

  // TEST: comparison and difference operators
  slot1 = slot_point{0, 1, 5};
  slot2 = slot_point{0, 1, 5};
  ASSERT_TRUE(slot1 == slot2 and slot1 <= slot2 and slot1 >= slot2);
  slot1++;
  ASSERT_TRUE(slot1 != slot2 and slot1 >= slot2 and slot1 > slot2 and slot2 < slot1 and slot2 <= slot1);
  ASSERT_TRUE(slot1 - slot2 == 1 and slot2 - slot1 == -1);
  slot1 = slot_point{0, 2, 5};
  ASSERT_TRUE(slot1 != slot2 and slot1 >= slot2 and slot1 > slot2 and slot2 < slot1 and slot2 <= slot1);
  ASSERT_TRUE(slot1 - slot2 == 10 and slot2 - slot1 == -10);
  slot1 = slot_point{0, 1023, 5};
  ASSERT_TRUE(slot1 != slot2 and slot1 <= slot2 and slot1 < slot2 and slot2 > slot1 and slot2 >= slot1);
  ASSERT_TRUE(slot1 - slot2 == -20 and slot2 - slot1 == 20);

  // TEST: increment/decrement operators
  slot1 = slot_point{0, 1, 5};
  slot2 = slot_point{0, 1, 5};
  ASSERT_TRUE(slot1++ == slot2);
  ASSERT_TRUE(slot2 + 1 == slot1);
  ASSERT_TRUE(++slot2 == slot1);
  slot1 = slot_point{0, 1, 5};
  slot2 = slot_point{0, 1, 5};
  ASSERT_TRUE(slot1 - 100 == slot2 - 100);
  ASSERT_TRUE(((slot1 - 100000) + 100000) == slot1);
  ASSERT_TRUE((slot1 - 10240) == slot1);
  ASSERT_TRUE((slot1 - 100).slot_index() == 5 and (slot1 - 100).sfn() == 1015);
  ASSERT_TRUE(((slot1 - 100) + 100) == slot1);
  ASSERT_TRUE(((slot1 - 1) + 1) == slot1);

  ASSERT_TRUE(fmt::format("{}", slot1) == fmt::format("{}.{}", slot1.sfn(), slot1.slot_index()));
}
