// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "dmrs_pusch_estimator_impl.h"
#include "../dmrs_helper.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/ocuduvec/sc_prod.h"

using namespace ocudu;

void dmrs_pusch_estimator_impl::estimate(dmrs_pusch_estimator_notifier& notifier,
                                         const resource_grid_reader&    grid,
                                         const configuration&           config)
{
  dmrs_config_type type         = config.get_dmrs_type();
  unsigned         nof_rx_ports = config.rx_ports.size();
  nof_tx_layers                 = config.get_nof_tx_layers();
  nof_re                        = config.rb_mask.size() * NOF_SUBCARRIERS_PER_RB;
  ofdm_symbols.set(config.first_symbol, config.first_symbol + config.nof_symbols);

  ocudu_assert(nof_rx_ports <= ch_estimator.size(),
               "Trying to estimate the channel for {} antenna ports, configured {}.",
               nof_rx_ports,
               ch_estimator.size());

  // Select the DM-RS pattern for this PUSCH transmission.
  span<layer_dmrs_pattern> coordinates = span<layer_dmrs_pattern>(temp_pattern).first(nof_tx_layers);

  // Prepare DM-RS symbol buffer dimensions.
  re_measurement_dimensions dims;
  dims.nof_subc    = config.rb_mask.count() * get_nof_re_per_prb(type);
  dims.nof_symbols = config.symbols_mask.count();
  dims.nof_slices  = nof_tx_layers;

  // Resize DM-RS symbol buffer.
  temp_symbols.resize(dims);

  // Generate symbols and allocation patterns.
  generate(temp_symbols, coordinates, config);

  est_cfg.dmrs_pattern.assign(coordinates.begin(), coordinates.end());
  est_cfg.scs          = to_subcarrier_spacing(config.slot.numerology());
  est_cfg.first_symbol = config.first_symbol;
  est_cfg.nof_symbols  = config.nof_symbols;
  est_cfg.rx_ports     = config.rx_ports;
  est_cfg.scaling      = config.scaling;

  ch_est_result.resize(nof_rx_ports);
  pending_ports = nof_rx_ports;
  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    auto estimator_callback = [this, &grid, i_port, &notifier]() {
      const port_channel_estimator_results& ch_est_results =
          ch_estimator[i_port]->compute(grid, i_port, temp_symbols, est_cfg);
      ch_est_result[i_port] = &ch_est_results;

      if (pending_ports.fetch_sub(1) == 1) {
        notifier.on_estimation_complete(*this);
      }
    };

    bool enqueued = executor.defer(estimator_callback);
    if (!enqueued) {
      estimator_callback();
    }
  }
}

void dmrs_pusch_estimator_impl::sequence_generation(span<cf_t>           sequence,
                                                    unsigned             symbol,
                                                    const configuration& config) const
{
  // Generate low-PAPR sequence.
  if (std::holds_alternative<low_papr_sequence_configuration>(config.sequence_config)) {
    const auto& sequence_config = std::get<low_papr_sequence_configuration>(config.sequence_config);
    unsigned    sequence_group  = sequence_config.n_rs_id % 30;
    low_papr_sequence_gen->generate(sequence, sequence_group, 0, 0, 1);
    return;
  }

  // Generate pseudo-random sequence.
  const auto& sequence_config = std::get<pseudo_random_sequence_configuration>(config.sequence_config);

  // Get signal amplitude.
  float amplitude = M_SQRT1_2;

  // Extract parameters to calculate the PRG initial state.
  unsigned nslot    = config.slot.slot_index();
  unsigned nidnscid = sequence_config.scrambling_id;
  unsigned nscid    = sequence_config.n_scid ? 1 : 0;
  unsigned nsymb    = get_nsymb_per_slot(cyclic_prefix::NORMAL);

  // Calculate initial sequence state.
  unsigned c_init = ((nsymb * nslot + symbol + 1) * (2 * nidnscid + 1) * pow2(17) + (2 * nidnscid + nscid)) % pow2(31);

  // Initialize sequence.
  prg->init(c_init);

  // Generate sequence.
  dmrs_sequence_generate(
      sequence, *prg, amplitude, DMRS_REF_POINT_K_TO_POINT_A, get_nof_re_per_prb(sequence_config.type), config.rb_mask);
}

