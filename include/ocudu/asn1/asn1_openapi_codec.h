// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Table-driven codec of values carried in a JSON document whose shape an OpenAPI schema defines.
///
/// The tables of asn1_codec.h hold the shape of a type: the offset, presence and JSON name of every field. This codec
/// walks those tables to read and write nlohmann JSON, and adds the two things an OpenAPI schema needs and an ASN.1
/// module does not: a codec for the values the schema renders as strings, and a key set that selects one alternative
/// of a CHOICE. The JSON is not the ASN.1 JSON Encoding Rules: a CHOICE carries no name wrapper, and the field names
/// are the ones the schema gives, not ASN.1 identifiers.

#pragma once

#include "nlohmann/json.hpp"
#include "ocudu/asn1/asn1_codec.h"
#include "ocudu/asn1/asn1_types.h"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>

namespace asn1 {
namespace openapi {

/// Where a read is in the JSON document, and how strict it is.
struct read_context {
  /// Whether a key that a SEQUENCE does not have fails the read.
  bool reject_unknown_keys = false;
  /// Path of the value that the read is at, which the log of a failure names.
  std::string path;
  /// Name that a member of an enclosing object gave for the alternative of the CHOICE that it holds, or empty.
  std::string type_name;
};

/// Reads and writes one value whose field is of op codec.
struct value_codec {
  /// Returns false when the value cannot be written, which leaves the document without it.
  bool (*write)(nlohmann::ordered_json& out, const void* value);
  /// Returns false when the JSON does not hold a value of the type.
  bool (*read)(const nlohmann::ordered_json& in, void* value, read_context& ctx);
  /// Empties a list, which is how a list field is made absent. Null for a value that is not a list.
  void (*clear)(void* value) = nullptr;
};

/// OpenAPI table of one field of a SEQUENCE, in the order of the root fields of its shape.
struct field_desc {
  /// Codec of the value of a field of op codec, and null for the other ops.
  const value_codec* value = nullptr;
  /// Table of a field of op sequence, a seq_desc, or of op choice, a choice_desc, and null for the other ops.
  const void* nested = nullptr;
  /// Whether the string that this field holds names the alternative of a CHOICE that the object carries. A schema
  /// that leaves the alternatives of a oneOf without a name of their own tells them apart by their keys, which do
  /// not always differ, and names the one it holds in a member of the enclosing object instead.
  bool names_type = false;
};

/// SEQUENCE in a JSON document whose shape an OpenAPI schema defines.
struct seq_desc {
  /// Shape of the SEQUENCE: the offset, presence and JSON name of every root field.
  const table::seq_desc* shape;
  /// One entry per root field of the shape.
  const field_desc* fields;
  /// Number of entries that fields holds, which the codec checks against the shape before it indexes either of them.
  uint32_t nof_fields;

  /// Takes the length of the table from the array itself, so that the two cannot be given apart.
  template <size_t N>
  constexpr seq_desc(const table::seq_desc* shape_, const field_desc (&fields_)[N]) :
    shape(shape_), fields(fields_), nof_fields(N)
  {
  }
};

/// Alternative of a CHOICE in a JSON document whose shape an OpenAPI schema defines.
struct alternative_desc {
  /// Codec of the value of an alternative of op codec, and null for the other ops.
  const value_codec* value = nullptr;
  /// Table of an alternative of op sequence, a seq_desc, or of op choice, a choice_desc, and null for the other ops.
  const void* nested = nullptr;
  /// Every key that a JSON object of this alternative may hold, which is every property the schema gives it.
  const char* const* keys     = nullptr;
  uint32_t           nof_keys = 0;
};

/// CHOICE in a JSON document whose shape an OpenAPI schema defines.
struct choice_desc {
  /// Shape of the CHOICE: the type and storage of its alternatives, and their JSON names.
  const table::choice_desc* shape;
  /// One entry per alternative of the shape, in the order of the alternatives.
  const alternative_desc* alternatives;
  /// Number of entries that alternatives holds, which the codec checks against the shape before it indexes either.
  uint32_t nof_alternatives;
  /// Whether the JSON of the CHOICE is an object keyed by the name of the active alternative. A oneOf of an OpenAPI
  /// schema carries no such key, so the JSON is the value of the alternative on its own.
  bool wrapped;

