// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/flexible_o_du/o_du_high/du_high/commands/du_high_remote_commands.h"
#include "ocudu/du/du_high/du_manager/du_configurator.h"
#include "ocudu/support/async/async_task.h"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace ocudu;

namespace {

/// Dummy du_configurator that records the last request received.
class capturing_du_configurator : public odu::du_configurator
{
public:
  std::optional<odu::du_param_config_request> last_req;
  bool                                        next_response_success = true;

  async_task<odu::du_mac_sched_control_config_response>
  configure_ue_mac_scheduler(const odu::du_mac_sched_control_config&) override
  {
    return launch_async([](coro_context<async_task<odu::du_mac_sched_control_config_response>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(odu::du_mac_sched_control_config_response{});
    });
  }

  odu::du_param_config_response handle_sync_operator_config(const odu::du_param_config_request& req) override
  {
    last_req = req;
    return odu::du_param_config_response{next_response_success};
  }

  async_task<odu::du_param_config_response> handle_operator_config(const odu::du_param_config_request&,
                                                                   task_executor&) override
  {
    return launch_async([](coro_context<async_task<odu::du_param_config_response>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN(odu::du_param_config_response{});
    });
  }

  void handle_ntn_param_update(const odu::du_ntn_param_update_request&) override {}
};

/// Build a minimal valid cell skeleton (plmn, nci) with an empty sib section that the caller fills in.
nlohmann::json make_cell_skeleton()
{
  nlohmann::json cell;
  cell["plmn"] = "001001";
  cell["nci"]  = uint64_t{6733824}; // must be unsigned — adapter checks is_number_unsigned()
  return cell;
}

/// Wrap a cell object into the top-level {"cells": [...]} payload.
nlohmann::json wrap(nlohmann::json cell)
{
  nlohmann::json req;
  req["cells"] = nlohmann::json::array({std::move(cell)});
  return req;
}

/// Build a SIB2 content object with the three bounded_integer fields the boundary tests vary,
/// and stable defaults for the rest.
nlohmann::json sib2_content_with(int q_rx_lev_min, int thresh_low_p, int reselect_prio)
{
  return {{"q_hyst_db", 4},
          {"thresh_serving_low_p", thresh_low_p},
          {"cell_reselection_priority", reselect_prio},
          {"q_rx_lev_min", q_rx_lev_min},
          {"s_intra_search_p", 31},
          {"t_reselection_nr", 1}};
}

} // namespace

// Happy path — valid SIB2/SIB3/SIB4 payloads produce the right variant.

TEST(sib_update_remote_command, valid_sib2_request_builds_sib2_variant)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib2"},
                 {"content",
                  {{"q_hyst_db", 4},
                   {"thresh_serving_low_p", 14},
                   {"cell_reselection_priority", 4},
                   {"q_rx_lev_min", -70},
                   {"s_intra_search_p", 31},
                   {"t_reselection_nr", 1}}}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_TRUE(res.has_value()) << res.error();

  ASSERT_TRUE(mock.last_req.has_value());
  ASSERT_EQ(mock.last_req->cells.size(), 1U);
  const auto& cell_cfg = mock.last_req->cells[0];
  ASSERT_TRUE(cell_cfg.nr_cgi.has_value());
  ASSERT_TRUE(cell_cfg.new_sys_info.has_value());
  ASSERT_TRUE(std::holds_alternative<sib2_info>(*cell_cfg.new_sys_info));

  const auto& sib2 = std::get<sib2_info>(*cell_cfg.new_sys_info);
  EXPECT_EQ(static_cast<int>(sib2.q_hyst), 4);
  EXPECT_EQ(sib2.thresh_serving_low_p.value(), 14);
  EXPECT_EQ(sib2.cell_reselection_priority.value(), 4);
  EXPECT_EQ(sib2.q_rx_lev_min.value(), -70);
  EXPECT_EQ(sib2.s_intra_search_p.value(), 31);
  EXPECT_EQ(sib2.t_reselection_nr.value(), 1);
}

