// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cuda/adt/cuda_stream.h"
#include "ocudu/cuda/prach/prach_detector_backend.h"
#include "ocudu/phy/upper/channel_processors/prach/factories.h"
#include "ocudu/phy/upper/channel_processors/prach/prach_detector.h"
#include "ocudu/phy/upper/channel_processors/prach/prach_generator.h"
#include "ocudu/ran/prach/prach_preamble_information.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace ocudu {

/// \brief PRACH detector that searches the preambles on an NVIDIA GPU.
///
/// The detector uses the fallback in two cases: the occasion is too small to offload, or the device
/// failed the detection. It always produces a result.
///
/// \remark An instance holds the state of one detection and is not safe to use from several threads
/// at once, as is the case for the generic detector. Use \ref create_prach_detector_pool_factory to
/// obtain one instance per thread.
class prach_detector_cuda_impl : public prach_detector
{
public:
  /// \brief Creates a CUDA-accelerated detector.
  ///
  /// \param[in] generator_ Generator of the root sequences the received samples are correlated with.
  /// \param[in] fallback_  Detector used when the detection is not offloaded or the device fails.
  /// \param[in] backend_   Device backend performing the detection.
  /// \param[in] stream_    CUDA stream carrying the enqueued detections.
  /// \param[in] config_    Offload limits.
  prach_detector_cuda_impl(std::unique_ptr<prach_generator>         generator_,
                           std::unique_ptr<prach_detector>          fallback_,
                           cuda::prach_detector_backend             backend_,
                           cuda::cuda_stream                        stream_,
                           const prach_detector_cuda_configuration& config_);

  // See interface for documentation.
  prach_detection_result detect(const prach_buffer& input, const configuration& config) override;

private:
  /// \brief Geometry of one detection.
  ///
  /// It holds the geometry handed to the device together with the quantities the result is converted
  /// with, both of which are derived from the same PRACH configuration.
  struct detection_geometry {
    cuda::prach_detector_geometry device;
    prach_preamble_information    preamble_info;
    double                        sampling_rate_hz;
  };

  /// Derives the geometry of a detection from its PRACH configuration.
  static detection_geometry make_geometry(const configuration& config);

  /// Returns true when the occasion is large enough to be worth offloading.
  bool is_worth_offloading(const configuration& config, const detection_geometry& geometry) const;

  /// Generates the root sequences the occasion is searched against, reusing the previous ones when
  /// the configuration has not changed.
  void prepare_roots(const configuration& config, const detection_geometry& geometry);

  /// Packs the received samples of an occasion into the layout expected by the CUDA device.
  void pack_input(const prach_buffer& input, const configuration& config, const detection_geometry& geometry);

  /// Identifies the root sequences currently held in \ref roots.
  static uint64_t make_roots_key(const configuration& config, const detection_geometry& geometry);

  std::unique_ptr<prach_generator>  generator;
  std::unique_ptr<prach_detector>   fallback;
  cuda::prach_detector_backend      backend;
  cuda::cuda_stream                 stream;
  prach_detector_cuda_configuration offload_limits;

  /// Received samples of the occasion being detected. (Packing layout: port-major, then symbol-major).
  std::vector<uint32_t> packed_input;
  /// Root sequences of the current PRACH configuration, one after another.
  std::vector<cf_t> roots;
  /// Identifies the configuration \ref roots was generated for. Zero when none has been generated.
  uint64_t roots_key = 0;
};

} // namespace ocudu
