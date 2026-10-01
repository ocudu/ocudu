// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/support/support_factories.h"
#include "interpolator/interpolator_linear_impl.h"
#include "prach_buffer_impl.h"
#include "resource_grid_impl.h"
#include "resource_grid_pool_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/antenna_ports.h"
#include "ocudu/phy/generic_functions/precoding/precoding_factories.h"
#include "ocudu/ran/prach/prach_constants.h"
#include <utility>

using namespace ocudu;

namespace {

/// Factory that builds resource grid objects.
class resource_grid_factory_impl : public resource_grid_factory
{
public:
  /// \brief Creates a resource grid instance for a number of ports, symbols and subcarriers.
  /// \param[in] nof_ports Number of ports.
  /// \param[in] nof_symb  Number of OFDM symbols.
  /// \param[in] nof_subc  Number of subcarriers.
  /// \return A resource grid object.
  std::unique_ptr<resource_grid> create(unsigned nof_ports, unsigned nof_symb, unsigned nof_subc) override
  {
    return std::make_unique<resource_grid_impl>(nof_ports, nof_symb, nof_subc);
  }
};

} // namespace

std::unique_ptr<resource_grid_pool>
ocudu::create_generic_resource_grid_pool(std::vector<std::unique_ptr<resource_grid>> grids)
{
  std::vector<resource_grid_pool_wrapper> rg_wrappers;
  std::for_each(grids.begin(), grids.end(), [&rg_wrappers](std::unique_ptr<resource_grid>& grid) {
    rg_wrappers.emplace_back(std::move(grid), nullptr);
  });

  return std::make_unique<resource_grid_pool_impl>(std::move(rg_wrappers));
}

std::unique_ptr<resource_grid_pool>
ocudu::create_asynchronous_resource_grid_pool(task_executor&                              async_executor,
                                              std::vector<std::unique_ptr<resource_grid>> grids)
{
  std::vector<resource_grid_pool_wrapper> rg_wrappers;
  std::for_each(grids.begin(),
                grids.end(),
                [&rg_wrappers, async_executor_ptr = &async_executor](std::unique_ptr<resource_grid>& grid) {
                  rg_wrappers.emplace_back(std::move(grid), async_executor_ptr);
                });

  return std::make_unique<resource_grid_pool_impl>(std::move(rg_wrappers));
}

namespace {

/// Factory that builds resource grid mapper objects.
class resource_grid_mapper_factory_impl : public resource_grid_mapper_factory
{
public:
  resource_grid_mapper_factory_impl(std::shared_ptr<channel_precoder_factory> precoder_factory_) :
    precoder_factory(std::move(precoder_factory_))
  {
    ocudu_assert(precoder_factory, "Invalid precoder factory.");
  }

  /// \brief Creates a resource grid mapper instance.
  /// \return A resource grid mapper object.
  std::unique_ptr<resource_grid_mapper> create() override
  {
    return std::make_unique<resource_grid_mapper_impl>(precoder_factory->create());
  }

private:
  std::shared_ptr<channel_precoder_factory> precoder_factory;
};

} // namespace

std::shared_ptr<resource_grid_mapper_factory>
ocudu::create_resource_grid_mapper_factory(std::shared_ptr<channel_precoder_factory> precoder_factory)
{
  return std::make_shared<resource_grid_mapper_factory_impl>(precoder_factory);
}

std::unique_ptr<prach_buffer> ocudu::create_prach_buffer_long(unsigned max_nof_antennas, unsigned max_nof_fd_occasions)
{
  static constexpr interval<unsigned, true> nof_rx_ports_range(1, MAX_PORTS);
  static constexpr interval<unsigned, true> max_nof_fd_prach_occasions_range(
      1, prach_constants::MAX_NOF_PRACH_FD_OCCASIONS);
  ocudu_assert(nof_rx_ports_range.contains(max_nof_antennas),
               "The maximum number of antennas (i.e., {}) is out of range {}·",
               max_nof_antennas,
               nof_rx_ports_range);
  ocudu_assert(max_nof_fd_prach_occasions_range.contains(max_nof_fd_occasions),
               "The maximum number of frequency domain occasions (i.e., {}) is out of range {}·",
               max_nof_fd_occasions,
               max_nof_fd_prach_occasions_range);
  return std::make_unique<prach_buffer_impl>(max_nof_antennas,
                                             1,
                                             max_nof_fd_occasions,
                                             prach_constants::LONG_SEQUENCE_MAX_NOF_SYMBOLS,
                                             prach_constants::LONG_SEQUENCE_LENGTH);
}

std::unique_ptr<prach_buffer> ocudu::create_prach_buffer_short(unsigned max_nof_antennas,
                                                               unsigned max_nof_td_occasions,
                                                               unsigned max_nof_fd_occasions)
{
  static constexpr interval<unsigned, true> nof_rx_ports_range(1, MAX_PORTS);
  static constexpr interval<unsigned, true> max_nof_td_prach_occasions_range(
      1, prach_constants::MAX_NOF_PRACH_TD_OCCASIONS);
  static constexpr interval<unsigned, true> max_nof_fd_prach_occasions_range(
      1, prach_constants::MAX_NOF_PRACH_FD_OCCASIONS);
  ocudu_assert(nof_rx_ports_range.contains(max_nof_antennas),
               "The maximum number of antennas (i.e., {}) is out of range {}·",
               max_nof_antennas,
               nof_rx_ports_range);
  ocudu_assert(max_nof_td_prach_occasions_range.contains(max_nof_td_occasions),
               "The maximum number of time domain occasions (i.e., {}) is out of range {}·",
               max_nof_td_occasions,
               max_nof_td_prach_occasions_range);
  ocudu_assert(max_nof_fd_prach_occasions_range.contains(max_nof_fd_occasions),
               "The maximum number of frequency domain occasions (i.e., {}) is out of range {}·",
               max_nof_fd_occasions,
               max_nof_fd_prach_occasions_range);
  return std::make_unique<prach_buffer_impl>(max_nof_antennas,
                                             max_nof_td_occasions,
                                             max_nof_fd_occasions,
                                             prach_constants::SHORT_SEQUENCE_MAX_NOF_SYMBOLS,
                                             prach_constants::SHORT_SEQUENCE_LENGTH);
}

std::unique_ptr<ocudu::interpolator> ocudu::create_interpolator()
{
  return std::make_unique<interpolator_linear_impl>();
}

std::shared_ptr<resource_grid_factory> ocudu::create_resource_grid_factory()
{
  return std::make_shared<resource_grid_factory_impl>();
}