TEST(sib_update_remote_command, valid_sib3_request_builds_neighbor_and_excluded_lists)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib3"},
                 {"content",
                  {{"intra_freq_neigh_cell_list",
                    nlohmann::json::array({{{"pci", 47}, {"q_offset_cell", 0}}, {{"pci", 48}, {"q_offset_cell", -2}}})},
                   {"intra_freq_excluded_cell_list", nlohmann::json::array({{{"pci_start", 100}, {"range", 4}}})}}}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_TRUE(res.has_value()) << res.error();

  ASSERT_TRUE(mock.last_req.has_value());
  const auto& cell_cfg = mock.last_req->cells[0];
  ASSERT_TRUE(cell_cfg.new_sys_info.has_value());
  ASSERT_TRUE(std::holds_alternative<sib3_info>(*cell_cfg.new_sys_info));

  const auto& sib3 = std::get<sib3_info>(*cell_cfg.new_sys_info);
  ASSERT_EQ(sib3.intra_freq_neigh_cell_list.size(), 2U);
  EXPECT_EQ(sib3.intra_freq_neigh_cell_list[0].pci, 47);
  EXPECT_EQ(static_cast<int>(sib3.intra_freq_neigh_cell_list[0].q_offset_cell), 0);
  EXPECT_EQ(sib3.intra_freq_neigh_cell_list[1].pci, 48);
  EXPECT_EQ(static_cast<int>(sib3.intra_freq_neigh_cell_list[1].q_offset_cell), -2);

  ASSERT_EQ(sib3.intra_freq_excluded_cell_list.size(), 1U);
  EXPECT_EQ(sib3.intra_freq_excluded_cell_list[0].start, 100);
  EXPECT_EQ(static_cast<int>(sib3.intra_freq_excluded_cell_list[0].size), 4);
}

TEST(sib_update_remote_command, valid_sib4_request_builds_inter_freq_carrier_list)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib4"},
                 {"content",
                  {{"inter_freq_carrier_freq_list",
                    nlohmann::json::array({{{"arfcn", 649632},
                                            {"ssb_scs", 30},
                                            {"derive_ssb_index_from_cell", true},
                                            {"q_rx_lev_min", -70},
                                            {"thresh_x_high_p", 16},
                                            {"thresh_x_low_p", 4},
                                            {"q_offset_freq", 0}}})}}}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_TRUE(res.has_value()) << res.error();

  const auto& sib4 = std::get<sib4_info>(*mock.last_req->cells[0].new_sys_info);
  ASSERT_EQ(sib4.inter_freq_carrier_freq_list.size(), 1U);
  const auto& carrier = sib4.inter_freq_carrier_freq_list[0];
  EXPECT_EQ(carrier.arfcn, 649632U);
  EXPECT_EQ(carrier.ssb_scs, subcarrier_spacing::kHz30);
  EXPECT_TRUE(carrier.derive_ssb_index_from_cell);
  EXPECT_EQ(carrier.q_rx_lev_min.value(), -70);
  EXPECT_EQ(carrier.thresh_x_high_p.value(), 16);
  EXPECT_EQ(carrier.thresh_x_low_p.value(), 4);
}

TEST(sib_update_remote_command, sib4_omitting_optional_q_offset_freq_defaults_to_db0)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib4"},
                 {"content",
                  {{"inter_freq_carrier_freq_list",
                    nlohmann::json::array({{{"arfcn", 649632},
                                            {"ssb_scs", 30},
                                            {"derive_ssb_index_from_cell", false},
                                            {"q_rx_lev_min", -68},
                                            {"thresh_x_high_p", 18},
                                            {"thresh_x_low_p", 6}}})}}}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_TRUE(res.has_value()) << res.error();

  const auto& sib4 = std::get<sib4_info>(*mock.last_req->cells[0].new_sys_info);
  EXPECT_EQ(sib4.inter_freq_carrier_freq_list[0].q_offset_freq, q_offset_range_t::db0);
}

// Top-level validation errors.

TEST(sib_update_remote_command, missing_cells_returns_error)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto res = cmd.execute(nlohmann::json::object());
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("cells"), std::string::npos) << "actual: " << res.error();
  EXPECT_FALSE(mock.last_req.has_value());
}

TEST(sib_update_remote_command, cells_not_array_returns_error)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  nlohmann::json req;
  req["cells"] = "not an array";

  auto res = cmd.execute(req);
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("array"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, empty_cells_array_returns_error)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  nlohmann::json req;
  req["cells"] = nlohmann::json::array();

  auto res = cmd.execute(req);
  ASSERT_FALSE(res.has_value());
}

// Cell-level validation errors.

