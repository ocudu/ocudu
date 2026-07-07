// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "resource_grid_impl.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;

resource_grid_impl::resource_grid_impl(unsigned nof_ports_, unsigned nof_symb_, unsigned nof_subc_) :
  nof_ports(nof_ports_),
  nof_symb(nof_symb_),
  nof_subc(nof_subc_),
  alloc_mask(nof_ports_, nof_symb_, divide_ceil(nof_subc, NOF_SUBCARRIERS_PER_RB)),
  writer(rg_buffer, alloc_mask),
  reader(rg_buffer, alloc_mask)
{
  // Reserve memory for the internal buffer.
  rg_buffer.reserve({nof_subc, nof_symb, nof_ports});

  // Set all the resource elements to zero.
  ocuduvec::zero(rg_buffer.get_data());
}

void resource_grid_impl::set_all_zero()
{
  // Zero data for all allocated port-symbol pairs and clear the mask.
  for (unsigned port = 0; port != nof_ports; ++port) {
    for (unsigned l = 0; l != nof_symb; ++l) {
      if (alloc_mask.is_allocated(port, l)) {
        ocuduvec::zero(rg_buffer.get_view({l, port}));
      }
    }
  }
  alloc_mask.reset_all();
}

resource_grid_writer& resource_grid_impl::get_writer()
{
  return writer;
}

const resource_grid_reader& resource_grid_impl::get_reader() const
{
  return reader;
}
