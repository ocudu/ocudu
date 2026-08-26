// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/phy/upper/upper_phy_execution_configuration.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/support/executors/task_executor.h"
#include <chrono>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

namespace ocudu {

class executor_metrics_channel_registry;

namespace odu {

/// \brief DU low executor mapper interface.
///
/// Provides access to the different cell executor mappers.
class du_low_executor_mapper
{
public:
  /// Default destructor.
  virtual ~du_low_executor_mapper() = default;

  /// Retrieves the upper physical layer execution configuration.
  virtual const upper_phy_execution_configuration& get_upper_phy_execution_config() const = 0;
};

/// \brief Collects the task executor mapper for using a single task executor.
struct du_low_executor_mapper_single_exec_config {
  /// Common executor for all the DU low cell tasks.
  upper_phy_executor common_executor;
};

/// \brief DU low executor mapper manual configuration.
///
/// The configuration consists of a set of executors and parameters for limiting the number of threads used for certain
/// channels.
///
/// The provided executors contain a reference to a task executor and their maximum concurrency. The executor maximum
/// concurrency given by each executor is used for creating pools of physical layer instances. These pools ensure that
/// a number of instances are available for processing channels simultaneously.
struct du_low_executor_mapper_flexible_exec_config {
  /// Real-time high priority executor.
  upper_phy_executor rt_hi_prio_exec;
  /// Non-real-time high priority executor.
  upper_phy_executor non_rt_hi_prio_exec;
  /// Non-real-time medium priority executor.
  upper_phy_executor non_rt_medium_prio_exec;
  /// Non-real-time low priority executor.
  upper_phy_executor non_rt_low_prio_exec;
  /// \brief Maximum PUCCH processing concurrency.
  ///
  /// The Physical Uplink Control Channel (PUCCH) is processed using the high priority executor. This parameter limits
  /// the maximum number of threads that can simultaneously process PUCCH.
  ///
  /// No limit in concurrency is imposed if this parameter is zero, greater than or equal to the high priority executor
  /// maximum concurrency.
  unsigned max_pucch_concurrency = 0;
  /// \brief Maximum joint the PUSCH and SRS processing concurrency.
  ///
  /// The Physical Uplink Shared Channel (PUSCH) and Sounding Reference Signals (SRS) are processed using the medium
  /// priority executor. This parameter limits the maximum number of threads that can simultaneously process PUSCH and
  /// SRS.
  ///
  /// No limit in concurrency is imposed if this parameter is zero, greater than or equal to the medium priority
  /// executor maximum concurrency.
  ///
  /// This parameter is necessary when hardware acceleration is used for decoding PUSCH. This parameter must not exceed
  /// the capacity of the accelerator.
  unsigned max_pusch_and_srs_concurrency = 1;
  /// \brief Maximum concurrency level for PDSCH processing.
  ///
  /// Maximum number of threads that can concurrently process Physical Downlink Shared Channel (PDSCH). Set to zero for
  /// no limitation.
  ///
  /// This parameter is necessary when hardware acceleration is used for encoding PDSCH. This parameter must not exceed
  /// the capacity of the accelerator.
  unsigned max_pdsch_concurrency = 0;
};

/// Variant of the DU low executor mapping configuration.
using du_low_executor_mapper_exec_config =
    std::variant<du_low_executor_mapper_single_exec_config, du_low_executor_mapper_flexible_exec_config>;

/// Configuration of DU-low executor mapper.
struct du_low_executor_mapper_config {
  /// Common executor configuration.
  du_low_executor_mapper_exec_config executors;
  /// \brief Optional executor metrics channel registry.
  ///
  /// If it is initialized, the executor mapper wraps the executors with metric decorators.
  executor_metrics_channel_registry* exec_metrics_channel_registry = nullptr;
  /// Enable task tracing.
  bool executor_tracing_enable = false;
};

/// \brief Creates an executor mapper for the DU-low.
std::unique_ptr<du_low_executor_mapper> create_du_low_executor_mapper(const du_low_executor_mapper_config& config);

} // namespace odu
} // namespace ocudu
