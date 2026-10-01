// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ofh/ofh_sector_executor_mapper.h"

namespace ocudu {

class task_executor;

/// \brief Open Fronthaul RU executor mapper interface.
///
/// Provides access to the different sector executor mappers.
class ru_ofh_executor_mapper
{
public:
  /// Default destructor.
  virtual ~ru_ofh_executor_mapper() = default;

  /// \brief Retrieves the Open Fronthaul sector executor mapper for a given sector index.
  ///
  /// \remark An assertion is triggered if the sector index exceeds the number of executor configurations.
  virtual ofh::ofh_sector_executor_mapper& get_sector_mapper(unsigned index) = 0;

  /// Retrieves the  Open Fronthaul sector executor mapper via closed braces operator.
  ofh::ofh_sector_executor_mapper& operator[](unsigned cell_index) { return get_sector_mapper(cell_index); }

  /// \brief Retrieves Open Fronthaul timing executor.
  ///
  /// \remark this executor is common for all sectors.
  virtual task_executor& timing_executor() = 0;
};

} // namespace ocudu
