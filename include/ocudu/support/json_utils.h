// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "nlohmann/json.hpp"
#include "ocudu/adt/expected.h"

// Validate struct field definitions.
namespace ocudu {
using JsonValidatorErrorMsg = std::string;

template <typename T>
struct JsonValidator {
  static expected<bool, JsonValidatorErrorMsg> validate(const nlohmann::json& j, const std::string& key)
  {
    auto it = j.find(key);
    if (it == j.end()) {
      return make_unexpected(fmt::format("{}: mandatory field missing", key));
    }

    if constexpr (std::is_same_v<T, bool>) {
      if (!it->is_boolean()) {
        return make_unexpected(fmt::format("{}: Field must be a boolean", key));
      }
      return true;
    } else if constexpr (std::is_same_v<T, std::string>) {
      if (!it->is_string()) {
        return make_unexpected(fmt::format("{}: Field must be a string", key));
      }
      return true;
    } else if constexpr (std::is_unsigned_v<T>) {
      if (!it->is_number_unsigned()) {
        return make_unexpected(fmt::format("{}: Field must be an unsigned integer", key));
      }
      return true;
    } else if constexpr (std::is_integral_v<T>) {
      if (!it->is_number_integer()) {
        return make_unexpected(fmt::format("{}: Field must be an integer", key));
      }
      return true;
    } else if constexpr (std::is_arithmetic_v<T>) {
      if (!it->is_number()) {
        return make_unexpected(fmt::format("{}: Field must be a number", key));
      }
      return true;
    }
    return make_unexpected(fmt::format("{}: No validator defined for json type: {}", key, j.type_name()));
  }
};

#define _OCUDU_VALIDATE_STRUCT_FIELD(field, json_alias)                                                                \
  result = JsonValidator<std::decay_t<decltype(obj.field)>>::validate(json[key], json_alias);                          \
  if (!result) {                                                                                                       \
    return make_unexpected(fmt::format("{}.{}", key, result.error()));                                                 \
  }

#define _OCUDU_VALIDATE_STRUCT_FIELD_IF_PRESENT(field, json_alias)                                                     \
  if (json.at(key).contains(json_alias)) {                                                                             \
    _OCUDU_VALIDATE_STRUCT_FIELD(field, json_alias)                                                                    \
  }

#define _OCUDU_VALIDATE_STRUCT_FIELD_required(field, json_alias) _OCUDU_VALIDATE_STRUCT_FIELD(field, json_alias)
#define _OCUDU_VALIDATE_STRUCT_FIELD_optional(field, json_alias)                                                       \
  _OCUDU_VALIDATE_STRUCT_FIELD_IF_PRESENT(field, json_alias)

// Select proper UNPACK macro based on field properties.
#define _OCUDU_VALIDATE_STRUCT_UNPACK_DEF_SIMPLE(field) _OCUDU_VALIDATE_STRUCT_FIELD(field, #field)
#define _OCUDU_VALIDATE_STRUCT_UNPACK_DEF_1(field) _OCUDU_VALIDATE_STRUCT_FIELD(field, #field)
#define _OCUDU_VALIDATE_STRUCT_UNPACK_DEF_2(field, json_alias) _OCUDU_VALIDATE_STRUCT_FIELD(field, json_alias)
#define _OCUDU_VALIDATE_STRUCT_UNPACK_DEF_3(field, json_alias, mode)                                                   \
  _OCUDU_VALIDATE_STRUCT_FIELD_##mode(field, json_alias)
#define _OCUDU_VALIDATE_STRUCT_UNPACK_DEF_4(field, json_alias, mode, default_val)                                      \
  _OCUDU_VALIDATE_STRUCT_FIELD_##mode(field, json_alias)

#define _OCUDU_VALIDATE_STRUCT_UNPACK_DEF(...)                                                                         \
  _OCUDU_GET_MACRO(__VA_ARGS__,                                                                                        \
                   _OCUDU_VALIDATE_STRUCT_UNPACK_DEF_4,                                                                \
                   _OCUDU_VALIDATE_STRUCT_UNPACK_DEF_3,                                                                \
                   _OCUDU_VALIDATE_STRUCT_UNPACK_DEF_2,                                                                \
                   _OCUDU_VALIDATE_STRUCT_UNPACK_DEF_1)                                                                \
  (__VA_ARGS__)

#define _OCUDU_VALIDATE_STRUCT_FIELDS_DEF(tuple) _OCUDU_VALIDATE_STRUCT_UNPACK_DEF tuple

#define _OCUDU_VALIDATE_STRUCT_FIELDS_DEF_LIST(...) NLOHMANN_JSON_PASTE(_OCUDU_VALIDATE_STRUCT_FIELDS_DEF, __VA_ARGS__)

#define _OCUDU_JSON_DEFINE_STRUCT_VALIDATOR_SIMPLE(NAME, ...)                                                          \
  template <>                                                                                                          \
  struct JsonValidator<NAME> {                                                                                         \
    static expected<bool, JsonValidatorErrorMsg> validate(const nlohmann::json& json, const std::string& key)          \
    {                                                                                                                  \
      NAME                                  obj;                                                                       \
      expected<bool, JsonValidatorErrorMsg> result;                                                                    \
      NLOHMANN_JSON_PASTE(_OCUDU_VALIDATE_STRUCT_UNPACK_DEF_SIMPLE, __VA_ARGS__)                                       \
      return true;                                                                                                     \
    }                                                                                                                  \
  };

#define _OCUDU_JSON_DEFINE_STRUCT_VALIDATOR(NAME, ...)                                                                 \
  template <>                                                                                                          \
  struct JsonValidator<NAME> {                                                                                         \
    static expected<bool, JsonValidatorErrorMsg> validate(const nlohmann::json& json, const std::string& key)          \
    {                                                                                                                  \
      NAME                                  obj;                                                                       \
      expected<bool, JsonValidatorErrorMsg> result;                                                                    \
      _OCUDU_VALIDATE_STRUCT_FIELDS_DEF_LIST(__VA_ARGS__)                                                              \
      return true;                                                                                                     \
    }                                                                                                                  \
  };

template <typename T>
expected<nlohmann::basic_json<>, JsonValidatorErrorMsg> validate_json(const nlohmann::json& j, const std::string& key)
{
  auto result = JsonValidator<T>::validate(j, key);
  if (!result) {
    return make_unexpected(result.error());
  }
  return j.at(key);
}
} // namespace ocudu

