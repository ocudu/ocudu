// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/*******************************************************************************
 *
 *                     3GPP TS ASN1 NRPPA v18.7.0 (2025-12)
 *
 ******************************************************************************/

#pragma once

#include "ocudu/asn1/asn1_ap_utils.h"

namespace asn1 {
namespace nrppa {

struct e_c_id_meas_initiation_request_ies_container;
using e_c_id_meas_initiation_request_s = elementary_procedure_option<e_c_id_meas_initiation_request_ies_container>;
struct otdoa_info_request_ies_container;
using otdoa_info_request_s = elementary_procedure_option<otdoa_info_request_ies_container>;
struct positioning_info_request_ies_container;
using positioning_info_request_s = elementary_procedure_option<positioning_info_request_ies_container>;
struct meas_request_ies_container;
using meas_request_s = elementary_procedure_option<meas_request_ies_container>;
struct trp_info_request_ies_container;
using trp_info_request_s = elementary_procedure_option<trp_info_request_ies_container>;
struct positioning_activation_request_ies_container;
using positioning_activation_request_s = elementary_procedure_option<positioning_activation_request_ies_container>;
struct prs_cfg_request_ies_container;
using prs_cfg_request_s = elementary_procedure_option<prs_cfg_request_ies_container>;
struct meas_precfg_required_ies_container;
using meas_precfg_required_s = elementary_procedure_option<meas_precfg_required_ies_container>;
struct e_c_id_meas_fail_ind_ies_container;
using e_c_id_meas_fail_ind_s = elementary_procedure_option<e_c_id_meas_fail_ind_ies_container>;
struct e_c_id_meas_report_ies_container;
using e_c_id_meas_report_s = elementary_procedure_option<e_c_id_meas_report_ies_container>;
struct e_c_id_meas_termination_cmd_ies_container;
using e_c_id_meas_termination_cmd_s = elementary_procedure_option<e_c_id_meas_termination_cmd_ies_container>;
struct error_ind_ies_container;
using error_ind_s = elementary_procedure_option<error_ind_ies_container>;
struct private_msg_s;
struct assist_info_ctrl_ies_container;
using assist_info_ctrl_s = elementary_procedure_option<assist_info_ctrl_ies_container>;
struct assist_info_feedback_ies_container;
using assist_info_feedback_s = elementary_procedure_option<assist_info_feedback_ies_container>;
struct positioning_info_upd_ies_container;
using positioning_info_upd_s = elementary_procedure_option<positioning_info_upd_ies_container>;
struct meas_report_ies_container;
using meas_report_s = elementary_procedure_option<meas_report_ies_container>;
struct meas_upd_ies_container;
using meas_upd_s = elementary_procedure_option<meas_upd_ies_container>;
struct meas_abort_ies_container;
using meas_abort_s = elementary_procedure_option<meas_abort_ies_container>;
struct meas_fail_ind_ies_container;
using meas_fail_ind_s = elementary_procedure_option<meas_fail_ind_ies_container>;
struct positioning_deactivation_ies_container;
using positioning_deactivation_s = elementary_procedure_option<positioning_deactivation_ies_container>;
struct meas_activation_ies_container;
using meas_activation_s = elementary_procedure_option<meas_activation_ies_container>;
struct srs_info_reserv_notif_ies_container;
using srs_info_reserv_notif_s = elementary_procedure_option<srs_info_reserv_notif_ies_container>;
struct e_c_id_meas_initiation_resp_ies_container;
using e_c_id_meas_initiation_resp_s = elementary_procedure_option<e_c_id_meas_initiation_resp_ies_container>;
struct otdoa_info_resp_ies_container;
using otdoa_info_resp_s = elementary_procedure_option<otdoa_info_resp_ies_container>;
struct positioning_info_resp_ies_container;
using positioning_info_resp_s = elementary_procedure_option<positioning_info_resp_ies_container>;
struct meas_resp_ies_container;
using meas_resp_s = elementary_procedure_option<meas_resp_ies_container>;
struct trp_info_resp_ies_container;
using trp_info_resp_s = elementary_procedure_option<trp_info_resp_ies_container>;
struct positioning_activation_resp_ies_container;
using positioning_activation_resp_s = elementary_procedure_option<positioning_activation_resp_ies_container>;
struct prs_cfg_resp_ies_container;
using prs_cfg_resp_s = elementary_procedure_option<prs_cfg_resp_ies_container>;
struct meas_precfg_confirm_ies_container;
using meas_precfg_confirm_s = elementary_procedure_option<meas_precfg_confirm_ies_container>;
struct e_c_id_meas_initiation_fail_ies_container;
using e_c_id_meas_initiation_fail_s = elementary_procedure_option<e_c_id_meas_initiation_fail_ies_container>;
struct otdoa_info_fail_ies_container;
using otdoa_info_fail_s = elementary_procedure_option<otdoa_info_fail_ies_container>;
struct positioning_info_fail_ies_container;
using positioning_info_fail_s = elementary_procedure_option<positioning_info_fail_ies_container>;
struct meas_fail_ies_container;
using meas_fail_s = elementary_procedure_option<meas_fail_ies_container>;
struct trp_info_fail_ies_container;
using trp_info_fail_s = elementary_procedure_option<trp_info_fail_ies_container>;
struct positioning_activation_fail_ies_container;
using positioning_activation_fail_s = elementary_procedure_option<positioning_activation_fail_ies_container>;
struct prs_cfg_fail_ies_container;
using prs_cfg_fail_s = elementary_procedure_option<prs_cfg_fail_ies_container>;
struct meas_precfg_refuse_ies_container;
using meas_precfg_refuse_s = elementary_procedure_option<meas_precfg_refuse_ies_container>;

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// NRPPA-ELEMENTARY-PROCEDURES ::= OBJECT SET OF NRPPA-ELEMENTARY-PROCEDURE
struct nr_ppa_elem_procs_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        e_c_id_meas_initiation_request,
        otdoa_info_request,
        positioning_info_request,
        meas_request,
        trp_info_request,
        positioning_activation_request,
        prs_cfg_request,
        meas_precfg_required,
        e_c_id_meas_fail_ind,
        e_c_id_meas_report,
        e_c_id_meas_termination_cmd,
        error_ind,
        private_msg,
        assist_info_ctrl,
        assist_info_feedback,
        positioning_info_upd,
        meas_report,
        meas_upd,
        meas_abort,
        meas_fail_ind,
        positioning_deactivation,
        meas_activation,
        srs_info_reserv_notif,
        nulltype
      } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    void                                    set(types::options e = types::nulltype);
    types                                   type() const { return type_; }
    static const asn1::table::choice_desc   desc;
    e_c_id_meas_initiation_request_s&       e_c_id_meas_initiation_request();
    otdoa_info_request_s&                   otdoa_info_request();
    positioning_info_request_s&             positioning_info_request();
    meas_request_s&                         meas_request();
    trp_info_request_s&                     trp_info_request();
    positioning_activation_request_s&       positioning_activation_request();
    prs_cfg_request_s&                      prs_cfg_request();
    meas_precfg_required_s&                 meas_precfg_required();
    e_c_id_meas_fail_ind_s&                 e_c_id_meas_fail_ind();
    e_c_id_meas_report_s&                   e_c_id_meas_report();
    e_c_id_meas_termination_cmd_s&          e_c_id_meas_termination_cmd();
    error_ind_s&                            error_ind();
    private_msg_s&                          private_msg();
    assist_info_ctrl_s&                     assist_info_ctrl();
    assist_info_feedback_s&                 assist_info_feedback();
    positioning_info_upd_s&                 positioning_info_upd();
    meas_report_s&                          meas_report();
    meas_upd_s&                             meas_upd();
    meas_abort_s&                           meas_abort();
    meas_fail_ind_s&                        meas_fail_ind();
    positioning_deactivation_s&             positioning_deactivation();
    meas_activation_s&                      meas_activation();
    srs_info_reserv_notif_s&                srs_info_reserv_notif();
    const e_c_id_meas_initiation_request_s& e_c_id_meas_initiation_request() const;
    const otdoa_info_request_s&             otdoa_info_request() const;
    const positioning_info_request_s&       positioning_info_request() const;
    const meas_request_s&                   meas_request() const;
    const trp_info_request_s&               trp_info_request() const;
    const positioning_activation_request_s& positioning_activation_request() const;
    const prs_cfg_request_s&                prs_cfg_request() const;
    const meas_precfg_required_s&           meas_precfg_required() const;
    const e_c_id_meas_fail_ind_s&           e_c_id_meas_fail_ind() const;
    const e_c_id_meas_report_s&             e_c_id_meas_report() const;
    const e_c_id_meas_termination_cmd_s&    e_c_id_meas_termination_cmd() const;
    const error_ind_s&                      error_ind() const;
    const private_msg_s&                    private_msg() const;
    const assist_info_ctrl_s&               assist_info_ctrl() const;
    const assist_info_feedback_s&           assist_info_feedback() const;
    const positioning_info_upd_s&           positioning_info_upd() const;
    const meas_report_s&                    meas_report() const;
    const meas_upd_s&                       meas_upd() const;
    const meas_abort_s&                     meas_abort() const;
    const meas_fail_ind_s&                  meas_fail_ind() const;
    const positioning_deactivation_s&       positioning_deactivation() const;
    const meas_activation_s&                meas_activation() const;
    const srs_info_reserv_notif_s&          srs_info_reserv_notif() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        e_c_id_meas_initiation_resp,
        otdoa_info_resp,
        positioning_info_resp,
        meas_resp,
        trp_info_resp,
        positioning_activation_resp,
        prs_cfg_resp,
        meas_precfg_confirm,
        nulltype
      } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    void                                  set(types::options e = types::nulltype);
    types                                 type() const { return type_; }
    static const asn1::table::choice_desc desc;
    e_c_id_meas_initiation_resp_s&        e_c_id_meas_initiation_resp();
    otdoa_info_resp_s&                    otdoa_info_resp();
    positioning_info_resp_s&              positioning_info_resp();
    meas_resp_s&                          meas_resp();
    trp_info_resp_s&                      trp_info_resp();
    positioning_activation_resp_s&        positioning_activation_resp();
    prs_cfg_resp_s&                       prs_cfg_resp();
    meas_precfg_confirm_s&                meas_precfg_confirm();
    const e_c_id_meas_initiation_resp_s&  e_c_id_meas_initiation_resp() const;
    const otdoa_info_resp_s&              otdoa_info_resp() const;
    const positioning_info_resp_s&        positioning_info_resp() const;
    const meas_resp_s&                    meas_resp() const;
    const trp_info_resp_s&                trp_info_resp() const;
    const positioning_activation_resp_s&  positioning_activation_resp() const;
    const prs_cfg_resp_s&                 prs_cfg_resp() const;
    const meas_precfg_confirm_s&          meas_precfg_confirm() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        e_c_id_meas_initiation_fail,
        otdoa_info_fail,
        positioning_info_fail,
        meas_fail,
        trp_info_fail,
        positioning_activation_fail,
        prs_cfg_fail,
        meas_precfg_refuse,
        nulltype
      } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    void                                  set(types::options e = types::nulltype);
    types                                 type() const { return type_; }
    static const asn1::table::choice_desc desc;
    e_c_id_meas_initiation_fail_s&        e_c_id_meas_initiation_fail();
    otdoa_info_fail_s&                    otdoa_info_fail();
    positioning_info_fail_s&              positioning_info_fail();
    meas_fail_s&                          meas_fail();
    trp_info_fail_s&                      trp_info_fail();
    positioning_activation_fail_s&        positioning_activation_fail();
    prs_cfg_fail_s&                       prs_cfg_fail();
    meas_precfg_refuse_s&                 meas_precfg_refuse();
    const e_c_id_meas_initiation_fail_s&  e_c_id_meas_initiation_fail() const;
    const otdoa_info_fail_s&              otdoa_info_fail() const;
    const positioning_info_fail_s&        positioning_info_fail() const;
    const meas_fail_s&                    meas_fail() const;
    const trp_info_fail_s&                trp_info_fail() const;
    const positioning_activation_fail_s&  positioning_activation_fail() const;
    const prs_cfg_fail_s&                 prs_cfg_fail() const;
    const meas_precfg_refuse_s&           meas_precfg_refuse() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  static const asn1::table::objset_desc desc;
};

