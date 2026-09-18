// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_location_manager.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <algorithm>

using namespace ocudu;
using namespace ocudu::ocucp;

ue_location_manager::ue_location_manager() : logger(ocudulog::fetch_basic_logger("CU-CP")) {}

std::optional<ngap_cause_t> ue_location_manager::configure_location_reporting(const location_report_request& ctrl)
{
  using event_type = location_report_request::event_type;
  auto req_type    = ctrl.location_reporting_type;

  if (req_type == event_type::nulltype) {
    // Reject malformed requests.
    logger.error("Received malformed Location Report Request with event type = null");
    return std::nullopt;
  }

  if (req_type == event_type::direct) {
    // Ignore direct reports, those should be handled with get_direct_location_report(), not for configuration.
    logger.error("Direct type Location Report Request should not be used for location reporting configuration");
    return std::nullopt;
  }

  if (req_type == event_type::cancel_location_report_for_the_ue) {
    // Cancel location reporting for the UE.
    cfg.report_on_cell_change     = false;
    cfg.report_ue_presence_in_aoi = false;
    cfg.area_of_interest_list.clear();
  }

  if (req_type == event_type::stop_change_of_serve_cell) {
    // Cancel change of serve cell reporting for the UE.
    cfg.report_on_cell_change = false;
    return std::nullopt;
  }

  if (req_type == event_type::stop_ue_presence_in_area_of_interest) {
    // Cancel UE presence in area of interest reporting for requested report reference IDs.
    if (ctrl.location_report_ref_id_to_be_cancelled.has_value()) {
      cfg.area_of_interest_list.erase(ctrl.location_report_ref_id_to_be_cancelled.value());
    }
    for (const auto& ref_id : ctrl.additional_location_report_ref_ids_to_be_cancelled) {
      cfg.area_of_interest_list.erase(ref_id);
    }
    // Cancel UE presence in area of interest reporting for the UE if no configured areas left.
    if (cfg.area_of_interest_list.empty()) {
      cfg.report_ue_presence_in_aoi = false;
    }
    return std::nullopt;
  }

  if (req_type == event_type::change_of_serve_cell ||
      req_type == event_type::change_of_serving_cell_and_ue_presence_in_the_area_of_interest) {
    // Enable change of serve cell reporting for the UE.
    cfg.report_on_cell_change = true;
  }

  if (req_type == event_type::ue_presence_in_area_of_interest ||
      req_type == event_type::change_of_serving_cell_and_ue_presence_in_the_area_of_interest) {
    cfg.report_ue_presence_in_aoi = true;
    // Validate input, reject on ref_ids duplicated within the message or current configuration (TS 38.413 8.12.1.3).
    std::map<location_report_ref_id_t, area_of_interest> incoming;
    for (const auto& item : ctrl.area_of_interest_list) {
      if (!(incoming.emplace(item.location_report_ref_id, item.aio).second) ||
          cfg.area_of_interest_list.count(item.location_report_ref_id) != 0) {
        logger.error("Location Reporting Control contains duplicate or conflicting Location Reporting Reference IDs");
        return ngap_cause_radio_network_t::multiple_location_report_ref_id_instances;
      }
    }

    // Apply all validated entries.
    cfg.area_of_interest_list.insert(incoming.begin(), incoming.end());
  }

  return std::nullopt;
}

location_report_request::event_type ue_location_manager::get_current_location_reporting_type() const
{
  if (cfg.report_on_cell_change && cfg.report_ue_presence_in_aoi) {
    return location_report_request::event_type::change_of_serving_cell_and_ue_presence_in_the_area_of_interest;
  }
  if (cfg.report_on_cell_change) {
    return location_report_request::event_type::change_of_serve_cell;
  }
  if (cfg.report_ue_presence_in_aoi) {
    return location_report_request::event_type::ue_presence_in_area_of_interest;
  }
  return location_report_request::event_type::nulltype;
}

std::optional<location_report_request> ue_location_manager::get_location_reporting_request() const
{
  if (cfg.report_on_cell_change || cfg.report_ue_presence_in_aoi) {
    location_report_request req;
    req.location_reporting_type = get_current_location_reporting_type();
    req.location_report_area    = location_report_request::report_area::cell;
    for (const auto& [ref_id, aoi] : cfg.area_of_interest_list) {
      req.area_of_interest_list.push_back({aoi, ref_id});
    }
    return req;
  }
  return std::nullopt;
}

/// Whether the UE is reported to be in the tracking area \c tac. A UE Location Derived TAC is where the UE actually
/// is, TS 23.502 sec. 4.10, so it decides alone and the cell's broadcast TACs do not enter into it. Without one the
/// UE counts as being in every area the cell broadcasts.
static bool is_ue_in_tac(const cu_cp_user_location_info_nr& loc, tac_t tac)
{
  if (loc.ue_location_derived_tac.has_value()) {
    return loc.ue_location_derived_tac.value() == tac;
  }
  if (loc.tac_list.empty()) {
    return loc.tai.tac == tac;
  }
  return std::any_of(
      loc.tac_list.begin(), loc.tac_list.end(), [tac](tac_t broadcast_tac) { return broadcast_tac == tac; });
}

