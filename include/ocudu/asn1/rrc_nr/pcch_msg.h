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

#include "pcch_msg_ies.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// PCCH-MessageType ::= CHOICE
struct pcch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { paging, spare1, nulltype } value;

      const char* to_string() const;
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
    paging_s& paging()
    {
      assert_choice_type(types::paging, type_, "c1");
      return c;
    }
    const paging_s& paging() const
    {
      assert_choice_type(types::paging, type_, "c1");
      return c;
    }
    paging_s& set_paging();
    void      set_spare1();

  private:
    types    type_;
    paging_s c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;
    typedef uint8_t number_type;

    const char* to_string() const;
    uint8_t     to_number() const;
  };
  using types = enumerated<types_opts>;

  // choice methods
  pcch_msg_type_c() = default;
  void          set(types::options e = types::nulltype);
  types         type() const { return type_; }
  OCUDUASN_CODE pack(bit_ref& bref) const;
  OCUDUASN_CODE unpack(cbit_ref& bref);
  void          to_json(json_writer& j) const;
  // getters
  c1_c_& c1()
  {
    assert_choice_type(types::c1, type_, "PCCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "PCCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// PCCH-Message ::= SEQUENCE
struct pcch_msg_s {
  pcch_msg_type_c msg;

  // sequence methods
  OCUDUASN_CODE pack(bit_ref& bref) const;
  OCUDUASN_CODE unpack(cbit_ref& bref);
  void          to_json(json_writer& j) const;
};

} // namespace rrc_nr
} // namespace asn1
