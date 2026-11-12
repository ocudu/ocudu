// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/e2ap/e2ap_pdu_contents.h"
#include "ocudu/asn1/asn1_ap_codec.h"

using namespace asn1;
using namespace asn1::e2ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

static const uint32_t objset_id_e2_removal_fail_ies_o[]   = {49, 1, 2};
static const uint8_t  objset_crit_e2_removal_fail_ies_o[] = {crit_e::reject, crit_e::ignore, crit_e::ignore};

// E2RemovalFailureIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2_removal_fail_ies_container_values[] = {
    {offsetof(e2_removal_fail_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e2_removal_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(e2_removal_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(e2_removal_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_e2_removal_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2_removal_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc e2_removal_fail_ies_container::desc = {
    objset_id_e2_removal_fail_ies_o,
    objset_crit_e2_removal_fail_ies_o,
    ie_table_e2_removal_fail_ies_container_values,
    ie_table_e2_removal_fail_ies_container_presences,
    3,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e2_removal_request_ies_o[]   = {49};
static const uint8_t  objset_crit_e2_removal_request_ies_o[] = {crit_e::reject};

// E2RemovalRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2_removal_request_ies_container_values[] = {
    {offsetof(e2_removal_request_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr}};
static const int32_t ie_table_e2_removal_request_ies_container_presences[]  = {asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc e2_removal_request_ies_container::desc = {
    objset_id_e2_removal_request_ies_o,
    objset_crit_e2_removal_request_ies_o,
    ie_table_e2_removal_request_ies_container_values,
    ie_table_e2_removal_request_ies_container_presences,
    1,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e2_removal_resp_ies_o[]   = {49, 2};
static const uint8_t  objset_crit_e2_removal_resp_ies_o[] = {crit_e::reject, crit_e::ignore};

// E2RemovalResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2_removal_resp_ies_container_values[] = {
    {offsetof(e2_removal_resp_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e2_removal_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(e2_removal_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_e2_removal_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2_removal_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc e2_removal_resp_ies_container::desc = {
    objset_id_e2_removal_resp_ies_o,
    objset_crit_e2_removal_resp_ies_o,
    ie_table_e2_removal_resp_ies_container_values,
    ie_table_e2_removal_resp_ies_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e2_res_status_fail_ies_o[]   = {49, 117, 105, 1, 2};
static const uint8_t  objset_crit_e2_res_status_fail_ies_o[] = {crit_e::reject,
                                                                crit_e::reject,
                                                                crit_e::reject,
                                                                crit_e::ignore,
                                                                crit_e::ignore};

// E2ResourceStatusFailureIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2_res_status_fail_ies_container_values[] = {
    {offsetof(e2_res_status_fail_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e2_res_status_fail_ies_container, ric_load_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(e2_res_status_fail_ies_container, e2node_load_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(e2_res_status_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(e2_res_status_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(e2_res_status_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_e2_res_status_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2_res_status_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc e2_res_status_fail_ies_container::desc = {
    objset_id_e2_res_status_fail_ies_o,
    objset_crit_e2_res_status_fail_ies_o,
    ie_table_e2_res_status_fail_ies_container_values,
    ie_table_e2_res_status_fail_ies_container_presences,
    5,
    4,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e2_res_status_request_ies_o[] = {49, 117, 105, 133, 135, 134};
static const uint8_t  objset_crit_e2_res_status_request_ies_o[] =
    {crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// E2ResourceStatusRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2_res_status_request_ies_container_values[] = {
    {offsetof(e2_res_status_request_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e2_res_status_request_ies_container, ric_load_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(e2_res_status_request_ies_container, e2node_load_meas_id),
     (int32_t)offsetof(e2_res_status_request_ies_container, e2node_load_meas_id_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(e2_res_status_request_ies_container, regist_request_e2),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(e2_res_status_request_ies_container::regist_request_e2)>(),
     decltype(e2_res_status_request_ies_container::regist_request_e2)::has_ext,
     false,
     decltype(e2_res_status_request_ies_container::regist_request_e2)::nof_types,
     decltype(e2_res_status_request_ies_container::regist_request_e2)::nof_exts,
     nullptr,
     &regist_request_e2_opts::desc},
    {offsetof(e2_res_status_request_ies_container, report_characteristics),
     (int32_t)offsetof(e2_res_status_request_ies_container, report_characteristics_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2_res_status_request_ies_container::report_characteristics),
                                asn1::table::struct_json>::value},
    {offsetof(e2_res_status_request_ies_container, report_periodicity_e2),
     (int32_t)offsetof(e2_res_status_request_ies_container, report_periodicity_e2_present),
     asn1::table::enum_op<decltype(e2_res_status_request_ies_container::report_periodicity_e2)>(),
     decltype(e2_res_status_request_ies_container::report_periodicity_e2)::has_ext,
     false,
     decltype(e2_res_status_request_ies_container::report_periodicity_e2)::nof_types,
     decltype(e2_res_status_request_ies_container::report_periodicity_e2)::nof_exts,
     nullptr,
     &report_periodicity_e2_opts::desc}};
static const int32_t ie_table_e2_res_status_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2_res_status_request_ies_container, e2node_load_meas_id_present),
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2_res_status_request_ies_container, report_characteristics_present),
    (int32_t)offsetof(e2_res_status_request_ies_container, report_periodicity_e2_present)};
const asn1::table::ie_container_desc e2_res_status_request_ies_container::desc = {
    objset_id_e2_res_status_request_ies_o,
    objset_crit_e2_res_status_request_ies_o,
    ie_table_e2_res_status_request_ies_container_values,
    ie_table_e2_res_status_request_ies_container_presences,
    6,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e2_res_status_resp_ies_o[]   = {49, 117, 105, 2};
static const uint8_t  objset_crit_e2_res_status_resp_ies_o[] = {crit_e::reject,
                                                                crit_e::reject,
                                                                crit_e::reject,
                                                                crit_e::ignore};

// E2ResourceStatusResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2_res_status_resp_ies_container_values[] = {
    {offsetof(e2_res_status_resp_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e2_res_status_resp_ies_container, ric_load_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(e2_res_status_resp_ies_container, e2node_load_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(e2_res_status_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(e2_res_status_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_e2_res_status_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2_res_status_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc e2_res_status_resp_ies_container::desc = {
    objset_id_e2_res_status_resp_ies_o,
    objset_crit_e2_res_status_resp_ies_o,
    ie_table_e2_res_status_resp_ies_container_values,
    ie_table_e2_res_status_resp_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e2_res_status_upd_ies_o[]   = {49, 117, 105, 136, 137};
static const uint8_t  objset_crit_e2_res_status_upd_ies_o[] = {crit_e::reject,
                                                               crit_e::reject,
                                                               crit_e::reject,
                                                               crit_e::ignore,
                                                               crit_e::ignore};

// E2ResourceStatusUpdateIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2_res_status_upd_ies_container_values[] = {
    {offsetof(e2_res_status_upd_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e2_res_status_upd_ies_container, ric_load_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(e2_res_status_upd_ies_container, e2node_load_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {0,
     (int32_t)offsetof(e2_res_status_upd_ies_container, tnl_capacity_ind_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(e2_res_status_upd_ies_container, hardware_load_ind_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const int32_t ie_table_e2_res_status_upd_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2_res_status_upd_ies_container, tnl_capacity_ind_present),
    (int32_t)offsetof(e2_res_status_upd_ies_container, hardware_load_ind_present)};
const asn1::table::ie_container_desc e2_res_status_upd_ies_container::desc = {
    objset_id_e2_res_status_upd_ies_o,
    objset_crit_e2_res_status_upd_ies_o,
    ie_table_e2_res_status_upd_ies_container_values,
    ie_table_e2_res_status_upd_ies_container_presences,
    5,
    3,
    0,
    65535,
    true,
    "value"};

// E2connectionSetupFailed-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2conn_setup_failed_item_s_fields[] = {
    {offsetof(e2conn_setup_failed_item_s, tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tnl_info_s::desc},
    {offsetof(e2conn_setup_failed_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const asn1::table::field_name seq_table_e2conn_setup_failed_item_s_names[] = {{"tnlInformation", nullptr},
                                                                                     {"cause", nullptr}};
const asn1::table::seq_desc          e2conn_setup_failed_item_s::desc = {{seq_table_e2conn_setup_failed_item_s_fields,
                                                                          uint64_t{0x3},
                                                                          nullptr,
                                                                          seq_table_e2conn_setup_failed_item_s_names,
                                                                          2,
                                                                          0},
                                                                         true,
                                                                         true,
                                                                         nullptr,
                                                                         0};

// E2connectionSetupFailed-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_e2conn_setup_failed_item_ies_o[]    = {41};
static const uint8_t  objset_crit_e2conn_setup_failed_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_e2conn_setup_failed_item_ies_o[] = {
    e2conn_setup_failed_item_ies_o::value_c::types::e2conn_setup_failed_item};
static const uint8_t                    objset_presence_e2conn_setup_failed_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_e2conn_setup_failed_item_ies_o[]  = {
    {objset_crit_e2conn_setup_failed_item_ies_o, crit_e::nulltype},
    {objset_value_e2conn_setup_failed_item_ies_o, e2conn_setup_failed_item_ies_o::value_c::types::nulltype},
    {objset_presence_e2conn_setup_failed_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc e2conn_setup_failed_item_ies_o::desc = {objset_id_e2conn_setup_failed_item_ies_o,
                                                                       1,
                                                                       "id",
                                                                       objset_columns_e2conn_setup_failed_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e2conn_setup_failed_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2conn_setup_failed_item_s::desc}};
static const asn1::table::field_name choice_table_e2conn_setup_failed_item_ies_o_value_c_names[] = {
    {"E2connectionSetupFailed-Item", nullptr}};
const asn1::table::choice_desc e2conn_setup_failed_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<e2conn_setup_failed_item_ies_o::value_c::types>(),
    e2conn_setup_failed_item_ies_o::value_c::types::nof_types,
    e2conn_setup_failed_item_ies_o::value_c::types::nof_exts,
    e2conn_setup_failed_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_e2conn_setup_failed_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_e2conn_setup_failed_item_ies_o_value_c_names,
    "e2conn_setup_failed_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(e2conn_setup_failed_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_e2conn_setup_failed_item_ies_o_value_c_types_values[] = {"E2connectionSetupFailed-Item"};
const asn1::table::enum_desc e2conn_setup_failed_item_ies_o::value_c::types_opts::desc = {
    enum_names_e2conn_setup_failed_item_ies_o_value_c_types_values,
    1,
    "e2conn_setup_failed_item_ies_o::value_c::types"};
uint8_t e2conn_setup_failed_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2conn_setup_failed_item_ies_o::value_c::types");
}

// E2connectionUpdate-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2conn_upd_item_s_fields[] = {
    {offsetof(e2conn_upd_item_s, tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tnl_info_s::desc},
    {offsetof(e2conn_upd_item_s, tnl_usage),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(e2conn_upd_item_s::tnl_usage)>(),
     decltype(e2conn_upd_item_s::tnl_usage)::has_ext,
     false,
     decltype(e2conn_upd_item_s::tnl_usage)::nof_types,
     decltype(e2conn_upd_item_s::tnl_usage)::nof_exts,
     nullptr,
     &tnl_usage_opts::desc}};
static const asn1::table::field_name seq_table_e2conn_upd_item_s_names[] = {{"tnlInformation", nullptr},
                                                                            {"tnlUsage", nullptr}};
const asn1::table::seq_desc          e2conn_upd_item_s::desc             = {
    {seq_table_e2conn_upd_item_s_fields, uint64_t{0x3}, nullptr, seq_table_e2conn_upd_item_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// E2connectionUpdate-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_e2conn_upd_item_ies_o[]    = {43};
static const uint8_t  objset_crit_e2conn_upd_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_e2conn_upd_item_ies_o[] = {e2conn_upd_item_ies_o::value_c::types::e2conn_upd_item};
static const uint8_t  objset_presence_e2conn_upd_item_ies_o[]                  = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_e2conn_upd_item_ies_o[] = {
    {objset_crit_e2conn_upd_item_ies_o, crit_e::nulltype},
    {objset_value_e2conn_upd_item_ies_o, e2conn_upd_item_ies_o::value_c::types::nulltype},
    {objset_presence_e2conn_upd_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc e2conn_upd_item_ies_o::desc = {objset_id_e2conn_upd_item_ies_o,
                                                              1,
                                                              "id",
                                                              objset_columns_e2conn_upd_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e2conn_upd_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2conn_upd_item_s::desc}};
static const asn1::table::field_name choice_table_e2conn_upd_item_ies_o_value_c_names[] = {
    {"E2connectionUpdate-Item", nullptr}};
const asn1::table::choice_desc e2conn_upd_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<e2conn_upd_item_ies_o::value_c::types>(),
    e2conn_upd_item_ies_o::value_c::types::nof_types,
    e2conn_upd_item_ies_o::value_c::types::nof_exts,
    e2conn_upd_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_e2conn_upd_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_e2conn_upd_item_ies_o_value_c_names,
    "e2conn_upd_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(e2conn_upd_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_e2conn_upd_item_ies_o_value_c_types_values[] = {"E2connectionUpdate-Item"};
const asn1::table::enum_desc e2conn_upd_item_ies_o::value_c::types_opts::desc        = {
    enum_names_e2conn_upd_item_ies_o_value_c_types_values,
    1,
    "e2conn_upd_item_ies_o::value_c::types"};
uint8_t e2conn_upd_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2conn_upd_item_ies_o::value_c::types");
}

// E2connectionUpdateRemove-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2conn_upd_rem_item_s_fields[] = {
    {offsetof(e2conn_upd_rem_item_s, tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tnl_info_s::desc}};
static const asn1::table::field_name seq_table_e2conn_upd_rem_item_s_names[] = {{"tnlInformation", nullptr}};
const asn1::table::seq_desc          e2conn_upd_rem_item_s::desc             = {
    {seq_table_e2conn_upd_rem_item_s_fields, uint64_t{0x1}, nullptr, seq_table_e2conn_upd_rem_item_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// E2connectionUpdateRemove-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_e2conn_upd_rem_item_ies_o[]    = {47};
static const uint8_t  objset_crit_e2conn_upd_rem_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_e2conn_upd_rem_item_ies_o[] = {
    e2conn_upd_rem_item_ies_o::value_c::types::e2conn_upd_rem_item};
static const uint8_t                    objset_presence_e2conn_upd_rem_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_e2conn_upd_rem_item_ies_o[]  = {
    {objset_crit_e2conn_upd_rem_item_ies_o, crit_e::nulltype},
    {objset_value_e2conn_upd_rem_item_ies_o, e2conn_upd_rem_item_ies_o::value_c::types::nulltype},
    {objset_presence_e2conn_upd_rem_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc e2conn_upd_rem_item_ies_o::desc = {objset_id_e2conn_upd_rem_item_ies_o,
                                                                  1,
                                                                  "id",
                                                                  objset_columns_e2conn_upd_rem_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e2conn_upd_rem_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2conn_upd_rem_item_s::desc}};
static const asn1::table::field_name choice_table_e2conn_upd_rem_item_ies_o_value_c_names[] = {
    {"E2connectionUpdateRemove-Item", nullptr}};
const asn1::table::choice_desc e2conn_upd_rem_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<e2conn_upd_rem_item_ies_o::value_c::types>(),
    e2conn_upd_rem_item_ies_o::value_c::types::nof_types,
    e2conn_upd_rem_item_ies_o::value_c::types::nof_exts,
    e2conn_upd_rem_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_e2conn_upd_rem_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_e2conn_upd_rem_item_ies_o_value_c_names,
    "e2conn_upd_rem_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(e2conn_upd_rem_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_e2conn_upd_rem_item_ies_o_value_c_types_values[]    = {"E2connectionUpdateRemove-Item"};
const asn1::table::enum_desc e2conn_upd_rem_item_ies_o::value_c::types_opts::desc = {
    enum_names_e2conn_upd_rem_item_ies_o_value_c_types_values,
    1,
    "e2conn_upd_rem_item_ies_o::value_c::types"};
uint8_t e2conn_upd_rem_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2conn_upd_rem_item_ies_o::value_c::types");
}

static const uint32_t objset_id_e2conn_upd_ies_o[]   = {49, 44, 46, 45};
static const uint8_t  objset_crit_e2conn_upd_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject};

// E2connectionUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2conn_upd_ies_container_values[] = {
    {offsetof(e2conn_upd_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e2conn_upd_ies_container, e2conn_upd_add),
     (int32_t)offsetof(e2conn_upd_ies_container, e2conn_upd_add_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::list_codec<decltype(e2conn_upd_ies_container::e2conn_upd_add), asn1::table::struct_json>::value},
    {offsetof(e2conn_upd_ies_container, e2conn_upd_rem),
     (int32_t)offsetof(e2conn_upd_ies_container, e2conn_upd_rem_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::list_codec<decltype(e2conn_upd_ies_container::e2conn_upd_rem), asn1::table::struct_json>::value},
    {offsetof(e2conn_upd_ies_container, e2conn_upd_modify),
     (int32_t)offsetof(e2conn_upd_ies_container, e2conn_upd_modify_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::list_codec<decltype(e2conn_upd_ies_container::e2conn_upd_modify), asn1::table::struct_json>::value}};
static const int32_t ie_table_e2conn_upd_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2conn_upd_ies_container, e2conn_upd_add_present),
    (int32_t)offsetof(e2conn_upd_ies_container, e2conn_upd_rem_present),
    (int32_t)offsetof(e2conn_upd_ies_container, e2conn_upd_modify_present)};
const asn1::table::ie_container_desc e2conn_upd_ies_container::desc = {objset_id_e2conn_upd_ies_o,
                                                                       objset_crit_e2conn_upd_ies_o,
                                                                       ie_table_e2conn_upd_ies_container_values,
                                                                       ie_table_e2conn_upd_ies_container_presences,
                                                                       4,
                                                                       1,
                                                                       0,
                                                                       65535,
                                                                       true,
                                                                       "value"};

static const uint32_t objset_id_e2conn_upd_ack_ies_o[]   = {49, 39, 40};
static const uint8_t  objset_crit_e2conn_upd_ack_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::reject};

// E2connectionUpdateAck-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2conn_upd_ack_ies_container_values[] = {
    {offsetof(e2conn_upd_ack_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e2conn_upd_ack_ies_container, e2conn_setup),
     (int32_t)offsetof(e2conn_upd_ack_ies_container, e2conn_setup_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::list_codec<decltype(e2conn_upd_ack_ies_container::e2conn_setup), asn1::table::struct_json>::value},
    {offsetof(e2conn_upd_ack_ies_container, e2conn_setup_failed),
     (int32_t)offsetof(e2conn_upd_ack_ies_container, e2conn_setup_failed_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::list_codec<decltype(e2conn_upd_ack_ies_container::e2conn_setup_failed),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_e2conn_upd_ack_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2conn_upd_ack_ies_container, e2conn_setup_present),
    (int32_t)offsetof(e2conn_upd_ack_ies_container, e2conn_setup_failed_present)};
const asn1::table::ie_container_desc e2conn_upd_ack_ies_container::desc = {
    objset_id_e2conn_upd_ack_ies_o,
    objset_crit_e2conn_upd_ack_ies_o,
    ie_table_e2conn_upd_ack_ies_container_values,
    ie_table_e2conn_upd_ack_ies_container_presences,
    3,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e2conn_upd_fail_ies_o[]   = {49, 1, 31, 2};
static const uint8_t  objset_crit_e2conn_upd_fail_ies_o[] = {crit_e::reject,
                                                             crit_e::reject,
                                                             crit_e::ignore,
                                                             crit_e::ignore};

// E2connectionUpdateFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2conn_upd_fail_ies_container_values[] = {
    {offsetof(e2conn_upd_fail_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e2conn_upd_fail_ies_container, cause),
     (int32_t)offsetof(e2conn_upd_fail_ies_container, cause_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(e2conn_upd_fail_ies_container, time_to_wait),
     (int32_t)offsetof(e2conn_upd_fail_ies_container, time_to_wait_present),
     asn1::table::enum_op<decltype(e2conn_upd_fail_ies_container::time_to_wait)>(),
     decltype(e2conn_upd_fail_ies_container::time_to_wait)::has_ext,
     false,
     decltype(e2conn_upd_fail_ies_container::time_to_wait)::nof_types,
     decltype(e2conn_upd_fail_ies_container::time_to_wait)::nof_exts,
     nullptr,
     &time_to_wait_opts::desc},
    {offsetof(e2conn_upd_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(e2conn_upd_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_e2conn_upd_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2conn_upd_fail_ies_container, cause_present),
    (int32_t)offsetof(e2conn_upd_fail_ies_container, time_to_wait_present),
    (int32_t)offsetof(e2conn_upd_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc e2conn_upd_fail_ies_container::desc = {
    objset_id_e2conn_upd_fail_ies_o,
    objset_crit_e2conn_upd_fail_ies_o,
    ie_table_e2conn_upd_fail_ies_container_values,
    ie_table_e2conn_upd_fail_ies_container_presences,
    4,
    1,
    0,
    65535,
    true,
    "value"};

// E2nodeComponentConfigAddition-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_component_cfg_addition_item_s_fields[] = {
    {offsetof(e2node_component_cfg_addition_item_s, e2node_component_interface_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(e2node_component_cfg_addition_item_s::e2node_component_interface_type)>(),
     decltype(e2node_component_cfg_addition_item_s::e2node_component_interface_type)::has_ext,
     false,
     decltype(e2node_component_cfg_addition_item_s::e2node_component_interface_type)::nof_types,
     decltype(e2node_component_cfg_addition_item_s::e2node_component_interface_type)::nof_exts,
     nullptr,
     &e2node_component_interface_type_opts::desc},
    {offsetof(e2node_component_cfg_addition_item_s, e2node_component_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_id_c::desc},
    {offsetof(e2node_component_cfg_addition_item_s, e2node_component_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_cfg_s::desc}};
static const asn1::table::field_name seq_table_e2node_component_cfg_addition_item_s_names[] = {
    {"e2nodeComponentInterfaceType", nullptr},
    {"e2nodeComponentID", nullptr},
    {"e2nodeComponentConfiguration", nullptr}};
const asn1::table::seq_desc e2node_component_cfg_addition_item_s::desc = {
    {seq_table_e2node_component_cfg_addition_item_s_fields,
     uint64_t{0x7},
     nullptr,
     seq_table_e2node_component_cfg_addition_item_s_names,
     3,
     0},
    true,
    true,
    nullptr,
    0};

// E2nodeComponentConfigAddition-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_e2node_component_cfg_addition_item_ies_o[]    = {51};
static const uint8_t  objset_crit_e2node_component_cfg_addition_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_e2node_component_cfg_addition_item_ies_o[] = {
    e2node_component_cfg_addition_item_ies_o::value_c::types::e2node_component_cfg_addition_item};
static const uint8_t objset_presence_e2node_component_cfg_addition_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_e2node_component_cfg_addition_item_ies_o[] = {
    {objset_crit_e2node_component_cfg_addition_item_ies_o, crit_e::nulltype},
    {objset_value_e2node_component_cfg_addition_item_ies_o,
     e2node_component_cfg_addition_item_ies_o::value_c::types::nulltype},
    {objset_presence_e2node_component_cfg_addition_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc e2node_component_cfg_addition_item_ies_o::desc = {
    objset_id_e2node_component_cfg_addition_item_ies_o,
    1,
    "id",
    objset_columns_e2node_component_cfg_addition_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e2node_component_cfg_addition_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_cfg_addition_item_s::desc}};
static const asn1::table::field_name choice_table_e2node_component_cfg_addition_item_ies_o_value_c_names[] = {
    {"E2nodeComponentConfigAddition-Item", nullptr}};
const asn1::table::choice_desc e2node_component_cfg_addition_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<e2node_component_cfg_addition_item_ies_o::value_c::types>(),
    e2node_component_cfg_addition_item_ies_o::value_c::types::nof_types,
    e2node_component_cfg_addition_item_ies_o::value_c::types::nof_exts,
    e2node_component_cfg_addition_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_e2node_component_cfg_addition_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_e2node_component_cfg_addition_item_ies_o_value_c_names,
    "e2node_component_cfg_addition_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(e2node_component_cfg_addition_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_e2node_component_cfg_addition_item_ies_o_value_c_types_values[] = {
    "E2nodeComponentConfigAddition-Item"};
const asn1::table::enum_desc e2node_component_cfg_addition_item_ies_o::value_c::types_opts::desc = {
    enum_names_e2node_component_cfg_addition_item_ies_o_value_c_types_values,
    1,
    "e2node_component_cfg_addition_item_ies_o::value_c::types"};
uint8_t e2node_component_cfg_addition_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2node_component_cfg_addition_item_ies_o::value_c::types");
}

// E2nodeComponentConfigAdditionAck-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_component_cfg_addition_ack_item_s_fields[] = {
    {offsetof(e2node_component_cfg_addition_ack_item_s, e2node_component_interface_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(e2node_component_cfg_addition_ack_item_s::e2node_component_interface_type)>(),
     decltype(e2node_component_cfg_addition_ack_item_s::e2node_component_interface_type)::has_ext,
     false,
     decltype(e2node_component_cfg_addition_ack_item_s::e2node_component_interface_type)::nof_types,
     decltype(e2node_component_cfg_addition_ack_item_s::e2node_component_interface_type)::nof_exts,
     nullptr,
     &e2node_component_interface_type_opts::desc},
    {offsetof(e2node_component_cfg_addition_ack_item_s, e2node_component_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_id_c::desc},
    {offsetof(e2node_component_cfg_addition_ack_item_s, e2node_component_cfg_ack),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_cfg_ack_s::desc}};
static const asn1::table::field_name seq_table_e2node_component_cfg_addition_ack_item_s_names[] = {
    {"e2nodeComponentInterfaceType", nullptr},
    {"e2nodeComponentID", nullptr},
    {"e2nodeComponentConfigurationAck", nullptr}};
const asn1::table::seq_desc e2node_component_cfg_addition_ack_item_s::desc = {
    {seq_table_e2node_component_cfg_addition_ack_item_s_fields,
     uint64_t{0x7},
     nullptr,
     seq_table_e2node_component_cfg_addition_ack_item_s_names,
     3,
     0},
    true,
    true,
    nullptr,
    0};

// E2nodeComponentConfigAdditionAck-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_e2node_component_cfg_addition_ack_item_ies_o[]    = {53};
static const uint8_t  objset_crit_e2node_component_cfg_addition_ack_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_e2node_component_cfg_addition_ack_item_ies_o[] = {
    e2node_component_cfg_addition_ack_item_ies_o::value_c::types::e2node_component_cfg_addition_ack_item};
static const uint8_t objset_presence_e2node_component_cfg_addition_ack_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_e2node_component_cfg_addition_ack_item_ies_o[] = {
    {objset_crit_e2node_component_cfg_addition_ack_item_ies_o, crit_e::nulltype},
    {objset_value_e2node_component_cfg_addition_ack_item_ies_o,
     e2node_component_cfg_addition_ack_item_ies_o::value_c::types::nulltype},
    {objset_presence_e2node_component_cfg_addition_ack_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc e2node_component_cfg_addition_ack_item_ies_o::desc = {
    objset_id_e2node_component_cfg_addition_ack_item_ies_o,
    1,
    "id",
    objset_columns_e2node_component_cfg_addition_ack_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e2node_component_cfg_addition_ack_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_cfg_addition_ack_item_s::desc}};
static const asn1::table::field_name choice_table_e2node_component_cfg_addition_ack_item_ies_o_value_c_names[] = {
    {"E2nodeComponentConfigAdditionAck-Item", nullptr}};
const asn1::table::choice_desc e2node_component_cfg_addition_ack_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<e2node_component_cfg_addition_ack_item_ies_o::value_c::types>(),
    e2node_component_cfg_addition_ack_item_ies_o::value_c::types::nof_types,
    e2node_component_cfg_addition_ack_item_ies_o::value_c::types::nof_exts,
    e2node_component_cfg_addition_ack_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_e2node_component_cfg_addition_ack_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_e2node_component_cfg_addition_ack_item_ies_o_value_c_names,
    "e2node_component_cfg_addition_ack_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(e2node_component_cfg_addition_ack_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_e2node_component_cfg_addition_ack_item_ies_o_value_c_types_values[] = {
    "E2nodeComponentConfigAdditionAck-Item"};
const asn1::table::enum_desc e2node_component_cfg_addition_ack_item_ies_o::value_c::types_opts::desc = {
    enum_names_e2node_component_cfg_addition_ack_item_ies_o_value_c_types_values,
    1,
    "e2node_component_cfg_addition_ack_item_ies_o::value_c::types"};
uint8_t e2node_component_cfg_addition_ack_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2node_component_cfg_addition_ack_item_ies_o::value_c::types");
}

// E2nodeComponentConfigRemoval-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_component_cfg_removal_item_s_fields[] = {
    {offsetof(e2node_component_cfg_removal_item_s, e2node_component_interface_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(e2node_component_cfg_removal_item_s::e2node_component_interface_type)>(),
     decltype(e2node_component_cfg_removal_item_s::e2node_component_interface_type)::has_ext,
     false,
     decltype(e2node_component_cfg_removal_item_s::e2node_component_interface_type)::nof_types,
     decltype(e2node_component_cfg_removal_item_s::e2node_component_interface_type)::nof_exts,
     nullptr,
     &e2node_component_interface_type_opts::desc},
    {offsetof(e2node_component_cfg_removal_item_s, e2node_component_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_id_c::desc}};
static const asn1::table::field_name seq_table_e2node_component_cfg_removal_item_s_names[] = {
    {"e2nodeComponentInterfaceType", nullptr},
    {"e2nodeComponentID", nullptr}};
const asn1::table::seq_desc e2node_component_cfg_removal_item_s::desc = {
    {seq_table_e2node_component_cfg_removal_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2node_component_cfg_removal_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2nodeComponentConfigRemoval-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_e2node_component_cfg_removal_item_ies_o[]    = {55};
static const uint8_t  objset_crit_e2node_component_cfg_removal_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_e2node_component_cfg_removal_item_ies_o[] = {
    e2node_component_cfg_removal_item_ies_o::value_c::types::e2node_component_cfg_removal_item};
static const uint8_t objset_presence_e2node_component_cfg_removal_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_e2node_component_cfg_removal_item_ies_o[] = {
    {objset_crit_e2node_component_cfg_removal_item_ies_o, crit_e::nulltype},
    {objset_value_e2node_component_cfg_removal_item_ies_o,
     e2node_component_cfg_removal_item_ies_o::value_c::types::nulltype},
    {objset_presence_e2node_component_cfg_removal_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc e2node_component_cfg_removal_item_ies_o::desc = {
    objset_id_e2node_component_cfg_removal_item_ies_o,
    1,
    "id",
    objset_columns_e2node_component_cfg_removal_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e2node_component_cfg_removal_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_cfg_removal_item_s::desc}};
static const asn1::table::field_name choice_table_e2node_component_cfg_removal_item_ies_o_value_c_names[] = {
    {"E2nodeComponentConfigRemoval-Item", nullptr}};
const asn1::table::choice_desc e2node_component_cfg_removal_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<e2node_component_cfg_removal_item_ies_o::value_c::types>(),
    e2node_component_cfg_removal_item_ies_o::value_c::types::nof_types,
    e2node_component_cfg_removal_item_ies_o::value_c::types::nof_exts,
    e2node_component_cfg_removal_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_e2node_component_cfg_removal_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_e2node_component_cfg_removal_item_ies_o_value_c_names,
    "e2node_component_cfg_removal_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(e2node_component_cfg_removal_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_e2node_component_cfg_removal_item_ies_o_value_c_types_values[] = {
    "E2nodeComponentConfigRemoval-Item"};
const asn1::table::enum_desc e2node_component_cfg_removal_item_ies_o::value_c::types_opts::desc = {
    enum_names_e2node_component_cfg_removal_item_ies_o_value_c_types_values,
    1,
    "e2node_component_cfg_removal_item_ies_o::value_c::types"};
uint8_t e2node_component_cfg_removal_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2node_component_cfg_removal_item_ies_o::value_c::types");
}

// E2nodeComponentConfigRemovalAck-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_component_cfg_removal_ack_item_s_fields[] = {
    {offsetof(e2node_component_cfg_removal_ack_item_s, e2node_component_interface_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(e2node_component_cfg_removal_ack_item_s::e2node_component_interface_type)>(),
     decltype(e2node_component_cfg_removal_ack_item_s::e2node_component_interface_type)::has_ext,
     false,
     decltype(e2node_component_cfg_removal_ack_item_s::e2node_component_interface_type)::nof_types,
     decltype(e2node_component_cfg_removal_ack_item_s::e2node_component_interface_type)::nof_exts,
     nullptr,
     &e2node_component_interface_type_opts::desc},
    {offsetof(e2node_component_cfg_removal_ack_item_s, e2node_component_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_id_c::desc},
    {offsetof(e2node_component_cfg_removal_ack_item_s, e2node_component_cfg_ack),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_cfg_ack_s::desc}};
static const asn1::table::field_name seq_table_e2node_component_cfg_removal_ack_item_s_names[] = {
    {"e2nodeComponentInterfaceType", nullptr},
    {"e2nodeComponentID", nullptr},
    {"e2nodeComponentConfigurationAck", nullptr}};
const asn1::table::seq_desc e2node_component_cfg_removal_ack_item_s::desc = {
    {seq_table_e2node_component_cfg_removal_ack_item_s_fields,
     uint64_t{0x7},
     nullptr,
     seq_table_e2node_component_cfg_removal_ack_item_s_names,
     3,
     0},
    true,
    true,
    nullptr,
    0};

// E2nodeComponentConfigRemovalAck-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_e2node_component_cfg_removal_ack_item_ies_o[]    = {57};
static const uint8_t  objset_crit_e2node_component_cfg_removal_ack_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_e2node_component_cfg_removal_ack_item_ies_o[] = {
    e2node_component_cfg_removal_ack_item_ies_o::value_c::types::e2node_component_cfg_removal_ack_item};
static const uint8_t objset_presence_e2node_component_cfg_removal_ack_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_e2node_component_cfg_removal_ack_item_ies_o[] = {
    {objset_crit_e2node_component_cfg_removal_ack_item_ies_o, crit_e::nulltype},
    {objset_value_e2node_component_cfg_removal_ack_item_ies_o,
     e2node_component_cfg_removal_ack_item_ies_o::value_c::types::nulltype},
    {objset_presence_e2node_component_cfg_removal_ack_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc e2node_component_cfg_removal_ack_item_ies_o::desc = {
    objset_id_e2node_component_cfg_removal_ack_item_ies_o,
    1,
    "id",
    objset_columns_e2node_component_cfg_removal_ack_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e2node_component_cfg_removal_ack_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_cfg_removal_ack_item_s::desc}};
static const asn1::table::field_name choice_table_e2node_component_cfg_removal_ack_item_ies_o_value_c_names[] = {
    {"E2nodeComponentConfigRemovalAck-Item", nullptr}};
const asn1::table::choice_desc e2node_component_cfg_removal_ack_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<e2node_component_cfg_removal_ack_item_ies_o::value_c::types>(),
    e2node_component_cfg_removal_ack_item_ies_o::value_c::types::nof_types,
    e2node_component_cfg_removal_ack_item_ies_o::value_c::types::nof_exts,
    e2node_component_cfg_removal_ack_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_e2node_component_cfg_removal_ack_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_e2node_component_cfg_removal_ack_item_ies_o_value_c_names,
    "e2node_component_cfg_removal_ack_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(e2node_component_cfg_removal_ack_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_e2node_component_cfg_removal_ack_item_ies_o_value_c_types_values[] = {
    "E2nodeComponentConfigRemovalAck-Item"};
const asn1::table::enum_desc e2node_component_cfg_removal_ack_item_ies_o::value_c::types_opts::desc = {
    enum_names_e2node_component_cfg_removal_ack_item_ies_o_value_c_types_values,
    1,
    "e2node_component_cfg_removal_ack_item_ies_o::value_c::types"};
uint8_t e2node_component_cfg_removal_ack_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2node_component_cfg_removal_ack_item_ies_o::value_c::types");
}

// E2nodeComponentConfigUpdate-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_component_cfg_upd_item_s_fields[] = {
    {offsetof(e2node_component_cfg_upd_item_s, e2node_component_interface_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(e2node_component_cfg_upd_item_s::e2node_component_interface_type)>(),
     decltype(e2node_component_cfg_upd_item_s::e2node_component_interface_type)::has_ext,
     false,
     decltype(e2node_component_cfg_upd_item_s::e2node_component_interface_type)::nof_types,
     decltype(e2node_component_cfg_upd_item_s::e2node_component_interface_type)::nof_exts,
     nullptr,
     &e2node_component_interface_type_opts::desc},
    {offsetof(e2node_component_cfg_upd_item_s, e2node_component_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_id_c::desc},
    {offsetof(e2node_component_cfg_upd_item_s, e2node_component_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_cfg_s::desc}};
static const asn1::table::field_name seq_table_e2node_component_cfg_upd_item_s_names[] = {
    {"e2nodeComponentInterfaceType", nullptr},
    {"e2nodeComponentID", nullptr},
    {"e2nodeComponentConfiguration", nullptr}};
const asn1::table::seq_desc e2node_component_cfg_upd_item_s::desc = {{seq_table_e2node_component_cfg_upd_item_s_fields,
                                                                      uint64_t{0x7},
                                                                      nullptr,
                                                                      seq_table_e2node_component_cfg_upd_item_s_names,
                                                                      3,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// E2nodeComponentConfigUpdate-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_e2node_component_cfg_upd_item_ies_o[]    = {34};
static const uint8_t  objset_crit_e2node_component_cfg_upd_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_e2node_component_cfg_upd_item_ies_o[] = {
    e2node_component_cfg_upd_item_ies_o::value_c::types::e2node_component_cfg_upd_item};
static const uint8_t                    objset_presence_e2node_component_cfg_upd_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_e2node_component_cfg_upd_item_ies_o[]  = {
    {objset_crit_e2node_component_cfg_upd_item_ies_o, crit_e::nulltype},
    {objset_value_e2node_component_cfg_upd_item_ies_o, e2node_component_cfg_upd_item_ies_o::value_c::types::nulltype},
    {objset_presence_e2node_component_cfg_upd_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc e2node_component_cfg_upd_item_ies_o::desc =
    {objset_id_e2node_component_cfg_upd_item_ies_o, 1, "id", objset_columns_e2node_component_cfg_upd_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e2node_component_cfg_upd_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_cfg_upd_item_s::desc}};
static const asn1::table::field_name choice_table_e2node_component_cfg_upd_item_ies_o_value_c_names[] = {
    {"E2nodeComponentConfigUpdate-Item", nullptr}};
const asn1::table::choice_desc e2node_component_cfg_upd_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<e2node_component_cfg_upd_item_ies_o::value_c::types>(),
    e2node_component_cfg_upd_item_ies_o::value_c::types::nof_types,
    e2node_component_cfg_upd_item_ies_o::value_c::types::nof_exts,
    e2node_component_cfg_upd_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_e2node_component_cfg_upd_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_e2node_component_cfg_upd_item_ies_o_value_c_names,
    "e2node_component_cfg_upd_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(e2node_component_cfg_upd_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_e2node_component_cfg_upd_item_ies_o_value_c_types_values[] = {
    "E2nodeComponentConfigUpdate-Item"};
const asn1::table::enum_desc e2node_component_cfg_upd_item_ies_o::value_c::types_opts::desc = {
    enum_names_e2node_component_cfg_upd_item_ies_o_value_c_types_values,
    1,
    "e2node_component_cfg_upd_item_ies_o::value_c::types"};
uint8_t e2node_component_cfg_upd_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2node_component_cfg_upd_item_ies_o::value_c::types");
}

// E2nodeComponentConfigUpdateAck-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_component_cfg_upd_ack_item_s_fields[] = {
    {offsetof(e2node_component_cfg_upd_ack_item_s, e2node_component_interface_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(e2node_component_cfg_upd_ack_item_s::e2node_component_interface_type)>(),
     decltype(e2node_component_cfg_upd_ack_item_s::e2node_component_interface_type)::has_ext,
     false,
     decltype(e2node_component_cfg_upd_ack_item_s::e2node_component_interface_type)::nof_types,
     decltype(e2node_component_cfg_upd_ack_item_s::e2node_component_interface_type)::nof_exts,
     nullptr,
     &e2node_component_interface_type_opts::desc},
    {offsetof(e2node_component_cfg_upd_ack_item_s, e2node_component_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_id_c::desc},
    {offsetof(e2node_component_cfg_upd_ack_item_s, e2node_component_cfg_ack),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_cfg_ack_s::desc}};
static const asn1::table::field_name seq_table_e2node_component_cfg_upd_ack_item_s_names[] = {
    {"e2nodeComponentInterfaceType", nullptr},
    {"e2nodeComponentID", nullptr},
    {"e2nodeComponentConfigurationAck", nullptr}};
const asn1::table::seq_desc e2node_component_cfg_upd_ack_item_s::desc = {
    {seq_table_e2node_component_cfg_upd_ack_item_s_fields,
     uint64_t{0x7},
     nullptr,
     seq_table_e2node_component_cfg_upd_ack_item_s_names,
     3,
     0},
    true,
    true,
    nullptr,
    0};

// E2nodeComponentConfigUpdateAck-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_e2node_component_cfg_upd_ack_item_ies_o[]    = {36};
static const uint8_t  objset_crit_e2node_component_cfg_upd_ack_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_e2node_component_cfg_upd_ack_item_ies_o[] = {
    e2node_component_cfg_upd_ack_item_ies_o::value_c::types::e2node_component_cfg_upd_ack_item};
static const uint8_t objset_presence_e2node_component_cfg_upd_ack_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_e2node_component_cfg_upd_ack_item_ies_o[] = {
    {objset_crit_e2node_component_cfg_upd_ack_item_ies_o, crit_e::nulltype},
    {objset_value_e2node_component_cfg_upd_ack_item_ies_o,
     e2node_component_cfg_upd_ack_item_ies_o::value_c::types::nulltype},
    {objset_presence_e2node_component_cfg_upd_ack_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc e2node_component_cfg_upd_ack_item_ies_o::desc = {
    objset_id_e2node_component_cfg_upd_ack_item_ies_o,
    1,
    "id",
    objset_columns_e2node_component_cfg_upd_ack_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e2node_component_cfg_upd_ack_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_cfg_upd_ack_item_s::desc}};
static const asn1::table::field_name choice_table_e2node_component_cfg_upd_ack_item_ies_o_value_c_names[] = {
    {"E2nodeComponentConfigUpdateAck-Item", nullptr}};
const asn1::table::choice_desc e2node_component_cfg_upd_ack_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<e2node_component_cfg_upd_ack_item_ies_o::value_c::types>(),
    e2node_component_cfg_upd_ack_item_ies_o::value_c::types::nof_types,
    e2node_component_cfg_upd_ack_item_ies_o::value_c::types::nof_exts,
    e2node_component_cfg_upd_ack_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_e2node_component_cfg_upd_ack_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_e2node_component_cfg_upd_ack_item_ies_o_value_c_names,
    "e2node_component_cfg_upd_ack_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(e2node_component_cfg_upd_ack_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_e2node_component_cfg_upd_ack_item_ies_o_value_c_types_values[] = {
    "E2nodeComponentConfigUpdateAck-Item"};
const asn1::table::enum_desc e2node_component_cfg_upd_ack_item_ies_o::value_c::types_opts::desc = {
    enum_names_e2node_component_cfg_upd_ack_item_ies_o_value_c_types_values,
    1,
    "e2node_component_cfg_upd_ack_item_ies_o::value_c::types"};
uint8_t e2node_component_cfg_upd_ack_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2node_component_cfg_upd_ack_item_ies_o::value_c::types");
}

// E2nodeTNLassociationRemoval-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_tnl_assoc_removal_item_s_fields[] = {
    {offsetof(e2node_tnl_assoc_removal_item_s, tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tnl_info_s::desc},
    {offsetof(e2node_tnl_assoc_removal_item_s, tnl_info_ric),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tnl_info_s::desc}};
static const asn1::table::field_name seq_table_e2node_tnl_assoc_removal_item_s_names[] = {
    {"tnlInformation", nullptr},
    {"tnlInformationRIC", nullptr}};
const asn1::table::seq_desc e2node_tnl_assoc_removal_item_s::desc = {{seq_table_e2node_tnl_assoc_removal_item_s_fields,
                                                                      uint64_t{0x3},
                                                                      nullptr,
                                                                      seq_table_e2node_tnl_assoc_removal_item_s_names,
                                                                      2,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// E2nodeTNLassociationRemoval-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_e2node_tnl_assoc_removal_item_ies_o[]    = {59};
static const uint8_t  objset_crit_e2node_tnl_assoc_removal_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_e2node_tnl_assoc_removal_item_ies_o[] = {
    e2node_tnl_assoc_removal_item_ies_o::value_c::types::e2node_tnl_assoc_removal_item};
static const uint8_t                    objset_presence_e2node_tnl_assoc_removal_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_e2node_tnl_assoc_removal_item_ies_o[]  = {
    {objset_crit_e2node_tnl_assoc_removal_item_ies_o, crit_e::nulltype},
    {objset_value_e2node_tnl_assoc_removal_item_ies_o, e2node_tnl_assoc_removal_item_ies_o::value_c::types::nulltype},
    {objset_presence_e2node_tnl_assoc_removal_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc e2node_tnl_assoc_removal_item_ies_o::desc =
    {objset_id_e2node_tnl_assoc_removal_item_ies_o, 1, "id", objset_columns_e2node_tnl_assoc_removal_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e2node_tnl_assoc_removal_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_tnl_assoc_removal_item_s::desc}};
static const asn1::table::field_name choice_table_e2node_tnl_assoc_removal_item_ies_o_value_c_names[] = {
    {"E2nodeTNLassociationRemoval-Item", nullptr}};
const asn1::table::choice_desc e2node_tnl_assoc_removal_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<e2node_tnl_assoc_removal_item_ies_o::value_c::types>(),
    e2node_tnl_assoc_removal_item_ies_o::value_c::types::nof_types,
    e2node_tnl_assoc_removal_item_ies_o::value_c::types::nof_exts,
    e2node_tnl_assoc_removal_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_e2node_tnl_assoc_removal_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_e2node_tnl_assoc_removal_item_ies_o_value_c_names,
    "e2node_tnl_assoc_removal_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(e2node_tnl_assoc_removal_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_e2node_tnl_assoc_removal_item_ies_o_value_c_types_values[] = {
    "E2nodeTNLassociationRemoval-Item"};
const asn1::table::enum_desc e2node_tnl_assoc_removal_item_ies_o::value_c::types_opts::desc = {
    enum_names_e2node_tnl_assoc_removal_item_ies_o_value_c_types_values,
    1,
    "e2node_tnl_assoc_removal_item_ies_o::value_c::types"};
uint8_t e2node_tnl_assoc_removal_item_ies_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "e2node_tnl_assoc_removal_item_ies_o::value_c::types");
}

static const uint32_t objset_id_e2node_cfg_upd_ies_o[] = {49, 3, 50, 33, 54, 58};
static const uint8_t  objset_crit_e2node_cfg_upd_ies_o[] =
    {crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject};

// E2nodeConfigurationUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2node_cfg_upd_ies_container_values[] = {
    {offsetof(e2node_cfg_upd_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e2node_cfg_upd_ies_container, global_e2node_id),
     (int32_t)offsetof(e2node_cfg_upd_ies_container, global_e2node_id_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &global_e2node_id_c::desc},
    {offsetof(e2node_cfg_upd_ies_container, e2node_component_cfg_addition),
     (int32_t)offsetof(e2node_cfg_upd_ies_container, e2node_component_cfg_addition_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(e2node_cfg_upd_ies_container::e2node_component_cfg_addition),
                              asn1::table::struct_json>::value},
    {offsetof(e2node_cfg_upd_ies_container, e2node_component_cfg_upd),
     (int32_t)offsetof(e2node_cfg_upd_ies_container, e2node_component_cfg_upd_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(e2node_cfg_upd_ies_container::e2node_component_cfg_upd),
                              asn1::table::struct_json>::value},
    {offsetof(e2node_cfg_upd_ies_container, e2node_component_cfg_removal),
     (int32_t)offsetof(e2node_cfg_upd_ies_container, e2node_component_cfg_removal_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(e2node_cfg_upd_ies_container::e2node_component_cfg_removal),
                              asn1::table::struct_json>::value},
    {offsetof(e2node_cfg_upd_ies_container, e2node_tnl_assoc_removal),
     (int32_t)offsetof(e2node_cfg_upd_ies_container, e2node_tnl_assoc_removal_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::list_codec<decltype(e2node_cfg_upd_ies_container::e2node_tnl_assoc_removal),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_e2node_cfg_upd_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2node_cfg_upd_ies_container, global_e2node_id_present),
    (int32_t)offsetof(e2node_cfg_upd_ies_container, e2node_component_cfg_addition_present),
    (int32_t)offsetof(e2node_cfg_upd_ies_container, e2node_component_cfg_upd_present),
    (int32_t)offsetof(e2node_cfg_upd_ies_container, e2node_component_cfg_removal_present),
    (int32_t)offsetof(e2node_cfg_upd_ies_container, e2node_tnl_assoc_removal_present)};
const asn1::table::ie_container_desc e2node_cfg_upd_ies_container::desc = {
    objset_id_e2node_cfg_upd_ies_o,
    objset_crit_e2node_cfg_upd_ies_o,
    ie_table_e2node_cfg_upd_ies_container_values,
    ie_table_e2node_cfg_upd_ies_container_presences,
    6,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e2node_cfg_upd_ack_ies_o[]   = {49, 52, 35, 56};
static const uint8_t  objset_crit_e2node_cfg_upd_ack_ies_o[] = {crit_e::reject,
                                                                crit_e::reject,
                                                                crit_e::reject,
                                                                crit_e::reject};

// E2nodeConfigurationUpdateAcknowledge-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2node_cfg_upd_ack_ies_container_values[] = {
    {offsetof(e2node_cfg_upd_ack_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e2node_cfg_upd_ack_ies_container, e2node_component_cfg_addition_ack),
     (int32_t)offsetof(e2node_cfg_upd_ack_ies_container, e2node_component_cfg_addition_ack_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(e2node_cfg_upd_ack_ies_container::e2node_component_cfg_addition_ack),
                              asn1::table::struct_json>::value},
    {offsetof(e2node_cfg_upd_ack_ies_container, e2node_component_cfg_upd_ack),
     (int32_t)offsetof(e2node_cfg_upd_ack_ies_container, e2node_component_cfg_upd_ack_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(e2node_cfg_upd_ack_ies_container::e2node_component_cfg_upd_ack),
                              asn1::table::struct_json>::value},
    {offsetof(e2node_cfg_upd_ack_ies_container, e2node_component_cfg_removal_ack),
     (int32_t)offsetof(e2node_cfg_upd_ack_ies_container, e2node_component_cfg_removal_ack_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(e2node_cfg_upd_ack_ies_container::e2node_component_cfg_removal_ack),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_e2node_cfg_upd_ack_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2node_cfg_upd_ack_ies_container, e2node_component_cfg_addition_ack_present),
    (int32_t)offsetof(e2node_cfg_upd_ack_ies_container, e2node_component_cfg_upd_ack_present),
    (int32_t)offsetof(e2node_cfg_upd_ack_ies_container, e2node_component_cfg_removal_ack_present)};
const asn1::table::ie_container_desc e2node_cfg_upd_ack_ies_container::desc = {
    objset_id_e2node_cfg_upd_ack_ies_o,
    objset_crit_e2node_cfg_upd_ack_ies_o,
    ie_table_e2node_cfg_upd_ack_ies_container_values,
    ie_table_e2node_cfg_upd_ack_ies_container_presences,
    4,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e2node_cfg_upd_fail_ies_o[]   = {49, 1, 31, 2};
static const uint8_t  objset_crit_e2node_cfg_upd_fail_ies_o[] = {crit_e::reject,
                                                                 crit_e::ignore,
                                                                 crit_e::ignore,
                                                                 crit_e::ignore};

// E2nodeConfigurationUpdateFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2node_cfg_upd_fail_ies_container_values[] = {
    {offsetof(e2node_cfg_upd_fail_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e2node_cfg_upd_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(e2node_cfg_upd_fail_ies_container, time_to_wait),
     (int32_t)offsetof(e2node_cfg_upd_fail_ies_container, time_to_wait_present),
     asn1::table::enum_op<decltype(e2node_cfg_upd_fail_ies_container::time_to_wait)>(),
     decltype(e2node_cfg_upd_fail_ies_container::time_to_wait)::has_ext,
     false,
     decltype(e2node_cfg_upd_fail_ies_container::time_to_wait)::nof_types,
     decltype(e2node_cfg_upd_fail_ies_container::time_to_wait)::nof_exts,
     nullptr,
     &time_to_wait_opts::desc},
    {offsetof(e2node_cfg_upd_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(e2node_cfg_upd_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_e2node_cfg_upd_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2node_cfg_upd_fail_ies_container, time_to_wait_present),
    (int32_t)offsetof(e2node_cfg_upd_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc e2node_cfg_upd_fail_ies_container::desc = {
    objset_id_e2node_cfg_upd_fail_ies_o,
    objset_crit_e2node_cfg_upd_fail_ies_o,
    ie_table_e2node_cfg_upd_fail_ies_container_values,
    ie_table_e2node_cfg_upd_fail_ies_container_presences,
    4,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e2setup_fail_ies_o[]   = {49, 1, 31, 2, 48};
static const uint8_t  objset_crit_e2setup_fail_ies_o[] = {crit_e::reject,
                                                          crit_e::ignore,
                                                          crit_e::ignore,
                                                          crit_e::ignore,
                                                          crit_e::ignore};

// E2setupFailureIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2setup_fail_ies_container_values[] = {
    {offsetof(e2setup_fail_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e2setup_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(e2setup_fail_ies_container, time_to_wait),
     (int32_t)offsetof(e2setup_fail_ies_container, time_to_wait_present),
     asn1::table::enum_op<decltype(e2setup_fail_ies_container::time_to_wait)>(),
     decltype(e2setup_fail_ies_container::time_to_wait)::has_ext,
     false,
     decltype(e2setup_fail_ies_container::time_to_wait)::nof_types,
     decltype(e2setup_fail_ies_container::time_to_wait)::nof_exts,
     nullptr,
     &time_to_wait_opts::desc},
    {offsetof(e2setup_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(e2setup_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc},
    {offsetof(e2setup_fail_ies_container, tnl_info),
     (int32_t)offsetof(e2setup_fail_ies_container, tnl_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tnl_info_s::desc}};
static const int32_t ie_table_e2setup_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2setup_fail_ies_container, time_to_wait_present),
    (int32_t)offsetof(e2setup_fail_ies_container, crit_diagnostics_present),
    (int32_t)offsetof(e2setup_fail_ies_container, tnl_info_present)};
const asn1::table::ie_container_desc e2setup_fail_ies_container::desc = {objset_id_e2setup_fail_ies_o,
                                                                         objset_crit_e2setup_fail_ies_o,
                                                                         ie_table_e2setup_fail_ies_container_values,
                                                                         ie_table_e2setup_fail_ies_container_presences,
                                                                         5,
                                                                         2,
                                                                         0,
                                                                         65535,
                                                                         true,
                                                                         "value"};

// RANfunction-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_item_s_fields[] = {
    {offsetof(ran_function_item_s, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ran_function_item_s, ran_function_definition),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_function_item_s::ran_function_definition),
                                asn1::table::string_json>::value},
    {offsetof(ran_function_item_s, ran_function_revision),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ran_function_item_s, ran_function_o_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_function_item_s::ran_function_o_id), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_ran_function_item_s_names[] = {{"ranFunctionID", nullptr},
                                                                              {"ranFunctionDefinition", nullptr},
                                                                              {"ranFunctionRevision", nullptr},
                                                                              {"ranFunctionOID", nullptr}};
const asn1::table::seq_desc          ran_function_item_s::desc             = {
    {seq_table_ran_function_item_s_fields, uint64_t{0xf}, nullptr, seq_table_ran_function_item_s_names, 4, 0},
    true,
    true,
    nullptr,
    0};

// RANfunction-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ran_function_item_ies_o[]    = {8};
static const uint8_t  objset_crit_ran_function_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ran_function_item_ies_o[] = {
    ran_function_item_ies_o::value_c::types::ran_function_item};
static const uint8_t                    objset_presence_ran_function_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ran_function_item_ies_o[]  = {
    {objset_crit_ran_function_item_ies_o, crit_e::nulltype},
    {objset_value_ran_function_item_ies_o, ran_function_item_ies_o::value_c::types::nulltype},
    {objset_presence_ran_function_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ran_function_item_ies_o::desc = {objset_id_ran_function_item_ies_o,
                                                                1,
                                                                "id",
                                                                objset_columns_ran_function_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ran_function_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_function_item_s::desc}};
static const asn1::table::field_name choice_table_ran_function_item_ies_o_value_c_names[] = {
    {"RANfunction-Item", nullptr}};
const asn1::table::choice_desc ran_function_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ran_function_item_ies_o::value_c::types>(),
    ran_function_item_ies_o::value_c::types::nof_types,
    ran_function_item_ies_o::value_c::types::nof_exts,
    ran_function_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ran_function_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ran_function_item_ies_o_value_c_names,
    "ran_function_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ran_function_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_ran_function_item_ies_o_value_c_types_values[] = {"RANfunction-Item"};
const asn1::table::enum_desc ran_function_item_ies_o::value_c::types_opts::desc        = {
    enum_names_ran_function_item_ies_o_value_c_types_values,
    1,
    "ran_function_item_ies_o::value_c::types"};

static const uint32_t objset_id_e2setup_request_ies_o[]   = {49, 3, 10, 50};
static const uint8_t  objset_crit_e2setup_request_ies_o[] = {crit_e::reject,
                                                             crit_e::reject,
                                                             crit_e::reject,
                                                             crit_e::reject};

// E2setupRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2setup_request_ies_container_values[] = {
    {offsetof(e2setup_request_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e2setup_request_ies_container, global_e2node_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &global_e2node_id_c::desc},
    {offsetof(e2setup_request_ies_container, ran_functions_added),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(e2setup_request_ies_container::ran_functions_added),
                              asn1::table::struct_json>::value},
    {offsetof(e2setup_request_ies_container, e2node_component_cfg_addition),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(e2setup_request_ies_container::e2node_component_cfg_addition),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_e2setup_request_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc e2setup_request_ies_container::desc = {
    objset_id_e2setup_request_ies_o,
    objset_crit_e2setup_request_ies_o,
    ie_table_e2setup_request_ies_container_values,
    ie_table_e2setup_request_ies_container_presences,
    4,
    4,
    0,
    65535,
    true,
    "value"};

// RANfunctionID-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_id_item_s_fields[] = {
    {offsetof(ran_function_id_item_s, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ran_function_id_item_s, ran_function_revision),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr}};
static const asn1::table::field_name seq_table_ran_function_id_item_s_names[] = {{"ranFunctionID", nullptr},
                                                                                 {"ranFunctionRevision", nullptr}};
const asn1::table::seq_desc          ran_function_id_item_s::desc             = {
    {seq_table_ran_function_id_item_s_fields, uint64_t{0x3}, nullptr, seq_table_ran_function_id_item_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// RANfunctionID-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ran_function_id_item_ies_o[]    = {6};
static const uint8_t  objset_crit_ran_function_id_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ran_function_id_item_ies_o[] = {
    ran_function_id_item_ies_o::value_c::types::ran_function_id_item};
static const uint8_t                    objset_presence_ran_function_id_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ran_function_id_item_ies_o[]  = {
    {objset_crit_ran_function_id_item_ies_o, crit_e::nulltype},
    {objset_value_ran_function_id_item_ies_o, ran_function_id_item_ies_o::value_c::types::nulltype},
    {objset_presence_ran_function_id_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ran_function_id_item_ies_o::desc = {objset_id_ran_function_id_item_ies_o,
                                                                   1,
                                                                   "id",
                                                                   objset_columns_ran_function_id_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ran_function_id_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_function_id_item_s::desc}};
static const asn1::table::field_name choice_table_ran_function_id_item_ies_o_value_c_names[] = {
    {"RANfunctionID-Item", nullptr}};
const asn1::table::choice_desc ran_function_id_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ran_function_id_item_ies_o::value_c::types>(),
    ran_function_id_item_ies_o::value_c::types::nof_types,
    ran_function_id_item_ies_o::value_c::types::nof_exts,
    ran_function_id_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ran_function_id_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ran_function_id_item_ies_o_value_c_names,
    "ran_function_id_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ran_function_id_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_ran_function_id_item_ies_o_value_c_types_values[] = {"RANfunctionID-Item"};
const asn1::table::enum_desc ran_function_id_item_ies_o::value_c::types_opts::desc        = {
    enum_names_ran_function_id_item_ies_o_value_c_types_values,
    1,
    "ran_function_id_item_ies_o::value_c::types"};

// RANfunctionIDcause-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_id_cause_item_s_fields[] = {
    {offsetof(ran_function_id_cause_item_s, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ran_function_id_cause_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const asn1::table::field_name seq_table_ran_function_id_cause_item_s_names[] = {{"ranFunctionID", nullptr},
                                                                                       {"cause", nullptr}};
const asn1::table::seq_desc ran_function_id_cause_item_s::desc = {{seq_table_ran_function_id_cause_item_s_fields,
                                                                   uint64_t{0x3},
                                                                   nullptr,
                                                                   seq_table_ran_function_id_cause_item_s_names,
                                                                   2,
                                                                   0},
                                                                  true,
                                                                  true,
                                                                  nullptr,
                                                                  0};

// RANfunctionIDcause-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ran_function_id_cause_item_ies_o[]    = {7};
static const uint8_t  objset_crit_ran_function_id_cause_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ran_function_id_cause_item_ies_o[] = {
    ran_function_id_cause_item_ies_o::value_c::types::ran_function_i_ecause_item};
static const uint8_t                    objset_presence_ran_function_id_cause_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ran_function_id_cause_item_ies_o[]  = {
    {objset_crit_ran_function_id_cause_item_ies_o, crit_e::nulltype},
    {objset_value_ran_function_id_cause_item_ies_o, ran_function_id_cause_item_ies_o::value_c::types::nulltype},
    {objset_presence_ran_function_id_cause_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ran_function_id_cause_item_ies_o::desc =
    {objset_id_ran_function_id_cause_item_ies_o, 1, "id", objset_columns_ran_function_id_cause_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ran_function_id_cause_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_function_id_cause_item_s::desc}};
static const asn1::table::field_name choice_table_ran_function_id_cause_item_ies_o_value_c_names[] = {
    {"RANfunctionIDcause-Item", nullptr}};
const asn1::table::choice_desc ran_function_id_cause_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ran_function_id_cause_item_ies_o::value_c::types>(),
    ran_function_id_cause_item_ies_o::value_c::types::nof_types,
    ran_function_id_cause_item_ies_o::value_c::types::nof_exts,
    ran_function_id_cause_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ran_function_id_cause_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ran_function_id_cause_item_ies_o_value_c_names,
    "ran_function_id_cause_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ran_function_id_cause_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ran_function_id_cause_item_ies_o_value_c_types_values[]    = {"RANfunctionIDcause-Item"};
const asn1::table::enum_desc ran_function_id_cause_item_ies_o::value_c::types_opts::desc = {
    enum_names_ran_function_id_cause_item_ies_o_value_c_types_values,
    1,
    "ran_function_id_cause_item_ies_o::value_c::types"};

// RANfunction-AcceptModified-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_accept_modified_item_s_fields[] = {
    {offsetof(ran_function_accept_modified_item_s, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ran_function_accept_modified_item_s, rev_ran_function_definition),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_function_accept_modified_item_s::rev_ran_function_definition),
                                asn1::table::string_json>::value},
    {offsetof(ran_function_accept_modified_item_s, ran_function_revision),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr}};
static const asn1::table::field_name seq_table_ran_function_accept_modified_item_s_names[] = {
    {"ranFunctionID", nullptr},
    {"revRanFunctionDefinition", nullptr},
    {"ranFunctionRevision", nullptr}};
const asn1::table::seq_desc ran_function_accept_modified_item_s::desc = {
    {seq_table_ran_function_accept_modified_item_s_fields,
     uint64_t{0x7},
     nullptr,
     seq_table_ran_function_accept_modified_item_s_names,
     3,
     0},
    true,
    true,
    nullptr,
    0};

// RANfunction-AcceptModified-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ran_function_accept_modified_item_ies_o[]    = {139};
static const uint8_t  objset_crit_ran_function_accept_modified_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ran_function_accept_modified_item_ies_o[] = {
    ran_function_accept_modified_item_ies_o::value_c::types::ran_function_accept_modified_item};
static const uint8_t objset_presence_ran_function_accept_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ran_function_accept_modified_item_ies_o[] = {
    {objset_crit_ran_function_accept_modified_item_ies_o, crit_e::nulltype},
    {objset_value_ran_function_accept_modified_item_ies_o,
     ran_function_accept_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_ran_function_accept_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ran_function_accept_modified_item_ies_o::desc = {
    objset_id_ran_function_accept_modified_item_ies_o,
    1,
    "id",
    objset_columns_ran_function_accept_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ran_function_accept_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_function_accept_modified_item_s::desc}};
static const asn1::table::field_name choice_table_ran_function_accept_modified_item_ies_o_value_c_names[] = {
    {"RANfunction-AcceptModified-Item", nullptr}};
const asn1::table::choice_desc ran_function_accept_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ran_function_accept_modified_item_ies_o::value_c::types>(),
    ran_function_accept_modified_item_ies_o::value_c::types::nof_types,
    ran_function_accept_modified_item_ies_o::value_c::types::nof_exts,
    ran_function_accept_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ran_function_accept_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ran_function_accept_modified_item_ies_o_value_c_names,
    "ran_function_accept_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ran_function_accept_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ran_function_accept_modified_item_ies_o_value_c_types_values[] = {
    "RANfunction-AcceptModified-Item"};
const asn1::table::enum_desc ran_function_accept_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_ran_function_accept_modified_item_ies_o_value_c_types_values,
    1,
    "ran_function_accept_modified_item_ies_o::value_c::types"};

static const uint32_t objset_id_e2setup_resp_ies_o[] = {49, 4, 9, 13, 52, 138};
static const uint8_t  objset_crit_e2setup_resp_ies_o[] =
    {crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject};

// E2setupResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e2setup_resp_ies_container_values[] = {
    {offsetof(e2setup_resp_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e2setup_resp_ies_container, global_ric_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_ric_id_s::desc},
    {offsetof(e2setup_resp_ies_container, ran_functions_accepted),
     (int32_t)offsetof(e2setup_resp_ies_container, ran_functions_accepted_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(e2setup_resp_ies_container::ran_functions_accepted),
                              asn1::table::struct_json>::value},
    {offsetof(e2setup_resp_ies_container, ran_functions_rejected),
     (int32_t)offsetof(e2setup_resp_ies_container, ran_functions_rejected_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(e2setup_resp_ies_container::ran_functions_rejected),
                              asn1::table::struct_json>::value},
    {offsetof(e2setup_resp_ies_container, e2node_component_cfg_addition_ack),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(e2setup_resp_ies_container::e2node_component_cfg_addition_ack),
                              asn1::table::struct_json>::value},
    {offsetof(e2setup_resp_ies_container, ran_functions_accept_modified_list),
     (int32_t)offsetof(e2setup_resp_ies_container, ran_functions_accept_modified_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(e2setup_resp_ies_container::ran_functions_accept_modified_list),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_e2setup_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2setup_resp_ies_container, ran_functions_accepted_present),
    (int32_t)offsetof(e2setup_resp_ies_container, ran_functions_rejected_present),
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e2setup_resp_ies_container, ran_functions_accept_modified_list_present)};
const asn1::table::ie_container_desc e2setup_resp_ies_container::desc = {objset_id_e2setup_resp_ies_o,
                                                                         objset_crit_e2setup_resp_ies_o,
                                                                         ie_table_e2setup_resp_ies_container_values,
                                                                         ie_table_e2setup_resp_ies_container_presences,
                                                                         6,
                                                                         3,
                                                                         0,
                                                                         65535,
                                                                         true,
                                                                         "value"};

static const uint32_t objset_id_error_ind_ies_o[]   = {49, 29, 5, 1, 2};
static const uint8_t  objset_crit_error_ind_ies_o[] = {crit_e::reject,
                                                       crit_e::reject,
                                                       crit_e::reject,
                                                       crit_e::ignore,
                                                       crit_e::ignore};

// ErrorIndication-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_error_ind_ies_container_values[] = {
    {offsetof(error_ind_ies_container, transaction_id),
     (int32_t)offsetof(error_ind_ies_container, transaction_id_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(error_ind_ies_container, ric_request_id),
     (int32_t)offsetof(error_ind_ies_container, ric_request_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(error_ind_ies_container, ran_function_id),
     (int32_t)offsetof(error_ind_ies_container, ran_function_id_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(error_ind_ies_container, cause),
     (int32_t)offsetof(error_ind_ies_container, cause_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(error_ind_ies_container, crit_diagnostics),
     (int32_t)offsetof(error_ind_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_error_ind_ies_container_presences[] = {
    (int32_t)offsetof(error_ind_ies_container, transaction_id_present),
    (int32_t)offsetof(error_ind_ies_container, ric_request_id_present),
    (int32_t)offsetof(error_ind_ies_container, ran_function_id_present),
    (int32_t)offsetof(error_ind_ies_container, cause_present),
    (int32_t)offsetof(error_ind_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc error_ind_ies_container::desc = {objset_id_error_ind_ies_o,
                                                                      objset_crit_error_ind_ies_o,
                                                                      ie_table_error_ind_ies_container_values,
                                                                      ie_table_error_ind_ies_container_presences,
                                                                      5,
                                                                      0,
                                                                      0,
                                                                      65535,
                                                                      true,
                                                                      "value"};

// RICactionLoad-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_load_item_s_fields[] = {
    {offsetof(ric_action_load_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_load_item_s, ric_action_load_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_load_info_s::desc}};
static const asn1::table::field_name seq_table_ric_action_load_item_s_names[] = {{"ricActionID", nullptr},
                                                                                 {"ricActionLoadInformation", nullptr}};
const asn1::table::seq_desc          ric_action_load_item_s::desc             = {
    {seq_table_ric_action_load_item_s_fields, uint64_t{0x3}, nullptr, seq_table_ric_action_load_item_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// RICactionLoad-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_load_item_ies_o[]    = {114};
static const uint8_t  objset_crit_ric_action_load_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ric_action_load_item_ies_o[] = {
    ric_action_load_item_ies_o::value_c::types::ric_action_load_item};
static const uint8_t                    objset_presence_ric_action_load_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_load_item_ies_o[]  = {
    {objset_crit_ric_action_load_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_load_item_ies_o, ric_action_load_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_load_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_load_item_ies_o::desc = {objset_id_ric_action_load_item_ies_o,
                                                                   1,
                                                                   "id",
                                                                   objset_columns_ric_action_load_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_load_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_load_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_load_item_ies_o_value_c_names[] = {
    {"RICactionLoad-Item", nullptr}};
const asn1::table::choice_desc ric_action_load_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_load_item_ies_o::value_c::types>(),
    ric_action_load_item_ies_o::value_c::types::nof_types,
    ric_action_load_item_ies_o::value_c::types::nof_exts,
    ric_action_load_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_load_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_load_item_ies_o_value_c_names,
    "ric_action_load_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_load_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_ric_action_load_item_ies_o_value_c_types_values[] = {"RICactionLoad-Item"};
const asn1::table::enum_desc ric_action_load_item_ies_o::value_c::types_opts::desc        = {
    enum_names_ric_action_load_item_ies_o_value_c_types_values,
    1,
    "ric_action_load_item_ies_o::value_c::types"};

// RICsubscriptionLoad-ItemIE ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_sub_load_item_ie_s_fields[] = {
    {offsetof(ric_sub_load_item_ie_s, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_load_item_ie_s, ric_sub_load_info),
     (int32_t)offsetof(ric_sub_load_item_ie_s, ric_sub_load_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_load_info_s::desc},
    {offsetof(ric_sub_load_item_ie_s, ric_action_load_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     15,
     &asn1::table::list_codec<decltype(ric_sub_load_item_ie_s::ric_action_load_list),
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ric_sub_load_item_ie_s_names[] = {
    {"ricRequestID", nullptr},
    {"ricSubscriptionLoadInformation", nullptr},
    {"ricActionLoad-list", nullptr}};
static const asn1::table::seq_opt seq_table_ric_sub_load_item_ie_s_opts[] = {
    {1, (int32_t)offsetof(ric_sub_load_item_ie_s, ric_sub_load_info_present)},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ric_sub_load_item_ie_s::desc = {{seq_table_ric_sub_load_item_ie_s_fields,
                                                             uint64_t{0x1},
                                                             seq_table_ric_sub_load_item_ie_s_opts,
                                                             seq_table_ric_sub_load_item_ie_s_names,
                                                             3,
                                                             2},
                                                            true,
                                                            true,
                                                            nullptr,
                                                            0};

// RICsubscriptionLoad-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_sub_load_item_ies_o[]    = {118};
static const uint8_t  objset_crit_ric_sub_load_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ric_sub_load_item_ies_o[] = {
    ric_sub_load_item_ies_o::value_c::types::ric_sub_load_item};
static const uint8_t                    objset_presence_ric_sub_load_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_sub_load_item_ies_o[]  = {
    {objset_crit_ric_sub_load_item_ies_o, crit_e::nulltype},
    {objset_value_ric_sub_load_item_ies_o, ric_sub_load_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_sub_load_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_sub_load_item_ies_o::desc = {objset_id_ric_sub_load_item_ies_o,
                                                                1,
                                                                "id",
                                                                objset_columns_ric_sub_load_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_sub_load_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_sub_load_item_ie_s::desc}};
static const asn1::table::field_name choice_table_ric_sub_load_item_ies_o_value_c_names[] = {
    {"RICsubscriptionLoad-ItemIE", nullptr}};
const asn1::table::choice_desc ric_sub_load_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_sub_load_item_ies_o::value_c::types>(),
    ric_sub_load_item_ies_o::value_c::types::nof_types,
    ric_sub_load_item_ies_o::value_c::types::nof_exts,
    ric_sub_load_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_sub_load_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_sub_load_item_ies_o_value_c_names,
    "ric_sub_load_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_sub_load_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_ric_sub_load_item_ies_o_value_c_types_values[] = {"RICsubscriptionLoad-ItemIE"};
const asn1::table::enum_desc ric_sub_load_item_ies_o::value_c::types_opts::desc        = {
    enum_names_ric_sub_load_item_ies_o_value_c_types_values,
    1,
    "ric_sub_load_item_ies_o::value_c::types"};

// RANfunctionLoad-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_load_item_s_fields[] = {
    {offsetof(ran_function_load_item_s, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ran_function_load_item_s, ran_function_load_info),
     (int32_t)offsetof(ran_function_load_item_s, ran_function_load_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_load_info_s::desc},
    {offsetof(ran_function_load_item_s, ric_service_load_info),
     (int32_t)offsetof(ran_function_load_item_s, ric_service_load_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_service_load_info_s::desc},
    {offsetof(ran_function_load_item_s, ric_sub_load_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(ran_function_load_item_s::ric_sub_load_list), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ran_function_load_item_s_names[] = {
    {"ranFunctionID", nullptr},
    {"ranFunctionLoadInformation", nullptr},
    {"ricServiceLoadInformation", nullptr},
    {"ricSubscriptionLoad-list", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_load_item_s_opts[] = {
    {1, (int32_t)offsetof(ran_function_load_item_s, ran_function_load_info_present)},
    {2, (int32_t)offsetof(ran_function_load_item_s, ric_service_load_info_present)},
    {3, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_function_load_item_s::desc = {{seq_table_ran_function_load_item_s_fields,
                                                               uint64_t{0x1},
                                                               seq_table_ran_function_load_item_s_opts,
                                                               seq_table_ran_function_load_item_s_names,
                                                               4,
                                                               3},
                                                              true,
                                                              true,
                                                              nullptr,
                                                              0};

// RANfunctionLoad-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ran_function_load_item_ies_o[]    = {106};
static const uint8_t  objset_crit_ran_function_load_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ran_function_load_item_ies_o[] = {
    ran_function_load_item_ies_o::value_c::types::ran_function_load_item};
static const uint8_t                    objset_presence_ran_function_load_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ran_function_load_item_ies_o[]  = {
    {objset_crit_ran_function_load_item_ies_o, crit_e::nulltype},
    {objset_value_ran_function_load_item_ies_o, ran_function_load_item_ies_o::value_c::types::nulltype},
    {objset_presence_ran_function_load_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ran_function_load_item_ies_o::desc = {objset_id_ran_function_load_item_ies_o,
                                                                     1,
                                                                     "id",
                                                                     objset_columns_ran_function_load_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ran_function_load_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_function_load_item_s::desc}};
static const asn1::table::field_name choice_table_ran_function_load_item_ies_o_value_c_names[] = {
    {"RANfunctionLoad-Item", nullptr}};
const asn1::table::choice_desc ran_function_load_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ran_function_load_item_ies_o::value_c::types>(),
    ran_function_load_item_ies_o::value_c::types::nof_types,
    ran_function_load_item_ies_o::value_c::types::nof_exts,
    ran_function_load_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ran_function_load_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ran_function_load_item_ies_o_value_c_names,
    "ran_function_load_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ran_function_load_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_ran_function_load_item_ies_o_value_c_types_values[] = {"RANfunctionLoad-Item"};
const asn1::table::enum_desc ran_function_load_item_ies_o::value_c::types_opts::desc        = {
    enum_names_ran_function_load_item_ies_o_value_c_types_values,
    1,
    "ran_function_load_item_ies_o::value_c::types"};

// RICactionLoadConfirm-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_load_confirm_item_s_fields[] = {
    {offsetof(ric_action_load_confirm_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_load_confirm_item_s, ric_action_load_confirm),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ric_action_load_confirm_item_s::ric_action_load_confirm)>(),
     decltype(ric_action_load_confirm_item_s::ric_action_load_confirm)::has_ext,
     false,
     decltype(ric_action_load_confirm_item_s::ric_action_load_confirm)::nof_types,
     decltype(ric_action_load_confirm_item_s::ric_action_load_confirm)::nof_exts,
     nullptr,
     &ric_load_confirm_opts::desc}};
static const asn1::table::field_name seq_table_ric_action_load_confirm_item_s_names[] = {
    {"ricActionID", nullptr},
    {"ricActionLoadConfirm", nullptr}};
const asn1::table::seq_desc ric_action_load_confirm_item_s::desc = {{seq_table_ric_action_load_confirm_item_s_fields,
                                                                     uint64_t{0x3},
                                                                     nullptr,
                                                                     seq_table_ric_action_load_confirm_item_s_names,
                                                                     2,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// RICactionLoadConfirm-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_load_confirm_item_ies_o[]    = {115};
static const uint8_t  objset_crit_ric_action_load_confirm_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ric_action_load_confirm_item_ies_o[] = {
    ric_action_load_confirm_item_ies_o::value_c::types::ric_action_load_confirm_item};
static const uint8_t                    objset_presence_ric_action_load_confirm_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_load_confirm_item_ies_o[]  = {
    {objset_crit_ric_action_load_confirm_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_load_confirm_item_ies_o, ric_action_load_confirm_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_load_confirm_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_load_confirm_item_ies_o::desc =
    {objset_id_ric_action_load_confirm_item_ies_o, 1, "id", objset_columns_ric_action_load_confirm_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_load_confirm_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_load_confirm_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_load_confirm_item_ies_o_value_c_names[] = {
    {"RICactionLoadConfirm-Item", nullptr}};
const asn1::table::choice_desc ric_action_load_confirm_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_load_confirm_item_ies_o::value_c::types>(),
    ric_action_load_confirm_item_ies_o::value_c::types::nof_types,
    ric_action_load_confirm_item_ies_o::value_c::types::nof_exts,
    ric_action_load_confirm_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_load_confirm_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_load_confirm_item_ies_o_value_c_names,
    "ric_action_load_confirm_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_load_confirm_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_load_confirm_item_ies_o_value_c_types_values[] = {"RICactionLoadConfirm-Item"};
const asn1::table::enum_desc ric_action_load_confirm_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_load_confirm_item_ies_o_value_c_types_values,
    1,
    "ric_action_load_confirm_item_ies_o::value_c::types"};

// RICsubscriptionLoadConfirm-ItemIE ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_sub_load_confirm_item_ie_s_fields[] = {
    {offsetof(ric_sub_load_confirm_item_ie_s, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_load_confirm_item_ie_s, ric_sub_load_confirm),
     (int32_t)offsetof(ric_sub_load_confirm_item_ie_s, ric_sub_load_confirm_present),
     asn1::table::enum_op<decltype(ric_sub_load_confirm_item_ie_s::ric_sub_load_confirm)>(),
     decltype(ric_sub_load_confirm_item_ie_s::ric_sub_load_confirm)::has_ext,
     false,
     decltype(ric_sub_load_confirm_item_ie_s::ric_sub_load_confirm)::nof_types,
     decltype(ric_sub_load_confirm_item_ie_s::ric_sub_load_confirm)::nof_exts,
     nullptr,
     &ric_load_confirm_opts::desc},
    {offsetof(ric_sub_load_confirm_item_ie_s, ric_action_load_confirm_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     15,
     &asn1::table::list_codec<decltype(ric_sub_load_confirm_item_ie_s::ric_action_load_confirm_list),
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ric_sub_load_confirm_item_ie_s_names[] = {
    {"ricRequestID", nullptr},
    {"ricSubscriptionLoadConfirm", nullptr},
    {"ricActionLoadConfirm-list", nullptr}};
static const asn1::table::seq_opt seq_table_ric_sub_load_confirm_item_ie_s_opts[] = {
    {1, (int32_t)offsetof(ric_sub_load_confirm_item_ie_s, ric_sub_load_confirm_present)},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ric_sub_load_confirm_item_ie_s::desc = {{seq_table_ric_sub_load_confirm_item_ie_s_fields,
                                                                     uint64_t{0x1},
                                                                     seq_table_ric_sub_load_confirm_item_ie_s_opts,
                                                                     seq_table_ric_sub_load_confirm_item_ie_s_names,
                                                                     3,
                                                                     2},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// RICsubscriptionLoadConfirm-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_sub_load_confirm_item_ies_o[]    = {119};
static const uint8_t  objset_crit_ric_sub_load_confirm_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ric_sub_load_confirm_item_ies_o[] = {
    ric_sub_load_confirm_item_ies_o::value_c::types::ric_sub_load_confirm_item};
static const uint8_t                    objset_presence_ric_sub_load_confirm_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_sub_load_confirm_item_ies_o[]  = {
    {objset_crit_ric_sub_load_confirm_item_ies_o, crit_e::nulltype},
    {objset_value_ric_sub_load_confirm_item_ies_o, ric_sub_load_confirm_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_sub_load_confirm_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_sub_load_confirm_item_ies_o::desc = {objset_id_ric_sub_load_confirm_item_ies_o,
                                                                        1,
                                                                        "id",
                                                                        objset_columns_ric_sub_load_confirm_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_sub_load_confirm_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_sub_load_confirm_item_ie_s::desc}};
static const asn1::table::field_name choice_table_ric_sub_load_confirm_item_ies_o_value_c_names[] = {
    {"RICsubscriptionLoadConfirm-ItemIE", nullptr}};
const asn1::table::choice_desc ric_sub_load_confirm_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_sub_load_confirm_item_ies_o::value_c::types>(),
    ric_sub_load_confirm_item_ies_o::value_c::types::nof_types,
    ric_sub_load_confirm_item_ies_o::value_c::types::nof_exts,
    ric_sub_load_confirm_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_sub_load_confirm_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_sub_load_confirm_item_ies_o_value_c_names,
    "ric_sub_load_confirm_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_sub_load_confirm_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_sub_load_confirm_item_ies_o_value_c_types_values[] = {
    "RICsubscriptionLoadConfirm-ItemIE"};
const asn1::table::enum_desc ric_sub_load_confirm_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_sub_load_confirm_item_ies_o_value_c_types_values,
    1,
    "ric_sub_load_confirm_item_ies_o::value_c::types"};

// RANfunctionLoadConfirm-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_load_confirm_item_s_fields[] = {
    {offsetof(ran_function_load_confirm_item_s, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ran_function_load_confirm_item_s, ran_function_load_confirm),
     (int32_t)offsetof(ran_function_load_confirm_item_s, ran_function_load_confirm_present),
     asn1::table::enum_op<decltype(ran_function_load_confirm_item_s::ran_function_load_confirm)>(),
     decltype(ran_function_load_confirm_item_s::ran_function_load_confirm)::has_ext,
     false,
     decltype(ran_function_load_confirm_item_s::ran_function_load_confirm)::nof_types,
     decltype(ran_function_load_confirm_item_s::ran_function_load_confirm)::nof_exts,
     nullptr,
     &ric_load_confirm_opts::desc},
    {offsetof(ran_function_load_confirm_item_s, ric_service_load_confirm),
     (int32_t)offsetof(ran_function_load_confirm_item_s, ric_service_load_confirm_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_service_load_confirm_s::desc},
    {offsetof(ran_function_load_confirm_item_s, ric_sub_load_confirm_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(ran_function_load_confirm_item_s::ric_sub_load_confirm_list),
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ran_function_load_confirm_item_s_names[] = {
    {"ranFunctionID", nullptr},
    {"ranFunctionLoadConfirm", nullptr},
    {"ricServiceLoadConfirm", nullptr},
    {"ricSubscriptionLoadConfirm-list", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_load_confirm_item_s_opts[] = {
    {1, (int32_t)offsetof(ran_function_load_confirm_item_s, ran_function_load_confirm_present)},
    {2, (int32_t)offsetof(ran_function_load_confirm_item_s, ric_service_load_confirm_present)},
    {3, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_function_load_confirm_item_s::desc = {
    {seq_table_ran_function_load_confirm_item_s_fields,
     uint64_t{0x1},
     seq_table_ran_function_load_confirm_item_s_opts,
     seq_table_ran_function_load_confirm_item_s_names,
     4,
     3},
    true,
    true,
    nullptr,
    0};

// RANfunctionLoadConfirm-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ran_function_load_confirm_item_ies_o[]    = {108};
static const uint8_t  objset_crit_ran_function_load_confirm_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ran_function_load_confirm_item_ies_o[] = {
    ran_function_load_confirm_item_ies_o::value_c::types::ran_function_load_confirm_item};
static const uint8_t objset_presence_ran_function_load_confirm_item_ies_o[]                   = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ran_function_load_confirm_item_ies_o[] = {
    {objset_crit_ran_function_load_confirm_item_ies_o, crit_e::nulltype},
    {objset_value_ran_function_load_confirm_item_ies_o, ran_function_load_confirm_item_ies_o::value_c::types::nulltype},
    {objset_presence_ran_function_load_confirm_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ran_function_load_confirm_item_ies_o::desc =
    {objset_id_ran_function_load_confirm_item_ies_o, 1, "id", objset_columns_ran_function_load_confirm_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ran_function_load_confirm_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_function_load_confirm_item_s::desc}};
static const asn1::table::field_name choice_table_ran_function_load_confirm_item_ies_o_value_c_names[] = {
    {"RANfunctionLoadConfirm-Item", nullptr}};
const asn1::table::choice_desc ran_function_load_confirm_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ran_function_load_confirm_item_ies_o::value_c::types>(),
    ran_function_load_confirm_item_ies_o::value_c::types::nof_types,
    ran_function_load_confirm_item_ies_o::value_c::types::nof_exts,
    ran_function_load_confirm_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ran_function_load_confirm_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ran_function_load_confirm_item_ies_o_value_c_names,
    "ran_function_load_confirm_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ran_function_load_confirm_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ran_function_load_confirm_item_ies_o_value_c_types_values[] = {
    "RANfunctionLoadConfirm-Item"};
const asn1::table::enum_desc ran_function_load_confirm_item_ies_o::value_c::types_opts::desc = {
    enum_names_ran_function_load_confirm_item_ies_o_value_c_types_values,
    1,
    "ran_function_load_confirm_item_ies_o::value_c::types"};

// RICactionLoadRequest-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_load_request_item_s_fields[] = {
    {offsetof(ric_action_load_request_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_load_request_item_s, ric_action_load_request),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ric_action_load_request_item_s::ric_action_load_request)>(),
     decltype(ric_action_load_request_item_s::ric_action_load_request)::has_ext,
     false,
     decltype(ric_action_load_request_item_s::ric_action_load_request)::nof_types,
     decltype(ric_action_load_request_item_s::ric_action_load_request)::nof_exts,
     nullptr,
     &ric_load_request_opts::desc}};
static const asn1::table::field_name seq_table_ric_action_load_request_item_s_names[] = {
    {"ricActionID", nullptr},
    {"ricActionLoadRequest", nullptr}};
const asn1::table::seq_desc ric_action_load_request_item_s::desc = {{seq_table_ric_action_load_request_item_s_fields,
                                                                     uint64_t{0x3},
                                                                     nullptr,
                                                                     seq_table_ric_action_load_request_item_s_names,
                                                                     2,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// RICactionLoadRequest-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_load_request_item_ies_o[]    = {116};
static const uint8_t  objset_crit_ric_action_load_request_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ric_action_load_request_item_ies_o[] = {
    ric_action_load_request_item_ies_o::value_c::types::ric_action_load_request_item};
static const uint8_t                    objset_presence_ric_action_load_request_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_load_request_item_ies_o[]  = {
    {objset_crit_ric_action_load_request_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_load_request_item_ies_o, ric_action_load_request_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_load_request_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_load_request_item_ies_o::desc =
    {objset_id_ric_action_load_request_item_ies_o, 1, "id", objset_columns_ric_action_load_request_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_load_request_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_load_request_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_load_request_item_ies_o_value_c_names[] = {
    {"RICactionLoadRequest-Item", nullptr}};
const asn1::table::choice_desc ric_action_load_request_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_load_request_item_ies_o::value_c::types>(),
    ric_action_load_request_item_ies_o::value_c::types::nof_types,
    ric_action_load_request_item_ies_o::value_c::types::nof_exts,
    ric_action_load_request_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_load_request_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_load_request_item_ies_o_value_c_names,
    "ric_action_load_request_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_load_request_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_load_request_item_ies_o_value_c_types_values[] = {"RICactionLoadRequest-Item"};
const asn1::table::enum_desc ric_action_load_request_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_load_request_item_ies_o_value_c_types_values,
    1,
    "ric_action_load_request_item_ies_o::value_c::types"};

// RICsubscriptionLoadRequest-ItemIE ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_sub_load_request_item_ie_s_fields[] = {
    {offsetof(ric_sub_load_request_item_ie_s, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_load_request_item_ie_s, ric_sub_load_request),
     (int32_t)offsetof(ric_sub_load_request_item_ie_s, ric_sub_load_request_present),
     asn1::table::enum_op<decltype(ric_sub_load_request_item_ie_s::ric_sub_load_request)>(),
     decltype(ric_sub_load_request_item_ie_s::ric_sub_load_request)::has_ext,
     false,
     decltype(ric_sub_load_request_item_ie_s::ric_sub_load_request)::nof_types,
     decltype(ric_sub_load_request_item_ie_s::ric_sub_load_request)::nof_exts,
     nullptr,
     &ric_load_request_opts::desc},
    {offsetof(ric_sub_load_request_item_ie_s, ric_action_load_request_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     15,
     &asn1::table::list_codec<decltype(ric_sub_load_request_item_ie_s::ric_action_load_request_list),
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ric_sub_load_request_item_ie_s_names[] = {
    {"ricRequestID", nullptr},
    {"ricSubscriptionLoadRequest", nullptr},
    {"ricActionLoadRequest-list", nullptr}};
static const asn1::table::seq_opt seq_table_ric_sub_load_request_item_ie_s_opts[] = {
    {1, (int32_t)offsetof(ric_sub_load_request_item_ie_s, ric_sub_load_request_present)},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ric_sub_load_request_item_ie_s::desc = {{seq_table_ric_sub_load_request_item_ie_s_fields,
                                                                     uint64_t{0x1},
                                                                     seq_table_ric_sub_load_request_item_ie_s_opts,
                                                                     seq_table_ric_sub_load_request_item_ie_s_names,
                                                                     3,
                                                                     2},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// RICsubscriptionLoadRequest-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_sub_load_request_item_ies_o[]    = {120};
static const uint8_t  objset_crit_ric_sub_load_request_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ric_sub_load_request_item_ies_o[] = {
    ric_sub_load_request_item_ies_o::value_c::types::ric_sub_load_request_item};
static const uint8_t                    objset_presence_ric_sub_load_request_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_sub_load_request_item_ies_o[]  = {
    {objset_crit_ric_sub_load_request_item_ies_o, crit_e::nulltype},
    {objset_value_ric_sub_load_request_item_ies_o, ric_sub_load_request_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_sub_load_request_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_sub_load_request_item_ies_o::desc = {objset_id_ric_sub_load_request_item_ies_o,
                                                                        1,
                                                                        "id",
                                                                        objset_columns_ric_sub_load_request_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_sub_load_request_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_sub_load_request_item_ie_s::desc}};
static const asn1::table::field_name choice_table_ric_sub_load_request_item_ies_o_value_c_names[] = {
    {"RICsubscriptionLoadRequest-ItemIE", nullptr}};
const asn1::table::choice_desc ric_sub_load_request_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_sub_load_request_item_ies_o::value_c::types>(),
    ric_sub_load_request_item_ies_o::value_c::types::nof_types,
    ric_sub_load_request_item_ies_o::value_c::types::nof_exts,
    ric_sub_load_request_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_sub_load_request_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_sub_load_request_item_ies_o_value_c_names,
    "ric_sub_load_request_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_sub_load_request_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_sub_load_request_item_ies_o_value_c_types_values[] = {
    "RICsubscriptionLoadRequest-ItemIE"};
const asn1::table::enum_desc ric_sub_load_request_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_sub_load_request_item_ies_o_value_c_types_values,
    1,
    "ric_sub_load_request_item_ies_o::value_c::types"};

// RANfunctionLoadRequest-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_load_request_item_s_fields[] = {
    {offsetof(ran_function_load_request_item_s, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ran_function_load_request_item_s, ran_function_load_request),
     (int32_t)offsetof(ran_function_load_request_item_s, ran_function_load_request_present),
     asn1::table::enum_op<decltype(ran_function_load_request_item_s::ran_function_load_request)>(),
     decltype(ran_function_load_request_item_s::ran_function_load_request)::has_ext,
     false,
     decltype(ran_function_load_request_item_s::ran_function_load_request)::nof_types,
     decltype(ran_function_load_request_item_s::ran_function_load_request)::nof_exts,
     nullptr,
     &ric_load_request_opts::desc},
    {offsetof(ran_function_load_request_item_s, ric_service_load_request),
     (int32_t)offsetof(ran_function_load_request_item_s, ric_service_load_request_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_service_load_request_s::desc},
    {offsetof(ran_function_load_request_item_s, ric_sub_load_request_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(ran_function_load_request_item_s::ric_sub_load_request_list),
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ran_function_load_request_item_s_names[] = {
    {"ranFunctionID", nullptr},
    {"ranFunctionLoadRequest", nullptr},
    {"ricServiceLoadRequest", nullptr},
    {"ricSubscriptionLoadRequest-list", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_load_request_item_s_opts[] = {
    {1, (int32_t)offsetof(ran_function_load_request_item_s, ran_function_load_request_present)},
    {2, (int32_t)offsetof(ran_function_load_request_item_s, ric_service_load_request_present)},
    {3, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_function_load_request_item_s::desc = {
    {seq_table_ran_function_load_request_item_s_fields,
     uint64_t{0x1},
     seq_table_ran_function_load_request_item_s_opts,
     seq_table_ran_function_load_request_item_s_names,
     4,
     3},
    true,
    true,
    nullptr,
    0};

// RANfunctionLoadRequest-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ran_function_load_request_item_ies_o[]    = {110};
static const uint8_t  objset_crit_ran_function_load_request_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ran_function_load_request_item_ies_o[] = {
    ran_function_load_request_item_ies_o::value_c::types::ran_function_load_request_item};
static const uint8_t objset_presence_ran_function_load_request_item_ies_o[]                   = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ran_function_load_request_item_ies_o[] = {
    {objset_crit_ran_function_load_request_item_ies_o, crit_e::nulltype},
    {objset_value_ran_function_load_request_item_ies_o, ran_function_load_request_item_ies_o::value_c::types::nulltype},
    {objset_presence_ran_function_load_request_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ran_function_load_request_item_ies_o::desc =
    {objset_id_ran_function_load_request_item_ies_o, 1, "id", objset_columns_ran_function_load_request_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ran_function_load_request_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_function_load_request_item_s::desc}};
static const asn1::table::field_name choice_table_ran_function_load_request_item_ies_o_value_c_names[] = {
    {"RANfunctionLoadRequest-Item", nullptr}};
const asn1::table::choice_desc ran_function_load_request_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ran_function_load_request_item_ies_o::value_c::types>(),
    ran_function_load_request_item_ies_o::value_c::types::nof_types,
    ran_function_load_request_item_ies_o::value_c::types::nof_exts,
    ran_function_load_request_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ran_function_load_request_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ran_function_load_request_item_ies_o_value_c_names,
    "ran_function_load_request_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ran_function_load_request_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ran_function_load_request_item_ies_o_value_c_types_values[] = {
    "RANfunctionLoadRequest-Item"};
const asn1::table::enum_desc ran_function_load_request_item_ies_o::value_c::types_opts::desc = {
    enum_names_ran_function_load_request_item_ies_o_value_c_types_values,
    1,
    "ran_function_load_request_item_ies_o::value_c::types"};

// RICactionList-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_list_item_s_fields[] = {
    {offsetof(ric_action_list_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr}};
static const asn1::table::field_name seq_table_ric_action_list_item_s_names[] = {{"ricActionID", nullptr}};
const asn1::table::seq_desc          ric_action_list_item_s::desc             = {
    {seq_table_ric_action_list_item_s_fields, uint64_t{0x1}, nullptr, seq_table_ric_action_list_item_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// RICaction-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_item_ies_o[]    = {125};
static const uint8_t  objset_crit_ric_action_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ric_action_item_ies_o[] = {
    ric_action_item_ies_o::value_c::types::ric_action_list_item};
static const uint8_t                    objset_presence_ric_action_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_item_ies_o[]  = {
    {objset_crit_ric_action_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_item_ies_o, ric_action_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_item_ies_o::desc = {objset_id_ric_action_item_ies_o,
                                                              1,
                                                              "id",
                                                              objset_columns_ric_action_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_list_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_item_ies_o_value_c_names[] = {
    {"RICactionList-Item", nullptr}};
const asn1::table::choice_desc ric_action_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_item_ies_o::value_c::types>(),
    ric_action_item_ies_o::value_c::types::nof_types,
    ric_action_item_ies_o::value_c::types::nof_exts,
    ric_action_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_item_ies_o_value_c_names,
    "ric_action_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_ric_action_item_ies_o_value_c_types_values[] = {"RICactionList-Item"};
const asn1::table::enum_desc ric_action_item_ies_o::value_c::types_opts::desc        = {
    enum_names_ric_action_item_ies_o_value_c_types_values,
    1,
    "ric_action_item_ies_o::value_c::types"};

// RICsubscriptionList-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_sub_list_item_s_fields[] = {
    {offsetof(ric_sub_list_item_s, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_list_item_s, ric_action_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     15,
     &asn1::table::list_codec<decltype(ric_sub_list_item_s::ric_action_list), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ric_sub_list_item_s_names[] = {{"ricRequestID", nullptr},
                                                                              {"ricAction-list", nullptr}};
static const asn1::table::seq_opt seq_table_ric_sub_list_item_s_opts[] = {{1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       ric_sub_list_item_s::desc            = {{seq_table_ric_sub_list_item_s_fields,
                                                                           uint64_t{0x1},
                                                                           seq_table_ric_sub_list_item_s_opts,
                                                                           seq_table_ric_sub_list_item_s_names,
                                                                           2,
                                                                           1},
                                                                          true,
                                                                          true,
                                                                          nullptr,
                                                                          0};

// RICsubscriptionList-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_sub_list_item_ies_o[]    = {126};
static const uint8_t  objset_crit_ric_sub_list_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ric_sub_list_item_ies_o[] = {
    ric_sub_list_item_ies_o::value_c::types::ric_sub_list_item};
static const uint8_t                    objset_presence_ric_sub_list_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_sub_list_item_ies_o[]  = {
    {objset_crit_ric_sub_list_item_ies_o, crit_e::nulltype},
    {objset_value_ric_sub_list_item_ies_o, ric_sub_list_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_sub_list_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_sub_list_item_ies_o::desc = {objset_id_ric_sub_list_item_ies_o,
                                                                1,
                                                                "id",
                                                                objset_columns_ric_sub_list_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_sub_list_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_sub_list_item_s::desc}};
static const asn1::table::field_name choice_table_ric_sub_list_item_ies_o_value_c_names[] = {
    {"RICsubscriptionList-Item", nullptr}};
const asn1::table::choice_desc ric_sub_list_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_sub_list_item_ies_o::value_c::types>(),
    ric_sub_list_item_ies_o::value_c::types::nof_types,
    ric_sub_list_item_ies_o::value_c::types::nof_exts,
    ric_sub_list_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_sub_list_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_sub_list_item_ies_o_value_c_names,
    "ric_sub_list_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_sub_list_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_ric_sub_list_item_ies_o_value_c_types_values[] = {"RICsubscriptionList-Item"};
const asn1::table::enum_desc ric_sub_list_item_ies_o::value_c::types_opts::desc        = {
    enum_names_ric_sub_list_item_ies_o_value_c_types_values,
    1,
    "ric_sub_list_item_ies_o::value_c::types"};

// RANfunctionStateConfirm-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_state_confirm_item_s_fields[] = {
    {offsetof(ran_function_state_confirm_item_s, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ran_function_state_confirm_item_s, ric_sub_suspended_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(ran_function_state_confirm_item_s::ric_sub_suspended_list),
                              asn1::table::struct_json>::value},
    {offsetof(ran_function_state_confirm_item_s, ric_sub_resumed_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(ran_function_state_confirm_item_s::ric_sub_resumed_list),
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ran_function_state_confirm_item_s_names[] = {
    {"ranFunctionID", nullptr},
    {"ricSubscriptionSuspended-list", nullptr},
    {"ricSubscriptionResumed-list", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_state_confirm_item_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_function_state_confirm_item_s::desc = {
    {seq_table_ran_function_state_confirm_item_s_fields,
     uint64_t{0x1},
     seq_table_ran_function_state_confirm_item_s_opts,
     seq_table_ran_function_state_confirm_item_s_names,
     3,
     2},
    true,
    true,
    nullptr,
    0};

// RANfunctionStateConfirm-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ran_function_state_confirm_item_ies_o[]    = {121};
static const uint8_t  objset_crit_ran_function_state_confirm_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ran_function_state_confirm_item_ies_o[] = {
    ran_function_state_confirm_item_ies_o::value_c::types::ran_function_state_confirm_item};
static const uint8_t objset_presence_ran_function_state_confirm_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ran_function_state_confirm_item_ies_o[] = {
    {objset_crit_ran_function_state_confirm_item_ies_o, crit_e::nulltype},
    {objset_value_ran_function_state_confirm_item_ies_o,
     ran_function_state_confirm_item_ies_o::value_c::types::nulltype},
    {objset_presence_ran_function_state_confirm_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ran_function_state_confirm_item_ies_o::desc =
    {objset_id_ran_function_state_confirm_item_ies_o, 1, "id", objset_columns_ran_function_state_confirm_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ran_function_state_confirm_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_function_state_confirm_item_s::desc}};
static const asn1::table::field_name choice_table_ran_function_state_confirm_item_ies_o_value_c_names[] = {
    {"RANfunctionStateConfirm-Item", nullptr}};
const asn1::table::choice_desc ran_function_state_confirm_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ran_function_state_confirm_item_ies_o::value_c::types>(),
    ran_function_state_confirm_item_ies_o::value_c::types::nof_types,
    ran_function_state_confirm_item_ies_o::value_c::types::nof_exts,
    ran_function_state_confirm_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ran_function_state_confirm_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ran_function_state_confirm_item_ies_o_value_c_names,
    "ran_function_state_confirm_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ran_function_state_confirm_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ran_function_state_confirm_item_ies_o_value_c_types_values[] = {
    "RANfunctionStateConfirm-Item"};
const asn1::table::enum_desc ran_function_state_confirm_item_ies_o::value_c::types_opts::desc = {
    enum_names_ran_function_state_confirm_item_ies_o_value_c_types_values,
    1,
    "ran_function_state_confirm_item_ies_o::value_c::types"};

// RANfunctionStateControl-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_state_ctrl_item_s_fields[] = {
    {offsetof(ran_function_state_ctrl_item_s, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ran_function_state_ctrl_item_s, ric_sub_to_be_suspended_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(ran_function_state_ctrl_item_s::ric_sub_to_be_suspended_list),
                              asn1::table::struct_json>::value},
    {offsetof(ran_function_state_ctrl_item_s, ric_sub_to_be_resumed_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(ran_function_state_ctrl_item_s::ric_sub_to_be_resumed_list),
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ran_function_state_ctrl_item_s_names[] = {
    {"ranFunctionID", nullptr},
    {"ricSubscriptionToBeSuspended-list", nullptr},
    {"ricSubscriptionToBeResumed-list", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_state_ctrl_item_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_function_state_ctrl_item_s::desc = {{seq_table_ran_function_state_ctrl_item_s_fields,
                                                                     uint64_t{0x1},
                                                                     seq_table_ran_function_state_ctrl_item_s_opts,
                                                                     seq_table_ran_function_state_ctrl_item_s_names,
                                                                     3,
                                                                     2},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// RANfunctionStateControl-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ran_function_state_ctrl_item_ies_o[]    = {123};
static const uint8_t  objset_crit_ran_function_state_ctrl_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ran_function_state_ctrl_item_ies_o[] = {
    ran_function_state_ctrl_item_ies_o::value_c::types::ran_function_state_ctrl_item};
static const uint8_t                    objset_presence_ran_function_state_ctrl_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ran_function_state_ctrl_item_ies_o[]  = {
    {objset_crit_ran_function_state_ctrl_item_ies_o, crit_e::nulltype},
    {objset_value_ran_function_state_ctrl_item_ies_o, ran_function_state_ctrl_item_ies_o::value_c::types::nulltype},
    {objset_presence_ran_function_state_ctrl_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ran_function_state_ctrl_item_ies_o::desc =
    {objset_id_ran_function_state_ctrl_item_ies_o, 1, "id", objset_columns_ran_function_state_ctrl_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ran_function_state_ctrl_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_function_state_ctrl_item_s::desc}};
static const asn1::table::field_name choice_table_ran_function_state_ctrl_item_ies_o_value_c_names[] = {
    {"RANfunctionStateControl-Item", nullptr}};
const asn1::table::choice_desc ran_function_state_ctrl_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ran_function_state_ctrl_item_ies_o::value_c::types>(),
    ran_function_state_ctrl_item_ies_o::value_c::types::nof_types,
    ran_function_state_ctrl_item_ies_o::value_c::types::nof_exts,
    ran_function_state_ctrl_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ran_function_state_ctrl_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ran_function_state_ctrl_item_ies_o_value_c_names,
    "ran_function_state_ctrl_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ran_function_state_ctrl_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ran_function_state_ctrl_item_ies_o_value_c_types_values[] = {
    "RANfunctionStateControl-Item"};
const asn1::table::enum_desc ran_function_state_ctrl_item_ies_o::value_c::types_opts::desc = {
    enum_names_ran_function_state_ctrl_item_ies_o_value_c_types_values,
    1,
    "ran_function_state_ctrl_item_ies_o::value_c::types"};

// RICaction-AddedForModification-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_added_for_mod_item_s_fields[] = {
    {offsetof(ric_action_added_for_mod_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr}};
static const asn1::table::field_name seq_table_ric_action_added_for_mod_item_s_names[] = {{"ricActionID", nullptr}};
const asn1::table::seq_desc ric_action_added_for_mod_item_s::desc = {{seq_table_ric_action_added_for_mod_item_s_fields,
                                                                      uint64_t{0x1},
                                                                      nullptr,
                                                                      seq_table_ric_action_added_for_mod_item_s_names,
                                                                      1,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// RICaction-AddedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_added_for_mod_item_ies_o[]    = {80};
static const uint8_t  objset_crit_ric_action_added_for_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_added_for_mod_item_ies_o[] = {
    ric_action_added_for_mod_item_ies_o::value_c::types::ric_action_added_for_mod_item};
static const uint8_t                    objset_presence_ric_action_added_for_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_added_for_mod_item_ies_o[]  = {
    {objset_crit_ric_action_added_for_mod_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_added_for_mod_item_ies_o, ric_action_added_for_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_added_for_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_added_for_mod_item_ies_o::desc =
    {objset_id_ric_action_added_for_mod_item_ies_o, 1, "id", objset_columns_ric_action_added_for_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_added_for_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_added_for_mod_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_added_for_mod_item_ies_o_value_c_names[] = {
    {"RICaction-AddedForModification-Item", nullptr}};
const asn1::table::choice_desc ric_action_added_for_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_added_for_mod_item_ies_o::value_c::types>(),
    ric_action_added_for_mod_item_ies_o::value_c::types::nof_types,
    ric_action_added_for_mod_item_ies_o::value_c::types::nof_exts,
    ric_action_added_for_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_added_for_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_added_for_mod_item_ies_o_value_c_names,
    "ric_action_added_for_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_added_for_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_added_for_mod_item_ies_o_value_c_types_values[] = {
    "RICaction-AddedForModification-Item"};
const asn1::table::enum_desc ric_action_added_for_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_added_for_mod_item_ies_o_value_c_types_values,
    1,
    "ric_action_added_for_mod_item_ies_o::value_c::types"};

// RICaction-Admitted-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_admitted_item_s_fields[] = {
    {offsetof(ric_action_admitted_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr}};
static const asn1::table::field_name seq_table_ric_action_admitted_item_s_names[] = {{"ricActionID", nullptr}};
const asn1::table::seq_desc          ric_action_admitted_item_s::desc = {{seq_table_ric_action_admitted_item_s_fields,
                                                                          uint64_t{0x1},
                                                                          nullptr,
                                                                          seq_table_ric_action_admitted_item_s_names,
                                                                          1,
                                                                          0},
                                                                         true,
                                                                         true,
                                                                         nullptr,
                                                                         0};

// RICaction-Admitted-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_admitted_item_ies_o[]    = {14};
static const uint8_t  objset_crit_ric_action_admitted_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_admitted_item_ies_o[] = {
    ric_action_admitted_item_ies_o::value_c::types::ric_action_admitted_item};
static const uint8_t                    objset_presence_ric_action_admitted_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_admitted_item_ies_o[]  = {
    {objset_crit_ric_action_admitted_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_admitted_item_ies_o, ric_action_admitted_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_admitted_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_admitted_item_ies_o::desc = {objset_id_ric_action_admitted_item_ies_o,
                                                                       1,
                                                                       "id",
                                                                       objset_columns_ric_action_admitted_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_admitted_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_admitted_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_admitted_item_ies_o_value_c_names[] = {
    {"RICaction-Admitted-Item", nullptr}};
const asn1::table::choice_desc ric_action_admitted_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_admitted_item_ies_o::value_c::types>(),
    ric_action_admitted_item_ies_o::value_c::types::nof_types,
    ric_action_admitted_item_ies_o::value_c::types::nof_exts,
    ric_action_admitted_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_admitted_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_admitted_item_ies_o_value_c_names,
    "ric_action_admitted_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_admitted_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_admitted_item_ies_o_value_c_types_values[]    = {"RICaction-Admitted-Item"};
const asn1::table::enum_desc ric_action_admitted_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_admitted_item_ies_o_value_c_types_values,
    1,
    "ric_action_admitted_item_ies_o::value_c::types"};

// RICaction-ConfirmedForModification-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_confirmed_for_mod_item_s_fields[] = {
    {offsetof(ric_action_confirmed_for_mod_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr}};
static const asn1::table::field_name seq_table_ric_action_confirmed_for_mod_item_s_names[] = {{"ricActionID", nullptr}};
const asn1::table::seq_desc          ric_action_confirmed_for_mod_item_s::desc             = {
    {seq_table_ric_action_confirmed_for_mod_item_s_fields,
                          uint64_t{0x1},
                          nullptr,
                          seq_table_ric_action_confirmed_for_mod_item_s_names,
                          1,
                          0},
    true,
    true,
    nullptr,
    0};

// RICaction-ConfirmedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_confirmed_for_mod_item_ies_o[]    = {88};
static const uint8_t  objset_crit_ric_action_confirmed_for_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_confirmed_for_mod_item_ies_o[] = {
    ric_action_confirmed_for_mod_item_ies_o::value_c::types::ric_action_confirmed_for_mod_item};
static const uint8_t objset_presence_ric_action_confirmed_for_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_confirmed_for_mod_item_ies_o[] = {
    {objset_crit_ric_action_confirmed_for_mod_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_confirmed_for_mod_item_ies_o,
     ric_action_confirmed_for_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_confirmed_for_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_confirmed_for_mod_item_ies_o::desc = {
    objset_id_ric_action_confirmed_for_mod_item_ies_o,
    1,
    "id",
    objset_columns_ric_action_confirmed_for_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_confirmed_for_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_confirmed_for_mod_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_confirmed_for_mod_item_ies_o_value_c_names[] = {
    {"RICaction-ConfirmedForModification-Item", nullptr}};
const asn1::table::choice_desc ric_action_confirmed_for_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_confirmed_for_mod_item_ies_o::value_c::types>(),
    ric_action_confirmed_for_mod_item_ies_o::value_c::types::nof_types,
    ric_action_confirmed_for_mod_item_ies_o::value_c::types::nof_exts,
    ric_action_confirmed_for_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_confirmed_for_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_confirmed_for_mod_item_ies_o_value_c_names,
    "ric_action_confirmed_for_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_confirmed_for_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_confirmed_for_mod_item_ies_o_value_c_types_values[] = {
    "RICaction-ConfirmedForModification-Item"};
const asn1::table::enum_desc ric_action_confirmed_for_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_confirmed_for_mod_item_ies_o_value_c_types_values,
    1,
    "ric_action_confirmed_for_mod_item_ies_o::value_c::types"};

// RICaction-ConfirmedForRemoval-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_confirmed_for_removal_item_s_fields[] = {
    {offsetof(ric_action_confirmed_for_removal_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr}};
static const asn1::table::field_name seq_table_ric_action_confirmed_for_removal_item_s_names[] = {
    {"ricActionID", nullptr}};
const asn1::table::seq_desc ric_action_confirmed_for_removal_item_s::desc = {
    {seq_table_ric_action_confirmed_for_removal_item_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_ric_action_confirmed_for_removal_item_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// RICaction-ConfirmedForRemoval-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_confirmed_for_removal_item_ies_o[]    = {92};
static const uint8_t  objset_crit_ric_action_confirmed_for_removal_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_confirmed_for_removal_item_ies_o[] = {
    ric_action_confirmed_for_removal_item_ies_o::value_c::types::ric_action_confirmed_for_removal_item};
static const uint8_t objset_presence_ric_action_confirmed_for_removal_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_confirmed_for_removal_item_ies_o[] = {
    {objset_crit_ric_action_confirmed_for_removal_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_confirmed_for_removal_item_ies_o,
     ric_action_confirmed_for_removal_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_confirmed_for_removal_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_confirmed_for_removal_item_ies_o::desc = {
    objset_id_ric_action_confirmed_for_removal_item_ies_o,
    1,
    "id",
    objset_columns_ric_action_confirmed_for_removal_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_confirmed_for_removal_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_confirmed_for_removal_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_confirmed_for_removal_item_ies_o_value_c_names[] = {
    {"RICaction-ConfirmedForRemoval-Item", nullptr}};
const asn1::table::choice_desc ric_action_confirmed_for_removal_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_confirmed_for_removal_item_ies_o::value_c::types>(),
    ric_action_confirmed_for_removal_item_ies_o::value_c::types::nof_types,
    ric_action_confirmed_for_removal_item_ies_o::value_c::types::nof_exts,
    ric_action_confirmed_for_removal_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_confirmed_for_removal_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_confirmed_for_removal_item_ies_o_value_c_names,
    "ric_action_confirmed_for_removal_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_confirmed_for_removal_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_confirmed_for_removal_item_ies_o_value_c_types_values[] = {
    "RICaction-ConfirmedForRemoval-Item"};
const asn1::table::enum_desc ric_action_confirmed_for_removal_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_confirmed_for_removal_item_ies_o_value_c_types_values,
    1,
    "ric_action_confirmed_for_removal_item_ies_o::value_c::types"};

// RICaction-FailedToBeAddedForModification-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_failed_to_be_added_for_mod_item_s_fields[] = {
    {offsetof(ric_action_failed_to_be_added_for_mod_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_failed_to_be_added_for_mod_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const asn1::table::field_name seq_table_ric_action_failed_to_be_added_for_mod_item_s_names[] = {
    {"ricActionID", nullptr},
    {"cause", nullptr}};
const asn1::table::seq_desc ric_action_failed_to_be_added_for_mod_item_s::desc = {
    {seq_table_ric_action_failed_to_be_added_for_mod_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_ric_action_failed_to_be_added_for_mod_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// RICaction-FailedToBeAddedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_failed_to_be_added_for_mod_item_ies_o[]    = {82};
static const uint8_t  objset_crit_ric_action_failed_to_be_added_for_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_failed_to_be_added_for_mod_item_ies_o[] = {
    ric_action_failed_to_be_added_for_mod_item_ies_o::value_c::types::ric_action_failed_to_be_added_for_mod_item};
static const uint8_t objset_presence_ric_action_failed_to_be_added_for_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_failed_to_be_added_for_mod_item_ies_o[] = {
    {objset_crit_ric_action_failed_to_be_added_for_mod_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_failed_to_be_added_for_mod_item_ies_o,
     ric_action_failed_to_be_added_for_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_failed_to_be_added_for_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_failed_to_be_added_for_mod_item_ies_o::desc = {
    objset_id_ric_action_failed_to_be_added_for_mod_item_ies_o,
    1,
    "id",
    objset_columns_ric_action_failed_to_be_added_for_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field
    choice_table_ric_action_failed_to_be_added_for_mod_item_ies_o_value_c_alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &ric_action_failed_to_be_added_for_mod_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_failed_to_be_added_for_mod_item_ies_o_value_c_names[] = {
    {"RICaction-FailedToBeAddedForModification-Item", nullptr}};
const asn1::table::choice_desc ric_action_failed_to_be_added_for_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_failed_to_be_added_for_mod_item_ies_o::value_c::types>(),
    ric_action_failed_to_be_added_for_mod_item_ies_o::value_c::types::nof_types,
    ric_action_failed_to_be_added_for_mod_item_ies_o::value_c::types::nof_exts,
    ric_action_failed_to_be_added_for_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_failed_to_be_added_for_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_failed_to_be_added_for_mod_item_ies_o_value_c_names,
    "ric_action_failed_to_be_added_for_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_failed_to_be_added_for_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_failed_to_be_added_for_mod_item_ies_o_value_c_types_values[] = {
    "RICaction-FailedToBeAddedForModification-Item"};
const asn1::table::enum_desc ric_action_failed_to_be_added_for_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_failed_to_be_added_for_mod_item_ies_o_value_c_types_values,
    1,
    "ric_action_failed_to_be_added_for_mod_item_ies_o::value_c::types"};

// RICaction-FailedToBeModifiedForModification-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_failed_to_be_modified_for_mod_item_s_fields[] = {
    {offsetof(ric_action_failed_to_be_modified_for_mod_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_failed_to_be_modified_for_mod_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const asn1::table::field_name seq_table_ric_action_failed_to_be_modified_for_mod_item_s_names[] = {
    {"ricActionID", nullptr},
    {"cause", nullptr}};
const asn1::table::seq_desc ric_action_failed_to_be_modified_for_mod_item_s::desc = {
    {seq_table_ric_action_failed_to_be_modified_for_mod_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_ric_action_failed_to_be_modified_for_mod_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// RICaction-FailedToBeModifiedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_failed_to_be_modified_for_mod_item_ies_o[]    = {78};
static const uint8_t  objset_crit_ric_action_failed_to_be_modified_for_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_failed_to_be_modified_for_mod_item_ies_o[] = {
    ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c::types::ric_action_failed_to_be_modified_for_mod_item};
static const uint8_t objset_presence_ric_action_failed_to_be_modified_for_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_failed_to_be_modified_for_mod_item_ies_o[] = {
    {objset_crit_ric_action_failed_to_be_modified_for_mod_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_failed_to_be_modified_for_mod_item_ies_o,
     ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_failed_to_be_modified_for_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_failed_to_be_modified_for_mod_item_ies_o::desc = {
    objset_id_ric_action_failed_to_be_modified_for_mod_item_ies_o,
    1,
    "id",
    objset_columns_ric_action_failed_to_be_modified_for_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field
    choice_table_ric_action_failed_to_be_modified_for_mod_item_ies_o_value_c_alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &ric_action_failed_to_be_modified_for_mod_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_failed_to_be_modified_for_mod_item_ies_o_value_c_names[] =
    {{"RICaction-FailedToBeModifiedForModification-Item", nullptr}};
const asn1::table::choice_desc ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c::types>(),
    ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c::types::nof_types,
    ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c::types::nof_exts,
    ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_failed_to_be_modified_for_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_failed_to_be_modified_for_mod_item_ies_o_value_c_names,
    "ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_failed_to_be_modified_for_mod_item_ies_o_value_c_types_values[] = {
    "RICaction-FailedToBeModifiedForModification-Item"};
const asn1::table::enum_desc ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_failed_to_be_modified_for_mod_item_ies_o_value_c_types_values,
    1,
    "ric_action_failed_to_be_modified_for_mod_item_ies_o::value_c::types"};

// RICaction-FailedToBeRemovedForModification-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_failed_to_be_remd_for_mod_item_s_fields[] = {
    {offsetof(ric_action_failed_to_be_remd_for_mod_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_failed_to_be_remd_for_mod_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const asn1::table::field_name seq_table_ric_action_failed_to_be_remd_for_mod_item_s_names[] = {
    {"ricActionID", nullptr},
    {"cause", nullptr}};
const asn1::table::seq_desc ric_action_failed_to_be_remd_for_mod_item_s::desc = {
    {seq_table_ric_action_failed_to_be_remd_for_mod_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_ric_action_failed_to_be_remd_for_mod_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// RICaction-FailedToBeRemovedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_failed_to_be_remd_for_mod_item_ies_o[]    = {74};
static const uint8_t  objset_crit_ric_action_failed_to_be_remd_for_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_failed_to_be_remd_for_mod_item_ies_o[] = {
    ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c::types::ric_action_failed_to_be_remd_for_mod_item};
static const uint8_t objset_presence_ric_action_failed_to_be_remd_for_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_failed_to_be_remd_for_mod_item_ies_o[] = {
    {objset_crit_ric_action_failed_to_be_remd_for_mod_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_failed_to_be_remd_for_mod_item_ies_o,
     ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_failed_to_be_remd_for_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_failed_to_be_remd_for_mod_item_ies_o::desc = {
    objset_id_ric_action_failed_to_be_remd_for_mod_item_ies_o,
    1,
    "id",
    objset_columns_ric_action_failed_to_be_remd_for_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field
    choice_table_ric_action_failed_to_be_remd_for_mod_item_ies_o_value_c_alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &ric_action_failed_to_be_remd_for_mod_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_failed_to_be_remd_for_mod_item_ies_o_value_c_names[] = {
    {"RICaction-FailedToBeRemovedForModification-Item", nullptr}};
const asn1::table::choice_desc ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c::types>(),
    ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c::types::nof_types,
    ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c::types::nof_exts,
    ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_failed_to_be_remd_for_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_failed_to_be_remd_for_mod_item_ies_o_value_c_names,
    "ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_failed_to_be_remd_for_mod_item_ies_o_value_c_types_values[] = {
    "RICaction-FailedToBeRemovedForModification-Item"};
const asn1::table::enum_desc ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_failed_to_be_remd_for_mod_item_ies_o_value_c_types_values,
    1,
    "ric_action_failed_to_be_remd_for_mod_item_ies_o::value_c::types"};

// RICaction-ModifiedForModification-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_modified_for_mod_item_s_fields[] = {
    {offsetof(ric_action_modified_for_mod_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr}};
static const asn1::table::field_name seq_table_ric_action_modified_for_mod_item_s_names[] = {{"ricActionID", nullptr}};
const asn1::table::seq_desc          ric_action_modified_for_mod_item_s::desc             = {
    {seq_table_ric_action_modified_for_mod_item_s_fields,
                          uint64_t{0x1},
                          nullptr,
                          seq_table_ric_action_modified_for_mod_item_s_names,
                          1,
                          0},
    true,
    true,
    nullptr,
    0};

// RICaction-ModifiedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_modified_for_mod_item_ies_o[]    = {76};
static const uint8_t  objset_crit_ric_action_modified_for_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_modified_for_mod_item_ies_o[] = {
    ric_action_modified_for_mod_item_ies_o::value_c::types::ric_action_modified_for_mod_item};
static const uint8_t objset_presence_ric_action_modified_for_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_modified_for_mod_item_ies_o[] = {
    {objset_crit_ric_action_modified_for_mod_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_modified_for_mod_item_ies_o,
     ric_action_modified_for_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_modified_for_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_modified_for_mod_item_ies_o::desc =
    {objset_id_ric_action_modified_for_mod_item_ies_o, 1, "id", objset_columns_ric_action_modified_for_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_modified_for_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_modified_for_mod_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_modified_for_mod_item_ies_o_value_c_names[] = {
    {"RICaction-ModifiedForModification-Item", nullptr}};
const asn1::table::choice_desc ric_action_modified_for_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_modified_for_mod_item_ies_o::value_c::types>(),
    ric_action_modified_for_mod_item_ies_o::value_c::types::nof_types,
    ric_action_modified_for_mod_item_ies_o::value_c::types::nof_exts,
    ric_action_modified_for_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_modified_for_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_modified_for_mod_item_ies_o_value_c_names,
    "ric_action_modified_for_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_modified_for_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_modified_for_mod_item_ies_o_value_c_types_values[] = {
    "RICaction-ModifiedForModification-Item"};
const asn1::table::enum_desc ric_action_modified_for_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_modified_for_mod_item_ies_o_value_c_types_values,
    1,
    "ric_action_modified_for_mod_item_ies_o::value_c::types"};

// RICaction-NotAdmitted-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_not_admitted_item_s_fields[] = {
    {offsetof(ric_action_not_admitted_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_not_admitted_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const asn1::table::field_name seq_table_ric_action_not_admitted_item_s_names[] = {{"ricActionID", nullptr},
                                                                                         {"cause", nullptr}};
const asn1::table::seq_desc ric_action_not_admitted_item_s::desc = {{seq_table_ric_action_not_admitted_item_s_fields,
                                                                     uint64_t{0x3},
                                                                     nullptr,
                                                                     seq_table_ric_action_not_admitted_item_s_names,
                                                                     2,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// RICaction-NotAdmitted-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_not_admitted_item_ies_o[]    = {16};
static const uint8_t  objset_crit_ric_action_not_admitted_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_not_admitted_item_ies_o[] = {
    ric_action_not_admitted_item_ies_o::value_c::types::ric_action_not_admitted_item};
static const uint8_t                    objset_presence_ric_action_not_admitted_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_not_admitted_item_ies_o[]  = {
    {objset_crit_ric_action_not_admitted_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_not_admitted_item_ies_o, ric_action_not_admitted_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_not_admitted_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_not_admitted_item_ies_o::desc =
    {objset_id_ric_action_not_admitted_item_ies_o, 1, "id", objset_columns_ric_action_not_admitted_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_not_admitted_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_not_admitted_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_not_admitted_item_ies_o_value_c_names[] = {
    {"RICaction-NotAdmitted-Item", nullptr}};
const asn1::table::choice_desc ric_action_not_admitted_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_not_admitted_item_ies_o::value_c::types>(),
    ric_action_not_admitted_item_ies_o::value_c::types::nof_types,
    ric_action_not_admitted_item_ies_o::value_c::types::nof_exts,
    ric_action_not_admitted_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_not_admitted_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_not_admitted_item_ies_o_value_c_names,
    "ric_action_not_admitted_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_not_admitted_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_not_admitted_item_ies_o_value_c_types_values[] = {
    "RICaction-NotAdmitted-Item"};
const asn1::table::enum_desc ric_action_not_admitted_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_not_admitted_item_ies_o_value_c_types_values,
    1,
    "ric_action_not_admitted_item_ies_o::value_c::types"};

// RICaction-RefusedToBeModified-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_refused_to_be_modified_item_s_fields[] = {
    {offsetof(ric_action_refused_to_be_modified_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_refused_to_be_modified_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const asn1::table::field_name seq_table_ric_action_refused_to_be_modified_item_s_names[] = {
    {"ricActionID", nullptr},
    {"cause", nullptr}};
const asn1::table::seq_desc ric_action_refused_to_be_modified_item_s::desc = {
    {seq_table_ric_action_refused_to_be_modified_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_ric_action_refused_to_be_modified_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// RICaction-RefusedToBeModified-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_refused_to_be_modified_item_ies_o[]    = {90};
static const uint8_t  objset_crit_ric_action_refused_to_be_modified_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_refused_to_be_modified_item_ies_o[] = {
    ric_action_refused_to_be_modified_item_ies_o::value_c::types::ric_action_refused_to_be_modified_item};
static const uint8_t objset_presence_ric_action_refused_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_refused_to_be_modified_item_ies_o[] = {
    {objset_crit_ric_action_refused_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_refused_to_be_modified_item_ies_o,
     ric_action_refused_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_refused_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_refused_to_be_modified_item_ies_o::desc = {
    objset_id_ric_action_refused_to_be_modified_item_ies_o,
    1,
    "id",
    objset_columns_ric_action_refused_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_refused_to_be_modified_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_refused_to_be_modified_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_refused_to_be_modified_item_ies_o_value_c_names[] = {
    {"RICaction-RefusedToBeModified-Item", nullptr}};
const asn1::table::choice_desc ric_action_refused_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_refused_to_be_modified_item_ies_o::value_c::types>(),
    ric_action_refused_to_be_modified_item_ies_o::value_c::types::nof_types,
    ric_action_refused_to_be_modified_item_ies_o::value_c::types::nof_exts,
    ric_action_refused_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_refused_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_refused_to_be_modified_item_ies_o_value_c_names,
    "ric_action_refused_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_refused_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_refused_to_be_modified_item_ies_o_value_c_types_values[] = {
    "RICaction-RefusedToBeModified-Item"};
const asn1::table::enum_desc ric_action_refused_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_refused_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "ric_action_refused_to_be_modified_item_ies_o::value_c::types"};

// RICaction-RefusedToBeRemoved-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_refused_to_be_remd_item_s_fields[] = {
    {offsetof(ric_action_refused_to_be_remd_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_refused_to_be_remd_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const asn1::table::field_name seq_table_ric_action_refused_to_be_remd_item_s_names[] = {{"ricActionID", nullptr},
                                                                                               {"cause", nullptr}};
const asn1::table::seq_desc          ric_action_refused_to_be_remd_item_s::desc             = {
    {seq_table_ric_action_refused_to_be_remd_item_s_fields,
                          uint64_t{0x3},
                          nullptr,
                          seq_table_ric_action_refused_to_be_remd_item_s_names,
                          2,
                          0},
    true,
    true,
    nullptr,
    0};

// RICaction-RefusedToBeRemoved-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_refused_to_be_remd_item_ies_o[]    = {94};
static const uint8_t  objset_crit_ric_action_refused_to_be_remd_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_refused_to_be_remd_item_ies_o[] = {
    ric_action_refused_to_be_remd_item_ies_o::value_c::types::ric_action_refused_to_be_remd_item};
static const uint8_t objset_presence_ric_action_refused_to_be_remd_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_refused_to_be_remd_item_ies_o[] = {
    {objset_crit_ric_action_refused_to_be_remd_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_refused_to_be_remd_item_ies_o,
     ric_action_refused_to_be_remd_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_refused_to_be_remd_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_refused_to_be_remd_item_ies_o::desc = {
    objset_id_ric_action_refused_to_be_remd_item_ies_o,
    1,
    "id",
    objset_columns_ric_action_refused_to_be_remd_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_refused_to_be_remd_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_refused_to_be_remd_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_refused_to_be_remd_item_ies_o_value_c_names[] = {
    {"RICaction-RefusedToBeRemoved-Item", nullptr}};
const asn1::table::choice_desc ric_action_refused_to_be_remd_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_refused_to_be_remd_item_ies_o::value_c::types>(),
    ric_action_refused_to_be_remd_item_ies_o::value_c::types::nof_types,
    ric_action_refused_to_be_remd_item_ies_o::value_c::types::nof_exts,
    ric_action_refused_to_be_remd_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_refused_to_be_remd_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_refused_to_be_remd_item_ies_o_value_c_names,
    "ric_action_refused_to_be_remd_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_refused_to_be_remd_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_refused_to_be_remd_item_ies_o_value_c_types_values[] = {
    "RICaction-RefusedToBeRemoved-Item"};
const asn1::table::enum_desc ric_action_refused_to_be_remd_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_refused_to_be_remd_item_ies_o_value_c_types_values,
    1,
    "ric_action_refused_to_be_remd_item_ies_o::value_c::types"};

// RICaction-RemovedForModification-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_remd_for_mod_item_s_fields[] = {
    {offsetof(ric_action_remd_for_mod_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr}};
static const asn1::table::field_name seq_table_ric_action_remd_for_mod_item_s_names[] = {{"ricActionID", nullptr}};
const asn1::table::seq_desc ric_action_remd_for_mod_item_s::desc = {{seq_table_ric_action_remd_for_mod_item_s_fields,
                                                                     uint64_t{0x1},
                                                                     nullptr,
                                                                     seq_table_ric_action_remd_for_mod_item_s_names,
                                                                     1,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// RICaction-RemovedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_remd_for_mod_item_ies_o[]    = {72};
static const uint8_t  objset_crit_ric_action_remd_for_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_remd_for_mod_item_ies_o[] = {
    ric_action_remd_for_mod_item_ies_o::value_c::types::ric_action_remd_for_mod_item};
static const uint8_t                    objset_presence_ric_action_remd_for_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_remd_for_mod_item_ies_o[]  = {
    {objset_crit_ric_action_remd_for_mod_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_remd_for_mod_item_ies_o, ric_action_remd_for_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_remd_for_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_remd_for_mod_item_ies_o::desc =
    {objset_id_ric_action_remd_for_mod_item_ies_o, 1, "id", objset_columns_ric_action_remd_for_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_remd_for_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_remd_for_mod_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_remd_for_mod_item_ies_o_value_c_names[] = {
    {"RICaction-RemovedForModification-Item", nullptr}};
const asn1::table::choice_desc ric_action_remd_for_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_remd_for_mod_item_ies_o::value_c::types>(),
    ric_action_remd_for_mod_item_ies_o::value_c::types::nof_types,
    ric_action_remd_for_mod_item_ies_o::value_c::types::nof_exts,
    ric_action_remd_for_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_remd_for_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_remd_for_mod_item_ies_o_value_c_names,
    "ric_action_remd_for_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_remd_for_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_remd_for_mod_item_ies_o_value_c_types_values[] = {
    "RICaction-RemovedForModification-Item"};
const asn1::table::enum_desc ric_action_remd_for_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_remd_for_mod_item_ies_o_value_c_types_values,
    1,
    "ric_action_remd_for_mod_item_ies_o::value_c::types"};

// RICaction-RequiredToBeModified-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_required_to_be_modified_item_s_fields[] = {
    {offsetof(ric_action_required_to_be_modified_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_required_to_be_modified_item_s, ric_time_to_wait),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ric_action_required_to_be_modified_item_s::ric_time_to_wait)>(),
     decltype(ric_action_required_to_be_modified_item_s::ric_time_to_wait)::has_ext,
     false,
     decltype(ric_action_required_to_be_modified_item_s::ric_time_to_wait)::nof_types,
     decltype(ric_action_required_to_be_modified_item_s::ric_time_to_wait)::nof_exts,
     nullptr,
     &ric_time_to_wait_opts::desc}};
static const asn1::table::field_name seq_table_ric_action_required_to_be_modified_item_s_names[] = {
    {"ricActionID", nullptr},
    {"ricTimeToWait", nullptr}};
const asn1::table::seq_desc ric_action_required_to_be_modified_item_s::desc = {
    {seq_table_ric_action_required_to_be_modified_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_ric_action_required_to_be_modified_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// RICaction-RequiredToBeModified-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_required_to_be_modified_item_ies_o[]    = {84};
static const uint8_t  objset_crit_ric_action_required_to_be_modified_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_required_to_be_modified_item_ies_o[] = {
    ric_action_required_to_be_modified_item_ies_o::value_c::types::ric_action_required_to_be_modified_item};
static const uint8_t objset_presence_ric_action_required_to_be_modified_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_required_to_be_modified_item_ies_o[] = {
    {objset_crit_ric_action_required_to_be_modified_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_required_to_be_modified_item_ies_o,
     ric_action_required_to_be_modified_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_required_to_be_modified_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_required_to_be_modified_item_ies_o::desc = {
    objset_id_ric_action_required_to_be_modified_item_ies_o,
    1,
    "id",
    objset_columns_ric_action_required_to_be_modified_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_required_to_be_modified_item_ies_o_value_c_alternatives[] =
    {{0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      nullptr,
      &ric_action_required_to_be_modified_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_required_to_be_modified_item_ies_o_value_c_names[] = {
    {"RICaction-RequiredToBeModified-Item", nullptr}};
const asn1::table::choice_desc ric_action_required_to_be_modified_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_required_to_be_modified_item_ies_o::value_c::types>(),
    ric_action_required_to_be_modified_item_ies_o::value_c::types::nof_types,
    ric_action_required_to_be_modified_item_ies_o::value_c::types::nof_exts,
    ric_action_required_to_be_modified_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_required_to_be_modified_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_required_to_be_modified_item_ies_o_value_c_names,
    "ric_action_required_to_be_modified_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_required_to_be_modified_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_required_to_be_modified_item_ies_o_value_c_types_values[] = {
    "RICaction-RequiredToBeModified-Item"};
const asn1::table::enum_desc ric_action_required_to_be_modified_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_required_to_be_modified_item_ies_o_value_c_types_values,
    1,
    "ric_action_required_to_be_modified_item_ies_o::value_c::types"};

// RICaction-RequiredToBeRemoved-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_required_to_be_remd_item_s_fields[] = {
    {offsetof(ric_action_required_to_be_remd_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_required_to_be_remd_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const asn1::table::field_name seq_table_ric_action_required_to_be_remd_item_s_names[] = {
    {"ricActionID", nullptr},
    {"cause", nullptr}};
const asn1::table::seq_desc ric_action_required_to_be_remd_item_s::desc = {
    {seq_table_ric_action_required_to_be_remd_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_ric_action_required_to_be_remd_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// RICaction-RequiredToBeRemoved-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_required_to_be_remd_item_ies_o[]    = {86};
static const uint8_t  objset_crit_ric_action_required_to_be_remd_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_required_to_be_remd_item_ies_o[] = {
    ric_action_required_to_be_remd_item_ies_o::value_c::types::ric_action_required_to_be_remd_item};
static const uint8_t objset_presence_ric_action_required_to_be_remd_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_required_to_be_remd_item_ies_o[] = {
    {objset_crit_ric_action_required_to_be_remd_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_required_to_be_remd_item_ies_o,
     ric_action_required_to_be_remd_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_required_to_be_remd_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_required_to_be_remd_item_ies_o::desc = {
    objset_id_ric_action_required_to_be_remd_item_ies_o,
    1,
    "id",
    objset_columns_ric_action_required_to_be_remd_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_required_to_be_remd_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_required_to_be_remd_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_required_to_be_remd_item_ies_o_value_c_names[] = {
    {"RICaction-RequiredToBeRemoved-Item", nullptr}};
const asn1::table::choice_desc ric_action_required_to_be_remd_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_required_to_be_remd_item_ies_o::value_c::types>(),
    ric_action_required_to_be_remd_item_ies_o::value_c::types::nof_types,
    ric_action_required_to_be_remd_item_ies_o::value_c::types::nof_exts,
    ric_action_required_to_be_remd_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_required_to_be_remd_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_required_to_be_remd_item_ies_o_value_c_names,
    "ric_action_required_to_be_remd_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_required_to_be_remd_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_required_to_be_remd_item_ies_o_value_c_types_values[] = {
    "RICaction-RequiredToBeRemoved-Item"};
const asn1::table::enum_desc ric_action_required_to_be_remd_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_required_to_be_remd_item_ies_o_value_c_types_values,
    1,
    "ric_action_required_to_be_remd_item_ies_o::value_c::types"};

// RICaction-ToBeAddedForModification-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_to_be_added_for_mod_item_s_fields[] = {
    {offsetof(ric_action_to_be_added_for_mod_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_to_be_added_for_mod_item_s, ric_action_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ric_action_to_be_added_for_mod_item_s::ric_action_type)>(),
     decltype(ric_action_to_be_added_for_mod_item_s::ric_action_type)::has_ext,
     false,
     decltype(ric_action_to_be_added_for_mod_item_s::ric_action_type)::nof_types,
     decltype(ric_action_to_be_added_for_mod_item_s::ric_action_type)::nof_exts,
     nullptr,
     &ric_action_type_opts::desc},
    {offsetof(ric_action_to_be_added_for_mod_item_s, ric_action_definition),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_action_to_be_added_for_mod_item_s::ric_action_definition),
                                asn1::table::string_json>::value},
    {offsetof(ric_action_to_be_added_for_mod_item_s, ric_action_execution_order),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_to_be_added_for_mod_item_s, ric_subsequent_action),
     (int32_t)offsetof(ric_action_to_be_added_for_mod_item_s, ric_subsequent_action_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_subsequent_action_s::desc}};
static const asn1::table::field_name seq_table_ric_action_to_be_added_for_mod_item_s_names[] = {
    {"ricActionID", nullptr},
    {"ricActionType", nullptr},
    {"ricActionDefinition", nullptr},
    {"ricActionExecutionOrder", nullptr},
    {"ricSubsequentAction", nullptr}};
static const asn1::table::seq_opt seq_table_ric_action_to_be_added_for_mod_item_s_opts[] = {
    {4, (int32_t)offsetof(ric_action_to_be_added_for_mod_item_s, ric_subsequent_action_present)}};
const asn1::table::seq_desc ric_action_to_be_added_for_mod_item_s::desc = {
    {seq_table_ric_action_to_be_added_for_mod_item_s_fields,
     uint64_t{0xf},
     seq_table_ric_action_to_be_added_for_mod_item_s_opts,
     seq_table_ric_action_to_be_added_for_mod_item_s_names,
     5,
     1},
    true,
    true,
    nullptr,
    0};

// RICaction-ToBeAddedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_to_be_added_for_mod_item_ies_o[]    = {70};
static const uint8_t  objset_crit_ric_action_to_be_added_for_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_to_be_added_for_mod_item_ies_o[] = {
    ric_action_to_be_added_for_mod_item_ies_o::value_c::types::ric_action_to_be_added_for_mod_item};
static const uint8_t objset_presence_ric_action_to_be_added_for_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_to_be_added_for_mod_item_ies_o[] = {
    {objset_crit_ric_action_to_be_added_for_mod_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_to_be_added_for_mod_item_ies_o,
     ric_action_to_be_added_for_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_to_be_added_for_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_to_be_added_for_mod_item_ies_o::desc = {
    objset_id_ric_action_to_be_added_for_mod_item_ies_o,
    1,
    "id",
    objset_columns_ric_action_to_be_added_for_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_to_be_added_for_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_to_be_added_for_mod_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_to_be_added_for_mod_item_ies_o_value_c_names[] = {
    {"RICaction-ToBeAddedForModification-Item", nullptr}};
const asn1::table::choice_desc ric_action_to_be_added_for_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_to_be_added_for_mod_item_ies_o::value_c::types>(),
    ric_action_to_be_added_for_mod_item_ies_o::value_c::types::nof_types,
    ric_action_to_be_added_for_mod_item_ies_o::value_c::types::nof_exts,
    ric_action_to_be_added_for_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_to_be_added_for_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_to_be_added_for_mod_item_ies_o_value_c_names,
    "ric_action_to_be_added_for_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_to_be_added_for_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_to_be_added_for_mod_item_ies_o_value_c_types_values[] = {
    "RICaction-ToBeAddedForModification-Item"};
const asn1::table::enum_desc ric_action_to_be_added_for_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_to_be_added_for_mod_item_ies_o_value_c_types_values,
    1,
    "ric_action_to_be_added_for_mod_item_ies_o::value_c::types"};

// RICaction-ToBeModifiedForModification-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_to_be_modified_for_mod_item_s_fields[] = {
    {offsetof(ric_action_to_be_modified_for_mod_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_to_be_modified_for_mod_item_s, ric_action_definition),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(ric_action_to_be_modified_for_mod_item_s::ric_action_definition),
                                      asn1::table::string_json>::value},
    {offsetof(ric_action_to_be_modified_for_mod_item_s, ric_action_execution_order),
     (int32_t)offsetof(ric_action_to_be_modified_for_mod_item_s, ric_action_execution_order_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_to_be_modified_for_mod_item_s, ric_subsequent_action),
     (int32_t)offsetof(ric_action_to_be_modified_for_mod_item_s, ric_subsequent_action_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_subsequent_action_s::desc}};
static const asn1::table::field_name seq_table_ric_action_to_be_modified_for_mod_item_s_names[] = {
    {"ricActionID", nullptr},
    {"ricActionDefinition", nullptr},
    {"ricActionExecutionOrder", nullptr},
    {"ricSubsequentAction", nullptr}};
static const asn1::table::seq_opt seq_table_ric_action_to_be_modified_for_mod_item_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(ric_action_to_be_modified_for_mod_item_s, ric_action_execution_order_present)},
    {3, (int32_t)offsetof(ric_action_to_be_modified_for_mod_item_s, ric_subsequent_action_present)}};
const asn1::table::seq_desc ric_action_to_be_modified_for_mod_item_s::desc = {
    {seq_table_ric_action_to_be_modified_for_mod_item_s_fields,
     uint64_t{0x1},
     seq_table_ric_action_to_be_modified_for_mod_item_s_opts,
     seq_table_ric_action_to_be_modified_for_mod_item_s_names,
     4,
     3},
    true,
    true,
    nullptr,
    0};

// RICaction-ToBeModifiedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_to_be_modified_for_mod_item_ies_o[]    = {68};
static const uint8_t  objset_crit_ric_action_to_be_modified_for_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_to_be_modified_for_mod_item_ies_o[] = {
    ric_action_to_be_modified_for_mod_item_ies_o::value_c::types::ric_action_to_be_modified_for_mod_item};
static const uint8_t objset_presence_ric_action_to_be_modified_for_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_to_be_modified_for_mod_item_ies_o[] = {
    {objset_crit_ric_action_to_be_modified_for_mod_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_to_be_modified_for_mod_item_ies_o,
     ric_action_to_be_modified_for_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_to_be_modified_for_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_to_be_modified_for_mod_item_ies_o::desc = {
    objset_id_ric_action_to_be_modified_for_mod_item_ies_o,
    1,
    "id",
    objset_columns_ric_action_to_be_modified_for_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_to_be_modified_for_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_to_be_modified_for_mod_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_to_be_modified_for_mod_item_ies_o_value_c_names[] = {
    {"RICaction-ToBeModifiedForModification-Item", nullptr}};
const asn1::table::choice_desc ric_action_to_be_modified_for_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_to_be_modified_for_mod_item_ies_o::value_c::types>(),
    ric_action_to_be_modified_for_mod_item_ies_o::value_c::types::nof_types,
    ric_action_to_be_modified_for_mod_item_ies_o::value_c::types::nof_exts,
    ric_action_to_be_modified_for_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_to_be_modified_for_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_to_be_modified_for_mod_item_ies_o_value_c_names,
    "ric_action_to_be_modified_for_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_to_be_modified_for_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_to_be_modified_for_mod_item_ies_o_value_c_types_values[] = {
    "RICaction-ToBeModifiedForModification-Item"};
const asn1::table::enum_desc ric_action_to_be_modified_for_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_to_be_modified_for_mod_item_ies_o_value_c_types_values,
    1,
    "ric_action_to_be_modified_for_mod_item_ies_o::value_c::types"};

// RICaction-ToBeRemovedForModification-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_to_be_remd_for_mod_item_s_fields[] = {
    {offsetof(ric_action_to_be_remd_for_mod_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr}};
static const asn1::table::field_name seq_table_ric_action_to_be_remd_for_mod_item_s_names[] = {
    {"ricActionID", nullptr}};
const asn1::table::seq_desc ric_action_to_be_remd_for_mod_item_s::desc = {
    {seq_table_ric_action_to_be_remd_for_mod_item_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_ric_action_to_be_remd_for_mod_item_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// RICaction-ToBeRemovedForModification-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_to_be_remd_for_mod_item_ies_o[]    = {66};
static const uint8_t  objset_crit_ric_action_to_be_remd_for_mod_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_to_be_remd_for_mod_item_ies_o[] = {
    ric_action_to_be_remd_for_mod_item_ies_o::value_c::types::ric_action_to_be_remd_for_mod_item};
static const uint8_t objset_presence_ric_action_to_be_remd_for_mod_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_to_be_remd_for_mod_item_ies_o[] = {
    {objset_crit_ric_action_to_be_remd_for_mod_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_to_be_remd_for_mod_item_ies_o,
     ric_action_to_be_remd_for_mod_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_to_be_remd_for_mod_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_to_be_remd_for_mod_item_ies_o::desc = {
    objset_id_ric_action_to_be_remd_for_mod_item_ies_o,
    1,
    "id",
    objset_columns_ric_action_to_be_remd_for_mod_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_to_be_remd_for_mod_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_to_be_remd_for_mod_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_to_be_remd_for_mod_item_ies_o_value_c_names[] = {
    {"RICaction-ToBeRemovedForModification-Item", nullptr}};
const asn1::table::choice_desc ric_action_to_be_remd_for_mod_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_to_be_remd_for_mod_item_ies_o::value_c::types>(),
    ric_action_to_be_remd_for_mod_item_ies_o::value_c::types::nof_types,
    ric_action_to_be_remd_for_mod_item_ies_o::value_c::types::nof_exts,
    ric_action_to_be_remd_for_mod_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_to_be_remd_for_mod_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_to_be_remd_for_mod_item_ies_o_value_c_names,
    "ric_action_to_be_remd_for_mod_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_to_be_remd_for_mod_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_to_be_remd_for_mod_item_ies_o_value_c_types_values[] = {
    "RICaction-ToBeRemovedForModification-Item"};
const asn1::table::enum_desc ric_action_to_be_remd_for_mod_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_to_be_remd_for_mod_item_ies_o_value_c_types_values,
    1,
    "ric_action_to_be_remd_for_mod_item_ies_o::value_c::types"};

// RICaction-ToBeSetup-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_to_be_setup_item_s_fields[] = {
    {offsetof(ric_action_to_be_setup_item_s, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_action_to_be_setup_item_s, ric_action_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ric_action_to_be_setup_item_s::ric_action_type)>(),
     decltype(ric_action_to_be_setup_item_s::ric_action_type)::has_ext,
     false,
     decltype(ric_action_to_be_setup_item_s::ric_action_type)::nof_types,
     decltype(ric_action_to_be_setup_item_s::ric_action_type)::nof_exts,
     nullptr,
     &ric_action_type_opts::desc},
    {offsetof(ric_action_to_be_setup_item_s, ric_action_definition),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(ric_action_to_be_setup_item_s::ric_action_definition),
                                      asn1::table::string_json>::value},
    {offsetof(ric_action_to_be_setup_item_s, ric_subsequent_action),
     (int32_t)offsetof(ric_action_to_be_setup_item_s, ric_subsequent_action_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_subsequent_action_s::desc}};
static const asn1::table::field_name seq_table_ric_action_to_be_setup_item_s_names[] = {
    {"ricActionID", nullptr},
    {"ricActionType", nullptr},
    {"ricActionDefinition", nullptr},
    {"ricSubsequentAction", nullptr}};
static const asn1::table::seq_opt seq_table_ric_action_to_be_setup_item_s_opts[] = {
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(ric_action_to_be_setup_item_s, ric_subsequent_action_present)}};
static const asn1::table::seq_field seq_table_ric_action_to_be_setup_item_s_g0_fields[] = {
    {offsetof(ric_action_to_be_setup_item_s, ric_action_execution_order),
     (int32_t)offsetof(ric_action_to_be_setup_item_s, ric_action_execution_order_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr}};
static const asn1::table::field_name seq_table_ric_action_to_be_setup_item_s_g0_names[] = {
    {"ricActionExecutionOrder", nullptr}};
static const asn1::table::seq_opt seq_table_ric_action_to_be_setup_item_s_g0_opts[] = {
    {0, (int32_t)offsetof(ric_action_to_be_setup_item_s, ric_action_execution_order_present)}};
static const asn1::table::seq_group seq_table_ric_action_to_be_setup_item_s_groups[] = {
    {{seq_table_ric_action_to_be_setup_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_ric_action_to_be_setup_item_s_g0_opts,
      seq_table_ric_action_to_be_setup_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc ric_action_to_be_setup_item_s::desc = {{seq_table_ric_action_to_be_setup_item_s_fields,
                                                                    uint64_t{0x3},
                                                                    seq_table_ric_action_to_be_setup_item_s_opts,
                                                                    seq_table_ric_action_to_be_setup_item_s_names,
                                                                    4,
                                                                    2},
                                                                   true,
                                                                   true,
                                                                   seq_table_ric_action_to_be_setup_item_s_groups,
                                                                   1};

// RICaction-ToBeSetup-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_action_to_be_setup_item_ies_o[]    = {19};
static const uint8_t  objset_crit_ric_action_to_be_setup_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_action_to_be_setup_item_ies_o[] = {
    ric_action_to_be_setup_item_ies_o::value_c::types::ric_action_to_be_setup_item};
static const uint8_t                    objset_presence_ric_action_to_be_setup_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_action_to_be_setup_item_ies_o[]  = {
    {objset_crit_ric_action_to_be_setup_item_ies_o, crit_e::nulltype},
    {objset_value_ric_action_to_be_setup_item_ies_o, ric_action_to_be_setup_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_action_to_be_setup_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_action_to_be_setup_item_ies_o::desc =
    {objset_id_ric_action_to_be_setup_item_ies_o, 1, "id", objset_columns_ric_action_to_be_setup_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_action_to_be_setup_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_action_to_be_setup_item_s::desc}};
static const asn1::table::field_name choice_table_ric_action_to_be_setup_item_ies_o_value_c_names[] = {
    {"RICaction-ToBeSetup-Item", nullptr}};
const asn1::table::choice_desc ric_action_to_be_setup_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_action_to_be_setup_item_ies_o::value_c::types>(),
    ric_action_to_be_setup_item_ies_o::value_c::types::nof_types,
    ric_action_to_be_setup_item_ies_o::value_c::types::nof_exts,
    ric_action_to_be_setup_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_action_to_be_setup_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_action_to_be_setup_item_ies_o_value_c_names,
    "ric_action_to_be_setup_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_action_to_be_setup_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_action_to_be_setup_item_ies_o_value_c_types_values[] = {"RICaction-ToBeSetup-Item"};
const asn1::table::enum_desc ric_action_to_be_setup_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_action_to_be_setup_item_ies_o_value_c_types_values,
    1,
    "ric_action_to_be_setup_item_ies_o::value_c::types"};

static const uint32_t objset_id_ric_assist_fail_ies_o[]   = {29, 1, 2};
static const uint8_t  objset_crit_ric_assist_fail_ies_o[] = {crit_e::reject, crit_e::ignore, crit_e::ignore};

// RICassistanceFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_assist_fail_ies_container_values[] = {
    {offsetof(ric_assist_fail_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_assist_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(ric_assist_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(ric_assist_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_ric_assist_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_assist_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc ric_assist_fail_ies_container::desc = {
    objset_id_ric_assist_fail_ies_o,
    objset_crit_ric_assist_fail_ies_o,
    ie_table_ric_assist_fail_ies_container_values,
    ie_table_ric_assist_fail_ies_container_presences,
    3,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_assist_halt_ies_o[]   = {29};
static const uint8_t  objset_crit_ric_assist_halt_ies_o[] = {crit_e::reject};

// RICassistanceHalt-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_assist_halt_ies_container_values[] = {
    {offsetof(ric_assist_halt_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc}};
static const int32_t ie_table_ric_assist_halt_ies_container_presences[]  = {asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc ric_assist_halt_ies_container::desc = {
    objset_id_ric_assist_halt_ies_o,
    objset_crit_ric_assist_halt_ies_o,
    ie_table_ric_assist_halt_ies_container_values,
    ie_table_ric_assist_halt_ies_container_presences,
    1,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_assist_ind_ies_o[]   = {29, 130, 127, 129};
static const uint8_t  objset_crit_ric_assist_ind_ies_o[] = {crit_e::reject,
                                                            crit_e::reject,
                                                            crit_e::reject,
                                                            crit_e::reject};

// RICassistanceIndication-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_assist_ind_ies_container_values[] = {
    {offsetof(ric_assist_ind_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_assist_ind_ies_container, ric_assist_sn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(ric_assist_ind_ies_container, ric_assist_hdr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_assist_ind_ies_container::ric_assist_hdr),
                                asn1::table::string_json>::value},
    {offsetof(ric_assist_ind_ies_container, ric_assist_outcome),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_assist_ind_ies_container::ric_assist_outcome),
                                asn1::table::string_json>::value}};
static const int32_t ie_table_ric_assist_ind_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                           asn1::table::seq_field::mandatory,
                                                                           asn1::table::seq_field::mandatory,
                                                                           asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc ric_assist_ind_ies_container::desc = {
    objset_id_ric_assist_ind_ies_o,
    objset_crit_ric_assist_ind_ies_o,
    ie_table_ric_assist_ind_ies_container_values,
    ie_table_ric_assist_ind_ies_container_presences,
    4,
    4,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_assist_request_ies_o[]   = {29, 127, 128, 131, 132};
static const uint8_t  objset_crit_ric_assist_request_ies_o[] = {crit_e::reject,
                                                                crit_e::reject,
                                                                crit_e::reject,
                                                                crit_e::reject,
                                                                crit_e::reject};

// RICassistanceRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_assist_request_ies_container_values[] = {
    {offsetof(ric_assist_request_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_assist_request_ies_container, ric_assist_hdr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_assist_request_ies_container::ric_assist_hdr),
                                asn1::table::string_json>::value},
    {offsetof(ric_assist_request_ies_container, ric_assist_msg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_assist_request_ies_container::ric_assist_msg),
                                asn1::table::string_json>::value},
    {offsetof(ric_assist_request_ies_container, ric_assist_upd),
     (int32_t)offsetof(ric_assist_request_ies_container, ric_assist_upd_present),
     asn1::table::enum_op<decltype(ric_assist_request_ies_container::ric_assist_upd)>(),
     decltype(ric_assist_request_ies_container::ric_assist_upd)::has_ext,
     false,
     decltype(ric_assist_request_ies_container::ric_assist_upd)::nof_types,
     decltype(ric_assist_request_ies_container::ric_assist_upd)::nof_exts,
     nullptr,
     &ric_assist_upd_opts::desc},
    {offsetof(ric_assist_request_ies_container, ric_assist_upd_num),
     (int32_t)offsetof(ric_assist_request_ies_container, ric_assist_upd_num_present),
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr}};
static const int32_t ie_table_ric_assist_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_assist_request_ies_container, ric_assist_upd_present),
    (int32_t)offsetof(ric_assist_request_ies_container, ric_assist_upd_num_present)};
const asn1::table::ie_container_desc ric_assist_request_ies_container::desc = {
    objset_id_ric_assist_request_ies_o,
    objset_crit_ric_assist_request_ies_o,
    ie_table_ric_assist_request_ies_container_values,
    ie_table_ric_assist_request_ies_container_presences,
    5,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_assist_resp_ies_o[]   = {29, 127, 129};
static const uint8_t  objset_crit_ric_assist_resp_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::reject};

// RICassistanceResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_assist_resp_ies_container_values[] = {
    {offsetof(ric_assist_resp_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_assist_resp_ies_container, ric_assist_hdr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_assist_resp_ies_container::ric_assist_hdr),
                                asn1::table::string_json>::value},
    {offsetof(ric_assist_resp_ies_container, ric_assist_outcome),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_assist_resp_ies_container::ric_assist_outcome),
                                asn1::table::string_json>::value}};
static const int32_t ie_table_ric_assist_resp_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc ric_assist_resp_ies_container::desc = {
    objset_id_ric_assist_resp_ies_o,
    objset_crit_ric_assist_resp_ies_o,
    ie_table_ric_assist_resp_ies_container_values,
    ie_table_ric_assist_resp_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_ctrl_ack_ies_o[]   = {29, 5, 20, 32};
static const uint8_t  objset_crit_ric_ctrl_ack_ies_o[] = {crit_e::reject,
                                                          crit_e::reject,
                                                          crit_e::reject,
                                                          crit_e::reject};

// RICcontrolAcknowledge-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_ctrl_ack_ies_container_values[] = {
    {offsetof(ric_ctrl_ack_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_ctrl_ack_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_ctrl_ack_ies_container, ric_call_process_id),
     (int32_t)offsetof(ric_ctrl_ack_ies_container, ric_call_process_id_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_ctrl_ack_ies_container::ric_call_process_id),
                                asn1::table::string_json>::value},
    {offsetof(ric_ctrl_ack_ies_container, ric_ctrl_outcome),
     (int32_t)offsetof(ric_ctrl_ack_ies_container, ric_ctrl_outcome_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_ctrl_ack_ies_container::ric_ctrl_outcome),
                                asn1::table::string_json>::value}};
static const int32_t ie_table_ric_ctrl_ack_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_ctrl_ack_ies_container, ric_call_process_id_present),
    (int32_t)offsetof(ric_ctrl_ack_ies_container, ric_ctrl_outcome_present)};
const asn1::table::ie_container_desc ric_ctrl_ack_ies_container::desc = {objset_id_ric_ctrl_ack_ies_o,
                                                                         objset_crit_ric_ctrl_ack_ies_o,
                                                                         ie_table_ric_ctrl_ack_ies_container_values,
                                                                         ie_table_ric_ctrl_ack_ies_container_presences,
                                                                         4,
                                                                         2,
                                                                         0,
                                                                         65535,
                                                                         true,
                                                                         "value"};

static const uint32_t objset_id_ric_ctrl_fail_ies_o[] = {29, 5, 20, 1, 32, 2};
static const uint8_t  objset_crit_ric_ctrl_fail_ies_o[] =
    {crit_e::reject, crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::reject, crit_e::ignore};

// RICcontrolFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_ctrl_fail_ies_container_values[] = {
    {offsetof(ric_ctrl_fail_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_ctrl_fail_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_ctrl_fail_ies_container, ric_call_process_id),
     (int32_t)offsetof(ric_ctrl_fail_ies_container, ric_call_process_id_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_ctrl_fail_ies_container::ric_call_process_id),
                                asn1::table::string_json>::value},
    {offsetof(ric_ctrl_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(ric_ctrl_fail_ies_container, ric_ctrl_outcome),
     (int32_t)offsetof(ric_ctrl_fail_ies_container, ric_ctrl_outcome_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_ctrl_fail_ies_container::ric_ctrl_outcome),
                                asn1::table::string_json>::value},
    {offsetof(ric_ctrl_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(ric_ctrl_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_ric_ctrl_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_ctrl_fail_ies_container, ric_call_process_id_present),
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_ctrl_fail_ies_container, ric_ctrl_outcome_present),
    (int32_t)offsetof(ric_ctrl_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc ric_ctrl_fail_ies_container::desc = {
    objset_id_ric_ctrl_fail_ies_o,
    objset_crit_ric_ctrl_fail_ies_o,
    ie_table_ric_ctrl_fail_ies_container_values,
    ie_table_ric_ctrl_fail_ies_container_presences,
    6,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_ctrl_request_ies_o[] = {29, 5, 20, 22, 23, 21};
static const uint8_t  objset_crit_ric_ctrl_request_ies_o[] =
    {crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject};

// RICcontrolRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_ctrl_request_ies_container_values[] = {
    {offsetof(ric_ctrl_request_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_ctrl_request_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_ctrl_request_ies_container, ric_call_process_id),
     (int32_t)offsetof(ric_ctrl_request_ies_container, ric_call_process_id_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_ctrl_request_ies_container::ric_call_process_id),
                                asn1::table::string_json>::value},
    {offsetof(ric_ctrl_request_ies_container, ric_ctrl_hdr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_ctrl_request_ies_container::ric_ctrl_hdr),
                                asn1::table::string_json>::value},
    {offsetof(ric_ctrl_request_ies_container, ric_ctrl_msg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_ctrl_request_ies_container::ric_ctrl_msg),
                                asn1::table::string_json>::value},
    {offsetof(ric_ctrl_request_ies_container, ric_ctrl_ack_request),
     (int32_t)offsetof(ric_ctrl_request_ies_container, ric_ctrl_ack_request_present),
     asn1::table::enum_op<decltype(ric_ctrl_request_ies_container::ric_ctrl_ack_request)>(),
     decltype(ric_ctrl_request_ies_container::ric_ctrl_ack_request)::has_ext,
     false,
     decltype(ric_ctrl_request_ies_container::ric_ctrl_ack_request)::nof_types,
     decltype(ric_ctrl_request_ies_container::ric_ctrl_ack_request)::nof_exts,
     nullptr,
     &ric_ctrl_ack_request_opts::desc}};
static const int32_t ie_table_ric_ctrl_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_ctrl_request_ies_container, ric_call_process_id_present),
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_ctrl_request_ies_container, ric_ctrl_ack_request_present)};
const asn1::table::ie_container_desc ric_ctrl_request_ies_container::desc = {
    objset_id_ric_ctrl_request_ies_o,
    objset_crit_ric_ctrl_request_ies_o,
    ie_table_ric_ctrl_request_ies_container_values,
    ie_table_ric_ctrl_request_ies_container_presences,
    6,
    4,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_ind_ies_o[]   = {29, 5, 15, 27, 28, 25, 26, 20};
static const uint8_t  objset_crit_ric_ind_ies_o[] = {crit_e::reject,
                                                     crit_e::reject,
                                                     crit_e::reject,
                                                     crit_e::reject,
                                                     crit_e::reject,
                                                     crit_e::reject,
                                                     crit_e::reject,
                                                     crit_e::reject};

// RICindication-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_ind_ies_container_values[] = {
    {offsetof(ric_ind_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_ind_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_ind_ies_container, ric_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_ind_ies_container, ric_ind_sn),
     (int32_t)offsetof(ric_ind_ies_container, ric_ind_sn_present),
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(ric_ind_ies_container, ric_ind_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ric_ind_ies_container::ric_ind_type)>(),
     decltype(ric_ind_ies_container::ric_ind_type)::has_ext,
     false,
     decltype(ric_ind_ies_container::ric_ind_type)::nof_types,
     decltype(ric_ind_ies_container::ric_ind_type)::nof_exts,
     nullptr,
     &ric_ind_type_opts::desc},
    {offsetof(ric_ind_ies_container, ric_ind_hdr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_ind_ies_container::ric_ind_hdr), asn1::table::string_json>::value},
    {offsetof(ric_ind_ies_container, ric_ind_msg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_ind_ies_container::ric_ind_msg), asn1::table::string_json>::value},
    {offsetof(ric_ind_ies_container, ric_call_process_id),
     (int32_t)offsetof(ric_ind_ies_container, ric_call_process_id_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_ind_ies_container::ric_call_process_id),
                                asn1::table::string_json>::value}};
static const int32_t ie_table_ric_ind_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_ind_ies_container, ric_ind_sn_present),
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_ind_ies_container, ric_call_process_id_present)};
const asn1::table::ie_container_desc ric_ind_ies_container::desc = {objset_id_ric_ind_ies_o,
                                                                    objset_crit_ric_ind_ies_o,
                                                                    ie_table_ric_ind_ies_container_values,
                                                                    ie_table_ric_ind_ies_container_presences,
                                                                    8,
                                                                    6,
                                                                    0,
                                                                    65535,
                                                                    true,
                                                                    "value"};

static const uint32_t objset_id_ric_query_fail_ies_o[]   = {29, 5, 1, 2};
static const uint8_t  objset_crit_ric_query_fail_ies_o[] = {crit_e::reject,
                                                            crit_e::reject,
                                                            crit_e::ignore,
                                                            crit_e::ignore};

// RICqueryFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_query_fail_ies_container_values[] = {
    {offsetof(ric_query_fail_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_query_fail_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_query_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(ric_query_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(ric_query_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_ric_query_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_query_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc ric_query_fail_ies_container::desc = {
    objset_id_ric_query_fail_ies_o,
    objset_crit_ric_query_fail_ies_o,
    ie_table_ric_query_fail_ies_container_values,
    ie_table_ric_query_fail_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_query_request_ies_o[]   = {29, 5, 95, 96};
static const uint8_t  objset_crit_ric_query_request_ies_o[] = {crit_e::reject,
                                                               crit_e::reject,
                                                               crit_e::reject,
                                                               crit_e::reject};

// RICqueryRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_query_request_ies_container_values[] = {
    {offsetof(ric_query_request_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_query_request_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_query_request_ies_container, ric_query_hdr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_query_request_ies_container::ric_query_hdr),
                                asn1::table::string_json>::value},
    {offsetof(ric_query_request_ies_container, ric_query_definition),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_query_request_ies_container::ric_query_definition),
                                asn1::table::string_json>::value}};
static const int32_t ie_table_ric_query_request_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                              asn1::table::seq_field::mandatory,
                                                                              asn1::table::seq_field::mandatory,
                                                                              asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc ric_query_request_ies_container::desc = {
    objset_id_ric_query_request_ies_o,
    objset_crit_ric_query_request_ies_o,
    ie_table_ric_query_request_ies_container_values,
    ie_table_ric_query_request_ies_container_presences,
    4,
    4,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_query_resp_ies_o[]   = {29, 5, 97};
static const uint8_t  objset_crit_ric_query_resp_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::reject};

// RICqueryResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_query_resp_ies_container_values[] = {
    {offsetof(ric_query_resp_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_query_resp_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_query_resp_ies_container, ric_query_outcome),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_query_resp_ies_container::ric_query_outcome),
                                asn1::table::string_json>::value}};
static const int32_t ie_table_ric_query_resp_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                           asn1::table::seq_field::mandatory,
                                                                           asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc ric_query_resp_ies_container::desc = {
    objset_id_ric_query_resp_ies_o,
    objset_crit_ric_query_resp_ies_o,
    ie_table_ric_query_resp_ies_container_values,
    ie_table_ric_query_resp_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_service_load_status_fail_ies_o[]   = {117, 105, 1, 2};
static const uint8_t  objset_crit_ric_service_load_status_fail_ies_o[] = {crit_e::reject,
                                                                          crit_e::ignore,
                                                                          crit_e::ignore,
                                                                          crit_e::ignore};

// RICserviceLoadStatusFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_service_load_status_fail_ies_container_values[] = {
    {offsetof(ric_service_load_status_fail_ies_container, ric_load_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(ric_service_load_status_fail_ies_container, e2node_load_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(ric_service_load_status_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(ric_service_load_status_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(ric_service_load_status_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_ric_service_load_status_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_service_load_status_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc ric_service_load_status_fail_ies_container::desc = {
    objset_id_ric_service_load_status_fail_ies_o,
    objset_crit_ric_service_load_status_fail_ies_o,
    ie_table_ric_service_load_status_fail_ies_container_values,
    ie_table_ric_service_load_status_fail_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_service_load_status_request_ies_o[]   = {117, 105, 112, 111, 113};
static const uint8_t  objset_crit_ric_service_load_status_request_ies_o[] = {crit_e::reject,
                                                                             crit_e::ignore,
                                                                             crit_e::ignore,
                                                                             crit_e::reject,
                                                                             crit_e::ignore};

// RICserviceLoadStatusRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_service_load_status_request_ies_container_values[] = {
    {offsetof(ric_service_load_status_request_ies_container, ric_load_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(ric_service_load_status_request_ies_container, e2node_load_meas_id),
     (int32_t)offsetof(ric_service_load_status_request_ies_container, e2node_load_meas_id_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(ric_service_load_status_request_ies_container, regist_request),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ric_service_load_status_request_ies_container::regist_request)>(),
     decltype(ric_service_load_status_request_ies_container::regist_request)::has_ext,
     false,
     decltype(ric_service_load_status_request_ies_container::regist_request)::nof_types,
     decltype(ric_service_load_status_request_ies_container::regist_request)::nof_exts,
     nullptr,
     &regist_request_opts::desc},
    {offsetof(ric_service_load_status_request_ies_container, ran_function_load_request_list),
     (int32_t)offsetof(ric_service_load_status_request_ies_container, ran_function_load_request_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(ric_service_load_status_request_ies_container::ran_function_load_request_list),
                              asn1::table::struct_json>::value},
    {offsetof(ric_service_load_status_request_ies_container, report_periodicity),
     (int32_t)offsetof(ric_service_load_status_request_ies_container, report_periodicity_present),
     asn1::table::enum_op<decltype(ric_service_load_status_request_ies_container::report_periodicity)>(),
     decltype(ric_service_load_status_request_ies_container::report_periodicity)::has_ext,
     false,
     decltype(ric_service_load_status_request_ies_container::report_periodicity)::nof_types,
     decltype(ric_service_load_status_request_ies_container::report_periodicity)::nof_exts,
     nullptr,
     &report_periodicity_opts::desc}};
static const int32_t ie_table_ric_service_load_status_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_service_load_status_request_ies_container, e2node_load_meas_id_present),
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_service_load_status_request_ies_container, ran_function_load_request_list_present),
    (int32_t)offsetof(ric_service_load_status_request_ies_container, report_periodicity_present)};
const asn1::table::ie_container_desc ric_service_load_status_request_ies_container::desc = {
    objset_id_ric_service_load_status_request_ies_o,
    objset_crit_ric_service_load_status_request_ies_o,
    ie_table_ric_service_load_status_request_ies_container_values,
    ie_table_ric_service_load_status_request_ies_container_presences,
    5,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_service_load_status_resp_ies_o[]   = {117, 105, 109};
static const uint8_t  objset_crit_ric_service_load_status_resp_ies_o[] = {crit_e::reject,
                                                                          crit_e::ignore,
                                                                          crit_e::reject};

// RICserviceLoadStatusResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_service_load_status_resp_ies_container_values[] = {
    {offsetof(ric_service_load_status_resp_ies_container, ric_load_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(ric_service_load_status_resp_ies_container, e2node_load_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(ric_service_load_status_resp_ies_container, ran_function_load_confirm_list),
     (int32_t)offsetof(ric_service_load_status_resp_ies_container, ran_function_load_confirm_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(ric_service_load_status_resp_ies_container::ran_function_load_confirm_list),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_ric_service_load_status_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_service_load_status_resp_ies_container, ran_function_load_confirm_list_present)};
const asn1::table::ie_container_desc ric_service_load_status_resp_ies_container::desc = {
    objset_id_ric_service_load_status_resp_ies_o,
    objset_crit_ric_service_load_status_resp_ies_o,
    ie_table_ric_service_load_status_resp_ies_container_values,
    ie_table_ric_service_load_status_resp_ies_container_presences,
    3,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_service_load_upd_ies_o[]   = {117, 105, 107};
static const uint8_t  objset_crit_ric_service_load_upd_ies_o[] = {crit_e::reject, crit_e::ignore, crit_e::reject};

// RICserviceLoadUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_service_load_upd_ies_container_values[] = {
    {offsetof(ric_service_load_upd_ies_container, ric_load_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(ric_service_load_upd_ies_container, e2node_load_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(ric_service_load_upd_ies_container, ran_function_load_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(ric_service_load_upd_ies_container::ran_function_load_list),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_ric_service_load_upd_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                                 asn1::table::seq_field::mandatory,
                                                                                 asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc ric_service_load_upd_ies_container::desc = {
    objset_id_ric_service_load_upd_ies_o,
    objset_crit_ric_service_load_upd_ies_o,
    ie_table_ric_service_load_upd_ies_container_values,
    ie_table_ric_service_load_upd_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_service_query_ies_o[]   = {49, 9};
static const uint8_t  objset_crit_ric_service_query_ies_o[] = {crit_e::reject, crit_e::reject};

// RICserviceQuery-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_service_query_ies_container_values[] = {
    {offsetof(ric_service_query_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_service_query_ies_container, ran_functions_accepted),
     (int32_t)offsetof(ric_service_query_ies_container, ran_functions_accepted_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(ric_service_query_ies_container::ran_functions_accepted),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_ric_service_query_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_service_query_ies_container, ran_functions_accepted_present)};
const asn1::table::ie_container_desc ric_service_query_ies_container::desc = {
    objset_id_ric_service_query_ies_o,
    objset_crit_ric_service_query_ies_o,
    ie_table_ric_service_query_ies_container_values,
    ie_table_ric_service_query_ies_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_service_upd_ies_o[]   = {49, 10, 12, 11};
static const uint8_t  objset_crit_ric_service_upd_ies_o[] = {crit_e::reject,
                                                             crit_e::reject,
                                                             crit_e::reject,
                                                             crit_e::reject};

// RICserviceUpdate-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_service_upd_ies_container_values[] = {
    {offsetof(ric_service_upd_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_service_upd_ies_container, ran_functions_added),
     (int32_t)offsetof(ric_service_upd_ies_container, ran_functions_added_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(ric_service_upd_ies_container::ran_functions_added),
                              asn1::table::struct_json>::value},
    {offsetof(ric_service_upd_ies_container, ran_functions_modified),
     (int32_t)offsetof(ric_service_upd_ies_container, ran_functions_modified_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(ric_service_upd_ies_container::ran_functions_modified),
                              asn1::table::struct_json>::value},
    {offsetof(ric_service_upd_ies_container, ran_functions_deleted),
     (int32_t)offsetof(ric_service_upd_ies_container, ran_functions_deleted_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(ric_service_upd_ies_container::ran_functions_deleted),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_ric_service_upd_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_service_upd_ies_container, ran_functions_added_present),
    (int32_t)offsetof(ric_service_upd_ies_container, ran_functions_modified_present),
    (int32_t)offsetof(ric_service_upd_ies_container, ran_functions_deleted_present)};
const asn1::table::ie_container_desc ric_service_upd_ies_container::desc = {
    objset_id_ric_service_upd_ies_o,
    objset_crit_ric_service_upd_ies_o,
    ie_table_ric_service_upd_ies_container_values,
    ie_table_ric_service_upd_ies_container_presences,
    4,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_service_upd_ack_ies_o[]   = {49, 9, 13, 138};
static const uint8_t  objset_crit_ric_service_upd_ack_ies_o[] = {crit_e::reject,
                                                                 crit_e::reject,
                                                                 crit_e::reject,
                                                                 crit_e::reject};

// RICserviceUpdateAcknowledge-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_service_upd_ack_ies_container_values[] = {
    {offsetof(ric_service_upd_ack_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_service_upd_ack_ies_container, ran_functions_accepted),
     (int32_t)offsetof(ric_service_upd_ack_ies_container, ran_functions_accepted_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(ric_service_upd_ack_ies_container::ran_functions_accepted),
                              asn1::table::struct_json>::value},
    {offsetof(ric_service_upd_ack_ies_container, ran_functions_rejected),
     (int32_t)offsetof(ric_service_upd_ack_ies_container, ran_functions_rejected_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(ric_service_upd_ack_ies_container::ran_functions_rejected),
                              asn1::table::struct_json>::value},
    {offsetof(ric_service_upd_ack_ies_container, ran_functions_accept_modified_list),
     (int32_t)offsetof(ric_service_upd_ack_ies_container, ran_functions_accept_modified_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(ric_service_upd_ack_ies_container::ran_functions_accept_modified_list),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_ric_service_upd_ack_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_service_upd_ack_ies_container, ran_functions_accepted_present),
    (int32_t)offsetof(ric_service_upd_ack_ies_container, ran_functions_rejected_present),
    (int32_t)offsetof(ric_service_upd_ack_ies_container, ran_functions_accept_modified_list_present)};
const asn1::table::ie_container_desc ric_service_upd_ack_ies_container::desc = {
    objset_id_ric_service_upd_ack_ies_o,
    objset_crit_ric_service_upd_ack_ies_o,
    ie_table_ric_service_upd_ack_ies_container_values,
    ie_table_ric_service_upd_ack_ies_container_presences,
    4,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_service_upd_fail_ies_o[]   = {49, 1, 31, 2};
static const uint8_t  objset_crit_ric_service_upd_fail_ies_o[] = {crit_e::reject,
                                                                  crit_e::reject,
                                                                  crit_e::ignore,
                                                                  crit_e::ignore};

// RICserviceUpdateFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_service_upd_fail_ies_container_values[] = {
    {offsetof(ric_service_upd_fail_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(ric_service_upd_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(ric_service_upd_fail_ies_container, time_to_wait),
     (int32_t)offsetof(ric_service_upd_fail_ies_container, time_to_wait_present),
     asn1::table::enum_op<decltype(ric_service_upd_fail_ies_container::time_to_wait)>(),
     decltype(ric_service_upd_fail_ies_container::time_to_wait)::has_ext,
     false,
     decltype(ric_service_upd_fail_ies_container::time_to_wait)::nof_types,
     decltype(ric_service_upd_fail_ies_container::time_to_wait)::nof_exts,
     nullptr,
     &time_to_wait_opts::desc},
    {offsetof(ric_service_upd_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(ric_service_upd_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_ric_service_upd_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_service_upd_fail_ies_container, time_to_wait_present),
    (int32_t)offsetof(ric_service_upd_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc ric_service_upd_fail_ies_container::desc = {
    objset_id_ric_service_upd_fail_ies_o,
    objset_crit_ric_service_upd_fail_ies_o,
    ie_table_ric_service_upd_fail_ies_container_values,
    ie_table_ric_service_upd_fail_ies_container_presences,
    4,
    2,
    0,
    65535,
    true,
    "value"};

// RICsubscription-withCause-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_sub_with_cause_item_s_fields[] = {
    {offsetof(ric_sub_with_cause_item_s, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_with_cause_item_s, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_sub_with_cause_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const asn1::table::field_name seq_table_ric_sub_with_cause_item_s_names[] = {{"ricRequestID", nullptr},
                                                                                    {"ranFunctionID", nullptr},
                                                                                    {"cause", nullptr}};
const asn1::table::seq_desc          ric_sub_with_cause_item_s::desc = {{seq_table_ric_sub_with_cause_item_s_fields,
                                                                         uint64_t{0x7},
                                                                         nullptr,
                                                                         seq_table_ric_sub_with_cause_item_s_names,
                                                                         3,
                                                                         0},
                                                                        true,
                                                                        true,
                                                                        nullptr,
                                                                        0};

// RICsubscription-withCause-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_sub_with_cause_item_ies_o[]    = {61};
static const uint8_t  objset_crit_ric_sub_with_cause_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_sub_with_cause_item_ies_o[] = {
    ric_sub_with_cause_item_ies_o::value_c::types::ric_sub_with_cause_item};
static const uint8_t                    objset_presence_ric_sub_with_cause_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_sub_with_cause_item_ies_o[]  = {
    {objset_crit_ric_sub_with_cause_item_ies_o, crit_e::nulltype},
    {objset_value_ric_sub_with_cause_item_ies_o, ric_sub_with_cause_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_sub_with_cause_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_sub_with_cause_item_ies_o::desc = {objset_id_ric_sub_with_cause_item_ies_o,
                                                                      1,
                                                                      "id",
                                                                      objset_columns_ric_sub_with_cause_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_sub_with_cause_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_sub_with_cause_item_s::desc}};
static const asn1::table::field_name choice_table_ric_sub_with_cause_item_ies_o_value_c_names[] = {
    {"RICsubscription-withCause-Item", nullptr}};
const asn1::table::choice_desc ric_sub_with_cause_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_sub_with_cause_item_ies_o::value_c::types>(),
    ric_sub_with_cause_item_ies_o::value_c::types::nof_types,
    ric_sub_with_cause_item_ies_o::value_c::types::nof_exts,
    ric_sub_with_cause_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_sub_with_cause_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_sub_with_cause_item_ies_o_value_c_names,
    "ric_sub_with_cause_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_sub_with_cause_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_sub_with_cause_item_ies_o_value_c_types_values[] = {"RICsubscription-withCause-Item"};
const asn1::table::enum_desc ric_sub_with_cause_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_sub_with_cause_item_ies_o_value_c_types_values,
    1,
    "ric_sub_with_cause_item_ies_o::value_c::types"};

// RICsubscriptionAudit-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_sub_audit_item_s_fields[] = {
    {offsetof(ric_sub_audit_item_s, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_audit_item_s, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr}};
static const asn1::table::field_name seq_table_ric_sub_audit_item_s_names[] = {{"ricRequestID", nullptr},
                                                                               {"ranFunctionID", nullptr}};
const asn1::table::seq_desc          ric_sub_audit_item_s::desc             = {
    {seq_table_ric_sub_audit_item_s_fields, uint64_t{0x3}, nullptr, seq_table_ric_sub_audit_item_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// RICsubscriptionAudit-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_sub_audit_item_ies_o[]    = {100};
static const uint8_t  objset_crit_ric_sub_audit_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_sub_audit_item_ies_o[] = {
    ric_sub_audit_item_ies_o::value_c::types::ric_sub_audit_item};
static const uint8_t                    objset_presence_ric_sub_audit_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_sub_audit_item_ies_o[]  = {
    {objset_crit_ric_sub_audit_item_ies_o, crit_e::nulltype},
    {objset_value_ric_sub_audit_item_ies_o, ric_sub_audit_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_sub_audit_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_sub_audit_item_ies_o::desc = {objset_id_ric_sub_audit_item_ies_o,
                                                                 1,
                                                                 "id",
                                                                 objset_columns_ric_sub_audit_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_sub_audit_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_sub_audit_item_s::desc}};
static const asn1::table::field_name choice_table_ric_sub_audit_item_ies_o_value_c_names[] = {
    {"RICsubscriptionAudit-Item", nullptr}};
const asn1::table::choice_desc ric_sub_audit_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_sub_audit_item_ies_o::value_c::types>(),
    ric_sub_audit_item_ies_o::value_c::types::nof_types,
    ric_sub_audit_item_ies_o::value_c::types::nof_exts,
    ric_sub_audit_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_sub_audit_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_sub_audit_item_ies_o_value_c_names,
    "ric_sub_audit_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_sub_audit_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_ric_sub_audit_item_ies_o_value_c_types_values[] = {"RICsubscriptionAudit-Item"};
const asn1::table::enum_desc ric_sub_audit_item_ies_o::value_c::types_opts::desc        = {
    enum_names_ric_sub_audit_item_ies_o_value_c_types_values,
    1,
    "ric_sub_audit_item_ies_o::value_c::types"};

// RICsubscriptionAuditAction-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_sub_audit_action_item_s_fields[] = {
    {offsetof(ric_sub_audit_action_item_s, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_audit_action_item_s, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_sub_audit_action_item_s, ric_action_admitted_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     15,
     &asn1::table::list_codec<decltype(ric_sub_audit_action_item_s::ric_action_admitted_list),
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ric_sub_audit_action_item_s_names[] = {
    {"ricRequestID", nullptr},
    {"ranFunctionID", nullptr},
    {"ricAction-Admitted-List", nullptr}};
const asn1::table::seq_desc ric_sub_audit_action_item_s::desc = {{seq_table_ric_sub_audit_action_item_s_fields,
                                                                  uint64_t{0x7},
                                                                  nullptr,
                                                                  seq_table_ric_sub_audit_action_item_s_names,
                                                                  3,
                                                                  0},
                                                                 true,
                                                                 true,
                                                                 nullptr,
                                                                 0};

// RICsubscriptionAuditAction-ItemIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const uint32_t objset_id_ric_sub_audit_action_item_ies_o[]    = {101};
static const uint8_t  objset_crit_ric_sub_audit_action_item_ies_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ric_sub_audit_action_item_ies_o[] = {
    ric_sub_audit_action_item_ies_o::value_c::types::ric_sub_audit_action_item};
static const uint8_t                    objset_presence_ric_sub_audit_action_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ric_sub_audit_action_item_ies_o[]  = {
    {objset_crit_ric_sub_audit_action_item_ies_o, crit_e::nulltype},
    {objset_value_ric_sub_audit_action_item_ies_o, ric_sub_audit_action_item_ies_o::value_c::types::nulltype},
    {objset_presence_ric_sub_audit_action_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc ric_sub_audit_action_item_ies_o::desc = {objset_id_ric_sub_audit_action_item_ies_o,
                                                                        1,
                                                                        "id",
                                                                        objset_columns_ric_sub_audit_action_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ric_sub_audit_action_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_sub_audit_action_item_s::desc}};
static const asn1::table::field_name choice_table_ric_sub_audit_action_item_ies_o_value_c_names[] = {
    {"RICsubscriptionAuditAction-Item", nullptr}};
const asn1::table::choice_desc ric_sub_audit_action_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<ric_sub_audit_action_item_ies_o::value_c::types>(),
    ric_sub_audit_action_item_ies_o::value_c::types::nof_types,
    ric_sub_audit_action_item_ies_o::value_c::types::nof_exts,
    ric_sub_audit_action_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_ric_sub_audit_action_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_ric_sub_audit_action_item_ies_o_value_c_names,
    "ric_sub_audit_action_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ric_sub_audit_action_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ric_sub_audit_action_item_ies_o_value_c_types_values[] = {
    "RICsubscriptionAuditAction-Item"};
const asn1::table::enum_desc ric_sub_audit_action_item_ies_o::value_c::types_opts::desc = {
    enum_names_ric_sub_audit_action_item_ies_o_value_c_types_values,
    1,
    "ric_sub_audit_action_item_ies_o::value_c::types"};

static const uint32_t objset_id_ric_sub_audit_fail_ies_o[]   = {29, 1, 2};
static const uint8_t  objset_crit_ric_sub_audit_fail_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::ignore};

// RICsubscriptionAuditFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_audit_fail_ies_container_values[] = {
    {offsetof(ric_sub_audit_fail_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_audit_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(ric_sub_audit_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(ric_sub_audit_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_ric_sub_audit_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_sub_audit_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc ric_sub_audit_fail_ies_container::desc = {
    objset_id_ric_sub_audit_fail_ies_o,
    objset_crit_ric_sub_audit_fail_ies_o,
    ie_table_ric_sub_audit_fail_ies_container_values,
    ie_table_ric_sub_audit_fail_ies_container_presences,
    3,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_sub_audit_request_ies_o[]   = {29, 98, 99};
static const uint8_t  objset_crit_ric_sub_audit_request_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::reject};

// RICsubscriptionAuditRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_audit_request_ies_container_values[] = {
    {offsetof(ric_sub_audit_request_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_audit_request_ies_container, ric_sub_audit_flag),
     (int32_t)offsetof(ric_sub_audit_request_ies_container, ric_sub_audit_flag_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_sub_audit_flag_s::desc},
    {offsetof(ric_sub_audit_request_ies_container, ric_sub_audit_list),
     (int32_t)offsetof(ric_sub_audit_request_ies_container, ric_sub_audit_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2147483647,
     &asn1::table::list_codec<decltype(ric_sub_audit_request_ies_container::ric_sub_audit_list),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_ric_sub_audit_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_sub_audit_request_ies_container, ric_sub_audit_flag_present),
    (int32_t)offsetof(ric_sub_audit_request_ies_container, ric_sub_audit_list_present)};
const asn1::table::ie_container_desc ric_sub_audit_request_ies_container::desc = {
    objset_id_ric_sub_audit_request_ies_o,
    objset_crit_ric_sub_audit_request_ies_o,
    ie_table_ric_sub_audit_request_ies_container_values,
    ie_table_ric_sub_audit_request_ies_container_presences,
    3,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_sub_audit_resp_ies_o[]   = {29, 102, 104, 103};
static const uint8_t  objset_crit_ric_sub_audit_resp_ies_o[] = {crit_e::reject,
                                                                crit_e::reject,
                                                                crit_e::reject,
                                                                crit_e::reject};

// RICsubscriptionAuditResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_audit_resp_ies_container_values[] = {
    {offsetof(ric_sub_audit_resp_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_audit_resp_ies_container, ric_sub_audit_confirmed_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2147483647,
     &asn1::table::list_codec<decltype(ric_sub_audit_resp_ies_container::ric_sub_audit_confirmed_list),
                              asn1::table::struct_json>::value},
    {offsetof(ric_sub_audit_resp_ies_container, ric_sub_audit_unkown_list),
     (int32_t)offsetof(ric_sub_audit_resp_ies_container, ric_sub_audit_unkown_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2147483647,
     &asn1::table::list_codec<decltype(ric_sub_audit_resp_ies_container::ric_sub_audit_unkown_list),
                              asn1::table::struct_json>::value},
    {offsetof(ric_sub_audit_resp_ies_container, ric_sub_audit_missing_list),
     (int32_t)offsetof(ric_sub_audit_resp_ies_container, ric_sub_audit_missing_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2147483647,
     &asn1::table::list_codec<decltype(ric_sub_audit_resp_ies_container::ric_sub_audit_missing_list),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_ric_sub_audit_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_sub_audit_resp_ies_container, ric_sub_audit_unkown_list_present),
    (int32_t)offsetof(ric_sub_audit_resp_ies_container, ric_sub_audit_missing_list_present)};
const asn1::table::ie_container_desc ric_sub_audit_resp_ies_container::desc = {
    objset_id_ric_sub_audit_resp_ies_o,
    objset_crit_ric_sub_audit_resp_ies_o,
    ie_table_ric_sub_audit_resp_ies_container_values,
    ie_table_ric_sub_audit_resp_ies_container_presences,
    4,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_sub_delete_fail_ies_o[]   = {29, 5, 1, 2};
static const uint8_t  objset_crit_ric_sub_delete_fail_ies_o[] = {crit_e::reject,
                                                                 crit_e::reject,
                                                                 crit_e::ignore,
                                                                 crit_e::ignore};

// RICsubscriptionDeleteFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_delete_fail_ies_container_values[] = {
    {offsetof(ric_sub_delete_fail_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_delete_fail_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_sub_delete_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(ric_sub_delete_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(ric_sub_delete_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_ric_sub_delete_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_sub_delete_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc ric_sub_delete_fail_ies_container::desc = {
    objset_id_ric_sub_delete_fail_ies_o,
    objset_crit_ric_sub_delete_fail_ies_o,
    ie_table_ric_sub_delete_fail_ies_container_values,
    ie_table_ric_sub_delete_fail_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_sub_delete_request_ies_o[]   = {29, 5};
static const uint8_t  objset_crit_ric_sub_delete_request_ies_o[] = {crit_e::reject, crit_e::reject};

// RICsubscriptionDeleteRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_delete_request_ies_container_values[] = {
    {offsetof(ric_sub_delete_request_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_delete_request_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr}};
static const int32_t ie_table_ric_sub_delete_request_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                                   asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc ric_sub_delete_request_ies_container::desc = {
    objset_id_ric_sub_delete_request_ies_o,
    objset_crit_ric_sub_delete_request_ies_o,
    ie_table_ric_sub_delete_request_ies_container_values,
    ie_table_ric_sub_delete_request_ies_container_presences,
    2,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_sub_delete_required_ies_o[]   = {60};
static const uint8_t  objset_crit_ric_sub_delete_required_ies_o[] = {crit_e::ignore};

// RICsubscriptionDeleteRequired-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_delete_required_ies_container_values[] = {
    {offsetof(ric_sub_delete_required_ies_container, ric_sub_to_be_remd),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::list_codec<decltype(ric_sub_delete_required_ies_container::ric_sub_to_be_remd),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_ric_sub_delete_required_ies_container_presences[]  = {asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc ric_sub_delete_required_ies_container::desc = {
    objset_id_ric_sub_delete_required_ies_o,
    objset_crit_ric_sub_delete_required_ies_o,
    ie_table_ric_sub_delete_required_ies_container_values,
    ie_table_ric_sub_delete_required_ies_container_presences,
    1,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_sub_delete_resp_ies_o[]   = {29, 5};
static const uint8_t  objset_crit_ric_sub_delete_resp_ies_o[] = {crit_e::reject, crit_e::reject};

// RICsubscriptionDeleteResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_delete_resp_ies_container_values[] = {
    {offsetof(ric_sub_delete_resp_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_delete_resp_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr}};
static const int32_t ie_table_ric_sub_delete_resp_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                                asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc ric_sub_delete_resp_ies_container::desc = {
    objset_id_ric_sub_delete_resp_ies_o,
    objset_crit_ric_sub_delete_resp_ies_o,
    ie_table_ric_sub_delete_resp_ies_container_values,
    ie_table_ric_sub_delete_resp_ies_container_presences,
    2,
    2,
    0,
    65535,
    true,
    "value"};

// RICsubscriptionDetails ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_sub_details_s_fields[] = {
    {offsetof(ric_sub_details_s, ric_event_trigger_definition),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_sub_details_s::ric_event_trigger_definition),
                                asn1::table::string_json>::value},
    {offsetof(ric_sub_details_s, ric_action_to_be_setup_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     15,
     &asn1::table::list_codec<decltype(ric_sub_details_s::ric_action_to_be_setup_list),
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ric_sub_details_s_names[] = {{"ricEventTriggerDefinition", nullptr},
                                                                            {"ricAction-ToBeSetup-List", nullptr}};
const asn1::table::seq_desc          ric_sub_details_s::desc             = {
    {seq_table_ric_sub_details_s_fields, uint64_t{0x3}, nullptr, seq_table_ric_sub_details_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

static const uint32_t objset_id_ric_sub_fail_ies_o[]   = {29, 5, 1, 2};
static const uint8_t  objset_crit_ric_sub_fail_ies_o[] = {crit_e::reject,
                                                          crit_e::reject,
                                                          crit_e::reject,
                                                          crit_e::ignore};

// RICsubscriptionFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_fail_ies_container_values[] = {
    {offsetof(ric_sub_fail_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_fail_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_sub_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(ric_sub_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(ric_sub_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_ric_sub_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_sub_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc ric_sub_fail_ies_container::desc = {objset_id_ric_sub_fail_ies_o,
                                                                         objset_crit_ric_sub_fail_ies_o,
                                                                         ie_table_ric_sub_fail_ies_container_values,
                                                                         ie_table_ric_sub_fail_ies_container_presences,
                                                                         4,
                                                                         3,
                                                                         0,
                                                                         65535,
                                                                         true,
                                                                         "value"};

static const uint32_t objset_id_ric_sub_mod_confirm_ies_o[] = {29, 5, 87, 89, 91, 93};
static const uint8_t  objset_crit_ric_sub_mod_confirm_ies_o[] =
    {crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// RICsubscriptionModificationConfirm-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_mod_confirm_ies_container_values[] = {
    {offsetof(ric_sub_mod_confirm_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_mod_confirm_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_sub_mod_confirm_ies_container, ric_actions_confirmed_for_mod_list),
     (int32_t)offsetof(ric_sub_mod_confirm_ies_container, ric_actions_confirmed_for_mod_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_mod_confirm_ies_container::ric_actions_confirmed_for_mod_list),
                              asn1::table::struct_json>::value},
    {offsetof(ric_sub_mod_confirm_ies_container, ric_actions_refused_to_be_modified_list),
     (int32_t)offsetof(ric_sub_mod_confirm_ies_container, ric_actions_refused_to_be_modified_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_mod_confirm_ies_container::ric_actions_refused_to_be_modified_list),
                              asn1::table::struct_json>::value},
    {offsetof(ric_sub_mod_confirm_ies_container, ric_actions_confirmed_for_removal_list),
     (int32_t)offsetof(ric_sub_mod_confirm_ies_container, ric_actions_confirmed_for_removal_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_mod_confirm_ies_container::ric_actions_confirmed_for_removal_list),
                              asn1::table::struct_json>::value},
    {offsetof(ric_sub_mod_confirm_ies_container, ric_actions_refused_to_be_remd_list),
     (int32_t)offsetof(ric_sub_mod_confirm_ies_container, ric_actions_refused_to_be_remd_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_mod_confirm_ies_container::ric_actions_refused_to_be_remd_list),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_ric_sub_mod_confirm_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_sub_mod_confirm_ies_container, ric_actions_confirmed_for_mod_list_present),
    (int32_t)offsetof(ric_sub_mod_confirm_ies_container, ric_actions_refused_to_be_modified_list_present),
    (int32_t)offsetof(ric_sub_mod_confirm_ies_container, ric_actions_confirmed_for_removal_list_present),
    (int32_t)offsetof(ric_sub_mod_confirm_ies_container, ric_actions_refused_to_be_remd_list_present)};
const asn1::table::ie_container_desc ric_sub_mod_confirm_ies_container::desc = {
    objset_id_ric_sub_mod_confirm_ies_o,
    objset_crit_ric_sub_mod_confirm_ies_o,
    ie_table_ric_sub_mod_confirm_ies_container_values,
    ie_table_ric_sub_mod_confirm_ies_container_presences,
    6,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_sub_mod_fail_ies_o[]   = {29, 5, 1, 2};
static const uint8_t  objset_crit_ric_sub_mod_fail_ies_o[] = {crit_e::reject,
                                                              crit_e::reject,
                                                              crit_e::reject,
                                                              crit_e::ignore};

// RICsubscriptionModificationFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_mod_fail_ies_container_values[] = {
    {offsetof(ric_sub_mod_fail_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_mod_fail_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_sub_mod_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(ric_sub_mod_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(ric_sub_mod_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_ric_sub_mod_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_sub_mod_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc ric_sub_mod_fail_ies_container::desc = {
    objset_id_ric_sub_mod_fail_ies_o,
    objset_crit_ric_sub_mod_fail_ies_o,
    ie_table_ric_sub_mod_fail_ies_container_values,
    ie_table_ric_sub_mod_fail_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_sub_mod_refuse_ies_o[]   = {29, 5, 1, 2};
static const uint8_t  objset_crit_ric_sub_mod_refuse_ies_o[] = {crit_e::reject,
                                                                crit_e::reject,
                                                                crit_e::reject,
                                                                crit_e::ignore};

// RICsubscriptionModificationRefuse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_mod_refuse_ies_container_values[] = {
    {offsetof(ric_sub_mod_refuse_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_mod_refuse_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_sub_mod_refuse_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(ric_sub_mod_refuse_ies_container, crit_diagnostics),
     (int32_t)offsetof(ric_sub_mod_refuse_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_ric_sub_mod_refuse_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_sub_mod_refuse_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc ric_sub_mod_refuse_ies_container::desc = {
    objset_id_ric_sub_mod_refuse_ies_o,
    objset_crit_ric_sub_mod_refuse_ies_o,
    ie_table_ric_sub_mod_refuse_ies_container_values,
    ie_table_ric_sub_mod_refuse_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_sub_mod_request_ies_o[] = {29, 5, 64, 65, 67, 69};
static const uint8_t  objset_crit_ric_sub_mod_request_ies_o[] =
    {crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// RICsubscriptionModificationRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_mod_request_ies_container_values[] = {
    {offsetof(ric_sub_mod_request_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_mod_request_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_sub_mod_request_ies_container, ric_event_trigger_definition_to_be_modified),
     (int32_t)offsetof(ric_sub_mod_request_ies_container, ric_event_trigger_definition_to_be_modified_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<
         decltype(ric_sub_mod_request_ies_container::ric_event_trigger_definition_to_be_modified),
         asn1::table::string_json>::value},
    {offsetof(ric_sub_mod_request_ies_container, ric_actions_to_be_remd_for_mod_list),
     (int32_t)offsetof(ric_sub_mod_request_ies_container, ric_actions_to_be_remd_for_mod_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_mod_request_ies_container::ric_actions_to_be_remd_for_mod_list),
                              asn1::table::struct_json>::value},
    {offsetof(ric_sub_mod_request_ies_container, ric_actions_to_be_modified_for_mod_list),
     (int32_t)offsetof(ric_sub_mod_request_ies_container, ric_actions_to_be_modified_for_mod_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_mod_request_ies_container::ric_actions_to_be_modified_for_mod_list),
                              asn1::table::struct_json>::value},
    {offsetof(ric_sub_mod_request_ies_container, ric_actions_to_be_added_for_mod_list),
     (int32_t)offsetof(ric_sub_mod_request_ies_container, ric_actions_to_be_added_for_mod_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_mod_request_ies_container::ric_actions_to_be_added_for_mod_list),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_ric_sub_mod_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_sub_mod_request_ies_container, ric_event_trigger_definition_to_be_modified_present),
    (int32_t)offsetof(ric_sub_mod_request_ies_container, ric_actions_to_be_remd_for_mod_list_present),
    (int32_t)offsetof(ric_sub_mod_request_ies_container, ric_actions_to_be_modified_for_mod_list_present),
    (int32_t)offsetof(ric_sub_mod_request_ies_container, ric_actions_to_be_added_for_mod_list_present)};
const asn1::table::ie_container_desc ric_sub_mod_request_ies_container::desc = {
    objset_id_ric_sub_mod_request_ies_o,
    objset_crit_ric_sub_mod_request_ies_o,
    ie_table_ric_sub_mod_request_ies_container_values,
    ie_table_ric_sub_mod_request_ies_container_presences,
    6,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_sub_mod_required_ies_o[]   = {29, 5, 83, 85};
static const uint8_t  objset_crit_ric_sub_mod_required_ies_o[] = {crit_e::reject,
                                                                  crit_e::reject,
                                                                  crit_e::ignore,
                                                                  crit_e::ignore};

// RICsubscriptionModificationRequired-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_mod_required_ies_container_values[] = {
    {offsetof(ric_sub_mod_required_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_mod_required_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_sub_mod_required_ies_container, ric_actions_required_to_be_modified_list),
     (int32_t)offsetof(ric_sub_mod_required_ies_container, ric_actions_required_to_be_modified_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_mod_required_ies_container::ric_actions_required_to_be_modified_list),
                              asn1::table::struct_json>::value},
    {offsetof(ric_sub_mod_required_ies_container, ric_actions_required_to_be_remd_list),
     (int32_t)offsetof(ric_sub_mod_required_ies_container, ric_actions_required_to_be_remd_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_mod_required_ies_container::ric_actions_required_to_be_remd_list),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_ric_sub_mod_required_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_sub_mod_required_ies_container, ric_actions_required_to_be_modified_list_present),
    (int32_t)offsetof(ric_sub_mod_required_ies_container, ric_actions_required_to_be_remd_list_present)};
const asn1::table::ie_container_desc ric_sub_mod_required_ies_container::desc = {
    objset_id_ric_sub_mod_required_ies_o,
    objset_crit_ric_sub_mod_required_ies_o,
    ie_table_ric_sub_mod_required_ies_container_values,
    ie_table_ric_sub_mod_required_ies_container_presences,
    4,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_sub_mod_resp_ies_o[]   = {29, 5, 71, 73, 75, 77, 79, 81};
static const uint8_t  objset_crit_ric_sub_mod_resp_ies_o[] = {crit_e::reject,
                                                              crit_e::reject,
                                                              crit_e::ignore,
                                                              crit_e::ignore,
                                                              crit_e::ignore,
                                                              crit_e::ignore,
                                                              crit_e::ignore,
                                                              crit_e::ignore};

// RICsubscriptionModificationResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_mod_resp_ies_container_values[] = {
    {offsetof(ric_sub_mod_resp_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_mod_resp_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_sub_mod_resp_ies_container, ric_actions_remd_for_mod_list),
     (int32_t)offsetof(ric_sub_mod_resp_ies_container, ric_actions_remd_for_mod_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_mod_resp_ies_container::ric_actions_remd_for_mod_list),
                              asn1::table::struct_json>::value},
    {offsetof(ric_sub_mod_resp_ies_container, ric_actions_failed_to_be_remd_for_mod_list),
     (int32_t)offsetof(ric_sub_mod_resp_ies_container, ric_actions_failed_to_be_remd_for_mod_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_mod_resp_ies_container::ric_actions_failed_to_be_remd_for_mod_list),
                              asn1::table::struct_json>::value},
    {offsetof(ric_sub_mod_resp_ies_container, ric_actions_modified_for_mod_list),
     (int32_t)offsetof(ric_sub_mod_resp_ies_container, ric_actions_modified_for_mod_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_mod_resp_ies_container::ric_actions_modified_for_mod_list),
                              asn1::table::struct_json>::value},
    {offsetof(ric_sub_mod_resp_ies_container, ric_actions_failed_to_be_modified_for_mod_list),
     (int32_t)offsetof(ric_sub_mod_resp_ies_container, ric_actions_failed_to_be_modified_for_mod_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_mod_resp_ies_container::ric_actions_failed_to_be_modified_for_mod_list),
                              asn1::table::struct_json>::value},
    {offsetof(ric_sub_mod_resp_ies_container, ric_actions_added_for_mod_list),
     (int32_t)offsetof(ric_sub_mod_resp_ies_container, ric_actions_added_for_mod_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_mod_resp_ies_container::ric_actions_added_for_mod_list),
                              asn1::table::struct_json>::value},
    {offsetof(ric_sub_mod_resp_ies_container, ric_actions_failed_to_be_added_for_mod_list),
     (int32_t)offsetof(ric_sub_mod_resp_ies_container, ric_actions_failed_to_be_added_for_mod_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_mod_resp_ies_container::ric_actions_failed_to_be_added_for_mod_list),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_ric_sub_mod_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_sub_mod_resp_ies_container, ric_actions_remd_for_mod_list_present),
    (int32_t)offsetof(ric_sub_mod_resp_ies_container, ric_actions_failed_to_be_remd_for_mod_list_present),
    (int32_t)offsetof(ric_sub_mod_resp_ies_container, ric_actions_modified_for_mod_list_present),
    (int32_t)offsetof(ric_sub_mod_resp_ies_container, ric_actions_failed_to_be_modified_for_mod_list_present),
    (int32_t)offsetof(ric_sub_mod_resp_ies_container, ric_actions_added_for_mod_list_present),
    (int32_t)offsetof(ric_sub_mod_resp_ies_container, ric_actions_failed_to_be_added_for_mod_list_present)};
const asn1::table::ie_container_desc ric_sub_mod_resp_ies_container::desc = {
    objset_id_ric_sub_mod_resp_ies_o,
    objset_crit_ric_sub_mod_resp_ies_o,
    ie_table_ric_sub_mod_resp_ies_container_values,
    ie_table_ric_sub_mod_resp_ies_container_presences,
    8,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_sub_request_ies_o[]   = {29, 5, 30, 62, 63};
static const uint8_t  objset_crit_ric_sub_request_ies_o[] = {crit_e::reject,
                                                             crit_e::reject,
                                                             crit_e::reject,
                                                             crit_e::reject,
                                                             crit_e::reject};

// RICsubscriptionRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_request_ies_container_values[] = {
    {offsetof(ric_sub_request_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_request_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_sub_request_ies_container, ric_sub_details),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_sub_details_s::desc},
    {offsetof(ric_sub_request_ies_container, ric_sub_start_time),
     (int32_t)offsetof(ric_sub_request_ies_container, ric_sub_start_time_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_sub_request_ies_container::ric_sub_start_time),
                                asn1::table::string_json>::value},
    {offsetof(ric_sub_request_ies_container, ric_sub_end_time),
     (int32_t)offsetof(ric_sub_request_ies_container, ric_sub_end_time_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_sub_request_ies_container::ric_sub_end_time),
                                asn1::table::string_json>::value}};
static const int32_t ie_table_ric_sub_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_sub_request_ies_container, ric_sub_start_time_present),
    (int32_t)offsetof(ric_sub_request_ies_container, ric_sub_end_time_present)};
const asn1::table::ie_container_desc ric_sub_request_ies_container::desc = {
    objset_id_ric_sub_request_ies_o,
    objset_crit_ric_sub_request_ies_o,
    ie_table_ric_sub_request_ies_container_values,
    ie_table_ric_sub_request_ies_container_presences,
    5,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_sub_resp_ies_o[]   = {29, 5, 17, 18};
static const uint8_t  objset_crit_ric_sub_resp_ies_o[] = {crit_e::reject,
                                                          crit_e::reject,
                                                          crit_e::reject,
                                                          crit_e::reject};

// RICsubscriptionResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_resp_ies_container_values[] = {
    {offsetof(ric_sub_resp_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_resp_ies_container, ran_function_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(ric_sub_resp_ies_container, ric_actions_admitted),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     15,
     &asn1::table::list_codec<decltype(ric_sub_resp_ies_container::ric_actions_admitted),
                              asn1::table::struct_json>::value},
    {offsetof(ric_sub_resp_ies_container, ric_actions_not_admitted),
     (int32_t)offsetof(ric_sub_resp_ies_container, ric_actions_not_admitted_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(ric_sub_resp_ies_container::ric_actions_not_admitted),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_ric_sub_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_sub_resp_ies_container, ric_actions_not_admitted_present)};
const asn1::table::ie_container_desc ric_sub_resp_ies_container::desc = {objset_id_ric_sub_resp_ies_o,
                                                                         objset_crit_ric_sub_resp_ies_o,
                                                                         ie_table_ric_sub_resp_ies_container_values,
                                                                         ie_table_ric_sub_resp_ies_container_presences,
                                                                         4,
                                                                         3,
                                                                         0,
                                                                         65535,
                                                                         true,
                                                                         "value"};

static const uint32_t objset_id_ric_sub_state_ctrl_fail_ies_o[]   = {29, 1, 2};
static const uint8_t  objset_crit_ric_sub_state_ctrl_fail_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::ignore};

// RICsubscriptionStateControlFailure-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_state_ctrl_fail_ies_container_values[] = {
    {offsetof(ric_sub_state_ctrl_fail_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_state_ctrl_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(ric_sub_state_ctrl_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(ric_sub_state_ctrl_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_ric_sub_state_ctrl_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ric_sub_state_ctrl_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc ric_sub_state_ctrl_fail_ies_container::desc = {
    objset_id_ric_sub_state_ctrl_fail_ies_o,
    objset_crit_ric_sub_state_ctrl_fail_ies_o,
    ie_table_ric_sub_state_ctrl_fail_ies_container_values,
    ie_table_ric_sub_state_ctrl_fail_ies_container_presences,
    3,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_sub_state_ctrl_request_ies_o[]   = {29, 124};
static const uint8_t  objset_crit_ric_sub_state_ctrl_request_ies_o[] = {crit_e::reject, crit_e::reject};

// RICsubscriptionStateControlRequest-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_state_ctrl_request_ies_container_values[] = {
    {offsetof(ric_sub_state_ctrl_request_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_state_ctrl_request_ies_container, ran_function_state_ctrl_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(ric_sub_state_ctrl_request_ies_container::ran_function_state_ctrl_list),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_ric_sub_state_ctrl_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc ric_sub_state_ctrl_request_ies_container::desc = {
    objset_id_ric_sub_state_ctrl_request_ies_o,
    objset_crit_ric_sub_state_ctrl_request_ies_o,
    ie_table_ric_sub_state_ctrl_request_ies_container_values,
    ie_table_ric_sub_state_ctrl_request_ies_container_presences,
    2,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ric_sub_state_ctrl_resp_ies_o[]   = {29, 122};
static const uint8_t  objset_crit_ric_sub_state_ctrl_resp_ies_o[] = {crit_e::reject, crit_e::reject};

// RICsubscriptionStateControlResponse-IEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ric_sub_state_ctrl_resp_ies_container_values[] = {
    {offsetof(ric_sub_state_ctrl_resp_ies_container, ric_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(ric_sub_state_ctrl_resp_ies_container, ran_function_state_confirm_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::list_codec<decltype(ric_sub_state_ctrl_resp_ies_container::ran_function_state_confirm_list),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_ric_sub_state_ctrl_resp_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                                    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc ric_sub_state_ctrl_resp_ies_container::desc = {
    objset_id_ric_sub_state_ctrl_resp_ies_o,
    objset_crit_ric_sub_state_ctrl_resp_ies_o,
    ie_table_ric_sub_state_ctrl_resp_ies_container_values,
    ie_table_ric_sub_state_ctrl_resp_ies_container_presences,
    2,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_reset_request_ies_o[]   = {49, 1};
static const uint8_t  objset_crit_reset_request_ies_o[] = {crit_e::reject, crit_e::ignore};

// ResetRequestIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_reset_request_ies_container_values[] = {
    {offsetof(reset_request_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(reset_request_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const int32_t ie_table_reset_request_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                          asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc reset_request_ies_container::desc = {
    objset_id_reset_request_ies_o,
    objset_crit_reset_request_ies_o,
    ie_table_reset_request_ies_container_values,
    ie_table_reset_request_ies_container_presences,
    2,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_reset_resp_ies_o[]   = {49, 2};
static const uint8_t  objset_crit_reset_resp_ies_o[] = {crit_e::reject, crit_e::ignore};

// ResetResponseIEs ::= OBJECT SET OF E2AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_reset_resp_ies_container_values[] = {
    {offsetof(reset_resp_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(reset_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(reset_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_reset_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(reset_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc reset_resp_ies_container::desc = {objset_id_reset_resp_ies_o,
                                                                       objset_crit_reset_resp_ies_o,
                                                                       ie_table_reset_resp_ies_container_values,
                                                                       ie_table_reset_resp_ies_container_presences,
                                                                       2,
                                                                       1,
                                                                       0,
                                                                       65535,
                                                                       true,
                                                                       "value"};
