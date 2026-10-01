// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
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
  std::optional<nr_cell_global_id_t> last_bar_cgi;
  std::optional<bool>                last_bar_value;
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

  async_task<ocucp::cu_cp_cell_command_response> bar_cell(const nr_cell_global_id_t&, bool) override
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

  bool dispatch_bar_cell(const nr_cell_global_id_t& cgi, bool barred) override
  {
    last_bar_cgi   = cgi;
    last_bar_value = barred;
    return next_dispatch_result;
  }

  bool dispatch_activate_cell(const nr_cell_global_id_t& cgi) override
  {
    last_activate_cgi = cgi;
    return next_dispatch_result;
  }

  std::optional<ocucp::cu_cp_cell_state> get_cell_state(const nr_cell_global_id_t&) const override
  {
    // Not exercised by the WS command tests.
    return std::nullopt;
  }

  std::optional<ocucp::cu_cp_cell_state> dispatch_get_cell_state(const nr_cell_global_id_t& cgi) override
  {
    last_status_cgi = cgi;
    return next_cell_state;
  }

  std::optional<nr_cell_global_id_t>     last_status_cgi;
  std::optional<ocucp::cu_cp_cell_state> next_cell_state;
};

/// Fake cu_cp_command_handler whose get_cell_command_handler() returns the capturing handler.
/// Other accessors are not exercised by the WS command tests and abort if called.
class fake_cu_cp_command_handler : public ocucp::cu_cp_command_handler
{
public:
  capturing_cell_command_handler cell_cmd;

  ocucp::cu_cp_mobility_command_handler& get_mobility_command_handler() override { return mobility_cmd; }

  /// Fake cu_cp_mobility_command_handler recording the last trigger arguments.
  class capturing_mobility_command_handler : public ocucp::cu_cp_mobility_command_handler
  {
  public:
    struct ho_args {
      pci_t         serving_pci;
      rnti_t        rnti;
      pci_t         target_pci;
      plmn_identity plmn = plmn_identity::test_value();
      tac_t         tac;
    };
    struct cho_args {
      pci_t                                                serving_pci;
      rnti_t                                               rnti;
      std::vector<pci_t>                                   target_pcis;
      std::chrono::milliseconds                            timeout;
      std::optional<std::chrono::system_clock::time_point> t1_thres_override;
    };
    std::optional<ho_args>  last_ho;
    std::optional<cho_args> last_cho;
    /// What the CU-CP answers to a trigger.
    bool accept = true;

    bool trigger_handover(pci_t         source_pci,
                          rnti_t        rnti,
                          pci_t         target_pci,
                          plmn_identity target_plmn,
                          tac_t         target_tac) override
    {
      last_ho = ho_args{source_pci, rnti, target_pci, target_plmn, target_tac};
      return accept;
    }

    bool trigger_conditional_handover(
        pci_t                                                source_pci,
        rnti_t                                               rnti,
        span<const pci_t>                                    target_pcis,
        std::chrono::milliseconds                            timeout,
        std::optional<std::chrono::system_clock::time_point> t1_thres_override = std::nullopt) override
    {
      last_cho = cho_args{source_pci, rnti, {target_pcis.begin(), target_pcis.end()}, timeout, t1_thres_override};
      return accept;
    }
  };

  capturing_mobility_command_handler mobility_cmd;

  ocucp::cu_cp_ue_release_command_handler& get_ue_release_command_handler() override { std::abort(); }

  ocucp::cu_cp_ntn_meas_update_handler& get_ntn_meas_update_handler() override { std::abort(); }

  ocucp::cu_cp_cell_command_handler& get_cell_command_handler() override { return cell_cmd; }

  ocucp::cu_cp_mobility_config_handler& get_mobility_config_handler() override { return mobility_cfg; }

  /// Fake cu_cp_mobility_config_handler recording the last operation arguments and returning a
  /// configurable result.
  class capturing_mobility_config_handler : public ocucp::cu_cp_mobility_config_handler
  {
  public:
    std::optional<ocucp::serving_cell_meas_config>                                    last_cell_cfg;
    std::optional<nr_cell_identity>                                                   last_removed_cell;
    std::optional<std::pair<nr_cell_identity, nr_cell_identity>>                      last_neighbor;
    std::vector<ocucp::report_cfg_id_t>                                               last_report_cfg_ids;
    std::optional<std::pair<ocucp::report_cfg_id_t, ocucp::rrc_report_cfg_nr>>        last_report_cfg;
    std::optional<ocucp::report_cfg_id_t>                                             last_removed_report_cfg;
    std::optional<std::pair<nr_cell_identity, std::optional<ocucp::report_cfg_id_t>>> last_periodic_report;
    bool                                                                              next_result = true;

