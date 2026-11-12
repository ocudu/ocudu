// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/e2ap/e2ap_ies.h"
#include "ocudu/asn1/asn1_ap_codec.h"

using namespace asn1;
using namespace asn1::e2ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// CauseRICrequest ::= ENUMERATED
static const char*           enum_names_cause_ric_request_e_values[] = {"ran-function-id-invalid",
                                                                        "action-not-supported",
                                                                        "excessive-actions",
                                                                        "duplicate-action",
                                                                        "duplicate-event-trigger",
                                                                        "function-resource-limit",
                                                                        "request-id-unknown",
                                                                        "inconsistent-action-subsequent-action-sequence",
                                                                        "control-message-invalid",
                                                                        "ric-call-process-id-invalid",
                                                                        "control-timer-expired",
                                                                        "control-failed-to-execute",
                                                                        "system-not-ready",
                                                                        "unspecified",
                                                                        "ric-subscription-end-time-expired",
                                                                        "ric-subscription-end-time-invalid",
                                                                        "duplicate-ric-request-id",
                                                                        "eventTriggerNotSupported",
                                                                        "requested-information-unavailable",
                                                                        "invalid-information-request"};
const asn1::table::enum_desc cause_ric_request_opts::desc            = {enum_names_cause_ric_request_e_values,
                                                                        20,
                                                                        "cause_ric_request_e"};

// CauseRICservice ::= ENUMERATED
static const char*           enum_names_cause_ric_service_e_values[] = {"ran-function-not-supported",
                                                                        "excessive-functions",
                                                                        "ric-resource-limit"};
const asn1::table::enum_desc cause_ric_service_opts::desc            = {enum_names_cause_ric_service_e_values,
                                                                        3,
                                                                        "cause_ric_service_e"};

// CauseE2node ::= ENUMERATED
static const char*           enum_names_cause_e2node_e_values[] = {"e2node-component-unknown"};
const asn1::table::enum_desc cause_e2node_opts::desc = {enum_names_cause_e2node_e_values, 1, "cause_e2node_e"};
uint8_t                      cause_e2node_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "cause_e2node_e");
}

// CauseTransport ::= ENUMERATED
static const char*           enum_names_cause_transport_e_values[] = {"unspecified", "transport-resource-unavailable"};
const asn1::table::enum_desc cause_transport_opts::desc = {enum_names_cause_transport_e_values, 2, "cause_transport_e"};

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
                                                                 "hardware-failure",
                                                                 "om-intervention",
                                                                 "unspecified"};
const asn1::table::enum_desc cause_misc_opts::desc            = {enum_names_cause_misc_e_values, 4, "cause_misc_e"};

