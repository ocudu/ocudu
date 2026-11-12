// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/asn1_openapi_codec.h"
#include <cctype>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

using namespace asn1;
using namespace asn1::openapi;

namespace {

/// Score of an alternative that does not accept every key of a JSON object.
constexpr uint32_t no_match = std::numeric_limits<uint32_t>::max();

/// Integer type of a field of an integer or an enumerated op.
table::int_type op_int_type(table::field_op op)
{
  if (op >= table::field_op::enum_u8) {
    return static_cast<table::int_type>(static_cast<uint8_t>(op) - static_cast<uint8_t>(table::field_op::enum_u8));
  }
  return static_cast<table::int_type>(static_cast<uint8_t>(op) - static_cast<uint8_t>(table::field_op::int_u8));
}

uint64_t load_unsigned(const void* field, table::int_type itype)
{
  switch (itype) {
    case table::int_type::u8:
      return *static_cast<const uint8_t*>(field);
    case table::int_type::u16:
      return *static_cast<const uint16_t*>(field);
    case table::int_type::u32:
      return *static_cast<const uint32_t*>(field);
    default:
      return *static_cast<const uint64_t*>(field);
  }
}

int64_t load_signed(const void* field, table::int_type itype)
{
  switch (itype) {
    case table::int_type::i8:
      return *static_cast<const int8_t*>(field);
    case table::int_type::i16:
      return *static_cast<const int16_t*>(field);
    case table::int_type::i32:
      return *static_cast<const int32_t*>(field);
    default:
      return *static_cast<const int64_t*>(field);
  }
}

void store_unsigned(void* field, table::int_type itype, uint64_t value)
{
  switch (itype) {
    case table::int_type::u8:
      *static_cast<uint8_t*>(field) = static_cast<uint8_t>(value);
      break;
    case table::int_type::u16:
      *static_cast<uint16_t*>(field) = static_cast<uint16_t>(value);
      break;
    case table::int_type::u32:
      *static_cast<uint32_t*>(field) = static_cast<uint32_t>(value);
      break;
    default:
      *static_cast<uint64_t*>(field) = value;
  }
}

void store_signed(void* field, table::int_type itype, int64_t value)
{
  switch (itype) {
    case table::int_type::i8:
      *static_cast<int8_t*>(field) = static_cast<int8_t>(value);
      break;
    case table::int_type::i16:
      *static_cast<int16_t*>(field) = static_cast<int16_t>(value);
      break;
    case table::int_type::i32:
      *static_cast<int32_t*>(field) = static_cast<int32_t>(value);
      break;
    default:
      *static_cast<int64_t*>(field) = value;
  }
}

/// Whether a field holds its value in a copy_ptr.
bool is_ptr_field(const table::seq_field& f)
{
  return f.presence == table::seq_field::ptr_presence or f.presence == table::seq_field::ptr_mandatory;
}

/// Value of a field, which its copy_ptr or its std::any may hold.
const void* field_value(const void* field, const table::seq_field& f)
{
  return f.codec != nullptr and f.codec->get != nullptr ? f.codec->get(field) : field;
}

void* field_value(void* field, const table::seq_field& f)
{
  return f.codec != nullptr and f.codec->get_mut != nullptr ? f.codec->get_mut(field) : field;
}

/// Whether the field holds a value to write.
bool is_present(const uint8_t* base, const table::seq_field& f)
{
  switch (f.presence) {
    case table::seq_field::mandatory:
      return true;
    case table::seq_field::list_presence:
      return not f.codec->empty(base + f.offset);
    case table::seq_field::ptr_presence:
    case table::seq_field::ptr_mandatory:
      return table::ptr_present(base + f.offset);
    default:
      return *reinterpret_cast<const bool*>(base + f.presence);
  }
}

/// Marks the field as holding a value, allocating the value of a copy_ptr field.
void set_present(uint8_t* base, const table::seq_field& f)
{
  if (is_ptr_field(f)) {
    f.codec->set_present(base + f.offset, true);
  } else if (f.presence >= 0) {
    *reinterpret_cast<bool*>(base + f.presence) = true;
  }
  if (f.codec != nullptr and f.codec->init != nullptr) {
    f.codec->init(base + f.offset);
  }
}

/// Marks the field as absent, so that a value the JSON does not carry is not the one the destination held.
void set_absent(uint8_t* base, const table::seq_field& f, const field_desc& desc)
{
  if (is_ptr_field(f)) {
    f.codec->set_present(base + f.offset, false);
  } else if (f.presence >= 0) {
    *reinterpret_cast<bool*>(base + f.presence) = false;
  } else if (f.presence == table::seq_field::list_presence and desc.value != nullptr and desc.value->clear != nullptr) {
    // A list is present when it holds an item, so emptying it is what makes it absent.
    desc.value->clear(base + f.offset);
  }
}

/// Whether the field must hold a value.
bool is_mandatory(const table::seq_field& f)
{
  return f.presence == table::seq_field::mandatory or f.presence == table::seq_field::ptr_mandatory;
}

/// Index of the active alternative of a CHOICE.
uint32_t active_type(const void* value, const table::choice_desc& shape)
{
  if (shape.type_offset == table::choice_desc::no_member) {
    return 0;
  }
  const auto* type = static_cast<const uint8_t*>(value) + shape.type_offset;
  return static_cast<uint32_t>(load_unsigned(type, shape.type_itype));
}

/// Storage of the active alternative of a CHOICE.
const void* alternative_storage(const void* value, const table::choice_desc& shape)
{
  if (shape.storage_offset == table::choice_desc::no_member) {
    return value;
  }
  return static_cast<const uint8_t*>(value) + shape.storage_offset;
}

void* alternative_storage(void* value, const table::choice_desc& shape)
{
  if (shape.storage_offset == table::choice_desc::no_member) {
    return value;
  }
  return static_cast<uint8_t*>(value) + shape.storage_offset;
}

/// Writes the value of a field or of an alternative, given its op and its OpenAPI table.
bool value_to_json(nlohmann::ordered_json& out,
                   const void*             value,
                   const table::seq_field& f,
                   const value_codec*      codec,
                   const void*             nested,
                   const char*             enumerator)
{
  switch (f.op) {
    case table::field_op::int_u8:
    case table::field_op::int_u16:
    case table::field_op::int_u32:
    case table::field_op::int_u64:
      out = load_unsigned(value, op_int_type(f.op));
      return true;
    case table::field_op::int_i8:
    case table::field_op::int_i16:
    case table::field_op::int_i32:
    case table::field_op::int_i64:
      out = load_signed(value, op_int_type(f.op));
      return true;
    case table::field_op::boolean:
      out = *static_cast<const bool*>(value);
      return true;
    case table::field_op::enum_u8:
    case table::field_op::enum_u16:
    case table::field_op::enum_u32:
    case table::field_op::enum_u64: {
      if (f.nested == nullptr) {
        log_error("An ENUMERATED of an OpenAPI schema needs the names of its values.");
        return false;
      }
      const auto& names = *static_cast<const table::enum_desc*>(f.nested);
      out               = table::enum_to_string(names, static_cast<uint32_t>(load_unsigned(value, op_int_type(f.op))));
      return true;
    }
    case table::field_op::none:
      if (enumerator != nullptr) {
        out = enumerator;
      } else {
        out = nullptr;
      }
      return true;
    case table::field_op::sequence:
      if (nested == nullptr) {
        log_error("A SEQUENCE field of an OpenAPI schema needs its table.");
        return false;
      }
      return to_json(out, value, *static_cast<const seq_desc*>(nested));
    case table::field_op::choice:
      if (nested == nullptr) {
        log_error("A CHOICE field of an OpenAPI schema needs its table.");
        return false;
      }
      return to_json(out, value, *static_cast<const choice_desc*>(nested));
    case table::field_op::codec:
      if (codec == nullptr) {
        log_error("A field of an OpenAPI schema whose value its type writes needs its codec.");
        return false;
      }
      return codec->write(out, value);
    case table::field_op::ie_container:
      log_error("An information element container has no form in an OpenAPI schema.");
      return false;
  }
  return false;
}

/// Reads the value of a field or of an alternative, given its op and its OpenAPI table.
bool value_from_json(const nlohmann::ordered_json& in,
                     void*                         value,
                     const table::seq_field&       f,
                     const value_codec*            codec,
                     const void*                   nested,
                     read_context&                 ctx)
{
  switch (f.op) {
    case table::field_op::int_u8:
    case table::field_op::int_u16:
    case table::field_op::int_u32:
    case table::field_op::int_u64: {
      if (not in.is_number_integer()) {
        log_error("The value at '{}' is not an integer.", ctx.path);
        return false;
      }
      // A caller that builds the JSON rather than parsing it holds a value that is not negative as a signed one,
      // which is the same value, so only a negative one has no place in an unsigned field.
      if (not in.is_number_unsigned() and in.get<int64_t>() < 0) {
        log_error("The value {} at '{}' is negative, and the field is unsigned.", in.get<int64_t>(), ctx.path);
        return false;
      }
      auto read = in.is_number_unsigned() ? in.get<uint64_t>() : static_cast<uint64_t>(in.get<int64_t>());
      if ((f.lb > 0 and read < static_cast<uint64_t>(f.lb)) or read > f.ub()) {
        log_error("The value {} at '{}' falls outside its bounds [{}, {}].", read, ctx.path, f.lb, f.ub());
        return false;
      }
      store_unsigned(value, op_int_type(f.op), read);
      return true;
    }
    case table::field_op::int_i8:
    case table::field_op::int_i16:
    case table::field_op::int_i32:
    case table::field_op::int_i64: {
      if (not in.is_number_integer()) {
        log_error("The value at '{}' is not an integer.", ctx.path);
        return false;
      }
      // An unsigned value is an integer too, and one above the signed range would wrap rather than be rejected.
      if (in.is_number_unsigned() and in.get<uint64_t>() > static_cast<uint64_t>(std::numeric_limits<int64_t>::max())) {
        log_error("The value {} at '{}' does not fit in the signed field.", in.get<uint64_t>(), ctx.path);
        return false;
      }
      auto read = in.get<int64_t>();
      // A field that the schema leaves unbounded carries the whole range of its type, whose upper bound does not fit
      // in the signed value that holds it.
      bool above =
          f.ub() <= static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) and read > static_cast<int64_t>(f.ub());
      if (read < f.lb or above) {
        log_error("The value {} at '{}' falls outside its bounds [{}, {}].", read, ctx.path, f.lb, f.ub());
        return false;
      }
      store_signed(value, op_int_type(f.op), read);
      return true;
    }
    case table::field_op::boolean:
      if (not in.is_boolean()) {
        log_error("The value at '{}' is not a boolean.", ctx.path);
        return false;
      }
      *static_cast<bool*>(value) = in.get<bool>();
      return true;
    case table::field_op::enum_u8:
    case table::field_op::enum_u16:
    case table::field_op::enum_u32:
    case table::field_op::enum_u64: {
      if (not in.is_string() or f.nested == nullptr) {
        log_error("The value at '{}' is not the name of an enumerated value.", ctx.path);
        return false;
      }
      const auto& names = *static_cast<const table::enum_desc*>(f.nested);
      auto        read  = in.get<std::string>();
      for (uint32_t i = 0; i != names.nof_types; ++i) {
        if (read == names.names[i]) {
          store_unsigned(value, op_int_type(f.op), i);
          return true;
        }
      }
      log_error("The string '{}' at '{}' is not a value of the enumerated type {}.", read, ctx.path, names.type_name);
      return false;
    }
    case table::field_op::none:
      return true;
    case table::field_op::sequence:
      if (nested == nullptr) {
        return false;
      }
      return from_json(in, value, *static_cast<const seq_desc*>(nested), ctx);
    case table::field_op::choice:
      if (nested == nullptr) {
        return false;
      }
      return from_json(in, value, *static_cast<const choice_desc*>(nested), ctx);
    case table::field_op::codec:
      if (codec == nullptr) {
        return false;
      }
      // A list carries the bounds of its SIZE constraint in its own field, and its codec sizes the destination from
      // the JSON before it reads an item, so the count of items is what is checked here, ahead of that. A field that
      // the schema leaves unbounded carries no bounds to check.
      if (in.is_array() and f.ub() != 0 and
          ((f.lb > 0 and in.size() < static_cast<uint64_t>(f.lb)) or in.size() > f.ub())) {
        log_error("The array at '{}' holds {} items, outside the bounds [{}, {}] of its field.",
                  ctx.path,
                  in.size(),
                  f.lb,
                  f.ub());
        return false;
      }
      return codec->read(in, value, ctx);
    case table::field_op::ie_container:
      return false;
  }
  return false;
}

