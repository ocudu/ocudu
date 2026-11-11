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

#include "ocudu/asn1/asn1_ap_utils.h"

namespace asn1 {
namespace e2sm {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// Beam-ID ::= CHOICE
struct beam_id_c {
  struct types_opts {
    enum options { nr_beam_id, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  types                                 type() const { return types::nr_beam_id; }
  static const asn1::table::choice_desc desc;
  uint8_t&                              nr_beam_id() { return c; }
  const uint8_t&                        nr_beam_id() const { return c; }

private:
  uint8_t c;
};

// NR-CGI ::= SEQUENCE
struct nr_cgi_s {
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<36, false, true> nr_cell_id;
  // ...

  static const asn1::table::seq_desc desc;
};

// EUTRA-CGI ::= SEQUENCE
struct eutra_cgi_s {
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<28, false, true> eutra_cell_id;
  // ...

  static const asn1::table::seq_desc desc;
};

// CGI ::= CHOICE
struct cgi_c {
  struct types_opts {
    enum options { nr_cgi, eutra_cgi, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  nr_cgi_s&                                     nr_cgi()
  {
    assert_choice_type(types::nr_cgi, c.type, "CGI");
    return c.get<nr_cgi_s>();
  }
  eutra_cgi_s& eutra_cgi()
  {
    assert_choice_type(types::eutra_cgi, c.type, "CGI");
    return c.get<eutra_cgi_s>();
  }
  const nr_cgi_s& nr_cgi() const
  {
    assert_choice_type(types::nr_cgi, c.type, "CGI");
    return c.get<nr_cgi_s>();
  }
  const eutra_cgi_s& eutra_cgi() const
  {
    assert_choice_type(types::eutra_cgi, c.type, "CGI");
    return c.get<eutra_cgi_s>();
  }
  nr_cgi_s&    set_nr_cgi();
  eutra_cgi_s& set_eutra_cgi();

private:
  asn1::table::choice_storage<cgi_c, types, choice_buffer_t<eutra_cgi_s, nr_cgi_s>> c;
};

// Cell-RNTI ::= SEQUENCE
struct cell_rnti_s {
  uint32_t c_rnti = 0;
  cgi_c    cell_global_id;
  // ...

  static const asn1::table::seq_desc desc;
};

// GUAMI ::= SEQUENCE
struct guami_s {
  fixed_octstring<3, true>         plmn_id;
  fixed_bitstring<8, false, true>  amf_region_id;
  fixed_bitstring<10, false, true> amf_set_id;
  fixed_bitstring<6, false, true>  amf_pointer;
  // ...

  static const asn1::table::seq_desc desc;
};

// GUMMEI ::= SEQUENCE
struct gummei_s {
  fixed_octstring<3, true> plmn_id;
  fixed_octstring<2, true> mme_group_id;
  fixed_octstring<1, true> mme_code;
  // ...