// InitiatingMessage ::= SEQUENCE{{NRPPA-ELEMENTARY-PROCEDURE}}
struct init_msg_s {
  uint16_t                        proc_code = 0;
  crit_e                          crit;
  uint16_t                        nrppatransaction_id = 0;
  nr_ppa_elem_procs_o::init_msg_c value;

  static const asn1::table::seq_desc desc;
  bool                               load_info_obj(const uint16_t& proc_code_);
};

// SuccessfulOutcome ::= SEQUENCE{{NRPPA-ELEMENTARY-PROCEDURE}}
struct successful_outcome_s {
  uint16_t                                  proc_code = 0;
  crit_e                                    crit;
  uint16_t                                  nrppatransaction_id = 0;
  nr_ppa_elem_procs_o::successful_outcome_c value;

  static const asn1::table::seq_desc desc;
  bool                               load_info_obj(const uint16_t& proc_code_);
};

// UnsuccessfulOutcome ::= SEQUENCE{{NRPPA-ELEMENTARY-PROCEDURE}}
struct unsuccessful_outcome_s {
  uint16_t                                    proc_code = 0;
  crit_e                                      crit;
  uint16_t                                    nrppatransaction_id = 0;
  nr_ppa_elem_procs_o::unsuccessful_outcome_c value;

