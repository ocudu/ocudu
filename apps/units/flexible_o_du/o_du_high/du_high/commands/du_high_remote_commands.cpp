// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/units/flexible_o_du/o_du_high/du_high/commands/du_high_remote_commands.h"
#include "nlohmann/json.hpp"

using namespace ocudu;

error_type<std::string> ssb_modify_remote_command::execute(const nlohmann::json& json)
{
  auto cells_key = json.find("cells");
  if (cells_key == json.end()) {
    return make_unexpected("'cells' object is missing and it is mandatory");
  }
  if (!cells_key->is_array()) {
    return make_unexpected("'cells' object value type should be an array");
  }

  auto cells_items = cells_key->items();
  if (cells_items.begin() == cells_items.end()) {
    return make_unexpected("'cells' object does not contain any cell entries");
  }

  odu::du_param_config_request req;
  for (const auto& cell : cells_items) {
    auto plmn_key = cell.value().find("plmn");
    if (plmn_key == cell.value().end()) {
      return make_unexpected("'plmn' object is missing and it is mandatory");
    }
    if (!plmn_key->is_string()) {
      return make_unexpected("'plmn' object value type should be a string");
    }

    auto nci_key = cell.value().find("nci");
    if (nci_key == cell.value().end()) {
      return make_unexpected("'nci' object is missing and it is mandatory");
    }
    if (!nci_key->is_number_unsigned()) {
      return make_unexpected("'nci' object value type should be an integer");
    }

    auto plmn = plmn_identity::parse(plmn_key.value().get_ref<const nlohmann::json::string_t&>());
    if (!plmn) {
      return make_unexpected("Invalid PLMN identity value");
    }
    auto nci = nr_cell_identity::create(nci_key->get<uint64_t>());
    if (!nci) {
      return make_unexpected("Invalid NR cell identity value");
    }
    nr_cell_global_id_t nr_cgi;
    nr_cgi.nci     = nci.value();
    nr_cgi.plmn_id = plmn.value();

    auto ssb_block_power_key = cell.value().find("ssb_block_power_dbm");
    if (ssb_block_power_key == cell.value().end()) {
      return make_unexpected("'ssb_block_power_dbm' object is missing and it is mandatory");
    }
    if (!ssb_block_power_key->is_number_integer()) {
      return make_unexpected("'ssb_block_power_dbm' object value type should be an integer");
    }
    int ssb_block_power_value = ssb_block_power_key->get<int>();
    if (ssb_block_power_value < -60 || ssb_block_power_value > 50) {
      return make_unexpected(
          fmt::format("'ssb_block_power_dbm' value out of range, received '{}', valid range is from -60 to 50",
                      ssb_block_power_value));
    }
    req.cells.emplace_back(nr_cgi, ssb_block_power_value);
  }

  if (configurator.handle_sync_operator_config(req).success) {
    return {};
  }

  return make_unexpected("SSB modify command procedure failed to be applied by the DU");
}

