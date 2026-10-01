// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/cu_cp_location_reporting_types.h"
#include <map>

namespace ocudu::ocucp {

using location_report_ref_id_t = uint8_t; // (1..64)

struct ue_location_manager_cfg {
  bool                                                 report_on_cell_change     = false;
  bool                                                 report_ue_presence_in_aoi = false;
  std::map<location_report_ref_id_t, area_of_interest> area_of_interest_list;
  std::optional<cu_cp_user_location_info_nr>           last_reported_location;
};

class ue_location_manager
{
public:
  ue_location_manager();

  /// \brief Restore location reporting state (e.g. after intra-CU handover).
  void set_config(const ue_location_manager_cfg& new_cfg) { cfg = new_cfg; }

  /// \brief Extract the current location reporting state for transfer.
  ue_location_manager_cfg get_config() const { return cfg; }

  /// \brief Store a location reporting configuration received from AMF.
  /// \returns nullopt on success, or a Location Reporting Failure Indication cause if the configuration failed.
  std::optional<ngap_cause_t> configure_location_reporting(const location_report_request& ctrl);

  /// \brief Build the location reporting request from stored configuration, to be sent in Xn Handover Request.
  /// \returns The location report request if location reporting is configured, nullopt otherwise.
  std::optional<location_report_request> get_location_reporting_request() const;

  /// \brief Build and return a location report, if location reporting is configured.
  std::optional<location_report> get_location_report(cu_cp_ue_index_t                   ue_index,
                                                     const cu_cp_user_location_info_nr& user_location_info);

  /// \brief Build and return a direct location report, using the provided request.
  location_report get_direct_location_report(cu_cp_ue_index_t                   ue_index,
                                             const cu_cp_user_location_info_nr& user_location_info,
                                             const location_report_request&     request);

private:
  location_report_request::event_type get_current_location_reporting_type() const;

  static ue_presence check_ue_presence(const area_of_interest& aoi, const cu_cp_user_location_info_nr& loc);

  std::optional<std::vector<ue_presence_in_area_of_interest_item>>
  build_ue_presence_list(const cu_cp_user_location_info_nr& user_location_info) const;

  ue_location_manager_cfg cfg;
  ocudulog::basic_logger& logger;
};

} // namespace ocudu::ocucp