  static const asn1::table::seq_desc desc;
};

// CoreCPID ::= CHOICE
struct core_cp_id_c {
  struct types_opts {
    enum options { five_gc, epc, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  guami_s&                                      five_gc()
  {
    assert_choice_type(types::five_gc, c.type, "CoreCPID");
    return c.get<guami_s>();
  }
  gummei_s& epc()
  {
    assert_choice_type(types::epc, c.type, "CoreCPID");
    return c.get<gummei_s>();
  }
  const guami_s& five_gc() const
  {
    assert_choice_type(types::five_gc, c.type, "CoreCPID");
    return c.get<guami_s>();
  }
  const gummei_s& epc() const
  {
    assert_choice_type(types::epc, c.type, "CoreCPID");
    return c.get<gummei_s>();
  }
  guami_s&  set_five_gc();
  gummei_s& set_epc();

private:
  asn1::table::choice_storage<core_cp_id_c, types, choice_buffer_t<guami_s, gummei_s>> c;
};

// EN-GNB-ID ::= CHOICE
struct en_gnb_id_c {
  struct types_opts {
    enum options { en_gnb_id, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  types                                         type() const { return types::en_gnb_id; }
  static const asn1::table::choice_desc         desc;
  bounded_bitstring<22, 32, false, true>&       en_gnb_id() { return c; }
  const bounded_bitstring<22, 32, false, true>& en_gnb_id() const { return c; }

private:
  bounded_bitstring<22, 32, false, true> c;
};

// ENB-ID ::= CHOICE
struct enb_id_c {
  struct types_opts {
    enum options { macro_enb_id, home_enb_id, /*...*/ short_macro_enb_id, long_macro_enb_id, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true, 2>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  fixed_bitstring<20, false, true>&             macro_enb_id()
  {
    assert_choice_type(types::macro_enb_id, c.type, "ENB-ID");
    return c.get<fixed_bitstring<20, false, true>>();
  }
  fixed_bitstring<28, false, true>& home_enb_id()
  {
    assert_choice_type(types::home_enb_id, c.type, "ENB-ID");
    return c.get<fixed_bitstring<28, false, true>>();
  }
  fixed_bitstring<18, false, true>& short_macro_enb_id()
  {
    assert_choice_type(types::short_macro_enb_id, c.type, "ENB-ID");
    return c.get<fixed_bitstring<18, false, true>>();
  }
  fixed_bitstring<21, false, true>& long_macro_enb_id()
  {
    assert_choice_type(types::long_macro_enb_id, c.type, "ENB-ID");
    return c.get<fixed_bitstring<21, false, true>>();
  }
  const fixed_bitstring<20, false, true>& macro_enb_id() const
  {
    assert_choice_type(types::macro_enb_id, c.type, "ENB-ID");
    return c.get<fixed_bitstring<20, false, true>>();
  }
  const fixed_bitstring<28, false, true>& home_enb_id() const
  {
    assert_choice_type(types::home_enb_id, c.type, "ENB-ID");
    return c.get<fixed_bitstring<28, false, true>>();
  }
  const fixed_bitstring<18, false, true>& short_macro_enb_id() const
  {
    assert_choice_type(types::short_macro_enb_id, c.type, "ENB-ID");
    return c.get<fixed_bitstring<18, false, true>>();
  }
  const fixed_bitstring<21, false, true>& long_macro_enb_id() const
  {
    assert_choice_type(types::long_macro_enb_id, c.type, "ENB-ID");
    return c.get<fixed_bitstring<21, false, true>>();
  }
  fixed_bitstring<20, false, true>& set_macro_enb_id();
  fixed_bitstring<28, false, true>& set_home_enb_id();
  fixed_bitstring<18, false, true>& set_short_macro_enb_id();
  fixed_bitstring<21, false, true>& set_long_macro_enb_id();

private:
  asn1::table::choice_storage<enb_id_c, types, choice_buffer_t<fixed_bitstring<28, false, true>>> c;
};

// FreqBandNrItem ::= SEQUENCE
struct freq_band_nr_item_s {
  uint16_t freq_band_ind_nr = 1;
  // ...

  static const asn1::table::seq_desc desc;
};

// GNB-ID ::= CHOICE
struct gnb_id_c {
  struct types_opts {
    enum options { gnb_id, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  types                                         type() const { return types::gnb_id; }
  static const asn1::table::choice_desc         desc;
  bounded_bitstring<22, 32, false, true>&       gnb_id() { return c; }
  const bounded_bitstring<22, 32, false, true>& gnb_id() const { return c; }

private:
  bounded_bitstring<22, 32, false, true> c;
};

// GlobalENB-ID ::= SEQUENCE
struct global_enb_id_s {
  fixed_octstring<3, true> plmn_id;
  enb_id_c                 enb_id;
  // ...

  static const asn1::table::seq_desc desc;
};

// GlobalGNB-ID ::= SEQUENCE
struct global_gnb_id_s {
  fixed_octstring<3, true> plmn_id;
  gnb_id_c                 gnb_id;
  // ...

  static const asn1::table::seq_desc desc;
};

// NgENB-ID ::= CHOICE
struct ng_enb_id_c {
  struct types_opts {
    enum options { macro_ng_enb_id, short_macro_ng_enb_id, long_macro_ng_enb_id, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  fixed_bitstring<20, false, true>&             macro_ng_enb_id()
  {
    assert_choice_type(types::macro_ng_enb_id, c.type, "NgENB-ID");
    return c.get<fixed_bitstring<20, false, true>>();
  }
  fixed_bitstring<18, false, true>& short_macro_ng_enb_id()
  {
    assert_choice_type(types::short_macro_ng_enb_id, c.type, "NgENB-ID");
    return c.get<fixed_bitstring<18, false, true>>();
  }
  fixed_bitstring<21, false, true>& long_macro_ng_enb_id()
  {
    assert_choice_type(types::long_macro_ng_enb_id, c.type, "NgENB-ID");
    return c.get<fixed_bitstring<21, false, true>>();
  }
  const fixed_bitstring<20, false, true>& macro_ng_enb_id() const
  {
    assert_choice_type(types::macro_ng_enb_id, c.type, "NgENB-ID");
    return c.get<fixed_bitstring<20, false, true>>();
  }
  const fixed_bitstring<18, false, true>& short_macro_ng_enb_id() const
  {
    assert_choice_type(types::short_macro_ng_enb_id, c.type, "NgENB-ID");
    return c.get<fixed_bitstring<18, false, true>>();
  }
  const fixed_bitstring<21, false, true>& long_macro_ng_enb_id() const
  {
    assert_choice_type(types::long_macro_ng_enb_id, c.type, "NgENB-ID");
    return c.get<fixed_bitstring<21, false, true>>();
  }
  fixed_bitstring<20, false, true>& set_macro_ng_enb_id();
  fixed_bitstring<18, false, true>& set_short_macro_ng_enb_id();
  fixed_bitstring<21, false, true>& set_long_macro_ng_enb_id();

private:
  asn1::table::choice_storage<ng_enb_id_c, types, choice_buffer_t<fixed_bitstring<21, false, true>>> c;
};

// GlobalNgENB-ID ::= SEQUENCE
struct global_ng_enb_id_s {
  fixed_octstring<3, true> plmn_id;
  ng_enb_id_c              ng_enb_id;
  // ...

