// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "fmt/format.h"

namespace ocudu {

/// GTP-U logical interface type.
enum class gtpu_logical_interface {
  /// F1-U at CU-UP.
  f1u_cu_up,
  /// F1-U at DU.
  f1u_du,
  /// NG-U (N3) at CU-UP.
  ngu,
  /// Xn-U at CU-UP.
  xnu,
  /// Invalid.
  invalid
};

inline const char* to_string(gtpu_logical_interface li)
{
  switch (li) {
    case gtpu_logical_interface::f1u_cu_up:
      return "F1-U(CU-UP)";
    case gtpu_logical_interface::f1u_du:
      return "F1-U(DU)";
    case gtpu_logical_interface::ngu:
      return "NG-U";
    case gtpu_logical_interface::xnu:
      return "Xn-U";
    default:
      return "invalid";
  }
}

} // namespace ocudu

namespace fmt {

// GTP-U logical interface
template <>
struct formatter<ocudu::gtpu_logical_interface> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::gtpu_logical_interface li, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "{}", ocudu::to_string(li));
  }
};

} // namespace fmt