/// Whether the name of a structure and the identifier of an alternative are the same name.
bool same_name(const std::string& type_name, const char* alternative)
{
  size_t len = std::strlen(alternative);
  if (type_name.size() != len) {
    return false;
  }
  for (size_t i = 0; i != len; ++i) {
    if (std::tolower(static_cast<unsigned char>(type_name[i])) !=
        std::tolower(static_cast<unsigned char>(alternative[i]))) {
      return false;
    }
  }
  return true;
}

/// Whether the alternative holds a key of the given name.
bool holds_key(const alternative_desc& alt, const std::string& name)
{
  for (uint32_t i = 0; i != alt.nof_keys; ++i) {
    if (name == alt.keys[i]) {
      return true;
    }
  }
  return false;
}

/// Number of keys of an alternative that accepts every key of the JSON object, and no_match otherwise.
///
/// A schema leaves the properties of an alternative optional, so the keys an object holds cannot select it. What
/// selects it is the alternative accepting all of them, and the alternative that accepts the fewest keys in all is
/// the one that fits the object most closely.
uint32_t match_score(const nlohmann::ordered_json& in, const alternative_desc& alt)
{
  if (alt.nof_keys == 0) {
    return no_match - 1;
  }
  for (auto member = in.begin(); member != in.end(); ++member) {
    if (not holds_key(alt, member.key())) {
      return no_match;
    }
  }
  return alt.nof_keys;
}

