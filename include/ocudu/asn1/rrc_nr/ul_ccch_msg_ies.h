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

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// EstablishmentCause ::= ENUMERATED
struct establishment_cause_opts {
  enum options {
    emergency,
    high_prio_access,
    mt_access,
    mo_sig,
    mo_data,
    mo_voice_call,
    mo_video_call,
    mo_sms,
    mps_prio_access,
    mcs_prio_access,
    spare6,
    spare5,
    spare4,
    spare3,
    spare2,
    spare1,
    nulltype
  } value;

  static const asn1::table::enum_desc desc;
};
using establishment_cause_e = enumerated<establishment_cause_opts>;

// InitialUE-Identity ::= CHOICE
struct init_ue_id_c {
  struct types_opts {
    enum options { ng_5_g_s_tmsi_part1, random_value, nulltype } value;

    static const asn1::table::enum_desc desc;
    int8_t                              to_number() const;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  fixed_bitstring<39>&                          ng_5_g_s_tmsi_part1()
  {
    assert_choice_type(types::ng_5_g_s_tmsi_part1, c.type, "InitialUE-Identity");
    return c.get<fixed_bitstring<39>>();
  }
  fixed_bitstring<39>& random_value()
  {
    assert_choice_type(types::random_value, c.type, "InitialUE-Identity");
    return c.get<fixed_bitstring<39>>();
  }
  const fixed_bitstring<39>& ng_5_g_s_tmsi_part1() const
  {
    assert_choice_type(types::ng_5_g_s_tmsi_part1, c.type, "InitialUE-Identity");
    return c.get<fixed_bitstring<39>>();
  }
  const fixed_bitstring<39>& random_value() const
  {
    assert_choice_type(types::random_value, c.type, "InitialUE-Identity");
    return c.get<fixed_bitstring<39>>();
  }
  fixed_bitstring<39>& set_ng_5_g_s_tmsi_part1();
  fixed_bitstring<39>& set_random_value();

private:
  asn1::table::choice_storage<init_ue_id_c, types, choice_buffer_t<fixed_bitstring<39>>> c;
};

// RRC-PosSystemInfoRequest-r16-IEs ::= SEQUENCE
struct rrc_pos_sys_info_request_r16_ies_s {
  fixed_bitstring<32> requested_pos_si_list;
  fixed_bitstring<11> spare;

  static const asn1::table::seq_desc desc;
};

// ReestabUE-Identity ::= SEQUENCE
struct reestab_ue_id_s {
  uint32_t            c_rnti = 0;
  uint16_t            pci    = 0;
  fixed_bitstring<16> short_mac_i;

  static const asn1::table::seq_desc desc;
};

// ReestablishmentCause ::= ENUMERATED
struct reest_cause_opts {
  enum options { recfg_fail, ho_fail, other_fail, spare1, nulltype } value;

  static const asn1::table::enum_desc desc;
};
using reest_cause_e = enumerated<reest_cause_opts>;

// RRCReestablishmentRequest-IEs ::= SEQUENCE
struct rrc_reest_request_ies_s {
  reestab_ue_id_s    ue_id;
  reest_cause_e      reest_cause;
  fixed_bitstring<1> spare;

  static const asn1::table::seq_desc desc;
};

// RRCReestablishmentRequest ::= SEQUENCE
struct rrc_reest_request_s {
  rrc_reest_request_ies_s rrc_reest_request;

  static const asn1::table::seq_desc desc;
};

// ResumeCause ::= ENUMERATED
struct resume_cause_opts {
  enum options {
    emergency,
    high_prio_access,
    mt_access,
    mo_sig,
    mo_data,
    mo_voice_call,
    mo_video_call,
    mo_sms,
    rna_upd,
    mps_prio_access,
    mcs_prio_access,
    mt_sdt_v1810,
    srs_pos_cfg_or_activation_req_v1800,
    spare3,
    spare2,
    spare1,
    nulltype
  } value;

