// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/resource_allocation/rb_interval.h"
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <memory>

namespace ocudu {

/// \brief Encapsulates per-port, per-symbol allocation tracking for a resource grid.
///
/// Stores one atomic 64-bit packed CRB interval per (port, symbol) pair in a fixed-size array of \c
/// max_nof_port_symbol_pairs elements. A non-zero value indicates that at least one resource element at that (port,
/// symbol) position has been written since the last \p reset_all call.
///
/// The 64-bit value packs two \c uint32_t fields into one word: upper 32 bits hold the starting CRB index, lower 32
/// bits hold the stopping CRB index (exclusive). A value of zero represents an empty range.
class resource_grid_allocation_info
{
public:
  /// \brief Constructs an allocation mask.
  ///
  /// Stores entries in a fixed-size array of \c max_nof_port_symbol_pairs elements, indexed by \p i_port * \p
  /// nof_symb + \p i_symbol.
  ///
  /// \param[in] nof_ports  Number of antenna ports.
  /// \param[in] nof_symb   Number of OFDM symbols per slot.
  /// \param[in] nof_crbs   Total number of CRBs used as the default range when \p update_crb_range is called with an
  ///                       empty interval.
  /// \remark An assertion is triggered if the number of ports times the number of symbols exceeds the maximum
  /// number of (port, symbol) pairs that can be tracked.
  resource_grid_allocation_info(unsigned nof_ports_, unsigned nof_symb_, unsigned nof_crbs_) :
    nof_ports(nof_ports_),
    nof_symb(nof_symb_),
    nof_crbs(nof_crbs_),
    crb_ranges(std::make_unique<std::atomic<uint64_t>[]>(nof_ports * nof_symb))
  {
    reset_all();
  }

  /// \brief Gets the current min/max allocated common resource blocks range.
  ///
  /// \param[in] i_port   Port index.
  /// \param[in] i_symbol Symbol index.
  /// \return Allocated CRB interval, or empty if nothing is allocated.
  /// \remark An assertion is triggered if the port index is greater than or equal to the number of ports, or the
  /// symbol index is greater than or equal to the number of symbols.
  crb_interval get_allocation_range(unsigned i_port, unsigned i_symbol) const
  {
    // Acquire semantics: the iteration loop in is_empty()/is_port_empty() reads from these slots without
    // acquiring them directly; this ensures visibility of concurrent writes in the hot-path check.
    return unpack_crb_interval(crb_ranges[get_index(i_port, i_symbol)].load(std::memory_order_acquire));
  }

  /// \brief Checks if a specific (port, symbol) pair is allocated.
  ///
  /// \param[in] i_port   Port index.
  /// \param[in] i_symbol Symbol index.
  /// \return True if the (port, symbol) pair is allocated, false otherwise.
  /// \remark An assertion is triggered if the port index is greater than or equal to the number of ports, or the
  /// symbol index is greater than or equal to the number of symbols.
  bool is_allocated(unsigned i_port, unsigned i_symbol) const { return !is_empty(i_port, i_symbol); }

  /// \brief Checks if a port is empty (all its symbols are zero).
  ///
  /// \param[in] i_port Port index.
  /// \return True if the port is empty, false otherwise.
  /// \remark An assertion is triggered if the port index is greater than or equal to the number of ports.
  bool is_port_empty(unsigned i_port) const
  {
    for (unsigned i_symbol = 0; i_symbol != nof_symb; ++i_symbol) {
      if (is_allocated(i_port, i_symbol)) {
        return false;
      }
    }
    return true;
  }

  /// \brief Checks if the entire resource grid is empty.
  /// \return True if all (port, symbol) pairs are empty, false otherwise.
  bool is_empty() const
  {
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      if (!is_port_empty(i_port)) {
        return false;
      }
    }
    return true;
  }

  /// \brief Checks if a specific (port, symbol) pair is empty.
  ///
  /// \param[in] i_port     Port index.
  /// \param[in] i_symbol   Symbol index.
  /// \return True if the (port, symbol) pair is empty, false otherwise.
  /// \remark An assertion is triggered if the port index is greater than or equal to the number of ports, or the
  /// symbol index is greater than or equal to the number of symbols.
  bool is_empty(unsigned i_port, unsigned i_symbol) const { return get_allocation_range(i_port, i_symbol).empty(); }

