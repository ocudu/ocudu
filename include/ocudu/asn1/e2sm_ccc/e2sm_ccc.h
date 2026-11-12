// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/*******************************************************************************
 *
 *                    O-RAN WG3 TS E2SM-CCC v08.00 (2026-10)
 *
 ******************************************************************************/

#pragma once

#include "ocudu/asn1/asn1_ap_utils.h"

namespace asn1 {

// The tables that shape the JSON of these types, defined in asn1_openapi_codec.h. A type names them without their
// definition, so that this header carries no codec of the JSON.
namespace openapi {
struct seq_desc;
struct choice_desc;
} // namespace openapi

namespace e2sm_ccc {

/*******************************************************************************
 *                             Constant Definitions
 ******************************************************************************/

#define ASN1_E2SM_CCC_MAXNOOF_ELEMS 64

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// AttributeName ::= SEQUENCE
struct attribute_name_s {
  printable_string<1, 150, true, true> attribute_name;

  static const asn1::table::seq_desc desc;
};

// RANConfigurationStructureForADF ::= SEQUENCE
struct ran_cfg_structure_for_adf_s {
  struct report_type_opts {
    enum options { all, change, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using report_type_e_        = enumerated<report_type_opts>;
  using list_of_attributes_l_ = dyn_array<attribute_name_s>;

  // member variables
  report_type_e_                       report_type;
  printable_string<0, 150, true, true> ran_cfg_structure_name;
  list_of_attributes_l_                list_of_attributes;

  static const asn1::table::seq_desc desc;
};

// ListOfRANConfigurationStructuresForADF ::= SEQUENCE (SIZE (1..64)) OF RANConfigurationStructureForADF
using list_of_ran_cfg_structures_for_adf_l = dyn_array<ran_cfg_structure_for_adf_s>;

// E2SM-CCC-ActionDefinitionFormat1 ::= SEQUENCE
struct e2sm_ccc_action_definition_format1_s {
  list_of_ran_cfg_structures_for_adf_l list_of_node_level_ran_cfg_structures_for_adf;

  static const asn1::table::seq_desc desc;
};

// PlmnId ::= SEQUENCE
struct plmn_id_s {
  bool                               mcc_present = false;
  bool                               mnc_present = false;
  printable_string<3, 3, true, true> mcc;
  printable_string<2, 3, true, true> mnc;

  static const asn1::table::seq_desc desc;
};

// NR-CGI ::= SEQUENCE
struct nr_cgi_s {
  plmn_id_s                        plmn_id;
  fixed_bitstring<36, false, true> nr_cell_id;

  static const asn1::table::seq_desc desc;
};

// EUTRA-CGI ::= SEQUENCE
struct eutra_cgi_s {
  plmn_id_s                        plmn_id;
  fixed_bitstring<28, false, true> eutra_cell_id;

  static const asn1::table::seq_desc desc;
};

// CellGlobalId ::= CHOICE
struct cell_global_id_c {
  struct types_opts {
    enum options { nr_cgi, eutra_cgi, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  nr_cgi_s&                                     nr_cgi()
  {
    assert_choice_type(types::nr_cgi, c.type, "CellGlobalId");
    return c.get<nr_cgi_s>();
  }
  eutra_cgi_s& eutra_cgi()
  {
    assert_choice_type(types::eutra_cgi, c.type, "CellGlobalId");
    return c.get<eutra_cgi_s>();
  }
  const nr_cgi_s& nr_cgi() const
  {
    assert_choice_type(types::nr_cgi, c.type, "CellGlobalId");
    return c.get<nr_cgi_s>();
  }
  const eutra_cgi_s& eutra_cgi() const
  {
    assert_choice_type(types::eutra_cgi, c.type, "CellGlobalId");
    return c.get<eutra_cgi_s>();
  }
  nr_cgi_s&    set_nr_cgi();
  eutra_cgi_s& set_eutra_cgi();

private:
  asn1::table::choice_storage<cell_global_id_c, types, choice_buffer_t<eutra_cgi_s, nr_cgi_s>> c;
};

// CellConfigurationToBeReportedForADF ::= SEQUENCE
struct cell_cfg_to_be_reported_for_adf_s {
  bool                                 cell_global_id_present = false;
  cell_global_id_c                     cell_global_id;
  list_of_ran_cfg_structures_for_adf_l list_of_cell_level_ran_cfg_structures_for_adf;

  static const asn1::table::seq_desc desc;
};

// ListOfCellConfigurationsToBeReportedForADF ::= SEQUENCE (SIZE (1..64)) OF CellConfigurationToBeReportedForADF
using list_of_cell_cfgs_to_be_reported_for_adf_l = dyn_array<cell_cfg_to_be_reported_for_adf_s>;

// E2SM-CCC-ActionDefinitionFormat2 ::= SEQUENCE
struct e2sm_ccc_action_definition_format2_s {
  list_of_cell_cfgs_to_be_reported_for_adf_l list_of_cell_cfgs_to_be_reported_for_adf;

  static const asn1::table::seq_desc desc;
};

// ActionDefinitionFormat ::= CHOICE
struct action_definition_format_c {
  struct types_opts {
    enum options { action_definition_format1, action_definition_format2, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  e2sm_ccc_action_definition_format1_s&         action_definition_format1()
  {
    assert_choice_type(types::action_definition_format1, c.type, "ActionDefinitionFormat");
    return c.get<e2sm_ccc_action_definition_format1_s>();
  }
  e2sm_ccc_action_definition_format2_s& action_definition_format2()
  {
    assert_choice_type(types::action_definition_format2, c.type, "ActionDefinitionFormat");
    return c.get<e2sm_ccc_action_definition_format2_s>();
  }
  const e2sm_ccc_action_definition_format1_s& action_definition_format1() const
  {
    assert_choice_type(types::action_definition_format1, c.type, "ActionDefinitionFormat");
    return c.get<e2sm_ccc_action_definition_format1_s>();
  }
  const e2sm_ccc_action_definition_format2_s& action_definition_format2() const
  {
    assert_choice_type(types::action_definition_format2, c.type, "ActionDefinitionFormat");
    return c.get<e2sm_ccc_action_definition_format2_s>();
  }
  e2sm_ccc_action_definition_format1_s& set_action_definition_format1();
  e2sm_ccc_action_definition_format2_s& set_action_definition_format2();

private:
  asn1::table::choice_storage<
      action_definition_format_c,
      types,
      choice_buffer_t<e2sm_ccc_action_definition_format1_s, e2sm_ccc_action_definition_format2_s>>
      c;
};

// AdministrativeState ::= ENUMERATED
struct administrative_state_opts {
  enum options { locked, shuttingdown, unlocked, nulltype } value;

  static const asn1::table::enum_desc desc;
};
using administrative_state_e = enumerated<administrative_state_opts>;

// SleepMode ::= SEQUENCE
struct sleep_mode_s {
  struct sleep_mode_type_opts {
    enum options { sleep_mode0, sleep_mode1, sleep_mode2, sleep_mode3, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using sleep_mode_type_e_ = enumerated<sleep_mode_type_opts>;

  // member variables
  sleep_mode_type_e_ sleep_mode_type;
  int64_t            wakeup_dur            = 0;
  bool               wakeup_dur_guaranteed = false;

  static const asn1::table::seq_desc desc;
};

// SleepModes ::= SEQUENCE (SIZE (1..64)) OF SleepMode
using sleep_modes_l = dyn_array<sleep_mode_s>;

// AsmCapabilityInfo ::= SEQUENCE
struct asm_cap_info_s {
  sleep_modes_l sleep_modes;
  bool          defined_dur_sleep_supported   = false;
  bool          undefined_dur_sleep_supported = false;

  static const asn1::table::seq_desc desc;
};

// EventTriggerStyle ::= SEQUENCE
struct event_trigger_style_s {
  int64_t                              event_trigger_style_type = 0;
  printable_string<1, 150, true, true> event_trigger_style_name;
  int64_t                              event_trigger_format_type = 0;

  static const asn1::table::seq_desc desc;
};

// ListOfSupportedEventTriggerStyles ::= SEQUENCE (SIZE (1..64)) OF EventTriggerStyle
using list_of_supported_event_trigger_styles_l = dyn_array<event_trigger_style_s>;

// EventTrigger ::= SEQUENCE
struct event_trigger_s {
  list_of_supported_event_trigger_styles_l list_of_supported_event_trigger_styles;

  static const asn1::table::seq_desc desc;
};

// EventTriggerStyleType ::= SEQUENCE
struct event_trigger_style_type_s {
  int64_t event_trigger_style_type = 0;

  static const asn1::table::seq_desc desc;
};

// ReportStyle ::= SEQUENCE
struct report_style_s {
  using list_of_supported_event_trigger_styles_for_report_style_l_ = dyn_array<event_trigger_style_type_s>;

  // member variables
  int64_t                                                    report_service_style_type = 0;
  printable_string<1, 150, true, true>                       report_service_style_name;
  list_of_supported_event_trigger_styles_for_report_style_l_ list_of_supported_event_trigger_styles_for_report_style;
  int64_t                                                    report_service_action_definition_format_type = 0;
  int64_t                                                    report_service_ind_hdr_format_type           = 0;
  int64_t                                                    report_service_ind_msg_format_type           = 0;

  static const asn1::table::seq_desc desc;
};

// ListOfSupportedReportStyles ::= SEQUENCE (SIZE (1..64)) OF ReportStyle
using list_of_supported_report_styles_l = dyn_array<report_style_s>;

// ReportService ::= SEQUENCE
struct report_service_s {
  list_of_supported_report_styles_l list_of_supported_report_styles;

