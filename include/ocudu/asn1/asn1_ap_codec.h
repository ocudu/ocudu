// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Tables and codecs of the object sets, IE containers and information object classes of the AP layers.

#pragma once

#include "ocudu/asn1/asn1_ap_utils.h"
#include "ocudu/asn1/asn1_codec.h"

namespace asn1 {
namespace table {

/// Column of an object set, with the value of each object and the value for keys outside of the set. The values are
/// ENUMERATED values or alternatives of an open type.
struct objset_column {
  const uint8_t* values;
  uint8_t        missing;
};
/// Object set: the key of each object, and one column per class field other than the key.
struct objset_desc {
  const uint32_t*      keys;
  uint32_t             nof_keys;
  const char*          key_name;
  const objset_column* columns;
};

/// Protocol IE container whose IEs consist of an id, a criticality and an open type value. The ids and criticalities
/// are the keys and the criticality column of the object set of the IEs.
struct ie_container_desc {
  /// Maximum number of IEs, which the engine tracks in a mask of 64-bit words.
  static constexpr uint32_t max_ies = 256;

  const uint32_t* ids;
  /// Criticality of each IE, a crit_e::options.
  const uint8_t* crits;
  /// Value of each IE, which is encoded as an open type.
  const seq_field* values;
  /// Presence of each IE: seq_field::mandatory, or the offset of its presence flag.
  const int32_t* presences;
  uint32_t       nof_ies;
  /// Number of mandatory IEs.
  uint32_t nof_mandatory;
  /// Bounds of the number of IEs.
  uint32_t len_lb;
  uint32_t len_ub;
  bool     aligned;
  /// JSON name of the value of the IEs, such as "value" or "extensionValue".
  const char* value_name = nullptr;
};

OCUDUASN_CODE pack_ie_container(bit_ref& bref, const void* container, const ie_container_desc& desc);
OCUDUASN_CODE unpack_ie_container(void* container, cbit_ref& bref, const ie_container_desc& desc);
/// Writes the IE container as a JSON array of IEs. The values of the IEs must have JSON codecs, and the table the JSON
/// name of the values.
void to_json_ie_container(json_writer& j, const void* container, const ie_container_desc& desc);

/// Key of a class field of an information object class SEQUENCE, at the offset lb of the descriptor from the field.
template <class K>
const K& class_field_key(const void* field, const seq_field& desc)
{
  return *reinterpret_cast<const K*>(static_cast<const uint8_t*>(field) + desc.lb);
}
template <class T, class K, const objset_desc& Desc, auto Col>
OCUDUASN_CODE pack_checked_object(bit_ref& bref, const void* field, const seq_field& desc)
{
  const T& v = *static_cast<const T*>(field);
  warn_assert(v != objset_enum<T>(Desc, Col, class_field_key<K>(field, desc)), __func__, __LINE__);
  return asn1::pack(bref, v);
}
template <class T, class K, const objset_desc& Desc, auto Col>
OCUDUASN_CODE unpack_selected_object(void* field, cbit_ref& bref, const seq_field& desc)
{
  T& v = *static_cast<T*>(field);
  objset_select(v, Desc, Col, class_field_key<K>(field, desc));
  return asn1::unpack(v, bref);
}
/// Codec of a class field whose value is the column Col of the object set Desc for the key of type K, checked when
/// packed.
template <class T, class K, const objset_desc& Desc, auto Col, class J = no_json>
struct checked_codec {
  static constexpr field_codec value{&pack_checked_object<T, K, Desc, Col>,
                                     &unpack_object<T>,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     &object_to_json<T, J>};
};
template <class T, class K, const objset_desc& Desc, auto Col>
struct checked_codec<T, K, Desc, Col, no_json> {
  static constexpr field_codec value{&pack_checked_object<T, K, Desc, Col>,
                                     &unpack_object<T>,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr};
};
/// Codec of a class field of an open type, whose alternative the column Col of the object set Desc selects from the
/// key of type K before it is unpacked.
template <class T, class K, const objset_desc& Desc, auto Col, class J = no_json>
struct selected_codec {
  static constexpr field_codec value{&pack_object<T>,
                                     &unpack_selected_object<T, K, Desc, Col>,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     &object_to_json<T, J>};
};
template <class T, class K, const objset_desc& Desc, auto Col>
struct selected_codec<T, K, Desc, Col, no_json> {
  static constexpr field_codec value{&pack_object<T>,
                                     &unpack_selected_object<T, K, Desc, Col>,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr};
};

} // namespace table
} // namespace asn1
