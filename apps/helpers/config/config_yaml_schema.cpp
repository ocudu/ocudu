// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "apps/helpers/config/config_yaml_schema.h"
#include "ocudu/support/error_handling.h"
#include "CLI/CLI11.hpp"
#include <algorithm>
#include <fstream>
#include <yaml-cpp/yaml.h>

using namespace ocudu;
using namespace ocudu::config;

namespace {

/// Collapses embedded newlines in a description to single spaces (multi-line C++ strings would break YAML comments
/// and read poorly in a schema) and trims the result, so a description ending in '\n' (multi-line CLI help) or with
/// stray edge spaces is not needlessly quoted by the YAML emitter.
std::string single_line(const std::string& s)
{
  std::string out = s;
  std::replace(out.begin(), out.end(), '\n', ' ');
  const auto first = out.find_first_not_of(" \t");
  if (first == std::string::npos) {
    return {};
  }
  return out.substr(first, out.find_last_not_of(" \t") - first + 1);
}

/// JSON-Schema type keyword for a leaf type.
const char* type_keyword(leaf_type t)
{
  switch (t) {
    case leaf_type::integer:
      return "integer";
    case leaf_type::number:
      return "number";
    case leaf_type::boolean:
      return "boolean";
    case leaf_type::string:
      break;
  }
  return "string";
}

/// Renders a single typed scalar default as a YAML node.
YAML::Node scalar_node(const schema_scalar& value)
{
  return std::visit([](const auto& v) { return YAML::Node(v); }, value);
}

/// Renders a captured default (scalar or sequence) as a YAML node.
YAML::Node default_node(const schema_default& dflt)
{
  if (dflt.is_sequence) {
    YAML::Node seq(YAML::NodeType::Sequence);
    for (const schema_scalar& v : dflt.values) {
      seq.push_back(scalar_node(v));
    }
    return seq;
  }
  return dflt.values.empty() ? YAML::Node(YAML::NodeType::Null) : scalar_node(dflt.values.front());
}

bool is_required(const schema_node& leaf)
{
  return leaf.option != nullptr && leaf.option->get_required();
}

/// Writes the value constraints (minimum/maximum/exclusiveMinimum/enum) onto \c target.
void add_constraints(const schema_constraints& constraints, YAML::Node& target)
{
  if (constraints.minimum) {
    target["minimum"] = scalar_node(*constraints.minimum);
  }
  if (constraints.maximum) {
    target["maximum"] = scalar_node(*constraints.maximum);
  }
  if (constraints.exclusive_minimum) {
    target["exclusiveMinimum"] = scalar_node(*constraints.exclusive_minimum);
  }
  if (constraints.multiple_of) {
    target["multipleOf"] = scalar_node(*constraints.multiple_of);
  }
  if (!constraints.enums.empty()) {
    YAML::Node values(YAML::NodeType::Sequence);
    for (const schema_scalar& v : constraints.enums) {
      values.push_back(scalar_node(v));
    }
    target["enum"] = values;
  }
  if (constraints.pattern) {
    target["pattern"] = *constraints.pattern;
  }
  if (constraints.min_length) {
    target["minLength"] = *constraints.min_length;
  }
  if (constraints.max_length) {
    target["maxLength"] = *constraints.max_length;
  }
}

// Forward declaration for recursion.
void fill_object(const schema_node& node, YAML::Node& out);

/// Builds the schema fragment describing a single leaf option.
YAML::Node leaf_schema(const schema_node& leaf)
{
  YAML::Node node(YAML::NodeType::Map);
  if (!leaf.description.empty()) {
    node["description"] = single_line(leaf.description);
  }

  if (leaf.is_scalar_array) {
    // A std::vector<T> option accepts, in CLI11, EITHER a single value OR a list: a lone scalar in the config is
    // parsed as a one-element vector. Configs rely on this - e.g. an SCTP multi-homing "addrs" list is almost always
    // written as a single address. Describe the option as "scalar OR array of scalars" (oneOf) so the schema accepts
    // both forms exactly as CLI11 does; an array-only type would wrongly reject the common single-value case. The
    // item constraints apply to the scalar and to each array element alike, so both branches carry them.
    const char* item_type = type_keyword(leaf.type);

    YAML::Node scalar_form(YAML::NodeType::Map);
    scalar_form["type"] = item_type;
    add_constraints(leaf.constraints, scalar_form);

    YAML::Node array_items(YAML::NodeType::Map);
    array_items["type"] = item_type;
    add_constraints(leaf.constraints, array_items);
    YAML::Node array_form(YAML::NodeType::Map);
    array_form["type"]  = "array";
    array_form["items"] = array_items;

    YAML::Node one_of(YAML::NodeType::Sequence);
    one_of.push_back(scalar_form);
    one_of.push_back(array_form);
    node["oneOf"] = one_of;
  } else {
    node["type"] = type_keyword(leaf.type);
    add_constraints(leaf.constraints, node);
  }

  // A required option has no meaningful default (omission is an error, not a fall-back).
  if (leaf.dflt.present && !is_required(leaf)) {
    node["default"] = default_node(leaf.dflt);
  }
  return node;
}

/// Builds the schema fragment describing a list-of-struct (array) option: an array of objects whose item shape is
/// the array node's children.
YAML::Node array_schema(const schema_node& array)
{
  YAML::Node node(YAML::NodeType::Map);
  if (!array.description.empty()) {
    node["description"] = single_line(array.description);
  }
  node["type"] = "array";

  YAML::Node items(YAML::NodeType::Map);
  fill_object(array, items);
  node["items"] = items;
  return node;
}

/// Fills \c out with { type: object, properties: {...}, required: [...] } for a group/root/array-item node.
void fill_object(const schema_node& node, YAML::Node& out)
{
  out["type"] = "object";

  YAML::Node               properties(YAML::NodeType::Map);
  std::vector<std::string> required;

  for (const auto& child : node.children) {
    switch (child->kind) {
      case node_kind::leaf:
        properties[child->name] = leaf_schema(*child);
        if (is_required(*child)) {
          required.push_back(child->name);
        }
        break;
      case node_kind::array:
        properties[child->name] = array_schema(*child);
        break;
      case node_kind::group:
      case node_kind::root: {
        YAML::Node group(YAML::NodeType::Map);
        if (!child->description.empty()) {
          group["description"] = single_line(child->description);
        }
        fill_object(*child, group);
        properties[child->name] = group;
        break;
      }
    }
  }

  out["properties"] = properties;
  // Reject unknown keys: the apps parse config with CLI::config_extras_mode::error, so a key absent from the option
  // set is a user error (typo, stale option). Mirror that in the schema rather than silently accepting extras.
  out["additionalProperties"] = false;
  if (!required.empty()) {
    YAML::Node req(YAML::NodeType::Sequence);
    for (const std::string& name : required) {
      req.push_back(name);
    }
    out["required"] = req;
  }
}

} // namespace