  static const asn1::table::seq_desc desc;
};

// ControlStyle ::= SEQUENCE
struct ctrl_style_s {
  bool                                 ric_call_process_id_format_type_present = false;
  int64_t                              ctrl_service_style_type                 = 0;
  printable_string<1, 150, true, true> ctrl_service_style_name;
  int64_t                              ctrl_service_hdr_format_type          = 0;
  int64_t                              ctrl_service_msg_format_type          = 0;
  int64_t                              ric_call_process_id_format_type       = 0;
  int64_t                              ctrl_service_ctrl_outcome_format_type = 0;

  static const asn1::table::seq_desc desc;
};

// ListOfSupportedControlStyles ::= SEQUENCE (SIZE (1..64)) OF ControlStyle
using list_of_supported_ctrl_styles_l = dyn_array<ctrl_style_s>;

// ControlService ::= SEQUENCE
struct ctrl_service_s {
  list_of_supported_ctrl_styles_l list_of_supported_ctrl_styles;

  static const asn1::table::seq_desc desc;
};

// QueryStyle ::= SEQUENCE
struct query_style_s {
  int64_t                              query_service_style_type = 0;
  printable_string<0, 150, true, true> query_service_style_name;
  int64_t                              query_service_hdr_format_type        = 0;
  int64_t                              query_service_definition_format_type = 0;
  int64_t                              query_service_outcome_format_type    = 0;

  static const asn1::table::seq_desc desc;
};

// ListOfSupportedQueryStyles ::= SEQUENCE (SIZE (1..64)) OF QueryStyle
using list_of_supported_query_styles_l = dyn_array<query_style_s>;

// QueryService ::= SEQUENCE
struct query_service_s {
  list_of_supported_query_styles_l list_of_supported_query_styles;

  static const asn1::table::seq_desc desc;
};

// RICServices ::= SEQUENCE
struct ric_services_s {
  bool             event_trigger_present  = false;
  bool             report_service_present = false;
  bool             insert_service_present = false;
  bool             ctrl_service_present   = false;
  bool             policy_service_present = false;
  bool             query_service_present  = false;
  event_trigger_s  event_trigger;
  report_service_s report_service;
  int64_t          insert_service = 0;
  ctrl_service_s   ctrl_service;
  int64_t          policy_service = 0;
  query_service_s  query_service;

  static const asn1::table::seq_desc desc;
};

// Attribute ::= SEQUENCE
struct attribute_s {
  printable_string<1, 150, true, true> attribute_name;
  ric_services_s                       supported_services;

  static const asn1::table::seq_desc desc;
};

// BwpContext ::= ENUMERATED
struct bwp_context_opts {
  enum options { dl, ul, sul, nulltype } value;

  static const asn1::table::enum_desc desc;
};
using bwp_context_e = enumerated<bwp_context_opts>;

// Cause ::= ENUMERATED
struct cause_opts {
  enum options {
    not_supported,
    not_available,
    incompatible_state,
    json_error,
    semantic_error,
    unspecified,
    nulltype
  } value;

  static const asn1::table::enum_desc desc;
};
using cause_e = enumerated<cause_opts>;

// GEnbIdList ::= SEQUENCE (SIZE (1..64)) OF PrintableString (SIZE (10..17,...))
using g_enb_id_list_l = dyn_array<printable_string<10, 17, true, true>>;

// GGnbIdList ::= SEQUENCE (SIZE (1..64)) OF PrintableString (SIZE (10..20,...))
using g_gnb_id_list_l = dyn_array<printable_string<10, 20, true, true>>;

// O-GnbCuCpFunction ::= SEQUENCE
struct o_gnb_cu_cp_function_s {
  bool                                 gnb_id_present      = false;
  bool                                 gnb_id_len_present  = false;
  bool                                 gnb_cu_name_present = false;
  bool                                 plmn_id_present     = false;
  uint64_t                             gnb_id              = 0;
  uint8_t                              gnb_id_len          = 22;
  printable_string<1, 150, true, true> gnb_cu_name;
  plmn_id_s                            plmn_id;
  g_enb_id_list_l                      x2_block_list;
  g_gnb_id_list_l                      xn_block_list;
  g_enb_id_list_l                      x2_allow_list;
  g_gnb_id_list_l                      xn_allow_list;
  g_enb_id_list_l                      x2_ho_block_list;
  g_gnb_id_list_l                      xn_ho_block_list;

  static const asn1::table::seq_desc desc;
};

// Snssai ::= SEQUENCE
struct snssai_s {
  bool                     sst_present = false;
  bool                     sd_present  = false;
  uint16_t                 sst         = 0;
  fixed_octstring<3, true> sd;

  static const asn1::table::seq_desc desc;
};

// PlmnInfo ::= SEQUENCE
struct plmn_info_s {
  bool      plmn_id_present = false;
  bool      snssai_present  = false;
  plmn_id_s plmn_id;
  snssai_s  snssai;

  static const asn1::table::seq_desc   desc;
  static const asn1::openapi::seq_desc openapi;
};

// PlmnInfoList ::= SEQUENCE (SIZE (1..64)) OF PlmnInfo
using plmn_info_list_l = dyn_array<plmn_info_s>;

// O-GnbCuUpFunction ::= SEQUENCE
struct o_gnb_cu_up_function_s {
  bool             gnb_id_present       = false;
  bool             gnb_id_len_present   = false;
  bool             gnb_cu_up_id_present = false;
  uint64_t         gnb_id               = 0;
  uint8_t          gnb_id_len           = 22;
  uint64_t         gnb_cu_up_id         = 0;
  plmn_info_list_l plmn_info_list;

  static const asn1::table::seq_desc desc;
};

// O-GnbDuFunction ::= SEQUENCE
struct o_gnb_du_function_s {
  bool                                 gnb_du_id_present   = false;
  bool                                 gnb_du_name_present = false;
  bool                                 gnb_id_present      = false;
  bool                                 gnb_id_len_present  = false;
  uint64_t                             gnb_du_id           = 0;
  printable_string<1, 150, true, true> gnb_du_name;
  uint64_t                             gnb_id     = 0;
  uint8_t                              gnb_id_len = 22;

  static const asn1::table::seq_desc desc;
};

// O-NrCellCu ::= SEQUENCE
struct o_nr_cell_cu_s {
  bool             cell_local_id_present = false;
  int64_t          cell_local_id         = 0;
  plmn_info_list_l plmn_info_list;

  static const asn1::table::seq_desc desc;
};

// OperationalState ::= ENUMERATED
struct operational_state_opts {
  enum options { enabled, disabled, nulltype } value;

  static const asn1::table::enum_desc desc;
};
using operational_state_e = enumerated<operational_state_opts>;

// CellState ::= ENUMERATED
struct cell_state_opts {
  enum options { id_le, inactive, active, nulltype } value;

  static const asn1::table::enum_desc desc;
};
using cell_state_e = enumerated<cell_state_opts>;

// IsInitialBwp ::= ENUMERATED
struct is_init_bwp_opts {
  enum options { init, other, sul, nulltype } value;

  static const asn1::table::enum_desc desc;
};
using is_init_bwp_e = enumerated<is_init_bwp_opts>;

// CyclicPrefix ::= ENUMERATED
struct cp_opts {
  enum options { n15, n30, n60, n120, nulltype } value;

  static const asn1::table::enum_desc desc;
  uint8_t                             to_number() const;
};
using cp_e = enumerated<cp_opts>;

// O-Bwp ::= SEQUENCE
struct o_bwp_s {
  bool          bwp_context_present         = false;
  bool          is_init_bwp_present         = false;
  bool          sub_carrier_spacing_present = false;
  bool          cp_present                  = false;
  bool          start_rb_present            = false;
  bool          nof_rbs_present             = false;
  bwp_context_e bwp_context;
  is_init_bwp_e is_init_bwp;
  int64_t       sub_carrier_spacing = 0;
  cp_e          cp;
  int64_t       start_rb = 0;
  int64_t       nof_rbs  = 0;

  static const asn1::table::seq_desc desc;
};

// M-5QiList ::= SEQUENCE (SIZE (1..64)) OF INTEGER (0..255)
using m_5qi_list_l = dyn_array<uint16_t>;

struct partition_flow_list_item_s_ {
  bool         snssai_present  = false;
  bool         plmn_id_present = false;
  snssai_s     snssai;
  plmn_id_s    plmn_id;
  m_5qi_list_l n5qi_list;

  static const asn1::table::seq_desc desc;
};

// PartitionFlowList ::= SEQUENCE (SIZE (1..64)) OF PartitionFlowList-item
using partition_flow_list_l = dyn_array<partition_flow_list_item_s_>;

struct partition_list_item_s_ {
  bool                  poffset_to_point_a_present = false;
  bool                  pnof_rbs_present           = false;
  int64_t               poffset_to_point_a         = 0;
  int64_t               pnof_rbs                   = 0;
  partition_flow_list_l partition_flow_list;

  static const asn1::table::seq_desc desc;
};

// PartitionList ::= SEQUENCE (SIZE (1..64)) OF PartitionList-item
using partition_list_l = dyn_array<partition_list_item_s_>;

// O-NrCellDu ::= SEQUENCE
struct o_nr_cell_du_s {
  using bwp_list_l_ = dyn_array<o_bwp_s>;

