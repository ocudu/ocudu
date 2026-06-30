// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdsch.h"
#include "ocudu/fapi_adaptor/precoding_matrix_mapper.h"
#include "ocudu/mac/mac_cell_result.h"
#include "ocudu/ran/resource_allocation/vrb_to_prb.h"
#include "ocudu/ran/sch/sch_constants.h"
#include "ocudu/scheduler/result/pdsch_info.h"

using namespace ocudu;
using namespace fapi_adaptor;

static crb_interval get_crb_interval(const pdsch_information& pdsch_cfg)
{
  if (pdsch_cfg.coreset_cfg->get_id() == to_coreset_id(0)) {
    return pdsch_cfg.coreset_cfg->coreset0_crbs();
  }
  return pdsch_cfg.bwp_cfg->crbs;
}

static void
fill_codewords(fapi::dl_pdsch_pdu_builder& builder, span<const pdsch_codeword> codewords, pdsch_mcs_table mcs_table)
{
  ocudu_assert(codewords.size() == 1, "Current FAPI implementation only supports 1 transport block per PDU");
  for (const auto& cw : codewords) {
    fapi::dl_pdsch_codeword_builder cw_builder = builder.add_codeword();
    cw_builder.set_codeword_parameters(
        cw.mcs_descr.modulation, cw.mcs_index, mcs_table, cw.rv_index, units::bytes{cw.tb_size_bytes});
  }

  const pdsch_codeword& cw = codewords.front();

  // NOTE: MAC uses the value of the target code rate x[1024], as per TS 38.214, Section 5.1.3.1, table 5.1.3.1-1.
  float R = cw.mcs_descr.get_normalised_target_code_rate();
  builder.set_ldpc_base_graph(get_ldpc_base_graph(R, units::bytes{cw.tb_size_bytes}.to_bits()))
      .set_codeword_parameters(tbs_lbrm_default);
}

static void fill_codeword_information(fapi::dl_pdsch_pdu_builder& builder,
                                      unsigned                    nid_pdsch,
                                      fapi::pdsch_ref_point_type  ref_point,
                                      unsigned                    nof_layers)
{
  builder.set_codeword_generation_parameters(nid_pdsch, nof_layers, ref_point);
}

static void fill_dmrs(fapi::dl_pdsch_pdu_builder& builder, const dmrs_information& dmrs)
{
  builder.set_dmrs_parameters(dmrs.dmrs_symb_pos,
                              dmrs.config_type,
                              dmrs.dmrs_scrambling_id,
                              dmrs.n_scid,
                              dmrs.num_dmrs_cdm_grps_no_data,
                              dmrs.dmrs_ports);
}

static void
fill_frequency_allocation(fapi::dl_pdsch_pdu_builder& builder, const vrb_alloc& rbs, vrb_to_prb::mapping_type mapping)
{
  const vrb_interval& vrbs = rbs.type1();
  builder.set_frequency_allocation_type_1(vrbs, mapping);
}

static void fill_time_allocation(fapi::dl_pdsch_pdu_builder& builder, const ofdm_symbol_range& symbols)
{
  builder.set_time_allocation_parameters(symbols);
}

static void fill_power_parameters(fapi::dl_pdsch_pdu_builder& builder, const tx_power_pdsch_information& power_params)
{
  builder.set_profile_nr_tx_power_info_parameters(power_params.pwr_ctrl_offset,
                                                  fapi::to_power_control_offset_ss(power_params.pwr_ctrl_offset_ss));
}