// CauseServiceLayer ::= SEQUENCE
static const asn1::table::seq_field seq_table_cause_service_layer_s_fields[] = {
    {offsetof(cause_service_layer_s, service_layer_cause),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cause_service_layer_s::service_layer_cause),
                                asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_cause_service_layer_s_names[] = {{"serviceLayerCause", nullptr}};
const asn1::table::seq_desc          cause_service_layer_s::desc             = {
    {seq_table_cause_service_layer_s_fields, uint64_t{0x1}, nullptr, seq_table_cause_service_layer_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// Cause ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_cause_c_alternatives[] = {
    &asn1::table::alternative_ops<cause_ric_request_e, true>::value,
    &asn1::table::alternative_ops<cause_ric_service_e, true>::value,
    &asn1::table::alternative_ops<cause_e2node_e, true>::value,
    &asn1::table::alternative_ops<cause_transport_e, true>::value,
    &asn1::table::alternative_ops<cause_protocol_e, true>::value,
    &asn1::table::alternative_ops<cause_misc_e, true>::value,
    &asn1::table::alternative_ops<cause_service_layer_s, false>::value};
const asn1::table::choice_storage_desc cause_c::storage_desc = {choice_storage_cause_c_alternatives, 7, "cause_c"};

cause_ric_request_e& cause_c::set_ric_request()
{
  set(types::ric_request);
  return c.get<cause_ric_request_e>();
}
cause_ric_service_e& cause_c::set_ric_service()
{
  set(types::ric_service);
  return c.get<cause_ric_service_e>();
}
cause_e2node_e& cause_c::set_e2_node()
{
  set(types::e2_node);
  return c.get<cause_e2node_e>();
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
cause_service_layer_s& cause_c::set_service_layer()
{
  set(types::service_layer);
  return c.get<cause_service_layer_s>();
}
static const asn1::table::seq_field  choice_table_cause_c_alternatives[] = {{0,
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::enum_op<cause_ric_request_e>(),
                                                                             cause_ric_request_e::has_ext,
                                                                             false,
                                                                             cause_ric_request_e::nof_types,
                                                                             cause_ric_request_e::nof_exts,
                                                                             nullptr,
                                                                             &cause_ric_request_opts::desc},
                                                                            {0,
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::enum_op<cause_ric_service_e>(),
                                                                             cause_ric_service_e::has_ext,
                                                                             false,
                                                                             cause_ric_service_e::nof_types,
                                                                             cause_ric_service_e::nof_exts,
                                                                             nullptr,
                                                                             &cause_ric_service_opts::desc},
                                                                            {0,
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::enum_op<cause_e2node_e>(),
                                                                             cause_e2node_e::has_ext,
                                                                             false,
                                                                             cause_e2node_e::nof_types,
                                                                             cause_e2node_e::nof_exts,
                                                                             nullptr,
                                                                             &cause_e2node_opts::desc},
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
                                                                             asn1::table::field_op::sequence,
                                                                             false,
                                                                             false,
                                                                             0,
                                                                             0,
                                                                             nullptr,
                                                                             &cause_service_layer_s::desc}};
static const asn1::table::field_name choice_table_cause_c_names[]        = {{"ricRequest", nullptr},
                                                                            {"ricService", nullptr},
                                                                            {"e2Node", nullptr},
                                                                            {"transport", nullptr},
                                                                            {"protocol", nullptr},
                                                                            {"misc", nullptr},
                                                                            {"serviceLayer", nullptr}};
const asn1::table::choice_desc       cause_c::desc = {asn1::table::enum_int_type<cause_c::types>(),
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

static const char* enum_names_cause_c_types_values[] =
    {"ricRequest", "ricService", "e2Node", "transport", "protocol", "misc", "serviceLayer"};
const asn1::table::enum_desc cause_c::types_opts::desc = {enum_names_cause_c_types_values, 7, "cause_c::types"};
uint8_t                      cause_c::types_opts::to_number() const
{
  if (value == e2_node) {
    return 2;
  }
  invalid_enum_number(value, "cause_c::types");
  return 0;
}

// RICrequestID ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_request_id_s_fields[] = {
    {offsetof(ric_request_id_s, ric_requestor_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(ric_request_id_s, ric_instance_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr}};
static const asn1::table::field_name seq_table_ric_request_id_s_names[] = {{"ricRequestorID", nullptr},
                                                                           {"ricInstanceID", nullptr}};
const asn1::table::seq_desc          ric_request_id_s::desc             = {
    {seq_table_ric_request_id_s_fields, uint64_t{0x3}, nullptr, seq_table_ric_request_id_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// TypeOfError ::= ENUMERATED
static const char*           enum_names_type_of_error_e_values[] = {"not-understood", "missing"};
const asn1::table::enum_desc type_of_error_opts::desc = {enum_names_type_of_error_e_values, 2, "type_of_error_e"};

// CriticalityDiagnostics-IE-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_crit_diagnostics_ie_item_s_fields[] = {
    {offsetof(crit_diagnostics_ie_item_s, ie_crit),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(crit_diagnostics_ie_item_s::ie_crit)>(),
     decltype(crit_diagnostics_ie_item_s::ie_crit)::has_ext,
     false,
     decltype(crit_diagnostics_ie_item_s::ie_crit)::nof_types,
     decltype(crit_diagnostics_ie_item_s::ie_crit)::nof_exts,
     nullptr,
     &crit_opts::desc},
    {offsetof(crit_diagnostics_ie_item_s, ie_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(crit_diagnostics_ie_item_s, type_of_error),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(crit_diagnostics_ie_item_s::type_of_error)>(),
     decltype(crit_diagnostics_ie_item_s::type_of_error)::has_ext,
     false,
     decltype(crit_diagnostics_ie_item_s::type_of_error)::nof_types,
     decltype(crit_diagnostics_ie_item_s::type_of_error)::nof_exts,
     nullptr,
     &type_of_error_opts::desc}};
static const asn1::table::field_name seq_table_crit_diagnostics_ie_item_s_names[] = {{"iECriticality", nullptr},
                                                                                     {"iE-ID", nullptr},
                                                                                     {"typeOfError", nullptr}};
const asn1::table::seq_desc          crit_diagnostics_ie_item_s::desc = {{seq_table_crit_diagnostics_ie_item_s_fields,
                                                                          uint64_t{0x7},
                                                                          nullptr,
                                                                          seq_table_crit_diagnostics_ie_item_s_names,
                                                                          3,
                                                                          0},
                                                                         true,
                                                                         true,
                                                                         nullptr,
                                                                         0};

// CriticalityDiagnostics ::= SEQUENCE
static const asn1::table::seq_field seq_table_crit_diagnostics_s_fields[] = {
    {offsetof(crit_diagnostics_s, proc_code),
     (int32_t)offsetof(crit_diagnostics_s, proc_code_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
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
    {offsetof(crit_diagnostics_s, ric_requestor_id),
     (int32_t)offsetof(crit_diagnostics_s, ric_requestor_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_request_id_s::desc},
    {offsetof(crit_diagnostics_s, ies_crit_diagnostics),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     255,
     &asn1::table::item_list_codec<decltype(crit_diagnostics_s::ies_crit_diagnostics),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &crit_diagnostics_ie_item_s::desc}};
static const asn1::table::field_name seq_table_crit_diagnostics_s_names[] = {{"procedureCode", nullptr},
                                                                             {"triggeringMessage", nullptr},
                                                                             {"procedureCriticality", nullptr},
                                                                             {"ricRequestorID", nullptr},
                                                                             {"iEsCriticalityDiagnostics", nullptr}};
static const asn1::table::seq_opt    seq_table_crit_diagnostics_s_opts[]  = {
    {0, (int32_t)offsetof(crit_diagnostics_s, proc_code_present)},
    {1, (int32_t)offsetof(crit_diagnostics_s, trigger_msg_present)},
    {2, (int32_t)offsetof(crit_diagnostics_s, proc_crit_present)},
    {3, (int32_t)offsetof(crit_diagnostics_s, ric_requestor_id_present)},
    {4, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc crit_diagnostics_s::desc = {{seq_table_crit_diagnostics_s_fields,
                                                         uint64_t{0x0},
                                                         seq_table_crit_diagnostics_s_opts,
                                                         seq_table_crit_diagnostics_s_names,
                                                         5,
                                                         5},
                                                        true,
                                                        true,
                                                        nullptr,
                                                        0};

// E2nodeComponentConfiguration ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_component_cfg_s_fields[] = {
    {offsetof(e2node_component_cfg_s, e2node_component_request_part),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2node_component_cfg_s::e2node_component_request_part),
                                asn1::table::string_json>::value},
    {offsetof(e2node_component_cfg_s, e2node_component_resp_part),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2node_component_cfg_s::e2node_component_resp_part),
                                asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_e2node_component_cfg_s_names[] = {
    {"e2nodeComponentRequestPart", nullptr},
    {"e2nodeComponentResponsePart", nullptr}};
const asn1::table::seq_desc e2node_component_cfg_s::desc = {
    {seq_table_e2node_component_cfg_s_fields, uint64_t{0x3}, nullptr, seq_table_e2node_component_cfg_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// E2nodeComponentConfigurationAck ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_component_cfg_ack_s_fields[] = {
    {offsetof(e2node_component_cfg_ack_s, upd_outcome),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(e2node_component_cfg_ack_s::upd_outcome)>(),
     decltype(e2node_component_cfg_ack_s::upd_outcome)::has_ext,
     false,
     decltype(e2node_component_cfg_ack_s::upd_outcome)::nof_types,
     decltype(e2node_component_cfg_ack_s::upd_outcome)::nof_exts,
     nullptr,
     &e2node_component_cfg_ack_s::upd_outcome_opts::desc},
    {offsetof(e2node_component_cfg_ack_s, fail_cause),
     (int32_t)offsetof(e2node_component_cfg_ack_s, fail_cause_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cause_c::desc}};
static const asn1::table::field_name seq_table_e2node_component_cfg_ack_s_names[] = {{"updateOutcome", nullptr},
                                                                                     {"failureCause", nullptr}};
static const asn1::table::seq_opt    seq_table_e2node_component_cfg_ack_s_opts[]  = {
    {1, (int32_t)offsetof(e2node_component_cfg_ack_s, fail_cause_present)}};
const asn1::table::seq_desc e2node_component_cfg_ack_s::desc = {{seq_table_e2node_component_cfg_ack_s_fields,
                                                                 uint64_t{0x1},
                                                                 seq_table_e2node_component_cfg_ack_s_opts,
                                                                 seq_table_e2node_component_cfg_ack_s_names,
                                                                 2,
                                                                 1},
                                                                true,
                                                                true,
                                                                nullptr,
                                                                0};

static const char*           enum_names_e2node_component_cfg_ack_s_upd_outcome_e__values[] = {"success", "failure"};
const asn1::table::enum_desc e2node_component_cfg_ack_s::upd_outcome_opts::desc            = {
    enum_names_e2node_component_cfg_ack_s_upd_outcome_e__values,
    2,
    "e2node_component_cfg_ack_s::upd_outcome_e_"};

// E2nodeComponentInterfaceNG ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_component_interface_ng_s_fields[] = {
    {offsetof(e2node_component_interface_ng_s, amf_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2node_component_interface_ng_s::amf_name), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_e2node_component_interface_ng_s_names[] = {{"amf-name", nullptr}};
const asn1::table::seq_desc e2node_component_interface_ng_s::desc = {{seq_table_e2node_component_interface_ng_s_fields,
                                                                      uint64_t{0x1},
                                                                      nullptr,
                                                                      seq_table_e2node_component_interface_ng_s_names,
                                                                      1,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// GNB-ID-Choice ::= CHOICE
static const asn1::table::seq_field choice_table_gnb_id_choice_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<bounded_bitstring<22, 32, false, true>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_gnb_id_choice_c_names[] = {{"gnb-ID", nullptr}};
const asn1::table::choice_desc       gnb_id_choice_c::desc = {asn1::table::enum_int_type<gnb_id_choice_c::types>(),
                                                              gnb_id_choice_c::types::nof_types,
                                                              gnb_id_choice_c::types::nof_exts,
                                                              gnb_id_choice_c::types::has_ext,
                                                              true,
                                                              choice_table_gnb_id_choice_c_alternatives,
                                                              nullptr,
                                                              choice_table_gnb_id_choice_c_names,
                                                              "gnb_id_choice_c",
                                                              asn1::table::choice_desc::no_member,
                                                              offsetof(gnb_id_choice_c, c),
                                                              false};

static const char*           enum_names_gnb_id_choice_c_types_values[] = {"gnb-ID"};
const asn1::table::enum_desc gnb_id_choice_c::types_opts::desc         = {enum_names_gnb_id_choice_c_types_values,
                                                                          1,
                                                                          "gnb_id_choice_c::types"};

// GlobalgNB-ID ::= SEQUENCE
static const asn1::table::seq_field seq_table_global_gnb_id_s_fields[] = {
    {offsetof(global_gnb_id_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(global_gnb_id_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(global_gnb_id_s, gnb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &gnb_id_choice_c::desc}};
static const asn1::table::field_name seq_table_global_gnb_id_s_names[] = {{"plmn-id", nullptr}, {"gnb-id", nullptr}};
const asn1::table::seq_desc          global_gnb_id_s::desc             = {
    {seq_table_global_gnb_id_s_fields, uint64_t{0x3}, nullptr, seq_table_global_gnb_id_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// ENB-ID-Choice ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_enb_id_choice_c_alternatives[] = {
    &asn1::table::alternative_ops<fixed_bitstring<20, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<18, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<21, false, true>, false>::value};
const asn1::table::choice_storage_desc enb_id_choice_c::storage_desc = {choice_storage_enb_id_choice_c_alternatives,
                                                                        3,
                                                                        "enb_id_choice_c"};

fixed_bitstring<20, false, true>& enb_id_choice_c::set_enb_id_macro()
{
  set(types::enb_id_macro);
  return c.get<fixed_bitstring<20, false, true>>();
}
fixed_bitstring<18, false, true>& enb_id_choice_c::set_enb_id_shortmacro()
{
  set(types::enb_id_shortmacro);
  return c.get<fixed_bitstring<18, false, true>>();
}
fixed_bitstring<21, false, true>& enb_id_choice_c::set_enb_id_longmacro()
{
  set(types::enb_id_longmacro);
  return c.get<fixed_bitstring<21, false, true>>();
}
static const asn1::table::seq_field choice_table_enb_id_choice_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<20, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<18, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<21, false, true>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_enb_id_choice_c_names[] = {{"enb-ID-macro", nullptr},
                                                                             {"enb-ID-shortmacro", nullptr},
                                                                             {"enb-ID-longmacro", nullptr}};
const asn1::table::choice_desc       enb_id_choice_c::desc = {asn1::table::enum_int_type<enb_id_choice_c::types>(),
                                                              enb_id_choice_c::types::nof_types,
                                                              enb_id_choice_c::types::nof_exts,
                                                              enb_id_choice_c::types::has_ext,
                                                              true,
                                                              choice_table_enb_id_choice_c_alternatives,
                                                              nullptr,
                                                              choice_table_enb_id_choice_c_names,
                                                              "enb_id_choice_c",
                                                              offsetof(enb_id_choice_c, c.type),
                                                              offsetof(enb_id_choice_c, c.value),
                                                              false,
                                                              &enb_id_choice_c::storage_desc};

static const char*           enum_names_enb_id_choice_c_types_values[] = {"enb-ID-macro",
                                                                          "enb-ID-shortmacro",
                                                                          "enb-ID-longmacro"};
const asn1::table::enum_desc enb_id_choice_c::types_opts::desc         = {enum_names_enb_id_choice_c_types_values,
                                                                          3,
                                                                          "enb_id_choice_c::types"};

// GlobalngeNB-ID ::= SEQUENCE
static const asn1::table::seq_field seq_table_globalng_enb_id_s_fields[] = {
    {offsetof(globalng_enb_id_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(globalng_enb_id_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(globalng_enb_id_s, enb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &enb_id_choice_c::desc}};
static const asn1::table::field_name seq_table_globalng_enb_id_s_names[] = {{"plmn-id", nullptr}, {"enb-id", nullptr}};
const asn1::table::seq_desc          globalng_enb_id_s::desc             = {
    {seq_table_globalng_enb_id_s_fields, uint64_t{0x3}, nullptr, seq_table_globalng_enb_id_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// GlobalNG-RANNode-ID ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_global_ng_ran_node_id_c_alternatives[] = {
    &asn1::table::alternative_ops<global_gnb_id_s, false>::value,
    &asn1::table::alternative_ops<globalng_enb_id_s, false>::value};
const asn1::table::choice_storage_desc global_ng_ran_node_id_c::storage_desc = {
    choice_storage_global_ng_ran_node_id_c_alternatives,
    2,
    "global_ng_ran_node_id_c"};

global_gnb_id_s& global_ng_ran_node_id_c::set_gnb()
{
  set(types::gnb);
  return c.get<global_gnb_id_s>();
}
globalng_enb_id_s& global_ng_ran_node_id_c::set_ng_enb()
{
  set(types::ng_enb);
  return c.get<globalng_enb_id_s>();
}
static const asn1::table::seq_field choice_table_global_ng_ran_node_id_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_gnb_id_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &globalng_enb_id_s::desc}};
static const asn1::table::field_name choice_table_global_ng_ran_node_id_c_names[] = {{"gNB", nullptr},
                                                                                     {"ng-eNB", nullptr}};
const asn1::table::choice_desc       global_ng_ran_node_id_c::desc                = {
    asn1::table::enum_int_type<global_ng_ran_node_id_c::types>(),
    global_ng_ran_node_id_c::types::nof_types,
    global_ng_ran_node_id_c::types::nof_exts,
    global_ng_ran_node_id_c::types::has_ext,
    true,
    choice_table_global_ng_ran_node_id_c_alternatives,
    nullptr,
    choice_table_global_ng_ran_node_id_c_names,
    "global_ng_ran_node_id_c",
    offsetof(global_ng_ran_node_id_c, c.type),
    offsetof(global_ng_ran_node_id_c, c.value),
    false,
    &global_ng_ran_node_id_c::storage_desc};

static const char*           enum_names_global_ng_ran_node_id_c_types_values[] = {"gNB", "ng-eNB"};
const asn1::table::enum_desc global_ng_ran_node_id_c::types_opts::desc         = {
    enum_names_global_ng_ran_node_id_c_types_values,
    2,
    "global_ng_ran_node_id_c::types"};

// E2nodeComponentInterfaceXn ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_component_interface_xn_s_fields[] = {
    {offsetof(e2node_component_interface_xn_s, global_ng_ran_node_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &global_ng_ran_node_id_c::desc}};
static const asn1::table::field_name seq_table_e2node_component_interface_xn_s_names[] = {
    {"global-NG-RAN-Node-ID", nullptr}};
const asn1::table::seq_desc e2node_component_interface_xn_s::desc = {{seq_table_e2node_component_interface_xn_s_fields,
                                                                      uint64_t{0x1},
                                                                      nullptr,
                                                                      seq_table_e2node_component_interface_xn_s_names,
                                                                      1,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// E2nodeComponentInterfaceE1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_component_interface_e1_s_fields[] = {
    {offsetof(e2node_component_interface_e1_s, gnb_cu_up_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr}};
static const asn1::table::field_name seq_table_e2node_component_interface_e1_s_names[] = {{"gNB-CU-UP-ID", nullptr}};
const asn1::table::seq_desc e2node_component_interface_e1_s::desc = {{seq_table_e2node_component_interface_e1_s_fields,
                                                                      uint64_t{0x1},
                                                                      nullptr,
                                                                      seq_table_e2node_component_interface_e1_s_names,
                                                                      1,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// E2nodeComponentInterfaceF1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_component_interface_f1_s_fields[] = {
    {offsetof(e2node_component_interface_f1_s, gnb_du_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr}};
static const asn1::table::field_name seq_table_e2node_component_interface_f1_s_names[] = {{"gNB-DU-ID", nullptr}};
const asn1::table::seq_desc e2node_component_interface_f1_s::desc = {{seq_table_e2node_component_interface_f1_s_fields,
                                                                      uint64_t{0x1},
                                                                      nullptr,
                                                                      seq_table_e2node_component_interface_f1_s_names,
                                                                      1,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// E2nodeComponentInterfaceW1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_component_interface_w1_s_fields[] = {
    {offsetof(e2node_component_interface_w1_s, ng_enb_du_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr}};
static const asn1::table::field_name seq_table_e2node_component_interface_w1_s_names[] = {{"ng-eNB-DU-ID", nullptr}};
const asn1::table::seq_desc e2node_component_interface_w1_s::desc = {{seq_table_e2node_component_interface_w1_s_fields,
                                                                      uint64_t{0x1},
                                                                      nullptr,
                                                                      seq_table_e2node_component_interface_w1_s_names,
                                                                      1,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// E2nodeComponentInterfaceS1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_component_interface_s1_s_fields[] = {
    {offsetof(e2node_component_interface_s1_s, mme_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2node_component_interface_s1_s::mme_name), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_e2node_component_interface_s1_s_names[] = {{"mme-name", nullptr}};
const asn1::table::seq_desc e2node_component_interface_s1_s::desc = {{seq_table_e2node_component_interface_s1_s_fields,
                                                                      uint64_t{0x1},
                                                                      nullptr,
                                                                      seq_table_e2node_component_interface_s1_s_names,
                                                                      1,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// ENB-ID ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_enb_id_c_alternatives[] = {
    &asn1::table::alternative_ops<fixed_bitstring<20, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<28, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<18, false, true>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<21, false, true>, false>::value};
const asn1::table::choice_storage_desc enb_id_c::storage_desc = {choice_storage_enb_id_c_alternatives, 4, "enb_id_c"};

fixed_bitstring<20, false, true>& enb_id_c::set_macro_enb_id()
{
  set(types::macro_enb_id);
  return c.get<fixed_bitstring<20, false, true>>();
}
fixed_bitstring<28, false, true>& enb_id_c::set_home_enb_id()
{
  set(types::home_enb_id);
  return c.get<fixed_bitstring<28, false, true>>();
}
fixed_bitstring<18, false, true>& enb_id_c::set_short_macro_enb_id()
{
  set(types::short_macro_enb_id);
  return c.get<fixed_bitstring<18, false, true>>();
}
fixed_bitstring<21, false, true>& enb_id_c::set_long_macro_enb_id()
{
  set(types::long_macro_enb_id);
  return c.get<fixed_bitstring<21, false, true>>();
}
static const asn1::table::seq_field choice_table_enb_id_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<20, false, true>, asn1::table::struct_json>::value},
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
     &asn1::table::object_codec<fixed_bitstring<18, false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<21, false, true>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_enb_id_c_names[] = {{"macro-eNB-ID", nullptr},
                                                                      {"home-eNB-ID", nullptr},
                                                                      {"short-Macro-eNB-ID", nullptr},
                                                                      {"long-Macro-eNB-ID", nullptr}};
const asn1::table::choice_desc       enb_id_c::desc                = {asn1::table::enum_int_type<enb_id_c::types>(),
                                                                      enb_id_c::types::nof_types,
                                                                      enb_id_c::types::nof_exts,
                                                                      enb_id_c::types::has_ext,
                                                                      true,
                                                                      choice_table_enb_id_c_alternatives,
                                                                      nullptr,
                                                                      choice_table_enb_id_c_names,
                                                                      "enb_id_c",
                                                                      offsetof(enb_id_c, c.type),
                                                                      offsetof(enb_id_c, c.value),
                                                                      false,
                                                                      &enb_id_c::storage_desc};

static const char*           enum_names_enb_id_c_types_values[] = {"macro-eNB-ID",
                                                                   "home-eNB-ID",
                                                                   "short-Macro-eNB-ID",
                                                                   "long-Macro-eNB-ID"};
const asn1::table::enum_desc enb_id_c::types_opts::desc = {enum_names_enb_id_c_types_values, 4, "enb_id_c::types"};

// GlobalENB-ID ::= SEQUENCE
static const asn1::table::seq_field seq_table_global_enb_id_s_fields[] = {
    {offsetof(global_enb_id_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(global_enb_id_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(global_enb_id_s, enb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &enb_id_c::desc}};
static const asn1::table::field_name seq_table_global_enb_id_s_names[] = {{"pLMN-Identity", nullptr},
                                                                          {"eNB-ID", nullptr}};
const asn1::table::seq_desc          global_enb_id_s::desc             = {
    {seq_table_global_enb_id_s_fields, uint64_t{0x3}, nullptr, seq_table_global_enb_id_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// ENGNB-ID ::= CHOICE
static const asn1::table::seq_field choice_table_en_gnb_id_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<bounded_bitstring<22, 32, false, true>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_en_gnb_id_c_names[] = {{"gNB-ID", nullptr}};
const asn1::table::choice_desc       en_gnb_id_c::desc = {asn1::table::enum_int_type<en_gnb_id_c::types>(),
                                                          en_gnb_id_c::types::nof_types,
                                                          en_gnb_id_c::types::nof_exts,
                                                          en_gnb_id_c::types::has_ext,
                                                          true,
                                                          choice_table_en_gnb_id_c_alternatives,
                                                          nullptr,
                                                          choice_table_en_gnb_id_c_names,
                                                          "en_gnb_id_c",
                                                          asn1::table::choice_desc::no_member,
                                                          offsetof(en_gnb_id_c, c),
                                                          false};

static const char*           enum_names_en_gnb_id_c_types_values[] = {"gNB-ID"};
const asn1::table::enum_desc en_gnb_id_c::types_opts::desc         = {enum_names_en_gnb_id_c_types_values,
                                                                      1,
                                                                      "en_gnb_id_c::types"};

// GlobalenGNB-ID ::= SEQUENCE
static const asn1::table::seq_field seq_table_globalen_gnb_id_s_fields[] = {
    {offsetof(globalen_gnb_id_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(globalen_gnb_id_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(globalen_gnb_id_s, gnb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &en_gnb_id_c::desc}};
static const asn1::table::field_name seq_table_globalen_gnb_id_s_names[] = {{"pLMN-Identity", nullptr},
                                                                            {"gNB-ID", nullptr}};
const asn1::table::seq_desc          globalen_gnb_id_s::desc             = {
    {seq_table_globalen_gnb_id_s_fields, uint64_t{0x3}, nullptr, seq_table_globalen_gnb_id_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// E2nodeComponentInterfaceX2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2node_component_interface_x2_s_fields[] = {
    {offsetof(e2node_component_interface_x2_s, global_enb_id),
     (int32_t)offsetof(e2node_component_interface_x2_s, global_enb_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_enb_id_s::desc},
    {offsetof(e2node_component_interface_x2_s, global_en_gnb_id),
     (int32_t)offsetof(e2node_component_interface_x2_s, global_en_gnb_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &globalen_gnb_id_s::desc}};
static const asn1::table::field_name seq_table_e2node_component_interface_x2_s_names[] = {
    {"global-eNB-ID", nullptr},
    {"global-en-gNB-ID", nullptr}};
static const asn1::table::seq_opt seq_table_e2node_component_interface_x2_s_opts[] = {
    {0, (int32_t)offsetof(e2node_component_interface_x2_s, global_enb_id_present)},
    {1, (int32_t)offsetof(e2node_component_interface_x2_s, global_en_gnb_id_present)}};
const asn1::table::seq_desc e2node_component_interface_x2_s::desc = {{seq_table_e2node_component_interface_x2_s_fields,
                                                                      uint64_t{0x0},
                                                                      seq_table_e2node_component_interface_x2_s_opts,
                                                                      seq_table_e2node_component_interface_x2_s_names,
                                                                      2,
                                                                      2},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// E2nodeComponentID ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_e2node_component_id_c_alternatives[] = {
    &asn1::table::alternative_ops<e2node_component_interface_ng_s, false, false>::value,
    &asn1::table::alternative_ops<e2node_component_interface_xn_s, false>::value,
    &asn1::table::alternative_ops<e2node_component_interface_e1_s, false>::value,
    &asn1::table::alternative_ops<e2node_component_interface_f1_s, false>::value,
    &asn1::table::alternative_ops<e2node_component_interface_w1_s, false>::value,
    &asn1::table::alternative_ops<e2node_component_interface_s1_s, false, false>::value,
    &asn1::table::alternative_ops<e2node_component_interface_x2_s, false>::value};
const asn1::table::choice_storage_desc e2node_component_id_c::storage_desc = {
    choice_storage_e2node_component_id_c_alternatives,
    7,
    "e2node_component_id_c"};

e2node_component_interface_ng_s& e2node_component_id_c::set_e2node_component_interface_type_ng()
{
  set(types::e2node_component_interface_type_ng);
  return c.get<e2node_component_interface_ng_s>();
}
e2node_component_interface_xn_s& e2node_component_id_c::set_e2node_component_interface_type_xn()
{
  set(types::e2node_component_interface_type_xn);
  return c.get<e2node_component_interface_xn_s>();
}
e2node_component_interface_e1_s& e2node_component_id_c::set_e2node_component_interface_type_e1()
{
  set(types::e2node_component_interface_type_e1);
  return c.get<e2node_component_interface_e1_s>();
}
e2node_component_interface_f1_s& e2node_component_id_c::set_e2node_component_interface_type_f1()
{
  set(types::e2node_component_interface_type_f1);
  return c.get<e2node_component_interface_f1_s>();
}
e2node_component_interface_w1_s& e2node_component_id_c::set_e2node_component_interface_type_w1()
{
  set(types::e2node_component_interface_type_w1);
  return c.get<e2node_component_interface_w1_s>();
}
e2node_component_interface_s1_s& e2node_component_id_c::set_e2node_component_interface_type_s1()
{
  set(types::e2node_component_interface_type_s1);
  return c.get<e2node_component_interface_s1_s>();
}
e2node_component_interface_x2_s& e2node_component_id_c::set_e2node_component_interface_type_x2()
{
  set(types::e2node_component_interface_type_x2);
  return c.get<e2node_component_interface_x2_s>();
}
static const asn1::table::seq_field choice_table_e2node_component_id_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_interface_ng_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_interface_xn_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_interface_e1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_interface_f1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_interface_w1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_interface_s1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2node_component_interface_x2_s::desc}};
static const asn1::table::field_name choice_table_e2node_component_id_c_names[] = {
    {"e2nodeComponentInterfaceTypeNG", nullptr},
    {"e2nodeComponentInterfaceTypeXn", nullptr},
    {"e2nodeComponentInterfaceTypeE1", nullptr},
    {"e2nodeComponentInterfaceTypeF1", nullptr},
    {"e2nodeComponentInterfaceTypeW1", nullptr},
    {"e2nodeComponentInterfaceTypeS1", nullptr},
    {"e2nodeComponentInterfaceTypeX2", nullptr}};
const asn1::table::choice_desc e2node_component_id_c::desc = {
    asn1::table::enum_int_type<e2node_component_id_c::types>(),
    e2node_component_id_c::types::nof_types,
    e2node_component_id_c::types::nof_exts,
    e2node_component_id_c::types::has_ext,
    true,
    choice_table_e2node_component_id_c_alternatives,
    nullptr,
    choice_table_e2node_component_id_c_names,
    "e2node_component_id_c",
    offsetof(e2node_component_id_c, c.type),
    offsetof(e2node_component_id_c, c.value),
    false,
    &e2node_component_id_c::storage_desc};

static const char*           enum_names_e2node_component_id_c_types_values[] = {"e2nodeComponentInterfaceTypeNG",
                                                                                "e2nodeComponentInterfaceTypeXn",
                                                                                "e2nodeComponentInterfaceTypeE1",
                                                                                "e2nodeComponentInterfaceTypeF1",
                                                                                "e2nodeComponentInterfaceTypeW1",
                                                                                "e2nodeComponentInterfaceTypeS1",
                                                                                "e2nodeComponentInterfaceTypeX2"};
const asn1::table::enum_desc e2node_component_id_c::types_opts::desc = {enum_names_e2node_component_id_c_types_values,
                                                                        7,
                                                                        "e2node_component_id_c::types"};

// E2nodeComponentInterfaceType ::= ENUMERATED
static const char* enum_names_e2node_component_interface_type_e_values[] = {"ng", "xn", "e1", "f1", "w1", "s1", "x2"};
const asn1::table::enum_desc e2node_component_interface_type_opts::desc  = {
    enum_names_e2node_component_interface_type_e_values,
    7,
    "e2node_component_interface_type_e"};

// GlobalE2node-gNB-ID ::= SEQUENCE
static const asn1::table::seq_field seq_table_global_e2node_gnb_id_s_fields[] = {
    {offsetof(global_e2node_gnb_id_s, global_gnb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_gnb_id_s::desc},
    {offsetof(global_e2node_gnb_id_s, global_en_gnb_id),
     (int32_t)offsetof(global_e2node_gnb_id_s, global_en_gnb_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &globalen_gnb_id_s::desc},
    {offsetof(global_e2node_gnb_id_s, gnb_cu_up_id),
     (int32_t)offsetof(global_e2node_gnb_id_s, gnb_cu_up_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr},
    {offsetof(global_e2node_gnb_id_s, gnb_du_id),
     (int32_t)offsetof(global_e2node_gnb_id_s, gnb_du_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr}};
static const asn1::table::field_name seq_table_global_e2node_gnb_id_s_names[] = {{"global-gNB-ID", nullptr},
                                                                                 {"global-en-gNB-ID", nullptr},
                                                                                 {"gNB-CU-UP-ID", nullptr},
                                                                                 {"gNB-DU-ID", nullptr}};
static const asn1::table::seq_opt    seq_table_global_e2node_gnb_id_s_opts[]  = {
    {1, (int32_t)offsetof(global_e2node_gnb_id_s, global_en_gnb_id_present)},
    {2, (int32_t)offsetof(global_e2node_gnb_id_s, gnb_cu_up_id_present)},
    {3, (int32_t)offsetof(global_e2node_gnb_id_s, gnb_du_id_present)}};
const asn1::table::seq_desc global_e2node_gnb_id_s::desc = {{seq_table_global_e2node_gnb_id_s_fields,
                                                             uint64_t{0x1},
                                                             seq_table_global_e2node_gnb_id_s_opts,
                                                             seq_table_global_e2node_gnb_id_s_names,
                                                             4,
                                                             3},
                                                            true,
                                                            true,
                                                            nullptr,
                                                            0};

// GlobalE2node-en-gNB-ID ::= SEQUENCE
static const asn1::table::seq_field seq_table_global_e2node_en_gnb_id_s_fields[] = {
    {offsetof(global_e2node_en_gnb_id_s, global_en_gnb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &globalen_gnb_id_s::desc},
    {offsetof(global_e2node_en_gnb_id_s, en_gnb_cu_up_id),
     (int32_t)offsetof(global_e2node_en_gnb_id_s, en_gnb_cu_up_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr},
    {offsetof(global_e2node_en_gnb_id_s, en_gnb_du_id),
     (int32_t)offsetof(global_e2node_en_gnb_id_s, en_gnb_du_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr}};
static const asn1::table::field_name seq_table_global_e2node_en_gnb_id_s_names[] = {{"global-en-gNB-ID", nullptr},
                                                                                    {"en-gNB-CU-UP-ID", nullptr},
                                                                                    {"en-gNB-DU-ID", nullptr}};
static const asn1::table::seq_opt    seq_table_global_e2node_en_gnb_id_s_opts[]  = {
    {1, (int32_t)offsetof(global_e2node_en_gnb_id_s, en_gnb_cu_up_id_present)},
    {2, (int32_t)offsetof(global_e2node_en_gnb_id_s, en_gnb_du_id_present)}};
const asn1::table::seq_desc global_e2node_en_gnb_id_s::desc = {{seq_table_global_e2node_en_gnb_id_s_fields,
                                                                uint64_t{0x1},
                                                                seq_table_global_e2node_en_gnb_id_s_opts,
                                                                seq_table_global_e2node_en_gnb_id_s_names,
                                                                3,
                                                                2},
                                                               true,
                                                               true,
                                                               nullptr,
                                                               0};

// GlobalE2node-ng-eNB-ID ::= SEQUENCE
static const asn1::table::seq_field seq_table_global_e2node_ng_enb_id_s_fields[] = {
    {offsetof(global_e2node_ng_enb_id_s, global_ng_enb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &globalng_enb_id_s::desc},
    {offsetof(global_e2node_ng_enb_id_s, global_enb_id),
     (int32_t)offsetof(global_e2node_ng_enb_id_s, global_enb_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_enb_id_s::desc},
    {offsetof(global_e2node_ng_enb_id_s, ng_enb_du_id),
     (int32_t)offsetof(global_e2node_ng_enb_id_s, ng_enb_du_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr}};
static const asn1::table::field_name seq_table_global_e2node_ng_enb_id_s_names[] = {{"global-ng-eNB-ID", nullptr},
                                                                                    {"global-eNB-ID", nullptr},
                                                                                    {"ngENB-DU-ID", nullptr}};
static const asn1::table::seq_opt    seq_table_global_e2node_ng_enb_id_s_opts[]  = {
    {1, (int32_t)offsetof(global_e2node_ng_enb_id_s, global_enb_id_present)},
    {2, (int32_t)offsetof(global_e2node_ng_enb_id_s, ng_enb_du_id_present)}};
const asn1::table::seq_desc global_e2node_ng_enb_id_s::desc = {{seq_table_global_e2node_ng_enb_id_s_fields,
                                                                uint64_t{0x1},
                                                                seq_table_global_e2node_ng_enb_id_s_opts,
                                                                seq_table_global_e2node_ng_enb_id_s_names,
                                                                3,
                                                                2},
                                                               true,
                                                               true,
                                                               nullptr,
                                                               0};

// GlobalE2node-eNB-ID ::= SEQUENCE
static const asn1::table::seq_field seq_table_global_e2node_enb_id_s_fields[] = {
    {offsetof(global_e2node_enb_id_s, global_enb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_enb_id_s::desc}};
static const asn1::table::field_name seq_table_global_e2node_enb_id_s_names[] = {{"global-eNB-ID", nullptr}};
const asn1::table::seq_desc          global_e2node_enb_id_s::desc             = {
    {seq_table_global_e2node_enb_id_s_fields, uint64_t{0x1}, nullptr, seq_table_global_e2node_enb_id_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// GlobalE2node-ID ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_global_e2node_id_c_alternatives[] = {
    &asn1::table::alternative_ops<global_e2node_gnb_id_s, false>::value,
    &asn1::table::alternative_ops<global_e2node_en_gnb_id_s, false>::value,
    &asn1::table::alternative_ops<global_e2node_ng_enb_id_s, false>::value,
    &asn1::table::alternative_ops<global_e2node_enb_id_s, false>::value};
const asn1::table::choice_storage_desc global_e2node_id_c::storage_desc = {
    choice_storage_global_e2node_id_c_alternatives,
    4,
    "global_e2node_id_c"};

global_e2node_gnb_id_s& global_e2node_id_c::set_gnb()
{
  set(types::gnb);
  return c.get<global_e2node_gnb_id_s>();
}
global_e2node_en_gnb_id_s& global_e2node_id_c::set_en_gnb()
{
  set(types::en_gnb);
  return c.get<global_e2node_en_gnb_id_s>();
}
global_e2node_ng_enb_id_s& global_e2node_id_c::set_ng_enb()
{
  set(types::ng_enb);
  return c.get<global_e2node_ng_enb_id_s>();
}
global_e2node_enb_id_s& global_e2node_id_c::set_enb()
{
  set(types::enb);
  return c.get<global_e2node_enb_id_s>();
}
static const asn1::table::seq_field choice_table_global_e2node_id_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_e2node_gnb_id_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_e2node_en_gnb_id_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_e2node_ng_enb_id_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &global_e2node_enb_id_s::desc}};
static const asn1::table::field_name choice_table_global_e2node_id_c_names[] = {{"gNB", nullptr},
                                                                                {"en-gNB", nullptr},
                                                                                {"ng-eNB", nullptr},
                                                                                {"eNB", nullptr}};
const asn1::table::choice_desc global_e2node_id_c::desc = {asn1::table::enum_int_type<global_e2node_id_c::types>(),
                                                           global_e2node_id_c::types::nof_types,
                                                           global_e2node_id_c::types::nof_exts,
                                                           global_e2node_id_c::types::has_ext,
                                                           true,
                                                           choice_table_global_e2node_id_c_alternatives,
                                                           nullptr,
                                                           choice_table_global_e2node_id_c_names,
                                                           "global_e2node_id_c",
                                                           offsetof(global_e2node_id_c, c.type),
                                                           offsetof(global_e2node_id_c, c.value),
                                                           false,
                                                           &global_e2node_id_c::storage_desc};

static const char*           enum_names_global_e2node_id_c_types_values[] = {"gNB", "en-gNB", "ng-eNB", "eNB"};
const asn1::table::enum_desc global_e2node_id_c::types_opts::desc         = {enum_names_global_e2node_id_c_types_values,
                                                                             4,
                                                                             "global_e2node_id_c::types"};

// GlobalRIC-ID ::= SEQUENCE
static const asn1::table::seq_field seq_table_global_ric_id_s_fields[] = {
    {offsetof(global_ric_id_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(global_ric_id_s::plmn_id), asn1::table::string_json>::value},
    {offsetof(global_ric_id_s, ric_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(global_ric_id_s::ric_id), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_global_ric_id_s_names[] = {{"pLMN-Identity", nullptr},
                                                                          {"ric-ID", nullptr}};
const asn1::table::seq_desc          global_ric_id_s::desc             = {
    {seq_table_global_ric_id_s_fields, uint64_t{0x3}, nullptr, seq_table_global_ric_id_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// ListedRecordsOnly ::= ENUMERATED
static const char*           enum_names_listed_records_only_e_values[] = {"true"};
const asn1::table::enum_desc listed_records_only_opts::desc            = {enum_names_listed_records_only_e_values,
                                                                          1,
                                                                          "listed_records_only_e"};

// RICactionType ::= ENUMERATED
static const char*           enum_names_ric_action_type_e_values[] = {"report", "insert", "policy"};
const asn1::table::enum_desc ric_action_type_opts::desc = {enum_names_ric_action_type_e_values, 3, "ric_action_type_e"};

// RICassistanceUpdate ::= ENUMERATED
static const char*           enum_names_ric_assist_upd_e_values[] = {"true"};
const asn1::table::enum_desc ric_assist_upd_opts::desc = {enum_names_ric_assist_upd_e_values, 1, "ric_assist_upd_e"};

// RICcontrolAckRequest ::= ENUMERATED
static const char*           enum_names_ric_ctrl_ack_request_e_values[] = {"noAck", "ack"};
const asn1::table::enum_desc ric_ctrl_ack_request_opts::desc            = {enum_names_ric_ctrl_ack_request_e_values,
                                                                           2,
                                                                           "ric_ctrl_ack_request_e"};

// RICindicationType ::= ENUMERATED
static const char*           enum_names_ric_ind_type_e_values[] = {"report", "insert"};
const asn1::table::enum_desc ric_ind_type_opts::desc = {enum_names_ric_ind_type_e_values, 2, "ric_ind_type_e"};

// RICloadConfirm ::= ENUMERATED
static const char*           enum_names_ric_load_confirm_e_values[] = {"true"};
const asn1::table::enum_desc ric_load_confirm_opts::desc            = {enum_names_ric_load_confirm_e_values,
                                                                       1,
                                                                       "ric_load_confirm_e"};

// RICloadInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_load_info_s_fields[] = {
    {offsetof(ric_load_info_s, load_status),
     (int32_t)offsetof(ric_load_info_s, load_status_present),
     asn1::table::enum_op<decltype(ric_load_info_s::load_status)>(),
     decltype(ric_load_info_s::load_status)::has_ext,
     false,
     decltype(ric_load_info_s::load_status)::nof_types,
     decltype(ric_load_info_s::load_status)::nof_exts,
     nullptr,
     &ric_load_info_s::load_status_opts::desc},
    {offsetof(ric_load_info_s, load_estimate),
     (int32_t)offsetof(ric_load_info_s, load_estimate_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     int64_t{0},
     uint64_t{100},
     nullptr}};
static const asn1::table::field_name seq_table_ric_load_info_s_names[] = {{"loadStatus", nullptr},
                                                                          {"loadEstimate", nullptr}};
static const asn1::table::seq_opt    seq_table_ric_load_info_s_opts[]  = {
    {0, (int32_t)offsetof(ric_load_info_s, load_status_present)},
    {1, (int32_t)offsetof(ric_load_info_s, load_estimate_present)}};
const asn1::table::seq_desc ric_load_info_s::desc = {{seq_table_ric_load_info_s_fields,
                                                      uint64_t{0x0},
                                                      seq_table_ric_load_info_s_opts,
                                                      seq_table_ric_load_info_s_names,
                                                      2,
                                                      2},
                                                     true,
                                                     true,
                                                     nullptr,
                                                     0};

static const char*           enum_names_ric_load_info_s_load_status_e__values[] = {"overload", "notoverload"};
const asn1::table::enum_desc ric_load_info_s::load_status_opts::desc            = {
    enum_names_ric_load_info_s_load_status_e__values,
    2,
    "ric_load_info_s::load_status_e_"};

// RICloadRequest ::= ENUMERATED
static const char*           enum_names_ric_load_request_e_values[] = {"true"};
const asn1::table::enum_desc ric_load_request_opts::desc            = {enum_names_ric_load_request_e_values,
                                                                       1,
                                                                       "ric_load_request_e"};

// RICserviceLoadConfirm ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_service_load_confirm_s_fields[] = {
    {offsetof(ric_service_load_confirm_s, ric_service_report_load_confirm),
     (int32_t)offsetof(ric_service_load_confirm_s, ric_service_report_load_confirm_present),
     asn1::table::enum_op<decltype(ric_service_load_confirm_s::ric_service_report_load_confirm)>(),
     decltype(ric_service_load_confirm_s::ric_service_report_load_confirm)::has_ext,
     false,
     decltype(ric_service_load_confirm_s::ric_service_report_load_confirm)::nof_types,
     decltype(ric_service_load_confirm_s::ric_service_report_load_confirm)::nof_exts,
     nullptr,
     &ric_load_confirm_opts::desc},
    {offsetof(ric_service_load_confirm_s, ric_service_insert_load_confirm),
     (int32_t)offsetof(ric_service_load_confirm_s, ric_service_insert_load_confirm_present),
     asn1::table::enum_op<decltype(ric_service_load_confirm_s::ric_service_insert_load_confirm)>(),
     decltype(ric_service_load_confirm_s::ric_service_insert_load_confirm)::has_ext,
     false,
     decltype(ric_service_load_confirm_s::ric_service_insert_load_confirm)::nof_types,
     decltype(ric_service_load_confirm_s::ric_service_insert_load_confirm)::nof_exts,
     nullptr,
     &ric_load_confirm_opts::desc},
    {offsetof(ric_service_load_confirm_s, ric_service_ctrl_load_confirm),
     (int32_t)offsetof(ric_service_load_confirm_s, ric_service_ctrl_load_confirm_present),
     asn1::table::enum_op<decltype(ric_service_load_confirm_s::ric_service_ctrl_load_confirm)>(),
     decltype(ric_service_load_confirm_s::ric_service_ctrl_load_confirm)::has_ext,
     false,
     decltype(ric_service_load_confirm_s::ric_service_ctrl_load_confirm)::nof_types,
     decltype(ric_service_load_confirm_s::ric_service_ctrl_load_confirm)::nof_exts,
     nullptr,
     &ric_load_confirm_opts::desc},
    {offsetof(ric_service_load_confirm_s, ric_service_policy_load_confirm),
     (int32_t)offsetof(ric_service_load_confirm_s, ric_service_policy_load_confirm_present),
     asn1::table::enum_op<decltype(ric_service_load_confirm_s::ric_service_policy_load_confirm)>(),
     decltype(ric_service_load_confirm_s::ric_service_policy_load_confirm)::has_ext,
     false,
     decltype(ric_service_load_confirm_s::ric_service_policy_load_confirm)::nof_types,
     decltype(ric_service_load_confirm_s::ric_service_policy_load_confirm)::nof_exts,
     nullptr,
     &ric_load_confirm_opts::desc},
    {offsetof(ric_service_load_confirm_s, ric_service_query_load_confirm),
     (int32_t)offsetof(ric_service_load_confirm_s, ric_service_query_load_confirm_present),
     asn1::table::enum_op<decltype(ric_service_load_confirm_s::ric_service_query_load_confirm)>(),
     decltype(ric_service_load_confirm_s::ric_service_query_load_confirm)::has_ext,
     false,
     decltype(ric_service_load_confirm_s::ric_service_query_load_confirm)::nof_types,
     decltype(ric_service_load_confirm_s::ric_service_query_load_confirm)::nof_exts,
     nullptr,
     &ric_load_confirm_opts::desc}};
static const asn1::table::field_name seq_table_ric_service_load_confirm_s_names[] = {
    {"ricServiceReportLoadConfirm", nullptr},
    {"ricServiceInsertLoadConfirm", nullptr},
    {"ricServiceControlLoadConfirm", nullptr},
    {"ricServicePolicyLoadConfirm", nullptr},
    {"ricServiceQueryLoadConfirm", nullptr}};
static const asn1::table::seq_opt seq_table_ric_service_load_confirm_s_opts[] = {
    {0, (int32_t)offsetof(ric_service_load_confirm_s, ric_service_report_load_confirm_present)},
    {1, (int32_t)offsetof(ric_service_load_confirm_s, ric_service_insert_load_confirm_present)},
    {2, (int32_t)offsetof(ric_service_load_confirm_s, ric_service_ctrl_load_confirm_present)},
    {3, (int32_t)offsetof(ric_service_load_confirm_s, ric_service_policy_load_confirm_present)},
    {4, (int32_t)offsetof(ric_service_load_confirm_s, ric_service_query_load_confirm_present)}};
const asn1::table::seq_desc ric_service_load_confirm_s::desc = {{seq_table_ric_service_load_confirm_s_fields,
                                                                 uint64_t{0x0},
                                                                 seq_table_ric_service_load_confirm_s_opts,
                                                                 seq_table_ric_service_load_confirm_s_names,
                                                                 5,
                                                                 5},
                                                                true,
                                                                true,
                                                                nullptr,
                                                                0};

// RICserviceLoadInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_service_load_info_s_fields[] = {
    {offsetof(ric_service_load_info_s, ric_service_report_load_info),
     (int32_t)offsetof(ric_service_load_info_s, ric_service_report_load_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_load_info_s::desc},
    {offsetof(ric_service_load_info_s, ric_service_insert_load_info),
     (int32_t)offsetof(ric_service_load_info_s, ric_service_insert_load_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_load_info_s::desc},
    {offsetof(ric_service_load_info_s, ric_service_ctrl_load_info),
     (int32_t)offsetof(ric_service_load_info_s, ric_service_ctrl_load_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_load_info_s::desc},
    {offsetof(ric_service_load_info_s, ric_service_policy_load_info),
     (int32_t)offsetof(ric_service_load_info_s, ric_service_policy_load_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_load_info_s::desc},
    {offsetof(ric_service_load_info_s, ric_service_query_load_info),
     (int32_t)offsetof(ric_service_load_info_s, ric_service_query_load_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_load_info_s::desc}};
static const asn1::table::field_name seq_table_ric_service_load_info_s_names[] = {
    {"ricServiceReportLoadInformation", nullptr},
    {"ricServiceInsertLoadInformation", nullptr},
    {"ricServiceControlLoadInformation", nullptr},
    {"ricServicePolicyLoadInformation", nullptr},
    {"ricServiceQueryLoadInformation", nullptr}};
static const asn1::table::seq_opt seq_table_ric_service_load_info_s_opts[] = {
    {0, (int32_t)offsetof(ric_service_load_info_s, ric_service_report_load_info_present)},
    {1, (int32_t)offsetof(ric_service_load_info_s, ric_service_insert_load_info_present)},
    {2, (int32_t)offsetof(ric_service_load_info_s, ric_service_ctrl_load_info_present)},
    {3, (int32_t)offsetof(ric_service_load_info_s, ric_service_policy_load_info_present)},
    {4, (int32_t)offsetof(ric_service_load_info_s, ric_service_query_load_info_present)}};
const asn1::table::seq_desc ric_service_load_info_s::desc = {{seq_table_ric_service_load_info_s_fields,
                                                              uint64_t{0x0},
                                                              seq_table_ric_service_load_info_s_opts,
                                                              seq_table_ric_service_load_info_s_names,
                                                              5,
                                                              5},
                                                             true,
                                                             true,
                                                             nullptr,
                                                             0};

// RICserviceLoadRequest ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_service_load_request_s_fields[] = {
    {offsetof(ric_service_load_request_s, ric_service_report_load_request),
     (int32_t)offsetof(ric_service_load_request_s, ric_service_report_load_request_present),
     asn1::table::enum_op<decltype(ric_service_load_request_s::ric_service_report_load_request)>(),
     decltype(ric_service_load_request_s::ric_service_report_load_request)::has_ext,
     false,
     decltype(ric_service_load_request_s::ric_service_report_load_request)::nof_types,
     decltype(ric_service_load_request_s::ric_service_report_load_request)::nof_exts,
     nullptr,
     &ric_load_request_opts::desc},
    {offsetof(ric_service_load_request_s, ric_service_insert_load_request),
     (int32_t)offsetof(ric_service_load_request_s, ric_service_insert_load_request_present),
     asn1::table::enum_op<decltype(ric_service_load_request_s::ric_service_insert_load_request)>(),
     decltype(ric_service_load_request_s::ric_service_insert_load_request)::has_ext,
     false,
     decltype(ric_service_load_request_s::ric_service_insert_load_request)::nof_types,
     decltype(ric_service_load_request_s::ric_service_insert_load_request)::nof_exts,
     nullptr,
     &ric_load_request_opts::desc},
    {offsetof(ric_service_load_request_s, ric_service_ctrl_load_request),
     (int32_t)offsetof(ric_service_load_request_s, ric_service_ctrl_load_request_present),
     asn1::table::enum_op<decltype(ric_service_load_request_s::ric_service_ctrl_load_request)>(),
     decltype(ric_service_load_request_s::ric_service_ctrl_load_request)::has_ext,
     false,
     decltype(ric_service_load_request_s::ric_service_ctrl_load_request)::nof_types,
     decltype(ric_service_load_request_s::ric_service_ctrl_load_request)::nof_exts,
     nullptr,
     &ric_load_request_opts::desc},
    {offsetof(ric_service_load_request_s, ric_service_policy_load_request),
     (int32_t)offsetof(ric_service_load_request_s, ric_service_policy_load_request_present),
     asn1::table::enum_op<decltype(ric_service_load_request_s::ric_service_policy_load_request)>(),
     decltype(ric_service_load_request_s::ric_service_policy_load_request)::has_ext,
     false,
     decltype(ric_service_load_request_s::ric_service_policy_load_request)::nof_types,
     decltype(ric_service_load_request_s::ric_service_policy_load_request)::nof_exts,
     nullptr,
     &ric_load_request_opts::desc},
    {offsetof(ric_service_load_request_s, ric_service_query_load_request),
     (int32_t)offsetof(ric_service_load_request_s, ric_service_query_load_request_present),
     asn1::table::enum_op<decltype(ric_service_load_request_s::ric_service_query_load_request)>(),
     decltype(ric_service_load_request_s::ric_service_query_load_request)::has_ext,
     false,
     decltype(ric_service_load_request_s::ric_service_query_load_request)::nof_types,
     decltype(ric_service_load_request_s::ric_service_query_load_request)::nof_exts,
     nullptr,
     &ric_load_request_opts::desc}};
static const asn1::table::field_name seq_table_ric_service_load_request_s_names[] = {
    {"ricServiceReportLoadRequest", nullptr},
    {"ricServiceInsertLoadRequest", nullptr},
    {"ricServiceControlLoadRequest", nullptr},
    {"ricServicePolicyLoadRequest", nullptr},
    {"ricServiceQueryLoadRequest", nullptr}};
static const asn1::table::seq_opt seq_table_ric_service_load_request_s_opts[] = {
    {0, (int32_t)offsetof(ric_service_load_request_s, ric_service_report_load_request_present)},
    {1, (int32_t)offsetof(ric_service_load_request_s, ric_service_insert_load_request_present)},
    {2, (int32_t)offsetof(ric_service_load_request_s, ric_service_ctrl_load_request_present)},
    {3, (int32_t)offsetof(ric_service_load_request_s, ric_service_policy_load_request_present)},
    {4, (int32_t)offsetof(ric_service_load_request_s, ric_service_query_load_request_present)}};
const asn1::table::seq_desc ric_service_load_request_s::desc = {{seq_table_ric_service_load_request_s_fields,
                                                                 uint64_t{0x0},
                                                                 seq_table_ric_service_load_request_s_opts,
                                                                 seq_table_ric_service_load_request_s_names,
                                                                 5,
                                                                 5},
                                                                true,
                                                                true,
                                                                nullptr,
                                                                0};

// RICsubscriptionAuditFlag ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_sub_audit_flag_s_fields[] = {
    {offsetof(ric_sub_audit_flag_s, listed_records_only),
     (int32_t)offsetof(ric_sub_audit_flag_s, listed_records_only_present),
     asn1::table::enum_op<decltype(ric_sub_audit_flag_s::listed_records_only)>(),
     decltype(ric_sub_audit_flag_s::listed_records_only)::has_ext,
     false,
     decltype(ric_sub_audit_flag_s::listed_records_only)::nof_types,
     decltype(ric_sub_audit_flag_s::listed_records_only)::nof_exts,
     nullptr,
     &listed_records_only_opts::desc}};
static const asn1::table::field_name seq_table_ric_sub_audit_flag_s_names[] = {{"listedRecordsOnly", nullptr}};
static const asn1::table::seq_opt    seq_table_ric_sub_audit_flag_s_opts[]  = {
    {0, (int32_t)offsetof(ric_sub_audit_flag_s, listed_records_only_present)}};
const asn1::table::seq_desc ric_sub_audit_flag_s::desc = {{seq_table_ric_sub_audit_flag_s_fields,
                                                           uint64_t{0x0},
                                                           seq_table_ric_sub_audit_flag_s_opts,
                                                           seq_table_ric_sub_audit_flag_s_names,
                                                           1,
                                                           1},
                                                          true,
                                                          true,
                                                          nullptr,
                                                          0};

// RICsubsequentActionType ::= ENUMERATED
static const char*           enum_names_ric_subsequent_action_type_e_values[] = {"continue", "wait"};
const asn1::table::enum_desc ric_subsequent_action_type_opts::desc = {enum_names_ric_subsequent_action_type_e_values,
                                                                      2,
                                                                      "ric_subsequent_action_type_e"};

// RICtimeToWait ::= ENUMERATED
static const char*           enum_names_ric_time_to_wait_e_values[] = {"w1ms",
                                                                       "w2ms",
                                                                       "w5ms",
                                                                       "w10ms",
                                                                       "w20ms",
                                                                       "w30ms",
                                                                       "w40ms",
                                                                       "w50ms",
                                                                       "w100ms",
                                                                       "w200ms",
                                                                       "w500ms",
                                                                       "w1s",
                                                                       "w2s",
                                                                       "w5s",
                                                                       "w10s",
                                                                       "w20s",
                                                                       "w60s"};
const asn1::table::enum_desc ric_time_to_wait_opts::desc            = {enum_names_ric_time_to_wait_e_values,
                                                                       17,
                                                                       "ric_time_to_wait_e"};

// RICsubsequentAction ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_subsequent_action_s_fields[] = {
    {offsetof(ric_subsequent_action_s, ric_subsequent_action_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ric_subsequent_action_s::ric_subsequent_action_type)>(),
     decltype(ric_subsequent_action_s::ric_subsequent_action_type)::has_ext,
     false,
     decltype(ric_subsequent_action_s::ric_subsequent_action_type)::nof_types,
     decltype(ric_subsequent_action_s::ric_subsequent_action_type)::nof_exts,
     nullptr,
     &ric_subsequent_action_type_opts::desc},
    {offsetof(ric_subsequent_action_s, ric_time_to_wait),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ric_subsequent_action_s::ric_time_to_wait)>(),
     decltype(ric_subsequent_action_s::ric_time_to_wait)::has_ext,
     false,
     decltype(ric_subsequent_action_s::ric_time_to_wait)::nof_types,
     decltype(ric_subsequent_action_s::ric_time_to_wait)::nof_exts,
     nullptr,
     &ric_time_to_wait_opts::desc}};
static const asn1::table::field_name seq_table_ric_subsequent_action_s_names[] = {{"ricSubsequentActionType", nullptr},
                                                                                  {"ricTimeToWait", nullptr}};
const asn1::table::seq_desc          ric_subsequent_action_s::desc             = {
    {seq_table_ric_subsequent_action_s_fields, uint64_t{0x3}, nullptr, seq_table_ric_subsequent_action_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

// RegistrationRequest ::= ENUMERATED
static const char*           enum_names_regist_request_e_values[] = {"start", "stop", "add"};
const asn1::table::enum_desc regist_request_opts::desc = {enum_names_regist_request_e_values, 3, "regist_request_e"};

// RegistrationRequest-E2 ::= ENUMERATED
static const char*           enum_names_regist_request_e2_e_values[] = {"start", "stop", "add"};
const asn1::table::enum_desc regist_request_e2_opts::desc            = {enum_names_regist_request_e2_e_values,
                                                                        3,
                                                                        "regist_request_e2_e"};

// ReportingPeriodicity ::= ENUMERATED
static const char* enum_names_report_periodicity_e_values[] = {"ms500", "ms1000", "ms2000", "ms5000", "ms10000"};
const asn1::table::enum_desc report_periodicity_opts::desc  = {enum_names_report_periodicity_e_values,
                                                               5,
                                                               "report_periodicity_e"};
uint16_t                     report_periodicity_opts::to_number() const
{
  static const uint16_t numbers[] = {500, 1000, 2000, 5000, 10000};
  return map_enum_number(numbers, 5, value, "report_periodicity_e");
}

// ReportingPeriodicity-E2 ::= ENUMERATED
static const char* enum_names_report_periodicity_e2_e_values[] = {"ms500", "ms1000", "ms2000", "ms5000", "ms10000"};
const asn1::table::enum_desc report_periodicity_e2_opts::desc  = {enum_names_report_periodicity_e2_e_values,
                                                                  5,
                                                                  "report_periodicity_e2_e"};
uint16_t                     report_periodicity_e2_opts::to_number() const
{
  static const uint16_t numbers[] = {500, 1000, 2000, 5000, 10000};
  return map_enum_number(numbers, 5, value, "report_periodicity_e2_e");
}

// TNLinformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_tnl_info_s_fields[] = {
    {offsetof(tnl_info_s, tnl_address),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tnl_info_s::tnl_address), asn1::table::struct_json>::value},
    {offsetof(tnl_info_s, tnl_port),
     (int32_t)offsetof(tnl_info_s, tnl_port_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tnl_info_s::tnl_port), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_tnl_info_s_names[] = {{"tnlAddress", nullptr}, {"tnlPort", nullptr}};
static const asn1::table::seq_opt seq_table_tnl_info_s_opts[] = {{1, (int32_t)offsetof(tnl_info_s, tnl_port_present)}};
const asn1::table::seq_desc       tnl_info_s::desc            = {
    {seq_table_tnl_info_s_fields, uint64_t{0x1}, seq_table_tnl_info_s_opts, seq_table_tnl_info_s_names, 2, 1},
    true,
    true,
    nullptr,
    0};

// TNLusage ::= ENUMERATED
static const char*           enum_names_tnl_usage_e_values[] = {"ric-service", "support-function", "both"};
const asn1::table::enum_desc tnl_usage_opts::desc            = {enum_names_tnl_usage_e_values, 3, "tnl_usage_e"};

// TimeToWait ::= ENUMERATED
static const char*           enum_names_time_to_wait_e_values[] = {"v1s", "v2s", "v5s", "v10s", "v20s", "v60s"};
const asn1::table::enum_desc time_to_wait_opts::desc = {enum_names_time_to_wait_e_values, 6, "time_to_wait_e"};
uint8_t                      time_to_wait_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 5, 10, 20, 60};
  return map_enum_number(numbers, 6, value, "time_to_wait_e");
}