  // member variables
  bool                   cell_local_id_present           = false;
  bool                   operational_state_present       = false;
  bool                   administrative_state_present    = false;
  bool                   cell_state_present              = false;
  bool                   nr_pci_present                  = false;
  bool                   nr_tac_present                  = false;
  bool                   arfcn_dl_present                = false;
  bool                   arfcn_ul_present                = false;
  bool                   arfcn_sul_present               = false;
  bool                   bs_ch_bw_dl_present             = false;
  bool                   ssb_freq_present                = false;
  bool                   ssb_periodicity_present         = false;
  bool                   ssb_sub_carrier_spacing_present = false;
  bool                   ssb_offset_present              = false;
  bool                   ssb_dur_present                 = false;
  bool                   bs_ch_bw_ul_present             = false;
  bool                   bs_ch_bw_sul_present            = false;
  int64_t                cell_local_id                   = 0;
  operational_state_e    operational_state;
  administrative_state_e administrative_state;
  cell_state_e           cell_state;
  plmn_info_list_l       plmn_info_list;
  uint16_t               nr_pci                  = 0;
  uint32_t               nr_tac                  = 0;
  int64_t                arfcn_dl                = 0;
  int64_t                arfcn_ul                = 0;
  int64_t                arfcn_sul               = 0;
  int64_t                bs_ch_bw_dl             = 0;
  uint32_t               ssb_freq                = 0;
  int64_t                ssb_periodicity         = 0;
  int64_t                ssb_sub_carrier_spacing = 0;
  uint8_t                ssb_offset              = 0;
  int64_t                ssb_dur                 = 0;
  int64_t                bs_ch_bw_ul             = 0;
  int64_t                bs_ch_bw_sul            = 0;
  bwp_list_l_            bwp_list;
  partition_list_l       partition_list;

  static const asn1::table::seq_desc desc;
};

// ResourceType ::= ENUMERATED
struct res_type_opts {
  enum options { prb_dl, prb_ul, drb, rrc, nulltype } value;

  static const asn1::table::enum_desc desc;
};
using res_type_e = enumerated<res_type_opts>;

// RrmPolicyMember ::= SEQUENCE
struct rrm_policy_member_s {
  bool      plmn_id_present = false;
  bool      snssai_present  = false;
  plmn_id_s plmn_id;
  snssai_s  snssai;

  static const asn1::table::seq_desc desc;
};

// RrmPolicyMemberList ::= SEQUENCE (SIZE (1..64)) OF RrmPolicyMember
using rrm_policy_member_list_l = dyn_array<rrm_policy_member_s>;

// O-RRMPolicyRatio ::= SEQUENCE
struct o_rrm_policy_ratio_s {
  bool                     res_type_present             = false;
  bool                     rrm_policy_max_ratio_present = false;
  bool                     rrm_policy_min_ratio_present = false;
  bool                     rrm_policy_ded_ratio_present = false;
  res_type_e               res_type;
  rrm_policy_member_list_l rrm_policy_member_list;
  int64_t                  rrm_policy_max_ratio = 0;
  int64_t                  rrm_policy_min_ratio = 0;
  int64_t                  rrm_policy_ded_ratio = 0;

  static const asn1::table::seq_desc desc;
};

// O-CESManagementFunction ::= SEQUENCE
struct o_ces_management_function_s {
  struct energy_saving_state_opts {
    enum options { is_not_energy_saving, is_energy_saving, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using energy_saving_state_e_ = enumerated<energy_saving_state_opts>;
  struct energy_saving_ctrl_opts {
    enum options { to_be_energy_saving, to_be_not_energy_saving, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using energy_saving_ctrl_e_ = enumerated<energy_saving_ctrl_opts>;

  // member variables
  bool                   ces_switch_present          = false;
  bool                   energy_saving_state_present = false;
  bool                   energy_saving_ctrl_present  = false;
  bool                   ces_switch                  = false;
  energy_saving_state_e_ energy_saving_state;
  energy_saving_ctrl_e_  energy_saving_ctrl;

  static const asn1::table::seq_desc desc;
};

// EsObjective ::= CHOICE
struct es_objive_c {
  struct target_ec_s_ {
    bool   target_ec_present = false;
    real_s target_ec;

    static const asn1::table::seq_desc desc;
  };
  struct es_percentage_s_ {
    bool    es_percentage_present = false;
    uint8_t es_percentage         = 0;

    static const asn1::table::seq_desc desc;
  };
  struct types_opts {
    enum options { target_ec, es_percentage, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  target_ec_s_&                                 target_ec()
  {
    assert_choice_type(types::target_ec, c.type, "EsObjective");
    return c.get<target_ec_s_>();
  }
  es_percentage_s_& es_percentage()
  {
    assert_choice_type(types::es_percentage, c.type, "EsObjective");
    return c.get<es_percentage_s_>();
  }
  const target_ec_s_& target_ec() const
  {
    assert_choice_type(types::target_ec, c.type, "EsObjective");
    return c.get<target_ec_s_>();
  }
  const es_percentage_s_& es_percentage() const
  {
    assert_choice_type(types::es_percentage, c.type, "EsObjective");
    return c.get<es_percentage_s_>();
  }
  target_ec_s_&     set_target_ec();
  es_percentage_s_& set_es_percentage();

private:
  asn1::table::choice_storage<es_objive_c, types, choice_buffer_t<es_percentage_s_, target_ec_s_>> c;
};

// PerfObjective ::= SEQUENCE
struct perf_objive_s {
  bool                                 max_ngbr_flow_br_present = false;
  bool                                 flow_br_avg_win_present  = false;
  bool                                 max_pd_present           = false;
  bool                                 target_pd_present        = false;
  plmn_info_list_l                     plmn_info_list;
  uint16_t                             five_qi_value = 0;
  printable_string<0, 400, true, true> max_ngbr_flow_br;
  uint16_t                             flow_br_avg_win = 0;
  uint16_t                             max_pd          = 0;
  uint16_t                             target_pd       = 0;

  static const asn1::table::seq_desc desc;
};

// PerfObjectiveList ::= SEQUENCE (SIZE (1..64)) OF PerfObjective
using perf_objive_list_l = dyn_array<perf_objive_s>;

// O-NESPolicy ::= SEQUENCE
struct o_nes_policy_s {
  struct policy_type_opts {
    enum options { asm_value, trx_ctrl, deep_hibernate, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using policy_type_e_ = enumerated<policy_type_opts>;
  struct sleep_mode_opts {
    enum options { sleep_mode0, sleep_mode1, sleep_mode2, sleep_mode3, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using sleep_mode_e_ = enumerated<sleep_mode_opts>;
  struct data_dir_opts {
    enum options { dl, ul, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using data_dir_e_ = enumerated<data_dir_opts>;

  // member variables
  bool                                 policy_type_present   = false;
  bool                                 ant_mask_name_present = false;
  bool                                 sleep_mode_present    = false;
  bool                                 data_dir_present      = false;
  bool                                 symbol_mask_present   = false;
  bool                                 hibernate_dur_present = false;
  bool                                 es_objive_present     = false;
  policy_type_e_                       policy_type;
  printable_string<1, 150, true, true> ant_mask_name;
  unbounded_octstring<true>            ant_mask;
  sleep_mode_e_                        sleep_mode;
  data_dir_e_                          data_dir;
  uint16_t                             symbol_mask = 0;
  unbounded_octstring<true>            slot_mask;
  uint64_t                             hibernate_dur = 0;
  es_objive_c                          es_objive;
  perf_objive_list_l                   perf_objive_list;

  static const asn1::table::seq_desc desc;
};

// OnDurationTimer ::= CHOICE
struct on_dur_timer_c {
  struct timeout_opts {
    enum options {
      ms1,
      ms2,
      ms3,
      ms4,
      ms5,
      ms6,
      ms8,
      ms10,
      ms20,
      ms40,
      ms50,
      ms60,
      ms80,
      ms100,
      ms200,
      ms300,
      ms400,
      ms500,
      ms600,
      ms800,
      ms1000,
      ms1200,
      ms1600,
      nulltype
    } value;

    static const asn1::table::enum_desc desc;
    uint16_t                            to_number() const;
  };
  using timeout_e_ = enumerated<timeout_opts>;
  struct types_opts {
    enum options { integer, timeout, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  uint8_t&                                      integer()
  {
    assert_choice_type(types::integer, c.type, "OnDurationTimer");
    return c.get<uint8_t>();
  }
  timeout_e_& timeout()
  {
    assert_choice_type(types::timeout, c.type, "OnDurationTimer");
    return c.get<timeout_e_>();
  }
  const uint8_t& integer() const
  {
    assert_choice_type(types::integer, c.type, "OnDurationTimer");
    return c.get<uint8_t>();
  }
  const timeout_e_& timeout() const
  {
    assert_choice_type(types::timeout, c.type, "OnDurationTimer");
    return c.get<timeout_e_>();
  }
  uint8_t&    set_integer();
  timeout_e_& set_timeout();

private:
  asn1::table::choice_storage<on_dur_timer_c, types, choice_buffer_t<timeout_e_, uint8_t>> c;
};

// CycleStartOffset ::= SEQUENCE
struct cycle_start_offset_s {
  struct periodicity_opts {
    enum options {
      ms10,
      ms20,
      ms32,
      ms40,
      ms60,
      ms64,
      ms70,
      ms80,
      ms128,
      ms160,
      ms256,
      ms320,
      ms512,
      ms640,
      ms1024,
      ms1280,
      ms2048,
      ms2560,
      ms5120,
      ms10240,
      nulltype
    } value;

    static const asn1::table::enum_desc desc;
    uint16_t                            to_number() const;
  };
  using periodicity_e_ = enumerated<periodicity_opts>;

  // member variables
  bool           periodicity_present = false;
  bool           offset_present      = false;
  periodicity_e_ periodicity;
  uint16_t       offset = 0;

  static const asn1::table::seq_desc desc;
};

// O-CellDTXDRXConfig ::= SEQUENCE
struct o_cell_dtxdrx_cfg_s {
  struct cfg_type_opts {
    enum options { dtx, drx, dtxdrx, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using cfg_type_e_ = enumerated<cfg_type_opts>;
  struct activation_status_opts {
    enum options { activ, deactiv, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using activation_status_e_ = enumerated<activation_status_opts>;
  struct l1_activation_opts {
    enum options { enabled, disabled, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using l1_activation_e_ = enumerated<l1_activation_opts>;

