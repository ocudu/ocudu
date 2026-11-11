// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/nrppa/nrppa_ies.h"
#include "ocudu/asn1/asn1_ap_codec.h"

using namespace asn1;
using namespace asn1::nrppa;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// TriggeringMessage ::= ENUMERATED
static const char*           enum_names_trigger_msg_e_values[] = {"initiating-message",
                                                                  "successful-outcome",
                                                                  "unsuccessful-outcome"};
const asn1::table::enum_desc trigger_msg_opts::desc            = {enum_names_trigger_msg_e_values, 3, "trigger_msg_e"};

// LocationUncertainty ::= SEQUENCE
static const asn1::table::seq_field seq_table_location_uncertainty_s_fields[] = {
    {offsetof(location_uncertainty_s, horizontal_uncertainty),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(location_uncertainty_s, horizontal_confidence),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(100),
     nullptr},
    {offsetof(location_uncertainty_s, vertical_uncertainty),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(location_uncertainty_s, vertical_confidence),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(100),
     nullptr},
    {offsetof(location_uncertainty_s, ie_exts),
     (int32_t)offsetof(location_uncertainty_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(location_uncertainty_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_location_uncertainty_s_names[] = {{"horizontalUncertainty", nullptr},
                                                                                 {"horizontalConfidence", nullptr},
                                                                                 {"verticalUncertainty", nullptr},
                                                                                 {"verticalConfidence", nullptr},
                                                                                 {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_location_uncertainty_s_opts[]  = {
    {4, (int32_t)offsetof(location_uncertainty_s, ie_exts_present)}};
const asn1::table::seq_desc location_uncertainty_s::desc = {
    {seq_table_location_uncertainty_s_fields, 5, UINT64_C(0xf), seq_table_location_uncertainty_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_location_uncertainty_s_names};

// RelativeGeodeticLocation ::= SEQUENCE
static const asn1::table::seq_field seq_table_relative_geodetic_location_s_fields[] = {
    {offsetof(relative_geodetic_location_s, milli_arc_second_units),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(relative_geodetic_location_s::milli_arc_second_units)>(),
     decltype(relative_geodetic_location_s::milli_arc_second_units)::has_ext,
     false,
     decltype(relative_geodetic_location_s::milli_arc_second_units)::nof_types,
     decltype(relative_geodetic_location_s::milli_arc_second_units)::nof_exts,
     nullptr,
     &relative_geodetic_location_s::milli_arc_second_units_opts::desc},
    {offsetof(relative_geodetic_location_s, height_units),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(relative_geodetic_location_s::height_units)>(),
     decltype(relative_geodetic_location_s::height_units)::has_ext,
     false,
     decltype(relative_geodetic_location_s::height_units)::nof_types,
     decltype(relative_geodetic_location_s::height_units)::nof_exts,
     nullptr,
     &relative_geodetic_location_s::height_units_opts::desc},
    {offsetof(relative_geodetic_location_s, delta_latitude),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i16,
     false,
     true,
     INT64_C(-1024),
     UINT64_C(1023),
     nullptr},
    {offsetof(relative_geodetic_location_s, delta_longitude),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i16,
     false,
     true,
     INT64_C(-1024),
     UINT64_C(1023),
     nullptr},
    {offsetof(relative_geodetic_location_s, delta_height),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i16,
     false,
     true,
     INT64_C(-1024),
     UINT64_C(1023),
     nullptr},
    {offsetof(relative_geodetic_location_s, location_uncertainty),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &location_uncertainty_s::desc},
    {offsetof(relative_geodetic_location_s, ie_exts),
     (int32_t)offsetof(relative_geodetic_location_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(relative_geodetic_location_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_relative_geodetic_location_s_names[] = {
    {"milli-Arc-SecondUnits", nullptr},
    {"heightUnits", nullptr},
    {"deltaLatitude", nullptr},
    {"deltaLongitude", nullptr},
    {"deltaHeight", nullptr},
    {"locationUncertainty", nullptr},
    {"iE-extensions", nullptr}};
static const asn1::table::seq_opt seq_table_relative_geodetic_location_s_opts[] = {
    {6, (int32_t)offsetof(relative_geodetic_location_s, ie_exts_present)}};
const asn1::table::seq_desc relative_geodetic_location_s::desc = {
    {seq_table_relative_geodetic_location_s_fields, 7, UINT64_C(0x3f), seq_table_relative_geodetic_location_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_relative_geodetic_location_s_names};

static const char*           enum_names_relative_geodetic_location_s_milli_arc_second_units_e__values[] = {"zerodot03",
                                                                                                           "zerodot3",
                                                                                                           "three"};
const asn1::table::enum_desc relative_geodetic_location_s::milli_arc_second_units_opts::desc            = {
    enum_names_relative_geodetic_location_s_milli_arc_second_units_e__values,
    3,
    "relative_geodetic_location_s::milli_arc_second_units_e_"};

static const char*           enum_names_relative_geodetic_location_s_height_units_e__values[] = {"mm", "cm", "m"};
const asn1::table::enum_desc relative_geodetic_location_s::height_units_opts::desc            = {
    enum_names_relative_geodetic_location_s_height_units_e__values,
    3,
    "relative_geodetic_location_s::height_units_e_"};

// RelativeCartesianLocation ::= SEQUENCE
static const asn1::table::seq_field seq_table_relative_cartesian_location_s_fields[] = {
    {offsetof(relative_cartesian_location_s, xy_zunit),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(relative_cartesian_location_s::xy_zunit)>(),
     decltype(relative_cartesian_location_s::xy_zunit)::has_ext,
     false,
     decltype(relative_cartesian_location_s::xy_zunit)::nof_types,
     decltype(relative_cartesian_location_s::xy_zunit)::nof_exts,
     nullptr,
     &relative_cartesian_location_s::xy_zunit_opts::desc},
    {offsetof(relative_cartesian_location_s, xvalue),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i32,
     false,
     true,
     INT64_C(-65536),
     UINT64_C(65535),
     nullptr},
    {offsetof(relative_cartesian_location_s, yvalue),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i32,
     false,
     true,
     INT64_C(-65536),
     UINT64_C(65535),
     nullptr},
    {offsetof(relative_cartesian_location_s, zvalue),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i32,
     false,
     true,
     INT64_C(-32768),
     UINT64_C(32767),
     nullptr},
    {offsetof(relative_cartesian_location_s, location_uncertainty),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &location_uncertainty_s::desc},
    {offsetof(relative_cartesian_location_s, ie_exts),
     (int32_t)offsetof(relative_cartesian_location_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(relative_cartesian_location_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_relative_cartesian_location_s_names[] = {
    {"xYZunit", nullptr},
    {"xvalue", nullptr},
    {"yvalue", nullptr},
    {"zvalue", nullptr},
    {"locationUncertainty", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_relative_cartesian_location_s_opts[] = {
    {5, (int32_t)offsetof(relative_cartesian_location_s, ie_exts_present)}};
const asn1::table::seq_desc relative_cartesian_location_s::desc = {{seq_table_relative_cartesian_location_s_fields,
                                                                    6,
                                                                    UINT64_C(0x1f),
                                                                    seq_table_relative_cartesian_location_s_opts,
                                                                    1},
                                                                   true,
                                                                   true,
                                                                   nullptr,
                                                                   0,
                                                                   seq_table_relative_cartesian_location_s_names};

static const char*           enum_names_relative_cartesian_location_s_xy_zunit_e__values[] = {"mm", "cm", "dm"};
const asn1::table::enum_desc relative_cartesian_location_s::xy_zunit_opts::desc            = {
    enum_names_relative_cartesian_location_s_xy_zunit_e__values,
    3,
    "relative_cartesian_location_s::xy_zunit_e_"};

// ARPLocationType ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_arp_location_type_c_alternatives[] = {
    &asn1::table::alternative_ops<relative_geodetic_location_s, false>::value,
    &asn1::table::alternative_ops<relative_cartesian_location_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<arp_location_type_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc arp_location_type_c::storage_desc = {
    choice_storage_arp_location_type_c_alternatives,
    3,
    "arp_location_type_c"};

relative_geodetic_location_s& arp_location_type_c::set_arp_position_relative_geodetic()
{
  set(types::arp_position_relative_geodetic);
  return c.get<relative_geodetic_location_s>();
}
relative_cartesian_location_s& arp_location_type_c::set_arp_position_relative_cartesian()
{
  set(types::arp_position_relative_cartesian);
  return c.get<relative_cartesian_location_s>();
}
protocol_ie_single_container_s<arp_location_type_ext_ies_o>& arp_location_type_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<arp_location_type_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_arp_location_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &relative_geodetic_location_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &relative_cartesian_location_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<arp_location_type_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_arp_location_type_c_names[] = {
    {"aRPPositionRelativeGeodetic", nullptr},
    {"aRPPositionRelativeCartesian", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc arp_location_type_c::desc = {asn1::table::enum_int_type<arp_location_type_c::types>(),
                                                            arp_location_type_c::types::nof_types,
                                                            arp_location_type_c::types::nof_exts,
                                                            arp_location_type_c::types::has_ext,
                                                            true,
                                                            choice_table_arp_location_type_c_alternatives,
                                                            nullptr,
                                                            choice_table_arp_location_type_c_names,
                                                            "arp_location_type_c",
                                                            offsetof(arp_location_type_c, c.type),
                                                            offsetof(arp_location_type_c, c.value),
                                                            false,
                                                            &arp_location_type_c::storage_desc};

static const char*           enum_names_arp_location_type_c_types_values[] = {"aRPPositionRelativeGeodetic",
                                                                              "aRPPositionRelativeCartesian",
                                                                              "choice-extension"};
const asn1::table::enum_desc arp_location_type_c::types_opts::desc = {enum_names_arp_location_type_c_types_values,
                                                                      3,
                                                                      "arp_location_type_c::types"};

// ARPLocationInformation-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_arp_location_info_item_s_fields[] = {
    {offsetof(arp_location_info_item_s, arp_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(16),
     nullptr},
    {offsetof(arp_location_info_item_s, arp_location_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &arp_location_type_c::desc},
    {offsetof(arp_location_info_item_s, ie_exts),
     (int32_t)offsetof(arp_location_info_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(arp_location_info_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_arp_location_info_item_s_names[] = {{"aRP-ID", nullptr},
                                                                                   {"aRPLocationType", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_arp_location_info_item_s_opts[]  = {
    {2, (int32_t)offsetof(arp_location_info_item_s, ie_exts_present)}};
const asn1::table::seq_desc arp_location_info_item_s::desc = {
    {seq_table_arp_location_info_item_s_fields, 3, UINT64_C(0x3), seq_table_arp_location_info_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_arp_location_info_item_s_names};

// SCS-SpecificCarrier ::= SEQUENCE
static const asn1::table::seq_field seq_table_scs_specific_carrier_s_fields[] = {
    {offsetof(scs_specific_carrier_s, offset_to_carrier),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(2199),
     nullptr},
    {offsetof(scs_specific_carrier_s, subcarrier_spacing),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(scs_specific_carrier_s::subcarrier_spacing)>(),
     decltype(scs_specific_carrier_s::subcarrier_spacing)::has_ext,
     false,
     decltype(scs_specific_carrier_s::subcarrier_spacing)::nof_types,
     decltype(scs_specific_carrier_s::subcarrier_spacing)::nof_exts,
     nullptr,
     &scs_specific_carrier_s::subcarrier_spacing_opts::desc},
    {offsetof(scs_specific_carrier_s, carrier_bw),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(275),
     nullptr},
    {offsetof(scs_specific_carrier_s, ie_exts),
     (int32_t)offsetof(scs_specific_carrier_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(scs_specific_carrier_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_scs_specific_carrier_s_names[] = {{"offsetToCarrier", nullptr},
                                                                                 {"subcarrierSpacing", nullptr},
                                                                                 {"carrierBandwidth", nullptr},
                                                                                 {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_scs_specific_carrier_s_opts[]  = {
    {3, (int32_t)offsetof(scs_specific_carrier_s, ie_exts_present)}};
const asn1::table::seq_desc scs_specific_carrier_s::desc = {
    {seq_table_scs_specific_carrier_s_fields, 4, UINT64_C(0x7), seq_table_scs_specific_carrier_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_scs_specific_carrier_s_names};

static const char* enum_names_scs_specific_carrier_s_subcarrier_spacing_e__values[] =
    {"kHz15", "kHz30", "kHz60", "kHz120", "kHz480", "kHz960"};
const asn1::table::enum_desc scs_specific_carrier_s::subcarrier_spacing_opts::desc = {
    enum_names_scs_specific_carrier_s_subcarrier_spacing_e__values,
    6,
    "scs_specific_carrier_s::subcarrier_spacing_e_"};
uint16_t scs_specific_carrier_s::subcarrier_spacing_opts::to_number() const
{
  static const uint16_t numbers[] = {15, 30, 60, 120, 480, 960};
  return map_enum_number(numbers, 6, value, "scs_specific_carrier_s::subcarrier_spacing_e_");
}

// Combined-PosSRSResourceSet-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_combined_pos_srs_res_set_item_s_fields[] = {
    {offsetof(combined_pos_srs_res_set_item_s, point_a),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(combined_pos_srs_res_set_item_s, pci_nr),
     (int32_t)offsetof(combined_pos_srs_res_set_item_s, pci_nr_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(combined_pos_srs_res_set_item_s, possrs_res_set_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(15),
     nullptr},
    {offsetof(combined_pos_srs_res_set_item_s, scs_specific_carrier),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &scs_specific_carrier_s::desc},
    {offsetof(combined_pos_srs_res_set_item_s, ie_exts),
     (int32_t)offsetof(combined_pos_srs_res_set_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(combined_pos_srs_res_set_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_combined_pos_srs_res_set_item_s_names[] = {
    {"pointA", nullptr},
    {"pCI-NR", nullptr},
    {"possrsResourceSetID", nullptr},
    {"scs-specificCarrier", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_combined_pos_srs_res_set_item_s_opts[] = {
    {1, (int32_t)offsetof(combined_pos_srs_res_set_item_s, pci_nr_present)},
    {4, (int32_t)offsetof(combined_pos_srs_res_set_item_s, ie_exts_present)}};
const asn1::table::seq_desc combined_pos_srs_res_set_item_s::desc = {{seq_table_combined_pos_srs_res_set_item_s_fields,
                                                                      5,
                                                                      UINT64_C(0xd),
                                                                      seq_table_combined_pos_srs_res_set_item_s_opts,
                                                                      2},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_combined_pos_srs_res_set_item_s_names};

// PosSRSResourceSet-Aggregation-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pos_srs_res_set_aggregation_item_s_fields[] = {
    {offsetof(pos_srs_res_set_aggregation_item_s, combined_pos_srs_res_set_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     2,
     3,
     &asn1::table::item_list_codec<decltype(pos_srs_res_set_aggregation_item_s::combined_pos_srs_res_set_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &combined_pos_srs_res_set_item_s::desc},
    {offsetof(pos_srs_res_set_aggregation_item_s, ie_exts),
     (int32_t)offsetof(pos_srs_res_set_aggregation_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pos_srs_res_set_aggregation_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pos_srs_res_set_aggregation_item_s_names[] = {
    {"combined-posSRSResourceSet-List", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pos_srs_res_set_aggregation_item_s_opts[] = {
    {1, (int32_t)offsetof(pos_srs_res_set_aggregation_item_s, ie_exts_present)}};
const asn1::table::seq_desc pos_srs_res_set_aggregation_item_s::desc = {
    {seq_table_pos_srs_res_set_aggregation_item_s_fields,
     2,
     UINT64_C(0x1),
     seq_table_pos_srs_res_set_aggregation_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_pos_srs_res_set_aggregation_item_s_names};

// AbortTransmission-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_abort_tx_ext_ies_o[]    = {122};
static const uint8_t  objset_crit_abort_tx_ext_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_abort_tx_ext_ies_o[] = {
    abort_tx_ext_ies_o::value_c::types::pos_srs_res_set_aggregation_list};
static const uint8_t                    objset_presence_abort_tx_ext_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_abort_tx_ext_ies_o[]  = {
    {objset_crit_abort_tx_ext_ies_o, crit_e::nulltype},
    {objset_value_abort_tx_ext_ies_o, abort_tx_ext_ies_o::value_c::types::nulltype},
    {objset_presence_abort_tx_ext_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc abort_tx_ext_ies_o::desc = {objset_id_abort_tx_ext_ies_o,
                                                           1,
                                                           "id",
                                                           objset_columns_abort_tx_ext_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_abort_tx_ext_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::list_codec<pos_srs_res_set_aggregation_list_l, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_abort_tx_ext_ies_o_value_c_names[] = {
    {"PosSRSResourceSet-Aggregation-List", nullptr}};
const asn1::table::choice_desc abort_tx_ext_ies_o::value_c::desc = {
    asn1::table::enum_int_type<abort_tx_ext_ies_o::value_c::types>(),
    abort_tx_ext_ies_o::value_c::types::nof_types,
    abort_tx_ext_ies_o::value_c::types::nof_exts,
    abort_tx_ext_ies_o::value_c::types::has_ext,
    true,
    choice_table_abort_tx_ext_ies_o_value_c_alternatives,
    nullptr,
    choice_table_abort_tx_ext_ies_o_value_c_names,
    "abort_tx_ext_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(abort_tx_ext_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_abort_tx_ext_ies_o_value_c_types_values[]    = {"PosSRSResourceSet-Aggregation-List"};
const asn1::table::enum_desc abort_tx_ext_ies_o::value_c::types_opts::desc = {
    enum_names_abort_tx_ext_ies_o_value_c_types_values,
    1,
    "abort_tx_ext_ies_o::value_c::types"};

// AbortTransmission ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_abort_tx_c_alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    nullptr,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<abort_tx_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc abort_tx_c::storage_desc = {choice_storage_abort_tx_c_alternatives,
                                                                   3,
                                                                   "abort_tx_c"};

uint8_t& abort_tx_c::set_deactiv_srs_res_set_id()
{
  set(types::deactiv_srs_res_set_id);
  return c.get<uint8_t>();
}
void abort_tx_c::set_release_all()
{
  set(types::release_all);
}
protocol_ie_single_container_s<abort_tx_ext_ies_o>& abort_tx_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<abort_tx_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_abort_tx_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(0),
     UINT64_C(15),
     nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<abort_tx_ext_ies_o>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_abort_tx_c_names[] = {{"deactivateSRSResourceSetID", nullptr},
                                                                        {"releaseALL", nullptr},
                                                                        {"choice-extension", nullptr}};
const asn1::table::choice_desc       abort_tx_c::desc                = {asn1::table::enum_int_type<abort_tx_c::types>(),
                                                                        abort_tx_c::types::nof_types,
                                                                        abort_tx_c::types::nof_exts,
                                                                        abort_tx_c::types::has_ext,
                                                                        true,
                                                                        choice_table_abort_tx_c_alternatives,
                                                                        nullptr,
                                                                        choice_table_abort_tx_c_names,
                                                                        "abort_tx_c",
                                                                        offsetof(abort_tx_c, c.type),
                                                                        offsetof(abort_tx_c, c.value),
                                                                        false,
                                                                        &abort_tx_c::storage_desc};

static const char*           enum_names_abort_tx_c_types_values[] = {"deactivateSRSResourceSetID",
                                                                     "releaseALL",
                                                                     "choice-extension"};
const asn1::table::enum_desc abort_tx_c::types_opts::desc         = {enum_names_abort_tx_c_types_values,
                                                                     3,
                                                                     "abort_tx_c::types"};

// TransmissionCombn8 ::= SEQUENCE
static const asn1::table::seq_field seq_table_tx_combn8_s_fields[] = {
    {offsetof(tx_combn8_s, comb_offset_n8),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(tx_combn8_s, cyclic_shift_n8),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(5),
     nullptr},
    {offsetof(tx_combn8_s, ie_exts),
     (int32_t)offsetof(tx_combn8_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tx_combn8_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_tx_combn8_s_names[] = {{"combOffset-n8", nullptr},
                                                                      {"cyclicShift-n8", nullptr},
                                                                      {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_tx_combn8_s_opts[] = {{2, (int32_t)offsetof(tx_combn8_s, ie_exts_present)}};
const asn1::table::seq_desc       tx_combn8_s::desc            = {
    {seq_table_tx_combn8_s_fields, 3, UINT64_C(0x3), seq_table_tx_combn8_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_tx_combn8_s_names};

// TransmissionComb-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_tx_comb_ext_ies_o[]       = {111};
static const uint8_t  objset_crit_tx_comb_ext_ies_o[]     = {crit_e::reject};
static const uint8_t  objset_value_tx_comb_ext_ies_o[]    = {tx_comb_ext_ies_o::value_c::types::tx_combn8};
static const uint8_t  objset_presence_tx_comb_ext_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_tx_comb_ext_ies_o[] = {
    {objset_crit_tx_comb_ext_ies_o, crit_e::nulltype},
    {objset_value_tx_comb_ext_ies_o, tx_comb_ext_ies_o::value_c::types::nulltype},
    {objset_presence_tx_comb_ext_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc tx_comb_ext_ies_o::desc = {objset_id_tx_comb_ext_ies_o,
                                                          1,
                                                          "id",
                                                          objset_columns_tx_comb_ext_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_tx_comb_ext_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tx_combn8_s::desc}};
static const asn1::table::field_name choice_table_tx_comb_ext_ies_o_value_c_names[] = {{"TransmissionCombn8", nullptr}};
const asn1::table::choice_desc       tx_comb_ext_ies_o::value_c::desc               = {
    asn1::table::enum_int_type<tx_comb_ext_ies_o::value_c::types>(),
    tx_comb_ext_ies_o::value_c::types::nof_types,
    tx_comb_ext_ies_o::value_c::types::nof_exts,
    tx_comb_ext_ies_o::value_c::types::has_ext,
    true,
    choice_table_tx_comb_ext_ies_o_value_c_alternatives,
    nullptr,
    choice_table_tx_comb_ext_ies_o_value_c_names,
    "tx_comb_ext_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(tx_comb_ext_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_tx_comb_ext_ies_o_value_c_types_values[] = {"TransmissionCombn8"};
const asn1::table::enum_desc tx_comb_ext_ies_o::value_c::types_opts::desc        = {
    enum_names_tx_comb_ext_ies_o_value_c_types_values,
    1,
    "tx_comb_ext_ies_o::value_c::types"};
uint8_t tx_comb_ext_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {8};
  return map_enum_number(numbers, 1, value, "tx_comb_ext_ies_o::value_c::types");
}

// TransmissionComb ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_tx_comb_c_alternatives[] = {
    &asn1::table::alternative_ops<tx_comb_c::n2_s_, false>::value,
    &asn1::table::alternative_ops<tx_comb_c::n4_s_, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<tx_comb_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc tx_comb_c::storage_desc = {choice_storage_tx_comb_c_alternatives,
                                                                  3,
                                                                  "tx_comb_c"};

tx_comb_c::n2_s_& tx_comb_c::set_n2()
{
  set(types::n2);
  return c.get<n2_s_>();
}
tx_comb_c::n4_s_& tx_comb_c::set_n4()
{
  set(types::n4);
  return c.get<n4_s_>();
}
protocol_ie_single_container_s<tx_comb_ext_ies_o>& tx_comb_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<tx_comb_ext_ies_o>>();
}
static const asn1::table::seq_field  seq_table_tx_comb_c_n2_s__fields[] = {{offsetof(tx_comb_c::n2_s_, comb_offset_n2),
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::field_op::int_u8,
                                                                            false,
                                                                            true,
                                                                            INT64_C(0),
                                                                            UINT64_C(1),
                                                                            nullptr},
                                                                           {offsetof(tx_comb_c::n2_s_, cyclic_shift_n2),
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::field_op::int_u8,
                                                                            false,
                                                                            true,
                                                                            INT64_C(0),
                                                                            UINT64_C(7),
                                                                            nullptr}};
static const asn1::table::field_name seq_table_tx_comb_c_n2_s__names[]  = {{"combOffset-n2", nullptr},
                                                                           {"cyclicShift-n2", nullptr}};
static const asn1::table::seq_desc   seq_table_tx_comb_c_n2_s_          = {
    {seq_table_tx_comb_c_n2_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    true,
    nullptr,
    0,
    seq_table_tx_comb_c_n2_s__names};
static const asn1::table::seq_field  seq_table_tx_comb_c_n4_s__fields[] = {{offsetof(tx_comb_c::n4_s_, comb_offset_n4),
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::field_op::int_u8,
                                                                            false,
                                                                            true,
                                                                            INT64_C(0),
                                                                            UINT64_C(3),
                                                                            nullptr},
                                                                           {offsetof(tx_comb_c::n4_s_, cyclic_shift_n4),
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::field_op::int_u8,
                                                                            false,
                                                                            true,
                                                                            INT64_C(0),
                                                                            UINT64_C(11),
                                                                            nullptr}};
static const asn1::table::field_name seq_table_tx_comb_c_n4_s__names[]  = {{"combOffset-n4", nullptr},
                                                                           {"cyclicShift-n4", nullptr}};
static const asn1::table::seq_desc   seq_table_tx_comb_c_n4_s_          = {
    {seq_table_tx_comb_c_n4_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    true,
    nullptr,
    0,
    seq_table_tx_comb_c_n4_s__names};
static const asn1::table::seq_field choice_table_tx_comb_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_tx_comb_c_n2_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_tx_comb_c_n4_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<tx_comb_ext_ies_o>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_tx_comb_c_names[] = {{"n2", nullptr},
                                                                       {"n4", nullptr},
                                                                       {"choice-extension", nullptr}};
const asn1::table::choice_desc       tx_comb_c::desc                = {asn1::table::enum_int_type<tx_comb_c::types>(),
                                                                       tx_comb_c::types::nof_types,
                                                                       tx_comb_c::types::nof_exts,
                                                                       tx_comb_c::types::has_ext,
                                                                       true,
                                                                       choice_table_tx_comb_c_alternatives,
                                                                       nullptr,
                                                                       choice_table_tx_comb_c_names,
                                                                       "tx_comb_c",
                                                                       offsetof(tx_comb_c, c.type),
                                                                       offsetof(tx_comb_c, c.value),
                                                                       false,
                                                                       &tx_comb_c::storage_desc};

static const char*           enum_names_tx_comb_c_types_values[] = {"n2", "n4", "choice-extension"};
const asn1::table::enum_desc tx_comb_c::types_opts::desc = {enum_names_tx_comb_c_types_values, 3, "tx_comb_c::types"};
uint8_t                      tx_comb_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "tx_comb_c::types");
}

// ResourceTypePeriodic ::= SEQUENCE
static const asn1::table::seq_field seq_table_res_type_periodic_s_fields[] = {
    {offsetof(res_type_periodic_s, periodicity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(res_type_periodic_s::periodicity)>(),
     decltype(res_type_periodic_s::periodicity)::has_ext,
     false,
     decltype(res_type_periodic_s::periodicity)::nof_types,
     decltype(res_type_periodic_s::periodicity)::nof_exts,
     nullptr,
     &res_type_periodic_s::periodicity_opts::desc},
    {offsetof(res_type_periodic_s, offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(2559),
     nullptr},
    {offsetof(res_type_periodic_s, ie_exts),
     (int32_t)offsetof(res_type_periodic_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(res_type_periodic_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_res_type_periodic_s_names[] = {{"periodicity", nullptr},
                                                                              {"offset", nullptr},
                                                                              {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_res_type_periodic_s_opts[]  = {
    {2, (int32_t)offsetof(res_type_periodic_s, ie_exts_present)}};
const asn1::table::seq_desc res_type_periodic_s::desc = {
    {seq_table_res_type_periodic_s_fields, 3, UINT64_C(0x3), seq_table_res_type_periodic_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_res_type_periodic_s_names};

static const char*           enum_names_res_type_periodic_s_periodicity_e__values[] = {"slot1",
                                                                                       "slot2",
                                                                                       "slot4",
                                                                                       "slot5",
                                                                                       "slot8",
                                                                                       "slot10",
                                                                                       "slot16",
                                                                                       "slot20",
                                                                                       "slot32",
                                                                                       "slot40",
                                                                                       "slot64",
                                                                                       "slot80",
                                                                                       "slot160",
                                                                                       "slot320",
                                                                                       "slot640",
                                                                                       "slot1280",
                                                                                       "slot2560"};
const asn1::table::enum_desc res_type_periodic_s::periodicity_opts::desc            = {
    enum_names_res_type_periodic_s_periodicity_e__values,
    17,
    "res_type_periodic_s::periodicity_e_"};
uint16_t res_type_periodic_s::periodicity_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 2, 4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 160, 320, 640, 1280, 2560};
  return map_enum_number(numbers, 17, value, "res_type_periodic_s::periodicity_e_");
}

// ResourceTypeSemi-persistent ::= SEQUENCE
static const asn1::table::seq_field seq_table_res_type_semi_persistent_s_fields[] = {
    {offsetof(res_type_semi_persistent_s, periodicity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(res_type_semi_persistent_s::periodicity)>(),
     decltype(res_type_semi_persistent_s::periodicity)::has_ext,
     false,
     decltype(res_type_semi_persistent_s::periodicity)::nof_types,
     decltype(res_type_semi_persistent_s::periodicity)::nof_exts,
     nullptr,
     &res_type_semi_persistent_s::periodicity_opts::desc},
    {offsetof(res_type_semi_persistent_s, offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(2559),
     nullptr},
    {offsetof(res_type_semi_persistent_s, ie_exts),
     (int32_t)offsetof(res_type_semi_persistent_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(res_type_semi_persistent_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_res_type_semi_persistent_s_names[] = {{"periodicity", nullptr},
                                                                                     {"offset", nullptr},
                                                                                     {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_res_type_semi_persistent_s_opts[]  = {
    {2, (int32_t)offsetof(res_type_semi_persistent_s, ie_exts_present)}};
const asn1::table::seq_desc res_type_semi_persistent_s::desc = {
    {seq_table_res_type_semi_persistent_s_fields, 3, UINT64_C(0x3), seq_table_res_type_semi_persistent_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_res_type_semi_persistent_s_names};

static const char*           enum_names_res_type_semi_persistent_s_periodicity_e__values[] = {"slot1",
                                                                                              "slot2",
                                                                                              "slot4",
                                                                                              "slot5",
                                                                                              "slot8",
                                                                                              "slot10",
                                                                                              "slot16",
                                                                                              "slot20",
                                                                                              "slot32",
                                                                                              "slot40",
                                                                                              "slot64",
                                                                                              "slot80",
                                                                                              "slot160",
                                                                                              "slot320",
                                                                                              "slot640",
                                                                                              "slot1280",
                                                                                              "slot2560"};
const asn1::table::enum_desc res_type_semi_persistent_s::periodicity_opts::desc            = {
    enum_names_res_type_semi_persistent_s_periodicity_e__values,
    17,
    "res_type_semi_persistent_s::periodicity_e_"};
uint16_t res_type_semi_persistent_s::periodicity_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 2, 4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 160, 320, 640, 1280, 2560};
  return map_enum_number(numbers, 17, value, "res_type_semi_persistent_s::periodicity_e_");
}

// ResourceTypeAperiodic ::= SEQUENCE
static const asn1::table::seq_field seq_table_res_type_aperiodic_s_fields[] = {
    {offsetof(res_type_aperiodic_s, aperiodic_res_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(res_type_aperiodic_s::aperiodic_res_type)>(),
     decltype(res_type_aperiodic_s::aperiodic_res_type)::has_ext,
     false,
     decltype(res_type_aperiodic_s::aperiodic_res_type)::nof_types,
     decltype(res_type_aperiodic_s::aperiodic_res_type)::nof_exts,
     nullptr,
     &res_type_aperiodic_s::aperiodic_res_type_opts::desc},
    {offsetof(res_type_aperiodic_s, ie_exts),
     (int32_t)offsetof(res_type_aperiodic_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(res_type_aperiodic_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_res_type_aperiodic_s_names[] = {{"aperiodicResourceType", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_res_type_aperiodic_s_opts[]  = {
    {1, (int32_t)offsetof(res_type_aperiodic_s, ie_exts_present)}};
const asn1::table::seq_desc res_type_aperiodic_s::desc = {
    {seq_table_res_type_aperiodic_s_fields, 2, UINT64_C(0x1), seq_table_res_type_aperiodic_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_res_type_aperiodic_s_names};

static const char*           enum_names_res_type_aperiodic_s_aperiodic_res_type_e__values[] = {"true"};
const asn1::table::enum_desc res_type_aperiodic_s::aperiodic_res_type_opts::desc            = {
    enum_names_res_type_aperiodic_s_aperiodic_res_type_e__values,
    1,
    "res_type_aperiodic_s::aperiodic_res_type_e_"};

// ResourceType ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_res_type_c_alternatives[] = {
    &asn1::table::alternative_ops<res_type_periodic_s, false>::value,
    &asn1::table::alternative_ops<res_type_semi_persistent_s, false>::value,
    &asn1::table::alternative_ops<res_type_aperiodic_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<res_type_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc res_type_c::storage_desc = {choice_storage_res_type_c_alternatives,
                                                                   4,
                                                                   "res_type_c"};

res_type_periodic_s& res_type_c::set_periodic()
{
  set(types::periodic);
  return c.get<res_type_periodic_s>();
}
res_type_semi_persistent_s& res_type_c::set_semi_persistent()
{
  set(types::semi_persistent);
  return c.get<res_type_semi_persistent_s>();
}
res_type_aperiodic_s& res_type_c::set_aperiodic()
{
  set(types::aperiodic);
  return c.get<res_type_aperiodic_s>();
}
protocol_ie_single_container_s<res_type_ext_ies_o>& res_type_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<res_type_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_res_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &res_type_periodic_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &res_type_semi_persistent_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &res_type_aperiodic_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<res_type_ext_ies_o>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_res_type_c_names[] = {{"periodic", nullptr},
                                                                        {"semi-persistent", nullptr},
                                                                        {"aperiodic", nullptr},
                                                                        {"choice-extension", nullptr}};
const asn1::table::choice_desc       res_type_c::desc                = {asn1::table::enum_int_type<res_type_c::types>(),
                                                                        res_type_c::types::nof_types,
                                                                        res_type_c::types::nof_exts,
                                                                        res_type_c::types::has_ext,
                                                                        true,
                                                                        choice_table_res_type_c_alternatives,
                                                                        nullptr,
                                                                        choice_table_res_type_c_names,
                                                                        "res_type_c",
                                                                        offsetof(res_type_c, c.type),
                                                                        offsetof(res_type_c, c.value),
                                                                        false,
                                                                        &res_type_c::storage_desc};

static const char*           enum_names_res_type_c_types_values[] = {"periodic",
                                                                     "semi-persistent",
                                                                     "aperiodic",
                                                                     "choice-extension"};
const asn1::table::enum_desc res_type_c::types_opts::desc         = {enum_names_res_type_c_types_values,
                                                                     4,
                                                                     "res_type_c::types"};

// NrofSymbolsExtended ::= ENUMERATED
static const char*           enum_names_nrof_symbols_extended_e_values[] = {"n8", "n10", "n12", "n14"};
const asn1::table::enum_desc nrof_symbols_extended_opts::desc            = {enum_names_nrof_symbols_extended_e_values,
                                                                            4,
                                                                            "nrof_symbols_extended_e"};
uint8_t                      nrof_symbols_extended_opts::to_number() const
{
  static const uint8_t numbers[] = {8, 10, 12, 14};
  return map_enum_number(numbers, 4, value, "nrof_symbols_extended_e");
}

// RepetitionFactorExtended ::= ENUMERATED
static const char* enum_names_repeat_factor_extended_e_values[] = {"n3", "n5", "n6", "n7", "n8", "n10", "n12", "n14"};
const asn1::table::enum_desc repeat_factor_extended_opts::desc  = {enum_names_repeat_factor_extended_e_values,
                                                                   8,
                                                                   "repeat_factor_extended_e"};
uint8_t                      repeat_factor_extended_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 5, 6, 7, 8, 10, 12, 14};
  return map_enum_number(numbers, 8, value, "repeat_factor_extended_e");
}

// StartRBHopping ::= ENUMERATED
static const char*           enum_names_start_rb_hop_e_values[] = {"enable"};
const asn1::table::enum_desc start_rb_hop_opts::desc = {enum_names_start_rb_hop_e_values, 1, "start_rb_hop_e"};

// StartRBIndex ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_start_rb_idx_c_alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<start_rb_idx_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc start_rb_idx_c::storage_desc = {choice_storage_start_rb_idx_c_alternatives,
                                                                       3,
                                                                       "start_rb_idx_c"};

uint8_t& start_rb_idx_c::set_freq_scaling_factor2()
{
  set(types::freq_scaling_factor2);
  return c.get<uint8_t>();
}
uint8_t& start_rb_idx_c::set_freq_scaling_factor4()
{
  set(types::freq_scaling_factor4);
  return c.get<uint8_t>();
}
protocol_ie_single_container_s<start_rb_idx_ext_ies_o>& start_rb_idx_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<start_rb_idx_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_start_rb_idx_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(1),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<start_rb_idx_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_start_rb_idx_c_names[] = {{"freqScalingFactor2", nullptr},
                                                                            {"freqScalingFactor4", nullptr},
                                                                            {"choice-extension", nullptr}};
const asn1::table::choice_desc       start_rb_idx_c::desc = {asn1::table::enum_int_type<start_rb_idx_c::types>(),
                                                             start_rb_idx_c::types::nof_types,
                                                             start_rb_idx_c::types::nof_exts,
                                                             start_rb_idx_c::types::has_ext,
                                                             true,
                                                             choice_table_start_rb_idx_c_alternatives,
                                                             nullptr,
                                                             choice_table_start_rb_idx_c_names,
                                                             "start_rb_idx_c",
                                                             offsetof(start_rb_idx_c, c.type),
                                                             offsetof(start_rb_idx_c, c.value),
                                                             false,
                                                             &start_rb_idx_c::storage_desc};

static const char*           enum_names_start_rb_idx_c_types_values[] = {"freqScalingFactor2",
                                                                         "freqScalingFactor4",
                                                                         "choice-extension"};
const asn1::table::enum_desc start_rb_idx_c::types_opts::desc         = {enum_names_start_rb_idx_c_types_values,
                                                                         3,
                                                                         "start_rb_idx_c::types"};
uint8_t                      start_rb_idx_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "start_rb_idx_c::types");
}

static const uint32_t objset_id_srs_res_ext_ies_o[]  = {107, 108, 109, 110};
static const uint8_t objset_crit_srs_res_ext_ies_o[] = {crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// SRSResource-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_srs_res_ext_ies_container_values[] = {
    {offsetof(srs_res_ext_ies_container, nrof_symbols_extended),
     (int32_t)offsetof(srs_res_ext_ies_container, nrof_symbols_extended_present),
     asn1::table::enum_op<decltype(srs_res_ext_ies_container::nrof_symbols_extended)>(),
     decltype(srs_res_ext_ies_container::nrof_symbols_extended)::has_ext,
     false,
     decltype(srs_res_ext_ies_container::nrof_symbols_extended)::nof_types,
     decltype(srs_res_ext_ies_container::nrof_symbols_extended)::nof_exts,
     nullptr,
     &nrof_symbols_extended_opts::desc},
    {offsetof(srs_res_ext_ies_container, repeat_factor_extended),
     (int32_t)offsetof(srs_res_ext_ies_container, repeat_factor_extended_present),
     asn1::table::enum_op<decltype(srs_res_ext_ies_container::repeat_factor_extended)>(),
     decltype(srs_res_ext_ies_container::repeat_factor_extended)::has_ext,
     false,
     decltype(srs_res_ext_ies_container::repeat_factor_extended)::nof_types,
     decltype(srs_res_ext_ies_container::repeat_factor_extended)::nof_exts,
     nullptr,
     &repeat_factor_extended_opts::desc},
    {offsetof(srs_res_ext_ies_container, start_rb_hop),
     (int32_t)offsetof(srs_res_ext_ies_container, start_rb_hop_present),
     asn1::table::enum_op<decltype(srs_res_ext_ies_container::start_rb_hop)>(),
     decltype(srs_res_ext_ies_container::start_rb_hop)::has_ext,
     false,
     decltype(srs_res_ext_ies_container::start_rb_hop)::nof_types,
     decltype(srs_res_ext_ies_container::start_rb_hop)::nof_exts,
     nullptr,
     &start_rb_hop_opts::desc},
    {offsetof(srs_res_ext_ies_container, start_rb_idx),
     (int32_t)offsetof(srs_res_ext_ies_container, start_rb_idx_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &start_rb_idx_c::desc}};
static const int32_t ie_table_srs_res_ext_ies_container_presences[] = {
    (int32_t)offsetof(srs_res_ext_ies_container, nrof_symbols_extended_present),
    (int32_t)offsetof(srs_res_ext_ies_container, repeat_factor_extended_present),
    (int32_t)offsetof(srs_res_ext_ies_container, start_rb_hop_present),
    (int32_t)offsetof(srs_res_ext_ies_container, start_rb_idx_present)};
const asn1::table::ie_container_desc srs_res_ext_ies_container::desc = {objset_id_srs_res_ext_ies_o,
                                                                        objset_crit_srs_res_ext_ies_o,
                                                                        ie_table_srs_res_ext_ies_container_values,
                                                                        ie_table_srs_res_ext_ies_container_presences,
                                                                        4,
                                                                        0,
                                                                        1,
                                                                        65535,
                                                                        true,
                                                                        "extensionValue"};

// SRSResource ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_res_s_fields[] = {
    {offsetof(srs_res_s, srs_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(srs_res_s, nrof_srs_ports),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_res_s::nrof_srs_ports)>(),
     decltype(srs_res_s::nrof_srs_ports)::has_ext,
     false,
     decltype(srs_res_s::nrof_srs_ports)::nof_types,
     decltype(srs_res_s::nrof_srs_ports)::nof_exts,
     nullptr,
     &srs_res_s::nrof_srs_ports_opts::desc},
    {offsetof(srs_res_s, tx_comb),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &tx_comb_c::desc},
    {offsetof(srs_res_s, start_position),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(13),
     nullptr},
    {offsetof(srs_res_s, nrof_symbols),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_res_s::nrof_symbols)>(),
     decltype(srs_res_s::nrof_symbols)::has_ext,
     false,
     decltype(srs_res_s::nrof_symbols)::nof_types,
     decltype(srs_res_s::nrof_symbols)::nof_exts,
     nullptr,
     &srs_res_s::nrof_symbols_opts::desc},
    {offsetof(srs_res_s, repeat_factor),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_res_s::repeat_factor)>(),
     decltype(srs_res_s::repeat_factor)::has_ext,
     false,
     decltype(srs_res_s::repeat_factor)::nof_types,
     decltype(srs_res_s::repeat_factor)::nof_exts,
     nullptr,
     &srs_res_s::repeat_factor_opts::desc},
    {offsetof(srs_res_s, freq_domain_position),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(67),
     nullptr},
    {offsetof(srs_res_s, freq_domain_shift),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(268),
     nullptr},
    {offsetof(srs_res_s, c_srs),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(srs_res_s, b_srs),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(srs_res_s, b_hop),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(srs_res_s, group_or_seq_hop),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_res_s::group_or_seq_hop)>(),
     decltype(srs_res_s::group_or_seq_hop)::has_ext,
     false,
     decltype(srs_res_s::group_or_seq_hop)::nof_types,
     decltype(srs_res_s::group_or_seq_hop)::nof_exts,
     nullptr,
     &srs_res_s::group_or_seq_hop_opts::desc},
    {offsetof(srs_res_s, res_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &res_type_c::desc},
    {offsetof(srs_res_s, seq_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {offsetof(srs_res_s, ie_exts),
     (int32_t)offsetof(srs_res_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_res_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_srs_res_s_names[] = {{"sRSResourceID", nullptr},
                                                                    {"nrofSRS-Ports", nullptr},
                                                                    {"transmissionComb", nullptr},
                                                                    {"startPosition", nullptr},
                                                                    {"nrofSymbols", nullptr},
                                                                    {"repetitionFactor", nullptr},
                                                                    {"freqDomainPosition", nullptr},
                                                                    {"freqDomainShift", nullptr},
                                                                    {"c-SRS", nullptr},
                                                                    {"b-SRS", nullptr},
                                                                    {"b-hop", nullptr},
                                                                    {"groupOrSequenceHopping", nullptr},
                                                                    {"resourceType", nullptr},
                                                                    {"sequenceId", nullptr},
                                                                    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_res_s_opts[] = {{14, (int32_t)offsetof(srs_res_s, ie_exts_present)}};
const asn1::table::seq_desc          srs_res_s::desc            = {
    {seq_table_srs_res_s_fields, 15, UINT64_C(0x3fff), seq_table_srs_res_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_srs_res_s_names};

static const char*           enum_names_srs_res_s_nrof_srs_ports_e__values[] = {"port1", "ports2", "ports4"};
const asn1::table::enum_desc srs_res_s::nrof_srs_ports_opts::desc = {enum_names_srs_res_s_nrof_srs_ports_e__values,
                                                                     3,
                                                                     "srs_res_s::nrof_srs_ports_e_"};
uint8_t                      srs_res_s::nrof_srs_ports_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "srs_res_s::nrof_srs_ports_e_");
}

static const char*           enum_names_srs_res_s_nrof_symbols_e__values[] = {"n1", "n2", "n4"};
const asn1::table::enum_desc srs_res_s::nrof_symbols_opts::desc = {enum_names_srs_res_s_nrof_symbols_e__values,
                                                                   3,
                                                                   "srs_res_s::nrof_symbols_e_"};
uint8_t                      srs_res_s::nrof_symbols_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "srs_res_s::nrof_symbols_e_");
}

static const char*           enum_names_srs_res_s_repeat_factor_e__values[] = {"n1", "n2", "n4"};
const asn1::table::enum_desc srs_res_s::repeat_factor_opts::desc = {enum_names_srs_res_s_repeat_factor_e__values,
                                                                    3,
                                                                    "srs_res_s::repeat_factor_e_"};
uint8_t                      srs_res_s::repeat_factor_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "srs_res_s::repeat_factor_e_");
}

static const char* enum_names_srs_res_s_group_or_seq_hop_e__values[] = {"neither", "groupHopping", "sequenceHopping"};
const asn1::table::enum_desc srs_res_s::group_or_seq_hop_opts::desc  = {enum_names_srs_res_s_group_or_seq_hop_e__values,
                                                                        3,
                                                                        "srs_res_s::group_or_seq_hop_e_"};

// TransmissionCombPos ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_tx_comb_pos_c_alternatives[] = {
    &asn1::table::alternative_ops<tx_comb_pos_c::n2_s_, false>::value,
    &asn1::table::alternative_ops<tx_comb_pos_c::n4_s_, false>::value,
    &asn1::table::alternative_ops<tx_comb_pos_c::n8_s_, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<tx_comb_pos_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc tx_comb_pos_c::storage_desc = {choice_storage_tx_comb_pos_c_alternatives,
                                                                      4,
                                                                      "tx_comb_pos_c"};

tx_comb_pos_c::n2_s_& tx_comb_pos_c::set_n2()
{
  set(types::n2);
  return c.get<n2_s_>();
}
tx_comb_pos_c::n4_s_& tx_comb_pos_c::set_n4()
{
  set(types::n4);
  return c.get<n4_s_>();
}
tx_comb_pos_c::n8_s_& tx_comb_pos_c::set_n8()
{
  set(types::n8);
  return c.get<n8_s_>();
}
protocol_ie_single_container_s<tx_comb_pos_ext_ies_o>& tx_comb_pos_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<tx_comb_pos_ext_ies_o>>();
}
static const asn1::table::seq_field seq_table_tx_comb_pos_c_n2_s__fields[] = {
    {offsetof(tx_comb_pos_c::n2_s_, comb_offset_n2),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(1),
     nullptr},
    {offsetof(tx_comb_pos_c::n2_s_, cyclic_shift_n2),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr}};
static const asn1::table::field_name seq_table_tx_comb_pos_c_n2_s__names[] = {{"combOffset-n2", nullptr},
                                                                              {"cyclicShift-n2", nullptr}};
static const asn1::table::seq_desc   seq_table_tx_comb_pos_c_n2_s_         = {
    {seq_table_tx_comb_pos_c_n2_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    true,
    nullptr,
    0,
    seq_table_tx_comb_pos_c_n2_s__names};
static const asn1::table::seq_field seq_table_tx_comb_pos_c_n4_s__fields[] = {
    {offsetof(tx_comb_pos_c::n4_s_, comb_offset_n4),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(tx_comb_pos_c::n4_s_, cyclic_shift_n4),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(11),
     nullptr}};
static const asn1::table::field_name seq_table_tx_comb_pos_c_n4_s__names[] = {{"combOffset-n4", nullptr},
                                                                              {"cyclicShift-n4", nullptr}};
static const asn1::table::seq_desc   seq_table_tx_comb_pos_c_n4_s_         = {
    {seq_table_tx_comb_pos_c_n4_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    true,
    nullptr,
    0,
    seq_table_tx_comb_pos_c_n4_s__names};
static const asn1::table::seq_field seq_table_tx_comb_pos_c_n8_s__fields[] = {
    {offsetof(tx_comb_pos_c::n8_s_, comb_offset_n8),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(tx_comb_pos_c::n8_s_, cyclic_shift_n8),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(5),
     nullptr}};
static const asn1::table::field_name seq_table_tx_comb_pos_c_n8_s__names[] = {{"combOffset-n8", nullptr},
                                                                              {"cyclicShift-n8", nullptr}};
static const asn1::table::seq_desc   seq_table_tx_comb_pos_c_n8_s_         = {
    {seq_table_tx_comb_pos_c_n8_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    true,
    nullptr,
    0,
    seq_table_tx_comb_pos_c_n8_s__names};
static const asn1::table::seq_field choice_table_tx_comb_pos_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_tx_comb_pos_c_n2_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_tx_comb_pos_c_n4_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_tx_comb_pos_c_n8_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<tx_comb_pos_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_tx_comb_pos_c_names[] = {{"n2", nullptr},
                                                                           {"n4", nullptr},
                                                                           {"n8", nullptr},
                                                                           {"choice-extension", nullptr}};
const asn1::table::choice_desc       tx_comb_pos_c::desc = {asn1::table::enum_int_type<tx_comb_pos_c::types>(),
                                                            tx_comb_pos_c::types::nof_types,
                                                            tx_comb_pos_c::types::nof_exts,
                                                            tx_comb_pos_c::types::has_ext,
                                                            true,
                                                            choice_table_tx_comb_pos_c_alternatives,
                                                            nullptr,
                                                            choice_table_tx_comb_pos_c_names,
                                                            "tx_comb_pos_c",
                                                            offsetof(tx_comb_pos_c, c.type),
                                                            offsetof(tx_comb_pos_c, c.value),
                                                            false,
                                                            &tx_comb_pos_c::storage_desc};

static const char*           enum_names_tx_comb_pos_c_types_values[] = {"n2", "n4", "n8", "choice-extension"};
const asn1::table::enum_desc tx_comb_pos_c::types_opts::desc         = {enum_names_tx_comb_pos_c_types_values,
                                                                        4,
                                                                        "tx_comb_pos_c::types"};
uint8_t                      tx_comb_pos_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "tx_comb_pos_c::types");
}

// SRSPeriodicity ::= ENUMERATED
static const char* enum_names_srs_periodicity_e_values[] = {
    "slot1",     "slot2",     "slot4",     "slot5",   "slot8",   "slot10",  "slot16",   "slot20",   "slot32",
    "slot40",    "slot64",    "slot80",    "slot160", "slot320", "slot640", "slot1280", "slot2560", "slot5120",
    "slot10240", "slot40960", "slot81920", "slot128", "slot256", "slot512", "slot20480"};
const asn1::table::enum_desc srs_periodicity_opts::desc = {enum_names_srs_periodicity_e_values,
                                                           25,
                                                           "srs_periodicity_e"};
uint32_t                     srs_periodicity_opts::to_number() const
{
  static const uint32_t numbers[] = {1,   2,   4,    5,    8,    10,    16,    20,    32,  40,  64,  80,   160,
                                     320, 640, 1280, 2560, 5120, 10240, 40960, 81920, 128, 256, 512, 20480};
  return map_enum_number(numbers, 25, value, "srs_periodicity_e");
}

// SRSPosPeriodicConfigHyperSFNIndex ::= ENUMERATED
static const char*           enum_names_srs_pos_periodic_cfg_hyper_sfn_idx_e_values[] = {"even0", "odd1"};
const asn1::table::enum_desc srs_pos_periodic_cfg_hyper_sfn_idx_opts::desc            = {
    enum_names_srs_pos_periodic_cfg_hyper_sfn_idx_e_values,
    2,
    "srs_pos_periodic_cfg_hyper_sfn_idx_e"};
uint8_t srs_pos_periodic_cfg_hyper_sfn_idx_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "srs_pos_periodic_cfg_hyper_sfn_idx_e");
}

static const uint32_t objset_id_res_type_periodic_pos_ext_ies_o[]   = {165};
static const uint8_t  objset_crit_res_type_periodic_pos_ext_ies_o[] = {crit_e::ignore};

// ResourceTypePeriodicPos-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_res_type_periodic_pos_ext_ies_container_values[] = {
    {offsetof(res_type_periodic_pos_ext_ies_container, srs_pos_periodic_cfg_hyper_sfn_idx),
     (int32_t)offsetof(res_type_periodic_pos_ext_ies_container, srs_pos_periodic_cfg_hyper_sfn_idx_present),
     asn1::table::enum_op<decltype(res_type_periodic_pos_ext_ies_container::srs_pos_periodic_cfg_hyper_sfn_idx)>(),
     decltype(res_type_periodic_pos_ext_ies_container::srs_pos_periodic_cfg_hyper_sfn_idx)::has_ext,
     false,
     decltype(res_type_periodic_pos_ext_ies_container::srs_pos_periodic_cfg_hyper_sfn_idx)::nof_types,
     decltype(res_type_periodic_pos_ext_ies_container::srs_pos_periodic_cfg_hyper_sfn_idx)::nof_exts,
     nullptr,
     &srs_pos_periodic_cfg_hyper_sfn_idx_opts::desc}};
static const int32_t ie_table_res_type_periodic_pos_ext_ies_container_presences[] = {
    (int32_t)offsetof(res_type_periodic_pos_ext_ies_container, srs_pos_periodic_cfg_hyper_sfn_idx_present)};
const asn1::table::ie_container_desc res_type_periodic_pos_ext_ies_container::desc = {
    objset_id_res_type_periodic_pos_ext_ies_o,
    objset_crit_res_type_periodic_pos_ext_ies_o,
    ie_table_res_type_periodic_pos_ext_ies_container_values,
    ie_table_res_type_periodic_pos_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// ResourceTypePeriodicPos ::= SEQUENCE
static const asn1::table::seq_field seq_table_res_type_periodic_pos_s_fields[] = {
    {offsetof(res_type_periodic_pos_s, srs_periodicity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(res_type_periodic_pos_s::srs_periodicity)>(),
     decltype(res_type_periodic_pos_s::srs_periodicity)::has_ext,
     false,
     decltype(res_type_periodic_pos_s::srs_periodicity)::nof_types,
     decltype(res_type_periodic_pos_s::srs_periodicity)::nof_exts,
     nullptr,
     &srs_periodicity_opts::desc},
    {offsetof(res_type_periodic_pos_s, offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(81919),
     nullptr},
    {offsetof(res_type_periodic_pos_s, ie_exts),
     (int32_t)offsetof(res_type_periodic_pos_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &res_type_periodic_pos_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_res_type_periodic_pos_s_names[] = {{"sRSPeriodicity", nullptr},
                                                                                  {"offset", nullptr},
                                                                                  {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_res_type_periodic_pos_s_opts[]  = {
    {2, (int32_t)offsetof(res_type_periodic_pos_s, ie_exts_present)}};
const asn1::table::seq_desc res_type_periodic_pos_s::desc = {
    {seq_table_res_type_periodic_pos_s_fields, 3, UINT64_C(0x3), seq_table_res_type_periodic_pos_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_res_type_periodic_pos_s_names};

static const uint32_t objset_id_res_type_semi_persistent_pos_ext_ies_o[]   = {165};
static const uint8_t  objset_crit_res_type_semi_persistent_pos_ext_ies_o[] = {crit_e::ignore};

// ResourceTypeSemi-persistentPos-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_res_type_semi_persistent_pos_ext_ies_container_values[] = {
    {offsetof(res_type_semi_persistent_pos_ext_ies_container, srs_pos_periodic_cfg_hyper_sfn_idx),
     (int32_t)offsetof(res_type_semi_persistent_pos_ext_ies_container, srs_pos_periodic_cfg_hyper_sfn_idx_present),
     asn1::table::enum_op<
         decltype(res_type_semi_persistent_pos_ext_ies_container::srs_pos_periodic_cfg_hyper_sfn_idx)>(),
     decltype(res_type_semi_persistent_pos_ext_ies_container::srs_pos_periodic_cfg_hyper_sfn_idx)::has_ext,
     false,
     decltype(res_type_semi_persistent_pos_ext_ies_container::srs_pos_periodic_cfg_hyper_sfn_idx)::nof_types,
     decltype(res_type_semi_persistent_pos_ext_ies_container::srs_pos_periodic_cfg_hyper_sfn_idx)::nof_exts,
     nullptr,
     &srs_pos_periodic_cfg_hyper_sfn_idx_opts::desc}};
static const int32_t ie_table_res_type_semi_persistent_pos_ext_ies_container_presences[] = {
    (int32_t)offsetof(res_type_semi_persistent_pos_ext_ies_container, srs_pos_periodic_cfg_hyper_sfn_idx_present)};
const asn1::table::ie_container_desc res_type_semi_persistent_pos_ext_ies_container::desc = {
    objset_id_res_type_semi_persistent_pos_ext_ies_o,
    objset_crit_res_type_semi_persistent_pos_ext_ies_o,
    ie_table_res_type_semi_persistent_pos_ext_ies_container_values,
    ie_table_res_type_semi_persistent_pos_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// ResourceTypeSemi-persistentPos ::= SEQUENCE
static const asn1::table::seq_field seq_table_res_type_semi_persistent_pos_s_fields[] = {
    {offsetof(res_type_semi_persistent_pos_s, srs_periodicity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(res_type_semi_persistent_pos_s::srs_periodicity)>(),
     decltype(res_type_semi_persistent_pos_s::srs_periodicity)::has_ext,
     false,
     decltype(res_type_semi_persistent_pos_s::srs_periodicity)::nof_types,
     decltype(res_type_semi_persistent_pos_s::srs_periodicity)::nof_exts,
     nullptr,
     &srs_periodicity_opts::desc},
    {offsetof(res_type_semi_persistent_pos_s, offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(81919),
     nullptr},
    {offsetof(res_type_semi_persistent_pos_s, ie_exts),
     (int32_t)offsetof(res_type_semi_persistent_pos_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &res_type_semi_persistent_pos_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_res_type_semi_persistent_pos_s_names[] = {{"sRSPeriodicity", nullptr},
                                                                                         {"offset", nullptr},
                                                                                         {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_res_type_semi_persistent_pos_s_opts[]  = {
    {2, (int32_t)offsetof(res_type_semi_persistent_pos_s, ie_exts_present)}};
const asn1::table::seq_desc res_type_semi_persistent_pos_s::desc = {{seq_table_res_type_semi_persistent_pos_s_fields,
                                                                     3,
                                                                     UINT64_C(0x3),
                                                                     seq_table_res_type_semi_persistent_pos_s_opts,
                                                                     1},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_res_type_semi_persistent_pos_s_names};

// ResourceTypeAperiodicPos ::= SEQUENCE
static const asn1::table::seq_field seq_table_res_type_aperiodic_pos_s_fields[] = {
    {offsetof(res_type_aperiodic_pos_s, slot_offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(32),
     nullptr},
    {offsetof(res_type_aperiodic_pos_s, ie_exts),
     (int32_t)offsetof(res_type_aperiodic_pos_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(res_type_aperiodic_pos_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_res_type_aperiodic_pos_s_names[] = {{"slotOffset", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_res_type_aperiodic_pos_s_opts[]  = {
    {1, (int32_t)offsetof(res_type_aperiodic_pos_s, ie_exts_present)}};
const asn1::table::seq_desc res_type_aperiodic_pos_s::desc = {
    {seq_table_res_type_aperiodic_pos_s_fields, 2, UINT64_C(0x1), seq_table_res_type_aperiodic_pos_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_res_type_aperiodic_pos_s_names};

// ResourceTypePos ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_res_type_pos_c_alternatives[] = {
    &asn1::table::alternative_ops<res_type_periodic_pos_s, false>::value,
    &asn1::table::alternative_ops<res_type_semi_persistent_pos_s, false>::value,
    &asn1::table::alternative_ops<res_type_aperiodic_pos_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<res_type_pos_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc res_type_pos_c::storage_desc = {choice_storage_res_type_pos_c_alternatives,
                                                                       4,
                                                                       "res_type_pos_c"};

res_type_periodic_pos_s& res_type_pos_c::set_periodic()
{
  set(types::periodic);
  return c.get<res_type_periodic_pos_s>();
}
res_type_semi_persistent_pos_s& res_type_pos_c::set_semi_persistent()
{
  set(types::semi_persistent);
  return c.get<res_type_semi_persistent_pos_s>();
}
res_type_aperiodic_pos_s& res_type_pos_c::set_aperiodic()
{
  set(types::aperiodic);
  return c.get<res_type_aperiodic_pos_s>();
}
protocol_ie_single_container_s<res_type_pos_ext_ies_o>& res_type_pos_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<res_type_pos_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_res_type_pos_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &res_type_periodic_pos_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &res_type_semi_persistent_pos_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &res_type_aperiodic_pos_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<res_type_pos_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_res_type_pos_c_names[] = {{"periodic", nullptr},
                                                                            {"semi-persistent", nullptr},
                                                                            {"aperiodic", nullptr},
                                                                            {"choice-extension", nullptr}};
const asn1::table::choice_desc       res_type_pos_c::desc = {asn1::table::enum_int_type<res_type_pos_c::types>(),
                                                             res_type_pos_c::types::nof_types,
                                                             res_type_pos_c::types::nof_exts,
                                                             res_type_pos_c::types::has_ext,
                                                             true,
                                                             choice_table_res_type_pos_c_alternatives,
                                                             nullptr,
                                                             choice_table_res_type_pos_c_names,
                                                             "res_type_pos_c",
                                                             offsetof(res_type_pos_c, c.type),
                                                             offsetof(res_type_pos_c, c.value),
                                                             false,
                                                             &res_type_pos_c::storage_desc};

static const char*           enum_names_res_type_pos_c_types_values[] = {"periodic",
                                                                         "semi-persistent",
                                                                         "aperiodic",
                                                                         "choice-extension"};
const asn1::table::enum_desc res_type_pos_c::types_opts::desc         = {enum_names_res_type_pos_c_types_values,
                                                                         4,
                                                                         "res_type_pos_c::types"};

// SSB ::= SEQUENCE
static const asn1::table::seq_field seq_table_ssb_s_fields[] = {
    {offsetof(ssb_s, pci_nr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(ssb_s, ssb_idx),
     (int32_t)offsetof(ssb_s, ssb_idx_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(ssb_s, ie_exts),
     (int32_t)offsetof(ssb_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ssb_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ssb_s_names[] = {{"pCI-NR", nullptr},
                                                                {"ssb-index", nullptr},
                                                                {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ssb_s_opts[]  = {{1, (int32_t)offsetof(ssb_s, ssb_idx_present)},
                                                                {2, (int32_t)offsetof(ssb_s, ie_exts_present)}};
const asn1::table::seq_desc          ssb_s::desc = {{seq_table_ssb_s_fields, 3, UINT64_C(0x1), seq_table_ssb_s_opts, 2},
                                                    true,
                                                    true,
                                                    nullptr,
                                                    0,
                                                    seq_table_ssb_s_names};

// PRSInformationPos ::= SEQUENCE
static const asn1::table::seq_field seq_table_pr_si_nformation_pos_s_fields[] = {
    {offsetof(pr_si_nformation_pos_s, prs_id_pos),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pr_si_nformation_pos_s, prs_res_set_id_pos),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(pr_si_nformation_pos_s, prs_res_id_pos),
     (int32_t)offsetof(pr_si_nformation_pos_s, prs_res_id_pos_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(pr_si_nformation_pos_s, ie_exts),
     (int32_t)offsetof(pr_si_nformation_pos_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pr_si_nformation_pos_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pr_si_nformation_pos_s_names[] = {{"pRS-IDPos", nullptr},
                                                                                 {"pRS-Resource-Set-IDPos", nullptr},
                                                                                 {"pRS-Resource-IDPos", nullptr},
                                                                                 {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_pr_si_nformation_pos_s_opts[]  = {
    {2, (int32_t)offsetof(pr_si_nformation_pos_s, prs_res_id_pos_present)},
    {3, (int32_t)offsetof(pr_si_nformation_pos_s, ie_exts_present)}};
const asn1::table::seq_desc pr_si_nformation_pos_s::desc = {
    {seq_table_pr_si_nformation_pos_s_fields, 4, UINT64_C(0x3), seq_table_pr_si_nformation_pos_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_pr_si_nformation_pos_s_names};

// SpatialRelationPos ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_spatial_relation_pos_c_alternatives[] = {
    &asn1::table::alternative_ops<ssb_s, false>::value,
    &asn1::table::alternative_ops<pr_si_nformation_pos_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<spatial_info_pos_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc spatial_relation_pos_c::storage_desc = {
    choice_storage_spatial_relation_pos_c_alternatives,
    3,
    "spatial_relation_pos_c"};

ssb_s& spatial_relation_pos_c::set_ssb_pos()
{
  set(types::ssb_pos);
  return c.get<ssb_s>();
}
pr_si_nformation_pos_s& spatial_relation_pos_c::set_pr_si_nformation_pos()
{
  set(types::pr_si_nformation_pos);
  return c.get<pr_si_nformation_pos_s>();
}
protocol_ie_single_container_s<spatial_info_pos_ext_ies_o>& spatial_relation_pos_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<spatial_info_pos_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_spatial_relation_pos_c_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::sequence, false, false, 0, 0, nullptr, &ssb_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pr_si_nformation_pos_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<spatial_info_pos_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_spatial_relation_pos_c_names[] = {{"sSBPos", nullptr},
                                                                                    {"pRSInformationPos", nullptr},
                                                                                    {"choice-extension", nullptr}};
const asn1::table::choice_desc       spatial_relation_pos_c::desc                = {
    asn1::table::enum_int_type<spatial_relation_pos_c::types>(),
    spatial_relation_pos_c::types::nof_types,
    spatial_relation_pos_c::types::nof_exts,
    spatial_relation_pos_c::types::has_ext,
    true,
    choice_table_spatial_relation_pos_c_alternatives,
    nullptr,
    choice_table_spatial_relation_pos_c_names,
    "spatial_relation_pos_c",
    offsetof(spatial_relation_pos_c, c.type),
    offsetof(spatial_relation_pos_c, c.value),
    false,
    &spatial_relation_pos_c::storage_desc};

static const char*           enum_names_spatial_relation_pos_c_types_values[] = {"sSBPos",
                                                                                 "pRSInformationPos",
                                                                                 "choice-extension"};
const asn1::table::enum_desc spatial_relation_pos_c::types_opts::desc = {enum_names_spatial_relation_pos_c_types_values,
                                                                         3,
                                                                         "spatial_relation_pos_c::types"};

// SlotOffsetRemainingHopsAperiodic ::= SEQUENCE
static const asn1::table::seq_field seq_table_slot_offset_remaining_hops_aperiodic_s_fields[] = {
    {offsetof(slot_offset_remaining_hops_aperiodic_s, slot_offset),
     (int32_t)offsetof(slot_offset_remaining_hops_aperiodic_s, slot_offset_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(slot_offset_remaining_hops_aperiodic_s, start_position),
     (int32_t)offsetof(slot_offset_remaining_hops_aperiodic_s, start_position_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(13),
     nullptr},
    {offsetof(slot_offset_remaining_hops_aperiodic_s, ie_exts),
     (int32_t)offsetof(slot_offset_remaining_hops_aperiodic_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(slot_offset_remaining_hops_aperiodic_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_slot_offset_remaining_hops_aperiodic_s_names[] = {
    {"slotOffset", nullptr},
    {"startPosition", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_slot_offset_remaining_hops_aperiodic_s_opts[] = {
    {0, (int32_t)offsetof(slot_offset_remaining_hops_aperiodic_s, slot_offset_present)},
    {1, (int32_t)offsetof(slot_offset_remaining_hops_aperiodic_s, start_position_present)},
    {2, (int32_t)offsetof(slot_offset_remaining_hops_aperiodic_s, ie_exts_present)}};
const asn1::table::seq_desc slot_offset_remaining_hops_aperiodic_s::desc = {
    {seq_table_slot_offset_remaining_hops_aperiodic_s_fields,
     3,
     UINT64_C(0x0),
     seq_table_slot_offset_remaining_hops_aperiodic_s_opts,
     3},
    true,
    true,
    nullptr,
    0,
    seq_table_slot_offset_remaining_hops_aperiodic_s_names};

// SlotOffsetRemainingHopsSemiPersistent ::= SEQUENCE
static const asn1::table::seq_field seq_table_slot_offset_remaining_hops_semi_persistent_s_fields[] = {
    {offsetof(slot_offset_remaining_hops_semi_persistent_s, srs_periodicity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(slot_offset_remaining_hops_semi_persistent_s::srs_periodicity)>(),
     decltype(slot_offset_remaining_hops_semi_persistent_s::srs_periodicity)::has_ext,
     false,
     decltype(slot_offset_remaining_hops_semi_persistent_s::srs_periodicity)::nof_types,
     decltype(slot_offset_remaining_hops_semi_persistent_s::srs_periodicity)::nof_exts,
     nullptr,
     &srs_periodicity_opts::desc},
    {offsetof(slot_offset_remaining_hops_semi_persistent_s, offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(81919),
     nullptr},
    {offsetof(slot_offset_remaining_hops_semi_persistent_s, start_position),
     (int32_t)offsetof(slot_offset_remaining_hops_semi_persistent_s, start_position_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(13),
     nullptr},
    {offsetof(slot_offset_remaining_hops_semi_persistent_s, ie_exts),
     (int32_t)offsetof(slot_offset_remaining_hops_semi_persistent_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(slot_offset_remaining_hops_semi_persistent_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_slot_offset_remaining_hops_semi_persistent_s_names[] =
    {{"sRSperiodicity", nullptr}, {"offset", nullptr}, {"startPosition", nullptr}, {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_slot_offset_remaining_hops_semi_persistent_s_opts[] = {
    {2, (int32_t)offsetof(slot_offset_remaining_hops_semi_persistent_s, start_position_present)},
    {3, (int32_t)offsetof(slot_offset_remaining_hops_semi_persistent_s, ie_exts_present)}};
const asn1::table::seq_desc slot_offset_remaining_hops_semi_persistent_s::desc = {
    {seq_table_slot_offset_remaining_hops_semi_persistent_s_fields,
     4,
     UINT64_C(0x3),
     seq_table_slot_offset_remaining_hops_semi_persistent_s_opts,
     2},
    true,
    true,
    nullptr,
    0,
    seq_table_slot_offset_remaining_hops_semi_persistent_s_names};

// SlotOffsetRemainingHopsPeriodic ::= SEQUENCE
static const asn1::table::seq_field seq_table_slot_offset_remaining_hops_periodic_s_fields[] = {
    {offsetof(slot_offset_remaining_hops_periodic_s, srs_periodicity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(slot_offset_remaining_hops_periodic_s::srs_periodicity)>(),
     decltype(slot_offset_remaining_hops_periodic_s::srs_periodicity)::has_ext,
     false,
     decltype(slot_offset_remaining_hops_periodic_s::srs_periodicity)::nof_types,
     decltype(slot_offset_remaining_hops_periodic_s::srs_periodicity)::nof_exts,
     nullptr,
     &srs_periodicity_opts::desc},
    {offsetof(slot_offset_remaining_hops_periodic_s, offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(81919),
     nullptr},
    {offsetof(slot_offset_remaining_hops_periodic_s, start_position),
     (int32_t)offsetof(slot_offset_remaining_hops_periodic_s, start_position_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(13),
     nullptr},
    {offsetof(slot_offset_remaining_hops_periodic_s, ie_exts),
     (int32_t)offsetof(slot_offset_remaining_hops_periodic_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(slot_offset_remaining_hops_periodic_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_slot_offset_remaining_hops_periodic_s_names[] =
    {{"sRSperiodicity", nullptr}, {"offset", nullptr}, {"startPosition", nullptr}, {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_slot_offset_remaining_hops_periodic_s_opts[] = {
    {2, (int32_t)offsetof(slot_offset_remaining_hops_periodic_s, start_position_present)},
    {3, (int32_t)offsetof(slot_offset_remaining_hops_periodic_s, ie_exts_present)}};
const asn1::table::seq_desc slot_offset_remaining_hops_periodic_s::desc = {
    {seq_table_slot_offset_remaining_hops_periodic_s_fields,
     4,
     UINT64_C(0x3),
     seq_table_slot_offset_remaining_hops_periodic_s_opts,
     2},
    true,
    true,
    nullptr,
    0,
    seq_table_slot_offset_remaining_hops_periodic_s_names};

// SlotOffsetRemainingHops ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_slot_offset_remaining_hops_c_alternatives[] = {
    &asn1::table::alternative_ops<slot_offset_remaining_hops_aperiodic_s, false>::value,
    &asn1::table::alternative_ops<slot_offset_remaining_hops_semi_persistent_s, false>::value,
    &asn1::table::alternative_ops<slot_offset_remaining_hops_periodic_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<slot_offset_remaining_hops_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc slot_offset_remaining_hops_c::storage_desc = {
    choice_storage_slot_offset_remaining_hops_c_alternatives,
    4,
    "slot_offset_remaining_hops_c"};

slot_offset_remaining_hops_aperiodic_s& slot_offset_remaining_hops_c::set_aperiodic()
{
  set(types::aperiodic);
  return c.get<slot_offset_remaining_hops_aperiodic_s>();
}
slot_offset_remaining_hops_semi_persistent_s& slot_offset_remaining_hops_c::set_semi_persistent()
{
  set(types::semi_persistent);
  return c.get<slot_offset_remaining_hops_semi_persistent_s>();
}
slot_offset_remaining_hops_periodic_s& slot_offset_remaining_hops_c::set_periodic()
{
  set(types::periodic);
  return c.get<slot_offset_remaining_hops_periodic_s>();
}
protocol_ie_single_container_s<slot_offset_remaining_hops_ext_ies_o>& slot_offset_remaining_hops_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<slot_offset_remaining_hops_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_slot_offset_remaining_hops_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &slot_offset_remaining_hops_aperiodic_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &slot_offset_remaining_hops_semi_persistent_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &slot_offset_remaining_hops_periodic_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<slot_offset_remaining_hops_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_slot_offset_remaining_hops_c_names[] =
    {{"aperiodic", nullptr}, {"semi-persistent", nullptr}, {"periodic", nullptr}, {"choice-extension", nullptr}};
const asn1::table::choice_desc slot_offset_remaining_hops_c::desc = {
    asn1::table::enum_int_type<slot_offset_remaining_hops_c::types>(),
    slot_offset_remaining_hops_c::types::nof_types,
    slot_offset_remaining_hops_c::types::nof_exts,
    slot_offset_remaining_hops_c::types::has_ext,
    true,
    choice_table_slot_offset_remaining_hops_c_alternatives,
    nullptr,
    choice_table_slot_offset_remaining_hops_c_names,
    "slot_offset_remaining_hops_c",
    offsetof(slot_offset_remaining_hops_c, c.type),
    offsetof(slot_offset_remaining_hops_c, c.value),
    false,
    &slot_offset_remaining_hops_c::storage_desc};

static const char*           enum_names_slot_offset_remaining_hops_c_types_values[] = {"aperiodic",
                                                                                       "semi-persistent",
                                                                                       "periodic",
                                                                                       "choice-extension"};
const asn1::table::enum_desc slot_offset_remaining_hops_c::types_opts::desc         = {
    enum_names_slot_offset_remaining_hops_c_types_values,
    4,
    "slot_offset_remaining_hops_c::types"};

// SlotOffsetForRemainingHopsItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_slot_offset_for_remaining_hops_item_s_fields[] = {
    {offsetof(slot_offset_for_remaining_hops_item_s, slot_offset_remaining_hops),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &slot_offset_remaining_hops_c::desc},
    {offsetof(slot_offset_for_remaining_hops_item_s, ie_exts),
     (int32_t)offsetof(slot_offset_for_remaining_hops_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(slot_offset_for_remaining_hops_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_slot_offset_for_remaining_hops_item_s_names[] = {
    {"slotOffsetRemainingHops", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_slot_offset_for_remaining_hops_item_s_opts[] = {
    {1, (int32_t)offsetof(slot_offset_for_remaining_hops_item_s, ie_exts_present)}};
const asn1::table::seq_desc slot_offset_for_remaining_hops_item_s::desc = {
    {seq_table_slot_offset_for_remaining_hops_item_s_fields,
     2,
     UINT64_C(0x1),
     seq_table_slot_offset_for_remaining_hops_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_slot_offset_for_remaining_hops_item_s_names};

// TxHoppingConfiguration ::= SEQUENCE
static const asn1::table::seq_field seq_table_tx_hop_cfg_s_fields[] = {
    {offsetof(tx_hop_cfg_s, overlap_value),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(tx_hop_cfg_s::overlap_value)>(),
     decltype(tx_hop_cfg_s::overlap_value)::has_ext,
     false,
     decltype(tx_hop_cfg_s::overlap_value)::nof_types,
     decltype(tx_hop_cfg_s::overlap_value)::nof_exts,
     nullptr,
     &tx_hop_cfg_s::overlap_value_opts::desc},
    {offsetof(tx_hop_cfg_s, nof_hops),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(2),
     UINT64_C(6),
     nullptr},
    {offsetof(tx_hop_cfg_s, slot_offset_for_remaining_hops_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     5,
     &asn1::table::item_list_codec<decltype(tx_hop_cfg_s::slot_offset_for_remaining_hops_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &slot_offset_for_remaining_hops_item_s::desc},
    {offsetof(tx_hop_cfg_s, ie_exts),
     (int32_t)offsetof(tx_hop_cfg_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tx_hop_cfg_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_tx_hop_cfg_s_names[] = {{"overlapValue", nullptr},
                                                                       {"numberOfHops", nullptr},
                                                                       {"slotOffsetForRemainingHopsList", nullptr},
                                                                       {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_tx_hop_cfg_s_opts[]  = {
    {3, (int32_t)offsetof(tx_hop_cfg_s, ie_exts_present)}};
const asn1::table::seq_desc tx_hop_cfg_s::desc = {
    {seq_table_tx_hop_cfg_s_fields, 4, UINT64_C(0x7), seq_table_tx_hop_cfg_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_tx_hop_cfg_s_names};

static const char*           enum_names_tx_hop_cfg_s_overlap_value_e__values[] = {"rb0", "rb1", "rb2", "rb4"};
const asn1::table::enum_desc tx_hop_cfg_s::overlap_value_opts::desc = {enum_names_tx_hop_cfg_s_overlap_value_e__values,
                                                                       4,
                                                                       "tx_hop_cfg_s::overlap_value_e_"};
uint8_t                      tx_hop_cfg_s::overlap_value_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 4};
  return map_enum_number(numbers, 4, value, "tx_hop_cfg_s::overlap_value_e_");
}

static const uint32_t objset_id_pos_srs_res_item_ext_ies_o[]   = {139};
static const uint8_t  objset_crit_pos_srs_res_item_ext_ies_o[] = {crit_e::ignore};

// PosSRSResource-Item-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_pos_srs_res_item_ext_ies_container_values[] = {
    {offsetof(pos_srs_res_item_ext_ies_container, tx_hop_cfg),
     (int32_t)offsetof(pos_srs_res_item_ext_ies_container, tx_hop_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tx_hop_cfg_s::desc}};
static const int32_t ie_table_pos_srs_res_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(pos_srs_res_item_ext_ies_container, tx_hop_cfg_present)};
const asn1::table::ie_container_desc pos_srs_res_item_ext_ies_container::desc = {
    objset_id_pos_srs_res_item_ext_ies_o,
    objset_crit_pos_srs_res_item_ext_ies_o,
    ie_table_pos_srs_res_item_ext_ies_container_values,
    ie_table_pos_srs_res_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// PosSRSResource-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pos_srs_res_item_s_fields[] = {
    {offsetof(pos_srs_res_item_s, srs_pos_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(pos_srs_res_item_s, tx_comb_pos),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &tx_comb_pos_c::desc},
    {offsetof(pos_srs_res_item_s, start_position),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(13),
     nullptr},
    {offsetof(pos_srs_res_item_s, nrof_symbols),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pos_srs_res_item_s::nrof_symbols)>(),
     decltype(pos_srs_res_item_s::nrof_symbols)::has_ext,
     false,
     decltype(pos_srs_res_item_s::nrof_symbols)::nof_types,
     decltype(pos_srs_res_item_s::nrof_symbols)::nof_exts,
     nullptr,
     &pos_srs_res_item_s::nrof_symbols_opts::desc},
    {offsetof(pos_srs_res_item_s, freq_domain_shift),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(268),
     nullptr},
    {offsetof(pos_srs_res_item_s, c_srs),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(pos_srs_res_item_s, group_or_seq_hop),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pos_srs_res_item_s::group_or_seq_hop)>(),
     decltype(pos_srs_res_item_s::group_or_seq_hop)::has_ext,
     false,
     decltype(pos_srs_res_item_s::group_or_seq_hop)::nof_types,
     decltype(pos_srs_res_item_s::group_or_seq_hop)::nof_exts,
     nullptr,
     &pos_srs_res_item_s::group_or_seq_hop_opts::desc},
    {offsetof(pos_srs_res_item_s, res_type_pos),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &res_type_pos_c::desc},
    {offsetof(pos_srs_res_item_s, seq_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(pos_srs_res_item_s, spatial_relation_pos),
     (int32_t)offsetof(pos_srs_res_item_s, spatial_relation_pos_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &spatial_relation_pos_c::desc},
    {offsetof(pos_srs_res_item_s, ie_exts),
     (int32_t)offsetof(pos_srs_res_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &pos_srs_res_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_pos_srs_res_item_s_names[] = {{"srs-PosResourceId", nullptr},
                                                                             {"transmissionCombPos", nullptr},
                                                                             {"startPosition", nullptr},
                                                                             {"nrofSymbols", nullptr},
                                                                             {"freqDomainShift", nullptr},
                                                                             {"c-SRS", nullptr},
                                                                             {"groupOrSequenceHopping", nullptr},
                                                                             {"resourceTypePos", nullptr},
                                                                             {"sequenceId", nullptr},
                                                                             {"spatialRelationPos", nullptr},
                                                                             {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_pos_srs_res_item_s_opts[]  = {
    {9, (int32_t)offsetof(pos_srs_res_item_s, spatial_relation_pos_present)},
    {10, (int32_t)offsetof(pos_srs_res_item_s, ie_exts_present)}};
const asn1::table::seq_desc pos_srs_res_item_s::desc = {
    {seq_table_pos_srs_res_item_s_fields, 11, UINT64_C(0x1ff), seq_table_pos_srs_res_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_pos_srs_res_item_s_names};

static const char*           enum_names_pos_srs_res_item_s_nrof_symbols_e__values[] = {"n1", "n2", "n4", "n8", "n12"};
const asn1::table::enum_desc pos_srs_res_item_s::nrof_symbols_opts::desc            = {
    enum_names_pos_srs_res_item_s_nrof_symbols_e__values,
    5,
    "pos_srs_res_item_s::nrof_symbols_e_"};
uint8_t pos_srs_res_item_s::nrof_symbols_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 12};
  return map_enum_number(numbers, 5, value, "pos_srs_res_item_s::nrof_symbols_e_");
}

static const char*           enum_names_pos_srs_res_item_s_group_or_seq_hop_e__values[] = {"neither",
                                                                                           "groupHopping",
                                                                                           "sequenceHopping"};
const asn1::table::enum_desc pos_srs_res_item_s::group_or_seq_hop_opts::desc            = {
    enum_names_pos_srs_res_item_s_group_or_seq_hop_e__values,
    3,
    "pos_srs_res_item_s::group_or_seq_hop_e_"};

// ResourceSetTypePeriodic ::= SEQUENCE
static const asn1::table::seq_field seq_table_res_set_type_periodic_s_fields[] = {
    {offsetof(res_set_type_periodic_s, periodic_set),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(res_set_type_periodic_s::periodic_set)>(),
     decltype(res_set_type_periodic_s::periodic_set)::has_ext,
     false,
     decltype(res_set_type_periodic_s::periodic_set)::nof_types,
     decltype(res_set_type_periodic_s::periodic_set)::nof_exts,
     nullptr,
     &res_set_type_periodic_s::periodic_set_opts::desc},
    {offsetof(res_set_type_periodic_s, ie_exts),
     (int32_t)offsetof(res_set_type_periodic_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(res_set_type_periodic_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_res_set_type_periodic_s_names[] = {{"periodicSet", nullptr},
                                                                                  {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_res_set_type_periodic_s_opts[]  = {
    {1, (int32_t)offsetof(res_set_type_periodic_s, ie_exts_present)}};
const asn1::table::seq_desc res_set_type_periodic_s::desc = {
    {seq_table_res_set_type_periodic_s_fields, 2, UINT64_C(0x1), seq_table_res_set_type_periodic_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_res_set_type_periodic_s_names};

static const char*           enum_names_res_set_type_periodic_s_periodic_set_e__values[] = {"true"};
const asn1::table::enum_desc res_set_type_periodic_s::periodic_set_opts::desc            = {
    enum_names_res_set_type_periodic_s_periodic_set_e__values,
    1,
    "res_set_type_periodic_s::periodic_set_e_"};

// ResourceSetTypeSemi-persistent ::= SEQUENCE
static const asn1::table::seq_field seq_table_res_set_type_semi_persistent_s_fields[] = {
    {offsetof(res_set_type_semi_persistent_s, semi_persistent_set),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(res_set_type_semi_persistent_s::semi_persistent_set)>(),
     decltype(res_set_type_semi_persistent_s::semi_persistent_set)::has_ext,
     false,
     decltype(res_set_type_semi_persistent_s::semi_persistent_set)::nof_types,
     decltype(res_set_type_semi_persistent_s::semi_persistent_set)::nof_exts,
     nullptr,
     &res_set_type_semi_persistent_s::semi_persistent_set_opts::desc},
    {offsetof(res_set_type_semi_persistent_s, ie_exts),
     (int32_t)offsetof(res_set_type_semi_persistent_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(res_set_type_semi_persistent_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_res_set_type_semi_persistent_s_names[] = {
    {"semi-persistentSet", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_res_set_type_semi_persistent_s_opts[] = {
    {1, (int32_t)offsetof(res_set_type_semi_persistent_s, ie_exts_present)}};
const asn1::table::seq_desc res_set_type_semi_persistent_s::desc = {{seq_table_res_set_type_semi_persistent_s_fields,
                                                                     2,
                                                                     UINT64_C(0x1),
                                                                     seq_table_res_set_type_semi_persistent_s_opts,
                                                                     1},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_res_set_type_semi_persistent_s_names};

static const char*           enum_names_res_set_type_semi_persistent_s_semi_persistent_set_e__values[] = {"true"};
const asn1::table::enum_desc res_set_type_semi_persistent_s::semi_persistent_set_opts::desc            = {
    enum_names_res_set_type_semi_persistent_s_semi_persistent_set_e__values,
    1,
    "res_set_type_semi_persistent_s::semi_persistent_set_e_"};

// ResourceSetTypeAperiodic ::= SEQUENCE
static const asn1::table::seq_field seq_table_res_set_type_aperiodic_s_fields[] = {
    {offsetof(res_set_type_aperiodic_s, srs_res_trigger),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(1),
     UINT64_C(3),
     nullptr},
    {offsetof(res_set_type_aperiodic_s, slotoffset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(32),
     nullptr},
    {offsetof(res_set_type_aperiodic_s, ie_exts),
     (int32_t)offsetof(res_set_type_aperiodic_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(res_set_type_aperiodic_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_res_set_type_aperiodic_s_names[] = {{"sRSResourceTrigger", nullptr},
                                                                                   {"slotoffset", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_res_set_type_aperiodic_s_opts[]  = {
    {2, (int32_t)offsetof(res_set_type_aperiodic_s, ie_exts_present)}};
const asn1::table::seq_desc res_set_type_aperiodic_s::desc = {
    {seq_table_res_set_type_aperiodic_s_fields, 3, UINT64_C(0x3), seq_table_res_set_type_aperiodic_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_res_set_type_aperiodic_s_names};

// ResourceSetType ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_res_set_type_c_alternatives[] = {
    &asn1::table::alternative_ops<res_set_type_periodic_s, false>::value,
    &asn1::table::alternative_ops<res_set_type_semi_persistent_s, false>::value,
    &asn1::table::alternative_ops<res_set_type_aperiodic_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<res_set_type_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc res_set_type_c::storage_desc = {choice_storage_res_set_type_c_alternatives,
                                                                       4,
                                                                       "res_set_type_c"};

res_set_type_periodic_s& res_set_type_c::set_periodic()
{
  set(types::periodic);
  return c.get<res_set_type_periodic_s>();
}
res_set_type_semi_persistent_s& res_set_type_c::set_semi_persistent()
{
  set(types::semi_persistent);
  return c.get<res_set_type_semi_persistent_s>();
}
res_set_type_aperiodic_s& res_set_type_c::set_aperiodic()
{
  set(types::aperiodic);
  return c.get<res_set_type_aperiodic_s>();
}
protocol_ie_single_container_s<res_set_type_ext_ies_o>& res_set_type_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<res_set_type_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_res_set_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &res_set_type_periodic_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &res_set_type_semi_persistent_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &res_set_type_aperiodic_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<res_set_type_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_res_set_type_c_names[] = {{"periodic", nullptr},
                                                                            {"semi-persistent", nullptr},
                                                                            {"aperiodic", nullptr},
                                                                            {"choice-extension", nullptr}};
const asn1::table::choice_desc       res_set_type_c::desc = {asn1::table::enum_int_type<res_set_type_c::types>(),
                                                             res_set_type_c::types::nof_types,
                                                             res_set_type_c::types::nof_exts,
                                                             res_set_type_c::types::has_ext,
                                                             true,
                                                             choice_table_res_set_type_c_alternatives,
                                                             nullptr,
                                                             choice_table_res_set_type_c_names,
                                                             "res_set_type_c",
                                                             offsetof(res_set_type_c, c.type),
                                                             offsetof(res_set_type_c, c.value),
                                                             false,
                                                             &res_set_type_c::storage_desc};

static const char*           enum_names_res_set_type_c_types_values[] = {"periodic",
                                                                         "semi-persistent",
                                                                         "aperiodic",
                                                                         "choice-extension"};
const asn1::table::enum_desc res_set_type_c::types_opts::desc         = {enum_names_res_set_type_c_types_values,
                                                                         4,
                                                                         "res_set_type_c::types"};

// SRSResourceSet ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_res_set_s_fields[] = {
    {offsetof(srs_res_set_s, srs_res_set_id1),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(15),
     nullptr},
    {offsetof(srs_res_set_s, srs_res_id_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16,
     &asn1::table::list_codec<decltype(srs_res_set_s::srs_res_id_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(63), false, true>>::value},
    {offsetof(srs_res_set_s, res_set_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &res_set_type_c::desc},
    {offsetof(srs_res_set_s, ie_exts),
     (int32_t)offsetof(srs_res_set_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(srs_res_set_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_srs_res_set_s_names[] = {{"sRSResourceSetID1", nullptr},
                                                                        {"sRSResourceID-List", nullptr},
                                                                        {"resourceSetType", nullptr},
                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_res_set_s_opts[]  = {
    {3, (int32_t)offsetof(srs_res_set_s, ie_exts_present)}};
const asn1::table::seq_desc srs_res_set_s::desc = {
    {seq_table_srs_res_set_s_fields, 4, UINT64_C(0x7), seq_table_srs_res_set_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_srs_res_set_s_names};

// PosResourceSetTypePeriodic ::= SEQUENCE
static const asn1::table::seq_field seq_table_pos_res_set_type_periodic_s_fields[] = {
    {offsetof(pos_res_set_type_periodic_s, posperiodic_set),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pos_res_set_type_periodic_s::posperiodic_set)>(),
     decltype(pos_res_set_type_periodic_s::posperiodic_set)::has_ext,
     false,
     decltype(pos_res_set_type_periodic_s::posperiodic_set)::nof_types,
     decltype(pos_res_set_type_periodic_s::posperiodic_set)::nof_exts,
     nullptr,
     &pos_res_set_type_periodic_s::posperiodic_set_opts::desc},
    {offsetof(pos_res_set_type_periodic_s, ie_exts),
     (int32_t)offsetof(pos_res_set_type_periodic_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pos_res_set_type_periodic_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pos_res_set_type_periodic_s_names[] = {{"posperiodicSet", nullptr},
                                                                                      {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_pos_res_set_type_periodic_s_opts[]  = {
    {1, (int32_t)offsetof(pos_res_set_type_periodic_s, ie_exts_present)}};
const asn1::table::seq_desc pos_res_set_type_periodic_s::desc = {
    {seq_table_pos_res_set_type_periodic_s_fields, 2, UINT64_C(0x1), seq_table_pos_res_set_type_periodic_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_pos_res_set_type_periodic_s_names};

static const char*           enum_names_pos_res_set_type_periodic_s_posperiodic_set_e__values[] = {"true"};
const asn1::table::enum_desc pos_res_set_type_periodic_s::posperiodic_set_opts::desc            = {
    enum_names_pos_res_set_type_periodic_s_posperiodic_set_e__values,
    1,
    "pos_res_set_type_periodic_s::posperiodic_set_e_"};

// PosResourceSetTypeSemi-persistent ::= SEQUENCE
static const asn1::table::seq_field seq_table_pos_res_set_type_semi_persistent_s_fields[] = {
    {offsetof(pos_res_set_type_semi_persistent_s, possemi_persistent_set),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pos_res_set_type_semi_persistent_s::possemi_persistent_set)>(),
     decltype(pos_res_set_type_semi_persistent_s::possemi_persistent_set)::has_ext,
     false,
     decltype(pos_res_set_type_semi_persistent_s::possemi_persistent_set)::nof_types,
     decltype(pos_res_set_type_semi_persistent_s::possemi_persistent_set)::nof_exts,
     nullptr,
     &pos_res_set_type_semi_persistent_s::possemi_persistent_set_opts::desc},
    {offsetof(pos_res_set_type_semi_persistent_s, ie_exts),
     (int32_t)offsetof(pos_res_set_type_semi_persistent_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pos_res_set_type_semi_persistent_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pos_res_set_type_semi_persistent_s_names[] = {
    {"possemi-persistentSet", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pos_res_set_type_semi_persistent_s_opts[] = {
    {1, (int32_t)offsetof(pos_res_set_type_semi_persistent_s, ie_exts_present)}};
const asn1::table::seq_desc pos_res_set_type_semi_persistent_s::desc = {
    {seq_table_pos_res_set_type_semi_persistent_s_fields,
     2,
     UINT64_C(0x1),
     seq_table_pos_res_set_type_semi_persistent_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_pos_res_set_type_semi_persistent_s_names};

static const char* enum_names_pos_res_set_type_semi_persistent_s_possemi_persistent_set_e__values[] = {"true"};
const asn1::table::enum_desc pos_res_set_type_semi_persistent_s::possemi_persistent_set_opts::desc  = {
    enum_names_pos_res_set_type_semi_persistent_s_possemi_persistent_set_e__values,
    1,
    "pos_res_set_type_semi_persistent_s::possemi_persistent_set_e_"};

// PosResourceSetTypeAperiodic ::= SEQUENCE
static const asn1::table::seq_field seq_table_pos_res_set_type_aperiodic_s_fields[] = {
    {offsetof(pos_res_set_type_aperiodic_s, srs_res_trigger),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(1),
     UINT64_C(3),
     nullptr},
    {offsetof(pos_res_set_type_aperiodic_s, ie_exts),
     (int32_t)offsetof(pos_res_set_type_aperiodic_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pos_res_set_type_aperiodic_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pos_res_set_type_aperiodic_s_names[] = {{"sRSResourceTrigger", nullptr},
                                                                                       {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_pos_res_set_type_aperiodic_s_opts[]  = {
    {1, (int32_t)offsetof(pos_res_set_type_aperiodic_s, ie_exts_present)}};
const asn1::table::seq_desc pos_res_set_type_aperiodic_s::desc = {
    {seq_table_pos_res_set_type_aperiodic_s_fields, 2, UINT64_C(0x1), seq_table_pos_res_set_type_aperiodic_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_pos_res_set_type_aperiodic_s_names};

// PosResourceSetType ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_pos_res_set_type_c_alternatives[] = {
    &asn1::table::alternative_ops<pos_res_set_type_periodic_s, false>::value,
    &asn1::table::alternative_ops<pos_res_set_type_semi_persistent_s, false>::value,
    &asn1::table::alternative_ops<pos_res_set_type_aperiodic_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<pos_res_set_type_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc pos_res_set_type_c::storage_desc = {
    choice_storage_pos_res_set_type_c_alternatives,
    4,
    "pos_res_set_type_c"};

pos_res_set_type_periodic_s& pos_res_set_type_c::set_periodic()
{
  set(types::periodic);
  return c.get<pos_res_set_type_periodic_s>();
}
pos_res_set_type_semi_persistent_s& pos_res_set_type_c::set_semi_persistent()
{
  set(types::semi_persistent);
  return c.get<pos_res_set_type_semi_persistent_s>();
}
pos_res_set_type_aperiodic_s& pos_res_set_type_c::set_aperiodic()
{
  set(types::aperiodic);
  return c.get<pos_res_set_type_aperiodic_s>();
}
protocol_ie_single_container_s<pos_res_set_type_ext_ies_o>& pos_res_set_type_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<pos_res_set_type_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_pos_res_set_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pos_res_set_type_periodic_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pos_res_set_type_semi_persistent_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pos_res_set_type_aperiodic_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<pos_res_set_type_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_pos_res_set_type_c_names[] = {{"periodic", nullptr},
                                                                                {"semi-persistent", nullptr},
                                                                                {"aperiodic", nullptr},
                                                                                {"choice-extension", nullptr}};
const asn1::table::choice_desc pos_res_set_type_c::desc = {asn1::table::enum_int_type<pos_res_set_type_c::types>(),
                                                           pos_res_set_type_c::types::nof_types,
                                                           pos_res_set_type_c::types::nof_exts,
                                                           pos_res_set_type_c::types::has_ext,
                                                           true,
                                                           choice_table_pos_res_set_type_c_alternatives,
                                                           nullptr,
                                                           choice_table_pos_res_set_type_c_names,
                                                           "pos_res_set_type_c",
                                                           offsetof(pos_res_set_type_c, c.type),
                                                           offsetof(pos_res_set_type_c, c.value),
                                                           false,
                                                           &pos_res_set_type_c::storage_desc};

static const char*           enum_names_pos_res_set_type_c_types_values[] = {"periodic",
                                                                             "semi-persistent",
                                                                             "aperiodic",
                                                                             "choice-extension"};
const asn1::table::enum_desc pos_res_set_type_c::types_opts::desc         = {enum_names_pos_res_set_type_c_types_values,
                                                                             4,
                                                                             "pos_res_set_type_c::types"};

// PosSRSResourceSet-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pos_srs_res_set_item_s_fields[] = {
    {offsetof(pos_srs_res_set_item_s, possrs_res_set_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(15),
     nullptr},
    {offsetof(pos_srs_res_set_item_s, poss_rs_res_id_per_set_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16,
     &asn1::table::list_codec<decltype(pos_srs_res_set_item_s::poss_rs_res_id_per_set_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(63), false, true>>::value},
    {offsetof(pos_srs_res_set_item_s, posres_set_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &pos_res_set_type_c::desc},
    {offsetof(pos_srs_res_set_item_s, ie_exts),
     (int32_t)offsetof(pos_srs_res_set_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pos_srs_res_set_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pos_srs_res_set_item_s_names[] = {
    {"possrsResourceSetID", nullptr},
    {"possRSResourceIDPerSet-List", nullptr},
    {"posresourceSetType", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pos_srs_res_set_item_s_opts[] = {
    {3, (int32_t)offsetof(pos_srs_res_set_item_s, ie_exts_present)}};
const asn1::table::seq_desc pos_srs_res_set_item_s::desc = {
    {seq_table_pos_srs_res_set_item_s_fields, 4, UINT64_C(0x7), seq_table_pos_srs_res_set_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_pos_srs_res_set_item_s_names};

// SRSConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_cfg_s_fields[] = {
    {offsetof(srs_cfg_s, srs_res_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(srs_cfg_s::srs_res_list), asn1::table::sequence_item_ops, false, true>::
         value,
     &srs_res_s::desc},
    {offsetof(srs_cfg_s, pos_srs_res_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(srs_cfg_s::pos_srs_res_list), asn1::table::sequence_item_ops, false, true>::
         value,
     &pos_srs_res_item_s::desc},
    {offsetof(srs_cfg_s, srs_res_set_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16,
     &asn1::table::item_list_codec<decltype(srs_cfg_s::srs_res_set_list), asn1::table::sequence_item_ops, false, true>::
         value,
     &srs_res_set_s::desc},
    {offsetof(srs_cfg_s, pos_srs_res_set_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16,
     &asn1::table::
         item_list_codec<decltype(srs_cfg_s::pos_srs_res_set_list), asn1::table::sequence_item_ops, false, true>::value,
     &pos_srs_res_set_item_s::desc},
    {offsetof(srs_cfg_s, ie_exts),
     (int32_t)offsetof(srs_cfg_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(srs_cfg_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_srs_cfg_s_names[] = {{"sRSResource-List", nullptr},
                                                                    {"posSRSResource-List", nullptr},
                                                                    {"sRSResourceSet-List", nullptr},
                                                                    {"posSRSResourceSet-List", nullptr},
                                                                    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_cfg_s_opts[]  = {{0, asn1::table::seq_field::list_presence},
                                                                    {1, asn1::table::seq_field::list_presence},
                                                                    {2, asn1::table::seq_field::list_presence},
                                                                    {3, asn1::table::seq_field::list_presence},
                                                                    {4, (int32_t)offsetof(srs_cfg_s, ie_exts_present)}};
const asn1::table::seq_desc          srs_cfg_s::desc             = {
    {seq_table_srs_cfg_s_fields, 5, UINT64_C(0x0), seq_table_srs_cfg_s_opts, 5},
    true,
    true,
    nullptr,
    0,
    seq_table_srs_cfg_s_names};

// ActiveULBWP ::= SEQUENCE
static const asn1::table::seq_field seq_table_active_ul_bwp_s_fields[] = {
    {offsetof(active_ul_bwp_s, location_and_bw),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(37949),
     nullptr},
    {offsetof(active_ul_bwp_s, subcarrier_spacing),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(active_ul_bwp_s::subcarrier_spacing)>(),
     decltype(active_ul_bwp_s::subcarrier_spacing)::has_ext,
     false,
     decltype(active_ul_bwp_s::subcarrier_spacing)::nof_types,
     decltype(active_ul_bwp_s::subcarrier_spacing)::nof_exts,
     nullptr,
     &active_ul_bwp_s::subcarrier_spacing_opts::desc},
    {offsetof(active_ul_bwp_s, cp),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(active_ul_bwp_s::cp)>(),
     decltype(active_ul_bwp_s::cp)::has_ext,
     false,
     decltype(active_ul_bwp_s::cp)::nof_types,
     decltype(active_ul_bwp_s::cp)::nof_exts,
     nullptr,
     &active_ul_bwp_s::cp_opts::desc},
    {offsetof(active_ul_bwp_s, tx_direct_current_location),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(3301),
     nullptr},
    {offsetof(active_ul_bwp_s, shift7dot5k_hz),
     (int32_t)offsetof(active_ul_bwp_s, shift7dot5k_hz_present),
     asn1::table::enum_op<decltype(active_ul_bwp_s::shift7dot5k_hz)>(),
     decltype(active_ul_bwp_s::shift7dot5k_hz)::has_ext,
     false,
     decltype(active_ul_bwp_s::shift7dot5k_hz)::nof_types,
     decltype(active_ul_bwp_s::shift7dot5k_hz)::nof_exts,
     nullptr,
     &active_ul_bwp_s::shift7dot5k_hz_opts::desc},
    {offsetof(active_ul_bwp_s, srs_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_cfg_s::desc},
    {offsetof(active_ul_bwp_s, ie_exts),
     (int32_t)offsetof(active_ul_bwp_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(active_ul_bwp_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_active_ul_bwp_s_names[] = {{"locationAndBandwidth", nullptr},
                                                                          {"subcarrierSpacing", nullptr},
                                                                          {"cyclicPrefix", nullptr},
                                                                          {"txDirectCurrentLocation", nullptr},
                                                                          {"shift7dot5kHz", nullptr},
                                                                          {"sRSConfig", nullptr},
                                                                          {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_active_ul_bwp_s_opts[]  = {
    {4, (int32_t)offsetof(active_ul_bwp_s, shift7dot5k_hz_present)},
    {6, (int32_t)offsetof(active_ul_bwp_s, ie_exts_present)}};
const asn1::table::seq_desc active_ul_bwp_s::desc = {
    {seq_table_active_ul_bwp_s_fields, 7, UINT64_C(0x2f), seq_table_active_ul_bwp_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_active_ul_bwp_s_names};

static const char* enum_names_active_ul_bwp_s_subcarrier_spacing_e__values[] =
    {"kHz15", "kHz30", "kHz60", "kHz120", "kHz480", "kHz960"};
const asn1::table::enum_desc active_ul_bwp_s::subcarrier_spacing_opts::desc = {
    enum_names_active_ul_bwp_s_subcarrier_spacing_e__values,
    6,
    "active_ul_bwp_s::subcarrier_spacing_e_"};
uint16_t active_ul_bwp_s::subcarrier_spacing_opts::to_number() const
{
  static const uint16_t numbers[] = {15, 30, 60, 120, 480, 960};
  return map_enum_number(numbers, 6, value, "active_ul_bwp_s::subcarrier_spacing_e_");
}

static const char*           enum_names_active_ul_bwp_s_cp_e__values[] = {"normal", "extended"};
const asn1::table::enum_desc active_ul_bwp_s::cp_opts::desc            = {enum_names_active_ul_bwp_s_cp_e__values,
                                                                          2,
                                                                          "active_ul_bwp_s::cp_e_"};

static const char*           enum_names_active_ul_bwp_s_shift7dot5k_hz_e__values[] = {"true"};
const asn1::table::enum_desc active_ul_bwp_s::shift7dot5k_hz_opts::desc            = {
    enum_names_active_ul_bwp_s_shift7dot5k_hz_e__values,
    1,
    "active_ul_bwp_s::shift7dot5k_hz_e_"};

// RelativePathDelay-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_relative_path_delay_ext_ies_o[] = {147, 148, 149, 150, 151, 152};
static const uint8_t  objset_crit_relative_path_delay_ext_ies_o[] =
    {crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};
static const uint8_t objset_value_relative_path_delay_ext_ies_o[] = {
    relative_path_delay_ext_ies_o::value_c::types::report_granularitykminus1_add_path,
    relative_path_delay_ext_ies_o::value_c::types::report_granularitykminus2_add_path,
    relative_path_delay_ext_ies_o::value_c::types::report_granularitykminus3_add_path,
    relative_path_delay_ext_ies_o::value_c::types::report_granularitykminus4_add_path,
    relative_path_delay_ext_ies_o::value_c::types::report_granularitykminus5_add_path,
    relative_path_delay_ext_ies_o::value_c::types::report_granularitykminus6_add_path};
static const uint8_t                    objset_presence_relative_path_delay_ext_ies_o[] = {presence_e::mandatory,
                                                                                           presence_e::mandatory,
                                                                                           presence_e::mandatory,
                                                                                           presence_e::mandatory,
                                                                                           presence_e::mandatory,
                                                                                           presence_e::mandatory};
static const asn1::table::objset_column objset_columns_relative_path_delay_ext_ies_o[]  = {
    {objset_crit_relative_path_delay_ext_ies_o, crit_e::nulltype},
    {objset_value_relative_path_delay_ext_ies_o, relative_path_delay_ext_ies_o::value_c::types::nulltype},
    {objset_presence_relative_path_delay_ext_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc relative_path_delay_ext_ies_o::desc = {objset_id_relative_path_delay_ext_ies_o,
                                                                      6,
                                                                      "id",
                                                                      objset_columns_relative_path_delay_ext_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_relative_path_delay_ext_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(32701),
     &asn1::table::any_value_codec<uint16_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(65401),
     &asn1::table::any_value_codec<uint16_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(130801),
     &asn1::table::any_value_codec<uint32_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(261601),
     &asn1::table::any_value_codec<uint32_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(523201),
     &asn1::table::any_value_codec<uint32_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(1046401),
     &asn1::table::any_value_codec<uint32_t>::value}};
static const asn1::table::field_name choice_table_relative_path_delay_ext_ies_o_value_c_names[] = {
    {"INTEGER (0..32701)", nullptr},
    {"INTEGER (0..65401)", nullptr},
    {"INTEGER (0..130801)", nullptr},
    {"INTEGER (0..261601)", nullptr},
    {"INTEGER (0..523201)", nullptr},
    {"INTEGER (0..1046401)", nullptr}};
const asn1::table::choice_desc relative_path_delay_ext_ies_o::value_c::desc = {
    asn1::table::enum_int_type<relative_path_delay_ext_ies_o::value_c::types>(),
    relative_path_delay_ext_ies_o::value_c::types::nof_types,
    relative_path_delay_ext_ies_o::value_c::types::nof_exts,
    relative_path_delay_ext_ies_o::value_c::types::has_ext,
    true,
    choice_table_relative_path_delay_ext_ies_o_value_c_alternatives,
    &asn1::table::choice_set<relative_path_delay_ext_ies_o::value_c>,
    choice_table_relative_path_delay_ext_ies_o_value_c_names,
    "relative_path_delay_ext_ies_o::value_c",
    offsetof(relative_path_delay_ext_ies_o::value_c, type_),
    offsetof(relative_path_delay_ext_ies_o::value_c, c),
    true,
    nullptr,
    true};

void relative_path_delay_ext_ies_o::value_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, relative_path_delay_ext_ies_o::value_c::desc);
}
uint16_t& relative_path_delay_ext_ies_o::value_c::report_granularitykminus1_add_path()
{
  assert_choice_type(types::report_granularitykminus1_add_path, type_, "Value");
  return c.get<uint16_t>();
}
uint16_t& relative_path_delay_ext_ies_o::value_c::report_granularitykminus2_add_path()
{
  assert_choice_type(types::report_granularitykminus2_add_path, type_, "Value");
  return c.get<uint16_t>();
}
uint32_t& relative_path_delay_ext_ies_o::value_c::report_granularitykminus3_add_path()
{
  assert_choice_type(types::report_granularitykminus3_add_path, type_, "Value");
  return c.get<uint32_t>();
}
uint32_t& relative_path_delay_ext_ies_o::value_c::report_granularitykminus4_add_path()
{
  assert_choice_type(types::report_granularitykminus4_add_path, type_, "Value");
  return c.get<uint32_t>();
}
uint32_t& relative_path_delay_ext_ies_o::value_c::report_granularitykminus5_add_path()
{
  assert_choice_type(types::report_granularitykminus5_add_path, type_, "Value");
  return c.get<uint32_t>();
}
uint32_t& relative_path_delay_ext_ies_o::value_c::report_granularitykminus6_add_path()
{
  assert_choice_type(types::report_granularitykminus6_add_path, type_, "Value");
  return c.get<uint32_t>();
}
const uint16_t& relative_path_delay_ext_ies_o::value_c::report_granularitykminus1_add_path() const
{
  assert_choice_type(types::report_granularitykminus1_add_path, type_, "Value");
  return c.get<uint16_t>();
}
const uint16_t& relative_path_delay_ext_ies_o::value_c::report_granularitykminus2_add_path() const
{
  assert_choice_type(types::report_granularitykminus2_add_path, type_, "Value");
  return c.get<uint16_t>();
}
const uint32_t& relative_path_delay_ext_ies_o::value_c::report_granularitykminus3_add_path() const
{
  assert_choice_type(types::report_granularitykminus3_add_path, type_, "Value");
  return c.get<uint32_t>();
}
const uint32_t& relative_path_delay_ext_ies_o::value_c::report_granularitykminus4_add_path() const
{
  assert_choice_type(types::report_granularitykminus4_add_path, type_, "Value");
  return c.get<uint32_t>();
}
const uint32_t& relative_path_delay_ext_ies_o::value_c::report_granularitykminus5_add_path() const
{
  assert_choice_type(types::report_granularitykminus5_add_path, type_, "Value");
  return c.get<uint32_t>();
}
const uint32_t& relative_path_delay_ext_ies_o::value_c::report_granularitykminus6_add_path() const
{
  assert_choice_type(types::report_granularitykminus6_add_path, type_, "Value");
  return c.get<uint32_t>();
}

static const char*           enum_names_relative_path_delay_ext_ies_o_value_c_types_values[] = {"INTEGER (0..32701)",
                                                                                                "INTEGER (0..65401)",
                                                                                                "INTEGER (0..130801)",
                                                                                                "INTEGER (0..261601)",
                                                                                                "INTEGER (0..523201)",
                                                                                                "INTEGER (0..1046401)"};
const asn1::table::enum_desc relative_path_delay_ext_ies_o::value_c::types_opts::desc        = {
    enum_names_relative_path_delay_ext_ies_o_value_c_types_values,
    6,
    "relative_path_delay_ext_ies_o::value_c::types"};

// RelativePathDelay ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_relative_path_delay_c_alternatives[] = {
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<relative_path_delay_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc relative_path_delay_c::storage_desc = {
    choice_storage_relative_path_delay_c_alternatives,
    7,
    "relative_path_delay_c"};

uint16_t& relative_path_delay_c::set_k0()
{
  set(types::k0);
  return c.get<uint16_t>();
}
uint16_t& relative_path_delay_c::set_k1()
{
  set(types::k1);
  return c.get<uint16_t>();
}
uint16_t& relative_path_delay_c::set_k2()
{
  set(types::k2);
  return c.get<uint16_t>();
}
uint16_t& relative_path_delay_c::set_k3()
{
  set(types::k3);
  return c.get<uint16_t>();
}
uint16_t& relative_path_delay_c::set_k4()
{
  set(types::k4);
  return c.get<uint16_t>();
}
uint16_t& relative_path_delay_c::set_k5()
{
  set(types::k5);
  return c.get<uint16_t>();
}
protocol_ie_single_container_s<relative_path_delay_ext_ies_o>& relative_path_delay_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<relative_path_delay_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_relative_path_delay_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(16351),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(8176),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(4088),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(2044),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1022),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(511),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<relative_path_delay_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_relative_path_delay_c_names[] = {{"k0", nullptr},
                                                                                   {"k1", nullptr},
                                                                                   {"k2", nullptr},
                                                                                   {"k3", nullptr},
                                                                                   {"k4", nullptr},
                                                                                   {"k5", nullptr},
                                                                                   {"choice-Extension", nullptr}};
const asn1::table::choice_desc       relative_path_delay_c::desc                = {
    asn1::table::enum_int_type<relative_path_delay_c::types>(),
    relative_path_delay_c::types::nof_types,
    relative_path_delay_c::types::nof_exts,
    relative_path_delay_c::types::has_ext,
    true,
    choice_table_relative_path_delay_c_alternatives,
    nullptr,
    choice_table_relative_path_delay_c_names,
    "relative_path_delay_c",
    offsetof(relative_path_delay_c, c.type),
    offsetof(relative_path_delay_c, c.value),
    false,
    &relative_path_delay_c::storage_desc};

static const char* enum_names_relative_path_delay_c_types_values[] =
    {"k0", "k1", "k2", "k3", "k4", "k5", "choice-Extension"};
const asn1::table::enum_desc relative_path_delay_c::types_opts::desc = {enum_names_relative_path_delay_c_types_values,
                                                                        7,
                                                                        "relative_path_delay_c::types"};
uint8_t                      relative_path_delay_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5};
  return map_enum_number(numbers, 6, value, "relative_path_delay_c::types");
}

// TrpMeasurementTimingQuality ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_meas_timing_quality_s_fields[] = {
    {offsetof(trp_meas_timing_quality_s, meas_quality),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(trp_meas_timing_quality_s, resolution),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(trp_meas_timing_quality_s::resolution)>(),
     decltype(trp_meas_timing_quality_s::resolution)::has_ext,
     false,
     decltype(trp_meas_timing_quality_s::resolution)::nof_types,
     decltype(trp_meas_timing_quality_s::resolution)::nof_exts,
     nullptr,
     &trp_meas_timing_quality_s::resolution_opts::desc},
    {offsetof(trp_meas_timing_quality_s, ie_exts),
     (int32_t)offsetof(trp_meas_timing_quality_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_meas_timing_quality_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_meas_timing_quality_s_names[] = {{"measurementQuality", nullptr},
                                                                                    {"resolution", nullptr},
                                                                                    {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_trp_meas_timing_quality_s_opts[]  = {
    {2, (int32_t)offsetof(trp_meas_timing_quality_s, ie_exts_present)}};
const asn1::table::seq_desc trp_meas_timing_quality_s::desc = {
    {seq_table_trp_meas_timing_quality_s_fields, 3, UINT64_C(0x3), seq_table_trp_meas_timing_quality_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_meas_timing_quality_s_names};

static const char* enum_names_trp_meas_timing_quality_s_resolution_e__values[] = {"m0dot1", "m1", "m10", "m30"};
const asn1::table::enum_desc trp_meas_timing_quality_s::resolution_opts::desc  = {
    enum_names_trp_meas_timing_quality_s_resolution_e__values,
    4,
    "trp_meas_timing_quality_s::resolution_e_"};
float trp_meas_timing_quality_s::resolution_opts::to_number() const
{
  static const float numbers[] = {0.1, 1.0, 10.0, 30.0};
  return map_enum_number(numbers, 4, value, "trp_meas_timing_quality_s::resolution_e_");
}
const char* trp_meas_timing_quality_s::resolution_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.1", "1", "10", "30"};
  return convert_enum_idx(number_strs, 4, value, "trp_meas_timing_quality_s::resolution_e_");
}

// TrpMeasurementAngleQuality ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_meas_angle_quality_s_fields[] = {
    {offsetof(trp_meas_angle_quality_s, azimuth_quality),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(trp_meas_angle_quality_s, zenith_quality),
     (int32_t)offsetof(trp_meas_angle_quality_s, zenith_quality_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(trp_meas_angle_quality_s, resolution),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(trp_meas_angle_quality_s::resolution)>(),
     decltype(trp_meas_angle_quality_s::resolution)::has_ext,
     false,
     decltype(trp_meas_angle_quality_s::resolution)::nof_types,
     decltype(trp_meas_angle_quality_s::resolution)::nof_exts,
     nullptr,
     &trp_meas_angle_quality_s::resolution_opts::desc},
    {offsetof(trp_meas_angle_quality_s, ie_exts),
     (int32_t)offsetof(trp_meas_angle_quality_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_meas_angle_quality_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_meas_angle_quality_s_names[] = {{"azimuthQuality", nullptr},
                                                                                   {"zenithQuality", nullptr},
                                                                                   {"resolution", nullptr},
                                                                                   {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_trp_meas_angle_quality_s_opts[]  = {
    {1, (int32_t)offsetof(trp_meas_angle_quality_s, zenith_quality_present)},
    {3, (int32_t)offsetof(trp_meas_angle_quality_s, ie_exts_present)}};
const asn1::table::seq_desc trp_meas_angle_quality_s::desc = {
    {seq_table_trp_meas_angle_quality_s_fields, 4, UINT64_C(0x5), seq_table_trp_meas_angle_quality_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_meas_angle_quality_s_names};

static const char*           enum_names_trp_meas_angle_quality_s_resolution_e__values[] = {"deg0dot1"};
const asn1::table::enum_desc trp_meas_angle_quality_s::resolution_opts::desc            = {
    enum_names_trp_meas_angle_quality_s_resolution_e__values,
    1,
    "trp_meas_angle_quality_s::resolution_e_"};
float trp_meas_angle_quality_s::resolution_opts::to_number() const
{
  static const float numbers[] = {0.1};
  return map_enum_number(numbers, 1, value, "trp_meas_angle_quality_s::resolution_e_");
}
const char* trp_meas_angle_quality_s::resolution_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.1"};
  return convert_enum_idx(number_strs, 1, value, "trp_meas_angle_quality_s::resolution_e_");
}

// TRPPhaseQuality ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_phase_quality_s_fields[] = {
    {offsetof(trp_phase_quality_s, phase_quality_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(179),
     nullptr},
    {offsetof(trp_phase_quality_s, resolution),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(trp_phase_quality_s::resolution)>(),
     decltype(trp_phase_quality_s::resolution)::has_ext,
     false,
     decltype(trp_phase_quality_s::resolution)::nof_types,
     decltype(trp_phase_quality_s::resolution)::nof_exts,
     nullptr,
     &trp_phase_quality_s::resolution_opts::desc},
    {offsetof(trp_phase_quality_s, ie_exts),
     (int32_t)offsetof(trp_phase_quality_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_phase_quality_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_phase_quality_s_names[] = {{"phaseQualityIndex", nullptr},
                                                                              {"resolution", nullptr},
                                                                              {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_trp_phase_quality_s_opts[]  = {
    {2, (int32_t)offsetof(trp_phase_quality_s, ie_exts_present)}};
const asn1::table::seq_desc trp_phase_quality_s::desc = {
    {seq_table_trp_phase_quality_s_fields, 3, UINT64_C(0x3), seq_table_trp_phase_quality_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_phase_quality_s_names};

static const char*           enum_names_trp_phase_quality_s_resolution_e__values[] = {"deg0dot1", "deg1"};
const asn1::table::enum_desc trp_phase_quality_s::resolution_opts::desc            = {
    enum_names_trp_phase_quality_s_resolution_e__values,
    2,
    "trp_phase_quality_s::resolution_e_"};
float trp_phase_quality_s::resolution_opts::to_number() const
{
  static const float numbers[] = {0.1, 1.0};
  return map_enum_number(numbers, 2, value, "trp_phase_quality_s::resolution_e_");
}
const char* trp_phase_quality_s::resolution_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.1", "1"};
  return convert_enum_idx(number_strs, 2, value, "trp_phase_quality_s::resolution_e_");
}

// TrpMeasurementQuality-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_trp_meas_quality_ext_ies_o[]    = {133};
static const uint8_t  objset_crit_trp_meas_quality_ext_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_trp_meas_quality_ext_ies_o[] = {
    trp_meas_quality_ext_ies_o::value_c::types::trp_phase_quality};
static const uint8_t                    objset_presence_trp_meas_quality_ext_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_trp_meas_quality_ext_ies_o[]  = {
    {objset_crit_trp_meas_quality_ext_ies_o, crit_e::nulltype},
    {objset_value_trp_meas_quality_ext_ies_o, trp_meas_quality_ext_ies_o::value_c::types::nulltype},
    {objset_presence_trp_meas_quality_ext_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc trp_meas_quality_ext_ies_o::desc = {objset_id_trp_meas_quality_ext_ies_o,
                                                                   1,
                                                                   "id",
                                                                   objset_columns_trp_meas_quality_ext_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_trp_meas_quality_ext_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_phase_quality_s::desc}};
static const asn1::table::field_name choice_table_trp_meas_quality_ext_ies_o_value_c_names[] = {
    {"TRPPhaseQuality", nullptr}};
const asn1::table::choice_desc trp_meas_quality_ext_ies_o::value_c::desc = {
    asn1::table::enum_int_type<trp_meas_quality_ext_ies_o::value_c::types>(),
    trp_meas_quality_ext_ies_o::value_c::types::nof_types,
    trp_meas_quality_ext_ies_o::value_c::types::nof_exts,
    trp_meas_quality_ext_ies_o::value_c::types::has_ext,
    true,
    choice_table_trp_meas_quality_ext_ies_o_value_c_alternatives,
    nullptr,
    choice_table_trp_meas_quality_ext_ies_o_value_c_names,
    "trp_meas_quality_ext_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(trp_meas_quality_ext_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_trp_meas_quality_ext_ies_o_value_c_types_values[] = {"TRPPhaseQuality"};
const asn1::table::enum_desc trp_meas_quality_ext_ies_o::value_c::types_opts::desc        = {
    enum_names_trp_meas_quality_ext_ies_o_value_c_types_values,
    1,
    "trp_meas_quality_ext_ies_o::value_c::types"};

// TrpMeasurementQuality ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_trp_meas_quality_c_alternatives[] = {
    &asn1::table::alternative_ops<trp_meas_timing_quality_s, false>::value,
    &asn1::table::alternative_ops<trp_meas_angle_quality_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<trp_meas_quality_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc trp_meas_quality_c::storage_desc = {
    choice_storage_trp_meas_quality_c_alternatives,
    3,
    "trp_meas_quality_c"};

trp_meas_timing_quality_s& trp_meas_quality_c::set_timing_meas_quality()
{
  set(types::timing_meas_quality);
  return c.get<trp_meas_timing_quality_s>();
}
trp_meas_angle_quality_s& trp_meas_quality_c::set_angle_meas_quality()
{
  set(types::angle_meas_quality);
  return c.get<trp_meas_angle_quality_s>();
}
protocol_ie_single_container_s<trp_meas_quality_ext_ies_o>& trp_meas_quality_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<trp_meas_quality_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_trp_meas_quality_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_meas_timing_quality_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_meas_angle_quality_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<trp_meas_quality_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_trp_meas_quality_c_names[] = {{"timingMeasQuality", nullptr},
                                                                                {"angleMeasQuality", nullptr},
                                                                                {"choice-Extension", nullptr}};
const asn1::table::choice_desc trp_meas_quality_c::desc = {asn1::table::enum_int_type<trp_meas_quality_c::types>(),
                                                           trp_meas_quality_c::types::nof_types,
                                                           trp_meas_quality_c::types::nof_exts,
                                                           trp_meas_quality_c::types::has_ext,
                                                           true,
                                                           choice_table_trp_meas_quality_c_alternatives,
                                                           nullptr,
                                                           choice_table_trp_meas_quality_c_names,
                                                           "trp_meas_quality_c",
                                                           offsetof(trp_meas_quality_c, c.type),
                                                           offsetof(trp_meas_quality_c, c.value),
                                                           false,
                                                           &trp_meas_quality_c::storage_desc};

static const char*           enum_names_trp_meas_quality_c_types_values[] = {"timingMeasQuality",
                                                                             "angleMeasQuality",
                                                                             "choice-Extension"};
const asn1::table::enum_desc trp_meas_quality_c::types_opts::desc         = {enum_names_trp_meas_quality_c_types_values,
                                                                             3,
                                                                             "trp_meas_quality_c::types"};

// LCS-to-GCS-Translation ::= SEQUENCE
static const asn1::table::seq_field seq_table_lcs_to_gcs_translation_s_fields[] = {
    {offsetof(lcs_to_gcs_translation_s, alpha),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(3599),
     nullptr},
    {offsetof(lcs_to_gcs_translation_s, beta),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(3599),
     nullptr},
    {offsetof(lcs_to_gcs_translation_s, gamma),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(3599),
     nullptr},
    {offsetof(lcs_to_gcs_translation_s, ie_exts),
     (int32_t)offsetof(lcs_to_gcs_translation_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(lcs_to_gcs_translation_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_lcs_to_gcs_translation_s_names[] = {{"alpha", nullptr},
                                                                                   {"beta", nullptr},
                                                                                   {"gamma", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_lcs_to_gcs_translation_s_opts[]  = {
    {3, (int32_t)offsetof(lcs_to_gcs_translation_s, ie_exts_present)}};
const asn1::table::seq_desc lcs_to_gcs_translation_s::desc = {
    {seq_table_lcs_to_gcs_translation_s_fields, 4, UINT64_C(0x7), seq_table_lcs_to_gcs_translation_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_lcs_to_gcs_translation_s_names};

// UL-AoA ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_ao_a_s_fields[] = {
    {offsetof(ul_ao_a_s, azimuth_ao_a),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(3599),
     nullptr},
    {offsetof(ul_ao_a_s, zenith_ao_a),
     (int32_t)offsetof(ul_ao_a_s, zenith_ao_a_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1799),
     nullptr},
    {offsetof(ul_ao_a_s, lcs_to_gcs_translation),
     (int32_t)offsetof(ul_ao_a_s, lcs_to_gcs_translation_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &lcs_to_gcs_translation_s::desc},
    {offsetof(ul_ao_a_s, ie_exts),
     (int32_t)offsetof(ul_ao_a_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ul_ao_a_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ul_ao_a_s_names[] = {{"azimuthAoA", nullptr},
                                                                    {"zenithAoA", nullptr},
                                                                    {"lCS-to-GCS-Translation", nullptr},
                                                                    {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ul_ao_a_s_opts[]  = {
    {1, (int32_t)offsetof(ul_ao_a_s, zenith_ao_a_present)},
    {2, (int32_t)offsetof(ul_ao_a_s, lcs_to_gcs_translation_present)},
    {3, (int32_t)offsetof(ul_ao_a_s, ie_exts_present)}};
const asn1::table::seq_desc ul_ao_a_s::desc = {
    {seq_table_ul_ao_a_s_fields, 4, UINT64_C(0x1), seq_table_ul_ao_a_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_ul_ao_a_s_names};

// ZoA ::= SEQUENCE
static const asn1::table::seq_field seq_table_zo_a_s_fields[] = {
    {offsetof(zo_a_s, zenith_ao_a),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1799),
     nullptr},
    {offsetof(zo_a_s, lcs_to_gcs_translation),
     (int32_t)offsetof(zo_a_s, lcs_to_gcs_translation_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &lcs_to_gcs_translation_s::desc},
    {offsetof(zo_a_s, ie_exts),
     (int32_t)offsetof(zo_a_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(zo_a_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_zo_a_s_names[] = {{"zenithAoA", nullptr},
                                                                 {"lCS-to-GCS-Translation", nullptr},
                                                                 {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_zo_a_s_opts[]  = {
    {1, (int32_t)offsetof(zo_a_s, lcs_to_gcs_translation_present)},
    {2, (int32_t)offsetof(zo_a_s, ie_exts_present)}};
const asn1::table::seq_desc zo_a_s::desc = {{seq_table_zo_a_s_fields, 3, UINT64_C(0x1), seq_table_zo_a_s_opts, 2},
                                            true,
                                            true,
                                            nullptr,
                                            0,
                                            seq_table_zo_a_s_names};

// MultipleULAoA-Item ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_multiple_ul_ao_a_item_c_alternatives[] = {
    &asn1::table::alternative_ops<ul_ao_a_s, false>::value,
    &asn1::table::alternative_ops<zo_a_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<multiple_ul_ao_a_item_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc multiple_ul_ao_a_item_c::storage_desc = {
    choice_storage_multiple_ul_ao_a_item_c_alternatives,
    3,
    "multiple_ul_ao_a_item_c"};

ul_ao_a_s& multiple_ul_ao_a_item_c::set_ul_ao_a()
{
  set(types::ul_ao_a);
  return c.get<ul_ao_a_s>();
}
zo_a_s& multiple_ul_ao_a_item_c::set_ul_zo_a()
{
  set(types::ul_zo_a);
  return c.get<zo_a_s>();
}
protocol_ie_single_container_s<multiple_ul_ao_a_item_ext_ies_o>& multiple_ul_ao_a_item_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<multiple_ul_ao_a_item_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_multiple_ul_ao_a_item_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_ao_a_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::sequence, false, false, 0, 0, nullptr, &zo_a_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<multiple_ul_ao_a_item_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_multiple_ul_ao_a_item_c_names[] = {{"uL-AoA", nullptr},
                                                                                     {"ul-ZoA", nullptr},
                                                                                     {"choice-extension", nullptr}};
const asn1::table::choice_desc       multiple_ul_ao_a_item_c::desc                = {
    asn1::table::enum_int_type<multiple_ul_ao_a_item_c::types>(),
    multiple_ul_ao_a_item_c::types::nof_types,
    multiple_ul_ao_a_item_c::types::nof_exts,
    multiple_ul_ao_a_item_c::types::has_ext,
    true,
    choice_table_multiple_ul_ao_a_item_c_alternatives,
    nullptr,
    choice_table_multiple_ul_ao_a_item_c_names,
    "multiple_ul_ao_a_item_c",
    offsetof(multiple_ul_ao_a_item_c, c.type),
    offsetof(multiple_ul_ao_a_item_c, c.value),
    false,
    &multiple_ul_ao_a_item_c::storage_desc};

static const char* enum_names_multiple_ul_ao_a_item_c_types_values[]   = {"uL-AoA", "ul-ZoA", "choice-extension"};
const asn1::table::enum_desc multiple_ul_ao_a_item_c::types_opts::desc = {
    enum_names_multiple_ul_ao_a_item_c_types_values,
    3,
    "multiple_ul_ao_a_item_c::types"};

// MultipleULAoA ::= SEQUENCE
static const asn1::table::seq_field seq_table_multiple_ul_ao_a_s_fields[] = {
    {offsetof(multiple_ul_ao_a_s, multiple_ul_ao_a),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(multiple_ul_ao_a_s::multiple_ul_ao_a),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &multiple_ul_ao_a_item_c::desc},
    {offsetof(multiple_ul_ao_a_s, ie_exts),
     (int32_t)offsetof(multiple_ul_ao_a_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(multiple_ul_ao_a_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_multiple_ul_ao_a_s_names[] = {{"multipleULAoA", nullptr},
                                                                             {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_multiple_ul_ao_a_s_opts[]  = {
    {1, (int32_t)offsetof(multiple_ul_ao_a_s, ie_exts_present)}};
const asn1::table::seq_desc multiple_ul_ao_a_s::desc = {
    {seq_table_multiple_ul_ao_a_s_fields, 2, UINT64_C(0x1), seq_table_multiple_ul_ao_a_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_multiple_ul_ao_a_s_names};

// UL-SRS-RSRPP ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_srs_rsrp_p_s_fields[] = {
    {offsetof(ul_srs_rsrp_p_s, first_path_rsrp_p),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(126),
     nullptr},
    {offsetof(ul_srs_rsrp_p_s, ie_exts),
     (int32_t)offsetof(ul_srs_rsrp_p_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ul_srs_rsrp_p_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ul_srs_rsrp_p_s_names[] = {{"firstPathRSRPP", nullptr},
                                                                          {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ul_srs_rsrp_p_s_opts[]  = {
    {1, (int32_t)offsetof(ul_srs_rsrp_p_s, ie_exts_present)}};
const asn1::table::seq_desc ul_srs_rsrp_p_s::desc = {
    {seq_table_ul_srs_rsrp_p_s_fields, 2, UINT64_C(0x1), seq_table_ul_srs_rsrp_p_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_ul_srs_rsrp_p_s_names};

static const uint32_t objset_id_add_path_list_item_ext_ies_o[]   = {74, 96};
static const uint8_t  objset_crit_add_path_list_item_ext_ies_o[] = {crit_e::ignore, crit_e::ignore};

// AdditionalPathListItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_add_path_list_item_ext_ies_container_values[] = {
    {offsetof(add_path_list_item_ext_ies_container, multiple_ul_ao_a),
     (int32_t)offsetof(add_path_list_item_ext_ies_container, multiple_ul_ao_a_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &multiple_ul_ao_a_s::desc},
    {offsetof(add_path_list_item_ext_ies_container, path_pwr),
     (int32_t)offsetof(add_path_list_item_ext_ies_container, path_pwr_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_srs_rsrp_p_s::desc}};
static const int32_t ie_table_add_path_list_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(add_path_list_item_ext_ies_container, multiple_ul_ao_a_present),
    (int32_t)offsetof(add_path_list_item_ext_ies_container, path_pwr_present)};
const asn1::table::ie_container_desc add_path_list_item_ext_ies_container::desc = {
    objset_id_add_path_list_item_ext_ies_o,
    objset_crit_add_path_list_item_ext_ies_o,
    ie_table_add_path_list_item_ext_ies_container_values,
    ie_table_add_path_list_item_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// AdditionalPathListItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_add_path_list_item_s_fields[] = {
    {offsetof(add_path_list_item_s, relative_time_of_path),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &relative_path_delay_c::desc},
    {offsetof(add_path_list_item_s, path_quality),
     (int32_t)offsetof(add_path_list_item_s, path_quality_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_meas_quality_c::desc},
    {offsetof(add_path_list_item_s, ie_exts),
     (int32_t)offsetof(add_path_list_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &add_path_list_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_add_path_list_item_s_names[] = {{"relativeTimeOfPath", nullptr},
                                                                               {"pathQuality", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_add_path_list_item_s_opts[]  = {
    {1, (int32_t)offsetof(add_path_list_item_s, path_quality_present)},
    {2, (int32_t)offsetof(add_path_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc add_path_list_item_s::desc = {
    {seq_table_add_path_list_item_s_fields, 3, UINT64_C(0x1), seq_table_add_path_list_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_add_path_list_item_s_names};

// DL-PRS-ResourceSet-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_prs_res_set_item_s_fields[] = {
    {offsetof(dl_prs_res_set_item_s, dl_prs_res_set_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(1),
     UINT64_C(8),
     nullptr},
    {offsetof(dl_prs_res_set_item_s, ie_exts),
     (int32_t)offsetof(dl_prs_res_set_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(dl_prs_res_set_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_dl_prs_res_set_item_s_names[] = {{"dl-prs-ResourceSetIndex", nullptr},
                                                                                {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_dl_prs_res_set_item_s_opts[]  = {
    {1, (int32_t)offsetof(dl_prs_res_set_item_s, ie_exts_present)}};
const asn1::table::seq_desc dl_prs_res_set_item_s::desc = {
    {seq_table_dl_prs_res_set_item_s_fields, 2, UINT64_C(0x1), seq_table_dl_prs_res_set_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_dl_prs_res_set_item_s_names};

// AggregatedPRSResourceSet-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_aggr_prs_res_set_item_s_fields[] = {
    {offsetof(aggr_prs_res_set_item_s, dl_prs_res_set_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     3,
     &asn1::table::item_list_codec<decltype(aggr_prs_res_set_item_s::dl_prs_res_set_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &dl_prs_res_set_item_s::desc},
    {offsetof(aggr_prs_res_set_item_s, ie_exts),
     (int32_t)offsetof(aggr_prs_res_set_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(aggr_prs_res_set_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_aggr_prs_res_set_item_s_names[] = {{"dl-PRS-ResourceSet-List", nullptr},
                                                                                  {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_aggr_prs_res_set_item_s_opts[]  = {
    {1, (int32_t)offsetof(aggr_prs_res_set_item_s, ie_exts_present)}};
const asn1::table::seq_desc aggr_prs_res_set_item_s::desc = {
    {seq_table_aggr_prs_res_set_item_s_fields, 2, UINT64_C(0x1), seq_table_aggr_prs_res_set_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_aggr_prs_res_set_item_s_names};

static const uint32_t objset_id_aggr_pos_srs_res_id_item_ext_ies_o[]   = {154, 156, 155};
static const uint8_t  objset_crit_aggr_pos_srs_res_id_item_ext_ies_o[] = {crit_e::ignore,
                                                                          crit_e::ignore,
                                                                          crit_e::ignore};

// AggregatedPosSRSResourceID-Item-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_aggr_pos_srs_res_id_item_ext_ies_container_values[] = {
    {offsetof(aggr_pos_srs_res_id_item_ext_ies_container, point_a),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(aggr_pos_srs_res_id_item_ext_ies_container, scs_specific_carrier),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &scs_specific_carrier_s::desc},
    {offsetof(aggr_pos_srs_res_id_item_ext_ies_container, nr_pci),
     (int32_t)offsetof(aggr_pos_srs_res_id_item_ext_ies_container, nr_pci_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1007),
     nullptr}};
static const int32_t ie_table_aggr_pos_srs_res_id_item_ext_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(aggr_pos_srs_res_id_item_ext_ies_container, nr_pci_present)};
const asn1::table::ie_container_desc aggr_pos_srs_res_id_item_ext_ies_container::desc = {
    objset_id_aggr_pos_srs_res_id_item_ext_ies_o,
    objset_crit_aggr_pos_srs_res_id_item_ext_ies_o,
    ie_table_aggr_pos_srs_res_id_item_ext_ies_container_values,
    ie_table_aggr_pos_srs_res_id_item_ext_ies_container_presences,
    3,
    2,
    1,
    65535,
    true,
    "extensionValue"};

// AggregatedPosSRSResourceID-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_aggr_pos_srs_res_id_item_s_fields[] = {
    {offsetof(aggr_pos_srs_res_id_item_s, srs_pos_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(aggr_pos_srs_res_id_item_s, ie_exts),
     (int32_t)offsetof(aggr_pos_srs_res_id_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &aggr_pos_srs_res_id_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_aggr_pos_srs_res_id_item_s_names[] = {{"sRSPosResource-ID", nullptr},
                                                                                     {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_aggr_pos_srs_res_id_item_s_opts[]  = {
    {1, (int32_t)offsetof(aggr_pos_srs_res_id_item_s, ie_exts_present)}};
const asn1::table::seq_desc aggr_pos_srs_res_id_item_s::desc = {
    {seq_table_aggr_pos_srs_res_id_item_s_fields, 2, UINT64_C(0x1), seq_table_aggr_pos_srs_res_id_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_aggr_pos_srs_res_id_item_s_names};

// Expected-Azimuth-AoA ::= SEQUENCE
static const asn1::table::seq_field seq_table_expected_azimuth_ao_a_s_fields[] = {
    {offsetof(expected_azimuth_ao_a_s, expected_azimuth_ao_a_value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(3599),
     nullptr},
    {offsetof(expected_azimuth_ao_a_s, expected_azimuth_ao_a_uncertainty),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(3599),
     nullptr},
    {offsetof(expected_azimuth_ao_a_s, ie_exts),
     (int32_t)offsetof(expected_azimuth_ao_a_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(expected_azimuth_ao_a_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_expected_azimuth_ao_a_s_names[] = {
    {"expected-Azimuth-AoA-value", nullptr},
    {"expected-Azimuth-AoA-uncertainty", nullptr},
    {"iE-extensions", nullptr}};
static const asn1::table::seq_opt seq_table_expected_azimuth_ao_a_s_opts[] = {
    {2, (int32_t)offsetof(expected_azimuth_ao_a_s, ie_exts_present)}};
const asn1::table::seq_desc expected_azimuth_ao_a_s::desc = {
    {seq_table_expected_azimuth_ao_a_s_fields, 3, UINT64_C(0x3), seq_table_expected_azimuth_ao_a_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_expected_azimuth_ao_a_s_names};

// Expected-Zenith-AoA ::= SEQUENCE
static const asn1::table::seq_field seq_table_expected_zenith_ao_a_s_fields[] = {
    {offsetof(expected_zenith_ao_a_s, expected_zenith_ao_a_value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1799),
     nullptr},
    {offsetof(expected_zenith_ao_a_s, expected_zenith_ao_a_uncertainty),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1799),
     nullptr},
    {offsetof(expected_zenith_ao_a_s, ie_exts),
     (int32_t)offsetof(expected_zenith_ao_a_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(expected_zenith_ao_a_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_expected_zenith_ao_a_s_names[] = {
    {"expected-Zenith-AoA-value", nullptr},
    {"expected-Zenith-AoA-uncertainty", nullptr},
    {"iE-extensions", nullptr}};
static const asn1::table::seq_opt seq_table_expected_zenith_ao_a_s_opts[] = {
    {2, (int32_t)offsetof(expected_zenith_ao_a_s, ie_exts_present)}};
const asn1::table::seq_desc expected_zenith_ao_a_s::desc = {
    {seq_table_expected_zenith_ao_a_s_fields, 3, UINT64_C(0x3), seq_table_expected_zenith_ao_a_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_expected_zenith_ao_a_s_names};

// Expected-UL-AoA ::= SEQUENCE
static const asn1::table::seq_field seq_table_expected_ul_ao_a_s_fields[] = {
    {offsetof(expected_ul_ao_a_s, expected_azimuth_ao_a),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &expected_azimuth_ao_a_s::desc},
    {offsetof(expected_ul_ao_a_s, expected_zenith_ao_a),
     (int32_t)offsetof(expected_ul_ao_a_s, expected_zenith_ao_a_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &expected_zenith_ao_a_s::desc},
    {offsetof(expected_ul_ao_a_s, ie_exts),
     (int32_t)offsetof(expected_ul_ao_a_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(expected_ul_ao_a_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_expected_ul_ao_a_s_names[] = {{"expected-Azimuth-AoA", nullptr},
                                                                             {"expected-Zenith-AoA", nullptr},
                                                                             {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_expected_ul_ao_a_s_opts[]  = {
    {1, (int32_t)offsetof(expected_ul_ao_a_s, expected_zenith_ao_a_present)},
    {2, (int32_t)offsetof(expected_ul_ao_a_s, ie_exts_present)}};
const asn1::table::seq_desc expected_ul_ao_a_s::desc = {
    {seq_table_expected_ul_ao_a_s_fields, 3, UINT64_C(0x1), seq_table_expected_ul_ao_a_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_expected_ul_ao_a_s_names};

// Expected-ZoA-only ::= SEQUENCE
static const asn1::table::seq_field seq_table_expected_zo_a_only_s_fields[] = {
    {offsetof(expected_zo_a_only_s, expected_zo_a_only),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &expected_zenith_ao_a_s::desc},
    {offsetof(expected_zo_a_only_s, ie_exts),
     (int32_t)offsetof(expected_zo_a_only_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(expected_zo_a_only_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_expected_zo_a_only_s_names[] = {{"expected-ZoA-only", nullptr},
                                                                               {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_expected_zo_a_only_s_opts[]  = {
    {1, (int32_t)offsetof(expected_zo_a_only_s, ie_exts_present)}};
const asn1::table::seq_desc expected_zo_a_only_s::desc = {
    {seq_table_expected_zo_a_only_s_fields, 2, UINT64_C(0x1), seq_table_expected_zo_a_only_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_expected_zo_a_only_s_names};

// AngleMeasurementType ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_angle_meas_type_c_alternatives[] = {
    &asn1::table::alternative_ops<expected_ul_ao_a_s, false>::value,
    &asn1::table::alternative_ops<expected_zo_a_only_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<angle_meas_type_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc angle_meas_type_c::storage_desc = {choice_storage_angle_meas_type_c_alternatives,
                                                                          3,
                                                                          "angle_meas_type_c"};

expected_ul_ao_a_s& angle_meas_type_c::set_expected_ul_ao_a()
{
  set(types::expected_ul_ao_a);
  return c.get<expected_ul_ao_a_s>();
}
expected_zo_a_only_s& angle_meas_type_c::set_expected_zo_a()
{
  set(types::expected_zo_a);
  return c.get<expected_zo_a_only_s>();
}
protocol_ie_single_container_s<angle_meas_type_ext_ies_o>& angle_meas_type_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<angle_meas_type_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_angle_meas_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &expected_ul_ao_a_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &expected_zo_a_only_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<angle_meas_type_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_angle_meas_type_c_names[] = {{"expected-ULAoA", nullptr},
                                                                               {"expected-ZoA", nullptr},
                                                                               {"choice-extension", nullptr}};
const asn1::table::choice_desc       angle_meas_type_c::desc = {asn1::table::enum_int_type<angle_meas_type_c::types>(),
                                                                angle_meas_type_c::types::nof_types,
                                                                angle_meas_type_c::types::nof_exts,
                                                                angle_meas_type_c::types::has_ext,
                                                                true,
                                                                choice_table_angle_meas_type_c_alternatives,
                                                                nullptr,
                                                                choice_table_angle_meas_type_c_names,
                                                                "angle_meas_type_c",
                                                                offsetof(angle_meas_type_c, c.type),
                                                                offsetof(angle_meas_type_c, c.value),
                                                                false,
                                                                &angle_meas_type_c::storage_desc};

static const char* enum_names_angle_meas_type_c_types_values[] = {"expected-ULAoA", "expected-ZoA", "choice-extension"};
const asn1::table::enum_desc angle_meas_type_c::types_opts::desc = {enum_names_angle_meas_type_c_types_values,
                                                                    3,
                                                                    "angle_meas_type_c::types"};

// AoA-AssistanceInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_ao_a_assist_info_s_fields[] = {
    {offsetof(ao_a_assist_info_s, angle_meas),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &angle_meas_type_c::desc},
    {offsetof(ao_a_assist_info_s, lcs_to_gcs_translation),
     (int32_t)offsetof(ao_a_assist_info_s, lcs_to_gcs_translation_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &lcs_to_gcs_translation_s::desc},
    {offsetof(ao_a_assist_info_s, ie_exts),
     (int32_t)offsetof(ao_a_assist_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ao_a_assist_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ao_a_assist_info_s_names[] = {{"angleMeasurement", nullptr},
                                                                             {"lCS-to-GCS-Translation", nullptr},
                                                                             {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ao_a_assist_info_s_opts[]  = {
    {1, (int32_t)offsetof(ao_a_assist_info_s, lcs_to_gcs_translation_present)},
    {2, (int32_t)offsetof(ao_a_assist_info_s, ie_exts_present)}};
const asn1::table::seq_desc ao_a_assist_info_s::desc = {
    {seq_table_ao_a_assist_info_s_fields, 3, UINT64_C(0x1), seq_table_ao_a_assist_info_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_ao_a_assist_info_s_names};

// BroadcastPeriodicity ::= ENUMERATED
static const char* enum_names_broadcast_periodicity_e_values[] =
    {"ms80", "ms160", "ms320", "ms640", "ms1280", "ms2560", "ms5120"};
const asn1::table::enum_desc broadcast_periodicity_opts::desc = {enum_names_broadcast_periodicity_e_values,
                                                                 7,
                                                                 "broadcast_periodicity_e"};
uint16_t                     broadcast_periodicity_opts::to_number() const
{
  static const uint16_t numbers[] = {80, 160, 320, 640, 1280, 2560, 5120};
  return map_enum_number(numbers, 7, value, "broadcast_periodicity_e");
}

// PosSIB-Type ::= ENUMERATED
static const char* enum_names_pos_sib_type_e_values[] = {
    "posSibType1-1",   "posSibType1-2",  "posSibType1-3",  "posSibType1-4",  "posSibType1-5",   "posSibType1-6",
    "posSibType1-7",   "posSibType1-8",  "posSibType2-1",  "posSibType2-2",  "posSibType2-3",   "posSibType2-4",
    "posSibType2-5",   "posSibType2-6",  "posSibType2-7",  "posSibType2-8",  "posSibType2-9",   "posSibType2-10",
    "posSibType2-11",  "posSibType2-12", "posSibType2-13", "posSibType2-14", "posSibType2-15",  "posSibType2-16",
    "posSibType2-17",  "posSibType2-18", "posSibType2-19", "posSibType2-20", "posSibType2-21",  "posSibType2-22",
    "posSibType2-23",  "posSibType2-24", "posSibType2-25", "posSibType3-1",  "posSibType4-1",   "posSibType5-1",
    "posSibType6-1",   "posSibType6-2",  "posSibType6-3",  "posSibType1-9",  "posSibType1-10",  "posSibType6-4",
    "posSibType6-5",   "posSibType6-6",  "posSibType1-11", "posSibType1-12", "posSibType2-17a", "posSibType2-18a",
    "posSibType2-20a", "posSibType2-26", "posSibType2-27", "posSibType6-7",  "posSibType7-1",   "posSibType7-2",
    "posSibType7-3",   "posSibType7-4"};
const asn1::table::enum_desc pos_sib_type_opts::desc = {enum_names_pos_sib_type_e_values, 56, "pos_sib_type_e"};

static const asn1::table::seq_field seq_table_pos_sib_segments_item_s__fields[] = {
    {offsetof(pos_sib_segments_item_s_, assist_data_sib_elem),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pos_sib_segments_item_s_::assist_data_sib_elem),
                                asn1::table::string_json>::value},
    {offsetof(pos_sib_segments_item_s_, ie_exts),
     (int32_t)offsetof(pos_sib_segments_item_s_, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pos_sib_segments_item_s_::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pos_sib_segments_item_s__names[] = {
    {"assistanceDataSIBelement", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pos_sib_segments_item_s__opts[] = {
    {1, (int32_t)offsetof(pos_sib_segments_item_s_, ie_exts_present)}};
const asn1::table::seq_desc pos_sib_segments_item_s_::desc = {
    {seq_table_pos_sib_segments_item_s__fields, 2, UINT64_C(0x1), seq_table_pos_sib_segments_item_s__opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_pos_sib_segments_item_s__names};

// AssistanceInformationMetaData ::= SEQUENCE
static const asn1::table::seq_field seq_table_assist_info_meta_data_s_fields[] = {
    {offsetof(assist_info_meta_data_s, encrypted),
     (int32_t)offsetof(assist_info_meta_data_s, encrypted_present),
     asn1::table::enum_op<decltype(assist_info_meta_data_s::encrypted)>(),
     decltype(assist_info_meta_data_s::encrypted)::has_ext,
     false,
     decltype(assist_info_meta_data_s::encrypted)::nof_types,
     decltype(assist_info_meta_data_s::encrypted)::nof_exts,
     nullptr,
     &assist_info_meta_data_s::encrypted_opts::desc},
    {offsetof(assist_info_meta_data_s, gns_si_d),
     (int32_t)offsetof(assist_info_meta_data_s, gns_si_d_present),
     asn1::table::enum_op<decltype(assist_info_meta_data_s::gns_si_d)>(),
     decltype(assist_info_meta_data_s::gns_si_d)::has_ext,
     false,
     decltype(assist_info_meta_data_s::gns_si_d)::nof_types,
     decltype(assist_info_meta_data_s::gns_si_d)::nof_exts,
     nullptr,
     &assist_info_meta_data_s::gns_si_d_opts::desc},
    {offsetof(assist_info_meta_data_s, sba_si_d),
     (int32_t)offsetof(assist_info_meta_data_s, sba_si_d_present),
     asn1::table::enum_op<decltype(assist_info_meta_data_s::sba_si_d)>(),
     decltype(assist_info_meta_data_s::sba_si_d)::has_ext,
     false,
     decltype(assist_info_meta_data_s::sba_si_d)::nof_types,
     decltype(assist_info_meta_data_s::sba_si_d)::nof_exts,
     nullptr,
     &assist_info_meta_data_s::sba_si_d_opts::desc},
    {offsetof(assist_info_meta_data_s, ie_exts),
     (int32_t)offsetof(assist_info_meta_data_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(assist_info_meta_data_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_assist_info_meta_data_s_names[] = {{"encrypted", nullptr},
                                                                                  {"gNSSID", nullptr},
                                                                                  {"sBASID", nullptr},
                                                                                  {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_assist_info_meta_data_s_opts[]  = {
    {0, (int32_t)offsetof(assist_info_meta_data_s, encrypted_present)},
    {1, (int32_t)offsetof(assist_info_meta_data_s, gns_si_d_present)},
    {2, (int32_t)offsetof(assist_info_meta_data_s, sba_si_d_present)},
    {3, (int32_t)offsetof(assist_info_meta_data_s, ie_exts_present)}};
const asn1::table::seq_desc assist_info_meta_data_s::desc = {
    {seq_table_assist_info_meta_data_s_fields, 4, UINT64_C(0x0), seq_table_assist_info_meta_data_s_opts, 4},
    true,
    true,
    nullptr,
    0,
    seq_table_assist_info_meta_data_s_names};

static const char*           enum_names_assist_info_meta_data_s_encrypted_e__values[] = {"true"};
const asn1::table::enum_desc assist_info_meta_data_s::encrypted_opts::desc            = {
    enum_names_assist_info_meta_data_s_encrypted_e__values,
    1,
    "assist_info_meta_data_s::encrypted_e_"};

static const char* enum_names_assist_info_meta_data_s_gns_si_d_e__values[] =
    {"gps", "sbas", "qzss", "galileo", "glonass", "bds", "navic"};
const asn1::table::enum_desc assist_info_meta_data_s::gns_si_d_opts::desc = {
    enum_names_assist_info_meta_data_s_gns_si_d_e__values,
    7,
    "assist_info_meta_data_s::gns_si_d_e_"};

static const char* enum_names_assist_info_meta_data_s_sba_si_d_e__values[] = {"waas", "egnos", "msas", "gagan"};
const asn1::table::enum_desc assist_info_meta_data_s::sba_si_d_opts::desc  = {
    enum_names_assist_info_meta_data_s_sba_si_d_e__values,
    4,
    "assist_info_meta_data_s::sba_si_d_e_"};

static const asn1::table::seq_field seq_table_pos_sibs_item_s__fields[] = {
    {offsetof(pos_sibs_item_s_, pos_sib_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pos_sibs_item_s_::pos_sib_type)>(),
     decltype(pos_sibs_item_s_::pos_sib_type)::has_ext,
     false,
     decltype(pos_sibs_item_s_::pos_sib_type)::nof_types,
     decltype(pos_sibs_item_s_::pos_sib_type)::nof_exts,
     nullptr,
     &pos_sib_type_opts::desc},
    {offsetof(pos_sibs_item_s_, pos_sib_segments),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(pos_sibs_item_s_::pos_sib_segments),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pos_sib_segments_item_s_::desc},
    {offsetof(pos_sibs_item_s_, assist_info_meta_data),
     (int32_t)offsetof(pos_sibs_item_s_, assist_info_meta_data_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &assist_info_meta_data_s::desc},
    {offsetof(pos_sibs_item_s_, broadcast_prio),
     (int32_t)offsetof(pos_sibs_item_s_, broadcast_prio_present),
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(16),
     nullptr},
    {offsetof(pos_sibs_item_s_, ie_exts),
     (int32_t)offsetof(pos_sibs_item_s_, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pos_sibs_item_s_::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pos_sibs_item_s__names[] = {{"posSIB-Type", nullptr},
                                                                           {"posSIB-Segments", nullptr},
                                                                           {"assistanceInformationMetaData", nullptr},
                                                                           {"broadcastPriority", nullptr},
                                                                           {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_pos_sibs_item_s__opts[]  = {
    {2, (int32_t)offsetof(pos_sibs_item_s_, assist_info_meta_data_present)},
    {3, (int32_t)offsetof(pos_sibs_item_s_, broadcast_prio_present)},
    {4, (int32_t)offsetof(pos_sibs_item_s_, ie_exts_present)}};
const asn1::table::seq_desc pos_sibs_item_s_::desc = {
    {seq_table_pos_sibs_item_s__fields, 5, UINT64_C(0x3), seq_table_pos_sibs_item_s__opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_pos_sibs_item_s__names};

static const asn1::table::seq_field seq_table_sys_info_item_s__fields[] = {
    {offsetof(sys_info_item_s_, broadcast_periodicity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sys_info_item_s_::broadcast_periodicity)>(),
     decltype(sys_info_item_s_::broadcast_periodicity)::has_ext,
     false,
     decltype(sys_info_item_s_::broadcast_periodicity)::nof_types,
     decltype(sys_info_item_s_::broadcast_periodicity)::nof_exts,
     nullptr,
     &broadcast_periodicity_opts::desc},
    {offsetof(sys_info_item_s_, pos_sibs),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(sys_info_item_s_::pos_sibs), asn1::table::sequence_item_ops, false, true>::
         value,
     &pos_sibs_item_s_::desc},
    {offsetof(sys_info_item_s_, ie_exts),
     (int32_t)offsetof(sys_info_item_s_, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sys_info_item_s_::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_sys_info_item_s__names[] = {{"broadcastPeriodicity", nullptr},
                                                                           {"posSIBs", nullptr},
                                                                           {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_sys_info_item_s__opts[]  = {
    {2, (int32_t)offsetof(sys_info_item_s_, ie_exts_present)}};
const asn1::table::seq_desc sys_info_item_s_::desc = {
    {seq_table_sys_info_item_s__fields, 3, UINT64_C(0x3), seq_table_sys_info_item_s__opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_sys_info_item_s__names};

// Assistance-Information ::= SEQUENCE
static const asn1::table::seq_field seq_table_assist_info_s_fields[] = {
    {offsetof(assist_info_s, sys_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(assist_info_s::sys_info), asn1::table::sequence_item_ops, false, true>::
         value,
     &sys_info_item_s_::desc},
    {offsetof(assist_info_s, ie_exts),
     (int32_t)offsetof(assist_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(assist_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_assist_info_s_names[] = {{"systemInformation", nullptr},
                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_assist_info_s_opts[]  = {
    {1, (int32_t)offsetof(assist_info_s, ie_exts_present)}};
const asn1::table::seq_desc assist_info_s::desc = {
    {seq_table_assist_info_s_fields, 2, UINT64_C(0x1), seq_table_assist_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_assist_info_s_names};

// Outcome ::= ENUMERATED
static const char*           enum_names_outcome_e_values[] = {"failed"};
const asn1::table::enum_desc outcome_opts::desc            = {enum_names_outcome_e_values, 1, "outcome_e"};

static const asn1::table::seq_field seq_table_assist_info_fail_list_item_s__fields[] = {
    {offsetof(assist_info_fail_list_item_s_, pos_sib_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(assist_info_fail_list_item_s_::pos_sib_type)>(),
     decltype(assist_info_fail_list_item_s_::pos_sib_type)::has_ext,
     false,
     decltype(assist_info_fail_list_item_s_::pos_sib_type)::nof_types,
     decltype(assist_info_fail_list_item_s_::pos_sib_type)::nof_exts,
     nullptr,
     &pos_sib_type_opts::desc},
    {offsetof(assist_info_fail_list_item_s_, outcome),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(assist_info_fail_list_item_s_::outcome)>(),
     decltype(assist_info_fail_list_item_s_::outcome)::has_ext,
     false,
     decltype(assist_info_fail_list_item_s_::outcome)::nof_types,
     decltype(assist_info_fail_list_item_s_::outcome)::nof_exts,
     nullptr,
     &outcome_opts::desc},
    {offsetof(assist_info_fail_list_item_s_, ie_exts),
     (int32_t)offsetof(assist_info_fail_list_item_s_, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(assist_info_fail_list_item_s_::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_assist_info_fail_list_item_s__names[] = {{"posSIB-Type", nullptr},
                                                                                        {"outcome", nullptr},
                                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_assist_info_fail_list_item_s__opts[]  = {
    {2, (int32_t)offsetof(assist_info_fail_list_item_s_, ie_exts_present)}};
const asn1::table::seq_desc assist_info_fail_list_item_s_::desc = {
    {seq_table_assist_info_fail_list_item_s__fields, 3, UINT64_C(0x3), seq_table_assist_info_fail_list_item_s__opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_assist_info_fail_list_item_s__names};

// Bandwidth-Aggregation-Request-Indication ::= ENUMERATED
static const char*           enum_names_bw_aggregation_request_ind_e_values[] = {"true"};
const asn1::table::enum_desc bw_aggregation_request_ind_opts::desc = {enum_names_bw_aggregation_request_ind_e_values,
                                                                      1,
                                                                      "bw_aggregation_request_ind_e"};

// BandwidthSRS ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_bw_srs_c_alternatives[] = {
    &asn1::table::alternative_ops<bw_srs_c::fr1_e_, true>::value,
    &asn1::table::alternative_ops<bw_srs_c::fr2_e_, true>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<bw_srs_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc bw_srs_c::storage_desc = {choice_storage_bw_srs_c_alternatives, 3, "bw_srs_c"};

bw_srs_c::fr1_e_& bw_srs_c::set_fr1()
{
  set(types::fr1);
  return c.get<fr1_e_>();
}
bw_srs_c::fr2_e_& bw_srs_c::set_fr2()
{
  set(types::fr2);
  return c.get<fr2_e_>();
}
protocol_ie_single_container_s<bw_srs_ext_ies_o>& bw_srs_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<bw_srs_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_bw_srs_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<bw_srs_c::fr1_e_>(),
     bw_srs_c::fr1_e_::has_ext,
     false,
     bw_srs_c::fr1_e_::nof_types,
     bw_srs_c::fr1_e_::nof_exts,
     nullptr,
     &bw_srs_c::fr1_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<bw_srs_c::fr2_e_>(),
     bw_srs_c::fr2_e_::has_ext,
     false,
     bw_srs_c::fr2_e_::nof_types,
     bw_srs_c::fr2_e_::nof_exts,
     nullptr,
     &bw_srs_c::fr2_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<bw_srs_ext_ies_o>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_bw_srs_c_names[] = {{"fR1", nullptr},
                                                                      {"fR2", nullptr},
                                                                      {"choice-extension", nullptr}};
const asn1::table::choice_desc       bw_srs_c::desc                = {asn1::table::enum_int_type<bw_srs_c::types>(),
                                                                      bw_srs_c::types::nof_types,
                                                                      bw_srs_c::types::nof_exts,
                                                                      bw_srs_c::types::has_ext,
                                                                      true,
                                                                      choice_table_bw_srs_c_alternatives,
                                                                      nullptr,
                                                                      choice_table_bw_srs_c_names,
                                                                      "bw_srs_c",
                                                                      offsetof(bw_srs_c, c.type),
                                                                      offsetof(bw_srs_c, c.value),
                                                                      false,
                                                                      &bw_srs_c::storage_desc};

static const char*           enum_names_bw_srs_c_fr1_e__values[] = {"mHz5",
                                                                    "mHz10",
                                                                    "mHz20",
                                                                    "mHz40",
                                                                    "mHz50",
                                                                    "mHz80",
                                                                    "mHz100",
                                                                    "mHz160",
                                                                    "mHz200",
                                                                    "mHz15",
                                                                    "mHz25",
                                                                    "mHz30",
                                                                    "mHz60",
                                                                    "mHz35",
                                                                    "mHz45",
                                                                    "mHz70",
                                                                    "mHz90"};
const asn1::table::enum_desc bw_srs_c::fr1_opts::desc = {enum_names_bw_srs_c_fr1_e__values, 17, "bw_srs_c::fr1_e_"};
uint8_t                      bw_srs_c::fr1_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 40, 50, 80, 100, 160, 200, 15, 25, 30, 60, 35, 45, 70, 90};
  return map_enum_number(numbers, 17, value, "bw_srs_c::fr1_e_");
}

static const char* enum_names_bw_srs_c_fr2_e__values[] =
    {"mHz50", "mHz100", "mHz200", "mHz400", "mHz600", "mhz800", "mHz1600", "mHz2000"};
const asn1::table::enum_desc bw_srs_c::fr2_opts::desc = {enum_names_bw_srs_c_fr2_e__values, 8, "bw_srs_c::fr2_e_"};
uint16_t                     bw_srs_c::fr2_opts::to_number() const
{
  static const uint16_t numbers[] = {50, 100, 200, 400, 600, 800, 1600, 2000};
  return map_enum_number(numbers, 8, value, "bw_srs_c::fr2_e_");
}

static const char*           enum_names_bw_srs_c_types_values[] = {"fR1", "fR2", "choice-extension"};
const asn1::table::enum_desc bw_srs_c::types_opts::desc = {enum_names_bw_srs_c_types_values, 3, "bw_srs_c::types"};
uint8_t                      bw_srs_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "bw_srs_c::types");
}

// Broadcast ::= ENUMERATED
static const char*           enum_names_broadcast_e_values[] = {"start", "stop"};
const asn1::table::enum_desc broadcast_opts::desc            = {enum_names_broadcast_e_values, 2, "broadcast_e"};

// CGI-EUTRA ::= SEQUENCE
static const asn1::table::seq_field seq_table_cgi_eutra_s_fields[] = {
    {offsetof(cgi_eutra_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cgi_eutra_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(cgi_eutra_s, eutr_acell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cgi_eutra_s::eutr_acell_id), asn1::table::struct_json>::value},
    {offsetof(cgi_eutra_s, ie_exts),
     (int32_t)offsetof(cgi_eutra_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cgi_eutra_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_cgi_eutra_s_names[] = {{"pLMN-Identity", nullptr},
                                                                      {"eUTRAcellIdentifier", nullptr},
                                                                      {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_cgi_eutra_s_opts[] = {{2, (int32_t)offsetof(cgi_eutra_s, ie_exts_present)}};
const asn1::table::seq_desc       cgi_eutra_s::desc            = {
    {seq_table_cgi_eutra_s_fields, 3, UINT64_C(0x3), seq_table_cgi_eutra_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_cgi_eutra_s_names};

// CGI-NR ::= SEQUENCE
static const asn1::table::seq_field seq_table_cgi_nr_s_fields[] = {
    {offsetof(cgi_nr_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cgi_nr_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(cgi_nr_s, nr_cell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cgi_nr_s::nr_cell_id), asn1::table::struct_json>::value},
    {offsetof(cgi_nr_s, ie_exts),
     (int32_t)offsetof(cgi_nr_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cgi_nr_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_cgi_nr_s_names[] = {{"pLMN-Identity", nullptr},
                                                                   {"nRcellIdentifier", nullptr},
                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_cgi_nr_s_opts[]  = {{2, (int32_t)offsetof(cgi_nr_s, ie_exts_present)}};
const asn1::table::seq_desc cgi_nr_s::desc = {{seq_table_cgi_nr_s_fields, 3, UINT64_C(0x3), seq_table_cgi_nr_s_opts, 1},
                                              true,
                                              true,
                                              nullptr,
                                              0,
                                              seq_table_cgi_nr_s_names};

// CPLength-EUTRA ::= ENUMERATED
static const char*           enum_names_cp_len_eutra_e_values[] = {"normal", "extended"};
const asn1::table::enum_desc cp_len_eutra_opts::desc = {enum_names_cp_len_eutra_e_values, 2, "cp_len_eutra_e"};

// CarrierFreq ::= SEQUENCE
static const asn1::table::seq_field seq_table_carrier_freq_s_fields[] = {
    {offsetof(carrier_freq_s, point_a),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(carrier_freq_s, offset_to_carrier),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(2199),
     nullptr},
    {offsetof(carrier_freq_s, ie_exts),
     (int32_t)offsetof(carrier_freq_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(carrier_freq_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_carrier_freq_s_names[] = {{"pointA", nullptr},
                                                                         {"offsetToCarrier", nullptr},
                                                                         {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_carrier_freq_s_opts[]  = {
    {2, (int32_t)offsetof(carrier_freq_s, ie_exts_present)}};
const asn1::table::seq_desc carrier_freq_s::desc = {
    {seq_table_carrier_freq_s_fields, 3, UINT64_C(0x3), seq_table_carrier_freq_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_carrier_freq_s_names};

// CauseRadioNetwork ::= ENUMERATED
static const char*           enum_names_cause_radio_network_e_values[] = {"unspecified",
                                                                          "requested-item-not-supported",
                                                                          "requested-item-temporarily-not-available",
                                                                          "serving-NG-RAN-node-changed",
                                                                          "requested-item-not-supported-on-time"};
const asn1::table::enum_desc cause_radio_network_opts::desc            = {enum_names_cause_radio_network_e_values,
                                                                          5,
                                                                          "cause_radio_network_e"};

// CauseProtocol ::= ENUMERATED
static const char*           enum_names_cause_protocol_e_values[] = {"transfer-syntax-error",
                                                                     "abstract-syntax-error-reject",
                                                                     "abstract-syntax-error-ignore-and-notify",
                                                                     "message-not-compatible-with-receiver-state",
                                                                     "semantic-error",
                                                                     "unspecified",
                                                                     "abstract-syntax-error-falsely-constructed-message"};
const asn1::table::enum_desc cause_protocol_opts::desc = {enum_names_cause_protocol_e_values, 7, "cause_protocol_e"};

// CauseMisc ::= ENUMERATED
static const char*           enum_names_cause_misc_e_values[] = {"unspecified"};
const asn1::table::enum_desc cause_misc_opts::desc            = {enum_names_cause_misc_e_values, 1, "cause_misc_e"};

// Cause ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_cause_c_alternatives[] = {
    &asn1::table::alternative_ops<cause_radio_network_e, true>::value,
    &asn1::table::alternative_ops<cause_protocol_e, true>::value,
    &asn1::table::alternative_ops<cause_misc_e, true>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<cause_ext_ie_o>, false>::value};
const asn1::table::choice_storage_desc cause_c::storage_desc = {choice_storage_cause_c_alternatives, 4, "cause_c"};

cause_radio_network_e& cause_c::set_radio_network()
{
  set(types::radio_network);
  return c.get<cause_radio_network_e>();
}
cause_protocol_e& cause_c::set_protocol()
{
  set(types::protocol);
  return c.get<cause_protocol_e>();
}
cause_misc_e& cause_c::set_misc()
{
  set(types::misc);
  return c.get<cause_misc_e>();
}
protocol_ie_single_container_s<cause_ext_ie_o>& cause_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<cause_ext_ie_o>>();
}
static const asn1::table::seq_field choice_table_cause_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<cause_radio_network_e>(),
     cause_radio_network_e::has_ext,
     false,
     cause_radio_network_e::nof_types,
     cause_radio_network_e::nof_exts,
     nullptr,
     &cause_radio_network_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<cause_protocol_e>(),
     cause_protocol_e::has_ext,
     false,
     cause_protocol_e::nof_types,
     cause_protocol_e::nof_exts,
     nullptr,
     &cause_protocol_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<cause_misc_e>(),
     cause_misc_e::has_ext,
     false,
     cause_misc_e::nof_types,
     cause_misc_e::nof_exts,
     nullptr,
     &cause_misc_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<cause_ext_ie_o>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_cause_c_names[] = {{"radioNetwork", nullptr},
                                                                     {"protocol", nullptr},
                                                                     {"misc", nullptr},
                                                                     {"choice-Extension", nullptr}};
const asn1::table::choice_desc       cause_c::desc                = {asn1::table::enum_int_type<cause_c::types>(),
                                                                     cause_c::types::nof_types,
                                                                     cause_c::types::nof_exts,
                                                                     cause_c::types::has_ext,
                                                                     true,
                                                                     choice_table_cause_c_alternatives,
                                                                     nullptr,
                                                                     choice_table_cause_c_names,
                                                                     "cause_c",
                                                                     offsetof(cause_c, c.type),
                                                                     offsetof(cause_c, c.value),
                                                                     false,
                                                                     &cause_c::storage_desc};

static const char* enum_names_cause_c_types_values[]   = {"radioNetwork", "protocol", "misc", "choice-Extension"};
const asn1::table::enum_desc cause_c::types_opts::desc = {enum_names_cause_c_types_values, 4, "cause_c::types"};
uint8_t                      cause_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "cause_c::types");
}

// TRP-Beam-Power-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_beam_pwr_item_s_fields[] = {
    {offsetof(trp_beam_pwr_item_s, prs_res_set_id),
     (int32_t)offsetof(trp_beam_pwr_item_s, prs_res_set_id_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(trp_beam_pwr_item_s, prs_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(trp_beam_pwr_item_s, relative_pwr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(30),
     nullptr},
    {offsetof(trp_beam_pwr_item_s, relative_pwr_fine),
     (int32_t)offsetof(trp_beam_pwr_item_s, relative_pwr_fine_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(9),
     nullptr},
    {offsetof(trp_beam_pwr_item_s, ie_exts),
     (int32_t)offsetof(trp_beam_pwr_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_beam_pwr_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_beam_pwr_item_s_names[] = {{"pRSResourceSetID", nullptr},
                                                                              {"pRSResourceID", nullptr},
                                                                              {"relativePower", nullptr},
                                                                              {"relativePowerFine", nullptr},
                                                                              {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_trp_beam_pwr_item_s_opts[]  = {
    {0, (int32_t)offsetof(trp_beam_pwr_item_s, prs_res_set_id_present)},
    {3, (int32_t)offsetof(trp_beam_pwr_item_s, relative_pwr_fine_present)},
    {4, (int32_t)offsetof(trp_beam_pwr_item_s, ie_exts_present)}};
const asn1::table::seq_desc trp_beam_pwr_item_s::desc = {
    {seq_table_trp_beam_pwr_item_s_fields, 5, UINT64_C(0x6), seq_table_trp_beam_pwr_item_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_beam_pwr_item_s_names};

// TRP-ElevationAngleList-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_elevation_angle_list_item_s_fields[] = {
    {offsetof(trp_elevation_angle_list_item_s, trp_elevation_angle),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(180),
     nullptr},
    {offsetof(trp_elevation_angle_list_item_s, trp_elevation_angle_fine),
     (int32_t)offsetof(trp_elevation_angle_list_item_s, trp_elevation_angle_fine_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(9),
     nullptr},
    {offsetof(trp_elevation_angle_list_item_s, trp_beam_pwr_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     2,
     24,
     &asn1::table::item_list_codec<decltype(trp_elevation_angle_list_item_s::trp_beam_pwr_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &trp_beam_pwr_item_s::desc},
    {offsetof(trp_elevation_angle_list_item_s, ie_exts),
     (int32_t)offsetof(trp_elevation_angle_list_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_elevation_angle_list_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_elevation_angle_list_item_s_names[] = {
    {"trp-elevation-angle", nullptr},
    {"trp-elevation-angle-fine", nullptr},
    {"trp-beam-power-list", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_trp_elevation_angle_list_item_s_opts[] = {
    {1, (int32_t)offsetof(trp_elevation_angle_list_item_s, trp_elevation_angle_fine_present)},
    {3, (int32_t)offsetof(trp_elevation_angle_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc trp_elevation_angle_list_item_s::desc = {{seq_table_trp_elevation_angle_list_item_s_fields,
                                                                      4,
                                                                      UINT64_C(0x5),
                                                                      seq_table_trp_elevation_angle_list_item_s_opts,
                                                                      2},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_trp_elevation_angle_list_item_s_names};

// TRP-BeamAntennaAnglesList-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_beam_ant_angles_list_item_s_fields[] = {
    {offsetof(trp_beam_ant_angles_list_item_s, trp_azimuth_angle),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(359),
     nullptr},
    {offsetof(trp_beam_ant_angles_list_item_s, trp_azimuth_angle_fine),
     (int32_t)offsetof(trp_beam_ant_angles_list_item_s, trp_azimuth_angle_fine_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(9),
     nullptr},
    {offsetof(trp_beam_ant_angles_list_item_s, trp_elevation_angle_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1801,
     &asn1::table::item_list_codec<decltype(trp_beam_ant_angles_list_item_s::trp_elevation_angle_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &trp_elevation_angle_list_item_s::desc},
    {offsetof(trp_beam_ant_angles_list_item_s, ie_exts),
     (int32_t)offsetof(trp_beam_ant_angles_list_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_beam_ant_angles_list_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_beam_ant_angles_list_item_s_names[] = {
    {"trp-azimuth-angle", nullptr},
    {"trp-azimuth-angle-fine", nullptr},
    {"trp-elevation-angle-list", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_trp_beam_ant_angles_list_item_s_opts[] = {
    {1, (int32_t)offsetof(trp_beam_ant_angles_list_item_s, trp_azimuth_angle_fine_present)},
    {3, (int32_t)offsetof(trp_beam_ant_angles_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc trp_beam_ant_angles_list_item_s::desc = {{seq_table_trp_beam_ant_angles_list_item_s_fields,
                                                                      4,
                                                                      UINT64_C(0x5),
                                                                      seq_table_trp_beam_ant_angles_list_item_s_opts,
                                                                      2},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_trp_beam_ant_angles_list_item_s_names};

// TRP-BeamAntennaExplicitInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_beam_ant_explicit_info_s_fields[] = {
    {offsetof(trp_beam_ant_explicit_info_s, trp_beam_ant_angles),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     3600,
     &asn1::table::item_list_codec<decltype(trp_beam_ant_explicit_info_s::trp_beam_ant_angles),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &trp_beam_ant_angles_list_item_s::desc},
    {offsetof(trp_beam_ant_explicit_info_s, lcs_to_gcs_translation),
     (int32_t)offsetof(trp_beam_ant_explicit_info_s, lcs_to_gcs_translation_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &lcs_to_gcs_translation_s::desc},
    {offsetof(trp_beam_ant_explicit_info_s, ie_exts),
     (int32_t)offsetof(trp_beam_ant_explicit_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_beam_ant_explicit_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_beam_ant_explicit_info_s_names[] = {
    {"trp-BeamAntennaAngles", nullptr},
    {"lcs-to-gcs-translation", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_trp_beam_ant_explicit_info_s_opts[] = {
    {1, (int32_t)offsetof(trp_beam_ant_explicit_info_s, lcs_to_gcs_translation_present)},
    {2, (int32_t)offsetof(trp_beam_ant_explicit_info_s, ie_exts_present)}};
const asn1::table::seq_desc trp_beam_ant_explicit_info_s::desc = {
    {seq_table_trp_beam_ant_explicit_info_s_fields, 3, UINT64_C(0x1), seq_table_trp_beam_ant_explicit_info_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_beam_ant_explicit_info_s_names};

// Choice-TRP-Beam-Antenna-Info-Item ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_choice_trp_beam_ant_info_item_c_alternatives[] = {
    &asn1::table::alternative_ops<uint32_t, true>::value,
    &asn1::table::alternative_ops<trp_beam_ant_explicit_info_s, false>::value,
    nullptr,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<choice_trp_beam_info_item_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc choice_trp_beam_ant_info_item_c::storage_desc = {
    choice_storage_choice_trp_beam_ant_info_item_c_alternatives,
    4,
    "choice_trp_beam_ant_info_item_c"};

uint32_t& choice_trp_beam_ant_info_item_c::set_ref()
{
  set(types::ref);
  return c.get<uint32_t>();
}
trp_beam_ant_explicit_info_s& choice_trp_beam_ant_info_item_c::set_explicit_value()
{
  set(types::explicit_value);
  return c.get<trp_beam_ant_explicit_info_s>();
}
void choice_trp_beam_ant_info_item_c::set_no_change()
{
  set(types::no_change);
}
protocol_ie_single_container_s<choice_trp_beam_info_item_ext_ies_o>& choice_trp_beam_ant_info_item_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<choice_trp_beam_info_item_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_choice_trp_beam_ant_info_item_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(65535),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_beam_ant_explicit_info_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<choice_trp_beam_info_item_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_choice_trp_beam_ant_info_item_c_names[] =
    {{"reference", nullptr}, {"explicit", nullptr}, {"noChange", nullptr}, {"choice-extension", nullptr}};
const asn1::table::choice_desc choice_trp_beam_ant_info_item_c::desc = {
    asn1::table::enum_int_type<choice_trp_beam_ant_info_item_c::types>(),
    choice_trp_beam_ant_info_item_c::types::nof_types,
    choice_trp_beam_ant_info_item_c::types::nof_exts,
    choice_trp_beam_ant_info_item_c::types::has_ext,
    true,
    choice_table_choice_trp_beam_ant_info_item_c_alternatives,
    nullptr,
    choice_table_choice_trp_beam_ant_info_item_c_names,
    "choice_trp_beam_ant_info_item_c",
    offsetof(choice_trp_beam_ant_info_item_c, c.type),
    offsetof(choice_trp_beam_ant_info_item_c, c.value),
    false,
    &choice_trp_beam_ant_info_item_c::storage_desc};

static const char*           enum_names_choice_trp_beam_ant_info_item_c_types_values[] = {"reference",
                                                                                          "explicit",
                                                                                          "noChange",
                                                                                          "choice-extension"};
const asn1::table::enum_desc choice_trp_beam_ant_info_item_c::types_opts::desc         = {
    enum_names_choice_trp_beam_ant_info_item_c_types_values,
    4,
    "choice_trp_beam_ant_info_item_c::types"};

// CommonTAParameters ::= SEQUENCE
static const asn1::table::seq_field seq_table_common_ta_params_s_fields[] = {
    {offsetof(common_ta_params_s, epoch_time),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(common_ta_params_s::epoch_time), asn1::table::string_json>::value},
    {offsetof(common_ta_params_s, ta_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(common_ta_params_s::ta_info), asn1::table::string_json>::value},
    {offsetof(common_ta_params_s, ie_exts),
     (int32_t)offsetof(common_ta_params_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(common_ta_params_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_common_ta_params_s_names[] = {{"epochTime", nullptr},
                                                                             {"taInfo", nullptr},
                                                                             {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_common_ta_params_s_opts[]  = {
    {2, (int32_t)offsetof(common_ta_params_s, ie_exts_present)}};
const asn1::table::seq_desc common_ta_params_s::desc = {
    {seq_table_common_ta_params_s_fields, 3, UINT64_C(0x3), seq_table_common_ta_params_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_common_ta_params_s_names};

// TypeOfError ::= ENUMERATED
static const char*           enum_names_type_of_error_e_values[] = {"not-understood", "missing"};
const asn1::table::enum_desc type_of_error_opts::desc = {enum_names_type_of_error_e_values, 2, "type_of_error_e"};

static const asn1::table::seq_field seq_table_crit_diagnostics_ie_list_item_s__fields[] = {
    {offsetof(crit_diagnostics_ie_list_item_s_, ie_crit),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(crit_diagnostics_ie_list_item_s_::ie_crit)>(),
     decltype(crit_diagnostics_ie_list_item_s_::ie_crit)::has_ext,
     false,
     decltype(crit_diagnostics_ie_list_item_s_::ie_crit)::nof_types,
     decltype(crit_diagnostics_ie_list_item_s_::ie_crit)::nof_exts,
     nullptr,
     &crit_opts::desc},
    {offsetof(crit_diagnostics_ie_list_item_s_, ie_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(crit_diagnostics_ie_list_item_s_, type_of_error),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(crit_diagnostics_ie_list_item_s_::type_of_error)>(),
     decltype(crit_diagnostics_ie_list_item_s_::type_of_error)::has_ext,
     false,
     decltype(crit_diagnostics_ie_list_item_s_::type_of_error)::nof_types,
     decltype(crit_diagnostics_ie_list_item_s_::type_of_error)::nof_exts,
     nullptr,
     &type_of_error_opts::desc},
    {offsetof(crit_diagnostics_ie_list_item_s_, ie_exts),
     (int32_t)offsetof(crit_diagnostics_ie_list_item_s_, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(crit_diagnostics_ie_list_item_s_::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_crit_diagnostics_ie_list_item_s__names[] = {{"iECriticality", nullptr},
                                                                                           {"iE-ID", nullptr},
                                                                                           {"typeOfError", nullptr},
                                                                                           {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_crit_diagnostics_ie_list_item_s__opts[]  = {
    {3, (int32_t)offsetof(crit_diagnostics_ie_list_item_s_, ie_exts_present)}};
const asn1::table::seq_desc crit_diagnostics_ie_list_item_s_::desc = {
    {seq_table_crit_diagnostics_ie_list_item_s__fields,
     4,
     UINT64_C(0x7),
     seq_table_crit_diagnostics_ie_list_item_s__opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_crit_diagnostics_ie_list_item_s__names};

// CriticalityDiagnostics ::= SEQUENCE
static const asn1::table::seq_field seq_table_crit_diagnostics_s_fields[] = {
    {offsetof(crit_diagnostics_s, proc_code),
     (int32_t)offsetof(crit_diagnostics_s, proc_code_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(crit_diagnostics_s, trigger_msg),
     (int32_t)offsetof(crit_diagnostics_s, trigger_msg_present),
     asn1::table::enum_op<decltype(crit_diagnostics_s::trigger_msg)>(),
     decltype(crit_diagnostics_s::trigger_msg)::has_ext,
     false,
     decltype(crit_diagnostics_s::trigger_msg)::nof_types,
     decltype(crit_diagnostics_s::trigger_msg)::nof_exts,
     nullptr,
     &trigger_msg_opts::desc},
    {offsetof(crit_diagnostics_s, proc_crit),
     (int32_t)offsetof(crit_diagnostics_s, proc_crit_present),
     asn1::table::enum_op<decltype(crit_diagnostics_s::proc_crit)>(),
     decltype(crit_diagnostics_s::proc_crit)::has_ext,
     false,
     decltype(crit_diagnostics_s::proc_crit)::nof_types,
     decltype(crit_diagnostics_s::proc_crit)::nof_exts,
     nullptr,
     &crit_opts::desc},
    {offsetof(crit_diagnostics_s, nrppatransaction_id),
     (int32_t)offsetof(crit_diagnostics_s, nrppatransaction_id_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(32767),
     nullptr},
    {offsetof(crit_diagnostics_s, ies_crit_diagnostics),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     256,
     &asn1::table::item_list_codec<decltype(crit_diagnostics_s::ies_crit_diagnostics),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &crit_diagnostics_ie_list_item_s_::desc},
    {offsetof(crit_diagnostics_s, ie_exts),
     (int32_t)offsetof(crit_diagnostics_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(crit_diagnostics_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_crit_diagnostics_s_names[] = {{"procedureCode", nullptr},
                                                                             {"triggeringMessage", nullptr},
                                                                             {"procedureCriticality", nullptr},
                                                                             {"nrppatransactionID", nullptr},
                                                                             {"iEsCriticalityDiagnostics", nullptr},
                                                                             {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_crit_diagnostics_s_opts[]  = {
    {0, (int32_t)offsetof(crit_diagnostics_s, proc_code_present)},
    {1, (int32_t)offsetof(crit_diagnostics_s, trigger_msg_present)},
    {2, (int32_t)offsetof(crit_diagnostics_s, proc_crit_present)},
    {3, (int32_t)offsetof(crit_diagnostics_s, nrppatransaction_id_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(crit_diagnostics_s, ie_exts_present)}};
const asn1::table::seq_desc crit_diagnostics_s::desc = {
    {seq_table_crit_diagnostics_s_fields, 6, UINT64_C(0x0), seq_table_crit_diagnostics_s_opts, 6},
    true,
    true,
    nullptr,
    0,
    seq_table_crit_diagnostics_s_names};

// DL-Bandwidth-EUTRA ::= ENUMERATED
static const char*           enum_names_dl_bw_eutra_e_values[] = {"bw6", "bw15", "bw25", "bw50", "bw75", "bw100"};
const asn1::table::enum_desc dl_bw_eutra_opts::desc            = {enum_names_dl_bw_eutra_e_values, 6, "dl_bw_eutra_e"};
uint8_t                      dl_bw_eutra_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(numbers, 6, value, "dl_bw_eutra_e");
}

// DL-PRS ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_prs_s_fields[] = {
    {offsetof(dl_prs_s, prsid),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(dl_prs_s, dl_prs_res_set_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(dl_prs_s, dl_prs_res_id),
     (int32_t)offsetof(dl_prs_s, dl_prs_res_id_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(dl_prs_s, ie_exts),
     (int32_t)offsetof(dl_prs_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(dl_prs_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_dl_prs_s_names[] = {{"prsid", nullptr},
                                                                   {"dl-PRSResourceSetID", nullptr},
                                                                   {"dl-PRSResourceID", nullptr},
                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_dl_prs_s_opts[] = {{2, (int32_t)offsetof(dl_prs_s, dl_prs_res_id_present)},
                                                               {3, (int32_t)offsetof(dl_prs_s, ie_exts_present)}};
const asn1::table::seq_desc dl_prs_s::desc = {{seq_table_dl_prs_s_fields, 4, UINT64_C(0x3), seq_table_dl_prs_s_opts, 2},
                                              true,
                                              true,
                                              nullptr,
                                              0,
                                              seq_table_dl_prs_s_names};

// DL-PRSBWAggregationRequestInfo-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_prsbw_aggregation_request_info_item_s_fields[] = {
    {offsetof(dl_prsbw_aggregation_request_info_item_s, dl_prs_res_set_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(1),
     UINT64_C(8),
     nullptr},
    {offsetof(dl_prsbw_aggregation_request_info_item_s, ie_exts),
     (int32_t)offsetof(dl_prsbw_aggregation_request_info_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(dl_prsbw_aggregation_request_info_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_dl_prsbw_aggregation_request_info_item_s_names[] = {
    {"dl-prs-ResourceSetIndex", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_dl_prsbw_aggregation_request_info_item_s_opts[] = {
    {1, (int32_t)offsetof(dl_prsbw_aggregation_request_info_item_s, ie_exts_present)}};
const asn1::table::seq_desc dl_prsbw_aggregation_request_info_item_s::desc = {
    {seq_table_dl_prsbw_aggregation_request_info_item_s_fields,
     2,
     UINT64_C(0x1),
     seq_table_dl_prsbw_aggregation_request_info_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_dl_prsbw_aggregation_request_info_item_s_names};

// DL-PRSMutingPattern ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_dl_prs_muting_pattern_c_alternatives[] = {
    &asn1::table::alternative_ops<fixed_bitstring<2, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<4, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<6, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<8, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<16, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<32, false, true>, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<dl_prs_muting_pattern_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc dl_prs_muting_pattern_c::storage_desc = {
    choice_storage_dl_prs_muting_pattern_c_alternatives,
    7,
    "dl_prs_muting_pattern_c"};

fixed_bitstring<2, false, true>& dl_prs_muting_pattern_c::set_two()
{
  set(types::two);
  return c.get<fixed_bitstring<2, false, true>>();
}
fixed_bitstring<4, false, true>& dl_prs_muting_pattern_c::set_four()
{
  set(types::four);
  return c.get<fixed_bitstring<4, false, true>>();
}
fixed_bitstring<6, false, true>& dl_prs_muting_pattern_c::set_six()
{
  set(types::six);
  return c.get<fixed_bitstring<6, false, true>>();
}
fixed_bitstring<8, false, true>& dl_prs_muting_pattern_c::set_eight()
{
  set(types::eight);
  return c.get<fixed_bitstring<8, false, true>>();
}
fixed_bitstring<16, false, true>& dl_prs_muting_pattern_c::set_sixteen()
{
  set(types::sixteen);
  return c.get<fixed_bitstring<16, false, true>>();
}
fixed_bitstring<32, false, true>& dl_prs_muting_pattern_c::set_thirty_two()
{
  set(types::thirty_two);
  return c.get<fixed_bitstring<32, false, true>>();
}
protocol_ie_single_container_s<dl_prs_muting_pattern_ext_ies_o>& dl_prs_muting_pattern_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<dl_prs_muting_pattern_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_dl_prs_muting_pattern_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<2, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<4, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<6, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<8, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<16, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<32, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<dl_prs_muting_pattern_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_dl_prs_muting_pattern_c_names[] = {{"two", nullptr},
                                                                                     {"four", nullptr},
                                                                                     {"six", nullptr},
                                                                                     {"eight", nullptr},
                                                                                     {"sixteen", nullptr},
                                                                                     {"thirty-two", nullptr},
                                                                                     {"choice-extension", nullptr}};
const asn1::table::choice_desc       dl_prs_muting_pattern_c::desc                = {
    asn1::table::enum_int_type<dl_prs_muting_pattern_c::types>(),
    dl_prs_muting_pattern_c::types::nof_types,
    dl_prs_muting_pattern_c::types::nof_exts,
    dl_prs_muting_pattern_c::types::has_ext,
    true,
    choice_table_dl_prs_muting_pattern_c_alternatives,
    nullptr,
    choice_table_dl_prs_muting_pattern_c_names,
    "dl_prs_muting_pattern_c",
    offsetof(dl_prs_muting_pattern_c, c.type),
    offsetof(dl_prs_muting_pattern_c, c.value),
    false,
    &dl_prs_muting_pattern_c::storage_desc};

static const char* enum_names_dl_prs_muting_pattern_c_types_values[] =
    {"two", "four", "six", "eight", "sixteen", "thirty-two", "choice-extension"};
const asn1::table::enum_desc dl_prs_muting_pattern_c::types_opts::desc = {
    enum_names_dl_prs_muting_pattern_c_types_values,
    7,
    "dl_prs_muting_pattern_c::types"};

// DL-PRSResourceARPLocation ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_dl_prs_res_arp_location_c_alternatives[] = {
    &asn1::table::alternative_ops<relative_geodetic_location_s, false>::value,
    &asn1::table::alternative_ops<relative_cartesian_location_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<dl_prs_res_arp_location_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc dl_prs_res_arp_location_c::storage_desc = {
    choice_storage_dl_prs_res_arp_location_c_alternatives,
    3,
    "dl_prs_res_arp_location_c"};

relative_geodetic_location_s& dl_prs_res_arp_location_c::set_relative_geodetic_location()
{
  set(types::relative_geodetic_location);
  return c.get<relative_geodetic_location_s>();
}
relative_cartesian_location_s& dl_prs_res_arp_location_c::set_relative_cartesian_location()
{
  set(types::relative_cartesian_location);
  return c.get<relative_cartesian_location_s>();
}
protocol_ie_single_container_s<dl_prs_res_arp_location_ext_ies_o>& dl_prs_res_arp_location_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<dl_prs_res_arp_location_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_dl_prs_res_arp_location_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &relative_geodetic_location_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &relative_cartesian_location_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<dl_prs_res_arp_location_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_dl_prs_res_arp_location_c_names[] = {
    {"relativeGeodeticLocation", nullptr},
    {"relativeCartesianLocation", nullptr},
    {"choice-Extension", nullptr}};
const asn1::table::choice_desc dl_prs_res_arp_location_c::desc = {
    asn1::table::enum_int_type<dl_prs_res_arp_location_c::types>(),
    dl_prs_res_arp_location_c::types::nof_types,
    dl_prs_res_arp_location_c::types::nof_exts,
    dl_prs_res_arp_location_c::types::has_ext,
    true,
    choice_table_dl_prs_res_arp_location_c_alternatives,
    nullptr,
    choice_table_dl_prs_res_arp_location_c_names,
    "dl_prs_res_arp_location_c",
    offsetof(dl_prs_res_arp_location_c, c.type),
    offsetof(dl_prs_res_arp_location_c, c.value),
    false,
    &dl_prs_res_arp_location_c::storage_desc};

static const char*           enum_names_dl_prs_res_arp_location_c_types_values[] = {"relativeGeodeticLocation",
                                                                                    "relativeCartesianLocation",
                                                                                    "choice-Extension"};
const asn1::table::enum_desc dl_prs_res_arp_location_c::types_opts::desc         = {
    enum_names_dl_prs_res_arp_location_c_types_values,
    3,
    "dl_prs_res_arp_location_c::types"};

// DL-PRSResourceSetARPLocation ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_dl_prs_res_set_arp_location_c_alternatives[] = {
    &asn1::table::alternative_ops<relative_geodetic_location_s, false>::value,
    &asn1::table::alternative_ops<relative_cartesian_location_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<dl_prs_res_set_arp_location_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc dl_prs_res_set_arp_location_c::storage_desc = {
    choice_storage_dl_prs_res_set_arp_location_c_alternatives,
    3,
    "dl_prs_res_set_arp_location_c"};

relative_geodetic_location_s& dl_prs_res_set_arp_location_c::set_relative_geodetic_location()
{
  set(types::relative_geodetic_location);
  return c.get<relative_geodetic_location_s>();
}
relative_cartesian_location_s& dl_prs_res_set_arp_location_c::set_relative_cartesian_location()
{
  set(types::relative_cartesian_location);
  return c.get<relative_cartesian_location_s>();
}
protocol_ie_single_container_s<dl_prs_res_set_arp_location_ext_ies_o>& dl_prs_res_set_arp_location_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<dl_prs_res_set_arp_location_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_dl_prs_res_set_arp_location_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &relative_geodetic_location_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &relative_cartesian_location_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<dl_prs_res_set_arp_location_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_dl_prs_res_set_arp_location_c_names[] = {
    {"relativeGeodeticLocation", nullptr},
    {"relativeCartesianLocation", nullptr},
    {"choice-Extension", nullptr}};
const asn1::table::choice_desc dl_prs_res_set_arp_location_c::desc = {
    asn1::table::enum_int_type<dl_prs_res_set_arp_location_c::types>(),
    dl_prs_res_set_arp_location_c::types::nof_types,
    dl_prs_res_set_arp_location_c::types::nof_exts,
    dl_prs_res_set_arp_location_c::types::has_ext,
    true,
    choice_table_dl_prs_res_set_arp_location_c_alternatives,
    nullptr,
    choice_table_dl_prs_res_set_arp_location_c_names,
    "dl_prs_res_set_arp_location_c",
    offsetof(dl_prs_res_set_arp_location_c, c.type),
    offsetof(dl_prs_res_set_arp_location_c, c.value),
    false,
    &dl_prs_res_set_arp_location_c::storage_desc};

static const char*           enum_names_dl_prs_res_set_arp_location_c_types_values[] = {"relativeGeodeticLocation",
                                                                                        "relativeCartesianLocation",
                                                                                        "choice-Extension"};
const asn1::table::enum_desc dl_prs_res_set_arp_location_c::types_opts::desc         = {
    enum_names_dl_prs_res_set_arp_location_c_types_values,
    3,
    "dl_prs_res_set_arp_location_c::types"};

// DL-reference-signal-UERxTx-TD ::= ENUMERATED
static const char*           enum_names_dl_ref_sig_ue_rx_tx_td_e_values[] = {"csirs", "prs"};
const asn1::table::enum_desc dl_ref_sig_ue_rx_tx_td_opts::desc            = {enum_names_dl_ref_sig_ue_rx_tx_td_e_values,
                                                                             2,
                                                                             "dl_ref_sig_ue_rx_tx_td_e"};

// DLPRSResourceARP ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_prs_res_arp_s_fields[] = {
    {offsetof(dl_prs_res_arp_s, dl_prs_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(dl_prs_res_arp_s, dl_prs_res_arp_location),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &dl_prs_res_arp_location_c::desc},
    {offsetof(dl_prs_res_arp_s, ie_exts),
     (int32_t)offsetof(dl_prs_res_arp_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(dl_prs_res_arp_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_dl_prs_res_arp_s_names[] = {{"dl-PRSResourceID", nullptr},
                                                                           {"dL-PRSResourceARPLocation", nullptr},
                                                                           {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_dl_prs_res_arp_s_opts[]  = {
    {2, (int32_t)offsetof(dl_prs_res_arp_s, ie_exts_present)}};
const asn1::table::seq_desc dl_prs_res_arp_s::desc = {
    {seq_table_dl_prs_res_arp_s_fields, 3, UINT64_C(0x3), seq_table_dl_prs_res_arp_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_dl_prs_res_arp_s_names};

// DLPRSResourceSetARP ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_prs_res_set_arp_s_fields[] = {
    {offsetof(dl_prs_res_set_arp_s, dl_prs_res_set_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(dl_prs_res_set_arp_s, dl_prs_res_set_arp_location),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &dl_prs_res_set_arp_location_c::desc},
    {offsetof(dl_prs_res_set_arp_s, listof_dl_prs_res_arp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(dl_prs_res_set_arp_s::listof_dl_prs_res_arp),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &dl_prs_res_arp_s::desc},
    {offsetof(dl_prs_res_set_arp_s, ie_exts),
     (int32_t)offsetof(dl_prs_res_set_arp_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(dl_prs_res_set_arp_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_dl_prs_res_set_arp_s_names[] = {
    {"dl-PRSResourceSetID", nullptr},
    {"dL-PRSResourceSetARPLocation", nullptr},
    {"listofDL-PRSResourceARP", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_dl_prs_res_set_arp_s_opts[] = {
    {3, (int32_t)offsetof(dl_prs_res_set_arp_s, ie_exts_present)}};
const asn1::table::seq_desc dl_prs_res_set_arp_s::desc = {
    {seq_table_dl_prs_res_set_arp_s_fields, 4, UINT64_C(0x7), seq_table_dl_prs_res_set_arp_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_dl_prs_res_set_arp_s_names};

// DLPRSResourceCoordinates ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_prs_res_coordinates_s_fields[] = {
    {offsetof(dl_prs_res_coordinates_s, listof_dl_prs_res_set_arp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2,
     &asn1::table::item_list_codec<decltype(dl_prs_res_coordinates_s::listof_dl_prs_res_set_arp),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &dl_prs_res_set_arp_s::desc},
    {offsetof(dl_prs_res_coordinates_s, ie_exts),
     (int32_t)offsetof(dl_prs_res_coordinates_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(dl_prs_res_coordinates_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_dl_prs_res_coordinates_s_names[] = {
    {"listofDL-PRSResourceSetARP", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_dl_prs_res_coordinates_s_opts[] = {
    {1, (int32_t)offsetof(dl_prs_res_coordinates_s, ie_exts_present)}};
const asn1::table::seq_desc dl_prs_res_coordinates_s::desc = {
    {seq_table_dl_prs_res_coordinates_s_fields, 2, UINT64_C(0x1), seq_table_dl_prs_res_coordinates_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_dl_prs_res_coordinates_s_names};

// DLPRSResourceID-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_prs_res_id_item_s_fields[] = {
    {offsetof(dl_prs_res_id_item_s, dl_prs_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(dl_prs_res_id_item_s, ie_exts),
     (int32_t)offsetof(dl_prs_res_id_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(dl_prs_res_id_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_dl_prs_res_id_item_s_names[] = {{"dl-PRSResourceID", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_dl_prs_res_id_item_s_opts[]  = {
    {1, (int32_t)offsetof(dl_prs_res_id_item_s, ie_exts_present)}};
const asn1::table::seq_desc dl_prs_res_id_item_s::desc = {
    {seq_table_dl_prs_res_id_item_s_fields, 2, UINT64_C(0x1), seq_table_dl_prs_res_id_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_dl_prs_res_id_item_s_names};

// NG-RANCell ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ng_ran_cell_c_alternatives[] = {
    &asn1::table::alternative_ops<fixed_bitstring<28, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<36, false, true>, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<ng_ran_cell_ext_ie_o>, false>::value};
const asn1::table::choice_storage_desc ng_ran_cell_c::storage_desc = {choice_storage_ng_ran_cell_c_alternatives,
                                                                      3,
                                                                      "ng_ran_cell_c"};

fixed_bitstring<28, false, true>& ng_ran_cell_c::set_eutra_cell_id()
{
  set(types::eutra_cell_id);
  return c.get<fixed_bitstring<28, false, true>>();
}
fixed_bitstring<36, false, true>& ng_ran_cell_c::set_nr_cell_id()
{
  set(types::nr_cell_id);
  return c.get<fixed_bitstring<36, false, true>>();
}
protocol_ie_single_container_s<ng_ran_cell_ext_ie_o>& ng_ran_cell_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<ng_ran_cell_ext_ie_o>>();
}
static const asn1::table::seq_field choice_table_ng_ran_cell_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<28, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<36, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<ng_ran_cell_ext_ie_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_ng_ran_cell_c_names[] = {{"eUTRA-CellID", nullptr},
                                                                           {"nR-CellID", nullptr},
                                                                           {"choice-Extension", nullptr}};
const asn1::table::choice_desc       ng_ran_cell_c::desc = {asn1::table::enum_int_type<ng_ran_cell_c::types>(),
                                                            ng_ran_cell_c::types::nof_types,
                                                            ng_ran_cell_c::types::nof_exts,
                                                            ng_ran_cell_c::types::has_ext,
                                                            true,
                                                            choice_table_ng_ran_cell_c_alternatives,
                                                            nullptr,
                                                            choice_table_ng_ran_cell_c_names,
                                                            "ng_ran_cell_c",
                                                            offsetof(ng_ran_cell_c, c.type),
                                                            offsetof(ng_ran_cell_c, c.value),
                                                            false,
                                                            &ng_ran_cell_c::storage_desc};

static const char* enum_names_ng_ran_cell_c_types_values[]   = {"eUTRA-CellID", "nR-CellID", "choice-Extension"};
const asn1::table::enum_desc ng_ran_cell_c::types_opts::desc = {enum_names_ng_ran_cell_c_types_values,
                                                                3,
                                                                "ng_ran_cell_c::types"};

// NG-RAN-CGI ::= SEQUENCE
static const asn1::table::seq_field seq_table_ng_ran_cgi_s_fields[] = {
    {offsetof(ng_ran_cgi_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ng_ran_cgi_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(ng_ran_cgi_s, ng_ran_cell),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ng_ran_cell_c::desc},
    {offsetof(ng_ran_cgi_s, ie_exts),
     (int32_t)offsetof(ng_ran_cgi_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ng_ran_cgi_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ng_ran_cgi_s_names[] = {{"pLMN-Identity", nullptr},
                                                                       {"nG-RANcell", nullptr},
                                                                       {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ng_ran_cgi_s_opts[]  = {
    {2, (int32_t)offsetof(ng_ran_cgi_s, ie_exts_present)}};
const asn1::table::seq_desc ng_ran_cgi_s::desc = {
    {seq_table_ng_ran_cgi_s_fields, 3, UINT64_C(0x3), seq_table_ng_ran_cgi_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_ng_ran_cgi_s_names};

// NG-RANAccessPointPosition ::= SEQUENCE
static const asn1::table::seq_field seq_table_ng_ran_access_point_position_s_fields[] = {
    {offsetof(ng_ran_access_point_position_s, latitude_sign),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ng_ran_access_point_position_s::latitude_sign)>(),
     decltype(ng_ran_access_point_position_s::latitude_sign)::has_ext,
     false,
     decltype(ng_ran_access_point_position_s::latitude_sign)::nof_types,
     decltype(ng_ran_access_point_position_s::latitude_sign)::nof_exts,
     nullptr,
     &ng_ran_access_point_position_s::latitude_sign_opts::desc},
    {offsetof(ng_ran_access_point_position_s, latitude),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(8388607),
     nullptr},
    {offsetof(ng_ran_access_point_position_s, longitude),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i32,
     false,
     true,
     INT64_C(-8388608),
     UINT64_C(8388607),
     nullptr},
    {offsetof(ng_ran_access_point_position_s, direction_of_altitude),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ng_ran_access_point_position_s::direction_of_altitude)>(),
     decltype(ng_ran_access_point_position_s::direction_of_altitude)::has_ext,
     false,
     decltype(ng_ran_access_point_position_s::direction_of_altitude)::nof_types,
     decltype(ng_ran_access_point_position_s::direction_of_altitude)::nof_exts,
     nullptr,
     &ng_ran_access_point_position_s::direction_of_altitude_opts::desc},
    {offsetof(ng_ran_access_point_position_s, altitude),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(32767),
     nullptr},
    {offsetof(ng_ran_access_point_position_s, uncertainty_semi_major),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(ng_ran_access_point_position_s, uncertainty_semi_minor),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(ng_ran_access_point_position_s, orientation_of_major_axis),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(179),
     nullptr},
    {offsetof(ng_ran_access_point_position_s, uncertainty_altitude),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(ng_ran_access_point_position_s, confidence),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(100),
     nullptr},
    {offsetof(ng_ran_access_point_position_s, ie_exts),
     (int32_t)offsetof(ng_ran_access_point_position_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ng_ran_access_point_position_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ng_ran_access_point_position_s_names[] = {
    {"latitudeSign", nullptr},
    {"latitude", nullptr},
    {"longitude", nullptr},
    {"directionOfAltitude", nullptr},
    {"altitude", nullptr},
    {"uncertaintySemi-major", nullptr},
    {"uncertaintySemi-minor", nullptr},
    {"orientationOfMajorAxis", nullptr},
    {"uncertaintyAltitude", nullptr},
    {"confidence", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_ng_ran_access_point_position_s_opts[] = {
    {10, (int32_t)offsetof(ng_ran_access_point_position_s, ie_exts_present)}};
const asn1::table::seq_desc ng_ran_access_point_position_s::desc = {{seq_table_ng_ran_access_point_position_s_fields,
                                                                     11,
                                                                     UINT64_C(0x3ff),
                                                                     seq_table_ng_ran_access_point_position_s_opts,
                                                                     1},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_ng_ran_access_point_position_s_names};

static const char*           enum_names_ng_ran_access_point_position_s_latitude_sign_e__values[] = {"north", "south"};
const asn1::table::enum_desc ng_ran_access_point_position_s::latitude_sign_opts::desc            = {
    enum_names_ng_ran_access_point_position_s_latitude_sign_e__values,
    2,
    "ng_ran_access_point_position_s::latitude_sign_e_"};

static const char* enum_names_ng_ran_access_point_position_s_direction_of_altitude_e__values[] = {"height", "depth"};
const asn1::table::enum_desc ng_ran_access_point_position_s::direction_of_altitude_opts::desc  = {
    enum_names_ng_ran_access_point_position_s_direction_of_altitude_e__values,
    2,
    "ng_ran_access_point_position_s::direction_of_altitude_e_"};
uint8_t ng_ran_access_point_position_s::direction_of_altitude_opts::to_number() const
{
  static const uint8_t numbers[] = {8};
  return map_enum_number(numbers, 1, value, "ng_ran_access_point_position_s::direction_of_altitude_e_");
}

// ResultRSRP-EUTRA-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_result_rsrp_eutra_item_s_fields[] = {
    {offsetof(result_rsrp_eutra_item_s, pci_eutra),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(503),
     nullptr},
    {offsetof(result_rsrp_eutra_item_s, earfcn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(262143),
     nullptr},
    {offsetof(result_rsrp_eutra_item_s, cgi_eutra),
     (int32_t)offsetof(result_rsrp_eutra_item_s, cgi_eutra_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_eutra_s::desc},
    {offsetof(result_rsrp_eutra_item_s, value_rsrp_eutra),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(97),
     nullptr},
    {offsetof(result_rsrp_eutra_item_s, ie_exts),
     (int32_t)offsetof(result_rsrp_eutra_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(result_rsrp_eutra_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_result_rsrp_eutra_item_s_names[] = {{"pCI-EUTRA", nullptr},
                                                                                   {"eARFCN", nullptr},
                                                                                   {"cGI-EUTRA", nullptr},
                                                                                   {"valueRSRP-EUTRA", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_result_rsrp_eutra_item_s_opts[]  = {
    {2, (int32_t)offsetof(result_rsrp_eutra_item_s, cgi_eutra_present)},
    {4, (int32_t)offsetof(result_rsrp_eutra_item_s, ie_exts_present)}};
const asn1::table::seq_desc result_rsrp_eutra_item_s::desc = {
    {seq_table_result_rsrp_eutra_item_s_fields, 5, UINT64_C(0xb), seq_table_result_rsrp_eutra_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_result_rsrp_eutra_item_s_names};

// ResultRSRQ-EUTRA-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_result_rsrq_eutra_item_s_fields[] = {
    {offsetof(result_rsrq_eutra_item_s, pci_eutra),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(503),
     nullptr},
    {offsetof(result_rsrq_eutra_item_s, earfcn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(262143),
     nullptr},
    {offsetof(result_rsrq_eutra_item_s, cgi_utra),
     (int32_t)offsetof(result_rsrq_eutra_item_s, cgi_utra_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_eutra_s::desc},
    {offsetof(result_rsrq_eutra_item_s, value_rsrq_eutra),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(0),
     UINT64_C(34),
     nullptr},
    {offsetof(result_rsrq_eutra_item_s, ie_exts),
     (int32_t)offsetof(result_rsrq_eutra_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(result_rsrq_eutra_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_result_rsrq_eutra_item_s_names[] = {{"pCI-EUTRA", nullptr},
                                                                                   {"eARFCN", nullptr},
                                                                                   {"cGI-UTRA", nullptr},
                                                                                   {"valueRSRQ-EUTRA", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_result_rsrq_eutra_item_s_opts[]  = {
    {2, (int32_t)offsetof(result_rsrq_eutra_item_s, cgi_utra_present)},
    {4, (int32_t)offsetof(result_rsrq_eutra_item_s, ie_exts_present)}};
const asn1::table::seq_desc result_rsrq_eutra_item_s::desc = {
    {seq_table_result_rsrq_eutra_item_s_fields, 5, UINT64_C(0xb), seq_table_result_rsrq_eutra_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_result_rsrq_eutra_item_s_names};

// ResultSS-RSRP-PerSSB-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_result_ss_rsrp_per_ssb_item_s_fields[] = {
    {offsetof(result_ss_rsrp_per_ssb_item_s, ssb_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(result_ss_rsrp_per_ssb_item_s, value_ss_rsrp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(result_ss_rsrp_per_ssb_item_s, ie_exts),
     (int32_t)offsetof(result_ss_rsrp_per_ssb_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(result_ss_rsrp_per_ssb_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_result_ss_rsrp_per_ssb_item_s_names[] = {{"sSB-Index", nullptr},
                                                                                        {"valueSS-RSRP", nullptr},
                                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_result_ss_rsrp_per_ssb_item_s_opts[]  = {
    {2, (int32_t)offsetof(result_ss_rsrp_per_ssb_item_s, ie_exts_present)}};
const asn1::table::seq_desc result_ss_rsrp_per_ssb_item_s::desc = {
    {seq_table_result_ss_rsrp_per_ssb_item_s_fields, 3, UINT64_C(0x3), seq_table_result_ss_rsrp_per_ssb_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_result_ss_rsrp_per_ssb_item_s_names};

// ResultSS-RSRP-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_result_ss_rsrp_item_s_fields[] = {
    {offsetof(result_ss_rsrp_item_s, nr_pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(result_ss_rsrp_item_s, nr_arfcn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(result_ss_rsrp_item_s, cgi_nr),
     (int32_t)offsetof(result_ss_rsrp_item_s, cgi_nr_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_nr_s::desc},
    {offsetof(result_ss_rsrp_item_s, value_ss_rsrp_cell),
     (int32_t)offsetof(result_ss_rsrp_item_s, value_ss_rsrp_cell_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(result_ss_rsrp_item_s, ss_rsrp_per_ssb),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(result_ss_rsrp_item_s::ss_rsrp_per_ssb),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &result_ss_rsrp_per_ssb_item_s::desc},
    {offsetof(result_ss_rsrp_item_s, ie_exts),
     (int32_t)offsetof(result_ss_rsrp_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(result_ss_rsrp_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_result_ss_rsrp_item_s_names[] = {{"nR-PCI", nullptr},
                                                                                {"nR-ARFCN", nullptr},
                                                                                {"cGI-NR", nullptr},
                                                                                {"valueSS-RSRP-Cell", nullptr},
                                                                                {"sS-RSRP-PerSSB", nullptr},
                                                                                {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_result_ss_rsrp_item_s_opts[]  = {
    {2, (int32_t)offsetof(result_ss_rsrp_item_s, cgi_nr_present)},
    {3, (int32_t)offsetof(result_ss_rsrp_item_s, value_ss_rsrp_cell_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(result_ss_rsrp_item_s, ie_exts_present)}};
const asn1::table::seq_desc result_ss_rsrp_item_s::desc = {
    {seq_table_result_ss_rsrp_item_s_fields, 6, UINT64_C(0x3), seq_table_result_ss_rsrp_item_s_opts, 4},
    true,
    true,
    nullptr,
    0,
    seq_table_result_ss_rsrp_item_s_names};

// ResultSS-RSRQ-PerSSB-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_result_ss_rsrq_per_ssb_item_s_fields[] = {
    {offsetof(result_ss_rsrq_per_ssb_item_s, ssb_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(result_ss_rsrq_per_ssb_item_s, value_ss_rsrq),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(result_ss_rsrq_per_ssb_item_s, ie_exts),
     (int32_t)offsetof(result_ss_rsrq_per_ssb_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(result_ss_rsrq_per_ssb_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_result_ss_rsrq_per_ssb_item_s_names[] = {{"sSB-Index", nullptr},
                                                                                        {"valueSS-RSRQ", nullptr},
                                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_result_ss_rsrq_per_ssb_item_s_opts[]  = {
    {2, (int32_t)offsetof(result_ss_rsrq_per_ssb_item_s, ie_exts_present)}};
const asn1::table::seq_desc result_ss_rsrq_per_ssb_item_s::desc = {
    {seq_table_result_ss_rsrq_per_ssb_item_s_fields, 3, UINT64_C(0x3), seq_table_result_ss_rsrq_per_ssb_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_result_ss_rsrq_per_ssb_item_s_names};

// ResultSS-RSRQ-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_result_ss_rsrq_item_s_fields[] = {
    {offsetof(result_ss_rsrq_item_s, nr_pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(result_ss_rsrq_item_s, nr_arfcn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(result_ss_rsrq_item_s, cgi_nr),
     (int32_t)offsetof(result_ss_rsrq_item_s, cgi_nr_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_nr_s::desc},
    {offsetof(result_ss_rsrq_item_s, value_ss_rsrq_cell),
     (int32_t)offsetof(result_ss_rsrq_item_s, value_ss_rsrq_cell_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(result_ss_rsrq_item_s, ss_rsrq_per_ssb),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(result_ss_rsrq_item_s::ss_rsrq_per_ssb),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &result_ss_rsrq_per_ssb_item_s::desc},
    {offsetof(result_ss_rsrq_item_s, ie_exts),
     (int32_t)offsetof(result_ss_rsrq_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(result_ss_rsrq_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_result_ss_rsrq_item_s_names[] = {{"nR-PCI", nullptr},
                                                                                {"nR-ARFCN", nullptr},
                                                                                {"cGI-NR", nullptr},
                                                                                {"valueSS-RSRQ-Cell", nullptr},
                                                                                {"sS-RSRQ-PerSSB", nullptr},
                                                                                {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_result_ss_rsrq_item_s_opts[]  = {
    {2, (int32_t)offsetof(result_ss_rsrq_item_s, cgi_nr_present)},
    {3, (int32_t)offsetof(result_ss_rsrq_item_s, value_ss_rsrq_cell_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(result_ss_rsrq_item_s, ie_exts_present)}};
const asn1::table::seq_desc result_ss_rsrq_item_s::desc = {
    {seq_table_result_ss_rsrq_item_s_fields, 6, UINT64_C(0x3), seq_table_result_ss_rsrq_item_s_opts, 4},
    true,
    true,
    nullptr,
    0,
    seq_table_result_ss_rsrq_item_s_names};

// ResultCSI-RSRP-PerCSI-RS-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_result_csi_rsrp_per_csi_rs_item_s_fields[] = {
    {offsetof(result_csi_rsrp_per_csi_rs_item_s, csi_rs_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(95),
     nullptr},
    {offsetof(result_csi_rsrp_per_csi_rs_item_s, value_csi_rsrp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(result_csi_rsrp_per_csi_rs_item_s, ie_exts),
     (int32_t)offsetof(result_csi_rsrp_per_csi_rs_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(result_csi_rsrp_per_csi_rs_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_result_csi_rsrp_per_csi_rs_item_s_names[] = {{"cSI-RS-Index", nullptr},
                                                                                            {"valueCSI-RSRP", nullptr},
                                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_result_csi_rsrp_per_csi_rs_item_s_opts[]  = {
    {2, (int32_t)offsetof(result_csi_rsrp_per_csi_rs_item_s, ie_exts_present)}};
const asn1::table::seq_desc result_csi_rsrp_per_csi_rs_item_s::desc = {
    {seq_table_result_csi_rsrp_per_csi_rs_item_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_result_csi_rsrp_per_csi_rs_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_result_csi_rsrp_per_csi_rs_item_s_names};

// ResultCSI-RSRP-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_result_csi_rsrp_item_s_fields[] = {
    {offsetof(result_csi_rsrp_item_s, nr_pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(result_csi_rsrp_item_s, nr_arfcn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(result_csi_rsrp_item_s, cgi_nr),
     (int32_t)offsetof(result_csi_rsrp_item_s, cgi_nr_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_nr_s::desc},
    {offsetof(result_csi_rsrp_item_s, value_csi_rsrp_cell),
     (int32_t)offsetof(result_csi_rsrp_item_s, value_csi_rsrp_cell_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(result_csi_rsrp_item_s, csi_rsrp_per_csi_rs),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(result_csi_rsrp_item_s::csi_rsrp_per_csi_rs),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &result_csi_rsrp_per_csi_rs_item_s::desc},
    {offsetof(result_csi_rsrp_item_s, ie_exts),
     (int32_t)offsetof(result_csi_rsrp_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(result_csi_rsrp_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_result_csi_rsrp_item_s_names[] = {{"nR-PCI", nullptr},
                                                                                 {"nR-ARFCN", nullptr},
                                                                                 {"cGI-NR", nullptr},
                                                                                 {"valueCSI-RSRP-Cell", nullptr},
                                                                                 {"cSI-RSRP-PerCSI-RS", nullptr},
                                                                                 {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_result_csi_rsrp_item_s_opts[]  = {
    {2, (int32_t)offsetof(result_csi_rsrp_item_s, cgi_nr_present)},
    {3, (int32_t)offsetof(result_csi_rsrp_item_s, value_csi_rsrp_cell_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(result_csi_rsrp_item_s, ie_exts_present)}};
const asn1::table::seq_desc result_csi_rsrp_item_s::desc = {
    {seq_table_result_csi_rsrp_item_s_fields, 6, UINT64_C(0x3), seq_table_result_csi_rsrp_item_s_opts, 4},
    true,
    true,
    nullptr,
    0,
    seq_table_result_csi_rsrp_item_s_names};

// ResultCSI-RSRQ-PerCSI-RS-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_result_csi_rsrq_per_csi_rs_item_s_fields[] = {
    {offsetof(result_csi_rsrq_per_csi_rs_item_s, csi_rs_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(95),
     nullptr},
    {offsetof(result_csi_rsrq_per_csi_rs_item_s, value_csi_rsrq),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(result_csi_rsrq_per_csi_rs_item_s, ie_exts),
     (int32_t)offsetof(result_csi_rsrq_per_csi_rs_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(result_csi_rsrq_per_csi_rs_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_result_csi_rsrq_per_csi_rs_item_s_names[] = {{"cSI-RS-Index", nullptr},
                                                                                            {"valueCSI-RSRQ", nullptr},
                                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_result_csi_rsrq_per_csi_rs_item_s_opts[]  = {
    {2, (int32_t)offsetof(result_csi_rsrq_per_csi_rs_item_s, ie_exts_present)}};
const asn1::table::seq_desc result_csi_rsrq_per_csi_rs_item_s::desc = {
    {seq_table_result_csi_rsrq_per_csi_rs_item_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_result_csi_rsrq_per_csi_rs_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_result_csi_rsrq_per_csi_rs_item_s_names};

// ResultCSI-RSRQ-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_result_csi_rsrq_item_s_fields[] = {
    {offsetof(result_csi_rsrq_item_s, nr_pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(result_csi_rsrq_item_s, nr_arfcn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(result_csi_rsrq_item_s, cgi_nr),
     (int32_t)offsetof(result_csi_rsrq_item_s, cgi_nr_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_nr_s::desc},
    {offsetof(result_csi_rsrq_item_s, value_csi_rsrq_cell),
     (int32_t)offsetof(result_csi_rsrq_item_s, value_csi_rsrq_cell_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(result_csi_rsrq_item_s, csi_rsrq_per_csi_rs),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(result_csi_rsrq_item_s::csi_rsrq_per_csi_rs),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &result_csi_rsrq_per_csi_rs_item_s::desc},
    {offsetof(result_csi_rsrq_item_s, ie_exts),
     (int32_t)offsetof(result_csi_rsrq_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(result_csi_rsrq_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_result_csi_rsrq_item_s_names[] = {{"nR-PCI", nullptr},
                                                                                 {"nR-ARFCN", nullptr},
                                                                                 {"cGI-NR", nullptr},
                                                                                 {"valueCSI-RSRQ-Cell", nullptr},
                                                                                 {"cSI-RSRQ-PerCSI-RS", nullptr},
                                                                                 {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_result_csi_rsrq_item_s_opts[]  = {
    {2, (int32_t)offsetof(result_csi_rsrq_item_s, cgi_nr_present)},
    {3, (int32_t)offsetof(result_csi_rsrq_item_s, value_csi_rsrq_cell_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(result_csi_rsrq_item_s, ie_exts_present)}};
const asn1::table::seq_desc result_csi_rsrq_item_s::desc = {
    {seq_table_result_csi_rsrq_item_s_fields, 6, UINT64_C(0x3), seq_table_result_csi_rsrq_item_s_opts, 4},
    true,
    true,
    nullptr,
    0,
    seq_table_result_csi_rsrq_item_s_names};

// MeasuredResultsValue-ExtensionIE ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_measured_results_value_ext_ie_o[] = {32, 33, 34, 35, 36, 94, 118};
static const uint8_t  objset_crit_measured_results_value_ext_ie_o[] =
    {crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};
static const uint8_t objset_value_measured_results_value_ext_ie_o[] = {
    measured_results_value_ext_ie_o::value_c::types::result_ss_rsrp,
    measured_results_value_ext_ie_o::value_c::types::result_ss_rsrq,
    measured_results_value_ext_ie_o::value_c::types::result_csi_rsrp,
    measured_results_value_ext_ie_o::value_c::types::result_csi_rsrq,
    measured_results_value_ext_ie_o::value_c::types::angle_of_arrival_nr,
    measured_results_value_ext_ie_o::value_c::types::nr_tadv,
    measured_results_value_ext_ie_o::value_c::types::ue_rx_tx_time_diff};
static const uint8_t                    objset_presence_measured_results_value_ext_ie_o[] = {presence_e::mandatory,
                                                                                             presence_e::mandatory,
                                                                                             presence_e::mandatory,
                                                                                             presence_e::mandatory,
                                                                                             presence_e::mandatory,
                                                                                             presence_e::mandatory,
                                                                                             presence_e::mandatory};
static const asn1::table::objset_column objset_columns_measured_results_value_ext_ie_o[]  = {
    {objset_crit_measured_results_value_ext_ie_o, crit_e::nulltype},
    {objset_value_measured_results_value_ext_ie_o, measured_results_value_ext_ie_o::value_c::types::nulltype},
    {objset_presence_measured_results_value_ext_ie_o, presence_e::nulltype}};
const asn1::table::objset_desc measured_results_value_ext_ie_o::desc = {objset_id_measured_results_value_ext_ie_o,
                                                                        7,
                                                                        "id",
                                                                        objset_columns_measured_results_value_ext_ie_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_measured_results_value_ext_ie_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     9,
     &asn1::table::any_list_codec<result_ss_rsrp_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     9,
     &asn1::table::any_list_codec<result_ss_rsrq_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     9,
     &asn1::table::any_list_codec<result_csi_rsrp_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     9,
     &asn1::table::any_list_codec<result_csi_rsrq_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ul_ao_a_s>::value,
     &ul_ao_a_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(7690),
     &asn1::table::any_value_codec<uint16_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(61565),
     &asn1::table::any_value_codec<uint16_t>::value}};
static const asn1::table::field_name choice_table_measured_results_value_ext_ie_o_value_c_names[] = {
    {"ResultSS-RSRP", nullptr},
    {"ResultSS-RSRQ", nullptr},
    {"ResultCSI-RSRP", nullptr},
    {"ResultCSI-RSRQ", nullptr},
    {"UL-AoA", nullptr},
    {"INTEGER (0..7690)", nullptr},
    {"INTEGER (0..61565)", nullptr}};
const asn1::table::choice_desc measured_results_value_ext_ie_o::value_c::desc = {
    asn1::table::enum_int_type<measured_results_value_ext_ie_o::value_c::types>(),
    measured_results_value_ext_ie_o::value_c::types::nof_types,
    measured_results_value_ext_ie_o::value_c::types::nof_exts,
    measured_results_value_ext_ie_o::value_c::types::has_ext,
    true,
    choice_table_measured_results_value_ext_ie_o_value_c_alternatives,
    &asn1::table::choice_set<measured_results_value_ext_ie_o::value_c>,
    choice_table_measured_results_value_ext_ie_o_value_c_names,
    "measured_results_value_ext_ie_o::value_c",
    offsetof(measured_results_value_ext_ie_o::value_c, type_),
    offsetof(measured_results_value_ext_ie_o::value_c, c),
    true,
    nullptr,
    true};

void measured_results_value_ext_ie_o::value_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, measured_results_value_ext_ie_o::value_c::desc);
}
result_ss_rsrp_l& measured_results_value_ext_ie_o::value_c::result_ss_rsrp()
{
  assert_choice_type(types::result_ss_rsrp, type_, "Value");
  return c.get<result_ss_rsrp_l>();
}
result_ss_rsrq_l& measured_results_value_ext_ie_o::value_c::result_ss_rsrq()
{
  assert_choice_type(types::result_ss_rsrq, type_, "Value");
  return c.get<result_ss_rsrq_l>();
}
result_csi_rsrp_l& measured_results_value_ext_ie_o::value_c::result_csi_rsrp()
{
  assert_choice_type(types::result_csi_rsrp, type_, "Value");
  return c.get<result_csi_rsrp_l>();
}
result_csi_rsrq_l& measured_results_value_ext_ie_o::value_c::result_csi_rsrq()
{
  assert_choice_type(types::result_csi_rsrq, type_, "Value");
  return c.get<result_csi_rsrq_l>();
}
ul_ao_a_s& measured_results_value_ext_ie_o::value_c::angle_of_arrival_nr()
{
  assert_choice_type(types::angle_of_arrival_nr, type_, "Value");
  return c.get<ul_ao_a_s>();
}
uint16_t& measured_results_value_ext_ie_o::value_c::nr_tadv()
{
  assert_choice_type(types::nr_tadv, type_, "Value");
  return c.get<uint16_t>();
}
uint16_t& measured_results_value_ext_ie_o::value_c::ue_rx_tx_time_diff()
{
  assert_choice_type(types::ue_rx_tx_time_diff, type_, "Value");
  return c.get<uint16_t>();
}
const result_ss_rsrp_l& measured_results_value_ext_ie_o::value_c::result_ss_rsrp() const
{
  assert_choice_type(types::result_ss_rsrp, type_, "Value");
  return c.get<result_ss_rsrp_l>();
}
const result_ss_rsrq_l& measured_results_value_ext_ie_o::value_c::result_ss_rsrq() const
{
  assert_choice_type(types::result_ss_rsrq, type_, "Value");
  return c.get<result_ss_rsrq_l>();
}
const result_csi_rsrp_l& measured_results_value_ext_ie_o::value_c::result_csi_rsrp() const
{
  assert_choice_type(types::result_csi_rsrp, type_, "Value");
  return c.get<result_csi_rsrp_l>();
}
const result_csi_rsrq_l& measured_results_value_ext_ie_o::value_c::result_csi_rsrq() const
{
  assert_choice_type(types::result_csi_rsrq, type_, "Value");
  return c.get<result_csi_rsrq_l>();
}
const ul_ao_a_s& measured_results_value_ext_ie_o::value_c::angle_of_arrival_nr() const
{
  assert_choice_type(types::angle_of_arrival_nr, type_, "Value");
  return c.get<ul_ao_a_s>();
}
const uint16_t& measured_results_value_ext_ie_o::value_c::nr_tadv() const
{
  assert_choice_type(types::nr_tadv, type_, "Value");
  return c.get<uint16_t>();
}
const uint16_t& measured_results_value_ext_ie_o::value_c::ue_rx_tx_time_diff() const
{
  assert_choice_type(types::ue_rx_tx_time_diff, type_, "Value");
  return c.get<uint16_t>();
}

static const char*           enum_names_measured_results_value_ext_ie_o_value_c_types_values[] = {"ResultSS-RSRP",
                                                                                                  "ResultSS-RSRQ",
                                                                                                  "ResultCSI-RSRP",
                                                                                                  "ResultCSI-RSRQ",
                                                                                                  "UL-AoA",
                                                                                                  "INTEGER (0..7690)",
                                                                                                  "INTEGER (0..61565)"};
const asn1::table::enum_desc measured_results_value_ext_ie_o::value_c::types_opts::desc        = {
    enum_names_measured_results_value_ext_ie_o_value_c_types_values,
    7,
    "measured_results_value_ext_ie_o::value_c::types"};

// MeasuredResultsValue ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_measured_results_value_c_alternatives[] = {
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<result_rsrp_eutra_l, false>::value,
    &asn1::table::alternative_ops<result_rsrq_eutra_l, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<measured_results_value_ext_ie_o>, false>::value};
const asn1::table::choice_storage_desc measured_results_value_c::storage_desc = {
    choice_storage_measured_results_value_c_alternatives,
    6,
    "measured_results_value_c"};

uint16_t& measured_results_value_c::set_value_angle_of_arrival_eutra()
{
  set(types::value_angle_of_arrival_eutra);
  return c.get<uint16_t>();
}
uint16_t& measured_results_value_c::set_value_timing_advance_type1_eutra()
{
  set(types::value_timing_advance_type1_eutra);
  return c.get<uint16_t>();
}
uint16_t& measured_results_value_c::set_value_timing_advance_type2_eutra()
{
  set(types::value_timing_advance_type2_eutra);
  return c.get<uint16_t>();
}
result_rsrp_eutra_l& measured_results_value_c::set_result_rsrp_eutra()
{
  set(types::result_rsrp_eutra);
  return c.get<result_rsrp_eutra_l>();
}
result_rsrq_eutra_l& measured_results_value_c::set_result_rsrq_eutra()
{
  set(types::result_rsrq_eutra);
  return c.get<result_rsrq_eutra_l>();
}
protocol_ie_single_container_s<measured_results_value_ext_ie_o>& measured_results_value_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<measured_results_value_ext_ie_o>>();
}
static const asn1::table::seq_field choice_table_measured_results_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(719),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(7690),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(7690),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     9,
     &asn1::table::list_codec<result_rsrp_eutra_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     9,
     &asn1::table::list_codec<result_rsrq_eutra_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<measured_results_value_ext_ie_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_measured_results_value_c_names[] = {
    {"valueAngleOfArrival-EUTRA", nullptr},
    {"valueTimingAdvanceType1-EUTRA", nullptr},
    {"valueTimingAdvanceType2-EUTRA", nullptr},
    {"resultRSRP-EUTRA", nullptr},
    {"resultRSRQ-EUTRA", nullptr},
    {"choice-Extension", nullptr}};
const asn1::table::choice_desc measured_results_value_c::desc = {
    asn1::table::enum_int_type<measured_results_value_c::types>(),
    measured_results_value_c::types::nof_types,
    measured_results_value_c::types::nof_exts,
    measured_results_value_c::types::has_ext,
    true,
    choice_table_measured_results_value_c_alternatives,
    nullptr,
    choice_table_measured_results_value_c_names,
    "measured_results_value_c",
    offsetof(measured_results_value_c, c.type),
    offsetof(measured_results_value_c, c.value),
    false,
    &measured_results_value_c::storage_desc};

static const char*           enum_names_measured_results_value_c_types_values[] = {"valueAngleOfArrival-EUTRA",
                                                                                   "valueTimingAdvanceType1-EUTRA",
                                                                                   "valueTimingAdvanceType2-EUTRA",
                                                                                   "resultRSRP-EUTRA",
                                                                                   "resultRSRQ-EUTRA",
                                                                                   "choice-Extension"};
const asn1::table::enum_desc measured_results_value_c::types_opts::desc         = {
    enum_names_measured_results_value_c_types_values,
    6,
    "measured_results_value_c::types"};
uint8_t measured_results_value_c::types_opts::to_number() const
{
  switch (value) {
    case value_timing_advance_type1_eutra:
      return 1;
    case value_timing_advance_type2_eutra:
      return 2;
    default:
      invalid_enum_number(value, "measured_results_value_c::types");
  }
  return 0;
}

// NGRANHighAccuracyAccessPointPosition ::= SEQUENCE
static const asn1::table::seq_field seq_table_ngran_high_accuracy_access_point_position_s_fields[] = {
    {offsetof(ngran_high_accuracy_access_point_position_s, latitude),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     INT64_C(-2147483648),
     UINT64_C(2147483647),
     nullptr},
    {offsetof(ngran_high_accuracy_access_point_position_s, longitude),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     INT64_C(-2147483648),
     UINT64_C(2147483647),
     nullptr},
    {offsetof(ngran_high_accuracy_access_point_position_s, altitude),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i32,
     false,
     true,
     INT64_C(-64000),
     UINT64_C(1280000),
     nullptr},
    {offsetof(ngran_high_accuracy_access_point_position_s, uncertainty_semi_major),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(ngran_high_accuracy_access_point_position_s, uncertainty_semi_minor),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(ngran_high_accuracy_access_point_position_s, orientation_of_major_axis),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(179),
     nullptr},
    {offsetof(ngran_high_accuracy_access_point_position_s, horizontal_confidence),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(100),
     nullptr},
    {offsetof(ngran_high_accuracy_access_point_position_s, uncertainty_altitude),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(ngran_high_accuracy_access_point_position_s, vertical_confidence),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(100),
     nullptr},
    {offsetof(ngran_high_accuracy_access_point_position_s, ie_exts),
     (int32_t)offsetof(ngran_high_accuracy_access_point_position_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ngran_high_accuracy_access_point_position_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ngran_high_accuracy_access_point_position_s_names[] = {
    {"latitude", nullptr},
    {"longitude", nullptr},
    {"altitude", nullptr},
    {"uncertaintySemi-major", nullptr},
    {"uncertaintySemi-minor", nullptr},
    {"orientationOfMajorAxis", nullptr},
    {"horizontalConfidence", nullptr},
    {"uncertaintyAltitude", nullptr},
    {"verticalConfidence", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_ngran_high_accuracy_access_point_position_s_opts[] = {
    {9, (int32_t)offsetof(ngran_high_accuracy_access_point_position_s, ie_exts_present)}};
const asn1::table::seq_desc ngran_high_accuracy_access_point_position_s::desc = {
    {seq_table_ngran_high_accuracy_access_point_position_s_fields,
     10,
     UINT64_C(0x1ff),
     seq_table_ngran_high_accuracy_access_point_position_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_ngran_high_accuracy_access_point_position_s_names};

// TRPPositionDirectAccuracy ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_trp_position_direct_accuracy_c_alternatives[] = {
    &asn1::table::alternative_ops<ng_ran_access_point_position_s, false>::value,
    &asn1::table::alternative_ops<ngran_high_accuracy_access_point_position_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<trp_position_direct_accuracy_ext_ies_o>,
                                  false>::value};
const asn1::table::choice_storage_desc trp_position_direct_accuracy_c::storage_desc = {
    choice_storage_trp_position_direct_accuracy_c_alternatives,
    3,
    "trp_position_direct_accuracy_c"};

ng_ran_access_point_position_s& trp_position_direct_accuracy_c::set_trp_position()
{
  set(types::trp_position);
  return c.get<ng_ran_access_point_position_s>();
}
ngran_high_accuracy_access_point_position_s& trp_position_direct_accuracy_c::set_trph_aposition()
{
  set(types::trph_aposition);
  return c.get<ngran_high_accuracy_access_point_position_s>();
}
protocol_ie_single_container_s<trp_position_direct_accuracy_ext_ies_o>& trp_position_direct_accuracy_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<trp_position_direct_accuracy_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_trp_position_direct_accuracy_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ng_ran_access_point_position_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ngran_high_accuracy_access_point_position_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<trp_position_direct_accuracy_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_trp_position_direct_accuracy_c_names[] = {
    {"tRPPosition", nullptr},
    {"tRPHAposition", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc trp_position_direct_accuracy_c::desc = {
    asn1::table::enum_int_type<trp_position_direct_accuracy_c::types>(),
    trp_position_direct_accuracy_c::types::nof_types,
    trp_position_direct_accuracy_c::types::nof_exts,
    trp_position_direct_accuracy_c::types::has_ext,
    true,
    choice_table_trp_position_direct_accuracy_c_alternatives,
    nullptr,
    choice_table_trp_position_direct_accuracy_c_names,
    "trp_position_direct_accuracy_c",
    offsetof(trp_position_direct_accuracy_c, c.type),
    offsetof(trp_position_direct_accuracy_c, c.value),
    false,
    &trp_position_direct_accuracy_c::storage_desc};

static const char*           enum_names_trp_position_direct_accuracy_c_types_values[] = {"tRPPosition",
                                                                                         "tRPHAposition",
                                                                                         "choice-extension"};
const asn1::table::enum_desc trp_position_direct_accuracy_c::types_opts::desc         = {
    enum_names_trp_position_direct_accuracy_c_types_values,
    3,
    "trp_position_direct_accuracy_c::types"};

// TRPPositionDirect ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_position_direct_s_fields[] = {
    {offsetof(trp_position_direct_s, accuracy),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_position_direct_accuracy_c::desc},
    {offsetof(trp_position_direct_s, ie_exts),
     (int32_t)offsetof(trp_position_direct_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_position_direct_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_position_direct_s_names[] = {{"accuracy", nullptr},
                                                                                {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_trp_position_direct_s_opts[]  = {
    {1, (int32_t)offsetof(trp_position_direct_s, ie_exts_present)}};
const asn1::table::seq_desc trp_position_direct_s::desc = {
    {seq_table_trp_position_direct_s_fields, 2, UINT64_C(0x1), seq_table_trp_position_direct_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_position_direct_s_names};

// LocalOrigin ::= SEQUENCE
static const asn1::table::seq_field seq_table_local_origin_s_fields[] = {
    {offsetof(local_origin_s, relative_coordinate_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(511),
     nullptr},
    {offsetof(local_origin_s, horizontal_axes_orientation),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(3599),
     nullptr},
    {offsetof(local_origin_s, ref_point_coordinate_ha),
     (int32_t)offsetof(local_origin_s, ref_point_coordinate_ha_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ngran_high_accuracy_access_point_position_s::desc},
    {offsetof(local_origin_s, ie_exts),
     (int32_t)offsetof(local_origin_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(local_origin_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_local_origin_s_names[] = {{"relativeCoordinateID", nullptr},
                                                                         {"horizontalAxesOrientation", nullptr},
                                                                         {"referencePointCoordinateHA", nullptr},
                                                                         {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_local_origin_s_opts[]  = {
    {2, (int32_t)offsetof(local_origin_s, ref_point_coordinate_ha_present)},
    {3, (int32_t)offsetof(local_origin_s, ie_exts_present)}};
const asn1::table::seq_desc local_origin_s::desc = {
    {seq_table_local_origin_s_fields, 4, UINT64_C(0x3), seq_table_local_origin_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_local_origin_s_names};

// ReferencePoint-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_ref_point_ext_ies_o[]       = {162};
static const uint8_t  objset_crit_ref_point_ext_ies_o[]     = {crit_e::ignore};
static const uint8_t  objset_value_ref_point_ext_ies_o[]    = {ref_point_ext_ies_o::value_c::types::local_origin};
static const uint8_t  objset_presence_ref_point_ext_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ref_point_ext_ies_o[] = {
    {objset_crit_ref_point_ext_ies_o, crit_e::nulltype},
    {objset_value_ref_point_ext_ies_o, ref_point_ext_ies_o::value_c::types::nulltype},
    {objset_presence_ref_point_ext_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ref_point_ext_ies_o::desc = {objset_id_ref_point_ext_ies_o,
                                                            1,
                                                            "id",
                                                            objset_columns_ref_point_ext_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ref_point_ext_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &local_origin_s::desc}};
static const asn1::table::field_name choice_table_ref_point_ext_ies_o_value_c_names[] = {{"LocalOrigin", nullptr}};
const asn1::table::choice_desc       ref_point_ext_ies_o::value_c::desc               = {
    asn1::table::enum_int_type<ref_point_ext_ies_o::value_c::types>(),
    ref_point_ext_ies_o::value_c::types::nof_types,
    ref_point_ext_ies_o::value_c::types::nof_exts,
    ref_point_ext_ies_o::value_c::types::has_ext,
    true,
    choice_table_ref_point_ext_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ref_point_ext_ies_o_value_c_names,
    "ref_point_ext_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ref_point_ext_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_ref_point_ext_ies_o_value_c_types_values[] = {"LocalOrigin"};
const asn1::table::enum_desc ref_point_ext_ies_o::value_c::types_opts::desc        = {
    enum_names_ref_point_ext_ies_o_value_c_types_values,
    1,
    "ref_point_ext_ies_o::value_c::types"};

// ReferencePoint ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ref_point_c_alternatives[] = {
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<ng_ran_access_point_position_s, false>::value,
    &asn1::table::alternative_ops<ngran_high_accuracy_access_point_position_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<ref_point_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc ref_point_c::storage_desc = {choice_storage_ref_point_c_alternatives,
                                                                    4,
                                                                    "ref_point_c"};

uint16_t& ref_point_c::set_relative_coordinate_id()
{
  set(types::relative_coordinate_id);
  return c.get<uint16_t>();
}
ng_ran_access_point_position_s& ref_point_c::set_ref_point_coordinate()
{
  set(types::ref_point_coordinate);
  return c.get<ng_ran_access_point_position_s>();
}
ngran_high_accuracy_access_point_position_s& ref_point_c::set_ref_point_coordinate_ha()
{
  set(types::ref_point_coordinate_ha);
  return c.get<ngran_high_accuracy_access_point_position_s>();
}
protocol_ie_single_container_s<ref_point_ext_ies_o>& ref_point_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<ref_point_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_ref_point_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(511),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ng_ran_access_point_position_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ngran_high_accuracy_access_point_position_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<ref_point_ext_ies_o>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_ref_point_c_names[] = {{"relativeCoordinateID", nullptr},
                                                                         {"referencePointCoordinate", nullptr},
                                                                         {"referencePointCoordinateHA", nullptr},
                                                                         {"choice-Extension", nullptr}};
const asn1::table::choice_desc       ref_point_c::desc = {asn1::table::enum_int_type<ref_point_c::types>(),
                                                          ref_point_c::types::nof_types,
                                                          ref_point_c::types::nof_exts,
                                                          ref_point_c::types::has_ext,
                                                          true,
                                                          choice_table_ref_point_c_alternatives,
                                                          nullptr,
                                                          choice_table_ref_point_c_names,
                                                          "ref_point_c",
                                                          offsetof(ref_point_c, c.type),
                                                          offsetof(ref_point_c, c.value),
                                                          false,
                                                          &ref_point_c::storage_desc};

static const char*           enum_names_ref_point_c_types_values[] = {"relativeCoordinateID",
                                                                      "referencePointCoordinate",
                                                                      "referencePointCoordinateHA",
                                                                      "choice-Extension"};
const asn1::table::enum_desc ref_point_c::types_opts::desc         = {enum_names_ref_point_c_types_values,
                                                                      4,
                                                                      "ref_point_c::types"};

// TRPReferencePointType ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_trp_ref_point_type_c_alternatives[] = {
    &asn1::table::alternative_ops<relative_geodetic_location_s, false>::value,
    &asn1::table::alternative_ops<relative_cartesian_location_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<trp_ref_point_type_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc trp_ref_point_type_c::storage_desc = {
    choice_storage_trp_ref_point_type_c_alternatives,
    3,
    "trp_ref_point_type_c"};

relative_geodetic_location_s& trp_ref_point_type_c::set_trp_position_relative_geodetic()
{
  set(types::trp_position_relative_geodetic);
  return c.get<relative_geodetic_location_s>();
}
relative_cartesian_location_s& trp_ref_point_type_c::set_trp_position_relative_cartesian()
{
  set(types::trp_position_relative_cartesian);
  return c.get<relative_cartesian_location_s>();
}
protocol_ie_single_container_s<trp_ref_point_type_ext_ies_o>& trp_ref_point_type_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<trp_ref_point_type_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_trp_ref_point_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &relative_geodetic_location_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &relative_cartesian_location_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<trp_ref_point_type_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_trp_ref_point_type_c_names[] = {
    {"tRPPositionRelativeGeodetic", nullptr},
    {"tRPPositionRelativeCartesian", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc trp_ref_point_type_c::desc = {asn1::table::enum_int_type<trp_ref_point_type_c::types>(),
                                                             trp_ref_point_type_c::types::nof_types,
                                                             trp_ref_point_type_c::types::nof_exts,
                                                             trp_ref_point_type_c::types::has_ext,
                                                             true,
                                                             choice_table_trp_ref_point_type_c_alternatives,
                                                             nullptr,
                                                             choice_table_trp_ref_point_type_c_names,
                                                             "trp_ref_point_type_c",
                                                             offsetof(trp_ref_point_type_c, c.type),
                                                             offsetof(trp_ref_point_type_c, c.value),
                                                             false,
                                                             &trp_ref_point_type_c::storage_desc};

static const char*           enum_names_trp_ref_point_type_c_types_values[] = {"tRPPositionRelativeGeodetic",
                                                                               "tRPPositionRelativeCartesian",
                                                                               "choice-extension"};
const asn1::table::enum_desc trp_ref_point_type_c::types_opts::desc = {enum_names_trp_ref_point_type_c_types_values,
                                                                       3,
                                                                       "trp_ref_point_type_c::types"};

// TRPPositionReferenced ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_position_refd_s_fields[] = {
    {offsetof(trp_position_refd_s, ref_point),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ref_point_c::desc},
    {offsetof(trp_position_refd_s, ref_point_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_ref_point_type_c::desc},
    {offsetof(trp_position_refd_s, ie_exts),
     (int32_t)offsetof(trp_position_refd_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_position_refd_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_position_refd_s_names[] = {{"referencePoint", nullptr},
                                                                              {"referencePointType", nullptr},
                                                                              {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_trp_position_refd_s_opts[]  = {
    {2, (int32_t)offsetof(trp_position_refd_s, ie_exts_present)}};
const asn1::table::seq_desc trp_position_refd_s::desc = {
    {seq_table_trp_position_refd_s_fields, 3, UINT64_C(0x3), seq_table_trp_position_refd_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_position_refd_s_names};

// TRPPositionDefinitionType ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_trp_position_definition_type_c_alternatives[] = {
    &asn1::table::alternative_ops<trp_position_direct_s, false>::value,
    &asn1::table::alternative_ops<trp_position_refd_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<trp_position_definition_type_ext_ies_o>,
                                  false>::value};
const asn1::table::choice_storage_desc trp_position_definition_type_c::storage_desc = {
    choice_storage_trp_position_definition_type_c_alternatives,
    3,
    "trp_position_definition_type_c"};

trp_position_direct_s& trp_position_definition_type_c::set_direct()
{
  set(types::direct);
  return c.get<trp_position_direct_s>();
}
trp_position_refd_s& trp_position_definition_type_c::set_refd()
{
  set(types::refd);
  return c.get<trp_position_refd_s>();
}
protocol_ie_single_container_s<trp_position_definition_type_ext_ies_o>& trp_position_definition_type_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<trp_position_definition_type_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_trp_position_definition_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_position_direct_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_position_refd_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<trp_position_definition_type_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_trp_position_definition_type_c_names[] = {
    {"direct", nullptr},
    {"referenced", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc trp_position_definition_type_c::desc = {
    asn1::table::enum_int_type<trp_position_definition_type_c::types>(),
    trp_position_definition_type_c::types::nof_types,
    trp_position_definition_type_c::types::nof_exts,
    trp_position_definition_type_c::types::has_ext,
    true,
    choice_table_trp_position_definition_type_c_alternatives,
    nullptr,
    choice_table_trp_position_definition_type_c_names,
    "trp_position_definition_type_c",
    offsetof(trp_position_definition_type_c, c.type),
    offsetof(trp_position_definition_type_c, c.value),
    false,
    &trp_position_definition_type_c::storage_desc};

static const char*           enum_names_trp_position_definition_type_c_types_values[] = {"direct",
                                                                                         "referenced",
                                                                                         "choice-extension"};
const asn1::table::enum_desc trp_position_definition_type_c::types_opts::desc         = {
    enum_names_trp_position_definition_type_c_types_values,
    3,
    "trp_position_definition_type_c::types"};

static const uint32_t objset_id_geographical_coordinates_ext_ies_o[]   = {78};
static const uint8_t  objset_crit_geographical_coordinates_ext_ies_o[] = {crit_e::ignore};

// GeographicalCoordinates-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_geographical_coordinates_ext_ies_container_values[] = {
    {offsetof(geographical_coordinates_ext_ies_container, arp_location_info),
     (int32_t)offsetof(geographical_coordinates_ext_ies_container, arp_location_info_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16,
     &asn1::table::item_list_codec<decltype(geographical_coordinates_ext_ies_container::arp_location_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &arp_location_info_item_s::desc}};
static const int32_t ie_table_geographical_coordinates_ext_ies_container_presences[] = {
    (int32_t)offsetof(geographical_coordinates_ext_ies_container, arp_location_info_present)};
const asn1::table::ie_container_desc geographical_coordinates_ext_ies_container::desc = {
    objset_id_geographical_coordinates_ext_ies_o,
    objset_crit_geographical_coordinates_ext_ies_o,
    ie_table_geographical_coordinates_ext_ies_container_values,
    ie_table_geographical_coordinates_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// GeographicalCoordinates ::= SEQUENCE
static const asn1::table::seq_field seq_table_geographical_coordinates_s_fields[] = {
    {offsetof(geographical_coordinates_s, trp_position_definition_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_position_definition_type_c::desc},
    {offsetof(geographical_coordinates_s, dl_prs_res_coordinates),
     (int32_t)offsetof(geographical_coordinates_s, dl_prs_res_coordinates_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &dl_prs_res_coordinates_s::desc},
    {offsetof(geographical_coordinates_s, ie_exts),
     (int32_t)offsetof(geographical_coordinates_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &geographical_coordinates_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_geographical_coordinates_s_names[] = {
    {"tRPPositionDefinitionType", nullptr},
    {"dLPRSResourceCoordinates", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_geographical_coordinates_s_opts[] = {
    {1, (int32_t)offsetof(geographical_coordinates_s, dl_prs_res_coordinates_present)},
    {2, (int32_t)offsetof(geographical_coordinates_s, ie_exts_present)}};
const asn1::table::seq_desc geographical_coordinates_s::desc = {
    {seq_table_geographical_coordinates_s_fields, 3, UINT64_C(0x1), seq_table_geographical_coordinates_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_geographical_coordinates_s_names};

// TimeStampSlotIndex-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_time_stamp_slot_idx_ext_ies_o[]    = {119, 120};
static const uint8_t  objset_crit_time_stamp_slot_idx_ext_ies_o[]  = {crit_e::reject, crit_e::reject};
static const uint8_t  objset_value_time_stamp_slot_idx_ext_ies_o[] = {
    time_stamp_slot_idx_ext_ies_o::value_c::types::scs_480,
    time_stamp_slot_idx_ext_ies_o::value_c::types::scs_960};
static const uint8_t objset_presence_time_stamp_slot_idx_ext_ies_o[] = {presence_e::mandatory, presence_e::mandatory};
static const asn1::table::objset_column objset_columns_time_stamp_slot_idx_ext_ies_o[] = {
    {objset_crit_time_stamp_slot_idx_ext_ies_o, crit_e::nulltype},
    {objset_value_time_stamp_slot_idx_ext_ies_o, time_stamp_slot_idx_ext_ies_o::value_c::types::nulltype},
    {objset_presence_time_stamp_slot_idx_ext_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc time_stamp_slot_idx_ext_ies_o::desc = {objset_id_time_stamp_slot_idx_ext_ies_o,
                                                                      2,
                                                                      "id",
                                                                      objset_columns_time_stamp_slot_idx_ext_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_time_stamp_slot_idx_ext_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(319),
     &asn1::table::any_value_codec<uint16_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(639),
     &asn1::table::any_value_codec<uint16_t>::value}};
static const asn1::table::field_name choice_table_time_stamp_slot_idx_ext_ies_o_value_c_names[] = {
    {"INTEGER (0..319)", nullptr},
    {"INTEGER (0..639)", nullptr}};
const asn1::table::choice_desc time_stamp_slot_idx_ext_ies_o::value_c::desc = {
    asn1::table::enum_int_type<time_stamp_slot_idx_ext_ies_o::value_c::types>(),
    time_stamp_slot_idx_ext_ies_o::value_c::types::nof_types,
    time_stamp_slot_idx_ext_ies_o::value_c::types::nof_exts,
    time_stamp_slot_idx_ext_ies_o::value_c::types::has_ext,
    true,
    choice_table_time_stamp_slot_idx_ext_ies_o_value_c_alternatives,
    &asn1::table::choice_set<time_stamp_slot_idx_ext_ies_o::value_c>,
    choice_table_time_stamp_slot_idx_ext_ies_o_value_c_names,
    "time_stamp_slot_idx_ext_ies_o::value_c",
    offsetof(time_stamp_slot_idx_ext_ies_o::value_c, type_),
    offsetof(time_stamp_slot_idx_ext_ies_o::value_c, c),
    true,
    nullptr,
    true};

void time_stamp_slot_idx_ext_ies_o::value_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, time_stamp_slot_idx_ext_ies_o::value_c::desc);
}
uint16_t& time_stamp_slot_idx_ext_ies_o::value_c::scs_480()
{
  assert_choice_type(types::scs_480, type_, "Value");
  return c.get<uint16_t>();
}
uint16_t& time_stamp_slot_idx_ext_ies_o::value_c::scs_960()
{
  assert_choice_type(types::scs_960, type_, "Value");
  return c.get<uint16_t>();
}
const uint16_t& time_stamp_slot_idx_ext_ies_o::value_c::scs_480() const
{
  assert_choice_type(types::scs_480, type_, "Value");
  return c.get<uint16_t>();
}
const uint16_t& time_stamp_slot_idx_ext_ies_o::value_c::scs_960() const
{
  assert_choice_type(types::scs_960, type_, "Value");
  return c.get<uint16_t>();
}

static const char*           enum_names_time_stamp_slot_idx_ext_ies_o_value_c_types_values[] = {"INTEGER (0..319)",
                                                                                                "INTEGER (0..639)"};
const asn1::table::enum_desc time_stamp_slot_idx_ext_ies_o::value_c::types_opts::desc        = {
    enum_names_time_stamp_slot_idx_ext_ies_o_value_c_types_values,
    2,
    "time_stamp_slot_idx_ext_ies_o::value_c::types"};

// TimeStampSlotIndex ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_time_stamp_slot_idx_c_alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<time_stamp_slot_idx_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc time_stamp_slot_idx_c::storage_desc = {
    choice_storage_time_stamp_slot_idx_c_alternatives,
    5,
    "time_stamp_slot_idx_c"};

uint8_t& time_stamp_slot_idx_c::set_scs_15()
{
  set(types::scs_15);
  return c.get<uint8_t>();
}
uint8_t& time_stamp_slot_idx_c::set_scs_30()
{
  set(types::scs_30);
  return c.get<uint8_t>();
}
uint8_t& time_stamp_slot_idx_c::set_scs_60()
{
  set(types::scs_60);
  return c.get<uint8_t>();
}
uint8_t& time_stamp_slot_idx_c::set_scs_120()
{
  set(types::scs_120);
  return c.get<uint8_t>();
}
protocol_ie_single_container_s<time_stamp_slot_idx_ext_ies_o>& time_stamp_slot_idx_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<time_stamp_slot_idx_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_time_stamp_slot_idx_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(9),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(19),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(39),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(79),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<time_stamp_slot_idx_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_time_stamp_slot_idx_c_names[] = {{"sCS-15", nullptr},
                                                                                   {"sCS-30", nullptr},
                                                                                   {"sCS-60", nullptr},
                                                                                   {"sCS-120", nullptr},
                                                                                   {"choice-extension", nullptr}};
const asn1::table::choice_desc       time_stamp_slot_idx_c::desc                = {
    asn1::table::enum_int_type<time_stamp_slot_idx_c::types>(),
    time_stamp_slot_idx_c::types::nof_types,
    time_stamp_slot_idx_c::types::nof_exts,
    time_stamp_slot_idx_c::types::has_ext,
    true,
    choice_table_time_stamp_slot_idx_c_alternatives,
    nullptr,
    choice_table_time_stamp_slot_idx_c_names,
    "time_stamp_slot_idx_c",
    offsetof(time_stamp_slot_idx_c, c.type),
    offsetof(time_stamp_slot_idx_c, c.value),
    false,
    &time_stamp_slot_idx_c::storage_desc};

static const char*           enum_names_time_stamp_slot_idx_c_types_values[] = {"sCS-15",
                                                                                "sCS-30",
                                                                                "sCS-60",
                                                                                "sCS-120",
                                                                                "choice-extension"};
const asn1::table::enum_desc time_stamp_slot_idx_c::types_opts::desc = {enum_names_time_stamp_slot_idx_c_types_values,
                                                                        5,
                                                                        "time_stamp_slot_idx_c::types"};
int8_t                       time_stamp_slot_idx_c::types_opts::to_number() const
{
  static const int8_t numbers[] = {-15, -30, -60, -120};
  return map_enum_number(numbers, 4, value, "time_stamp_slot_idx_c::types");
}

static const uint32_t objset_id_time_stamp_ext_ies_o[]   = {127};
static const uint8_t  objset_crit_time_stamp_ext_ies_o[] = {crit_e::ignore};

// TimeStamp-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_time_stamp_ext_ies_container_values[] = {
    {offsetof(time_stamp_ext_ies_container, symbol_idx),
     (int32_t)offsetof(time_stamp_ext_ies_container, symbol_idx_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(13),
     nullptr}};
static const int32_t ie_table_time_stamp_ext_ies_container_presences[] = {
    (int32_t)offsetof(time_stamp_ext_ies_container, symbol_idx_present)};
const asn1::table::ie_container_desc time_stamp_ext_ies_container::desc = {
    objset_id_time_stamp_ext_ies_o,
    objset_crit_time_stamp_ext_ies_o,
    ie_table_time_stamp_ext_ies_container_values,
    ie_table_time_stamp_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// TimeStamp ::= SEQUENCE
static const asn1::table::seq_field seq_table_time_stamp_s_fields[] = {
    {offsetof(time_stamp_s, sys_frame_num),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {offsetof(time_stamp_s, slot_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &time_stamp_slot_idx_c::desc},
    {offsetof(time_stamp_s, meas_time),
     (int32_t)offsetof(time_stamp_s, meas_time_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(time_stamp_s::meas_time), asn1::table::struct_json>::value},
    {offsetof(time_stamp_s, ie_ext),
     (int32_t)offsetof(time_stamp_s, ie_ext_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &time_stamp_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_time_stamp_s_names[] = {{"systemFrameNumber", nullptr},
                                                                       {"slotIndex", nullptr},
                                                                       {"measurementTime", nullptr},
                                                                       {"iE-Extension", nullptr}};
static const asn1::table::seq_opt    seq_table_time_stamp_s_opts[]  = {
    {2, (int32_t)offsetof(time_stamp_s, meas_time_present)},
    {3, (int32_t)offsetof(time_stamp_s, ie_ext_present)}};
const asn1::table::seq_desc time_stamp_s::desc = {
    {seq_table_time_stamp_s_fields, 4, UINT64_C(0x3), seq_table_time_stamp_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_time_stamp_s_names};

// Mobile-TRP-LocationInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_mobile_trp_location_info_s_fields[] = {
    {offsetof(mobile_trp_location_info_s, location_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mobile_trp_location_info_s::location_info), asn1::table::string_json>::value},
    {offsetof(mobile_trp_location_info_s, velocity_info),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(mobile_trp_location_info_s::velocity_info),
                                      asn1::table::string_json>::value},
    {offsetof(mobile_trp_location_info_s, location_time_stamp),
     (int32_t)offsetof(mobile_trp_location_info_s, location_time_stamp_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &time_stamp_s::desc},
    {offsetof(mobile_trp_location_info_s, ie_exts),
     (int32_t)offsetof(mobile_trp_location_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mobile_trp_location_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mobile_trp_location_info_s_names[] = {{"location-Information", nullptr},
                                                                                     {"velocity-Information", nullptr},
                                                                                     {"location-time-stamp", nullptr},
                                                                                     {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mobile_trp_location_info_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(mobile_trp_location_info_s, location_time_stamp_present)},
    {3, (int32_t)offsetof(mobile_trp_location_info_s, ie_exts_present)}};
const asn1::table::seq_desc mobile_trp_location_info_s::desc = {
    {seq_table_mobile_trp_location_info_s_fields, 4, UINT64_C(0x1), seq_table_mobile_trp_location_info_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_mobile_trp_location_info_s_names};

static const uint32_t objset_id_measured_results_associated_info_item_ext_ies_o[]   = {164};
static const uint8_t  objset_crit_measured_results_associated_info_item_ext_ies_o[] = {crit_e::ignore};

// MeasuredResultsAssociatedInfoItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_measured_results_associated_info_item_ext_ies_container_values[] = {
    {offsetof(measured_results_associated_info_item_ext_ies_container, dl_ref_sig_ue_rx_tx_td),
     (int32_t)offsetof(measured_results_associated_info_item_ext_ies_container, dl_ref_sig_ue_rx_tx_td_present),
     asn1::table::enum_op<decltype(measured_results_associated_info_item_ext_ies_container::dl_ref_sig_ue_rx_tx_td)>(),
     decltype(measured_results_associated_info_item_ext_ies_container::dl_ref_sig_ue_rx_tx_td)::has_ext,
     false,
     decltype(measured_results_associated_info_item_ext_ies_container::dl_ref_sig_ue_rx_tx_td)::nof_types,
     decltype(measured_results_associated_info_item_ext_ies_container::dl_ref_sig_ue_rx_tx_td)::nof_exts,
     nullptr,
     &dl_ref_sig_ue_rx_tx_td_opts::desc}};
static const int32_t ie_table_measured_results_associated_info_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(measured_results_associated_info_item_ext_ies_container, dl_ref_sig_ue_rx_tx_td_present)};
const asn1::table::ie_container_desc measured_results_associated_info_item_ext_ies_container::desc = {
    objset_id_measured_results_associated_info_item_ext_ies_o,
    objset_crit_measured_results_associated_info_item_ext_ies_o,
    ie_table_measured_results_associated_info_item_ext_ies_container_values,
    ie_table_measured_results_associated_info_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// MeasuredResultsAssociatedInfoItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_measured_results_associated_info_item_s_fields[] = {
    {offsetof(measured_results_associated_info_item_s, time_stamp),
     (int32_t)offsetof(measured_results_associated_info_item_s, time_stamp_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &time_stamp_s::desc},
    {offsetof(measured_results_associated_info_item_s, meas_quality),
     (int32_t)offsetof(measured_results_associated_info_item_s, meas_quality_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_meas_quality_c::desc},
    {offsetof(measured_results_associated_info_item_s, ie_exts),
     (int32_t)offsetof(measured_results_associated_info_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &measured_results_associated_info_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_measured_results_associated_info_item_s_names[] = {
    {"timeStamp", nullptr},
    {"measurementQuality", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_measured_results_associated_info_item_s_opts[] = {
    {0, (int32_t)offsetof(measured_results_associated_info_item_s, time_stamp_present)},
    {1, (int32_t)offsetof(measured_results_associated_info_item_s, meas_quality_present)},
    {2, (int32_t)offsetof(measured_results_associated_info_item_s, ie_exts_present)}};
const asn1::table::seq_desc measured_results_associated_info_item_s::desc = {
    {seq_table_measured_results_associated_info_item_s_fields,
     3,
     UINT64_C(0x0),
     seq_table_measured_results_associated_info_item_s_opts,
     3},
    true,
    true,
    nullptr,
    0,
    seq_table_measured_results_associated_info_item_s_names};

static const uint32_t objset_id_e_c_id_meas_result_ext_ies_o[]   = {37, 116, 153};
static const uint8_t  objset_crit_e_c_id_meas_result_ext_ies_o[] = {crit_e::ignore, crit_e::ignore, crit_e::ignore};

// E-CID-MeasurementResult-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_e_c_id_meas_result_ext_ies_container_values[] = {
    {offsetof(e_c_id_meas_result_ext_ies_container, geographical_coordinates),
     (int32_t)offsetof(e_c_id_meas_result_ext_ies_container, geographical_coordinates_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &geographical_coordinates_s::desc},
    {offsetof(e_c_id_meas_result_ext_ies_container, mobile_access_point_location),
     (int32_t)offsetof(e_c_id_meas_result_ext_ies_container, mobile_access_point_location_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mobile_trp_location_info_s::desc},
    {offsetof(e_c_id_meas_result_ext_ies_container, measured_results_associated_info_list),
     (int32_t)offsetof(e_c_id_meas_result_ext_ies_container, measured_results_associated_info_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<
         decltype(e_c_id_meas_result_ext_ies_container::measured_results_associated_info_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &measured_results_associated_info_item_s::desc}};
static const int32_t ie_table_e_c_id_meas_result_ext_ies_container_presences[] = {
    (int32_t)offsetof(e_c_id_meas_result_ext_ies_container, geographical_coordinates_present),
    (int32_t)offsetof(e_c_id_meas_result_ext_ies_container, mobile_access_point_location_present),
    (int32_t)offsetof(e_c_id_meas_result_ext_ies_container, measured_results_associated_info_list_present)};
const asn1::table::ie_container_desc e_c_id_meas_result_ext_ies_container::desc = {
    objset_id_e_c_id_meas_result_ext_ies_o,
    objset_crit_e_c_id_meas_result_ext_ies_o,
    ie_table_e_c_id_meas_result_ext_ies_container_values,
    ie_table_e_c_id_meas_result_ext_ies_container_presences,
    3,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// E-CID-MeasurementResult ::= SEQUENCE
static const asn1::table::seq_field seq_table_e_c_id_meas_result_s_fields[] = {
    {offsetof(e_c_id_meas_result_s, serving_cell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ng_ran_cgi_s::desc},
    {offsetof(e_c_id_meas_result_s, serving_cell_tac),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e_c_id_meas_result_s::serving_cell_tac), asn1::table::string_json>::value},
    {offsetof(e_c_id_meas_result_s, ng_ran_access_point_position),
     (int32_t)offsetof(e_c_id_meas_result_s, ng_ran_access_point_position_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ng_ran_access_point_position_s::desc},
    {offsetof(e_c_id_meas_result_s, measured_results),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(e_c_id_meas_result_s::measured_results),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &measured_results_value_c::desc},
    {offsetof(e_c_id_meas_result_s, ie_exts),
     (int32_t)offsetof(e_c_id_meas_result_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &e_c_id_meas_result_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_e_c_id_meas_result_s_names[] = {{"servingCell-ID", nullptr},
                                                                               {"servingCellTAC", nullptr},
                                                                               {"nG-RANAccessPointPosition", nullptr},
                                                                               {"measuredResults", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_e_c_id_meas_result_s_opts[]  = {
    {2, (int32_t)offsetof(e_c_id_meas_result_s, ng_ran_access_point_position_present)},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(e_c_id_meas_result_s, ie_exts_present)}};
const asn1::table::seq_desc e_c_id_meas_result_s::desc = {
    {seq_table_e_c_id_meas_result_s_fields, 5, UINT64_C(0x3), seq_table_e_c_id_meas_result_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_e_c_id_meas_result_s_names};

// ExtendedAdditionalPathList-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_extended_add_path_list_item_s_fields[] = {
    {offsetof(extended_add_path_list_item_s, relative_time_of_path),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &relative_path_delay_c::desc},
    {offsetof(extended_add_path_list_item_s, path_quality),
     (int32_t)offsetof(extended_add_path_list_item_s, path_quality_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_meas_quality_c::desc},
    {offsetof(extended_add_path_list_item_s, multiple_ul_ao_a),
     (int32_t)offsetof(extended_add_path_list_item_s, multiple_ul_ao_a_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &multiple_ul_ao_a_s::desc},
    {offsetof(extended_add_path_list_item_s, path_pwr),
     (int32_t)offsetof(extended_add_path_list_item_s, path_pwr_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_srs_rsrp_p_s::desc},
    {offsetof(extended_add_path_list_item_s, ie_exts),
     (int32_t)offsetof(extended_add_path_list_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(extended_add_path_list_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_extended_add_path_list_item_s_names[] = {{"relativeTimeOfPath", nullptr},
                                                                                        {"pathQuality", nullptr},
                                                                                        {"multipleULAoA", nullptr},
                                                                                        {"pathPower", nullptr},
                                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_extended_add_path_list_item_s_opts[]  = {
    {1, (int32_t)offsetof(extended_add_path_list_item_s, path_quality_present)},
    {2, (int32_t)offsetof(extended_add_path_list_item_s, multiple_ul_ao_a_present)},
    {3, (int32_t)offsetof(extended_add_path_list_item_s, path_pwr_present)},
    {4, (int32_t)offsetof(extended_add_path_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc extended_add_path_list_item_s::desc = {
    {seq_table_extended_add_path_list_item_s_fields, 5, UINT64_C(0x1), seq_table_extended_add_path_list_item_s_opts, 4},
    true,
    true,
    nullptr,
    0,
    seq_table_extended_add_path_list_item_s_names};

// GNBRxTxTimeDiffMeas-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_gnb_rx_tx_time_diff_meas_ext_ies_o[] = {141, 142, 143, 144, 145, 146};
static const uint8_t  objset_crit_gnb_rx_tx_time_diff_meas_ext_ies_o[] =
    {crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};
static const uint8_t objset_value_gnb_rx_tx_time_diff_meas_ext_ies_o[] = {
    gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::types::report_granularitykminus1,
    gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::types::report_granularitykminus2,
    gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::types::report_granularitykminus3,
    gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::types::report_granularitykminus4,
    gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::types::report_granularitykminus5,
    gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::types::report_granularitykminus6};
static const uint8_t                    objset_presence_gnb_rx_tx_time_diff_meas_ext_ies_o[] = {presence_e::mandatory,
                                                                                                presence_e::mandatory,
                                                                                                presence_e::mandatory,
                                                                                                presence_e::mandatory,
                                                                                                presence_e::mandatory,
                                                                                                presence_e::mandatory};
static const asn1::table::objset_column objset_columns_gnb_rx_tx_time_diff_meas_ext_ies_o[]  = {
    {objset_crit_gnb_rx_tx_time_diff_meas_ext_ies_o, crit_e::nulltype},
    {objset_value_gnb_rx_tx_time_diff_meas_ext_ies_o, gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::types::nulltype},
    {objset_presence_gnb_rx_tx_time_diff_meas_ext_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc gnb_rx_tx_time_diff_meas_ext_ies_o::desc =
    {objset_id_gnb_rx_tx_time_diff_meas_ext_ies_o, 6, "id", objset_columns_gnb_rx_tx_time_diff_meas_ext_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_gnb_rx_tx_time_diff_meas_ext_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3940097),
     &asn1::table::any_value_codec<uint32_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(7880193),
     &asn1::table::any_value_codec<uint32_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(15760385),
     &asn1::table::any_value_codec<uint32_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(31520769),
     &asn1::table::any_value_codec<uint32_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(63041537),
     &asn1::table::any_value_codec<uint32_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(126083073),
     &asn1::table::any_value_codec<uint32_t>::value}};
static const asn1::table::field_name choice_table_gnb_rx_tx_time_diff_meas_ext_ies_o_value_c_names[] = {
    {"INTEGER (0..3940097)", nullptr},
    {"INTEGER (0..7880193)", nullptr},
    {"INTEGER (0..15760385)", nullptr},
    {"INTEGER (0..31520769)", nullptr},
    {"INTEGER (0..63041537)", nullptr},
    {"INTEGER (0..126083073)", nullptr}};
const asn1::table::choice_desc gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::desc = {
    asn1::table::enum_int_type<gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::types>(),
    gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::types::nof_types,
    gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::types::nof_exts,
    gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::types::has_ext,
    true,
    choice_table_gnb_rx_tx_time_diff_meas_ext_ies_o_value_c_alternatives,
    &asn1::table::choice_set<gnb_rx_tx_time_diff_meas_ext_ies_o::value_c>,
    choice_table_gnb_rx_tx_time_diff_meas_ext_ies_o_value_c_names,
    "gnb_rx_tx_time_diff_meas_ext_ies_o::value_c",
    offsetof(gnb_rx_tx_time_diff_meas_ext_ies_o::value_c, type_),
    offsetof(gnb_rx_tx_time_diff_meas_ext_ies_o::value_c, c),
    true,
    nullptr,
    true};

void gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::desc);
}
uint32_t& gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::report_granularitykminus1()
{
  assert_choice_type(types::report_granularitykminus1, type_, "Value");
  return c.get<uint32_t>();
}
uint32_t& gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::report_granularitykminus2()
{
  assert_choice_type(types::report_granularitykminus2, type_, "Value");
  return c.get<uint32_t>();
}
uint32_t& gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::report_granularitykminus3()
{
  assert_choice_type(types::report_granularitykminus3, type_, "Value");
  return c.get<uint32_t>();
}
uint32_t& gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::report_granularitykminus4()
{
  assert_choice_type(types::report_granularitykminus4, type_, "Value");
  return c.get<uint32_t>();
}
uint32_t& gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::report_granularitykminus5()
{
  assert_choice_type(types::report_granularitykminus5, type_, "Value");
  return c.get<uint32_t>();
}
uint32_t& gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::report_granularitykminus6()
{
  assert_choice_type(types::report_granularitykminus6, type_, "Value");
  return c.get<uint32_t>();
}
const uint32_t& gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::report_granularitykminus1() const
{
  assert_choice_type(types::report_granularitykminus1, type_, "Value");
  return c.get<uint32_t>();
}
const uint32_t& gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::report_granularitykminus2() const
{
  assert_choice_type(types::report_granularitykminus2, type_, "Value");
  return c.get<uint32_t>();
}
const uint32_t& gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::report_granularitykminus3() const
{
  assert_choice_type(types::report_granularitykminus3, type_, "Value");
  return c.get<uint32_t>();
}
const uint32_t& gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::report_granularitykminus4() const
{
  assert_choice_type(types::report_granularitykminus4, type_, "Value");
  return c.get<uint32_t>();
}
const uint32_t& gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::report_granularitykminus5() const
{
  assert_choice_type(types::report_granularitykminus5, type_, "Value");
  return c.get<uint32_t>();
}
const uint32_t& gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::report_granularitykminus6() const
{
  assert_choice_type(types::report_granularitykminus6, type_, "Value");
  return c.get<uint32_t>();
}

static const char* enum_names_gnb_rx_tx_time_diff_meas_ext_ies_o_value_c_types_values[]    = {"INTEGER (0..3940097)",
                                                                                              "INTEGER (0..7880193)",
                                                                                              "INTEGER (0..15760385)",
                                                                                              "INTEGER (0..31520769)",
                                                                                              "INTEGER (0..63041537)",
                                                                                              "INTEGER (0..126083073)"};
const asn1::table::enum_desc gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::types_opts::desc = {
    enum_names_gnb_rx_tx_time_diff_meas_ext_ies_o_value_c_types_values,
    6,
    "gnb_rx_tx_time_diff_meas_ext_ies_o::value_c::types"};

// GNBRxTxTimeDiffMeas ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_gnb_rx_tx_time_diff_meas_c_alternatives[] = {
    &asn1::table::alternative_ops<uint32_t, true>::value,
    &asn1::table::alternative_ops<uint32_t, true>::value,
    &asn1::table::alternative_ops<uint32_t, true>::value,
    &asn1::table::alternative_ops<uint32_t, true>::value,
    &asn1::table::alternative_ops<uint32_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<gnb_rx_tx_time_diff_meas_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc gnb_rx_tx_time_diff_meas_c::storage_desc = {
    choice_storage_gnb_rx_tx_time_diff_meas_c_alternatives,
    7,
    "gnb_rx_tx_time_diff_meas_c"};

uint32_t& gnb_rx_tx_time_diff_meas_c::set_k0()
{
  set(types::k0);
  return c.get<uint32_t>();
}
uint32_t& gnb_rx_tx_time_diff_meas_c::set_k1()
{
  set(types::k1);
  return c.get<uint32_t>();
}
uint32_t& gnb_rx_tx_time_diff_meas_c::set_k2()
{
  set(types::k2);
  return c.get<uint32_t>();
}
uint32_t& gnb_rx_tx_time_diff_meas_c::set_k3()
{
  set(types::k3);
  return c.get<uint32_t>();
}
uint32_t& gnb_rx_tx_time_diff_meas_c::set_k4()
{
  set(types::k4);
  return c.get<uint32_t>();
}
uint16_t& gnb_rx_tx_time_diff_meas_c::set_k5()
{
  set(types::k5);
  return c.get<uint16_t>();
}
protocol_ie_single_container_s<gnb_rx_tx_time_diff_meas_ext_ies_o>& gnb_rx_tx_time_diff_meas_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<gnb_rx_tx_time_diff_meas_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_gnb_rx_tx_time_diff_meas_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(1970049),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(985025),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(492513),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(246257),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(123129),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(61565),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<gnb_rx_tx_time_diff_meas_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_gnb_rx_tx_time_diff_meas_c_names[] = {{"k0", nullptr},
                                                                                        {"k1", nullptr},
                                                                                        {"k2", nullptr},
                                                                                        {"k3", nullptr},
                                                                                        {"k4", nullptr},
                                                                                        {"k5", nullptr},
                                                                                        {"choice-extension", nullptr}};
const asn1::table::choice_desc       gnb_rx_tx_time_diff_meas_c::desc                = {
    asn1::table::enum_int_type<gnb_rx_tx_time_diff_meas_c::types>(),
    gnb_rx_tx_time_diff_meas_c::types::nof_types,
    gnb_rx_tx_time_diff_meas_c::types::nof_exts,
    gnb_rx_tx_time_diff_meas_c::types::has_ext,
    true,
    choice_table_gnb_rx_tx_time_diff_meas_c_alternatives,
    nullptr,
    choice_table_gnb_rx_tx_time_diff_meas_c_names,
    "gnb_rx_tx_time_diff_meas_c",
    offsetof(gnb_rx_tx_time_diff_meas_c, c.type),
    offsetof(gnb_rx_tx_time_diff_meas_c, c.value),
    false,
    &gnb_rx_tx_time_diff_meas_c::storage_desc};

static const char* enum_names_gnb_rx_tx_time_diff_meas_c_types_values[] =
    {"k0", "k1", "k2", "k3", "k4", "k5", "choice-extension"};
const asn1::table::enum_desc gnb_rx_tx_time_diff_meas_c::types_opts::desc = {
    enum_names_gnb_rx_tx_time_diff_meas_c_types_values,
    7,
    "gnb_rx_tx_time_diff_meas_c::types"};
uint8_t gnb_rx_tx_time_diff_meas_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5};
  return map_enum_number(numbers, 6, value, "gnb_rx_tx_time_diff_meas_c::types");
}

// RxTxTimingErrorMargin ::= ENUMERATED
static const char*           enum_names_rx_tx_timing_error_margin_e_values[] = {"tc0dot5",
                                                                                "tc1",
                                                                                "tc2",
                                                                                "tc4",
                                                                                "tc8",
                                                                                "tc12",
                                                                                "tc16",
                                                                                "tc20",
                                                                                "tc24",
                                                                                "tc32",
                                                                                "tc40",
                                                                                "tc48",
                                                                                "tc64",
                                                                                "tc80",
                                                                                "tc96",
                                                                                "tc128"};
const asn1::table::enum_desc rx_tx_timing_error_margin_opts::desc = {enum_names_rx_tx_timing_error_margin_e_values,
                                                                     16,
                                                                     "rx_tx_timing_error_margin_e"};
float                        rx_tx_timing_error_margin_opts::to_number() const
{
  static const float numbers[] = {
      0.5, 1.0, 2.0, 4.0, 8.0, 12.0, 16.0, 20.0, 24.0, 32.0, 40.0, 48.0, 64.0, 80.0, 96.0, 128.0};
  return map_enum_number(numbers, 16, value, "rx_tx_timing_error_margin_e");
}
const char* rx_tx_timing_error_margin_opts::to_number_string() const
{
  static const char* number_strs[] = {
      "0.5", "1", "2", "4", "8", "12", "16", "20", "24", "32", "40", "48", "64", "80", "96", "128"};
  return convert_enum_idx(number_strs, 16, value, "rx_tx_timing_error_margin_e");
}

// TRP-RxTx-TEGInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_rx_tx_teg_info_s_fields[] = {
    {offsetof(trp_rx_tx_teg_info_s, trp_rx_tx_teg_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(trp_rx_tx_teg_info_s, trp_rx_tx_timing_error_margin),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(trp_rx_tx_teg_info_s::trp_rx_tx_timing_error_margin)>(),
     decltype(trp_rx_tx_teg_info_s::trp_rx_tx_timing_error_margin)::has_ext,
     false,
     decltype(trp_rx_tx_teg_info_s::trp_rx_tx_timing_error_margin)::nof_types,
     decltype(trp_rx_tx_teg_info_s::trp_rx_tx_timing_error_margin)::nof_exts,
     nullptr,
     &rx_tx_timing_error_margin_opts::desc},
    {offsetof(trp_rx_tx_teg_info_s, ie_exts),
     (int32_t)offsetof(trp_rx_tx_teg_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_rx_tx_teg_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_rx_tx_teg_info_s_names[] = {{"tRP-RxTx-TEGID", nullptr},
                                                                               {"tRP-RxTx-TimingErrorMargin", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_trp_rx_tx_teg_info_s_opts[]  = {
    {2, (int32_t)offsetof(trp_rx_tx_teg_info_s, ie_exts_present)}};
const asn1::table::seq_desc trp_rx_tx_teg_info_s::desc = {
    {seq_table_trp_rx_tx_teg_info_s_fields, 3, UINT64_C(0x3), seq_table_trp_rx_tx_teg_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_rx_tx_teg_info_s_names};

// TimingErrorMargin ::= ENUMERATED
static const char*           enum_names_timing_error_margin_e_values[] = {"tc0",
                                                                          "tc2",
                                                                          "tc4",
                                                                          "tc6",
                                                                          "tc8",
                                                                          "tc12",
                                                                          "tc16",
                                                                          "tc20",
                                                                          "tc24",
                                                                          "tc32",
                                                                          "tc40",
                                                                          "tc48",
                                                                          "tc56",
                                                                          "tc64",
                                                                          "tc72",
                                                                          "tc80"};
const asn1::table::enum_desc timing_error_margin_opts::desc            = {enum_names_timing_error_margin_e_values,
                                                                          16,
                                                                          "timing_error_margin_e"};
uint8_t                      timing_error_margin_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 2, 4, 6, 8, 12, 16, 20, 24, 32, 40, 48, 56, 64, 72, 80};
  return map_enum_number(numbers, 16, value, "timing_error_margin_e");
}

// TRP-Tx-TEGInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_tx_teg_info_s_fields[] = {
    {offsetof(trp_tx_teg_info_s, trp_tx_teg_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(trp_tx_teg_info_s, trp_tx_timing_error_margin),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(trp_tx_teg_info_s::trp_tx_timing_error_margin)>(),
     decltype(trp_tx_teg_info_s::trp_tx_timing_error_margin)::has_ext,
     false,
     decltype(trp_tx_teg_info_s::trp_tx_timing_error_margin)::nof_types,
     decltype(trp_tx_teg_info_s::trp_tx_timing_error_margin)::nof_exts,
     nullptr,
     &timing_error_margin_opts::desc},
    {offsetof(trp_tx_teg_info_s, ie_exts),
     (int32_t)offsetof(trp_tx_teg_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_tx_teg_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_tx_teg_info_s_names[] = {{"tRP-Tx-TEGID", nullptr},
                                                                            {"tRP-Tx-TimingErrorMargin", nullptr},
                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_trp_tx_teg_info_s_opts[]  = {
    {2, (int32_t)offsetof(trp_tx_teg_info_s, ie_exts_present)}};
const asn1::table::seq_desc trp_tx_teg_info_s::desc = {
    {seq_table_trp_tx_teg_info_s_fields, 3, UINT64_C(0x3), seq_table_trp_tx_teg_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_tx_teg_info_s_names};

// RxTxTEG ::= SEQUENCE
static const asn1::table::seq_field seq_table_rx_tx_teg_s_fields[] = {
    {offsetof(rx_tx_teg_s, trp_rx_tx_teg_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_rx_tx_teg_info_s::desc},
    {offsetof(rx_tx_teg_s, trp_tx_teg_info),
     (int32_t)offsetof(rx_tx_teg_s, trp_tx_teg_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_tx_teg_info_s::desc},
    {offsetof(rx_tx_teg_s, ie_exts),
     (int32_t)offsetof(rx_tx_teg_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rx_tx_teg_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_rx_tx_teg_s_names[] = {{"tRP-RxTx-TEGInformation", nullptr},
                                                                      {"tRP-Tx-TEGInformation", nullptr},
                                                                      {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_rx_tx_teg_s_opts[]  = {
    {1, (int32_t)offsetof(rx_tx_teg_s, trp_tx_teg_info_present)},
    {2, (int32_t)offsetof(rx_tx_teg_s, ie_exts_present)}};
const asn1::table::seq_desc rx_tx_teg_s::desc = {
    {seq_table_rx_tx_teg_s_fields, 3, UINT64_C(0x1), seq_table_rx_tx_teg_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_rx_tx_teg_s_names};

// TRP-Rx-TEGInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_rx_teg_info_s_fields[] = {
    {offsetof(trp_rx_teg_info_s, trp_rx_teg_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(trp_rx_teg_info_s, trp_rx_timing_error_margin),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(trp_rx_teg_info_s::trp_rx_timing_error_margin)>(),
     decltype(trp_rx_teg_info_s::trp_rx_timing_error_margin)::has_ext,
     false,
     decltype(trp_rx_teg_info_s::trp_rx_timing_error_margin)::nof_types,
     decltype(trp_rx_teg_info_s::trp_rx_timing_error_margin)::nof_exts,
     nullptr,
     &timing_error_margin_opts::desc},
    {offsetof(trp_rx_teg_info_s, ie_exts),
     (int32_t)offsetof(trp_rx_teg_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_rx_teg_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_rx_teg_info_s_names[] = {{"tRP-Rx-TEGID", nullptr},
                                                                            {"tRP-Rx-TimingErrorMargin", nullptr},
                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_trp_rx_teg_info_s_opts[]  = {
    {2, (int32_t)offsetof(trp_rx_teg_info_s, ie_exts_present)}};
const asn1::table::seq_desc trp_rx_teg_info_s::desc = {
    {seq_table_trp_rx_teg_info_s_fields, 3, UINT64_C(0x3), seq_table_trp_rx_teg_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_rx_teg_info_s_names};

// RxTEG ::= SEQUENCE
static const asn1::table::seq_field seq_table_rx_teg_s_fields[] = {
    {offsetof(rx_teg_s, trp_rx_teg_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_rx_teg_info_s::desc},
    {offsetof(rx_teg_s, trp_tx_teg_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_tx_teg_info_s::desc},
    {offsetof(rx_teg_s, ie_exts),
     (int32_t)offsetof(rx_teg_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rx_teg_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_rx_teg_s_names[] = {{"tRP-Rx-TEGInformation", nullptr},
                                                                   {"tRP-Tx-TEGInformation", nullptr},
                                                                   {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_rx_teg_s_opts[]  = {{2, (int32_t)offsetof(rx_teg_s, ie_exts_present)}};
const asn1::table::seq_desc rx_teg_s::desc = {{seq_table_rx_teg_s_fields, 3, UINT64_C(0x3), seq_table_rx_teg_s_opts, 1},
                                              true,
                                              true,
                                              nullptr,
                                              0,
                                              seq_table_rx_teg_s_names};

// TRPTEGInformation ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_trpteg_info_c_alternatives[] = {
    &asn1::table::alternative_ops<rx_tx_teg_s, false>::value,
    &asn1::table::alternative_ops<rx_teg_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<trpteg_info_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc trpteg_info_c::storage_desc = {choice_storage_trpteg_info_c_alternatives,
                                                                      3,
                                                                      "trpteg_info_c"};

rx_tx_teg_s& trpteg_info_c::set_rx_tx_teg()
{
  set(types::rx_tx_teg);
  return c.get<rx_tx_teg_s>();
}
rx_teg_s& trpteg_info_c::set_rx_teg()
{
  set(types::rx_teg);
  return c.get<rx_teg_s>();
}
protocol_ie_single_container_s<trpteg_info_ext_ies_o>& trpteg_info_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<trpteg_info_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_trpteg_info_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rx_tx_teg_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rx_teg_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<trpteg_info_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_trpteg_info_c_names[] = {{"rxTx-TEG", nullptr},
                                                                           {"rx-TEG", nullptr},
                                                                           {"choice-extension", nullptr}};
const asn1::table::choice_desc       trpteg_info_c::desc = {asn1::table::enum_int_type<trpteg_info_c::types>(),
                                                            trpteg_info_c::types::nof_types,
                                                            trpteg_info_c::types::nof_exts,
                                                            trpteg_info_c::types::has_ext,
                                                            true,
                                                            choice_table_trpteg_info_c_alternatives,
                                                            nullptr,
                                                            choice_table_trpteg_info_c_names,
                                                            "trpteg_info_c",
                                                            offsetof(trpteg_info_c, c.type),
                                                            offsetof(trpteg_info_c, c.value),
                                                            false,
                                                            &trpteg_info_c::storage_desc};

static const char*           enum_names_trpteg_info_c_types_values[] = {"rxTx-TEG", "rx-TEG", "choice-extension"};
const asn1::table::enum_desc trpteg_info_c::types_opts::desc         = {enum_names_trpteg_info_c_types_values,
                                                                        3,
                                                                        "trpteg_info_c::types"};

static const uint32_t objset_id_gnb_rx_tx_time_diff_ext_ies_o[]   = {77, 85};
static const uint8_t  objset_crit_gnb_rx_tx_time_diff_ext_ies_o[] = {crit_e::ignore, crit_e::ignore};

// GNB-RxTxTimeDiff-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_gnb_rx_tx_time_diff_ext_ies_container_values[] = {
    {offsetof(gnb_rx_tx_time_diff_ext_ies_container, extended_add_path_list),
     (int32_t)offsetof(gnb_rx_tx_time_diff_ext_ies_container, extended_add_path_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(gnb_rx_tx_time_diff_ext_ies_container::extended_add_path_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &extended_add_path_list_item_s::desc},
    {offsetof(gnb_rx_tx_time_diff_ext_ies_container, trpteg_info),
     (int32_t)offsetof(gnb_rx_tx_time_diff_ext_ies_container, trpteg_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &trpteg_info_c::desc}};
static const int32_t ie_table_gnb_rx_tx_time_diff_ext_ies_container_presences[] = {
    (int32_t)offsetof(gnb_rx_tx_time_diff_ext_ies_container, extended_add_path_list_present),
    (int32_t)offsetof(gnb_rx_tx_time_diff_ext_ies_container, trpteg_info_present)};
const asn1::table::ie_container_desc gnb_rx_tx_time_diff_ext_ies_container::desc = {
    objset_id_gnb_rx_tx_time_diff_ext_ies_o,
    objset_crit_gnb_rx_tx_time_diff_ext_ies_o,
    ie_table_gnb_rx_tx_time_diff_ext_ies_container_values,
    ie_table_gnb_rx_tx_time_diff_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// GNB-RxTxTimeDiff ::= SEQUENCE
static const asn1::table::seq_field seq_table_gnb_rx_tx_time_diff_s_fields[] = {
    {offsetof(gnb_rx_tx_time_diff_s, rx_tx_time_diff),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &gnb_rx_tx_time_diff_meas_c::desc},
    {offsetof(gnb_rx_tx_time_diff_s, add_path_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2,
     &asn1::table::item_list_codec<decltype(gnb_rx_tx_time_diff_s::add_path_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &add_path_list_item_s::desc},
    {offsetof(gnb_rx_tx_time_diff_s, ie_exts),
     (int32_t)offsetof(gnb_rx_tx_time_diff_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &gnb_rx_tx_time_diff_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_gnb_rx_tx_time_diff_s_names[] = {{"rxTxTimeDiff", nullptr},
                                                                                {"additionalPathList", nullptr},
                                                                                {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_gnb_rx_tx_time_diff_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(gnb_rx_tx_time_diff_s, ie_exts_present)}};
const asn1::table::seq_desc gnb_rx_tx_time_diff_s::desc = {
    {seq_table_gnb_rx_tx_time_diff_s_fields, 3, UINT64_C(0x1), seq_table_gnb_rx_tx_time_diff_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_gnb_rx_tx_time_diff_s_names};

// LCS-to-GCS-TranslationItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_lcs_to_gcs_translation_item_s_fields[] = {
    {offsetof(lcs_to_gcs_translation_item_s, alpha),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(359),
     nullptr},
    {offsetof(lcs_to_gcs_translation_item_s, alpha_fine),
     (int32_t)offsetof(lcs_to_gcs_translation_item_s, alpha_fine_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(9),
     nullptr},
    {offsetof(lcs_to_gcs_translation_item_s, beta),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(359),
     nullptr},
    {offsetof(lcs_to_gcs_translation_item_s, beta_fine),
     (int32_t)offsetof(lcs_to_gcs_translation_item_s, beta_fine_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(9),
     nullptr},
    {offsetof(lcs_to_gcs_translation_item_s, gamma),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(359),
     nullptr},
    {offsetof(lcs_to_gcs_translation_item_s, gamma_fine),
     (int32_t)offsetof(lcs_to_gcs_translation_item_s, gamma_fine_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(9),
     nullptr},
    {offsetof(lcs_to_gcs_translation_item_s, ie_exts),
     (int32_t)offsetof(lcs_to_gcs_translation_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(lcs_to_gcs_translation_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_lcs_to_gcs_translation_item_s_names[] = {{"alpha", nullptr},
                                                                                        {"alphaFine", nullptr},
                                                                                        {"beta", nullptr},
                                                                                        {"betaFine", nullptr},
                                                                                        {"gamma", nullptr},
                                                                                        {"gammaFine", nullptr},
                                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_lcs_to_gcs_translation_item_s_opts[]  = {
    {1, (int32_t)offsetof(lcs_to_gcs_translation_item_s, alpha_fine_present)},
    {3, (int32_t)offsetof(lcs_to_gcs_translation_item_s, beta_fine_present)},
    {5, (int32_t)offsetof(lcs_to_gcs_translation_item_s, gamma_fine_present)},
    {6, (int32_t)offsetof(lcs_to_gcs_translation_item_s, ie_exts_present)}};
const asn1::table::seq_desc lcs_to_gcs_translation_item_s::desc = {{seq_table_lcs_to_gcs_translation_item_s_fields,
                                                                    7,
                                                                    UINT64_C(0x15),
                                                                    seq_table_lcs_to_gcs_translation_item_s_opts,
                                                                    4},
                                                                   true,
                                                                   true,
                                                                   nullptr,
                                                                   0,
                                                                   seq_table_lcs_to_gcs_translation_item_s_names};

// LoS-NLoSIndicatorHard ::= ENUMERATED
static const char*           enum_names_lo_s_n_lo_si_ndicator_hard_e_values[] = {"nlos", "los"};
const asn1::table::enum_desc lo_s_n_lo_si_ndicator_hard_opts::desc = {enum_names_lo_s_n_lo_si_ndicator_hard_e_values,
                                                                      2,
                                                                      "lo_s_n_lo_si_ndicator_hard_e"};

// LoS-NLoSInformation ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_lo_s_n_lo_si_nformation_c_alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<lo_s_n_lo_si_ndicator_hard_e, true>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<lo_s_n_lo_si_nformation_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc lo_s_n_lo_si_nformation_c::storage_desc = {
    choice_storage_lo_s_n_lo_si_nformation_c_alternatives,
    3,
    "lo_s_n_lo_si_nformation_c"};

uint8_t& lo_s_n_lo_si_nformation_c::set_lo_s_n_lo_si_ndicator_soft()
{
  set(types::lo_s_n_lo_si_ndicator_soft);
  return c.get<uint8_t>();
}
lo_s_n_lo_si_ndicator_hard_e& lo_s_n_lo_si_nformation_c::set_lo_s_n_lo_si_ndicator_hard()
{
  set(types::lo_s_n_lo_si_ndicator_hard);
  return c.get<lo_s_n_lo_si_ndicator_hard_e>();
}
protocol_ie_single_container_s<lo_s_n_lo_si_nformation_ext_ies_o>& lo_s_n_lo_si_nformation_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<lo_s_n_lo_si_nformation_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_lo_s_n_lo_si_nformation_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(10),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<lo_s_n_lo_si_ndicator_hard_e>(),
     lo_s_n_lo_si_ndicator_hard_e::has_ext,
     false,
     lo_s_n_lo_si_ndicator_hard_e::nof_types,
     lo_s_n_lo_si_ndicator_hard_e::nof_exts,
     nullptr,
     &lo_s_n_lo_si_ndicator_hard_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<lo_s_n_lo_si_nformation_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_lo_s_n_lo_si_nformation_c_names[] = {
    {"loS-NLoSIndicatorSoft", nullptr},
    {"loS-NLoSIndicatorHard", nullptr},
    {"choice-Extension", nullptr}};
const asn1::table::choice_desc lo_s_n_lo_si_nformation_c::desc = {
    asn1::table::enum_int_type<lo_s_n_lo_si_nformation_c::types>(),
    lo_s_n_lo_si_nformation_c::types::nof_types,
    lo_s_n_lo_si_nformation_c::types::nof_exts,
    lo_s_n_lo_si_nformation_c::types::has_ext,
    true,
    choice_table_lo_s_n_lo_si_nformation_c_alternatives,
    nullptr,
    choice_table_lo_s_n_lo_si_nformation_c_names,
    "lo_s_n_lo_si_nformation_c",
    offsetof(lo_s_n_lo_si_nformation_c, c.type),
    offsetof(lo_s_n_lo_si_nformation_c, c.value),
    false,
    &lo_s_n_lo_si_nformation_c::storage_desc};

static const char*           enum_names_lo_s_n_lo_si_nformation_c_types_values[] = {"loS-NLoSIndicatorSoft",
                                                                                    "loS-NLoSIndicatorHard",
                                                                                    "choice-Extension"};
const asn1::table::enum_desc lo_s_n_lo_si_nformation_c::types_opts::desc         = {
    enum_names_lo_s_n_lo_si_nformation_c_types_values,
    3,
    "lo_s_n_lo_si_nformation_c::types"};

// MeasBasedOnAggregatedResources ::= ENUMERATED
static const char*           enum_names_meas_based_on_aggr_res_e_values[] = {"true"};
const asn1::table::enum_desc meas_based_on_aggr_res_opts::desc            = {enum_names_meas_based_on_aggr_res_e_values,
                                                                             1,
                                                                             "meas_based_on_aggr_res_e"};

// MeasuredFrequencyHops ::= ENUMERATED
static const char*           enum_names_measured_freq_hops_e_values[] = {"singleHop", "multiHop"};
const asn1::table::enum_desc measured_freq_hops_opts::desc            = {enum_names_measured_freq_hops_e_values,
                                                                         2,
                                                                         "measured_freq_hops_e"};

// MeasurementAmount ::= ENUMERATED
static const char* enum_names_meas_amount_e_values[] = {"ma0", "ma1", "ma2", "ma4", "ma8", "ma16", "ma32", "ma64"};
const asn1::table::enum_desc meas_amount_opts::desc  = {enum_names_meas_amount_e_values, 8, "meas_amount_e"};
uint8_t                      meas_amount_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 4, 8, 16, 32, 64};
  return map_enum_number(numbers, 8, value, "meas_amount_e");
}

// MeasurementBeamInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_beam_info_s_fields[] = {
    {offsetof(meas_beam_info_s, prs_res_id),
     (int32_t)offsetof(meas_beam_info_s, prs_res_id_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(meas_beam_info_s, prs_res_set_id),
     (int32_t)offsetof(meas_beam_info_s, prs_res_set_id_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(meas_beam_info_s, ssb_idx),
     (int32_t)offsetof(meas_beam_info_s, ssb_idx_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(meas_beam_info_s, ie_exts),
     (int32_t)offsetof(meas_beam_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(meas_beam_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_meas_beam_info_s_names[] = {{"pRS-Resource-ID", nullptr},
                                                                           {"pRS-Resource-Set-ID", nullptr},
                                                                           {"sSB-Index", nullptr},
                                                                           {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_beam_info_s_opts[]  = {
    {0, (int32_t)offsetof(meas_beam_info_s, prs_res_id_present)},
    {1, (int32_t)offsetof(meas_beam_info_s, prs_res_set_id_present)},
    {2, (int32_t)offsetof(meas_beam_info_s, ssb_idx_present)},
    {3, (int32_t)offsetof(meas_beam_info_s, ie_exts_present)}};
const asn1::table::seq_desc meas_beam_info_s::desc = {
    {seq_table_meas_beam_info_s_fields, 4, UINT64_C(0x0), seq_table_meas_beam_info_s_opts, 4},
    true,
    true,
    nullptr,
    0,
    seq_table_meas_beam_info_s_names};

// MeasurementBeamInfoRequest ::= ENUMERATED
static const char*           enum_names_meas_beam_info_request_e_values[] = {"true"};
const asn1::table::enum_desc meas_beam_info_request_opts::desc            = {enum_names_meas_beam_info_request_e_values,
                                                                             1,
                                                                             "meas_beam_info_request_e"};

// MeasurementPeriodicity ::= ENUMERATED
static const char*           enum_names_meas_periodicity_e_values[] = {"ms120",
                                                                       "ms240",
                                                                       "ms480",
                                                                       "ms640",
                                                                       "ms1024",
                                                                       "ms2048",
                                                                       "ms5120",
                                                                       "ms10240",
                                                                       "min1",
                                                                       "min6",
                                                                       "min12",
                                                                       "min30",
                                                                       "min60",
                                                                       "ms20480",
                                                                       "ms40960",
                                                                       "extended"};
const asn1::table::enum_desc meas_periodicity_opts::desc            = {enum_names_meas_periodicity_e_values,
                                                                       16,
                                                                       "meas_periodicity_e"};
uint16_t                     meas_periodicity_opts::to_number() const
{
  static const uint16_t numbers[] = {120, 240, 480, 640, 1024, 2048, 5120, 10240, 1, 6, 12, 30, 60, 20480, 40960};
  return map_enum_number(numbers, 15, value, "meas_periodicity_e");
}

// MeasurementPeriodicityExtended ::= ENUMERATED
static const char* enum_names_meas_periodicity_extended_e_values[] =
    {"ms160", "ms320", "ms1280", "ms2560", "ms61440", "ms81920", "ms368640", "ms737280", "ms1843200"};
const asn1::table::enum_desc meas_periodicity_extended_opts::desc = {enum_names_meas_periodicity_extended_e_values,
                                                                     9,
                                                                     "meas_periodicity_extended_e"};
uint32_t                     meas_periodicity_extended_opts::to_number() const
{
  static const uint32_t numbers[] = {160, 320, 1280, 2560, 61440, 81920, 368640, 737280, 1843200};
  return map_enum_number(numbers, 9, value, "meas_periodicity_extended_e");
}

// MeasurementPeriodicityNR-AoA ::= ENUMERATED
static const char*           enum_names_meas_periodicity_nr_ao_a_e_values[] = {"ms160",
                                                                               "ms320",
                                                                               "ms640",
                                                                               "ms1280",
                                                                               "ms2560",
                                                                               "ms5120",
                                                                               "ms10240",
                                                                               "ms20480",
                                                                               "ms40960",
                                                                               "ms61440",
                                                                               "ms81920",
                                                                               "ms368640",
                                                                               "ms737280",
                                                                               "ms1843200"};
const asn1::table::enum_desc meas_periodicity_nr_ao_a_opts::desc = {enum_names_meas_periodicity_nr_ao_a_e_values,
                                                                    14,
                                                                    "meas_periodicity_nr_ao_a_e"};
uint32_t                     meas_periodicity_nr_ao_a_opts::to_number() const
{
  static const uint32_t numbers[] = {
      160, 320, 640, 1280, 2560, 5120, 10240, 20480, 40960, 61440, 81920, 368640, 737280, 1843200};
  return map_enum_number(numbers, 14, value, "meas_periodicity_nr_ao_a_e");
}

// MeasurementQuantitiesValue ::= ENUMERATED
static const char*           enum_names_meas_quantities_value_e_values[] = {"cell-ID",
                                                                            "angleOfArrival",
                                                                            "timingAdvanceType1",
                                                                            "timingAdvanceType2",
                                                                            "rSRP",
                                                                            "rSRQ",
                                                                            "sS-RSRP",
                                                                            "sS-RSRQ",
                                                                            "cSI-RSRP",
                                                                            "cSI-RSRQ",
                                                                            "angleOfArrivalNR",
                                                                            "timingAdvanceNR",
                                                                            "uE-Rx-Tx-Time-Diff"};
const asn1::table::enum_desc meas_quantities_value_opts::desc            = {enum_names_meas_quantities_value_e_values,
                                                                            13,
                                                                            "meas_quantities_value_e"};
uint8_t                      meas_quantities_value_opts::to_number() const
{
  switch (value) {
    case timing_advance_type1:
      return 1;
    case timing_advance_type2:
      return 2;
    default:
      invalid_enum_number(value, "meas_quantities_value_e");
  }
  return 0;
}

// MeasurementQuantities-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_quantities_item_s_fields[] = {
    {offsetof(meas_quantities_item_s, meas_quantities_value),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(meas_quantities_item_s::meas_quantities_value)>(),
     decltype(meas_quantities_item_s::meas_quantities_value)::has_ext,
     false,
     decltype(meas_quantities_item_s::meas_quantities_value)::nof_types,
     decltype(meas_quantities_item_s::meas_quantities_value)::nof_exts,
     nullptr,
     &meas_quantities_value_opts::desc},
    {offsetof(meas_quantities_item_s, ie_exts),
     (int32_t)offsetof(meas_quantities_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(meas_quantities_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_meas_quantities_item_s_names[] = {
    {"measurementQuantitiesValue", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_meas_quantities_item_s_opts[] = {
    {1, (int32_t)offsetof(meas_quantities_item_s, ie_exts_present)}};
const asn1::table::seq_desc meas_quantities_item_s::desc = {
    {seq_table_meas_quantities_item_s_fields, 2, UINT64_C(0x1), seq_table_meas_quantities_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_meas_quantities_item_s_names};

// MeasurementQuantities-ItemIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_meas_quantities_item_ies_o[]    = {11};
static const uint8_t  objset_crit_meas_quantities_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_meas_quantities_item_ies_o[] = {
    meas_quantities_item_ies_o::value_c::types::meas_quantities_item};
static const uint8_t                    objset_presence_meas_quantities_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_meas_quantities_item_ies_o[]  = {
    {objset_crit_meas_quantities_item_ies_o, crit_e::nulltype},
    {objset_value_meas_quantities_item_ies_o, meas_quantities_item_ies_o::value_c::types::nulltype},
    {objset_presence_meas_quantities_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc meas_quantities_item_ies_o::desc = {objset_id_meas_quantities_item_ies_o,
                                                                   1,
                                                                   "id",
                                                                   objset_columns_meas_quantities_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_meas_quantities_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_quantities_item_s::desc}};
static const asn1::table::field_name choice_table_meas_quantities_item_ies_o_value_c_names[] = {
    {"MeasurementQuantities-Item", nullptr}};
const asn1::table::choice_desc meas_quantities_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<meas_quantities_item_ies_o::value_c::types>(),
    meas_quantities_item_ies_o::value_c::types::nof_types,
    meas_quantities_item_ies_o::value_c::types::nof_exts,
    meas_quantities_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_meas_quantities_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_meas_quantities_item_ies_o_value_c_names,
    "meas_quantities_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(meas_quantities_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_meas_quantities_item_ies_o_value_c_types_values[]    = {"MeasurementQuantities-Item"};
const asn1::table::enum_desc meas_quantities_item_ies_o::value_c::types_opts::desc = {
    enum_names_meas_quantities_item_ies_o_value_c_types_values,
    1,
    "meas_quantities_item_ies_o::value_c::types"};

// MeasurementTimeOccasion ::= ENUMERATED
static const char*           enum_names_meas_time_occasion_e_values[] = {"o1", "o4"};
const asn1::table::enum_desc meas_time_occasion_opts::desc            = {enum_names_meas_time_occasion_e_values,
                                                                         2,
                                                                         "meas_time_occasion_e"};
uint8_t                      meas_time_occasion_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 4};
  return map_enum_number(numbers, 2, value, "meas_time_occasion_e");
}

static const uint32_t objset_id_prs_angle_item_ext_ies_o[]   = {65};
static const uint8_t  objset_crit_prs_angle_item_ext_ies_o[] = {crit_e::ignore};

// PRSAngleItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_prs_angle_item_ext_ies_container_values[] = {
    {offsetof(prs_angle_item_ext_ies_container, prs_res_id),
     (int32_t)offsetof(prs_angle_item_ext_ies_container, prs_res_id_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr}};
static const int32_t ie_table_prs_angle_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(prs_angle_item_ext_ies_container, prs_res_id_present)};
const asn1::table::ie_container_desc prs_angle_item_ext_ies_container::desc = {
    objset_id_prs_angle_item_ext_ies_o,
    objset_crit_prs_angle_item_ext_ies_o,
    ie_table_prs_angle_item_ext_ies_container_values,
    ie_table_prs_angle_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// PRSAngleItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_angle_item_s_fields[] = {
    {offsetof(prs_angle_item_s, nr_prs_azimuth),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(359),
     nullptr},
    {offsetof(prs_angle_item_s, nr_prs_azimuth_fine),
     (int32_t)offsetof(prs_angle_item_s, nr_prs_azimuth_fine_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(9),
     nullptr},
    {offsetof(prs_angle_item_s, nr_prs_elevation),
     (int32_t)offsetof(prs_angle_item_s, nr_prs_elevation_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(180),
     nullptr},
    {offsetof(prs_angle_item_s, nr_prs_elevation_fine),
     (int32_t)offsetof(prs_angle_item_s, nr_prs_elevation_fine_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(9),
     nullptr},
    {offsetof(prs_angle_item_s, ie_exts),
     (int32_t)offsetof(prs_angle_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_angle_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_prs_angle_item_s_names[] = {{"nRPRSAzimuth", nullptr},
                                                                           {"nRPRSAzimuthFine", nullptr},
                                                                           {"nRPRSElevation", nullptr},
                                                                           {"nRPRSElevationFine", nullptr},
                                                                           {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_prs_angle_item_s_opts[]  = {
    {1, (int32_t)offsetof(prs_angle_item_s, nr_prs_azimuth_fine_present)},
    {2, (int32_t)offsetof(prs_angle_item_s, nr_prs_elevation_present)},
    {3, (int32_t)offsetof(prs_angle_item_s, nr_prs_elevation_fine_present)},
    {4, (int32_t)offsetof(prs_angle_item_s, ie_exts_present)}};
const asn1::table::seq_desc prs_angle_item_s::desc = {
    {seq_table_prs_angle_item_s_fields, 5, UINT64_C(0x1), seq_table_prs_angle_item_s_opts, 4},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_angle_item_s_names};

// NR-PRS-Beam-InformationItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_prs_beam_info_item_s_fields[] = {
    {offsetof(nr_prs_beam_info_item_s, pr_sres_set_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(nr_prs_beam_info_item_s, prs_angle),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(nr_prs_beam_info_item_s::prs_angle),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &prs_angle_item_s::desc},
    {offsetof(nr_prs_beam_info_item_s, ie_exts),
     (int32_t)offsetof(nr_prs_beam_info_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(nr_prs_beam_info_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_nr_prs_beam_info_item_s_names[] = {{"pRSresourceSetID", nullptr},
                                                                                  {"pRSAngle", nullptr},
                                                                                  {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_nr_prs_beam_info_item_s_opts[]  = {
    {2, (int32_t)offsetof(nr_prs_beam_info_item_s, ie_exts_present)}};
const asn1::table::seq_desc nr_prs_beam_info_item_s::desc = {
    {seq_table_nr_prs_beam_info_item_s_fields, 3, UINT64_C(0x3), seq_table_nr_prs_beam_info_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_nr_prs_beam_info_item_s_names};

// NR-PRS-Beam-Information ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_prs_beam_info_s_fields[] = {
    {offsetof(nr_prs_beam_info_s, nr_prs_beam_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2,
     &asn1::table::item_list_codec<decltype(nr_prs_beam_info_s::nr_prs_beam_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_prs_beam_info_item_s::desc},
    {offsetof(nr_prs_beam_info_s, lcs_to_gcs_translation_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     3,
     &asn1::table::item_list_codec<decltype(nr_prs_beam_info_s::lcs_to_gcs_translation_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &lcs_to_gcs_translation_item_s::desc},
    {offsetof(nr_prs_beam_info_s, ie_exts),
     (int32_t)offsetof(nr_prs_beam_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(nr_prs_beam_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_nr_prs_beam_info_s_names[] = {{"nR-PRS-Beam-InformationList", nullptr},
                                                                             {"lCS-to-GCS-TranslationList", nullptr},
                                                                             {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_nr_prs_beam_info_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(nr_prs_beam_info_s, ie_exts_present)}};
const asn1::table::seq_desc nr_prs_beam_info_s::desc = {
    {seq_table_nr_prs_beam_info_s_fields, 3, UINT64_C(0x1), seq_table_nr_prs_beam_info_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_nr_prs_beam_info_s_names};

// NumberOfAntennaPorts-EUTRA ::= ENUMERATED
static const char*           enum_names_nof_ant_ports_eutra_e_values[] = {"n1-or-n2", "n4"};
const asn1::table::enum_desc nof_ant_ports_eutra_opts::desc            = {enum_names_nof_ant_ports_eutra_e_values,
                                                                          2,
                                                                          "nof_ant_ports_eutra_e"};
uint8_t                      nof_ant_ports_eutra_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 4};
  return map_enum_number(numbers, 2, value, "nof_ant_ports_eutra_e");
}

// NumberOfDlFrames-EUTRA ::= ENUMERATED
static const char*           enum_names_nof_dl_frames_eutra_e_values[] = {"sf1", "sf2", "sf4", "sf6"};
const asn1::table::enum_desc nof_dl_frames_eutra_opts::desc            = {enum_names_nof_dl_frames_eutra_e_values,
                                                                          4,
                                                                          "nof_dl_frames_eutra_e"};
uint8_t                      nof_dl_frames_eutra_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 6};
  return map_enum_number(numbers, 4, value, "nof_dl_frames_eutra_e");
}

// NumberOfFrequencyHoppingBands ::= ENUMERATED
static const char*           enum_names_nof_freq_hop_bands_e_values[] = {"twobands", "fourbands"};
const asn1::table::enum_desc nof_freq_hop_bands_opts::desc            = {enum_names_nof_freq_hop_bands_e_values,
                                                                         2,
                                                                         "nof_freq_hop_bands_e"};
uint8_t                      nof_freq_hop_bands_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "nof_freq_hop_bands_e");
}

// NumberOfTRPRxTEG ::= ENUMERATED
static const char*           enum_names_nof_trp_rx_teg_e_values[] = {"two", "three", "four", "six", "eight"};
const asn1::table::enum_desc nof_trp_rx_teg_opts::desc = {enum_names_nof_trp_rx_teg_e_values, 5, "nof_trp_rx_teg_e"};
uint8_t                      nof_trp_rx_teg_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 4, 6, 8};
  return map_enum_number(numbers, 5, value, "nof_trp_rx_teg_e");
}

// NumberOfTRPRxTxTEG ::= ENUMERATED
static const char*           enum_names_nof_trp_rx_tx_teg_e_values[] = {"two", "three", "four", "six", "eight"};
const asn1::table::enum_desc nof_trp_rx_tx_teg_opts::desc            = {enum_names_nof_trp_rx_tx_teg_e_values,
                                                                        5,
                                                                        "nof_trp_rx_tx_teg_e"};
uint8_t                      nof_trp_rx_tx_teg_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 4, 6, 8};
  return map_enum_number(numbers, 5, value, "nof_trp_rx_tx_teg_e");
}

// OTDOA-Information-Item ::= ENUMERATED
static const char*           enum_names_otdoa_info_item_e_values[] = {"pci",
                                                                      "cGI",
                                                                      "tac",
                                                                      "earfcn",
                                                                      "prsBandwidth",
                                                                      "prsConfigIndex",
                                                                      "cpLength",
                                                                      "noDlFrames",
                                                                      "noAntennaPorts",
                                                                      "sFNInitTime",
                                                                      "nG-RANAccessPointPosition",
                                                                      "prsmutingconfiguration",
                                                                      "prsid",
                                                                      "tpid",
                                                                      "tpType",
                                                                      "crsCPlength",
                                                                      "dlBandwidth",
                                                                      "multipleprsConfigurationsperCell",
                                                                      "prsOccasionGroup",
                                                                      "prsFrequencyHoppingConfiguration",
                                                                      "tddConfig"};
const asn1::table::enum_desc otdoa_info_item_opts::desc            = {enum_names_otdoa_info_item_e_values,
                                                                      21,
                                                                      "otdoa_info_item_e"};

// PRS-Bandwidth-EUTRA ::= ENUMERATED
static const char*           enum_names_prs_bw_eutra_e_values[] = {"bw6", "bw15", "bw25", "bw50", "bw75", "bw100"};
const asn1::table::enum_desc prs_bw_eutra_opts::desc = {enum_names_prs_bw_eutra_e_values, 6, "prs_bw_eutra_e"};
uint8_t                      prs_bw_eutra_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(numbers, 6, value, "prs_bw_eutra_e");
}

// PRSMutingConfiguration-EUTRA ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_prs_muting_cfg_eutra_c_alternatives[] = {
    &asn1::table::alternative_ops<fixed_bitstring<2, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<4, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<8, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<16, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<32, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<64, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<128, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<256, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<512, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<1024, false, true>, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<prs_muting_cfg_eutra_ext_ie_o>, false>::value};
const asn1::table::choice_storage_desc prs_muting_cfg_eutra_c::storage_desc = {
    choice_storage_prs_muting_cfg_eutra_c_alternatives,
    11,
    "prs_muting_cfg_eutra_c"};

fixed_bitstring<2, false, true>& prs_muting_cfg_eutra_c::set_two()
{
  set(types::two);
  return c.get<fixed_bitstring<2, false, true>>();
}
fixed_bitstring<4, false, true>& prs_muting_cfg_eutra_c::set_four()
{
  set(types::four);
  return c.get<fixed_bitstring<4, false, true>>();
}
fixed_bitstring<8, false, true>& prs_muting_cfg_eutra_c::set_eight()
{
  set(types::eight);
  return c.get<fixed_bitstring<8, false, true>>();
}
fixed_bitstring<16, false, true>& prs_muting_cfg_eutra_c::set_sixteen()
{
  set(types::sixteen);
  return c.get<fixed_bitstring<16, false, true>>();
}
fixed_bitstring<32, false, true>& prs_muting_cfg_eutra_c::set_thirty_two()
{
  set(types::thirty_two);
  return c.get<fixed_bitstring<32, false, true>>();
}
fixed_bitstring<64, false, true>& prs_muting_cfg_eutra_c::set_sixty_four()
{
  set(types::sixty_four);
  return c.get<fixed_bitstring<64, false, true>>();
}
fixed_bitstring<128, false, true>& prs_muting_cfg_eutra_c::set_one_hundred_and_twenty_eight()
{
  set(types::one_hundred_and_twenty_eight);
  return c.get<fixed_bitstring<128, false, true>>();
}
fixed_bitstring<256, false, true>& prs_muting_cfg_eutra_c::set_two_hundred_and_fifty_six()
{
  set(types::two_hundred_and_fifty_six);
  return c.get<fixed_bitstring<256, false, true>>();
}
fixed_bitstring<512, false, true>& prs_muting_cfg_eutra_c::set_five_hundred_and_twelve()
{
  set(types::five_hundred_and_twelve);
  return c.get<fixed_bitstring<512, false, true>>();
}
fixed_bitstring<1024, false, true>& prs_muting_cfg_eutra_c::set_one_thousand_and_twenty_four()
{
  set(types::one_thousand_and_twenty_four);
  return c.get<fixed_bitstring<1024, false, true>>();
}
protocol_ie_single_container_s<prs_muting_cfg_eutra_ext_ie_o>& prs_muting_cfg_eutra_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<prs_muting_cfg_eutra_ext_ie_o>>();
}
static const asn1::table::seq_field choice_table_prs_muting_cfg_eutra_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<2, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<4, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<8, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<16, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<32, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<64, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<128, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<256, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<512, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<1024, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<prs_muting_cfg_eutra_ext_ie_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_prs_muting_cfg_eutra_c_names[] = {
    {"two", nullptr},
    {"four", nullptr},
    {"eight", nullptr},
    {"sixteen", nullptr},
    {"thirty-two", nullptr},
    {"sixty-four", nullptr},
    {"one-hundred-and-twenty-eight", nullptr},
    {"two-hundred-and-fifty-six", nullptr},
    {"five-hundred-and-twelve", nullptr},
    {"one-thousand-and-twenty-four", nullptr},
    {"choice-Extension", nullptr}};
const asn1::table::choice_desc prs_muting_cfg_eutra_c::desc = {
    asn1::table::enum_int_type<prs_muting_cfg_eutra_c::types>(),
    prs_muting_cfg_eutra_c::types::nof_types,
    prs_muting_cfg_eutra_c::types::nof_exts,
    prs_muting_cfg_eutra_c::types::has_ext,
    true,
    choice_table_prs_muting_cfg_eutra_c_alternatives,
    nullptr,
    choice_table_prs_muting_cfg_eutra_c_names,
    "prs_muting_cfg_eutra_c",
    offsetof(prs_muting_cfg_eutra_c, c.type),
    offsetof(prs_muting_cfg_eutra_c, c.value),
    false,
    &prs_muting_cfg_eutra_c::storage_desc};

static const char*           enum_names_prs_muting_cfg_eutra_c_types_values[] = {"two",
                                                                                 "four",
                                                                                 "eight",
                                                                                 "sixteen",
                                                                                 "thirty-two",
                                                                                 "sixty-four",
                                                                                 "one-hundred-and-twenty-eight",
                                                                                 "two-hundred-and-fifty-six",
                                                                                 "five-hundred-and-twelve",
                                                                                 "one-thousand-and-twenty-four",
                                                                                 "choice-Extension"};
const asn1::table::enum_desc prs_muting_cfg_eutra_c::types_opts::desc = {enum_names_prs_muting_cfg_eutra_c_types_values,
                                                                         11,
                                                                         "prs_muting_cfg_eutra_c::types"};

// TP-Type-EUTRA ::= ENUMERATED
static const char*           enum_names_tp_type_eutra_e_values[] = {"prs-only-tp"};
const asn1::table::enum_desc tp_type_eutra_opts::desc = {enum_names_tp_type_eutra_e_values, 1, "tp_type_eutra_e"};

// PRSOccasionGroup-EUTRA ::= ENUMERATED
static const char* enum_names_prs_occasion_group_eutra_e_values[] =
    {"og2", "og4", "og8", "og16", "og32", "og64", "og128"};
const asn1::table::enum_desc prs_occasion_group_eutra_opts::desc = {enum_names_prs_occasion_group_eutra_e_values,
                                                                    7,
                                                                    "prs_occasion_group_eutra_e"};
uint8_t                      prs_occasion_group_eutra_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8, 16, 32, 64, 128};
  return map_enum_number(numbers, 7, value, "prs_occasion_group_eutra_e");
}

// PRSFrequencyHoppingConfiguration-EUTRA ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_freq_hop_cfg_eutra_s_fields[] = {
    {offsetof(prs_freq_hop_cfg_eutra_s, no_of_freq_hop_bands),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(prs_freq_hop_cfg_eutra_s::no_of_freq_hop_bands)>(),
     decltype(prs_freq_hop_cfg_eutra_s::no_of_freq_hop_bands)::has_ext,
     false,
     decltype(prs_freq_hop_cfg_eutra_s::no_of_freq_hop_bands)::nof_types,
     decltype(prs_freq_hop_cfg_eutra_s::no_of_freq_hop_bands)::nof_exts,
     nullptr,
     &nof_freq_hop_bands_opts::desc},
    {offsetof(prs_freq_hop_cfg_eutra_s, band_positions),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     7,
     &asn1::table::list_codec<decltype(prs_freq_hop_cfg_eutra_s::band_positions),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(15), true, true>>::value},
    {offsetof(prs_freq_hop_cfg_eutra_s, ie_exts),
     (int32_t)offsetof(prs_freq_hop_cfg_eutra_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prs_freq_hop_cfg_eutra_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_prs_freq_hop_cfg_eutra_s_names[] = {{"noOfFreqHoppingBands", nullptr},
                                                                                   {"bandPositions", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_prs_freq_hop_cfg_eutra_s_opts[]  = {
    {2, (int32_t)offsetof(prs_freq_hop_cfg_eutra_s, ie_exts_present)}};
const asn1::table::seq_desc prs_freq_hop_cfg_eutra_s::desc = {
    {seq_table_prs_freq_hop_cfg_eutra_s_fields, 3, UINT64_C(0x3), seq_table_prs_freq_hop_cfg_eutra_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_freq_hop_cfg_eutra_s_names};

// TDD-Config-EUTRA-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_tdd_cfg_eutra_item_s_fields[] = {
    {offsetof(tdd_cfg_eutra_item_s, sf_assign),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(tdd_cfg_eutra_item_s::sf_assign)>(),
     decltype(tdd_cfg_eutra_item_s::sf_assign)::has_ext,
     false,
     decltype(tdd_cfg_eutra_item_s::sf_assign)::nof_types,
     decltype(tdd_cfg_eutra_item_s::sf_assign)::nof_exts,
     nullptr,
     &tdd_cfg_eutra_item_s::sf_assign_opts::desc},
    {offsetof(tdd_cfg_eutra_item_s, ie_exts),
     (int32_t)offsetof(tdd_cfg_eutra_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tdd_cfg_eutra_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_tdd_cfg_eutra_item_s_names[] = {{"subframeAssignment", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_tdd_cfg_eutra_item_s_opts[]  = {
    {1, (int32_t)offsetof(tdd_cfg_eutra_item_s, ie_exts_present)}};
const asn1::table::seq_desc tdd_cfg_eutra_item_s::desc = {
    {seq_table_tdd_cfg_eutra_item_s_fields, 2, UINT64_C(0x1), seq_table_tdd_cfg_eutra_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_tdd_cfg_eutra_item_s_names};

static const char* enum_names_tdd_cfg_eutra_item_s_sf_assign_e__values[] =
    {"sa0", "sa1", "sa2", "sa3", "sa4", "sa5", "sa6"};
const asn1::table::enum_desc tdd_cfg_eutra_item_s::sf_assign_opts::desc = {
    enum_names_tdd_cfg_eutra_item_s_sf_assign_e__values,
    7,
    "tdd_cfg_eutra_item_s::sf_assign_e_"};
uint8_t tdd_cfg_eutra_item_s::sf_assign_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5, 6};
  return map_enum_number(numbers, 7, value, "tdd_cfg_eutra_item_s::sf_assign_e_");
}

// OTDOACell-Information-Item-ExtensionIE ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_otdoa_cell_info_item_ext_ie_o[]    = {22, 58, 59};
static const uint8_t  objset_crit_otdoa_cell_info_item_ext_ie_o[]  = {crit_e::ignore, crit_e::ignore, crit_e::ignore};
static const uint8_t  objset_value_otdoa_cell_info_item_ext_ie_o[] = {
    otdoa_cell_info_item_ext_ie_o::value_c::types::tdd_cfg_eutra_item,
    otdoa_cell_info_item_ext_ie_o::value_c::types::cgi_nr,
    otdoa_cell_info_item_ext_ie_o::value_c::types::sfn_initisation_time_nr};
static const uint8_t                    objset_presence_otdoa_cell_info_item_ext_ie_o[] = {presence_e::mandatory,
                                                                                           presence_e::mandatory,
                                                                                           presence_e::mandatory};
static const asn1::table::objset_column objset_columns_otdoa_cell_info_item_ext_ie_o[]  = {
    {objset_crit_otdoa_cell_info_item_ext_ie_o, crit_e::nulltype},
    {objset_value_otdoa_cell_info_item_ext_ie_o, otdoa_cell_info_item_ext_ie_o::value_c::types::nulltype},
    {objset_presence_otdoa_cell_info_item_ext_ie_o, presence_e::nulltype}};
const asn1::table::objset_desc otdoa_cell_info_item_ext_ie_o::desc = {objset_id_otdoa_cell_info_item_ext_ie_o,
                                                                      3,
                                                                      "id",
                                                                      objset_columns_otdoa_cell_info_item_ext_ie_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_otdoa_cell_info_item_ext_ie_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<tdd_cfg_eutra_item_s>::value,
     &tdd_cfg_eutra_item_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<cgi_nr_s>::value,
     &cgi_nr_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<fixed_bitstring<64, false, true>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_otdoa_cell_info_item_ext_ie_o_value_c_names[] = {
    {"TDD-Config-EUTRA-Item", nullptr},
    {"CGI-NR", nullptr},
    {"BIT STRING", nullptr}};
const asn1::table::choice_desc otdoa_cell_info_item_ext_ie_o::value_c::desc = {
    asn1::table::enum_int_type<otdoa_cell_info_item_ext_ie_o::value_c::types>(),
    otdoa_cell_info_item_ext_ie_o::value_c::types::nof_types,
    otdoa_cell_info_item_ext_ie_o::value_c::types::nof_exts,
    otdoa_cell_info_item_ext_ie_o::value_c::types::has_ext,
    true,
    choice_table_otdoa_cell_info_item_ext_ie_o_value_c_alternatives,
    &asn1::table::choice_set<otdoa_cell_info_item_ext_ie_o::value_c>,
    choice_table_otdoa_cell_info_item_ext_ie_o_value_c_names,
    "otdoa_cell_info_item_ext_ie_o::value_c",
    offsetof(otdoa_cell_info_item_ext_ie_o::value_c, type_),
    offsetof(otdoa_cell_info_item_ext_ie_o::value_c, c),
    true,
    nullptr,
    true};

void otdoa_cell_info_item_ext_ie_o::value_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, otdoa_cell_info_item_ext_ie_o::value_c::desc);
}
tdd_cfg_eutra_item_s& otdoa_cell_info_item_ext_ie_o::value_c::tdd_cfg_eutra_item()
{
  assert_choice_type(types::tdd_cfg_eutra_item, type_, "Value");
  return c.get<tdd_cfg_eutra_item_s>();
}
cgi_nr_s& otdoa_cell_info_item_ext_ie_o::value_c::cgi_nr()
{
  assert_choice_type(types::cgi_nr, type_, "Value");
  return c.get<cgi_nr_s>();
}
fixed_bitstring<64, false, true>& otdoa_cell_info_item_ext_ie_o::value_c::sfn_initisation_time_nr()
{
  assert_choice_type(types::sfn_initisation_time_nr, type_, "Value");
  return c.get<fixed_bitstring<64, false, true>>();
}
const tdd_cfg_eutra_item_s& otdoa_cell_info_item_ext_ie_o::value_c::tdd_cfg_eutra_item() const
{
  assert_choice_type(types::tdd_cfg_eutra_item, type_, "Value");
  return c.get<tdd_cfg_eutra_item_s>();
}
const cgi_nr_s& otdoa_cell_info_item_ext_ie_o::value_c::cgi_nr() const
{
  assert_choice_type(types::cgi_nr, type_, "Value");
  return c.get<cgi_nr_s>();
}
const fixed_bitstring<64, false, true>& otdoa_cell_info_item_ext_ie_o::value_c::sfn_initisation_time_nr() const
{
  assert_choice_type(types::sfn_initisation_time_nr, type_, "Value");
  return c.get<fixed_bitstring<64, false, true>>();
}

static const char*           enum_names_otdoa_cell_info_item_ext_ie_o_value_c_types_values[] = {"TDD-Config-EUTRA-Item",
                                                                                                "CGI-NR",
                                                                                                "BIT STRING"};
const asn1::table::enum_desc otdoa_cell_info_item_ext_ie_o::value_c::types_opts::desc        = {
    enum_names_otdoa_cell_info_item_ext_ie_o_value_c_types_values,
    3,
    "otdoa_cell_info_item_ext_ie_o::value_c::types"};

// OTDOACell-Information-Item ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_otdoa_cell_info_item_c_alternatives[] = {
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<cgi_eutra_s, false>::value,
    &asn1::table::alternative_ops<fixed_octstring<3, true>, false>::value,
    &asn1::table::alternative_ops<uint32_t, true>::value,
    &asn1::table::alternative_ops<prs_bw_eutra_e, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<cp_len_eutra_e, true>::value,
    &asn1::table::alternative_ops<nof_dl_frames_eutra_e, true>::value,
    &asn1::table::alternative_ops<nof_ant_ports_eutra_e, true>::value,
    &asn1::table::alternative_ops<fixed_bitstring<64, false, true>, false>::value,
    &asn1::table::alternative_ops<ng_ran_access_point_position_s, false>::value,
    &asn1::table::alternative_ops<prs_muting_cfg_eutra_c, false>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<tp_type_eutra_e, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<cp_len_eutra_e, true>::value,
    &asn1::table::alternative_ops<dl_bw_eutra_e, true>::value,
    &asn1::table::alternative_ops<prs_occasion_group_eutra_e, true>::value,
    &asn1::table::alternative_ops<prs_freq_hop_cfg_eutra_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<otdoa_cell_info_item_ext_ie_o>, false>::value};
const asn1::table::choice_storage_desc otdoa_cell_info_item_c::storage_desc = {
    choice_storage_otdoa_cell_info_item_c_alternatives,
    21,
    "otdoa_cell_info_item_c"};

uint16_t& otdoa_cell_info_item_c::set_pci_eutra()
{
  set(types::pci_eutra);
  return c.get<uint16_t>();
}
cgi_eutra_s& otdoa_cell_info_item_c::set_cgi_eutra()
{
  set(types::cgi_eutra);
  return c.get<cgi_eutra_s>();
}
fixed_octstring<3, true>& otdoa_cell_info_item_c::set_tac()
{
  set(types::tac);
  return c.get<fixed_octstring<3, true>>();
}
uint32_t& otdoa_cell_info_item_c::set_earfcn()
{
  set(types::earfcn);
  return c.get<uint32_t>();
}
prs_bw_eutra_e& otdoa_cell_info_item_c::set_prs_bw_eutra()
{
  set(types::prs_bw_eutra);
  return c.get<prs_bw_eutra_e>();
}
uint16_t& otdoa_cell_info_item_c::set_prs_cfg_idx_eutra()
{
  set(types::prs_cfg_idx_eutra);
  return c.get<uint16_t>();
}
cp_len_eutra_e& otdoa_cell_info_item_c::set_cp_len_eutra()
{
  set(types::cp_len_eutra);
  return c.get<cp_len_eutra_e>();
}
nof_dl_frames_eutra_e& otdoa_cell_info_item_c::set_nof_dl_frames_eutra()
{
  set(types::nof_dl_frames_eutra);
  return c.get<nof_dl_frames_eutra_e>();
}
nof_ant_ports_eutra_e& otdoa_cell_info_item_c::set_nof_ant_ports_eutra()
{
  set(types::nof_ant_ports_eutra);
  return c.get<nof_ant_ports_eutra_e>();
}
fixed_bitstring<64, false, true>& otdoa_cell_info_item_c::set_sfn_initisation_time_eutra()
{
  set(types::sfn_initisation_time_eutra);
  return c.get<fixed_bitstring<64, false, true>>();
}
ng_ran_access_point_position_s& otdoa_cell_info_item_c::set_ng_ran_access_point_position()
{
  set(types::ng_ran_access_point_position);
  return c.get<ng_ran_access_point_position_s>();
}
prs_muting_cfg_eutra_c& otdoa_cell_info_item_c::set_prs_muting_cfg_eutra()
{
  set(types::prs_muting_cfg_eutra);
  return c.get<prs_muting_cfg_eutra_c>();
}
uint16_t& otdoa_cell_info_item_c::set_prsid_eutra()
{
  set(types::prsid_eutra);
  return c.get<uint16_t>();
}
uint16_t& otdoa_cell_info_item_c::set_tpid_eutra()
{
  set(types::tpid_eutra);
  return c.get<uint16_t>();
}
tp_type_eutra_e& otdoa_cell_info_item_c::set_tp_type_eutra()
{
  set(types::tp_type_eutra);
  return c.get<tp_type_eutra_e>();
}
uint16_t& otdoa_cell_info_item_c::set_nof_dl_frames_extended_eutra()
{
  set(types::nof_dl_frames_extended_eutra);
  return c.get<uint16_t>();
}
cp_len_eutra_e& otdoa_cell_info_item_c::set_crs_cp_len_eutra()
{
  set(types::crs_cp_len_eutra);
  return c.get<cp_len_eutra_e>();
}
dl_bw_eutra_e& otdoa_cell_info_item_c::set_dl_bw_eutra()
{
  set(types::dl_bw_eutra);
  return c.get<dl_bw_eutra_e>();
}
prs_occasion_group_eutra_e& otdoa_cell_info_item_c::set_prs_occasion_group_eutra()
{
  set(types::prs_occasion_group_eutra);
  return c.get<prs_occasion_group_eutra_e>();
}
prs_freq_hop_cfg_eutra_s& otdoa_cell_info_item_c::set_prs_freq_hop_cfg_eutra()
{
  set(types::prs_freq_hop_cfg_eutra);
  return c.get<prs_freq_hop_cfg_eutra_s>();
}
protocol_ie_single_container_s<otdoa_cell_info_item_ext_ie_o>& otdoa_cell_info_item_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<otdoa_cell_info_item_ext_ie_o>>();
}
static const asn1::table::seq_field choice_table_otdoa_cell_info_item_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(503),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_eutra_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_octstring<3, true>, asn1::table::string_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(262143),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<prs_bw_eutra_e>(),
     prs_bw_eutra_e::has_ext,
     false,
     prs_bw_eutra_e::nof_types,
     prs_bw_eutra_e::nof_exts,
     nullptr,
     &prs_bw_eutra_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(4095),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<cp_len_eutra_e>(),
     cp_len_eutra_e::has_ext,
     false,
     cp_len_eutra_e::nof_types,
     cp_len_eutra_e::nof_exts,
     nullptr,
     &cp_len_eutra_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<nof_dl_frames_eutra_e>(),
     nof_dl_frames_eutra_e::has_ext,
     false,
     nof_dl_frames_eutra_e::nof_types,
     nof_dl_frames_eutra_e::nof_exts,
     nullptr,
     &nof_dl_frames_eutra_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<nof_ant_ports_eutra_e>(),
     nof_ant_ports_eutra_e::has_ext,
     false,
     nof_ant_ports_eutra_e::nof_types,
     nof_ant_ports_eutra_e::nof_exts,
     nullptr,
     &nof_ant_ports_eutra_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<64, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ng_ran_access_point_position_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_muting_cfg_eutra_c::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(4095),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(4095),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<tp_type_eutra_e>(),
     tp_type_eutra_e::has_ext,
     false,
     tp_type_eutra_e::nof_types,
     tp_type_eutra_e::nof_exts,
     nullptr,
     &tp_type_eutra_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(160),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<cp_len_eutra_e>(),
     cp_len_eutra_e::has_ext,
     false,
     cp_len_eutra_e::nof_types,
     cp_len_eutra_e::nof_exts,
     nullptr,
     &cp_len_eutra_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<dl_bw_eutra_e>(),
     dl_bw_eutra_e::has_ext,
     false,
     dl_bw_eutra_e::nof_types,
     dl_bw_eutra_e::nof_exts,
     nullptr,
     &dl_bw_eutra_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<prs_occasion_group_eutra_e>(),
     prs_occasion_group_eutra_e::has_ext,
     false,
     prs_occasion_group_eutra_e::nof_types,
     prs_occasion_group_eutra_e::nof_exts,
     nullptr,
     &prs_occasion_group_eutra_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_freq_hop_cfg_eutra_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<otdoa_cell_info_item_ext_ie_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_otdoa_cell_info_item_c_names[] = {
    {"pCI-EUTRA", nullptr},
    {"cGI-EUTRA", nullptr},
    {"tAC", nullptr},
    {"eARFCN", nullptr},
    {"pRS-Bandwidth-EUTRA", nullptr},
    {"pRS-ConfigurationIndex-EUTRA", nullptr},
    {"cPLength-EUTRA", nullptr},
    {"numberOfDlFrames-EUTRA", nullptr},
    {"numberOfAntennaPorts-EUTRA", nullptr},
    {"sFNInitialisationTime-EUTRA", nullptr},
    {"nG-RANAccessPointPosition", nullptr},
    {"pRSMutingConfiguration-EUTRA", nullptr},
    {"prsid-EUTRA", nullptr},
    {"tpid-EUTRA", nullptr},
    {"tpType-EUTRA", nullptr},
    {"numberOfDlFrames-Extended-EUTRA", nullptr},
    {"crsCPlength-EUTRA", nullptr},
    {"dL-Bandwidth-EUTRA", nullptr},
    {"pRSOccasionGroup-EUTRA", nullptr},
    {"pRSFrequencyHoppingConfiguration-EUTRA", nullptr},
    {"choice-Extension", nullptr}};
const asn1::table::choice_desc otdoa_cell_info_item_c::desc = {
    asn1::table::enum_int_type<otdoa_cell_info_item_c::types>(),
    otdoa_cell_info_item_c::types::nof_types,
    otdoa_cell_info_item_c::types::nof_exts,
    otdoa_cell_info_item_c::types::has_ext,
    true,
    choice_table_otdoa_cell_info_item_c_alternatives,
    nullptr,
    choice_table_otdoa_cell_info_item_c_names,
    "otdoa_cell_info_item_c",
    offsetof(otdoa_cell_info_item_c, c.type),
    offsetof(otdoa_cell_info_item_c, c.value),
    false,
    &otdoa_cell_info_item_c::storage_desc};

static const char*           enum_names_otdoa_cell_info_item_c_types_values[] = {"pCI-EUTRA",
                                                                                 "cGI-EUTRA",
                                                                                 "tAC",
                                                                                 "eARFCN",
                                                                                 "pRS-Bandwidth-EUTRA",
                                                                                 "pRS-ConfigurationIndex-EUTRA",
                                                                                 "cPLength-EUTRA",
                                                                                 "numberOfDlFrames-EUTRA",
                                                                                 "numberOfAntennaPorts-EUTRA",
                                                                                 "sFNInitialisationTime-EUTRA",
                                                                                 "nG-RANAccessPointPosition",
                                                                                 "pRSMutingConfiguration-EUTRA",
                                                                                 "prsid-EUTRA",
                                                                                 "tpid-EUTRA",
                                                                                 "tpType-EUTRA",
                                                                                 "numberOfDlFrames-Extended-EUTRA",
                                                                                 "crsCPlength-EUTRA",
                                                                                 "dL-Bandwidth-EUTRA",
                                                                                 "pRSOccasionGroup-EUTRA",
                                                                                 "pRSFrequencyHoppingConfiguration-EUTRA",
                                                                                 "choice-Extension"};
const asn1::table::enum_desc otdoa_cell_info_item_c::types_opts::desc = {enum_names_otdoa_cell_info_item_c_types_values,
                                                                         21,
                                                                         "otdoa_cell_info_item_c::types"};

static const asn1::table::seq_field seq_table_otdoa_cells_item_s__fields[] = {
    {offsetof(otdoa_cells_item_s_, otdoa_cell_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(otdoa_cells_item_s_::otdoa_cell_info),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &otdoa_cell_info_item_c::desc},
    {offsetof(otdoa_cells_item_s_, ie_exts),
     (int32_t)offsetof(otdoa_cells_item_s_, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(otdoa_cells_item_s_::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_otdoa_cells_item_s__names[] = {{"oTDOACellInfo", nullptr},
                                                                              {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_otdoa_cells_item_s__opts[]  = {
    {1, (int32_t)offsetof(otdoa_cells_item_s_, ie_exts_present)}};
const asn1::table::seq_desc otdoa_cells_item_s_::desc = {
    {seq_table_otdoa_cells_item_s__fields, 2, UINT64_C(0x1), seq_table_otdoa_cells_item_s__opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_otdoa_cells_item_s__names};

// OnDemandPRS-Info ::= SEQUENCE
static const asn1::table::seq_field seq_table_on_demand_prs_info_s_fields[] = {
    {offsetof(on_demand_prs_info_s, on_demand_prs_request_allowed),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(on_demand_prs_info_s::on_demand_prs_request_allowed),
                                asn1::table::struct_json>::value},
    {offsetof(on_demand_prs_info_s, allowed_res_set_periodicity_values),
     (int32_t)offsetof(on_demand_prs_info_s, allowed_res_set_periodicity_values_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(on_demand_prs_info_s::allowed_res_set_periodicity_values),
                                asn1::table::struct_json>::value},
    {offsetof(on_demand_prs_info_s, allowed_prs_bw_values),
     (int32_t)offsetof(on_demand_prs_info_s, allowed_prs_bw_values_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(on_demand_prs_info_s::allowed_prs_bw_values),
                                asn1::table::struct_json>::value},
    {offsetof(on_demand_prs_info_s, allowed_res_repeat_factor_values),
     (int32_t)offsetof(on_demand_prs_info_s, allowed_res_repeat_factor_values_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(on_demand_prs_info_s::allowed_res_repeat_factor_values),
                                asn1::table::struct_json>::value},
    {offsetof(on_demand_prs_info_s, allowed_res_nof_symbols_values),
     (int32_t)offsetof(on_demand_prs_info_s, allowed_res_nof_symbols_values_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(on_demand_prs_info_s::allowed_res_nof_symbols_values),
                                asn1::table::struct_json>::value},
    {offsetof(on_demand_prs_info_s, allowed_comb_size_values),
     (int32_t)offsetof(on_demand_prs_info_s, allowed_comb_size_values_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(on_demand_prs_info_s::allowed_comb_size_values),
                                asn1::table::struct_json>::value},
    {offsetof(on_demand_prs_info_s, ie_exts),
     (int32_t)offsetof(on_demand_prs_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(on_demand_prs_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_on_demand_prs_info_s_names[] = {
    {"onDemandPRSRequestAllowed", nullptr},
    {"allowedResourceSetPeriodicityValues", nullptr},
    {"allowedPRSBandwidthValues", nullptr},
    {"allowedResourceRepetitionFactorValues", nullptr},
    {"allowedResourceNumberOfSymbolsValues", nullptr},
    {"allowedCombSizeValues", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_on_demand_prs_info_s_opts[] = {
    {1, (int32_t)offsetof(on_demand_prs_info_s, allowed_res_set_periodicity_values_present)},
    {2, (int32_t)offsetof(on_demand_prs_info_s, allowed_prs_bw_values_present)},
    {3, (int32_t)offsetof(on_demand_prs_info_s, allowed_res_repeat_factor_values_present)},
    {4, (int32_t)offsetof(on_demand_prs_info_s, allowed_res_nof_symbols_values_present)},
    {5, (int32_t)offsetof(on_demand_prs_info_s, allowed_comb_size_values_present)},
    {6, (int32_t)offsetof(on_demand_prs_info_s, ie_exts_present)}};
const asn1::table::seq_desc on_demand_prs_info_s::desc = {
    {seq_table_on_demand_prs_info_s_fields, 7, UINT64_C(0x1), seq_table_on_demand_prs_info_s_opts, 6},
    true,
    true,
    nullptr,
    0,
    seq_table_on_demand_prs_info_s_names};

// ResultGERAN-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_result_geran_item_s_fields[] = {
    {offsetof(result_geran_item_s, bcch),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {offsetof(result_geran_item_s, pci_geran),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(result_geran_item_s, rssi),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(result_geran_item_s, ie_exts),
     (int32_t)offsetof(result_geran_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(result_geran_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_result_geran_item_s_names[] = {{"bCCH", nullptr},
                                                                              {"physCellIDGERAN", nullptr},
                                                                              {"rSSI", nullptr},
                                                                              {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_result_geran_item_s_opts[]  = {
    {3, (int32_t)offsetof(result_geran_item_s, ie_exts_present)}};
const asn1::table::seq_desc result_geran_item_s::desc = {
    {seq_table_result_geran_item_s_fields, 4, UINT64_C(0x7), seq_table_result_geran_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_result_geran_item_s_names};

// ResultUTRAN-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_result_utran_item_s_fields[] = {
    {offsetof(result_utran_item_s, uarfcn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(16383),
     nullptr},
    {offsetof(result_utran_item_s, pci_utran),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &result_utran_item_s::pci_utran_c_::desc},
    {offsetof(result_utran_item_s, utra_rs_cp),
     (int32_t)offsetof(result_utran_item_s, utra_rs_cp_present),
     asn1::table::field_op::int_i16,
     true,
     true,
     INT64_C(-5),
     UINT64_C(91),
     nullptr},
    {offsetof(result_utran_item_s, utra_ec_n0),
     (int32_t)offsetof(result_utran_item_s, utra_ec_n0_present),
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(0),
     UINT64_C(49),
     nullptr},
    {offsetof(result_utran_item_s, ie_exts),
     (int32_t)offsetof(result_utran_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(result_utran_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_result_utran_item_s_names[] = {{"uARFCN", nullptr},
                                                                              {"physCellIDUTRAN", nullptr},
                                                                              {"uTRA-RSCP", nullptr},
                                                                              {"uTRA-EcN0", nullptr},
                                                                              {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_result_utran_item_s_opts[]  = {
    {2, (int32_t)offsetof(result_utran_item_s, utra_rs_cp_present)},
    {3, (int32_t)offsetof(result_utran_item_s, utra_ec_n0_present)},
    {4, (int32_t)offsetof(result_utran_item_s, ie_exts_present)}};
const asn1::table::seq_desc result_utran_item_s::desc = {
    {seq_table_result_utran_item_s_fields, 5, UINT64_C(0x3), seq_table_result_utran_item_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_result_utran_item_s_names};

static const asn1::table::storage_ops* const choice_storage_result_utran_item_s_pci_utran_c__alternatives[] = {
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value};
const asn1::table::choice_storage_desc result_utran_item_s::pci_utran_c_::storage_desc = {
    choice_storage_result_utran_item_s_pci_utran_c__alternatives,
    2,
    "result_utran_item_s::pci_utran_c_"};

uint16_t& result_utran_item_s::pci_utran_c_::set_phys_cell_i_du_tra_fdd()
{
  set(types::phys_cell_i_du_tra_fdd);
  return c.get<uint16_t>();
}
uint16_t& result_utran_item_s::pci_utran_c_::set_phys_cell_i_du_tra_tdd()
{
  set(types::phys_cell_i_du_tra_tdd);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field choice_table_result_utran_item_s_pci_utran_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(511),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr}};
static const asn1::table::field_name choice_table_result_utran_item_s_pci_utran_c__names[] = {
    {"physCellIDUTRA-FDD", nullptr},
    {"physCellIDUTRA-TDD", nullptr}};
const asn1::table::choice_desc result_utran_item_s::pci_utran_c_::desc = {
    asn1::table::enum_int_type<result_utran_item_s::pci_utran_c_::types>(),
    result_utran_item_s::pci_utran_c_::types::nof_types,
    result_utran_item_s::pci_utran_c_::types::nof_exts,
    result_utran_item_s::pci_utran_c_::types::has_ext,
    true,
    choice_table_result_utran_item_s_pci_utran_c__alternatives,
    nullptr,
    choice_table_result_utran_item_s_pci_utran_c__names,
    "result_utran_item_s::pci_utran_c_",
    offsetof(result_utran_item_s::pci_utran_c_, c.type),
    offsetof(result_utran_item_s::pci_utran_c_, c.value),
    false,
    &result_utran_item_s::pci_utran_c_::storage_desc};

static const char*           enum_names_result_utran_item_s_pci_utran_c__types_values[] = {"physCellIDUTRA-FDD",
                                                                                           "physCellIDUTRA-TDD"};
const asn1::table::enum_desc result_utran_item_s::pci_utran_c_::types_opts::desc        = {
    enum_names_result_utran_item_s_pci_utran_c__types_values,
    2,
    "result_utran_item_s::pci_utran_c_::types"};

// ResultNR-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_result_nr_item_s_fields[] = {
    {offsetof(result_nr_item_s, nr_pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(result_nr_item_s, nr_arfcn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(result_nr_item_s, value_ss_rsrp_cell),
     (int32_t)offsetof(result_nr_item_s, value_ss_rsrp_cell_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(result_nr_item_s, value_ss_rsrq_cell),
     (int32_t)offsetof(result_nr_item_s, value_ss_rsrq_cell_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(result_nr_item_s, ss_rsrp_per_ssb),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(result_nr_item_s::ss_rsrp_per_ssb),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &result_ss_rsrp_per_ssb_item_s::desc},
    {offsetof(result_nr_item_s, ss_rsrq_per_ssb),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(result_nr_item_s::ss_rsrq_per_ssb),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &result_ss_rsrq_per_ssb_item_s::desc},
    {offsetof(result_nr_item_s, cgi_nr),
     (int32_t)offsetof(result_nr_item_s, cgi_nr_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_nr_s::desc},
    {offsetof(result_nr_item_s, ie_exts),
     (int32_t)offsetof(result_nr_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(result_nr_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_result_nr_item_s_names[] = {{"nR-PCI", nullptr},
                                                                           {"nR-ARFCN", nullptr},
                                                                           {"valueSS-RSRP-Cell", nullptr},
                                                                           {"valueSS-RSRQ-Cell", nullptr},
                                                                           {"sS-RSRP-PerSSB", nullptr},
                                                                           {"sS-RSRQ-PerSSB", nullptr},
                                                                           {"cGI-NR", nullptr},
                                                                           {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_result_nr_item_s_opts[]  = {
    {2, (int32_t)offsetof(result_nr_item_s, value_ss_rsrp_cell_present)},
    {3, (int32_t)offsetof(result_nr_item_s, value_ss_rsrq_cell_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence},
    {6, (int32_t)offsetof(result_nr_item_s, cgi_nr_present)},
    {7, (int32_t)offsetof(result_nr_item_s, ie_exts_present)}};
const asn1::table::seq_desc result_nr_item_s::desc = {
    {seq_table_result_nr_item_s_fields, 8, UINT64_C(0x3), seq_table_result_nr_item_s_opts, 6},
    true,
    true,
    nullptr,
    0,
    seq_table_result_nr_item_s_names};

// ResultEUTRA-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_result_eutra_item_s_fields[] = {
    {offsetof(result_eutra_item_s, pci_eutra),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(503),
     nullptr},
    {offsetof(result_eutra_item_s, earfcn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(262143),
     nullptr},
    {offsetof(result_eutra_item_s, value_rsrp_eutra),
     (int32_t)offsetof(result_eutra_item_s, value_rsrp_eutra_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(97),
     nullptr},
    {offsetof(result_eutra_item_s, value_rsrq_eutra),
     (int32_t)offsetof(result_eutra_item_s, value_rsrq_eutra_present),
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(0),
     UINT64_C(34),
     nullptr},
    {offsetof(result_eutra_item_s, cgi_eutra),
     (int32_t)offsetof(result_eutra_item_s, cgi_eutra_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_eutra_s::desc},
    {offsetof(result_eutra_item_s, ie_exts),
     (int32_t)offsetof(result_eutra_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(result_eutra_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_result_eutra_item_s_names[] = {{"pCI-EUTRA", nullptr},
                                                                              {"eARFCN", nullptr},
                                                                              {"valueRSRP-EUTRA", nullptr},
                                                                              {"valueRSRQ-EUTRA", nullptr},
                                                                              {"cGI-EUTRA", nullptr},
                                                                              {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_result_eutra_item_s_opts[]  = {
    {2, (int32_t)offsetof(result_eutra_item_s, value_rsrp_eutra_present)},
    {3, (int32_t)offsetof(result_eutra_item_s, value_rsrq_eutra_present)},
    {4, (int32_t)offsetof(result_eutra_item_s, cgi_eutra_present)},
    {5, (int32_t)offsetof(result_eutra_item_s, ie_exts_present)}};
const asn1::table::seq_desc result_eutra_item_s::desc = {
    {seq_table_result_eutra_item_s_fields, 6, UINT64_C(0x3), seq_table_result_eutra_item_s_opts, 4},
    true,
    true,
    nullptr,
    0,
    seq_table_result_eutra_item_s_names};

// OtherRATMeasuredResultsValue-ExtensionIE ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_other_rat_measured_results_value_ext_ie_o[]    = {55, 56};
static const uint8_t  objset_crit_other_rat_measured_results_value_ext_ie_o[]  = {crit_e::ignore, crit_e::ignore};
static const uint8_t  objset_value_other_rat_measured_results_value_ext_ie_o[] = {
    other_rat_measured_results_value_ext_ie_o::value_c::types::result_nr,
    other_rat_measured_results_value_ext_ie_o::value_c::types::result_eutra};
static const uint8_t objset_presence_other_rat_measured_results_value_ext_ie_o[] = {presence_e::mandatory,
                                                                                    presence_e::mandatory};
static const asn1::table::objset_column objset_columns_other_rat_measured_results_value_ext_ie_o[] = {
    {objset_crit_other_rat_measured_results_value_ext_ie_o, crit_e::nulltype},
    {objset_value_other_rat_measured_results_value_ext_ie_o,
     other_rat_measured_results_value_ext_ie_o::value_c::types::nulltype},
    {objset_presence_other_rat_measured_results_value_ext_ie_o, presence_e::nulltype}};
const asn1::table::objset_desc other_rat_measured_results_value_ext_ie_o::desc = {
    objset_id_other_rat_measured_results_value_ext_ie_o,
    2,
    "id",
    objset_columns_other_rat_measured_results_value_ext_ie_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_other_rat_measured_results_value_ext_ie_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::any_list_codec<result_nr_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::any_list_codec<result_eutra_l, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_other_rat_measured_results_value_ext_ie_o_value_c_names[] = {
    {"ResultNR", nullptr},
    {"ResultEUTRA", nullptr}};
const asn1::table::choice_desc other_rat_measured_results_value_ext_ie_o::value_c::desc = {
    asn1::table::enum_int_type<other_rat_measured_results_value_ext_ie_o::value_c::types>(),
    other_rat_measured_results_value_ext_ie_o::value_c::types::nof_types,
    other_rat_measured_results_value_ext_ie_o::value_c::types::nof_exts,
    other_rat_measured_results_value_ext_ie_o::value_c::types::has_ext,
    true,
    choice_table_other_rat_measured_results_value_ext_ie_o_value_c_alternatives,
    &asn1::table::choice_set<other_rat_measured_results_value_ext_ie_o::value_c>,
    choice_table_other_rat_measured_results_value_ext_ie_o_value_c_names,
    "other_rat_measured_results_value_ext_ie_o::value_c",
    offsetof(other_rat_measured_results_value_ext_ie_o::value_c, type_),
    offsetof(other_rat_measured_results_value_ext_ie_o::value_c, c),
    true,
    nullptr,
    true};

void other_rat_measured_results_value_ext_ie_o::value_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, other_rat_measured_results_value_ext_ie_o::value_c::desc);
}
result_nr_l& other_rat_measured_results_value_ext_ie_o::value_c::result_nr()
{
  assert_choice_type(types::result_nr, type_, "Value");
  return c.get<result_nr_l>();
}
result_eutra_l& other_rat_measured_results_value_ext_ie_o::value_c::result_eutra()
{
  assert_choice_type(types::result_eutra, type_, "Value");
  return c.get<result_eutra_l>();
}
const result_nr_l& other_rat_measured_results_value_ext_ie_o::value_c::result_nr() const
{
  assert_choice_type(types::result_nr, type_, "Value");
  return c.get<result_nr_l>();
}
const result_eutra_l& other_rat_measured_results_value_ext_ie_o::value_c::result_eutra() const
{
  assert_choice_type(types::result_eutra, type_, "Value");
  return c.get<result_eutra_l>();
}

static const char*           enum_names_other_rat_measured_results_value_ext_ie_o_value_c_types_values[] = {"ResultNR",
                                                                                                            "ResultEUTRA"};
const asn1::table::enum_desc other_rat_measured_results_value_ext_ie_o::value_c::types_opts::desc        = {
    enum_names_other_rat_measured_results_value_ext_ie_o_value_c_types_values,
    2,
    "other_rat_measured_results_value_ext_ie_o::value_c::types"};

// OtherRATMeasuredResultsValue ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_other_rat_measured_results_value_c_alternatives[] = {
    &asn1::table::alternative_ops<result_geran_l, false>::value,
    &asn1::table::alternative_ops<result_utran_l, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<other_rat_measured_results_value_ext_ie_o>,
                                  false>::value};
const asn1::table::choice_storage_desc other_rat_measured_results_value_c::storage_desc = {
    choice_storage_other_rat_measured_results_value_c_alternatives,
    3,
    "other_rat_measured_results_value_c"};

result_geran_l& other_rat_measured_results_value_c::set_result_geran()
{
  set(types::result_geran);
  return c.get<result_geran_l>();
}
result_utran_l& other_rat_measured_results_value_c::set_result_utran()
{
  set(types::result_utran);
  return c.get<result_utran_l>();
}
protocol_ie_single_container_s<other_rat_measured_results_value_ext_ie_o>&
other_rat_measured_results_value_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<other_rat_measured_results_value_ext_ie_o>>();
}
static const asn1::table::seq_field choice_table_other_rat_measured_results_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::list_codec<result_geran_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::list_codec<result_utran_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<other_rat_measured_results_value_ext_ie_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_other_rat_measured_results_value_c_names[] = {
    {"resultGERAN", nullptr},
    {"resultUTRAN", nullptr},
    {"choice-Extension", nullptr}};
const asn1::table::choice_desc other_rat_measured_results_value_c::desc = {
    asn1::table::enum_int_type<other_rat_measured_results_value_c::types>(),
    other_rat_measured_results_value_c::types::nof_types,
    other_rat_measured_results_value_c::types::nof_exts,
    other_rat_measured_results_value_c::types::has_ext,
    true,
    choice_table_other_rat_measured_results_value_c_alternatives,
    nullptr,
    choice_table_other_rat_measured_results_value_c_names,
    "other_rat_measured_results_value_c",
    offsetof(other_rat_measured_results_value_c, c.type),
    offsetof(other_rat_measured_results_value_c, c.value),
    false,
    &other_rat_measured_results_value_c::storage_desc};

static const char*           enum_names_other_rat_measured_results_value_c_types_values[] = {"resultGERAN",
                                                                                             "resultUTRAN",
                                                                                             "choice-Extension"};
const asn1::table::enum_desc other_rat_measured_results_value_c::types_opts::desc         = {
    enum_names_other_rat_measured_results_value_c_types_values,
    3,
    "other_rat_measured_results_value_c::types"};

// OtherRATMeasurementQuantitiesValue ::= ENUMERATED
static const char*           enum_names_other_rat_meas_quantities_value_e_values[] = {"geran", "utran", "nR", "eUTRA"};
const asn1::table::enum_desc other_rat_meas_quantities_value_opts::desc            = {
    enum_names_other_rat_meas_quantities_value_e_values,
    4,
    "other_rat_meas_quantities_value_e"};

// OtherRATMeasurementQuantities-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_other_rat_meas_quantities_item_s_fields[] = {
    {offsetof(other_rat_meas_quantities_item_s, other_rat_meas_quantities_value),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(other_rat_meas_quantities_item_s::other_rat_meas_quantities_value)>(),
     decltype(other_rat_meas_quantities_item_s::other_rat_meas_quantities_value)::has_ext,
     false,
     decltype(other_rat_meas_quantities_item_s::other_rat_meas_quantities_value)::nof_types,
     decltype(other_rat_meas_quantities_item_s::other_rat_meas_quantities_value)::nof_exts,
     nullptr,
     &other_rat_meas_quantities_value_opts::desc},
    {offsetof(other_rat_meas_quantities_item_s, ie_exts),
     (int32_t)offsetof(other_rat_meas_quantities_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(other_rat_meas_quantities_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_other_rat_meas_quantities_item_s_names[] = {
    {"otherRATMeasurementQuantitiesValue", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_other_rat_meas_quantities_item_s_opts[] = {
    {1, (int32_t)offsetof(other_rat_meas_quantities_item_s, ie_exts_present)}};
const asn1::table::seq_desc other_rat_meas_quantities_item_s::desc = {
    {seq_table_other_rat_meas_quantities_item_s_fields,
     2,
     UINT64_C(0x1),
     seq_table_other_rat_meas_quantities_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_other_rat_meas_quantities_item_s_names};

// OtherRATMeasurementQuantities-ItemIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_other_rat_meas_quantities_item_ies_o[]    = {16};
static const uint8_t  objset_crit_other_rat_meas_quantities_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_other_rat_meas_quantities_item_ies_o[] = {
    other_rat_meas_quantities_item_ies_o::value_c::types::other_rat_meas_quantities_item};
static const uint8_t objset_presence_other_rat_meas_quantities_item_ies_o[]                   = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_other_rat_meas_quantities_item_ies_o[] = {
    {objset_crit_other_rat_meas_quantities_item_ies_o, crit_e::nulltype},
    {objset_value_other_rat_meas_quantities_item_ies_o, other_rat_meas_quantities_item_ies_o::value_c::types::nulltype},
    {objset_presence_other_rat_meas_quantities_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc other_rat_meas_quantities_item_ies_o::desc =
    {objset_id_other_rat_meas_quantities_item_ies_o, 1, "id", objset_columns_other_rat_meas_quantities_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_other_rat_meas_quantities_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &other_rat_meas_quantities_item_s::desc}};
static const asn1::table::field_name choice_table_other_rat_meas_quantities_item_ies_o_value_c_names[] = {
    {"OtherRATMeasurementQuantities-Item", nullptr}};
const asn1::table::choice_desc other_rat_meas_quantities_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<other_rat_meas_quantities_item_ies_o::value_c::types>(),
    other_rat_meas_quantities_item_ies_o::value_c::types::nof_types,
    other_rat_meas_quantities_item_ies_o::value_c::types::nof_exts,
    other_rat_meas_quantities_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_other_rat_meas_quantities_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_other_rat_meas_quantities_item_ies_o_value_c_names,
    "other_rat_meas_quantities_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(other_rat_meas_quantities_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_other_rat_meas_quantities_item_ies_o_value_c_types_values[] = {
    "OtherRATMeasurementQuantities-Item"};
const asn1::table::enum_desc other_rat_meas_quantities_item_ies_o::value_c::types_opts::desc = {
    enum_names_other_rat_meas_quantities_item_ies_o_value_c_types_values,
    1,
    "other_rat_meas_quantities_item_ies_o::value_c::types"};

// PRS-Measurements-Info-List-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_meass_info_list_item_s_fields[] = {
    {offsetof(prs_meass_info_list_item_s, point_a),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(prs_meass_info_list_item_s, meas_prs_periodicity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(prs_meass_info_list_item_s::meas_prs_periodicity)>(),
     decltype(prs_meass_info_list_item_s::meas_prs_periodicity)::has_ext,
     false,
     decltype(prs_meass_info_list_item_s::meas_prs_periodicity)::nof_types,
     decltype(prs_meass_info_list_item_s::meas_prs_periodicity)::nof_exts,
     nullptr,
     &prs_meass_info_list_item_s::meas_prs_periodicity_opts::desc},
    {offsetof(prs_meass_info_list_item_s, meas_prs_offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(159),
     nullptr},
    {offsetof(prs_meass_info_list_item_s, meas_prs_len),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(prs_meass_info_list_item_s::meas_prs_len)>(),
     decltype(prs_meass_info_list_item_s::meas_prs_len)::has_ext,
     false,
     decltype(prs_meass_info_list_item_s::meas_prs_len)::nof_types,
     decltype(prs_meass_info_list_item_s::meas_prs_len)::nof_exts,
     nullptr,
     &prs_meass_info_list_item_s::meas_prs_len_opts::desc},
    {offsetof(prs_meass_info_list_item_s, ie_exts),
     (int32_t)offsetof(prs_meass_info_list_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prs_meass_info_list_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_prs_meass_info_list_item_s_names[] = {{"pointA", nullptr},
                                                                                     {"measPRSPeriodicity", nullptr},
                                                                                     {"measPRSOffset", nullptr},
                                                                                     {"measurementPRSLength", nullptr},
                                                                                     {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_prs_meass_info_list_item_s_opts[]  = {
    {4, (int32_t)offsetof(prs_meass_info_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc prs_meass_info_list_item_s::desc = {
    {seq_table_prs_meass_info_list_item_s_fields, 5, UINT64_C(0xf), seq_table_prs_meass_info_list_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_meass_info_list_item_s_names};

static const char*           enum_names_prs_meass_info_list_item_s_meas_prs_periodicity_e__values[] = {"ms20",
                                                                                                       "ms40",
                                                                                                       "ms80",
                                                                                                       "ms160"};
const asn1::table::enum_desc prs_meass_info_list_item_s::meas_prs_periodicity_opts::desc            = {
    enum_names_prs_meass_info_list_item_s_meas_prs_periodicity_e__values,
    4,
    "prs_meass_info_list_item_s::meas_prs_periodicity_e_"};
uint8_t prs_meass_info_list_item_s::meas_prs_periodicity_opts::to_number() const
{
  static const uint8_t numbers[] = {20, 40, 80, 160};
  return map_enum_number(numbers, 4, value, "prs_meass_info_list_item_s::meas_prs_periodicity_e_");
}

static const char* enum_names_prs_meass_info_list_item_s_meas_prs_len_e__values[] =
    {"ms1dot5", "ms3", "ms3dot5", "ms4", "ms5dot5", "ms6", "ms10", "ms20"};
const asn1::table::enum_desc prs_meass_info_list_item_s::meas_prs_len_opts::desc = {
    enum_names_prs_meass_info_list_item_s_meas_prs_len_e__values,
    8,
    "prs_meass_info_list_item_s::meas_prs_len_e_"};
float prs_meass_info_list_item_s::meas_prs_len_opts::to_number() const
{
  static const float numbers[] = {1.5, 3.0, 3.5, 4.0, 5.5, 6.0, 10.0, 20.0};
  return map_enum_number(numbers, 8, value, "prs_meass_info_list_item_s::meas_prs_len_e_");
}
const char* prs_meass_info_list_item_s::meas_prs_len_opts::to_number_string() const
{
  static const char* number_strs[] = {"1.5", "3", "3.5", "4", "5.5", "6", "10", "20"};
  return convert_enum_idx(number_strs, 8, value, "prs_meass_info_list_item_s::meas_prs_len_e_");
}

// PRSBWAggregationRequestInfo-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_prsbw_aggregation_request_info_item_s_fields[] = {
    {offsetof(prsbw_aggregation_request_info_item_s, dl_prsbw_aggregation_request_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     2,
     3,
     &asn1::table::item_list_codec<
         decltype(prsbw_aggregation_request_info_item_s::dl_prsbw_aggregation_request_info_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &dl_prsbw_aggregation_request_info_item_s::desc},
    {offsetof(prsbw_aggregation_request_info_item_s, ie_exts),
     (int32_t)offsetof(prsbw_aggregation_request_info_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prsbw_aggregation_request_info_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_prsbw_aggregation_request_info_item_s_names[] = {
    {"dl-PRSBWAggregationRequestInfo-List", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_prsbw_aggregation_request_info_item_s_opts[] = {
    {1, (int32_t)offsetof(prsbw_aggregation_request_info_item_s, ie_exts_present)}};
const asn1::table::seq_desc prsbw_aggregation_request_info_item_s::desc = {
    {seq_table_prsbw_aggregation_request_info_item_s_fields,
     2,
     UINT64_C(0x1),
     seq_table_prsbw_aggregation_request_info_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_prsbw_aggregation_request_info_item_s_names};

// PRSConfigRequestType ::= ENUMERATED
static const char*           enum_names_prs_cfg_request_type_e_values[] = {"configure", "off"};
const asn1::table::enum_desc prs_cfg_request_type_opts::desc            = {enum_names_prs_cfg_request_type_e_values,
                                                                           2,
                                                                           "prs_cfg_request_type_e"};

// PRSMutingOption1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_muting_option1_s_fields[] = {
    {offsetof(prs_muting_option1_s, muting_pattern),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &dl_prs_muting_pattern_c::desc},
    {offsetof(prs_muting_option1_s, muting_bit_repeat_factor),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(prs_muting_option1_s::muting_bit_repeat_factor)>(),
     decltype(prs_muting_option1_s::muting_bit_repeat_factor)::has_ext,
     false,
     decltype(prs_muting_option1_s::muting_bit_repeat_factor)::nof_types,
     decltype(prs_muting_option1_s::muting_bit_repeat_factor)::nof_exts,
     nullptr,
     &prs_muting_option1_s::muting_bit_repeat_factor_opts::desc},
    {offsetof(prs_muting_option1_s, ie_exts),
     (int32_t)offsetof(prs_muting_option1_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prs_muting_option1_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_prs_muting_option1_s_names[] = {{"mutingPattern", nullptr},
                                                                               {"mutingBitRepetitionFactor", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_prs_muting_option1_s_opts[]  = {
    {2, (int32_t)offsetof(prs_muting_option1_s, ie_exts_present)}};
const asn1::table::seq_desc prs_muting_option1_s::desc = {
    {seq_table_prs_muting_option1_s_fields, 3, UINT64_C(0x3), seq_table_prs_muting_option1_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_muting_option1_s_names};

static const char* enum_names_prs_muting_option1_s_muting_bit_repeat_factor_e__values[] = {"n1", "n2", "n4", "n8"};
const asn1::table::enum_desc prs_muting_option1_s::muting_bit_repeat_factor_opts::desc  = {
    enum_names_prs_muting_option1_s_muting_bit_repeat_factor_e__values,
    4,
    "prs_muting_option1_s::muting_bit_repeat_factor_e_"};
uint8_t prs_muting_option1_s::muting_bit_repeat_factor_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8};
  return map_enum_number(numbers, 4, value, "prs_muting_option1_s::muting_bit_repeat_factor_e_");
}

// PRSMutingOption2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_muting_option2_s_fields[] = {
    {offsetof(prs_muting_option2_s, muting_pattern),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &dl_prs_muting_pattern_c::desc},
    {offsetof(prs_muting_option2_s, ie_exts),
     (int32_t)offsetof(prs_muting_option2_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prs_muting_option2_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_prs_muting_option2_s_names[] = {{"mutingPattern", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_prs_muting_option2_s_opts[]  = {
    {1, (int32_t)offsetof(prs_muting_option2_s, ie_exts_present)}};
const asn1::table::seq_desc prs_muting_option2_s::desc = {
    {seq_table_prs_muting_option2_s_fields, 2, UINT64_C(0x1), seq_table_prs_muting_option2_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_muting_option2_s_names};

// PRSMuting ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_muting_s_fields[] = {
    {offsetof(prs_muting_s, prs_muting_option1),
     (int32_t)offsetof(prs_muting_s, prs_muting_option1_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_muting_option1_s::desc},
    {offsetof(prs_muting_s, prs_muting_option2),
     (int32_t)offsetof(prs_muting_s, prs_muting_option2_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_muting_option2_s::desc},
    {offsetof(prs_muting_s, ie_exts),
     (int32_t)offsetof(prs_muting_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prs_muting_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_prs_muting_s_names[] = {{"pRSMutingOption1", nullptr},
                                                                       {"pRSMutingOption2", nullptr},
                                                                       {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_prs_muting_s_opts[]  = {
    {0, (int32_t)offsetof(prs_muting_s, prs_muting_option1_present)},
    {1, (int32_t)offsetof(prs_muting_s, prs_muting_option2_present)},
    {2, (int32_t)offsetof(prs_muting_s, ie_exts_present)}};
const asn1::table::seq_desc prs_muting_s::desc = {
    {seq_table_prs_muting_s_fields, 3, UINT64_C(0x0), seq_table_prs_muting_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_muting_s_names};

// PRSResource-QCLSourceSSB ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_res_qcl_source_ssb_s_fields[] = {
    {offsetof(prs_res_qcl_source_ssb_s, pci_nr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(prs_res_qcl_source_ssb_s, ssb_idx),
     (int32_t)offsetof(prs_res_qcl_source_ssb_s, ssb_idx_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(prs_res_qcl_source_ssb_s, ie_exts),
     (int32_t)offsetof(prs_res_qcl_source_ssb_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prs_res_qcl_source_ssb_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_prs_res_qcl_source_ssb_s_names[] = {{"pCI-NR", nullptr},
                                                                                   {"sSB-Index", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_prs_res_qcl_source_ssb_s_opts[]  = {
    {1, (int32_t)offsetof(prs_res_qcl_source_ssb_s, ssb_idx_present)},
    {2, (int32_t)offsetof(prs_res_qcl_source_ssb_s, ie_exts_present)}};
const asn1::table::seq_desc prs_res_qcl_source_ssb_s::desc = {
    {seq_table_prs_res_qcl_source_ssb_s_fields, 3, UINT64_C(0x1), seq_table_prs_res_qcl_source_ssb_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_res_qcl_source_ssb_s_names};

// PRSResource-QCLSourcePRS ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_res_qcl_source_prs_s_fields[] = {
    {offsetof(prs_res_qcl_source_prs_s, qcl_source_prs_res_set_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(prs_res_qcl_source_prs_s, qcl_source_prs_res_id),
     (int32_t)offsetof(prs_res_qcl_source_prs_s, qcl_source_prs_res_id_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(prs_res_qcl_source_prs_s, ie_exts),
     (int32_t)offsetof(prs_res_qcl_source_prs_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prs_res_qcl_source_prs_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_prs_res_qcl_source_prs_s_names[] = {
    {"qCLSourcePRSResourceSetID", nullptr},
    {"qCLSourcePRSResourceID", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_prs_res_qcl_source_prs_s_opts[] = {
    {1, (int32_t)offsetof(prs_res_qcl_source_prs_s, qcl_source_prs_res_id_present)},
    {2, (int32_t)offsetof(prs_res_qcl_source_prs_s, ie_exts_present)}};
const asn1::table::seq_desc prs_res_qcl_source_prs_s::desc = {
    {seq_table_prs_res_qcl_source_prs_s_fields, 3, UINT64_C(0x1), seq_table_prs_res_qcl_source_prs_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_res_qcl_source_prs_s_names};

// PRSResource-QCLInfo ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_prs_res_qcl_info_c_alternatives[] = {
    &asn1::table::alternative_ops<prs_res_qcl_source_ssb_s, false>::value,
    &asn1::table::alternative_ops<prs_res_qcl_source_prs_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<prs_res_qcl_info_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc prs_res_qcl_info_c::storage_desc = {
    choice_storage_prs_res_qcl_info_c_alternatives,
    3,
    "prs_res_qcl_info_c"};

prs_res_qcl_source_ssb_s& prs_res_qcl_info_c::set_qcl_source_ssb()
{
  set(types::qcl_source_ssb);
  return c.get<prs_res_qcl_source_ssb_s>();
}
prs_res_qcl_source_prs_s& prs_res_qcl_info_c::set_qcl_source_prs()
{
  set(types::qcl_source_prs);
  return c.get<prs_res_qcl_source_prs_s>();
}
protocol_ie_single_container_s<prs_res_qcl_info_ext_ies_o>& prs_res_qcl_info_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<prs_res_qcl_info_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_prs_res_qcl_info_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_res_qcl_source_ssb_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_res_qcl_source_prs_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<prs_res_qcl_info_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_prs_res_qcl_info_c_names[] = {{"qCLSourceSSB", nullptr},
                                                                                {"qCLSourcePRS", nullptr},
                                                                                {"choice-Extension", nullptr}};
const asn1::table::choice_desc prs_res_qcl_info_c::desc = {asn1::table::enum_int_type<prs_res_qcl_info_c::types>(),
                                                           prs_res_qcl_info_c::types::nof_types,
                                                           prs_res_qcl_info_c::types::nof_exts,
                                                           prs_res_qcl_info_c::types::has_ext,
                                                           true,
                                                           choice_table_prs_res_qcl_info_c_alternatives,
                                                           nullptr,
                                                           choice_table_prs_res_qcl_info_c_names,
                                                           "prs_res_qcl_info_c",
                                                           offsetof(prs_res_qcl_info_c, c.type),
                                                           offsetof(prs_res_qcl_info_c, c.value),
                                                           false,
                                                           &prs_res_qcl_info_c::storage_desc};

static const char* enum_names_prs_res_qcl_info_c_types_values[] = {"qCLSourceSSB", "qCLSourcePRS", "choice-Extension"};
const asn1::table::enum_desc prs_res_qcl_info_c::types_opts::desc = {enum_names_prs_res_qcl_info_c_types_values,
                                                                     3,
                                                                     "prs_res_qcl_info_c::types"};

static const uint32_t objset_id_prs_res_item_ext_ies_o[]   = {112};
static const uint8_t  objset_crit_prs_res_item_ext_ies_o[] = {crit_e::ignore};

// PRSResource-Item-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_prs_res_item_ext_ies_container_values[] = {
    {offsetof(prs_res_item_ext_ies_container, extended_res_symbol_offset),
     (int32_t)offsetof(prs_res_item_ext_ies_container, extended_res_symbol_offset_present),
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(0),
     UINT64_C(13),
     nullptr}};
static const int32_t ie_table_prs_res_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(prs_res_item_ext_ies_container, extended_res_symbol_offset_present)};
const asn1::table::ie_container_desc prs_res_item_ext_ies_container::desc = {
    objset_id_prs_res_item_ext_ies_o,
    objset_crit_prs_res_item_ext_ies_o,
    ie_table_prs_res_item_ext_ies_container_values,
    ie_table_prs_res_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// PRSResource-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_res_item_s_fields[] = {
    {offsetof(prs_res_item_s, prs_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(prs_res_item_s, seq_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(4095),
     nullptr},
    {offsetof(prs_res_item_s, re_offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(0),
     UINT64_C(11),
     nullptr},
    {offsetof(prs_res_item_s, res_slot_offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(511),
     nullptr},
    {offsetof(prs_res_item_s, res_symbol_offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(12),
     nullptr},
    {offsetof(prs_res_item_s, qcl_info),
     (int32_t)offsetof(prs_res_item_s, qcl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_res_qcl_info_c::desc},
    {offsetof(prs_res_item_s, ie_exts),
     (int32_t)offsetof(prs_res_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_res_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_prs_res_item_s_names[] = {{"pRSResourceID", nullptr},
                                                                         {"sequenceID", nullptr},
                                                                         {"rEOffset", nullptr},
                                                                         {"resourceSlotOffset", nullptr},
                                                                         {"resourceSymbolOffset", nullptr},
                                                                         {"qCLInfo", nullptr},
                                                                         {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_prs_res_item_s_opts[]  = {
    {5, (int32_t)offsetof(prs_res_item_s, qcl_info_present)},
    {6, (int32_t)offsetof(prs_res_item_s, ie_exts_present)}};
const asn1::table::seq_desc prs_res_item_s::desc = {
    {seq_table_prs_res_item_s_fields, 7, UINT64_C(0x1f), seq_table_prs_res_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_res_item_s_names};

// PRSResourceSet-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_res_set_item_s_fields[] = {
    {offsetof(prs_res_set_item_s, prs_res_set_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(prs_res_set_item_s, subcarrier_spacing),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(prs_res_set_item_s::subcarrier_spacing)>(),
     decltype(prs_res_set_item_s::subcarrier_spacing)::has_ext,
     false,
     decltype(prs_res_set_item_s::subcarrier_spacing)::nof_types,
     decltype(prs_res_set_item_s::subcarrier_spacing)::nof_exts,
     nullptr,
     &prs_res_set_item_s::subcarrier_spacing_opts::desc},
    {offsetof(prs_res_set_item_s, pr_sbw),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(1),
     UINT64_C(63),
     nullptr},
    {offsetof(prs_res_set_item_s, start_prb),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(2176),
     nullptr},
    {offsetof(prs_res_set_item_s, point_a),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(prs_res_set_item_s, comb_size),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(prs_res_set_item_s::comb_size)>(),
     decltype(prs_res_set_item_s::comb_size)::has_ext,
     false,
     decltype(prs_res_set_item_s::comb_size)::nof_types,
     decltype(prs_res_set_item_s::comb_size)::nof_exts,
     nullptr,
     &prs_res_set_item_s::comb_size_opts::desc},
    {offsetof(prs_res_set_item_s, cp_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(prs_res_set_item_s::cp_type)>(),
     decltype(prs_res_set_item_s::cp_type)::has_ext,
     false,
     decltype(prs_res_set_item_s::cp_type)::nof_types,
     decltype(prs_res_set_item_s::cp_type)::nof_exts,
     nullptr,
     &prs_res_set_item_s::cp_type_opts::desc},
    {offsetof(prs_res_set_item_s, res_set_periodicity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(prs_res_set_item_s::res_set_periodicity)>(),
     decltype(prs_res_set_item_s::res_set_periodicity)::has_ext,
     false,
     decltype(prs_res_set_item_s::res_set_periodicity)::nof_types,
     decltype(prs_res_set_item_s::res_set_periodicity)::nof_exts,
     nullptr,
     &prs_res_set_item_s::res_set_periodicity_opts::desc},
    {offsetof(prs_res_set_item_s, res_set_slot_offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(81919),
     nullptr},
    {offsetof(prs_res_set_item_s, res_repeat_factor),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(prs_res_set_item_s::res_repeat_factor)>(),
     decltype(prs_res_set_item_s::res_repeat_factor)::has_ext,
     false,
     decltype(prs_res_set_item_s::res_repeat_factor)::nof_types,
     decltype(prs_res_set_item_s::res_repeat_factor)::nof_exts,
     nullptr,
     &prs_res_set_item_s::res_repeat_factor_opts::desc},
    {offsetof(prs_res_set_item_s, res_time_gap),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(prs_res_set_item_s::res_time_gap)>(),
     decltype(prs_res_set_item_s::res_time_gap)::has_ext,
     false,
     decltype(prs_res_set_item_s::res_time_gap)::nof_types,
     decltype(prs_res_set_item_s::res_time_gap)::nof_exts,
     nullptr,
     &prs_res_set_item_s::res_time_gap_opts::desc},
    {offsetof(prs_res_set_item_s, res_numof_symbols),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(prs_res_set_item_s::res_numof_symbols)>(),
     decltype(prs_res_set_item_s::res_numof_symbols)::has_ext,
     false,
     decltype(prs_res_set_item_s::res_numof_symbols)::nof_types,
     decltype(prs_res_set_item_s::res_numof_symbols)::nof_exts,
     nullptr,
     &prs_res_set_item_s::res_numof_symbols_opts::desc},
    {offsetof(prs_res_set_item_s, prs_muting),
     (int32_t)offsetof(prs_res_set_item_s, prs_muting_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_muting_s::desc},
    {offsetof(prs_res_set_item_s, prs_res_tx_pwr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     true,
     INT64_C(-60),
     UINT64_C(50),
     nullptr},
    {offsetof(prs_res_set_item_s, prs_res_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(prs_res_set_item_s::prs_res_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &prs_res_item_s::desc},
    {offsetof(prs_res_set_item_s, ie_exts),
     (int32_t)offsetof(prs_res_set_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prs_res_set_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_prs_res_set_item_s_names[] = {{"pRSResourceSetID", nullptr},
                                                                             {"subcarrierSpacing", nullptr},
                                                                             {"pRSbandwidth", nullptr},
                                                                             {"startPRB", nullptr},
                                                                             {"pointA", nullptr},
                                                                             {"combSize", nullptr},
                                                                             {"cPType", nullptr},
                                                                             {"resourceSetPeriodicity", nullptr},
                                                                             {"resourceSetSlotOffset", nullptr},
                                                                             {"resourceRepetitionFactor", nullptr},
                                                                             {"resourceTimeGap", nullptr},
                                                                             {"resourceNumberofSymbols", nullptr},
                                                                             {"pRSMuting", nullptr},
                                                                             {"pRSResourceTransmitPower", nullptr},
                                                                             {"pRSResource-List", nullptr},
                                                                             {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_prs_res_set_item_s_opts[]  = {
    {12, (int32_t)offsetof(prs_res_set_item_s, prs_muting_present)},
    {15, (int32_t)offsetof(prs_res_set_item_s, ie_exts_present)}};
const asn1::table::seq_desc prs_res_set_item_s::desc = {
    {seq_table_prs_res_set_item_s_fields, 16, UINT64_C(0x6fff), seq_table_prs_res_set_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_res_set_item_s_names};

static const char* enum_names_prs_res_set_item_s_subcarrier_spacing_e__values[] = {"kHz15", "kHz30", "kHz60", "kHz120"};
const asn1::table::enum_desc prs_res_set_item_s::subcarrier_spacing_opts::desc  = {
    enum_names_prs_res_set_item_s_subcarrier_spacing_e__values,
    4,
    "prs_res_set_item_s::subcarrier_spacing_e_"};
uint8_t prs_res_set_item_s::subcarrier_spacing_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 30, 60, 120};
  return map_enum_number(numbers, 4, value, "prs_res_set_item_s::subcarrier_spacing_e_");
}

static const char*           enum_names_prs_res_set_item_s_comb_size_e__values[] = {"n2", "n4", "n6", "n12"};
const asn1::table::enum_desc prs_res_set_item_s::comb_size_opts::desc            = {
    enum_names_prs_res_set_item_s_comb_size_e__values,
    4,
    "prs_res_set_item_s::comb_size_e_"};
uint8_t prs_res_set_item_s::comb_size_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 12};
  return map_enum_number(numbers, 4, value, "prs_res_set_item_s::comb_size_e_");
}

static const char*           enum_names_prs_res_set_item_s_cp_type_e__values[] = {"normal", "extended"};
const asn1::table::enum_desc prs_res_set_item_s::cp_type_opts::desc = {enum_names_prs_res_set_item_s_cp_type_e__values,
                                                                       2,
                                                                       "prs_res_set_item_s::cp_type_e_"};

static const char* enum_names_prs_res_set_item_s_res_set_periodicity_e__values[] = {
    "n4",   "n5",    "n8",    "n10",   "n16",    "n20",    "n32",    "n40",    "n64",  "n80",  "n160", "n320",
    "n640", "n1280", "n2560", "n5120", "n10240", "n20480", "n40960", "n81920", "n128", "n256", "n512"};
const asn1::table::enum_desc prs_res_set_item_s::res_set_periodicity_opts::desc = {
    enum_names_prs_res_set_item_s_res_set_periodicity_e__values,
    23,
    "prs_res_set_item_s::res_set_periodicity_e_"};
uint32_t prs_res_set_item_s::res_set_periodicity_opts::to_number() const
{
  static const uint32_t numbers[] = {4,   5,    8,    10,   16,    20,    32,    40,    64,  80,  160, 320,
                                     640, 1280, 2560, 5120, 10240, 20480, 40960, 81920, 128, 256, 512};
  return map_enum_number(numbers, 23, value, "prs_res_set_item_s::res_set_periodicity_e_");
}

static const char* enum_names_prs_res_set_item_s_res_repeat_factor_e__values[] =
    {"rf1", "rf2", "rf4", "rf6", "rf8", "rf16", "rf32"};
const asn1::table::enum_desc prs_res_set_item_s::res_repeat_factor_opts::desc = {
    enum_names_prs_res_set_item_s_res_repeat_factor_e__values,
    7,
    "prs_res_set_item_s::res_repeat_factor_e_"};
uint8_t prs_res_set_item_s::res_repeat_factor_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 6, 8, 16, 32};
  return map_enum_number(numbers, 7, value, "prs_res_set_item_s::res_repeat_factor_e_");
}

static const char* enum_names_prs_res_set_item_s_res_time_gap_e__values[] =
    {"tg1", "tg2", "tg4", "tg8", "tg16", "tg32"};
const asn1::table::enum_desc prs_res_set_item_s::res_time_gap_opts::desc = {
    enum_names_prs_res_set_item_s_res_time_gap_e__values,
    6,
    "prs_res_set_item_s::res_time_gap_e_"};
uint8_t prs_res_set_item_s::res_time_gap_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32};
  return map_enum_number(numbers, 6, value, "prs_res_set_item_s::res_time_gap_e_");
}

static const char* enum_names_prs_res_set_item_s_res_numof_symbols_e__values[] = {"n2", "n4", "n6", "n12", "n1"};
const asn1::table::enum_desc prs_res_set_item_s::res_numof_symbols_opts::desc  = {
    enum_names_prs_res_set_item_s_res_numof_symbols_e__values,
    5,
    "prs_res_set_item_s::res_numof_symbols_e_"};
uint8_t prs_res_set_item_s::res_numof_symbols_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 12, 1};
  return map_enum_number(numbers, 5, value, "prs_res_set_item_s::res_numof_symbols_e_");
}

static const uint32_t objset_id_prs_cfg_ext_ies_o[]   = {132};
static const uint8_t  objset_crit_prs_cfg_ext_ies_o[] = {crit_e::ignore};

// PRSConfiguration-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_prs_cfg_ext_ies_container_values[] = {
    {offsetof(prs_cfg_ext_ies_container, aggr_prs_res_set_list),
     (int32_t)offsetof(prs_cfg_ext_ies_container, aggr_prs_res_set_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2,
     &asn1::table::item_list_codec<decltype(prs_cfg_ext_ies_container::aggr_prs_res_set_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &aggr_prs_res_set_item_s::desc}};
static const int32_t ie_table_prs_cfg_ext_ies_container_presences[] = {
    (int32_t)offsetof(prs_cfg_ext_ies_container, aggr_prs_res_set_list_present)};
const asn1::table::ie_container_desc prs_cfg_ext_ies_container::desc = {objset_id_prs_cfg_ext_ies_o,
                                                                        objset_crit_prs_cfg_ext_ies_o,
                                                                        ie_table_prs_cfg_ext_ies_container_values,
                                                                        ie_table_prs_cfg_ext_ies_container_presences,
                                                                        1,
                                                                        0,
                                                                        1,
                                                                        65535,
                                                                        true,
                                                                        "extensionValue"};

// PRSConfiguration ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_cfg_s_fields[] = {
    {offsetof(prs_cfg_s, prs_res_set_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(prs_cfg_s::prs_res_set_list), asn1::table::sequence_item_ops, false, true>::
         value,
     &prs_res_set_item_s::desc},
    {offsetof(prs_cfg_s, ie_exts),
     (int32_t)offsetof(prs_cfg_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_cfg_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_prs_cfg_s_names[] = {{"pRSResourceSet-List", nullptr},
                                                                    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_prs_cfg_s_opts[]  = {{1, (int32_t)offsetof(prs_cfg_s, ie_exts_present)}};
const asn1::table::seq_desc          prs_cfg_s::desc             = {
    {seq_table_prs_cfg_s_fields, 2, UINT64_C(0x1), seq_table_prs_cfg_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_cfg_s_names};

// RequestedDLPRSResource-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_requested_dl_prs_res_item_s_fields[] = {
    {offsetof(requested_dl_prs_res_item_s, qcl_info),
     (int32_t)offsetof(requested_dl_prs_res_item_s, qcl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_res_qcl_info_c::desc},
    {offsetof(requested_dl_prs_res_item_s, ie_exts),
     (int32_t)offsetof(requested_dl_prs_res_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(requested_dl_prs_res_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_requested_dl_prs_res_item_s_names[] = {{"qCLInfo", nullptr},
                                                                                      {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_requested_dl_prs_res_item_s_opts[]  = {
    {0, (int32_t)offsetof(requested_dl_prs_res_item_s, qcl_info_present)},
    {1, (int32_t)offsetof(requested_dl_prs_res_item_s, ie_exts_present)}};
const asn1::table::seq_desc requested_dl_prs_res_item_s::desc = {
    {seq_table_requested_dl_prs_res_item_s_fields, 2, UINT64_C(0x0), seq_table_requested_dl_prs_res_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_requested_dl_prs_res_item_s_names};

// StartTimeAndDuration ::= SEQUENCE
static const asn1::table::seq_field seq_table_start_time_and_dur_s_fields[] = {
    {offsetof(start_time_and_dur_s, start_time),
     (int32_t)offsetof(start_time_and_dur_s, start_time_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(start_time_and_dur_s::start_time), asn1::table::struct_json>::value},
    {offsetof(start_time_and_dur_s, dur),
     (int32_t)offsetof(start_time_and_dur_s, dur_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(90060),
     nullptr},
    {offsetof(start_time_and_dur_s, ie_exts),
     (int32_t)offsetof(start_time_and_dur_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(start_time_and_dur_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_start_time_and_dur_s_names[] = {{"startTime", nullptr},
                                                                               {"duration", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_start_time_and_dur_s_opts[]  = {
    {0, (int32_t)offsetof(start_time_and_dur_s, start_time_present)},
    {1, (int32_t)offsetof(start_time_and_dur_s, dur_present)},
    {2, (int32_t)offsetof(start_time_and_dur_s, ie_exts_present)}};
const asn1::table::seq_desc start_time_and_dur_s::desc = {
    {seq_table_start_time_and_dur_s_fields, 3, UINT64_C(0x0), seq_table_start_time_and_dur_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_start_time_and_dur_s_names};

// RequestedDLPRSResourceSet-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_requested_dl_prs_res_set_item_s_fields[] = {
    {offsetof(requested_dl_prs_res_set_item_s, pr_sbw),
     (int32_t)offsetof(requested_dl_prs_res_set_item_s, pr_sbw_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(1),
     UINT64_C(63),
     nullptr},
    {offsetof(requested_dl_prs_res_set_item_s, comb_size),
     (int32_t)offsetof(requested_dl_prs_res_set_item_s, comb_size_present),
     asn1::table::enum_op<decltype(requested_dl_prs_res_set_item_s::comb_size)>(),
     decltype(requested_dl_prs_res_set_item_s::comb_size)::has_ext,
     false,
     decltype(requested_dl_prs_res_set_item_s::comb_size)::nof_types,
     decltype(requested_dl_prs_res_set_item_s::comb_size)::nof_exts,
     nullptr,
     &requested_dl_prs_res_set_item_s::comb_size_opts::desc},
    {offsetof(requested_dl_prs_res_set_item_s, res_set_periodicity),
     (int32_t)offsetof(requested_dl_prs_res_set_item_s, res_set_periodicity_present),
     asn1::table::enum_op<decltype(requested_dl_prs_res_set_item_s::res_set_periodicity)>(),
     decltype(requested_dl_prs_res_set_item_s::res_set_periodicity)::has_ext,
     false,
     decltype(requested_dl_prs_res_set_item_s::res_set_periodicity)::nof_types,
     decltype(requested_dl_prs_res_set_item_s::res_set_periodicity)::nof_exts,
     nullptr,
     &requested_dl_prs_res_set_item_s::res_set_periodicity_opts::desc},
    {offsetof(requested_dl_prs_res_set_item_s, res_repeat_factor),
     (int32_t)offsetof(requested_dl_prs_res_set_item_s, res_repeat_factor_present),
     asn1::table::enum_op<decltype(requested_dl_prs_res_set_item_s::res_repeat_factor)>(),
     decltype(requested_dl_prs_res_set_item_s::res_repeat_factor)::has_ext,
     false,
     decltype(requested_dl_prs_res_set_item_s::res_repeat_factor)::nof_types,
     decltype(requested_dl_prs_res_set_item_s::res_repeat_factor)::nof_exts,
     nullptr,
     &requested_dl_prs_res_set_item_s::res_repeat_factor_opts::desc},
    {offsetof(requested_dl_prs_res_set_item_s, res_numof_symbols),
     (int32_t)offsetof(requested_dl_prs_res_set_item_s, res_numof_symbols_present),
     asn1::table::enum_op<decltype(requested_dl_prs_res_set_item_s::res_numof_symbols)>(),
     decltype(requested_dl_prs_res_set_item_s::res_numof_symbols)::has_ext,
     false,
     decltype(requested_dl_prs_res_set_item_s::res_numof_symbols)::nof_types,
     decltype(requested_dl_prs_res_set_item_s::res_numof_symbols)::nof_exts,
     nullptr,
     &requested_dl_prs_res_set_item_s::res_numof_symbols_opts::desc},
    {offsetof(requested_dl_prs_res_set_item_s, requested_dl_prs_res_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(requested_dl_prs_res_set_item_s::requested_dl_prs_res_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &requested_dl_prs_res_item_s::desc},
    {offsetof(requested_dl_prs_res_set_item_s, res_set_start_time_and_dur),
     (int32_t)offsetof(requested_dl_prs_res_set_item_s, res_set_start_time_and_dur_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &start_time_and_dur_s::desc},
    {offsetof(requested_dl_prs_res_set_item_s, ie_exts),
     (int32_t)offsetof(requested_dl_prs_res_set_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(requested_dl_prs_res_set_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_requested_dl_prs_res_set_item_s_names[] = {
    {"pRSbandwidth", nullptr},
    {"combSize", nullptr},
    {"resourceSetPeriodicity", nullptr},
    {"resourceRepetitionFactor", nullptr},
    {"resourceNumberofSymbols", nullptr},
    {"requestedDLPRSResource-List", nullptr},
    {"resourceSetStartTimeAndDuration", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_requested_dl_prs_res_set_item_s_opts[] = {
    {0, (int32_t)offsetof(requested_dl_prs_res_set_item_s, pr_sbw_present)},
    {1, (int32_t)offsetof(requested_dl_prs_res_set_item_s, comb_size_present)},
    {2, (int32_t)offsetof(requested_dl_prs_res_set_item_s, res_set_periodicity_present)},
    {3, (int32_t)offsetof(requested_dl_prs_res_set_item_s, res_repeat_factor_present)},
    {4, (int32_t)offsetof(requested_dl_prs_res_set_item_s, res_numof_symbols_present)},
    {5, asn1::table::seq_field::list_presence},
    {6, (int32_t)offsetof(requested_dl_prs_res_set_item_s, res_set_start_time_and_dur_present)},
    {7, (int32_t)offsetof(requested_dl_prs_res_set_item_s, ie_exts_present)}};
const asn1::table::seq_desc requested_dl_prs_res_set_item_s::desc = {{seq_table_requested_dl_prs_res_set_item_s_fields,
                                                                      8,
                                                                      UINT64_C(0x0),
                                                                      seq_table_requested_dl_prs_res_set_item_s_opts,
                                                                      8},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_requested_dl_prs_res_set_item_s_names};

static const char* enum_names_requested_dl_prs_res_set_item_s_comb_size_e__values[] = {"n2", "n4", "n6", "n12"};
const asn1::table::enum_desc requested_dl_prs_res_set_item_s::comb_size_opts::desc  = {
    enum_names_requested_dl_prs_res_set_item_s_comb_size_e__values,
    4,
    "requested_dl_prs_res_set_item_s::comb_size_e_"};
uint8_t requested_dl_prs_res_set_item_s::comb_size_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 12};
  return map_enum_number(numbers, 4, value, "requested_dl_prs_res_set_item_s::comb_size_e_");
}

static const char* enum_names_requested_dl_prs_res_set_item_s_res_set_periodicity_e__values[] = {
    "n4",   "n5",    "n8",    "n10",   "n16",    "n20",    "n32",    "n40",    "n64",  "n80",  "n160", "n320",
    "n640", "n1280", "n2560", "n5120", "n10240", "n20480", "n40960", "n81920", "n128", "n256", "n512"};
const asn1::table::enum_desc requested_dl_prs_res_set_item_s::res_set_periodicity_opts::desc = {
    enum_names_requested_dl_prs_res_set_item_s_res_set_periodicity_e__values,
    23,
    "requested_dl_prs_res_set_item_s::res_set_periodicity_e_"};
uint32_t requested_dl_prs_res_set_item_s::res_set_periodicity_opts::to_number() const
{
  static const uint32_t numbers[] = {4,   5,    8,    10,   16,    20,    32,    40,    64,  80,  160, 320,
                                     640, 1280, 2560, 5120, 10240, 20480, 40960, 81920, 128, 256, 512};
  return map_enum_number(numbers, 23, value, "requested_dl_prs_res_set_item_s::res_set_periodicity_e_");
}

static const char* enum_names_requested_dl_prs_res_set_item_s_res_repeat_factor_e__values[] =
    {"rf1", "rf2", "rf4", "rf6", "rf8", "rf16", "rf32"};
const asn1::table::enum_desc requested_dl_prs_res_set_item_s::res_repeat_factor_opts::desc = {
    enum_names_requested_dl_prs_res_set_item_s_res_repeat_factor_e__values,
    7,
    "requested_dl_prs_res_set_item_s::res_repeat_factor_e_"};
uint8_t requested_dl_prs_res_set_item_s::res_repeat_factor_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 6, 8, 16, 32};
  return map_enum_number(numbers, 7, value, "requested_dl_prs_res_set_item_s::res_repeat_factor_e_");
}

static const char*           enum_names_requested_dl_prs_res_set_item_s_res_numof_symbols_e__values[] = {"n2",
                                                                                                         "n4",
                                                                                                         "n6",
                                                                                                         "n12",
                                                                                                         "n1"};
const asn1::table::enum_desc requested_dl_prs_res_set_item_s::res_numof_symbols_opts::desc            = {
    enum_names_requested_dl_prs_res_set_item_s_res_numof_symbols_e__values,
    5,
    "requested_dl_prs_res_set_item_s::res_numof_symbols_e_"};
uint8_t requested_dl_prs_res_set_item_s::res_numof_symbols_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 12, 1};
  return map_enum_number(numbers, 5, value, "requested_dl_prs_res_set_item_s::res_numof_symbols_e_");
}

static const uint32_t objset_id_requested_dl_prs_tx_characteristics_ext_ies_o[]   = {130};
static const uint8_t  objset_crit_requested_dl_prs_tx_characteristics_ext_ies_o[] = {crit_e::ignore};

// RequestedDLPRSTransmissionCharacteristics-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_requested_dl_prs_tx_characteristics_ext_ies_container_values[] = {
    {offsetof(requested_dl_prs_tx_characteristics_ext_ies_container, prsbw_aggregation_request_info_list),
     (int32_t)offsetof(requested_dl_prs_tx_characteristics_ext_ies_container,
                       prsbw_aggregation_request_info_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2,
     &asn1::table::item_list_codec<
         decltype(requested_dl_prs_tx_characteristics_ext_ies_container::prsbw_aggregation_request_info_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &prsbw_aggregation_request_info_item_s::desc}};
static const int32_t ie_table_requested_dl_prs_tx_characteristics_ext_ies_container_presences[] = {
    (int32_t)offsetof(requested_dl_prs_tx_characteristics_ext_ies_container,
                      prsbw_aggregation_request_info_list_present)};
const asn1::table::ie_container_desc requested_dl_prs_tx_characteristics_ext_ies_container::desc = {
    objset_id_requested_dl_prs_tx_characteristics_ext_ies_o,
    objset_crit_requested_dl_prs_tx_characteristics_ext_ies_o,
    ie_table_requested_dl_prs_tx_characteristics_ext_ies_container_values,
    ie_table_requested_dl_prs_tx_characteristics_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// RequestedDLPRSTransmissionCharacteristics ::= SEQUENCE
static const asn1::table::seq_field seq_table_requested_dl_prs_tx_characteristics_s_fields[] = {
    {offsetof(requested_dl_prs_tx_characteristics_s, requested_dl_prs_res_set_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(requested_dl_prs_tx_characteristics_s::requested_dl_prs_res_set_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &requested_dl_prs_res_set_item_s::desc},
    {offsetof(requested_dl_prs_tx_characteristics_s, numof_freq_layers),
     (int32_t)offsetof(requested_dl_prs_tx_characteristics_s, numof_freq_layers_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(1),
     UINT64_C(4),
     nullptr},
    {offsetof(requested_dl_prs_tx_characteristics_s, start_time_and_dur),
     (int32_t)offsetof(requested_dl_prs_tx_characteristics_s, start_time_and_dur_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &start_time_and_dur_s::desc},
    {offsetof(requested_dl_prs_tx_characteristics_s, ie_exts),
     (int32_t)offsetof(requested_dl_prs_tx_characteristics_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &requested_dl_prs_tx_characteristics_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_requested_dl_prs_tx_characteristics_s_names[] = {
    {"requestedDLPRSResourceSet-List", nullptr},
    {"numberofFrequencyLayers", nullptr},
    {"startTimeAndDuration", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_requested_dl_prs_tx_characteristics_s_opts[] = {
    {1, (int32_t)offsetof(requested_dl_prs_tx_characteristics_s, numof_freq_layers_present)},
    {2, (int32_t)offsetof(requested_dl_prs_tx_characteristics_s, start_time_and_dur_present)},
    {3, (int32_t)offsetof(requested_dl_prs_tx_characteristics_s, ie_exts_present)}};
const asn1::table::seq_desc requested_dl_prs_tx_characteristics_s::desc = {
    {seq_table_requested_dl_prs_tx_characteristics_s_fields,
     4,
     UINT64_C(0x1),
     seq_table_requested_dl_prs_tx_characteristics_s_opts,
     3},
    true,
    true,
    nullptr,
    0,
    seq_table_requested_dl_prs_tx_characteristics_s_names};

// PRSTransmissionOffPerResourceSet-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_tx_off_per_res_set_item_s_fields[] = {
    {offsetof(prs_tx_off_per_res_set_item_s, prs_res_set_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(prs_tx_off_per_res_set_item_s, ie_exts),
     (int32_t)offsetof(prs_tx_off_per_res_set_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prs_tx_off_per_res_set_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_prs_tx_off_per_res_set_item_s_names[] = {{"pRSResourceSetID", nullptr},
                                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_prs_tx_off_per_res_set_item_s_opts[]  = {
    {1, (int32_t)offsetof(prs_tx_off_per_res_set_item_s, ie_exts_present)}};
const asn1::table::seq_desc prs_tx_off_per_res_set_item_s::desc = {
    {seq_table_prs_tx_off_per_res_set_item_s_fields, 2, UINT64_C(0x1), seq_table_prs_tx_off_per_res_set_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_tx_off_per_res_set_item_s_names};

// PRSTransmissionOffIndicationPerResource-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_tx_off_ind_per_res_item_s_fields[] = {
    {offsetof(prs_tx_off_ind_per_res_item_s, prs_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(prs_tx_off_ind_per_res_item_s, ie_exts),
     (int32_t)offsetof(prs_tx_off_ind_per_res_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prs_tx_off_ind_per_res_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_prs_tx_off_ind_per_res_item_s_names[] = {{"pRSResourceID", nullptr},
                                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_prs_tx_off_ind_per_res_item_s_opts[]  = {
    {1, (int32_t)offsetof(prs_tx_off_ind_per_res_item_s, ie_exts_present)}};
const asn1::table::seq_desc prs_tx_off_ind_per_res_item_s::desc = {
    {seq_table_prs_tx_off_ind_per_res_item_s_fields, 2, UINT64_C(0x1), seq_table_prs_tx_off_ind_per_res_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_tx_off_ind_per_res_item_s_names};

// PRSTransmissionOffPerResource-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_tx_off_per_res_item_s_fields[] = {
    {offsetof(prs_tx_off_per_res_item_s, prs_res_set_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(prs_tx_off_per_res_item_s, prs_tx_off_ind_per_res_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(prs_tx_off_per_res_item_s::prs_tx_off_ind_per_res_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &prs_tx_off_ind_per_res_item_s::desc},
    {offsetof(prs_tx_off_per_res_item_s, ie_exts),
     (int32_t)offsetof(prs_tx_off_per_res_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prs_tx_off_per_res_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_prs_tx_off_per_res_item_s_names[] = {
    {"pRSResourceSetID", nullptr},
    {"pRSTransmissionOffIndicationPerResourceList", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_prs_tx_off_per_res_item_s_opts[] = {
    {2, (int32_t)offsetof(prs_tx_off_per_res_item_s, ie_exts_present)}};
const asn1::table::seq_desc prs_tx_off_per_res_item_s::desc = {
    {seq_table_prs_tx_off_per_res_item_s_fields, 3, UINT64_C(0x3), seq_table_prs_tx_off_per_res_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_tx_off_per_res_item_s_names};

// PRSTransmissionOffIndication ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_prs_tx_off_ind_c_alternatives[] = {
    nullptr,
    &asn1::table::alternative_ops<prs_tx_off_per_res_set_l, false>::value,
    &asn1::table::alternative_ops<prs_tx_off_per_res_l, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<prs_tx_off_ind_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc prs_tx_off_ind_c::storage_desc = {choice_storage_prs_tx_off_ind_c_alternatives,
                                                                         4,
                                                                         "prs_tx_off_ind_c"};

void prs_tx_off_ind_c::set_prs_tx_off_per_trp()
{
  set(types::prs_tx_off_per_trp);
}
prs_tx_off_per_res_set_l& prs_tx_off_ind_c::set_prs_tx_off_per_res_set()
{
  set(types::prs_tx_off_per_res_set);
  return c.get<prs_tx_off_per_res_set_l>();
}
prs_tx_off_per_res_l& prs_tx_off_ind_c::set_prs_tx_off_per_res()
{
  set(types::prs_tx_off_per_res);
  return c.get<prs_tx_off_per_res_l>();
}
protocol_ie_single_container_s<prs_tx_off_ind_ext_ies_o>& prs_tx_off_ind_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<prs_tx_off_ind_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_prs_tx_off_ind_c_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::list_codec<prs_tx_off_per_res_set_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::list_codec<prs_tx_off_per_res_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<prs_tx_off_ind_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_prs_tx_off_ind_c_names[] = {
    {"pRSTransmissionOffPerTRP", nullptr},
    {"pRSTransmissionOffPerResourceSet", nullptr},
    {"pRSTransmissionOffPerResource", nullptr},
    {"choice-Extension", nullptr}};
const asn1::table::choice_desc prs_tx_off_ind_c::desc = {asn1::table::enum_int_type<prs_tx_off_ind_c::types>(),
                                                         prs_tx_off_ind_c::types::nof_types,
                                                         prs_tx_off_ind_c::types::nof_exts,
                                                         prs_tx_off_ind_c::types::has_ext,
                                                         true,
                                                         choice_table_prs_tx_off_ind_c_alternatives,
                                                         nullptr,
                                                         choice_table_prs_tx_off_ind_c_names,
                                                         "prs_tx_off_ind_c",
                                                         offsetof(prs_tx_off_ind_c, c.type),
                                                         offsetof(prs_tx_off_ind_c, c.value),
                                                         false,
                                                         &prs_tx_off_ind_c::storage_desc};

static const char*           enum_names_prs_tx_off_ind_c_types_values[] = {"pRSTransmissionOffPerTRP",
                                                                           "pRSTransmissionOffPerResourceSet",
                                                                           "pRSTransmissionOffPerResource",
                                                                           "choice-Extension"};
const asn1::table::enum_desc prs_tx_off_ind_c::types_opts::desc         = {enum_names_prs_tx_off_ind_c_types_values,
                                                                           4,
                                                                           "prs_tx_off_ind_c::types"};

// PRSTransmissionOffInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_tx_off_info_s_fields[] = {
    {offsetof(prs_tx_off_info_s, prs_tx_off_ind),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_tx_off_ind_c::desc},
    {offsetof(prs_tx_off_info_s, ie_exts),
     (int32_t)offsetof(prs_tx_off_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prs_tx_off_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_prs_tx_off_info_s_names[] = {{"pRSTransmissionOffIndication", nullptr},
                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_prs_tx_off_info_s_opts[]  = {
    {1, (int32_t)offsetof(prs_tx_off_info_s, ie_exts_present)}};
const asn1::table::seq_desc prs_tx_off_info_s::desc = {
    {seq_table_prs_tx_off_info_s_fields, 2, UINT64_C(0x1), seq_table_prs_tx_off_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_tx_off_info_s_names};

// PRSTRPItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_prstrp_item_s_fields[] = {
    {offsetof(prstrp_item_s, trp_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(65535),
     nullptr},
    {offsetof(prstrp_item_s, requested_dl_prs_tx_characteristics),
     (int32_t)offsetof(prstrp_item_s, requested_dl_prs_tx_characteristics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &requested_dl_prs_tx_characteristics_s::desc},
    {offsetof(prstrp_item_s, prs_tx_off_info),
     (int32_t)offsetof(prstrp_item_s, prs_tx_off_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_tx_off_info_s::desc},
    {offsetof(prstrp_item_s, ie_exts),
     (int32_t)offsetof(prstrp_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prstrp_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_prstrp_item_s_names[] = {
    {"tRP-ID", nullptr},
    {"requestedDLPRSTransmissionCharacteristics", nullptr},
    {"pRSTransmissionOffInformation", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_prstrp_item_s_opts[] = {
    {1, (int32_t)offsetof(prstrp_item_s, requested_dl_prs_tx_characteristics_present)},
    {2, (int32_t)offsetof(prstrp_item_s, prs_tx_off_info_present)},
    {3, (int32_t)offsetof(prstrp_item_s, ie_exts_present)}};
const asn1::table::seq_desc prstrp_item_s::desc = {
    {seq_table_prstrp_item_s_fields, 4, UINT64_C(0x1), seq_table_prstrp_item_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_prstrp_item_s_names};

// PRSTransmissionTRPItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_prs_tx_trp_item_s_fields[] = {
    {offsetof(prs_tx_trp_item_s, trp_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(65535),
     nullptr},
    {offsetof(prs_tx_trp_item_s, prs_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_cfg_s::desc},
    {offsetof(prs_tx_trp_item_s, ie_exts),
     (int32_t)offsetof(prs_tx_trp_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prs_tx_trp_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_prs_tx_trp_item_s_names[] = {{"tRP-ID", nullptr},
                                                                            {"pRSConfiguration", nullptr},
                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_prs_tx_trp_item_s_opts[]  = {
    {2, (int32_t)offsetof(prs_tx_trp_item_s, ie_exts_present)}};
const asn1::table::seq_desc prs_tx_trp_item_s::desc = {
    {seq_table_prs_tx_trp_item_s_fields, 3, UINT64_C(0x3), seq_table_prs_tx_trp_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_prs_tx_trp_item_s_names};

// PathlossReferenceSignal ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_pathloss_ref_sig_c_alternatives[] = {
    &asn1::table::alternative_ops<ssb_s, false>::value,
    &asn1::table::alternative_ops<dl_prs_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<pathloss_ref_sig_ext_ie_o>, false>::value};
const asn1::table::choice_storage_desc pathloss_ref_sig_c::storage_desc = {
    choice_storage_pathloss_ref_sig_c_alternatives,
    3,
    "pathloss_ref_sig_c"};

ssb_s& pathloss_ref_sig_c::set_ssb_ref()
{
  set(types::ssb_ref);
  return c.get<ssb_s>();
}
dl_prs_s& pathloss_ref_sig_c::set_dl_prs_ref()
{
  set(types::dl_prs_ref);
  return c.get<dl_prs_s>();
}
protocol_ie_single_container_s<pathloss_ref_sig_ext_ie_o>& pathloss_ref_sig_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<pathloss_ref_sig_ext_ie_o>>();
}
static const asn1::table::seq_field choice_table_pathloss_ref_sig_c_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::sequence, false, false, 0, 0, nullptr, &ssb_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &dl_prs_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<pathloss_ref_sig_ext_ie_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_pathloss_ref_sig_c_names[] = {{"sSB-Reference", nullptr},
                                                                                {"dL-PRS-Reference", nullptr},
                                                                                {"choice-Extension", nullptr}};
const asn1::table::choice_desc pathloss_ref_sig_c::desc = {asn1::table::enum_int_type<pathloss_ref_sig_c::types>(),
                                                           pathloss_ref_sig_c::types::nof_types,
                                                           pathloss_ref_sig_c::types::nof_exts,
                                                           pathloss_ref_sig_c::types::has_ext,
                                                           true,
                                                           choice_table_pathloss_ref_sig_c_alternatives,
                                                           nullptr,
                                                           choice_table_pathloss_ref_sig_c_names,
                                                           "pathloss_ref_sig_c",
                                                           offsetof(pathloss_ref_sig_c, c.type),
                                                           offsetof(pathloss_ref_sig_c, c.value),
                                                           false,
                                                           &pathloss_ref_sig_c::storage_desc};

static const char*           enum_names_pathloss_ref_sig_c_types_values[] = {"sSB-Reference",
                                                                             "dL-PRS-Reference",
                                                                             "choice-Extension"};
const asn1::table::enum_desc pathloss_ref_sig_c::types_opts::desc         = {enum_names_pathloss_ref_sig_c_types_values,
                                                                             3,
                                                                             "pathloss_ref_sig_c::types"};

// PathlossReferenceInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_pathloss_ref_info_s_fields[] = {
    {offsetof(pathloss_ref_info_s, pathloss_ref_sig),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &pathloss_ref_sig_c::desc},
    {offsetof(pathloss_ref_info_s, ie_exts),
     (int32_t)offsetof(pathloss_ref_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pathloss_ref_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pathloss_ref_info_s_names[] = {{"pathlossReferenceSignal", nullptr},
                                                                              {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_pathloss_ref_info_s_opts[]  = {
    {1, (int32_t)offsetof(pathloss_ref_info_s, ie_exts_present)}};
const asn1::table::seq_desc pathloss_ref_info_s::desc = {
    {seq_table_pathloss_ref_info_s_fields, 2, UINT64_C(0x1), seq_table_pathloss_ref_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_pathloss_ref_info_s_names};

// PeriodicityItem ::= ENUMERATED
static const char* enum_names_periodicity_item_e_values[] = {
    "ms0dot125", "ms0dot25", "ms0dot5", "ms0dot625", "ms1",    "ms1dot25", "ms2",    "ms2dot5", "ms4dot",
    "ms5",       "ms8",      "ms10",    "ms16",      "ms20",   "ms32",     "ms40",   "ms64",    "ms80m",
    "ms160",     "ms320",    "ms640m",  "ms1280",    "ms2560", "ms5120",   "ms10240"};
const asn1::table::enum_desc periodicity_item_opts::desc = {enum_names_periodicity_item_e_values,
                                                            25,
                                                            "periodicity_item_e"};
float                        periodicity_item_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25,  0.5,   0.625,  1.0,    1.25,   2.0,    2.5,  4.0,
                                  5.0,   8.0,   10.0,  16.0,   20.0,   32.0,   40.0,   64.0, 80.0,
                                  160.0, 320.0, 640.0, 1280.0, 2560.0, 5120.0, 10240.0};
  return map_enum_number(numbers, 25, value, "periodicity_item_e");
}
const char* periodicity_item_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.125", "0.25", "0.5", "0.625", "1",    "1.25", "2",    "2.5", "4",
                                      "5",     "8",    "10",  "16",    "20",   "32",   "40",   "64",  "80",
                                      "160",   "320",  "640", "1280",  "2560", "5120", "10240"};
  return convert_enum_idx(number_strs, 25, value, "periodicity_item_e");
}

// PosSRSInfo ::= SEQUENCE
static const asn1::table::seq_field  seq_table_pos_srs_info_s_fields[] = {{offsetof(pos_srs_info_s, pos_srs_res_id),
                                                                           asn1::table::seq_field::mandatory,
                                                                           asn1::table::field_op::int_u8,
                                                                           false,
                                                                           true,
                                                                           INT64_C(0),
                                                                           UINT64_C(63),
                                                                           nullptr}};
static const asn1::table::field_name seq_table_pos_srs_info_s_names[]  = {{"posSRSResourceID", nullptr}};
const asn1::table::seq_desc pos_srs_info_s::desc = {{seq_table_pos_srs_info_s_fields, 1, UINT64_C(0x1), nullptr, 0},
                                                    true,
                                                    true,
                                                    nullptr,
                                                    0,
                                                    seq_table_pos_srs_info_s_names};

// PosValidityAreaCell-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pos_validity_area_cell_item_s_fields[] = {
    {offsetof(pos_validity_area_cell_item_s, nr_cgi),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_nr_s::desc},
    {offsetof(pos_validity_area_cell_item_s, nr_pci),
     (int32_t)offsetof(pos_validity_area_cell_item_s, nr_pci_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(pos_validity_area_cell_item_s, ie_exts),
     (int32_t)offsetof(pos_validity_area_cell_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pos_validity_area_cell_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pos_validity_area_cell_item_s_names[] = {{"nR-CGI", nullptr},
                                                                                        {"nR-PCI", nullptr},
                                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_pos_validity_area_cell_item_s_opts[]  = {
    {1, (int32_t)offsetof(pos_validity_area_cell_item_s, nr_pci_present)},
    {2, (int32_t)offsetof(pos_validity_area_cell_item_s, ie_exts_present)}};
const asn1::table::seq_desc pos_validity_area_cell_item_s::desc = {
    {seq_table_pos_validity_area_cell_item_s_fields, 3, UINT64_C(0x1), seq_table_pos_validity_area_cell_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_pos_validity_area_cell_item_s_names};

// ReferenceSignal ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ref_sig_c_alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<ssb_s, false>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<dl_prs_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<ref_sig_ext_ie_o>, false>::value};
const asn1::table::choice_storage_desc ref_sig_c::storage_desc = {choice_storage_ref_sig_c_alternatives,
                                                                  6,
                                                                  "ref_sig_c"};

uint8_t& ref_sig_c::set_nzp_csi_rs()
{
  set(types::nzp_csi_rs);
  return c.get<uint8_t>();
}
ssb_s& ref_sig_c::set_ssb()
{
  set(types::ssb);
  return c.get<ssb_s>();
}
uint8_t& ref_sig_c::set_srs()
{
  set(types::srs);
  return c.get<uint8_t>();
}
uint8_t& ref_sig_c::set_positioning_srs()
{
  set(types::positioning_srs);
  return c.get<uint8_t>();
}
dl_prs_s& ref_sig_c::set_dl_prs()
{
  set(types::dl_prs);
  return c.get<dl_prs_s>();
}
protocol_ie_single_container_s<ref_sig_ext_ie_o>& ref_sig_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<ref_sig_ext_ie_o>>();
}
static const asn1::table::seq_field choice_table_ref_sig_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(191),
     nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::sequence, false, false, 0, 0, nullptr, &ssb_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &dl_prs_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<ref_sig_ext_ie_o>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_ref_sig_c_names[] = {{"nZP-CSI-RS", nullptr},
                                                                       {"sSB", nullptr},
                                                                       {"sRS", nullptr},
                                                                       {"positioningSRS", nullptr},
                                                                       {"dL-PRS", nullptr},
                                                                       {"choice-Extension", nullptr}};
const asn1::table::choice_desc       ref_sig_c::desc                = {asn1::table::enum_int_type<ref_sig_c::types>(),
                                                                       ref_sig_c::types::nof_types,
                                                                       ref_sig_c::types::nof_exts,
                                                                       ref_sig_c::types::has_ext,
                                                                       true,
                                                                       choice_table_ref_sig_c_alternatives,
                                                                       nullptr,
                                                                       choice_table_ref_sig_c_names,
                                                                       "ref_sig_c",
                                                                       offsetof(ref_sig_c, c.type),
                                                                       offsetof(ref_sig_c, c.value),
                                                                       false,
                                                                       &ref_sig_c::storage_desc};

static const char* enum_names_ref_sig_c_types_values[] =
    {"nZP-CSI-RS", "sSB", "sRS", "positioningSRS", "dL-PRS", "choice-Extension"};
const asn1::table::enum_desc ref_sig_c::types_opts::desc = {enum_names_ref_sig_c_types_values, 6, "ref_sig_c::types"};

// Remote-UE-Indication ::= ENUMERATED
static const char*           enum_names_remote_ue_ind_e_values[] = {"l2-u2n-remote-ue"};
const asn1::table::enum_desc remote_ue_ind_opts::desc = {enum_names_remote_ue_ind_e_values, 1, "remote_ue_ind_e"};
uint8_t                      remote_ue_ind_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "remote_ue_ind_e");
}

// Remote-UE-Indication-Request ::= ENUMERATED
static const char*           enum_names_remote_ue_ind_request_e_values[] = {"true"};
const asn1::table::enum_desc remote_ue_ind_request_opts::desc            = {enum_names_remote_ue_ind_request_e_values,
                                                                            1,
                                                                            "remote_ue_ind_request_e"};

// Remote-UE-Status ::= ENUMERATED
static const char*           enum_names_remote_ue_status_e_values[] = {"l2-u2n-remote-ue", "no"};
const asn1::table::enum_desc remote_ue_status_opts::desc            = {enum_names_remote_ue_status_e_values,
                                                                       2,
                                                                       "remote_ue_status_e"};
uint8_t                      remote_ue_status_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "remote_ue_status_e");
}

// ReportCharacteristics ::= ENUMERATED
static const char*           enum_names_report_characteristics_e_values[] = {"onDemand", "periodic"};
const asn1::table::enum_desc report_characteristics_opts::desc            = {enum_names_report_characteristics_e_values,
                                                                             2,
                                                                             "report_characteristics_e"};

// RequestType ::= ENUMERATED
static const char*           enum_names_request_type_e_values[] = {"activate", "deactivate"};
const asn1::table::enum_desc request_type_opts::desc = {enum_names_request_type_e_values, 2, "request_type_e"};

// SpatialRelationforResourceIDItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_spatial_relationfor_res_id_item_s_fields[] = {
    {offsetof(spatial_relationfor_res_id_item_s, ref_sig),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ref_sig_c::desc},
    {offsetof(spatial_relationfor_res_id_item_s, ie_exts),
     (int32_t)offsetof(spatial_relationfor_res_id_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(spatial_relationfor_res_id_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_spatial_relationfor_res_id_item_s_names[] = {
    {"referenceSignal", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_spatial_relationfor_res_id_item_s_opts[] = {
    {1, (int32_t)offsetof(spatial_relationfor_res_id_item_s, ie_exts_present)}};
const asn1::table::seq_desc spatial_relationfor_res_id_item_s::desc = {
    {seq_table_spatial_relationfor_res_id_item_s_fields,
     2,
     UINT64_C(0x1),
     seq_table_spatial_relationfor_res_id_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_spatial_relationfor_res_id_item_s_names};

// SpatialRelationInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_spatial_relation_info_s_fields[] = {
    {offsetof(spatial_relation_info_s, spatial_relationfor_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(spatial_relation_info_s::spatial_relationfor_res_id),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &spatial_relationfor_res_id_item_s::desc},
    {offsetof(spatial_relation_info_s, ie_exts),
     (int32_t)offsetof(spatial_relation_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(spatial_relation_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_spatial_relation_info_s_names[] = {
    {"spatialRelationforResourceID", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_spatial_relation_info_s_opts[] = {
    {1, (int32_t)offsetof(spatial_relation_info_s, ie_exts_present)}};
const asn1::table::seq_desc spatial_relation_info_s::desc = {
    {seq_table_spatial_relation_info_s_fields, 2, UINT64_C(0x1), seq_table_spatial_relation_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_spatial_relation_info_s_names};

// SpatialRelationPerSRSResourceItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_spatial_relation_per_srs_res_item_s_fields[] = {
    {offsetof(spatial_relation_per_srs_res_item_s, ref_sig),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ref_sig_c::desc},
    {offsetof(spatial_relation_per_srs_res_item_s, ie_exts),
     (int32_t)offsetof(spatial_relation_per_srs_res_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(spatial_relation_per_srs_res_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_spatial_relation_per_srs_res_item_s_names[] = {
    {"referenceSignal", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_spatial_relation_per_srs_res_item_s_opts[] = {
    {1, (int32_t)offsetof(spatial_relation_per_srs_res_item_s, ie_exts_present)}};
const asn1::table::seq_desc spatial_relation_per_srs_res_item_s::desc = {
    {seq_table_spatial_relation_per_srs_res_item_s_fields,
     2,
     UINT64_C(0x1),
     seq_table_spatial_relation_per_srs_res_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_spatial_relation_per_srs_res_item_s_names};

// SpatialRelationPerSRSResource ::= SEQUENCE
static const asn1::table::seq_field seq_table_spatial_relation_per_srs_res_s_fields[] = {
    {offsetof(spatial_relation_per_srs_res_s, spatial_relation_per_srs_res_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16,
     &asn1::table::item_list_codec<decltype(spatial_relation_per_srs_res_s::spatial_relation_per_srs_res_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &spatial_relation_per_srs_res_item_s::desc},
    {offsetof(spatial_relation_per_srs_res_s, ie_exts),
     (int32_t)offsetof(spatial_relation_per_srs_res_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(spatial_relation_per_srs_res_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_spatial_relation_per_srs_res_s_names[] = {
    {"spatialRelationPerSRSResource-List", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_spatial_relation_per_srs_res_s_opts[] = {
    {1, (int32_t)offsetof(spatial_relation_per_srs_res_s, ie_exts_present)}};
const asn1::table::seq_desc spatial_relation_per_srs_res_s::desc = {{seq_table_spatial_relation_per_srs_res_s_fields,
                                                                     2,
                                                                     UINT64_C(0x1),
                                                                     seq_table_spatial_relation_per_srs_res_s_opts,
                                                                     1},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_spatial_relation_per_srs_res_s_names};

static const uint32_t objset_id_srs_res_set_item_ext_ies_o[]   = {63};
static const uint8_t  objset_crit_srs_res_set_item_ext_ies_o[] = {crit_e::ignore};

// SRSResourceSet-Item-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_srs_res_set_item_ext_ies_container_values[] = {
    {offsetof(srs_res_set_item_ext_ies_container, srs_spatial_relation_per_srs_res),
     (int32_t)offsetof(srs_res_set_item_ext_ies_container, srs_spatial_relation_per_srs_res_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &spatial_relation_per_srs_res_s::desc}};
static const int32_t ie_table_srs_res_set_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(srs_res_set_item_ext_ies_container, srs_spatial_relation_per_srs_res_present)};
const asn1::table::ie_container_desc srs_res_set_item_ext_ies_container::desc = {
    objset_id_srs_res_set_item_ext_ies_o,
    objset_crit_srs_res_set_item_ext_ies_o,
    ie_table_srs_res_set_item_ext_ies_container_values,
    ie_table_srs_res_set_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// SRSResourceSet-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_res_set_item_s_fields[] = {
    {offsetof(srs_res_set_item_s, nof_srs_res_per_set),
     (int32_t)offsetof(srs_res_set_item_s, nof_srs_res_per_set_present),
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(16),
     nullptr},
    {offsetof(srs_res_set_item_s, periodicity_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16,
     &asn1::table::list_codec<decltype(srs_res_set_item_s::periodicity_list), asn1::table::string_json>::value},
    {offsetof(srs_res_set_item_s, spatial_relation_info),
     (int32_t)offsetof(srs_res_set_item_s, spatial_relation_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &spatial_relation_info_s::desc},
    {offsetof(srs_res_set_item_s, pathloss_ref_info),
     (int32_t)offsetof(srs_res_set_item_s, pathloss_ref_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pathloss_ref_info_s::desc},
    {offsetof(srs_res_set_item_s, ie_exts),
     (int32_t)offsetof(srs_res_set_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_res_set_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_srs_res_set_item_s_names[] = {{"numberOfSRSResourcePerSet", nullptr},
                                                                             {"periodicityList", nullptr},
                                                                             {"spatialRelationInformation", nullptr},
                                                                             {"pathlossReferenceInformation", nullptr},
                                                                             {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_res_set_item_s_opts[]  = {
    {0, (int32_t)offsetof(srs_res_set_item_s, nof_srs_res_per_set_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(srs_res_set_item_s, spatial_relation_info_present)},
    {3, (int32_t)offsetof(srs_res_set_item_s, pathloss_ref_info_present)},
    {4, (int32_t)offsetof(srs_res_set_item_s, ie_exts_present)}};
const asn1::table::seq_desc srs_res_set_item_s::desc = {
    {seq_table_srs_res_set_item_s_fields, 5, UINT64_C(0x0), seq_table_srs_res_set_item_s_opts, 5},
    true,
    true,
    nullptr,
    0,
    seq_table_srs_res_set_item_s_names};

// SSBBurstPosition ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ssb_burst_position_c_alternatives[] = {
    &asn1::table::alternative_ops<fixed_bitstring<4, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<8, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<64, false, true>, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<ssb_burst_position_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc ssb_burst_position_c::storage_desc = {
    choice_storage_ssb_burst_position_c_alternatives,
    4,
    "ssb_burst_position_c"};

fixed_bitstring<4, false, true>& ssb_burst_position_c::set_short_bitmap()
{
  set(types::short_bitmap);
  return c.get<fixed_bitstring<4, false, true>>();
}
fixed_bitstring<8, false, true>& ssb_burst_position_c::set_medium_bitmap()
{
  set(types::medium_bitmap);
  return c.get<fixed_bitstring<8, false, true>>();
}
fixed_bitstring<64, false, true>& ssb_burst_position_c::set_long_bitmap()
{
  set(types::long_bitmap);
  return c.get<fixed_bitstring<64, false, true>>();
}
protocol_ie_single_container_s<ssb_burst_position_ext_ies_o>& ssb_burst_position_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<ssb_burst_position_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_ssb_burst_position_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<4, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<8, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<64, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<ssb_burst_position_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_ssb_burst_position_c_names[] = {{"shortBitmap", nullptr},
                                                                                  {"mediumBitmap", nullptr},
                                                                                  {"longBitmap", nullptr},
                                                                                  {"choice-extension", nullptr}};
const asn1::table::choice_desc ssb_burst_position_c::desc = {asn1::table::enum_int_type<ssb_burst_position_c::types>(),
                                                             ssb_burst_position_c::types::nof_types,
                                                             ssb_burst_position_c::types::nof_exts,
                                                             ssb_burst_position_c::types::has_ext,
                                                             true,
                                                             choice_table_ssb_burst_position_c_alternatives,
                                                             nullptr,
                                                             choice_table_ssb_burst_position_c_names,
                                                             "ssb_burst_position_c",
                                                             offsetof(ssb_burst_position_c, c.type),
                                                             offsetof(ssb_burst_position_c, c.value),
                                                             false,
                                                             &ssb_burst_position_c::storage_desc};

static const char*           enum_names_ssb_burst_position_c_types_values[] = {"shortBitmap",
                                                                               "mediumBitmap",
                                                                               "longBitmap",
                                                                               "choice-extension"};
const asn1::table::enum_desc ssb_burst_position_c::types_opts::desc = {enum_names_ssb_burst_position_c_types_values,
                                                                       4,
                                                                       "ssb_burst_position_c::types"};

// TF-Configuration ::= SEQUENCE
static const asn1::table::seq_field seq_table_tf_cfg_s_fields[] = {
    {offsetof(tf_cfg_s, ssb_freq),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(tf_cfg_s, ssb_subcarrier_spacing),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(tf_cfg_s::ssb_subcarrier_spacing)>(),
     decltype(tf_cfg_s::ssb_subcarrier_spacing)::has_ext,
     false,
     decltype(tf_cfg_s::ssb_subcarrier_spacing)::nof_types,
     decltype(tf_cfg_s::ssb_subcarrier_spacing)::nof_exts,
     nullptr,
     &tf_cfg_s::ssb_subcarrier_spacing_opts::desc},
    {offsetof(tf_cfg_s, ssb_tx_pwr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     true,
     INT64_C(-60),
     UINT64_C(50),
     nullptr},
    {offsetof(tf_cfg_s, ssb_periodicity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(tf_cfg_s::ssb_periodicity)>(),
     decltype(tf_cfg_s::ssb_periodicity)::has_ext,
     false,
     decltype(tf_cfg_s::ssb_periodicity)::nof_types,
     decltype(tf_cfg_s::ssb_periodicity)::nof_exts,
     nullptr,
     &tf_cfg_s::ssb_periodicity_opts::desc},
    {offsetof(tf_cfg_s, ssb_half_frame_offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(1),
     nullptr},
    {offsetof(tf_cfg_s, ssb_sfn_offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(15),
     nullptr},
    {offsetof(tf_cfg_s, ssb_burst_position),
     (int32_t)offsetof(tf_cfg_s, ssb_burst_position_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ssb_burst_position_c::desc},
    {offsetof(tf_cfg_s, sfn_initisation_time),
     (int32_t)offsetof(tf_cfg_s, sfn_initisation_time_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tf_cfg_s::sfn_initisation_time), asn1::table::struct_json>::value},
    {offsetof(tf_cfg_s, ie_exts),
     (int32_t)offsetof(tf_cfg_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tf_cfg_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_tf_cfg_s_names[] = {{"sSB-frequency", nullptr},
                                                                   {"sSB-subcarrier-spacing", nullptr},
                                                                   {"sSB-Transmit-power", nullptr},
                                                                   {"sSB-periodicity", nullptr},
                                                                   {"sSB-half-frame-offset", nullptr},
                                                                   {"sSB-SFN-offset", nullptr},
                                                                   {"sSB-BurstPosition", nullptr},
                                                                   {"sFN-initialisation-time", nullptr},
                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_tf_cfg_s_opts[]  = {
    {6, (int32_t)offsetof(tf_cfg_s, ssb_burst_position_present)},
    {7, (int32_t)offsetof(tf_cfg_s, sfn_initisation_time_present)},
    {8, (int32_t)offsetof(tf_cfg_s, ie_exts_present)}};
const asn1::table::seq_desc tf_cfg_s::desc = {
    {seq_table_tf_cfg_s_fields, 9, UINT64_C(0x3f), seq_table_tf_cfg_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_tf_cfg_s_names};

static const char* enum_names_tf_cfg_s_ssb_subcarrier_spacing_e__values[] =
    {"kHz15", "kHz30", "kHz120", "kHz240", "kHz60", "kHz480", "kHz960"};
const asn1::table::enum_desc tf_cfg_s::ssb_subcarrier_spacing_opts::desc = {
    enum_names_tf_cfg_s_ssb_subcarrier_spacing_e__values,
    7,
    "tf_cfg_s::ssb_subcarrier_spacing_e_"};
uint16_t tf_cfg_s::ssb_subcarrier_spacing_opts::to_number() const
{
  static const uint16_t numbers[] = {15, 30, 120, 240, 60, 480, 960};
  return map_enum_number(numbers, 7, value, "tf_cfg_s::ssb_subcarrier_spacing_e_");
}

static const char* enum_names_tf_cfg_s_ssb_periodicity_e__values[] = {"ms5", "ms10", "ms20", "ms40", "ms80", "ms160"};
const asn1::table::enum_desc tf_cfg_s::ssb_periodicity_opts::desc  = {enum_names_tf_cfg_s_ssb_periodicity_e__values,
                                                                      6,
                                                                      "tf_cfg_s::ssb_periodicity_e_"};
uint8_t                      tf_cfg_s::ssb_periodicity_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(numbers, 6, value, "tf_cfg_s::ssb_periodicity_e_");
}

// SSBInfoItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_ssb_info_item_s_fields[] = {
    {offsetof(ssb_info_item_s, ssb_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tf_cfg_s::desc},
    {offsetof(ssb_info_item_s, pci_nr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(ssb_info_item_s, ie_exts),
     (int32_t)offsetof(ssb_info_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ssb_info_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ssb_info_item_s_names[] = {{"sSB-Configuration", nullptr},
                                                                          {"pCI-NR", nullptr},
                                                                          {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ssb_info_item_s_opts[]  = {
    {2, (int32_t)offsetof(ssb_info_item_s, ie_exts_present)}};
const asn1::table::seq_desc ssb_info_item_s::desc = {
    {seq_table_ssb_info_item_s_fields, 3, UINT64_C(0x3), seq_table_ssb_info_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_ssb_info_item_s_names};

// SSBInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_ssb_info_s_fields[] = {
    {offsetof(ssb_info_s, list_of_ssb_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::
         item_list_codec<decltype(ssb_info_s::list_of_ssb_info), asn1::table::sequence_item_ops, false, true>::value,
     &ssb_info_item_s::desc},
    {offsetof(ssb_info_s, ie_exts),
     (int32_t)offsetof(ssb_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ssb_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ssb_info_s_names[] = {{"listOfSSBInfo", nullptr},
                                                                     {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_ssb_info_s_opts[] = {{1, (int32_t)offsetof(ssb_info_s, ie_exts_present)}};
const asn1::table::seq_desc       ssb_info_s::desc            = {
    {seq_table_ssb_info_s_fields, 2, UINT64_C(0x1), seq_table_ssb_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_ssb_info_s_names};

// ResourceMapping ::= SEQUENCE
static const asn1::table::seq_field seq_table_res_map_s_fields[] = {
    {offsetof(res_map_s, start_position),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(13),
     nullptr},
    {offsetof(res_map_s, nrof_sumbols),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(res_map_s::nrof_sumbols)>(),
     decltype(res_map_s::nrof_sumbols)::has_ext,
     false,
     decltype(res_map_s::nrof_sumbols)::nof_types,
     decltype(res_map_s::nrof_sumbols)::nof_exts,
     nullptr,
     &res_map_s::nrof_sumbols_opts::desc},
    {offsetof(res_map_s, ie_exts),
     (int32_t)offsetof(res_map_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(res_map_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_res_map_s_names[] = {{"startPosition", nullptr},
                                                                    {"nrofSumbols", nullptr},
                                                                    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_res_map_s_opts[]  = {{2, (int32_t)offsetof(res_map_s, ie_exts_present)}};
const asn1::table::seq_desc          res_map_s::desc             = {
    {seq_table_res_map_s_fields, 3, UINT64_C(0x3), seq_table_res_map_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_res_map_s_names};

static const char*           enum_names_res_map_s_nrof_sumbols_e__values[] = {"n1", "n2", "n4", "n8", "n12"};
const asn1::table::enum_desc res_map_s::nrof_sumbols_opts::desc = {enum_names_res_map_s_nrof_sumbols_e__values,
                                                                   5,
                                                                   "res_map_s::nrof_sumbols_e_"};
uint8_t                      res_map_s::nrof_sumbols_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 12};
  return map_enum_number(numbers, 5, value, "res_map_s::nrof_sumbols_e_");
}

// ValidityAreaSpecificSRSInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_validity_area_specific_srs_info_s_fields[] = {
    {offsetof(validity_area_specific_srs_info_s, tx_comb_pos),
     (int32_t)offsetof(validity_area_specific_srs_info_s, tx_comb_pos_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &tx_comb_pos_c::desc},
    {offsetof(validity_area_specific_srs_info_s, res_map),
     (int32_t)offsetof(validity_area_specific_srs_info_s, res_map_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &res_map_s::desc},
    {offsetof(validity_area_specific_srs_info_s, freq_domain_shift),
     (int32_t)offsetof(validity_area_specific_srs_info_s, freq_domain_shift_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(268),
     nullptr},
    {offsetof(validity_area_specific_srs_info_s, c_srs),
     (int32_t)offsetof(validity_area_specific_srs_info_s, c_srs_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(validity_area_specific_srs_info_s, res_type_pos),
     (int32_t)offsetof(validity_area_specific_srs_info_s, res_type_pos_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &res_type_pos_c::desc},
    {offsetof(validity_area_specific_srs_info_s, seq_id_pos),
     (int32_t)offsetof(validity_area_specific_srs_info_s, seq_id_pos_present),
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(validity_area_specific_srs_info_s, ie_exts),
     (int32_t)offsetof(validity_area_specific_srs_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(validity_area_specific_srs_info_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_validity_area_specific_srs_info_s_names[] = {
    {"transmissionCombPos", nullptr},
    {"resourceMapping", nullptr},
    {"freqDomainShift", nullptr},
    {"c-SRS", nullptr},
    {"resourceTypePos", nullptr},
    {"sequenceIDPos", nullptr},
    {"iE-extensions", nullptr}};
static const asn1::table::seq_opt seq_table_validity_area_specific_srs_info_s_opts[] = {
    {0, (int32_t)offsetof(validity_area_specific_srs_info_s, tx_comb_pos_present)},
    {1, (int32_t)offsetof(validity_area_specific_srs_info_s, res_map_present)},
    {2, (int32_t)offsetof(validity_area_specific_srs_info_s, freq_domain_shift_present)},
    {3, (int32_t)offsetof(validity_area_specific_srs_info_s, c_srs_present)},
    {4, (int32_t)offsetof(validity_area_specific_srs_info_s, res_type_pos_present)},
    {5, (int32_t)offsetof(validity_area_specific_srs_info_s, seq_id_pos_present)},
    {6, (int32_t)offsetof(validity_area_specific_srs_info_s, ie_exts_present)}};
const asn1::table::seq_desc validity_area_specific_srs_info_s::desc = {
    {seq_table_validity_area_specific_srs_info_s_fields,
     7,
     UINT64_C(0x0),
     seq_table_validity_area_specific_srs_info_s_opts,
     7},
    true,
    true,
    nullptr,
    0,
    seq_table_validity_area_specific_srs_info_s_names};

// ValidityAreaSpecificSRSInformationExtended ::= SEQUENCE
static const asn1::table::seq_field seq_table_validity_area_specific_srs_info_extended_s_fields[] = {
    {offsetof(validity_area_specific_srs_info_extended_s, pos_srs_res_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(validity_area_specific_srs_info_extended_s::pos_srs_res_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pos_srs_res_item_s::desc},
    {offsetof(validity_area_specific_srs_info_extended_s, pos_srs_res_set_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16,
     &asn1::table::item_list_codec<decltype(validity_area_specific_srs_info_extended_s::pos_srs_res_set_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pos_srs_res_set_item_s::desc},
    {offsetof(validity_area_specific_srs_info_extended_s, ie_exts),
     (int32_t)offsetof(validity_area_specific_srs_info_extended_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(validity_area_specific_srs_info_extended_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_validity_area_specific_srs_info_extended_s_names[] = {
    {"posSRSResource-List", nullptr},
    {"posSRSResourceSet-List", nullptr},
    {"iE-extensions", nullptr}};
static const asn1::table::seq_opt seq_table_validity_area_specific_srs_info_extended_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(validity_area_specific_srs_info_extended_s, ie_exts_present)}};
const asn1::table::seq_desc validity_area_specific_srs_info_extended_s::desc = {
    {seq_table_validity_area_specific_srs_info_extended_s_fields,
     3,
     UINT64_C(0x0),
     seq_table_validity_area_specific_srs_info_extended_s_opts,
     3},
    true,
    true,
    nullptr,
    0,
    seq_table_validity_area_specific_srs_info_extended_s_names};

static const uint32_t objset_id_requested_srs_tx_characteristics_ext_ies_o[]   = {61, 121, 128, 135, 166};
static const uint8_t  objset_crit_requested_srs_tx_characteristics_ext_ies_o[] = {crit_e::ignore,
                                                                                  crit_e::ignore,
                                                                                  crit_e::ignore,
                                                                                  crit_e::ignore,
                                                                                  crit_e::ignore};

// RequestedSRSTransmissionCharacteristics-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_requested_srs_tx_characteristics_ext_ies_container_values[] = {
    {offsetof(requested_srs_tx_characteristics_ext_ies_container, srs_freq),
     (int32_t)offsetof(requested_srs_tx_characteristics_ext_ies_container, srs_freq_present),
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(requested_srs_tx_characteristics_ext_ies_container, bw_aggregation_request_ind),
     (int32_t)offsetof(requested_srs_tx_characteristics_ext_ies_container, bw_aggregation_request_ind_present),
     asn1::table::enum_op<decltype(requested_srs_tx_characteristics_ext_ies_container::bw_aggregation_request_ind)>(),
     decltype(requested_srs_tx_characteristics_ext_ies_container::bw_aggregation_request_ind)::has_ext,
     false,
     decltype(requested_srs_tx_characteristics_ext_ies_container::bw_aggregation_request_ind)::nof_types,
     decltype(requested_srs_tx_characteristics_ext_ies_container::bw_aggregation_request_ind)::nof_exts,
     nullptr,
     &bw_aggregation_request_ind_opts::desc},
    {offsetof(requested_srs_tx_characteristics_ext_ies_container, pos_validity_area_cell_list),
     (int32_t)offsetof(requested_srs_tx_characteristics_ext_ies_container, pos_validity_area_cell_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<
         decltype(requested_srs_tx_characteristics_ext_ies_container::pos_validity_area_cell_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &pos_validity_area_cell_item_s::desc},
    {offsetof(requested_srs_tx_characteristics_ext_ies_container, validity_area_specific_srs_info),
     (int32_t)offsetof(requested_srs_tx_characteristics_ext_ies_container, validity_area_specific_srs_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &validity_area_specific_srs_info_s::desc},
    {offsetof(requested_srs_tx_characteristics_ext_ies_container, validity_area_specific_srs_info_extended),
     (int32_t)offsetof(requested_srs_tx_characteristics_ext_ies_container,
                       validity_area_specific_srs_info_extended_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &validity_area_specific_srs_info_extended_s::desc}};
static const int32_t ie_table_requested_srs_tx_characteristics_ext_ies_container_presences[] = {
    (int32_t)offsetof(requested_srs_tx_characteristics_ext_ies_container, srs_freq_present),
    (int32_t)offsetof(requested_srs_tx_characteristics_ext_ies_container, bw_aggregation_request_ind_present),
    (int32_t)offsetof(requested_srs_tx_characteristics_ext_ies_container, pos_validity_area_cell_list_present),
    (int32_t)offsetof(requested_srs_tx_characteristics_ext_ies_container, validity_area_specific_srs_info_present),
    (int32_t)offsetof(requested_srs_tx_characteristics_ext_ies_container,
                      validity_area_specific_srs_info_extended_present)};
const asn1::table::ie_container_desc requested_srs_tx_characteristics_ext_ies_container::desc = {
    objset_id_requested_srs_tx_characteristics_ext_ies_o,
    objset_crit_requested_srs_tx_characteristics_ext_ies_o,
    ie_table_requested_srs_tx_characteristics_ext_ies_container_values,
    ie_table_requested_srs_tx_characteristics_ext_ies_container_presences,
    5,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// RequestedSRSTransmissionCharacteristics ::= SEQUENCE
static const asn1::table::seq_field seq_table_requested_srs_tx_characteristics_s_fields[] = {
    {offsetof(requested_srs_tx_characteristics_s, nof_txs),
     (int32_t)offsetof(requested_srs_tx_characteristics_s, nof_txs_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(500),
     nullptr},
    {offsetof(requested_srs_tx_characteristics_s, res_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(requested_srs_tx_characteristics_s::res_type)>(),
     decltype(requested_srs_tx_characteristics_s::res_type)::has_ext,
     false,
     decltype(requested_srs_tx_characteristics_s::res_type)::nof_types,
     decltype(requested_srs_tx_characteristics_s::res_type)::nof_exts,
     nullptr,
     &requested_srs_tx_characteristics_s::res_type_opts::desc},
    {offsetof(requested_srs_tx_characteristics_s, bw),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &bw_srs_c::desc},
    {offsetof(requested_srs_tx_characteristics_s, list_of_srs_res_set),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16,
     &asn1::table::item_list_codec<decltype(requested_srs_tx_characteristics_s::list_of_srs_res_set),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &srs_res_set_item_s::desc},
    {offsetof(requested_srs_tx_characteristics_s, ssb_info),
     (int32_t)offsetof(requested_srs_tx_characteristics_s, ssb_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ssb_info_s::desc},
    {offsetof(requested_srs_tx_characteristics_s, ie_exts),
     (int32_t)offsetof(requested_srs_tx_characteristics_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &requested_srs_tx_characteristics_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_requested_srs_tx_characteristics_s_names[] = {
    {"numberOfTransmissions", nullptr},
    {"resourceType", nullptr},
    {"bandwidth", nullptr},
    {"listOfSRSResourceSet", nullptr},
    {"sSBInformation", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_requested_srs_tx_characteristics_s_opts[] = {
    {0, (int32_t)offsetof(requested_srs_tx_characteristics_s, nof_txs_present)},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(requested_srs_tx_characteristics_s, ssb_info_present)},
    {5, (int32_t)offsetof(requested_srs_tx_characteristics_s, ie_exts_present)}};
const asn1::table::seq_desc requested_srs_tx_characteristics_s::desc = {
    {seq_table_requested_srs_tx_characteristics_s_fields,
     6,
     UINT64_C(0x6),
     seq_table_requested_srs_tx_characteristics_s_opts,
     4},
    true,
    true,
    nullptr,
    0,
    seq_table_requested_srs_tx_characteristics_s_names};

static const char*           enum_names_requested_srs_tx_characteristics_s_res_type_e__values[] = {"periodic",
                                                                                                   "semi-persistent",
                                                                                                   "aperiodic"};
const asn1::table::enum_desc requested_srs_tx_characteristics_s::res_type_opts::desc            = {
    enum_names_requested_srs_tx_characteristics_s_res_type_e__values,
    3,
    "requested_srs_tx_characteristics_s::res_type_e_"};

// RequestedSRSPreconfigurationCharacteristics-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_requested_srs_precfg_characteristics_item_s_fields[] = {
    {offsetof(requested_srs_precfg_characteristics_item_s, requested_srs_tx_characteristics),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &requested_srs_tx_characteristics_s::desc},
    {offsetof(requested_srs_precfg_characteristics_item_s, ie_exts),
     (int32_t)offsetof(requested_srs_precfg_characteristics_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(requested_srs_precfg_characteristics_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_requested_srs_precfg_characteristics_item_s_names[] = {
    {"requestedSRSTransmissionCharacteristics", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_requested_srs_precfg_characteristics_item_s_opts[] = {
    {1, (int32_t)offsetof(requested_srs_precfg_characteristics_item_s, ie_exts_present)}};
const asn1::table::seq_desc requested_srs_precfg_characteristics_item_s::desc = {
    {seq_table_requested_srs_precfg_characteristics_item_s_fields,
     2,
     UINT64_C(0x1),
     seq_table_requested_srs_precfg_characteristics_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_requested_srs_precfg_characteristics_item_s_names};

// ResponseTime ::= SEQUENCE
static const asn1::table::seq_field seq_table_resp_time_s_fields[] = {
    {offsetof(resp_time_s, time),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(128),
     nullptr},
    {offsetof(resp_time_s, time_unit),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(resp_time_s::time_unit)>(),
     decltype(resp_time_s::time_unit)::has_ext,
     false,
     decltype(resp_time_s::time_unit)::nof_types,
     decltype(resp_time_s::time_unit)::nof_exts,
     nullptr,
     &resp_time_s::time_unit_opts::desc},
    {offsetof(resp_time_s, ie_exts),
     (int32_t)offsetof(resp_time_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(resp_time_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_resp_time_s_names[] = {{"time", nullptr},
                                                                      {"timeUnit", nullptr},
                                                                      {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_resp_time_s_opts[] = {{2, (int32_t)offsetof(resp_time_s, ie_exts_present)}};
const asn1::table::seq_desc       resp_time_s::desc            = {
    {seq_table_resp_time_s_fields, 3, UINT64_C(0x3), seq_table_resp_time_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_resp_time_s_names};

static const char* enum_names_resp_time_s_time_unit_e__values[] = {"second", "ten-seconds", "ten-milliseconds"};
const asn1::table::enum_desc resp_time_s::time_unit_opts::desc  = {enum_names_resp_time_s_time_unit_e__values,
                                                                   3,
                                                                   "resp_time_s::time_unit_e_"};

// SRSCarrier-List-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_carrier_list_item_s_fields[] = {
    {offsetof(srs_carrier_list_item_s, point_a),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(srs_carrier_list_item_s, ul_ch_bw_per_scs_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     5,
     &asn1::table::item_list_codec<decltype(srs_carrier_list_item_s::ul_ch_bw_per_scs_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &scs_specific_carrier_s::desc},
    {offsetof(srs_carrier_list_item_s, active_ul_bwp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &active_ul_bwp_s::desc},
    {offsetof(srs_carrier_list_item_s, pci_nr),
     (int32_t)offsetof(srs_carrier_list_item_s, pci_nr_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(srs_carrier_list_item_s, ie_exts),
     (int32_t)offsetof(srs_carrier_list_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(srs_carrier_list_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_srs_carrier_list_item_s_names[] = {
    {"pointA", nullptr},
    {"uplinkChannelBW-PerSCS-List", nullptr},
    {"activeULBWP", nullptr},
    {"pCI-NR", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_srs_carrier_list_item_s_opts[] = {
    {3, (int32_t)offsetof(srs_carrier_list_item_s, pci_nr_present)},
    {4, (int32_t)offsetof(srs_carrier_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc srs_carrier_list_item_s::desc = {
    {seq_table_srs_carrier_list_item_s_fields, 5, UINT64_C(0x7), seq_table_srs_carrier_list_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_srs_carrier_list_item_s_names};

static const uint32_t objset_id_srs_configuration_ext_ies_o[]   = {122};
static const uint8_t  objset_crit_srs_configuration_ext_ies_o[] = {crit_e::ignore};

// SRSConfiguration-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_srs_configuration_ext_ies_container_values[] = {
    {offsetof(srs_configuration_ext_ies_container, pos_srs_res_set_aggregation_list),
     (int32_t)offsetof(srs_configuration_ext_ies_container, pos_srs_res_set_aggregation_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(srs_configuration_ext_ies_container::pos_srs_res_set_aggregation_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pos_srs_res_set_aggregation_item_s::desc}};
static const int32_t ie_table_srs_configuration_ext_ies_container_presences[] = {
    (int32_t)offsetof(srs_configuration_ext_ies_container, pos_srs_res_set_aggregation_list_present)};
const asn1::table::ie_container_desc srs_configuration_ext_ies_container::desc = {
    objset_id_srs_configuration_ext_ies_o,
    objset_crit_srs_configuration_ext_ies_o,
    ie_table_srs_configuration_ext_ies_container_values,
    ie_table_srs_configuration_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// SRSConfiguration ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_configuration_s_fields[] = {
    {offsetof(srs_configuration_s, srs_carrier_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(srs_configuration_s::srs_carrier_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &srs_carrier_list_item_s::desc},
    {offsetof(srs_configuration_s, ie_exts),
     (int32_t)offsetof(srs_configuration_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_configuration_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_srs_configuration_s_names[] = {{"sRSCarrier-List", nullptr},
                                                                              {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_configuration_s_opts[]  = {
    {1, (int32_t)offsetof(srs_configuration_s, ie_exts_present)}};
const asn1::table::seq_desc srs_configuration_s::desc = {
    {seq_table_srs_configuration_s_fields, 2, UINT64_C(0x1), seq_table_srs_configuration_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_srs_configuration_s_names};

// SRSInfo ::= SEQUENCE
static const asn1::table::seq_field  seq_table_srs_info_s_fields[] = {{offsetof(srs_info_s, srs_res),
                                                                       asn1::table::seq_field::mandatory,
                                                                       asn1::table::field_op::int_u8,
                                                                       false,
                                                                       true,
                                                                       INT64_C(0),
                                                                       UINT64_C(63),
                                                                       nullptr}};
static const asn1::table::field_name seq_table_srs_info_s_names[]  = {{"sRSResource", nullptr}};
const asn1::table::seq_desc          srs_info_s::desc =
    {{seq_table_srs_info_s_fields, 1, UINT64_C(0x1), nullptr, 0}, true, true, nullptr, 0, seq_table_srs_info_s_names};

// SRSPortIndex ::= ENUMERATED
static const char*           enum_names_srs_port_idx_e_values[] = {"id1000", "id1001", "id1002", "id1003"};
const asn1::table::enum_desc srs_port_idx_opts::desc = {enum_names_srs_port_idx_e_values, 4, "srs_port_idx_e"};
uint16_t                     srs_port_idx_opts::to_number() const
{
  static const uint16_t numbers[] = {1000, 1001, 1002, 1003};
  return map_enum_number(numbers, 4, value, "srs_port_idx_e");
}

// SRSPreconfiguration-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_precfg_item_s_fields[] = {
    {offsetof(srs_precfg_item_s, srs_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_configuration_s::desc},
    {offsetof(srs_precfg_item_s, pos_validity_area_cell_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(srs_precfg_item_s::pos_validity_area_cell_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pos_validity_area_cell_item_s::desc},
    {offsetof(srs_precfg_item_s, ie_exts),
     (int32_t)offsetof(srs_precfg_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(srs_precfg_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_srs_precfg_item_s_names[] = {{"sRSConfiguration", nullptr},
                                                                            {"posValidityAreaCellList", nullptr},
                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_precfg_item_s_opts[]  = {
    {2, (int32_t)offsetof(srs_precfg_item_s, ie_exts_present)}};
const asn1::table::seq_desc srs_precfg_item_s::desc = {
    {seq_table_srs_precfg_item_s_fields, 3, UINT64_C(0x3), seq_table_srs_precfg_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_srs_precfg_item_s_names};

// SRSReservationType ::= ENUMERATED
static const char*           enum_names_srs_reserv_type_e_values[] = {"reserve", "release"};
const asn1::table::enum_desc srs_reserv_type_opts::desc = {enum_names_srs_reserv_type_e_values, 2, "srs_reserv_type_e"};

// SRSResourceID-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_res_id_item_s_fields[] = {
    {offsetof(srs_res_id_item_s, srs_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(srs_res_id_item_s, ie_exts),
     (int32_t)offsetof(srs_res_id_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(srs_res_id_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_srs_res_id_item_s_names[] = {{"sRSResourceID", nullptr},
                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_res_id_item_s_opts[]  = {
    {1, (int32_t)offsetof(srs_res_id_item_s, ie_exts_present)}};
const asn1::table::seq_desc srs_res_id_item_s::desc = {
    {seq_table_srs_res_id_item_s_fields, 2, UINT64_C(0x1), seq_table_srs_res_id_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_srs_res_id_item_s_names};

// SRSResourceTrigger ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_res_trigger_s_fields[] = {
    {offsetof(srs_res_trigger_s, aperiodic_srs_res_trigger_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     3,
     &asn1::table::list_codec<decltype(srs_res_trigger_s::aperiodic_srs_res_trigger_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(3), false, true>>::value},
    {offsetof(srs_res_trigger_s, ie_exts),
     (int32_t)offsetof(srs_res_trigger_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(srs_res_trigger_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_srs_res_trigger_s_names[] = {
    {"aperiodicSRSResourceTriggerList", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_srs_res_trigger_s_opts[] = {
    {1, (int32_t)offsetof(srs_res_trigger_s, ie_exts_present)}};
const asn1::table::seq_desc srs_res_trigger_s::desc = {
    {seq_table_srs_res_trigger_s_fields, 2, UINT64_C(0x1), seq_table_srs_res_trigger_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_srs_res_trigger_s_names};

// SRSResourceTypeChoice ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_srs_res_type_choice_c_alternatives[] = {
    &asn1::table::alternative_ops<srs_info_s, false>::value,
    &asn1::table::alternative_ops<pos_srs_info_s, false>::value};
const asn1::table::choice_storage_desc srs_res_type_choice_c::storage_desc = {
    choice_storage_srs_res_type_choice_c_alternatives,
    2,
    "srs_res_type_choice_c"};

srs_info_s& srs_res_type_choice_c::set_srs_res_info()
{
  set(types::srs_res_info);
  return c.get<srs_info_s>();
}
pos_srs_info_s& srs_res_type_choice_c::set_pos_srs_res_info()
{
  set(types::pos_srs_res_info);
  return c.get<pos_srs_info_s>();
}
static const asn1::table::seq_field choice_table_srs_res_type_choice_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_info_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pos_srs_info_s::desc}};
static const asn1::table::field_name choice_table_srs_res_type_choice_c_names[] = {{"sRSResourceInfo", nullptr},
                                                                                   {"posSRSResourceInfo", nullptr}};
const asn1::table::choice_desc       srs_res_type_choice_c::desc                = {
    asn1::table::enum_int_type<srs_res_type_choice_c::types>(),
    srs_res_type_choice_c::types::nof_types,
    srs_res_type_choice_c::types::nof_exts,
    srs_res_type_choice_c::types::has_ext,
    true,
    choice_table_srs_res_type_choice_c_alternatives,
    nullptr,
    choice_table_srs_res_type_choice_c_names,
    "srs_res_type_choice_c",
    offsetof(srs_res_type_choice_c, c.type),
    offsetof(srs_res_type_choice_c, c.value),
    false,
    &srs_res_type_choice_c::storage_desc};

static const char* enum_names_srs_res_type_choice_c_types_values[]   = {"sRSResourceInfo", "posSRSResourceInfo"};
const asn1::table::enum_desc srs_res_type_choice_c::types_opts::desc = {enum_names_srs_res_type_choice_c_types_values,
                                                                        2,
                                                                        "srs_res_type_choice_c::types"};

static const uint32_t objset_id_srs_restype_ext_ies_o[]   = {100};
static const uint8_t  objset_crit_srs_restype_ext_ies_o[] = {crit_e::ignore};

// SRSResourcetype-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_srs_restype_ext_ies_container_values[] = {
    {offsetof(srs_restype_ext_ies_container, srs_port_idx),
     (int32_t)offsetof(srs_restype_ext_ies_container, srs_port_idx_present),
     asn1::table::enum_op<decltype(srs_restype_ext_ies_container::srs_port_idx)>(),
     decltype(srs_restype_ext_ies_container::srs_port_idx)::has_ext,
     false,
     decltype(srs_restype_ext_ies_container::srs_port_idx)::nof_types,
     decltype(srs_restype_ext_ies_container::srs_port_idx)::nof_exts,
     nullptr,
     &srs_port_idx_opts::desc}};
static const int32_t ie_table_srs_restype_ext_ies_container_presences[] = {
    (int32_t)offsetof(srs_restype_ext_ies_container, srs_port_idx_present)};
const asn1::table::ie_container_desc srs_restype_ext_ies_container::desc = {
    objset_id_srs_restype_ext_ies_o,
    objset_crit_srs_restype_ext_ies_o,
    ie_table_srs_restype_ext_ies_container_values,
    ie_table_srs_restype_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// SRSResourcetype ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_restype_s_fields[] = {
    {offsetof(srs_restype_s, srs_res_type_choice),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_res_type_choice_c::desc},
    {offsetof(srs_restype_s, ie_exts),
     (int32_t)offsetof(srs_restype_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_restype_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_srs_restype_s_names[] = {{"sRSResourceTypeChoice", nullptr},
                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_restype_s_opts[]  = {
    {1, (int32_t)offsetof(srs_restype_s, ie_exts_present)}};
const asn1::table::seq_desc srs_restype_s::desc = {
    {seq_table_srs_restype_s_fields, 2, UINT64_C(0x1), seq_table_srs_restype_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_srs_restype_s_names};

// SRSTransmissionStatus ::= ENUMERATED
static const char*           enum_names_srs_tx_status_e_values[] = {"stopped", "area-specific-SRS-activated"};
const asn1::table::enum_desc srs_tx_status_opts::desc = {enum_names_srs_tx_status_e_values, 2, "srs_tx_status_e"};

// Search-window-information ::= SEQUENCE
static const asn1::table::seq_field seq_table_search_win_info_s_fields[] = {
    {offsetof(search_win_info_s, expected_propagation_delay),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i16,
     true,
     true,
     INT64_C(-3841),
     UINT64_C(3841),
     nullptr},
    {offsetof(search_win_info_s, delay_uncertainty),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(246),
     nullptr},
    {offsetof(search_win_info_s, ie_exts),
     (int32_t)offsetof(search_win_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(search_win_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_search_win_info_s_names[] = {{"expectedPropagationDelay", nullptr},
                                                                            {"delayUncertainty", nullptr},
                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_search_win_info_s_opts[]  = {
    {2, (int32_t)offsetof(search_win_info_s, ie_exts_present)}};
const asn1::table::seq_desc search_win_info_s::desc = {
    {seq_table_search_win_info_s_fields, 3, UINT64_C(0x3), seq_table_search_win_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_search_win_info_s_names};

// SpatialDirectionInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_spatial_direction_info_s_fields[] = {
    {offsetof(spatial_direction_info_s, nr_prs_beam_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_prs_beam_info_s::desc},
    {offsetof(spatial_direction_info_s, ie_exts),
     (int32_t)offsetof(spatial_direction_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(spatial_direction_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_spatial_direction_info_s_names[] = {{"nR-PRS-Beam-Information", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_spatial_direction_info_s_opts[]  = {
    {1, (int32_t)offsetof(spatial_direction_info_s, ie_exts_present)}};
const asn1::table::seq_desc spatial_direction_info_s::desc = {
    {seq_table_spatial_direction_info_s_fields, 2, UINT64_C(0x1), seq_table_spatial_direction_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_spatial_direction_info_s_names};

static const uint32_t objset_id_trp_meas_request_item_ext_ies_o[]   = {60, 69, 82, 83};
static const uint8_t  objset_crit_trp_meas_request_item_ext_ies_o[] = {crit_e::ignore,
                                                                       crit_e::ignore,
                                                                       crit_e::ignore,
                                                                       crit_e::ignore};

// TRP-MeasurementRequestItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_trp_meas_request_item_ext_ies_container_values[] = {
    {offsetof(trp_meas_request_item_ext_ies_container, cell_id),
     (int32_t)offsetof(trp_meas_request_item_ext_ies_container, cell_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_nr_s::desc},
    {offsetof(trp_meas_request_item_ext_ies_container, ao_a_search_win),
     (int32_t)offsetof(trp_meas_request_item_ext_ies_container, ao_a_search_win_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ao_a_assist_info_s::desc},
    {offsetof(trp_meas_request_item_ext_ies_container, nof_trp_rx_teg),
     (int32_t)offsetof(trp_meas_request_item_ext_ies_container, nof_trp_rx_teg_present),
     asn1::table::enum_op<decltype(trp_meas_request_item_ext_ies_container::nof_trp_rx_teg)>(),
     decltype(trp_meas_request_item_ext_ies_container::nof_trp_rx_teg)::has_ext,
     false,
     decltype(trp_meas_request_item_ext_ies_container::nof_trp_rx_teg)::nof_types,
     decltype(trp_meas_request_item_ext_ies_container::nof_trp_rx_teg)::nof_exts,
     nullptr,
     &nof_trp_rx_teg_opts::desc},
    {offsetof(trp_meas_request_item_ext_ies_container, nof_trp_rx_tx_teg),
     (int32_t)offsetof(trp_meas_request_item_ext_ies_container, nof_trp_rx_tx_teg_present),
     asn1::table::enum_op<decltype(trp_meas_request_item_ext_ies_container::nof_trp_rx_tx_teg)>(),
     decltype(trp_meas_request_item_ext_ies_container::nof_trp_rx_tx_teg)::has_ext,
     false,
     decltype(trp_meas_request_item_ext_ies_container::nof_trp_rx_tx_teg)::nof_types,
     decltype(trp_meas_request_item_ext_ies_container::nof_trp_rx_tx_teg)::nof_exts,
     nullptr,
     &nof_trp_rx_tx_teg_opts::desc}};
static const int32_t ie_table_trp_meas_request_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(trp_meas_request_item_ext_ies_container, cell_id_present),
    (int32_t)offsetof(trp_meas_request_item_ext_ies_container, ao_a_search_win_present),
    (int32_t)offsetof(trp_meas_request_item_ext_ies_container, nof_trp_rx_teg_present),
    (int32_t)offsetof(trp_meas_request_item_ext_ies_container, nof_trp_rx_tx_teg_present)};
const asn1::table::ie_container_desc trp_meas_request_item_ext_ies_container::desc = {
    objset_id_trp_meas_request_item_ext_ies_o,
    objset_crit_trp_meas_request_item_ext_ies_o,
    ie_table_trp_meas_request_item_ext_ies_container_values,
    ie_table_trp_meas_request_item_ext_ies_container_presences,
    4,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// TRP-MeasurementRequestItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_meas_request_item_s_fields[] = {
    {offsetof(trp_meas_request_item_s, trp_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(65535),
     nullptr},
    {offsetof(trp_meas_request_item_s, search_win_info),
     (int32_t)offsetof(trp_meas_request_item_s, search_win_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &search_win_info_s::desc},
    {offsetof(trp_meas_request_item_s, ie_exts),
     (int32_t)offsetof(trp_meas_request_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_meas_request_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_trp_meas_request_item_s_names[] = {
    {"tRP-ID", nullptr},
    {"search-window-information", nullptr},
    {"iE-extensions", nullptr}};
static const asn1::table::seq_opt seq_table_trp_meas_request_item_s_opts[] = {
    {1, (int32_t)offsetof(trp_meas_request_item_s, search_win_info_present)},
    {2, (int32_t)offsetof(trp_meas_request_item_s, ie_exts_present)}};
const asn1::table::seq_desc trp_meas_request_item_s::desc = {
    {seq_table_trp_meas_request_item_s_fields, 3, UINT64_C(0x1), seq_table_trp_meas_request_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_meas_request_item_s_names};

// ULRTOAMeas-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_ul_rtoa_meas_ext_ies_o[] = {141, 142, 143, 144, 145, 146};
static const uint8_t  objset_crit_ul_rtoa_meas_ext_ies_o[] =
    {crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};
static const uint8_t objset_value_ul_rtoa_meas_ext_ies_o[] = {
    ul_rtoa_meas_ext_ies_o::value_c::types::report_granularitykminus1,
    ul_rtoa_meas_ext_ies_o::value_c::types::report_granularitykminus2,
    ul_rtoa_meas_ext_ies_o::value_c::types::report_granularitykminus3,
    ul_rtoa_meas_ext_ies_o::value_c::types::report_granularitykminus4,
    ul_rtoa_meas_ext_ies_o::value_c::types::report_granularitykminus5,
    ul_rtoa_meas_ext_ies_o::value_c::types::report_granularitykminus6};
static const uint8_t                    objset_presence_ul_rtoa_meas_ext_ies_o[] = {presence_e::mandatory,
                                                                                    presence_e::mandatory,
                                                                                    presence_e::mandatory,
                                                                                    presence_e::mandatory,
                                                                                    presence_e::mandatory,
                                                                                    presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ul_rtoa_meas_ext_ies_o[]  = {
    {objset_crit_ul_rtoa_meas_ext_ies_o, crit_e::nulltype},
    {objset_value_ul_rtoa_meas_ext_ies_o, ul_rtoa_meas_ext_ies_o::value_c::types::nulltype},
    {objset_presence_ul_rtoa_meas_ext_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ul_rtoa_meas_ext_ies_o::desc = {objset_id_ul_rtoa_meas_ext_ies_o,
                                                               6,
                                                               "id",
                                                               objset_columns_ul_rtoa_meas_ext_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ul_rtoa_meas_ext_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3940097),
     &asn1::table::any_value_codec<uint32_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(7880193),
     &asn1::table::any_value_codec<uint32_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(15760385),
     &asn1::table::any_value_codec<uint32_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(31520769),
     &asn1::table::any_value_codec<uint32_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(63041537),
     &asn1::table::any_value_codec<uint32_t>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(126083073),
     &asn1::table::any_value_codec<uint32_t>::value}};
static const asn1::table::field_name choice_table_ul_rtoa_meas_ext_ies_o_value_c_names[] = {
    {"INTEGER (0..3940097)", nullptr},
    {"INTEGER (0..7880193)", nullptr},
    {"INTEGER (0..15760385)", nullptr},
    {"INTEGER (0..31520769)", nullptr},
    {"INTEGER (0..63041537)", nullptr},
    {"INTEGER (0..126083073)", nullptr}};
const asn1::table::choice_desc ul_rtoa_meas_ext_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ul_rtoa_meas_ext_ies_o::value_c::types>(),
    ul_rtoa_meas_ext_ies_o::value_c::types::nof_types,
    ul_rtoa_meas_ext_ies_o::value_c::types::nof_exts,
    ul_rtoa_meas_ext_ies_o::value_c::types::has_ext,
    true,
    choice_table_ul_rtoa_meas_ext_ies_o_value_c_alternatives,
    &asn1::table::choice_set<ul_rtoa_meas_ext_ies_o::value_c>,
    choice_table_ul_rtoa_meas_ext_ies_o_value_c_names,
    "ul_rtoa_meas_ext_ies_o::value_c",
    offsetof(ul_rtoa_meas_ext_ies_o::value_c, type_),
    offsetof(ul_rtoa_meas_ext_ies_o::value_c, c),
    true,
    nullptr,
    true};

void ul_rtoa_meas_ext_ies_o::value_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, ul_rtoa_meas_ext_ies_o::value_c::desc);
}
uint32_t& ul_rtoa_meas_ext_ies_o::value_c::report_granularitykminus1()
{
  assert_choice_type(types::report_granularitykminus1, type_, "Value");
  return c.get<uint32_t>();
}
uint32_t& ul_rtoa_meas_ext_ies_o::value_c::report_granularitykminus2()
{
  assert_choice_type(types::report_granularitykminus2, type_, "Value");
  return c.get<uint32_t>();
}
uint32_t& ul_rtoa_meas_ext_ies_o::value_c::report_granularitykminus3()
{
  assert_choice_type(types::report_granularitykminus3, type_, "Value");
  return c.get<uint32_t>();
}
uint32_t& ul_rtoa_meas_ext_ies_o::value_c::report_granularitykminus4()
{
  assert_choice_type(types::report_granularitykminus4, type_, "Value");
  return c.get<uint32_t>();
}
uint32_t& ul_rtoa_meas_ext_ies_o::value_c::report_granularitykminus5()
{
  assert_choice_type(types::report_granularitykminus5, type_, "Value");
  return c.get<uint32_t>();
}
uint32_t& ul_rtoa_meas_ext_ies_o::value_c::report_granularitykminus6()
{
  assert_choice_type(types::report_granularitykminus6, type_, "Value");
  return c.get<uint32_t>();
}
const uint32_t& ul_rtoa_meas_ext_ies_o::value_c::report_granularitykminus1() const
{
  assert_choice_type(types::report_granularitykminus1, type_, "Value");
  return c.get<uint32_t>();
}
const uint32_t& ul_rtoa_meas_ext_ies_o::value_c::report_granularitykminus2() const
{
  assert_choice_type(types::report_granularitykminus2, type_, "Value");
  return c.get<uint32_t>();
}
const uint32_t& ul_rtoa_meas_ext_ies_o::value_c::report_granularitykminus3() const
{
  assert_choice_type(types::report_granularitykminus3, type_, "Value");
  return c.get<uint32_t>();
}
const uint32_t& ul_rtoa_meas_ext_ies_o::value_c::report_granularitykminus4() const
{
  assert_choice_type(types::report_granularitykminus4, type_, "Value");
  return c.get<uint32_t>();
}
const uint32_t& ul_rtoa_meas_ext_ies_o::value_c::report_granularitykminus5() const
{
  assert_choice_type(types::report_granularitykminus5, type_, "Value");
  return c.get<uint32_t>();
}
const uint32_t& ul_rtoa_meas_ext_ies_o::value_c::report_granularitykminus6() const
{
  assert_choice_type(types::report_granularitykminus6, type_, "Value");
  return c.get<uint32_t>();
}

static const char*           enum_names_ul_rtoa_meas_ext_ies_o_value_c_types_values[] = {"INTEGER (0..3940097)",
                                                                                         "INTEGER (0..7880193)",
                                                                                         "INTEGER (0..15760385)",
                                                                                         "INTEGER (0..31520769)",
                                                                                         "INTEGER (0..63041537)",
                                                                                         "INTEGER (0..126083073)"};
const asn1::table::enum_desc ul_rtoa_meas_ext_ies_o::value_c::types_opts::desc        = {
    enum_names_ul_rtoa_meas_ext_ies_o_value_c_types_values,
    6,
    "ul_rtoa_meas_ext_ies_o::value_c::types"};

// ULRTOAMeas ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ul_rtoa_meas_c_alternatives[] = {
    &asn1::table::alternative_ops<uint32_t, true>::value,
    &asn1::table::alternative_ops<uint32_t, true>::value,
    &asn1::table::alternative_ops<uint32_t, true>::value,
    &asn1::table::alternative_ops<uint32_t, true>::value,
    &asn1::table::alternative_ops<uint32_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<ul_rtoa_meas_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc ul_rtoa_meas_c::storage_desc = {choice_storage_ul_rtoa_meas_c_alternatives,
                                                                       7,
                                                                       "ul_rtoa_meas_c"};

uint32_t& ul_rtoa_meas_c::set_k0()
{
  set(types::k0);
  return c.get<uint32_t>();
}
uint32_t& ul_rtoa_meas_c::set_k1()
{
  set(types::k1);
  return c.get<uint32_t>();
}
uint32_t& ul_rtoa_meas_c::set_k2()
{
  set(types::k2);
  return c.get<uint32_t>();
}
uint32_t& ul_rtoa_meas_c::set_k3()
{
  set(types::k3);
  return c.get<uint32_t>();
}
uint32_t& ul_rtoa_meas_c::set_k4()
{
  set(types::k4);
  return c.get<uint32_t>();
}
uint16_t& ul_rtoa_meas_c::set_k5()
{
  set(types::k5);
  return c.get<uint16_t>();
}
protocol_ie_single_container_s<ul_rtoa_meas_ext_ies_o>& ul_rtoa_meas_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<ul_rtoa_meas_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_ul_rtoa_meas_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(1970049),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(985025),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(492513),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(246257),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(123129),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(61565),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<ul_rtoa_meas_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_ul_rtoa_meas_c_names[] = {{"k0", nullptr},
                                                                            {"k1", nullptr},
                                                                            {"k2", nullptr},
                                                                            {"k3", nullptr},
                                                                            {"k4", nullptr},
                                                                            {"k5", nullptr},
                                                                            {"choice-extension", nullptr}};
const asn1::table::choice_desc       ul_rtoa_meas_c::desc = {asn1::table::enum_int_type<ul_rtoa_meas_c::types>(),
                                                             ul_rtoa_meas_c::types::nof_types,
                                                             ul_rtoa_meas_c::types::nof_exts,
                                                             ul_rtoa_meas_c::types::has_ext,
                                                             true,
                                                             choice_table_ul_rtoa_meas_c_alternatives,
                                                             nullptr,
                                                             choice_table_ul_rtoa_meas_c_names,
                                                             "ul_rtoa_meas_c",
                                                             offsetof(ul_rtoa_meas_c, c.type),
                                                             offsetof(ul_rtoa_meas_c, c.value),
                                                             false,
                                                             &ul_rtoa_meas_c::storage_desc};

static const char* enum_names_ul_rtoa_meas_c_types_values[] = {"k0", "k1", "k2", "k3", "k4", "k5", "choice-extension"};
const asn1::table::enum_desc ul_rtoa_meas_c::types_opts::desc = {enum_names_ul_rtoa_meas_c_types_values,
                                                                 7,
                                                                 "ul_rtoa_meas_c::types"};
uint8_t                      ul_rtoa_meas_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5};
  return map_enum_number(numbers, 6, value, "ul_rtoa_meas_c::types");
}

static const uint32_t objset_id_ul_rtoa_measurement_ext_ies_o[]   = {77, 86};
static const uint8_t  objset_crit_ul_rtoa_measurement_ext_ies_o[] = {crit_e::ignore, crit_e::ignore};

// UL-RTOAMeasurement-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_ul_rtoa_measurement_ext_ies_container_values[] = {
    {offsetof(ul_rtoa_measurement_ext_ies_container, extended_add_path_list),
     (int32_t)offsetof(ul_rtoa_measurement_ext_ies_container, extended_add_path_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(ul_rtoa_measurement_ext_ies_container::extended_add_path_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &extended_add_path_list_item_s::desc},
    {offsetof(ul_rtoa_measurement_ext_ies_container, trp_rx_teg_info),
     (int32_t)offsetof(ul_rtoa_measurement_ext_ies_container, trp_rx_teg_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_rx_teg_info_s::desc}};
static const int32_t ie_table_ul_rtoa_measurement_ext_ies_container_presences[] = {
    (int32_t)offsetof(ul_rtoa_measurement_ext_ies_container, extended_add_path_list_present),
    (int32_t)offsetof(ul_rtoa_measurement_ext_ies_container, trp_rx_teg_info_present)};
const asn1::table::ie_container_desc ul_rtoa_measurement_ext_ies_container::desc = {
    objset_id_ul_rtoa_measurement_ext_ies_o,
    objset_crit_ul_rtoa_measurement_ext_ies_o,
    ie_table_ul_rtoa_measurement_ext_ies_container_values,
    ie_table_ul_rtoa_measurement_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// UL-RTOAMeasurement ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_rtoa_measurement_s_fields[] = {
    {offsetof(ul_rtoa_measurement_s, ul_rto_ameas),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_rtoa_meas_c::desc},
    {offsetof(ul_rtoa_measurement_s, add_path_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2,
     &asn1::table::item_list_codec<decltype(ul_rtoa_measurement_s::add_path_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &add_path_list_item_s::desc},
    {offsetof(ul_rtoa_measurement_s, ie_exts),
     (int32_t)offsetof(ul_rtoa_measurement_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_rtoa_measurement_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_ul_rtoa_measurement_s_names[] = {{"uLRTOAmeas", nullptr},
                                                                                {"additionalPathList", nullptr},
                                                                                {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ul_rtoa_measurement_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(ul_rtoa_measurement_s, ie_exts_present)}};
const asn1::table::seq_desc ul_rtoa_measurement_s::desc = {
    {seq_table_ul_rtoa_measurement_s_fields, 3, UINT64_C(0x1), seq_table_ul_rtoa_measurement_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_ul_rtoa_measurement_s_names};

// UL-RSCPMeas ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_rs_cp_meas_s_fields[] = {
    {offsetof(ul_rs_cp_meas_s, ul_rs_cp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(3599),
     nullptr},
    {offsetof(ul_rs_cp_meas_s, ie_exts),
     (int32_t)offsetof(ul_rs_cp_meas_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ul_rs_cp_meas_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ul_rs_cp_meas_s_names[] = {{"uLRSCP", nullptr},
                                                                          {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ul_rs_cp_meas_s_opts[]  = {
    {1, (int32_t)offsetof(ul_rs_cp_meas_s, ie_exts_present)}};
const asn1::table::seq_desc ul_rs_cp_meas_s::desc = {
    {seq_table_ul_rs_cp_meas_s_fields, 2, UINT64_C(0x1), seq_table_ul_rs_cp_meas_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_ul_rs_cp_meas_s_names};

// TrpMeasuredResultsValue-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_trp_measured_results_value_ext_ies_o[]    = {71, 74, 75, 126};
static const uint8_t  objset_crit_trp_measured_results_value_ext_ies_o[]  = {crit_e::reject,
                                                                             crit_e::reject,
                                                                             crit_e::reject,
                                                                             crit_e::reject};
static const uint8_t  objset_value_trp_measured_results_value_ext_ies_o[] = {
    trp_measured_results_value_ext_ies_o::value_c::types::zo_a,
    trp_measured_results_value_ext_ies_o::value_c::types::multiple_ul_ao_a,
    trp_measured_results_value_ext_ies_o::value_c::types::ul_srs_rsrp_p,
    trp_measured_results_value_ext_ies_o::value_c::types::ul_rs_cp_meas};
static const uint8_t                    objset_presence_trp_measured_results_value_ext_ies_o[] = {presence_e::mandatory,
                                                                                                  presence_e::mandatory,
                                                                                                  presence_e::mandatory,
                                                                                                  presence_e::mandatory};
static const asn1::table::objset_column objset_columns_trp_measured_results_value_ext_ies_o[]  = {
    {objset_crit_trp_measured_results_value_ext_ies_o, crit_e::nulltype},
    {objset_value_trp_measured_results_value_ext_ies_o, trp_measured_results_value_ext_ies_o::value_c::types::nulltype},
    {objset_presence_trp_measured_results_value_ext_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc trp_measured_results_value_ext_ies_o::desc =
    {objset_id_trp_measured_results_value_ext_ies_o, 4, "id", objset_columns_trp_measured_results_value_ext_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_trp_measured_results_value_ext_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<zo_a_s>::value,
     &zo_a_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<multiple_ul_ao_a_s>::value,
     &multiple_ul_ao_a_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ul_srs_rsrp_p_s>::value,
     &ul_srs_rsrp_p_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ul_rs_cp_meas_s>::value,
     &ul_rs_cp_meas_s::desc}};
static const asn1::table::field_name choice_table_trp_measured_results_value_ext_ies_o_value_c_names[] =
    {{"ZoA", nullptr}, {"MultipleULAoA", nullptr}, {"UL-SRS-RSRPP", nullptr}, {"UL-RSCPMeas", nullptr}};
const asn1::table::choice_desc trp_measured_results_value_ext_ies_o::value_c::desc = {
    asn1::table::enum_int_type<trp_measured_results_value_ext_ies_o::value_c::types>(),
    trp_measured_results_value_ext_ies_o::value_c::types::nof_types,
    trp_measured_results_value_ext_ies_o::value_c::types::nof_exts,
    trp_measured_results_value_ext_ies_o::value_c::types::has_ext,
    true,
    choice_table_trp_measured_results_value_ext_ies_o_value_c_alternatives,
    &asn1::table::choice_set<trp_measured_results_value_ext_ies_o::value_c>,
    choice_table_trp_measured_results_value_ext_ies_o_value_c_names,
    "trp_measured_results_value_ext_ies_o::value_c",
    offsetof(trp_measured_results_value_ext_ies_o::value_c, type_),
    offsetof(trp_measured_results_value_ext_ies_o::value_c, c),
    true,
    nullptr,
    true};

void trp_measured_results_value_ext_ies_o::value_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, trp_measured_results_value_ext_ies_o::value_c::desc);
}
zo_a_s& trp_measured_results_value_ext_ies_o::value_c::zo_a()
{
  assert_choice_type(types::zo_a, type_, "Value");
  return c.get<zo_a_s>();
}
multiple_ul_ao_a_s& trp_measured_results_value_ext_ies_o::value_c::multiple_ul_ao_a()
{
  assert_choice_type(types::multiple_ul_ao_a, type_, "Value");
  return c.get<multiple_ul_ao_a_s>();
}
ul_srs_rsrp_p_s& trp_measured_results_value_ext_ies_o::value_c::ul_srs_rsrp_p()
{
  assert_choice_type(types::ul_srs_rsrp_p, type_, "Value");
  return c.get<ul_srs_rsrp_p_s>();
}
ul_rs_cp_meas_s& trp_measured_results_value_ext_ies_o::value_c::ul_rs_cp_meas()
{
  assert_choice_type(types::ul_rs_cp_meas, type_, "Value");
  return c.get<ul_rs_cp_meas_s>();
}
const zo_a_s& trp_measured_results_value_ext_ies_o::value_c::zo_a() const
{
  assert_choice_type(types::zo_a, type_, "Value");
  return c.get<zo_a_s>();
}
const multiple_ul_ao_a_s& trp_measured_results_value_ext_ies_o::value_c::multiple_ul_ao_a() const
{
  assert_choice_type(types::multiple_ul_ao_a, type_, "Value");
  return c.get<multiple_ul_ao_a_s>();
}
const ul_srs_rsrp_p_s& trp_measured_results_value_ext_ies_o::value_c::ul_srs_rsrp_p() const
{
  assert_choice_type(types::ul_srs_rsrp_p, type_, "Value");
  return c.get<ul_srs_rsrp_p_s>();
}
const ul_rs_cp_meas_s& trp_measured_results_value_ext_ies_o::value_c::ul_rs_cp_meas() const
{
  assert_choice_type(types::ul_rs_cp_meas, type_, "Value");
  return c.get<ul_rs_cp_meas_s>();
}

static const char*           enum_names_trp_measured_results_value_ext_ies_o_value_c_types_values[] = {"ZoA",
                                                                                                       "MultipleULAoA",
                                                                                                       "UL-SRS-RSRPP",
                                                                                                       "UL-RSCPMeas"};
const asn1::table::enum_desc trp_measured_results_value_ext_ies_o::value_c::types_opts::desc        = {
    enum_names_trp_measured_results_value_ext_ies_o_value_c_types_values,
    4,
    "trp_measured_results_value_ext_ies_o::value_c::types"};

// TrpMeasuredResultsValue ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_trp_measured_results_value_c_alternatives[] = {
    &asn1::table::alternative_ops<ul_ao_a_s, false>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<ul_rtoa_measurement_s, false>::value,
    &asn1::table::alternative_ops<gnb_rx_tx_time_diff_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<trp_measured_results_value_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc trp_measured_results_value_c::storage_desc = {
    choice_storage_trp_measured_results_value_c_alternatives,
    5,
    "trp_measured_results_value_c"};

ul_ao_a_s& trp_measured_results_value_c::set_ul_angle_of_arrival()
{
  set(types::ul_angle_of_arrival);
  return c.get<ul_ao_a_s>();
}
uint8_t& trp_measured_results_value_c::set_ul_srs_rsrp()
{
  set(types::ul_srs_rsrp);
  return c.get<uint8_t>();
}
ul_rtoa_measurement_s& trp_measured_results_value_c::set_ul_rtoa()
{
  set(types::ul_rtoa);
  return c.get<ul_rtoa_measurement_s>();
}
gnb_rx_tx_time_diff_s& trp_measured_results_value_c::set_gnb_rx_tx_time_diff()
{
  set(types::gnb_rx_tx_time_diff);
  return c.get<gnb_rx_tx_time_diff_s>();
}
protocol_ie_single_container_s<trp_measured_results_value_ext_ies_o>& trp_measured_results_value_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<trp_measured_results_value_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_trp_measured_results_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_ao_a_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(126),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_rtoa_measurement_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gnb_rx_tx_time_diff_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<trp_measured_results_value_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_trp_measured_results_value_c_names[] = {
    {"uL-AngleOfArrival", nullptr},
    {"uL-SRS-RSRP", nullptr},
    {"uL-RTOA", nullptr},
    {"gNB-RxTxTimeDiff", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc trp_measured_results_value_c::desc = {
    asn1::table::enum_int_type<trp_measured_results_value_c::types>(),
    trp_measured_results_value_c::types::nof_types,
    trp_measured_results_value_c::types::nof_exts,
    trp_measured_results_value_c::types::has_ext,
    true,
    choice_table_trp_measured_results_value_c_alternatives,
    nullptr,
    choice_table_trp_measured_results_value_c_names,
    "trp_measured_results_value_c",
    offsetof(trp_measured_results_value_c, c.type),
    offsetof(trp_measured_results_value_c, c.value),
    false,
    &trp_measured_results_value_c::storage_desc};

static const char*           enum_names_trp_measured_results_value_c_types_values[] = {"uL-AngleOfArrival",
                                                                                       "uL-SRS-RSRP",
                                                                                       "uL-RTOA",
                                                                                       "gNB-RxTxTimeDiff",
                                                                                       "choice-extension"};
const asn1::table::enum_desc trp_measured_results_value_c::types_opts::desc         = {
    enum_names_trp_measured_results_value_c_types_values,
    5,
    "trp_measured_results_value_c::types"};

static const uint32_t objset_id_trp_meas_result_item_ext_ies_o[] = {76, 79, 80, 114, 140, 131, 157};
static const uint8_t  objset_crit_trp_meas_result_item_ext_ies_o[] =
    {crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// TrpMeasurementResultItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_trp_meas_result_item_ext_ies_container_values[] = {
    {offsetof(trp_meas_result_item_ext_ies_container, srs_restype),
     (int32_t)offsetof(trp_meas_result_item_ext_ies_container, srs_restype_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_restype_s::desc},
    {offsetof(trp_meas_result_item_ext_ies_container, arp_id),
     (int32_t)offsetof(trp_meas_result_item_ext_ies_container, arp_id_present),
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(16),
     nullptr},
    {offsetof(trp_meas_result_item_ext_ies_container, lo_s_n_lo_si_nformation),
     (int32_t)offsetof(trp_meas_result_item_ext_ies_container, lo_s_n_lo_si_nformation_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &lo_s_n_lo_si_nformation_c::desc},
    {offsetof(trp_meas_result_item_ext_ies_container, mobile_trp_location_info),
     (int32_t)offsetof(trp_meas_result_item_ext_ies_container, mobile_trp_location_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mobile_trp_location_info_s::desc},
    {offsetof(trp_meas_result_item_ext_ies_container, measured_freq_hops),
     (int32_t)offsetof(trp_meas_result_item_ext_ies_container, measured_freq_hops_present),
     asn1::table::enum_op<decltype(trp_meas_result_item_ext_ies_container::measured_freq_hops)>(),
     decltype(trp_meas_result_item_ext_ies_container::measured_freq_hops)::has_ext,
     false,
     decltype(trp_meas_result_item_ext_ies_container::measured_freq_hops)::nof_types,
     decltype(trp_meas_result_item_ext_ies_container::measured_freq_hops)::nof_exts,
     nullptr,
     &measured_freq_hops_opts::desc},
    {offsetof(trp_meas_result_item_ext_ies_container, aggr_pos_srs_res_id_list),
     (int32_t)offsetof(trp_meas_result_item_ext_ies_container, aggr_pos_srs_res_id_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     2,
     3,
     &asn1::table::item_list_codec<decltype(trp_meas_result_item_ext_ies_container::aggr_pos_srs_res_id_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &aggr_pos_srs_res_id_item_s::desc},
    {offsetof(trp_meas_result_item_ext_ies_container, meas_based_on_aggr_res),
     (int32_t)offsetof(trp_meas_result_item_ext_ies_container, meas_based_on_aggr_res_present),
     asn1::table::enum_op<decltype(trp_meas_result_item_ext_ies_container::meas_based_on_aggr_res)>(),
     decltype(trp_meas_result_item_ext_ies_container::meas_based_on_aggr_res)::has_ext,
     false,
     decltype(trp_meas_result_item_ext_ies_container::meas_based_on_aggr_res)::nof_types,
     decltype(trp_meas_result_item_ext_ies_container::meas_based_on_aggr_res)::nof_exts,
     nullptr,
     &meas_based_on_aggr_res_opts::desc}};
static const int32_t ie_table_trp_meas_result_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(trp_meas_result_item_ext_ies_container, srs_restype_present),
    (int32_t)offsetof(trp_meas_result_item_ext_ies_container, arp_id_present),
    (int32_t)offsetof(trp_meas_result_item_ext_ies_container, lo_s_n_lo_si_nformation_present),
    (int32_t)offsetof(trp_meas_result_item_ext_ies_container, mobile_trp_location_info_present),
    (int32_t)offsetof(trp_meas_result_item_ext_ies_container, measured_freq_hops_present),
    (int32_t)offsetof(trp_meas_result_item_ext_ies_container, aggr_pos_srs_res_id_list_present),
    (int32_t)offsetof(trp_meas_result_item_ext_ies_container, meas_based_on_aggr_res_present)};
const asn1::table::ie_container_desc trp_meas_result_item_ext_ies_container::desc = {
    objset_id_trp_meas_result_item_ext_ies_o,
    objset_crit_trp_meas_result_item_ext_ies_o,
    ie_table_trp_meas_result_item_ext_ies_container_values,
    ie_table_trp_meas_result_item_ext_ies_container_presences,
    7,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// TrpMeasurementResultItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_meas_result_item_s_fields[] = {
    {offsetof(trp_meas_result_item_s, measured_results_value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_measured_results_value_c::desc},
    {offsetof(trp_meas_result_item_s, time_stamp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &time_stamp_s::desc},
    {offsetof(trp_meas_result_item_s, meas_quality),
     (int32_t)offsetof(trp_meas_result_item_s, meas_quality_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_meas_quality_c::desc},
    {offsetof(trp_meas_result_item_s, meas_beam_info),
     (int32_t)offsetof(trp_meas_result_item_s, meas_beam_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_beam_info_s::desc},
    {offsetof(trp_meas_result_item_s, ie_exts),
     (int32_t)offsetof(trp_meas_result_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_meas_result_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_trp_meas_result_item_s_names[] = {{"measuredResultsValue", nullptr},
                                                                                 {"timeStamp", nullptr},
                                                                                 {"measurementQuality", nullptr},
                                                                                 {"measurementBeamInfo", nullptr},
                                                                                 {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_trp_meas_result_item_s_opts[]  = {
    {2, (int32_t)offsetof(trp_meas_result_item_s, meas_quality_present)},
    {3, (int32_t)offsetof(trp_meas_result_item_s, meas_beam_info_present)},
    {4, (int32_t)offsetof(trp_meas_result_item_s, ie_exts_present)}};
const asn1::table::seq_desc trp_meas_result_item_s::desc = {
    {seq_table_trp_meas_result_item_s_fields, 5, UINT64_C(0x3), seq_table_trp_meas_result_item_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_meas_result_item_s_names};

static const uint32_t objset_id_trp_meas_resp_item_ext_ies_o[]   = {60};
static const uint8_t  objset_crit_trp_meas_resp_item_ext_ies_o[] = {crit_e::ignore};

// TRP-MeasurementResponseItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_trp_meas_resp_item_ext_ies_container_values[] = {
    {offsetof(trp_meas_resp_item_ext_ies_container, cell_id),
     (int32_t)offsetof(trp_meas_resp_item_ext_ies_container, cell_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_nr_s::desc}};
static const int32_t ie_table_trp_meas_resp_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(trp_meas_resp_item_ext_ies_container, cell_id_present)};
const asn1::table::ie_container_desc trp_meas_resp_item_ext_ies_container::desc = {
    objset_id_trp_meas_resp_item_ext_ies_o,
    objset_crit_trp_meas_resp_item_ext_ies_o,
    ie_table_trp_meas_resp_item_ext_ies_container_values,
    ie_table_trp_meas_resp_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// TRP-MeasurementResponseItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_meas_resp_item_s_fields[] = {
    {offsetof(trp_meas_resp_item_s, trp_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(65535),
     nullptr},
    {offsetof(trp_meas_resp_item_s, meas_result),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16384,
     &asn1::table::item_list_codec<decltype(trp_meas_resp_item_s::meas_result),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &trp_meas_result_item_s::desc},
    {offsetof(trp_meas_resp_item_s, ie_exts),
     (int32_t)offsetof(trp_meas_resp_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_meas_resp_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_trp_meas_resp_item_s_names[] = {{"tRP-ID", nullptr},
                                                                               {"measurementResult", nullptr},
                                                                               {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_trp_meas_resp_item_s_opts[]  = {
    {2, (int32_t)offsetof(trp_meas_resp_item_s, ie_exts_present)}};
const asn1::table::seq_desc trp_meas_resp_item_s::desc = {
    {seq_table_trp_meas_resp_item_s_fields, 3, UINT64_C(0x3), seq_table_trp_meas_resp_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_meas_resp_item_s_names};

static const uint32_t objset_id_trp_meas_upd_item_ext_ies_o[]   = {82, 83};
static const uint8_t  objset_crit_trp_meas_upd_item_ext_ies_o[] = {crit_e::ignore, crit_e::ignore};

// TRP-MeasurementUpdateItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_trp_meas_upd_item_ext_ies_container_values[] = {
    {offsetof(trp_meas_upd_item_ext_ies_container, nof_trp_rx_teg),
     (int32_t)offsetof(trp_meas_upd_item_ext_ies_container, nof_trp_rx_teg_present),
     asn1::table::enum_op<decltype(trp_meas_upd_item_ext_ies_container::nof_trp_rx_teg)>(),
     decltype(trp_meas_upd_item_ext_ies_container::nof_trp_rx_teg)::has_ext,
     false,
     decltype(trp_meas_upd_item_ext_ies_container::nof_trp_rx_teg)::nof_types,
     decltype(trp_meas_upd_item_ext_ies_container::nof_trp_rx_teg)::nof_exts,
     nullptr,
     &nof_trp_rx_teg_opts::desc},
    {offsetof(trp_meas_upd_item_ext_ies_container, nof_trp_rx_tx_teg),
     (int32_t)offsetof(trp_meas_upd_item_ext_ies_container, nof_trp_rx_tx_teg_present),
     asn1::table::enum_op<decltype(trp_meas_upd_item_ext_ies_container::nof_trp_rx_tx_teg)>(),
     decltype(trp_meas_upd_item_ext_ies_container::nof_trp_rx_tx_teg)::has_ext,
     false,
     decltype(trp_meas_upd_item_ext_ies_container::nof_trp_rx_tx_teg)::nof_types,
     decltype(trp_meas_upd_item_ext_ies_container::nof_trp_rx_tx_teg)::nof_exts,
     nullptr,
     &nof_trp_rx_tx_teg_opts::desc}};
static const int32_t ie_table_trp_meas_upd_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(trp_meas_upd_item_ext_ies_container, nof_trp_rx_teg_present),
    (int32_t)offsetof(trp_meas_upd_item_ext_ies_container, nof_trp_rx_tx_teg_present)};
const asn1::table::ie_container_desc trp_meas_upd_item_ext_ies_container::desc = {
    objset_id_trp_meas_upd_item_ext_ies_o,
    objset_crit_trp_meas_upd_item_ext_ies_o,
    ie_table_trp_meas_upd_item_ext_ies_container_values,
    ie_table_trp_meas_upd_item_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// TRP-MeasurementUpdateItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_meas_upd_item_s_fields[] = {
    {offsetof(trp_meas_upd_item_s, trp_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(65535),
     nullptr},
    {offsetof(trp_meas_upd_item_s, ao_a_win_info),
     (int32_t)offsetof(trp_meas_upd_item_s, ao_a_win_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ao_a_assist_info_s::desc},
    {offsetof(trp_meas_upd_item_s, ie_exts),
     (int32_t)offsetof(trp_meas_upd_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_meas_upd_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_trp_meas_upd_item_s_names[] = {{"tRP-ID", nullptr},
                                                                              {"aoA-window-information", nullptr},
                                                                              {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_trp_meas_upd_item_s_opts[]  = {
    {1, (int32_t)offsetof(trp_meas_upd_item_s, ao_a_win_info_present)},
    {2, (int32_t)offsetof(trp_meas_upd_item_s, ie_exts_present)}};
const asn1::table::seq_desc trp_meas_upd_item_s::desc = {
    {seq_table_trp_meas_upd_item_s_fields, 3, UINT64_C(0x1), seq_table_trp_meas_upd_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_meas_upd_item_s_names};

// TRP-PRS-Information-List-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_prs_info_list_item_s_fields[] = {
    {offsetof(trp_prs_info_list_item_s, trp_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(65535),
     nullptr},
    {offsetof(trp_prs_info_list_item_s, nr_pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(trp_prs_info_list_item_s, cgi_nr),
     (int32_t)offsetof(trp_prs_info_list_item_s, cgi_nr_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_nr_s::desc},
    {offsetof(trp_prs_info_list_item_s, prs_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_cfg_s::desc},
    {offsetof(trp_prs_info_list_item_s, ie_exts),
     (int32_t)offsetof(trp_prs_info_list_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_prs_info_list_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_prs_info_list_item_s_names[] = {{"tRP-ID", nullptr},
                                                                                   {"nR-PCI", nullptr},
                                                                                   {"cGI-NR", nullptr},
                                                                                   {"pRSConfiguration", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_trp_prs_info_list_item_s_opts[]  = {
    {2, (int32_t)offsetof(trp_prs_info_list_item_s, cgi_nr_present)},
    {4, (int32_t)offsetof(trp_prs_info_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc trp_prs_info_list_item_s::desc = {
    {seq_table_trp_prs_info_list_item_s_fields, 5, UINT64_C(0xb), seq_table_trp_prs_info_list_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_prs_info_list_item_s_names};

// TRPBeamAntennaInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_beam_ant_info_s_fields[] = {
    {offsetof(trp_beam_ant_info_s, choice_trp_beam_ant_info_item),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &choice_trp_beam_ant_info_item_c::desc},
    {offsetof(trp_beam_ant_info_s, ie_exts),
     (int32_t)offsetof(trp_beam_ant_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_beam_ant_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_beam_ant_info_s_names[] = {
    {"choice-TRP-Beam-Antenna-Info-Item", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_trp_beam_ant_info_s_opts[] = {
    {1, (int32_t)offsetof(trp_beam_ant_info_s, ie_exts_present)}};
const asn1::table::seq_desc trp_beam_ant_info_s::desc = {
    {seq_table_trp_beam_ant_info_s_fields, 2, UINT64_C(0x1), seq_table_trp_beam_ant_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_beam_ant_info_s_names};

// TRPType ::= ENUMERATED
static const char* enum_names_trp_type_e_values[] = {"prsOnlyTP", "srsOnlyRP", "tp", "rp", "trp", "mobile-trp"};
const asn1::table::enum_desc trp_type_opts::desc  = {enum_names_trp_type_e_values, 6, "trp_type_e"};

// TRPTEGItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_trpteg_item_s_fields[] = {
    {offsetof(trpteg_item_s, trp_tx_teg_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_tx_teg_info_s::desc},
    {offsetof(trpteg_item_s, dl_prs_res_set_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(trpteg_item_s, dl_prs_res_id_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(trpteg_item_s::dl_prs_res_id_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &dl_prs_res_id_item_s::desc},
    {offsetof(trpteg_item_s, ie_exts),
     (int32_t)offsetof(trpteg_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trpteg_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trpteg_item_s_names[] = {{"tRP-Tx-TEGInformation", nullptr},
                                                                        {"dl-PRSResourceSetID", nullptr},
                                                                        {"dl-PRSResourceID-List", nullptr},
                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_trpteg_item_s_opts[]  = {
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(trpteg_item_s, ie_exts_present)}};
const asn1::table::seq_desc trpteg_item_s::desc = {
    {seq_table_trpteg_item_s_fields, 4, UINT64_C(0x3), seq_table_trpteg_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_trpteg_item_s_names};

// TRPInformationTypeResponseItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_trp_info_type_resp_item_ext_ies_o[] = {62, 68, 84, 93, 114, 117};
static const uint8_t  objset_crit_trp_info_type_resp_item_ext_ies_o[] =
    {crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::reject};
static const uint8_t objset_value_trp_info_type_resp_item_ext_ies_o[] = {
    trp_info_type_resp_item_ext_ies_o::value_c::types::trp_type,
    trp_info_type_resp_item_ext_ies_o::value_c::types::on_demand_prs,
    trp_info_type_resp_item_ext_ies_o::value_c::types::trp_tx_teg_assoc,
    trp_info_type_resp_item_ext_ies_o::value_c::types::trp_beam_ant_info,
    trp_info_type_resp_item_ext_ies_o::value_c::types::mobile_trp_location_info,
    trp_info_type_resp_item_ext_ies_o::value_c::types::common_ta_params};
static const uint8_t                    objset_presence_trp_info_type_resp_item_ext_ies_o[] = {presence_e::mandatory,
                                                                                               presence_e::mandatory,
                                                                                               presence_e::mandatory,
                                                                                               presence_e::mandatory,
                                                                                               presence_e::mandatory,
                                                                                               presence_e::mandatory};
static const asn1::table::objset_column objset_columns_trp_info_type_resp_item_ext_ies_o[]  = {
    {objset_crit_trp_info_type_resp_item_ext_ies_o, crit_e::nulltype},
    {objset_value_trp_info_type_resp_item_ext_ies_o, trp_info_type_resp_item_ext_ies_o::value_c::types::nulltype},
    {objset_presence_trp_info_type_resp_item_ext_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc trp_info_type_resp_item_ext_ies_o::desc =
    {objset_id_trp_info_type_resp_item_ext_ies_o, 6, "id", objset_columns_trp_info_type_resp_item_ext_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_trp_info_type_resp_item_ext_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<trp_type_e>(),
     trp_type_e::has_ext,
     false,
     trp_type_e::nof_types,
     trp_type_e::nof_exts,
     &asn1::table::any_value_codec<trp_type_e>::value,
     &trp_type_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<on_demand_prs_info_s>::value,
     &on_demand_prs_info_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::any_list_codec<trp_tx_teg_assoc_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<trp_beam_ant_info_s>::value,
     &trp_beam_ant_info_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<mobile_trp_location_info_s>::value,
     &mobile_trp_location_info_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<common_ta_params_s>::value,
     &common_ta_params_s::desc}};
static const asn1::table::field_name choice_table_trp_info_type_resp_item_ext_ies_o_value_c_names[] = {
    {"TRPType", nullptr},
    {"OnDemandPRS-Info", nullptr},
    {"TRPTxTEGAssociation", nullptr},
    {"TRPBeamAntennaInformation", nullptr},
    {"Mobile-TRP-LocationInformation", nullptr},
    {"CommonTAParameters", nullptr}};
const asn1::table::choice_desc trp_info_type_resp_item_ext_ies_o::value_c::desc = {
    asn1::table::enum_int_type<trp_info_type_resp_item_ext_ies_o::value_c::types>(),
    trp_info_type_resp_item_ext_ies_o::value_c::types::nof_types,
    trp_info_type_resp_item_ext_ies_o::value_c::types::nof_exts,
    trp_info_type_resp_item_ext_ies_o::value_c::types::has_ext,
    true,
    choice_table_trp_info_type_resp_item_ext_ies_o_value_c_alternatives,
    &asn1::table::choice_set<trp_info_type_resp_item_ext_ies_o::value_c>,
    choice_table_trp_info_type_resp_item_ext_ies_o_value_c_names,
    "trp_info_type_resp_item_ext_ies_o::value_c",
    offsetof(trp_info_type_resp_item_ext_ies_o::value_c, type_),
    offsetof(trp_info_type_resp_item_ext_ies_o::value_c, c),
    true,
    nullptr,
    true};

void trp_info_type_resp_item_ext_ies_o::value_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, trp_info_type_resp_item_ext_ies_o::value_c::desc);
}
trp_type_e& trp_info_type_resp_item_ext_ies_o::value_c::trp_type()
{
  assert_choice_type(types::trp_type, type_, "Value");
  return c.get<trp_type_e>();
}
on_demand_prs_info_s& trp_info_type_resp_item_ext_ies_o::value_c::on_demand_prs()
{
  assert_choice_type(types::on_demand_prs, type_, "Value");
  return c.get<on_demand_prs_info_s>();
}
trp_tx_teg_assoc_l& trp_info_type_resp_item_ext_ies_o::value_c::trp_tx_teg_assoc()
{
  assert_choice_type(types::trp_tx_teg_assoc, type_, "Value");
  return c.get<trp_tx_teg_assoc_l>();
}
trp_beam_ant_info_s& trp_info_type_resp_item_ext_ies_o::value_c::trp_beam_ant_info()
{
  assert_choice_type(types::trp_beam_ant_info, type_, "Value");
  return c.get<trp_beam_ant_info_s>();
}
mobile_trp_location_info_s& trp_info_type_resp_item_ext_ies_o::value_c::mobile_trp_location_info()
{
  assert_choice_type(types::mobile_trp_location_info, type_, "Value");
  return c.get<mobile_trp_location_info_s>();
}
common_ta_params_s& trp_info_type_resp_item_ext_ies_o::value_c::common_ta_params()
{
  assert_choice_type(types::common_ta_params, type_, "Value");
  return c.get<common_ta_params_s>();
}
const trp_type_e& trp_info_type_resp_item_ext_ies_o::value_c::trp_type() const
{
  assert_choice_type(types::trp_type, type_, "Value");
  return c.get<trp_type_e>();
}
const on_demand_prs_info_s& trp_info_type_resp_item_ext_ies_o::value_c::on_demand_prs() const
{
  assert_choice_type(types::on_demand_prs, type_, "Value");
  return c.get<on_demand_prs_info_s>();
}
const trp_tx_teg_assoc_l& trp_info_type_resp_item_ext_ies_o::value_c::trp_tx_teg_assoc() const
{
  assert_choice_type(types::trp_tx_teg_assoc, type_, "Value");
  return c.get<trp_tx_teg_assoc_l>();
}
const trp_beam_ant_info_s& trp_info_type_resp_item_ext_ies_o::value_c::trp_beam_ant_info() const
{
  assert_choice_type(types::trp_beam_ant_info, type_, "Value");
  return c.get<trp_beam_ant_info_s>();
}
const mobile_trp_location_info_s& trp_info_type_resp_item_ext_ies_o::value_c::mobile_trp_location_info() const
{
  assert_choice_type(types::mobile_trp_location_info, type_, "Value");
  return c.get<mobile_trp_location_info_s>();
}
const common_ta_params_s& trp_info_type_resp_item_ext_ies_o::value_c::common_ta_params() const
{
  assert_choice_type(types::common_ta_params, type_, "Value");
  return c.get<common_ta_params_s>();
}

static const char* enum_names_trp_info_type_resp_item_ext_ies_o_value_c_types_values[] = {
    "TRPType",
    "OnDemandPRS-Info",
    "TRPTxTEGAssociation",
    "TRPBeamAntennaInformation",
    "Mobile-TRP-LocationInformation",
    "CommonTAParameters"};
const asn1::table::enum_desc trp_info_type_resp_item_ext_ies_o::value_c::types_opts::desc = {
    enum_names_trp_info_type_resp_item_ext_ies_o_value_c_types_values,
    6,
    "trp_info_type_resp_item_ext_ies_o::value_c::types"};

// TRPInformationTypeResponseItem ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_trp_info_type_resp_item_c_alternatives[] = {
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<cgi_nr_s, false>::value,
    &asn1::table::alternative_ops<uint32_t, true>::value,
    &asn1::table::alternative_ops<prs_cfg_s, false>::value,
    &asn1::table::alternative_ops<ssb_info_s, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<64, false, true>, false>::value,
    &asn1::table::alternative_ops<spatial_direction_info_s, false>::value,
    &asn1::table::alternative_ops<geographical_coordinates_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<trp_info_type_resp_item_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc trp_info_type_resp_item_c::storage_desc = {
    choice_storage_trp_info_type_resp_item_c_alternatives,
    9,
    "trp_info_type_resp_item_c"};

uint16_t& trp_info_type_resp_item_c::set_pci_nr()
{
  set(types::pci_nr);
  return c.get<uint16_t>();
}
cgi_nr_s& trp_info_type_resp_item_c::set_cgi_nr()
{
  set(types::cgi_nr);
  return c.get<cgi_nr_s>();
}
uint32_t& trp_info_type_resp_item_c::set_arfcn()
{
  set(types::arfcn);
  return c.get<uint32_t>();
}
prs_cfg_s& trp_info_type_resp_item_c::set_prs_cfg()
{
  set(types::prs_cfg);
  return c.get<prs_cfg_s>();
}
ssb_info_s& trp_info_type_resp_item_c::set_ss_binfo()
{
  set(types::ss_binfo);
  return c.get<ssb_info_s>();
}
fixed_bitstring<64, false, true>& trp_info_type_resp_item_c::set_sfn_initisation_time()
{
  set(types::sfn_initisation_time);
  return c.get<fixed_bitstring<64, false, true>>();
}
spatial_direction_info_s& trp_info_type_resp_item_c::set_spatial_direction_info()
{
  set(types::spatial_direction_info);
  return c.get<spatial_direction_info_s>();
}
geographical_coordinates_s& trp_info_type_resp_item_c::set_geographical_coordinates()
{
  set(types::geographical_coordinates);
  return c.get<geographical_coordinates_s>();
}
protocol_ie_single_container_s<trp_info_type_resp_item_ext_ies_o>& trp_info_type_resp_item_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<trp_info_type_resp_item_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_trp_info_type_resp_item_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_nr_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &prs_cfg_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ssb_info_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<64, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &spatial_direction_info_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &geographical_coordinates_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<trp_info_type_resp_item_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_trp_info_type_resp_item_c_names[] = {
    {"pCI-NR", nullptr},
    {"cGI-NR", nullptr},
    {"aRFCN", nullptr},
    {"pRSConfiguration", nullptr},
    {"sSBinformation", nullptr},
    {"sFNInitialisationTime", nullptr},
    {"spatialDirectionInformation", nullptr},
    {"geographicalCoordinates", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc trp_info_type_resp_item_c::desc = {
    asn1::table::enum_int_type<trp_info_type_resp_item_c::types>(),
    trp_info_type_resp_item_c::types::nof_types,
    trp_info_type_resp_item_c::types::nof_exts,
    trp_info_type_resp_item_c::types::has_ext,
    true,
    choice_table_trp_info_type_resp_item_c_alternatives,
    nullptr,
    choice_table_trp_info_type_resp_item_c_names,
    "trp_info_type_resp_item_c",
    offsetof(trp_info_type_resp_item_c, c.type),
    offsetof(trp_info_type_resp_item_c, c.value),
    false,
    &trp_info_type_resp_item_c::storage_desc};

static const char*           enum_names_trp_info_type_resp_item_c_types_values[] = {"pCI-NR",
                                                                                    "cGI-NR",
                                                                                    "aRFCN",
                                                                                    "pRSConfiguration",
                                                                                    "sSBinformation",
                                                                                    "sFNInitialisationTime",
                                                                                    "spatialDirectionInformation",
                                                                                    "geographicalCoordinates",
                                                                                    "choice-extension"};
const asn1::table::enum_desc trp_info_type_resp_item_c::types_opts::desc         = {
    enum_names_trp_info_type_resp_item_c_types_values,
    9,
    "trp_info_type_resp_item_c::types"};

static const uint32_t objset_id_trp_info_ext_ies_o[]   = {115};
static const uint8_t  objset_crit_trp_info_ext_ies_o[] = {crit_e::reject};

// TRPInformation-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_trp_info_ext_ies_container_values[] = {
    {offsetof(trp_info_ext_ies_container, mobile_iab_mt_ue_id),
     (int32_t)offsetof(trp_info_ext_ies_container, mobile_iab_mt_ue_id_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_info_ext_ies_container::mobile_iab_mt_ue_id),
                                asn1::table::string_json>::value}};
static const int32_t ie_table_trp_info_ext_ies_container_presences[] = {
    (int32_t)offsetof(trp_info_ext_ies_container, mobile_iab_mt_ue_id_present)};
const asn1::table::ie_container_desc trp_info_ext_ies_container::desc = {objset_id_trp_info_ext_ies_o,
                                                                         objset_crit_trp_info_ext_ies_o,
                                                                         ie_table_trp_info_ext_ies_container_values,
                                                                         ie_table_trp_info_ext_ies_container_presences,
                                                                         1,
                                                                         0,
                                                                         1,
                                                                         65535,
                                                                         true,
                                                                         "extensionValue"};

// TRPInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_info_s_fields[] = {
    {offsetof(trp_info_s, trp_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(65535),
     nullptr},
    {offsetof(trp_info_s, trp_info_type_resp_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(trp_info_s::trp_info_type_resp_list),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &trp_info_type_resp_item_c::desc},
    {offsetof(trp_info_s, ie_exts),
     (int32_t)offsetof(trp_info_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_info_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_trp_info_s_names[] = {{"tRP-ID", nullptr},
                                                                     {"tRPInformationTypeResponseList", nullptr},
                                                                     {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_trp_info_s_opts[] = {{2, (int32_t)offsetof(trp_info_s, ie_exts_present)}};
const asn1::table::seq_desc       trp_info_s::desc            = {
    {seq_table_trp_info_s_fields, 3, UINT64_C(0x3), seq_table_trp_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_info_s_names};

static const asn1::table::seq_field seq_table_trp_info_list_trp_resp_item_s__fields[] = {
    {offsetof(trp_info_list_trp_resp_item_s_, trp_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_info_s::desc},
    {offsetof(trp_info_list_trp_resp_item_s_, ie_exts),
     (int32_t)offsetof(trp_info_list_trp_resp_item_s_, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_info_list_trp_resp_item_s_::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_info_list_trp_resp_item_s__names[] = {{"tRPInformation", nullptr},
                                                                                         {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_trp_info_list_trp_resp_item_s__opts[]  = {
    {1, (int32_t)offsetof(trp_info_list_trp_resp_item_s_, ie_exts_present)}};
const asn1::table::seq_desc trp_info_list_trp_resp_item_s_::desc = {{seq_table_trp_info_list_trp_resp_item_s__fields,
                                                                     2,
                                                                     UINT64_C(0x1),
                                                                     seq_table_trp_info_list_trp_resp_item_s__opts,
                                                                     1},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_trp_info_list_trp_resp_item_s__names};

// TRPInformationTypeItem ::= ENUMERATED
static const char*           enum_names_trp_info_type_item_e_values[] = {"nrPCI",
                                                                         "nG-RAN-CGI",
                                                                         "arfcn",
                                                                         "pRSConfig",
                                                                         "sSBInfo",
                                                                         "sFNInitTime",
                                                                         "spatialDirectInfo",
                                                                         "geoCoord",
                                                                         "trp-type",
                                                                         "ondemandPRSInfo",
                                                                         "trpTxTeg",
                                                                         "beam-antenna-info",
                                                                         "mobile-trp-location-info",
                                                                         "commonTA"};
const asn1::table::enum_desc trp_info_type_item_opts::desc            = {enum_names_trp_info_type_item_e_values,
                                                                         14,
                                                                         "trp_info_type_item_e"};

// TRPInformationTypeItemTRPReq ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_trp_info_type_item_trp_req_o[]    = {57};
static const uint8_t  objset_crit_trp_info_type_item_trp_req_o[]  = {crit_e::reject};
static const uint8_t  objset_value_trp_info_type_item_trp_req_o[] = {
    trp_info_type_item_trp_req_o::value_c::types::trp_info_type_item};
static const uint8_t                    objset_presence_trp_info_type_item_trp_req_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_trp_info_type_item_trp_req_o[]  = {
    {objset_crit_trp_info_type_item_trp_req_o, crit_e::nulltype},
    {objset_value_trp_info_type_item_trp_req_o, trp_info_type_item_trp_req_o::value_c::types::nulltype},
    {objset_presence_trp_info_type_item_trp_req_o, presence_e::nulltype}};
const asn1::table::objset_desc trp_info_type_item_trp_req_o::desc = {objset_id_trp_info_type_item_trp_req_o,
                                                                     1,
                                                                     "id",
                                                                     objset_columns_trp_info_type_item_trp_req_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_trp_info_type_item_trp_req_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<trp_info_type_item_e>(),
     trp_info_type_item_e::has_ext,
     false,
     trp_info_type_item_e::nof_types,
     trp_info_type_item_e::nof_exts,
     nullptr,
     &trp_info_type_item_opts::desc}};
static const asn1::table::field_name choice_table_trp_info_type_item_trp_req_o_value_c_names[] = {
    {"TRPInformationTypeItem", nullptr}};
const asn1::table::choice_desc trp_info_type_item_trp_req_o::value_c::desc = {
    asn1::table::enum_int_type<trp_info_type_item_trp_req_o::value_c::types>(),
    trp_info_type_item_trp_req_o::value_c::types::nof_types,
    trp_info_type_item_trp_req_o::value_c::types::nof_exts,
    trp_info_type_item_trp_req_o::value_c::types::has_ext,
    true,
    choice_table_trp_info_type_item_trp_req_o_value_c_alternatives,
    nullptr,
    choice_table_trp_info_type_item_trp_req_o_value_c_names,
    "trp_info_type_item_trp_req_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(trp_info_type_item_trp_req_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_trp_info_type_item_trp_req_o_value_c_types_values[]    = {"TRPInformationTypeItem"};
const asn1::table::enum_desc trp_info_type_item_trp_req_o::value_c::types_opts::desc = {
    enum_names_trp_info_type_item_trp_req_o_value_c_types_values,
    1,
    "trp_info_type_item_trp_req_o::value_c::types"};

// TRPItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_item_s_fields[] = {
    {offsetof(trp_item_s, trp_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(65535),
     nullptr},
    {offsetof(trp_item_s, ie_exts),
     (int32_t)offsetof(trp_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trp_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_trp_item_s_names[] = {{"tRP-ID", nullptr}, {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_trp_item_s_opts[] = {{1, (int32_t)offsetof(trp_item_s, ie_exts_present)}};
const asn1::table::seq_desc       trp_item_s::desc            = {
    {seq_table_trp_item_s_fields, 2, UINT64_C(0x1), seq_table_trp_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_trp_item_s_names};

// TRPMeasurementType ::= ENUMERATED
static const char* enum_names_trp_meas_type_e_values[] =
    {"gNB-RxTxTimeDiff", "uL-SRS-RSRP", "uL-AoA", "uL-RTOA", "multiple-UL-AoA", "uL-SRS-RSRPP", "ul-RSCP"};
const asn1::table::enum_desc trp_meas_type_opts::desc = {enum_names_trp_meas_type_e_values, 7, "trp_meas_type_e"};

static const uint32_t objset_id_trp_meas_quantities_list_item_ext_ies_o[]   = {123};
static const uint8_t  objset_crit_trp_meas_quantities_list_item_ext_ies_o[] = {crit_e::ignore};

// TRPMeasurementQuantitiesList-Item-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_trp_meas_quantities_list_item_ext_ies_container_values[] = {
    {offsetof(trp_meas_quantities_list_item_ext_ies_container, timing_report_granularity_factor_extended),
     (int32_t)offsetof(trp_meas_quantities_list_item_ext_ies_container,
                       timing_report_granularity_factor_extended_present),
     asn1::table::field_op::int_i8,
     true,
     true,
     INT64_C(-6),
     UINT64_C(-1),
     nullptr}};
static const int32_t ie_table_trp_meas_quantities_list_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(trp_meas_quantities_list_item_ext_ies_container,
                      timing_report_granularity_factor_extended_present)};
const asn1::table::ie_container_desc trp_meas_quantities_list_item_ext_ies_container::desc = {
    objset_id_trp_meas_quantities_list_item_ext_ies_o,
    objset_crit_trp_meas_quantities_list_item_ext_ies_o,
    ie_table_trp_meas_quantities_list_item_ext_ies_container_values,
    ie_table_trp_meas_quantities_list_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// TRPMeasurementQuantitiesList-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_trp_meas_quantities_list_item_s_fields[] = {
    {offsetof(trp_meas_quantities_list_item_s, trp_meas_quantities_item),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(trp_meas_quantities_list_item_s::trp_meas_quantities_item)>(),
     decltype(trp_meas_quantities_list_item_s::trp_meas_quantities_item)::has_ext,
     false,
     decltype(trp_meas_quantities_list_item_s::trp_meas_quantities_item)::nof_types,
     decltype(trp_meas_quantities_list_item_s::trp_meas_quantities_item)::nof_exts,
     nullptr,
     &trp_meas_type_opts::desc},
    {offsetof(trp_meas_quantities_list_item_s, timing_report_granularity_factor),
     (int32_t)offsetof(trp_meas_quantities_list_item_s, timing_report_granularity_factor_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(5),
     nullptr},
    {offsetof(trp_meas_quantities_list_item_s, ie_exts),
     (int32_t)offsetof(trp_meas_quantities_list_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &trp_meas_quantities_list_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_trp_meas_quantities_list_item_s_names[] = {
    {"tRPMeasurementQuantities-Item", nullptr},
    {"timingReportingGranularityFactor", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_trp_meas_quantities_list_item_s_opts[] = {
    {1, (int32_t)offsetof(trp_meas_quantities_list_item_s, timing_report_granularity_factor_present)},
    {2, (int32_t)offsetof(trp_meas_quantities_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc trp_meas_quantities_list_item_s::desc = {{seq_table_trp_meas_quantities_list_item_s_fields,
                                                                      3,
                                                                      UINT64_C(0x1),
                                                                      seq_table_trp_meas_quantities_list_item_s_opts,
                                                                      2},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_trp_meas_quantities_list_item_s_names};

// TimeWindowDurationMeasurement ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_time_win_dur_meas_c_alternatives[] = {
    &asn1::table::alternative_ops<time_win_dur_meas_c::dur_slots_e_, true>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<time_win_dur_meas_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc time_win_dur_meas_c::storage_desc = {
    choice_storage_time_win_dur_meas_c_alternatives,
    2,
    "time_win_dur_meas_c"};

time_win_dur_meas_c::dur_slots_e_& time_win_dur_meas_c::set_dur_slots()
{
  set(types::dur_slots);
  return c.get<dur_slots_e_>();
}
protocol_ie_single_container_s<time_win_dur_meas_ext_ies_o>& time_win_dur_meas_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<time_win_dur_meas_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_time_win_dur_meas_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<time_win_dur_meas_c::dur_slots_e_>(),
     time_win_dur_meas_c::dur_slots_e_::has_ext,
     false,
     time_win_dur_meas_c::dur_slots_e_::nof_types,
     time_win_dur_meas_c::dur_slots_e_::nof_exts,
     nullptr,
     &time_win_dur_meas_c::dur_slots_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<time_win_dur_meas_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_time_win_dur_meas_c_names[] = {{"durationSlots", nullptr},
                                                                                 {"choice-extension", nullptr}};
const asn1::table::choice_desc time_win_dur_meas_c::desc = {asn1::table::enum_int_type<time_win_dur_meas_c::types>(),
                                                            time_win_dur_meas_c::types::nof_types,
                                                            time_win_dur_meas_c::types::nof_exts,
                                                            time_win_dur_meas_c::types::has_ext,
                                                            true,
                                                            choice_table_time_win_dur_meas_c_alternatives,
                                                            nullptr,
                                                            choice_table_time_win_dur_meas_c_names,
                                                            "time_win_dur_meas_c",
                                                            offsetof(time_win_dur_meas_c, c.type),
                                                            offsetof(time_win_dur_meas_c, c.value),
                                                            false,
                                                            &time_win_dur_meas_c::storage_desc};

static const char* enum_names_time_win_dur_meas_c_dur_slots_e__values[] = {"n1", "n2", "n4", "n6", "n8", "n12", "n16"};
const asn1::table::enum_desc time_win_dur_meas_c::dur_slots_opts::desc  = {
    enum_names_time_win_dur_meas_c_dur_slots_e__values,
    7,
    "time_win_dur_meas_c::dur_slots_e_"};
uint8_t time_win_dur_meas_c::dur_slots_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 6, 8, 12, 16};
  return map_enum_number(numbers, 7, value, "time_win_dur_meas_c::dur_slots_e_");
}

static const char*           enum_names_time_win_dur_meas_c_types_values[] = {"durationSlots", "choice-extension"};
const asn1::table::enum_desc time_win_dur_meas_c::types_opts::desc = {enum_names_time_win_dur_meas_c_types_values,
                                                                      2,
                                                                      "time_win_dur_meas_c::types"};

// TimeWindowDurationSRS ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_time_win_dur_srs_c_alternatives[] = {
    &asn1::table::alternative_ops<time_win_dur_srs_c::dur_symbols_e_, true>::value,
    &asn1::table::alternative_ops<time_win_dur_srs_c::dur_slots_e_, true>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<time_win_dur_srs_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc time_win_dur_srs_c::storage_desc = {
    choice_storage_time_win_dur_srs_c_alternatives,
    3,
    "time_win_dur_srs_c"};

time_win_dur_srs_c::dur_symbols_e_& time_win_dur_srs_c::set_dur_symbols()
{
  set(types::dur_symbols);
  return c.get<dur_symbols_e_>();
}
time_win_dur_srs_c::dur_slots_e_& time_win_dur_srs_c::set_dur_slots()
{
  set(types::dur_slots);
  return c.get<dur_slots_e_>();
}
protocol_ie_single_container_s<time_win_dur_srs_ext_ies_o>& time_win_dur_srs_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<time_win_dur_srs_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_time_win_dur_srs_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<time_win_dur_srs_c::dur_symbols_e_>(),
     time_win_dur_srs_c::dur_symbols_e_::has_ext,
     false,
     time_win_dur_srs_c::dur_symbols_e_::nof_types,
     time_win_dur_srs_c::dur_symbols_e_::nof_exts,
     nullptr,
     &time_win_dur_srs_c::dur_symbols_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<time_win_dur_srs_c::dur_slots_e_>(),
     time_win_dur_srs_c::dur_slots_e_::has_ext,
     false,
     time_win_dur_srs_c::dur_slots_e_::nof_types,
     time_win_dur_srs_c::dur_slots_e_::nof_exts,
     nullptr,
     &time_win_dur_srs_c::dur_slots_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<time_win_dur_srs_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_time_win_dur_srs_c_names[] = {{"durationSymbols", nullptr},
                                                                                {"durationSlots", nullptr},
                                                                                {"choice-extension", nullptr}};
const asn1::table::choice_desc time_win_dur_srs_c::desc = {asn1::table::enum_int_type<time_win_dur_srs_c::types>(),
                                                           time_win_dur_srs_c::types::nof_types,
                                                           time_win_dur_srs_c::types::nof_exts,
                                                           time_win_dur_srs_c::types::has_ext,
                                                           true,
                                                           choice_table_time_win_dur_srs_c_alternatives,
                                                           nullptr,
                                                           choice_table_time_win_dur_srs_c_names,
                                                           "time_win_dur_srs_c",
                                                           offsetof(time_win_dur_srs_c, c.type),
                                                           offsetof(time_win_dur_srs_c, c.value),
                                                           false,
                                                           &time_win_dur_srs_c::storage_desc};

static const char*           enum_names_time_win_dur_srs_c_dur_symbols_e__values[] = {"n1", "n2", "n4", "n8", "n12"};
const asn1::table::enum_desc time_win_dur_srs_c::dur_symbols_opts::desc            = {
    enum_names_time_win_dur_srs_c_dur_symbols_e__values,
    5,
    "time_win_dur_srs_c::dur_symbols_e_"};
uint8_t time_win_dur_srs_c::dur_symbols_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 12};
  return map_enum_number(numbers, 5, value, "time_win_dur_srs_c::dur_symbols_e_");
}

static const char* enum_names_time_win_dur_srs_c_dur_slots_e__values[] = {"n1", "n2", "n4", "n6", "n8", "n12", "n16"};
const asn1::table::enum_desc time_win_dur_srs_c::dur_slots_opts::desc  = {
    enum_names_time_win_dur_srs_c_dur_slots_e__values,
    7,
    "time_win_dur_srs_c::dur_slots_e_"};
uint8_t time_win_dur_srs_c::dur_slots_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 6, 8, 12, 16};
  return map_enum_number(numbers, 7, value, "time_win_dur_srs_c::dur_slots_e_");
}

static const char*           enum_names_time_win_dur_srs_c_types_values[] = {"durationSymbols",
                                                                             "durationSlots",
                                                                             "choice-extension"};
const asn1::table::enum_desc time_win_dur_srs_c::types_opts::desc         = {enum_names_time_win_dur_srs_c_types_values,
                                                                             3,
                                                                             "time_win_dur_srs_c::types"};

// TimeWindowPeriodicityMeasurement ::= ENUMERATED
static const char*           enum_names_time_win_periodicity_meas_e_values[] = {"ms160",
                                                                                "ms320",
                                                                                "ms640",
                                                                                "ms1280",
                                                                                "ms2560",
                                                                                "ms5120",
                                                                                "ms10240",
                                                                                "ms20480",
                                                                                "ms40960",
                                                                                "ms61440",
                                                                                "ms81920",
                                                                                "ms368640",
                                                                                "ms737280",
                                                                                "ms1843200"};
const asn1::table::enum_desc time_win_periodicity_meas_opts::desc = {enum_names_time_win_periodicity_meas_e_values,
                                                                     14,
                                                                     "time_win_periodicity_meas_e"};
uint32_t                     time_win_periodicity_meas_opts::to_number() const
{
  static const uint32_t numbers[] = {
      160, 320, 640, 1280, 2560, 5120, 10240, 20480, 40960, 61440, 81920, 368640, 737280, 1843200};
  return map_enum_number(numbers, 14, value, "time_win_periodicity_meas_e");
}

// TimeWindowStart ::= SEQUENCE
static const asn1::table::seq_field seq_table_time_win_start_s_fields[] = {
    {offsetof(time_win_start_s, sys_frame_num),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {offsetof(time_win_start_s, slot_num),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(79),
     nullptr},
    {offsetof(time_win_start_s, symbol_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(13),
     nullptr},
    {offsetof(time_win_start_s, ie_ext),
     (int32_t)offsetof(time_win_start_s, ie_ext_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(time_win_start_s::ie_ext), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_time_win_start_s_names[] = {{"systemFrameNumber", nullptr},
                                                                           {"slotNumber", nullptr},
                                                                           {"symbolIndex", nullptr},
                                                                           {"iE-Extension", nullptr}};
static const asn1::table::seq_opt    seq_table_time_win_start_s_opts[]  = {
    {3, (int32_t)offsetof(time_win_start_s, ie_ext_present)}};
const asn1::table::seq_desc time_win_start_s::desc = {
    {seq_table_time_win_start_s_fields, 4, UINT64_C(0x7), seq_table_time_win_start_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_time_win_start_s_names};

// TimeWindowInformation-Measurement-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_time_win_info_meas_item_s_fields[] = {
    {offsetof(time_win_info_meas_item_s, time_win_dur_meas),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &time_win_dur_meas_c::desc},
    {offsetof(time_win_info_meas_item_s, time_win_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(time_win_info_meas_item_s::time_win_type)>(),
     decltype(time_win_info_meas_item_s::time_win_type)::has_ext,
     false,
     decltype(time_win_info_meas_item_s::time_win_type)::nof_types,
     decltype(time_win_info_meas_item_s::time_win_type)::nof_exts,
     nullptr,
     &time_win_info_meas_item_s::time_win_type_opts::desc},
    {offsetof(time_win_info_meas_item_s, time_win_periodicity_meas),
     (int32_t)offsetof(time_win_info_meas_item_s, time_win_periodicity_meas_present),
     asn1::table::enum_op<decltype(time_win_info_meas_item_s::time_win_periodicity_meas)>(),
     decltype(time_win_info_meas_item_s::time_win_periodicity_meas)::has_ext,
     false,
     decltype(time_win_info_meas_item_s::time_win_periodicity_meas)::nof_types,
     decltype(time_win_info_meas_item_s::time_win_periodicity_meas)::nof_exts,
     nullptr,
     &time_win_periodicity_meas_opts::desc},
    {offsetof(time_win_info_meas_item_s, time_win_start),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &time_win_start_s::desc},
    {offsetof(time_win_info_meas_item_s, ie_ext),
     (int32_t)offsetof(time_win_info_meas_item_s, ie_ext_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(time_win_info_meas_item_s::ie_ext), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_time_win_info_meas_item_s_names[] = {
    {"timeWindowDurationMeasurement", nullptr},
    {"timeWindowType", nullptr},
    {"timeWindowPeriodicityMeasurement", nullptr},
    {"timeWindowStart", nullptr},
    {"iE-Extension", nullptr}};
static const asn1::table::seq_opt seq_table_time_win_info_meas_item_s_opts[] = {
    {2, (int32_t)offsetof(time_win_info_meas_item_s, time_win_periodicity_meas_present)},
    {4, (int32_t)offsetof(time_win_info_meas_item_s, ie_ext_present)}};
const asn1::table::seq_desc time_win_info_meas_item_s::desc = {
    {seq_table_time_win_info_meas_item_s_fields, 5, UINT64_C(0xb), seq_table_time_win_info_meas_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_time_win_info_meas_item_s_names};

static const char*           enum_names_time_win_info_meas_item_s_time_win_type_e__values[] = {"single", "periodic"};
const asn1::table::enum_desc time_win_info_meas_item_s::time_win_type_opts::desc            = {
    enum_names_time_win_info_meas_item_s_time_win_type_e__values,
    2,
    "time_win_info_meas_item_s::time_win_type_e_"};

// TimeWindowStartSRS ::= SEQUENCE
static const asn1::table::seq_field seq_table_time_win_start_srs_s_fields[] = {
    {offsetof(time_win_start_srs_s, sys_frame_num),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {offsetof(time_win_start_srs_s, slot_num),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(79),
     nullptr},
    {offsetof(time_win_start_srs_s, symbol_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(13),
     nullptr},
    {offsetof(time_win_start_srs_s, ie_ext),
     (int32_t)offsetof(time_win_start_srs_s, ie_ext_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(time_win_start_srs_s::ie_ext), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_time_win_start_srs_s_names[] = {{"systemFrameNumber", nullptr},
                                                                               {"slotNumber", nullptr},
                                                                               {"symbolIndex", nullptr},
                                                                               {"iE-Extension", nullptr}};
static const asn1::table::seq_opt    seq_table_time_win_start_srs_s_opts[]  = {
    {3, (int32_t)offsetof(time_win_start_srs_s, ie_ext_present)}};
const asn1::table::seq_desc time_win_start_srs_s::desc = {
    {seq_table_time_win_start_srs_s_fields, 4, UINT64_C(0x7), seq_table_time_win_start_srs_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_time_win_start_srs_s_names};

// TimeWindowPeriodicitySRS ::= ENUMERATED
static const char* enum_names_time_win_periodicity_srs_e_values[] = {
    "ms0dot125", "ms0dot25", "ms0dot5", "ms0dot625", "ms1",    "ms1dot25", "ms2",    "ms2dot5", "ms4",
    "ms5",       "ms8",      "ms10",    "ms16",      "ms20",   "ms32",     "ms40",   "ms64",    "ms80",
    "ms160",     "ms320",    "ms640",   "ms1280",    "ms2560", "ms5120",   "ms10240"};
const asn1::table::enum_desc time_win_periodicity_srs_opts::desc = {enum_names_time_win_periodicity_srs_e_values,
                                                                    25,
                                                                    "time_win_periodicity_srs_e"};
float                        time_win_periodicity_srs_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25,  0.5,   0.625,  1.0,    1.25,   2.0,    2.5,  4.0,
                                  5.0,   8.0,   10.0,  16.0,   20.0,   32.0,   40.0,   64.0, 80.0,
                                  160.0, 320.0, 640.0, 1280.0, 2560.0, 5120.0, 10240.0};
  return map_enum_number(numbers, 25, value, "time_win_periodicity_srs_e");
}
const char* time_win_periodicity_srs_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.125", "0.25", "0.5", "0.625", "1",    "1.25", "2",    "2.5", "4",
                                      "5",     "8",    "10",  "16",    "20",   "32",   "40",   "64",  "80",
                                      "160",   "320",  "640", "1280",  "2560", "5120", "10240"};
  return convert_enum_idx(number_strs, 25, value, "time_win_periodicity_srs_e");
}

// TimeWindowInformation-SRS-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_time_win_info_srs_item_s_fields[] = {
    {offsetof(time_win_info_srs_item_s, time_win_start_srs),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &time_win_start_srs_s::desc},
    {offsetof(time_win_info_srs_item_s, time_win_dur_srs),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &time_win_dur_srs_c::desc},
    {offsetof(time_win_info_srs_item_s, time_win_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(time_win_info_srs_item_s::time_win_type)>(),
     decltype(time_win_info_srs_item_s::time_win_type)::has_ext,
     false,
     decltype(time_win_info_srs_item_s::time_win_type)::nof_types,
     decltype(time_win_info_srs_item_s::time_win_type)::nof_exts,
     nullptr,
     &time_win_info_srs_item_s::time_win_type_opts::desc},
    {offsetof(time_win_info_srs_item_s, time_win_periodicity_srs),
     (int32_t)offsetof(time_win_info_srs_item_s, time_win_periodicity_srs_present),
     asn1::table::enum_op<decltype(time_win_info_srs_item_s::time_win_periodicity_srs)>(),
     decltype(time_win_info_srs_item_s::time_win_periodicity_srs)::has_ext,
     false,
     decltype(time_win_info_srs_item_s::time_win_periodicity_srs)::nof_types,
     decltype(time_win_info_srs_item_s::time_win_periodicity_srs)::nof_exts,
     nullptr,
     &time_win_periodicity_srs_opts::desc},
    {offsetof(time_win_info_srs_item_s, ie_ext),
     (int32_t)offsetof(time_win_info_srs_item_s, ie_ext_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(time_win_info_srs_item_s::ie_ext), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_time_win_info_srs_item_s_names[] = {
    {"timeWindowStartSRS", nullptr},
    {"timeWindowDurationSRS", nullptr},
    {"timeWindowType", nullptr},
    {"timeWindowPeriodicitySRS", nullptr},
    {"iE-Extension", nullptr}};
static const asn1::table::seq_opt seq_table_time_win_info_srs_item_s_opts[] = {
    {3, (int32_t)offsetof(time_win_info_srs_item_s, time_win_periodicity_srs_present)},
    {4, (int32_t)offsetof(time_win_info_srs_item_s, ie_ext_present)}};
const asn1::table::seq_desc time_win_info_srs_item_s::desc = {
    {seq_table_time_win_info_srs_item_s_fields, 5, UINT64_C(0x7), seq_table_time_win_info_srs_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_time_win_info_srs_item_s_names};

static const char*           enum_names_time_win_info_srs_item_s_time_win_type_e__values[] = {"single", "periodic"};
const asn1::table::enum_desc time_win_info_srs_item_s::time_win_type_opts::desc            = {
    enum_names_time_win_info_srs_item_s_time_win_type_e__values,
    2,
    "time_win_info_srs_item_s::time_win_type_e_"};

// UE-TEG-Info-Request ::= ENUMERATED
static const char*           enum_names_ue_teg_info_request_e_values[] = {"onDemand", "periodic", "stop"};
const asn1::table::enum_desc ue_teg_info_request_opts::desc            = {enum_names_ue_teg_info_request_e_values,
                                                                          3,
                                                                          "ue_teg_info_request_e"};

// UE-TEG-ReportingPeriodicity ::= ENUMERATED
static const char* enum_names_ue_teg_report_periodicity_e_values[] =
    {"ms160", "ms320", "ms1280", "ms2560", "ms61440", "ms81920", "ms368640", "ms737280"};
const asn1::table::enum_desc ue_teg_report_periodicity_opts::desc = {enum_names_ue_teg_report_periodicity_e_values,
                                                                     8,
                                                                     "ue_teg_report_periodicity_e"};
uint32_t                     ue_teg_report_periodicity_opts::to_number() const
{
  static const uint32_t numbers[] = {160, 320, 1280, 2560, 61440, 81920, 368640, 737280};
  return map_enum_number(numbers, 8, value, "ue_teg_report_periodicity_e");
}

static const uint32_t objset_id_ue_report_info_ext_ies_o[]   = {158};
static const uint8_t  objset_crit_ue_report_info_ext_ies_o[] = {crit_e::ignore};

// UEReportingInformation-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_ue_report_info_ext_ies_container_values[] = {
    {offsetof(ue_report_info_ext_ies_container, ue_report_interv_milliseconds),
     (int32_t)offsetof(ue_report_info_ext_ies_container, ue_report_interv_milliseconds_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(1),
     UINT64_C(999),
     nullptr}};
static const int32_t ie_table_ue_report_info_ext_ies_container_presences[] = {
    (int32_t)offsetof(ue_report_info_ext_ies_container, ue_report_interv_milliseconds_present)};
const asn1::table::ie_container_desc ue_report_info_ext_ies_container::desc = {
    objset_id_ue_report_info_ext_ies_o,
    objset_crit_ue_report_info_ext_ies_o,
    ie_table_ue_report_info_ext_ies_container_values,
    ie_table_ue_report_info_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// UEReportingInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_report_info_s_fields[] = {
    {offsetof(ue_report_info_s, report_amount),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ue_report_info_s::report_amount)>(),
     decltype(ue_report_info_s::report_amount)::has_ext,
     false,
     decltype(ue_report_info_s::report_amount)::nof_types,
     decltype(ue_report_info_s::report_amount)::nof_exts,
     nullptr,
     &ue_report_info_s::report_amount_opts::desc},
    {offsetof(ue_report_info_s, report_interv),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ue_report_info_s::report_interv)>(),
     decltype(ue_report_info_s::report_interv)::has_ext,
     false,
     decltype(ue_report_info_s::report_interv)::nof_types,
     decltype(ue_report_info_s::report_interv)::nof_exts,
     nullptr,
     &ue_report_info_s::report_interv_opts::desc},
    {offsetof(ue_report_info_s, ie_exts),
     (int32_t)offsetof(ue_report_info_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_report_info_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_ue_report_info_s_names[] = {{"reportingAmount", nullptr},
                                                                           {"reportingInterval", nullptr},
                                                                           {"iE-extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_report_info_s_opts[]  = {
    {2, (int32_t)offsetof(ue_report_info_s, ie_exts_present)}};
const asn1::table::seq_desc ue_report_info_s::desc = {
    {seq_table_ue_report_info_s_fields, 3, UINT64_C(0x3), seq_table_ue_report_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_ue_report_info_s_names};

static const char* enum_names_ue_report_info_s_report_amount_e__values[] =
    {"ma0", "ma1", "ma2", "ma4", "ma8", "ma16", "ma32", "ma64"};
const asn1::table::enum_desc ue_report_info_s::report_amount_opts::desc = {
    enum_names_ue_report_info_s_report_amount_e__values,
    8,
    "ue_report_info_s::report_amount_e_"};
uint8_t ue_report_info_s::report_amount_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 4, 8, 16, 32, 64};
  return map_enum_number(numbers, 8, value, "ue_report_info_s::report_amount_e_");
}

static const char* enum_names_ue_report_info_s_report_interv_e__values[] =
    {"none", "one", "two", "four", "eight", "ten", "sixteen", "twenty", "thirty-two", "sixty-four"};
const asn1::table::enum_desc ue_report_info_s::report_interv_opts::desc = {
    enum_names_ue_report_info_s_report_interv_e__values,
    10,
    "ue_report_info_s::report_interv_e_"};

static const uint32_t objset_id_ue_tx_teg_assoc_item_ext_ies_o[]   = {104};
static const uint8_t  objset_crit_ue_tx_teg_assoc_item_ext_ies_o[] = {crit_e::ignore};

// UETxTEGAssociationItem-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_ue_tx_teg_assoc_item_ext_ies_container_values[] = {
    {offsetof(ue_tx_teg_assoc_item_ext_ies_container, ue_tx_timing_error_margin),
     (int32_t)offsetof(ue_tx_teg_assoc_item_ext_ies_container, ue_tx_timing_error_margin_present),
     asn1::table::enum_op<decltype(ue_tx_teg_assoc_item_ext_ies_container::ue_tx_timing_error_margin)>(),
     decltype(ue_tx_teg_assoc_item_ext_ies_container::ue_tx_timing_error_margin)::has_ext,
     false,
     decltype(ue_tx_teg_assoc_item_ext_ies_container::ue_tx_timing_error_margin)::nof_types,
     decltype(ue_tx_teg_assoc_item_ext_ies_container::ue_tx_timing_error_margin)::nof_exts,
     nullptr,
     &timing_error_margin_opts::desc}};
static const int32_t ie_table_ue_tx_teg_assoc_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(ue_tx_teg_assoc_item_ext_ies_container, ue_tx_timing_error_margin_present)};
const asn1::table::ie_container_desc ue_tx_teg_assoc_item_ext_ies_container::desc = {
    objset_id_ue_tx_teg_assoc_item_ext_ies_o,
    objset_crit_ue_tx_teg_assoc_item_ext_ies_o,
    ie_table_ue_tx_teg_assoc_item_ext_ies_container_values,
    ie_table_ue_tx_teg_assoc_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// UETxTEGAssociationItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_tx_teg_assoc_item_s_fields[] = {
    {offsetof(ue_tx_teg_assoc_item_s, ue_tx_teg_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(ue_tx_teg_assoc_item_s, pos_srs_res_id_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::list_codec<decltype(ue_tx_teg_assoc_item_s::pos_srs_res_id_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(63), false, true>>::value},
    {offsetof(ue_tx_teg_assoc_item_s, time_stamp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &time_stamp_s::desc},
    {offsetof(ue_tx_teg_assoc_item_s, carrier_freq),
     (int32_t)offsetof(ue_tx_teg_assoc_item_s, carrier_freq_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &carrier_freq_s::desc},
    {offsetof(ue_tx_teg_assoc_item_s, ie_exts),
     (int32_t)offsetof(ue_tx_teg_assoc_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_tx_teg_assoc_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_ue_tx_teg_assoc_item_s_names[] = {{"uE-Tx-TEG-ID", nullptr},
                                                                                 {"posSRSResourceID-List", nullptr},
                                                                                 {"timeStamp", nullptr},
                                                                                 {"carrierFreq", nullptr},
                                                                                 {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_tx_teg_assoc_item_s_opts[]  = {
    {3, (int32_t)offsetof(ue_tx_teg_assoc_item_s, carrier_freq_present)},
    {4, (int32_t)offsetof(ue_tx_teg_assoc_item_s, ie_exts_present)}};
const asn1::table::seq_desc ue_tx_teg_assoc_item_s::desc = {
    {seq_table_ue_tx_teg_assoc_item_s_fields, 5, UINT64_C(0x7), seq_table_ue_tx_teg_assoc_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_ue_tx_teg_assoc_item_s_names};

// WLANBand ::= ENUMERATED
static const char*           enum_names_wlan_band_e_values[] = {"band2dot4", "band5"};
const asn1::table::enum_desc wlan_band_opts::desc            = {enum_names_wlan_band_e_values, 2, "wlan_band_e"};
float                        wlan_band_opts::to_number() const
{
  static const float numbers[] = {2.4, 5.0};
  return map_enum_number(numbers, 2, value, "wlan_band_e");
}
const char* wlan_band_opts::to_number_string() const
{
  static const char* number_strs[] = {"2.4", "5"};
  return convert_enum_idx(number_strs, 2, value, "wlan_band_e");
}

// WLANCountryCode ::= ENUMERATED
static const char*           enum_names_wlan_country_code_e_values[] = {"unitedStates", "europe", "japan", "global"};
const asn1::table::enum_desc wlan_country_code_opts::desc            = {enum_names_wlan_country_code_e_values,
                                                                        4,
                                                                        "wlan_country_code_e"};

// WLANMeasurementQuantitiesValue ::= ENUMERATED
static const char*           enum_names_wlan_meas_quantities_value_e_values[] = {"wlan"};
const asn1::table::enum_desc wlan_meas_quantities_value_opts::desc = {enum_names_wlan_meas_quantities_value_e_values,
                                                                      1,
                                                                      "wlan_meas_quantities_value_e"};

// WLANMeasurementQuantities-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_wlan_meas_quantities_item_s_fields[] = {
    {offsetof(wlan_meas_quantities_item_s, wlan_meas_quantities_value),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(wlan_meas_quantities_item_s::wlan_meas_quantities_value)>(),
     decltype(wlan_meas_quantities_item_s::wlan_meas_quantities_value)::has_ext,
     false,
     decltype(wlan_meas_quantities_item_s::wlan_meas_quantities_value)::nof_types,
     decltype(wlan_meas_quantities_item_s::wlan_meas_quantities_value)::nof_exts,
     nullptr,
     &wlan_meas_quantities_value_opts::desc},
    {offsetof(wlan_meas_quantities_item_s, ie_exts),
     (int32_t)offsetof(wlan_meas_quantities_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(wlan_meas_quantities_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_wlan_meas_quantities_item_s_names[] = {
    {"wLANMeasurementQuantitiesValue", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_wlan_meas_quantities_item_s_opts[] = {
    {1, (int32_t)offsetof(wlan_meas_quantities_item_s, ie_exts_present)}};
const asn1::table::seq_desc wlan_meas_quantities_item_s::desc = {
    {seq_table_wlan_meas_quantities_item_s_fields, 2, UINT64_C(0x1), seq_table_wlan_meas_quantities_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_wlan_meas_quantities_item_s_names};

// WLANMeasurementQuantities-ItemIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_wlan_meas_quantities_item_ies_o[]    = {20};
static const uint8_t  objset_crit_wlan_meas_quantities_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_wlan_meas_quantities_item_ies_o[] = {
    wlan_meas_quantities_item_ies_o::value_c::types::wlan_meas_quantities_item};
static const uint8_t                    objset_presence_wlan_meas_quantities_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_wlan_meas_quantities_item_ies_o[]  = {
    {objset_crit_wlan_meas_quantities_item_ies_o, crit_e::nulltype},
    {objset_value_wlan_meas_quantities_item_ies_o, wlan_meas_quantities_item_ies_o::value_c::types::nulltype},
    {objset_presence_wlan_meas_quantities_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc wlan_meas_quantities_item_ies_o::desc = {objset_id_wlan_meas_quantities_item_ies_o,
                                                                        1,
                                                                        "id",
                                                                        objset_columns_wlan_meas_quantities_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_wlan_meas_quantities_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &wlan_meas_quantities_item_s::desc}};
static const asn1::table::field_name choice_table_wlan_meas_quantities_item_ies_o_value_c_names[] = {
    {"WLANMeasurementQuantities-Item", nullptr}};
const asn1::table::choice_desc wlan_meas_quantities_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<wlan_meas_quantities_item_ies_o::value_c::types>(),
    wlan_meas_quantities_item_ies_o::value_c::types::nof_types,
    wlan_meas_quantities_item_ies_o::value_c::types::nof_exts,
    wlan_meas_quantities_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_wlan_meas_quantities_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_wlan_meas_quantities_item_ies_o_value_c_names,
    "wlan_meas_quantities_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(wlan_meas_quantities_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_wlan_meas_quantities_item_ies_o_value_c_types_values[] = {
    "WLANMeasurementQuantities-Item"};
const asn1::table::enum_desc wlan_meas_quantities_item_ies_o::value_c::types_opts::desc = {
    enum_names_wlan_meas_quantities_item_ies_o_value_c_types_values,
    1,
    "wlan_meas_quantities_item_ies_o::value_c::types"};

// WLANMeasurementResult-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_wlan_meas_result_item_s_fields[] = {
    {offsetof(wlan_meas_result_item_s, wlan_rssi),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(141),
     nullptr},
    {offsetof(wlan_meas_result_item_s, ssi_d),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(wlan_meas_result_item_s::ssi_d), asn1::table::string_json>::value},
    {offsetof(wlan_meas_result_item_s, bs_si_d),
     (int32_t)offsetof(wlan_meas_result_item_s, bs_si_d_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(wlan_meas_result_item_s::bs_si_d), asn1::table::string_json>::value},
    {offsetof(wlan_meas_result_item_s, hes_si_d),
     (int32_t)offsetof(wlan_meas_result_item_s, hes_si_d_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(wlan_meas_result_item_s::hes_si_d), asn1::table::string_json>::value},
    {offsetof(wlan_meas_result_item_s, operating_class),
     (int32_t)offsetof(wlan_meas_result_item_s, operating_class_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(wlan_meas_result_item_s, country_code),
     (int32_t)offsetof(wlan_meas_result_item_s, country_code_present),
     asn1::table::enum_op<decltype(wlan_meas_result_item_s::country_code)>(),
     decltype(wlan_meas_result_item_s::country_code)::has_ext,
     false,
     decltype(wlan_meas_result_item_s::country_code)::nof_types,
     decltype(wlan_meas_result_item_s::country_code)::nof_exts,
     nullptr,
     &wlan_country_code_opts::desc},
    {offsetof(wlan_meas_result_item_s, wlan_ch_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16,
     &asn1::table::list_codec<decltype(wlan_meas_result_item_s::wlan_ch_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(255), false, true>>::value},
    {offsetof(wlan_meas_result_item_s, wlan_band),
     (int32_t)offsetof(wlan_meas_result_item_s, wlan_band_present),
     asn1::table::enum_op<decltype(wlan_meas_result_item_s::wlan_band)>(),
     decltype(wlan_meas_result_item_s::wlan_band)::has_ext,
     false,
     decltype(wlan_meas_result_item_s::wlan_band)::nof_types,
     decltype(wlan_meas_result_item_s::wlan_band)::nof_exts,
     nullptr,
     &wlan_band_opts::desc},
    {offsetof(wlan_meas_result_item_s, ie_exts),
     (int32_t)offsetof(wlan_meas_result_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(wlan_meas_result_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_wlan_meas_result_item_s_names[] = {{"wLAN-RSSI", nullptr},
                                                                                  {"sSID", nullptr},
                                                                                  {"bSSID", nullptr},
                                                                                  {"hESSID", nullptr},
                                                                                  {"operatingClass", nullptr},
                                                                                  {"countryCode", nullptr},
                                                                                  {"wLANChannelList", nullptr},
                                                                                  {"wLANBand", nullptr},
                                                                                  {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_wlan_meas_result_item_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(wlan_meas_result_item_s, bs_si_d_present)},
    {3, (int32_t)offsetof(wlan_meas_result_item_s, hes_si_d_present)},
    {4, (int32_t)offsetof(wlan_meas_result_item_s, operating_class_present)},
    {5, (int32_t)offsetof(wlan_meas_result_item_s, country_code_present)},
    {6, asn1::table::seq_field::list_presence},
    {7, (int32_t)offsetof(wlan_meas_result_item_s, wlan_band_present)},
    {8, (int32_t)offsetof(wlan_meas_result_item_s, ie_exts_present)}};
const asn1::table::seq_desc wlan_meas_result_item_s::desc = {
    {seq_table_wlan_meas_result_item_s_fields, 9, UINT64_C(0x1), seq_table_wlan_meas_result_item_s_opts, 8},
    true,
    true,
    nullptr,
    0,
    seq_table_wlan_meas_result_item_s_names};