static void fill_precoding_and_beamforming(fapi::dl_pdsch_pdu_builder&                builder,
                                           const std::optional<pdsch_precoding_info>& mac_info,
                                           const precoding_matrix_mapper&             pm_mapper,
                                           unsigned                                   nof_layers,
                                           unsigned                                   cell_nof_prbs)
{
  fapi::tx_precoding_and_beamforming_pdu_builder pm_bf_builder = builder.get_tx_precoding_and_beamforming_pdu_builder();
  pm_bf_builder.set_prg_parameters((mac_info) ? mac_info->nof_rbs_per_prg : cell_nof_prbs);

  if (!mac_info) {
    mac_pdsch_precoding_info info;
    pm_bf_builder.set_pmi(pm_mapper.map(info, nof_layers));

    return;
  }

  for (const auto& prg : mac_info->prg_infos) {
    mac_pdsch_precoding_info info;
    info.report = prg;
    pm_bf_builder.set_pmi(pm_mapper.map(info, nof_layers));
  }
}

static void fill_omnidirectional_precoding(fapi::dl_pdsch_pdu_builder&    builder,
                                           const precoding_matrix_mapper& pm_mapper,
                                           unsigned                       nof_layers,
                                           unsigned                       cell_nof_prbs)
{
  fapi::tx_precoding_and_beamforming_pdu_builder pm_bf_builder = builder.get_tx_precoding_and_beamforming_pdu_builder();
  pm_bf_builder.set_prg_parameters(cell_nof_prbs);
  pm_bf_builder.set_pmi(pm_mapper.map({}, nof_layers));
}

static void fill_pdsch_information(fapi::dl_pdsch_pdu_builder& builder, const pdsch_information& pdsch_cfg)
{
  // Basic parameters.
  builder.set_ue_specific_parameters(pdsch_cfg.rnti);

  // Codewords.
  fill_codewords(builder, pdsch_cfg.codewords, pdsch_cfg.mcs_table);

  // DMRS.
  fill_dmrs(builder, pdsch_cfg.dmrs);

  // Time allocation.
  fill_time_allocation(builder, pdsch_cfg.symbols);

  // Power parameters.
  fill_power_parameters(builder, pdsch_cfg.tx_pwr_info);
}

static void fill_pdsch_vrb_to_prb_configuration(fapi::dl_pdsch_pdu_builder& builder, const pdsch_information& mac_pdu)
{
  bool is_dci_1_0 = mac_pdu.dci_fmt == dci_dl_format::f1_0;

  // Non-interleaved cases.
  if (mac_pdu.vrb_prb_mapping == vrb_to_prb::mapping_type::non_interleaved) {
    if (is_dci_1_0 && is_common_search_space(mac_pdu.ss_set_type)) {
      builder.set_vrb_to_prb_non_interleaved_common_ss_parameters();
      return;
    }

    builder.set_vrb_to_prb_non_interleaved_other_parameters();
    return;
  }

  // Interleaved cases for DCI 1_0, CORESET0 and in Common Search Space.
  ocudu_assert(mac_pdu.coreset_cfg != nullptr, "CORESET configuration cannot be null for interleaved PDSCH mapping");

  if (mac_pdu.coreset_cfg->get_id() == to_coreset_id(0) && is_common_search_space(mac_pdu.ss_set_type) && is_dci_1_0) {
    const crb_interval& crbs            = get_crb_interval(mac_pdu);
    unsigned            N_start_coreset = mac_pdu.coreset_cfg->get_coreset_start_crb() - crbs.start();
    unsigned            N_bwp_init_size = mac_pdu.coreset_cfg->coreset0_crbs().length();

    if (mac_pdu.ss_set_type == search_space_set_type::type0) {
      builder.set_vrb_to_prb_interleaved_common_type0_coreset0_parameters(N_start_coreset, N_bwp_init_size);
      return;
    }

    builder.set_vrb_to_prb_interleaved_common_any_coreset0_present_parameters(N_start_coreset, N_bwp_init_size);
    return;
  }

  builder.set_vrb_to_prb_interleaved_other_parameters();
}

