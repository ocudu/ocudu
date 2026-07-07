// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "resource_grid_allocation_info.h"
#include "ocudu/phy/support/resource_grid_dimensions.h"
#include "ocudu/phy/support/resource_grid_writer.h"

namespace ocudu {

/// Implements the resource grid writer interface.
class resource_grid_writer_impl : public resource_grid_writer
{
public:
  using storage_type = tensor<static_cast<unsigned>(resource_grid_dimensions::all), cbf16_t, resource_grid_dimensions>;

  /// Constructs a resource grid writer implementation from a tensor and allocation mask.
  resource_grid_writer_impl(storage_type& data_, resource_grid_allocation_info& alloc_mask_) :
    data(data_), alloc_mask(alloc_mask_)
  {
  }

  // See interface for documentation.
  unsigned get_nof_ports() const override;

  // See interface for documentation.
  unsigned get_nof_subc() const override;

  // See interface for documentation.
  unsigned get_nof_symbols() const override;

  // See interface for documentation.
  span<const cf_t> put(unsigned                                   port,
                       unsigned                                   l,
                       unsigned                                   k_init,
                       const bounded_bitset<MAX_NOF_SUBCARRIERS>& mask,
                       span<const cf_t>                           symbols) override;

  // See interface for documentation.
  span<const cbf16_t> put(unsigned                                   port,
                          unsigned                                   l,
                          unsigned                                   k_init,
                          const bounded_bitset<MAX_NOF_SUBCARRIERS>& mask,
                          span<const cbf16_t>                        symbols) override;

  // See interface for documentation.
  void put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) override;

  // See interface for documentation.
  void put(unsigned port, unsigned l, unsigned k_init, unsigned stride, span<const cbf16_t> symbols) override;

  // See interface for documentation.
  span<cbf16_t> get_view(unsigned port, unsigned l) override;

private:
  storage_type&                  data;
  resource_grid_allocation_info& alloc_mask;
};

} // namespace ocudu
