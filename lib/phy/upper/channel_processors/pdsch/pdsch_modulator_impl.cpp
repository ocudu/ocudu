// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdsch_modulator_impl.h"
#include "ocudu/phy/support/resource_grid_mapper.h"
#include "ocudu/phy/upper/dmrs_mapping.h"
#include "ocudu/ran/precoding/precoding_weight_matrix_formatters.h"

using namespace ocudu;

const bit_buffer& pdsch_modulator_impl::scramble(const bit_buffer& b, unsigned q, const config_t& config)
{
  temp_b_hat.resize(b.size());

  // Calculate initial scrambling state.
  unsigned c_init = (static_cast<unsigned>(config.rnti) << 15U) + (q << 14U) + config.n_id;

  // Initialize scrambling sequence.
  scrambler->init(c_init);

  // Apply scrambling sequence.
  scrambler->apply_xor(temp_b_hat, b);

  return temp_b_hat;
}

float pdsch_modulator_impl::modulate(span<ci8_t> d_pdsch, const bit_buffer& b_hat, modulation_scheme modulation)
{
  // Actual modulate.
  return modulator->modulate(d_pdsch, b_hat, modulation);
}

void pdsch_modulator_impl::map(resource_grid_writer&    grid,
                               span<const ci8_t>        data_re,
                               unsigned                 i_codeword,
                               precoding_configuration& precoding,
                               span<uint8_t>            ports,
                               const config_t&          config)
{
  ocudu_assert(config.time_alloc.stop() <= MAX_NSYMB_PER_SLOT,
               "The time allocation of the transmission {} exceeds the slot boundary.",
               config.time_alloc);

  // PDSCH OFDM symbol mask.
  symbol_slot_mask symbols;
  symbols.fill(config.time_alloc.start(), config.time_alloc.stop());

  // Reserved REs, including DM-RS and CSI-RS.
  re_pattern_list reserved(config.reserved);

  // Get DM-RS RE pattern.
  re_pattern dmrs_pattern = get_dmrs_pattern(config.dmrs_type,
                                             config.bwp.start(),
                                             config.bwp.length(),
                                             config.nof_cdm_groups_without_data,
                                             config.dmrs_symb_pos);

  // Merge DM-RS RE pattern into the reserved RE patterns.
  reserved.merge(dmrs_pattern);

  resource_grid_mapper::symbol_buffer_adapter buffer_adapter(data_re);

  // Prepare resource grid mapper allocation.
  resource_grid_mapper::allocation_configuration allocation = {
      .bwp = config.bwp, .freq_alloc = config.freq_allocation, .time_alloc = config.time_alloc};

  // Map into the resource grid.
  mapper->map(grid, buffer_adapter, allocation, reserved, ports, precoding);
}

void pdsch_modulator_impl::modulate(resource_grid_writer&            grid,
                                    span<const bit_buffer>           codewords,
                                    const pdsch_modulator::config_t& config)
{
  // Number of modulated codewords.
  unsigned nof_codewords = codewords.size();

  // Ensure that if the second codeword is being mapped, the modulation is provided.
  ocudu_assert((nof_codewords == 1) || (config.modulation2.has_value()),
               "Missing second codeword modulation in a two-codeword modulation.");

  // Number of ports for precoding - in case of two codewords, each is mapped to a different half.
  unsigned nof_ports = config.ports.size() / nof_codewords;

  // Total number of transmission layers.
  unsigned nof_layers = config.precoding.get().get_nof_layers();

  // List of resource grid ports where each codeword is being mapped to.
  static_vector<uint8_t, precoding_constants::MAX_NOF_PORTS> ports(nof_ports);

  for (unsigned i_cw = 0; i_cw != nof_codewords; ++i_cw) {
    modulation_scheme mod = (i_cw == 0) ? config.modulation1 : *config.modulation2;
    unsigned          Qm  = get_bits_per_symbol(mod);

    // Calculate number of REs.
    unsigned nof_bits = codewords[i_cw].size();
    unsigned nof_re   = nof_bits / Qm;

    // Scramble.
    const bit_buffer& b_hat = scramble(codewords[i_cw], i_cw, config);

    // View over the PDSCH symbols buffer. For a single layer, skip layer mapping and use the final destination RE
    // buffer.
    span<ci8_t> pdsch_symbols = span<ci8_t>(temp_pdsch_symbols).first(nof_re);

    // Modulate codeword.
    float scaling = modulate(pdsch_symbols, b_hat, mod);

    unsigned nof_layers_cw0 = nof_layers / nof_codewords;
    unsigned nof_layers_cw1 = nof_layers - nof_layers_cw0;

    // Extract the codeword-specific precoding.
    precoding_configuration precoding = config.precoding.get().slice(
        interval<uint8_t>::start_and_len(i_cw * nof_layers_cw0, (i_cw == 0) ? nof_layers_cw0 : nof_layers_cw1),
        interval<uint8_t>::start_and_len(i_cw * nof_ports, nof_ports));

    // Populate the list of resource grid ports for this codeword. The first codeword is mapped to the first half of
    // ports, while the second is mapped to the last half.
    span<const uint8_t> codeword_ports =
        span(config.ports.data(), config.ports.size()).subspan(i_cw * nof_ports, nof_ports);
    ocuduvec::copy(ports, codeword_ports);

    // Apply scaling.
    if (std::isnormal(config.scaling)) {
      scaling *= config.scaling;
    }

    precoding *= scaling;

    // Map resource elements.
    map(grid, pdsch_symbols, i_cw, precoding, ports, config);
  }
}