    bool update_mobility_cell(const ocucp::serving_cell_meas_config& cell_cfg) override
    {
      last_cell_cfg = cell_cfg;
      return next_result;
    }
    bool remove_mobility_cell(nr_cell_identity nci) override
    {
      last_removed_cell = nci;
      return next_result;
    }
    bool update_neighbor(nr_cell_identity                    serving_nci,
                         nr_cell_identity                    neighbor_nci,
                         std::vector<ocucp::report_cfg_id_t> report_cfg_ids) override
    {
      last_neighbor       = {serving_nci, neighbor_nci};
      last_report_cfg_ids = std::move(report_cfg_ids);
      return next_result;
    }
    bool remove_neighbor(nr_cell_identity serving_nci, nr_cell_identity neighbor_nci) override
    {
      last_neighbor = {serving_nci, neighbor_nci};
      return next_result;
    }
    bool update_report_config(ocucp::report_cfg_id_t report_cfg_id, const ocucp::rrc_report_cfg_nr& report_cfg) override
    {
      last_report_cfg = {report_cfg_id, report_cfg};
      return next_result;
    }
    bool remove_report_config(ocucp::report_cfg_id_t report_cfg_id) override
    {
      last_removed_report_cfg = report_cfg_id;
      return next_result;
    }
    bool set_periodic_report(nr_cell_identity nci, std::optional<ocucp::report_cfg_id_t> report_cfg_id) override
    {
      last_periodic_report = {nci, report_cfg_id};
      return next_result;
    }
  };

  capturing_mobility_config_handler mobility_cfg;
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

  expected<nlohmann::json, std::string> result = cmd.execute(make_valid_payload());

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

  expected<nlohmann::json, std::string> result = cmd.execute(make_valid_payload());

  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("CU-CP rejected cell_lock"), std::string::npos)
      << "Unexpected error message: " << result.error();
}

// ── cell_unlock happy path + dispatch behavior ──

TEST(cu_cp_cell_unlock_remote_command_test, valid_payload_dispatches_activate_with_the_provided_cgi)
{
  fake_cu_cp_command_handler cu_cp;
  cell_unlock_remote_command cmd(cu_cp);

  expected<nlohmann::json, std::string> result = cmd.execute(make_valid_payload());

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

  expected<nlohmann::json, std::string> result = cmd.execute(make_valid_payload());

  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("CU-CP rejected cell_unlock"), std::string::npos)
      << "Unexpected error message: " << result.error();
}

TEST(cu_cp_cell_bar_remote_command_test, valid_payload_dispatches_bar_with_the_provided_cgi)
{
  fake_cu_cp_command_handler cu_cp;
  cell_bar_remote_command    cmd(cu_cp);

  expected<nlohmann::json, std::string> result = cmd.execute(make_valid_payload());

  ASSERT_TRUE(result.has_value()) << "execute returned error: " << result.error();
  ASSERT_TRUE(cu_cp.cell_cmd.last_bar_cgi.has_value()) << "dispatch_bar_cell was not invoked";
  EXPECT_EQ(cu_cp.cell_cmd.last_bar_cgi->nci.value(), 6733824U);
  ASSERT_TRUE(cu_cp.cell_cmd.last_bar_value.has_value());
  EXPECT_TRUE(cu_cp.cell_cmd.last_bar_value.value()) << "cell_bar must dispatch barred=true";
  EXPECT_FALSE(cu_cp.cell_cmd.last_deactivate_cgi.has_value())
      << "dispatch_deactivate_cell should not be invoked from cell_bar";
}

TEST(cu_cp_cell_bar_remote_command_test, cu_cp_rejects_dispatch_returns_error)
{
  fake_cu_cp_command_handler cu_cp;
  cu_cp.cell_cmd.next_dispatch_result = false;
  cell_bar_remote_command cmd(cu_cp);

  expected<nlohmann::json, std::string> result = cmd.execute(make_valid_payload());

  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("CU-CP rejected cell_bar"), std::string::npos)
      << "Unexpected error message: " << result.error();
}

