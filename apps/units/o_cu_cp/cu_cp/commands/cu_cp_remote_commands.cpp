// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/o_cu_cp/cu_cp/commands/cu_cp_remote_commands.h"
#include "nlohmann/json.hpp"
#include "ocudu/cu_cp/cu_cp_cell_command_handler.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/plmn_identity.h"

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