void ocudu::fapi_adaptor::convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder&    builder,
                                                    const sib_information&         mac_pdu,
                                                    unsigned                       nof_csi_pdus,
                                                    const precoding_matrix_mapper& pm_mapper,
                                                    unsigned                       cell_nof_prbs)
{
  ocudu_assert(mac_pdu.pdsch_cfg.codewords.size() == 1, "This version only supports one transport block");
  ocudu_assert(mac_pdu.pdsch_cfg.coreset_cfg, "Invalid CORESET configuration");

  // Fill all the parameters contained in the MAC PDSCH information struct.
  fill_pdsch_information(builder, mac_pdu.pdsch_cfg);

  // Omnidirectional precoding.
  fill_omnidirectional_precoding(builder, pm_mapper, mac_pdu.pdsch_cfg.nof_layers, cell_nof_prbs);

  // Codeword information.
  fill_codeword_information(builder,
                            mac_pdu.pdsch_cfg.n_id,
                            (mac_pdu.si_indicator == sib_information::other_si)
                                ? fapi::pdsch_ref_point_type::point_a
                                : fapi::pdsch_ref_point_type::subcarrier_0,
                            mac_pdu.pdsch_cfg.nof_layers);
  // BWP parameters.
  const crb_interval& crbs = get_crb_interval(mac_pdu.pdsch_cfg);
  builder.set_bwp_parameters(crbs, mac_pdu.pdsch_cfg.bwp_cfg->scs, mac_pdu.pdsch_cfg.bwp_cfg->cp);

  // Frequency allocation.
  // Note: As defined in TS38.214 Section 5.1.2.3, DCI format 1_0 uses bundle size of 2.
  fill_frequency_allocation(builder, mac_pdu.pdsch_cfg.rbs, mac_pdu.pdsch_cfg.vrb_prb_mapping);

  builder.set_number_of_csi_puds(nof_csi_pdus);

  // Get the VRB-to-PRB mapping from the DCI.
  fill_pdsch_vrb_to_prb_configuration(builder, mac_pdu.pdsch_cfg);
}

void ocudu::fapi_adaptor::convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder&    builder,
                                                    const rar_information&         mac_pdu,
                                                    unsigned                       nof_csi_pdus,
                                                    const precoding_matrix_mapper& pm_mapper,
                                                    unsigned                       cell_nof_prbs)
{
  ocudu_assert(mac_pdu.pdsch_cfg.codewords.size() == 1, "This version only supports one transport block");
  ocudu_assert(mac_pdu.pdsch_cfg.coreset_cfg, "Invalid CORESET configuration");

  // Fill all the parameters contained in the MAC PDSCH information struct.
  fill_pdsch_information(builder, mac_pdu.pdsch_cfg);

  // Omnidirectional precoding.
  fill_omnidirectional_precoding(builder, pm_mapper, mac_pdu.pdsch_cfg.nof_layers, cell_nof_prbs);

  // Codeword information.
  fill_codeword_information(
      builder, mac_pdu.pdsch_cfg.n_id, fapi::pdsch_ref_point_type::point_a, mac_pdu.pdsch_cfg.nof_layers);

  // BWP parameters.
  const crb_interval& crbs = get_crb_interval(mac_pdu.pdsch_cfg);
  builder.set_bwp_parameters(crbs, mac_pdu.pdsch_cfg.bwp_cfg->scs, mac_pdu.pdsch_cfg.bwp_cfg->cp);

  // Frequency allocation.
  // Note: As defined in TS38.214 Section 5.1.2.3, DCI format 1_0 uses bundle size of 2.
  fill_frequency_allocation(builder, mac_pdu.pdsch_cfg.rbs, mac_pdu.pdsch_cfg.vrb_prb_mapping);

  builder.set_number_of_csi_puds(nof_csi_pdus);

  // Get the VRB-to-PRB mapping from the DCI.
  fill_pdsch_vrb_to_prb_configuration(builder, mac_pdu.pdsch_cfg);
}

