// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief PRACH buffer held in CUDA managed memory, so that the host and the GPU share one buffer.

#pragma once

#include "ocudu/cuda/adt/managed_tensor.h"
#include "ocudu/cuda/support/cuda_device.h"
#include "ocudu/phy/support/prach_buffer.h"
#include "ocudu/phy/support/support_factories.h"

namespace ocudu {

/// \brief PRACH buffer whose host and device sides share one managed allocation.
///
/// The buffer receives the PRACH samples written by the host from the fronthaul or by the GPU
/// decompressing them in place, and is read by the detector. One allocation serves both sides, so
/// the samples are read in place rather than copied across.
///
/// As with the resource grid, the buffer holds no ordering state. A producer synchronizes its own
/// work before it hands the buffer on, and a consumer prepares the residency that it needs.
class prach_buffer_cuda_visible_impl : public prach_buffer
{
public:
  /// \brief Creates a buffer sized for the given occasions.
  ///
  /// \param[in] max_nof_ports_        Maximum number of ports.
  /// \param[in] max_nof_td_occasions_ Maximum number of time-domain occasions.
  /// \param[in] max_nof_fd_occasions_ Maximum number of frequency-domain occasions.
  /// \param[in] max_nof_symbols_      Maximum number of symbols per occasion.
  /// \param[in] sequence_length_      Length of one PRACH sequence (in samples).
  /// \param[in] config_               Residency options.
  prach_buffer_cuda_visible_impl(unsigned                                       max_nof_ports_,
                                 unsigned                                       max_nof_td_occasions_,
                                 unsigned                                       max_nof_fd_occasions_,
                                 unsigned                                       max_nof_symbols_,
                                 unsigned                                       sequence_length_,
                                 const cuda_visible_prach_buffer_configuration& config_);

  ~prach_buffer_cuda_visible_impl() override;

  // See interface for documentation.
  unsigned get_max_nof_ports() const override { return max_nof_ports; }

  // See interface for documentation.
  unsigned get_max_nof_td_occasions() const override { return max_nof_td_occasions; }

  // See interface for documentation.
  unsigned get_max_nof_fd_occasions() const override { return max_nof_fd_occasions; }

  // See interface for documentation.
  unsigned get_max_nof_symbols() const override { return max_nof_symbols; }

  // See interface for documentation.
  unsigned get_sequence_length() const override { return sequence_length; }

  // See interface for documentation.
  span<cbf16_t> get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) override;

  // See interface for documentation.
  span<const cbf16_t>
  get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) const override;

  // See interface for documentation.
  span<const cbf16_t> get_buffer() const override;

  // See interface for documentation.
  unsigned
  get_symbol_offset(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) const override;

  /// Returns true if the buffer obtained its managed allocation and can be shared with the device.
  bool is_device_visible() const { return !buffer_tensor.get_data().empty(); }

private:
  /// Dimensions of the buffer: sequence, symbol, frequency occasion, time occasion, and port.
  static constexpr unsigned nof_dimensions = 5;

  using buffer_tensor_type = cuda::managed_tensor<nof_dimensions, cbf16_t>;

  unsigned max_nof_ports;
  unsigned max_nof_td_occasions;
  unsigned max_nof_fd_occasions;
  unsigned max_nof_symbols;
  unsigned sequence_length;

  cuda_visible_prach_buffer_configuration config;
  cuda::cuda_device_properties            device_properties;

  /// PRACH samples, addressable by the host and by the device, indexed by port, occasions, symbol
  /// and sample.
  buffer_tensor_type buffer_tensor;
};

} // namespace ocudu
