// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Free functions asn1::pack, asn1::unpack and asn1::to_json of the ASN.1 types.

#pragma once

#include "ocudu/asn1/asn1_types.h"
#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace asn1 {

/// Tables of the table-driven codecs, defined in asn1_codec.h and asn1_ap_codec.h, and the engine functions that
/// encode and write the JSON of a value of a type with a desc member.
namespace table {
struct seq_desc;
struct choice_desc;
struct ie_container_desc;
OCUDUASN_CODE pack_value(bit_ref& bref, const void* value, const seq_desc& desc);
OCUDUASN_CODE pack_value(bit_ref& bref, const void* value, const choice_desc& desc);
OCUDUASN_CODE pack_value(bit_ref& bref, const void* value, const ie_container_desc& desc);
OCUDUASN_CODE unpack_value(void* value, cbit_ref& bref, const seq_desc& desc);
OCUDUASN_CODE unpack_value(void* value, cbit_ref& bref, const choice_desc& desc);
OCUDUASN_CODE unpack_value(void* value, cbit_ref& bref, const ie_container_desc& desc);
void          to_json_value(json_writer& j, const void* value, const seq_desc& desc);
void          to_json_value(json_writer& j, const void* value, const choice_desc& desc);
void          to_json_value(json_writer& j, const void* value, const ie_container_desc& desc);

/// Codec of a type with hand-written pack, unpack and JSON functions.
struct custom_desc {
  OCUDUASN_CODE (*pack)(bit_ref& bref, const void* value);
  OCUDUASN_CODE (*unpack)(void* value, cbit_ref& bref);
  void (*to_json)(json_writer& j, const void* value);
};

inline OCUDUASN_CODE pack_value(bit_ref& bref, const void* value, const custom_desc& desc)
{
  return desc.pack(bref, value);
}
inline OCUDUASN_CODE unpack_value(void* value, cbit_ref& bref, const custom_desc& desc)
{
  return desc.unpack(value, bref);
}
inline void to_json_value(json_writer& j, const void* value, const custom_desc& desc)
{
  desc.to_json(j, value);
}

} // namespace table

namespace detail {

template <typename T, typename = void>
struct has_pack_member : std::false_type {};
template <typename T>
struct has_pack_member<T, std::void_t<decltype(std::declval<const T&>().pack(std::declval<bit_ref&>()))>>
  : std::true_type {};
template <typename T, typename = void>
struct has_unpack_member : std::false_type {};
template <typename T>
struct has_unpack_member<T, std::void_t<decltype(std::declval<T&>().unpack(std::declval<cbit_ref&>()))>>
  : std::true_type {};
template <typename T, typename = void>
struct has_to_json_member : std::false_type {};
template <typename T>
struct has_to_json_member<T, std::void_t<decltype(std::declval<const T&>().to_json(std::declval<json_writer&>()))>>
  : std::true_type {};

template <typename T>
inline constexpr bool has_pack_member_v = has_pack_member<T>::value;
template <typename T>
inline constexpr bool has_unpack_member_v = has_unpack_member<T>::value;
template <typename T>
inline constexpr bool has_to_json_member_v = has_to_json_member<T>::value;

} // namespace detail

/// Packs the value with its pack() member.
template <typename T>
inline auto pack(bit_ref& bref, const T& obj) -> decltype(obj.pack(bref))
{
  return obj.pack(bref);
}
/// Packs the value of a generated type without pack() member through the table of its codecs.
template <typename T, std::enable_if_t<not detail::has_pack_member_v<T>, int> = 0>
inline auto pack(bit_ref& bref, const T& obj) -> decltype(table::pack_value(bref, &obj, T::desc))
{
  return table::pack_value(bref, &obj, T::desc);
}
/// Unpacks the value with its unpack() member.
template <typename T>
inline auto unpack(T& obj, cbit_ref& bref) -> decltype(obj.unpack(bref))
{
  return obj.unpack(bref);
}
/// Unpacks the value of a generated type without unpack() member through the table of its codecs.
template <typename T, std::enable_if_t<not detail::has_unpack_member_v<T>, int> = 0>
inline auto unpack(T& obj, cbit_ref& bref) -> decltype(table::unpack_value(&obj, bref, T::desc))
{
  return table::unpack_value(&obj, bref, T::desc);
}

/// Writes the value with its to_json() member.
template <typename T>
inline auto to_json(json_writer& j, const T& obj) -> decltype(obj.to_json(j))
{
  obj.to_json(j);
}
/// Writes the value of a generated type without to_json() member through the table of its codecs.
template <typename T, std::enable_if_t<not detail::has_to_json_member_v<T>, int> = 0>
inline auto to_json(json_writer& j, const T& obj) -> decltype(table::to_json_value(j, &obj, T::desc))
{
  table::to_json_value(j, &obj, T::desc);
}
/// Writes the value as the string of its to_string() member.
template <typename T>
inline auto to_json(json_writer& j, const T& obj) -> decltype(j.write_str(obj.to_string()))
{
  j.write_str(obj.to_string());
}

// Bit strings have both to_json() and to_string(), for which the overloads above are ambiguous.
template <uint32_t LB, uint32_t UB, bool ext, bool aligned>
inline void to_json(json_writer& j, const bitstring<LB, UB, ext, aligned>& obj)
{
  obj.to_json(j);
}

template <class IntType, IntType LB, IntType UB, bool Ext, bool Al>
inline void to_json(json_writer& j, const integer<IntType, LB, UB, Ext, Al>& obj)
{
  // Values of unsigned 64-bit integers may not fit in the int64_t of write_int.
  if constexpr (std::is_same_v<IntType, uint64_t>) {
    j.write_uint(obj.value);
  } else {
    j.write_int(obj.value);
  }
}

template <class IntType, std::enable_if_t<std::is_integral_v<IntType>, int> = 0>
inline void to_json(json_writer& j, IntType number)
{
  // Values of unsigned 64-bit integers may not fit in the int64_t of write_int.
  if constexpr (std::is_unsigned_v<IntType> and sizeof(IntType) == sizeof(uint64_t)) {
    j.write_uint(number);
  } else {
    j.write_int(number);
  }
}

template <typename T>
inline void to_json(json_writer& j, const asn1::dyn_array<T>& lst)
{
  j.start_array();
  for (const auto& o : lst) {
    to_json(j, o);
  }
  j.end_array();
}

template <typename T, size_t N>
inline void to_json(json_writer& j, const std::array<T, N>& lst)
{
  j.start_array();
  for (const auto& o : lst) {
    to_json(j, o);
  }
  j.end_array();
}

// The items are written with the asn1::to_json overloads above.
template <class ItemType, uint32_t lb, uint32_t ub, bool aligned>
void dyn_seq_of<ItemType, lb, ub, aligned>::to_json(json_writer& j) const
{
  j.start_array();
  for (const ItemType& item : *this) {
    asn1::to_json(j, item);
  }
  j.end_array();
}

} // namespace asn1
