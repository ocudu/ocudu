// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/builders/tx_precoding_and_beamforming_pdu_builder.h"
#include "ocudu/fapi/p7/messages/dl_pdsch_pdu.h"
#include "ocudu/fapi/p7/messages/power_control_offset_ss.h"
#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/dmrs/dmrs.h"
#include "ocudu/ran/subcarrier_spacing.h"

namespace ocudu {
namespace fapi {

/// Builder that helps to fill the parameters of a DL PDSCH codeword.
class dl_pdsch_codeword_builder
{
public:
  explicit dl_pdsch_codeword_builder(dl_pdsch_codeword& cw_) : cw(cw_) {}

  /// \brief Sets the codeword basic parameters.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH PDU.
  dl_pdsch_codeword_builder& set_codeword_parameters(modulation_scheme qam_mod,
                                                     sch_mcs_index     mcs_index,
                                                     pdsch_mcs_table   mcs_table,
                                                     uint8_t           rv_index,
                                                     units::bytes      tb_size)
  {
    cw.qam_mod_order = qam_mod;
    cw.mcs_index     = mcs_index;
    cw.mcs_table     = mcs_table;
    cw.rv_index      = rv_index;
    cw.tb_size       = tb_size;

    return *this;
  }

private:
  dl_pdsch_codeword& cw;
};

/// DL PDSCH PDU builder that helps to fill the parameters specified in SCF-222 v4.0 section 3.4.2.2.
class dl_pdsch_pdu_builder
{
public:
  explicit dl_pdsch_pdu_builder(dl_pdsch_pdu& pdu_) : pdu(pdu_) {}

  /// \brief Sets the UE specific parameters for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH PDU.
  dl_pdsch_pdu_builder& set_ue_specific_parameters(rnti_t rnti)
  {
    pdu.rnti = rnti;

    return *this;
  }

  /// \brief Sets the BWP parameters for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH PDU.
  dl_pdsch_pdu_builder& set_bwp_parameters(crb_interval bwp, subcarrier_spacing scs, cyclic_prefix cp)
  {
    pdu.bwp = bwp;
    pdu.scs = scs;
    pdu.cp  = cp;

    return *this;
  }

  /// \brief Adds a codeword to the PDSCH PDU and returns a codeword builder to fill the codeword parameters.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH PDU.
  dl_pdsch_codeword_builder add_codeword()
  {
    dl_pdsch_codeword_builder builder(pdu.cws.emplace_back());

    return builder;
  }

  /// \brief Sets the codeword generation parameters for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH PDU.
  dl_pdsch_pdu_builder&
  set_codeword_generation_parameters(uint16_t n_id_pdsch, uint8_t num_layers, pdsch_ref_point_type ref_point)
  {
    pdu.nid_pdsch  = n_id_pdsch;
    pdu.num_layers = num_layers;
    pdu.ref_point  = ref_point;

    return *this;
  }

  /// \brief Sets the DMRS parameters for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH PDU.
  dl_pdsch_pdu_builder& set_dmrs_parameters(dmrs_symbol_mask dl_dmrs_symb_pos,
                                            dmrs_config_type dmrs_type,
                                            uint16_t         pdsch_dmrs_scrambling_id,
                                            uint8_t          nscid,
                                            uint8_t          num_dmrs_cdm_groups_no_data,
                                            dmrs_ports_mask  dmrs_ports)
  {
    pdu.dl_dmrs_symb_pos          = dl_dmrs_symb_pos;
    pdu.dmrs_type                 = dmrs_type;
    pdu.pdsch_dmrs_scrambling_id  = pdsch_dmrs_scrambling_id;
    pdu.nscid                     = nscid;
    pdu.num_dmrs_cdm_grps_no_data = num_dmrs_cdm_groups_no_data;
    pdu.dmrs_ports                = dmrs_ports;

    return *this;
  }

  /// \brief Sets the frequency allocation type 1 parameters for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH PDU.
  dl_pdsch_pdu_builder& set_frequency_allocation_type_1(vrb_interval vrbs, vrb_to_prb::mapping_type vrb_to_prb_mapping)
  {
    pdu.resource_alloc.vrbs = vrbs;
    pdu.vrb_to_prb_mapping  = vrb_to_prb_mapping;

    return *this;
  }

  /// \brief Sets the time allocation  parameters for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH PDU.
  dl_pdsch_pdu_builder& set_time_allocation_parameters(ofdm_symbol_range symbols)
  {
    pdu.symbols = symbols;

    return *this;
  }

  /// \brief Sets the profile NR Tx Power info parameters for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH PDU.
  dl_pdsch_pdu_builder& set_profile_nr_tx_power_info_parameters(int                     pwr_control_offset_db,
                                                                power_control_offset_ss pwr_control_offset_ss)
  {
    auto& power = pdu.power_config.emplace<dl_pdsch_pdu::power_profile_nr>();

    power.pwr_control_offset_db = pwr_control_offset_db;
    power.pwr_control_offset_ss = pwr_control_offset_ss;

    return *this;
  }