  static const asn1::table::enum_desc desc;
};
using resume_cause_e = enumerated<resume_cause_opts>;

// RRCResumeRequest-IEs ::= SEQUENCE
struct rrc_resume_request_ies_s {
  fixed_bitstring<24> resume_id;
  fixed_bitstring<16> resume_mac_i;
  resume_cause_e      resume_cause;
  fixed_bitstring<1>  spare;

  static const asn1::table::seq_desc desc;
};

// RRCResumeRequest ::= SEQUENCE
struct rrc_resume_request_s {
  rrc_resume_request_ies_s rrc_resume_request;

  static const asn1::table::seq_desc desc;
};

// RRCSetupRequest-IEs ::= SEQUENCE
struct rrc_setup_request_ies_s {
  init_ue_id_c          ue_id;
  establishment_cause_e establishment_cause;
  fixed_bitstring<1>    spare;

  static const asn1::table::seq_desc desc;
};

// RRCSetupRequest ::= SEQUENCE
struct rrc_setup_request_s {
  rrc_setup_request_ies_s rrc_setup_request;

  static const asn1::table::seq_desc desc;
};

// RRCSystemInfoRequest-IEs ::= SEQUENCE
struct rrc_sys_info_request_ies_s {
  fixed_bitstring<32> requested_si_list;
  fixed_bitstring<12> spare;

  static const asn1::table::seq_desc desc;
};

// RRCSystemInfoRequest ::= SEQUENCE
struct rrc_sys_info_request_s {
  struct crit_exts_c_ {
    struct crit_exts_future_r16_c_ {
      struct types_opts {
        enum options { rrc_pos_sys_info_request_r16, crit_exts_future, nulltype } value;

        static const asn1::table::enum_desc desc;
      };
      using types = enumerated<types_opts>;

      void                                  set(types::options e = types::nulltype);
      types                                 type() const { return type_; }
      static const asn1::table::choice_desc desc;
      rrc_pos_sys_info_request_r16_ies_s&   rrc_pos_sys_info_request_r16()
      {
        assert_choice_type(types::rrc_pos_sys_info_request_r16, type_, "criticalExtensionsFuture-r16");
        return c;
      }
      const rrc_pos_sys_info_request_r16_ies_s& rrc_pos_sys_info_request_r16() const
      {
        assert_choice_type(types::rrc_pos_sys_info_request_r16, type_, "criticalExtensionsFuture-r16");
        return c;
      }
      rrc_pos_sys_info_request_r16_ies_s& set_rrc_pos_sys_info_request_r16();
      void                                set_crit_exts_future();

    private:
      types                              type_;
      rrc_pos_sys_info_request_r16_ies_s c;
    };
    struct types_opts {
      enum options { rrc_sys_info_request, crit_exts_future_r16, nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    rrc_sys_info_request_ies_s&                   rrc_sys_info_request()
    {
      assert_choice_type(types::rrc_sys_info_request, c.type, "criticalExtensions");
      return c.get<rrc_sys_info_request_ies_s>();
    }
    crit_exts_future_r16_c_& crit_exts_future_r16()
    {
      assert_choice_type(types::crit_exts_future_r16, c.type, "criticalExtensions");
      return c.get<crit_exts_future_r16_c_>();
    }
    const rrc_sys_info_request_ies_s& rrc_sys_info_request() const
    {
      assert_choice_type(types::rrc_sys_info_request, c.type, "criticalExtensions");
      return c.get<rrc_sys_info_request_ies_s>();
    }
    const crit_exts_future_r16_c_& crit_exts_future_r16() const
    {
      assert_choice_type(types::crit_exts_future_r16, c.type, "criticalExtensions");
      return c.get<crit_exts_future_r16_c_>();
    }
    rrc_sys_info_request_ies_s& set_rrc_sys_info_request();
    crit_exts_future_r16_c_&    set_crit_exts_future_r16();

  private:
    asn1::table::
        choice_storage<crit_exts_c_, types, choice_buffer_t<crit_exts_future_r16_c_, rrc_sys_info_request_ies_s>>
            c;
  };

  // member variables
  crit_exts_c_ crit_exts;

  static const asn1::table::seq_desc desc;
};

} // namespace rrc_nr
} // namespace asn1
