// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/upper/signal_processors/srs/doa_estimator_factory.h"

using namespace ocudu;

namespace {

/// \brief Dummy factory class for DOA estimator.
///
/// The constructor is called if DOA features are configured but DOA dependencies are not installed, causing a fatal
/// error.
class doa_estimator_dummy_factory : public doa_estimator_factory
{
public:
  doa_estimator_dummy_factory(const doa_estimator_configuration& /* unused */)
  {
    report_fatal_error("Install the Eigen library <https://libeigen.gitlab.io> to enable DOA estimation.");
  }

  std::unique_ptr<doa_estimator> create() override { return nullptr; }
};

} // namespace

std::shared_ptr<doa_estimator_factory> ocudu::create_doa_estimator_factory(const doa_estimator_configuration& config)
{
  return std::make_shared<doa_estimator_dummy_factory>(config);
}