std::string ocudu::app_helpers::generate_yaml_config_schema(const schema_node& root,
                                                            const std::string& title,
                                                            const std::string& id_slug)
{
  YAML::Node doc;
  // Plain JSON Schema (draft-07) serialised as YAML: the document uses only standard JSON Schema keywords
  // (type/properties/required/enum/minimum/oneOf/additionalProperties/...), so any JSON Schema tool - check-jsonschema
  // in CI, the YAML language server for editor autocompletion - can validate a YAML (or JSON) config against it. $id
  // gives each application's schema a stable identity under the ocudu.org namespace.
  doc["$schema"] = "http://json-schema.org/draft-07/schema#";
  if (!id_slug.empty()) {
    doc["$id"] = "https://ocudu.org/schemas/" + id_slug + ".schema.json";
  }
  doc["title"] = title;
  // Skip a redundant top-level description when it would merely repeat the title (register_config_schema titles the
  // schema with the root's description).
  if (!root.description.empty() && single_line(root.description) != title) {
    doc["description"] = single_line(root.description);
  }
  fill_object(root, doc);

  return YAML::Dump(doc) + "\n";
}

void ocudu::app_helpers::register_config_schema(CLI::App& app, schema_node& root, const std::string& id_slug)
{
  register_schema_root(app, root);

  // Registered directly on CLI11 (not through the schema-aware helpers) so the flag itself does not appear in the
  // schema. The callback triggers as soon as the option is parsed, before requirement checks, so the schema can be
  // produced without a valid configuration; an optional value selects an output file, otherwise it goes to stdout.
  // The schema is titled with the root's description.
  app.add_option(
         "--emit-config-schema",
         [&root, id_slug](const CLI::results_t& values) -> bool {
           const std::string schema = generate_yaml_config_schema(root, root.description, id_slug);
           if (values.empty() || values.front().empty()) {
             fmt::print("{}", schema);
           } else {
             std::ofstream out(values.front());
             if (!out) {
               report_error("Could not open '{}' for writing the configuration schema.\n", values.front());
             }
             out << schema;
           }
           throw CLI::Success();
         },
         "Emit the YAML configuration schema (optionally to the given file path) and exit")
      ->expected(0, 1)
      ->configurable(false)
      ->trigger_on_parse();
}