error_type<std::string> rrm_policy_ratio_remote_command::execute(const nlohmann::json& json)
{
  auto policies_key = json.find("policies");
  if (policies_key == json.end()) {
    return make_unexpected("'policies' object is missing and it is mandatory");
  }

  odu::du_param_config_request req;

  // RRM Policy ratio group.
  rrm_policy_ratio_group rrm_policy_group;

  // Resource type.
  auto resource_type_key = policies_key.value().find("resourceType");
  if (resource_type_key == policies_key.value().end()) {
    return make_unexpected("'resourceType' object is missing and it is mandatory");
  }
  if (!resource_type_key->is_string()) {
    return make_unexpected("'resourceType' object value type should be a string");
  }
  if (resource_type_key->get<std::string>() == "PRB") {
    rrm_policy_group.resource_type = rrm_policy_ratio_group::resource_type_t::prb;
  } else {
    // TODO: Add support for "RRC" and "DRB"
    return make_unexpected("Only 'resourceType' PRB value is supported");
  }

  // RRM Policy members list.
  auto policy_members_list = policies_key.value().find("rRMPolicyMemberList");
  if (policy_members_list == policies_key.value().end()) {
    return make_unexpected("'rRMPolicyMemberList' object is missing and it is mandatory");
  }
  if (!policy_members_list->is_array()) {
    return make_unexpected("'rRMPolicyMemberList' object value type should be an array");
  }

  for (const auto& policy_member_key : policy_members_list->items()) {
    auto plmn_key = policy_member_key.value().find("plmn");
    if (plmn_key == policy_member_key.value().end()) {
      return make_unexpected("'plmn' object is missing and it is mandatory");
    }
    if (!plmn_key->is_string()) {
      return make_unexpected("'plmn' object value type should be a string");
    }
    expected<plmn_identity> plmn = plmn_identity::parse(plmn_key.value().get_ref<const nlohmann::json::string_t&>());
    if (!plmn) {
      return make_unexpected("Invalid PLMN identity value");
    }

    auto sst_key = policy_member_key.value().find("sst");
    if (sst_key == policy_member_key.value().end()) {
      return make_unexpected("'sst' object is missing and it is mandatory");
    }
    if (!sst_key->is_number_integer()) {
      return make_unexpected("'sst' object value type should be an integer");
    }
    uint8_t sst = sst_key->get<uint8_t>();

    auto sd_key = policy_member_key.value().find("sd");

    expected<slice_differentiator> sd = make_unexpected(default_error_t{});
    // SD is optional.
    if (sd_key != policy_member_key.value().end()) {
      if (!sd_key->is_number_integer()) {
        return make_unexpected("'sd' object value type should be an integer");
      }

      uint32_t sd_int = sd_key->get<uint32_t>();
      sd              = slice_differentiator::create(sd_int);
      if (!sd) {
        return make_unexpected("Invalid slice differentiator value");
      }
    }

    rrm_policy_member policy_member;
    policy_member.plmn_id     = plmn.value();
    policy_member.s_nssai.sst = slice_service_type{sst};
    policy_member.s_nssai.sd  = sd.has_value() ? sd.value() : slice_differentiator{};

    rrm_policy_group.policy_members_list.push_back(policy_member);
  }

  // Minimum percentage of PRBs to be allocated to this group.
  auto                    min_prb_policy_ratio       = policies_key.value().find("min_prb_policy_ratio");
  std::optional<unsigned> min_prb_policy_ratio_value = std::nullopt;
  if (min_prb_policy_ratio != policies_key.value().end()) {
    if (!min_prb_policy_ratio->is_number_integer()) {
      return make_unexpected("'min_prb_policy_ratio' object value type should be an integer");
    }
    min_prb_policy_ratio_value = static_cast<unsigned>(min_prb_policy_ratio->get<int>());
    if (min_prb_policy_ratio_value < 0 || min_prb_policy_ratio_value > 100) {
      return make_unexpected(
          fmt::format("'min_prb_policy_ratio' value out of range, received '{}', valid range is from 0 to 100",
                      min_prb_policy_ratio_value.value()));
    }
  }

  // Maximum percentage of PRBs to be allocated to this group.
  auto                    max_prb_policy_ratio       = policies_key.value().find("max_prb_policy_ratio");
  std::optional<unsigned> max_prb_policy_ratio_value = std::nullopt;
  if (max_prb_policy_ratio != policies_key.value().end()) {
    if (!max_prb_policy_ratio->is_number_integer()) {
      return make_unexpected("'max_prb_policy_ratio' object value type should be an integer");
    }
    max_prb_policy_ratio_value = static_cast<unsigned>(max_prb_policy_ratio->get<int>());
    if (max_prb_policy_ratio_value < 0 || max_prb_policy_ratio_value > 100) {
      return make_unexpected(
          fmt::format("'max_prb_policy_ratio' value out of range, received '{}', valid range is from 0 to 100",
                      max_prb_policy_ratio_value.value()));
    }
  }

  /// The percentage of PRBs to be allocated to this group.
  auto                    dedicated_ratio       = policies_key.value().find("dedicated_ratio");
  std::optional<unsigned> dedicated_ratio_value = std::nullopt;
  if (dedicated_ratio != policies_key.value().end()) {
    if (!dedicated_ratio->is_number_integer()) {
      return make_unexpected("'dedicated_ratio' object value type should be an integer");
    }
    dedicated_ratio_value = static_cast<unsigned>(dedicated_ratio->get<int>());
    if (dedicated_ratio_value < 0 || dedicated_ratio_value > 100) {
      return make_unexpected(
          fmt::format("'dedicated_ratio' value out of range, received '{}', valid range is from 0 to 100",
                      dedicated_ratio_value.value()));
    }
  }

  rrm_policy_group.minimum_ratio   = min_prb_policy_ratio_value;
  rrm_policy_group.maximum_ratio   = max_prb_policy_ratio_value;
  rrm_policy_group.dedicated_ratio = dedicated_ratio_value;

  req.cells.emplace_back(std::nullopt, std::nullopt, std::vector<rrm_policy_ratio_group>{rrm_policy_group});

  if (configurator.handle_sync_operator_config(req).success) {
    return {};
  }

  return make_unexpected("RRM policy ratio remote command procedure failed to be applied by the DU");
}