  /// Takes the length of the table from the array itself, so that the two cannot be given apart.
  template <size_t N>
  constexpr choice_desc(const table::choice_desc* shape_,
                        const alternative_desc (&alternatives_)[N],
                        bool wrapped_ = false) :
    shape(shape_), alternatives(alternatives_), nof_alternatives(N), wrapped(wrapped_)
  {
  }
};

/// Writes the SEQUENCE as a JSON object.
///
/// Returns false when the table does not describe the SEQUENCE, a nested value has no table or no codec, or a nested
/// value fails to write. A write that fails leaves the document holding whatever it had written before it stopped,
/// which the caller does not send.
bool to_json(nlohmann::ordered_json& out, const void* value, const seq_desc& desc);

/// Reads the SEQUENCE from a JSON object, naming the path of a failure in the log.
///
/// Returns false when the JSON holds no object, a mandatory field is absent, a value falls outside the bounds of its
/// field, a nested value fails to read, or the context rejects unknown keys and the object holds one. A read that
/// fails leaves the fields it had already read in the value, and leaves the path of the failure in the context.
bool from_json(const nlohmann::ordered_json& in, void* value, const seq_desc& desc, read_context& ctx);

/// Reads the SEQUENCE from a JSON object, accepting a key that the SEQUENCE does not have.
bool from_json(const nlohmann::ordered_json& in, void* value, const seq_desc& desc);

/// Writes the active alternative of the CHOICE, under its name when the CHOICE is wrapped.
///
/// Returns false when the CHOICE holds no alternative, when a wrapped one has no names, or when the value of the
/// alternative fails to write.
bool to_json(nlohmann::ordered_json& out, const void* value, const choice_desc& desc);

/// Reads the CHOICE from the JSON, naming the path of a failure in the log.
///
/// An unwrapped CHOICE selects the alternative that accepts every key of the JSON object and accepts the fewest keys
/// in all, which is the alternative that fits the object most closely. A schema leaves the properties of an
/// alternative optional, so the keys an object holds cannot select it on their own.
///
/// Returns false when no alternative accepts every key of the object, or the value of the alternative fails to read.
bool from_json(const nlohmann::ordered_json& in, void* value, const choice_desc& desc, read_context& ctx);

/// Reads the CHOICE from the JSON, accepting a key that no alternative has.
bool from_json(const nlohmann::ordered_json& in, void* value, const choice_desc& desc);

/// Appends the index of a list item to the path of a read, and returns the length of the path before it.
size_t push_item(read_context& ctx, uint32_t index);

/// Writes the value of a type that holds its own table, as asn1::pack packs one through the table it holds.
template <typename T>
bool to_json(nlohmann::ordered_json& out, const T& value)
{
  return to_json(out, &value, T::openapi);
}

/// Reads the value of a type that holds its own table, naming the path of a failure in the log.
template <typename T>
bool from_json(const nlohmann::ordered_json& in, T& value, read_context& ctx)
{
  return from_json(in, &value, T::openapi, ctx);
}

/// Reads the value of a type that holds its own table, accepting a key that the type does not have.
template <typename T>
bool from_json(const nlohmann::ordered_json& in, T& value)
{
  return from_json(in, &value, T::openapi);
}

/// Writes the value of a type whose table the caller holds, and leaves the document empty when it cannot.
template <typename T>
nlohmann::ordered_json to_json(const T& value, const seq_desc& desc)
{
  nlohmann::ordered_json out;
  if (not to_json(out, &value, desc)) {
    return nlohmann::ordered_json::object();
  }
  return out;
}

/// Reads the value of a type whose table the caller holds.
template <typename T>
bool from_json(const nlohmann::ordered_json& in, T& value, const seq_desc& desc)
{
  return from_json(in, &value, desc);
}

/// Codec of a value that writes itself as the string of its to_string() and reads itself with its from_string().
///
/// A from_string() says nothing when it cannot hold what it was given: one of a fixed size leaves the value empty,
/// and one of a BIT STRING or an OCTET STRING takes a character outside its alphabet as a zero. The read therefore
/// writes the value back and fails when that is not what it was given, which covers the length and the characters
/// alike. A hexadecimal string comes back in lower case, which is the same value, so the two are compared without
/// regard to case.
///
/// A character string holds whatever it is given, so the comparison cannot fail for one and the specialization below
/// checks it against the bounds of its type instead.
template <typename T>
struct string_value {
  static bool write(nlohmann::ordered_json& out, const void* value)
  {
    out = static_cast<const T*>(value)->to_string();
    return true;
  }
  static bool read(const nlohmann::ordered_json& in, void* value, read_context& ctx)
  {
    if (not in.is_string()) {
      log_error("The value at '{}' is not a string.", ctx.path);
      return false;
    }
    auto read_str = in.get<std::string>();
    static_cast<T*>(value)->from_string(read_str);
    std::string stored = static_cast<const T*>(value)->to_string();
    auto        same   = [](char held, char given) {
      return std::tolower(static_cast<unsigned char>(held)) == std::tolower(static_cast<unsigned char>(given));
    };
    if (stored.size() != read_str.size() or not std::equal(stored.begin(), stored.end(), read_str.begin(), same)) {
      log_error("The string '{}' at '{}' is not a value of its type, which holds '{}'.", read_str, ctx.path, stored);
      return false;
    }
    return true;
  }