  static const asn1::table::seq_desc desc;
};

// GlobalNGRANNodeID ::= CHOICE
struct global_ngran_node_id_c {
  struct types_opts {
    enum options { gnb, ng_enb, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  global_gnb_id_s&                              gnb()
  {
    assert_choice_type(types::gnb, c.type, "GlobalNGRANNodeID");
    return c.get<global_gnb_id_s>();
  }
  global_ng_enb_id_s& ng_enb()
  {
    assert_choice_type(types::ng_enb, c.type, "GlobalNGRANNodeID");
    return c.get<global_ng_enb_id_s>();
  }
  const global_gnb_id_s& gnb() const
  {
    assert_choice_type(types::gnb, c.type, "GlobalNGRANNodeID");
    return c.get<global_gnb_id_s>();
  }
  const global_ng_enb_id_s& ng_enb() const
  {
    assert_choice_type(types::ng_enb, c.type, "GlobalNGRANNodeID");
    return c.get<global_ng_enb_id_s>();
  }
  global_gnb_id_s&    set_gnb();
  global_ng_enb_id_s& set_ng_enb();

private:
  asn1::table::choice_storage<global_ngran_node_id_c, types, choice_buffer_t<global_gnb_id_s, global_ng_enb_id_s>> c;
};

// GlobalenGNB-ID ::= SEQUENCE
struct globalen_gnb_id_s {
  fixed_octstring<3, true> plmn_id;
  en_gnb_id_c              en_gnb_id;
  // ...

  static const asn1::table::seq_desc desc;
};

// GroupID ::= CHOICE
struct group_id_c {
  struct types_opts {
    enum options { five_gc, epc, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  uint16_t&                                     five_gc()
  {
    assert_choice_type(types::five_gc, c.type, "GroupID");
    return c.get<uint16_t>();
  }
  uint16_t& epc()
  {
    assert_choice_type(types::epc, c.type, "GroupID");
    return c.get<uint16_t>();
  }
  const uint16_t& five_gc() const
  {
    assert_choice_type(types::five_gc, c.type, "GroupID");
    return c.get<uint16_t>();
  }
  const uint16_t& epc() const
  {
    assert_choice_type(types::epc, c.type, "GroupID");
    return c.get<uint16_t>();
  }
  uint16_t& set_five_gc();
  uint16_t& set_epc();

private:
  asn1::table::choice_storage<group_id_c, types, pod_choice_buffer_t> c;
};

// Interface-MessageID ::= SEQUENCE
struct interface_msg_id_s {
  struct msg_type_opts {
    enum options { init_msg, successful_outcome, unsuccessful_outcome, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using msg_type_e_ = enumerated<msg_type_opts, true>;

  // member variables
  int64_t     interface_proc_id = 0;
  msg_type_e_ msg_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// InterfaceID-E1 ::= SEQUENCE
struct interface_id_e1_s {
  global_gnb_id_s global_gnb_id;
  uint64_t        gnb_cu_up_id = 0;
  // ...

  static const asn1::table::seq_desc desc;
};

// InterfaceID-F1 ::= SEQUENCE
struct interface_id_f1_s {
  global_gnb_id_s global_gnb_id;
  uint64_t        gnb_du_id = 0;
  // ...

  static const asn1::table::seq_desc desc;
};

// InterfaceID-NG ::= SEQUENCE
struct interface_id_ng_s {
  guami_s guami;
  // ...

  static const asn1::table::seq_desc desc;
};

// InterfaceID-S1 ::= SEQUENCE
struct interface_id_s1_s {
  gummei_s gummei;
  // ...

  static const asn1::table::seq_desc desc;
};

// InterfaceID-W1 ::= SEQUENCE
struct interface_id_w1_s {
  global_ng_enb_id_s global_ng_enb_id;
  uint64_t           ng_enb_du_id = 0;
  // ...