/// Parses a NR cell global identifier (PLMN + NCI) from a JSON cell entry.
static expected<nr_cell_global_id_t, std::string> parse_cgi(const nlohmann::json& cell)
{
  auto plmn_key = cell.find("plmn");
  if (plmn_key == cell.end()) {
    return make_unexpected("'plmn' object is missing and it is mandatory");
  }
  if (!plmn_key->is_string()) {
    return make_unexpected("'plmn' object value type should be a string");
  }
  auto plmn = plmn_identity::parse(plmn_key->get_ref<const nlohmann::json::string_t&>());
  if (!plmn) {
    return make_unexpected("Invalid PLMN identity value");
  }

  auto nci_key = cell.find("nci");
  if (nci_key == cell.end()) {
    return make_unexpected("'nci' object is missing and it is mandatory");
  }
  if (!nci_key->is_number_unsigned()) {
    return make_unexpected("'nci' object value type should be an unsigned integer");
  }
  auto nci = nr_cell_identity::create(nci_key->get<uint64_t>());
  if (!nci) {
    return make_unexpected("Invalid NR cell identity value");
  }

  nr_cell_global_id_t cgi;
  cgi.plmn_id = plmn.value();
  cgi.nci     = nci.value();
  return cgi;
}

/// Parses the 'q_hyst_db' field into a q_hyst_t.
///
/// q_hyst_t enum values equal the integer dB values (db4 = 4), but the enum skips 7, 9, 11, ...,
/// so the switch validates membership; a cast then performs the mapping.
static expected<q_hyst_t, std::string> parse_q_hyst_db(const nlohmann::json& obj)
{
  auto key = obj.find("q_hyst_db");
  if (key == obj.end()) {
    return make_unexpected("'q_hyst_db' field is missing and it is mandatory");
  }
  if (!key->is_number_integer()) {
    return make_unexpected("'q_hyst_db' value type should be an integer");
  }
  const int64_t v = key->get<int64_t>();
  switch (v) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 8:
    case 10:
    case 12:
    case 14:
    case 16:
    case 18:
    case 20:
    case 22:
    case 24:
      return static_cast<q_hyst_t>(v);
    default:
      return make_unexpected(
          fmt::format("'q_hyst_db' value '{}' is invalid; valid values: 0,1,2,3,4,5,6,8,10,12,14,16,18,20,22,24", v));
  }
}

