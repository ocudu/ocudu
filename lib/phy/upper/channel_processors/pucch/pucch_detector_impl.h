// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief PUCCH detector declaration.

#pragma once

#include "pucch_detector_format0.h"
#include "pucch_detector_format1.h"
#include "ocudu/phy/upper/channel_processors/pucch/pucch_detector.h"

namespace ocudu {

/// PUCCH detector implementation for Formats 0 and 1.
class pucch_detector_impl : public pucch_detector
{
public:
  /// \brief Constructor: provides access to the actual Format 0 and Format 1 detectors.
  /// \param[in] detector_format0_ PUCCH Format 0 detector.
  /// \param[in] detector_format1_ PUCCH Format 1 detector.
  pucch_detector_impl(std::unique_ptr<pucch_detector_format0> detector_format0_,
                      std::unique_ptr<pucch_detector_format1> detector_format1_) :
    detector_format0(std::move(detector_format0_)), detector_format1(std::move(detector_format1_))
  {
    ocudu_assert(detector_format0, "PUCCH Format 0 detector.");
    ocudu_assert(detector_format1, "PUCCH Format 1 detector.");
  }

  // See interface for documentation.
  std::pair<pucch_uci_message, channel_state_information> detect(const resource_grid_reader&  grid,
                                                                 const format0_configuration& config) override
  {
    return detector_format0->detect(grid, config);
  }

  // See interface for documentation.
  const pucch_format1_map<pucch_detection_result_csi>& detect(const resource_grid_reader&        grid,
                                                              const format1_configuration&       config,
                                                              const pucch_format1_map<unsigned>& mux_map) override
  {
    return detector_format1->detect(grid, config, mux_map);
  }

private:
  /// PUCCH Format 0 detector.
  std::unique_ptr<pucch_detector_format0> detector_format0;
  /// PUCCH Format 1 detector.
  std::unique_ptr<pucch_detector_format1> detector_format1;
};

} // namespace ocudu