  static const asn1::table::seq_desc desc;
};

// InterfaceID-X2 ::= SEQUENCE
struct interface_id_x2_s {
  struct node_type_c_ {
    struct types_opts {
      enum options { global_enb_id, global_en_gnb_id, /*...*/ nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts, true>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    global_enb_id_s&                              global_enb_id()
    {
      assert_choice_type(types::global_enb_id, c.type, "nodeType");
      return c.get<global_enb_id_s>();
    }
    globalen_gnb_id_s& global_en_gnb_id()
    {
      assert_choice_type(types::global_en_gnb_id, c.type, "nodeType");
      return c.get<globalen_gnb_id_s>();
    }
    const global_enb_id_s& global_enb_id() const
    {
      assert_choice_type(types::global_enb_id, c.type, "nodeType");
      return c.get<global_enb_id_s>();
    }
    const globalen_gnb_id_s& global_en_gnb_id() const
    {
      assert_choice_type(types::global_en_gnb_id, c.type, "nodeType");
      return c.get<globalen_gnb_id_s>();
    }
    global_enb_id_s&   set_global_enb_id();
    globalen_gnb_id_s& set_global_en_gnb_id();

  private:
    asn1::table::choice_storage<node_type_c_, types, choice_buffer_t<global_enb_id_s, globalen_gnb_id_s>> c;
  };

  // member variables
  node_type_c_ node_type;
  // ...

  static const asn1::table::seq_desc desc;
};

// InterfaceID-Xn ::= SEQUENCE
struct interface_id_xn_s {
  global_ngran_node_id_c global_ng_ran_id;
  // ...

  static const asn1::table::seq_desc desc;
};

// InterfaceIdentifier ::= CHOICE
struct interface_id_c {
  struct types_opts {
    enum options { ng, xn, f1, e1, s1, x2, w1, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  interface_id_ng_s&                            ng()
  {
    assert_choice_type(types::ng, c.type, "InterfaceIdentifier");
    return c.get<interface_id_ng_s>();
  }
  interface_id_xn_s& xn()
  {
    assert_choice_type(types::xn, c.type, "InterfaceIdentifier");
    return c.get<interface_id_xn_s>();
  }
  interface_id_f1_s& f1()
  {
    assert_choice_type(types::f1, c.type, "InterfaceIdentifier");
    return c.get<interface_id_f1_s>();
  }
  interface_id_e1_s& e1()
  {
    assert_choice_type(types::e1, c.type, "InterfaceIdentifier");
    return c.get<interface_id_e1_s>();
  }
  interface_id_s1_s& s1()
  {
    assert_choice_type(types::s1, c.type, "InterfaceIdentifier");
    return c.get<interface_id_s1_s>();
  }
  interface_id_x2_s& x2()
  {
    assert_choice_type(types::x2, c.type, "InterfaceIdentifier");
    return c.get<interface_id_x2_s>();
  }
  interface_id_w1_s& w1()
  {
    assert_choice_type(types::w1, c.type, "InterfaceIdentifier");
    return c.get<interface_id_w1_s>();
  }
  const interface_id_ng_s& ng() const
  {
    assert_choice_type(types::ng, c.type, "InterfaceIdentifier");
    return c.get<interface_id_ng_s>();
  }
  const interface_id_xn_s& xn() const
  {
    assert_choice_type(types::xn, c.type, "InterfaceIdentifier");
    return c.get<interface_id_xn_s>();
  }
  const interface_id_f1_s& f1() const
  {
    assert_choice_type(types::f1, c.type, "InterfaceIdentifier");
    return c.get<interface_id_f1_s>();
  }
  const interface_id_e1_s& e1() const
  {
    assert_choice_type(types::e1, c.type, "InterfaceIdentifier");
    return c.get<interface_id_e1_s>();
  }
  const interface_id_s1_s& s1() const
  {
    assert_choice_type(types::s1, c.type, "InterfaceIdentifier");
    return c.get<interface_id_s1_s>();
  }
  const interface_id_x2_s& x2() const
  {
    assert_choice_type(types::x2, c.type, "InterfaceIdentifier");
    return c.get<interface_id_x2_s>();
  }
  const interface_id_w1_s& w1() const
  {
    assert_choice_type(types::w1, c.type, "InterfaceIdentifier");
    return c.get<interface_id_w1_s>();
  }
  interface_id_ng_s& set_ng();
  interface_id_xn_s& set_xn();
  interface_id_f1_s& set_f1();
  interface_id_e1_s& set_e1();
  interface_id_s1_s& set_s1();
  interface_id_x2_s& set_x2();
  interface_id_w1_s& set_w1();

private:
  asn1::table::choice_storage<interface_id_c,
                              types,
                              choice_buffer_t<interface_id_e1_s,
                                              interface_id_f1_s,
                                              interface_id_ng_s,
                                              interface_id_s1_s,
                                              interface_id_w1_s,
                                              interface_id_x2_s,
                                              interface_id_xn_s>>
      c;
};

// InterfaceType ::= ENUMERATED
struct interface_type_opts {
  enum options { ng, xn, f1, e1, s1, x2, w1, /*...*/ nulltype } value;

  static const asn1::table::enum_desc desc;
};
using interface_type_e = enumerated<interface_type_opts, true>;

// NR-ARFCN ::= SEQUENCE
struct nr_arfcn_s {
  uint32_t nr_arfcn = 0;
  // ...

