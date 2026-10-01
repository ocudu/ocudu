// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include <thread>

namespace ocudu {

/// Spy implementation of a PDSCH processor.
class pdsch_processor_spy : public pdsch_processor
{
  bool process_method_called = false;

public:
  void process(resource_grid_writer&                                           grid,
               pdsch_processor_notifier&                                       notifier,
               static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdu_t&                                                    pdu) override
  {
    process_method_called = true;
    notifier.on_finish_processing();
  }

  /// Returns true if the process method has been called, false otherwise.
  bool is_process_called() const { return process_method_called; }
};

class pdsch_processor_notifier_spy : public pdsch_processor_notifier
{
public:
  void on_finish_processing() override { finished = true; }

  void wait_for_finished()
  {
    while (!finished) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }

  void reset() { finished = false; }

private:
  std::atomic<bool> finished = {};
};

} // namespace ocudu
