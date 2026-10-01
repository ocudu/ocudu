// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/nr_cell_identity.h"
#include "ocudu/ran/plmn_identity.h"

namespace ocudu {

/// \brief The NR Cell Global Identity (NR-CGI).
struct nr_cell_global_id_t {
  nr_cell_global_id_t() = default;
  nr_cell_global_id_t(plmn_identity plmn_id_, nr_cell_identity nci_) : plmn_id(plmn_id_), nci(nci_) {}

  /// PLMN identity.
  plmn_identity plmn_id = plmn_identity::test_value();
  /// NR cell id.
  nr_cell_identity nci;

  bool operator==(const nr_cell_global_id_t& rhs) const { return plmn_id == rhs.plmn_id and nci == rhs.nci; }
  bool operator!=(const nr_cell_global_id_t& rhs) const { return !(rhs == *this); }
  bool operator<(const nr_cell_global_id_t& rhs) const
  {
    return (plmn_id < rhs.plmn_id) || (plmn_id == rhs.plmn_id && nci < rhs.nci);
  }
};

} // namespace ocudu

namespace fmt {

template <>
struct formatter<ocudu::nr_cell_global_id_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::nr_cell_global_id_t& val, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "[plmn={} nci={}]", val.plmn_id, val.nci);
  }
};

} // namespace fmt
