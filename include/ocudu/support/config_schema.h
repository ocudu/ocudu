// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "CLI/CLI11.hpp"
#include <array>
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

/// \file
/// \brief Configuration-schema metadata model.
///
/// This header holds a lightweight, CLI11-independent description of an application's configuration: a tree of
/// named nodes (groups, leaves and arrays) carrying the type, default and description of every option. The tree is
/// populated as a side effect of the existing \c ocudu::add_option / \c add_subcommand / \c add_option_object_list
/// helpers (see \c cli11_utils.h) whenever the owning \c CLI::App has been registered against a schema root.
/// Applications that never register a root are unaffected: the helpers behave exactly as before.
///
/// Everything here is header-only and free of any dependency beyond CLI11 and the standard library. The value of a
/// default is captured from the typed C++ target variable, never from CLI11's string rendering, so it is always
/// correctly typed (e.g. a \c uint8_t default is a number, not a raw byte).

namespace ocudu {

// Forward-declared so capture_leaf can recognise bounded_integer<> option targets (e.g. arfcn_t, a
// bounded_integer<uint32_t, ...>) as integers rather than falling back to "string", without this dependency-light
// header pulling in the ADT. The complete type is always available wherever capture_leaf is instantiated.
template <typename Integer, Integer MinValue, Integer MaxValue>
class bounded_integer;

namespace config {

/// JSON-Schema scalar type of a leaf option.
enum class leaf_type { integer, number, boolean, string };

/// A single scalar default value, typed. \c std::string covers text (and enum names when applicable).
using schema_scalar = std::variant<std::int64_t, std::uint64_t, double, bool, std::string>;

/// The default value of a leaf, captured at registration from the typed target.
///
/// - \c present == false: the option has no meaningful default (e.g. an empty \c std::optional).
/// - scalar leaf: \c values holds exactly one element.
/// - array leaf: \c is_sequence == true and \c values holds the (possibly empty) list of element defaults.
struct schema_default {
  bool                       present     = false;
  bool                       is_sequence = false;
  std::vector<schema_scalar> values;
};

/// Value constraints of a leaf option (JSON Schema keywords). For a scalar-array leaf these apply to the array
/// items. Populated either from a first-class handle method (.range/.enum_values/...) or by parsing a CLI11
/// validator description at registration.
struct schema_constraints {
  std::optional<schema_scalar> minimum;
  std::optional<schema_scalar> maximum;
  std::optional<schema_scalar> exclusive_minimum;
  std::optional<schema_scalar> multiple_of; // JSON Schema "multipleOf": the value must be a multiple of this.
  std::vector<schema_scalar>   enums;
  std::optional<std::string>   pattern;    // JSON Schema "pattern" (ECMA-262 regex) for a string value.
  std::optional<std::uint64_t> min_length; // JSON Schema "minLength" for a string value.
  std::optional<std::uint64_t> max_length; // JSON Schema "maxLength" for a string value.

  bool empty() const
  {
    return !minimum && !maximum && !exclusive_minimum && !multiple_of && enums.empty() && !pattern && !min_length &&
           !max_length;
  }
};

/// Kind of a schema tree node.
enum class node_kind { root, group, leaf, array };

/// A node of the configuration schema tree.
///
/// - \c root / \c group: a section; \c children are its options and sub-sections.
/// - \c leaf: a scalar (or scalar-array) option; \c type / \c is_scalar_array / \c dflt describe it, and \c option
///   points at the CLI11 option so \c required can be read at emit time (a clean boolean, not string parsing).
/// - \c array: a list-of-struct option; \c children are the fields of one element (its item shape). \c exemplars
///   keep the throwaway CLI::App(s) used to capture that shape alive so the element options' \c required flags stay
///   readable until emission.
// Layout-sensitive: schema_node (and the schema_constraints it embeds by value) lives in this widely-included header,
// so changing its fields changes its sizeof in every schema translation unit. After such a change do a clean/forced
// rebuild: an incremental build that recompiles only some TUs yields an ABI mismatch (mixed struct sizes) and heap
// corruption at runtime.
struct schema_node {
  schema_node() = default;
  /// Constructs a node carrying \c node_description (used for the schema root; \ref register_schema_root sets the
  /// kind).
  explicit schema_node(std::string node_description) : description(std::move(node_description)) {}

  node_kind   kind = node_kind::group;
  std::string name;
  std::string description;

