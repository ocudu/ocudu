// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcch.h"
#include "ocudu/adt/format.h"
#include "ocudu/fapi_adaptor/precoding_matrix_mapper.h"
#include "ocudu/scheduler/config/bwp_configuration.h"

using namespace ocudu;
using namespace fapi_adaptor;

static void fill_bwp_parameters(fapi::dl_pdcch_pdu_builder&  builder,
                                const bwp_configuration&     bwp_cfg,
                                const coreset_configuration& coreset_cfg)
{
  // According to the FreqDomainResource description field in the FAPI specs, for CORESET0 we need to take its starting
  // point and size, otherwise, for the rest of CORESETs take it from the BWP where the CORESET belongs.
  const crb_interval& crbs = (coreset_cfg.get_id() == to_coreset_id(0)) ? coreset_cfg.coreset0_crbs() : bwp_cfg.crbs;

  builder.set_bwp_parameters(crbs, bwp_cfg.scs, bwp_cfg.cp);
}

static freq_resource_bitmap calculate_coreset0_freq_res_bitmap(const coreset_configuration& coreset_cfg)
{
  freq_resource_bitmap freq_bitmap_coreset0(pdcch_constants::MAX_NOF_FREQ_RESOURCES);

  unsigned num_bits = coreset_cfg.coreset0_crbs().length() / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE;
  freq_bitmap_coreset0.fill(0, num_bits);

  return freq_bitmap_coreset0;
}

static void fill_coreset_parameters(fapi::dl_pdcch_pdu_builder&  builder,
                                    const coreset_configuration& coreset_cfg,
                                    unsigned                     start_symbol_index)
{
  builder.set_coreset_parameters(ofdm_symbol_range::start_and_len(start_symbol_index, coreset_cfg.duration()),
                                 coreset_cfg.get_precoder_granularity());
}

static void fill_precoding_and_beamforming(fapi::dl_dci_pdu_builder&             builder,
                                           const precoding_and_beamforming_info& mac_info,
                                           const precoding_matrix_mapper&        pm_mapper,
                                           unsigned                              cell_nof_prbs)
{
  fapi::tx_precoding_and_beamforming_pdu_builder pm_bf_builder = builder.get_tx_precoding_and_beamforming_pdu_builder();
  pm_bf_builder.set_prg_parameters(cell_nof_prbs);

  if (const auto* beam_id = std::get_if<beam_identifier>(&mac_info)) {
    pm_bf_builder.set_beams({*beam_id});

    return;
  }

  mac_pdcch_precoding_info info;
  pm_bf_builder.set_pmi(pm_mapper.map(info));
}

void ocudu::fapi_adaptor::convert_pdcch_mac_to_fapi(fapi::dl_pdcch_pdu_builder&    builder,
                                                    const dci_context_information& context_information,
                                                    const dci_payload&             payload,
                                                    const precoding_matrix_mapper& pm_mapper,
                                                    unsigned                       cell_nof_prbs)
{
  const coreset_configuration& coreset_cfg = *context_information.coreset_cfg;

  // Fill BWP parameters.
  fill_bwp_parameters(builder, *context_information.bwp_cfg, coreset_cfg);

  // Fill CORESET parameters.
  fill_coreset_parameters(builder, coreset_cfg, context_information.starting_symbol);

  if (coreset_cfg.get_id() == to_coreset_id(0)) {
    builder.set_coreset_0_parameters(*coreset_cfg.interleaved_mapping(),
                                     calculate_coreset0_freq_res_bitmap(coreset_cfg));
  } else if (coreset_cfg.interleaved_mapping().has_value()) {
    builder.set_interleaver_parameters(*coreset_cfg.interleaved_mapping(), coreset_cfg.freq_domain_resources());
  } else {
    builder.set_non_interleaver_parameters(coreset_cfg.freq_domain_resources());
  }

  // Fill the DCI.
  fapi::dl_dci_pdu_builder dci_builder = builder.get_dl_dci_pdu_builder();

  fill_precoding_and_beamforming(dci_builder, context_information.precoding_and_beamforming, pm_mapper, cell_nof_prbs);

  dci_builder.set_ue_specific_parameters(context_information.rnti)
      .set_control_channel_parameters(context_information.cces.ncce, context_information.cces.aggr_lvl)
      .set_data_scrambling_parameters(context_information.n_id_pdcch_data, context_information.n_rnti_pdcch_data)
      .set_profile_nr_tx_power_info_parameters(context_information.tx_pwr.pwr_ctrl_offset_ss)
      .set_dmrs_parameters(context_information.n_id_pdcch_dmrs)
      .set_payload(payload)
      .set_context_parameters(context_information.context.ss_id,
                              context_information.context.dci_format,
                              context_information.context.harq_feedback_timing);
}