//// STRUCT with json aliases, optional fields and default values ////
// Define behavior for TO JSON.
#define _OCUDU_STRUCT_FIELD_TO_JSON(field, json_alias) json[json_alias] = obj.field;
#define _OCUDU_STRUCT_FIELD_TO_JSON_WITH_DEFAULT(field, json_alias, default_val)                                       \
  if (obj.field != default_val) {                                                                                      \
    json[json_alias] = obj.field;                                                                                      \
  };
// Define behavior for FROM JSON.
#define _OCUDU_STRUCT_FIELD_FROM_JSON(field, json_alias) json.at(json_alias).get_to(obj.field);
#define _OCUDU_STRUCT_FIELD_FROM_JSON_IF_PRESENT(field, json_alias)                                                    \
  if (json.contains(json_alias)) {                                                                                     \
    json.at(json_alias).get_to(obj.field);                                                                             \
  };
#define _OCUDU_STRUCT_FIELD_FROM_JSON_WITH_DEFAULT(field, json_alias, default_val)                                     \
  if (json.contains(json_alias)) {                                                                                     \
    json.at(json_alias).get_to(obj.field);                                                                             \
  } else {                                                                                                             \
    obj.field = default_val;                                                                                           \
  }

// Define mapping for TO JSON macros.
#define _OCUDU_STRUCT_FIELD_TO_required(field, json_alias) _OCUDU_STRUCT_FIELD_TO_JSON(field, json_alias)
#define _OCUDU_STRUCT_FIELD_TO_optional(field, json_alias) _OCUDU_STRUCT_FIELD_TO_JSON(field, json_alias)
#define _OCUDU_STRUCT_FIELD_TO_with_default(field, json_alias, default_val)                                            \
  _OCUDU_STRUCT_FIELD_TO_JSON_WITH_DEFAULT(field, json_alias, default_val)
// Define mapping for FROM JSON macros.
#define _OCUDU_STRUCT_FIELD_FROM_required(field, json_alias) _OCUDU_STRUCT_FIELD_FROM_JSON(field, json_alias)
#define _OCUDU_STRUCT_FIELD_FROM_optional(field, json_alias) _OCUDU_STRUCT_FIELD_FROM_JSON_IF_PRESENT(field, json_alias)
#define _OCUDU_FIELD_FROM_with_default(field, json_alias, default_val)                                                 \
  _OCUDU_STRUCT_FIELD_FROM_JSON_WITH_DEFAULT(field, json_alias, default_val)

#define _OCUDU_GET_MACRO(_1, _2, _3, _4, NAME, ...) NAME
// Select proper TO macro based on field properties.
#define _OCUDU_STRUCT_FIELD_TO_1(field) _OCUDU_STRUCT_FIELD_TO_required(field, #field)
#define _OCUDU_STRUCT_FIELD_TO_2(field, json_alias) _OCUDU_STRUCT_FIELD_TO_required(field, json_alias)
#define _OCUDU_STRUCT_FIELD_TO_3(field, json_alias, mode) _OCUDU_STRUCT_FIELD_TO_##mode(field, json_alias)
#define _OCUDU_STRUCT_FIELD_TO_4(field, json_alias, mode, default_val)                                                 \
  _OCUDU_STRUCT_FIELD_TO_with_default(field, json_alias, default_val)