/// Makes the given alternative the active one of a CHOICE.
///
/// A generated CHOICE sets its alternative through its own function, or, when it keeps its alternatives in a shared
/// buffer, leaves the engine to destroy the alternative it holds and to construct the new one.
void set_choice_type(void* choice, void* value, uint32_t type, const table::choice_desc& shape)
{
  if (shape.set != nullptr) {
    shape.set(choice, type);
    return;
  }
  if (shape.type_offset == table::choice_desc::no_member) {
    return;
  }
  void* type_field = static_cast<uint8_t*>(choice) + shape.type_offset;
  if (shape.storage != nullptr) {
    table::storage_destroy_choice(
        value, static_cast<uint32_t>(load_unsigned(type_field, shape.type_itype)), *shape.storage);
  }
  store_unsigned(type_field, shape.type_itype, type);
  if (shape.storage != nullptr) {
    table::storage_init_choice(value, type, *shape.storage);
  } else if (type < shape.nof_types) {
    const table::seq_field& f = shape.alternatives[type];
    if (f.codec != nullptr and f.codec->init != nullptr) {
      f.codec->init(value);
    }
  }
}

/// Appends the name of a member to the path of a read.
void push_member(read_context& ctx, const char* name)
{
  if (not ctx.path.empty()) {
    ctx.path += '.';
  }
  ctx.path += name;
}

} // namespace