  static const asn1::table::seq_desc desc;
};

// SupportedSULFreqBandItem ::= SEQUENCE
struct supported_sul_freq_band_item_s {
  uint16_t freq_band_ind_nr = 1;
  // ...

  static const asn1::table::seq_desc desc;
};

// SupportedSULBandList ::= SEQUENCE (SIZE (0..32)) OF SupportedSULFreqBandItem
using supported_sul_band_list_l = dyn_array<supported_sul_freq_band_item_s>;

// NRFrequencyBandItem ::= SEQUENCE
struct nr_freq_band_item_s {
  uint16_t                  freq_band_ind_nr = 1;
  supported_sul_band_list_l supported_sul_band_list;
  // ...

  static const asn1::table::seq_desc desc;
};

// NRFrequencyBand-List ::= SEQUENCE (SIZE (1..32)) OF NRFrequencyBandItem
using nr_freq_band_list_l = dyn_array<nr_freq_band_item_s>;

// NRFrequencyShift7p5khz ::= ENUMERATED
struct nr_freq_shift7p5khz_opts {
  enum options { false_value, true_value, /*...*/ nulltype } value;

  static const asn1::table::enum_desc desc;
};
using nr_freq_shift7p5khz_e = enumerated<nr_freq_shift7p5khz_opts, true>;

// NRFrequencyInfo ::= SEQUENCE
struct nr_freq_info_s {
  bool                  freq_shift7p5khz_present = false;
  nr_arfcn_s            nr_arfcn;
  nr_freq_band_list_l   freq_band_list;
  nr_freq_shift7p5khz_e freq_shift7p5khz;
  // ...

  static const asn1::table::seq_desc desc;
};

// PartialUEID ::= SEQUENCE
struct partial_ue_id_s {
  bool                     amf_ue_ngap_id_present        = false;
  bool                     guami_present                 = false;
  bool                     gnb_cu_ue_f1ap_id_present     = false;
  bool                     gnb_cu_cp_ue_e1ap_id_present  = false;
  bool                     ran_ue_id_present             = false;
  bool                     m_ng_ran_ue_xnap_id_present   = false;
  bool                     global_ng_ran_node_id_present = false;
  bool                     cell_rnti_present             = false;
  bool                     ng_enb_cu_ue_w1_ap_id_present = false;
  bool                     m_enb_ue_x2ap_id_present      = false;
  bool                     m_enb_ue_x2ap_id_ext_present  = false;
  bool                     global_enb_id_present         = false;
  bool                     mme_ue_s1ap_id_present        = false;
  bool                     gummei_present                = false;
  uint64_t                 amf_ue_ngap_id                = 0;
  guami_s                  guami;
  uint64_t                 gnb_cu_ue_f1ap_id    = 0;
  uint64_t                 gnb_cu_cp_ue_e1ap_id = 0;
  fixed_octstring<8, true> ran_ue_id;
  uint64_t                 m_ng_ran_ue_xnap_id = 0;
  global_ngran_node_id_c   global_ng_ran_node_id;
  cell_rnti_s              cell_rnti;
  uint64_t                 ng_enb_cu_ue_w1_ap_id = 0;
  uint16_t                 m_enb_ue_x2ap_id      = 0;
  uint16_t                 m_enb_ue_x2ap_id_ext  = 0;
  global_enb_id_s          global_enb_id;
  uint64_t                 mme_ue_s1ap_id = 0;
  gummei_s                 gummei;
  // ...

  static const asn1::table::seq_desc desc;
};

// QoSID ::= CHOICE
struct qos_id_c {
  struct types_opts {
    enum options { five_gc, epc, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
    uint8_t                             to_number() const;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  uint16_t&                                     five_gc()
  {
    assert_choice_type(types::five_gc, c.type, "QoSID");
    return c.get<uint16_t>();
  }
  uint16_t& epc()
  {
    assert_choice_type(types::epc, c.type, "QoSID");
    return c.get<uint16_t>();
  }
  const uint16_t& five_gc() const
  {
    assert_choice_type(types::five_gc, c.type, "QoSID");
    return c.get<uint16_t>();
  }
  const uint16_t& epc() const
  {
    assert_choice_type(types::epc, c.type, "QoSID");
    return c.get<uint16_t>();
  }
  uint16_t& set_five_gc();
  uint16_t& set_epc();

private:
  asn1::table::choice_storage<qos_id_c, types, pod_choice_buffer_t> c;
};

// RANfunction-Name ::= SEQUENCE
struct ra_nfunction_name_s {
  bool                                  ran_function_instance_present = false;
  printable_string<1, 150, true, true>  ran_function_short_name;
  printable_string<1, 1000, true, true> ran_function_e2sm_o_id;
  printable_string<1, 150, true, true>  ran_function_description;
  int64_t                               ran_function_instance = 0;
  // ...

