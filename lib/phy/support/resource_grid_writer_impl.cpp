// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "resource_grid_writer_impl.h"
#include "ocudu/adt/interval.h"
#include "ocudu/ocuduvec/conversion.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/phy/support/resource_grid.h"
#include "ocudu/support/math/math_utils.h"

using namespace ocudu;

/// Converts a subcarrier range \c [start_subc, stop_subc) into the CRB range that fully contains it.
static crb_interval subc_range_to_crb_range(unsigned start_subc, unsigned stop_subc)
{
  return {start_subc / NOF_SUBCARRIERS_PER_RB, divide_ceil(stop_subc, NOF_SUBCARRIERS_PER_RB)};
}

unsigned resource_grid_writer_impl::get_nof_ports() const
{
  return data.get_dimension_size(resource_grid_dimensions::port);
}

unsigned resource_grid_writer_impl::get_nof_subc() const
{
  return data.get_dimension_size(resource_grid_dimensions::subc);
}

unsigned resource_grid_writer_impl::get_nof_symbols() const
{
  return data.get_dimension_size(resource_grid_dimensions::symbol);
}

span<const cf_t> resource_grid_writer_impl::put(unsigned                                   port,
                                                unsigned                                   l,
                                                unsigned                                   k_init,
                                                const bounded_bitset<MAX_NOF_SUBCARRIERS>& mask,
                                                span<const cf_t>                           symbols)
{
  interval<unsigned, false> l_range(0, get_nof_symbols());
  interval<unsigned, true>  mask_size_range(1, get_nof_subc());
  interval<unsigned, false> port_range(0, get_nof_ports());
  ocudu_assert(l_range.contains(l), "Symbol index (i.e., {}) is out of the range {}.", l, l_range);
  ocudu_assert(
      mask_size_range.contains(mask.size()), "Mask size (i.e., {}) is out of range {}.", mask.size(), mask_size_range);
  ocudu_assert(port_range.contains(port), "Port identifier (i.e., {}) is out of range {}.", port, port_range);

  // Get view of the OFDM symbol subcarriers.
  span<cbf16_t> symb = data.get_view({l, port}).subspan(k_init, mask.size());

  unsigned mask_count = mask.count();
  if (mask_count != 0) {
    alloc_mask.update_crb_range(
        port, l, subc_range_to_crb_range(k_init + mask.find_lowest(), k_init + mask.find_highest() + 1));
  }

  ocudu_assert(mask_count <= symbols.size(),
               "The number of active subcarriers (i.e., {}) exceeds the number of symbols (i.e., {}).",
               mask_count,
               symbols.size());

  // Do a straight copy if the elements of the mask are all contiguous.
  if (mask_count and mask.is_contiguous()) {
    ocuduvec::convert(symb.subspan(mask.find_lowest(), mask_count), symbols.first(mask_count));
    return symbols.last(symbols.size() - mask_count);
  }

  mask.for_each(0, mask.size(), [&symb, &symbols](unsigned i_subc) {
    symb[i_subc] = to_cbf16(symbols.front());
    symbols      = symbols.last(symbols.size() - 1);
  });

  return symbols;
}