size_t asn1::openapi::push_item(read_context& ctx, uint32_t index)
{
  size_t len = ctx.path.size();
  ctx.path += '[';
  ctx.path += std::to_string(index);
  ctx.path += ']';
  return len;
}

bool asn1::openapi::to_json(nlohmann::ordered_json& out, const void* value, const seq_desc& desc)
{
  out                             = nlohmann::ordered_json::object();
  const table::seq_desc&   shape  = *desc.shape;
  const table::seq_field*  fields = shape.root.fields;
  const table::field_name* names  = shape.root.names;
  if (names == nullptr) {
    log_error("A SEQUENCE of an OpenAPI schema needs the JSON names of its fields.");
    return false;
  }
  // The shape says how many fields there are, and the two tables are indexed together, so a table that holds fewer
  // entries than the shape is one the codec must not walk.
  if (desc.nof_fields < shape.root.nof_fields) {
    log_error(
        "A SEQUENCE of an OpenAPI schema holds {} fields and its shape {}.", desc.nof_fields, shape.root.nof_fields);
    return false;
  }
  const auto* base = static_cast<const uint8_t*>(value);
  for (uint32_t i = 0; i != shape.root.nof_fields; ++i) {
    const table::seq_field& f = fields[i];
    if (not is_present(base, f)) {
      continue;
    }
    nlohmann::ordered_json member;
    if (not value_to_json(
            member, field_value(base + f.offset, f), f, desc.fields[i].value, desc.fields[i].nested, names[i].value)) {
      return false;
    }
    out[names[i].name] = std::move(member);
  }
  return true;
}