#define _OCUDU_STRUCT_FIELD_TO(...)                                                                                    \
  _OCUDU_GET_MACRO(__VA_ARGS__,                                                                                        \
                   _OCUDU_STRUCT_FIELD_TO_4,                                                                           \
                   _OCUDU_STRUCT_FIELD_TO_3,                                                                           \
                   _OCUDU_STRUCT_FIELD_TO_2,                                                                           \
                   _OCUDU_STRUCT_FIELD_TO_1)                                                                           \
  (__VA_ARGS__)
// Select proper FROM macro based on field properties.
#define _OCUDU_STRUCT_FIELD_FROM_1(field) _OCUDU_STRUCT_FIELD_FROM_required(field, #field)
#define _OCUDU_STRUCT_FIELD_FROM_2(field, json_alias) _OCUDU_STRUCT_FIELD_FROM_required(field, json_alias)
#define _OCUDU_STRUCT_FIELD_FROM_3(field, json_alias, mode) _OCUDU_STRUCT_FIELD_FROM_##mode(field, json_alias)
#define _OCUDU_STRUCT_FIELD_FROM_4(field, json_alias, mode, default_val)                                               \
  _OCUDU_FIELD_FROM_with_default(field, json_alias, default_val)
#define _OCUDU_STRUCT_FIELD_FROM(...)                                                                                  \
  _OCUDU_GET_MACRO(__VA_ARGS__,                                                                                        \
                   _OCUDU_STRUCT_FIELD_FROM_4,                                                                         \
                   _OCUDU_STRUCT_FIELD_FROM_3,                                                                         \
                   _OCUDU_STRUCT_FIELD_FROM_2,                                                                         \
                   _OCUDU_STRUCT_FIELD_FROM_1)                                                                         \
  (__VA_ARGS__)

// Unpack (field, json_alias, mode, optional(default_val)) and apply both TO/FROM macros.
#define _OCUDU_UNPACK_AND_PROCESS_STRUCT_FIELDS_DEFS_TO(tuple) _OCUDU_STRUCT_FIELD_TO tuple
#define _OCUDU_UNPACK_AND_PROCESS_STRUCT_FIELDS_DEFS_FROM(tuple) _OCUDU_STRUCT_FIELD_FROM tuple

// Process struct field definitions.
#define _OCUDU_PROCESS_STRUCT_FIELDS_DEFS_TO(...)                                                                      \
  NLOHMANN_JSON_PASTE(_OCUDU_UNPACK_AND_PROCESS_STRUCT_FIELDS_DEFS_TO, __VA_ARGS__)
#define _OCUDU_PROCESS_STRUCT_FIELDS_DEFS_FROM(...)                                                                    \
  NLOHMANN_JSON_PASTE(_OCUDU_UNPACK_AND_PROCESS_STRUCT_FIELDS_DEFS_FROM, __VA_ARGS__)

//// STRUCT simple ////
#define _OCUDU_STRUCT_FIELD_TO_JSON_SIMPLE(MEMBER_NAME) json[#MEMBER_NAME] = obj.MEMBER_NAME;
#define _OCUDU_STRUCT_FIELD_FROM_JSON_SIMPLE(MEMBER_NAME) json.at(#MEMBER_NAME).get_to(obj.MEMBER_NAME);

#define OCUDU_JSON_DEFINE_TYPE_STRUCT_SIMPLE(NAME, ...)                                                                \
  _OCUDU_JSON_DEFINE_STRUCT_VALIDATOR_SIMPLE(NAME, __VA_ARGS__)                                                        \
  void to_json(nlohmann::json& json, const NAME& obj)                                                                  \
  {                                                                                                                    \
    NLOHMANN_JSON_PASTE(_OCUDU_STRUCT_FIELD_TO_JSON_SIMPLE, __VA_ARGS__)                                               \
  }                                                                                                                    \
  void from_json(const nlohmann::json& json, NAME& obj)                                                                \
  {                                                                                                                    \
    NLOHMANN_JSON_PASTE(_OCUDU_STRUCT_FIELD_FROM_JSON_SIMPLE, __VA_ARGS__)                                             \
  }

// Generate to_json and from_json funcs for a struct.
#define OCUDU_JSON_DEFINE_TYPE_STRUCT(NAME, ...)                                                                       \
  _OCUDU_JSON_DEFINE_STRUCT_VALIDATOR(NAME, __VA_ARGS__)                                                               \
  inline void to_json(nlohmann::json& json, const NAME& obj)                                                           \
  {                                                                                                                    \
    _OCUDU_PROCESS_STRUCT_FIELDS_DEFS_TO(__VA_ARGS__)                                                                  \
  }                                                                                                                    \
  inline void from_json(const nlohmann::json& json, NAME& obj)                                                         \
  {                                                                                                                    \
    _OCUDU_PROCESS_STRUCT_FIELDS_DEFS_FROM(__VA_ARGS__)                                                                \
  }