  static const asn1::table::seq_desc desc;
};

// RRCclass-LTE ::= ENUMERATED
struct rrc_class_lte_opts {
  enum options {
    bcch_bch,
    bcch_bch_mbms,
    bcch_dl_sch,
    bcch_dl_sch_br,
    bcch_dl_sch_mbms,
    mcch,
    pcch,
    dl_ccch,
    dl_dcch,
    ul_ccch,
    ul_dcch,
    sc_mcch,
    // ...
    nulltype
  } value;

  static const asn1::table::enum_desc desc;
};
using rrc_class_lte_e = enumerated<rrc_class_lte_opts, true>;

// RRCclass-NR ::= ENUMERATED
struct rrc_class_nr_opts {
  enum options { bcch_bch, bcch_dl_sch, dl_ccch, dl_dcch, pcch, ul_ccch, ul_ccch1, ul_dcch, /*...*/ nulltype } value;

  static const asn1::table::enum_desc desc;
  uint8_t                             to_number() const;
};
using rrc_class_nr_e = enumerated<rrc_class_nr_opts, true>;

// RRC-MessageID ::= SEQUENCE
struct rrc_msg_id_s {
  struct rrc_type_c_ {
    struct types_opts {
      enum options { lte, nr, /*...*/ nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts, true>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    rrc_class_lte_e&                              lte()
    {
      assert_choice_type(types::lte, c.type, "rrcType");
      return c.get<rrc_class_lte_e>();
    }
    rrc_class_nr_e& nr()
    {
      assert_choice_type(types::nr, c.type, "rrcType");
      return c.get<rrc_class_nr_e>();
    }
    const rrc_class_lte_e& lte() const
    {
      assert_choice_type(types::lte, c.type, "rrcType");
      return c.get<rrc_class_lte_e>();
    }
    const rrc_class_nr_e& nr() const
    {
      assert_choice_type(types::nr, c.type, "rrcType");
      return c.get<rrc_class_nr_e>();
    }
    rrc_class_lte_e& set_lte();
    rrc_class_nr_e&  set_nr();

  private:
    asn1::table::choice_storage<rrc_type_c_, types, pod_choice_buffer_t> c;
  };

  // member variables
  rrc_type_c_ rrc_type;
  int64_t     msg_id = 0;
  // ...

  static const asn1::table::seq_desc desc;
};

// S-NSSAI ::= SEQUENCE
struct s_nssai_s {
  bool                     sd_present = false;
  fixed_octstring<1, true> sst;
  fixed_octstring<3, true> sd;
  // ...

  static const asn1::table::seq_desc desc;
};

// ServingCell-ARFCN ::= CHOICE
struct serving_cell_arfcn_c {
  struct types_opts {
    enum options { nr, eutra, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  nr_arfcn_s&                                   nr()
  {
    assert_choice_type(types::nr, c.type, "ServingCell-ARFCN");
    return c.get<nr_arfcn_s>();
  }
  uint32_t& eutra()
  {
    assert_choice_type(types::eutra, c.type, "ServingCell-ARFCN");
    return c.get<uint32_t>();
  }
  const nr_arfcn_s& nr() const
  {
    assert_choice_type(types::nr, c.type, "ServingCell-ARFCN");
    return c.get<nr_arfcn_s>();
  }
  const uint32_t& eutra() const
  {
    assert_choice_type(types::eutra, c.type, "ServingCell-ARFCN");
    return c.get<uint32_t>();
  }
  nr_arfcn_s& set_nr();
  uint32_t&   set_eutra();

private:
  asn1::table::choice_storage<serving_cell_arfcn_c, types, choice_buffer_t<nr_arfcn_s>> c;
};

// ServingCell-PCI ::= CHOICE
struct serving_cell_pci_c {
  struct types_opts {
    enum options { nr, eutra, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  uint16_t&                                     nr()
  {
    assert_choice_type(types::nr, c.type, "ServingCell-PCI");
    return c.get<uint16_t>();
  }
  uint16_t& eutra()
  {
    assert_choice_type(types::eutra, c.type, "ServingCell-PCI");
    return c.get<uint16_t>();
  }
  const uint16_t& nr() const
  {
    assert_choice_type(types::nr, c.type, "ServingCell-PCI");
    return c.get<uint16_t>();
  }
  const uint16_t& eutra() const
  {
    assert_choice_type(types::eutra, c.type, "ServingCell-PCI");
    return c.get<uint16_t>();
  }
  uint16_t& set_nr();
  uint16_t& set_eutra();

private:
  asn1::table::choice_storage<serving_cell_pci_c, types, pod_choice_buffer_t> c;
};

// UEID-GNB-CU-CP-F1AP-ID-Item ::= SEQUENCE
struct ue_id_gnb_cu_cp_f1ap_id_item_s {
  uint64_t gnb_cu_ue_f1ap_id = 0;
  // ...

