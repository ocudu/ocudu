// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief PUSCH channel estimator interface declaration.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/phy/upper/channel_state_information.h"
#include "ocudu/phy/upper/dmrs_mapping.h"
#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/slot_point.h"
#include <variant>

namespace ocudu {

class resource_grid_reader;

class dmrs_pusch_estimator_results;

/// Notifier to communicate the completion of the estimation process.
class dmrs_pusch_estimator_notifier
{
public:
  /// Default destructor.
  virtual ~dmrs_pusch_estimator_notifier() = default;

  /// \brief Communicates the notified object about the completion of the estimation process.
  /// \param[in] est_results  Provides access to the estimation results.
  virtual void on_estimation_complete(const dmrs_pusch_estimator_results& est_results) = 0;
};

/// DM-RS-based PUSCH channel estimator interface.
class dmrs_pusch_estimator
{
public:
  /// Parameters for pseudo-random sequence.
  struct pseudo_random_sequence_configuration {
    /// DL DM-RS configuration type.
    dmrs_config_type type;
    /// Number of transmit layers.
    unsigned nof_tx_layers;
    /// PUSCH DM-RS scrambling ID.
    unsigned scrambling_id;
    /// DM-RS sequence initialization (parameter \f$n_{SCID}\f$ in the TS).
    bool n_scid;
  };

  /// Parameters for pseudo-random sequence.
  struct low_papr_sequence_configuration {
    /// Reference signal sequence identifier {0, ..., 1007}.
    unsigned n_rs_id;
  };

  /// Parameters required to receive the demodulation reference signals described in 3GPP TS38.211 Section 6.4.1.1.
  struct configuration {
    /// Slot context for sequence initialization.
    slot_point slot;
    /// Sequence generator configuration.
    std::variant<pseudo_random_sequence_configuration, low_papr_sequence_configuration> sequence_config;
    /// \brif DM-RS amplitude scaling factor.
    ///
    /// Parameter \f$\beta _{\textup{PUSCH}}^{\textup{DMRS}}\f$ as per TS38.211 Section 6.4.1.1.3. It must be set
    /// to \f$\beta _{\textup{PUSCH}}^{\textup{DMRS}}=10^{-\beta_{\textup{DMRS}}/20}\f$, as per TS38.214
    /// Section 6.2.2, where \f$\beta_{\textup{DMRS}}\f$ is the PUSCH EPRE to DM-RS EPRE ratio expressed in decibels, as
    /// specified in TS38.214 Table 6.2.2-1.
    ///
    /// \sa get_sch_to_dmrs_ratio_dB()
    float scaling;
    /// Cyclic prefix.
    cyclic_prefix c_prefix = cyclic_prefix::NORMAL;
    /// DM-RS position mask. Indicates the OFDM symbols carrying DM-RS within the slot.
    bounded_bitset<MAX_NSYMB_PER_SLOT> symbols_mask;
    /// Allocation CRB list: the entries set to true are used for transmission.
    crb_bitmap rb_mask;
    /// First OFDM symbol within the slot for which the channel should be estimated.
    unsigned first_symbol = 0;
    /// Number of OFDM symbols for which the channel should be estimated.
    unsigned nof_symbols = 0;
    /// List of receive ports.
    static_vector<uint8_t, DMRS_MAX_NPORTS> rx_ports;

    /// \brief Gets the number of transmit layers.
    ///
    /// The number of transmit layers when low-PAPR sequences are used is always one. Otherwise, it is specified in the
    /// sequence configuration.
    unsigned get_nof_tx_layers() const
    {
      if (std::holds_alternative<pseudo_random_sequence_configuration>(sequence_config)) {
        return std::get<pseudo_random_sequence_configuration>(sequence_config).nof_tx_layers;
      }

      return 1;
    }

    /// \brief  Gets the DM-RS type.
    ///
    /// The DM-RS type is always 1 when low-PAPR sequences are used. Otherwise, it is specified in the sequence
    /// configuration.
    dmrs_config_type get_dmrs_type() const
    {
      if (std::holds_alternative<pseudo_random_sequence_configuration>(sequence_config)) {
        return std::get<pseudo_random_sequence_configuration>(sequence_config).type;
      }

      return dmrs_config_type::type1;
    }
  };