void ocudu::fapi_adaptor::convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder&    builder,
                                                    const dl_msg_alloc&            mac_pdu,
                                                    unsigned                       nof_csi_pdus,
                                                    const precoding_matrix_mapper& pm_mapper,
                                                    unsigned                       cell_nof_prbs)
{
  ocudu_assert(mac_pdu.pdsch_cfg.codewords.size() == 1, "This version only supports one transport block");
  ocudu_assert(mac_pdu.pdsch_cfg.coreset_cfg, "Invalid CORESET configuration");

  // Fill all the parameters contained in the MAC PDSCH information struct.
  fill_pdsch_information(builder, mac_pdu.pdsch_cfg);

  // Precoding and beamforming.
  fill_precoding_and_beamforming(
      builder, mac_pdu.pdsch_cfg.precoding, pm_mapper, mac_pdu.pdsch_cfg.nof_layers, cell_nof_prbs);

  // Codeword information.
  fill_codeword_information(
      builder, mac_pdu.pdsch_cfg.n_id, fapi::pdsch_ref_point_type::point_a, mac_pdu.pdsch_cfg.nof_layers);

  // BWP parameters.
  const crb_interval& crbs = get_crb_interval(mac_pdu.pdsch_cfg);
  builder.set_bwp_parameters(crbs, mac_pdu.pdsch_cfg.bwp_cfg->scs, mac_pdu.pdsch_cfg.bwp_cfg->cp);

  // Frequency allocation.
  // Note: As defined in TS38.214 Section 5.1.2.3, DCI format 1_0 uses bundle size of 2.
  fill_frequency_allocation(builder, mac_pdu.pdsch_cfg.rbs, mac_pdu.pdsch_cfg.vrb_prb_mapping);

  // Fill PDSCH context for logging.
  builder.set_context_vendor_specific(mac_pdu.pdsch_cfg.harq_id, mac_pdu.context.k1, mac_pdu.context.nof_retxs);

  builder.set_number_of_csi_puds(nof_csi_pdus);

  // Get the VRB-to-PRB mapping from the DCI.
  fill_pdsch_vrb_to_prb_configuration(builder, mac_pdu.pdsch_cfg);
}

void ocudu::fapi_adaptor::convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder&    builder,
                                                    const dl_paging_allocation&    mac_pdu,
                                                    unsigned                       nof_csi_pdus,
                                                    const precoding_matrix_mapper& pm_mapper,
                                                    unsigned                       cell_nof_prbs)
{
  ocudu_assert(mac_pdu.pdsch_cfg.codewords.size() == 1, "This version only supports one transport block");
  ocudu_assert(mac_pdu.pdsch_cfg.coreset_cfg, "Invalid CORESET configuration");

  // Fill all the parameters contained in the MAC PDSCH information struct.
  fill_pdsch_information(builder, mac_pdu.pdsch_cfg);

  // Omnidirectional precoding.
  fill_omnidirectional_precoding(builder, pm_mapper, mac_pdu.pdsch_cfg.nof_layers, cell_nof_prbs);

  // Codeword information.
  fill_codeword_information(
      builder, mac_pdu.pdsch_cfg.n_id, fapi::pdsch_ref_point_type::point_a, mac_pdu.pdsch_cfg.nof_layers);

  // BWP parameters.
  const crb_interval& crbs = get_crb_interval(mac_pdu.pdsch_cfg);
  builder.set_bwp_parameters(crbs, mac_pdu.pdsch_cfg.bwp_cfg->scs, mac_pdu.pdsch_cfg.bwp_cfg->cp);

  // Frequency allocation.
  // Note: As defined in TS38.214 Section 5.1.2.3, DCI format 1_0 uses bundle size of 2.
  fill_frequency_allocation(builder, mac_pdu.pdsch_cfg.rbs, mac_pdu.pdsch_cfg.vrb_prb_mapping);

  builder.set_number_of_csi_puds(nof_csi_pdus);

  // Get the VRB-to-PRB mapping from the DCI.
  fill_pdsch_vrb_to_prb_configuration(builder, mac_pdu.pdsch_cfg);
}
