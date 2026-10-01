// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "rx_resource_grid_printer_backend.h"
#include "ocudu/phy/upper/uplink_processor.h"
#include <memory>

namespace ocudu {

/// Uplink processor decorator that stops the receive resource grid printer backend before the processor.
class uplink_processor_printer_decorator : public uplink_processor
{
public:
  uplink_processor_printer_decorator(std::unique_ptr<uplink_processor>                 processor_,
                                     std::shared_ptr<rx_resource_grid_printer_backend> backend_) :
    processor(std::move(processor_)), backend(std::move(backend_))
  {
  }

  // See the uplink_processor interface for documentation.
  unique_uplink_pdu_slot_repository get_pdu_slot_repository(slot_point slot) override
  {
    return processor->get_pdu_slot_repository(slot);
  }

  // See the uplink_processor interface for documentation.
  uplink_slot_processor& get_slot_processor(slot_point slot) override { return processor->get_slot_processor(slot); }

  // See the uplink_processor interface for documentation.
  void stop() override
  {
    // Release the resources buffered in the backend.
    backend->stop();

    // Stop the processor.
    processor->stop();
  }

private:
  std::unique_ptr<uplink_processor>                 processor;
  std::shared_ptr<rx_resource_grid_printer_backend> backend;
};

} // namespace ocudu
