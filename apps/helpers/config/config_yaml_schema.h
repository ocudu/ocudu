// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/config_schema.h"
#include <string>

namespace ocudu {
namespace app_helpers {

/// \brief Generates a JSON Schema document (serialised as YAML) describing the configuration captured in \c root.
///
/// The output is plain JSON Schema draft-07 (\c $schema: http://json-schema.org/draft-07/schema#) written as YAML:
/// every option becomes a typed property, sections become nested objects, and list-of-struct options become arrays
/// of objects. Because JSON is a subset of YAML and only standard JSON Schema keywords are used, any JSON Schema tool
/// (check-jsonschema, the YAML language server, ...) can validate a YAML or JSON config against it. Required options
/// are listed in the enclosing object's \c required array and carry no default.
///
/// \param root Fully-populated configuration-schema tree (see config_schema.h).
/// \param title Human-readable schema title.
/// \param id_slug Optional application slug (e.g. "gnb", "du", "cu-cp"); when non-empty it becomes the schema's
///                \c $id (https://ocudu.org/schemas/<slug>.schema.json).
/// \return The schema as a YAML document (ending in a newline).
std::string
generate_yaml_config_schema(const config::schema_node& root, const std::string& title, const std::string& id_slug = "");

/// \brief Registers \c root as \c app's configuration-schema root and adds a --emit-config-schema option.
///
/// Combines \ref config::register_schema_root with a CLI11 option that emits the YAML schema built from \c root - to
/// the optional file-path argument, or to stdout - and exits. The schema is titled with \c root's description. The
/// option is registered directly on CLI11 rather than through the schema-aware helpers, so it does not itself appear
/// in the schema; it triggers on parse, before requirement checks, so the schema can be produced without a valid
/// configuration. This keeps the emit boilerplate out of the individual application main files.
///
/// \param app Application the schema root and option are attached to.
/// \param root Schema root node; kept alive by the caller. Its description is the schema title (see \ref schema_node).
/// \param id_slug Application slug (e.g. "gnb", "du", "cu-cp") used as the schema's \c $id.
void register_config_schema(CLI::App& app, config::schema_node& root, const std::string& id_slug);

} // namespace app_helpers
} // namespace ocudu