bool asn1::openapi::from_json(const nlohmann::ordered_json& in, void* value, const seq_desc& desc, read_context& ctx)
{
  if (not in.is_object()) {
    log_error("The value at '{}' is not an object.", ctx.path);
    return false;
  }
  const table::seq_desc&   shape  = *desc.shape;
  const table::seq_field*  fields = shape.root.fields;
  const table::field_name* names  = shape.root.names;
  if (names == nullptr) {
    log_error("A SEQUENCE of an OpenAPI schema needs the JSON names of its fields.");
    return false;
  }
  // The shape says how many fields there are, and the two tables are indexed together, so a table that holds fewer
  // entries than the shape is one the codec must not walk.
  if (desc.nof_fields < shape.root.nof_fields) {
    log_error(
        "A SEQUENCE of an OpenAPI schema holds {} fields and its shape {}.", desc.nof_fields, shape.root.nof_fields);
    return false;
  }
  if (ctx.reject_unknown_keys) {
    for (auto member = in.begin(); member != in.end(); ++member) {
      bool known = false;
      for (uint32_t i = 0; i != shape.root.nof_fields and not known; ++i) {
        known = member.key() == names[i].name;
      }
      if (not known) {
        log_error("The object at '{}' holds the key '{}', which the SEQUENCE does not have.", ctx.path, member.key());
        return false;
      }
    }
  }
  auto*  base     = static_cast<uint8_t*>(value);
  size_t path_len = ctx.path.size();
  // A member that names the alternative of a CHOICE is read first, whatever its place in the object, so that the
  // CHOICE reads with the name whether or not the member comes before it.
  std::string outer_type_name = ctx.type_name;
  for (uint32_t i = 0; i != shape.root.nof_fields; ++i) {
    if (not desc.fields[i].names_type) {
      continue;
    }
    auto member = in.find(names[i].name);
    if (member != in.end() and member->is_string()) {
      ctx.type_name = member->get<std::string>();
    }
  }
  for (uint32_t i = 0; i != shape.root.nof_fields; ++i) {
    const table::seq_field& f      = fields[i];
    auto                    member = in.find(names[i].name);
    if (member == in.end()) {
      if (is_mandatory(f)) {
        log_error("The mandatory field '{}' is absent from the object at '{}'.", names[i].name, ctx.path);
        ctx.type_name = outer_type_name;
        return false;
      }
      // The destination may hold a value of an earlier read, which the JSON of this one does not carry.
      set_absent(base, f, desc.fields[i]);
      continue;
    }
    set_present(base, f);
    push_member(ctx, names[i].name);
    // The path of a failure stays in the context, so the caller reads where the read stopped.
    if (not value_from_json(
            *member, field_value(base + f.offset, f), f, desc.fields[i].value, desc.fields[i].nested, ctx)) {
      ctx.type_name = outer_type_name;
      return false;
    }
    ctx.path.resize(path_len);
  }
  ctx.type_name = outer_type_name;
  return true;
}

