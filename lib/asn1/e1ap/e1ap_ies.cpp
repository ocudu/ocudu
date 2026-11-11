// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/e1ap/e1ap_ies.h"
#include "ocudu/asn1/asn1_ap_codec.h"

using namespace asn1;
using namespace asn1::e1ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// TriggeringMessage ::= ENUMERATED
static const char*           enum_names_trigger_msg_e_values[] = {"initiating-message",
                                                                  "successful-outcome",
                                                                  "unsuccessful-outcome"};
const asn1::table::enum_desc trigger_msg_opts::desc            = {enum_names_trigger_msg_e_values, 3, "trigger_msg_e"};

// DRB-Activity ::= ENUMERATED
static const char*           enum_names_drb_activity_e_values[] = {"active", "not-active"};
const asn1::table::enum_desc drb_activity_opts::desc = {enum_names_drb_activity_e_values, 2, "drb_activity_e"};

// DRB-Activity-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_activity_item_s_fields[] = {
    {offsetof(drb_activity_item_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_activity_item_s, drb_activity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(drb_activity_item_s::drb_activity)>(),
     decltype(drb_activity_item_s::drb_activity)::has_ext,
     false,
     decltype(drb_activity_item_s::drb_activity)::nof_types,
     decltype(drb_activity_item_s::drb_activity)::nof_exts,
     nullptr,
     &drb_activity_opts::desc},
    {offsetof(drb_activity_item_s, ie_exts),
     (int32_t)offsetof(drb_activity_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_activity_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_activity_item_s_names[] = {{"dRB-ID", nullptr},
                                                                              {"dRB-Activity", nullptr},
                                                                              {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_drb_activity_item_s_opts[]  = {
    {2, (int32_t)offsetof(drb_activity_item_s, ie_exts_present)}};
const asn1::table::seq_desc drb_activity_item_s::desc = {
    {seq_table_drb_activity_item_s_fields, 3, UINT64_C(0x3), seq_table_drb_activity_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_activity_item_s_names};

// PDU-Session-Resource-Activity ::= ENUMERATED
static const char*           enum_names_pdu_session_res_activity_e_values[] = {"active", "not-active"};
const asn1::table::enum_desc pdu_session_res_activity_opts::desc = {enum_names_pdu_session_res_activity_e_values,
                                                                    2,
                                                                    "pdu_session_res_activity_e"};

// PDU-Session-Resource-Activity-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_session_res_activity_item_s_fields[] = {
    {offsetof(pdu_session_res_activity_item_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pdu_session_res_activity_item_s, pdu_session_res_activity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pdu_session_res_activity_item_s::pdu_session_res_activity)>(),
     decltype(pdu_session_res_activity_item_s::pdu_session_res_activity)::has_ext,
     false,
     decltype(pdu_session_res_activity_item_s::pdu_session_res_activity)::nof_types,
     decltype(pdu_session_res_activity_item_s::pdu_session_res_activity)::nof_exts,
     nullptr,
     &pdu_session_res_activity_opts::desc},
    {offsetof(pdu_session_res_activity_item_s, ie_exts),
     (int32_t)offsetof(pdu_session_res_activity_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pdu_session_res_activity_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pdu_session_res_activity_item_s_names[] = {
    {"pDU-Session-ID", nullptr},
    {"pDU-Session-Resource-Activity", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pdu_session_res_activity_item_s_opts[] = {
    {2, (int32_t)offsetof(pdu_session_res_activity_item_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_session_res_activity_item_s::desc = {{seq_table_pdu_session_res_activity_item_s_fields,
                                                                      3,
                                                                      UINT64_C(0x3),
                                                                      seq_table_pdu_session_res_activity_item_s_opts,
                                                                      1},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_pdu_session_res_activity_item_s_names};

// UE-Activity ::= ENUMERATED
static const char*           enum_names_ue_activity_e_values[] = {"active", "not-active"};
const asn1::table::enum_desc ue_activity_opts::desc            = {enum_names_ue_activity_e_values, 2, "ue_activity_e"};

// ActivityInformation ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_activity_info_c_alternatives[] = {
    &asn1::table::alternative_ops<drb_activity_list_l, false>::value,
    &asn1::table::alternative_ops<pdu_session_res_activity_list_l, false>::value,
    &asn1::table::alternative_ops<ue_activity_e, true>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<activity_info_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc activity_info_c::storage_desc = {choice_storage_activity_info_c_alternatives,
                                                                        4,
                                                                        "activity_info_c"};

drb_activity_list_l& activity_info_c::set_drb_activity_list()
{
  set(types::drb_activity_list);
  return c.get<drb_activity_list_l>();
}
pdu_session_res_activity_list_l& activity_info_c::set_pdu_session_res_activity_list()
{
  set(types::pdu_session_res_activity_list);
  return c.get<pdu_session_res_activity_list_l>();
}
ue_activity_e& activity_info_c::set_ue_activity()
{
  set(types::ue_activity);
  return c.get<ue_activity_e>();
}
protocol_ie_single_container_s<activity_info_ext_ies_o>& activity_info_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<activity_info_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_activity_info_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::list_codec<drb_activity_list_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     256,
     &asn1::table::list_codec<pdu_session_res_activity_list_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<ue_activity_e>(),
     ue_activity_e::has_ext,
     false,
     ue_activity_e::nof_types,
     ue_activity_e::nof_exts,
     nullptr,
     &ue_activity_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<activity_info_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_activity_info_c_names[] = {
    {"dRB-Activity-List", nullptr},
    {"pDU-Session-Resource-Activity-List", nullptr},
    {"uE-Activity", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc activity_info_c::desc = {asn1::table::enum_int_type<activity_info_c::types>(),
                                                        activity_info_c::types::nof_types,
                                                        activity_info_c::types::nof_exts,
                                                        activity_info_c::types::has_ext,
                                                        true,
                                                        choice_table_activity_info_c_alternatives,
                                                        nullptr,
                                                        choice_table_activity_info_c_names,
                                                        "activity_info_c",
                                                        offsetof(activity_info_c, c.type),
                                                        offsetof(activity_info_c, c.value),
                                                        false,
                                                        &activity_info_c::storage_desc};

static const char*           enum_names_activity_info_c_types_values[] = {"dRB-Activity-List",
                                                                          "pDU-Session-Resource-Activity-List",
                                                                          "uE-Activity",
                                                                          "choice-extension"};
const asn1::table::enum_desc activity_info_c::types_opts::desc         = {enum_names_activity_info_c_types_values,
                                                                          4,
                                                                          "activity_info_c::types"};

// ActivityNotificationLevel ::= ENUMERATED
static const char*           enum_names_activity_notif_level_e_values[] = {"drb", "pdu-session", "ue"};
const asn1::table::enum_desc activity_notif_level_opts::desc            = {enum_names_activity_notif_level_e_values,
                                                                           3,
                                                                           "activity_notif_level_e"};

// AdditionalHandoverInfo ::= ENUMERATED
static const char*           enum_names_add_ho_info_e_values[] = {"discard-pdpc-SN"};
const asn1::table::enum_desc add_ho_info_opts::desc            = {enum_names_add_ho_info_e_values, 1, "add_ho_info_e"};

// AdditionalPDCPduplicationInformation ::= ENUMERATED
static const char*           enum_names_add_pdcp_dupl_info_e_values[] = {"three", "four"};
const asn1::table::enum_desc add_pdcp_dupl_info_opts::desc            = {enum_names_add_pdcp_dupl_info_e_values,
                                                                         2,
                                                                         "add_pdcp_dupl_info_e"};
uint8_t                      add_pdcp_dupl_info_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 4};
  return map_enum_number(numbers, 2, value, "add_pdcp_dupl_info_e");
}

// PacketErrorRate ::= SEQUENCE
static const asn1::table::seq_field seq_table_packet_error_rate_s_fields[] = {
    {offsetof(packet_error_rate_s, per_scalar),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(0),
     UINT64_C(9),
     nullptr},
    {offsetof(packet_error_rate_s, per_exponent),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(0),
     UINT64_C(9),
     nullptr},
    {offsetof(packet_error_rate_s, ie_exts),
     (int32_t)offsetof(packet_error_rate_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(packet_error_rate_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_packet_error_rate_s_names[] = {{"pER-Scalar", nullptr},
                                                                              {"pER-Exponent", nullptr},
                                                                              {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_packet_error_rate_s_opts[]  = {
    {2, (int32_t)offsetof(packet_error_rate_s, ie_exts_present)}};
const asn1::table::seq_desc packet_error_rate_s::desc = {
    {seq_table_packet_error_rate_s_fields, 3, UINT64_C(0x3), seq_table_packet_error_rate_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_packet_error_rate_s_names};

static const uint32_t objset_id_alt_qos_para_set_item_ext_ies_o[]   = {216};
static const uint8_t  objset_crit_alt_qos_para_set_item_ext_ies_o[] = {crit_e::ignore};

// AlternativeQoSParaSetItem-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_alt_qos_para_set_item_ext_ies_container_values[] = {
    {offsetof(alt_qos_para_set_item_ext_ies_container, max_data_burst_volume),
     (int32_t)offsetof(alt_qos_para_set_item_ext_ies_container, max_data_burst_volume_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(4095),
     nullptr}};
static const int32_t ie_table_alt_qos_para_set_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(alt_qos_para_set_item_ext_ies_container, max_data_burst_volume_present)};
const asn1::table::ie_container_desc alt_qos_para_set_item_ext_ies_container::desc = {
    objset_id_alt_qos_para_set_item_ext_ies_o,
    objset_crit_alt_qos_para_set_item_ext_ies_o,
    ie_table_alt_qos_para_set_item_ext_ies_container_values,
    ie_table_alt_qos_para_set_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// AlternativeQoSParaSetItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_alt_qos_para_set_item_s_fields[] = {
    {offsetof(alt_qos_para_set_item_s, alt_qos_param_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(8),
     nullptr},
    {offsetof(alt_qos_para_set_item_s, guaranteed_flow_bit_rate_dl),
     (int32_t)offsetof(alt_qos_para_set_item_s, guaranteed_flow_bit_rate_dl_present),
     asn1::table::field_op::int_u64,
     true,
     true,
     INT64_C(0),
     UINT64_C(4000000000000),
     nullptr},
    {offsetof(alt_qos_para_set_item_s, guaranteed_flow_bit_rate_ul),
     (int32_t)offsetof(alt_qos_para_set_item_s, guaranteed_flow_bit_rate_ul_present),
     asn1::table::field_op::int_u64,
     true,
     true,
     INT64_C(0),
     UINT64_C(4000000000000),
     nullptr},
    {offsetof(alt_qos_para_set_item_s, packet_delay_budget),
     (int32_t)offsetof(alt_qos_para_set_item_s, packet_delay_budget_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {offsetof(alt_qos_para_set_item_s, packet_error_rate),
     (int32_t)offsetof(alt_qos_para_set_item_s, packet_error_rate_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &packet_error_rate_s::desc},
    {offsetof(alt_qos_para_set_item_s, ie_exts),
     (int32_t)offsetof(alt_qos_para_set_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &alt_qos_para_set_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_alt_qos_para_set_item_s_names[] = {
    {"alternativeQoSParameterIndex", nullptr},
    {"guaranteedFlowBitRateDL", nullptr},
    {"guaranteedFlowBitRateUL", nullptr},
    {"packetDelayBudget", nullptr},
    {"packetErrorRate", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_alt_qos_para_set_item_s_opts[] = {
    {1, (int32_t)offsetof(alt_qos_para_set_item_s, guaranteed_flow_bit_rate_dl_present)},
    {2, (int32_t)offsetof(alt_qos_para_set_item_s, guaranteed_flow_bit_rate_ul_present)},
    {3, (int32_t)offsetof(alt_qos_para_set_item_s, packet_delay_budget_present)},
    {4, (int32_t)offsetof(alt_qos_para_set_item_s, packet_error_rate_present)},
    {5, (int32_t)offsetof(alt_qos_para_set_item_s, ie_exts_present)}};
const asn1::table::seq_desc alt_qos_para_set_item_s::desc = {
    {seq_table_alt_qos_para_set_item_s_fields, 6, UINT64_C(0x1), seq_table_alt_qos_para_set_item_s_opts, 5},
    true,
    true,
    nullptr,
    0,
    seq_table_alt_qos_para_set_item_s_names};

// GTPTunnel ::= SEQUENCE
static const asn1::table::seq_field seq_table_gtp_tunnel_s_fields[] = {
    {offsetof(gtp_tunnel_s, transport_layer_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gtp_tunnel_s::transport_layer_address), asn1::table::struct_json>::value},
    {offsetof(gtp_tunnel_s, gtp_teid),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gtp_tunnel_s::gtp_teid), asn1::table::string_json>::value},
    {offsetof(gtp_tunnel_s, ie_exts),
     (int32_t)offsetof(gtp_tunnel_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gtp_tunnel_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_gtp_tunnel_s_names[] = {{"transportLayerAddress", nullptr},
                                                                       {"gTP-TEID", nullptr},
                                                                       {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_gtp_tunnel_s_opts[]  = {
    {2, (int32_t)offsetof(gtp_tunnel_s, ie_exts_present)}};
const asn1::table::seq_desc gtp_tunnel_s::desc = {
    {seq_table_gtp_tunnel_s_fields, 3, UINT64_C(0x3), seq_table_gtp_tunnel_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_gtp_tunnel_s_names};

// UP-TNL-Information ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_up_tnl_info_c_alternatives[] = {
    &asn1::table::alternative_ops<gtp_tunnel_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<up_tnl_info_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc up_tnl_info_c::storage_desc = {choice_storage_up_tnl_info_c_alternatives,
                                                                      2,
                                                                      "up_tnl_info_c"};

gtp_tunnel_s& up_tnl_info_c::set_gtp_tunnel()
{
  set(types::gtp_tunnel);
  return c.get<gtp_tunnel_s>();
}
protocol_ie_single_container_s<up_tnl_info_ext_ies_o>& up_tnl_info_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<up_tnl_info_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_up_tnl_info_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gtp_tunnel_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<up_tnl_info_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_up_tnl_info_c_names[] = {{"gTPTunnel", nullptr},
                                                                           {"choice-extension", nullptr}};
const asn1::table::choice_desc       up_tnl_info_c::desc = {asn1::table::enum_int_type<up_tnl_info_c::types>(),
                                                            up_tnl_info_c::types::nof_types,
                                                            up_tnl_info_c::types::nof_exts,
                                                            up_tnl_info_c::types::has_ext,
                                                            true,
                                                            choice_table_up_tnl_info_c_alternatives,
                                                            nullptr,
                                                            choice_table_up_tnl_info_c_names,
                                                            "up_tnl_info_c",
                                                            offsetof(up_tnl_info_c, c.type),
                                                            offsetof(up_tnl_info_c, c.value),
                                                            false,
                                                            &up_tnl_info_c::storage_desc};

static const char*           enum_names_up_tnl_info_c_types_values[] = {"gTPTunnel", "choice-extension"};
const asn1::table::enum_desc up_tnl_info_c::types_opts::desc         = {enum_names_up_tnl_info_c_types_values,
                                                                        2,
                                                                        "up_tnl_info_c::types"};

// MBSF1UInformationAtCU ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbsf1_u_info_at_cu_s_fields[] = {
    {offsetof(mbsf1_u_info_at_cu_s, mbs_f1u_info_at_cu),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(mbsf1_u_info_at_cu_s, ie_exts),
     (int32_t)offsetof(mbsf1_u_info_at_cu_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbsf1_u_info_at_cu_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mbsf1_u_info_at_cu_s_names[] = {{"mbs-f1u-info-at-CU", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mbsf1_u_info_at_cu_s_opts[]  = {
    {1, (int32_t)offsetof(mbsf1_u_info_at_cu_s, ie_exts_present)}};
const asn1::table::seq_desc mbsf1_u_info_at_cu_s::desc = {
    {seq_table_mbsf1_u_info_at_cu_s_fields, 2, UINT64_C(0x1), seq_table_mbsf1_u_info_at_cu_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_mbsf1_u_info_at_cu_s_names};

// LocationDependentMBSF1UInformationAtCU-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_location_dependent_mbsf1_u_info_at_cu_item_s_fields[] = {
    {offsetof(location_dependent_mbsf1_u_info_at_cu_item_s, mbs_area_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(location_dependent_mbsf1_u_info_at_cu_item_s, mbs_f1u_info_at_cu),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(location_dependent_mbsf1_u_info_at_cu_item_s, ie_exts),
     (int32_t)offsetof(location_dependent_mbsf1_u_info_at_cu_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(location_dependent_mbsf1_u_info_at_cu_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_location_dependent_mbsf1_u_info_at_cu_item_s_names[] = {
    {"mbsAreaSession-ID", nullptr},
    {"mbs-f1u-info-at-CU", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_location_dependent_mbsf1_u_info_at_cu_item_s_opts[] = {
    {2, (int32_t)offsetof(location_dependent_mbsf1_u_info_at_cu_item_s, ie_exts_present)}};
const asn1::table::seq_desc location_dependent_mbsf1_u_info_at_cu_item_s::desc = {
    {seq_table_location_dependent_mbsf1_u_info_at_cu_item_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_location_dependent_mbsf1_u_info_at_cu_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_location_dependent_mbsf1_u_info_at_cu_item_s_names};

// BCBearerContextF1U-TNLInfoatCU ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_bc_bearer_context_f1_u_tnl_infoat_cu_c_alternatives[] = {
    &asn1::table::alternative_ops<mbsf1_u_info_at_cu_s, false>::value,
    &asn1::table::alternative_ops<location_dependent_mbsf1_u_info_at_cu_l, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_cu_ext_ies_o>,
                                  false>::value};
const asn1::table::choice_storage_desc bc_bearer_context_f1_u_tnl_infoat_cu_c::storage_desc = {
    choice_storage_bc_bearer_context_f1_u_tnl_infoat_cu_c_alternatives,
    3,
    "bc_bearer_context_f1_u_tnl_infoat_cu_c"};

mbsf1_u_info_at_cu_s& bc_bearer_context_f1_u_tnl_infoat_cu_c::set_locationindependent()
{
  set(types::locationindependent);
  return c.get<mbsf1_u_info_at_cu_s>();
}
location_dependent_mbsf1_u_info_at_cu_l& bc_bearer_context_f1_u_tnl_infoat_cu_c::set_locationdependent()
{
  set(types::locationdependent);
  return c.get<location_dependent_mbsf1_u_info_at_cu_l>();
}
protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_cu_ext_ies_o>&
bc_bearer_context_f1_u_tnl_infoat_cu_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_cu_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_bc_bearer_context_f1_u_tnl_infoat_cu_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbsf1_u_info_at_cu_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     256,
     &asn1::table::list_codec<location_dependent_mbsf1_u_info_at_cu_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_cu_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_bc_bearer_context_f1_u_tnl_infoat_cu_c_names[] = {
    {"locationindependent", nullptr},
    {"locationdependent", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc bc_bearer_context_f1_u_tnl_infoat_cu_c::desc = {
    asn1::table::enum_int_type<bc_bearer_context_f1_u_tnl_infoat_cu_c::types>(),
    bc_bearer_context_f1_u_tnl_infoat_cu_c::types::nof_types,
    bc_bearer_context_f1_u_tnl_infoat_cu_c::types::nof_exts,
    bc_bearer_context_f1_u_tnl_infoat_cu_c::types::has_ext,
    true,
    choice_table_bc_bearer_context_f1_u_tnl_infoat_cu_c_alternatives,
    nullptr,
    choice_table_bc_bearer_context_f1_u_tnl_infoat_cu_c_names,
    "bc_bearer_context_f1_u_tnl_infoat_cu_c",
    offsetof(bc_bearer_context_f1_u_tnl_infoat_cu_c, c.type),
    offsetof(bc_bearer_context_f1_u_tnl_infoat_cu_c, c.value),
    false,
    &bc_bearer_context_f1_u_tnl_infoat_cu_c::storage_desc};

static const char*           enum_names_bc_bearer_context_f1_u_tnl_infoat_cu_c_types_values[] = {"locationindependent",
                                                                                                 "locationdependent",
                                                                                                 "choice-extension"};
const asn1::table::enum_desc bc_bearer_context_f1_u_tnl_infoat_cu_c::types_opts::desc         = {
    enum_names_bc_bearer_context_f1_u_tnl_infoat_cu_c_types_values,
    3,
    "bc_bearer_context_f1_u_tnl_infoat_cu_c::types"};

// F1UTunnelNotEstablished ::= ENUMERATED
static const char*           enum_names_f1_u_tunnel_not_established_e_values[] = {"true"};
const asn1::table::enum_desc f1_u_tunnel_not_established_opts::desc = {enum_names_f1_u_tunnel_not_established_e_values,
                                                                       1,
                                                                       "f1_u_tunnel_not_established_e"};

static const uint32_t objset_id_mbsf1_u_info_at_du_ext_ies_o[]   = {207};
static const uint8_t  objset_crit_mbsf1_u_info_at_du_ext_ies_o[] = {crit_e::ignore};

// MBSF1UInformationAtDU-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_mbsf1_u_info_at_du_ext_ies_container_values[] = {
    {offsetof(mbsf1_u_info_at_du_ext_ies_container, f1_u_tunnel_not_established),
     (int32_t)offsetof(mbsf1_u_info_at_du_ext_ies_container, f1_u_tunnel_not_established_present),
     asn1::table::enum_op<decltype(mbsf1_u_info_at_du_ext_ies_container::f1_u_tunnel_not_established)>(),
     decltype(mbsf1_u_info_at_du_ext_ies_container::f1_u_tunnel_not_established)::has_ext,
     false,
     decltype(mbsf1_u_info_at_du_ext_ies_container::f1_u_tunnel_not_established)::nof_types,
     decltype(mbsf1_u_info_at_du_ext_ies_container::f1_u_tunnel_not_established)::nof_exts,
     nullptr,
     &f1_u_tunnel_not_established_opts::desc}};
static const int32_t ie_table_mbsf1_u_info_at_du_ext_ies_container_presences[] = {
    (int32_t)offsetof(mbsf1_u_info_at_du_ext_ies_container, f1_u_tunnel_not_established_present)};
const asn1::table::ie_container_desc mbsf1_u_info_at_du_ext_ies_container::desc = {
    objset_id_mbsf1_u_info_at_du_ext_ies_o,
    objset_crit_mbsf1_u_info_at_du_ext_ies_o,
    ie_table_mbsf1_u_info_at_du_ext_ies_container_values,
    ie_table_mbsf1_u_info_at_du_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// MBSF1UInformationAtDU ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbsf1_u_info_at_du_s_fields[] = {
    {offsetof(mbsf1_u_info_at_du_s, mbs_f1u_info_at_du),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(mbsf1_u_info_at_du_s, ie_exts),
     (int32_t)offsetof(mbsf1_u_info_at_du_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &mbsf1_u_info_at_du_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_mbsf1_u_info_at_du_s_names[] = {{"mbs-f1u-info-at-DU", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mbsf1_u_info_at_du_s_opts[]  = {
    {1, (int32_t)offsetof(mbsf1_u_info_at_du_s, ie_exts_present)}};
const asn1::table::seq_desc mbsf1_u_info_at_du_s::desc = {
    {seq_table_mbsf1_u_info_at_du_s_fields, 2, UINT64_C(0x1), seq_table_mbsf1_u_info_at_du_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_mbsf1_u_info_at_du_s_names};

static const uint32_t objset_id_location_dependent_mbsf1_u_info_at_du_item_ext_ies_o[]   = {207};
static const uint8_t  objset_crit_location_dependent_mbsf1_u_info_at_du_item_ext_ies_o[] = {crit_e::ignore};

// LocationDependentMBSF1UInformationAtDU-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_location_dependent_mbsf1_u_info_at_du_item_ext_ies_container_values[] = {
    {offsetof(location_dependent_mbsf1_u_info_at_du_item_ext_ies_container, f1_u_tunnel_not_established),
     (int32_t)offsetof(location_dependent_mbsf1_u_info_at_du_item_ext_ies_container,
                       f1_u_tunnel_not_established_present),
     asn1::table::enum_op<
         decltype(location_dependent_mbsf1_u_info_at_du_item_ext_ies_container::f1_u_tunnel_not_established)>(),
     decltype(location_dependent_mbsf1_u_info_at_du_item_ext_ies_container::f1_u_tunnel_not_established)::has_ext,
     false,
     decltype(location_dependent_mbsf1_u_info_at_du_item_ext_ies_container::f1_u_tunnel_not_established)::nof_types,
     decltype(location_dependent_mbsf1_u_info_at_du_item_ext_ies_container::f1_u_tunnel_not_established)::nof_exts,
     nullptr,
     &f1_u_tunnel_not_established_opts::desc}};
static const int32_t ie_table_location_dependent_mbsf1_u_info_at_du_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(location_dependent_mbsf1_u_info_at_du_item_ext_ies_container,
                      f1_u_tunnel_not_established_present)};
const asn1::table::ie_container_desc location_dependent_mbsf1_u_info_at_du_item_ext_ies_container::desc = {
    objset_id_location_dependent_mbsf1_u_info_at_du_item_ext_ies_o,
    objset_crit_location_dependent_mbsf1_u_info_at_du_item_ext_ies_o,
    ie_table_location_dependent_mbsf1_u_info_at_du_item_ext_ies_container_values,
    ie_table_location_dependent_mbsf1_u_info_at_du_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// LocationDependentMBSF1UInformationAtDU-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_location_dependent_mbsf1_u_info_at_du_item_s_fields[] = {
    {offsetof(location_dependent_mbsf1_u_info_at_du_item_s, mbs_area_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(location_dependent_mbsf1_u_info_at_du_item_s, mbs_f1u_info_at_du),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(location_dependent_mbsf1_u_info_at_du_item_s, ie_exts),
     (int32_t)offsetof(location_dependent_mbsf1_u_info_at_du_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &location_dependent_mbsf1_u_info_at_du_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_location_dependent_mbsf1_u_info_at_du_item_s_names[] = {
    {"mbsAreaSession-ID", nullptr},
    {"mbs-f1u-info-at-DU", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_location_dependent_mbsf1_u_info_at_du_item_s_opts[] = {
    {2, (int32_t)offsetof(location_dependent_mbsf1_u_info_at_du_item_s, ie_exts_present)}};
const asn1::table::seq_desc location_dependent_mbsf1_u_info_at_du_item_s::desc = {
    {seq_table_location_dependent_mbsf1_u_info_at_du_item_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_location_dependent_mbsf1_u_info_at_du_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_location_dependent_mbsf1_u_info_at_du_item_s_names};

// BCBearerContextF1U-TNLInfoatDU ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_bc_bearer_context_f1_u_tnl_infoat_du_c_alternatives[] = {
    &asn1::table::alternative_ops<mbsf1_u_info_at_du_s, false>::value,
    &asn1::table::alternative_ops<location_dependent_mbsf1_u_info_at_du_l, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_du_ext_ies_o>,
                                  false>::value};
const asn1::table::choice_storage_desc bc_bearer_context_f1_u_tnl_infoat_du_c::storage_desc = {
    choice_storage_bc_bearer_context_f1_u_tnl_infoat_du_c_alternatives,
    3,
    "bc_bearer_context_f1_u_tnl_infoat_du_c"};

mbsf1_u_info_at_du_s& bc_bearer_context_f1_u_tnl_infoat_du_c::set_locationindependent()
{
  set(types::locationindependent);
  return c.get<mbsf1_u_info_at_du_s>();
}
location_dependent_mbsf1_u_info_at_du_l& bc_bearer_context_f1_u_tnl_infoat_du_c::set_locationdependent()
{
  set(types::locationdependent);
  return c.get<location_dependent_mbsf1_u_info_at_du_l>();
}
protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_du_ext_ies_o>&
bc_bearer_context_f1_u_tnl_infoat_du_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_du_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_bc_bearer_context_f1_u_tnl_infoat_du_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbsf1_u_info_at_du_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     256,
     &asn1::table::list_codec<location_dependent_mbsf1_u_info_at_du_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<bc_bearer_context_f1_u_tnl_infoat_du_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_bc_bearer_context_f1_u_tnl_infoat_du_c_names[] = {
    {"locationindependent", nullptr},
    {"locationdependent", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc bc_bearer_context_f1_u_tnl_infoat_du_c::desc = {
    asn1::table::enum_int_type<bc_bearer_context_f1_u_tnl_infoat_du_c::types>(),
    bc_bearer_context_f1_u_tnl_infoat_du_c::types::nof_types,
    bc_bearer_context_f1_u_tnl_infoat_du_c::types::nof_exts,
    bc_bearer_context_f1_u_tnl_infoat_du_c::types::has_ext,
    true,
    choice_table_bc_bearer_context_f1_u_tnl_infoat_du_c_alternatives,
    nullptr,
    choice_table_bc_bearer_context_f1_u_tnl_infoat_du_c_names,
    "bc_bearer_context_f1_u_tnl_infoat_du_c",
    offsetof(bc_bearer_context_f1_u_tnl_infoat_du_c, c.type),
    offsetof(bc_bearer_context_f1_u_tnl_infoat_du_c, c.value),
    false,
    &bc_bearer_context_f1_u_tnl_infoat_du_c::storage_desc};

static const char*           enum_names_bc_bearer_context_f1_u_tnl_infoat_du_c_types_values[] = {"locationindependent",
                                                                                                 "locationdependent",
                                                                                                 "choice-extension"};
const asn1::table::enum_desc bc_bearer_context_f1_u_tnl_infoat_du_c::types_opts::desc         = {
    enum_names_bc_bearer_context_f1_u_tnl_infoat_du_c_types_values,
    3,
    "bc_bearer_context_f1_u_tnl_infoat_du_c::types"};

// MBSNGUInformationAt5GC-Multicast ::= SEQUENCE
static const asn1::table::seq_field seq_table_mb_sn_gu_info_at5_gc_multicast_s_fields[] = {
    {offsetof(mb_sn_gu_info_at5_gc_multicast_s, ipmc_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mb_sn_gu_info_at5_gc_multicast_s::ipmc_address),
                                asn1::table::struct_json>::value},
    {offsetof(mb_sn_gu_info_at5_gc_multicast_s, ipsource_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mb_sn_gu_info_at5_gc_multicast_s::ipsource_address),
                                asn1::table::struct_json>::value},
    {offsetof(mb_sn_gu_info_at5_gc_multicast_s, gtp_dl_teid),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mb_sn_gu_info_at5_gc_multicast_s::gtp_dl_teid),
                                asn1::table::string_json>::value},
    {offsetof(mb_sn_gu_info_at5_gc_multicast_s, ie_exts),
     (int32_t)offsetof(mb_sn_gu_info_at5_gc_multicast_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mb_sn_gu_info_at5_gc_multicast_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mb_sn_gu_info_at5_gc_multicast_s_names[] = {{"ipmcAddress", nullptr},
                                                                                           {"ipsourceAddress", nullptr},
                                                                                           {"gtpDLTEID", nullptr},
                                                                                           {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mb_sn_gu_info_at5_gc_multicast_s_opts[]  = {
    {3, (int32_t)offsetof(mb_sn_gu_info_at5_gc_multicast_s, ie_exts_present)}};
const asn1::table::seq_desc mb_sn_gu_info_at5_gc_multicast_s::desc = {
    {seq_table_mb_sn_gu_info_at5_gc_multicast_s_fields,
     4,
     UINT64_C(0x7),
     seq_table_mb_sn_gu_info_at5_gc_multicast_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_mb_sn_gu_info_at5_gc_multicast_s_names};

// MBSNGUInformationAt5GC ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_mb_sn_gu_info_at5_gc_c_alternatives[] = {
    &asn1::table::alternative_ops<mb_sn_gu_info_at5_gc_multicast_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<mb_sn_gu_info_at5_gc_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc mb_sn_gu_info_at5_gc_c::storage_desc = {
    choice_storage_mb_sn_gu_info_at5_gc_c_alternatives,
    2,
    "mb_sn_gu_info_at5_gc_c"};

mb_sn_gu_info_at5_gc_multicast_s& mb_sn_gu_info_at5_gc_c::set_multicast()
{
  set(types::multicast);
  return c.get<mb_sn_gu_info_at5_gc_multicast_s>();
}
protocol_ie_single_container_s<mb_sn_gu_info_at5_gc_ext_ies_o>& mb_sn_gu_info_at5_gc_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<mb_sn_gu_info_at5_gc_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_mb_sn_gu_info_at5_gc_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mb_sn_gu_info_at5_gc_multicast_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<mb_sn_gu_info_at5_gc_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_mb_sn_gu_info_at5_gc_c_names[] = {{"multicast", nullptr},
                                                                                    {"choice-extension", nullptr}};
const asn1::table::choice_desc       mb_sn_gu_info_at5_gc_c::desc                = {
    asn1::table::enum_int_type<mb_sn_gu_info_at5_gc_c::types>(),
    mb_sn_gu_info_at5_gc_c::types::nof_types,
    mb_sn_gu_info_at5_gc_c::types::nof_exts,
    mb_sn_gu_info_at5_gc_c::types::has_ext,
    true,
    choice_table_mb_sn_gu_info_at5_gc_c_alternatives,
    nullptr,
    choice_table_mb_sn_gu_info_at5_gc_c_names,
    "mb_sn_gu_info_at5_gc_c",
    offsetof(mb_sn_gu_info_at5_gc_c, c.type),
    offsetof(mb_sn_gu_info_at5_gc_c, c.value),
    false,
    &mb_sn_gu_info_at5_gc_c::storage_desc};

static const char*           enum_names_mb_sn_gu_info_at5_gc_c_types_values[] = {"multicast", "choice-extension"};
const asn1::table::enum_desc mb_sn_gu_info_at5_gc_c::types_opts::desc = {enum_names_mb_sn_gu_info_at5_gc_c_types_values,
                                                                         2,
                                                                         "mb_sn_gu_info_at5_gc_c::types"};

// LocationDependentMBSNGUInformationAt5GC-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_location_dependent_mb_sn_gu_info_at5_gc_item_s_fields[] = {
    {offsetof(location_dependent_mb_sn_gu_info_at5_gc_item_s, mbs_area_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(location_dependent_mb_sn_gu_info_at5_gc_item_s, mbs_ngu_info_at5_gc),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mb_sn_gu_info_at5_gc_c::desc},
    {offsetof(location_dependent_mb_sn_gu_info_at5_gc_item_s, ie_exts),
     (int32_t)offsetof(location_dependent_mb_sn_gu_info_at5_gc_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(location_dependent_mb_sn_gu_info_at5_gc_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_location_dependent_mb_sn_gu_info_at5_gc_item_s_names[] = {
    {"mbsAreaSession-ID", nullptr},
    {"mbsNGUInformationAt5GC", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_location_dependent_mb_sn_gu_info_at5_gc_item_s_opts[] = {
    {2, (int32_t)offsetof(location_dependent_mb_sn_gu_info_at5_gc_item_s, ie_exts_present)}};
const asn1::table::seq_desc location_dependent_mb_sn_gu_info_at5_gc_item_s::desc = {
    {seq_table_location_dependent_mb_sn_gu_info_at5_gc_item_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_location_dependent_mb_sn_gu_info_at5_gc_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_location_dependent_mb_sn_gu_info_at5_gc_item_s_names};

// BCBearerContextNGU-TNLInfoat5GC ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_bc_bearer_context_ngu_tnl_infoat5_gc_c_alternatives[] = {
    &asn1::table::alternative_ops<mb_sn_gu_info_at5_gc_c, false>::value,
    &asn1::table::alternative_ops<location_dependent_mb_sn_gu_info_at5_gc_l, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat5_gc_ext_ies_o>,
                                  false>::value};
const asn1::table::choice_storage_desc bc_bearer_context_ngu_tnl_infoat5_gc_c::storage_desc = {
    choice_storage_bc_bearer_context_ngu_tnl_infoat5_gc_c_alternatives,
    3,
    "bc_bearer_context_ngu_tnl_infoat5_gc_c"};

mb_sn_gu_info_at5_gc_c& bc_bearer_context_ngu_tnl_infoat5_gc_c::set_locationindependent()
{
  set(types::locationindependent);
  return c.get<mb_sn_gu_info_at5_gc_c>();
}
location_dependent_mb_sn_gu_info_at5_gc_l& bc_bearer_context_ngu_tnl_infoat5_gc_c::set_locationdependent()
{
  set(types::locationdependent);
  return c.get<location_dependent_mb_sn_gu_info_at5_gc_l>();
}
protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat5_gc_ext_ies_o>&
bc_bearer_context_ngu_tnl_infoat5_gc_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat5_gc_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_bc_bearer_context_ngu_tnl_infoat5_gc_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mb_sn_gu_info_at5_gc_c::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     256,
     &asn1::table::list_codec<location_dependent_mb_sn_gu_info_at5_gc_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat5_gc_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_bc_bearer_context_ngu_tnl_infoat5_gc_c_names[] = {
    {"locationindependent", nullptr},
    {"locationdependent", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc bc_bearer_context_ngu_tnl_infoat5_gc_c::desc = {
    asn1::table::enum_int_type<bc_bearer_context_ngu_tnl_infoat5_gc_c::types>(),
    bc_bearer_context_ngu_tnl_infoat5_gc_c::types::nof_types,
    bc_bearer_context_ngu_tnl_infoat5_gc_c::types::nof_exts,
    bc_bearer_context_ngu_tnl_infoat5_gc_c::types::has_ext,
    true,
    choice_table_bc_bearer_context_ngu_tnl_infoat5_gc_c_alternatives,
    nullptr,
    choice_table_bc_bearer_context_ngu_tnl_infoat5_gc_c_names,
    "bc_bearer_context_ngu_tnl_infoat5_gc_c",
    offsetof(bc_bearer_context_ngu_tnl_infoat5_gc_c, c.type),
    offsetof(bc_bearer_context_ngu_tnl_infoat5_gc_c, c.value),
    false,
    &bc_bearer_context_ngu_tnl_infoat5_gc_c::storage_desc};

static const char*           enum_names_bc_bearer_context_ngu_tnl_infoat5_gc_c_types_values[] = {"locationindependent",
                                                                                                 "locationdependent",
                                                                                                 "choice-extension"};
const asn1::table::enum_desc bc_bearer_context_ngu_tnl_infoat5_gc_c::types_opts::desc         = {
    enum_names_bc_bearer_context_ngu_tnl_infoat5_gc_c_types_values,
    3,
    "bc_bearer_context_ngu_tnl_infoat5_gc_c::types"};

// MBSNGUInformationAtNGRAN ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_mb_sn_gu_info_at_ngran_c_alternatives[] = {
    &asn1::table::alternative_ops<up_tnl_info_c, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<mb_sn_gu_info_at_ngran_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc mb_sn_gu_info_at_ngran_c::storage_desc = {
    choice_storage_mb_sn_gu_info_at_ngran_c_alternatives,
    2,
    "mb_sn_gu_info_at_ngran_c"};

up_tnl_info_c& mb_sn_gu_info_at_ngran_c::set_unicast()
{
  set(types::unicast);
  return c.get<up_tnl_info_c>();
}
protocol_ie_single_container_s<mb_sn_gu_info_at_ngran_ext_ies_o>& mb_sn_gu_info_at_ngran_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<mb_sn_gu_info_at_ngran_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_mb_sn_gu_info_at_ngran_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<mb_sn_gu_info_at_ngran_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_mb_sn_gu_info_at_ngran_c_names[] = {{"unicast", nullptr},
                                                                                      {"choice-extension", nullptr}};
const asn1::table::choice_desc       mb_sn_gu_info_at_ngran_c::desc                = {
    asn1::table::enum_int_type<mb_sn_gu_info_at_ngran_c::types>(),
    mb_sn_gu_info_at_ngran_c::types::nof_types,
    mb_sn_gu_info_at_ngran_c::types::nof_exts,
    mb_sn_gu_info_at_ngran_c::types::has_ext,
    true,
    choice_table_mb_sn_gu_info_at_ngran_c_alternatives,
    nullptr,
    choice_table_mb_sn_gu_info_at_ngran_c_names,
    "mb_sn_gu_info_at_ngran_c",
    offsetof(mb_sn_gu_info_at_ngran_c, c.type),
    offsetof(mb_sn_gu_info_at_ngran_c, c.value),
    false,
    &mb_sn_gu_info_at_ngran_c::storage_desc};

static const char*           enum_names_mb_sn_gu_info_at_ngran_c_types_values[] = {"unicast", "choice-extension"};
const asn1::table::enum_desc mb_sn_gu_info_at_ngran_c::types_opts::desc         = {
    enum_names_mb_sn_gu_info_at_ngran_c_types_values,
    2,
    "mb_sn_gu_info_at_ngran_c::types"};

// LocationDependentMBSNGUInformationAtNGRAN-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_location_dependent_mb_sn_gu_info_at_ngran_item_s_fields[] = {
    {offsetof(location_dependent_mb_sn_gu_info_at_ngran_item_s, mbs_area_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(location_dependent_mb_sn_gu_info_at_ngran_item_s, mbs_ngu_info_at_ngran),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mb_sn_gu_info_at_ngran_c::desc},
    {offsetof(location_dependent_mb_sn_gu_info_at_ngran_item_s, ie_exts),
     (int32_t)offsetof(location_dependent_mb_sn_gu_info_at_ngran_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(location_dependent_mb_sn_gu_info_at_ngran_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_location_dependent_mb_sn_gu_info_at_ngran_item_s_names[] = {
    {"mbsAreaSession-ID", nullptr},
    {"mbsNGUInformationAtNGRAN", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_location_dependent_mb_sn_gu_info_at_ngran_item_s_opts[] = {
    {2, (int32_t)offsetof(location_dependent_mb_sn_gu_info_at_ngran_item_s, ie_exts_present)}};
const asn1::table::seq_desc location_dependent_mb_sn_gu_info_at_ngran_item_s::desc = {
    {seq_table_location_dependent_mb_sn_gu_info_at_ngran_item_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_location_dependent_mb_sn_gu_info_at_ngran_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_location_dependent_mb_sn_gu_info_at_ngran_item_s_names};

// BCBearerContextNGU-TNLInfoatNGRAN ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_bc_bearer_context_ngu_tnl_infoat_ngran_c_alternatives[] = {
    &asn1::table::alternative_ops<mb_sn_gu_info_at_ngran_c, false>::value,
    &asn1::table::alternative_ops<location_dependent_mb_sn_gu_info_at_ngran_l, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>,
                                  false>::value};
const asn1::table::choice_storage_desc bc_bearer_context_ngu_tnl_infoat_ngran_c::storage_desc = {
    choice_storage_bc_bearer_context_ngu_tnl_infoat_ngran_c_alternatives,
    3,
    "bc_bearer_context_ngu_tnl_infoat_ngran_c"};

mb_sn_gu_info_at_ngran_c& bc_bearer_context_ngu_tnl_infoat_ngran_c::set_locationindependent()
{
  set(types::locationindependent);
  return c.get<mb_sn_gu_info_at_ngran_c>();
}
location_dependent_mb_sn_gu_info_at_ngran_l& bc_bearer_context_ngu_tnl_infoat_ngran_c::set_locationdependent()
{
  set(types::locationdependent);
  return c.get<location_dependent_mb_sn_gu_info_at_ngran_l>();
}
protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>&
bc_bearer_context_ngu_tnl_infoat_ngran_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_bc_bearer_context_ngu_tnl_infoat_ngran_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mb_sn_gu_info_at_ngran_c::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     256,
     &asn1::table::list_codec<location_dependent_mb_sn_gu_info_at_ngran_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_bc_bearer_context_ngu_tnl_infoat_ngran_c_names[] = {
    {"locationindependent", nullptr},
    {"locationdependent", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc bc_bearer_context_ngu_tnl_infoat_ngran_c::desc = {
    asn1::table::enum_int_type<bc_bearer_context_ngu_tnl_infoat_ngran_c::types>(),
    bc_bearer_context_ngu_tnl_infoat_ngran_c::types::nof_types,
    bc_bearer_context_ngu_tnl_infoat_ngran_c::types::nof_exts,
    bc_bearer_context_ngu_tnl_infoat_ngran_c::types::has_ext,
    true,
    choice_table_bc_bearer_context_ngu_tnl_infoat_ngran_c_alternatives,
    nullptr,
    choice_table_bc_bearer_context_ngu_tnl_infoat_ngran_c_names,
    "bc_bearer_context_ngu_tnl_infoat_ngran_c",
    offsetof(bc_bearer_context_ngu_tnl_infoat_ngran_c, c.type),
    offsetof(bc_bearer_context_ngu_tnl_infoat_ngran_c, c.value),
    false,
    &bc_bearer_context_ngu_tnl_infoat_ngran_c::storage_desc};

static const char* enum_names_bc_bearer_context_ngu_tnl_infoat_ngran_c_types_values[]   = {"locationindependent",
                                                                                           "locationdependent",
                                                                                           "choice-extension"};
const asn1::table::enum_desc bc_bearer_context_ngu_tnl_infoat_ngran_c::types_opts::desc = {
    enum_names_bc_bearer_context_ngu_tnl_infoat_ngran_c_types_values,
    3,
    "bc_bearer_context_ngu_tnl_infoat_ngran_c::types"};

// MBSNGUInformationAtNGRAN-Request ::= ENUMERATED
static const char*           enum_names_mb_sn_gu_info_at_ngran_request_e_values[] = {"true"};
const asn1::table::enum_desc mb_sn_gu_info_at_ngran_request_opts::desc            = {
    enum_names_mb_sn_gu_info_at_ngran_request_e_values,
    1,
    "mb_sn_gu_info_at_ngran_request_e"};

// MBSNGUInformationAtNGRAN-Request-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mb_sn_gu_info_at_ngran_request_item_s_fields[] = {
    {offsetof(mb_sn_gu_info_at_ngran_request_item_s, mbs_area_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(mb_sn_gu_info_at_ngran_request_item_s, mbs_ngu_info_at_ngran_request),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mb_sn_gu_info_at_ngran_request_item_s::mbs_ngu_info_at_ngran_request)>(),
     decltype(mb_sn_gu_info_at_ngran_request_item_s::mbs_ngu_info_at_ngran_request)::has_ext,
     false,
     decltype(mb_sn_gu_info_at_ngran_request_item_s::mbs_ngu_info_at_ngran_request)::nof_types,
     decltype(mb_sn_gu_info_at_ngran_request_item_s::mbs_ngu_info_at_ngran_request)::nof_exts,
     nullptr,
     &mb_sn_gu_info_at_ngran_request_opts::desc},
    {offsetof(mb_sn_gu_info_at_ngran_request_item_s, ie_exts),
     (int32_t)offsetof(mb_sn_gu_info_at_ngran_request_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mb_sn_gu_info_at_ngran_request_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mb_sn_gu_info_at_ngran_request_item_s_names[] = {
    {"mbsAreaSession-ID", nullptr},
    {"mbsNGUInformationAtNGRAN-Request", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mb_sn_gu_info_at_ngran_request_item_s_opts[] = {
    {2, (int32_t)offsetof(mb_sn_gu_info_at_ngran_request_item_s, ie_exts_present)}};
const asn1::table::seq_desc mb_sn_gu_info_at_ngran_request_item_s::desc = {
    {seq_table_mb_sn_gu_info_at_ngran_request_item_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_mb_sn_gu_info_at_ngran_request_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_mb_sn_gu_info_at_ngran_request_item_s_names};

// BCBearerContextNGU-TNLInfoatNGRAN-Request ::= CHOICE
static const asn1::table::storage_ops* const
    choice_storage_bc_bearer_context_ngu_tnl_infoat_ngran_request_c_alternatives[] = {
        &asn1::table::alternative_ops<mb_sn_gu_info_at_ngran_request_e, true>::value,
        &asn1::table::alternative_ops<mb_sn_gu_info_at_ngran_request_list_l, false>::value,
        &asn1::table::alternative_ops<
            protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_request_ext_ies_o>,
            false>::value};
const asn1::table::choice_storage_desc bc_bearer_context_ngu_tnl_infoat_ngran_request_c::storage_desc = {
    choice_storage_bc_bearer_context_ngu_tnl_infoat_ngran_request_c_alternatives,
    3,
    "bc_bearer_context_ngu_tnl_infoat_ngran_request_c"};

mb_sn_gu_info_at_ngran_request_e& bc_bearer_context_ngu_tnl_infoat_ngran_request_c::set_locationindependent()
{
  set(types::locationindependent);
  return c.get<mb_sn_gu_info_at_ngran_request_e>();
}
mb_sn_gu_info_at_ngran_request_list_l& bc_bearer_context_ngu_tnl_infoat_ngran_request_c::set_locationdependent()
{
  set(types::locationdependent);
  return c.get<mb_sn_gu_info_at_ngran_request_list_l>();
}
protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_request_ext_ies_o>&
bc_bearer_context_ngu_tnl_infoat_ngran_request_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_request_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_bc_bearer_context_ngu_tnl_infoat_ngran_request_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<mb_sn_gu_info_at_ngran_request_e>(),
     mb_sn_gu_info_at_ngran_request_e::has_ext,
     false,
     mb_sn_gu_info_at_ngran_request_e::nof_types,
     mb_sn_gu_info_at_ngran_request_e::nof_exts,
     nullptr,
     &mb_sn_gu_info_at_ngran_request_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     256,
     &asn1::table::list_codec<mb_sn_gu_info_at_ngran_request_list_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<
         protocol_ie_single_container_s<bc_bearer_context_ngu_tnl_infoat_ngran_request_ext_ies_o>,
         asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_bc_bearer_context_ngu_tnl_infoat_ngran_request_c_names[] = {
    {"locationindependent", nullptr},
    {"locationdependent", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc bc_bearer_context_ngu_tnl_infoat_ngran_request_c::desc = {
    asn1::table::enum_int_type<bc_bearer_context_ngu_tnl_infoat_ngran_request_c::types>(),
    bc_bearer_context_ngu_tnl_infoat_ngran_request_c::types::nof_types,
    bc_bearer_context_ngu_tnl_infoat_ngran_request_c::types::nof_exts,
    bc_bearer_context_ngu_tnl_infoat_ngran_request_c::types::has_ext,
    true,
    choice_table_bc_bearer_context_ngu_tnl_infoat_ngran_request_c_alternatives,
    nullptr,
    choice_table_bc_bearer_context_ngu_tnl_infoat_ngran_request_c_names,
    "bc_bearer_context_ngu_tnl_infoat_ngran_request_c",
    offsetof(bc_bearer_context_ngu_tnl_infoat_ngran_request_c, c.type),
    offsetof(bc_bearer_context_ngu_tnl_infoat_ngran_request_c, c.value),
    false,
    &bc_bearer_context_ngu_tnl_infoat_ngran_request_c::storage_desc};

static const char* enum_names_bc_bearer_context_ngu_tnl_infoat_ngran_request_c_types_values[] = {"locationindependent",
                                                                                                 "locationdependent",
                                                                                                 "choice-extension"};
const asn1::table::enum_desc bc_bearer_context_ngu_tnl_infoat_ngran_request_c::types_opts::desc = {
    enum_names_bc_bearer_context_ngu_tnl_infoat_ngran_request_c_types_values,
    3,
    "bc_bearer_context_ngu_tnl_infoat_ngran_request_c::types"};

// PDCP-SN-Size ::= ENUMERATED
static const char*           enum_names_pdcp_sn_size_e_values[] = {"s-12", "s-18", "s-7", "s-15", "s-16"};
const asn1::table::enum_desc pdcp_sn_size_opts::desc = {enum_names_pdcp_sn_size_e_values, 5, "pdcp_sn_size_e"};
int8_t                       pdcp_sn_size_opts::to_number() const
{
  static const int8_t numbers[] = {-12, -18, -7, -15, -16};
  return map_enum_number(numbers, 5, value, "pdcp_sn_size_e");
}

// RLC-Mode ::= ENUMERATED
static const char*           enum_names_rlc_mode_e_values[] = {"rlc-tm",
                                                               "rlc-am",
                                                               "rlc-um-bidirectional",
                                                               "rlc-um-unidirectional-ul",
                                                               "rlc-um-unidirectional-dl"};
const asn1::table::enum_desc rlc_mode_opts::desc            = {enum_names_rlc_mode_e_values, 5, "rlc_mode_e"};

// ROHC ::= SEQUENCE
static const asn1::table::seq_field seq_table_rohc_s_fields[] = {
    {offsetof(rohc_s, max_c_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(16383),
     nullptr},
    {offsetof(rohc_s, rohc_profiles),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(511),
     nullptr},
    {offsetof(rohc_s, continue_rohc),
     (int32_t)offsetof(rohc_s, continue_rohc_present),
     asn1::table::enum_op<decltype(rohc_s::continue_rohc)>(),
     decltype(rohc_s::continue_rohc)::has_ext,
     false,
     decltype(rohc_s::continue_rohc)::nof_types,
     decltype(rohc_s::continue_rohc)::nof_exts,
     nullptr,
     &rohc_s::continue_rohc_opts::desc},
    {offsetof(rohc_s, ie_exts),
     (int32_t)offsetof(rohc_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rohc_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_rohc_s_names[] = {{"maxCID", nullptr},
                                                                 {"rOHC-Profiles", nullptr},
                                                                 {"continueROHC", nullptr},
                                                                 {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_rohc_s_opts[]  = {{2, (int32_t)offsetof(rohc_s, continue_rohc_present)},
                                                                 {3, (int32_t)offsetof(rohc_s, ie_exts_present)}};
const asn1::table::seq_desc rohc_s::desc = {{seq_table_rohc_s_fields, 4, UINT64_C(0x3), seq_table_rohc_s_opts, 2},
                                            false,
                                            true,
                                            nullptr,
                                            0,
                                            seq_table_rohc_s_names};

static const char*           enum_names_rohc_s_continue_rohc_e__values[] = {"true"};
const asn1::table::enum_desc rohc_s::continue_rohc_opts::desc            = {enum_names_rohc_s_continue_rohc_e__values,
                                                                            1,
                                                                            "rohc_s::continue_rohc_e_"};

// UplinkOnlyROHC ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_only_rohc_s_fields[] = {
    {offsetof(ul_only_rohc_s, max_c_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(16383),
     nullptr},
    {offsetof(ul_only_rohc_s, rohc_profiles),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(511),
     nullptr},
    {offsetof(ul_only_rohc_s, continue_rohc),
     (int32_t)offsetof(ul_only_rohc_s, continue_rohc_present),
     asn1::table::enum_op<decltype(ul_only_rohc_s::continue_rohc)>(),
     decltype(ul_only_rohc_s::continue_rohc)::has_ext,
     false,
     decltype(ul_only_rohc_s::continue_rohc)::nof_types,
     decltype(ul_only_rohc_s::continue_rohc)::nof_exts,
     nullptr,
     &ul_only_rohc_s::continue_rohc_opts::desc},
    {offsetof(ul_only_rohc_s, ie_exts),
     (int32_t)offsetof(ul_only_rohc_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ul_only_rohc_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ul_only_rohc_s_names[] = {{"maxCID", nullptr},
                                                                         {"rOHC-Profiles", nullptr},
                                                                         {"continueROHC", nullptr},
                                                                         {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ul_only_rohc_s_opts[]  = {
    {2, (int32_t)offsetof(ul_only_rohc_s, continue_rohc_present)},
    {3, (int32_t)offsetof(ul_only_rohc_s, ie_exts_present)}};
const asn1::table::seq_desc ul_only_rohc_s::desc = {
    {seq_table_ul_only_rohc_s_fields, 4, UINT64_C(0x3), seq_table_ul_only_rohc_s_opts, 2},
    false,
    true,
    nullptr,
    0,
    seq_table_ul_only_rohc_s_names};

static const char*           enum_names_ul_only_rohc_s_continue_rohc_e__values[] = {"true"};
const asn1::table::enum_desc ul_only_rohc_s::continue_rohc_opts::desc            = {
    enum_names_ul_only_rohc_s_continue_rohc_e__values,
    1,
    "ul_only_rohc_s::continue_rohc_e_"};

// ROHC-Parameters ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_rohc_params_c_alternatives[] = {
    &asn1::table::alternative_ops<rohc_s, false>::value,
    &asn1::table::alternative_ops<ul_only_rohc_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<rohc_params_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc rohc_params_c::storage_desc = {choice_storage_rohc_params_c_alternatives,
                                                                      3,
                                                                      "rohc_params_c"};

rohc_s& rohc_params_c::set_rohc()
{
  set(types::rohc);
  return c.get<rohc_s>();
}
ul_only_rohc_s& rohc_params_c::set_ul_only_rohc()
{
  set(types::ul_only_rohc);
  return c.get<ul_only_rohc_s>();
}
protocol_ie_single_container_s<rohc_params_ext_ies_o>& rohc_params_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<rohc_params_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_rohc_params_c_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::sequence, false, false, 0, 0, nullptr, &rohc_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_only_rohc_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<rohc_params_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_rohc_params_c_names[] = {{"rOHC", nullptr},
                                                                           {"uPlinkOnlyROHC", nullptr},
                                                                           {"choice-Extension", nullptr}};
const asn1::table::choice_desc       rohc_params_c::desc = {asn1::table::enum_int_type<rohc_params_c::types>(),
                                                            rohc_params_c::types::nof_types,
                                                            rohc_params_c::types::nof_exts,
                                                            rohc_params_c::types::has_ext,
                                                            true,
                                                            choice_table_rohc_params_c_alternatives,
                                                            nullptr,
                                                            choice_table_rohc_params_c_names,
                                                            "rohc_params_c",
                                                            offsetof(rohc_params_c, c.type),
                                                            offsetof(rohc_params_c, c.value),
                                                            false,
                                                            &rohc_params_c::storage_desc};

static const char*           enum_names_rohc_params_c_types_values[] = {"rOHC", "uPlinkOnlyROHC", "choice-Extension"};
const asn1::table::enum_desc rohc_params_c::types_opts::desc         = {enum_names_rohc_params_c_types_values,
                                                                        3,
                                                                        "rohc_params_c::types"};

// T-Reordering ::= ENUMERATED
static const char* enum_names_t_reordering_e_values[] = {
    "ms0",    "ms1",    "ms2",    "ms4",    "ms5",    "ms8",    "ms10",   "ms15",   "ms20",
    "ms30",   "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms120",  "ms140",  "ms160",
    "ms180",  "ms200",  "ms220",  "ms240",  "ms260",  "ms280",  "ms300",  "ms500",  "ms750",
    "ms1000", "ms1250", "ms1500", "ms1750", "ms2000", "ms2250", "ms2500", "ms2750", "ms3000"};
const asn1::table::enum_desc t_reordering_opts::desc = {enum_names_t_reordering_e_values, 36, "t_reordering_e"};
uint16_t                     t_reordering_opts::to_number() const
{
  static const uint16_t numbers[] = {0,   1,   2,   4,    5,    8,    10,   15,   20,   30,   40,   50,
                                     60,  80,  100, 120,  140,  160,  180,  200,  220,  240,  260,  280,
                                     300, 500, 750, 1000, 1250, 1500, 1750, 2000, 2250, 2500, 2750, 3000};
  return map_enum_number(numbers, 36, value, "t_reordering_e");
}

// T-ReorderingTimer ::= SEQUENCE
static const asn1::table::seq_field seq_table_t_reordering_timer_s_fields[] = {
    {offsetof(t_reordering_timer_s, t_reordering),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(t_reordering_timer_s::t_reordering)>(),
     decltype(t_reordering_timer_s::t_reordering)::has_ext,
     false,
     decltype(t_reordering_timer_s::t_reordering)::nof_types,
     decltype(t_reordering_timer_s::t_reordering)::nof_exts,
     nullptr,
     &t_reordering_opts::desc},
    {offsetof(t_reordering_timer_s, ie_exts),
     (int32_t)offsetof(t_reordering_timer_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(t_reordering_timer_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_t_reordering_timer_s_names[] = {{"t-Reordering", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_t_reordering_timer_s_opts[]  = {
    {1, (int32_t)offsetof(t_reordering_timer_s, ie_exts_present)}};
const asn1::table::seq_desc t_reordering_timer_s::desc = {
    {seq_table_t_reordering_timer_s_fields, 2, UINT64_C(0x1), seq_table_t_reordering_timer_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_t_reordering_timer_s_names};

// DiscardTimer ::= ENUMERATED
static const char*           enum_names_discard_timer_e_values[] = {"ms10",
                                                                    "ms20",
                                                                    "ms30",
                                                                    "ms40",
                                                                    "ms50",
                                                                    "ms60",
                                                                    "ms75",
                                                                    "ms100",
                                                                    "ms150",
                                                                    "ms200",
                                                                    "ms250",
                                                                    "ms300",
                                                                    "ms500",
                                                                    "ms750",
                                                                    "ms1500",
                                                                    "infinity"};
const asn1::table::enum_desc discard_timer_opts::desc = {enum_names_discard_timer_e_values, 16, "discard_timer_e"};
int16_t                      discard_timer_opts::to_number() const
{
  static const int16_t numbers[] = {10, 20, 30, 40, 50, 60, 75, 100, 150, 200, 250, 300, 500, 750, 1500, -1};
  return map_enum_number(numbers, 16, value, "discard_timer_e");
}

// ULDataSplitThreshold ::= ENUMERATED
static const char* enum_names_ul_data_split_thres_e_values[] = {
    "b0",       "b100",     "b200",     "b400",     "b800",     "b1600",    "b3200",    "b6400",
    "b12800",   "b25600",   "b51200",   "b102400",  "b204800",  "b409600",  "b819200",  "b1228800",
    "b1638400", "b2457600", "b3276800", "b4096000", "b4915200", "b5734400", "b6553600", "infinity"};
const asn1::table::enum_desc ul_data_split_thres_opts::desc = {enum_names_ul_data_split_thres_e_values,
                                                               24,
                                                               "ul_data_split_thres_e"};
int32_t                      ul_data_split_thres_opts::to_number() const
{
  static const int32_t numbers[] = {0,       100,     200,     400,     800,     1600,    3200,    6400,
                                    12800,   25600,   51200,   102400,  204800,  409600,  819200,  1228800,
                                    1638400, 2457600, 3276800, 4096000, 4915200, 5734400, 6553600, -1};
  return map_enum_number(numbers, 24, value, "ul_data_split_thres_e");
}

// PDCP-Duplication ::= ENUMERATED
static const char*           enum_names_pdcp_dupl_e_values[] = {"true"};
const asn1::table::enum_desc pdcp_dupl_opts::desc            = {enum_names_pdcp_dupl_e_values, 1, "pdcp_dupl_e"};

// PDCP-Reestablishment ::= ENUMERATED
static const char*           enum_names_pdcp_reest_e_values[] = {"true"};
const asn1::table::enum_desc pdcp_reest_opts::desc            = {enum_names_pdcp_reest_e_values, 1, "pdcp_reest_e"};

// PDCP-DataRecovery ::= ENUMERATED
static const char*           enum_names_pdcp_data_recovery_e_values[] = {"true"};
const asn1::table::enum_desc pdcp_data_recovery_opts::desc            = {enum_names_pdcp_data_recovery_e_values,
                                                                         1,
                                                                         "pdcp_data_recovery_e"};

// Duplication-Activation ::= ENUMERATED
static const char*           enum_names_dupl_activation_e_values[] = {"active", "inactive"};
const asn1::table::enum_desc dupl_activation_opts::desc = {enum_names_dupl_activation_e_values, 2, "dupl_activation_e"};

// OutOfOrderDelivery ::= ENUMERATED
static const char*           enum_names_out_of_order_delivery_e_values[] = {"true"};
const asn1::table::enum_desc out_of_order_delivery_opts::desc            = {enum_names_out_of_order_delivery_e_values,
                                                                            1,
                                                                            "out_of_order_delivery_e"};

// PDCP-StatusReportIndication ::= ENUMERATED
static const char*           enum_names_pdcp_status_report_ind_e_values[] = {"downlink", "uplink", "both"};
const asn1::table::enum_desc pdcp_status_report_ind_opts::desc            = {enum_names_pdcp_status_report_ind_e_values,
                                                                             3,
                                                                             "pdcp_status_report_ind_e"};

// EHC-Common-Parameters ::= SEQUENCE
static const asn1::table::seq_field seq_table_ehc_common_params_s_fields[] = {
    {offsetof(ehc_common_params_s, ehc_c_id_len),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ehc_common_params_s::ehc_c_id_len)>(),
     decltype(ehc_common_params_s::ehc_c_id_len)::has_ext,
     false,
     decltype(ehc_common_params_s::ehc_c_id_len)::nof_types,
     decltype(ehc_common_params_s::ehc_c_id_len)::nof_exts,
     nullptr,
     &ehc_common_params_s::ehc_c_id_len_opts::desc},
    {offsetof(ehc_common_params_s, ie_exts),
     (int32_t)offsetof(ehc_common_params_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ehc_common_params_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ehc_common_params_s_names[] = {{"ehc-CID-Length", nullptr},
                                                                              {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ehc_common_params_s_opts[]  = {
    {1, (int32_t)offsetof(ehc_common_params_s, ie_exts_present)}};
const asn1::table::seq_desc ehc_common_params_s::desc = {
    {seq_table_ehc_common_params_s_fields, 2, UINT64_C(0x1), seq_table_ehc_common_params_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_ehc_common_params_s_names};

static const char*           enum_names_ehc_common_params_s_ehc_c_id_len_e__values[] = {"bits7", "bits15"};
const asn1::table::enum_desc ehc_common_params_s::ehc_c_id_len_opts::desc            = {
    enum_names_ehc_common_params_s_ehc_c_id_len_e__values,
    2,
    "ehc_common_params_s::ehc_c_id_len_e_"};
uint8_t ehc_common_params_s::ehc_c_id_len_opts::to_number() const
{
  static const uint8_t numbers[] = {7, 15};
  return map_enum_number(numbers, 2, value, "ehc_common_params_s::ehc_c_id_len_e_");
}

static const uint32_t objset_id_ehc_dl_params_ext_ies_o[]   = {137};
static const uint8_t  objset_crit_ehc_dl_params_ext_ies_o[] = {crit_e::ignore};

// EHC-Downlink-Parameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_ehc_dl_params_ext_ies_container_values[] = {
    {offsetof(ehc_dl_params_ext_ies_container, max_c_id_ehc_dl),
     (int32_t)offsetof(ehc_dl_params_ext_ies_container, max_c_id_ehc_dl_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(32767),
     nullptr}};
static const int32_t ie_table_ehc_dl_params_ext_ies_container_presences[] = {
    (int32_t)offsetof(ehc_dl_params_ext_ies_container, max_c_id_ehc_dl_present)};
const asn1::table::ie_container_desc ehc_dl_params_ext_ies_container::desc = {
    objset_id_ehc_dl_params_ext_ies_o,
    objset_crit_ehc_dl_params_ext_ies_o,
    ie_table_ehc_dl_params_ext_ies_container_values,
    ie_table_ehc_dl_params_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// EHC-Downlink-Parameters ::= SEQUENCE
static const asn1::table::seq_field seq_table_ehc_dl_params_s_fields[] = {
    {offsetof(ehc_dl_params_s, drb_continue_ehc_dl),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ehc_dl_params_s::drb_continue_ehc_dl)>(),
     decltype(ehc_dl_params_s::drb_continue_ehc_dl)::has_ext,
     false,
     decltype(ehc_dl_params_s::drb_continue_ehc_dl)::nof_types,
     decltype(ehc_dl_params_s::drb_continue_ehc_dl)::nof_exts,
     nullptr,
     &ehc_dl_params_s::drb_continue_ehc_dl_opts::desc},
    {offsetof(ehc_dl_params_s, ie_exts),
     (int32_t)offsetof(ehc_dl_params_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &ehc_dl_params_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_ehc_dl_params_s_names[] = {{"drb-ContinueEHC-DL", nullptr},
                                                                          {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ehc_dl_params_s_opts[]  = {
    {1, (int32_t)offsetof(ehc_dl_params_s, ie_exts_present)}};
const asn1::table::seq_desc ehc_dl_params_s::desc = {
    {seq_table_ehc_dl_params_s_fields, 2, UINT64_C(0x1), seq_table_ehc_dl_params_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_ehc_dl_params_s_names};

static const char*           enum_names_ehc_dl_params_s_drb_continue_ehc_dl_e__values[] = {"true", "false"};
const asn1::table::enum_desc ehc_dl_params_s::drb_continue_ehc_dl_opts::desc            = {
    enum_names_ehc_dl_params_s_drb_continue_ehc_dl_e__values,
    2,
    "ehc_dl_params_s::drb_continue_ehc_dl_e_"};

// EHC-Uplink-Parameters ::= SEQUENCE
static const asn1::table::seq_field seq_table_ehc_ul_params_s_fields[] = {
    {offsetof(ehc_ul_params_s, drb_continue_ehc_ul),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ehc_ul_params_s::drb_continue_ehc_ul)>(),
     decltype(ehc_ul_params_s::drb_continue_ehc_ul)::has_ext,
     false,
     decltype(ehc_ul_params_s::drb_continue_ehc_ul)::nof_types,
     decltype(ehc_ul_params_s::drb_continue_ehc_ul)::nof_exts,
     nullptr,
     &ehc_ul_params_s::drb_continue_ehc_ul_opts::desc},
    {offsetof(ehc_ul_params_s, ie_exts),
     (int32_t)offsetof(ehc_ul_params_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ehc_ul_params_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ehc_ul_params_s_names[] = {{"drb-ContinueEHC-UL", nullptr},
                                                                          {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ehc_ul_params_s_opts[]  = {
    {1, (int32_t)offsetof(ehc_ul_params_s, ie_exts_present)}};
const asn1::table::seq_desc ehc_ul_params_s::desc = {
    {seq_table_ehc_ul_params_s_fields, 2, UINT64_C(0x1), seq_table_ehc_ul_params_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_ehc_ul_params_s_names};

static const char*           enum_names_ehc_ul_params_s_drb_continue_ehc_ul_e__values[] = {"true", "false"};
const asn1::table::enum_desc ehc_ul_params_s::drb_continue_ehc_ul_opts::desc            = {
    enum_names_ehc_ul_params_s_drb_continue_ehc_ul_e__values,
    2,
    "ehc_ul_params_s::drb_continue_ehc_ul_e_"};

// EHC-Parameters ::= SEQUENCE
static const asn1::table::seq_field seq_table_ehc_params_s_fields[] = {
    {offsetof(ehc_params_s, ehc_common),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ehc_common_params_s::desc},
    {offsetof(ehc_params_s, ehc_dl),
     (int32_t)offsetof(ehc_params_s, ehc_dl_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ehc_dl_params_s::desc},
    {offsetof(ehc_params_s, ehc_ul),
     (int32_t)offsetof(ehc_params_s, ehc_ul_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ehc_ul_params_s::desc},
    {offsetof(ehc_params_s, ie_exts),
     (int32_t)offsetof(ehc_params_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ehc_params_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ehc_params_s_names[] = {{"ehc-Common", nullptr},
                                                                       {"ehc-Downlink", nullptr},
                                                                       {"ehc-Uplink", nullptr},
                                                                       {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ehc_params_s_opts[]  = {
    {1, (int32_t)offsetof(ehc_params_s, ehc_dl_present)},
    {2, (int32_t)offsetof(ehc_params_s, ehc_ul_present)},
    {3, (int32_t)offsetof(ehc_params_s, ie_exts_present)}};
const asn1::table::seq_desc ehc_params_s::desc = {
    {seq_table_ehc_params_s_fields, 4, UINT64_C(0x1), seq_table_ehc_params_s_opts, 3},
    false,
    true,
    nullptr,
    0,
    seq_table_ehc_params_s_names};

// BufferSize ::= ENUMERATED
static const char*           enum_names_buffer_size_e_values[] = {"kbyte2", "kbyte4", "kbyte8"};
const asn1::table::enum_desc buffer_size_opts::desc            = {enum_names_buffer_size_e_values, 3, "buffer_size_e"};
uint8_t                      buffer_size_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "buffer_size_e");
}

// Dictionary ::= ENUMERATED
static const char*           enum_names_dictionary_e_values[] = {"sip-SDP", "operator"};
const asn1::table::enum_desc dictionary_opts::desc            = {enum_names_dictionary_e_values, 2, "dictionary_e"};

static const uint32_t objset_id_udc_params_ext_ies_o[]   = {186};
static const uint8_t  objset_crit_udc_params_ext_ies_o[] = {crit_e::ignore};

// UDC-Parameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_udc_params_ext_ies_container_values[] = {
    {offsetof(udc_params_ext_ies_container, version_id),
     (int32_t)offsetof(udc_params_ext_ies_container, version_id_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(15),
     nullptr}};
static const int32_t ie_table_udc_params_ext_ies_container_presences[] = {
    (int32_t)offsetof(udc_params_ext_ies_container, version_id_present)};
const asn1::table::ie_container_desc udc_params_ext_ies_container::desc = {
    objset_id_udc_params_ext_ies_o,
    objset_crit_udc_params_ext_ies_o,
    ie_table_udc_params_ext_ies_container_values,
    ie_table_udc_params_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// UDC-Parameters ::= SEQUENCE
static const asn1::table::seq_field seq_table_udc_params_s_fields[] = {
    {offsetof(udc_params_s, buffer_size),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(udc_params_s::buffer_size)>(),
     decltype(udc_params_s::buffer_size)::has_ext,
     false,
     decltype(udc_params_s::buffer_size)::nof_types,
     decltype(udc_params_s::buffer_size)::nof_exts,
     nullptr,
     &buffer_size_opts::desc},
    {offsetof(udc_params_s, dictionary),
     (int32_t)offsetof(udc_params_s, dictionary_present),
     asn1::table::enum_op<decltype(udc_params_s::dictionary)>(),
     decltype(udc_params_s::dictionary)::has_ext,
     false,
     decltype(udc_params_s::dictionary)::nof_types,
     decltype(udc_params_s::dictionary)::nof_exts,
     nullptr,
     &dictionary_opts::desc},
    {offsetof(udc_params_s, continue_udc),
     (int32_t)offsetof(udc_params_s, continue_udc_present),
     asn1::table::enum_op<decltype(udc_params_s::continue_udc)>(),
     decltype(udc_params_s::continue_udc)::has_ext,
     false,
     decltype(udc_params_s::continue_udc)::nof_types,
     decltype(udc_params_s::continue_udc)::nof_exts,
     nullptr,
     &udc_params_s::continue_udc_opts::desc},
    {offsetof(udc_params_s, ie_exts),
     (int32_t)offsetof(udc_params_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &udc_params_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_udc_params_s_names[] = {{"bufferSize", nullptr},
                                                                       {"dictionary", nullptr},
                                                                       {"continueUDC", nullptr},
                                                                       {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_udc_params_s_opts[]  = {
    {1, (int32_t)offsetof(udc_params_s, dictionary_present)},
    {2, (int32_t)offsetof(udc_params_s, continue_udc_present)},
    {3, (int32_t)offsetof(udc_params_s, ie_exts_present)}};
const asn1::table::seq_desc udc_params_s::desc = {
    {seq_table_udc_params_s_fields, 4, UINT64_C(0x1), seq_table_udc_params_s_opts, 3},
    false,
    true,
    nullptr,
    0,
    seq_table_udc_params_s_names};

static const char*           enum_names_udc_params_s_continue_udc_e__values[] = {"true"};
const asn1::table::enum_desc udc_params_s::continue_udc_opts::desc = {enum_names_udc_params_s_continue_udc_e__values,
                                                                      1,
                                                                      "udc_params_s::continue_udc_e_"};

// DiscardTimerExtended ::= ENUMERATED
static const char* enum_names_discard_timer_extended_e_values[] =
    {"ms0dot5", "ms1", "ms2", "ms4", "ms6", "ms8", "ms2000"};
const asn1::table::enum_desc discard_timer_extended_opts::desc = {enum_names_discard_timer_extended_e_values,
                                                                  7,
                                                                  "discard_timer_extended_e"};
float                        discard_timer_extended_opts::to_number() const
{
  static const float numbers[] = {0.5, 1.0, 2.0, 4.0, 6.0, 8.0, 2000.0};
  return map_enum_number(numbers, 7, value, "discard_timer_extended_e");
}
const char* discard_timer_extended_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.5", "1", "2", "4", "6", "8", "2000"};
  return convert_enum_idx(number_strs, 7, value, "discard_timer_extended_e");
}

// PSIbasedDiscardTimer ::= ENUMERATED
static const char*           enum_names_psi_based_discard_timer_e_values[] = {"ms0",
                                                                              "ms2",
                                                                              "ms4",
                                                                              "ms6",
                                                                              "ms8",
                                                                              "ms10",
                                                                              "ms12",
                                                                              "ms14",
                                                                              "ms18",
                                                                              "ms22",
                                                                              "ms26",
                                                                              "ms30",
                                                                              "ms40",
                                                                              "ms50",
                                                                              "ms75",
                                                                              "ms100"};
const asn1::table::enum_desc psi_based_discard_timer_opts::desc = {enum_names_psi_based_discard_timer_e_values,
                                                                   16,
                                                                   "psi_based_discard_timer_e"};
uint8_t                      psi_based_discard_timer_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 2, 4, 6, 8, 10, 12, 14, 18, 22, 26, 30, 40, 50, 75, 100};
  return map_enum_number(numbers, 16, value, "psi_based_discard_timer_e");
}

// PDCPSNGapReport ::= ENUMERATED
static const char*           enum_names_pdcp_sn_gap_report_e_values[] = {"true"};
const asn1::table::enum_desc pdcp_sn_gap_report_opts::desc            = {enum_names_pdcp_sn_gap_report_e_values,
                                                                         1,
                                                                         "pdcp_sn_gap_report_e"};

static const uint32_t objset_id_pdcp_cfg_ext_ies_o[] = {88, 104, 118, 153, 177, 214, 218};
static const uint8_t  objset_crit_pdcp_cfg_ext_ies_o[] =
    {crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::ignore, crit_e::ignore};

// PDCP-Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_pdcp_cfg_ext_ies_container_values[] = {
    {offsetof(pdcp_cfg_ext_ies_container, pdcp_status_report_ind),
     (int32_t)offsetof(pdcp_cfg_ext_ies_container, pdcp_status_report_ind_present),
     asn1::table::enum_op<decltype(pdcp_cfg_ext_ies_container::pdcp_status_report_ind)>(),
     decltype(pdcp_cfg_ext_ies_container::pdcp_status_report_ind)::has_ext,
     false,
     decltype(pdcp_cfg_ext_ies_container::pdcp_status_report_ind)::nof_types,
     decltype(pdcp_cfg_ext_ies_container::pdcp_status_report_ind)::nof_exts,
     nullptr,
     &pdcp_status_report_ind_opts::desc},
    {offsetof(pdcp_cfg_ext_ies_container, add_pdcp_dupl_info),
     (int32_t)offsetof(pdcp_cfg_ext_ies_container, add_pdcp_dupl_info_present),
     asn1::table::enum_op<decltype(pdcp_cfg_ext_ies_container::add_pdcp_dupl_info)>(),
     decltype(pdcp_cfg_ext_ies_container::add_pdcp_dupl_info)::has_ext,
     false,
     decltype(pdcp_cfg_ext_ies_container::add_pdcp_dupl_info)::nof_types,
     decltype(pdcp_cfg_ext_ies_container::add_pdcp_dupl_info)::nof_exts,
     nullptr,
     &add_pdcp_dupl_info_opts::desc},
    {offsetof(pdcp_cfg_ext_ies_container, ehc_params),
     (int32_t)offsetof(pdcp_cfg_ext_ies_container, ehc_params_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ehc_params_s::desc},
    {offsetof(pdcp_cfg_ext_ies_container, udc_params),
     (int32_t)offsetof(pdcp_cfg_ext_ies_container, udc_params_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &udc_params_s::desc},
    {offsetof(pdcp_cfg_ext_ies_container, discard_timer_extended),
     (int32_t)offsetof(pdcp_cfg_ext_ies_container, discard_timer_extended_present),
     asn1::table::enum_op<decltype(pdcp_cfg_ext_ies_container::discard_timer_extended)>(),
     decltype(pdcp_cfg_ext_ies_container::discard_timer_extended)::has_ext,
     false,
     decltype(pdcp_cfg_ext_ies_container::discard_timer_extended)::nof_types,
     decltype(pdcp_cfg_ext_ies_container::discard_timer_extended)::nof_exts,
     nullptr,
     &discard_timer_extended_opts::desc},
    {offsetof(pdcp_cfg_ext_ies_container, psi_based_discard_timer),
     (int32_t)offsetof(pdcp_cfg_ext_ies_container, psi_based_discard_timer_present),
     asn1::table::enum_op<decltype(pdcp_cfg_ext_ies_container::psi_based_discard_timer)>(),
     decltype(pdcp_cfg_ext_ies_container::psi_based_discard_timer)::has_ext,
     false,
     decltype(pdcp_cfg_ext_ies_container::psi_based_discard_timer)::nof_types,
     decltype(pdcp_cfg_ext_ies_container::psi_based_discard_timer)::nof_exts,
     nullptr,
     &psi_based_discard_timer_opts::desc},
    {offsetof(pdcp_cfg_ext_ies_container, pdcp_sn_gap_report),
     (int32_t)offsetof(pdcp_cfg_ext_ies_container, pdcp_sn_gap_report_present),
     asn1::table::enum_op<decltype(pdcp_cfg_ext_ies_container::pdcp_sn_gap_report)>(),
     decltype(pdcp_cfg_ext_ies_container::pdcp_sn_gap_report)::has_ext,
     false,
     decltype(pdcp_cfg_ext_ies_container::pdcp_sn_gap_report)::nof_types,
     decltype(pdcp_cfg_ext_ies_container::pdcp_sn_gap_report)::nof_exts,
     nullptr,
     &pdcp_sn_gap_report_opts::desc}};
static const int32_t ie_table_pdcp_cfg_ext_ies_container_presences[] = {
    (int32_t)offsetof(pdcp_cfg_ext_ies_container, pdcp_status_report_ind_present),
    (int32_t)offsetof(pdcp_cfg_ext_ies_container, add_pdcp_dupl_info_present),
    (int32_t)offsetof(pdcp_cfg_ext_ies_container, ehc_params_present),
    (int32_t)offsetof(pdcp_cfg_ext_ies_container, udc_params_present),
    (int32_t)offsetof(pdcp_cfg_ext_ies_container, discard_timer_extended_present),
    (int32_t)offsetof(pdcp_cfg_ext_ies_container, psi_based_discard_timer_present),
    (int32_t)offsetof(pdcp_cfg_ext_ies_container, pdcp_sn_gap_report_present)};
const asn1::table::ie_container_desc pdcp_cfg_ext_ies_container::desc = {objset_id_pdcp_cfg_ext_ies_o,
                                                                         objset_crit_pdcp_cfg_ext_ies_o,
                                                                         ie_table_pdcp_cfg_ext_ies_container_values,
                                                                         ie_table_pdcp_cfg_ext_ies_container_presences,
                                                                         7,
                                                                         0,
                                                                         1,
                                                                         65535,
                                                                         true,
                                                                         "extensionValue"};

// PDCP-Configuration ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdcp_cfg_s_fields[] = {
    {offsetof(pdcp_cfg_s, pdcp_sn_size_ul),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pdcp_cfg_s::pdcp_sn_size_ul)>(),
     decltype(pdcp_cfg_s::pdcp_sn_size_ul)::has_ext,
     false,
     decltype(pdcp_cfg_s::pdcp_sn_size_ul)::nof_types,
     decltype(pdcp_cfg_s::pdcp_sn_size_ul)::nof_exts,
     nullptr,
     &pdcp_sn_size_opts::desc},
    {offsetof(pdcp_cfg_s, pdcp_sn_size_dl),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pdcp_cfg_s::pdcp_sn_size_dl)>(),
     decltype(pdcp_cfg_s::pdcp_sn_size_dl)::has_ext,
     false,
     decltype(pdcp_cfg_s::pdcp_sn_size_dl)::nof_types,
     decltype(pdcp_cfg_s::pdcp_sn_size_dl)::nof_exts,
     nullptr,
     &pdcp_sn_size_opts::desc},
    {offsetof(pdcp_cfg_s, rlc_mode),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pdcp_cfg_s::rlc_mode)>(),
     decltype(pdcp_cfg_s::rlc_mode)::has_ext,
     false,
     decltype(pdcp_cfg_s::rlc_mode)::nof_types,
     decltype(pdcp_cfg_s::rlc_mode)::nof_exts,
     nullptr,
     &rlc_mode_opts::desc},
    {offsetof(pdcp_cfg_s, rohc_params),
     (int32_t)offsetof(pdcp_cfg_s, rohc_params_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &rohc_params_c::desc},
    {offsetof(pdcp_cfg_s, t_reordering_timer),
     (int32_t)offsetof(pdcp_cfg_s, t_reordering_timer_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &t_reordering_timer_s::desc},
    {offsetof(pdcp_cfg_s, discard_timer),
     (int32_t)offsetof(pdcp_cfg_s, discard_timer_present),
     asn1::table::enum_op<decltype(pdcp_cfg_s::discard_timer)>(),
     decltype(pdcp_cfg_s::discard_timer)::has_ext,
     false,
     decltype(pdcp_cfg_s::discard_timer)::nof_types,
     decltype(pdcp_cfg_s::discard_timer)::nof_exts,
     nullptr,
     &discard_timer_opts::desc},
    {offsetof(pdcp_cfg_s, ul_data_split_thres),
     (int32_t)offsetof(pdcp_cfg_s, ul_data_split_thres_present),
     asn1::table::enum_op<decltype(pdcp_cfg_s::ul_data_split_thres)>(),
     decltype(pdcp_cfg_s::ul_data_split_thres)::has_ext,
     false,
     decltype(pdcp_cfg_s::ul_data_split_thres)::nof_types,
     decltype(pdcp_cfg_s::ul_data_split_thres)::nof_exts,
     nullptr,
     &ul_data_split_thres_opts::desc},
    {offsetof(pdcp_cfg_s, pdcp_dupl),
     (int32_t)offsetof(pdcp_cfg_s, pdcp_dupl_present),
     asn1::table::enum_op<decltype(pdcp_cfg_s::pdcp_dupl)>(),
     decltype(pdcp_cfg_s::pdcp_dupl)::has_ext,
     false,
     decltype(pdcp_cfg_s::pdcp_dupl)::nof_types,
     decltype(pdcp_cfg_s::pdcp_dupl)::nof_exts,
     nullptr,
     &pdcp_dupl_opts::desc},
    {offsetof(pdcp_cfg_s, pdcp_reest),
     (int32_t)offsetof(pdcp_cfg_s, pdcp_reest_present),
     asn1::table::enum_op<decltype(pdcp_cfg_s::pdcp_reest)>(),
     decltype(pdcp_cfg_s::pdcp_reest)::has_ext,
     false,
     decltype(pdcp_cfg_s::pdcp_reest)::nof_types,
     decltype(pdcp_cfg_s::pdcp_reest)::nof_exts,
     nullptr,
     &pdcp_reest_opts::desc},
    {offsetof(pdcp_cfg_s, pdcp_data_recovery),
     (int32_t)offsetof(pdcp_cfg_s, pdcp_data_recovery_present),
     asn1::table::enum_op<decltype(pdcp_cfg_s::pdcp_data_recovery)>(),
     decltype(pdcp_cfg_s::pdcp_data_recovery)::has_ext,
     false,
     decltype(pdcp_cfg_s::pdcp_data_recovery)::nof_types,
     decltype(pdcp_cfg_s::pdcp_data_recovery)::nof_exts,
     nullptr,
     &pdcp_data_recovery_opts::desc},
    {offsetof(pdcp_cfg_s, dupl_activation),
     (int32_t)offsetof(pdcp_cfg_s, dupl_activation_present),
     asn1::table::enum_op<decltype(pdcp_cfg_s::dupl_activation)>(),
     decltype(pdcp_cfg_s::dupl_activation)::has_ext,
     false,
     decltype(pdcp_cfg_s::dupl_activation)::nof_types,
     decltype(pdcp_cfg_s::dupl_activation)::nof_exts,
     nullptr,
     &dupl_activation_opts::desc},
    {offsetof(pdcp_cfg_s, out_of_order_delivery),
     (int32_t)offsetof(pdcp_cfg_s, out_of_order_delivery_present),
     asn1::table::enum_op<decltype(pdcp_cfg_s::out_of_order_delivery)>(),
     decltype(pdcp_cfg_s::out_of_order_delivery)::has_ext,
     false,
     decltype(pdcp_cfg_s::out_of_order_delivery)::nof_types,
     decltype(pdcp_cfg_s::out_of_order_delivery)::nof_exts,
     nullptr,
     &out_of_order_delivery_opts::desc},
    {offsetof(pdcp_cfg_s, ie_exts),
     (int32_t)offsetof(pdcp_cfg_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_cfg_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_names[] = {{"pDCP-SN-Size-UL", nullptr},
                                                                     {"pDCP-SN-Size-DL", nullptr},
                                                                     {"rLC-Mode", nullptr},
                                                                     {"rOHC-Parameters", nullptr},
                                                                     {"t-ReorderingTimer", nullptr},
                                                                     {"discardTimer", nullptr},
                                                                     {"uLDataSplitThreshold", nullptr},
                                                                     {"pDCP-Duplication", nullptr},
                                                                     {"pDCP-Reestablishment", nullptr},
                                                                     {"pDCP-DataRecovery", nullptr},
                                                                     {"duplication-Activation", nullptr},
                                                                     {"outOfOrderDelivery", nullptr},
                                                                     {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_pdcp_cfg_s_opts[]  = {
    {3, (int32_t)offsetof(pdcp_cfg_s, rohc_params_present)},
    {4, (int32_t)offsetof(pdcp_cfg_s, t_reordering_timer_present)},
    {5, (int32_t)offsetof(pdcp_cfg_s, discard_timer_present)},
    {6, (int32_t)offsetof(pdcp_cfg_s, ul_data_split_thres_present)},
    {7, (int32_t)offsetof(pdcp_cfg_s, pdcp_dupl_present)},
    {8, (int32_t)offsetof(pdcp_cfg_s, pdcp_reest_present)},
    {9, (int32_t)offsetof(pdcp_cfg_s, pdcp_data_recovery_present)},
    {10, (int32_t)offsetof(pdcp_cfg_s, dupl_activation_present)},
    {11, (int32_t)offsetof(pdcp_cfg_s, out_of_order_delivery_present)},
    {12, (int32_t)offsetof(pdcp_cfg_s, ie_exts_present)}};
const asn1::table::seq_desc pdcp_cfg_s::desc = {
    {seq_table_pdcp_cfg_s_fields, 13, UINT64_C(0x7), seq_table_pdcp_cfg_s_opts, 10},
    true,
    true,
    nullptr,
    0,
    seq_table_pdcp_cfg_s_names};

static const uint32_t objset_id_non_dyn_5qi_descriptor_ext_ies_o[]   = {101, 102};
static const uint8_t  objset_crit_non_dyn_5qi_descriptor_ext_ies_o[] = {crit_e::ignore, crit_e::ignore};

// Non-Dynamic5QIDescriptor-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_non_dyn_5qi_descriptor_ext_ies_container_values[] = {
    {offsetof(non_dyn_5qi_descriptor_ext_ies_container, cn_packet_delay_budget_dl),
     (int32_t)offsetof(non_dyn_5qi_descriptor_ext_ies_container, cn_packet_delay_budget_dl_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(65535),
     nullptr},
    {offsetof(non_dyn_5qi_descriptor_ext_ies_container, cn_packet_delay_budget_ul),
     (int32_t)offsetof(non_dyn_5qi_descriptor_ext_ies_container, cn_packet_delay_budget_ul_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(65535),
     nullptr}};
static const int32_t ie_table_non_dyn_5qi_descriptor_ext_ies_container_presences[] = {
    (int32_t)offsetof(non_dyn_5qi_descriptor_ext_ies_container, cn_packet_delay_budget_dl_present),
    (int32_t)offsetof(non_dyn_5qi_descriptor_ext_ies_container, cn_packet_delay_budget_ul_present)};
const asn1::table::ie_container_desc non_dyn_5qi_descriptor_ext_ies_container::desc = {
    objset_id_non_dyn_5qi_descriptor_ext_ies_o,
    objset_crit_non_dyn_5qi_descriptor_ext_ies_o,
    ie_table_non_dyn_5qi_descriptor_ext_ies_container_values,
    ie_table_non_dyn_5qi_descriptor_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// Non-Dynamic5QIDescriptor ::= SEQUENCE
static const asn1::table::seq_field seq_table_non_dyn_5qi_descriptor_s_fields[] = {
    {offsetof(non_dyn_5qi_descriptor_s, five_qi),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(non_dyn_5qi_descriptor_s, qos_prio_level),
     (int32_t)offsetof(non_dyn_5qi_descriptor_s, qos_prio_level_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(non_dyn_5qi_descriptor_s, averaging_win),
     (int32_t)offsetof(non_dyn_5qi_descriptor_s, averaging_win_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(4095),
     nullptr},
    {offsetof(non_dyn_5qi_descriptor_s, max_data_burst_volume),
     (int32_t)offsetof(non_dyn_5qi_descriptor_s, max_data_burst_volume_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(4095),
     nullptr},
    {offsetof(non_dyn_5qi_descriptor_s, ie_exts),
     (int32_t)offsetof(non_dyn_5qi_descriptor_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &non_dyn_5qi_descriptor_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_non_dyn_5qi_descriptor_s_names[] = {{"fiveQI", nullptr},
                                                                                   {"qoSPriorityLevel", nullptr},
                                                                                   {"averagingWindow", nullptr},
                                                                                   {"maxDataBurstVolume", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_non_dyn_5qi_descriptor_s_opts[]  = {
    {1, (int32_t)offsetof(non_dyn_5qi_descriptor_s, qos_prio_level_present)},
    {2, (int32_t)offsetof(non_dyn_5qi_descriptor_s, averaging_win_present)},
    {3, (int32_t)offsetof(non_dyn_5qi_descriptor_s, max_data_burst_volume_present)},
    {4, (int32_t)offsetof(non_dyn_5qi_descriptor_s, ie_exts_present)}};
const asn1::table::seq_desc non_dyn_5qi_descriptor_s::desc = {
    {seq_table_non_dyn_5qi_descriptor_s_fields, 5, UINT64_C(0x1), seq_table_non_dyn_5qi_descriptor_s_opts, 4},
    false,
    true,
    nullptr,
    0,
    seq_table_non_dyn_5qi_descriptor_s_names};

static const uint32_t objset_id_dyn_5qi_descriptor_ext_ies_o[]   = {103, 101, 102};
static const uint8_t  objset_crit_dyn_5qi_descriptor_ext_ies_o[] = {crit_e::ignore, crit_e::ignore, crit_e::ignore};

// Dynamic5QIDescriptor-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_dyn_5qi_descriptor_ext_ies_container_values[] = {
    {offsetof(dyn_5qi_descriptor_ext_ies_container, extended_packet_delay_budget),
     (int32_t)offsetof(dyn_5qi_descriptor_ext_ies_container, extended_packet_delay_budget_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(65535),
     nullptr},
    {offsetof(dyn_5qi_descriptor_ext_ies_container, cn_packet_delay_budget_dl),
     (int32_t)offsetof(dyn_5qi_descriptor_ext_ies_container, cn_packet_delay_budget_dl_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(65535),
     nullptr},
    {offsetof(dyn_5qi_descriptor_ext_ies_container, cn_packet_delay_budget_ul),
     (int32_t)offsetof(dyn_5qi_descriptor_ext_ies_container, cn_packet_delay_budget_ul_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(65535),
     nullptr}};
static const int32_t ie_table_dyn_5qi_descriptor_ext_ies_container_presences[] = {
    (int32_t)offsetof(dyn_5qi_descriptor_ext_ies_container, extended_packet_delay_budget_present),
    (int32_t)offsetof(dyn_5qi_descriptor_ext_ies_container, cn_packet_delay_budget_dl_present),
    (int32_t)offsetof(dyn_5qi_descriptor_ext_ies_container, cn_packet_delay_budget_ul_present)};
const asn1::table::ie_container_desc dyn_5qi_descriptor_ext_ies_container::desc = {
    objset_id_dyn_5qi_descriptor_ext_ies_o,
    objset_crit_dyn_5qi_descriptor_ext_ies_o,
    ie_table_dyn_5qi_descriptor_ext_ies_container_values,
    ie_table_dyn_5qi_descriptor_ext_ies_container_presences,
    3,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// Dynamic5QIDescriptor ::= SEQUENCE
static const asn1::table::seq_field seq_table_dyn_5qi_descriptor_s_fields[] = {
    {offsetof(dyn_5qi_descriptor_s, qos_prio_level),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(dyn_5qi_descriptor_s, packet_delay_budget),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {offsetof(dyn_5qi_descriptor_s, packet_error_rate),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &packet_error_rate_s::desc},
    {offsetof(dyn_5qi_descriptor_s, five_qi),
     (int32_t)offsetof(dyn_5qi_descriptor_s, five_qi_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(dyn_5qi_descriptor_s, delay_crit),
     (int32_t)offsetof(dyn_5qi_descriptor_s, delay_crit_present),
     asn1::table::enum_op<decltype(dyn_5qi_descriptor_s::delay_crit)>(),
     decltype(dyn_5qi_descriptor_s::delay_crit)::has_ext,
     false,
     decltype(dyn_5qi_descriptor_s::delay_crit)::nof_types,
     decltype(dyn_5qi_descriptor_s::delay_crit)::nof_exts,
     nullptr,
     &dyn_5qi_descriptor_s::delay_crit_opts::desc},
    {offsetof(dyn_5qi_descriptor_s, averaging_win),
     (int32_t)offsetof(dyn_5qi_descriptor_s, averaging_win_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(4095),
     nullptr},
    {offsetof(dyn_5qi_descriptor_s, max_data_burst_volume),
     (int32_t)offsetof(dyn_5qi_descriptor_s, max_data_burst_volume_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(4095),
     nullptr},
    {offsetof(dyn_5qi_descriptor_s, ie_exts),
     (int32_t)offsetof(dyn_5qi_descriptor_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &dyn_5qi_descriptor_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_dyn_5qi_descriptor_s_names[] = {{"qoSPriorityLevel", nullptr},
                                                                               {"packetDelayBudget", nullptr},
                                                                               {"packetErrorRate", nullptr},
                                                                               {"fiveQI", nullptr},
                                                                               {"delayCritical", nullptr},
                                                                               {"averagingWindow", nullptr},
                                                                               {"maxDataBurstVolume", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_dyn_5qi_descriptor_s_opts[]  = {
    {3, (int32_t)offsetof(dyn_5qi_descriptor_s, five_qi_present)},
    {4, (int32_t)offsetof(dyn_5qi_descriptor_s, delay_crit_present)},
    {5, (int32_t)offsetof(dyn_5qi_descriptor_s, averaging_win_present)},
    {6, (int32_t)offsetof(dyn_5qi_descriptor_s, max_data_burst_volume_present)},
    {7, (int32_t)offsetof(dyn_5qi_descriptor_s, ie_exts_present)}};
const asn1::table::seq_desc dyn_5qi_descriptor_s::desc = {
    {seq_table_dyn_5qi_descriptor_s_fields, 8, UINT64_C(0x7), seq_table_dyn_5qi_descriptor_s_opts, 5},
    false,
    true,
    nullptr,
    0,
    seq_table_dyn_5qi_descriptor_s_names};

static const char* enum_names_dyn_5qi_descriptor_s_delay_crit_e__values[] = {"delay-critical", "non-delay-critical"};
const asn1::table::enum_desc dyn_5qi_descriptor_s::delay_crit_opts::desc  = {
    enum_names_dyn_5qi_descriptor_s_delay_crit_e__values,
    2,
    "dyn_5qi_descriptor_s::delay_crit_e_"};

// QoS-Characteristics ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_qos_characteristics_c_alternatives[] = {
    &asn1::table::alternative_ops<non_dyn_5qi_descriptor_s, false>::value,
    &asn1::table::alternative_ops<dyn_5qi_descriptor_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc qos_characteristics_c::storage_desc = {
    choice_storage_qos_characteristics_c_alternatives,
    3,
    "qos_characteristics_c"};

non_dyn_5qi_descriptor_s& qos_characteristics_c::set_non_dyn_5qi()
{
  set(types::non_dyn_5qi);
  return c.get<non_dyn_5qi_descriptor_s>();
}
dyn_5qi_descriptor_s& qos_characteristics_c::set_dyn_5qi()
{
  set(types::dyn_5qi);
  return c.get<dyn_5qi_descriptor_s>();
}
protocol_ie_single_container_s<qos_characteristics_ext_ies_o>& qos_characteristics_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_qos_characteristics_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &non_dyn_5qi_descriptor_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &dyn_5qi_descriptor_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<qos_characteristics_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_qos_characteristics_c_names[] = {{"non-Dynamic-5QI", nullptr},
                                                                                   {"dynamic-5QI", nullptr},
                                                                                   {"choice-extension", nullptr}};
const asn1::table::choice_desc       qos_characteristics_c::desc                = {
    asn1::table::enum_int_type<qos_characteristics_c::types>(),
    qos_characteristics_c::types::nof_types,
    qos_characteristics_c::types::nof_exts,
    qos_characteristics_c::types::has_ext,
    true,
    choice_table_qos_characteristics_c_alternatives,
    nullptr,
    choice_table_qos_characteristics_c_names,
    "qos_characteristics_c",
    offsetof(qos_characteristics_c, c.type),
    offsetof(qos_characteristics_c, c.value),
    false,
    &qos_characteristics_c::storage_desc};

static const char*           enum_names_qos_characteristics_c_types_values[] = {"non-Dynamic-5QI",
                                                                                "dynamic-5QI",
                                                                                "choice-extension"};
const asn1::table::enum_desc qos_characteristics_c::types_opts::desc = {enum_names_qos_characteristics_c_types_values,
                                                                        3,
                                                                        "qos_characteristics_c::types"};

// Pre-emptionCapability ::= ENUMERATED
static const char* enum_names_pre_emption_cap_e_values[] = {"shall-not-trigger-pre-emption", "may-trigger-pre-emption"};
const asn1::table::enum_desc pre_emption_cap_opts::desc = {enum_names_pre_emption_cap_e_values, 2, "pre_emption_cap_e"};

// Pre-emptionVulnerability ::= ENUMERATED
static const char*           enum_names_pre_emption_vulnerability_e_values[] = {"not-pre-emptable", "pre-emptable"};
const asn1::table::enum_desc pre_emption_vulnerability_opts::desc = {enum_names_pre_emption_vulnerability_e_values,
                                                                     2,
                                                                     "pre_emption_vulnerability_e"};

// NGRANAllocationAndRetentionPriority ::= SEQUENCE
static const asn1::table::seq_field seq_table_ngran_alloc_and_retention_prio_s_fields[] = {
    {offsetof(ngran_alloc_and_retention_prio_s, prio_level),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(15),
     nullptr},
    {offsetof(ngran_alloc_and_retention_prio_s, pre_emption_cap),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ngran_alloc_and_retention_prio_s::pre_emption_cap)>(),
     decltype(ngran_alloc_and_retention_prio_s::pre_emption_cap)::has_ext,
     false,
     decltype(ngran_alloc_and_retention_prio_s::pre_emption_cap)::nof_types,
     decltype(ngran_alloc_and_retention_prio_s::pre_emption_cap)::nof_exts,
     nullptr,
     &pre_emption_cap_opts::desc},
    {offsetof(ngran_alloc_and_retention_prio_s, pre_emption_vulnerability),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ngran_alloc_and_retention_prio_s::pre_emption_vulnerability)>(),
     decltype(ngran_alloc_and_retention_prio_s::pre_emption_vulnerability)::has_ext,
     false,
     decltype(ngran_alloc_and_retention_prio_s::pre_emption_vulnerability)::nof_types,
     decltype(ngran_alloc_and_retention_prio_s::pre_emption_vulnerability)::nof_exts,
     nullptr,
     &pre_emption_vulnerability_opts::desc},
    {offsetof(ngran_alloc_and_retention_prio_s, ie_exts),
     (int32_t)offsetof(ngran_alloc_and_retention_prio_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ngran_alloc_and_retention_prio_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ngran_alloc_and_retention_prio_s_names[] = {
    {"priorityLevel", nullptr},
    {"pre-emptionCapability", nullptr},
    {"pre-emptionVulnerability", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_ngran_alloc_and_retention_prio_s_opts[] = {
    {3, (int32_t)offsetof(ngran_alloc_and_retention_prio_s, ie_exts_present)}};
const asn1::table::seq_desc ngran_alloc_and_retention_prio_s::desc = {
    {seq_table_ngran_alloc_and_retention_prio_s_fields,
     4,
     UINT64_C(0x7),
     seq_table_ngran_alloc_and_retention_prio_s_opts,
     1},
    false,
    true,
    nullptr,
    0,
    seq_table_ngran_alloc_and_retention_prio_s_names};

static const uint32_t objset_id_gbr_qos_flow_info_ext_ies_o[]   = {124};
static const uint8_t  objset_crit_gbr_qos_flow_info_ext_ies_o[] = {crit_e::ignore};

// GBR-QosFlowInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_gbr_qos_flow_info_ext_ies_container_values[] = {
    {offsetof(gbr_qos_flow_info_ext_ies_container, alt_qos_para_set_list),
     (int32_t)offsetof(gbr_qos_flow_info_ext_ies_container, alt_qos_para_set_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(gbr_qos_flow_info_ext_ies_container::alt_qos_para_set_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &alt_qos_para_set_item_s::desc}};
static const int32_t ie_table_gbr_qos_flow_info_ext_ies_container_presences[] = {
    (int32_t)offsetof(gbr_qos_flow_info_ext_ies_container, alt_qos_para_set_list_present)};
const asn1::table::ie_container_desc gbr_qos_flow_info_ext_ies_container::desc = {
    objset_id_gbr_qos_flow_info_ext_ies_o,
    objset_crit_gbr_qos_flow_info_ext_ies_o,
    ie_table_gbr_qos_flow_info_ext_ies_container_values,
    ie_table_gbr_qos_flow_info_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// GBR-QoSFlowInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_gbr_qos_flow_info_s_fields[] = {
    {offsetof(gbr_qos_flow_info_s, max_flow_bit_rate_dl),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     INT64_C(0),
     UINT64_C(4000000000000),
     nullptr},
    {offsetof(gbr_qos_flow_info_s, max_flow_bit_rate_ul),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     INT64_C(0),
     UINT64_C(4000000000000),
     nullptr},
    {offsetof(gbr_qos_flow_info_s, guaranteed_flow_bit_rate_dl),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     INT64_C(0),
     UINT64_C(4000000000000),
     nullptr},
    {offsetof(gbr_qos_flow_info_s, guaranteed_flow_bit_rate_ul),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     INT64_C(0),
     UINT64_C(4000000000000),
     nullptr},
    {offsetof(gbr_qos_flow_info_s, max_packet_loss_rate_dl),
     (int32_t)offsetof(gbr_qos_flow_info_s, max_packet_loss_rate_dl_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(1000),
     nullptr},
    {offsetof(gbr_qos_flow_info_s, max_packet_loss_rate_ul),
     (int32_t)offsetof(gbr_qos_flow_info_s, max_packet_loss_rate_ul_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(1000),
     nullptr},
    {offsetof(gbr_qos_flow_info_s, ie_exts),
     (int32_t)offsetof(gbr_qos_flow_info_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &gbr_qos_flow_info_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_gbr_qos_flow_info_s_names[] = {
    {"maxFlowBitRateDownlink", nullptr},
    {"maxFlowBitRateUplink", nullptr},
    {"guaranteedFlowBitRateDownlink", nullptr},
    {"guaranteedFlowBitRateUplink", nullptr},
    {"maxPacketLossRateDownlink", nullptr},
    {"maxPacketLossRateUplink", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_gbr_qos_flow_info_s_opts[] = {
    {4, (int32_t)offsetof(gbr_qos_flow_info_s, max_packet_loss_rate_dl_present)},
    {5, (int32_t)offsetof(gbr_qos_flow_info_s, max_packet_loss_rate_ul_present)},
    {6, (int32_t)offsetof(gbr_qos_flow_info_s, ie_exts_present)}};
const asn1::table::seq_desc gbr_qos_flow_info_s::desc = {
    {seq_table_gbr_qos_flow_info_s_fields, 7, UINT64_C(0xf), seq_table_gbr_qos_flow_info_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_gbr_qos_flow_info_s_names};

// QosMonitoringRequest ::= ENUMERATED
static const char*           enum_names_qos_monitoring_request_e_values[] = {"ul", "dl", "both"};
const asn1::table::enum_desc qos_monitoring_request_opts::desc            = {enum_names_qos_monitoring_request_e_values,
                                                                             3,
                                                                             "qos_monitoring_request_e"};

// QosMonitoringDisabled ::= ENUMERATED
static const char*           enum_names_qos_monitoring_disabled_e_values[] = {"true"};
const asn1::table::enum_desc qos_monitoring_disabled_opts::desc = {enum_names_qos_monitoring_disabled_e_values,
                                                                   1,
                                                                   "qos_monitoring_disabled_e"};

// PDUSetQoSInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_set_qos_info_s_fields[] = {
    {offsetof(pdu_set_qos_info_s, pdu_set_delay_budget),
     (int32_t)offsetof(pdu_set_qos_info_s, pdu_set_delay_budget_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(65535),
     nullptr},
    {offsetof(pdu_set_qos_info_s, pdu_set_error_rate),
     (int32_t)offsetof(pdu_set_qos_info_s, pdu_set_error_rate_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &packet_error_rate_s::desc},
    {offsetof(pdu_set_qos_info_s, pdu_set_integrated_handling_info),
     (int32_t)offsetof(pdu_set_qos_info_s, pdu_set_integrated_handling_info_present),
     asn1::table::enum_op<decltype(pdu_set_qos_info_s::pdu_set_integrated_handling_info)>(),
     decltype(pdu_set_qos_info_s::pdu_set_integrated_handling_info)::has_ext,
     false,
     decltype(pdu_set_qos_info_s::pdu_set_integrated_handling_info)::nof_types,
     decltype(pdu_set_qos_info_s::pdu_set_integrated_handling_info)::nof_exts,
     nullptr,
     &pdu_set_qos_info_s::pdu_set_integrated_handling_info_opts::desc},
    {offsetof(pdu_set_qos_info_s, ie_exts),
     (int32_t)offsetof(pdu_set_qos_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pdu_set_qos_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pdu_set_qos_info_s_names[] = {
    {"pduSetDelayBudget", nullptr},
    {"pduSetErrorRate", nullptr},
    {"pduSetIntegratedHandlingInformation", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pdu_set_qos_info_s_opts[] = {
    {0, (int32_t)offsetof(pdu_set_qos_info_s, pdu_set_delay_budget_present)},
    {1, (int32_t)offsetof(pdu_set_qos_info_s, pdu_set_error_rate_present)},
    {2, (int32_t)offsetof(pdu_set_qos_info_s, pdu_set_integrated_handling_info_present)},
    {3, (int32_t)offsetof(pdu_set_qos_info_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_set_qos_info_s::desc = {
    {seq_table_pdu_set_qos_info_s_fields, 4, UINT64_C(0x0), seq_table_pdu_set_qos_info_s_opts, 4},
    false,
    true,
    nullptr,
    0,
    seq_table_pdu_set_qos_info_s_names};

static const char* enum_names_pdu_set_qos_info_s_pdu_set_integrated_handling_info_e__values[] = {"true", "false"};
const asn1::table::enum_desc pdu_set_qos_info_s::pdu_set_integrated_handling_info_opts::desc  = {
    enum_names_pdu_set_qos_info_s_pdu_set_integrated_handling_info_e__values,
    2,
    "pdu_set_qos_info_s::pdu_set_integrated_handling_info_e_"};

// PDUSetQoSParameters ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_set_qos_params_s_fields[] = {
    {offsetof(pdu_set_qos_params_s, ul_pdu_set_qos_info),
     (int32_t)offsetof(pdu_set_qos_params_s, ul_pdu_set_qos_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdu_set_qos_info_s::desc},
    {offsetof(pdu_set_qos_params_s, dl_pdu_set_qos_info),
     (int32_t)offsetof(pdu_set_qos_params_s, dl_pdu_set_qos_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdu_set_qos_info_s::desc},
    {offsetof(pdu_set_qos_params_s, ie_exts),
     (int32_t)offsetof(pdu_set_qos_params_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pdu_set_qos_params_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pdu_set_qos_params_s_names[] = {{"ulPDUSetQoSInformation", nullptr},
                                                                               {"dlPDUSetQoSInformation", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_pdu_set_qos_params_s_opts[]  = {
    {0, (int32_t)offsetof(pdu_set_qos_params_s, ul_pdu_set_qos_info_present)},
    {1, (int32_t)offsetof(pdu_set_qos_params_s, dl_pdu_set_qos_info_present)},
    {2, (int32_t)offsetof(pdu_set_qos_params_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_set_qos_params_s::desc = {
    {seq_table_pdu_set_qos_params_s_fields, 3, UINT64_C(0x0), seq_table_pdu_set_qos_params_s_opts, 3},
    false,
    true,
    nullptr,
    0,
    seq_table_pdu_set_qos_params_s_names};

static const uint32_t objset_id_qos_flow_level_qos_params_ext_ies_o[] = {87, 126, 132, 133, 142, 201};
static const uint8_t  objset_crit_qos_flow_level_qos_params_ext_ies_o[] =
    {crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// QoSFlowLevelQoSParameters-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_qos_flow_level_qos_params_ext_ies_container_values[] = {
    {offsetof(qos_flow_level_qos_params_ext_ies_container, qos_monitoring_request),
     (int32_t)offsetof(qos_flow_level_qos_params_ext_ies_container, qos_monitoring_request_present),
     asn1::table::enum_op<decltype(qos_flow_level_qos_params_ext_ies_container::qos_monitoring_request)>(),
     decltype(qos_flow_level_qos_params_ext_ies_container::qos_monitoring_request)::has_ext,
     false,
     decltype(qos_flow_level_qos_params_ext_ies_container::qos_monitoring_request)::nof_types,
     decltype(qos_flow_level_qos_params_ext_ies_container::qos_monitoring_request)::nof_exts,
     nullptr,
     &qos_monitoring_request_opts::desc},
    {offsetof(qos_flow_level_qos_params_ext_ies_container, mcg_offered_gbr_qos_flow_info),
     (int32_t)offsetof(qos_flow_level_qos_params_ext_ies_container, mcg_offered_gbr_qos_flow_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gbr_qos_flow_info_s::desc},
    {offsetof(qos_flow_level_qos_params_ext_ies_container, qos_monitoring_report_freq),
     (int32_t)offsetof(qos_flow_level_qos_params_ext_ies_container, qos_monitoring_report_freq_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(1800),
     nullptr},
    {offsetof(qos_flow_level_qos_params_ext_ies_container, qos_monitoring_disabled),
     (int32_t)offsetof(qos_flow_level_qos_params_ext_ies_container, qos_monitoring_disabled_present),
     asn1::table::enum_op<decltype(qos_flow_level_qos_params_ext_ies_container::qos_monitoring_disabled)>(),
     decltype(qos_flow_level_qos_params_ext_ies_container::qos_monitoring_disabled)::has_ext,
     false,
     decltype(qos_flow_level_qos_params_ext_ies_container::qos_monitoring_disabled)::nof_types,
     decltype(qos_flow_level_qos_params_ext_ies_container::qos_monitoring_disabled)::nof_exts,
     nullptr,
     &qos_monitoring_disabled_opts::desc},
    {offsetof(qos_flow_level_qos_params_ext_ies_container, data_forwarding_source_ip_address),
     (int32_t)offsetof(qos_flow_level_qos_params_ext_ies_container, data_forwarding_source_ip_address_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<
         decltype(qos_flow_level_qos_params_ext_ies_container::data_forwarding_source_ip_address),
         asn1::table::struct_json>::value},
    {offsetof(qos_flow_level_qos_params_ext_ies_container, pdu_set_qos_params),
     (int32_t)offsetof(qos_flow_level_qos_params_ext_ies_container, pdu_set_qos_params_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdu_set_qos_params_s::desc}};
static const int32_t ie_table_qos_flow_level_qos_params_ext_ies_container_presences[] = {
    (int32_t)offsetof(qos_flow_level_qos_params_ext_ies_container, qos_monitoring_request_present),
    (int32_t)offsetof(qos_flow_level_qos_params_ext_ies_container, mcg_offered_gbr_qos_flow_info_present),
    (int32_t)offsetof(qos_flow_level_qos_params_ext_ies_container, qos_monitoring_report_freq_present),
    (int32_t)offsetof(qos_flow_level_qos_params_ext_ies_container, qos_monitoring_disabled_present),
    (int32_t)offsetof(qos_flow_level_qos_params_ext_ies_container, data_forwarding_source_ip_address_present),
    (int32_t)offsetof(qos_flow_level_qos_params_ext_ies_container, pdu_set_qos_params_present)};
const asn1::table::ie_container_desc qos_flow_level_qos_params_ext_ies_container::desc = {
    objset_id_qos_flow_level_qos_params_ext_ies_o,
    objset_crit_qos_flow_level_qos_params_ext_ies_o,
    ie_table_qos_flow_level_qos_params_ext_ies_container_values,
    ie_table_qos_flow_level_qos_params_ext_ies_container_presences,
    6,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// QoSFlowLevelQoSParameters ::= SEQUENCE
static const asn1::table::seq_field seq_table_qos_flow_level_qos_params_s_fields[] = {
    {offsetof(qos_flow_level_qos_params_s, qos_characteristics),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &qos_characteristics_c::desc},
    {offsetof(qos_flow_level_qos_params_s, ngran_alloc_retention_prio),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ngran_alloc_and_retention_prio_s::desc},
    {offsetof(qos_flow_level_qos_params_s, gbr_qos_flow_info),
     (int32_t)offsetof(qos_flow_level_qos_params_s, gbr_qos_flow_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gbr_qos_flow_info_s::desc},
    {offsetof(qos_flow_level_qos_params_s, reflective_qos_attribute),
     (int32_t)offsetof(qos_flow_level_qos_params_s, reflective_qos_attribute_present),
     asn1::table::enum_op<decltype(qos_flow_level_qos_params_s::reflective_qos_attribute)>(),
     decltype(qos_flow_level_qos_params_s::reflective_qos_attribute)::has_ext,
     false,
     decltype(qos_flow_level_qos_params_s::reflective_qos_attribute)::nof_types,
     decltype(qos_flow_level_qos_params_s::reflective_qos_attribute)::nof_exts,
     nullptr,
     &qos_flow_level_qos_params_s::reflective_qos_attribute_opts::desc},
    {offsetof(qos_flow_level_qos_params_s, add_qos_info),
     (int32_t)offsetof(qos_flow_level_qos_params_s, add_qos_info_present),
     asn1::table::enum_op<decltype(qos_flow_level_qos_params_s::add_qos_info)>(),
     decltype(qos_flow_level_qos_params_s::add_qos_info)::has_ext,
     false,
     decltype(qos_flow_level_qos_params_s::add_qos_info)::nof_types,
     decltype(qos_flow_level_qos_params_s::add_qos_info)::nof_exts,
     nullptr,
     &qos_flow_level_qos_params_s::add_qos_info_opts::desc},
    {offsetof(qos_flow_level_qos_params_s, paging_policy_idx),
     (int32_t)offsetof(qos_flow_level_qos_params_s, paging_policy_idx_present),
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(8),
     nullptr},
    {offsetof(qos_flow_level_qos_params_s, reflective_qos_ind),
     (int32_t)offsetof(qos_flow_level_qos_params_s, reflective_qos_ind_present),
     asn1::table::enum_op<decltype(qos_flow_level_qos_params_s::reflective_qos_ind)>(),
     decltype(qos_flow_level_qos_params_s::reflective_qos_ind)::has_ext,
     false,
     decltype(qos_flow_level_qos_params_s::reflective_qos_ind)::nof_types,
     decltype(qos_flow_level_qos_params_s::reflective_qos_ind)::nof_exts,
     nullptr,
     &qos_flow_level_qos_params_s::reflective_qos_ind_opts::desc},
    {offsetof(qos_flow_level_qos_params_s, ie_exts),
     (int32_t)offsetof(qos_flow_level_qos_params_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &qos_flow_level_qos_params_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_qos_flow_level_qos_params_s_names[] = {
    {"qoS-Characteristics", nullptr},
    {"nGRANallocationRetentionPriority", nullptr},
    {"gBR-QoS-Flow-Information", nullptr},
    {"reflective-QoS-Attribute", nullptr},
    {"additional-QoS-Information", nullptr},
    {"paging-Policy-Index", nullptr},
    {"reflective-QoS-Indicator", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_qos_flow_level_qos_params_s_opts[] = {
    {2, (int32_t)offsetof(qos_flow_level_qos_params_s, gbr_qos_flow_info_present)},
    {3, (int32_t)offsetof(qos_flow_level_qos_params_s, reflective_qos_attribute_present)},
    {4, (int32_t)offsetof(qos_flow_level_qos_params_s, add_qos_info_present)},
    {5, (int32_t)offsetof(qos_flow_level_qos_params_s, paging_policy_idx_present)},
    {6, (int32_t)offsetof(qos_flow_level_qos_params_s, reflective_qos_ind_present)},
    {7, (int32_t)offsetof(qos_flow_level_qos_params_s, ie_exts_present)}};
const asn1::table::seq_desc qos_flow_level_qos_params_s::desc = {
    {seq_table_qos_flow_level_qos_params_s_fields, 8, UINT64_C(0x3), seq_table_qos_flow_level_qos_params_s_opts, 6},
    false,
    true,
    nullptr,
    0,
    seq_table_qos_flow_level_qos_params_s_names};

static const char* enum_names_qos_flow_level_qos_params_s_reflective_qos_attribute_e__values[] = {"subject-to"};
const asn1::table::enum_desc qos_flow_level_qos_params_s::reflective_qos_attribute_opts::desc  = {
    enum_names_qos_flow_level_qos_params_s_reflective_qos_attribute_e__values,
    1,
    "qos_flow_level_qos_params_s::reflective_qos_attribute_e_"};

static const char*           enum_names_qos_flow_level_qos_params_s_add_qos_info_e__values[] = {"more-likely"};
const asn1::table::enum_desc qos_flow_level_qos_params_s::add_qos_info_opts::desc            = {
    enum_names_qos_flow_level_qos_params_s_add_qos_info_e__values,
    1,
    "qos_flow_level_qos_params_s::add_qos_info_e_"};

static const char*           enum_names_qos_flow_level_qos_params_s_reflective_qos_ind_e__values[] = {"enabled"};
const asn1::table::enum_desc qos_flow_level_qos_params_s::reflective_qos_ind_opts::desc            = {
    enum_names_qos_flow_level_qos_params_s_reflective_qos_ind_e__values,
    1,
    "qos_flow_level_qos_params_s::reflective_qos_ind_e_"};

// QoS-Flow-Mapping-Indication ::= ENUMERATED
static const char*           enum_names_qos_flow_map_ind_e_values[] = {"ul", "dl"};
const asn1::table::enum_desc qos_flow_map_ind_opts::desc            = {enum_names_qos_flow_map_ind_e_values,
                                                                       2,
                                                                       "qos_flow_map_ind_e"};

// RedundantQoSFlowIndicator ::= ENUMERATED
static const char*           enum_names_redundant_qos_flow_ind_e_values[] = {"true", "false"};
const asn1::table::enum_desc redundant_qos_flow_ind_opts::desc            = {enum_names_redundant_qos_flow_ind_e_values,
                                                                             2,
                                                                             "redundant_qos_flow_ind_e"};

// N6JitterInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_n6_jitter_info_s_fields[] = {
    {offsetof(n6_jitter_info_s, n6_jitter_lower_bound),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i16,
     false,
     true,
     INT64_C(-127),
     UINT64_C(127),
     nullptr},
    {offsetof(n6_jitter_info_s, n6_jitter_upper_bound),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i16,
     false,
     true,
     INT64_C(-127),
     UINT64_C(127),
     nullptr},
    {offsetof(n6_jitter_info_s, ie_exts),
     (int32_t)offsetof(n6_jitter_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(n6_jitter_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_n6_jitter_info_s_names[] = {{"n6JitterLowerBound", nullptr},
                                                                           {"n6JitterUpperBound", nullptr},
                                                                           {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_n6_jitter_info_s_opts[]  = {
    {2, (int32_t)offsetof(n6_jitter_info_s, ie_exts_present)}};
const asn1::table::seq_desc n6_jitter_info_s::desc = {
    {seq_table_n6_jitter_info_s_fields, 3, UINT64_C(0x3), seq_table_n6_jitter_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_n6_jitter_info_s_names};

static const uint32_t objset_id_tsc_assist_info_ext_ies_o[]   = {152, 202};
static const uint8_t  objset_crit_tsc_assist_info_ext_ies_o[] = {crit_e::ignore, crit_e::ignore};

// TSCAssistanceInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_tsc_assist_info_ext_ies_container_values[] = {
    {offsetof(tsc_assist_info_ext_ies_container, survival_time),
     (int32_t)offsetof(tsc_assist_info_ext_ies_container, survival_time_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(1920000),
     nullptr},
    {offsetof(tsc_assist_info_ext_ies_container, n6_jitter_info),
     (int32_t)offsetof(tsc_assist_info_ext_ies_container, n6_jitter_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &n6_jitter_info_s::desc}};
static const int32_t ie_table_tsc_assist_info_ext_ies_container_presences[] = {
    (int32_t)offsetof(tsc_assist_info_ext_ies_container, survival_time_present),
    (int32_t)offsetof(tsc_assist_info_ext_ies_container, n6_jitter_info_present)};
const asn1::table::ie_container_desc tsc_assist_info_ext_ies_container::desc = {
    objset_id_tsc_assist_info_ext_ies_o,
    objset_crit_tsc_assist_info_ext_ies_o,
    ie_table_tsc_assist_info_ext_ies_container_values,
    ie_table_tsc_assist_info_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// TSCAssistanceInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_tsc_assist_info_s_fields[] = {
    {offsetof(tsc_assist_info_s, periodicity),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(640000),
     nullptr},
    {offsetof(tsc_assist_info_s, burst_arrival_time),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(tsc_assist_info_s::burst_arrival_time),
                                      asn1::table::string_json>::value},
    {offsetof(tsc_assist_info_s, ie_exts),
     (int32_t)offsetof(tsc_assist_info_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &tsc_assist_info_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_tsc_assist_info_s_names[] = {{"periodicity", nullptr},
                                                                            {"burstArrivalTime", nullptr},
                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_tsc_assist_info_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(tsc_assist_info_s, ie_exts_present)}};
const asn1::table::seq_desc tsc_assist_info_s::desc = {
    {seq_table_tsc_assist_info_s_fields, 3, UINT64_C(0x1), seq_table_tsc_assist_info_s_opts, 2},
    false,
    true,
    nullptr,
    0,
    seq_table_tsc_assist_info_s_names};

// TSCTrafficCharacteristics ::= SEQUENCE
static const asn1::table::seq_field seq_table_tsc_traffic_characteristics_s_fields[] = {
    {offsetof(tsc_traffic_characteristics_s, tsc_traffic_characteristics_ul),
     (int32_t)offsetof(tsc_traffic_characteristics_s, tsc_traffic_characteristics_ul_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tsc_assist_info_s::desc},
    {offsetof(tsc_traffic_characteristics_s, tsc_traffic_characteristics_dl),
     (int32_t)offsetof(tsc_traffic_characteristics_s, tsc_traffic_characteristics_dl_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tsc_assist_info_s::desc},
    {offsetof(tsc_traffic_characteristics_s, ie_exts),
     (int32_t)offsetof(tsc_traffic_characteristics_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tsc_traffic_characteristics_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_tsc_traffic_characteristics_s_names[] = {
    {"tSCTrafficCharacteristicsUL", nullptr},
    {"tSCTrafficCharacteristicsDL", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_tsc_traffic_characteristics_s_opts[] = {
    {0, (int32_t)offsetof(tsc_traffic_characteristics_s, tsc_traffic_characteristics_ul_present)},
    {1, (int32_t)offsetof(tsc_traffic_characteristics_s, tsc_traffic_characteristics_dl_present)},
    {2, (int32_t)offsetof(tsc_traffic_characteristics_s, ie_exts_present)}};
const asn1::table::seq_desc tsc_traffic_characteristics_s::desc = {
    {seq_table_tsc_traffic_characteristics_s_fields, 3, UINT64_C(0x0), seq_table_tsc_traffic_characteristics_s_opts, 3},
    false,
    true,
    nullptr,
    0,
    seq_table_tsc_traffic_characteristics_s_names};

// ECNMarkingorCongestionInformationReportingRequest ::= CHOICE
static const asn1::table::storage_ops* const
    choice_storage_ecn_markingor_congestion_info_report_request_c_alternatives[] = {
        &asn1::table::alternative_ops<ecn_markingor_congestion_info_report_request_c::ecn_markingat_ngran_e_,
                                      true>::value,
        &asn1::table::alternative_ops<ecn_markingor_congestion_info_report_request_c::ecn_markingat_up_f_e_,
                                      true>::value,
        &asn1::table::alternative_ops<ecn_markingor_congestion_info_report_request_c::congestion_info_e_, true>::value,
        &asn1::table::alternative_ops<
            protocol_ie_single_container_s<ecn_markingor_congestion_info_report_request_ext_ies_o>,
            false>::value};
const asn1::table::choice_storage_desc ecn_markingor_congestion_info_report_request_c::storage_desc = {
    choice_storage_ecn_markingor_congestion_info_report_request_c_alternatives,
    4,
    "ecn_markingor_congestion_info_report_request_c"};

ecn_markingor_congestion_info_report_request_c::ecn_markingat_ngran_e_&
ecn_markingor_congestion_info_report_request_c::set_ecn_markingat_ngran()
{
  set(types::ecn_markingat_ngran);
  return c.get<ecn_markingat_ngran_e_>();
}
ecn_markingor_congestion_info_report_request_c::ecn_markingat_up_f_e_&
ecn_markingor_congestion_info_report_request_c::set_ecn_markingat_up_f()
{
  set(types::ecn_markingat_up_f);
  return c.get<ecn_markingat_up_f_e_>();
}
ecn_markingor_congestion_info_report_request_c::congestion_info_e_&
ecn_markingor_congestion_info_report_request_c::set_congestion_info()
{
  set(types::congestion_info);
  return c.get<congestion_info_e_>();
}
protocol_ie_single_container_s<ecn_markingor_congestion_info_report_request_ext_ies_o>&
ecn_markingor_congestion_info_report_request_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<ecn_markingor_congestion_info_report_request_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_ecn_markingor_congestion_info_report_request_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<ecn_markingor_congestion_info_report_request_c::ecn_markingat_ngran_e_>(),
     ecn_markingor_congestion_info_report_request_c::ecn_markingat_ngran_e_::has_ext,
     false,
     ecn_markingor_congestion_info_report_request_c::ecn_markingat_ngran_e_::nof_types,
     ecn_markingor_congestion_info_report_request_c::ecn_markingat_ngran_e_::nof_exts,
     nullptr,
     &ecn_markingor_congestion_info_report_request_c::ecn_markingat_ngran_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<ecn_markingor_congestion_info_report_request_c::ecn_markingat_up_f_e_>(),
     ecn_markingor_congestion_info_report_request_c::ecn_markingat_up_f_e_::has_ext,
     false,
     ecn_markingor_congestion_info_report_request_c::ecn_markingat_up_f_e_::nof_types,
     ecn_markingor_congestion_info_report_request_c::ecn_markingat_up_f_e_::nof_exts,
     nullptr,
     &ecn_markingor_congestion_info_report_request_c::ecn_markingat_up_f_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<ecn_markingor_congestion_info_report_request_c::congestion_info_e_>(),
     ecn_markingor_congestion_info_report_request_c::congestion_info_e_::has_ext,
     false,
     ecn_markingor_congestion_info_report_request_c::congestion_info_e_::nof_types,
     ecn_markingor_congestion_info_report_request_c::congestion_info_e_::nof_exts,
     nullptr,
     &ecn_markingor_congestion_info_report_request_c::congestion_info_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<ecn_markingor_congestion_info_report_request_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_ecn_markingor_congestion_info_report_request_c_names[] = {
    {"eCNMarkingatNGRAN", nullptr},
    {"eCNMarkingatUPF", nullptr},
    {"congestionInformation", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc ecn_markingor_congestion_info_report_request_c::desc = {
    asn1::table::enum_int_type<ecn_markingor_congestion_info_report_request_c::types>(),
    ecn_markingor_congestion_info_report_request_c::types::nof_types,
    ecn_markingor_congestion_info_report_request_c::types::nof_exts,
    ecn_markingor_congestion_info_report_request_c::types::has_ext,
    true,
    choice_table_ecn_markingor_congestion_info_report_request_c_alternatives,
    nullptr,
    choice_table_ecn_markingor_congestion_info_report_request_c_names,
    "ecn_markingor_congestion_info_report_request_c",
    offsetof(ecn_markingor_congestion_info_report_request_c, c.type),
    offsetof(ecn_markingor_congestion_info_report_request_c, c.value),
    false,
    &ecn_markingor_congestion_info_report_request_c::storage_desc};

static const char* enum_names_ecn_markingor_congestion_info_report_request_c_ecn_markingat_ngran_e__values[] = {"ul",
                                                                                                                "dl",
                                                                                                                "both",
                                                                                                                "stop"};
const asn1::table::enum_desc ecn_markingor_congestion_info_report_request_c::ecn_markingat_ngran_opts::desc  = {
    enum_names_ecn_markingor_congestion_info_report_request_c_ecn_markingat_ngran_e__values,
    4,
    "ecn_markingor_congestion_info_report_request_c::ecn_markingat_ngran_e_"};

static const char* enum_names_ecn_markingor_congestion_info_report_request_c_ecn_markingat_up_f_e__values[] = {"ul",
                                                                                                               "dl",
                                                                                                               "both",
                                                                                                               "stop"};
const asn1::table::enum_desc ecn_markingor_congestion_info_report_request_c::ecn_markingat_up_f_opts::desc  = {
    enum_names_ecn_markingor_congestion_info_report_request_c_ecn_markingat_up_f_e__values,
    4,
    "ecn_markingor_congestion_info_report_request_c::ecn_markingat_up_f_e_"};

static const char* enum_names_ecn_markingor_congestion_info_report_request_c_congestion_info_e__values[] = {"ul",
                                                                                                            "dl",
                                                                                                            "both",
                                                                                                            "stop"};
const asn1::table::enum_desc ecn_markingor_congestion_info_report_request_c::congestion_info_opts::desc  = {
    enum_names_ecn_markingor_congestion_info_report_request_c_congestion_info_e__values,
    4,
    "ecn_markingor_congestion_info_report_request_c::congestion_info_e_"};

static const char* enum_names_ecn_markingor_congestion_info_report_request_c_types_values[]   = {"eCNMarkingatNGRAN",
                                                                                                 "eCNMarkingatUPF",
                                                                                                 "congestionInformation",
                                                                                                 "choice-extension"};
const asn1::table::enum_desc ecn_markingor_congestion_info_report_request_c::types_opts::desc = {
    enum_names_ecn_markingor_congestion_info_report_request_c_types_values,
    4,
    "ecn_markingor_congestion_info_report_request_c::types"};

static const uint32_t objset_id_qos_flow_qos_param_item_ext_ies_o[]  = {99, 100, 203};
static const uint8_t objset_crit_qos_flow_qos_param_item_ext_ies_o[] = {crit_e::ignore, crit_e::ignore, crit_e::ignore};

// QoS-Flow-QoS-Parameter-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_qos_flow_qos_param_item_ext_ies_container_values[] = {
    {offsetof(qos_flow_qos_param_item_ext_ies_container, redundant_qos_flow_ind),
     (int32_t)offsetof(qos_flow_qos_param_item_ext_ies_container, redundant_qos_flow_ind_present),
     asn1::table::enum_op<decltype(qos_flow_qos_param_item_ext_ies_container::redundant_qos_flow_ind)>(),
     decltype(qos_flow_qos_param_item_ext_ies_container::redundant_qos_flow_ind)::has_ext,
     false,
     decltype(qos_flow_qos_param_item_ext_ies_container::redundant_qos_flow_ind)::nof_types,
     decltype(qos_flow_qos_param_item_ext_ies_container::redundant_qos_flow_ind)::nof_exts,
     nullptr,
     &redundant_qos_flow_ind_opts::desc},
    {offsetof(qos_flow_qos_param_item_ext_ies_container, tsc_traffic_characteristics),
     (int32_t)offsetof(qos_flow_qos_param_item_ext_ies_container, tsc_traffic_characteristics_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tsc_traffic_characteristics_s::desc},
    {offsetof(qos_flow_qos_param_item_ext_ies_container, ecn_markingor_congestion_info_report_request),
     (int32_t)offsetof(qos_flow_qos_param_item_ext_ies_container, ecn_markingor_congestion_info_report_request_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ecn_markingor_congestion_info_report_request_c::desc}};
static const int32_t ie_table_qos_flow_qos_param_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(qos_flow_qos_param_item_ext_ies_container, redundant_qos_flow_ind_present),
    (int32_t)offsetof(qos_flow_qos_param_item_ext_ies_container, tsc_traffic_characteristics_present),
    (int32_t)offsetof(qos_flow_qos_param_item_ext_ies_container, ecn_markingor_congestion_info_report_request_present)};
const asn1::table::ie_container_desc qos_flow_qos_param_item_ext_ies_container::desc = {
    objset_id_qos_flow_qos_param_item_ext_ies_o,
    objset_crit_qos_flow_qos_param_item_ext_ies_o,
    ie_table_qos_flow_qos_param_item_ext_ies_container_values,
    ie_table_qos_flow_qos_param_item_ext_ies_container_presences,
    3,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// QoS-Flow-QoS-Parameter-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_qos_flow_qos_param_item_s_fields[] = {
    {offsetof(qos_flow_qos_param_item_s, qos_flow_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(qos_flow_qos_param_item_s, qos_flow_level_qos_params),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &qos_flow_level_qos_params_s::desc},
    {offsetof(qos_flow_qos_param_item_s, qos_flow_map_ind),
     (int32_t)offsetof(qos_flow_qos_param_item_s, qos_flow_map_ind_present),
     asn1::table::enum_op<decltype(qos_flow_qos_param_item_s::qos_flow_map_ind)>(),
     decltype(qos_flow_qos_param_item_s::qos_flow_map_ind)::has_ext,
     false,
     decltype(qos_flow_qos_param_item_s::qos_flow_map_ind)::nof_types,
     decltype(qos_flow_qos_param_item_s::qos_flow_map_ind)::nof_exts,
     nullptr,
     &qos_flow_map_ind_opts::desc},
    {offsetof(qos_flow_qos_param_item_s, ie_exts),
     (int32_t)offsetof(qos_flow_qos_param_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &qos_flow_qos_param_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_qos_flow_qos_param_item_s_names[] = {
    {"qoS-Flow-Identifier", nullptr},
    {"qoSFlowLevelQoSParameters", nullptr},
    {"qoSFlowMappingIndication", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_qos_flow_qos_param_item_s_opts[] = {
    {2, (int32_t)offsetof(qos_flow_qos_param_item_s, qos_flow_map_ind_present)},
    {3, (int32_t)offsetof(qos_flow_qos_param_item_s, ie_exts_present)}};
const asn1::table::seq_desc qos_flow_qos_param_item_s::desc = {
    {seq_table_qos_flow_qos_param_item_s_fields, 4, UINT64_C(0x3), seq_table_qos_flow_qos_param_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_qos_flow_qos_param_item_s_names};

// F1U-TNL-InfoToAdd-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_f1_u_tnl_info_to_add_item_s_fields[] = {
    {offsetof(f1_u_tnl_info_to_add_item_s, broadcast_f1_u_context_ref_e1),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(f1_u_tnl_info_to_add_item_s::broadcast_f1_u_context_ref_e1),
                                asn1::table::string_json>::value},
    {offsetof(f1_u_tnl_info_to_add_item_s, ie_exts),
     (int32_t)offsetof(f1_u_tnl_info_to_add_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(f1_u_tnl_info_to_add_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_f1_u_tnl_info_to_add_item_s_names[] = {
    {"broadcastF1U-ContextReferenceE1", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_f1_u_tnl_info_to_add_item_s_opts[] = {
    {1, (int32_t)offsetof(f1_u_tnl_info_to_add_item_s, ie_exts_present)}};
const asn1::table::seq_desc f1_u_tnl_info_to_add_item_s::desc = {
    {seq_table_f1_u_tnl_info_to_add_item_s_fields, 2, UINT64_C(0x1), seq_table_f1_u_tnl_info_to_add_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_f1_u_tnl_info_to_add_item_s_names};

static const uint32_t objset_id_bcmrb_setup_cfg_item_ext_ies_o[]   = {208};
static const uint8_t  objset_crit_bcmrb_setup_cfg_item_ext_ies_o[] = {crit_e::ignore};

// BCMRBSetupConfiguration-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_bcmrb_setup_cfg_item_ext_ies_container_values[] = {
    {offsetof(bcmrb_setup_cfg_item_ext_ies_container, f1_u_tnl_info_to_add_list),
     (int32_t)offsetof(bcmrb_setup_cfg_item_ext_ies_container, f1_u_tnl_info_to_add_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     512,
     &asn1::table::item_list_codec<decltype(bcmrb_setup_cfg_item_ext_ies_container::f1_u_tnl_info_to_add_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &f1_u_tnl_info_to_add_item_s::desc}};
static const int32_t ie_table_bcmrb_setup_cfg_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(bcmrb_setup_cfg_item_ext_ies_container, f1_u_tnl_info_to_add_list_present)};
const asn1::table::ie_container_desc bcmrb_setup_cfg_item_ext_ies_container::desc = {
    objset_id_bcmrb_setup_cfg_item_ext_ies_o,
    objset_crit_bcmrb_setup_cfg_item_ext_ies_o,
    ie_table_bcmrb_setup_cfg_item_ext_ies_container_values,
    ie_table_bcmrb_setup_cfg_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// BCMRBSetupConfiguration-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_bcmrb_setup_cfg_item_s_fields[] = {
    {offsetof(bcmrb_setup_cfg_item_s, mrb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(bcmrb_setup_cfg_item_s, mbs_pdcp_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_cfg_s::desc},
    {offsetof(bcmrb_setup_cfg_item_s, qos_flow_qos_param_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(bcmrb_setup_cfg_item_s::qos_flow_qos_param_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_qos_param_item_s::desc},
    {offsetof(bcmrb_setup_cfg_item_s, qos_flow_level_qos_params),
     (int32_t)offsetof(bcmrb_setup_cfg_item_s, qos_flow_level_qos_params_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &qos_flow_level_qos_params_s::desc},
    {offsetof(bcmrb_setup_cfg_item_s, ie_exts),
     (int32_t)offsetof(bcmrb_setup_cfg_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &bcmrb_setup_cfg_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_bcmrb_setup_cfg_item_s_names[] = {
    {"mrb-ID", nullptr},
    {"mbs-pdcp-config", nullptr},
    {"qoS-Flow-QoS-Parameter-List", nullptr},
    {"qoSFlowLevelQoSParameters", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_bcmrb_setup_cfg_item_s_opts[] = {
    {3, (int32_t)offsetof(bcmrb_setup_cfg_item_s, qos_flow_level_qos_params_present)},
    {4, (int32_t)offsetof(bcmrb_setup_cfg_item_s, ie_exts_present)}};
const asn1::table::seq_desc bcmrb_setup_cfg_item_s::desc = {
    {seq_table_bcmrb_setup_cfg_item_s_fields, 5, UINT64_C(0x7), seq_table_bcmrb_setup_cfg_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_bcmrb_setup_cfg_item_s_names};

// F1U-TNL-InfoToAddOrModify-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_f1_u_tnl_info_to_add_or_modify_item_s_fields[] = {
    {offsetof(f1_u_tnl_info_to_add_or_modify_item_s, broadcast_f1_u_context_ref_e1),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(f1_u_tnl_info_to_add_or_modify_item_s::broadcast_f1_u_context_ref_e1),
                                asn1::table::string_json>::value},
    {offsetof(f1_u_tnl_info_to_add_or_modify_item_s, bc_bearer_context_f1_u_tnl_infoat_du),
     (int32_t)offsetof(f1_u_tnl_info_to_add_or_modify_item_s, bc_bearer_context_f1_u_tnl_infoat_du_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_f1_u_tnl_infoat_du_c::desc},
    {offsetof(f1_u_tnl_info_to_add_or_modify_item_s, ie_exts),
     (int32_t)offsetof(f1_u_tnl_info_to_add_or_modify_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(f1_u_tnl_info_to_add_or_modify_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_f1_u_tnl_info_to_add_or_modify_item_s_names[] = {
    {"broadcastF1U-ContextReferenceE1", nullptr},
    {"bcBearerContextF1U-TNLInfoatDU", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_f1_u_tnl_info_to_add_or_modify_item_s_opts[] = {
    {1, (int32_t)offsetof(f1_u_tnl_info_to_add_or_modify_item_s, bc_bearer_context_f1_u_tnl_infoat_du_present)},
    {2, (int32_t)offsetof(f1_u_tnl_info_to_add_or_modify_item_s, ie_exts_present)}};
const asn1::table::seq_desc f1_u_tnl_info_to_add_or_modify_item_s::desc = {
    {seq_table_f1_u_tnl_info_to_add_or_modify_item_s_fields,
     3,
     UINT64_C(0x1),
     seq_table_f1_u_tnl_info_to_add_or_modify_item_s_opts,
     2},
    true,
    true,
    nullptr,
    0,
    seq_table_f1_u_tnl_info_to_add_or_modify_item_s_names};

// F1U-TNL-InfoToRelease-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_f1_u_tnl_info_to_release_item_s_fields[] = {
    {offsetof(f1_u_tnl_info_to_release_item_s, broadcast_f1_u_context_ref_e1),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(f1_u_tnl_info_to_release_item_s::broadcast_f1_u_context_ref_e1),
                                asn1::table::string_json>::value},
    {offsetof(f1_u_tnl_info_to_release_item_s, ie_exts),
     (int32_t)offsetof(f1_u_tnl_info_to_release_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(f1_u_tnl_info_to_release_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_f1_u_tnl_info_to_release_item_s_names[] = {
    {"broadcastF1U-ContextReferenceE1", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_f1_u_tnl_info_to_release_item_s_opts[] = {
    {1, (int32_t)offsetof(f1_u_tnl_info_to_release_item_s, ie_exts_present)}};
const asn1::table::seq_desc f1_u_tnl_info_to_release_item_s::desc = {{seq_table_f1_u_tnl_info_to_release_item_s_fields,
                                                                      2,
                                                                      UINT64_C(0x1),
                                                                      seq_table_f1_u_tnl_info_to_release_item_s_opts,
                                                                      1},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_f1_u_tnl_info_to_release_item_s_names};

static const uint32_t objset_id_bcmrb_modify_cfg_item_ext_ies_o[]   = {210, 212};
static const uint8_t  objset_crit_bcmrb_modify_cfg_item_ext_ies_o[] = {crit_e::ignore, crit_e::ignore};

// BCMRBModifyConfiguration-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_bcmrb_modify_cfg_item_ext_ies_container_values[] = {
    {offsetof(bcmrb_modify_cfg_item_ext_ies_container, f1_u_tnl_info_to_add_or_modify_list),
     (int32_t)offsetof(bcmrb_modify_cfg_item_ext_ies_container, f1_u_tnl_info_to_add_or_modify_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     512,
     &asn1::table::item_list_codec<
         decltype(bcmrb_modify_cfg_item_ext_ies_container::f1_u_tnl_info_to_add_or_modify_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &f1_u_tnl_info_to_add_or_modify_item_s::desc},
    {offsetof(bcmrb_modify_cfg_item_ext_ies_container, f1_u_tnl_info_to_release_list),
     (int32_t)offsetof(bcmrb_modify_cfg_item_ext_ies_container, f1_u_tnl_info_to_release_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     512,
     &asn1::table::item_list_codec<decltype(bcmrb_modify_cfg_item_ext_ies_container::f1_u_tnl_info_to_release_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &f1_u_tnl_info_to_release_item_s::desc}};
static const int32_t ie_table_bcmrb_modify_cfg_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(bcmrb_modify_cfg_item_ext_ies_container, f1_u_tnl_info_to_add_or_modify_list_present),
    (int32_t)offsetof(bcmrb_modify_cfg_item_ext_ies_container, f1_u_tnl_info_to_release_list_present)};
const asn1::table::ie_container_desc bcmrb_modify_cfg_item_ext_ies_container::desc = {
    objset_id_bcmrb_modify_cfg_item_ext_ies_o,
    objset_crit_bcmrb_modify_cfg_item_ext_ies_o,
    ie_table_bcmrb_modify_cfg_item_ext_ies_container_values,
    ie_table_bcmrb_modify_cfg_item_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// BCMRBModifyConfiguration-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_bcmrb_modify_cfg_item_s_fields[] = {
    {offsetof(bcmrb_modify_cfg_item_s, mrb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(bcmrb_modify_cfg_item_s, bc_bearer_context_f1_u_tnl_infoat_du),
     (int32_t)offsetof(bcmrb_modify_cfg_item_s, bc_bearer_context_f1_u_tnl_infoat_du_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_f1_u_tnl_infoat_du_c::desc},
    {offsetof(bcmrb_modify_cfg_item_s, mbs_pdcp_cfg),
     (int32_t)offsetof(bcmrb_modify_cfg_item_s, mbs_pdcp_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_cfg_s::desc},
    {offsetof(bcmrb_modify_cfg_item_s, qos_flow_qos_param_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(bcmrb_modify_cfg_item_s::qos_flow_qos_param_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_qos_param_item_s::desc},
    {offsetof(bcmrb_modify_cfg_item_s, qos_flow_level_qos_params),
     (int32_t)offsetof(bcmrb_modify_cfg_item_s, qos_flow_level_qos_params_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &qos_flow_level_qos_params_s::desc},
    {offsetof(bcmrb_modify_cfg_item_s, ie_exts),
     (int32_t)offsetof(bcmrb_modify_cfg_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &bcmrb_modify_cfg_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_bcmrb_modify_cfg_item_s_names[] = {
    {"mrb-ID", nullptr},
    {"bcBearerContextF1U-TNLInfoatDU", nullptr},
    {"mbs-pdcp-config", nullptr},
    {"qoS-Flow-QoS-Parameter-List", nullptr},
    {"qoSFlowLevelQoSParameters", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_bcmrb_modify_cfg_item_s_opts[] = {
    {1, (int32_t)offsetof(bcmrb_modify_cfg_item_s, bc_bearer_context_f1_u_tnl_infoat_du_present)},
    {2, (int32_t)offsetof(bcmrb_modify_cfg_item_s, mbs_pdcp_cfg_present)},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(bcmrb_modify_cfg_item_s, qos_flow_level_qos_params_present)},
    {5, (int32_t)offsetof(bcmrb_modify_cfg_item_s, ie_exts_present)}};
const asn1::table::seq_desc bcmrb_modify_cfg_item_s::desc = {
    {seq_table_bcmrb_modify_cfg_item_s_fields, 6, UINT64_C(0x1), seq_table_bcmrb_modify_cfg_item_s_opts, 5},
    true,
    true,
    nullptr,
    0,
    seq_table_bcmrb_modify_cfg_item_s_names};

static const uint32_t objset_id_bc_bearer_context_to_modify_ext_ies_o[]   = {217};
static const uint8_t  objset_crit_bc_bearer_context_to_modify_ext_ies_o[] = {crit_e::ignore};

// BCBearerContextToModify-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_bc_bearer_context_to_modify_ext_ies_container_values[] = {
    {offsetof(bc_bearer_context_to_modify_ext_ies_container, bc_bearer_context_ngu_tnl_infoat_ngran_request),
     (int32_t)offsetof(bc_bearer_context_to_modify_ext_ies_container,
                       bc_bearer_context_ngu_tnl_infoat_ngran_request_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_ngu_tnl_infoat_ngran_request_c::desc}};
static const int32_t ie_table_bc_bearer_context_to_modify_ext_ies_container_presences[] = {
    (int32_t)offsetof(bc_bearer_context_to_modify_ext_ies_container,
                      bc_bearer_context_ngu_tnl_infoat_ngran_request_present)};
const asn1::table::ie_container_desc bc_bearer_context_to_modify_ext_ies_container::desc = {
    objset_id_bc_bearer_context_to_modify_ext_ies_o,
    objset_crit_bc_bearer_context_to_modify_ext_ies_o,
    ie_table_bc_bearer_context_to_modify_ext_ies_container_values,
    ie_table_bc_bearer_context_to_modify_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// BCBearerContextToModify ::= SEQUENCE
static const asn1::table::seq_field seq_table_bc_bearer_context_to_modify_s_fields[] = {
    {offsetof(bc_bearer_context_to_modify_s, bc_bearer_context_ngu_tnl_infoat5_gc),
     (int32_t)offsetof(bc_bearer_context_to_modify_s, bc_bearer_context_ngu_tnl_infoat5_gc_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_ngu_tnl_infoat5_gc_c::desc},
    {offsetof(bc_bearer_context_to_modify_s, bc_mrb_to_setup_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(bc_bearer_context_to_modify_s::bc_mrb_to_setup_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &bcmrb_setup_cfg_item_s::desc},
    {offsetof(bc_bearer_context_to_modify_s, bc_mrb_to_modify_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(bc_bearer_context_to_modify_s::bc_mrb_to_modify_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &bcmrb_modify_cfg_item_s::desc},
    {offsetof(bc_bearer_context_to_modify_s, bc_mrb_to_rem_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::list_codec<decltype(bc_bearer_context_to_modify_s::bc_mrb_to_rem_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(512), true, true>>::value},
    {offsetof(bc_bearer_context_to_modify_s, ie_exts),
     (int32_t)offsetof(bc_bearer_context_to_modify_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_to_modify_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_bc_bearer_context_to_modify_s_names[] = {
    {"bcBearerContextNGU-TNLInfoat5GC", nullptr},
    {"bcMRBToSetupList", nullptr},
    {"bcMRBToModifyList", nullptr},
    {"bcMRBToRemoveList", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_bc_bearer_context_to_modify_s_opts[] = {
    {0, (int32_t)offsetof(bc_bearer_context_to_modify_s, bc_bearer_context_ngu_tnl_infoat5_gc_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(bc_bearer_context_to_modify_s, ie_exts_present)}};
const asn1::table::seq_desc bc_bearer_context_to_modify_s::desc = {
    {seq_table_bc_bearer_context_to_modify_s_fields, 5, UINT64_C(0x0), seq_table_bc_bearer_context_to_modify_s_opts, 5},
    true,
    true,
    nullptr,
    0,
    seq_table_bc_bearer_context_to_modify_s_names};

// BCBearerContextToModifyConfirm ::= SEQUENCE
static const asn1::table::seq_field seq_table_bc_bearer_context_to_modify_confirm_s_fields[] = {
    {offsetof(bc_bearer_context_to_modify_confirm_s, ie_exts),
     (int32_t)offsetof(bc_bearer_context_to_modify_confirm_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(bc_bearer_context_to_modify_confirm_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_bc_bearer_context_to_modify_confirm_s_names[] = {
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_bc_bearer_context_to_modify_confirm_s_opts[] = {
    {0, (int32_t)offsetof(bc_bearer_context_to_modify_confirm_s, ie_exts_present)}};
const asn1::table::seq_desc bc_bearer_context_to_modify_confirm_s::desc = {
    {seq_table_bc_bearer_context_to_modify_confirm_s_fields,
     1,
     UINT64_C(0x0),
     seq_table_bc_bearer_context_to_modify_confirm_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_bc_bearer_context_to_modify_confirm_s_names};

// BCBearerContextToModifyRequired ::= SEQUENCE
static const asn1::table::seq_field seq_table_bc_bearer_context_to_modify_required_s_fields[] = {
    {offsetof(bc_bearer_context_to_modify_required_s, bc_mrb_to_rem_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::list_codec<decltype(bc_bearer_context_to_modify_required_s::bc_mrb_to_rem_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(512), true, true>>::value},
    {offsetof(bc_bearer_context_to_modify_required_s, ie_exts),
     (int32_t)offsetof(bc_bearer_context_to_modify_required_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(bc_bearer_context_to_modify_required_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_bc_bearer_context_to_modify_required_s_names[] = {
    {"bcMRBToRemoveList", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_bc_bearer_context_to_modify_required_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(bc_bearer_context_to_modify_required_s, ie_exts_present)}};
const asn1::table::seq_desc bc_bearer_context_to_modify_required_s::desc = {
    {seq_table_bc_bearer_context_to_modify_required_s_fields,
     2,
     UINT64_C(0x0),
     seq_table_bc_bearer_context_to_modify_required_s_opts,
     2},
    true,
    true,
    nullptr,
    0,
    seq_table_bc_bearer_context_to_modify_required_s_names};

// ECNMarkingorCongestionInformationReportingStatus ::= ENUMERATED
static const char* enum_names_ecn_markingor_congestion_info_report_status_e_values[] = {"active", "not-active"};
const asn1::table::enum_desc ecn_markingor_congestion_info_report_status_opts::desc  = {
    enum_names_ecn_markingor_congestion_info_report_status_e_values,
    2,
    "ecn_markingor_congestion_info_report_status_e"};

static const uint32_t objset_id_qos_flow_item_ext_ies_o[]   = {80, 142, 204};
static const uint8_t  objset_crit_qos_flow_item_ext_ies_o[] = {crit_e::ignore, crit_e::ignore, crit_e::ignore};

// QoS-Flow-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_qos_flow_item_ext_ies_container_values[] = {
    {offsetof(qos_flow_item_ext_ies_container, qos_flow_map_ind),
     (int32_t)offsetof(qos_flow_item_ext_ies_container, qos_flow_map_ind_present),
     asn1::table::enum_op<decltype(qos_flow_item_ext_ies_container::qos_flow_map_ind)>(),
     decltype(qos_flow_item_ext_ies_container::qos_flow_map_ind)::has_ext,
     false,
     decltype(qos_flow_item_ext_ies_container::qos_flow_map_ind)::nof_types,
     decltype(qos_flow_item_ext_ies_container::qos_flow_map_ind)::nof_exts,
     nullptr,
     &qos_flow_map_ind_opts::desc},
    {offsetof(qos_flow_item_ext_ies_container, data_forwarding_source_ip_address),
     (int32_t)offsetof(qos_flow_item_ext_ies_container, data_forwarding_source_ip_address_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(qos_flow_item_ext_ies_container::data_forwarding_source_ip_address),
                                asn1::table::struct_json>::value},
    {offsetof(qos_flow_item_ext_ies_container, ecn_markingor_congestion_info_report_status),
     (int32_t)offsetof(qos_flow_item_ext_ies_container, ecn_markingor_congestion_info_report_status_present),
     asn1::table::enum_op<decltype(qos_flow_item_ext_ies_container::ecn_markingor_congestion_info_report_status)>(),
     decltype(qos_flow_item_ext_ies_container::ecn_markingor_congestion_info_report_status)::has_ext,
     false,
     decltype(qos_flow_item_ext_ies_container::ecn_markingor_congestion_info_report_status)::nof_types,
     decltype(qos_flow_item_ext_ies_container::ecn_markingor_congestion_info_report_status)::nof_exts,
     nullptr,
     &ecn_markingor_congestion_info_report_status_opts::desc}};
static const int32_t ie_table_qos_flow_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(qos_flow_item_ext_ies_container, qos_flow_map_ind_present),
    (int32_t)offsetof(qos_flow_item_ext_ies_container, data_forwarding_source_ip_address_present),
    (int32_t)offsetof(qos_flow_item_ext_ies_container, ecn_markingor_congestion_info_report_status_present)};
const asn1::table::ie_container_desc qos_flow_item_ext_ies_container::desc = {
    objset_id_qos_flow_item_ext_ies_o,
    objset_crit_qos_flow_item_ext_ies_o,
    ie_table_qos_flow_item_ext_ies_container_values,
    ie_table_qos_flow_item_ext_ies_container_presences,
    3,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// QoS-Flow-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_qos_flow_item_s_fields[] = {
    {offsetof(qos_flow_item_s, qos_flow_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(qos_flow_item_s, ie_exts),
     (int32_t)offsetof(qos_flow_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &qos_flow_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_qos_flow_item_s_names[] = {{"qoS-Flow-Identifier", nullptr},
                                                                          {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_qos_flow_item_s_opts[]  = {
    {1, (int32_t)offsetof(qos_flow_item_s, ie_exts_present)}};
const asn1::table::seq_desc qos_flow_item_s::desc = {
    {seq_table_qos_flow_item_s_fields, 2, UINT64_C(0x1), seq_table_qos_flow_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_qos_flow_item_s_names};

// CauseRadioNetwork ::= ENUMERATED
static const char*           enum_names_cause_radio_network_e_values[] = {"unspecified",
                                                                          "unknown-or-already-allocated-gnb-cu-cp-ue-e1ap-id",
                                                                          "unknown-or-already-allocated-gnb-cu-up-ue-e1ap-id",
                                                                          "unknown-or-inconsistent-pair-of-ue-e1ap-id",
                                                                          "interaction-with-other-procedure",
                                                                          "pPDCP-Count-wrap-around",
                                                                          "not-supported-QCI-value",
                                                                          "not-supported-5QI-value",
                                                                          "encryption-algorithms-not-supported",
                                                                          "integrity-protection-algorithms-not-supported",
                                                                          "uP-integrity-protection-not-possible",
                                                                          "uP-confidentiality-protection-not-possible",
                                                                          "multiple-PDU-Session-ID-Instances",
                                                                          "unknown-PDU-Session-ID",
                                                                          "multiple-QoS-Flow-ID-Instances",
                                                                          "unknown-QoS-Flow-ID",
                                                                          "multiple-DRB-ID-Instances",
                                                                          "unknown-DRB-ID",
                                                                          "invalid-QoS-combination",
                                                                          "procedure-cancelled",
                                                                          "normal-release",
                                                                          "no-radio-resources-available",
                                                                          "action-desirable-for-radio-reasons",
                                                                          "resources-not-available-for-the-slice",
                                                                          "pDCP-configuration-not-supported",
                                                                          "ue-dl-max-IP-data-rate-reason",
                                                                          "uP-integrity-protection-failure",
                                                                          "release-due-to-pre-emption",
                                                                          "rsn-not-available-for-the-up",
                                                                          "nPN-not-supported",
                                                                          "report-characteristic-empty",
                                                                          "existing-measurement-ID",
                                                                          "measurement-temporarily-not-available",
                                                                          "measurement-not-supported-for-the-object",
                                                                          "scg-activation-deactivation-failure",
                                                                          "scg-deactivation-failure-due-to-data-transmission",
                                                                          "unknown-or-already-allocated-gNB-CU-CP-MBS-E1AP-ID",
                                                                          "unknown-or-already-allocated-gNB-CU-UP-MBS-E1AP-ID",
                                                                          "unknown-or-inconsistent-pair-of-MBS-E1AP-ID",
                                                                          "unknown-or-inconsistent-MRB-ID"};
const asn1::table::enum_desc cause_radio_network_opts::desc            = {enum_names_cause_radio_network_e_values,
                                                                          40,
                                                                          "cause_radio_network_e"};

// CauseTransport ::= ENUMERATED
static const char*           enum_names_cause_transport_e_values[] = {"unspecified",
                                                                      "transport-resource-unavailable",
                                                                      "unknown-TNL-address-for-IAB"};
const asn1::table::enum_desc cause_transport_opts::desc = {enum_names_cause_transport_e_values, 3, "cause_transport_e"};

// CauseProtocol ::= ENUMERATED
static const char*           enum_names_cause_protocol_e_values[] = {"transfer-syntax-error",
                                                                     "abstract-syntax-error-reject",
                                                                     "abstract-syntax-error-ignore-and-notify",
                                                                     "message-not-compatible-with-receiver-state",
                                                                     "semantic-error",
                                                                     "abstract-syntax-error-falsely-constructed-message",
                                                                     "unspecified"};
const asn1::table::enum_desc cause_protocol_opts::desc = {enum_names_cause_protocol_e_values, 7, "cause_protocol_e"};

// CauseMisc ::= ENUMERATED
static const char*           enum_names_cause_misc_e_values[] = {"control-processing-overload",
                                                                 "not-enough-user-plane-processing-resources",
                                                                 "hardware-failure",
                                                                 "om-intervention",
                                                                 "unspecified"};
const asn1::table::enum_desc cause_misc_opts::desc            = {enum_names_cause_misc_e_values, 5, "cause_misc_e"};

// Cause ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_cause_c_alternatives[] = {
    &asn1::table::alternative_ops<cause_radio_network_e, true>::value,
    &asn1::table::alternative_ops<cause_transport_e, true>::value,
    &asn1::table::alternative_ops<cause_protocol_e, true>::value,
    &asn1::table::alternative_ops<cause_misc_e, true>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<cause_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc cause_c::storage_desc = {choice_storage_cause_c_alternatives, 5, "cause_c"};

cause_radio_network_e& cause_c::set_radio_network()
{
  set(types::radio_network);
  return c.get<cause_radio_network_e>();
}
cause_transport_e& cause_c::set_transport()
{
  set(types::transport);
  return c.get<cause_transport_e>();
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
protocol_ie_single_container_s<cause_ext_ies_o>& cause_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<cause_ext_ies_o>>();
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
     asn1::table::enum_op<cause_transport_e>(),
     cause_transport_e::has_ext,
     false,
     cause_transport_e::nof_types,
     cause_transport_e::nof_exts,
     nullptr,
     &cause_transport_opts::desc},
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
     &asn1::table::object_codec<protocol_ie_single_container_s<cause_ext_ies_o>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_cause_c_names[] = {{"radioNetwork", nullptr},
                                                                     {"transport", nullptr},
                                                                     {"protocol", nullptr},
                                                                     {"misc", nullptr},
                                                                     {"choice-extension", nullptr}};
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

static const char*           enum_names_cause_c_types_values[] = {"radioNetwork",
                                                                  "transport",
                                                                  "protocol",
                                                                  "misc",
                                                                  "choice-extension"};
const asn1::table::enum_desc cause_c::types_opts::desc         = {enum_names_cause_c_types_values, 5, "cause_c::types"};
uint8_t                      cause_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "cause_c::types");
}

// QoS-Flow-Failed-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_qos_flow_failed_item_s_fields[] = {
    {offsetof(qos_flow_failed_item_s, qos_flow_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(qos_flow_failed_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(qos_flow_failed_item_s, ie_exts),
     (int32_t)offsetof(qos_flow_failed_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(qos_flow_failed_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_qos_flow_failed_item_s_names[] = {{"qoS-Flow-Identifier", nullptr},
                                                                                 {"cause", nullptr},
                                                                                 {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_qos_flow_failed_item_s_opts[]  = {
    {2, (int32_t)offsetof(qos_flow_failed_item_s, ie_exts_present)}};
const asn1::table::seq_desc qos_flow_failed_item_s::desc = {
    {seq_table_qos_flow_failed_item_s_fields, 3, UINT64_C(0x3), seq_table_qos_flow_failed_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_qos_flow_failed_item_s_names};

// F1U-TNL-InfoAddedOrModified-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_f1_u_tnl_info_added_or_modified_item_s_fields[] = {
    {offsetof(f1_u_tnl_info_added_or_modified_item_s, broadcast_f1_u_context_ref_e1),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(f1_u_tnl_info_added_or_modified_item_s::broadcast_f1_u_context_ref_e1),
                                asn1::table::string_json>::value},
    {offsetof(f1_u_tnl_info_added_or_modified_item_s, bc_bearer_context_f1_u_tnl_infoat_cu),
     (int32_t)offsetof(f1_u_tnl_info_added_or_modified_item_s, bc_bearer_context_f1_u_tnl_infoat_cu_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_f1_u_tnl_infoat_cu_c::desc},
    {offsetof(f1_u_tnl_info_added_or_modified_item_s, ie_exts),
     (int32_t)offsetof(f1_u_tnl_info_added_or_modified_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(f1_u_tnl_info_added_or_modified_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_f1_u_tnl_info_added_or_modified_item_s_names[] = {
    {"broadcastF1U-ContextReferenceE1", nullptr},
    {"bcBearerContextF1U-TNLInfoatCU", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_f1_u_tnl_info_added_or_modified_item_s_opts[] = {
    {1, (int32_t)offsetof(f1_u_tnl_info_added_or_modified_item_s, bc_bearer_context_f1_u_tnl_infoat_cu_present)},
    {2, (int32_t)offsetof(f1_u_tnl_info_added_or_modified_item_s, ie_exts_present)}};
const asn1::table::seq_desc f1_u_tnl_info_added_or_modified_item_s::desc = {
    {seq_table_f1_u_tnl_info_added_or_modified_item_s_fields,
     3,
     UINT64_C(0x1),
     seq_table_f1_u_tnl_info_added_or_modified_item_s_opts,
     2},
    true,
    true,
    nullptr,
    0,
    seq_table_f1_u_tnl_info_added_or_modified_item_s_names};

static const uint32_t objset_id_bcmrb_setup_modify_resp_list_item_ext_ies_o[]   = {211};
static const uint8_t  objset_crit_bcmrb_setup_modify_resp_list_item_ext_ies_o[] = {crit_e::ignore};

// BCMRBSetupModifyResponseList-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_bcmrb_setup_modify_resp_list_item_ext_ies_container_values[] = {
    {offsetof(bcmrb_setup_modify_resp_list_item_ext_ies_container, f1_u_tnl_info_added_or_modified_list),
     (int32_t)offsetof(bcmrb_setup_modify_resp_list_item_ext_ies_container,
                       f1_u_tnl_info_added_or_modified_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     512,
     &asn1::table::item_list_codec<
         decltype(bcmrb_setup_modify_resp_list_item_ext_ies_container::f1_u_tnl_info_added_or_modified_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &f1_u_tnl_info_added_or_modified_item_s::desc}};
static const int32_t ie_table_bcmrb_setup_modify_resp_list_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(bcmrb_setup_modify_resp_list_item_ext_ies_container,
                      f1_u_tnl_info_added_or_modified_list_present)};
const asn1::table::ie_container_desc bcmrb_setup_modify_resp_list_item_ext_ies_container::desc = {
    objset_id_bcmrb_setup_modify_resp_list_item_ext_ies_o,
    objset_crit_bcmrb_setup_modify_resp_list_item_ext_ies_o,
    ie_table_bcmrb_setup_modify_resp_list_item_ext_ies_container_values,
    ie_table_bcmrb_setup_modify_resp_list_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// BCMRBSetupModifyResponseList-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_bcmrb_setup_modify_resp_list_item_s_fields[] = {
    {offsetof(bcmrb_setup_modify_resp_list_item_s, mrb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(bcmrb_setup_modify_resp_list_item_s, qosflow_setup),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(bcmrb_setup_modify_resp_list_item_s::qosflow_setup),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_item_s::desc},
    {offsetof(bcmrb_setup_modify_resp_list_item_s, qosflow_failed),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(bcmrb_setup_modify_resp_list_item_s::qosflow_failed),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_failed_item_s::desc},
    {offsetof(bcmrb_setup_modify_resp_list_item_s, bc_bearer_context_f1_u_tnl_infoat_cu),
     (int32_t)offsetof(bcmrb_setup_modify_resp_list_item_s, bc_bearer_context_f1_u_tnl_infoat_cu_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_f1_u_tnl_infoat_cu_c::desc},
    {offsetof(bcmrb_setup_modify_resp_list_item_s, ie_exts),
     (int32_t)offsetof(bcmrb_setup_modify_resp_list_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &bcmrb_setup_modify_resp_list_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_bcmrb_setup_modify_resp_list_item_s_names[] = {
    {"mrb-ID", nullptr},
    {"qosflow-setup", nullptr},
    {"qosflow-failed", nullptr},
    {"bcBearerContextF1U-TNLInfoatCU", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_bcmrb_setup_modify_resp_list_item_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(bcmrb_setup_modify_resp_list_item_s, bc_bearer_context_f1_u_tnl_infoat_cu_present)},
    {4, (int32_t)offsetof(bcmrb_setup_modify_resp_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc bcmrb_setup_modify_resp_list_item_s::desc = {
    {seq_table_bcmrb_setup_modify_resp_list_item_s_fields,
     5,
     UINT64_C(0x1),
     seq_table_bcmrb_setup_modify_resp_list_item_s_opts,
     4},
    true,
    true,
    nullptr,
    0,
    seq_table_bcmrb_setup_modify_resp_list_item_s_names};

// BCMRBFailedList-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_bcmrb_failed_list_item_s_fields[] = {
    {offsetof(bcmrb_failed_list_item_s, mrb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(bcmrb_failed_list_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(bcmrb_failed_list_item_s, ie_exts),
     (int32_t)offsetof(bcmrb_failed_list_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(bcmrb_failed_list_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_bcmrb_failed_list_item_s_names[] = {{"mrb-ID", nullptr},
                                                                                   {"cause", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_bcmrb_failed_list_item_s_opts[]  = {
    {2, (int32_t)offsetof(bcmrb_failed_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc bcmrb_failed_list_item_s::desc = {
    {seq_table_bcmrb_failed_list_item_s_fields, 3, UINT64_C(0x3), seq_table_bcmrb_failed_list_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_bcmrb_failed_list_item_s_names};

// BCBearerContextToModifyResponse ::= SEQUENCE
static const asn1::table::seq_field seq_table_bc_bearer_context_to_modify_resp_s_fields[] = {
    {offsetof(bc_bearer_context_to_modify_resp_s, bc_bearer_context_ngu_tnl_infoat_ngran),
     (int32_t)offsetof(bc_bearer_context_to_modify_resp_s, bc_bearer_context_ngu_tnl_infoat_ngran_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_ngu_tnl_infoat_ngran_c::desc},
    {offsetof(bc_bearer_context_to_modify_resp_s, bc_mrb_setup_modify_resp_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(bc_bearer_context_to_modify_resp_s::bc_mrb_setup_modify_resp_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &bcmrb_setup_modify_resp_list_item_s::desc},
    {offsetof(bc_bearer_context_to_modify_resp_s, bc_mrb_failed_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(bc_bearer_context_to_modify_resp_s::bc_mrb_failed_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &bcmrb_failed_list_item_s::desc},
    {offsetof(bc_bearer_context_to_modify_resp_s, available_bcmrb_cfg),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(bc_bearer_context_to_modify_resp_s::available_bcmrb_cfg),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &bcmrb_setup_cfg_item_s::desc},
    {offsetof(bc_bearer_context_to_modify_resp_s, ie_exts),
     (int32_t)offsetof(bc_bearer_context_to_modify_resp_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(bc_bearer_context_to_modify_resp_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_bc_bearer_context_to_modify_resp_s_names[] = {
    {"bcBearerContextNGU-TNLInfoatNGRAN", nullptr},
    {"bcMRBSetupModifyResponseList", nullptr},
    {"bcMRBFailedList", nullptr},
    {"availableBCMRBConfig", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_bc_bearer_context_to_modify_resp_s_opts[] = {
    {0, (int32_t)offsetof(bc_bearer_context_to_modify_resp_s, bc_bearer_context_ngu_tnl_infoat_ngran_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(bc_bearer_context_to_modify_resp_s, ie_exts_present)}};
const asn1::table::seq_desc bc_bearer_context_to_modify_resp_s::desc = {
    {seq_table_bc_bearer_context_to_modify_resp_s_fields,
     5,
     UINT64_C(0x2),
     seq_table_bc_bearer_context_to_modify_resp_s_opts,
     4},
    true,
    true,
    nullptr,
    0,
    seq_table_bc_bearer_context_to_modify_resp_s_names};

// SNSSAI ::= SEQUENCE
static const asn1::table::seq_field seq_table_snssai_s_fields[] = {
    {offsetof(snssai_s, sst),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(snssai_s::sst), asn1::table::string_json>::value},
    {offsetof(snssai_s, sd),
     (int32_t)offsetof(snssai_s, sd_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(snssai_s::sd), asn1::table::string_json>::value},
    {offsetof(snssai_s, ie_exts),
     (int32_t)offsetof(snssai_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(snssai_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_snssai_s_names[] = {{"sST", nullptr},
                                                                   {"sD", nullptr},
                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_snssai_s_opts[]  = {{1, (int32_t)offsetof(snssai_s, sd_present)},
                                                                   {2, (int32_t)offsetof(snssai_s, ie_exts_present)}};
const asn1::table::seq_desc snssai_s::desc = {{seq_table_snssai_s_fields, 3, UINT64_C(0x1), seq_table_snssai_s_opts, 2},
                                              true,
                                              true,
                                              nullptr,
                                              0,
                                              seq_table_snssai_s_names};

// RequestedAction4AvailNGUTermination ::= ENUMERATED
static const char* enum_names_requested_action4_avail_ngu_termination_e_values[] = {
    "apply-available-configuration",
    "apply-requested-configuration",
    "apply-available-configuration-if-same-as-requested"};
const asn1::table::enum_desc requested_action4_avail_ngu_termination_opts::desc = {
    enum_names_requested_action4_avail_ngu_termination_e_values,
    3,
    "requested_action4_avail_ngu_termination_e"};

// BCBearerContextToSetup ::= SEQUENCE
static const asn1::table::seq_field seq_table_bc_bearer_context_to_setup_s_fields[] = {
    {offsetof(bc_bearer_context_to_setup_s, snssai),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &snssai_s::desc},
    {offsetof(bc_bearer_context_to_setup_s, bc_bearer_context_ngu_tnl_infoat5_gc),
     (int32_t)offsetof(bc_bearer_context_to_setup_s, bc_bearer_context_ngu_tnl_infoat5_gc_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_ngu_tnl_infoat5_gc_c::desc},
    {offsetof(bc_bearer_context_to_setup_s, bc_mrb_to_setup_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(bc_bearer_context_to_setup_s::bc_mrb_to_setup_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &bcmrb_setup_cfg_item_s::desc},
    {offsetof(bc_bearer_context_to_setup_s, requested_action),
     (int32_t)offsetof(bc_bearer_context_to_setup_s, requested_action_present),
     asn1::table::enum_op<decltype(bc_bearer_context_to_setup_s::requested_action)>(),
     decltype(bc_bearer_context_to_setup_s::requested_action)::has_ext,
     false,
     decltype(bc_bearer_context_to_setup_s::requested_action)::nof_types,
     decltype(bc_bearer_context_to_setup_s::requested_action)::nof_exts,
     nullptr,
     &requested_action4_avail_ngu_termination_opts::desc},
    {offsetof(bc_bearer_context_to_setup_s, ie_exts),
     (int32_t)offsetof(bc_bearer_context_to_setup_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(bc_bearer_context_to_setup_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_bc_bearer_context_to_setup_s_names[] = {
    {"snssai", nullptr},
    {"bcBearerContextNGU-TNLInfoat5GC", nullptr},
    {"bcMRBToSetupList", nullptr},
    {"requestedAction", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_bc_bearer_context_to_setup_s_opts[] = {
    {1, (int32_t)offsetof(bc_bearer_context_to_setup_s, bc_bearer_context_ngu_tnl_infoat5_gc_present)},
    {3, (int32_t)offsetof(bc_bearer_context_to_setup_s, requested_action_present)},
    {4, (int32_t)offsetof(bc_bearer_context_to_setup_s, ie_exts_present)}};
const asn1::table::seq_desc bc_bearer_context_to_setup_s::desc = {
    {seq_table_bc_bearer_context_to_setup_s_fields, 5, UINT64_C(0x5), seq_table_bc_bearer_context_to_setup_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_bc_bearer_context_to_setup_s_names};

// F1U-TNL-InfoAdded-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_f1_u_tnl_info_added_item_s_fields[] = {
    {offsetof(f1_u_tnl_info_added_item_s, broadcast_f1_u_context_ref_e1),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(f1_u_tnl_info_added_item_s::broadcast_f1_u_context_ref_e1),
                                asn1::table::string_json>::value},
    {offsetof(f1_u_tnl_info_added_item_s, bc_bearer_context_f1_u_tnl_infoat_cu),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_f1_u_tnl_infoat_cu_c::desc},
    {offsetof(f1_u_tnl_info_added_item_s, ie_exts),
     (int32_t)offsetof(f1_u_tnl_info_added_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(f1_u_tnl_info_added_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_f1_u_tnl_info_added_item_s_names[] = {
    {"broadcastF1U-ContextReferenceE1", nullptr},
    {"bcBearerContextF1U-TNLInfoatCU", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_f1_u_tnl_info_added_item_s_opts[] = {
    {2, (int32_t)offsetof(f1_u_tnl_info_added_item_s, ie_exts_present)}};
const asn1::table::seq_desc f1_u_tnl_info_added_item_s::desc = {
    {seq_table_f1_u_tnl_info_added_item_s_fields, 3, UINT64_C(0x3), seq_table_f1_u_tnl_info_added_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_f1_u_tnl_info_added_item_s_names};

static const uint32_t objset_id_bcmrb_setup_resp_list_item_ext_ies_o[]   = {209};
static const uint8_t  objset_crit_bcmrb_setup_resp_list_item_ext_ies_o[] = {crit_e::ignore};

// BCMRBSetupResponseList-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_bcmrb_setup_resp_list_item_ext_ies_container_values[] = {
    {offsetof(bcmrb_setup_resp_list_item_ext_ies_container, f1_u_tnl_info_added_list),
     (int32_t)offsetof(bcmrb_setup_resp_list_item_ext_ies_container, f1_u_tnl_info_added_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     512,
     &asn1::table::item_list_codec<decltype(bcmrb_setup_resp_list_item_ext_ies_container::f1_u_tnl_info_added_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &f1_u_tnl_info_added_item_s::desc}};
static const int32_t ie_table_bcmrb_setup_resp_list_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(bcmrb_setup_resp_list_item_ext_ies_container, f1_u_tnl_info_added_list_present)};
const asn1::table::ie_container_desc bcmrb_setup_resp_list_item_ext_ies_container::desc = {
    objset_id_bcmrb_setup_resp_list_item_ext_ies_o,
    objset_crit_bcmrb_setup_resp_list_item_ext_ies_o,
    ie_table_bcmrb_setup_resp_list_item_ext_ies_container_values,
    ie_table_bcmrb_setup_resp_list_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// BCMRBSetupResponseList-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_bcmrb_setup_resp_list_item_s_fields[] = {
    {offsetof(bcmrb_setup_resp_list_item_s, mrb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(bcmrb_setup_resp_list_item_s, qosflow_setup),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(bcmrb_setup_resp_list_item_s::qosflow_setup),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_item_s::desc},
    {offsetof(bcmrb_setup_resp_list_item_s, qosflow_failed),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(bcmrb_setup_resp_list_item_s::qosflow_failed),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_failed_item_s::desc},
    {offsetof(bcmrb_setup_resp_list_item_s, bc_bearer_context_f1_u_tnl_infoat_cu),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_f1_u_tnl_infoat_cu_c::desc},
    {offsetof(bcmrb_setup_resp_list_item_s, ie_exts),
     (int32_t)offsetof(bcmrb_setup_resp_list_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &bcmrb_setup_resp_list_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_bcmrb_setup_resp_list_item_s_names[] = {
    {"mrb-ID", nullptr},
    {"qosflow-setup", nullptr},
    {"qosflow-failed", nullptr},
    {"bcBearerContextF1U-TNLInfoatCU", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_bcmrb_setup_resp_list_item_s_opts[] = {
    {2, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(bcmrb_setup_resp_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc bcmrb_setup_resp_list_item_s::desc = {
    {seq_table_bcmrb_setup_resp_list_item_s_fields, 5, UINT64_C(0xb), seq_table_bcmrb_setup_resp_list_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_bcmrb_setup_resp_list_item_s_names};

// BCBearerContextToSetupResponse ::= SEQUENCE
static const asn1::table::seq_field seq_table_bc_bearer_context_to_setup_resp_s_fields[] = {
    {offsetof(bc_bearer_context_to_setup_resp_s, bc_bearer_context_ngu_tnl_infoat_ngran),
     (int32_t)offsetof(bc_bearer_context_to_setup_resp_s, bc_bearer_context_ngu_tnl_infoat_ngran_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &bc_bearer_context_ngu_tnl_infoat_ngran_c::desc},
    {offsetof(bc_bearer_context_to_setup_resp_s, bc_mrb_setup_resp_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(bc_bearer_context_to_setup_resp_s::bc_mrb_setup_resp_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &bcmrb_setup_resp_list_item_s::desc},
    {offsetof(bc_bearer_context_to_setup_resp_s, bc_mrb_failed_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(bc_bearer_context_to_setup_resp_s::bc_mrb_failed_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &bcmrb_failed_list_item_s::desc},
    {offsetof(bc_bearer_context_to_setup_resp_s, available_bcmrb_cfg),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(bc_bearer_context_to_setup_resp_s::available_bcmrb_cfg),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &bcmrb_setup_cfg_item_s::desc},
    {offsetof(bc_bearer_context_to_setup_resp_s, ie_exts),
     (int32_t)offsetof(bc_bearer_context_to_setup_resp_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(bc_bearer_context_to_setup_resp_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_bc_bearer_context_to_setup_resp_s_names[] = {
    {"bcBearerContextNGU-TNLInfoatNGRAN", nullptr},
    {"bcMRBSetupResponseList", nullptr},
    {"bcMRBFailedList", nullptr},
    {"availableBCMRBConfig", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_bc_bearer_context_to_setup_resp_s_opts[] = {
    {0, (int32_t)offsetof(bc_bearer_context_to_setup_resp_s, bc_bearer_context_ngu_tnl_infoat_ngran_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(bc_bearer_context_to_setup_resp_s, ie_exts_present)}};
const asn1::table::seq_desc bc_bearer_context_to_setup_resp_s::desc = {
    {seq_table_bc_bearer_context_to_setup_resp_s_fields,
     5,
     UINT64_C(0x2),
     seq_table_bc_bearer_context_to_setup_resp_s_opts,
     4},
    true,
    true,
    nullptr,
    0,
    seq_table_bc_bearer_context_to_setup_resp_s_names};

// BearerContextStatusChange ::= ENUMERATED
static const char*           enum_names_bearer_context_status_change_e_values[] = {"suspend", "resume", "resumeforSDT"};
const asn1::table::enum_desc bearer_context_status_change_opts::desc            = {
    enum_names_bearer_context_status_change_e_values,
    3,
    "bearer_context_status_change_e"};

// CHOInitiation ::= ENUMERATED
static const char*           enum_names_cho_initiation_e_values[] = {"true"};
const asn1::table::enum_desc cho_initiation_opts::desc = {enum_names_cho_initiation_e_values, 1, "cho_initiation_e"};

// CNSupport ::= ENUMERATED
static const char*           enum_names_cn_support_e_values[] = {"c-epc", "c-5gc", "both"};
const asn1::table::enum_desc cn_support_opts::desc            = {enum_names_cn_support_e_values, 3, "cn_support_e"};
int8_t                       cn_support_opts::to_number() const
{
  if (value == c_5gc) {
    return -5;
  }
  invalid_enum_number(value, "cn_support_e");
  return 0;
}

// Endpoint-IP-address-and-port ::= SEQUENCE
static const asn1::table::seq_field seq_table_endpoint_ip_address_and_port_s_fields[] = {
    {offsetof(endpoint_ip_address_and_port_s, endpoint_ip_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(endpoint_ip_address_and_port_s::endpoint_ip_address),
                                asn1::table::struct_json>::value},
    {offsetof(endpoint_ip_address_and_port_s, port_num),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(endpoint_ip_address_and_port_s::port_num), asn1::table::struct_json>::value},
    {offsetof(endpoint_ip_address_and_port_s, ie_exts),
     (int32_t)offsetof(endpoint_ip_address_and_port_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(endpoint_ip_address_and_port_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_endpoint_ip_address_and_port_s_names[] = {
    {"endpoint-IP-Address", nullptr},
    {"portNumber", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_endpoint_ip_address_and_port_s_opts[] = {
    {2, (int32_t)offsetof(endpoint_ip_address_and_port_s, ie_exts_present)}};
const asn1::table::seq_desc endpoint_ip_address_and_port_s::desc = {{seq_table_endpoint_ip_address_and_port_s_fields,
                                                                     3,
                                                                     UINT64_C(0x3),
                                                                     seq_table_endpoint_ip_address_and_port_s_opts,
                                                                     1},
                                                                    false,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_endpoint_ip_address_and_port_s_names};

// CP-TNL-Information-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-IES
static const uint32_t objset_id_cp_tnl_info_ext_ies_o[]    = {74};
static const uint8_t  objset_crit_cp_tnl_info_ext_ies_o[]  = {crit_e::reject};
static const uint8_t  objset_value_cp_tnl_info_ext_ies_o[] = {
    cp_tnl_info_ext_ies_o::value_c::types::endpoint_ip_address_and_port};
static const uint8_t                    objset_presence_cp_tnl_info_ext_ies_o[] = {presence_e::mandatory};
static const asn1::table::objset_column objset_columns_cp_tnl_info_ext_ies_o[]  = {
    {objset_crit_cp_tnl_info_ext_ies_o, crit_e::nulltype},
    {objset_value_cp_tnl_info_ext_ies_o, cp_tnl_info_ext_ies_o::value_c::types::nulltype},
    {objset_presence_cp_tnl_info_ext_ies_o, presence_e::nulltype}};
const asn1::table::objset_desc cp_tnl_info_ext_ies_o::desc = {objset_id_cp_tnl_info_ext_ies_o,
                                                              1,
                                                              "id",
                                                              objset_columns_cp_tnl_info_ext_ies_o};

// Value ::= OPEN TYPE
static const asn1::table::seq_field choice_table_cp_tnl_info_ext_ies_o_value_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &endpoint_ip_address_and_port_s::desc}};
static const asn1::table::field_name choice_table_cp_tnl_info_ext_ies_o_value_c_names[] = {
    {"Endpoint-IP-address-and-port", nullptr}};
const asn1::table::choice_desc cp_tnl_info_ext_ies_o::value_c::desc = {
    asn1::table::enum_int_type<cp_tnl_info_ext_ies_o::value_c::types>(),
    cp_tnl_info_ext_ies_o::value_c::types::nof_types,
    cp_tnl_info_ext_ies_o::value_c::types::nof_exts,
    cp_tnl_info_ext_ies_o::value_c::types::has_ext,
    true,
    choice_table_cp_tnl_info_ext_ies_o_value_c_alternatives,
    nullptr,
    choice_table_cp_tnl_info_ext_ies_o_value_c_names,
    "cp_tnl_info_ext_ies_o::value_c",
    asn1::table::choice_desc::no_member,
    offsetof(cp_tnl_info_ext_ies_o::value_c, c),
    true,
    nullptr,
    true};

static const char*           enum_names_cp_tnl_info_ext_ies_o_value_c_types_values[] = {"Endpoint-IP-address-and-port"};
const asn1::table::enum_desc cp_tnl_info_ext_ies_o::value_c::types_opts::desc        = {
    enum_names_cp_tnl_info_ext_ies_o_value_c_types_values,
    1,
    "cp_tnl_info_ext_ies_o::value_c::types"};

// CP-TNL-Information ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_cp_tnl_info_c_alternatives[] = {
    &asn1::table::alternative_ops<bounded_bitstring<1, 160, true, true>, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc cp_tnl_info_c::storage_desc = {choice_storage_cp_tnl_info_c_alternatives,
                                                                      2,
                                                                      "cp_tnl_info_c"};

bounded_bitstring<1, 160, true, true>& cp_tnl_info_c::set_endpoint_ip_address()
{
  set(types::endpoint_ip_address);
  return c.get<bounded_bitstring<1, 160, true, true>>();
}
protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>& cp_tnl_info_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_cp_tnl_info_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<bounded_bitstring<1, 160, true, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<cp_tnl_info_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_cp_tnl_info_c_names[] = {{"endpoint-IP-Address", nullptr},
                                                                           {"choice-extension", nullptr}};
const asn1::table::choice_desc       cp_tnl_info_c::desc = {asn1::table::enum_int_type<cp_tnl_info_c::types>(),
                                                            cp_tnl_info_c::types::nof_types,
                                                            cp_tnl_info_c::types::nof_exts,
                                                            cp_tnl_info_c::types::has_ext,
                                                            true,
                                                            choice_table_cp_tnl_info_c_alternatives,
                                                            nullptr,
                                                            choice_table_cp_tnl_info_c_names,
                                                            "cp_tnl_info_c",
                                                            offsetof(cp_tnl_info_c, c.type),
                                                            offsetof(cp_tnl_info_c, c.value),
                                                            false,
                                                            &cp_tnl_info_c::storage_desc};

static const char*           enum_names_cp_tnl_info_c_types_values[] = {"endpoint-IP-Address", "choice-extension"};
const asn1::table::enum_desc cp_tnl_info_c::types_opts::desc         = {enum_names_cp_tnl_info_c_types_values,
                                                                        2,
                                                                        "cp_tnl_info_c::types"};

// UL-Configuration ::= ENUMERATED
static const char*           enum_names_ul_cfg_e_values[] = {"no-data", "shared", "only"};
const asn1::table::enum_desc ul_cfg_opts::desc            = {enum_names_ul_cfg_e_values, 3, "ul_cfg_e"};

// DL-TX-Stop ::= ENUMERATED
static const char*           enum_names_dl_tx_stop_e_values[] = {"stop", "resume"};
const asn1::table::enum_desc dl_tx_stop_opts::desc            = {enum_names_dl_tx_stop_e_values, 2, "dl_tx_stop_e"};

// RAT-Type ::= ENUMERATED
static const char*           enum_names_rat_type_e_values[] = {"e-UTRA", "nR"};
const asn1::table::enum_desc rat_type_opts::desc            = {enum_names_rat_type_e_values, 2, "rat_type_e"};

static const uint32_t objset_id_cell_group_info_item_ext_ies_o[]   = {127};
static const uint8_t  objset_crit_cell_group_info_item_ext_ies_o[] = {crit_e::ignore};

// Cell-Group-Information-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_cell_group_info_item_ext_ies_container_values[] = {
    {offsetof(cell_group_info_item_ext_ies_container, nof_tunnels),
     (int32_t)offsetof(cell_group_info_item_ext_ies_container, nof_tunnels_present),
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(4),
     nullptr}};
static const int32_t ie_table_cell_group_info_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(cell_group_info_item_ext_ies_container, nof_tunnels_present)};
const asn1::table::ie_container_desc cell_group_info_item_ext_ies_container::desc = {
    objset_id_cell_group_info_item_ext_ies_o,
    objset_crit_cell_group_info_item_ext_ies_o,
    ie_table_cell_group_info_item_ext_ies_container_values,
    ie_table_cell_group_info_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// Cell-Group-Information-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_cell_group_info_item_s_fields[] = {
    {offsetof(cell_group_info_item_s, cell_group_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(cell_group_info_item_s, ul_cfg),
     (int32_t)offsetof(cell_group_info_item_s, ul_cfg_present),
     asn1::table::enum_op<decltype(cell_group_info_item_s::ul_cfg)>(),
     decltype(cell_group_info_item_s::ul_cfg)::has_ext,
     false,
     decltype(cell_group_info_item_s::ul_cfg)::nof_types,
     decltype(cell_group_info_item_s::ul_cfg)::nof_exts,
     nullptr,
     &ul_cfg_opts::desc},
    {offsetof(cell_group_info_item_s, dl_tx_stop),
     (int32_t)offsetof(cell_group_info_item_s, dl_tx_stop_present),
     asn1::table::enum_op<decltype(cell_group_info_item_s::dl_tx_stop)>(),
     decltype(cell_group_info_item_s::dl_tx_stop)::has_ext,
     false,
     decltype(cell_group_info_item_s::dl_tx_stop)::nof_types,
     decltype(cell_group_info_item_s::dl_tx_stop)::nof_exts,
     nullptr,
     &dl_tx_stop_opts::desc},
    {offsetof(cell_group_info_item_s, rat_type),
     (int32_t)offsetof(cell_group_info_item_s, rat_type_present),
     asn1::table::enum_op<decltype(cell_group_info_item_s::rat_type)>(),
     decltype(cell_group_info_item_s::rat_type)::has_ext,
     false,
     decltype(cell_group_info_item_s::rat_type)::nof_types,
     decltype(cell_group_info_item_s::rat_type)::nof_exts,
     nullptr,
     &rat_type_opts::desc},
    {offsetof(cell_group_info_item_s, ie_exts),
     (int32_t)offsetof(cell_group_info_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_group_info_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_cell_group_info_item_s_names[] = {{"cell-Group-ID", nullptr},
                                                                                 {"uL-Configuration", nullptr},
                                                                                 {"dL-TX-Stop", nullptr},
                                                                                 {"rAT-Type", nullptr},
                                                                                 {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_cell_group_info_item_s_opts[]  = {
    {1, (int32_t)offsetof(cell_group_info_item_s, ul_cfg_present)},
    {2, (int32_t)offsetof(cell_group_info_item_s, dl_tx_stop_present)},
    {3, (int32_t)offsetof(cell_group_info_item_s, rat_type_present)},
    {4, (int32_t)offsetof(cell_group_info_item_s, ie_exts_present)}};
const asn1::table::seq_desc cell_group_info_item_s::desc = {
    {seq_table_cell_group_info_item_s_fields, 5, UINT64_C(0x1), seq_table_cell_group_info_item_s_opts, 4},
    true,
    true,
    nullptr,
    0,
    seq_table_cell_group_info_item_s_names};

// CipheringAlgorithm ::= ENUMERATED
static const char* enum_names_ciphering_algorithm_e_values[] = {"nEA0", "c-128-NEA1", "c-128-NEA2", "c-128-NEA3"};
const asn1::table::enum_desc ciphering_algorithm_opts::desc  = {enum_names_ciphering_algorithm_e_values,
                                                                4,
                                                                "ciphering_algorithm_e"};

// ConfidentialityProtectionIndication ::= ENUMERATED
static const char* enum_names_confidentiality_protection_ind_e_values[] = {"required", "preferred", "not-needed"};
const asn1::table::enum_desc confidentiality_protection_ind_opts::desc  = {
    enum_names_confidentiality_protection_ind_e_values,
    3,
    "confidentiality_protection_ind_e"};

// ConfidentialityProtectionResult ::= ENUMERATED
static const char*           enum_names_confidentiality_protection_result_e_values[] = {"performed", "not-performed"};
const asn1::table::enum_desc confidentiality_protection_result_opts::desc            = {
    enum_names_confidentiality_protection_result_e_values,
    2,
    "confidentiality_protection_result_e"};

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
    {offsetof(crit_diagnostics_s, transaction_id),
     (int32_t)offsetof(crit_diagnostics_s, transaction_id_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(255),
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
                                                                             {"transactionID", nullptr},
                                                                             {"iEsCriticalityDiagnostics", nullptr},
                                                                             {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_crit_diagnostics_s_opts[]  = {
    {0, (int32_t)offsetof(crit_diagnostics_s, proc_code_present)},
    {1, (int32_t)offsetof(crit_diagnostics_s, trigger_msg_present)},
    {2, (int32_t)offsetof(crit_diagnostics_s, proc_crit_present)},
    {3, (int32_t)offsetof(crit_diagnostics_s, transaction_id_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(crit_diagnostics_s, ie_exts_present)}};
const asn1::table::seq_desc crit_diagnostics_s::desc = {
    {seq_table_crit_diagnostics_s_fields, 6, UINT64_C(0x0), seq_table_crit_diagnostics_s_opts, 6},
    true,
    true,
    nullptr,
    0,
    seq_table_crit_diagnostics_s_names};

// DAPSRequestInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_daps_request_info_s_fields[] = {
    {offsetof(daps_request_info_s, daps_ind),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(daps_request_info_s::daps_ind)>(),
     decltype(daps_request_info_s::daps_ind)::has_ext,
     false,
     decltype(daps_request_info_s::daps_ind)::nof_types,
     decltype(daps_request_info_s::daps_ind)::nof_exts,
     nullptr,
     &daps_request_info_s::daps_ind_opts::desc},
    {offsetof(daps_request_info_s, ie_exts),
     (int32_t)offsetof(daps_request_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(daps_request_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_daps_request_info_s_names[] = {{"dapsIndicator", nullptr},
                                                                              {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_daps_request_info_s_opts[]  = {
    {1, (int32_t)offsetof(daps_request_info_s, ie_exts_present)}};
const asn1::table::seq_desc daps_request_info_s::desc = {
    {seq_table_daps_request_info_s_fields, 2, UINT64_C(0x1), seq_table_daps_request_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_daps_request_info_s_names};

static const char*           enum_names_daps_request_info_s_daps_ind_e__values[] = {"daps-HO-required"};
const asn1::table::enum_desc daps_request_info_s::daps_ind_opts::desc            = {
    enum_names_daps_request_info_s_daps_ind_e__values,
    1,
    "daps_request_info_s::daps_ind_e_"};

// PDCP-Count ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdcp_count_s_fields[] = {
    {offsetof(pdcp_count_s, pdcp_sn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(262143),
     nullptr},
    {offsetof(pdcp_count_s, hfn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     INT64_C(0),
     UINT64_C(4294967295),
     nullptr},
    {offsetof(pdcp_count_s, ie_exts),
     (int32_t)offsetof(pdcp_count_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pdcp_count_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pdcp_count_s_names[] = {{"pDCP-SN", nullptr},
                                                                       {"hFN", nullptr},
                                                                       {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_pdcp_count_s_opts[]  = {
    {2, (int32_t)offsetof(pdcp_count_s, ie_exts_present)}};
const asn1::table::seq_desc pdcp_count_s::desc = {
    {seq_table_pdcp_count_s_fields, 3, UINT64_C(0x3), seq_table_pdcp_count_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_pdcp_count_s_names};

// DLDiscarding ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_discarding_s_fields[] = {
    {offsetof(dl_discarding_s, dl_discarding_count_val),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_count_s::desc},
    {offsetof(dl_discarding_s, ie_exts),
     (int32_t)offsetof(dl_discarding_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(dl_discarding_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_dl_discarding_s_names[] = {{"dLDiscardingCountVal", nullptr},
                                                                          {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_dl_discarding_s_opts[]  = {
    {1, (int32_t)offsetof(dl_discarding_s, ie_exts_present)}};
const asn1::table::seq_desc dl_discarding_s::desc = {
    {seq_table_dl_discarding_s_fields, 2, UINT64_C(0x1), seq_table_dl_discarding_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_dl_discarding_s_names};

// DLUPTNLAddressToUpdateItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_up_tnl_address_to_upd_item_s_fields[] = {
    {offsetof(dl_up_tnl_address_to_upd_item_s, old_tnl_adress),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(dl_up_tnl_address_to_upd_item_s::old_tnl_adress),
                                asn1::table::struct_json>::value},
    {offsetof(dl_up_tnl_address_to_upd_item_s, new_tnl_adress),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(dl_up_tnl_address_to_upd_item_s::new_tnl_adress),
                                asn1::table::struct_json>::value},
    {offsetof(dl_up_tnl_address_to_upd_item_s, ie_exts),
     (int32_t)offsetof(dl_up_tnl_address_to_upd_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(dl_up_tnl_address_to_upd_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_dl_up_tnl_address_to_upd_item_s_names[] = {{"oldTNLAdress", nullptr},
                                                                                          {"newTNLAdress", nullptr},
                                                                                          {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_dl_up_tnl_address_to_upd_item_s_opts[]  = {
    {2, (int32_t)offsetof(dl_up_tnl_address_to_upd_item_s, ie_exts_present)}};
const asn1::table::seq_desc dl_up_tnl_address_to_upd_item_s::desc = {{seq_table_dl_up_tnl_address_to_upd_item_s_fields,
                                                                      3,
                                                                      UINT64_C(0x3),
                                                                      seq_table_dl_up_tnl_address_to_upd_item_s_opts,
                                                                      1},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_dl_up_tnl_address_to_upd_item_s_names};

// DRB-Confirm-Modified-Item-EUTRAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_confirm_modified_item_eutran_s_fields[] = {
    {offsetof(drb_confirm_modified_item_eutran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_confirm_modified_item_eutran_s, cell_group_info),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     4,
     &asn1::table::item_list_codec<decltype(drb_confirm_modified_item_eutran_s::cell_group_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_group_info_item_s::desc},
    {offsetof(drb_confirm_modified_item_eutran_s, ie_exts),
     (int32_t)offsetof(drb_confirm_modified_item_eutran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_confirm_modified_item_eutran_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_confirm_modified_item_eutran_s_names[] = {
    {"dRB-ID", nullptr},
    {"cell-Group-Information", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_confirm_modified_item_eutran_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(drb_confirm_modified_item_eutran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_confirm_modified_item_eutran_s::desc = {
    {seq_table_drb_confirm_modified_item_eutran_s_fields,
     3,
     UINT64_C(0x1),
     seq_table_drb_confirm_modified_item_eutran_s_opts,
     2},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_confirm_modified_item_eutran_s_names};

// DRB-Confirm-Modified-Item-NG-RAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_confirm_modified_item_ng_ran_s_fields[] = {
    {offsetof(drb_confirm_modified_item_ng_ran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_confirm_modified_item_ng_ran_s, cell_group_info),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     4,
     &asn1::table::item_list_codec<decltype(drb_confirm_modified_item_ng_ran_s::cell_group_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_group_info_item_s::desc},
    {offsetof(drb_confirm_modified_item_ng_ran_s, ie_exts),
     (int32_t)offsetof(drb_confirm_modified_item_ng_ran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_confirm_modified_item_ng_ran_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_confirm_modified_item_ng_ran_s_names[] = {
    {"dRB-ID", nullptr},
    {"cell-Group-Information", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_confirm_modified_item_ng_ran_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(drb_confirm_modified_item_ng_ran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_confirm_modified_item_ng_ran_s::desc = {
    {seq_table_drb_confirm_modified_item_ng_ran_s_fields,
     3,
     UINT64_C(0x1),
     seq_table_drb_confirm_modified_item_ng_ran_s_opts,
     2},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_confirm_modified_item_ng_ran_s_names};

// DRB-Failed-Item-EUTRAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_failed_item_eutran_s_fields[] = {
    {offsetof(drb_failed_item_eutran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_failed_item_eutran_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(drb_failed_item_eutran_s, ie_exts),
     (int32_t)offsetof(drb_failed_item_eutran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_failed_item_eutran_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_failed_item_eutran_s_names[] = {{"dRB-ID", nullptr},
                                                                                   {"cause", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_drb_failed_item_eutran_s_opts[]  = {
    {2, (int32_t)offsetof(drb_failed_item_eutran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_failed_item_eutran_s::desc = {
    {seq_table_drb_failed_item_eutran_s_fields, 3, UINT64_C(0x3), seq_table_drb_failed_item_eutran_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_failed_item_eutran_s_names};

// DRB-Failed-Item-NG-RAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_failed_item_ng_ran_s_fields[] = {
    {offsetof(drb_failed_item_ng_ran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_failed_item_ng_ran_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(drb_failed_item_ng_ran_s, ie_exts),
     (int32_t)offsetof(drb_failed_item_ng_ran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_failed_item_ng_ran_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_failed_item_ng_ran_s_names[] = {{"dRB-ID", nullptr},
                                                                                   {"cause", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_drb_failed_item_ng_ran_s_opts[]  = {
    {2, (int32_t)offsetof(drb_failed_item_ng_ran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_failed_item_ng_ran_s::desc = {
    {seq_table_drb_failed_item_ng_ran_s_fields, 3, UINT64_C(0x3), seq_table_drb_failed_item_ng_ran_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_failed_item_ng_ran_s_names};

// DRB-Failed-Mod-Item-EUTRAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_failed_mod_item_eutran_s_fields[] = {
    {offsetof(drb_failed_mod_item_eutran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_failed_mod_item_eutran_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(drb_failed_mod_item_eutran_s, ie_exts),
     (int32_t)offsetof(drb_failed_mod_item_eutran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_failed_mod_item_eutran_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_failed_mod_item_eutran_s_names[] = {{"dRB-ID", nullptr},
                                                                                       {"cause", nullptr},
                                                                                       {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_drb_failed_mod_item_eutran_s_opts[]  = {
    {2, (int32_t)offsetof(drb_failed_mod_item_eutran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_failed_mod_item_eutran_s::desc = {
    {seq_table_drb_failed_mod_item_eutran_s_fields, 3, UINT64_C(0x3), seq_table_drb_failed_mod_item_eutran_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_failed_mod_item_eutran_s_names};

// DRB-Failed-Mod-Item-NG-RAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_failed_mod_item_ng_ran_s_fields[] = {
    {offsetof(drb_failed_mod_item_ng_ran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_failed_mod_item_ng_ran_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(drb_failed_mod_item_ng_ran_s, ie_exts),
     (int32_t)offsetof(drb_failed_mod_item_ng_ran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_failed_mod_item_ng_ran_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_failed_mod_item_ng_ran_s_names[] = {{"dRB-ID", nullptr},
                                                                                       {"cause", nullptr},
                                                                                       {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_drb_failed_mod_item_ng_ran_s_opts[]  = {
    {2, (int32_t)offsetof(drb_failed_mod_item_ng_ran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_failed_mod_item_ng_ran_s::desc = {
    {seq_table_drb_failed_mod_item_ng_ran_s_fields, 3, UINT64_C(0x3), seq_table_drb_failed_mod_item_ng_ran_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_failed_mod_item_ng_ran_s_names};

// DRB-Failed-To-Modify-Item-EUTRAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_failed_to_modify_item_eutran_s_fields[] = {
    {offsetof(drb_failed_to_modify_item_eutran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_failed_to_modify_item_eutran_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(drb_failed_to_modify_item_eutran_s, ie_exts),
     (int32_t)offsetof(drb_failed_to_modify_item_eutran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_failed_to_modify_item_eutran_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_failed_to_modify_item_eutran_s_names[] = {
    {"dRB-ID", nullptr},
    {"cause", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_failed_to_modify_item_eutran_s_opts[] = {
    {2, (int32_t)offsetof(drb_failed_to_modify_item_eutran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_failed_to_modify_item_eutran_s::desc = {
    {seq_table_drb_failed_to_modify_item_eutran_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_drb_failed_to_modify_item_eutran_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_failed_to_modify_item_eutran_s_names};

// DRB-Failed-To-Modify-Item-NG-RAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_failed_to_modify_item_ng_ran_s_fields[] = {
    {offsetof(drb_failed_to_modify_item_ng_ran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_failed_to_modify_item_ng_ran_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(drb_failed_to_modify_item_ng_ran_s, ie_exts),
     (int32_t)offsetof(drb_failed_to_modify_item_ng_ran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_failed_to_modify_item_ng_ran_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_failed_to_modify_item_ng_ran_s_names[] = {
    {"dRB-ID", nullptr},
    {"cause", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_failed_to_modify_item_ng_ran_s_opts[] = {
    {2, (int32_t)offsetof(drb_failed_to_modify_item_ng_ran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_failed_to_modify_item_ng_ran_s::desc = {
    {seq_table_drb_failed_to_modify_item_ng_ran_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_drb_failed_to_modify_item_ng_ran_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_failed_to_modify_item_ng_ran_s_names};

// DRB-Measurement-Results-Information-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_meas_results_info_item_s_fields[] = {
    {offsetof(drb_meas_results_info_item_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_meas_results_info_item_s, ul_d1_result),
     (int32_t)offsetof(drb_meas_results_info_item_s, ul_d1_result_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(10000),
     nullptr},
    {offsetof(drb_meas_results_info_item_s, ie_exts),
     (int32_t)offsetof(drb_meas_results_info_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_meas_results_info_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_meas_results_info_item_s_names[] = {{"dRB-ID", nullptr},
                                                                                       {"uL-D1-Result", nullptr},
                                                                                       {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_drb_meas_results_info_item_s_opts[]  = {
    {1, (int32_t)offsetof(drb_meas_results_info_item_s, ul_d1_result_present)},
    {2, (int32_t)offsetof(drb_meas_results_info_item_s, ie_exts_present)}};
const asn1::table::seq_desc drb_meas_results_info_item_s::desc = {
    {seq_table_drb_meas_results_info_item_s_fields, 3, UINT64_C(0x1), seq_table_drb_meas_results_info_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_meas_results_info_item_s_names};

// DRBBStatusTransfer ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_b_status_transfer_s_fields[] = {
    {offsetof(drb_b_status_transfer_s, receive_statusof_pdcp_sdu),
     (int32_t)offsetof(drb_b_status_transfer_s, receive_statusof_pdcp_sdu_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_b_status_transfer_s::receive_statusof_pdcp_sdu),
                                asn1::table::struct_json>::value},
    {offsetof(drb_b_status_transfer_s, count_value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_count_s::desc},
    {offsetof(drb_b_status_transfer_s, ie_ext),
     (int32_t)offsetof(drb_b_status_transfer_s, ie_ext_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_b_status_transfer_s::ie_ext), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_b_status_transfer_s_names[] = {{"receiveStatusofPDCPSDU", nullptr},
                                                                                  {"countValue", nullptr},
                                                                                  {"iE-Extension", nullptr}};
static const asn1::table::seq_opt    seq_table_drb_b_status_transfer_s_opts[]  = {
    {0, (int32_t)offsetof(drb_b_status_transfer_s, receive_statusof_pdcp_sdu_present)},
    {2, (int32_t)offsetof(drb_b_status_transfer_s, ie_ext_present)}};
const asn1::table::seq_desc drb_b_status_transfer_s::desc = {
    {seq_table_drb_b_status_transfer_s_fields, 3, UINT64_C(0x2), seq_table_drb_b_status_transfer_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_b_status_transfer_s_names};

// PDCP-SN-Status-Information ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdcp_sn_status_info_s_fields[] = {
    {offsetof(pdcp_sn_status_info_s, pdcp_status_transfer_ul),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drb_b_status_transfer_s::desc},
    {offsetof(pdcp_sn_status_info_s, pdcp_status_transfer_dl),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_count_s::desc},
    {offsetof(pdcp_sn_status_info_s, ie_ext),
     (int32_t)offsetof(pdcp_sn_status_info_s, ie_ext_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pdcp_sn_status_info_s::ie_ext), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pdcp_sn_status_info_s_names[] = {{"pdcpStatusTransfer-UL", nullptr},
                                                                                {"pdcpStatusTransfer-DL", nullptr},
                                                                                {"iE-Extension", nullptr}};
static const asn1::table::seq_opt    seq_table_pdcp_sn_status_info_s_opts[]  = {
    {2, (int32_t)offsetof(pdcp_sn_status_info_s, ie_ext_present)}};
const asn1::table::seq_desc pdcp_sn_status_info_s::desc = {
    {seq_table_pdcp_sn_status_info_s_fields, 3, UINT64_C(0x3), seq_table_pdcp_sn_status_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_pdcp_sn_status_info_s_names};

// QoS-Mapping-Information ::= SEQUENCE
static const asn1::table::seq_field seq_table_qos_map_info_s_fields[] = {
    {offsetof(qos_map_info_s, dscp),
     (int32_t)offsetof(qos_map_info_s, dscp_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(qos_map_info_s::dscp), asn1::table::struct_json>::value},
    {offsetof(qos_map_info_s, flow_label),
     (int32_t)offsetof(qos_map_info_s, flow_label_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(qos_map_info_s::flow_label), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_qos_map_info_s_names[] = {{"dscp", nullptr}, {"flow-label", nullptr}};
static const asn1::table::seq_opt    seq_table_qos_map_info_s_opts[]  = {
    {0, (int32_t)offsetof(qos_map_info_s, dscp_present)},
    {1, (int32_t)offsetof(qos_map_info_s, flow_label_present)}};
const asn1::table::seq_desc qos_map_info_s::desc = {
    {seq_table_qos_map_info_s_fields, 2, UINT64_C(0x0), seq_table_qos_map_info_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_qos_map_info_s_names};

// IndirectPathIndication ::= ENUMERATED
static const char*           enum_names_indirect_path_ind_e_values[] = {"true"};
const asn1::table::enum_desc indirect_path_ind_opts::desc            = {enum_names_indirect_path_ind_e_values,
                                                                        1,
                                                                        "indirect_path_ind_e"};

static const uint32_t objset_id_up_params_item_ext_ies_o[]   = {107, 206};
static const uint8_t  objset_crit_up_params_item_ext_ies_o[] = {crit_e::reject, crit_e::ignore};

// UP-Parameters-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_up_params_item_ext_ies_container_values[] = {
    {offsetof(up_params_item_ext_ies_container, qos_map_info),
     (int32_t)offsetof(up_params_item_ext_ies_container, qos_map_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &qos_map_info_s::desc},
    {offsetof(up_params_item_ext_ies_container, indirect_path_ind),
     (int32_t)offsetof(up_params_item_ext_ies_container, indirect_path_ind_present),
     asn1::table::enum_op<decltype(up_params_item_ext_ies_container::indirect_path_ind)>(),
     decltype(up_params_item_ext_ies_container::indirect_path_ind)::has_ext,
     false,
     decltype(up_params_item_ext_ies_container::indirect_path_ind)::nof_types,
     decltype(up_params_item_ext_ies_container::indirect_path_ind)::nof_exts,
     nullptr,
     &indirect_path_ind_opts::desc}};
static const int32_t ie_table_up_params_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(up_params_item_ext_ies_container, qos_map_info_present),
    (int32_t)offsetof(up_params_item_ext_ies_container, indirect_path_ind_present)};
const asn1::table::ie_container_desc up_params_item_ext_ies_container::desc = {
    objset_id_up_params_item_ext_ies_o,
    objset_crit_up_params_item_ext_ies_o,
    ie_table_up_params_item_ext_ies_container_values,
    ie_table_up_params_item_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// UP-Parameters-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_up_params_item_s_fields[] = {
    {offsetof(up_params_item_s, up_tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(up_params_item_s, cell_group_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(up_params_item_s, ie_exts),
     (int32_t)offsetof(up_params_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &up_params_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_up_params_item_s_names[] = {{"uP-TNL-Information", nullptr},
                                                                           {"cell-Group-ID", nullptr},
                                                                           {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_up_params_item_s_opts[]  = {
    {2, (int32_t)offsetof(up_params_item_s, ie_exts_present)}};
const asn1::table::seq_desc up_params_item_s::desc = {
    {seq_table_up_params_item_s_fields, 3, UINT64_C(0x3), seq_table_up_params_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_up_params_item_s_names};

// DRB-Modified-Item-EUTRAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_modified_item_eutran_s_fields[] = {
    {offsetof(drb_modified_item_eutran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_modified_item_eutran_s, s1_dl_up_tnl_info),
     (int32_t)offsetof(drb_modified_item_eutran_s, s1_dl_up_tnl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(drb_modified_item_eutran_s, pdcp_sn_status_info),
     (int32_t)offsetof(drb_modified_item_eutran_s, pdcp_sn_status_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_sn_status_info_s::desc},
    {offsetof(drb_modified_item_eutran_s, ul_up_transport_params),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(drb_modified_item_eutran_s::ul_up_transport_params),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &up_params_item_s::desc},
    {offsetof(drb_modified_item_eutran_s, ie_exts),
     (int32_t)offsetof(drb_modified_item_eutran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_modified_item_eutran_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_modified_item_eutran_s_names[] = {
    {"dRB-ID", nullptr},
    {"s1-DL-UP-TNL-Information", nullptr},
    {"pDCP-SN-Status-Information", nullptr},
    {"uL-UP-Transport-Parameters", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_modified_item_eutran_s_opts[] = {
    {1, (int32_t)offsetof(drb_modified_item_eutran_s, s1_dl_up_tnl_info_present)},
    {2, (int32_t)offsetof(drb_modified_item_eutran_s, pdcp_sn_status_info_present)},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(drb_modified_item_eutran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_modified_item_eutran_s::desc = {
    {seq_table_drb_modified_item_eutran_s_fields, 5, UINT64_C(0x1), seq_table_drb_modified_item_eutran_s_opts, 4},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_modified_item_eutran_s_names};

// FirstDLCount ::= SEQUENCE
static const asn1::table::seq_field seq_table_first_dl_count_s_fields[] = {
    {offsetof(first_dl_count_s, first_dl_count_val),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_count_s::desc},
    {offsetof(first_dl_count_s, ie_exts),
     (int32_t)offsetof(first_dl_count_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(first_dl_count_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_first_dl_count_s_names[] = {{"firstDLCountVal", nullptr},
                                                                           {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_first_dl_count_s_opts[]  = {
    {1, (int32_t)offsetof(first_dl_count_s, ie_exts_present)}};
const asn1::table::seq_desc first_dl_count_s::desc = {
    {seq_table_first_dl_count_s_fields, 2, UINT64_C(0x1), seq_table_first_dl_count_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_first_dl_count_s_names};

// EarlyForwardingCOUNTInfo ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_early_forwarding_count_info_c_alternatives[] = {
    &asn1::table::alternative_ops<first_dl_count_s, false>::value,
    &asn1::table::alternative_ops<dl_discarding_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc early_forwarding_count_info_c::storage_desc = {
    choice_storage_early_forwarding_count_info_c_alternatives,
    3,
    "early_forwarding_count_info_c"};

first_dl_count_s& early_forwarding_count_info_c::set_first_dl_count()
{
  set(types::first_dl_count);
  return c.get<first_dl_count_s>();
}
dl_discarding_s& early_forwarding_count_info_c::set_dl_discarding_count()
{
  set(types::dl_discarding_count);
  return c.get<dl_discarding_s>();
}
protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>& early_forwarding_count_info_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_early_forwarding_count_info_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &first_dl_count_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &dl_discarding_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<early_forwarding_count_info_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_early_forwarding_count_info_c_names[] = {
    {"firstDLCount", nullptr},
    {"dLDiscardingCount", nullptr},
    {"choice-Extension", nullptr}};
const asn1::table::choice_desc early_forwarding_count_info_c::desc = {
    asn1::table::enum_int_type<early_forwarding_count_info_c::types>(),
    early_forwarding_count_info_c::types::nof_types,
    early_forwarding_count_info_c::types::nof_exts,
    early_forwarding_count_info_c::types::has_ext,
    true,
    choice_table_early_forwarding_count_info_c_alternatives,
    nullptr,
    choice_table_early_forwarding_count_info_c_names,
    "early_forwarding_count_info_c",
    offsetof(early_forwarding_count_info_c, c.type),
    offsetof(early_forwarding_count_info_c, c.value),
    false,
    &early_forwarding_count_info_c::storage_desc};

static const char*           enum_names_early_forwarding_count_info_c_types_values[] = {"firstDLCount",
                                                                                        "dLDiscardingCount",
                                                                                        "choice-Extension"};
const asn1::table::enum_desc early_forwarding_count_info_c::types_opts::desc         = {
    enum_names_early_forwarding_count_info_c_types_values,
    3,
    "early_forwarding_count_info_c::types"};

static const uint32_t objset_id_drb_modified_item_ng_ran_ext_ies_o[]   = {123, 71};
static const uint8_t  objset_crit_drb_modified_item_ng_ran_ext_ies_o[] = {crit_e::reject, crit_e::ignore};

// DRB-Modified-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_drb_modified_item_ng_ran_ext_ies_container_values[] = {
    {offsetof(drb_modified_item_ng_ran_ext_ies_container, early_forwarding_count_info),
     (int32_t)offsetof(drb_modified_item_ng_ran_ext_ies_container, early_forwarding_count_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &early_forwarding_count_info_c::desc},
    {offsetof(drb_modified_item_ng_ran_ext_ies_container, old_qos_flow_map_ul_endmarkerexpected),
     (int32_t)offsetof(drb_modified_item_ng_ran_ext_ies_container, old_qos_flow_map_ul_endmarkerexpected_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<
         decltype(drb_modified_item_ng_ran_ext_ies_container::old_qos_flow_map_ul_endmarkerexpected),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &qos_flow_item_s::desc}};
static const int32_t ie_table_drb_modified_item_ng_ran_ext_ies_container_presences[] = {
    (int32_t)offsetof(drb_modified_item_ng_ran_ext_ies_container, early_forwarding_count_info_present),
    (int32_t)offsetof(drb_modified_item_ng_ran_ext_ies_container, old_qos_flow_map_ul_endmarkerexpected_present)};
const asn1::table::ie_container_desc drb_modified_item_ng_ran_ext_ies_container::desc = {
    objset_id_drb_modified_item_ng_ran_ext_ies_o,
    objset_crit_drb_modified_item_ng_ran_ext_ies_o,
    ie_table_drb_modified_item_ng_ran_ext_ies_container_values,
    ie_table_drb_modified_item_ng_ran_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// DRB-Modified-Item-NG-RAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_modified_item_ng_ran_s_fields[] = {
    {offsetof(drb_modified_item_ng_ran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_modified_item_ng_ran_s, ul_up_transport_params),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(drb_modified_item_ng_ran_s::ul_up_transport_params),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &up_params_item_s::desc},
    {offsetof(drb_modified_item_ng_ran_s, pdcp_sn_status_info),
     (int32_t)offsetof(drb_modified_item_ng_ran_s, pdcp_sn_status_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_sn_status_info_s::desc},
    {offsetof(drb_modified_item_ng_ran_s, flow_setup_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(drb_modified_item_ng_ran_s::flow_setup_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_item_s::desc},
    {offsetof(drb_modified_item_ng_ran_s, flow_failed_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(drb_modified_item_ng_ran_s::flow_failed_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_failed_item_s::desc},
    {offsetof(drb_modified_item_ng_ran_s, ie_exts),
     (int32_t)offsetof(drb_modified_item_ng_ran_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &drb_modified_item_ng_ran_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_drb_modified_item_ng_ran_s_names[] = {
    {"dRB-ID", nullptr},
    {"uL-UP-Transport-Parameters", nullptr},
    {"pDCP-SN-Status-Information", nullptr},
    {"flow-Setup-List", nullptr},
    {"flow-Failed-List", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_modified_item_ng_ran_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(drb_modified_item_ng_ran_s, pdcp_sn_status_info_present)},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(drb_modified_item_ng_ran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_modified_item_ng_ran_s::desc = {
    {seq_table_drb_modified_item_ng_ran_s_fields, 6, UINT64_C(0x1), seq_table_drb_modified_item_ng_ran_s_opts, 5},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_modified_item_ng_ran_s_names};

// QoS-Flow-Removed-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_qos_flow_remd_item_s_fields[] = {
    {offsetof(qos_flow_remd_item_s, qos_flow_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(qos_flow_remd_item_s, qos_flow_released_in_session),
     (int32_t)offsetof(qos_flow_remd_item_s, qos_flow_released_in_session_present),
     asn1::table::enum_op<decltype(qos_flow_remd_item_s::qos_flow_released_in_session)>(),
     decltype(qos_flow_remd_item_s::qos_flow_released_in_session)::has_ext,
     false,
     decltype(qos_flow_remd_item_s::qos_flow_released_in_session)::nof_types,
     decltype(qos_flow_remd_item_s::qos_flow_released_in_session)::nof_exts,
     nullptr,
     &qos_flow_remd_item_s::qos_flow_released_in_session_opts::desc},
    {offsetof(qos_flow_remd_item_s, qos_flow_accumulated_session_time),
     (int32_t)offsetof(qos_flow_remd_item_s, qos_flow_accumulated_session_time_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(qos_flow_remd_item_s::qos_flow_accumulated_session_time),
                                asn1::table::string_json>::value},
    {offsetof(qos_flow_remd_item_s, ie_exts),
     (int32_t)offsetof(qos_flow_remd_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(qos_flow_remd_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_qos_flow_remd_item_s_names[] = {
    {"qoS-Flow-Identifier", nullptr},
    {"qoS-Flow-Released-In-Session", nullptr},
    {"qoS-Flow-Accumulated-Session-Time", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_qos_flow_remd_item_s_opts[] = {
    {1, (int32_t)offsetof(qos_flow_remd_item_s, qos_flow_released_in_session_present)},
    {2, (int32_t)offsetof(qos_flow_remd_item_s, qos_flow_accumulated_session_time_present)},
    {3, (int32_t)offsetof(qos_flow_remd_item_s, ie_exts_present)}};
const asn1::table::seq_desc qos_flow_remd_item_s::desc = {
    {seq_table_qos_flow_remd_item_s_fields, 4, UINT64_C(0x1), seq_table_qos_flow_remd_item_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_qos_flow_remd_item_s_names};

static const char* enum_names_qos_flow_remd_item_s_qos_flow_released_in_session_e__values[] = {
    "released-in-session",
    "not-released-in-session"};
const asn1::table::enum_desc qos_flow_remd_item_s::qos_flow_released_in_session_opts::desc = {
    enum_names_qos_flow_remd_item_s_qos_flow_released_in_session_e__values,
    2,
    "qos_flow_remd_item_s::qos_flow_released_in_session_e_"};

// DRB-Removed-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_remd_item_s_fields[] = {
    {offsetof(drb_remd_item_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_remd_item_s, drb_released_in_session),
     (int32_t)offsetof(drb_remd_item_s, drb_released_in_session_present),
     asn1::table::enum_op<decltype(drb_remd_item_s::drb_released_in_session)>(),
     decltype(drb_remd_item_s::drb_released_in_session)::has_ext,
     false,
     decltype(drb_remd_item_s::drb_released_in_session)::nof_types,
     decltype(drb_remd_item_s::drb_released_in_session)::nof_exts,
     nullptr,
     &drb_remd_item_s::drb_released_in_session_opts::desc},
    {offsetof(drb_remd_item_s, drb_accumulated_session_time),
     (int32_t)offsetof(drb_remd_item_s, drb_accumulated_session_time_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_remd_item_s::drb_accumulated_session_time),
                                asn1::table::string_json>::value},
    {offsetof(drb_remd_item_s, qos_flow_remd_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(drb_remd_item_s::qos_flow_remd_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_remd_item_s::desc},
    {offsetof(drb_remd_item_s, ie_exts),
     (int32_t)offsetof(drb_remd_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_remd_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_remd_item_s_names[] = {{"dRB-ID", nullptr},
                                                                          {"dRB-Released-In-Session", nullptr},
                                                                          {"dRB-Accumulated-Session-Time", nullptr},
                                                                          {"qoS-Flow-Removed-List", nullptr},
                                                                          {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_drb_remd_item_s_opts[]  = {
    {1, (int32_t)offsetof(drb_remd_item_s, drb_released_in_session_present)},
    {2, (int32_t)offsetof(drb_remd_item_s, drb_accumulated_session_time_present)},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(drb_remd_item_s, ie_exts_present)}};
const asn1::table::seq_desc drb_remd_item_s::desc = {
    {seq_table_drb_remd_item_s_fields, 5, UINT64_C(0x1), seq_table_drb_remd_item_s_opts, 4},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_remd_item_s_names};

static const char*           enum_names_drb_remd_item_s_drb_released_in_session_e__values[] = {"released-in-session",
                                                                                               "not-released-in-session"};
const asn1::table::enum_desc drb_remd_item_s::drb_released_in_session_opts::desc            = {
    enum_names_drb_remd_item_s_drb_released_in_session_e__values,
    2,
    "drb_remd_item_s::drb_released_in_session_e_"};

// GNB-CU-UP-CellGroupRelatedConfiguration-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_gnb_cu_up_cell_group_related_cfg_item_s_fields[] = {
    {offsetof(gnb_cu_up_cell_group_related_cfg_item_s, cell_group_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(gnb_cu_up_cell_group_related_cfg_item_s, up_tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(gnb_cu_up_cell_group_related_cfg_item_s, ul_cfg),
     (int32_t)offsetof(gnb_cu_up_cell_group_related_cfg_item_s, ul_cfg_present),
     asn1::table::enum_op<decltype(gnb_cu_up_cell_group_related_cfg_item_s::ul_cfg)>(),
     decltype(gnb_cu_up_cell_group_related_cfg_item_s::ul_cfg)::has_ext,
     false,
     decltype(gnb_cu_up_cell_group_related_cfg_item_s::ul_cfg)::nof_types,
     decltype(gnb_cu_up_cell_group_related_cfg_item_s::ul_cfg)::nof_exts,
     nullptr,
     &ul_cfg_opts::desc},
    {offsetof(gnb_cu_up_cell_group_related_cfg_item_s, ie_exts),
     (int32_t)offsetof(gnb_cu_up_cell_group_related_cfg_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gnb_cu_up_cell_group_related_cfg_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_gnb_cu_up_cell_group_related_cfg_item_s_names[] = {
    {"cell-Group-ID", nullptr},
    {"uP-TNL-Information", nullptr},
    {"uL-Configuration", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_gnb_cu_up_cell_group_related_cfg_item_s_opts[] = {
    {2, (int32_t)offsetof(gnb_cu_up_cell_group_related_cfg_item_s, ul_cfg_present)},
    {3, (int32_t)offsetof(gnb_cu_up_cell_group_related_cfg_item_s, ie_exts_present)}};
const asn1::table::seq_desc gnb_cu_up_cell_group_related_cfg_item_s::desc = {
    {seq_table_gnb_cu_up_cell_group_related_cfg_item_s_fields,
     4,
     UINT64_C(0x3),
     seq_table_gnb_cu_up_cell_group_related_cfg_item_s_opts,
     2},
    false,
    true,
    nullptr,
    0,
    seq_table_gnb_cu_up_cell_group_related_cfg_item_s_names};

// DRB-Required-To-Modify-Item-EUTRAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_required_to_modify_item_eutran_s_fields[] = {
    {offsetof(drb_required_to_modify_item_eutran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_required_to_modify_item_eutran_s, s1_dl_up_tnl_info),
     (int32_t)offsetof(drb_required_to_modify_item_eutran_s, s1_dl_up_tnl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(drb_required_to_modify_item_eutran_s, gnb_cu_up_cell_group_related_cfg),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(drb_required_to_modify_item_eutran_s::gnb_cu_up_cell_group_related_cfg),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &gnb_cu_up_cell_group_related_cfg_item_s::desc},
    {offsetof(drb_required_to_modify_item_eutran_s, cause),
     (int32_t)offsetof(drb_required_to_modify_item_eutran_s, cause_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(drb_required_to_modify_item_eutran_s, ie_exts),
     (int32_t)offsetof(drb_required_to_modify_item_eutran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_required_to_modify_item_eutran_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_required_to_modify_item_eutran_s_names[] = {
    {"dRB-ID", nullptr},
    {"s1-DL-UP-TNL-Information", nullptr},
    {"gNB-CU-UP-CellGroupRelatedConfiguration", nullptr},
    {"cause", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_required_to_modify_item_eutran_s_opts[] = {
    {1, (int32_t)offsetof(drb_required_to_modify_item_eutran_s, s1_dl_up_tnl_info_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(drb_required_to_modify_item_eutran_s, cause_present)},
    {4, (int32_t)offsetof(drb_required_to_modify_item_eutran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_required_to_modify_item_eutran_s::desc = {
    {seq_table_drb_required_to_modify_item_eutran_s_fields,
     5,
     UINT64_C(0x1),
     seq_table_drb_required_to_modify_item_eutran_s_opts,
     4},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_required_to_modify_item_eutran_s_names};

// DRB-Required-To-Modify-Item-NG-RAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_required_to_modify_item_ng_ran_s_fields[] = {
    {offsetof(drb_required_to_modify_item_ng_ran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_required_to_modify_item_ng_ran_s, gnb_cu_up_cell_group_related_cfg),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(drb_required_to_modify_item_ng_ran_s::gnb_cu_up_cell_group_related_cfg),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &gnb_cu_up_cell_group_related_cfg_item_s::desc},
    {offsetof(drb_required_to_modify_item_ng_ran_s, flow_to_rem),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(drb_required_to_modify_item_ng_ran_s::flow_to_rem),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_item_s::desc},
    {offsetof(drb_required_to_modify_item_ng_ran_s, cause),
     (int32_t)offsetof(drb_required_to_modify_item_ng_ran_s, cause_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(drb_required_to_modify_item_ng_ran_s, ie_exts),
     (int32_t)offsetof(drb_required_to_modify_item_ng_ran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_required_to_modify_item_ng_ran_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_required_to_modify_item_ng_ran_s_names[] = {
    {"dRB-ID", nullptr},
    {"gNB-CU-UP-CellGroupRelatedConfiguration", nullptr},
    {"flow-To-Remove", nullptr},
    {"cause", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_required_to_modify_item_ng_ran_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(drb_required_to_modify_item_ng_ran_s, cause_present)},
    {4, (int32_t)offsetof(drb_required_to_modify_item_ng_ran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_required_to_modify_item_ng_ran_s::desc = {
    {seq_table_drb_required_to_modify_item_ng_ran_s_fields,
     5,
     UINT64_C(0x1),
     seq_table_drb_required_to_modify_item_ng_ran_s_opts,
     4},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_required_to_modify_item_ng_ran_s_names};

// DRB-Required-To-Remove-Item-EUTRAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_required_to_rem_item_eutran_s_fields[] = {
    {offsetof(drb_required_to_rem_item_eutran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_required_to_rem_item_eutran_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(drb_required_to_rem_item_eutran_s, ie_exts),
     (int32_t)offsetof(drb_required_to_rem_item_eutran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_required_to_rem_item_eutran_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_required_to_rem_item_eutran_s_names[] = {{"dRB-ID", nullptr},
                                                                                            {"cause", nullptr},
                                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_drb_required_to_rem_item_eutran_s_opts[]  = {
    {2, (int32_t)offsetof(drb_required_to_rem_item_eutran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_required_to_rem_item_eutran_s::desc = {
    {seq_table_drb_required_to_rem_item_eutran_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_drb_required_to_rem_item_eutran_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_required_to_rem_item_eutran_s_names};

// DRB-Required-To-Remove-Item-NG-RAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_required_to_rem_item_ng_ran_s_fields[] = {
    {offsetof(drb_required_to_rem_item_ng_ran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_required_to_rem_item_ng_ran_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(drb_required_to_rem_item_ng_ran_s, ie_exts),
     (int32_t)offsetof(drb_required_to_rem_item_ng_ran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_required_to_rem_item_ng_ran_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_required_to_rem_item_ng_ran_s_names[] = {{"dRB-ID", nullptr},
                                                                                            {"cause", nullptr},
                                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_drb_required_to_rem_item_ng_ran_s_opts[]  = {
    {2, (int32_t)offsetof(drb_required_to_rem_item_ng_ran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_required_to_rem_item_ng_ran_s::desc = {
    {seq_table_drb_required_to_rem_item_ng_ran_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_drb_required_to_rem_item_ng_ran_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_required_to_rem_item_ng_ran_s_names};

// DataForwardingtoNG-RANQoSFlowInformationList-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_data_forwardingto_ng_ran_qos_flow_info_list_item_s_fields[] = {
    {offsetof(data_forwardingto_ng_ran_qos_flow_info_list_item_s, qos_flow_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(data_forwardingto_ng_ran_qos_flow_info_list_item_s, ie_exts),
     (int32_t)offsetof(data_forwardingto_ng_ran_qos_flow_info_list_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(data_forwardingto_ng_ran_qos_flow_info_list_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_data_forwardingto_ng_ran_qos_flow_info_list_item_s_names[] = {
    {"qoS-Flow-Identifier", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_data_forwardingto_ng_ran_qos_flow_info_list_item_s_opts[] = {
    {1, (int32_t)offsetof(data_forwardingto_ng_ran_qos_flow_info_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc data_forwardingto_ng_ran_qos_flow_info_list_item_s::desc = {
    {seq_table_data_forwardingto_ng_ran_qos_flow_info_list_item_s_fields,
     2,
     UINT64_C(0x1),
     seq_table_data_forwardingto_ng_ran_qos_flow_info_list_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_data_forwardingto_ng_ran_qos_flow_info_list_item_s_names};

// PDUSetbasedHandlingIndicator ::= ENUMERATED
static const char*           enum_names_pdu_setbased_handling_ind_e_values[] = {"supported"};
const asn1::table::enum_desc pdu_setbased_handling_ind_opts::desc = {enum_names_pdu_setbased_handling_ind_e_values,
                                                                     1,
                                                                     "pdu_setbased_handling_ind_e"};

static const uint32_t objset_id_data_forwarding_info_ext_ies_o[]   = {136, 205};
static const uint8_t  objset_crit_data_forwarding_info_ext_ies_o[] = {crit_e::ignore, crit_e::ignore};

// Data-Forwarding-Information-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_data_forwarding_info_ext_ies_container_values[] = {
    {offsetof(data_forwarding_info_ext_ies_container, data_forwardingto_ng_ran_qos_flow_info_list),
     (int32_t)offsetof(data_forwarding_info_ext_ies_container, data_forwardingto_ng_ran_qos_flow_info_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<
         decltype(data_forwarding_info_ext_ies_container::data_forwardingto_ng_ran_qos_flow_info_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &data_forwardingto_ng_ran_qos_flow_info_list_item_s::desc},
    {offsetof(data_forwarding_info_ext_ies_container, pdu_setbased_handling_ind),
     (int32_t)offsetof(data_forwarding_info_ext_ies_container, pdu_setbased_handling_ind_present),
     asn1::table::enum_op<decltype(data_forwarding_info_ext_ies_container::pdu_setbased_handling_ind)>(),
     decltype(data_forwarding_info_ext_ies_container::pdu_setbased_handling_ind)::has_ext,
     false,
     decltype(data_forwarding_info_ext_ies_container::pdu_setbased_handling_ind)::nof_types,
     decltype(data_forwarding_info_ext_ies_container::pdu_setbased_handling_ind)::nof_exts,
     nullptr,
     &pdu_setbased_handling_ind_opts::desc}};
static const int32_t ie_table_data_forwarding_info_ext_ies_container_presences[] = {
    (int32_t)offsetof(data_forwarding_info_ext_ies_container, data_forwardingto_ng_ran_qos_flow_info_list_present),
    (int32_t)offsetof(data_forwarding_info_ext_ies_container, pdu_setbased_handling_ind_present)};
const asn1::table::ie_container_desc data_forwarding_info_ext_ies_container::desc = {
    objset_id_data_forwarding_info_ext_ies_o,
    objset_crit_data_forwarding_info_ext_ies_o,
    ie_table_data_forwarding_info_ext_ies_container_values,
    ie_table_data_forwarding_info_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// Data-Forwarding-Information ::= SEQUENCE
static const asn1::table::seq_field seq_table_data_forwarding_info_s_fields[] = {
    {offsetof(data_forwarding_info_s, ul_data_forwarding),
     (int32_t)offsetof(data_forwarding_info_s, ul_data_forwarding_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(data_forwarding_info_s, dl_data_forwarding),
     (int32_t)offsetof(data_forwarding_info_s, dl_data_forwarding_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(data_forwarding_info_s, ie_exts),
     (int32_t)offsetof(data_forwarding_info_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_data_forwarding_info_s_names[] = {{"uL-Data-Forwarding", nullptr},
                                                                                 {"dL-Data-Forwarding", nullptr},
                                                                                 {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_data_forwarding_info_s_opts[]  = {
    {0, (int32_t)offsetof(data_forwarding_info_s, ul_data_forwarding_present)},
    {1, (int32_t)offsetof(data_forwarding_info_s, dl_data_forwarding_present)},
    {2, (int32_t)offsetof(data_forwarding_info_s, ie_exts_present)}};
const asn1::table::seq_desc data_forwarding_info_s::desc = {
    {seq_table_data_forwarding_info_s_fields, 3, UINT64_C(0x0), seq_table_data_forwarding_info_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_data_forwarding_info_s_names};

// IntegrityProtectionResult ::= ENUMERATED
static const char*           enum_names_integrity_protection_result_e_values[] = {"performed", "not-performed"};
const asn1::table::enum_desc integrity_protection_result_opts::desc = {enum_names_integrity_protection_result_e_values,
                                                                       2,
                                                                       "integrity_protection_result_e"};

// SecurityResult ::= SEQUENCE
static const asn1::table::seq_field seq_table_security_result_s_fields[] = {
    {offsetof(security_result_s, integrity_protection_result),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(security_result_s::integrity_protection_result)>(),
     decltype(security_result_s::integrity_protection_result)::has_ext,
     false,
     decltype(security_result_s::integrity_protection_result)::nof_types,
     decltype(security_result_s::integrity_protection_result)::nof_exts,
     nullptr,
     &integrity_protection_result_opts::desc},
    {offsetof(security_result_s, confidentiality_protection_result),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(security_result_s::confidentiality_protection_result)>(),
     decltype(security_result_s::confidentiality_protection_result)::has_ext,
     false,
     decltype(security_result_s::confidentiality_protection_result)::nof_types,
     decltype(security_result_s::confidentiality_protection_result)::nof_exts,
     nullptr,
     &confidentiality_protection_result_opts::desc},
    {offsetof(security_result_s, ie_exts),
     (int32_t)offsetof(security_result_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(security_result_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_security_result_s_names[] = {
    {"integrityProtectionResult", nullptr},
    {"confidentialityProtectionResult", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_security_result_s_opts[] = {
    {2, (int32_t)offsetof(security_result_s, ie_exts_present)}};
const asn1::table::seq_desc security_result_s::desc = {
    {seq_table_security_result_s_fields, 3, UINT64_C(0x3), seq_table_security_result_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_security_result_s_names};

static const uint32_t objset_id_drb_setup_item_eutran_ext_ies_o[]   = {142, 173};
static const uint8_t  objset_crit_drb_setup_item_eutran_ext_ies_o[] = {crit_e::ignore, crit_e::ignore};

// DRB-Setup-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_drb_setup_item_eutran_ext_ies_container_values[] = {
    {offsetof(drb_setup_item_eutran_ext_ies_container, data_forwarding_source_ip_address),
     (int32_t)offsetof(drb_setup_item_eutran_ext_ies_container, data_forwarding_source_ip_address_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_setup_item_eutran_ext_ies_container::data_forwarding_source_ip_address),
                                asn1::table::struct_json>::value},
    {offsetof(drb_setup_item_eutran_ext_ies_container, security_result),
     (int32_t)offsetof(drb_setup_item_eutran_ext_ies_container, security_result_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_result_s::desc}};
static const int32_t ie_table_drb_setup_item_eutran_ext_ies_container_presences[] = {
    (int32_t)offsetof(drb_setup_item_eutran_ext_ies_container, data_forwarding_source_ip_address_present),
    (int32_t)offsetof(drb_setup_item_eutran_ext_ies_container, security_result_present)};
const asn1::table::ie_container_desc drb_setup_item_eutran_ext_ies_container::desc = {
    objset_id_drb_setup_item_eutran_ext_ies_o,
    objset_crit_drb_setup_item_eutran_ext_ies_o,
    ie_table_drb_setup_item_eutran_ext_ies_container_values,
    ie_table_drb_setup_item_eutran_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// DRB-Setup-Item-EUTRAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_setup_item_eutran_s_fields[] = {
    {offsetof(drb_setup_item_eutran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_setup_item_eutran_s, s1_dl_up_tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(drb_setup_item_eutran_s, data_forwarding_info_resp),
     (int32_t)offsetof(drb_setup_item_eutran_s, data_forwarding_info_resp_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_s::desc},
    {offsetof(drb_setup_item_eutran_s, ul_up_transport_params),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(drb_setup_item_eutran_s::ul_up_transport_params),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &up_params_item_s::desc},
    {offsetof(drb_setup_item_eutran_s, s1_dl_up_unchanged),
     (int32_t)offsetof(drb_setup_item_eutran_s, s1_dl_up_unchanged_present),
     asn1::table::enum_op<decltype(drb_setup_item_eutran_s::s1_dl_up_unchanged)>(),
     decltype(drb_setup_item_eutran_s::s1_dl_up_unchanged)::has_ext,
     false,
     decltype(drb_setup_item_eutran_s::s1_dl_up_unchanged)::nof_types,
     decltype(drb_setup_item_eutran_s::s1_dl_up_unchanged)::nof_exts,
     nullptr,
     &drb_setup_item_eutran_s::s1_dl_up_unchanged_opts::desc},
    {offsetof(drb_setup_item_eutran_s, ie_exts),
     (int32_t)offsetof(drb_setup_item_eutran_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &drb_setup_item_eutran_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_drb_setup_item_eutran_s_names[] = {
    {"dRB-ID", nullptr},
    {"s1-DL-UP-TNL-Information", nullptr},
    {"data-Forwarding-Information-Response", nullptr},
    {"uL-UP-Transport-Parameters", nullptr},
    {"s1-DL-UP-Unchanged", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_setup_item_eutran_s_opts[] = {
    {2, (int32_t)offsetof(drb_setup_item_eutran_s, data_forwarding_info_resp_present)},
    {4, (int32_t)offsetof(drb_setup_item_eutran_s, s1_dl_up_unchanged_present)},
    {5, (int32_t)offsetof(drb_setup_item_eutran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_setup_item_eutran_s::desc = {
    {seq_table_drb_setup_item_eutran_s_fields, 6, UINT64_C(0xb), seq_table_drb_setup_item_eutran_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_setup_item_eutran_s_names};

static const char*           enum_names_drb_setup_item_eutran_s_s1_dl_up_unchanged_e__values[] = {"true"};
const asn1::table::enum_desc drb_setup_item_eutran_s::s1_dl_up_unchanged_opts::desc            = {
    enum_names_drb_setup_item_eutran_s_s1_dl_up_unchanged_e__values,
    1,
    "drb_setup_item_eutran_s::s1_dl_up_unchanged_e_"};

// DRB-Setup-Item-NG-RAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_setup_item_ng_ran_s_fields[] = {
    {offsetof(drb_setup_item_ng_ran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_setup_item_ng_ran_s, drb_data_forwarding_info_resp),
     (int32_t)offsetof(drb_setup_item_ng_ran_s, drb_data_forwarding_info_resp_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_s::desc},
    {offsetof(drb_setup_item_ng_ran_s, ul_up_transport_params),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(drb_setup_item_ng_ran_s::ul_up_transport_params),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &up_params_item_s::desc},
    {offsetof(drb_setup_item_ng_ran_s, flow_setup_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(drb_setup_item_ng_ran_s::flow_setup_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_item_s::desc},
    {offsetof(drb_setup_item_ng_ran_s, flow_failed_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(drb_setup_item_ng_ran_s::flow_failed_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_failed_item_s::desc},
    {offsetof(drb_setup_item_ng_ran_s, ie_exts),
     (int32_t)offsetof(drb_setup_item_ng_ran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_setup_item_ng_ran_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_setup_item_ng_ran_s_names[] = {
    {"dRB-ID", nullptr},
    {"dRB-data-Forwarding-Information-Response", nullptr},
    {"uL-UP-Transport-Parameters", nullptr},
    {"flow-Setup-List", nullptr},
    {"flow-Failed-List", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_setup_item_ng_ran_s_opts[] = {
    {1, (int32_t)offsetof(drb_setup_item_ng_ran_s, drb_data_forwarding_info_resp_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(drb_setup_item_ng_ran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_setup_item_ng_ran_s::desc = {
    {seq_table_drb_setup_item_ng_ran_s_fields, 6, UINT64_C(0xd), seq_table_drb_setup_item_ng_ran_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_setup_item_ng_ran_s_names};

static const uint32_t objset_id_drb_setup_mod_item_eutran_ext_ies_o[]   = {173, 142};
static const uint8_t  objset_crit_drb_setup_mod_item_eutran_ext_ies_o[] = {crit_e::ignore, crit_e::ignore};

// DRB-Setup-Mod-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_drb_setup_mod_item_eutran_ext_ies_container_values[] = {
    {offsetof(drb_setup_mod_item_eutran_ext_ies_container, security_result),
     (int32_t)offsetof(drb_setup_mod_item_eutran_ext_ies_container, security_result_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_result_s::desc},
    {offsetof(drb_setup_mod_item_eutran_ext_ies_container, data_forwarding_source_ip_address),
     (int32_t)offsetof(drb_setup_mod_item_eutran_ext_ies_container, data_forwarding_source_ip_address_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<
         decltype(drb_setup_mod_item_eutran_ext_ies_container::data_forwarding_source_ip_address),
         asn1::table::struct_json>::value}};
static const int32_t ie_table_drb_setup_mod_item_eutran_ext_ies_container_presences[] = {
    (int32_t)offsetof(drb_setup_mod_item_eutran_ext_ies_container, security_result_present),
    (int32_t)offsetof(drb_setup_mod_item_eutran_ext_ies_container, data_forwarding_source_ip_address_present)};
const asn1::table::ie_container_desc drb_setup_mod_item_eutran_ext_ies_container::desc = {
    objset_id_drb_setup_mod_item_eutran_ext_ies_o,
    objset_crit_drb_setup_mod_item_eutran_ext_ies_o,
    ie_table_drb_setup_mod_item_eutran_ext_ies_container_values,
    ie_table_drb_setup_mod_item_eutran_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// DRB-Setup-Mod-Item-EUTRAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_setup_mod_item_eutran_s_fields[] = {
    {offsetof(drb_setup_mod_item_eutran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_setup_mod_item_eutran_s, s1_dl_up_tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(drb_setup_mod_item_eutran_s, data_forwarding_info_resp),
     (int32_t)offsetof(drb_setup_mod_item_eutran_s, data_forwarding_info_resp_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_s::desc},
    {offsetof(drb_setup_mod_item_eutran_s, ul_up_transport_params),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(drb_setup_mod_item_eutran_s::ul_up_transport_params),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &up_params_item_s::desc},
    {offsetof(drb_setup_mod_item_eutran_s, ie_exts),
     (int32_t)offsetof(drb_setup_mod_item_eutran_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &drb_setup_mod_item_eutran_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_drb_setup_mod_item_eutran_s_names[] = {
    {"dRB-ID", nullptr},
    {"s1-DL-UP-TNL-Information", nullptr},
    {"data-Forwarding-Information-Response", nullptr},
    {"uL-UP-Transport-Parameters", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_setup_mod_item_eutran_s_opts[] = {
    {2, (int32_t)offsetof(drb_setup_mod_item_eutran_s, data_forwarding_info_resp_present)},
    {4, (int32_t)offsetof(drb_setup_mod_item_eutran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_setup_mod_item_eutran_s::desc = {
    {seq_table_drb_setup_mod_item_eutran_s_fields, 5, UINT64_C(0xb), seq_table_drb_setup_mod_item_eutran_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_setup_mod_item_eutran_s_names};

// DRB-Setup-Mod-Item-NG-RAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_setup_mod_item_ng_ran_s_fields[] = {
    {offsetof(drb_setup_mod_item_ng_ran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_setup_mod_item_ng_ran_s, drb_data_forwarding_info_resp),
     (int32_t)offsetof(drb_setup_mod_item_ng_ran_s, drb_data_forwarding_info_resp_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_s::desc},
    {offsetof(drb_setup_mod_item_ng_ran_s, ul_up_transport_params),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(drb_setup_mod_item_ng_ran_s::ul_up_transport_params),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &up_params_item_s::desc},
    {offsetof(drb_setup_mod_item_ng_ran_s, flow_setup_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(drb_setup_mod_item_ng_ran_s::flow_setup_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_item_s::desc},
    {offsetof(drb_setup_mod_item_ng_ran_s, flow_failed_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(drb_setup_mod_item_ng_ran_s::flow_failed_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_failed_item_s::desc},
    {offsetof(drb_setup_mod_item_ng_ran_s, ie_exts),
     (int32_t)offsetof(drb_setup_mod_item_ng_ran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_setup_mod_item_ng_ran_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_setup_mod_item_ng_ran_s_names[] = {
    {"dRB-ID", nullptr},
    {"dRB-data-Forwarding-Information-Response", nullptr},
    {"uL-UP-Transport-Parameters", nullptr},
    {"flow-Setup-List", nullptr},
    {"flow-Failed-List", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_setup_mod_item_ng_ran_s_opts[] = {
    {1, (int32_t)offsetof(drb_setup_mod_item_ng_ran_s, drb_data_forwarding_info_resp_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(drb_setup_mod_item_ng_ran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_setup_mod_item_ng_ran_s::desc = {
    {seq_table_drb_setup_mod_item_ng_ran_s_fields, 6, UINT64_C(0xd), seq_table_drb_setup_mod_item_ng_ran_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_setup_mod_item_ng_ran_s_names};

// DRB-Status-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_status_item_s_fields[] = {
    {offsetof(drb_status_item_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_status_item_s, pdcp_dl_count),
     (int32_t)offsetof(drb_status_item_s, pdcp_dl_count_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_count_s::desc},
    {offsetof(drb_status_item_s, pdcp_ul_count),
     (int32_t)offsetof(drb_status_item_s, pdcp_ul_count_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_count_s::desc},
    {offsetof(drb_status_item_s, ie_exts),
     (int32_t)offsetof(drb_status_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_status_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_status_item_s_names[] = {{"dRB-ID", nullptr},
                                                                            {"pDCP-DL-Count", nullptr},
                                                                            {"pDCP-UL-Count", nullptr},
                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_drb_status_item_s_opts[]  = {
    {1, (int32_t)offsetof(drb_status_item_s, pdcp_dl_count_present)},
    {2, (int32_t)offsetof(drb_status_item_s, pdcp_ul_count_present)},
    {3, (int32_t)offsetof(drb_status_item_s, ie_exts_present)}};
const asn1::table::seq_desc drb_status_item_s::desc = {
    {seq_table_drb_status_item_s_fields, 4, UINT64_C(0x1), seq_table_drb_status_item_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_status_item_s_names};

// EUTRANAllocationAndRetentionPriority ::= SEQUENCE
static const asn1::table::seq_field seq_table_eutran_alloc_and_retention_prio_s_fields[] = {
    {offsetof(eutran_alloc_and_retention_prio_s, prio_level),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(15),
     nullptr},
    {offsetof(eutran_alloc_and_retention_prio_s, pre_emption_cap),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(eutran_alloc_and_retention_prio_s::pre_emption_cap)>(),
     decltype(eutran_alloc_and_retention_prio_s::pre_emption_cap)::has_ext,
     false,
     decltype(eutran_alloc_and_retention_prio_s::pre_emption_cap)::nof_types,
     decltype(eutran_alloc_and_retention_prio_s::pre_emption_cap)::nof_exts,
     nullptr,
     &pre_emption_cap_opts::desc},
    {offsetof(eutran_alloc_and_retention_prio_s, pre_emption_vulnerability),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(eutran_alloc_and_retention_prio_s::pre_emption_vulnerability)>(),
     decltype(eutran_alloc_and_retention_prio_s::pre_emption_vulnerability)::has_ext,
     false,
     decltype(eutran_alloc_and_retention_prio_s::pre_emption_vulnerability)::nof_types,
     decltype(eutran_alloc_and_retention_prio_s::pre_emption_vulnerability)::nof_exts,
     nullptr,
     &pre_emption_vulnerability_opts::desc},
    {offsetof(eutran_alloc_and_retention_prio_s, ie_exts),
     (int32_t)offsetof(eutran_alloc_and_retention_prio_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(eutran_alloc_and_retention_prio_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_eutran_alloc_and_retention_prio_s_names[] = {
    {"priorityLevel", nullptr},
    {"pre-emptionCapability", nullptr},
    {"pre-emptionVulnerability", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_eutran_alloc_and_retention_prio_s_opts[] = {
    {3, (int32_t)offsetof(eutran_alloc_and_retention_prio_s, ie_exts_present)}};
const asn1::table::seq_desc eutran_alloc_and_retention_prio_s::desc = {
    {seq_table_eutran_alloc_and_retention_prio_s_fields,
     4,
     UINT64_C(0x7),
     seq_table_eutran_alloc_and_retention_prio_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_eutran_alloc_and_retention_prio_s_names};

// GBR-QosInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_gbr_qos_info_s_fields[] = {
    {offsetof(gbr_qos_info_s, erab_max_bitrate_dl),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     INT64_C(0),
     UINT64_C(4000000000000),
     nullptr},
    {offsetof(gbr_qos_info_s, erab_max_bitrate_ul),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     INT64_C(0),
     UINT64_C(4000000000000),
     nullptr},
    {offsetof(gbr_qos_info_s, erab_guaranteed_bitrate_dl),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     INT64_C(0),
     UINT64_C(4000000000000),
     nullptr},
    {offsetof(gbr_qos_info_s, erab_guaranteed_bitrate_ul),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     INT64_C(0),
     UINT64_C(4000000000000),
     nullptr},
    {offsetof(gbr_qos_info_s, ie_exts),
     (int32_t)offsetof(gbr_qos_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gbr_qos_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_gbr_qos_info_s_names[] = {{"e-RAB-MaximumBitrateDL", nullptr},
                                                                         {"e-RAB-MaximumBitrateUL", nullptr},
                                                                         {"e-RAB-GuaranteedBitrateDL", nullptr},
                                                                         {"e-RAB-GuaranteedBitrateUL", nullptr},
                                                                         {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_gbr_qos_info_s_opts[]  = {
    {4, (int32_t)offsetof(gbr_qos_info_s, ie_exts_present)}};
const asn1::table::seq_desc gbr_qos_info_s::desc = {
    {seq_table_gbr_qos_info_s_fields, 5, UINT64_C(0xf), seq_table_gbr_qos_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_gbr_qos_info_s_names};

// EUTRAN-QoS ::= SEQUENCE
static const asn1::table::seq_field seq_table_eutran_qos_s_fields[] = {
    {offsetof(eutran_qos_s, qci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(eutran_qos_s, eutran_alloc_and_retention_prio),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &eutran_alloc_and_retention_prio_s::desc},
    {offsetof(eutran_qos_s, gbr_qos_info),
     (int32_t)offsetof(eutran_qos_s, gbr_qos_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gbr_qos_info_s::desc},
    {offsetof(eutran_qos_s, ie_exts),
     (int32_t)offsetof(eutran_qos_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(eutran_qos_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_eutran_qos_s_names[] = {
    {"qCI", nullptr},
    {"eUTRANallocationAndRetentionPriority", nullptr},
    {"gbrQosInformation", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_eutran_qos_s_opts[] = {
    {2, (int32_t)offsetof(eutran_qos_s, gbr_qos_info_present)},
    {3, (int32_t)offsetof(eutran_qos_s, ie_exts_present)}};
const asn1::table::seq_desc eutran_qos_s::desc = {
    {seq_table_eutran_qos_s_fields, 4, UINT64_C(0x3), seq_table_eutran_qos_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_eutran_qos_s_names};

// PDCP-SN-Status-Request ::= ENUMERATED
static const char*           enum_names_pdcp_sn_status_request_e_values[] = {"requested"};
const asn1::table::enum_desc pdcp_sn_status_request_opts::desc            = {enum_names_pdcp_sn_status_request_e_values,
                                                                             1,
                                                                             "pdcp_sn_status_request_e"};

// DRB-To-Modify-Item-EUTRAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_to_modify_item_eutran_s_fields[] = {
    {offsetof(drb_to_modify_item_eutran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_to_modify_item_eutran_s, pdcp_cfg),
     (int32_t)offsetof(drb_to_modify_item_eutran_s, pdcp_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_cfg_s::desc},
    {offsetof(drb_to_modify_item_eutran_s, eutran_qos),
     (int32_t)offsetof(drb_to_modify_item_eutran_s, eutran_qos_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &eutran_qos_s::desc},
    {offsetof(drb_to_modify_item_eutran_s, s1_ul_up_tnl_info),
     (int32_t)offsetof(drb_to_modify_item_eutran_s, s1_ul_up_tnl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(drb_to_modify_item_eutran_s, data_forwarding_info),
     (int32_t)offsetof(drb_to_modify_item_eutran_s, data_forwarding_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_s::desc},
    {offsetof(drb_to_modify_item_eutran_s, pdcp_sn_status_request),
     (int32_t)offsetof(drb_to_modify_item_eutran_s, pdcp_sn_status_request_present),
     asn1::table::enum_op<decltype(drb_to_modify_item_eutran_s::pdcp_sn_status_request)>(),
     decltype(drb_to_modify_item_eutran_s::pdcp_sn_status_request)::has_ext,
     false,
     decltype(drb_to_modify_item_eutran_s::pdcp_sn_status_request)::nof_types,
     decltype(drb_to_modify_item_eutran_s::pdcp_sn_status_request)::nof_exts,
     nullptr,
     &pdcp_sn_status_request_opts::desc},
    {offsetof(drb_to_modify_item_eutran_s, pdcp_sn_status_info),
     (int32_t)offsetof(drb_to_modify_item_eutran_s, pdcp_sn_status_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_sn_status_info_s::desc},
    {offsetof(drb_to_modify_item_eutran_s, dl_up_params),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(drb_to_modify_item_eutran_s::dl_up_params),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &up_params_item_s::desc},
    {offsetof(drb_to_modify_item_eutran_s, cell_group_to_add),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     4,
     &asn1::table::item_list_codec<decltype(drb_to_modify_item_eutran_s::cell_group_to_add),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_group_info_item_s::desc},
    {offsetof(drb_to_modify_item_eutran_s, cell_group_to_modify),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     4,
     &asn1::table::item_list_codec<decltype(drb_to_modify_item_eutran_s::cell_group_to_modify),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_group_info_item_s::desc},
    {offsetof(drb_to_modify_item_eutran_s, cell_group_to_rem),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     4,
     &asn1::table::item_list_codec<decltype(drb_to_modify_item_eutran_s::cell_group_to_rem),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_group_info_item_s::desc},
    {offsetof(drb_to_modify_item_eutran_s, drb_inactivity_timer),
     (int32_t)offsetof(drb_to_modify_item_eutran_s, drb_inactivity_timer_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(7200),
     nullptr},
    {offsetof(drb_to_modify_item_eutran_s, ie_exts),
     (int32_t)offsetof(drb_to_modify_item_eutran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_to_modify_item_eutran_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_to_modify_item_eutran_s_names[] = {
    {"dRB-ID", nullptr},
    {"pDCP-Configuration", nullptr},
    {"eUTRAN-QoS", nullptr},
    {"s1-UL-UP-TNL-Information", nullptr},
    {"data-Forwarding-Information", nullptr},
    {"pDCP-SN-Status-Request", nullptr},
    {"pDCP-SN-Status-Information", nullptr},
    {"dL-UP-Parameters", nullptr},
    {"cell-Group-To-Add", nullptr},
    {"cell-Group-To-Modify", nullptr},
    {"cell-Group-To-Remove", nullptr},
    {"dRB-Inactivity-Timer", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_to_modify_item_eutran_s_opts[] = {
    {1, (int32_t)offsetof(drb_to_modify_item_eutran_s, pdcp_cfg_present)},
    {2, (int32_t)offsetof(drb_to_modify_item_eutran_s, eutran_qos_present)},
    {3, (int32_t)offsetof(drb_to_modify_item_eutran_s, s1_ul_up_tnl_info_present)},
    {4, (int32_t)offsetof(drb_to_modify_item_eutran_s, data_forwarding_info_present)},
    {5, (int32_t)offsetof(drb_to_modify_item_eutran_s, pdcp_sn_status_request_present)},
    {6, (int32_t)offsetof(drb_to_modify_item_eutran_s, pdcp_sn_status_info_present)},
    {7, asn1::table::seq_field::list_presence},
    {8, asn1::table::seq_field::list_presence},
    {9, asn1::table::seq_field::list_presence},
    {10, asn1::table::seq_field::list_presence},
    {11, (int32_t)offsetof(drb_to_modify_item_eutran_s, drb_inactivity_timer_present)},
    {12, (int32_t)offsetof(drb_to_modify_item_eutran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_to_modify_item_eutran_s::desc = {
    {seq_table_drb_to_modify_item_eutran_s_fields, 13, UINT64_C(0x1), seq_table_drb_to_modify_item_eutran_s_opts, 12},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_to_modify_item_eutran_s_names};

// DefaultDRB ::= ENUMERATED
static const char*           enum_names_default_drb_e_values[] = {"true", "false"};
const asn1::table::enum_desc default_drb_opts::desc            = {enum_names_default_drb_e_values, 2, "default_drb_e"};

// SDAP-Header-UL ::= ENUMERATED
static const char*           enum_names_sdap_hdr_ul_e_values[] = {"present", "absent"};
const asn1::table::enum_desc sdap_hdr_ul_opts::desc            = {enum_names_sdap_hdr_ul_e_values, 2, "sdap_hdr_ul_e"};

// SDAP-Header-DL ::= ENUMERATED
static const char*           enum_names_sdap_hdr_dl_e_values[] = {"present", "absent"};
const asn1::table::enum_desc sdap_hdr_dl_opts::desc            = {enum_names_sdap_hdr_dl_e_values, 2, "sdap_hdr_dl_e"};

// SDAP-Configuration ::= SEQUENCE
static const asn1::table::seq_field seq_table_sdap_cfg_s_fields[] = {
    {offsetof(sdap_cfg_s, default_drb),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sdap_cfg_s::default_drb)>(),
     decltype(sdap_cfg_s::default_drb)::has_ext,
     false,
     decltype(sdap_cfg_s::default_drb)::nof_types,
     decltype(sdap_cfg_s::default_drb)::nof_exts,
     nullptr,
     &default_drb_opts::desc},
    {offsetof(sdap_cfg_s, sdap_hdr_ul),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sdap_cfg_s::sdap_hdr_ul)>(),
     decltype(sdap_cfg_s::sdap_hdr_ul)::has_ext,
     false,
     decltype(sdap_cfg_s::sdap_hdr_ul)::nof_types,
     decltype(sdap_cfg_s::sdap_hdr_ul)::nof_exts,
     nullptr,
     &sdap_hdr_ul_opts::desc},
    {offsetof(sdap_cfg_s, sdap_hdr_dl),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sdap_cfg_s::sdap_hdr_dl)>(),
     decltype(sdap_cfg_s::sdap_hdr_dl)::has_ext,
     false,
     decltype(sdap_cfg_s::sdap_hdr_dl)::nof_types,
     decltype(sdap_cfg_s::sdap_hdr_dl)::nof_exts,
     nullptr,
     &sdap_hdr_dl_opts::desc},
    {offsetof(sdap_cfg_s, ie_exts),
     (int32_t)offsetof(sdap_cfg_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sdap_cfg_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_sdap_cfg_s_names[] = {{"defaultDRB", nullptr},
                                                                     {"sDAP-Header-UL", nullptr},
                                                                     {"sDAP-Header-DL", nullptr},
                                                                     {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_sdap_cfg_s_opts[] = {{3, (int32_t)offsetof(sdap_cfg_s, ie_exts_present)}};
const asn1::table::seq_desc       sdap_cfg_s::desc            = {
    {seq_table_sdap_cfg_s_fields, 4, UINT64_C(0x7), seq_table_sdap_cfg_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_sdap_cfg_s_names};

// EarlyForwardingCOUNTReq ::= ENUMERATED
static const char*           enum_names_early_forwarding_count_req_e_values[] = {"first-dl-count", "dl-discarding"};
const asn1::table::enum_desc early_forwarding_count_req_opts::desc = {enum_names_early_forwarding_count_req_e_values,
                                                                      2,
                                                                      "early_forwarding_count_req_e"};

// EarlyDataForwardingIndicator ::= ENUMERATED
static const char*           enum_names_early_data_forwarding_ind_e_values[] = {"stop"};
const asn1::table::enum_desc early_data_forwarding_ind_opts::desc = {enum_names_early_data_forwarding_ind_e_values,
                                                                     1,
                                                                     "early_data_forwarding_ind_e"};

// SDTindicatorMod ::= ENUMERATED
static const char*           enum_names_sd_tind_mod_e_values[] = {"true", "false"};
const asn1::table::enum_desc sd_tind_mod_opts::desc            = {enum_names_sd_tind_mod_e_values, 2, "sd_tind_mod_e"};

// PDCP-COUNT-Reset ::= ENUMERATED
static const char*           enum_names_pdcp_count_reset_e_values[] = {"true"};
const asn1::table::enum_desc pdcp_count_reset_opts::desc            = {enum_names_pdcp_count_reset_e_values,
                                                                       1,
                                                                       "pdcp_count_reset_e"};

static const uint32_t objset_id_drb_to_modify_item_ng_ran_ext_ies_o[]   = {71, 72, 122, 123, 120, 140, 176, 184};
static const uint8_t  objset_crit_drb_to_modify_item_ng_ran_ext_ies_o[] = {crit_e::reject,
                                                                           crit_e::ignore,
                                                                           crit_e::reject,
                                                                           crit_e::reject,
                                                                           crit_e::ignore,
                                                                           crit_e::ignore,
                                                                           crit_e::reject,
                                                                           crit_e::reject};

// DRB-To-Modify-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_drb_to_modify_item_ng_ran_ext_ies_container_values[] = {
    {offsetof(drb_to_modify_item_ng_ran_ext_ies_container, old_qos_flow_map_ul_endmarkerexpected),
     (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, old_qos_flow_map_ul_endmarkerexpected_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<
         decltype(drb_to_modify_item_ng_ran_ext_ies_container::old_qos_flow_map_ul_endmarkerexpected),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &qos_flow_item_s::desc},
    {offsetof(drb_to_modify_item_ng_ran_ext_ies_container, drb_qos),
     (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, drb_qos_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &qos_flow_level_qos_params_s::desc},
    {offsetof(drb_to_modify_item_ng_ran_ext_ies_container, early_forwarding_count_req),
     (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, early_forwarding_count_req_present),
     asn1::table::enum_op<decltype(drb_to_modify_item_ng_ran_ext_ies_container::early_forwarding_count_req)>(),
     decltype(drb_to_modify_item_ng_ran_ext_ies_container::early_forwarding_count_req)::has_ext,
     false,
     decltype(drb_to_modify_item_ng_ran_ext_ies_container::early_forwarding_count_req)::nof_types,
     decltype(drb_to_modify_item_ng_ran_ext_ies_container::early_forwarding_count_req)::nof_exts,
     nullptr,
     &early_forwarding_count_req_opts::desc},
    {offsetof(drb_to_modify_item_ng_ran_ext_ies_container, early_forwarding_count_info),
     (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, early_forwarding_count_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &early_forwarding_count_info_c::desc},
    {offsetof(drb_to_modify_item_ng_ran_ext_ies_container, daps_request_info),
     (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, daps_request_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &daps_request_info_s::desc},
    {offsetof(drb_to_modify_item_ng_ran_ext_ies_container, early_data_forwarding_ind),
     (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, early_data_forwarding_ind_present),
     asn1::table::enum_op<decltype(drb_to_modify_item_ng_ran_ext_ies_container::early_data_forwarding_ind)>(),
     decltype(drb_to_modify_item_ng_ran_ext_ies_container::early_data_forwarding_ind)::has_ext,
     false,
     decltype(drb_to_modify_item_ng_ran_ext_ies_container::early_data_forwarding_ind)::nof_types,
     decltype(drb_to_modify_item_ng_ran_ext_ies_container::early_data_forwarding_ind)::nof_exts,
     nullptr,
     &early_data_forwarding_ind_opts::desc},
    {offsetof(drb_to_modify_item_ng_ran_ext_ies_container, sd_tind_mod),
     (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, sd_tind_mod_present),
     asn1::table::enum_op<decltype(drb_to_modify_item_ng_ran_ext_ies_container::sd_tind_mod)>(),
     decltype(drb_to_modify_item_ng_ran_ext_ies_container::sd_tind_mod)::has_ext,
     false,
     decltype(drb_to_modify_item_ng_ran_ext_ies_container::sd_tind_mod)::nof_types,
     decltype(drb_to_modify_item_ng_ran_ext_ies_container::sd_tind_mod)::nof_exts,
     nullptr,
     &sd_tind_mod_opts::desc},
    {offsetof(drb_to_modify_item_ng_ran_ext_ies_container, pdcp_count_reset),
     (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, pdcp_count_reset_present),
     asn1::table::enum_op<decltype(drb_to_modify_item_ng_ran_ext_ies_container::pdcp_count_reset)>(),
     decltype(drb_to_modify_item_ng_ran_ext_ies_container::pdcp_count_reset)::has_ext,
     false,
     decltype(drb_to_modify_item_ng_ran_ext_ies_container::pdcp_count_reset)::nof_types,
     decltype(drb_to_modify_item_ng_ran_ext_ies_container::pdcp_count_reset)::nof_exts,
     nullptr,
     &pdcp_count_reset_opts::desc}};
static const int32_t ie_table_drb_to_modify_item_ng_ran_ext_ies_container_presences[] = {
    (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, old_qos_flow_map_ul_endmarkerexpected_present),
    (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, drb_qos_present),
    (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, early_forwarding_count_req_present),
    (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, early_forwarding_count_info_present),
    (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, daps_request_info_present),
    (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, early_data_forwarding_ind_present),
    (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, sd_tind_mod_present),
    (int32_t)offsetof(drb_to_modify_item_ng_ran_ext_ies_container, pdcp_count_reset_present)};
const asn1::table::ie_container_desc drb_to_modify_item_ng_ran_ext_ies_container::desc = {
    objset_id_drb_to_modify_item_ng_ran_ext_ies_o,
    objset_crit_drb_to_modify_item_ng_ran_ext_ies_o,
    ie_table_drb_to_modify_item_ng_ran_ext_ies_container_values,
    ie_table_drb_to_modify_item_ng_ran_ext_ies_container_presences,
    8,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// DRB-To-Modify-Item-NG-RAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_to_modify_item_ng_ran_s_fields[] = {
    {offsetof(drb_to_modify_item_ng_ran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_to_modify_item_ng_ran_s, sdap_cfg),
     (int32_t)offsetof(drb_to_modify_item_ng_ran_s, sdap_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sdap_cfg_s::desc},
    {offsetof(drb_to_modify_item_ng_ran_s, pdcp_cfg),
     (int32_t)offsetof(drb_to_modify_item_ng_ran_s, pdcp_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_cfg_s::desc},
    {offsetof(drb_to_modify_item_ng_ran_s, drb_data_forwarding_info),
     (int32_t)offsetof(drb_to_modify_item_ng_ran_s, drb_data_forwarding_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_s::desc},
    {offsetof(drb_to_modify_item_ng_ran_s, pdcp_sn_status_request),
     (int32_t)offsetof(drb_to_modify_item_ng_ran_s, pdcp_sn_status_request_present),
     asn1::table::enum_op<decltype(drb_to_modify_item_ng_ran_s::pdcp_sn_status_request)>(),
     decltype(drb_to_modify_item_ng_ran_s::pdcp_sn_status_request)::has_ext,
     false,
     decltype(drb_to_modify_item_ng_ran_s::pdcp_sn_status_request)::nof_types,
     decltype(drb_to_modify_item_ng_ran_s::pdcp_sn_status_request)::nof_exts,
     nullptr,
     &pdcp_sn_status_request_opts::desc},
    {offsetof(drb_to_modify_item_ng_ran_s, pdcp_sn_status_info),
     (int32_t)offsetof(drb_to_modify_item_ng_ran_s, pdcp_sn_status_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_sn_status_info_s::desc},
    {offsetof(drb_to_modify_item_ng_ran_s, dl_up_params),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(drb_to_modify_item_ng_ran_s::dl_up_params),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &up_params_item_s::desc},
    {offsetof(drb_to_modify_item_ng_ran_s, cell_group_to_add),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     4,
     &asn1::table::item_list_codec<decltype(drb_to_modify_item_ng_ran_s::cell_group_to_add),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_group_info_item_s::desc},
    {offsetof(drb_to_modify_item_ng_ran_s, cell_group_to_modify),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     4,
     &asn1::table::item_list_codec<decltype(drb_to_modify_item_ng_ran_s::cell_group_to_modify),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_group_info_item_s::desc},
    {offsetof(drb_to_modify_item_ng_ran_s, cell_group_to_rem),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     4,
     &asn1::table::item_list_codec<decltype(drb_to_modify_item_ng_ran_s::cell_group_to_rem),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_group_info_item_s::desc},
    {offsetof(drb_to_modify_item_ng_ran_s, flow_map_info),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(drb_to_modify_item_ng_ran_s::flow_map_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_qos_param_item_s::desc},
    {offsetof(drb_to_modify_item_ng_ran_s, drb_inactivity_timer),
     (int32_t)offsetof(drb_to_modify_item_ng_ran_s, drb_inactivity_timer_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(7200),
     nullptr},
    {offsetof(drb_to_modify_item_ng_ran_s, ie_exts),
     (int32_t)offsetof(drb_to_modify_item_ng_ran_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &drb_to_modify_item_ng_ran_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_drb_to_modify_item_ng_ran_s_names[] = {
    {"dRB-ID", nullptr},
    {"sDAP-Configuration", nullptr},
    {"pDCP-Configuration", nullptr},
    {"dRB-Data-Forwarding-Information", nullptr},
    {"pDCP-SN-Status-Request", nullptr},
    {"pdcp-SN-Status-Information", nullptr},
    {"dL-UP-Parameters", nullptr},
    {"cell-Group-To-Add", nullptr},
    {"cell-Group-To-Modify", nullptr},
    {"cell-Group-To-Remove", nullptr},
    {"flow-Mapping-Information", nullptr},
    {"dRB-Inactivity-Timer", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_to_modify_item_ng_ran_s_opts[] = {
    {1, (int32_t)offsetof(drb_to_modify_item_ng_ran_s, sdap_cfg_present)},
    {2, (int32_t)offsetof(drb_to_modify_item_ng_ran_s, pdcp_cfg_present)},
    {3, (int32_t)offsetof(drb_to_modify_item_ng_ran_s, drb_data_forwarding_info_present)},
    {4, (int32_t)offsetof(drb_to_modify_item_ng_ran_s, pdcp_sn_status_request_present)},
    {5, (int32_t)offsetof(drb_to_modify_item_ng_ran_s, pdcp_sn_status_info_present)},
    {6, asn1::table::seq_field::list_presence},
    {7, asn1::table::seq_field::list_presence},
    {8, asn1::table::seq_field::list_presence},
    {9, asn1::table::seq_field::list_presence},
    {10, asn1::table::seq_field::list_presence},
    {11, (int32_t)offsetof(drb_to_modify_item_ng_ran_s, drb_inactivity_timer_present)},
    {12, (int32_t)offsetof(drb_to_modify_item_ng_ran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_to_modify_item_ng_ran_s::desc = {
    {seq_table_drb_to_modify_item_ng_ran_s_fields, 13, UINT64_C(0x1), seq_table_drb_to_modify_item_ng_ran_s_opts, 12},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_to_modify_item_ng_ran_s_names};

// DRB-To-Remove-Item-EUTRAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_to_rem_item_eutran_s_fields[] = {
    {offsetof(drb_to_rem_item_eutran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_to_rem_item_eutran_s, ie_exts),
     (int32_t)offsetof(drb_to_rem_item_eutran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_to_rem_item_eutran_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_to_rem_item_eutran_s_names[] = {{"dRB-ID", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_drb_to_rem_item_eutran_s_opts[]  = {
    {1, (int32_t)offsetof(drb_to_rem_item_eutran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_to_rem_item_eutran_s::desc = {
    {seq_table_drb_to_rem_item_eutran_s_fields, 2, UINT64_C(0x1), seq_table_drb_to_rem_item_eutran_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_to_rem_item_eutran_s_names};

// DRB-To-Remove-Item-NG-RAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_to_rem_item_ng_ran_s_fields[] = {
    {offsetof(drb_to_rem_item_ng_ran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_to_rem_item_ng_ran_s, ie_exts),
     (int32_t)offsetof(drb_to_rem_item_ng_ran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_to_rem_item_ng_ran_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_to_rem_item_ng_ran_s_names[] = {{"dRB-ID", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_drb_to_rem_item_ng_ran_s_opts[]  = {
    {1, (int32_t)offsetof(drb_to_rem_item_ng_ran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_to_rem_item_ng_ran_s::desc = {
    {seq_table_drb_to_rem_item_ng_ran_s_fields, 2, UINT64_C(0x1), seq_table_drb_to_rem_item_ng_ran_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_to_rem_item_ng_ran_s_names};

// Data-Forwarding-Request ::= ENUMERATED
static const char*           enum_names_data_forwarding_request_e_values[] = {"uL", "dL", "both"};
const asn1::table::enum_desc data_forwarding_request_opts::desc = {enum_names_data_forwarding_request_e_values,
                                                                   3,
                                                                   "data_forwarding_request_e"};

// QoS-Flow-Mapping-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_qos_flow_map_item_s_fields[] = {
    {offsetof(qos_flow_map_item_s, qos_flow_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(qos_flow_map_item_s, qos_flow_map_ind),
     (int32_t)offsetof(qos_flow_map_item_s, qos_flow_map_ind_present),
     asn1::table::enum_op<decltype(qos_flow_map_item_s::qos_flow_map_ind)>(),
     decltype(qos_flow_map_item_s::qos_flow_map_ind)::has_ext,
     false,
     decltype(qos_flow_map_item_s::qos_flow_map_ind)::nof_types,
     decltype(qos_flow_map_item_s::qos_flow_map_ind)::nof_exts,
     nullptr,
     &qos_flow_map_ind_opts::desc},
    {offsetof(qos_flow_map_item_s, ie_exts),
     (int32_t)offsetof(qos_flow_map_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(qos_flow_map_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_qos_flow_map_item_s_names[] = {{"qoS-Flow-Identifier", nullptr},
                                                                              {"qoSFlowMappingIndication", nullptr},
                                                                              {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_qos_flow_map_item_s_opts[]  = {
    {1, (int32_t)offsetof(qos_flow_map_item_s, qos_flow_map_ind_present)},
    {2, (int32_t)offsetof(qos_flow_map_item_s, ie_exts_present)}};
const asn1::table::seq_desc qos_flow_map_item_s::desc = {
    {seq_table_qos_flow_map_item_s_fields, 3, UINT64_C(0x1), seq_table_qos_flow_map_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_qos_flow_map_item_s_names};

// Data-Forwarding-Information-Request ::= SEQUENCE
static const asn1::table::seq_field seq_table_data_forwarding_info_request_s_fields[] = {
    {offsetof(data_forwarding_info_request_s, data_forwarding_request),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(data_forwarding_info_request_s::data_forwarding_request)>(),
     decltype(data_forwarding_info_request_s::data_forwarding_request)::has_ext,
     false,
     decltype(data_forwarding_info_request_s::data_forwarding_request)::nof_types,
     decltype(data_forwarding_info_request_s::data_forwarding_request)::nof_exts,
     nullptr,
     &data_forwarding_request_opts::desc},
    {offsetof(data_forwarding_info_request_s, qos_flows_forwarded_on_fwd_tunnels),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(data_forwarding_info_request_s::qos_flows_forwarded_on_fwd_tunnels),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_map_item_s::desc},
    {offsetof(data_forwarding_info_request_s, ie_exts),
     (int32_t)offsetof(data_forwarding_info_request_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(data_forwarding_info_request_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_data_forwarding_info_request_s_names[] = {
    {"data-Forwarding-Request", nullptr},
    {"qoS-Flows-Forwarded-On-Fwd-Tunnels", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_data_forwarding_info_request_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(data_forwarding_info_request_s, ie_exts_present)}};
const asn1::table::seq_desc data_forwarding_info_request_s::desc = {{seq_table_data_forwarding_info_request_s_fields,
                                                                     3,
                                                                     UINT64_C(0x1),
                                                                     seq_table_data_forwarding_info_request_s_opts,
                                                                     2},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_data_forwarding_info_request_s_names};

// IntegrityProtectionIndication ::= ENUMERATED
static const char*           enum_names_integrity_protection_ind_e_values[] = {"required", "preferred", "not-needed"};
const asn1::table::enum_desc integrity_protection_ind_opts::desc = {enum_names_integrity_protection_ind_e_values,
                                                                    3,
                                                                    "integrity_protection_ind_e"};

// MaxIPrate ::= ENUMERATED
static const char*           enum_names_max_ip_rate_e_values[] = {"bitrate64kbs", "max-UErate"};
const asn1::table::enum_desc max_ip_rate_opts::desc            = {enum_names_max_ip_rate_e_values, 2, "max_ip_rate_e"};
uint8_t                      max_ip_rate_opts::to_number() const
{
  static const uint8_t numbers[] = {64};
  return map_enum_number(numbers, 1, value, "max_ip_rate_e");
}

// MaximumIPdatarate ::= SEQUENCE
static const asn1::table::seq_field seq_table_max_ip_datarate_s_fields[] = {
    {offsetof(max_ip_datarate_s, max_ip_rate),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(max_ip_datarate_s::max_ip_rate)>(),
     decltype(max_ip_datarate_s::max_ip_rate)::has_ext,
     false,
     decltype(max_ip_datarate_s::max_ip_rate)::nof_types,
     decltype(max_ip_datarate_s::max_ip_rate)::nof_exts,
     nullptr,
     &max_ip_rate_opts::desc},
    {offsetof(max_ip_datarate_s, ie_exts),
     (int32_t)offsetof(max_ip_datarate_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(max_ip_datarate_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_max_ip_datarate_s_names[] = {{"maxIPrate", nullptr},
                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_max_ip_datarate_s_opts[]  = {
    {1, (int32_t)offsetof(max_ip_datarate_s, ie_exts_present)}};
const asn1::table::seq_desc max_ip_datarate_s::desc = {
    {seq_table_max_ip_datarate_s_fields, 2, UINT64_C(0x1), seq_table_max_ip_datarate_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_max_ip_datarate_s_names};

// SecurityIndication ::= SEQUENCE
static const asn1::table::seq_field seq_table_security_ind_s_fields[] = {
    {offsetof(security_ind_s, integrity_protection_ind),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(security_ind_s::integrity_protection_ind)>(),
     decltype(security_ind_s::integrity_protection_ind)::has_ext,
     false,
     decltype(security_ind_s::integrity_protection_ind)::nof_types,
     decltype(security_ind_s::integrity_protection_ind)::nof_exts,
     nullptr,
     &integrity_protection_ind_opts::desc},
    {offsetof(security_ind_s, confidentiality_protection_ind),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(security_ind_s::confidentiality_protection_ind)>(),
     decltype(security_ind_s::confidentiality_protection_ind)::has_ext,
     false,
     decltype(security_ind_s::confidentiality_protection_ind)::nof_types,
     decltype(security_ind_s::confidentiality_protection_ind)::nof_exts,
     nullptr,
     &confidentiality_protection_ind_opts::desc},
    {offsetof(security_ind_s, max_ip_datarate),
     (int32_t)offsetof(security_ind_s, max_ip_datarate_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &max_ip_datarate_s::desc},
    {offsetof(security_ind_s, ie_exts),
     (int32_t)offsetof(security_ind_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(security_ind_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_security_ind_s_names[] = {
    {"integrityProtectionIndication", nullptr},
    {"confidentialityProtectionIndication", nullptr},
    {"maximumIPdatarate", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_security_ind_s_opts[] = {
    {2, (int32_t)offsetof(security_ind_s, max_ip_datarate_present)},
    {3, (int32_t)offsetof(security_ind_s, ie_exts_present)}};
const asn1::table::seq_desc security_ind_s::desc = {
    {seq_table_security_ind_s_fields, 4, UINT64_C(0x3), seq_table_security_ind_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_security_ind_s_names};

static const uint32_t objset_id_drb_to_setup_item_eutran_ext_ies_o[]   = {142, 172};
static const uint8_t  objset_crit_drb_to_setup_item_eutran_ext_ies_o[] = {crit_e::ignore, crit_e::reject};

// DRB-To-Setup-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_drb_to_setup_item_eutran_ext_ies_container_values[] = {
    {offsetof(drb_to_setup_item_eutran_ext_ies_container, data_forwarding_source_ip_address),
     (int32_t)offsetof(drb_to_setup_item_eutran_ext_ies_container, data_forwarding_source_ip_address_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_to_setup_item_eutran_ext_ies_container::data_forwarding_source_ip_address),
                                asn1::table::struct_json>::value},
    {offsetof(drb_to_setup_item_eutran_ext_ies_container, security_ind),
     (int32_t)offsetof(drb_to_setup_item_eutran_ext_ies_container, security_ind_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_ind_s::desc}};
static const int32_t ie_table_drb_to_setup_item_eutran_ext_ies_container_presences[] = {
    (int32_t)offsetof(drb_to_setup_item_eutran_ext_ies_container, data_forwarding_source_ip_address_present),
    (int32_t)offsetof(drb_to_setup_item_eutran_ext_ies_container, security_ind_present)};
const asn1::table::ie_container_desc drb_to_setup_item_eutran_ext_ies_container::desc = {
    objset_id_drb_to_setup_item_eutran_ext_ies_o,
    objset_crit_drb_to_setup_item_eutran_ext_ies_o,
    ie_table_drb_to_setup_item_eutran_ext_ies_container_values,
    ie_table_drb_to_setup_item_eutran_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// DRB-To-Setup-Item-EUTRAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_to_setup_item_eutran_s_fields[] = {
    {offsetof(drb_to_setup_item_eutran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_to_setup_item_eutran_s, pdcp_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_cfg_s::desc},
    {offsetof(drb_to_setup_item_eutran_s, eutran_qos),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &eutran_qos_s::desc},
    {offsetof(drb_to_setup_item_eutran_s, s1_ul_up_tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(drb_to_setup_item_eutran_s, data_forwarding_info_request),
     (int32_t)offsetof(drb_to_setup_item_eutran_s, data_forwarding_info_request_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_request_s::desc},
    {offsetof(drb_to_setup_item_eutran_s, cell_group_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     4,
     &asn1::table::item_list_codec<decltype(drb_to_setup_item_eutran_s::cell_group_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_group_info_item_s::desc},
    {offsetof(drb_to_setup_item_eutran_s, dl_up_params),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(drb_to_setup_item_eutran_s::dl_up_params),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &up_params_item_s::desc},
    {offsetof(drb_to_setup_item_eutran_s, drb_inactivity_timer),
     (int32_t)offsetof(drb_to_setup_item_eutran_s, drb_inactivity_timer_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(7200),
     nullptr},
    {offsetof(drb_to_setup_item_eutran_s, existing_allocated_s1_dl_up_tnl_info),
     (int32_t)offsetof(drb_to_setup_item_eutran_s, existing_allocated_s1_dl_up_tnl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(drb_to_setup_item_eutran_s, ie_exts),
     (int32_t)offsetof(drb_to_setup_item_eutran_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &drb_to_setup_item_eutran_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_drb_to_setup_item_eutran_s_names[] = {
    {"dRB-ID", nullptr},
    {"pDCP-Configuration", nullptr},
    {"eUTRAN-QoS", nullptr},
    {"s1-UL-UP-TNL-Information", nullptr},
    {"data-Forwarding-Information-Request", nullptr},
    {"cell-Group-Information", nullptr},
    {"dL-UP-Parameters", nullptr},
    {"dRB-Inactivity-Timer", nullptr},
    {"existing-Allocated-S1-DL-UP-TNL-Info", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_to_setup_item_eutran_s_opts[] = {
    {4, (int32_t)offsetof(drb_to_setup_item_eutran_s, data_forwarding_info_request_present)},
    {6, asn1::table::seq_field::list_presence},
    {7, (int32_t)offsetof(drb_to_setup_item_eutran_s, drb_inactivity_timer_present)},
    {8, (int32_t)offsetof(drb_to_setup_item_eutran_s, existing_allocated_s1_dl_up_tnl_info_present)},
    {9, (int32_t)offsetof(drb_to_setup_item_eutran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_to_setup_item_eutran_s::desc = {
    {seq_table_drb_to_setup_item_eutran_s_fields, 10, UINT64_C(0x2f), seq_table_drb_to_setup_item_eutran_s_opts, 5},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_to_setup_item_eutran_s_names};

// IgnoreMappingRuleIndication ::= ENUMERATED
static const char*           enum_names_ignore_map_rule_ind_e_values[] = {"true"};
const asn1::table::enum_desc ignore_map_rule_ind_opts::desc            = {enum_names_ignore_map_rule_ind_e_values,
                                                                          1,
                                                                          "ignore_map_rule_ind_e"};

// QoS-Flows-DRB-Remapping ::= ENUMERATED
static const char*           enum_names_qos_flows_drb_remap_e_values[] = {"update", "source-configuration"};
const asn1::table::enum_desc qos_flows_drb_remap_opts::desc            = {enum_names_qos_flows_drb_remap_e_values,
                                                                          2,
                                                                          "qos_flows_drb_remap_e"};

// SDTindicatorSetup ::= ENUMERATED
static const char*           enum_names_sd_tind_setup_e_values[] = {"true"};
const asn1::table::enum_desc sd_tind_setup_opts::desc = {enum_names_sd_tind_setup_e_values, 1, "sd_tind_setup_e"};

// SpecialTriggeringPurpose ::= ENUMERATED
static const char*           enum_names_special_trigger_purpose_e_values[] = {"indirect-data-forwarding"};
const asn1::table::enum_desc special_trigger_purpose_opts::desc = {enum_names_special_trigger_purpose_e_values,
                                                                   1,
                                                                   "special_trigger_purpose_e"};

static const uint32_t objset_id_drb_to_setup_item_ng_ran_ext_ies_o[] = {72, 120, 138, 141, 175, 198};
static const uint8_t  objset_crit_drb_to_setup_item_ng_ran_ext_ies_o[] =
    {crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::ignore};

// DRB-To-Setup-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_drb_to_setup_item_ng_ran_ext_ies_container_values[] = {
    {offsetof(drb_to_setup_item_ng_ran_ext_ies_container, drb_qos),
     (int32_t)offsetof(drb_to_setup_item_ng_ran_ext_ies_container, drb_qos_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &qos_flow_level_qos_params_s::desc},
    {offsetof(drb_to_setup_item_ng_ran_ext_ies_container, daps_request_info),
     (int32_t)offsetof(drb_to_setup_item_ng_ran_ext_ies_container, daps_request_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &daps_request_info_s::desc},
    {offsetof(drb_to_setup_item_ng_ran_ext_ies_container, ignore_map_rule_ind),
     (int32_t)offsetof(drb_to_setup_item_ng_ran_ext_ies_container, ignore_map_rule_ind_present),
     asn1::table::enum_op<decltype(drb_to_setup_item_ng_ran_ext_ies_container::ignore_map_rule_ind)>(),
     decltype(drb_to_setup_item_ng_ran_ext_ies_container::ignore_map_rule_ind)::has_ext,
     false,
     decltype(drb_to_setup_item_ng_ran_ext_ies_container::ignore_map_rule_ind)::nof_types,
     decltype(drb_to_setup_item_ng_ran_ext_ies_container::ignore_map_rule_ind)::nof_exts,
     nullptr,
     &ignore_map_rule_ind_opts::desc},
    {offsetof(drb_to_setup_item_ng_ran_ext_ies_container, qos_flows_drb_remap),
     (int32_t)offsetof(drb_to_setup_item_ng_ran_ext_ies_container, qos_flows_drb_remap_present),
     asn1::table::enum_op<decltype(drb_to_setup_item_ng_ran_ext_ies_container::qos_flows_drb_remap)>(),
     decltype(drb_to_setup_item_ng_ran_ext_ies_container::qos_flows_drb_remap)::has_ext,
     false,
     decltype(drb_to_setup_item_ng_ran_ext_ies_container::qos_flows_drb_remap)::nof_types,
     decltype(drb_to_setup_item_ng_ran_ext_ies_container::qos_flows_drb_remap)::nof_exts,
     nullptr,
     &qos_flows_drb_remap_opts::desc},
    {offsetof(drb_to_setup_item_ng_ran_ext_ies_container, sd_tind_setup),
     (int32_t)offsetof(drb_to_setup_item_ng_ran_ext_ies_container, sd_tind_setup_present),
     asn1::table::enum_op<decltype(drb_to_setup_item_ng_ran_ext_ies_container::sd_tind_setup)>(),
     decltype(drb_to_setup_item_ng_ran_ext_ies_container::sd_tind_setup)::has_ext,
     false,
     decltype(drb_to_setup_item_ng_ran_ext_ies_container::sd_tind_setup)::nof_types,
     decltype(drb_to_setup_item_ng_ran_ext_ies_container::sd_tind_setup)::nof_exts,
     nullptr,
     &sd_tind_setup_opts::desc},
    {offsetof(drb_to_setup_item_ng_ran_ext_ies_container, special_trigger_purpose),
     (int32_t)offsetof(drb_to_setup_item_ng_ran_ext_ies_container, special_trigger_purpose_present),
     asn1::table::enum_op<decltype(drb_to_setup_item_ng_ran_ext_ies_container::special_trigger_purpose)>(),
     decltype(drb_to_setup_item_ng_ran_ext_ies_container::special_trigger_purpose)::has_ext,
     false,
     decltype(drb_to_setup_item_ng_ran_ext_ies_container::special_trigger_purpose)::nof_types,
     decltype(drb_to_setup_item_ng_ran_ext_ies_container::special_trigger_purpose)::nof_exts,
     nullptr,
     &special_trigger_purpose_opts::desc}};
static const int32_t ie_table_drb_to_setup_item_ng_ran_ext_ies_container_presences[] = {
    (int32_t)offsetof(drb_to_setup_item_ng_ran_ext_ies_container, drb_qos_present),
    (int32_t)offsetof(drb_to_setup_item_ng_ran_ext_ies_container, daps_request_info_present),
    (int32_t)offsetof(drb_to_setup_item_ng_ran_ext_ies_container, ignore_map_rule_ind_present),
    (int32_t)offsetof(drb_to_setup_item_ng_ran_ext_ies_container, qos_flows_drb_remap_present),
    (int32_t)offsetof(drb_to_setup_item_ng_ran_ext_ies_container, sd_tind_setup_present),
    (int32_t)offsetof(drb_to_setup_item_ng_ran_ext_ies_container, special_trigger_purpose_present)};
const asn1::table::ie_container_desc drb_to_setup_item_ng_ran_ext_ies_container::desc = {
    objset_id_drb_to_setup_item_ng_ran_ext_ies_o,
    objset_crit_drb_to_setup_item_ng_ran_ext_ies_o,
    ie_table_drb_to_setup_item_ng_ran_ext_ies_container_values,
    ie_table_drb_to_setup_item_ng_ran_ext_ies_container_presences,
    6,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// DRB-To-Setup-Item-NG-RAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_to_setup_item_ng_ran_s_fields[] = {
    {offsetof(drb_to_setup_item_ng_ran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_to_setup_item_ng_ran_s, sdap_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sdap_cfg_s::desc},
    {offsetof(drb_to_setup_item_ng_ran_s, pdcp_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_cfg_s::desc},
    {offsetof(drb_to_setup_item_ng_ran_s, cell_group_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     4,
     &asn1::table::item_list_codec<decltype(drb_to_setup_item_ng_ran_s::cell_group_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_group_info_item_s::desc},
    {offsetof(drb_to_setup_item_ng_ran_s, qos_flow_info_to_be_setup),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(drb_to_setup_item_ng_ran_s::qos_flow_info_to_be_setup),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_qos_param_item_s::desc},
    {offsetof(drb_to_setup_item_ng_ran_s, drb_data_forwarding_info_request),
     (int32_t)offsetof(drb_to_setup_item_ng_ran_s, drb_data_forwarding_info_request_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_request_s::desc},
    {offsetof(drb_to_setup_item_ng_ran_s, drb_inactivity_timer),
     (int32_t)offsetof(drb_to_setup_item_ng_ran_s, drb_inactivity_timer_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(7200),
     nullptr},
    {offsetof(drb_to_setup_item_ng_ran_s, pdcp_sn_status_info),
     (int32_t)offsetof(drb_to_setup_item_ng_ran_s, pdcp_sn_status_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_sn_status_info_s::desc},
    {offsetof(drb_to_setup_item_ng_ran_s, ie_exts),
     (int32_t)offsetof(drb_to_setup_item_ng_ran_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &drb_to_setup_item_ng_ran_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_drb_to_setup_item_ng_ran_s_names[] = {
    {"dRB-ID", nullptr},
    {"sDAP-Configuration", nullptr},
    {"pDCP-Configuration", nullptr},
    {"cell-Group-Information", nullptr},
    {"qos-flow-Information-To-Be-Setup", nullptr},
    {"dRB-Data-Forwarding-Information-Request", nullptr},
    {"dRB-Inactivity-Timer", nullptr},
    {"pDCP-SN-Status-Information", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_to_setup_item_ng_ran_s_opts[] = {
    {5, (int32_t)offsetof(drb_to_setup_item_ng_ran_s, drb_data_forwarding_info_request_present)},
    {6, (int32_t)offsetof(drb_to_setup_item_ng_ran_s, drb_inactivity_timer_present)},
    {7, (int32_t)offsetof(drb_to_setup_item_ng_ran_s, pdcp_sn_status_info_present)},
    {8, (int32_t)offsetof(drb_to_setup_item_ng_ran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_to_setup_item_ng_ran_s::desc = {
    {seq_table_drb_to_setup_item_ng_ran_s_fields, 9, UINT64_C(0x1f), seq_table_drb_to_setup_item_ng_ran_s_opts, 4},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_to_setup_item_ng_ran_s_names};

static const uint32_t objset_id_drb_to_setup_mod_item_eutran_ext_ies_o[]   = {172, 142};
static const uint8_t  objset_crit_drb_to_setup_mod_item_eutran_ext_ies_o[] = {crit_e::reject, crit_e::ignore};

// DRB-To-Setup-Mod-Item-EUTRAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_drb_to_setup_mod_item_eutran_ext_ies_container_values[] = {
    {offsetof(drb_to_setup_mod_item_eutran_ext_ies_container, security_ind),
     (int32_t)offsetof(drb_to_setup_mod_item_eutran_ext_ies_container, security_ind_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_ind_s::desc},
    {offsetof(drb_to_setup_mod_item_eutran_ext_ies_container, data_forwarding_source_ip_address),
     (int32_t)offsetof(drb_to_setup_mod_item_eutran_ext_ies_container, data_forwarding_source_ip_address_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<
         decltype(drb_to_setup_mod_item_eutran_ext_ies_container::data_forwarding_source_ip_address),
         asn1::table::struct_json>::value}};
static const int32_t ie_table_drb_to_setup_mod_item_eutran_ext_ies_container_presences[] = {
    (int32_t)offsetof(drb_to_setup_mod_item_eutran_ext_ies_container, security_ind_present),
    (int32_t)offsetof(drb_to_setup_mod_item_eutran_ext_ies_container, data_forwarding_source_ip_address_present)};
const asn1::table::ie_container_desc drb_to_setup_mod_item_eutran_ext_ies_container::desc = {
    objset_id_drb_to_setup_mod_item_eutran_ext_ies_o,
    objset_crit_drb_to_setup_mod_item_eutran_ext_ies_o,
    ie_table_drb_to_setup_mod_item_eutran_ext_ies_container_values,
    ie_table_drb_to_setup_mod_item_eutran_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// DRB-To-Setup-Mod-Item-EUTRAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_to_setup_mod_item_eutran_s_fields[] = {
    {offsetof(drb_to_setup_mod_item_eutran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_to_setup_mod_item_eutran_s, pdcp_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_cfg_s::desc},
    {offsetof(drb_to_setup_mod_item_eutran_s, eutran_qos),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &eutran_qos_s::desc},
    {offsetof(drb_to_setup_mod_item_eutran_s, s1_ul_up_tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(drb_to_setup_mod_item_eutran_s, data_forwarding_info_request),
     (int32_t)offsetof(drb_to_setup_mod_item_eutran_s, data_forwarding_info_request_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_request_s::desc},
    {offsetof(drb_to_setup_mod_item_eutran_s, cell_group_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     4,
     &asn1::table::item_list_codec<decltype(drb_to_setup_mod_item_eutran_s::cell_group_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_group_info_item_s::desc},
    {offsetof(drb_to_setup_mod_item_eutran_s, dl_up_params),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     8,
     &asn1::table::item_list_codec<decltype(drb_to_setup_mod_item_eutran_s::dl_up_params),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &up_params_item_s::desc},
    {offsetof(drb_to_setup_mod_item_eutran_s, drb_inactivity_timer),
     (int32_t)offsetof(drb_to_setup_mod_item_eutran_s, drb_inactivity_timer_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(7200),
     nullptr},
    {offsetof(drb_to_setup_mod_item_eutran_s, ie_exts),
     (int32_t)offsetof(drb_to_setup_mod_item_eutran_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &drb_to_setup_mod_item_eutran_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_drb_to_setup_mod_item_eutran_s_names[] = {
    {"dRB-ID", nullptr},
    {"pDCP-Configuration", nullptr},
    {"eUTRAN-QoS", nullptr},
    {"s1-UL-UP-TNL-Information", nullptr},
    {"data-Forwarding-Information-Request", nullptr},
    {"cell-Group-Information", nullptr},
    {"dL-UP-Parameters", nullptr},
    {"dRB-Inactivity-Timer", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_to_setup_mod_item_eutran_s_opts[] = {
    {4, (int32_t)offsetof(drb_to_setup_mod_item_eutran_s, data_forwarding_info_request_present)},
    {6, asn1::table::seq_field::list_presence},
    {7, (int32_t)offsetof(drb_to_setup_mod_item_eutran_s, drb_inactivity_timer_present)},
    {8, (int32_t)offsetof(drb_to_setup_mod_item_eutran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_to_setup_mod_item_eutran_s::desc = {{seq_table_drb_to_setup_mod_item_eutran_s_fields,
                                                                     9,
                                                                     UINT64_C(0x2f),
                                                                     seq_table_drb_to_setup_mod_item_eutran_s_opts,
                                                                     4},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_drb_to_setup_mod_item_eutran_s_names};

static const uint32_t objset_id_drb_to_setup_mod_item_ng_ran_ext_ies_o[]   = {72, 138, 120, 175, 198};
static const uint8_t  objset_crit_drb_to_setup_mod_item_ng_ran_ext_ies_o[] = {crit_e::ignore,
                                                                              crit_e::reject,
                                                                              crit_e::ignore,
                                                                              crit_e::reject,
                                                                              crit_e::ignore};

// DRB-To-Setup-Mod-Item-NG-RAN-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_drb_to_setup_mod_item_ng_ran_ext_ies_container_values[] = {
    {offsetof(drb_to_setup_mod_item_ng_ran_ext_ies_container, drb_qos),
     (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_ext_ies_container, drb_qos_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &qos_flow_level_qos_params_s::desc},
    {offsetof(drb_to_setup_mod_item_ng_ran_ext_ies_container, ignore_map_rule_ind),
     (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_ext_ies_container, ignore_map_rule_ind_present),
     asn1::table::enum_op<decltype(drb_to_setup_mod_item_ng_ran_ext_ies_container::ignore_map_rule_ind)>(),
     decltype(drb_to_setup_mod_item_ng_ran_ext_ies_container::ignore_map_rule_ind)::has_ext,
     false,
     decltype(drb_to_setup_mod_item_ng_ran_ext_ies_container::ignore_map_rule_ind)::nof_types,
     decltype(drb_to_setup_mod_item_ng_ran_ext_ies_container::ignore_map_rule_ind)::nof_exts,
     nullptr,
     &ignore_map_rule_ind_opts::desc},
    {offsetof(drb_to_setup_mod_item_ng_ran_ext_ies_container, daps_request_info),
     (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_ext_ies_container, daps_request_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &daps_request_info_s::desc},
    {offsetof(drb_to_setup_mod_item_ng_ran_ext_ies_container, sd_tind_setup),
     (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_ext_ies_container, sd_tind_setup_present),
     asn1::table::enum_op<decltype(drb_to_setup_mod_item_ng_ran_ext_ies_container::sd_tind_setup)>(),
     decltype(drb_to_setup_mod_item_ng_ran_ext_ies_container::sd_tind_setup)::has_ext,
     false,
     decltype(drb_to_setup_mod_item_ng_ran_ext_ies_container::sd_tind_setup)::nof_types,
     decltype(drb_to_setup_mod_item_ng_ran_ext_ies_container::sd_tind_setup)::nof_exts,
     nullptr,
     &sd_tind_setup_opts::desc},
    {offsetof(drb_to_setup_mod_item_ng_ran_ext_ies_container, special_trigger_purpose),
     (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_ext_ies_container, special_trigger_purpose_present),
     asn1::table::enum_op<decltype(drb_to_setup_mod_item_ng_ran_ext_ies_container::special_trigger_purpose)>(),
     decltype(drb_to_setup_mod_item_ng_ran_ext_ies_container::special_trigger_purpose)::has_ext,
     false,
     decltype(drb_to_setup_mod_item_ng_ran_ext_ies_container::special_trigger_purpose)::nof_types,
     decltype(drb_to_setup_mod_item_ng_ran_ext_ies_container::special_trigger_purpose)::nof_exts,
     nullptr,
     &special_trigger_purpose_opts::desc}};
static const int32_t ie_table_drb_to_setup_mod_item_ng_ran_ext_ies_container_presences[] = {
    (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_ext_ies_container, drb_qos_present),
    (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_ext_ies_container, ignore_map_rule_ind_present),
    (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_ext_ies_container, daps_request_info_present),
    (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_ext_ies_container, sd_tind_setup_present),
    (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_ext_ies_container, special_trigger_purpose_present)};
const asn1::table::ie_container_desc drb_to_setup_mod_item_ng_ran_ext_ies_container::desc = {
    objset_id_drb_to_setup_mod_item_ng_ran_ext_ies_o,
    objset_crit_drb_to_setup_mod_item_ng_ran_ext_ies_o,
    ie_table_drb_to_setup_mod_item_ng_ran_ext_ies_container_values,
    ie_table_drb_to_setup_mod_item_ng_ran_ext_ies_container_presences,
    5,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// DRB-To-Setup-Mod-Item-NG-RAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_to_setup_mod_item_ng_ran_s_fields[] = {
    {offsetof(drb_to_setup_mod_item_ng_ran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drb_to_setup_mod_item_ng_ran_s, sdap_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sdap_cfg_s::desc},
    {offsetof(drb_to_setup_mod_item_ng_ran_s, pdcp_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_cfg_s::desc},
    {offsetof(drb_to_setup_mod_item_ng_ran_s, cell_group_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     4,
     &asn1::table::item_list_codec<decltype(drb_to_setup_mod_item_ng_ran_s::cell_group_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_group_info_item_s::desc},
    {offsetof(drb_to_setup_mod_item_ng_ran_s, flow_map_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(drb_to_setup_mod_item_ng_ran_s::flow_map_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_qos_param_item_s::desc},
    {offsetof(drb_to_setup_mod_item_ng_ran_s, drb_data_forwarding_info_request),
     (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_s, drb_data_forwarding_info_request_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_request_s::desc},
    {offsetof(drb_to_setup_mod_item_ng_ran_s, drb_inactivity_timer),
     (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_s, drb_inactivity_timer_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(7200),
     nullptr},
    {offsetof(drb_to_setup_mod_item_ng_ran_s, pdcp_sn_status_info),
     (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_s, pdcp_sn_status_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_sn_status_info_s::desc},
    {offsetof(drb_to_setup_mod_item_ng_ran_s, ie_exts),
     (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &drb_to_setup_mod_item_ng_ran_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_drb_to_setup_mod_item_ng_ran_s_names[] = {
    {"dRB-ID", nullptr},
    {"sDAP-Configuration", nullptr},
    {"pDCP-Configuration", nullptr},
    {"cell-Group-Information", nullptr},
    {"flow-Mapping-Information", nullptr},
    {"dRB-Data-Forwarding-Information-Request", nullptr},
    {"dRB-Inactivity-Timer", nullptr},
    {"pDCP-SN-Status-Information", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drb_to_setup_mod_item_ng_ran_s_opts[] = {
    {5, (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_s, drb_data_forwarding_info_request_present)},
    {6, (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_s, drb_inactivity_timer_present)},
    {7, (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_s, pdcp_sn_status_info_present)},
    {8, (int32_t)offsetof(drb_to_setup_mod_item_ng_ran_s, ie_exts_present)}};
const asn1::table::seq_desc drb_to_setup_mod_item_ng_ran_s::desc = {{seq_table_drb_to_setup_mod_item_ng_ran_s_fields,
                                                                     9,
                                                                     UINT64_C(0x1f),
                                                                     seq_table_drb_to_setup_mod_item_ng_ran_s_opts,
                                                                     4},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_drb_to_setup_mod_item_ng_ran_s_names};

// DRB-Usage-Report-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_usage_report_item_s_fields[] = {
    {offsetof(drb_usage_report_item_s, start_time_stamp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_usage_report_item_s::start_time_stamp), asn1::table::string_json>::value},
    {offsetof(drb_usage_report_item_s, end_time_stamp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_usage_report_item_s::end_time_stamp), asn1::table::string_json>::value},
    {offsetof(drb_usage_report_item_s, usage_count_ul),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     INT64_C(0),
     UINT64_C(18446744073709551615),
     nullptr},
    {offsetof(drb_usage_report_item_s, usage_count_dl),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     INT64_C(0),
     UINT64_C(18446744073709551615),
     nullptr},
    {offsetof(drb_usage_report_item_s, ie_exts),
     (int32_t)offsetof(drb_usage_report_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drb_usage_report_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drb_usage_report_item_s_names[] = {{"startTimeStamp", nullptr},
                                                                                  {"endTimeStamp", nullptr},
                                                                                  {"usageCountUL", nullptr},
                                                                                  {"usageCountDL", nullptr},
                                                                                  {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_drb_usage_report_item_s_opts[]  = {
    {4, (int32_t)offsetof(drb_usage_report_item_s, ie_exts_present)}};
const asn1::table::seq_desc drb_usage_report_item_s::desc = {
    {seq_table_drb_usage_report_item_s_fields, 5, UINT64_C(0xf), seq_table_drb_usage_report_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_drb_usage_report_item_s_names};

// DRBs-Subject-To-Counter-Check-Item-EUTRAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drbs_subject_to_counter_check_item_eutran_s_fields[] = {
    {offsetof(drbs_subject_to_counter_check_item_eutran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drbs_subject_to_counter_check_item_eutran_s, pdcp_ul_count),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_count_s::desc},
    {offsetof(drbs_subject_to_counter_check_item_eutran_s, pdcp_dl_count),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_count_s::desc},
    {offsetof(drbs_subject_to_counter_check_item_eutran_s, ie_exts),
     (int32_t)offsetof(drbs_subject_to_counter_check_item_eutran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drbs_subject_to_counter_check_item_eutran_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drbs_subject_to_counter_check_item_eutran_s_names[] =
    {{"dRB-ID", nullptr}, {"pDCP-UL-Count", nullptr}, {"pDCP-DL-Count", nullptr}, {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drbs_subject_to_counter_check_item_eutran_s_opts[] = {
    {3, (int32_t)offsetof(drbs_subject_to_counter_check_item_eutran_s, ie_exts_present)}};
const asn1::table::seq_desc drbs_subject_to_counter_check_item_eutran_s::desc = {
    {seq_table_drbs_subject_to_counter_check_item_eutran_s_fields,
     4,
     UINT64_C(0x7),
     seq_table_drbs_subject_to_counter_check_item_eutran_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_drbs_subject_to_counter_check_item_eutran_s_names};

// DRBs-Subject-To-Counter-Check-Item-NG-RAN ::= SEQUENCE
static const asn1::table::seq_field seq_table_drbs_subject_to_counter_check_item_ng_ran_s_fields[] = {
    {offsetof(drbs_subject_to_counter_check_item_ng_ran_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(drbs_subject_to_counter_check_item_ng_ran_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drbs_subject_to_counter_check_item_ng_ran_s, pdcp_ul_count),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_count_s::desc},
    {offsetof(drbs_subject_to_counter_check_item_ng_ran_s, pdcp_dl_count),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_count_s::desc},
    {offsetof(drbs_subject_to_counter_check_item_ng_ran_s, ie_exts),
     (int32_t)offsetof(drbs_subject_to_counter_check_item_ng_ran_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drbs_subject_to_counter_check_item_ng_ran_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drbs_subject_to_counter_check_item_ng_ran_s_names[] = {
    {"pDU-Session-ID", nullptr},
    {"dRB-ID", nullptr},
    {"pDCP-UL-Count", nullptr},
    {"pDCP-DL-Count", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drbs_subject_to_counter_check_item_ng_ran_s_opts[] = {
    {4, (int32_t)offsetof(drbs_subject_to_counter_check_item_ng_ran_s, ie_exts_present)}};
const asn1::table::seq_desc drbs_subject_to_counter_check_item_ng_ran_s::desc = {
    {seq_table_drbs_subject_to_counter_check_item_ng_ran_s_fields,
     5,
     UINT64_C(0xf),
     seq_table_drbs_subject_to_counter_check_item_ng_ran_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_drbs_subject_to_counter_check_item_ng_ran_s_names};

// DRBs-Subject-To-Early-Forwarding-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_drbs_subject_to_early_forwarding_item_s_fields[] = {
    {offsetof(drbs_subject_to_early_forwarding_item_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(drbs_subject_to_early_forwarding_item_s, dl_count_value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_count_s::desc},
    {offsetof(drbs_subject_to_early_forwarding_item_s, ie_exts),
     (int32_t)offsetof(drbs_subject_to_early_forwarding_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(drbs_subject_to_early_forwarding_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_drbs_subject_to_early_forwarding_item_s_names[] = {
    {"dRB-ID", nullptr},
    {"dLCountValue", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_drbs_subject_to_early_forwarding_item_s_opts[] = {
    {2, (int32_t)offsetof(drbs_subject_to_early_forwarding_item_s, ie_exts_present)}};
const asn1::table::seq_desc drbs_subject_to_early_forwarding_item_s::desc = {
    {seq_table_drbs_subject_to_early_forwarding_item_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_drbs_subject_to_early_forwarding_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_drbs_subject_to_early_forwarding_item_s_names};

// Data-Usage-Report-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_data_usage_report_item_s_fields[] = {
    {offsetof(data_usage_report_item_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(data_usage_report_item_s, rat_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(data_usage_report_item_s::rat_type)>(),
     decltype(data_usage_report_item_s::rat_type)::has_ext,
     false,
     decltype(data_usage_report_item_s::rat_type)::nof_types,
     decltype(data_usage_report_item_s::rat_type)::nof_exts,
     nullptr,
     &rat_type_opts::desc},
    {offsetof(data_usage_report_item_s, drb_usage_report_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2,
     &asn1::table::item_list_codec<decltype(data_usage_report_item_s::drb_usage_report_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_usage_report_item_s::desc},
    {offsetof(data_usage_report_item_s, ie_exts),
     (int32_t)offsetof(data_usage_report_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(data_usage_report_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_data_usage_report_item_s_names[] = {{"dRB-ID", nullptr},
                                                                                   {"rAT-Type", nullptr},
                                                                                   {"dRB-Usage-Report-List", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_data_usage_report_item_s_opts[]  = {
    {3, (int32_t)offsetof(data_usage_report_item_s, ie_exts_present)}};
const asn1::table::seq_desc data_usage_report_item_s::desc = {
    {seq_table_data_usage_report_item_s_fields, 4, UINT64_C(0x7), seq_table_data_usage_report_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_data_usage_report_item_s_names};

// MRDC-Data-Usage-Report-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mrdc_data_usage_report_item_s_fields[] = {
    {offsetof(mrdc_data_usage_report_item_s, start_time_stamp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mrdc_data_usage_report_item_s::start_time_stamp),
                                asn1::table::string_json>::value},
    {offsetof(mrdc_data_usage_report_item_s, end_time_stamp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mrdc_data_usage_report_item_s::end_time_stamp),
                                asn1::table::string_json>::value},
    {offsetof(mrdc_data_usage_report_item_s, usage_count_ul),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     INT64_C(0),
     UINT64_C(18446744073709551615),
     nullptr},
    {offsetof(mrdc_data_usage_report_item_s, usage_count_dl),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     INT64_C(0),
     UINT64_C(18446744073709551615),
     nullptr},
    {offsetof(mrdc_data_usage_report_item_s, ie_exts),
     (int32_t)offsetof(mrdc_data_usage_report_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mrdc_data_usage_report_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mrdc_data_usage_report_item_s_names[] = {{"startTimeStamp", nullptr},
                                                                                        {"endTimeStamp", nullptr},
                                                                                        {"usageCountUL", nullptr},
                                                                                        {"usageCountDL", nullptr},
                                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mrdc_data_usage_report_item_s_opts[]  = {
    {4, (int32_t)offsetof(mrdc_data_usage_report_item_s, ie_exts_present)}};
const asn1::table::seq_desc mrdc_data_usage_report_item_s::desc = {
    {seq_table_mrdc_data_usage_report_item_s_fields, 5, UINT64_C(0xf), seq_table_mrdc_data_usage_report_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_mrdc_data_usage_report_item_s_names};

// Data-Usage-per-PDU-Session-Report ::= SEQUENCE
static const asn1::table::seq_field seq_table_data_usage_per_pdu_session_report_s_fields[] = {
    {offsetof(data_usage_per_pdu_session_report_s, secondary_rat_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(data_usage_per_pdu_session_report_s::secondary_rat_type)>(),
     decltype(data_usage_per_pdu_session_report_s::secondary_rat_type)::has_ext,
     false,
     decltype(data_usage_per_pdu_session_report_s::secondary_rat_type)::nof_types,
     decltype(data_usage_per_pdu_session_report_s::secondary_rat_type)::nof_exts,
     nullptr,
     &data_usage_per_pdu_session_report_s::secondary_rat_type_opts::desc},
    {offsetof(data_usage_per_pdu_session_report_s, pdu_session_timed_report_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2,
     &asn1::table::item_list_codec<decltype(data_usage_per_pdu_session_report_s::pdu_session_timed_report_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mrdc_data_usage_report_item_s::desc},
    {offsetof(data_usage_per_pdu_session_report_s, ie_exts),
     (int32_t)offsetof(data_usage_per_pdu_session_report_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(data_usage_per_pdu_session_report_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_data_usage_per_pdu_session_report_s_names[] = {
    {"secondaryRATType", nullptr},
    {"pDU-session-Timed-Report-List", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_data_usage_per_pdu_session_report_s_opts[] = {
    {2, (int32_t)offsetof(data_usage_per_pdu_session_report_s, ie_exts_present)}};
const asn1::table::seq_desc data_usage_per_pdu_session_report_s::desc = {
    {seq_table_data_usage_per_pdu_session_report_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_data_usage_per_pdu_session_report_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_data_usage_per_pdu_session_report_s_names};

static const char* enum_names_data_usage_per_pdu_session_report_s_secondary_rat_type_e__values[] = {"nR", "e-UTRA"};
const asn1::table::enum_desc data_usage_per_pdu_session_report_s::secondary_rat_type_opts::desc  = {
    enum_names_data_usage_per_pdu_session_report_s_secondary_rat_type_e__values,
    2,
    "data_usage_per_pdu_session_report_s::secondary_rat_type_e_"};

// Data-Usage-per-QoS-Flow-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_data_usage_per_qos_flow_item_s_fields[] = {
    {offsetof(data_usage_per_qos_flow_item_s, qos_flow_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(data_usage_per_qos_flow_item_s, secondary_rat_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(data_usage_per_qos_flow_item_s::secondary_rat_type)>(),
     decltype(data_usage_per_qos_flow_item_s::secondary_rat_type)::has_ext,
     false,
     decltype(data_usage_per_qos_flow_item_s::secondary_rat_type)::nof_types,
     decltype(data_usage_per_qos_flow_item_s::secondary_rat_type)::nof_exts,
     nullptr,
     &data_usage_per_qos_flow_item_s::secondary_rat_type_opts::desc},
    {offsetof(data_usage_per_qos_flow_item_s, qos_flow_timed_report_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     2,
     &asn1::table::item_list_codec<decltype(data_usage_per_qos_flow_item_s::qos_flow_timed_report_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mrdc_data_usage_report_item_s::desc},
    {offsetof(data_usage_per_qos_flow_item_s, ie_exts),
     (int32_t)offsetof(data_usage_per_qos_flow_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(data_usage_per_qos_flow_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_data_usage_per_qos_flow_item_s_names[] = {
    {"qoS-Flow-Identifier", nullptr},
    {"secondaryRATType", nullptr},
    {"qoS-Flow-Timed-Report-List", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_data_usage_per_qos_flow_item_s_opts[] = {
    {3, (int32_t)offsetof(data_usage_per_qos_flow_item_s, ie_exts_present)}};
const asn1::table::seq_desc data_usage_per_qos_flow_item_s::desc = {{seq_table_data_usage_per_qos_flow_item_s_fields,
                                                                     4,
                                                                     UINT64_C(0x7),
                                                                     seq_table_data_usage_per_qos_flow_item_s_opts,
                                                                     1},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_data_usage_per_qos_flow_item_s_names};

static const char* enum_names_data_usage_per_qos_flow_item_s_secondary_rat_type_e__values[] = {"nR", "e-UTRA"};
const asn1::table::enum_desc data_usage_per_qos_flow_item_s::secondary_rat_type_opts::desc  = {
    enum_names_data_usage_per_qos_flow_item_s_secondary_rat_type_e__values,
    2,
    "data_usage_per_qos_flow_item_s::secondary_rat_type_e_"};

// DataDiscardRequired ::= ENUMERATED
static const char*           enum_names_data_discard_required_e_values[] = {"required"};
const asn1::table::enum_desc data_discard_required_opts::desc            = {enum_names_data_discard_required_e_values,
                                                                            1,
                                                                            "data_discard_required_e"};

// QoS-Flows-to-be-forwarded-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_qos_flows_to_be_forwarded_item_s_fields[] = {
    {offsetof(qos_flows_to_be_forwarded_item_s, qos_flow_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(qos_flows_to_be_forwarded_item_s, ie_exts),
     (int32_t)offsetof(qos_flows_to_be_forwarded_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(qos_flows_to_be_forwarded_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_qos_flows_to_be_forwarded_item_s_names[] = {
    {"qoS-Flow-Identifier", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_qos_flows_to_be_forwarded_item_s_opts[] = {
    {1, (int32_t)offsetof(qos_flows_to_be_forwarded_item_s, ie_exts_present)}};
const asn1::table::seq_desc qos_flows_to_be_forwarded_item_s::desc = {
    {seq_table_qos_flows_to_be_forwarded_item_s_fields,
     2,
     UINT64_C(0x1),
     seq_table_qos_flows_to_be_forwarded_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_qos_flows_to_be_forwarded_item_s_names};

// DataForwardingtoE-UTRANInformationListItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_data_forwardingto_e_utran_info_list_item_s_fields[] = {
    {offsetof(data_forwardingto_e_utran_info_list_item_s, data_forwarding_tunnel_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(data_forwardingto_e_utran_info_list_item_s, qos_flows_to_be_forwarded_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(data_forwardingto_e_utran_info_list_item_s::qos_flows_to_be_forwarded_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flows_to_be_forwarded_item_s::desc},
    {offsetof(data_forwardingto_e_utran_info_list_item_s, ie_exts),
     (int32_t)offsetof(data_forwardingto_e_utran_info_list_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(data_forwardingto_e_utran_info_list_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_data_forwardingto_e_utran_info_list_item_s_names[] = {
    {"data-forwarding-tunnel-information", nullptr},
    {"qoS-Flows-to-be-forwarded-List", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_data_forwardingto_e_utran_info_list_item_s_opts[] = {
    {2, (int32_t)offsetof(data_forwardingto_e_utran_info_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc data_forwardingto_e_utran_info_list_item_s::desc = {
    {seq_table_data_forwardingto_e_utran_info_list_item_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_data_forwardingto_e_utran_info_list_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_data_forwardingto_e_utran_info_list_item_s_names};

// DirectForwardingPathAvailability ::= ENUMERATED
static const char* enum_names_direct_forwarding_path_availability_e_values[] = {"inter-system-direct-path-available",
                                                                                "intra-system-direct-path-available"};
const asn1::table::enum_desc direct_forwarding_path_availability_opts::desc  = {
    enum_names_direct_forwarding_path_availability_e_values,
    2,
    "direct_forwarding_path_availability_e"};

// ECGI ::= SEQUENCE
static const asn1::table::seq_field seq_table_ecgi_s_fields[] = {
    {offsetof(ecgi_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ecgi_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(ecgi_s, eutran_cell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ecgi_s::eutran_cell_id), asn1::table::struct_json>::value},
    {offsetof(ecgi_s, ie_exts),
     (int32_t)offsetof(ecgi_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ecgi_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ecgi_s_names[] = {{"pLMN-Identity", nullptr},
                                                                 {"eUTRAN-Cell-Identity", nullptr},
                                                                 {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ecgi_s_opts[]  = {{2, (int32_t)offsetof(ecgi_s, ie_exts_present)}};
const asn1::table::seq_desc ecgi_s::desc = {{seq_table_ecgi_s_fields, 3, UINT64_C(0x3), seq_table_ecgi_s_opts, 1},
                                            false,
                                            true,
                                            nullptr,
                                            0,
                                            seq_table_ecgi_s_names};

// ECGI-Support-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ecgi_support_item_s_fields[] = {
    {offsetof(ecgi_support_item_s, ecgi),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ecgi_s::desc},
    {offsetof(ecgi_support_item_s, ie_exts),
     (int32_t)offsetof(ecgi_support_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ecgi_support_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ecgi_support_item_s_names[] = {{"eCGI", nullptr},
                                                                              {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ecgi_support_item_s_opts[]  = {
    {1, (int32_t)offsetof(ecgi_support_item_s, ie_exts_present)}};
const asn1::table::seq_desc ecgi_support_item_s::desc = {
    {seq_table_ecgi_support_item_s_fields, 2, UINT64_C(0x1), seq_table_ecgi_support_item_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_ecgi_support_item_s_names};

// EUTRAN-QoS-Support-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_eutran_qos_support_item_s_fields[] = {
    {offsetof(eutran_qos_support_item_s, eutran_qos),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &eutran_qos_s::desc},
    {offsetof(eutran_qos_support_item_s, ie_exts),
     (int32_t)offsetof(eutran_qos_support_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(eutran_qos_support_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_eutran_qos_support_item_s_names[] = {{"eUTRAN-QoS", nullptr},
                                                                                    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_eutran_qos_support_item_s_opts[]  = {
    {1, (int32_t)offsetof(eutran_qos_support_item_s, ie_exts_present)}};
const asn1::table::seq_desc eutran_qos_support_item_s::desc = {
    {seq_table_eutran_qos_support_item_s_fields, 2, UINT64_C(0x1), seq_table_eutran_qos_support_item_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_eutran_qos_support_item_s_names};

// Extended-GNB-CU-CP-Name ::= SEQUENCE
static const asn1::table::seq_field seq_table_extended_gnb_cu_cp_name_s_fields[] = {
    {offsetof(extended_gnb_cu_cp_name_s, gnb_cu_cp_name_visible_string),
     (int32_t)offsetof(extended_gnb_cu_cp_name_s, gnb_cu_cp_name_visible_string_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(extended_gnb_cu_cp_name_s::gnb_cu_cp_name_visible_string),
                                asn1::table::string_json>::value},
    {offsetof(extended_gnb_cu_cp_name_s, gnb_cu_cp_name_utf8_string),
     (int32_t)offsetof(extended_gnb_cu_cp_name_s, gnb_cu_cp_name_utf8_string_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(extended_gnb_cu_cp_name_s::gnb_cu_cp_name_utf8_string),
                                asn1::table::string_json>::value},
    {offsetof(extended_gnb_cu_cp_name_s, ie_exts),
     (int32_t)offsetof(extended_gnb_cu_cp_name_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(extended_gnb_cu_cp_name_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_extended_gnb_cu_cp_name_s_names[] = {
    {"gNB-CU-CP-NameVisibleString", nullptr},
    {"gNB-CU-CP-NameUTF8String", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_extended_gnb_cu_cp_name_s_opts[] = {
    {0, (int32_t)offsetof(extended_gnb_cu_cp_name_s, gnb_cu_cp_name_visible_string_present)},
    {1, (int32_t)offsetof(extended_gnb_cu_cp_name_s, gnb_cu_cp_name_utf8_string_present)},
    {2, (int32_t)offsetof(extended_gnb_cu_cp_name_s, ie_exts_present)}};
const asn1::table::seq_desc extended_gnb_cu_cp_name_s::desc = {
    {seq_table_extended_gnb_cu_cp_name_s_fields, 3, UINT64_C(0x0), seq_table_extended_gnb_cu_cp_name_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_extended_gnb_cu_cp_name_s_names};

// Extended-GNB-CU-UP-Name ::= SEQUENCE
static const asn1::table::seq_field seq_table_extended_gnb_cu_up_name_s_fields[] = {
    {offsetof(extended_gnb_cu_up_name_s, gnb_cu_up_name_visible_string),
     (int32_t)offsetof(extended_gnb_cu_up_name_s, gnb_cu_up_name_visible_string_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(extended_gnb_cu_up_name_s::gnb_cu_up_name_visible_string),
                                asn1::table::string_json>::value},
    {offsetof(extended_gnb_cu_up_name_s, gnb_cu_up_name_utf8_string),
     (int32_t)offsetof(extended_gnb_cu_up_name_s, gnb_cu_up_name_utf8_string_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(extended_gnb_cu_up_name_s::gnb_cu_up_name_utf8_string),
                                asn1::table::string_json>::value},
    {offsetof(extended_gnb_cu_up_name_s, ie_exts),
     (int32_t)offsetof(extended_gnb_cu_up_name_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(extended_gnb_cu_up_name_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_extended_gnb_cu_up_name_s_names[] = {
    {"gNB-CU-UP-NameVisibleString", nullptr},
    {"gNB-CU-UP-NameUTF8String", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_extended_gnb_cu_up_name_s_opts[] = {
    {0, (int32_t)offsetof(extended_gnb_cu_up_name_s, gnb_cu_up_name_visible_string_present)},
    {1, (int32_t)offsetof(extended_gnb_cu_up_name_s, gnb_cu_up_name_utf8_string_present)},
    {2, (int32_t)offsetof(extended_gnb_cu_up_name_s, ie_exts_present)}};
const asn1::table::seq_desc extended_gnb_cu_up_name_s::desc = {
    {seq_table_extended_gnb_cu_up_name_s_fields, 3, UINT64_C(0x0), seq_table_extended_gnb_cu_up_name_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_extended_gnb_cu_up_name_s_names};

// NR-CGI ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_cgi_s_fields[] = {
    {offsetof(nr_cgi_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(nr_cgi_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(nr_cgi_s, nr_cell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(nr_cgi_s::nr_cell_id), asn1::table::struct_json>::value},
    {offsetof(nr_cgi_s, ie_exts),
     (int32_t)offsetof(nr_cgi_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(nr_cgi_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_nr_cgi_s_names[] = {{"pLMN-Identity", nullptr},
                                                                   {"nR-Cell-Identity", nullptr},
                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_nr_cgi_s_opts[]  = {{2, (int32_t)offsetof(nr_cgi_s, ie_exts_present)}};
const asn1::table::seq_desc nr_cgi_s::desc = {{seq_table_nr_cgi_s_fields, 3, UINT64_C(0x3), seq_table_nr_cgi_s_opts, 1},
                                              false,
                                              true,
                                              nullptr,
                                              0,
                                              seq_table_nr_cgi_s_names};

// Extended-NR-CGI-Support-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_extended_nr_cgi_support_item_s_fields[] = {
    {offsetof(extended_nr_cgi_support_item_s, nr_cgi),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_cgi_s::desc},
    {offsetof(extended_nr_cgi_support_item_s, ie_exts),
     (int32_t)offsetof(extended_nr_cgi_support_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(extended_nr_cgi_support_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_extended_nr_cgi_support_item_s_names[] = {{"nR-CGI", nullptr},
                                                                                         {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_extended_nr_cgi_support_item_s_opts[]  = {
    {1, (int32_t)offsetof(extended_nr_cgi_support_item_s, ie_exts_present)}};
const asn1::table::seq_desc extended_nr_cgi_support_item_s::desc = {{seq_table_extended_nr_cgi_support_item_s_fields,
                                                                     2,
                                                                     UINT64_C(0x1),
                                                                     seq_table_extended_nr_cgi_support_item_s_opts,
                                                                     1},
                                                                    false,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_extended_nr_cgi_support_item_s_names};

// Slice-Support-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_slice_support_item_s_fields[] = {
    {offsetof(slice_support_item_s, snssai),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &snssai_s::desc},
    {offsetof(slice_support_item_s, ie_exts),
     (int32_t)offsetof(slice_support_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(slice_support_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_slice_support_item_s_names[] = {{"sNSSAI", nullptr},
                                                                               {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_slice_support_item_s_opts[]  = {
    {1, (int32_t)offsetof(slice_support_item_s, ie_exts_present)}};
const asn1::table::seq_desc slice_support_item_s::desc = {
    {seq_table_slice_support_item_s_fields, 2, UINT64_C(0x1), seq_table_slice_support_item_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_slice_support_item_s_names};

// GNB-CU-CP-TNLA-Failed-To-Setup-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_gnb_cu_cp_tnl_a_failed_to_setup_item_s_fields[] = {
    {offsetof(gnb_cu_cp_tnl_a_failed_to_setup_item_s, tnl_assoc_transport_layer_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cp_tnl_info_c::desc},
    {offsetof(gnb_cu_cp_tnl_a_failed_to_setup_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(gnb_cu_cp_tnl_a_failed_to_setup_item_s, ie_exts),
     (int32_t)offsetof(gnb_cu_cp_tnl_a_failed_to_setup_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gnb_cu_cp_tnl_a_failed_to_setup_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_gnb_cu_cp_tnl_a_failed_to_setup_item_s_names[] = {
    {"tNLAssociationTransportLayerAddress", nullptr},
    {"cause", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_gnb_cu_cp_tnl_a_failed_to_setup_item_s_opts[] = {
    {2, (int32_t)offsetof(gnb_cu_cp_tnl_a_failed_to_setup_item_s, ie_exts_present)}};
const asn1::table::seq_desc gnb_cu_cp_tnl_a_failed_to_setup_item_s::desc = {
    {seq_table_gnb_cu_cp_tnl_a_failed_to_setup_item_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_gnb_cu_cp_tnl_a_failed_to_setup_item_s_opts,
     1},
    false,
    true,
    nullptr,
    0,
    seq_table_gnb_cu_cp_tnl_a_failed_to_setup_item_s_names};

// GNB-CU-CP-TNLA-Setup-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_gnb_cu_cp_tnl_a_setup_item_s_fields[] = {
    {offsetof(gnb_cu_cp_tnl_a_setup_item_s, tnl_assoc_transport_layer_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cp_tnl_info_c::desc},
    {offsetof(gnb_cu_cp_tnl_a_setup_item_s, ie_exts),
     (int32_t)offsetof(gnb_cu_cp_tnl_a_setup_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gnb_cu_cp_tnl_a_setup_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_gnb_cu_cp_tnl_a_setup_item_s_names[] = {
    {"tNLAssociationTransportLayerAddress", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_gnb_cu_cp_tnl_a_setup_item_s_opts[] = {
    {1, (int32_t)offsetof(gnb_cu_cp_tnl_a_setup_item_s, ie_exts_present)}};
const asn1::table::seq_desc gnb_cu_cp_tnl_a_setup_item_s::desc = {
    {seq_table_gnb_cu_cp_tnl_a_setup_item_s_fields, 2, UINT64_C(0x1), seq_table_gnb_cu_cp_tnl_a_setup_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_gnb_cu_cp_tnl_a_setup_item_s_names};

// TNLAssociationUsage ::= ENUMERATED
static const char*           enum_names_tnl_assoc_usage_e_values[] = {"ue", "non-ue", "both"};
const asn1::table::enum_desc tnl_assoc_usage_opts::desc = {enum_names_tnl_assoc_usage_e_values, 3, "tnl_assoc_usage_e"};

// GNB-CU-CP-TNLA-To-Add-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_gnb_cu_cp_tnl_a_to_add_item_s_fields[] = {
    {offsetof(gnb_cu_cp_tnl_a_to_add_item_s, tnl_assoc_transport_layer_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cp_tnl_info_c::desc},
    {offsetof(gnb_cu_cp_tnl_a_to_add_item_s, tnl_assoc_usage),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(gnb_cu_cp_tnl_a_to_add_item_s::tnl_assoc_usage)>(),
     decltype(gnb_cu_cp_tnl_a_to_add_item_s::tnl_assoc_usage)::has_ext,
     false,
     decltype(gnb_cu_cp_tnl_a_to_add_item_s::tnl_assoc_usage)::nof_types,
     decltype(gnb_cu_cp_tnl_a_to_add_item_s::tnl_assoc_usage)::nof_exts,
     nullptr,
     &tnl_assoc_usage_opts::desc},
    {offsetof(gnb_cu_cp_tnl_a_to_add_item_s, ie_exts),
     (int32_t)offsetof(gnb_cu_cp_tnl_a_to_add_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gnb_cu_cp_tnl_a_to_add_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_gnb_cu_cp_tnl_a_to_add_item_s_names[] = {
    {"tNLAssociationTransportLayerAddress", nullptr},
    {"tNLAssociationUsage", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_gnb_cu_cp_tnl_a_to_add_item_s_opts[] = {
    {2, (int32_t)offsetof(gnb_cu_cp_tnl_a_to_add_item_s, ie_exts_present)}};
const asn1::table::seq_desc gnb_cu_cp_tnl_a_to_add_item_s::desc = {
    {seq_table_gnb_cu_cp_tnl_a_to_add_item_s_fields, 3, UINT64_C(0x3), seq_table_gnb_cu_cp_tnl_a_to_add_item_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_gnb_cu_cp_tnl_a_to_add_item_s_names};

static const uint32_t objset_id_gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o[]   = {75};
static const uint8_t  objset_crit_gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o[] = {crit_e::reject};

// GNB-CU-CP-TNLA-To-Remove-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_gnb_cu_cp_tnl_a_to_rem_item_ext_ies_container_values[] = {
    {offsetof(gnb_cu_cp_tnl_a_to_rem_item_ext_ies_container, tnl_assoc_transport_layer_address_gnb_cu_up),
     (int32_t)offsetof(gnb_cu_cp_tnl_a_to_rem_item_ext_ies_container,
                       tnl_assoc_transport_layer_address_gnb_cu_up_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cp_tnl_info_c::desc}};
static const int32_t ie_table_gnb_cu_cp_tnl_a_to_rem_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(gnb_cu_cp_tnl_a_to_rem_item_ext_ies_container,
                      tnl_assoc_transport_layer_address_gnb_cu_up_present)};
const asn1::table::ie_container_desc gnb_cu_cp_tnl_a_to_rem_item_ext_ies_container::desc = {
    objset_id_gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o,
    objset_crit_gnb_cu_cp_tnl_a_to_rem_item_ext_ies_o,
    ie_table_gnb_cu_cp_tnl_a_to_rem_item_ext_ies_container_values,
    ie_table_gnb_cu_cp_tnl_a_to_rem_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// GNB-CU-CP-TNLA-To-Remove-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_gnb_cu_cp_tnl_a_to_rem_item_s_fields[] = {
    {offsetof(gnb_cu_cp_tnl_a_to_rem_item_s, tnl_assoc_transport_layer_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cp_tnl_info_c::desc},
    {offsetof(gnb_cu_cp_tnl_a_to_rem_item_s, ie_exts),
     (int32_t)offsetof(gnb_cu_cp_tnl_a_to_rem_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &gnb_cu_cp_tnl_a_to_rem_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_gnb_cu_cp_tnl_a_to_rem_item_s_names[] = {
    {"tNLAssociationTransportLayerAddress", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_gnb_cu_cp_tnl_a_to_rem_item_s_opts[] = {
    {1, (int32_t)offsetof(gnb_cu_cp_tnl_a_to_rem_item_s, ie_exts_present)}};
const asn1::table::seq_desc gnb_cu_cp_tnl_a_to_rem_item_s::desc = {
    {seq_table_gnb_cu_cp_tnl_a_to_rem_item_s_fields, 2, UINT64_C(0x1), seq_table_gnb_cu_cp_tnl_a_to_rem_item_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_gnb_cu_cp_tnl_a_to_rem_item_s_names};

// GNB-CU-CP-TNLA-To-Update-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_gnb_cu_cp_tnl_a_to_upd_item_s_fields[] = {
    {offsetof(gnb_cu_cp_tnl_a_to_upd_item_s, tnl_assoc_transport_layer_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cp_tnl_info_c::desc},
    {offsetof(gnb_cu_cp_tnl_a_to_upd_item_s, tnl_assoc_usage),
     (int32_t)offsetof(gnb_cu_cp_tnl_a_to_upd_item_s, tnl_assoc_usage_present),
     asn1::table::enum_op<decltype(gnb_cu_cp_tnl_a_to_upd_item_s::tnl_assoc_usage)>(),
     decltype(gnb_cu_cp_tnl_a_to_upd_item_s::tnl_assoc_usage)::has_ext,
     false,
     decltype(gnb_cu_cp_tnl_a_to_upd_item_s::tnl_assoc_usage)::nof_types,
     decltype(gnb_cu_cp_tnl_a_to_upd_item_s::tnl_assoc_usage)::nof_exts,
     nullptr,
     &tnl_assoc_usage_opts::desc},
    {offsetof(gnb_cu_cp_tnl_a_to_upd_item_s, ie_exts),
     (int32_t)offsetof(gnb_cu_cp_tnl_a_to_upd_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gnb_cu_cp_tnl_a_to_upd_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_gnb_cu_cp_tnl_a_to_upd_item_s_names[] = {
    {"tNLAssociationTransportLayerAddress", nullptr},
    {"tNLAssociationUsage", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_gnb_cu_cp_tnl_a_to_upd_item_s_opts[] = {
    {1, (int32_t)offsetof(gnb_cu_cp_tnl_a_to_upd_item_s, tnl_assoc_usage_present)},
    {2, (int32_t)offsetof(gnb_cu_cp_tnl_a_to_upd_item_s, ie_exts_present)}};
const asn1::table::seq_desc gnb_cu_cp_tnl_a_to_upd_item_s::desc = {
    {seq_table_gnb_cu_cp_tnl_a_to_upd_item_s_fields, 3, UINT64_C(0x1), seq_table_gnb_cu_cp_tnl_a_to_upd_item_s_opts, 2},
    false,
    true,
    nullptr,
    0,
    seq_table_gnb_cu_cp_tnl_a_to_upd_item_s_names};

// GlobalMBSSessionID ::= SEQUENCE
static const asn1::table::seq_field seq_table_global_mbs_session_id_s_fields[] = {
    {offsetof(global_mbs_session_id_s, tmgi),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(global_mbs_session_id_s::tmgi), asn1::table::string_json>::value},
    {offsetof(global_mbs_session_id_s, nid),
     (int32_t)offsetof(global_mbs_session_id_s, nid_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(global_mbs_session_id_s::nid), asn1::table::struct_json>::value},
    {offsetof(global_mbs_session_id_s, ie_exts),
     (int32_t)offsetof(global_mbs_session_id_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(global_mbs_session_id_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_global_mbs_session_id_s_names[] = {{"tmgi", nullptr},
                                                                                  {"nid", nullptr},
                                                                                  {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_global_mbs_session_id_s_opts[]  = {
    {1, (int32_t)offsetof(global_mbs_session_id_s, nid_present)},
    {2, (int32_t)offsetof(global_mbs_session_id_s, ie_exts_present)}};
const asn1::table::seq_desc global_mbs_session_id_s::desc = {
    {seq_table_global_mbs_session_id_s_fields, 3, UINT64_C(0x1), seq_table_global_mbs_session_id_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_global_mbs_session_id_s_names};

// MBS-Support-Info-ToAdd-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_support_info_to_add_item_s_fields[] = {
    {offsetof(mbs_support_info_to_add_item_s, global_mbs_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_mbs_session_id_s::desc},
    {offsetof(mbs_support_info_to_add_item_s, ie_exts),
     (int32_t)offsetof(mbs_support_info_to_add_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_support_info_to_add_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mbs_support_info_to_add_item_s_names[] = {
    {"globalMBSSessionID", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mbs_support_info_to_add_item_s_opts[] = {
    {1, (int32_t)offsetof(mbs_support_info_to_add_item_s, ie_exts_present)}};
const asn1::table::seq_desc mbs_support_info_to_add_item_s::desc = {{seq_table_mbs_support_info_to_add_item_s_fields,
                                                                     2,
                                                                     UINT64_C(0x1),
                                                                     seq_table_mbs_support_info_to_add_item_s_opts,
                                                                     1},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_mbs_support_info_to_add_item_s_names};

// MBS-Support-Info-ToRemove-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_support_info_to_rem_item_s_fields[] = {
    {offsetof(mbs_support_info_to_rem_item_s, global_mbs_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_mbs_session_id_s::desc},
    {offsetof(mbs_support_info_to_rem_item_s, ie_exts),
     (int32_t)offsetof(mbs_support_info_to_rem_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_support_info_to_rem_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mbs_support_info_to_rem_item_s_names[] = {
    {"globalMBSSessionID", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mbs_support_info_to_rem_item_s_opts[] = {
    {1, (int32_t)offsetof(mbs_support_info_to_rem_item_s, ie_exts_present)}};
const asn1::table::seq_desc mbs_support_info_to_rem_item_s::desc = {{seq_table_mbs_support_info_to_rem_item_s_fields,
                                                                     2,
                                                                     UINT64_C(0x1),
                                                                     seq_table_mbs_support_info_to_rem_item_s_opts,
                                                                     1},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_mbs_support_info_to_rem_item_s_names};

// GNB-CU-UP-MBS-Support-Info ::= SEQUENCE
static const asn1::table::seq_field seq_table_gnb_cu_up_mbs_support_info_s_fields[] = {
    {offsetof(gnb_cu_up_mbs_support_info_s, mbs_support_info_to_add_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     512,
     &asn1::table::item_list_codec<decltype(gnb_cu_up_mbs_support_info_s::mbs_support_info_to_add_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mbs_support_info_to_add_item_s::desc},
    {offsetof(gnb_cu_up_mbs_support_info_s, mbs_support_info_to_rem_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     512,
     &asn1::table::item_list_codec<decltype(gnb_cu_up_mbs_support_info_s::mbs_support_info_to_rem_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mbs_support_info_to_rem_item_s::desc},
    {offsetof(gnb_cu_up_mbs_support_info_s, ie_exts),
     (int32_t)offsetof(gnb_cu_up_mbs_support_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gnb_cu_up_mbs_support_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_gnb_cu_up_mbs_support_info_s_names[] = {
    {"mbs-Support-Info-ToAdd-List", nullptr},
    {"mbs-Support-Info-ToRemove-List", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_gnb_cu_up_mbs_support_info_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(gnb_cu_up_mbs_support_info_s, ie_exts_present)}};
const asn1::table::seq_desc gnb_cu_up_mbs_support_info_s::desc = {
    {seq_table_gnb_cu_up_mbs_support_info_s_fields, 3, UINT64_C(0x0), seq_table_gnb_cu_up_mbs_support_info_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_gnb_cu_up_mbs_support_info_s_names};

// GNB-CU-UP-OverloadInformation ::= ENUMERATED
static const char*           enum_names_gnb_cu_up_overload_info_e_values[] = {"overloaded", "not-overloaded"};
const asn1::table::enum_desc gnb_cu_up_overload_info_opts::desc = {enum_names_gnb_cu_up_overload_info_e_values,
                                                                   2,
                                                                   "gnb_cu_up_overload_info_e"};

// GNB-CU-UP-TNLA-To-Remove-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_gnb_cu_up_tnl_a_to_rem_item_s_fields[] = {
    {offsetof(gnb_cu_up_tnl_a_to_rem_item_s, tnl_assoc_transport_layer_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cp_tnl_info_c::desc},
    {offsetof(gnb_cu_up_tnl_a_to_rem_item_s, tnl_assoc_transport_layer_address_gnb_cu_cp),
     (int32_t)offsetof(gnb_cu_up_tnl_a_to_rem_item_s, tnl_assoc_transport_layer_address_gnb_cu_cp_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cp_tnl_info_c::desc},
    {offsetof(gnb_cu_up_tnl_a_to_rem_item_s, ie_exts),
     (int32_t)offsetof(gnb_cu_up_tnl_a_to_rem_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gnb_cu_up_tnl_a_to_rem_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_gnb_cu_up_tnl_a_to_rem_item_s_names[] = {
    {"tNLAssociationTransportLayerAddress", nullptr},
    {"tNLAssociationTransportLayerAddressgNBCUCP", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_gnb_cu_up_tnl_a_to_rem_item_s_opts[] = {
    {1, (int32_t)offsetof(gnb_cu_up_tnl_a_to_rem_item_s, tnl_assoc_transport_layer_address_gnb_cu_cp_present)},
    {2, (int32_t)offsetof(gnb_cu_up_tnl_a_to_rem_item_s, ie_exts_present)}};
const asn1::table::seq_desc gnb_cu_up_tnl_a_to_rem_item_s::desc = {
    {seq_table_gnb_cu_up_tnl_a_to_rem_item_s_fields, 3, UINT64_C(0x1), seq_table_gnb_cu_up_tnl_a_to_rem_item_s_opts, 2},
    false,
    true,
    nullptr,
    0,
    seq_table_gnb_cu_up_tnl_a_to_rem_item_s_names};

// GTPTLA-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_gtp_tla_item_s_fields[] = {
    {offsetof(gtp_tla_item_s, gtp_transport_layer_addresses),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gtp_tla_item_s::gtp_transport_layer_addresses),
                                asn1::table::struct_json>::value},
    {offsetof(gtp_tla_item_s, ie_exts),
     (int32_t)offsetof(gtp_tla_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gtp_tla_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_gtp_tla_item_s_names[] = {{"gTPTransportLayerAddresses", nullptr},
                                                                         {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_gtp_tla_item_s_opts[]  = {
    {1, (int32_t)offsetof(gtp_tla_item_s, ie_exts_present)}};
const asn1::table::seq_desc gtp_tla_item_s::desc = {
    {seq_table_gtp_tla_item_s_fields, 2, UINT64_C(0x1), seq_table_gtp_tla_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_gtp_tla_item_s_names};

// HW-CapacityIndicator ::= SEQUENCE
static const asn1::table::seq_field seq_table_hw_capacity_ind_s_fields[] = {
    {offsetof(hw_capacity_ind_s, offered_throughput),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(16777216),
     nullptr},
    {offsetof(hw_capacity_ind_s, available_throughput),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(100),
     nullptr},
    {offsetof(hw_capacity_ind_s, ie_exts),
     (int32_t)offsetof(hw_capacity_ind_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(hw_capacity_ind_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_hw_capacity_ind_s_names[] = {{"offeredThroughput", nullptr},
                                                                            {"availableThroughput", nullptr},
                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_hw_capacity_ind_s_opts[]  = {
    {2, (int32_t)offsetof(hw_capacity_ind_s, ie_exts_present)}};
const asn1::table::seq_desc hw_capacity_ind_s::desc = {
    {seq_table_hw_capacity_ind_s_fields, 3, UINT64_C(0x3), seq_table_hw_capacity_ind_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_hw_capacity_ind_s_names};

// IAB-Donor-CU-UPPSKInfo-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_iab_donor_cu_up_psk_info_item_s_fields[] = {
    {offsetof(iab_donor_cu_up_psk_info_item_s, iab_donor_cu_up_psk),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(iab_donor_cu_up_psk_info_item_s::iab_donor_cu_up_psk),
                                asn1::table::string_json>::value},
    {offsetof(iab_donor_cu_up_psk_info_item_s, iab_donor_cu_up_ip_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(iab_donor_cu_up_psk_info_item_s::iab_donor_cu_up_ip_address),
                                asn1::table::struct_json>::value},
    {offsetof(iab_donor_cu_up_psk_info_item_s, iab_du_ip_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(iab_donor_cu_up_psk_info_item_s::iab_du_ip_address),
                                asn1::table::struct_json>::value},
    {offsetof(iab_donor_cu_up_psk_info_item_s, ie_exts),
     (int32_t)offsetof(iab_donor_cu_up_psk_info_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(iab_donor_cu_up_psk_info_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_iab_donor_cu_up_psk_info_item_s_names[] = {
    {"iAB-donor-CU-UPPSK", nullptr},
    {"iAB-donor-CU-UPIPAddress", nullptr},
    {"iAB-DUIPAddress", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_iab_donor_cu_up_psk_info_item_s_opts[] = {
    {3, (int32_t)offsetof(iab_donor_cu_up_psk_info_item_s, ie_exts_present)}};
const asn1::table::seq_desc iab_donor_cu_up_psk_info_item_s::desc = {{seq_table_iab_donor_cu_up_psk_info_item_s_fields,
                                                                      4,
                                                                      UINT64_C(0x7),
                                                                      seq_table_iab_donor_cu_up_psk_info_item_s_opts,
                                                                      1},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_iab_donor_cu_up_psk_info_item_s_names};

// M4period ::= ENUMERATED
static const char*           enum_names_m4period_e_values[] = {"ms1024", "ms2048", "ms5120", "ms10240", "min1"};
const asn1::table::enum_desc m4period_opts::desc            = {enum_names_m4period_e_values, 5, "m4period_e"};
uint16_t                     m4period_opts::to_number() const
{
  static const uint16_t numbers[] = {1024, 2048, 5120, 10240, 1};
  return map_enum_number(numbers, 5, value, "m4period_e");
}

// Links-to-log ::= ENUMERATED
static const char*           enum_names_links_to_log_e_values[] = {"uplink", "downlink", "both-uplink-and-downlink"};
const asn1::table::enum_desc links_to_log_opts::desc = {enum_names_links_to_log_e_values, 3, "links_to_log_e"};

// M4ReportAmount ::= ENUMERATED
static const char* enum_names_m4_report_amount_e_values[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
const asn1::table::enum_desc m4_report_amount_opts::desc  = {enum_names_m4_report_amount_e_values,
                                                             8,
                                                             "m4_report_amount_e"};
int8_t                       m4_report_amount_opts::to_number() const
{
  static const int8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(numbers, 8, value, "m4_report_amount_e");
}

static const uint32_t objset_id_m4_cfg_ext_ies_o[]   = {147};
static const uint8_t  objset_crit_m4_cfg_ext_ies_o[] = {crit_e::ignore};

// M4Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_m4_cfg_ext_ies_container_values[] = {
    {offsetof(m4_cfg_ext_ies_container, m4_report_amount),
     (int32_t)offsetof(m4_cfg_ext_ies_container, m4_report_amount_present),
     asn1::table::enum_op<decltype(m4_cfg_ext_ies_container::m4_report_amount)>(),
     decltype(m4_cfg_ext_ies_container::m4_report_amount)::has_ext,
     false,
     decltype(m4_cfg_ext_ies_container::m4_report_amount)::nof_types,
     decltype(m4_cfg_ext_ies_container::m4_report_amount)::nof_exts,
     nullptr,
     &m4_report_amount_opts::desc}};
static const int32_t ie_table_m4_cfg_ext_ies_container_presences[] = {
    (int32_t)offsetof(m4_cfg_ext_ies_container, m4_report_amount_present)};
const asn1::table::ie_container_desc m4_cfg_ext_ies_container::desc = {objset_id_m4_cfg_ext_ies_o,
                                                                       objset_crit_m4_cfg_ext_ies_o,
                                                                       ie_table_m4_cfg_ext_ies_container_values,
                                                                       ie_table_m4_cfg_ext_ies_container_presences,
                                                                       1,
                                                                       0,
                                                                       1,
                                                                       65535,
                                                                       true,
                                                                       "extensionValue"};

// M4Configuration ::= SEQUENCE
static const asn1::table::seq_field seq_table_m4_cfg_s_fields[] = {
    {offsetof(m4_cfg_s, m4period),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(m4_cfg_s::m4period)>(),
     decltype(m4_cfg_s::m4period)::has_ext,
     false,
     decltype(m4_cfg_s::m4period)::nof_types,
     decltype(m4_cfg_s::m4period)::nof_exts,
     nullptr,
     &m4period_opts::desc},
    {offsetof(m4_cfg_s, m4_links_to_log),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(m4_cfg_s::m4_links_to_log)>(),
     decltype(m4_cfg_s::m4_links_to_log)::has_ext,
     false,
     decltype(m4_cfg_s::m4_links_to_log)::nof_types,
     decltype(m4_cfg_s::m4_links_to_log)::nof_exts,
     nullptr,
     &links_to_log_opts::desc},
    {offsetof(m4_cfg_s, ie_exts),
     (int32_t)offsetof(m4_cfg_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &m4_cfg_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_m4_cfg_s_names[] = {{"m4period", nullptr},
                                                                   {"m4-links-to-log", nullptr},
                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_m4_cfg_s_opts[]  = {{2, (int32_t)offsetof(m4_cfg_s, ie_exts_present)}};
const asn1::table::seq_desc m4_cfg_s::desc = {{seq_table_m4_cfg_s_fields, 3, UINT64_C(0x3), seq_table_m4_cfg_s_opts, 1},
                                              true,
                                              true,
                                              nullptr,
                                              0,
                                              seq_table_m4_cfg_s_names};

// M6report-Interval ::= ENUMERATED
static const char*           enum_names_m6report_interv_e_values[] = {"ms120",
                                                                      "ms240",
                                                                      "ms480",
                                                                      "ms640",
                                                                      "ms1024",
                                                                      "ms2048",
                                                                      "ms5120",
                                                                      "ms10240",
                                                                      "ms20480",
                                                                      "ms40960",
                                                                      "min1",
                                                                      "min6",
                                                                      "min12",
                                                                      "min30"};
const asn1::table::enum_desc m6report_interv_opts::desc            = {enum_names_m6report_interv_e_values,
                                                                      14,
                                                                      "m6report_interv_e"};
uint16_t                     m6report_interv_opts::to_number() const
{
  static const uint16_t numbers[] = {120, 240, 480, 640, 1024, 2048, 5120, 10240, 20480, 40960, 1, 6, 12, 30};
  return map_enum_number(numbers, 14, value, "m6report_interv_e");
}

// M6ReportAmount ::= ENUMERATED
static const char* enum_names_m6_report_amount_e_values[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
const asn1::table::enum_desc m6_report_amount_opts::desc  = {enum_names_m6_report_amount_e_values,
                                                             8,
                                                             "m6_report_amount_e"};
int8_t                       m6_report_amount_opts::to_number() const
{
  static const int8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(numbers, 8, value, "m6_report_amount_e");
}

static const uint32_t objset_id_m6_cfg_ext_ies_o[]   = {148};
static const uint8_t  objset_crit_m6_cfg_ext_ies_o[] = {crit_e::ignore};

// M6Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_m6_cfg_ext_ies_container_values[] = {
    {offsetof(m6_cfg_ext_ies_container, m6_report_amount),
     (int32_t)offsetof(m6_cfg_ext_ies_container, m6_report_amount_present),
     asn1::table::enum_op<decltype(m6_cfg_ext_ies_container::m6_report_amount)>(),
     decltype(m6_cfg_ext_ies_container::m6_report_amount)::has_ext,
     false,
     decltype(m6_cfg_ext_ies_container::m6_report_amount)::nof_types,
     decltype(m6_cfg_ext_ies_container::m6_report_amount)::nof_exts,
     nullptr,
     &m6_report_amount_opts::desc}};
static const int32_t ie_table_m6_cfg_ext_ies_container_presences[] = {
    (int32_t)offsetof(m6_cfg_ext_ies_container, m6_report_amount_present)};
const asn1::table::ie_container_desc m6_cfg_ext_ies_container::desc = {objset_id_m6_cfg_ext_ies_o,
                                                                       objset_crit_m6_cfg_ext_ies_o,
                                                                       ie_table_m6_cfg_ext_ies_container_values,
                                                                       ie_table_m6_cfg_ext_ies_container_presences,
                                                                       1,
                                                                       0,
                                                                       1,
                                                                       65535,
                                                                       true,
                                                                       "extensionValue"};

// M6Configuration ::= SEQUENCE
static const asn1::table::seq_field seq_table_m6_cfg_s_fields[] = {
    {offsetof(m6_cfg_s, m6report_interv),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(m6_cfg_s::m6report_interv)>(),
     decltype(m6_cfg_s::m6report_interv)::has_ext,
     false,
     decltype(m6_cfg_s::m6report_interv)::nof_types,
     decltype(m6_cfg_s::m6report_interv)::nof_exts,
     nullptr,
     &m6report_interv_opts::desc},
    {offsetof(m6_cfg_s, m6_links_to_log),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(m6_cfg_s::m6_links_to_log)>(),
     decltype(m6_cfg_s::m6_links_to_log)::has_ext,
     false,
     decltype(m6_cfg_s::m6_links_to_log)::nof_types,
     decltype(m6_cfg_s::m6_links_to_log)::nof_exts,
     nullptr,
     &links_to_log_opts::desc},
    {offsetof(m6_cfg_s, ie_exts),
     (int32_t)offsetof(m6_cfg_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &m6_cfg_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_m6_cfg_s_names[] = {{"m6report-Interval", nullptr},
                                                                   {"m6-links-to-log", nullptr},
                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_m6_cfg_s_opts[]  = {{2, (int32_t)offsetof(m6_cfg_s, ie_exts_present)}};
const asn1::table::seq_desc m6_cfg_s::desc = {{seq_table_m6_cfg_s_fields, 3, UINT64_C(0x3), seq_table_m6_cfg_s_opts, 1},
                                              true,
                                              true,
                                              nullptr,
                                              0,
                                              seq_table_m6_cfg_s_names};

// M7ReportAmount ::= ENUMERATED
static const char* enum_names_m7_report_amount_e_values[] = {"r1", "r2", "r4", "r8", "r16", "r32", "r64", "infinity"};
const asn1::table::enum_desc m7_report_amount_opts::desc  = {enum_names_m7_report_amount_e_values,
                                                             8,
                                                             "m7_report_amount_e"};
int8_t                       m7_report_amount_opts::to_number() const
{
  static const int8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, -1};
  return map_enum_number(numbers, 8, value, "m7_report_amount_e");
}

static const uint32_t objset_id_m7_cfg_ext_ies_o[]   = {149};
static const uint8_t  objset_crit_m7_cfg_ext_ies_o[] = {crit_e::ignore};

// M7Configuration-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_m7_cfg_ext_ies_container_values[] = {
    {offsetof(m7_cfg_ext_ies_container, m7_report_amount),
     (int32_t)offsetof(m7_cfg_ext_ies_container, m7_report_amount_present),
     asn1::table::enum_op<decltype(m7_cfg_ext_ies_container::m7_report_amount)>(),
     decltype(m7_cfg_ext_ies_container::m7_report_amount)::has_ext,
     false,
     decltype(m7_cfg_ext_ies_container::m7_report_amount)::nof_types,
     decltype(m7_cfg_ext_ies_container::m7_report_amount)::nof_exts,
     nullptr,
     &m7_report_amount_opts::desc}};
static const int32_t ie_table_m7_cfg_ext_ies_container_presences[] = {
    (int32_t)offsetof(m7_cfg_ext_ies_container, m7_report_amount_present)};
const asn1::table::ie_container_desc m7_cfg_ext_ies_container::desc = {objset_id_m7_cfg_ext_ies_o,
                                                                       objset_crit_m7_cfg_ext_ies_o,
                                                                       ie_table_m7_cfg_ext_ies_container_values,
                                                                       ie_table_m7_cfg_ext_ies_container_presences,
                                                                       1,
                                                                       0,
                                                                       1,
                                                                       65535,
                                                                       true,
                                                                       "extensionValue"};

// M7Configuration ::= SEQUENCE
static const asn1::table::seq_field seq_table_m7_cfg_s_fields[] = {
    {offsetof(m7_cfg_s, m7period),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(1),
     UINT64_C(60),
     nullptr},
    {offsetof(m7_cfg_s, m7_links_to_log),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(m7_cfg_s::m7_links_to_log)>(),
     decltype(m7_cfg_s::m7_links_to_log)::has_ext,
     false,
     decltype(m7_cfg_s::m7_links_to_log)::nof_types,
     decltype(m7_cfg_s::m7_links_to_log)::nof_exts,
     nullptr,
     &links_to_log_opts::desc},
    {offsetof(m7_cfg_s, ie_exts),
     (int32_t)offsetof(m7_cfg_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &m7_cfg_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_m7_cfg_s_names[] = {{"m7period", nullptr},
                                                                   {"m7-links-to-log", nullptr},
                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_m7_cfg_s_opts[]  = {{2, (int32_t)offsetof(m7_cfg_s, ie_exts_present)}};
const asn1::table::seq_desc m7_cfg_s::desc = {{seq_table_m7_cfg_s_fields, 3, UINT64_C(0x3), seq_table_m7_cfg_s_opts, 1},
                                              true,
                                              true,
                                              nullptr,
                                              0,
                                              seq_table_m7_cfg_s_names};

// ImmediateMDT ::= SEQUENCE
static const asn1::table::seq_field seq_table_immediate_mdt_s_fields[] = {
    {offsetof(immediate_mdt_s, meass_to_activ),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(immediate_mdt_s::meass_to_activ), asn1::table::struct_json>::value},
    {offsetof(immediate_mdt_s, meas_four),
     (int32_t)offsetof(immediate_mdt_s, meas_four_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &m4_cfg_s::desc},
    {offsetof(immediate_mdt_s, meas_six),
     (int32_t)offsetof(immediate_mdt_s, meas_six_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &m6_cfg_s::desc},
    {offsetof(immediate_mdt_s, meas_seven),
     (int32_t)offsetof(immediate_mdt_s, meas_seven_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &m7_cfg_s::desc},
    {offsetof(immediate_mdt_s, ie_exts),
     (int32_t)offsetof(immediate_mdt_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(immediate_mdt_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_immediate_mdt_s_names[] = {{"measurementsToActivate", nullptr},
                                                                          {"measurementFour", nullptr},
                                                                          {"measurementSix", nullptr},
                                                                          {"measurementSeven", nullptr},
                                                                          {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_immediate_mdt_s_opts[]  = {
    {1, (int32_t)offsetof(immediate_mdt_s, meas_four_present)},
    {2, (int32_t)offsetof(immediate_mdt_s, meas_six_present)},
    {3, (int32_t)offsetof(immediate_mdt_s, meas_seven_present)},
    {4, (int32_t)offsetof(immediate_mdt_s, ie_exts_present)}};
const asn1::table::seq_desc immediate_mdt_s::desc = {
    {seq_table_immediate_mdt_s_fields, 5, UINT64_C(0x1), seq_table_immediate_mdt_s_opts, 4},
    true,
    true,
    nullptr,
    0,
    seq_table_immediate_mdt_s_names};

// InactivityInformationRequest ::= ENUMERATED
static const char*           enum_names_inactivity_info_request_e_values[] = {"true"};
const asn1::table::enum_desc inactivity_info_request_opts::desc = {enum_names_inactivity_info_request_e_values,
                                                                   1,
                                                                   "inactivity_info_request_e"};

// IntegrityProtectionAlgorithm ::= ENUMERATED
static const char*           enum_names_integrity_protection_algorithm_e_values[] = {"nIA0",
                                                                                     "i-128-NIA1",
                                                                                     "i-128-NIA2",
                                                                                     "i-128-NIA3"};
const asn1::table::enum_desc integrity_protection_algorithm_opts::desc            = {
    enum_names_integrity_protection_algorithm_e_values,
    4,
    "integrity_protection_algorithm_e"};

// MBS-DL-Data-Arrival ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_dl_data_arrival_s_fields[] = {
    {offsetof(mbs_dl_data_arrival_s, dl_data_arrival),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mbs_dl_data_arrival_s::dl_data_arrival)>(),
     decltype(mbs_dl_data_arrival_s::dl_data_arrival)::has_ext,
     false,
     decltype(mbs_dl_data_arrival_s::dl_data_arrival)::nof_types,
     decltype(mbs_dl_data_arrival_s::dl_data_arrival)::nof_exts,
     nullptr,
     &mbs_dl_data_arrival_s::dl_data_arrival_opts::desc},
    {offsetof(mbs_dl_data_arrival_s, ppi),
     (int32_t)offsetof(mbs_dl_data_arrival_s, ppi_present),
     asn1::table::field_op::int_u8,
     true,
     true,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(mbs_dl_data_arrival_s, ie_exts),
     (int32_t)offsetof(mbs_dl_data_arrival_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_dl_data_arrival_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mbs_dl_data_arrival_s_names[] = {{"dlDataArrival", nullptr},
                                                                                {"ppi", nullptr},
                                                                                {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mbs_dl_data_arrival_s_opts[]  = {
    {1, (int32_t)offsetof(mbs_dl_data_arrival_s, ppi_present)},
    {2, (int32_t)offsetof(mbs_dl_data_arrival_s, ie_exts_present)}};
const asn1::table::seq_desc mbs_dl_data_arrival_s::desc = {
    {seq_table_mbs_dl_data_arrival_s_fields, 3, UINT64_C(0x1), seq_table_mbs_dl_data_arrival_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_mbs_dl_data_arrival_s_names};

static const char*           enum_names_mbs_dl_data_arrival_s_dl_data_arrival_e__values[] = {"true"};
const asn1::table::enum_desc mbs_dl_data_arrival_s::dl_data_arrival_opts::desc            = {
    enum_names_mbs_dl_data_arrival_s_dl_data_arrival_e__values,
    1,
    "mbs_dl_data_arrival_s::dl_data_arrival_e_"};

// MBS-PDCP-COUNT-Req ::= ENUMERATED
static const char*           enum_names_mbs_pdcp_count_req_e_values[] = {"true"};
const asn1::table::enum_desc mbs_pdcp_count_req_opts::desc            = {enum_names_mbs_pdcp_count_req_e_values,
                                                                         1,
                                                                         "mbs_pdcp_count_req_e"};

// MBS-ServiceAreaTAIList-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_service_area_tai_list_item_s_fields[] = {
    {offsetof(mbs_service_area_tai_list_item_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_service_area_tai_list_item_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(mbs_service_area_tai_list_item_s, five_gs_tac),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_service_area_tai_list_item_s::five_gs_tac),
                                asn1::table::string_json>::value},
    {offsetof(mbs_service_area_tai_list_item_s, ie_exts),
     (int32_t)offsetof(mbs_service_area_tai_list_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_service_area_tai_list_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mbs_service_area_tai_list_item_s_names[] = {{"plmn-ID", nullptr},
                                                                                           {"fiveGS-TAC", nullptr},
                                                                                           {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mbs_service_area_tai_list_item_s_opts[]  = {
    {2, (int32_t)offsetof(mbs_service_area_tai_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc mbs_service_area_tai_list_item_s::desc = {
    {seq_table_mbs_service_area_tai_list_item_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_mbs_service_area_tai_list_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_mbs_service_area_tai_list_item_s_names};

// MBS-ServiceAreaInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_service_area_info_s_fields[] = {
    {offsetof(mbs_service_area_info_s, mbs_service_area_cell_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     512,
     &asn1::table::item_list_codec<decltype(mbs_service_area_info_s::mbs_service_area_cell_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_cgi_s::desc},
    {offsetof(mbs_service_area_info_s, mbs_service_area_tai_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     512,
     &asn1::table::item_list_codec<decltype(mbs_service_area_info_s::mbs_service_area_tai_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mbs_service_area_tai_list_item_s::desc},
    {offsetof(mbs_service_area_info_s, ie_exts),
     (int32_t)offsetof(mbs_service_area_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_service_area_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mbs_service_area_info_s_names[] = {{"mBS-ServiceAreaCellList", nullptr},
                                                                                  {"mBS-ServiceAreaTAIList", nullptr},
                                                                                  {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mbs_service_area_info_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(mbs_service_area_info_s, ie_exts_present)}};
const asn1::table::seq_desc mbs_service_area_info_s::desc = {
    {seq_table_mbs_service_area_info_s_fields, 3, UINT64_C(0x0), seq_table_mbs_service_area_info_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_mbs_service_area_info_s_names};

// MBS-ServiceAreaInformationItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_service_area_info_item_s_fields[] = {
    {offsetof(mbs_service_area_info_item_s, mbs_area_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(mbs_service_area_info_item_s, mbs_service_area_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_service_area_info_s::desc},
    {offsetof(mbs_service_area_info_item_s, ie_exts),
     (int32_t)offsetof(mbs_service_area_info_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_service_area_info_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mbs_service_area_info_item_s_names[] = {
    {"mBS-AreaSessionID", nullptr},
    {"mBS-ServiceAreaInformation", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mbs_service_area_info_item_s_opts[] = {
    {2, (int32_t)offsetof(mbs_service_area_info_item_s, ie_exts_present)}};
const asn1::table::seq_desc mbs_service_area_info_item_s::desc = {
    {seq_table_mbs_service_area_info_item_s_fields, 3, UINT64_C(0x3), seq_table_mbs_service_area_info_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_mbs_service_area_info_item_s_names};

// MBS-ServiceArea ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_service_area_s_fields[] = {
    {offsetof(mbs_service_area_s, mbs_service_area_info_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     256,
     &asn1::table::item_list_codec<decltype(mbs_service_area_s::mbs_service_area_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mbs_service_area_info_item_s::desc},
    {offsetof(mbs_service_area_s, ie_exts),
     (int32_t)offsetof(mbs_service_area_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_service_area_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mbs_service_area_s_names[] = {
    {"mBS-ServiceAreaInformationList", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mbs_service_area_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(mbs_service_area_s, ie_exts_present)}};
const asn1::table::seq_desc mbs_service_area_s::desc = {
    {seq_table_mbs_service_area_s_fields, 2, UINT64_C(0x0), seq_table_mbs_service_area_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_mbs_service_area_s_names};

// MBSMulticastF1UContextDescriptor ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_multicast_f1_u_context_descriptor_s_fields[] = {
    {offsetof(mbs_multicast_f1_u_context_descriptor_s, multicast_f1_u_context_ref_e1),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_multicast_f1_u_context_descriptor_s::multicast_f1_u_context_ref_e1),
                                asn1::table::string_json>::value},
    {offsetof(mbs_multicast_f1_u_context_descriptor_s, mc_f1_u_ctxtusage),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mbs_multicast_f1_u_context_descriptor_s::mc_f1_u_ctxtusage)>(),
     decltype(mbs_multicast_f1_u_context_descriptor_s::mc_f1_u_ctxtusage)::has_ext,
     false,
     decltype(mbs_multicast_f1_u_context_descriptor_s::mc_f1_u_ctxtusage)::nof_types,
     decltype(mbs_multicast_f1_u_context_descriptor_s::mc_f1_u_ctxtusage)::nof_exts,
     nullptr,
     &mbs_multicast_f1_u_context_descriptor_s::mc_f1_u_ctxtusage_opts::desc},
    {offsetof(mbs_multicast_f1_u_context_descriptor_s, mbs_area_session),
     (int32_t)offsetof(mbs_multicast_f1_u_context_descriptor_s, mbs_area_session_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(mbs_multicast_f1_u_context_descriptor_s, ie_exts),
     (int32_t)offsetof(mbs_multicast_f1_u_context_descriptor_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_multicast_f1_u_context_descriptor_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mbs_multicast_f1_u_context_descriptor_s_names[] = {
    {"multicastF1UContextReferenceE1", nullptr},
    {"mc-F1UCtxtusage", nullptr},
    {"mbsAreaSession", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mbs_multicast_f1_u_context_descriptor_s_opts[] = {
    {2, (int32_t)offsetof(mbs_multicast_f1_u_context_descriptor_s, mbs_area_session_present)},
    {3, (int32_t)offsetof(mbs_multicast_f1_u_context_descriptor_s, ie_exts_present)}};
const asn1::table::seq_desc mbs_multicast_f1_u_context_descriptor_s::desc = {
    {seq_table_mbs_multicast_f1_u_context_descriptor_s_fields,
     4,
     UINT64_C(0x3),
     seq_table_mbs_multicast_f1_u_context_descriptor_s_opts,
     2},
    true,
    true,
    nullptr,
    0,
    seq_table_mbs_multicast_f1_u_context_descriptor_s_names};

static const char* enum_names_mbs_multicast_f1_u_context_descriptor_s_mc_f1_u_ctxtusage_e__values[] =
    {"ptm", "ptp", "ptp-retransmission", "ptp-forwarding"};
const asn1::table::enum_desc mbs_multicast_f1_u_context_descriptor_s::mc_f1_u_ctxtusage_opts::desc = {
    enum_names_mbs_multicast_f1_u_context_descriptor_s_mc_f1_u_ctxtusage_e__values,
    4,
    "mbs_multicast_f1_u_context_descriptor_s::mc_f1_u_ctxtusage_e_"};

// MBSSessionAssociatedInformation-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_session_associated_info_item_s_fields[] = {
    {offsetof(mbs_session_associated_info_item_s, mbs_qos_flow_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(mbs_session_associated_info_item_s, associated_unicast_qos_flow_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(mbs_session_associated_info_item_s, ie_exts),
     (int32_t)offsetof(mbs_session_associated_info_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_session_associated_info_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mbs_session_associated_info_item_s_names[] = {
    {"mbs-QoS-Flow-Identifier", nullptr},
    {"associated-unicast-QoS-Flow-Identifier", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mbs_session_associated_info_item_s_opts[] = {
    {2, (int32_t)offsetof(mbs_session_associated_info_item_s, ie_exts_present)}};
const asn1::table::seq_desc mbs_session_associated_info_item_s::desc = {
    {seq_table_mbs_session_associated_info_item_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_mbs_session_associated_info_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_mbs_session_associated_info_item_s_names};

// MBSSessionAssociatedInfoNonSupportToSupport ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_session_associated_info_non_support_to_support_s_fields[] = {
    {offsetof(mbs_session_associated_info_non_support_to_support_s, ue_ref_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     INT64_C(0),
     UINT64_C(4294967295),
     nullptr},
    {offsetof(mbs_session_associated_info_non_support_to_support_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(mbs_session_associated_info_non_support_to_support_s, associated_qos_flow_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<
         decltype(mbs_session_associated_info_non_support_to_support_s::associated_qos_flow_info_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &mbs_session_associated_info_item_s::desc},
    {offsetof(mbs_session_associated_info_non_support_to_support_s, ie_exts),
     (int32_t)offsetof(mbs_session_associated_info_non_support_to_support_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_session_associated_info_non_support_to_support_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mbs_session_associated_info_non_support_to_support_s_names[] = {
    {"ue-Reference-ID", nullptr},
    {"pDU-Session-ID", nullptr},
    {"associatedQoSFlowInformationList", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mbs_session_associated_info_non_support_to_support_s_opts[] = {
    {3, (int32_t)offsetof(mbs_session_associated_info_non_support_to_support_s, ie_exts_present)}};
const asn1::table::seq_desc mbs_session_associated_info_non_support_to_support_s::desc = {
    {seq_table_mbs_session_associated_info_non_support_to_support_s_fields,
     4,
     UINT64_C(0x7),
     seq_table_mbs_session_associated_info_non_support_to_support_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_mbs_session_associated_info_non_support_to_support_s_names};

// MBSSessionAssociatedInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_session_associated_info_s_fields[] = {
    {offsetof(mbs_session_associated_info_s, mbs_session_associated_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(mbs_session_associated_info_s::mbs_session_associated_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mbs_session_associated_info_item_s::desc},
    {offsetof(mbs_session_associated_info_s, mbs_session_forwarding_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(mbs_session_associated_info_s, ie_exts),
     (int32_t)offsetof(mbs_session_associated_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_session_associated_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mbs_session_associated_info_s_names[] = {
    {"mbsSessionAssociatedInformationList", nullptr},
    {"mbsSessionForwardingAddress", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mbs_session_associated_info_s_opts[] = {
    {2, (int32_t)offsetof(mbs_session_associated_info_s, ie_exts_present)}};
const asn1::table::seq_desc mbs_session_associated_info_s::desc = {
    {seq_table_mbs_session_associated_info_s_fields, 3, UINT64_C(0x3), seq_table_mbs_session_associated_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_mbs_session_associated_info_s_names};

// MCBearerContext-Inactivity ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_bearer_context_inactivity_s_fields[] = {
    {offsetof(mc_bearer_context_inactivity_s, mc_bearer_context_inactivity_ind),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mc_bearer_context_inactivity_s::mc_bearer_context_inactivity_ind)>(),
     decltype(mc_bearer_context_inactivity_s::mc_bearer_context_inactivity_ind)::has_ext,
     false,
     decltype(mc_bearer_context_inactivity_s::mc_bearer_context_inactivity_ind)::nof_types,
     decltype(mc_bearer_context_inactivity_s::mc_bearer_context_inactivity_ind)::nof_exts,
     nullptr,
     &mc_bearer_context_inactivity_s::mc_bearer_context_inactivity_ind_opts::desc},
    {offsetof(mc_bearer_context_inactivity_s, ie_exts),
     (int32_t)offsetof(mc_bearer_context_inactivity_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_bearer_context_inactivity_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mc_bearer_context_inactivity_s_names[] = {
    {"mcBearerContext-Inactivity-Indication", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mc_bearer_context_inactivity_s_opts[] = {
    {1, (int32_t)offsetof(mc_bearer_context_inactivity_s, ie_exts_present)}};
const asn1::table::seq_desc mc_bearer_context_inactivity_s::desc = {{seq_table_mc_bearer_context_inactivity_s_fields,
                                                                     2,
                                                                     UINT64_C(0x1),
                                                                     seq_table_mc_bearer_context_inactivity_s_opts,
                                                                     1},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_mc_bearer_context_inactivity_s_names};

static const char* enum_names_mc_bearer_context_inactivity_s_mc_bearer_context_inactivity_ind_e__values[] = {"true"};
const asn1::table::enum_desc mc_bearer_context_inactivity_s::mc_bearer_context_inactivity_ind_opts::desc  = {
    enum_names_mc_bearer_context_inactivity_s_mc_bearer_context_inactivity_ind_e__values,
    1,
    "mc_bearer_context_inactivity_s::mc_bearer_context_inactivity_ind_e_"};

// MBSSessionResourceNotification ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_mbs_session_res_notif_c_alternatives[] = {
    &asn1::table::alternative_ops<mbs_dl_data_arrival_s, false>::value,
    &asn1::table::alternative_ops<mc_bearer_context_inactivity_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<mbs_session_res_notif_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc mbs_session_res_notif_c::storage_desc = {
    choice_storage_mbs_session_res_notif_c_alternatives,
    3,
    "mbs_session_res_notif_c"};

mbs_dl_data_arrival_s& mbs_session_res_notif_c::set_mbs_dl_data_arrival()
{
  set(types::mbs_dl_data_arrival);
  return c.get<mbs_dl_data_arrival_s>();
}
mc_bearer_context_inactivity_s& mbs_session_res_notif_c::set_inactivity()
{
  set(types::inactivity);
  return c.get<mc_bearer_context_inactivity_s>();
}
protocol_ie_single_container_s<mbs_session_res_notif_ext_ies_o>& mbs_session_res_notif_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<mbs_session_res_notif_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_mbs_session_res_notif_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_dl_data_arrival_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_inactivity_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<mbs_session_res_notif_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_mbs_session_res_notif_c_names[] = {{"mbs-DL-Data-Arrival", nullptr},
                                                                                     {"inactivity", nullptr},
                                                                                     {"choice-extension", nullptr}};
const asn1::table::choice_desc       mbs_session_res_notif_c::desc                = {
    asn1::table::enum_int_type<mbs_session_res_notif_c::types>(),
    mbs_session_res_notif_c::types::nof_types,
    mbs_session_res_notif_c::types::nof_exts,
    mbs_session_res_notif_c::types::has_ext,
    true,
    choice_table_mbs_session_res_notif_c_alternatives,
    nullptr,
    choice_table_mbs_session_res_notif_c_names,
    "mbs_session_res_notif_c",
    offsetof(mbs_session_res_notif_c, c.type),
    offsetof(mbs_session_res_notif_c, c.value),
    false,
    &mbs_session_res_notif_c::storage_desc};

static const char*           enum_names_mbs_session_res_notif_c_types_values[] = {"mbs-DL-Data-Arrival",
                                                                                  "inactivity",
                                                                                  "choice-extension"};
const asn1::table::enum_desc mbs_session_res_notif_c::types_opts::desc         = {
    enum_names_mbs_session_res_notif_c_types_values,
    3,
    "mbs_session_res_notif_c::types"};

// MCBearerContextF1UTNLInfoatDU ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_bearer_context_f1_u_tnl_infoat_du_s_fields[] = {
    {offsetof(mc_bearer_context_f1_u_tnl_infoat_du_s, mbs_f1_u_infoat_du),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(mc_bearer_context_f1_u_tnl_infoat_du_s, mbs_multicast_f1_u_context_descriptor),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_multicast_f1_u_context_descriptor_s::desc},
    {offsetof(mc_bearer_context_f1_u_tnl_infoat_du_s, ie_exts),
     (int32_t)offsetof(mc_bearer_context_f1_u_tnl_infoat_du_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_bearer_context_f1_u_tnl_infoat_du_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mc_bearer_context_f1_u_tnl_infoat_du_s_names[] = {
    {"mbsF1UInfoatDU", nullptr},
    {"mbsMulticastF1UContextDescriptor", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mc_bearer_context_f1_u_tnl_infoat_du_s_opts[] = {
    {2, (int32_t)offsetof(mc_bearer_context_f1_u_tnl_infoat_du_s, ie_exts_present)}};
const asn1::table::seq_desc mc_bearer_context_f1_u_tnl_infoat_du_s::desc = {
    {seq_table_mc_bearer_context_f1_u_tnl_infoat_du_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_mc_bearer_context_f1_u_tnl_infoat_du_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_mc_bearer_context_f1_u_tnl_infoat_du_s_names};

// MCBearerContextNGU-TNLInfoatNGRAN ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_mc_bearer_context_ngu_tnl_infoat_ngran_c_alternatives[] = {
    &asn1::table::alternative_ops<mb_sn_gu_info_at_ngran_c, false>::value,
    &asn1::table::alternative_ops<location_dependent_mb_sn_gu_info_at_ngran_l, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<mc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>,
                                  false>::value};
const asn1::table::choice_storage_desc mc_bearer_context_ngu_tnl_infoat_ngran_c::storage_desc = {
    choice_storage_mc_bearer_context_ngu_tnl_infoat_ngran_c_alternatives,
    3,
    "mc_bearer_context_ngu_tnl_infoat_ngran_c"};

mb_sn_gu_info_at_ngran_c& mc_bearer_context_ngu_tnl_infoat_ngran_c::set_locationindependent()
{
  set(types::locationindependent);
  return c.get<mb_sn_gu_info_at_ngran_c>();
}
location_dependent_mb_sn_gu_info_at_ngran_l& mc_bearer_context_ngu_tnl_infoat_ngran_c::set_locationdependent()
{
  set(types::locationdependent);
  return c.get<location_dependent_mb_sn_gu_info_at_ngran_l>();
}
protocol_ie_single_container_s<mc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>&
mc_bearer_context_ngu_tnl_infoat_ngran_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<mc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_mc_bearer_context_ngu_tnl_infoat_ngran_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mb_sn_gu_info_at_ngran_c::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     256,
     &asn1::table::list_codec<location_dependent_mb_sn_gu_info_at_ngran_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<mc_bearer_context_ngu_tnl_infoat_ngran_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_mc_bearer_context_ngu_tnl_infoat_ngran_c_names[] = {
    {"locationindependent", nullptr},
    {"locationdependent", nullptr},
    {"choice-extension", nullptr}};
const asn1::table::choice_desc mc_bearer_context_ngu_tnl_infoat_ngran_c::desc = {
    asn1::table::enum_int_type<mc_bearer_context_ngu_tnl_infoat_ngran_c::types>(),
    mc_bearer_context_ngu_tnl_infoat_ngran_c::types::nof_types,
    mc_bearer_context_ngu_tnl_infoat_ngran_c::types::nof_exts,
    mc_bearer_context_ngu_tnl_infoat_ngran_c::types::has_ext,
    true,
    choice_table_mc_bearer_context_ngu_tnl_infoat_ngran_c_alternatives,
    nullptr,
    choice_table_mc_bearer_context_ngu_tnl_infoat_ngran_c_names,
    "mc_bearer_context_ngu_tnl_infoat_ngran_c",
    offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_c, c.type),
    offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_c, c.value),
    false,
    &mc_bearer_context_ngu_tnl_infoat_ngran_c::storage_desc};

static const char* enum_names_mc_bearer_context_ngu_tnl_infoat_ngran_c_types_values[]   = {"locationindependent",
                                                                                           "locationdependent",
                                                                                           "choice-extension"};
const asn1::table::enum_desc mc_bearer_context_ngu_tnl_infoat_ngran_c::types_opts::desc = {
    enum_names_mc_bearer_context_ngu_tnl_infoat_ngran_c_types_values,
    3,
    "mc_bearer_context_ngu_tnl_infoat_ngran_c::types"};

// MCBearerContextNGU-TNLInfoatNGRANModifyResponse ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s_fields[] = {
    {offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s, mbs_ngu_infoat_ngran),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mb_sn_gu_info_at_ngran_c::desc},
    {offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s, mbs_area_session),
     (int32_t)offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s, mbs_area_session_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s, ie_exts),
     (int32_t)offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s_names[] = {
    {"mbs-NGU-InfoatNGRAN", nullptr},
    {"mbsAreaSession", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s_opts[] = {
    {1, (int32_t)offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s, mbs_area_session_present)},
    {2, (int32_t)offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s, ie_exts_present)}};
const asn1::table::seq_desc mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s::desc = {
    {seq_table_mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s_fields,
     3,
     UINT64_C(0x1),
     seq_table_mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s_opts,
     2},
    true,
    true,
    nullptr,
    0,
    seq_table_mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s_names};

// MCBearerContextNGUTNLInfoat5GC ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_bearer_context_ngu_tnl_infoat5_gc_s_fields[] = {
    {offsetof(mc_bearer_context_ngu_tnl_infoat5_gc_s, mbs_ngu_info_at5_gc),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mb_sn_gu_info_at5_gc_c::desc},
    {offsetof(mc_bearer_context_ngu_tnl_infoat5_gc_s, mbs_area_session_id),
     (int32_t)offsetof(mc_bearer_context_ngu_tnl_infoat5_gc_s, mbs_area_session_id_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(mc_bearer_context_ngu_tnl_infoat5_gc_s, ie_exts),
     (int32_t)offsetof(mc_bearer_context_ngu_tnl_infoat5_gc_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_bearer_context_ngu_tnl_infoat5_gc_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mc_bearer_context_ngu_tnl_infoat5_gc_s_names[] = {
    {"mbsNGUInformationAt5GC", nullptr},
    {"mbsAreaSession-ID", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mc_bearer_context_ngu_tnl_infoat5_gc_s_opts[] = {
    {1, (int32_t)offsetof(mc_bearer_context_ngu_tnl_infoat5_gc_s, mbs_area_session_id_present)},
    {2, (int32_t)offsetof(mc_bearer_context_ngu_tnl_infoat5_gc_s, ie_exts_present)}};
const asn1::table::seq_desc mc_bearer_context_ngu_tnl_infoat5_gc_s::desc = {
    {seq_table_mc_bearer_context_ngu_tnl_infoat5_gc_s_fields,
     3,
     UINT64_C(0x1),
     seq_table_mc_bearer_context_ngu_tnl_infoat5_gc_s_opts,
     2},
    true,
    true,
    nullptr,
    0,
    seq_table_mc_bearer_context_ngu_tnl_infoat5_gc_s_names};

// MCBearerContextNGUTnlInfoatNGRANRequest ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_bearer_context_ngu_tnl_infoat_ngran_request_s_fields[] = {
    {offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_request_s, ng_ranngu_tnl_requested),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mc_bearer_context_ngu_tnl_infoat_ngran_request_s::ng_ranngu_tnl_requested)>(),
     decltype(mc_bearer_context_ngu_tnl_infoat_ngran_request_s::ng_ranngu_tnl_requested)::has_ext,
     false,
     decltype(mc_bearer_context_ngu_tnl_infoat_ngran_request_s::ng_ranngu_tnl_requested)::nof_types,
     decltype(mc_bearer_context_ngu_tnl_infoat_ngran_request_s::ng_ranngu_tnl_requested)::nof_exts,
     nullptr,
     &mc_bearer_context_ngu_tnl_infoat_ngran_request_s::ng_ranngu_tnl_requested_opts::desc},
    {offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_request_s, mbs_area_session_id),
     (int32_t)offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_request_s, mbs_area_session_id_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_request_s, ie_exts),
     (int32_t)offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_request_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_bearer_context_ngu_tnl_infoat_ngran_request_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mc_bearer_context_ngu_tnl_infoat_ngran_request_s_names[] = {
    {"ngRANNGUTNLRequested", nullptr},
    {"mbsAreaSession-ID", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mc_bearer_context_ngu_tnl_infoat_ngran_request_s_opts[] = {
    {1, (int32_t)offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_request_s, mbs_area_session_id_present)},
    {2, (int32_t)offsetof(mc_bearer_context_ngu_tnl_infoat_ngran_request_s, ie_exts_present)}};
const asn1::table::seq_desc mc_bearer_context_ngu_tnl_infoat_ngran_request_s::desc = {
    {seq_table_mc_bearer_context_ngu_tnl_infoat_ngran_request_s_fields,
     3,
     UINT64_C(0x1),
     seq_table_mc_bearer_context_ngu_tnl_infoat_ngran_request_s_opts,
     2},
    true,
    true,
    nullptr,
    0,
    seq_table_mc_bearer_context_ngu_tnl_infoat_ngran_request_s_names};

static const char* enum_names_mc_bearer_context_ngu_tnl_infoat_ngran_request_s_ng_ranngu_tnl_requested_e__values[] = {
    "requested"};
const asn1::table::enum_desc mc_bearer_context_ngu_tnl_infoat_ngran_request_s::ng_ranngu_tnl_requested_opts::desc = {
    enum_names_mc_bearer_context_ngu_tnl_infoat_ngran_request_s_ng_ranngu_tnl_requested_e__values,
    1,
    "mc_bearer_context_ngu_tnl_infoat_ngran_request_s::ng_ranngu_tnl_requested_e_"};

// MCBearerContextStatusChange ::= ENUMERATED
static const char*           enum_names_mc_bearer_context_status_change_e_values[] = {"suspend", "resume"};
const asn1::table::enum_desc mc_bearer_context_status_change_opts::desc            = {
    enum_names_mc_bearer_context_status_change_e_values,
    2,
    "mc_bearer_context_status_change_e"};

// MCMRBSetupModifyConfiguration-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mcmrb_setup_modify_cfg_item_s_fields[] = {
    {offsetof(mcmrb_setup_modify_cfg_item_s, mrb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(mcmrb_setup_modify_cfg_item_s, f1u_tnl_at_du),
     (int32_t)offsetof(mcmrb_setup_modify_cfg_item_s, f1u_tnl_at_du_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_f1_u_tnl_infoat_du_s::desc},
    {offsetof(mcmrb_setup_modify_cfg_item_s, mbs_pdcp_cfg),
     (int32_t)offsetof(mcmrb_setup_modify_cfg_item_s, mbs_pdcp_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_cfg_s::desc},
    {offsetof(mcmrb_setup_modify_cfg_item_s, qos_flow_qos_param_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(mcmrb_setup_modify_cfg_item_s::qos_flow_qos_param_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_qos_param_item_s::desc},
    {offsetof(mcmrb_setup_modify_cfg_item_s, mrb_qos),
     (int32_t)offsetof(mcmrb_setup_modify_cfg_item_s, mrb_qos_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &qos_flow_level_qos_params_s::desc},
    {offsetof(mcmrb_setup_modify_cfg_item_s, mbs_pdcp_count_req),
     (int32_t)offsetof(mcmrb_setup_modify_cfg_item_s, mbs_pdcp_count_req_present),
     asn1::table::enum_op<decltype(mcmrb_setup_modify_cfg_item_s::mbs_pdcp_count_req)>(),
     decltype(mcmrb_setup_modify_cfg_item_s::mbs_pdcp_count_req)::has_ext,
     false,
     decltype(mcmrb_setup_modify_cfg_item_s::mbs_pdcp_count_req)::nof_types,
     decltype(mcmrb_setup_modify_cfg_item_s::mbs_pdcp_count_req)::nof_exts,
     nullptr,
     &mbs_pdcp_count_req_opts::desc},
    {offsetof(mcmrb_setup_modify_cfg_item_s, ie_exts),
     (int32_t)offsetof(mcmrb_setup_modify_cfg_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mcmrb_setup_modify_cfg_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mcmrb_setup_modify_cfg_item_s_names[] = {
    {"mrb-ID", nullptr},
    {"f1uTNLatDU", nullptr},
    {"mbs-pdcp-config", nullptr},
    {"qoS-Flow-QoS-Parameter-List", nullptr},
    {"mrbQoS", nullptr},
    {"mbs-PDCP-COUNT-Req", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mcmrb_setup_modify_cfg_item_s_opts[] = {
    {1, (int32_t)offsetof(mcmrb_setup_modify_cfg_item_s, f1u_tnl_at_du_present)},
    {2, (int32_t)offsetof(mcmrb_setup_modify_cfg_item_s, mbs_pdcp_cfg_present)},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(mcmrb_setup_modify_cfg_item_s, mrb_qos_present)},
    {5, (int32_t)offsetof(mcmrb_setup_modify_cfg_item_s, mbs_pdcp_count_req_present)},
    {6, (int32_t)offsetof(mcmrb_setup_modify_cfg_item_s, ie_exts_present)}};
const asn1::table::seq_desc mcmrb_setup_modify_cfg_item_s::desc = {
    {seq_table_mcmrb_setup_modify_cfg_item_s_fields, 7, UINT64_C(0x1), seq_table_mcmrb_setup_modify_cfg_item_s_opts, 6},
    true,
    true,
    nullptr,
    0,
    seq_table_mcmrb_setup_modify_cfg_item_s_names};

// MRB-ProgressInformationType ::= ENUMERATED
static const char*           enum_names_mrb_progress_info_type_e_values[] = {"oldest-available", "last-delivered"};
const asn1::table::enum_desc mrb_progress_info_type_opts::desc            = {enum_names_mrb_progress_info_type_e_values,
                                                                             2,
                                                                             "mrb_progress_info_type_e"};

// MRBForwardingResourceRequest-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mrb_forwarding_res_request_item_s_fields[] = {
    {offsetof(mrb_forwarding_res_request_item_s, mrb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(mrb_forwarding_res_request_item_s, mrb_progress_request_type),
     (int32_t)offsetof(mrb_forwarding_res_request_item_s, mrb_progress_request_type_present),
     asn1::table::enum_op<decltype(mrb_forwarding_res_request_item_s::mrb_progress_request_type)>(),
     decltype(mrb_forwarding_res_request_item_s::mrb_progress_request_type)::has_ext,
     false,
     decltype(mrb_forwarding_res_request_item_s::mrb_progress_request_type)::nof_types,
     decltype(mrb_forwarding_res_request_item_s::mrb_progress_request_type)::nof_exts,
     nullptr,
     &mrb_progress_info_type_opts::desc},
    {offsetof(mrb_forwarding_res_request_item_s, mrb_forwarding_address_request),
     (int32_t)offsetof(mrb_forwarding_res_request_item_s, mrb_forwarding_address_request_present),
     asn1::table::enum_op<decltype(mrb_forwarding_res_request_item_s::mrb_forwarding_address_request)>(),
     decltype(mrb_forwarding_res_request_item_s::mrb_forwarding_address_request)::has_ext,
     false,
     decltype(mrb_forwarding_res_request_item_s::mrb_forwarding_address_request)::nof_types,
     decltype(mrb_forwarding_res_request_item_s::mrb_forwarding_address_request)::nof_exts,
     nullptr,
     &mrb_forwarding_res_request_item_s::mrb_forwarding_address_request_opts::desc},
    {offsetof(mrb_forwarding_res_request_item_s, ie_exts),
     (int32_t)offsetof(mrb_forwarding_res_request_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mrb_forwarding_res_request_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mrb_forwarding_res_request_item_s_names[] = {
    {"mrb-ID", nullptr},
    {"mrbProgressRequestType", nullptr},
    {"mrbForwardingAddressRequest", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mrb_forwarding_res_request_item_s_opts[] = {
    {1, (int32_t)offsetof(mrb_forwarding_res_request_item_s, mrb_progress_request_type_present)},
    {2, (int32_t)offsetof(mrb_forwarding_res_request_item_s, mrb_forwarding_address_request_present)},
    {3, (int32_t)offsetof(mrb_forwarding_res_request_item_s, ie_exts_present)}};
const asn1::table::seq_desc mrb_forwarding_res_request_item_s::desc = {
    {seq_table_mrb_forwarding_res_request_item_s_fields,
     4,
     UINT64_C(0x1),
     seq_table_mrb_forwarding_res_request_item_s_opts,
     3},
    true,
    true,
    nullptr,
    0,
    seq_table_mrb_forwarding_res_request_item_s_names};

static const char* enum_names_mrb_forwarding_res_request_item_s_mrb_forwarding_address_request_e__values[] = {
    "request"};
const asn1::table::enum_desc mrb_forwarding_res_request_item_s::mrb_forwarding_address_request_opts::desc = {
    enum_names_mrb_forwarding_res_request_item_s_mrb_forwarding_address_request_e__values,
    1,
    "mrb_forwarding_res_request_item_s::mrb_forwarding_address_request_e_"};

// MCForwardingResourceRequest ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_forwarding_res_request_s_fields[] = {
    {offsetof(mc_forwarding_res_request_s, mc_forwarding_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_forwarding_res_request_s::mc_forwarding_res_id),
                                asn1::table::string_json>::value},
    {offsetof(mc_forwarding_res_request_s, mbs_area_session_id),
     (int32_t)offsetof(mc_forwarding_res_request_s, mbs_area_session_id_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(mc_forwarding_res_request_s, mrb_forwarding_res_request_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(mc_forwarding_res_request_s::mrb_forwarding_res_request_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mrb_forwarding_res_request_item_s::desc},
    {offsetof(mc_forwarding_res_request_s, ie_exts),
     (int32_t)offsetof(mc_forwarding_res_request_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_forwarding_res_request_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mc_forwarding_res_request_s_names[] = {
    {"mcForwardingResourceID", nullptr},
    {"mbsAreaSession-ID", nullptr},
    {"mrbForwardingResourceRequestList", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mc_forwarding_res_request_s_opts[] = {
    {1, (int32_t)offsetof(mc_forwarding_res_request_s, mbs_area_session_id_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(mc_forwarding_res_request_s, ie_exts_present)}};
const asn1::table::seq_desc mc_forwarding_res_request_s::desc = {
    {seq_table_mc_forwarding_res_request_s_fields, 4, UINT64_C(0x1), seq_table_mc_forwarding_res_request_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_mc_forwarding_res_request_s_names};

// MRB-ProgressInformationSNs ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_mrb_progress_info_sn_s_c_alternatives[] = {
    &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint32_t, true>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<mrb_progress_info_sn_s_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc mrb_progress_info_sn_s_c::storage_desc = {
    choice_storage_mrb_progress_info_sn_s_c_alternatives,
    3,
    "mrb_progress_info_sn_s_c"};

uint16_t& mrb_progress_info_sn_s_c::set_pdcp_sn12()
{
  set(types::pdcp_sn12);
  return c.get<uint16_t>();
}
uint32_t& mrb_progress_info_sn_s_c::set_pdcp_sn18()
{
  set(types::pdcp_sn18);
  return c.get<uint32_t>();
}
protocol_ie_single_container_s<mrb_progress_info_sn_s_ext_ies_o>& mrb_progress_info_sn_s_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<mrb_progress_info_sn_s_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_mrb_progress_info_sn_s_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(4095),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     INT64_C(0),
     UINT64_C(262143),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<mrb_progress_info_sn_s_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_mrb_progress_info_sn_s_c_names[] = {{"pdcp-SN12", nullptr},
                                                                                      {"pdcp-SN18", nullptr},
                                                                                      {"choice-extension", nullptr}};
const asn1::table::choice_desc       mrb_progress_info_sn_s_c::desc                = {
    asn1::table::enum_int_type<mrb_progress_info_sn_s_c::types>(),
    mrb_progress_info_sn_s_c::types::nof_types,
    mrb_progress_info_sn_s_c::types::nof_exts,
    mrb_progress_info_sn_s_c::types::has_ext,
    true,
    choice_table_mrb_progress_info_sn_s_c_alternatives,
    nullptr,
    choice_table_mrb_progress_info_sn_s_c_names,
    "mrb_progress_info_sn_s_c",
    offsetof(mrb_progress_info_sn_s_c, c.type),
    offsetof(mrb_progress_info_sn_s_c, c.value),
    false,
    &mrb_progress_info_sn_s_c::storage_desc};

static const char* enum_names_mrb_progress_info_sn_s_c_types_values[] = {"pdcp-SN12", "pdcp-SN18", "choice-extension"};
const asn1::table::enum_desc mrb_progress_info_sn_s_c::types_opts::desc = {
    enum_names_mrb_progress_info_sn_s_c_types_values,
    3,
    "mrb_progress_info_sn_s_c::types"};
uint8_t mrb_progress_info_sn_s_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {12, 18};
  return map_enum_number(numbers, 2, value, "mrb_progress_info_sn_s_c::types");
}

// MRB-ProgressInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_mrb_progress_info_s_fields[] = {
    {offsetof(mrb_progress_info_s, mrb_progress_info_sn_s),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mrb_progress_info_sn_s_c::desc},
    {offsetof(mrb_progress_info_s, mrb_progress_info_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mrb_progress_info_s::mrb_progress_info_type)>(),
     decltype(mrb_progress_info_s::mrb_progress_info_type)::has_ext,
     false,
     decltype(mrb_progress_info_s::mrb_progress_info_type)::nof_types,
     decltype(mrb_progress_info_s::mrb_progress_info_type)::nof_exts,
     nullptr,
     &mrb_progress_info_type_opts::desc},
    {offsetof(mrb_progress_info_s, ie_exts),
     (int32_t)offsetof(mrb_progress_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mrb_progress_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mrb_progress_info_s_names[] = {{"mrb-ProgressInformationSNs", nullptr},
                                                                              {"mrb-ProgressInformationType", nullptr},
                                                                              {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mrb_progress_info_s_opts[]  = {
    {2, (int32_t)offsetof(mrb_progress_info_s, ie_exts_present)}};
const asn1::table::seq_desc mrb_progress_info_s::desc = {
    {seq_table_mrb_progress_info_s_fields, 3, UINT64_C(0x3), seq_table_mrb_progress_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_mrb_progress_info_s_names};

// MRBForwardingResourceIndication-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mrb_forwarding_res_ind_item_s_fields[] = {
    {offsetof(mrb_forwarding_res_ind_item_s, mrb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(mrb_forwarding_res_ind_item_s, mrb_progress_info),
     (int32_t)offsetof(mrb_forwarding_res_ind_item_s, mrb_progress_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mrb_progress_info_s::desc},
    {offsetof(mrb_forwarding_res_ind_item_s, mrb_forwarding_address),
     (int32_t)offsetof(mrb_forwarding_res_ind_item_s, mrb_forwarding_address_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(mrb_forwarding_res_ind_item_s, ie_exts),
     (int32_t)offsetof(mrb_forwarding_res_ind_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mrb_forwarding_res_ind_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mrb_forwarding_res_ind_item_s_names[] = {
    {"mrb-ID", nullptr},
    {"mrb-ProgressInformation", nullptr},
    {"mrbForwardingAddress", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mrb_forwarding_res_ind_item_s_opts[] = {
    {1, (int32_t)offsetof(mrb_forwarding_res_ind_item_s, mrb_progress_info_present)},
    {2, (int32_t)offsetof(mrb_forwarding_res_ind_item_s, mrb_forwarding_address_present)},
    {3, (int32_t)offsetof(mrb_forwarding_res_ind_item_s, ie_exts_present)}};
const asn1::table::seq_desc mrb_forwarding_res_ind_item_s::desc = {
    {seq_table_mrb_forwarding_res_ind_item_s_fields, 4, UINT64_C(0x1), seq_table_mrb_forwarding_res_ind_item_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_mrb_forwarding_res_ind_item_s_names};

// MCForwardingResourceIndication ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_forwarding_res_ind_s_fields[] = {
    {offsetof(mc_forwarding_res_ind_s, mc_forwarding_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_forwarding_res_ind_s::mc_forwarding_res_id),
                                asn1::table::string_json>::value},
    {offsetof(mc_forwarding_res_ind_s, mrb_forwarding_res_ind_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(mc_forwarding_res_ind_s::mrb_forwarding_res_ind_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mrb_forwarding_res_ind_item_s::desc},
    {offsetof(mc_forwarding_res_ind_s, mbs_session_associated_info),
     (int32_t)offsetof(mc_forwarding_res_ind_s, mbs_session_associated_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_session_associated_info_s::desc},
    {offsetof(mc_forwarding_res_ind_s, ie_exts),
     (int32_t)offsetof(mc_forwarding_res_ind_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_forwarding_res_ind_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mc_forwarding_res_ind_s_names[] = {
    {"mcForwardingResourceID", nullptr},
    {"mrbForwardingResourceIndicationList", nullptr},
    {"mbsSessionAssociatedInformation", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mc_forwarding_res_ind_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(mc_forwarding_res_ind_s, mbs_session_associated_info_present)},
    {3, (int32_t)offsetof(mc_forwarding_res_ind_s, ie_exts_present)}};
const asn1::table::seq_desc mc_forwarding_res_ind_s::desc = {
    {seq_table_mc_forwarding_res_ind_s_fields, 4, UINT64_C(0x1), seq_table_mc_forwarding_res_ind_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_mc_forwarding_res_ind_s_names};

// MCForwardingResourceRelease ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_forwarding_res_release_s_fields[] = {
    {offsetof(mc_forwarding_res_release_s, mc_forwarding_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_forwarding_res_release_s::mc_forwarding_res_id),
                                asn1::table::string_json>::value},
    {offsetof(mc_forwarding_res_release_s, ie_exts),
     (int32_t)offsetof(mc_forwarding_res_release_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_forwarding_res_release_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mc_forwarding_res_release_s_names[] = {
    {"mcForwardingResourceID", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mc_forwarding_res_release_s_opts[] = {
    {1, (int32_t)offsetof(mc_forwarding_res_release_s, ie_exts_present)}};
const asn1::table::seq_desc mc_forwarding_res_release_s::desc = {
    {seq_table_mc_forwarding_res_release_s_fields, 2, UINT64_C(0x1), seq_table_mc_forwarding_res_release_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_mc_forwarding_res_release_s_names};

static const uint32_t objset_id_mc_bearer_context_to_modify_ext_ies_o[] = {179, 180, 182, 185, 192, 193};
static const uint8_t  objset_crit_mc_bearer_context_to_modify_ext_ies_o[] =
    {crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};

// MCBearerContextToModify-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_mc_bearer_context_to_modify_ext_ies_container_values[] = {
    {offsetof(mc_bearer_context_to_modify_ext_ies_container, mc_forwarding_res_request),
     (int32_t)offsetof(mc_bearer_context_to_modify_ext_ies_container, mc_forwarding_res_request_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_forwarding_res_request_s::desc},
    {offsetof(mc_bearer_context_to_modify_ext_ies_container, mc_forwarding_res_ind),
     (int32_t)offsetof(mc_bearer_context_to_modify_ext_ies_container, mc_forwarding_res_ind_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_forwarding_res_ind_s::desc},
    {offsetof(mc_bearer_context_to_modify_ext_ies_container, mc_forwarding_res_release),
     (int32_t)offsetof(mc_bearer_context_to_modify_ext_ies_container, mc_forwarding_res_release_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_forwarding_res_release_s::desc},
    {offsetof(mc_bearer_context_to_modify_ext_ies_container, mbs_session_associated_info_non_support_to_support),
     (int32_t)offsetof(mc_bearer_context_to_modify_ext_ies_container,
                       mbs_session_associated_info_non_support_to_support_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_session_associated_info_non_support_to_support_s::desc},
    {offsetof(mc_bearer_context_to_modify_ext_ies_container, mc_bearer_context_inactivity_timer),
     (int32_t)offsetof(mc_bearer_context_to_modify_ext_ies_container, mc_bearer_context_inactivity_timer_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(7200),
     nullptr},
    {offsetof(mc_bearer_context_to_modify_ext_ies_container, mc_bearer_context_status_change),
     (int32_t)offsetof(mc_bearer_context_to_modify_ext_ies_container, mc_bearer_context_status_change_present),
     asn1::table::enum_op<decltype(mc_bearer_context_to_modify_ext_ies_container::mc_bearer_context_status_change)>(),
     decltype(mc_bearer_context_to_modify_ext_ies_container::mc_bearer_context_status_change)::has_ext,
     false,
     decltype(mc_bearer_context_to_modify_ext_ies_container::mc_bearer_context_status_change)::nof_types,
     decltype(mc_bearer_context_to_modify_ext_ies_container::mc_bearer_context_status_change)::nof_exts,
     nullptr,
     &mc_bearer_context_status_change_opts::desc}};
static const int32_t ie_table_mc_bearer_context_to_modify_ext_ies_container_presences[] = {
    (int32_t)offsetof(mc_bearer_context_to_modify_ext_ies_container, mc_forwarding_res_request_present),
    (int32_t)offsetof(mc_bearer_context_to_modify_ext_ies_container, mc_forwarding_res_ind_present),
    (int32_t)offsetof(mc_bearer_context_to_modify_ext_ies_container, mc_forwarding_res_release_present),
    (int32_t)offsetof(mc_bearer_context_to_modify_ext_ies_container,
                      mbs_session_associated_info_non_support_to_support_present),
    (int32_t)offsetof(mc_bearer_context_to_modify_ext_ies_container, mc_bearer_context_inactivity_timer_present),
    (int32_t)offsetof(mc_bearer_context_to_modify_ext_ies_container, mc_bearer_context_status_change_present)};
const asn1::table::ie_container_desc mc_bearer_context_to_modify_ext_ies_container::desc = {
    objset_id_mc_bearer_context_to_modify_ext_ies_o,
    objset_crit_mc_bearer_context_to_modify_ext_ies_o,
    ie_table_mc_bearer_context_to_modify_ext_ies_container_values,
    ie_table_mc_bearer_context_to_modify_ext_ies_container_presences,
    6,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// MCBearerContextToModify ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_bearer_context_to_modify_s_fields[] = {
    {offsetof(mc_bearer_context_to_modify_s, mc_bearer_context_ngu_tnl_infoat5_gc),
     (int32_t)offsetof(mc_bearer_context_to_modify_s, mc_bearer_context_ngu_tnl_infoat5_gc_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_ngu_tnl_infoat5_gc_s::desc},
    {offsetof(mc_bearer_context_to_modify_s, mc_bearer_context_ngu_tnl_infoat_ngran_request),
     (int32_t)offsetof(mc_bearer_context_to_modify_s, mc_bearer_context_ngu_tnl_infoat_ngran_request_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_ngu_tnl_infoat_ngran_request_s::desc},
    {offsetof(mc_bearer_context_to_modify_s, mbs_multicast_f1_u_context_descriptor),
     (int32_t)offsetof(mc_bearer_context_to_modify_s, mbs_multicast_f1_u_context_descriptor_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_multicast_f1_u_context_descriptor_s::desc},
    {offsetof(mc_bearer_context_to_modify_s, requested_action),
     (int32_t)offsetof(mc_bearer_context_to_modify_s, requested_action_present),
     asn1::table::enum_op<decltype(mc_bearer_context_to_modify_s::requested_action)>(),
     decltype(mc_bearer_context_to_modify_s::requested_action)::has_ext,
     false,
     decltype(mc_bearer_context_to_modify_s::requested_action)::nof_types,
     decltype(mc_bearer_context_to_modify_s::requested_action)::nof_exts,
     nullptr,
     &requested_action4_avail_ngu_termination_opts::desc},
    {offsetof(mc_bearer_context_to_modify_s, mc_mrb_to_setup_modify_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(mc_bearer_context_to_modify_s::mc_mrb_to_setup_modify_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mcmrb_setup_modify_cfg_item_s::desc},
    {offsetof(mc_bearer_context_to_modify_s, mc_mrb_to_rem_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::list_codec<decltype(mc_bearer_context_to_modify_s::mc_mrb_to_rem_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(512), true, true>>::value},
    {offsetof(mc_bearer_context_to_modify_s, ie_exts),
     (int32_t)offsetof(mc_bearer_context_to_modify_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_to_modify_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_mc_bearer_context_to_modify_s_names[] = {
    {"mcBearerContextNGUTNLInfoat5GC", nullptr},
    {"mcBearerContextNGUTnlInfoatNGRANRequest", nullptr},
    {"mbsMulticastF1UContextDescriptor", nullptr},
    {"requestedAction", nullptr},
    {"mcMRBToSetupModifyList", nullptr},
    {"mcMRBToRemoveList", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mc_bearer_context_to_modify_s_opts[] = {
    {0, (int32_t)offsetof(mc_bearer_context_to_modify_s, mc_bearer_context_ngu_tnl_infoat5_gc_present)},
    {1, (int32_t)offsetof(mc_bearer_context_to_modify_s, mc_bearer_context_ngu_tnl_infoat_ngran_request_present)},
    {2, (int32_t)offsetof(mc_bearer_context_to_modify_s, mbs_multicast_f1_u_context_descriptor_present)},
    {3, (int32_t)offsetof(mc_bearer_context_to_modify_s, requested_action_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence},
    {6, (int32_t)offsetof(mc_bearer_context_to_modify_s, ie_exts_present)}};
const asn1::table::seq_desc mc_bearer_context_to_modify_s::desc = {
    {seq_table_mc_bearer_context_to_modify_s_fields, 7, UINT64_C(0x0), seq_table_mc_bearer_context_to_modify_s_opts, 7},
    true,
    true,
    nullptr,
    0,
    seq_table_mc_bearer_context_to_modify_s_names};

// MCMRBModifyConfirmList-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mcmrb_modify_confirm_list_item_s_fields[] = {
    {offsetof(mcmrb_modify_confirm_list_item_s, mrb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(mcmrb_modify_confirm_list_item_s, ie_exts),
     (int32_t)offsetof(mcmrb_modify_confirm_list_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mcmrb_modify_confirm_list_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mcmrb_modify_confirm_list_item_s_names[] = {{"mrb-ID", nullptr},
                                                                                           {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mcmrb_modify_confirm_list_item_s_opts[]  = {
    {1, (int32_t)offsetof(mcmrb_modify_confirm_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc mcmrb_modify_confirm_list_item_s::desc = {
    {seq_table_mcmrb_modify_confirm_list_item_s_fields,
     2,
     UINT64_C(0x1),
     seq_table_mcmrb_modify_confirm_list_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_mcmrb_modify_confirm_list_item_s_names};

// MCBearerContextToModifyConfirm ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_bearer_context_to_modify_confirm_s_fields[] = {
    {offsetof(mc_bearer_context_to_modify_confirm_s, mbs_multicast_f1_u_context_descriptor),
     (int32_t)offsetof(mc_bearer_context_to_modify_confirm_s, mbs_multicast_f1_u_context_descriptor_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_multicast_f1_u_context_descriptor_s::desc},
    {offsetof(mc_bearer_context_to_modify_confirm_s, mc_mrb_modify_confirm_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(mc_bearer_context_to_modify_confirm_s::mc_mrb_modify_confirm_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mcmrb_modify_confirm_list_item_s::desc},
    {offsetof(mc_bearer_context_to_modify_confirm_s, ie_exts),
     (int32_t)offsetof(mc_bearer_context_to_modify_confirm_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_bearer_context_to_modify_confirm_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mc_bearer_context_to_modify_confirm_s_names[] = {
    {"mbsMulticastF1UContextDescriptor", nullptr},
    {"mcMRBModifyConfirmList", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mc_bearer_context_to_modify_confirm_s_opts[] = {
    {0, (int32_t)offsetof(mc_bearer_context_to_modify_confirm_s, mbs_multicast_f1_u_context_descriptor_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(mc_bearer_context_to_modify_confirm_s, ie_exts_present)}};
const asn1::table::seq_desc mc_bearer_context_to_modify_confirm_s::desc = {
    {seq_table_mc_bearer_context_to_modify_confirm_s_fields,
     3,
     UINT64_C(0x0),
     seq_table_mc_bearer_context_to_modify_confirm_s_opts,
     3},
    true,
    true,
    nullptr,
    0,
    seq_table_mc_bearer_context_to_modify_confirm_s_names};

// MCMRBModifyRequiredConfiguration-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mcmrb_modify_required_cfg_item_s_fields[] = {
    {offsetof(mcmrb_modify_required_cfg_item_s, mrb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(mcmrb_modify_required_cfg_item_s, mbs_pdcp_count),
     (int32_t)offsetof(mcmrb_modify_required_cfg_item_s, mbs_pdcp_count_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mcmrb_modify_required_cfg_item_s::mbs_pdcp_count),
                                asn1::table::struct_json>::value},
    {offsetof(mcmrb_modify_required_cfg_item_s, ie_exts),
     (int32_t)offsetof(mcmrb_modify_required_cfg_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mcmrb_modify_required_cfg_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mcmrb_modify_required_cfg_item_s_names[] = {{"mrb-ID", nullptr},
                                                                                           {"mBS-PDCP-COUNT", nullptr},
                                                                                           {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mcmrb_modify_required_cfg_item_s_opts[]  = {
    {1, (int32_t)offsetof(mcmrb_modify_required_cfg_item_s, mbs_pdcp_count_present)},
    {2, (int32_t)offsetof(mcmrb_modify_required_cfg_item_s, ie_exts_present)}};
const asn1::table::seq_desc mcmrb_modify_required_cfg_item_s::desc = {
    {seq_table_mcmrb_modify_required_cfg_item_s_fields,
     3,
     UINT64_C(0x1),
     seq_table_mcmrb_modify_required_cfg_item_s_opts,
     2},
    true,
    true,
    nullptr,
    0,
    seq_table_mcmrb_modify_required_cfg_item_s_names};

// MCForwardingResourceReleaseIndication ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_forwarding_res_release_ind_s_fields[] = {
    {offsetof(mc_forwarding_res_release_ind_s, mc_forwarding_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_forwarding_res_release_ind_s::mc_forwarding_res_id),
                                asn1::table::string_json>::value},
    {offsetof(mc_forwarding_res_release_ind_s, ie_exts),
     (int32_t)offsetof(mc_forwarding_res_release_ind_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_forwarding_res_release_ind_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mc_forwarding_res_release_ind_s_names[] = {
    {"mcForwardingResourceID", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mc_forwarding_res_release_ind_s_opts[] = {
    {1, (int32_t)offsetof(mc_forwarding_res_release_ind_s, ie_exts_present)}};
const asn1::table::seq_desc mc_forwarding_res_release_ind_s::desc = {{seq_table_mc_forwarding_res_release_ind_s_fields,
                                                                      2,
                                                                      UINT64_C(0x1),
                                                                      seq_table_mc_forwarding_res_release_ind_s_opts,
                                                                      1},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_mc_forwarding_res_release_ind_s_names};

static const uint32_t objset_id_mc_bearer_context_to_modify_required_ext_ies_o[]   = {183};
static const uint8_t  objset_crit_mc_bearer_context_to_modify_required_ext_ies_o[] = {crit_e::ignore};

// MCBearerContextToModifyRequired-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_mc_bearer_context_to_modify_required_ext_ies_container_values[] = {
    {offsetof(mc_bearer_context_to_modify_required_ext_ies_container, mc_forwarding_res_release_ind),
     (int32_t)offsetof(mc_bearer_context_to_modify_required_ext_ies_container, mc_forwarding_res_release_ind_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_forwarding_res_release_ind_s::desc}};
static const int32_t ie_table_mc_bearer_context_to_modify_required_ext_ies_container_presences[] = {
    (int32_t)offsetof(mc_bearer_context_to_modify_required_ext_ies_container, mc_forwarding_res_release_ind_present)};
const asn1::table::ie_container_desc mc_bearer_context_to_modify_required_ext_ies_container::desc = {
    objset_id_mc_bearer_context_to_modify_required_ext_ies_o,
    objset_crit_mc_bearer_context_to_modify_required_ext_ies_o,
    ie_table_mc_bearer_context_to_modify_required_ext_ies_container_values,
    ie_table_mc_bearer_context_to_modify_required_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// MCBearerContextToModifyRequired ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_bearer_context_to_modify_required_s_fields[] = {
    {offsetof(mc_bearer_context_to_modify_required_s, mbs_multicast_f1_u_context_descriptor),
     (int32_t)offsetof(mc_bearer_context_to_modify_required_s, mbs_multicast_f1_u_context_descriptor_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_multicast_f1_u_context_descriptor_s::desc},
    {offsetof(mc_bearer_context_to_modify_required_s, mc_mrb_to_rem_required_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::list_codec<decltype(mc_bearer_context_to_modify_required_s::mc_mrb_to_rem_required_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(512), true, true>>::value},
    {offsetof(mc_bearer_context_to_modify_required_s, mc_mrb_to_modify_required_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(mc_bearer_context_to_modify_required_s::mc_mrb_to_modify_required_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mcmrb_modify_required_cfg_item_s::desc},
    {offsetof(mc_bearer_context_to_modify_required_s, ie_exts),
     (int32_t)offsetof(mc_bearer_context_to_modify_required_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_to_modify_required_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_mc_bearer_context_to_modify_required_s_names[] = {
    {"mbsMulticastF1UContextDescriptor", nullptr},
    {"mcMRBToRemoveRequiredList", nullptr},
    {"mcMRBToModifyRequiredList", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mc_bearer_context_to_modify_required_s_opts[] = {
    {0, (int32_t)offsetof(mc_bearer_context_to_modify_required_s, mbs_multicast_f1_u_context_descriptor_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(mc_bearer_context_to_modify_required_s, ie_exts_present)}};
const asn1::table::seq_desc mc_bearer_context_to_modify_required_s::desc = {
    {seq_table_mc_bearer_context_to_modify_required_s_fields,
     4,
     UINT64_C(0x0),
     seq_table_mc_bearer_context_to_modify_required_s_opts,
     4},
    true,
    true,
    nullptr,
    0,
    seq_table_mc_bearer_context_to_modify_required_s_names};

// MCMRBSetupModifyResponseList-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mcmrb_setup_modify_resp_list_item_s_fields[] = {
    {offsetof(mcmrb_setup_modify_resp_list_item_s, mrb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(mcmrb_setup_modify_resp_list_item_s, qosflow_setup),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(mcmrb_setup_modify_resp_list_item_s::qosflow_setup),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_item_s::desc},
    {offsetof(mcmrb_setup_modify_resp_list_item_s, qosflow_failed),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(mcmrb_setup_modify_resp_list_item_s::qosflow_failed),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_failed_item_s::desc},
    {offsetof(mcmrb_setup_modify_resp_list_item_s, mc_bearer_context_f1_u_tnl_infoat_cu),
     (int32_t)offsetof(mcmrb_setup_modify_resp_list_item_s, mc_bearer_context_f1_u_tnl_infoat_cu_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(mcmrb_setup_modify_resp_list_item_s, mbs_pdcp_count),
     (int32_t)offsetof(mcmrb_setup_modify_resp_list_item_s, mbs_pdcp_count_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mcmrb_setup_modify_resp_list_item_s::mbs_pdcp_count),
                                asn1::table::struct_json>::value},
    {offsetof(mcmrb_setup_modify_resp_list_item_s, ie_exts),
     (int32_t)offsetof(mcmrb_setup_modify_resp_list_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mcmrb_setup_modify_resp_list_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mcmrb_setup_modify_resp_list_item_s_names[] = {
    {"mrb-ID", nullptr},
    {"qosflow-setup", nullptr},
    {"qosflow-failed", nullptr},
    {"mcBearerContextF1UTNLInfoatCU", nullptr},
    {"mBS-PDCP-COUNT", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mcmrb_setup_modify_resp_list_item_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(mcmrb_setup_modify_resp_list_item_s, mc_bearer_context_f1_u_tnl_infoat_cu_present)},
    {4, (int32_t)offsetof(mcmrb_setup_modify_resp_list_item_s, mbs_pdcp_count_present)},
    {5, (int32_t)offsetof(mcmrb_setup_modify_resp_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc mcmrb_setup_modify_resp_list_item_s::desc = {
    {seq_table_mcmrb_setup_modify_resp_list_item_s_fields,
     6,
     UINT64_C(0x1),
     seq_table_mcmrb_setup_modify_resp_list_item_s_opts,
     5},
    true,
    true,
    nullptr,
    0,
    seq_table_mcmrb_setup_modify_resp_list_item_s_names};

// MCMRBFailedList-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mcmrb_failed_list_item_s_fields[] = {
    {offsetof(mcmrb_failed_list_item_s, mrb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(mcmrb_failed_list_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(mcmrb_failed_list_item_s, ie_exts),
     (int32_t)offsetof(mcmrb_failed_list_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mcmrb_failed_list_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mcmrb_failed_list_item_s_names[] = {{"mrb-ID", nullptr},
                                                                                   {"cause", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mcmrb_failed_list_item_s_opts[]  = {
    {2, (int32_t)offsetof(mcmrb_failed_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc mcmrb_failed_list_item_s::desc = {
    {seq_table_mcmrb_failed_list_item_s_fields, 3, UINT64_C(0x3), seq_table_mcmrb_failed_list_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_mcmrb_failed_list_item_s_names};

// MCMRBSetupConfiguration-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mcmrb_setup_cfg_item_s_fields[] = {
    {offsetof(mcmrb_setup_cfg_item_s, mrb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(mcmrb_setup_cfg_item_s, mbs_pdcp_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_cfg_s::desc},
    {offsetof(mcmrb_setup_cfg_item_s, qos_flow_qos_param_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(mcmrb_setup_cfg_item_s::qos_flow_qos_param_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_qos_param_item_s::desc},
    {offsetof(mcmrb_setup_cfg_item_s, qos_flow_level_qos_params),
     (int32_t)offsetof(mcmrb_setup_cfg_item_s, qos_flow_level_qos_params_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &qos_flow_level_qos_params_s::desc},
    {offsetof(mcmrb_setup_cfg_item_s, ie_exts),
     (int32_t)offsetof(mcmrb_setup_cfg_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mcmrb_setup_cfg_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mcmrb_setup_cfg_item_s_names[] = {
    {"mrb-ID", nullptr},
    {"mbs-pdcp-config", nullptr},
    {"qoS-Flow-QoS-Parameter-List", nullptr},
    {"qoSFlowLevelQoSParameters", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mcmrb_setup_cfg_item_s_opts[] = {
    {3, (int32_t)offsetof(mcmrb_setup_cfg_item_s, qos_flow_level_qos_params_present)},
    {4, (int32_t)offsetof(mcmrb_setup_cfg_item_s, ie_exts_present)}};
const asn1::table::seq_desc mcmrb_setup_cfg_item_s::desc = {
    {seq_table_mcmrb_setup_cfg_item_s_fields, 5, UINT64_C(0x7), seq_table_mcmrb_setup_cfg_item_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_mcmrb_setup_cfg_item_s_names};

// MRBForwardingResourceResponse-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mrb_forwarding_res_resp_item_s_fields[] = {
    {offsetof(mrb_forwarding_res_resp_item_s, mrb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(mrb_forwarding_res_resp_item_s, mrb_progress_info),
     (int32_t)offsetof(mrb_forwarding_res_resp_item_s, mrb_progress_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mrb_progress_info_s::desc},
    {offsetof(mrb_forwarding_res_resp_item_s, mrb_forwarding_address),
     (int32_t)offsetof(mrb_forwarding_res_resp_item_s, mrb_forwarding_address_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(mrb_forwarding_res_resp_item_s, ie_exts),
     (int32_t)offsetof(mrb_forwarding_res_resp_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mrb_forwarding_res_resp_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mrb_forwarding_res_resp_item_s_names[] = {
    {"mrb-ID", nullptr},
    {"mrb-ProgressInformation", nullptr},
    {"mrbForwardingAddress", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mrb_forwarding_res_resp_item_s_opts[] = {
    {1, (int32_t)offsetof(mrb_forwarding_res_resp_item_s, mrb_progress_info_present)},
    {2, (int32_t)offsetof(mrb_forwarding_res_resp_item_s, mrb_forwarding_address_present)},
    {3, (int32_t)offsetof(mrb_forwarding_res_resp_item_s, ie_exts_present)}};
const asn1::table::seq_desc mrb_forwarding_res_resp_item_s::desc = {{seq_table_mrb_forwarding_res_resp_item_s_fields,
                                                                     4,
                                                                     UINT64_C(0x1),
                                                                     seq_table_mrb_forwarding_res_resp_item_s_opts,
                                                                     3},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_mrb_forwarding_res_resp_item_s_names};

// MCForwardingResourceResponse ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_forwarding_res_resp_s_fields[] = {
    {offsetof(mc_forwarding_res_resp_s, mc_forwarding_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_forwarding_res_resp_s::mc_forwarding_res_id),
                                asn1::table::string_json>::value},
    {offsetof(mc_forwarding_res_resp_s, mrb_forwarding_res_resp_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(mc_forwarding_res_resp_s::mrb_forwarding_res_resp_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mrb_forwarding_res_resp_item_s::desc},
    {offsetof(mc_forwarding_res_resp_s, ie_exts),
     (int32_t)offsetof(mc_forwarding_res_resp_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_forwarding_res_resp_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mc_forwarding_res_resp_s_names[] = {
    {"mcForwardingResourceID", nullptr},
    {"mrbForwardingResourceResponseList", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mc_forwarding_res_resp_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(mc_forwarding_res_resp_s, ie_exts_present)}};
const asn1::table::seq_desc mc_forwarding_res_resp_s::desc = {
    {seq_table_mc_forwarding_res_resp_s_fields, 3, UINT64_C(0x1), seq_table_mc_forwarding_res_resp_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_mc_forwarding_res_resp_s_names};

static const uint32_t objset_id_mc_bearer_context_to_modify_resp_ext_ies_o[]   = {181};
static const uint8_t  objset_crit_mc_bearer_context_to_modify_resp_ext_ies_o[] = {crit_e::ignore};

// MCBearerContextToModifyResponse-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_mc_bearer_context_to_modify_resp_ext_ies_container_values[] = {
    {offsetof(mc_bearer_context_to_modify_resp_ext_ies_container, mc_forwarding_res_resp),
     (int32_t)offsetof(mc_bearer_context_to_modify_resp_ext_ies_container, mc_forwarding_res_resp_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_forwarding_res_resp_s::desc}};
static const int32_t ie_table_mc_bearer_context_to_modify_resp_ext_ies_container_presences[] = {
    (int32_t)offsetof(mc_bearer_context_to_modify_resp_ext_ies_container, mc_forwarding_res_resp_present)};
const asn1::table::ie_container_desc mc_bearer_context_to_modify_resp_ext_ies_container::desc = {
    objset_id_mc_bearer_context_to_modify_resp_ext_ies_o,
    objset_crit_mc_bearer_context_to_modify_resp_ext_ies_o,
    ie_table_mc_bearer_context_to_modify_resp_ext_ies_container_values,
    ie_table_mc_bearer_context_to_modify_resp_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// MCBearerContextToModifyResponse ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_bearer_context_to_modify_resp_s_fields[] = {
    {offsetof(mc_bearer_context_to_modify_resp_s, mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp),
     (int32_t)offsetof(mc_bearer_context_to_modify_resp_s, mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_s::desc},
    {offsetof(mc_bearer_context_to_modify_resp_s, mbs_multicast_f1_u_context_descriptor),
     (int32_t)offsetof(mc_bearer_context_to_modify_resp_s, mbs_multicast_f1_u_context_descriptor_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_multicast_f1_u_context_descriptor_s::desc},
    {offsetof(mc_bearer_context_to_modify_resp_s, mc_mrb_modify_setup_resp_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(mc_bearer_context_to_modify_resp_s::mc_mrb_modify_setup_resp_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mcmrb_setup_modify_resp_list_item_s::desc},
    {offsetof(mc_bearer_context_to_modify_resp_s, mc_mrb_failed_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(mc_bearer_context_to_modify_resp_s::mc_mrb_failed_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mcmrb_failed_list_item_s::desc},
    {offsetof(mc_bearer_context_to_modify_resp_s, available_mcmrb_cfg),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(mc_bearer_context_to_modify_resp_s::available_mcmrb_cfg),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mcmrb_setup_cfg_item_s::desc},
    {offsetof(mc_bearer_context_to_modify_resp_s, ie_exts),
     (int32_t)offsetof(mc_bearer_context_to_modify_resp_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_to_modify_resp_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_mc_bearer_context_to_modify_resp_s_names[] = {
    {"mcBearerContextNGU-TNLInfoatNGRANModifyResponse", nullptr},
    {"mbsMulticastF1UContextDescriptor", nullptr},
    {"mcMRBModifySetupResponseList", nullptr},
    {"mcMRBFailedList", nullptr},
    {"availableMCMRBConfig", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mc_bearer_context_to_modify_resp_s_opts[] = {
    {0,
     (int32_t)offsetof(mc_bearer_context_to_modify_resp_s, mc_bearer_context_ngu_tnl_infoat_ngran_modify_resp_present)},
    {1, (int32_t)offsetof(mc_bearer_context_to_modify_resp_s, mbs_multicast_f1_u_context_descriptor_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(mc_bearer_context_to_modify_resp_s, ie_exts_present)}};
const asn1::table::seq_desc mc_bearer_context_to_modify_resp_s::desc = {
    {seq_table_mc_bearer_context_to_modify_resp_s_fields,
     6,
     UINT64_C(0x0),
     seq_table_mc_bearer_context_to_modify_resp_s_opts,
     6},
    true,
    true,
    nullptr,
    0,
    seq_table_mc_bearer_context_to_modify_resp_s_names};

static const uint32_t objset_id_mc_bearer_context_to_setup_ext_ies_o[]   = {185, 189, 192, 193};
static const uint8_t  objset_crit_mc_bearer_context_to_setup_ext_ies_o[] = {crit_e::ignore,
                                                                            crit_e::ignore,
                                                                            crit_e::ignore,
                                                                            crit_e::ignore};

// MCBearerContextToSetup-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_mc_bearer_context_to_setup_ext_ies_container_values[] = {
    {offsetof(mc_bearer_context_to_setup_ext_ies_container, mbs_session_associated_info_non_support_to_support),
     (int32_t)offsetof(mc_bearer_context_to_setup_ext_ies_container,
                       mbs_session_associated_info_non_support_to_support_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_session_associated_info_non_support_to_support_s::desc},
    {offsetof(mc_bearer_context_to_setup_ext_ies_container, mbs_area_session_id),
     (int32_t)offsetof(mc_bearer_context_to_setup_ext_ies_container, mbs_area_session_id_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(mc_bearer_context_to_setup_ext_ies_container, mc_bearer_context_inactivity_timer),
     (int32_t)offsetof(mc_bearer_context_to_setup_ext_ies_container, mc_bearer_context_inactivity_timer_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(7200),
     nullptr},
    {offsetof(mc_bearer_context_to_setup_ext_ies_container, mc_bearer_context_status_change),
     (int32_t)offsetof(mc_bearer_context_to_setup_ext_ies_container, mc_bearer_context_status_change_present),
     asn1::table::enum_op<decltype(mc_bearer_context_to_setup_ext_ies_container::mc_bearer_context_status_change)>(),
     decltype(mc_bearer_context_to_setup_ext_ies_container::mc_bearer_context_status_change)::has_ext,
     false,
     decltype(mc_bearer_context_to_setup_ext_ies_container::mc_bearer_context_status_change)::nof_types,
     decltype(mc_bearer_context_to_setup_ext_ies_container::mc_bearer_context_status_change)::nof_exts,
     nullptr,
     &mc_bearer_context_status_change_opts::desc}};
static const int32_t ie_table_mc_bearer_context_to_setup_ext_ies_container_presences[] = {
    (int32_t)offsetof(mc_bearer_context_to_setup_ext_ies_container,
                      mbs_session_associated_info_non_support_to_support_present),
    (int32_t)offsetof(mc_bearer_context_to_setup_ext_ies_container, mbs_area_session_id_present),
    (int32_t)offsetof(mc_bearer_context_to_setup_ext_ies_container, mc_bearer_context_inactivity_timer_present),
    (int32_t)offsetof(mc_bearer_context_to_setup_ext_ies_container, mc_bearer_context_status_change_present)};
const asn1::table::ie_container_desc mc_bearer_context_to_setup_ext_ies_container::desc = {
    objset_id_mc_bearer_context_to_setup_ext_ies_o,
    objset_crit_mc_bearer_context_to_setup_ext_ies_o,
    ie_table_mc_bearer_context_to_setup_ext_ies_container_values,
    ie_table_mc_bearer_context_to_setup_ext_ies_container_presences,
    4,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// MCBearerContextToSetup ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_bearer_context_to_setup_s_fields[] = {
    {offsetof(mc_bearer_context_to_setup_s, snssai),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &snssai_s::desc},
    {offsetof(mc_bearer_context_to_setup_s, mc_mrb_to_setup_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(mc_bearer_context_to_setup_s::mc_mrb_to_setup_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mcmrb_setup_cfg_item_s::desc},
    {offsetof(mc_bearer_context_to_setup_s, requested_action),
     (int32_t)offsetof(mc_bearer_context_to_setup_s, requested_action_present),
     asn1::table::enum_op<decltype(mc_bearer_context_to_setup_s::requested_action)>(),
     decltype(mc_bearer_context_to_setup_s::requested_action)::has_ext,
     false,
     decltype(mc_bearer_context_to_setup_s::requested_action)::nof_types,
     decltype(mc_bearer_context_to_setup_s::requested_action)::nof_exts,
     nullptr,
     &requested_action4_avail_ngu_termination_opts::desc},
    {offsetof(mc_bearer_context_to_setup_s, ie_exts),
     (int32_t)offsetof(mc_bearer_context_to_setup_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_to_setup_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_mc_bearer_context_to_setup_s_names[] = {{"snssai", nullptr},
                                                                                       {"mcMRBToSetupList", nullptr},
                                                                                       {"requestedAction", nullptr},
                                                                                       {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mc_bearer_context_to_setup_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(mc_bearer_context_to_setup_s, requested_action_present)},
    {3, (int32_t)offsetof(mc_bearer_context_to_setup_s, ie_exts_present)}};
const asn1::table::seq_desc mc_bearer_context_to_setup_s::desc = {
    {seq_table_mc_bearer_context_to_setup_s_fields, 4, UINT64_C(0x1), seq_table_mc_bearer_context_to_setup_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_mc_bearer_context_to_setup_s_names};

// MCMRBSetupResponseList-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_mcmrb_setup_resp_list_item_s_fields[] = {
    {offsetof(mcmrb_setup_resp_list_item_s, mrb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(mcmrb_setup_resp_list_item_s, qosflow_setup),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(mcmrb_setup_resp_list_item_s::qosflow_setup),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_item_s::desc},
    {offsetof(mcmrb_setup_resp_list_item_s, qosflow_failed),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(mcmrb_setup_resp_list_item_s::qosflow_failed),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_failed_item_s::desc},
    {offsetof(mcmrb_setup_resp_list_item_s, mbs_pdcp_count),
     (int32_t)offsetof(mcmrb_setup_resp_list_item_s, mbs_pdcp_count_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mcmrb_setup_resp_list_item_s::mbs_pdcp_count),
                                asn1::table::struct_json>::value},
    {offsetof(mcmrb_setup_resp_list_item_s, ie_exts),
     (int32_t)offsetof(mcmrb_setup_resp_list_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mcmrb_setup_resp_list_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mcmrb_setup_resp_list_item_s_names[] = {{"mrb-ID", nullptr},
                                                                                       {"qosflow-setup", nullptr},
                                                                                       {"qosflow-failed", nullptr},
                                                                                       {"mBS-PDCP-COUNT", nullptr},
                                                                                       {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mcmrb_setup_resp_list_item_s_opts[]  = {
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(mcmrb_setup_resp_list_item_s, mbs_pdcp_count_present)},
    {4, (int32_t)offsetof(mcmrb_setup_resp_list_item_s, ie_exts_present)}};
const asn1::table::seq_desc mcmrb_setup_resp_list_item_s::desc = {
    {seq_table_mcmrb_setup_resp_list_item_s_fields, 5, UINT64_C(0x3), seq_table_mcmrb_setup_resp_list_item_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_mcmrb_setup_resp_list_item_s_names};

// MCBearerContextToSetupResponse ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_bearer_context_to_setup_resp_s_fields[] = {
    {offsetof(mc_bearer_context_to_setup_resp_s, mc_bearer_context_ngu_tnl_infoat_ngran),
     (int32_t)offsetof(mc_bearer_context_to_setup_resp_s, mc_bearer_context_ngu_tnl_infoat_ngran_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mc_bearer_context_ngu_tnl_infoat_ngran_c::desc},
    {offsetof(mc_bearer_context_to_setup_resp_s, mc_mrb_setup_resp_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(mc_bearer_context_to_setup_resp_s::mc_mrb_setup_resp_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mcmrb_setup_resp_list_item_s::desc},
    {offsetof(mc_bearer_context_to_setup_resp_s, mc_mrb_failed_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(mc_bearer_context_to_setup_resp_s::mc_mrb_failed_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mcmrb_failed_list_item_s::desc},
    {offsetof(mc_bearer_context_to_setup_resp_s, available_mcmrb_cfg),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(mc_bearer_context_to_setup_resp_s::available_mcmrb_cfg),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mcmrb_setup_cfg_item_s::desc},
    {offsetof(mc_bearer_context_to_setup_resp_s, ie_exts),
     (int32_t)offsetof(mc_bearer_context_to_setup_resp_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mc_bearer_context_to_setup_resp_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mc_bearer_context_to_setup_resp_s_names[] = {
    {"mcBearerContextNGU-TNLInfoatNGRAN", nullptr},
    {"mcMRBSetupResponseList", nullptr},
    {"mcMRBFailedList", nullptr},
    {"availableMCMRBConfig", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mc_bearer_context_to_setup_resp_s_opts[] = {
    {0, (int32_t)offsetof(mc_bearer_context_to_setup_resp_s, mc_bearer_context_ngu_tnl_infoat_ngran_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(mc_bearer_context_to_setup_resp_s, ie_exts_present)}};
const asn1::table::seq_desc mc_bearer_context_to_setup_resp_s::desc = {
    {seq_table_mc_bearer_context_to_setup_resp_s_fields,
     5,
     UINT64_C(0x0),
     seq_table_mc_bearer_context_to_setup_resp_s_opts,
     5},
    true,
    true,
    nullptr,
    0,
    seq_table_mc_bearer_context_to_setup_resp_s_names};

// MDT-Activation ::= ENUMERATED
static const char*           enum_names_mdt_activation_e_values[] = {"immediate-MDT-only", "immediate-MDT-and-Trace"};
const asn1::table::enum_desc mdt_activation_opts::desc = {enum_names_mdt_activation_e_values, 2, "mdt_activation_e"};

// MDTMode ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_mdt_mode_c_alternatives[] = {
    &asn1::table::alternative_ops<immediate_mdt_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<mdt_mode_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc mdt_mode_c::storage_desc = {choice_storage_mdt_mode_c_alternatives,
                                                                   2,
                                                                   "mdt_mode_c"};

immediate_mdt_s& mdt_mode_c::set_immediate_mdt()
{
  set(types::immediate_mdt);
  return c.get<immediate_mdt_s>();
}
protocol_ie_single_container_s<mdt_mode_ext_ies_o>& mdt_mode_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<mdt_mode_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_mdt_mode_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &immediate_mdt_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<mdt_mode_ext_ies_o>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_mdt_mode_c_names[] = {{"immediateMDT", nullptr},
                                                                        {"choice-extension", nullptr}};
const asn1::table::choice_desc       mdt_mode_c::desc                = {asn1::table::enum_int_type<mdt_mode_c::types>(),
                                                                        mdt_mode_c::types::nof_types,
                                                                        mdt_mode_c::types::nof_exts,
                                                                        mdt_mode_c::types::has_ext,
                                                                        true,
                                                                        choice_table_mdt_mode_c_alternatives,
                                                                        nullptr,
                                                                        choice_table_mdt_mode_c_names,
                                                                        "mdt_mode_c",
                                                                        offsetof(mdt_mode_c, c.type),
                                                                        offsetof(mdt_mode_c, c.value),
                                                                        false,
                                                                        &mdt_mode_c::storage_desc};

static const char*           enum_names_mdt_mode_c_types_values[] = {"immediateMDT", "choice-extension"};
const asn1::table::enum_desc mdt_mode_c::types_opts::desc         = {enum_names_mdt_mode_c_types_values,
                                                                     2,
                                                                     "mdt_mode_c::types"};

// MDT-Configuration ::= SEQUENCE
static const asn1::table::seq_field seq_table_mdt_cfg_s_fields[] = {
    {offsetof(mdt_cfg_s, mdt_activation),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mdt_cfg_s::mdt_activation)>(),
     decltype(mdt_cfg_s::mdt_activation)::has_ext,
     false,
     decltype(mdt_cfg_s::mdt_activation)::nof_types,
     decltype(mdt_cfg_s::mdt_activation)::nof_exts,
     nullptr,
     &mdt_activation_opts::desc},
    {offsetof(mdt_cfg_s, mdt_mode),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mdt_mode_c::desc},
    {offsetof(mdt_cfg_s, ie_exts),
     (int32_t)offsetof(mdt_cfg_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mdt_cfg_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mdt_cfg_s_names[] = {{"mdt-Activation", nullptr},
                                                                    {"mDTMode", nullptr},
                                                                    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mdt_cfg_s_opts[]  = {{2, (int32_t)offsetof(mdt_cfg_s, ie_exts_present)}};
const asn1::table::seq_desc          mdt_cfg_s::desc             = {
    {seq_table_mdt_cfg_s_fields, 3, UINT64_C(0x3), seq_table_mdt_cfg_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_mdt_cfg_s_names};

// MDTPollutedMeasurementIndicator ::= ENUMERATED
static const char*           enum_names_mdt_polluted_meas_ind_e_values[] = {"iDC", "no-IDC"};
const asn1::table::enum_desc mdt_polluted_meas_ind_opts::desc            = {enum_names_mdt_polluted_meas_ind_e_values,
                                                                            2,
                                                                            "mdt_polluted_meas_ind_e"};

// MRDC-Usage-Information ::= SEQUENCE
static const asn1::table::seq_field seq_table_mrdc_usage_info_s_fields[] = {
    {offsetof(mrdc_usage_info_s, data_usage_per_pdu_session_report),
     (int32_t)offsetof(mrdc_usage_info_s, data_usage_per_pdu_session_report_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_usage_per_pdu_session_report_s::desc},
    {offsetof(mrdc_usage_info_s, data_usage_per_qos_flow_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(mrdc_usage_info_s::data_usage_per_qos_flow_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &data_usage_per_qos_flow_item_s::desc},
    {offsetof(mrdc_usage_info_s, ie_exts),
     (int32_t)offsetof(mrdc_usage_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mrdc_usage_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mrdc_usage_info_s_names[] = {
    {"data-Usage-per-PDU-Session-Report", nullptr},
    {"data-Usage-per-QoS-Flow-List", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_mrdc_usage_info_s_opts[] = {
    {0, (int32_t)offsetof(mrdc_usage_info_s, data_usage_per_pdu_session_report_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(mrdc_usage_info_s, ie_exts_present)}};
const asn1::table::seq_desc mrdc_usage_info_s::desc = {
    {seq_table_mrdc_usage_info_s_fields, 3, UINT64_C(0x0), seq_table_mrdc_usage_info_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_mrdc_usage_info_s_names};

// MT-SDT-Information ::= SEQUENCE
static const asn1::table::seq_field seq_table_mt_sdt_info_s_fields[] = {
    {offsetof(mt_sdt_info_s, mt_sdt_data_size),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(1),
     UINT64_C(96000),
     nullptr},
    {offsetof(mt_sdt_info_s, ie_exts),
     (int32_t)offsetof(mt_sdt_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mt_sdt_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mt_sdt_info_s_names[] = {{"mT-SDT-Data-Size", nullptr},
                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_mt_sdt_info_s_opts[]  = {
    {1, (int32_t)offsetof(mt_sdt_info_s, ie_exts_present)}};
const asn1::table::seq_desc mt_sdt_info_s::desc = {
    {seq_table_mt_sdt_info_s_fields, 2, UINT64_C(0x1), seq_table_mt_sdt_info_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_mt_sdt_info_s_names};

// MT-SDT-Information-Request ::= ENUMERATED
static const char*           enum_names_mt_sdt_info_request_e_values[] = {"true"};
const asn1::table::enum_desc mt_sdt_info_request_opts::desc            = {enum_names_mt_sdt_info_request_e_values,
                                                                          1,
                                                                          "mt_sdt_info_request_e"};

// NG-RAN-QoS-Support-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ng_ran_qos_support_item_s_fields[] = {
    {offsetof(ng_ran_qos_support_item_s, non_dyn_5qi_descriptor),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &non_dyn_5qi_descriptor_s::desc},
    {offsetof(ng_ran_qos_support_item_s, ie_exts),
     (int32_t)offsetof(ng_ran_qos_support_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ng_ran_qos_support_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ng_ran_qos_support_item_s_names[] = {
    {"non-Dynamic5QIDescriptor", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_ng_ran_qos_support_item_s_opts[] = {
    {1, (int32_t)offsetof(ng_ran_qos_support_item_s, ie_exts_present)}};
const asn1::table::seq_desc ng_ran_qos_support_item_s::desc = {
    {seq_table_ng_ran_qos_support_item_s_fields, 2, UINT64_C(0x1), seq_table_ng_ran_qos_support_item_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_ng_ran_qos_support_item_s_names};

// NPNContextInfo-SNPN ::= SEQUENCE
static const asn1::table::seq_field seq_table_npn_context_info_sn_pn_s_fields[] = {
    {offsetof(npn_context_info_sn_pn_s, nid),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(npn_context_info_sn_pn_s::nid), asn1::table::struct_json>::value},
    {offsetof(npn_context_info_sn_pn_s, ie_exts),
     (int32_t)offsetof(npn_context_info_sn_pn_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(npn_context_info_sn_pn_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_npn_context_info_sn_pn_s_names[] = {{"nID", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_npn_context_info_sn_pn_s_opts[]  = {
    {1, (int32_t)offsetof(npn_context_info_sn_pn_s, ie_exts_present)}};
const asn1::table::seq_desc npn_context_info_sn_pn_s::desc = {
    {seq_table_npn_context_info_sn_pn_s_fields, 2, UINT64_C(0x1), seq_table_npn_context_info_sn_pn_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_npn_context_info_sn_pn_s_names};

// NPNContextInfo ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_npn_context_info_c_alternatives[] = {
    &asn1::table::alternative_ops<npn_context_info_sn_pn_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<npn_context_info_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc npn_context_info_c::storage_desc = {
    choice_storage_npn_context_info_c_alternatives,
    2,
    "npn_context_info_c"};

npn_context_info_sn_pn_s& npn_context_info_c::set_sn_pn()
{
  set(types::sn_pn);
  return c.get<npn_context_info_sn_pn_s>();
}
protocol_ie_single_container_s<npn_context_info_ext_ies_o>& npn_context_info_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<npn_context_info_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_npn_context_info_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &npn_context_info_sn_pn_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<npn_context_info_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_npn_context_info_c_names[] = {{"sNPN", nullptr},
                                                                                {"choice-extension", nullptr}};
const asn1::table::choice_desc npn_context_info_c::desc = {asn1::table::enum_int_type<npn_context_info_c::types>(),
                                                           npn_context_info_c::types::nof_types,
                                                           npn_context_info_c::types::nof_exts,
                                                           npn_context_info_c::types::has_ext,
                                                           true,
                                                           choice_table_npn_context_info_c_alternatives,
                                                           nullptr,
                                                           choice_table_npn_context_info_c_names,
                                                           "npn_context_info_c",
                                                           offsetof(npn_context_info_c, c.type),
                                                           offsetof(npn_context_info_c, c.value),
                                                           false,
                                                           &npn_context_info_c::storage_desc};

static const char*           enum_names_npn_context_info_c_types_values[] = {"sNPN", "choice-extension"};
const asn1::table::enum_desc npn_context_info_c::types_opts::desc         = {enum_names_npn_context_info_c_types_values,
                                                                             2,
                                                                             "npn_context_info_c::types"};

// NPNSupportInfo-SNPN ::= SEQUENCE
static const asn1::table::seq_field seq_table_npn_support_info_sn_pn_s_fields[] = {
    {offsetof(npn_support_info_sn_pn_s, nid),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(npn_support_info_sn_pn_s::nid), asn1::table::struct_json>::value},
    {offsetof(npn_support_info_sn_pn_s, ie_exts),
     (int32_t)offsetof(npn_support_info_sn_pn_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(npn_support_info_sn_pn_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_npn_support_info_sn_pn_s_names[] = {{"nID", nullptr},
                                                                                   {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_npn_support_info_sn_pn_s_opts[]  = {
    {1, (int32_t)offsetof(npn_support_info_sn_pn_s, ie_exts_present)}};
const asn1::table::seq_desc npn_support_info_sn_pn_s::desc = {
    {seq_table_npn_support_info_sn_pn_s_fields, 2, UINT64_C(0x1), seq_table_npn_support_info_sn_pn_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_npn_support_info_sn_pn_s_names};

// NPNSupportInfo ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_npn_support_info_c_alternatives[] = {
    &asn1::table::alternative_ops<npn_support_info_sn_pn_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<npn_support_info_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc npn_support_info_c::storage_desc = {
    choice_storage_npn_support_info_c_alternatives,
    2,
    "npn_support_info_c"};

npn_support_info_sn_pn_s& npn_support_info_c::set_sn_pn()
{
  set(types::sn_pn);
  return c.get<npn_support_info_sn_pn_s>();
}
protocol_ie_single_container_s<npn_support_info_ext_ies_o>& npn_support_info_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<npn_support_info_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_npn_support_info_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &npn_support_info_sn_pn_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<npn_support_info_ext_ies_o>,
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_npn_support_info_c_names[] = {{"sNPN", nullptr},
                                                                                {"choice-extension", nullptr}};
const asn1::table::choice_desc npn_support_info_c::desc = {asn1::table::enum_int_type<npn_support_info_c::types>(),
                                                           npn_support_info_c::types::nof_types,
                                                           npn_support_info_c::types::nof_exts,
                                                           npn_support_info_c::types::has_ext,
                                                           true,
                                                           choice_table_npn_support_info_c_alternatives,
                                                           nullptr,
                                                           choice_table_npn_support_info_c_names,
                                                           "npn_support_info_c",
                                                           offsetof(npn_support_info_c, c.type),
                                                           offsetof(npn_support_info_c, c.value),
                                                           false,
                                                           &npn_support_info_c::storage_desc};

static const char*           enum_names_npn_support_info_c_types_values[] = {"sNPN", "choice-extension"};
const asn1::table::enum_desc npn_support_info_c::types_opts::desc         = {enum_names_npn_support_info_c_types_values,
                                                                             2,
                                                                             "npn_support_info_c::types"};

// NR-CGI-Support-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_cgi_support_item_s_fields[] = {
    {offsetof(nr_cgi_support_item_s, nr_cgi),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_cgi_s::desc},
    {offsetof(nr_cgi_support_item_s, ie_exts),
     (int32_t)offsetof(nr_cgi_support_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(nr_cgi_support_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_nr_cgi_support_item_s_names[] = {{"nR-CGI", nullptr},
                                                                                {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_nr_cgi_support_item_s_opts[]  = {
    {1, (int32_t)offsetof(nr_cgi_support_item_s, ie_exts_present)}};
const asn1::table::seq_desc nr_cgi_support_item_s::desc = {
    {seq_table_nr_cgi_support_item_s_fields, 2, UINT64_C(0x1), seq_table_nr_cgi_support_item_s_opts, 1},
    false,
    true,
    nullptr,
    0,
    seq_table_nr_cgi_support_item_s_names};

// New-UL-TNL-Information-Required ::= ENUMERATED
static const char*           enum_names_new_ul_tnl_info_required_e_values[] = {"required"};
const asn1::table::enum_desc new_ul_tnl_info_required_opts::desc = {enum_names_new_ul_tnl_info_required_e_values,
                                                                    1,
                                                                    "new_ul_tnl_info_required_e"};

// PDU-Session-Resource-Confirm-Modified-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_session_res_confirm_modified_item_s_fields[] = {
    {offsetof(pdu_session_res_confirm_modified_item_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pdu_session_res_confirm_modified_item_s, drb_confirm_modified_list_ng_ran),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pdu_session_res_confirm_modified_item_s::drb_confirm_modified_list_ng_ran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_confirm_modified_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_confirm_modified_item_s, ie_exts),
     (int32_t)offsetof(pdu_session_res_confirm_modified_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pdu_session_res_confirm_modified_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pdu_session_res_confirm_modified_item_s_names[] = {
    {"pDU-Session-ID", nullptr},
    {"dRB-Confirm-Modified-List-NG-RAN", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pdu_session_res_confirm_modified_item_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(pdu_session_res_confirm_modified_item_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_session_res_confirm_modified_item_s::desc = {
    {seq_table_pdu_session_res_confirm_modified_item_s_fields,
     3,
     UINT64_C(0x1),
     seq_table_pdu_session_res_confirm_modified_item_s_opts,
     2},
    true,
    true,
    nullptr,
    0,
    seq_table_pdu_session_res_confirm_modified_item_s_names};

// PDU-Session-Resource-Data-Usage-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_session_res_data_usage_item_s_fields[] = {
    {offsetof(pdu_session_res_data_usage_item_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pdu_session_res_data_usage_item_s, mrdc_usage_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mrdc_usage_info_s::desc},
    {offsetof(pdu_session_res_data_usage_item_s, ie_exts),
     (int32_t)offsetof(pdu_session_res_data_usage_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pdu_session_res_data_usage_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pdu_session_res_data_usage_item_s_names[] = {
    {"pDU-Session-ID", nullptr},
    {"mRDC-Usage-Information", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pdu_session_res_data_usage_item_s_opts[] = {
    {2, (int32_t)offsetof(pdu_session_res_data_usage_item_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_session_res_data_usage_item_s::desc = {
    {seq_table_pdu_session_res_data_usage_item_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_pdu_session_res_data_usage_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_pdu_session_res_data_usage_item_s_names};

// PDU-Session-Resource-Failed-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_session_res_failed_item_s_fields[] = {
    {offsetof(pdu_session_res_failed_item_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pdu_session_res_failed_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(pdu_session_res_failed_item_s, ie_exts),
     (int32_t)offsetof(pdu_session_res_failed_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pdu_session_res_failed_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pdu_session_res_failed_item_s_names[] = {{"pDU-Session-ID", nullptr},
                                                                                        {"cause", nullptr},
                                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_pdu_session_res_failed_item_s_opts[]  = {
    {2, (int32_t)offsetof(pdu_session_res_failed_item_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_session_res_failed_item_s::desc = {
    {seq_table_pdu_session_res_failed_item_s_fields, 3, UINT64_C(0x3), seq_table_pdu_session_res_failed_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_pdu_session_res_failed_item_s_names};

// PDU-Session-Resource-Failed-Mod-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_session_res_failed_mod_item_s_fields[] = {
    {offsetof(pdu_session_res_failed_mod_item_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pdu_session_res_failed_mod_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(pdu_session_res_failed_mod_item_s, ie_exts),
     (int32_t)offsetof(pdu_session_res_failed_mod_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pdu_session_res_failed_mod_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pdu_session_res_failed_mod_item_s_names[] = {{"pDU-Session-ID", nullptr},
                                                                                            {"cause", nullptr},
                                                                                            {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_pdu_session_res_failed_mod_item_s_opts[]  = {
    {2, (int32_t)offsetof(pdu_session_res_failed_mod_item_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_session_res_failed_mod_item_s::desc = {
    {seq_table_pdu_session_res_failed_mod_item_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_pdu_session_res_failed_mod_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_pdu_session_res_failed_mod_item_s_names};

// PDU-Session-Resource-Failed-To-Modify-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_session_res_failed_to_modify_item_s_fields[] = {
    {offsetof(pdu_session_res_failed_to_modify_item_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pdu_session_res_failed_to_modify_item_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(pdu_session_res_failed_to_modify_item_s, ie_exts),
     (int32_t)offsetof(pdu_session_res_failed_to_modify_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pdu_session_res_failed_to_modify_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pdu_session_res_failed_to_modify_item_s_names[] = {
    {"pDU-Session-ID", nullptr},
    {"cause", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pdu_session_res_failed_to_modify_item_s_opts[] = {
    {2, (int32_t)offsetof(pdu_session_res_failed_to_modify_item_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_session_res_failed_to_modify_item_s::desc = {
    {seq_table_pdu_session_res_failed_to_modify_item_s_fields,
     3,
     UINT64_C(0x3),
     seq_table_pdu_session_res_failed_to_modify_item_s_opts,
     1},
    true,
    true,
    nullptr,
    0,
    seq_table_pdu_session_res_failed_to_modify_item_s_names};

static const uint32_t objset_id_pdu_session_res_modified_item_ext_ies_o[]   = {98};
static const uint8_t  objset_crit_pdu_session_res_modified_item_ext_ies_o[] = {crit_e::ignore};

// PDU-Session-Resource-Modified-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_pdu_session_res_modified_item_ext_ies_container_values[] = {
    {offsetof(pdu_session_res_modified_item_ext_ies_container, redundant_n_g_dl_up_tnl_info),
     (int32_t)offsetof(pdu_session_res_modified_item_ext_ies_container, redundant_n_g_dl_up_tnl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc}};
static const int32_t ie_table_pdu_session_res_modified_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(pdu_session_res_modified_item_ext_ies_container, redundant_n_g_dl_up_tnl_info_present)};
const asn1::table::ie_container_desc pdu_session_res_modified_item_ext_ies_container::desc = {
    objset_id_pdu_session_res_modified_item_ext_ies_o,
    objset_crit_pdu_session_res_modified_item_ext_ies_o,
    ie_table_pdu_session_res_modified_item_ext_ies_container_values,
    ie_table_pdu_session_res_modified_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// PDU-Session-Resource-Modified-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_session_res_modified_item_s_fields[] = {
    {offsetof(pdu_session_res_modified_item_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pdu_session_res_modified_item_s, ng_dl_up_tnl_info),
     (int32_t)offsetof(pdu_session_res_modified_item_s, ng_dl_up_tnl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(pdu_session_res_modified_item_s, security_result),
     (int32_t)offsetof(pdu_session_res_modified_item_s, security_result_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_result_s::desc},
    {offsetof(pdu_session_res_modified_item_s, pdu_session_data_forwarding_info_resp),
     (int32_t)offsetof(pdu_session_res_modified_item_s, pdu_session_data_forwarding_info_resp_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_s::desc},
    {offsetof(pdu_session_res_modified_item_s, drb_setup_list_ng_ran),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pdu_session_res_modified_item_s::drb_setup_list_ng_ran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_setup_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_modified_item_s, drb_failed_list_ng_ran),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pdu_session_res_modified_item_s::drb_failed_list_ng_ran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_failed_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_modified_item_s, drb_modified_list_ng_ran),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pdu_session_res_modified_item_s::drb_modified_list_ng_ran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_modified_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_modified_item_s, drb_failed_to_modify_list_ng_ran),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pdu_session_res_modified_item_s::drb_failed_to_modify_list_ng_ran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_failed_to_modify_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_modified_item_s, ie_exts),
     (int32_t)offsetof(pdu_session_res_modified_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &pdu_session_res_modified_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_pdu_session_res_modified_item_s_names[] = {
    {"pDU-Session-ID", nullptr},
    {"nG-DL-UP-TNL-Information", nullptr},
    {"securityResult", nullptr},
    {"pDU-Session-Data-Forwarding-Information-Response", nullptr},
    {"dRB-Setup-List-NG-RAN", nullptr},
    {"dRB-Failed-List-NG-RAN", nullptr},
    {"dRB-Modified-List-NG-RAN", nullptr},
    {"dRB-Failed-To-Modify-List-NG-RAN", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pdu_session_res_modified_item_s_opts[] = {
    {1, (int32_t)offsetof(pdu_session_res_modified_item_s, ng_dl_up_tnl_info_present)},
    {2, (int32_t)offsetof(pdu_session_res_modified_item_s, security_result_present)},
    {3, (int32_t)offsetof(pdu_session_res_modified_item_s, pdu_session_data_forwarding_info_resp_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence},
    {6, asn1::table::seq_field::list_presence},
    {7, asn1::table::seq_field::list_presence},
    {8, (int32_t)offsetof(pdu_session_res_modified_item_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_session_res_modified_item_s::desc = {{seq_table_pdu_session_res_modified_item_s_fields,
                                                                      9,
                                                                      UINT64_C(0x1),
                                                                      seq_table_pdu_session_res_modified_item_s_opts,
                                                                      8},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_pdu_session_res_modified_item_s_names};

static const uint32_t objset_id_pdu_session_res_required_to_modify_item_ext_ies_o[]   = {98};
static const uint8_t  objset_crit_pdu_session_res_required_to_modify_item_ext_ies_o[] = {crit_e::ignore};

// PDU-Session-Resource-Required-To-Modify-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_pdu_session_res_required_to_modify_item_ext_ies_container_values[] = {
    {offsetof(pdu_session_res_required_to_modify_item_ext_ies_container, redundant_n_g_dl_up_tnl_info),
     (int32_t)offsetof(pdu_session_res_required_to_modify_item_ext_ies_container, redundant_n_g_dl_up_tnl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc}};
static const int32_t ie_table_pdu_session_res_required_to_modify_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(pdu_session_res_required_to_modify_item_ext_ies_container, redundant_n_g_dl_up_tnl_info_present)};
const asn1::table::ie_container_desc pdu_session_res_required_to_modify_item_ext_ies_container::desc = {
    objset_id_pdu_session_res_required_to_modify_item_ext_ies_o,
    objset_crit_pdu_session_res_required_to_modify_item_ext_ies_o,
    ie_table_pdu_session_res_required_to_modify_item_ext_ies_container_values,
    ie_table_pdu_session_res_required_to_modify_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// PDU-Session-Resource-Required-To-Modify-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_session_res_required_to_modify_item_s_fields[] = {
    {offsetof(pdu_session_res_required_to_modify_item_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pdu_session_res_required_to_modify_item_s, ng_dl_up_tnl_info),
     (int32_t)offsetof(pdu_session_res_required_to_modify_item_s, ng_dl_up_tnl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(pdu_session_res_required_to_modify_item_s, drb_required_to_modify_list_ng_ran),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<
         decltype(pdu_session_res_required_to_modify_item_s::drb_required_to_modify_list_ng_ran),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &drb_required_to_modify_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_required_to_modify_item_s, drb_required_to_rem_list_ng_ran),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pdu_session_res_required_to_modify_item_s::drb_required_to_rem_list_ng_ran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_required_to_rem_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_required_to_modify_item_s, ie_exts),
     (int32_t)offsetof(pdu_session_res_required_to_modify_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &pdu_session_res_required_to_modify_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_pdu_session_res_required_to_modify_item_s_names[] = {
    {"pDU-Session-ID", nullptr},
    {"nG-DL-UP-TNL-Information", nullptr},
    {"dRB-Required-To-Modify-List-NG-RAN", nullptr},
    {"dRB-Required-To-Remove-List-NG-RAN", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pdu_session_res_required_to_modify_item_s_opts[] = {
    {1, (int32_t)offsetof(pdu_session_res_required_to_modify_item_s, ng_dl_up_tnl_info_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(pdu_session_res_required_to_modify_item_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_session_res_required_to_modify_item_s::desc = {
    {seq_table_pdu_session_res_required_to_modify_item_s_fields,
     5,
     UINT64_C(0x1),
     seq_table_pdu_session_res_required_to_modify_item_s_opts,
     4},
    true,
    true,
    nullptr,
    0,
    seq_table_pdu_session_res_required_to_modify_item_s_names};

// RSN ::= ENUMERATED
static const char*           enum_names_r_sn_e_values[] = {"v1", "v2"};
const asn1::table::enum_desc r_sn_opts::desc            = {enum_names_r_sn_e_values, 2, "r_sn_e"};
uint8_t                      r_sn_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "r_sn_e");
}

static const uint32_t objset_id_redundant_pdu_session_info_ext_ies_o[]   = {151};
static const uint8_t  objset_crit_redundant_pdu_session_info_ext_ies_o[] = {crit_e::ignore};

// RedundantPDUSessionInformation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_redundant_pdu_session_info_ext_ies_container_values[] = {
    {offsetof(redundant_pdu_session_info_ext_ies_container, pdu_session_pair_id),
     (int32_t)offsetof(redundant_pdu_session_info_ext_ies_container, pdu_session_pair_id_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr}};
static const int32_t ie_table_redundant_pdu_session_info_ext_ies_container_presences[] = {
    (int32_t)offsetof(redundant_pdu_session_info_ext_ies_container, pdu_session_pair_id_present)};
const asn1::table::ie_container_desc redundant_pdu_session_info_ext_ies_container::desc = {
    objset_id_redundant_pdu_session_info_ext_ies_o,
    objset_crit_redundant_pdu_session_info_ext_ies_o,
    ie_table_redundant_pdu_session_info_ext_ies_container_values,
    ie_table_redundant_pdu_session_info_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// RedundantPDUSessionInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_redundant_pdu_session_info_s_fields[] = {
    {offsetof(redundant_pdu_session_info_s, rsn),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(redundant_pdu_session_info_s::rsn)>(),
     decltype(redundant_pdu_session_info_s::rsn)::has_ext,
     false,
     decltype(redundant_pdu_session_info_s::rsn)::nof_types,
     decltype(redundant_pdu_session_info_s::rsn)::nof_exts,
     nullptr,
     &r_sn_opts::desc},
    {offsetof(redundant_pdu_session_info_s, ie_exts),
     (int32_t)offsetof(redundant_pdu_session_info_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &redundant_pdu_session_info_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_redundant_pdu_session_info_s_names[] = {{"rSN", nullptr},
                                                                                       {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_redundant_pdu_session_info_s_opts[]  = {
    {1, (int32_t)offsetof(redundant_pdu_session_info_s, ie_exts_present)}};
const asn1::table::seq_desc redundant_pdu_session_info_s::desc = {
    {seq_table_redundant_pdu_session_info_s_fields, 2, UINT64_C(0x1), seq_table_redundant_pdu_session_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_redundant_pdu_session_info_s_names};

static const uint32_t objset_id_pdu_session_res_setup_item_ext_ies_o[]   = {98, 106};
static const uint8_t  objset_crit_pdu_session_res_setup_item_ext_ies_o[] = {crit_e::ignore, crit_e::ignore};

// PDU-Session-Resource-Setup-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_pdu_session_res_setup_item_ext_ies_container_values[] = {
    {offsetof(pdu_session_res_setup_item_ext_ies_container, redundant_n_g_dl_up_tnl_info),
     (int32_t)offsetof(pdu_session_res_setup_item_ext_ies_container, redundant_n_g_dl_up_tnl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(pdu_session_res_setup_item_ext_ies_container, redundant_pdu_session_info_used),
     (int32_t)offsetof(pdu_session_res_setup_item_ext_ies_container, redundant_pdu_session_info_used_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &redundant_pdu_session_info_s::desc}};
static const int32_t ie_table_pdu_session_res_setup_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(pdu_session_res_setup_item_ext_ies_container, redundant_n_g_dl_up_tnl_info_present),
    (int32_t)offsetof(pdu_session_res_setup_item_ext_ies_container, redundant_pdu_session_info_used_present)};
const asn1::table::ie_container_desc pdu_session_res_setup_item_ext_ies_container::desc = {
    objset_id_pdu_session_res_setup_item_ext_ies_o,
    objset_crit_pdu_session_res_setup_item_ext_ies_o,
    ie_table_pdu_session_res_setup_item_ext_ies_container_values,
    ie_table_pdu_session_res_setup_item_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// PDU-Session-Resource-Setup-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_session_res_setup_item_s_fields[] = {
    {offsetof(pdu_session_res_setup_item_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pdu_session_res_setup_item_s, security_result),
     (int32_t)offsetof(pdu_session_res_setup_item_s, security_result_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_result_s::desc},
    {offsetof(pdu_session_res_setup_item_s, ng_dl_up_tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(pdu_session_res_setup_item_s, pdu_session_data_forwarding_info_resp),
     (int32_t)offsetof(pdu_session_res_setup_item_s, pdu_session_data_forwarding_info_resp_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_s::desc},
    {offsetof(pdu_session_res_setup_item_s, ng_dl_up_unchanged),
     (int32_t)offsetof(pdu_session_res_setup_item_s, ng_dl_up_unchanged_present),
     asn1::table::enum_op<decltype(pdu_session_res_setup_item_s::ng_dl_up_unchanged)>(),
     decltype(pdu_session_res_setup_item_s::ng_dl_up_unchanged)::has_ext,
     false,
     decltype(pdu_session_res_setup_item_s::ng_dl_up_unchanged)::nof_types,
     decltype(pdu_session_res_setup_item_s::ng_dl_up_unchanged)::nof_exts,
     nullptr,
     &pdu_session_res_setup_item_s::ng_dl_up_unchanged_opts::desc},
    {offsetof(pdu_session_res_setup_item_s, drb_setup_list_ng_ran),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pdu_session_res_setup_item_s::drb_setup_list_ng_ran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_setup_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_setup_item_s, drb_failed_list_ng_ran),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pdu_session_res_setup_item_s::drb_failed_list_ng_ran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_failed_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_setup_item_s, ie_exts),
     (int32_t)offsetof(pdu_session_res_setup_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &pdu_session_res_setup_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_pdu_session_res_setup_item_s_names[] = {
    {"pDU-Session-ID", nullptr},
    {"securityResult", nullptr},
    {"nG-DL-UP-TNL-Information", nullptr},
    {"pDU-Session-Data-Forwarding-Information-Response", nullptr},
    {"nG-DL-UP-Unchanged", nullptr},
    {"dRB-Setup-List-NG-RAN", nullptr},
    {"dRB-Failed-List-NG-RAN", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pdu_session_res_setup_item_s_opts[] = {
    {1, (int32_t)offsetof(pdu_session_res_setup_item_s, security_result_present)},
    {3, (int32_t)offsetof(pdu_session_res_setup_item_s, pdu_session_data_forwarding_info_resp_present)},
    {4, (int32_t)offsetof(pdu_session_res_setup_item_s, ng_dl_up_unchanged_present)},
    {6, asn1::table::seq_field::list_presence},
    {7, (int32_t)offsetof(pdu_session_res_setup_item_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_session_res_setup_item_s::desc = {
    {seq_table_pdu_session_res_setup_item_s_fields, 8, UINT64_C(0x25), seq_table_pdu_session_res_setup_item_s_opts, 5},
    true,
    true,
    nullptr,
    0,
    seq_table_pdu_session_res_setup_item_s_names};

static const char*           enum_names_pdu_session_res_setup_item_s_ng_dl_up_unchanged_e__values[] = {"true"};
const asn1::table::enum_desc pdu_session_res_setup_item_s::ng_dl_up_unchanged_opts::desc            = {
    enum_names_pdu_session_res_setup_item_s_ng_dl_up_unchanged_e__values,
    1,
    "pdu_session_res_setup_item_s::ng_dl_up_unchanged_e_"};

static const uint32_t objset_id_pdu_session_res_setup_mod_item_ext_ies_o[]   = {98};
static const uint8_t  objset_crit_pdu_session_res_setup_mod_item_ext_ies_o[] = {crit_e::ignore};

// PDU-Session-Resource-Setup-Mod-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_pdu_session_res_setup_mod_item_ext_ies_container_values[] = {
    {offsetof(pdu_session_res_setup_mod_item_ext_ies_container, redundant_n_g_dl_up_tnl_info),
     (int32_t)offsetof(pdu_session_res_setup_mod_item_ext_ies_container, redundant_n_g_dl_up_tnl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc}};
static const int32_t ie_table_pdu_session_res_setup_mod_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(pdu_session_res_setup_mod_item_ext_ies_container, redundant_n_g_dl_up_tnl_info_present)};
const asn1::table::ie_container_desc pdu_session_res_setup_mod_item_ext_ies_container::desc = {
    objset_id_pdu_session_res_setup_mod_item_ext_ies_o,
    objset_crit_pdu_session_res_setup_mod_item_ext_ies_o,
    ie_table_pdu_session_res_setup_mod_item_ext_ies_container_values,
    ie_table_pdu_session_res_setup_mod_item_ext_ies_container_presences,
    1,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// PDU-Session-Resource-Setup-Mod-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_session_res_setup_mod_item_s_fields[] = {
    {offsetof(pdu_session_res_setup_mod_item_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pdu_session_res_setup_mod_item_s, security_result),
     (int32_t)offsetof(pdu_session_res_setup_mod_item_s, security_result_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_result_s::desc},
    {offsetof(pdu_session_res_setup_mod_item_s, ng_dl_up_tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(pdu_session_res_setup_mod_item_s, pdu_session_data_forwarding_info_resp),
     (int32_t)offsetof(pdu_session_res_setup_mod_item_s, pdu_session_data_forwarding_info_resp_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_s::desc},
    {offsetof(pdu_session_res_setup_mod_item_s, drb_setup_mod_list_ng_ran),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pdu_session_res_setup_mod_item_s::drb_setup_mod_list_ng_ran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_setup_mod_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_setup_mod_item_s, drb_failed_mod_list_ng_ran),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pdu_session_res_setup_mod_item_s::drb_failed_mod_list_ng_ran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_failed_mod_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_setup_mod_item_s, ie_exts),
     (int32_t)offsetof(pdu_session_res_setup_mod_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &pdu_session_res_setup_mod_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_pdu_session_res_setup_mod_item_s_names[] = {
    {"pDU-Session-ID", nullptr},
    {"securityResult", nullptr},
    {"nG-DL-UP-TNL-Information", nullptr},
    {"pDU-Session-Data-Forwarding-Information-Response", nullptr},
    {"dRB-Setup-Mod-List-NG-RAN", nullptr},
    {"dRB-Failed-Mod-List-NG-RAN", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pdu_session_res_setup_mod_item_s_opts[] = {
    {1, (int32_t)offsetof(pdu_session_res_setup_mod_item_s, security_result_present)},
    {3, (int32_t)offsetof(pdu_session_res_setup_mod_item_s, pdu_session_data_forwarding_info_resp_present)},
    {5, asn1::table::seq_field::list_presence},
    {6, (int32_t)offsetof(pdu_session_res_setup_mod_item_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_session_res_setup_mod_item_s::desc = {
    {seq_table_pdu_session_res_setup_mod_item_s_fields,
     7,
     UINT64_C(0x15),
     seq_table_pdu_session_res_setup_mod_item_s_opts,
     4},
    true,
    true,
    nullptr,
    0,
    seq_table_pdu_session_res_setup_mod_item_s_names};

// UserPlaneFailureType ::= ENUMERATED
static const char* enum_names_user_plane_fail_type_e_values[] = {"gtp-u-error-indication-received", "up-path-failure"};
const asn1::table::enum_desc user_plane_fail_type_opts::desc  = {enum_names_user_plane_fail_type_e_values,
                                                                 2,
                                                                 "user_plane_fail_type_e"};

// UserPlaneFailureIndication ::= SEQUENCE
static const asn1::table::seq_field seq_table_user_plane_fail_ind_s_fields[] = {
    {offsetof(user_plane_fail_ind_s, user_plane_fail_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(user_plane_fail_ind_s::user_plane_fail_type)>(),
     decltype(user_plane_fail_ind_s::user_plane_fail_type)::has_ext,
     false,
     decltype(user_plane_fail_ind_s::user_plane_fail_type)::nof_types,
     decltype(user_plane_fail_ind_s::user_plane_fail_type)::nof_exts,
     nullptr,
     &user_plane_fail_type_opts::desc},
    {offsetof(user_plane_fail_ind_s, ng_dl_up_tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(user_plane_fail_ind_s, ng_ul_up_tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(user_plane_fail_ind_s, ie_exts),
     (int32_t)offsetof(user_plane_fail_ind_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(user_plane_fail_ind_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_user_plane_fail_ind_s_names[] = {{"userPlaneFailureType", nullptr},
                                                                                {"nG-DL-UP-TNL-Information", nullptr},
                                                                                {"nG-UL-UP-TNL-Information", nullptr},
                                                                                {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_user_plane_fail_ind_s_opts[]  = {
    {3, (int32_t)offsetof(user_plane_fail_ind_s, ie_exts_present)}};
const asn1::table::seq_desc user_plane_fail_ind_s::desc = {
    {seq_table_user_plane_fail_ind_s_fields, 4, UINT64_C(0x7), seq_table_user_plane_fail_ind_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_user_plane_fail_ind_s_names};

static const uint32_t objset_id_pdu_session_res_to_modify_item_ext_ies_o[] = {69, 78, 97, 96, 131, 143, 190, 219, 198};
static const uint8_t  objset_crit_pdu_session_res_to_modify_item_ext_ies_o[] = {crit_e::reject,
                                                                                crit_e::ignore,
                                                                                crit_e::ignore,
                                                                                crit_e::ignore,
                                                                                crit_e::ignore,
                                                                                crit_e::ignore,
                                                                                crit_e::ignore,
                                                                                crit_e::ignore,
                                                                                crit_e::ignore};

// PDU-Session-Resource-To-Modify-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_pdu_session_res_to_modify_item_ext_ies_container_values[] = {
    {offsetof(pdu_session_res_to_modify_item_ext_ies_container, snssai),
     (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, snssai_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &snssai_s::desc},
    {offsetof(pdu_session_res_to_modify_item_ext_ies_container, common_network_instance),
     (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, common_network_instance_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pdu_session_res_to_modify_item_ext_ies_container::common_network_instance),
                                asn1::table::string_json>::value},
    {offsetof(pdu_session_res_to_modify_item_ext_ies_container, redundant_n_g_ul_up_tnl_info),
     (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, redundant_n_g_ul_up_tnl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(pdu_session_res_to_modify_item_ext_ies_container, redundant_common_network_instance),
     (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, redundant_common_network_instance_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<
         decltype(pdu_session_res_to_modify_item_ext_ies_container::redundant_common_network_instance),
         asn1::table::string_json>::value},
    {offsetof(pdu_session_res_to_modify_item_ext_ies_container, data_forwardingto_e_utran_info_list),
     (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, data_forwardingto_e_utran_info_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     1,
     256,
     &asn1::table::item_list_codec<
         decltype(pdu_session_res_to_modify_item_ext_ies_container::data_forwardingto_e_utran_info_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &data_forwardingto_e_utran_info_list_item_s::desc},
    {offsetof(pdu_session_res_to_modify_item_ext_ies_container, security_ind_modify),
     (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, security_ind_modify_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_ind_s::desc},
    {offsetof(pdu_session_res_to_modify_item_ext_ies_container, secondary_pdu_session_data_forwarding_info),
     (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container,
                       secondary_pdu_session_data_forwarding_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_s::desc},
    {offsetof(pdu_session_res_to_modify_item_ext_ies_container, user_plane_fail_ind),
     (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, user_plane_fail_ind_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &user_plane_fail_ind_s::desc},
    {offsetof(pdu_session_res_to_modify_item_ext_ies_container, special_trigger_purpose),
     (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, special_trigger_purpose_present),
     asn1::table::enum_op<decltype(pdu_session_res_to_modify_item_ext_ies_container::special_trigger_purpose)>(),
     decltype(pdu_session_res_to_modify_item_ext_ies_container::special_trigger_purpose)::has_ext,
     false,
     decltype(pdu_session_res_to_modify_item_ext_ies_container::special_trigger_purpose)::nof_types,
     decltype(pdu_session_res_to_modify_item_ext_ies_container::special_trigger_purpose)::nof_exts,
     nullptr,
     &special_trigger_purpose_opts::desc}};
static const int32_t ie_table_pdu_session_res_to_modify_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, snssai_present),
    (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, common_network_instance_present),
    (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, redundant_n_g_ul_up_tnl_info_present),
    (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, redundant_common_network_instance_present),
    (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, data_forwardingto_e_utran_info_list_present),
    (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, security_ind_modify_present),
    (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container,
                      secondary_pdu_session_data_forwarding_info_present),
    (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, user_plane_fail_ind_present),
    (int32_t)offsetof(pdu_session_res_to_modify_item_ext_ies_container, special_trigger_purpose_present)};
const asn1::table::ie_container_desc pdu_session_res_to_modify_item_ext_ies_container::desc = {
    objset_id_pdu_session_res_to_modify_item_ext_ies_o,
    objset_crit_pdu_session_res_to_modify_item_ext_ies_o,
    ie_table_pdu_session_res_to_modify_item_ext_ies_container_values,
    ie_table_pdu_session_res_to_modify_item_ext_ies_container_presences,
    9,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// PDU-Session-Resource-To-Modify-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_session_res_to_modify_item_s_fields[] = {
    {offsetof(pdu_session_res_to_modify_item_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pdu_session_res_to_modify_item_s, security_ind),
     (int32_t)offsetof(pdu_session_res_to_modify_item_s, security_ind_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_ind_s::desc},
    {offsetof(pdu_session_res_to_modify_item_s, pdu_session_res_dl_ambr),
     (int32_t)offsetof(pdu_session_res_to_modify_item_s, pdu_session_res_dl_ambr_present),
     asn1::table::field_op::int_u64,
     true,
     true,
     INT64_C(0),
     UINT64_C(4000000000000),
     nullptr},
    {offsetof(pdu_session_res_to_modify_item_s, ng_ul_up_tnl_info),
     (int32_t)offsetof(pdu_session_res_to_modify_item_s, ng_ul_up_tnl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(pdu_session_res_to_modify_item_s, pdu_session_data_forwarding_info_request),
     (int32_t)offsetof(pdu_session_res_to_modify_item_s, pdu_session_data_forwarding_info_request_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_request_s::desc},
    {offsetof(pdu_session_res_to_modify_item_s, pdu_session_data_forwarding_info),
     (int32_t)offsetof(pdu_session_res_to_modify_item_s, pdu_session_data_forwarding_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_s::desc},
    {offsetof(pdu_session_res_to_modify_item_s, pdu_session_inactivity_timer),
     (int32_t)offsetof(pdu_session_res_to_modify_item_s, pdu_session_inactivity_timer_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(7200),
     nullptr},
    {offsetof(pdu_session_res_to_modify_item_s, network_instance),
     (int32_t)offsetof(pdu_session_res_to_modify_item_s, network_instance_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(256),
     nullptr},
    {offsetof(pdu_session_res_to_modify_item_s, drb_to_setup_list_ng_ran),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pdu_session_res_to_modify_item_s::drb_to_setup_list_ng_ran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_to_setup_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_to_modify_item_s, drb_to_modify_list_ng_ran),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pdu_session_res_to_modify_item_s::drb_to_modify_list_ng_ran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_to_modify_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_to_modify_item_s, drb_to_rem_list_ng_ran),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pdu_session_res_to_modify_item_s::drb_to_rem_list_ng_ran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_to_rem_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_to_modify_item_s, ie_exts),
     (int32_t)offsetof(pdu_session_res_to_modify_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &pdu_session_res_to_modify_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_pdu_session_res_to_modify_item_s_names[] = {
    {"pDU-Session-ID", nullptr},
    {"securityIndication", nullptr},
    {"pDU-Session-Resource-DL-AMBR", nullptr},
    {"nG-UL-UP-TNL-Information", nullptr},
    {"pDU-Session-Data-Forwarding-Information-Request", nullptr},
    {"pDU-Session-Data-Forwarding-Information", nullptr},
    {"pDU-Session-Inactivity-Timer", nullptr},
    {"networkInstance", nullptr},
    {"dRB-To-Setup-List-NG-RAN", nullptr},
    {"dRB-To-Modify-List-NG-RAN", nullptr},
    {"dRB-To-Remove-List-NG-RAN", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pdu_session_res_to_modify_item_s_opts[] = {
    {1, (int32_t)offsetof(pdu_session_res_to_modify_item_s, security_ind_present)},
    {2, (int32_t)offsetof(pdu_session_res_to_modify_item_s, pdu_session_res_dl_ambr_present)},
    {3, (int32_t)offsetof(pdu_session_res_to_modify_item_s, ng_ul_up_tnl_info_present)},
    {4, (int32_t)offsetof(pdu_session_res_to_modify_item_s, pdu_session_data_forwarding_info_request_present)},
    {5, (int32_t)offsetof(pdu_session_res_to_modify_item_s, pdu_session_data_forwarding_info_present)},
    {6, (int32_t)offsetof(pdu_session_res_to_modify_item_s, pdu_session_inactivity_timer_present)},
    {7, (int32_t)offsetof(pdu_session_res_to_modify_item_s, network_instance_present)},
    {8, asn1::table::seq_field::list_presence},
    {9, asn1::table::seq_field::list_presence},
    {10, asn1::table::seq_field::list_presence},
    {11, (int32_t)offsetof(pdu_session_res_to_modify_item_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_session_res_to_modify_item_s::desc = {
    {seq_table_pdu_session_res_to_modify_item_s_fields,
     12,
     UINT64_C(0x1),
     seq_table_pdu_session_res_to_modify_item_s_opts,
     11},
    true,
    true,
    nullptr,
    0,
    seq_table_pdu_session_res_to_modify_item_s_names};

// UserPlaneErrorIndicator ::= ENUMERATED
static const char*           enum_names_user_plane_error_ind_e_values[] = {"gTP-U-error-indication-received"};
const asn1::table::enum_desc user_plane_error_ind_opts::desc            = {enum_names_user_plane_error_ind_e_values,
                                                                           1,
                                                                           "user_plane_error_ind_e"};

static const uint32_t objset_id_pdu_session_res_to_rem_item_ext_ies_o[]   = {0, 215};
static const uint8_t  objset_crit_pdu_session_res_to_rem_item_ext_ies_o[] = {crit_e::ignore, crit_e::ignore};

// PDU-Session-Resource-To-Remove-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_pdu_session_res_to_rem_item_ext_ies_container_values[] = {
    {offsetof(pdu_session_res_to_rem_item_ext_ies_container, cause),
     (int32_t)offsetof(pdu_session_res_to_rem_item_ext_ies_container, cause_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc},
    {offsetof(pdu_session_res_to_rem_item_ext_ies_container, user_plane_error_ind),
     (int32_t)offsetof(pdu_session_res_to_rem_item_ext_ies_container, user_plane_error_ind_present),
     asn1::table::enum_op<decltype(pdu_session_res_to_rem_item_ext_ies_container::user_plane_error_ind)>(),
     decltype(pdu_session_res_to_rem_item_ext_ies_container::user_plane_error_ind)::has_ext,
     false,
     decltype(pdu_session_res_to_rem_item_ext_ies_container::user_plane_error_ind)::nof_types,
     decltype(pdu_session_res_to_rem_item_ext_ies_container::user_plane_error_ind)::nof_exts,
     nullptr,
     &user_plane_error_ind_opts::desc}};
static const int32_t ie_table_pdu_session_res_to_rem_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(pdu_session_res_to_rem_item_ext_ies_container, cause_present),
    (int32_t)offsetof(pdu_session_res_to_rem_item_ext_ies_container, user_plane_error_ind_present)};
const asn1::table::ie_container_desc pdu_session_res_to_rem_item_ext_ies_container::desc = {
    objset_id_pdu_session_res_to_rem_item_ext_ies_o,
    objset_crit_pdu_session_res_to_rem_item_ext_ies_o,
    ie_table_pdu_session_res_to_rem_item_ext_ies_container_values,
    ie_table_pdu_session_res_to_rem_item_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// PDU-Session-Resource-To-Remove-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_session_res_to_rem_item_s_fields[] = {
    {offsetof(pdu_session_res_to_rem_item_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pdu_session_res_to_rem_item_s, ie_exts),
     (int32_t)offsetof(pdu_session_res_to_rem_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &pdu_session_res_to_rem_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_pdu_session_res_to_rem_item_s_names[] = {{"pDU-Session-ID", nullptr},
                                                                                        {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_pdu_session_res_to_rem_item_s_opts[]  = {
    {1, (int32_t)offsetof(pdu_session_res_to_rem_item_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_session_res_to_rem_item_s::desc = {
    {seq_table_pdu_session_res_to_rem_item_s_fields, 2, UINT64_C(0x1), seq_table_pdu_session_res_to_rem_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_pdu_session_res_to_rem_item_s_names};

// PDU-Session-Type ::= ENUMERATED
static const char* enum_names_pdu_session_type_e_values[] = {"ipv4", "ipv6", "ipv4v6", "ethernet", "unstructured"};
const asn1::table::enum_desc pdu_session_type_opts::desc  = {enum_names_pdu_session_type_e_values,
                                                             5,
                                                             "pdu_session_type_e"};

static const uint32_t objset_id_pdu_session_res_to_setup_item_ext_ies_o[]   = {78, 97, 96, 105, 198};
static const uint8_t  objset_crit_pdu_session_res_to_setup_item_ext_ies_o[] = {crit_e::ignore,
                                                                               crit_e::ignore,
                                                                               crit_e::ignore,
                                                                               crit_e::ignore,
                                                                               crit_e::ignore};

// PDU-Session-Resource-To-Setup-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_pdu_session_res_to_setup_item_ext_ies_container_values[] = {
    {offsetof(pdu_session_res_to_setup_item_ext_ies_container, common_network_instance),
     (int32_t)offsetof(pdu_session_res_to_setup_item_ext_ies_container, common_network_instance_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pdu_session_res_to_setup_item_ext_ies_container::common_network_instance),
                                asn1::table::string_json>::value},
    {offsetof(pdu_session_res_to_setup_item_ext_ies_container, redundant_n_g_ul_up_tnl_info),
     (int32_t)offsetof(pdu_session_res_to_setup_item_ext_ies_container, redundant_n_g_ul_up_tnl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(pdu_session_res_to_setup_item_ext_ies_container, redundant_common_network_instance),
     (int32_t)offsetof(pdu_session_res_to_setup_item_ext_ies_container, redundant_common_network_instance_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<
         decltype(pdu_session_res_to_setup_item_ext_ies_container::redundant_common_network_instance),
         asn1::table::string_json>::value},
    {offsetof(pdu_session_res_to_setup_item_ext_ies_container, redundant_pdu_session_info),
     (int32_t)offsetof(pdu_session_res_to_setup_item_ext_ies_container, redundant_pdu_session_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &redundant_pdu_session_info_s::desc},
    {offsetof(pdu_session_res_to_setup_item_ext_ies_container, special_trigger_purpose),
     (int32_t)offsetof(pdu_session_res_to_setup_item_ext_ies_container, special_trigger_purpose_present),
     asn1::table::enum_op<decltype(pdu_session_res_to_setup_item_ext_ies_container::special_trigger_purpose)>(),
     decltype(pdu_session_res_to_setup_item_ext_ies_container::special_trigger_purpose)::has_ext,
     false,
     decltype(pdu_session_res_to_setup_item_ext_ies_container::special_trigger_purpose)::nof_types,
     decltype(pdu_session_res_to_setup_item_ext_ies_container::special_trigger_purpose)::nof_exts,
     nullptr,
     &special_trigger_purpose_opts::desc}};
static const int32_t ie_table_pdu_session_res_to_setup_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(pdu_session_res_to_setup_item_ext_ies_container, common_network_instance_present),
    (int32_t)offsetof(pdu_session_res_to_setup_item_ext_ies_container, redundant_n_g_ul_up_tnl_info_present),
    (int32_t)offsetof(pdu_session_res_to_setup_item_ext_ies_container, redundant_common_network_instance_present),
    (int32_t)offsetof(pdu_session_res_to_setup_item_ext_ies_container, redundant_pdu_session_info_present),
    (int32_t)offsetof(pdu_session_res_to_setup_item_ext_ies_container, special_trigger_purpose_present)};
const asn1::table::ie_container_desc pdu_session_res_to_setup_item_ext_ies_container::desc = {
    objset_id_pdu_session_res_to_setup_item_ext_ies_o,
    objset_crit_pdu_session_res_to_setup_item_ext_ies_o,
    ie_table_pdu_session_res_to_setup_item_ext_ies_container_values,
    ie_table_pdu_session_res_to_setup_item_ext_ies_container_presences,
    5,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// PDU-Session-Resource-To-Setup-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_session_res_to_setup_item_s_fields[] = {
    {offsetof(pdu_session_res_to_setup_item_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pdu_session_res_to_setup_item_s, pdu_session_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pdu_session_res_to_setup_item_s::pdu_session_type)>(),
     decltype(pdu_session_res_to_setup_item_s::pdu_session_type)::has_ext,
     false,
     decltype(pdu_session_res_to_setup_item_s::pdu_session_type)::nof_types,
     decltype(pdu_session_res_to_setup_item_s::pdu_session_type)::nof_exts,
     nullptr,
     &pdu_session_type_opts::desc},
    {offsetof(pdu_session_res_to_setup_item_s, snssai),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &snssai_s::desc},
    {offsetof(pdu_session_res_to_setup_item_s, security_ind),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_ind_s::desc},
    {offsetof(pdu_session_res_to_setup_item_s, pdu_session_res_dl_ambr),
     (int32_t)offsetof(pdu_session_res_to_setup_item_s, pdu_session_res_dl_ambr_present),
     asn1::table::field_op::int_u64,
     true,
     true,
     INT64_C(0),
     UINT64_C(4000000000000),
     nullptr},
    {offsetof(pdu_session_res_to_setup_item_s, ng_ul_up_tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(pdu_session_res_to_setup_item_s, pdu_session_data_forwarding_info_request),
     (int32_t)offsetof(pdu_session_res_to_setup_item_s, pdu_session_data_forwarding_info_request_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_request_s::desc},
    {offsetof(pdu_session_res_to_setup_item_s, pdu_session_inactivity_timer),
     (int32_t)offsetof(pdu_session_res_to_setup_item_s, pdu_session_inactivity_timer_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(7200),
     nullptr},
    {offsetof(pdu_session_res_to_setup_item_s, existing_allocated_ng_dl_up_tnl_info),
     (int32_t)offsetof(pdu_session_res_to_setup_item_s, existing_allocated_ng_dl_up_tnl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(pdu_session_res_to_setup_item_s, network_instance),
     (int32_t)offsetof(pdu_session_res_to_setup_item_s, network_instance_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(256),
     nullptr},
    {offsetof(pdu_session_res_to_setup_item_s, drb_to_setup_list_ng_ran),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pdu_session_res_to_setup_item_s::drb_to_setup_list_ng_ran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_to_setup_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_to_setup_item_s, ie_exts),
     (int32_t)offsetof(pdu_session_res_to_setup_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &pdu_session_res_to_setup_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_pdu_session_res_to_setup_item_s_names[] = {
    {"pDU-Session-ID", nullptr},
    {"pDU-Session-Type", nullptr},
    {"sNSSAI", nullptr},
    {"securityIndication", nullptr},
    {"pDU-Session-Resource-DL-AMBR", nullptr},
    {"nG-UL-UP-TNL-Information", nullptr},
    {"pDU-Session-Data-Forwarding-Information-Request", nullptr},
    {"pDU-Session-Inactivity-Timer", nullptr},
    {"existing-Allocated-NG-DL-UP-TNL-Info", nullptr},
    {"networkInstance", nullptr},
    {"dRB-To-Setup-List-NG-RAN", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pdu_session_res_to_setup_item_s_opts[] = {
    {4, (int32_t)offsetof(pdu_session_res_to_setup_item_s, pdu_session_res_dl_ambr_present)},
    {6, (int32_t)offsetof(pdu_session_res_to_setup_item_s, pdu_session_data_forwarding_info_request_present)},
    {7, (int32_t)offsetof(pdu_session_res_to_setup_item_s, pdu_session_inactivity_timer_present)},
    {8, (int32_t)offsetof(pdu_session_res_to_setup_item_s, existing_allocated_ng_dl_up_tnl_info_present)},
    {9, (int32_t)offsetof(pdu_session_res_to_setup_item_s, network_instance_present)},
    {11, (int32_t)offsetof(pdu_session_res_to_setup_item_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_session_res_to_setup_item_s::desc = {{seq_table_pdu_session_res_to_setup_item_s_fields,
                                                                      12,
                                                                      UINT64_C(0x42f),
                                                                      seq_table_pdu_session_res_to_setup_item_s_opts,
                                                                      6},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_pdu_session_res_to_setup_item_s_names};

static const uint32_t objset_id_pdu_session_res_to_setup_mod_item_ext_ies_o[]   = {79, 78, 97, 96, 198};
static const uint8_t  objset_crit_pdu_session_res_to_setup_mod_item_ext_ies_o[] = {crit_e::ignore,
                                                                                   crit_e::ignore,
                                                                                   crit_e::ignore,
                                                                                   crit_e::ignore,
                                                                                   crit_e::ignore};

// PDU-Session-Resource-To-Setup-Mod-Item-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_pdu_session_res_to_setup_mod_item_ext_ies_container_values[] = {
    {offsetof(pdu_session_res_to_setup_mod_item_ext_ies_container, network_instance),
     (int32_t)offsetof(pdu_session_res_to_setup_mod_item_ext_ies_container, network_instance_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(256),
     nullptr},
    {offsetof(pdu_session_res_to_setup_mod_item_ext_ies_container, common_network_instance),
     (int32_t)offsetof(pdu_session_res_to_setup_mod_item_ext_ies_container, common_network_instance_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pdu_session_res_to_setup_mod_item_ext_ies_container::common_network_instance),
                                asn1::table::string_json>::value},
    {offsetof(pdu_session_res_to_setup_mod_item_ext_ies_container, redundant_n_g_ul_up_tnl_info),
     (int32_t)offsetof(pdu_session_res_to_setup_mod_item_ext_ies_container, redundant_n_g_ul_up_tnl_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(pdu_session_res_to_setup_mod_item_ext_ies_container, redundant_common_network_instance),
     (int32_t)offsetof(pdu_session_res_to_setup_mod_item_ext_ies_container, redundant_common_network_instance_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<
         decltype(pdu_session_res_to_setup_mod_item_ext_ies_container::redundant_common_network_instance),
         asn1::table::string_json>::value},
    {offsetof(pdu_session_res_to_setup_mod_item_ext_ies_container, special_trigger_purpose),
     (int32_t)offsetof(pdu_session_res_to_setup_mod_item_ext_ies_container, special_trigger_purpose_present),
     asn1::table::enum_op<decltype(pdu_session_res_to_setup_mod_item_ext_ies_container::special_trigger_purpose)>(),
     decltype(pdu_session_res_to_setup_mod_item_ext_ies_container::special_trigger_purpose)::has_ext,
     false,
     decltype(pdu_session_res_to_setup_mod_item_ext_ies_container::special_trigger_purpose)::nof_types,
     decltype(pdu_session_res_to_setup_mod_item_ext_ies_container::special_trigger_purpose)::nof_exts,
     nullptr,
     &special_trigger_purpose_opts::desc}};
static const int32_t ie_table_pdu_session_res_to_setup_mod_item_ext_ies_container_presences[] = {
    (int32_t)offsetof(pdu_session_res_to_setup_mod_item_ext_ies_container, network_instance_present),
    (int32_t)offsetof(pdu_session_res_to_setup_mod_item_ext_ies_container, common_network_instance_present),
    (int32_t)offsetof(pdu_session_res_to_setup_mod_item_ext_ies_container, redundant_n_g_ul_up_tnl_info_present),
    (int32_t)offsetof(pdu_session_res_to_setup_mod_item_ext_ies_container, redundant_common_network_instance_present),
    (int32_t)offsetof(pdu_session_res_to_setup_mod_item_ext_ies_container, special_trigger_purpose_present)};
const asn1::table::ie_container_desc pdu_session_res_to_setup_mod_item_ext_ies_container::desc = {
    objset_id_pdu_session_res_to_setup_mod_item_ext_ies_o,
    objset_crit_pdu_session_res_to_setup_mod_item_ext_ies_o,
    ie_table_pdu_session_res_to_setup_mod_item_ext_ies_container_values,
    ie_table_pdu_session_res_to_setup_mod_item_ext_ies_container_presences,
    5,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// PDU-Session-Resource-To-Setup-Mod-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_session_res_to_setup_mod_item_s_fields[] = {
    {offsetof(pdu_session_res_to_setup_mod_item_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pdu_session_res_to_setup_mod_item_s, pdu_session_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pdu_session_res_to_setup_mod_item_s::pdu_session_type)>(),
     decltype(pdu_session_res_to_setup_mod_item_s::pdu_session_type)::has_ext,
     false,
     decltype(pdu_session_res_to_setup_mod_item_s::pdu_session_type)::nof_types,
     decltype(pdu_session_res_to_setup_mod_item_s::pdu_session_type)::nof_exts,
     nullptr,
     &pdu_session_type_opts::desc},
    {offsetof(pdu_session_res_to_setup_mod_item_s, snssai),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &snssai_s::desc},
    {offsetof(pdu_session_res_to_setup_mod_item_s, security_ind),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_ind_s::desc},
    {offsetof(pdu_session_res_to_setup_mod_item_s, pdu_session_res_ambr),
     (int32_t)offsetof(pdu_session_res_to_setup_mod_item_s, pdu_session_res_ambr_present),
     asn1::table::field_op::int_u64,
     true,
     true,
     INT64_C(0),
     UINT64_C(4000000000000),
     nullptr},
    {offsetof(pdu_session_res_to_setup_mod_item_s, ng_ul_up_tnl_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &up_tnl_info_c::desc},
    {offsetof(pdu_session_res_to_setup_mod_item_s, pdu_session_data_forwarding_info_request),
     (int32_t)offsetof(pdu_session_res_to_setup_mod_item_s, pdu_session_data_forwarding_info_request_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &data_forwarding_info_request_s::desc},
    {offsetof(pdu_session_res_to_setup_mod_item_s, pdu_session_inactivity_timer),
     (int32_t)offsetof(pdu_session_res_to_setup_mod_item_s, pdu_session_inactivity_timer_present),
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(1),
     UINT64_C(7200),
     nullptr},
    {offsetof(pdu_session_res_to_setup_mod_item_s, drb_to_setup_mod_list_ng_ran),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pdu_session_res_to_setup_mod_item_s::drb_to_setup_mod_list_ng_ran),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_to_setup_mod_item_ng_ran_s::desc},
    {offsetof(pdu_session_res_to_setup_mod_item_s, ie_exts),
     (int32_t)offsetof(pdu_session_res_to_setup_mod_item_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &pdu_session_res_to_setup_mod_item_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_pdu_session_res_to_setup_mod_item_s_names[] = {
    {"pDU-Session-ID", nullptr},
    {"pDU-Session-Type", nullptr},
    {"sNSSAI", nullptr},
    {"securityIndication", nullptr},
    {"pDU-Session-Resource-AMBR", nullptr},
    {"nG-UL-UP-TNL-Information", nullptr},
    {"pDU-Session-Data-Forwarding-Information-Request", nullptr},
    {"pDU-Session-Inactivity-Timer", nullptr},
    {"dRB-To-Setup-Mod-List-NG-RAN", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_pdu_session_res_to_setup_mod_item_s_opts[] = {
    {4, (int32_t)offsetof(pdu_session_res_to_setup_mod_item_s, pdu_session_res_ambr_present)},
    {6, (int32_t)offsetof(pdu_session_res_to_setup_mod_item_s, pdu_session_data_forwarding_info_request_present)},
    {7, (int32_t)offsetof(pdu_session_res_to_setup_mod_item_s, pdu_session_inactivity_timer_present)},
    {9, (int32_t)offsetof(pdu_session_res_to_setup_mod_item_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_session_res_to_setup_mod_item_s::desc = {
    {seq_table_pdu_session_res_to_setup_mod_item_s_fields,
     10,
     UINT64_C(0x12f),
     seq_table_pdu_session_res_to_setup_mod_item_s_opts,
     4},
    true,
    true,
    nullptr,
    0,
    seq_table_pdu_session_res_to_setup_mod_item_s_names};

// PDU-Session-To-Notify-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdu_session_to_notify_item_s_fields[] = {
    {offsetof(pdu_session_to_notify_item_s, pdu_session_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(pdu_session_to_notify_item_s, qos_flow_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     64,
     &asn1::table::item_list_codec<decltype(pdu_session_to_notify_item_s::qos_flow_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &qos_flow_item_s::desc},
    {offsetof(pdu_session_to_notify_item_s, ie_exts),
     (int32_t)offsetof(pdu_session_to_notify_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(pdu_session_to_notify_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pdu_session_to_notify_item_s_names[] = {{"pDU-Session-ID", nullptr},
                                                                                       {"qoS-Flow-List", nullptr},
                                                                                       {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_pdu_session_to_notify_item_s_opts[]  = {
    {2, (int32_t)offsetof(pdu_session_to_notify_item_s, ie_exts_present)}};
const asn1::table::seq_desc pdu_session_to_notify_item_s::desc = {
    {seq_table_pdu_session_to_notify_item_s_fields, 3, UINT64_C(0x3), seq_table_pdu_session_to_notify_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_pdu_session_to_notify_item_s_names};

// PrivacyIndicator ::= ENUMERATED
static const char*           enum_names_privacy_ind_e_values[] = {"immediate-MDT", "logged-MDT"};
const asn1::table::enum_desc privacy_ind_opts::desc            = {enum_names_privacy_ind_e_values, 2, "privacy_ind_e"};

// QoS-Parameters-Support-List ::= SEQUENCE
static const asn1::table::seq_field seq_table_qos_params_support_list_s_fields[] = {
    {offsetof(qos_params_support_list_s, eutran_qos_support_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     256,
     &asn1::table::item_list_codec<decltype(qos_params_support_list_s::eutran_qos_support_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &eutran_qos_support_item_s::desc},
    {offsetof(qos_params_support_list_s, ng_ran_qos_support_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     256,
     &asn1::table::item_list_codec<decltype(qos_params_support_list_s::ng_ran_qos_support_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ng_ran_qos_support_item_s::desc},
    {offsetof(qos_params_support_list_s, ie_exts),
     (int32_t)offsetof(qos_params_support_list_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(qos_params_support_list_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_qos_params_support_list_s_names[] = {
    {"eUTRAN-QoS-Support-List", nullptr},
    {"nG-RAN-QoS-Support-List", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_qos_params_support_list_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(qos_params_support_list_s, ie_exts_present)}};
const asn1::table::seq_desc qos_params_support_list_s::desc = {
    {seq_table_qos_params_support_list_s_fields, 3, UINT64_C(0x0), seq_table_qos_params_support_list_s_opts, 3},
    true,
    true,
    nullptr,
    0,
    seq_table_qos_params_support_list_s_names};

// RegistrationRequest ::= ENUMERATED
static const char*           enum_names_regist_request_e_values[] = {"start", "stop"};
const asn1::table::enum_desc regist_request_opts::desc = {enum_names_regist_request_e_values, 2, "regist_request_e"};

// ReportingPeriodicity ::= ENUMERATED
static const char*           enum_names_report_periodicity_e_values[] = {"ms500",
                                                                         "ms1000",
                                                                         "ms2000",
                                                                         "ms5000",
                                                                         "ms10000",
                                                                         "ms20000",
                                                                         "ms30000",
                                                                         "ms40000",
                                                                         "ms50000",
                                                                         "ms60000",
                                                                         "ms70000",
                                                                         "ms80000",
                                                                         "ms90000",
                                                                         "ms100000",
                                                                         "ms110000",
                                                                         "ms120000"};
const asn1::table::enum_desc report_periodicity_opts::desc            = {enum_names_report_periodicity_e_values,
                                                                         16,
                                                                         "report_periodicity_e"};
uint32_t                     report_periodicity_opts::to_number() const
{
  static const uint32_t numbers[] = {
      500, 1000, 2000, 5000, 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000, 110000, 120000};
  return map_enum_number(numbers, 16, value, "report_periodicity_e");
}

// SCGActivationStatus ::= ENUMERATED
static const char*           enum_names_scg_activation_status_e_values[] = {"scg-activated", "scg-deactivated"};
const asn1::table::enum_desc scg_activation_status_opts::desc            = {enum_names_scg_activation_status_e_values,
                                                                            2,
                                                                            "scg_activation_status_e"};

// SDT-data-size-threshold-Crossed ::= ENUMERATED
static const char*           enum_names_sdt_data_size_thres_crossed_e_values[] = {"true"};
const asn1::table::enum_desc sdt_data_size_thres_crossed_opts::desc = {enum_names_sdt_data_size_thres_crossed_e_values,
                                                                       1,
                                                                       "sdt_data_size_thres_crossed_e"};

// SDTContinueROHC ::= ENUMERATED
static const char*           enum_names_sdt_continue_rohc_e_values[] = {"true"};
const asn1::table::enum_desc sdt_continue_rohc_opts::desc            = {enum_names_sdt_continue_rohc_e_values,
                                                                        1,
                                                                        "sdt_continue_rohc_e"};

// SecurityAlgorithm ::= SEQUENCE
static const asn1::table::seq_field seq_table_security_algorithm_s_fields[] = {
    {offsetof(security_algorithm_s, ciphering_algorithm),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(security_algorithm_s::ciphering_algorithm)>(),
     decltype(security_algorithm_s::ciphering_algorithm)::has_ext,
     false,
     decltype(security_algorithm_s::ciphering_algorithm)::nof_types,
     decltype(security_algorithm_s::ciphering_algorithm)::nof_exts,
     nullptr,
     &ciphering_algorithm_opts::desc},
    {offsetof(security_algorithm_s, integrity_protection_algorithm),
     (int32_t)offsetof(security_algorithm_s, integrity_protection_algorithm_present),
     asn1::table::enum_op<decltype(security_algorithm_s::integrity_protection_algorithm)>(),
     decltype(security_algorithm_s::integrity_protection_algorithm)::has_ext,
     false,
     decltype(security_algorithm_s::integrity_protection_algorithm)::nof_types,
     decltype(security_algorithm_s::integrity_protection_algorithm)::nof_exts,
     nullptr,
     &integrity_protection_algorithm_opts::desc},
    {offsetof(security_algorithm_s, ie_exts),
     (int32_t)offsetof(security_algorithm_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(security_algorithm_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_security_algorithm_s_names[] = {
    {"cipheringAlgorithm", nullptr},
    {"integrityProtectionAlgorithm", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_security_algorithm_s_opts[] = {
    {1, (int32_t)offsetof(security_algorithm_s, integrity_protection_algorithm_present)},
    {2, (int32_t)offsetof(security_algorithm_s, ie_exts_present)}};
const asn1::table::seq_desc security_algorithm_s::desc = {
    {seq_table_security_algorithm_s_fields, 3, UINT64_C(0x1), seq_table_security_algorithm_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_security_algorithm_s_names};

// UPSecuritykey ::= SEQUENCE
static const asn1::table::seq_field seq_table_up_securitykey_s_fields[] = {
    {offsetof(up_securitykey_s, encryption_key),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(up_securitykey_s::encryption_key), asn1::table::string_json>::value},
    {offsetof(up_securitykey_s, integrity_protection_key),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(up_securitykey_s::integrity_protection_key),
                                      asn1::table::string_json>::value},
    {offsetof(up_securitykey_s, ie_exts),
     (int32_t)offsetof(up_securitykey_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(up_securitykey_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_up_securitykey_s_names[] = {{"encryptionKey", nullptr},
                                                                           {"integrityProtectionKey", nullptr},
                                                                           {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_up_securitykey_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(up_securitykey_s, ie_exts_present)}};
const asn1::table::seq_desc up_securitykey_s::desc = {
    {seq_table_up_securitykey_s_fields, 3, UINT64_C(0x1), seq_table_up_securitykey_s_opts, 2},
    true,
    true,
    nullptr,
    0,
    seq_table_up_securitykey_s_names};

// SecurityInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_security_info_s_fields[] = {
    {offsetof(security_info_s, security_algorithm),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_algorithm_s::desc},
    {offsetof(security_info_s, up_securitykey),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &up_securitykey_s::desc},
    {offsetof(security_info_s, ie_exts),
     (int32_t)offsetof(security_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(security_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_security_info_s_names[] = {{"securityAlgorithm", nullptr},
                                                                          {"uPSecuritykey", nullptr},
                                                                          {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_security_info_s_opts[]  = {
    {2, (int32_t)offsetof(security_info_s, ie_exts_present)}};
const asn1::table::seq_desc security_info_s::desc = {
    {seq_table_security_info_s_fields, 3, UINT64_C(0x3), seq_table_security_info_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_security_info_s_names};

// TNL-AvailableCapacityIndicator ::= SEQUENCE
static const asn1::table::seq_field seq_table_tnl_available_capacity_ind_s_fields[] = {
    {offsetof(tnl_available_capacity_ind_s, dl_tnl_offered_capacity),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(16777216),
     nullptr},
    {offsetof(tnl_available_capacity_ind_s, dl_tnl_available_capacity),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(100),
     nullptr},
    {offsetof(tnl_available_capacity_ind_s, ul_tnl_offered_capacity),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     INT64_C(0),
     UINT64_C(16777216),
     nullptr},
    {offsetof(tnl_available_capacity_ind_s, ul_tnl_available_capacity),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     INT64_C(0),
     UINT64_C(100),
     nullptr},
    {offsetof(tnl_available_capacity_ind_s, ie_exts),
     (int32_t)offsetof(tnl_available_capacity_ind_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tnl_available_capacity_ind_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_tnl_available_capacity_ind_s_names[] = {
    {"dL-TNL-OfferedCapacity", nullptr},
    {"dL-TNL-AvailableCapacity", nullptr},
    {"uL-TNL-OfferedCapacity", nullptr},
    {"uL-TNL-AvailableCapacity", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_tnl_available_capacity_ind_s_opts[] = {
    {4, (int32_t)offsetof(tnl_available_capacity_ind_s, ie_exts_present)}};
const asn1::table::seq_desc tnl_available_capacity_ind_s::desc = {
    {seq_table_tnl_available_capacity_ind_s_fields, 5, UINT64_C(0xf), seq_table_tnl_available_capacity_ind_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_tnl_available_capacity_ind_s_names};

// TimeToWait ::= ENUMERATED
static const char*           enum_names_time_to_wait_e_values[] = {"v1s", "v2s", "v5s", "v10s", "v20s", "v60s"};
const asn1::table::enum_desc time_to_wait_opts::desc = {enum_names_time_to_wait_e_values, 6, "time_to_wait_e"};
uint8_t                      time_to_wait_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 5, 10, 20, 60};
  return map_enum_number(numbers, 6, value, "time_to_wait_e");
}

// TraceDepth ::= ENUMERATED
static const char*           enum_names_trace_depth_e_values[] = {"minimum",
                                                                  "medium",
                                                                  "maximum",
                                                                  "minimumWithoutVendorSpecificExtension",
                                                                  "mediumWithoutVendorSpecificExtension",
                                                                  "maximumWithoutVendorSpecificExtension"};
const asn1::table::enum_desc trace_depth_opts::desc            = {enum_names_trace_depth_e_values, 6, "trace_depth_e"};

static const uint32_t objset_id_trace_activation_ext_ies_o[]   = {112, 116};
static const uint8_t  objset_crit_trace_activation_ext_ies_o[] = {crit_e::ignore, crit_e::ignore};

// TraceActivation-ExtIEs ::= OBJECT SET OF E1AP-PROTOCOL-EXTENSION
static const asn1::table::seq_field ie_table_trace_activation_ext_ies_container_values[] = {
    {offsetof(trace_activation_ext_ies_container, mdt_cfg),
     (int32_t)offsetof(trace_activation_ext_ies_container, mdt_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mdt_cfg_s::desc},
    {offsetof(trace_activation_ext_ies_container, trace_collection_entity_uri),
     (int32_t)offsetof(trace_activation_ext_ies_container, trace_collection_entity_uri_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trace_activation_ext_ies_container::trace_collection_entity_uri),
                                asn1::table::string_json>::value}};
static const int32_t ie_table_trace_activation_ext_ies_container_presences[] = {
    (int32_t)offsetof(trace_activation_ext_ies_container, mdt_cfg_present),
    (int32_t)offsetof(trace_activation_ext_ies_container, trace_collection_entity_uri_present)};
const asn1::table::ie_container_desc trace_activation_ext_ies_container::desc = {
    objset_id_trace_activation_ext_ies_o,
    objset_crit_trace_activation_ext_ies_o,
    ie_table_trace_activation_ext_ies_container_values,
    ie_table_trace_activation_ext_ies_container_presences,
    2,
    0,
    1,
    65535,
    true,
    "extensionValue"};

// TraceActivation ::= SEQUENCE
static const asn1::table::seq_field seq_table_trace_activation_s_fields[] = {
    {offsetof(trace_activation_s, trace_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trace_activation_s::trace_id), asn1::table::string_json>::value},
    {offsetof(trace_activation_s, interfaces_to_trace),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trace_activation_s::interfaces_to_trace), asn1::table::struct_json>::value},
    {offsetof(trace_activation_s, trace_depth),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(trace_activation_s::trace_depth)>(),
     decltype(trace_activation_s::trace_depth)::has_ext,
     false,
     decltype(trace_activation_s::trace_depth)::nof_types,
     decltype(trace_activation_s::trace_depth)::nof_exts,
     nullptr,
     &trace_depth_opts::desc},
    {offsetof(trace_activation_s, trace_collection_entity_ip_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trace_activation_s::trace_collection_entity_ip_address),
                                asn1::table::struct_json>::value},
    {offsetof(trace_activation_s, ie_exts),
     (int32_t)offsetof(trace_activation_s, ie_exts_present),
     asn1::table::field_op::ie_container,
     false,
     false,
     0,
     0,
     nullptr,
     &trace_activation_ext_ies_container::desc}};
static const asn1::table::field_name seq_table_trace_activation_s_names[] = {
    {"traceID", nullptr},
    {"interfacesToTrace", nullptr},
    {"traceDepth", nullptr},
    {"traceCollectionEntityIPAddress", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_trace_activation_s_opts[] = {
    {4, (int32_t)offsetof(trace_activation_s, ie_exts_present)}};
const asn1::table::seq_desc trace_activation_s::desc = {
    {seq_table_trace_activation_s_fields, 5, UINT64_C(0xf), seq_table_trace_activation_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_trace_activation_s_names};

// Transport-UP-Layer-Addresses-Info-To-Add-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_transport_up_layer_addresses_info_to_add_item_s_fields[] = {
    {offsetof(transport_up_layer_addresses_info_to_add_item_s, ip_sec_transport_layer_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<
         decltype(transport_up_layer_addresses_info_to_add_item_s::ip_sec_transport_layer_address),
         asn1::table::struct_json>::value},
    {offsetof(transport_up_layer_addresses_info_to_add_item_s, gtp_transport_layer_addresses_to_add),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16,
     &asn1::table::item_list_codec<
         decltype(transport_up_layer_addresses_info_to_add_item_s::gtp_transport_layer_addresses_to_add),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &gtp_tla_item_s::desc},
    {offsetof(transport_up_layer_addresses_info_to_add_item_s, ie_exts),
     (int32_t)offsetof(transport_up_layer_addresses_info_to_add_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(transport_up_layer_addresses_info_to_add_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_transport_up_layer_addresses_info_to_add_item_s_names[] = {
    {"iP-SecTransportLayerAddress", nullptr},
    {"gTPTransportLayerAddressesToAdd", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_transport_up_layer_addresses_info_to_add_item_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(transport_up_layer_addresses_info_to_add_item_s, ie_exts_present)}};
const asn1::table::seq_desc transport_up_layer_addresses_info_to_add_item_s::desc = {
    {seq_table_transport_up_layer_addresses_info_to_add_item_s_fields,
     3,
     UINT64_C(0x1),
     seq_table_transport_up_layer_addresses_info_to_add_item_s_opts,
     2},
    true,
    true,
    nullptr,
    0,
    seq_table_transport_up_layer_addresses_info_to_add_item_s_names};

// Transport-UP-Layer-Addresses-Info-To-Remove-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_transport_up_layer_addresses_info_to_rem_item_s_fields[] = {
    {offsetof(transport_up_layer_addresses_info_to_rem_item_s, ip_sec_transport_layer_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<
         decltype(transport_up_layer_addresses_info_to_rem_item_s::ip_sec_transport_layer_address),
         asn1::table::struct_json>::value},
    {offsetof(transport_up_layer_addresses_info_to_rem_item_s, gtp_transport_layer_addresses_to_rem),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16,
     &asn1::table::item_list_codec<
         decltype(transport_up_layer_addresses_info_to_rem_item_s::gtp_transport_layer_addresses_to_rem),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &gtp_tla_item_s::desc},
    {offsetof(transport_up_layer_addresses_info_to_rem_item_s, ie_exts),
     (int32_t)offsetof(transport_up_layer_addresses_info_to_rem_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(transport_up_layer_addresses_info_to_rem_item_s::ie_exts),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_transport_up_layer_addresses_info_to_rem_item_s_names[] = {
    {"iP-SecTransportLayerAddress", nullptr},
    {"gTPTransportLayerAddressesToRemove", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_transport_up_layer_addresses_info_to_rem_item_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(transport_up_layer_addresses_info_to_rem_item_s, ie_exts_present)}};
const asn1::table::seq_desc transport_up_layer_addresses_info_to_rem_item_s::desc = {
    {seq_table_transport_up_layer_addresses_info_to_rem_item_s_fields,
     3,
     UINT64_C(0x1),
     seq_table_transport_up_layer_addresses_info_to_rem_item_s_opts,
     2},
    true,
    true,
    nullptr,
    0,
    seq_table_transport_up_layer_addresses_info_to_rem_item_s_names};

// Transport-Layer-Address-Info ::= SEQUENCE
static const asn1::table::seq_field seq_table_transport_layer_address_info_s_fields[] = {
    {offsetof(transport_layer_address_info_s, transport_up_layer_addresses_info_to_add_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16,
     &asn1::table::item_list_codec<
         decltype(transport_layer_address_info_s::transport_up_layer_addresses_info_to_add_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &transport_up_layer_addresses_info_to_add_item_s::desc},
    {offsetof(transport_layer_address_info_s, transport_up_layer_addresses_info_to_rem_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     16,
     &asn1::table::item_list_codec<
         decltype(transport_layer_address_info_s::transport_up_layer_addresses_info_to_rem_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &transport_up_layer_addresses_info_to_rem_item_s::desc},
    {offsetof(transport_layer_address_info_s, ie_exts),
     (int32_t)offsetof(transport_layer_address_info_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(transport_layer_address_info_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_transport_layer_address_info_s_names[] = {
    {"transport-UP-Layer-Addresses-Info-To-Add-List", nullptr},
    {"transport-UP-Layer-Addresses-Info-To-Remove-List", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_transport_layer_address_info_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(transport_layer_address_info_s, ie_exts_present)}};
const asn1::table::seq_desc transport_layer_address_info_s::desc = {{seq_table_transport_layer_address_info_s_fields,
                                                                     3,
                                                                     UINT64_C(0x0),
                                                                     seq_table_transport_layer_address_info_s_opts,
                                                                     3},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_transport_layer_address_info_s_names};

// UE-associatedLogicalE1-ConnectionItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_associated_lc_e1_conn_item_s_fields[] = {
    {offsetof(ue_associated_lc_e1_conn_item_s, gnb_cu_cp_ue_e1ap_id),
     (int32_t)offsetof(ue_associated_lc_e1_conn_item_s, gnb_cu_cp_ue_e1ap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     INT64_C(0),
     UINT64_C(4294967295),
     nullptr},
    {offsetof(ue_associated_lc_e1_conn_item_s, gnb_cu_up_ue_e1ap_id),
     (int32_t)offsetof(ue_associated_lc_e1_conn_item_s, gnb_cu_up_ue_e1ap_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     INT64_C(0),
     UINT64_C(4294967295),
     nullptr},
    {offsetof(ue_associated_lc_e1_conn_item_s, ie_exts),
     (int32_t)offsetof(ue_associated_lc_e1_conn_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ue_associated_lc_e1_conn_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ue_associated_lc_e1_conn_item_s_names[] = {
    {"gNB-CU-CP-UE-E1AP-ID", nullptr},
    {"gNB-CU-UP-UE-E1AP-ID", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_ue_associated_lc_e1_conn_item_s_opts[] = {
    {0, (int32_t)offsetof(ue_associated_lc_e1_conn_item_s, gnb_cu_cp_ue_e1ap_id_present)},
    {1, (int32_t)offsetof(ue_associated_lc_e1_conn_item_s, gnb_cu_up_ue_e1ap_id_present)},
    {2, (int32_t)offsetof(ue_associated_lc_e1_conn_item_s, ie_exts_present)}};
const asn1::table::seq_desc ue_associated_lc_e1_conn_item_s::desc = {{seq_table_ue_associated_lc_e1_conn_item_s_fields,
                                                                      3,
                                                                      UINT64_C(0x0),
                                                                      seq_table_ue_associated_lc_e1_conn_item_s_opts,
                                                                      3},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_ue_associated_lc_e1_conn_item_s_names};

// UESliceMaximumBitRateItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_slice_max_bit_rate_item_s_fields[] = {
    {offsetof(ue_slice_max_bit_rate_item_s, snssai),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &snssai_s::desc},
    {offsetof(ue_slice_max_bit_rate_item_s, ue_slice_max_bit_rate_dl),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     INT64_C(0),
     UINT64_C(4000000000000),
     nullptr},
    {offsetof(ue_slice_max_bit_rate_item_s, ie_exts),
     (int32_t)offsetof(ue_slice_max_bit_rate_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ue_slice_max_bit_rate_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ue_slice_max_bit_rate_item_s_names[] = {
    {"sNSSAI", nullptr},
    {"uESliceMaximumBitRateDL", nullptr},
    {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt seq_table_ue_slice_max_bit_rate_item_s_opts[] = {
    {2, (int32_t)offsetof(ue_slice_max_bit_rate_item_s, ie_exts_present)}};
const asn1::table::seq_desc ue_slice_max_bit_rate_item_s::desc = {
    {seq_table_ue_slice_max_bit_rate_item_s_fields, 3, UINT64_C(0x3), seq_table_ue_slice_max_bit_rate_item_s_opts, 1},
    true,
    true,
    nullptr,
    0,
    seq_table_ue_slice_max_bit_rate_item_s_names};

// ULUPTNLAddressToUpdateItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_up_tnl_address_to_upd_item_s_fields[] = {
    {offsetof(ul_up_tnl_address_to_upd_item_s, old_tnl_adress),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ul_up_tnl_address_to_upd_item_s::old_tnl_adress),
                                asn1::table::struct_json>::value},
    {offsetof(ul_up_tnl_address_to_upd_item_s, new_tnl_adress),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ul_up_tnl_address_to_upd_item_s::new_tnl_adress),
                                asn1::table::struct_json>::value},
    {offsetof(ul_up_tnl_address_to_upd_item_s, ie_exts),
     (int32_t)offsetof(ul_up_tnl_address_to_upd_item_s, ie_exts_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ul_up_tnl_address_to_upd_item_s::ie_exts), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ul_up_tnl_address_to_upd_item_s_names[] = {{"oldTNLAdress", nullptr},
                                                                                          {"newTNLAdress", nullptr},
                                                                                          {"iE-Extensions", nullptr}};
static const asn1::table::seq_opt    seq_table_ul_up_tnl_address_to_upd_item_s_opts[]  = {
    {2, (int32_t)offsetof(ul_up_tnl_address_to_upd_item_s, ie_exts_present)}};
const asn1::table::seq_desc ul_up_tnl_address_to_upd_item_s::desc = {{seq_table_ul_up_tnl_address_to_upd_item_s_fields,
                                                                      3,
                                                                      UINT64_C(0x3),
                                                                      seq_table_ul_up_tnl_address_to_upd_item_s_opts,
                                                                      1},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_ul_up_tnl_address_to_upd_item_s_names};
