// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/o_cu_cp/cu_cp/commands/cu_cp_remote_commands.h"
#include "apps/units/o_cu_cp/cu_cp/cu_cp_config_translators.h"
#include "apps/units/o_cu_cp/cu_cp/cu_cp_unit_config.h"
#include "apps/units/o_cu_cp/cu_cp/cu_cp_unit_config_helpers.h"
#include "apps/units/o_cu_cp/cu_cp/cu_cp_unit_config_validator.h"
#include "nlohmann/json.hpp"
#include "ocudu/cu_cp/cu_cp_cell_command_handler.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/ran/tac.h"
#include "ocudu/support/enum_utils.h"
#include <limits>

using namespace ocudu;

namespace {

/// Parse the common {cgi: {plmn, nci}} JSON payload used by the cell lifecycle commands.
error_type<std::string> parse_cgi(const nlohmann::json& json, nr_cell_global_id_t& cgi)
{
  auto cgi_key = json.find("cgi");
  if (cgi_key == json.end()) {
    return make_unexpected("'cgi' object is missing and it is mandatory");
  }
  if (!cgi_key->is_object()) {
    return make_unexpected("'cgi' object value type should be an object");
  }

  auto plmn_key = cgi_key->find("plmn");
  if (plmn_key == cgi_key->end()) {
    return make_unexpected("'cgi.plmn' object is missing and it is mandatory");
  }
  if (!plmn_key->is_string()) {
    return make_unexpected("'cgi.plmn' object value type should be a string");
  }

  auto nci_key = cgi_key->find("nci");
  if (nci_key == cgi_key->end()) {
    return make_unexpected("'cgi.nci' object is missing and it is mandatory");
  }
  if (!nci_key->is_number_unsigned()) {
    return make_unexpected("'cgi.nci' object value type should be an unsigned integer");
  }

  auto plmn = plmn_identity::parse(plmn_key.value().get_ref<const nlohmann::json::string_t&>());
  if (!plmn) {
    return make_unexpected("Invalid PLMN identity value");
  }
  auto nci = nr_cell_identity::create(nci_key->get<uint64_t>());
  if (!nci) {
    return make_unexpected("Invalid NR cell identity value");
  }

  cgi.plmn_id = plmn.value();
  cgi.nci     = nci.value();
  return {};
}

/// Parse a mandatory unsigned integer field.
error_type<std::string> parse_unsigned(const nlohmann::json& json, const char* key, uint64_t& out)
{
  auto it = json.find(key);
  if (it == json.end()) {
    return make_unexpected(fmt::format("'{}' object is missing and it is mandatory", key));
  }
  if (!it->is_number_unsigned()) {
    return make_unexpected(fmt::format("'{}' object value type should be an unsigned integer", key));
  }
  out = it->get<uint64_t>();
  return {};
}

/// Parse an optional unsigned integer field. Leaves \c out untouched when the field is absent.
error_type<std::string>
parse_optional_unsigned(const nlohmann::json& json, const char* key, std::optional<uint64_t>& out)
{
  auto it = json.find(key);
  if (it == json.end()) {
    return {};
  }
  if (!it->is_number_unsigned()) {
    return make_unexpected(fmt::format("'{}' object value type should be an unsigned integer", key));
  }
  out = it->get<uint64_t>();
  return {};
}

/// Parse an optional signed integer field. Leaves \c out untouched when the field is absent.
error_type<std::string> parse_optional_int(const nlohmann::json& json, const char* key, std::optional<int>& out)
{
  auto it = json.find(key);
  if (it == json.end()) {
    return {};
  }
  if (!it->is_number_integer()) {
    return make_unexpected(fmt::format("'{}' object value type should be an integer", key));
  }
  out = it->get<int>();
  return {};
}

/// Whether a parsed number is a TAC this CU-CP accepts: within what a TAC can encode (is_valid) and not one of
/// the values TS 23.003 reserves, 0 and 0xfffffe.
bool is_acceptable_tac(uint64_t value)
{
  return value <= std::numeric_limits<tac_t>::max() && is_valid(static_cast<tac_t>(value)) && value != 0U &&
         value != 0xfffffeU;
}

/// Parse a mandatory NR cell identity field.
error_type<std::string> parse_nci(const nlohmann::json& json, const char* key, nr_cell_identity& out)
{
  uint64_t                value  = 0;
  error_type<std::string> result = parse_unsigned(json, key, value);
  if (!result.has_value()) {
    return result;
  }
  auto nci = nr_cell_identity::create(value);
  if (!nci) {
    return make_unexpected(fmt::format("Invalid NR cell identity value in '{}'", key));
  }
  out = nci.value();
  return {};
}

/// Parse a mandatory report config id field (1..63).
error_type<std::string> parse_report_cfg_id(const nlohmann::json& json, const char* key, ocucp::report_cfg_id_t& out)
{
  uint64_t                value  = 0;
  error_type<std::string> result = parse_unsigned(json, key, value);
  if (!result.has_value()) {
    return result;
  }
  if (value < 1 || value > 63) {
    return make_unexpected(fmt::format("'{}' must be in range [1, 63]", key));
  }
  out = ocucp::uint_to_report_cfg_id(value);
  return {};
}

/// Check that an unsigned value is one of the allowed values.
error_type<std::string> check_member(const char* key, uint64_t value, std::initializer_list<uint64_t> allowed)
{
  for (uint64_t a : allowed) {
    if (value == a) {
      return {};
    }
  }
  return make_unexpected(fmt::format("'{}' value {} is not one of the allowed values", key, value));
}

} // namespace

