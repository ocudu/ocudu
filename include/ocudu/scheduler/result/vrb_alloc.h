// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/resource_allocation/rb_bitmap.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"
#include "ocudu/scheduler/result/resource_block_group.h"
#include <variant>

namespace ocudu {

/// VRB Resource Allocation that can be of allocation type 0 (RBGs) or 1 (VRB range).
struct vrb_alloc {
  /// Creates an empty interval.
  vrb_alloc() = default;

  /// \brief Creates a RB grant of allocation type1, i.e. a contiguous range of RBs.
  vrb_alloc(const vrb_interval& other) noexcept : alloc(other) {}

  /// \brief Creates a RB grant of allocation type0, i.e. a set of potentially non-contiguous RBGs.
  vrb_alloc(const rbg_bitmap& other) noexcept : alloc(other) {}

  vrb_alloc& operator=(const vrb_interval& vrbs)
  {
    alloc = vrbs;
    return *this;
  }
  vrb_alloc& operator=(const rbg_bitmap& rbgs)
  {
    alloc = rbgs;
    return *this;
  }

  /// Checks whether the grant is of type0.
  bool is_type0() const { return std::holds_alternative<rbg_bitmap>(alloc); }

  /// Checks whether the grant is of type1.
  bool is_type1() const { return not is_type0(); }

  /// \brief Extracts the RBG bitmap of the allocation, in the case it is of type0. This function fails if allocation
  /// is of type1.
  const rbg_bitmap& type0() const
  {
    ocudu_assert(is_type0(), "Access to type0() for prb_grant with allocation type 1 is invalid");
    return std::get<rbg_bitmap>(alloc);
  }
  rbg_bitmap& type0()
  {
    ocudu_assert(is_type0(), "Access to type0() for prb_grant with allocation type 1 is invalid");
    return std::get<rbg_bitmap>(alloc);
  }

  /// \brief Gets the VRB interval of the resource allocation, in case it is of type1. This function fails if
  /// allocation is of type0.
  const vrb_interval& type1() const
  {
    ocudu_assert(is_type1(), "Access to type1() for prb_grant with allocation type 0 is invalid");
    return std::get<vrb_interval>(alloc);
  }
  vrb_interval& type1()
  {
    ocudu_assert(is_type1(), "Access to type1() of prb_grant with allocation type 0 is invalid");
    return std::get<vrb_interval>(alloc);
  }

  /// \brief Verifies if grant is not empty in terms of RBs.
  bool any() const { return is_type0() ? type0().any() : not type1().empty(); }
  bool empty() const { return is_type0() ? not type0().any() : type1().empty(); }

  bool operator==(const vrb_alloc& other) const { return alloc == other.alloc; }
  bool operator!=(const vrb_alloc& other) const { return not(*this == other); }

private:
  std::variant<vrb_interval, rbg_bitmap> alloc;
};

/// Converts RBG bitmap to PRB bitmap given a BWP PRB dimensions and the nominal RBG-size.
/// \remark See TS 38.214, Sections 5.1.2.2.1 and 6.1.2.2.1.
prb_bitmap convert_rbgs_to_prbs(const rbg_bitmap& rbgs, crb_interval bwp_rbs, nominal_rbg_size P);

} // namespace ocudu

namespace fmt {

template <>
struct formatter<ocudu::vrb_alloc> : public formatter<ocudu::rbg_bitmap> {
  template <typename FormatContext>
  auto format(const ocudu::vrb_alloc& grant, FormatContext& ctx) const
  {
    if (grant.is_type0()) {
      if (this->mode == hexadecimal) {
        return format_to(ctx.out(), "{:x}", grant.type0());
      }
      return format_to(ctx.out(), "{}", grant.type0());
    }
    return format_to(ctx.out(), "{}", grant.type1());
  }
};

} // namespace fmt
