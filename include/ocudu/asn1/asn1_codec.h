// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Tables and engine of the table-driven codecs of the generated ASN.1 sources.

#pragma once

#include "ocudu/asn1/asn1_dispatch.h"
#include <any>
#include <cstring>
#include <limits>
#include <type_traits>

namespace asn1 {

/*********************
  table-driven SEQUENCE
*********************/

namespace table {

struct seq_field;
struct seq_desc;

/// Packs and unpacks a field through the type of the field.
struct field_codec {
  OCUDUASN_CODE (*pack)(bit_ref& bref, const void* field, const seq_field& desc);
  OCUDUASN_CODE (*unpack)(void* field, cbit_ref& bref, const seq_field& desc);
  /// Whether a list field is empty. Null for other fields.
  bool (*empty)(const void* field);
  /// Whether a copy_ptr field holds a value. Null for other fields.
  bool (*present)(const void* field);
  /// Allocates or frees the value of a copy_ptr field. Null for other fields.
  void (*set_present)(void* field, bool present);
  /// Writes the field as JSON under the given name. Null when the table does not drive the JSON of the field.
  void (*to_json)(json_writer& j, const char* name, const void* field, const seq_field& desc);
  /// Value that the field holds, which the engine encodes by the op of the field: the value of a copy_ptr of op
  /// sequence, or of a std::any. Null for other fields.
  const void* (*get)(const void* field) = nullptr;
  /// Stores a default value in a std::any field. Null for other fields.
  void (*init)(void* field) = nullptr;
  /// Writable value that the field holds, as get. Null when get is.
  void* (*get_mut)(void* field) = nullptr;
};

/// JSON name of a field, and the value that fields without a value write: an enumerator name, or null when absent.
struct field_name {
  const char* name;
  const char* value;
};

/// Integer type that holds a value.
enum class int_type : uint8_t { u8, u16, u32, u64, i8, i16, i32, i64 };

/// How the engine encodes the value of a field, with the integer type that holds an INTEGER or ENUMERATED value.
/// Fields of op none encode only their presence bit. Fields of op sequence, choice and ie_container are values that
/// the engine encodes with their own table.
enum class field_op : uint8_t {
  codec,
  boolean,
  none,
  sequence,
  choice,
  ie_container,
  int_u8,
  int_u16,
  int_u32,
  int_u64,
  int_i8,
  int_i16,
  int_i32,
  int_i64,
  enum_u8,
  enum_u16,
  enum_u32,
  enum_u64,
};

/// Field of a SEQUENCE or of one of its extension additions.
struct seq_field {
  /// Mandatory field, which has no presence flag.
  static constexpr int32_t mandatory = -1;
  /// Optional list, which is present when it is not empty.
  static constexpr int32_t list_presence = -2;
  /// Optional copy_ptr field, which is present when it holds a value.
  static constexpr int32_t ptr_presence = -3;
  /// Mandatory copy_ptr field.
  static constexpr int32_t ptr_mandatory = -4;

  /// Upper bound, modulo 2^64.
  uint64_t ub() const { return static_cast<uint64_t>(lb) + range; }

  /// Offset of the field from the start of the SEQUENCE.
  uint32_t offset;
  /// Offset of the presence flag, or one of the special values above.
  int32_t  presence;
  field_op op;
  bool     has_ext;
  bool     aligned;
  /// Lower bound, or number of values of an ENUMERATED.
  int64_t lb;
  /// Upper bound minus lb, or number of extension values of an ENUMERATED.
  uint64_t           range;
  const field_codec* codec;
  /// Table of a field of op sequence, a seq_desc, of op choice, a choice_desc, of op ie_container, an
  /// ie_container_desc, or of the items of a list of sequences, or the enum_desc of an ENUMERATED field without a JSON
  /// codec. Its codec, if any, holds the presence
  /// functions of a copy_ptr field.
  const void* nested = nullptr;
};

/// Optional field of a field_set, which has a presence bit.
struct seq_opt {
  /// Index of the field in the field_set.
  uint32_t field;
  /// Presence of the field, as in seq_field.
  int32_t presence;
};

/// Fields of a SEQUENCE or of an extension addition, in encoding order.
struct field_set {
  /// Number of fields up to which the engine tracks presence in a 64-bit mask.
  static constexpr uint32_t max_masked_fields = 64;

  const seq_field* fields;
  /// Fields without a presence bit, one bit per field index. Valid up to max_masked_fields fields.
  uint64_t mandatory;
  /// Optional fields, in encoding order.
  const seq_opt* opts;
  /// JSON names of the fields, or null when the table does not drive the JSON of the SEQUENCE.
  const field_name* names;
  uint32_t          nof_fields;
  uint32_t          nof_opts;
};

/// Extension addition: an extension group, or a single field outside of groups.
struct seq_group {
  field_set set;
  /// Whether the addition is a single field outside of groups, which encodes only its value.
  bool single;
};

/// SEQUENCE.
struct seq_desc {
  /// Number of extension additions up to which the engine tracks their presence in a 64-bit mask.
  static constexpr uint32_t max_groups = 64;