TEST(sib_update_remote_command, missing_plmn_returns_error)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  nlohmann::json cell;
  cell["nci"] = uint64_t{6733824};
  cell["sib"] = {{"type", "sib2"}, {"content", nlohmann::json::object()}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("plmn"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, missing_nci_returns_error)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  nlohmann::json cell;
  cell["plmn"] = "001001";
  cell["sib"]  = {{"type", "sib2"}, {"content", nlohmann::json::object()}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("nci"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, invalid_plmn_string_returns_error)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell    = make_cell_skeleton();
  cell["plmn"] = "BADPLMN";
  cell["sib"]  = {{"type", "sib2"}, {"content", nlohmann::json::object()}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("PLMN"), std::string::npos) << "actual: " << res.error();
}

// SIB-level validation errors.

TEST(sib_update_remote_command, missing_sib_object_returns_error)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto res = cmd.execute(wrap(make_cell_skeleton()));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("sib"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, unknown_sib_type_returns_error)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib99"}, {"content", nlohmann::json::object()}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("sib99"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, sib_type_not_string_returns_error)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", 2}, {"content", nlohmann::json::object()}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("string"), std::string::npos) << "actual: " << res.error();
}

// Content-level field validation.

TEST(sib_update_remote_command, sib2_invalid_q_hyst_value_returns_error)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib2"},
                 {"content",
                  {{"q_hyst_db", 7}, // 7 is not a valid q_hyst_db value (0-6,8,10,...,24)
                   {"thresh_serving_low_p", 14},
                   {"cell_reselection_priority", 4},
                   {"q_rx_lev_min", -70},
                   {"s_intra_search_p", 31},
                   {"t_reselection_nr", 1}}}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("q_hyst_db"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, sib2_q_rx_lev_min_out_of_range_returns_error)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib2"},
                 {"content",
                  {{"q_hyst_db", 4},
                   {"thresh_serving_low_p", 14},
                   {"cell_reselection_priority", 4},
                   {"q_rx_lev_min", -100}, // below allowed range [-70, -22]
                   {"s_intra_search_p", 31},
                   {"t_reselection_nr", 1}}}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("q_rx_lev_min"), std::string::npos) << "actual: " << res.error();
  EXPECT_NE(res.error().find("out of range"), std::string::npos) << "actual: " << res.error();
}

// q_rx_lev_min is bounded [-70, -22] and stored as int8_t. JSON value 200 must
// be rejected even though static_cast<int8_t>(200) = -56 falls inside the bound.
// Guards against narrowing bugs where val.get<int8_t>() would silently wrap.
TEST(sib_update_remote_command, sib2_q_rx_lev_min_positive_overflow_rejected)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib2"},
                 {"content",
                  {{"q_hyst_db", 4},
                   {"thresh_serving_low_p", 14},
                   {"cell_reselection_priority", 4},
                   {"q_rx_lev_min", 200}, // above int8_t bound; static_cast wraps to -56
                   {"s_intra_search_p", 31},
                   {"t_reselection_nr", 1}}}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("q_rx_lev_min"), std::string::npos) << "actual: " << res.error();
  EXPECT_NE(res.error().find("out of range"), std::string::npos) << "actual: " << res.error();
}

// thresh_serving_low_p is bounded [0, 31] and stored as uint8_t. JSON 256 must
// be rejected even though static_cast<uint8_t>(256) = 0 falls inside the bound.
// Guards against narrowing bugs where val.get<uint8_t>() would silently wrap.
TEST(sib_update_remote_command, sib2_thresh_serving_low_p_overshoot_rejected)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib2"},
                 {"content",
                  {{"q_hyst_db", 4},
                   {"thresh_serving_low_p", 256}, // above uint8_t bound; static_cast wraps to 0
                   {"cell_reselection_priority", 4},
                   {"q_rx_lev_min", -70},
                   {"s_intra_search_p", 31},
                   {"t_reselection_nr", 1}}}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("thresh_serving_low_p"), std::string::npos) << "actual: " << res.error();
  EXPECT_NE(res.error().find("out of range"), std::string::npos) << "actual: " << res.error();
}

// Boundary tests for SIB2 bounded_integer fields: exact MIN, exact MAX, just
// outside on each side. Make sure the validation is inclusive on both ends.

TEST(sib_update_remote_command, sib2_q_rx_lev_min_at_min_boundary_accepted)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib2"}, {"content", sib2_content_with(-70, 14, 4)}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_TRUE(res.has_value()) << res.error();
}

