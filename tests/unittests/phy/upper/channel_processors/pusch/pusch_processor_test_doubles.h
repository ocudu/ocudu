// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"

namespace ocudu {

class pusch_processor_spy : public pusch_processor
{
public:
  void process(span<uint8_t>                    data,
               unique_rx_buffer                 buffer,
               pusch_processor_result_notifier& notifier,
               const resource_grid_reader&      grid,
               const pdu_t&                     pdu) override
  {
    processed_method_been_called = true;

    // Notify completion of PUSCH UCI.
    if ((pdu.uci.nof_harq_ack != 0) || (pdu.uci.nof_csi_part1 != 0)) {
      pusch_processor_result_control uci_result;

      // Report HARQ-ACK if present.
      if (pdu.uci.nof_harq_ack != 0) {
        uci_result.harq_ack.payload.resize(pdu.uci.nof_harq_ack);
        uci_result.harq_ack.status = uci_status::valid;
      }

      // Report CSI-Part1 if present.
      if (pdu.uci.nof_csi_part1 != 0) {
        uci_result.csi_part1.payload.resize(pdu.uci.nof_csi_part1);
        uci_result.csi_part1.status = uci_status::valid;
      }

      // CSI Part 2: derive size from the fixed-size entry (if present).
      if (pdu.uci.csi_part2_size.entries.size() == 1 && pdu.uci.csi_part2_size.entries[0].map.size() == 1) {
        unsigned part2_bits = pdu.uci.csi_part2_size.entries[0].map[0];
        if (part2_bits != 0) {
          uci_result.csi_part2.payload.resize(part2_bits);
          uci_result.csi_part2.status = uci_status::valid;
        }
      }

      notifier.on_uci(uci_result);
    }

    // Notify completion of PUSCH data.
    notifier.on_sch({});
  }

  bool has_process_method_been_called() const { return processed_method_been_called; }

private:
  bool processed_method_been_called = false;
};

} // namespace ocudu
