// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief PER encoding of enums, whole numbers, lengths, integers, sequences-of, extension groups, open types and REAL.

#include "ocudu/asn1/asn1_per.h"
#include "asn1_internal.h"
#include "ocudu/support/math/bit_ops.h"
#include <algorithm>
#include <cmath>

namespace asn1 {

using detail::bit_width;
using detail::ceil_log2;

/************************
      sequence-of
************************/

OCUDUASN_CODE detail::pack_dyn_seq_of_items(bit_ref&     bref,
                                            const void*  items,
                                            uint32_t     nof_items,
                                            uint32_t     item_size,
                                            uint32_t     lb,
                                            uint32_t     ub,
                                            bool         aligned,
                                            pack_item_fn pack_item,
                                            void*        packer)
{
  HANDLE_CODE(pack_length(bref, nof_items, lb, ub, aligned));
  const auto* item = static_cast<const uint8_t*>(items);
  for (uint32_t i = 0; i != nof_items; ++i, item += item_size) {
    HANDLE_CODE(pack_item(bref, item, packer));
  }
  return OCUDUASN_SUCCESS;
}

OCUDUASN_CODE detail::unpack_dyn_seq_of_items(void*          seqof,
                                              resize_seq_fn  resize,
                                              cbit_ref&      bref,
                                              uint32_t       item_size,
                                              uint32_t       lb,
                                              uint32_t       ub,
                                              bool           aligned,
                                              unpack_item_fn unpack_item,
                                              void*          unpacker)
{
  uint32_t nof_items;
  HANDLE_CODE(unpack_length(nof_items, bref, lb, ub, aligned));
  auto* item = static_cast<uint8_t*>(resize(seqof, nof_items));
  for (uint32_t i = 0; i != nof_items; ++i, item += item_size) {
    HANDLE_CODE(unpack_item(item, bref, unpacker));
  }
  return OCUDUASN_SUCCESS;
}

/************************
      enum packing
************************/

// Pack without extension
OCUDUASN_CODE pack_enum(bit_ref& bref, uint32_t enum_val, uint32_t nbits)
{
  return bref.pack(enum_val, nbits);
}

// Pack with extension
OCUDUASN_CODE pack_enum(bit_ref& bref, uint32_t enum_val, uint32_t nbits, uint32_t nof_noext)
{
  bool ext = enum_val >= nof_noext;
  HANDLE_CODE(bref.pack((bool)ext, 1));
  OCUDUASN_CODE ret;
  if (not ext) {
    ret = bref.pack(enum_val, nbits);
  } else {
    ret = pack_norm_small_non_neg_whole_number(bref, enum_val - nof_noext);
  }
  return ret;
}

OCUDUASN_CODE pack_enum(bit_ref& bref, uint32_t e, uint32_t nof_types, uint32_t nof_exts, bool has_ext)
{
  if (e >= nof_types) {
    log_error(
        "The provided enum is not within the range of possible values ({}>={})", (unsigned)e, (unsigned)nof_types);
    return OCUDUASN_ERROR_ENCODE_FAIL;
  }
  OCUDUASN_CODE ret;
  if (has_ext) {
    uint32_t nof_bits = ceil_log2(nof_types - nof_exts);
    ret               = pack_enum(bref, e, nof_bits, nof_types - nof_exts);
  } else {
    uint32_t nof_bits = ceil_log2(nof_types);
    ret               = pack_enum(bref, e, nof_bits);
  }
  return ret;
}

ValOrError unpack_enum(uint32_t nof_types, uint32_t nof_exts, bool has_ext, cbit_ref& bref)
{
  ValOrError ret;
  if (has_ext) {
    uint32_t nof_bits = ceil_log2(nof_types - nof_exts);
    bool     ext;
    ret.code = bref.unpack(ext, 1);
    if (ret.code != OCUDUASN_SUCCESS) {
      return ret;
    }
    if (not ext) {
      ret.code = bref.unpack(ret.val, nof_bits);
    } else {
      ret.code = unpack_norm_small_non_neg_whole_number(ret.val, bref);
      ret.val += nof_types - nof_exts;
    }
  } else {
    uint32_t nof_bits = ceil_log2(nof_types);
    ret.code          = bref.unpack(ret.val, nof_bits);
  }
  if (ret.val >= nof_types) {
    log_error("The provided enum is not within the range of possible values ({}>={})",
              (unsigned)ret.val,
              (unsigned)nof_types);
    ret.code = OCUDUASN_ERROR_DECODE_FAIL;
  }
  return ret;
}

/************************
     PER encoding
************************/

/**
 * X.691 - Section 10.5
 * Encoder function for a constrained whole number
 * @tparam IntType type of integer (e.g. uint8_t, uint32_t, etc.)
 * @param bref bit_ref where to encode integer
 * @param n integer current value
 * @param lb lower bound
 * @param ub upper bound
 * @return success or failure
 */
template <class IntType>
OCUDUASN_CODE pack_constrained_whole_number(bit_ref& bref, IntType n, IntType lb, IntType ub, bool aligned)
{
  if (ub < lb or n < lb or n > ub) {
    log_error("The condition lb <= n <= ub ({} <= {} <= {}) was not met", (long)lb, (long)n, (long)ub);
    return OCUDUASN_ERROR_ENCODE_FAIL;
  }
  // Unsigned arithmetic, since the range of a 64-bit type does not fit in it.
  uint64_t max_offset = (uint64_t)ub - (uint64_t)lb;
  if (max_offset == 0) {
    return OCUDUASN_SUCCESS;
  }
  uint32_t n_bits   = bit_width(max_offset);
  uint64_t toencode = (uint64_t)n - (uint64_t)lb;
  if (not aligned) {
    // UNALIGNED variant
    return bref.pack(toencode, n_bits);
  }
  // ALIGNED variant
  if (max_offset < 255) {
    return bref.pack(toencode, n_bits);
  }
  if (max_offset < ASN_64K) {
    // both one and two octets cases
    uint32_t n_octets = ceil_frac(n_bits, 8u);
    HANDLE_CODE(bref.align_bytes_zero());
    HANDLE_CODE(bref.pack(toencode, n_octets * 8));
    return bref.align_bytes_zero();
  }
  // Octet count as a constrained whole number in 1..max octets, followed by the minimum octets.
  uint32_t max_octets = ceil_frac(n_bits, 8u);
  uint32_t n_octets   = std::max(ceil_frac(bit_width(toencode), 8u), 1u);
  HANDLE_CODE(bref.pack(n_octets - 1, bit_width(max_octets - 1)));
  HANDLE_CODE(bref.align_bytes_zero());
  return bref.pack(toencode, n_octets * 8u);
}
template OCUDUASN_CODE
pack_constrained_whole_number<int8_t>(bit_ref& bref, int8_t n, int8_t lb, int8_t ub, bool aligned);
template OCUDUASN_CODE
pack_constrained_whole_number<int16_t>(bit_ref& bref, int16_t n, int16_t lb, int16_t ub, bool aligned);
template OCUDUASN_CODE
pack_constrained_whole_number<int32_t>(bit_ref& bref, int32_t n, int32_t lb, int32_t ub, bool aligned);
template OCUDUASN_CODE
pack_constrained_whole_number<int64_t>(bit_ref& bref, int64_t n, int64_t lb, int64_t ub, bool aligned);
template OCUDUASN_CODE
pack_constrained_whole_number<uint8_t>(bit_ref& bref, uint8_t n, uint8_t lb, uint8_t ub, bool aligned);
template OCUDUASN_CODE
pack_constrained_whole_number<uint16_t>(bit_ref& bref, uint16_t n, uint16_t lb, uint16_t ub, bool aligned);
template OCUDUASN_CODE
pack_constrained_whole_number<uint32_t>(bit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub, bool aligned);
template OCUDUASN_CODE
pack_constrained_whole_number<uint64_t>(bit_ref& bref, uint64_t n, uint64_t lb, uint64_t ub, bool aligned);

/**
 * X.691 - Section 10.5
 * Decoder function for a constrained whole number
 * @tparam IntType type of integer (e.g. uint8_t, uint32_t, etc.)
 * @param n integer with decoded value
 * @param bref bit_ref where to encode integer
 * @param lb lower bound
 * @param ub upper bound
 * @return success or failure
 */
template <class IntType>
OCUDUASN_CODE unpack_constrained_whole_number(IntType& n, cbit_ref& bref, IntType lb, IntType ub, bool aligned)
{
  if (ub < lb) {
    log_error("The condition lb <= ub ({} <= {}) was not met", (long)lb, (long)ub);
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
  // Unsigned arithmetic, since the range of a 64-bit type does not fit in it.
  uint64_t max_offset = (uint64_t)ub - (uint64_t)lb;
  uint64_t offset     = 0;
  uint32_t n_bits     = bit_width(max_offset);
  if (max_offset == 0) {
    // Single value, nothing to decode.
  } else if (not aligned or max_offset < 255) {
    HANDLE_CODE(bref.unpack(offset, n_bits));
  } else if (max_offset < ASN_64K) {
    HANDLE_CODE(bref.align_bytes());
    HANDLE_CODE(bref.unpack(offset, ceil_frac(n_bits, 8u) * 8));
    HANDLE_CODE(bref.align_bytes());
  } else {
    uint32_t max_octets = ceil_frac(n_bits, 8u);
    uint32_t n_octets;
    HANDLE_CODE(bref.unpack(n_octets, bit_width(max_octets - 1)));
    n_octets += 1;
    if (n_octets > max_octets) {
      log_error("Integer of {} octets exceeds the maximum of {} octets", n_octets, max_octets);
      return OCUDUASN_ERROR_DECODE_FAIL;
    }
    HANDLE_CODE(bref.align_bytes());
    HANDLE_CODE(bref.unpack(offset, n_octets * 8));
  }
  if (offset > max_offset) {
    log_error(
        "The condition lb <= n <= ub ({} <= {} <= {}) was not met", (long)lb, (long)((uint64_t)lb + offset), (long)ub);
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
  n = (IntType)((uint64_t)lb + offset);
  return OCUDUASN_SUCCESS;
}
template OCUDUASN_CODE
unpack_constrained_whole_number<int8_t>(int8_t& n, cbit_ref& bref, int8_t lb, int8_t ub, bool aligned);
template OCUDUASN_CODE
unpack_constrained_whole_number<int16_t>(int16_t& n, cbit_ref& bref, int16_t lb, int16_t ub, bool aligned);
template OCUDUASN_CODE
unpack_constrained_whole_number<int32_t>(int32_t& n, cbit_ref& bref, int32_t lb, int32_t ub, bool aligned);
template OCUDUASN_CODE
unpack_constrained_whole_number<int64_t>(int64_t& n, cbit_ref& bref, int64_t lb, int64_t ub, bool aligned);
template OCUDUASN_CODE
unpack_constrained_whole_number<uint8_t>(uint8_t& n, cbit_ref& bref, uint8_t lb, uint8_t ub, bool aligned);
template OCUDUASN_CODE
unpack_constrained_whole_number<uint16_t>(uint16_t& n, cbit_ref& bref, uint16_t lb, uint16_t ub, bool aligned);
template OCUDUASN_CODE
unpack_constrained_whole_number<uint32_t>(uint32_t& n, cbit_ref& bref, uint32_t lb, uint32_t ub, bool aligned);
template OCUDUASN_CODE
unpack_constrained_whole_number<uint64_t>(uint64_t& n, cbit_ref& bref, uint64_t lb, uint64_t ub, bool aligned);

/**
 * X.691 - Section 10.6
 * Encoder function for a normally small non-negative whole number
 * @tparam IntType type of integer (e.g. uint8_t, uint32_t, etc.)
 * @param bref bit_ref where to encode integer
 * @param n integer current value
 * @return success or failure
 */
template <typename UintType>
OCUDUASN_CODE pack_norm_small_non_neg_whole_number(bit_ref& bref, UintType n)
{
  if (n <= 63) {
    HANDLE_CODE(bref.pack(n, 7)); // [1 bit: 0 | 6 bit: n]
  } else {
    HANDLE_CODE(bref.pack(1, 1));
    log_error("Long small integers not supported");
    return OCUDUASN_ERROR_ENCODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}
template <typename UintType>
OCUDUASN_CODE unpack_norm_small_non_neg_whole_number(UintType& n, cbit_ref& bref)
{
  bool          ext;
  OCUDUASN_CODE ret = bref.unpack(ext, 1);
  HANDLE_CODE(ret);
  if (not ext) {
    ret = bref.unpack(n, 6);
  } else {
    log_error("Long small integers not supported");
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
  return ret;
}
template OCUDUASN_CODE pack_norm_small_non_neg_whole_number<uint8_t>(bit_ref& bref, uint8_t n);
template OCUDUASN_CODE pack_norm_small_non_neg_whole_number<uint16_t>(bit_ref& bref, uint16_t n);
template OCUDUASN_CODE pack_norm_small_non_neg_whole_number<uint32_t>(bit_ref& bref, uint32_t n);
template OCUDUASN_CODE pack_norm_small_non_neg_whole_number<uint64_t>(bit_ref& bref, uint64_t n);
template OCUDUASN_CODE unpack_norm_small_non_neg_whole_number<uint8_t>(uint8_t& n, cbit_ref& bref);
template OCUDUASN_CODE unpack_norm_small_non_neg_whole_number<uint16_t>(uint16_t& n, cbit_ref& bref);
template OCUDUASN_CODE unpack_norm_small_non_neg_whole_number<uint32_t>(uint32_t& n, cbit_ref& bref);
template OCUDUASN_CODE unpack_norm_small_non_neg_whole_number<uint64_t>(uint64_t& n, cbit_ref& bref);

/// Number of octets of the minimal two's-complement encoding of an unconstrained whole number (X.691 - Section 10.8,
/// X.690 - Section 8.3). A non-negative value needs a leading zero bit, so e.g. 128 takes two octets, not one.
template <typename IntType>
uint32_t unconstrained_whole_number_length(IntType n)
{
  uint32_t len = 1;
  if constexpr (std::is_signed<IntType>::value) {
    auto val = static_cast<int64_t>(n);
    while (val > 127 or val < -128) {
      val >>= 8;
      ++len;
    }
  } else {
    auto val = static_cast<uint64_t>(n);
    while (val > 127) {
      val >>= 8;
      ++len;
    }
  }
  return len;
}

/**
 * X.691 - Section 10.8
 * Encoder function for an unconstrained whole number
 * @tparam IntType type of integer (e.g. int8_t, uint32_t, etc.)
 * @param bref bit_ref where to encode integer
 * @param n integer current value
 * @return success or failure
 */
template <typename IntType>
OCUDUASN_CODE pack_unconstrained_whole_number(bit_ref& bref, IntType n, bool aligned)
{
  uint32_t len = unconstrained_whole_number_length(n);
  if (aligned) {
    HANDLE_CODE(bref.align_bytes_zero());
  }
  // Two's-complement content octets, most significant first. The conversion to uint64_t yields the 64-bit two's-
  // complement pattern of the value, so shifting out the low octets works for both signed and unsigned types.
  auto bits = static_cast<uint64_t>(n);
  for (uint32_t i = len; i != 0; --i) {
    uint32_t shift = 8U * (i - 1);
    // A non-negative value wider than 63 bits needs a 9th octet, which is the leading zero octet.
    uint64_t octet = shift < 64U ? (bits >> shift) & 0xffU : 0U;
    HANDLE_CODE(bref.pack(octet, 8));
  }

  return OCUDUASN_SUCCESS;
}
template <typename IntType>
OCUDUASN_CODE unpack_unconstrained_whole_number(IntType& n, cbit_ref& bref, uint32_t len, bool aligned)
{
  if (aligned) {
    HANDLE_CODE(bref.align_bytes());
  }
  if (len == 0) {
    log_error("Unconstrained whole number with no content octets");
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
  uint64_t bits     = 0;
  bool     negative = false;
  bool     fits     = true;
  for (uint32_t i = 0; i != len; ++i) {
    uint8_t octet = 0;
    HANDLE_CODE(bref.unpack(octet, 8));
    if (i == 0) {
      // The encoding is negative if the top bit of the first content octet is set.
      negative = (octet & 0x80U) != 0;
      fits     = not negative or std::is_signed<IntType>::value;
    }
    if (len - i > sizeof(uint64_t)) {
      // Leading octet that is dropped. It only carries sign extension if it repeats the sign bit.
      fits = fits and octet == (negative ? 0xffU : 0x00U);
      continue;
    }
    bits = (bits << 8U) | octet;
  }
  if (negative and len < sizeof(uint64_t)) {
    // Sign extension of the two's-complement encoding.
    bits |= ~static_cast<uint64_t>(0) << (8U * len);
  } else if (std::is_signed<IntType>::value and len >= sizeof(uint64_t)) {
    // Octets were dropped from the front, so the retained bits must carry the sign of the encoding themselves.
    fits = fits and ((bits >> 63U) != 0) == negative;
  }
  n = static_cast<IntType>(bits);
  if (not fits or static_cast<uint64_t>(n) != bits) {
    log_error("Unconstrained whole number of {} octets does not fit in the destination type", len);
    return OCUDUASN_ERROR_DECODE_FAIL;
  }

  return OCUDUASN_SUCCESS;
}
template OCUDUASN_CODE pack_unconstrained_whole_number<int8_t>(bit_ref& bref, int8_t n, bool aligned);
template OCUDUASN_CODE pack_unconstrained_whole_number<int16_t>(bit_ref& bref, int16_t n, bool aligned);
template OCUDUASN_CODE pack_unconstrained_whole_number<int32_t>(bit_ref& bref, int32_t n, bool aligned);
template OCUDUASN_CODE pack_unconstrained_whole_number<int64_t>(bit_ref& bref, int64_t n, bool aligned);
template OCUDUASN_CODE unpack_unconstrained_whole_number<int8_t>(int8_t& n, cbit_ref& bref, uint32_t len, bool aligned);
template OCUDUASN_CODE
unpack_unconstrained_whole_number<int16_t>(int16_t& n, cbit_ref& bref, uint32_t len, bool aligned);
template OCUDUASN_CODE
unpack_unconstrained_whole_number<int32_t>(int32_t& n, cbit_ref& bref, uint32_t len, bool aligned);
template OCUDUASN_CODE
unpack_unconstrained_whole_number<int64_t>(int64_t& n, cbit_ref& bref, uint32_t len, bool aligned);
template OCUDUASN_CODE pack_unconstrained_whole_number<uint8_t>(bit_ref& bref, uint8_t n, bool aligned);
template OCUDUASN_CODE pack_unconstrained_whole_number<uint16_t>(bit_ref& bref, uint16_t n, bool aligned);
template OCUDUASN_CODE pack_unconstrained_whole_number<uint32_t>(bit_ref& bref, uint32_t n, bool aligned);
template OCUDUASN_CODE pack_unconstrained_whole_number<uint64_t>(bit_ref& bref, uint64_t n, bool aligned);
template OCUDUASN_CODE
unpack_unconstrained_whole_number<uint8_t>(uint8_t& n, cbit_ref& bref, uint32_t len, bool aligned);
template OCUDUASN_CODE
unpack_unconstrained_whole_number<uint16_t>(uint16_t& n, cbit_ref& bref, uint32_t len, bool aligned);
template OCUDUASN_CODE
unpack_unconstrained_whole_number<uint32_t>(uint32_t& n, cbit_ref& bref, uint32_t len, bool aligned);
template OCUDUASN_CODE
unpack_unconstrained_whole_number<uint64_t>(uint64_t& n, cbit_ref& bref, uint32_t len, bool aligned);

/*********************
   varlength_packing
*********************/

template <typename IntType>
OCUDUASN_CODE pack_length(bit_ref& bref, IntType n, IntType lb, IntType ub, bool aligned)
{
  if (ub >= ASN_64K) {
    return pack_length(bref, n, aligned);
  }
  return pack_constrained_whole_number(bref, n, lb, ub, aligned);
}
template OCUDUASN_CODE pack_length<uint8_t>(bit_ref& bref, uint8_t n, uint8_t lb, uint8_t ub, bool aligned);
template OCUDUASN_CODE pack_length<uint16_t>(bit_ref& bref, uint16_t n, uint16_t lb, uint16_t ub, bool aligned);
template OCUDUASN_CODE pack_length<uint32_t>(bit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub, bool aligned);
template OCUDUASN_CODE pack_length<uint64_t>(bit_ref& bref, uint64_t n, uint64_t lb, uint64_t ub, bool aligned);
template OCUDUASN_CODE pack_length<int8_t>(bit_ref& bref, int8_t n, int8_t lb, int8_t ub, bool aligned);
template OCUDUASN_CODE pack_length<int16_t>(bit_ref& bref, int16_t n, int16_t lb, int16_t ub, bool aligned);
template OCUDUASN_CODE pack_length<int32_t>(bit_ref& bref, int32_t n, int32_t lb, int32_t ub, bool aligned);
template OCUDUASN_CODE pack_length<int64_t>(bit_ref& bref, int64_t n, int64_t lb, int64_t ub, bool aligned);

template <typename IntType>
OCUDUASN_CODE unpack_length(IntType& n, cbit_ref& bref, IntType lb, IntType ub, bool aligned)
{
  if (ub >= ASN_64K) {
    uint32_t      len;
    OCUDUASN_CODE ret = unpack_length(len, bref, aligned);
    n                 = len;
    return ret;
  }
  return unpack_constrained_whole_number(n, bref, lb, ub, aligned);
}
template OCUDUASN_CODE unpack_length<uint8_t>(uint8_t& n, cbit_ref& bref, uint8_t lb, uint8_t ub, bool aligned);
template OCUDUASN_CODE unpack_length<uint16_t>(uint16_t& n, cbit_ref& bref, uint16_t lb, uint16_t ub, bool aligned);
template OCUDUASN_CODE unpack_length<uint32_t>(uint32_t& n, cbit_ref& bref, uint32_t lb, uint32_t ub, bool aligned);
template OCUDUASN_CODE unpack_length<uint64_t>(uint64_t& n, cbit_ref& bref, uint64_t lb, uint64_t ub, bool aligned);
template OCUDUASN_CODE unpack_length<int8_t>(int8_t& n, cbit_ref& bref, int8_t lb, int8_t ub, bool aligned);
template OCUDUASN_CODE unpack_length<int16_t>(int16_t& n, cbit_ref& bref, int16_t lb, int16_t ub, bool aligned);
template OCUDUASN_CODE unpack_length<int32_t>(int32_t& n, cbit_ref& bref, int32_t lb, int32_t ub, bool aligned);
template OCUDUASN_CODE unpack_length<int64_t>(int64_t& n, cbit_ref& bref, int64_t lb, int64_t ub, bool aligned);

/**
 * X.691 - Section 10.9
 * Pack unconstrained length determinant
 * @param bref
 * @param val "n" length value to pack
 * @param aligned
 * @return
 */
OCUDUASN_CODE pack_length(bit_ref& bref, uint32_t val, bool aligned)
{
  if (not aligned) {
    if (val < 128) {
      HANDLE_CODE(bref.pack(val, 8)); // first bit is 0
    } else if (val < ASN_16K) {
      HANDLE_CODE(bref.pack(0b10, 2));
      HANDLE_CODE(bref.pack(val, 14));
    } else {
      log_error("Not handling sizes longer than 16383 octets");
      return OCUDUASN_ERROR_ENCODE_FAIL;
    }
  } else {
    if (val > ASN_64K) {
      // TODO: Error message
      return OCUDUASN_ERROR_ENCODE_FAIL;
    }
    HANDLE_CODE(bref.align_bytes_zero());
    if (val < 128) {
      return bref.pack(val, 8); // first bit is 0
    } else if (val < ASN_16K) {
      HANDLE_CODE(bref.pack(0b10, 2));
      return bref.pack(val, 14);
    } else {
      HANDLE_CODE(bref.pack(0b11, 2));
      uint32_t m = val / ASN_16K;
      HANDLE_CODE(bref.pack(m, 6));
      val = val - m * ASN_16K;
      return bref.pack(val, 16); // TODO: TODO
    }
  }
  return OCUDUASN_SUCCESS;
}

OCUDUASN_CODE unpack_length(uint32_t& val, cbit_ref& bref, bool aligned)
{
  bool ext;
  if (not aligned) {
    OCUDUASN_CODE ret = bref.unpack(ext, 1);
    HANDLE_CODE(ret);
    if (not ext) {
      ret = bref.unpack(val, 7);
    } else {
      ret = bref.unpack(ext, 1);
      HANDLE_CODE(ret);
      if (not ext) {
        ret = bref.unpack(val, 14);
      } else {
        log_error("Not handling octet strings longer than 16383 octets");
        val = 0;
        return OCUDUASN_ERROR_DECODE_FAIL;
      }
    }
    return ret;
  } else {
    HANDLE_CODE(bref.align_bytes());
    HANDLE_CODE(bref.unpack(val, 8));
    if (val < 128) {
      // "n" < 128
      // first bit was set to zero
      return OCUDUASN_SUCCESS;
    }
    val -= 128;
    if (val < 64) {
      // 128 <= "n" < 16K
      // second bit is set to zero as well
      uint32_t val_octet_2;
      HANDLE_CODE(bref.unpack(val_octet_2, 8));
      val = (val << 8u) + val_octet_2;
      return OCUDUASN_SUCCESS;
    }
    val = 0;
    log_error("Not handling octet strings longer than 16383 octets");
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
}

/************************
        Integer
************************/

template <typename IntType>
OCUDUASN_CODE pack_integer(bit_ref& bref, IntType n, IntType lb, IntType ub, bool has_ext, bool aligned)
{
  // pack ext marker
  bool within_bounds = n >= lb and n <= ub;
  if (has_ext) {
    HANDLE_CODE(bref.pack(not within_bounds, 1));
  } else if (not within_bounds) {
    log_error("The condition lb <= n <= ub ({} <= {} <= {}) was not met", (long)lb, (long)n, (long)ub);
    return OCUDUASN_ERROR_ENCODE_FAIL;
  }
  // Type limits of signed integers stand for absent bounds; unsigned integers are always constrained.
  bool lower_bounded = lb != std::numeric_limits<IntType>::min() or std::is_unsigned<IntType>::value;
  bool upper_bounded = ub != std::numeric_limits<IntType>::max() or std::is_unsigned<IntType>::value;

  if (within_bounds and lower_bounded and upper_bounded) {
    HANDLE_CODE(pack_constrained_whole_number(bref, n, (IntType)lb, (IntType)ub, aligned));
  } else {
    // See X.691 - 12.2.6
    if (not within_bounds or (not lower_bounded and not upper_bounded)) {
      HANDLE_CODE(pack_length(bref, unconstrained_whole_number_length(n), aligned));
      HANDLE_CODE(pack_unconstrained_whole_number(bref, n, aligned));
    } else {
      log_error("Semi-constrained integers are not supported");
      return OCUDUASN_ERROR_ENCODE_FAIL;
    }
  }
  return OCUDUASN_SUCCESS;
}
template OCUDUASN_CODE
pack_integer<uint8_t>(bit_ref& bref, uint8_t n, uint8_t lb, uint8_t ub, bool has_ext, bool aligned);
template OCUDUASN_CODE
pack_integer<uint16_t>(bit_ref& bref, uint16_t n, uint16_t lb, uint16_t ub, bool has_ext, bool aligned);
template OCUDUASN_CODE
pack_integer<uint32_t>(bit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub, bool has_ext, bool aligned);
template OCUDUASN_CODE
pack_integer<uint64_t>(bit_ref& bref, uint64_t n, uint64_t lb, uint64_t ub, bool has_ext, bool aligned);
template OCUDUASN_CODE pack_integer<int8_t>(bit_ref& bref, int8_t n, int8_t lb, int8_t ub, bool has_ext, bool aligned);
template OCUDUASN_CODE
pack_integer<int16_t>(bit_ref& bref, int16_t n, int16_t lb, int16_t ub, bool has_ext, bool aligned);
template OCUDUASN_CODE
pack_integer<int32_t>(bit_ref& bref, int32_t n, int32_t lb, int32_t ub, bool has_ext, bool aligned);
template OCUDUASN_CODE
pack_integer<int64_t>(bit_ref& bref, int64_t n, int64_t lb, int64_t ub, bool has_ext, bool aligned);

// unconstrained specialization case
template <typename IntType>
OCUDUASN_CODE pack_unconstrained_integer(bit_ref& bref, IntType n, bool has_ext, bool aligned)
{
  return pack_integer(
      bref, n, std::numeric_limits<IntType>::min(), std::numeric_limits<IntType>::max(), has_ext, aligned);
}
template OCUDUASN_CODE pack_unconstrained_integer<int8_t>(bit_ref& bref, int8_t n, bool has_ext, bool aligned);
template OCUDUASN_CODE pack_unconstrained_integer<int16_t>(bit_ref& bref, int16_t n, bool has_ext, bool aligned);
template OCUDUASN_CODE pack_unconstrained_integer<int32_t>(bit_ref& bref, int32_t n, bool has_ext, bool aligned);
template OCUDUASN_CODE pack_unconstrained_integer<int64_t>(bit_ref& bref, int64_t n, bool has_ext, bool aligned);

template <typename IntType>
OCUDUASN_CODE unpack_integer(IntType& n, cbit_ref& bref, IntType lb, IntType ub, bool has_ext, bool aligned)
{
  bool within_bounds = true;
  if (has_ext) {
    HANDLE_CODE(bref.unpack(within_bounds, 1));
    within_bounds = not within_bounds;
  }
  // Type limits of signed integers stand for absent bounds; unsigned integers are always constrained.
  bool lower_bounded = lb != std::numeric_limits<IntType>::min() or std::is_unsigned<IntType>::value;
  bool upper_bounded = ub != std::numeric_limits<IntType>::max() or std::is_unsigned<IntType>::value;

  if (within_bounds and lower_bounded and upper_bounded) {
    HANDLE_CODE(unpack_constrained_whole_number(n, bref, (IntType)lb, (IntType)ub, aligned));
  } else {
    // See X.691 - 12.2.6
    if (not within_bounds or (not lower_bounded and not upper_bounded)) {
      uint32_t len;
      HANDLE_CODE(unpack_length(len, bref, aligned));
      HANDLE_CODE(unpack_unconstrained_whole_number(n, bref, len, aligned));
    } else {
      log_error("Semi-constrained integers are not supported");
      return OCUDUASN_ERROR_DECODE_FAIL;
    }
  }
  return OCUDUASN_SUCCESS;
}
template OCUDUASN_CODE
unpack_integer<uint8_t>(uint8_t& n, cbit_ref& bref, uint8_t lb, uint8_t ub, bool has_ext, bool aligned);
template OCUDUASN_CODE
unpack_integer<uint16_t>(uint16_t& n, cbit_ref& bref, uint16_t lb, uint16_t ub, bool has_ext, bool aligned);
template OCUDUASN_CODE
unpack_integer<uint32_t>(uint32_t& n, cbit_ref& bref, uint32_t lb, uint32_t ub, bool has_ext, bool aligned);
template OCUDUASN_CODE
unpack_integer<uint64_t>(uint64_t& n, cbit_ref& bref, uint64_t lb, uint64_t ub, bool has_ext, bool aligned);
template OCUDUASN_CODE
unpack_integer<int8_t>(int8_t& n, cbit_ref& bref, int8_t lb, int8_t ub, bool has_ext, bool aligned);
template OCUDUASN_CODE
unpack_integer<int16_t>(int16_t& n, cbit_ref& bref, int16_t lb, int16_t ub, bool has_ext, bool aligned);
template OCUDUASN_CODE
unpack_integer<int32_t>(int32_t& n, cbit_ref& bref, int32_t lb, int32_t ub, bool has_ext, bool aligned);
template OCUDUASN_CODE
unpack_integer<int64_t>(int64_t& n, cbit_ref& bref, int64_t lb, int64_t ub, bool has_ext, bool aligned);

// unconstrained specialization case
template <typename IntType>
OCUDUASN_CODE unpack_unconstrained_integer(IntType& n, cbit_ref& bref, bool has_ext, bool aligned)
{
  return unpack_integer(
      n, bref, std::numeric_limits<IntType>::min(), std::numeric_limits<IntType>::max(), has_ext, aligned);
}
template OCUDUASN_CODE unpack_unconstrained_integer<int8_t>(int8_t& n, cbit_ref& bref, bool has_ext, bool aligned);
template OCUDUASN_CODE unpack_unconstrained_integer<int16_t>(int16_t& n, cbit_ref& bref, bool has_ext, bool aligned);
template OCUDUASN_CODE unpack_unconstrained_integer<int32_t>(int32_t& n, cbit_ref& bref, bool has_ext, bool aligned);
template OCUDUASN_CODE unpack_unconstrained_integer<int64_t>(int64_t& n, cbit_ref& bref, bool has_ext, bool aligned);

// standalone packer
template <class IntType>
integer_packer<IntType>::integer_packer(IntType lb_, IntType ub_, bool has_ext_, bool aligned_) :
  lb(lb_), ub(ub_), has_ext(has_ext_), aligned(aligned_)
{
}

template <class IntType>
OCUDUASN_CODE integer_packer<IntType>::pack(bit_ref& bref, IntType n)
{
  return pack_integer(bref, n, lb, ub, has_ext, aligned);
}
template <class IntType>
OCUDUASN_CODE integer_packer<IntType>::unpack(IntType& n, cbit_ref& bref)
{
  return unpack_integer(n, bref, lb, ub, has_ext, aligned);
}
template struct integer_packer<int8_t>;
template struct integer_packer<int16_t>;
template struct integer_packer<int32_t>;
template struct integer_packer<int64_t>;
template struct integer_packer<uint8_t>;
template struct integer_packer<uint16_t>;
template struct integer_packer<uint32_t>;
template struct integer_packer<uint64_t>;

/*********************
      ext group
*********************/

ext_groups_unpacker::ext_groups_unpacker(cbit_ref& bref, bool aligned_) :
  aligned(aligned_), outer_bref(bref), group_bref({})
{
}

OCUDUASN_CODE ext_groups_unpacker::unpack_presence_flags()
{
  // unpack nof of ext groups
  uint32_t nof_ext_groups = 0;
  HANDLE_CODE(unpack_norm_small_non_neg_whole_number(nof_ext_groups, outer_bref));
  nof_unpacked_groups = nof_ext_groups + 1;

  // Resize presence flags vector
  groups.resize(nof_unpacked_groups);
  std::fill(groups.data(), groups.data() + nof_unpacked_groups, false);

  // unpack each group presence flag
  for (uint32_t i = 0; i < nof_unpacked_groups; ++i) {
    HANDLE_CODE(outer_bref.unpack(groups[i], 1));
  }

  return OCUDUASN_SUCCESS;
}

OCUDUASN_CODE ext_groups_unpacker::unpack_next_group()
{
  if (state == state_t::unpack_presence_flags) {
    state = state_t::unpack_groups;
    HANDLE_CODE(unpack_presence_flags());
  }

  if (next_group_idx >= nof_unpacked_groups) {
    state = state_t::done;
    return OCUDUASN_SUCCESS;
  }

  if (groups[next_group_idx++]) {
    // Group is present. Decode its length.
    uint32_t group_len;
    HANDLE_CODE(unpack_length(group_len, outer_bref, aligned));
    // Create subview for the group
    group_bref = outer_bref.subview(0, group_len);
    // Consume the padding bits of the group from the outer bitref
    HANDLE_CODE(outer_bref.advance_bytes(group_len));
  }

  return OCUDUASN_SUCCESS;
}

OCUDUASN_CODE ext_groups_unpacker::consume_remaining_groups(cbit_ref& bref)
{
  if (state == state_t::unpack_presence_flags) {
    // No group was unpacked yet, so the presence flags are still pending.
    HANDLE_CODE(unpack_next_group());
  }
  while (next_group_idx < nof_unpacked_groups) {
    HANDLE_CODE(unpack_next_group());
  }
  bref = outer_bref;
  return OCUDUASN_SUCCESS;
}

/*********************
     Open Field
*********************/

varlength_field_pack_guard::varlength_field_pack_guard(bit_ref& bref, bool align_) : brefstart(bref)
{
  bref         = bit_ref(varlen_buffer);
  bref_tracker = &bref;
  align        = align_;
}

varlength_field_pack_guard::~varlength_field_pack_guard()
{
  if (bref_tracker->distance() == 0) {
    // X.691 replaces an empty complete encoding, e.g. of NULL, with a single zero octet.
    bref_tracker->pack(0, 8);
  }
  // fill the spare bits
  uint32_t leftover = 7U - ((bref_tracker->distance() - (uint32_t)1U) % (uint32_t)8U);
  bref_tracker->pack(0, leftover);

  // check how many bytes were written in total
  uint32_t nof_bytes = bref_tracker->distance() / (uint32_t)8U;

  // go back in time to pack length
  pack_length(brefstart, nof_bytes, align);

  // pack encoded bytes
  brefstart.pack_bytes(varlen_buffer);
  *bref_tracker = brefstart;
}

varlength_field_unpack_guard::varlength_field_unpack_guard(cbit_ref& bref, bool align) : bref0(bref)
{
  if (unpack_length(len, bref, align) != OCUDUASN_SUCCESS) {
    // The length could not be decoded, e.g. due to a truncated buffer. Leave an empty view, so that unpacking the field
    // contents fails, and do not restore the caller's bit_ref on destruction.
    len  = 0;
    bref = cbit_ref(ocudu::byte_buffer_view{});
    return;
  }
  bref0        = bref;
  bref_tracker = &bref;
  bref         = bref.subview(0, len);
}

varlength_field_unpack_guard::~varlength_field_unpack_guard()
{
  if (bref_tracker == nullptr) {
    return;
  }
  if (len * 8 < (unsigned)bref_tracker->distance(bref0)) {
    log_error("The number of bits unpacked exceeds the variable length field size ({} > {})",
              bref_tracker->distance(bref0),
              len * 8);
    return;
  }

  // Ignore padding bits, and skip to the end of the varlength field.
  bref0.advance_bytes(len);
  *bref_tracker = bref0;
}

/************************
          REAL
************************/

OCUDUASN_CODE pack_unconstrained_real(bit_ref& bref, float n, bool aligned)
{
  if (aligned) {
    HANDLE_CODE(bref.align_bytes_zero());
  }

  // Handle special cases
  if (std::isinf(n)) {
    pack_length(bref, 1, aligned);
    bref.pack(n > 0 ? 0x40 : 0x41, 8);
    return OCUDUASN_SUCCESS;
  } else if (std::isnan(n)) {
    pack_length(bref, 1, aligned);
    bref.pack(0x42, 8);
    return OCUDUASN_SUCCESS;
  } else if (n == 0.0f) {
    pack_length(bref, 0, aligned);
    return OCUDUASN_SUCCESS;
  }

  uint32_t* bits_ptr = (uint32_t*)&n;
  uint32_t  bits     = *bits_ptr;
  bool      sign     = (bits >> 31) & 0x1;
  int8_t    exponent = ((bits >> 23) & 0xff);
  uint32_t  mantissa = (bits & 0x7fffff) | 0x800000;

  uint8_t info_octet = 0x80; // Base-2
  if (sign) {
    info_octet |= 0x1 << 6;
  }

  uint32_t trailing_zeros = ocudu::zero_lsb_count<unsigned>(mantissa);
  mantissa >>= trailing_zeros;
  // the inverse of the trailing zeros gives the number of bits to shift
  // the mantissa to the right to make it a whole number, this number must be added to the exponent
  uint8_t inv_trailing_zeros = 23 - trailing_zeros;
  exponent -= (127 + inv_trailing_zeros);
  uint8_t asn1_exponent = exponent & 0xff;

  uint8_t mantissa_bit_len = 23 + 1 - trailing_zeros;
  uint8_t mantissa_oct_len = (mantissa_bit_len + 7) / 8;

  uint32_t len = mantissa_oct_len + 2;
  pack_length(bref, len, aligned);
  bref.pack(info_octet, 8);
  bref.pack(asn1_exponent, 8);
  for (int8_t i = mantissa_oct_len - 1; i >= 0; --i) {
    uint8_t octet = (mantissa >> (i * 8)) & 0xff;
    bref.pack(octet, 8);
  }
  return OCUDUASN_SUCCESS;
}

OCUDUASN_CODE unpack_unconstrained_real(float& n, cbit_ref& bref, bool aligned)
{
  if (aligned) {
    HANDLE_CODE(bref.align_bytes());
  }
  uint32_t len;
  HANDLE_CODE(unpack_length(len, bref, aligned));

  // Handle special case 0
  if (len == 0) {
    n = 0;
    return OCUDUASN_SUCCESS;
  }

  if (len == 2) {
    return OCUDUASN_ERROR_DECODE_FAIL;
  }

  uint8_t buf[10];
  if (len > sizeof(buf)) {
    asn1::log_error("REAL length {} exceeds maximum supported size {}", len, sizeof(buf));
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
  for (uint32_t i = 0; i < len; i++) {
    HANDLE_CODE(bref.unpack(buf[i], 8));
  }

  uint8_t info_octet = buf[0];

  // Handle special cases
  if (info_octet == 0x40) {
    n = std::numeric_limits<float>::infinity(); // Positive infinity
    return OCUDUASN_SUCCESS;
  } else if (info_octet == 0x41) {
    n = -std::numeric_limits<float>::infinity(); // Negative infinity
    return OCUDUASN_SUCCESS;
  } else if (info_octet == 0x42) {
    n = std::numeric_limits<float>::quiet_NaN(); // NaN
    return OCUDUASN_SUCCESS;
  }

  uint8_t exponent_length = (info_octet & 0x3) + 1;
  uint8_t mantissa_length = len - exponent_length - 1;
  if (exponent_length != 1) {
    // Currently we support decoding only float with 1 byte exponent.
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
  if (mantissa_length > 3) {
    // Currently we support decoding only float with 3 byte mantissa.
    return OCUDUASN_ERROR_DECODE_FAIL;
  }

  bool     sign          = (info_octet >> 6) & 0x1;
  int8_t   asn1_exponent = static_cast<int8_t>(buf[1]);
  uint32_t mantissa      = 0;

  for (size_t i = 2; i < len; ++i) {
    mantissa = (mantissa << 8) | buf[i];
  }
  uint8_t leading_zeros = ocudu::zero_msb_count<unsigned>(mantissa) - 8;
  mantissa <<= leading_zeros;

  // Build the IEEE 754 float value from the sign, exponent, and mantissa
  uint32_t ieee_exponent = asn1_exponent + 127;
  uint32_t ieee_mantissa = mantissa & 0x7fffff;

  if (ieee_mantissa) {
    ieee_exponent += (23 - leading_zeros);
  }

  uint32_t ieee_bits = (sign << 31) | (ieee_exponent << 23) | ieee_mantissa;
  float*   bits_ptr  = (float*)&ieee_bits;
  n                  = *bits_ptr;
  return OCUDUASN_SUCCESS;
}

} // namespace asn1
