// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief PER encoding of whole numbers, lengths, enums, lists, extension groups and REAL values.

#pragma once

#include "ocudu/adt/expected.h"
#include "ocudu/asn1/asn1_containers.h"
#include "ocudu/support/ocudu_assert.h"
#include <limits>

namespace asn1 {

/// 16K, the unit of fragmented PER lengths.
constexpr int ASN_16K = 16384;
/// 64K, the upper bound from which PER encodes lengths as unconstrained.
constexpr int ASN_64K = 65536;

/************************
     enum packing
************************/

OCUDUASN_CODE pack_enum(bit_ref& bref, uint32_t enum_val, uint32_t nbits);
OCUDUASN_CODE pack_enum(bit_ref& bref, uint32_t enum_val, uint32_t nbits, uint32_t nof_noext);
OCUDUASN_CODE pack_enum(bit_ref& bref, uint32_t e, uint32_t nof_types, uint32_t nof_exts, bool has_ext);
/// Unpacks the index of an ENUMERATED value.
ocudu::expected<uint32_t, OCUDUASN_CODE>
unpack_enum(uint32_t nof_types, uint32_t nof_exts, bool has_ext, cbit_ref& bref);
template <typename EnumType>
OCUDUASN_CODE pack_enum(bit_ref& bref, EnumType e)
{
  return pack_enum(bref, e, EnumType::nof_types, EnumType::nof_exts, EnumType::has_ext);
}
template <typename EnumType>
OCUDUASN_CODE unpack_enum(EnumType& e, cbit_ref& bref)
{
  auto ret = unpack_enum(EnumType::nof_types, EnumType::nof_exts, EnumType::has_ext, bref);
  if (not ret.has_value()) {
    return ret.error();
  }
  e = static_cast<typename EnumType::options>(*ret);
  return OCUDUASN_SUCCESS;
}

/************************
     PER encoding
************************/

/* X.691 - Section 10.5 - Constrained Whole Number */
template <class IntType>
OCUDUASN_CODE pack_constrained_whole_number(bit_ref& bref, IntType n, IntType lb, IntType ub, bool aligned);
template <class IntType>
OCUDUASN_CODE unpack_constrained_whole_number(IntType& n, cbit_ref& bref, IntType lb, IntType ub, bool aligned);

/* X.691 - Section 10.6 - Normally small non-negative whole Number */
template <typename UintType>
OCUDUASN_CODE pack_norm_small_non_neg_whole_number(bit_ref& bref, UintType n);
template <typename UintType>
OCUDUASN_CODE unpack_norm_small_non_neg_whole_number(UintType& n, cbit_ref& bref);

/* X.691 - Section 10.8 - Unconstrained Whole Number */
template <typename IntType>
OCUDUASN_CODE pack_unconstrained_whole_number(bit_ref& bref, IntType n, bool aligned);
template <typename IntType>
OCUDUASN_CODE unpack_unconstrained_whole_number(IntType& n, cbit_ref& bref, uint32_t len, bool aligned);

/************************
   length determinant
************************/

// Pack as whole constrained number
template <typename IntType>
OCUDUASN_CODE pack_length(bit_ref& bref, IntType n, IntType lb, IntType ub, bool aligned = false);
template <typename IntType>
OCUDUASN_CODE unpack_length(IntType& n, cbit_ref& bref, IntType lb, IntType ub, bool aligned = false);

// Pack as a small non-negative whole number
OCUDUASN_CODE pack_length(bit_ref& ref, uint32_t val, bool aligned = false);
OCUDUASN_CODE unpack_length(uint32_t& val, cbit_ref& ref, bool aligned = false);

/************************
        Integer
************************/

template <typename IntType>
OCUDUASN_CODE pack_integer(bit_ref& bref,
                           IntType  n,
                           IntType  lb      = std::numeric_limits<IntType>::min(),
                           IntType  ub      = std::numeric_limits<IntType>::max(),
                           bool     has_ext = false,
                           bool     aligned = false);
template <typename IntType>
OCUDUASN_CODE unpack_integer(IntType&  n,
                             cbit_ref& bref,
                             IntType   lb      = std::numeric_limits<IntType>::min(),
                             IntType   ub      = std::numeric_limits<IntType>::max(),
                             bool      has_ext = false,
                             bool      aligned = false);
// unconstrained case
template <typename IntType>
OCUDUASN_CODE pack_unconstrained_integer(bit_ref& bref, IntType n, bool has_ext = false, bool aligned = false);
template <typename IntType>
OCUDUASN_CODE unpack_unconstrained_integer(IntType& n, cbit_ref& bref, bool has_ext = false, bool aligned = false);

template <class IntType>
struct integer_packer {
  integer_packer(IntType lb_, IntType ub_, bool has_ext_ = false, bool aligned_ = false);
  OCUDUASN_CODE pack(bit_ref& bref, IntType n) const;
  OCUDUASN_CODE unpack(IntType& n, cbit_ref& bref) const;
  IntType       lb;
  IntType       ub;
  bool          has_ext;
  bool          aligned;
};

/************************
  Bit packer
************************/

struct BitPacker {
  BitPacker(uint32_t nof_bits_) : nof_bits(nof_bits_) {}
  template <typename T>
  OCUDUASN_CODE pack(bit_ref& bref, const T& topack) const
  {
    return bref.pack(topack, nof_bits);
  }
  template <typename T>
  OCUDUASN_CODE unpack(T& tounpack, cbit_ref& bref) const
  {
    return bref.unpack(tounpack, nof_bits);
  }
  uint32_t nof_bits;
};

/*********************
  fixed sequence of
*********************/

// packers/unpackers for fixed_length sequence-of
template <class ArrayType, class ItemPacker>
OCUDUASN_CODE pack_fixed_seq_of(bit_ref& bref, const ArrayType& item_array, uint32_t nof_items, ItemPacker packer)
{
  for (uint32_t i = 0; i < nof_items; ++i) {
    HANDLE_CODE(packer.pack(bref, item_array[i]));
  }
  return OCUDUASN_SUCCESS;
}
template <class ArrayType>
OCUDUASN_CODE pack_fixed_seq_of(bit_ref& bref, const ArrayType& item_array, uint32_t nof_items)
{
  for (uint32_t i = 0; i < nof_items; ++i) {
    // asn1::pack, found by argument-dependent lookup, also packs types without pack() member.
    HANDLE_CODE(pack(bref, item_array[i]));
  }
  return OCUDUASN_SUCCESS;
}
template <class ArrayType, class ItemUnpacker>
OCUDUASN_CODE unpack_fixed_seq_of(ArrayType& item_array, cbit_ref& bref, uint32_t nof_items, ItemUnpacker unpacker)
{
  for (uint32_t i = 0; i < nof_items; ++i) {
    HANDLE_CODE(unpacker.unpack(item_array[i], bref));
  }
  return OCUDUASN_SUCCESS;
}
template <class ArrayType>
OCUDUASN_CODE unpack_fixed_seq_of(ArrayType& item_array, cbit_ref& bref, uint32_t nof_items)
{
  for (uint32_t i = 0; i < nof_items; ++i) {
    HANDLE_CODE(unpack(item_array[i], bref));
  }
  return OCUDUASN_SUCCESS;
}

/*********************
   dyn sequence of
*********************/

namespace detail {

using pack_item_fn   = OCUDUASN_CODE (*)(bit_ref& bref, const void* item, const void* packer);
using unpack_item_fn = OCUDUASN_CODE (*)(void* item, cbit_ref& bref, const void* packer);
using resize_seq_fn  = void* (*)(void* seqof, uint32_t nof_items);

/// Packs the length and the items of a SEQUENCE OF stored contiguously.
OCUDUASN_CODE pack_dyn_seq_of_items(bit_ref&     bref,
                                    const void*  items,
                                    uint32_t     nof_items,
                                    uint32_t     item_size,
                                    uint32_t     lb,
                                    uint32_t     ub,
                                    bool         aligned,
                                    pack_item_fn pack_item,
                                    const void*  packer);

/// Unpacks the length and the items of a SEQUENCE OF stored contiguously.
OCUDUASN_CODE unpack_dyn_seq_of_items(void*          seqof,
                                      resize_seq_fn  resize,
                                      cbit_ref&      bref,
                                      uint32_t       item_size,
                                      uint32_t       lb,
                                      uint32_t       ub,
                                      bool           aligned,
                                      unpack_item_fn unpack_item,
                                      const void*    unpacker);

template <class ArrayType>
void* resize_seq_of(void* seqof, uint32_t nof_items)
{
  auto& arr = *static_cast<ArrayType*>(seqof);
  arr.resize(nof_items);
  return arr.data();
}

} // namespace detail

template <class ArrayType, class ItemPacker>
OCUDUASN_CODE pack_dyn_seq_of(bit_ref&         bref,
                              const ArrayType& seqof,
                              uint32_t         lb,
                              uint32_t         ub,
                              ItemPacker       packer,
                              bool             aligned = false)
{
  using item_type = typename ArrayType::value_type;
  return detail::pack_dyn_seq_of_items(
      bref,
      seqof.data(),
      seqof.size(),
      sizeof(item_type),
      lb,
      ub,
      aligned,
      [](bit_ref& b, const void* item, const void* p) {
        return static_cast<const ItemPacker*>(p)->pack(b, *static_cast<const item_type*>(item));
      },
      &packer);
}

template <class ArrayType>
OCUDUASN_CODE pack_dyn_seq_of(bit_ref& bref, const ArrayType& seqof, uint32_t lb, uint32_t ub, bool aligned = false)
{
  using item_type = typename ArrayType::value_type;
  return detail::pack_dyn_seq_of_items(
      bref,
      seqof.data(),
      seqof.size(),
      sizeof(item_type),
      lb,
      ub,
      aligned,
      // asn1::pack, found by argument-dependent lookup, also packs types without pack() member.
      [](bit_ref& b, const void* item, const void*) { return pack(b, *static_cast<const item_type*>(item)); },
      nullptr);
}

template <class ArrayType, class ItemUnpacker>
OCUDUASN_CODE unpack_dyn_seq_of(ArrayType&   seqof,
                                cbit_ref&    bref,
                                uint32_t     lb,
                                uint32_t     ub,
                                ItemUnpacker unpacker,
                                bool         aligned = false)
{
  using item_type = typename ArrayType::value_type;
  return detail::unpack_dyn_seq_of_items(
      &seqof,
      &detail::resize_seq_of<ArrayType>,
      bref,
      sizeof(item_type),
      lb,
      ub,
      aligned,
      [](void* item, cbit_ref& b, const void* p) {
        return static_cast<const ItemUnpacker*>(p)->unpack(*static_cast<item_type*>(item), b);
      },
      &unpacker);
}

template <class ArrayType>
OCUDUASN_CODE unpack_dyn_seq_of(ArrayType& seqof, cbit_ref& bref, uint32_t lb, uint32_t ub, bool aligned = false)
{
  using item_type = typename ArrayType::value_type;
  return detail::unpack_dyn_seq_of_items(
      &seqof,
      &detail::resize_seq_of<ArrayType>,
      bref,
      sizeof(item_type),
      lb,
      ub,
      aligned,
      [](void* item, cbit_ref& b, const void*) { return unpack(*static_cast<item_type*>(item), b); },
      nullptr);
}

/*********************
      ext group
*********************/

/// \brief Used to decode extension groups.
class ext_groups_unpacker
{
public:
  ext_groups_unpacker(cbit_ref& bref_, bool aligned_ = false);