span<const cbf16_t> resource_grid_writer_impl::put(unsigned                                   port,
                                                   unsigned                                   l,
                                                   unsigned                                   k_init,
                                                   const bounded_bitset<MAX_NOF_SUBCARRIERS>& mask,
                                                   span<const cbf16_t>                        symbols)
{
  interval<unsigned, false> l_range(0, get_nof_symbols());
  interval<unsigned, true>  mask_size_range(1, get_nof_subc());
  interval<unsigned, false> port_range(0, get_nof_ports());
  ocudu_assert(l_range.contains(l), "Symbol index (i.e., {}) is out of the range {}.", l_range, l_range);
  ocudu_assert(
      mask_size_range.contains(mask.size()), "Mask size (i.e., {}) is out of range {}.", mask.size(), mask_size_range);
  ocudu_assert(port_range.contains(port), "Port identifier (i.e., {}) is out of range {}.", port, port_range);

  // Get view of the OFDM symbol subcarriers.
  span<cbf16_t> symb = data.get_view({l, port}).subspan(k_init, mask.size());

  unsigned mask_count = mask.count();
  if (mask_count != 0) {
    alloc_mask.update_crb_range(
        port, l, subc_range_to_crb_range(k_init + mask.find_lowest(), k_init + mask.find_highest() + 1));
  }

  ocudu_assert(mask_count <= symbols.size(),
               "The number of active subcarriers (i.e., {}) exceeds the number of symbols (i.e., {}).",
               mask_count,
               symbols.size());

  // Do a straight copy if the elements of the mask are all contiguous.
  if (mask_count and mask.is_contiguous()) {
    ocuduvec::copy(symb.subspan(mask.find_lowest(), mask_count), symbols.first(mask_count));
    return symbols.last(symbols.size() - mask_count);
  }

  mask.for_each(0, mask.size(), [&symb, &symbols](unsigned i_subc) {
    symb[i_subc] = symbols.front();
    symbols      = symbols.last(symbols.size() - 1);
  });

  return symbols;
}

void resource_grid_writer_impl::put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols)
{
  ocudu_assert(k_init + symbols.size() <= get_nof_subc(),
               "The initial subcarrier index (i.e., {}) plus the number of symbols (i.e., {}) exceeds the maximum "
               "number of subcarriers (i.e., {})",
               k_init,
               symbols.size(),
               get_nof_subc());
  ocudu_assert(l < get_nof_symbols(),
               "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})",
               l,
               get_nof_symbols());
  ocudu_assert(port < get_nof_ports(),
               "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})",
               port,
               get_nof_ports());

  // Select destination OFDM symbol from the resource grid.
  span<cbf16_t> rg_symbol = data.get_view({l, port});

  // Copy resource elements.
  ocuduvec::convert(rg_symbol.subspan(k_init, symbols.size()), symbols);
  if (!symbols.empty()) {
    alloc_mask.update_crb_range(port, l, subc_range_to_crb_range(k_init, k_init + symbols.size()));
  }
}

void resource_grid_writer_impl::put(unsigned            port,
                                    unsigned            l,
                                    unsigned            k_init,
                                    unsigned            stride,
                                    span<const cbf16_t> symbols)
{
  unsigned nof_symbols = symbols.size();
  ocudu_assert(k_init + ((nof_symbols - 1) * stride) < get_nof_subc(),
               "The initial subcarrier index (i.e., {}) plus the number of RE (i.e., {}) exceeds the maximum number of "
               "subcarriers (i.e., {})",
               k_init,
               (((nof_symbols - 1) * stride) + 1),
               get_nof_subc());
  ocudu_assert(l < get_nof_symbols(),
               "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})",
               l,
               get_nof_symbols());
  ocudu_assert(port < get_nof_ports(),
               "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})",
               port,
               get_nof_ports());

  // Insert symbols.
  span<cbf16_t> rg_symbol = data.get_view({l, port});
  for (unsigned i_symbol = 0, i_re = k_init; i_symbol != nof_symbols; ++i_symbol) {
    rg_symbol[i_re] = symbols[i_symbol];
    i_re += stride;
  }

  if (nof_symbols != 0) {
    unsigned last_subc = k_init + ((nof_symbols - 1) * stride);
    alloc_mask.update_crb_range(port, l, subc_range_to_crb_range(k_init, last_subc + 1));
  }
}

span<cbf16_t> resource_grid_writer_impl::get_view(unsigned port, unsigned l)
{
  ocudu_assert(l < get_nof_symbols(),
               "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})",
               l,
               get_nof_symbols());
  ocudu_assert(port < get_nof_ports(),
               "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})",
               port,
               get_nof_ports());
  alloc_mask.update_crb_range(port, l);
  return data.get_view({l, port});
}
