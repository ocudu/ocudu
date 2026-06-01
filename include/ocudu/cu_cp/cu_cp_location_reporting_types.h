// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/tai.h"
#include "fmt/base.h"
#include <optional>
#include <vector>

namespace ocudu::ocucp {

/// \brief Area of Interest, as defined in TS 38.413 9.3.1.66.
struct area_of_interest {
  std::vector<tai_t>               tai_list;
  std::vector<nr_cell_global_id_t> cell_list;
  std::vector<cu_cp_global_gnb_id> ran_node_list;
};

/// \brief Area of Interest item, as defined in TS 38.413 9.3.1.65.
struct area_of_interest_item {
  area_of_interest aio;
  uint8_t          location_report_ref_id = 1; // 1..64 (TS 38.413 9.3.1.76).
};

/// \brief UE Presence status, as defined in TS 38.413 9.3.1.67.
enum class ue_presence { in, out, unknown };

/// \brief UE Presence in Area of Interest item, as defined in TS 38.413 9.3.1.67.
struct ue_presence_in_area_of_interest_item {
  uint8_t     location_report_ref_id = 1; // 1..64 (TS 38.413 9.3.1.76).
  ue_presence ue_presence_in_aio     = ue_presence::unknown;
};

struct location_report_request {
  enum class event_type {
    direct,
    change_of_serve_cell,
    ue_presence_in_area_of_interest,
    stop_change_of_serve_cell,
    stop_ue_presence_in_area_of_interest,
    cancel_location_report_for_the_ue,
    // ...
    change_of_serving_cell_and_ue_presence_in_the_area_of_interest,
    nulltype
  };

  enum class report_area { cell };

  event_type                         location_reporting_type = event_type::nulltype;
  report_area                        location_report_area    = report_area::cell;
  std::vector<area_of_interest_item> area_of_interest_list;
  std::optional<uint8_t>             location_report_ref_id_to_be_cancelled;
  std::vector<uint8_t>               additional_location_report_ref_ids_to_be_cancelled;
};

struct location_report {
  cu_cp_ue_index_t                                                 ue_index = cu_cp_ue_index_t::invalid;
  cu_cp_user_location_info_nr                                      user_location_info;
  std::optional<std::vector<ue_presence_in_area_of_interest_item>> ue_presence_in_area_of_interest_list;
  // Location Reporting Request Type to which the Location Report refers to (build from multiple combined requests).
  location_report_request request;
};

struct location_report_failure_indication {
  cu_cp_ue_index_t ue_index = cu_cp_ue_index_t::invalid;
  ngap_cause_t     cause;
};

} // namespace ocudu::ocucp

template <>
struct fmt::formatter<ocudu::ocucp::location_report_request::event_type> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::ocucp::location_report_request::event_type& cfg, FormatContext& ctx) const
  {
    std::string_view str;
    switch (cfg) {
      case ocudu::ocucp::location_report_request::event_type::direct:
        str = "direct";
        break;
      case ocudu::ocucp::location_report_request::event_type::change_of_serve_cell:
        str = "change_of_serve_cell";
        break;
      case ocudu::ocucp::location_report_request::event_type::ue_presence_in_area_of_interest:
        str = "ue_presence_in_area_of_interest";
        break;
      case ocudu::ocucp::location_report_request::event_type::stop_change_of_serve_cell:
        str = "stop_change_of_serve_cell";
        break;
      case ocudu::ocucp::location_report_request::event_type::stop_ue_presence_in_area_of_interest:
        str = "stop_ue_presence_in_area_of_interest";
        break;
      case ocudu::ocucp::location_report_request::event_type::cancel_location_report_for_the_ue:
        str = "cancel_location_report_for_the_ue";
        break;
      case ocudu::ocucp::location_report_request::event_type::
          change_of_serving_cell_and_ue_presence_in_the_area_of_interest:
        str = "change_of_serving_cell_and_ue_presence_in_the_area_of_interest";
        break;
      default:
        str = "nulltype";
    }
    return fmt::format_to(ctx.out(), "{}", str);
  }
};