  /// Unpacks presence flags and extension groups.
  [[nodiscard]] OCUDUASN_CODE unpack_next_group();

  /// Retrieves a bit_ref representing the last unpacked extension group.
  /// \return Returns false if there was no extension group left to unpack.
  [[nodiscard]] bool get_last_group_range(cbit_ref& bref)
  {
    ocudu_sanity_check(next_group_idx > 0, "No group has been unpacked yet");
    if (state != state_t::done and groups[next_group_idx - 1]) {
      bref = group_bref;
      return true;
    }
    return false;
  }

  /// Skips all remaining extension groups and sets the position of bit_ref.
  [[nodiscard]] OCUDUASN_CODE consume_remaining_groups(cbit_ref& bref);

private:
  enum class state_t : uint8_t { unpack_presence_flags, unpack_groups, done };

  // Decodes only the presence flags of the extension groups.
  OCUDUASN_CODE unpack_presence_flags();

  const bool      aligned;
  state_t         state               = state_t::unpack_presence_flags;
  uint32_t        nof_unpacked_groups = 0;
  uint32_t        next_group_idx      = 0;
  cbit_ref        outer_bref;
  cbit_ref        group_bref;
  ext_array<bool> groups;
};

/*********************
   Var Length Field
*********************/

namespace detail {

/// Pads the contents of a variable length field to whole octets and packs them after their length determinant.
OCUDUASN_CODE end_varlength_field(bit_ref& bref, bit_ref& contents, const ocudu::byte_buffer& buffer, bool aligned);

} // namespace detail

/// Packs a variable length field, such as an open type, whose contents pack_contents(bit_ref&) writes.
template <class PackContents>
OCUDUASN_CODE pack_varlength_field(bit_ref& bref, bool aligned, PackContents&& pack_contents)
{
  ocudu::byte_buffer buffer;
  bit_ref            contents(buffer);
  if (OCUDUASN_CODE ret = pack_contents(contents); ret != OCUDUASN_SUCCESS) {
    return ret;
  }
  return detail::end_varlength_field(bref, contents, buffer, aligned);
}

/// Unpacks a variable length field, such as an open type, whose contents unpack_contents(cbit_ref&) reads.
template <class UnpackContents>
OCUDUASN_CODE unpack_varlength_field(cbit_ref& bref, bool aligned, UnpackContents&& unpack_contents)
{
  uint32_t len = 0;
  HANDLE_CODE(unpack_length(len, bref, aligned));
  cbit_ref contents = bref.subview(0, len);
  if (OCUDUASN_CODE ret = unpack_contents(contents); ret != OCUDUASN_SUCCESS) {
    return ret;
  }
  // The padding bits of the contents are skipped.
  return bref.advance_bytes(len);
}

/****************
 REAL type
***************/

OCUDUASN_CODE pack_unconstrained_real(bit_ref& bref, float n, bool aligned = false);

OCUDUASN_CODE unpack_unconstrained_real(float& n, cbit_ref& bref, bool aligned = false);

} // namespace asn1
