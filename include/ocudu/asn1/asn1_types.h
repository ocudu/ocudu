// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Value types of the ASN.1 runtime.

#pragma once

#include "ocudu/asn1/asn1_json.h"
#include "ocudu/asn1/asn1_per.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>

namespace asn1 {

/************************
    asn1 null packing
************************/

struct asn1_null_t {
  OCUDUASN_CODE pack(bit_ref& bref) const { return OCUDUASN_SUCCESS; }
  OCUDUASN_CODE unpack(cbit_ref& bref) const { return OCUDUASN_SUCCESS; }
};

/*********************
  ENUMERATED
*********************/

const char* convert_enum_idx(const char* array[], uint32_t nof_types, uint32_t enum_val, const char* enum_type);
template <class ItemType>
ItemType map_enum_number(ItemType* array, uint32_t nof_types, uint32_t enum_val, const char* enum_type);

template <class EnumType>
bool string_to_enum(EnumType& e, const std::string& s)
{
  for (uint32_t i = 0; i < EnumType::nof_types; ++i) {
    e = (typename EnumType::options)i;
    if (e.to_string() == s) {
      return true;
    }
  }
  return false;
}
template <class EnumType, class NumberType>
bool number_to_enum(EnumType& e, NumberType val)
{
  for (uint32_t i = 0; i != e.nof_types; ++i) {
    e = (typename EnumType::options)i;
    if (e.to_number() == val) {
      return true;
    }
  }
  return false;
}
template <class EnumType, class NumberType>
bool float_number_to_enum(EnumType& e, NumberType val, NumberType precision)
{
  static_assert(std::is_floating_point_v<NumberType>, "Expected float or double");
  static_assert(std::is_floating_point_v<decltype(std::declval<EnumType>().to_number())>,
                "Expected float or double to_number()");
  for (uint32_t i = 0; i != e.nof_types; ++i) {
    e = (typename EnumType::options)i;
    if (std::abs(e.to_number() - val) < precision) {
      return true;
    }
  }
  return false;
}
template <class EnumType>
void bool_to_enum(EnumType& e, bool s)
{
  ocudu_assert(e.nof_types == 1, "Can't convert enum with too many values");

  if (s) {
    e = (typename EnumType::options)0;
  } else {
    e = EnumType::nulltype;
  }
}
template <class EnumType>
bool enum_to_bool(EnumType& e)
{
  ocudu_assert(e.nof_types == 1, "Can't convert enum with too many values");

  return !(bool)e;
}

namespace table {

/// Names of the values of an ENUMERATED, and the name of its type for the log of an invalid value.
struct enum_desc {
  const char** names;
  uint32_t     nof_types;
  const char*  type_name;
};

/// Name of the given value of an ENUMERATED.
const char* enum_to_string(const enum_desc& desc, uint32_t value);

} // namespace table

namespace detail {

template <typename T, typename = void>
struct has_to_string_member : std::false_type {};
template <typename T>
struct has_to_string_member<T, std::void_t<decltype(std::declval<const T&>().to_string())>> : std::true_type {};

} // namespace detail

/// ENUMERATED value of the options of EnumType, whose names are in its desc member unless it has its own to_string().
template <class EnumType, bool E = false, uint32_t M = 0>
class enumerated : public EnumType
{
public:
  static const uint32_t nof_types = EnumType::nulltype, nof_exts = M;
  static const bool     has_ext = E;

  enumerated() { EnumType::value = EnumType::nulltype; }
  enumerated(typename EnumType::options enumerated_option_val) { EnumType::value = enumerated_option_val; }
  EnumType& operator=(EnumType v)
  {
    EnumType::value = v;
    return *this;
  }
  operator typename EnumType::options() const { return EnumType::value; }

