// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Types shared by RRC and RRC NR.

#pragma once

#include "ocudu/asn1/asn1_dispatch.h"
#include <array>
#include <cstddef>

namespace asn1 {

/// Enumerated used in RRC and RRC NR that distinguishes Release and Setup modes
struct setup_release_opts {
  enum options { release, setup, nulltype } value;

  const char* to_string() const
  {
    static const char* names[] = {"release", "setup"};
    return convert_enum_idx(names, 2, value, "setup_release_c::types");
  }
};
using setup_release_e = enumerated<setup_release_opts>;

template <class T>
struct setup_release_c;

namespace detail {

template <class T>
OCUDUASN_CODE pack_setup(bit_ref& bref, const T& value)
{
  return asn1::pack(bref, value);
}
template <class T, size_t N>
OCUDUASN_CODE pack_setup(bit_ref& bref, const std::array<T, N>& value)
{
  return pack_fixed_seq_of(bref, value, N);
}
template <class T>
OCUDUASN_CODE unpack_setup(T& value, cbit_ref& bref)
{
  return asn1::unpack(value, bref);
}
template <class T, size_t N>
OCUDUASN_CODE unpack_setup(std::array<T, N>& value, cbit_ref& bref)
{
  return unpack_fixed_seq_of(value, bref, N);
}

template <class T>
OCUDUASN_CODE pack_setup_release(bit_ref& bref, const void* value)
{
  const auto& choice = *static_cast<const setup_release_c<T>*>(value);
  HANDLE_CODE(asn1::pack(bref, choice.type()));
  switch (choice.type()) {
    case setup_release_opts::release:
      return OCUDUASN_SUCCESS;
    case setup_release_opts::setup:
      return pack_setup(bref, choice.setup());
    default:
      log_invalid_choice_id(choice.type(), "setup_release_c");
      return OCUDUASN_ERROR_ENCODE_FAIL;
  }
}
template <class T>
OCUDUASN_CODE unpack_setup_release(void* value, cbit_ref& bref)
{
  auto&           choice = *static_cast<setup_release_c<T>*>(value);
  setup_release_e e;
  HANDLE_CODE(asn1::unpack(e, bref));
  switch (e) {
    case setup_release_opts::release:
      choice.set_release();
      return OCUDUASN_SUCCESS;
    case setup_release_opts::setup:
      return unpack_setup(choice.set_setup(), bref);
    default:
      log_invalid_choice_id(e, "setup_release_c");
      return OCUDUASN_ERROR_DECODE_FAIL;
  }
}
template <class T>
void setup_release_to_json(json_writer& j, const void* value)
{
  const auto& choice = *static_cast<const setup_release_c<T>*>(value);
  j.start_obj();
  switch (choice.type()) {
    case setup_release_opts::release:
      j.write_null("release");
      break;
    case setup_release_opts::setup:
      j.write_fieldname("setup");
      asn1::to_json(j, choice.setup());
      break;
    default:
      log_invalid_choice_id(choice.type(), "setup_release_c");
  }
  j.end_obj();
}

} // namespace detail

// SetupRelease{ElementTypeParam} ::= CHOICE
template <class T>
struct setup_release_c {
  using types_opts = setup_release_opts;
  using types      = setup_release_e;
  using value_type = T;

  // choice methods
  setup_release_c() = default;
  void  set(typename types::options e = types::nulltype) { type_ = e; }
  types type() const { return type_; }
  // getters
  bool is_setup() const { return type_.value == setup_release_opts::setup; }
  T&   setup()
  {
    assert_choice_type(types::setup, type_, "SetupRelease");
    return c;
  }
  const T& setup() const
  {
    assert_choice_type(types::setup, type_, "SetupRelease");
    return c;
  }
  void set_release() { set(types::release); }
  T&   set_setup()
  {
    set(types::setup);
    return c;
  }

  bool operator==(const setup_release_c<T>& other) const
  {
    return type_ == other.type_ and (type_ != types::setup or (c == other.c));
  }

  static constexpr table::custom_desc desc{&detail::pack_setup_release<T>,
                                           &detail::unpack_setup_release<T>,
                                           &detail::setup_release_to_json<T>};

  /// Offsets of the type and of the setup value, for a choice_desc of the type.
  static constexpr uint32_t type_offset() { return offsetof(setup_release_c, type_); }
  static constexpr uint32_t value_offset() { return offsetof(setup_release_c, c); }

private:
  types type_;
  T     c;
};

} // namespace asn1
