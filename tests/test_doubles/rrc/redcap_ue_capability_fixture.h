// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/asn1/rrc_nr/rrc_nr.h"
#include "ocudu/asn1/rrc_nr/ue_cap.h"
#include "ocudu/support/error_handling.h"

namespace ocudu::test_helpers {

/// Returns a captured Release-17 RedCap NR capability container.
inline byte_buffer make_captured_redcap_nr_capability()
{
  return make_byte_buffer(
             "e1a5500050280c07814060139e80d58000f1480005820020301262e001387a16003120d00980c0311c00aa3626eb04610d04"
             "083a4220028b64c15090e7d033ec09f18e7cc31e50b0381c3c09c00700007001c00007004001400100389a00000000404d1d"
             "00000e010103d088800e001c0000040500603009401800180f80000800000001919400040000006c00005238a1e001700140"
             "0010000000801002050a510100b11894080004")
      .value();
}

/// Returns RedCap NR capabilities with the requested PDCP long-SN support.
inline byte_buffer make_redcap_nr_capability(bool long_sn_supported)
{
  auto                      captured = make_captured_redcap_nr_capability();
  asn1::rrc_nr::ue_nr_cap_s cap;
  asn1::cbit_ref            reader{captured};
  report_fatal_error_if_not(cap.unpack(reader) == asn1::OCUDUASN_SUCCESS, "Invalid capability fixture");
  cap.pdcp_params.long_sn_red_cap_r17_present = long_sn_supported;
  byte_buffer   packed_cap;
  asn1::bit_ref cap_writer{packed_cap};
  report_fatal_error_if_not(cap.pack(cap_writer) == asn1::OCUDUASN_SUCCESS, "Invalid capability fixture");

  return packed_cap;
}

/// Returns RedCap capabilities and a complete one-DRB source AS configuration.
inline byte_buffer make_redcap_handover_preparation(bool long_sn_supported, unsigned source_sn_bits = 12)
{
  auto                      packed_cap = make_redcap_nr_capability(long_sn_supported);
  asn1::rrc_nr::rrc_recfg_s reconfiguration;
  auto&                     recfg = reconfiguration.crit_exts.set_rrc_recfg();
  recfg.radio_bearer_cfg_present  = true;
  recfg.radio_bearer_cfg.drb_to_add_mod_list.resize(1);
  auto& drb                = recfg.radio_bearer_cfg.drb_to_add_mod_list[0];
  drb.drb_id               = 1;
  drb.pdcp_cfg_present     = true;
  drb.pdcp_cfg.drb_present = true;
  drb.pdcp_cfg.drb.hdr_compress.set_not_used();
  drb.pdcp_cfg.drb.pdcp_sn_size_dl_present = true;
  drb.pdcp_cfg.drb.pdcp_sn_size_ul_present = true;
  asn1::number_to_enum(drb.pdcp_cfg.drb.pdcp_sn_size_dl, source_sn_bits);
  asn1::number_to_enum(drb.pdcp_cfg.drb.pdcp_sn_size_ul, source_sn_bits);
  drb.cn_assoc_present = true;
  auto& sdap           = drb.cn_assoc.set_sdap_cfg();
  sdap.pdu_session     = 1;
  sdap.sdap_hdr_dl     = asn1::rrc_nr::sdap_cfg_s::sdap_hdr_dl_opts::absent;
  sdap.sdap_hdr_ul     = asn1::rrc_nr::sdap_cfg_s::sdap_hdr_ul_opts::absent;
  sdap.default_drb     = true;
  sdap.mapped_qos_flows_to_add.push_back(1);
  byte_buffer   packed_recfg;
  asn1::bit_ref recfg_writer{packed_recfg};
  report_fatal_error_if_not(reconfiguration.pack(recfg_writer) == asn1::OCUDUASN_SUCCESS,
                            "Invalid source configuration");

  asn1::rrc_nr::ho_prep_info_s info;
  auto&                        ies = info.crit_exts.set_c1().set_ho_prep_info();
  ies.ue_cap_rat_list.resize(1);
  ies.ue_cap_rat_list[0].rat_type             = asn1::rrc_nr::rat_type_opts::nr;
  ies.ue_cap_rat_list[0].ue_cap_rat_container = std::move(packed_cap);
  ies.source_cfg_present                      = true;
  ies.source_cfg.rrc_recfg                    = std::move(packed_recfg);
  byte_buffer   result;
  asn1::bit_ref writer{result};
  report_fatal_error_if_not(info.pack(writer) == asn1::OCUDUASN_SUCCESS, "Invalid handover fixture");
  return result;
}

} // namespace ocudu::test_helpers