expected<nlohmann::json, std::string> cell_lock_remote_command::execute(const nlohmann::json& json)
{
  nr_cell_global_id_t     cgi;
  error_type<std::string> cgi_result = parse_cgi(json, cgi);
  if (not cgi_result.has_value()) {
    return make_unexpected(cgi_result.error());
  }

  if (not cu_cp.get_cell_command_handler().dispatch_deactivate_cell(cgi)) {
    return make_unexpected("CU-CP rejected cell_lock: no served DU matches the provided CGI, or scheduling failed");
  }
  return {};
}

expected<nlohmann::json, std::string> cell_unlock_remote_command::execute(const nlohmann::json& json)
{
  nr_cell_global_id_t     cgi;
  error_type<std::string> cgi_result = parse_cgi(json, cgi);
  if (not cgi_result.has_value()) {
    return make_unexpected(cgi_result.error());
  }

  if (not cu_cp.get_cell_command_handler().dispatch_activate_cell(cgi)) {
    return make_unexpected("CU-CP rejected cell_unlock: no served DU matches the provided CGI, or scheduling failed");
  }
  return {};
}

expected<nlohmann::json, std::string> cell_bar_remote_command::execute(const nlohmann::json& json)
{
  nr_cell_global_id_t     cgi;
  error_type<std::string> cgi_result = parse_cgi(json, cgi);
  if (not cgi_result.has_value()) {
    return make_unexpected(cgi_result.error());
  }

  if (not cu_cp.get_cell_command_handler().dispatch_bar_cell(cgi, /* barred = */ true)) {
    return make_unexpected("CU-CP rejected cell_bar: no served DU matches the provided CGI, or scheduling failed");
  }
  return {};
}

expected<nlohmann::json, std::string> cell_unbar_remote_command::execute(const nlohmann::json& json)
{
  nr_cell_global_id_t     cgi;
  error_type<std::string> cgi_result = parse_cgi(json, cgi);
  if (not cgi_result.has_value()) {
    return make_unexpected(cgi_result.error());
  }

  if (not cu_cp.get_cell_command_handler().dispatch_bar_cell(cgi, /* barred = */ false)) {
    return make_unexpected("CU-CP rejected cell_unbar: no served DU matches the provided CGI, or scheduling failed");
  }
  return {};
}

