// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ntn_satellite_config.h"
#include "ocudu/adt/span.h"
#include "ocudu/ntn/ntn_configuration_manager_config.h"
#include <optional>
#include <string>
#include <vector>

namespace ocudu {

/// \brief Appends the globally-defined satellites (mandatory satellite_idx) to \p out_satellites.
///
/// The user-defined satellite_idx is used as the internal satellite_index.
/// \return The next free satellite index for auto-assigned inline satellite definitions.
unsigned add_global_ntn_satellites(span<const ntn_satellite_config>              ntn_satellites,
                                   std::vector<ocudu_ntn::ntn_satellite_config>& out_satellites);

/// \brief Ensures \p sat_ref references a resolved satellite.
///
/// If satellite_idx is unset, appends an inline satellite definition built from the reference to \p out_satellites,
/// assigning the next free index. Reports an error when neither satellite_idx nor an inline definition
/// (epoch_timestamp and ephemeris_info) is present.
/// \param sat_ref Satellite reference to resolve. Its satellite_idx is guaranteed set on return.
/// \param out_satellites Resolved satellite list that inline definitions are appended to.
/// \param next_satellite_idx Next free auto-assigned satellite index; advanced when an inline satellite is added.
/// \param ta_info TA info override to store in an inline satellite definition.
/// \param err_context Context string used in the error message for an unresolvable reference.
void resolve_ntn_satellite_ref(ntn_satellite_config&                         sat_ref,
                               std::vector<ocudu_ntn::ntn_satellite_config>& out_satellites,
                               unsigned&                                     next_satellite_idx,
                               const std::optional<ta_info_t>&               ta_info,
                               const std::string&                            err_context);

/// Derives the ephemeris format (ECEF state vector vs ECI orbital parameters) for an NTN entity (serving
/// cell, sat-switch target, or neighbor cell): the explicit override if set, else the variant of its own inline
/// ephemeris_info, else (if it references a shared satellite_idx) the variant of the referenced satellite's
/// ephemeris_info.
std::optional<bool> derive_use_state_vector(std::optional<bool>                         configured_value,
                                            const std::optional<ntn_ephemeris_info_t>&  own_ephemeris_info,
                                            unsigned                                    satellite_index,
                                            span<const ocudu_ntn::ntn_satellite_config> resolved_satellites);

} // namespace ocudu
