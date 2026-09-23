// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcch.h"
#include "ocudu/adt/format.h"
#include "ocudu/fapi_adaptor/precoding_matrix_repository.h"

using namespace ocudu;
using namespace fapi_adaptor;

/// Fills the DL DCI parameters of the PDCCH processor PDU.
static void fill_dci(pdcch_processor::pdu_t&            proc_pdu,
                     const fapi::dl_pdcch_pdu&          fapi_pdu,
                     const precoding_matrix_repository& pm_repo)
{
  const auto& fapi_dci = fapi_pdu.dl_dci;

  pdcch_processor::dci_description& dci = proc_pdu.dci;

  dci.rnti                  = fapi_dci.rnti;
  dci.n_id_pdcch_data       = fapi_dci.nid_pdcch_data;
  dci.n_id_pdcch_dmrs       = fapi_dci.nid_pdcch_dmrs;
  dci.n_rnti                = fapi_dci.nrnti_pdcch_data;
  dci.cce_index             = fapi_dci.cce_index;
  dci.dci_aggregation_level = fapi_dci.dci_aggregation_level;

  if (const auto* profile_nr = std::get_if<fapi::dl_dci_pdu::power_profile_nr>(&fapi_dci.power_config)) {
    dci.dmrs_power_offset_dB = profile_nr->power_control_offset_ss_db;
    dci.data_power_offset_dB = dci.dmrs_power_offset_dB;
  } else if (const auto* profile_sss = std::get_if<fapi::dl_dci_pdu::power_profile_sss>(&fapi_dci.power_config)) {
    dci.dmrs_power_offset_dB = profile_sss->dmrs_power_offset_db;
    dci.data_power_offset_dB = profile_sss->data_power_offset_db;
  } else {
    report_error("PDCCH PDU power values are not configured");
  }

  // Unpack the payload.
  dci.payload.resize(fapi_dci.payload.size());
  fapi_dci.payload.to_unpacked_bits(span<uint8_t>{dci.payload.data(), dci.payload.size()});

  const fapi::tx_precoding_and_beamforming_pdu::prgs_info& prg = fapi_dci.precoding_and_beamforming.prg;
  if (prg.beams.empty()) {
    dci.precoding_and_beamforming =
        precoding_beamforming_configuration::make_wideband(pm_repo.get_precoding(prg.pm_index));
  } else {
    dci.precoding_and_beamforming = precoding_beamforming_configuration::make_wideband(prg.beams);
  }

  // Fill PDCCH context for logging.
  proc_pdu.context = fapi_pdu.dl_dci.context;
}

/// Fills the CORESET parameters of the PDCCH processor PDU.
static void fill_coreset(pdcch_processor::coreset_description& coreset, const fapi::dl_pdcch_pdu& fapi_pdu)
{
  coreset.bwp_size_rb         = fapi_pdu.coreset_bwp.length();
  coreset.bwp_start_rb        = fapi_pdu.coreset_bwp.start();
  coreset.start_symbol_index  = fapi_pdu.symbols.start();
  coreset.duration            = fapi_pdu.symbols.length();
  coreset.frequency_resources = fapi_pdu.freq_domain_resource;

  if (const auto* dci_coreset_0 = std::get_if<fapi::dl_pdcch_pdu::mapping_coreset_0>(&fapi_pdu.mapping)) {
    // The PDCCH is located in CORESET0.
    coreset.cce_to_reg_mapping = pdcch_processor::cce_to_reg_mapping_type::CORESET0;

    // The REG bundle size and interleaver size are ignored and CORESET 0 is always interleaved.
    coreset.reg_bundle_size  = 0;
    coreset.interleaver_size = 0;
    coreset.shift_index      = dci_coreset_0->interleaved.shift_index;

  } else if (const auto* dci_interleaved = std::get_if<fapi::dl_pdcch_pdu::mapping_interleaved>(&fapi_pdu.mapping)) {
    // The PDCCH is NOT located in CORESET0 and is interleaved.
    coreset.cce_to_reg_mapping = pdcch_processor::cce_to_reg_mapping_type::INTERLEAVED;

    coreset.reg_bundle_size  = dci_interleaved->interleaved.reg_bundle_sz;
    coreset.interleaver_size = dci_interleaved->interleaved.interleaver_sz;
    coreset.shift_index      = dci_interleaved->interleaved.shift_index;

  } else if (std::get_if<fapi::dl_pdcch_pdu::mapping_non_interleaved>(&fapi_pdu.mapping)) {
    // Non-interleaved case.
    coreset.cce_to_reg_mapping = pdcch_processor::cce_to_reg_mapping_type::NON_INTERLEAVED;

    // The REG bundle size and interleaver size are ignored.
    coreset.reg_bundle_size  = 0;
    coreset.interleaver_size = 0;
    coreset.shift_index      = 0;
  }
}

void ocudu::fapi_adaptor::convert_pdcch_fapi_to_phy(pdcch_processor::pdu_t&            proc_pdu,
                                                    const fapi::dl_pdcch_pdu&          fapi_pdu,
                                                    slot_point                         slot,
                                                    const precoding_matrix_repository& pm_repo)
{
  proc_pdu.slot = slot;
  proc_pdu.cp   = fapi_pdu.cp;

  fill_coreset(proc_pdu.coreset, fapi_pdu);

  fill_dci(proc_pdu, fapi_pdu, pm_repo);
}
