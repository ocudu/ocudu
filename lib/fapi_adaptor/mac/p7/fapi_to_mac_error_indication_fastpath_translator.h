// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/common/error_indication_notifier.h"
#include "ocudu/ocudulog/ocudulog.h"

namespace ocudu {

class mac_cell_slot_handler;

namespace fapi_adaptor {

/// FAPI-to-MAC error indication fastpath translator.
class fapi_to_mac_error_indication_fastpath_translator : public fapi::error_indication_notifier
{
public:
  fapi_to_mac_error_indication_fastpath_translator(unsigned sector_id_, ocudulog::basic_logger& logger_);

  // See interface for documentation.
  void on_error_indication(const fapi::error_indication& msg) override;

  /// Sets the given cell-specific slot handler. This handler will be used to receive error notifications.
  void set_cell_slot_handler(mac_cell_slot_handler& handler) { mac_slot_handler = &handler; }

private:
  const unsigned          sector_id;
  mac_cell_slot_handler*  mac_slot_handler;
  ocudulog::basic_logger& logger;
};

} // namespace fapi_adaptor
} // namespace ocudu
