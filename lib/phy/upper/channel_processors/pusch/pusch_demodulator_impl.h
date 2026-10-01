// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief PUSCH demodulator implementation declaration.

#pragma once

#include "ocudu/phy/generic_functions/transform_precoding/transform_precoder.h"
#include "ocudu/phy/support/re_buffer.h"
#include "ocudu/phy/support/resource_grid_reader.h"
#include "ocudu/phy/upper/channel_modulation/demodulation_mapper.h"
#include "ocudu/phy/upper/channel_modulation/evm_calculator.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_demodulator.h"
#include "ocudu/phy/upper/equalization/channel_equalizer.h"
#include "ocudu/phy/upper/equalization/dynamic_ch_est_list.h"
#include "ocudu/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "ocudu/ran/pusch/pusch_constants.h"

namespace ocudu {

/// PUSCH demodulator implementation.
class pusch_demodulator_impl : public pusch_demodulator
{
public:
  /// Constructor: sets up internal components and acquires their ownership.
  pusch_demodulator_impl(std::unique_ptr<channel_equalizer>       equalizer_,
                         std::unique_ptr<transform_precoder>      precoder_,
                         std::unique_ptr<demodulation_mapper>     demapper_,
                         std::unique_ptr<evm_calculator>          evm_calc_,
                         std::unique_ptr<pseudo_random_generator> descrambler_,
                         unsigned                                 max_nof_rb,
                         bool                                     compute_post_eq_sinr_) :
    equalizer(std::move(equalizer_)),
    precoder(std::move(precoder_)),
    demapper(std::move(demapper_)),
    evm_calc(std::move(evm_calc_)),
    descrambler(std::move(descrambler_)),
    ch_re_copy(MAX_PORTS, max_nof_rb * NOF_SUBCARRIERS_PER_RB),
    temp_eq_re(max_nof_rb * NOF_SUBCARRIERS_PER_RB * pusch_constants::MAX_NOF_LAYERS),
    temp_eq_noise_vars(max_nof_rb * NOF_SUBCARRIERS_PER_RB * pusch_constants::MAX_NOF_LAYERS),
    ch_estimates_copy(max_nof_rb * NOF_SUBCARRIERS_PER_RB,
                      pusch_constants::MAX_NOF_RX_PORTS,
                      pusch_constants::MAX_NOF_LAYERS),
    compute_post_eq_sinr(compute_post_eq_sinr_)
  {
    ocudu_assert(equalizer, "Invalid pointer to channel_equalizer object.");
    ocudu_assert(demapper, "Invalid pointer to demodulation_mapper object.");
    ocudu_assert(descrambler, "Invalid pointer to pseudo_random_generator object.");
  }
  // See interface for the documentation.
  void demodulate(pusch_codeword_buffer&              codeword_buffer,
                  pusch_demodulator_notifier&         notifier,
                  const resource_grid_reader&         grid,
                  const dmrs_pusch_estimator_results& est_results,
                  const configuration&                config) override;

private:
  /// Data type for representing an RE mask within an OFDM symbol.
  using re_symbol_mask_type = bounded_bitset<MAX_NOF_SUBCARRIERS>;

  /// \brief Gets channel data Resource Elements from the resource grid.
  ///
  /// Extracts the PUSCH data RE's from the provided resource grid. The DM-RS symbols are skipped. The extracted RE's
  /// are arranged in two dimensions, i.e., resource element and receive antenna port, as the channel equalizer expects.
  ///
  /// \param[in]  grid      Resource grid for the current slot.
  /// \param[in]  i_symbol  OFDM symbol index relative to the beginning of the slot.
  /// \param[in]  re_mask   Resource element mask, it selects the RE elements to extract.
  /// \param[in]  rx_ports  Receive ports.
  /// \return A reference to the PUSCH channel data symbols.
  const re_buffer_reader<cbf16_t>& get_ch_data_re(const resource_grid_reader&              grid,
                                                  unsigned                                 i_symbol,
                                                  const re_symbol_mask_type&               re_mask,
                                                  const static_vector<uint8_t, MAX_PORTS>& rx_ports);

  /// \brief Gets channel data estimates.
  ///
  /// Extracts the channel estimation coefficients corresponding to the PUSCH data Resource Elements from the channel
  /// estimate. The DM-RS symbols are skipped. The extracted channel coefficients are arranged in three dimensions,
  /// i.e., resource element, receive port and transmit layer, as the channel equalizer expects.
  ///
  /// \param[in]  est_results   Interface to access the channel estimates for the REs allocated to the PUSCH
  ///                           transmission.
  /// \param[in]  i_symbol      OFDM symbol index relative to the beginning of the slot.
  /// \param[in]  init_subc     Initial subcarrier index relative to Point A.
  /// \param[in]  nof_tx_layers Number of layers.
  /// \param[in]  re_mask       Resource element mask, it selects the RE elements to extract (only relative to the
  ///                           allocated RBs).
  /// \param[in]  dc_position   Direct current position, relative to the first allocated RE: emtpy if not configured or
  ///                           with transform precoding enabled.
  /// \param[in]  rx_ports      Receive ports list.
  /// \return A reference to the PUSCH channel data estimates.
  const channel_equalizer::ch_est_list& get_ch_data_estimates(const dmrs_pusch_estimator_results&      est_results,
                                                              unsigned                                 i_symbol,
                                                              unsigned                                 nof_tx_layers,
                                                              const re_symbol_mask_type&               re_mask,
                                                              std::optional<unsigned>                  dc_position,
                                                              const static_vector<uint8_t, MAX_PORTS>& rx_ports);

  /// Channel equalization component, also in charge of combining contributions of all receive antenna ports.
  std::unique_ptr<channel_equalizer> equalizer;
  /// Transform precoder.
  std::unique_ptr<transform_precoder> precoder;
  /// Demodulation mapper component: transforms channel symbols into log-likelihood ratios (i.e., soft bits).
  std::unique_ptr<demodulation_mapper> demapper;
  /// EVM calculator. Optional, set to nullptr if not available.
  std::unique_ptr<evm_calculator> evm_calc;
  /// Descrambler component.
  std::unique_ptr<pseudo_random_generator> descrambler;
  /// Copy buffer used to transfer channel modulation symbols from the resource grid to the equalizer.
  dynamic_re_buffer<cbf16_t> ch_re_copy;
  /// View buffer used to transfer channel modulation symbols from the resource grid to the equalizer.
  modular_re_buffer_reader<cbf16_t, MAX_PORTS> ch_re_view;
  /// Buffer used to store channel modulation resource elements at the equalizer output.
  std::vector<cf_t> temp_eq_re;
  /// Buffer used to transfer symbol noise variances at the equalizer output.
  std::vector<float> temp_eq_noise_vars;
  /// Copy buffer used to transfer channel estimation coefficients from the channel estimate to the equalizer.
  dynamic_ch_est_list ch_estimates_copy;
  /// Buffer used to transfer noise variance estimates from the channel estimate to the equalizer.
  std::array<float, MAX_PORTS> noise_var_estimates;

  /// Enables post equalization SINR calculation.
  bool compute_post_eq_sinr;
};

} // namespace ocudu
