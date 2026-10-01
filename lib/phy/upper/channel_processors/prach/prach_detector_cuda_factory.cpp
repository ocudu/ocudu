// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "prach_detector_cuda_impl.h"
#include "ocudu/phy/upper/channel_processors/prach/factories.h"
#include "ocudu/support/error_handling.h"

using namespace ocudu;

namespace {

/// \brief Factory wrapping another one so that the detectors it creates run on an NVIDIA GPU.
///
/// The device resources of a detector belong to that detector, so a pool of them holds one CUDA stream
/// and one set of device buffers per pooled instance.
class prach_detector_cuda_factory : public prach_detector_factory
{
public:
  prach_detector_cuda_factory(std::shared_ptr<prach_detector_factory>  fallback_factory_,
                              std::shared_ptr<prach_generator_factory> generator_factory_,
                              const prach_detector_cuda_configuration& config_) :
    fallback_factory(std::move(fallback_factory_)), generator_factory(std::move(generator_factory_)), config(config_)
  {
    ocudu_assert(fallback_factory, "Invalid fallback PRACH detector factory");
    ocudu_assert(generator_factory, "Invalid PRACH generator factory");
  }

  // See the prach_detector_factory interface for documentation.
  std::unique_ptr<prach_detector> create() override
  {
    // Detection has a deadline within the slot it arrives in, so it is submitted on a high-priority stream.
    cuda::cuda_expected<cuda::cuda_stream> stream = cuda::cuda_stream::create(cuda::cuda_stream_priority::high);
    if (!stream.has_value()) {
      return fallback_factory->create();
    }

    cuda::cuda_expected<cuda::prach_detector_backend> backend = cuda::prach_detector_backend::create();
    if (!backend.has_value()) {
      return fallback_factory->create();
    }

    return std::make_unique<prach_detector_cuda_impl>(generator_factory->create(),
                                                      fallback_factory->create(),
                                                      std::move(backend.value()),
                                                      std::move(stream.value()),
                                                      config);
  }

  // See the prach_detector_factory interface for documentation.
  std::unique_ptr<prach_detector_validator> create_validator() override
  {
    // The accelerated detector searches the same configurations as the one it falls back to.
    return fallback_factory->create_validator();
  }

private:
  std::shared_ptr<prach_detector_factory>  fallback_factory;
  std::shared_ptr<prach_generator_factory> generator_factory;
  prach_detector_cuda_configuration        config;
};

} // namespace

std::shared_ptr<prach_detector_factory>
ocudu::create_prach_detector_factory_cuda(std::shared_ptr<prach_detector_factory>  fallback_factory,
                                          std::shared_ptr<prach_generator_factory> generator_factory,
                                          const prach_detector_cuda_configuration& config)
{
  if (!cuda::is_prach_detector_backend_available()) {
    return nullptr;
  }

  return std::make_shared<prach_detector_cuda_factory>(
      std::move(fallback_factory), std::move(generator_factory), config);
}
