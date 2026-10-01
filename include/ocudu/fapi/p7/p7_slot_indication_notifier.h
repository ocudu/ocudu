// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {
namespace fapi {

struct slot_indication;

/// \brief P7 slot indication message notifier interface.
///
/// This interface notifies the reception of slot indications from the underlying PHY.
class p7_slot_indication_notifier
{
public:
  virtual ~p7_slot_indication_notifier() = default;

  /// \brief Notifies the reception of a slot indication message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_slot_indication(const slot_indication& msg) = 0;
};

} // namespace fapi
} // namespace ocudu