void dmrs_pusch_estimator_impl::generate(dmrs_symbol_list&        symbols,
                                         span<layer_dmrs_pattern> mask,
                                         const configuration&     cfg)
{
  dmrs_config_type type = cfg.get_dmrs_type();

  // For each symbol in the transmission generate DMRS for layer 0.
  for (unsigned ofdm_symbol_index = cfg.first_symbol,
                ofdm_symbol_end   = cfg.first_symbol + cfg.nof_symbols,
                dmrs_symbol_index = 0;
       ofdm_symbol_index != ofdm_symbol_end;
       ++ofdm_symbol_index) {
    // Skip symbol if it does not carry DM-RS.
    if (!cfg.symbols_mask.test(ofdm_symbol_index)) {
      continue;
    }

    // Select a view to the DM-RS symbols for this OFDM symbol and layer 0.
    span<cf_t> dmrs_symbols = symbols.get_symbol(dmrs_symbol_index, 0);

    // Generate DM-RS for PUSCH.
    sequence_generation(dmrs_symbols, ofdm_symbol_index, cfg);

    // Increment DM-RS OFDM symbol index.
    ++dmrs_symbol_index;
  }

  // For each layer...
  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    // Select layer parameters.
    dmrs_pxsch_parameters params = get_pxsch_dmrs_params(type, i_layer);

    // Skip copy for layer 0.
    if (i_layer != 0) {
      // For each symbol containing DM-RS...
      for (unsigned i_symbol = 0, i_symbol_end = symbols.size().nof_symbols; i_symbol != i_symbol_end; ++i_symbol) {
        // Get a view of the symbols for the current layer.
        span<cf_t> dmrs = symbols.get_symbol(i_symbol, i_layer);

        // Get a view of the symbols for layer 0.
        span<const cf_t> dmrs_layer0 = symbols.get_symbol(i_symbol, 0);

        // If a time weight is required...
        if ((params.w_t[0] != params.w_t[1]) && (i_symbol % 2 == 1)) {
          // apply the weight...
          ocuduvec::sc_prod(dmrs, dmrs_layer0, params.w_t[1]);
        } else {
          // otherwise, copy symbols from layer 0 to the current layer.
          ocuduvec::copy(dmrs, dmrs_layer0);
        }

        // If a frequency weight is required...
        if (params.w_f[0] != params.w_f[1]) {
          // apply frequency domain mask.
          for (unsigned subc = 1, subc_end = 2 * (dmrs.size() / 2) + 1; subc != subc_end; subc += 2) {
            dmrs[subc] *= params.w_f[1];
          }
        }
      }
    }

    mask[i_layer].symbols    = cfg.symbols_mask;
    mask[i_layer].rb_mask    = cfg.rb_mask;
    mask[i_layer].re_pattern = params.re_pattern;
  }
}

void dmrs_pusch_estimator_impl::get_symbol_ch_estimate(span<cbf16_t> estimates,
                                                       unsigned      i_symbol,
                                                       unsigned      rx_port,
                                                       unsigned      tx_layer) const
{
  ocudu_assert(ch_est_result[rx_port], "Invalid channel estimator results for port {}.", rx_port);
  ocudu_assert(tx_layer < nof_tx_layers,
               "Invalid transmission layer {} - number of supported layers is {}.",
               tx_layer,
               nof_tx_layers);
  ch_est_result[rx_port]->get_symbol_ch_estimate(estimates, i_symbol, tx_layer);
}

void dmrs_pusch_estimator_impl::get_symbol_ch_estimate(span<cbf16_t>                              estimates,
                                                       unsigned                                   i_symbol,
                                                       unsigned                                   rx_port,
                                                       unsigned                                   tx_layer,
                                                       const bounded_bitset<MAX_NOF_SUBCARRIERS>& re_mask) const
{
  ocudu_assert(ch_est_result[rx_port], "Invalid channel estimator results for port {}.", rx_port);
  ocudu_assert(tx_layer < nof_tx_layers,
               "Invalid transmission layer {} - number of supported layers is {}.",
               tx_layer,
               nof_tx_layers);
  ch_est_result[rx_port]->get_symbol_ch_estimate(estimates, i_symbol, tx_layer, re_mask);
}

float dmrs_pusch_estimator_impl::get_rsrp(unsigned rx_port, unsigned tx_layer) const
{
  ocudu_assert(ch_est_result[rx_port], "Invalid channel estimator results for port {}.", rx_port);
  ocudu_assert(tx_layer < nof_tx_layers,
               "Invalid transmission layer {} - number of supported layers is {}.",
               tx_layer,
               nof_tx_layers);
  return ch_est_result[rx_port]->get_rsrp(tx_layer);
}