ue_presence ue_location_manager::check_ue_presence(const area_of_interest& aoi, const cu_cp_user_location_info_nr& loc)
{
  // TS 38.300 sec. 16.14.5: in an NTN cell, and where one is configured for the UE's position, the Cell Identity of an
  // Area of Interest is a Mapped Cell ID.
  const nr_cell_global_id_t reported_cgi{loc.nr_cgi.plmn_id, loc.mapped_nci.value_or(loc.nr_cgi.nci)};
  for (const auto& cell : aoi.cell_list) {
    // TODO: add handling for other types of CGIs
    if (cell == reported_cgi) {
      return ue_presence::in;
    }
  }

  for (const auto& tai : aoi.tai_list) {
    if (tai.plmn_id == loc.tai.plmn_id && is_ue_in_tac(loc, tai.tac)) {
      return ue_presence::in;
    }
  }

  for (const auto& ran_node : aoi.ran_node_list) {
    // TODO: add handling for other types of RAN Nodes
    if (ran_node.plmn_id == loc.nr_cgi.plmn_id &&
        ran_node.gnb_id == loc.nr_cgi.nci.gnb_id(ran_node.gnb_id.bit_length)) {
      return ue_presence::in;
    }
  }

  // Without the position, an NTN cell naming its areas by Mapped Cell ID cannot tell the UE is outside them,
  // TS 38.413 sec. 9.3.1.67. Only the cell list needs the position, so only it can leave the presence unknown.
  return (loc.mapped_nci_unknown && !aoi.cell_list.empty()) ? ue_presence::unknown : ue_presence::out;
}

std::optional<location_report>
ue_location_manager::get_location_report(cu_cp_ue_index_t                   ue_index,
                                         const cu_cp_user_location_info_nr& user_location_info)
{
  if (!cfg.report_on_cell_change && !cfg.report_ue_presence_in_aoi) {
    return std::nullopt;
  }

  // Suppress report if only change_of_serve_cell reporting is active and the location hasn't changed since the last
  // report. In an NTN cell the location also holds the TAC and the Mapped Cell ID derived from the UE's own position,
  // so moving between areas counts as a new location without a cell change; areas may share a TAC and name different
  // Mapped Cell IDs, TS 38.300 sec. 16.14.5 NOTE 2. Outside NTN neither is set and only the serving cell moves.
  if (cfg.report_on_cell_change && !cfg.report_ue_presence_in_aoi && cfg.last_reported_location.has_value() &&
      cfg.last_reported_location->nr_cgi == user_location_info.nr_cgi &&
      cfg.last_reported_location->ue_location_derived_tac == user_location_info.ue_location_derived_tac &&
      cfg.last_reported_location->mapped_nci == user_location_info.mapped_nci) {
    return std::nullopt;
  }

  location_report report;
  report.ue_index           = ue_index;
  report.user_location_info = user_location_info;

  // Build location_report_request that the report refers to from current configuration.
  report.request.location_reporting_type = get_current_location_reporting_type();
  report.request.location_report_area    = location_report_request::report_area::cell;
  for (const auto& [ref_id, aoi] : cfg.area_of_interest_list) {
    report.request.area_of_interest_list.push_back({aoi, ref_id});
  }

  report.ue_presence_in_area_of_interest_list = build_ue_presence_list(user_location_info);

  cfg.last_reported_location = user_location_info;
  return report;
}

location_report ue_location_manager::get_direct_location_report(cu_cp_ue_index_t                   ue_index,
                                                                const cu_cp_user_location_info_nr& user_location_info,
                                                                const location_report_request&     request)
{
  location_report report;
  report.ue_index                             = ue_index;
  report.user_location_info                   = user_location_info;
  report.request                              = request;
  report.ue_presence_in_area_of_interest_list = build_ue_presence_list(user_location_info);
  cfg.last_reported_location                  = user_location_info;
  return report;
}

std::optional<std::vector<ue_presence_in_area_of_interest_item>>
ue_location_manager::build_ue_presence_list(const cu_cp_user_location_info_nr& user_location_info) const
{
  if (!cfg.report_ue_presence_in_aoi || cfg.area_of_interest_list.empty()) {
    return std::nullopt;
  }
  std::vector<ue_presence_in_area_of_interest_item> list;
  list.reserve(cfg.area_of_interest_list.size());
  for (const auto& [ref_id, aoi] : cfg.area_of_interest_list) {
    list.push_back({ref_id, check_ue_presence(aoi, user_location_info)});
  }
  return list;
}
