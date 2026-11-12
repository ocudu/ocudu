// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/e2sm/e2sm_kpm_ies.h"
#include "ocudu/asn1/asn1_ap_codec.h"

using namespace asn1;
using namespace asn1::e2sm;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// AggregationFunctionType ::= ENUMERATED
static const char*           enum_names_aggregation_function_type_e_values[] = {"min", "max", "avg", "sum"};
const asn1::table::enum_desc aggregation_function_type_opts::desc = {enum_names_aggregation_function_type_e_values,
                                                                     4,
                                                                     "aggregation_function_type_e"};

// BinRangeValue ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_bin_range_value_c_alternatives[] = {
    &asn1::table::alternative_ops<int64_t, true>::value,
    &asn1::table::alternative_ops<real_s, false>::value};
const asn1::table::choice_storage_desc bin_range_value_c::storage_desc = {choice_storage_bin_range_value_c_alternatives,
                                                                          2,
                                                                          "bin_range_value_c"};

int64_t& bin_range_value_c::set_value_int()
{
  set(types::value_int);
  return c.get<int64_t>();
}
real_s& bin_range_value_c::set_value_real()
{
  set(types::value_real);
  return c.get<real_s>();
}
static const asn1::table::seq_field choice_table_bin_range_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<real_s, asn1::table::float_json>::value}};
static const asn1::table::field_name choice_table_bin_range_value_c_names[] = {{"valueInt", nullptr},
                                                                               {"valueReal", nullptr}};
const asn1::table::choice_desc       bin_range_value_c::desc = {asn1::table::enum_int_type<bin_range_value_c::types>(),
                                                                bin_range_value_c::types::nof_types,
                                                                bin_range_value_c::types::nof_exts,
                                                                bin_range_value_c::types::has_ext,
                                                                true,
                                                                choice_table_bin_range_value_c_alternatives,
                                                                nullptr,
                                                                choice_table_bin_range_value_c_names,
                                                                "bin_range_value_c",
                                                                offsetof(bin_range_value_c, c.type),
                                                                offsetof(bin_range_value_c, c.value),
                                                                false,
                                                                &bin_range_value_c::storage_desc};

static const char*           enum_names_bin_range_value_c_types_values[] = {"valueInt", "valueReal"};
const asn1::table::enum_desc bin_range_value_c::types_opts::desc         = {enum_names_bin_range_value_c_types_values,
                                                                            2,
                                                                            "bin_range_value_c::types"};

// BinRangeItem ::= SEQUENCE
static const asn1::table::seq_field  seq_table_bin_range_item_s_fields[] = {{offsetof(bin_range_item_s, bin_idx),
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::int_u32,
                                                                             true,
                                                                             true,
                                                                             int64_t{1},
                                                                             uint64_t{65534},
                                                                             nullptr},
                                                                            {offsetof(bin_range_item_s, start_value),
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::choice,
                                                                             false,
                                                                             false,
                                                                             0,
                                                                             0,
                                                                             nullptr,
                                                                             &bin_range_value_c::desc},
                                                                            {offsetof(bin_range_item_s, end_value),
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::choice,
                                                                             false,
                                                                             false,
                                                                             0,
                                                                             0,
                                                                             nullptr,
                                                                             &bin_range_value_c::desc}};
static const asn1::table::field_name seq_table_bin_range_item_s_names[]  = {{"binIndex", nullptr},
                                                                            {"startValue", nullptr},
                                                                            {"endValue", nullptr}};
const asn1::table::seq_desc          bin_range_item_s::desc              = {
    {seq_table_bin_range_item_s_fields, uint64_t{0x7}, nullptr, seq_table_bin_range_item_s_names, 3, 0},
    true,
    true,
    nullptr,
    0};

// BinRangeDefinition ::= SEQUENCE
static const asn1::table::seq_field seq_table_bin_range_definition_s_fields[] = {
    {offsetof(bin_range_definition_s, bin_range_list_x),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(bin_range_definition_s::bin_range_list_x),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &bin_range_item_s::desc},
    {offsetof(bin_range_definition_s, bin_range_list_y),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(bin_range_definition_s::bin_range_list_y),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &bin_range_item_s::desc},
    {offsetof(bin_range_definition_s, bin_range_list_z),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(bin_range_definition_s::bin_range_list_z),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &bin_range_item_s::desc}};
static const asn1::table::field_name seq_table_bin_range_definition_s_names[] = {{"binRangeListX", nullptr},
                                                                                 {"binRangeListY", nullptr},
                                                                                 {"binRangeListZ", nullptr}};
static const asn1::table::seq_opt    seq_table_bin_range_definition_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc bin_range_definition_s::desc = {{seq_table_bin_range_definition_s_fields,
                                                             uint64_t{0x1},
                                                             seq_table_bin_range_definition_s_opts,
                                                             seq_table_bin_range_definition_s_names,
                                                             3,
                                                             2},
                                                            true,
                                                            true,
                                                            nullptr,
                                                            0};

// MeasurementType ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_meas_type_c_alternatives[] = {
    &asn1::table::alternative_ops<printable_string<1, 150, true, true>, false, false>::value,
    &asn1::table::alternative_ops<uint32_t, true>::value};
const asn1::table::choice_storage_desc meas_type_c::storage_desc = {choice_storage_meas_type_c_alternatives,
                                                                    2,
                                                                    "meas_type_c"};

printable_string<1, 150, true, true>& meas_type_c::set_meas_name()
{
  set(types::meas_name);
  return c.get<printable_string<1, 150, true, true>>();
}
uint32_t& meas_type_c::set_meas_id()
{
  set(types::meas_id);
  return c.get<uint32_t>();
}
static const asn1::table::seq_field choice_table_meas_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<printable_string<1, 150, true, true>, asn1::table::string_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65535},
     nullptr}};
static const asn1::table::field_name choice_table_meas_type_c_names[] = {{"measName", nullptr}, {"measID", nullptr}};
const asn1::table::choice_desc       meas_type_c::desc = {asn1::table::enum_int_type<meas_type_c::types>(),
                                                          meas_type_c::types::nof_types,
                                                          meas_type_c::types::nof_exts,
                                                          meas_type_c::types::has_ext,
                                                          true,
                                                          choice_table_meas_type_c_alternatives,
                                                          nullptr,
                                                          choice_table_meas_type_c_names,
                                                          "meas_type_c",
                                                          offsetof(meas_type_c, c.type),
                                                          offsetof(meas_type_c, c.value),
                                                          false,
                                                          &meas_type_c::storage_desc};

static const char*           enum_names_meas_type_c_types_values[] = {"measName", "measID"};
const asn1::table::enum_desc meas_type_c::types_opts::desc         = {enum_names_meas_type_c_types_values,
                                                                      2,
                                                                      "meas_type_c::types"};

// DistMeasurementBinRangeItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_dist_meas_bin_range_item_s_fields[] = {
    {offsetof(dist_meas_bin_range_item_s, meas_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_type_c::desc},
    {offsetof(dist_meas_bin_range_item_s, bin_range_def),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bin_range_definition_s::desc}};
static const asn1::table::field_name seq_table_dist_meas_bin_range_item_s_names[] = {{"measType", nullptr},
                                                                                     {"binRangeDef", nullptr}};
const asn1::table::seq_desc          dist_meas_bin_range_item_s::desc = {{seq_table_dist_meas_bin_range_item_s_fields,
                                                                          uint64_t{0x3},
                                                                          nullptr,
                                                                          seq_table_dist_meas_bin_range_item_s_names,
                                                                          2,
                                                                          0},
                                                                         true,
                                                                         true,
                                                                         nullptr,
                                                                         0};

