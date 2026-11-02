// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "fapi_to_mac_slot_indication_fastpath_translator.h"
#include "ocudu/fapi/p7/messages/slot_indication.h"
#include "ocudu/mac/mac_cell_slot_handler.h"
#include "ocudu/mac/mac_cell_timing_context.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <gtest/gtest.h>
#include <ostream>
#include <vector>

using namespace ocudu;
using namespace fapi_adaptor;

namespace {

/// Identifies a slot handler spy in the shared call log.
enum class handler_id { fapi, mac };

/// Prints the given handler identifier, so that gtest shows a readable value on failure.
std::ostream& operator<<(std::ostream& os, handler_id id)
{
  return os << (id == handler_id::fapi ? "fapi" : "mac");
}

/// MAC slot handler spy that records its slot indications in a log shared with the other spies, to check the order.
class mac_cell_slot_handler_spy : public mac_cell_slot_handler
{
public:
  mac_cell_slot_handler_spy(handler_id id_, std::vector<handler_id>& call_log_) : id(id_), call_log(call_log_) {}

  void handle_slot_indication(const mac_cell_timing_context& context) override
  {
    call_log.push_back(id);
    contexts.push_back(context);
  }

  void handle_error_indication(slot_point sl_tx, error_event event) override {}
  void handle_stop_indication() override {}

  std::vector<mac_cell_timing_context> contexts;

private:
  const handler_id         id;
  std::vector<handler_id>& call_log;
};

/// Fixture with a FAPI and a MAC slot handler spy that share their call log.
class fapi_to_mac_slot_indication_test : public ::testing::Test
{
protected:
  std::vector<handler_id>                         call_log;
  mac_cell_slot_handler_spy                       fapi_handler{handler_id::fapi, call_log};
  mac_cell_slot_handler_spy                       mac_handler{handler_id::mac, call_log};
  fapi_to_mac_slot_indication_fastpath_translator translator{0, fapi_handler, ocudulog::fetch_basic_logger("FAPI")};
};

} // namespace

/// Builds a SLOT.indication with a known slot and time point.
static fapi::slot_indication build_slot_indication()
{
  return fapi::slot_indication{.slot       = slot_point_extended(slot_point(subcarrier_spacing::kHz30, 5, 7), 3),
                               .time_point = std::chrono::system_clock::time_point(std::chrono::microseconds(123456))};
}

TEST_F(fapi_to_mac_slot_indication_test, slot_is_forwarded_to_both_handlers_with_the_same_context)
{
  translator.set_cell_slot_handler(mac_handler);

  fapi::slot_indication msg = build_slot_indication();
  translator.on_slot_indication(msg);

  ASSERT_EQ(1U, fapi_handler.contexts.size());
  ASSERT_EQ(1U, mac_handler.contexts.size());
  for (const mac_cell_timing_context& context : {fapi_handler.contexts.front(), mac_handler.contexts.front()}) {
    EXPECT_EQ(msg.slot, context.sl_tx);
    EXPECT_EQ(msg.time_point, context.time_point);
  }
}

TEST_F(fapi_to_mac_slot_indication_test, the_fapi_handler_is_notified_before_the_mac_handler)
{
  translator.set_cell_slot_handler(mac_handler);

  translator.on_slot_indication(build_slot_indication());

  EXPECT_EQ((std::vector<handler_id>{handler_id::fapi, handler_id::mac}), call_log);
}

TEST_F(fapi_to_mac_slot_indication_test, only_the_fapi_handler_is_notified_until_the_mac_handler_is_set)
{
  translator.on_slot_indication(build_slot_indication());

  EXPECT_EQ((std::vector<handler_id>{handler_id::fapi}), call_log);
}