/// Parses an integer dB value into a q_offset_range_t.
///
/// q_offset_range_t enum values equal the signed integer dB values (db24 = 24, db_24 = -24),
/// with gaps at odd values above +/-5. Validate then cast.
static expected<q_offset_range_t, std::string> parse_q_offset_range(const nlohmann::json& val,
                                                                    std::string_view      field_name)
{
  if (!val.is_number_integer()) {
    return make_unexpected(fmt::format("'{}' value type should be an integer", field_name));
  }
  const int64_t v = val.get<int64_t>();
  switch (v) {
    case -24:
    case -22:
    case -20:
    case -18:
    case -16:
    case -14:
    case -12:
    case -10:
    case -8:
    case -6:
    case -5:
    case -4:
    case -3:
    case -2:
    case -1:
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 8:
    case 10:
    case 12:
    case 14:
    case 16:
    case 18:
    case 20:
    case 22:
    case 24:
      return static_cast<q_offset_range_t>(v);
    default:
      return make_unexpected(
          fmt::format("'{}' value '{}' is invalid; valid values: "
                      "-24,-22,-20,-18,-16,-14,-12,-10,-8,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,8,10,12,14,16,18,20,22,24",
                      field_name,
                      v));
  }
}

/// Parses a kHz subcarrier-spacing value into a subcarrier_spacing.
///
/// subcarrier_spacing is numerology-indexed (kHz30 = 1), so the switch is a genuine mapping.
static expected<subcarrier_spacing, std::string> parse_scs_khz(const nlohmann::json& val, std::string_view field_name)
{
  if (!val.is_number_integer()) {
    return make_unexpected(fmt::format("'{}' value type should be a non-negative integer", field_name));
  }
  const auto v = val.get<int64_t>();
  if (v < 0) {
    return make_unexpected(fmt::format("'{}' value type should be a non-negative integer", field_name));
  }
  switch (v) {
    case 15:
      return subcarrier_spacing::kHz15;
    case 30:
      return subcarrier_spacing::kHz30;
    case 60:
      return subcarrier_spacing::kHz60;
    case 120:
      return subcarrier_spacing::kHz120;
    case 240:
      return subcarrier_spacing::kHz240;
    default:
      return make_unexpected(
          fmt::format("'{}' value '{}' is invalid; valid kHz values: 15, 30, 60, 120, 240", field_name, v));
  }
}

/// Parses an integer JSON value into a bounded_integer<T, MIN, MAX> after range validation.
template <typename T, T MIN, T MAX>
static expected<bounded_integer<T, MIN, MAX>, std::string> parse_bounded_int(const nlohmann::json& val,
                                                                             std::string_view      field_name)
{
  if (!val.is_number_integer()) {
    return make_unexpected(fmt::format("'{}' value type should be an integer", field_name));
  }
  const auto v = val.get<int64_t>();
  if (v < static_cast<int64_t>(MIN) || v > static_cast<int64_t>(MAX)) {
    return make_unexpected(fmt::format(
        "'{}' value '{}' out of range [{},{}]", field_name, v, static_cast<int64_t>(MIN), static_cast<int64_t>(MAX)));
  }
  return bounded_integer<T, MIN, MAX>{static_cast<T>(v)};
}

/// Looks up a mandatory bounded-integer field in an object and parses it.
template <typename T, T MIN, T MAX>
static expected<bounded_integer<T, MIN, MAX>, std::string> find_and_parse_bounded_int(const nlohmann::json& obj,
                                                                                      std::string_view      field)
{
  auto it = obj.find(field);
  if (it == obj.end()) {
    return make_unexpected(fmt::format("'{}' field is missing and it is mandatory", field));
  }
  return parse_bounded_int<T, MIN, MAX>(*it, field);
}

/// Looks up a mandatory PCI field (range 0..1007) in an object.
static expected<pci_t, std::string> find_and_parse_pci(const nlohmann::json& obj, std::string_view field)
{
  auto it = obj.find(field);
  if (it == obj.end()) {
    return make_unexpected(fmt::format("'{}' field is missing and it is mandatory", field));
  }
  if (!it->is_number_integer()) {
    return make_unexpected(fmt::format("'{}' value type should be an integer", field));
  }
  const int64_t v = it->get<int64_t>();
  if (v < 0 || v > 1007) {
    return make_unexpected(fmt::format("'{}' value '{}' out of range [0, 1007]", field, v));
  }
  return static_cast<pci_t>(v);
}

