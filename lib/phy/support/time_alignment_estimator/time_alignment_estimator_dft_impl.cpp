// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "time_alignment_estimator_dft_impl.h"
#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/ocuduvec/compare.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/ocuduvec/dot_prod.h"
#include "ocudu/ocuduvec/modulus_square.h"
#include "ocudu/phy/support/re_buffer.h"
#include "ocudu/phy/support/time_alignment_estimator/time_alignment_measurement.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/support/math/curve_fitting_find_max.h"
#include <algorithm>
#include <utility>

using namespace ocudu;

const unsigned time_alignment_estimator_dft_impl::min_dft_size = pow2(log2_ceil(static_cast<unsigned>(
    1.0F / (15000 * phy_time_unit::from_timing_advance(1, subcarrier_spacing::kHz15).to_seconds()))));

const unsigned time_alignment_estimator_dft_impl::max_dft_size = pow2(log2_ceil(max_nof_re));

time_alignment_estimator_dft_impl::time_alignment_estimator_dft_impl(
    ocudu::time_alignment_estimator_dft_impl::collection_dft_processors dft_processors_) :
  dft_processors(std::move(dft_processors_)), idft_abs2(max_dft_size)
{
  // Make sure all the possible powers of 2 between the minimum and the maximum DFT sizes are present and valid.
  for (unsigned dft_size     = time_alignment_estimator_dft_impl::min_dft_size,
                dft_size_max = time_alignment_estimator_dft_impl::max_dft_size;
       dft_size <= dft_size_max;
       dft_size *= 2) {
    // Check the IDFT size is present.
    ocudu_assert(dft_processors.count(dft_size), "Missing DFT.");

    // Select IDFT and validate.
    const auto& idft = dft_processors.at(dft_size);
    ocudu_assert(idft, "Invalid DFT processor.");
    ocudu_assert(idft->get_size() == dft_size, "Invalid DFT size.");
    ocudu_assert(idft->get_direction() == dft_processor::direction::INVERSE, "Invalid DFT direction.");
  }
}

time_alignment_measurement time_alignment_estimator_dft_impl::estimate(const re_buffer_reader<cf_t>&          symbols,
                                                                       const bounded_bitset<max_nof_symbols>& mask,
                                                                       subcarrier_spacing                     scs,
                                                                       double                                 max_ta)
{
  ocudu_assert(mask.count() == symbols.get_slice(0).size(),
               "The number of complex symbols per port {} does not match the mask size {}.",
               symbols.get_slice(0).size(),
               mask.count());
  unsigned       mask_highest = mask.find_highest();
  unsigned       mask_lowest  = mask.find_lowest();
  dft_processor& idft         = get_idft(mask_highest - mask_lowest + 1);

  // Get IDFT input buffer.
  span<cf_t> channel_observed_freq = idft.get_input();

  // Zero input buffer.
  ocuduvec::zero(channel_observed_freq);

  // Prepare correlation temporary buffer.
  span<float> correlation = span<float>(idft_abs2).first(idft.get_size());

  // Correlate each of the symbol slices.
  for (unsigned i_in = 0, max_in = symbols.get_nof_slices(); i_in != max_in; ++i_in) {
    // Get view of the input symbols for the given slice.
    span<const cf_t> symbols_in = symbols.get_slice(i_in);

    // Write the symbols in their corresponding positions.
    mask.for_each(
        0, mask.size(), [&channel_observed_freq, &symbols_in, mask_lowest, i_lse = 0U](unsigned i_re) mutable {
          channel_observed_freq[i_re - mask_lowest] = symbols_in[i_lse++];
        });

    // Perform correlation in frequency domain.
    span<const cf_t> channel_observed_time = idft.run();

    // Calculate the absolute square of the correlation.
    if (i_in == 0) {
      ocuduvec::modulus_square(correlation, channel_observed_time);
    } else {
      // Accumulate the correlation.
      ocuduvec::modulus_square_and_add(correlation, channel_observed_time, correlation);
    }
  }

  // Estimate the time alignment from the correlation.
  return estimate_ta_correlation(correlation, /* stride = */ 1, scs, max_ta);
}

time_alignment_measurement time_alignment_estimator_dft_impl::estimate(span<const cf_t>                       symbols,
                                                                       const bounded_bitset<max_nof_symbols>& mask,
                                                                       subcarrier_spacing                     scs,
                                                                       double                                 max_ta)
{
  modular_re_buffer_reader<cf_t, 1> symbols_view(1, symbols.size());
  symbols_view.set_slice(0, symbols);

  return estimate(symbols_view, mask, scs, max_ta);
}

