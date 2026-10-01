// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/*******************************************************************************
 *
 *                    3GPP TS ASN1 RRC NR v18.8.0 (2025-12)
 *
 ******************************************************************************/

#pragma once

#include "ocudu/asn1/asn1_utils.h"

namespace asn1 {
namespace rrc_nr {

struct sys_info_s;
struct sib1_s;

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// BCCH-DL-SCH-MessageType ::= CHOICE
struct bcch_dl_sch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { sys_info, sib_type1, nulltype } value;
      typedef uint8_t number_type;

      const char* to_string() const;
      uint8_t     to_number() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    c1_c_() = default;
    void          set(types::options e = types::nulltype);
    types         type() const { return type_; }
    OCUDUASN_CODE pack(bit_ref& bref) const;
    OCUDUASN_CODE unpack(cbit_ref& bref);
    void          to_json(json_writer& j) const;
    // getters
    sys_info_s& sys_info()
    {
      assert_choice_type(types::sys_info, type_, "c1");
      return c.get<sys_info_s>();
    }
    sib1_s& sib_type1()
    {
      assert_choice_type(types::sib_type1, type_, "c1");
      return c.get<sib1_s>();
    }
    const sys_info_s& sys_info() const
    {
      assert_choice_type(types::sys_info, type_, "c1");
      return c.get<sys_info_s>();
    }
    const sib1_s& sib_type1() const
    {
      assert_choice_type(types::sib_type1, type_, "c1");
      return c.get<sib1_s>();
    }
    sys_info_s& set_sys_info();
    sib1_s&     set_sib_type1();

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  bcch_dl_sch_msg_type_c() = default;
  void          set(types::options e = types::nulltype);
  types         type() const { return type_; }
  OCUDUASN_CODE pack(bit_ref& bref) const;
  OCUDUASN_CODE unpack(cbit_ref& bref);
  void          to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "BCCH-DL-SCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "BCCH-DL-SCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// BCCH-DL-SCH-Message ::= SEQUENCE
struct bcch_dl_sch_msg_s {
  bcch_dl_sch_msg_type_c msg;

  // sequence methods
  OCUDUASN_CODE pack(bit_ref& bref) const;
  OCUDUASN_CODE unpack(cbit_ref& bref);
  void          to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