TEST(cu_cp_cell_unbar_remote_command_test, valid_payload_dispatches_unbar_with_the_provided_cgi)
{
  fake_cu_cp_command_handler cu_cp;
  cell_unbar_remote_command  cmd(cu_cp);

  expected<nlohmann::json, std::string> result = cmd.execute(make_valid_payload());

  ASSERT_TRUE(result.has_value()) << "execute returned error: " << result.error();
  ASSERT_TRUE(cu_cp.cell_cmd.last_bar_cgi.has_value()) << "dispatch_bar_cell was not invoked";
  EXPECT_EQ(cu_cp.cell_cmd.last_bar_cgi->nci.value(), 6733824U);
  ASSERT_TRUE(cu_cp.cell_cmd.last_bar_value.has_value());
  EXPECT_FALSE(cu_cp.cell_cmd.last_bar_value.value()) << "cell_unbar must dispatch barred=false";
}

TEST(cu_cp_cell_unbar_remote_command_test, cu_cp_rejects_dispatch_returns_error)
{
  fake_cu_cp_command_handler cu_cp;
  cu_cp.cell_cmd.next_dispatch_result = false;
  cell_unbar_remote_command cmd(cu_cp);

  expected<nlohmann::json, std::string> result = cmd.execute(make_valid_payload());

  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("CU-CP rejected cell_unbar"), std::string::npos)
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

  expected<nlohmann::json, std::string> result = cmd.execute(req);
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

  expected<nlohmann::json, std::string> result = cmd.execute(req);
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("'cgi' object value type"), std::string::npos) << result.error();
}

TEST(cu_cp_cell_lock_remote_command_test, missing_plmn_returns_error)
{
  fake_cu_cp_command_handler cu_cp;
  cell_lock_remote_command   cmd(cu_cp);

  nlohmann::json req;
  req["cgi"]["nci"] = uint64_t{6733824};

  expected<nlohmann::json, std::string> result = cmd.execute(req);
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

  expected<nlohmann::json, std::string> result = cmd.execute(req);
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

  expected<nlohmann::json, std::string> result = cmd.execute(req);
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("Invalid PLMN"), std::string::npos) << result.error();
}

TEST(cu_cp_cell_lock_remote_command_test, missing_nci_returns_error)
{
  fake_cu_cp_command_handler cu_cp;
  cell_lock_remote_command   cmd(cu_cp);

  nlohmann::json req;
  req["cgi"]["plmn"] = "00101";

  expected<nlohmann::json, std::string> result = cmd.execute(req);
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

  expected<nlohmann::json, std::string> result = cmd.execute(req);
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

  expected<nlohmann::json, std::string> result = cmd.execute(req);
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("Invalid NR cell identity"), std::string::npos) << result.error();
}

TEST(mobility_cell_set_remote_command_test, full_payload_is_parsed_into_serving_cell_config)
{
  fake_cu_cp_command_handler       cu_cp;
  mobility_cell_set_remote_command command(cu_cp);

  // Canonical payload of the mobility_cell_set command.
  const nlohmann::json req = nlohmann::json::parse(R"({
    "nci": 6577, "gnb_id_bit_length": 32, "pci": 3, "plmn": "00101", "tac": 7, "band": 78,
    "ssb_arfcn": 632628, "ssb_scs": 30, "ssb_period": 20, "ssb_offset": 0, "ssb_duration": 5
  })");

  ASSERT_TRUE(command.execute(req).has_value());
  ASSERT_TRUE(cu_cp.mobility_cfg.last_cell_cfg.has_value());
  const ocucp::serving_cell_meas_config& cfg = cu_cp.mobility_cfg.last_cell_cfg.value();
  EXPECT_EQ(cfg.nci, nr_cell_identity::create(6577).value());
  EXPECT_EQ(cfg.gnb_id_bit_length, 32U);
  EXPECT_EQ(cfg.pci, 3);
  EXPECT_EQ(cfg.tac, 7);
  EXPECT_EQ(cfg.band, nr_band::n78);
  EXPECT_EQ(cfg.ssb_arfcn->value(), 632628U);
  EXPECT_EQ(cfg.ssb_scs, subcarrier_spacing::kHz30);
  ASSERT_TRUE(cfg.ssb_mtc.has_value());
  EXPECT_EQ(static_cast<unsigned>(cfg.ssb_mtc->periodicity_and_offset.periodicity), 20U);
  EXPECT_EQ(cfg.ssb_mtc->periodicity_and_offset.offset, 0U);
  EXPECT_EQ(cfg.ssb_mtc->dur, 5U);
}