  /// \brief Sets the profile SSS Tx Power info parameters for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH PDU.
  dl_pdsch_pdu_builder& set_profile_sss_tx_power_info_parameters(float dmrs_pwr_offset_db, float data_pwr_offset_db)
  {
    auto& power              = pdu.power_config.emplace<dl_pdsch_pdu::power_profile_sss>();
    power.data_pwr_offset_db = data_pwr_offset_db;
    power.dmrs_pwr_offset_db = dmrs_pwr_offset_db;

    return *this;
  }

  /// \brief Sets the LDPC base graph for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH maintenance parameters v3.
  dl_pdsch_pdu_builder& set_ldpc_base_graph(ldpc_base_graph_type ldpc_base_graph)
  {
    pdu.ldpc_base_graph = ldpc_base_graph;

    return *this;
  }

  /// \brief Sets the VRB to PRB mapping for non interleaved common search space for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH maintenance parameters v3.
  dl_pdsch_pdu_builder& set_vrb_to_prb_non_interleaved_common_ss_parameters()
  {
    auto& pdu_vrb_to_prb_mapping           = pdu.mapping.emplace<dl_pdsch_pdu::non_interleaved_common_ss>();
    pdu_vrb_to_prb_mapping.N_start_coreset = 0U;

    return *this;
  }

  /// \brief Sets the VRB to PRB mapping for non interleaved other for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH maintenance parameters v3.
  dl_pdsch_pdu_builder& set_vrb_to_prb_non_interleaved_other_parameters()
  {
    pdu.mapping.emplace<dl_pdsch_pdu::non_interleaved_other>();

    return *this;
  }

  /// \brief Sets the VRB to PRB mapping for interleaved common type 0 coreset 0 for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH maintenance parameters v3.
  dl_pdsch_pdu_builder& set_vrb_to_prb_interleaved_common_type0_coreset0_parameters(unsigned N_start_coreset,
                                                                                    unsigned N_bwp_init_size)
  {
    auto& pdu_vrb_to_prb_mapping           = pdu.mapping.emplace<dl_pdsch_pdu::interleaved_common_type0_coreset0>();
    pdu_vrb_to_prb_mapping.N_start_coreset = N_start_coreset;
    pdu_vrb_to_prb_mapping.N_bwp_init_size = N_bwp_init_size;

    return *this;
  }

  /// \brief Sets the VRB to PRB mapping for interleaved common any coreset 0 present for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH maintenance parameters v3.
  dl_pdsch_pdu_builder& set_vrb_to_prb_interleaved_common_any_coreset0_present_parameters(unsigned N_start_coreset,
                                                                                          unsigned N_bwp_init_size)
  {
    auto& pdu_vrb_to_prb_mapping = pdu.mapping.emplace<dl_pdsch_pdu::interleaved_common_any_coreset0_present>();
    pdu_vrb_to_prb_mapping.N_start_coreset = N_start_coreset;
    pdu_vrb_to_prb_mapping.N_bwp_init_size = N_bwp_init_size;

    return *this;
  }

  /// \brief Sets the VRB to PRB mapping for interleaved other for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH maintenance parameters v3.
  dl_pdsch_pdu_builder& set_vrb_to_prb_interleaved_other_parameters()
  {
    pdu.mapping.emplace<dl_pdsch_pdu::interleaved_other>();

    return *this;
  }

  /// \brief Sets the codeword parameters for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH maintenance parameters v3.
  dl_pdsch_pdu_builder& set_codeword_parameters(units::bytes tb_size_lbrm)
  {
    pdu.tb_size_lbrm = tb_size_lbrm;

    return *this;
  }

  /// \brief Sets the number of CSI-RS PDUs for the fields of the PDSCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.2, in table PDSCH maintenance parameters v3.
  dl_pdsch_pdu_builder& set_number_of_csi_puds(uint16_t nof_csi_pdus)
  {
    pdu.nof_csi_pdus_for_rm = nof_csi_pdus;

    return *this;
  }

  /// Sets the PDSCH context as vendor specific.
  dl_pdsch_pdu_builder& set_context_vendor_specific(harq_id_t harq_id, unsigned k1, unsigned nof_retxs)
  {
    pdu.context = pdsch_context(harq_id, k1, nof_retxs);
    return *this;
  }

  /// Returns a transmission precoding and beamforming PDU builder of this PDSCH PDU.
  tx_precoding_and_beamforming_pdu_builder get_tx_precoding_and_beamforming_pdu_builder()
  {
    tx_precoding_and_beamforming_pdu_builder builder(pdu.precoding_and_beamforming);

    return builder;
  }

private:
  dl_pdsch_pdu& pdu;
};

} // namespace fapi
} // namespace ocudu
