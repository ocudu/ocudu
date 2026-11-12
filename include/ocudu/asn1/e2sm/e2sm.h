// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/*******************************************************************************
 *
 *                      3GPP TS ASN1 E2SM v07.00 (2025-10)
 *
 ******************************************************************************/

#pragma once

#include "ocudu/asn1/e2sm/e2sm_common_ies.h"

namespace asn1 {
namespace e2sm {

/*******************************************************************************
 *                         Struct Forward Declarations
 ******************************************************************************/

struct ran_param_testing_item_choice_list_s;
struct ran_param_testing_item_choice_structure_s;
struct ran_param_testing_item_choice_elem_true_s;
struct ran_param_testing_item_choice_elem_false_s;
struct ran_param_definition_choice_list_s;
struct ran_param_definition_choice_structure_s;
struct ran_param_value_type_choice_structure_s;
struct ran_param_value_type_choice_list_s;

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// AdditionalSupportedFormat ::= SEQUENCE
struct add_supported_format_s {
  int64_t ric_ctrl_hdr_format_type = 0;
  int64_t ric_ctrl_msg_format_type = 0;
  // ...
  bool     ric_ctrl_outcome_format_type_present = false;
  bool     ric_ctrl_action_id_present           = false;
  int64_t  ric_ctrl_outcome_format_type         = 0;
  uint32_t ric_ctrl_action_id                   = 1;

  static const asn1::table::seq_desc desc;
};

// RANParameter-Testing-Item ::= SEQUENCE
struct ran_param_testing_item_s {
  struct ran_param_type_c_ {
    struct types_opts {
      enum options {
        ran_p_choice_list,
        ran_p_choice_structure,
        ran_p_choice_elem_true,
        ran_p_choice_elem_false,
        // ...
        nulltype
      } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts, true>;

    void                                  set(types::options e = types::nulltype);
    types                                 type() const { return type_; }
    static const asn1::table::choice_desc desc;
    ran_param_testing_item_choice_list_s& ran_p_choice_list()
    {
      assert_choice_type(types::ran_p_choice_list, type_, "ranParameter-Type");
      return c.get<ran_param_testing_item_choice_list_s>();
    }
    ran_param_testing_item_choice_structure_s& ran_p_choice_structure()
    {
      assert_choice_type(types::ran_p_choice_structure, type_, "ranParameter-Type");
      return c.get<ran_param_testing_item_choice_structure_s>();
    }
    ran_param_testing_item_choice_elem_true_s& ran_p_choice_elem_true()
    {
      assert_choice_type(types::ran_p_choice_elem_true, type_, "ranParameter-Type");
      return c.get<ran_param_testing_item_choice_elem_true_s>();
    }
    ran_param_testing_item_choice_elem_false_s& ran_p_choice_elem_false()
    {
      assert_choice_type(types::ran_p_choice_elem_false, type_, "ranParameter-Type");
      return c.get<ran_param_testing_item_choice_elem_false_s>();
    }
    const ran_param_testing_item_choice_list_s& ran_p_choice_list() const
    {
      assert_choice_type(types::ran_p_choice_list, type_, "ranParameter-Type");
      return c.get<ran_param_testing_item_choice_list_s>();
    }
    const ran_param_testing_item_choice_structure_s& ran_p_choice_structure() const
    {
      assert_choice_type(types::ran_p_choice_structure, type_, "ranParameter-Type");
      return c.get<ran_param_testing_item_choice_structure_s>();
    }
    const ran_param_testing_item_choice_elem_true_s& ran_p_choice_elem_true() const
    {
      assert_choice_type(types::ran_p_choice_elem_true, type_, "ranParameter-Type");
      return c.get<ran_param_testing_item_choice_elem_true_s>();
    }
    const ran_param_testing_item_choice_elem_false_s& ran_p_choice_elem_false() const
    {
      assert_choice_type(types::ran_p_choice_elem_false, type_, "ranParameter-Type");
      return c.get<ran_param_testing_item_choice_elem_false_s>();
    }
    ran_param_testing_item_choice_list_s&       set_ran_p_choice_list();
    ran_param_testing_item_choice_structure_s&  set_ran_p_choice_structure();
    ran_param_testing_item_choice_elem_true_s&  set_ran_p_choice_elem_true();
    ran_param_testing_item_choice_elem_false_s& set_ran_p_choice_elem_false();

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // member variables
  uint64_t          ran_param_id = 1;
  ran_param_type_c_ ran_param_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-Testing-LIST ::= SEQUENCE (SIZE (1..65535)) OF RANParameter-Testing-Item
using ran_param_testing_list_l = dyn_array<ran_param_testing_item_s>;

// RANParameter-Testing-Item-Choice-List ::= SEQUENCE
struct ran_param_testing_item_choice_list_s {
  ran_param_testing_list_l ran_param_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-Testing-STRUCTURE ::= SEQUENCE (SIZE (1..65535)) OF RANParameter-Testing-Item
using ran_param_testing_structure_l = dyn_array<ran_param_testing_item_s>;

// RANParameter-Testing-Item-Choice-Structure ::= SEQUENCE
struct ran_param_testing_item_choice_structure_s {
  ran_param_testing_structure_l ran_param_structure;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-Value ::= CHOICE
struct ran_param_value_c {
  struct types_opts {
    enum options {
      value_boolean,
      value_int,
      value_real,
      value_bit_s,
      value_oct_s,
      value_printable_string,
      /*...*/ nulltype
    } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  bool&                                         value_boolean()
  {
    assert_choice_type(types::value_boolean, c.type, "RANParameter-Value");
    return c.get<bool>();
  }
  int64_t& value_int()
  {
    assert_choice_type(types::value_int, c.type, "RANParameter-Value");
    return c.get<int64_t>();
  }
  real_s& value_real()
  {
    assert_choice_type(types::value_real, c.type, "RANParameter-Value");
    return c.get<real_s>();
  }
  unbounded_bitstring<false, true>& value_bit_s()
  {
    assert_choice_type(types::value_bit_s, c.type, "RANParameter-Value");
    return c.get<unbounded_bitstring<false, true>>();
  }
  unbounded_octstring<true>& value_oct_s()
  {
    assert_choice_type(types::value_oct_s, c.type, "RANParameter-Value");
    return c.get<unbounded_octstring<true>>();
  }
  printable_string<0, MAX_ASN_STRING_LENGTH, false, true>& value_printable_string()
  {
    assert_choice_type(types::value_printable_string, c.type, "RANParameter-Value");
    return c.get<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>();
  }
  const bool& value_boolean() const
  {
    assert_choice_type(types::value_boolean, c.type, "RANParameter-Value");
    return c.get<bool>();
  }
  const int64_t& value_int() const
  {
    assert_choice_type(types::value_int, c.type, "RANParameter-Value");
    return c.get<int64_t>();
  }
  const real_s& value_real() const
  {
    assert_choice_type(types::value_real, c.type, "RANParameter-Value");
    return c.get<real_s>();
  }
  const unbounded_bitstring<false, true>& value_bit_s() const
  {
    assert_choice_type(types::value_bit_s, c.type, "RANParameter-Value");
    return c.get<unbounded_bitstring<false, true>>();
  }
  const unbounded_octstring<true>& value_oct_s() const
  {
    assert_choice_type(types::value_oct_s, c.type, "RANParameter-Value");
    return c.get<unbounded_octstring<true>>();
  }
  const printable_string<0, MAX_ASN_STRING_LENGTH, false, true>& value_printable_string() const
  {
    assert_choice_type(types::value_printable_string, c.type, "RANParameter-Value");
    return c.get<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>();
  }
  bool&                                                    set_value_boolean();
  int64_t&                                                 set_value_int();
  real_s&                                                  set_value_real();
  unbounded_bitstring<false, true>&                        set_value_bit_s();
  unbounded_octstring<true>&                               set_value_oct_s();
  printable_string<0, MAX_ASN_STRING_LENGTH, false, true>& set_value_printable_string();

private:
  asn1::table::choice_storage<ran_param_value_c,
                              types,
                              choice_buffer_t<bool,
                                              int64_t,
                                              printable_string<0, MAX_ASN_STRING_LENGTH, false, true>,
                                              real_s,
                                              unbounded_bitstring<false, true>,
                                              unbounded_octstring<true>>>
      c;
};

// RANParameter-Testing-Item-Choice-ElementTrue ::= SEQUENCE
struct ran_param_testing_item_choice_elem_true_s {
  ran_param_value_c ran_param_value;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-TestingCondition ::= CHOICE
struct ran_param_testing_condition_c {
  struct ran_p_choice_comparison_opts {
    enum options { equal, difference, greaterthan, lessthan, contains, starts_with, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using ran_p_choice_comparison_e_ = enumerated<ran_p_choice_comparison_opts, true>;
  struct ran_p_choice_presence_opts {
    enum options { present, cfg, rollover, non_zero, /*...*/ value_change, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using ran_p_choice_presence_e_ = enumerated<ran_p_choice_presence_opts, true, 1>;
  struct types_opts {
    enum options { ran_p_choice_comparison, ran_p_choice_presence, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  ran_p_choice_comparison_e_&                   ran_p_choice_comparison()
  {
    assert_choice_type(types::ran_p_choice_comparison, c.type, "RANParameter-TestingCondition");
    return c.get<ran_p_choice_comparison_e_>();
  }
  ran_p_choice_presence_e_& ran_p_choice_presence()
  {
    assert_choice_type(types::ran_p_choice_presence, c.type, "RANParameter-TestingCondition");
    return c.get<ran_p_choice_presence_e_>();
  }
  const ran_p_choice_comparison_e_& ran_p_choice_comparison() const
  {
    assert_choice_type(types::ran_p_choice_comparison, c.type, "RANParameter-TestingCondition");
    return c.get<ran_p_choice_comparison_e_>();
  }
  const ran_p_choice_presence_e_& ran_p_choice_presence() const
  {
    assert_choice_type(types::ran_p_choice_presence, c.type, "RANParameter-TestingCondition");
    return c.get<ran_p_choice_presence_e_>();
  }
  ran_p_choice_comparison_e_& set_ran_p_choice_comparison();
  ran_p_choice_presence_e_&   set_ran_p_choice_presence();

private:
  asn1::table::choice_storage<ran_param_testing_condition_c,
                              types,
                              choice_buffer_t<ran_p_choice_comparison_e_, ran_p_choice_presence_e_>>
      c;
};

// LogicalOR-E2SM-RC-IEs ::= ENUMERATED
struct lc_or_e2sm_rc_ies_opts {
  enum options { true_value, false_value, /*...*/ nulltype } value;

  static const asn1::table::enum_desc desc;
};
using lc_or_e2sm_rc_ies_e = enumerated<lc_or_e2sm_rc_ies_opts, true>;

// RANParameter-Testing-Item-Choice-ElementFalse ::= SEQUENCE
struct ran_param_testing_item_choice_elem_false_s {
  bool                          ran_param_value_present = false;
  bool                          lc_or_present           = false;
  ran_param_testing_condition_c ran_param_test_condition;
  ran_param_value_c             ran_param_value;
  lc_or_e2sm_rc_ies_e           lc_or;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-Testing ::= SEQUENCE (SIZE (1..255)) OF RANParameter-Testing-Item
using ran_param_testing_l = dyn_array<ran_param_testing_item_s>;

// EventTrigger-UE-Info-Item-Choice-Individual ::= SEQUENCE
struct event_trigger_ue_info_item_choice_individual_s {
  ue_id_c             ue_id;
  ran_param_testing_l ran_param_testing;
  // ...

  static const asn1::table::seq_desc desc;
};

// EventTrigger-UE-Info-Item-Choice-Group ::= SEQUENCE
struct event_trigger_ue_info_item_choice_group_s {
  ran_param_testing_l ran_param_testing;
  // ...

  static const asn1::table::seq_desc desc;
};

// UEQuery ::= SEQUENCE
struct ue_query_s {
  partial_ue_id_s     partial_ue_id;
  ran_param_testing_l ran_param_testing;
  // ...

  static const asn1::table::seq_desc desc;
};

// Associated-UE-Info-Item ::= SEQUENCE
struct associated_ue_info_item_s {
  struct ue_type_c_ {
    struct types_opts {
      enum options { ue_type_choice_individual, ue_type_choice_group, /*...*/ ue_query, nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts, true, 1>;

    static const asn1::table::choice_storage_desc   storage_desc;
    void                                            set(types::options e = types::nulltype) { c.set(e); }
    types                                           type() const { return c.type; }
    static const asn1::table::choice_desc           desc;
    event_trigger_ue_info_item_choice_individual_s& ue_type_choice_individual()
    {
      assert_choice_type(types::ue_type_choice_individual, c.type, "ueType");
      return c.get<event_trigger_ue_info_item_choice_individual_s>();
    }
    event_trigger_ue_info_item_choice_group_s& ue_type_choice_group()
    {
      assert_choice_type(types::ue_type_choice_group, c.type, "ueType");
      return c.get<event_trigger_ue_info_item_choice_group_s>();
    }
    ue_query_s& ue_query()
    {
      assert_choice_type(types::ue_query, c.type, "ueType");
      return c.get<ue_query_s>();
    }
    const event_trigger_ue_info_item_choice_individual_s& ue_type_choice_individual() const
    {
      assert_choice_type(types::ue_type_choice_individual, c.type, "ueType");
      return c.get<event_trigger_ue_info_item_choice_individual_s>();
    }
    const event_trigger_ue_info_item_choice_group_s& ue_type_choice_group() const
    {
      assert_choice_type(types::ue_type_choice_group, c.type, "ueType");
      return c.get<event_trigger_ue_info_item_choice_group_s>();
    }
    const ue_query_s& ue_query() const
    {
      assert_choice_type(types::ue_query, c.type, "ueType");
      return c.get<ue_query_s>();
    }
    event_trigger_ue_info_item_choice_individual_s& set_ue_type_choice_individual();
    event_trigger_ue_info_item_choice_group_s&      set_ue_type_choice_group();
    ue_query_s&                                     set_ue_query();

