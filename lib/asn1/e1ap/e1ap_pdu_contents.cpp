// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/e1ap/e1ap_pdu_contents.h"
#include "ocudu/asn1/asn1_ap_codec.h"

using namespace asn1;
using namespace asn1::e1ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

static const uint32_t objset_id_bc_bearer_context_mod_confirm_ies_o[]   = {155, 156, 163, 1};
static const uint8_t  objset_crit_bc_bearer_context_mod_confirm_ies_o[] = {crit_e::reject,
                                                                           crit_e::reject,
                                                                           crit_e::reject,
                                                                           crit_e::ignore};

// BCBearerContextModificationConfirmIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bc_bearer_context_mod_confirm_ies_container_values[] = {
    {offsetof(bc_bearer_context_mod_confirm_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(bc_bearer_context_mod_confirm_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(bc_bearer_context_mod_confirm_ies_container, bc_bearer_context_to_modify_confirm),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_to_modify_confirm_s::desc},
    {offsetof(bc_bearer_context_mod_confirm_ies_container, crit_diagnostics),
     (int32_t)offsetof(bc_bearer_context_mod_confirm_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_bc_bearer_context_mod_confirm_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bc_bearer_context_mod_confirm_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc bc_bearer_context_mod_confirm_ies_container::desc = {
    objset_id_bc_bearer_context_mod_confirm_ies_o,
    objset_crit_bc_bearer_context_mod_confirm_ies_o,
    ie_table_bc_bearer_context_mod_confirm_ies_container_values,
    ie_table_bc_bearer_context_mod_confirm_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bc_bearer_context_mod_fail_ies_o[]   = {155, 156, 0, 1};
static const uint8_t  objset_crit_bc_bearer_context_mod_fail_ies_o[] = {crit_e::reject,
                                                                        crit_e::reject,
                                                                        crit_e::ignore,
                                                                        crit_e::ignore};

// BCBearerContextModificationFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bc_bearer_context_mod_fail_ies_container_values[] = {
    {offsetof(bc_bearer_context_mod_fail_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(bc_bearer_context_mod_fail_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(bc_bearer_context_mod_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(bc_bearer_context_mod_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(bc_bearer_context_mod_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_bc_bearer_context_mod_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bc_bearer_context_mod_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc bc_bearer_context_mod_fail_ies_container::desc = {
    objset_id_bc_bearer_context_mod_fail_ies_o,
    objset_crit_bc_bearer_context_mod_fail_ies_o,
    ie_table_bc_bearer_context_mod_fail_ies_container_values,
    ie_table_bc_bearer_context_mod_fail_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bc_bearer_context_mod_request_ies_o[]   = {155, 156, 160};
static const uint8_t  objset_crit_bc_bearer_context_mod_request_ies_o[] = {crit_e::reject,
                                                                           crit_e::reject,
                                                                           crit_e::reject};

// BCBearerContextModificationRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bc_bearer_context_mod_request_ies_container_values[] = {
    {offsetof(bc_bearer_context_mod_request_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(bc_bearer_context_mod_request_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(bc_bearer_context_mod_request_ies_container, bc_bearer_context_to_modify),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_to_modify_s::desc}};
static const int32_t ie_table_bc_bearer_context_mod_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc bc_bearer_context_mod_request_ies_container::desc = {
    objset_id_bc_bearer_context_mod_request_ies_o,
    objset_crit_bc_bearer_context_mod_request_ies_o,
    ie_table_bc_bearer_context_mod_request_ies_container_values,
    ie_table_bc_bearer_context_mod_request_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bc_bearer_context_mod_required_ies_o[]   = {155, 156, 162};
static const uint8_t  objset_crit_bc_bearer_context_mod_required_ies_o[] = {crit_e::reject,
                                                                            crit_e::reject,
                                                                            crit_e::reject};

// BCBearerContextModificationRequiredIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bc_bearer_context_mod_required_ies_container_values[] = {
    {offsetof(bc_bearer_context_mod_required_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(bc_bearer_context_mod_required_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(bc_bearer_context_mod_required_ies_container, bc_bearer_context_to_modify_required),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_to_modify_required_s::desc}};
static const int32_t ie_table_bc_bearer_context_mod_required_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc bc_bearer_context_mod_required_ies_container::desc = {
    objset_id_bc_bearer_context_mod_required_ies_o,
    objset_crit_bc_bearer_context_mod_required_ies_o,
    ie_table_bc_bearer_context_mod_required_ies_container_values,
    ie_table_bc_bearer_context_mod_required_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bc_bearer_context_mod_resp_ies_o[]   = {155, 156, 161, 1};
static const uint8_t  objset_crit_bc_bearer_context_mod_resp_ies_o[] = {crit_e::reject,
                                                                        crit_e::reject,
                                                                        crit_e::reject,
                                                                        crit_e::ignore};

// BCBearerContextModificationResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bc_bearer_context_mod_resp_ies_container_values[] = {
    {offsetof(bc_bearer_context_mod_resp_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(bc_bearer_context_mod_resp_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(bc_bearer_context_mod_resp_ies_container, bc_bearer_context_to_modify_resp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_to_modify_resp_s::desc},
    {offsetof(bc_bearer_context_mod_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(bc_bearer_context_mod_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_bc_bearer_context_mod_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bc_bearer_context_mod_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc bc_bearer_context_mod_resp_ies_container::desc = {
    objset_id_bc_bearer_context_mod_resp_ies_o,
    objset_crit_bc_bearer_context_mod_resp_ies_o,
    ie_table_bc_bearer_context_mod_resp_ies_container_values,
    ie_table_bc_bearer_context_mod_resp_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bc_bearer_context_release_cmd_ies_o[]   = {155, 156, 0};
static const uint8_t  objset_crit_bc_bearer_context_release_cmd_ies_o[] = {crit_e::reject,
                                                                           crit_e::reject,
                                                                           crit_e::ignore};

// BCBearerContextReleaseCommandIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bc_bearer_context_release_cmd_ies_container_values[] = {
    {offsetof(bc_bearer_context_release_cmd_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(bc_bearer_context_release_cmd_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(bc_bearer_context_release_cmd_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const int32_t ie_table_bc_bearer_context_release_cmd_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc bc_bearer_context_release_cmd_ies_container::desc = {
    objset_id_bc_bearer_context_release_cmd_ies_o,
    objset_crit_bc_bearer_context_release_cmd_ies_o,
    ie_table_bc_bearer_context_release_cmd_ies_container_values,
    ie_table_bc_bearer_context_release_cmd_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bc_bearer_context_release_complete_ies_o[]   = {155, 156, 1};
static const uint8_t  objset_crit_bc_bearer_context_release_complete_ies_o[] = {crit_e::reject,
                                                                                crit_e::reject,
                                                                                crit_e::ignore};

// BCBearerContextReleaseCompleteIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bc_bearer_context_release_complete_ies_container_values[] = {
    {offsetof(bc_bearer_context_release_complete_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(bc_bearer_context_release_complete_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(bc_bearer_context_release_complete_ies_container, crit_diagnostics),
     (int32_t)offsetof(bc_bearer_context_release_complete_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_bc_bearer_context_release_complete_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bc_bearer_context_release_complete_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc bc_bearer_context_release_complete_ies_container::desc = {
    objset_id_bc_bearer_context_release_complete_ies_o,
    objset_crit_bc_bearer_context_release_complete_ies_o,
    ie_table_bc_bearer_context_release_complete_ies_container_values,
    ie_table_bc_bearer_context_release_complete_ies_container_presences,
    3,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bc_bearer_context_release_request_ies_o[]   = {155, 156, 0};
static const uint8_t  objset_crit_bc_bearer_context_release_request_ies_o[] = {crit_e::reject,
                                                                               crit_e::reject,
                                                                               crit_e::ignore};

// BCBearerContextReleaseRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bc_bearer_context_release_request_ies_container_values[] = {
    {offsetof(bc_bearer_context_release_request_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(bc_bearer_context_release_request_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(bc_bearer_context_release_request_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const int32_t ie_table_bc_bearer_context_release_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc bc_bearer_context_release_request_ies_container::desc = {
    objset_id_bc_bearer_context_release_request_ies_o,
    objset_crit_bc_bearer_context_release_request_ies_o,
    ie_table_bc_bearer_context_release_request_ies_container_values,
    ie_table_bc_bearer_context_release_request_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bc_bearer_context_setup_fail_ies_o[]   = {155, 156, 0, 1};
static const uint8_t  objset_crit_bc_bearer_context_setup_fail_ies_o[] = {crit_e::reject,
                                                                          crit_e::ignore,
                                                                          crit_e::ignore,
                                                                          crit_e::ignore};

// BCBearerContextSetupFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bc_bearer_context_setup_fail_ies_container_values[] = {
    {offsetof(bc_bearer_context_setup_fail_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(bc_bearer_context_setup_fail_ies_container, gnb_cu_up_mbs_e1ap_id),
     (int32_t)offsetof(bc_bearer_context_setup_fail_ies_container, gnb_cu_up_mbs_e1ap_id_present),
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(bc_bearer_context_setup_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(bc_bearer_context_setup_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(bc_bearer_context_setup_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_bc_bearer_context_setup_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bc_bearer_context_setup_fail_ies_container, gnb_cu_up_mbs_e1ap_id_present),
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bc_bearer_context_setup_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc bc_bearer_context_setup_fail_ies_container::desc = {
    objset_id_bc_bearer_context_setup_fail_ies_o,
    objset_crit_bc_bearer_context_setup_fail_ies_o,
    ie_table_bc_bearer_context_setup_fail_ies_container_values,
    ie_table_bc_bearer_context_setup_fail_ies_container_presences,
    4,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bc_bearer_context_setup_request_ies_o[]   = {155, 157, 158, 199, 200};
static const uint8_t  objset_crit_bc_bearer_context_setup_request_ies_o[] = {crit_e::reject,
                                                                             crit_e::reject,
                                                                             crit_e::reject,
                                                                             crit_e::ignore,
                                                                             crit_e::ignore};

// BCBearerContextSetupRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bc_bearer_context_setup_request_ies_container_values[] = {
    {offsetof(bc_bearer_context_setup_request_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(bc_bearer_context_setup_request_ies_container, global_mbs_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_mbs_session_id_s::desc},
    {offsetof(bc_bearer_context_setup_request_ies_container, bc_bearer_context_to_setup),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_to_setup_s::desc},
    {offsetof(bc_bearer_context_setup_request_ies_container, associated_session_id),
     (int32_t)offsetof(bc_bearer_context_setup_request_ies_container, associated_session_id_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(bc_bearer_context_setup_request_ies_container::associated_session_id),
                                asn1::table::string_json>::value},
    {offsetof(bc_bearer_context_setup_request_ies_container, mbs_service_area),
     (int32_t)offsetof(bc_bearer_context_setup_request_ies_container, mbs_service_area_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_service_area_s::desc}};
static const int32_t ie_table_bc_bearer_context_setup_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bc_bearer_context_setup_request_ies_container, associated_session_id_present),
    (int32_t)offsetof(bc_bearer_context_setup_request_ies_container, mbs_service_area_present)};
const asn1::table::ie_container_desc bc_bearer_context_setup_request_ies_container::desc = {
    objset_id_bc_bearer_context_setup_request_ies_o,
    objset_crit_bc_bearer_context_setup_request_ies_o,
    ie_table_bc_bearer_context_setup_request_ies_container_values,
    ie_table_bc_bearer_context_setup_request_ies_container_presences,
    5,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bc_bearer_context_setup_resp_ies_o[]   = {155, 156, 159, 1};
static const uint8_t  objset_crit_bc_bearer_context_setup_resp_ies_o[] = {crit_e::reject,
                                                                          crit_e::reject,
                                                                          crit_e::reject,
                                                                          crit_e::ignore};

// BCBearerContextSetupResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bc_bearer_context_setup_resp_ies_container_values[] = {
    {offsetof(bc_bearer_context_setup_resp_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(bc_bearer_context_setup_resp_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(bc_bearer_context_setup_resp_ies_container, bc_bearer_context_to_setup_resp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_to_setup_resp_s::desc},
    {offsetof(bc_bearer_context_setup_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(bc_bearer_context_setup_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_bc_bearer_context_setup_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bc_bearer_context_setup_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc bc_bearer_context_setup_resp_ies_container::desc = {
    objset_id_bc_bearer_context_setup_resp_ies_o,
    objset_crit_bc_bearer_context_setup_resp_ies_o,
    ie_table_bc_bearer_context_setup_resp_ies_container_values,
    ie_table_bc_bearer_context_setup_resp_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bearer_context_inactivity_notif_ies_o[]   = {2, 3, 24};
static const uint8_t  objset_crit_bearer_context_inactivity_notif_ies_o[] = {crit_e::reject,
                                                                             crit_e::reject,
                                                                             crit_e::reject};

// BearerContextInactivityNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bearer_context_inactivity_notif_ies_container_values[] = {
    {offsetof(bearer_context_inactivity_notif_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_inactivity_notif_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_inactivity_notif_ies_container, activity_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &activity_info_c::desc}};
static const int32_t ie_table_bearer_context_inactivity_notif_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc bearer_context_inactivity_notif_ies_container::desc = {
    objset_id_bearer_context_inactivity_notif_ies_o,
    objset_crit_bearer_context_inactivity_notif_ies_o,
    ie_table_bearer_context_inactivity_notif_ies_container_values,
    ie_table_bearer_context_inactivity_notif_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_eutran_bearer_context_mod_confirm_o[]   = {41};
static const uint8_t  objset_crit_eutran_bearer_context_mod_confirm_o[] = {crit_e::ignore};

// EUTRAN-BearerContextModificationConfirm ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_eutran_bearer_context_mod_confirm_container_values[] = {
    {offsetof(eutran_bearer_context_mod_confirm_container, drb_confirm_modified_list_eutran),
     (int32_t)offsetof(eutran_bearer_context_mod_confirm_container, drb_confirm_modified_list_eutran_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<
         decltype(eutran_bearer_context_mod_confirm_container::drb_confirm_modified_list_eutran),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &drb_confirm_modified_item_eutran_s::desc}};
static const int32_t ie_table_eutran_bearer_context_mod_confirm_container_presences[] = {
    (int32_t)offsetof(eutran_bearer_context_mod_confirm_container, drb_confirm_modified_list_eutran_present)};
const asn1::table::ie_container_desc eutran_bearer_context_mod_confirm_container::desc = {
    objset_id_eutran_bearer_context_mod_confirm_o,
    objset_crit_eutran_bearer_context_mod_confirm_o,
    ie_table_eutran_bearer_context_mod_confirm_container_values,
    ie_table_eutran_bearer_context_mod_confirm_container_presences,
    1,
    0,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ng_ran_bearer_context_mod_confirm_o[]   = {50};
static const uint8_t  objset_crit_ng_ran_bearer_context_mod_confirm_o[] = {crit_e::ignore};

// NG-RAN-BearerContextModificationConfirm ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ng_ran_bearer_context_mod_confirm_container_values[] = {
    {offsetof(ng_ran_bearer_context_mod_confirm_container, pdu_session_res_confirm_modified_list),
     (int32_t)offsetof(ng_ran_bearer_context_mod_confirm_container, pdu_session_res_confirm_modified_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<
         decltype(ng_ran_bearer_context_mod_confirm_container::pdu_session_res_confirm_modified_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &pdu_session_res_confirm_modified_item_s::desc}};
static const int32_t ie_table_ng_ran_bearer_context_mod_confirm_container_presences[] = {
    (int32_t)offsetof(ng_ran_bearer_context_mod_confirm_container, pdu_session_res_confirm_modified_list_present)};
const asn1::table::ie_container_desc ng_ran_bearer_context_mod_confirm_container::desc = {
    objset_id_ng_ran_bearer_context_mod_confirm_o,
    objset_crit_ng_ran_bearer_context_mod_confirm_o,
    ie_table_ng_ran_bearer_context_mod_confirm_container_values,
    ie_table_ng_ran_bearer_context_mod_confirm_container_presences,
    1,
    0,
    0,
    65535,
    true,
    "value"};

// System-BearerContextModificationConfirm ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_sys_bearer_context_mod_confirm_c_alternatives[] = {
    &asn1::table::alternative_ops<eutran_bearer_context_mod_confirm_container, false>::value,
    &asn1::table::alternative_ops<ng_ran_bearer_context_mod_confirm_container, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>,
                                  false>::value};
const asn1::table::choice_storage_desc sys_bearer_context_mod_confirm_c::storage_desc = {
    choice_storage_sys_bearer_context_mod_confirm_c_alternatives,
    3,
    "sys_bearer_context_mod_confirm_c"};

eutran_bearer_context_mod_confirm_container& sys_bearer_context_mod_confirm_c::set_e_utran_bearer_context_mod_confirm()
{
  set(types::e_utran_bearer_context_mod_confirm);
  return c.get<eutran_bearer_context_mod_confirm_container>();
}
ng_ran_bearer_context_mod_confirm_container& sys_bearer_context_mod_confirm_c::set_ng_ran_bearer_context_mod_confirm()
{
  set(types::ng_ran_bearer_context_mod_confirm);
  return c.get<ng_ran_bearer_context_mod_confirm_container>();
}
protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>&
sys_bearer_context_mod_confirm_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_sys_bearer_context_mod_confirm_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &eutran_bearer_context_mod_confirm_container::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &ng_ran_bearer_context_mod_confirm_container::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<sys_bearer_context_mod_confirm_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_sys_bearer_context_mod_confirm_c_names[] = {
    {"e-UTRAN-BearerContextModificationConfirm", nullptr},
    {"nG-RAN-BearerContextModificationConfirm", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc sys_bearer_context_mod_confirm_c::desc = {
    asn1::table::enum_int_type<sys_bearer_context_mod_confirm_c::types>(),
    sys_bearer_context_mod_confirm_c::types::nof_types,
    sys_bearer_context_mod_confirm_c::types::nof_exts,
    sys_bearer_context_mod_confirm_c::types::has_ext,
    true,
    choice_table_sys_bearer_context_mod_confirm_c_alternatives,
    nullptr,
    choice_table_sys_bearer_context_mod_confirm_c_names,
    "sys_bearer_context_mod_confirm_c",
    offsetof(sys_bearer_context_mod_confirm_c, c.type),
    offsetof(sys_bearer_context_mod_confirm_c, c.value),
    false,
    &sys_bearer_context_mod_confirm_c::storage_desc};

static const char* enum_names_sys_bearer_context_mod_confirm_c_types_values[] = {
    "e-UTRAN-BearerContextModificationConfirm",
    "nG-RAN-BearerContextModificationConfirm",
    "choice-extension"};
const asn1::table::enum_desc sys_bearer_context_mod_confirm_c::types_opts::desc = {
    enum_names_sys_bearer_context_mod_confirm_c_types_values,
    3,
    "sys_bearer_context_mod_confirm_c::types"};

static const uint32_t objset_id_bearer_context_mod_confirm_ies_o[]   = {2, 3, 20, 1};
static const uint8_t  objset_crit_bearer_context_mod_confirm_ies_o[] = {crit_e::reject,
                                                                        crit_e::reject,
                                                                        crit_e::ignore,
                                                                        crit_e::ignore};

// BearerContextModificationConfirmIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bearer_context_mod_confirm_ies_container_values[] = {
    {offsetof(bearer_context_mod_confirm_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_mod_confirm_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_mod_confirm_ies_container, sys_bearer_context_mod_confirm),
     (int32_t)offsetof(bearer_context_mod_confirm_ies_container, sys_bearer_context_mod_confirm_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sys_bearer_context_mod_confirm_c::desc},
    {offsetof(bearer_context_mod_confirm_ies_container, crit_diagnostics),
     (int32_t)offsetof(bearer_context_mod_confirm_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_bearer_context_mod_confirm_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bearer_context_mod_confirm_ies_container, sys_bearer_context_mod_confirm_present),
    (int32_t)offsetof(bearer_context_mod_confirm_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc bearer_context_mod_confirm_ies_container::desc = {
    objset_id_bearer_context_mod_confirm_ies_o,
    objset_crit_bearer_context_mod_confirm_ies_o,
    ie_table_bearer_context_mod_confirm_ies_container_values,
    ie_table_bearer_context_mod_confirm_ies_container_presences,
    4,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bearer_context_mod_fail_ies_o[]   = {2, 3, 0, 1};
static const uint8_t  objset_crit_bearer_context_mod_fail_ies_o[] = {crit_e::reject,
                                                                     crit_e::reject,
                                                                     crit_e::ignore,
                                                                     crit_e::ignore};

// BearerContextModificationFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bearer_context_mod_fail_ies_container_values[] = {
    {offsetof(bearer_context_mod_fail_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_mod_fail_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_mod_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(bearer_context_mod_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(bearer_context_mod_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_bearer_context_mod_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bearer_context_mod_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc bearer_context_mod_fail_ies_container::desc = {
    objset_id_bearer_context_mod_fail_ies_o,
    objset_crit_bearer_context_mod_fail_ies_o,
    ie_table_bearer_context_mod_fail_ies_container_values,
    ie_table_bearer_context_mod_fail_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_eutran_bearer_context_mod_request_o[]   = {51, 33, 34, 83, 84};
static const uint8_t  objset_crit_eutran_bearer_context_mod_request_o[] = {crit_e::reject,
                                                                           crit_e::reject,
                                                                           crit_e::reject,
                                                                           crit_e::ignore,
                                                                           crit_e::ignore};

// EUTRAN-BearerContextModificationRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_eutran_bearer_context_mod_request_container_values[] = {
    {offsetof(eutran_bearer_context_mod_request_container, drb_to_setup_mod_list_eutran),
     (int32_t)offsetof(eutran_bearer_context_mod_request_container, drb_to_setup_mod_list_eutran_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(eutran_bearer_context_mod_request_container::drb_to_setup_mod_list_eutran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_to_setup_mod_item_eutran_s::desc},
    {offsetof(eutran_bearer_context_mod_request_container, drb_to_modify_list_eutran),
     (int32_t)offsetof(eutran_bearer_context_mod_request_container, drb_to_modify_list_eutran_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(eutran_bearer_context_mod_request_container::drb_to_modify_list_eutran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_to_modify_item_eutran_s::desc},
    {offsetof(eutran_bearer_context_mod_request_container, drb_to_rem_list_eutran),
     (int32_t)offsetof(eutran_bearer_context_mod_request_container, drb_to_rem_list_eutran_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(eutran_bearer_context_mod_request_container::drb_to_rem_list_eutran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_to_rem_item_eutran_s::desc},
    {offsetof(eutran_bearer_context_mod_request_container, sub_profile_id_for_rfp),
     (int32_t)offsetof(eutran_bearer_context_mod_request_container, sub_profile_id_for_rfp_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{255},
     nullptr},
    {offsetof(eutran_bearer_context_mod_request_container, add_rrm_prio_idx),
     (int32_t)offsetof(eutran_bearer_context_mod_request_container, add_rrm_prio_idx_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(eutran_bearer_context_mod_request_container::add_rrm_prio_idx),
                                asn1::table::struct_json>::value}};
static const int32_t ie_table_eutran_bearer_context_mod_request_container_presences[] = {
    (int32_t)offsetof(eutran_bearer_context_mod_request_container, drb_to_setup_mod_list_eutran_present),
    (int32_t)offsetof(eutran_bearer_context_mod_request_container, drb_to_modify_list_eutran_present),
    (int32_t)offsetof(eutran_bearer_context_mod_request_container, drb_to_rem_list_eutran_present),
    (int32_t)offsetof(eutran_bearer_context_mod_request_container, sub_profile_id_for_rfp_present),
    (int32_t)offsetof(eutran_bearer_context_mod_request_container, add_rrm_prio_idx_present)};
const asn1::table::ie_container_desc eutran_bearer_context_mod_request_container::desc = {
    objset_id_eutran_bearer_context_mod_request_o,
    objset_crit_eutran_bearer_context_mod_request_o,
    ie_table_eutran_bearer_context_mod_request_container_values,
    ie_table_eutran_bearer_context_mod_request_container_presences,
    5,
    0,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ng_ran_bearer_context_mod_request_o[]   = {56, 43, 44};
static const uint8_t  objset_crit_ng_ran_bearer_context_mod_request_o[] = {crit_e::reject,
                                                                           crit_e::reject,
                                                                           crit_e::reject};

// NG-RAN-BearerContextModificationRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ng_ran_bearer_context_mod_request_container_values[] = {
    {offsetof(ng_ran_bearer_context_mod_request_container, pdu_session_res_to_setup_mod_list),
     (int32_t)offsetof(ng_ran_bearer_context_mod_request_container, pdu_session_res_to_setup_mod_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<
         decltype(ng_ran_bearer_context_mod_request_container::pdu_session_res_to_setup_mod_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &pdu_session_res_to_setup_mod_item_s::desc},
    {offsetof(ng_ran_bearer_context_mod_request_container, pdu_session_res_to_modify_list),
     (int32_t)offsetof(ng_ran_bearer_context_mod_request_container, pdu_session_res_to_modify_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<
         decltype(ng_ran_bearer_context_mod_request_container::pdu_session_res_to_modify_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &pdu_session_res_to_modify_item_s::desc},
    {offsetof(ng_ran_bearer_context_mod_request_container, pdu_session_res_to_rem_list),
     (int32_t)offsetof(ng_ran_bearer_context_mod_request_container, pdu_session_res_to_rem_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<decltype(ng_ran_bearer_context_mod_request_container::pdu_session_res_to_rem_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pdu_session_res_to_rem_item_s::desc}};
static const int32_t ie_table_ng_ran_bearer_context_mod_request_container_presences[] = {
    (int32_t)offsetof(ng_ran_bearer_context_mod_request_container, pdu_session_res_to_setup_mod_list_present),
    (int32_t)offsetof(ng_ran_bearer_context_mod_request_container, pdu_session_res_to_modify_list_present),
    (int32_t)offsetof(ng_ran_bearer_context_mod_request_container, pdu_session_res_to_rem_list_present)};
const asn1::table::ie_container_desc ng_ran_bearer_context_mod_request_container::desc = {
    objset_id_ng_ran_bearer_context_mod_request_o,
    objset_crit_ng_ran_bearer_context_mod_request_o,
    ie_table_ng_ran_bearer_context_mod_request_container_values,
    ie_table_ng_ran_bearer_context_mod_request_container_presences,
    3,
    0,
    0,
    65535,
    true,
    "value"};

// System-BearerContextModificationRequest ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_sys_bearer_context_mod_request_c_alternatives[] = {
    &asn1::table::alternative_ops<eutran_bearer_context_mod_request_container, false>::value,
    &asn1::table::alternative_ops<ng_ran_bearer_context_mod_request_container, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>,
                                  false>::value};
const asn1::table::choice_storage_desc sys_bearer_context_mod_request_c::storage_desc = {
    choice_storage_sys_bearer_context_mod_request_c_alternatives,
    3,
    "sys_bearer_context_mod_request_c"};

eutran_bearer_context_mod_request_container& sys_bearer_context_mod_request_c::set_e_utran_bearer_context_mod_request()
{
  set(types::e_utran_bearer_context_mod_request);
  return c.get<eutran_bearer_context_mod_request_container>();
}
ng_ran_bearer_context_mod_request_container& sys_bearer_context_mod_request_c::set_ng_ran_bearer_context_mod_request()
{
  set(types::ng_ran_bearer_context_mod_request);
  return c.get<ng_ran_bearer_context_mod_request_container>();
}
protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>&
sys_bearer_context_mod_request_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_sys_bearer_context_mod_request_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &eutran_bearer_context_mod_request_container::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &ng_ran_bearer_context_mod_request_container::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<sys_bearer_context_mod_request_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_sys_bearer_context_mod_request_c_names[] = {
    {"e-UTRAN-BearerContextModificationRequest", nullptr},
    {"nG-RAN-BearerContextModificationRequest", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc sys_bearer_context_mod_request_c::desc = {
    asn1::table::enum_int_type<sys_bearer_context_mod_request_c::types>(),
    sys_bearer_context_mod_request_c::types::nof_types,
    sys_bearer_context_mod_request_c::types::nof_exts,
    sys_bearer_context_mod_request_c::types::has_ext,
    true,
    choice_table_sys_bearer_context_mod_request_c_alternatives,
    nullptr,
    choice_table_sys_bearer_context_mod_request_c_names,
    "sys_bearer_context_mod_request_c",
    offsetof(sys_bearer_context_mod_request_c, c.type),
    offsetof(sys_bearer_context_mod_request_c, c.value),
    false,
    &sys_bearer_context_mod_request_c::storage_desc};

static const char* enum_names_sys_bearer_context_mod_request_c_types_values[] = {
    "e-UTRAN-BearerContextModificationRequest",
    "nG-RAN-BearerContextModificationRequest",
    "choice-extension"};
const asn1::table::enum_desc sys_bearer_context_mod_request_c::types_opts::desc = {
    enum_names_sys_bearer_context_mod_request_c_types_values,
    3,
    "sys_bearer_context_mod_request_c::types"};

static const uint32_t objset_id_bearer_context_mod_request_ies_o[] = {2,  3,  13,  14,  66,  17,  26,  59,  70,  18, 76,
                                                                      77, 23, 146, 150, 154, 174, 178, 187, 195, 196};
static const uint8_t  objset_crit_bearer_context_mod_request_ies_o[] = {
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::ignore, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// BearerContextModificationRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bearer_context_mod_request_ies_container_values[] = {
    {offsetof(bearer_context_mod_request_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_mod_request_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_mod_request_ies_container, security_info),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, security_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_info_s::desc},
    {offsetof(bearer_context_mod_request_ies_container, ue_dl_aggr_max_bit_rate),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, ue_dl_aggr_max_bit_rate_present),
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{0},
     uint64_t{4000000000000},
     nullptr},
    {offsetof(bearer_context_mod_request_ies_container, ue_dl_max_integrity_protected_data_rate),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, ue_dl_max_integrity_protected_data_rate_present),
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{0},
     uint64_t{4000000000000},
     nullptr},
    {offsetof(bearer_context_mod_request_ies_container, bearer_context_status_change),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, bearer_context_status_change_present),
     asn1::table::enum_op<decltype(bearer_context_mod_request_ies_container::bearer_context_status_change)>(),
     decltype(bearer_context_mod_request_ies_container::bearer_context_status_change)::has_ext,
     false,
     decltype(bearer_context_mod_request_ies_container::bearer_context_status_change)::nof_types,
     decltype(bearer_context_mod_request_ies_container::bearer_context_status_change)::nof_exts,
     nullptr,
     &bearer_context_status_change_opts::desc},
    {offsetof(bearer_context_mod_request_ies_container, new_ul_tnl_info_required),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, new_ul_tnl_info_required_present),
     asn1::table::enum_op<decltype(bearer_context_mod_request_ies_container::new_ul_tnl_info_required)>(),
     decltype(bearer_context_mod_request_ies_container::new_ul_tnl_info_required)::has_ext,
     false,
     decltype(bearer_context_mod_request_ies_container::new_ul_tnl_info_required)::nof_types,
     decltype(bearer_context_mod_request_ies_container::new_ul_tnl_info_required)::nof_exts,
     nullptr,
     &new_ul_tnl_info_required_opts::desc},
    {offsetof(bearer_context_mod_request_ies_container, ue_inactivity_timer),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, ue_inactivity_timer_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{7199},
     nullptr},
    {offsetof(bearer_context_mod_request_ies_container, data_discard_required),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, data_discard_required_present),
     asn1::table::enum_op<decltype(bearer_context_mod_request_ies_container::data_discard_required)>(),
     decltype(bearer_context_mod_request_ies_container::data_discard_required)::has_ext,
     false,
     decltype(bearer_context_mod_request_ies_container::data_discard_required)::nof_types,
     decltype(bearer_context_mod_request_ies_container::data_discard_required)::nof_exts,
     nullptr,
     &data_discard_required_opts::desc},
    {offsetof(bearer_context_mod_request_ies_container, sys_bearer_context_mod_request),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, sys_bearer_context_mod_request_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sys_bearer_context_mod_request_c::desc},
    {offsetof(bearer_context_mod_request_ies_container, ran_ue_id),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, ran_ue_id_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(bearer_context_mod_request_ies_container::ran_ue_id),
                                asn1::table::string_json>::value},
    {offsetof(bearer_context_mod_request_ies_container, gnb_du_id),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, gnb_du_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr},
    {offsetof(bearer_context_mod_request_ies_container, activity_notif_level),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, activity_notif_level_present),
     asn1::table::enum_op<decltype(bearer_context_mod_request_ies_container::activity_notif_level)>(),
     decltype(bearer_context_mod_request_ies_container::activity_notif_level)::has_ext,
     false,
     decltype(bearer_context_mod_request_ies_container::activity_notif_level)::nof_types,
     decltype(bearer_context_mod_request_ies_container::activity_notif_level)::nof_exts,
     nullptr,
     &activity_notif_level_opts::desc},
    {offsetof(bearer_context_mod_request_ies_container, mdt_polluted_meas_ind),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, mdt_polluted_meas_ind_present),
     asn1::table::enum_op<decltype(bearer_context_mod_request_ies_container::mdt_polluted_meas_ind)>(),
     decltype(bearer_context_mod_request_ies_container::mdt_polluted_meas_ind)::has_ext,
     false,
     decltype(bearer_context_mod_request_ies_container::mdt_polluted_meas_ind)::nof_types,
     decltype(bearer_context_mod_request_ies_container::mdt_polluted_meas_ind)::nof_exts,
     nullptr,
     &mdt_polluted_meas_ind_opts::desc},
    {offsetof(bearer_context_mod_request_ies_container, ue_slice_max_bit_rate_list),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, ue_slice_max_bit_rate_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     7,
     &asn1::table::item_list_codec<decltype(bearer_context_mod_request_ies_container::ue_slice_max_bit_rate_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ue_slice_max_bit_rate_item_s::desc},
    {offsetof(bearer_context_mod_request_ies_container, scg_activation_status),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, scg_activation_status_present),
     asn1::table::enum_op<decltype(bearer_context_mod_request_ies_container::scg_activation_status)>(),
     decltype(bearer_context_mod_request_ies_container::scg_activation_status)::has_ext,
     false,
     decltype(bearer_context_mod_request_ies_container::scg_activation_status)::nof_types,
     decltype(bearer_context_mod_request_ies_container::scg_activation_status)::nof_exts,
     nullptr,
     &scg_activation_status_opts::desc},
    {offsetof(bearer_context_mod_request_ies_container, sdt_continue_rohc),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, sdt_continue_rohc_present),
     asn1::table::enum_op<decltype(bearer_context_mod_request_ies_container::sdt_continue_rohc)>(),
     decltype(bearer_context_mod_request_ies_container::sdt_continue_rohc)::has_ext,
     false,
     decltype(bearer_context_mod_request_ies_container::sdt_continue_rohc)::nof_types,
     decltype(bearer_context_mod_request_ies_container::sdt_continue_rohc)::nof_exts,
     nullptr,
     &sdt_continue_rohc_opts::desc},
    {offsetof(bearer_context_mod_request_ies_container, management_based_mdt_plmn_mod_list),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, management_based_mdt_plmn_mod_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     16,
     &asn1::table::list_codec<decltype(bearer_context_mod_request_ies_container::management_based_mdt_plmn_mod_list),
                              asn1::table::string_json>::value},
    {offsetof(bearer_context_mod_request_ies_container, inactivity_info_request),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, inactivity_info_request_present),
     asn1::table::enum_op<decltype(bearer_context_mod_request_ies_container::inactivity_info_request)>(),
     decltype(bearer_context_mod_request_ies_container::inactivity_info_request)::has_ext,
     false,
     decltype(bearer_context_mod_request_ies_container::inactivity_info_request)::nof_types,
     decltype(bearer_context_mod_request_ies_container::inactivity_info_request)::nof_exts,
     nullptr,
     &inactivity_info_request_opts::desc},
    {offsetof(bearer_context_mod_request_ies_container, mt_sdt_info_request),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, mt_sdt_info_request_present),
     asn1::table::enum_op<decltype(bearer_context_mod_request_ies_container::mt_sdt_info_request)>(),
     decltype(bearer_context_mod_request_ies_container::mt_sdt_info_request)::has_ext,
     false,
     decltype(bearer_context_mod_request_ies_container::mt_sdt_info_request)::nof_types,
     decltype(bearer_context_mod_request_ies_container::mt_sdt_info_request)::nof_exts,
     nullptr,
     &mt_sdt_info_request_opts::desc},
    {offsetof(bearer_context_mod_request_ies_container, sdt_data_size_thres),
     (int32_t)offsetof(bearer_context_mod_request_ies_container, sdt_data_size_thres_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{191999},
     nullptr}};
static const int32_t ie_table_bearer_context_mod_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bearer_context_mod_request_ies_container, security_info_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, ue_dl_aggr_max_bit_rate_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, ue_dl_max_integrity_protected_data_rate_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, bearer_context_status_change_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, new_ul_tnl_info_required_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, ue_inactivity_timer_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, data_discard_required_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, sys_bearer_context_mod_request_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, ran_ue_id_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, gnb_du_id_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, activity_notif_level_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, mdt_polluted_meas_ind_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, ue_slice_max_bit_rate_list_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, scg_activation_status_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, sdt_continue_rohc_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, management_based_mdt_plmn_mod_list_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, inactivity_info_request_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, mt_sdt_info_request_present),
    (int32_t)offsetof(bearer_context_mod_request_ies_container, sdt_data_size_thres_present)};
const asn1::table::ie_container_desc bearer_context_mod_request_ies_container::desc = {
    objset_id_bearer_context_mod_request_ies_o,
    objset_crit_bearer_context_mod_request_ies_o,
    ie_table_bearer_context_mod_request_ies_container_values,
    ie_table_bearer_context_mod_request_ies_container_presences,
    21,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_eutran_bearer_context_mod_required_o[]   = {35, 36};
static const uint8_t  objset_crit_eutran_bearer_context_mod_required_o[] = {crit_e::reject, crit_e::reject};

// EUTRAN-BearerContextModificationRequired ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_eutran_bearer_context_mod_required_container_values[] = {
    {offsetof(eutran_bearer_context_mod_required_container, drb_required_to_modify_list_eutran),
     (int32_t)offsetof(eutran_bearer_context_mod_required_container, drb_required_to_modify_list_eutran_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<
         decltype(eutran_bearer_context_mod_required_container::drb_required_to_modify_list_eutran),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &drb_required_to_modify_item_eutran_s::desc},
    {offsetof(eutran_bearer_context_mod_required_container, drb_required_to_rem_list_eutran),
     (int32_t)offsetof(eutran_bearer_context_mod_required_container, drb_required_to_rem_list_eutran_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<
         decltype(eutran_bearer_context_mod_required_container::drb_required_to_rem_list_eutran),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &drb_required_to_rem_item_eutran_s::desc}};
static const int32_t ie_table_eutran_bearer_context_mod_required_container_presences[] = {
    (int32_t)offsetof(eutran_bearer_context_mod_required_container, drb_required_to_modify_list_eutran_present),
    (int32_t)offsetof(eutran_bearer_context_mod_required_container, drb_required_to_rem_list_eutran_present)};
const asn1::table::ie_container_desc eutran_bearer_context_mod_required_container::desc = {
    objset_id_eutran_bearer_context_mod_required_o,
    objset_crit_eutran_bearer_context_mod_required_o,
    ie_table_eutran_bearer_context_mod_required_container_values,
    ie_table_eutran_bearer_context_mod_required_container_presences,
    2,
    0,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ng_ran_bearer_context_mod_required_o[]   = {45, 44};
static const uint8_t  objset_crit_ng_ran_bearer_context_mod_required_o[] = {crit_e::reject, crit_e::reject};

// NG-RAN-BearerContextModificationRequired ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ng_ran_bearer_context_mod_required_container_values[] = {
    {offsetof(ng_ran_bearer_context_mod_required_container, pdu_session_res_required_to_modify_list),
     (int32_t)offsetof(ng_ran_bearer_context_mod_required_container, pdu_session_res_required_to_modify_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<
         decltype(ng_ran_bearer_context_mod_required_container::pdu_session_res_required_to_modify_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &pdu_session_res_required_to_modify_item_s::desc},
    {offsetof(ng_ran_bearer_context_mod_required_container, pdu_session_res_to_rem_list),
     (int32_t)offsetof(ng_ran_bearer_context_mod_required_container, pdu_session_res_to_rem_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<decltype(ng_ran_bearer_context_mod_required_container::pdu_session_res_to_rem_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pdu_session_res_to_rem_item_s::desc}};
static const int32_t ie_table_ng_ran_bearer_context_mod_required_container_presences[] = {
    (int32_t)offsetof(ng_ran_bearer_context_mod_required_container, pdu_session_res_required_to_modify_list_present),
    (int32_t)offsetof(ng_ran_bearer_context_mod_required_container, pdu_session_res_to_rem_list_present)};
const asn1::table::ie_container_desc ng_ran_bearer_context_mod_required_container::desc = {
    objset_id_ng_ran_bearer_context_mod_required_o,
    objset_crit_ng_ran_bearer_context_mod_required_o,
    ie_table_ng_ran_bearer_context_mod_required_container_values,
    ie_table_ng_ran_bearer_context_mod_required_container_presences,
    2,
    0,
    0,
    65535,
    true,
    "value"};

// System-BearerContextModificationRequired ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_sys_bearer_context_mod_required_c_alternatives[] = {
    &asn1::table::alternative_ops<eutran_bearer_context_mod_required_container, false>::value,
    &asn1::table::alternative_ops<ng_ran_bearer_context_mod_required_container, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>,
                                  false>::value};
const asn1::table::choice_storage_desc sys_bearer_context_mod_required_c::storage_desc = {
    choice_storage_sys_bearer_context_mod_required_c_alternatives,
    3,
    "sys_bearer_context_mod_required_c"};

eutran_bearer_context_mod_required_container&
sys_bearer_context_mod_required_c::set_e_utran_bearer_context_mod_required()
{
  set(types::e_utran_bearer_context_mod_required);
  return c.get<eutran_bearer_context_mod_required_container>();
}
ng_ran_bearer_context_mod_required_container&
sys_bearer_context_mod_required_c::set_ng_ran_bearer_context_mod_required()
{
  set(types::ng_ran_bearer_context_mod_required);
  return c.get<ng_ran_bearer_context_mod_required_container>();
}
protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>&
sys_bearer_context_mod_required_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_sys_bearer_context_mod_required_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &eutran_bearer_context_mod_required_container::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &ng_ran_bearer_context_mod_required_container::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<sys_bearer_context_mod_required_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_sys_bearer_context_mod_required_c_names[] = {
    {"e-UTRAN-BearerContextModificationRequired", nullptr},
    {"nG-RAN-BearerContextModificationRequired", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc sys_bearer_context_mod_required_c::desc = {
    asn1::table::enum_int_type<sys_bearer_context_mod_required_c::types>(),
    sys_bearer_context_mod_required_c::types::nof_types,
    sys_bearer_context_mod_required_c::types::nof_exts,
    sys_bearer_context_mod_required_c::types::has_ext,
    true,
    choice_table_sys_bearer_context_mod_required_c_alternatives,
    nullptr,
    choice_table_sys_bearer_context_mod_required_c_names,
    "sys_bearer_context_mod_required_c",
    offsetof(sys_bearer_context_mod_required_c, c.type),
    offsetof(sys_bearer_context_mod_required_c, c.value),
    false,
    &sys_bearer_context_mod_required_c::storage_desc};

static const char* enum_names_sys_bearer_context_mod_required_c_types_values[] = {
    "e-UTRAN-BearerContextModificationRequired",
    "nG-RAN-BearerContextModificationRequired",
    "choice-extension"};
const asn1::table::enum_desc sys_bearer_context_mod_required_c::types_opts::desc = {
    enum_names_sys_bearer_context_mod_required_c_types_values,
    3,
    "sys_bearer_context_mod_required_c::types"};

static const uint32_t objset_id_bearer_context_mod_required_ies_o[]  = {2, 3, 21};
static const uint8_t objset_crit_bearer_context_mod_required_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::reject};

// BearerContextModificationRequiredIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bearer_context_mod_required_ies_container_values[] = {
    {offsetof(bearer_context_mod_required_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_mod_required_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_mod_required_ies_container, sys_bearer_context_mod_required),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sys_bearer_context_mod_required_c::desc}};
static const int32_t ie_table_bearer_context_mod_required_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc bearer_context_mod_required_ies_container::desc = {
    objset_id_bearer_context_mod_required_ies_o,
    objset_crit_bearer_context_mod_required_ies_o,
    ie_table_bearer_context_mod_required_ies_container_values,
    ie_table_bearer_context_mod_required_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_eutran_bearer_context_mod_resp_o[]   = {52, 53, 39, 40, 85};
static const uint8_t  objset_crit_eutran_bearer_context_mod_resp_o[] = {crit_e::ignore,
                                                                        crit_e::ignore,
                                                                        crit_e::ignore,
                                                                        crit_e::ignore,
                                                                        crit_e::ignore};

// EUTRAN-BearerContextModificationResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_eutran_bearer_context_mod_resp_container_values[] = {
    {offsetof(eutran_bearer_context_mod_resp_container, drb_setup_mod_list_eutran),
     (int32_t)offsetof(eutran_bearer_context_mod_resp_container, drb_setup_mod_list_eutran_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(eutran_bearer_context_mod_resp_container::drb_setup_mod_list_eutran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_setup_mod_item_eutran_s::desc},
    {offsetof(eutran_bearer_context_mod_resp_container, drb_failed_mod_list_eutran),
     (int32_t)offsetof(eutran_bearer_context_mod_resp_container, drb_failed_mod_list_eutran_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(eutran_bearer_context_mod_resp_container::drb_failed_mod_list_eutran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_failed_mod_item_eutran_s::desc},
    {offsetof(eutran_bearer_context_mod_resp_container, drb_modified_list_eutran),
     (int32_t)offsetof(eutran_bearer_context_mod_resp_container, drb_modified_list_eutran_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(eutran_bearer_context_mod_resp_container::drb_modified_list_eutran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_modified_item_eutran_s::desc},
    {offsetof(eutran_bearer_context_mod_resp_container, drb_failed_to_modify_list_eutran),
     (int32_t)offsetof(eutran_bearer_context_mod_resp_container, drb_failed_to_modify_list_eutran_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(eutran_bearer_context_mod_resp_container::drb_failed_to_modify_list_eutran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_failed_to_modify_item_eutran_s::desc},
    {offsetof(eutran_bearer_context_mod_resp_container, retainability_meass_info),
     (int32_t)offsetof(eutran_bearer_context_mod_resp_container, retainability_meass_info_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(eutran_bearer_context_mod_resp_container::retainability_meass_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_remd_item_s::desc}};
static const int32_t ie_table_eutran_bearer_context_mod_resp_container_presences[] = {
    (int32_t)offsetof(eutran_bearer_context_mod_resp_container, drb_setup_mod_list_eutran_present),
    (int32_t)offsetof(eutran_bearer_context_mod_resp_container, drb_failed_mod_list_eutran_present),
    (int32_t)offsetof(eutran_bearer_context_mod_resp_container, drb_modified_list_eutran_present),
    (int32_t)offsetof(eutran_bearer_context_mod_resp_container, drb_failed_to_modify_list_eutran_present),
    (int32_t)offsetof(eutran_bearer_context_mod_resp_container, retainability_meass_info_present)};
const asn1::table::ie_container_desc eutran_bearer_context_mod_resp_container::desc = {
    objset_id_eutran_bearer_context_mod_resp_o,
    objset_crit_eutran_bearer_context_mod_resp_o,
    ie_table_eutran_bearer_context_mod_resp_container_values,
    ie_table_eutran_bearer_context_mod_resp_container_presences,
    5,
    0,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ng_ran_bearer_context_mod_resp_o[]   = {54, 55, 48, 49, 85};
static const uint8_t  objset_crit_ng_ran_bearer_context_mod_resp_o[] = {crit_e::reject,
                                                                        crit_e::reject,
                                                                        crit_e::reject,
                                                                        crit_e::reject,
                                                                        crit_e::ignore};

// NG-RAN-BearerContextModificationResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ng_ran_bearer_context_mod_resp_container_values[] = {
    {offsetof(ng_ran_bearer_context_mod_resp_container, pdu_session_res_setup_mod_list),
     (int32_t)offsetof(ng_ran_bearer_context_mod_resp_container, pdu_session_res_setup_mod_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<decltype(ng_ran_bearer_context_mod_resp_container::pdu_session_res_setup_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pdu_session_res_setup_mod_item_s::desc},
    {offsetof(ng_ran_bearer_context_mod_resp_container, pdu_session_res_failed_mod_list),
     (int32_t)offsetof(ng_ran_bearer_context_mod_resp_container, pdu_session_res_failed_mod_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<decltype(ng_ran_bearer_context_mod_resp_container::pdu_session_res_failed_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pdu_session_res_failed_mod_item_s::desc},
    {offsetof(ng_ran_bearer_context_mod_resp_container, pdu_session_res_modified_list),
     (int32_t)offsetof(ng_ran_bearer_context_mod_resp_container, pdu_session_res_modified_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<decltype(ng_ran_bearer_context_mod_resp_container::pdu_session_res_modified_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pdu_session_res_modified_item_s::desc},
    {offsetof(ng_ran_bearer_context_mod_resp_container, pdu_session_res_failed_to_modify_list),
     (int32_t)offsetof(ng_ran_bearer_context_mod_resp_container, pdu_session_res_failed_to_modify_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<
         decltype(ng_ran_bearer_context_mod_resp_container::pdu_session_res_failed_to_modify_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &pdu_session_res_failed_to_modify_item_s::desc},
    {offsetof(ng_ran_bearer_context_mod_resp_container, retainability_meass_info),
     (int32_t)offsetof(ng_ran_bearer_context_mod_resp_container, retainability_meass_info_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(ng_ran_bearer_context_mod_resp_container::retainability_meass_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_remd_item_s::desc}};
static const int32_t ie_table_ng_ran_bearer_context_mod_resp_container_presences[] = {
    (int32_t)offsetof(ng_ran_bearer_context_mod_resp_container, pdu_session_res_setup_mod_list_present),
    (int32_t)offsetof(ng_ran_bearer_context_mod_resp_container, pdu_session_res_failed_mod_list_present),
    (int32_t)offsetof(ng_ran_bearer_context_mod_resp_container, pdu_session_res_modified_list_present),
    (int32_t)offsetof(ng_ran_bearer_context_mod_resp_container, pdu_session_res_failed_to_modify_list_present),
    (int32_t)offsetof(ng_ran_bearer_context_mod_resp_container, retainability_meass_info_present)};
const asn1::table::ie_container_desc ng_ran_bearer_context_mod_resp_container::desc = {
    objset_id_ng_ran_bearer_context_mod_resp_o,
    objset_crit_ng_ran_bearer_context_mod_resp_o,
    ie_table_ng_ran_bearer_context_mod_resp_container_values,
    ie_table_ng_ran_bearer_context_mod_resp_container_presences,
    5,
    0,
    0,
    65535,
    true,
    "value"};

// System-BearerContextModificationResponse ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_sys_bearer_context_mod_resp_c_alternatives[] = {
    &asn1::table::alternative_ops<eutran_bearer_context_mod_resp_container, false>::value,
    &asn1::table::alternative_ops<ng_ran_bearer_context_mod_resp_container, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc sys_bearer_context_mod_resp_c::storage_desc = {
    choice_storage_sys_bearer_context_mod_resp_c_alternatives,
    3,
    "sys_bearer_context_mod_resp_c"};

eutran_bearer_context_mod_resp_container& sys_bearer_context_mod_resp_c::set_e_utran_bearer_context_mod_resp()
{
  set(types::e_utran_bearer_context_mod_resp);
  return c.get<eutran_bearer_context_mod_resp_container>();
}
ng_ran_bearer_context_mod_resp_container& sys_bearer_context_mod_resp_c::set_ng_ran_bearer_context_mod_resp()
{
  set(types::ng_ran_bearer_context_mod_resp);
  return c.get<ng_ran_bearer_context_mod_resp_container>();
}
protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>& sys_bearer_context_mod_resp_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_sys_bearer_context_mod_resp_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &eutran_bearer_context_mod_resp_container::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &ng_ran_bearer_context_mod_resp_container::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<sys_bearer_context_mod_resp_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_sys_bearer_context_mod_resp_c_names[] = {
    {"e-UTRAN-BearerContextModificationResponse", nullptr},
    {"nG-RAN-BearerContextModificationResponse", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc sys_bearer_context_mod_resp_c::desc = {
    asn1::table::enum_int_type<sys_bearer_context_mod_resp_c::types>(),
    sys_bearer_context_mod_resp_c::types::nof_types,
    sys_bearer_context_mod_resp_c::types::nof_exts,
    sys_bearer_context_mod_resp_c::types::has_ext,
    true,
    choice_table_sys_bearer_context_mod_resp_c_alternatives,
    nullptr,
    choice_table_sys_bearer_context_mod_resp_c_names,
    "sys_bearer_context_mod_resp_c",
    offsetof(sys_bearer_context_mod_resp_c, c.type),
    offsetof(sys_bearer_context_mod_resp_c, c.value),
    false,
    &sys_bearer_context_mod_resp_c::storage_desc};

static const char* enum_names_sys_bearer_context_mod_resp_c_types_values[] = {
    "e-UTRAN-BearerContextModificationResponse",
    "nG-RAN-BearerContextModificationResponse",
    "choice-extension"};
const asn1::table::enum_desc sys_bearer_context_mod_resp_c::types_opts::desc = {
    enum_names_sys_bearer_context_mod_resp_c_types_values,
    3,
    "sys_bearer_context_mod_resp_c::types"};

static const uint32_t objset_id_bearer_context_mod_resp_ies_o[]   = {2, 3, 19, 1, 188};
static const uint8_t  objset_crit_bearer_context_mod_resp_ies_o[] = {crit_e::reject,
                                                                     crit_e::reject,
                                                                     crit_e::ignore,
                                                                     crit_e::ignore,
                                                                     crit_e::ignore};

// BearerContextModificationResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bearer_context_mod_resp_ies_container_values[] = {
    {offsetof(bearer_context_mod_resp_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_mod_resp_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_mod_resp_ies_container, sys_bearer_context_mod_resp),
     (int32_t)offsetof(bearer_context_mod_resp_ies_container, sys_bearer_context_mod_resp_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sys_bearer_context_mod_resp_c::desc},
    {offsetof(bearer_context_mod_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(bearer_context_mod_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc},
    {offsetof(bearer_context_mod_resp_ies_container, ue_inactivity_info),
     (int32_t)offsetof(bearer_context_mod_resp_ies_container, ue_inactivity_info_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{7199},
     nullptr}};
static const int32_t ie_table_bearer_context_mod_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bearer_context_mod_resp_ies_container, sys_bearer_context_mod_resp_present),
    (int32_t)offsetof(bearer_context_mod_resp_ies_container, crit_diagnostics_present),
    (int32_t)offsetof(bearer_context_mod_resp_ies_container, ue_inactivity_info_present)};
const asn1::table::ie_container_desc bearer_context_mod_resp_ies_container::desc = {
    objset_id_bearer_context_mod_resp_ies_o,
    objset_crit_bearer_context_mod_resp_ies_o,
    ie_table_bearer_context_mod_resp_ies_container_values,
    ie_table_bearer_context_mod_resp_ies_container_presences,
    5,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bearer_context_release_cmd_ies_o[]   = {2, 3, 0};
static const uint8_t  objset_crit_bearer_context_release_cmd_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::ignore};

// BearerContextReleaseCommandIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bearer_context_release_cmd_ies_container_values[] = {
    {offsetof(bearer_context_release_cmd_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_release_cmd_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_release_cmd_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const int32_t ie_table_bearer_context_release_cmd_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc bearer_context_release_cmd_ies_container::desc = {
    objset_id_bearer_context_release_cmd_ies_o,
    objset_crit_bearer_context_release_cmd_ies_o,
    ie_table_bearer_context_release_cmd_ies_container_values,
    ie_table_bearer_context_release_cmd_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bearer_context_release_complete_ies_o[]   = {2, 3, 1, 85};
static const uint8_t  objset_crit_bearer_context_release_complete_ies_o[] = {crit_e::reject,
                                                                             crit_e::reject,
                                                                             crit_e::ignore,
                                                                             crit_e::ignore};

// BearerContextReleaseCompleteIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bearer_context_release_complete_ies_container_values[] = {
    {offsetof(bearer_context_release_complete_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_release_complete_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_release_complete_ies_container, crit_diagnostics),
     (int32_t)offsetof(bearer_context_release_complete_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc},
    {offsetof(bearer_context_release_complete_ies_container, retainability_meass_info),
     (int32_t)offsetof(bearer_context_release_complete_ies_container, retainability_meass_info_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(bearer_context_release_complete_ies_container::retainability_meass_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_remd_item_s::desc}};
static const int32_t ie_table_bearer_context_release_complete_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bearer_context_release_complete_ies_container, crit_diagnostics_present),
    (int32_t)offsetof(bearer_context_release_complete_ies_container, retainability_meass_info_present)};
const asn1::table::ie_container_desc bearer_context_release_complete_ies_container::desc = {
    objset_id_bearer_context_release_complete_ies_o,
    objset_crit_bearer_context_release_complete_ies_o,
    ie_table_bearer_context_release_complete_ies_container_values,
    ie_table_bearer_context_release_complete_ies_container_presences,
    4,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bearer_context_release_request_ies_o[]   = {2, 3, 22, 0};
static const uint8_t  objset_crit_bearer_context_release_request_ies_o[] = {crit_e::reject,
                                                                            crit_e::reject,
                                                                            crit_e::ignore,
                                                                            crit_e::ignore};

// BearerContextReleaseRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bearer_context_release_request_ies_container_values[] = {
    {offsetof(bearer_context_release_request_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_release_request_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_release_request_ies_container, drb_status_list),
     (int32_t)offsetof(bearer_context_release_request_ies_container, drb_status_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(bearer_context_release_request_ies_container::drb_status_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_status_item_s::desc},
    {offsetof(bearer_context_release_request_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const int32_t ie_table_bearer_context_release_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bearer_context_release_request_ies_container, drb_status_list_present),
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc bearer_context_release_request_ies_container::desc = {
    objset_id_bearer_context_release_request_ies_o,
    objset_crit_bearer_context_release_request_ies_o,
    ie_table_bearer_context_release_request_ies_container_values,
    ie_table_bearer_context_release_request_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_bearer_context_setup_fail_ies_o[]   = {2, 3, 0, 1};
static const uint8_t  objset_crit_bearer_context_setup_fail_ies_o[] = {crit_e::reject,
                                                                       crit_e::ignore,
                                                                       crit_e::ignore,
                                                                       crit_e::ignore};

// BearerContextSetupFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bearer_context_setup_fail_ies_container_values[] = {
    {offsetof(bearer_context_setup_fail_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_setup_fail_ies_container, gnb_cu_up_ue_e1ap_id),
     (int32_t)offsetof(bearer_context_setup_fail_ies_container, gnb_cu_up_ue_e1ap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_setup_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(bearer_context_setup_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(bearer_context_setup_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_bearer_context_setup_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bearer_context_setup_fail_ies_container, gnb_cu_up_ue_e1ap_id_present),
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bearer_context_setup_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc bearer_context_setup_fail_ies_container::desc = {
    objset_id_bearer_context_setup_fail_ies_o,
    objset_crit_bearer_context_setup_fail_ies_o,
    ie_table_bearer_context_setup_fail_ies_container_values,
    ie_table_bearer_context_setup_fail_ies_container_presences,
    4,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_eutran_bearer_context_setup_request_o[]   = {32, 83, 84};
static const uint8_t  objset_crit_eutran_bearer_context_setup_request_o[] = {crit_e::reject,
                                                                             crit_e::ignore,
                                                                             crit_e::ignore};

// EUTRAN-BearerContextSetupRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_eutran_bearer_context_setup_request_container_values[] = {
    {offsetof(eutran_bearer_context_setup_request_container, drb_to_setup_list_eutran),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(eutran_bearer_context_setup_request_container::drb_to_setup_list_eutran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_to_setup_item_eutran_s::desc},
    {offsetof(eutran_bearer_context_setup_request_container, sub_profile_id_for_rfp),
     (int32_t)offsetof(eutran_bearer_context_setup_request_container, sub_profile_id_for_rfp_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{255},
     nullptr},
    {offsetof(eutran_bearer_context_setup_request_container, add_rrm_prio_idx),
     (int32_t)offsetof(eutran_bearer_context_setup_request_container, add_rrm_prio_idx_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(eutran_bearer_context_setup_request_container::add_rrm_prio_idx),
                                asn1::table::struct_json>::value}};
static const int32_t ie_table_eutran_bearer_context_setup_request_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(eutran_bearer_context_setup_request_container, sub_profile_id_for_rfp_present),
    (int32_t)offsetof(eutran_bearer_context_setup_request_container, add_rrm_prio_idx_present)};
const asn1::table::ie_container_desc eutran_bearer_context_setup_request_container::desc = {
    objset_id_eutran_bearer_context_setup_request_o,
    objset_crit_eutran_bearer_context_setup_request_o,
    ie_table_eutran_bearer_context_setup_request_container_values,
    ie_table_eutran_bearer_context_setup_request_container_presences,
    3,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ng_ran_bearer_context_setup_request_o[]   = {42};
static const uint8_t  objset_crit_ng_ran_bearer_context_setup_request_o[] = {crit_e::reject};

// NG-RAN-BearerContextSetupRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ng_ran_bearer_context_setup_request_container_values[] = {
    {offsetof(ng_ran_bearer_context_setup_request_container, pdu_session_res_to_setup_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<
         decltype(ng_ran_bearer_context_setup_request_container::pdu_session_res_to_setup_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &pdu_session_res_to_setup_item_s::desc}};
static const int32_t ie_table_ng_ran_bearer_context_setup_request_container_presences[] = {
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc ng_ran_bearer_context_setup_request_container::desc = {
    objset_id_ng_ran_bearer_context_setup_request_o,
    objset_crit_ng_ran_bearer_context_setup_request_o,
    ie_table_ng_ran_bearer_context_setup_request_container_values,
    ie_table_ng_ran_bearer_context_setup_request_container_presences,
    1,
    1,
    0,
    65535,
    true,
    "value"};

// System-BearerContextSetupRequest ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_sys_bearer_context_setup_request_c_alternatives[] = {
    &asn1::table::alternative_ops<eutran_bearer_context_setup_request_container, false>::value,
    &asn1::table::alternative_ops<ng_ran_bearer_context_setup_request_container, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>,
                                  false>::value};
const asn1::table::choice_storage_desc sys_bearer_context_setup_request_c::storage_desc = {
    choice_storage_sys_bearer_context_setup_request_c_alternatives,
    3,
    "sys_bearer_context_setup_request_c"};

eutran_bearer_context_setup_request_container&
sys_bearer_context_setup_request_c::set_e_utran_bearer_context_setup_request()
{
  set(types::e_utran_bearer_context_setup_request);
  return c.get<eutran_bearer_context_setup_request_container>();
}
ng_ran_bearer_context_setup_request_container&
sys_bearer_context_setup_request_c::set_ng_ran_bearer_context_setup_request()
{
  set(types::ng_ran_bearer_context_setup_request);
  return c.get<ng_ran_bearer_context_setup_request_container>();
}
protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>&
sys_bearer_context_setup_request_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_sys_bearer_context_setup_request_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &eutran_bearer_context_setup_request_container::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &ng_ran_bearer_context_setup_request_container::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<sys_bearer_context_setup_request_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_sys_bearer_context_setup_request_c_names[] = {
    {"e-UTRAN-BearerContextSetupRequest", nullptr},
    {"nG-RAN-BearerContextSetupRequest", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc sys_bearer_context_setup_request_c::desc = {
    asn1::table::enum_int_type<sys_bearer_context_setup_request_c::types>(),
    sys_bearer_context_setup_request_c::types::nof_types,
    sys_bearer_context_setup_request_c::types::nof_exts,
    sys_bearer_context_setup_request_c::types::has_ext,
    true,
    choice_table_sys_bearer_context_setup_request_c_alternatives,
    nullptr,
    choice_table_sys_bearer_context_setup_request_c_names,
    "sys_bearer_context_setup_request_c",
    offsetof(sys_bearer_context_setup_request_c, c.type),
    offsetof(sys_bearer_context_setup_request_c, c.value),
    false,
    &sys_bearer_context_setup_request_c::storage_desc};

static const char* enum_names_sys_bearer_context_setup_request_c_types_values[] = {"e-UTRAN-BearerContextSetupRequest",
                                                                                   "nG-RAN-BearerContextSetupRequest",
                                                                                   "choice-extension"};
const asn1::table::enum_desc sys_bearer_context_setup_request_c::types_opts::desc = {
    enum_names_sys_bearer_context_setup_request_c_types_values,
    3,
    "sys_bearer_context_setup_request_c::types"};

static const uint32_t objset_id_bearer_context_setup_request_ies_o[] = {
    2, 13, 14, 66, 58, 23, 59, 17, 15, 76, 77, 81, 111, 113, 121, 134, 139, 3, 146, 150, 154, 195, 196};
static const uint8_t objset_crit_bearer_context_setup_request_ies_o[] = {
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::reject, crit_e::ignore, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// BearerContextSetupRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bearer_context_setup_request_ies_container_values[] = {
    {offsetof(bearer_context_setup_request_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_setup_request_ies_container, security_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_info_s::desc},
    {offsetof(bearer_context_setup_request_ies_container, ue_dl_aggr_max_bit_rate),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{0},
     uint64_t{4000000000000},
     nullptr},
    {offsetof(bearer_context_setup_request_ies_container, ue_dl_max_integrity_protected_data_rate),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, ue_dl_max_integrity_protected_data_rate_present),
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{0},
     uint64_t{4000000000000},
     nullptr},
    {offsetof(bearer_context_setup_request_ies_container, serving_plmn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(bearer_context_setup_request_ies_container::serving_plmn),
                                asn1::table::string_json>::value},
    {offsetof(bearer_context_setup_request_ies_container, activity_notif_level),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(bearer_context_setup_request_ies_container::activity_notif_level)>(),
     decltype(bearer_context_setup_request_ies_container::activity_notif_level)::has_ext,
     false,
     decltype(bearer_context_setup_request_ies_container::activity_notif_level)::nof_types,
     decltype(bearer_context_setup_request_ies_container::activity_notif_level)::nof_exts,
     nullptr,
     &activity_notif_level_opts::desc},
    {offsetof(bearer_context_setup_request_ies_container, ue_inactivity_timer),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, ue_inactivity_timer_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{7199},
     nullptr},
    {offsetof(bearer_context_setup_request_ies_container, bearer_context_status_change),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, bearer_context_status_change_present),
     asn1::table::enum_op<decltype(bearer_context_setup_request_ies_container::bearer_context_status_change)>(),
     decltype(bearer_context_setup_request_ies_container::bearer_context_status_change)::has_ext,
     false,
     decltype(bearer_context_setup_request_ies_container::bearer_context_status_change)::nof_types,
     decltype(bearer_context_setup_request_ies_container::bearer_context_status_change)::nof_exts,
     nullptr,
     &bearer_context_status_change_opts::desc},
    {offsetof(bearer_context_setup_request_ies_container, sys_bearer_context_setup_request),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sys_bearer_context_setup_request_c::desc},
    {offsetof(bearer_context_setup_request_ies_container, ran_ue_id),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, ran_ue_id_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(bearer_context_setup_request_ies_container::ran_ue_id),
                                asn1::table::string_json>::value},
    {offsetof(bearer_context_setup_request_ies_container, gnb_du_id),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, gnb_du_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr},
    {offsetof(bearer_context_setup_request_ies_container, trace_activation),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, trace_activation_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trace_activation_s::desc},
    {offsetof(bearer_context_setup_request_ies_container, npn_context_info),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, npn_context_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &npn_context_info_c::desc},
    {offsetof(bearer_context_setup_request_ies_container, management_based_mdt_plmn_list),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, management_based_mdt_plmn_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     15,
     &asn1::table::list_codec<decltype(bearer_context_setup_request_ies_container::management_based_mdt_plmn_list),
                              asn1::table::string_json>::value},
    {offsetof(bearer_context_setup_request_ies_container, cho_initiation),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, cho_initiation_present),
     asn1::table::enum_op<decltype(bearer_context_setup_request_ies_container::cho_initiation)>(),
     decltype(bearer_context_setup_request_ies_container::cho_initiation)::has_ext,
     false,
     decltype(bearer_context_setup_request_ies_container::cho_initiation)::nof_types,
     decltype(bearer_context_setup_request_ies_container::cho_initiation)::nof_exts,
     nullptr,
     &cho_initiation_opts::desc},
    {offsetof(bearer_context_setup_request_ies_container, add_ho_info),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, add_ho_info_present),
     asn1::table::enum_op<decltype(bearer_context_setup_request_ies_container::add_ho_info)>(),
     decltype(bearer_context_setup_request_ies_container::add_ho_info)::has_ext,
     false,
     decltype(bearer_context_setup_request_ies_container::add_ho_info)::nof_types,
     decltype(bearer_context_setup_request_ies_container::add_ho_info)::nof_exts,
     nullptr,
     &add_ho_info_opts::desc},
    {offsetof(bearer_context_setup_request_ies_container, direct_forwarding_path_availability),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, direct_forwarding_path_availability_present),
     asn1::table::enum_op<decltype(bearer_context_setup_request_ies_container::direct_forwarding_path_availability)>(),
     decltype(bearer_context_setup_request_ies_container::direct_forwarding_path_availability)::has_ext,
     false,
     decltype(bearer_context_setup_request_ies_container::direct_forwarding_path_availability)::nof_types,
     decltype(bearer_context_setup_request_ies_container::direct_forwarding_path_availability)::nof_exts,
     nullptr,
     &direct_forwarding_path_availability_opts::desc},
    {offsetof(bearer_context_setup_request_ies_container, gnb_cu_up_ue_e1ap_id),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, gnb_cu_up_ue_e1ap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_setup_request_ies_container, mdt_polluted_meas_ind),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, mdt_polluted_meas_ind_present),
     asn1::table::enum_op<decltype(bearer_context_setup_request_ies_container::mdt_polluted_meas_ind)>(),
     decltype(bearer_context_setup_request_ies_container::mdt_polluted_meas_ind)::has_ext,
     false,
     decltype(bearer_context_setup_request_ies_container::mdt_polluted_meas_ind)::nof_types,
     decltype(bearer_context_setup_request_ies_container::mdt_polluted_meas_ind)::nof_exts,
     nullptr,
     &mdt_polluted_meas_ind_opts::desc},
    {offsetof(bearer_context_setup_request_ies_container, ue_slice_max_bit_rate_list),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, ue_slice_max_bit_rate_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     7,
     &asn1::table::item_list_codec<decltype(bearer_context_setup_request_ies_container::ue_slice_max_bit_rate_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ue_slice_max_bit_rate_item_s::desc},
    {offsetof(bearer_context_setup_request_ies_container, scg_activation_status),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, scg_activation_status_present),
     asn1::table::enum_op<decltype(bearer_context_setup_request_ies_container::scg_activation_status)>(),
     decltype(bearer_context_setup_request_ies_container::scg_activation_status)::has_ext,
     false,
     decltype(bearer_context_setup_request_ies_container::scg_activation_status)::nof_types,
     decltype(bearer_context_setup_request_ies_container::scg_activation_status)::nof_exts,
     nullptr,
     &scg_activation_status_opts::desc},
    {offsetof(bearer_context_setup_request_ies_container, mt_sdt_info_request),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, mt_sdt_info_request_present),
     asn1::table::enum_op<decltype(bearer_context_setup_request_ies_container::mt_sdt_info_request)>(),
     decltype(bearer_context_setup_request_ies_container::mt_sdt_info_request)::has_ext,
     false,
     decltype(bearer_context_setup_request_ies_container::mt_sdt_info_request)::nof_types,
     decltype(bearer_context_setup_request_ies_container::mt_sdt_info_request)::nof_exts,
     nullptr,
     &mt_sdt_info_request_opts::desc},
    {offsetof(bearer_context_setup_request_ies_container, sdt_data_size_thres),
     (int32_t)offsetof(bearer_context_setup_request_ies_container, sdt_data_size_thres_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{191999},
     nullptr}};
static const int32_t ie_table_bearer_context_setup_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bearer_context_setup_request_ies_container, ue_dl_max_integrity_protected_data_rate_present),
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bearer_context_setup_request_ies_container, ue_inactivity_timer_present),
    (int32_t)offsetof(bearer_context_setup_request_ies_container, bearer_context_status_change_present),
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bearer_context_setup_request_ies_container, ran_ue_id_present),
    (int32_t)offsetof(bearer_context_setup_request_ies_container, gnb_du_id_present),
    (int32_t)offsetof(bearer_context_setup_request_ies_container, trace_activation_present),
    (int32_t)offsetof(bearer_context_setup_request_ies_container, npn_context_info_present),
    (int32_t)offsetof(bearer_context_setup_request_ies_container, management_based_mdt_plmn_list_present),
    (int32_t)offsetof(bearer_context_setup_request_ies_container, cho_initiation_present),
    (int32_t)offsetof(bearer_context_setup_request_ies_container, add_ho_info_present),
    (int32_t)offsetof(bearer_context_setup_request_ies_container, direct_forwarding_path_availability_present),
    (int32_t)offsetof(bearer_context_setup_request_ies_container, gnb_cu_up_ue_e1ap_id_present),
    (int32_t)offsetof(bearer_context_setup_request_ies_container, mdt_polluted_meas_ind_present),
    (int32_t)offsetof(bearer_context_setup_request_ies_container, ue_slice_max_bit_rate_list_present),
    (int32_t)offsetof(bearer_context_setup_request_ies_container, scg_activation_status_present),
    (int32_t)offsetof(bearer_context_setup_request_ies_container, mt_sdt_info_request_present),
    (int32_t)offsetof(bearer_context_setup_request_ies_container, sdt_data_size_thres_present)};
const asn1::table::ie_container_desc bearer_context_setup_request_ies_container::desc = {
    objset_id_bearer_context_setup_request_ies_o,
    objset_crit_bearer_context_setup_request_ies_o,
    ie_table_bearer_context_setup_request_ies_container_values,
    ie_table_bearer_context_setup_request_ies_container_presences,
    23,
    6,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_eutran_bearer_context_setup_resp_o[]   = {37, 38};
static const uint8_t  objset_crit_eutran_bearer_context_setup_resp_o[] = {crit_e::ignore, crit_e::ignore};

// EUTRAN-BearerContextSetupResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_eutran_bearer_context_setup_resp_container_values[] = {
    {offsetof(eutran_bearer_context_setup_resp_container, drb_setup_list_eutran),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(eutran_bearer_context_setup_resp_container::drb_setup_list_eutran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_setup_item_eutran_s::desc},
    {offsetof(eutran_bearer_context_setup_resp_container, drb_failed_list_eutran),
     (int32_t)offsetof(eutran_bearer_context_setup_resp_container, drb_failed_list_eutran_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(eutran_bearer_context_setup_resp_container::drb_failed_list_eutran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_failed_item_eutran_s::desc}};
static const int32_t ie_table_eutran_bearer_context_setup_resp_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(eutran_bearer_context_setup_resp_container, drb_failed_list_eutran_present)};
const asn1::table::ie_container_desc eutran_bearer_context_setup_resp_container::desc = {
    objset_id_eutran_bearer_context_setup_resp_o,
    objset_crit_eutran_bearer_context_setup_resp_o,
    ie_table_eutran_bearer_context_setup_resp_container_values,
    ie_table_eutran_bearer_context_setup_resp_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ng_ran_bearer_context_setup_resp_o[]   = {46, 47};
static const uint8_t  objset_crit_ng_ran_bearer_context_setup_resp_o[] = {crit_e::ignore, crit_e::ignore};

// NG-RAN-BearerContextSetupResponse ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ng_ran_bearer_context_setup_resp_container_values[] = {
    {offsetof(ng_ran_bearer_context_setup_resp_container, pdu_session_res_setup_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<decltype(ng_ran_bearer_context_setup_resp_container::pdu_session_res_setup_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pdu_session_res_setup_item_s::desc},
    {offsetof(ng_ran_bearer_context_setup_resp_container, pdu_session_res_failed_list),
     (int32_t)offsetof(ng_ran_bearer_context_setup_resp_container, pdu_session_res_failed_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<decltype(ng_ran_bearer_context_setup_resp_container::pdu_session_res_failed_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pdu_session_res_failed_item_s::desc}};
static const int32_t ie_table_ng_ran_bearer_context_setup_resp_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(ng_ran_bearer_context_setup_resp_container, pdu_session_res_failed_list_present)};
const asn1::table::ie_container_desc ng_ran_bearer_context_setup_resp_container::desc = {
    objset_id_ng_ran_bearer_context_setup_resp_o,
    objset_crit_ng_ran_bearer_context_setup_resp_o,
    ie_table_ng_ran_bearer_context_setup_resp_container_values,
    ie_table_ng_ran_bearer_context_setup_resp_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

// System-BearerContextSetupResponse ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_sys_bearer_context_setup_resp_c_alternatives[] = {
    &asn1::table::alternative_ops<eutran_bearer_context_setup_resp_container, false>::value,
    &asn1::table::alternative_ops<ng_ran_bearer_context_setup_resp_container, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>,
                                  false>::value};
const asn1::table::choice_storage_desc sys_bearer_context_setup_resp_c::storage_desc = {
    choice_storage_sys_bearer_context_setup_resp_c_alternatives,
    3,
    "sys_bearer_context_setup_resp_c"};

eutran_bearer_context_setup_resp_container& sys_bearer_context_setup_resp_c::set_e_utran_bearer_context_setup_resp()
{
  set(types::e_utran_bearer_context_setup_resp);
  return c.get<eutran_bearer_context_setup_resp_container>();
}
ng_ran_bearer_context_setup_resp_container& sys_bearer_context_setup_resp_c::set_ng_ran_bearer_context_setup_resp()
{
  set(types::ng_ran_bearer_context_setup_resp);
  return c.get<ng_ran_bearer_context_setup_resp_container>();
}
protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>&
sys_bearer_context_setup_resp_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_sys_bearer_context_setup_resp_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &eutran_bearer_context_setup_resp_container::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &ng_ran_bearer_context_setup_resp_container::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<sys_bearer_context_setup_resp_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_sys_bearer_context_setup_resp_c_names[] = {
    {"e-UTRAN-BearerContextSetupResponse", nullptr},
    {"nG-RAN-BearerContextSetupResponse", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc sys_bearer_context_setup_resp_c::desc = {
    asn1::table::enum_int_type<sys_bearer_context_setup_resp_c::types>(),
    sys_bearer_context_setup_resp_c::types::nof_types,
    sys_bearer_context_setup_resp_c::types::nof_exts,
    sys_bearer_context_setup_resp_c::types::has_ext,
    true,
    choice_table_sys_bearer_context_setup_resp_c_alternatives,
    nullptr,
    choice_table_sys_bearer_context_setup_resp_c_names,
    "sys_bearer_context_setup_resp_c",
    offsetof(sys_bearer_context_setup_resp_c, c.type),
    offsetof(sys_bearer_context_setup_resp_c, c.value),
    false,
    &sys_bearer_context_setup_resp_c::storage_desc};

static const char* enum_names_sys_bearer_context_setup_resp_c_types_values[]   = {"e-UTRAN-BearerContextSetupResponse",
                                                                                  "nG-RAN-BearerContextSetupResponse",
                                                                                  "choice-extension"};
const asn1::table::enum_desc sys_bearer_context_setup_resp_c::types_opts::desc = {
    enum_names_sys_bearer_context_setup_resp_c_types_values,
    3,
    "sys_bearer_context_setup_resp_c::types"};

static const uint32_t objset_id_bearer_context_setup_resp_ies_o[]   = {2, 3, 16, 1};
static const uint8_t  objset_crit_bearer_context_setup_resp_ies_o[] = {crit_e::reject,
                                                                       crit_e::reject,
                                                                       crit_e::ignore,
                                                                       crit_e::ignore};

// BearerContextSetupResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_bearer_context_setup_resp_ies_container_values[] = {
    {offsetof(bearer_context_setup_resp_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_setup_resp_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(bearer_context_setup_resp_ies_container, sys_bearer_context_setup_resp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sys_bearer_context_setup_resp_c::desc},
    {offsetof(bearer_context_setup_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(bearer_context_setup_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_bearer_context_setup_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(bearer_context_setup_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc bearer_context_setup_resp_ies_container::desc = {
    objset_id_bearer_context_setup_resp_ies_o,
    objset_crit_bearer_context_setup_resp_ies_o,
    ie_table_bearer_context_setup_resp_ies_container_values,
    ie_table_bearer_context_setup_resp_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_cell_traffic_trace_ies_o[] = {2, 3, 82, 114, 115, 117};
static const uint8_t  objset_crit_cell_traffic_trace_ies_o[] =
    {crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// CellTrafficTraceIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_cell_traffic_trace_ies_container_values[] = {
    {offsetof(cell_traffic_trace_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(cell_traffic_trace_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(cell_traffic_trace_ies_container, trace_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cell_traffic_trace_ies_container::trace_id), asn1::table::string_json>::value},
    {offsetof(cell_traffic_trace_ies_container, trace_collection_entity_ip_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cell_traffic_trace_ies_container::trace_collection_entity_ip_address),
                                asn1::table::struct_json>::value},
    {offsetof(cell_traffic_trace_ies_container, privacy_ind),
     (int32_t)offsetof(cell_traffic_trace_ies_container, privacy_ind_present),
     asn1::table::enum_op<decltype(cell_traffic_trace_ies_container::privacy_ind)>(),
     decltype(cell_traffic_trace_ies_container::privacy_ind)::has_ext,
     false,
     decltype(cell_traffic_trace_ies_container::privacy_ind)::nof_types,
     decltype(cell_traffic_trace_ies_container::privacy_ind)::nof_exts,
     nullptr,
     &privacy_ind_opts::desc},
    {offsetof(cell_traffic_trace_ies_container, ur_iaddress),
     (int32_t)offsetof(cell_traffic_trace_ies_container, ur_iaddress_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cell_traffic_trace_ies_container::ur_iaddress),
                                asn1::table::string_json>::value}};
static const int32_t ie_table_cell_traffic_trace_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(cell_traffic_trace_ies_container, privacy_ind_present),
    (int32_t)offsetof(cell_traffic_trace_ies_container, ur_iaddress_present)};
const asn1::table::ie_container_desc cell_traffic_trace_ies_container::desc = {
    objset_id_cell_traffic_trace_ies_o,
    objset_crit_cell_traffic_trace_ies_o,
    ie_table_cell_traffic_trace_ies_container_values,
    ie_table_cell_traffic_trace_ies_container_presences,
    6,
    4,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_dl_data_notif_ies_o[] = {2, 3, 63, 67, 194, 197};
static const uint8_t  objset_crit_dl_data_notif_ies_o[] =
    {crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// DLDataNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_dl_data_notif_ies_container_values[] = {
    {offsetof(dl_data_notif_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(dl_data_notif_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(dl_data_notif_ies_container, ppi),
     (int32_t)offsetof(dl_data_notif_ies_container, ppi_present),
     asn1::table::field_op::int_u8,
     true,
     true,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(dl_data_notif_ies_container, pdu_session_to_notify_list),
     (int32_t)offsetof(dl_data_notif_ies_container, pdu_session_to_notify_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<decltype(dl_data_notif_ies_container::pdu_session_to_notify_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pdu_session_to_notify_item_s::desc},
    {offsetof(dl_data_notif_ies_container, mt_sdt_info),
     (int32_t)offsetof(dl_data_notif_ies_container, mt_sdt_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mt_sdt_info_s::desc},
    {offsetof(dl_data_notif_ies_container, sdt_data_size_thres_crossed),
     (int32_t)offsetof(dl_data_notif_ies_container, sdt_data_size_thres_crossed_present),
     asn1::table::enum_op<decltype(dl_data_notif_ies_container::sdt_data_size_thres_crossed)>(),
     decltype(dl_data_notif_ies_container::sdt_data_size_thres_crossed)::has_ext,
     false,
     decltype(dl_data_notif_ies_container::sdt_data_size_thres_crossed)::nof_types,
     decltype(dl_data_notif_ies_container::sdt_data_size_thres_crossed)::nof_exts,
     nullptr,
     &sdt_data_size_thres_crossed_opts::desc}};
static const int32_t ie_table_dl_data_notif_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(dl_data_notif_ies_container, ppi_present),
    (int32_t)offsetof(dl_data_notif_ies_container, pdu_session_to_notify_list_present),
    (int32_t)offsetof(dl_data_notif_ies_container, mt_sdt_info_present),
    (int32_t)offsetof(dl_data_notif_ies_container, sdt_data_size_thres_crossed_present)};
const asn1::table::ie_container_desc dl_data_notif_ies_container::desc = {
    objset_id_dl_data_notif_ies_o,
    objset_crit_dl_data_notif_ies_o,
    ie_table_dl_data_notif_ies_container_values,
    ie_table_dl_data_notif_ies_container_presences,
    6,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_data_usage_report_ies_o[]   = {2, 3, 25};
static const uint8_t  objset_crit_data_usage_report_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::ignore};

// DataUsageReportIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_data_usage_report_ies_container_values[] = {
    {offsetof(data_usage_report_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(data_usage_report_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(data_usage_report_ies_container, data_usage_report_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(data_usage_report_ies_container::data_usage_report_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &data_usage_report_item_s::desc}};
static const int32_t ie_table_data_usage_report_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                              asn1::table::seq_field::mandatory,
                                                                              asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc data_usage_report_ies_container::desc = {
    objset_id_data_usage_report_ies_o,
    objset_crit_data_usage_report_ies_o,
    ie_table_data_usage_report_ies_container_values,
    ie_table_data_usage_report_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_deactiv_trace_ies_o[]   = {2, 3, 82};
static const uint8_t  objset_crit_deactiv_trace_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::ignore};

// DeactivateTraceIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_deactiv_trace_ies_container_values[] = {
    {offsetof(deactiv_trace_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(deactiv_trace_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(deactiv_trace_ies_container, trace_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(deactiv_trace_ies_container::trace_id), asn1::table::string_json>::value}};
static const int32_t ie_table_deactiv_trace_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                          asn1::table::seq_field::mandatory,
                                                                          asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc deactiv_trace_ies_container::desc = {
    objset_id_deactiv_trace_ies_o,
    objset_crit_deactiv_trace_ies_o,
    ie_table_deactiv_trace_ies_container_values,
    ie_table_deactiv_trace_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e1_release_request_ies_o[]   = {57, 0};
static const uint8_t  objset_crit_e1_release_request_ies_o[] = {crit_e::reject, crit_e::ignore};

// E1ReleaseRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e1_release_request_ies_container_values[] = {
    {offsetof(e1_release_request_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e1_release_request_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const int32_t ie_table_e1_release_request_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                               asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc e1_release_request_ies_container::desc = {
    objset_id_e1_release_request_ies_o,
    objset_crit_e1_release_request_ies_o,
    ie_table_e1_release_request_ies_container_values,
    ie_table_e1_release_request_ies_container_presences,
    2,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_e1_release_resp_ies_o[]   = {57, 1};
static const uint8_t  objset_crit_e1_release_resp_ies_o[] = {crit_e::reject, crit_e::ignore};

// E1ReleaseResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_e1_release_resp_ies_container_values[] = {
    {offsetof(e1_release_resp_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(e1_release_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(e1_release_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_e1_release_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(e1_release_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc e1_release_resp_ies_container::desc = {
    objset_id_e1_release_resp_ies_o,
    objset_crit_e1_release_resp_ies_o,
    ie_table_e1_release_resp_ies_container_values,
    ie_table_e1_release_resp_ies_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_eutran_gnb_cu_up_counter_check_request_o[]   = {61};
static const uint8_t  objset_crit_eutran_gnb_cu_up_counter_check_request_o[] = {crit_e::ignore};

static const uint32_t objset_id_early_forwarding_sn_transfer_ies_o[]   = {2, 3, 119};
static const uint8_t  objset_crit_early_forwarding_sn_transfer_ies_o[] = {crit_e::reject,
                                                                          crit_e::reject,
                                                                          crit_e::reject};

// EarlyForwardingSNTransferIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_early_forwarding_sn_transfer_ies_container_values[] = {
    {offsetof(early_forwarding_sn_transfer_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(early_forwarding_sn_transfer_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(early_forwarding_sn_transfer_ies_container, drbs_subject_to_early_forwarding_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<
         decltype(early_forwarding_sn_transfer_ies_container::drbs_subject_to_early_forwarding_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &drbs_subject_to_early_forwarding_item_s::desc}};
static const int32_t ie_table_early_forwarding_sn_transfer_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc early_forwarding_sn_transfer_ies_container::desc = {
    objset_id_early_forwarding_sn_transfer_ies_o,
    objset_crit_early_forwarding_sn_transfer_ies_o,
    ie_table_early_forwarding_sn_transfer_ies_container_values,
    ie_table_early_forwarding_sn_transfer_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_error_ind_ies_o[] = {57, 2, 3, 0, 1, 155, 156};
static const uint8_t  objset_crit_error_ind_ies_o[] =
    {crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// ErrorIndication-IEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_error_ind_ies_container_values[] = {
    {offsetof(error_ind_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(error_ind_ies_container, gnb_cu_cp_ue_e1ap_id),
     (int32_t)offsetof(error_ind_ies_container, gnb_cu_cp_ue_e1ap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(error_ind_ies_container, gnb_cu_up_ue_e1ap_id),
     (int32_t)offsetof(error_ind_ies_container, gnb_cu_up_ue_e1ap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
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
     &crit_diagnostics_s::desc},
    {offsetof(error_ind_ies_container, gnb_cu_cp_mbs_e1ap_id),
     (int32_t)offsetof(error_ind_ies_container, gnb_cu_cp_mbs_e1ap_id_present),
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(error_ind_ies_container, gnb_cu_up_mbs_e1ap_id),
     (int32_t)offsetof(error_ind_ies_container, gnb_cu_up_mbs_e1ap_id_present),
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr}};
static const int32_t ie_table_error_ind_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(error_ind_ies_container, gnb_cu_cp_ue_e1ap_id_present),
    (int32_t)offsetof(error_ind_ies_container, gnb_cu_up_ue_e1ap_id_present),
    (int32_t)offsetof(error_ind_ies_container, cause_present),
    (int32_t)offsetof(error_ind_ies_container, crit_diagnostics_present),
    (int32_t)offsetof(error_ind_ies_container, gnb_cu_cp_mbs_e1ap_id_present),
    (int32_t)offsetof(error_ind_ies_container, gnb_cu_up_mbs_e1ap_id_present)};
const asn1::table::ie_container_desc error_ind_ies_container::desc = {objset_id_error_ind_ies_o,
                                                                      objset_crit_error_ind_ies_o,
                                                                      ie_table_error_ind_ies_container_values,
                                                                      ie_table_error_ind_ies_container_presences,
                                                                      7,
                                                                      1,
                                                                      0,
                                                                      65535,
                                                                      true,
                                                                      "value"};

static const uint32_t objset_id_gnb_cu_cp_cfg_upd_ies_o[] = {57, 9, 27, 28, 29, 86, 129};
static const uint8_t  objset_crit_gnb_cu_cp_cfg_upd_ies_o[] =
    {crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// GNB-CU-CP-ConfigurationUpdateIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_gnb_cu_cp_cfg_upd_ies_container_values[] = {
    {offsetof(gnb_cu_cp_cfg_upd_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_cp_cfg_upd_ies_container, gnb_cu_cp_name),
     (int32_t)offsetof(gnb_cu_cp_cfg_upd_ies_container, gnb_cu_cp_name_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gnb_cu_cp_cfg_upd_ies_container::gnb_cu_cp_name),
                                asn1::table::string_json>::value},
    {offsetof(gnb_cu_cp_cfg_upd_ies_container, gnb_cu_cp_tnl_a_to_add_list),
     (int32_t)offsetof(gnb_cu_cp_cfg_upd_ies_container, gnb_cu_cp_tnl_a_to_add_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(gnb_cu_cp_cfg_upd_ies_container::gnb_cu_cp_tnl_a_to_add_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &gnb_cu_cp_tnl_a_to_add_item_s::desc},
    {offsetof(gnb_cu_cp_cfg_upd_ies_container, gnb_cu_cp_tnl_a_to_rem_list),
     (int32_t)offsetof(gnb_cu_cp_cfg_upd_ies_container, gnb_cu_cp_tnl_a_to_rem_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(gnb_cu_cp_cfg_upd_ies_container::gnb_cu_cp_tnl_a_to_rem_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &gnb_cu_cp_tnl_a_to_rem_item_s::desc},
    {offsetof(gnb_cu_cp_cfg_upd_ies_container, gnb_cu_cp_tnl_a_to_upd_list),
     (int32_t)offsetof(gnb_cu_cp_cfg_upd_ies_container, gnb_cu_cp_tnl_a_to_upd_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(gnb_cu_cp_cfg_upd_ies_container::gnb_cu_cp_tnl_a_to_upd_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &gnb_cu_cp_tnl_a_to_upd_item_s::desc},
    {offsetof(gnb_cu_cp_cfg_upd_ies_container, transport_layer_address_info),
     (int32_t)offsetof(gnb_cu_cp_cfg_upd_ies_container, transport_layer_address_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &transport_layer_address_info_s::desc},
    {offsetof(gnb_cu_cp_cfg_upd_ies_container, extended_gnb_cu_cp_name),
     (int32_t)offsetof(gnb_cu_cp_cfg_upd_ies_container, extended_gnb_cu_cp_name_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &extended_gnb_cu_cp_name_s::desc}};
static const int32_t ie_table_gnb_cu_cp_cfg_upd_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(gnb_cu_cp_cfg_upd_ies_container, gnb_cu_cp_name_present),
    (int32_t)offsetof(gnb_cu_cp_cfg_upd_ies_container, gnb_cu_cp_tnl_a_to_add_list_present),
    (int32_t)offsetof(gnb_cu_cp_cfg_upd_ies_container, gnb_cu_cp_tnl_a_to_rem_list_present),
    (int32_t)offsetof(gnb_cu_cp_cfg_upd_ies_container, gnb_cu_cp_tnl_a_to_upd_list_present),
    (int32_t)offsetof(gnb_cu_cp_cfg_upd_ies_container, transport_layer_address_info_present),
    (int32_t)offsetof(gnb_cu_cp_cfg_upd_ies_container, extended_gnb_cu_cp_name_present)};
const asn1::table::ie_container_desc gnb_cu_cp_cfg_upd_ies_container::desc = {
    objset_id_gnb_cu_cp_cfg_upd_ies_o,
    objset_crit_gnb_cu_cp_cfg_upd_ies_o,
    ie_table_gnb_cu_cp_cfg_upd_ies_container_values,
    ie_table_gnb_cu_cp_cfg_upd_ies_container_presences,
    7,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_gnb_cu_cp_cfg_upd_ack_ies_o[]   = {57, 1, 30, 31, 86};
static const uint8_t  objset_crit_gnb_cu_cp_cfg_upd_ack_ies_o[] = {crit_e::reject,
                                                                   crit_e::ignore,
                                                                   crit_e::ignore,
                                                                   crit_e::ignore,
                                                                   crit_e::ignore};

// GNB-CU-CP-ConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_gnb_cu_cp_cfg_upd_ack_ies_container_values[] = {
    {offsetof(gnb_cu_cp_cfg_upd_ack_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_cp_cfg_upd_ack_ies_container, crit_diagnostics),
     (int32_t)offsetof(gnb_cu_cp_cfg_upd_ack_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc},
    {offsetof(gnb_cu_cp_cfg_upd_ack_ies_container, gnb_cu_cp_tnl_a_setup_list),
     (int32_t)offsetof(gnb_cu_cp_cfg_upd_ack_ies_container, gnb_cu_cp_tnl_a_setup_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(gnb_cu_cp_cfg_upd_ack_ies_container::gnb_cu_cp_tnl_a_setup_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &gnb_cu_cp_tnl_a_setup_item_s::desc},
    {offsetof(gnb_cu_cp_cfg_upd_ack_ies_container, gnb_cu_cp_tnl_a_failed_to_setup_list),
     (int32_t)offsetof(gnb_cu_cp_cfg_upd_ack_ies_container, gnb_cu_cp_tnl_a_failed_to_setup_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(gnb_cu_cp_cfg_upd_ack_ies_container::gnb_cu_cp_tnl_a_failed_to_setup_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &gnb_cu_cp_tnl_a_failed_to_setup_item_s::desc},
    {offsetof(gnb_cu_cp_cfg_upd_ack_ies_container, transport_layer_address_info),
     (int32_t)offsetof(gnb_cu_cp_cfg_upd_ack_ies_container, transport_layer_address_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &transport_layer_address_info_s::desc}};
static const int32_t ie_table_gnb_cu_cp_cfg_upd_ack_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(gnb_cu_cp_cfg_upd_ack_ies_container, crit_diagnostics_present),
    (int32_t)offsetof(gnb_cu_cp_cfg_upd_ack_ies_container, gnb_cu_cp_tnl_a_setup_list_present),
    (int32_t)offsetof(gnb_cu_cp_cfg_upd_ack_ies_container, gnb_cu_cp_tnl_a_failed_to_setup_list_present),
    (int32_t)offsetof(gnb_cu_cp_cfg_upd_ack_ies_container, transport_layer_address_info_present)};
const asn1::table::ie_container_desc gnb_cu_cp_cfg_upd_ack_ies_container::desc = {
    objset_id_gnb_cu_cp_cfg_upd_ack_ies_o,
    objset_crit_gnb_cu_cp_cfg_upd_ack_ies_o,
    ie_table_gnb_cu_cp_cfg_upd_ack_ies_container_values,
    ie_table_gnb_cu_cp_cfg_upd_ack_ies_container_presences,
    5,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_gnb_cu_cp_cfg_upd_fail_ies_o[]   = {57, 0, 12, 1};
static const uint8_t  objset_crit_gnb_cu_cp_cfg_upd_fail_ies_o[] = {crit_e::reject,
                                                                    crit_e::ignore,
                                                                    crit_e::ignore,
                                                                    crit_e::ignore};

// GNB-CU-CP-ConfigurationUpdateFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_gnb_cu_cp_cfg_upd_fail_ies_container_values[] = {
    {offsetof(gnb_cu_cp_cfg_upd_fail_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_cp_cfg_upd_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(gnb_cu_cp_cfg_upd_fail_ies_container, time_to_wait),
     (int32_t)offsetof(gnb_cu_cp_cfg_upd_fail_ies_container, time_to_wait_present),
     asn1::table::enum_op<decltype(gnb_cu_cp_cfg_upd_fail_ies_container::time_to_wait)>(),
     decltype(gnb_cu_cp_cfg_upd_fail_ies_container::time_to_wait)::has_ext,
     false,
     decltype(gnb_cu_cp_cfg_upd_fail_ies_container::time_to_wait)::nof_types,
     decltype(gnb_cu_cp_cfg_upd_fail_ies_container::time_to_wait)::nof_exts,
     nullptr,
     &time_to_wait_opts::desc},
    {offsetof(gnb_cu_cp_cfg_upd_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(gnb_cu_cp_cfg_upd_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_gnb_cu_cp_cfg_upd_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(gnb_cu_cp_cfg_upd_fail_ies_container, time_to_wait_present),
    (int32_t)offsetof(gnb_cu_cp_cfg_upd_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc gnb_cu_cp_cfg_upd_fail_ies_container::desc = {
    objset_id_gnb_cu_cp_cfg_upd_fail_ies_o,
    objset_crit_gnb_cu_cp_cfg_upd_fail_ies_o,
    ie_table_gnb_cu_cp_cfg_upd_fail_ies_container_values,
    ie_table_gnb_cu_cp_cfg_upd_fail_ies_container_presences,
    4,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_gnb_cu_cp_e1_setup_fail_ies_o[]   = {57, 0, 12, 1};
static const uint8_t  objset_crit_gnb_cu_cp_e1_setup_fail_ies_o[] = {crit_e::reject,
                                                                     crit_e::ignore,
                                                                     crit_e::ignore,
                                                                     crit_e::ignore};

// GNB-CU-CP-E1SetupFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_gnb_cu_cp_e1_setup_fail_ies_container_values[] = {
    {offsetof(gnb_cu_cp_e1_setup_fail_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_cp_e1_setup_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(gnb_cu_cp_e1_setup_fail_ies_container, time_to_wait),
     (int32_t)offsetof(gnb_cu_cp_e1_setup_fail_ies_container, time_to_wait_present),
     asn1::table::enum_op<decltype(gnb_cu_cp_e1_setup_fail_ies_container::time_to_wait)>(),
     decltype(gnb_cu_cp_e1_setup_fail_ies_container::time_to_wait)::has_ext,
     false,
     decltype(gnb_cu_cp_e1_setup_fail_ies_container::time_to_wait)::nof_types,
     decltype(gnb_cu_cp_e1_setup_fail_ies_container::time_to_wait)::nof_exts,
     nullptr,
     &time_to_wait_opts::desc},
    {offsetof(gnb_cu_cp_e1_setup_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(gnb_cu_cp_e1_setup_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_gnb_cu_cp_e1_setup_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(gnb_cu_cp_e1_setup_fail_ies_container, time_to_wait_present),
    (int32_t)offsetof(gnb_cu_cp_e1_setup_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc gnb_cu_cp_e1_setup_fail_ies_container::desc = {
    objset_id_gnb_cu_cp_e1_setup_fail_ies_o,
    objset_crit_gnb_cu_cp_e1_setup_fail_ies_o,
    ie_table_gnb_cu_cp_e1_setup_fail_ies_container_values,
    ie_table_gnb_cu_cp_e1_setup_fail_ies_container_presences,
    4,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_gnb_cu_cp_e1_setup_request_ies_o[]   = {57, 9, 86, 129};
static const uint8_t  objset_crit_gnb_cu_cp_e1_setup_request_ies_o[] = {crit_e::reject,
                                                                        crit_e::ignore,
                                                                        crit_e::ignore,
                                                                        crit_e::ignore};

// GNB-CU-CP-E1SetupRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_gnb_cu_cp_e1_setup_request_ies_container_values[] = {
    {offsetof(gnb_cu_cp_e1_setup_request_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_cp_e1_setup_request_ies_container, gnb_cu_cp_name),
     (int32_t)offsetof(gnb_cu_cp_e1_setup_request_ies_container, gnb_cu_cp_name_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gnb_cu_cp_e1_setup_request_ies_container::gnb_cu_cp_name),
                                asn1::table::string_json>::value},
    {offsetof(gnb_cu_cp_e1_setup_request_ies_container, transport_layer_address_info),
     (int32_t)offsetof(gnb_cu_cp_e1_setup_request_ies_container, transport_layer_address_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &transport_layer_address_info_s::desc},
    {offsetof(gnb_cu_cp_e1_setup_request_ies_container, extended_gnb_cu_cp_name),
     (int32_t)offsetof(gnb_cu_cp_e1_setup_request_ies_container, extended_gnb_cu_cp_name_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &extended_gnb_cu_cp_name_s::desc}};
static const int32_t ie_table_gnb_cu_cp_e1_setup_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(gnb_cu_cp_e1_setup_request_ies_container, gnb_cu_cp_name_present),
    (int32_t)offsetof(gnb_cu_cp_e1_setup_request_ies_container, transport_layer_address_info_present),
    (int32_t)offsetof(gnb_cu_cp_e1_setup_request_ies_container, extended_gnb_cu_cp_name_present)};
const asn1::table::ie_container_desc gnb_cu_cp_e1_setup_request_ies_container::desc = {
    objset_id_gnb_cu_cp_e1_setup_request_ies_o,
    objset_crit_gnb_cu_cp_e1_setup_request_ies_o,
    ie_table_gnb_cu_cp_e1_setup_request_ies_container_values,
    ie_table_gnb_cu_cp_e1_setup_request_ies_container_presences,
    4,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_supported_plmns_ext_ies_o[]   = {110, 125, 135, 145};
static const uint8_t  objset_crit_supported_plmns_ext_ies_o[] = {crit_e::reject,
                                                                 crit_e::reject,
                                                                 crit_e::ignore,
                                                                 crit_e::ignore};

// SupportedPLMNs-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_supported_plmns_ext_ies_container_values[] = {
    {offsetof(supported_plmns_ext_ies_container, npn_support_info),
     (int32_t)offsetof(supported_plmns_ext_ies_container, npn_support_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &npn_support_info_c::desc},
    {offsetof(supported_plmns_ext_ies_container, extended_slice_support_list),
     (int32_t)offsetof(supported_plmns_ext_ies_container, extended_slice_support_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(supported_plmns_ext_ies_container::extended_slice_support_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &slice_support_item_s::desc},
    {offsetof(supported_plmns_ext_ies_container, extended_nr_cgi_support_list),
     (int32_t)offsetof(supported_plmns_ext_ies_container, extended_nr_cgi_support_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16383,
     &asn1::table::item_list_codec<decltype(supported_plmns_ext_ies_container::extended_nr_cgi_support_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &extended_nr_cgi_support_item_s::desc},
    {offsetof(supported_plmns_ext_ies_container, ecgi_support_list),
     (int32_t)offsetof(supported_plmns_ext_ies_container, ecgi_support_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     511,
     &asn1::table::item_list_codec<decltype(supported_plmns_ext_ies_container::ecgi_support_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ecgi_support_item_s::desc}};
static const int32_t ie_table_supported_plmns_ext_ies_container_presences[] = {
    (int32_t)offsetof(supported_plmns_ext_ies_container, npn_support_info_present),
    (int32_t)offsetof(supported_plmns_ext_ies_container, extended_slice_support_list_present),
    (int32_t)offsetof(supported_plmns_ext_ies_container, extended_nr_cgi_support_list_present),
    (int32_t)offsetof(supported_plmns_ext_ies_container, ecgi_support_list_present)};
const asn1::table::ie_container_desc supported_plmns_ext_ies_container::desc = {
    objset_id_supported_plmns_ext_ies_o,
    objset_crit_supported_plmns_ext_ies_o,
    ie_table_supported_plmns_ext_ies_container_values,
    ie_table_supported_plmns_ext_ies_container_presences,
    4,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// SupportedPLMNs-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_supported_plmns_item_s_fields[] = {
    {offsetof(supported_plmns_item_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(supported_plmns_item_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(supported_plmns_item_s, slice_support_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     1023,
     &asn1::table::item_list_codec<decltype(supported_plmns_item_s::slice_support_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &slice_support_item_s::desc},
    {offsetof(supported_plmns_item_s, nr_cgi_support_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     511,
     &asn1::table::item_list_codec<decltype(supported_plmns_item_s::nr_cgi_support_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_cgi_support_item_s::desc},
    {offsetof(supported_plmns_item_s, qos_params_support_list),
     (int32_t)offsetof(supported_plmns_item_s, qos_params_support_list_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &qos_params_support_list_s::desc},
    {offsetof(supported_plmns_item_s, ie_exts),
     (int32_t)offsetof(supported_plmns_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &supported_plmns_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_supported_plmns_item_s_names[] = {
    {"pLMN-Identity", nullptr},
    {"slice-Support-List", nullptr},
    {"nR-CGI-Support-List", nullptr},
    {"qoS-Parameters-Support-List", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_supported_plmns_item_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(supported_plmns_item_s, qos_params_support_list_present)},
    {4, (int32_t)offsetof(supported_plmns_item_s, ie_exts_present)}};
const asn1::table::seq_desc supported_plmns_item_s::desc = {{seq_table_supported_plmns_item_s_fields,
                                                             uint64_t{0x1},
                                                             seq_table_supported_plmns_item_s_opts,
                                                             seq_table_supported_plmns_item_s_names,
                                                             5,
                                                             4},
                                                            true,
                                                            true,
                                                            nullptr,
                                                            0};

static const uint32_t objset_id_gnb_cu_cp_e1_setup_resp_ies_o[]   = {57, 7, 8, 10, 11, 64, 86, 130, 1};
static const uint8_t  objset_crit_gnb_cu_cp_e1_setup_resp_ies_o[] = {crit_e::reject,
                                                                     crit_e::reject,
                                                                     crit_e::ignore,
                                                                     crit_e::reject,
                                                                     crit_e::reject,
                                                                     crit_e::ignore,
                                                                     crit_e::ignore,
                                                                     crit_e::ignore,
                                                                     crit_e::ignore};

// GNB-CU-CP-E1SetupResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_gnb_cu_cp_e1_setup_resp_ies_container_values[] = {
    {offsetof(gnb_cu_cp_e1_setup_resp_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_cp_e1_setup_resp_ies_container, gnb_cu_up_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr},
    {offsetof(gnb_cu_cp_e1_setup_resp_ies_container, gnb_cu_up_name),
     (int32_t)offsetof(gnb_cu_cp_e1_setup_resp_ies_container, gnb_cu_up_name_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gnb_cu_cp_e1_setup_resp_ies_container::gnb_cu_up_name),
                                asn1::table::string_json>::value},
    {offsetof(gnb_cu_cp_e1_setup_resp_ies_container, cn_support),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(gnb_cu_cp_e1_setup_resp_ies_container::cn_support)>(),
     decltype(gnb_cu_cp_e1_setup_resp_ies_container::cn_support)::has_ext,
     false,
     decltype(gnb_cu_cp_e1_setup_resp_ies_container::cn_support)::nof_types,
     decltype(gnb_cu_cp_e1_setup_resp_ies_container::cn_support)::nof_exts,
     nullptr,
     &cn_support_opts::desc},
    {offsetof(gnb_cu_cp_e1_setup_resp_ies_container, supported_plmns),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     11,
     &asn1::table::item_list_codec<decltype(gnb_cu_cp_e1_setup_resp_ies_container::supported_plmns),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &supported_plmns_item_s::desc},
    {offsetof(gnb_cu_cp_e1_setup_resp_ies_container, gnb_cu_up_capacity),
     (int32_t)offsetof(gnb_cu_cp_e1_setup_resp_ies_container, gnb_cu_up_capacity_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_cp_e1_setup_resp_ies_container, transport_layer_address_info),
     (int32_t)offsetof(gnb_cu_cp_e1_setup_resp_ies_container, transport_layer_address_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &transport_layer_address_info_s::desc},
    {offsetof(gnb_cu_cp_e1_setup_resp_ies_container, extended_gnb_cu_up_name),
     (int32_t)offsetof(gnb_cu_cp_e1_setup_resp_ies_container, extended_gnb_cu_up_name_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &extended_gnb_cu_up_name_s::desc},
    {offsetof(gnb_cu_cp_e1_setup_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(gnb_cu_cp_e1_setup_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_gnb_cu_cp_e1_setup_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(gnb_cu_cp_e1_setup_resp_ies_container, gnb_cu_up_name_present),
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(gnb_cu_cp_e1_setup_resp_ies_container, gnb_cu_up_capacity_present),
    (int32_t)offsetof(gnb_cu_cp_e1_setup_resp_ies_container, transport_layer_address_info_present),
    (int32_t)offsetof(gnb_cu_cp_e1_setup_resp_ies_container, extended_gnb_cu_up_name_present),
    (int32_t)offsetof(gnb_cu_cp_e1_setup_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc gnb_cu_cp_e1_setup_resp_ies_container::desc = {
    objset_id_gnb_cu_cp_e1_setup_resp_ies_o,
    objset_crit_gnb_cu_cp_e1_setup_resp_ies_o,
    ie_table_gnb_cu_cp_e1_setup_resp_ies_container_values,
    ie_table_gnb_cu_cp_e1_setup_resp_ies_container_presences,
    9,
    4,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_gnb_cu_cp_meas_results_info_ies_o[]  = {2, 3, 128};
static const uint8_t objset_crit_gnb_cu_cp_meas_results_info_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::ignore};

// GNB-CU-CPMeasurementResultsInformationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_gnb_cu_cp_meas_results_info_ies_container_values[] = {
    {offsetof(gnb_cu_cp_meas_results_info_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(gnb_cu_cp_meas_results_info_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(gnb_cu_cp_meas_results_info_ies_container, drb_meas_results_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(gnb_cu_cp_meas_results_info_ies_container::drb_meas_results_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_meas_results_info_item_s::desc}};
static const int32_t ie_table_gnb_cu_cp_meas_results_info_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc gnb_cu_cp_meas_results_info_ies_container::desc = {
    objset_id_gnb_cu_cp_meas_results_info_ies_o,
    objset_crit_gnb_cu_cp_meas_results_info_ies_o,
    ie_table_gnb_cu_cp_meas_results_info_ies_container_values,
    ie_table_gnb_cu_cp_meas_results_info_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_gnb_cu_up_cfg_upd_ies_o[]   = {57, 7, 8, 11, 64, 73, 86, 130, 171};
static const uint8_t  objset_crit_gnb_cu_up_cfg_upd_ies_o[] = {crit_e::reject,
                                                               crit_e::reject,
                                                               crit_e::ignore,
                                                               crit_e::reject,
                                                               crit_e::ignore,
                                                               crit_e::reject,
                                                               crit_e::ignore,
                                                               crit_e::ignore,
                                                               crit_e::reject};

// GNB-CU-UP-ConfigurationUpdateIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_gnb_cu_up_cfg_upd_ies_container_values[] = {
    {offsetof(gnb_cu_up_cfg_upd_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_up_cfg_upd_ies_container, gnb_cu_up_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr},
    {offsetof(gnb_cu_up_cfg_upd_ies_container, gnb_cu_up_name),
     (int32_t)offsetof(gnb_cu_up_cfg_upd_ies_container, gnb_cu_up_name_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gnb_cu_up_cfg_upd_ies_container::gnb_cu_up_name),
                                asn1::table::string_json>::value},
    {offsetof(gnb_cu_up_cfg_upd_ies_container, supported_plmns),
     (int32_t)offsetof(gnb_cu_up_cfg_upd_ies_container, supported_plmns_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     11,
     &asn1::table::item_list_codec<decltype(gnb_cu_up_cfg_upd_ies_container::supported_plmns),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &supported_plmns_item_s::desc},
    {offsetof(gnb_cu_up_cfg_upd_ies_container, gnb_cu_up_capacity),
     (int32_t)offsetof(gnb_cu_up_cfg_upd_ies_container, gnb_cu_up_capacity_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_up_cfg_upd_ies_container, gnb_cu_up_tnl_a_to_rem_list),
     (int32_t)offsetof(gnb_cu_up_cfg_upd_ies_container, gnb_cu_up_tnl_a_to_rem_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<decltype(gnb_cu_up_cfg_upd_ies_container::gnb_cu_up_tnl_a_to_rem_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &gnb_cu_up_tnl_a_to_rem_item_s::desc},
    {offsetof(gnb_cu_up_cfg_upd_ies_container, transport_layer_address_info),
     (int32_t)offsetof(gnb_cu_up_cfg_upd_ies_container, transport_layer_address_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &transport_layer_address_info_s::desc},
    {offsetof(gnb_cu_up_cfg_upd_ies_container, extended_gnb_cu_up_name),
     (int32_t)offsetof(gnb_cu_up_cfg_upd_ies_container, extended_gnb_cu_up_name_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &extended_gnb_cu_up_name_s::desc},
    {offsetof(gnb_cu_up_cfg_upd_ies_container, gnb_cu_up_mbs_support_info),
     (int32_t)offsetof(gnb_cu_up_cfg_upd_ies_container, gnb_cu_up_mbs_support_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gnb_cu_up_mbs_support_info_s::desc}};
static const int32_t ie_table_gnb_cu_up_cfg_upd_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(gnb_cu_up_cfg_upd_ies_container, gnb_cu_up_name_present),
    (int32_t)offsetof(gnb_cu_up_cfg_upd_ies_container, supported_plmns_present),
    (int32_t)offsetof(gnb_cu_up_cfg_upd_ies_container, gnb_cu_up_capacity_present),
    (int32_t)offsetof(gnb_cu_up_cfg_upd_ies_container, gnb_cu_up_tnl_a_to_rem_list_present),
    (int32_t)offsetof(gnb_cu_up_cfg_upd_ies_container, transport_layer_address_info_present),
    (int32_t)offsetof(gnb_cu_up_cfg_upd_ies_container, extended_gnb_cu_up_name_present),
    (int32_t)offsetof(gnb_cu_up_cfg_upd_ies_container, gnb_cu_up_mbs_support_info_present)};
const asn1::table::ie_container_desc gnb_cu_up_cfg_upd_ies_container::desc = {
    objset_id_gnb_cu_up_cfg_upd_ies_o,
    objset_crit_gnb_cu_up_cfg_upd_ies_o,
    ie_table_gnb_cu_up_cfg_upd_ies_container_values,
    ie_table_gnb_cu_up_cfg_upd_ies_container_presences,
    9,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_gnb_cu_up_cfg_upd_ack_ies_o[]   = {57, 1, 86};
static const uint8_t  objset_crit_gnb_cu_up_cfg_upd_ack_ies_o[] = {crit_e::reject, crit_e::ignore, crit_e::ignore};

// GNB-CU-UP-ConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_gnb_cu_up_cfg_upd_ack_ies_container_values[] = {
    {offsetof(gnb_cu_up_cfg_upd_ack_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_up_cfg_upd_ack_ies_container, crit_diagnostics),
     (int32_t)offsetof(gnb_cu_up_cfg_upd_ack_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc},
    {offsetof(gnb_cu_up_cfg_upd_ack_ies_container, transport_layer_address_info),
     (int32_t)offsetof(gnb_cu_up_cfg_upd_ack_ies_container, transport_layer_address_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &transport_layer_address_info_s::desc}};
static const int32_t ie_table_gnb_cu_up_cfg_upd_ack_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(gnb_cu_up_cfg_upd_ack_ies_container, crit_diagnostics_present),
    (int32_t)offsetof(gnb_cu_up_cfg_upd_ack_ies_container, transport_layer_address_info_present)};
const asn1::table::ie_container_desc gnb_cu_up_cfg_upd_ack_ies_container::desc = {
    objset_id_gnb_cu_up_cfg_upd_ack_ies_o,
    objset_crit_gnb_cu_up_cfg_upd_ack_ies_o,
    ie_table_gnb_cu_up_cfg_upd_ack_ies_container_values,
    ie_table_gnb_cu_up_cfg_upd_ack_ies_container_presences,
    3,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_gnb_cu_up_cfg_upd_fail_ies_o[]   = {57, 0, 12, 1};
static const uint8_t  objset_crit_gnb_cu_up_cfg_upd_fail_ies_o[] = {crit_e::reject,
                                                                    crit_e::ignore,
                                                                    crit_e::ignore,
                                                                    crit_e::ignore};

// GNB-CU-UP-ConfigurationUpdateFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_gnb_cu_up_cfg_upd_fail_ies_container_values[] = {
    {offsetof(gnb_cu_up_cfg_upd_fail_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_up_cfg_upd_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(gnb_cu_up_cfg_upd_fail_ies_container, time_to_wait),
     (int32_t)offsetof(gnb_cu_up_cfg_upd_fail_ies_container, time_to_wait_present),
     asn1::table::enum_op<decltype(gnb_cu_up_cfg_upd_fail_ies_container::time_to_wait)>(),
     decltype(gnb_cu_up_cfg_upd_fail_ies_container::time_to_wait)::has_ext,
     false,
     decltype(gnb_cu_up_cfg_upd_fail_ies_container::time_to_wait)::nof_types,
     decltype(gnb_cu_up_cfg_upd_fail_ies_container::time_to_wait)::nof_exts,
     nullptr,
     &time_to_wait_opts::desc},
    {offsetof(gnb_cu_up_cfg_upd_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(gnb_cu_up_cfg_upd_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_gnb_cu_up_cfg_upd_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(gnb_cu_up_cfg_upd_fail_ies_container, time_to_wait_present),
    (int32_t)offsetof(gnb_cu_up_cfg_upd_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc gnb_cu_up_cfg_upd_fail_ies_container::desc = {
    objset_id_gnb_cu_up_cfg_upd_fail_ies_o,
    objset_crit_gnb_cu_up_cfg_upd_fail_ies_o,
    ie_table_gnb_cu_up_cfg_upd_fail_ies_container_values,
    ie_table_gnb_cu_up_cfg_upd_fail_ies_container_presences,
    4,
    2,
    0,
    65535,
    true,
    "value"};

// EUTRAN-GNB-CU-UP-CounterCheckRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_eutran_gnb_cu_up_counter_check_request_container_values[] = {
    {offsetof(eutran_gnb_cu_up_counter_check_request_container, drbs_subject_to_counter_check_list_eutran),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<
         decltype(eutran_gnb_cu_up_counter_check_request_container::drbs_subject_to_counter_check_list_eutran),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &drbs_subject_to_counter_check_item_eutran_s::desc}};
static const int32_t ie_table_eutran_gnb_cu_up_counter_check_request_container_presences[] = {
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc eutran_gnb_cu_up_counter_check_request_container::desc = {
    objset_id_eutran_gnb_cu_up_counter_check_request_o,
    objset_crit_eutran_gnb_cu_up_counter_check_request_o,
    ie_table_eutran_gnb_cu_up_counter_check_request_container_values,
    ie_table_eutran_gnb_cu_up_counter_check_request_container_presences,
    1,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_ng_ran_gnb_cu_up_counter_check_request_o[]   = {62};
static const uint8_t  objset_crit_ng_ran_gnb_cu_up_counter_check_request_o[] = {crit_e::ignore};

// NG-RAN-GNB-CU-UP-CounterCheckRequest ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ng_ran_gnb_cu_up_counter_check_request_container_values[] = {
    {offsetof(ng_ran_gnb_cu_up_counter_check_request_container, drbs_subject_to_counter_check_list_ng_ran),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     31,
     &asn1::table::item_list_codec<
         decltype(ng_ran_gnb_cu_up_counter_check_request_container::drbs_subject_to_counter_check_list_ng_ran),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &drbs_subject_to_counter_check_item_ng_ran_s::desc}};
static const int32_t ie_table_ng_ran_gnb_cu_up_counter_check_request_container_presences[] = {
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc ng_ran_gnb_cu_up_counter_check_request_container::desc = {
    objset_id_ng_ran_gnb_cu_up_counter_check_request_o,
    objset_crit_ng_ran_gnb_cu_up_counter_check_request_o,
    ie_table_ng_ran_gnb_cu_up_counter_check_request_container_values,
    ie_table_ng_ran_gnb_cu_up_counter_check_request_container_presences,
    1,
    1,
    0,
    65535,
    true,
    "value"};

// System-GNB-CU-UP-CounterCheckRequest ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_sys_gnb_cu_up_counter_check_request_c_alternatives[] = {
    &asn1::table::alternative_ops<eutran_gnb_cu_up_counter_check_request_container, false>::value,
    &asn1::table::alternative_ops<ng_ran_gnb_cu_up_counter_check_request_container, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>,
                                  false>::value};
const asn1::table::choice_storage_desc sys_gnb_cu_up_counter_check_request_c::storage_desc = {
    choice_storage_sys_gnb_cu_up_counter_check_request_c_alternatives,
    3,
    "sys_gnb_cu_up_counter_check_request_c"};

eutran_gnb_cu_up_counter_check_request_container&
sys_gnb_cu_up_counter_check_request_c::set_e_utran_gnb_cu_up_counter_check_request()
{
  set(types::e_utran_gnb_cu_up_counter_check_request);
  return c.get<eutran_gnb_cu_up_counter_check_request_container>();
}
ng_ran_gnb_cu_up_counter_check_request_container&
sys_gnb_cu_up_counter_check_request_c::set_ng_ran_gnb_cu_up_counter_check_request()
{
  set(types::ng_ran_gnb_cu_up_counter_check_request);
  return c.get<ng_ran_gnb_cu_up_counter_check_request_container>();
}
protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>&
sys_gnb_cu_up_counter_check_request_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_sys_gnb_cu_up_counter_check_request_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &eutran_gnb_cu_up_counter_check_request_container::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &ng_ran_gnb_cu_up_counter_check_request_container::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<sys_gnb_cu_up_counter_check_request_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_sys_gnb_cu_up_counter_check_request_c_names[] = {
    {"e-UTRAN-GNB-CU-UP-CounterCheckRequest", nullptr},
    {"nG-RAN-GNB-CU-UP-CounterCheckRequest", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc sys_gnb_cu_up_counter_check_request_c::desc = {
    asn1::table::enum_int_type<sys_gnb_cu_up_counter_check_request_c::types>(),
    sys_gnb_cu_up_counter_check_request_c::types::nof_types,
    sys_gnb_cu_up_counter_check_request_c::types::nof_exts,
    sys_gnb_cu_up_counter_check_request_c::types::has_ext,
    true,
    choice_table_sys_gnb_cu_up_counter_check_request_c_alternatives,
    nullptr,
    choice_table_sys_gnb_cu_up_counter_check_request_c_names,
    "sys_gnb_cu_up_counter_check_request_c",
    offsetof(sys_gnb_cu_up_counter_check_request_c, c.type),
    offsetof(sys_gnb_cu_up_counter_check_request_c, c.value),
    false,
    &sys_gnb_cu_up_counter_check_request_c::storage_desc};

static const char* enum_names_sys_gnb_cu_up_counter_check_request_c_types_values[] = {
    "e-UTRAN-GNB-CU-UP-CounterCheckRequest",
    "nG-RAN-GNB-CU-UP-CounterCheckRequest",
    "choice-extension"};
const asn1::table::enum_desc sys_gnb_cu_up_counter_check_request_c::types_opts::desc = {
    enum_names_sys_gnb_cu_up_counter_check_request_c_types_values,
    3,
    "sys_gnb_cu_up_counter_check_request_c::types"};

static const uint32_t objset_id_gnb_cu_up_counter_check_request_ies_o[]   = {2, 3, 60};
static const uint8_t  objset_crit_gnb_cu_up_counter_check_request_ies_o[] = {crit_e::reject,
                                                                             crit_e::reject,
                                                                             crit_e::reject};

// GNB-CU-UP-CounterCheckRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_gnb_cu_up_counter_check_request_ies_container_values[] = {
    {offsetof(gnb_cu_up_counter_check_request_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(gnb_cu_up_counter_check_request_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(gnb_cu_up_counter_check_request_ies_container, sys_gnb_cu_up_counter_check_request),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sys_gnb_cu_up_counter_check_request_c::desc}};
static const int32_t ie_table_gnb_cu_up_counter_check_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc gnb_cu_up_counter_check_request_ies_container::desc = {
    objset_id_gnb_cu_up_counter_check_request_ies_o,
    objset_crit_gnb_cu_up_counter_check_request_ies_o,
    ie_table_gnb_cu_up_counter_check_request_ies_container_values,
    ie_table_gnb_cu_up_counter_check_request_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_gnb_cu_up_e1_setup_fail_ies_o[]   = {57, 0, 12, 1};
static const uint8_t  objset_crit_gnb_cu_up_e1_setup_fail_ies_o[] = {crit_e::reject,
                                                                     crit_e::ignore,
                                                                     crit_e::ignore,
                                                                     crit_e::ignore};

// GNB-CU-UP-E1SetupFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_gnb_cu_up_e1_setup_fail_ies_container_values[] = {
    {offsetof(gnb_cu_up_e1_setup_fail_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_up_e1_setup_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(gnb_cu_up_e1_setup_fail_ies_container, time_to_wait),
     (int32_t)offsetof(gnb_cu_up_e1_setup_fail_ies_container, time_to_wait_present),
     asn1::table::enum_op<decltype(gnb_cu_up_e1_setup_fail_ies_container::time_to_wait)>(),
     decltype(gnb_cu_up_e1_setup_fail_ies_container::time_to_wait)::has_ext,
     false,
     decltype(gnb_cu_up_e1_setup_fail_ies_container::time_to_wait)::nof_types,
     decltype(gnb_cu_up_e1_setup_fail_ies_container::time_to_wait)::nof_exts,
     nullptr,
     &time_to_wait_opts::desc},
    {offsetof(gnb_cu_up_e1_setup_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(gnb_cu_up_e1_setup_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_gnb_cu_up_e1_setup_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(gnb_cu_up_e1_setup_fail_ies_container, time_to_wait_present),
    (int32_t)offsetof(gnb_cu_up_e1_setup_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc gnb_cu_up_e1_setup_fail_ies_container::desc = {
    objset_id_gnb_cu_up_e1_setup_fail_ies_o,
    objset_crit_gnb_cu_up_e1_setup_fail_ies_o,
    ie_table_gnb_cu_up_e1_setup_fail_ies_container_values,
    ie_table_gnb_cu_up_e1_setup_fail_ies_container_presences,
    4,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_gnb_cu_up_e1_setup_request_ies_o[]   = {57, 7, 8, 10, 11, 64, 86, 130, 171};
static const uint8_t  objset_crit_gnb_cu_up_e1_setup_request_ies_o[] = {crit_e::reject,
                                                                        crit_e::reject,
                                                                        crit_e::ignore,
                                                                        crit_e::reject,
                                                                        crit_e::reject,
                                                                        crit_e::ignore,
                                                                        crit_e::ignore,
                                                                        crit_e::ignore,
                                                                        crit_e::reject};

// GNB-CU-UP-E1SetupRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_gnb_cu_up_e1_setup_request_ies_container_values[] = {
    {offsetof(gnb_cu_up_e1_setup_request_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_up_e1_setup_request_ies_container, gnb_cu_up_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr},
    {offsetof(gnb_cu_up_e1_setup_request_ies_container, gnb_cu_up_name),
     (int32_t)offsetof(gnb_cu_up_e1_setup_request_ies_container, gnb_cu_up_name_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gnb_cu_up_e1_setup_request_ies_container::gnb_cu_up_name),
                                asn1::table::string_json>::value},
    {offsetof(gnb_cu_up_e1_setup_request_ies_container, cn_support),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(gnb_cu_up_e1_setup_request_ies_container::cn_support)>(),
     decltype(gnb_cu_up_e1_setup_request_ies_container::cn_support)::has_ext,
     false,
     decltype(gnb_cu_up_e1_setup_request_ies_container::cn_support)::nof_types,
     decltype(gnb_cu_up_e1_setup_request_ies_container::cn_support)::nof_exts,
     nullptr,
     &cn_support_opts::desc},
    {offsetof(gnb_cu_up_e1_setup_request_ies_container, supported_plmns),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     11,
     &asn1::table::item_list_codec<decltype(gnb_cu_up_e1_setup_request_ies_container::supported_plmns),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &supported_plmns_item_s::desc},
    {offsetof(gnb_cu_up_e1_setup_request_ies_container, gnb_cu_up_capacity),
     (int32_t)offsetof(gnb_cu_up_e1_setup_request_ies_container, gnb_cu_up_capacity_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_up_e1_setup_request_ies_container, transport_layer_address_info),
     (int32_t)offsetof(gnb_cu_up_e1_setup_request_ies_container, transport_layer_address_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &transport_layer_address_info_s::desc},
    {offsetof(gnb_cu_up_e1_setup_request_ies_container, extended_gnb_cu_up_name),
     (int32_t)offsetof(gnb_cu_up_e1_setup_request_ies_container, extended_gnb_cu_up_name_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &extended_gnb_cu_up_name_s::desc},
    {offsetof(gnb_cu_up_e1_setup_request_ies_container, gnb_cu_up_mbs_support_info),
     (int32_t)offsetof(gnb_cu_up_e1_setup_request_ies_container, gnb_cu_up_mbs_support_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gnb_cu_up_mbs_support_info_s::desc}};
static const int32_t ie_table_gnb_cu_up_e1_setup_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(gnb_cu_up_e1_setup_request_ies_container, gnb_cu_up_name_present),
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(gnb_cu_up_e1_setup_request_ies_container, gnb_cu_up_capacity_present),
    (int32_t)offsetof(gnb_cu_up_e1_setup_request_ies_container, transport_layer_address_info_present),
    (int32_t)offsetof(gnb_cu_up_e1_setup_request_ies_container, extended_gnb_cu_up_name_present),
    (int32_t)offsetof(gnb_cu_up_e1_setup_request_ies_container, gnb_cu_up_mbs_support_info_present)};
const asn1::table::ie_container_desc gnb_cu_up_e1_setup_request_ies_container::desc = {
    objset_id_gnb_cu_up_e1_setup_request_ies_o,
    objset_crit_gnb_cu_up_e1_setup_request_ies_o,
    ie_table_gnb_cu_up_e1_setup_request_ies_container_values,
    ie_table_gnb_cu_up_e1_setup_request_ies_container_presences,
    9,
    4,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_gnb_cu_up_e1_setup_resp_ies_o[]   = {57, 9, 86, 129, 1};
static const uint8_t  objset_crit_gnb_cu_up_e1_setup_resp_ies_o[] = {crit_e::reject,
                                                                     crit_e::ignore,
                                                                     crit_e::ignore,
                                                                     crit_e::ignore,
                                                                     crit_e::ignore};

// GNB-CU-UP-E1SetupResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_gnb_cu_up_e1_setup_resp_ies_container_values[] = {
    {offsetof(gnb_cu_up_e1_setup_resp_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_up_e1_setup_resp_ies_container, gnb_cu_cp_name),
     (int32_t)offsetof(gnb_cu_up_e1_setup_resp_ies_container, gnb_cu_cp_name_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gnb_cu_up_e1_setup_resp_ies_container::gnb_cu_cp_name),
                                asn1::table::string_json>::value},
    {offsetof(gnb_cu_up_e1_setup_resp_ies_container, transport_layer_address_info),
     (int32_t)offsetof(gnb_cu_up_e1_setup_resp_ies_container, transport_layer_address_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &transport_layer_address_info_s::desc},
    {offsetof(gnb_cu_up_e1_setup_resp_ies_container, extended_gnb_cu_cp_name),
     (int32_t)offsetof(gnb_cu_up_e1_setup_resp_ies_container, extended_gnb_cu_cp_name_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &extended_gnb_cu_cp_name_s::desc},
    {offsetof(gnb_cu_up_e1_setup_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(gnb_cu_up_e1_setup_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_gnb_cu_up_e1_setup_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(gnb_cu_up_e1_setup_resp_ies_container, gnb_cu_cp_name_present),
    (int32_t)offsetof(gnb_cu_up_e1_setup_resp_ies_container, transport_layer_address_info_present),
    (int32_t)offsetof(gnb_cu_up_e1_setup_resp_ies_container, extended_gnb_cu_cp_name_present),
    (int32_t)offsetof(gnb_cu_up_e1_setup_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc gnb_cu_up_e1_setup_resp_ies_container::desc = {
    objset_id_gnb_cu_up_e1_setup_resp_ies_o,
    objset_crit_gnb_cu_up_e1_setup_resp_ies_o,
    ie_table_gnb_cu_up_e1_setup_resp_ies_container_values,
    ie_table_gnb_cu_up_e1_setup_resp_ies_container_presences,
    5,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_gnb_cu_up_status_ind_ies_o[]   = {57, 65};
static const uint8_t  objset_crit_gnb_cu_up_status_ind_ies_o[] = {crit_e::reject, crit_e::reject};

// GNB-CU-UP-StatusIndicationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_gnb_cu_up_status_ind_ies_container_values[] = {
    {offsetof(gnb_cu_up_status_ind_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(gnb_cu_up_status_ind_ies_container, gnb_cu_up_overload_info),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(gnb_cu_up_status_ind_ies_container::gnb_cu_up_overload_info)>(),
     decltype(gnb_cu_up_status_ind_ies_container::gnb_cu_up_overload_info)::has_ext,
     false,
     decltype(gnb_cu_up_status_ind_ies_container::gnb_cu_up_overload_info)::nof_types,
     decltype(gnb_cu_up_status_ind_ies_container::gnb_cu_up_overload_info)::nof_exts,
     nullptr,
     &gnb_cu_up_overload_info_opts::desc}};
static const int32_t ie_table_gnb_cu_up_status_ind_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                                 asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc gnb_cu_up_status_ind_ies_container::desc = {
    objset_id_gnb_cu_up_status_ind_ies_o,
    objset_crit_gnb_cu_up_status_ind_ies_o,
    ie_table_gnb_cu_up_status_ind_ies_container_values,
    ie_table_gnb_cu_up_status_ind_ies_container_presences,
    2,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_iab_up_tnl_address_upd_ies_o[]   = {57, 108};
static const uint8_t  objset_crit_iab_up_tnl_address_upd_ies_o[] = {crit_e::reject, crit_e::ignore};

// IAB-UPTNLAddressUpdateIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_iab_up_tnl_address_upd_ies_container_values[] = {
    {offsetof(iab_up_tnl_address_upd_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(iab_up_tnl_address_upd_ies_container, dl_up_tnl_address_to_upd_list),
     (int32_t)offsetof(iab_up_tnl_address_upd_ies_container, dl_up_tnl_address_to_upd_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     7,
     &asn1::table::item_list_codec<decltype(iab_up_tnl_address_upd_ies_container::dl_up_tnl_address_to_upd_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &dl_up_tnl_address_to_upd_item_s::desc}};
static const int32_t ie_table_iab_up_tnl_address_upd_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(iab_up_tnl_address_upd_ies_container, dl_up_tnl_address_to_upd_list_present)};
const asn1::table::ie_container_desc iab_up_tnl_address_upd_ies_container::desc = {
    objset_id_iab_up_tnl_address_upd_ies_o,
    objset_crit_iab_up_tnl_address_upd_ies_o,
    ie_table_iab_up_tnl_address_upd_ies_container_values,
    ie_table_iab_up_tnl_address_upd_ies_container_presences,
    2,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_iab_up_tnl_address_upd_ack_ies_o[]   = {57, 1, 109};
static const uint8_t  objset_crit_iab_up_tnl_address_upd_ack_ies_o[] = {crit_e::reject, crit_e::ignore, crit_e::ignore};

// IAB-UPTNLAddressUpdateAcknowledgeIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_iab_up_tnl_address_upd_ack_ies_container_values[] = {
    {offsetof(iab_up_tnl_address_upd_ack_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(iab_up_tnl_address_upd_ack_ies_container, crit_diagnostics),
     (int32_t)offsetof(iab_up_tnl_address_upd_ack_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc},
    {offsetof(iab_up_tnl_address_upd_ack_ies_container, ul_up_tnl_address_to_upd_list),
     (int32_t)offsetof(iab_up_tnl_address_upd_ack_ies_container, ul_up_tnl_address_to_upd_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     7,
     &asn1::table::item_list_codec<decltype(iab_up_tnl_address_upd_ack_ies_container::ul_up_tnl_address_to_upd_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ul_up_tnl_address_to_upd_item_s::desc}};
static const int32_t ie_table_iab_up_tnl_address_upd_ack_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(iab_up_tnl_address_upd_ack_ies_container, crit_diagnostics_present),
    (int32_t)offsetof(iab_up_tnl_address_upd_ack_ies_container, ul_up_tnl_address_to_upd_list_present)};
const asn1::table::ie_container_desc iab_up_tnl_address_upd_ack_ies_container::desc = {
    objset_id_iab_up_tnl_address_upd_ack_ies_o,
    objset_crit_iab_up_tnl_address_upd_ack_ies_o,
    ie_table_iab_up_tnl_address_upd_ack_ies_container_values,
    ie_table_iab_up_tnl_address_upd_ack_ies_container_presences,
    3,
    1,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_iab_up_tnl_address_upd_fail_ies_o[]   = {57, 0, 12, 1};
static const uint8_t  objset_crit_iab_up_tnl_address_upd_fail_ies_o[] = {crit_e::reject,
                                                                         crit_e::ignore,
                                                                         crit_e::ignore,
                                                                         crit_e::ignore};

// IAB-UPTNLAddressUpdateFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_iab_up_tnl_address_upd_fail_ies_container_values[] = {
    {offsetof(iab_up_tnl_address_upd_fail_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(iab_up_tnl_address_upd_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(iab_up_tnl_address_upd_fail_ies_container, time_to_wait),
     (int32_t)offsetof(iab_up_tnl_address_upd_fail_ies_container, time_to_wait_present),
     asn1::table::enum_op<decltype(iab_up_tnl_address_upd_fail_ies_container::time_to_wait)>(),
     decltype(iab_up_tnl_address_upd_fail_ies_container::time_to_wait)::has_ext,
     false,
     decltype(iab_up_tnl_address_upd_fail_ies_container::time_to_wait)::nof_types,
     decltype(iab_up_tnl_address_upd_fail_ies_container::time_to_wait)::nof_exts,
     nullptr,
     &time_to_wait_opts::desc},
    {offsetof(iab_up_tnl_address_upd_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(iab_up_tnl_address_upd_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_iab_up_tnl_address_upd_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(iab_up_tnl_address_upd_fail_ies_container, time_to_wait_present),
    (int32_t)offsetof(iab_up_tnl_address_upd_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc iab_up_tnl_address_upd_fail_ies_container::desc = {
    objset_id_iab_up_tnl_address_upd_fail_ies_o,
    objset_crit_iab_up_tnl_address_upd_fail_ies_o,
    ie_table_iab_up_tnl_address_upd_fail_ies_container_values,
    ie_table_iab_up_tnl_address_upd_fail_ies_container_presences,
    4,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_iabpsk_notif_ies_o[]   = {57, 144};
static const uint8_t  objset_crit_iabpsk_notif_ies_o[] = {crit_e::reject, crit_e::reject};

// IABPSKNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_iabpsk_notif_ies_container_values[] = {
    {offsetof(iabpsk_notif_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(iabpsk_notif_ies_container, iab_donor_cu_up_psk_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<decltype(iabpsk_notif_ies_container::iab_donor_cu_up_psk_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &iab_donor_cu_up_psk_info_item_s::desc}};
static const int32_t ie_table_iabpsk_notif_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                         asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc iabpsk_notif_ies_container::desc = {objset_id_iabpsk_notif_ies_o,
                                                                         objset_crit_iabpsk_notif_ies_o,
                                                                         ie_table_iabpsk_notif_ies_container_values,
                                                                         ie_table_iabpsk_notif_ies_container_presences,
                                                                         2,
                                                                         2,
                                                                         0,
                                                                         65535,
                                                                         true,
                                                                         "value"};

static const uint32_t objset_id_mc_bearer_context_mod_confirm_ies_o[]   = {155, 156, 169, 1};
static const uint8_t  objset_crit_mc_bearer_context_mod_confirm_ies_o[] = {crit_e::reject,
                                                                           crit_e::reject,
                                                                           crit_e::reject,
                                                                           crit_e::ignore};

// MCBearerContextModificationConfirmIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_mc_bearer_context_mod_confirm_ies_container_values[] = {
    {offsetof(mc_bearer_context_mod_confirm_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(mc_bearer_context_mod_confirm_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(mc_bearer_context_mod_confirm_ies_container, mc_bearer_context_to_modify_confirm),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_to_modify_confirm_s::desc},
    {offsetof(mc_bearer_context_mod_confirm_ies_container, crit_diagnostics),
     (int32_t)offsetof(mc_bearer_context_mod_confirm_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_mc_bearer_context_mod_confirm_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(mc_bearer_context_mod_confirm_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc mc_bearer_context_mod_confirm_ies_container::desc = {
    objset_id_mc_bearer_context_mod_confirm_ies_o,
    objset_crit_mc_bearer_context_mod_confirm_ies_o,
    ie_table_mc_bearer_context_mod_confirm_ies_container_values,
    ie_table_mc_bearer_context_mod_confirm_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_mc_bearer_context_mod_fail_ies_o[]   = {155, 156, 170, 0, 1};
static const uint8_t  objset_crit_mc_bearer_context_mod_fail_ies_o[] = {crit_e::reject,
                                                                        crit_e::reject,
                                                                        crit_e::reject,
                                                                        crit_e::ignore,
                                                                        crit_e::ignore};

// MCBearerContextModificationFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_mc_bearer_context_mod_fail_ies_container_values[] = {
    {offsetof(mc_bearer_context_mod_fail_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(mc_bearer_context_mod_fail_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(mc_bearer_context_mod_fail_ies_container, mbs_multicast_f1_u_context_descriptor),
     (int32_t)offsetof(mc_bearer_context_mod_fail_ies_container, mbs_multicast_f1_u_context_descriptor_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_multicast_f1_u_context_descriptor_s::desc},
    {offsetof(mc_bearer_context_mod_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(mc_bearer_context_mod_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(mc_bearer_context_mod_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_mc_bearer_context_mod_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(mc_bearer_context_mod_fail_ies_container, mbs_multicast_f1_u_context_descriptor_present),
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(mc_bearer_context_mod_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc mc_bearer_context_mod_fail_ies_container::desc = {
    objset_id_mc_bearer_context_mod_fail_ies_o,
    objset_crit_mc_bearer_context_mod_fail_ies_o,
    ie_table_mc_bearer_context_mod_fail_ies_container_values,
    ie_table_mc_bearer_context_mod_fail_ies_container_presences,
    5,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_mc_bearer_context_mod_request_ies_o[]   = {155, 156, 166};
static const uint8_t  objset_crit_mc_bearer_context_mod_request_ies_o[] = {crit_e::reject,
                                                                           crit_e::reject,
                                                                           crit_e::reject};

// MCBearerContextModificationRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_mc_bearer_context_mod_request_ies_container_values[] = {
    {offsetof(mc_bearer_context_mod_request_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(mc_bearer_context_mod_request_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(mc_bearer_context_mod_request_ies_container, mc_bearer_context_to_modify),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_to_modify_s::desc}};
static const int32_t ie_table_mc_bearer_context_mod_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc mc_bearer_context_mod_request_ies_container::desc = {
    objset_id_mc_bearer_context_mod_request_ies_o,
    objset_crit_mc_bearer_context_mod_request_ies_o,
    ie_table_mc_bearer_context_mod_request_ies_container_values,
    ie_table_mc_bearer_context_mod_request_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_mc_bearer_context_mod_required_ies_o[]   = {155, 156, 168};
static const uint8_t  objset_crit_mc_bearer_context_mod_required_ies_o[] = {crit_e::reject,
                                                                            crit_e::reject,
                                                                            crit_e::reject};

// MCBearerContextModificationRequiredIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_mc_bearer_context_mod_required_ies_container_values[] = {
    {offsetof(mc_bearer_context_mod_required_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(mc_bearer_context_mod_required_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(mc_bearer_context_mod_required_ies_container, mc_bearer_context_to_modify_required),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_to_modify_required_s::desc}};
static const int32_t ie_table_mc_bearer_context_mod_required_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc mc_bearer_context_mod_required_ies_container::desc = {
    objset_id_mc_bearer_context_mod_required_ies_o,
    objset_crit_mc_bearer_context_mod_required_ies_o,
    ie_table_mc_bearer_context_mod_required_ies_container_values,
    ie_table_mc_bearer_context_mod_required_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_mc_bearer_context_mod_resp_ies_o[]   = {155, 156, 167, 1};
static const uint8_t  objset_crit_mc_bearer_context_mod_resp_ies_o[] = {crit_e::reject,
                                                                        crit_e::reject,
                                                                        crit_e::reject,
                                                                        crit_e::ignore};

// MCBearerContextModificationResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_mc_bearer_context_mod_resp_ies_container_values[] = {
    {offsetof(mc_bearer_context_mod_resp_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(mc_bearer_context_mod_resp_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(mc_bearer_context_mod_resp_ies_container, mc_bearer_context_to_modify_resp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_to_modify_resp_s::desc},
    {offsetof(mc_bearer_context_mod_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(mc_bearer_context_mod_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_mc_bearer_context_mod_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(mc_bearer_context_mod_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc mc_bearer_context_mod_resp_ies_container::desc = {
    objset_id_mc_bearer_context_mod_resp_ies_o,
    objset_crit_mc_bearer_context_mod_resp_ies_o,
    ie_table_mc_bearer_context_mod_resp_ies_container_values,
    ie_table_mc_bearer_context_mod_resp_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_mc_bearer_context_release_cmd_ies_o[]   = {155, 156, 0};
static const uint8_t  objset_crit_mc_bearer_context_release_cmd_ies_o[] = {crit_e::reject,
                                                                           crit_e::reject,
                                                                           crit_e::ignore};

// MCBearerContextReleaseCommandIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_mc_bearer_context_release_cmd_ies_container_values[] = {
    {offsetof(mc_bearer_context_release_cmd_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(mc_bearer_context_release_cmd_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(mc_bearer_context_release_cmd_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const int32_t ie_table_mc_bearer_context_release_cmd_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc mc_bearer_context_release_cmd_ies_container::desc = {
    objset_id_mc_bearer_context_release_cmd_ies_o,
    objset_crit_mc_bearer_context_release_cmd_ies_o,
    ie_table_mc_bearer_context_release_cmd_ies_container_values,
    ie_table_mc_bearer_context_release_cmd_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_mc_bearer_context_release_complete_ies_o[]   = {155, 156, 1};
static const uint8_t  objset_crit_mc_bearer_context_release_complete_ies_o[] = {crit_e::reject,
                                                                                crit_e::reject,
                                                                                crit_e::ignore};

// MCBearerContextReleaseCompleteIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_mc_bearer_context_release_complete_ies_container_values[] = {
    {offsetof(mc_bearer_context_release_complete_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(mc_bearer_context_release_complete_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(mc_bearer_context_release_complete_ies_container, crit_diagnostics),
     (int32_t)offsetof(mc_bearer_context_release_complete_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_mc_bearer_context_release_complete_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(mc_bearer_context_release_complete_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc mc_bearer_context_release_complete_ies_container::desc = {
    objset_id_mc_bearer_context_release_complete_ies_o,
    objset_crit_mc_bearer_context_release_complete_ies_o,
    ie_table_mc_bearer_context_release_complete_ies_container_values,
    ie_table_mc_bearer_context_release_complete_ies_container_presences,
    3,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_mc_bearer_context_release_request_ies_o[]   = {155, 156, 0};
static const uint8_t  objset_crit_mc_bearer_context_release_request_ies_o[] = {crit_e::reject,
                                                                               crit_e::reject,
                                                                               crit_e::ignore};

// MCBearerContextReleaseRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_mc_bearer_context_release_request_ies_container_values[] = {
    {offsetof(mc_bearer_context_release_request_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(mc_bearer_context_release_request_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(mc_bearer_context_release_request_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const int32_t ie_table_mc_bearer_context_release_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc mc_bearer_context_release_request_ies_container::desc = {
    objset_id_mc_bearer_context_release_request_ies_o,
    objset_crit_mc_bearer_context_release_request_ies_o,
    ie_table_mc_bearer_context_release_request_ies_container_values,
    ie_table_mc_bearer_context_release_request_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_mc_bearer_context_setup_fail_ies_o[]   = {155, 156, 0, 1};
static const uint8_t  objset_crit_mc_bearer_context_setup_fail_ies_o[] = {crit_e::reject,
                                                                          crit_e::ignore,
                                                                          crit_e::ignore,
                                                                          crit_e::ignore};

// MCBearerContextSetupFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_mc_bearer_context_setup_fail_ies_container_values[] = {
    {offsetof(mc_bearer_context_setup_fail_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(mc_bearer_context_setup_fail_ies_container, gnb_cu_up_mbs_e1ap_id),
     (int32_t)offsetof(mc_bearer_context_setup_fail_ies_container, gnb_cu_up_mbs_e1ap_id_present),
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(mc_bearer_context_setup_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(mc_bearer_context_setup_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(mc_bearer_context_setup_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_mc_bearer_context_setup_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(mc_bearer_context_setup_fail_ies_container, gnb_cu_up_mbs_e1ap_id_present),
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(mc_bearer_context_setup_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc mc_bearer_context_setup_fail_ies_container::desc = {
    objset_id_mc_bearer_context_setup_fail_ies_o,
    objset_crit_mc_bearer_context_setup_fail_ies_o,
    ie_table_mc_bearer_context_setup_fail_ies_container_values,
    ie_table_mc_bearer_context_setup_fail_ies_container_presences,
    4,
    2,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_mc_bearer_context_setup_request_ies_o[]   = {155, 157, 164};
static const uint8_t  objset_crit_mc_bearer_context_setup_request_ies_o[] = {crit_e::reject,
                                                                             crit_e::reject,
                                                                             crit_e::reject};

// MCBearerContextSetupRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_mc_bearer_context_setup_request_ies_container_values[] = {
    {offsetof(mc_bearer_context_setup_request_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(mc_bearer_context_setup_request_ies_container, global_mbs_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_mbs_session_id_s::desc},
    {offsetof(mc_bearer_context_setup_request_ies_container, mc_bearer_context_to_setup),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_to_setup_s::desc}};
static const int32_t ie_table_mc_bearer_context_setup_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc mc_bearer_context_setup_request_ies_container::desc = {
    objset_id_mc_bearer_context_setup_request_ies_o,
    objset_crit_mc_bearer_context_setup_request_ies_o,
    ie_table_mc_bearer_context_setup_request_ies_container_values,
    ie_table_mc_bearer_context_setup_request_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_mc_bearer_context_setup_resp_ies_o[]   = {155, 156, 165, 1};
static const uint8_t  objset_crit_mc_bearer_context_setup_resp_ies_o[] = {crit_e::reject,
                                                                          crit_e::reject,
                                                                          crit_e::reject,
                                                                          crit_e::ignore};

// MCBearerContextSetupResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_mc_bearer_context_setup_resp_ies_container_values[] = {
    {offsetof(mc_bearer_context_setup_resp_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(mc_bearer_context_setup_resp_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(mc_bearer_context_setup_resp_ies_container, mc_bearer_context_to_setup_resp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_to_setup_resp_s::desc},
    {offsetof(mc_bearer_context_setup_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(mc_bearer_context_setup_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_mc_bearer_context_setup_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(mc_bearer_context_setup_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc mc_bearer_context_setup_resp_ies_container::desc = {
    objset_id_mc_bearer_context_setup_resp_ies_o,
    objset_crit_mc_bearer_context_setup_resp_ies_o,
    ie_table_mc_bearer_context_setup_resp_ies_container_values,
    ie_table_mc_bearer_context_setup_resp_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_mc_bearer_notif_ies_o[]   = {155, 156, 191};
static const uint8_t  objset_crit_mc_bearer_notif_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::ignore};

// MCBearerNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_mc_bearer_notif_ies_container_values[] = {
    {offsetof(mc_bearer_notif_ies_container, gnb_cu_cp_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(mc_bearer_notif_ies_container, gnb_cu_up_mbs_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(mc_bearer_notif_ies_container, mbs_session_res_notif),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_session_res_notif_c::desc}};
static const int32_t ie_table_mc_bearer_notif_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc mc_bearer_notif_ies_container::desc = {
    objset_id_mc_bearer_notif_ies_o,
    objset_crit_mc_bearer_notif_ies_o,
    ie_table_mc_bearer_notif_ies_container_values,
    ie_table_mc_bearer_notif_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_mrdc_data_usage_report_ies_o[]   = {2, 3, 68};
static const uint8_t  objset_crit_mrdc_data_usage_report_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::ignore};

// MRDC-DataUsageReportIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_mrdc_data_usage_report_ies_container_values[] = {
    {offsetof(mrdc_data_usage_report_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(mrdc_data_usage_report_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(mrdc_data_usage_report_ies_container, pdu_session_res_data_usage_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<decltype(mrdc_data_usage_report_ies_container::pdu_session_res_data_usage_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pdu_session_res_data_usage_item_s::desc}};
static const int32_t ie_table_mrdc_data_usage_report_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                                   asn1::table::seq_field::mandatory,
                                                                                   asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc mrdc_data_usage_report_ies_container::desc = {
    objset_id_mrdc_data_usage_report_ies_o,
    objset_crit_mrdc_data_usage_report_ies_o,
    ie_table_mrdc_data_usage_report_ies_container_values,
    ie_table_mrdc_data_usage_report_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};

// Value ::= OPEN TYPE
static const asn1::table::field_name choice_table_e1ap_private_ies_empty_o_value_c_names[] = {};
const asn1::table::choice_desc       e1ap_private_ies_empty_o::value_c::desc               = {
    asn1::table::enum_int_type<e1ap_private_ies_empty_o::value_c::types>(),
    e1ap_private_ies_empty_o::value_c::types::nof_types,
    e1ap_private_ies_empty_o::value_c::types::nof_exts,
    e1ap_private_ies_empty_o::value_c::types::has_ext,
    true,
    nullptr,
    nullptr,
    choice_table_e1ap_private_ies_empty_o_value_c_names,
    "e1ap_private_ies_empty_o::value_c",
    asn1::table::choice_desc::no_member,
    asn1::table::choice_desc::no_member,
    false,
    nullptr,
    true};

static const char*           enum_names_e1ap_private_ies_empty_o_value_c_types_values[] = {};
const asn1::table::enum_desc e1ap_private_ies_empty_o::value_c::types_opts::desc        = {
    enum_names_e1ap_private_ies_empty_o_value_c_types_values,
    0,
    "e1ap_private_ies_empty_o::value_c::types"};

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

// ResetAll ::= ENUMERATED
static const char*           enum_names_reset_all_e_values[] = {"reset-all"};
const asn1::table::enum_desc reset_all_opts::desc            = {enum_names_reset_all_e_values, 1, "reset_all_e"};

// UE-associatedLogicalE1-ConnectionItemRes ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const uint32_t objset_id_ue_associated_lc_e1_conn_item_res_o[]    = {5};
static const uint8_t  objset_crit_ue_associated_lc_e1_conn_item_res_o[]  = {crit_e::reject};
static const uint8_t  objset_value_ue_associated_lc_e1_conn_item_res_o[] = {
    ue_associated_lc_e1_conn_item_res_o::value_c::types::ue_associated_lc_e1_conn_item};
static const uint8_t                    objset_presence_ue_associated_lc_e1_conn_item_res_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ue_associated_lc_e1_conn_item_res_o[]  = {
    {objset_crit_ue_associated_lc_e1_conn_item_res_o, crit_e::nulltype},
    {objset_value_ue_associated_lc_e1_conn_item_res_o, ue_associated_lc_e1_conn_item_res_o::value_c::types::nulltype},
    {objset_presence_ue_associated_lc_e1_conn_item_res_o, presence_e::nulltype}};
const asn1::table::objset_desc ue_associated_lc_e1_conn_item_res_o::desc =
    {objset_id_ue_associated_lc_e1_conn_item_res_o, 1, "id", objset_columns_ue_associated_lc_e1_conn_item_res_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ue_associated_lc_e1_conn_item_res_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_associated_lc_e1_conn_item_s::desc}};
static const asn1::table::field_name choice_table_ue_associated_lc_e1_conn_item_res_o_value_c_names[] = {
    {"UE-associatedLogicalE1-ConnectionItem", nullptr}};
const asn1::table::choice_desc ue_associated_lc_e1_conn_item_res_o::value_c::desc = {
    asn1::table::enum_int_type<ue_associated_lc_e1_conn_item_res_o::value_c::types>(),
    ue_associated_lc_e1_conn_item_res_o::value_c::types::nof_types,
    ue_associated_lc_e1_conn_item_res_o::value_c::types::nof_exts,
    ue_associated_lc_e1_conn_item_res_o::value_c::types::has_ext,
    true,
    choice_table_ue_associated_lc_e1_conn_item_res_o_value_c_alternatives,
    nullptr,
    choice_table_ue_associated_lc_e1_conn_item_res_o_value_c_names,
    "ue_associated_lc_e1_conn_item_res_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ue_associated_lc_e1_conn_item_res_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ue_associated_lc_e1_conn_item_res_o_value_c_types_values[] = {
    "UE-associatedLogicalE1-ConnectionItem"};
const asn1::table::enum_desc ue_associated_lc_e1_conn_item_res_o::value_c::types_opts::desc = {
    enum_names_ue_associated_lc_e1_conn_item_res_o_value_c_types_values,
    1,
    "ue_associated_lc_e1_conn_item_res_o::value_c::types"};
uint8_t ue_associated_lc_e1_conn_item_res_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ue_associated_lc_e1_conn_item_res_o::value_c::types");
}

// ResetType ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_reset_type_c_alternatives[] = {
    &asn1::table::alternative_ops<reset_all_e, true>::value,
    &asn1::table::alternative_ops<ue_associated_lc_e1_conn_list_res_l, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<reset_type_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc reset_type_c::storage_desc = {choice_storage_reset_type_c_alternatives,
                                                                     3,
                                                                     "reset_type_c"};

reset_all_e& reset_type_c::set_e1_interface()
{
  set(types::e1_interface);
  return c.get<reset_all_e>();
}
ue_associated_lc_e1_conn_list_res_l& reset_type_c::set_part_of_e1_interface()
{
  set(types::part_of_e1_interface);
  return c.get<ue_associated_lc_e1_conn_list_res_l>();
}
protocol_ie_single_container_s<reset_type_ext_ies_o>& reset_type_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_reset_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<reset_all_e>(),
     reset_all_e::has_ext,
     false,
     reset_all_e::nof_types,
     reset_all_e::nof_exts,
     nullptr,
     &reset_all_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65535,
     &asn1::table::list_codec<ue_associated_lc_e1_conn_list_res_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<reset_type_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_reset_type_c_names[] = {{"e1-Interface", nullptr},
                                                                          {"partOfE1-Interface", nullptr},
                                                                          {"choice-extension", nullptr}};
const asn1::table::choice_desc       reset_type_c::desc = {asn1::table::enum_int_type<reset_type_c::types>(),
                                                           reset_type_c::types::nof_types,
                                                           reset_type_c::types::nof_exts,
                                                           reset_type_c::types::has_ext,
                                                           true,
                                                           choice_table_reset_type_c_alternatives,
                                                           nullptr,
                                                           choice_table_reset_type_c_names,
                                                           "reset_type_c",
                                                           offsetof(reset_type_c, c.type),
                                                           offsetof(reset_type_c, c.value),
                                                           false,
                                                           &reset_type_c::storage_desc};

static const char* enum_names_reset_type_c_types_values[] = {"e1-Interface", "partOfE1-Interface", "choice-extension"};
const asn1::table::enum_desc reset_type_c::types_opts::desc = {enum_names_reset_type_c_types_values,
                                                               3,
                                                               "reset_type_c::types"};

static const uint32_t objset_id_reset_ies_o[]   = {57, 0, 4};
static const uint8_t  objset_crit_reset_ies_o[] = {crit_e::reject, crit_e::ignore, crit_e::reject};

// ResetIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_reset_ies_container_values[] = {
    {offsetof(reset_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(reset_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(reset_ies_container, reset_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &reset_type_c::desc}};
static const int32_t                 ie_table_reset_ies_container_presences[] = {asn1::table::seq_field::mandatory,
                                                                                 asn1::table::seq_field::mandatory,
                                                                                 asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc reset_ies_container::desc                = {objset_id_reset_ies_o,
                                                                                 objset_crit_reset_ies_o,
                                                                                 ie_table_reset_ies_container_values,
                                                                                 ie_table_reset_ies_container_presences,
                                                                                 3,
                                                                                 3,
                                                                                 0,
                                                                                 65535,
                                                                                 true,
                                                                                 "value"};

// UE-associatedLogicalE1-ConnectionItemResAck ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const uint32_t objset_id_ue_associated_lc_e1_conn_item_res_ack_o[]    = {5};
static const uint8_t  objset_crit_ue_associated_lc_e1_conn_item_res_ack_o[]  = {crit_e::ignore};
static const uint8_t  objset_value_ue_associated_lc_e1_conn_item_res_ack_o[] = {
    ue_associated_lc_e1_conn_item_res_ack_o::value_c::types::ue_associated_lc_e1_conn_item};
static const uint8_t objset_presence_ue_associated_lc_e1_conn_item_res_ack_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_ue_associated_lc_e1_conn_item_res_ack_o[] = {
    {objset_crit_ue_associated_lc_e1_conn_item_res_ack_o, crit_e::nulltype},
    {objset_value_ue_associated_lc_e1_conn_item_res_ack_o,
     ue_associated_lc_e1_conn_item_res_ack_o::value_c::types::nulltype},
    {objset_presence_ue_associated_lc_e1_conn_item_res_ack_o, presence_e::nulltype}};
const asn1::table::objset_desc ue_associated_lc_e1_conn_item_res_ack_o::desc = {
    objset_id_ue_associated_lc_e1_conn_item_res_ack_o,
    1,
    "id",
    objset_columns_ue_associated_lc_e1_conn_item_res_ack_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ue_associated_lc_e1_conn_item_res_ack_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_associated_lc_e1_conn_item_s::desc}};
static const asn1::table::field_name choice_table_ue_associated_lc_e1_conn_item_res_ack_o_value_c_names[] = {
    {"UE-associatedLogicalE1-ConnectionItem", nullptr}};
const asn1::table::choice_desc ue_associated_lc_e1_conn_item_res_ack_o::value_c::desc = {
    asn1::table::enum_int_type<ue_associated_lc_e1_conn_item_res_ack_o::value_c::types>(),
    ue_associated_lc_e1_conn_item_res_ack_o::value_c::types::nof_types,
    ue_associated_lc_e1_conn_item_res_ack_o::value_c::types::nof_exts,
    ue_associated_lc_e1_conn_item_res_ack_o::value_c::types::has_ext,
    true,
    choice_table_ue_associated_lc_e1_conn_item_res_ack_o_value_c_alternatives,
    nullptr,
    choice_table_ue_associated_lc_e1_conn_item_res_ack_o_value_c_names,
    "ue_associated_lc_e1_conn_item_res_ack_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(ue_associated_lc_e1_conn_item_res_ack_o::value_c, c),
    true,
    nullptr,
    true};

static const char* enum_names_ue_associated_lc_e1_conn_item_res_ack_o_value_c_types_values[] = {
    "UE-associatedLogicalE1-ConnectionItem"};
const asn1::table::enum_desc ue_associated_lc_e1_conn_item_res_ack_o::value_c::types_opts::desc = {
    enum_names_ue_associated_lc_e1_conn_item_res_ack_o_value_c_types_values,
    1,
    "ue_associated_lc_e1_conn_item_res_ack_o::value_c::types"};
uint8_t ue_associated_lc_e1_conn_item_res_ack_o::value_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ue_associated_lc_e1_conn_item_res_ack_o::value_c::types");
}

static const uint32_t objset_id_reset_ack_ies_o[]   = {57, 6, 1};
static const uint8_t  objset_crit_reset_ack_ies_o[] = {crit_e::reject, crit_e::ignore, crit_e::ignore};

// ResetAcknowledgeIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_reset_ack_ies_container_values[] = {
    {offsetof(reset_ack_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(reset_ack_ies_container, ue_associated_lc_e1_conn_list_res_ack),
     (int32_t)offsetof(reset_ack_ies_container, ue_associated_lc_e1_conn_list_res_ack_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65535,
     &asn1::table::list_codec<decltype(reset_ack_ies_container::ue_associated_lc_e1_conn_list_res_ack),
                              asn1::table::struct_json>::value},
    {offsetof(reset_ack_ies_container, crit_diagnostics),
     (int32_t)offsetof(reset_ack_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_reset_ack_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(reset_ack_ies_container, ue_associated_lc_e1_conn_list_res_ack_present),
    (int32_t)offsetof(reset_ack_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc reset_ack_ies_container::desc = {objset_id_reset_ack_ies_o,
                                                                      objset_crit_reset_ack_ies_o,
                                                                      ie_table_reset_ack_ies_container_values,
                                                                      ie_table_reset_ack_ies_container_presences,
                                                                      3,
                                                                      1,
                                                                      0,
                                                                      65535,
                                                                      true,
                                                                      "value"};

static const uint32_t objset_id_res_status_fail_ies_o[]   = {57, 89, 90, 0, 1};
static const uint8_t  objset_crit_res_status_fail_ies_o[] = {crit_e::reject,
                                                             crit_e::reject,
                                                             crit_e::ignore,
                                                             crit_e::ignore,
                                                             crit_e::ignore};

// ResourceStatusFailureIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_res_status_fail_ies_container_values[] = {
    {offsetof(res_status_fail_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(res_status_fail_ies_container, gnb_cu_cp_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(res_status_fail_ies_container, gnb_cu_up_meas_id),
     (int32_t)offsetof(res_status_fail_ies_container, gnb_cu_up_meas_id_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(res_status_fail_ies_container, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(res_status_fail_ies_container, crit_diagnostics),
     (int32_t)offsetof(res_status_fail_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_res_status_fail_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(res_status_fail_ies_container, gnb_cu_up_meas_id_present),
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(res_status_fail_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc res_status_fail_ies_container::desc = {
    objset_id_res_status_fail_ies_o,
    objset_crit_res_status_fail_ies_o,
    ie_table_res_status_fail_ies_container_values,
    ie_table_res_status_fail_ies_container_presences,
    5,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_res_status_request_ies_o[] = {57, 89, 90, 91, 92, 93};
static const uint8_t  objset_crit_res_status_request_ies_o[] =
    {crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::reject, crit_e::reject, crit_e::ignore};

// ResourceStatusRequestIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_res_status_request_ies_container_values[] = {
    {offsetof(res_status_request_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(res_status_request_ies_container, gnb_cu_cp_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(res_status_request_ies_container, gnb_cu_up_meas_id),
     (int32_t)offsetof(res_status_request_ies_container, gnb_cu_up_meas_id_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(res_status_request_ies_container, regist_request),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(res_status_request_ies_container::regist_request)>(),
     decltype(res_status_request_ies_container::regist_request)::has_ext,
     false,
     decltype(res_status_request_ies_container::regist_request)::nof_types,
     decltype(res_status_request_ies_container::regist_request)::nof_exts,
     nullptr,
     &regist_request_opts::desc},
    {offsetof(res_status_request_ies_container, report_characteristics),
     (int32_t)offsetof(res_status_request_ies_container, report_characteristics_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(res_status_request_ies_container::report_characteristics),
                                asn1::table::struct_json>::value},
    {offsetof(res_status_request_ies_container, report_periodicity),
     (int32_t)offsetof(res_status_request_ies_container, report_periodicity_present),
     asn1::table::enum_op<decltype(res_status_request_ies_container::report_periodicity)>(),
     decltype(res_status_request_ies_container::report_periodicity)::has_ext,
     false,
     decltype(res_status_request_ies_container::report_periodicity)::nof_types,
     decltype(res_status_request_ies_container::report_periodicity)::nof_exts,
     nullptr,
     &report_periodicity_opts::desc}};
static const int32_t ie_table_res_status_request_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(res_status_request_ies_container, gnb_cu_up_meas_id_present),
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(res_status_request_ies_container, report_characteristics_present),
    (int32_t)offsetof(res_status_request_ies_container, report_periodicity_present)};
const asn1::table::ie_container_desc res_status_request_ies_container::desc = {
    objset_id_res_status_request_ies_o,
    objset_crit_res_status_request_ies_o,
    ie_table_res_status_request_ies_container_values,
    ie_table_res_status_request_ies_container_presences,
    6,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_res_status_resp_ies_o[]   = {57, 89, 90, 1};
static const uint8_t  objset_crit_res_status_resp_ies_o[] = {crit_e::reject,
                                                             crit_e::reject,
                                                             crit_e::ignore,
                                                             crit_e::ignore};

// ResourceStatusResponseIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_res_status_resp_ies_container_values[] = {
    {offsetof(res_status_resp_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(res_status_resp_ies_container, gnb_cu_cp_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(res_status_resp_ies_container, gnb_cu_up_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(res_status_resp_ies_container, crit_diagnostics),
     (int32_t)offsetof(res_status_resp_ies_container, crit_diagnostics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &crit_diagnostics_s::desc}};
static const int32_t ie_table_res_status_resp_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(res_status_resp_ies_container, crit_diagnostics_present)};
const asn1::table::ie_container_desc res_status_resp_ies_container::desc = {
    objset_id_res_status_resp_ies_o,
    objset_crit_res_status_resp_ies_o,
    ie_table_res_status_resp_ies_container_values,
    ie_table_res_status_resp_ies_container_presences,
    4,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_res_status_upd_ies_o[]   = {57, 89, 90, 94, 95};
static const uint8_t  objset_crit_res_status_upd_ies_o[] = {crit_e::reject,
                                                            crit_e::reject,
                                                            crit_e::ignore,
                                                            crit_e::ignore,
                                                            crit_e::ignore};

// ResourceStatusUpdateIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_res_status_upd_ies_container_values[] = {
    {offsetof(res_status_upd_ies_container, transaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(res_status_upd_ies_container, gnb_cu_cp_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(res_status_upd_ies_container, gnb_cu_up_meas_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{4094},
     nullptr},
    {offsetof(res_status_upd_ies_container, tnl_available_capacity_ind),
     (int32_t)offsetof(res_status_upd_ies_container, tnl_available_capacity_ind_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tnl_available_capacity_ind_s::desc},
    {offsetof(res_status_upd_ies_container, hw_capacity_ind),
     (int32_t)offsetof(res_status_upd_ies_container, hw_capacity_ind_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &hw_capacity_ind_s::desc}};
static const int32_t ie_table_res_status_upd_ies_container_presences[] = {
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    asn1::table::seq_field::mandatory,
    (int32_t)offsetof(res_status_upd_ies_container, tnl_available_capacity_ind_present),
    (int32_t)offsetof(res_status_upd_ies_container, hw_capacity_ind_present)};
const asn1::table::ie_container_desc res_status_upd_ies_container::desc = {
    objset_id_res_status_upd_ies_o,
    objset_crit_res_status_upd_ies_o,
    ie_table_res_status_upd_ies_container_values,
    ie_table_res_status_upd_ies_container_presences,
    5,
    3,
    0,
    65535,
    true,
    "value"};

static const uint32_t objset_id_trace_start_ies_o[]   = {2, 3, 81};
static const uint8_t  objset_crit_trace_start_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::ignore};

// TraceStartIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_trace_start_ies_container_values[] = {
    {offsetof(trace_start_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(trace_start_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(trace_start_ies_container, trace_activation),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trace_activation_s::desc}};
static const int32_t ie_table_trace_start_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                        asn1::table::seq_field::mandatory,
                                                                        asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc trace_start_ies_container::desc = {objset_id_trace_start_ies_o,
                                                                        objset_crit_trace_start_ies_o,
                                                                        ie_table_trace_start_ies_container_values,
                                                                        ie_table_trace_start_ies_container_presences,
                                                                        3,
                                                                        3,
                                                                        0,
                                                                        65535,
                                                                        true,
                                                                        "value"};

static const uint32_t objset_id_ul_data_notif_ies_o[]   = {2, 3, 67};
static const uint8_t  objset_crit_ul_data_notif_ies_o[] = {crit_e::reject, crit_e::reject, crit_e::reject};

// ULDataNotificationIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const asn1::table::seq_field ie_table_ul_data_notif_ies_container_values[] = {
    {offsetof(ul_data_notif_ies_container, gnb_cu_cp_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(ul_data_notif_ies_container, gnb_cu_up_ue_e1ap_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(ul_data_notif_ies_container, pdu_session_to_notify_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<decltype(ul_data_notif_ies_container::pdu_session_to_notify_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pdu_session_to_notify_item_s::desc}};
static const int32_t ie_table_ul_data_notif_ies_container_presences[]  = {asn1::table::seq_field::mandatory,
                                                                          asn1::table::seq_field::mandatory,
                                                                          asn1::table::seq_field::mandatory};
const asn1::table::ie_container_desc ul_data_notif_ies_container::desc = {
    objset_id_ul_data_notif_ies_o,
    objset_crit_ul_data_notif_ies_o,
    ie_table_ul_data_notif_ies_container_values,
    ie_table_ul_data_notif_ies_container_presences,
    3,
    3,
    0,
    65535,
    true,
    "value"};
