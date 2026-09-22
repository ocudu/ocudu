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

  /// Decodes the given Open Fronthaul message associated to the given eAxC.
  virtual void decode_type1_message(unsigned eaxc, span<const uint8_t> message, bool is_seq_id_correct) = 0;

  /// Returns the metrics collector of this data flow.
  virtual data_flow_message_decoding_metrics_collector& get_metrics_collector() = 0;
};

} // namespace ofh
} // namespace ocudu
