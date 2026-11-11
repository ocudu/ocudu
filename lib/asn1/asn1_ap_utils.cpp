// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Criticality and presence enums, empty object sets and IE containers, and object set lookups of AP layers.

#include "ocudu/asn1/asn1_ap_codec.h"

namespace asn1 {

/************************
   General Layer Types
************************/

static const char*     crit_names[]    = {"reject", "ignore", "notify"};
const table::enum_desc crit_opts::desc = {crit_names, 3, "crit_e"};

static const char*     presence_names[]    = {"optional", "conditional", "mandatory"};
const table::enum_desc presence_opts::desc = {presence_names, 3, "presence_e"};

OCUDUASN_CODE skip_ignored_ie(cbit_ref& bref, crit_e crit, uint32_t id)
{
  if (crit != crit_e::ignore) {
    log_error("Unpacked object ID={} is not recognized", id);
    return OCUDUASN_ERROR_DECODE_FAIL;
  }
  uint32_t len;
  HANDLE_CODE(unpack_length(len, bref, true));
  return bref.advance_bytes(len);
}

// The columns of the criticality, the value and the presence.
static const table::objset_column empty_objset_columns[]     = {{nullptr, crit_e::nulltype},
                                                                {nullptr, detail::empty_obj_set_item_c::types::nulltype},
                                                                {nullptr, presence_e::nulltype}};
const table::objset_desc          detail::empty_objset_table = {nullptr, 0, "id", empty_objset_columns};

static OCUDUASN_CODE pack_empty_obj_set_item(bit_ref& bref, const void*)
{
  varlength_field_pack_guard varlen_scope(bref, true);
  return OCUDUASN_SUCCESS;
}
static OCUDUASN_CODE unpack_empty_obj_set_item(void*, cbit_ref& bref)
{
  varlength_field_unpack_guard varlen_scope(bref, true);
  return OCUDUASN_SUCCESS;
}
static void empty_obj_set_item_to_json(json_writer& j, const void*)
{
  j.start_obj();
  j.end_obj();
}
const char* detail::empty_obj_set_item_c::types_opts::to_string() const
{
  log_error("The enum value=0 of type protocol_ies_empty_o::value_c::types is not valid.");
  return "";
}
const table::custom_desc detail::empty_obj_set_item_c::desc = {&pack_empty_obj_set_item,
                                                               &unpack_empty_obj_set_item,
                                                               &empty_obj_set_item_to_json};

void detail::empty_container_to_json(json_writer& j, const void*)
{
  j.start_array();
  j.end_array();
}

static OCUDUASN_CODE pack_private_ie_container_empty(bit_ref& bref, const void*)
{
  return pack_length(bref, 0u, 1u, 65535u, true);
}
static OCUDUASN_CODE unpack_private_ie_container_empty(void*, cbit_ref& bref)
{
  uint32_t nof_ies = 0;
  HANDLE_CODE(unpack_length(nof_ies, bref, 1u, 65535u, true));
  // The IDs of private IEs aren't integers, so unknown private IEs can't be skipped.
  return nof_ies > 0 ? OCUDUASN_ERROR_DECODE_FAIL : OCUDUASN_SUCCESS;
}
const table::custom_desc private_ie_container_empty_l::desc = {&pack_private_ie_container_empty,
                                                               &unpack_private_ie_container_empty,
                                                               &detail::empty_container_to_json};

/************************
      object sets
************************/

/// Index of the key among the keys of the object set, or the number of keys when the set has no such key.
static uint32_t objset_index(const table::objset_desc& desc, uint32_t key)
{
  uint32_t idx = 0;
  while (idx != desc.nof_keys and desc.keys[idx] != key) {
    ++idx;
  }
  return idx;
}

uint32_t table::objset_key(const objset_desc& desc, uint32_t idx)
{
  return map_enum_number(desc.keys, desc.nof_keys, idx, desc.key_name);
}

bool table::objset_has_key(const objset_desc& desc, uint32_t key)
{
  return objset_index(desc, key) != desc.nof_keys;
}

void table::objset_check_key(const objset_desc& desc, uint32_t key)
{
  if (not objset_has_key(desc, key)) {
    log_error("The {}={} is not recognized", desc.key_name, key);
  }
}

uint8_t table::objset_lookup(const objset_desc& desc, uint32_t column, uint32_t key)
{
  uint32_t idx = objset_index(desc, key);
  if (idx == desc.nof_keys) {
    log_error("The {}={} is not recognized", desc.key_name, key);
    return desc.columns[column].missing;
  }
  return desc.columns[column].values[idx];
}

} // namespace asn1
