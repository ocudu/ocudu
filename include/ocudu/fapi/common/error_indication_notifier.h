// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace fapi {

struct error_indication;

/// \brief Error-specific message notifier interface.
///
/// This interface listens to FAPI error indication events and translates them to the suitable data types
/// for the MAC layer.
class error_indication_notifier
{
public:
  virtual ~error_indication_notifier() = default;

  /// \brief Notifies the reception of an error indication message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_error_indication(const error_indication& msg) = 0;
};

} // namespace fapi
} // namespace ocudu