  static constexpr value_codec codec{&write, &read};
};

/// Codec of an ASN.1 character string, which holds whatever it is given.
///
/// Its from_string() neither shortens what is too long nor refuses a character its alphabet does not carry, so the
/// read checks the string against the length and the alphabet of the type before it stores it. The alphabet is the
/// range of characters that the type encodes, which is what a value of it may hold.
template <uint32_t LB, uint32_t UB, uint32_t ALB, uint32_t AUB, bool ext, bool aligned>
struct string_value<asn_string<LB, UB, ALB, AUB, ext, aligned>> {
  using string_type = asn_string<LB, UB, ALB, AUB, ext, aligned>;

  static bool write(nlohmann::ordered_json& out, const void* value)
  {
    out = static_cast<const string_type*>(value)->to_string();
    return true;
  }
  static bool read(const nlohmann::ordered_json& in, void* value, read_context& ctx)
  {
    if (not in.is_string()) {
      log_error("The value at '{}' is not a string.", ctx.path);
      return false;
    }
    auto read_str = in.get<std::string>();
    if (read_str.size() < ALB or read_str.size() > AUB) {
      log_error("The string at '{}' is {} characters long, outside the bounds [{}, {}] of its type.",
                ctx.path,
                read_str.size(),
                ALB,
                AUB);
      return false;
    }
    for (char c : read_str) {
      auto code = static_cast<uint32_t>(static_cast<unsigned char>(c));
      if (code < LB or code > UB) {
        log_error("The string at '{}' holds the character '{}', which its alphabet does not carry.", ctx.path, c);
        return false;
      }
    }
    static_cast<string_type*>(value)->from_string(read_str);
    return true;
  }

  static constexpr value_codec codec{&write, &read};
};

/// Codec of an OCTET STRING of no fixed size, which a field may carry as an optional list.
///
/// It is the same codec as the one above and adds the one thing a list field needs: a way to empty the value, which
/// is what marks such a field absent when a later read does not carry it.
template <bool aligned>
struct string_value<unbounded_octstring<aligned>> {
  using octstring_type = unbounded_octstring<aligned>;