  const char* to_string() const
  {
    if constexpr (detail::has_to_string_member<EnumType>::value) {
      return EnumType::to_string();
    } else {
      return table::enum_to_string(EnumType::desc, EnumType::value);
    }
  }
};

/// Packs an ENUMERATED value.
template <class EnumType, bool E, uint32_t M>
OCUDUASN_CODE pack(bit_ref& bref, const enumerated<EnumType, E, M>& e)
{
  return pack_enum(bref, e);
}
/// Unpacks an ENUMERATED value.
template <class EnumType, bool E, uint32_t M>
OCUDUASN_CODE unpack(enumerated<EnumType, E, M>& e, cbit_ref& bref)
{
  return unpack_enum(e, bref);
}

/*********************
  INTEGER
*********************/

template <class IntType,
          IntType LB  = std::numeric_limits<IntType>::min(),
          IntType UB  = std::numeric_limits<IntType>::max(),
          bool    Ext = false,
          bool    Al  = false>
class integer
{
public:
  static const IntType ub = UB, lb = LB;
  static const bool    has_ext = Ext, is_aligned = Al;
  IntType              value;
  integer() = default;
  integer(IntType value_) : value(value_) {}
  operator IntType() const { return value; }

  OCUDUASN_CODE pack(bit_ref& bref) const { return pack_integer(bref, value, lb, ub, has_ext, is_aligned); }
  OCUDUASN_CODE unpack(cbit_ref& bref) { return unpack_integer(value, bref, lb, ub, has_ext, is_aligned); }
};

/*********************
    OCTET STRING
*********************/

namespace octet_string_helper {

/// Convert octet string to an integer.
uint64_t to_uint(ocudu::span<const uint8_t> buf);
uint64_t to_uint(const ocudu::byte_buffer& buf);

/// Convert unsigned integer to span of octets.
void to_octet_string(ocudu::span<uint8_t> buf, uint64_t number);
void to_octet_string(ocudu::byte_buffer& buf, uint64_t number);

/// Convert octets into hex string.
std::string to_hex_string(ocudu::span<const uint8_t> buf);
std::string to_hex_string(const ocudu::byte_buffer& buf);

/// Convert std::string of hexadecimal numbers to span of bytes.
unsigned hex_string_to_octets(ocudu::span<uint8_t> buf, const std::string& str);

/// Append std::string of hexadecimal numbers to byte_buffer.
void append_hex_string(ocudu::byte_buffer& buf, const std::string& str);

} // namespace octet_string_helper

/************************
    fixed_octstring
************************/

template <uint32_t N, bool aligned = false>
class fixed_octstring
{
public:
  using iterator       = typename std::array<uint8_t, N>::iterator;
  using const_iterator = typename std::array<uint8_t, N>::const_iterator;

  fixed_octstring() = default;
  explicit fixed_octstring(const std::array<uint8_t, N>& array_) : octets_(array_) {}
  fixed_octstring<N, aligned>& operator=(const std::array<uint8_t, N>& array_)
  {
    octets_ = array_;
    return *this;
  }

  const uint8_t& operator[](uint32_t idx) const { return octets_[idx]; }
  uint8_t&       operator[](uint32_t idx) { return octets_[idx]; }
  bool           operator==(const fixed_octstring<N>& other) const { return octets_ == other.octets_; }
  uint8_t*       data() { return &octets_[0]; }
  const uint8_t* data() const { return &octets_[0]; }

  static uint32_t              size() { return N; }
  std::string                  to_string() const { return octet_string_helper::to_hex_string(octets_); }
  fixed_octstring<N, aligned>& from_string(const std::string& hexstr)
  {
    if (hexstr.size() != 2 * N) {
      log_error("The provided hex string size is not valid ({}!=2*{}).", hexstr.size(), (size_t)N);
    } else {
      octet_string_helper::hex_string_to_octets(octets_, hexstr);
    }
    return *this;
  }
  uint64_t                     to_number() const { return octet_string_helper::to_uint(octets_); }
  fixed_octstring<N, aligned>& from_number(uint64_t val)
  {
    octet_string_helper::to_octet_string(octets_, val);
    return *this;
  }
  const std::array<uint8_t, N> to_bytes() const { return octets_; }

