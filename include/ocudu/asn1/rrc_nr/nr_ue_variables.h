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

#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"

namespace asn1 {
namespace rrc_nr {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// CLI-RSSI-TriggeredList-r16 ::= SEQUENCE (SIZE (1..64)) OF INTEGER (0..63)
using cli_rssi_triggered_list_r16_l = dyn_array<uint8_t>;

// SRS-RSRP-TriggeredList-r16 ::= SEQUENCE (SIZE (1..32)) OF INTEGER (0..63)
using srs_rsrp_triggered_list_r16_l = bounded_array<uint8_t, 32>;

// CLI-TriggeredList-r16 ::= CHOICE
struct cli_triggered_list_r16_c {
  struct types_opts {
    enum options { srs_rsrp_triggered_list_r16, cli_rssi_triggered_list_r16, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  srs_rsrp_triggered_list_r16_l&                srs_rsrp_triggered_list_r16()
  {
    assert_choice_type(types::srs_rsrp_triggered_list_r16, c.type, "CLI-TriggeredList-r16");
    return c.get<srs_rsrp_triggered_list_r16_l>();
  }
  cli_rssi_triggered_list_r16_l& cli_rssi_triggered_list_r16()
  {
    assert_choice_type(types::cli_rssi_triggered_list_r16, c.type, "CLI-TriggeredList-r16");
    return c.get<cli_rssi_triggered_list_r16_l>();
  }
  const srs_rsrp_triggered_list_r16_l& srs_rsrp_triggered_list_r16() const
  {
    assert_choice_type(types::srs_rsrp_triggered_list_r16, c.type, "CLI-TriggeredList-r16");
    return c.get<srs_rsrp_triggered_list_r16_l>();
  }
  const cli_rssi_triggered_list_r16_l& cli_rssi_triggered_list_r16() const
  {
    assert_choice_type(types::cli_rssi_triggered_list_r16, c.type, "CLI-TriggeredList-r16");
    return c.get<cli_rssi_triggered_list_r16_l>();
  }
  srs_rsrp_triggered_list_r16_l& set_srs_rsrp_triggered_list_r16();
  cli_rssi_triggered_list_r16_l& set_cli_rssi_triggered_list_r16();

private:
  asn1::table::choice_storage<cli_triggered_list_r16_c,
                              types,
                              choice_buffer_t<cli_rssi_triggered_list_r16_l, srs_rsrp_triggered_list_r16_l>>
      c;
};

struct cells_triggered_list_item_c_ {
  struct types_opts {
    enum options { pci, pci_eutra, pci_utra_fdd_r16, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  uint16_t&                                     pci()
  {
    assert_choice_type(types::pci, c.type, "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  uint16_t& pci_eutra()
  {
    assert_choice_type(types::pci_eutra, c.type, "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  uint16_t& pci_utra_fdd_r16()
  {
    assert_choice_type(types::pci_utra_fdd_r16, c.type, "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  const uint16_t& pci() const
  {
    assert_choice_type(types::pci, c.type, "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  const uint16_t& pci_eutra() const
  {
    assert_choice_type(types::pci_eutra, c.type, "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  const uint16_t& pci_utra_fdd_r16() const
  {
    assert_choice_type(types::pci_utra_fdd_r16, c.type, "CellsTriggeredList-item");
    return c.get<uint16_t>();
  }
  uint16_t& set_pci();
  uint16_t& set_pci_eutra();
  uint16_t& set_pci_utra_fdd_r16();

private:
  asn1::table::choice_storage<cells_triggered_list_item_c_, types, pod_choice_buffer_t> c;
};

// CellsTriggeredList ::= SEQUENCE (SIZE (1..32)) OF CellsTriggeredList-item
using cells_triggered_list_l = dyn_array<cells_triggered_list_item_c_>;

// RelaysTriggeredList-r17 ::= SEQUENCE (SIZE (1..32)) OF BIT STRING (SIZE (24))
using relays_triggered_list_r17_l = bounded_array<fixed_bitstring<24>, 32>;

// SNPN-Identity-r18 ::= SEQUENCE
struct sn_pn_id_r18_s {
  plmn_id_s           plmn_id_r18;
  fixed_bitstring<44> nid_r18;

  static const asn1::table::seq_desc desc;
};

// VarAppLayerIdle-r18 ::= SEQUENCE
struct var_app_layer_idle_r18_s {
  struct service_type_r18_opts {
    enum options { streaming, mtsi, vr, spare5, spare4, spare3, spare2, spare1, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using service_type_r18_e_ = enumerated<service_type_r18_opts>;

  // member variables
  bool                              app_layer_meas_prio_r18_present = false;
  uint8_t                           meas_cfg_app_layer_id_r18       = 0;
  service_type_r18_e_               service_type_r18;
  app_layer_idle_inactive_cfg_r18_s app_layer_idle_inactive_cfg_r18;
  uint8_t                           app_layer_meas_prio_r18 = 1;

  static const asn1::table::seq_desc desc;
};

// VarAppLayerIdleConfig-r18 ::= SEQUENCE
struct var_app_layer_idle_cfg_r18_s {
  using app_layer_idle_cfg_list_r18_l_ = dyn_array<var_app_layer_idle_r18_s>;

  // member variables
  app_layer_idle_cfg_list_r18_l_ app_layer_idle_cfg_list_r18;

  static const asn1::table::seq_desc desc;
};

// VarAppLayerPLMN-List-r18 ::= SEQUENCE
struct var_app_layer_plmn_list_r18_s {
  uint8_t             meas_cfg_app_layer_id_r18 = 0;
  plmn_id_list2_r16_l plmn_id_list_r18;

  static const asn1::table::seq_desc desc;
};

// VarAppLayerPLMN-ListConfig-r18 ::= SEQUENCE
struct var_app_layer_plmn_list_cfg_r18_s {
  using plmn_cfg_list_r18_l_ = dyn_array<var_app_layer_plmn_list_r18_s>;

  // member variables
  plmn_cfg_list_r18_l_ plmn_cfg_list_r18;

  static const asn1::table::seq_desc desc;
};

// VarConditionalReconfig ::= SEQUENCE
struct var_conditional_recfg_s {
  using sk_counter_configuration_r18_l_ = dyn_array<sk_counter_cfg_r18_s>;

  // member variables
  cond_recfg_to_add_mod_list_r16_l cond_recfg_list;
  dyn_octstring                    scpac_ref_cfg_r18;
  sk_counter_configuration_r18_l_  sk_counter_cfg_r18;

  static const asn1::table::seq_desc desc;
};

// VarConnEstFailReport-r16 ::= SEQUENCE
struct var_conn_est_fail_report_r16_s {
  struct network_id_r18_c_ {
    struct types_opts {
      enum options { plmn_id_r18, snpn_id_r18, nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    plmn_id_s&                                    plmn_id_r18()
    {
      assert_choice_type(types::plmn_id_r18, c.type, "networkIdentity-r18");
      return c.get<plmn_id_s>();
    }
    sn_pn_id_r18_s& snpn_id_r18()
    {
      assert_choice_type(types::snpn_id_r18, c.type, "networkIdentity-r18");
      return c.get<sn_pn_id_r18_s>();
    }
    const plmn_id_s& plmn_id_r18() const
    {
      assert_choice_type(types::plmn_id_r18, c.type, "networkIdentity-r18");
      return c.get<plmn_id_s>();
    }
    const sn_pn_id_r18_s& snpn_id_r18() const
    {
      assert_choice_type(types::snpn_id_r18, c.type, "networkIdentity-r18");
      return c.get<sn_pn_id_r18_s>();
    }
    plmn_id_s&      set_plmn_id_r18();
    sn_pn_id_r18_s& set_snpn_id_r18();

  private:
    asn1::table::choice_storage<network_id_r18_c_, types, choice_buffer_t<plmn_id_s, sn_pn_id_r18_s>> c;
  };

  // member variables
  conn_est_fail_report_r16_s conn_est_fail_report_r16;
  network_id_r18_c_          network_id_r18;

  static const asn1::table::seq_desc desc;
};

// VarConnEstFailReportList-r17 ::= SEQUENCE
struct var_conn_est_fail_report_list_r17_s {
  using conn_est_fail_report_list_r17_l_ = dyn_array<var_conn_est_fail_report_r16_s>;

  // member variables
  conn_est_fail_report_list_r17_l_ conn_est_fail_report_list_r17;

  static const asn1::table::seq_desc desc;
};

// VarEnhMeasIdleConfig-r18 ::= SEQUENCE
struct var_enh_meas_idle_cfg_r18_s {
  bool                    meas_idle_validity_dur_r18_present = false;
  meas_validity_dur_r18_e meas_idle_validity_dur_r18;

  static const asn1::table::seq_desc desc;
};

// VarLTM-ServingCellNoResetID-r18 ::= SEQUENCE
struct var_ltm_serving_cell_no_reset_id_r18_s {
  bool    ltm_serving_cell_no_reset_id_r18_present = false;
  uint8_t ltm_serving_cell_no_reset_id_r18         = 1;

  static const asn1::table::seq_desc desc;
};

// VarLTM-ServingCellUE-MeasuredTA-ID-r18 ::= SEQUENCE
struct var_ltm_serving_cell_ue_measured_ta_id_r18_s {
  bool    ltm_serving_cell_ue_measured_ta_id_r18_present = false;
  uint8_t ltm_serving_cell_ue_measured_ta_id_r18         = 1;

  static const asn1::table::seq_desc desc;
};

// VarLogMeasConfig-r16 ::= SEQUENCE
struct var_log_meas_cfg_r16_s {
  struct report_type_c_ {
    struct types_opts {
      enum options { periodical, event_triggered, nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    logged_periodical_report_cfg_r16_s&           periodical()
    {
      assert_choice_type(types::periodical, c.type, "reportType");
      return c.get<logged_periodical_report_cfg_r16_s>();
    }
    logged_event_trigger_cfg_r16_s& event_triggered()
    {
      assert_choice_type(types::event_triggered, c.type, "reportType");
      return c.get<logged_event_trigger_cfg_r16_s>();
    }
    const logged_periodical_report_cfg_r16_s& periodical() const
    {
      assert_choice_type(types::periodical, c.type, "reportType");
      return c.get<logged_periodical_report_cfg_r16_s>();
    }
    const logged_event_trigger_cfg_r16_s& event_triggered() const
    {
      assert_choice_type(types::event_triggered, c.type, "reportType");
      return c.get<logged_event_trigger_cfg_r16_s>();
    }
    logged_periodical_report_cfg_r16_s& set_periodical();
    logged_event_trigger_cfg_r16_s&     set_event_triggered();

  private:
    asn1::table::choice_storage<report_type_c_,
                                types,
                                choice_buffer_t<logged_event_trigger_cfg_r16_s, logged_periodical_report_cfg_r16_s>>
        c;
  };

  // member variables
  bool                   area_cfg_r16_present         = false;
  bool                   sensor_name_list_r16_present = false;
  bool                   early_meas_ind_r17_present   = false;
  bool                   area_cfg_r17_present         = false;
  bool                   area_cfg_v1800_present       = false;
  area_cfg_r16_s         area_cfg_r16;
  bt_name_list_r16_l     bt_name_list_r16;
  wlan_name_list_r16_l   wlan_name_list_r16;
  sensor_name_list_r16_s sensor_name_list_r16;
  logging_dur_r16_e      logging_dur_r16;
  report_type_c_         report_type;
  area_cfg_r17_s         area_cfg_r17;
  area_cfg_v1800_c       area_cfg_v1800;

  static const asn1::table::seq_desc desc;
};

// VarLogMeasReport-r16 ::= SEQUENCE
struct var_log_meas_report_r16_s {
  struct id_list_r18_c_ {
    struct types_opts {
      enum options { plmn_id_list_r18, snpn_cfg_id_list_r18, nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    plmn_id_list2_r16_l&                          plmn_id_list_r18()
    {
      assert_choice_type(types::plmn_id_list_r18, c.type, "identityList-r18");
      return c.get<plmn_id_list2_r16_l>();
    }
    sn_pn_cfg_id_list_r18_l& snpn_cfg_id_list_r18()
    {
      assert_choice_type(types::snpn_cfg_id_list_r18, c.type, "identityList-r18");
      return c.get<sn_pn_cfg_id_list_r18_l>();
    }
    const plmn_id_list2_r16_l& plmn_id_list_r18() const
    {
      assert_choice_type(types::plmn_id_list_r18, c.type, "identityList-r18");
      return c.get<plmn_id_list2_r16_l>();
    }
    const sn_pn_cfg_id_list_r18_l& snpn_cfg_id_list_r18() const
    {
      assert_choice_type(types::snpn_cfg_id_list_r18, c.type, "identityList-r18");
      return c.get<sn_pn_cfg_id_list_r18_l>();
    }
    plmn_id_list2_r16_l&     set_plmn_id_list_r18();
    sn_pn_cfg_id_list_r18_l& set_snpn_cfg_id_list_r18();

  private:
    asn1::table::choice_storage<id_list_r18_c_, types, choice_buffer_t<plmn_id_list2_r16_l, sn_pn_cfg_id_list_r18_l>> c;
  };

  // member variables
  fixed_bitstring<48>      absolute_time_info_r16;
  trace_ref_r16_s          trace_ref_r16;
  fixed_octstring<2>       trace_recording_session_ref_r16;
  fixed_octstring<1>       tce_id_r16;
  log_meas_info_list_r16_l log_meas_info_list_r16;
  id_list_r18_c_           id_list_r18;

  static const asn1::table::seq_desc desc;
};

// VarMeasConfig ::= SEQUENCE
struct var_meas_cfg_s {
  struct s_measure_cfg_c_ {
    struct types_opts {
      enum options { ssb_rsrp, csi_rsrp, nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    uint8_t&                                      ssb_rsrp()
    {
      assert_choice_type(types::ssb_rsrp, c.type, "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    uint8_t& csi_rsrp()
    {
      assert_choice_type(types::csi_rsrp, c.type, "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& ssb_rsrp() const
    {
      assert_choice_type(types::ssb_rsrp, c.type, "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    const uint8_t& csi_rsrp() const
    {
      assert_choice_type(types::csi_rsrp, c.type, "s-MeasureConfig");
      return c.get<uint8_t>();
    }
    uint8_t& set_ssb_rsrp();
    uint8_t& set_csi_rsrp();

  private:
    asn1::table::choice_storage<s_measure_cfg_c_, types, pod_choice_buffer_t> c;
  };

  // member variables
  bool                         quant_cfg_present     = false;
  bool                         s_measure_cfg_present = false;
  meas_id_to_add_mod_list_l    meas_id_list;
  meas_obj_to_add_mod_list_l   meas_obj_list;
  report_cfg_to_add_mod_list_l report_cfg_list;
  quant_cfg_s                  quant_cfg;
  s_measure_cfg_c_             s_measure_cfg;

  static const asn1::table::seq_desc desc;
};

// VarMeasConfigSL-r16 ::= SEQUENCE
struct var_meas_cfg_sl_r16_s {
  bool                     sl_quant_cfg_r16_present = false;
  sl_meas_id_list_r16_l    sl_meas_id_list_r16;
  sl_meas_obj_list_r16_l   sl_meas_obj_list_r16;
  sl_report_cfg_list_r16_l sl_report_cfg_list_r16;
  sl_quant_cfg_r16_s       sl_quant_cfg_r16;

  static const asn1::table::seq_desc desc;
};

// VarMeasIdleConfig-r16 ::= SEQUENCE
struct var_meas_idle_cfg_r16_s {
  using meas_idle_carrier_list_nr_r16_l_    = dyn_array<meas_idle_carrier_nr_r16_s>;
  using meas_idle_carrier_list_eutra_r16_l_ = dyn_array<meas_idle_carrier_eutra_r16_s>;
  struct meas_idle_dur_r16_opts {
    enum options { sec10, sec30, sec60, sec120, sec180, sec240, sec300, spare, nulltype } value;

    static const asn1::table::enum_desc desc;
    uint16_t                            to_number() const;
  };
  using meas_idle_dur_r16_e_ = enumerated<meas_idle_dur_r16_opts>;

  // member variables
  meas_idle_carrier_list_nr_r16_l_    meas_idle_carrier_list_nr_r16;
  meas_idle_carrier_list_eutra_r16_l_ meas_idle_carrier_list_eutra_r16;
  meas_idle_dur_r16_e_                meas_idle_dur_r16;
  validity_area_list_r16_l            validity_area_list_r16;

  static const asn1::table::seq_desc desc;
};

// VarMeasIdleReport-r16 ::= SEQUENCE
struct var_meas_idle_report_r16_s {
  bool                         meas_report_idle_nr_r16_present    = false;
  bool                         meas_report_idle_eutra_r16_present = false;
  meas_result_idle_nr_r16_s    meas_report_idle_nr_r16;
  meas_result_idle_eutra_r16_s meas_report_idle_eutra_r16;

  static const asn1::table::seq_desc desc;
};

// VarMeasReport ::= SEQUENCE
struct var_meas_report_s {
  using cells_met_leaving_cond_r18_l_       = bounded_array<uint16_t, 8>;
  using reported_best_neighbour_cell_r18_l_ = bounded_array<uint16_t, 2>;

  // member variables
  bool                                cli_triggered_list_r16_present = false;
  uint8_t                             meas_id                        = 1;
  cells_triggered_list_l              cells_triggered_list;
  int64_t                             nof_reports_sent = 0;
  cli_triggered_list_r16_c            cli_triggered_list_r16;
  tx_pool_meas_list_r16_l             tx_pool_meas_to_add_mod_list_nr_r16;
  relays_triggered_list_r17_l         relays_triggered_list_r17;
  cells_met_leaving_cond_r18_l_       cells_met_leaving_cond_r18;
  reported_best_neighbour_cell_r18_l_ reported_best_neighbour_cell_r18;

  static const asn1::table::seq_desc desc;
};

// VarMeasReportList ::= SEQUENCE (SIZE (1..64)) OF VarMeasReport
using var_meas_report_list_l = dyn_array<var_meas_report_s>;

// VarMeasReportSL-r16 ::= SEQUENCE
struct var_meas_report_sl_r16_s {
  using sl_freq_triggered_list_r16_l_ = bounded_array<uint32_t, 8>;

  // member variables
  uint8_t                       sl_meas_id_r16 = 1;
  sl_freq_triggered_list_r16_l_ sl_freq_triggered_list_r16;
  int64_t                       sl_nof_reports_sent_r16 = 0;

  static const asn1::table::seq_desc desc;
};

// VarMeasReportListSL-r16 ::= SEQUENCE (SIZE (1..64)) OF VarMeasReportSL-r16
using var_meas_report_list_sl_r16_l = dyn_array<var_meas_report_sl_r16_s>;

// VarMeasReselectionConfig-r18 ::= SEQUENCE
struct var_meas_resel_cfg_r18_s {
  using meas_resel_carrier_list_nr_r18_l_ = dyn_array<meas_resel_carrier_nr_r18_s>;

  // member variables
  bool                              meas_resel_validity_dur_r18_present = false;
  meas_resel_carrier_list_nr_r18_l_ meas_resel_carrier_list_nr_r18;
  meas_validity_dur_r18_e           meas_resel_validity_dur_r18;

  static const asn1::table::seq_desc desc;
};

// VarMobilityHistoryReport-r17 ::= SEQUENCE
struct var_mob_history_report_r17_s {
  visited_cell_info_list_r16_l   visited_cell_info_list_r16;
  visited_pscell_info_list_r17_l visited_pscell_info_list_r17;

  static const asn1::table::seq_desc desc;
};

// VarPendingRNA-Update ::= SEQUENCE
struct var_pending_rna_upd_s {
  bool pending_rna_upd_present = false;
  bool pending_rna_upd         = false;

  static const asn1::table::seq_desc desc;
};

// VarRA-Report-r16 ::= SEQUENCE
struct var_ra_report_r16_s {
  struct id_list_r18_c_ {
    using snpn_id_list_r18_l_ = dyn_array<sn_pn_id_r18_s>;
    struct types_opts {
      enum options { plmn_id_list_r18, snpn_id_list_r18, nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    plmn_id_list2_r16_l&                          plmn_id_list_r18()
    {
      assert_choice_type(types::plmn_id_list_r18, c.type, "identityList-r18");
      return c.get<plmn_id_list2_r16_l>();
    }
    snpn_id_list_r18_l_& snpn_id_list_r18()
    {
      assert_choice_type(types::snpn_id_list_r18, c.type, "identityList-r18");
      return c.get<snpn_id_list_r18_l_>();
    }
    const plmn_id_list2_r16_l& plmn_id_list_r18() const
    {
      assert_choice_type(types::plmn_id_list_r18, c.type, "identityList-r18");
      return c.get<plmn_id_list2_r16_l>();
    }
    const snpn_id_list_r18_l_& snpn_id_list_r18() const
    {
      assert_choice_type(types::snpn_id_list_r18, c.type, "identityList-r18");
      return c.get<snpn_id_list_r18_l_>();
    }
    plmn_id_list2_r16_l& set_plmn_id_list_r18();
    snpn_id_list_r18_l_& set_snpn_id_list_r18();

  private:
    asn1::table::choice_storage<id_list_r18_c_, types, choice_buffer_t<plmn_id_list2_r16_l, snpn_id_list_r18_l_>> c;
  };

  // member variables
  ra_report_list_r16_l ra_report_list_r16;
  id_list_r18_c_       id_list_r18;

  static const asn1::table::seq_desc desc;
};

// VarRLF-Report-r16 ::= SEQUENCE
struct var_rlf_report_r16_s {
  struct id_list_r18_c_ {
    using snpn_id_list_r18_l_ = dyn_array<sn_pn_id_r18_s>;
    struct types_opts {
      enum options { plmn_id_list_r18, snpn_id_list_r18, nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    plmn_id_list2_r16_l&                          plmn_id_list_r18()
    {
      assert_choice_type(types::plmn_id_list_r18, c.type, "identityList-r18");
      return c.get<plmn_id_list2_r16_l>();
    }
    snpn_id_list_r18_l_& snpn_id_list_r18()
    {
      assert_choice_type(types::snpn_id_list_r18, c.type, "identityList-r18");
      return c.get<snpn_id_list_r18_l_>();
    }
    const plmn_id_list2_r16_l& plmn_id_list_r18() const
    {
      assert_choice_type(types::plmn_id_list_r18, c.type, "identityList-r18");
      return c.get<plmn_id_list2_r16_l>();
    }
    const snpn_id_list_r18_l_& snpn_id_list_r18() const
    {
      assert_choice_type(types::snpn_id_list_r18, c.type, "identityList-r18");
      return c.get<snpn_id_list_r18_l_>();
    }
    plmn_id_list2_r16_l& set_plmn_id_list_r18();
    snpn_id_list_r18_l_& set_snpn_id_list_r18();

  private:
    asn1::table::choice_storage<id_list_r18_c_, types, choice_buffer_t<plmn_id_list2_r16_l, snpn_id_list_r18_l_>> c;
  };

  // member variables
  rlf_report_r16_c rlf_report_r16;
  id_list_r18_c_   id_list_r18;

  static const asn1::table::seq_desc desc;
};

// VarResumeMAC-Input ::= SEQUENCE
struct var_resume_mac_input_s {
  uint16_t            source_pci = 0;
  fixed_bitstring<36> target_cell_id;
  uint32_t            source_c_rnti = 0;

  static const asn1::table::seq_desc desc;
};

// VarServingSecurityCellSetID ::= SEQUENCE
struct var_serving_security_cell_set_id_s {
  uint8_t serving_security_cell_set_id_r18 = 1;

  static const asn1::table::seq_desc desc;
};

// VarShortMAC-Input ::= SEQUENCE
struct var_short_mac_input_s {
  uint16_t            source_pci = 0;
  fixed_bitstring<36> target_cell_id;
  uint32_t            source_c_rnti = 0;

  static const asn1::table::seq_desc desc;
};

// VarSuccessHO-Report-r17 ::= SEQUENCE
struct var_success_ho_report_r17_s {
  struct id_list_r18_c_ {
    using snpn_id_list_r18_l_ = dyn_array<sn_pn_id_r18_s>;
    struct types_opts {
      enum options { plmn_id_list_r18, snpn_id_list_r18, nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    plmn_id_list2_r16_l&                          plmn_id_list_r18()
    {
      assert_choice_type(types::plmn_id_list_r18, c.type, "identityList-r18");
      return c.get<plmn_id_list2_r16_l>();
    }
    snpn_id_list_r18_l_& snpn_id_list_r18()
    {
      assert_choice_type(types::snpn_id_list_r18, c.type, "identityList-r18");
      return c.get<snpn_id_list_r18_l_>();
    }
    const plmn_id_list2_r16_l& plmn_id_list_r18() const
    {
      assert_choice_type(types::plmn_id_list_r18, c.type, "identityList-r18");
      return c.get<plmn_id_list2_r16_l>();
    }
    const snpn_id_list_r18_l_& snpn_id_list_r18() const
    {
      assert_choice_type(types::snpn_id_list_r18, c.type, "identityList-r18");
      return c.get<snpn_id_list_r18_l_>();
    }
    plmn_id_list2_r16_l& set_plmn_id_list_r18();
    snpn_id_list_r18_l_& set_snpn_id_list_r18();

  private:
    asn1::table::choice_storage<id_list_r18_c_, types, choice_buffer_t<plmn_id_list2_r16_l, snpn_id_list_r18_l_>> c;
  };

  // member variables
  success_ho_report_r17_s success_ho_report_r17;
  id_list_r18_c_          id_list_r18;

  static const asn1::table::seq_desc desc;
};

// VarSuccessPSCell-Report-r18 ::= SEQUENCE
struct var_success_pscell_report_r18_s {
  struct id_list_r18_c_ {
    using snpn_id_list_r18_l_ = dyn_array<sn_pn_id_r18_s>;
    struct types_opts {
      enum options { plmn_id_list_r18, snpn_id_list_r18, nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    static const asn1::table::choice_storage_desc storage_desc;
    void                                          set(types::options e = types::nulltype) { c.set(e); }
    types                                         type() const { return c.type; }
    static const asn1::table::choice_desc         desc;
    plmn_id_list2_r16_l&                          plmn_id_list_r18()
    {
      assert_choice_type(types::plmn_id_list_r18, c.type, "identityList-r18");
      return c.get<plmn_id_list2_r16_l>();
    }
    snpn_id_list_r18_l_& snpn_id_list_r18()
    {
      assert_choice_type(types::snpn_id_list_r18, c.type, "identityList-r18");
      return c.get<snpn_id_list_r18_l_>();
    }
    const plmn_id_list2_r16_l& plmn_id_list_r18() const
    {
      assert_choice_type(types::plmn_id_list_r18, c.type, "identityList-r18");
      return c.get<plmn_id_list2_r16_l>();
    }
    const snpn_id_list_r18_l_& snpn_id_list_r18() const
    {
      assert_choice_type(types::snpn_id_list_r18, c.type, "identityList-r18");
      return c.get<snpn_id_list_r18_l_>();
    }
    plmn_id_list2_r16_l& set_plmn_id_list_r18();
    snpn_id_list_r18_l_& set_snpn_id_list_r18();

  private:
    asn1::table::choice_storage<id_list_r18_c_, types, choice_buffer_t<plmn_id_list2_r16_l, snpn_id_list_r18_l_>> c;
  };

  // member variables
  success_pscell_report_r18_s success_pscell_report_r18;
  id_list_r18_c_              id_list_r18;

  static const asn1::table::seq_desc desc;
};

// VarTSS-Info-r18 ::= SEQUENCE
struct var_tss_info_r18_s {
  struct stored_global_gnb_id_r18_s_ {
    plmn_id_s                 plmn_id_r18;
    bounded_bitstring<22, 32> gnb_id_r18;
  };

  // member variables
  uint8_t                     stored_event_id_r18 = 0;
  stored_global_gnb_id_r18_s_ stored_global_gnb_id_r18;

  static const asn1::table::seq_desc desc;
};

} // namespace rrc_nr
} // namespace asn1

extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::rrc_nr::cells_triggered_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::rrc_nr::var_app_layer_idle_cfg_r18_s::app_layer_idle_cfg_list_r18_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::rrc_nr::var_app_layer_plmn_list_cfg_r18_s::plmn_cfg_list_r18_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<
    asn1::rrc_nr::var_conn_est_fail_report_list_r17_s::conn_est_fail_report_list_r17_l_::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::rrc_nr::var_meas_report_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::rrc_nr::var_meas_report_list_sl_r16_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::rrc_nr::var_ra_report_r16_s::id_list_r18_c_::snpn_id_list_r18_l_::value_type>::ops;
