// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ran/nr_cell_identity.h"
#include "ocudu/ran/reference_location.h"
#include "ocudu/ran/tac.h"
#include <algorithm>
#include <optional>
#include <vector>

namespace ocudu {

/// One geographic area of an NTN cell, mapped to a TAC for the UE Location Derived TAC in NR NTN IE of TS 38.413, to
/// a Mapped Cell ID of TS 38.300 sec. 16.14.5, or to both. Either may be listed more than once to cover ground that
/// is not a single rectangle.
struct ntn_location_area {
  /// TAC reported for this area as the UE Location Derived TAC in NR NTN, TS 38.413. Absent in an area that reports
  /// none, leaving the AMF the broadcast TAI.
  std::optional<tac_t> tac;
  /// Mapped Cell ID reported for this area, TS 38.300 sec. 16.14.5. Absent in an area that reports the Uu Cell ID of
  /// the serving cell. Not checked against the cells this gNB serves: it names an area agreed between RAN and core,
  /// and sec. 16.14.5 NOTE 3 allows special values outside the serving PLMN's country.
  std::optional<nr_cell_identity> mapped_nci;
  double                          lat_min;
  double                          lat_max;
  double                          lon_min;
  double                          lon_max;

  /// Whether a position falls inside this area. Bounds are inclusive, so areas sharing an edge overlap there.
  bool contains(const reference_location& loc) const
  {
    return loc.latitude >= lat_min and loc.latitude <= lat_max and loc.longitude >= lon_min and
           loc.longitude <= lon_max;
  }
};

/// \brief Maps a coarse UE location to a TAC, for the UE Location Derived TAC in NR NTN IE of TS 38.413, and to a
/// Mapped Cell ID, TS 38.300 sec. 16.14.5.
///
/// Empty in a cell without a configured mapping, in which case neither is ever derived.
struct ntn_location_mapping {
  /// Areas in configuration order. The first area containing the position wins.
  std::vector<ntn_location_area> location_areas;

  bool empty() const { return location_areas.empty(); }

  /// Whether any area reports \c nci as its Mapped Cell ID, TS 38.300 sec. 16.14.5.
  bool reports_mapped_cell_id(nr_cell_identity nci) const
  {
    return std::any_of(location_areas.begin(), location_areas.end(), [nci](const ntn_location_area& area) {
      return area.mapped_nci.has_value() and area.mapped_nci.value() == nci;
    });
  }
};

/// \brief The location mapping configured for one NTN cell.
struct ntn_cell_location_mapping {
  nr_cell_identity     nci;
  ntn_location_mapping mapping;
};

/// \brief Derives the TAC to report for a coarse UE location, TS 38.413 UE Location Derived TAC in NR NTN.
///
/// The derived TAC is the tracking area the UE is geographically in, TS 23.502 sec. 4.10, which need not be one the
/// cell broadcasts: a TAC change in system information is not synchronised with the illumination on ground,
/// TS 38.300 sec. 16.14.3.1, so a UE outside the broadcast areas is the case this IE exists to report.
///
/// \param[in] mapping  Areas configured for the serving cell.
/// \param[in] position Position reported by the UE.
/// \return The derived TAC, or nullopt when the position matches no configured area.
std::optional<tac_t> derive_tac_from_location(const ntn_location_mapping& mapping, const reference_location& position);

/// \brief Derives the Mapped Cell ID to report for a coarse UE location, TS 38.300 sec. 16.14.5.
///
/// \param[in] mapping  Areas configured for the serving cell.
/// \param[in] position Position reported by the UE.
/// \return The Mapped Cell ID of the area holding the position, or nullopt when the position matches no configured
/// area, or matches one that configures no Mapped Cell ID. The caller then reports the Uu Cell ID, which
/// sec. 16.14.5 leaves in place wherever no Mapped Cell ID applies.
std::optional<nr_cell_identity> derive_mapped_cell_id_from_location(const ntn_location_mapping& mapping,
                                                                    const reference_location&   position);

} // namespace ocudu