bool asn1::openapi::from_json(const nlohmann::ordered_json& in, void* value, const seq_desc& desc)
{
  read_context ctx;
  return from_json(in, value, desc, ctx);
}

bool asn1::openapi::to_json(nlohmann::ordered_json& out, const void* value, const choice_desc& desc)
{
  const table::choice_desc& shape = *desc.shape;
  out                             = nlohmann::ordered_json::object();
  // The shape says how many alternatives there are, and the two tables are indexed together, so a table that holds
  // fewer entries than the shape is one the codec must not walk.
  if (desc.nof_alternatives < shape.nof_types) {
    log_error("A CHOICE of an OpenAPI schema holds {} alternatives and its shape {}.",
              desc.nof_alternatives,
              shape.nof_types);
    return false;
  }
  uint32_t type = active_type(value, shape);
  if (type >= shape.nof_types) {
    log_error("The CHOICE {} holds no alternative to write.", shape.name != nullptr ? shape.name : "");
    return false;
  }
  const alternative_desc& alt = desc.alternatives[type];
  nlohmann::ordered_json  member;
  if (not value_to_json(member,
                        field_value(alternative_storage(value, shape), shape.alternatives[type]),
                        shape.alternatives[type],
                        alt.value,
                        alt.nested,
                        shape.names != nullptr ? shape.names[type].value : nullptr)) {
    return false;
  }
  if (not desc.wrapped) {
    out = std::move(member);
    return true;
  }
  if (shape.names == nullptr) {
    log_error("A wrapped CHOICE of an OpenAPI schema needs the JSON names of its alternatives.");
    return false;
  }
  out[shape.names[type].name] = std::move(member);
  return true;
}

