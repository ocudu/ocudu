// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief PUSCH channel estimator definition.

#pragma once

#include "ocudu/phy/upper/sequence_generators/low_papr_sequence_generator.h"
#include "ocudu/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "ocudu/phy/upper/signal_processors/channel_estimator/port_channel_estimator.h"
#include "ocudu/phy/upper/signal_processors/pusch/dmrs_pusch_estimator.h"
#include "ocudu/support/executors/task_executor.h"

namespace ocudu {

class dmrs_pusch_estimator_impl : public dmrs_pusch_estimator, private dmrs_pusch_estimator_results
{
public:
  using layer_dmrs_pattern = port_channel_estimator::layer_dmrs_pattern;

  /// Constructor - configures all dependencies.
  explicit dmrs_pusch_estimator_impl(std::unique_ptr<pseudo_random_generator>             prg_,
                                     std::unique_ptr<low_papr_sequence_generator>         tp_sequence_generator_,
                                     std::vector<std::unique_ptr<port_channel_estimator>> ch_est,
                                     task_executor&                                       executor_) :
    prg(std::move(prg_)),
    low_papr_sequence_gen(std::move(tp_sequence_generator_)),
    ch_estimator(std::move(ch_est)),
    ch_est_result(ch_estimator.size(), nullptr),
    executor(executor_)
  {
    ocudu_assert(prg, "Invalid PRG.");
    ocudu_assert(low_papr_sequence_gen, "Invalid sequence generator.");
    for (const auto& ch : ch_estimator) {
      ocudu_assert(ch, "Invalid port channel estimator.");
    }
  }

  // See the dmrs_pusch_estimator interface for the documentation.
  void estimate(dmrs_pusch_estimator_notifier& notifier,
                const resource_grid_reader&    grid,
                const configuration&           config) override;

private:
  /// Maximum supported number of transmission layers.
  static constexpr unsigned MAX_TX_LAYERS = pusch_constants::MAX_NOF_LAYERS;
  /// DM-RS for PUSCH reference point \f$k\f$ relative to Point A.
  static constexpr unsigned DMRS_REF_POINT_K_TO_POINT_A = 0;

  /// Pseudorandom generator.
  std::unique_ptr<pseudo_random_generator> prg;
  /// Sequence generator for transform precoding.
  std::unique_ptr<low_papr_sequence_generator> low_papr_sequence_gen;
  /// Antenna port channel estimators.
  std::vector<std::unique_ptr<port_channel_estimator>> ch_estimator;
  /// Antenna port channel estimator results.
  std::vector<const port_channel_estimator_results*> ch_est_result;
  /// Buffer for DM-RS symbols.
  dmrs_symbol_list temp_symbols;
  /// Buffer for DM-RS symbol coordinates.
  std::array<layer_dmrs_pattern, MAX_TX_LAYERS> temp_pattern;
  /// Configuration of the port channel estimator.
  port_channel_estimator::configuration est_cfg;
  /// Counter of ports still pending to be estimated.
  std::atomic<unsigned> pending_ports;
  /// Task executor for running the port channel estimator.
  task_executor& executor;
  /// Number of REs (subcarriers) in the most recently processed transmission.
  unsigned nof_re = 0;
  /// OFDM symbols in the most recently processed transmission.
  interval<unsigned> ofdm_symbols;
  /// Number of layers in the most recently processed transmission.
  unsigned nof_tx_layers = 0;

  // See the dmrs_pusch_estimator_results interface for the documentation.
  float get_noise_variance(unsigned rx_port) const override;

  // See the dmrs_pusch_estimator_results interface for the documentation.
  float get_rsrp(unsigned rx_port, unsigned tx_layer = 0) const override;

  // See the dmrs_pusch_estimator_results interface for the documentation.
  static_vector<float, MAX_PORTS> get_rsrp_all_ports(unsigned tx_layer = 0) const override;

  // See the dmrs_pusch_estimator_results interface for the documentation.
  float get_epre(unsigned rx_port) const override;

  // See the dmrs_pusch_estimator_results interface for the documentation.
  float get_snr(unsigned rx_port) const override;

  // See the dmrs_pusch_estimator_results interface for the documentation.
  float get_layer_average_snr(unsigned tx_layer = 0) const override;

  // See the dmrs_pusch_estimator_results interface for the documentation.
  phy_time_unit get_time_alignment(unsigned rx_port) const override;

  // See the dmrs_pusch_estimator_results interface for the documentation.
  std::optional<float> get_cfo_Hz(unsigned rx_port) const override;

  // See the dmrs_pusch_estimator_results interface for the documentation.
  void get_symbol_ch_estimate(span<cbf16_t> estimates,
                              unsigned      i_symbol,
                              unsigned      rx_port,
                              unsigned      tx_layer) const override;

  // See the dmrs_pusch_estimator_results interface for the documentation.
  void get_symbol_ch_estimate(span<cbf16_t>                              estimates,
                              unsigned                                   i_symbol,
                              unsigned                                   rx_port,
                              unsigned                                   tx_layer,
                              const bounded_bitset<MAX_NOF_SUBCARRIERS>& re_mask) const override;

  // See the dmrs_pusch_estimator_results interface for the documentation.
  void get_channel_state_information(channel_state_information& csi) const override;

  /// \brief Generates the sequence described in TS38.211 Section 6.4.1.1.1, considering the only values required
  /// in TS38.211 Section 6.4.1.1.2.
  ///
  /// \param[out] sequence Sequence destination.
  /// \param[in] symbol    Symbol index within the slot.
  /// \param[in] config    Configuration parameters.
  void sequence_generation(span<cf_t> sequence, unsigned symbol, const configuration& config) const;

  /// \brief Generates the PUSCH DM-RS symbols for one transmission port.
  ///
  /// Implements the PUSCH DM-RS generation, precoding and mapping procedures described in TS38.211 Section 6.4.1.1.
  /// \param[out] symbols Lists of generated DM-RS symbols, one per transmission layer.
  /// \param[out] pattern Lists of DM-RS patterns representing the REs the DM-RS symbols should be mapped to, one per
  ///                     transmission layer.
  /// \param[in]  cfg     Configuration parameters.
  void generate(dmrs_symbol_list& symbols, span<layer_dmrs_pattern> mask, const configuration& cfg);
};

} // namespace ocudu