static_vector<float, MAX_PORTS> dmrs_pusch_estimator_impl::get_rsrp_all_ports(unsigned tx_layer) const
{
  ocudu_assert(tx_layer < nof_tx_layers,
               "Invalid transmission layer {} - number of supported layers is {}.",
               tx_layer,
               nof_tx_layers);
  unsigned nof_ports = ch_est_result.size();

  static_vector<float, MAX_PORTS> rsrp_values(nof_ports);
  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    ocudu_assert(ch_est_result[i_port], "Invalid channel estimator results for port {}.", i_port);
    rsrp_values[i_port] = ch_est_result[i_port]->get_rsrp(tx_layer);
  }

  return rsrp_values;
}

float dmrs_pusch_estimator_impl::get_noise_variance(unsigned rx_port) const
{
  ocudu_assert(ch_est_result[rx_port], "Invalid channel estimator results for port {}.", rx_port);
  return ch_est_result[rx_port]->get_noise_variance();
}

float dmrs_pusch_estimator_impl::get_epre(unsigned rx_port) const
{
  ocudu_assert(ch_est_result[rx_port], "Invalid channel estimator results for port {}.", rx_port);
  return ch_est_result[rx_port]->get_epre();
}

float dmrs_pusch_estimator_impl::get_snr(unsigned rx_port) const
{
  ocudu_assert(ch_est_result[rx_port], "Invalid channel estimator results for port {}.", rx_port);
  return ch_est_result[rx_port]->get_snr();
}

float dmrs_pusch_estimator_impl::get_layer_average_snr(unsigned tx_layer) const
{
  ocudu_assert(tx_layer < nof_tx_layers,
               "Invalid transmission layer {} - number of supported layers is {}.",
               tx_layer,
               nof_tx_layers);
  float total_rsrp      = 0;
  float total_noise_var = 0;
  for (const auto& r : ch_est_result) {
    total_rsrp += r->get_rsrp(tx_layer);
    total_noise_var += r->get_noise_variance();
  }

  if (std::isnormal(total_noise_var)) {
    return total_rsrp / total_noise_var;
  }

  return 0;
}

phy_time_unit dmrs_pusch_estimator_impl::get_time_alignment(unsigned rx_port) const
{
  ocudu_assert(ch_est_result[rx_port], "Invalid channel estimator results for port {}.", rx_port);
  return ch_est_result[rx_port]->get_time_alignment();
}

std::optional<float> dmrs_pusch_estimator_impl::get_cfo_Hz(unsigned rx_port) const
{
  ocudu_assert(ch_est_result[rx_port], "Invalid channel estimator results for port {}.", rx_port);
  return ch_est_result[rx_port]->get_cfo_Hz();
}

void dmrs_pusch_estimator_impl::get_channel_state_information(channel_state_information& csi) const
{
  // EPRE and RSRP are reported as a linear average of the results for all Rx ports.
  float    epre_lin      = 0.0F;
  unsigned best_rx_port  = 0;
  float    best_path_snr = -std::numeric_limits<float>::infinity();
  unsigned nof_rx_ports  = ch_est_result.size();

  for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
    // Accumulate EPRE and RSRP values.
    epre_lin += get_epre(i_rx_port);

    // Determine the Rx port with better SNR.
    float port_snr = get_snr(i_rx_port);
    if (port_snr > best_path_snr) {
      best_path_snr = port_snr;
      best_rx_port  = i_rx_port;
    }
  }

  // Set the RSRP according to the estimated values for each receive port (currently, only layer 0).
  static_vector<float, MAX_PORTS> rsrp_help = get_rsrp_all_ports(/*tx_layer=*/0);
  csi.set_rsrp_lin(rsrp_help);

  epre_lin /= static_cast<float>(nof_rx_ports);

  csi.set_epre(convert_power_to_dB(epre_lin));

  // Use the time alignment of the channel path with best SNR.
  csi.set_time_alignment(get_time_alignment(best_rx_port));

  // Use the CFO of the channel path with best SNR.
  std::optional<float> cfo_help = get_cfo_Hz(best_rx_port);
  if (cfo_help.has_value()) {
    csi.set_cfo(*cfo_help);
  }

  // SINR is reported by averaging the signal and noise power contributions of all Rx ports.
  csi.set_sinr_dB(channel_state_information::sinr_type::channel_estimator,
                  convert_power_to_dB(get_layer_average_snr(0)));
}