  /// Pack fixed-size octet string as per X.691 Section 16 - Encoding the octetstring type.
  /// \tparam N - number of items
  /// \tparam ext - aligned variant
  /// \param bref
  /// \return
  OCUDUASN_CODE pack(bit_ref& bref) const
  {
    if (aligned and N > 2) {
      bref.align_bytes_zero();
    }
    return bref.pack_bytes(octets_);
  }
  OCUDUASN_CODE unpack(cbit_ref& bref)
  {
    if (aligned and N > 2) {
      bref.align_bytes();
    }
    return bref.unpack_bytes(octets_);
  }

  iterator       begin() { return octets_.begin(); }
  const_iterator begin() const { return octets_.begin(); }
  iterator       end() { return octets_.end(); }
  const_iterator end() const { return octets_.end(); }

private:
  std::array<uint8_t, N> octets_;
};

/************************
  constrained_octstring
************************/

template <uint32_t LB, uint32_t UB, bool aligned = false>
class bounded_octstring
{
public:
  const uint8_t& operator[](uint32_t idx) const { return octets_[idx]; }
  uint8_t&       operator[](uint32_t idx) { return octets_[idx]; }
  bool           operator==(const bounded_octstring& other) const { return octets_ == other.octets_; }
  uint8_t*       data() { return &octets_[0]; }
  const uint8_t* data() const { return &octets_[0]; }

  void        resize(uint32_t new_size) { octets_.resize(new_size); }
  uint32_t    size() const { return octets_.size(); }
  std::string to_string() const
  {
    return octet_string_helper::to_hex_string(ocudu::span<const uint8_t>{data(), size()});
  }
  bounded_octstring<LB, UB, aligned>& from_string(const std::string& hexstr)
  {
    if (hexstr.size() > 2 * UB) {
      log_error("The provided hex string size is not valid ({}>2*{}).", hexstr.size(), (size_t)UB);
    } else {
      resize(hexstr.size() / 2);
      octet_string_helper::hex_string_to_octets(ocudu::span<uint8_t>{octets_.data(), octets_.size()}, hexstr);
    }
    return *this;
  }
  uint64_t to_number() const
  {
    return octet_string_helper::to_uint(ocudu::span<const uint8_t>{octets_.data(), size()});
  }

  OCUDUASN_CODE pack(bit_ref& bref) const
  {
    HANDLE_CODE(pack_length(bref, size(), LB, UB, aligned));
    if (aligned) {
      HANDLE_CODE(bref.align_bytes_zero());
    }
    return bref.pack_bytes(ocudu::span<const uint8_t>{data(), size()});
  }
  OCUDUASN_CODE unpack(cbit_ref& bref)
  {
    uint32_t len;
    HANDLE_CODE(unpack_length(len, bref, LB, UB, aligned));
    resize(len);
    if (aligned) {
      HANDLE_CODE(bref.align_bytes());
    }
    return bref.unpack_bytes(ocudu::span<uint8_t>{data(), size()});
  }

private:
  ext_array<uint8_t, max_size({UB, 64U})> octets_;
};

/************************
 unbounded OCTET STRING
************************/

template <bool Al = false>
class unbounded_octstring : public ocudu::byte_buffer
{
public:
  static const bool aligned = Al;
  using value_type          = byte_buffer::value_type;
  using iterator            = byte_buffer::iterator;
  using const_iterator      = byte_buffer::const_iterator;

  using ocudu::byte_buffer::byte_buffer;
  unbounded_octstring(byte_buffer other) noexcept : ocudu::byte_buffer(std::move(other)) {}
  unbounded_octstring(const unbounded_octstring& other) noexcept;
  unbounded_octstring(unbounded_octstring&& other) noexcept : ocudu::byte_buffer(std::move(other)) {}