  private:
    asn1::table::choice_storage<ue_type_c_,
                                types,
                                choice_buffer_t<event_trigger_ue_info_item_choice_group_s,
                                                event_trigger_ue_info_item_choice_individual_s,
                                                ue_query_s>>
        c;
  };

  // member variables
  bool                lc_or_present = false;
  uint32_t            ue_filt_id    = 1;
  ue_type_c_          ue_type;
  lc_or_e2sm_rc_ies_e lc_or;
  // ...

  static const asn1::table::seq_desc desc;
};

// Associated-UE-Info ::= SEQUENCE
struct associated_ue_info_s {
  using associated_ue_info_list_l_ = dyn_array<associated_ue_info_item_s>;

  // member variables
  associated_ue_info_list_l_ associated_ue_info_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-Definition-Choice ::= CHOICE
struct ran_param_definition_choice_c {
  struct types_opts {
    enum options { choice_list, choice_structure, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  void                                  set(types::options e = types::nulltype);
  types                                 type() const { return type_; }
  static const asn1::table::choice_desc desc;
  ran_param_definition_choice_list_s&   choice_list()
  {
    assert_choice_type(types::choice_list, type_, "RANParameter-Definition-Choice");
    return c.get<ran_param_definition_choice_list_s>();
  }
  ran_param_definition_choice_structure_s& choice_structure()
  {
    assert_choice_type(types::choice_structure, type_, "RANParameter-Definition-Choice");
    return c.get<ran_param_definition_choice_structure_s>();
  }
  const ran_param_definition_choice_list_s& choice_list() const
  {
    assert_choice_type(types::choice_list, type_, "RANParameter-Definition-Choice");
    return c.get<ran_param_definition_choice_list_s>();
  }
  const ran_param_definition_choice_structure_s& choice_structure() const
  {
    assert_choice_type(types::choice_structure, type_, "RANParameter-Definition-Choice");
    return c.get<ran_param_definition_choice_structure_s>();
  }
  ran_param_definition_choice_list_s&      set_choice_list();
  ran_param_definition_choice_structure_s& set_choice_structure();

private:
  types             type_;
  choice_buffer_ptr c;
};

// RANParameter-Definition ::= SEQUENCE
struct ran_param_definition_s {
  ran_param_definition_choice_c ran_param_definition_choice;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-Definition-Choice-LIST-Item ::= SEQUENCE
struct ran_param_definition_choice_list_item_s {
  bool                                 ran_param_definition_present = false;
  uint64_t                             ran_param_id                 = 1;
  printable_string<1, 150, true, true> ran_param_name;
  ran_param_definition_s               ran_param_definition;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-Definition-Choice-LIST ::= SEQUENCE
struct ran_param_definition_choice_list_s {
  using ran_param_list_l_ = dyn_array<ran_param_definition_choice_list_item_s>;

  // member variables
  ran_param_list_l_ ran_param_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-Definition-Choice-STRUCTURE-Item ::= SEQUENCE
struct ran_param_definition_choice_structure_item_s {
  bool                                 ran_param_definition_present = false;
  uint64_t                             ran_param_id                 = 1;
  printable_string<1, 150, true, true> ran_param_name;
  ran_param_definition_s               ran_param_definition;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-Definition-Choice-STRUCTURE ::= SEQUENCE
struct ran_param_definition_choice_structure_s {
  using ran_param_structure_l_ = dyn_array<ran_param_definition_choice_structure_item_s>;

  // member variables
  ran_param_structure_l_ ran_param_structure;
  // ...

  static const asn1::table::seq_desc desc;
};

// CallProcessBreakpoint-RANParameter-Item ::= SEQUENCE
struct call_process_breakpoint_ran_param_item_s {
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  static const asn1::table::seq_desc desc;
};

// CauseProtocol ::= ENUMERATED
struct cause_protocol_opts {
  enum options {
    transfer_syntax_error,
    abstract_syntax_error_reject,
    abstract_syntax_error_ignore_and_notify,
    msg_not_compatible_with_receiver_state,
    semantic_error,
    abstract_syntax_error_falsely_constructed_msg,
    unspecified,
    // ...
    nulltype
  } value;

  static const asn1::table::enum_desc desc;
};
using cause_protocol_e = enumerated<cause_protocol_opts, true>;

// CauseRICservice ::= ENUMERATED
struct cause_ri_cservice_opts {
  enum options {
    ric_event_trigger_style_type_not_supported,
    ric_event_trigger_format_type_not_supported,
    ran_param_id_not_supported,
    call_process_type_id_not_supported,
    call_process_breakpoint_id_not_supported,
    ric_report_style_type_not_supported,
    supported_ric_event_trigger_style_type_not_supported,
    ric_action_format_type_not_supported,
    ric_ind_hdr_format_type_not_supported,
    ric_ind_msg_format_type_not_supported,
    ric_insert_style_type_not_supported,
    insert_ind_id_not_supported,
    ric_call_process_id_format_type_not_supported,
    ric_ctrl_style_type_not_supported,
    ctrl_action_id_not_supported,
    ric_ctrl_hdr_format_type_not_supported,
    ric_ctrl_msg_format_type_not_supported,
    ric_ctrl_outcome_format_type_not_supported,
    ric_policy_style_type_not_supported,
    policy_action_id_not_supported,
    ric_query_style_type_not_supported,
    ric_query_hdr_format_type_not_supported,
    ric_query_definition_format_type_not_supported,
    ric_query_outcome_format_type_not_supported,
    // ...
    nulltype
  } value;

  static const asn1::table::enum_desc desc;
};
using cause_ri_cservice_e = enumerated<cause_ri_cservice_opts, true>;

// CellIdentification-RANParameter-Item ::= SEQUENCE
struct cell_identif_ran_param_item_s {
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  static const asn1::table::seq_desc desc;
};

// ControlAction-RANParameter-Item ::= SEQUENCE
struct ctrl_action_ran_param_item_s {
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  static const asn1::table::seq_desc desc;
};

// ControlOutcome-RANParameter-Item ::= SEQUENCE
struct ctrl_outcome_ran_param_item_s {
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ActionDefinition-Format1-Item ::= SEQUENCE
struct e2sm_rc_action_definition_format1_item_s {
  uint64_t ran_param_id = 1;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ActionDefinition-Format1 ::= SEQUENCE
struct e2sm_rc_action_definition_format1_s {
  using ran_p_to_be_reported_list_l_ = dyn_array<e2sm_rc_action_definition_format1_item_s>;

  // member variables
  ran_p_to_be_reported_list_l_ ran_p_to_be_reported_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-ValueType-Choice-ElementTrue ::= SEQUENCE
struct ran_param_value_type_choice_elem_true_s {
  ran_param_value_c ran_param_value;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-ValueType-Choice-ElementFalse ::= SEQUENCE
struct ran_param_value_type_choice_elem_false_s {
  bool              ran_param_value_present = false;
  ran_param_value_c ran_param_value;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-ValueType ::= CHOICE
struct ran_param_value_type_c {
  struct types_opts {
    enum options {
      ran_p_choice_elem_true,
      ran_p_choice_elem_false,
      ran_p_choice_structure,
      ran_p_choice_list,
      // ...
      nulltype
    } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  void                                     set(types::options e = types::nulltype);
  types                                    type() const { return type_; }
  static const asn1::table::choice_desc    desc;
  ran_param_value_type_choice_elem_true_s& ran_p_choice_elem_true()
  {
    assert_choice_type(types::ran_p_choice_elem_true, type_, "RANParameter-ValueType");
    return c.get<ran_param_value_type_choice_elem_true_s>();
  }
  ran_param_value_type_choice_elem_false_s& ran_p_choice_elem_false()
  {
    assert_choice_type(types::ran_p_choice_elem_false, type_, "RANParameter-ValueType");
    return c.get<ran_param_value_type_choice_elem_false_s>();
  }
  ran_param_value_type_choice_structure_s& ran_p_choice_structure()
  {
    assert_choice_type(types::ran_p_choice_structure, type_, "RANParameter-ValueType");
    return c.get<ran_param_value_type_choice_structure_s>();
  }
  ran_param_value_type_choice_list_s& ran_p_choice_list()
  {
    assert_choice_type(types::ran_p_choice_list, type_, "RANParameter-ValueType");
    return c.get<ran_param_value_type_choice_list_s>();
  }
  const ran_param_value_type_choice_elem_true_s& ran_p_choice_elem_true() const
  {
    assert_choice_type(types::ran_p_choice_elem_true, type_, "RANParameter-ValueType");
    return c.get<ran_param_value_type_choice_elem_true_s>();
  }
  const ran_param_value_type_choice_elem_false_s& ran_p_choice_elem_false() const
  {
    assert_choice_type(types::ran_p_choice_elem_false, type_, "RANParameter-ValueType");
    return c.get<ran_param_value_type_choice_elem_false_s>();
  }
  const ran_param_value_type_choice_structure_s& ran_p_choice_structure() const
  {
    assert_choice_type(types::ran_p_choice_structure, type_, "RANParameter-ValueType");
    return c.get<ran_param_value_type_choice_structure_s>();
  }
  const ran_param_value_type_choice_list_s& ran_p_choice_list() const
  {
    assert_choice_type(types::ran_p_choice_list, type_, "RANParameter-ValueType");
    return c.get<ran_param_value_type_choice_list_s>();
  }
  ran_param_value_type_choice_elem_true_s&  set_ran_p_choice_elem_true();
  ran_param_value_type_choice_elem_false_s& set_ran_p_choice_elem_false();
  ran_param_value_type_choice_structure_s&  set_ran_p_choice_structure();
  ran_param_value_type_choice_list_s&       set_ran_p_choice_list();

private:
  types             type_;
  choice_buffer_ptr c;
};

// RANParameter-STRUCTURE-Item ::= SEQUENCE
struct ran_param_structure_item_s {
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-STRUCTURE ::= SEQUENCE
struct ran_param_structure_s {
  using seq_of_ran_params_l_ = dyn_array<ran_param_structure_item_s>;

  // member variables
  seq_of_ran_params_l_ seq_of_ran_params;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-ValueType-Choice-Structure ::= SEQUENCE
struct ran_param_value_type_choice_structure_s {
  ran_param_structure_s ran_param_structure;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-LIST ::= SEQUENCE
struct ran_param_list_s {
  using list_of_ran_param_l_ = dyn_array<ran_param_structure_s>;

  // member variables
  list_of_ran_param_l_ list_of_ran_param;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANParameter-ValueType-Choice-List ::= SEQUENCE
struct ran_param_value_type_choice_list_s {
  ran_param_list_s ran_param_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// RIC-PolicyAction-RANParameter-Item ::= SEQUENCE
struct ric_policy_action_ran_param_item_s {
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// RIC-PolicyAction ::= SEQUENCE
struct ric_policy_action_s {
  using ran_params_list_l_ = dyn_array<ric_policy_action_ran_param_item_s>;
  struct ric_policy_decision_opts {
    enum options { accept, reject, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using ric_policy_decision_e_ = enumerated<ric_policy_decision_opts, true>;

  // member variables
  uint32_t           ric_policy_action_id = 1;
  ran_params_list_l_ ran_params_list;
  // ...
  bool                   ric_policy_decision_present = false;
  ric_policy_decision_e_ ric_policy_decision;

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ActionDefinition-Format2-Item ::= SEQUENCE
struct e2sm_rc_action_definition_format2_item_s {
  ric_policy_action_s ric_policy_action;
  ran_param_testing_l ric_policy_condition_definition;
  // ...
  bool     ric_policy_condition_id_present = false;
  uint32_t ric_policy_condition_id         = 0;

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ActionDefinition-Format2 ::= SEQUENCE
struct e2sm_rc_action_definition_format2_s {
  using ric_policy_conditions_list_l_ = dyn_array<e2sm_rc_action_definition_format2_item_s>;

  // member variables
  ric_policy_conditions_list_l_ ric_policy_conditions_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ActionDefinition-Format3-Item ::= SEQUENCE
struct e2sm_rc_action_definition_format3_item_s {
  uint64_t ran_param_id = 1;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ActionDefinition-Format3 ::= SEQUENCE
struct e2sm_rc_action_definition_format3_s {
  using ran_p_insert_ind_list_l_ = dyn_array<e2sm_rc_action_definition_format3_item_s>;

  // member variables
  bool                     ue_id_present     = false;
  uint32_t                 ric_insert_ind_id = 1;
  ran_p_insert_ind_list_l_ ran_p_insert_ind_list;
  ue_id_c                  ue_id;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ActionDefinition-Format4-RANP-Item ::= SEQUENCE
struct e2sm_rc_action_definition_format4_ranp_item_s {
  uint64_t ran_param_id = 1;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ActionDefinition-Format4-Indication-Item ::= SEQUENCE
struct e2sm_rc_action_definition_format4_ind_item_s {
  using ran_p_insert_ind_list_l_ = dyn_array<e2sm_rc_action_definition_format4_ranp_item_s>;

  // member variables
  uint32_t                 ric_insert_ind_id = 1;
  ran_p_insert_ind_list_l_ ran_p_insert_ind_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ActionDefinition-Format4-Style-Item ::= SEQUENCE
struct e2sm_rc_action_definition_format4_style_item_s {
  using ric_insert_ind_list_l_ = dyn_array<e2sm_rc_action_definition_format4_ind_item_s>;