  static const asn1::table::seq_desc desc;
};

// UEID-GNB-CU-F1AP-ID-List ::= SEQUENCE (SIZE (1..4)) OF UEID-GNB-CU-CP-F1AP-ID-Item
using ue_id_gnb_cu_f1ap_id_list_l = dyn_array<ue_id_gnb_cu_cp_f1ap_id_item_s>;

// UEID-GNB-CU-CP-E1AP-ID-Item ::= SEQUENCE
struct ue_id_gnb_cu_cp_e1ap_id_item_s {
  uint64_t gnb_cu_cp_ue_e1ap_id = 0;
  // ...

  static const asn1::table::seq_desc desc;
};

// UEID-GNB-CU-CP-E1AP-ID-List ::= SEQUENCE (SIZE (1..65535)) OF UEID-GNB-CU-CP-E1AP-ID-Item
using ue_id_gnb_cu_cp_e1ap_id_list_l = dyn_array<ue_id_gnb_cu_cp_e1ap_id_item_s>;

// UEID-GNB ::= SEQUENCE
struct ue_id_gnb_s {
  bool                           ran_ue_id_present           = false;
  bool                           m_ng_ran_ue_xnap_id_present = false;
  bool                           global_gnb_id_present       = false;
  uint64_t                       amf_ue_ngap_id              = 0;
  guami_s                        guami;
  ue_id_gnb_cu_f1ap_id_list_l    gnb_cu_ue_f1ap_id_list;
  ue_id_gnb_cu_cp_e1ap_id_list_l gnb_cu_cp_ue_e1ap_id_list;
  fixed_octstring<8, true>       ran_ue_id;
  uint64_t                       m_ng_ran_ue_xnap_id = 0;
  global_gnb_id_s                global_gnb_id;
  // ...
  copy_ptr<global_ngran_node_id_c> global_ng_ran_node_id;
  copy_ptr<cell_rnti_s>            cell_rnti;

  static const asn1::table::seq_desc desc;
};

// UEID-GNB-DU ::= SEQUENCE
struct ue_id_gnb_du_s {
  bool                     ran_ue_id_present = false;
  uint64_t                 gnb_cu_ue_f1ap_id = 0;
  fixed_octstring<8, true> ran_ue_id;
  // ...
  copy_ptr<cell_rnti_s> cell_rnti;

  static const asn1::table::seq_desc desc;
};

// UEID-GNB-CU-UP ::= SEQUENCE
struct ue_id_gnb_cu_up_s {
  bool                     ran_ue_id_present    = false;
  uint64_t                 gnb_cu_cp_ue_e1ap_id = 0;
  fixed_octstring<8, true> ran_ue_id;
  // ...

  static const asn1::table::seq_desc desc;
};

// UEID-NG-ENB ::= SEQUENCE
struct ue_id_ng_enb_s {
  bool               ng_enb_cu_ue_w1_ap_id_present = false;
  bool               m_ng_ran_ue_xnap_id_present   = false;
  bool               global_ng_enb_id_present      = false;
  uint64_t           amf_ue_ngap_id                = 0;
  guami_s            guami;
  uint64_t           ng_enb_cu_ue_w1_ap_id = 0;
  uint64_t           m_ng_ran_ue_xnap_id   = 0;
  global_ng_enb_id_s global_ng_enb_id;
  // ...
  copy_ptr<global_ngran_node_id_c> global_ng_ran_node_id;
  copy_ptr<cell_rnti_s>            cell_rnti;

  static const asn1::table::seq_desc desc;
};

// UEID-NG-ENB-DU ::= SEQUENCE
struct ue_id_ng_enb_du_s {
  uint64_t ng_enb_cu_ue_w1_ap_id = 0;
  // ...
  copy_ptr<cell_rnti_s> cell_rnti;

  static const asn1::table::seq_desc desc;
};

// UEID-EN-GNB ::= SEQUENCE
struct ue_id_en_gnb_s {
  bool                           m_enb_ue_x2ap_id_ext_present = false;
  bool                           gnb_cu_ue_f1ap_id_present    = false;
  bool                           ran_ue_id_present            = false;
  uint16_t                       m_enb_ue_x2ap_id             = 0;
  uint16_t                       m_enb_ue_x2ap_id_ext         = 0;
  global_enb_id_s                global_enb_id;
  uint64_t                       gnb_cu_ue_f1ap_id = 0;
  ue_id_gnb_cu_cp_e1ap_id_list_l gnb_cu_cp_ue_e1ap_id_list;
  fixed_octstring<8, true>       ran_ue_id;
  // ...
  copy_ptr<cell_rnti_s> cell_rnti;

  static const asn1::table::seq_desc desc;
};

// UEID-ENB ::= SEQUENCE
struct ue_id_enb_s {
  bool            m_enb_ue_x2ap_id_present     = false;
  bool            m_enb_ue_x2ap_id_ext_present = false;
  bool            global_enb_id_present        = false;
  uint64_t        mme_ue_s1ap_id               = 0;
  gummei_s        gummei;
  uint16_t        m_enb_ue_x2ap_id     = 0;
  uint16_t        m_enb_ue_x2ap_id_ext = 0;
  global_enb_id_s global_enb_id;
  // ...
  copy_ptr<cell_rnti_s> cell_rnti;

