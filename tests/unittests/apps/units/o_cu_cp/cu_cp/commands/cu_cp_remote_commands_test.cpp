// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/o_cu_cp/cu_cp/commands/cu_cp_remote_commands.h"
#include "ocudu/cu_cp/cu_cp_cell_command_handler.h"
#include "ocudu/cu_cp/cu_cp_command_handler.h"
#include "ocudu/ran/nr_cgi.h"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace ocudu;

namespace {

/// Fake cu_cp_cell_command_handler that records the last dispatch call and returns a
/// configurable success/failure for dispatch_*.
class capturing_cell_command_handler : public ocucp::cu_cp_cell_command_handler
{
public:
  std::optional<nr_cell_global_id_t> last_deactivate_cgi;
  std::optional<nr_cell_global_id_t> last_activate_cgi;
  bool                               next_dispatch_result = true;

  async_task<ocucp::cu_cp_cell_command_response> deactivate_cell(const nr_cell_global_id_t&) override
  {
    return launch_async([](coro_context<async_task<ocucp::cu_cp_cell_command_response>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(ocucp::cu_cp_cell_command_response{});
    });
  }

  async_task<ocucp::cu_cp_cell_command_response> activate_cell(const nr_cell_global_id_t&) override
  {
    return launch_async([](coro_context<async_task<ocucp::cu_cp_cell_command_response>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(ocucp::cu_cp_cell_command_response{});
    });
  }

  bool dispatch_deactivate_cell(const nr_cell_global_id_t& cgi) override
  {
    last_deactivate_cgi = cgi;
    return next_dispatch_result;
  }

  bool dispatch_activate_cell(const nr_cell_global_id_t& cgi) override
  {
    last_activate_cgi = cgi;
    return next_dispatch_result;
  }
};

/// Fake cu_cp_command_handler whose get_cell_command_handler() returns the capturing handler.
/// Other accessors are not exercised by the WS command tests and abort if called.
class fake_cu_cp_command_handler : public ocucp::cu_cp_command_handler
{
public:
  capturing_cell_command_handler cell_cmd;

  ocucp::cu_cp_mobility_command_handler& get_mobility_command_handler() override { std::abort(); }

  ocucp::cu_cp_ue_release_command_handler& get_ue_release_command_handler() override { std::abort(); }

  ocucp::cu_cp_ntn_meas_update_handler& get_ntn_meas_update_handler() override { std::abort(); }

  ocucp::cu_cp_cell_command_handler& get_cell_command_handler() override { return cell_cmd; }
};

/// Build the canonical {cgi: {plmn, nci}} payload accepted by cell_lock and cell_unlock.
nlohmann::json make_valid_payload()
{
  nlohmann::json req;
  req["cgi"]["plmn"] = "00101";
  req["cgi"]["nci"]  = uint64_t{6733824};
  return req;
}

} // namespace

// ── cell_lock happy path + dispatch behavior ──

TEST(cu_cp_cell_lock_remote_command_test, valid_payload_dispatches_deactivate_with_the_provided_cgi)
{
  fake_cu_cp_command_handler cu_cp;
  cell_lock_remote_command   cmd(cu_cp);

  error_type<std::string> result = cmd.execute(make_valid_payload());

  ASSERT_TRUE(result.has_value()) << "execute returned error: " << result.error();
  ASSERT_TRUE(cu_cp.cell_cmd.last_deactivate_cgi.has_value()) << "dispatch_deactivate_cell was not invoked";
  EXPECT_EQ(cu_cp.cell_cmd.last_deactivate_cgi->nci.value(), 6733824U);
  EXPECT_FALSE(cu_cp.cell_cmd.last_activate_cgi.has_value())
      << "dispatch_activate_cell should not be invoked from cell_lock";
}

TEST(cu_cp_cell_lock_remote_command_test, cu_cp_rejects_dispatch_returns_error)
{
  fake_cu_cp_command_handler cu_cp;
  cu_cp.cell_cmd.next_dispatch_result = false;
  cell_lock_remote_command cmd(cu_cp);

  error_type<std::string> result = cmd.execute(make_valid_payload());

  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("CU-CP rejected cell_lock"), std::string::npos)
      << "Unexpected error message: " << result.error();
}

// ── cell_unlock happy path + dispatch behavior ──

TEST(cu_cp_cell_unlock_remote_command_test, valid_payload_dispatches_activate_with_the_provided_cgi)
{
  fake_cu_cp_command_handler cu_cp;
  cell_unlock_remote_command cmd(cu_cp);

  error_type<std::string> result = cmd.execute(make_valid_payload());

  ASSERT_TRUE(result.has_value()) << "execute returned error: " << result.error();
  ASSERT_TRUE(cu_cp.cell_cmd.last_activate_cgi.has_value()) << "dispatch_activate_cell was not invoked";
  EXPECT_EQ(cu_cp.cell_cmd.last_activate_cgi->nci.value(), 6733824U);
  EXPECT_FALSE(cu_cp.cell_cmd.last_deactivate_cgi.has_value())
      << "dispatch_deactivate_cell should not be invoked from cell_unlock";
}