  // member variables
  int64_t                requested_insert_style_type = 0;
  ric_insert_ind_list_l_ ric_insert_ind_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ActionDefinition-Format4 ::= SEQUENCE
struct e2sm_rc_action_definition_format4_s {
  using ric_insert_style_list_l_ = dyn_array<e2sm_rc_action_definition_format4_style_item_s>;

  // member variables
  bool                     ue_id_present = false;
  ric_insert_style_list_l_ ric_insert_style_list;
  ue_id_c                  ue_id;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ActionDefinition ::= SEQUENCE
struct e2sm_rc_action_definition_s {
  struct ric_action_definition_formats_c_ {
    struct types_opts {
      enum options {
        action_definition_format1,
        action_definition_format2,
        action_definition_format3,
        // ...
        action_definition_format4,
        nulltype
      } value;

      static const asn1::table::enum_desc desc;
      uint8_t                             to_number() const;
    };
    using types = enumerated<types_opts, true, 1>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    e2sm_rc_action_definition_format1_s&          action_definition_format1()
    {
      assert_choice_type(types::action_definition_format1, c.type, "ric-actionDefinition-formats");
      return c.get<e2sm_rc_action_definition_format1_s>();
    }
    e2sm_rc_action_definition_format2_s& action_definition_format2()
    {
      assert_choice_type(types::action_definition_format2, c.type, "ric-actionDefinition-formats");
      return c.get<e2sm_rc_action_definition_format2_s>();
    }
    e2sm_rc_action_definition_format3_s& action_definition_format3()
    {
      assert_choice_type(types::action_definition_format3, c.type, "ric-actionDefinition-formats");
      return c.get<e2sm_rc_action_definition_format3_s>();
    }
    e2sm_rc_action_definition_format4_s& action_definition_format4()
    {
      assert_choice_type(types::action_definition_format4, c.type, "ric-actionDefinition-formats");
      return c.get<e2sm_rc_action_definition_format4_s>();
    }
    const e2sm_rc_action_definition_format1_s& action_definition_format1() const
    {
      assert_choice_type(types::action_definition_format1, c.type, "ric-actionDefinition-formats");
      return c.get<e2sm_rc_action_definition_format1_s>();
    }
    const e2sm_rc_action_definition_format2_s& action_definition_format2() const
    {
      assert_choice_type(types::action_definition_format2, c.type, "ric-actionDefinition-formats");
      return c.get<e2sm_rc_action_definition_format2_s>();
    }
    const e2sm_rc_action_definition_format3_s& action_definition_format3() const
    {
      assert_choice_type(types::action_definition_format3, c.type, "ric-actionDefinition-formats");
      return c.get<e2sm_rc_action_definition_format3_s>();
    }
    const e2sm_rc_action_definition_format4_s& action_definition_format4() const
    {
      assert_choice_type(types::action_definition_format4, c.type, "ric-actionDefinition-formats");
      return c.get<e2sm_rc_action_definition_format4_s>();
    }
    e2sm_rc_action_definition_format1_s& set_action_definition_format1();
    e2sm_rc_action_definition_format2_s& set_action_definition_format2();
    e2sm_rc_action_definition_format3_s& set_action_definition_format3();
    e2sm_rc_action_definition_format4_s& set_action_definition_format4();

  private:
    asn1::table::choice_storage<ric_action_definition_formats_c_,
                                types,
                                choice_buffer_t<e2sm_rc_action_definition_format1_s,
                                                e2sm_rc_action_definition_format2_s,
                                                e2sm_rc_action_definition_format3_s,
                                                e2sm_rc_action_definition_format4_s>>
        c;
  };

  // member variables
  int64_t                          ric_style_type = 0;
  ric_action_definition_formats_c_ ric_action_definition_formats;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-CallProcessID-Format1 ::= SEQUENCE
struct e2sm_rc_call_process_id_format1_s {
  uint64_t ric_call_process_id = 1;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-CallProcessID ::= SEQUENCE
struct e2sm_rc_call_process_id_s {
  struct ric_call_process_id_formats_c_ {
    struct types_opts {
      enum options { call_process_id_format1, /*...*/ nulltype } value;

      static const asn1::table::enum_desc desc;
      uint8_t                             to_number() const;
    };
    using types = enumerated<types_opts, true>;

    types                                    type() const { return types::call_process_id_format1; }
    static const asn1::table::choice_desc    desc;
    e2sm_rc_call_process_id_format1_s&       call_process_id_format1() { return c; }
    const e2sm_rc_call_process_id_format1_s& call_process_id_format1() const { return c; }

  private:
    e2sm_rc_call_process_id_format1_s c;
  };

  // member variables
  ric_call_process_id_formats_c_ ric_call_process_id_formats;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlHeader-Format1 ::= SEQUENCE
struct e2sm_rc_ctrl_hdr_format1_s {
  struct ric_ctrl_decision_opts {
    enum options { accept, reject, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using ric_ctrl_decision_e_ = enumerated<ric_ctrl_decision_opts, true>;

  // member variables
  bool                 ric_ctrl_decision_present = false;
  ue_id_c              ue_id;
  int64_t              ric_style_type     = 0;
  uint32_t             ric_ctrl_action_id = 1;
  ric_ctrl_decision_e_ ric_ctrl_decision;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlHeader-Format2 ::= SEQUENCE
struct e2sm_rc_ctrl_hdr_format2_s {
  struct ric_ctrl_decision_opts {
    enum options { accept, reject, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using ric_ctrl_decision_e_ = enumerated<ric_ctrl_decision_opts, true>;

  // member variables
  bool                 ue_id_present             = false;
  bool                 ric_ctrl_decision_present = false;
  ue_id_c              ue_id;
  ric_ctrl_decision_e_ ric_ctrl_decision;
  // ...

  static const asn1::table::seq_desc desc;
};

// UEGroupDefinitionIdentifier-Item ::= SEQUENCE
struct ue_group_definition_id_item_s {
  bool                   lc_or_present = false;
  uint64_t               ran_param_id  = 1;
  ran_param_value_type_c ran_param_value_type;
  lc_or_e2sm_rc_ies_e    lc_or;
  // ...

  static const asn1::table::seq_desc desc;
};

// UE-Group-Definition ::= SEQUENCE
struct ue_group_definition_s {
  using ue_group_definition_id_list_l_ = dyn_array<ue_group_definition_id_item_s>;

  // member variables
  ue_group_definition_id_list_l_ ue_group_definition_id_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlHeader-Format3 ::= SEQUENCE
struct e2sm_rc_ctrl_hdr_format3_s {
  uint32_t              ue_group_id = 1;
  ue_group_definition_s ue_group_definition;
  int64_t               ric_style_type     = 0;
  uint32_t              ric_ctrl_action_id = 1;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlHeader-Format4 ::= SEQUENCE
struct e2sm_rc_ctrl_hdr_format4_s {
  struct ric_ctrl_decision_opts {
    enum options { accept, reject, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using ric_ctrl_decision_e_ = enumerated<ric_ctrl_decision_opts, true>;

  // member variables
  bool                 ric_ctrl_decision_present = false;
  partial_ue_id_s      partial_ue_id;
  int64_t              ric_style_type     = 0;
  uint32_t             ric_ctrl_action_id = 1;
  ric_ctrl_decision_e_ ric_ctrl_decision;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlHeader ::= SEQUENCE
struct e2sm_rc_ctrl_hdr_s {
  struct ric_ctrl_hdr_formats_c_ {
    struct types_opts {
      enum options { ctrl_hdr_format1, /*...*/ ctrl_hdr_format2, ctrl_hdr_format3, ctrl_hdr_format4, nulltype } value;

      static const asn1::table::enum_desc desc;
      uint8_t                             to_number() const;
    };
    using types = enumerated<types_opts, true, 3>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    e2sm_rc_ctrl_hdr_format1_s&                   ctrl_hdr_format1()
    {
      assert_choice_type(types::ctrl_hdr_format1, c.type, "ric-controlHeader-formats");
      return c.get<e2sm_rc_ctrl_hdr_format1_s>();
    }
    e2sm_rc_ctrl_hdr_format2_s& ctrl_hdr_format2()
    {
      assert_choice_type(types::ctrl_hdr_format2, c.type, "ric-controlHeader-formats");
      return c.get<e2sm_rc_ctrl_hdr_format2_s>();
    }
    e2sm_rc_ctrl_hdr_format3_s& ctrl_hdr_format3()
    {
      assert_choice_type(types::ctrl_hdr_format3, c.type, "ric-controlHeader-formats");
      return c.get<e2sm_rc_ctrl_hdr_format3_s>();
    }
    e2sm_rc_ctrl_hdr_format4_s& ctrl_hdr_format4()
    {
      assert_choice_type(types::ctrl_hdr_format4, c.type, "ric-controlHeader-formats");
      return c.get<e2sm_rc_ctrl_hdr_format4_s>();
    }
    const e2sm_rc_ctrl_hdr_format1_s& ctrl_hdr_format1() const
    {
      assert_choice_type(types::ctrl_hdr_format1, c.type, "ric-controlHeader-formats");
      return c.get<e2sm_rc_ctrl_hdr_format1_s>();
    }
    const e2sm_rc_ctrl_hdr_format2_s& ctrl_hdr_format2() const
    {
      assert_choice_type(types::ctrl_hdr_format2, c.type, "ric-controlHeader-formats");
      return c.get<e2sm_rc_ctrl_hdr_format2_s>();
    }
    const e2sm_rc_ctrl_hdr_format3_s& ctrl_hdr_format3() const
    {
      assert_choice_type(types::ctrl_hdr_format3, c.type, "ric-controlHeader-formats");
      return c.get<e2sm_rc_ctrl_hdr_format3_s>();
    }
    const e2sm_rc_ctrl_hdr_format4_s& ctrl_hdr_format4() const
    {
      assert_choice_type(types::ctrl_hdr_format4, c.type, "ric-controlHeader-formats");
      return c.get<e2sm_rc_ctrl_hdr_format4_s>();
    }
    e2sm_rc_ctrl_hdr_format1_s& set_ctrl_hdr_format1();
    e2sm_rc_ctrl_hdr_format2_s& set_ctrl_hdr_format2();
    e2sm_rc_ctrl_hdr_format3_s& set_ctrl_hdr_format3();
    e2sm_rc_ctrl_hdr_format4_s& set_ctrl_hdr_format4();

  private:
    asn1::table::choice_storage<ric_ctrl_hdr_formats_c_,
                                types,
                                choice_buffer_t<e2sm_rc_ctrl_hdr_format1_s,
                                                e2sm_rc_ctrl_hdr_format2_s,
                                                e2sm_rc_ctrl_hdr_format3_s,
                                                e2sm_rc_ctrl_hdr_format4_s>>
        c;
  };

  // member variables
  ric_ctrl_hdr_formats_c_ ric_ctrl_hdr_formats;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlMessage-Format1-Item ::= SEQUENCE
struct e2sm_rc_ctrl_msg_format1_item_s {
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlMessage-Format1 ::= SEQUENCE
struct e2sm_rc_ctrl_msg_format1_s {
  using ran_p_list_l_ = dyn_array<e2sm_rc_ctrl_msg_format1_item_s>;

  // member variables
  ran_p_list_l_ ran_p_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlMessage-Format2-ControlAction-Item ::= SEQUENCE
struct e2sm_rc_ctrl_msg_format2_ctrl_action_item_s {
  uint32_t                   ric_ctrl_action_id = 1;
  e2sm_rc_ctrl_msg_format1_s ran_p_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlMessage-Format2-Style-Item ::= SEQUENCE
struct e2sm_rc_ctrl_msg_format2_style_item_s {
  using ric_ctrl_action_list_l_ = dyn_array<e2sm_rc_ctrl_msg_format2_ctrl_action_item_s>;

  // member variables
  int64_t                 indicated_ctrl_style_type = 0;
  ric_ctrl_action_list_l_ ric_ctrl_action_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlMessage-Format2 ::= SEQUENCE
struct e2sm_rc_ctrl_msg_format2_s {
  using ric_ctrl_style_list_l_ = dyn_array<e2sm_rc_ctrl_msg_format2_style_item_s>;

  // member variables
  ric_ctrl_style_list_l_ ric_ctrl_style_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// EntitySpecific-ranP-ControlParameters ::= SEQUENCE
struct entity_specific_ran_p_ctrl_params_s {
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-EntityFilter ::= SEQUENCE
struct e2sm_rc_entity_filt_s {
  using entity_specific_ctrl_ran_p_list_l_ = dyn_array<entity_specific_ran_p_ctrl_params_s>;

  // member variables
  uint16_t                           entity_filt_id = 1;
  ran_param_testing_l                entity_filt_definition;
  entity_specific_ctrl_ran_p_list_l_ entity_specific_ctrl_ran_p_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// EntityAgnostic-ranP-ControlParameters ::= SEQUENCE
struct entity_agnostic_ran_p_ctrl_params_s {
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlMessage-Format3 ::= SEQUENCE
struct e2sm_rc_ctrl_msg_format3_s {
  using list_of_entity_filts_l_            = dyn_array<e2sm_rc_entity_filt_s>;
  using entity_agnostic_ctrl_ran_p_list_l_ = dyn_array<entity_agnostic_ran_p_ctrl_params_s>;

  // member variables
  bool                               list_of_entity_filts_present            = false;
  bool                               entity_agnostic_ctrl_ran_p_list_present = false;
  list_of_entity_filts_l_            list_of_entity_filts;
  entity_agnostic_ctrl_ran_p_list_l_ entity_agnostic_ctrl_ran_p_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlMessage-Format4-Item ::= SEQUENCE
struct e2sm_rc_ctrl_msg_format4_item_s {
  bool                   ran_param_definition_present = false;
  uint64_t               ran_param_id                 = 1;
  ran_param_definition_s ran_param_definition;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlMessage-Format4 ::= SEQUENCE
struct e2sm_rc_ctrl_msg_format4_s {
  using ran_p_list_l_ = dyn_array<e2sm_rc_ctrl_msg_format4_item_s>;

