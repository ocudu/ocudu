// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/nrppa/nrppa_pdu_contents.h"
#include "ocudu/asn1/asn1_ap_codec.h"

using namespace asn1;
using namespace asn1::nrppa;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

static const uint32_t objset_id_aperiodic_srs_ext_ies_o[]   = {122};
static const uint8_t  objset_crit_aperiodic_srs_ext_ies_o[] = {crit_e::ignore};

// AperiodicSRS-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_aperiodic_srs_ext_ies_container_values[] = {
    {offsetof(aperiodic_srs_ext_ies_container, pos_srs_res_set_aggregation_list),
     (int32_t)offsetof(aperiodic_srs_ext_ies_container, pos_srs_res_set_aggregation_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(aperiodic_srs_ext_ies_container::pos_srs_res_set_aggregation_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pos_srs_res_set_aggregation_item_s::desc}};
static const int32_t ie_table_aperiodic_srs_ext_ies_container_presences[] = {
    (int32_t)offsetof(aperiodic_srs_ext_ies_container, pos_srs_res_set_aggregation_list_present)};
const asn1::table::ie_container_desc aperiodic_srs_ext_ies_container::desc = {
    objset_id_aperiodic_srs_ext_ies_o,
    objset_crit_aperiodic_srs_ext_ies_o,
    ie_table_aperiodic_srs_ext_ies_container_values,
    ie_table_aperiodic_srs_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// AperiodicSRS ::= SEQUENCE
static const asn1::table::seq_field seq_table_aperiodic_srs_s_fields[] = {
    {offsetof(aperiodic_srs_s, aperiodic),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(aperiodic_srs_s::aperiodic)>(),
     decltype(aperiodic_srs_s::aperiodic)::has_ext,
     false,
     decltype(aperiodic_srs_s::aperiodic)::nof_types,
     decltype(aperiodic_srs_s::aperiodic)::nof_exts,
     nullptr,
     &aperiodic_srs_s::aperiodic_opts::desc},
    {offsetof(aperiodic_srs_s, srs_res_trigger),
     (int32_t)offsetof(aperiodic_srs_s, srs_res_trigger_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_res_trigger_s::desc},
    {offsetof(aperiodic_srs_s, ie_exts),
     (int32_t)offsetof(aperiodic_srs_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &aperiodic_srs_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_aperiodic_srs_s_names[] = {{"aperiodic", nullptr},
                                                                          {"sRSResourceTrigger", nullptr},
                                                                          {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_aperiodic_srs_s_opts[]  = {
    {1, (int32_t)offsetof(aperiodic_srs_s, srs_res_trigger_present)},
    {2, (int32_t)offsetof(aperiodic_srs_s, ie_exts_present)}};
const asn1::table::seq_desc aperiodic_srs_s::desc = {{seq_table_aperiodic_srs_s_fields,
                                                      uint64_t{0x1},
                                                      seq_table_aperiodic_srs_s_opts,
                                                      seq_table_aperiodic_srs_s_names,
                                                      3,
                                                      2},
                                                     true,
                                                     true,
                                                     nullptr,
                                                     0};

static const char*           enum_names_aperiodic_srs_s_aperiodic_e__values[] = {"true"};
const asn1::table::enum_desc aperiodic_srs_s::aperiodic_opts::desc = {enum_names_aperiodic_srs_s_aperiodic_e__values,
                                                                      1,
                                                                      "aperiodic_srs_s::aperiodic_e_"};

static const uint32_t objset_id_assist_info_ctrl_ies_o[]   = {23, 24, 38};
static const uint8_t  objset_crit_assist_info_ctrl_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::reject};

// AssistanceInformationControl-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_assist_info_ctrl_ies_container_values[] = {
    {offsetof(assist_info_ctrl_ies_container, assist_info),
     (int32_t)offsetof(assist_info_ctrl_ies_container, assist_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &assist_info_s::desc},
    {offsetof(assist_info_ctrl_ies_container, broadcast),
     (int32_t)offsetof(assist_info_ctrl_ies_container, broadcast_present),
     asn1::table::enum_op<decltype(assist_info_ctrl_ies_container::broadcast)>(),
     decltype(assist_info_ctrl_ies_container::broadcast)::has_ext,
     false,
     decltype(assist_info_ctrl_ies_container::broadcast)::nof_types,
     decltype(assist_info_ctrl_ies_container::broadcast)::nof_exts,
     nullptr,
     &broadcast_opts::desc},
    {offsetof(assist_info_ctrl_ies_container, positioning_broadcast_cells),
     (int32_t)offsetof(assist_info_ctrl_ies_container, positioning_broadcast_cells_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16383,
     &asn1::table::item_list_codec<decltype(assist_info_ctrl_ies_container::positioning_broadcast_cells),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ng_ran_cgi_s::desc}};
static const int32_t ie_table_assist_info_ctrl_ies_container_presences[] = {
    (int32_t)offsetof(assist_info_ctrl_ies_container, assist_info_present),
    (int32_t)offsetof(assist_info_ctrl_ies_container, broadcast_present),
    (int32_t)offsetof(assist_info_ctrl_ies_container, positioning_broadcast_cells_present)};
const asn1::table::ie_container_desc assist_info_ctrl_ies_container::desc = {
    objset_id_assist_info_ctrl_ies_o,
    objset_crit_assist_info_ctrl_ies_o,
    ie_table_assist_info_ctrl_ies_container_values,
    ie_table_assist_info_ctrl_ies_container_presences,
    3,
    0,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_assist_info_feedback_ies_o[]   = {25, 38, 1};
static const uint8_t  objset_crit_assist_info_feedback_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::ignore};

// AssistanceInformationFeedback-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_assist_info_feedback_ies_container_values[] = {
    {offsetof(assist_info_feedback_ies_container, assist_info_fail_list),
     (int32_t)offsetof(assist_info_feedback_ies_container, assist_info_fail_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(assist_info_feedback_ies_container::assist_info_fail_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &assist_info_fail_list_item_s_::desc},
    {offsetof(assist_info_feedback_ies_container, positioning_broadcast_cells),
     (int32_t)offsetof(assist_info_feedback_ies_container, positioning_broadcast_cells_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16383,
     &asn1::table::item_list_codec<decltype(assist_info_feedback_ies_container::positioning_broadcast_cells),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ng_ran_cgi_s::desc},
    {offsetof(assist_info_feedback_ies_container, crit_diagnostics),
     (int32_t)offsetof(assist_info_feedback_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_assist_info_feedback_ies_container_presences[] = {
    (int32_t)offsetof(assist_info_feedback_ies_container, assist_info_fail_list_present),
    (int32_t)offsetof(assist_info_feedback_ies_container, positioning_broadcast_cells_present),
    (int32_t)offsetof(assist_info_feedback_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc assist_info_feedback_ies_container::desc = {
    objset_id_assist_info_feedback_ies_o,
    objset_crit_assist_info_feedback_ies_o,
    ie_table_assist_info_feedback_ies_container_values,
    ie_table_assist_info_feedback_ies_container_presences,
    3,
    0,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e_c_id_meas_fail_ind_ies_o[]   = {2, 6, 0};
static const uint8_t  objset_crit_e_c_id_meas_fail_ind_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::ignore};

// E-CIDMeasurementFailureIndication-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e_c_id_meas_fail_ind_ies_container_values[] = {
    {offsetof(e_c_id_meas_fail_ind_ies_container, lmf_ue_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{1},
     uint64_t{14},
     nullptr},
    {offsetof(e_c_id_meas_fail_ind_ies_container, ran_ue_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{1},
     uint64_t{14},
     nullptr},
    {offsetof(e_c_id_meas_fail_ind_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const int32_t ie_table_e_c_id_meas_fail_ind_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                                 asn1::table::seq_field::mandatory,
                                                                                 asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc e_c_id_meas_fail_ind_ies_container::desc = {
    objset_id_e_c_id_meas_fail_ind_ies_o,
    objset_crit_e_c_id_meas_fail_ind_ies_o,
    ie_table_e_c_id_meas_fail_ind_ies_container_values,
    ie_table_e_c_id_meas_fail_ind_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e_c_id_meas_initiation_fail_ies_o[]  = {2, 0, 1};
static const uint8_t objset_crit_e_c_id_meas_initiation_fail_ies_o[] = {crit_e::reject, crit_e::ignore, crit_e::ignore};

// E-CIDMeasurementInitiationFailure-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e_c_id_meas_initiation_fail_ies_container_values[] = {
    {offsetof(e_c_id_meas_initiation_fail_ies_container, lmf_ue_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{1},
     uint64_t{14},
     nullptr},
    {offsetof(e_c_id_meas_initiation_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(e_c_id_meas_initiation_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(e_c_id_meas_initiation_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_e_c_id_meas_initiation_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e_c_id_meas_initiation_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc e_c_id_meas_initiation_fail_ies_container::desc = {
    objset_id_e_c_id_meas_initiation_fail_ies_o,
    objset_crit_e_c_id_meas_initiation_fail_ies_o,
    ie_table_e_c_id_meas_initiation_fail_ies_container_values,
    ie_table_e_c_id_meas_initiation_fail_ies_container_presences,
    3,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e_c_id_meas_initiation_request_ies_o[] = {2, 3, 4, 5, 15, 19, 105};
static const uint8_t  objset_crit_e_c_id_meas_initiation_request_ies_o[] =
    {crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::reject};

// E-CIDMeasurementInitiationRequest-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e_c_id_meas_initiation_request_ies_container_values[] = {
    {offsetof(e_c_id_meas_initiation_request_ies_container, lmf_ue_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{1},
     uint64_t{14},
     nullptr},
    {offsetof(e_c_id_meas_initiation_request_ies_container, report_characteristics),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(e_c_id_meas_initiation_request_ies_container::report_characteristics)>(),
     decltype(e_c_id_meas_initiation_request_ies_container::report_characteristics)::has_ext,
     false,
     decltype(e_c_id_meas_initiation_request_ies_container::report_characteristics)::nof_types,
     decltype(e_c_id_meas_initiation_request_ies_container::report_characteristics)::nof_exts,
     nullptr,
     &report_characteristics_opts::desc},
    {offsetof(e_c_id_meas_initiation_request_ies_container, meas_periodicity),
     (int32_t)offsetof(e_c_id_meas_initiation_request_ies_container, meas_periodicity_present),
     asn1::table::enum_op<decltype(e_c_id_meas_initiation_request_ies_container::meas_periodicity)>(),
     decltype(e_c_id_meas_initiation_request_ies_container::meas_periodicity)::has_ext,
     false,
     decltype(e_c_id_meas_initiation_request_ies_container::meas_periodicity)::nof_types,
     decltype(e_c_id_meas_initiation_request_ies_container::meas_periodicity)::nof_exts,
     nullptr,
     &meas_periodicity_opts::desc},
    {offsetof(e_c_id_meas_initiation_request_ies_container, meas_quantities),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::list_codec<decltype(e_c_id_meas_initiation_request_ies_container::meas_quantities),
                              asn1::table::struct_json>::value},
    {offsetof(e_c_id_meas_initiation_request_ies_container, other_rat_meas_quantities),
     (int32_t)offsetof(e_c_id_meas_initiation_request_ies_container, other_rat_meas_quantities_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     64,
     &asn1::table::list_codec<decltype(e_c_id_meas_initiation_request_ies_container::other_rat_meas_quantities),
                              asn1::table::struct_json>::value},
    {offsetof(e_c_id_meas_initiation_request_ies_container, wlan_meas_quantities),
     (int32_t)offsetof(e_c_id_meas_initiation_request_ies_container, wlan_meas_quantities_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     64,
     &asn1::table::list_codec<decltype(e_c_id_meas_initiation_request_ies_container::wlan_meas_quantities),
                              asn1::table::struct_json>::value},
    {offsetof(e_c_id_meas_initiation_request_ies_container, meas_periodicity_nr_ao_a),
     (int32_t)offsetof(e_c_id_meas_initiation_request_ies_container, meas_periodicity_nr_ao_a_present),
     asn1::table::enum_op<decltype(e_c_id_meas_initiation_request_ies_container::meas_periodicity_nr_ao_a)>(),
     decltype(e_c_id_meas_initiation_request_ies_container::meas_periodicity_nr_ao_a)::has_ext,
     false,
     decltype(e_c_id_meas_initiation_request_ies_container::meas_periodicity_nr_ao_a)::nof_types,
     decltype(e_c_id_meas_initiation_request_ies_container::meas_periodicity_nr_ao_a)::nof_exts,
     nullptr,
     &meas_periodicity_nr_ao_a_opts::desc}};
static const int32_t ie_table_e_c_id_meas_initiation_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e_c_id_meas_initiation_request_ies_container, meas_periodicity_present),
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e_c_id_meas_initiation_request_ies_container, other_rat_meas_quantities_present),
    (int32_t)offsetof(e_c_id_meas_initiation_request_ies_container, wlan_meas_quantities_present),
    (int32_t)offsetof(e_c_id_meas_initiation_request_ies_container, meas_periodicity_nr_ao_a_present)};
const asn1::table::ie_container_desc e_c_id_meas_initiation_request_ies_container::desc = {
    objset_id_e_c_id_meas_initiation_request_ies_o,
    objset_crit_e_c_id_meas_initiation_request_ies_o,
    ie_table_e_c_id_meas_initiation_request_ies_container_values,
    ie_table_e_c_id_meas_initiation_request_ies_container_presences,
    7,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e_c_id_meas_initiation_resp_ies_o[] = {2, 6, 7, 1, 14, 17, 21};
static const uint8_t  objset_crit_e_c_id_meas_initiation_resp_ies_o[] =
    {crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// E-CIDMeasurementInitiationResponse-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e_c_id_meas_initiation_resp_ies_container_values[] = {
    {offsetof(e_c_id_meas_initiation_resp_ies_container, lmf_ue_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{1},
     uint64_t{14},
     nullptr},
    {offsetof(e_c_id_meas_initiation_resp_ies_container, ran_ue_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{1},
     uint64_t{14},
     nullptr},
    {offsetof(e_c_id_meas_initiation_resp_ies_container, e_c_id_meas_result),
     (int32_t)offsetof(e_c_id_meas_initiation_resp_ies_container, e_c_id_meas_result_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e_c_id_meas_result_s::desc},
    {offsetof(e_c_id_meas_initiation_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(e_c_id_meas_initiation_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc},
    {offsetof(e_c_id_meas_initiation_resp_ies_container, cell_portion_id),
     (int32_t)offsetof(e_c_id_meas_initiation_resp_ies_container, cell_portion_id_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(e_c_id_meas_initiation_resp_ies_container, other_rat_meas_result),
     (int32_t)offsetof(e_c_id_meas_initiation_resp_ies_container, other_rat_meas_result_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(e_c_id_meas_initiation_resp_ies_container::other_rat_meas_result),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &other_rat_measured_results_value_c::desc},
    {offsetof(e_c_id_meas_initiation_resp_ies_container, wlan_meas_result),
     (int32_t)offsetof(e_c_id_meas_initiation_resp_ies_container, wlan_meas_result_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(e_c_id_meas_initiation_resp_ies_container::wlan_meas_result),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &wlan_meas_result_item_s::desc}};
static const int32_t ie_table_e_c_id_meas_initiation_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e_c_id_meas_initiation_resp_ies_container, e_c_id_meas_result_present),
    (int32_t)offsetof(e_c_id_meas_initiation_resp_ies_container, crit_diagnostics_present),
    (int32_t)offsetof(e_c_id_meas_initiation_resp_ies_container, cell_portion_id_present),
    (int32_t)offsetof(e_c_id_meas_initiation_resp_ies_container, other_rat_meas_result_present),
    (int32_t)offsetof(e_c_id_meas_initiation_resp_ies_container, wlan_meas_result_present)};
const asn1::table::ie_container_desc e_c_id_meas_initiation_resp_ies_container::desc = {
    objset_id_e_c_id_meas_initiation_resp_ies_o,
    objset_crit_e_c_id_meas_initiation_resp_ies_o,
    ie_table_e_c_id_meas_initiation_resp_ies_container_values,
    ie_table_e_c_id_meas_initiation_resp_ies_container_presences,
    7,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e_c_id_meas_report_ies_o[]   = {2, 6, 7, 14};
static const uint8_t  objset_crit_e_c_id_meas_report_ies_o[] = {crit_e::reject,
                                                                crit_e::reject,
                                                                crit_e::ignore,
                                                                crit_e::ignore};

// E-CIDMeasurementReport-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e_c_id_meas_report_ies_container_values[] = {
    {offsetof(e_c_id_meas_report_ies_container, lmf_ue_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{1},
     uint64_t{14},
     nullptr},
    {offsetof(e_c_id_meas_report_ies_container, ran_ue_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{1},
     uint64_t{14},
     nullptr},
    {offsetof(e_c_id_meas_report_ies_container, e_c_id_meas_result),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e_c_id_meas_result_s::desc},
    {offsetof(e_c_id_meas_report_ies_container, cell_portion_id),
     (int32_t)offsetof(e_c_id_meas_report_ies_container, cell_portion_id_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr}};
static const int32_t ie_table_e_c_id_meas_report_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e_c_id_meas_report_ies_container, cell_portion_id_present)};
const asn1::table::ie_container_desc e_c_id_meas_report_ies_container::desc = {
    objset_id_e_c_id_meas_report_ies_o,
    objset_crit_e_c_id_meas_report_ies_o,
    ie_table_e_c_id_meas_report_ies_container_values,
    ie_table_e_c_id_meas_report_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e_c_id_meas_termination_cmd_ies_o[]   = {2, 6};
static const uint8_t  objset_crit_e_c_id_meas_termination_cmd_ies_o[] = {crit_e::reject, crit_e::reject};

// E-CIDMeasurementTerminationCommand-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e_c_id_meas_termination_cmd_ies_container_values[] = {
    {offsetof(e_c_id_meas_termination_cmd_ies_container, lmf_ue_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{1},
     uint64_t{14},
     nullptr},
    {offsetof(e_c_id_meas_termination_cmd_ies_container, ran_ue_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{1},
     uint64_t{14},
     nullptr}};
static const int32_t ie_table_e_c_id_meas_termination_cmd_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc e_c_id_meas_termination_cmd_ies_container::desc = {
    objset_id_e_c_id_meas_termination_cmd_ies_o,
    objset_crit_e_c_id_meas_termination_cmd_ies_o,
    ie_table_e_c_id_meas_termination_cmd_ies_container_values,
    ie_table_e_c_id_meas_termination_cmd_ies_container_presences,
    2,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_error_ind_ies_o[]   = {0, 1};
static const uint8_t  objset_crit_error_ind_ies_o[] = {crit_e::ignore, crit_e::ignore};

// ErrorIndication-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_error_ind_ies_container_values[] = {
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
    (int32_t)offsetof(error_ind_ies_container, cause_present),
    (int32_t)offsetof(error_ind_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc error_ind_ies_container::desc = {objset_id_error_ind_ies_o,
                                                                      objset_crit_error_ind_ies_o,
                                                                      ie_table_error_ind_ies_container_values,
                                                                      ie_table_error_ind_ies_container_presences,
                                                                      2,
                                                                      0,
                                                                      0,
                                                                      65535,
                                                                      true,
                                                                      "value"};

static const uint32_t objset_id_meas_abort_ies_o[]   = {39, 40};
static const uint8_t  objset_crit_meas_abort_ies_o[] = {crit_e::reject, crit_e::reject};

// MeasurementAbort-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_meas_abort_ies_container_values[] = {
    {offsetof(meas_abort_ies_container, lmf_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65535},
     nullptr},
    {offsetof(meas_abort_ies_container, ran_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65535},
     nullptr}};
static const int32_t                 ie_table_meas_abort_ies_container_presences[] = {asn1::table::seq_field::mandatory,
                                                                                      asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc meas_abort_ies_container::desc                = {objset_id_meas_abort_ies_o,
                                                                                      objset_crit_meas_abort_ies_o,
                                                                                      ie_table_meas_abort_ies_container_values,
                                                                                      ie_table_meas_abort_ies_container_presences,
                                                                                      2,
                                                                                      2,
                                                                                      0,
                                                                                      65535,
                                                                                      true,
                                                                                      "value"};

static const uint32_t objset_id_meas_activation_ies_o[]   = {98, 88};
static const uint8_t  objset_crit_meas_activation_ies_o[] = {crit_e::reject, crit_e::ignore};

// MeasurementActivation-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_meas_activation_ies_container_values[] = {
    {offsetof(meas_activation_ies_container, request_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(meas_activation_ies_container::request_type)>(),
     decltype(meas_activation_ies_container::request_type)::has_ext,
     false,
     decltype(meas_activation_ies_container::request_type)::nof_types,
     decltype(meas_activation_ies_container::request_type)::nof_exts,
     nullptr,
     &request_type_opts::desc},
    {offsetof(meas_activation_ies_container, prs_meass_info_list),
     (int32_t)offsetof(meas_activation_ies_container, prs_meass_info_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     3,
     &asn1::table::item_list_codec<decltype(meas_activation_ies_container::prs_meass_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &prs_meass_info_list_item_s::desc}};
static const int32_t ie_table_meas_activation_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(meas_activation_ies_container, prs_meass_info_list_present)};
const asn1::table::ie_container_desc meas_activation_ies_container::desc = {
    objset_id_meas_activation_ies_o,
    objset_crit_meas_activation_ies_o,
    ie_table_meas_activation_ies_container_values,
    ie_table_meas_activation_ies_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_meas_fail_ies_o[]   = {39, 0, 1};
static const uint8_t  objset_crit_meas_fail_ies_o[] = {crit_e::reject, crit_e::ignore, crit_e::ignore};

// MeasurementFailure-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_meas_fail_ies_container_values[] = {
    {offsetof(meas_fail_ies_container, lmf_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65535},
     nullptr},
    {offsetof(meas_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(meas_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(meas_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_meas_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(meas_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc meas_fail_ies_container::desc = {objset_id_meas_fail_ies_o,
                                                                      objset_crit_meas_fail_ies_o,
                                                                      ie_table_meas_fail_ies_container_values,
                                                                      ie_table_meas_fail_ies_container_presences,
                                                                      3,
                                                                      2,
                                                                      0,
                                                                      65535,
                                                                      true,
                                                                      "value"};

static const uint32_t objset_id_meas_fail_ind_ies_o[]   = {39, 40, 0};
static const uint8_t  objset_crit_meas_fail_ind_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::ignore};

// MeasurementFailureIndication-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_meas_fail_ind_ies_container_values[] = {
    {offsetof(meas_fail_ind_ies_container, lmf_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65535},
     nullptr},
    {offsetof(meas_fail_ind_ies_container, ran_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65535},
     nullptr},
    {offsetof(meas_fail_ind_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const int32_t ie_table_meas_fail_ind_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                          asn1::table::seq_field::mandatory,
                                                                          asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc meas_fail_ind_ies_container::desc = {
    objset_id_meas_fail_ind_ies_o,
    objset_crit_meas_fail_ind_ies_o,
    ie_table_meas_fail_ind_ies_container_values,
    ie_table_meas_fail_ind_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_meas_precfg_confirm_ies_o[]   = {97, 1};
static const uint8_t  objset_crit_meas_precfg_confirm_ies_o[] = {crit_e::ignore, crit_e::ignore};

// MeasurementPreconfigurationConfirm-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_meas_precfg_confirm_ies_container_values[] = {
    {offsetof(meas_precfg_confirm_ies_container, precfg_result),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(meas_precfg_confirm_ies_container::precfg_result),
                                asn1::table::struct_json>::value},
    {offsetof(meas_precfg_confirm_ies_container, crit_diagnostics),
     (int32_t)offsetof(meas_precfg_confirm_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_meas_precfg_confirm_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(meas_precfg_confirm_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc meas_precfg_confirm_ies_container::desc = {
    objset_id_meas_precfg_confirm_ies_o,
    objset_crit_meas_precfg_confirm_ies_o,
    ie_table_meas_precfg_confirm_ies_container_values,
    ie_table_meas_precfg_confirm_ies_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_meas_precfg_refuse_ies_o[]   = {0, 1};
static const uint8_t  objset_crit_meas_precfg_refuse_ies_o[] = {crit_e::ignore, crit_e::ignore};

// MeasurementPreconfigurationRefuse-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_meas_precfg_refuse_ies_container_values[] = {
    {offsetof(meas_precfg_refuse_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(meas_precfg_refuse_ies_container, crit_diagnostics),
     (int32_t)offsetof(meas_precfg_refuse_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_meas_precfg_refuse_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(meas_precfg_refuse_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc meas_precfg_refuse_ies_container::desc = {
    objset_id_meas_precfg_refuse_ies_o,
    objset_crit_meas_precfg_refuse_ies_o,
    ie_table_meas_precfg_refuse_ies_container_values,
    ie_table_meas_precfg_refuse_ies_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_meas_precfg_required_ies_o[]   = {87};
static const uint8_t  objset_crit_meas_precfg_required_ies_o[] = {crit_e::ignore};

// MeasurementPreconfigurationRequired-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_meas_precfg_required_ies_container_values[] = {
    {offsetof(meas_precfg_required_ies_container, trp_prs_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<decltype(meas_precfg_required_ies_container::trp_prs_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &trp_prs_info_list_item_s::desc}};
static const int32_t ie_table_meas_precfg_required_ies_container_presences[]  = {asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc meas_precfg_required_ies_container::desc = {
    objset_id_meas_precfg_required_ies_o,
    objset_crit_meas_precfg_required_ies_o,
    ie_table_meas_precfg_required_ies_container_values,
    ie_table_meas_precfg_required_ies_container_presences,
    1,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_meas_report_ies_o[]   = {39, 40, 43};
static const uint8_t  objset_crit_meas_report_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::reject};

// MeasurementReport-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_meas_report_ies_container_values[] = {
    {offsetof(meas_report_ies_container, lmf_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65535},
     nullptr},
    {offsetof(meas_report_ies_container, ran_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65535},
     nullptr},
    {offsetof(meas_report_ies_container, trp_meas_report_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(meas_report_ies_container::trp_meas_report_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &trp_meas_resp_item_s::desc}};
static const int32_t ie_table_meas_report_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                        asn1::table::seq_field::mandatory,
                                                                        asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc meas_report_ies_container::desc = {objset_id_meas_report_ies_o,
                                                                        objset_crit_meas_report_ies_o,
                                                                        ie_table_meas_report_ies_container_values,
                                                                        ie_table_meas_report_ies_container_presences,
                                                                        3,
                                                                        3,
                                                                        0,
                                                                        65535,
                                                                        true,
                                                                        "value"};

static const uint32_t objset_id_meas_request_ies_o[] = {39, 41, 3, 4, 52, 54, 26, 31, 49, 50, 64, 72, 92, 91, 95, 125};
static const uint8_t  objset_crit_meas_request_ies_o[] = {crit_e::reject,
                                                          crit_e::reject,
                                                          crit_e::reject,
                                                          crit_e::reject,
                                                          crit_e::reject,
                                                          crit_e::ignore,
                                                          crit_e::ignore,
                                                          crit_e::ignore,
                                                          crit_e::ignore,
                                                          crit_e::ignore,
                                                          crit_e::reject,
                                                          crit_e::ignore,
                                                          crit_e::ignore,
                                                          crit_e::ignore,
                                                          crit_e::ignore,
                                                          crit_e::ignore};

// MeasurementRequest-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_meas_request_ies_container_values[] = {
    {offsetof(meas_request_ies_container, lmf_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65535},
     nullptr},
    {offsetof(meas_request_ies_container, trp_meas_request_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(meas_request_ies_container::trp_meas_request_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &trp_meas_request_item_s::desc},
    {offsetof(meas_request_ies_container, report_characteristics),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(meas_request_ies_container::report_characteristics)>(),
     decltype(meas_request_ies_container::report_characteristics)::has_ext,
     false,
     decltype(meas_request_ies_container::report_characteristics)::nof_types,
     decltype(meas_request_ies_container::report_characteristics)::nof_exts,
     nullptr,
     &report_characteristics_opts::desc},
    {offsetof(meas_request_ies_container, meas_periodicity),
     (int32_t)offsetof(meas_request_ies_container, meas_periodicity_present),
     asn1::table::enum_op<decltype(meas_request_ies_container::meas_periodicity)>(),
     decltype(meas_request_ies_container::meas_periodicity)::has_ext,
     false,
     decltype(meas_request_ies_container::meas_periodicity)::nof_types,
     decltype(meas_request_ies_container::meas_periodicity)::nof_exts,
     nullptr,
     &meas_periodicity_opts::desc},
    {offsetof(meas_request_ies_container, trp_meas_quantities),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16383,
     &asn1::table::item_list_codec<decltype(meas_request_ies_container::trp_meas_quantities),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &trp_meas_quantities_list_item_s::desc},
    {offsetof(meas_request_ies_container, sfn_initisation_time),
     (int32_t)offsetof(meas_request_ies_container, sfn_initisation_time_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(meas_request_ies_container::sfn_initisation_time),
                                asn1::table::struct_json>::value},
    {offsetof(meas_request_ies_container, srs_cfg),
     (int32_t)offsetof(meas_request_ies_container, srs_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_configuration_s::desc},
    {offsetof(meas_request_ies_container, meas_beam_info_request),
     (int32_t)offsetof(meas_request_ies_container, meas_beam_info_request_present),
     asn1::table::enum_op<decltype(meas_request_ies_container::meas_beam_info_request)>(),
     decltype(meas_request_ies_container::meas_beam_info_request)::has_ext,
     false,
     decltype(meas_request_ies_container::meas_beam_info_request)::nof_types,
     decltype(meas_request_ies_container::meas_beam_info_request)::nof_exts,
     nullptr,
     &meas_beam_info_request_opts::desc},
    {offsetof(meas_request_ies_container, sys_frame_num),
     (int32_t)offsetof(meas_request_ies_container, sys_frame_num_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{1023},
     nullptr},
    {offsetof(meas_request_ies_container, slot_num),
     (int32_t)offsetof(meas_request_ies_container, slot_num_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     int64_t{0},
     uint64_t{79},
     nullptr},
    {offsetof(meas_request_ies_container, meas_periodicity_extended),
     (int32_t)offsetof(meas_request_ies_container, meas_periodicity_extended_present),
     asn1::table::enum_op<decltype(meas_request_ies_container::meas_periodicity_extended)>(),
     decltype(meas_request_ies_container::meas_periodicity_extended)::has_ext,
     false,
     decltype(meas_request_ies_container::meas_periodicity_extended)::nof_types,
     decltype(meas_request_ies_container::meas_periodicity_extended)::nof_exts,
     nullptr,
     &meas_periodicity_extended_opts::desc},
    {offsetof(meas_request_ies_container, resp_time),
     (int32_t)offsetof(meas_request_ies_container, resp_time_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &resp_time_s::desc},
    {offsetof(meas_request_ies_container, meas_characteristics_request_ind),
     (int32_t)offsetof(meas_request_ies_container, meas_characteristics_request_ind_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(meas_request_ies_container::meas_characteristics_request_ind),
                                asn1::table::struct_json>::value},
    {offsetof(meas_request_ies_container, meas_time_occasion),
     (int32_t)offsetof(meas_request_ies_container, meas_time_occasion_present),
     asn1::table::enum_op<decltype(meas_request_ies_container::meas_time_occasion)>(),
     decltype(meas_request_ies_container::meas_time_occasion)::has_ext,
     false,
     decltype(meas_request_ies_container::meas_time_occasion)::nof_types,
     decltype(meas_request_ies_container::meas_time_occasion)::nof_exts,
     nullptr,
     &meas_time_occasion_opts::desc},
    {offsetof(meas_request_ies_container, meas_amount),
     (int32_t)offsetof(meas_request_ies_container, meas_amount_present),
     asn1::table::enum_op<decltype(meas_request_ies_container::meas_amount)>(),
     decltype(meas_request_ies_container::meas_amount)::has_ext,
     false,
     decltype(meas_request_ies_container::meas_amount)::nof_types,
     decltype(meas_request_ies_container::meas_amount)::nof_exts,
     nullptr,
     &meas_amount_opts::desc},
    {offsetof(meas_request_ies_container, time_win_info_meas_list),
     (int32_t)offsetof(meas_request_ies_container, time_win_info_meas_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     15,
     &asn1::table::item_list_codec<decltype(meas_request_ies_container::time_win_info_meas_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &time_win_info_meas_item_s::desc}};
static const int32_t ie_table_meas_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(meas_request_ies_container, meas_periodicity_present),
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(meas_request_ies_container, sfn_initisation_time_present),
    (int32_t)offsetof(meas_request_ies_container, srs_cfg_present),
    (int32_t)offsetof(meas_request_ies_container, meas_beam_info_request_present),
    (int32_t)offsetof(meas_request_ies_container, sys_frame_num_present),
    (int32_t)offsetof(meas_request_ies_container, slot_num_present),
    (int32_t)offsetof(meas_request_ies_container, meas_periodicity_extended_present),
    (int32_t)offsetof(meas_request_ies_container, resp_time_present),
    (int32_t)offsetof(meas_request_ies_container, meas_characteristics_request_ind_present),
    (int32_t)offsetof(meas_request_ies_container, meas_time_occasion_present),
    (int32_t)offsetof(meas_request_ies_container, meas_amount_present),
    (int32_t)offsetof(meas_request_ies_container, time_win_info_meas_list_present)};
const asn1::table::ie_container_desc meas_request_ies_container::desc = {objset_id_meas_request_ies_o,
                                                                         objset_crit_meas_request_ies_o,
                                                                         ie_table_meas_request_ies_container_values,
                                                                         ie_table_meas_request_ies_container_presences,
                                                                         16,
                                                                         4,
                                                                         0,
                                                                         65535,
                                                                         true,
                                                                         "value"};

static const uint32_t objset_id_meas_resp_ies_o[]   = {39, 40, 42, 1};
static const uint8_t  objset_crit_meas_resp_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::reject, crit_e::ignore};

// MeasurementResponse-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_meas_resp_ies_container_values[] = {
    {offsetof(meas_resp_ies_container, lmf_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65535},
     nullptr},
    {offsetof(meas_resp_ies_container, ran_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65535},
     nullptr},
    {offsetof(meas_resp_ies_container, trp_meas_resp_list),
     (int32_t)offsetof(meas_resp_ies_container, trp_meas_resp_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(meas_resp_ies_container::trp_meas_resp_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &trp_meas_resp_item_s::desc},
    {offsetof(meas_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(meas_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_meas_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(meas_resp_ies_container, trp_meas_resp_list_present),
    (int32_t)offsetof(meas_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc meas_resp_ies_container::desc = {objset_id_meas_resp_ies_o,
                                                                      objset_crit_meas_resp_ies_o,
                                                                      ie_table_meas_resp_ies_container_values,
                                                                      ie_table_meas_resp_ies_container_presences,
                                                                      4,
                                                                      2,
                                                                      0,
                                                                      65535,
                                                                      true,
                                                                      "value"};

static const uint32_t objset_id_meas_upd_ies_o[] = {39, 40, 26, 70, 92, 91};
static const uint8_t  objset_crit_meas_upd_ies_o[] =
    {crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::reject, crit_e::ignore, crit_e::ignore};

// MeasurementUpdate-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_meas_upd_ies_container_values[] = {
    {offsetof(meas_upd_ies_container, lmf_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65535},
     nullptr},
    {offsetof(meas_upd_ies_container, ran_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65535},
     nullptr},
    {offsetof(meas_upd_ies_container, srs_cfg),
     (int32_t)offsetof(meas_upd_ies_container, srs_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_configuration_s::desc},
    {offsetof(meas_upd_ies_container, trp_meas_upd_list),
     (int32_t)offsetof(meas_upd_ies_container, trp_meas_upd_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(meas_upd_ies_container::trp_meas_upd_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &trp_meas_upd_item_s::desc},
    {offsetof(meas_upd_ies_container, meas_characteristics_request_ind),
     (int32_t)offsetof(meas_upd_ies_container, meas_characteristics_request_ind_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(meas_upd_ies_container::meas_characteristics_request_ind),
                                asn1::table::struct_json>::value},
    {offsetof(meas_upd_ies_container, meas_time_occasion),
     (int32_t)offsetof(meas_upd_ies_container, meas_time_occasion_present),
     asn1::table::enum_op<decltype(meas_upd_ies_container::meas_time_occasion)>(),
     decltype(meas_upd_ies_container::meas_time_occasion)::has_ext,
     false,
     decltype(meas_upd_ies_container::meas_time_occasion)::nof_types,
     decltype(meas_upd_ies_container::meas_time_occasion)::nof_exts,
     nullptr,
     &meas_time_occasion_opts::desc}};
static const int32_t ie_table_meas_upd_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(meas_upd_ies_container, srs_cfg_present),
    (int32_t)offsetof(meas_upd_ies_container, trp_meas_upd_list_present),
    (int32_t)offsetof(meas_upd_ies_container, meas_characteristics_request_ind_present),
    (int32_t)offsetof(meas_upd_ies_container, meas_time_occasion_present)};
const asn1::table::ie_container_desc meas_upd_ies_container::desc = {objset_id_meas_upd_ies_o,
                                                                     objset_crit_meas_upd_ies_o,
                                                                     ie_table_meas_upd_ies_container_values,
                                                                     ie_table_meas_upd_ies_container_presences,
                                                                     6,
                                                                     2,
                                                                     0,
                                                                     65535,
                                                                     true,
                                                                     "value"};

// OTDOA-Information-Type-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_otdoa_info_type_item_s_fields[] = {
    {offsetof(otdoa_info_type_item_s, otdoa_info_item),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(otdoa_info_type_item_s::otdoa_info_item)>(),
     decltype(otdoa_info_type_item_s::otdoa_info_item)::has_ext,
     false,
     decltype(otdoa_info_type_item_s::otdoa_info_item)::nof_types,
     decltype(otdoa_info_type_item_s::otdoa_info_item)::nof_exts,
     nullptr,
     &otdoa_info_item_opts::desc},
    {offsetof(otdoa_info_type_item_s, ie_exts),
     (int32_t)offsetof(otdoa_info_type_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(otdoa_info_type_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_otdoa_info_type_item_s_names[] = {{"oTDOA-Information-Item", nullptr},
                                                                                 {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_otdoa_info_type_item_s_opts[]  = {
    {1, (int32_t)offsetof(otdoa_info_type_item_s, ie_exts_present)}};
const asn1::table::seq_desc otdoa_info_type_item_s::desc = {{seq_table_otdoa_info_type_item_s_fields,
                                                             uint64_t{0x1},
                                                             seq_table_otdoa_info_type_item_s_opts,
                                                             seq_table_otdoa_info_type_item_s_names,
                                                             2,
                                                             1},
                                                            true,
                                                            true,
                                                            nullptr,
                                                            0};

// OTDOA-Information-Type-ItemIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const uint32_t objset_id_otdoa_info_type_item_ies_o[]    = {10};
static const uint8_t  objset_crit_otdoa_info_type_item_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_otdoa_info_type_item_ies_o[] = {
    otdoa_info_type_item_ies_o::value_c::types::otdoa_info_type_item};
static const uint8_t                    objset_presence_otdoa_info_type_item_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_otdoa_info_type_item_ies_o[]  = {
    {objset_crit_otdoa_info_type_item_ies_o, crit_e::nulltype},
    {objset_value_otdoa_info_type_item_ies_o, otdoa_info_type_item_ies_o::value_c::types::nulltype},
    {objset_presence_otdoa_info_type_item_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc otdoa_info_type_item_ies_o::desc = {objset_id_otdoa_info_type_item_ies_o,
                                                                   1,
                                                                   "id",
                                                                   objset_columns_otdoa_info_type_item_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_otdoa_info_type_item_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &otdoa_info_type_item_s::desc}};
static const asn1::table::field_name choice_table_otdoa_info_type_item_ies_o_value_c_names[] = {
    {"OTDOA-Information-Type-Item", nullptr}};
const asn1::table::choice_desc otdoa_info_type_item_ies_o::value_c::desc = {
    asn1::table::enum_int_type<otdoa_info_type_item_ies_o::value_c::types>(),
    otdoa_info_type_item_ies_o::value_c::types::nof_types,
    otdoa_info_type_item_ies_o::value_c::types::nof_exts,
    otdoa_info_type_item_ies_o::value_c::types::has_ext,
    true,
    choice_table_otdoa_info_type_item_ies_o_value_c_alternatives,
    nullptr,
    choice_table_otdoa_info_type_item_ies_o_value_c_names,
    "otdoa_info_type_item_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(otdoa_info_type_item_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_otdoa_info_type_item_ies_o_value_c_types_values[]    = {"OTDOA-Information-Type-Item"};
const asn1::table::enum_desc otdoa_info_type_item_ies_o::value_c::types_opts::desc = {
    enum_names_otdoa_info_type_item_ies_o_value_c_types_values,
    1,
    "otdoa_info_type_item_ies_o::value_c::types"};

static const uint32_t objset_id_otdoa_info_fail_ies_o[]   = {0, 1};
static const uint8_t  objset_crit_otdoa_info_fail_ies_o[] = {crit_e::ignore, crit_e::ignore};

// OTDOAInformationFailure-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_otdoa_info_fail_ies_container_values[] = {
    {offsetof(otdoa_info_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(otdoa_info_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(otdoa_info_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_otdoa_info_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(otdoa_info_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc otdoa_info_fail_ies_container::desc = {
    objset_id_otdoa_info_fail_ies_o,
    objset_crit_otdoa_info_fail_ies_o,
    ie_table_otdoa_info_fail_ies_container_values,
    ie_table_otdoa_info_fail_ies_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_otdoa_info_request_ies_o[]   = {9};
static const uint8_t  objset_crit_otdoa_info_request_ies_o[] = {crit_e::reject};

// OTDOAInformationRequest-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_otdoa_info_request_ies_container_values[] = {
    {offsetof(otdoa_info_request_ies_container, otdoa_info_type_group),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::list_codec<decltype(otdoa_info_request_ies_container::otdoa_info_type_group),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_otdoa_info_request_ies_container_presences[]  = {asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc otdoa_info_request_ies_container::desc = {
    objset_id_otdoa_info_request_ies_o,
    objset_crit_otdoa_info_request_ies_o,
    ie_table_otdoa_info_request_ies_container_values,
    ie_table_otdoa_info_request_ies_container_presences,
    1,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_otdoa_info_resp_ies_o[]   = {8, 1};
static const uint8_t  objset_crit_otdoa_info_resp_ies_o[] = {crit_e::ignore, crit_e::ignore};

// OTDOAInformationResponse-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_otdoa_info_resp_ies_container_values[] = {
    {offsetof(otdoa_info_resp_ies_container, otdoa_cells),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     3839,
     &asn1::table::item_list_codec<decltype(otdoa_info_resp_ies_container::otdoa_cells),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &otdoa_cells_item_s_::desc},
    {offsetof(otdoa_info_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(otdoa_info_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_otdoa_info_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(otdoa_info_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc otdoa_info_resp_ies_container::desc = {
    objset_id_otdoa_info_resp_ies_o,
    objset_crit_otdoa_info_resp_ies_o,
    ie_table_otdoa_info_resp_ies_container_values,
    ie_table_otdoa_info_resp_ies_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_prs_cfg_fail_ies_o[]   = {0, 1};
static const uint8_t  objset_crit_prs_cfg_fail_ies_o[] = {crit_e::ignore, crit_e::ignore};

// PRSConfigurationFailure-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_prs_cfg_fail_ies_container_values[] = {
    {offsetof(prs_cfg_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(prs_cfg_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(prs_cfg_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_prs_cfg_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(prs_cfg_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc prs_cfg_fail_ies_container::desc = {objset_id_prs_cfg_fail_ies_o,
                                                                         objset_crit_prs_cfg_fail_ies_o,
                                                                         ie_table_prs_cfg_fail_ies_container_values,
                                                                         ie_table_prs_cfg_fail_ies_container_presences,
                                                                         2,
                                                                         1,
                                                                         0,
                                                                         65535,
                                                                         true,
                                                                         "value"};

static const uint32_t objset_id_prs_cfg_request_ies_o[]   = {89, 66};
static const uint8_t  objset_crit_prs_cfg_request_ies_o[] = {crit_e::reject, crit_e::ignore};

// PRSConfigurationRequest-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_prs_cfg_request_ies_container_values[] = {
    {offsetof(prs_cfg_request_ies_container, prs_cfg_request_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(prs_cfg_request_ies_container::prs_cfg_request_type)>(),
     decltype(prs_cfg_request_ies_container::prs_cfg_request_type)::has_ext,
     false,
     decltype(prs_cfg_request_ies_container::prs_cfg_request_type)::nof_types,
     decltype(prs_cfg_request_ies_container::prs_cfg_request_type)::nof_exts,
     nullptr,
     &prs_cfg_request_type_opts::desc},
    {offsetof(prs_cfg_request_ies_container, prstrp_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(prs_cfg_request_ies_container::prstrp_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &prstrp_item_s::desc}};
static const int32_t ie_table_prs_cfg_request_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                            asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc prs_cfg_request_ies_container::desc = {
    objset_id_prs_cfg_request_ies_o,
    objset_crit_prs_cfg_request_ies_o,
    ie_table_prs_cfg_request_ies_container_values,
    ie_table_prs_cfg_request_ies_container_presences,
    2,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_prs_cfg_resp_ies_o[]   = {67, 1};
static const uint8_t  objset_crit_prs_cfg_resp_ies_o[] = {crit_e::ignore, crit_e::ignore};

// PRSConfigurationResponse-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_prs_cfg_resp_ies_container_values[] = {
    {offsetof(prs_cfg_resp_ies_container, prs_tx_trp_list),
     (int32_t)offsetof(prs_cfg_resp_ies_container, prs_tx_trp_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(prs_cfg_resp_ies_container::prs_tx_trp_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &prs_tx_trp_item_s::desc},
    {offsetof(prs_cfg_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(prs_cfg_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_prs_cfg_resp_ies_container_presences[] = {
    (int32_t)offsetof(prs_cfg_resp_ies_container, prs_tx_trp_list_present),
    (int32_t)offsetof(prs_cfg_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc prs_cfg_resp_ies_container::desc = {objset_id_prs_cfg_resp_ies_o,
                                                                         objset_crit_prs_cfg_resp_ies_o,
                                                                         ie_table_prs_cfg_resp_ies_container_values,
                                                                         ie_table_prs_cfg_resp_ies_container_presences,
                                                                         2,
                                                                         0,
                                                                         0,
                                                                         65535,
                                                                         true,
                                                                         "value"};

static const uint32_t objset_id_positioning_activation_fail_ies_o[]   = {0, 1};
static const uint8_t  objset_crit_positioning_activation_fail_ies_o[] = {crit_e::ignore, crit_e::ignore};

// PositioningActivationFailureIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_positioning_activation_fail_ies_container_values[] = {
    {offsetof(positioning_activation_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(positioning_activation_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(positioning_activation_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_positioning_activation_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(positioning_activation_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc positioning_activation_fail_ies_container::desc = {
    objset_id_positioning_activation_fail_ies_o,
    objset_crit_positioning_activation_fail_ies_o,
    ie_table_positioning_activation_fail_ies_container_values,
    ie_table_positioning_activation_fail_ies_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_semipersistent_srs_ext_ies_o[]   = {48, 63, 122};
static const uint8_t  objset_crit_semipersistent_srs_ext_ies_o[] = {crit_e::ignore, crit_e::ignore, crit_e::ignore};

// SemipersistentSRS-ExtIEs ::= OBJECT SET OF NRPPA-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_semipersistent_srs_ext_ies_container_values[] = {
    {offsetof(semipersistent_srs_ext_ies_container, srs_spatial_relation),
     (int32_t)offsetof(semipersistent_srs_ext_ies_container, srs_spatial_relation_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &spatial_relation_info_s::desc},
    {offsetof(semipersistent_srs_ext_ies_container, srs_spatial_relation_per_srs_res),
     (int32_t)offsetof(semipersistent_srs_ext_ies_container, srs_spatial_relation_per_srs_res_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &spatial_relation_per_srs_res_s::desc},
    {offsetof(semipersistent_srs_ext_ies_container, pos_srs_res_set_aggregation_list),
     (int32_t)offsetof(semipersistent_srs_ext_ies_container, pos_srs_res_set_aggregation_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(semipersistent_srs_ext_ies_container::pos_srs_res_set_aggregation_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pos_srs_res_set_aggregation_item_s::desc}};
static const int32_t ie_table_semipersistent_srs_ext_ies_container_presences[] = {
    (int32_t)offsetof(semipersistent_srs_ext_ies_container, srs_spatial_relation_present),
    (int32_t)offsetof(semipersistent_srs_ext_ies_container, srs_spatial_relation_per_srs_res_present),
    (int32_t)offsetof(semipersistent_srs_ext_ies_container, pos_srs_res_set_aggregation_list_present)};
const asn1::table::ie_container_desc semipersistent_srs_ext_ies_container::desc = {
    objset_id_semipersistent_srs_ext_ies_o,
    objset_crit_semipersistent_srs_ext_ies_o,
    ie_table_semipersistent_srs_ext_ies_container_values,
    ie_table_semipersistent_srs_ext_ies_container_presences,
    3,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// SemipersistentSRS ::= SEQUENCE
static const asn1::table::seq_field seq_table_semipersistent_srs_s_fields[] = {
    {offsetof(semipersistent_srs_s, srs_res_set_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{0},
     uint64_t{15},
     nullptr},
    {offsetof(semipersistent_srs_s, ie_exts),
     (int32_t)offsetof(semipersistent_srs_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &semipersistent_srs_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_semipersistent_srs_s_names[] = {{"sRSResourceSetID", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_semipersistent_srs_s_opts[]  = {
    {1, (int32_t)offsetof(semipersistent_srs_s, ie_exts_present)}};
const asn1::table::seq_desc semipersistent_srs_s::desc = {{seq_table_semipersistent_srs_s_fields,
                                                           uint64_t{0x1},
                                                           seq_table_semipersistent_srs_s_opts,
                                                           seq_table_semipersistent_srs_s_names,
                                                           2,
                                                           1},
                                                          true,
                                                          true,
                                                          nullptr,
                                                          0};

// SRSType ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_srs_type_c_alternatives[] = {
    &asn1::table::alternative_ops<semipersistent_srs_s, false>::value,
    &asn1::table::alternative_ops<aperiodic_srs_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<srs_type_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc srs_type_c::storage_desc = {choice_storage_srs_type_c_alternatives,
                                                                   3,
                                                                   "srs_type_c"};

semipersistent_srs_s& srs_type_c::set_semipersistent_srs()
{
  set(types::semipersistent_srs);
  return c.get<semipersistent_srs_s>();
}
aperiodic_srs_s& srs_type_c::set_aperiodic_srs()
{
  set(types::aperiodic_srs);
  return c.get<aperiodic_srs_s>();
}
protocol_ie_single_container_s<srs_type_ext_ies_o>& srs_type_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<srs_type_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_srs_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &semipersistent_srs_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &aperiodic_srs_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<srs_type_ext_ies_o>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_srs_type_c_names[] = {{"semipersistentSRS", nullptr},
                                                                        {"aperiodicSRS", nullptr},
                                                                        {"choice-Extension", nullptr}};
const asn1::table::choice_desc       srs_type_c::desc                = {asn1::table::enum_int_type<srs_type_c::types>(),
                                                                        srs_type_c::types::nof_types,
                                                                        srs_type_c::types::nof_exts,
                                                                        srs_type_c::types::has_ext,
                                                                        true,
                                                                        choice_table_srs_type_c_alternatives,
                                                                        nullptr,
                                                                        choice_table_srs_type_c_names,
                                                                        "srs_type_c",
                                                                        offsetof(srs_type_c, c.type),
                                                                        offsetof(srs_type_c, c.value),
                                                                        false,
                                                                        &srs_type_c::storage_desc};

static const char* enum_names_srs_type_c_types_values[]   = {"semipersistentSRS", "aperiodicSRS", "choice-Extension"};
const asn1::table::enum_desc srs_type_c::types_opts::desc = {enum_names_srs_type_c_types_values,
                                                             3,
                                                             "srs_type_c::types"};

static const uint32_t objset_id_positioning_activation_request_ies_o[]   = {44, 45};
static const uint8_t  objset_crit_positioning_activation_request_ies_o[] = {crit_e::reject, crit_e::ignore};

// PositioningActivationRequestIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_positioning_activation_request_ies_container_values[] = {
    {offsetof(positioning_activation_request_ies_container, srs_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_type_c::desc},
    {offsetof(positioning_activation_request_ies_container, activation_time),
     (int32_t)offsetof(positioning_activation_request_ies_container, activation_time_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(positioning_activation_request_ies_container::activation_time),
                                asn1::table::struct_json>::value}};
static const int32_t ie_table_positioning_activation_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(positioning_activation_request_ies_container, activation_time_present)};
const asn1::table::ie_container_desc positioning_activation_request_ies_container::desc = {
    objset_id_positioning_activation_request_ies_o,
    objset_crit_positioning_activation_request_ies_o,
    ie_table_positioning_activation_request_ies_container_values,
    ie_table_positioning_activation_request_ies_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_positioning_activation_resp_ies_o[]  = {1, 49, 50};
static const uint8_t objset_crit_positioning_activation_resp_ies_o[] = {crit_e::ignore, crit_e::ignore, crit_e::ignore};

// PositioningActivationResponseIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_positioning_activation_resp_ies_container_values[] = {
    {offsetof(positioning_activation_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(positioning_activation_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc},
    {offsetof(positioning_activation_resp_ies_container, sys_frame_num),
     (int32_t)offsetof(positioning_activation_resp_ies_container, sys_frame_num_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{1023},
     nullptr},
    {offsetof(positioning_activation_resp_ies_container, slot_num),
     (int32_t)offsetof(positioning_activation_resp_ies_container, slot_num_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     int64_t{0},
     uint64_t{79},
     nullptr}};
static const int32_t ie_table_positioning_activation_resp_ies_container_presences[] = {
    (int32_t)offsetof(positioning_activation_resp_ies_container, crit_diagnostics_present),
    (int32_t)offsetof(positioning_activation_resp_ies_container, sys_frame_num_present),
    (int32_t)offsetof(positioning_activation_resp_ies_container, slot_num_present)};
const asn1::table::ie_container_desc positioning_activation_resp_ies_container::desc = {
    objset_id_positioning_activation_resp_ies_o,
    objset_crit_positioning_activation_resp_ies_o,
    ie_table_positioning_activation_resp_ies_container_values,
    ie_table_positioning_activation_resp_ies_container_presences,
    3,
    0,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_positioning_deactivation_ies_o[]   = {53};
static const uint8_t  objset_crit_positioning_deactivation_ies_o[] = {crit_e::ignore};

// PositioningDeactivationIEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_positioning_deactivation_ies_container_values[] = {
    {offsetof(positioning_deactivation_ies_container, abort_tx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &abort_tx_c::desc}};
static const int32_t ie_table_positioning_deactivation_ies_container_presences[]  = {asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc positioning_deactivation_ies_container::desc = {
    objset_id_positioning_deactivation_ies_o,
    objset_crit_positioning_deactivation_ies_o,
    ie_table_positioning_deactivation_ies_container_values,
    ie_table_positioning_deactivation_ies_container_presences,
    1,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_positioning_info_fail_ies_o[]   = {0, 1};
static const uint8_t  objset_crit_positioning_info_fail_ies_o[] = {crit_e::ignore, crit_e::ignore};

// PositioningInformationFailure-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_positioning_info_fail_ies_container_values[] = {
    {offsetof(positioning_info_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(positioning_info_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(positioning_info_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_positioning_info_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(positioning_info_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc positioning_info_fail_ies_container::desc = {
    objset_id_positioning_info_fail_ies_o,
    objset_crit_positioning_info_fail_ies_o,
    ie_table_positioning_info_fail_ies_container_values,
    ie_table_positioning_info_fail_ies_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_positioning_info_request_ies_o[] = {12, 73, 90, 99, 124, 136, 159};
static const uint8_t  objset_crit_positioning_info_request_ies_o[] =
    {crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// PositioningInformationRequest-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_positioning_info_request_ies_container_values[] = {
    {offsetof(positioning_info_request_ies_container, requested_srs_tx_characteristics),
     (int32_t)offsetof(positioning_info_request_ies_container, requested_srs_tx_characteristics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &requested_srs_tx_characteristics_s::desc},
    {offsetof(positioning_info_request_ies_container, ue_report_info),
     (int32_t)offsetof(positioning_info_request_ies_container, ue_report_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_report_info_s::desc},
    {offsetof(positioning_info_request_ies_container, ue_teg_info_request),
     (int32_t)offsetof(positioning_info_request_ies_container, ue_teg_info_request_present),
     asn1::table::enum_op<decltype(positioning_info_request_ies_container::ue_teg_info_request)>(),
     decltype(positioning_info_request_ies_container::ue_teg_info_request)::has_ext,
     false,
     decltype(positioning_info_request_ies_container::ue_teg_info_request)::nof_types,
     decltype(positioning_info_request_ies_container::ue_teg_info_request)::nof_exts,
     nullptr,
     &ue_teg_info_request_opts::desc},
    {offsetof(positioning_info_request_ies_container, ue_teg_report_periodicity),
     (int32_t)offsetof(positioning_info_request_ies_container, ue_teg_report_periodicity_present),
     asn1::table::enum_op<decltype(positioning_info_request_ies_container::ue_teg_report_periodicity)>(),
     decltype(positioning_info_request_ies_container::ue_teg_report_periodicity)::has_ext,
     false,
     decltype(positioning_info_request_ies_container::ue_teg_report_periodicity)::nof_types,
     decltype(positioning_info_request_ies_container::ue_teg_report_periodicity)::nof_exts,
     nullptr,
     &ue_teg_report_periodicity_opts::desc},
    {offsetof(positioning_info_request_ies_container, time_win_info_srs_list),
     (int32_t)offsetof(positioning_info_request_ies_container, time_win_info_srs_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     15,
     &asn1::table::item_list_codec<decltype(positioning_info_request_ies_container::time_win_info_srs_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &time_win_info_srs_item_s::desc},
    {offsetof(positioning_info_request_ies_container, requested_srs_precfg_characteristics_list),
     (int32_t)offsetof(positioning_info_request_ies_container, requested_srs_precfg_characteristics_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     15,
     &asn1::table::item_list_codec<
         decltype(positioning_info_request_ies_container::requested_srs_precfg_characteristics_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &requested_srs_precfg_characteristics_item_s::desc},
    {offsetof(positioning_info_request_ies_container, remote_ue_ind_request),
     (int32_t)offsetof(positioning_info_request_ies_container, remote_ue_ind_request_present),
     asn1::table::enum_op<decltype(positioning_info_request_ies_container::remote_ue_ind_request)>(),
     decltype(positioning_info_request_ies_container::remote_ue_ind_request)::has_ext,
     false,
     decltype(positioning_info_request_ies_container::remote_ue_ind_request)::nof_types,
     decltype(positioning_info_request_ies_container::remote_ue_ind_request)::nof_exts,
     nullptr,
     &remote_ue_ind_request_opts::desc}};
static const int32_t ie_table_positioning_info_request_ies_container_presences[] = {
    (int32_t)offsetof(positioning_info_request_ies_container, requested_srs_tx_characteristics_present),
    (int32_t)offsetof(positioning_info_request_ies_container, ue_report_info_present),
    (int32_t)offsetof(positioning_info_request_ies_container, ue_teg_info_request_present),
    (int32_t)offsetof(positioning_info_request_ies_container, ue_teg_report_periodicity_present),
    (int32_t)offsetof(positioning_info_request_ies_container, time_win_info_srs_list_present),
    (int32_t)offsetof(positioning_info_request_ies_container, requested_srs_precfg_characteristics_list_present),
    (int32_t)offsetof(positioning_info_request_ies_container, remote_ue_ind_request_present)};
const asn1::table::ie_container_desc positioning_info_request_ies_container::desc = {
    objset_id_positioning_info_request_ies_o,
    objset_crit_positioning_info_request_ies_o,
    ie_table_positioning_info_request_ies_container_values,
    ie_table_positioning_info_request_ies_container_presences,
    7,
    0,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_positioning_info_resp_ies_o[]   = {26, 54, 1, 81, 113, 128, 137, 160};
static const uint8_t  objset_crit_positioning_info_resp_ies_o[] = {crit_e::ignore,
                                                                   crit_e::ignore,
                                                                   crit_e::ignore,
                                                                   crit_e::ignore,
                                                                   crit_e::ignore,
                                                                   crit_e::ignore,
                                                                   crit_e::ignore,
                                                                   crit_e::ignore};

// PositioningInformationResponse-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_positioning_info_resp_ies_container_values[] = {
    {offsetof(positioning_info_resp_ies_container, srs_cfg),
     (int32_t)offsetof(positioning_info_resp_ies_container, srs_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_configuration_s::desc},
    {offsetof(positioning_info_resp_ies_container, sfn_initisation_time),
     (int32_t)offsetof(positioning_info_resp_ies_container, sfn_initisation_time_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(positioning_info_resp_ies_container::sfn_initisation_time),
                                asn1::table::struct_json>::value},
    {offsetof(positioning_info_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(positioning_info_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc},
    {offsetof(positioning_info_resp_ies_container, ue_tx_teg_assoc_list),
     (int32_t)offsetof(positioning_info_resp_ies_container, ue_tx_teg_assoc_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<decltype(positioning_info_resp_ies_container::ue_tx_teg_assoc_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ue_tx_teg_assoc_item_s::desc},
    {offsetof(positioning_info_resp_ies_container, new_nr_cgi),
     (int32_t)offsetof(positioning_info_resp_ies_container, new_nr_cgi_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_nr_s::desc},
    {offsetof(positioning_info_resp_ies_container, pos_validity_area_cell_list),
     (int32_t)offsetof(positioning_info_resp_ies_container, pos_validity_area_cell_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(positioning_info_resp_ies_container::pos_validity_area_cell_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pos_validity_area_cell_item_s::desc},
    {offsetof(positioning_info_resp_ies_container, srs_precfg_list),
     (int32_t)offsetof(positioning_info_resp_ies_container, srs_precfg_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     15,
     &asn1::table::item_list_codec<decltype(positioning_info_resp_ies_container::srs_precfg_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &srs_precfg_item_s::desc},
    {offsetof(positioning_info_resp_ies_container, remote_ue_ind),
     (int32_t)offsetof(positioning_info_resp_ies_container, remote_ue_ind_present),
     asn1::table::enum_op<decltype(positioning_info_resp_ies_container::remote_ue_ind)>(),
     decltype(positioning_info_resp_ies_container::remote_ue_ind)::has_ext,
     false,
     decltype(positioning_info_resp_ies_container::remote_ue_ind)::nof_types,
     decltype(positioning_info_resp_ies_container::remote_ue_ind)::nof_exts,
     nullptr,
     &remote_ue_ind_opts::desc}};
static const int32_t ie_table_positioning_info_resp_ies_container_presences[] = {
    (int32_t)offsetof(positioning_info_resp_ies_container, srs_cfg_present),
    (int32_t)offsetof(positioning_info_resp_ies_container, sfn_initisation_time_present),
    (int32_t)offsetof(positioning_info_resp_ies_container, crit_diagnostics_present),
    (int32_t)offsetof(positioning_info_resp_ies_container, ue_tx_teg_assoc_list_present),
    (int32_t)offsetof(positioning_info_resp_ies_container, new_nr_cgi_present),
    (int32_t)offsetof(positioning_info_resp_ies_container, pos_validity_area_cell_list_present),
    (int32_t)offsetof(positioning_info_resp_ies_container, srs_precfg_list_present),
    (int32_t)offsetof(positioning_info_resp_ies_container, remote_ue_ind_present)};
const asn1::table::ie_container_desc positioning_info_resp_ies_container::desc = {
    objset_id_positioning_info_resp_ies_o,
    objset_crit_positioning_info_resp_ies_o,
    ie_table_positioning_info_resp_ies_container_values,
    ie_table_positioning_info_resp_ies_container_presences,
    8,
    0,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_positioning_info_upd_ies_o[] = {26, 54, 81, 106, 134, 161};
static const uint8_t  objset_crit_positioning_info_upd_ies_o[] =
    {crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// PositioningInformationUpdate-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_positioning_info_upd_ies_container_values[] = {
    {offsetof(positioning_info_upd_ies_container, srs_cfg),
     (int32_t)offsetof(positioning_info_upd_ies_container, srs_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_configuration_s::desc},
    {offsetof(positioning_info_upd_ies_container, sfn_initisation_time),
     (int32_t)offsetof(positioning_info_upd_ies_container, sfn_initisation_time_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(positioning_info_upd_ies_container::sfn_initisation_time),
                                asn1::table::struct_json>::value},
    {offsetof(positioning_info_upd_ies_container, ue_tx_teg_assoc_list),
     (int32_t)offsetof(positioning_info_upd_ies_container, ue_tx_teg_assoc_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<decltype(positioning_info_upd_ies_container::ue_tx_teg_assoc_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ue_tx_teg_assoc_item_s::desc},
    {offsetof(positioning_info_upd_ies_container, srs_tx_status),
     (int32_t)offsetof(positioning_info_upd_ies_container, srs_tx_status_present),
     asn1::table::enum_op<decltype(positioning_info_upd_ies_container::srs_tx_status)>(),
     decltype(positioning_info_upd_ies_container::srs_tx_status)::has_ext,
     false,
     decltype(positioning_info_upd_ies_container::srs_tx_status)::nof_types,
     decltype(positioning_info_upd_ies_container::srs_tx_status)::nof_exts,
     nullptr,
     &srs_tx_status_opts::desc},
    {offsetof(positioning_info_upd_ies_container, new_cell_id),
     (int32_t)offsetof(positioning_info_upd_ies_container, new_cell_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_nr_s::desc},
    {offsetof(positioning_info_upd_ies_container, remote_ue_status),
     (int32_t)offsetof(positioning_info_upd_ies_container, remote_ue_status_present),
     asn1::table::enum_op<decltype(positioning_info_upd_ies_container::remote_ue_status)>(),
     decltype(positioning_info_upd_ies_container::remote_ue_status)::has_ext,
     false,
     decltype(positioning_info_upd_ies_container::remote_ue_status)::nof_types,
     decltype(positioning_info_upd_ies_container::remote_ue_status)::nof_exts,
     nullptr,
     &remote_ue_status_opts::desc}};
static const int32_t ie_table_positioning_info_upd_ies_container_presences[] = {
    (int32_t)offsetof(positioning_info_upd_ies_container, srs_cfg_present),
    (int32_t)offsetof(positioning_info_upd_ies_container, sfn_initisation_time_present),
    (int32_t)offsetof(positioning_info_upd_ies_container, ue_tx_teg_assoc_list_present),
    (int32_t)offsetof(positioning_info_upd_ies_container, srs_tx_status_present),
    (int32_t)offsetof(positioning_info_upd_ies_container, new_cell_id_present),
    (int32_t)offsetof(positioning_info_upd_ies_container, remote_ue_status_present)};
const asn1::table::ie_container_desc positioning_info_upd_ies_container::desc = {
    objset_id_positioning_info_upd_ies_o,
    objset_crit_positioning_info_upd_ies_o,
    ie_table_positioning_info_upd_ies_container_values,
    ie_table_positioning_info_upd_ies_container_presences,
    6,
    0,
    0,
    65535,
    true,
    "value"};

// Value ::= OPEN TYPE
static const asn1::table::field_name choice_table_nr_ppa_private_ies_empty_o_value_c_names[] = {};
const asn1::table::choice_desc       nr_ppa_private_ies_empty_o::value_c::desc               = {
    asn1::table::enum_int_type<nr_ppa_private_ies_empty_o::value_c::types>(),
    nr_ppa_private_ies_empty_o::value_c::types::nof_types,
    nr_ppa_private_ies_empty_o::value_c::types::nof_exts,
    nr_ppa_private_ies_empty_o::value_c::types::has_ext,
    true,
    nullptr,
    nullptr,
    choice_table_nr_ppa_private_ies_empty_o_value_c_names,
    "nr_ppa_private_ies_empty_o::value_c",
    asn1::table::choice_desc::no_member,
    asn1::table::choice_desc::no_member,
    false,
    nullptr,
    true};

static const char*           enum_names_nr_ppa_private_ies_empty_o_value_c_types_values[] = {};
const asn1::table::enum_desc nr_ppa_private_ies_empty_o::value_c::types_opts::desc        = {
    enum_names_nr_ppa_private_ies_empty_o_value_c_types_values,
    0,
    "nr_ppa_private_ies_empty_o::value_c::types"};

// PrivateMessage ::= SEQUENCE
static const asn1::table::seq_field seq_table_private_msg_s_fields[] = {
    {offsetof(private_msg_s, private_ies),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(private_msg_s::private_ies), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_private_msg_s_names[] = {{"privateIEs", nullptr}};
const asn1::table::seq_desc          private_msg_s::desc             = {
    {seq_table_private_msg_s_fields, uint64_t{0x1}, nullptr, seq_table_private_msg_s_names, 1, 0},
    true,
    true,
    nullptr,
    0,
    "PrivateMessage"};

static const uint32_t objset_id_srs_info_reserv_notif_ies_o[]   = {129, 138, 163};
static const uint8_t  objset_crit_srs_info_reserv_notif_ies_o[] = {crit_e::reject, crit_e::ignore, crit_e::ignore};

// SRSInformationReservationNotification-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_srs_info_reserv_notif_ies_container_values[] = {
    {offsetof(srs_info_reserv_notif_ies_container, srs_reserv_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_info_reserv_notif_ies_container::srs_reserv_type)>(),
     decltype(srs_info_reserv_notif_ies_container::srs_reserv_type)::has_ext,
     false,
     decltype(srs_info_reserv_notif_ies_container::srs_reserv_type)::nof_types,
     decltype(srs_info_reserv_notif_ies_container::srs_reserv_type)::nof_exts,
     nullptr,
     &srs_reserv_type_opts::desc},
    {offsetof(srs_info_reserv_notif_ies_container, srs_info),
     (int32_t)offsetof(srs_info_reserv_notif_ies_container, srs_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &requested_srs_tx_characteristics_s::desc},
    {offsetof(srs_info_reserv_notif_ies_container, precfg_srs_info),
     (int32_t)offsetof(srs_info_reserv_notif_ies_container, precfg_srs_info_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     15,
     &asn1::table::item_list_codec<decltype(srs_info_reserv_notif_ies_container::precfg_srs_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &requested_srs_precfg_characteristics_item_s::desc}};
static const int32_t ie_table_srs_info_reserv_notif_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(srs_info_reserv_notif_ies_container, srs_info_present),
    (int32_t)offsetof(srs_info_reserv_notif_ies_container, precfg_srs_info_present)};
const asn1::table::ie_container_desc srs_info_reserv_notif_ies_container::desc = {
    objset_id_srs_info_reserv_notif_ies_o,
    objset_crit_srs_info_reserv_notif_ies_o,
    ie_table_srs_info_reserv_notif_ies_container_values,
    ie_table_srs_info_reserv_notif_ies_container_presences,
    3,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_trp_info_fail_ies_o[]   = {0, 1};
static const uint8_t  objset_crit_trp_info_fail_ies_o[] = {crit_e::ignore, crit_e::ignore};

// TRPInformationFailure-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_trp_info_fail_ies_container_values[] = {
    {offsetof(trp_info_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(trp_info_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(trp_info_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_trp_info_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(trp_info_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc trp_info_fail_ies_container::desc = {
    objset_id_trp_info_fail_ies_o,
    objset_crit_trp_info_fail_ies_o,
    ie_table_trp_info_fail_ies_container_values,
    ie_table_trp_info_fail_ies_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_trp_info_request_ies_o[]   = {47, 29};
static const uint8_t  objset_crit_trp_info_request_ies_o[] = {crit_e::ignore, crit_e::reject};

// TRPInformationRequest-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_trp_info_request_ies_container_values[] = {
    {offsetof(trp_info_request_ies_container, trp_list),
     (int32_t)offsetof(trp_info_request_ies_container, trp_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(trp_info_request_ies_container::trp_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &trp_item_s::desc},
    {offsetof(trp_info_request_ies_container, trp_info_type_list_trp_req),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::list_codec<decltype(trp_info_request_ies_container::trp_info_type_list_trp_req),
                              asn1::table::struct_json>::value}};
static const int32_t ie_table_trp_info_request_ies_container_presences[] = {
    (int32_t)offsetof(trp_info_request_ies_container, trp_list_present),
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc trp_info_request_ies_container::desc = {
    objset_id_trp_info_request_ies_o,
    objset_crit_trp_info_request_ies_o,
    ie_table_trp_info_request_ies_container_values,
    ie_table_trp_info_request_ies_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_trp_info_resp_ies_o[]   = {30, 1};
static const uint8_t  objset_crit_trp_info_resp_ies_o[] = {crit_e::ignore, crit_e::ignore};

// TRPInformationResponse-IEs ::= OBJECT SET OF NRPPA-PROTOCOL-IES
static const asn1::table::seq_field ie_table_trp_info_resp_ies_container_values[] = {
    {offsetof(trp_info_resp_ies_container, trp_info_list_trp_resp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(trp_info_resp_ies_container::trp_info_list_trp_resp),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &trp_info_list_trp_resp_item_s_::desc},
    {offsetof(trp_info_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(trp_info_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_trp_info_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(trp_info_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc trp_info_resp_ies_container::desc = {
    objset_id_trp_info_resp_ies_o,
    objset_crit_trp_info_resp_ies_o,
    ie_table_trp_info_resp_ies_container_values,
    ie_table_trp_info_resp_ies_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};
