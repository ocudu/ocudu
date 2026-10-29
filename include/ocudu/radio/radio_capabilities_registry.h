// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/strong_type.h"
#include "ocudu/radio/radio_base.h"
#include <cstdint>
#include <string_view>

namespace ocudu {
namespace detail {

/// \brief Derives the fully qualified name of type \c T at compile time.
///
/// The name is parsed from the compiler-provided function signature, so it is not affected by RTTI being disabled.
///
/// \remark Supports the GCC (<tt>[with T = name; ...]</tt>) and Clang (<tt>[T = name]</tt>) signature formats.
template <typename T>
constexpr std::string_view derive_type_name()
{
  constexpr std::string_view signature = __PRETTY_FUNCTION__;
  constexpr std::string_view marker    = "T = ";

  constexpr size_t begin = signature.find(marker) + marker.size();
  constexpr size_t end   = signature.find_first_of(";]", begin);
  return signature.substr(begin, end - begin);
}

/// Computes the 64-bit FNV-1a hash of a string.
constexpr uint64_t type_name_hash(std::string_view str)
{
  uint64_t hash = 0xcbf29ce484222325;
  for (char c : str) {
    hash ^= static_cast<uint8_t>(c);
    hash *= 0x100000001b3;
  }
  return hash;
}

} // namespace detail

/// Identifies a radio capability.
using radio_capability_key = strong_type<uint64_t, struct radio_capability_key_tag, strong_equality>;

/// \brief Gets the key of the capability \c Capability.
///
/// The key is a hash of the fully qualified name of the interface type, computed at compile time. Hence, the
/// application and the radio plugin obtain the same key as long as they share the interface header and are built with
/// the same compiler family. Capability interfaces must not be declared in an anonymous namespace.
template <typename Capability>
constexpr radio_capability_key get_radio_capability_key()
{
  return radio_capability_key(detail::type_name_hash(detail::derive_type_name<Capability>()));
}

/// \brief Radio capabilities registry interface.
///
/// Allows the application to retrieve capabilities that the radio implements. The capability interfaces are defined by
/// the application and the radio implementation, so they are opaque to the radio interfaces and each radio only needs
/// to know the ones it supports.
///
/// \remark Capabilities are meant to be retrieved during setup. The returned pointers are valid for the lifetime of the
/// radio session.
class radio_capabilities_registry : public radio_base
{
public:
  /// \brief Gets the implementation of the capability \c Capability.
  ///
  /// \tparam Capability Capability interface type, as defined by the application.
  /// \return A pointer to the capability if the radio supports it, otherwise \c nullptr.
  template <typename Capability>
  Capability* get()
  {
    return static_cast<Capability*>(query(get_radio_capability_key<Capability>()));
  }

private:
  /// \brief Gets the capability identified by a key.
  ///
  /// Implemented by each radio, typically as a chain of comparisons against get_radio_capability_key() for every
  /// supported capability.
  ///
  /// \remark The returned pointer must address the capability interface subobject: convert to the capability interface
  /// type before converting to \c void*. Returning \c this directly yields a wrong address when the radio inherits from
  /// several interfaces.
  ///
  /// \param[in] key Key of the requested capability.
  /// \return A pointer to the capability implementation if supported, otherwise \c nullptr.
  virtual void* query(radio_capability_key key) = 0;
};

} // namespace ocudu