  // member variables
  ran_p_list_l_ ran_p_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlMessage ::= SEQUENCE
struct e2sm_rc_ctrl_msg_s {
  struct ric_ctrl_msg_formats_c_ {
    struct types_opts {
      enum options {
        ctrl_msg_format1,
        /*...*/ ctrl_msg_format2,
        ctrl_msg_format3,
        ctrl_msg_format4,
        ctrl_msg_format5,
        nulltype
      } value;

      static const asn1::table::enum_desc desc;
      uint8_t                             to_number() const;
    };
    using types = enumerated<types_opts, true, 4>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    e2sm_rc_ctrl_msg_format1_s&                   ctrl_msg_format1()
    {
      assert_choice_type(types::ctrl_msg_format1, c.type, "ric-controlMessage-formats");
      return c.get<e2sm_rc_ctrl_msg_format1_s>();
    }
    e2sm_rc_ctrl_msg_format2_s& ctrl_msg_format2()
    {
      assert_choice_type(types::ctrl_msg_format2, c.type, "ric-controlMessage-formats");
      return c.get<e2sm_rc_ctrl_msg_format2_s>();
    }
    e2sm_rc_ctrl_msg_format3_s& ctrl_msg_format3()
    {
      assert_choice_type(types::ctrl_msg_format3, c.type, "ric-controlMessage-formats");
      return c.get<e2sm_rc_ctrl_msg_format3_s>();
    }
    e2sm_rc_ctrl_msg_format4_s& ctrl_msg_format4()
    {
      assert_choice_type(types::ctrl_msg_format4, c.type, "ric-controlMessage-formats");
      return c.get<e2sm_rc_ctrl_msg_format4_s>();
    }
    const e2sm_rc_ctrl_msg_format1_s& ctrl_msg_format1() const
    {
      assert_choice_type(types::ctrl_msg_format1, c.type, "ric-controlMessage-formats");
      return c.get<e2sm_rc_ctrl_msg_format1_s>();
    }
    const e2sm_rc_ctrl_msg_format2_s& ctrl_msg_format2() const
    {
      assert_choice_type(types::ctrl_msg_format2, c.type, "ric-controlMessage-formats");
      return c.get<e2sm_rc_ctrl_msg_format2_s>();
    }
    const e2sm_rc_ctrl_msg_format3_s& ctrl_msg_format3() const
    {
      assert_choice_type(types::ctrl_msg_format3, c.type, "ric-controlMessage-formats");
      return c.get<e2sm_rc_ctrl_msg_format3_s>();
    }
    const e2sm_rc_ctrl_msg_format4_s& ctrl_msg_format4() const
    {
      assert_choice_type(types::ctrl_msg_format4, c.type, "ric-controlMessage-formats");
      return c.get<e2sm_rc_ctrl_msg_format4_s>();
    }
    e2sm_rc_ctrl_msg_format1_s& set_ctrl_msg_format1();
    e2sm_rc_ctrl_msg_format2_s& set_ctrl_msg_format2();
    e2sm_rc_ctrl_msg_format3_s& set_ctrl_msg_format3();
    e2sm_rc_ctrl_msg_format4_s& set_ctrl_msg_format4();
    void                        set_ctrl_msg_format5();

  private:
    asn1::table::choice_storage<ric_ctrl_msg_formats_c_,
                                types,
                                choice_buffer_t<e2sm_rc_ctrl_msg_format1_s,
                                                e2sm_rc_ctrl_msg_format2_s,
                                                e2sm_rc_ctrl_msg_format3_s,
                                                e2sm_rc_ctrl_msg_format4_s>>
        c;
  };

  // member variables
  ric_ctrl_msg_formats_c_ ric_ctrl_msg_formats;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlOutcome-Format1-Item ::= SEQUENCE
struct e2sm_rc_ctrl_outcome_format1_item_s {
  uint64_t          ran_param_id = 1;
  ran_param_value_c ran_param_value;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlOutcome-Format1 ::= SEQUENCE
struct e2sm_rc_ctrl_outcome_format1_s {
  using ran_p_list_l_ = dyn_array<e2sm_rc_ctrl_outcome_format1_item_s>;

  // member variables
  ran_p_list_l_ ran_p_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlOutcome-Format2-RANP-Item ::= SEQUENCE
struct e2sm_rc_ctrl_outcome_format2_ranp_item_s {
  uint64_t          ran_param_id = 1;
  ran_param_value_c ran_param_value;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlOutcome-Format2-ControlOutcome-Item ::= SEQUENCE
struct e2sm_rc_ctrl_outcome_format2_ctrl_outcome_item_s {
  using ran_p_list_l_ = dyn_array<e2sm_rc_ctrl_outcome_format2_ranp_item_s>;

  // member variables
  uint32_t      ric_ctrl_action_id = 1;
  ran_p_list_l_ ran_p_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlOutcome-Format2-Style-Item ::= SEQUENCE
struct e2sm_rc_ctrl_outcome_format2_style_item_s {
  using ric_ctrl_outcome_list_l_ = dyn_array<e2sm_rc_ctrl_outcome_format2_ctrl_outcome_item_s>;

  // member variables
  int64_t                  indicated_ctrl_style_type = 0;
  ric_ctrl_outcome_list_l_ ric_ctrl_outcome_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlOutcome-Format2 ::= SEQUENCE
struct e2sm_rc_ctrl_outcome_format2_s {
  using ric_ctrl_style_list_l_ = dyn_array<e2sm_rc_ctrl_outcome_format2_style_item_s>;

  // member variables
  ric_ctrl_style_list_l_ ric_ctrl_style_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlOutcome-Format3-Item ::= SEQUENCE
struct e2sm_rc_ctrl_outcome_format3_item_s {
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlOutcome-Format3 ::= SEQUENCE
struct e2sm_rc_ctrl_outcome_format3_s {
  using ran_p_list_l_ = dyn_array<e2sm_rc_ctrl_outcome_format3_item_s>;

  // member variables
  ran_p_list_l_ ran_p_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ControlOutcome ::= SEQUENCE
struct e2sm_rc_ctrl_outcome_s {
  struct ric_ctrl_outcome_formats_c_ {
    struct types_opts {
      enum options { ctrl_outcome_format1, /*...*/ ctrl_outcome_format2, ctrl_outcome_format3, nulltype } value;

      static const asn1::table::enum_desc desc;
      uint8_t                             to_number() const;
    };
    using types = enumerated<types_opts, true, 2>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    e2sm_rc_ctrl_outcome_format1_s&               ctrl_outcome_format1()
    {
      assert_choice_type(types::ctrl_outcome_format1, c.type, "ric-controlOutcome-formats");
      return c.get<e2sm_rc_ctrl_outcome_format1_s>();
    }
    e2sm_rc_ctrl_outcome_format2_s& ctrl_outcome_format2()
    {
      assert_choice_type(types::ctrl_outcome_format2, c.type, "ric-controlOutcome-formats");
      return c.get<e2sm_rc_ctrl_outcome_format2_s>();
    }
    e2sm_rc_ctrl_outcome_format3_s& ctrl_outcome_format3()
    {
      assert_choice_type(types::ctrl_outcome_format3, c.type, "ric-controlOutcome-formats");
      return c.get<e2sm_rc_ctrl_outcome_format3_s>();
    }
    const e2sm_rc_ctrl_outcome_format1_s& ctrl_outcome_format1() const
    {
      assert_choice_type(types::ctrl_outcome_format1, c.type, "ric-controlOutcome-formats");
      return c.get<e2sm_rc_ctrl_outcome_format1_s>();
    }
    const e2sm_rc_ctrl_outcome_format2_s& ctrl_outcome_format2() const
    {
      assert_choice_type(types::ctrl_outcome_format2, c.type, "ric-controlOutcome-formats");
      return c.get<e2sm_rc_ctrl_outcome_format2_s>();
    }
    const e2sm_rc_ctrl_outcome_format3_s& ctrl_outcome_format3() const
    {
      assert_choice_type(types::ctrl_outcome_format3, c.type, "ric-controlOutcome-formats");
      return c.get<e2sm_rc_ctrl_outcome_format3_s>();
    }
    e2sm_rc_ctrl_outcome_format1_s& set_ctrl_outcome_format1();
    e2sm_rc_ctrl_outcome_format2_s& set_ctrl_outcome_format2();
    e2sm_rc_ctrl_outcome_format3_s& set_ctrl_outcome_format3();

  private:
    asn1::table::choice_storage<
        ric_ctrl_outcome_formats_c_,
        types,
        choice_buffer_t<e2sm_rc_ctrl_outcome_format1_s, e2sm_rc_ctrl_outcome_format2_s, e2sm_rc_ctrl_outcome_format3_s>>
        c;
  };

  // member variables
  ric_ctrl_outcome_formats_c_ ric_ctrl_outcome_formats;
  // ...

  static const asn1::table::seq_desc desc;
};

// MessageType-Choice-NI ::= SEQUENCE
struct msg_type_choice_ni_s {
  bool               ni_id_present  = false;
  bool               ni_msg_present = false;
  interface_type_e   ni_type;
  interface_id_c     ni_id;
  interface_msg_id_s ni_msg;
  // ...

  static const asn1::table::seq_desc desc;
};

// MessageType-Choice-RRC ::= SEQUENCE
struct msg_type_choice_rrc_s {
  rrc_msg_id_s rrc_msg;
  // ...

  static const asn1::table::seq_desc desc;
};

// MessageType-Choice ::= CHOICE
struct msg_type_choice_c {
  struct types_opts {
    enum options { msg_type_choice_ni, msg_type_choice_rrc, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  msg_type_choice_ni_s&                         msg_type_choice_ni()
  {
    assert_choice_type(types::msg_type_choice_ni, c.type, "MessageType-Choice");
    return c.get<msg_type_choice_ni_s>();
  }
  msg_type_choice_rrc_s& msg_type_choice_rrc()
  {
    assert_choice_type(types::msg_type_choice_rrc, c.type, "MessageType-Choice");
    return c.get<msg_type_choice_rrc_s>();
  }
  const msg_type_choice_ni_s& msg_type_choice_ni() const
  {
    assert_choice_type(types::msg_type_choice_ni, c.type, "MessageType-Choice");
    return c.get<msg_type_choice_ni_s>();
  }
  const msg_type_choice_rrc_s& msg_type_choice_rrc() const
  {
    assert_choice_type(types::msg_type_choice_rrc, c.type, "MessageType-Choice");
    return c.get<msg_type_choice_rrc_s>();
  }
  msg_type_choice_ni_s&  set_msg_type_choice_ni();
  msg_type_choice_rrc_s& set_msg_type_choice_rrc();

private:
  asn1::table::choice_storage<msg_type_choice_c, types, choice_buffer_t<msg_type_choice_ni_s, msg_type_choice_rrc_s>> c;
};

// EventTrigger-UE-Info-Item ::= SEQUENCE
struct event_trigger_ue_info_item_s {
  struct ue_type_c_ {
    struct types_opts {
      enum options { ue_type_choice_individual, ue_type_choice_group, /*...*/ nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts, true>;

    static const asn1::table::choice_storage_desc   storage_desc;
    void                                            set(types::options e = types::nulltype) { c.set(e); }
    types                                           type() const { return c.type; }
    static const asn1::table::choice_desc           desc;
    event_trigger_ue_info_item_choice_individual_s& ue_type_choice_individual()
    {
      assert_choice_type(types::ue_type_choice_individual, c.type, "ueType");
      return c.get<event_trigger_ue_info_item_choice_individual_s>();
    }
    event_trigger_ue_info_item_choice_group_s& ue_type_choice_group()
    {
      assert_choice_type(types::ue_type_choice_group, c.type, "ueType");
      return c.get<event_trigger_ue_info_item_choice_group_s>();
    }
    const event_trigger_ue_info_item_choice_individual_s& ue_type_choice_individual() const
    {
      assert_choice_type(types::ue_type_choice_individual, c.type, "ueType");
      return c.get<event_trigger_ue_info_item_choice_individual_s>();
    }
    const event_trigger_ue_info_item_choice_group_s& ue_type_choice_group() const
    {
      assert_choice_type(types::ue_type_choice_group, c.type, "ueType");
      return c.get<event_trigger_ue_info_item_choice_group_s>();
    }
    event_trigger_ue_info_item_choice_individual_s& set_ue_type_choice_individual();
    event_trigger_ue_info_item_choice_group_s&      set_ue_type_choice_group();

  private:
    asn1::table::choice_storage<
        ue_type_c_,
        types,
        choice_buffer_t<event_trigger_ue_info_item_choice_group_s, event_trigger_ue_info_item_choice_individual_s>>
        c;
  };

  // member variables
  bool                lc_or_present       = false;
  uint32_t            event_trigger_ue_id = 1;
  ue_type_c_          ue_type;
  lc_or_e2sm_rc_ies_e lc_or;
  // ...

  static const asn1::table::seq_desc desc;
};

// EventTrigger-UE-Info ::= SEQUENCE
struct event_trigger_ue_info_s {
  using ue_info_list_l_ = dyn_array<event_trigger_ue_info_item_s>;

  // member variables
  ue_info_list_l_ ue_info_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// EventTrigger-UEevent-Info-Item ::= SEQUENCE
struct event_trigger_ue_event_info_item_s {
  bool                lc_or_present = false;
  uint32_t            ue_event_id   = 1;
  lc_or_e2sm_rc_ies_e lc_or;
  // ...

  static const asn1::table::seq_desc desc;
};

// EventTrigger-UEevent-Info ::= SEQUENCE
struct event_trigger_ue_event_info_s {
  using ue_event_list_l_ = dyn_array<event_trigger_ue_event_info_item_s>;

