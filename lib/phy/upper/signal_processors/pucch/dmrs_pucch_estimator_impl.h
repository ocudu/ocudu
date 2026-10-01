// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief DM-RS PUCCH estimator implementation declaration.

#pragma once

#include "dmrs_pucch_estimator_format2.h"
#include "dmrs_pucch_estimator_formats3_4.h"
#include "ocudu/phy/upper/signal_processors/pucch/dmrs_pucch_estimator.h"

namespace ocudu {

/// DM-RS PUCCH estimator implementation.
class dmrs_pucch_estimator_impl : public dmrs_pucch_estimator
{
public:
  /// Constructor: sets up internal components and acquires their ownership.
  /// \param[in] estimator_format1_ PUCCH Format 1 DM-RS estimator.
  /// \param[in] estimator_format2_ PUCCH Format 1 DM-RS estimator.
  /// \param[in] estimator_formats3_4_ PUCCH Format 3 and Format 4 DM-RS estimator.
  dmrs_pucch_estimator_impl(std::unique_ptr<dmrs_pucch_estimator_format2>    estimator_format2_,
                            std::unique_ptr<dmrs_pucch_estimator_formats3_4> estimator_formats3_4_) :
    estimator_format2(std::move(estimator_format2_)), estimator_formats3_4(std::move(estimator_formats3_4_))
  {
    ocudu_assert(estimator_format2, "Invalid pointer to dmrs_pucch_estimator_format2 object.");
    ocudu_assert(estimator_formats3_4, "Invalid pointer to dmrs_pucch_estimator_formats3_4 object.");
  }

  // See interface for the documentation.
  void
  estimate(channel_estimate& estimate, const resource_grid_reader& grid, const format2_configuration& config) override
  {
    estimator_format2->estimate(estimate, grid, config);
  }

  // See interface for the documentation.
  void
  estimate(channel_estimate& estimate, const resource_grid_reader& grid, const format3_configuration& config) override
  {
    estimator_formats3_4->estimate(estimate, grid, config);
  }

  // See interface for the documentation.
  void
  estimate(channel_estimate& estimate, const resource_grid_reader& grid, const format4_configuration& config) override
  {
    estimator_formats3_4->estimate(estimate, grid, config);
  }

private:
  /// PUCCH DM-RS estimator Format 1 component.
  std::unique_ptr<dmrs_pucch_estimator_format2> estimator_format2;
  /// PUCCH DM-RS estimator Format 3 and Format 4 component.
  std::unique_ptr<dmrs_pucch_estimator_formats3_4> estimator_formats3_4;
};

} // namespace ocudu