TEST(mobility_cell_set_remote_command_test, invalid_payloads_are_rejected)
{
  fake_cu_cp_command_handler       cu_cp;
  mobility_cell_set_remote_command command(cu_cp);

  // Missing nci.
  EXPECT_FALSE(command.execute(nlohmann::json::parse(R"({"gnb_id_bit_length": 32})")).has_value());
  // Missing gnb_id_bit_length.
  EXPECT_FALSE(command.execute(nlohmann::json::parse(R"({"nci": 6577})")).has_value());
  // gnb_id_bit_length out of range.
  EXPECT_FALSE(command.execute(nlohmann::json::parse(R"({"nci": 6577, "gnb_id_bit_length": 40})")).has_value());
  // Incomplete SSB timing trio.
  EXPECT_FALSE(command.execute(nlohmann::json::parse(R"({"nci": 6577, "gnb_id_bit_length": 32, "ssb_period": 20})"))
                   .has_value());
  // Invalid SSB subcarrier spacing.
  EXPECT_FALSE(
      command.execute(nlohmann::json::parse(R"({"nci": 6577, "gnb_id_bit_length": 32, "ssb_scs": 25})")).has_value());
  EXPECT_FALSE(cu_cp.mobility_cfg.last_cell_cfg.has_value());

  // Refusal from the CU-CP crosses back as an error.
  cu_cp.mobility_cfg.next_result = false;
  EXPECT_FALSE(command.execute(nlohmann::json::parse(R"({"nci": 6577, "gnb_id_bit_length": 32})")).has_value());
}

TEST(mobility_cell_remove_remote_command_test, payload_is_parsed_and_refusal_reported)
{
  fake_cu_cp_command_handler          cu_cp;
  mobility_cell_remove_remote_command command(cu_cp);

  ASSERT_TRUE(command.execute(nlohmann::json::parse(R"({"nci": 6577})")).has_value());
  EXPECT_EQ(cu_cp.mobility_cfg.last_removed_cell, nr_cell_identity::create(6577).value());

  EXPECT_FALSE(command.execute(nlohmann::json::object()).has_value());
  cu_cp.mobility_cfg.next_result = false;
  EXPECT_FALSE(command.execute(nlohmann::json::parse(R"({"nci": 6577})")).has_value());
}

TEST(neighbor_add_remote_command_test, payload_is_parsed_into_relation)
{
  fake_cu_cp_command_handler  cu_cp;
  neighbor_add_remote_command command(cu_cp);

  // Canonical payload of the mobility_neighbor_add command.
  const nlohmann::json req = nlohmann::json::parse(R"({"nci": 6576, "neighbor_nci": 6577, "report_configs": [2, 3]})");

  ASSERT_TRUE(command.execute(req).has_value());
  ASSERT_TRUE(cu_cp.mobility_cfg.last_neighbor.has_value());
  EXPECT_EQ(cu_cp.mobility_cfg.last_neighbor->first, nr_cell_identity::create(6576).value());
  EXPECT_EQ(cu_cp.mobility_cfg.last_neighbor->second, nr_cell_identity::create(6577).value());
  ASSERT_EQ(cu_cp.mobility_cfg.last_report_cfg_ids.size(), 2U);
  EXPECT_EQ(cu_cp.mobility_cfg.last_report_cfg_ids[0], ocucp::uint_to_report_cfg_id(2));
  EXPECT_EQ(cu_cp.mobility_cfg.last_report_cfg_ids[1], ocucp::uint_to_report_cfg_id(3));
}

TEST(neighbor_add_remote_command_test, invalid_payloads_are_rejected)
{
  fake_cu_cp_command_handler  cu_cp;
  neighbor_add_remote_command command(cu_cp);

  // Missing neighbor_nci.
  EXPECT_FALSE(command.execute(nlohmann::json::parse(R"({"nci": 6576, "report_configs": [2]})")).has_value());
  // Missing report_configs.
  EXPECT_FALSE(command.execute(nlohmann::json::parse(R"({"nci": 6576, "neighbor_nci": 6577})")).has_value());
  // Empty report_configs.
  EXPECT_FALSE(command.execute(nlohmann::json::parse(R"({"nci": 6576, "neighbor_nci": 6577, "report_configs": []})"))
                   .has_value());
  // Report config id out of range (64 is the invalid sentinel).
  EXPECT_FALSE(command.execute(nlohmann::json::parse(R"({"nci": 6576, "neighbor_nci": 6577, "report_configs": [64]})"))
                   .has_value());
}