  /// \brief Atomically expands the packed min/max CRB range for a (port, symbol) pair.
  ///
  /// The new range is the union of the provided interval and the current stored interval. If \p new_crb_range is empty,
  /// the full span \c [0, \p nof_crbs) is used as a proxy for "everything is allocated." This avoids having to pass
  /// explicit range parameters when the only goal is to mark the position as written.
  ///
  /// \param[in] i_port        Port index.
  /// \param[in] i_symbol      Symbol index.
  /// \param[in] new_crb_range CRB interval to merge, or empty to signal the full span.
  /// \remark An assertion is triggered if the port index is greater than or equal to the number of ports, or the
  ///         symbol index is greater than or equal to the number of symbols.
  void update_crb_range(unsigned i_port, unsigned i_symbol, crb_interval new_crb_range = {})
  {
    if (new_crb_range.empty()) {
      new_crb_range = {0, nof_crbs};
    }

    auto& ref_crb_range = crb_ranges[get_index(i_port, i_symbol)];

    packed_crb_interval current_crb_range = ref_crb_range.load(std::memory_order_relaxed);
    while (!ref_crb_range.compare_exchange_weak(
        current_crb_range,
        pack_crb_interval(combine_crb_ranges(new_crb_range, unpack_crb_interval(current_crb_range))),
        std::memory_order_release,
        std::memory_order_relaxed)) {
    }
  }

  /// \brief Marks all (port, symbol) pairs as deallocated and resets the CRB range.
  ///
  /// Also called by the constructor so that newly-created masks start in a clean state.
  void reset_all()
  {
    for (unsigned i = 0, end = nof_ports * nof_symb; i != end; ++i) {
      crb_ranges[i].store(empty_packed_crb_interval, std::memory_order_release);
    }
  }

private:
  /// \brief Packed 64-bit representation of a CRB interval.
  ///
  /// Upper 32 bits: start index, lower 32 bits: stop index. A value of zero denotes an empty interval.
  using packed_crb_interval = uint64_t;

  /// Default packed CRB interval — value representing an empty interval.
  static constexpr packed_crb_interval empty_packed_crb_interval = 0;

  /// \brief Unpacks a packed CRB interval into a \c crb_interval structure.
  ///
  /// \param[in] packed  Packed 64-bit value.
  /// \return \c crb_interval{start, stop}.
  static crb_interval unpack_crb_interval(packed_crb_interval packed)
  {
    return crb_interval{packed >> 32, packed & 0xffffffff};
  }

  /// \brief Packs a \c crb_interval into a 64-bit word.
  ///
  /// \param[in] unpacked  Interval with start and stop fields.
  /// \return Packed 64-bit value.
  static packed_crb_interval pack_crb_interval(const crb_interval& unpacked)
  {
    return (static_cast<uint64_t>(unpacked.start()) << 32) | unpacked.stop();
  }

  /// \brief Gets the linear index for a given (port, symbol) pair.
  ///
  /// \param[in] i_port     Port index.
  /// \param[in] i_symbol   Symbol index.
  /// \return The linear index as \c i_port * nof_symb + i_symbol.
  /// \remark An assertion is triggered if the port index is greater than or equal to the number of ports, or the
  /// symbol index is greater than or equal to the number of symbols.
  unsigned get_index(unsigned i_port, unsigned i_symbol) const
  {
    ocudu_assert(i_port < nof_ports, "Port index {} is out of range (max {})", i_port, nof_ports - 1);
    ocudu_assert(i_symbol < nof_symb, "Symbol index {} is out of range (max {})", i_symbol, nof_symb - 1);
    return i_port * nof_symb + i_symbol;
  }

  /// \brief Combines two intervals, resulting in the minimum start and maximum stop.
  ///
  /// If either interval is empty the other is returned unchanged.
  ///
  /// Examples:
  /// - combine(\{2, 5\}, \{3, 8\}) → \{2, 8\}  (overlapping intervals merge)
  /// - combine(\{4, 6\}, \{8, 10\}) → \{4, 10\}  (disjoint intervals expand to span both)
  /// - combine(\{2, 5\}, \{\}) → \{2, 5\}  (empty argument returns the non-empty side)
  ///
  /// \param[in] left   Left interval.
  /// \param[in] right  Right interval.
  /// \return The union of the two intervals, or the non-empty one if the other is empty.
  static crb_interval combine_crb_ranges(const crb_interval& left, const crb_interval& right)
  {
    if (left.empty()) {
      return right;
    }

    if (right.empty()) {
      return left;
    }

    return {std::min(left.start(), right.start()), std::max(left.stop(), right.stop())};
  }

  unsigned                                 nof_ports;
  unsigned                                 nof_symb;
  unsigned                                 nof_crbs;
  std::unique_ptr<std::atomic<uint64_t>[]> crb_ranges;
};

} // namespace ocudu