TEST(cu_cp_cell_unlock_remote_command_test, cu_cp_rejects_dispatch_returns_error)
{
  fake_cu_cp_command_handler cu_cp;
  cu_cp.cell_cmd.next_dispatch_result = false;
  cell_unlock_remote_command cmd(cu_cp);

  error_type<std::string> result = cmd.execute(make_valid_payload());

  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("CU-CP rejected cell_unlock"), std::string::npos)
      << "Unexpected error message: " << result.error();
}

// ── Shared JSON parsing error paths (cell_lock used as the representative; cell_unlock shares the
// same parser so identical coverage would be redundant) ──

TEST(cu_cp_cell_lock_remote_command_test, missing_cgi_object_returns_error)
{
  fake_cu_cp_command_handler cu_cp;
  cell_lock_remote_command   cmd(cu_cp);

  nlohmann::json req;
  req["something_else"] = "value";

  error_type<std::string> result = cmd.execute(req);
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("'cgi'"), std::string::npos) << result.error();
  EXPECT_FALSE(cu_cp.cell_cmd.last_deactivate_cgi.has_value()) << "Dispatch must not run on parse error";
}

TEST(cu_cp_cell_lock_remote_command_test, cgi_not_object_returns_error)
{
  fake_cu_cp_command_handler cu_cp;
  cell_lock_remote_command   cmd(cu_cp);

  nlohmann::json req;
  req["cgi"] = "not_an_object";

  error_type<std::string> result = cmd.execute(req);
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("'cgi' object value type"), std::string::npos) << result.error();
}

TEST(cu_cp_cell_lock_remote_command_test, missing_plmn_returns_error)
{
  fake_cu_cp_command_handler cu_cp;
  cell_lock_remote_command   cmd(cu_cp);

  nlohmann::json req;
  req["cgi"]["nci"] = uint64_t{6733824};

  error_type<std::string> result = cmd.execute(req);
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("'cgi.plmn'"), std::string::npos) << result.error();
}

TEST(cu_cp_cell_lock_remote_command_test, plmn_not_string_returns_error)
{
  fake_cu_cp_command_handler cu_cp;
  cell_lock_remote_command   cmd(cu_cp);

  nlohmann::json req;
  req["cgi"]["plmn"] = 12345;
  req["cgi"]["nci"]  = uint64_t{6733824};

  error_type<std::string> result = cmd.execute(req);
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("'cgi.plmn' object value type"), std::string::npos) << result.error();
}

TEST(cu_cp_cell_lock_remote_command_test, invalid_plmn_string_returns_error)
{
  fake_cu_cp_command_handler cu_cp;
  cell_lock_remote_command   cmd(cu_cp);

  nlohmann::json req;
  req["cgi"]["plmn"] = "not-a-plmn";
  req["cgi"]["nci"]  = uint64_t{6733824};

  error_type<std::string> result = cmd.execute(req);
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("Invalid PLMN"), std::string::npos) << result.error();
}

TEST(cu_cp_cell_lock_remote_command_test, missing_nci_returns_error)
{
  fake_cu_cp_command_handler cu_cp;
  cell_lock_remote_command   cmd(cu_cp);

  nlohmann::json req;
  req["cgi"]["plmn"] = "00101";

  error_type<std::string> result = cmd.execute(req);
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("'cgi.nci'"), std::string::npos) << result.error();
}

TEST(cu_cp_cell_lock_remote_command_test, nci_not_unsigned_returns_error)
{
  fake_cu_cp_command_handler cu_cp;
  cell_lock_remote_command   cmd(cu_cp);

  nlohmann::json req;
  req["cgi"]["plmn"] = "00101";
  req["cgi"]["nci"]  = -1;

  error_type<std::string> result = cmd.execute(req);
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("'cgi.nci' object value type"), std::string::npos) << result.error();
}

TEST(cu_cp_cell_lock_remote_command_test, invalid_nci_value_returns_error)
{
  fake_cu_cp_command_handler cu_cp;
  cell_lock_remote_command   cmd(cu_cp);

  // nr_cell_identity is a 36-bit field; a value > (1 << 36) should be rejected.
  nlohmann::json req;
  req["cgi"]["plmn"] = "00101";
  req["cgi"]["nci"]  = uint64_t{1} << 40;

  error_type<std::string> result = cmd.execute(req);
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("Invalid NR cell identity"), std::string::npos) << result.error();
}
