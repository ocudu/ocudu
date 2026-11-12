// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/nr_ue_variables.h"
#include "ocudu/asn1/asn1_codec.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// CLI-TriggeredList-r16 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_cli_triggered_list_r16_c_alternatives[] = {
    &asn1::table::alternative_ops<srs_rsrp_triggered_list_r16_l, false>::value,
    &asn1::table::alternative_ops<cli_rssi_triggered_list_r16_l, false>::value};
const asn1::table::choice_storage_desc cli_triggered_list_r16_c::storage_desc = {
    choice_storage_cli_triggered_list_r16_c_alternatives,
    2,
    "cli_triggered_list_r16_c"};

srs_rsrp_triggered_list_r16_l& cli_triggered_list_r16_c::set_srs_rsrp_triggered_list_r16()
{
  set(types::srs_rsrp_triggered_list_r16);
  return c.get<srs_rsrp_triggered_list_r16_l>();
}
cli_rssi_triggered_list_r16_l& cli_triggered_list_r16_c::set_cli_rssi_triggered_list_r16()
{
  set(types::cli_rssi_triggered_list_r16);
  return c.get<cli_rssi_triggered_list_r16_l>();
}
static const asn1::table::seq_field choice_table_cli_triggered_list_r16_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::list_codec<srs_rsrp_triggered_list_r16_l,
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{63}, false, false>>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::list_codec<cli_rssi_triggered_list_r16_l,
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{63}, false, false>>::value}};
static const asn1::table::field_name choice_table_cli_triggered_list_r16_c_names[] = {
    {"srs-RSRP-TriggeredList-r16", nullptr},
    {"cli-RSSI-TriggeredList-r16", nullptr}};
const asn1::table::choice_desc cli_triggered_list_r16_c::desc = {
    asn1::table::enum_int_type<cli_triggered_list_r16_c::types>(),
    cli_triggered_list_r16_c::types::nof_types,
    cli_triggered_list_r16_c::types::nof_exts,
    cli_triggered_list_r16_c::types::has_ext,
    false,
    choice_table_cli_triggered_list_r16_c_alternatives,
    nullptr,
    choice_table_cli_triggered_list_r16_c_names,
    "cli_triggered_list_r16_c",
    offsetof(cli_triggered_list_r16_c, c.type),
    offsetof(cli_triggered_list_r16_c, c.value),
    false,
    &cli_triggered_list_r16_c::storage_desc};

static const char*           enum_names_cli_triggered_list_r16_c_types_values[] = {"srs-RSRP-TriggeredList-r16",
                                                                                   "cli-RSSI-TriggeredList-r16"};
const asn1::table::enum_desc cli_triggered_list_r16_c::types_opts::desc         = {
    enum_names_cli_triggered_list_r16_c_types_values,
    2,
    "cli_triggered_list_r16_c::types"};

static const asn1::table::storage_ops* const choice_storage_cells_triggered_list_item_c__alternatives[] = {
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value};
const asn1::table::choice_storage_desc cells_triggered_list_item_c_::storage_desc = {
    choice_storage_cells_triggered_list_item_c__alternatives,
    3,
    "cells_triggered_list_item_c_"};

uint16_t& cells_triggered_list_item_c_::set_pci()
{
  set(types::pci);
  return c.get<uint16_t>();
}
uint16_t& cells_triggered_list_item_c_::set_pci_eutra()
{
  set(types::pci_eutra);
  return c.get<uint16_t>();
}
uint16_t& cells_triggered_list_item_c_::set_pci_utra_fdd_r16()
{
  set(types::pci_utra_fdd_r16);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field choice_table_cells_triggered_list_item_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{1007},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{503},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{511},
     nullptr}};
static const asn1::table::field_name choice_table_cells_triggered_list_item_c__names[] = {
    {"physCellId", nullptr},
    {"physCellIdEUTRA", nullptr},
    {"physCellIdUTRA-FDD-r16", nullptr}};
const asn1::table::choice_desc cells_triggered_list_item_c_::desc = {
    asn1::table::enum_int_type<cells_triggered_list_item_c_::types>(),
    cells_triggered_list_item_c_::types::nof_types,
    cells_triggered_list_item_c_::types::nof_exts,
    cells_triggered_list_item_c_::types::has_ext,
    false,
    choice_table_cells_triggered_list_item_c__alternatives,
    nullptr,
    choice_table_cells_triggered_list_item_c__names,
    "cells_triggered_list_item_c_",
    offsetof(cells_triggered_list_item_c_, c.type),
    offsetof(cells_triggered_list_item_c_, c.value),
    false,
    &cells_triggered_list_item_c_::storage_desc};

static const char*           enum_names_cells_triggered_list_item_c__types_values[] = {"physCellId",
                                                                                       "physCellIdEUTRA",
                                                                                       "physCellIdUTRA-FDD-r16"};
const asn1::table::enum_desc cells_triggered_list_item_c_::types_opts::desc         = {
    enum_names_cells_triggered_list_item_c__types_values,
    3,
    "cells_triggered_list_item_c_::types"};