/// Parses SIB2 cell-reselection content from a JSON object.
static expected<sib2_info, std::string> parse_sib2(const nlohmann::json& content)
{
  sib2_info sib2;

  auto q_hyst_exp = parse_q_hyst_db(content);
  if (!q_hyst_exp) {
    return make_unexpected(q_hyst_exp.error());
  }
  sib2.q_hyst = q_hyst_exp.value();

  auto thresh_serv_exp = find_and_parse_bounded_int<uint8_t, 0, 31>(content, "thresh_serving_low_p");
  if (!thresh_serv_exp) {
    return make_unexpected(thresh_serv_exp.error());
  }
  sib2.thresh_serving_low_p = thresh_serv_exp.value();

  auto reselect_prio_exp = find_and_parse_bounded_int<uint8_t, 0, 7>(content, "cell_reselection_priority");
  if (!reselect_prio_exp) {
    return make_unexpected(reselect_prio_exp.error());
  }
  sib2.cell_reselection_priority = reselect_prio_exp.value();

  auto q_rx_lev_min_exp = find_and_parse_bounded_int<int8_t, -70, -22>(content, "q_rx_lev_min");
  if (!q_rx_lev_min_exp) {
    return make_unexpected(q_rx_lev_min_exp.error());
  }
  sib2.q_rx_lev_min = q_rx_lev_min_exp.value();

  auto s_intra_search_exp = find_and_parse_bounded_int<uint8_t, 0, 31>(content, "s_intra_search_p");
  if (!s_intra_search_exp) {
    return make_unexpected(s_intra_search_exp.error());
  }
  sib2.s_intra_search_p = s_intra_search_exp.value();

  auto t_reselection_exp = find_and_parse_bounded_int<uint8_t, 0, 7>(content, "t_reselection_nr");
  if (!t_reselection_exp) {
    return make_unexpected(t_reselection_exp.error());
  }
  sib2.t_reselection_nr = t_reselection_exp.value();

  return sib2;
}

/// Parses SIB3 intra-frequency neighbor list and excluded-cell list from a JSON object.
static expected<sib3_info, std::string> parse_sib3(const nlohmann::json& content)
{
  sib3_info sib3;

  auto neigh_list_it = content.find("intra_freq_neigh_cell_list");
  if (neigh_list_it != content.end()) {
    if (!neigh_list_it->is_array()) {
      return make_unexpected("'intra_freq_neigh_cell_list' value type should be an array");
    }
    for (const auto& entry : neigh_list_it->items()) {
      const auto& neigh_obj = entry.value();
      if (!neigh_obj.is_object()) {
        return make_unexpected("'intra_freq_neigh_cell_list' entries should be objects");
      }
      auto pci_exp = find_and_parse_pci(neigh_obj, "pci");
      if (!pci_exp) {
        return make_unexpected(pci_exp.error());
      }
      auto q_offset_it = neigh_obj.find("q_offset_cell");
      if (q_offset_it == neigh_obj.end()) {
        return make_unexpected("'q_offset_cell' field is missing in 'intra_freq_neigh_cell_list' entry");
      }
      auto q_offset_exp = parse_q_offset_range(*q_offset_it, "q_offset_cell");
      if (!q_offset_exp) {
        return make_unexpected(q_offset_exp.error());
      }
      intra_freq_neigh_cell_info neigh;
      neigh.pci           = pci_exp.value();
      neigh.q_offset_cell = q_offset_exp.value();
      sib3.intra_freq_neigh_cell_list.push_back(neigh);
    }
  }

  auto excluded_list_it = content.find("intra_freq_excluded_cell_list");
  if (excluded_list_it != content.end()) {
    if (!excluded_list_it->is_array()) {
      return make_unexpected("'intra_freq_excluded_cell_list' value type should be an array");
    }
    for (const auto& entry : excluded_list_it->items()) {
      const auto& excl_obj = entry.value();
      if (!excl_obj.is_object()) {
        return make_unexpected("'intra_freq_excluded_cell_list' entries should be objects");
      }
      auto start_exp = find_and_parse_pci(excl_obj, "pci_start");
      if (!start_exp) {
        return make_unexpected(start_exp.error());
      }
      auto range_it = excl_obj.find("range");
      if (range_it == excl_obj.end() || !range_it->is_number_integer()) {
        return make_unexpected("'range' missing or not a non-negative integer in excluded list entry");
      }
      const int64_t range_val = range_it->get<int64_t>();
      if (range_val < 0) {
        return make_unexpected("'range' missing or not a non-negative integer in excluded list entry");
      }
      // pci_range_t::range_t enum values equal the integer widths (n4 = 4, n1008 = 1008).
      pci_range_t pci_range;
      pci_range.start = start_exp.value();
      switch (range_val) {
        case 1:
        case 4:
        case 8:
        case 12:
        case 16:
        case 24:
        case 32:
        case 48:
        case 64:
        case 84:
        case 96:
        case 128:
        case 168:
        case 252:
        case 504:
        case 1008:
          pci_range.size = static_cast<pci_range_t::range_t>(range_val);
          break;
        default:
          return make_unexpected(fmt::format(
              "'range' value '{}' invalid; valid values: 1,4,8,12,16,24,32,48,64,84,96,128,168,252,504,1008",
              range_val));
      }
      sib3.intra_freq_excluded_cell_list.push_back(pci_range);
    }
  }

  return sib3;
}