TEST(sib_update_remote_command, sib2_q_rx_lev_min_at_max_boundary_accepted)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib2"}, {"content", sib2_content_with(-22, 14, 4)}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_TRUE(res.has_value()) << res.error();
}

TEST(sib_update_remote_command, sib2_q_rx_lev_min_just_below_min_rejected)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib2"}, {"content", sib2_content_with(-71, 14, 4)}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("q_rx_lev_min"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, sib2_q_rx_lev_min_just_above_max_rejected)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib2"}, {"content", sib2_content_with(-21, 14, 4)}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("q_rx_lev_min"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, sib2_thresh_serving_low_p_at_min_boundary_accepted)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib2"}, {"content", sib2_content_with(-70, 0, 4)}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_TRUE(res.has_value()) << res.error();
}

TEST(sib_update_remote_command, sib2_thresh_serving_low_p_at_max_boundary_accepted)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib2"}, {"content", sib2_content_with(-70, 31, 4)}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_TRUE(res.has_value()) << res.error();
}

TEST(sib_update_remote_command, sib2_thresh_serving_low_p_just_above_max_rejected)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib2"}, {"content", sib2_content_with(-70, 32, 4)}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("thresh_serving_low_p"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, sib2_cell_reselection_priority_at_max_boundary_accepted)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib2"}, {"content", sib2_content_with(-70, 14, 7)}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_TRUE(res.has_value()) << res.error();
}

TEST(sib_update_remote_command, sib2_cell_reselection_priority_just_above_max_rejected)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib2"}, {"content", sib2_content_with(-70, 14, 8)}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("cell_reselection_priority"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, sib2_missing_mandatory_field_returns_error)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib2"},
                 {"content",
                  {// q_hyst_db omitted
                   {"thresh_serving_low_p", 14},
                   {"cell_reselection_priority", 4},
                   {"q_rx_lev_min", -70},
                   {"s_intra_search_p", 31},
                   {"t_reselection_nr", 1}}}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("q_hyst_db"), std::string::npos) << "actual: " << res.error();
  EXPECT_NE(res.error().find("missing"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, sib3_pci_out_of_range_returns_error)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib3"},
                 {"content",
                  {{"intra_freq_neigh_cell_list",
                    nlohmann::json::array({{{"pci", 2000}, {"q_offset_cell", 0}}})}}}}; // PCI max is 1007

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("pci"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, sib3_invalid_q_offset_cell_value_returns_error)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib3"},
                 {"content",
                  {{"intra_freq_neigh_cell_list",
                    nlohmann::json::array({{{"pci", 47}, {"q_offset_cell", 7}}})}}}}; // 7 invalid; valid ±(1..6,8..24)

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("q_offset_cell"), std::string::npos) << "actual: " << res.error();
}

// Narrowing-wrap regression tests for the enum-mapped helpers (parse_q_hyst_db,
// parse_q_offset_range, parse_sib3 range parsing). With get<int> / get<unsigned>
// the JSON value would static_cast onto a 32-bit type and could land on a valid
// switch case; reading into int64_t preserves the original value so the switch
// default catches it.

