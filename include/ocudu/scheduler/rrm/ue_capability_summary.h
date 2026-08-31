// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/meas_gap_config.h"
#include "ocudu/ran/nr_band.h"
#include "ocudu/ran/pusch/tx_scheme_configuration.h"
#include <cstdint>
#include <optional>
#include <unordered_map>

namespace ocudu {

/// Flat structure summarizing the decoded ASN.1 UE capabilities.
struct ue_capability_summary {
  /// \defgroup default_caps Default parameters.
  /// @{
  /// Default PUSCH transmit coherence.
  static constexpr tx_scheme_codebook_subset default_pusch_tx_coherence = tx_scheme_codebook_subset::non_coherent;
  /// Default PUSCH maximum number of layers.
  static constexpr unsigned default_pusch_max_rank = 1;
  /// Default SRS number of transmit ports.
  static constexpr unsigned default_nof_srs_tx_ports = 1;
  /// Default max number of DL/UL HARQ processes.
  static constexpr unsigned default_max_harq_process_num = 16;
  /// Default maximum PDSCH TDRA repetition number.
  static constexpr unsigned default_max_pdsch_tdra_rep_number = 1;
  /// @}

  /// \brief Type-II codebook parameters supported by the UE.
  ///
  /// It is given by field \e type2 of \e codebookParameters in Information Element \e MIMO-ParametersPerBand.
  struct type2_codebook_params {
    // This user provided constructor is added here to fix a Clang compilation error related to the use of nested types
    // with std::optional.
    type2_codebook_params() {}

    /// Maximum number of beams supported for linear combination, given by field \e parameterLx. Values: {2, 3, 4}.
    uint8_t max_nof_beams = 2;
    /// Set to true if the UE supports subband amplitude scaling, given by field \e amplitudeScalingType.
    bool subband_amplitude_supported = false;
    /// Maximum number of TX ports in a CSI-RS resource, given by field \e supportedCSI-RS-ResourceList.
    uint8_t max_nof_tx_ports_per_resource = 2;

    /// Equality operator.
    bool operator==(const type2_codebook_params& other) const
    {
      return (max_nof_beams == other.max_nof_beams) &&
             (subband_amplitude_supported == other.subband_amplitude_supported) &&
             (max_nof_tx_ports_per_resource == other.max_nof_tx_ports_per_resource);
    }
  };

  /// Contains band specific parameters.
  struct supported_band {
    /// Set to true if QAM-256 is supported for PUSCH transmissions.
    bool pusch_qam256_supported = false;
    /// \brief PUSCH transmit coherence.
    ///
    /// It is given by field \e pusch-TransCoherence in Information Element \e MIMO-ParametersPerBand.
    ///
    /// The most limiting transmit codebook subset is selected by default.
    tx_scheme_codebook_subset pusch_tx_coherence = default_pusch_tx_coherence;
    /// Maximum PUSCH number of layers.
    unsigned pusch_max_rank = default_pusch_max_rank;
    /// Maximum number of ports that can be simultaneously used for transmiting Sounding Reference Signals.
    uint8_t nof_srs_tx_ports = default_nof_srs_tx_ports;
    /// Maximum number of DL HARQ processes.
    uint8_t max_dl_harq_process_num = default_max_harq_process_num;
    /// Maximum number of UL HARQ processes.
    uint8_t max_ul_harq_process_num = default_max_harq_process_num;
    /// Indicates whether the UE supports the uplink time and frequency pre-compensation.
    bool ul_pre_compensation_supported = false;
    /// Indicates whether the UE supports UE reporting of information related to TA pre-compensation.
    bool ul_ta_reporting_supported = false;
    /// Indicates whether the UE supports the reception of UE-specific K_offset.
    bool ue_specific_k_offset_supported = false;
    /// \brief Maximum repetition number supported in PDSCH TDRA.
    ///
    /// It is given by field \e supportRepNumPDSCH-TDRA-r16 in Information Element \e MIMO-ParametersPerBand. Value 1
    /// means that the UE does not support dynamic PDSCH repetitions.
    uint8_t max_pdsch_tdra_rep_number = default_max_pdsch_tdra_rep_number;
    /// \brief Indicates whether the UE supports counting PUSCH repetition Type A occasions on available slots.
    ///
    /// It is given by field \e puschTypeA-RepetitionsAvailSlot-r17 in Information Element \e BandNR.
    bool pusch_rep_type_a_avail_slot_supported = false;
    /// \brief Indicates whether the UE supports transmission of a PUCCH format 0 and 2 over multiple slots.
    ///
    /// It is given by field \e pucch-Repetition-F0-2-r17 in Information Element \e BandNR.
    bool pucch_repeat_f0_2_r17_supported = false;
    /// Type-II codebook parameters. It is empty if the UE does not support the Type-II codebook for CSI reporting.
    std::optional<type2_codebook_params> type2_codebook;