  /// Default destructor.
  virtual ~dmrs_pusch_estimator() = default;

  /// \brief Estimates the PUSCH propagation channel.
  /// \param[in]  notifier Notifier to communicate the end of the estimation process.
  /// \param[in]  grid     Received resource grid.
  /// \param[in]  config   DM-RS configuration parameters. They characterize the DM-RS symbols and their indices.
  virtual void
  estimate(dmrs_pusch_estimator_notifier& notifier, const resource_grid_reader& grid, const configuration& config) = 0;
};

/// Interface for accessing the results of running the DM-RS PUSCH estimator.
class dmrs_pusch_estimator_results
{
public:
  /// Default destructor.
  virtual ~dmrs_pusch_estimator_results() = default;

  /// Returns the estimated noise variance for the given Rx port (linear scale).
  virtual float get_noise_variance(unsigned rx_port) const = 0;

  /// Returns the estimated noise variance for the given Rx port (dB scale).
  float get_noise_variance_dB(unsigned rx_port) const { return convert_power_to_dB(get_noise_variance(rx_port)); }

  /// Returns the estimated RSRP for the path between the given Rx port and Tx layer (linear scale).
  virtual float get_rsrp(unsigned rx_port, unsigned tx_layer = 0) const = 0;

  /// Gets the estimated RSRP for all Rx ports for the given Tx layer (linear scale).
  virtual static_vector<float, MAX_PORTS> get_rsrp_all_ports(unsigned tx_layer = 0) const = 0;

  /// Returns the estimated RSRP for the path between the given Rx port and Tx layer (dB scale).
  float get_rsrp_dB(unsigned rx_port, unsigned tx_layer = 0) const
  {
    return convert_power_to_dB(get_rsrp(rx_port, tx_layer));
  }

  /// \brief Returns the average EPRE for the given Rx port (linear scale).
  ///
  /// \remark The EPRE is defined as the average received power (including noise) across all REs carrying DM-RS for all
  /// layers.
  virtual float get_epre(unsigned rx_port) const = 0;

  /// \brief Returns the average EPRE for the given Rx port (dB scale).
  ///
  /// \remark The EPRE is defined as the average received power (including noise) across all REs carrying DM-RS.
  float get_epre_dB(unsigned rx_port) const { return convert_power_to_dB(get_epre(rx_port)); }

  /// Returns the estimated SNR for the given Rx port (linear scale).
  virtual float get_snr(unsigned rx_port) const = 0;

  /// Returns the estimated average SNR for a given layer (linear scale).
  virtual float get_layer_average_snr(unsigned tx_layer = 0) const = 0;

  /// Returns the estimated SNR for the given Rx port (dB scale).
  float get_snr_dB(unsigned rx_port) const { return convert_power_to_dB(get_snr(rx_port)); }

  /// Returns the estimated time alignment in PHY time units between the given Rx port.
  virtual phy_time_unit get_time_alignment(unsigned rx_port) const = 0;

  /// \brief Returns the carrier frequency offset in hertz estimated for the given Rx port.
  ///
  /// \remark The CFO estimation may be emtpy depending on the DM-RS configuration.
  virtual std::optional<float> get_cfo_Hz(unsigned rx_port) const = 0;

  /// \brief Gets the RE channel estimates for a given OFDM symbol, Rx port and Tx layer.
  ///
  /// The values are indexed by subcarrier.
  virtual void
  get_symbol_ch_estimate(span<cbf16_t> estimates, unsigned i_symbol, unsigned rx_port, unsigned tx_layer) const = 0;

  /// \brief Gets the RE channel estimates for a given OFDM symbol, Rx port and Tx layer, only picking the REs flagged
  /// in the mask.
  ///
  /// The values are indexed by subcarrier.
  virtual void get_symbol_ch_estimate(span<cbf16_t>                              estimates,
                                      unsigned                                   i_symbol,
                                      unsigned                                   rx_port,
                                      unsigned                                   tx_layer,
                                      const bounded_bitset<MAX_NOF_SUBCARRIERS>& re_mask) const = 0;

  /// \brief Gets the general Channel State Information.
  ///
  /// \param[out] csi Channel State Information object where the CSI parameters are stored.
  virtual void get_channel_state_information(channel_state_information& csi) const = 0;
};

} // namespace ocudu
