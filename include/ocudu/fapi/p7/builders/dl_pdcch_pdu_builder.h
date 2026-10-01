// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/builders/tx_precoding_and_beamforming_pdu_builder.h"
#include "ocudu/fapi/p7/messages/dl_pdcch_pdu.h"

namespace ocudu {
namespace fapi {

/// Helper class to fill in the DL DCI PDU parameters specified in SCF-222 v4.0 section 3.4.2.1, including the PDCCH PDU
/// maintenance FAPIv3 and PDCCH PDU FAPIv4 parameters.
class dl_dci_pdu_builder
{
public:
  explicit dl_dci_pdu_builder(dl_dci_pdu& pdu_) : pdu(pdu_) {}

  /// \brief Sets the UE specific parameters for the fields of the DL DCI PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.1, in table DL DCI PDU.
  dl_dci_pdu_builder& set_ue_specific_parameters(rnti_t rnti)
  {
    pdu.rnti = rnti;

    return *this;
  }

  /// \brief Sets the control channel parameters for the fields of the DL DCI PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.1, in table DL DCI PDU.
  dl_dci_pdu_builder& set_control_channel_parameters(uint8_t cce_index, aggregation_level dci_aggregation_level)
  {
    pdu.cce_index             = cce_index;
    pdu.dci_aggregation_level = dci_aggregation_level;

    return *this;
  }

  /// \brief Sets the data scrambling parameters for the fields of the DL DCI PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.1, in table DL DCI PDU.
  dl_dci_pdu_builder& set_data_scrambling_parameters(uint16_t nid_pdcch_data, uint16_t nrnti_pdcch_data)
  {
    pdu.nid_pdcch_data   = nid_pdcch_data;
    pdu.nrnti_pdcch_data = nrnti_pdcch_data;

    return *this;
  }

  /// \brief Sets the profile NR Tx Power info parameters for the fields of the DL DCI PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.1, in table DL DCI PDU.
  dl_dci_pdu_builder& set_profile_nr_tx_power_info_parameters(int power_control_offset_ss_dB)
  {
    auto& power                      = pdu.power_config.emplace<dl_dci_pdu::power_profile_nr>();
    power.power_control_offset_ss_db = power_control_offset_ss_dB;

    return *this;
  }

  /// \brief Sets the profile SSS Tx Power info parameters for the fields of the DL DCI PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.1, in table PDCCH PDU maintenance PDU.
  dl_dci_pdu_builder& set_profile_sss_tx_power_info_parameters(float dmrs_offset_db, float data_offset_db)
  {
    auto& power                = pdu.power_config.emplace<dl_dci_pdu::power_profile_sss>();
    power.dmrs_power_offset_db = dmrs_offset_db;
    power.data_power_offset_db = data_offset_db;

    return *this;
  }

  /// \brief Sets the payload of the DL DCI PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.1, in table DL DCI PDU.
  dl_dci_pdu_builder& set_payload(const dci_payload& payload)
  {
    pdu.payload = payload;

    return *this;
  }

  /// \brief Sets the DCI parameters of the PDCCH parameters v4.
  ///
  /// These DMRS parameters as specified in SCF-222 v4.0 section 3.4.2.1, in table PDCCH PDU parameters FAPIv4.
  dl_dci_pdu_builder& set_dmrs_parameters(uint16_t nid_pdcch_dmrs)
  {
    pdu.nid_pdcch_dmrs = nid_pdcch_dmrs;

    return *this;
  }

  /// Sets the PDCCH context parameters.
  dl_dci_pdu_builder
  set_context_parameters(search_space_id ss_id, const char* dci_format, std::optional<unsigned> harq_feedback_timing)
  {
    pdu.context = pdcch_context(ss_id, dci_format, harq_feedback_timing);
    return *this;
  }

  /// Returns a transmission precoding and beamforming PDU builder of this DL DCI PDU.
  tx_precoding_and_beamforming_pdu_builder get_tx_precoding_and_beamforming_pdu_builder()
  {
    tx_precoding_and_beamforming_pdu_builder builder(pdu.precoding_and_beamforming);

    return builder;
  }

private:
  dl_dci_pdu& pdu;
};

/// Helper class to fill in the DL PDCCH PDU parameters specified in SCF-222 v4.0 section 3.4.2.1.
class dl_pdcch_pdu_builder
{
public:
  explicit dl_pdcch_pdu_builder(dl_pdcch_pdu& pdu_) : pdu(pdu_) {}

  /// \brief Sets the BWP parameters for the fields of the PDCCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.1, in table PDCCH PDU.
  dl_pdcch_pdu_builder& set_bwp_parameters(crb_interval coreset_bwp, subcarrier_spacing scs, cyclic_prefix cp)
  {
    pdu.coreset_bwp = coreset_bwp;
    pdu.scs         = scs;
    pdu.cp          = cp;

    return *this;
  }

  /// \brief Sets the coreset parameters for the fields of the PDCCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.1, in table PDCCH PDU.
  dl_pdcch_pdu_builder& set_coreset_parameters(ofdm_symbol_range                                symbols,
                                               coreset_configuration::precoder_granularity_type precoder_granularity)
  {
    pdu.symbols              = symbols;
    pdu.precoder_granularity = precoder_granularity;

    return *this;
  }

  /// \brief Sets the CORESET 0 parameters for the fields of the PDCCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.1, in table PDCCH PDU.
  dl_pdcch_pdu_builder& set_coreset_0_parameters(coreset_configuration::interleaved_mapping_type interleaved,
                                                 freq_resource_bitmap                            freq_domain_resource)
  {
    pdu.freq_domain_resource = freq_domain_resource;
    auto& mapping            = pdu.mapping.emplace<dl_pdcch_pdu::mapping_coreset_0>();
    mapping.interleaved      = interleaved;

    return *this;
  }

  /// \brief Sets the Interleaver parameters for the fields of the PDCCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.1, in table PDCCH PDU.
  dl_pdcch_pdu_builder& set_interleaver_parameters(coreset_configuration::interleaved_mapping_type interleaved,
                                                   freq_resource_bitmap                            freq_domain_resource)
  {
    pdu.freq_domain_resource = freq_domain_resource;
    auto& mapping            = pdu.mapping.emplace<dl_pdcch_pdu::mapping_interleaved>();
    mapping.interleaved      = interleaved;

    return *this;
  }

  /// \brief Sets the Non interleaver parameters for the fields of the PDCCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.1, in table PDCCH PDU.
  dl_pdcch_pdu_builder& set_non_interleaver_parameters(freq_resource_bitmap freq_domain_resource)
  {
    pdu.freq_domain_resource = freq_domain_resource;
    // This value is defined in TS 38.213 section 3.4.2.1.
    static constexpr auto NON_INTERLEAVED_REG_BUNDLE_SIZE = 6U;
    auto&                 mapping = pdu.mapping.emplace<dl_pdcch_pdu::mapping_non_interleaved>();
    mapping.reg_bundle_sz         = NON_INTERLEAVED_REG_BUNDLE_SIZE;

    return *this;
  }

  /// \brief Gets the DL DCI PDU builder of the PDCCH PDU.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.2.1, in table PDCCH PDU.
  dl_dci_pdu_builder get_dl_dci_pdu_builder()
  {
    // Set the DL DCI index.
    dl_dci_pdu_builder builder(pdu.dl_dci);

    return builder;
  }

private:
  dl_pdcch_pdu& pdu;
};

} // namespace fapi
} // namespace ocudu
