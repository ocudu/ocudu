// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {

class task_executor;

namespace ofh {

/// Interface used to access the different executors used in an Open Fronthaul sector.
class ofh_sector_executor_mapper
{
public:
  /// Default destructor.
  virtual ~ofh_sector_executor_mapper() = default;

  /// Retrieves the executor for Ethernet messages transmission and reception.
  virtual task_executor& get_txrx_executor() const = 0;

  /// Retrieves the downlink User-Plane processing executor for the requested eAxC.
  virtual task_executor& get_dl_up_executor(unsigned eaxc) const = 0;

  /// Retrieves the downlink Control-Plane processing executor for the requested eAxC.
  virtual task_executor& get_dl_cp_executor(unsigned eaxc) const = 0;

  /// Retrieves the uplink Control-Plane processing executor.
  virtual task_executor& get_ul_cp_executor() const = 0;

  /// Retrieves the uplink processing executor.
  virtual task_executor& get_uplink_executor() const = 0;
};

} // namespace ofh
} // namespace ocudu
