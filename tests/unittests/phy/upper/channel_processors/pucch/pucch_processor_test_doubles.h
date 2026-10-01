// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pucch/pucch_processor.h"

namespace ocudu {

class pucch_processor_dummy : public pucch_processor
{
public:
  pucch_processor_result process(const resource_grid_reader& grid, const format0_configuration& config) override
  {
    return {};
  }

  pucch_format1_map<pucch_processor_result> process(const resource_grid_reader&        grid,
                                                    const format1_batch_configuration& config) override
  {
    return {};
  }

  pucch_processor_result process(const resource_grid_reader& grid, const format2_configuration& config) override
  {
    return {};
  }

  pucch_processor_result process(const resource_grid_reader& grid, const format3_configuration& config) override
  {
    return {};
  }

  pucch_processor_result process(const resource_grid_reader& grid, const format4_configuration& config) override
  {
    return {};
  }
};

/// Spy for \ref pucch_processor that tracks whether \c process() was called.
class pucch_processor_spy : public pucch_processor
{
public:
  pucch_processor_result process(const resource_grid_reader& grid, const format0_configuration& config) override
  {
    format0_processed = true;
    return {};
  }

  pucch_format1_map<pucch_processor_result> process(const resource_grid_reader&        grid,
                                                    const format1_batch_configuration& config) override
  {
    format1_processed = true;
    return {};
  }

  pucch_processor_result process(const resource_grid_reader& grid, const format2_configuration& config) override
  {
    format2_processed = true;
    return {};
  }

  pucch_processor_result process(const resource_grid_reader& grid, const format3_configuration& config) override
  {
    format3_processed = true;
    return {};
  }

  pucch_processor_result process(const resource_grid_reader& grid, const format4_configuration& config) override
  {
    format4_processed = true;
    return {};
  }

  bool has_format0_been_called() const { return format0_processed; }
  bool has_format1_been_called() const { return format1_processed; }
  bool has_format2_been_called() const { return format2_processed; }
  bool has_format3_been_called() const { return format3_processed; }
  bool has_format4_been_called() const { return format4_processed; }

  void clear()
  {
    format0_processed = false;
    format1_processed = false;
    format2_processed = false;
    format3_processed = false;
    format4_processed = false;
  }

private:
  bool format0_processed = false;
  bool format1_processed = false;
  bool format2_processed = false;
  bool format3_processed = false;
  bool format4_processed = false;
};

} // namespace ocudu
