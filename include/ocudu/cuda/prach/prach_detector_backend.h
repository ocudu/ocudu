// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/cuda/adt/cuda_error.h"
#include "ocudu/cuda/adt/cuda_stream.h"
#include "ocudu/ran/prach/prach_constants.h"
#include <cstdint>
#include <memory>

namespace ocudu {
namespace cuda {

/// \brief Geometry and detection parameters of a single PRACH detection.
///
/// The values are derived by the caller from the PRACH configuration. The backend does not consult
/// the 3GPP tables itself, so that the accelerated and the generic detectors always agree on the
/// geometry they search.
struct prach_detector_geometry {
  /// Length of the preamble sequence (in samples).
  unsigned sequence_length = 0;
  /// Size of the inverse DFT used for the correlation (in samples). Not smaller than \c sequence_length.
  unsigned dft_size = 0;
  /// Number of receive ports to combine.
  unsigned nof_rx_ports = 0;
  /// Number of PRACH OFDM symbols per occasion.
  unsigned nof_symbols = 0;
  /// Number of root sequences to search.
  unsigned nof_sequences = 0;
  /// Index of the first root sequence to search.
  unsigned sequence_start = 0;
  /// Number of cyclic shifts per root sequence.
  unsigned nof_shifts = 0;
  /// Cyclic shift size (in samples). Zero for an unrestricted single-shift configuration.
  unsigned n_cs = 0;
  /// Width of the search window (in inverse DFT samples).
  unsigned win_width = 0;
  /// Guard margin excluded at each side of the search window (in inverse DFT samples).
  unsigned win_margin = 0;
  /// Largest delay reported as a valid detection (in inverse DFT samples).
  unsigned max_delay_samples = 0;
  /// Index of the first preamble to report.
  unsigned start_preamble_index = 0;
  /// Number of preambles to report starting at \c start_preamble_index.
  unsigned nof_preamble_indices = 0;
  /// Set to true to accumulate the symbols of an occasion before the search.
  bool combine_symbols = false;
  /// Detection threshold that the correlation metric is normalized by.
  float threshold = 0.0F;
};

/// \brief Single preamble reported by the accelerated detector.
struct prach_detector_candidate {
  /// Preamble index within the searched range.
  unsigned preamble_index = 0;
  /// Detected delay (in inverse DFT samples).
  unsigned delay_samples = 0;
  /// Correlation metric normalized by the detection threshold.
  float detection_metric = 0.0F;
  /// Average preamble power (in linear scale).
  float preamble_power = 0.0F;
};

/// \brief Outcome of one accelerated detection.
struct prach_detector_output {
  /// RSSI: the mean squared sample magnitude across the searched ports and symbols (in linear scale).
  float rssi = 0.0F;
  /// Detected preambles (in increasing preamble index order).
  static_vector<prach_detector_candidate, prach_constants::MAX_NUM_PREAMBLES> candidates;
};

/// \brief PRACH preamble detector running on an NVIDIA GPU device.
///
/// One backend owns the device memory of a single detection at a time and is not safe to use from
/// several threads at once; the caller serializes the access or holds one backend per thread.
class prach_detector_backend
{
public:
  /// \brief Creates a detector backend on the CUDA device currently selected.
  /// \return A backend on success, otherwise the reason of the failure.
  static cuda_expected<prach_detector_backend> create();

  ~prach_detector_backend();

  prach_detector_backend(const prach_detector_backend&)            = delete;
  prach_detector_backend& operator=(const prach_detector_backend&) = delete;

  prach_detector_backend(prach_detector_backend&& other) noexcept;
  prach_detector_backend& operator=(prach_detector_backend&& other) noexcept;

  /// \brief Detects the PRACH preambles present in a received occasion.
  ///
  /// The call is synchronous: it returns once the results have been read back from the CUDA device.
  ///
  /// \param[out] output   Detected preambles and the measured RSSI.
  /// \param[in]  input    Received samples as packed \c cbf16_t: port-major then symbol-major.
  /// \param[in]  roots    Root sequences to correlate against, one after another.
  /// \param[in]  geometry Geometry of the detection.
  /// \param[in]  stream   CUDA stream that the detection is enqueued on.
  /// \return Nothing on success, otherwise the reason of the failure.
  cuda_result detect(prach_detector_output&         output,
                     span<const uint32_t>           input,
                     span<const cf_t>               roots,
                     const prach_detector_geometry& geometry,
                     const cuda_stream&             stream);

private:
  class impl;

  explicit prach_detector_backend(std::unique_ptr<impl> impl_);

  std::unique_ptr<impl> pimpl;
};

/// \brief Returns true if a CUDA device, capable of running the accelerated detector, is available.
///
/// This check does not create a CUDA context, so it can be safely called before the acceleration has been
/// selected.
bool is_prach_detector_backend_available();

} // namespace cuda
} // namespace ocudu
