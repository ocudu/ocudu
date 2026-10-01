// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct srs_estimator_configuration;
struct srs_estimator_result;
class resource_grid_reader;

/// Propagation channel estimator interface based on Sounding Reference Signals (SRS).
class srs_estimator
{
public:
  /// Default destructor.
  virtual ~srs_estimator() = default;

  /// \brief Estimates the SRS propagation channel.
  ///
  /// \param[in] grid   Received resource grid containing the SRS.
  /// \param[in] config Sounding Reference Signal parameters.
  /// \return The propagation channel estimation results.
  virtual srs_estimator_result estimate(const resource_grid_reader&        grid,
                                        const srs_estimator_configuration& config) = 0;
};

} // namespace ocudu
