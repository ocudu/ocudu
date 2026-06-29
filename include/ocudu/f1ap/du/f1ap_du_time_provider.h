// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/slot_point.h"
#include <optional>

namespace ocudu {
namespace odu {

/// Slot-to-time mapping used by the F1AP DU reference time reporting procedure.
struct f1ap_du_slot_time_info {
  slot_point ref_slot;
  /// PER-encoded ReferenceTime-r16 (TS 38.331 section 6.3.2), packed by the adapter implementation.
  byte_buffer ref_time_r16;
  /// When true, ref_time_r16 is relative to a local (non-GPS) clock; GPS origin (6 Jan 1980) is assumed otherwise.
  bool is_local_clock = true;
};

/// \brief Abstract time source used by the F1AP DU reference time reporting procedure.
///
/// Decouples the F1AP DU procedure layer from the MAC subframe time mapper interface.
/// Implementations wrap mac_subframe_time_mapper or supply synthetic data in tests.
class f1ap_du_time_provider
{
public:
  virtual ~f1ap_du_time_provider() = default;

  /// \brief Return the most recent slot-to-time mapping, if available.
  virtual std::optional<f1ap_du_slot_time_info> get_last_mapping(subcarrier_spacing scs) = 0;
};

} // namespace odu
} // namespace ocudu
