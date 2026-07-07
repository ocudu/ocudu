// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "resource_grid_allocation_info.h"
#include "ocudu/phy/support/resource_grid_dimensions.h"
#include "ocudu/phy/support/resource_grid_reader.h"

namespace ocudu {

/// Implements the resource grid reader interface.
class resource_grid_reader_impl : public resource_grid_reader
{
public:
  using storage_type = tensor<static_cast<unsigned>(resource_grid_dimensions::all), cbf16_t, resource_grid_dimensions>;

  /// Constructs a resource grid reader implementation from a tensor and allocation mask.
  resource_grid_reader_impl(const storage_type& data_, const resource_grid_allocation_info& alloc_mask_) :
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
  bool is_empty(unsigned port) const override;

  // See interface for documentation.
  crb_interval get_allocation_range(unsigned port, unsigned l) const override;

  // See interface for documentation.
  bool is_empty() const override;

  // See interface for documentation.
  span<cf_t> get(span<cf_t>                                 symbols,
                 unsigned                                   port,
                 unsigned                                   l,
                 unsigned                                   k_init,
                 const bounded_bitset<MAX_NOF_SUBCARRIERS>& mask) const override;

  // See interface for documentation.
  span<cbf16_t> get(span<cbf16_t>                              symbols,
                    unsigned                                   port,
                    unsigned                                   l,
                    unsigned                                   k_init,
                    const bounded_bitset<MAX_NOF_SUBCARRIERS>& mask) const override;

  // See interface for documentation.
  void get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, unsigned stride) const override;

  // See interface for documentation.
  void get(span<cbf16_t> symbols, unsigned port, unsigned l, unsigned k_init) const override;

  // See interface for documentation.
  span<const cbf16_t> get_view(unsigned port, unsigned l) const override;

private:
  const storage_type&                  data;
  const resource_grid_allocation_info& alloc_mask;
};

} // namespace ocudu