  // member variables
  ue_event_list_l_ ue_event_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-EventTrigger-Format1-Item ::= SEQUENCE
struct e2sm_rc_event_trigger_format1_item_s {
  struct msg_direction_opts {
    enum options { incoming, outgoing, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using msg_direction_e_ = enumerated<msg_direction_opts, true>;

  // member variables
  bool                          msg_direction_present          = false;
  bool                          associated_ue_info_present     = false;
  bool                          associated_ue_event_present    = false;
  bool                          lc_or_present                  = false;
  uint32_t                      ric_event_trigger_condition_id = 1;
  msg_type_choice_c             msg_type;
  msg_direction_e_              msg_direction;
  event_trigger_ue_info_s       associated_ue_info;
  event_trigger_ue_event_info_s associated_ue_event;
  lc_or_e2sm_rc_ies_e           lc_or;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-EventTrigger-Format1 ::= SEQUENCE
struct e2sm_rc_event_trigger_format1_s {
  using msg_list_l_ = dyn_array<e2sm_rc_event_trigger_format1_item_s>;

  // member variables
  bool                    global_associated_ue_info_present = false;
  msg_list_l_             msg_list;
  event_trigger_ue_info_s global_associated_ue_info;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-EventTrigger-Format2 ::= SEQUENCE
struct e2sm_rc_event_trigger_format2_s {
  bool                    associated_ue_info_present     = false;
  uint32_t                ric_call_process_type_id       = 1;
  uint32_t                ric_call_process_breakpoint_id = 1;
  ran_param_testing_l     associated_e2_node_info;
  event_trigger_ue_info_s associated_ue_info;
  // ...

  static const asn1::table::seq_desc desc;
};

// EventTrigger-Cell-Info-Item-Choice-Individual ::= SEQUENCE
struct event_trigger_cell_info_item_choice_individual_s {
  cgi_c cell_global_id;
  // ...

  static const asn1::table::seq_desc desc;
};

// EventTrigger-Cell-Info-Item-Choice-Group ::= SEQUENCE
struct event_trigger_cell_info_item_choice_group_s {
  ran_param_testing_l ran_param_testing;
  // ...

  static const asn1::table::seq_desc desc;
};

// EventTrigger-Cell-Info-Item ::= SEQUENCE
struct event_trigger_cell_info_item_s {
  struct cell_type_c_ {
    struct types_opts {
      enum options { cell_type_choice_individual, cell_type_choice_group, /*...*/ nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts, true>;

    static const asn1::table::choice_storage_desc     storage_desc;
    void                                              set(types::options e = types::nulltype) { c.set(e); }
    types                                             type() const { return c.type; }
    static const asn1::table::choice_desc             desc;
    event_trigger_cell_info_item_choice_individual_s& cell_type_choice_individual()
    {
      assert_choice_type(types::cell_type_choice_individual, c.type, "cellType");
      return c.get<event_trigger_cell_info_item_choice_individual_s>();
    }
    event_trigger_cell_info_item_choice_group_s& cell_type_choice_group()
    {
      assert_choice_type(types::cell_type_choice_group, c.type, "cellType");
      return c.get<event_trigger_cell_info_item_choice_group_s>();
    }
    const event_trigger_cell_info_item_choice_individual_s& cell_type_choice_individual() const
    {
      assert_choice_type(types::cell_type_choice_individual, c.type, "cellType");
      return c.get<event_trigger_cell_info_item_choice_individual_s>();
    }
    const event_trigger_cell_info_item_choice_group_s& cell_type_choice_group() const
    {
      assert_choice_type(types::cell_type_choice_group, c.type, "cellType");
      return c.get<event_trigger_cell_info_item_choice_group_s>();
    }
    event_trigger_cell_info_item_choice_individual_s& set_cell_type_choice_individual();
    event_trigger_cell_info_item_choice_group_s&      set_cell_type_choice_group();

  private:
    asn1::table::choice_storage<
        cell_type_c_,
        types,
        choice_buffer_t<event_trigger_cell_info_item_choice_group_s, event_trigger_cell_info_item_choice_individual_s>>
        c;
  };

  // member variables
  bool                lc_or_present         = false;
  uint32_t            event_trigger_cell_id = 1;
  cell_type_c_        cell_type;
  lc_or_e2sm_rc_ies_e lc_or;
  // ...

  static const asn1::table::seq_desc desc;
};

// EventTrigger-Cell-Info ::= SEQUENCE
struct event_trigger_cell_info_s {
  using cell_info_list_l_ = dyn_array<event_trigger_cell_info_item_s>;

  // member variables
  cell_info_list_l_ cell_info_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-EventTrigger-Format3-Item ::= SEQUENCE
struct e2sm_rc_event_trigger_format3_item_s {
  bool                      associated_cell_info_present   = false;
  bool                      lc_or_present                  = false;
  uint32_t                  ric_event_trigger_condition_id = 1;
  uint16_t                  e2_node_info_change_id         = 1;
  event_trigger_cell_info_s associated_cell_info;
  lc_or_e2sm_rc_ies_e       lc_or;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-EventTrigger-Format3 ::= SEQUENCE
struct e2sm_rc_event_trigger_format3_s {
  using e2_node_info_change_list_l_ = dyn_array<e2sm_rc_event_trigger_format3_item_s>;

  // member variables
  e2_node_info_change_list_l_ e2_node_info_change_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// RRC-State ::= ENUMERATED
struct rrc_state_opts {
  enum options { rrc_connected, rrc_inactive, rrc_idle, any, /*...*/ nulltype } value;

  static const asn1::table::enum_desc desc;
};
using rrc_state_e = enumerated<rrc_state_opts, true>;

// TriggerType-Choice-RRCstate-Item ::= SEQUENCE
struct trigger_type_choice_rrc_state_item_s {
  bool                lc_or_present = false;
  rrc_state_e         state_changed_to;
  lc_or_e2sm_rc_ies_e lc_or;
  // ...

  static const asn1::table::seq_desc desc;
};

// TriggerType-Choice-RRCstate ::= SEQUENCE
struct trigger_type_choice_rrc_state_s {
  using rrc_state_list_l_ = dyn_array<trigger_type_choice_rrc_state_item_s>;

  // member variables
  rrc_state_list_l_ rrc_state_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// TriggerType-Choice-UEID ::= SEQUENCE
struct trigger_type_choice_ue_id_s {
  uint16_t ue_id_change_id = 1;
  // ...

  static const asn1::table::seq_desc desc;
};

// TriggerType-Choice-L2state ::= SEQUENCE
struct trigger_type_choice_l2state_s {
  ran_param_testing_l associated_l2variables;
  // ...

  static const asn1::table::seq_desc desc;
};

// TriggerType-Choice-UEcontext ::= SEQUENCE
struct trigger_type_choice_ue_context_s {
  ran_param_testing_l associated_ue_ctxt_variables;
  // ...

  static const asn1::table::seq_desc desc;
};

// TriggerType-Choice-MIMOandBFconfig ::= SEQUENCE
struct trigger_type_choice_mim_oand_b_fcfg_s {
  struct mim_otrans_mode_state_opts {
    enum options { enabled, disabled, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using mim_otrans_mode_state_e_ = enumerated<mim_otrans_mode_state_opts, true>;

  // member variables
  mim_otrans_mode_state_e_ mim_otrans_mode_state;
  // ...

  static const asn1::table::seq_desc desc;
};

// L2MACschChgType-Choice ::= CHOICE
struct l2_ma_csch_chg_type_choice_c {
  struct types_opts {
    enum options { trigger_type_choice_mim_oand_b_fcfg, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  types                                        type() const { return types::trigger_type_choice_mim_oand_b_fcfg; }
  static const asn1::table::choice_desc        desc;
  trigger_type_choice_mim_oand_b_fcfg_s&       trigger_type_choice_mim_oand_b_fcfg() { return c; }
  const trigger_type_choice_mim_oand_b_fcfg_s& trigger_type_choice_mim_oand_b_fcfg() const { return c; }

private:
  trigger_type_choice_mim_oand_b_fcfg_s c;
};

// TriggerType-Choice-L2MACschChg ::= SEQUENCE
struct trigger_type_choice_l2_ma_csch_chg_s {
  l2_ma_csch_chg_type_choice_c l2_ma_csch_chg_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// TriggerType-Choice ::= CHOICE
struct trigger_type_choice_c {
  struct types_opts {
    enum options {
      trigger_type_choice_rrc_state,
      trigger_type_choice_ue_id,
      trigger_type_choice_l2state,
      // ...
      trigger_type_choice_ue_context,
      trigger_type_choice_l2_ma_csch_chg,
      nulltype
    } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true, 2>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  trigger_type_choice_rrc_state_s&              trigger_type_choice_rrc_state()
  {
    assert_choice_type(types::trigger_type_choice_rrc_state, c.type, "TriggerType-Choice");
    return c.get<trigger_type_choice_rrc_state_s>();
  }
  trigger_type_choice_ue_id_s& trigger_type_choice_ue_id()
  {
    assert_choice_type(types::trigger_type_choice_ue_id, c.type, "TriggerType-Choice");
    return c.get<trigger_type_choice_ue_id_s>();
  }
  trigger_type_choice_l2state_s& trigger_type_choice_l2state()
  {
    assert_choice_type(types::trigger_type_choice_l2state, c.type, "TriggerType-Choice");
    return c.get<trigger_type_choice_l2state_s>();
  }
  trigger_type_choice_ue_context_s& trigger_type_choice_ue_context()
  {
    assert_choice_type(types::trigger_type_choice_ue_context, c.type, "TriggerType-Choice");
    return c.get<trigger_type_choice_ue_context_s>();
  }
  trigger_type_choice_l2_ma_csch_chg_s& trigger_type_choice_l2_ma_csch_chg()
  {
    assert_choice_type(types::trigger_type_choice_l2_ma_csch_chg, c.type, "TriggerType-Choice");
    return c.get<trigger_type_choice_l2_ma_csch_chg_s>();
  }
  const trigger_type_choice_rrc_state_s& trigger_type_choice_rrc_state() const
  {
    assert_choice_type(types::trigger_type_choice_rrc_state, c.type, "TriggerType-Choice");
    return c.get<trigger_type_choice_rrc_state_s>();
  }
  const trigger_type_choice_ue_id_s& trigger_type_choice_ue_id() const
  {
    assert_choice_type(types::trigger_type_choice_ue_id, c.type, "TriggerType-Choice");
    return c.get<trigger_type_choice_ue_id_s>();
  }
  const trigger_type_choice_l2state_s& trigger_type_choice_l2state() const
  {
    assert_choice_type(types::trigger_type_choice_l2state, c.type, "TriggerType-Choice");
    return c.get<trigger_type_choice_l2state_s>();
  }
  const trigger_type_choice_ue_context_s& trigger_type_choice_ue_context() const
  {
    assert_choice_type(types::trigger_type_choice_ue_context, c.type, "TriggerType-Choice");
    return c.get<trigger_type_choice_ue_context_s>();
  }
  const trigger_type_choice_l2_ma_csch_chg_s& trigger_type_choice_l2_ma_csch_chg() const
  {
    assert_choice_type(types::trigger_type_choice_l2_ma_csch_chg, c.type, "TriggerType-Choice");
    return c.get<trigger_type_choice_l2_ma_csch_chg_s>();
  }
  trigger_type_choice_rrc_state_s&      set_trigger_type_choice_rrc_state();
  trigger_type_choice_ue_id_s&          set_trigger_type_choice_ue_id();
  trigger_type_choice_l2state_s&        set_trigger_type_choice_l2state();
  trigger_type_choice_ue_context_s&     set_trigger_type_choice_ue_context();
  trigger_type_choice_l2_ma_csch_chg_s& set_trigger_type_choice_l2_ma_csch_chg();

private:
  asn1::table::choice_storage<trigger_type_choice_c,
                              types,
                              choice_buffer_t<trigger_type_choice_l2_ma_csch_chg_s,
                                              trigger_type_choice_l2state_s,
                                              trigger_type_choice_rrc_state_s,
                                              trigger_type_choice_ue_context_s,
                                              trigger_type_choice_ue_id_s>>
      c;
};

// E2SM-RC-EventTrigger-Format4-Item ::= SEQUENCE
struct e2sm_rc_event_trigger_format4_item_s {
  bool                    associated_ue_info_present     = false;
  bool                    lc_or_present                  = false;
  uint32_t                ric_event_trigger_condition_id = 1;
  trigger_type_choice_c   trigger_type;
  event_trigger_ue_info_s associated_ue_info;
  lc_or_e2sm_rc_ies_e     lc_or;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-EventTrigger-Format4 ::= SEQUENCE
struct e2sm_rc_event_trigger_format4_s {
  using ue_info_change_list_l_ = dyn_array<e2sm_rc_event_trigger_format4_item_s>;

  // member variables
  ue_info_change_list_l_ ue_info_change_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-EventTrigger ::= SEQUENCE
struct e2sm_rc_event_trigger_s {
  struct ric_event_trigger_formats_c_ {
    struct types_opts {
      enum options {
        event_trigger_format1,
        event_trigger_format2,
        event_trigger_format3,
        event_trigger_format4,
        event_trigger_format5,
        // ...
        nulltype
      } value;

