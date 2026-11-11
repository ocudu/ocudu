// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/asn1_dispatch.h"

namespace asn1 {

// Criticality ::= ENUMERATED
struct crit_opts {
  enum options { reject, ignore, notify, nulltype } value;
  static const table::enum_desc desc;
};
typedef enumerated<crit_opts> crit_e;

// Skips an unknown IE when its criticality is ignore.
OCUDUASN_CODE skip_ignored_ie(cbit_ref& bref, crit_e crit, uint32_t id);

// Presence ::= ENUMERATED
struct presence_opts {
  enum options { optional, conditional, mandatory, nulltype } value;

  static const table::enum_desc desc;
};
typedef enumerated<presence_opts> presence_e;

namespace table {

struct objset_desc;

/// Key of the object at index idx.
uint32_t objset_key(const objset_desc& desc, uint32_t idx);
/// Whether the object set has an object with the key.
bool objset_has_key(const objset_desc& desc, uint32_t key);
/// Logs an error when the object set has no object with the key.
void objset_check_key(const objset_desc& desc, uint32_t key);
/// Value of a column for the key, or its missing value, with an error log, when the set has no object with the key.
uint8_t objset_lookup(const objset_desc& desc, uint32_t column, uint32_t key);

/// ENUMERATED value of a column of the object set for the key.
template <class T, class Column>
T objset_enum(const objset_desc& desc, Column column, uint32_t key)
{
  return static_cast<typename T::options>(objset_lookup(desc, static_cast<uint32_t>(column), key));
}

/// Sets the open type to the alternative of a column of the object set for the key.
template <class T, class Column>
void objset_select(T& value, const objset_desc& desc, Column column, uint32_t key)
{
  // An open type with at most one alternative has nothing to select.
  if constexpr (T::types_opts::nulltype > 1) {
    value.set(static_cast<typename T::types::options>(objset_lookup(desc, static_cast<uint32_t>(column), key)));
  } else {
    objset_check_key(desc, key);
    value = T{};
  }
}

} // namespace table

/// Columns of the codec tables of the object sets of IEs and of extensions.
enum class ie_objset_column : uint32_t { crit, value, presence };

namespace detail {

struct empty_obj_set_item_c {
  struct types_opts {
    enum options { nulltype } value;
    const char* to_string() const;
  };
  typedef enumerated<types_opts> types;

  // choice methods
  types type() const { return types::nulltype; }

  static const table::custom_desc desc;
};

/// Object set without objects.
extern const table::objset_desc empty_objset_table;

} // namespace detail

/// Empty Protocol IE Object Set
struct protocol_ies_empty_o {
  using value_c = detail::empty_obj_set_item_c;

  static constexpr const table::objset_desc& desc = detail::empty_objset_table;
};

/// Empty Protocol Extension Object Set
struct protocol_ext_empty_o {
  using ext_c = detail::empty_obj_set_item_c;

  static constexpr const table::objset_desc& desc = detail::empty_objset_table;
};

namespace detail {

template <typename IEsSetParam>
struct ie_field_value_item {
  using obj_set_type = IEsSetParam;
  using value_type   = typename IEsSetParam::value_c;
  const char* item_name() const { return "value"; }
  void        set_item(uint32_t id) { table::objset_select(item, IEsSetParam::desc, ie_objset_column::value, id); }

protected:
  value_type item;
};

template <typename ExtensionSetParam>
struct ie_field_ext_item {
  using obj_set_type = ExtensionSetParam;
  using value_type   = typename ExtensionSetParam::ext_c;
  const char* item_name() const { return "extensionValue"; }
  void set_item(uint32_t id) { table::objset_select(item, ExtensionSetParam::desc, ie_objset_column::value, id); }

protected:
  value_type item;
};

template <class IEItem>
OCUDUASN_CODE pack_ie_field(bit_ref& bref, const void* value);
template <class IEItem>
OCUDUASN_CODE unpack_ie_field(void* value, cbit_ref& bref);
template <class IEItem>
void ie_field_to_json(json_writer& j, const void* value);

template <class IEItem>
struct base_ie_field : public IEItem {
  using obj_set_type = typename IEItem::obj_set_type;
  using value_type   = typename IEItem::value_type;

  uint32_t    id() const { return table::objset_key(obj_set_type::desc, value().type().value); }
  crit_e      crit() const { return table::objset_enum<crit_e>(obj_set_type::desc, ie_objset_column::crit, id()); }
  value_type& value() { return this->item; }
  const value_type& value() const { return this->item; }

  value_type*       operator->() { return &value(); }
  const value_type* operator->() const { return &value(); }
  value_type&       operator*() { return value(); }
  const value_type& operator*() const { return value(); }

  bool load_info_obj(const uint32_t& id_)
  {
    if (not table::objset_has_key(obj_set_type::desc, id_)) {
      return false;
    }
    this->set_item(id_);
    return value().type().value != obj_set_type::value_c::types_opts::nulltype;
  }

