// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/phy/support/resource_grid_base.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"
#include "ocudu/ran/resource_block.h"

namespace ocudu {

/// Describes a resource grid reader interface.
class resource_grid_reader : public resource_grid_base
{
public:
  /// Default destructor.
  virtual ~resource_grid_reader() = default;

  /// \brief Determines whether a given port is empty.
  ///
  /// A port is considered empty if no \c put method have been called since last zero set.
  ///
  /// If zeros are written using any \c put method, the port is considered not empty.
  ///
  /// An invalid port index triggers an assertion.
  ///
  /// \param[in] port Port index.
  /// \return True if the given port is empty. Otherwise, false.
  virtual bool is_empty(unsigned port) const = 0;

  /// \brief Determines whether the resource grid is empty.
  ///
  /// The grid is considered empty if no \c put method have been called since last zero set.
  ///
  /// If zeros are written using any \c put method, the grid is considered not empty.
  ///
  /// \return \c true if the resource grid is empty. Otherwise, \c false.
  virtual bool is_empty() const = 0;

  /// \brief Gets the allocated common resource block range for a given OFDM symbol at a given port.
  ///
  /// The range spans the minimum and maximum CRB written by a \c put method since the last zero set. An empty
  /// interval indicates that the given (port, symbol) pair is empty.
  ///
  /// \param[in] port   Port index.
  /// \param[in] symbol OFDM symbol index.
  /// \return The allocated CRB range, or an empty interval if the given symbol is empty.
  virtual crb_interval get_allocation_range(unsigned port, unsigned symbol) const = 0;

  /// \brief Gets a number of resource elements in the resource grid at the given port and symbol using a bounded bitset
  /// to indicate which subcarriers are allocated and which are not.
  ///
  /// \param[out] symbols Destination symbol buffer.
  /// \param[in]  port    Port index.
  /// \param[in]  l       OFDM symbol index.
  /// \param[in]  k_init  Initial subcarrier index.
  /// \param[in]  mask    Boolean mask denoting the subcarriers to be read (if \c true), starting from \c k_init.
  /// \return A view to the unused entries of \c symbols.
  /// \note The initial subcarrier plus the number of elements of \c mask shall not exceed the number of resource grid
  ///       subcarriers.
  /// \note The number of elements of \c symbols shall be equal to or greater than the number of true elements in
  ///       \c mask.
  virtual span<cf_t> get(span<cf_t>                                 symbols,
                         unsigned                                   port,
                         unsigned                                   l,
                         unsigned                                   k_init,
                         const bounded_bitset<MAX_NOF_SUBCARRIERS>& mask) const = 0;

  /// \brief Gets a number of resource elements in the resource grid at the given port and symbol using a bounded bitset
  /// to indicate which subcarriers are allocated and which are not.
  ///
  /// \param[out] symbols Destination symbol buffer.
  /// \param[in]  port    Port index.
  /// \param[in]  l       OFDM symbol index.
  /// \param[in]  k_init  Initial subcarrier index.
  /// \param[in] mask     Boolean mask denoting the subcarriers to be read (if \c true), starting from \c k_init.
  /// \return A view to the unused entries of \c symbols.
  /// \note The initial subcarrier plus the number of elements of \c mask shall not exceed the number of resource grid
  ///       subcarriers.
  /// \note The number of elements of \c symbols shall be equal to or greater than the number of true elements in
  ///       \c mask.
  virtual span<cbf16_t> get(span<cbf16_t>                              symbols,
                            unsigned                                   port,
                            unsigned                                   l,
                            unsigned                                   k_init,
                            const bounded_bitset<MAX_NOF_SUBCARRIERS>& mask) const = 0;

  /// \brief Gets a number of resource elements for a given port and symbol \c l starting at \c k_init and picks the
  /// first element every \c stride.
  ///
  /// \param[out] symbols Destination symbol buffer.
  /// \param[in]  port    Port index.
  /// \param[in]  l       OFDM symbol index.
  /// \param[in]  k_init  Initial subcarrier index.
  /// \param[in]  stride  Distance between the elements to get.
  /// \note The sum of \c k_init and the number of elements in \c symbols shall not exceed the resource grid number of
  /// subcarriers.
  virtual void get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, unsigned stride = 1) const = 0;

  /// \brief Gets a consecutive number of resource elements for a given port and OFDM symbol \c l starting at
  /// subcarrier \c k_init.
  ///
  /// \param[out] symbols Destination symbol buffer.
  /// \param[in]  port    Port index.
  /// \param[in]  l       OFDM symbol index.
  /// \param[in]  k_init  Initial subcarrier index.
  /// \note The sum of \c k_init and the number of elements in \c symbols shall not exceed the resource grid number of
  /// subcarriers.
  virtual void get(span<cbf16_t> symbols, unsigned port, unsigned l, unsigned k_init) const = 0;

  /// \brief Gets a view of all resource elements for a given port and symbol \c l.
  ///
  /// \param[in]  port    Port index.
  /// \param[in]  l       OFDM symbol index.
  virtual span<const cbf16_t> get_view(unsigned port, unsigned l) const = 0;
};

} // namespace ocudu