      static const asn1::table::enum_desc desc;
      uint8_t                             to_number() const;
    };
    using types = enumerated<types_opts, true>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    e2sm_rc_event_trigger_format1_s&              event_trigger_format1()
    {
      assert_choice_type(types::event_trigger_format1, c.type, "ric-eventTrigger-formats");
      return c.get<e2sm_rc_event_trigger_format1_s>();
    }
    e2sm_rc_event_trigger_format2_s& event_trigger_format2()
    {
      assert_choice_type(types::event_trigger_format2, c.type, "ric-eventTrigger-formats");
      return c.get<e2sm_rc_event_trigger_format2_s>();
    }
    e2sm_rc_event_trigger_format3_s& event_trigger_format3()
    {
      assert_choice_type(types::event_trigger_format3, c.type, "ric-eventTrigger-formats");
      return c.get<e2sm_rc_event_trigger_format3_s>();
    }
    e2sm_rc_event_trigger_format4_s& event_trigger_format4()
    {
      assert_choice_type(types::event_trigger_format4, c.type, "ric-eventTrigger-formats");
      return c.get<e2sm_rc_event_trigger_format4_s>();
    }
    const e2sm_rc_event_trigger_format1_s& event_trigger_format1() const
    {
      assert_choice_type(types::event_trigger_format1, c.type, "ric-eventTrigger-formats");
      return c.get<e2sm_rc_event_trigger_format1_s>();
    }
    const e2sm_rc_event_trigger_format2_s& event_trigger_format2() const
    {
      assert_choice_type(types::event_trigger_format2, c.type, "ric-eventTrigger-formats");
      return c.get<e2sm_rc_event_trigger_format2_s>();
    }
    const e2sm_rc_event_trigger_format3_s& event_trigger_format3() const
    {
      assert_choice_type(types::event_trigger_format3, c.type, "ric-eventTrigger-formats");
      return c.get<e2sm_rc_event_trigger_format3_s>();
    }
    const e2sm_rc_event_trigger_format4_s& event_trigger_format4() const
    {
      assert_choice_type(types::event_trigger_format4, c.type, "ric-eventTrigger-formats");
      return c.get<e2sm_rc_event_trigger_format4_s>();
    }
    e2sm_rc_event_trigger_format1_s& set_event_trigger_format1();
    e2sm_rc_event_trigger_format2_s& set_event_trigger_format2();
    e2sm_rc_event_trigger_format3_s& set_event_trigger_format3();
    e2sm_rc_event_trigger_format4_s& set_event_trigger_format4();
    void                             set_event_trigger_format5();

  private:
    asn1::table::choice_storage<ric_event_trigger_formats_c_,
                                types,
                                choice_buffer_t<e2sm_rc_event_trigger_format1_s,
                                                e2sm_rc_event_trigger_format2_s,
                                                e2sm_rc_event_trigger_format3_s,
                                                e2sm_rc_event_trigger_format4_s>>
        c;
  };

  // member variables
  ric_event_trigger_formats_c_ ric_event_trigger_formats;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationHeader-Format1 ::= SEQUENCE
struct e2sm_rc_ind_hdr_format1_s {
  bool     ric_event_trigger_condition_id_present = false;
  uint32_t ric_event_trigger_condition_id         = 1;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationHeader-Format2 ::= SEQUENCE
struct e2sm_rc_ind_hdr_format2_s {
  ue_id_c  ue_id;
  int64_t  ric_insert_style_type = 0;
  uint32_t ric_insert_ind_id     = 1;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationHeader-Format3 ::= SEQUENCE
struct e2sm_rc_ind_hdr_format3_s {
  bool     ric_event_trigger_condition_id_present = false;
  bool     ue_id_present                          = false;
  uint32_t ric_event_trigger_condition_id         = 1;
  ue_id_c  ue_id;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationHeader ::= SEQUENCE
struct e2sm_rc_ind_hdr_s {
  struct ric_ind_hdr_formats_c_ {
    struct types_opts {
      enum options { ind_hdr_format1, ind_hdr_format2, /*...*/ ind_hdr_format3, nulltype } value;

      static const asn1::table::enum_desc desc;
      uint8_t                             to_number() const;
    };
    using types = enumerated<types_opts, true, 1>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    e2sm_rc_ind_hdr_format1_s&                    ind_hdr_format1()
    {
      assert_choice_type(types::ind_hdr_format1, c.type, "ric-indicationHeader-formats");
      return c.get<e2sm_rc_ind_hdr_format1_s>();
    }
    e2sm_rc_ind_hdr_format2_s& ind_hdr_format2()
    {
      assert_choice_type(types::ind_hdr_format2, c.type, "ric-indicationHeader-formats");
      return c.get<e2sm_rc_ind_hdr_format2_s>();
    }
    e2sm_rc_ind_hdr_format3_s& ind_hdr_format3()
    {
      assert_choice_type(types::ind_hdr_format3, c.type, "ric-indicationHeader-formats");
      return c.get<e2sm_rc_ind_hdr_format3_s>();
    }
    const e2sm_rc_ind_hdr_format1_s& ind_hdr_format1() const
    {
      assert_choice_type(types::ind_hdr_format1, c.type, "ric-indicationHeader-formats");
      return c.get<e2sm_rc_ind_hdr_format1_s>();
    }
    const e2sm_rc_ind_hdr_format2_s& ind_hdr_format2() const
    {
      assert_choice_type(types::ind_hdr_format2, c.type, "ric-indicationHeader-formats");
      return c.get<e2sm_rc_ind_hdr_format2_s>();
    }
    const e2sm_rc_ind_hdr_format3_s& ind_hdr_format3() const
    {
      assert_choice_type(types::ind_hdr_format3, c.type, "ric-indicationHeader-formats");
      return c.get<e2sm_rc_ind_hdr_format3_s>();
    }
    e2sm_rc_ind_hdr_format1_s& set_ind_hdr_format1();
    e2sm_rc_ind_hdr_format2_s& set_ind_hdr_format2();
    e2sm_rc_ind_hdr_format3_s& set_ind_hdr_format3();

  private:
    asn1::table::choice_storage<
        ric_ind_hdr_formats_c_,
        types,
        choice_buffer_t<e2sm_rc_ind_hdr_format1_s, e2sm_rc_ind_hdr_format2_s, e2sm_rc_ind_hdr_format3_s>>
        c;
  };

  // member variables
  ric_ind_hdr_formats_c_ ric_ind_hdr_formats;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationMessage-Format1-Item ::= SEQUENCE
struct e2sm_rc_ind_msg_format1_item_s {
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationMessage-Format1 ::= SEQUENCE
struct e2sm_rc_ind_msg_format1_s {
  using ran_p_reported_list_l_ = dyn_array<e2sm_rc_ind_msg_format1_item_s>;

  // member variables
  ran_p_reported_list_l_ ran_p_reported_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationMessage-Format2-RANParameter-Item ::= SEQUENCE
struct e2sm_rc_ind_msg_format2_ran_param_item_s {
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationMessage-Format2-Item ::= SEQUENCE
struct e2sm_rc_ind_msg_format2_item_s {
  using ran_p_list_l_ = dyn_array<e2sm_rc_ind_msg_format2_ran_param_item_s>;

  // member variables
  ue_id_c       ue_id;
  ran_p_list_l_ ran_p_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationMessage-Format2 ::= SEQUENCE
struct e2sm_rc_ind_msg_format2_s {
  using ue_param_list_l_ = dyn_array<e2sm_rc_ind_msg_format2_item_s>;

  // member variables
  ue_param_list_l_ ue_param_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// NeighbourCell-Item-Choice-NR ::= SEQUENCE
struct neighbour_cell_item_choice_nr_s {
  struct nr_mode_info_opts {
    enum options { fdd, tdd, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using nr_mode_info_e_ = enumerated<nr_mode_info_opts, true>;
  struct x2_xn_established_opts {
    enum options { true_value, false_value, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using x2_xn_established_e_ = enumerated<x2_xn_established_opts, true>;
  struct ho_validated_opts {
    enum options { true_value, false_value, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using ho_validated_e_ = enumerated<ho_validated_opts, true>;

  // member variables
  nr_cgi_s                 nr_cgi;
  uint16_t                 nr_pci = 0;
  fixed_octstring<3, true> five_gs_tac;
  nr_mode_info_e_          nr_mode_info;
  nr_freq_info_s           nr_freq_info;
  x2_xn_established_e_     x2_xn_established;
  ho_validated_e_          ho_validated;
  uint32_t                 version = 1;
  // ...

  static const asn1::table::seq_desc desc;
};

// NeighbourCell-Item-Choice-E-UTRA ::= SEQUENCE
struct neighbour_cell_item_choice_e_utra_s {
  struct x2_xn_established_opts {
    enum options { true_value, false_value, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using x2_xn_established_e_ = enumerated<x2_xn_established_opts, true>;
  struct ho_validated_opts {
    enum options { true_value, false_value, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using ho_validated_e_ = enumerated<ho_validated_opts, true>;

  // member variables
  eutra_cgi_s              eutra_cgi;
  uint16_t                 eutra_pci   = 0;
  uint32_t                 eutra_arfcn = 0;
  fixed_octstring<2, true> eutra_tac;
  x2_xn_established_e_     x2_xn_established;
  ho_validated_e_          ho_validated;
  uint32_t                 version = 1;
  // ...

  static const asn1::table::seq_desc desc;
};

// NeighbourCell-Item ::= CHOICE
struct neighbour_cell_item_c {
  struct types_opts {
    enum options { ran_type_choice_nr, ran_type_choice_eutra, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  neighbour_cell_item_choice_nr_s&              ran_type_choice_nr()
  {
    assert_choice_type(types::ran_type_choice_nr, c.type, "NeighbourCell-Item");
    return c.get<neighbour_cell_item_choice_nr_s>();
  }
  neighbour_cell_item_choice_e_utra_s& ran_type_choice_eutra()
  {
    assert_choice_type(types::ran_type_choice_eutra, c.type, "NeighbourCell-Item");
    return c.get<neighbour_cell_item_choice_e_utra_s>();
  }
  const neighbour_cell_item_choice_nr_s& ran_type_choice_nr() const
  {
    assert_choice_type(types::ran_type_choice_nr, c.type, "NeighbourCell-Item");
    return c.get<neighbour_cell_item_choice_nr_s>();
  }
  const neighbour_cell_item_choice_e_utra_s& ran_type_choice_eutra() const
  {
    assert_choice_type(types::ran_type_choice_eutra, c.type, "NeighbourCell-Item");
    return c.get<neighbour_cell_item_choice_e_utra_s>();
  }
  neighbour_cell_item_choice_nr_s&     set_ran_type_choice_nr();
  neighbour_cell_item_choice_e_utra_s& set_ran_type_choice_eutra();

private:
  asn1::table::choice_storage<neighbour_cell_item_c,
                              types,
                              choice_buffer_t<neighbour_cell_item_choice_e_utra_s, neighbour_cell_item_choice_nr_s>>
      c;
};

// NeighbourCell-List ::= SEQUENCE (SIZE (1..65535)) OF NeighbourCell-Item
using neighbour_cell_list_l = dyn_array<neighbour_cell_item_c>;

// NeighbourRelation-Info ::= SEQUENCE
struct neighbour_relation_info_s {
  serving_cell_pci_c    serving_cell_pci;
  serving_cell_arfcn_c  serving_cell_arfcn;
  neighbour_cell_list_l neighbour_cell_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// ServingCellConfigCommon ::= SEQUENCE
struct serving_cell_cfg_common_s {
  struct ssb_periodicity_opts {
    enum options { ms5, ms10, ms20, ms40, ms80, ms160, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using ssb_periodicity_e_ = enumerated<ssb_periodicity_opts, true>;
  struct ssb_subcarrier_spacing_opts {
    enum options { khz15, khz30, khz60, khz120, khz240, khz480_v1700, khz960_v1700, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
    uint16_t                            to_number() const;
  };
  using ssb_subcarrier_spacing_e_ = enumerated<ssb_subcarrier_spacing_opts, true>;

  // member variables
  unbounded_octstring<true> ssb_positions_in_burst;
  ssb_periodicity_e_        ssb_periodicity;
  ssb_subcarrier_spacing_e_ ssb_subcarrier_spacing;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationMessage-Format3-Item ::= SEQUENCE
struct e2sm_rc_ind_msg_format3_item_s {
  bool                      cell_deleted_present             = false;
  bool                      neighbour_relation_table_present = false;
  cgi_c                     cell_global_id;
  unbounded_octstring<true> cell_context_info;
  bool                      cell_deleted = false;
  neighbour_relation_info_s neighbour_relation_table;
  // ...
  unbounded_octstring<true>           mib;
  copy_ptr<serving_cell_cfg_common_s> serving_cell_cfg_common;

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationMessage-Format3 ::= SEQUENCE
struct e2sm_rc_ind_msg_format3_s {
  using cell_info_list_l_ = dyn_array<e2sm_rc_ind_msg_format3_item_s>;

  // member variables
  cell_info_list_l_ cell_info_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationMessage-Format5-Item ::= SEQUENCE
struct e2sm_rc_ind_msg_format5_item_s {
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationMessage-Format5 ::= SEQUENCE
struct e2sm_rc_ind_msg_format5_s {
  using ran_p_requested_list_l_ = dyn_array<e2sm_rc_ind_msg_format5_item_s>;

  // member variables
  ran_p_requested_list_l_ ran_p_requested_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationMessage-Format6-RANP-Item ::= SEQUENCE
struct e2sm_rc_ind_msg_format6_ranp_item_s {
  uint64_t               ran_param_id = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationMessage-Format6-Indication-Item ::= SEQUENCE
struct e2sm_rc_ind_msg_format6_ind_item_s {
  using ran_p_insert_ind_list_l_ = dyn_array<e2sm_rc_ind_msg_format6_ranp_item_s>;

  // member variables
  uint32_t                 ric_insert_ind_id = 1;
  ran_p_insert_ind_list_l_ ran_p_insert_ind_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationMessage-Format6-Style-Item ::= SEQUENCE
struct e2sm_rc_ind_msg_format6_style_item_s {
  using ric_insert_ind_list_l_ = dyn_array<e2sm_rc_ind_msg_format6_ind_item_s>;