  static bool write(nlohmann::ordered_json& out, const void* value)
  {
    out = static_cast<const octstring_type*>(value)->to_string();
    return true;
  }
  static bool read(const nlohmann::ordered_json& in, void* value, read_context& ctx)
  {
    if (not in.is_string()) {
      log_error("The value at '{}' is not a string.", ctx.path);
      return false;
    }
    auto read_str = in.get<std::string>();
    static_cast<octstring_type*>(value)->from_string(read_str);
    std::string stored = static_cast<const octstring_type*>(value)->to_string();
    auto        same   = [](char held, char given) {
      return std::tolower(static_cast<unsigned char>(held)) == std::tolower(static_cast<unsigned char>(given));
    };
    if (stored.size() != read_str.size() or not std::equal(stored.begin(), stored.end(), read_str.begin(), same)) {
      log_error("The string '{}' at '{}' is not a value of its type, which holds '{}'.", read_str, ctx.path, stored);
      return false;
    }
    return true;
  }

  static void clear(void* value) { static_cast<octstring_type*>(value)->clear(); }

  static constexpr value_codec codec{&write, &read, &clear};
};

/// Codec of a string whose schema bounds its length to MinLen and MaxLen characters.
template <typename T, uint32_t MinLen, uint32_t MaxLen>
struct bounded_string_value {
  static bool write(nlohmann::ordered_json& out, const void* value)
  {
    out = static_cast<const T*>(value)->to_string();
    return true;
  }
  static bool read(const nlohmann::ordered_json& in, void* value, read_context& ctx)
  {
    if (not in.is_string()) {
      log_error("The value at '{}' is not a string.", ctx.path);
      return false;
    }
    auto read_str = in.get<std::string>();
    if (read_str.size() < MinLen or read_str.size() > MaxLen) {
      log_error("The string at '{}' is {} characters long, outside its bounds [{}, {}].",
                ctx.path,
                read_str.size(),
                MinLen,
                MaxLen);
      return false;
    }
    static_cast<T*>(value)->from_string(read_str);
    return true;
  }

  static constexpr value_codec codec{&write, &read};
};

/// Codec of a value that a schema renders as a string of Digits hexadecimal characters, such as a cell identity.
///
/// The string the codec writes is uppercase and padded with leading zeros. The read takes either case and fails on
/// any other character or on a string of another length.
template <typename T, uint32_t Digits>
struct hex_number_value {
  static bool write(nlohmann::ordered_json& out, const void* value)
  {
    static const char hex[] = "0123456789ABCDEF";
    uint64_t          v     = static_cast<const T*>(value)->to_number();
    std::string       s(Digits, '0');
    for (uint32_t i = Digits; i != 0; --i) {
      s[i - 1] = hex[v & 0xf];
      v >>= 4;
    }
    out = s;
    return true;
  }
  static bool read(const nlohmann::ordered_json& in, void* value, read_context& ctx)
  {
    if (not in.is_string() or in.get<std::string>().size() != Digits) {
      log_error("The value at '{}' is not a string of {} hexadecimal characters.", ctx.path, Digits);
      return false;
    }
    uint64_t v = 0;
    for (char c : in.get<std::string>()) {
      uint32_t digit = 0;
      if (c >= '0' and c <= '9') {
        digit = c - '0';
      } else if (c >= 'a' and c <= 'f') {
        digit = c - 'a' + 10;
      } else if (c >= 'A' and c <= 'F') {
        digit = c - 'A' + 10;
      } else {
        log_error("The string at '{}' holds the character '{}', which is not hexadecimal.", ctx.path, c);
        return false;
      }
      if (v > (std::numeric_limits<uint64_t>::max() >> 4)) {
        log_error("The string at '{}' holds more digits than a value of its type carries.", ctx.path);
        return false;
      }
      v = (v << 4) | digit;
    }
    // from_number() says nothing when the value needs more bits than the type holds, and leaves it as it was, so the
    // value is read back to tell a stored value from one that was turned away.
    static_cast<T*>(value)->from_number(v);
    if (static_cast<const T*>(value)->to_number() != v) {
      log_error("The value {} at '{}' does not fit the type that holds it.", v, ctx.path);
      return false;
    }
    return true;
  }