  static const asn1::table::seq_desc desc;
  bool                               load_info_obj(const uint16_t& proc_code_);
};

// NRPPA-PDU ::= CHOICE
struct nr_ppa_pdu_c {
  struct types_opts {
    enum options { init_msg, successful_outcome, unsuccessful_outcome, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts, true>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  init_msg_s&                                   init_msg()
  {
    assert_choice_type(types::init_msg, c.type, "NRPPA-PDU");
    return c.get<init_msg_s>();
  }
  successful_outcome_s& successful_outcome()
  {
    assert_choice_type(types::successful_outcome, c.type, "NRPPA-PDU");
    return c.get<successful_outcome_s>();
  }
  unsuccessful_outcome_s& unsuccessful_outcome()
  {
    assert_choice_type(types::unsuccessful_outcome, c.type, "NRPPA-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  const init_msg_s& init_msg() const
  {
    assert_choice_type(types::init_msg, c.type, "NRPPA-PDU");
    return c.get<init_msg_s>();
  }
  const successful_outcome_s& successful_outcome() const
  {
    assert_choice_type(types::successful_outcome, c.type, "NRPPA-PDU");
    return c.get<successful_outcome_s>();
  }
  const unsuccessful_outcome_s& unsuccessful_outcome() const
  {
    assert_choice_type(types::unsuccessful_outcome, c.type, "NRPPA-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  init_msg_s&             set_init_msg();
  successful_outcome_s&   set_successful_outcome();
  unsuccessful_outcome_s& set_unsuccessful_outcome();

private:
  asn1::table::
      choice_storage<nr_ppa_pdu_c, types, choice_buffer_t<init_msg_s, successful_outcome_s, unsuccessful_outcome_s>>
          c;
};

} // namespace nrppa
} // namespace asn1