    /// Equality operator.
    bool operator==(const supported_band& other) const
    {
      return (pusch_qam256_supported == other.pusch_qam256_supported) &&
             (pusch_tx_coherence == other.pusch_tx_coherence) && (pusch_max_rank == other.pusch_max_rank) &&
             (nof_srs_tx_ports == other.nof_srs_tx_ports) &&
             (max_dl_harq_process_num == other.max_dl_harq_process_num) &&
             (max_ul_harq_process_num == other.max_ul_harq_process_num) &&
             (ul_pre_compensation_supported == other.ul_pre_compensation_supported) &&
             (ul_ta_reporting_supported == other.ul_ta_reporting_supported) &&
             (ue_specific_k_offset_supported == other.ue_specific_k_offset_supported) &&
             (max_pdsch_tdra_rep_number == other.max_pdsch_tdra_rep_number) &&
             (pusch_rep_type_a_avail_slot_supported == other.pusch_rep_type_a_avail_slot_supported) &&
             (pucch_repeat_f0_2_r17_supported == other.pucch_repeat_f0_2_r17_supported) &&
             (type2_codebook == other.type2_codebook);
    }
  };

  /// Set to true if QAM-256 MCS table are supported for PDSCH transmissions.
  bool pdsch_qam256_supported = false;
  /// Set to true if QAM-64 LowSe MCS table are supported for PDSCH transmissions.
  bool pdsch_qam64lowse_supported = false;
  /// Set to true if QAM-64 LowSe MCS table are supported for PUSCH transmissions.
  bool pusch_qam64lowse_supported = false;
  /// \brief Set to true if dynamically scheduled PUSCH repetition Type A is supported.
  ///
  /// It is given by field \e non-sharedSpectrumChAccess-r16 of \e pusch-RepetitionTypeA-r16 in Information Element
  /// \e Phy-ParametersCommon.
  bool pusch_rep_type_a_supported = false;
  /// Contains specific bands capabilities.
  std::unordered_map<nr_band, supported_band> bands;
  /// Set to true if Long DRX cycle is supported.
  bool long_drx_cycle_supported = false;
  /// Set to true if Short DRX cycle is supported.
  bool short_drx_cycle_supported = false;
  /// Set to true if UE supports \c interleavingVRB-ToPRB-PDSCH, as per TS 38.306, Section 4.2.7.10.
  bool pdsch_interleaving_vrb_to_prb_supported = false;
  /// Set to true if UE supports NTN features.
  bool ntn_supported = false;
  /// Indicates whether the UE supports disabled HARQ feedback for downlink transmission.
  bool disabled_dl_harq_feedback_supported = false;
  /// Indicates whether the UE supports HARQ Mode B and the corresponding LCP restrictions for uplink transmission.
  bool ul_harq_mode_b_supported = false;
  /// \brief Indicates whether the UE can raise a Scheduling Request when a Timing Advance Report is triggered and
  /// no UL-SCH resource is available. Given by \e sr-TriggeredBy-TA-Report-r17 in \e MAC-Parameters.
  bool sr_triggered_by_ta_report_supported = false;
  /// Measurement gap patterns supported by the UE, defaults to patterns 0 and 1 supported.
  supported_meas_gap_patterns supported_meas_gaps;
  /// \brief Indicates whether the UE supports transmission of a PUCCH format 1 or 3 or 4 over multiple slots.
  ///
  /// It is given by field \e pucch-Repetition-F1-3-4 in Information Element \e Phy-ParametersCommon.
  bool pucch_repeat_f1_3_4_supported = false;
  /// \brief Indicates whether the UE supports slot-based dynamic PUCCH repetition.
  ///
  /// It is given by field \e slotBasedDynamicPUCCH-Rep-r17 in Information Element \e Phy-ParametersCommon.
  bool slot_based_dyn_pucch_rep_r17_supported = false;

  /// Equality operator.
  bool operator==(const ue_capability_summary& other) const
  {
    return (pdsch_qam256_supported == other.pdsch_qam256_supported) &&
           (pdsch_qam64lowse_supported == other.pdsch_qam64lowse_supported) &&
           (pusch_qam64lowse_supported == other.pusch_qam64lowse_supported) &&
           (pusch_rep_type_a_supported == other.pusch_rep_type_a_supported) && (bands == other.bands) &&
           (long_drx_cycle_supported == other.long_drx_cycle_supported) &&
           (short_drx_cycle_supported == other.short_drx_cycle_supported) &&
           (pdsch_interleaving_vrb_to_prb_supported == other.pdsch_interleaving_vrb_to_prb_supported) &&
           (ntn_supported == other.ntn_supported) &&
           (disabled_dl_harq_feedback_supported == other.disabled_dl_harq_feedback_supported) &&
           (ul_harq_mode_b_supported == other.ul_harq_mode_b_supported) &&
           (sr_triggered_by_ta_report_supported == other.sr_triggered_by_ta_report_supported) &&
           (supported_meas_gaps == other.supported_meas_gaps) &&
           (pucch_repeat_f1_3_4_supported == other.pucch_repeat_f1_3_4_supported) &&
           (slot_based_dyn_pucch_rep_r17_supported == other.slot_based_dyn_pucch_rep_r17_supported);
  }
};

} // namespace ocudu
