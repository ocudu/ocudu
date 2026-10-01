// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/phy/generic_functions/generic_functions_factories.h"
#include "ocudu/phy/upper/channel_processors/prach/prach_detector.h"
#include "ocudu/phy/upper/channel_processors/prach/prach_generator.h"
#include <memory>

namespace ocudu {

class prach_generator_factory;
class task_executor;

class prach_detector_factory
{
public:
  virtual ~prach_detector_factory()                                    = default;
  virtual std::unique_ptr<prach_detector>           create()           = 0;
  virtual std::unique_ptr<prach_detector_validator> create_validator() = 0;
  virtual std::unique_ptr<prach_detector>           create(ocudulog::basic_logger& logger, bool log_all_opportunities);
};

struct prach_detector_factory_sw_configuration {
  unsigned idft_long_size    = 1024;
  unsigned idft_short_size   = 256;
  float    threshold_scaling = 1.0F;
};

std::shared_ptr<prach_detector_factory>
create_prach_detector_factory_sw(std::shared_ptr<dft_processor_factory>         dft_factory,
                                 std::shared_ptr<prach_generator_factory>       prach_gen_factory,
                                 const prach_detector_factory_sw_configuration& config = {});

std::shared_ptr<prach_detector_factory>
create_prach_detector_pool_factory(std::shared_ptr<prach_detector_factory> factory, unsigned nof_concurrent_threads);

/// \brief Configuration of the NVIDIA CUDA-accelerated PRACH detector.
///
/// The offload limits decide, per occasion, whether the detection is worth the cost of moving the
/// samples to the device. They are expressed in correlations, that is, the number of independent
/// search actions required by the occasion. The defaults were measured on an NVIDIA RTX 5000 Ada.
struct prach_detector_cuda_configuration {
  /// Smallest amount of work offloaded for a short preamble.
  unsigned min_short_preamble_work = 64;
  /// Smallest amount of work offloaded for a long preamble.
  unsigned min_long_preamble_work = 4;
};

/// \brief Creates a PRACH detector factory that searches the preambles on an NVIDIA GPU.
///
/// Each created detector decorates one from \c fallback_factory. It uses the fallback in two cases: the
/// occasion is too small to offload, or the device failed the detection.
///
/// \remark The accelerated detector searches with the same algorithm as the generic one, so the
/// probability of detection and of false alarm match it. The device accumulates the correlation in a
/// different order, so the metrics agree closely rather than exactly.
///
/// \param[in] fallback_factory  Factory of the detectors used when the detection is not offloaded.
/// \param[in] generator_factory Factory of the generators producing the root sequences.
/// \param[in] config            Offload limits.
/// \return A factory, or nullptr when the build has no CUDA support or no device is available, in
/// which case the caller uses \c fallback_factory directly (without the wrapper).
std::shared_ptr<prach_detector_factory>
create_prach_detector_factory_cuda(std::shared_ptr<prach_detector_factory>  fallback_factory,
                                   std::shared_ptr<prach_generator_factory> generator_factory,
                                   const prach_detector_cuda_configuration& config = {});

class prach_generator_factory
{
public:
  virtual ~prach_generator_factory()                = default;
  virtual std::unique_ptr<prach_generator> create() = 0;
};

std::shared_ptr<prach_generator_factory> create_prach_generator_factory_sw();

} // namespace ocudu
