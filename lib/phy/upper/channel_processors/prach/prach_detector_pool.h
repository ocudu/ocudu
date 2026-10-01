// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/prach/prach_detector.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"

namespace ocudu {

/// Concurrent PRACH detector pool.
class prach_detector_pool : public prach_detector
{
public:
  using detector_pool = bounded_unique_object_pool<prach_detector>;

  /// Creates a PRACH detector pool from a shared detector pool.
  explicit prach_detector_pool(std::shared_ptr<detector_pool> detectors_) : detectors(std::move(detectors_)) {}

  // See interface for documentation.
  prach_detection_result detect(const prach_buffer& input, const configuration& config) override
  {
    auto detector = detectors->get();
    report_fatal_error_if_not(detector, "Failed to retrieve PRACH detector.");
    return detector->detect(input, config);
  }

private:
  std::shared_ptr<detector_pool> detectors;
};

} // namespace ocudu