  // member variables
  bool                 on_dur_timer_present       = false;
  bool                 cycle_start_offset_present = false;
  bool                 slot_offset_present        = false;
  bool                 cfg_type_present           = false;
  bool                 activation_status_present  = false;
  bool                 l1_activation_present      = false;
  on_dur_timer_c       on_dur_timer;
  cycle_start_offset_s cycle_start_offset;
  uint8_t              slot_offset = 0;
  cfg_type_e_          cfg_type;
  activation_status_e_ activation_status;
  l1_activation_e_     l1_activation;

  static const asn1::table::seq_desc desc;
};

// TPolarizationItem ::= SEQUENCE
struct t_polarization_item_s {
  struct tpolarization_type_opts {
    enum options { minus_neg45, zero, plus_neg45, plus_neg90, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using tpolarization_type_e_ = enumerated<tpolarization_type_opts>;

  // member variables
  uint16_t              tpolarization_idx = 0;
  tpolarization_type_e_ tpolarization_type;

  static const asn1::table::seq_desc desc;
};

// TxArrayItem ::= SEQUENCE
struct tx_array_item_s {
  struct tnormal_vector_direction_s_ {
    bool   tazimuth_angle_present = false;
    bool   tzenith_angle_present  = false;
    real_s tazimuth_angle;
    real_s tzenith_angle;

    static const asn1::table::seq_desc desc;
  };
  struct tleftmost_bottom_array_elem_position_s_ {
    bool   tx_axis_present = false;
    bool   ty_axis_present = false;
    bool   tz_axis_present = false;
    real_s tx_axis;
    real_s ty_axis;
    real_s tz_axis;

    static const asn1::table::seq_desc desc;
  };
  using tpolarization_list_l_ = dyn_array<t_polarization_item_s>;

  // member variables
  bool                                    thorizontal_spacing_present                  = false;
  bool                                    tvertical_spacing_present                    = false;
  bool                                    tnormal_vector_direction_present             = false;
  bool                                    tleftmost_bottom_array_elem_position_present = false;
  bool                                    tmin_gain_present                            = false;
  printable_string<1, 150, true, true>    tname;
  uint32_t                                tnof_rows         = 1;
  uint32_t                                tnof_columns      = 1;
  uint16_t                                tnof_array_layers = 1;
  real_s                                  thorizontal_spacing;
  real_s                                  tvertical_spacing;
  tnormal_vector_direction_s_             tnormal_vector_direction;
  tleftmost_bottom_array_elem_position_s_ tleftmost_bottom_array_elem_position;
  tpolarization_list_l_                   tpolarization_list;
  uint16_t                                tband_num = 1;
  real_s                                  tmax_gain;
  real_s                                  tmin_gain;
  bool                                    tindependent_pwr_budget = false;

  static const asn1::table::seq_desc desc;
};

// TxArrayList ::= SEQUENCE (SIZE (1..64)) OF TxArrayItem
using tx_array_list_l = dyn_array<tx_array_item_s>;

// RPolarizationItem ::= SEQUENCE
struct r_polarization_item_s {
  struct rpolarization_type_opts {
    enum options { minus_neg45, zero, plus_neg45, plus_neg90, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using rpolarization_type_e_ = enumerated<rpolarization_type_opts>;

  // member variables
  uint16_t              rpolarization_idx = 0;
  rpolarization_type_e_ rpolarization_type;

  static const asn1::table::seq_desc desc;
};

// RxArrayItem ::= SEQUENCE
struct rx_array_item_s {
  struct rnormal_vector_direction_s_ {
    bool   razimuth_angle_present = false;
    bool   rzenith_angle_present  = false;
    real_s razimuth_angle;
    real_s rzenith_angle;

    static const asn1::table::seq_desc desc;
  };
  struct rleftmost_bottom_array_elem_position_s_ {
    bool   rx_axis_present = false;
    bool   ry_axis_present = false;
    bool   rz_axis_present = false;
    real_s rx_axis;
    real_s ry_axis;
    real_s rz_axis;

    static const asn1::table::seq_desc desc;
  };
  using rpolarization_list_l_ = dyn_array<r_polarization_item_s>;
  struct rgain_correction_range_s_ {
    bool   rmax_present = false;
    bool   rmin_present = false;
    real_s rmax;
    real_s rmin;

    static const asn1::table::seq_desc desc;
  };

  // member variables
  bool                                    rhorizontal_spacing_present                  = false;
  bool                                    rvertical_spacing_present                    = false;
  bool                                    rnormal_vector_direction_present             = false;
  bool                                    rleftmost_bottom_array_elem_position_present = false;
  printable_string<1, 150, true, true>    rname;
  uint32_t                                rnof_rows         = 1;
  uint32_t                                rnof_columns      = 1;
  uint16_t                                rnof_array_layers = 1;
  real_s                                  rhorizontal_spacing;
  real_s                                  rvertical_spacing;
  rnormal_vector_direction_s_             rnormal_vector_direction;
  rleftmost_bottom_array_elem_position_s_ rleftmost_bottom_array_elem_position;
  rpolarization_list_l_                   rpolarization_list;
  uint16_t                                rband_num = 1;
  rgain_correction_range_s_               rgain_correction_range;

  static const asn1::table::seq_desc desc;
};

// RxArrayList ::= SEQUENCE (SIZE (1..64)) OF RxArrayItem
using rx_array_list_l = dyn_array<rx_array_item_s>;

// OruUserPlaneConfiguration ::= SEQUENCE
struct oru_user_plane_cfg_s {
  tx_array_list_l tx_array_list;
  rx_array_list_l rx_array_list;

  static const asn1::table::seq_desc desc;
};

// EnergySavingCapabilityCommonInfo ::= SEQUENCE
struct energy_saving_cap_common_info_s {
  bool st8_ready_msg_supported         = false;
  bool sleep_dur_ext_supported         = false;
  bool emergency_wake_up_cmd_supported = false;

  static const asn1::table::seq_desc desc;
};

// SupportedTrxControlMask ::= SEQUENCE
struct supported_trx_ctrl_mask_s {
  printable_string<1, 150, true, true> mask_name;
  int64_t                              ant_mask = 0;

  static const asn1::table::seq_desc desc;
};

// SupportedTrxControlMasks ::= SEQUENCE (SIZE (1..64)) OF SupportedTrxControlMask
using supported_trx_ctrl_masks_l = dyn_array<supported_trx_ctrl_mask_s>;

// TrxControlCapabilityInfo ::= SEQUENCE
struct trx_ctrl_cap_info_s {
  supported_trx_ctrl_masks_l supported_trx_ctrl_masks;
  sleep_modes_l              sleep_modes;
  bool                       defined_dur_sleep_supported   = false;
  bool                       undefined_dur_sleep_supported = false;

  static const asn1::table::seq_desc desc;
};

// DeepHibernateCapabilityInfo ::= SEQUENCE
struct deep_hibernate_cap_info_s {
  bool     min_hibernate_dur_present = false;
  uint64_t max_hibernate_dur         = 0;
  uint64_t min_hibernate_dur         = 0;

  static const asn1::table::seq_desc desc;
};

// OruCapabilities ::= SEQUENCE
struct oru_cap_s {
  bool                            energy_saving_cap_common_info_present = false;
  bool                            asm_cap_info_present                  = false;
  bool                            trx_ctrl_cap_info_present             = false;
  bool                            deep_hibernate_cap_info_present       = false;
  energy_saving_cap_common_info_s energy_saving_cap_common_info;
  asm_cap_info_s                  asm_cap_info;
  trx_ctrl_cap_info_s             trx_ctrl_cap_info;
  deep_hibernate_cap_info_s       deep_hibernate_cap_info;

  static const asn1::table::seq_desc desc;
};

// O-RUInfo ::= SEQUENCE
struct o_ru_info_s {
  bool                 oru_user_plane_cfg_present = false;
  bool                 oru_cap_present            = false;
  oru_user_plane_cfg_s oru_user_plane_cfg;
  oru_cap_s            oru_cap;

  static const asn1::table::seq_desc desc;
};

// PrbBlankingConfigItem ::= SEQUENCE
struct prb_blanking_cfg_item_s {
  bool                                  bwp_idx_present   = false;
  bool                                  frame_num_present = false;
  bool                                  slot_num_present  = false;
  bool                                  prb_idx_present   = false;
  uint8_t                               bwp_idx           = 0;
  printable_string<1, 1000, true, true> frame_num;
  printable_string<1, 1000, true, true> slot_num;
  printable_string<1, 1000, true, true> prb_idx;

  static const asn1::table::seq_desc desc;
};

// PrbBlankingConfigurationList ::= SEQUENCE (SIZE (1..64)) OF PrbBlankingConfigItem
using prb_blanking_cfg_list_l = dyn_array<prb_blanking_cfg_item_s>;

// O-PRBBlankingPolicy ::= SEQUENCE
struct o_prb_blanking_policy_s {
  struct policy_category_opts {
    enum options { prb_blanking_cfg, prb_blanking_objive, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using policy_category_e_ = enumerated<policy_category_opts>;
  struct link_dir_opts {
    enum options { dl, ul, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using link_dir_e_ = enumerated<link_dir_opts>;

  // member variables
  bool                     policy_category_present  = false;
  bool                     link_dir_present         = false;
  bool                     start_time_present       = false;
  bool                     end_time_present         = false;
  bool                     prb_blanking_obj_present = false;
  policy_category_e_       policy_category;
  link_dir_e_              link_dir;
  fixed_octstring<8, true> start_time;
  fixed_octstring<8, true> end_time;
  prb_blanking_cfg_list_l  prb_blanking_cfg_list;
  real_s                   prb_blanking_obj;