TEST(neighbor_remove_remote_command_test, payload_is_parsed)
{
  fake_cu_cp_command_handler     cu_cp;
  neighbor_remove_remote_command command(cu_cp);

  ASSERT_TRUE(command.execute(nlohmann::json::parse(R"({"nci": 6576, "neighbor_nci": 6577})")).has_value());
  ASSERT_TRUE(cu_cp.mobility_cfg.last_neighbor.has_value());
  EXPECT_EQ(cu_cp.mobility_cfg.last_neighbor->first, nr_cell_identity::create(6576).value());
  EXPECT_EQ(cu_cp.mobility_cfg.last_neighbor->second, nr_cell_identity::create(6577).value());

  EXPECT_FALSE(command.execute(nlohmann::json::parse(R"({"nci": 6576})")).has_value());
}

TEST(report_config_set_remote_command_test, event_triggered_payload_is_validated_and_translated)
{
  fake_cu_cp_command_handler       cu_cp;
  report_config_set_remote_command command(cu_cp);

  // Canonical payload of an A3 event-triggered report config.
  const nlohmann::json req = nlohmann::json::parse(R"({
    "report_cfg_id": 5, "report_type": "event_triggered", "event_triggered_report_type": "a3",
    "meas_trigger_quantity": "rsrp", "meas_trigger_quantity_offset_db": 3, "hysteresis_db": 0,
    "time_to_trigger_ms": 100, "report_interval_ms": 1024, "t312": 200
  })");

  ASSERT_TRUE(command.execute(req).has_value());
  ASSERT_TRUE(cu_cp.mobility_cfg.last_report_cfg.has_value());
  EXPECT_EQ(cu_cp.mobility_cfg.last_report_cfg->first, ocucp::uint_to_report_cfg_id(5));
  ASSERT_TRUE(std::holds_alternative<ocucp::rrc_event_trigger_cfg>(cu_cp.mobility_cfg.last_report_cfg->second));
  const auto& event_cfg = std::get<ocucp::rrc_event_trigger_cfg>(cu_cp.mobility_cfg.last_report_cfg->second);
  EXPECT_EQ(event_cfg.event_id.id, ocucp::rrc_event_id::event_id_t::a3);
  EXPECT_EQ(event_cfg.report_interv, 1024U);
}

TEST(report_config_set_remote_command_test, periodical_payload_is_validated_and_translated)
{
  fake_cu_cp_command_handler       cu_cp;
  report_config_set_remote_command command(cu_cp);

  // Canonical payload of a periodical report config.
  const nlohmann::json req = nlohmann::json::parse(R"({
    "report_cfg_id": 1, "report_type": "periodical", "report_interval_ms": 1024,
    "periodic_ho_rsrp_offset_db": 5
  })");

  ASSERT_TRUE(command.execute(req).has_value());
  ASSERT_TRUE(cu_cp.mobility_cfg.last_report_cfg.has_value());
  ASSERT_TRUE(std::holds_alternative<ocucp::rrc_periodical_report_cfg>(cu_cp.mobility_cfg.last_report_cfg->second));
  const auto& periodical_cfg = std::get<ocucp::rrc_periodical_report_cfg>(cu_cp.mobility_cfg.last_report_cfg->second);
  EXPECT_EQ(periodical_cfg.report_interv, 1024U);
  EXPECT_EQ(periodical_cfg.periodic_ho_rsrp_offset, 5);
}

