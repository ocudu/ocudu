// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1ap_configuration_helpers.h"
#include "asn1_rrc_config_helpers.h"
#include "asn1_sys_info_packer.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/rrc_nr/bcch_dl_sch_msg.h"
#include "ocudu/asn1/rrc_nr/rrc_nr.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/support/error_handling.h"

using namespace ocudu;
using namespace odu;

byte_buffer ocudu::odu::make_asn1_meas_time_cfg_buffer(const du_cell_config& du_cfg)
{
  byte_buffer                     buf;
  asn1::bit_ref                   bref{buf};
  asn1::rrc_nr::meas_timing_cfg_s cfg;
  auto&                           meas_timing = cfg.crit_exts.set_c1().set_meas_timing_conf();
  meas_timing.meas_timing.resize(1);
  auto& meas_item = meas_timing.meas_timing[0];

  // MeasTiming
  meas_item.freq_and_timing_present = true;
  auto& freq_time                   = meas_item.freq_and_timing;
  freq_time.ssb_subcarrier_spacing  = get_asn1_scs(du_cfg.ran.ssb_cfg.scs);
  freq_time.carrier_freq            = du_cfg.ran.dl_cfg_common.freq_info_dl.absolute_frequency_ssb.value();

  // > Derive SSB periodicity, duration and offset.
  // TODO: Derive the correct duration.
  freq_time.ssb_meas_timing_cfg.dur.value = asn1::rrc_nr::ssb_mtc_s::dur_opts::sf5;
  // TODO: Derive the correct offset.
  switch (du_cfg.ran.ssb_cfg.ssb_period) {
    case ssb_periodicity::ms5:
      freq_time.ssb_meas_timing_cfg.periodicity_and_offset.set_sf5() = 0;
      break;
    case ssb_periodicity::ms10:
      freq_time.ssb_meas_timing_cfg.periodicity_and_offset.set_sf10() = 0;
      break;
    case ssb_periodicity::ms20:
      freq_time.ssb_meas_timing_cfg.periodicity_and_offset.set_sf20() = 0;
      break;
    case ssb_periodicity::ms40:
      freq_time.ssb_meas_timing_cfg.periodicity_and_offset.set_sf40() = 0;
      break;
    case ssb_periodicity::ms80:
      freq_time.ssb_meas_timing_cfg.periodicity_and_offset.set_sf80() = 0;
      break;
    case ssb_periodicity::ms160:
      freq_time.ssb_meas_timing_cfg.periodicity_and_offset.set_sf160() = 0;
      break;
    default:
      report_fatal_error("Invalid SSB periodicity {}.", fmt::underlying(du_cfg.ran.ssb_cfg.ssb_period));
  }
  meas_item.pci_present = true;
  meas_item.pci         = du_cfg.ran.pci;

  asn1::OCUDUASN_CODE ret = cfg.pack(bref);
  ocudu_assert(ret == asn1::OCUDUASN_SUCCESS, "Failed to pack meas_time_cfg");
  return buf;
}

du_served_cell_info ocudu::odu::make_f1ap_du_cell_info(const du_cell_config& du_cfg)
{
  du_served_cell_info serv_cell;

  serv_cell.nr_cgi     = du_cfg.nr_cgi;
  serv_cell.pci        = du_cfg.ran.pci;
  serv_cell.tac        = du_cfg.tac;
  serv_cell.duplx_mode = du_cfg.ran.tdd_cfg.has_value() ? duplex_mode::TDD : duplex_mode::FDD;
  serv_cell.scs_common = du_cfg.ran.dl_cfg_common.init_dl_bwp.generic_params.scs;
  serv_cell.dl_carrier = du_cfg.ran.dl_carrier;
  if (serv_cell.duplx_mode == duplex_mode::FDD) {
    serv_cell.ul_carrier = du_cfg.ran.ul_carrier;
  }
  serv_cell.packed_meas_time_cfg = make_asn1_meas_time_cfg_buffer(du_cfg);
  serv_cell.ntn_link_rtt =
      du_cfg.ran.ntn_params.has_value()
          ? du_cfg.ran.ntn_params->ntn_cfg.cell_specific_koffset.value_or(std::chrono::milliseconds(0))
          : std::chrono::milliseconds(0);

  return serv_cell;
}

gnb_du_sys_info ocudu::odu::make_f1ap_du_sys_info(const du_cell_config&     du_cfg,
                                                  std::string*              js_str,
                                                  std::vector<std::string>* si_json_strs)
{
  gnb_du_sys_info sys_info;

  // The CU-CP forwards this System Information to UEs, so it must state what the cell broadcasts in the normal
  // operation. An SI message carrying a warning is only broadcast while the warning is on air, and the MAC is the one
  // that lists it in SIB1 for as long as that lasts.
  sys_info.packed_mib  = asn1_packer::pack_mib(du_cfg);
  sys_info.packed_sib1 = asn1_packer::pack_sib1(du_cfg, js_str);

  // Pack extra SI messages.
  std::vector<std::string> all_msg_json;
  auto all_msgs = asn1_packer::pack_all_bcch_dl_sch_msgs(du_cfg, si_json_strs != nullptr ? &all_msg_json : nullptr);
  std::size_t nof_si_segments = 0;
  for (std::size_t i = 1; i < all_msgs.size(); ++i) {
    nof_si_segments += all_msgs[i].size();
  }
  sys_info.packed_si_msgs.reserve(nof_si_segments);
  if (si_json_strs != nullptr) {
    si_json_strs->clear();
    si_json_strs->reserve(nof_si_segments);
  }
  for (std::size_t i = 1; i < all_msgs.size(); ++i) {
    // Keep all encoded segments for each SI entry (SIB1 is at index 0 and is already copied to packed_sib1).
    for (auto& segment : all_msgs[i]) {
      sys_info.packed_si_msgs.push_back(std::move(segment));
      if (si_json_strs != nullptr && i < all_msg_json.size()) {
        // Segment-specific JSON is not currently generated; reuse the SI-entry JSON for all its segments.
        si_json_strs->push_back(all_msg_json[i]);
      }
    }
  }

  return sys_info;
}

void ocudu::odu::fill_f1_setup_request(f1_setup_request_message& req, const du_manager_params::ran_params& ran_params)
{
  req.gnb_du_id   = ran_params.gnb_du_id;
  req.gnb_du_name = ran_params.gnb_du_name;
  req.rrc_version = ran_params.rrc_version;

  req.served_cells.resize(ran_params.cells.size());

  for (unsigned i = 0; i != ran_params.cells.size(); ++i) {
    const du_cell_config& cell_cfg  = ran_params.cells[i];
    f1_cell_setup_params& serv_cell = req.served_cells[i];

    // Fill serving cell info.
    serv_cell.cell_index = to_du_cell_index(i);
    serv_cell.cell_info  = make_f1ap_du_cell_info(cell_cfg);
    for (const auto& slice : cell_cfg.rrm_policy_members) {
      serv_cell.slices.push_back(slice.rrc_member.s_nssai);
    }

    // Pack RRC ASN.1 Serving Cell system info.
    serv_cell.du_sys_info = make_f1ap_du_sys_info(cell_cfg, nullptr);
  }
}