  static constexpr value_codec codec{&write, &read};
};

/// Codec of a REAL value, a JSON number.
struct real_value {
  static bool write(nlohmann::ordered_json& out, const void* value)
  {
    out = static_cast<const real_s*>(value)->value;
    return true;
  }
  static bool read(const nlohmann::ordered_json& in, void* value, read_context& ctx)
  {
    if (not in.is_number()) {
      log_error("The value at '{}' is not a number.", ctx.path);
      return false;
    }
    // The JSON carries a number of twice the range of the REAL that holds it, and narrowing one that does not fit is
    // undefined, so it is turned away instead.
    double read = in.get<double>();
    if (read < static_cast<double>(std::numeric_limits<float>::lowest()) or
        read > static_cast<double>(std::numeric_limits<float>::max())) {
      log_error("The value {} at '{}' falls outside the range of the REAL that holds it.", read, ctx.path);
      return false;
    }
    static_cast<real_s*>(value)->value = static_cast<float>(read);
    return true;
  }

  static constexpr value_codec codec{&write, &read};
};

/// Codec of a list whose items are the SEQUENCE that Items describes.
template <typename L, const seq_desc* Items>
struct seq_list_value {
  static bool write(nlohmann::ordered_json& out, const void* value)
  {
    const L& list = *static_cast<const L*>(value);
    out           = nlohmann::ordered_json::array();
    for (const auto& item : list) {
      nlohmann::ordered_json j;
      if (not to_json(j, &item, *Items)) {
        return false;
      }
      out.push_back(std::move(j));
    }
    return true;
  }
  static bool read(const nlohmann::ordered_json& in, void* value, read_context& ctx)
  {
    if (not in.is_array()) {
      log_error("The value at '{}' is not an array.", ctx.path);
      return false;
    }
    L& list = *static_cast<L*>(value);
    list.resize(in.size());
    // A list of a fixed capacity holds what it has room for and no more, so an array of more items than that is one
    // the destination cannot take.
    if (list.size() != in.size()) {
      log_error("The array at '{}' holds {} items, more than the {} its destination has room for.",
                ctx.path,
                in.size(),
                list.size());
      return false;
    }
    for (uint32_t i = 0; i != in.size(); ++i) {
      size_t len = push_item(ctx, i);
      if (not from_json(in[i], &list[i], *Items, ctx)) {
        return false;
      }
      ctx.path.resize(len);
    }
    return true;
  }

  static void clear(void* value) { static_cast<L*>(value)->resize(0); }

  static constexpr value_codec codec{&write, &read, &clear};
};

/// Codec of a list whose items are the CHOICE that Items describes.
template <typename L, const choice_desc* Items>
struct choice_list_value {
  static bool write(nlohmann::ordered_json& out, const void* value)
  {
    const L& list = *static_cast<const L*>(value);
    out           = nlohmann::ordered_json::array();
    for (const auto& item : list) {
      nlohmann::ordered_json j;
      if (not to_json(j, &item, *Items)) {
        return false;
      }
      out.push_back(std::move(j));
    }
    return true;
  }
  static bool read(const nlohmann::ordered_json& in, void* value, read_context& ctx)
  {
    if (not in.is_array()) {
      log_error("The value at '{}' is not an array.", ctx.path);
      return false;
    }
    L& list = *static_cast<L*>(value);
    list.resize(in.size());
    // A list of a fixed capacity holds what it has room for and no more, so an array of more items than that is one
    // the destination cannot take.
    if (list.size() != in.size()) {
      log_error("The array at '{}' holds {} items, more than the {} its destination has room for.",
                ctx.path,
                in.size(),
                list.size());
      return false;
    }
    for (uint32_t i = 0; i != in.size(); ++i) {
      size_t len = push_item(ctx, i);
      if (not from_json(in[i], &list[i], *Items, ctx)) {
        return false;
      }
      ctx.path.resize(len);
    }
    return true;
  }

