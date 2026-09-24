// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"

namespace ocudu {
namespace ofh {

class data_flow_message_decoding_metrics_collector;

/// Open Fronthaul User-Plane uplink PRACH data flow.
class data_flow_uplane_uplink_prach
{
public:
  /// Default destructor.
  virtual ~data_flow_uplane_uplink_prach() = default;

  /// \brief Decodes the given Open Fronthaul message associated to the given eAxC.
  ///
  /// This function receives the argument \c is_seq_id_correct, that indicates that the sequence identifier of the
  /// message matches the expected value. This information is used by the data flow metrics collector as per
  /// O-RAN.WG4.TS.CUS.0-R005-v21.00 in section 9.1 to update the corresponding KPI.
  virtual void decode_type1_message(unsigned eaxc, span<const uint8_t> message, bool is_seq_id_correct) = 0;

  /// Returns the metrics collector of this data flow.
  virtual data_flow_message_decoding_metrics_collector& get_metrics_collector() = 0;
};

} // namespace ofh
} // namespace ocudu
