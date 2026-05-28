// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdsch.h"
#include "ocudu/fapi_adaptor/precoding_matrix_repository.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/resource_allocation/vrb_to_prb.h"
#include "ocudu/ran/sch/sch_dmrs_power.h"

using namespace ocudu;
using namespace fapi_adaptor;

/// Fills the reserved RE pattern list field in a PDSCH PDU.
static void fill_reserved_re_pattern(pdsch_processor::pdu_t&     proc_pdu,
                                     const fapi::dl_pdsch_pdu&   fapi_pdu,
                                     span<const re_pattern_list> csi_re_pattern_list)
{
  for (unsigned i = 0, e = fapi_pdu.nof_csi_pdus_for_rm; i != e; ++i) {
    ocudu_assert(i < csi_re_pattern_list.size(),
                 "CSI-RS PDU index={} value out of bounds CSI RE patterns={}",
                 i,
                 csi_re_pattern_list.size());

    proc_pdu.reserved.merge(csi_re_pattern_list[i]);
  }
}

/// Fills the codewords.
static void fill_codewords(pdsch_processor::pdu_t& proc_pdu, const fapi::dl_pdsch_pdu& fapi_pdu)
{
  for (const auto& cw : fapi_pdu.cws) {
    pdsch_processor::codeword_description codeword_descr;
    codeword_descr.modulation = static_cast<modulation_scheme>(cw.qam_mod_order);
    codeword_descr.rv         = cw.rv_index;
    proc_pdu.codewords.push_back(codeword_descr);
  }
}

static float get_power_control_offset_ss_dB(fapi::power_control_offset_ss power_control_offset_ss_profile_nr)
{
  switch (power_control_offset_ss_profile_nr) {
    case fapi::power_control_offset_ss::dB_minus_3:
      return -3.0F;
    case fapi::power_control_offset_ss::dB0:
      return +0.0F;
    case fapi::power_control_offset_ss::dB3:
      return +3.0F;
    case fapi::power_control_offset_ss::dB6:
    default:
      break;
  }
  return +6.0F;
}

/// Fills the power related parameters in the PDSCH PDU.
static void fill_power_values(pdsch_processor::pdu_t& proc_pdu, const fapi::dl_pdsch_pdu& fapi_pdu)
{
  if (const auto* profile_nr = std::get_if<fapi::dl_pdsch_pdu::power_profile_nr>(&fapi_pdu.power_config)) {
    proc_pdu.ratio_pdsch_data_to_sss_dB = get_power_control_offset_ss_dB(profile_nr->pwr_control_offset_ss) +
                                          static_cast<float>(profile_nr->pwr_control_offset_db);

    // Determine the PDSCH DMRS power from the PDSCH data power as per TS38.214 Table 4.1-1.
    proc_pdu.ratio_pdsch_dmrs_to_sss_dB =
        proc_pdu.ratio_pdsch_data_to_sss_dB + get_sch_to_dmrs_ratio_dB(fapi_pdu.num_dmrs_cdm_grps_no_data);
  } else if (const auto* profile_sss = std::get_if<fapi::dl_pdsch_pdu::power_profile_sss>(&fapi_pdu.power_config)) {
    proc_pdu.ratio_pdsch_dmrs_to_sss_dB = profile_sss->dmrs_pwr_offset_db;
    proc_pdu.ratio_pdsch_data_to_sss_dB = profile_sss->data_pwr_offset_db;
  }
}