/// Parses SIB4 inter-frequency carrier list from a JSON object.
static expected<sib4_info, std::string> parse_sib4(const nlohmann::json& content)
{
  sib4_info sib4;

  auto carrier_list_it = content.find("inter_freq_carrier_freq_list");
  if (carrier_list_it == content.end()) {
    return make_unexpected("'inter_freq_carrier_freq_list' field is missing and it is mandatory");
  }
  if (!carrier_list_it->is_array()) {
    return make_unexpected("'inter_freq_carrier_freq_list' value type should be an array");
  }
  if (carrier_list_it->empty()) {
    return make_unexpected("'inter_freq_carrier_freq_list' must contain at least one entry");
  }

  for (const auto& entry : carrier_list_it->items()) {
    const auto& carrier_obj = entry.value();
    if (!carrier_obj.is_object()) {
      return make_unexpected("'inter_freq_carrier_freq_list' entries should be objects");
    }

    // NR-ARFCN, TS 38.101-1 Table 5.4.2.1-1: max value 3279165.
    auto arfcn_it = carrier_obj.find("arfcn");
    if (arfcn_it == carrier_obj.end()) {
      return make_unexpected("'arfcn' field is missing in carrier list entry");
    }
    if (!arfcn_it->is_number_integer()) {
      return make_unexpected("'arfcn' value type should be an integer");
    }
    const int64_t arfcn_val = arfcn_it->get<int64_t>();
    if (arfcn_val < 0 || arfcn_val > 3279165) {
      return make_unexpected(fmt::format("'arfcn' value '{}' out of range [0, 3279165]", arfcn_val));
    }

    auto scs_it = carrier_obj.find("ssb_scs");
    if (scs_it == carrier_obj.end()) {
      return make_unexpected("'ssb_scs' missing in carrier list entry");
    }
    auto scs_exp = parse_scs_khz(*scs_it, "ssb_scs");
    if (!scs_exp) {
      return make_unexpected(scs_exp.error());
    }

    auto derive_it = carrier_obj.find("derive_ssb_index_from_cell");
    if (derive_it == carrier_obj.end() || !derive_it->is_boolean()) {
      return make_unexpected("'derive_ssb_index_from_cell' missing or non-boolean in carrier list entry");
    }

    auto q_rx_lev_min_exp = find_and_parse_bounded_int<int8_t, -70, -22>(carrier_obj, "q_rx_lev_min");
    if (!q_rx_lev_min_exp) {
      return make_unexpected(q_rx_lev_min_exp.error());
    }

    auto thresh_high_exp = find_and_parse_bounded_int<uint8_t, 0, 31>(carrier_obj, "thresh_x_high_p");
    if (!thresh_high_exp) {
      return make_unexpected(thresh_high_exp.error());
    }

    auto thresh_low_exp = find_and_parse_bounded_int<uint8_t, 0, 31>(carrier_obj, "thresh_x_low_p");
    if (!thresh_low_exp) {
      return make_unexpected(thresh_low_exp.error());
    }

    inter_freq_carrier_freq_info carrier;
    carrier.arfcn                      = static_cast<uint32_t>(arfcn_val);
    carrier.ssb_scs                    = scs_exp.value();
    carrier.derive_ssb_index_from_cell = derive_it->get<bool>();
    carrier.q_rx_lev_min               = q_rx_lev_min_exp.value();
    carrier.thresh_x_high_p            = thresh_high_exp.value();
    carrier.thresh_x_low_p             = thresh_low_exp.value();

    // q_offset_freq is optional (default db0).
    auto q_offset_freq_it = carrier_obj.find("q_offset_freq");
    if (q_offset_freq_it != carrier_obj.end()) {
      auto q_offset_freq_exp = parse_q_offset_range(*q_offset_freq_it, "q_offset_freq");
      if (!q_offset_freq_exp) {
        return make_unexpected(q_offset_freq_exp.error());
      }
      carrier.q_offset_freq = q_offset_freq_exp.value();
    }

    sib4.inter_freq_carrier_freq_list.push_back(carrier);
  }

  return sib4;
}