  unbounded_octstring& operator=(byte_buffer other) noexcept
  {
    *static_cast<ocudu::byte_buffer*>(this) = std::move(other);
    return *this;
  }
  unbounded_octstring& operator=(const unbounded_octstring& other) noexcept;
  unbounded_octstring& operator=(unbounded_octstring&& other) noexcept;

  size_t size() const { return this->length(); }

  std::string              to_string() const;
  unbounded_octstring<Al>& from_string(const std::string& hexstr);
  unbounded_octstring<Al>& from_bytes(ocudu::span<const uint8_t> bytes);

  uint64_t                 to_number() const;
  unbounded_octstring<Al>& from_number(uint64_t val);

  ocudu::byte_buffer to_byte_buffer() { return std::move(*static_cast<ocudu::byte_buffer*>(this)); }

  OCUDUASN_CODE pack(bit_ref& bref) const;
  OCUDUASN_CODE unpack(cbit_ref& bref);
};

using dyn_octstring = unbounded_octstring<false>;

/*********************
      bitstring
*********************/

namespace bitstring_utils {
inline bool get(const uint8_t* ptr, uint32_t idx)
{
  uint32_t byte_idx = idx / 8;
  uint32_t offset   = idx % 8;
  return (ptr[byte_idx] & (1u << offset)) > 0;
}
inline void set(uint8_t* ptr, uint32_t idx, bool value)
{
  uint32_t byte_idx = idx / 8;
  uint32_t offset   = idx % 8;
  if (value) {
    ptr[byte_idx] |= (1u << offset);
  } else {
    ptr[byte_idx] &= ((uint16_t)(1u << 8u) - 1u) - (1u << offset);
  }
}
OCUDUASN_CODE
pack(bit_ref& bref, const uint8_t* data, uint32_t size, uint32_t lb, uint32_t ub, bool has_ext, bool is_aligned);
OCUDUASN_CODE
unpack_length_prefix(uint32_t& len, cbit_ref& bref, uint32_t lb, uint32_t ub, bool has_ext, bool is_aligned);
OCUDUASN_CODE unpack_bitfield(uint8_t* buf, cbit_ref& bref, uint32_t n, uint32_t lb, uint32_t ub, bool is_aligned);

uint64_t    to_number(const uint8_t* ptr, uint32_t nbits);
void        from_number(uint8_t* ptr, uint64_t number, uint32_t nbits);
std::string to_string(const uint8_t* ptr, uint32_t nbits);
/// Hex string of the bits, first bit first, padded with zeros to whole octets.
std::string to_hex_string(const uint8_t* ptr, uint32_t nbits);

/// Unpacks a BIT STRING into bits, which resize sizes for the given number of bits, returning its octets.
OCUDUASN_CODE unpack(void* bits,
                     uint8_t* (*resize)(void* bits, uint32_t nof_bits),
                     cbit_ref& bref,
                     uint32_t  lb,
                     uint32_t  ub,
                     bool      has_ext,
                     bool      is_aligned);
/// Writes the JER form of a BIT STRING: a hex string when its size is fixed, {"value", "length"} otherwise.
void to_json(json_writer& j, const uint8_t* ptr, uint32_t nbits, bool fixed_size);

} // namespace bitstring_utils

template <uint32_t LB, uint32_t UB, bool ext = false, bool aligned = false>
class bitstring
{
  using this_type = bitstring<LB, UB, ext, aligned>;

public:
  static const uint32_t lb = LB, ub = UB;
  static const bool     has_ext = ext, is_aligned = aligned;

  bitstring() { resize(lb); }
  explicit bitstring(uint32_t siz_) { resize(siz_); }
  explicit bitstring(const std::string& s)
  {
    resize(s.size());
    std::memset(&octets_[0], 0, nof_octets());
    for (uint32_t i = 0; i < s.size(); ++i)
      set(s.size() - i - 1, s[i] == '1');
  }