  static constexpr table::custom_desc desc{&pack_ie_field<IEItem>, &unpack_ie_field<IEItem>, &ie_field_to_json<IEItem>};
};

template <class IEItem>
OCUDUASN_CODE pack_ie_field(bit_ref& bref, const void* value)
{
  const auto& ie = *static_cast<const base_ie_field<IEItem>*>(value);
  HANDLE_CODE(pack_integer(bref, ie.id(), (uint32_t)0u, (uint32_t)65535u, false, true));
  HANDLE_CODE(asn1::pack(bref, ie.crit()));
  return asn1::pack(bref, ie.value());
}
template <class IEItem>
OCUDUASN_CODE unpack_ie_field(void* value, cbit_ref& bref)
{
  auto&    ie = *static_cast<base_ie_field<IEItem>*>(value);
  uint32_t id_val;
  HANDLE_CODE(unpack_integer(id_val, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
  ie.set_item(id_val);
  // The criticality of the IE is the one of its object set.
  crit_e crit;
  HANDLE_CODE(asn1::unpack(crit, bref));
  return asn1::unpack(ie.value(), bref);
}
template <class IEItem>
void ie_field_to_json(json_writer& j, const void* value)
{
  const auto& ie = *static_cast<const base_ie_field<IEItem>*>(value);
  j.start_obj();
  j.write_int("id", ie.id());
  j.write_str("criticality", ie.crit().to_string());
  j.write_fieldname(ie.item_name());
  asn1::to_json(j, ie.value());
  j.end_obj();
}

} // namespace detail

// ProtocolIE-Field{LAYER-PROTOCOL-IES : IEsSetParam} ::= SEQUENCE{{IEsSetParam}}
template <class IEsSetParam>
struct protocol_ie_field_s : public detail::base_ie_field<detail::ie_field_value_item<IEsSetParam>> {};

// ProtocolIE-SingleContainer{LAYER-PROTOCOL-IES : IEsSetParam} ::= SEQUENCE{{IEsSetParam}}
template <class ies_set_paramT_>
struct protocol_ie_single_container_s : public protocol_ie_field_s<ies_set_paramT_> {};

// ProtocolExtensionField{LAYER-PROTOCOL-EXTENSION : ExtensionSetParam} ::= SEQUENCE{{LAYER-PROTOCOL-EXTENSION}}
template <class ExtensionSetParam>
struct protocol_ext_field_s : public detail::base_ie_field<detail::ie_field_ext_item<ExtensionSetParam>> {};

namespace detail {

template <uint32_t LB>
OCUDUASN_CODE pack_empty_container(bit_ref& bref, const void*)
{
  return pack_length(bref, 0u, LB, 65535u, true);
}
template <uint32_t LB>
OCUDUASN_CODE unpack_empty_container(void*, cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  HANDLE_CODE(unpack_length(nof_ies, bref, LB, 65535u, true));
  // Every item is unknown, so it is skipped when its criticality allows it.
  for (; nof_ies > 0; --nof_ies) {
    uint32_t id = 0;
    crit_e   crit;
    HANDLE_CODE(unpack_integer(id, bref, (uint32_t)0u, (uint32_t)65535u, false, true));
    HANDLE_CODE(asn1::unpack(crit, bref));
    HANDLE_CODE(skip_ignored_ie(bref, crit, id));
  }
  return OCUDUASN_SUCCESS;
}
/// Writes the JSON of a container without items.
void empty_container_to_json(json_writer& j, const void* value);

} // namespace detail

/// ProtocolIE-Container or ProtocolExtensionContainer whose object set is empty, of at least LB items.
template <uint32_t LB>
struct protocol_container_empty_l {
  static constexpr table::custom_desc desc{&detail::pack_empty_container<LB>,
                                           &detail::unpack_empty_container<LB>,
                                           &detail::empty_container_to_json};
};

using protocol_ie_container_empty_l  = protocol_container_empty_l<0>;
using protocol_ext_container_empty_l = protocol_container_empty_l<1>;

/// PrivateIE-Container whose object set is empty. Unpacking fails when the container holds IEs.
struct private_ie_container_empty_l {
  static const table::custom_desc desc;
};

template <typename ProtocolIEs>
class elementary_procedure_option;

namespace detail {

template <typename ProtocolIEs>
OCUDUASN_CODE pack_elementary_procedure_option(bit_ref& bref, const void* value)
{
  const auto& option = *static_cast<const elementary_procedure_option<ProtocolIEs>*>(value);
  // No extension addition is defined.
  HANDLE_CODE(bref.pack(false, 1));
  return asn1::pack(bref, *option);
}
template <typename ProtocolIEs>
OCUDUASN_CODE unpack_elementary_procedure_option(void* value, cbit_ref& bref)
{
  auto& option = *static_cast<elementary_procedure_option<ProtocolIEs>*>(value);
  bool  ext;
  HANDLE_CODE(bref.unpack(ext, 1));
  HANDLE_CODE(asn1::unpack(*option, bref));
  if (ext) {
    // Skip extension additions unknown to this version.
    ext_groups_unpacker unknown_ext_unpacker(bref, true);
    HANDLE_CODE(unknown_ext_unpacker.consume_remaining_groups(bref));
  }
  return OCUDUASN_SUCCESS;
}
template <typename ProtocolIEs>
void elementary_procedure_option_to_json(json_writer& j, const void* value)
{
  const auto& option = *static_cast<const elementary_procedure_option<ProtocolIEs>*>(value);
  j.start_obj();
  j.write_fieldname("protocolIEs");
  asn1::to_json(j, *option);
  j.end_obj();
}

} // namespace detail

template <typename ProtocolIEs>
class elementary_procedure_option
{
  ProtocolIEs protocol_ies;

public:
  ProtocolIEs*       operator->() { return &protocol_ies; }
  const ProtocolIEs* operator->() const { return &protocol_ies; }
  ProtocolIEs&       operator*() { return protocol_ies; }
  const ProtocolIEs& operator*() const { return protocol_ies; }

  static constexpr table::custom_desc desc{&detail::pack_elementary_procedure_option<ProtocolIEs>,
                                           &detail::unpack_elementary_procedure_option<ProtocolIEs>,
                                           &detail::elementary_procedure_option_to_json<ProtocolIEs>};
};

} // namespace asn1
