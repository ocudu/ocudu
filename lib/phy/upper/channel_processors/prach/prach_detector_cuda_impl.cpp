// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "prach_detector_cuda_impl.h"
#include "prach_detector_generic_thresholds.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/phy/support/prach_buffer.h"
#include "ocudu/ran/prach/prach_cyclic_shifts.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/math/math_utils.h"
#include <algorithm>
#include <cmath>

using namespace ocudu;

prach_detector_cuda_impl::prach_detector_cuda_impl(std::unique_ptr<prach_generator>         generator_,
                                                   std::unique_ptr<prach_detector>          fallback_,
                                                   cuda::prach_detector_backend             backend_,
                                                   cuda::cuda_stream                        stream_,
                                                   const prach_detector_cuda_configuration& config_) :
  generator(std::move(generator_)),
  fallback(std::move(fallback_)),
  backend(std::move(backend_)),
  stream(std::move(stream_)),
  offload_limits(config_)
{
  ocudu_assert(generator, "Invalid PRACH generator");
  ocudu_assert(fallback, "Invalid fallback PRACH detector");
}

prach_detector_cuda_impl::detection_geometry
prach_detector_cuda_impl::make_geometry(const prach_detector::configuration& config)
{
  detection_geometry geometry;

  geometry.preamble_info = is_long_preamble(config.format)
                               ? get_prach_preamble_long_info(config.format)
                               : get_prach_preamble_short_info(config.format, config.ra_scs, false);

  unsigned n_cs = prach_cyclic_shifts_get(config.ra_scs, config.restricted_set, config.zero_correlation_zone);
  report_fatal_error_if_not(n_cs != PRACH_CYCLIC_SHIFTS_RESERVED, "Reserved PRACH cyclic shift");

  unsigned sequence_length =
      is_short_preamble(config.format) ? prach_constants::SHORT_SEQUENCE_LENGTH : prach_constants::LONG_SEQUENCE_LENGTH;

  // Without a cyclic shift, a root sequence carries a single preamble, so all 64 preambles need 64
  // root sequences; otherwise each root sequence carries as many preambles as it has shifts.
  unsigned nof_shifts    = 1;
  unsigned nof_sequences = prach_constants::MAX_NUM_PREAMBLES;
  if (n_cs != 0) {
    nof_shifts    = std::min(prach_constants::MAX_NUM_PREAMBLES, sequence_length / n_cs);
    nof_sequences = divide_ceil(prach_constants::MAX_NUM_PREAMBLES, nof_shifts);
  }

  // Restrict the search to the root sequences that carry the requested preambles.
  unsigned first_sequence = std::min(config.start_preamble_index / nof_shifts, nof_sequences - 1U);
  unsigned last_preamble =
      std::min(prach_constants::MAX_NUM_PREAMBLES, config.start_preamble_index + config.nof_preamble_indices);
  unsigned last_sequence = std::min(nof_sequences, divide_ceil(last_preamble, nof_shifts));
  if (last_sequence <= first_sequence) {
    last_sequence = first_sequence + 1U;
  }

  unsigned dft_size         = is_long_preamble(config.format) ? 1024U : 256U;
  geometry.sampling_rate_hz = dft_size * ra_scs_to_Hz(geometry.preamble_info.scs);

  // Length of the cyclic prefix (in sequence samples) which bounds the delay a preamble can be
  // detected at.
  double   cp_duration = geometry.preamble_info.cp_length.to_seconds();
  unsigned cp_prach =
      static_cast<unsigned>(std::floor(cp_duration * sequence_length * ra_scs_to_Hz(geometry.preamble_info.scs)));

  unsigned win_width = (n_cs == 0) ? cp_prach : std::min(n_cs, cp_prach);
  if (win_width == sequence_length) {
    // A window covering the whole sequence leaves no samples to estimate the noise from.
    win_width -= 20;
  }

  detail::threshold_params th_params;
  th_params.nof_rx_ports                        = config.nof_rx_ports;
  th_params.scs                                 = config.ra_scs;
  th_params.format                              = config.format;
  th_params.zero_correlation_zone               = config.zero_correlation_zone;
  auto [threshold, combine_symbols, win_margin] = detail::get_threshold_and_margin(th_params);

  unsigned max_delay_samples = (n_cs == 0) ? cp_prach : std::min(std::max(n_cs, 1U) - 1U, cp_prach);

  cuda::prach_detector_geometry& device = geometry.device;
  device.sequence_length                = sequence_length;
  device.dft_size                       = dft_size;
  device.nof_rx_ports                   = config.nof_rx_ports;
  device.nof_symbols                    = geometry.preamble_info.nof_symbols;
  device.nof_sequences                  = last_sequence - first_sequence;
  device.sequence_start                 = first_sequence;
  device.nof_shifts                     = nof_shifts;
  device.n_cs                           = n_cs;
  // The search operates on the inverse DFT time axis, so the window and the delays are rescaled from
  // sequence samples to inverse DFT samples.
  device.win_width            = (win_width * dft_size) / sequence_length;
  device.win_margin           = win_margin;
  device.max_delay_samples    = (max_delay_samples * dft_size) / sequence_length;
  device.start_preamble_index = config.start_preamble_index;
  device.nof_preamble_indices = config.nof_preamble_indices;
  device.combine_symbols      = combine_symbols;
  device.threshold            = threshold;

  return geometry;
}