bool asn1::openapi::from_json(const nlohmann::ordered_json& in, void* value, const choice_desc& desc, read_context& ctx)
{
  const table::choice_desc& shape = *desc.shape;
  // The shape says how many alternatives there are, and the two tables are indexed together, so a table that holds
  // fewer entries than the shape is one the codec must not walk.
  if (desc.nof_alternatives < shape.nof_types) {
    log_error("A CHOICE of an OpenAPI schema holds {} alternatives and its shape {}.",
              desc.nof_alternatives,
              shape.nof_types);
    return false;
  }
  if (desc.wrapped) {
    if (not in.is_object() or in.size() != 1 or shape.names == nullptr) {
      log_error("The value at '{}' is not an object of one member.", ctx.path);
      return false;
    }
    for (uint32_t i = 0; i != shape.nof_types; ++i) {
      auto member = in.find(shape.names[i].name);
      if (member == in.end()) {
        continue;
      }
      set_choice_type(value, alternative_storage(value, shape), i, shape);
      const alternative_desc& alt      = desc.alternatives[i];
      size_t                  path_len = ctx.path.size();
      push_member(ctx, shape.names[i].name);
      if (not value_from_json(
              *member, alternative_storage(value, shape), shape.alternatives[i], alt.value, alt.nested, ctx)) {
        return false;
      }
      ctx.path.resize(path_len);
      return true;
    }
    return false;
  }

  if (not in.is_object()) {
    // The alternatives of a oneOf may be values rather than objects, and then the kind of the JSON selects one.
    for (uint32_t i = 0; i != shape.nof_types; ++i) {
      const table::field_op op = shape.alternatives[i].op;
      bool is = (in.is_number() and op >= table::field_op::int_u8 and op <= table::field_op::int_i64) or
                (in.is_boolean() and op == table::field_op::boolean) or
                (in.is_string() and (op >= table::field_op::enum_u8 or op == table::field_op::codec));
      if (not is) {
        continue;
      }
      set_choice_type(value, alternative_storage(value, shape), i, shape);
      const alternative_desc& alt = desc.alternatives[i];
      return value_from_json(in, alternative_storage(value, shape), shape.alternatives[i], alt.value, alt.nested, ctx);
    }
    log_error(
        "No alternative of the CHOICE {} takes the value at '{}'.", shape.name != nullptr ? shape.name : "", ctx.path);
    return false;
  }
  // A member of the enclosing object may name the alternative that the CHOICE holds, which is what the schema of a
  // oneOf whose alternatives no key tells apart relies on, so that name selects it, and a name that no alternative
  // carries leaves the keys to select one.
  if (not ctx.type_name.empty() and shape.names != nullptr) {
    for (uint32_t i = 0; i != shape.nof_types; ++i) {
      // The schema names a RAN configuration structure with a capital where the ASN.1 identifier of the
      // alternative, which the names hold and the JSON Encoding Rules write, must begin in lower case. The two
      // differ in that alone, so the name of the structure is matched without regard to case.
      if (shape.names[i].name == nullptr or not same_name(ctx.type_name, shape.names[i].name)) {
        continue;
      }
      // The object says which alternative it holds, so that is the one it holds. A value that the named
      // alternative does not take contradicts its own name, and no other alternative stands in for it.
      if (match_score(in, desc.alternatives[i]) == no_match) {
        log_error("The object at '{}' names the alternative {} of the CHOICE {} and holds a key it does not have.",
                  ctx.path,
                  shape.names[i].name,
                  shape.name != nullptr ? shape.name : "");
        return false;
      }
      set_choice_type(value, alternative_storage(value, shape), i, shape);
      const alternative_desc& named = desc.alternatives[i];
      if (value_from_json(
              in, alternative_storage(value, shape), shape.alternatives[i], named.value, named.nested, ctx)) {
        return true;
      }
      log_error("The object at '{}' names the alternative {} of the CHOICE {}, which does not read it.",
                ctx.path,
                shape.names[i].name,
                shape.name != nullptr ? shape.name : "");
      return false;
    }
  }
  // An alternative of a oneOf carries no name, so the alternative that accepts every key of the object selects it.
  // Several may accept it, and the one that accepts the fewest keys fits it most closely, so they are tried in that
  // order until one reads: an alternative whose own mandatory members the object lacks is not the one it holds. Two
  // that accept as many keys are tried in the order of the CHOICE, so that neither is left out.
  // The score of an alternative follows from the object alone, so it is taken once here. Taking it inside the loop
  // below would compare every key of the object against every key of every alternative again on each pass, which an
  // object that no alternative reads turns into the whole of that work for nothing.
  std::vector<uint32_t> scores(shape.nof_types);
  for (uint32_t i = 0; i != shape.nof_types; ++i) {
    scores[i] = match_score(in, desc.alternatives[i]);
  }
  // The path of the alternative that read the furthest, which names where a value that no alternative reads stops.
  std::string deepest     = ctx.path;
  uint32_t    taken_score = 0;
  uint32_t    taken_alt   = 0;
  bool        any_taken   = false;
  for (uint32_t tried = 0; tried != shape.nof_types; ++tried) {
    uint32_t best       = shape.nof_types;
    uint32_t best_score = no_match;
    for (uint32_t i = 0; i != shape.nof_types; ++i) {
      uint32_t score = scores[i];
      if (score == no_match) {
        continue;
      }
      if (any_taken and (score < taken_score or (score == taken_score and i <= taken_alt))) {
        continue;
      }
      if (score < best_score) {
        best_score = score;
        best       = i;
      }
    }
    if (best == shape.nof_types) {
      break;
    }
    any_taken   = true;
    taken_score = best_score;
    taken_alt   = best;
    set_choice_type(value, alternative_storage(value, shape), best, shape);
    const alternative_desc& alt      = desc.alternatives[best];
    size_t                  path_len = ctx.path.size();
    if (value_from_json(in, alternative_storage(value, shape), shape.alternatives[best], alt.value, alt.nested, ctx)) {
      return true;
    }
    if (ctx.path.size() > deepest.size()) {
      deepest = ctx.path;
    }
    ctx.path.resize(path_len);
  }
  ctx.path = deepest;
  log_error(
      "No alternative of the CHOICE {} reads the object at '{}'.", shape.name != nullptr ? shape.name : "", ctx.path);
  return false;
}

bool asn1::openapi::from_json(const nlohmann::ordered_json& in, void* value, const choice_desc& desc)
{
  read_context ctx;
  return from_json(in, value, desc, ctx);
}
