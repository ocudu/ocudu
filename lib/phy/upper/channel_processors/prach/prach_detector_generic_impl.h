// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/adt/tensor.h"
#include "ocudu/phy/generic_functions/dft_processor.h"
#include "ocudu/phy/upper/channel_processors/prach/prach_detector.h"
#include "ocudu/phy/upper/channel_processors/prach/prach_generator.h"

namespace ocudu {

/// Implements a parameter validator for the PRACH detector.
class prach_detector_validator_impl : public prach_detector_validator
{
public:
  // See interface for documentation.
  error_type<std::string> is_valid(const prach_detector::configuration& config) const override;
};

/// \brief Implements a simple PRACH detector.
///
/// Detects PRACH sequences from the frequency-domain channel samples. The detector is inspired by the generalized
/// log-likelihood ratio detection test.
class prach_detector_generic_impl : public prach_detector
{
public:
  /// \brief Constructor - Acquires ownership of the internal components.
  /// \param[in] idft_long_       Inverse DFT processor for long preambles.
  /// \param[in] idft_short_      Inverse DFT processor for short preambles.
  /// \param[in] generator_       PRACH frequency-domain sequence generator.
  /// \param[in] th_scaling       PRACH threshold scaling factor.
  /// \remark Assertions are triggered if the IDFT sizes are smaller than their sequences or greater than \ref
  /// MAX_IDFT_SIZE.
  prach_detector_generic_impl(std::unique_ptr<dft_processor>   idft_long_,
                              std::unique_ptr<dft_processor>   idft_short_,
                              std::unique_ptr<prach_generator> generator_,
                              float                            th_scaling);

  // See interface for documentation.
  prach_detection_result detect(const prach_buffer& input, const configuration& config) override;

private:
  /// Maximum IDFT size allowed.
  static constexpr unsigned MAX_IDFT_SIZE = 4096;

  /// \brief Estimates and accumulates the noise estimation.
  ///
  /// For each possible timing offset in the detection window, the noise is estimated by subtracting \c input (roughly
  /// speaking, an estimation of the PRACH power) from \c reference (roughly speaking, the total measured energy).
  ///
  /// \param[in,out] accumulator Noise estimation accumulator.
  /// \param[in]     reference   Reference power measurement, including noise.
  /// \param[in]     input       Input samples.
  /// \remark The input size must be equal to the accumulator size.
  static void vector_noise_estimation(span<float> accumulator, float reference, span<const float> input)
  {
    std::transform(
        input.begin(), input.end(), accumulator.begin(), accumulator.begin(), [&reference](float value, float acc) {
          float diff = reference - value;
          if (!std::isnormal(diff)) {
            diff = 1e-9F;
          }
          return diff + acc;
        });
  }

  /// Correlation tensor dimensions.
  enum class metric_global_dims : unsigned {
    /// Sample within a window.
    sample = 0,
    /// Correlation window for a sample.
    window,
    /// Total number of dimensions.
    all
  };

  /// \brief Detection threshold correction factor.
  ///
  /// The actual detection threshold is given by the default value multiplied by this scaling factor.
  float threshold_scaling = 1.0F;

  /// Metric global numerator.
  static_tensor<static_cast<unsigned>(metric_global_dims::all), float, MAX_IDFT_SIZE, metric_global_dims>
      metric_global_num;
  /// Metric global denominator.
  static_tensor<static_cast<unsigned>(metric_global_dims::all), float, MAX_IDFT_SIZE, metric_global_dims>
      metric_global_den;
  /// Temporal storage.
  std::array<cf_t, MAX_IDFT_SIZE>  cf_temp;
  std::array<float, MAX_IDFT_SIZE> temp;
  std::array<float, MAX_IDFT_SIZE> temp2;

  std::unique_ptr<dft_processor>   idft_long;
  std::unique_ptr<dft_processor>   idft_short;
  std::unique_ptr<prach_generator> generator;
};

} // namespace ocudu