bool prach_detector_cuda_impl::is_worth_offloading(const prach_detector::configuration& config,
                                                   const detection_geometry&            geometry) const
{
  if (is_short_preamble(config.format)) {
    return (config.nof_rx_ports * config.nof_preamble_indices) >= offload_limits.min_short_preamble_work;
  }

  return (config.nof_rx_ports * geometry.device.nof_sequences) >= offload_limits.min_long_preamble_work;
}

uint64_t prach_detector_cuda_impl::make_roots_key(const prach_detector::configuration& config,
                                                  const detection_geometry&            geometry)
{
  // The root sequences depend on the PRACH configuration and on how many shifts each of them
  // carries. Only the sequences carrying the requested preambles are generated, so the range they
  // span is part of the key as well. The key is only ever compared for equality, so any injective
  // packing will do; bit zero marks it as generated, keeping zero for "none generated yet".
  uint64_t key = 1;
  key |= static_cast<uint64_t>(config.root_sequence_index & 0x3ffU) << 1;
  key |= static_cast<uint64_t>(config.zero_correlation_zone & 0xfU) << 11;
  key |= static_cast<uint64_t>(static_cast<unsigned>(config.format) & 0xfU) << 15;
  key |= static_cast<uint64_t>(static_cast<unsigned>(config.ra_scs) & 0xfU) << 19;
  key |= static_cast<uint64_t>(static_cast<unsigned>(config.restricted_set) & 0x3U) << 23;
  key |= static_cast<uint64_t>(geometry.device.nof_shifts & 0x7fU) << 25;
  key |= static_cast<uint64_t>(geometry.device.sequence_start & 0x3fU) << 32;
  key |= static_cast<uint64_t>(geometry.device.nof_sequences & 0x7fU) << 38;
  return key;
}

void prach_detector_cuda_impl::prepare_roots(const prach_detector::configuration& config,
                                             const detection_geometry&            geometry)
{
  uint64_t key = make_roots_key(config, geometry);
  if (key == roots_key) {
    return;
  }

  // The device is handed the sequences it searches so only the root sequences
  // in the active range are generated.
  unsigned sequence_length = geometry.device.sequence_length;
  roots.resize(static_cast<size_t>(geometry.device.nof_sequences) * sequence_length);
  for (unsigned i_sequence = 0; i_sequence != geometry.device.nof_sequences; ++i_sequence) {
    prach_generator::configuration generator_config;
    generator_config.format                = config.format;
    generator_config.root_sequence_index   = config.root_sequence_index;
    generator_config.preamble_index        = (geometry.device.sequence_start + i_sequence) * geometry.device.nof_shifts;
    generator_config.restricted_set        = config.restricted_set;
    generator_config.zero_correlation_zone = config.zero_correlation_zone;

    span<const cf_t> root = generator->generate(generator_config);
    ocuduvec::copy(span<cf_t>(roots).subspan(i_sequence * sequence_length, sequence_length), root);
  }

  roots_key = key;
}

void prach_detector_cuda_impl::pack_input(const prach_buffer&                  input,
                                          const prach_detector::configuration& config,
                                          const detection_geometry&            geometry)
{
  unsigned sequence_length = geometry.device.sequence_length;
  unsigned nof_symbols     = geometry.device.nof_symbols;

  packed_input.resize(static_cast<size_t>(config.nof_rx_ports) * nof_symbols * sequence_length);
  for (unsigned i_port = 0; i_port != config.nof_rx_ports; ++i_port) {
    for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
      span<const cbf16_t> symbol = input.get_symbol(i_port, 0, 0, i_symbol);
      // The packed input is held as the words the device reads, so the destination is viewed as the
      // samples it actually carries in order to copy through a checked, bounded operation.
      span<cbf16_t> packed_view(reinterpret_cast<cbf16_t*>(packed_input.data()), packed_input.size());
      ocuduvec::copy(packed_view.subspan((i_port * nof_symbols + i_symbol) * sequence_length, sequence_length), symbol);
    }
  }
}

prach_detection_result prach_detector_cuda_impl::detect(const prach_buffer&                  input,
                                                        const prach_detector::configuration& config)
{
  detection_geometry geometry = make_geometry(config);

  if (!is_worth_offloading(config, geometry)) {
    return fallback->detect(input, config);
  }

  prepare_roots(config, geometry);
  pack_input(input, config, geometry);

  cuda::prach_detector_output output;
  cuda::cuda_result           result = backend.detect(output, packed_input, roots, geometry.device, stream);
  if (!result.has_value()) {
    // A failed detection falls back to the CPU rather than being dropped. Several UEs may have sent a
    // preamble in this occasion, and dropping it makes all of them invisible: each one of those UEs
    // would have to wait for its random access response to time out and then transmit again at
    // the next PRACH occasion.
    return fallback->detect(input, config);
  }

  prach_detection_result detection;
  detection.rssi_dB          = convert_power_to_dB(output.rssi);
  detection.time_resolution  = phy_time_unit::from_seconds(1.0 / geometry.sampling_rate_hz);
  detection.time_advance_max = phy_time_unit::from_seconds(static_cast<double>(geometry.device.max_delay_samples) *
                                                           0.8 / geometry.sampling_rate_hz);
  detection.preambles.clear();
  for (const cuda::prach_detector_candidate& candidate : output.candidates) {
    prach_detection_result::preamble_indication& preamble = detection.preambles.emplace_back();
    preamble.preamble_index                               = candidate.preamble_index;
    preamble.time_advance =
        phy_time_unit::from_seconds(static_cast<double>(candidate.delay_samples) / geometry.sampling_rate_hz);
    preamble.detection_metric  = candidate.detection_metric;
    preamble.preamble_power_dB = convert_power_to_dB(candidate.preamble_power);
  }

  return detection;
}