TEST(report_config_set_remote_command_test, invalid_payloads_are_rejected)
{
  fake_cu_cp_command_handler       cu_cp;
  report_config_set_remote_command command(cu_cp);

  // Missing report_type.
  EXPECT_FALSE(command.execute(nlohmann::json::parse(R"({"report_cfg_id": 5})")).has_value());
  // Distance/time based events are not supported through the command.
  EXPECT_FALSE(command
                   .execute(nlohmann::json::parse(
                       R"({"report_cfg_id": 5, "report_type": "cond_trigger", "event_triggered_report_type": "d1"})"))
                   .has_value());
  // Event config without its mandatory parameters fails validation.
  EXPECT_FALSE(command
                   .execute(nlohmann::json::parse(
                       R"({"report_cfg_id": 5, "report_type": "event_triggered",
                           "event_triggered_report_type": "a3", "report_interval_ms": 1024})"))
                   .has_value());
  // Non-member time to trigger value.
  EXPECT_FALSE(command
                   .execute(nlohmann::json::parse(
                       R"({"report_cfg_id": 5, "report_type": "event_triggered",
                           "event_triggered_report_type": "a3", "meas_trigger_quantity": "rsrp",
                           "meas_trigger_quantity_offset_db": 3, "hysteresis_db": 0,
                           "time_to_trigger_ms": 123, "report_interval_ms": 1024})"))
                   .has_value());
  // Missing report interval for a periodical config.
  EXPECT_FALSE(
      command.execute(nlohmann::json::parse(R"({"report_cfg_id": 5, "report_type": "periodical"})")).has_value());
  EXPECT_FALSE(cu_cp.mobility_cfg.last_report_cfg.has_value());
}

TEST(report_config_remove_remote_command_test, payload_is_parsed)
{
  fake_cu_cp_command_handler          cu_cp;
  report_config_remove_remote_command command(cu_cp);

  ASSERT_TRUE(command.execute(nlohmann::json::parse(R"({"report_cfg_id": 5})")).has_value());
  EXPECT_EQ(cu_cp.mobility_cfg.last_removed_report_cfg, ocucp::uint_to_report_cfg_id(5));

  EXPECT_FALSE(command.execute(nlohmann::json::parse(R"({"report_cfg_id": 0})")).has_value());
  EXPECT_FALSE(command.execute(nlohmann::json::object()).has_value());
}

TEST(periodic_report_set_remote_command_test, payload_with_and_without_id_is_parsed)
{
  fake_cu_cp_command_handler         cu_cp;
  periodic_report_set_remote_command command(cu_cp);

  // Set the periodical report of a cell.
  ASSERT_TRUE(command.execute(nlohmann::json::parse(R"({"nci": 6576, "report_cfg_id": 1})")).has_value());
  ASSERT_TRUE(cu_cp.mobility_cfg.last_periodic_report.has_value());
  EXPECT_EQ(cu_cp.mobility_cfg.last_periodic_report->first, nr_cell_identity::create(6576).value());
  EXPECT_EQ(cu_cp.mobility_cfg.last_periodic_report->second, ocucp::uint_to_report_cfg_id(1));

  // Omitting report_cfg_id clears it.
  ASSERT_TRUE(command.execute(nlohmann::json::parse(R"({"nci": 6576})")).has_value());
  EXPECT_FALSE(cu_cp.mobility_cfg.last_periodic_report->second.has_value());

  EXPECT_FALSE(command.execute(nlohmann::json::object()).has_value());
}

TEST(trigger_handover_remote_command_test, payload_is_parsed_into_trigger_arguments)
{
  fake_cu_cp_command_handler      cu_cp;
  trigger_handover_remote_command command(cu_cp);

  // Canonical payload of the trigger_handover command.
  const nlohmann::json req =
      nlohmann::json::parse(R"({"serving_pci": 1, "rnti": 17921, "target_pci": 2, "plmn": "00101", "tac": 7})");

  ASSERT_TRUE(command.execute(req).has_value());
  ASSERT_TRUE(cu_cp.mobility_cmd.last_ho.has_value());
  EXPECT_EQ(cu_cp.mobility_cmd.last_ho->serving_pci, 1);
  EXPECT_EQ(cu_cp.mobility_cmd.last_ho->rnti, to_rnti(17921));
  EXPECT_EQ(cu_cp.mobility_cmd.last_ho->target_pci, 2);
  EXPECT_EQ(cu_cp.mobility_cmd.last_ho->plmn, plmn_identity::parse("00101").value());
  EXPECT_EQ(cu_cp.mobility_cmd.last_ho->tac, 7);
}

TEST(trigger_handover_remote_command_test, cu_cp_rejects_trigger_returns_error)
{
  fake_cu_cp_command_handler      cu_cp;
  trigger_handover_remote_command command(cu_cp);
  cu_cp.mobility_cmd.accept = false;

  auto result = command.execute(
      nlohmann::json::parse(R"({"serving_pci": 1, "rnti": 17921, "target_pci": 2, "plmn": "00101", "tac": 7})"));
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("rejected"), std::string::npos);
  // The trigger reached the CU-CP; the refusal is the CU-CP's own.
  EXPECT_TRUE(cu_cp.mobility_cmd.last_ho.has_value());
}

