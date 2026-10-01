// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "doa_estimator_eigen.h"
#include "ocudu/phy/upper/signal_processors/srs/doa_estimator_configuration.h"
#include "ocudu/phy/upper/signal_processors/srs/doa_estimator_factory.h"

using namespace ocudu;

namespace {

/// Factory class for the Eigen-based DOA estimator.
class doa_estimator_eigen_factory : public doa_estimator_factory
{
public:
  doa_estimator_eigen_factory(const doa_estimator_configuration& config_) : config(config_)
  {
    report_fatal_error_if_not(config.nof_antennas > 1,
                              "Invalid number of antennas {}: DOA estimation requires at least 2 antennas.",
                              config.nof_antennas);
    report_fatal_error_if_not(
        !config.cross_polarized || (config.nof_antennas % 2 == 0),
        "For cross-polarized antenna arrays, the number of antenna elements should be even - provided {}.",
        config.nof_antennas);
    report_fatal_error_if_not(config.antenna_distance_over_wavelength > 0,
                              "Negative antenna distance {}.",
                              config.antenna_distance_over_wavelength);
  }

  std::unique_ptr<doa_estimator> create() override { return std::make_unique<doa_estimator_eigen>(config); }

private:
  doa_estimator_configuration config;
};

} // namespace

std::shared_ptr<doa_estimator_factory> ocudu::create_doa_estimator_factory(const doa_estimator_configuration& config)
{
  return std::make_shared<doa_estimator_eigen_factory>(config);
}