  /// Root fields.
  field_set root;
  bool      has_ext;
  bool      aligned;
  /// Extension additions, in encoding order, at most max_groups.
  const seq_group* groups     = nullptr;
  uint32_t         nof_groups = 0;
  /// Name of the message type, for the JSON of a message, or null for other sequences.
  const char* message_name = nullptr;
};

OCUDUASN_CODE pack_sequence(bit_ref& bref, const void* seq, const seq_desc& desc);
OCUDUASN_CODE unpack_sequence(void* seq, cbit_ref& bref, const seq_desc& desc);
/// Writes the SEQUENCE as a JSON object. The table must have the JSON names of its fields.
void to_json_sequence(json_writer& j, const void* seq, const seq_desc& desc);

/// Packs a message, a SEQUENCE padded with zeros to a whole number of bytes.
OCUDUASN_CODE pack_message(bit_ref& bref, const void* msg, const seq_desc& desc);
/// Unpacks a message, a SEQUENCE padded to a whole number of bytes.
OCUDUASN_CODE unpack_message(void* msg, cbit_ref& bref, const seq_desc& desc);
/// Writes a message as a JSON array with one object, which holds the SEQUENCE under the name of the message type.
void to_json_message(json_writer& j, const void* msg, const seq_desc& desc, const char* name);

/// Table of a SEQUENCE without fields, which encodes nothing and writes an empty JSON object.
inline constexpr seq_desc empty_sequence{{nullptr, 0, nullptr, nullptr, 0, 0}, false, false};

struct choice_storage_desc;

/// CHOICE.
struct choice_desc {
  /// Offset of a member that the CHOICE does not have.
  static constexpr uint32_t no_member = std::numeric_limits<uint32_t>::max();

