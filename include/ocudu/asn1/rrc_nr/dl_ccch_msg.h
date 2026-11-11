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

#include "ocudu/asn1/asn1_rrc_utils.h"

namespace asn1 {
namespace rrc_nr {

struct rrc_reject_s;
struct rrc_setup_s;

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// DL-CCCH-MessageType ::= CHOICE
struct dl_ccch_msg_type_c {
  struct c1_c_ {
    struct types_opts {
      enum options { rrc_reject, rrc_setup, spare2, spare1, nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    void                                  set(types::options e = types::nulltype);
    types                                 type() const { return type_; }
    static const asn1::table::choice_desc desc;
    rrc_reject_s&                         rrc_reject()
    {
      assert_choice_type(types::rrc_reject, type_, "c1");
      return c.get<rrc_reject_s>();
    }
    rrc_setup_s& rrc_setup()
    {
      assert_choice_type(types::rrc_setup, type_, "c1");
      return c.get<rrc_setup_s>();
    }
    const rrc_reject_s& rrc_reject() const
    {
      assert_choice_type(types::rrc_reject, type_, "c1");
      return c.get<rrc_reject_s>();
    }
    const rrc_setup_s& rrc_setup() const
    {
      assert_choice_type(types::rrc_setup, type_, "c1");
      return c.get<rrc_setup_s>();
    }
    rrc_reject_s& set_rrc_reject();
    rrc_setup_s&  set_rrc_setup();
    void          set_spare2();
    void          set_spare1();

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  struct types_opts {
    enum options { c1, msg_class_ext, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using types = enumerated<types_opts>;

  void                                  set(types::options e = types::nulltype);
  types                                 type() const { return type_; }
  static const asn1::table::choice_desc desc;
  c1_c_&                                c1()
  {
    assert_choice_type(types::c1, type_, "DL-CCCH-MessageType");
    return c;
  }
  const c1_c_& c1() const
  {
    assert_choice_type(types::c1, type_, "DL-CCCH-MessageType");
    return c;
  }
  c1_c_& set_c1();
  void   set_msg_class_ext();

private:
  types type_;
  c1_c_ c;
};

// DL-CCCH-Message ::= SEQUENCE
struct dl_ccch_msg_s {
  dl_ccch_msg_type_c msg;

  static const asn1::table::seq_desc desc;
};

} // namespace rrc_nr
} // namespace asn1