/// Fills the rb_allocation parameter of the PDSCH PDU.
static void fill_rb_allocation(pdsch_processor::pdu_t& proc_pdu, const fapi::dl_pdsch_pdu& fapi_pdu)
{
  vrb_to_prb::configuration vrb_to_prb_config;

  if (const auto* non_interleaved_common_ss_mapping =
          std::get_if<fapi::dl_pdsch_pdu::non_interleaved_common_ss>(&fapi_pdu.mapping)) {
    vrb_to_prb_config =
        vrb_to_prb::create_non_interleaved_common_ss(non_interleaved_common_ss_mapping->N_start_coreset);
  } else if (std::holds_alternative<fapi::dl_pdsch_pdu::non_interleaved_other>(fapi_pdu.mapping)) {
    vrb_to_prb_config = vrb_to_prb::create_non_interleaved_other();
  } else if (const auto* interleaved_common_type0_coreset0_mapping =
                 std::get_if<fapi::dl_pdsch_pdu::interleaved_common_type0_coreset0>(&fapi_pdu.mapping)) {
    vrb_to_prb_config =
        vrb_to_prb::create_interleaved_coreset0(interleaved_common_type0_coreset0_mapping->N_start_coreset,
                                                interleaved_common_type0_coreset0_mapping->N_bwp_init_size);
  } else if (const auto* interleaved_common_any_coreset0_present_mapping =
                 std::get_if<fapi::dl_pdsch_pdu::interleaved_common_any_coreset0_present>(&fapi_pdu.mapping)) {
    vrb_to_prb_config =
        vrb_to_prb::create_interleaved_common_ss(interleaved_common_any_coreset0_present_mapping->N_start_coreset,
                                                 fapi_pdu.bwp.start(),
                                                 interleaved_common_any_coreset0_present_mapping->N_bwp_init_size);
  } else if (std::holds_alternative<fapi::dl_pdsch_pdu::interleaved_other>(fapi_pdu.mapping)) {
    vrb_to_prb_config =
        vrb_to_prb::create_interleaved_other(fapi_pdu.bwp.start(), fapi_pdu.bwp.length(), fapi_pdu.vrb_to_prb_mapping);
  } else {
    report_error("Unsupported VRB to PRB mapping in PDSCH PDU");
  }

  proc_pdu.freq_alloc = rb_allocation::make_type1(
      fapi_pdu.resource_alloc.vrbs.start(), fapi_pdu.resource_alloc.vrbs.length(), vrb_to_prb_config);
}

void ocudu::fapi_adaptor::convert_pdsch_fapi_to_phy(pdsch_processor::pdu_t&            proc_pdu,
                                                    const fapi::dl_pdsch_pdu&          fapi_pdu,
                                                    slot_point                         slot,
                                                    span<const re_pattern_list>        csi_re_pattern_list,
                                                    const precoding_matrix_repository& pm_repo)
{
  proc_pdu.slot         = slot;
  proc_pdu.rnti         = to_value(fapi_pdu.rnti);
  proc_pdu.bwp_size_rb  = fapi_pdu.bwp.length();
  proc_pdu.bwp_start_rb = fapi_pdu.bwp.start();
  proc_pdu.cp           = fapi_pdu.cp;

  fill_codewords(proc_pdu, fapi_pdu);

  proc_pdu.n_id = fapi_pdu.nid_pdsch;

  proc_pdu.dmrs_symbol_mask = fapi_pdu.dl_dmrs_symb_pos;

  proc_pdu.ref_point = (fapi_pdu.ref_point == fapi::pdsch_ref_point_type::point_a) ? pdsch_processor::pdu_t::CRB0
                                                                                   : pdsch_processor::pdu_t::PRB0;

  proc_pdu.dmrs                        = fapi_pdu.dmrs_type;
  proc_pdu.scrambling_id               = fapi_pdu.pdsch_dmrs_scrambling_id;
  proc_pdu.n_scid                      = fapi_pdu.nscid == 1U;
  proc_pdu.nof_cdm_groups_without_data = fapi_pdu.num_dmrs_cdm_grps_no_data;
  proc_pdu.start_symbol_index          = fapi_pdu.symbols.start();
  proc_pdu.nof_symbols                 = fapi_pdu.symbols.length();

  fill_rb_allocation(proc_pdu, fapi_pdu);

  fill_power_values(proc_pdu, fapi_pdu);

  proc_pdu.ldpc_base_graph = fapi_pdu.ldpc_base_graph;
  proc_pdu.tbs_lbrm        = fapi_pdu.tb_size_lbrm;

  fill_reserved_re_pattern(proc_pdu, fapi_pdu, csi_re_pattern_list);

  proc_pdu.precoding = precoding_configuration::make_wideband(
      pm_repo.get_precoding_matrix(fapi_pdu.precoding_and_beamforming.prg.pm_index));

  // Fill PDSCH context for logging.
  proc_pdu.context = fapi_pdu.context;
}