  /// Integer type that holds the value of the type of the active alternative, an ENUMERATED.
  int_type type_itype;
  /// Number of alternatives and of extension alternatives, which come last.
  uint32_t nof_types;
  uint32_t nof_exts;
  bool     has_ext;
  bool     aligned;
  /// One entry per alternative, in the order of the type, with offset 0 from the storage of the alternative.
  const seq_field* alternatives;
  /// Makes the given alternative the active one. Null when the engine sets it, or for a CHOICE with a single
  /// alternative.
  void (*set)(void* choice, uint32_t type);
  /// JSON names of the alternatives, or null when the table does not drive the JSON of the CHOICE.
  const field_name* names = nullptr;
  /// Name of the CHOICE type, for the log of an invalid type.
  const char* name = nullptr;
  /// Offsets of the type and of the storage of the active alternative in the CHOICE, or no_member when the CHOICE has
  /// none.
  uint32_t type_offset    = no_member;
  uint32_t storage_offset = no_member;
  /// Whether the JSON of the CHOICE is the value of its alternative without its name, as for most open types.
  bool open_type_json = false;
  /// Operations on the alternatives of a CHOICE with choice_buffer_t storage, or null.
  const choice_storage_desc* storage = nullptr;
  /// Whether the CHOICE is an open type, whose alternative the object set selects and which is encoded with a length.
  bool open_type = false;
};

/// Operations on the value of one alternative of a CHOICE with choice_buffer_t storage. Null when they do nothing.
struct storage_ops {
  void (*destroy)(void* storage);
  /// Stores a default value in the storage.
  void (*init)(void* storage);
  /// Stores a copy of other in the storage, which holds no value. Null when a copy of the bytes of other does it.
  void (*copy)(void* storage, const void* other);
  /// Moves other into the storage, which holds no value, and destroys other. Null when a copy of the bytes of other
  /// does it.
  void (*relocate)(void* storage, void* other);
  /// Size of the value.
  uint32_t size;
};
template <class T>
void storage_destroy(void* storage)
{
  static_cast<T*>(storage)->~T();
}
template <class T>
void storage_init(void* storage)
{
  new (storage) T();
}
template <class T>
void storage_copy(void* storage, const void* other)
{
  new (storage) T(*static_cast<const T*>(other));
}
template <class T>
void storage_relocate(void* storage, void* other)
{
  T& value = *static_cast<T*>(other);
  new (storage) T(std::move(value));
  value.~T();
}
/// Returns the operation Op of T, or null when Skip holds, without instantiating Op.
template <bool Skip, class Fn, Fn Op>
constexpr Fn storage_op()
{
  if constexpr (Skip) {
    return nullptr;
  } else {
    return Op;
  }
}
/// Operations on an alternative of type T. Plain values, such as integers and enums, are not initialized. Relocatable
/// tells that a copy of the bytes of a value is a valid value, which the old one no longer owns.
template <class T, bool Plain, bool Relocatable = true>
struct alternative_ops {
  static constexpr storage_ops value{
      storage_op<std::is_trivially_destructible_v<T>, void (*)(void*), &storage_destroy<T>>(),
      storage_op<Plain, void (*)(void*), &storage_init<T>>(),
      storage_op<std::is_trivially_copyable_v<T>, void (*)(void*, const void*), &storage_copy<T>>(),
      storage_op<Relocatable or std::is_trivially_copyable_v<T>, void (*)(void*, void*), &storage_relocate<T>>(),
      sizeof(T)};
};

/// Stores a default value of the alternative of the given type in the std::any storage of a CHOICE.
void set_any_choice(void* storage, uint32_t type, const choice_desc& desc);
/// Packs a CHOICE, given its type, null with a single alternative, and the storage of its active alternative, null
/// without storage.
OCUDUASN_CODE pack_choice(bit_ref& bref, const void* type, const void* value, const choice_desc& desc);
/// Unpacks a CHOICE into choice, given the storage of its alternatives, which is null without storage.
OCUDUASN_CODE unpack_choice(void* choice, void* value, cbit_ref& bref, const choice_desc& desc);
/// Writes a CHOICE as a JSON object, given its type and the storage of its active alternative. The table must have the
/// JSON names of its alternatives.
void to_json_choice(json_writer& j, const void* type, const void* value, const choice_desc& desc);
/// Packs the value of an open type of an information object class, the alternative that its type selects, as an
/// open type. The type is null with a single alternative, and the table must have the name of the open type.
OCUDUASN_CODE pack_open_type(bit_ref& bref, const void* type, const void* value, const choice_desc& desc);
/// Unpacks the value of an open type of an information object class into the alternative that its type, set before,
/// selects.
OCUDUASN_CODE unpack_open_type(const void* type, void* value, cbit_ref& bref, const choice_desc& desc);
/// Writes the value of an open type of an information object class as JSON, without the name of its alternative. The
/// table must have the JSON names of its alternatives.
void to_json_open_type(json_writer& j, const void* type, const void* value, const choice_desc& desc);

template <class T>
void choice_set(void* choice, uint32_t type)
{
  static_cast<T*>(choice)->set(static_cast<typename T::types::options>(type));
}

template <class T>
OCUDUASN_CODE pack_object(bit_ref& bref, const void* field, const seq_field&)
{
  return asn1::pack(bref, *static_cast<const T*>(field));
}
template <class T>
OCUDUASN_CODE unpack_object(void* field, cbit_ref& bref, const seq_field&)
{
  return asn1::unpack(*static_cast<T*>(field), bref);
}
/// Items of a SEQUENCE OF that pack themselves. A list of fixed size encodes no size.
template <bool Fixed>
struct object_items {
  template <class L>
  static OCUDUASN_CODE pack(bit_ref& bref, const L& list, const seq_field& desc)
  {
    if constexpr (Fixed) {
      return pack_fixed_seq_of(bref, list, list.size());
    } else {
      return pack_dyn_seq_of(bref, list, desc.lb, desc.ub(), desc.aligned);
    }
  }
  template <class L>
  static OCUDUASN_CODE unpack(L& list, cbit_ref& bref, const seq_field& desc)
  {
    if constexpr (Fixed) {
      return unpack_fixed_seq_of(list, bref, list.size());
    } else {
      return unpack_dyn_seq_of(list, bref, desc.lb, desc.ub(), desc.aligned);
    }
  }
};
/// Primitive type of integer list items, which are a primitive type or an asn1::integer.
template <class T, class = void>
struct int_item_prim {
  using type = T;
};
template <class T>
struct int_item_prim<T, std::void_t<decltype(T::value)>> {
  using type = decltype(T::value);
};

/// Packer of asn1::integer items through the integer_packer of their primitive type.
template <class Item, class Prim>
struct wrapped_int_packer {
  integer_packer<Prim> prim;

