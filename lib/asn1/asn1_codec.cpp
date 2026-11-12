// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Table-driven codec engine of SEQUENCE, CHOICE, lists and IE containers.

#include "ocudu/asn1/asn1_ap_codec.h"
#include <algorithm>
#include <array>
#include <cstring>

namespace asn1 {

/************************
  table-driven SEQUENCE
************************/

/// Returns the failure of a nested value, which the engine logs where it happens, with the field, alternative or IE
/// that it went through.
#define RETURN_ON_FAILURE(ret)                                                                                         \
  do {                                                                                                                 \
    OCUDUASN_CODE code_ = (ret);                                                                                       \
    if (code_ != OCUDUASN_SUCCESS) {                                                                                   \
      return code_;                                                                                                    \
    }                                                                                                                  \
  } while (0)

namespace {

/// Kind of a failure, for the log.
const char* failure_kind(OCUDUASN_CODE code)
{
  return code == OCUDUASN_ERROR_ENCODE_FAIL ? "Encoding" : "Decoding";
}

/// Logs the field of a SEQUENCE that a failure went through, so that the log tells where the failure happened.
[[gnu::cold]] [[gnu::noinline]] OCUDUASN_CODE
field_failure(OCUDUASN_CODE code, const table::field_name* names, uint32_t idx)
{
  if (names != nullptr and names[idx].name != nullptr) {
    log_error("{} failure in field {}", failure_kind(code), names[idx].name);
  } else {
    log_error("{} failure in field number {}", failure_kind(code), idx);
  }
  return code;
}

/// Logs the alternative of a CHOICE that a failure went through.
[[gnu::cold]] [[gnu::noinline]] OCUDUASN_CODE
alternative_failure(OCUDUASN_CODE code, const table::choice_desc& desc, uint32_t type_idx)
{
  const char* choice = desc.name != nullptr ? desc.name : "a CHOICE";
  if (desc.names != nullptr and desc.names[type_idx].name != nullptr) {
    log_error("{} failure in alternative {} of {}", failure_kind(code), desc.names[type_idx].name, choice);
  } else {
    log_error("{} failure in alternative number {} of {}", failure_kind(code), type_idx, choice);
  }
  return code;
}

/// Logs the item of a list that a failure went through.
[[gnu::cold]] [[gnu::noinline]] OCUDUASN_CODE item_failure(OCUDUASN_CODE code, uint32_t idx)
{
  log_error("{} failure in item {} of a list", failure_kind(code), idx);
  return code;
}

/// Logs the IE of a container that a failure went through.
[[gnu::cold]] [[gnu::noinline]] OCUDUASN_CODE ie_failure(OCUDUASN_CODE code, uint32_t id)
{
  log_error("{} failure in IE id={}", failure_kind(code), id);
  return code;
}

/// Number of bits of a constrained whole number whose largest offset from its lower bound is max_offset.
OCUDU_FORCE_INLINE uint32_t offset_bits(uint64_t max_offset)
{
  return max_offset == 0 ? 0 : 64U - static_cast<uint32_t>(__builtin_clzll(max_offset));
}

/// Whether an integer field is constrained, without an extension marker, and encoded as a bit field.
template <class T>
OCUDU_FORCE_INLINE bool is_bit_field_int(T lb, T ub, const table::seq_field& f)
{
  // Type limits of signed integers stand for absent bounds.
  bool bounded = std::is_unsigned_v<T> or (lb != std::numeric_limits<T>::min() and ub != std::numeric_limits<T>::max());
  return not f.has_ext and bounded and (not f.aligned or f.range < 255);
}

template <class T>
OCUDU_FORCE_INLINE OCUDUASN_CODE pack_int_field(bit_ref& bref, const void* field, const table::seq_field& f)
{
  T n  = *static_cast<const T*>(field);
  T lb = static_cast<T>(f.lb);
  T ub = static_cast<T>(f.ub());
  // A value in range of a bit field integer, the common case, inline. Values below lb wrap above the range.
  uint64_t offset = static_cast<uint64_t>(n) - static_cast<uint64_t>(lb);
  if (is_bit_field_int(lb, ub, f) and offset <= f.range) {
    return bref.pack(offset, offset_bits(f.range));
  }
  return pack_integer(bref, n, lb, ub, f.has_ext, f.aligned);
}

/// Logs a decoded integer out of its bounds.
template <class T>
OCUDUASN_CODE int_out_of_bounds(T lb, uint64_t offset, T ub)
{
  log_error("The condition lb <= n <= ub ({} <= {} <= {}) was not met",
            static_cast<long>(lb),
            static_cast<long>(static_cast<uint64_t>(lb) + offset),
            static_cast<long>(ub));
  return OCUDUASN_ERROR_DECODE_FAIL;
}

template <class T>
OCUDU_FORCE_INLINE OCUDUASN_CODE unpack_int_field(void* field, cbit_ref& bref, const table::seq_field& f)
{
  T lb = static_cast<T>(f.lb);
  T ub = static_cast<T>(f.ub());
  if (is_bit_field_int(lb, ub, f)) {
    uint64_t max_offset = f.range;
    uint64_t offset     = 0;
    HANDLE_CODE(bref.unpack(offset, offset_bits(max_offset)));
    if (offset > max_offset) {
      return int_out_of_bounds(lb, offset, ub);
    }
    *static_cast<T*>(field) = static_cast<T>(static_cast<uint64_t>(lb) + offset);
    return OCUDUASN_SUCCESS;
  }
  return unpack_integer(*static_cast<T*>(field), bref, lb, ub, f.has_ext, f.aligned);
}

/// Packs an ENUMERATED value, the values of its root inline.
OCUDU_FORCE_INLINE OCUDUASN_CODE
pack_enum_value(bit_ref& bref, uint32_t e, uint32_t nof_types, uint32_t nof_exts, bool has_ext)
{
  uint32_t nof_root = nof_types - nof_exts;
  if (e < nof_root) {
    // The zero extension bit of an extensible ENUMERATED and the value in one call.
    return bref.pack(e, offset_bits(nof_root - 1) + (has_ext ? 1U : 0U));
  }
  return pack_enum(bref, e, nof_types, nof_exts, has_ext);
}

/// Logs a decoded ENUMERATED value out of range.
OCUDUASN_CODE enum_out_of_range(uint32_t e, uint32_t nof_types)
{
  log_error("The provided enum is not within the range of possible values ({}>={})",
            static_cast<unsigned>(e),
            static_cast<unsigned>(nof_types));
  return OCUDUASN_ERROR_DECODE_FAIL;
}

/// Unpacks an ENUMERATED value.
OCUDU_FORCE_INLINE OCUDUASN_CODE
unpack_enum_value(uint32_t& e, cbit_ref& bref, uint32_t nof_types, uint32_t nof_exts, bool has_ext)
{
  uint32_t nof_root = nof_types - nof_exts;
  bool     ext      = false;
  if (has_ext) {
    HANDLE_CODE(bref.unpack(ext, 1));
  }
  if (not ext) {
    HANDLE_CODE(bref.unpack(e, offset_bits(nof_root - 1)));
  } else {
    HANDLE_CODE(unpack_norm_small_non_neg_whole_number(e, bref));
    e += nof_root;
  }
  if (e >= nof_types) {
    return enum_out_of_range(e, nof_types);
  }
  return OCUDUASN_SUCCESS;
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

/// Packs an ENUMERATED field, whose number of values is in lb and number of extensions in ub.
template <class T>
OCUDU_FORCE_INLINE OCUDUASN_CODE pack_enum_field(bit_ref& bref, const void* field, const table::seq_field& f)
{
  return pack_enum_value(bref,
                         static_cast<uint32_t>(*static_cast<const T*>(field)),
                         static_cast<uint32_t>(f.lb),
                         static_cast<uint32_t>(f.range),
                         f.has_ext);
}

template <class T>
OCUDU_FORCE_INLINE OCUDUASN_CODE unpack_enum_field(void* field, cbit_ref& bref, const table::seq_field& f)
{
  uint32_t e = 0;
  HANDLE_CODE(unpack_enum_value(e, bref, static_cast<uint32_t>(f.lb), static_cast<uint32_t>(f.range), f.has_ext));
  *static_cast<T*>(field) = static_cast<T>(e);
  return OCUDUASN_SUCCESS;
}

/// Integer type of a field of an integer or enumerated op.
table::int_type op_int_type(table::field_op op)
{
  if (op >= table::field_op::enum_u8) {
    return static_cast<table::int_type>(static_cast<uint8_t>(op) - static_cast<uint8_t>(table::field_op::enum_u8));
  }
  return static_cast<table::int_type>(static_cast<uint8_t>(op) - static_cast<uint8_t>(table::field_op::int_u8));
}

/// Value of a field that its codec holds, such as a copy_ptr or a std::any, or the field itself.
const void* field_value(const void* field, const table::seq_field& f)
{
  return f.codec != nullptr and f.codec->get != nullptr ? f.codec->get(field) : field;
}
void* field_value(void* field, const table::seq_field& f)
{
  return f.codec != nullptr and f.codec->get_mut != nullptr ? f.codec->get_mut(field) : field;
}

/// Whether a field holds its value in a copy_ptr.
bool is_ptr_field(const table::seq_field& f)
{
  return f.presence == table::seq_field::ptr_presence or f.presence == table::seq_field::ptr_mandatory;
}

/// Value of a field of op sequence, choice or ie_container: the value of its copy_ptr, or the field itself. Other
/// ops encode the field itself, or their codec reaches the value.
const void* nested_value(const void* field, const table::seq_field& f)
{
  return is_ptr_field(f) ? f.codec->get(field) : field;
}
void* nested_value(void* field, const table::seq_field& f)
{
  return is_ptr_field(f) ? f.codec->get_mut(field) : field;
}

/// Type member of a CHOICE that starts at start, or null when the CHOICE has a single alternative and no type.
const void* choice_type_field(const uint8_t* start, const table::choice_desc& desc)
{
  return desc.type_offset == table::choice_desc::no_member ? nullptr : start + desc.type_offset;
}

/// Packs a field. A value held in a std::any is passed as the value, not as the std::any.
OCUDU_FORCE_INLINE OCUDUASN_CODE pack_field(bit_ref& bref, const void* field, const table::seq_field& f)
{
  switch (f.op) {
    case table::field_op::codec:
      return f.codec->pack(bref, field, f);
    case table::field_op::boolean:
      return bref.pack(*static_cast<const bool*>(field), 1);
    case table::field_op::none:
      return OCUDUASN_SUCCESS;
    case table::field_op::sequence:
      return table::pack_sequence(bref, nested_value(field, f), *static_cast<const table::seq_desc*>(f.nested));
    case table::field_op::choice: {
      const auto& child = *static_cast<const table::choice_desc*>(f.nested);
      const auto* start = static_cast<const uint8_t*>(nested_value(field, f));
      return table::pack_choice(bref, choice_type_field(start, child), start + child.storage_offset, child);
    }
    case table::field_op::ie_container:
      return table::pack_ie_container(
          bref, nested_value(field, f), *static_cast<const table::ie_container_desc*>(f.nested));
    case table::field_op::int_u8:
      return pack_int_field<uint8_t>(bref, field, f);
    case table::field_op::int_u16:
      return pack_int_field<uint16_t>(bref, field, f);
    case table::field_op::int_u32:
      return pack_int_field<uint32_t>(bref, field, f);
    case table::field_op::int_u64:
      return pack_int_field<uint64_t>(bref, field, f);
    case table::field_op::int_i8:
      return pack_int_field<int8_t>(bref, field, f);
    case table::field_op::int_i16:
      return pack_int_field<int16_t>(bref, field, f);
    case table::field_op::int_i32:
      return pack_int_field<int32_t>(bref, field, f);
    case table::field_op::int_i64:
      return pack_int_field<int64_t>(bref, field, f);
    case table::field_op::enum_u8:
      return pack_enum_field<uint8_t>(bref, field, f);
    case table::field_op::enum_u16:
      return pack_enum_field<uint16_t>(bref, field, f);
    case table::field_op::enum_u32:
      return pack_enum_field<uint32_t>(bref, field, f);
    case table::field_op::enum_u64:
      return pack_enum_field<uint64_t>(bref, field, f);
  }
  return OCUDUASN_ERROR_ENCODE_FAIL;
}

/// Unpacks a field. A value held in a std::any is passed as the value, not as the std::any.
OCUDU_FORCE_INLINE OCUDUASN_CODE unpack_field(void* field, cbit_ref& bref, const table::seq_field& f)
{
  switch (f.op) {
    case table::field_op::codec:
      return f.codec->unpack(field, bref, f);
    case table::field_op::boolean:
      return bref.unpack(*static_cast<bool*>(field), 1);
    case table::field_op::none:
      return OCUDUASN_SUCCESS;
    case table::field_op::sequence:
      return table::unpack_sequence(nested_value(field, f), bref, *static_cast<const table::seq_desc*>(f.nested));
    case table::field_op::choice: {
      const auto& child = *static_cast<const table::choice_desc*>(f.nested);
      void*       value = nested_value(field, f);
      return table::unpack_choice(value, static_cast<uint8_t*>(value) + child.storage_offset, bref, child);
    }
    case table::field_op::ie_container:
      return table::unpack_ie_container(
          nested_value(field, f), bref, *static_cast<const table::ie_container_desc*>(f.nested));
    case table::field_op::int_u8:
      return unpack_int_field<uint8_t>(field, bref, f);
    case table::field_op::int_u16:
      return unpack_int_field<uint16_t>(field, bref, f);
    case table::field_op::int_u32:
      return unpack_int_field<uint32_t>(field, bref, f);
    case table::field_op::int_u64:
      return unpack_int_field<uint64_t>(field, bref, f);
    case table::field_op::int_i8:
      return unpack_int_field<int8_t>(field, bref, f);
    case table::field_op::int_i16:
      return unpack_int_field<int16_t>(field, bref, f);
    case table::field_op::int_i32:
      return unpack_int_field<int32_t>(field, bref, f);
    case table::field_op::int_i64:
      return unpack_int_field<int64_t>(field, bref, f);
    case table::field_op::enum_u8:
      return unpack_enum_field<uint8_t>(field, bref, f);
    case table::field_op::enum_u16:
      return unpack_enum_field<uint16_t>(field, bref, f);
    case table::field_op::enum_u32:
      return unpack_enum_field<uint32_t>(field, bref, f);
    case table::field_op::enum_u64:
      return unpack_enum_field<uint64_t>(field, bref, f);
  }
  return OCUDUASN_ERROR_DECODE_FAIL;
}

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

/// Packs the presence bits of the optional fields, then the values of the present fields, of a field set with more
/// fields than its presence mask tracks.
OCUDUASN_CODE pack_unmasked_fields(bit_ref& bref, const uint8_t* base, const table::field_set& set)
{
  const table::seq_field* fields     = set.fields;
  uint32_t                nof_fields = set.nof_fields;
  for (uint32_t i = 0; i != nof_fields; ++i) {
    const table::seq_field& f = fields[i];
    if (f.presence != table::seq_field::mandatory and f.presence != table::seq_field::ptr_mandatory) {
      HANDLE_CODE(bref.pack(is_present(base, f), 1));
    }
  }
  for (uint32_t i = 0; i != nof_fields; ++i) {
    const table::seq_field& f = fields[i];
    if (not is_present(base, f)) {
      // Another field of the extension group may be present while this one is not.
      if (f.presence == table::seq_field::ptr_mandatory) {
        log_error("Mandatory field of an extension group is missing");
        return OCUDUASN_ERROR_ENCODE_FAIL;
      }
      continue;
    }
    if (OCUDUASN_CODE ret = pack_field(bref, base + f.offset, f); ret != OCUDUASN_SUCCESS) {
      return field_failure(ret, set.names, i);
    }
  }
  return OCUDUASN_SUCCESS;
}

/// Unpacks the presence bits of the optional fields, then the values of the present fields, of a field set with more
/// fields than its presence mask tracks.
OCUDUASN_CODE unpack_unmasked_fields(uint8_t* base, cbit_ref& bref, const table::field_set& set)
{
  const table::seq_field* fields     = set.fields;
  uint32_t                nof_fields = set.nof_fields;
  // Presence of the optional lists, which have no presence flag, one bit per list in order.
  uint64_t list_present = 0;
  uint32_t nof_lists    = 0;
  for (uint32_t i = 0; i != nof_fields; ++i) {
    const table::seq_field& f = fields[i];
    if (f.presence == table::seq_field::mandatory or f.presence == table::seq_field::ptr_mandatory) {
      continue;
    }
    bool present;
    HANDLE_CODE(bref.unpack(present, 1));
    if (f.presence == table::seq_field::list_presence) {
      list_present |= static_cast<uint64_t>(present) << nof_lists++;
    } else if (f.presence == table::seq_field::ptr_presence) {
      f.codec->set_present(base + f.offset, present);
    } else {
      *reinterpret_cast<bool*>(base + f.presence) = present;
    }
  }
  nof_lists = 0;
  for (uint32_t i = 0; i != nof_fields; ++i) {
    const table::seq_field& f = fields[i];
    switch (f.presence) {
      case table::seq_field::mandatory:
        break;
      case table::seq_field::ptr_mandatory:
        f.codec->set_present(base + f.offset, true);
        break;
      case table::seq_field::list_presence:
        if (((list_present >> nof_lists++) & 1U) == 0) {
          continue;
        }
        break;
      default:
        if (not is_present(base, f)) {
          continue;
        }
    }
    if (OCUDUASN_CODE ret = unpack_field(base + f.offset, bref, f); ret != OCUDUASN_SUCCESS) {
      return field_failure(ret, set.names, i);
    }
  }
  return OCUDUASN_SUCCESS;
}

/// Whether any field of the set is present.
bool any_present(const uint8_t* base, const table::field_set& set)
{
  if (set.nof_fields > table::field_set::max_masked_fields) {
    for (uint32_t i = 0; i != set.nof_fields; ++i) {
      if (is_present(base, set.fields[i])) {
        return true;
      }
    }
    return false;
  }
  for (uint32_t k = 0; k != set.nof_opts; ++k) {
    const table::seq_opt& opt = set.opts[k];
    if (opt.presence >= 0 ? *reinterpret_cast<const bool*>(base + opt.presence)
                          : is_present(base, set.fields[opt.field])) {
      return true;
    }
  }
  // Fields without a presence bit of an extension addition are copy_ptr fields, present when the addition is.
  for (uint64_t mandatory = set.mandatory; mandatory != 0; mandatory &= mandatory - 1) {
    if (is_present(base, set.fields[__builtin_ctzll(mandatory)])) {
      return true;
    }
  }
  return false;
}

/// Packs the presence bits of the optional fields, then the values of the present fields.
OCUDU_FORCE_INLINE OCUDUASN_CODE pack_fields(bit_ref& bref, const uint8_t* base, const table::field_set& set)
{
  if (set.nof_fields > table::field_set::max_masked_fields) {
    return pack_unmasked_fields(bref, base, set);
  }
  // Fields to encode, one bit per field index, and the presence bits, the first optional field in the highest bit.
  uint64_t visit = set.mandatory;
  uint64_t bits  = 0;
  for (uint32_t k = 0; k != set.nof_opts; ++k) {
    const table::seq_opt& opt     = set.opts[k];
    bool                  present = opt.presence >= 0 ? *reinterpret_cast<const bool*>(base + opt.presence)
                                                      : is_present(base, set.fields[opt.field]);
    bits                          = (bits << 1U) | static_cast<uint64_t>(present);
    visit |= static_cast<uint64_t>(present) << opt.field;
  }
  if (set.nof_opts != 0) {
    HANDLE_CODE(bref.pack(bits, set.nof_opts));
  }
  for (; visit != 0; visit &= visit - 1) {
    auto                    i = static_cast<uint32_t>(__builtin_ctzll(visit));
    const table::seq_field& f = set.fields[i];
    // Another field of the extension group may be present while this one is not.
    if (f.presence == table::seq_field::ptr_mandatory and not table::ptr_present(base + f.offset)) {
      log_error("Mandatory field of an extension group is missing");
      return OCUDUASN_ERROR_ENCODE_FAIL;
    }
    if (OCUDUASN_CODE ret = pack_field(bref, base + f.offset, f); ret != OCUDUASN_SUCCESS) {
      return field_failure(ret, set.names, i);
    }
  }
  return OCUDUASN_SUCCESS;
}

/// Unpacks the presence bits of the optional fields, then the values of the present fields.
OCUDU_FORCE_INLINE OCUDUASN_CODE unpack_fields(uint8_t* base, cbit_ref& bref, const table::field_set& set)
{
  if (set.nof_fields > table::field_set::max_masked_fields) {
    return unpack_unmasked_fields(base, bref, set);
  }
  // Fields to decode, one bit per field index.
  uint64_t visit = set.mandatory;
  uint64_t bits  = 0;
  if (set.nof_opts != 0) {
    HANDLE_CODE(bref.unpack(bits, set.nof_opts));
    // The presence bit of the next optional field in the highest bit.
    bits <<= 64U - set.nof_opts;
  }
  for (uint32_t k = 0; k != set.nof_opts; ++k, bits <<= 1U) {
    const table::seq_opt& opt     = set.opts[k];
    bool                  present = (bits >> 63U) != 0;
    if (opt.presence >= 0) {
      *reinterpret_cast<bool*>(base + opt.presence) = present;
    } else if (opt.presence == table::seq_field::ptr_presence) {
      const table::seq_field& f = set.fields[opt.field];
      f.codec->set_present(base + f.offset, present);
    }
    visit |= static_cast<uint64_t>(present) << opt.field;
  }
  for (; visit != 0; visit &= visit - 1) {
    auto                    i = static_cast<uint32_t>(__builtin_ctzll(visit));
    const table::seq_field& f = set.fields[i];
    if (f.presence == table::seq_field::ptr_mandatory) {
      f.codec->set_present(base + f.offset, true);
    }
    if (OCUDUASN_CODE ret = unpack_field(base + f.offset, bref, f); ret != OCUDUASN_SUCCESS) {
      return field_failure(ret, set.names, i);
    }
  }
  return OCUDUASN_SUCCESS;
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

/// Writes the name of the value of an ENUMERATED field whose table holds the names of its values.
void enum_to_json(json_writer& j, const char* name, const void* field, const table::seq_field& f)
{
  const auto& names = *static_cast<const table::enum_desc*>(f.nested);
  auto        value = static_cast<uint32_t>(load_unsigned(field, op_int_type(f.op)));
  j.write_str(name, convert_enum_idx(names.names, names.nof_types, value, names.type_name));
}

/// Writes a field as a JSON member. value is what a field without a value writes: an enumerator name, or null.
void field_to_json(json_writer& j, const uint8_t* base, const table::seq_field& f, const char* name, const char* value)
{
  const void* field = field_value(base + f.offset, f);
  switch (f.op) {
    case table::field_op::int_u64:
      j.write_uint(name, load_unsigned(field, table::int_type::u64));
      break;
    case table::field_op::int_u8:
    case table::field_op::int_u16:
    case table::field_op::int_u32:
      j.write_int(name, static_cast<int64_t>(load_unsigned(field, op_int_type(f.op))));
      break;
    case table::field_op::int_i8:
    case table::field_op::int_i16:
    case table::field_op::int_i32:
    case table::field_op::int_i64:
      j.write_int(name, load_signed(field, op_int_type(f.op)));
      break;
    case table::field_op::boolean:
      j.write_bool(name, *static_cast<const bool*>(field));
      break;
    case table::field_op::none:
      if (value != nullptr) {
        j.write_str(name, value);
      } else {
        j.write_null(name);
      }
      break;
    case table::field_op::enum_u8:
    case table::field_op::enum_u16:
    case table::field_op::enum_u32:
    case table::field_op::enum_u64:
      if (f.nested != nullptr) {
        enum_to_json(j, name, field, f);
        break;
      }
      [[fallthrough]];
    case table::field_op::codec:
      f.codec->to_json(j, name, field, f);
      break;
    case table::field_op::sequence:
      j.write_fieldname(name);
      table::to_json_sequence(j, field, *static_cast<const table::seq_desc*>(f.nested));
      break;
    case table::field_op::choice: {
      const auto& child = *static_cast<const table::choice_desc*>(f.nested);
      const auto* start = static_cast<const uint8_t*>(field);
      j.write_fieldname(name);
      table::to_json_choice(j, choice_type_field(start, child), start + child.storage_offset, child);
      break;
    }
    case table::field_op::ie_container:
      j.write_fieldname(name);
      table::to_json_ie_container(j, field, *static_cast<const table::ie_container_desc*>(f.nested));
      break;
  }
}

/// Writes the present fields as JSON members.
void fields_to_json(json_writer&             j,
                    const uint8_t*           base,
                    const table::seq_field*  fields,
                    const table::field_name* names,
                    uint32_t                 nof_fields)
{
  for (uint32_t i = 0; i != nof_fields; ++i) {
    if (is_present(base, fields[i])) {
      field_to_json(j, base, fields[i], names[i].name, names[i].value);
    }
  }
}

/// Marks the single field of an extension addition outside of groups as present.
void set_single_present(uint8_t* base, const table::seq_field& f)
{
  if (f.presence == table::seq_field::ptr_presence or f.presence == table::seq_field::ptr_mandatory) {
    f.codec->set_present(base + f.offset, true);
  } else if (f.presence >= 0) {
    *reinterpret_cast<bool*>(base + f.presence) = true;
  }
}

} // namespace

/// Packs the extension additions of a SEQUENCE, whose presence group_bits holds, the first in the highest bit.
[[gnu::noinline]] static OCUDUASN_CODE
pack_ext_groups(bit_ref& bref, const uint8_t* base, const table::seq_desc& desc, uint64_t group_bits)
{
  // X.691 requires one bit per extension addition of the type, including absent ones after the last present one.
  HANDLE_CODE(pack_norm_small_non_neg_whole_number(bref, desc.nof_groups - 1));
  HANDLE_CODE(bref.pack(group_bits, desc.nof_groups));
  for (uint32_t g = 0; g != desc.nof_groups; ++g) {
    if (((group_bits >> (desc.nof_groups - 1 - g)) & 1U) == 0) {
      continue;
    }
    const table::seq_group& group = desc.groups[g];
    RETURN_ON_FAILURE(pack_varlength_field(bref, desc.aligned, [&](bit_ref& contents) {
      if (not group.single) {
        return pack_fields(contents, base, group.set);
      }
      OCUDUASN_CODE ret = pack_field(contents, base + group.set.fields[0].offset, group.set.fields[0]);
      return ret == OCUDUASN_SUCCESS ? ret : field_failure(ret, group.set.names, 0);
    }));
  }
  return OCUDUASN_SUCCESS;
}

/// Unpacks the extension additions of a SEQUENCE whose extension bit is set.
OCUDU_FORCE_INLINE OCUDUASN_CODE unpack_ext_groups(uint8_t* base, cbit_ref& bref, const table::seq_desc& desc)
{
  ext_groups_unpacker group_unpacker(bref, desc.aligned);
  for (uint32_t g = 0; g != desc.nof_groups; ++g) {
    HANDLE_CODE(group_unpacker.unpack_next_group());
    if (not group_unpacker.get_last_group_range(bref)) {
      continue;
    }
    const table::seq_group& group = desc.groups[g];
    if (group.single) {
      set_single_present(base, group.set.fields[0]);
      if (OCUDUASN_CODE ret = unpack_field(base + group.set.fields[0].offset, bref, group.set.fields[0]);
          ret != OCUDUASN_SUCCESS) {
        return field_failure(ret, group.set.names, 0);
      }
    } else {
      RETURN_ON_FAILURE(unpack_fields(base, bref, group.set));
    }
  }
  return group_unpacker.consume_remaining_groups(bref);
}

OCUDUASN_CODE table::pack_sequence(bit_ref& bref, const void* seq, const seq_desc& desc)
{
  const auto* base = static_cast<const uint8_t*>(seq);
  // Presence of the extension additions, the first in the highest of nof_groups bits.
  uint64_t group_bits = 0;
  for (uint32_t g = 0; g != desc.nof_groups; ++g) {
    group_bits = (group_bits << 1U) | static_cast<uint64_t>(any_present(base, desc.groups[g].set));
  }
  bool ext = group_bits != 0;
  if (desc.has_ext) {
    HANDLE_CODE(bref.pack(ext, 1));
  }
  RETURN_ON_FAILURE(pack_fields(bref, base, desc.root));
  return ext ? pack_ext_groups(bref, base, desc, group_bits) : OCUDUASN_SUCCESS;
}

OCUDUASN_CODE table::unpack_sequence(void* seq, cbit_ref& bref, const seq_desc& desc)
{
  auto* base = static_cast<uint8_t*>(seq);
  bool  ext  = false;
  if (desc.has_ext) {
    HANDLE_CODE(bref.unpack(ext, 1));
  }
  RETURN_ON_FAILURE(unpack_fields(base, bref, desc.root));
  return ext ? unpack_ext_groups(base, bref, desc) : OCUDUASN_SUCCESS;
}

OCUDUASN_CODE table::pack_sequence_item(bit_ref& bref, const void* item, const void* packer)
{
  return pack_sequence(bref, item, *static_cast<const seq_desc*>(packer));
}

OCUDUASN_CODE table::unpack_sequence_item(void* item, cbit_ref& bref, const void* unpacker)
{
  return unpack_sequence(item, bref, *static_cast<const seq_desc*>(unpacker));
}

OCUDUASN_CODE table::pack_choice_item(bit_ref& bref, const void* item, const void* packer)
{
  const auto& desc  = *static_cast<const choice_desc*>(packer);
  const auto* start = static_cast<const uint8_t*>(item);
  return pack_choice(bref, choice_type_field(start, desc), start + desc.storage_offset, desc);
}

OCUDUASN_CODE table::unpack_choice_item(void* item, cbit_ref& bref, const void* unpacker)
{
  const auto& desc = *static_cast<const choice_desc*>(unpacker);
  return unpack_choice(item, static_cast<uint8_t*>(item) + desc.storage_offset, bref, desc);
}

void table::sequence_item_to_json(json_writer& j, const void* item, const void* desc)
{
  to_json_sequence(j, item, *static_cast<const seq_desc*>(desc));
}

void table::choice_item_to_json(json_writer& j, const void* item, const void* desc)
{
  to_json_value(j, item, *static_cast<const choice_desc*>(desc));
}

OCUDUASN_CODE table::pack_item_list(bit_ref& bref, const void* field, const seq_field& desc)
{
  const auto& codec = *static_cast<const item_list_field_codec*>(desc.codec);
  list_view   list  = codec.view(field);
  const void* table = desc.nested;
  if (codec.resize != nullptr) {
    return detail::pack_dyn_seq_of_items(
        bref, list.data, list.size, codec.item_size, desc.lb, desc.ub(), desc.aligned, codec.pack_item, table);
  }
  const auto* item = static_cast<const uint8_t*>(list.data);
  for (uint32_t i = 0; i != list.size; ++i, item += codec.item_size) {
    if (OCUDUASN_CODE ret = codec.pack_item(bref, item, table); ret != OCUDUASN_SUCCESS) {
      return item_failure(ret, i);
    }
  }
  return OCUDUASN_SUCCESS;
}

OCUDUASN_CODE table::unpack_item_list(void* field, cbit_ref& bref, const seq_field& desc)
{
  const auto& codec = *static_cast<const item_list_field_codec*>(desc.codec);
  const void* table = desc.nested;
  if (codec.resize != nullptr) {
    return detail::unpack_dyn_seq_of_items(
        field, codec.resize, bref, codec.item_size, desc.lb, desc.ub(), desc.aligned, codec.unpack_item, table);
  }
  list_view list = codec.view(field);
  auto*     item = static_cast<uint8_t*>(codec.fixed_items(field));
  for (uint32_t i = 0; i != list.size; ++i, item += codec.item_size) {
    if (OCUDUASN_CODE ret = codec.unpack_item(item, bref, table); ret != OCUDUASN_SUCCESS) {
      return item_failure(ret, i);
    }
  }
  return OCUDUASN_SUCCESS;
}

void table::item_list_to_json(json_writer& j, const char* name, const void* field, const seq_field& desc)
{
  const auto& codec = *static_cast<const item_list_field_codec*>(desc.codec);
  list_view   list  = codec.view(field);
  const auto* item  = static_cast<const uint8_t*>(list.data);
  j.start_array(name);
  for (uint32_t i = 0; i != list.size; ++i, item += codec.item_size) {
    codec.item_to_json(j, item, desc.nested);
  }
  j.end_array();
}

void table::to_json_sequence(json_writer& j, const void* seq, const seq_desc& desc)
{
  const auto* base = static_cast<const uint8_t*>(seq);
  j.start_obj();
  fields_to_json(j, base, desc.root.fields, desc.root.names, desc.root.nof_fields);
  for (uint32_t g = 0; g != desc.nof_groups; ++g) {
    const seq_group& group = desc.groups[g];
    fields_to_json(j, base, group.set.fields, group.set.names, group.set.nof_fields);
  }
  j.end_obj();
}

void table::to_json_value(json_writer& j, const void* value, const seq_desc& desc)
{
  if (desc.message_name != nullptr) {
    to_json_message(j, value, desc, desc.message_name);
  } else {
    to_json_sequence(j, value, desc);
  }
}

void table::to_json_value(json_writer& j, const void* value, const choice_desc& desc)
{
  const auto* start   = static_cast<const uint8_t*>(value);
  const void* type    = desc.type_offset == choice_desc::no_member ? nullptr : start + desc.type_offset;
  const void* storage = desc.storage_offset == choice_desc::no_member ? nullptr : start + desc.storage_offset;
  if (desc.open_type_json) {
    to_json_open_type(j, type, storage, desc);
  } else {
    to_json_choice(j, type, storage, desc);
  }
}

void table::to_json_value(json_writer& j, const void* value, const ie_container_desc& desc)
{
  to_json_ie_container(j, value, desc);
}

OCUDUASN_CODE table::pack_value(bit_ref& bref, const void* value, const seq_desc& desc)
{
  return desc.message_name != nullptr ? pack_message(bref, value, desc) : pack_sequence(bref, value, desc);
}

OCUDUASN_CODE table::pack_value(bit_ref& bref, const void* value, const choice_desc& desc)
{
  const auto* start   = static_cast<const uint8_t*>(value);
  const void* type    = desc.type_offset == choice_desc::no_member ? nullptr : start + desc.type_offset;
  const void* storage = desc.storage_offset == choice_desc::no_member ? nullptr : start + desc.storage_offset;
  return desc.open_type ? pack_open_type(bref, type, storage, desc) : pack_choice(bref, type, storage, desc);
}

OCUDUASN_CODE table::pack_value(bit_ref& bref, const void* value, const ie_container_desc& desc)
{
  return pack_ie_container(bref, value, desc);
}

OCUDUASN_CODE table::unpack_value(void* value, cbit_ref& bref, const seq_desc& desc)
{
  return desc.message_name != nullptr ? unpack_message(value, bref, desc) : unpack_sequence(value, bref, desc);
}

OCUDUASN_CODE table::unpack_value(void* value, cbit_ref& bref, const choice_desc& desc)
{
  auto*       start   = static_cast<uint8_t*>(value);
  void*       storage = desc.storage_offset == choice_desc::no_member ? nullptr : start + desc.storage_offset;
  const void* type    = desc.type_offset == choice_desc::no_member ? nullptr : start + desc.type_offset;
  return desc.open_type ? unpack_open_type(type, storage, bref, desc) : unpack_choice(value, storage, bref, desc);
}

OCUDUASN_CODE table::unpack_value(void* value, cbit_ref& bref, const ie_container_desc& desc)
{
  return unpack_ie_container(value, bref, desc);
}

OCUDUASN_CODE table::pack_message(bit_ref& bref, const void* msg, const seq_desc& desc)
{
  if (OCUDUASN_CODE ret = pack_sequence(bref, msg, desc); ret != OCUDUASN_SUCCESS) {
    log_error("Encoding failure in message {}", desc.message_name != nullptr ? desc.message_name : "");
    return ret;
  }
  return bref.align_bytes_zero();
}

OCUDUASN_CODE table::unpack_message(void* msg, cbit_ref& bref, const seq_desc& desc)
{
  if (OCUDUASN_CODE ret = unpack_sequence(msg, bref, desc); ret != OCUDUASN_SUCCESS) {
    log_error("Decoding failure in message {}", desc.message_name != nullptr ? desc.message_name : "");
    return ret;
  }
  return bref.align_bytes();
}

void table::to_json_message(json_writer& j, const void* msg, const seq_desc& desc, const char* name)
{
  j.start_array();
  j.start_obj();
  j.write_fieldname(name);
  to_json_sequence(j, msg, desc);
  j.end_obj();
  j.end_array();
}

/// Index of the active alternative of a CHOICE. A CHOICE with a single alternative has no type.
static uint32_t choice_type_idx(const void* type, const table::choice_desc& desc)
{
  return type == nullptr ? 0 : static_cast<uint32_t>(load_unsigned(type, desc.type_itype));
}

/// Operations of the alternative of the given type, null when it has none, with a log when the type is invalid.
static const table::storage_ops* storage_alternative(uint32_t type, const table::choice_storage_desc& desc)
{
  if (type < desc.nof_types) {
    return desc.alternatives[type];
  }
  // The value after the last alternative is the null type, which stores nothing.
  if (type != desc.nof_types) {
    log_invalid_choice_id(type, desc.name);
  }
  return nullptr;
}

/// Destroys the value of the alternative of the given type, inline in the engine.
OCUDU_FORCE_INLINE void destroy_alternative(void* storage, uint32_t type, const table::choice_storage_desc& desc)
{
  // Destroying an invalid type does nothing, without a log.
  if (type < desc.nof_types and desc.alternatives[type] != nullptr and desc.alternatives[type]->destroy != nullptr) {
    desc.alternatives[type]->destroy(storage);
  }
}

void table::storage_destroy_choice(void* storage, uint32_t type, const choice_storage_desc& desc)
{
  destroy_alternative(storage, type, desc);
}

void table::storage_init_choice(void* storage, uint32_t type, const choice_storage_desc& desc)
{
  const storage_ops* ops = storage_alternative(type, desc);
  if (ops != nullptr and ops->init != nullptr) {
    ops->init(storage);
  }
}

void table::storage_copy_choice(void* storage, const void* other, uint32_t type, const choice_storage_desc& desc)
{
  const storage_ops* ops = storage_alternative(type, desc);
  if (ops == nullptr) {
    return;
  }
  if (ops->copy != nullptr) {
    ops->copy(storage, other);
  } else {
    std::memcpy(storage, other, ops->size);
  }
}

void table::storage_relocate_choice(void* storage, void* other, uint32_t type, const choice_storage_desc& desc)
{
  const storage_ops* ops = storage_alternative(type, desc);
  if (ops == nullptr) {
    return;
  }
  if (ops->relocate != nullptr) {
    ops->relocate(storage, other);
  } else {
    std::memcpy(storage, other, ops->size);
  }
}

void table::set_any_choice(void* storage, uint32_t type, const choice_desc& desc)
{
  if (type < desc.nof_types) {
    const seq_field& f = desc.alternatives[type];
    if (f.codec != nullptr and f.codec->init != nullptr) {
      f.codec->init(storage);
    }
  } else if (type != desc.nof_types) {
    // The value after the last alternative is the null type, which stores nothing.
    log_invalid_choice_id(type, desc.name);
  }
}

/// Makes the given alternative the active one of a CHOICE whose storage, if any, the engine manages.
static void set_choice_type(void* choice, void* value, uint32_t type, const table::choice_desc& desc)
{
  void* type_field = static_cast<uint8_t*>(choice) + desc.type_offset;
  if (desc.storage != nullptr) {
    destroy_alternative(value, load_unsigned(type_field, desc.type_itype), *desc.storage);
  }
  store_unsigned(type_field, desc.type_itype, type);
  if (desc.storage != nullptr) {
    table::storage_init_choice(value, type, *desc.storage);
  }
}

/// Packs the value of an extension alternative of a CHOICE.
[[gnu::noinline]] static OCUDUASN_CODE
pack_ext_alternative(bit_ref& bref, const void* value, const table::seq_field& f, bool aligned)
{
  // Extension alternatives are open types, even when their encoding is empty.
  return pack_varlength_field(
      bref, aligned, [&](bit_ref& contents) { return pack_field(contents, field_value(value, f), f); });
}

/// Unpacks the value of an extension alternative of a CHOICE.
[[gnu::noinline]] static OCUDUASN_CODE
unpack_ext_alternative(void* value, cbit_ref& bref, const table::seq_field& f, bool aligned)
{
  return unpack_varlength_field(
      bref, aligned, [&](cbit_ref& contents) { return unpack_field(field_value(value, f), contents, f); });
}

OCUDUASN_CODE table::pack_choice(bit_ref& bref, const void* type, const void* value, const choice_desc& desc)
{
  uint32_t type_idx = choice_type_idx(type, desc);
  HANDLE_CODE(pack_enum_value(bref, type_idx, desc.nof_types, desc.nof_exts, desc.has_ext));
  OCUDUASN_CODE ret =
      type_idx < desc.nof_types - desc.nof_exts
          ? pack_field(bref, field_value(value, desc.alternatives[type_idx]), desc.alternatives[type_idx])
          : pack_ext_alternative(bref, value, desc.alternatives[type_idx], desc.aligned);
  return ret == OCUDUASN_SUCCESS ? ret : alternative_failure(ret, desc, type_idx);
}

OCUDUASN_CODE table::unpack_choice(void* choice, void* value, cbit_ref& bref, const choice_desc& desc)
{
  uint32_t type_idx = 0;
  HANDLE_CODE(unpack_enum_value(type_idx, bref, desc.nof_types, desc.nof_exts, desc.has_ext));
  if (desc.set != nullptr) {
    desc.set(choice, type_idx);
  } else if (desc.type_offset != choice_desc::no_member) {
    set_choice_type(choice, value, type_idx, desc);
  }
  OCUDUASN_CODE ret =
      type_idx < desc.nof_types - desc.nof_exts
          ? unpack_field(field_value(value, desc.alternatives[type_idx]), bref, desc.alternatives[type_idx])
          : unpack_ext_alternative(value, bref, desc.alternatives[type_idx], desc.aligned);
  return ret == OCUDUASN_SUCCESS ? ret : alternative_failure(ret, desc, type_idx);
}

OCUDUASN_CODE table::pack_open_type(bit_ref& bref, const void* type, const void* value, const choice_desc& desc)
{
  return pack_varlength_field(bref, desc.aligned, [&](bit_ref& contents) {
    if (desc.nof_types == 0) {
      return OCUDUASN_SUCCESS;
    }
    uint32_t type_idx = choice_type_idx(type, desc);
    if (type_idx >= desc.nof_types) {
      log_invalid_choice_id(type_idx, desc.name);
      return OCUDUASN_ERROR_ENCODE_FAIL;
    }
    const table::seq_field& f   = desc.alternatives[type_idx];
    OCUDUASN_CODE           ret = pack_field(contents, field_value(value, f), f);
    return ret == OCUDUASN_SUCCESS ? ret : alternative_failure(ret, desc, type_idx);
  });
}

OCUDUASN_CODE table::unpack_open_type(const void* type, void* value, cbit_ref& bref, const choice_desc& desc)
{
  return unpack_varlength_field(bref, desc.aligned, [&](cbit_ref& contents) {
    if (desc.nof_types == 0) {
      return OCUDUASN_SUCCESS;
    }
    uint32_t type_idx = choice_type_idx(type, desc);
    if (type_idx >= desc.nof_types) {
      log_invalid_choice_id(type_idx, desc.name);
      return OCUDUASN_ERROR_DECODE_FAIL;
    }
    const table::seq_field& f   = desc.alternatives[type_idx];
    OCUDUASN_CODE           ret = unpack_field(field_value(value, f), contents, f);
    return ret == OCUDUASN_SUCCESS ? ret : alternative_failure(ret, desc, type_idx);
  });
}

void table::to_json_open_type(json_writer& j, const void* type, const void* value, const choice_desc& desc)
{
  if (desc.nof_types == 0) {
    j.write_null();
    return;
  }
  uint32_t type_idx = choice_type_idx(type, desc);
  if (type_idx >= desc.nof_types) {
    log_invalid_choice_id(type_idx, desc.name);
    return;
  }
  const seq_field& f     = desc.alternatives[type_idx];
  const void*      field = field_value(value, f);
  switch (f.op) {
    case field_op::int_u64:
      j.write_uint(load_unsigned(field, int_type::u64));
      break;
    case field_op::int_u8:
    case field_op::int_u16:
    case field_op::int_u32:
      j.write_int(static_cast<int64_t>(load_unsigned(field, op_int_type(f.op))));
      break;
    case field_op::int_i8:
    case field_op::int_i16:
    case field_op::int_i32:
    case field_op::int_i64:
      j.write_int(load_signed(field, op_int_type(f.op)));
      break;
    case field_op::boolean:
      j.write_bool(*static_cast<const bool*>(field));
      break;
    case field_op::none:
      if (desc.names[type_idx].value != nullptr) {
        j.write_str(desc.names[type_idx].value);
      } else {
        j.write_null();
      }
      break;
    case field_op::enum_u8:
    case field_op::enum_u16:
    case field_op::enum_u32:
    case field_op::enum_u64:
      if (f.nested != nullptr) {
        enum_to_json(j, "", field, f);
        break;
      }
      [[fallthrough]];
    case field_op::codec:
      f.codec->to_json(j, "", field, f);
      break;
    case field_op::sequence:
      to_json_sequence(j, field, *static_cast<const seq_desc*>(f.nested));
      break;
    case field_op::choice: {
      const auto& child = *static_cast<const choice_desc*>(f.nested);
      const auto* start = static_cast<const uint8_t*>(field);
      to_json_choice(j, choice_type_field(start, child), start + child.storage_offset, child);
      break;
    }
    case field_op::ie_container:
      to_json_ie_container(j, field, *static_cast<const ie_container_desc*>(f.nested));
      break;
  }
}

void table::to_json_choice(json_writer& j, const void* type, const void* value, const choice_desc& desc)
{
  uint32_t type_idx = choice_type_idx(type, desc);
  j.start_obj();
  if (type_idx < desc.nof_types) {
    const field_name& name = desc.names[type_idx];
    field_to_json(j, static_cast<const uint8_t*>(value), desc.alternatives[type_idx], name.name, name.value);
  } else {
    log_invalid_choice_id(type_idx, desc.name);
  }
  j.end_obj();
}

/************************
     IE containers
************************/

namespace {

static_assert(table::ie_container_desc::max_ies % 64 == 0, "The masks of the IEs are made of whole 64-bit words");

/// Whether an IE of a container is present, given its presence: IEs are mandatory or have a presence flag.
OCUDU_FORCE_INLINE bool ie_present(const uint8_t* base, int32_t presence)
{
  return presence < 0 or *reinterpret_cast<const bool*>(base + presence);
}

/// Packs the id and the criticality of an IE: the id in two octets between alignments, the criticality in 2 bits.
OCUDU_FORCE_INLINE OCUDUASN_CODE pack_ie_header(bit_ref& bref, uint32_t id, uint8_t crit)
{
  HANDLE_CODE(bref.align_bytes_zero());
  HANDLE_CODE(bref.pack(id, 16));
  HANDLE_CODE(bref.align_bytes_zero());
  return pack_enum_value(bref, crit, crit_e::nof_types, 0, false);
}

/// Unpacks the id and the criticality of an IE.
OCUDU_FORCE_INLINE OCUDUASN_CODE unpack_ie_header(uint32_t& id, uint32_t& crit, cbit_ref& bref)
{
  HANDLE_CODE(bref.align_bytes());
  HANDLE_CODE(bref.unpack(id, 16));
  HANDLE_CODE(bref.align_bytes());
  return unpack_enum_value(crit, bref, crit_e::nof_types, 0, false);
}

/// Packs one IE of a container.
OCUDU_FORCE_INLINE OCUDUASN_CODE pack_ie(bit_ref&                        bref,
                                         const uint8_t*                  base,
                                         const table::ie_container_desc& desc,
                                         uint32_t                        i)
{
  HANDLE_CODE(pack_ie_header(bref, desc.ids[i], desc.crits[i]));
  const table::seq_field& value = desc.values[i];
  return pack_varlength_field(bref, true, [&](bit_ref& contents) {
    return pack_field(contents, field_value(base + value.offset, value), value);
  });
}

} // namespace

OCUDUASN_CODE table::pack_ie_container(bit_ref& bref, const void* container, const ie_container_desc& desc)
{
  const auto* base = static_cast<const uint8_t*>(container);
  // Present IEs, IE i in bit i % 64 of word i / 64.
  std::array<uint64_t, ie_container_desc::max_ies / 64> present   = {};
  uint32_t                                              nof_ies   = 0;
  uint32_t                                              nof_words = (desc.nof_ies + 63) / 64;
  for (uint32_t w = 0; w != nof_words; ++w) {
    // Builds each word in a register, since an update of the array per IE waits on the previous store.
    uint64_t bits = 0;
    uint32_t end  = std::min(desc.nof_ies, (w + 1) * 64);
    for (uint32_t i = w * 64; i != end; ++i) {
      bits |= static_cast<uint64_t>(ie_present(base, desc.presences[i])) << (i % 64);
    }
    present[w] = bits;
    nof_ies += static_cast<uint32_t>(__builtin_popcountll(bits));
  }
  HANDLE_CODE(pack_length(bref, nof_ies, desc.len_lb, desc.len_ub, desc.aligned));
  for (uint32_t w = 0; w != nof_words; ++w) {
    for (uint64_t bits = present[w]; bits != 0; bits &= bits - 1) {
      uint32_t i = w * 64 + static_cast<uint32_t>(__builtin_ctzll(bits));
      if (OCUDUASN_CODE ret = pack_ie(bref, base, desc, i); ret != OCUDUASN_SUCCESS) {
        return ie_failure(ret, desc.ids[i]);
      }
    }
  }
  return OCUDUASN_SUCCESS;
}

OCUDUASN_CODE table::unpack_ie_container(void* container, cbit_ref& bref, const ie_container_desc& desc)
{
  auto*    base    = static_cast<uint8_t*>(container);
  uint32_t nof_ies = 0;
  HANDLE_CODE(unpack_length(nof_ies, bref, desc.len_lb, desc.len_ub, desc.aligned));
  uint32_t nof_mandatory = desc.nof_mandatory;
  // Decoded IEs, IE i in bit i % 64 of word i / 64, since a repeated IE would count twice as a mandatory one.
  std::array<uint64_t, ie_container_desc::max_ies / 64> decoded = {};
  for (; nof_ies > 0; --nof_ies) {
    uint32_t id;
    uint32_t crit_value;
    HANDLE_CODE(unpack_ie_header(id, crit_value, bref));
    crit_e   crit{static_cast<crit_e::options>(crit_value)};
    uint32_t idx = static_cast<uint32_t>(std::find(desc.ids, desc.ids + desc.nof_ies, id) - desc.ids);
    // The criticality that the object set defines for an id outside of it is the default one.
    bool known = idx != desc.nof_ies;
    if (not known) {
      log_error("The id={} is not recognized", id);
    }
    if (crit != (known ? crit_e{static_cast<crit_e::options>(desc.crits[idx])} : crit_e{})) {
      log_warning("Unpacked crit does not match object set definition");
    }
    if (not known) {
      HANDLE_CODE(skip_ignored_ie(bref, crit, id));
      continue;
    }
    uint64_t idx_bit = uint64_t{1} << (idx % 64);
    if ((decoded[idx / 64] & idx_bit) != 0) {
      log_error("The IE id={} is repeated", id);
      return OCUDUASN_ERROR_DECODE_FAIL;
    }
    decoded[idx / 64] |= idx_bit;
    const seq_field& value = desc.values[idx];
    if (value.presence >= 0) {
      *reinterpret_cast<bool*>(base + value.presence) = true;
    } else {
      nof_mandatory--;
    }
    if (OCUDUASN_CODE ret = unpack_varlength_field(
            bref,
            true,
            [&](cbit_ref& contents) { return unpack_field(field_value(base + value.offset, value), contents, value); });
        ret != OCUDUASN_SUCCESS) {
      return ie_failure(ret, id);
    }
  }
  if (nof_mandatory > 0) {
    log_error("Mandatory fields are missing\n");
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
  return OCUDUASN_SUCCESS;
}

void table::to_json_ie_container(json_writer& j, const void* container, const ie_container_desc& desc)
{
  const auto* base = static_cast<const uint8_t*>(container);
  j.start_array();
  for (uint32_t i = 0; i != desc.nof_ies; ++i) {
    if (not is_present(base, desc.values[i])) {
      continue;
    }
    j.start_obj();
    j.write_int("id", desc.ids[i]);
    j.write_str("criticality", crit_e{static_cast<crit_e::options>(desc.crits[i])}.to_string());
    field_to_json(j, base, desc.values[i], desc.value_name, nullptr);
    j.end_obj();
  }
  j.end_array();
}

} // namespace asn1