TEST(trigger_handover_remote_command_test, invalid_payloads_are_rejected)
{
  fake_cu_cp_command_handler      cu_cp;
  trigger_handover_remote_command command(cu_cp);

  // Missing target_pci.
  EXPECT_FALSE(command.execute(nlohmann::json::parse(R"({"serving_pci": 1, "rnti": 17921, "plmn": "00101", "tac": 7})"))
                   .has_value());
  // PCI out of range.
  EXPECT_FALSE(command
                   .execute(nlohmann::json::parse(
                       R"({"serving_pci": 1, "rnti": 17921, "target_pci": 1100, "plmn": "00101", "tac": 7})"))
                   .has_value());
  // Reserved TAC.
  EXPECT_FALSE(command
                   .execute(nlohmann::json::parse(
                       R"({"serving_pci": 1, "rnti": 17921, "target_pci": 2, "plmn": "00101", "tac": 0})"))
                   .has_value());
  EXPECT_FALSE(cu_cp.mobility_cmd.last_ho.has_value());
}

TEST(trigger_conditional_handover_remote_command_test, payload_is_parsed_into_trigger_arguments)
{
  fake_cu_cp_command_handler                  cu_cp;
  trigger_conditional_handover_remote_command command(cu_cp, std::chrono::milliseconds{10000});

  // Canonical payload of the trigger_conditional_handover command.
  const nlohmann::json req = nlohmann::json::parse(
      R"({"serving_pci": 1, "rnti": 17921, "target_pcis": [2, 3], "timeout_ms": 30000,
          "t1_thres": "2026-03-01T10:00:00"})");

  ASSERT_TRUE(command.execute(req).has_value());
  ASSERT_TRUE(cu_cp.mobility_cmd.last_cho.has_value());
  EXPECT_EQ(cu_cp.mobility_cmd.last_cho->serving_pci, 1);
  EXPECT_EQ(cu_cp.mobility_cmd.last_cho->rnti, to_rnti(17921));
  ASSERT_EQ(cu_cp.mobility_cmd.last_cho->target_pcis.size(), 2U);
  EXPECT_EQ(cu_cp.mobility_cmd.last_cho->target_pcis[0], 2);
  EXPECT_EQ(cu_cp.mobility_cmd.last_cho->target_pcis[1], 3);
  EXPECT_EQ(cu_cp.mobility_cmd.last_cho->timeout, std::chrono::milliseconds{30000});
  EXPECT_TRUE(cu_cp.mobility_cmd.last_cho->t1_thres_override.has_value());

  // Omitting timeout_ms falls back to the configured default.
  ASSERT_TRUE(
      command.execute(nlohmann::json::parse(R"({"serving_pci": 1, "rnti": 17921, "target_pcis": [2]})")).has_value());
  EXPECT_EQ(cu_cp.mobility_cmd.last_cho->timeout, std::chrono::milliseconds{10000});
  EXPECT_FALSE(cu_cp.mobility_cmd.last_cho->t1_thres_override.has_value());
}

TEST(trigger_conditional_handover_remote_command_test, t1_thres_accepts_unix_ms_as_number_or_string)
{
  fake_cu_cp_command_handler                  cu_cp;
  trigger_conditional_handover_remote_command command(cu_cp, std::chrono::milliseconds{10000});

  ASSERT_TRUE(command
                  .execute(nlohmann::json::parse(
                      R"({"serving_pci": 1, "rnti": 17921, "target_pcis": [2], "t1_thres": 1756382400000})"))
                  .has_value());
  ASSERT_TRUE(cu_cp.mobility_cmd.last_cho->t1_thres_override.has_value());
  const auto from_number = cu_cp.mobility_cmd.last_cho->t1_thres_override.value();
  EXPECT_EQ(from_number, std::chrono::system_clock::time_point{std::chrono::milliseconds{1756382400000}});

  ASSERT_TRUE(command
                  .execute(nlohmann::json::parse(
                      R"({"serving_pci": 1, "rnti": 17921, "target_pcis": [2], "t1_thres": "1756382400000"})"))
                  .has_value());
  ASSERT_TRUE(cu_cp.mobility_cmd.last_cho->t1_thres_override.has_value());
  EXPECT_EQ(cu_cp.mobility_cmd.last_cho->t1_thres_override.value(), from_number);
}

