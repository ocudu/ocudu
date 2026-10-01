// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi_adaptor/mac/mac_fapi_sector_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/mac/operation_controller.h"
#include "ocudu/fapi_adaptor/mac/p5/mac_fapi_p5_sector_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/mac/p7/mac_fapi_p7_sector_fastpath_adaptor.h"
#include <memory>

namespace ocudu {
namespace fapi_adaptor {

/// MAC-FAPI bidirectional sector fastpath adaptor implementation.
class mac_fapi_sector_fastpath_adaptor_impl : public mac_fapi_sector_fastpath_adaptor, public operation_controller
{
public:
  /// Constructor for the MAC-FAPI bidirectional sector adaptor.
  mac_fapi_sector_fastpath_adaptor_impl(std::unique_ptr<mac_fapi_p5_sector_fastpath_adaptor> p5_adaptor_,
                                        std::unique_ptr<mac_fapi_p7_sector_fastpath_adaptor> p7_adaptor_);

  // See interface for documentation.
  mac_fapi_p5_sector_fastpath_adaptor& get_p5_sector_fastpath_adaptor() override;

  // See interface for documentation.
  mac_fapi_p7_sector_fastpath_adaptor& get_p7_sector_adaptor() override;

  // See interface for documentation.
  operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override { p7_adaptor->get_operation_controller().start(); }

  // See interface for documentation.
  void stop() override { p7_adaptor->get_operation_controller().stop(); }

private:
  std::unique_ptr<mac_fapi_p5_sector_fastpath_adaptor> p5_adaptor;
  std::unique_ptr<mac_fapi_p7_sector_fastpath_adaptor> p7_adaptor;
};

} // namespace fapi_adaptor
} // namespace ocudu
