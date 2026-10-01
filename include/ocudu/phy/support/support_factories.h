// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/generic_functions/precoding/precoding_factories.h"
#include "ocudu/phy/support/interpolator.h"
#include "ocudu/phy/support/prach_buffer.h"
#include "ocudu/phy/support/resource_grid_mapper.h"
#include "ocudu/phy/support/resource_grid_pool.h"
#include "ocudu/support/executors/task_executor.h"
#include <memory>
#include <vector>

namespace ocudu {

/// Factory that builds resource grids.
class resource_grid_factory
{
public:
  /// Default destructor.
  virtual ~resource_grid_factory() = default;

  /// Creates and returns an instance of a resource grid.
  virtual std::unique_ptr<resource_grid> create(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc) = 0;
};

/// \brief Creates and returns a resource grid factory that instantiates resource grids.
///
/// \return A pointer to a resource grid factory.
std::shared_ptr<resource_grid_factory> create_resource_grid_factory();

/// \brief Configuration of a resource grid shared with an NVIDIA GPU.
///
/// The grid lives in one allocation that both the host and the device address. The options below
/// decide where its pages are kept and how much ordering the two sides need, which depends on the
/// part (GPU model): some GPUs let host and device hold the allocation at once, other GPUs do not.
/// (For example, GH200 reports concurrent_managed_access and lets both sides hold the allocation;
/// GB10 and discrete GPUs don't.)
struct cuda_visible_resource_grid_configuration {
  /// Identifier of the CUDA device that shares the grid with the host.
  int device_id = 0;
  /// Set to true to migrate the pages before a consumer touches them rather than on first touch.
  bool enable_prefetch = false;
  /// Set to true to keep the pages in device memory between transmissions.
  bool prefer_device_residency = false;
  /// \brief Set to true to let the host write while a device write is outstanding.
  ///
  /// Sound only where the downlink writers target disjoint resource elements, as the scheduler
  /// arranges for reference signals and shared channel data.
  bool enable_concurrent_downlink_writes = false;
};

/// \brief Configuration of a PRACH buffer shared with an NVIDIA GPU.
struct cuda_visible_prach_buffer_configuration {
  /// Identifier of the CUDA device that shares the buffer with the host.
  int device_id = 0;
  /// \brief Set to true to migrate the pages before a consumer touches them.
  ///
  /// Where the GPU reaches host memory through the host page tables, a prefetch moves no pages and
  /// only adds a stream synchronization per occasion, so it is left off by default and the factory
  /// turns it on for the GPUs that do migrate.
  bool enable_prefetch = false;
};

/// \brief Creates PRACH buffers shared with an NVIDIA GPU.
///
/// \param[in] max_nof_antennas     Maximum number of antennas.
/// \param[in] max_nof_td_occasions Maximum number of time-domain occasions.
/// \param[in] max_nof_fd_occasions Maximum number of frequency-domain occasions.
/// \param[in] is_long_preamble     Set to true for a long preamble, false for a short one.
/// \param[in] config               Residency options.
/// \return A buffer, or nullptr when the build has no CUDA support or the shared allocation could
/// not be obtained, in which case the caller falls back to an ordinary PRACH buffer.
std::unique_ptr<prach_buffer>
create_prach_buffer_cuda_visible(unsigned                                       max_nof_antennas,
                                 unsigned                                       max_nof_td_occasions,
                                 unsigned                                       max_nof_fd_occasions,
                                 bool                                           is_long_preamble,
                                 const cuda_visible_prach_buffer_configuration& config = {});

/// \brief Creates a resource grid factory whose grids are shared with an NVIDIA GPU.
///
/// Each created grid holds one allocation addressable by the host and by the device, so an
/// accelerated block reads and writes it in place instead of copying it across.
///
/// \param[in] fallback_factory Factory of the grids used when a shared allocation is unavailable.
/// \param[in] config           Residency and ordering options.
/// \return A factory or nullptr when the build has no CUDA support or no device is available, in
/// which case the caller uses \c fallback_factory directly.
std::shared_ptr<resource_grid_factory>
create_resource_grid_factory_cuda_visible(std::shared_ptr<resource_grid_factory>          fallback_factory,
                                          const cuda_visible_resource_grid_configuration& config = {});

/// \brief Creates a generic resource grid pool.
///
/// It selects a different resource grid every time a resource grid is requested. The resource grid is repeated every
/// \c grids.size() requests.
///
/// \param[in] grids Resource grids. Ownership is transferred to the pool.
/// \return A generic resource grid pool.
std::unique_ptr<resource_grid_pool>
create_generic_resource_grid_pool(std::vector<std::unique_ptr<resource_grid>> grids);

/// \brief Creates an asynchronous resource grid pool.
///
/// It selects a different resource grid every time a resource grid is requested. The resource grid expires
/// \c expire_timeout_slots after it is requested. When a resource grid expires it is asynchronously set to zero.
///
/// The resource grid repetition is not deterministic but it is guaranteed that it is repeated after
/// \c expire_timeout_slots.
///
/// \param[in] async_executor       Asynchronous task executor for setting the grid to zero.
/// \param[in] grids                Resource grids. Ownership is transferred to the pool.
/// \return An asynchronous resource grid pool.
std::unique_ptr<resource_grid_pool>
create_asynchronous_resource_grid_pool(task_executor&                              async_executor,
                                       std::vector<std::unique_ptr<resource_grid>> grids);

/// Factory that builds resource grid mappers.
class resource_grid_mapper_factory
{
public:
  /// Default destructor.
  virtual ~resource_grid_mapper_factory() = default;

  /// Creates and returns an instance of a resource grid mapper.
  virtual std::unique_ptr<resource_grid_mapper> create() = 0;
};

/// \brief Creates and returns a resource grid mapper factory that instantiates resource grid mappers.
///
/// \param[in] precoder_factory Precoder factory instance.
/// \return A pointer to a resource grid mapper factory.
std::shared_ptr<resource_grid_mapper_factory>
create_resource_grid_mapper_factory(std::shared_ptr<channel_precoder_factory> precoder_factory);

/// \brief Creates a long PRACH sequence buffer.
///
/// Long buffers contain 839-element PRACH sequences for up to 4 OFDM symbols and a given maximum number of
/// frequency-domain occasions.
///
/// \param[in] max_nof_antennas     Maximum number of antennas.
/// \param[in] max_nof_fd_occasions Maximum number of frequency-domain occasions.
/// \return A long preamble sequence buffer.
std::unique_ptr<prach_buffer> create_prach_buffer_long(unsigned max_nof_antennas, unsigned max_nof_fd_occasions);

/// \brief Creates a short PRACH sequence buffer.
///
/// Short buffers contain 139-element PRACH sequences for up to \ref prach_constants::SHORT_SEQUENCE_MAX_NOF_SYMBOLS
/// symbols per occasion.
///
/// \param[in] max_nof_antennas     Maximum number of antennas.
/// \param[in] max_nof_td_occasions Maximum number of time-domain occasions.
/// \param[in] max_nof_fd_occasions Maximum number of frequency-domain occasions.
/// \return A short preamble sequence buffer containing PRACH sequence buffers for the number of selected occasions.
std::unique_ptr<prach_buffer>
create_prach_buffer_short(unsigned max_nof_antennas, unsigned max_nof_td_occasions, unsigned max_nof_fd_occasions);

/// \brief Returns an interpolator.
std::unique_ptr<ocudu::interpolator> create_interpolator();

} // namespace ocudu