  // Leaf-only.
  leaf_type          type            = leaf_type::string;
  bool               is_scalar_array = false;
  schema_default     dflt;
  schema_constraints constraints;
  const CLI::Option* option = nullptr;

  // Group / root / array.
  std::vector<std::unique_ptr<schema_node>> children;

  // Array-only: exemplar apps whose element options were captured into \c children.
  std::vector<std::shared_ptr<CLI::App>> exemplars;

  schema_node* find_child(const std::string& child_name)
  {
    for (auto& c : children) {
      if (c->name == child_name) {
        return c.get();
      }
    }
    return nullptr;
  }
};

// ===========================================================================
// Registry: maps a CLI::App node to the schema node that its options attach to.
// Header-only singleton; populated only while an application registers its
// configuration. Registration is single-threaded at start-up.
// ===========================================================================

class schema_registry
{
public:
  void         attach(const CLI::App* app, schema_node* node) { map_[app] = node; }
  schema_node* lookup(const CLI::App* app) const
  {
    auto it = map_.find(app);
    return it == map_.end() ? nullptr : it->second;
  }
  void reset() { map_.clear(); }

private:
  std::unordered_map<const CLI::App*, schema_node*> map_;
};

inline schema_registry& registry()
{
  static schema_registry instance;
  return instance;
}

/// Registers \c root as the schema node backing top-level options of \c app. Call once, before registration.
inline void register_schema_root(CLI::App& app, schema_node& root)
{
  root.kind = node_kind::root;
  registry().attach(&app, &root);
}

// ===========================================================================
// Type-aware capture of a leaf's type and default from the typed target.
// ===========================================================================

namespace detail {

template <typename T>
struct is_optional : std::false_type {};
template <typename U>
struct is_optional<std::optional<U>> : std::true_type {
  using value_type = U;
};

template <typename T>
struct is_vector : std::false_type {};
template <typename U, typename A>
struct is_vector<std::vector<U, A>> : std::true_type {
  using value_type = U;
};

/// Detects std::array<U, N> (fixed-size scalar sequences such as ss1_n_candidates), so it is captured as a scalar
/// array just like std::vector. is_vector does not match std::array, so without this such options would fall through
/// to the scalar path and be emitted as a plain string.
template <typename T>
struct is_std_array : std::false_type {};
template <typename U, std::size_t N>
struct is_std_array<std::array<U, N>> : std::true_type {
  using value_type = U;
};

template <typename T>
struct is_duration : std::false_type {};
template <typename R, typename P>
struct is_duration<std::chrono::duration<R, P>> : std::true_type {};

// Matches ocudu::bounded_integer<Integer, MIN, MAX> (e.g. arfcn_t): a strong-typed integer that std::is_integral does
// not recognise, so without this it would be captured as a string. \c underlying is the wrapped integer type.
template <typename T>
struct is_bounded_integer : std::false_type {};
template <typename I, I MIN, I MAX>
struct is_bounded_integer<::ocudu::bounded_integer<I, MIN, MAX>> : std::true_type {
  using underlying = I;
};

/// JSON-Schema scalar type for a single (non-container) C++ type.
template <typename U>
constexpr leaf_type scalar_leaf_type()
{
  using D = std::decay_t<U>;
  if constexpr (std::is_same_v<D, bool>) {
    return leaf_type::boolean;
  } else if constexpr (std::is_floating_point_v<D>) {
    return leaf_type::number;
  } else if constexpr (std::is_enum_v<D> || is_duration<D>::value || std::is_integral_v<D> ||
                       is_bounded_integer<D>::value) {
    return leaf_type::integer;
  } else {
    return leaf_type::string;
  }
}

/// Typed default for a single scalar value.
template <typename U>
schema_scalar scalar_default(const U& v)
{
  using D = std::decay_t<U>;
  if constexpr (std::is_same_v<D, bool>) {
    return schema_scalar{v};
  } else if constexpr (std::is_floating_point_v<D>) {
    return schema_scalar{static_cast<double>(v)};
  } else if constexpr (std::is_enum_v<D>) {
    return schema_scalar{static_cast<std::int64_t>(static_cast<std::underlying_type_t<D>>(v))};
  } else if constexpr (is_duration<D>::value) {
    return schema_scalar{static_cast<std::int64_t>(v.count())};
  } else if constexpr (std::is_integral_v<D>) {
    if constexpr (std::is_signed_v<D>) {
      return schema_scalar{static_cast<std::int64_t>(v)};
    } else {
      return schema_scalar{static_cast<std::uint64_t>(v)};
    }
  } else if constexpr (is_bounded_integer<D>::value) {
    using I = typename is_bounded_integer<D>::underlying;
    if constexpr (std::is_signed_v<I>) {
      return schema_scalar{static_cast<std::int64_t>(v.value())};
    } else {
      return schema_scalar{static_cast<std::uint64_t>(v.value())};
    }
  } else if constexpr (std::is_same_v<D, std::string>) {
    return schema_scalar{v};
  } else if constexpr (std::is_constructible_v<std::string, D>) {
    return schema_scalar{std::string{v}};
  } else {
    // Unknown target type (e.g. a class with a custom lexical_cast): describe it as a string with no default.
    return schema_scalar{std::string{}};
  }
}

} // namespace detail

/// Description of a leaf derived from its typed target: its JSON-Schema type, whether it is a scalar array, and its
/// typed default.
struct leaf_info {
  leaf_type      type            = leaf_type::string;
  bool           is_scalar_array = false;
  schema_default dflt;
};

/// Captures the leaf description from the current value of the typed target \c value.
///
/// - \c std::optional<U>: type of \c U; a default only when engaged.
/// - \c std::vector<U>: scalar array of \c U; the default is the current (possibly empty) list.
/// - otherwise: the scalar type of the target and its current value as the default.
template <typename T>
leaf_info capture_leaf(const T& value)
{
  using D = std::decay_t<T>;
  leaf_info info;
  if constexpr (detail::is_optional<D>::value) {
    using U   = typename detail::is_optional<D>::value_type;
    info.type = detail::scalar_leaf_type<U>();
    if (value.has_value()) {
      info.dflt.present = true;
      info.dflt.values.push_back(detail::scalar_default<U>(*value));
    }
  } else if constexpr (detail::is_vector<D>::value) {
    using U               = typename detail::is_vector<D>::value_type;
    info.type             = detail::scalar_leaf_type<U>();
    info.is_scalar_array  = true;
    info.dflt.present     = true;
    info.dflt.is_sequence = true;
    for (const auto& element : value) {
      info.dflt.values.push_back(detail::scalar_default<U>(element));
    }
  } else if constexpr (detail::is_std_array<D>::value) {
    using U               = typename detail::is_std_array<D>::value_type;
    info.type             = detail::scalar_leaf_type<U>();
    info.is_scalar_array  = true;
    info.dflt.present     = true;
    info.dflt.is_sequence = true;
    for (const auto& element : value) {
      info.dflt.values.push_back(detail::scalar_default<U>(element));
    }
  } else {
    info.type         = detail::scalar_leaf_type<D>();
    info.dflt.present = true;
    info.dflt.values.push_back(detail::scalar_default<D>(value));
  }
  return info;
}

// ===========================================================================
// Recording helpers, invoked by the add_* helpers in cli11_utils.h. Each is a
// no-op when the owning app is not registered against a schema root.
// ===========================================================================

/// First option-name alias, stripped of surrounding whitespace and leading dashes (e.g. "--addrs,--addr" ->
/// "addrs"). Robust to stray leading whitespace in the option name (some call sites pass e.g. " --e2ap_level").
inline std::string schema_option_name(const std::string& option_name)
{
  std::string first = option_name.substr(0, option_name.find(','));
  std::size_t begin = first.find_first_not_of(" \t-");
  if (begin == std::string::npos) {
    return first;
  }
  std::size_t end = first.find_last_not_of(" \t");
  return first.substr(begin, end - begin + 1);
}

/// Converts a raw string (from a CLI11 validator description) to a typed scalar following the leaf type.
inline schema_scalar to_scalar(leaf_type type, const std::string& raw)
{
  std::string s     = raw;
  std::size_t begin = s.find_first_not_of(" \t");
  std::size_t end   = s.find_last_not_of(" \t");
  s                 = (begin == std::string::npos) ? std::string{} : s.substr(begin, end - begin + 1);
  if (type == leaf_type::integer) {
    try {
      std::size_t consumed = 0;
      long long   v        = std::stoll(s, &consumed);
      if (consumed == s.size()) {
        return schema_scalar{static_cast<std::int64_t>(v)};
      }
    } catch (...) {
    }
    // Fall back to unsigned for bounds above INT64_MAX (e.g. a uint64 option's maximum).
    try {
      std::size_t        consumed = 0;
      unsigned long long v        = std::stoull(s, &consumed);
      if (consumed == s.size()) {
        return schema_scalar{static_cast<std::uint64_t>(v)};
      }
    } catch (...) {
    }
  } else if (type == leaf_type::number) {
    try {
      std::size_t consumed = 0;
      double      v        = std::stod(s, &consumed);
      if (consumed == s.size()) {
        return schema_scalar{v};
      }
    } catch (...) {
    }
  } else if (type == leaf_type::boolean) {
    return schema_scalar{s == "true" || s == "1"};
  }
  return schema_scalar{s};
}

/// Converts a numeric value (from a first-class constraint method) to a typed scalar following the leaf type.
inline schema_scalar to_scalar(leaf_type type, double v)
{
  return (type == leaf_type::integer) ? schema_scalar{static_cast<std::int64_t>(v)} : schema_scalar{v};
}

/// Records value constraints on \c leaf from a CLI11 validator's resolved description string (its get_description(),
/// or get_name() when the description is empty). Recognises Range ("<TYPE> in [<min> - <max>]"), IsMember
/// ("{a,b,c}"), NONNEGATIVE and POSITIVE; anything else is left as type-only. No-op when \c leaf is null.
inline void record_validator_constraint(schema_node* leaf, const std::string& desc)
{
  if (leaf == nullptr || desc.empty()) {
    return;
  }
  // Range: "<TYPE> in [<min> - <max>]".
  const std::string marker = " in [";
  std::size_t       mpos   = desc.find(marker);
  if (mpos != std::string::npos && desc.back() == ']') {
    std::string inner = desc.substr(mpos + marker.size(), desc.size() - (mpos + marker.size()) - 1);
    std::size_t sep   = inner.find(" - ");
    if (sep != std::string::npos) {
      leaf->constraints.minimum = to_scalar(leaf->type, inner.substr(0, sep));
      leaf->constraints.maximum = to_scalar(leaf->type, inner.substr(sep + 3));
      return;
    }
  }
  // IsMember: "{a,b,c}".
  if (desc.size() >= 2 && desc.front() == '{' && desc.back() == '}') {
    std::string inner = desc.substr(1, desc.size() - 2);
    std::size_t start = 0;
    while (!inner.empty()) {
      std::size_t comma = inner.find(',', start);
      std::string item  = inner.substr(start, comma == std::string::npos ? std::string::npos : comma - start);
      leaf->constraints.enums.push_back(to_scalar(leaf->type, item));
      if (comma == std::string::npos) {
        break;
      }
      start = comma + 1;
    }
    return;
  }
  if (desc == "NONNEGATIVE") {
    leaf->constraints.minimum = to_scalar(leaf->type, std::string("0"));
    return;
  }
  if (desc == "POSITIVE") {
    leaf->constraints.exclusive_minimum = to_scalar(leaf->type, std::string("0"));
    return;
  }
  // Unknown validator (e.g. a custom check): leave the leaf type-only.
}

/// Returns the leaf node to (re)populate when recording an option \c name under \c parent, or nullptr if \c name is
/// already an array (list-of-struct) node that must be preserved. A repeated declaration of the same leaf name - a
/// shared node populated by several units, as in the gNB - resets the node so the LAST declaration wins, matching
/// CLI11's merge-aware add_option (which removes the earlier option, keeping the last one's default and validators).
/// Array options are unioned instead (see \ref record_array), so an existing array node is never overwritten.
inline schema_node* leaf_slot(schema_node* parent, const std::string& name)
{
  if (schema_node* existing = parent->find_child(name)) {
    if (existing->kind == node_kind::array) {
      return nullptr;
    }
    existing->kind            = node_kind::leaf;
    existing->is_scalar_array = false;
    existing->dflt            = {};
    existing->constraints     = {};
    existing->option          = nullptr;
    return existing;
  }
  auto node        = std::make_unique<schema_node>();
  node->name       = name;
  node->kind       = node_kind::leaf;
  schema_node* raw = node.get();
  parent->children.push_back(std::move(node));
  return raw;
}

/// Records a scalar (or scalar-array) leaf option under \c app's node and returns it. A repeated declaration of the
/// same name updates the node in place (last declaration wins, see \ref leaf_slot); nullptr when the app is not
/// registered against a root.
template <typename T>
schema_node* record_option(const CLI::App&    app,
                           const std::string& option_name,
                           const T&           target,
                           const std::string& description,
                           const CLI::Option* option)
{
  schema_node* parent = registry().lookup(&app);
  if (parent == nullptr) {
    return nullptr;
  }
  schema_node* leaf = leaf_slot(parent, schema_option_name(option_name));
  if (leaf == nullptr) {
    return parent->find_child(schema_option_name(option_name));
  }
  leaf_info info        = capture_leaf(target);
  leaf->description     = description;
  leaf->type            = info.type;
  leaf->is_scalar_array = info.is_scalar_array;
  leaf->dflt            = std::move(info.dflt);
  leaf->option          = option;
  return leaf;
}

/// Records a boolean flag leaf under \c app's node and returns it (see \ref record_option for the return contract).
inline schema_node* record_flag(const CLI::App&    app,
                                const std::string& option_name,
                                const std::string& description,
                                const CLI::Option* option)
{
  schema_node* parent = registry().lookup(&app);
  if (parent == nullptr) {
    return nullptr;
  }
  schema_node* leaf = leaf_slot(parent, schema_option_name(option_name));
  if (leaf == nullptr) {
    return parent->find_child(schema_option_name(option_name));
  }
  leaf->description = description;
  leaf->type        = leaf_type::boolean;
  leaf->option      = option;
  return leaf;
}

/// Records a leaf option whose value is parsed by a custom function (add_option_function). The JSON-Schema type is
/// taken from the function's value type \c T (e.g. integer for add_option_function<unsigned>, string for an enum
/// name parser); there is no readable target variable, so no default is captured.
template <typename T>
schema_node* record_function_option(const CLI::App&    app,
                                    const std::string& option_name,
                                    const std::string& description,
                                    const CLI::Option* option)
{
  schema_node* parent = registry().lookup(&app);
  if (parent == nullptr) {
    return nullptr;
  }
  schema_node* leaf = leaf_slot(parent, schema_option_name(option_name));
  if (leaf == nullptr) {
    return parent->find_child(schema_option_name(option_name));
  }
  leaf->description = description;
  if constexpr (detail::is_vector<std::decay_t<T>>::value) {
    // A custom-parsed list of scalars (e.g. add_option_function<std::vector<std::string>>): a scalar array.
    leaf->type            = detail::scalar_leaf_type<typename detail::is_vector<std::decay_t<T>>::value_type>();
    leaf->is_scalar_array = true;
  } else {
    leaf->type = detail::scalar_leaf_type<T>();
  }
  leaf->option = option;
  return leaf;
}

/// Binds an option group to the schema node of its parent app. CLI11 option groups share the parent's
/// configuration namespace, so options added to the group are top-level options of the parent; recording them
/// against the parent's node makes them appear as its properties instead of being skipped.
inline void record_option_group(const CLI::App& parent_app, const CLI::App& group_app)
{
  if (schema_node* parent = registry().lookup(&parent_app)) {
    registry().attach(&group_app, parent);
  }
}

/// Records (or finds) a subcommand group \c name under \c parent_app and binds \c child_app to it.
inline void record_subcommand(const CLI::App&    parent_app,
                              const CLI::App&    child_app,
                              const std::string& name,
                              const std::string& description)
{
  schema_node* parent = registry().lookup(&parent_app);
  if (parent == nullptr) {
    return;
  }
  if (schema_node* existing = parent->find_child(name)) {
    registry().attach(&child_app, existing);
    return;
  }
  auto group         = std::make_unique<schema_node>();
  group->kind        = node_kind::group;
  group->name        = name;
  group->description = description;
  schema_node* raw   = group.get();
  parent->children.push_back(std::move(group));
  registry().attach(&child_app, raw);
}

/// Records (or finds) a list-of-struct array option \c option_name under \c app's node and binds \c exemplar_app to
/// the array node so the element configurator populates the item shape (the array node's children). Repeated
/// declarations of the same option (shared node used by several units) union their element fields; the array node's
/// description comes from the first declaration, while duplicate element fields follow the last-wins rule of \ref
/// leaf_slot.
inline void record_array(const CLI::App&           app,
                         const std::string&        option_name,
                         const std::string&        description,
                         std::shared_ptr<CLI::App> exemplar_app)
{
  schema_node* parent = registry().lookup(&app);
  if (parent == nullptr) {
    return;
  }
  std::string  name  = schema_option_name(option_name);
  schema_node* array = parent->find_child(name);
  if (array == nullptr) {
    auto node         = std::make_unique<schema_node>();
    node->kind        = node_kind::array;
    node->name        = name;
    node->description = description;
    array             = node.get();
    parent->children.push_back(std::move(node));
  }
  array->exemplars.push_back(exemplar_app);
  registry().attach(exemplar_app.get(), array);
}

} // namespace config
} // namespace ocudu