time_alignment_measurement time_alignment_estimator_dft_impl::estimate(const re_buffer_reader<cf_t>& symbols,
                                                                       unsigned                      stride,
                                                                       ocudu::subcarrier_spacing     scs,
                                                                       double                        max_ta)
{
  unsigned       nof_symbols = symbols.get_nof_re();
  dft_processor& idft        = get_idft(nof_symbols);

  // Get IDFT input buffer.
  span<cf_t> channel_observed_freq = idft.get_input();

  // Zero input buffer.
  ocuduvec::zero(channel_observed_freq.last(channel_observed_freq.size() - nof_symbols));

  // Prepare correlation temporary buffer.
  span<float> correlation = span<float>(idft_abs2).first(idft.get_size());

  // Correlate each of the symbol slices.
  for (unsigned i_in = 0, max_in = symbols.get_nof_slices(); i_in != max_in; ++i_in) {
    // Get view of the input symbols for the given slice.
    span<const cf_t> symbols_in = symbols.get_slice(i_in);

    // Write the symbols in their corresponding positions.
    ocuduvec::copy(channel_observed_freq.first(nof_symbols), symbols_in);

    // Perform correlation in frequency domain.
    span<const cf_t> channel_observed_time = idft.run();

    // Calculate the absolute square of the correlation.
    if (i_in == 0) {
      ocuduvec::modulus_square(correlation, channel_observed_time);
    } else {
      // Accumulate the correlation.
      ocuduvec::modulus_square_and_add(correlation, channel_observed_time, correlation);
    }
  }

  // Estimate the time alignment from the correlation.
  return estimate_ta_correlation(correlation, stride, scs, max_ta);
}

time_alignment_measurement time_alignment_estimator_dft_impl::estimate(span<const cf_t>   symbols,
                                                                       unsigned           stride,
                                                                       subcarrier_spacing scs,
                                                                       double             max_ta)
{
  modular_re_buffer_reader<cf_t, 1> symbols_view(1, symbols.size());
  symbols_view.set_slice(0, symbols);

  return estimate(symbols_view, stride, scs, max_ta);
}

dft_processor& time_alignment_estimator_dft_impl::get_idft(unsigned nof_required_re)
{
  // Ensure the number of required RE is smaller than the maximum DFT size.
  ocudu_assert(nof_required_re <= max_nof_re,
               "The number of required RE (i.e., {}) is larger than the maximum allowed number of RE (i.e., {}).",
               nof_required_re,
               max_nof_re);

  // Leave some guards to avoid circular interference.
  nof_required_re = (nof_required_re * max_dft_size) / max_nof_re;

  // Get the next power of 2 DFT size.
  unsigned dft_size = pow2(log2_ceil(nof_required_re));
  dft_size          = std::max(min_dft_size, dft_size);

  // Select the DFT processor.
  return *dft_processors[dft_size];
}

time_alignment_measurement time_alignment_estimator_dft_impl::estimate_ta_correlation(span<const float>  correlation,
                                                                                      unsigned           stride,
                                                                                      subcarrier_spacing scs,
                                                                                      double             max_ta)
{
  // Calculate half cyclic prefix duration.
  phy_time_unit half_cyclic_prefix_duration =
      phy_time_unit::from_units_of_kappa(144) / pow2(to_numerology_value(scs) + 1);

  // Deduce sampling rate.
  double sampling_rate_Hz = correlation.size() * scs_to_khz(scs) * 1000 * stride;

  // Maximum number of samples limited by half cyclic prefix.
  unsigned max_ta_samples = std::floor(half_cyclic_prefix_duration.to_seconds() * sampling_rate_Hz);

  // Select the most limiting number of samples.
  if (std::isnormal(max_ta)) {
    max_ta_samples = std::min(max_ta_samples, static_cast<unsigned>(std::floor(max_ta * sampling_rate_Hz)));
  }

  // Find the maximum of the delayed taps.
  std::pair<unsigned, float> observed_max_delay = ocuduvec::max_element(correlation.first(max_ta_samples));

  // Find the maximum of the advanced taps.
  std::pair<unsigned, float> observed_max_advance = ocuduvec::max_element(correlation.last(max_ta_samples));

  // Determine the number of taps the signal is advanced or delayed (negative).
  int idx = -(max_ta_samples - observed_max_advance.first);
  if (observed_max_delay.second >= observed_max_advance.second) {
    idx = observed_max_delay.first;
  }

  // Calculate the fractional sample.
  double fractional_sample_index = 0.0F;
  if (correlation.size() != max_dft_size) {
    // Select the number of samples for the fractional sample calculation depending on the maximum number of samples.
    unsigned nof_taps = (max_ta_samples > 2) ? 5 : 3;

    // Extract samples around the peak.
    static_vector<float, 5> peak_center_correlation(nof_taps);
    for (unsigned i = 0; i != nof_taps; ++i) {
      peak_center_correlation[i] = correlation[(idx + i + correlation.size() - nof_taps / 2) % correlation.size()];
    }

    // Calculate the fractional sample.
    fractional_sample_index = curve_fitting_fractional_max(peak_center_correlation);
  }

  // Final calculation of the time alignment in seconds.
  double t_align_seconds = (static_cast<double>(idx) + fractional_sample_index) / sampling_rate_Hz;

  // Produce results.
  return time_alignment_measurement{
      .time_alignment = t_align_seconds,
      .resolution     = 1.0 / sampling_rate_Hz,
      .min            = -static_cast<double>(max_ta_samples) / sampling_rate_Hz,
      .max            = static_cast<double>(max_ta_samples) / sampling_rate_Hz,
  };
}