TEST(trigger_conditional_handover_remote_command_test, cu_cp_rejects_trigger_returns_error)
{
  fake_cu_cp_command_handler                  cu_cp;
  trigger_conditional_handover_remote_command command(cu_cp, std::chrono::milliseconds{10000});
  cu_cp.mobility_cmd.accept = false;

  auto result = command.execute(nlohmann::json::parse(R"({"serving_pci": 1, "rnti": 17921, "target_pcis": [2]})"));
  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("rejected"), std::string::npos);
  EXPECT_TRUE(cu_cp.mobility_cmd.last_cho.has_value());
}

TEST(trigger_conditional_handover_remote_command_test, invalid_payloads_are_rejected)
{
  fake_cu_cp_command_handler                  cu_cp;
  trigger_conditional_handover_remote_command command(cu_cp, std::chrono::milliseconds{10000});

  // Missing target_pcis.
  EXPECT_FALSE(command.execute(nlohmann::json::parse(R"({"serving_pci": 1, "rnti": 17921})")).has_value());
  // Empty target list.
  EXPECT_FALSE(
      command.execute(nlohmann::json::parse(R"({"serving_pci": 1, "rnti": 17921, "target_pcis": []})")).has_value());
  // More than 8 candidates.
  EXPECT_FALSE(
      command.execute(nlohmann::json::parse(R"({"serving_pci": 1, "rnti": 17921, "target_pcis": [1,2,3,4,5,6,7,8,9]})"))
          .has_value());
  // Invalid t1_thres string.
  EXPECT_FALSE(command
                   .execute(nlohmann::json::parse(
                       R"({"serving_pci": 1, "rnti": 17921, "target_pcis": [2], "t1_thres": "not-a-date"})"))
                   .has_value());
  // t1_thres of a type that is neither an integer nor a string.
  EXPECT_FALSE(
      command
          .execute(nlohmann::json::parse(R"({"serving_pci": 1, "rnti": 17921, "target_pcis": [2], "t1_thres": 1.5})"))
          .has_value());
  EXPECT_FALSE(cu_cp.mobility_cmd.last_cho.has_value());
}

// ── cell_status query ──

TEST(cu_cp_cell_status_remote_command_test, valid_payload_returns_the_recorded_state)
{
  fake_cu_cp_command_handler cu_cp;
  cu_cp.cell_cmd.next_cell_state = ocucp::cu_cp_cell_state{
      ocucp::cell_admin_state::shutting_down, ocucp::cell_operational_state::enabled, /* barred = */ true};
  cell_status_remote_command cmd(cu_cp);

  expected<nlohmann::json, std::string> result = cmd.execute(make_valid_payload());

  ASSERT_TRUE(result.has_value()) << "execute returned error: " << result.error();
  ASSERT_TRUE(cu_cp.cell_cmd.last_status_cgi.has_value()) << "dispatch_get_cell_state was not invoked";
  EXPECT_EQ(cu_cp.cell_cmd.last_status_cgi->nci.value(), 6733824U);
  EXPECT_EQ(result.value()["admin_state"], "shutting_down");
  EXPECT_EQ(result.value()["operational_state"], "enabled");
  EXPECT_EQ(result.value()["cell_barred"], true);
}

TEST(cu_cp_cell_status_remote_command_test, unknown_cell_returns_error)
{
  fake_cu_cp_command_handler cu_cp;
  cu_cp.cell_cmd.next_cell_state = std::nullopt;
  cell_status_remote_command cmd(cu_cp);

  expected<nlohmann::json, std::string> result = cmd.execute(make_valid_payload());

  ASSERT_FALSE(result.has_value());
  EXPECT_NE(result.error().find("no cell matching"), std::string::npos)
      << "Unexpected error message: " << result.error();
}

TEST(cu_cp_cell_status_remote_command_test, missing_cgi_returns_parse_error)
{
  fake_cu_cp_command_handler cu_cp;
  cell_status_remote_command cmd(cu_cp);

  expected<nlohmann::json, std::string> result = cmd.execute(nlohmann::json::object());

  ASSERT_FALSE(result.has_value());
  EXPECT_FALSE(cu_cp.cell_cmd.last_status_cgi.has_value()) << "a parse error must not reach the CU-CP";
}