error_type<std::string> sib_update_remote_command::execute(const nlohmann::json& json)
{
  auto cells_key = json.find("cells");
  if (cells_key == json.end()) {
    return make_unexpected("'cells' object is missing and it is mandatory");
  }
  if (!cells_key->is_array()) {
    return make_unexpected("'cells' object value type should be an array");
  }
  auto cells_items = cells_key->items();
  if (cells_items.begin() == cells_items.end()) {
    return make_unexpected("'cells' object does not contain any cell entries");
  }

  odu::du_param_config_request req;
  for (const auto& cell : cells_items) {
    const auto& cell_obj = cell.value();

    auto cgi_exp = parse_cgi(cell_obj);
    if (!cgi_exp) {
      return make_unexpected(cgi_exp.error());
    }

    auto sib_key = cell_obj.find("sib");
    if (sib_key == cell_obj.end()) {
      return make_unexpected("'sib' object is missing and it is mandatory");
    }
    if (!sib_key->is_object()) {
      return make_unexpected("'sib' object value type should be an object");
    }

    auto type_key = sib_key->find("type");
    if (type_key == sib_key->end()) {
      return make_unexpected("'sib.type' field is missing and it is mandatory");
    }
    if (!type_key->is_string()) {
      return make_unexpected("'sib.type' value type should be a string");
    }

    auto content_key = sib_key->find("content");
    if (content_key == sib_key->end()) {
      return make_unexpected("'sib.content' field is missing and it is mandatory");
    }
    if (!content_key->is_object()) {
      return make_unexpected("'sib.content' value type should be an object");
    }

    std::string sib_type_str = type_key->get<std::string>();
    sib_info    sib_variant;
    if (sib_type_str == "sib2") {
      auto parsed = parse_sib2(*content_key);
      if (!parsed) {
        return make_unexpected(parsed.error());
      }
      sib_variant = std::move(parsed.value());
    } else if (sib_type_str == "sib3") {
      auto parsed = parse_sib3(*content_key);
      if (!parsed) {
        return make_unexpected(parsed.error());
      }
      sib_variant = std::move(parsed.value());
    } else if (sib_type_str == "sib4") {
      auto parsed = parse_sib4(*content_key);
      if (!parsed) {
        return make_unexpected(parsed.error());
      }
      sib_variant = std::move(parsed.value());
    } else {
      return make_unexpected(
          fmt::format("unsupported 'sib.type' value '{}'; supported types: sib2, sib3, sib4", sib_type_str));
    }

    auto& cell_cfg        = req.cells.emplace_back();
    cell_cfg.nr_cgi       = cgi_exp.value();
    cell_cfg.new_sys_info = std::move(sib_variant);
  }

  if (configurator.handle_sync_operator_config(req).success) {
    return {};
  }

  return make_unexpected("SIB update command procedure failed to be applied by the DU");
}