  static void clear(void* value) { static_cast<L*>(value)->resize(0); }

  static constexpr value_codec codec{&write, &read, &clear};
};

/// Codec of a list of integer items, which the schema bounds to [Lb, Ub].
template <typename L,
          typename Item,
          int64_t  Lb = std::numeric_limits<Item>::min(),
          uint64_t Ub = std::numeric_limits<Item>::max()>
struct int_list_value {
  static bool write(nlohmann::ordered_json& out, const void* value)
  {
    const L& list = *static_cast<const L*>(value);
    out           = nlohmann::ordered_json::array();
    for (const auto& item : list) {
      out.push_back(item);
    }
    return true;
  }
  static bool read(const nlohmann::ordered_json& in, void* value, read_context& ctx)
  {
    if (not in.is_array()) {
      log_error("The value at '{}' is not an array.", ctx.path);
      return false;
    }
    L& list = *static_cast<L*>(value);
    list.resize(in.size());
    // A list of a fixed capacity holds what it has room for and no more, so an array of more items than that is one
    // the destination cannot take.
    if (list.size() != in.size()) {
      log_error("The array at '{}' holds {} items, more than the {} its destination has room for.",
                ctx.path,
                in.size(),
                list.size());
      return false;
    }
    for (uint32_t i = 0; i != in.size(); ++i) {
      if (not in[i].is_number_integer()) {
        push_item(ctx, i);
        log_error("The value at '{}' is not an integer.", ctx.path);
        return false;
      }
      // The JSON converts without checking, so a value outside the bounds of the item would become another value. A
      // caller that builds the JSON rather than parsing it holds a value that is not negative as a signed one,
      // which is the same value.
      bool fits = false;
      if (in[i].is_number_unsigned()) {
        uint64_t read = in[i].get<uint64_t>();
        fits          = read <= Ub and (Lb <= 0 or read >= static_cast<uint64_t>(Lb));
      } else {
        int64_t read = in[i].get<int64_t>();
        fits         = read >= Lb and (read < 0 or static_cast<uint64_t>(read) <= Ub);
      }
      if (not fits) {
        push_item(ctx, i);
        log_error("The value at '{}' falls outside the bounds [{}, {}] of the item.", ctx.path, Lb, Ub);
        return false;
      }
      list[i] = in[i].get<Item>();
    }
    return true;
  }

  static void clear(void* value) { static_cast<L*>(value)->resize(0); }

  static constexpr value_codec codec{&write, &read, &clear};
};

/// Codec of a list of items of codec Item.
template <typename L, const value_codec* Item>
struct list_value {
  static bool write(nlohmann::ordered_json& out, const void* value)
  {
    const L& list = *static_cast<const L*>(value);
    out           = nlohmann::ordered_json::array();
    for (const auto& item : list) {
      nlohmann::ordered_json j;
      if (not Item->write(j, &item)) {
        return false;
      }
      out.push_back(std::move(j));
    }
    return true;
  }
  static bool read(const nlohmann::ordered_json& in, void* value, read_context& ctx)
  {
    if (not in.is_array()) {
      log_error("The value at '{}' is not an array.", ctx.path);
      return false;
    }
    L& list = *static_cast<L*>(value);
    list.resize(in.size());
    // A list of a fixed capacity holds what it has room for and no more, so an array of more items than that is one
    // the destination cannot take.
    if (list.size() != in.size()) {
      log_error("The array at '{}' holds {} items, more than the {} its destination has room for.",
                ctx.path,
                in.size(),
                list.size());
      return false;
    }
    for (uint32_t i = 0; i != in.size(); ++i) {
      size_t len = push_item(ctx, i);
      if (not Item->read(in[i], &list[i], ctx)) {
        return false;
      }
      ctx.path.resize(len);
    }
    return true;
  }

  static void clear(void* value) { static_cast<L*>(value)->resize(0); }

  static constexpr value_codec codec{&write, &read, &clear};
};

} // namespace openapi
} // namespace asn1
