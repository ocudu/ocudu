// SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Resource grid held in CUDA managed memory, so that the host and the GPU share one grid.

#pragma once

#include "resource_grid_allocation_info.h"
#include "resource_grid_reader_impl.h"
#include "resource_grid_writer_impl.h"
#include "ocudu/adt/tensor.h"
#include "ocudu/cuda/adt/cuda_stream.h"
#include "ocudu/cuda/adt/managed_tensor.h"
#include "ocudu/cuda/support/cuda_device.h"
#include "ocudu/phy/support/resource_grid.h"
#include "ocudu/phy/support/resource_grid_dimensions.h"
#include "ocudu/phy/support/support_factories.h"
#include <memory>
#include <mutex>

namespace ocudu {

/// \brief Resource grid whose host and device sides share one managed allocation.
///
/// The grid is written by the host, by the GPU, or by both, and read the same way. One allocation
/// serves both sides, so an accelerated block reads and writes the resource elements in place.
///
/// The grid holds no ordering state. A producer synchronizes its own work before it hands the grid
/// on, and a consumer prepares the residency that it needs. See \ref cuda::managed_tensor for the
/// prefetch and advice helpers a consumer uses.
class resource_grid_cuda_visible_impl : public resource_grid
{
public:
  /// \brief Creates a grid of the given dimensions.
  ///
  /// \param[in] nof_ports_   Number of ports.
  /// \param[in] nof_symbols_ Number of OFDM symbols per port.
  /// \param[in] nof_subc_    Number of subcarriers per OFDM symbol.
  /// \param[in] config_      Residency and ordering options.
  resource_grid_cuda_visible_impl(unsigned                                        nof_ports_,
                                  unsigned                                        nof_symbols_,
                                  unsigned                                        nof_subc_,
                                  const cuda_visible_resource_grid_configuration& config_);

  ~resource_grid_cuda_visible_impl() override;

  // See interface for documentation.
  void set_all_zero() override;

  // See interface for documentation.
  resource_grid_writer& get_writer() override;

  // See interface for documentation.
  const resource_grid_reader& get_reader() const override;

  /// Returns true if the grid obtained its managed allocation and can be shared with the device.
  bool is_device_visible() const { return !grid_tensor.get_data().empty(); }

private:
  using grid_tensor_type =
      cuda::managed_tensor<static_cast<unsigned>(resource_grid_dimensions::all), cbf16_t, resource_grid_dimensions>;

  /// \brief Clears the whole grid with a device kernel, which is cheaper than a host memset.
  ///
  /// The clear is synchronised before returning, so that the grid is ready for its next producer.
  bool clear_on_device();

  resource_grid_allocation_info alloc_mask;

  cuda_visible_resource_grid_configuration config;
  cuda::cuda_device_properties             device_properties;

  unsigned nof_ports;
  unsigned nof_symbols;
  unsigned nof_subc;

  /// Resource elements, addressable by the host and by the device, indexed by port, symbol and
  /// subcarrier.
  grid_tensor_type grid_tensor;

  std::unique_ptr<resource_grid_reader_impl> reader;
  std::unique_ptr<resource_grid_writer_impl> writer;

  /// Stream used for grid maintenance such as clearing it rather than for processing.
  cuda::cuda_stream maintenance_stream;

  /// Guards the clear that runs on the maintenance stream.
  mutable std::mutex clear_mutex;
};

} // namespace ocudu
