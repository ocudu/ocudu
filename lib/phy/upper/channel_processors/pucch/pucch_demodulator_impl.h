// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief PUCCH demodulator implementation declaration.

#pragma once

#include "pucch_demodulator_format2.h"
#include "pucch_demodulator_format3.h"
#include "pucch_demodulator_format4.h"
#include "ocudu/phy/upper/channel_processors/pucch/pucch_demodulator.h"

namespace ocudu {

/// PUCCH demodulator implementation.
class pucch_demodulator_impl : public pucch_demodulator
{
public:
  /// Constructor: sets up internal components and acquires their ownership.
  /// \param[in] demodulator_format2_ PUCCH Format 2 demodulator.
  /// \param[in] demodulator_format3_ PUCCH Format 3 demodulator.
  pucch_demodulator_impl(std::unique_ptr<pucch_demodulator_format2> demodulator_format2_,
                         std::unique_ptr<pucch_demodulator_format3> demodulator_format3_,
                         std::unique_ptr<pucch_demodulator_format4> demodulator_format4_) :
    demodulator_format2(std::move(demodulator_format2_)),
    demodulator_format3(std::move(demodulator_format3_)),
    demodulator_format4(std::move(demodulator_format4_))
  {
    ocudu_assert(demodulator_format2, "Invalid pointer to pucch_demodulator_format2 object.");
    ocudu_assert(demodulator_format3, "Invalid pointer to pucch_demodulator_format3 object.");
    ocudu_assert(demodulator_format4, "Invalid pointer to pucch_demodulator_format4 object.");
  }

  // See interface for the documentation.
  void demodulate(span<log_likelihood_ratio>   llr,
                  const resource_grid_reader&  grid,
                  const channel_estimate&      estimates,
                  const format2_configuration& config) override
  {
    demodulator_format2->demodulate(llr, grid, estimates, config);
  }

  // See interface for the documentation.
  void demodulate(span<log_likelihood_ratio>   llr,
                  const resource_grid_reader&  grid,
                  const channel_estimate&      estimates,
                  const format3_configuration& config) override
  {
    demodulator_format3->demodulate(llr, grid, estimates, config);
  }

  // See interface for the documentation.
  void demodulate(span<log_likelihood_ratio>   llr,
                  const resource_grid_reader&  grid,
                  const channel_estimate&      estimates,
                  const format4_configuration& config) override
  {
    demodulator_format4->demodulate(llr, grid, estimates, config);
  }

private:
  /// PUCCH demodulator Format 2 component.
  std::unique_ptr<pucch_demodulator_format2> demodulator_format2;
  /// PUCCH demodulator Format 3 component.
  std::unique_ptr<pucch_demodulator_format3> demodulator_format3;
  /// PUCCH demodulator Format 4 component.
  std::unique_ptr<pucch_demodulator_format4> demodulator_format4;
};

} // namespace ocudu