// SNPN-Identity-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sn_pn_id_r18_s_fields[] = {
    {offsetof(sn_pn_id_r18_s, plmn_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &plmn_id_s::desc},
    {offsetof(sn_pn_id_r18_s, nid_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sn_pn_id_r18_s::nid_r18), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_sn_pn_id_r18_s_names[] = {{"plmn-Identity-r18", nullptr},
                                                                         {"nid-r18", nullptr}};
const asn1::table::seq_desc          sn_pn_id_r18_s::desc             = {
    {seq_table_sn_pn_id_r18_s_fields, uint64_t{0x3}, nullptr, seq_table_sn_pn_id_r18_s_names, 2, 0},
    false,
    false,
    nullptr,
    0};

// VarAppLayerIdle-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_app_layer_idle_r18_s_fields[] = {
    {offsetof(var_app_layer_idle_r18_s, meas_cfg_app_layer_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr},
    {offsetof(var_app_layer_idle_r18_s, service_type_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(var_app_layer_idle_r18_s::service_type_r18)>(),
     decltype(var_app_layer_idle_r18_s::service_type_r18)::has_ext,
     false,
     decltype(var_app_layer_idle_r18_s::service_type_r18)::nof_types,
     decltype(var_app_layer_idle_r18_s::service_type_r18)::nof_exts,
     nullptr,
     &var_app_layer_idle_r18_s::service_type_r18_opts::desc},
    {offsetof(var_app_layer_idle_r18_s, app_layer_idle_inactive_cfg_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &app_layer_idle_inactive_cfg_r18_s::desc},
    {offsetof(var_app_layer_idle_r18_s, app_layer_meas_prio_r18),
     (int32_t)offsetof(var_app_layer_idle_r18_s, app_layer_meas_prio_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{15},
     nullptr}};
static const asn1::table::field_name seq_table_var_app_layer_idle_r18_s_names[] = {
    {"measConfigAppLayerId-r18", nullptr},
    {"serviceType-r18", nullptr},
    {"appLayerIdleInactiveConfig-r18", nullptr},
    {"appLayerMeasPriority-r18", nullptr}};
static const asn1::table::seq_opt seq_table_var_app_layer_idle_r18_s_opts[] = {
    {3, (int32_t)offsetof(var_app_layer_idle_r18_s, app_layer_meas_prio_r18_present)}};
const asn1::table::seq_desc var_app_layer_idle_r18_s::desc = {{seq_table_var_app_layer_idle_r18_s_fields,
                                                               uint64_t{0x7},
                                                               seq_table_var_app_layer_idle_r18_s_opts,
                                                               seq_table_var_app_layer_idle_r18_s_names,
                                                               4,
                                                               1},
                                                              false,
                                                              false,
                                                              nullptr,
                                                              0};

static const char* enum_names_var_app_layer_idle_r18_s_service_type_r18_e__values[] =
    {"streaming", "mtsi", "vr", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc var_app_layer_idle_r18_s::service_type_r18_opts::desc = {
    enum_names_var_app_layer_idle_r18_s_service_type_r18_e__values,
    8,
    "var_app_layer_idle_r18_s::service_type_r18_e_"};

// VarAppLayerIdleConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_app_layer_idle_cfg_r18_s_fields[] = {
    {offsetof(var_app_layer_idle_cfg_r18_s, app_layer_idle_cfg_list_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::item_list_codec<decltype(var_app_layer_idle_cfg_r18_s::app_layer_idle_cfg_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &var_app_layer_idle_r18_s::desc}};
static const asn1::table::field_name seq_table_var_app_layer_idle_cfg_r18_s_names[] = {
    {"appLayerIdleConfigList-r18", nullptr}};
const asn1::table::seq_desc var_app_layer_idle_cfg_r18_s::desc = {{seq_table_var_app_layer_idle_cfg_r18_s_fields,
                                                                   uint64_t{0x1},
                                                                   nullptr,
                                                                   seq_table_var_app_layer_idle_cfg_r18_s_names,
                                                                   1,
                                                                   0},
                                                                  false,
                                                                  false,
                                                                  nullptr,
                                                                  0};

// VarAppLayerPLMN-List-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_app_layer_plmn_list_r18_s_fields[] = {
    {offsetof(var_app_layer_plmn_list_r18_s, meas_cfg_app_layer_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr},
    {offsetof(var_app_layer_plmn_list_r18_s, plmn_id_list_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::item_list_codec<decltype(var_app_layer_plmn_list_r18_s::plmn_id_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &plmn_id_s::desc}};
static const asn1::table::field_name seq_table_var_app_layer_plmn_list_r18_s_names[] = {
    {"measConfigAppLayerId-r18", nullptr},
    {"plmn-IdentityList-r18", nullptr}};
const asn1::table::seq_desc var_app_layer_plmn_list_r18_s::desc = {{seq_table_var_app_layer_plmn_list_r18_s_fields,
                                                                    uint64_t{0x3},
                                                                    nullptr,
                                                                    seq_table_var_app_layer_plmn_list_r18_s_names,
                                                                    2,
                                                                    0},
                                                                   false,
                                                                   false,
                                                                   nullptr,
                                                                   0};

// VarAppLayerPLMN-ListConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_app_layer_plmn_list_cfg_r18_s_fields[] = {
    {offsetof(var_app_layer_plmn_list_cfg_r18_s, plmn_cfg_list_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::item_list_codec<decltype(var_app_layer_plmn_list_cfg_r18_s::plmn_cfg_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &var_app_layer_plmn_list_r18_s::desc}};
static const asn1::table::field_name seq_table_var_app_layer_plmn_list_cfg_r18_s_names[] = {
    {"plmnConfigList-r18", nullptr}};
const asn1::table::seq_desc var_app_layer_plmn_list_cfg_r18_s::desc = {
    {seq_table_var_app_layer_plmn_list_cfg_r18_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_var_app_layer_plmn_list_cfg_r18_s_names,
     1,
     0},
    false,
    false,
    nullptr,
    0};

// VarConditionalReconfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_conditional_recfg_s_fields[] = {
    {offsetof(var_conditional_recfg_s, cond_recfg_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(var_conditional_recfg_s::cond_recfg_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cond_recfg_to_add_mod_r16_s::desc},
    {offsetof(var_conditional_recfg_s, scpac_ref_cfg_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(var_conditional_recfg_s::scpac_ref_cfg_r18),
                                      asn1::table::string_json>::value},
    {offsetof(var_conditional_recfg_s, sk_counter_cfg_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(var_conditional_recfg_s::sk_counter_cfg_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sk_counter_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_var_conditional_recfg_s_names[] = {
    {"condReconfigList", nullptr},
    {"scpac-ReferenceConfiguration-r18", nullptr},
    {"sk-CounterConfiguration-r18", nullptr}};
static const asn1::table::seq_opt seq_table_var_conditional_recfg_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc var_conditional_recfg_s::desc = {{seq_table_var_conditional_recfg_s_fields,
                                                              uint64_t{0x0},
                                                              seq_table_var_conditional_recfg_s_opts,
                                                              seq_table_var_conditional_recfg_s_names,
                                                              3,
                                                              3},
                                                             false,
                                                             false,
                                                             nullptr,
                                                             0};

// VarConnEstFailReport-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_conn_est_fail_report_r16_s_fields[] = {
    {offsetof(var_conn_est_fail_report_r16_s, conn_est_fail_report_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &conn_est_fail_report_r16_s::desc},
    {offsetof(var_conn_est_fail_report_r16_s, network_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &var_conn_est_fail_report_r16_s::network_id_r18_c_::desc}};
static const asn1::table::field_name seq_table_var_conn_est_fail_report_r16_s_names[] = {
    {"connEstFailReport-r16", nullptr},
    {"networkIdentity-r18", nullptr}};
const asn1::table::seq_desc var_conn_est_fail_report_r16_s::desc = {{seq_table_var_conn_est_fail_report_r16_s_fields,
                                                                     uint64_t{0x3},
                                                                     nullptr,
                                                                     seq_table_var_conn_est_fail_report_r16_s_names,
                                                                     2,
                                                                     0},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0};

static const asn1::table::storage_ops* const
    choice_storage_var_conn_est_fail_report_r16_s_network_id_r18_c__alternatives[] = {
        &asn1::table::alternative_ops<plmn_id_s, false>::value,
        &asn1::table::alternative_ops<sn_pn_id_r18_s, false>::value};
const asn1::table::choice_storage_desc var_conn_est_fail_report_r16_s::network_id_r18_c_::storage_desc = {
    choice_storage_var_conn_est_fail_report_r16_s_network_id_r18_c__alternatives,
    2,
    "var_conn_est_fail_report_r16_s::network_id_r18_c_"};

plmn_id_s& var_conn_est_fail_report_r16_s::network_id_r18_c_::set_plmn_id_r18()
{
  set(types::plmn_id_r18);
  return c.get<plmn_id_s>();
}
sn_pn_id_r18_s& var_conn_est_fail_report_r16_s::network_id_r18_c_::set_snpn_id_r18()
{
  set(types::snpn_id_r18);
  return c.get<sn_pn_id_r18_s>();
}
static const asn1::table::seq_field choice_table_var_conn_est_fail_report_r16_s_network_id_r18_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &plmn_id_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sn_pn_id_r18_s::desc}};
static const asn1::table::field_name choice_table_var_conn_est_fail_report_r16_s_network_id_r18_c__names[] = {
    {"plmn-Identity-r18", nullptr},
    {"snpn-Identity-r18", nullptr}};
const asn1::table::choice_desc var_conn_est_fail_report_r16_s::network_id_r18_c_::desc = {
    asn1::table::enum_int_type<var_conn_est_fail_report_r16_s::network_id_r18_c_::types>(),
    var_conn_est_fail_report_r16_s::network_id_r18_c_::types::nof_types,
    var_conn_est_fail_report_r16_s::network_id_r18_c_::types::nof_exts,
    var_conn_est_fail_report_r16_s::network_id_r18_c_::types::has_ext,
    false,
    choice_table_var_conn_est_fail_report_r16_s_network_id_r18_c__alternatives,
    nullptr,
    choice_table_var_conn_est_fail_report_r16_s_network_id_r18_c__names,
    "var_conn_est_fail_report_r16_s::network_id_r18_c_",
    offsetof(var_conn_est_fail_report_r16_s::network_id_r18_c_, c.type),
    offsetof(var_conn_est_fail_report_r16_s::network_id_r18_c_, c.value),
    false,
    &var_conn_est_fail_report_r16_s::network_id_r18_c_::storage_desc};

static const char* enum_names_var_conn_est_fail_report_r16_s_network_id_r18_c__types_values[]    = {"plmn-Identity-r18",
                                                                                                    "snpn-Identity-r18"};
const asn1::table::enum_desc var_conn_est_fail_report_r16_s::network_id_r18_c_::types_opts::desc = {
    enum_names_var_conn_est_fail_report_r16_s_network_id_r18_c__types_values,
    2,
    "var_conn_est_fail_report_r16_s::network_id_r18_c_::types"};

// VarConnEstFailReportList-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_conn_est_fail_report_list_r17_s_fields[] = {
    {offsetof(var_conn_est_fail_report_list_r17_s, conn_est_fail_report_list_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     3,
     &asn1::table::item_list_codec<decltype(var_conn_est_fail_report_list_r17_s::conn_est_fail_report_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &var_conn_est_fail_report_r16_s::desc}};
static const asn1::table::field_name seq_table_var_conn_est_fail_report_list_r17_s_names[] = {
    {"connEstFailReportList-r17", nullptr}};
const asn1::table::seq_desc var_conn_est_fail_report_list_r17_s::desc = {
    {seq_table_var_conn_est_fail_report_list_r17_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_var_conn_est_fail_report_list_r17_s_names,
     1,
     0},
    false,
    false,
    nullptr,
    0};

// VarEnhMeasIdleConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_enh_meas_idle_cfg_r18_s_fields[] = {
    {offsetof(var_enh_meas_idle_cfg_r18_s, meas_idle_validity_dur_r18),
     (int32_t)offsetof(var_enh_meas_idle_cfg_r18_s, meas_idle_validity_dur_r18_present),
     asn1::table::enum_op<decltype(var_enh_meas_idle_cfg_r18_s::meas_idle_validity_dur_r18)>(),
     decltype(var_enh_meas_idle_cfg_r18_s::meas_idle_validity_dur_r18)::has_ext,
     false,
     decltype(var_enh_meas_idle_cfg_r18_s::meas_idle_validity_dur_r18)::nof_types,
     decltype(var_enh_meas_idle_cfg_r18_s::meas_idle_validity_dur_r18)::nof_exts,
     nullptr,
     &meas_validity_dur_r18_opts::desc}};
static const asn1::table::field_name seq_table_var_enh_meas_idle_cfg_r18_s_names[] = {
    {"measIdleValidityDuration-r18", nullptr}};
static const asn1::table::seq_opt seq_table_var_enh_meas_idle_cfg_r18_s_opts[] = {
    {0, (int32_t)offsetof(var_enh_meas_idle_cfg_r18_s, meas_idle_validity_dur_r18_present)}};
const asn1::table::seq_desc var_enh_meas_idle_cfg_r18_s::desc = {{seq_table_var_enh_meas_idle_cfg_r18_s_fields,
                                                                  uint64_t{0x0},
                                                                  seq_table_var_enh_meas_idle_cfg_r18_s_opts,
                                                                  seq_table_var_enh_meas_idle_cfg_r18_s_names,
                                                                  1,
                                                                  1},
                                                                 false,
                                                                 false,
                                                                 nullptr,
                                                                 0};

// VarLTM-ServingCellNoResetID-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_ltm_serving_cell_no_reset_id_r18_s_fields[] = {
    {offsetof(var_ltm_serving_cell_no_reset_id_r18_s, ltm_serving_cell_no_reset_id_r18),
     (int32_t)offsetof(var_ltm_serving_cell_no_reset_id_r18_s, ltm_serving_cell_no_reset_id_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{8},
     nullptr}};
static const asn1::table::field_name seq_table_var_ltm_serving_cell_no_reset_id_r18_s_names[] = {
    {"ltm-ServingCellNoResetID-r18", nullptr}};
static const asn1::table::seq_opt seq_table_var_ltm_serving_cell_no_reset_id_r18_s_opts[] = {
    {0, (int32_t)offsetof(var_ltm_serving_cell_no_reset_id_r18_s, ltm_serving_cell_no_reset_id_r18_present)}};
const asn1::table::seq_desc var_ltm_serving_cell_no_reset_id_r18_s::desc = {
    {seq_table_var_ltm_serving_cell_no_reset_id_r18_s_fields,
     uint64_t{0x0},
     seq_table_var_ltm_serving_cell_no_reset_id_r18_s_opts,
     seq_table_var_ltm_serving_cell_no_reset_id_r18_s_names,
     1,
     1},
    false,
    false,
    nullptr,
    0};

// VarLTM-ServingCellUE-MeasuredTA-ID-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_ltm_serving_cell_ue_measured_ta_id_r18_s_fields[] = {
    {offsetof(var_ltm_serving_cell_ue_measured_ta_id_r18_s, ltm_serving_cell_ue_measured_ta_id_r18),
     (int32_t)offsetof(var_ltm_serving_cell_ue_measured_ta_id_r18_s, ltm_serving_cell_ue_measured_ta_id_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{8},
     nullptr}};
static const asn1::table::field_name seq_table_var_ltm_serving_cell_ue_measured_ta_id_r18_s_names[] = {
    {"ltm-ServingCellUE-MeasuredTA-ID-r18", nullptr}};
static const asn1::table::seq_opt seq_table_var_ltm_serving_cell_ue_measured_ta_id_r18_s_opts[] = {
    {0,
     (int32_t)offsetof(var_ltm_serving_cell_ue_measured_ta_id_r18_s, ltm_serving_cell_ue_measured_ta_id_r18_present)}};
const asn1::table::seq_desc var_ltm_serving_cell_ue_measured_ta_id_r18_s::desc = {
    {seq_table_var_ltm_serving_cell_ue_measured_ta_id_r18_s_fields,
     uint64_t{0x0},
     seq_table_var_ltm_serving_cell_ue_measured_ta_id_r18_s_opts,
     seq_table_var_ltm_serving_cell_ue_measured_ta_id_r18_s_names,
     1,
     1},
    false,
    false,
    nullptr,
    0};

// VarLogMeasConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_log_meas_cfg_r16_s_fields[] = {
    {offsetof(var_log_meas_cfg_r16_s, area_cfg_r16),
     (int32_t)offsetof(var_log_meas_cfg_r16_s, area_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &area_cfg_r16_s::desc},
    {offsetof(var_log_meas_cfg_r16_s, bt_name_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     3,
     &asn1::table::list_codec<decltype(var_log_meas_cfg_r16_s::bt_name_list_r16), asn1::table::string_json>::value},
    {offsetof(var_log_meas_cfg_r16_s, wlan_name_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     3,
     &asn1::table::list_codec<decltype(var_log_meas_cfg_r16_s::wlan_name_list_r16), asn1::table::string_json>::value},
    {offsetof(var_log_meas_cfg_r16_s, sensor_name_list_r16),
     (int32_t)offsetof(var_log_meas_cfg_r16_s, sensor_name_list_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sensor_name_list_r16_s::desc},
    {offsetof(var_log_meas_cfg_r16_s, logging_dur_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(var_log_meas_cfg_r16_s::logging_dur_r16)>(),
     decltype(var_log_meas_cfg_r16_s::logging_dur_r16)::has_ext,
     false,
     decltype(var_log_meas_cfg_r16_s::logging_dur_r16)::nof_types,
     decltype(var_log_meas_cfg_r16_s::logging_dur_r16)::nof_exts,
     nullptr,
     &logging_dur_r16_opts::desc},
    {offsetof(var_log_meas_cfg_r16_s, report_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &var_log_meas_cfg_r16_s::report_type_c_::desc},
    {0,
     (int32_t)offsetof(var_log_meas_cfg_r16_s, early_meas_ind_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(var_log_meas_cfg_r16_s, area_cfg_r17),
     (int32_t)offsetof(var_log_meas_cfg_r16_s, area_cfg_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &area_cfg_r17_s::desc},
    {offsetof(var_log_meas_cfg_r16_s, area_cfg_v1800),
     (int32_t)offsetof(var_log_meas_cfg_r16_s, area_cfg_v1800_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &area_cfg_v1800_c::desc}};
static const asn1::table::field_name seq_table_var_log_meas_cfg_r16_s_names[] = {{"areaConfiguration-r16", nullptr},
                                                                                 {"bt-NameList-r16", nullptr},
                                                                                 {"wlan-NameList-r16", nullptr},
                                                                                 {"sensor-NameList-r16", nullptr},
                                                                                 {"loggingDuration-r16", nullptr},
                                                                                 {"reportType", nullptr},
                                                                                 {"earlyMeasIndication-r17", "true"},
                                                                                 {"areaConfiguration-r17", nullptr},
                                                                                 {"areaConfiguration-v1800", nullptr}};
static const asn1::table::seq_opt    seq_table_var_log_meas_cfg_r16_s_opts[]  = {
    {0, (int32_t)offsetof(var_log_meas_cfg_r16_s, area_cfg_r16_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(var_log_meas_cfg_r16_s, sensor_name_list_r16_present)},
    {6, (int32_t)offsetof(var_log_meas_cfg_r16_s, early_meas_ind_r17_present)},
    {7, (int32_t)offsetof(var_log_meas_cfg_r16_s, area_cfg_r17_present)},
    {8, (int32_t)offsetof(var_log_meas_cfg_r16_s, area_cfg_v1800_present)}};
const asn1::table::seq_desc var_log_meas_cfg_r16_s::desc = {{seq_table_var_log_meas_cfg_r16_s_fields,
                                                             uint64_t{0x30},
                                                             seq_table_var_log_meas_cfg_r16_s_opts,
                                                             seq_table_var_log_meas_cfg_r16_s_names,
                                                             9,
                                                             7},
                                                            false,
                                                            false,
                                                            nullptr,
                                                            0};

static const asn1::table::storage_ops* const choice_storage_var_log_meas_cfg_r16_s_report_type_c__alternatives[] = {
    &asn1::table::alternative_ops<logged_periodical_report_cfg_r16_s, false>::value,
    &asn1::table::alternative_ops<logged_event_trigger_cfg_r16_s, false>::value};
const asn1::table::choice_storage_desc var_log_meas_cfg_r16_s::report_type_c_::storage_desc = {
    choice_storage_var_log_meas_cfg_r16_s_report_type_c__alternatives,
    2,
    "var_log_meas_cfg_r16_s::report_type_c_"};

logged_periodical_report_cfg_r16_s& var_log_meas_cfg_r16_s::report_type_c_::set_periodical()
{
  set(types::periodical);
  return c.get<logged_periodical_report_cfg_r16_s>();
}
logged_event_trigger_cfg_r16_s& var_log_meas_cfg_r16_s::report_type_c_::set_event_triggered()
{
  set(types::event_triggered);
  return c.get<logged_event_trigger_cfg_r16_s>();
}
static const asn1::table::seq_field choice_table_var_log_meas_cfg_r16_s_report_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &logged_periodical_report_cfg_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &logged_event_trigger_cfg_r16_s::desc}};
static const asn1::table::field_name choice_table_var_log_meas_cfg_r16_s_report_type_c__names[] = {
    {"periodical", nullptr},
    {"eventTriggered", nullptr}};
const asn1::table::choice_desc var_log_meas_cfg_r16_s::report_type_c_::desc = {
    asn1::table::enum_int_type<var_log_meas_cfg_r16_s::report_type_c_::types>(),
    var_log_meas_cfg_r16_s::report_type_c_::types::nof_types,
    var_log_meas_cfg_r16_s::report_type_c_::types::nof_exts,
    var_log_meas_cfg_r16_s::report_type_c_::types::has_ext,
    false,
    choice_table_var_log_meas_cfg_r16_s_report_type_c__alternatives,
    nullptr,
    choice_table_var_log_meas_cfg_r16_s_report_type_c__names,
    "var_log_meas_cfg_r16_s::report_type_c_",
    offsetof(var_log_meas_cfg_r16_s::report_type_c_, c.type),
    offsetof(var_log_meas_cfg_r16_s::report_type_c_, c.value),
    false,
    &var_log_meas_cfg_r16_s::report_type_c_::storage_desc};

static const char* enum_names_var_log_meas_cfg_r16_s_report_type_c__types_values[] = {"periodical", "eventTriggered"};
const asn1::table::enum_desc var_log_meas_cfg_r16_s::report_type_c_::types_opts::desc = {
    enum_names_var_log_meas_cfg_r16_s_report_type_c__types_values,
    2,
    "var_log_meas_cfg_r16_s::report_type_c_::types"};

// VarLogMeasReport-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_log_meas_report_r16_s_fields[] = {
    {offsetof(var_log_meas_report_r16_s, absolute_time_info_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(var_log_meas_report_r16_s::absolute_time_info_r16),
                                asn1::table::struct_json>::value},
    {offsetof(var_log_meas_report_r16_s, trace_ref_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trace_ref_r16_s::desc},
    {offsetof(var_log_meas_report_r16_s, trace_recording_session_ref_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(var_log_meas_report_r16_s::trace_recording_session_ref_r16),
                                asn1::table::string_json>::value},
    {offsetof(var_log_meas_report_r16_s, tce_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(var_log_meas_report_r16_s::tce_id_r16), asn1::table::string_json>::value},
    {offsetof(var_log_meas_report_r16_s, log_meas_info_list_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     519,
     &asn1::table::item_list_codec<decltype(var_log_meas_report_r16_s::log_meas_info_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &log_meas_info_r16_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {offsetof(var_log_meas_report_r16_s, id_list_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &var_log_meas_report_r16_s::id_list_r18_c_::desc}};
static const asn1::table::field_name seq_table_var_log_meas_report_r16_s_names[] = {
    {"absoluteTimeInfo-r16", nullptr},
    {"traceReference-r16", nullptr},
    {"traceRecordingSessionRef-r16", nullptr},
    {"tce-Id-r16", nullptr},
    {"logMeasInfoList-r16", nullptr},
    {"sigLoggedMeasType-r17", "true"},
    {"identityList-r18", nullptr}};
const asn1::table::seq_desc var_log_meas_report_r16_s::desc = {{seq_table_var_log_meas_report_r16_s_fields,
                                                                uint64_t{0x7f},
                                                                nullptr,
                                                                seq_table_var_log_meas_report_r16_s_names,
                                                                7,
                                                                0},
                                                               false,
                                                               false,
                                                               nullptr,
                                                               0};

static const asn1::table::storage_ops* const choice_storage_var_log_meas_report_r16_s_id_list_r18_c__alternatives[] = {
    &asn1::table::alternative_ops<plmn_id_list2_r16_l, false>::value,
    &asn1::table::alternative_ops<sn_pn_cfg_id_list_r18_l, false>::value};
const asn1::table::choice_storage_desc var_log_meas_report_r16_s::id_list_r18_c_::storage_desc = {
    choice_storage_var_log_meas_report_r16_s_id_list_r18_c__alternatives,
    2,
    "var_log_meas_report_r16_s::id_list_r18_c_"};

plmn_id_list2_r16_l& var_log_meas_report_r16_s::id_list_r18_c_::set_plmn_id_list_r18()
{
  set(types::plmn_id_list_r18);
  return c.get<plmn_id_list2_r16_l>();
}
sn_pn_cfg_id_list_r18_l& var_log_meas_report_r16_s::id_list_r18_c_::set_snpn_cfg_id_list_r18()
{
  set(types::snpn_cfg_id_list_r18);
  return c.get<sn_pn_cfg_id_list_r18_l>();
}
static const asn1::table::seq_field choice_table_var_log_meas_report_r16_s_id_list_r18_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::list_codec<plmn_id_list2_r16_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::list_codec<sn_pn_cfg_id_list_r18_l, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_var_log_meas_report_r16_s_id_list_r18_c__names[] = {
    {"plmn-IdentityList-r18", nullptr},
    {"snpn-ConfigID-List-r18", nullptr}};
const asn1::table::choice_desc var_log_meas_report_r16_s::id_list_r18_c_::desc = {
    asn1::table::enum_int_type<var_log_meas_report_r16_s::id_list_r18_c_::types>(),
    var_log_meas_report_r16_s::id_list_r18_c_::types::nof_types,
    var_log_meas_report_r16_s::id_list_r18_c_::types::nof_exts,
    var_log_meas_report_r16_s::id_list_r18_c_::types::has_ext,
    false,
    choice_table_var_log_meas_report_r16_s_id_list_r18_c__alternatives,
    nullptr,
    choice_table_var_log_meas_report_r16_s_id_list_r18_c__names,
    "var_log_meas_report_r16_s::id_list_r18_c_",
    offsetof(var_log_meas_report_r16_s::id_list_r18_c_, c.type),
    offsetof(var_log_meas_report_r16_s::id_list_r18_c_, c.value),
    false,
    &var_log_meas_report_r16_s::id_list_r18_c_::storage_desc};

static const char* enum_names_var_log_meas_report_r16_s_id_list_r18_c__types_values[]    = {"plmn-IdentityList-r18",
                                                                                            "snpn-ConfigID-List-r18"};
const asn1::table::enum_desc var_log_meas_report_r16_s::id_list_r18_c_::types_opts::desc = {
    enum_names_var_log_meas_report_r16_s_id_list_r18_c__types_values,
    2,
    "var_log_meas_report_r16_s::id_list_r18_c_::types"};

// VarMeasConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_meas_cfg_s_fields[] = {
    {offsetof(var_meas_cfg_s, meas_id_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::
         item_list_codec<decltype(var_meas_cfg_s::meas_id_list), asn1::table::sequence_item_ops, false, true>::value,
     &meas_id_to_add_mod_s::desc},
    {offsetof(var_meas_cfg_s, meas_obj_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::
         item_list_codec<decltype(var_meas_cfg_s::meas_obj_list), asn1::table::sequence_item_ops, false, true>::value,
     &meas_obj_to_add_mod_s::desc},
    {offsetof(var_meas_cfg_s, report_cfg_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::
         item_list_codec<decltype(var_meas_cfg_s::report_cfg_list), asn1::table::sequence_item_ops, false, true>::value,
     &report_cfg_to_add_mod_s::desc},
    {offsetof(var_meas_cfg_s, quant_cfg),
     (int32_t)offsetof(var_meas_cfg_s, quant_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &quant_cfg_s::desc},
    {offsetof(var_meas_cfg_s, s_measure_cfg),
     (int32_t)offsetof(var_meas_cfg_s, s_measure_cfg_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &var_meas_cfg_s::s_measure_cfg_c_::desc}};
static const asn1::table::field_name seq_table_var_meas_cfg_s_names[] = {{"measIdList", nullptr},
                                                                         {"measObjectList", nullptr},
                                                                         {"reportConfigList", nullptr},
                                                                         {"quantityConfig", nullptr},
                                                                         {"s-MeasureConfig", nullptr}};
static const asn1::table::seq_opt    seq_table_var_meas_cfg_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(var_meas_cfg_s, quant_cfg_present)},
    {4, (int32_t)offsetof(var_meas_cfg_s, s_measure_cfg_present)}};
const asn1::table::seq_desc var_meas_cfg_s::desc = {{seq_table_var_meas_cfg_s_fields,
                                                     uint64_t{0x0},
                                                     seq_table_var_meas_cfg_s_opts,
                                                     seq_table_var_meas_cfg_s_names,
                                                     5,
                                                     5},
                                                    false,
                                                    false,
                                                    nullptr,
                                                    0};

static const asn1::table::storage_ops* const choice_storage_var_meas_cfg_s_s_measure_cfg_c__alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc var_meas_cfg_s::s_measure_cfg_c_::storage_desc = {
    choice_storage_var_meas_cfg_s_s_measure_cfg_c__alternatives,
    2,
    "var_meas_cfg_s::s_measure_cfg_c_"};

uint8_t& var_meas_cfg_s::s_measure_cfg_c_::set_ssb_rsrp()
{
  set(types::ssb_rsrp);
  return c.get<uint8_t>();
}
uint8_t& var_meas_cfg_s::s_measure_cfg_c_::set_csi_rsrp()
{
  set(types::csi_rsrp);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field choice_table_var_meas_cfg_s_s_measure_cfg_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{127},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{127},
     nullptr}};
static const asn1::table::field_name choice_table_var_meas_cfg_s_s_measure_cfg_c__names[] = {{"ssb-RSRP", nullptr},
                                                                                             {"csi-RSRP", nullptr}};
const asn1::table::choice_desc       var_meas_cfg_s::s_measure_cfg_c_::desc               = {
    asn1::table::enum_int_type<var_meas_cfg_s::s_measure_cfg_c_::types>(),
    var_meas_cfg_s::s_measure_cfg_c_::types::nof_types,
    var_meas_cfg_s::s_measure_cfg_c_::types::nof_exts,
    var_meas_cfg_s::s_measure_cfg_c_::types::has_ext,
    false,
    choice_table_var_meas_cfg_s_s_measure_cfg_c__alternatives,
    nullptr,
    choice_table_var_meas_cfg_s_s_measure_cfg_c__names,
    "var_meas_cfg_s::s_measure_cfg_c_",
    offsetof(var_meas_cfg_s::s_measure_cfg_c_, c.type),
    offsetof(var_meas_cfg_s::s_measure_cfg_c_, c.value),
    false,
    &var_meas_cfg_s::s_measure_cfg_c_::storage_desc};

static const char*           enum_names_var_meas_cfg_s_s_measure_cfg_c__types_values[] = {"ssb-RSRP", "csi-RSRP"};
const asn1::table::enum_desc var_meas_cfg_s::s_measure_cfg_c_::types_opts::desc        = {
    enum_names_var_meas_cfg_s_s_measure_cfg_c__types_values,
    2,
    "var_meas_cfg_s::s_measure_cfg_c_::types"};

// VarMeasConfigSL-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_meas_cfg_sl_r16_s_fields[] = {
    {offsetof(var_meas_cfg_sl_r16_s, sl_meas_id_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::item_list_codec<decltype(var_meas_cfg_sl_r16_s::sl_meas_id_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_meas_id_info_r16_s::desc},
    {offsetof(var_meas_cfg_sl_r16_s, sl_meas_obj_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::item_list_codec<decltype(var_meas_cfg_sl_r16_s::sl_meas_obj_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_meas_obj_info_r16_s::desc},
    {offsetof(var_meas_cfg_sl_r16_s, sl_report_cfg_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::item_list_codec<decltype(var_meas_cfg_sl_r16_s::sl_report_cfg_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_report_cfg_info_r16_s::desc},
    {offsetof(var_meas_cfg_sl_r16_s, sl_quant_cfg_r16),
     (int32_t)offsetof(var_meas_cfg_sl_r16_s, sl_quant_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_quant_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_var_meas_cfg_sl_r16_s_names[] = {{"sl-MeasIdList-r16", nullptr},
                                                                                {"sl-MeasObjectList-r16", nullptr},
                                                                                {"sl-ReportConfigList-r16", nullptr},
                                                                                {"sl-QuantityConfig-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_var_meas_cfg_sl_r16_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(var_meas_cfg_sl_r16_s, sl_quant_cfg_r16_present)}};
const asn1::table::seq_desc var_meas_cfg_sl_r16_s::desc = {{seq_table_var_meas_cfg_sl_r16_s_fields,
                                                            uint64_t{0x0},
                                                            seq_table_var_meas_cfg_sl_r16_s_opts,
                                                            seq_table_var_meas_cfg_sl_r16_s_names,
                                                            4,
                                                            4},
                                                           false,
                                                           false,
                                                           nullptr,
                                                           0};

// VarMeasIdleConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_meas_idle_cfg_r16_s_fields[] = {
    {offsetof(var_meas_idle_cfg_r16_s, meas_idle_carrier_list_nr_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(var_meas_idle_cfg_r16_s::meas_idle_carrier_list_nr_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &meas_idle_carrier_nr_r16_s::desc},
    {offsetof(var_meas_idle_cfg_r16_s, meas_idle_carrier_list_eutra_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(var_meas_idle_cfg_r16_s::meas_idle_carrier_list_eutra_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &meas_idle_carrier_eutra_r16_s::desc},
    {offsetof(var_meas_idle_cfg_r16_s, meas_idle_dur_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(var_meas_idle_cfg_r16_s::meas_idle_dur_r16)>(),
     decltype(var_meas_idle_cfg_r16_s::meas_idle_dur_r16)::has_ext,
     false,
     decltype(var_meas_idle_cfg_r16_s::meas_idle_dur_r16)::nof_types,
     decltype(var_meas_idle_cfg_r16_s::meas_idle_dur_r16)::nof_exts,
     nullptr,
     &var_meas_idle_cfg_r16_s::meas_idle_dur_r16_opts::desc},
    {offsetof(var_meas_idle_cfg_r16_s, validity_area_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(var_meas_idle_cfg_r16_s::validity_area_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &validity_area_r16_s::desc}};
static const asn1::table::field_name seq_table_var_meas_idle_cfg_r16_s_names[] = {
    {"measIdleCarrierListNR-r16", nullptr},
    {"measIdleCarrierListEUTRA-r16", nullptr},
    {"measIdleDuration-r16", nullptr},
    {"validityAreaList-r16", nullptr}};
static const asn1::table::seq_opt seq_table_var_meas_idle_cfg_r16_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc var_meas_idle_cfg_r16_s::desc = {{seq_table_var_meas_idle_cfg_r16_s_fields,
                                                              uint64_t{0x4},
                                                              seq_table_var_meas_idle_cfg_r16_s_opts,
                                                              seq_table_var_meas_idle_cfg_r16_s_names,
                                                              4,
                                                              3},
                                                             false,
                                                             false,
                                                             nullptr,
                                                             0};

static const char* enum_names_var_meas_idle_cfg_r16_s_meas_idle_dur_r16_e__values[] =
    {"sec10", "sec30", "sec60", "sec120", "sec180", "sec240", "sec300", "spare"};
const asn1::table::enum_desc var_meas_idle_cfg_r16_s::meas_idle_dur_r16_opts::desc = {
    enum_names_var_meas_idle_cfg_r16_s_meas_idle_dur_r16_e__values,
    8,
    "var_meas_idle_cfg_r16_s::meas_idle_dur_r16_e_"};
uint16_t var_meas_idle_cfg_r16_s::meas_idle_dur_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {10, 30, 60, 120, 180, 240, 300};
  return map_enum_number(numbers, 7, value, "var_meas_idle_cfg_r16_s::meas_idle_dur_r16_e_");
}

// VarMeasIdleReport-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_meas_idle_report_r16_s_fields[] = {
    {offsetof(var_meas_idle_report_r16_s, meas_report_idle_nr_r16),
     (int32_t)offsetof(var_meas_idle_report_r16_s, meas_report_idle_nr_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_result_idle_nr_r16_s::desc},
    {offsetof(var_meas_idle_report_r16_s, meas_report_idle_eutra_r16),
     (int32_t)offsetof(var_meas_idle_report_r16_s, meas_report_idle_eutra_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_result_idle_eutra_r16_s::desc}};
static const asn1::table::field_name seq_table_var_meas_idle_report_r16_s_names[] = {
    {"measReportIdleNR-r16", nullptr},
    {"measReportIdleEUTRA-r16", nullptr}};
static const asn1::table::seq_opt seq_table_var_meas_idle_report_r16_s_opts[] = {
    {0, (int32_t)offsetof(var_meas_idle_report_r16_s, meas_report_idle_nr_r16_present)},
    {1, (int32_t)offsetof(var_meas_idle_report_r16_s, meas_report_idle_eutra_r16_present)}};
const asn1::table::seq_desc var_meas_idle_report_r16_s::desc = {{seq_table_var_meas_idle_report_r16_s_fields,
                                                                 uint64_t{0x0},
                                                                 seq_table_var_meas_idle_report_r16_s_opts,
                                                                 seq_table_var_meas_idle_report_r16_s_names,
                                                                 2,
                                                                 2},
                                                                false,
                                                                false,
                                                                nullptr,
                                                                0};

// VarMeasReport ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_meas_report_s_fields[] = {
    {offsetof(var_meas_report_s, meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{63},
     nullptr},
    {offsetof(var_meas_report_s, cells_triggered_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::item_list_codec<decltype(var_meas_report_s::cells_triggered_list),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &cells_triggered_list_item_c_::desc},
    {offsetof(var_meas_report_s, nof_reports_sent),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     false,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(var_meas_report_s, cli_triggered_list_r16),
     (int32_t)offsetof(var_meas_report_s, cli_triggered_list_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cli_triggered_list_r16_c::desc},
    {offsetof(var_meas_report_s, tx_pool_meas_to_add_mod_list_nr_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(var_meas_report_s::tx_pool_meas_to_add_mod_list_nr_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{15}, false, false>>::value},
    {offsetof(var_meas_report_s, relays_triggered_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::list_codec<decltype(var_meas_report_s::relays_triggered_list_r17), asn1::table::struct_json>::value},
    {offsetof(var_meas_report_s, cells_met_leaving_cond_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(var_meas_report_s::cells_met_leaving_cond_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{1007}, false, false>>::value},
    {offsetof(var_meas_report_s, reported_best_neighbour_cell_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1,
     &asn1::table::list_codec<decltype(var_meas_report_s::reported_best_neighbour_cell_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{1007}, false, false>>::value}};
static const asn1::table::field_name seq_table_var_meas_report_s_names[] = {{"measId", nullptr},
                                                                            {"cellsTriggeredList", nullptr},
                                                                            {"numberOfReportsSent", nullptr},
                                                                            {"cli-TriggeredList-r16", nullptr},
                                                                            {"tx-PoolMeasToAddModListNR-r16", nullptr},
                                                                            {"relaysTriggeredList-r17", nullptr},
                                                                            {"cellsMetLeavingCond-r18", nullptr},
                                                                            {"reportedBestNeighbourCell-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_var_meas_report_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(var_meas_report_s, cli_triggered_list_r16_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence},
    {6, asn1::table::seq_field::list_presence},
    {7, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc var_meas_report_s::desc = {{seq_table_var_meas_report_s_fields,
                                                        uint64_t{0x5},
                                                        seq_table_var_meas_report_s_opts,
                                                        seq_table_var_meas_report_s_names,
                                                        8,
                                                        6},
                                                       false,
                                                       false,
                                                       nullptr,
                                                       0};

// VarMeasReportSL-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_meas_report_sl_r16_s_fields[] = {
    {offsetof(var_meas_report_sl_r16_s, sl_meas_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{63},
     nullptr},
    {offsetof(var_meas_report_sl_r16_s, sl_freq_triggered_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(var_meas_report_sl_r16_s::sl_freq_triggered_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{3279165}, false, false>>::value},
    {offsetof(var_meas_report_sl_r16_s, sl_nof_reports_sent_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     false,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_var_meas_report_sl_r16_s_names[] = {
    {"sl-MeasId-r16", nullptr},
    {"sl-FrequencyTriggeredList-r16", nullptr},
    {"sl-NumberOfReportsSent-r16", nullptr}};
static const asn1::table::seq_opt seq_table_var_meas_report_sl_r16_s_opts[] = {
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc var_meas_report_sl_r16_s::desc = {{seq_table_var_meas_report_sl_r16_s_fields,
                                                               uint64_t{0x5},
                                                               seq_table_var_meas_report_sl_r16_s_opts,
                                                               seq_table_var_meas_report_sl_r16_s_names,
                                                               3,
                                                               1},
                                                              false,
                                                              false,
                                                              nullptr,
                                                              0};

// VarMeasReselectionConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_meas_resel_cfg_r18_s_fields[] = {
    {offsetof(var_meas_resel_cfg_r18_s, meas_resel_carrier_list_nr_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(var_meas_resel_cfg_r18_s::meas_resel_carrier_list_nr_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &meas_resel_carrier_nr_r18_s::desc},
    {offsetof(var_meas_resel_cfg_r18_s, meas_resel_validity_dur_r18),
     (int32_t)offsetof(var_meas_resel_cfg_r18_s, meas_resel_validity_dur_r18_present),
     asn1::table::enum_op<decltype(var_meas_resel_cfg_r18_s::meas_resel_validity_dur_r18)>(),
     decltype(var_meas_resel_cfg_r18_s::meas_resel_validity_dur_r18)::has_ext,
     false,
     decltype(var_meas_resel_cfg_r18_s::meas_resel_validity_dur_r18)::nof_types,
     decltype(var_meas_resel_cfg_r18_s::meas_resel_validity_dur_r18)::nof_exts,
     nullptr,
     &meas_validity_dur_r18_opts::desc}};
static const asn1::table::field_name seq_table_var_meas_resel_cfg_r18_s_names[] = {
    {"measReselectionCarrierListNR-r18", nullptr},
    {"measReselectionValidityDuration-r18", nullptr}};
static const asn1::table::seq_opt seq_table_var_meas_resel_cfg_r18_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(var_meas_resel_cfg_r18_s, meas_resel_validity_dur_r18_present)}};
const asn1::table::seq_desc var_meas_resel_cfg_r18_s::desc = {{seq_table_var_meas_resel_cfg_r18_s_fields,
                                                               uint64_t{0x0},
                                                               seq_table_var_meas_resel_cfg_r18_s_opts,
                                                               seq_table_var_meas_resel_cfg_r18_s_names,
                                                               2,
                                                               2},
                                                              false,
                                                              false,
                                                              nullptr,
                                                              0};

// VarMobilityHistoryReport-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_mob_history_report_r17_s_fields[] = {
    {offsetof(var_mob_history_report_r17_s, visited_cell_info_list_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::item_list_codec<decltype(var_mob_history_report_r17_s::visited_cell_info_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &visited_cell_info_r16_s::desc},
    {offsetof(var_mob_history_report_r17_s, visited_pscell_info_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::item_list_codec<decltype(var_mob_history_report_r17_s::visited_pscell_info_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &visited_pscell_info_r17_s::desc}};
static const asn1::table::field_name seq_table_var_mob_history_report_r17_s_names[] = {
    {"visitedCellInfoList-r16", nullptr},
    {"visitedPSCellInfoList-r17", nullptr}};
static const asn1::table::seq_opt seq_table_var_mob_history_report_r17_s_opts[] = {
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc var_mob_history_report_r17_s::desc = {{seq_table_var_mob_history_report_r17_s_fields,
                                                                   uint64_t{0x1},
                                                                   seq_table_var_mob_history_report_r17_s_opts,
                                                                   seq_table_var_mob_history_report_r17_s_names,
                                                                   2,
                                                                   1},
                                                                  false,
                                                                  false,
                                                                  nullptr,
                                                                  0};

// VarPendingRNA-Update ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_pending_rna_upd_s_fields[] = {
    {offsetof(var_pending_rna_upd_s, pending_rna_upd),
     (int32_t)offsetof(var_pending_rna_upd_s, pending_rna_upd_present),
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_var_pending_rna_upd_s_names[] = {{"pendingRNA-Update", nullptr}};
static const asn1::table::seq_opt    seq_table_var_pending_rna_upd_s_opts[]  = {
    {0, (int32_t)offsetof(var_pending_rna_upd_s, pending_rna_upd_present)}};
const asn1::table::seq_desc var_pending_rna_upd_s::desc = {{seq_table_var_pending_rna_upd_s_fields,
                                                            uint64_t{0x0},
                                                            seq_table_var_pending_rna_upd_s_opts,
                                                            seq_table_var_pending_rna_upd_s_names,
                                                            1,
                                                            1},
                                                           false,
                                                           false,
                                                           nullptr,
                                                           0};

// VarRA-Report-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_ra_report_r16_s_fields[] = {
    {offsetof(var_ra_report_r16_s, ra_report_list_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(var_ra_report_r16_s::ra_report_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ra_report_r16_s::desc},
    {offsetof(var_ra_report_r16_s, id_list_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &var_ra_report_r16_s::id_list_r18_c_::desc}};
static const asn1::table::field_name seq_table_var_ra_report_r16_s_names[] = {{"ra-ReportList-r16", nullptr},
                                                                              {"identityList-r18", nullptr}};
const asn1::table::seq_desc          var_ra_report_r16_s::desc             = {
    {seq_table_var_ra_report_r16_s_fields, uint64_t{0x3}, nullptr, seq_table_var_ra_report_r16_s_names, 2, 0},
    false,
    false,
    nullptr,
    0};

static const asn1::table::storage_ops* const choice_storage_var_ra_report_r16_s_id_list_r18_c__alternatives[] = {
    &asn1::table::alternative_ops<plmn_id_list2_r16_l, false>::value,
    &asn1::table::alternative_ops<var_ra_report_r16_s::id_list_r18_c_::snpn_id_list_r18_l_, false>::value};
const asn1::table::choice_storage_desc var_ra_report_r16_s::id_list_r18_c_::storage_desc = {
    choice_storage_var_ra_report_r16_s_id_list_r18_c__alternatives,
    2,
    "var_ra_report_r16_s::id_list_r18_c_"};

plmn_id_list2_r16_l& var_ra_report_r16_s::id_list_r18_c_::set_plmn_id_list_r18()
{
  set(types::plmn_id_list_r18);
  return c.get<plmn_id_list2_r16_l>();
}
var_ra_report_r16_s::id_list_r18_c_::snpn_id_list_r18_l_& var_ra_report_r16_s::id_list_r18_c_::set_snpn_id_list_r18()
{
  set(types::snpn_id_list_r18);
  return c.get<snpn_id_list_r18_l_>();
}
static const asn1::table::seq_field choice_table_var_ra_report_r16_s_id_list_r18_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::list_codec<plmn_id_list2_r16_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     11,
     &asn1::table::list_codec<var_ra_report_r16_s::id_list_r18_c_::snpn_id_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_var_ra_report_r16_s_id_list_r18_c__names[] = {
    {"plmn-IdentityList-r18", nullptr},
    {"snpn-IdentityList-r18", nullptr}};
const asn1::table::choice_desc var_ra_report_r16_s::id_list_r18_c_::desc = {
    asn1::table::enum_int_type<var_ra_report_r16_s::id_list_r18_c_::types>(),
    var_ra_report_r16_s::id_list_r18_c_::types::nof_types,
    var_ra_report_r16_s::id_list_r18_c_::types::nof_exts,
    var_ra_report_r16_s::id_list_r18_c_::types::has_ext,
    false,
    choice_table_var_ra_report_r16_s_id_list_r18_c__alternatives,
    nullptr,
    choice_table_var_ra_report_r16_s_id_list_r18_c__names,
    "var_ra_report_r16_s::id_list_r18_c_",
    offsetof(var_ra_report_r16_s::id_list_r18_c_, c.type),
    offsetof(var_ra_report_r16_s::id_list_r18_c_, c.value),
    false,
    &var_ra_report_r16_s::id_list_r18_c_::storage_desc};

static const char*           enum_names_var_ra_report_r16_s_id_list_r18_c__types_values[] = {"plmn-IdentityList-r18",
                                                                                             "snpn-IdentityList-r18"};
const asn1::table::enum_desc var_ra_report_r16_s::id_list_r18_c_::types_opts::desc        = {
    enum_names_var_ra_report_r16_s_id_list_r18_c__types_values,
    2,
    "var_ra_report_r16_s::id_list_r18_c_::types"};

// VarRLF-Report-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_rlf_report_r16_s_fields[] = {
    {offsetof(var_rlf_report_r16_s, rlf_report_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &rlf_report_r16_c::desc},
    {offsetof(var_rlf_report_r16_s, id_list_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &var_rlf_report_r16_s::id_list_r18_c_::desc}};
static const asn1::table::field_name seq_table_var_rlf_report_r16_s_names[] = {{"rlf-Report-r16", nullptr},
                                                                               {"identityList-r18", nullptr}};
const asn1::table::seq_desc          var_rlf_report_r16_s::desc             = {
    {seq_table_var_rlf_report_r16_s_fields, uint64_t{0x3}, nullptr, seq_table_var_rlf_report_r16_s_names, 2, 0},
    false,
    false,
    nullptr,
    0};

static const asn1::table::storage_ops* const choice_storage_var_rlf_report_r16_s_id_list_r18_c__alternatives[] = {
    &asn1::table::alternative_ops<plmn_id_list2_r16_l, false>::value,
    &asn1::table::alternative_ops<var_rlf_report_r16_s::id_list_r18_c_::snpn_id_list_r18_l_, false>::value};
const asn1::table::choice_storage_desc var_rlf_report_r16_s::id_list_r18_c_::storage_desc = {
    choice_storage_var_rlf_report_r16_s_id_list_r18_c__alternatives,
    2,
    "var_rlf_report_r16_s::id_list_r18_c_"};

plmn_id_list2_r16_l& var_rlf_report_r16_s::id_list_r18_c_::set_plmn_id_list_r18()
{
  set(types::plmn_id_list_r18);
  return c.get<plmn_id_list2_r16_l>();
}
var_rlf_report_r16_s::id_list_r18_c_::snpn_id_list_r18_l_& var_rlf_report_r16_s::id_list_r18_c_::set_snpn_id_list_r18()
{
  set(types::snpn_id_list_r18);
  return c.get<snpn_id_list_r18_l_>();
}
static const asn1::table::seq_field choice_table_var_rlf_report_r16_s_id_list_r18_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::list_codec<plmn_id_list2_r16_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     11,
     &asn1::table::list_codec<var_rlf_report_r16_s::id_list_r18_c_::snpn_id_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_var_rlf_report_r16_s_id_list_r18_c__names[] = {
    {"plmn-IdentityList-r18", nullptr},
    {"snpn-IdentityList-r18", nullptr}};
const asn1::table::choice_desc var_rlf_report_r16_s::id_list_r18_c_::desc = {
    asn1::table::enum_int_type<var_rlf_report_r16_s::id_list_r18_c_::types>(),
    var_rlf_report_r16_s::id_list_r18_c_::types::nof_types,
    var_rlf_report_r16_s::id_list_r18_c_::types::nof_exts,
    var_rlf_report_r16_s::id_list_r18_c_::types::has_ext,
    false,
    choice_table_var_rlf_report_r16_s_id_list_r18_c__alternatives,
    nullptr,
    choice_table_var_rlf_report_r16_s_id_list_r18_c__names,
    "var_rlf_report_r16_s::id_list_r18_c_",
    offsetof(var_rlf_report_r16_s::id_list_r18_c_, c.type),
    offsetof(var_rlf_report_r16_s::id_list_r18_c_, c.value),
    false,
    &var_rlf_report_r16_s::id_list_r18_c_::storage_desc};

static const char*           enum_names_var_rlf_report_r16_s_id_list_r18_c__types_values[] = {"plmn-IdentityList-r18",
                                                                                              "snpn-IdentityList-r18"};
const asn1::table::enum_desc var_rlf_report_r16_s::id_list_r18_c_::types_opts::desc        = {
    enum_names_var_rlf_report_r16_s_id_list_r18_c__types_values,
    2,
    "var_rlf_report_r16_s::id_list_r18_c_::types"};

// VarResumeMAC-Input ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_resume_mac_input_s_fields[] = {
    {offsetof(var_resume_mac_input_s, source_pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{1007},
     nullptr},
    {offsetof(var_resume_mac_input_s, target_cell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(var_resume_mac_input_s::target_cell_id), asn1::table::struct_json>::value},
    {offsetof(var_resume_mac_input_s, source_c_rnti),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr}};
static const asn1::table::field_name seq_table_var_resume_mac_input_s_names[] = {{"sourcePhysCellId", nullptr},
                                                                                 {"targetCellIdentity", nullptr},
                                                                                 {"source-c-RNTI", nullptr}};
const asn1::table::seq_desc          var_resume_mac_input_s::desc             = {
    {seq_table_var_resume_mac_input_s_fields, uint64_t{0x7}, nullptr, seq_table_var_resume_mac_input_s_names, 3, 0},
    false,
    false,
    nullptr,
    0};

// VarServingSecurityCellSetID ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_serving_security_cell_set_id_s_fields[] = {
    {offsetof(var_serving_security_cell_set_id_s, serving_security_cell_set_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{8},
     nullptr}};
static const asn1::table::field_name seq_table_var_serving_security_cell_set_id_s_names[] = {
    {"servingSecurityCellSetId-r18", nullptr}};
const asn1::table::seq_desc var_serving_security_cell_set_id_s::desc = {
    {seq_table_var_serving_security_cell_set_id_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_var_serving_security_cell_set_id_s_names,
     1,
     0},
    false,
    false,
    nullptr,
    0};

// VarShortMAC-Input ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_short_mac_input_s_fields[] = {
    {offsetof(var_short_mac_input_s, source_pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{1007},
     nullptr},
    {offsetof(var_short_mac_input_s, target_cell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(var_short_mac_input_s::target_cell_id), asn1::table::struct_json>::value},
    {offsetof(var_short_mac_input_s, source_c_rnti),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr}};
static const asn1::table::field_name seq_table_var_short_mac_input_s_names[] = {{"sourcePhysCellId", nullptr},
                                                                                {"targetCellIdentity", nullptr},
                                                                                {"source-c-RNTI", nullptr}};
const asn1::table::seq_desc          var_short_mac_input_s::desc             = {
    {seq_table_var_short_mac_input_s_fields, uint64_t{0x7}, nullptr, seq_table_var_short_mac_input_s_names, 3, 0},
    false,
    false,
    nullptr,
    0};

// VarSuccessHO-Report-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_success_ho_report_r17_s_fields[] = {
    {offsetof(var_success_ho_report_r17_s, success_ho_report_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &success_ho_report_r17_s::desc},
    {offsetof(var_success_ho_report_r17_s, id_list_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &var_success_ho_report_r17_s::id_list_r18_c_::desc}};
static const asn1::table::field_name seq_table_var_success_ho_report_r17_s_names[] = {{"successHO-Report-r17", nullptr},
                                                                                      {"identityList-r18", nullptr}};
const asn1::table::seq_desc          var_success_ho_report_r17_s::desc = {{seq_table_var_success_ho_report_r17_s_fields,
                                                                           uint64_t{0x3},
                                                                           nullptr,
                                                                           seq_table_var_success_ho_report_r17_s_names,
                                                                           2,
                                                                           0},
                                                                          false,
                                                                          false,
                                                                          nullptr,
                                                                          0};

static const asn1::table::storage_ops* const choice_storage_var_success_ho_report_r17_s_id_list_r18_c__alternatives[] =
    {&asn1::table::alternative_ops<plmn_id_list2_r16_l, false>::value,
     &asn1::table::alternative_ops<var_success_ho_report_r17_s::id_list_r18_c_::snpn_id_list_r18_l_, false>::value};
const asn1::table::choice_storage_desc var_success_ho_report_r17_s::id_list_r18_c_::storage_desc = {
    choice_storage_var_success_ho_report_r17_s_id_list_r18_c__alternatives,
    2,
    "var_success_ho_report_r17_s::id_list_r18_c_"};

plmn_id_list2_r16_l& var_success_ho_report_r17_s::id_list_r18_c_::set_plmn_id_list_r18()
{
  set(types::plmn_id_list_r18);
  return c.get<plmn_id_list2_r16_l>();
}
var_success_ho_report_r17_s::id_list_r18_c_::snpn_id_list_r18_l_&
var_success_ho_report_r17_s::id_list_r18_c_::set_snpn_id_list_r18()
{
  set(types::snpn_id_list_r18);
  return c.get<snpn_id_list_r18_l_>();
}
static const asn1::table::seq_field choice_table_var_success_ho_report_r17_s_id_list_r18_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::list_codec<plmn_id_list2_r16_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     11,
     &asn1::table::list_codec<var_success_ho_report_r17_s::id_list_r18_c_::snpn_id_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_var_success_ho_report_r17_s_id_list_r18_c__names[] = {
    {"plmn-IdentityList-r18", nullptr},
    {"snpn-IdentityList-r18", nullptr}};
const asn1::table::choice_desc var_success_ho_report_r17_s::id_list_r18_c_::desc = {
    asn1::table::enum_int_type<var_success_ho_report_r17_s::id_list_r18_c_::types>(),
    var_success_ho_report_r17_s::id_list_r18_c_::types::nof_types,
    var_success_ho_report_r17_s::id_list_r18_c_::types::nof_exts,
    var_success_ho_report_r17_s::id_list_r18_c_::types::has_ext,
    false,
    choice_table_var_success_ho_report_r17_s_id_list_r18_c__alternatives,
    nullptr,
    choice_table_var_success_ho_report_r17_s_id_list_r18_c__names,
    "var_success_ho_report_r17_s::id_list_r18_c_",
    offsetof(var_success_ho_report_r17_s::id_list_r18_c_, c.type),
    offsetof(var_success_ho_report_r17_s::id_list_r18_c_, c.value),
    false,
    &var_success_ho_report_r17_s::id_list_r18_c_::storage_desc};

static const char* enum_names_var_success_ho_report_r17_s_id_list_r18_c__types_values[]    = {"plmn-IdentityList-r18",
                                                                                              "snpn-IdentityList-r18"};
const asn1::table::enum_desc var_success_ho_report_r17_s::id_list_r18_c_::types_opts::desc = {
    enum_names_var_success_ho_report_r17_s_id_list_r18_c__types_values,
    2,
    "var_success_ho_report_r17_s::id_list_r18_c_::types"};

// VarSuccessPSCell-Report-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_success_pscell_report_r18_s_fields[] = {
    {offsetof(var_success_pscell_report_r18_s, success_pscell_report_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &success_pscell_report_r18_s::desc},
    {offsetof(var_success_pscell_report_r18_s, id_list_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &var_success_pscell_report_r18_s::id_list_r18_c_::desc}};
static const asn1::table::field_name seq_table_var_success_pscell_report_r18_s_names[] = {
    {"successPSCell-Report-r18", nullptr},
    {"identityList-r18", nullptr}};
const asn1::table::seq_desc var_success_pscell_report_r18_s::desc = {{seq_table_var_success_pscell_report_r18_s_fields,
                                                                      uint64_t{0x3},
                                                                      nullptr,
                                                                      seq_table_var_success_pscell_report_r18_s_names,
                                                                      2,
                                                                      0},
                                                                     false,
                                                                     false,
                                                                     nullptr,
                                                                     0};

static const asn1::table::storage_ops* const
    choice_storage_var_success_pscell_report_r18_s_id_list_r18_c__alternatives[] = {
        &asn1::table::alternative_ops<plmn_id_list2_r16_l, false>::value,
        &asn1::table::alternative_ops<var_success_pscell_report_r18_s::id_list_r18_c_::snpn_id_list_r18_l_,
                                      false>::value};
const asn1::table::choice_storage_desc var_success_pscell_report_r18_s::id_list_r18_c_::storage_desc = {
    choice_storage_var_success_pscell_report_r18_s_id_list_r18_c__alternatives,
    2,
    "var_success_pscell_report_r18_s::id_list_r18_c_"};

plmn_id_list2_r16_l& var_success_pscell_report_r18_s::id_list_r18_c_::set_plmn_id_list_r18()
{
  set(types::plmn_id_list_r18);
  return c.get<plmn_id_list2_r16_l>();
}
var_success_pscell_report_r18_s::id_list_r18_c_::snpn_id_list_r18_l_&
var_success_pscell_report_r18_s::id_list_r18_c_::set_snpn_id_list_r18()
{
  set(types::snpn_id_list_r18);
  return c.get<snpn_id_list_r18_l_>();
}
static const asn1::table::seq_field choice_table_var_success_pscell_report_r18_s_id_list_r18_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::list_codec<plmn_id_list2_r16_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     11,
     &asn1::table::list_codec<var_success_pscell_report_r18_s::id_list_r18_c_::snpn_id_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_var_success_pscell_report_r18_s_id_list_r18_c__names[] = {
    {"plmn-IdentityList-r18", nullptr},
    {"snpn-IdentityList-r18", nullptr}};
const asn1::table::choice_desc var_success_pscell_report_r18_s::id_list_r18_c_::desc = {
    asn1::table::enum_int_type<var_success_pscell_report_r18_s::id_list_r18_c_::types>(),
    var_success_pscell_report_r18_s::id_list_r18_c_::types::nof_types,
    var_success_pscell_report_r18_s::id_list_r18_c_::types::nof_exts,
    var_success_pscell_report_r18_s::id_list_r18_c_::types::has_ext,
    false,
    choice_table_var_success_pscell_report_r18_s_id_list_r18_c__alternatives,
    nullptr,
    choice_table_var_success_pscell_report_r18_s_id_list_r18_c__names,
    "var_success_pscell_report_r18_s::id_list_r18_c_",
    offsetof(var_success_pscell_report_r18_s::id_list_r18_c_, c.type),
    offsetof(var_success_pscell_report_r18_s::id_list_r18_c_, c.value),
    false,
    &var_success_pscell_report_r18_s::id_list_r18_c_::storage_desc};

static const char* enum_names_var_success_pscell_report_r18_s_id_list_r18_c__types_values[] = {"plmn-IdentityList-r18",
                                                                                               "snpn-IdentityList-r18"};
const asn1::table::enum_desc var_success_pscell_report_r18_s::id_list_r18_c_::types_opts::desc = {
    enum_names_var_success_pscell_report_r18_s_id_list_r18_c__types_values,
    2,
    "var_success_pscell_report_r18_s::id_list_r18_c_::types"};

// VarTSS-Info-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_var_tss_info_r18_s_stored_global_gnb_id_r18_s__fields[] = {
    {offsetof(var_tss_info_r18_s::stored_global_gnb_id_r18_s_, plmn_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &plmn_id_s::desc},
    {offsetof(var_tss_info_r18_s::stored_global_gnb_id_r18_s_, gnb_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(var_tss_info_r18_s::stored_global_gnb_id_r18_s_::gnb_id_r18),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_var_tss_info_r18_s_stored_global_gnb_id_r18_s__names[] = {
    {"plmn-Identity-r18", nullptr},
    {"gnb-ID-r18", nullptr}};
static const asn1::table::seq_desc seq_table_var_tss_info_r18_s_stored_global_gnb_id_r18_s_ = {
    {seq_table_var_tss_info_r18_s_stored_global_gnb_id_r18_s__fields,
     uint64_t{0x3},
     nullptr,
     seq_table_var_tss_info_r18_s_stored_global_gnb_id_r18_s__names,
     2,
     0},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_var_tss_info_r18_s_fields[] = {
    {offsetof(var_tss_info_r18_s, stored_event_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{63},
     nullptr},
    {offsetof(var_tss_info_r18_s, stored_global_gnb_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_var_tss_info_r18_s_stored_global_gnb_id_r18_s_}};
static const asn1::table::field_name seq_table_var_tss_info_r18_s_names[] = {{"storedEventID-r18", nullptr},
                                                                             {"storedGlobalGnbID-r18", nullptr}};
const asn1::table::seq_desc          var_tss_info_r18_s::desc             = {
    {seq_table_var_tss_info_r18_s_fields, uint64_t{0x3}, nullptr, seq_table_var_tss_info_r18_s_names, 2, 0},
    false,
    false,
    nullptr,
    0};
