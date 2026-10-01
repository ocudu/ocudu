// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi_adaptor/mac/mac_fapi_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/mac/mac_fapi_sector_fastpath_adaptor.h"
#include <memory>
#include <vector>

namespace ocudu {
namespace fapi_adaptor {

/// MAC-FAPI fastpath adaptor implementation.
class mac_fapi_fastpath_adaptor_impl : public mac_fapi_fastpath_adaptor
{
public:
  explicit mac_fapi_fastpath_adaptor_impl(
      std::vector<std::unique_ptr<mac_fapi_sector_fastpath_adaptor>> sector_adaptors_);

  // See interface for documentation.
  mac_fapi_sector_fastpath_adaptor& get_sector_adaptor(unsigned cell_id) override;

  // See interface for documentation.
  void stop() override;

private:
  std::vector<std::unique_ptr<mac_fapi_sector_fastpath_adaptor>> sector_adaptors;
};

} // namespace fapi_adaptor
} // namespace ocudu