  // member variables
  int64_t                indicated_insert_style_type = 0;
  ric_insert_ind_list_l_ ric_insert_ind_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationMessage-Format6 ::= SEQUENCE
struct e2sm_rc_ind_msg_format6_s {
  using ric_insert_style_list_l_ = dyn_array<e2sm_rc_ind_msg_format6_style_item_s>;

  // member variables
  ric_insert_style_list_l_ ric_insert_style_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-IndicationMessage ::= SEQUENCE
struct e2sm_rc_ind_msg_s {
  struct ric_ind_msg_formats_c_ {
    struct types_opts {
      enum options {
        ind_msg_format1,
        ind_msg_format2,
        ind_msg_format3,
        ind_msg_format4,
        ind_msg_format5,
        // ...
        ind_msg_format6,
        nulltype
      } value;

      static const asn1::table::enum_desc desc;
      uint8_t                             to_number() const;
    };
    using types = enumerated<types_opts, true, 1>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    e2sm_rc_ind_msg_format1_s&                    ind_msg_format1()
    {
      assert_choice_type(types::ind_msg_format1, c.type, "ric-indicationMessage-formats");
      return c.get<e2sm_rc_ind_msg_format1_s>();
    }
    e2sm_rc_ind_msg_format2_s& ind_msg_format2()
    {
      assert_choice_type(types::ind_msg_format2, c.type, "ric-indicationMessage-formats");
      return c.get<e2sm_rc_ind_msg_format2_s>();
    }
    e2sm_rc_ind_msg_format3_s& ind_msg_format3()
    {
      assert_choice_type(types::ind_msg_format3, c.type, "ric-indicationMessage-formats");
      return c.get<e2sm_rc_ind_msg_format3_s>();
    }
    e2sm_rc_ind_msg_format5_s& ind_msg_format5()
    {
      assert_choice_type(types::ind_msg_format5, c.type, "ric-indicationMessage-formats");
      return c.get<e2sm_rc_ind_msg_format5_s>();
    }
    e2sm_rc_ind_msg_format6_s& ind_msg_format6()
    {
      assert_choice_type(types::ind_msg_format6, c.type, "ric-indicationMessage-formats");
      return c.get<e2sm_rc_ind_msg_format6_s>();
    }
    const e2sm_rc_ind_msg_format1_s& ind_msg_format1() const
    {
      assert_choice_type(types::ind_msg_format1, c.type, "ric-indicationMessage-formats");
      return c.get<e2sm_rc_ind_msg_format1_s>();
    }
    const e2sm_rc_ind_msg_format2_s& ind_msg_format2() const
    {
      assert_choice_type(types::ind_msg_format2, c.type, "ric-indicationMessage-formats");
      return c.get<e2sm_rc_ind_msg_format2_s>();
    }
    const e2sm_rc_ind_msg_format3_s& ind_msg_format3() const
    {
      assert_choice_type(types::ind_msg_format3, c.type, "ric-indicationMessage-formats");
      return c.get<e2sm_rc_ind_msg_format3_s>();
    }
    const e2sm_rc_ind_msg_format5_s& ind_msg_format5() const
    {
      assert_choice_type(types::ind_msg_format5, c.type, "ric-indicationMessage-formats");
      return c.get<e2sm_rc_ind_msg_format5_s>();
    }
    const e2sm_rc_ind_msg_format6_s& ind_msg_format6() const
    {
      assert_choice_type(types::ind_msg_format6, c.type, "ric-indicationMessage-formats");
      return c.get<e2sm_rc_ind_msg_format6_s>();
    }
    e2sm_rc_ind_msg_format1_s& set_ind_msg_format1();
    e2sm_rc_ind_msg_format2_s& set_ind_msg_format2();
    e2sm_rc_ind_msg_format3_s& set_ind_msg_format3();
    void                       set_ind_msg_format4();
    e2sm_rc_ind_msg_format5_s& set_ind_msg_format5();
    e2sm_rc_ind_msg_format6_s& set_ind_msg_format6();

  private:
    asn1::table::choice_storage<ric_ind_msg_formats_c_,
                                types,
                                choice_buffer_t<e2sm_rc_ind_msg_format1_s,
                                                e2sm_rc_ind_msg_format2_s,
                                                e2sm_rc_ind_msg_format3_s,
                                                e2sm_rc_ind_msg_format5_s,
                                                e2sm_rc_ind_msg_format6_s>>
        c;
  };

  // member variables
  ric_ind_msg_formats_c_ ric_ind_msg_formats;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-QueryDefinition-Format1-Item ::= SEQUENCE
struct e2sm_rc_query_definition_format1_item_s {
  bool                   ran_param_definition_present = false;
  uint64_t               ran_param_id                 = 1;
  ran_param_definition_s ran_param_definition;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-QueryDefinition-Format1 ::= SEQUENCE
struct e2sm_rc_query_definition_format1_s {
  using ran_p_list_l_ = dyn_array<e2sm_rc_query_definition_format1_item_s>;

  // member variables
  ran_p_list_l_ ran_p_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-QueryDefinition ::= SEQUENCE
struct e2sm_rc_query_definition_s {
  struct ric_query_definition_formats_c_ {
    struct types_opts {
      enum options { query_request_format1, /*...*/ nulltype } value;

      static const asn1::table::enum_desc desc;
      uint8_t                             to_number() const;
    };
    using types = enumerated<types_opts, true>;

    types                                     type() const { return types::query_request_format1; }
    static const asn1::table::choice_desc     desc;
    e2sm_rc_query_definition_format1_s&       query_request_format1() { return c; }
    const e2sm_rc_query_definition_format1_s& query_request_format1() const { return c; }

  private:
    e2sm_rc_query_definition_format1_s c;
  };

  // member variables
  ric_query_definition_formats_c_ ric_query_definition_formats;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-QueryHeader-Format1 ::= SEQUENCE
struct e2sm_rc_query_hdr_format1_s {
  bool                 associated_ue_info_present = false;
  int64_t              ric_style_type             = 0;
  ran_param_testing_l  associated_e2_node_info;
  associated_ue_info_s associated_ue_info;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-QueryHeader ::= SEQUENCE
struct e2sm_rc_query_hdr_s {
  struct ric_query_hdr_formats_c_ {
    struct types_opts {
      enum options { query_hdr_format1, /*...*/ nulltype } value;

      static const asn1::table::enum_desc desc;
      uint8_t                             to_number() const;
    };
    using types = enumerated<types_opts, true>;

    types                                 type() const { return types::query_hdr_format1; }
    static const asn1::table::choice_desc desc;
    e2sm_rc_query_hdr_format1_s&          query_hdr_format1() { return c; }
    const e2sm_rc_query_hdr_format1_s&    query_hdr_format1() const { return c; }

  private:
    e2sm_rc_query_hdr_format1_s c;
  };

  // member variables
  ric_query_hdr_formats_c_ ric_query_hdr_formats;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-QueryOutcome-Format1-ItemParameters ::= SEQUENCE
struct e2sm_rc_query_outcome_format1_item_params_s {
  bool                   ran_param_value_type_present = false;
  uint64_t               ran_param_id                 = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-QueryOutcome-Format1-ItemCell ::= SEQUENCE
struct e2sm_rc_query_outcome_format1_item_cell_s {
  using ran_p_list_l_ = dyn_array<e2sm_rc_query_outcome_format1_item_params_s>;

  // member variables
  bool                      neighbour_relation_table_present = false;
  cgi_c                     cell_global_id;
  ran_p_list_l_             ran_p_list;
  neighbour_relation_info_s neighbour_relation_table;
  // ...
  copy_ptr<serving_cell_cfg_common_s> serving_cell_cfg_common;

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-QueryOutcome-Format1 ::= SEQUENCE
struct e2sm_rc_query_outcome_format1_s {
  using cell_info_list_l_ = dyn_array<e2sm_rc_query_outcome_format1_item_cell_s>;

  // member variables
  cell_info_list_l_ cell_info_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-QueryOutcome-Format2-ItemParameters ::= SEQUENCE
struct e2sm_rc_query_outcome_format2_item_params_s {
  bool                   ran_param_value_type_present = false;
  uint64_t               ran_param_id                 = 1;
  ran_param_value_type_c ran_param_value_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-QueryOutcome-Format2-ItemUE ::= SEQUENCE
struct e2sm_rc_query_outcome_format2_item_ue_s {
  using ran_p_list_l_ = dyn_array<e2sm_rc_query_outcome_format2_item_params_s>;

  // member variables
  bool          ue_filt_id_present = false;
  ue_id_c       ue_id;
  ran_p_list_l_ ran_p_list;
  uint32_t      ue_filt_id = 1;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-QueryOutcome-Format2 ::= SEQUENCE
struct e2sm_rc_query_outcome_format2_s {
  using ue_info_list_l_ = dyn_array<e2sm_rc_query_outcome_format2_item_ue_s>;

  // member variables
  ue_info_list_l_ ue_info_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-QueryOutcome ::= SEQUENCE
struct e2sm_rc_query_outcome_s {
  struct ric_query_outcome_formats_c_ {
    struct types_opts {
      enum options { query_outcome_format1, query_outcome_format2, /*...*/ nulltype } value;

      static const asn1::table::enum_desc desc;
      uint8_t                             to_number() const;
    };
    using types = enumerated<types_opts, true>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    e2sm_rc_query_outcome_format1_s&              query_outcome_format1()
    {
      assert_choice_type(types::query_outcome_format1, c.type, "ric-queryOutcome-formats");
      return c.get<e2sm_rc_query_outcome_format1_s>();
    }
    e2sm_rc_query_outcome_format2_s& query_outcome_format2()
    {
      assert_choice_type(types::query_outcome_format2, c.type, "ric-queryOutcome-formats");
      return c.get<e2sm_rc_query_outcome_format2_s>();
    }
    const e2sm_rc_query_outcome_format1_s& query_outcome_format1() const
    {
      assert_choice_type(types::query_outcome_format1, c.type, "ric-queryOutcome-formats");
      return c.get<e2sm_rc_query_outcome_format1_s>();
    }
    const e2sm_rc_query_outcome_format2_s& query_outcome_format2() const
    {
      assert_choice_type(types::query_outcome_format2, c.type, "ric-queryOutcome-formats");
      return c.get<e2sm_rc_query_outcome_format2_s>();
    }
    e2sm_rc_query_outcome_format1_s& set_query_outcome_format1();
    e2sm_rc_query_outcome_format2_s& set_query_outcome_format2();

  private:
    asn1::table::choice_storage<ric_query_outcome_formats_c_,
                                types,
                                choice_buffer_t<e2sm_rc_query_outcome_format1_s, e2sm_rc_query_outcome_format2_s>>
        c;
  };

  // member variables
  ric_query_outcome_formats_c_ ric_query_outcome_formats;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-EventTrigger-Style-Item ::= SEQUENCE
struct ran_function_definition_event_trigger_style_item_s {
  int64_t                              ric_event_trigger_style_type = 0;
  printable_string<1, 150, true, true> ric_event_trigger_style_name;
  int64_t                              ric_event_trigger_format_type = 0;
  // ...

  static const asn1::table::seq_desc desc;
};

// L2Parameters-RANParameter-Item ::= SEQUENCE
struct l2_params_ran_param_item_s {
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-EventTrigger-Breakpoint-Item ::= SEQUENCE
struct ran_function_definition_event_trigger_breakpoint_item_s {
  using ran_call_process_breakpoint_params_list_l_ = dyn_array<call_process_breakpoint_ran_param_item_s>;

  // member variables
  uint32_t                                   call_process_breakpoint_id = 1;
  printable_string<1, 150, true, true>       call_process_breakpoint_name;
  ran_call_process_breakpoint_params_list_l_ ran_call_process_breakpoint_params_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-EventTrigger-CallProcess-Item ::= SEQUENCE
struct ran_function_definition_event_trigger_call_process_item_s {
  using call_process_breakpoints_list_l_ = dyn_array<ran_function_definition_event_trigger_breakpoint_item_s>;

  // member variables
  uint32_t                             call_process_type_id = 1;
  printable_string<1, 150, true, true> call_process_type_name;
  call_process_breakpoints_list_l_     call_process_breakpoints_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// UEIdentification-RANParameter-Item ::= SEQUENCE
struct ue_identif_ran_param_item_s {
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-EventTrigger ::= SEQUENCE
struct ran_function_definition_event_trigger_s {
  using ric_event_trigger_style_list_l_ = dyn_array<ran_function_definition_event_trigger_style_item_s>;
  using ran_l2_params_list_l_           = dyn_array<l2_params_ran_param_item_s>;
  using ran_call_process_types_list_l_  = dyn_array<ran_function_definition_event_trigger_call_process_item_s>;
  using ran_ue_identif_params_list_l_   = dyn_array<ue_identif_ran_param_item_s>;
  using ran_cell_identif_params_list_l_ = dyn_array<cell_identif_ran_param_item_s>;

  // member variables
  ric_event_trigger_style_list_l_ ric_event_trigger_style_list;
  ran_l2_params_list_l_           ran_l2_params_list;
  ran_call_process_types_list_l_  ran_call_process_types_list;
  ran_ue_identif_params_list_l_   ran_ue_identif_params_list;
  ran_cell_identif_params_list_l_ ran_cell_identif_params_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// Report-RANParameter-Item ::= SEQUENCE
struct report_ran_param_item_s {
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-Report-Item ::= SEQUENCE
struct ran_function_definition_report_item_s {
  using ran_report_params_list_l_ = dyn_array<report_ran_param_item_s>;