  OCUDUASN_CODE pack(bit_ref& bref, const Item& item) const { return prim.pack(bref, item.value); }
  OCUDUASN_CODE unpack(Item& item, cbit_ref& bref) const { return prim.unpack(item.value, bref); }
};

/// Constrained integer items of a SEQUENCE OF, with their bounds, extension marker and alignment.
template <bool Fixed, int64_t LB, uint64_t Range, bool Ext, bool Aligned>
struct int_items {
  template <class L>
  static auto packer()
  {
    using item_type = typename L::value_type;
    using prim_type = typename int_item_prim<item_type>::type;
    integer_packer<prim_type> prim(
        static_cast<prim_type>(LB), static_cast<prim_type>(static_cast<uint64_t>(LB) + Range), Ext, Aligned);
    // The integer_packer of asn1::integer items is not instantiated, unlike those of the primitive types.
    if constexpr (std::is_same_v<item_type, prim_type>) {
      return prim;
    } else {
      return wrapped_int_packer<item_type, prim_type>{prim};
    }
  }
  template <class L>
  static OCUDUASN_CODE pack(bit_ref& bref, const L& list, const seq_field& desc)
  {
    if constexpr (Fixed) {
      return pack_fixed_seq_of(bref, list, list.size(), packer<L>());
    } else {
      return pack_dyn_seq_of(bref, list, desc.lb, desc.ub(), packer<L>(), desc.aligned);
    }
  }
  template <class L>
  static OCUDUASN_CODE unpack(L& list, cbit_ref& bref, const seq_field& desc)
  {
    if constexpr (Fixed) {
      return unpack_fixed_seq_of(list, bref, list.size(), packer<L>());
    } else {
      return unpack_dyn_seq_of(list, bref, desc.lb, desc.ub(), packer<L>(), desc.aligned);
    }
  }
};
/// BOOLEAN items of a SEQUENCE OF, one bit each.
template <bool Fixed>
struct bool_items {
  template <class L>
  static OCUDUASN_CODE pack(bit_ref& bref, const L& list, const seq_field& desc)
  {
    if constexpr (Fixed) {
      return pack_fixed_seq_of(bref, list, list.size(), BitPacker(1));
    } else {
      return pack_dyn_seq_of(bref, list, desc.lb, desc.ub(), BitPacker(1), desc.aligned);
    }
  }
  template <class L>
  static OCUDUASN_CODE unpack(L& list, cbit_ref& bref, const seq_field& desc)
  {
    if constexpr (Fixed) {
      return unpack_fixed_seq_of(list, bref, list.size(), BitPacker(1));
    } else {
      return unpack_dyn_seq_of(list, bref, desc.lb, desc.ub(), BitPacker(1), desc.aligned);
    }
  }
};
/// Packer of the items of a SEQUENCE OF that are lists themselves, whose own items Inner packs.
template <class Inner, uint32_t LB, uint32_t UB, bool Aligned>
struct inner_list_packer {
  static seq_field desc()
  {
    seq_field d{};
    d.lb      = LB;
    d.range   = UB - LB;
    d.aligned = Aligned;
    return d;
  }
  template <class L>
  OCUDUASN_CODE pack(bit_ref& bref, const L& list) const
  {
    return Inner::pack(bref, list, desc());
  }
  template <class L>
  OCUDUASN_CODE unpack(L& list, cbit_ref& bref) const
  {
    return Inner::unpack(list, bref, desc());
  }
};
/// Items of a SEQUENCE OF that are lists, of size LB to UB, whose own items Inner packs.
template <bool Fixed, uint32_t LB, uint32_t UB, bool Aligned, class Inner>
struct list_items {
  using packer = inner_list_packer<Inner, LB, UB, Aligned>;
  template <class L>
  static OCUDUASN_CODE pack(bit_ref& bref, const L& list, const seq_field& desc)
  {
    if constexpr (Fixed) {
      return pack_fixed_seq_of(bref, list, list.size(), packer{});
    } else {
      return pack_dyn_seq_of(bref, list, desc.lb, desc.ub(), packer{}, desc.aligned);
    }
  }
  template <class L>
  static OCUDUASN_CODE unpack(L& list, cbit_ref& bref, const seq_field& desc)
  {
    if constexpr (Fixed) {
      return unpack_fixed_seq_of(list, bref, list.size(), packer{});
    } else {
      return unpack_dyn_seq_of(list, bref, desc.lb, desc.ub(), packer{}, desc.aligned);
    }
  }
};

template <class T, class Items>
OCUDUASN_CODE pack_list(bit_ref& bref, const void* field, const seq_field& desc)
{
  return Items::pack(bref, *static_cast<const T*>(field), desc);
}
template <class T, class Items>
OCUDUASN_CODE unpack_list(void* field, cbit_ref& bref, const seq_field& desc)
{
  return Items::unpack(*static_cast<T*>(field), bref, desc);
}
template <class T>
bool list_empty(const void* field)
{
  return static_cast<const T*>(field)->size() == 0;
}
template <class T, class Items>
OCUDUASN_CODE pack_ptr_list(bit_ref& bref, const void* field, const seq_field& desc)
{
  return Items::pack(bref, **static_cast<const copy_ptr<T>*>(field), desc);
}
template <class T, class Items>
OCUDUASN_CODE unpack_ptr_list(void* field, cbit_ref& bref, const seq_field& desc)
{
  return Items::unpack(**static_cast<copy_ptr<T>*>(field), bref, desc);
}
template <class T>
OCUDUASN_CODE pack_ptr(bit_ref& bref, const void* field, const seq_field&)
{
  return asn1::pack(bref, **static_cast<const copy_ptr<T>*>(field));
}
template <class T>
OCUDUASN_CODE unpack_ptr(void* field, cbit_ref& bref, const seq_field&)
{
  return asn1::unpack(**static_cast<copy_ptr<T>*>(field), bref);
}
/// Whether a copy_ptr, of any type, holds a value.
inline bool ptr_present(const void* field)
{
  // A copy_ptr holds only its pointer, whatever its type.
  static_assert(sizeof(copy_ptr<uint8_t>) == sizeof(void*), "copy_ptr holds more than a pointer");
  const void* ptr;
  std::memcpy(&ptr, field, sizeof(ptr));
  return ptr != nullptr;
}
template <class T>
void ptr_set_present(void* field, bool present)
{
  static_cast<copy_ptr<T>*>(field)->set_present(present);
}

/// Integer type that holds the value of an ENUMERATED type.
template <class EnumType>
constexpr int_type enum_int_type()
{
  constexpr size_t size = sizeof(typename EnumType::options);
  return size == 1 ? int_type::u8 : size == 2 ? int_type::u16 : size == 4 ? int_type::u32 : int_type::u64;
}

/// Field op of an ENUMERATED type.
template <class EnumType>
constexpr field_op enum_op()
{
  constexpr size_t size = sizeof(typename EnumType::options);
  return size == 1   ? field_op::enum_u8
         : size == 2 ? field_op::enum_u16
         : size == 4 ? field_op::enum_u32
                     : field_op::enum_u64;
}

/// JSON form of structured values, which asn1::to_json() writes, after their field name.
struct struct_json {
  template <class T>
  static void named(json_writer& j, const char* name, const T& v)
  {
    j.write_fieldname(name);
    asn1::to_json(j, v);
  }
  template <class T>
  static void item(json_writer& j, const T& v)
  {
    asn1::to_json(j, v);
  }
};
/// JSON form of values written as the string of their to_string().
struct string_json {
  template <class T>
  static void named(json_writer& j, const char* name, const T& v)
  {
    j.write_str(name, v.to_string());
  }
  template <class T>
  static void item(json_writer& j, const T& v)
  {
    j.write_str(v.to_string());
  }
};
/// JSON form of integers.
struct int_json {
  template <class T>
  static void named(json_writer& j, const char* name, const T& v)
  {
    // Values of unsigned 64-bit integers may not fit in the int64_t of write_int.
    if constexpr (std::is_same_v<T, uint64_t>) {
      j.write_uint(name, v);
    } else {
      j.write_int(name, v);
    }
  }
  template <class T>
  static void item(json_writer& j, const T& v)
  {
    if constexpr (std::is_same_v<T, uint64_t>) {
      j.write_uint(v);
    } else {
      j.write_int(v);
    }
  }
};
/// JSON form of REAL values, written as floats.
struct float_json {
  template <class T>
  static void named(json_writer& j, const char* name, const T& v)
  {
    j.write_float(name, v.value);
  }
  template <class T>
  static void item(json_writer& j, const T& v)
  {
    j.write_float(v.value);
  }
};
/// JSON form of BOOLEAN items.
struct bool_json {
  static void item(json_writer& j, bool v) { j.write_bool(v); }
};
/// JSON form of items that are lists, an array of items of JSON form J.
template <class J>
struct list_json {
  template <class L>
  static void item(json_writer& j, const L& list)
  {
    j.start_array();
    for (const auto& item : list) {
      J::item(j, item);
    }
    j.end_array();
  }
};
/// Fields whose JSON the table does not drive.
struct no_json {};

template <class T, class J>
void object_to_json(json_writer& j, const char* name, const void* field, const seq_field&)
{
  J::named(j, name, *static_cast<const T*>(field));
}
template <class T, class J>
void ptr_to_json(json_writer& j, const char* name, const void* field, const seq_field&)
{
  J::named(j, name, **static_cast<const copy_ptr<T>*>(field));
}
template <class J, class L>
void list_items_to_json(json_writer& j, const char* name, const L& list)
{
  j.start_array(name);
  for (const auto& item : list) {
    J::item(j, item);
  }
  j.end_array();
}
template <class T, class J>
void list_to_json(json_writer& j, const char* name, const void* field, const seq_field&)
{
  list_items_to_json<J>(j, name, *static_cast<const T*>(field));
}
template <class T, class J>
void ptr_list_to_json(json_writer& j, const char* name, const void* field, const seq_field&)
{
  list_items_to_json<J>(j, name, **static_cast<const copy_ptr<T>*>(field));
}

/// Codec of a field whose type has pack and unpack methods, with the JSON form J of its value.
template <class T, class J = no_json>
struct object_codec {
  static constexpr field_codec value{&pack_object<T>,
                                     &unpack_object<T>,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     &object_to_json<T, J>};
};
template <class T>
struct object_codec<T, no_json> {
  static constexpr field_codec value{&pack_object<T>, &unpack_object<T>, nullptr, nullptr, nullptr, nullptr};
};

/// Codec of a field whose type has pack and unpack methods, and which is present when it is not empty.
template <class T, class J = no_json>
struct sized_object_codec {
  static constexpr field_codec value{&pack_object<T>,
                                     &unpack_object<T>,
                                     &list_empty<T>,
                                     nullptr,
                                     nullptr,
                                     &object_to_json<T, J>};
};
template <class T>
struct sized_object_codec<T, no_json> {
  static constexpr field_codec value{&pack_object<T>, &unpack_object<T>, &list_empty<T>, nullptr, nullptr, nullptr};
};
/// Codec of a SEQUENCE OF field, with the JSON form J of its items, and Items, which packs them: by default items that
/// pack themselves in a list of variable size.
template <class T, class J = no_json, class Items = object_items<false>>
struct list_codec {
  static constexpr field_codec value{&pack_list<T, Items>,
                                     &unpack_list<T, Items>,
                                     &list_empty<T>,
                                     nullptr,
                                     nullptr,
                                     &list_to_json<T, J>};
};
template <class T, class Items>
struct list_codec<T, no_json, Items> {
  static constexpr field_codec value{&pack_list<T, Items>,
                                     &unpack_list<T, Items>,
                                     &list_empty<T>,
                                     nullptr,
                                     nullptr,
                                     nullptr};
};

/// Pack and unpack an item of a SEQUENCE OF with the table of the item, a seq_desc or a choice_desc, which unpacker or
/// packer points to.
OCUDUASN_CODE pack_sequence_item(bit_ref& bref, const void* item, const void* packer);
OCUDUASN_CODE unpack_sequence_item(void* item, cbit_ref& bref, const void* unpacker);
OCUDUASN_CODE pack_choice_item(bit_ref& bref, const void* item, const void* packer);
OCUDUASN_CODE unpack_choice_item(void* item, cbit_ref& bref, const void* unpacker);
/// Writes an item of a SEQUENCE OF as JSON with its table, a seq_desc or a choice_desc.
void sequence_item_to_json(json_writer& j, const void* item, const void* desc);
void choice_item_to_json(json_writer& j, const void* item, const void* desc);

/// Item functions of the lists whose items the engine encodes with the table of a SEQUENCE or of a CHOICE.
struct sequence_item_ops {
  static constexpr auto pack    = &pack_sequence_item;
  static constexpr auto unpack  = &unpack_sequence_item;
  static constexpr auto to_json = &sequence_item_to_json;
};
struct choice_item_ops {
  static constexpr auto pack    = &pack_choice_item;
  static constexpr auto unpack  = &unpack_choice_item;
  static constexpr auto to_json = &choice_item_to_json;
};
/// Items of a list: the address of the first and their number.
struct list_view {
  const void* data;
  uint32_t    size;
};
template <class T>
list_view view_list(const void* field)
{
  const T& list = *static_cast<const T*>(field);
  return {list.data(), static_cast<uint32_t>(list.size())};
}
/// Codec of a SEQUENCE OF field whose items the engine encodes with their table, in the nested table of the field.
struct item_list_field_codec : field_codec {
  list_view (*view)(const void* field);
  /// Resizes the list and returns its items. Null for lists of fixed size.
  void* (*resize)(void* list, uint32_t nof_items);
  /// Writable items of a list of fixed size. Null for other lists.
  void* (*fixed_items)(void* list);
  uint32_t item_size;
  OCUDUASN_CODE (*pack_item)(bit_ref& bref, const void* item, const void* table);
  OCUDUASN_CODE (*unpack_item)(void* item, cbit_ref& bref, const void* table);
  void (*item_to_json)(json_writer& j, const void* item, const void* table);
};
/// Resize function of a list of type T, null for lists of fixed size.
template <class T, bool Fixed>
constexpr ::asn1::detail::resize_seq_fn list_resize()
{
  if constexpr (Fixed) {
    return nullptr;
  } else {
    return &::asn1::detail::resize_seq_of<T>;
  }
}
template <class T>
void* fixed_list_items(void* field)
{
  return static_cast<T*>(field)->data();
}
/// Writable items function of a list of type T, null for lists of variable size.
template <class T, bool Fixed>
constexpr void* (*list_fixed_items())(void*)
{
  if constexpr (Fixed) {
    return &fixed_list_items<T>;
  } else {
    return nullptr;
  }
}
OCUDUASN_CODE pack_item_list(bit_ref& bref, const void* field, const seq_field& desc);
OCUDUASN_CODE unpack_item_list(void* field, cbit_ref& bref, const seq_field& desc);
void          item_list_to_json(json_writer& j, const char* name, const void* field, const seq_field& desc);
/// Codec of a SEQUENCE OF field of type T whose items the engine encodes with their table, in the nested table of the
/// descriptor: Ops gives the item functions of a SEQUENCE or a CHOICE table. Fixed tells that the list has a fixed
/// size, and Json that the table drives its JSON.
template <class T, class Ops, bool Fixed, bool Json>
struct item_list_codec {
  static constexpr item_list_field_codec value{
      {&pack_item_list, &unpack_item_list, &list_empty<T>, nullptr, nullptr, Json ? &item_list_to_json : nullptr},
      &view_list<T>,
      list_resize<T, Fixed>(),
      list_fixed_items<T, Fixed>(),
      sizeof(typename T::value_type),
      Ops::pack,
      Ops::unpack,
      Ops::to_json};
};
template <class T>
list_view view_ptr_list(const void* field)
{
  return view_list<T>(static_cast<const copy_ptr<T>*>(field)->get());
}
template <class T>
void* resize_ptr_list(void* field, uint32_t nof_items)
{
  return ::asn1::detail::resize_seq_of<T>(static_cast<copy_ptr<T>*>(field)->get(), nof_items);
}
template <class T, bool Fixed>
constexpr ::asn1::detail::resize_seq_fn ptr_list_resize()
{
  if constexpr (Fixed) {
    return nullptr;
  } else {
    return &resize_ptr_list<T>;
  }
}
template <class T>
void* ptr_list_items(void* field)
{
  return static_cast<copy_ptr<T>*>(field)->get()->data();
}
template <class T, bool Fixed>
constexpr void* (*ptr_list_fixed_items())(void*)
{
  if constexpr (Fixed) {
    return &ptr_list_items<T>;
  } else {
    return nullptr;
  }
}
/// Codec of a copy_ptr field P that holds a SEQUENCE OF, as item_list_codec.
template <class P, class Ops, bool Fixed, bool Json>
struct ptr_item_list_codec;
template <class T, class Ops, bool Fixed, bool Json>
struct ptr_item_list_codec<copy_ptr<T>, Ops, Fixed, Json> {
  static constexpr item_list_field_codec value{{&pack_item_list,
                                                &unpack_item_list,
                                                nullptr,
                                                &ptr_present,
                                                &ptr_set_present<T>,
                                                Json ? &item_list_to_json : nullptr},
                                               &view_ptr_list<T>,
                                               ptr_list_resize<T, Fixed>(),
                                               ptr_list_fixed_items<T, Fixed>(),
                                               sizeof(typename T::value_type),
                                               Ops::pack,
                                               Ops::unpack,
                                               Ops::to_json};
};
/// Codec of a copy_ptr field whose type has pack and unpack methods, with the JSON form J of its value.
template <class P, class J = no_json>
struct ptr_codec;
template <class T, class J>
struct ptr_codec<copy_ptr<T>, J> {
  static constexpr field_codec value{&pack_ptr<T>,
                                     &unpack_ptr<T>,
                                     nullptr,
                                     &ptr_present,
                                     &ptr_set_present<T>,
                                     &ptr_to_json<T, J>};
};
template <class T>
struct ptr_codec<copy_ptr<T>, no_json> {
  static constexpr field_codec value{&pack_ptr<T>, &unpack_ptr<T>, nullptr, &ptr_present, &ptr_set_present<T>, nullptr};
};
/// Codec of a copy_ptr field that holds a SEQUENCE OF, with the JSON form J of its items, and Items, which packs them.
template <class P, class J = no_json, class Items = object_items<false>>
struct ptr_list_codec;
template <class T, class J, class Items>
struct ptr_list_codec<copy_ptr<T>, J, Items> {
  static constexpr field_codec value{&pack_ptr_list<T, Items>,
                                     &unpack_ptr_list<T, Items>,
                                     nullptr,
                                     &ptr_present,
                                     &ptr_set_present<T>,
                                     &ptr_list_to_json<T, J>};
};
template <class T, class Items>
struct ptr_list_codec<copy_ptr<T>, no_json, Items> {
  static constexpr field_codec value{&pack_ptr_list<T, Items>,
                                     &unpack_ptr_list<T, Items>,
                                     nullptr,
                                     &ptr_present,
                                     &ptr_set_present<T>,
                                     nullptr};
};
template <class T>
OCUDUASN_CODE pack_any(bit_ref& bref, const void* field, const seq_field&)
{
  return asn1::pack(bref, static_cast<const choice_buffer_ptr*>(field)->get<T>());
}
template <class T>
OCUDUASN_CODE unpack_any(void* field, cbit_ref& bref, const seq_field&)
{
  return asn1::unpack(static_cast<choice_buffer_ptr*>(field)->get<T>(), bref);
}
template <class T, class J>
void any_to_json(json_writer& j, const char* name, const void* field, const seq_field&)
{
  J::named(j, name, static_cast<const choice_buffer_ptr*>(field)->get<T>());
}
template <class T>
void any_init(void* field)
{
  *static_cast<choice_buffer_ptr*>(field) = T{};
}
/// Codec of a CHOICE alternative of type T held in a choice_buffer_ptr, whose type has pack and unpack methods, with
/// the JSON form J of its value.
template <class T, class J = no_json>
struct any_codec {
  static constexpr field_codec
      value{&pack_any<T>, &unpack_any<T>, nullptr, nullptr, nullptr, &any_to_json<T, J>, nullptr, &any_init<T>};
};
template <class T>
struct any_codec<T, no_json> {
  static constexpr field_codec
      value{&pack_any<T>, &unpack_any<T>, nullptr, nullptr, nullptr, nullptr, nullptr, &any_init<T>};
};
template <class T>
const void* any_get(const void* field)
{
  return &static_cast<const choice_buffer_ptr*>(field)->get<T>();
}
template <class T>
void* any_get_mut(void* field)
{
  return &static_cast<choice_buffer_ptr*>(field)->get<T>();
}
/// Codec of a CHOICE alternative of type T held in a choice_buffer_ptr, whose value the engine encodes by the op of
/// the field, such as an integer, with the JSON form J of an ENUMERATED value.
template <class T, class J = no_json>
struct any_value_codec {
  static constexpr field_codec value{nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     &object_to_json<T, J>,
                                     &any_get<T>,
                                     &any_init<T>,
                                     &any_get_mut<T>};
};
template <class T>
struct any_value_codec<T, no_json> {
  static constexpr field_codec
      value{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &any_get<T>, &any_init<T>, &any_get_mut<T>};
};
template <class T, class Items>
OCUDUASN_CODE pack_any_list(bit_ref& bref, const void* field, const seq_field& desc)
{
  return Items::pack(bref, static_cast<const choice_buffer_ptr*>(field)->get<T>(), desc);
}
template <class T, class Items>
OCUDUASN_CODE unpack_any_list(void* field, cbit_ref& bref, const seq_field& desc)
{
  return Items::unpack(static_cast<choice_buffer_ptr*>(field)->get<T>(), bref, desc);
}
template <class T, class J>
void any_list_to_json(json_writer& j, const char* name, const void* field, const seq_field&)
{
  list_items_to_json<J>(j, name, static_cast<const choice_buffer_ptr*>(field)->get<T>());
}
/// Codec of a CHOICE alternative held in a choice_buffer_ptr that is a SEQUENCE OF of type T, with the JSON form J of
/// its items, and Items, which packs them.
template <class T, class J = no_json, class Items = object_items<false>>
struct any_list_codec {
  static constexpr field_codec value{&pack_any_list<T, Items>,
                                     &unpack_any_list<T, Items>,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     &any_list_to_json<T, J>,
                                     nullptr,
                                     &any_init<T>};
};
template <class T, class Items>
struct any_list_codec<T, no_json, Items> {
  static constexpr field_codec value{&pack_any_list<T, Items>,
                                     &unpack_any_list<T, Items>,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     &any_init<T>};
};
template <class T>
const void* ptr_get(const void* field)
{
  return static_cast<const copy_ptr<T>*>(field)->get();
}
template <class T>
void* ptr_get_mut(void* field)
{
  return static_cast<copy_ptr<T>*>(field)->get();
}
/// Presence of a copy_ptr field that holds a nested sequence of op sequence, which the engine encodes.
template <class P>
struct ptr_nested_codec;
template <class T>
struct ptr_nested_codec<copy_ptr<T>> {
  static constexpr field_codec value{nullptr,
                                     nullptr,
                                     nullptr,
                                     &ptr_present,
                                     &ptr_set_present<T>,
                                     nullptr,
                                     &ptr_get<T>,
                                     nullptr,
                                     &ptr_get_mut<T>};
};

} // namespace table

} // namespace asn1
