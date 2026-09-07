// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common.h" // IWYU pragma: keep
#include "lib/scheduler/trace/event_converter.h"
#include "roundtrip_test.h"
#include "ocudu/scheduler/scheduler_rach_handler.h"

namespace ocudu::schedtrace::roundtrip_test {

template <>
struct test_values<phy_time_unit> {
  static std::vector<phy_time_unit> get()
  {
    return {phy_time_unit::from_units_of_Tc(std::numeric_limits<int64_t>::min()),
            phy_time_unit::from_units_of_Tc(std::numeric_limits<int64_t>::max())};
  }
};

template <>
struct printer<phy_time_unit> {
  static std::string print(const phy_time_unit& value) { return fmt::format("{}Tc", value.to_Tc()); }
};

template <>
struct roundtrip_traits<rach_indication_message::preamble> {
  static constexpr auto members =
      std::make_tuple(field("preamble_id", &rach_indication_message::preamble::preamble_id),
                      field("tc_rnti", &rach_indication_message::preamble::tc_rnti),
                      field("time_advance", &rach_indication_message::preamble::time_advance));
};

template <>
struct roundtrip_traits<rach_indication_message::occasion> {
  static constexpr auto members =
      std::make_tuple(field("start_symbol", &rach_indication_message::occasion::start_symbol),
                      field("frequency_index", &rach_indication_message::occasion::frequency_index),
                      field("preambles", &rach_indication_message::occasion::preambles));
};

template <>
struct roundtrip_traits<rach_indication_message> {
  static constexpr auto members = std::make_tuple(field("cell_index", &rach_indication_message::cell_index),
                                                  field("slot_rx", &rach_indication_message::slot_rx),
                                                  field("occasions", &rach_indication_message::occasions));

  static rach_indication_message roundtrip(const rach_indication_message& rach)
  {
    return roundtrip_via<fbs::RachIndication>(
        rach, convert_rach_indication_to_fb, [&rach](rach_indication_message& out, const fbs::RachIndication& fb) {
          convert_fb_to_rach_indication(out, fb, rach.slot_rx.scs(), rach.cell_index);
        });
  }
};

} // namespace ocudu::schedtrace::roundtrip_test
