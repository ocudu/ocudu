// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief PUSCH demodulator interface.

#pragma once

#include "ocudu/phy/upper/signal_processors/pusch/dmrs_pusch_estimator.h"
#include "ocudu/ran/dmrs/dmrs.h"
#include "ocudu/ran/sch/modulation_scheme.h"

namespace ocudu {

class pusch_codeword_buffer;
class pusch_demodulator_notifier;
class resource_grid_reader;

/// \brief PUSCH demodulator interface.
///
/// The demodulation of a PUSCH consists of:
/// - extracting allocated REs from the resource grid,
/// - equalizing of the extracted RE,
/// - soft-demodulation of the complex data, and
/// - descrambling.
class pusch_demodulator
{
public:
  /// Parameters defining the demodulation procedure of a PUSCH transmission.
  struct configuration {
    /// Radio Network Temporary Identifier, see parameter \f$n_{RNTI}\f$ in TS38.211 Section 6.3.1.1.
    uint16_t rnti;
    /// Allocation CRB list: the entries set to true are used for transmission.
    crb_bitmap rb_mask;
    /// Modulation scheme used for transmission.
    modulation_scheme modulation;
    /// Time domain allocation within a slot: start symbol index {0, ..., 12}.
    unsigned start_symbol_index;
    /// Time domain allocation within a slot: number of symbols {1, ..., 14}.
    unsigned nof_symbols;
    /// OFDM symbols containing DM-RS: boolean mask.
    symbol_slot_mask dmrs_symb_pos;
    /// DM-RS configuration type.
    dmrs_config_type dmrs_type;
    /// Number of DM-RS CDM groups without data.
    unsigned nof_cdm_groups_without_data;
    /// Scrambling identifier, see parameter \f$n_{ID}\f$ in TS38.211 Section 6.3.1.1. Range is {0, ..., 1023}.
    unsigned n_id;
    /// Number of transmit layers.
    unsigned nof_tx_layers;
    /// \brief Direct current position.
    ///
    /// It may be empty if it falls outside the bandwidth or if not configured.
    std::optional<unsigned> dc_position;
    /// Set to true for enabling transform precoding.
    bool enable_transform_precoding;
    /// Receive antenna port indices the PUSCH transmission is mapped to.
    static_vector<uint8_t, MAX_PORTS> rx_ports;
    /// \brief Random Access Preamble Index (RAPID) associated with msgA on PUSCH (Release 16). Possible values
    /// are {0, ..., 63}.
    ///
    /// During 2-step RACH, the PUSCH carrying msgA is alternatively scrambled using an initialization value that
    /// depends on the sequence index of the detected RACH preamble.
    ///
    /// The presence of this parameter indicates the PUSCH is a msgA and the scrambling sequence is initialized as:
    /// \f$c_{init} = n_{RNTI} \cdot 2^{16} + n_{RAPID} \cdot 2^{10} + n_{ID}\f$
    /// as specified in TS 38.211, Section 6.3.1.1.
    std::optional<unsigned> n_rapid;
  };

  /// Default destructor.
  virtual ~pusch_demodulator() = default;

  /// \brief Demodulates a PUSCH transmission.
  ///
  /// Computes log-likelihood ratios from channel samples by reversing all the operations described in TS38.211 Section
  /// 6.3.1.
  ///
  /// \remarks
  /// - The size of \c data determines the codeword size in soft bits.
  /// - The total number of LLR must be consistent with the number of RE allocated to the transmission.
  ///
  /// \param[out] codeword_buffer Codeword buffer.
  /// \param[in]  notifier        Demodulation statistics notifier.
  /// \param[in]  grid            Resource grid for the current slot.
  /// \param[in]  est_results     Interface to access the channel estimates for the REs allocated to the PUSCH
  ///                             transmission.
  /// \param[in]  config          Configuration parameters.
  virtual void demodulate(pusch_codeword_buffer&              codeword_buffer,
                          pusch_demodulator_notifier&         notifier,
                          const resource_grid_reader&         grid,
                          const dmrs_pusch_estimator_results& est_results,
                          const configuration&                config) = 0;
};

} // namespace ocudu
