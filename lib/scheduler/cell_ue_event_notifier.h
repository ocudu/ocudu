// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/du_types.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {

/// \brief Interface used to notify the UE scheduler of the outcome of an event that the cell handled.
///
/// It carries the outcomes whose handling needs the state shared by the UEs of the cell group, which the cell
/// scheduler has no access to.
class cell_ue_event_notifier
{
public:
  virtual ~cell_ue_event_notifier() = default;

  /// The Msg3 of a contention-free access was ACKed, which completes the contention resolution of the UE.
  virtual void on_cfra_msg3_acked(du_ue_index_t ue_index) = 0;

  /// The contention resolution CE of the UE was ACKed.
  virtual void on_conres_ce_acked(du_ue_index_t ue_index) = 0;

  /// A scheduling request of the UE was detected in the given UCI slot.
  virtual void on_sr_detected(du_ue_index_t ue_index, slot_point uci_slot) = 0;
};

/// \brief Relays the notifications of a cell to the UE scheduler.
///
/// It is handed to both sides at their creation, so that the cell can notify without knowing whether the UE
/// scheduler cell that consumes the notifications exists yet.
class cell_ue_event_relay final : public cell_ue_event_notifier
{
public:
  /// Set the notifier that the cell notifications are relayed to.
  void connect(cell_ue_event_notifier& notifier) { handler = &notifier; }

  void on_cfra_msg3_acked(du_ue_index_t ue_index) override
  {
    if (handler != nullptr) {
      handler->on_cfra_msg3_acked(ue_index);
    }
  }

  void on_conres_ce_acked(du_ue_index_t ue_index) override
  {
    if (handler != nullptr) {
      handler->on_conres_ce_acked(ue_index);
    }
  }

  void on_sr_detected(du_ue_index_t ue_index, slot_point uci_slot) override
  {
    if (handler != nullptr) {
      handler->on_sr_detected(ue_index, uci_slot);
    }
  }

private:
  cell_ue_event_notifier* handler = nullptr;
};

} // namespace ocudu