// MeasurementLabel ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_label_s_fields[] = {
    {offsetof(meas_label_s, no_label),
     (int32_t)offsetof(meas_label_s, no_label_present),
     asn1::table::enum_op<decltype(meas_label_s::no_label)>(),
     decltype(meas_label_s::no_label)::has_ext,
     false,
     decltype(meas_label_s::no_label)::nof_types,
     decltype(meas_label_s::no_label)::nof_exts,
     nullptr,
     &meas_label_s::no_label_opts::desc},
    {offsetof(meas_label_s, plmn_id),
     (int32_t)offsetof(meas_label_s, plmn_id_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(meas_label_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(meas_label_s, slice_id),
     (int32_t)offsetof(meas_label_s, slice_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &s_nssai_s::desc},
    {offsetof(meas_label_s, five_qi),
     (int32_t)offsetof(meas_label_s, five_qi_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(meas_label_s, qfi),
     (int32_t)offsetof(meas_label_s, qfi_present),
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{0},
     uint64_t{63},
     nullptr},
    {offsetof(meas_label_s, qci),
     (int32_t)offsetof(meas_label_s, qci_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(meas_label_s, qc_imax),
     (int32_t)offsetof(meas_label_s, qc_imax_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(meas_label_s, qc_imin),
     (int32_t)offsetof(meas_label_s, qc_imin_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(meas_label_s, ar_pmax),
     (int32_t)offsetof(meas_label_s, ar_pmax_present),
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{1},
     uint64_t{14},
     nullptr},
    {offsetof(meas_label_s, ar_pmin),
     (int32_t)offsetof(meas_label_s, ar_pmin_present),
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{1},
     uint64_t{14},
     nullptr},
    {offsetof(meas_label_s, bitrate_range),
     (int32_t)offsetof(meas_label_s, bitrate_range_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(meas_label_s, layer_mu_mimo),
     (int32_t)offsetof(meas_label_s, layer_mu_mimo_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(meas_label_s, sum),
     (int32_t)offsetof(meas_label_s, sum_present),
     asn1::table::enum_op<decltype(meas_label_s::sum)>(),
     decltype(meas_label_s::sum)::has_ext,
     false,
     decltype(meas_label_s::sum)::nof_types,
     decltype(meas_label_s::sum)::nof_exts,
     nullptr,
     &meas_label_s::sum_opts::desc},
    {offsetof(meas_label_s, dist_bin_x),
     (int32_t)offsetof(meas_label_s, dist_bin_x_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(meas_label_s, dist_bin_y),
     (int32_t)offsetof(meas_label_s, dist_bin_y_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(meas_label_s, dist_bin_z),
     (int32_t)offsetof(meas_label_s, dist_bin_z_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(meas_label_s, pre_label_override),
     (int32_t)offsetof(meas_label_s, pre_label_override_present),
     asn1::table::enum_op<decltype(meas_label_s::pre_label_override)>(),
     decltype(meas_label_s::pre_label_override)::has_ext,
     false,
     decltype(meas_label_s::pre_label_override)::nof_types,
     decltype(meas_label_s::pre_label_override)::nof_exts,
     nullptr,
     &meas_label_s::pre_label_override_opts::desc},
    {offsetof(meas_label_s, start_end_ind),
     (int32_t)offsetof(meas_label_s, start_end_ind_present),
     asn1::table::enum_op<decltype(meas_label_s::start_end_ind)>(),
     decltype(meas_label_s::start_end_ind)::has_ext,
     false,
     decltype(meas_label_s::start_end_ind)::nof_types,
     decltype(meas_label_s::start_end_ind)::nof_exts,
     nullptr,
     &meas_label_s::start_end_ind_opts::desc},
    {offsetof(meas_label_s, min),
     (int32_t)offsetof(meas_label_s, min_present),
     asn1::table::enum_op<decltype(meas_label_s::min)>(),
     decltype(meas_label_s::min)::has_ext,
     false,
     decltype(meas_label_s::min)::nof_types,
     decltype(meas_label_s::min)::nof_exts,
     nullptr,
     &meas_label_s::min_opts::desc},
    {offsetof(meas_label_s, max),
     (int32_t)offsetof(meas_label_s, max_present),
     asn1::table::enum_op<decltype(meas_label_s::max)>(),
     decltype(meas_label_s::max)::has_ext,
     false,
     decltype(meas_label_s::max)::nof_types,
     decltype(meas_label_s::max)::nof_exts,
     nullptr,
     &meas_label_s::max_opts::desc},
    {offsetof(meas_label_s, avg),
     (int32_t)offsetof(meas_label_s, avg_present),
     asn1::table::enum_op<decltype(meas_label_s::avg)>(),
     decltype(meas_label_s::avg)::has_ext,
     false,
     decltype(meas_label_s::avg)::nof_types,
     decltype(meas_label_s::avg)::nof_exts,
     nullptr,
     &meas_label_s::avg_opts::desc}};
static const asn1::table::field_name seq_table_meas_label_s_names[] = {{"noLabel", nullptr},
                                                                       {"plmnID", nullptr},
                                                                       {"sliceID", nullptr},
                                                                       {"fiveQI", nullptr},
                                                                       {"qFI", nullptr},
                                                                       {"qCI", nullptr},
                                                                       {"qCImax", nullptr},
                                                                       {"qCImin", nullptr},
                                                                       {"aRPmax", nullptr},
                                                                       {"aRPmin", nullptr},
                                                                       {"bitrateRange", nullptr},
                                                                       {"layerMU-MIMO", nullptr},
                                                                       {"sUM", nullptr},
                                                                       {"distBinX", nullptr},
                                                                       {"distBinY", nullptr},
                                                                       {"distBinZ", nullptr},
                                                                       {"preLabelOverride", nullptr},
                                                                       {"startEndInd", nullptr},
                                                                       {"min", nullptr},
                                                                       {"max", nullptr},
                                                                       {"avg", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_label_s_opts[]  = {
    {0, (int32_t)offsetof(meas_label_s, no_label_present)},
    {1, (int32_t)offsetof(meas_label_s, plmn_id_present)},
    {2, (int32_t)offsetof(meas_label_s, slice_id_present)},
    {3, (int32_t)offsetof(meas_label_s, five_qi_present)},
    {4, (int32_t)offsetof(meas_label_s, qfi_present)},
    {5, (int32_t)offsetof(meas_label_s, qci_present)},
    {6, (int32_t)offsetof(meas_label_s, qc_imax_present)},
    {7, (int32_t)offsetof(meas_label_s, qc_imin_present)},
    {8, (int32_t)offsetof(meas_label_s, ar_pmax_present)},
    {9, (int32_t)offsetof(meas_label_s, ar_pmin_present)},
    {10, (int32_t)offsetof(meas_label_s, bitrate_range_present)},
    {11, (int32_t)offsetof(meas_label_s, layer_mu_mimo_present)},
    {12, (int32_t)offsetof(meas_label_s, sum_present)},
    {13, (int32_t)offsetof(meas_label_s, dist_bin_x_present)},
    {14, (int32_t)offsetof(meas_label_s, dist_bin_y_present)},
    {15, (int32_t)offsetof(meas_label_s, dist_bin_z_present)},
    {16, (int32_t)offsetof(meas_label_s, pre_label_override_present)},
    {17, (int32_t)offsetof(meas_label_s, start_end_ind_present)},
    {18, (int32_t)offsetof(meas_label_s, min_present)},
    {19, (int32_t)offsetof(meas_label_s, max_present)},
    {20, (int32_t)offsetof(meas_label_s, avg_present)}};
static const asn1::table::seq_field seq_table_meas_label_s_g0_fields[] = {
    {offsetof(meas_label_s, ssb_idx),
     (int32_t)offsetof(meas_label_s, ssb_idx_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr}};
static const asn1::table::field_name seq_table_meas_label_s_g0_names[] = {{"ssbIndex", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_label_s_g0_opts[]  = {
    {0, (int32_t)offsetof(meas_label_s, ssb_idx_present)}};
static const asn1::table::seq_field seq_table_meas_label_s_g1_fields[] = {
    {offsetof(meas_label_s, non_go_b_b_fmode_idx),
     (int32_t)offsetof(meas_label_s, non_go_b_b_fmode_idx_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr}};
static const asn1::table::field_name seq_table_meas_label_s_g1_names[] = {{"nonGoB-BFmode-Index", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_label_s_g1_opts[]  = {
    {0, (int32_t)offsetof(meas_label_s, non_go_b_b_fmode_idx_present)}};
static const asn1::table::seq_field seq_table_meas_label_s_g2_fields[] = {
    {offsetof(meas_label_s, mimo_mode_idx),
     (int32_t)offsetof(meas_label_s, mimo_mode_idx_present),
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{1},
     uint64_t{1},
     nullptr}};
static const asn1::table::field_name seq_table_meas_label_s_g2_names[] = {{"mIMO-mode-Index", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_label_s_g2_opts[]  = {
    {0, (int32_t)offsetof(meas_label_s, mimo_mode_idx_present)}};
static const asn1::table::seq_field seq_table_meas_label_s_g3_fields[] = {
    {offsetof(meas_label_s, cell_global_id),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(meas_label_s::cell_global_id)>::value,
     &cgi_c::desc}};
static const asn1::table::field_name seq_table_meas_label_s_g3_names[]  = {{"cellGlobalID", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_label_s_g3_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_meas_label_s_g4_fields[] = {
    {offsetof(meas_label_s, beam_id),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::choice,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(meas_label_s::beam_id)>::value,
      &beam_id_c::desc}};
static const asn1::table::field_name seq_table_meas_label_s_g4_names[] = {{"beamID", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_label_s_g4_opts[]  = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_meas_label_s_groups[]   = {{{seq_table_meas_label_s_g0_fields,
                                                                            uint64_t{0x0},
                                                                            seq_table_meas_label_s_g0_opts,
                                                                            seq_table_meas_label_s_g0_names,
                                                                            1,
                                                                            1},
                                                                           true},
                                                                          {{seq_table_meas_label_s_g1_fields,
                                                                            uint64_t{0x0},
                                                                            seq_table_meas_label_s_g1_opts,
                                                                            seq_table_meas_label_s_g1_names,
                                                                            1,
                                                                            1},
                                                                           true},
                                                                          {{seq_table_meas_label_s_g2_fields,
                                                                            uint64_t{0x0},
                                                                            seq_table_meas_label_s_g2_opts,
                                                                            seq_table_meas_label_s_g2_names,
                                                                            1,
                                                                            1},
                                                                           true},
                                                                          {{seq_table_meas_label_s_g3_fields,
                                                                            uint64_t{0x0},
                                                                            seq_table_meas_label_s_g3_opts,
                                                                            seq_table_meas_label_s_g3_names,
                                                                            1,
                                                                            1},
                                                                           true},
                                                                          {{seq_table_meas_label_s_g4_fields,
                                                                            uint64_t{0x0},
                                                                            seq_table_meas_label_s_g4_opts,
                                                                            seq_table_meas_label_s_g4_names,
                                                                            1,
                                                                            1},
                                                                           true}};
const asn1::table::seq_desc          meas_label_s::desc                = {
    {seq_table_meas_label_s_fields, uint64_t{0x0}, seq_table_meas_label_s_opts, seq_table_meas_label_s_names, 21, 21},
    true,
    true,
    seq_table_meas_label_s_groups,
    5};

static const char*           enum_names_meas_label_s_no_label_e__values[] = {"true"};
const asn1::table::enum_desc meas_label_s::no_label_opts::desc            = {enum_names_meas_label_s_no_label_e__values,
                                                                             1,
                                                                             "meas_label_s::no_label_e_"};

static const char*           enum_names_meas_label_s_sum_e__values[] = {"true"};
const asn1::table::enum_desc meas_label_s::sum_opts::desc            = {enum_names_meas_label_s_sum_e__values,
                                                                        1,
                                                                        "meas_label_s::sum_e_"};

static const char*           enum_names_meas_label_s_pre_label_override_e__values[] = {"true"};
const asn1::table::enum_desc meas_label_s::pre_label_override_opts::desc            = {
    enum_names_meas_label_s_pre_label_override_e__values,
    1,
    "meas_label_s::pre_label_override_e_"};

static const char*           enum_names_meas_label_s_start_end_ind_e__values[] = {"start", "end"};
const asn1::table::enum_desc meas_label_s::start_end_ind_opts::desc = {enum_names_meas_label_s_start_end_ind_e__values,
                                                                       2,
                                                                       "meas_label_s::start_end_ind_e_"};

static const char*           enum_names_meas_label_s_min_e__values[] = {"true"};
const asn1::table::enum_desc meas_label_s::min_opts::desc            = {enum_names_meas_label_s_min_e__values,
                                                                        1,
                                                                        "meas_label_s::min_e_"};

static const char*           enum_names_meas_label_s_max_e__values[] = {"true"};
const asn1::table::enum_desc meas_label_s::max_opts::desc            = {enum_names_meas_label_s_max_e__values,
                                                                        1,
                                                                        "meas_label_s::max_e_"};

static const char*           enum_names_meas_label_s_avg_e__values[] = {"true"};
const asn1::table::enum_desc meas_label_s::avg_opts::desc            = {enum_names_meas_label_s_avg_e__values,
                                                                        1,
                                                                        "meas_label_s::avg_e_"};

// LabelInfoItem ::= SEQUENCE
static const asn1::table::seq_field  seq_table_label_info_item_s_fields[] = {{offsetof(label_info_item_s, meas_label),
                                                                              asn1::table::seq_field::mandatory,
                                                                              asn1::table::field_op::sequence,
                                                                              false,
                                                                              false,
                                                                              0,
                                                                              0,
                                                                              nullptr,
                                                                              &meas_label_s::desc}};
static const asn1::table::field_name seq_table_label_info_item_s_names[]  = {{"measLabel", nullptr}};
const asn1::table::seq_desc          label_info_item_s::desc              = {
    {seq_table_label_info_item_s_fields, uint64_t{0x1}, nullptr, seq_table_label_info_item_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// MeasValueTestCond-Expression ::= ENUMERATED
static const char*           enum_names_meas_value_test_cond_expression_e_values[] = {"equal",
                                                                                      "greaterthan",
                                                                                      "lessthan",
                                                                                      "contains",
                                                                                      "present"};
const asn1::table::enum_desc meas_value_test_cond_expression_opts::desc            = {
    enum_names_meas_value_test_cond_expression_e_values,
    5,
    "meas_value_test_cond_expression_e"};

// TestCond-Value ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_test_cond_value_c_alternatives[] = {
    &asn1::table::alternative_ops<int64_t, true>::value,
    &asn1::table::alternative_ops<int64_t, true>::value,
    &asn1::table::alternative_ops<bool, false>::value,
    &asn1::table::alternative_ops<unbounded_bitstring<false, true>, false>::value,
    &asn1::table::alternative_ops<unbounded_octstring<true>, false>::value,
    &asn1::table::alternative_ops<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>, false, false>::value,
    &asn1::table::alternative_ops<real_s, false>::value};
const asn1::table::choice_storage_desc test_cond_value_c::storage_desc = {choice_storage_test_cond_value_c_alternatives,
                                                                          7,
                                                                          "test_cond_value_c"};

int64_t& test_cond_value_c::set_value_int()
{
  set(types::value_int);
  return c.get<int64_t>();
}
int64_t& test_cond_value_c::set_value_enum()
{
  set(types::value_enum);
  return c.get<int64_t>();
}
bool& test_cond_value_c::set_value_bool()
{
  set(types::value_bool);
  return c.get<bool>();
}
unbounded_bitstring<false, true>& test_cond_value_c::set_value_bit_s()
{
  set(types::value_bit_s);
  return c.get<unbounded_bitstring<false, true>>();
}
unbounded_octstring<true>& test_cond_value_c::set_value_oct_s()
{
  set(types::value_oct_s);
  return c.get<unbounded_octstring<true>>();
}
printable_string<0, MAX_ASN_STRING_LENGTH, false, true>& test_cond_value_c::set_value_prt_s()
{
  set(types::value_prt_s);
  return c.get<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>();
}
real_s& test_cond_value_c::set_value_real()
{
  set(types::value_real);
  return c.get<real_s>();
}
static const asn1::table::seq_field choice_table_test_cond_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::boolean, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<unbounded_bitstring<false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<unbounded_octstring<true>, asn1::table::string_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>,
                                asn1::table::string_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<real_s, asn1::table::float_json>::value}};
static const asn1::table::field_name choice_table_test_cond_value_c_names[] = {{"valueInt", nullptr},
                                                                               {"valueEnum", nullptr},
                                                                               {"valueBool", nullptr},
                                                                               {"valueBitS", nullptr},
                                                                               {"valueOctS", nullptr},
                                                                               {"valuePrtS", nullptr},
                                                                               {"valueReal", nullptr}};
const asn1::table::choice_desc       test_cond_value_c::desc = {asn1::table::enum_int_type<test_cond_value_c::types>(),
                                                                test_cond_value_c::types::nof_types,
                                                                test_cond_value_c::types::nof_exts,
                                                                test_cond_value_c::types::has_ext,
                                                                true,
                                                                choice_table_test_cond_value_c_alternatives,
                                                                nullptr,
                                                                choice_table_test_cond_value_c_names,
                                                                "test_cond_value_c",
                                                                offsetof(test_cond_value_c, c.type),
                                                                offsetof(test_cond_value_c, c.value),
                                                                false,
                                                                &test_cond_value_c::storage_desc};

static const char* enum_names_test_cond_value_c_types_values[] =
    {"valueInt", "valueEnum", "valueBool", "valueBitS", "valueOctS", "valuePrtS", "valueReal"};
const asn1::table::enum_desc test_cond_value_c::types_opts::desc = {enum_names_test_cond_value_c_types_values,
                                                                    7,
                                                                    "test_cond_value_c::types"};

// MeasValueReportCond ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_value_report_cond_s_fields[] = {
    {offsetof(meas_value_report_cond_s, test_expr),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(meas_value_report_cond_s::test_expr)>(),
     decltype(meas_value_report_cond_s::test_expr)::has_ext,
     false,
     decltype(meas_value_report_cond_s::test_expr)::nof_types,
     decltype(meas_value_report_cond_s::test_expr)::nof_exts,
     nullptr,
     &meas_value_test_cond_expression_opts::desc},
    {offsetof(meas_value_report_cond_s, test_value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &test_cond_value_c::desc}};
static const asn1::table::field_name seq_table_meas_value_report_cond_s_names[] = {{"testExpr", nullptr},
                                                                                   {"testValue", nullptr}};
const asn1::table::seq_desc          meas_value_report_cond_s::desc             = {
    {seq_table_meas_value_report_cond_s_fields, uint64_t{0x3}, nullptr, seq_table_meas_value_report_cond_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// LogicalOR ::= ENUMERATED
static const char*           enum_names_lc_or_e_values[] = {"true"};
const asn1::table::enum_desc lc_or_opts::desc            = {enum_names_lc_or_e_values, 1, "lc_or_e"};

// MatchCondReportItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_match_cond_report_item_s_fields[] = {
    {offsetof(match_cond_report_item_s, meas_value_report_cond),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_value_report_cond_s::desc},
    {offsetof(match_cond_report_item_s, lc_or),
     (int32_t)offsetof(match_cond_report_item_s, lc_or_present),
     asn1::table::enum_op<decltype(match_cond_report_item_s::lc_or)>(),
     decltype(match_cond_report_item_s::lc_or)::has_ext,
     false,
     decltype(match_cond_report_item_s::lc_or)::nof_types,
     decltype(match_cond_report_item_s::lc_or)::nof_exts,
     nullptr,
     &lc_or_opts::desc}};
static const asn1::table::field_name seq_table_match_cond_report_item_s_names[] = {{"measValueReportCond", nullptr},
                                                                                   {"logicalOR", nullptr}};
static const asn1::table::seq_opt    seq_table_match_cond_report_item_s_opts[]  = {
    {1, (int32_t)offsetof(match_cond_report_item_s, lc_or_present)}};
const asn1::table::seq_desc match_cond_report_item_s::desc = {{seq_table_match_cond_report_item_s_fields,
                                                               uint64_t{0x1},
                                                               seq_table_match_cond_report_item_s_opts,
                                                               seq_table_match_cond_report_item_s_names,
                                                               2,
                                                               1},
                                                              true,
                                                              true,
                                                              nullptr,
                                                              0};

// MeasurementInfoItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_info_item_s_fields[] = {
    {offsetof(meas_info_item_s, meas_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_type_c::desc},
    {offsetof(meas_info_item_s, label_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2147483646,
     &asn1::table::item_list_codec<decltype(meas_info_item_s::label_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &label_info_item_s::desc}};
static const asn1::table::field_name seq_table_meas_info_item_s_names[]     = {{"measType", nullptr},
                                                                               {"labelInfoList", nullptr}};
static const asn1::table::seq_field  seq_table_meas_info_item_s_g0_fields[] = {
    {offsetof(meas_info_item_s, match_cond_report_list),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      true,
      1,
      32767,
      &asn1::table::ptr_item_list_codec<decltype(meas_info_item_s::match_cond_report_list),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &match_cond_report_item_s::desc}};
static const asn1::table::field_name seq_table_meas_info_item_s_g0_names[] = {{"matchCondReportList", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_info_item_s_g0_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_meas_info_item_s_groups[]  = {{{seq_table_meas_info_item_s_g0_fields,
                                                                               uint64_t{0x0},
                                                                               seq_table_meas_info_item_s_g0_opts,
                                                                               seq_table_meas_info_item_s_g0_names,
                                                                               1,
                                                                               1},
                                                                              true}};
const asn1::table::seq_desc          meas_info_item_s::desc               = {
    {seq_table_meas_info_item_s_fields, uint64_t{0x3}, nullptr, seq_table_meas_info_item_s_names, 2, 0},
    true,
    true,
    seq_table_meas_info_item_s_groups,
    1};

// E2SM-KPM-ActionDefinition-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_action_definition_format1_s_fields[] = {
    {offsetof(e2sm_kpm_action_definition_format1_s, meas_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_kpm_action_definition_format1_s::meas_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &meas_info_item_s::desc},
    {offsetof(e2sm_kpm_action_definition_format1_s, granul_period),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(e2sm_kpm_action_definition_format1_s, cell_global_id),
     (int32_t)offsetof(e2sm_kpm_action_definition_format1_s, cell_global_id_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_c::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_action_definition_format1_s_names[] = {
    {"measInfoList", nullptr},
    {"granulPeriod", nullptr},
    {"cellGlobalID", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_kpm_action_definition_format1_s_opts[] = {
    {2, (int32_t)offsetof(e2sm_kpm_action_definition_format1_s, cell_global_id_present)}};
static const asn1::table::seq_field seq_table_e2sm_kpm_action_definition_format1_s_g0_fields[] = {
    {offsetof(e2sm_kpm_action_definition_format1_s, dist_meas_bin_range_info),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::ptr_item_list_codec<decltype(e2sm_kpm_action_definition_format1_s::dist_meas_bin_range_info),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &dist_meas_bin_range_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_action_definition_format1_s_g0_names[] = {
    {"distMeasBinRangeInfo", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_kpm_action_definition_format1_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_e2sm_kpm_action_definition_format1_s_groups[] = {
    {{seq_table_e2sm_kpm_action_definition_format1_s_g0_fields,
      uint64_t{0x0},
      seq_table_e2sm_kpm_action_definition_format1_s_g0_opts,
      seq_table_e2sm_kpm_action_definition_format1_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc e2sm_kpm_action_definition_format1_s::desc = {
    {seq_table_e2sm_kpm_action_definition_format1_s_fields,
     uint64_t{0x3},
     seq_table_e2sm_kpm_action_definition_format1_s_opts,
     seq_table_e2sm_kpm_action_definition_format1_s_names,
     3,
     1},
    true,
    true,
    seq_table_e2sm_kpm_action_definition_format1_s_groups,
    1};

// E2SM-KPM-ActionDefinition-Format2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_action_definition_format2_s_fields[] = {
    {offsetof(e2sm_kpm_action_definition_format2_s, ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_id_c::desc},
    {offsetof(e2sm_kpm_action_definition_format2_s, subscript_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_action_definition_format1_s::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_action_definition_format2_s_names[] = {
    {"ueID", nullptr},
    {"subscriptInfo", nullptr}};
const asn1::table::seq_desc e2sm_kpm_action_definition_format2_s::desc = {
    {seq_table_e2sm_kpm_action_definition_format2_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_kpm_action_definition_format2_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// TestCond-Type ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_test_cond_type_c_alternatives[] = {
    &asn1::table::alternative_ops<test_cond_type_c::gbr_e_, true>::value,
    &asn1::table::alternative_ops<test_cond_type_c::ambr_e_, true>::value,
    &asn1::table::alternative_ops<test_cond_type_c::is_stat_e_, true>::value,
    &asn1::table::alternative_ops<test_cond_type_c::is_cat_m_e_, true>::value,
    &asn1::table::alternative_ops<test_cond_type_c::rsrp_e_, true>::value,
    &asn1::table::alternative_ops<test_cond_type_c::rsrq_e_, true>::value,
    &asn1::table::alternative_ops<test_cond_type_c::ul_r_srp_e_, true>::value,
    &asn1::table::alternative_ops<test_cond_type_c::cqi_e_, true>::value,
    &asn1::table::alternative_ops<test_cond_type_c::five_qi_e_, true>::value,
    &asn1::table::alternative_ops<test_cond_type_c::qci_e_, true>::value,
    &asn1::table::alternative_ops<test_cond_type_c::snssai_e_, true>::value};
const asn1::table::choice_storage_desc test_cond_type_c::storage_desc = {choice_storage_test_cond_type_c_alternatives,
                                                                         11,
                                                                         "test_cond_type_c"};

test_cond_type_c::gbr_e_& test_cond_type_c::set_gbr()
{
  set(types::gbr);
  return c.get<gbr_e_>();
}
test_cond_type_c::ambr_e_& test_cond_type_c::set_ambr()
{
  set(types::ambr);
  return c.get<ambr_e_>();
}
test_cond_type_c::is_stat_e_& test_cond_type_c::set_is_stat()
{
  set(types::is_stat);
  return c.get<is_stat_e_>();
}
test_cond_type_c::is_cat_m_e_& test_cond_type_c::set_is_cat_m()
{
  set(types::is_cat_m);
  return c.get<is_cat_m_e_>();
}
test_cond_type_c::rsrp_e_& test_cond_type_c::set_rsrp()
{
  set(types::rsrp);
  return c.get<rsrp_e_>();
}
test_cond_type_c::rsrq_e_& test_cond_type_c::set_rsrq()
{
  set(types::rsrq);
  return c.get<rsrq_e_>();
}
test_cond_type_c::ul_r_srp_e_& test_cond_type_c::set_ul_r_srp()
{
  set(types::ul_r_srp);
  return c.get<ul_r_srp_e_>();
}
test_cond_type_c::cqi_e_& test_cond_type_c::set_cqi()
{
  set(types::cqi);
  return c.get<cqi_e_>();
}
test_cond_type_c::five_qi_e_& test_cond_type_c::set_five_qi()
{
  set(types::five_qi);
  return c.get<five_qi_e_>();
}
test_cond_type_c::qci_e_& test_cond_type_c::set_qci()
{
  set(types::qci);
  return c.get<qci_e_>();
}
test_cond_type_c::snssai_e_& test_cond_type_c::set_snssai()
{
  set(types::snssai);
  return c.get<snssai_e_>();
}
static const asn1::table::seq_field choice_table_test_cond_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<test_cond_type_c::gbr_e_>(),
     test_cond_type_c::gbr_e_::has_ext,
     false,
     test_cond_type_c::gbr_e_::nof_types,
     test_cond_type_c::gbr_e_::nof_exts,
     nullptr,
     &test_cond_type_c::gbr_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<test_cond_type_c::ambr_e_>(),
     test_cond_type_c::ambr_e_::has_ext,
     false,
     test_cond_type_c::ambr_e_::nof_types,
     test_cond_type_c::ambr_e_::nof_exts,
     nullptr,
     &test_cond_type_c::ambr_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<test_cond_type_c::is_stat_e_>(),
     test_cond_type_c::is_stat_e_::has_ext,
     false,
     test_cond_type_c::is_stat_e_::nof_types,
     test_cond_type_c::is_stat_e_::nof_exts,
     nullptr,
     &test_cond_type_c::is_stat_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<test_cond_type_c::is_cat_m_e_>(),
     test_cond_type_c::is_cat_m_e_::has_ext,
     false,
     test_cond_type_c::is_cat_m_e_::nof_types,
     test_cond_type_c::is_cat_m_e_::nof_exts,
     nullptr,
     &test_cond_type_c::is_cat_m_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<test_cond_type_c::rsrp_e_>(),
     test_cond_type_c::rsrp_e_::has_ext,
     false,
     test_cond_type_c::rsrp_e_::nof_types,
     test_cond_type_c::rsrp_e_::nof_exts,
     nullptr,
     &test_cond_type_c::rsrp_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<test_cond_type_c::rsrq_e_>(),
     test_cond_type_c::rsrq_e_::has_ext,
     false,
     test_cond_type_c::rsrq_e_::nof_types,
     test_cond_type_c::rsrq_e_::nof_exts,
     nullptr,
     &test_cond_type_c::rsrq_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<test_cond_type_c::ul_r_srp_e_>(),
     test_cond_type_c::ul_r_srp_e_::has_ext,
     false,
     test_cond_type_c::ul_r_srp_e_::nof_types,
     test_cond_type_c::ul_r_srp_e_::nof_exts,
     nullptr,
     &test_cond_type_c::ul_r_srp_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<test_cond_type_c::cqi_e_>(),
     test_cond_type_c::cqi_e_::has_ext,
     false,
     test_cond_type_c::cqi_e_::nof_types,
     test_cond_type_c::cqi_e_::nof_exts,
     nullptr,
     &test_cond_type_c::cqi_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<test_cond_type_c::five_qi_e_>(),
     test_cond_type_c::five_qi_e_::has_ext,
     false,
     test_cond_type_c::five_qi_e_::nof_types,
     test_cond_type_c::five_qi_e_::nof_exts,
     nullptr,
     &test_cond_type_c::five_qi_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<test_cond_type_c::qci_e_>(),
     test_cond_type_c::qci_e_::has_ext,
     false,
     test_cond_type_c::qci_e_::nof_types,
     test_cond_type_c::qci_e_::nof_exts,
     nullptr,
     &test_cond_type_c::qci_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<test_cond_type_c::snssai_e_>(),
     test_cond_type_c::snssai_e_::has_ext,
     false,
     test_cond_type_c::snssai_e_::nof_types,
     test_cond_type_c::snssai_e_::nof_exts,
     nullptr,
     &test_cond_type_c::snssai_opts::desc}};
static const asn1::table::field_name choice_table_test_cond_type_c_names[] = {{"gBR", nullptr},
                                                                              {"aMBR", nullptr},
                                                                              {"isStat", nullptr},
                                                                              {"isCatM", nullptr},
                                                                              {"rSRP", nullptr},
                                                                              {"rSRQ", nullptr},
                                                                              {"ul-rSRP", nullptr},
                                                                              {"cQI", nullptr},
                                                                              {"fiveQI", nullptr},
                                                                              {"qCI", nullptr},
                                                                              {"sNSSAI", nullptr}};
const asn1::table::choice_desc       test_cond_type_c::desc = {asn1::table::enum_int_type<test_cond_type_c::types>(),
                                                               test_cond_type_c::types::nof_types,
                                                               test_cond_type_c::types::nof_exts,
                                                               test_cond_type_c::types::has_ext,
                                                               true,
                                                               choice_table_test_cond_type_c_alternatives,
                                                               nullptr,
                                                               choice_table_test_cond_type_c_names,
                                                               "test_cond_type_c",
                                                               offsetof(test_cond_type_c, c.type),
                                                               offsetof(test_cond_type_c, c.value),
                                                               false,
                                                               &test_cond_type_c::storage_desc};

static const char*           enum_names_test_cond_type_c_gbr_e__values[] = {"true"};
const asn1::table::enum_desc test_cond_type_c::gbr_opts::desc            = {enum_names_test_cond_type_c_gbr_e__values,
                                                                            1,
                                                                            "test_cond_type_c::gbr_e_"};

static const char*           enum_names_test_cond_type_c_ambr_e__values[] = {"true"};
const asn1::table::enum_desc test_cond_type_c::ambr_opts::desc            = {enum_names_test_cond_type_c_ambr_e__values,
                                                                             1,
                                                                             "test_cond_type_c::ambr_e_"};

static const char*           enum_names_test_cond_type_c_is_stat_e__values[] = {"true"};
const asn1::table::enum_desc test_cond_type_c::is_stat_opts::desc = {enum_names_test_cond_type_c_is_stat_e__values,
                                                                     1,
                                                                     "test_cond_type_c::is_stat_e_"};

static const char*           enum_names_test_cond_type_c_is_cat_m_e__values[] = {"true"};
const asn1::table::enum_desc test_cond_type_c::is_cat_m_opts::desc = {enum_names_test_cond_type_c_is_cat_m_e__values,
                                                                      1,
                                                                      "test_cond_type_c::is_cat_m_e_"};

static const char*           enum_names_test_cond_type_c_rsrp_e__values[] = {"true"};
const asn1::table::enum_desc test_cond_type_c::rsrp_opts::desc            = {enum_names_test_cond_type_c_rsrp_e__values,
                                                                             1,
                                                                             "test_cond_type_c::rsrp_e_"};

static const char*           enum_names_test_cond_type_c_rsrq_e__values[] = {"true"};
const asn1::table::enum_desc test_cond_type_c::rsrq_opts::desc            = {enum_names_test_cond_type_c_rsrq_e__values,
                                                                             1,
                                                                             "test_cond_type_c::rsrq_e_"};

static const char*           enum_names_test_cond_type_c_ul_r_srp_e__values[] = {"true"};
const asn1::table::enum_desc test_cond_type_c::ul_r_srp_opts::desc = {enum_names_test_cond_type_c_ul_r_srp_e__values,
                                                                      1,
                                                                      "test_cond_type_c::ul_r_srp_e_"};

static const char*           enum_names_test_cond_type_c_cqi_e__values[] = {"true"};
const asn1::table::enum_desc test_cond_type_c::cqi_opts::desc            = {enum_names_test_cond_type_c_cqi_e__values,
                                                                            1,
                                                                            "test_cond_type_c::cqi_e_"};

static const char*           enum_names_test_cond_type_c_five_qi_e__values[] = {"true"};
const asn1::table::enum_desc test_cond_type_c::five_qi_opts::desc = {enum_names_test_cond_type_c_five_qi_e__values,
                                                                     1,
                                                                     "test_cond_type_c::five_qi_e_"};

static const char*           enum_names_test_cond_type_c_qci_e__values[] = {"true"};
const asn1::table::enum_desc test_cond_type_c::qci_opts::desc            = {enum_names_test_cond_type_c_qci_e__values,
                                                                            1,
                                                                            "test_cond_type_c::qci_e_"};

static const char*           enum_names_test_cond_type_c_snssai_e__values[] = {"true"};
const asn1::table::enum_desc test_cond_type_c::snssai_opts::desc = {enum_names_test_cond_type_c_snssai_e__values,
                                                                    1,
                                                                    "test_cond_type_c::snssai_e_"};

static const char* enum_names_test_cond_type_c_types_values[] =
    {"gBR", "aMBR", "isStat", "isCatM", "rSRP", "rSRQ", "ul-rSRP", "cQI", "fiveQI", "qCI", "sNSSAI"};
const asn1::table::enum_desc test_cond_type_c::types_opts::desc = {enum_names_test_cond_type_c_types_values,
                                                                   11,
                                                                   "test_cond_type_c::types"};
uint8_t                      test_cond_type_c::types_opts::to_number() const
{
  if (value == five_qi) {
    return 5;
  }
  invalid_enum_number(value, "test_cond_type_c::types");
  return 0;
}

// TestCond-Expression ::= ENUMERATED
static const char*           enum_names_test_cond_expression_e_values[] = {"equal",
                                                                           "greaterthan",
                                                                           "lessthan",
                                                                           "contains",
                                                                           "present"};
const asn1::table::enum_desc test_cond_expression_opts::desc            = {enum_names_test_cond_expression_e_values,
                                                                           5,
                                                                           "test_cond_expression_e"};

// TestCondInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_test_cond_info_s_fields[] = {
    {offsetof(test_cond_info_s, test_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &test_cond_type_c::desc},
    {offsetof(test_cond_info_s, test_expr),
     (int32_t)offsetof(test_cond_info_s, test_expr_present),
     asn1::table::enum_op<decltype(test_cond_info_s::test_expr)>(),
     decltype(test_cond_info_s::test_expr)::has_ext,
     false,
     decltype(test_cond_info_s::test_expr)::nof_types,
     decltype(test_cond_info_s::test_expr)::nof_exts,
     nullptr,
     &test_cond_expression_opts::desc},
    {offsetof(test_cond_info_s, test_value),
     (int32_t)offsetof(test_cond_info_s, test_value_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &test_cond_value_c::desc}};
static const asn1::table::field_name seq_table_test_cond_info_s_names[] = {{"testType", nullptr},
                                                                           {"testExpr", nullptr},
                                                                           {"testValue", nullptr}};
static const asn1::table::seq_opt    seq_table_test_cond_info_s_opts[]  = {
    {1, (int32_t)offsetof(test_cond_info_s, test_expr_present)},
    {2, (int32_t)offsetof(test_cond_info_s, test_value_present)}};
const asn1::table::seq_desc test_cond_info_s::desc = {{seq_table_test_cond_info_s_fields,
                                                       uint64_t{0x1},
                                                       seq_table_test_cond_info_s_opts,
                                                       seq_table_test_cond_info_s_names,
                                                       3,
                                                       2},
                                                      true,
                                                      true,
                                                      nullptr,
                                                      0};

// MatchingCondItem-Choice ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_matching_cond_item_choice_c_alternatives[] = {
    &asn1::table::alternative_ops<meas_label_s, false>::value,
    &asn1::table::alternative_ops<test_cond_info_s, false, false>::value};
const asn1::table::choice_storage_desc matching_cond_item_choice_c::storage_desc = {
    choice_storage_matching_cond_item_choice_c_alternatives,
    2,
    "matching_cond_item_choice_c"};

meas_label_s& matching_cond_item_choice_c::set_meas_label()
{
  set(types::meas_label);
  return c.get<meas_label_s>();
}
test_cond_info_s& matching_cond_item_choice_c::set_test_cond_info()
{
  set(types::test_cond_info);
  return c.get<test_cond_info_s>();
}
static const asn1::table::seq_field choice_table_matching_cond_item_choice_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_label_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &test_cond_info_s::desc}};
static const asn1::table::field_name choice_table_matching_cond_item_choice_c_names[] = {{"measLabel", nullptr},
                                                                                         {"testCondInfo", nullptr}};
const asn1::table::choice_desc       matching_cond_item_choice_c::desc                = {
    asn1::table::enum_int_type<matching_cond_item_choice_c::types>(),
    matching_cond_item_choice_c::types::nof_types,
    matching_cond_item_choice_c::types::nof_exts,
    matching_cond_item_choice_c::types::has_ext,
    true,
    choice_table_matching_cond_item_choice_c_alternatives,
    nullptr,
    choice_table_matching_cond_item_choice_c_names,
    "matching_cond_item_choice_c",
    offsetof(matching_cond_item_choice_c, c.type),
    offsetof(matching_cond_item_choice_c, c.value),
    false,
    &matching_cond_item_choice_c::storage_desc};

static const char*           enum_names_matching_cond_item_choice_c_types_values[] = {"measLabel", "testCondInfo"};
const asn1::table::enum_desc matching_cond_item_choice_c::types_opts::desc         = {
    enum_names_matching_cond_item_choice_c_types_values,
    2,
    "matching_cond_item_choice_c::types"};

// MatchingCondItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_matching_cond_item_s_fields[] = {
    {offsetof(matching_cond_item_s, matching_cond_choice),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &matching_cond_item_choice_c::desc},
    {offsetof(matching_cond_item_s, lc_or),
     (int32_t)offsetof(matching_cond_item_s, lc_or_present),
     asn1::table::enum_op<decltype(matching_cond_item_s::lc_or)>(),
     decltype(matching_cond_item_s::lc_or)::has_ext,
     false,
     decltype(matching_cond_item_s::lc_or)::nof_types,
     decltype(matching_cond_item_s::lc_or)::nof_exts,
     nullptr,
     &lc_or_opts::desc}};
static const asn1::table::field_name seq_table_matching_cond_item_s_names[] = {{"matchingCondChoice", nullptr},
                                                                               {"logicalOR", nullptr}};
static const asn1::table::seq_opt    seq_table_matching_cond_item_s_opts[]  = {
    {1, (int32_t)offsetof(matching_cond_item_s, lc_or_present)}};
const asn1::table::seq_desc matching_cond_item_s::desc = {{seq_table_matching_cond_item_s_fields,
                                                           uint64_t{0x1},
                                                           seq_table_matching_cond_item_s_opts,
                                                           seq_table_matching_cond_item_s_names,
                                                           2,
                                                           1},
                                                          true,
                                                          true,
                                                          nullptr,
                                                          0};

// MeasurementCondItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_cond_item_s_fields[] = {
    {offsetof(meas_cond_item_s, meas_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_type_c::desc},
    {offsetof(meas_cond_item_s, matching_cond),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32767,
     &asn1::table::
         item_list_codec<decltype(meas_cond_item_s::matching_cond), asn1::table::sequence_item_ops, false, true>::value,
     &matching_cond_item_s::desc}};
static const asn1::table::field_name seq_table_meas_cond_item_s_names[]     = {{"measType", nullptr},
                                                                               {"matchingCond", nullptr}};
static const asn1::table::seq_field  seq_table_meas_cond_item_s_g0_fields[] = {
    {offsetof(meas_cond_item_s, bin_range_def),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(meas_cond_item_s::bin_range_def)>::value,
      &bin_range_definition_s::desc}};
static const asn1::table::field_name seq_table_meas_cond_item_s_g0_names[] = {{"binRangeDef", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_cond_item_s_g0_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_meas_cond_item_s_groups[]  = {{{seq_table_meas_cond_item_s_g0_fields,
                                                                               uint64_t{0x0},
                                                                               seq_table_meas_cond_item_s_g0_opts,
                                                                               seq_table_meas_cond_item_s_g0_names,
                                                                               1,
                                                                               1},
                                                                              true}};
const asn1::table::seq_desc          meas_cond_item_s::desc               = {
    {seq_table_meas_cond_item_s_fields, uint64_t{0x3}, nullptr, seq_table_meas_cond_item_s_names, 2, 0},
    true,
    true,
    seq_table_meas_cond_item_s_groups,
    1};

// E2SM-KPM-ActionDefinition-Format3 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_action_definition_format3_s_fields[] = {
    {offsetof(e2sm_kpm_action_definition_format3_s, meas_cond_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_kpm_action_definition_format3_s::meas_cond_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &meas_cond_item_s::desc},
    {offsetof(e2sm_kpm_action_definition_format3_s, granul_period),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(e2sm_kpm_action_definition_format3_s, cell_global_id),
     (int32_t)offsetof(e2sm_kpm_action_definition_format3_s, cell_global_id_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_c::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_action_definition_format3_s_names[] = {
    {"measCondList", nullptr},
    {"granulPeriod", nullptr},
    {"cellGlobalID", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_kpm_action_definition_format3_s_opts[] = {
    {2, (int32_t)offsetof(e2sm_kpm_action_definition_format3_s, cell_global_id_present)}};
const asn1::table::seq_desc e2sm_kpm_action_definition_format3_s::desc = {
    {seq_table_e2sm_kpm_action_definition_format3_s_fields,
     uint64_t{0x3},
     seq_table_e2sm_kpm_action_definition_format3_s_opts,
     seq_table_e2sm_kpm_action_definition_format3_s_names,
     3,
     1},
    true,
    true,
    nullptr,
    0};

// MatchingUeCondPerSubItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_matching_ue_cond_per_sub_item_s_fields[] = {
    {offsetof(matching_ue_cond_per_sub_item_s, test_cond_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &test_cond_info_s::desc}};
static const asn1::table::field_name seq_table_matching_ue_cond_per_sub_item_s_names[] = {{"testCondInfo", nullptr}};
static const asn1::table::seq_field  seq_table_matching_ue_cond_per_sub_item_s_g0_fields[] = {
    {offsetof(matching_ue_cond_per_sub_item_s, lc_or),
      (int32_t)offsetof(matching_ue_cond_per_sub_item_s, lc_or_present),
      asn1::table::enum_op<decltype(matching_ue_cond_per_sub_item_s::lc_or)>(),
      decltype(matching_ue_cond_per_sub_item_s::lc_or)::has_ext,
      false,
      decltype(matching_ue_cond_per_sub_item_s::lc_or)::nof_types,
      decltype(matching_ue_cond_per_sub_item_s::lc_or)::nof_exts,
      nullptr,
      &lc_or_opts::desc}};
static const asn1::table::field_name seq_table_matching_ue_cond_per_sub_item_s_g0_names[] = {{"logicalOR", nullptr}};
static const asn1::table::seq_opt    seq_table_matching_ue_cond_per_sub_item_s_g0_opts[]  = {
    {0, (int32_t)offsetof(matching_ue_cond_per_sub_item_s, lc_or_present)}};
static const asn1::table::seq_group seq_table_matching_ue_cond_per_sub_item_s_groups[] = {
    {{seq_table_matching_ue_cond_per_sub_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_matching_ue_cond_per_sub_item_s_g0_opts,
      seq_table_matching_ue_cond_per_sub_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc matching_ue_cond_per_sub_item_s::desc = {{seq_table_matching_ue_cond_per_sub_item_s_fields,
                                                                      uint64_t{0x1},
                                                                      nullptr,
                                                                      seq_table_matching_ue_cond_per_sub_item_s_names,
                                                                      1,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     seq_table_matching_ue_cond_per_sub_item_s_groups,
                                                                     1};

// E2SM-KPM-ActionDefinition-Format4 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_action_definition_format4_s_fields[] = {
    {offsetof(e2sm_kpm_action_definition_format4_s, matching_ue_cond_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32767,
     &asn1::table::item_list_codec<decltype(e2sm_kpm_action_definition_format4_s::matching_ue_cond_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &matching_ue_cond_per_sub_item_s::desc},
    {offsetof(e2sm_kpm_action_definition_format4_s, sub_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_action_definition_format1_s::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_action_definition_format4_s_names[] = {
    {"matchingUeCondList", nullptr},
    {"subscriptionInfo", nullptr}};
const asn1::table::seq_desc e2sm_kpm_action_definition_format4_s::desc = {
    {seq_table_e2sm_kpm_action_definition_format4_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_kpm_action_definition_format4_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// MatchingUEidPerSubItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_matching_ue_id_per_sub_item_s_fields[] = {
    {offsetof(matching_ue_id_per_sub_item_s, ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_id_c::desc}};
static const asn1::table::field_name seq_table_matching_ue_id_per_sub_item_s_names[] = {{"ueID", nullptr}};
const asn1::table::seq_desc matching_ue_id_per_sub_item_s::desc = {{seq_table_matching_ue_id_per_sub_item_s_fields,
                                                                    uint64_t{0x1},
                                                                    nullptr,
                                                                    seq_table_matching_ue_id_per_sub_item_s_names,
                                                                    1,
                                                                    0},
                                                                   true,
                                                                   true,
                                                                   nullptr,
                                                                   0};

// E2SM-KPM-ActionDefinition-Format5 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_action_definition_format5_s_fields[] = {
    {offsetof(e2sm_kpm_action_definition_format5_s, matching_ue_id_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     2,
     65533,
     &asn1::table::item_list_codec<decltype(e2sm_kpm_action_definition_format5_s::matching_ue_id_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &matching_ue_id_per_sub_item_s::desc},
    {offsetof(e2sm_kpm_action_definition_format5_s, sub_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_action_definition_format1_s::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_action_definition_format5_s_names[] = {
    {"matchingUEidList", nullptr},
    {"subscriptionInfo", nullptr}};
const asn1::table::seq_desc e2sm_kpm_action_definition_format5_s::desc = {
    {seq_table_e2sm_kpm_action_definition_format5_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_kpm_action_definition_format5_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-KPM-ActionDefinition-Fundamental ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_action_definition_fundamental_s_fields[] = {
    {offsetof(e2sm_kpm_action_definition_fundamental_s, action_definition_fundamental_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_action_definition_fundamental_s_names[] = {
    {"actionDefinitionFundamental-formats", nullptr}};
const asn1::table::seq_desc e2sm_kpm_action_definition_fundamental_s::desc = {
    {seq_table_e2sm_kpm_action_definition_fundamental_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_kpm_action_definition_fundamental_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

static const asn1::table::storage_ops* const
    choice_storage_e2sm_kpm_action_definition_fundamental_s_action_definition_fundamental_formats_c__alternatives[] = {
        &asn1::table::alternative_ops<e2sm_kpm_action_definition_format1_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_kpm_action_definition_format2_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_kpm_action_definition_format3_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_kpm_action_definition_format4_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_kpm_action_definition_format5_s, false, false>::value};
const asn1::table::choice_storage_desc
    e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::storage_desc = {
        choice_storage_e2sm_kpm_action_definition_fundamental_s_action_definition_fundamental_formats_c__alternatives,
        5,
        "e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_"};

e2sm_kpm_action_definition_format1_s&
e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::set_action_definition_format1()
{
  set(types::action_definition_format1);
  return c.get<e2sm_kpm_action_definition_format1_s>();
}
e2sm_kpm_action_definition_format2_s&
e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::set_action_definition_format2()
{
  set(types::action_definition_format2);
  return c.get<e2sm_kpm_action_definition_format2_s>();
}
e2sm_kpm_action_definition_format3_s&
e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::set_action_definition_format3()
{
  set(types::action_definition_format3);
  return c.get<e2sm_kpm_action_definition_format3_s>();
}
e2sm_kpm_action_definition_format4_s&
e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::set_action_definition_format4()
{
  set(types::action_definition_format4);
  return c.get<e2sm_kpm_action_definition_format4_s>();
}
e2sm_kpm_action_definition_format5_s&
e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::set_action_definition_format5()
{
  set(types::action_definition_format5);
  return c.get<e2sm_kpm_action_definition_format5_s>();
}
static const asn1::table::seq_field
    choice_table_e2sm_kpm_action_definition_fundamental_s_action_definition_fundamental_formats_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_action_definition_format1_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_action_definition_format2_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_action_definition_format3_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_action_definition_format4_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_action_definition_format5_s::desc}};
static const asn1::table::field_name
    choice_table_e2sm_kpm_action_definition_fundamental_s_action_definition_fundamental_formats_c__names[] = {
        {"actionDefinition-Format1", nullptr},
        {"actionDefinition-Format2", nullptr},
        {"actionDefinition-Format3", nullptr},
        {"actionDefinition-Format4", nullptr},
        {"actionDefinition-Format5", nullptr}};
const asn1::table::choice_desc
    e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::desc = {
        asn1::table::enum_int_type<
            e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::types>(),
        e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::types::nof_types,
        e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::types::nof_exts,
        e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::types::has_ext,
        true,
        choice_table_e2sm_kpm_action_definition_fundamental_s_action_definition_fundamental_formats_c__alternatives,
        nullptr,
        choice_table_e2sm_kpm_action_definition_fundamental_s_action_definition_fundamental_formats_c__names,
        "e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_",
        offsetof(e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_, c.type),
        offsetof(e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_, c.value),
        false,
        &e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::storage_desc};

static const char*
    enum_names_e2sm_kpm_action_definition_fundamental_s_action_definition_fundamental_formats_c__types_values[] = {
        "actionDefinition-Format1",
        "actionDefinition-Format2",
        "actionDefinition-Format3",
        "actionDefinition-Format4",
        "actionDefinition-Format5"};
const asn1::table::enum_desc
    e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::types_opts::desc = {
        enum_names_e2sm_kpm_action_definition_fundamental_s_action_definition_fundamental_formats_c__types_values,
        5,
        "e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::types"};
uint8_t
e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5};
  return map_enum_number(
      numbers, 5, value, "e2sm_kpm_action_definition_fundamental_s::action_definition_fundamental_formats_c_::types");
}

// MultiReportActionDefinitionItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_multi_report_action_definition_item_s_fields[] = {
    {offsetof(multi_report_action_definition_item_s, job_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(multi_report_action_definition_item_s, job_action_definition),
     (int32_t)offsetof(multi_report_action_definition_item_s, job_action_definition_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_action_definition_fundamental_s::desc},
    {offsetof(multi_report_action_definition_item_s, job_cell_global_id),
     (int32_t)offsetof(multi_report_action_definition_item_s, job_cell_global_id_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_c::desc},
    {offsetof(multi_report_action_definition_item_s, job_ue_id),
     (int32_t)offsetof(multi_report_action_definition_item_s, job_ue_id_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_id_c::desc},
    {offsetof(multi_report_action_definition_item_s, job_matching_ue_id_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     2,
     65533,
     &asn1::table::item_list_codec<decltype(multi_report_action_definition_item_s::job_matching_ue_id_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &matching_ue_id_per_sub_item_s::desc}};
static const asn1::table::field_name seq_table_multi_report_action_definition_item_s_names[] = {
    {"jobID", nullptr},
    {"jobActionDefinition", nullptr},
    {"jobCellGlobalID", nullptr},
    {"jobUEID", nullptr},
    {"jobMatchingUEidList", nullptr}};
static const asn1::table::seq_opt seq_table_multi_report_action_definition_item_s_opts[] = {
    {1, (int32_t)offsetof(multi_report_action_definition_item_s, job_action_definition_present)},
    {2, (int32_t)offsetof(multi_report_action_definition_item_s, job_cell_global_id_present)},
    {3, (int32_t)offsetof(multi_report_action_definition_item_s, job_ue_id_present)},
    {4, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc multi_report_action_definition_item_s::desc = {
    {seq_table_multi_report_action_definition_item_s_fields,
     uint64_t{0x1},
     seq_table_multi_report_action_definition_item_s_opts,
     seq_table_multi_report_action_definition_item_s_names,
     5,
     4},
    true,
    true,
    nullptr,
    0};

// MultiReportApproach ::= ENUMERATED
static const char*           enum_names_multi_report_approach_e_values[] = {"single", "multiple"};
const asn1::table::enum_desc multi_report_approach_opts::desc            = {enum_names_multi_report_approach_e_values,
                                                                            2,
                                                                            "multi_report_approach_e"};

// E2SM-KPM-ActionDefinition-Format255 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_action_definition_format255_s_fields[] = {
    {offsetof(e2sm_kpm_action_definition_format255_s, multi_report_action_definition_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<
         decltype(e2sm_kpm_action_definition_format255_s::multi_report_action_definition_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &multi_report_action_definition_item_s::desc},
    {offsetof(e2sm_kpm_action_definition_format255_s, multi_report_approach),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(e2sm_kpm_action_definition_format255_s::multi_report_approach)>(),
     decltype(e2sm_kpm_action_definition_format255_s::multi_report_approach)::has_ext,
     false,
     decltype(e2sm_kpm_action_definition_format255_s::multi_report_approach)::nof_types,
     decltype(e2sm_kpm_action_definition_format255_s::multi_report_approach)::nof_exts,
     nullptr,
     &multi_report_approach_opts::desc},
    {offsetof(e2sm_kpm_action_definition_format255_s, common_action_definition),
     (int32_t)offsetof(e2sm_kpm_action_definition_format255_s, common_action_definition_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_action_definition_fundamental_s::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_action_definition_format255_s_names[] = {
    {"multiReportActionDefinitionList", nullptr},
    {"multiReportApproach", nullptr},
    {"commonActionDefinition", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_kpm_action_definition_format255_s_opts[] = {
    {2, (int32_t)offsetof(e2sm_kpm_action_definition_format255_s, common_action_definition_present)}};
const asn1::table::seq_desc e2sm_kpm_action_definition_format255_s::desc = {
    {seq_table_e2sm_kpm_action_definition_format255_s_fields,
     uint64_t{0x3},
     seq_table_e2sm_kpm_action_definition_format255_s_opts,
     seq_table_e2sm_kpm_action_definition_format255_s_names,
     3,
     1},
    true,
    true,
    nullptr,
    0};

// E2SM-KPM-ActionDefinition ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_action_definition_s_fields[] = {
    {offsetof(e2sm_kpm_action_definition_s, ric_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(e2sm_kpm_action_definition_s, action_definition_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_action_definition_s::action_definition_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_action_definition_s_names[] = {
    {"ric-Style-Type", nullptr},
    {"actionDefinition-formats", nullptr}};
const asn1::table::seq_desc e2sm_kpm_action_definition_s::desc = {{seq_table_e2sm_kpm_action_definition_s_fields,
                                                                   uint64_t{0x3},
                                                                   nullptr,
                                                                   seq_table_e2sm_kpm_action_definition_s_names,
                                                                   2,
                                                                   0},
                                                                  true,
                                                                  true,
                                                                  nullptr,
                                                                  0};

static const asn1::table::storage_ops* const
    choice_storage_e2sm_kpm_action_definition_s_action_definition_formats_c__alternatives[] = {
        &asn1::table::alternative_ops<e2sm_kpm_action_definition_format1_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_kpm_action_definition_format2_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_kpm_action_definition_format3_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_kpm_action_definition_format4_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_kpm_action_definition_format5_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_kpm_action_definition_format255_s, false, false>::value};
const asn1::table::choice_storage_desc e2sm_kpm_action_definition_s::action_definition_formats_c_::storage_desc = {
    choice_storage_e2sm_kpm_action_definition_s_action_definition_formats_c__alternatives,
    6,
    "e2sm_kpm_action_definition_s::action_definition_formats_c_"};

e2sm_kpm_action_definition_format1_s&
e2sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format1()
{
  set(types::action_definition_format1);
  return c.get<e2sm_kpm_action_definition_format1_s>();
}
e2sm_kpm_action_definition_format2_s&
e2sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format2()
{
  set(types::action_definition_format2);
  return c.get<e2sm_kpm_action_definition_format2_s>();
}
e2sm_kpm_action_definition_format3_s&
e2sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format3()
{
  set(types::action_definition_format3);
  return c.get<e2sm_kpm_action_definition_format3_s>();
}
e2sm_kpm_action_definition_format4_s&
e2sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format4()
{
  set(types::action_definition_format4);
  return c.get<e2sm_kpm_action_definition_format4_s>();
}
e2sm_kpm_action_definition_format5_s&
e2sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format5()
{
  set(types::action_definition_format5);
  return c.get<e2sm_kpm_action_definition_format5_s>();
}
e2sm_kpm_action_definition_format255_s&
e2sm_kpm_action_definition_s::action_definition_formats_c_::set_action_definition_format255()
{
  set(types::action_definition_format255);
  return c.get<e2sm_kpm_action_definition_format255_s>();
}
static const asn1::table::seq_field
    choice_table_e2sm_kpm_action_definition_s_action_definition_formats_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_action_definition_format1_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_action_definition_format2_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_action_definition_format3_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_action_definition_format4_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_action_definition_format5_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_action_definition_format255_s::desc}};
static const asn1::table::field_name choice_table_e2sm_kpm_action_definition_s_action_definition_formats_c__names[] = {
    {"actionDefinition-Format1", nullptr},
    {"actionDefinition-Format2", nullptr},
    {"actionDefinition-Format3", nullptr},
    {"actionDefinition-Format4", nullptr},
    {"actionDefinition-Format5", nullptr},
    {"actionDefinition-Format255", nullptr}};
const asn1::table::choice_desc e2sm_kpm_action_definition_s::action_definition_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_kpm_action_definition_s::action_definition_formats_c_::types>(),
    e2sm_kpm_action_definition_s::action_definition_formats_c_::types::nof_types,
    e2sm_kpm_action_definition_s::action_definition_formats_c_::types::nof_exts,
    e2sm_kpm_action_definition_s::action_definition_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_kpm_action_definition_s_action_definition_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_kpm_action_definition_s_action_definition_formats_c__names,
    "e2sm_kpm_action_definition_s::action_definition_formats_c_",
    offsetof(e2sm_kpm_action_definition_s::action_definition_formats_c_, c.type),
    offsetof(e2sm_kpm_action_definition_s::action_definition_formats_c_, c.value),
    false,
    &e2sm_kpm_action_definition_s::action_definition_formats_c_::storage_desc};

static const char* enum_names_e2sm_kpm_action_definition_s_action_definition_formats_c__types_values[] = {
    "actionDefinition-Format1",
    "actionDefinition-Format2",
    "actionDefinition-Format3",
    "actionDefinition-Format4",
    "actionDefinition-Format5",
    "actionDefinition-Format255"};
const asn1::table::enum_desc e2sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::desc = {
    enum_names_e2sm_kpm_action_definition_s_action_definition_formats_c__types_values,
    6,
    "e2sm_kpm_action_definition_s::action_definition_formats_c_::types"};
uint16_t e2sm_kpm_action_definition_s::action_definition_formats_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 2, 3, 4, 5, 255};
  return map_enum_number(numbers, 6, value, "e2sm_kpm_action_definition_s::action_definition_formats_c_::types");
}

// E2SM-KPM-EventTriggerDefinition-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_event_trigger_definition_format1_s_fields[] = {
    {offsetof(e2sm_kpm_event_trigger_definition_format1_s, report_period),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_kpm_event_trigger_definition_format1_s_names[] = {
    {"reportingPeriod", nullptr}};
const asn1::table::seq_desc e2sm_kpm_event_trigger_definition_format1_s::desc = {
    {seq_table_e2sm_kpm_event_trigger_definition_format1_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_kpm_event_trigger_definition_format1_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// EventMeasurementDefinition ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_meas_definition_s_fields[] = {
    {offsetof(event_meas_definition_s, meas_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_type_c::desc},
    {offsetof(event_meas_definition_s, label_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2147483646,
     &asn1::table::item_list_codec<decltype(event_meas_definition_s::label_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &label_info_item_s::desc},
    {offsetof(event_meas_definition_s, cell_global_id),
     (int32_t)offsetof(event_meas_definition_s, cell_global_id_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_c::desc}};
static const asn1::table::field_name seq_table_event_meas_definition_s_names[] = {{"measType", nullptr},
                                                                                  {"labelInfoList", nullptr},
                                                                                  {"cellGlobalID", nullptr}};
static const asn1::table::seq_opt    seq_table_event_meas_definition_s_opts[]  = {
    {2, (int32_t)offsetof(event_meas_definition_s, cell_global_id_present)}};
const asn1::table::seq_desc event_meas_definition_s::desc = {{seq_table_event_meas_definition_s_fields,
                                                              uint64_t{0x3},
                                                              seq_table_event_meas_definition_s_opts,
                                                              seq_table_event_meas_definition_s_names,
                                                              3,
                                                              1},
                                                             true,
                                                             true,
                                                             nullptr,
                                                             0};

// EventWindowDefinition ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_win_definition_s_fields[] = {
    {offsetof(event_win_definition_s, win_size),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     int64_t{1},
     uint64_t{99},
     nullptr},
    {offsetof(event_win_definition_s, aggre_func),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(event_win_definition_s::aggre_func)>(),
     decltype(event_win_definition_s::aggre_func)::has_ext,
     false,
     decltype(event_win_definition_s::aggre_func)::nof_types,
     decltype(event_win_definition_s::aggre_func)::nof_exts,
     nullptr,
     &aggregation_function_type_opts::desc}};
static const asn1::table::field_name seq_table_event_win_definition_s_names[] = {{"windowSize", nullptr},
                                                                                 {"aggreFunc", nullptr}};
const asn1::table::seq_desc          event_win_definition_s::desc             = {
    {seq_table_event_win_definition_s_fields, uint64_t{0x3}, nullptr, seq_table_event_win_definition_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// EventParameterMeasurement ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_param_meas_s_fields[] = {
    {offsetof(event_param_meas_s, meas_def),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_meas_definition_s::desc},
    {offsetof(event_param_meas_s, win_func),
     (int32_t)offsetof(event_param_meas_s, win_func_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_win_definition_s::desc}};
static const asn1::table::field_name seq_table_event_param_meas_s_names[] = {{"measDef", nullptr},
                                                                             {"windowFunc", nullptr}};
static const asn1::table::seq_opt    seq_table_event_param_meas_s_opts[]  = {
    {1, (int32_t)offsetof(event_param_meas_s, win_func_present)}};
const asn1::table::seq_desc event_param_meas_s::desc = {{seq_table_event_param_meas_s_fields,
                                                         uint64_t{0x1},
                                                         seq_table_event_param_meas_s_opts,
                                                         seq_table_event_param_meas_s_names,
                                                         2,
                                                         1},
                                                        true,
                                                        true,
                                                        nullptr,
                                                        0};

// EventParameterValue ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_param_value_s_fields[] = {
    {offsetof(event_param_value_s, value),
     (int32_t)offsetof(event_param_value_s, value_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &test_cond_value_c::desc}};
static const asn1::table::field_name seq_table_event_param_value_s_names[] = {{"value", nullptr}};
static const asn1::table::seq_opt    seq_table_event_param_value_s_opts[]  = {
    {0, (int32_t)offsetof(event_param_value_s, value_present)}};
const asn1::table::seq_desc event_param_value_s::desc = {{seq_table_event_param_value_s_fields,
                                                          uint64_t{0x0},
                                                          seq_table_event_param_value_s_opts,
                                                          seq_table_event_param_value_s_names,
                                                          1,
                                                          1},
                                                         true,
                                                         true,
                                                         nullptr,
                                                         0};

// NextMathOperator ::= ENUMERATED
static const char*           enum_names_next_math_operator_e_values[] = {"plus", "minus", "product", "divide-by"};
const asn1::table::enum_desc next_math_operator_opts::desc            = {enum_names_next_math_operator_e_values,
                                                                         4,
                                                                         "next_math_operator_e"};

// EventParameterItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_param_item_s_fields[] = {
    {offsetof(event_param_item_s, param_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &event_param_item_s::param_type_c_::desc},
    {offsetof(event_param_item_s, next_math_op),
     (int32_t)offsetof(event_param_item_s, next_math_op_present),
     asn1::table::enum_op<decltype(event_param_item_s::next_math_op)>(),
     decltype(event_param_item_s::next_math_op)::has_ext,
     false,
     decltype(event_param_item_s::next_math_op)::nof_types,
     decltype(event_param_item_s::next_math_op)::nof_exts,
     nullptr,
     &next_math_operator_opts::desc}};
static const asn1::table::field_name seq_table_event_param_item_s_names[] = {{"paramType", nullptr},
                                                                             {"nextMathOp", nullptr}};
static const asn1::table::seq_opt    seq_table_event_param_item_s_opts[]  = {
    {1, (int32_t)offsetof(event_param_item_s, next_math_op_present)}};
const asn1::table::seq_desc event_param_item_s::desc = {{seq_table_event_param_item_s_fields,
                                                         uint64_t{0x1},
                                                         seq_table_event_param_item_s_opts,
                                                         seq_table_event_param_item_s_names,
                                                         2,
                                                         1},
                                                        true,
                                                        true,
                                                        nullptr,
                                                        0};

static const asn1::table::storage_ops* const choice_storage_event_param_item_s_param_type_c__alternatives[] = {
    &asn1::table::alternative_ops<event_param_meas_s, false, false>::value,
    &asn1::table::alternative_ops<event_param_value_s, false, false>::value};
const asn1::table::choice_storage_desc event_param_item_s::param_type_c_::storage_desc = {
    choice_storage_event_param_item_s_param_type_c__alternatives,
    2,
    "event_param_item_s::param_type_c_"};

event_param_meas_s& event_param_item_s::param_type_c_::set_meas()
{
  set(types::meas);
  return c.get<event_param_meas_s>();
}
event_param_value_s& event_param_item_s::param_type_c_::set_value()
{
  set(types::val);
  return c.get<event_param_value_s>();
}
static const asn1::table::seq_field choice_table_event_param_item_s_param_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_param_meas_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_param_value_s::desc}};
static const asn1::table::field_name choice_table_event_param_item_s_param_type_c__names[] = {{"measurement", nullptr},
                                                                                              {"value", nullptr}};
const asn1::table::choice_desc       event_param_item_s::param_type_c_::desc               = {
    asn1::table::enum_int_type<event_param_item_s::param_type_c_::types>(),
    event_param_item_s::param_type_c_::types::nof_types,
    event_param_item_s::param_type_c_::types::nof_exts,
    event_param_item_s::param_type_c_::types::has_ext,
    true,
    choice_table_event_param_item_s_param_type_c__alternatives,
    nullptr,
    choice_table_event_param_item_s_param_type_c__names,
    "event_param_item_s::param_type_c_",
    offsetof(event_param_item_s::param_type_c_, c.type),
    offsetof(event_param_item_s::param_type_c_, c.value),
    false,
    &event_param_item_s::param_type_c_::storage_desc};

static const char*           enum_names_event_param_item_s_param_type_c__types_values[] = {"measurement", "value"};
const asn1::table::enum_desc event_param_item_s::param_type_c_::types_opts::desc        = {
    enum_names_event_param_item_s_param_type_c__types_values,
    2,
    "event_param_item_s::param_type_c_::types"};

// EventExpression ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_expression_s_fields[] = {
    {offsetof(event_expression_s, param_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::
         item_list_codec<decltype(event_expression_s::param_list), asn1::table::sequence_item_ops, false, true>::value,
     &event_param_item_s::desc},
    {offsetof(event_expression_s, aggre_func),
     (int32_t)offsetof(event_expression_s, aggre_func_present),
     asn1::table::enum_op<decltype(event_expression_s::aggre_func)>(),
     decltype(event_expression_s::aggre_func)::has_ext,
     false,
     decltype(event_expression_s::aggre_func)::nof_types,
     decltype(event_expression_s::aggre_func)::nof_exts,
     nullptr,
     &aggregation_function_type_opts::desc}};
static const asn1::table::field_name seq_table_event_expression_s_names[] = {{"paramList", nullptr},
                                                                             {"aggreFunc", nullptr}};
static const asn1::table::seq_opt    seq_table_event_expression_s_opts[]  = {
    {1, (int32_t)offsetof(event_expression_s, aggre_func_present)}};
const asn1::table::seq_desc event_expression_s::desc = {{seq_table_event_expression_s_fields,
                                                         uint64_t{0x1},
                                                         seq_table_event_expression_s_opts,
                                                         seq_table_event_expression_s_names,
                                                         2,
                                                         1},
                                                        true,
                                                        true,
                                                        nullptr,
                                                        0};

// EventLogicalOperator ::= ENUMERATED
static const char*           enum_names_event_lc_operator_e_values[] = {"equal", "greaterthan", "lessthan"};
const asn1::table::enum_desc event_lc_operator_opts::desc            = {enum_names_event_lc_operator_e_values,
                                                                        3,
                                                                        "event_lc_operator_e"};

// EventFormula ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_formula_s_fields[] = {
    {offsetof(event_formula_s, left_expr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_expression_s::desc},
    {offsetof(event_formula_s, logic_op),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(event_formula_s::logic_op)>(),
     decltype(event_formula_s::logic_op)::has_ext,
     false,
     decltype(event_formula_s::logic_op)::nof_types,
     decltype(event_formula_s::logic_op)::nof_exts,
     nullptr,
     &event_lc_operator_opts::desc},
    {offsetof(event_formula_s, right_expr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_expression_s::desc}};
static const asn1::table::field_name seq_table_event_formula_s_names[] = {{"leftExpr", nullptr},
                                                                          {"logicOp", nullptr},
                                                                          {"rightExpr", nullptr}};
const asn1::table::seq_desc          event_formula_s::desc             = {
    {seq_table_event_formula_s_fields, uint64_t{0x7}, nullptr, seq_table_event_formula_s_names, 3, 0},
    true,
    true,
    nullptr,
    0};

// EventTriggerItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_trigger_item_s_fields[] = {
    {offsetof(event_trigger_item_s, event_cond),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_formula_s::desc},
    {offsetof(event_trigger_item_s, event_eval_period),
     (int32_t)offsetof(event_trigger_item_s, event_eval_period_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(event_trigger_item_s, lc_or),
     (int32_t)offsetof(event_trigger_item_s, lc_or_present),
     asn1::table::enum_op<decltype(event_trigger_item_s::lc_or)>(),
     decltype(event_trigger_item_s::lc_or)::has_ext,
     false,
     decltype(event_trigger_item_s::lc_or)::nof_types,
     decltype(event_trigger_item_s::lc_or)::nof_exts,
     nullptr,
     &lc_or_opts::desc}};
static const asn1::table::field_name seq_table_event_trigger_item_s_names[] = {{"eventCond", nullptr},
                                                                               {"eventEvalPeriod", nullptr},
                                                                               {"logicalOR", nullptr}};
static const asn1::table::seq_opt    seq_table_event_trigger_item_s_opts[]  = {
    {1, (int32_t)offsetof(event_trigger_item_s, event_eval_period_present)},
    {2, (int32_t)offsetof(event_trigger_item_s, lc_or_present)}};
const asn1::table::seq_desc event_trigger_item_s::desc = {{seq_table_event_trigger_item_s_fields,
                                                           uint64_t{0x1},
                                                           seq_table_event_trigger_item_s_opts,
                                                           seq_table_event_trigger_item_s_names,
                                                           3,
                                                           2},
                                                          true,
                                                          true,
                                                          nullptr,
                                                          0};

// E2SM-KPM-EventTriggerDefinition-Format2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_event_trigger_definition_format2_s_fields[] = {
    {offsetof(e2sm_kpm_event_trigger_definition_format2_s, event_trigger_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_kpm_event_trigger_definition_format2_s::event_trigger_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &event_trigger_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_event_trigger_definition_format2_s_names[] = {
    {"eventTriggerList", nullptr}};
const asn1::table::seq_desc e2sm_kpm_event_trigger_definition_format2_s::desc = {
    {seq_table_e2sm_kpm_event_trigger_definition_format2_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_kpm_event_trigger_definition_format2_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-KPM-EventTriggerDefinition ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_event_trigger_definition_s_fields[] = {
    {offsetof(e2sm_kpm_event_trigger_definition_s, event_definition_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_event_trigger_definition_s_names[] = {
    {"eventDefinition-formats", nullptr}};
const asn1::table::seq_desc e2sm_kpm_event_trigger_definition_s::desc = {
    {seq_table_e2sm_kpm_event_trigger_definition_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_kpm_event_trigger_definition_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

static const asn1::table::storage_ops* const
    choice_storage_e2sm_kpm_event_trigger_definition_s_event_definition_formats_c__alternatives[] = {
        &asn1::table::alternative_ops<e2sm_kpm_event_trigger_definition_format1_s, false>::value,
        &asn1::table::alternative_ops<e2sm_kpm_event_trigger_definition_format2_s, false, false>::value};
const asn1::table::choice_storage_desc e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::storage_desc =
    {choice_storage_e2sm_kpm_event_trigger_definition_s_event_definition_formats_c__alternatives,
     2,
     "e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_"};

e2sm_kpm_event_trigger_definition_format1_s&
e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::set_event_definition_format1()
{
  set(types::event_definition_format1);
  return c.get<e2sm_kpm_event_trigger_definition_format1_s>();
}
e2sm_kpm_event_trigger_definition_format2_s&
e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::set_event_definition_format2()
{
  set(types::event_definition_format2);
  return c.get<e2sm_kpm_event_trigger_definition_format2_s>();
}
static const asn1::table::seq_field
    choice_table_e2sm_kpm_event_trigger_definition_s_event_definition_formats_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_event_trigger_definition_format1_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_event_trigger_definition_format2_s::desc}};
static const asn1::table::field_name
    choice_table_e2sm_kpm_event_trigger_definition_s_event_definition_formats_c__names[] = {
        {"eventDefinition-Format1", nullptr},
        {"eventDefinition-Format2", nullptr}};
const asn1::table::choice_desc e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types>(),
    e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types::nof_types,
    e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types::nof_exts,
    e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_kpm_event_trigger_definition_s_event_definition_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_kpm_event_trigger_definition_s_event_definition_formats_c__names,
    "e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_",
    offsetof(e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_, c.type),
    offsetof(e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_, c.value),
    false,
    &e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::storage_desc};

static const char* enum_names_e2sm_kpm_event_trigger_definition_s_event_definition_formats_c__types_values[] = {
    "eventDefinition-Format1",
    "eventDefinition-Format2"};
const asn1::table::enum_desc e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types_opts::desc = {
    enum_names_e2sm_kpm_event_trigger_definition_s_event_definition_formats_c__types_values,
    2,
    "e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types"};
uint8_t e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "e2sm_kpm_event_trigger_definition_s::event_definition_formats_c_::types");
}

// E2SM-KPM-IndicationHeader-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_ind_hdr_format1_s_fields[] = {
    {offsetof(e2sm_kpm_ind_hdr_format1_s, collet_start_time),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_kpm_ind_hdr_format1_s::collet_start_time),
                                asn1::table::string_json>::value},
    {offsetof(e2sm_kpm_ind_hdr_format1_s, file_formatversion),
     (int32_t)offsetof(e2sm_kpm_ind_hdr_format1_s, file_formatversion_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_kpm_ind_hdr_format1_s::file_formatversion),
                                asn1::table::string_json>::value},
    {offsetof(e2sm_kpm_ind_hdr_format1_s, sender_name),
     (int32_t)offsetof(e2sm_kpm_ind_hdr_format1_s, sender_name_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_kpm_ind_hdr_format1_s::sender_name), asn1::table::string_json>::value},
    {offsetof(e2sm_kpm_ind_hdr_format1_s, sender_type),
     (int32_t)offsetof(e2sm_kpm_ind_hdr_format1_s, sender_type_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_kpm_ind_hdr_format1_s::sender_type), asn1::table::string_json>::value},
    {offsetof(e2sm_kpm_ind_hdr_format1_s, vendor_name),
     (int32_t)offsetof(e2sm_kpm_ind_hdr_format1_s, vendor_name_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_kpm_ind_hdr_format1_s::vendor_name), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_e2sm_kpm_ind_hdr_format1_s_names[] = {{"colletStartTime", nullptr},
                                                                                     {"fileFormatversion", nullptr},
                                                                                     {"senderName", nullptr},
                                                                                     {"senderType", nullptr},
                                                                                     {"vendorName", nullptr}};
static const asn1::table::seq_opt    seq_table_e2sm_kpm_ind_hdr_format1_s_opts[]  = {
    {1, (int32_t)offsetof(e2sm_kpm_ind_hdr_format1_s, file_formatversion_present)},
    {2, (int32_t)offsetof(e2sm_kpm_ind_hdr_format1_s, sender_name_present)},
    {3, (int32_t)offsetof(e2sm_kpm_ind_hdr_format1_s, sender_type_present)},
    {4, (int32_t)offsetof(e2sm_kpm_ind_hdr_format1_s, vendor_name_present)}};
static const asn1::table::seq_field seq_table_e2sm_kpm_ind_hdr_format1_s_g0_fields[] = {
    {offsetof(e2sm_kpm_ind_hdr_format1_s, job_id),
     (int32_t)offsetof(e2sm_kpm_ind_hdr_format1_s, job_id_present),
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_kpm_ind_hdr_format1_s_g0_names[] = {{"jobID", nullptr}};
static const asn1::table::seq_opt    seq_table_e2sm_kpm_ind_hdr_format1_s_g0_opts[]  = {
    {0, (int32_t)offsetof(e2sm_kpm_ind_hdr_format1_s, job_id_present)}};
static const asn1::table::seq_group seq_table_e2sm_kpm_ind_hdr_format1_s_groups[] = {
    {{seq_table_e2sm_kpm_ind_hdr_format1_s_g0_fields,
      uint64_t{0x0},
      seq_table_e2sm_kpm_ind_hdr_format1_s_g0_opts,
      seq_table_e2sm_kpm_ind_hdr_format1_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc e2sm_kpm_ind_hdr_format1_s::desc = {{seq_table_e2sm_kpm_ind_hdr_format1_s_fields,
                                                                 uint64_t{0x1},
                                                                 seq_table_e2sm_kpm_ind_hdr_format1_s_opts,
                                                                 seq_table_e2sm_kpm_ind_hdr_format1_s_names,
                                                                 5,
                                                                 4},
                                                                true,
                                                                true,
                                                                seq_table_e2sm_kpm_ind_hdr_format1_s_groups,
                                                                1};

// E2SM-KPM-IndicationHeader ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_ind_hdr_s_fields[] = {
    {offsetof(e2sm_kpm_ind_hdr_s, ind_hdr_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_ind_hdr_s_names[] = {{"indicationHeader-formats", nullptr}};
const asn1::table::seq_desc          e2sm_kpm_ind_hdr_s::desc             = {
    {seq_table_e2sm_kpm_ind_hdr_s_fields, uint64_t{0x1}, nullptr, seq_table_e2sm_kpm_ind_hdr_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

static const asn1::table::seq_field choice_table_e2sm_kpm_ind_hdr_s_ind_hdr_formats_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_ind_hdr_format1_s::desc}};
static const asn1::table::field_name choice_table_e2sm_kpm_ind_hdr_s_ind_hdr_formats_c__names[] = {
    {"indicationHeader-Format1", nullptr}};
const asn1::table::choice_desc e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types>(),
    e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types::nof_types,
    e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types::nof_exts,
    e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_kpm_ind_hdr_s_ind_hdr_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_kpm_ind_hdr_s_ind_hdr_formats_c__names,
    "e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_",
    asn1::table::choice_desc::no_member,
    offsetof(e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_, c),
    false};

static const char* enum_names_e2sm_kpm_ind_hdr_s_ind_hdr_formats_c__types_values[]    = {"indicationHeader-Format1"};
const asn1::table::enum_desc e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types_opts::desc = {
    enum_names_e2sm_kpm_ind_hdr_s_ind_hdr_formats_c__types_values,
    1,
    "e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types"};
uint8_t e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "e2sm_kpm_ind_hdr_s::ind_hdr_formats_c_::types");
}

// MeasurementRecordItem ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_meas_record_item_c_alternatives[] = {
    &asn1::table::alternative_ops<uint64_t, true>::value,
    &asn1::table::alternative_ops<real_s, false>::value,
    nullptr,
    nullptr};
const asn1::table::choice_storage_desc meas_record_item_c::storage_desc = {
    choice_storage_meas_record_item_c_alternatives,
    4,
    "meas_record_item_c"};

uint64_t& meas_record_item_c::set_integer()
{
  set(types::integer);
  return c.get<uint64_t>();
}
real_s& meas_record_item_c::set_real()
{
  set(types::real);
  return c.get<real_s>();
}
void meas_record_item_c::set_no_value()
{
  set(types::no_value);
}
void meas_record_item_c::set_not_satisfied()
{
  set(types::not_satisfied);
}
static const asn1::table::seq_field choice_table_meas_record_item_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<real_s, asn1::table::float_json>::value},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_meas_record_item_c_names[] = {{"integer", nullptr},
                                                                                {"real", nullptr},
                                                                                {"noValue", nullptr},
                                                                                {"notSatisfied", nullptr}};
const asn1::table::choice_desc meas_record_item_c::desc = {asn1::table::enum_int_type<meas_record_item_c::types>(),
                                                           meas_record_item_c::types::nof_types,
                                                           meas_record_item_c::types::nof_exts,
                                                           meas_record_item_c::types::has_ext,
                                                           true,
                                                           choice_table_meas_record_item_c_alternatives,
                                                           nullptr,
                                                           choice_table_meas_record_item_c_names,
                                                           "meas_record_item_c",
                                                           offsetof(meas_record_item_c, c.type),
                                                           offsetof(meas_record_item_c, c.value),
                                                           false,
                                                           &meas_record_item_c::storage_desc};

static const char* enum_names_meas_record_item_c_types_values[]   = {"integer", "real", "noValue", "notSatisfied"};
const asn1::table::enum_desc meas_record_item_c::types_opts::desc = {enum_names_meas_record_item_c_types_values,
                                                                     4,
                                                                     "meas_record_item_c::types"};

// MeasurementDataItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_data_item_s_fields[] = {
    {offsetof(meas_data_item_s, meas_record),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2147483646,
     &asn1::table::item_list_codec<decltype(meas_data_item_s::meas_record), asn1::table::choice_item_ops, false, true>::
         value,
     &meas_record_item_c::desc},
    {offsetof(meas_data_item_s, incomplete_flag),
     (int32_t)offsetof(meas_data_item_s, incomplete_flag_present),
     asn1::table::enum_op<decltype(meas_data_item_s::incomplete_flag)>(),
     decltype(meas_data_item_s::incomplete_flag)::has_ext,
     false,
     decltype(meas_data_item_s::incomplete_flag)::nof_types,
     decltype(meas_data_item_s::incomplete_flag)::nof_exts,
     nullptr,
     &meas_data_item_s::incomplete_flag_opts::desc}};
static const asn1::table::field_name seq_table_meas_data_item_s_names[] = {{"measRecord", nullptr},
                                                                           {"incompleteFlag", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_data_item_s_opts[]  = {
    {1, (int32_t)offsetof(meas_data_item_s, incomplete_flag_present)}};
const asn1::table::seq_desc meas_data_item_s::desc = {{seq_table_meas_data_item_s_fields,
                                                       uint64_t{0x1},
                                                       seq_table_meas_data_item_s_opts,
                                                       seq_table_meas_data_item_s_names,
                                                       2,
                                                       1},
                                                      true,
                                                      true,
                                                      nullptr,
                                                      0};

static const char*           enum_names_meas_data_item_s_incomplete_flag_e__values[] = {"true"};
const asn1::table::enum_desc meas_data_item_s::incomplete_flag_opts::desc            = {
    enum_names_meas_data_item_s_incomplete_flag_e__values,
    1,
    "meas_data_item_s::incomplete_flag_e_"};

// E2SM-KPM-IndicationMessage-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_ind_msg_format1_s_fields[] = {
    {offsetof(e2sm_kpm_ind_msg_format1_s, meas_data),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_kpm_ind_msg_format1_s::meas_data),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &meas_data_item_s::desc},
    {offsetof(e2sm_kpm_ind_msg_format1_s, meas_info_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_kpm_ind_msg_format1_s::meas_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &meas_info_item_s::desc},
    {offsetof(e2sm_kpm_ind_msg_format1_s, granul_period),
     (int32_t)offsetof(e2sm_kpm_ind_msg_format1_s, granul_period_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_kpm_ind_msg_format1_s_names[] = {{"measData", nullptr},
                                                                                     {"measInfoList", nullptr},
                                                                                     {"granulPeriod", nullptr}};
static const asn1::table::seq_opt    seq_table_e2sm_kpm_ind_msg_format1_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(e2sm_kpm_ind_msg_format1_s, granul_period_present)}};
const asn1::table::seq_desc e2sm_kpm_ind_msg_format1_s::desc = {{seq_table_e2sm_kpm_ind_msg_format1_s_fields,
                                                                 uint64_t{0x1},
                                                                 seq_table_e2sm_kpm_ind_msg_format1_s_opts,
                                                                 seq_table_e2sm_kpm_ind_msg_format1_s_names,
                                                                 3,
                                                                 2},
                                                                true,
                                                                true,
                                                                nullptr,
                                                                0};

// MatchingUEidItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_matching_ue_id_item_s_fields[] = {{offsetof(matching_ue_id_item_s, ue_id),
                                                                                 asn1::table::seq_field::mandatory,
                                                                                 asn1::table::field_op::choice,
                                                                                 false,
                                                                                 false,
                                                                                 0,
                                                                                 0,
                                                                                 nullptr,
                                                                                 &ue_id_c::desc}};
static const asn1::table::field_name seq_table_matching_ue_id_item_s_names[] = {{"ueID", nullptr}};
const asn1::table::seq_desc          matching_ue_id_item_s::desc             = {
    {seq_table_matching_ue_id_item_s_fields, uint64_t{0x1}, nullptr, seq_table_matching_ue_id_item_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// MatchingUEidItem-PerGP ::= SEQUENCE
static const asn1::table::seq_field seq_table_matching_ue_id_item_per_gp_s_fields[] = {
    {offsetof(matching_ue_id_item_per_gp_s, ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_id_c::desc}};
static const asn1::table::field_name seq_table_matching_ue_id_item_per_gp_s_names[] = {{"ueID", nullptr}};
const asn1::table::seq_desc matching_ue_id_item_per_gp_s::desc = {{seq_table_matching_ue_id_item_per_gp_s_fields,
                                                                   uint64_t{0x1},
                                                                   nullptr,
                                                                   seq_table_matching_ue_id_item_per_gp_s_names,
                                                                   1,
                                                                   0},
                                                                  true,
                                                                  true,
                                                                  nullptr,
                                                                  0};

// MatchingUEidPerGP-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_matching_ue_id_per_gp_item_s_fields[] = {
    {offsetof(matching_ue_id_per_gp_item_s, matched_per_gp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &matching_ue_id_per_gp_item_s::matched_per_gp_c_::desc}};
static const asn1::table::field_name seq_table_matching_ue_id_per_gp_item_s_names[] = {{"matchedPerGP", nullptr}};
const asn1::table::seq_desc matching_ue_id_per_gp_item_s::desc = {{seq_table_matching_ue_id_per_gp_item_s_fields,
                                                                   uint64_t{0x1},
                                                                   nullptr,
                                                                   seq_table_matching_ue_id_per_gp_item_s_names,
                                                                   1,
                                                                   0},
                                                                  true,
                                                                  true,
                                                                  nullptr,
                                                                  0};

static const asn1::table::storage_ops* const
    choice_storage_matching_ue_id_per_gp_item_s_matched_per_gp_c__alternatives[] = {
        &asn1::table::alternative_ops<matching_ue_id_per_gp_item_s::matched_per_gp_c_::no_ue_matched_e_, true>::value,
        &asn1::table::alternative_ops<matching_ue_id_list_per_gp_l, false>::value};
const asn1::table::choice_storage_desc matching_ue_id_per_gp_item_s::matched_per_gp_c_::storage_desc = {
    choice_storage_matching_ue_id_per_gp_item_s_matched_per_gp_c__alternatives,
    2,
    "matching_ue_id_per_gp_item_s::matched_per_gp_c_"};

matching_ue_id_per_gp_item_s::matched_per_gp_c_::no_ue_matched_e_&
matching_ue_id_per_gp_item_s::matched_per_gp_c_::set_no_ue_matched()
{
  set(types::no_ue_matched);
  return c.get<no_ue_matched_e_>();
}
matching_ue_id_list_per_gp_l& matching_ue_id_per_gp_item_s::matched_per_gp_c_::set_one_or_more_ue_matched()
{
  set(types::one_or_more_ue_matched);
  return c.get<matching_ue_id_list_per_gp_l>();
}
static const asn1::table::seq_field choice_table_matching_ue_id_per_gp_item_s_matched_per_gp_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<matching_ue_id_per_gp_item_s::matched_per_gp_c_::no_ue_matched_e_>(),
     matching_ue_id_per_gp_item_s::matched_per_gp_c_::no_ue_matched_e_::has_ext,
     false,
     matching_ue_id_per_gp_item_s::matched_per_gp_c_::no_ue_matched_e_::nof_types,
     matching_ue_id_per_gp_item_s::matched_per_gp_c_::no_ue_matched_e_::nof_exts,
     nullptr,
     &matching_ue_id_per_gp_item_s::matched_per_gp_c_::no_ue_matched_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::list_codec<matching_ue_id_list_per_gp_l, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_matching_ue_id_per_gp_item_s_matched_per_gp_c__names[] = {
    {"noUEmatched", nullptr},
    {"oneOrMoreUEmatched", nullptr}};
const asn1::table::choice_desc matching_ue_id_per_gp_item_s::matched_per_gp_c_::desc = {
    asn1::table::enum_int_type<matching_ue_id_per_gp_item_s::matched_per_gp_c_::types>(),
    matching_ue_id_per_gp_item_s::matched_per_gp_c_::types::nof_types,
    matching_ue_id_per_gp_item_s::matched_per_gp_c_::types::nof_exts,
    matching_ue_id_per_gp_item_s::matched_per_gp_c_::types::has_ext,
    true,
    choice_table_matching_ue_id_per_gp_item_s_matched_per_gp_c__alternatives,
    nullptr,
    choice_table_matching_ue_id_per_gp_item_s_matched_per_gp_c__names,
    "matching_ue_id_per_gp_item_s::matched_per_gp_c_",
    offsetof(matching_ue_id_per_gp_item_s::matched_per_gp_c_, c.type),
    offsetof(matching_ue_id_per_gp_item_s::matched_per_gp_c_, c.value),
    false,
    &matching_ue_id_per_gp_item_s::matched_per_gp_c_::storage_desc};

static const char* enum_names_matching_ue_id_per_gp_item_s_matched_per_gp_c__no_ue_matched_e__values[] = {"true"};
const asn1::table::enum_desc matching_ue_id_per_gp_item_s::matched_per_gp_c_::no_ue_matched_opts::desc = {
    enum_names_matching_ue_id_per_gp_item_s_matched_per_gp_c__no_ue_matched_e__values,
    1,
    "matching_ue_id_per_gp_item_s::matched_per_gp_c_::no_ue_matched_e_"};

static const char*           enum_names_matching_ue_id_per_gp_item_s_matched_per_gp_c__types_values[] = {"noUEmatched",
                                                                                                         "oneOrMoreUEmatched"};
const asn1::table::enum_desc matching_ue_id_per_gp_item_s::matched_per_gp_c_::types_opts::desc        = {
    enum_names_matching_ue_id_per_gp_item_s_matched_per_gp_c__types_values,
    2,
    "matching_ue_id_per_gp_item_s::matched_per_gp_c_::types"};
uint8_t matching_ue_id_per_gp_item_s::matched_per_gp_c_::types_opts::to_number() const
{
  if (value == one_or_more_ue_matched) {
    return 1;
  }
  invalid_enum_number(value, "matching_ue_id_per_gp_item_s::matched_per_gp_c_::types");
  return 0;
}

// MeasurementCondUEidItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_cond_ue_id_item_s_fields[] = {
    {offsetof(meas_cond_ue_id_item_s, meas_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_type_c::desc},
    {offsetof(meas_cond_ue_id_item_s, matching_cond),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32767,
     &asn1::table::item_list_codec<decltype(meas_cond_ue_id_item_s::matching_cond),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &matching_cond_item_s::desc},
    {offsetof(meas_cond_ue_id_item_s, matching_ue_id_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(meas_cond_ue_id_item_s::matching_ue_id_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &matching_ue_id_item_s::desc}};
static const asn1::table::field_name seq_table_meas_cond_ue_id_item_s_names[] = {{"measType", nullptr},
                                                                                 {"matchingCond", nullptr},
                                                                                 {"matchingUEidList", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_cond_ue_id_item_s_opts[]  = {
    {2, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_meas_cond_ue_id_item_s_g0_fields[] = {
    {offsetof(meas_cond_ue_id_item_s, matching_ue_id_per_gp),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::ptr_item_list_codec<decltype(meas_cond_ue_id_item_s::matching_ue_id_per_gp),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &matching_ue_id_per_gp_item_s::desc}};
static const asn1::table::field_name seq_table_meas_cond_ue_id_item_s_g0_names[] = {{"matchingUEidPerGP", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_cond_ue_id_item_s_g0_opts[]  = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_meas_cond_ue_id_item_s_groups[] = {
    {{seq_table_meas_cond_ue_id_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_meas_cond_ue_id_item_s_g0_opts,
      seq_table_meas_cond_ue_id_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc meas_cond_ue_id_item_s::desc = {{seq_table_meas_cond_ue_id_item_s_fields,
                                                             uint64_t{0x3},
                                                             seq_table_meas_cond_ue_id_item_s_opts,
                                                             seq_table_meas_cond_ue_id_item_s_names,
                                                             3,
                                                             1},
                                                            true,
                                                            true,
                                                            seq_table_meas_cond_ue_id_item_s_groups,
                                                            1};

// E2SM-KPM-IndicationMessage-Format2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_ind_msg_format2_s_fields[] = {
    {offsetof(e2sm_kpm_ind_msg_format2_s, meas_data),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_kpm_ind_msg_format2_s::meas_data),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &meas_data_item_s::desc},
    {offsetof(e2sm_kpm_ind_msg_format2_s, meas_cond_ue_id_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_kpm_ind_msg_format2_s::meas_cond_ue_id_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &meas_cond_ue_id_item_s::desc},
    {offsetof(e2sm_kpm_ind_msg_format2_s, granul_period),
     (int32_t)offsetof(e2sm_kpm_ind_msg_format2_s, granul_period_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_kpm_ind_msg_format2_s_names[] = {{"measData", nullptr},
                                                                                     {"measCondUEidList", nullptr},
                                                                                     {"granulPeriod", nullptr}};
static const asn1::table::seq_opt    seq_table_e2sm_kpm_ind_msg_format2_s_opts[]  = {
    {2, (int32_t)offsetof(e2sm_kpm_ind_msg_format2_s, granul_period_present)}};
const asn1::table::seq_desc e2sm_kpm_ind_msg_format2_s::desc = {{seq_table_e2sm_kpm_ind_msg_format2_s_fields,
                                                                 uint64_t{0x3},
                                                                 seq_table_e2sm_kpm_ind_msg_format2_s_opts,
                                                                 seq_table_e2sm_kpm_ind_msg_format2_s_names,
                                                                 3,
                                                                 1},
                                                                true,
                                                                true,
                                                                nullptr,
                                                                0};

// UEMeasurementReportItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_meas_report_item_s_fields[] = {
    {offsetof(ue_meas_report_item_s, ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_id_c::desc},
    {offsetof(ue_meas_report_item_s, meas_report),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_ind_msg_format1_s::desc}};
static const asn1::table::field_name seq_table_ue_meas_report_item_s_names[] = {{"ueID", nullptr},
                                                                                {"measReport", nullptr}};
const asn1::table::seq_desc          ue_meas_report_item_s::desc             = {
    {seq_table_ue_meas_report_item_s_fields, uint64_t{0x3}, nullptr, seq_table_ue_meas_report_item_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// E2SM-KPM-IndicationMessage-Format3 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_ind_msg_format3_s_fields[] = {
    {offsetof(e2sm_kpm_ind_msg_format3_s, ue_meas_report_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_kpm_ind_msg_format3_s::ue_meas_report_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ue_meas_report_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_ind_msg_format3_s_names[] = {{"ueMeasReportList", nullptr}};
const asn1::table::seq_desc          e2sm_kpm_ind_msg_format3_s::desc = {{seq_table_e2sm_kpm_ind_msg_format3_s_fields,
                                                                          uint64_t{0x1},
                                                                          nullptr,
                                                                          seq_table_e2sm_kpm_ind_msg_format3_s_names,
                                                                          1,
                                                                          0},
                                                                         true,
                                                                         true,
                                                                         nullptr,
                                                                         0};

// E2SM-KPM-IndicationMessage-Fundamental ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_ind_msg_fundamental_s_fields[] = {
    {offsetof(e2sm_kpm_ind_msg_fundamental_s, ind_msg_fundamental_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_ind_msg_fundamental_s_names[] = {
    {"indicationMessageFundamental-formats", nullptr}};
const asn1::table::seq_desc e2sm_kpm_ind_msg_fundamental_s::desc = {{seq_table_e2sm_kpm_ind_msg_fundamental_s_fields,
                                                                     uint64_t{0x1},
                                                                     nullptr,
                                                                     seq_table_e2sm_kpm_ind_msg_fundamental_s_names,
                                                                     1,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

static const asn1::table::storage_ops* const
    choice_storage_e2sm_kpm_ind_msg_fundamental_s_ind_msg_fundamental_formats_c__alternatives[] = {
        &asn1::table::alternative_ops<e2sm_kpm_ind_msg_format1_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_kpm_ind_msg_format2_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_kpm_ind_msg_format3_s, false, false>::value};
const asn1::table::choice_storage_desc e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_::storage_desc = {
    choice_storage_e2sm_kpm_ind_msg_fundamental_s_ind_msg_fundamental_formats_c__alternatives,
    3,
    "e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_"};

e2sm_kpm_ind_msg_format1_s& e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_::set_ind_msg_format1()
{
  set(types::ind_msg_format1);
  return c.get<e2sm_kpm_ind_msg_format1_s>();
}
e2sm_kpm_ind_msg_format2_s& e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_::set_ind_msg_format2()
{
  set(types::ind_msg_format2);
  return c.get<e2sm_kpm_ind_msg_format2_s>();
}
e2sm_kpm_ind_msg_format3_s& e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_::set_ind_msg_format3()
{
  set(types::ind_msg_format3);
  return c.get<e2sm_kpm_ind_msg_format3_s>();
}
static const asn1::table::seq_field
    choice_table_e2sm_kpm_ind_msg_fundamental_s_ind_msg_fundamental_formats_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_ind_msg_format1_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_ind_msg_format2_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_kpm_ind_msg_format3_s::desc}};
static const asn1::table::field_name
    choice_table_e2sm_kpm_ind_msg_fundamental_s_ind_msg_fundamental_formats_c__names[] = {
        {"indicationMessage-Format1", nullptr},
        {"indicationMessage-Format2", nullptr},
        {"indicationMessage-Format3", nullptr}};
const asn1::table::choice_desc e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_::types>(),
    e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_::types::nof_types,
    e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_::types::nof_exts,
    e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_kpm_ind_msg_fundamental_s_ind_msg_fundamental_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_kpm_ind_msg_fundamental_s_ind_msg_fundamental_formats_c__names,
    "e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_",
    offsetof(e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_, c.type),
    offsetof(e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_, c.value),
    false,
    &e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_::storage_desc};

static const char* enum_names_e2sm_kpm_ind_msg_fundamental_s_ind_msg_fundamental_formats_c__types_values[] = {
    "indicationMessage-Format1",
    "indicationMessage-Format2",
    "indicationMessage-Format3"};
const asn1::table::enum_desc e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_::types_opts::desc = {
    enum_names_e2sm_kpm_ind_msg_fundamental_s_ind_msg_fundamental_formats_c__types_values,
    3,
    "e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_::types"};
uint8_t e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(numbers, 3, value, "e2sm_kpm_ind_msg_fundamental_s::ind_msg_fundamental_formats_c_::types");
}

// MultiReportIndicationMessageItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_multi_report_ind_msg_item_s_fields[] = {
    {offsetof(multi_report_ind_msg_item_s, job_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(multi_report_ind_msg_item_s, ind_msg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_ind_msg_fundamental_s::desc}};
static const asn1::table::field_name seq_table_multi_report_ind_msg_item_s_names[] = {{"jobID", nullptr},
                                                                                      {"indicationMessage", nullptr}};
const asn1::table::seq_desc          multi_report_ind_msg_item_s::desc = {{seq_table_multi_report_ind_msg_item_s_fields,
                                                                           uint64_t{0x3},
                                                                           nullptr,
                                                                           seq_table_multi_report_ind_msg_item_s_names,
                                                                           2,
                                                                           0},
                                                                          true,
                                                                          true,
                                                                          nullptr,
                                                                          0};

// E2SM-KPM-IndicationMessage-Format255 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_ind_msg_format255_s_fields[] = {
    {offsetof(e2sm_kpm_ind_msg_format255_s, multi_report_ind_msg_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_kpm_ind_msg_format255_s::multi_report_ind_msg_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &multi_report_ind_msg_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_ind_msg_format255_s_names[] = {
    {"multiReportIndicationMessageList", nullptr}};
const asn1::table::seq_desc e2sm_kpm_ind_msg_format255_s::desc = {{seq_table_e2sm_kpm_ind_msg_format255_s_fields,
                                                                   uint64_t{0x1},
                                                                   nullptr,
                                                                   seq_table_e2sm_kpm_ind_msg_format255_s_names,
                                                                   1,
                                                                   0},
                                                                  true,
                                                                  true,
                                                                  nullptr,
                                                                  0};

// E2SM-KPM-IndicationMessage ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_ind_msg_s_fields[] = {
    {offsetof(e2sm_kpm_ind_msg_s, ind_msg_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_ind_msg_s::ind_msg_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_ind_msg_s_names[] = {{"indicationMessage-formats", nullptr}};
const asn1::table::seq_desc          e2sm_kpm_ind_msg_s::desc             = {
    {seq_table_e2sm_kpm_ind_msg_s_fields, uint64_t{0x1}, nullptr, seq_table_e2sm_kpm_ind_msg_s_names, 1, 0},
    true,
    true,
    nullptr,
    0,
    "E2SM-KPM-IndicationMessage"};

static const asn1::table::storage_ops* const choice_storage_e2sm_kpm_ind_msg_s_ind_msg_formats_c__alternatives[] = {
    &asn1::table::alternative_ops<e2sm_kpm_ind_msg_format1_s, false, false>::value,
    &asn1::table::alternative_ops<e2sm_kpm_ind_msg_format2_s, false, false>::value,
    &asn1::table::alternative_ops<e2sm_kpm_ind_msg_format3_s, false, false>::value,
    &asn1::table::alternative_ops<e2sm_kpm_ind_msg_format255_s, false, false>::value};
const asn1::table::choice_storage_desc e2sm_kpm_ind_msg_s::ind_msg_formats_c_::storage_desc = {
    choice_storage_e2sm_kpm_ind_msg_s_ind_msg_formats_c__alternatives,
    4,
    "e2sm_kpm_ind_msg_s::ind_msg_formats_c_"};

e2sm_kpm_ind_msg_format1_s& e2sm_kpm_ind_msg_s::ind_msg_formats_c_::set_ind_msg_format1()
{
  set(types::ind_msg_format1);
  return c.get<e2sm_kpm_ind_msg_format1_s>();
}
e2sm_kpm_ind_msg_format2_s& e2sm_kpm_ind_msg_s::ind_msg_formats_c_::set_ind_msg_format2()
{
  set(types::ind_msg_format2);
  return c.get<e2sm_kpm_ind_msg_format2_s>();
}
e2sm_kpm_ind_msg_format3_s& e2sm_kpm_ind_msg_s::ind_msg_formats_c_::set_ind_msg_format3()
{
  set(types::ind_msg_format3);
  return c.get<e2sm_kpm_ind_msg_format3_s>();
}
e2sm_kpm_ind_msg_format255_s& e2sm_kpm_ind_msg_s::ind_msg_formats_c_::set_ind_msg_format255()
{
  set(types::ind_msg_format255);
  return c.get<e2sm_kpm_ind_msg_format255_s>();
}
static const asn1::table::seq_field choice_table_e2sm_kpm_ind_msg_s_ind_msg_formats_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_ind_msg_format1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_ind_msg_format2_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_ind_msg_format3_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_kpm_ind_msg_format255_s::desc}};
static const asn1::table::field_name choice_table_e2sm_kpm_ind_msg_s_ind_msg_formats_c__names[] = {
    {"indicationMessage-Format1", nullptr},
    {"indicationMessage-Format2", nullptr},
    {"indicationMessage-Format3", nullptr},
    {"indicationMessage-Format255", nullptr}};
const asn1::table::choice_desc e2sm_kpm_ind_msg_s::ind_msg_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_kpm_ind_msg_s::ind_msg_formats_c_::types>(),
    e2sm_kpm_ind_msg_s::ind_msg_formats_c_::types::nof_types,
    e2sm_kpm_ind_msg_s::ind_msg_formats_c_::types::nof_exts,
    e2sm_kpm_ind_msg_s::ind_msg_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_kpm_ind_msg_s_ind_msg_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_kpm_ind_msg_s_ind_msg_formats_c__names,
    "e2sm_kpm_ind_msg_s::ind_msg_formats_c_",
    offsetof(e2sm_kpm_ind_msg_s::ind_msg_formats_c_, c.type),
    offsetof(e2sm_kpm_ind_msg_s::ind_msg_formats_c_, c.value),
    false,
    &e2sm_kpm_ind_msg_s::ind_msg_formats_c_::storage_desc};

static const char* enum_names_e2sm_kpm_ind_msg_s_ind_msg_formats_c__types_values[]    = {"indicationMessage-Format1",
                                                                                         "indicationMessage-Format2",
                                                                                         "indicationMessage-Format3",
                                                                                         "indicationMessage-Format255"};
const asn1::table::enum_desc e2sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::desc = {
    enum_names_e2sm_kpm_ind_msg_s_ind_msg_formats_c__types_values,
    4,
    "e2sm_kpm_ind_msg_s::ind_msg_formats_c_::types"};
uint16_t e2sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 2, 3, 255};
  return map_enum_number(numbers, 4, value, "e2sm_kpm_ind_msg_s::ind_msg_formats_c_::types");
}

// RIC-EventTriggerStyle-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_event_trigger_style_item_s_fields[] = {
    {offsetof(ric_event_trigger_style_item_s, ric_event_trigger_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ric_event_trigger_style_item_s, ric_event_trigger_style_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_event_trigger_style_item_s::ric_event_trigger_style_name),
                                asn1::table::string_json>::value},
    {offsetof(ric_event_trigger_style_item_s, ric_event_trigger_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_ric_event_trigger_style_item_s_names[] = {
    {"ric-EventTriggerStyle-Type", nullptr},
    {"ric-EventTriggerStyle-Name", nullptr},
    {"ric-EventTriggerFormat-Type", nullptr}};
const asn1::table::seq_desc ric_event_trigger_style_item_s::desc = {{seq_table_ric_event_trigger_style_item_s_fields,
                                                                     uint64_t{0x7},
                                                                     nullptr,
                                                                     seq_table_ric_event_trigger_style_item_s_names,
                                                                     3,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// MeasurementInfo-Action-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_info_action_item_s_fields[] = {
    {offsetof(meas_info_action_item_s, meas_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(meas_info_action_item_s::meas_name), asn1::table::string_json>::value},
    {offsetof(meas_info_action_item_s, meas_id),
     (int32_t)offsetof(meas_info_action_item_s, meas_id_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65535},
     nullptr}};
static const asn1::table::field_name seq_table_meas_info_action_item_s_names[] = {{"measName", nullptr},
                                                                                  {"measID", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_info_action_item_s_opts[]  = {
    {1, (int32_t)offsetof(meas_info_action_item_s, meas_id_present)}};
static const asn1::table::seq_field seq_table_meas_info_action_item_s_g0_fields[] = {
    {offsetof(meas_info_action_item_s, bin_range_def),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(meas_info_action_item_s::bin_range_def)>::value,
     &bin_range_definition_s::desc}};
static const asn1::table::field_name seq_table_meas_info_action_item_s_g0_names[] = {{"binRangeDef", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_info_action_item_s_g0_opts[]  = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_meas_info_action_item_s_groups[] = {
    {{seq_table_meas_info_action_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_meas_info_action_item_s_g0_opts,
      seq_table_meas_info_action_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc meas_info_action_item_s::desc = {{seq_table_meas_info_action_item_s_fields,
                                                              uint64_t{0x1},
                                                              seq_table_meas_info_action_item_s_opts,
                                                              seq_table_meas_info_action_item_s_names,
                                                              2,
                                                              1},
                                                             true,
                                                             true,
                                                             seq_table_meas_info_action_item_s_groups,
                                                             1};

// RIC-ReportStyle-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_report_style_item_s_fields[] = {
    {offsetof(ric_report_style_item_s, ric_report_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ric_report_style_item_s, ric_report_style_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_report_style_item_s::ric_report_style_name),
                                asn1::table::string_json>::value},
    {offsetof(ric_report_style_item_s, ric_action_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ric_report_style_item_s, meas_info_action_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ric_report_style_item_s::meas_info_action_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &meas_info_action_item_s::desc},
    {offsetof(ric_report_style_item_s, ric_ind_hdr_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ric_report_style_item_s, ric_ind_msg_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_ric_report_style_item_s_names[] = {
    {"ric-ReportStyle-Type", nullptr},
    {"ric-ReportStyle-Name", nullptr},
    {"ric-ActionFormat-Type", nullptr},
    {"measInfo-Action-List", nullptr},
    {"ric-IndicationHeaderFormat-Type", nullptr},
    {"ric-IndicationMessageFormat-Type", nullptr}};
const asn1::table::seq_desc ric_report_style_item_s::desc = {
    {seq_table_ric_report_style_item_s_fields, uint64_t{0x3f}, nullptr, seq_table_ric_report_style_item_s_names, 6, 0},
    true,
    true,
    nullptr,
    0};

// RIC-FundamentalLevelReportStyle-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_fundamental_level_report_style_item_s_fields[] = {
    {offsetof(ric_fundamental_level_report_style_item_s, ric_report_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_ric_fundamental_level_report_style_item_s_names[] = {
    {"ric-ReportStyle-Type", nullptr}};
const asn1::table::seq_desc ric_fundamental_level_report_style_item_s::desc = {
    {seq_table_ric_fundamental_level_report_style_item_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_ric_fundamental_level_report_style_item_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// RIC-IntegratedReportStyle-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_integrated_report_style_item_s_fields[] = {
    {offsetof(ric_integrated_report_style_item_s, ric_report_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ric_integrated_report_style_item_s, ric_report_style_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_integrated_report_style_item_s::ric_report_style_name),
                                asn1::table::string_json>::value},
    {offsetof(ric_integrated_report_style_item_s, ric_action_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ric_integrated_report_style_item_s, ric_ind_hdr_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ric_integrated_report_style_item_s, ric_ind_msg_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ric_integrated_report_style_item_s, ric_fundamental_level_report_style_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<
         decltype(ric_integrated_report_style_item_s::ric_fundamental_level_report_style_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &ric_fundamental_level_report_style_item_s::desc},
    {offsetof(ric_integrated_report_style_item_s, ric_multiple_individual_report_support),
     (int32_t)offsetof(ric_integrated_report_style_item_s, ric_multiple_individual_report_support_present),
     asn1::table::enum_op<decltype(ric_integrated_report_style_item_s::ric_multiple_individual_report_support)>(),
     decltype(ric_integrated_report_style_item_s::ric_multiple_individual_report_support)::has_ext,
     false,
     decltype(ric_integrated_report_style_item_s::ric_multiple_individual_report_support)::nof_types,
     decltype(ric_integrated_report_style_item_s::ric_multiple_individual_report_support)::nof_exts,
     nullptr,
     &ric_integrated_report_style_item_s::ric_multiple_individual_report_support_opts::desc}};
static const asn1::table::field_name seq_table_ric_integrated_report_style_item_s_names[] = {
    {"ric-ReportStyle-Type", nullptr},
    {"ric-ReportStyle-Name", nullptr},
    {"ric-ActionFormat-Type", nullptr},
    {"ric-IndicationHeaderFormat-Type", nullptr},
    {"ric-IndicationMessageFormat-Type", nullptr},
    {"ric-FundamentalLevelReportStyle-List", nullptr},
    {"ric-MultipleIndividualReportingSupport", nullptr}};
static const asn1::table::seq_opt seq_table_ric_integrated_report_style_item_s_opts[] = {
    {6, (int32_t)offsetof(ric_integrated_report_style_item_s, ric_multiple_individual_report_support_present)}};
const asn1::table::seq_desc ric_integrated_report_style_item_s::desc = {
    {seq_table_ric_integrated_report_style_item_s_fields,
     uint64_t{0x3f},
     seq_table_ric_integrated_report_style_item_s_opts,
     seq_table_ric_integrated_report_style_item_s_names,
     7,
     1},
    true,
    true,
    nullptr,
    0};

static const char* enum_names_ric_integrated_report_style_item_s_ric_multiple_individual_report_support_e__values[] = {
    "true"};
const asn1::table::enum_desc ric_integrated_report_style_item_s::ric_multiple_individual_report_support_opts::desc = {
    enum_names_ric_integrated_report_style_item_s_ric_multiple_individual_report_support_e__values,
    1,
    "ric_integrated_report_style_item_s::ric_multiple_individual_report_support_e_"};

// E2SM-KPM-RANfunction-Description ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_kpm_ra_nfunction_description_s_fields[] = {
    {offsetof(e2sm_kpm_ra_nfunction_description_s, ran_function_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ra_nfunction_name_s::desc},
    {offsetof(e2sm_kpm_ra_nfunction_description_s, ric_event_trigger_style_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(e2sm_kpm_ra_nfunction_description_s::ric_event_trigger_style_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ric_event_trigger_style_item_s::desc},
    {offsetof(e2sm_kpm_ra_nfunction_description_s, ric_report_style_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(e2sm_kpm_ra_nfunction_description_s::ric_report_style_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ric_report_style_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_ra_nfunction_description_s_names[] = {
    {"ranFunction-Name", nullptr},
    {"ric-EventTriggerStyle-List", nullptr},
    {"ric-ReportStyle-List", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_kpm_ra_nfunction_description_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_e2sm_kpm_ra_nfunction_description_s_g0_fields[] = {
    {offsetof(e2sm_kpm_ra_nfunction_description_s, ric_integrated_report_style_list),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::ptr_item_list_codec<decltype(e2sm_kpm_ra_nfunction_description_s::ric_integrated_report_style_list),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &ric_integrated_report_style_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_kpm_ra_nfunction_description_s_g0_names[] = {
    {"ric-IntegratedReportStyle-List", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_kpm_ra_nfunction_description_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_e2sm_kpm_ra_nfunction_description_s_groups[] = {
    {{seq_table_e2sm_kpm_ra_nfunction_description_s_g0_fields,
      uint64_t{0x0},
      seq_table_e2sm_kpm_ra_nfunction_description_s_g0_opts,
      seq_table_e2sm_kpm_ra_nfunction_description_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc e2sm_kpm_ra_nfunction_description_s::desc = {
    {seq_table_e2sm_kpm_ra_nfunction_description_s_fields,
     uint64_t{0x1},
     seq_table_e2sm_kpm_ra_nfunction_description_s_opts,
     seq_table_e2sm_kpm_ra_nfunction_description_s_names,
     3,
     2},
    true,
    true,
    seq_table_e2sm_kpm_ra_nfunction_description_s_groups,
    1};