TEST(sib_update_remote_command, sib2_q_hyst_db_wrap_to_valid_case_rejected)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell = make_cell_skeleton();
  // 2^32 = 4294967296: get<int> would static_cast to 0 (a valid q_hyst_db case);
  // get<int64_t> preserves the value so the switch falls through to default.
  cell["sib"] = {{"type", "sib2"},
                 {"content",
                  {{"q_hyst_db", 4294967296LL},
                   {"thresh_serving_low_p", 14},
                   {"cell_reselection_priority", 4},
                   {"q_rx_lev_min", -70},
                   {"s_intra_search_p", 31},
                   {"t_reselection_nr", 1}}}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("q_hyst_db"), std::string::npos) << "actual: " << res.error();
  EXPECT_NE(res.error().find("invalid"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, sib3_q_offset_cell_wrap_to_valid_case_rejected)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell = make_cell_skeleton();
  // 2^32 = 4294967296: get<int> would static_cast to 0 (a valid q_offset_range case);
  // get<int64_t> preserves the value so the switch falls through to default.
  cell["sib"] = {
      {"type", "sib3"},
      {"content",
       {{"intra_freq_neigh_cell_list", nlohmann::json::array({{{"pci", 47}, {"q_offset_cell", 4294967296LL}}})}}}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("q_offset_cell"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, sib3_excluded_range_wrap_to_valid_case_rejected)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell = make_cell_skeleton();
  // 2^32 + 1 = 4294967297: get<unsigned> would static_cast to 1 (a valid pci_range case);
  // get<int64_t> preserves the value so the switch falls through to default.
  cell["sib"] = {
      {"type", "sib3"},
      {"content",
       {{"intra_freq_excluded_cell_list", nlohmann::json::array({{{"pci_start", 100}, {"range", 4294967297LL}}})}}}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("range"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, sib4_arfcn_out_of_range_returns_error)
{
  // Above the 3GPP NR-ARFCN max of 3279165; without a range check, uint32_t truncation would
  // silently accept a bogus value.
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib4"},
                 {"content",
                  {{"inter_freq_carrier_freq_list",
                    nlohmann::json::array({{{"arfcn", 5000000000ULL},
                                            {"ssb_scs", 30},
                                            {"derive_ssb_index_from_cell", true},
                                            {"q_rx_lev_min", -70},
                                            {"thresh_x_high_p", 16},
                                            {"thresh_x_low_p", 4}}})}}}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("arfcn"), std::string::npos) << "actual: " << res.error();
  EXPECT_NE(res.error().find("range"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, sib4_invalid_scs_value_returns_error)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib4"},
                 {"content",
                  {{"inter_freq_carrier_freq_list",
                    nlohmann::json::array({{{"arfcn", 649632},
                                            {"ssb_scs", 45}, // not a valid kHz value
                                            {"derive_ssb_index_from_cell", true},
                                            {"q_rx_lev_min", -70},
                                            {"thresh_x_high_p", 16},
                                            {"thresh_x_low_p", 4}}})}}}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("ssb_scs"), std::string::npos) << "actual: " << res.error();
}

TEST(sib_update_remote_command, sib3_with_empty_content_is_valid)
{
  // SIB3 has no mandatory top-level fields; an update with empty content should produce
  // a sib3_info with empty neighbor lists (a clear-all semantic).
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib3"}, {"content", nlohmann::json::object()}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_TRUE(res.has_value()) << res.error();

  const auto& sib3 = std::get<sib3_info>(*mock.last_req->cells[0].new_sys_info);
  EXPECT_TRUE(sib3.intra_freq_neigh_cell_list.empty());
  EXPECT_TRUE(sib3.intra_freq_excluded_cell_list.empty());
}

// Multi-cell.

TEST(sib_update_remote_command, multi_cell_request_produces_multiple_cell_entries)
{
  capturing_du_configurator mock;
  sib_update_remote_command cmd{mock};

  nlohmann::json cell_a = make_cell_skeleton();
  cell_a["sib"]         = {
      {"type", "sib3"},
      {"content", {{"intra_freq_neigh_cell_list", nlohmann::json::array({{{"pci", 47}, {"q_offset_cell", 0}}})}}}};

  nlohmann::json cell_b;
  cell_b["plmn"] = "001001";
  cell_b["nci"]  = uint64_t{6733825};
  cell_b["sib"]  = {
      {"type", "sib3"},
      {"content", {{"intra_freq_neigh_cell_list", nlohmann::json::array({{{"pci", 48}, {"q_offset_cell", 2}}})}}}};

  nlohmann::json req;
  req["cells"] = nlohmann::json::array({cell_a, cell_b});

  auto res = cmd.execute(req);
  ASSERT_TRUE(res.has_value()) << res.error();

  ASSERT_TRUE(mock.last_req.has_value());
  ASSERT_EQ(mock.last_req->cells.size(), 2U);
  EXPECT_EQ(std::get<sib3_info>(*mock.last_req->cells[0].new_sys_info).intra_freq_neigh_cell_list[0].pci, 47);
  EXPECT_EQ(std::get<sib3_info>(*mock.last_req->cells[1].new_sys_info).intra_freq_neigh_cell_list[0].pci, 48);
}

// Downstream failure propagation.

TEST(sib_update_remote_command, configurator_failure_is_reported_as_error)
{
  capturing_du_configurator mock;
  mock.next_response_success = false; // simulate DU rejecting the config
  sib_update_remote_command cmd{mock};

  auto cell   = make_cell_skeleton();
  cell["sib"] = {{"type", "sib3"}, {"content", nlohmann::json::object()}};

  auto res = cmd.execute(wrap(cell));
  ASSERT_FALSE(res.has_value());
  EXPECT_NE(res.error().find("failed"), std::string::npos) << "actual: " << res.error();
}