  // member variables
  int64_t                              ric_report_style_type = 0;
  printable_string<1, 150, true, true> ric_report_style_name;
  int64_t                              ric_supported_event_trigger_style_type = 0;
  int64_t                              ric_report_action_format_type          = 0;
  int64_t                              ric_ind_hdr_format_type                = 0;
  int64_t                              ric_ind_msg_format_type                = 0;
  ran_report_params_list_l_            ran_report_params_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-Report ::= SEQUENCE
struct ran_function_definition_report_s {
  using ric_report_style_list_l_ = dyn_array<ran_function_definition_report_item_s>;

  // member variables
  ric_report_style_list_l_ ric_report_style_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// InsertIndication-RANParameter-Item ::= SEQUENCE
struct insert_ind_ran_param_item_s {
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-Insert-Indication-Item ::= SEQUENCE
struct ran_function_definition_insert_ind_item_s {
  using ran_insert_ind_params_list_l_ = dyn_array<insert_ind_ran_param_item_s>;

  // member variables
  uint32_t                             ric_insert_ind_id = 1;
  printable_string<1, 150, true, true> ric_insert_ind_name;
  ran_insert_ind_params_list_l_        ran_insert_ind_params_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-Insert-Item ::= SEQUENCE
struct ran_function_definition_insert_item_s {
  using ric_insert_ind_list_l_ = dyn_array<ran_function_definition_insert_ind_item_s>;

  // member variables
  int64_t                              ric_insert_style_type = 0;
  printable_string<1, 150, true, true> ric_insert_style_name;
  int64_t                              ric_supported_event_trigger_style_type = 0;
  int64_t                              ric_action_definition_format_type      = 0;
  ric_insert_ind_list_l_               ric_insert_ind_list;
  int64_t                              ric_ind_hdr_format_type         = 0;
  int64_t                              ric_ind_msg_format_type         = 0;
  int64_t                              ric_call_process_id_format_type = 0;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-Insert ::= SEQUENCE
struct ran_function_definition_insert_s {
  using ric_insert_style_list_l_ = dyn_array<ran_function_definition_insert_item_s>;

  // member variables
  ric_insert_style_list_l_ ric_insert_style_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-Control-Action-Item ::= SEQUENCE
struct ran_function_definition_ctrl_action_item_s {
  using ran_ctrl_action_params_list_l_ = dyn_array<ctrl_action_ran_param_item_s>;
  struct ue_group_ctrl_action_supported_opts {
    enum options { true_value, false_value, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using ue_group_ctrl_action_supported_e_ = enumerated<ue_group_ctrl_action_supported_opts, true>;

  // member variables
  uint32_t                             ric_ctrl_action_id = 1;
  printable_string<1, 150, true, true> ric_ctrl_action_name;
  ran_ctrl_action_params_list_l_       ran_ctrl_action_params_list;
  // ...
  bool                              ue_group_ctrl_action_supported_present = false;
  ue_group_ctrl_action_supported_e_ ue_group_ctrl_action_supported;

  static const asn1::table::seq_desc desc;
};

// ListOfAdditionalSupportedFormats ::= SEQUENCE (SIZE (0..63)) OF AdditionalSupportedFormat
using list_of_add_supported_formats_l = dyn_array<add_supported_format_s>;

// RANFunctionDefinition-Control-Item ::= SEQUENCE
struct ran_function_definition_ctrl_item_s {
  using ric_ctrl_action_list_l_         = dyn_array<ran_function_definition_ctrl_action_item_s>;
  using ran_ctrl_outcome_params_list_l_ = dyn_array<ctrl_outcome_ran_param_item_s>;

  // member variables
  bool                                 ric_call_process_id_format_type_present = false;
  int64_t                              ric_ctrl_style_type                     = 0;
  printable_string<1, 150, true, true> ric_ctrl_style_name;
  ric_ctrl_action_list_l_              ric_ctrl_action_list;
  int64_t                              ric_ctrl_hdr_format_type        = 0;
  int64_t                              ric_ctrl_msg_format_type        = 0;
  int64_t                              ric_call_process_id_format_type = 0;
  int64_t                              ric_ctrl_outcome_format_type    = 0;
  ran_ctrl_outcome_params_list_l_      ran_ctrl_outcome_params_list;
  // ...
  copy_ptr<list_of_add_supported_formats_l> list_of_add_supported_formats;

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-Control ::= SEQUENCE
struct ran_function_definition_ctrl_s {
  using ric_ctrl_style_list_l_ = dyn_array<ran_function_definition_ctrl_item_s>;

  // member variables
  ric_ctrl_style_list_l_ ric_ctrl_style_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// PolicyAction-RANParameter-Item ::= SEQUENCE
struct policy_action_ran_param_item_s {
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  static const asn1::table::seq_desc desc;
};

// PolicyCondition-RANParameter-Item ::= SEQUENCE
struct policy_condition_ran_param_item_s {
  uint64_t                             ran_param_id = 1;
  printable_string<1, 150, true, true> ran_param_name;
  // ...
  copy_ptr<ran_param_definition_s> ran_param_definition;

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-Policy-Action-Item ::= SEQUENCE
struct ran_function_definition_policy_action_item_s {
  using ran_policy_action_params_list_l_    = dyn_array<policy_action_ran_param_item_s>;
  using ran_policy_condition_params_list_l_ = dyn_array<policy_condition_ran_param_item_s>;

  // member variables
  uint32_t                             ric_policy_action_id = 1;
  printable_string<1, 150, true, true> ric_policy_action_name;
  int64_t                              ric_action_definition_format_type = 0;
  ran_policy_action_params_list_l_     ran_policy_action_params_list;
  ran_policy_condition_params_list_l_  ran_policy_condition_params_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-Policy-Item ::= SEQUENCE
struct ran_function_definition_policy_item_s {
  using ric_policy_action_list_l_ = dyn_array<ran_function_definition_policy_action_item_s>;

  // member variables
  int64_t                              ric_policy_style_type = 0;
  printable_string<1, 150, true, true> ric_policy_style_name;
  int64_t                              ric_supported_event_trigger_style_type = 0;
  ric_policy_action_list_l_            ric_policy_action_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-Policy ::= SEQUENCE
struct ran_function_definition_policy_s {
  using ric_policy_style_list_l_ = dyn_array<ran_function_definition_policy_item_s>;

  // member variables
  ric_policy_style_list_l_ ric_policy_style_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// Query-RANParameter-Item ::= SEQUENCE
struct query_ran_param_item_s {
  bool                                 ran_param_definition_present = false;
  uint64_t                             ran_param_id                 = 1;
  printable_string<1, 150, true, true> ran_param_name;
  ran_param_definition_s               ran_param_definition;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-Query-Item ::= SEQUENCE
struct ran_function_definition_query_item_s {
  using ran_query_params_list_l_ = dyn_array<query_ran_param_item_s>;

  // member variables
  int64_t                              ric_query_style_type = 0;
  printable_string<1, 150, true, true> ric_query_style_name;
  int64_t                              ric_query_hdr_format_type        = 0;
  int64_t                              ric_query_definition_format_type = 0;
  int64_t                              ric_query_outcome_format_type    = 0;
  ran_query_params_list_l_             ran_query_params_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// RANFunctionDefinition-Query ::= SEQUENCE
struct ran_function_definition_query_s {
  using ric_query_style_list_l_ = dyn_array<ran_function_definition_query_item_s>;

  // member variables
  ric_query_style_list_l_ ric_query_style_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-RANFunctionDefinition ::= SEQUENCE
struct e2sm_rc_ran_function_definition_s {
  bool                                    ran_function_definition_event_trigger_present = false;
  bool                                    ran_function_definition_report_present        = false;
  bool                                    ran_function_definition_insert_present        = false;
  bool                                    ran_function_definition_ctrl_present          = false;
  bool                                    ran_function_definition_policy_present        = false;
  ra_nfunction_name_s                     ran_function_name;
  ran_function_definition_event_trigger_s ran_function_definition_event_trigger;
  ran_function_definition_report_s        ran_function_definition_report;
  ran_function_definition_insert_s        ran_function_definition_insert;
  ran_function_definition_ctrl_s          ran_function_definition_ctrl;
  ran_function_definition_policy_s        ran_function_definition_policy;
  // ...
  copy_ptr<ran_function_definition_query_s> ran_function_definition_query;

  static const asn1::table::seq_desc desc;
};

// E2SM-RC-ServiceLayerCause ::= CHOICE
struct e2sm_rc_service_layer_cause_c {
  struct types_opts {
    enum options { ric_request, ric_service, protocol, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  cause_ri_cservice_e&                          ric_service()
  {
    assert_choice_type(types::ric_service, c.type, "E2SM-RC-ServiceLayerCause");
    return c.get<cause_ri_cservice_e>();
  }
  cause_protocol_e& protocol()
  {
    assert_choice_type(types::protocol, c.type, "E2SM-RC-ServiceLayerCause");
    return c.get<cause_protocol_e>();
  }
  const cause_ri_cservice_e& ric_service() const
  {
    assert_choice_type(types::ric_service, c.type, "E2SM-RC-ServiceLayerCause");
    return c.get<cause_ri_cservice_e>();
  }
  const cause_protocol_e& protocol() const
  {
    assert_choice_type(types::protocol, c.type, "E2SM-RC-ServiceLayerCause");
    return c.get<cause_protocol_e>();
  }
  void                 set_ric_request();
  cause_ri_cservice_e& set_ric_service();
  cause_protocol_e&    set_protocol();

private:
  asn1::table::
      choice_storage<e2sm_rc_service_layer_cause_c, types, choice_buffer_t<cause_protocol_e, cause_ri_cservice_e>>
          c;
};

} // namespace e2sm
} // namespace asn1

extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_param_testing_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::associated_ue_info_s::associated_ue_info_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_param_definition_choice_list_s::ran_param_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_param_definition_choice_structure_s::ran_param_structure_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_action_definition_format1_s::ran_p_to_be_reported_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_param_structure_s::seq_of_ran_params_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_param_list_s::list_of_ran_param_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ric_policy_action_s::ran_params_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_action_definition_format2_s::ric_policy_conditions_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_action_definition_format3_s::ran_p_insert_ind_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<
    asn1::e2sm::e2sm_rc_action_definition_format4_ind_item_s::ran_p_insert_ind_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<
    asn1::e2sm::e2sm_rc_action_definition_format4_style_item_s::ric_insert_ind_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_action_definition_format4_s::ric_insert_style_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ue_group_definition_s::ue_group_definition_id_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ctrl_msg_format1_s::ran_p_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ctrl_msg_format2_style_item_s::ric_ctrl_action_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ctrl_msg_format2_s::ric_ctrl_style_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_entity_filt_s::entity_specific_ctrl_ran_p_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ctrl_msg_format3_s::list_of_entity_filts_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ctrl_msg_format3_s::entity_agnostic_ctrl_ran_p_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ctrl_msg_format4_s::ran_p_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ctrl_outcome_format1_s::ran_p_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ctrl_outcome_format2_ctrl_outcome_item_s::ran_p_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ctrl_outcome_format2_style_item_s::ric_ctrl_outcome_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ctrl_outcome_format2_s::ric_ctrl_style_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ctrl_outcome_format3_s::ran_p_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::event_trigger_ue_info_s::ue_info_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::event_trigger_ue_event_info_s::ue_event_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_event_trigger_format1_s::msg_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::event_trigger_cell_info_s::cell_info_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_event_trigger_format3_s::e2_node_info_change_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::trigger_type_choice_rrc_state_s::rrc_state_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_event_trigger_format4_s::ue_info_change_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ind_msg_format1_s::ran_p_reported_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ind_msg_format2_item_s::ran_p_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ind_msg_format2_s::ue_param_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm::neighbour_cell_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ind_msg_format3_s::cell_info_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ind_msg_format5_s::ran_p_requested_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ind_msg_format6_ind_item_s::ran_p_insert_ind_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ind_msg_format6_style_item_s::ric_insert_ind_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_ind_msg_format6_s::ric_insert_style_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_query_definition_format1_s::ran_p_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_query_outcome_format1_item_cell_s::ran_p_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_query_outcome_format1_s::cell_info_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_query_outcome_format2_item_ue_s::ran_p_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::e2sm_rc_query_outcome_format2_s::ue_info_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<
    asn1::e2sm::ran_function_definition_event_trigger_s::ric_event_trigger_style_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_function_definition_event_trigger_s::ran_l2_params_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_function_definition_event_trigger_breakpoint_item_s::
                        ran_call_process_breakpoint_params_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_function_definition_event_trigger_call_process_item_s::
                        call_process_breakpoints_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<
    asn1::e2sm::ran_function_definition_event_trigger_s::ran_call_process_types_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<
    asn1::e2sm::ran_function_definition_event_trigger_s::ran_ue_identif_params_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<
    asn1::e2sm::ran_function_definition_event_trigger_s::ran_cell_identif_params_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_function_definition_report_item_s::ran_report_params_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_function_definition_report_s::ric_report_style_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<
    asn1::e2sm::ran_function_definition_insert_ind_item_s::ran_insert_ind_params_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_function_definition_insert_item_s::ric_insert_ind_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_function_definition_insert_s::ric_insert_style_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<
    asn1::e2sm::ran_function_definition_ctrl_action_item_s::ran_ctrl_action_params_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_function_definition_ctrl_item_s::ric_ctrl_action_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_function_definition_ctrl_item_s::ran_ctrl_outcome_params_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::list_of_add_supported_formats_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_function_definition_ctrl_s::ric_ctrl_style_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<
    asn1::e2sm::ran_function_definition_policy_action_item_s::ran_policy_action_params_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<
    asn1::e2sm::ran_function_definition_policy_action_item_s::ran_policy_condition_params_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_function_definition_policy_item_s::ric_policy_action_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_function_definition_policy_s::ric_policy_style_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_function_definition_query_item_s::ran_query_params_list_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ran_function_definition_query_s::ric_query_style_list_l_::value_type>::ops;