  static const asn1::table::seq_desc desc;
};

// E2SM-CCC-RAN-Configuration-Structure ::= CHOICE
struct e2sm_ccc_ran_cfg_structure_c {
  struct types_opts {
    enum options {
      o_gnb_cu_cp_function,
      o_gnb_cu_up_function,
      o_gnb_du_function,
      o_nr_cell_cu,
      o_nr_cell_du,
      o_rrm_policy_ratio,
      o_bwp,
      o_ces_management_function,
      o_nes_policy,
      o_cell_dtxdrx_cfg,
      o_ru_info,
      o_prb_blanking_policy,
      nulltype
    } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  o_gnb_cu_cp_function_s&                       o_gnb_cu_cp_function()
  {
    assert_choice_type(types::o_gnb_cu_cp_function, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_gnb_cu_cp_function_s>();
  }
  o_gnb_cu_up_function_s& o_gnb_cu_up_function()
  {
    assert_choice_type(types::o_gnb_cu_up_function, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_gnb_cu_up_function_s>();
  }
  o_gnb_du_function_s& o_gnb_du_function()
  {
    assert_choice_type(types::o_gnb_du_function, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_gnb_du_function_s>();
  }
  o_nr_cell_cu_s& o_nr_cell_cu()
  {
    assert_choice_type(types::o_nr_cell_cu, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_nr_cell_cu_s>();
  }
  o_nr_cell_du_s& o_nr_cell_du()
  {
    assert_choice_type(types::o_nr_cell_du, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_nr_cell_du_s>();
  }
  o_rrm_policy_ratio_s& o_rrm_policy_ratio()
  {
    assert_choice_type(types::o_rrm_policy_ratio, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_rrm_policy_ratio_s>();
  }
  o_bwp_s& o_bwp()
  {
    assert_choice_type(types::o_bwp, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_bwp_s>();
  }
  o_ces_management_function_s& o_ces_management_function()
  {
    assert_choice_type(types::o_ces_management_function, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_ces_management_function_s>();
  }
  o_nes_policy_s& o_nes_policy()
  {
    assert_choice_type(types::o_nes_policy, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_nes_policy_s>();
  }
  o_cell_dtxdrx_cfg_s& o_cell_dtxdrx_cfg()
  {
    assert_choice_type(types::o_cell_dtxdrx_cfg, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_cell_dtxdrx_cfg_s>();
  }
  o_ru_info_s& o_ru_info()
  {
    assert_choice_type(types::o_ru_info, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_ru_info_s>();
  }
  o_prb_blanking_policy_s& o_prb_blanking_policy()
  {
    assert_choice_type(types::o_prb_blanking_policy, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_prb_blanking_policy_s>();
  }
  const o_gnb_cu_cp_function_s& o_gnb_cu_cp_function() const
  {
    assert_choice_type(types::o_gnb_cu_cp_function, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_gnb_cu_cp_function_s>();
  }
  const o_gnb_cu_up_function_s& o_gnb_cu_up_function() const
  {
    assert_choice_type(types::o_gnb_cu_up_function, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_gnb_cu_up_function_s>();
  }
  const o_gnb_du_function_s& o_gnb_du_function() const
  {
    assert_choice_type(types::o_gnb_du_function, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_gnb_du_function_s>();
  }
  const o_nr_cell_cu_s& o_nr_cell_cu() const
  {
    assert_choice_type(types::o_nr_cell_cu, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_nr_cell_cu_s>();
  }
  const o_nr_cell_du_s& o_nr_cell_du() const
  {
    assert_choice_type(types::o_nr_cell_du, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_nr_cell_du_s>();
  }
  const o_rrm_policy_ratio_s& o_rrm_policy_ratio() const
  {
    assert_choice_type(types::o_rrm_policy_ratio, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_rrm_policy_ratio_s>();
  }
  const o_bwp_s& o_bwp() const
  {
    assert_choice_type(types::o_bwp, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_bwp_s>();
  }
  const o_ces_management_function_s& o_ces_management_function() const
  {
    assert_choice_type(types::o_ces_management_function, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_ces_management_function_s>();
  }
  const o_nes_policy_s& o_nes_policy() const
  {
    assert_choice_type(types::o_nes_policy, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_nes_policy_s>();
  }
  const o_cell_dtxdrx_cfg_s& o_cell_dtxdrx_cfg() const
  {
    assert_choice_type(types::o_cell_dtxdrx_cfg, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_cell_dtxdrx_cfg_s>();
  }
  const o_ru_info_s& o_ru_info() const
  {
    assert_choice_type(types::o_ru_info, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_ru_info_s>();
  }
  const o_prb_blanking_policy_s& o_prb_blanking_policy() const
  {
    assert_choice_type(types::o_prb_blanking_policy, c.type, "E2SM-CCC-RAN-Configuration-Structure");
    return c.get<o_prb_blanking_policy_s>();
  }
  o_gnb_cu_cp_function_s&      set_o_gnb_cu_cp_function();
  o_gnb_cu_up_function_s&      set_o_gnb_cu_up_function();
  o_gnb_du_function_s&         set_o_gnb_du_function();
  o_nr_cell_cu_s&              set_o_nr_cell_cu();
  o_nr_cell_du_s&              set_o_nr_cell_du();
  o_rrm_policy_ratio_s&        set_o_rrm_policy_ratio();
  o_bwp_s&                     set_o_bwp();
  o_ces_management_function_s& set_o_ces_management_function();
  o_nes_policy_s&              set_o_nes_policy();
  o_cell_dtxdrx_cfg_s&         set_o_cell_dtxdrx_cfg();
  o_ru_info_s&                 set_o_ru_info();
  o_prb_blanking_policy_s&     set_o_prb_blanking_policy();

private:
  asn1::table::choice_storage<e2sm_ccc_ran_cfg_structure_c,
                              types,
                              choice_buffer_t<o_bwp_s,
                                              o_cell_dtxdrx_cfg_s,
                                              o_ces_management_function_s,
                                              o_gnb_cu_cp_function_s,
                                              o_gnb_cu_up_function_s,
                                              o_gnb_du_function_s,
                                              o_nes_policy_s,
                                              o_nr_cell_cu_s,
                                              o_nr_cell_du_s,
                                              o_prb_blanking_policy_s,
                                              o_rrm_policy_ratio_s,
                                              o_ru_info_s>>
      c;
};

// ValuesOfAttributes ::= SEQUENCE
struct values_of_attributes_s {
  e2sm_ccc_ran_cfg_structure_c ran_cfg_structure;

  static const asn1::table::seq_desc desc;
};

// ConfigurationStructureAccepted ::= SEQUENCE
struct cfg_structure_accepted_s {
  bool                                 applied_timestamp_present = false;
  printable_string<1, 150, true, true> ran_cfg_structure_name;
  values_of_attributes_s               old_values_of_attributes;
  values_of_attributes_s               current_values_of_attributes;
  fixed_octstring<8, true>             applied_timestamp;

  static const asn1::table::seq_desc desc;
};

// RanConfigurationStructuresAcceptedList ::= SEQUENCE (SIZE (1..64)) OF ConfigurationStructureAccepted
using ran_cfg_structures_accepted_list_l = dyn_array<cfg_structure_accepted_s>;

// ConfigurationStructureFailed ::= SEQUENCE
struct cfg_structure_failed_s {
  printable_string<1, 150, true, true> ran_cfg_structure_name;
  values_of_attributes_s               old_values_of_attributes;
  values_of_attributes_s               requested_values_of_attributes;
  cause_e                              cause;

  static const asn1::table::seq_desc desc;
};

// RanConfigurationStructuresFailedList ::= SEQUENCE (SIZE (1..64)) OF ConfigurationStructureFailed
using ran_cfg_structures_failed_list_l = dyn_array<cfg_structure_failed_s>;

// CellControlOutcome ::= SEQUENCE
struct cell_ctrl_outcome_s {
  cell_global_id_c                   cell_global_id;
  ran_cfg_structures_accepted_list_l ran_cfg_structures_accepted_list;
  ran_cfg_structures_failed_list_l   ran_cfg_structures_failed_list;

  static const asn1::table::seq_desc desc;
};

// ConfigurationStructureWrite ::= SEQUENCE
struct cfg_structure_write_s {
  printable_string<1, 150, true, true> ran_cfg_structure_name;
  values_of_attributes_s               old_values_of_attributes;
  values_of_attributes_s               new_values_of_attributes;

  static const asn1::table::seq_desc desc;
};

// ListOfConfigurationStructures ::= SEQUENCE (SIZE (1..64)) OF ConfigurationStructureWrite
using list_of_cfg_structures_l = dyn_array<cfg_structure_write_s>;

// CellControlled ::= SEQUENCE
struct cell_ctrl_s {
  cell_global_id_c         cell_global_id;
  list_of_cfg_structures_l list_of_cfg_structures;

  static const asn1::table::seq_desc desc;
};

// ListOfSupportedAttributes ::= SEQUENCE (SIZE (1..64)) OF Attribute
using list_of_supported_attributes_l = dyn_array<attribute_s>;

// RANConfigurationStructure ::= SEQUENCE
struct ran_cfg_structure_s {
  printable_string<0, 150, true, true> ran_cfg_structure_name;
  list_of_supported_attributes_l       list_of_supported_attributes;

  static const asn1::table::seq_desc desc;
};

// ListOfSupportedRANConfigurationStructures ::= SEQUENCE (SIZE (1..64)) OF RANConfigurationStructure
using list_of_supported_ran_cfg_structures_l = dyn_array<ran_cfg_structure_s>;

// CellForRANFunctionDefinition ::= SEQUENCE
struct cell_for_ran_function_definition_s {
  cell_global_id_c                       cell_global_id;
  list_of_supported_ran_cfg_structures_l list_of_supported_cell_level_ran_cfg_structures;

  static const asn1::table::seq_desc desc;
};

// RANConfigurationStructureForEventTrigger ::= SEQUENCE
struct ran_cfg_structure_for_event_trigger_s {
  using list_of_attributes_l_ = dyn_array<printable_string<0, 150, true, true>>;

  // member variables
  printable_string<0, 150, true, true> ran_cfg_structure_name;
  list_of_attributes_l_                list_of_attributes;

  static const asn1::table::seq_desc desc;
};

// ListOfRANConfigurationStructuresForEventTrigger ::= SEQUENCE (SIZE (1..64)) OF
// RANConfigurationStructureForEventTrigger
using list_of_ran_cfg_structures_for_event_trigger_l = dyn_array<ran_cfg_structure_for_event_trigger_s>;

// CellLevelConfigurationStructureForEventTrigger ::= SEQUENCE
struct cell_level_cfg_structure_for_event_trigger_s {
  bool                                           cell_global_id_present = false;
  cell_global_id_c                               cell_global_id;
  list_of_ran_cfg_structures_for_event_trigger_l list_of_ran_cfg_structures_for_event_trigger;

  static const asn1::table::seq_desc desc;
};

// ConfigurationStructure ::= SEQUENCE
struct cfg_structure_s {
  struct change_type_opts {
    enum options { none, mod, addition, deletion, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using change_type_e_ = enumerated<change_type_opts>;

  // member variables
  bool                                 old_values_of_attributes_present = false;
  change_type_e_                       change_type;
  printable_string<1, 150, true, true> ran_cfg_structure_name;
  values_of_attributes_s               values_of_attributes;
  values_of_attributes_s               old_values_of_attributes;

  static const asn1::table::seq_desc desc;
};

// ListOfConfigurationsReported ::= SEQUENCE (SIZE (1..64)) OF ConfigurationStructure
using list_of_cfgs_reported_l = dyn_array<cfg_structure_s>;

// CellReported ::= SEQUENCE
struct cell_reported_s {
  cell_global_id_c        cell_global_id;
  list_of_cfgs_reported_l list_of_cfg_structures_reported;

  static const asn1::table::seq_desc desc;
};

// CelllevelConfigurationStructuresForQuery ::= SEQUENCE
struct celllevel_cfg_structures_for_query_s {
  using list_of_attributes_l_ = dyn_array<attribute_name_s>;

  // member variables
  printable_string<1, 150, true, true> ran_cfg_structure_name;
  list_of_attributes_l_                list_of_attributes;

  static const asn1::table::seq_desc desc;
};

// ListOfCelllevelConfigurationStructuresForQuery ::= SEQUENCE (SIZE (1..64)) OF
// CelllevelConfigurationStructuresForQuery
using list_of_celllevel_cfg_structures_for_query_l = dyn_array<celllevel_cfg_structures_for_query_s>;

// CellsToBeQueried ::= SEQUENCE
struct cells_to_be_queried_s {
  bool                                         cell_global_id_present = false;
  cell_global_id_c                             cell_global_id;
  list_of_celllevel_cfg_structures_for_query_l list_of_celllevel_cfg_structures_for_query;

  static const asn1::table::seq_desc desc;
};

// ConfigurationStructuresToBeReportedForQuery ::= SEQUENCE
struct cfg_structures_to_be_reported_for_query_s {
  printable_string<1, 150, true, true> ran_cfg_structure_name;
  values_of_attributes_s               values_of_attributes;

  static const asn1::table::seq_desc desc;
};

// ListOfConfigurationStructuresToBeReportedForQuery ::= SEQUENCE (SIZE (1..64)) OF
// ConfigurationStructuresToBeReportedForQuery
using list_of_cfg_structures_to_be_reported_for_query_l = dyn_array<cfg_structures_to_be_reported_for_query_s>;

// CellsToBeReportedForQuery ::= SEQUENCE
struct cells_to_be_reported_for_query_s {
  cell_global_id_c                                  cell_global_id;
  list_of_cfg_structures_to_be_reported_for_query_l list_of_cfg_structures_to_be_reported_for_query;

  static const asn1::table::seq_desc desc;
};

// E2SM-CCC-ControlHeaderFormat1 ::= SEQUENCE
struct e2sm_ccc_ctrl_hdr_format1_s {
  int64_t ric_style_type = 0;

  static const asn1::table::seq_desc desc;
};

// ControlHeaderFormat ::= CHOICE
struct ctrl_hdr_format_c {
  struct types_opts {
    enum options { ctrl_hdr_format1, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using types = enumerated<types_opts>;

  types                                 type() const { return types::ctrl_hdr_format1; }
  static const asn1::table::choice_desc desc;
  e2sm_ccc_ctrl_hdr_format1_s&          ctrl_hdr_format1() { return c; }
  const e2sm_ccc_ctrl_hdr_format1_s&    ctrl_hdr_format1() const { return c; }

private:
  e2sm_ccc_ctrl_hdr_format1_s c;
};

// E2SM-CCC-ControlMessageFormat1 ::= SEQUENCE
struct e2sm_ccc_ctrl_msg_format1_s {
  list_of_cfg_structures_l list_of_cfg_structures;

  static const asn1::table::seq_desc desc;
};

// ListOfCellsControlled ::= SEQUENCE (SIZE (1..64)) OF CellControlled
using list_of_cells_ctrl_l = dyn_array<cell_ctrl_s>;

// E2SM-CCC-ControlMessageFormat2 ::= SEQUENCE
struct e2sm_ccc_ctrl_msg_format2_s {
  list_of_cells_ctrl_l list_of_cells_ctrl;

  static const asn1::table::seq_desc desc;
};

// ControlMessageFormat ::= CHOICE
struct ctrl_msg_format_c {
  struct types_opts {
    enum options { ctrl_msg_format1, ctrl_msg_format2, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  e2sm_ccc_ctrl_msg_format1_s&                  ctrl_msg_format1()
  {
    assert_choice_type(types::ctrl_msg_format1, c.type, "ControlMessageFormat");
    return c.get<e2sm_ccc_ctrl_msg_format1_s>();
  }
  e2sm_ccc_ctrl_msg_format2_s& ctrl_msg_format2()
  {
    assert_choice_type(types::ctrl_msg_format2, c.type, "ControlMessageFormat");
    return c.get<e2sm_ccc_ctrl_msg_format2_s>();
  }
  const e2sm_ccc_ctrl_msg_format1_s& ctrl_msg_format1() const
  {
    assert_choice_type(types::ctrl_msg_format1, c.type, "ControlMessageFormat");
    return c.get<e2sm_ccc_ctrl_msg_format1_s>();
  }
  const e2sm_ccc_ctrl_msg_format2_s& ctrl_msg_format2() const
  {
    assert_choice_type(types::ctrl_msg_format2, c.type, "ControlMessageFormat");
    return c.get<e2sm_ccc_ctrl_msg_format2_s>();
  }
  e2sm_ccc_ctrl_msg_format1_s& set_ctrl_msg_format1();
  e2sm_ccc_ctrl_msg_format2_s& set_ctrl_msg_format2();

private:
  asn1::table::choice_storage<ctrl_msg_format_c,
                              types,
                              choice_buffer_t<e2sm_ccc_ctrl_msg_format1_s, e2sm_ccc_ctrl_msg_format2_s>>
      c;
};

// E2SM-CCC-ControlOutcomeFormat1 ::= SEQUENCE
struct e2sm_ccc_ctrl_outcome_format1_s {
  bool                               rx_timestamp_present = false;
  fixed_octstring<8, true>           rx_timestamp;
  ran_cfg_structures_accepted_list_l ran_cfg_structures_accepted_list;
  ran_cfg_structures_failed_list_l   ran_cfg_structures_failed_list;

  static const asn1::table::seq_desc desc;
};

// ListOfCellsForControlOutcome ::= SEQUENCE (SIZE (1..64)) OF CellControlOutcome
using list_of_cells_for_ctrl_outcome_l = dyn_array<cell_ctrl_outcome_s>;

// E2SM-CCC-ControlOutcomeFormat2 ::= SEQUENCE
struct e2sm_ccc_ctrl_outcome_format2_s {
  bool                             rx_timestamp_present = false;
  fixed_octstring<8, true>         rx_timestamp;
  list_of_cells_for_ctrl_outcome_l list_of_cells_for_ctrl_outcome;

  static const asn1::table::seq_desc desc;
};

// ControlOutcomeFormat ::= CHOICE
struct ctrl_outcome_format_c {
  struct types_opts {
    enum options { ctrl_outcome_format1, ctrl_outcome_format2, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  static const asn1::openapi::choice_desc       openapi;
  e2sm_ccc_ctrl_outcome_format1_s&              ctrl_outcome_format1()
  {
    assert_choice_type(types::ctrl_outcome_format1, c.type, "ControlOutcomeFormat");
    return c.get<e2sm_ccc_ctrl_outcome_format1_s>();
  }
  e2sm_ccc_ctrl_outcome_format2_s& ctrl_outcome_format2()
  {
    assert_choice_type(types::ctrl_outcome_format2, c.type, "ControlOutcomeFormat");
    return c.get<e2sm_ccc_ctrl_outcome_format2_s>();
  }
  const e2sm_ccc_ctrl_outcome_format1_s& ctrl_outcome_format1() const
  {
    assert_choice_type(types::ctrl_outcome_format1, c.type, "ControlOutcomeFormat");
    return c.get<e2sm_ccc_ctrl_outcome_format1_s>();
  }
  const e2sm_ccc_ctrl_outcome_format2_s& ctrl_outcome_format2() const
  {
    assert_choice_type(types::ctrl_outcome_format2, c.type, "ControlOutcomeFormat");
    return c.get<e2sm_ccc_ctrl_outcome_format2_s>();
  }
  e2sm_ccc_ctrl_outcome_format1_s& set_ctrl_outcome_format1();
  e2sm_ccc_ctrl_outcome_format2_s& set_ctrl_outcome_format2();

private:
  asn1::table::choice_storage<ctrl_outcome_format_c,
                              types,
                              choice_buffer_t<e2sm_ccc_ctrl_outcome_format1_s, e2sm_ccc_ctrl_outcome_format2_s>>
      c;
};

// E2SM-CCC-EventTriggerDefinition-Format1 ::= SEQUENCE
struct e2sm_ccc_event_trigger_definition_format1_s {
  list_of_ran_cfg_structures_for_event_trigger_l list_of_node_level_cfg_structures_for_event_trigger;

  static const asn1::table::seq_desc desc;
};

// ListOfCellLevelConfigurationStructuresForEventTrigger ::= SEQUENCE (SIZE (1..64)) OF
// CellLevelConfigurationStructureForEventTrigger
using list_of_cell_level_cfg_structures_for_event_trigger_l = dyn_array<cell_level_cfg_structure_for_event_trigger_s>;

// E2SM-CCC-EventTriggerDefinition-Format2 ::= SEQUENCE
struct e2sm_ccc_event_trigger_definition_format2_s {
  list_of_cell_level_cfg_structures_for_event_trigger_l list_of_cell_level_cfg_structures_for_event_trigger;

  static const asn1::table::seq_desc desc;
};

// E2SM-CCC-EventTriggerDefinition-Format3 ::= SEQUENCE
struct e2sm_ccc_event_trigger_definition_format3_s {
  int64_t period = 0;

  static const asn1::table::seq_desc desc;
};

// E2SM-CCC-IndicationHeaderFormat1 ::= SEQUENCE
struct e2sm_ccc_ind_hdr_format1_s {
  struct ind_reason_opts {
    enum options { upon_sub, upon_change, periodic, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using ind_reason_e_ = enumerated<ind_reason_opts>;

  // member variables
  ind_reason_e_            ind_reason;
  fixed_octstring<8, true> event_time;

  static const asn1::table::seq_desc desc;
};

// E2SM-CCC-IndicationMessageFormat1 ::= SEQUENCE
struct e2sm_ccc_ind_msg_format1_s {
  list_of_cfgs_reported_l list_of_cfg_structures_reported;

  static const asn1::table::seq_desc desc;
};

// ListOfCellsReported ::= SEQUENCE (SIZE (1..64)) OF CellReported
using list_of_cells_reported_l = dyn_array<cell_reported_s>;

// E2SM-CCC-IndicationMessageFormat2 ::= SEQUENCE
struct e2sm_ccc_ind_msg_format2_s {
  list_of_cells_reported_l list_of_cells_reported;

  static const asn1::table::seq_desc desc;
};

// NodelevelRanConfigurationStructuresForQuery ::= SEQUENCE
struct nodelevel_ran_cfg_structures_for_query_s {
  using list_of_attributes_l_ = dyn_array<attribute_name_s>;

  // member variables
  printable_string<1, 150, true, true> ran_cfg_structure_name;
  list_of_attributes_l_                list_of_attributes;

  static const asn1::table::seq_desc desc;
};

// ListOfNodelevelRanConfigurationStructuresForQuery ::= SEQUENCE (SIZE (1..64)) OF
// NodelevelRanConfigurationStructuresForQuery
using list_of_nodelevel_ran_cfg_structures_for_query_l = dyn_array<nodelevel_ran_cfg_structures_for_query_s>;

// E2SM-CCC-QueryDefinitionFormat1 ::= SEQUENCE
struct e2sm_ccc_query_definition_format1_s {
  list_of_nodelevel_ran_cfg_structures_for_query_l list_of_nodelevel_ran_cfg_structures_for_query;

  static const asn1::table::seq_desc desc;
};

// ListOfCellsToBeQueriedForQuery ::= SEQUENCE (SIZE (1..64)) OF CellsToBeQueried
using list_of_cells_to_be_queried_for_query_l = dyn_array<cells_to_be_queried_s>;

// E2SM-CCC-QueryDefinitionFormat2 ::= SEQUENCE
struct e2sm_ccc_query_definition_format2_s {
  list_of_cells_to_be_queried_for_query_l list_of_cells_to_be_queried_for_query;

  static const asn1::table::seq_desc desc;
};

// E2SM-CCC-QueryHeaderFormat1 ::= SEQUENCE
struct e2sm_ccc_query_hdr_format1_s {
  int64_t ric_style_type = 0;

  static const asn1::table::seq_desc desc;
};

// E2SM-CCC-QueryOutcomeFormat1 ::= SEQUENCE
struct e2sm_ccc_query_outcome_format1_s {
  list_of_cfg_structures_to_be_reported_for_query_l list_of_cfg_structures_to_be_reported_for_query;

  static const asn1::table::seq_desc desc;
};

// ListOfCellsToBeReportedForQuery ::= SEQUENCE (SIZE (1..64)) OF CellsToBeReportedForQuery
using list_of_cells_to_be_reported_for_query_l = dyn_array<cells_to_be_reported_for_query_s>;

// E2SM-CCC-QueryOutcomeFormat2 ::= SEQUENCE
struct e2sm_ccc_query_outcome_format2_s {
  list_of_cells_to_be_reported_for_query_l list_of_cells_to_be_reported_for_query;

  static const asn1::table::seq_desc desc;
};

// EventTriggerDefinitionFormat ::= CHOICE
struct event_trigger_definition_format_c {
  struct types_opts {
    enum options {
      event_trigger_definition_format1,
      event_trigger_definition_format2,
      event_trigger_definition_format3,
      nulltype
    } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  e2sm_ccc_event_trigger_definition_format1_s&  event_trigger_definition_format1()
  {
    assert_choice_type(types::event_trigger_definition_format1, c.type, "EventTriggerDefinitionFormat");
    return c.get<e2sm_ccc_event_trigger_definition_format1_s>();
  }
  e2sm_ccc_event_trigger_definition_format2_s& event_trigger_definition_format2()
  {
    assert_choice_type(types::event_trigger_definition_format2, c.type, "EventTriggerDefinitionFormat");
    return c.get<e2sm_ccc_event_trigger_definition_format2_s>();
  }
  e2sm_ccc_event_trigger_definition_format3_s& event_trigger_definition_format3()
  {
    assert_choice_type(types::event_trigger_definition_format3, c.type, "EventTriggerDefinitionFormat");
    return c.get<e2sm_ccc_event_trigger_definition_format3_s>();
  }
  const e2sm_ccc_event_trigger_definition_format1_s& event_trigger_definition_format1() const
  {
    assert_choice_type(types::event_trigger_definition_format1, c.type, "EventTriggerDefinitionFormat");
    return c.get<e2sm_ccc_event_trigger_definition_format1_s>();
  }
  const e2sm_ccc_event_trigger_definition_format2_s& event_trigger_definition_format2() const
  {
    assert_choice_type(types::event_trigger_definition_format2, c.type, "EventTriggerDefinitionFormat");
    return c.get<e2sm_ccc_event_trigger_definition_format2_s>();
  }
  const e2sm_ccc_event_trigger_definition_format3_s& event_trigger_definition_format3() const
  {
    assert_choice_type(types::event_trigger_definition_format3, c.type, "EventTriggerDefinitionFormat");
    return c.get<e2sm_ccc_event_trigger_definition_format3_s>();
  }
  e2sm_ccc_event_trigger_definition_format1_s& set_event_trigger_definition_format1();
  e2sm_ccc_event_trigger_definition_format2_s& set_event_trigger_definition_format2();
  e2sm_ccc_event_trigger_definition_format3_s& set_event_trigger_definition_format3();

private:
  asn1::table::choice_storage<event_trigger_definition_format_c,
                              types,
                              choice_buffer_t<e2sm_ccc_event_trigger_definition_format1_s,
                                              e2sm_ccc_event_trigger_definition_format2_s,
                                              e2sm_ccc_event_trigger_definition_format3_s>>
      c;
};

// IndicationHeaderFormat ::= CHOICE
struct ind_hdr_format_c {
  struct types_opts {
    enum options { ind_hdr_format1, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using types = enumerated<types_opts>;

  types                                 type() const { return types::ind_hdr_format1; }
  static const asn1::table::choice_desc desc;
  e2sm_ccc_ind_hdr_format1_s&           ind_hdr_format1() { return c; }
  const e2sm_ccc_ind_hdr_format1_s&     ind_hdr_format1() const { return c; }

private:
  e2sm_ccc_ind_hdr_format1_s c;
};

// IndicationMessageFormat ::= CHOICE
struct ind_msg_format_c {
  struct types_opts {
    enum options { ind_msg_format1, ind_msg_format2, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  e2sm_ccc_ind_msg_format1_s&                   ind_msg_format1()
  {
    assert_choice_type(types::ind_msg_format1, c.type, "IndicationMessageFormat");
    return c.get<e2sm_ccc_ind_msg_format1_s>();
  }
  e2sm_ccc_ind_msg_format2_s& ind_msg_format2()
  {
    assert_choice_type(types::ind_msg_format2, c.type, "IndicationMessageFormat");
    return c.get<e2sm_ccc_ind_msg_format2_s>();
  }
  const e2sm_ccc_ind_msg_format1_s& ind_msg_format1() const
  {
    assert_choice_type(types::ind_msg_format1, c.type, "IndicationMessageFormat");
    return c.get<e2sm_ccc_ind_msg_format1_s>();
  }
  const e2sm_ccc_ind_msg_format2_s& ind_msg_format2() const
  {
    assert_choice_type(types::ind_msg_format2, c.type, "IndicationMessageFormat");
    return c.get<e2sm_ccc_ind_msg_format2_s>();
  }
  e2sm_ccc_ind_msg_format1_s& set_ind_msg_format1();
  e2sm_ccc_ind_msg_format2_s& set_ind_msg_format2();

private:
  asn1::table::
      choice_storage<ind_msg_format_c, types, choice_buffer_t<e2sm_ccc_ind_msg_format1_s, e2sm_ccc_ind_msg_format2_s>>
          c;
};

// ListOfCellsForRANFunctionDefinition ::= SEQUENCE (SIZE (1..64)) OF CellForRANFunctionDefinition
using list_of_cells_for_ran_function_definition_l = dyn_array<cell_for_ran_function_definition_s>;

// PlmnIdList ::= SEQUENCE (SIZE (1..64)) OF PlmnId
using plmn_id_list_l = dyn_array<plmn_id_s>;

// QueryDefinitionFormat ::= CHOICE
struct query_definition_format_c {
  struct types_opts {
    enum options { query_definition_format1, query_definition_format2, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  e2sm_ccc_query_definition_format1_s&          query_definition_format1()
  {
    assert_choice_type(types::query_definition_format1, c.type, "QueryDefinitionFormat");
    return c.get<e2sm_ccc_query_definition_format1_s>();
  }
  e2sm_ccc_query_definition_format2_s& query_definition_format2()
  {
    assert_choice_type(types::query_definition_format2, c.type, "QueryDefinitionFormat");
    return c.get<e2sm_ccc_query_definition_format2_s>();
  }
  const e2sm_ccc_query_definition_format1_s& query_definition_format1() const
  {
    assert_choice_type(types::query_definition_format1, c.type, "QueryDefinitionFormat");
    return c.get<e2sm_ccc_query_definition_format1_s>();
  }
  const e2sm_ccc_query_definition_format2_s& query_definition_format2() const
  {
    assert_choice_type(types::query_definition_format2, c.type, "QueryDefinitionFormat");
    return c.get<e2sm_ccc_query_definition_format2_s>();
  }
  e2sm_ccc_query_definition_format1_s& set_query_definition_format1();
  e2sm_ccc_query_definition_format2_s& set_query_definition_format2();

private:
  asn1::table::choice_storage<query_definition_format_c,
                              types,
                              choice_buffer_t<e2sm_ccc_query_definition_format1_s, e2sm_ccc_query_definition_format2_s>>
      c;
};

// QueryHeaderFormat ::= CHOICE
struct query_hdr_format_c {
  struct types_opts {
    enum options { query_hdr_format1, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using types = enumerated<types_opts>;

  types                                 type() const { return types::query_hdr_format1; }
  static const asn1::table::choice_desc desc;
  e2sm_ccc_query_hdr_format1_s&         query_hdr_format1() { return c; }
  const e2sm_ccc_query_hdr_format1_s&   query_hdr_format1() const { return c; }

private:
  e2sm_ccc_query_hdr_format1_s c;
};

// QueryOutcomeFormat ::= CHOICE
struct query_outcome_format_c {
  struct types_opts {
    enum options { query_outcome_format1, query_outcome_format2, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  e2sm_ccc_query_outcome_format1_s&             query_outcome_format1()
  {
    assert_choice_type(types::query_outcome_format1, c.type, "QueryOutcomeFormat");
    return c.get<e2sm_ccc_query_outcome_format1_s>();
  }
  e2sm_ccc_query_outcome_format2_s& query_outcome_format2()
  {
    assert_choice_type(types::query_outcome_format2, c.type, "QueryOutcomeFormat");
    return c.get<e2sm_ccc_query_outcome_format2_s>();
  }
  const e2sm_ccc_query_outcome_format1_s& query_outcome_format1() const
  {
    assert_choice_type(types::query_outcome_format1, c.type, "QueryOutcomeFormat");
    return c.get<e2sm_ccc_query_outcome_format1_s>();
  }
  const e2sm_ccc_query_outcome_format2_s& query_outcome_format2() const
  {
    assert_choice_type(types::query_outcome_format2, c.type, "QueryOutcomeFormat");
    return c.get<e2sm_ccc_query_outcome_format2_s>();
  }
  e2sm_ccc_query_outcome_format1_s& set_query_outcome_format1();
  e2sm_ccc_query_outcome_format2_s& set_query_outcome_format2();

private:
  asn1::table::choice_storage<query_outcome_format_c,
                              types,
                              choice_buffer_t<e2sm_ccc_query_outcome_format1_s, e2sm_ccc_query_outcome_format2_s>>
      c;
};

// RANFunctionName ::= SEQUENCE
struct ran_function_name_s {
  bool                                  ran_function_instance_present = false;
  printable_string<0, 150, true, true>  ran_function_short_name;
  printable_string<1, 1000, true, true> ran_function_service_model_o_id;
  printable_string<1, 150, true, true>  ran_function_description;
  int64_t                               ran_function_instance = 0;

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition ::= SEQUENCE
struct ran_function_definition_s {
  ran_function_name_s                         ran_function_name;
  list_of_supported_ran_cfg_structures_l      list_of_supported_node_level_cfg_structures;
  list_of_cells_for_ran_function_definition_l list_of_cells_for_ran_function_definition;

  static const asn1::table::seq_desc   desc;
  static const asn1::openapi::seq_desc openapi;
};

// RIC-Control-Header ::= SEQUENCE
struct ric_ctrl_hdr_s {
  ctrl_hdr_format_c ctrl_hdr_format;

  static const asn1::table::seq_desc   desc;
  static const asn1::openapi::seq_desc openapi;
};

// RIC-Control-Message ::= SEQUENCE
struct ric_ctrl_msg_s {
  ctrl_msg_format_c ctrl_msg_format;

  static const asn1::table::seq_desc   desc;
  static const asn1::openapi::seq_desc openapi;
};

// RIC-Control-Outcome ::= SEQUENCE
struct ric_ctrl_outcome_s {
  ctrl_outcome_format_c ctrl_outcome_format;

  static const asn1::table::seq_desc   desc;
  static const asn1::openapi::seq_desc openapi;
};

// RIC-Indication-Header ::= SEQUENCE
struct ric_ind_hdr_s {
  ind_hdr_format_c ind_hdr_format;

  static const asn1::table::seq_desc   desc;
  static const asn1::openapi::seq_desc openapi;
};

// RIC-Indication-Message ::= SEQUENCE
struct ric_ind_msg_s {
  ind_msg_format_c ind_msg_format;

  static const asn1::table::seq_desc   desc;
  static const asn1::openapi::seq_desc openapi;
};

// RIC-Query-Definition ::= SEQUENCE
struct ric_query_definition_s {
  query_definition_format_c query_definition_format;

  static const asn1::table::seq_desc   desc;
  static const asn1::openapi::seq_desc openapi;
};

// RIC-Query-Header ::= SEQUENCE
struct ric_query_hdr_s {
  query_hdr_format_c query_hdr_format;

  static const asn1::table::seq_desc   desc;
  static const asn1::openapi::seq_desc openapi;
};

// RIC-Query-Outcome ::= SEQUENCE
struct ric_query_outcome_s {
  query_outcome_format_c query_outcome_format;

  static const asn1::table::seq_desc   desc;
  static const asn1::openapi::seq_desc openapi;
};

// RICActionDefinition ::= SEQUENCE
struct ric_action_definition_s {
  int64_t                    ric_style_type = 0;
  action_definition_format_c action_definition_format;

  static const asn1::table::seq_desc   desc;
  static const asn1::openapi::seq_desc openapi;
};

// RICEventTriggerDefinition ::= SEQUENCE
struct ric_event_trigger_definition_s {
  event_trigger_definition_format_c event_trigger_definition_format;

  static const asn1::table::seq_desc   desc;
  static const asn1::openapi::seq_desc openapi;
};

// SnssaiList ::= SEQUENCE (SIZE (1..64)) OF Snssai
using snssai_list_l = dyn_array<snssai_s>;

} // namespace e2sm_ccc
} // namespace asn1

extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::ran_cfg_structure_for_adf_s::list_of_attributes_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_ran_cfg_structures_for_adf_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cell_cfgs_to_be_reported_for_adf_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::sleep_modes_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_supported_event_trigger_styles_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<
    asn1::e2sm_ccc::report_style_s::list_of_supported_event_trigger_styles_for_report_style_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_supported_report_styles_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_supported_ctrl_styles_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_supported_query_styles_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::plmn_info_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::o_nr_cell_du_s::bwp_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::partition_flow_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::partition_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::rrm_policy_member_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::perf_objive_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::tx_array_item_s::tpolarization_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::tx_array_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::rx_array_item_s::rpolarization_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::rx_array_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::supported_trx_ctrl_masks_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::prb_blanking_cfg_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::ran_cfg_structures_accepted_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::ran_cfg_structures_failed_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cfg_structures_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_supported_attributes_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_supported_ran_cfg_structures_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_ran_cfg_structures_for_event_trigger_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cfgs_reported_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_celllevel_cfg_structures_for_query_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cfg_structures_to_be_reported_for_query_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cells_ctrl_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cells_for_ctrl_outcome_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cell_level_cfg_structures_for_event_trigger_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cells_reported_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_nodelevel_ran_cfg_structures_for_query_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cells_to_be_queried_for_query_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cells_to_be_reported_for_query_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm_ccc::list_of_cells_for_ran_function_definition_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::plmn_id_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm_ccc::snssai_list_l::value_type>::ops;