  static const asn1::table::seq_desc desc;
};

// UEID ::= CHOICE
struct ue_id_c {
  struct types_opts {
    enum options {
      gnb_ue_id,
      gnb_du_ue_id,
      gnb_cu_up_ue_id,
      ng_enb_ue_id,
      ng_enb_du_ue_id,
      en_gnb_ue_id,
      enb_ue_id,
      // ...
      nulltype
    } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  ue_id_gnb_s&                                  gnb_ue_id()
  {
    assert_choice_type(types::gnb_ue_id, c.type, "UEID");
    return c.get<ue_id_gnb_s>();
  }
  ue_id_gnb_du_s& gnb_du_ue_id()
  {
    assert_choice_type(types::gnb_du_ue_id, c.type, "UEID");
    return c.get<ue_id_gnb_du_s>();
  }
  ue_id_gnb_cu_up_s& gnb_cu_up_ue_id()
  {
    assert_choice_type(types::gnb_cu_up_ue_id, c.type, "UEID");
    return c.get<ue_id_gnb_cu_up_s>();
  }
  ue_id_ng_enb_s& ng_enb_ue_id()
  {
    assert_choice_type(types::ng_enb_ue_id, c.type, "UEID");
    return c.get<ue_id_ng_enb_s>();
  }
  ue_id_ng_enb_du_s& ng_enb_du_ue_id()
  {
    assert_choice_type(types::ng_enb_du_ue_id, c.type, "UEID");
    return c.get<ue_id_ng_enb_du_s>();
  }
  ue_id_en_gnb_s& en_gnb_ue_id()
  {
    assert_choice_type(types::en_gnb_ue_id, c.type, "UEID");
    return c.get<ue_id_en_gnb_s>();
  }
  ue_id_enb_s& enb_ue_id()
  {
    assert_choice_type(types::enb_ue_id, c.type, "UEID");
    return c.get<ue_id_enb_s>();
  }
  const ue_id_gnb_s& gnb_ue_id() const
  {
    assert_choice_type(types::gnb_ue_id, c.type, "UEID");
    return c.get<ue_id_gnb_s>();
  }
  const ue_id_gnb_du_s& gnb_du_ue_id() const
  {
    assert_choice_type(types::gnb_du_ue_id, c.type, "UEID");
    return c.get<ue_id_gnb_du_s>();
  }
  const ue_id_gnb_cu_up_s& gnb_cu_up_ue_id() const
  {
    assert_choice_type(types::gnb_cu_up_ue_id, c.type, "UEID");
    return c.get<ue_id_gnb_cu_up_s>();
  }
  const ue_id_ng_enb_s& ng_enb_ue_id() const
  {
    assert_choice_type(types::ng_enb_ue_id, c.type, "UEID");
    return c.get<ue_id_ng_enb_s>();
  }
  const ue_id_ng_enb_du_s& ng_enb_du_ue_id() const
  {
    assert_choice_type(types::ng_enb_du_ue_id, c.type, "UEID");
    return c.get<ue_id_ng_enb_du_s>();
  }
  const ue_id_en_gnb_s& en_gnb_ue_id() const
  {
    assert_choice_type(types::en_gnb_ue_id, c.type, "UEID");
    return c.get<ue_id_en_gnb_s>();
  }
  const ue_id_enb_s& enb_ue_id() const
  {
    assert_choice_type(types::enb_ue_id, c.type, "UEID");
    return c.get<ue_id_enb_s>();
  }
  ue_id_gnb_s&       set_gnb_ue_id();
  ue_id_gnb_du_s&    set_gnb_du_ue_id();
  ue_id_gnb_cu_up_s& set_gnb_cu_up_ue_id();
  ue_id_ng_enb_s&    set_ng_enb_ue_id();
  ue_id_ng_enb_du_s& set_ng_enb_du_ue_id();
  ue_id_en_gnb_s&    set_en_gnb_ue_id();
  ue_id_enb_s&       set_enb_ue_id();

private:
  asn1::table::choice_storage<ue_id_c,
                              types,
                              choice_buffer_t<ue_id_en_gnb_s,
                                              ue_id_enb_s,
                                              ue_id_gnb_cu_up_s,
                                              ue_id_gnb_du_s,
                                              ue_id_gnb_s,
                                              ue_id_ng_enb_du_s,
                                              ue_id_ng_enb_s>>
      c;
};

} // namespace e2sm
} // namespace asn1

extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::supported_sul_band_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::e2sm::nr_freq_band_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ue_id_gnb_cu_f1ap_id_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::e2sm::ue_id_gnb_cu_cp_e1ap_id_list_l::value_type>::ops;