  bool     get(uint32_t idx) const { return bitstring_utils::get(data(), idx); }
  void     set(uint32_t idx, bool value) { bitstring_utils::set(data(), idx, value); }
  uint32_t nof_octets() const { return ceil_frac(length(), 8u); }

  const uint8_t* data() const { return &octets_[0]; }
  uint8_t*       data() { return &octets_[0]; }
  uint32_t       length() const { return nof_bits; }
  void           resize(uint32_t new_size)
  {
    nof_bits = new_size;
    octets_.resize(nof_octets());
    std::memset(data(), 0, nof_octets()); // resize always resets content
  }

  // comparison
  bool operator==(const this_type& other) const
  {
    return length() == other.length() and std::equal(data(), data() + nof_octets(), other.data());
  }
  bool operator==(const char* other_str) const
  {
    return strlen(other_str) == length() and (*this) == this_type{}.from_string(other_str);
  }

  // string conversion
  std::string to_string() const { return bitstring_utils::to_string(data(), length()); }
  this_type&  from_string(const std::string& s)
  {
    if (s.size() < lb or s.size() > ub) {
      log_error("The provided string size={} is not withing the bounds [{}, {}]", s.size(), uint32_t(lb), uint32_t(ub));
    } else {
      resize(s.size());
      for (uint32_t i = 0; i < s.size(); ++i) {
        set(s.size() - i - 1, s[i] == '1');
      }
    }
    return *this;
  }

  // number conversion
  uint64_t   to_number() const { return bitstring_utils::to_number(data(), length()); }
  this_type& from_number(uint64_t val)
  {
    auto nof_bits_ = std::max((uint32_t)std::ceil(std::log2(std::max(val, (uint64_t)1u))), LB);
    if (nof_bits_ > UB) {
      log_error("The provided bitstring value {} does not fit the bounds [{}, {}]", val, uint32_t(lb), uint32_t(ub));
      return *this;
    }
    resize(nof_bits_);
    bitstring_utils::from_number(data(), val, length());
    return *this;
  }

  this_type& from_number(uint64_t val, uint32_t nof_bits_)
  {
    if (nof_bits_ > UB) {
      log_error("The provided bitstring value {} does not fit the bounds [{}, {}]", val, uint32_t(lb), uint32_t(ub));
      return *this;
    }
    resize(nof_bits_);
    bitstring_utils::from_number(data(), val, length());
    return *this;
  }

  // packers / unpackers
  OCUDUASN_CODE pack(bit_ref& bref) const
  {
    return bitstring_utils::pack(bref, data(), length(), lb, ub, has_ext, is_aligned);
  }
  OCUDUASN_CODE unpack(cbit_ref& bref)
  {
    auto resize_bits = [](void* bits, uint32_t nof_bits_) {
      auto& self = *static_cast<this_type*>(bits);
      self.resize(nof_bits_);
      return self.data();
    };
    return bitstring_utils::unpack(this, resize_bits, bref, lb, ub, has_ext, is_aligned);
  }