expected<nlohmann::json, std::string> cell_status_remote_command::execute(const nlohmann::json& json)
{
  nr_cell_global_id_t     cgi;
  error_type<std::string> cgi_result = parse_cgi(json, cgi);
  if (not cgi_result.has_value()) {
    return make_unexpected(cgi_result.error());
  }

  std::optional<ocucp::cu_cp_cell_state> state = cu_cp.get_cell_command_handler().dispatch_get_cell_state(cgi);
  if (not state.has_value()) {
    return make_unexpected("CU-CP has no cell matching the provided CGI, or the state read failed");
  }

  nlohmann::json result;
  result["admin_state"]       = ocucp::to_string(state->admin_state);
  result["operational_state"] = ocucp::to_string(state->operational_state);
  result["cell_barred"]       = state->barred;
  return result;
}

expected<nlohmann::json, std::string> mobility_cell_set_remote_command::execute(const nlohmann::json& json)
{
  ocucp::serving_cell_meas_config cell_cfg;

  error_type<std::string> result = parse_nci(json, "nci", cell_cfg.nci);
  if (not result.has_value()) {
    return make_unexpected(result.error());
  }

  uint64_t gnb_id_bit_length = 0;
  if (result = parse_unsigned(json, "gnb_id_bit_length", gnb_id_bit_length); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (gnb_id_bit_length < 22 || gnb_id_bit_length > 32) {
    return make_unexpected("'gnb_id_bit_length' must be in range [22, 32]");
  }
  cell_cfg.gnb_id_bit_length = gnb_id_bit_length;

  if (auto plmn_key = json.find("plmn"); plmn_key != json.end()) {
    if (!plmn_key->is_string()) {
      return make_unexpected("'plmn' object value type should be a string");
    }
    auto plmn = plmn_identity::parse(plmn_key->get_ref<const nlohmann::json::string_t&>());
    if (!plmn) {
      return make_unexpected("Invalid PLMN identity value");
    }
    cell_cfg.plmn = plmn.value();
  }

  std::optional<uint64_t> pci;
  if (result = parse_optional_unsigned(json, "pci", pci); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (pci.has_value()) {
    if (pci.value() > MAX_PCI) {
      return make_unexpected("'pci' must be in range [0, 1007]");
    }
    cell_cfg.pci = static_cast<pci_t>(pci.value());
  }

  std::optional<uint64_t> tac;
  if (result = parse_optional_unsigned(json, "tac", tac); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (tac.has_value()) {
    if (!is_acceptable_tac(tac.value())) {
      return make_unexpected("'tac' must be in range [1, 0xffffff] and not the reserved 0xfffffe");
    }
    cell_cfg.tac = static_cast<tac_t>(tac.value());
  }

  std::optional<uint64_t> band;
  if (result = parse_optional_unsigned(json, "band", band); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (band.has_value()) {
    cell_cfg.band = static_cast<nr_band>(band.value());
  }

  std::optional<uint64_t> ssb_arfcn;
  if (result = parse_optional_unsigned(json, "ssb_arfcn", ssb_arfcn); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (ssb_arfcn.has_value()) {
    cell_cfg.ssb_arfcn = arfcn_t{static_cast<uint32_t>(ssb_arfcn.value())};
  }

  std::optional<uint64_t> ssb_scs;
  if (result = parse_optional_unsigned(json, "ssb_scs", ssb_scs); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (ssb_scs.has_value()) {
    if (result = check_member("ssb_scs", ssb_scs.value(), {15, 30, 60, 120, 240}); not result.has_value()) {
      return make_unexpected(result.error());
    }
    cell_cfg.ssb_scs = to_subcarrier_spacing(std::to_string(ssb_scs.value()));
  }

  std::optional<uint64_t> ssb_period;
  std::optional<uint64_t> ssb_offset;
  std::optional<uint64_t> ssb_duration;
  if (result = parse_optional_unsigned(json, "ssb_period", ssb_period); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (result = parse_optional_unsigned(json, "ssb_offset", ssb_offset); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (result = parse_optional_unsigned(json, "ssb_duration", ssb_duration); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (ssb_period.has_value() != ssb_offset.has_value() || ssb_period.has_value() != ssb_duration.has_value()) {
    return make_unexpected("'ssb_period', 'ssb_offset' and 'ssb_duration' must be provided together");
  }
  if (ssb_period.has_value()) {
    if (result = check_member("ssb_period", ssb_period.value(), {5, 10, 20, 40, 80, 160}); not result.has_value()) {
      return make_unexpected(result.error());
    }
    if (result = check_member("ssb_duration", ssb_duration.value(), {1, 2, 3, 4, 5}); not result.has_value()) {
      return make_unexpected(result.error());
    }
    if (ssb_offset.value() >= ssb_period.value()) {
      return make_unexpected("'ssb_offset' must be smaller than 'ssb_period'");
    }
    ocucp::rrc_ssb_mtc ssb_mtc;
    ssb_mtc.periodicity_and_offset.periodicity =
        static_cast<ocucp::rrc_periodicity_and_offset::periodicity_t>(ssb_period.value());
    ssb_mtc.periodicity_and_offset.offset = static_cast<uint8_t>(ssb_offset.value());
    ssb_mtc.dur                           = static_cast<uint8_t>(ssb_duration.value());
    cell_cfg.ssb_mtc                      = ssb_mtc;
  }

  if (not cu_cp.get_mobility_config_handler().update_mobility_cell(cell_cfg)) {
    return make_unexpected("CU-CP rejected mobility_cell_set");
  }
  return {};
}

expected<nlohmann::json, std::string> mobility_cell_remove_remote_command::execute(const nlohmann::json& json)
{
  nr_cell_identity        nci;
  error_type<std::string> result = parse_nci(json, "nci", nci);
  if (not result.has_value()) {
    return make_unexpected(result.error());
  }

  if (not cu_cp.get_mobility_config_handler().remove_mobility_cell(nci)) {
    return make_unexpected("CU-CP rejected mobility_cell_remove: no cell with the provided NCI");
  }
  return {};
}

expected<nlohmann::json, std::string> neighbor_add_remote_command::execute(const nlohmann::json& json)
{
  nr_cell_identity        serving_nci;
  nr_cell_identity        neighbor_nci;
  error_type<std::string> result = parse_nci(json, "nci", serving_nci);
  if (not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (result = parse_nci(json, "neighbor_nci", neighbor_nci); not result.has_value()) {
    return make_unexpected(result.error());
  }

  auto report_configs_key = json.find("report_configs");
  if (report_configs_key == json.end()) {
    return make_unexpected("'report_configs' object is missing and it is mandatory");
  }
  if (!report_configs_key->is_array() || report_configs_key->empty()) {
    return make_unexpected("'report_configs' object value type should be a non-empty array");
  }
  std::vector<ocucp::report_cfg_id_t> report_cfg_ids;
  for (const auto& item : *report_configs_key) {
    if (!item.is_number_unsigned()) {
      return make_unexpected("'report_configs' entries should be unsigned integers");
    }
    uint64_t value = item.get<uint64_t>();
    if (value < 1 || value > 63) {
      return make_unexpected("'report_configs' entries must be in range [1, 63]");
    }
    report_cfg_ids.push_back(ocucp::uint_to_report_cfg_id(value));
  }

  if (not cu_cp.get_mobility_config_handler().update_neighbor(serving_nci, neighbor_nci, std::move(report_cfg_ids))) {
    return make_unexpected("CU-CP rejected mobility_neighbor_add: unknown cell, or invalid report config reference");
  }
  return {};
}

expected<nlohmann::json, std::string> neighbor_remove_remote_command::execute(const nlohmann::json& json)
{
  nr_cell_identity        serving_nci;
  nr_cell_identity        neighbor_nci;
  error_type<std::string> result = parse_nci(json, "nci", serving_nci);
  if (not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (result = parse_nci(json, "neighbor_nci", neighbor_nci); not result.has_value()) {
    return make_unexpected(result.error());
  }

  if (not cu_cp.get_mobility_config_handler().remove_neighbor(serving_nci, neighbor_nci)) {
    return make_unexpected("CU-CP rejected mobility_neighbor_remove: no such neighbor relation");
  }
  return {};
}

expected<nlohmann::json, std::string> report_config_set_remote_command::execute(const nlohmann::json& json)
{
  cu_cp_unit_report_config report_cfg;

  ocucp::report_cfg_id_t  report_cfg_id;
  error_type<std::string> result = parse_report_cfg_id(json, "report_cfg_id", report_cfg_id);
  if (not result.has_value()) {
    return make_unexpected(result.error());
  }
  report_cfg.report_cfg_id = to_underlying(report_cfg_id);

  auto report_type_key = json.find("report_type");
  if (report_type_key == json.end()) {
    return make_unexpected("'report_type' object is missing and it is mandatory");
  }
  if (!report_type_key->is_string()) {
    return make_unexpected("'report_type' object value type should be a string");
  }
  report_cfg.report_type = report_type_key->get<std::string>();

  if (auto event_key = json.find("event_triggered_report_type"); event_key != json.end()) {
    if (!event_key->is_string()) {
      return make_unexpected("'event_triggered_report_type' object value type should be a string");
    }
    std::optional<ocucp::rrc_event_id::event_id_t> event_id = ocucp::from_string(event_key->get<std::string>());
    if (!event_id.has_value()) {
      return make_unexpected("Invalid 'event_triggered_report_type' value");
    }
    if (event_id == ocucp::rrc_event_id::event_id_t::d1 || event_id == ocucp::rrc_event_id::event_id_t::d2 ||
        event_id == ocucp::rrc_event_id::event_id_t::t1) {
      return make_unexpected("Distance and time based events (d1, d2, t1) are not supported through this command");
    }
    report_cfg.event_triggered_report_type = event_id;
  }

  if (auto quantity_key = json.find("meas_trigger_quantity"); quantity_key != json.end()) {
    if (!quantity_key->is_string()) {
      return make_unexpected("'meas_trigger_quantity' object value type should be a string");
    }
    const std::string& quantity = quantity_key->get_ref<const nlohmann::json::string_t&>();
    if (quantity != "rsrp" && quantity != "rsrq" && quantity != "sinr") {
      return make_unexpected("'meas_trigger_quantity' must be one of rsrp, rsrq, sinr");
    }
    report_cfg.meas_trigger_quantity = quantity;
  }

  // Signed thresholds and offsets.
  if (result =
          parse_optional_int(json, "meas_trigger_quantity_threshold_db", report_cfg.meas_trigger_quantity_threshold_db);
      not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (result = parse_optional_int(
          json, "meas_trigger_quantity_threshold_2_db", report_cfg.meas_trigger_quantity_threshold_2_db);
      not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (result = parse_optional_int(json, "meas_trigger_quantity_offset_db", report_cfg.meas_trigger_quantity_offset_db);
      not result.has_value()) {
    return make_unexpected(result.error());
  }

  std::optional<uint64_t> hysteresis;
  if (result = parse_optional_unsigned(json, "hysteresis_db", hysteresis); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (hysteresis.has_value()) {
    if (hysteresis.value() > 15) {
      return make_unexpected("'hysteresis_db' must be in range [0, 15]");
    }
    report_cfg.hysteresis_db = hysteresis.value();
  }

  std::optional<uint64_t> ttt;
  if (result = parse_optional_unsigned(json, "time_to_trigger_ms", ttt); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (ttt.has_value()) {
    if (result = check_member("time_to_trigger_ms",
                              ttt.value(),
                              {0, 40, 64, 80, 100, 128, 160, 256, 320, 480, 512, 640, 1024, 1280, 2560, 5120});
        not result.has_value()) {
      return make_unexpected(result.error());
    }
    report_cfg.time_to_trigger_ms = ttt.value();
  }

  std::optional<uint64_t> interval;
  if (result = parse_optional_unsigned(json, "report_interval_ms", interval); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (interval.has_value()) {
    if (result =
            check_member("report_interval_ms",
                         interval.value(),
                         {120, 240, 480, 640, 1024, 2048, 5120, 10240, 20480, 40960, 60000, 360000, 720000, 1800000});
        not result.has_value()) {
      return make_unexpected(result.error());
    }
    report_cfg.report_interval_ms = interval.value();
  } else if (report_cfg.report_type != "cond_trigger") {
    return make_unexpected("'report_interval_ms' is mandatory for periodical and event_triggered report configs");
  }

  std::optional<uint64_t> t312;
  if (result = parse_optional_unsigned(json, "t312", t312); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (t312.has_value()) {
    if (result = check_member("t312", t312.value(), {0, 50, 100, 200, 300, 400, 500, 1000}); not result.has_value()) {
      return make_unexpected(result.error());
    }
    report_cfg.t312_ms = t312.value();
  }

  if (auto offset_key = json.find("periodic_ho_rsrp_offset_db"); offset_key != json.end()) {
    if (!offset_key->is_number_integer()) {
      return make_unexpected("'periodic_ho_rsrp_offset_db' object value type should be an integer");
    }
    int value = offset_key->get<int>();
    if (value < -1 || value > 30) {
      return make_unexpected("'periodic_ho_rsrp_offset_db' must be in range [-1, 30]");
    }
    report_cfg.periodic_ho_rsrp_offset = value;
  }

  // Reuse the same validation and translation the YAML configuration path uses.
  if (error_type<std::string> res = validate_report_config(report_cfg); not res.has_value()) {
    return make_unexpected(fmt::format("Invalid report configuration: {}", res.error()));
  }

  if (not cu_cp.get_mobility_config_handler().update_report_config(report_cfg_id,
                                                                   generate_cu_cp_report_config(report_cfg))) {
    return make_unexpected("CU-CP rejected mobility_report_config_set: type conflicts with existing references");
  }
  return {};
}

expected<nlohmann::json, std::string> report_config_remove_remote_command::execute(const nlohmann::json& json)
{
  ocucp::report_cfg_id_t  report_cfg_id;
  error_type<std::string> result = parse_report_cfg_id(json, "report_cfg_id", report_cfg_id);
  if (not result.has_value()) {
    return make_unexpected(result.error());
  }

  if (not cu_cp.get_mobility_config_handler().remove_report_config(report_cfg_id)) {
    return make_unexpected("CU-CP rejected mobility_report_config_remove: unknown id, or still referenced");
  }
  return {};
}

expected<nlohmann::json, std::string> periodic_report_set_remote_command::execute(const nlohmann::json& json)
{
  nr_cell_identity        nci;
  error_type<std::string> result = parse_nci(json, "nci", nci);
  if (not result.has_value()) {
    return make_unexpected(result.error());
  }

  std::optional<ocucp::report_cfg_id_t> report_cfg_id;
  if (json.find("report_cfg_id") != json.end()) {
    ocucp::report_cfg_id_t id;
    if (result = parse_report_cfg_id(json, "report_cfg_id", id); not result.has_value()) {
      return make_unexpected(result.error());
    }
    report_cfg_id = id;
  }

  if (not cu_cp.get_mobility_config_handler().set_periodic_report(nci, report_cfg_id)) {
    return make_unexpected("CU-CP rejected mobility_periodic_report_set: unknown cell or non-periodical report config");
  }
  return {};
}

expected<nlohmann::json, std::string> trigger_handover_remote_command::execute(const nlohmann::json& json)
{
  uint64_t                serving_pci = 0;
  error_type<std::string> result      = parse_unsigned(json, "serving_pci", serving_pci);
  if (not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (serving_pci > MAX_PCI) {
    return make_unexpected("'serving_pci' must be in range [0, 1007]");
  }

  uint64_t rnti = 0;
  if (result = parse_unsigned(json, "rnti", rnti); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (rnti > 0xffff) {
    return make_unexpected("'rnti' must be in range [0, 65535]");
  }

  uint64_t target_pci = 0;
  if (result = parse_unsigned(json, "target_pci", target_pci); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (target_pci > MAX_PCI) {
    return make_unexpected("'target_pci' must be in range [0, 1007]");
  }

  auto plmn_key = json.find("plmn");
  if (plmn_key == json.end()) {
    return make_unexpected("'plmn' object is missing and it is mandatory");
  }
  if (!plmn_key->is_string()) {
    return make_unexpected("'plmn' object value type should be a string");
  }
  auto plmn = plmn_identity::parse(plmn_key->get_ref<const nlohmann::json::string_t&>());
  if (!plmn) {
    return make_unexpected("Invalid PLMN identity value");
  }

  uint64_t tac = 0;
  if (result = parse_unsigned(json, "tac", tac); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (!is_acceptable_tac(tac)) {
    return make_unexpected("'tac' must be in range [1, 0xffffff] and not the reserved 0xfffffe");
  }

  if (not cu_cp.get_mobility_command_handler().trigger_handover(static_cast<pci_t>(serving_pci),
                                                                static_cast<rnti_t>(rnti),
                                                                static_cast<pci_t>(target_pci),
                                                                plmn.value(),
                                                                static_cast<tac_t>(tac))) {
    return make_unexpected("CU-CP rejected trigger_handover: unknown UE or target cell, or the trigger could not be "
                           "dispatched (see CU-CP log for the cause)");
  }
  return {};
}

expected<nlohmann::json, std::string> trigger_conditional_handover_remote_command::execute(const nlohmann::json& json)
{
  uint64_t                serving_pci = 0;
  error_type<std::string> result      = parse_unsigned(json, "serving_pci", serving_pci);
  if (not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (serving_pci > MAX_PCI) {
    return make_unexpected("'serving_pci' must be in range [0, 1007]");
  }

  uint64_t rnti = 0;
  if (result = parse_unsigned(json, "rnti", rnti); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (rnti > 0xffff) {
    return make_unexpected("'rnti' must be in range [0, 65535]");
  }

  auto target_pcis_key = json.find("target_pcis");
  if (target_pcis_key == json.end()) {
    return make_unexpected("'target_pcis' object is missing and it is mandatory");
  }
  if (!target_pcis_key->is_array() || target_pcis_key->empty() || target_pcis_key->size() > 8) {
    return make_unexpected("'target_pcis' object value type should be an array of 1 to 8 PCIs");
  }
  std::vector<pci_t> target_pcis;
  for (const auto& item : *target_pcis_key) {
    if (!item.is_number_unsigned() || item.get<uint64_t>() > MAX_PCI) {
      return make_unexpected("'target_pcis' entries must be unsigned integers in range [0, 1007]");
    }
    target_pcis.push_back(static_cast<pci_t>(item.get<uint64_t>()));
  }

  std::chrono::milliseconds timeout = default_timeout;
  std::optional<uint64_t>   timeout_ms;
  if (result = parse_optional_unsigned(json, "timeout_ms", timeout_ms); not result.has_value()) {
    return make_unexpected(result.error());
  }
  if (timeout_ms.has_value()) {
    if (timeout_ms.value() < 1 || timeout_ms.value() > 600000) {
      return make_unexpected("'timeout_ms' must be in range [1, 600000]");
    }
    timeout = std::chrono::milliseconds{timeout_ms.value()};
  }

  std::optional<std::chrono::system_clock::time_point> t1_thres_override;
  if (auto t1_key = json.find("t1_thres"); t1_key != json.end()) {
    if (t1_key->is_number_integer()) {
      // Unix milliseconds as a number.
      t1_thres_override = std::chrono::system_clock::time_point{std::chrono::milliseconds{t1_key->get<int64_t>()}};
    } else if (t1_key->is_string()) {
      // Unix milliseconds or YYYY-MM-DDTHH:MM:SS[.mmm] as a string.
      auto t1 = parse_timestamp_ms(t1_key->get<std::string>());
      if (!t1.has_value()) {
        return make_unexpected(fmt::format("Invalid 't1_thres' value: {}", t1.error()));
      }
      t1_thres_override = t1.value();
    } else {
      return make_unexpected("'t1_thres' object value type should be an integer (unix ms) or a string");
    }
  }

  if (not cu_cp.get_mobility_command_handler().trigger_conditional_handover(
          static_cast<pci_t>(serving_pci), static_cast<rnti_t>(rnti), target_pcis, timeout, t1_thres_override)) {
    return make_unexpected("CU-CP rejected trigger_conditional_handover: the trigger could not be dispatched (see "
                           "CU-CP log for the cause)");
  }
  return {};
}
