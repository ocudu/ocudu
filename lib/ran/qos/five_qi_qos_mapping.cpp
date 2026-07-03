// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/qos/five_qi_qos_mapping.h"
#include <unordered_map>

using namespace ocudu;

using qos_chars = standardized_qos_characteristics;
using flow_type = qos_flow_resource_type;

/// \brief Standardized 5QI to QoS characteristics mapping as per TS 23.501, table 5.7.4-1.
static const std::unordered_map<five_qi_t, qos_chars> five_qi_to_qos_mapping = {
    // clang-format off

    // GBR.
    {uint_to_five_qi(1),
     qos_chars{flow_type::gbr, qos_prio_level_t{20}, 100, packet_error_rate_t::make(1e-2), 2000, std::nullopt}},
    {uint_to_five_qi(2),
     qos_chars{flow_type::gbr, qos_prio_level_t{40}, 150, packet_error_rate_t::make(1e-3), 2000, std::nullopt}},
    {uint_to_five_qi(3),
     qos_chars{flow_type::gbr, qos_prio_level_t{30}, 50,  packet_error_rate_t::make(1e-3), 2000, std::nullopt}},
    {uint_to_five_qi(4),
     qos_chars{flow_type::gbr, qos_prio_level_t{50}, 300, packet_error_rate_t::make(1e-6), 2000, std::nullopt}},
    {uint_to_five_qi(65),
     qos_chars{flow_type::gbr,qos_prio_level_t{ 7},  75,  packet_error_rate_t::make(1e-2), 2000, std::nullopt}},
    {uint_to_five_qi(66),
     qos_chars{flow_type::gbr, qos_prio_level_t{20}, 100, packet_error_rate_t::make(1e-2), 2000, std::nullopt}},
    {uint_to_five_qi(67),
     qos_chars{flow_type::gbr, qos_prio_level_t{15}, 100, packet_error_rate_t::make(1e-3), 2000, std::nullopt}},
    // NOTE: 5QI 75 is not supported in 5GS per TS 23.501 table 5.7.4-1, NOTE 14 (used only for V2X messages over
    // MBMS bearers in EPS; value reserved). Kept for interoperability with cores that still signal it; do NOT add
    // bearer configurations for this 5QI.
    {uint_to_five_qi(75),
     qos_chars{flow_type::gbr, qos_prio_level_t{25}, 50, packet_error_rate_t::make(1e-2), 2000, std::nullopt}},
    {uint_to_five_qi(71),
     qos_chars{flow_type::gbr, qos_prio_level_t{56}, 150, packet_error_rate_t::make(1e-6), 2000, std::nullopt}},
    {uint_to_five_qi(72),
     qos_chars{flow_type::gbr, qos_prio_level_t{56}, 300, packet_error_rate_t::make(1e-4), 2000, std::nullopt}},
    {uint_to_five_qi(73),
     qos_chars{flow_type::gbr, qos_prio_level_t{56}, 300, packet_error_rate_t::make(1e-8), 2000, std::nullopt}},
    {uint_to_five_qi(74),
     qos_chars{flow_type::gbr, qos_prio_level_t{56}, 500, packet_error_rate_t::make(1e-8), 2000, std::nullopt}},
    {uint_to_five_qi(76),
     qos_chars{flow_type::gbr, qos_prio_level_t{56}, 500, packet_error_rate_t::make(1e-4), 2000, std::nullopt}},

    // Non-GBR.
    {uint_to_five_qi(5),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{10}, 100, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(6),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{60}, 300, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(7),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{70}, 100, packet_error_rate_t::make(1e-3), std::nullopt, std::nullopt}},
    {uint_to_five_qi(8),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{80}, 300, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(9),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{90}, 300, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(10),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{90}, 1100, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(69),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{5},  60,  packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(70),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{55}, 200, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(79),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{65}, 50,  packet_error_rate_t::make(1e-2), std::nullopt, std::nullopt}},
    {uint_to_five_qi(80),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{68}, 10,  packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},

    // Delay Critical GBR.
    {uint_to_five_qi(82),
     qos_chars{flow_type::delay_critical_gbr, qos_prio_level_t{19}, 10,  packet_error_rate_t::make(1e-4), 2000, 255}},
    {uint_to_five_qi(83),
     qos_chars{flow_type::delay_critical_gbr, qos_prio_level_t{22}, 10,  packet_error_rate_t::make(1e-4), 2000, 1354}},
    {uint_to_five_qi(84),
     qos_chars{flow_type::delay_critical_gbr, qos_prio_level_t{24}, 30,  packet_error_rate_t::make(1e-5), 2000, 1354}},
    {uint_to_five_qi(85),
     qos_chars{flow_type::delay_critical_gbr, qos_prio_level_t{21}, 5,   packet_error_rate_t::make(1e-5), 2000, 255}},
    {uint_to_five_qi(86),
     qos_chars{flow_type::delay_critical_gbr, qos_prio_level_t{18}, 5,   packet_error_rate_t::make(1e-4), 2000, 1354}},
    {uint_to_five_qi(87),
     qos_chars{flow_type::delay_critical_gbr, qos_prio_level_t{25}, 5,   packet_error_rate_t::make(1e-3), 2000, 500}},
    {uint_to_five_qi(88),
     qos_chars{flow_type::delay_critical_gbr, qos_prio_level_t{25}, 10,   packet_error_rate_t::make(1e-3), 2000, 1125}},
    {uint_to_five_qi(89),
     qos_chars{flow_type::delay_critical_gbr, qos_prio_level_t{25}, 15,   packet_error_rate_t::make(1e-4), 2000, 17000}},
    {uint_to_five_qi(90),
     qos_chars{flow_type::delay_critical_gbr, qos_prio_level_t{25}, 20,   packet_error_rate_t::make(1e-4), 2000, 63000}},
    // clang-format on
};

const qos_chars* ocudu::get_5qi_to_qos_characteristics_mapping(five_qi_t five_qi)
{
  const auto qos_char = five_qi_to_qos_mapping.find(five_qi);
  return qos_char != five_qi_to_qos_mapping.end() ? &qos_char->second : nullptr;
}