  /// Writes the JER form: a hex string for fixed sizes, {"value", "length"} otherwise.
  void to_json(json_writer& j) const;

private:
  static const uint32_t          stack_size = (UB == std::numeric_limits<uint32_t>::max()) ? 4 : ceil_frac(ub, 8u);
  ext_array<uint8_t, stack_size> octets_;
  uint32_t                       nof_bits = 0;
};

template <uint32_t LB, uint32_t UB, bool ext = false, bool aligned = false>
using bounded_bitstring = bitstring<LB, UB, ext, aligned>;

template <bool Ext = false, bool Al = false>
using unbounded_bitstring = bitstring<0, std::numeric_limits<uint32_t>::max(), Ext, Al>;

using dyn_bitstring = unbounded_bitstring<false, false>;

template <uint32_t N, bool Ext = false, bool Al = false>
using fixed_bitstring = bitstring<N, N, Ext, Al>;

template <uint32_t LB, uint32_t UB, bool ext, bool aligned>
void bitstring<LB, UB, ext, aligned>::to_json(json_writer& j) const
{
  bitstring_utils::to_json(j, data(), length(), LB == UB and not ext);
}

/*********************
  SEQUENCE OF
*********************/

template <class ItemType, uint32_t lb, uint32_t ub, bool aligned = false>
struct dyn_seq_of : public dyn_array<ItemType> {
  dyn_seq_of() = default;
  dyn_seq_of(const dyn_array<ItemType>& other) : dyn_array<ItemType>(other) {}
  dyn_seq_of(const bounded_array<ItemType, ub>& other) : dyn_array<ItemType>(&other[0], other.size()) {}
  OCUDUASN_CODE pack(bit_ref& bref) const { return pack_dyn_seq_of(bref, *this, lb, ub, aligned); }
  OCUDUASN_CODE unpack(cbit_ref& bref) { return unpack_dyn_seq_of(*this, bref, lb, ub, aligned); }
  void          to_json(json_writer& j) const;
};

/*********************
   printable string
*********************/

/* X.691 - Section 27 - Character Restricted String */
namespace asn_string_utils {
OCUDUASN_CODE
pack(bit_ref& bref, const std::string& s, size_t lb, size_t ub, size_t alb, size_t aub, bool ext, bool aligned);
OCUDUASN_CODE
unpack(std::string& s, cbit_ref& bref, size_t lb, size_t ub, size_t alb, size_t aub, bool ext, bool aligned);
} // namespace asn_string_utils

constexpr uint32_t MAX_ASN_STRING_LENGTH = std::numeric_limits<uint32_t>::max();

template <uint32_t LB,
          uint32_t UB,
          uint32_t ALB     = 0,
          uint32_t AUB     = MAX_ASN_STRING_LENGTH,
          bool     ext     = false,
          bool     aligned = false>
class asn_string
{
public:
  OCUDUASN_CODE pack(bit_ref& bref) const { return asn_string_utils::pack(bref, str, LB, UB, ALB, AUB, ext, aligned); }
  OCUDUASN_CODE unpack(cbit_ref& bref) { return asn_string_utils::unpack(str, bref, LB, UB, ALB, AUB, ext, aligned); }
  char&         operator[](std::size_t idx) { return str[idx]; }
  const char&   operator[](std::size_t idx) const { return str[idx]; }
  void          resize(std::size_t newsize) { str.resize(newsize); }
  std::size_t   size() const { return str.size(); }
  std::string   to_string() const { return str; }
  void          from_string(const std::string& s) { str = s; }

private:
  std::string str;
};

template <uint32_t ALB = 0, uint32_t AUB = MAX_ASN_STRING_LENGTH, bool ext = false, bool aligned = false>
using printable_string = asn_string<32, 122, ALB, AUB, ext, aligned>;

// Size constraints of UTF8String are not PER-visible, so the string is encoded as an unconstrained octet string.
template <bool aligned = false>
using utf8_string = asn_string<0, 255, 0, MAX_ASN_STRING_LENGTH, false, aligned>;

template <uint32_t ALB = 0, uint32_t AUB = MAX_ASN_STRING_LENGTH, bool ext = false, bool aligned = false>
using visible_string = asn_string<32, 126, ALB, AUB, ext, aligned>;

/*********************
  REAL
*********************/

class real_s
{
public:
  float value;
  real_s() = default;
  OCUDUASN_CODE pack(bit_ref& bref) const { return pack_unconstrained_real(bref, value, true); }
  OCUDUASN_CODE unpack(cbit_ref& bref) { return unpack_unconstrained_real(value, bref, true); }
  void          to_json(json_writer& j) const { j.write_float(value); }
};

} // namespace asn1
