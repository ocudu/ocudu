// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/asn1_openapi_codec.h"
#include "ocudu/asn1/e2sm_ccc/e2sm_ccc.h"

namespace asn1 {
namespace e2sm_ccc {

static const asn1::openapi::field_desc openapi_attribute_name_s_fields[] = {
    {&asn1::openapi::string_value<decltype(attribute_name_s::attribute_name)>::codec}};
static const asn1::openapi::seq_desc openapi_attribute_name_s = {&attribute_name_s::desc,
                                                                 openapi_attribute_name_s_fields};

static const asn1::openapi::field_desc openapi_ran_cfg_structure_for_adf_s_fields[] = {
    {},
    {&asn1::openapi::string_value<decltype(ran_cfg_structure_for_adf_s::ran_cfg_structure_name)>::codec, nullptr, true},
    {&asn1::openapi::seq_list_value<decltype(ran_cfg_structure_for_adf_s::list_of_attributes),
                                    &openapi_attribute_name_s>::codec}};
static const asn1::openapi::seq_desc openapi_ran_cfg_structure_for_adf_s = {&ran_cfg_structure_for_adf_s::desc,
                                                                            openapi_ran_cfg_structure_for_adf_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_action_definition_format1_s_fields[] = {
    {&asn1::openapi::seq_list_value<
        decltype(e2sm_ccc_action_definition_format1_s::list_of_node_level_ran_cfg_structures_for_adf),
        &openapi_ran_cfg_structure_for_adf_s>::codec}};
static const asn1::openapi::seq_desc openapi_e2sm_ccc_action_definition_format1_s = {
    &e2sm_ccc_action_definition_format1_s::desc,
    openapi_e2sm_ccc_action_definition_format1_s_fields};

static const asn1::openapi::field_desc openapi_plmn_id_s_fields[] = {
    {&asn1::openapi::string_value<decltype(plmn_id_s::mcc)>::codec},
    {&asn1::openapi::string_value<decltype(plmn_id_s::mnc)>::codec}};
static const asn1::openapi::seq_desc openapi_plmn_id_s = {&plmn_id_s::desc, openapi_plmn_id_s_fields};

static const asn1::openapi::field_desc openapi_nr_cgi_s_fields[] = {
    {nullptr, &openapi_plmn_id_s},
    {&asn1::openapi::hex_number_value<decltype(nr_cgi_s::nr_cell_id), 9>::codec}};
static const asn1::openapi::seq_desc openapi_nr_cgi_s = {&nr_cgi_s::desc, openapi_nr_cgi_s_fields};

static const asn1::openapi::field_desc openapi_eutra_cgi_s_fields[] = {
    {nullptr, &openapi_plmn_id_s},
    {&asn1::openapi::string_value<decltype(eutra_cgi_s::eutra_cell_id)>::codec}};
static const asn1::openapi::seq_desc openapi_eutra_cgi_s = {&eutra_cgi_s::desc, openapi_eutra_cgi_s_fields};

static const char* const openapi_cell_global_id_c_alt0_keys[] = {"plmnIdentity", "nRCellIdentity"};
static const char* const openapi_cell_global_id_c_alt1_keys[] = {"plmnIdentity", "eUTRACellIdentity"};
static const asn1::openapi::alternative_desc openapi_cell_global_id_c_alternatives[] = {
    {nullptr, &openapi_nr_cgi_s, openapi_cell_global_id_c_alt0_keys, 2},
    {nullptr, &openapi_eutra_cgi_s, openapi_cell_global_id_c_alt1_keys, 2}};
static const asn1::openapi::choice_desc openapi_cell_global_id_c = {&cell_global_id_c::desc,
                                                                    openapi_cell_global_id_c_alternatives};

static const asn1::openapi::field_desc openapi_cell_cfg_to_be_reported_for_adf_s_fields[] = {
    {nullptr, &openapi_cell_global_id_c},
    {&asn1::openapi::seq_list_value<
        decltype(cell_cfg_to_be_reported_for_adf_s::list_of_cell_level_ran_cfg_structures_for_adf),
        &openapi_ran_cfg_structure_for_adf_s>::codec}};
static const asn1::openapi::seq_desc openapi_cell_cfg_to_be_reported_for_adf_s = {
    &cell_cfg_to_be_reported_for_adf_s::desc,
    openapi_cell_cfg_to_be_reported_for_adf_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_action_definition_format2_s_fields[] = {
    {&asn1::openapi::seq_list_value<
        decltype(e2sm_ccc_action_definition_format2_s::list_of_cell_cfgs_to_be_reported_for_adf),
        &openapi_cell_cfg_to_be_reported_for_adf_s>::codec}};
static const asn1::openapi::seq_desc openapi_e2sm_ccc_action_definition_format2_s = {
    &e2sm_ccc_action_definition_format2_s::desc,
    openapi_e2sm_ccc_action_definition_format2_s_fields};

static const asn1::openapi::field_desc openapi_sleep_mode_s_fields[] = {{}, {}, {}};
static const asn1::openapi::seq_desc   openapi_sleep_mode_s = {&sleep_mode_s::desc, openapi_sleep_mode_s_fields};

static const asn1::openapi::field_desc openapi_asm_cap_info_s_fields[] = {
    {&asn1::openapi::seq_list_value<decltype(asm_cap_info_s::sleep_modes), &openapi_sleep_mode_s>::codec},
    {},
    {}};
static const asn1::openapi::seq_desc openapi_asm_cap_info_s = {&asm_cap_info_s::desc, openapi_asm_cap_info_s_fields};

static const asn1::openapi::field_desc openapi_event_trigger_style_s_fields[] = {
    {},
    {&asn1::openapi::string_value<decltype(event_trigger_style_s::event_trigger_style_name)>::codec},
    {}};
static const asn1::openapi::seq_desc openapi_event_trigger_style_s = {&event_trigger_style_s::desc,
                                                                      openapi_event_trigger_style_s_fields};

static const asn1::openapi::field_desc openapi_event_trigger_s_fields[] = {
    {&asn1::openapi::seq_list_value<decltype(event_trigger_s::list_of_supported_event_trigger_styles),
                                    &openapi_event_trigger_style_s>::codec}};
static const asn1::openapi::seq_desc openapi_event_trigger_s = {&event_trigger_s::desc, openapi_event_trigger_s_fields};

static const asn1::openapi::field_desc openapi_event_trigger_style_type_s_fields[] = {{}};
static const asn1::openapi::seq_desc   openapi_event_trigger_style_type_s          = {&event_trigger_style_type_s::desc,
                                                                                      openapi_event_trigger_style_type_s_fields};

static const asn1::openapi::field_desc openapi_report_style_s_fields[] = {
    {},
    {&asn1::openapi::string_value<decltype(report_style_s::report_service_style_name)>::codec},
    {&asn1::openapi::seq_list_value<decltype(report_style_s::list_of_supported_event_trigger_styles_for_report_style),
                                    &openapi_event_trigger_style_type_s>::codec},
    {},
    {},
    {}};
static const asn1::openapi::seq_desc openapi_report_style_s = {&report_style_s::desc, openapi_report_style_s_fields};

static const asn1::openapi::field_desc openapi_report_service_s_fields[] = {
    {&asn1::openapi::seq_list_value<decltype(report_service_s::list_of_supported_report_styles),
                                    &openapi_report_style_s>::codec}};
static const asn1::openapi::seq_desc openapi_report_service_s = {&report_service_s::desc,
                                                                 openapi_report_service_s_fields};

static const asn1::openapi::field_desc openapi_ctrl_style_s_fields[] =
    {{}, {&asn1::openapi::string_value<decltype(ctrl_style_s::ctrl_service_style_name)>::codec}, {}, {}, {}, {}};
static const asn1::openapi::seq_desc openapi_ctrl_style_s = {&ctrl_style_s::desc, openapi_ctrl_style_s_fields};

static const asn1::openapi::field_desc openapi_ctrl_service_s_fields[] = {
    {&asn1::openapi::seq_list_value<decltype(ctrl_service_s::list_of_supported_ctrl_styles),
                                    &openapi_ctrl_style_s>::codec}};
static const asn1::openapi::seq_desc openapi_ctrl_service_s = {&ctrl_service_s::desc, openapi_ctrl_service_s_fields};

static const asn1::openapi::field_desc openapi_query_style_s_fields[] =
    {{}, {&asn1::openapi::string_value<decltype(query_style_s::query_service_style_name)>::codec}, {}, {}, {}};
static const asn1::openapi::seq_desc openapi_query_style_s = {&query_style_s::desc, openapi_query_style_s_fields};

static const asn1::openapi::field_desc openapi_query_service_s_fields[] = {
    {&asn1::openapi::seq_list_value<decltype(query_service_s::list_of_supported_query_styles),
                                    &openapi_query_style_s>::codec}};
static const asn1::openapi::seq_desc openapi_query_service_s = {&query_service_s::desc, openapi_query_service_s_fields};

static const asn1::openapi::field_desc openapi_ric_services_s_fields[] = {{nullptr, &openapi_event_trigger_s},
                                                                          {nullptr, &openapi_report_service_s},
                                                                          {},
                                                                          {nullptr, &openapi_ctrl_service_s},
                                                                          {},
                                                                          {nullptr, &openapi_query_service_s}};
static const asn1::openapi::seq_desc   openapi_ric_services_s = {&ric_services_s::desc, openapi_ric_services_s_fields};

static const asn1::openapi::field_desc openapi_attribute_s_fields[] = {
    {&asn1::openapi::string_value<decltype(attribute_s::attribute_name)>::codec},
    {nullptr, &openapi_ric_services_s}};
static const asn1::openapi::seq_desc openapi_attribute_s = {&attribute_s::desc, openapi_attribute_s_fields};

static const asn1::openapi::field_desc openapi_o_gnb_cu_cp_function_s_fields[] = {
    {},
    {},
    {&asn1::openapi::string_value<decltype(o_gnb_cu_cp_function_s::gnb_cu_name)>::codec},
    {nullptr, &openapi_plmn_id_s},
    {&asn1::openapi::list_value<
        decltype(o_gnb_cu_cp_function_s::x2_block_list),
        &asn1::openapi::string_value<decltype(o_gnb_cu_cp_function_s::x2_block_list)::value_type>::codec>::codec},
    {&asn1::openapi::list_value<
        decltype(o_gnb_cu_cp_function_s::xn_block_list),
        &asn1::openapi::string_value<decltype(o_gnb_cu_cp_function_s::xn_block_list)::value_type>::codec>::codec},
    {&asn1::openapi::list_value<
        decltype(o_gnb_cu_cp_function_s::x2_allow_list),
        &asn1::openapi::string_value<decltype(o_gnb_cu_cp_function_s::x2_allow_list)::value_type>::codec>::codec},
    {&asn1::openapi::list_value<
        decltype(o_gnb_cu_cp_function_s::xn_allow_list),
        &asn1::openapi::string_value<decltype(o_gnb_cu_cp_function_s::xn_allow_list)::value_type>::codec>::codec},
    {&asn1::openapi::list_value<
        decltype(o_gnb_cu_cp_function_s::x2_ho_block_list),
        &asn1::openapi::string_value<decltype(o_gnb_cu_cp_function_s::x2_ho_block_list)::value_type>::codec>::codec},
    {&asn1::openapi::list_value<
        decltype(o_gnb_cu_cp_function_s::xn_ho_block_list),
        &asn1::openapi::string_value<decltype(o_gnb_cu_cp_function_s::xn_ho_block_list)::value_type>::codec>::codec}};
static const asn1::openapi::seq_desc openapi_o_gnb_cu_cp_function_s = {&o_gnb_cu_cp_function_s::desc,
                                                                       openapi_o_gnb_cu_cp_function_s_fields};

static const asn1::openapi::field_desc openapi_snssai_s_fields[] = {
    {},
    {&asn1::openapi::string_value<decltype(snssai_s::sd)>::codec}};
static const asn1::openapi::seq_desc openapi_snssai_s = {&snssai_s::desc, openapi_snssai_s_fields};

static const asn1::openapi::field_desc openapi_plmn_info_s_fields[] = {{nullptr, &openapi_plmn_id_s},
                                                                       {nullptr, &openapi_snssai_s}};
const asn1::openapi::seq_desc          plmn_info_s::openapi         = {&plmn_info_s::desc, openapi_plmn_info_s_fields};

static const asn1::openapi::field_desc openapi_o_gnb_cu_up_function_s_fields[] = {
    {},
    {},
    {},
    {&asn1::openapi::seq_list_value<decltype(o_gnb_cu_up_function_s::plmn_info_list), &plmn_info_s::openapi>::codec}};
static const asn1::openapi::seq_desc openapi_o_gnb_cu_up_function_s = {&o_gnb_cu_up_function_s::desc,
                                                                       openapi_o_gnb_cu_up_function_s_fields};

static const asn1::openapi::field_desc openapi_o_gnb_du_function_s_fields[] =
    {{}, {&asn1::openapi::string_value<decltype(o_gnb_du_function_s::gnb_du_name)>::codec}, {}, {}};
static const asn1::openapi::seq_desc openapi_o_gnb_du_function_s = {&o_gnb_du_function_s::desc,
                                                                    openapi_o_gnb_du_function_s_fields};

static const asn1::openapi::field_desc openapi_o_nr_cell_cu_s_fields[] = {
    {},
    {&asn1::openapi::seq_list_value<decltype(o_nr_cell_cu_s::plmn_info_list), &plmn_info_s::openapi>::codec}};
static const asn1::openapi::seq_desc openapi_o_nr_cell_cu_s = {&o_nr_cell_cu_s::desc, openapi_o_nr_cell_cu_s_fields};

static const asn1::openapi::field_desc openapi_o_bwp_s_fields[] = {{}, {}, {}, {}, {}, {}};
static const asn1::openapi::seq_desc   openapi_o_bwp_s          = {&o_bwp_s::desc, openapi_o_bwp_s_fields};

static const asn1::openapi::field_desc openapi_partition_flow_list_item_s__fields[] = {
    {nullptr, &openapi_snssai_s},
    {nullptr, &openapi_plmn_id_s},
    {&asn1::openapi::int_list_value<decltype(partition_flow_list_item_s_::n5qi_list),
                                    decltype(partition_flow_list_item_s_::n5qi_list)::value_type,
                                    0,
                                    255>::codec}};
static const asn1::openapi::seq_desc openapi_partition_flow_list_item_s_ = {&partition_flow_list_item_s_::desc,
                                                                            openapi_partition_flow_list_item_s__fields};

static const asn1::openapi::field_desc openapi_partition_list_item_s__fields[] = {
    {},
    {},
    {&asn1::openapi::seq_list_value<decltype(partition_list_item_s_::partition_flow_list),
                                    &openapi_partition_flow_list_item_s_>::codec}};
static const asn1::openapi::seq_desc openapi_partition_list_item_s_ = {&partition_list_item_s_::desc,
                                                                       openapi_partition_list_item_s__fields};

static const asn1::openapi::field_desc openapi_o_nr_cell_du_s_fields[] = {
    {},
    {},
    {},
    {},
    {&asn1::openapi::seq_list_value<decltype(o_nr_cell_du_s::plmn_info_list), &plmn_info_s::openapi>::codec},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {},
    {&asn1::openapi::seq_list_value<decltype(o_nr_cell_du_s::bwp_list), &openapi_o_bwp_s>::codec},
    {&asn1::openapi::seq_list_value<decltype(o_nr_cell_du_s::partition_list), &openapi_partition_list_item_s_>::codec}};
static const asn1::openapi::seq_desc openapi_o_nr_cell_du_s = {&o_nr_cell_du_s::desc, openapi_o_nr_cell_du_s_fields};

static const asn1::openapi::field_desc openapi_rrm_policy_member_s_fields[] = {{nullptr, &openapi_plmn_id_s},
                                                                               {nullptr, &openapi_snssai_s}};
static const asn1::openapi::seq_desc   openapi_rrm_policy_member_s          = {&rrm_policy_member_s::desc,
                                                                               openapi_rrm_policy_member_s_fields};

static const asn1::openapi::field_desc openapi_o_rrm_policy_ratio_s_fields[] = {
    {},
    {&asn1::openapi::seq_list_value<decltype(o_rrm_policy_ratio_s::rrm_policy_member_list),
                                    &openapi_rrm_policy_member_s>::codec},
    {},
    {},
    {}};
static const asn1::openapi::seq_desc openapi_o_rrm_policy_ratio_s = {&o_rrm_policy_ratio_s::desc,
                                                                     openapi_o_rrm_policy_ratio_s_fields};

static const asn1::openapi::field_desc openapi_o_ces_management_function_s_fields[] = {{}, {}, {}};
static const asn1::openapi::seq_desc   openapi_o_ces_management_function_s = {&o_ces_management_function_s::desc,
                                                                              openapi_o_ces_management_function_s_fields};

static const asn1::openapi::field_desc openapi_es_objive_c_target_ec_s__fields[] = {
    {&asn1::openapi::real_value::codec}};
static const asn1::openapi::seq_desc openapi_es_objive_c_target_ec_s_ = {&es_objive_c::target_ec_s_::desc,
                                                                         openapi_es_objive_c_target_ec_s__fields};

static const asn1::openapi::field_desc openapi_es_objive_c_es_percentage_s__fields[] = {{}};
static const asn1::openapi::seq_desc   openapi_es_objive_c_es_percentage_s_          = {
    &es_objive_c::es_percentage_s_::desc,
    openapi_es_objive_c_es_percentage_s__fields};

static const asn1::openapi::field_desc openapi_perf_objive_s_fields[] = {
    {&asn1::openapi::seq_list_value<decltype(perf_objive_s::plmn_info_list), &plmn_info_s::openapi>::codec},
    {},
    {&asn1::openapi::string_value<decltype(perf_objive_s::max_ngbr_flow_br)>::codec},
    {},
    {},
    {}};
static const asn1::openapi::seq_desc openapi_perf_objive_s = {&perf_objive_s::desc, openapi_perf_objive_s_fields};

static const char* const                     openapi_es_objive_c_alt0_keys[]    = {"targetEc"};
static const char* const                     openapi_es_objive_c_alt1_keys[]    = {"esPercentage"};
static const asn1::openapi::alternative_desc openapi_es_objive_c_alternatives[] = {
    {nullptr, &openapi_es_objive_c_target_ec_s_, openapi_es_objive_c_alt0_keys, 1},
    {nullptr, &openapi_es_objive_c_es_percentage_s_, openapi_es_objive_c_alt1_keys, 1}};
static const asn1::openapi::choice_desc openapi_es_objive_c = {&es_objive_c::desc, openapi_es_objive_c_alternatives};

static const asn1::openapi::field_desc openapi_o_nes_policy_s_fields[] = {
    {},
    {&asn1::openapi::string_value<decltype(o_nes_policy_s::ant_mask_name)>::codec},
    {&asn1::openapi::string_value<decltype(o_nes_policy_s::ant_mask)>::codec},
    {},
    {},
    {},
    {&asn1::openapi::string_value<decltype(o_nes_policy_s::slot_mask)>::codec},
    {},
    {nullptr, &openapi_es_objive_c},
    {&asn1::openapi::seq_list_value<decltype(o_nes_policy_s::perf_objive_list), &openapi_perf_objive_s>::codec}};
static const asn1::openapi::seq_desc openapi_o_nes_policy_s = {&o_nes_policy_s::desc, openapi_o_nes_policy_s_fields};

static const asn1::openapi::field_desc openapi_cycle_start_offset_s_fields[] = {{}, {}};
static const asn1::openapi::seq_desc   openapi_cycle_start_offset_s          = {&cycle_start_offset_s::desc,
                                                                                openapi_cycle_start_offset_s_fields};

static const asn1::openapi::alternative_desc openapi_on_dur_timer_c_alternatives[] = {{nullptr, nullptr},
                                                                                      {nullptr, nullptr}};
static const asn1::openapi::choice_desc      openapi_on_dur_timer_c                = {&on_dur_timer_c::desc,
                                                                                      openapi_on_dur_timer_c_alternatives};

static const asn1::openapi::field_desc openapi_o_cell_dtxdrx_cfg_s_fields[] =
    {{nullptr, &openapi_on_dur_timer_c}, {nullptr, &openapi_cycle_start_offset_s}, {}, {}, {}, {}};
static const asn1::openapi::seq_desc openapi_o_cell_dtxdrx_cfg_s = {&o_cell_dtxdrx_cfg_s::desc,
                                                                    openapi_o_cell_dtxdrx_cfg_s_fields};

static const asn1::openapi::field_desc openapi_t_polarization_item_s_fields[] = {{}, {}};
static const asn1::openapi::seq_desc   openapi_t_polarization_item_s          = {&t_polarization_item_s::desc,
                                                                                 openapi_t_polarization_item_s_fields};

static const asn1::openapi::field_desc openapi_tx_array_item_s_tnormal_vector_direction_s__fields[] = {
    {&asn1::openapi::real_value::codec},
    {&asn1::openapi::real_value::codec}};
static const asn1::openapi::seq_desc openapi_tx_array_item_s_tnormal_vector_direction_s_ = {
    &tx_array_item_s::tnormal_vector_direction_s_::desc,
    openapi_tx_array_item_s_tnormal_vector_direction_s__fields};

static const asn1::openapi::field_desc openapi_tx_array_item_s_tleftmost_bottom_array_elem_position_s__fields[] = {
    {&asn1::openapi::real_value::codec},
    {&asn1::openapi::real_value::codec},
    {&asn1::openapi::real_value::codec}};
static const asn1::openapi::seq_desc openapi_tx_array_item_s_tleftmost_bottom_array_elem_position_s_ = {
    &tx_array_item_s::tleftmost_bottom_array_elem_position_s_::desc,
    openapi_tx_array_item_s_tleftmost_bottom_array_elem_position_s__fields};

static const asn1::openapi::field_desc openapi_tx_array_item_s_fields[] = {
    {&asn1::openapi::string_value<decltype(tx_array_item_s::tname)>::codec},
    {},
    {},
    {},
    {&asn1::openapi::real_value::codec},
    {&asn1::openapi::real_value::codec},
    {nullptr, &openapi_tx_array_item_s_tnormal_vector_direction_s_},
    {nullptr, &openapi_tx_array_item_s_tleftmost_bottom_array_elem_position_s_},
    {&asn1::openapi::seq_list_value<decltype(tx_array_item_s::tpolarization_list),
                                    &openapi_t_polarization_item_s>::codec},
    {},
    {&asn1::openapi::real_value::codec},
    {&asn1::openapi::real_value::codec},
    {}};
static const asn1::openapi::seq_desc openapi_tx_array_item_s = {&tx_array_item_s::desc, openapi_tx_array_item_s_fields};

static const asn1::openapi::field_desc openapi_r_polarization_item_s_fields[] = {{}, {}};
static const asn1::openapi::seq_desc   openapi_r_polarization_item_s          = {&r_polarization_item_s::desc,
                                                                                 openapi_r_polarization_item_s_fields};

static const asn1::openapi::field_desc openapi_rx_array_item_s_rnormal_vector_direction_s__fields[] = {
    {&asn1::openapi::real_value::codec},
    {&asn1::openapi::real_value::codec}};
static const asn1::openapi::seq_desc openapi_rx_array_item_s_rnormal_vector_direction_s_ = {
    &rx_array_item_s::rnormal_vector_direction_s_::desc,
    openapi_rx_array_item_s_rnormal_vector_direction_s__fields};

static const asn1::openapi::field_desc openapi_rx_array_item_s_rleftmost_bottom_array_elem_position_s__fields[] = {
    {&asn1::openapi::real_value::codec},
    {&asn1::openapi::real_value::codec},
    {&asn1::openapi::real_value::codec}};
static const asn1::openapi::seq_desc openapi_rx_array_item_s_rleftmost_bottom_array_elem_position_s_ = {
    &rx_array_item_s::rleftmost_bottom_array_elem_position_s_::desc,
    openapi_rx_array_item_s_rleftmost_bottom_array_elem_position_s__fields};

static const asn1::openapi::field_desc openapi_rx_array_item_s_rgain_correction_range_s__fields[] = {
    {&asn1::openapi::real_value::codec},
    {&asn1::openapi::real_value::codec}};
static const asn1::openapi::seq_desc openapi_rx_array_item_s_rgain_correction_range_s_ = {
    &rx_array_item_s::rgain_correction_range_s_::desc,
    openapi_rx_array_item_s_rgain_correction_range_s__fields};

static const asn1::openapi::field_desc openapi_rx_array_item_s_fields[] = {
    {&asn1::openapi::string_value<decltype(rx_array_item_s::rname)>::codec},
    {},
    {},
    {},
    {&asn1::openapi::real_value::codec},
    {&asn1::openapi::real_value::codec},
    {nullptr, &openapi_rx_array_item_s_rnormal_vector_direction_s_},
    {nullptr, &openapi_rx_array_item_s_rleftmost_bottom_array_elem_position_s_},
    {&asn1::openapi::seq_list_value<decltype(rx_array_item_s::rpolarization_list),
                                    &openapi_r_polarization_item_s>::codec},
    {},
    {nullptr, &openapi_rx_array_item_s_rgain_correction_range_s_}};
static const asn1::openapi::seq_desc openapi_rx_array_item_s = {&rx_array_item_s::desc, openapi_rx_array_item_s_fields};

static const asn1::openapi::field_desc openapi_oru_user_plane_cfg_s_fields[] = {
    {&asn1::openapi::seq_list_value<decltype(oru_user_plane_cfg_s::tx_array_list), &openapi_tx_array_item_s>::codec},
    {&asn1::openapi::seq_list_value<decltype(oru_user_plane_cfg_s::rx_array_list), &openapi_rx_array_item_s>::codec}};
static const asn1::openapi::seq_desc openapi_oru_user_plane_cfg_s = {&oru_user_plane_cfg_s::desc,
                                                                     openapi_oru_user_plane_cfg_s_fields};

static const asn1::openapi::field_desc openapi_energy_saving_cap_common_info_s_fields[] = {{}, {}, {}};
static const asn1::openapi::seq_desc   openapi_energy_saving_cap_common_info_s          = {
    &energy_saving_cap_common_info_s::desc,
    openapi_energy_saving_cap_common_info_s_fields};

static const asn1::openapi::field_desc openapi_supported_trx_ctrl_mask_s_fields[] = {
    {&asn1::openapi::string_value<decltype(supported_trx_ctrl_mask_s::mask_name)>::codec},
    {}};
static const asn1::openapi::seq_desc openapi_supported_trx_ctrl_mask_s = {&supported_trx_ctrl_mask_s::desc,
                                                                          openapi_supported_trx_ctrl_mask_s_fields};

static const asn1::openapi::field_desc openapi_trx_ctrl_cap_info_s_fields[] = {
    {&asn1::openapi::seq_list_value<decltype(trx_ctrl_cap_info_s::supported_trx_ctrl_masks),
                                    &openapi_supported_trx_ctrl_mask_s>::codec},
    {&asn1::openapi::seq_list_value<decltype(trx_ctrl_cap_info_s::sleep_modes), &openapi_sleep_mode_s>::codec},
    {},
    {}};
static const asn1::openapi::seq_desc openapi_trx_ctrl_cap_info_s = {&trx_ctrl_cap_info_s::desc,
                                                                    openapi_trx_ctrl_cap_info_s_fields};

static const asn1::openapi::field_desc openapi_deep_hibernate_cap_info_s_fields[] = {{}, {}};
static const asn1::openapi::seq_desc   openapi_deep_hibernate_cap_info_s          = {&deep_hibernate_cap_info_s::desc,
                                                                                     openapi_deep_hibernate_cap_info_s_fields};

static const asn1::openapi::field_desc openapi_oru_cap_s_fields[] = {
    {nullptr, &openapi_energy_saving_cap_common_info_s},
    {nullptr, &openapi_asm_cap_info_s},
    {nullptr, &openapi_trx_ctrl_cap_info_s},
    {nullptr, &openapi_deep_hibernate_cap_info_s}};
static const asn1::openapi::seq_desc openapi_oru_cap_s = {&oru_cap_s::desc, openapi_oru_cap_s_fields};

static const asn1::openapi::field_desc openapi_o_ru_info_s_fields[] = {{nullptr, &openapi_oru_user_plane_cfg_s},
                                                                       {nullptr, &openapi_oru_cap_s}};
static const asn1::openapi::seq_desc   openapi_o_ru_info_s          = {&o_ru_info_s::desc, openapi_o_ru_info_s_fields};

static const asn1::openapi::field_desc openapi_prb_blanking_cfg_item_s_fields[] = {
    {},
    {&asn1::openapi::string_value<decltype(prb_blanking_cfg_item_s::frame_num)>::codec},
    {&asn1::openapi::string_value<decltype(prb_blanking_cfg_item_s::slot_num)>::codec},
    {&asn1::openapi::string_value<decltype(prb_blanking_cfg_item_s::prb_idx)>::codec}};
static const asn1::openapi::seq_desc openapi_prb_blanking_cfg_item_s = {&prb_blanking_cfg_item_s::desc,
                                                                        openapi_prb_blanking_cfg_item_s_fields};

static const asn1::openapi::field_desc openapi_o_prb_blanking_policy_s_fields[] = {
    {},
    {},
    {&asn1::openapi::string_value<decltype(o_prb_blanking_policy_s::start_time)>::codec},
    {&asn1::openapi::string_value<decltype(o_prb_blanking_policy_s::end_time)>::codec},
    {&asn1::openapi::seq_list_value<decltype(o_prb_blanking_policy_s::prb_blanking_cfg_list),
                                    &openapi_prb_blanking_cfg_item_s>::codec},
    {&asn1::openapi::real_value::codec}};
static const asn1::openapi::seq_desc openapi_o_prb_blanking_policy_s = {&o_prb_blanking_policy_s::desc,
                                                                        openapi_o_prb_blanking_policy_s_fields};

static const char* const openapi_e2sm_ccc_ran_cfg_structure_c_alt0_keys[] = {"gnbId",
                                                                             "gnbIdLength",
                                                                             "gnbCuName",
                                                                             "plmnId",
                                                                             "x2BlockList",
                                                                             "xnBlockList",
                                                                             "x2AllowList",
                                                                             "xnAllowList",
                                                                             "x2HOBlockList",
                                                                             "xnHOBlockList"};
static const char* const openapi_e2sm_ccc_ran_cfg_structure_c_alt1_keys[] = {"gnbId",
                                                                             "gnbIdLength",
                                                                             "gnbCuUpId",
                                                                             "plmnInfoList"};
static const char* const openapi_e2sm_ccc_ran_cfg_structure_c_alt2_keys[] = {"gnbDuId",
                                                                             "gnbDuName",
                                                                             "gnbId",
                                                                             "gnbIdLength"};
static const char* const openapi_e2sm_ccc_ran_cfg_structure_c_alt3_keys[] = {"cellLocalId", "plmnInfoList"};
static const char* const openapi_e2sm_ccc_ran_cfg_structure_c_alt4_keys[] = {"cellLocalId",
                                                                             "operationalState",
                                                                             "administrativeState",
                                                                             "cellState",
                                                                             "plmnInfoList",
                                                                             "nrPci",
                                                                             "nrTac",
                                                                             "arfcnDL",
                                                                             "arfcnUL",
                                                                             "arfcnSUL",
                                                                             "bSChannelBwDL",
                                                                             "ssbFrequency",
                                                                             "ssbPeriodicity",
                                                                             "ssbSubCarrierSpacing",
                                                                             "ssbOffset",
                                                                             "ssbDuration",
                                                                             "bSChannelBwUL",
                                                                             "bSChannelBwSUL",
                                                                             "bwpList",
                                                                             "partitionList"};
static const char* const openapi_e2sm_ccc_ran_cfg_structure_c_alt5_keys[] = {"resourceType",
                                                                             "rRMPolicyMemberList",
                                                                             "rRMPolicyMaxRatio",
                                                                             "rRMPolicyMinRatio",
                                                                             "rRMPolicyDedicatedRatio"};
static const char* const openapi_e2sm_ccc_ran_cfg_structure_c_alt6_keys[] =
    {"bwpContext", "isInitialBwp", "subCarrierSpacing", "cyclicPrefix", "startRB", "numberOfRBs"};
static const char* const openapi_e2sm_ccc_ran_cfg_structure_c_alt7_keys[] = {"cesSwitch",
                                                                             "energySavingState",
                                                                             "energySavingControl"};
static const char* const openapi_e2sm_ccc_ran_cfg_structure_c_alt8_keys[] = {"policyType",
                                                                             "antennaMaskName",
                                                                             "antennaMask",
                                                                             "sleepMode",
                                                                             "dataDir",
                                                                             "symbolMask",
                                                                             "slotMask",
                                                                             "hibernateDuration",
                                                                             "esObjective",
                                                                             "perfObjectiveList"};
static const char* const openapi_e2sm_ccc_ran_cfg_structure_c_alt9_keys[] =
    {"onDurationTimer", "cycleStartOffset", "slotOffset", "configType", "activationStatus", "l1Activation"};
static const char* const openapi_e2sm_ccc_ran_cfg_structure_c_alt10_keys[] = {"oruUserPlaneConfiguration",
                                                                              "oruCapabilities"};
static const char* const openapi_e2sm_ccc_ran_cfg_structure_c_alt11_keys[] =
    {"policyCategory", "linkDir", "startTime", "endTime", "prbBlankingConfigurationList", "prbBlankingObj"};
static const asn1::openapi::alternative_desc openapi_e2sm_ccc_ran_cfg_structure_c_alternatives[] = {
    {nullptr, &openapi_o_gnb_cu_cp_function_s, openapi_e2sm_ccc_ran_cfg_structure_c_alt0_keys, 10},
    {nullptr, &openapi_o_gnb_cu_up_function_s, openapi_e2sm_ccc_ran_cfg_structure_c_alt1_keys, 4},
    {nullptr, &openapi_o_gnb_du_function_s, openapi_e2sm_ccc_ran_cfg_structure_c_alt2_keys, 4},
    {nullptr, &openapi_o_nr_cell_cu_s, openapi_e2sm_ccc_ran_cfg_structure_c_alt3_keys, 2},
    {nullptr, &openapi_o_nr_cell_du_s, openapi_e2sm_ccc_ran_cfg_structure_c_alt4_keys, 20},
    {nullptr, &openapi_o_rrm_policy_ratio_s, openapi_e2sm_ccc_ran_cfg_structure_c_alt5_keys, 5},
    {nullptr, &openapi_o_bwp_s, openapi_e2sm_ccc_ran_cfg_structure_c_alt6_keys, 6},
    {nullptr, &openapi_o_ces_management_function_s, openapi_e2sm_ccc_ran_cfg_structure_c_alt7_keys, 3},
    {nullptr, &openapi_o_nes_policy_s, openapi_e2sm_ccc_ran_cfg_structure_c_alt8_keys, 10},
    {nullptr, &openapi_o_cell_dtxdrx_cfg_s, openapi_e2sm_ccc_ran_cfg_structure_c_alt9_keys, 6},
    {nullptr, &openapi_o_ru_info_s, openapi_e2sm_ccc_ran_cfg_structure_c_alt10_keys, 2},
    {nullptr, &openapi_o_prb_blanking_policy_s, openapi_e2sm_ccc_ran_cfg_structure_c_alt11_keys, 6}};
static const asn1::openapi::choice_desc openapi_e2sm_ccc_ran_cfg_structure_c = {
    &e2sm_ccc_ran_cfg_structure_c::desc,
    openapi_e2sm_ccc_ran_cfg_structure_c_alternatives};

static const asn1::openapi::field_desc openapi_values_of_attributes_s_fields[] = {
    {nullptr, &openapi_e2sm_ccc_ran_cfg_structure_c}};
static const asn1::openapi::seq_desc openapi_values_of_attributes_s = {&values_of_attributes_s::desc,
                                                                       openapi_values_of_attributes_s_fields};

static const asn1::openapi::field_desc openapi_cfg_structure_accepted_s_fields[] = {
    {&asn1::openapi::string_value<decltype(cfg_structure_accepted_s::ran_cfg_structure_name)>::codec, nullptr, true},
    {nullptr, &openapi_values_of_attributes_s},
    {nullptr, &openapi_values_of_attributes_s},
    {&asn1::openapi::string_value<decltype(cfg_structure_accepted_s::applied_timestamp)>::codec}};
static const asn1::openapi::seq_desc openapi_cfg_structure_accepted_s = {&cfg_structure_accepted_s::desc,
                                                                         openapi_cfg_structure_accepted_s_fields};

static const asn1::openapi::field_desc openapi_cfg_structure_failed_s_fields[] = {
    {&asn1::openapi::string_value<decltype(cfg_structure_failed_s::ran_cfg_structure_name)>::codec, nullptr, true},
    {nullptr, &openapi_values_of_attributes_s},
    {nullptr, &openapi_values_of_attributes_s},
    {}};
static const asn1::openapi::seq_desc openapi_cfg_structure_failed_s = {&cfg_structure_failed_s::desc,
                                                                       openapi_cfg_structure_failed_s_fields};

static const asn1::openapi::field_desc openapi_cell_ctrl_outcome_s_fields[] = {
    {nullptr, &openapi_cell_global_id_c},
    {&asn1::openapi::seq_list_value<decltype(cell_ctrl_outcome_s::ran_cfg_structures_accepted_list),
                                    &openapi_cfg_structure_accepted_s>::codec},
    {&asn1::openapi::seq_list_value<decltype(cell_ctrl_outcome_s::ran_cfg_structures_failed_list),
                                    &openapi_cfg_structure_failed_s>::codec}};
static const asn1::openapi::seq_desc openapi_cell_ctrl_outcome_s = {&cell_ctrl_outcome_s::desc,
                                                                    openapi_cell_ctrl_outcome_s_fields};

static const asn1::openapi::field_desc openapi_cfg_structure_write_s_fields[] = {
    {&asn1::openapi::string_value<decltype(cfg_structure_write_s::ran_cfg_structure_name)>::codec, nullptr, true},
    {nullptr, &openapi_values_of_attributes_s},
    {nullptr, &openapi_values_of_attributes_s}};
static const asn1::openapi::seq_desc openapi_cfg_structure_write_s = {&cfg_structure_write_s::desc,
                                                                      openapi_cfg_structure_write_s_fields};

static const asn1::openapi::field_desc openapi_cell_ctrl_s_fields[] = {
    {nullptr, &openapi_cell_global_id_c},
    {&asn1::openapi::seq_list_value<decltype(cell_ctrl_s::list_of_cfg_structures),
                                    &openapi_cfg_structure_write_s>::codec}};
static const asn1::openapi::seq_desc openapi_cell_ctrl_s = {&cell_ctrl_s::desc, openapi_cell_ctrl_s_fields};

static const asn1::openapi::field_desc openapi_ran_cfg_structure_s_fields[] = {
    {&asn1::openapi::string_value<decltype(ran_cfg_structure_s::ran_cfg_structure_name)>::codec, nullptr, true},
    {&asn1::openapi::seq_list_value<decltype(ran_cfg_structure_s::list_of_supported_attributes),
                                    &openapi_attribute_s>::codec}};
static const asn1::openapi::seq_desc openapi_ran_cfg_structure_s = {&ran_cfg_structure_s::desc,
                                                                    openapi_ran_cfg_structure_s_fields};

static const asn1::openapi::field_desc openapi_cell_for_ran_function_definition_s_fields[] = {
    {nullptr, &openapi_cell_global_id_c},
    {&asn1::openapi::seq_list_value<
        decltype(cell_for_ran_function_definition_s::list_of_supported_cell_level_ran_cfg_structures),
        &openapi_ran_cfg_structure_s>::codec}};
static const asn1::openapi::seq_desc openapi_cell_for_ran_function_definition_s = {
    &cell_for_ran_function_definition_s::desc,
    openapi_cell_for_ran_function_definition_s_fields};

static const asn1::openapi::field_desc openapi_ran_cfg_structure_for_event_trigger_s_fields[] = {
    {&asn1::openapi::string_value<decltype(ran_cfg_structure_for_event_trigger_s::ran_cfg_structure_name)>::codec,
     nullptr,
     true},
    {&asn1::openapi::list_value<
        decltype(ran_cfg_structure_for_event_trigger_s::list_of_attributes),
        &asn1::openapi::string_value<
            decltype(ran_cfg_structure_for_event_trigger_s::list_of_attributes)::value_type>::codec>::codec}};
static const asn1::openapi::seq_desc openapi_ran_cfg_structure_for_event_trigger_s = {
    &ran_cfg_structure_for_event_trigger_s::desc,
    openapi_ran_cfg_structure_for_event_trigger_s_fields};

static const asn1::openapi::field_desc openapi_cell_level_cfg_structure_for_event_trigger_s_fields[] = {
    {nullptr, &openapi_cell_global_id_c},
    {&asn1::openapi::seq_list_value<
        decltype(cell_level_cfg_structure_for_event_trigger_s::list_of_ran_cfg_structures_for_event_trigger),
        &openapi_ran_cfg_structure_for_event_trigger_s>::codec}};
static const asn1::openapi::seq_desc openapi_cell_level_cfg_structure_for_event_trigger_s = {
    &cell_level_cfg_structure_for_event_trigger_s::desc,
    openapi_cell_level_cfg_structure_for_event_trigger_s_fields};

static const asn1::openapi::field_desc openapi_cfg_structure_s_fields[] = {
    {},
    {&asn1::openapi::string_value<decltype(cfg_structure_s::ran_cfg_structure_name)>::codec, nullptr, true},
    {nullptr, &openapi_values_of_attributes_s},
    {nullptr, &openapi_values_of_attributes_s}};
static const asn1::openapi::seq_desc openapi_cfg_structure_s = {&cfg_structure_s::desc, openapi_cfg_structure_s_fields};

static const asn1::openapi::field_desc openapi_cell_reported_s_fields[] = {
    {nullptr, &openapi_cell_global_id_c},
    {&asn1::openapi::seq_list_value<decltype(cell_reported_s::list_of_cfg_structures_reported),
                                    &openapi_cfg_structure_s>::codec}};
static const asn1::openapi::seq_desc openapi_cell_reported_s = {&cell_reported_s::desc, openapi_cell_reported_s_fields};

static const asn1::openapi::field_desc openapi_celllevel_cfg_structures_for_query_s_fields[] = {
    {&asn1::openapi::string_value<decltype(celllevel_cfg_structures_for_query_s::ran_cfg_structure_name)>::codec,
     nullptr,
     true},
    {&asn1::openapi::seq_list_value<decltype(celllevel_cfg_structures_for_query_s::list_of_attributes),
                                    &openapi_attribute_name_s>::codec}};
static const asn1::openapi::seq_desc openapi_celllevel_cfg_structures_for_query_s = {
    &celllevel_cfg_structures_for_query_s::desc,
    openapi_celllevel_cfg_structures_for_query_s_fields};

static const asn1::openapi::field_desc openapi_cells_to_be_queried_s_fields[] = {
    {nullptr, &openapi_cell_global_id_c},
    {&asn1::openapi::seq_list_value<decltype(cells_to_be_queried_s::list_of_celllevel_cfg_structures_for_query),
                                    &openapi_celllevel_cfg_structures_for_query_s>::codec}};
static const asn1::openapi::seq_desc openapi_cells_to_be_queried_s = {&cells_to_be_queried_s::desc,
                                                                      openapi_cells_to_be_queried_s_fields};

static const asn1::openapi::field_desc openapi_cfg_structures_to_be_reported_for_query_s_fields[] = {
    {&asn1::openapi::string_value<decltype(cfg_structures_to_be_reported_for_query_s::ran_cfg_structure_name)>::codec,
     nullptr,
     true},
    {nullptr, &openapi_values_of_attributes_s}};
static const asn1::openapi::seq_desc openapi_cfg_structures_to_be_reported_for_query_s = {
    &cfg_structures_to_be_reported_for_query_s::desc,
    openapi_cfg_structures_to_be_reported_for_query_s_fields};

static const asn1::openapi::field_desc openapi_cells_to_be_reported_for_query_s_fields[] = {
    {nullptr, &openapi_cell_global_id_c},
    {&asn1::openapi::seq_list_value<
        decltype(cells_to_be_reported_for_query_s::list_of_cfg_structures_to_be_reported_for_query),
        &openapi_cfg_structures_to_be_reported_for_query_s>::codec}};
static const asn1::openapi::seq_desc openapi_cells_to_be_reported_for_query_s = {
    &cells_to_be_reported_for_query_s::desc,
    openapi_cells_to_be_reported_for_query_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_ctrl_hdr_format1_s_fields[] = {{}};
static const asn1::openapi::seq_desc   openapi_e2sm_ccc_ctrl_hdr_format1_s = {&e2sm_ccc_ctrl_hdr_format1_s::desc,
                                                                              openapi_e2sm_ccc_ctrl_hdr_format1_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_ctrl_msg_format1_s_fields[] = {
    {&asn1::openapi::seq_list_value<decltype(e2sm_ccc_ctrl_msg_format1_s::list_of_cfg_structures),
                                    &openapi_cfg_structure_write_s>::codec}};
static const asn1::openapi::seq_desc openapi_e2sm_ccc_ctrl_msg_format1_s = {&e2sm_ccc_ctrl_msg_format1_s::desc,
                                                                            openapi_e2sm_ccc_ctrl_msg_format1_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_ctrl_msg_format2_s_fields[] = {
    {&asn1::openapi::seq_list_value<decltype(e2sm_ccc_ctrl_msg_format2_s::list_of_cells_ctrl),
                                    &openapi_cell_ctrl_s>::codec}};
static const asn1::openapi::seq_desc openapi_e2sm_ccc_ctrl_msg_format2_s = {&e2sm_ccc_ctrl_msg_format2_s::desc,
                                                                            openapi_e2sm_ccc_ctrl_msg_format2_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_ctrl_outcome_format1_s_fields[] = {
    {&asn1::openapi::string_value<decltype(e2sm_ccc_ctrl_outcome_format1_s::rx_timestamp)>::codec},
    {&asn1::openapi::seq_list_value<decltype(e2sm_ccc_ctrl_outcome_format1_s::ran_cfg_structures_accepted_list),
                                    &openapi_cfg_structure_accepted_s>::codec},
    {&asn1::openapi::seq_list_value<decltype(e2sm_ccc_ctrl_outcome_format1_s::ran_cfg_structures_failed_list),
                                    &openapi_cfg_structure_failed_s>::codec}};
static const asn1::openapi::seq_desc openapi_e2sm_ccc_ctrl_outcome_format1_s = {
    &e2sm_ccc_ctrl_outcome_format1_s::desc,
    openapi_e2sm_ccc_ctrl_outcome_format1_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_ctrl_outcome_format2_s_fields[] = {
    {&asn1::openapi::string_value<decltype(e2sm_ccc_ctrl_outcome_format2_s::rx_timestamp)>::codec},
    {&asn1::openapi::seq_list_value<decltype(e2sm_ccc_ctrl_outcome_format2_s::list_of_cells_for_ctrl_outcome),
                                    &openapi_cell_ctrl_outcome_s>::codec}};
static const asn1::openapi::seq_desc openapi_e2sm_ccc_ctrl_outcome_format2_s = {
    &e2sm_ccc_ctrl_outcome_format2_s::desc,
    openapi_e2sm_ccc_ctrl_outcome_format2_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_event_trigger_definition_format1_s_fields[] = {
    {&asn1::openapi::seq_list_value<
        decltype(e2sm_ccc_event_trigger_definition_format1_s::list_of_node_level_cfg_structures_for_event_trigger),
        &openapi_ran_cfg_structure_for_event_trigger_s>::codec}};
static const asn1::openapi::seq_desc openapi_e2sm_ccc_event_trigger_definition_format1_s = {
    &e2sm_ccc_event_trigger_definition_format1_s::desc,
    openapi_e2sm_ccc_event_trigger_definition_format1_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_event_trigger_definition_format2_s_fields[] = {
    {&asn1::openapi::seq_list_value<
        decltype(e2sm_ccc_event_trigger_definition_format2_s::list_of_cell_level_cfg_structures_for_event_trigger),
        &openapi_cell_level_cfg_structure_for_event_trigger_s>::codec}};
static const asn1::openapi::seq_desc openapi_e2sm_ccc_event_trigger_definition_format2_s = {
    &e2sm_ccc_event_trigger_definition_format2_s::desc,
    openapi_e2sm_ccc_event_trigger_definition_format2_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_event_trigger_definition_format3_s_fields[] = {{}};
static const asn1::openapi::seq_desc   openapi_e2sm_ccc_event_trigger_definition_format3_s          = {
    &e2sm_ccc_event_trigger_definition_format3_s::desc,
    openapi_e2sm_ccc_event_trigger_definition_format3_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_ind_hdr_format1_s_fields[] = {
    {},
    {&asn1::openapi::string_value<decltype(e2sm_ccc_ind_hdr_format1_s::event_time)>::codec}};
static const asn1::openapi::seq_desc openapi_e2sm_ccc_ind_hdr_format1_s = {&e2sm_ccc_ind_hdr_format1_s::desc,
                                                                           openapi_e2sm_ccc_ind_hdr_format1_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_ind_msg_format1_s_fields[] = {
    {&asn1::openapi::seq_list_value<decltype(e2sm_ccc_ind_msg_format1_s::list_of_cfg_structures_reported),
                                    &openapi_cfg_structure_s>::codec}};
static const asn1::openapi::seq_desc openapi_e2sm_ccc_ind_msg_format1_s = {&e2sm_ccc_ind_msg_format1_s::desc,
                                                                           openapi_e2sm_ccc_ind_msg_format1_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_ind_msg_format2_s_fields[] = {
    {&asn1::openapi::seq_list_value<decltype(e2sm_ccc_ind_msg_format2_s::list_of_cells_reported),
                                    &openapi_cell_reported_s>::codec}};
static const asn1::openapi::seq_desc openapi_e2sm_ccc_ind_msg_format2_s = {&e2sm_ccc_ind_msg_format2_s::desc,
                                                                           openapi_e2sm_ccc_ind_msg_format2_s_fields};

static const asn1::openapi::field_desc openapi_nodelevel_ran_cfg_structures_for_query_s_fields[] = {
    {&asn1::openapi::string_value<decltype(nodelevel_ran_cfg_structures_for_query_s::ran_cfg_structure_name)>::codec,
     nullptr,
     true},
    {&asn1::openapi::seq_list_value<decltype(nodelevel_ran_cfg_structures_for_query_s::list_of_attributes),
                                    &openapi_attribute_name_s>::codec}};
static const asn1::openapi::seq_desc openapi_nodelevel_ran_cfg_structures_for_query_s = {
    &nodelevel_ran_cfg_structures_for_query_s::desc,
    openapi_nodelevel_ran_cfg_structures_for_query_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_query_definition_format1_s_fields[] = {
    {&asn1::openapi::seq_list_value<
        decltype(e2sm_ccc_query_definition_format1_s::list_of_nodelevel_ran_cfg_structures_for_query),
        &openapi_nodelevel_ran_cfg_structures_for_query_s>::codec}};
static const asn1::openapi::seq_desc openapi_e2sm_ccc_query_definition_format1_s = {
    &e2sm_ccc_query_definition_format1_s::desc,
    openapi_e2sm_ccc_query_definition_format1_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_query_definition_format2_s_fields[] = {
    {&asn1::openapi::seq_list_value<
        decltype(e2sm_ccc_query_definition_format2_s::list_of_cells_to_be_queried_for_query),
        &openapi_cells_to_be_queried_s>::codec}};
static const asn1::openapi::seq_desc openapi_e2sm_ccc_query_definition_format2_s = {
    &e2sm_ccc_query_definition_format2_s::desc,
    openapi_e2sm_ccc_query_definition_format2_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_query_hdr_format1_s_fields[] = {{}};
static const asn1::openapi::seq_desc   openapi_e2sm_ccc_query_hdr_format1_s          = {
    &e2sm_ccc_query_hdr_format1_s::desc,
    openapi_e2sm_ccc_query_hdr_format1_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_query_outcome_format1_s_fields[] = {
    {&asn1::openapi::seq_list_value<
        decltype(e2sm_ccc_query_outcome_format1_s::list_of_cfg_structures_to_be_reported_for_query),
        &openapi_cfg_structures_to_be_reported_for_query_s>::codec}};
static const asn1::openapi::seq_desc openapi_e2sm_ccc_query_outcome_format1_s = {
    &e2sm_ccc_query_outcome_format1_s::desc,
    openapi_e2sm_ccc_query_outcome_format1_s_fields};

static const asn1::openapi::field_desc openapi_e2sm_ccc_query_outcome_format2_s_fields[] = {
    {&asn1::openapi::seq_list_value<decltype(e2sm_ccc_query_outcome_format2_s::list_of_cells_to_be_reported_for_query),
                                    &openapi_cells_to_be_reported_for_query_s>::codec}};
static const asn1::openapi::seq_desc openapi_e2sm_ccc_query_outcome_format2_s = {
    &e2sm_ccc_query_outcome_format2_s::desc,
    openapi_e2sm_ccc_query_outcome_format2_s_fields};

static const asn1::openapi::field_desc openapi_ran_function_name_s_fields[] = {
    {&asn1::openapi::string_value<decltype(ran_function_name_s::ran_function_short_name)>::codec},
    {&asn1::openapi::string_value<decltype(ran_function_name_s::ran_function_service_model_o_id)>::codec},
    {&asn1::openapi::string_value<decltype(ran_function_name_s::ran_function_description)>::codec},
    {}};
static const asn1::openapi::seq_desc openapi_ran_function_name_s = {&ran_function_name_s::desc,
                                                                    openapi_ran_function_name_s_fields};

static const asn1::openapi::field_desc openapi_ran_function_definition_s_fields[] = {
    {nullptr, &openapi_ran_function_name_s},
    {&asn1::openapi::seq_list_value<decltype(ran_function_definition_s::list_of_supported_node_level_cfg_structures),
                                    &openapi_ran_cfg_structure_s>::codec},
    {&asn1::openapi::seq_list_value<decltype(ran_function_definition_s::list_of_cells_for_ran_function_definition),
                                    &openapi_cell_for_ran_function_definition_s>::codec}};
const asn1::openapi::seq_desc ran_function_definition_s::openapi = {&ran_function_definition_s::desc,
                                                                    openapi_ran_function_definition_s_fields};

static const char* const                     openapi_ctrl_hdr_format_c_alt0_keys[]    = {"ricStyleType"};
static const asn1::openapi::alternative_desc openapi_ctrl_hdr_format_c_alternatives[] = {
    {nullptr, &openapi_e2sm_ccc_ctrl_hdr_format1_s, openapi_ctrl_hdr_format_c_alt0_keys, 1}};
static const asn1::openapi::choice_desc openapi_ctrl_hdr_format_c = {&ctrl_hdr_format_c::desc,
                                                                     openapi_ctrl_hdr_format_c_alternatives};

static const asn1::openapi::field_desc openapi_ric_ctrl_hdr_s_fields[] = {{nullptr, &openapi_ctrl_hdr_format_c}};
const asn1::openapi::seq_desc          ric_ctrl_hdr_s::openapi = {&ric_ctrl_hdr_s::desc, openapi_ric_ctrl_hdr_s_fields};

static const char* const                     openapi_ctrl_msg_format_c_alt0_keys[] = {"listOfConfigurationStructures"};
static const char* const                     openapi_ctrl_msg_format_c_alt1_keys[] = {"listOfCellsControlled"};
static const asn1::openapi::alternative_desc openapi_ctrl_msg_format_c_alternatives[] = {
    {nullptr, &openapi_e2sm_ccc_ctrl_msg_format1_s, openapi_ctrl_msg_format_c_alt0_keys, 1},
    {nullptr, &openapi_e2sm_ccc_ctrl_msg_format2_s, openapi_ctrl_msg_format_c_alt1_keys, 1}};
static const asn1::openapi::choice_desc openapi_ctrl_msg_format_c = {&ctrl_msg_format_c::desc,
                                                                     openapi_ctrl_msg_format_c_alternatives};

static const asn1::openapi::field_desc openapi_ric_ctrl_msg_s_fields[] = {{nullptr, &openapi_ctrl_msg_format_c}};
const asn1::openapi::seq_desc          ric_ctrl_msg_s::openapi = {&ric_ctrl_msg_s::desc, openapi_ric_ctrl_msg_s_fields};

static const asn1::openapi::field_desc openapi_ric_ctrl_outcome_s_fields[] = {
    {nullptr, &ctrl_outcome_format_c::openapi}};
const asn1::openapi::seq_desc ric_ctrl_outcome_s::openapi = {&ric_ctrl_outcome_s::desc,
                                                             openapi_ric_ctrl_outcome_s_fields};

static const char* const                     openapi_ind_hdr_format_c_alt0_keys[] = {"indicationReason", "eventTime"};
static const asn1::openapi::alternative_desc openapi_ind_hdr_format_c_alternatives[] = {
    {nullptr, &openapi_e2sm_ccc_ind_hdr_format1_s, openapi_ind_hdr_format_c_alt0_keys, 2}};
static const asn1::openapi::choice_desc openapi_ind_hdr_format_c = {&ind_hdr_format_c::desc,
                                                                    openapi_ind_hdr_format_c_alternatives};

static const asn1::openapi::field_desc openapi_ric_ind_hdr_s_fields[] = {{nullptr, &openapi_ind_hdr_format_c}};
const asn1::openapi::seq_desc          ric_ind_hdr_s::openapi = {&ric_ind_hdr_s::desc, openapi_ric_ind_hdr_s_fields};

static const char* const openapi_ind_msg_format_c_alt0_keys[] = {"listOfConfigurationStructuresReported"};
static const char* const openapi_ind_msg_format_c_alt1_keys[] = {"listOfCellsReported"};
static const asn1::openapi::alternative_desc openapi_ind_msg_format_c_alternatives[] = {
    {nullptr, &openapi_e2sm_ccc_ind_msg_format1_s, openapi_ind_msg_format_c_alt0_keys, 1},
    {nullptr, &openapi_e2sm_ccc_ind_msg_format2_s, openapi_ind_msg_format_c_alt1_keys, 1}};
static const asn1::openapi::choice_desc openapi_ind_msg_format_c = {&ind_msg_format_c::desc,
                                                                    openapi_ind_msg_format_c_alternatives};

static const asn1::openapi::field_desc openapi_ric_ind_msg_s_fields[] = {{nullptr, &openapi_ind_msg_format_c}};
const asn1::openapi::seq_desc          ric_ind_msg_s::openapi = {&ric_ind_msg_s::desc, openapi_ric_ind_msg_s_fields};

static const char* const openapi_query_definition_format_c_alt0_keys[] = {
    "listOfNodelevelRanConfigurationStructuresForQuery"};
static const char* const openapi_query_definition_format_c_alt1_keys[] = {"listOfCellsToBeQueriedForQuery"};
static const asn1::openapi::alternative_desc openapi_query_definition_format_c_alternatives[] = {
    {nullptr, &openapi_e2sm_ccc_query_definition_format1_s, openapi_query_definition_format_c_alt0_keys, 1},
    {nullptr, &openapi_e2sm_ccc_query_definition_format2_s, openapi_query_definition_format_c_alt1_keys, 1}};
static const asn1::openapi::choice_desc openapi_query_definition_format_c = {
    &query_definition_format_c::desc,
    openapi_query_definition_format_c_alternatives};

static const asn1::openapi::field_desc openapi_ric_query_definition_s_fields[] = {
    {nullptr, &openapi_query_definition_format_c}};
const asn1::openapi::seq_desc ric_query_definition_s::openapi = {&ric_query_definition_s::desc,
                                                                 openapi_ric_query_definition_s_fields};

static const char* const                     openapi_query_hdr_format_c_alt0_keys[]    = {"ricStyleType"};
static const asn1::openapi::alternative_desc openapi_query_hdr_format_c_alternatives[] = {
    {nullptr, &openapi_e2sm_ccc_query_hdr_format1_s, openapi_query_hdr_format_c_alt0_keys, 1}};
static const asn1::openapi::choice_desc openapi_query_hdr_format_c = {&query_hdr_format_c::desc,
                                                                      openapi_query_hdr_format_c_alternatives};

static const asn1::openapi::field_desc openapi_ric_query_hdr_s_fields[] = {{nullptr, &openapi_query_hdr_format_c}};
const asn1::openapi::seq_desc ric_query_hdr_s::openapi = {&ric_query_hdr_s::desc, openapi_ric_query_hdr_s_fields};

static const char* const openapi_query_outcome_format_c_alt0_keys[] = {
    "listOfConfigurationStructuresToBeReportedForQuery"};
static const char* const openapi_query_outcome_format_c_alt1_keys[] = {"listOfCellsToBeReportedForQuery"};
static const asn1::openapi::alternative_desc openapi_query_outcome_format_c_alternatives[] = {
    {nullptr, &openapi_e2sm_ccc_query_outcome_format1_s, openapi_query_outcome_format_c_alt0_keys, 1},
    {nullptr, &openapi_e2sm_ccc_query_outcome_format2_s, openapi_query_outcome_format_c_alt1_keys, 1}};
static const asn1::openapi::choice_desc openapi_query_outcome_format_c = {&query_outcome_format_c::desc,
                                                                          openapi_query_outcome_format_c_alternatives};

static const asn1::openapi::field_desc openapi_ric_query_outcome_s_fields[] = {
    {nullptr, &openapi_query_outcome_format_c}};
const asn1::openapi::seq_desc ric_query_outcome_s::openapi = {&ric_query_outcome_s::desc,
                                                              openapi_ric_query_outcome_s_fields};

static const char* const openapi_action_definition_format_c_alt0_keys[] = {
    "listOfNodeLevelRANConfigurationStructuresForADF"};
static const char* const openapi_action_definition_format_c_alt1_keys[] = {
    "listOfCellConfigurationsToBeReportedForADF"};
static const asn1::openapi::alternative_desc openapi_action_definition_format_c_alternatives[] = {
    {nullptr, &openapi_e2sm_ccc_action_definition_format1_s, openapi_action_definition_format_c_alt0_keys, 1},
    {nullptr, &openapi_e2sm_ccc_action_definition_format2_s, openapi_action_definition_format_c_alt1_keys, 1}};
static const asn1::openapi::choice_desc openapi_action_definition_format_c = {
    &action_definition_format_c::desc,
    openapi_action_definition_format_c_alternatives};

static const asn1::openapi::field_desc openapi_ric_action_definition_s_fields[] = {
    {},
    {nullptr, &openapi_action_definition_format_c}};
const asn1::openapi::seq_desc ric_action_definition_s::openapi = {&ric_action_definition_s::desc,
                                                                  openapi_ric_action_definition_s_fields};

static const char* const openapi_event_trigger_definition_format_c_alt0_keys[] = {
    "listOfNodeLevelConfigurationStructuresForEventTrigger"};
static const char* const openapi_event_trigger_definition_format_c_alt1_keys[] = {
    "listOfCellLevelConfigurationStructuresForEventTrigger"};
static const char* const                     openapi_event_trigger_definition_format_c_alt2_keys[]    = {"period"};
static const asn1::openapi::alternative_desc openapi_event_trigger_definition_format_c_alternatives[] = {
    {nullptr,
     &openapi_e2sm_ccc_event_trigger_definition_format1_s,
     openapi_event_trigger_definition_format_c_alt0_keys,
     1},
    {nullptr,
     &openapi_e2sm_ccc_event_trigger_definition_format2_s,
     openapi_event_trigger_definition_format_c_alt1_keys,
     1},
    {nullptr,
     &openapi_e2sm_ccc_event_trigger_definition_format3_s,
     openapi_event_trigger_definition_format_c_alt2_keys,
     1}};
static const asn1::openapi::choice_desc openapi_event_trigger_definition_format_c = {
    &event_trigger_definition_format_c::desc,
    openapi_event_trigger_definition_format_c_alternatives};

static const asn1::openapi::field_desc openapi_ric_event_trigger_definition_s_fields[] = {
    {nullptr, &openapi_event_trigger_definition_format_c}};
const asn1::openapi::seq_desc ric_event_trigger_definition_s::openapi = {&ric_event_trigger_definition_s::desc,
                                                                         openapi_ric_event_trigger_definition_s_fields};

static const char* const                     openapi_ctrl_outcome_format_c_alt0_keys[]    = {"receivedTimestamp",
                                                                                             "ranConfigurationStructuresAcceptedList",
                                                                                             "ranConfigurationStructuresFailedList"};
static const char* const                     openapi_ctrl_outcome_format_c_alt1_keys[]    = {"receivedTimestamp",
                                                                                             "listOfCellsForControlOutcome"};
static const asn1::openapi::alternative_desc openapi_ctrl_outcome_format_c_alternatives[] = {
    {nullptr, &openapi_e2sm_ccc_ctrl_outcome_format1_s, openapi_ctrl_outcome_format_c_alt0_keys, 3},
    {nullptr, &openapi_e2sm_ccc_ctrl_outcome_format2_s, openapi_ctrl_outcome_format_c_alt1_keys, 2}};
const asn1::openapi::choice_desc ctrl_outcome_format_c::openapi = {&ctrl_outcome_format_c::desc,
                                                                   openapi_ctrl_outcome_format_c_alternatives};

} // namespace e2sm_ccc
} // namespace asn1
