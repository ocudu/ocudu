// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi_adaptor/mac/p7/mac_fapi_p7_sector_adaptor.h"

namespace ocudu {

namespace fapi {
class error_indication_notifier;
class p7_indications_notifier;
class p7_slot_indication_notifier;
} // namespace fapi

class mac_cell_control_information_handler;
class mac_cell_rach_handler;
class mac_cell_result_notifier;
class mac_cell_slot_handler;
class mac_pdu_handler;

namespace fapi_adaptor {

class operation_controller;

/// \brief MAC-FAPI bidirectional sector fastpath adaptor interface.
///
/// This adaptor is a collection of interfaces to translate FAPI messages into their MAC layer counterpart and vice
/// versa.
class mac_fapi_p7_sector_fastpath_adaptor : public mac_fapi_p7_sector_adaptor
{
public:
  // Note: temporal method. When doing the task of stopping O-DU low using the FAPI adaptor, check if this method
  // is still required or can be moved to the implementation.
  virtual operation_controller& get_operation_controller() = 0;

  /// Returns a reference to the MAC cell results notifier used by the adaptor.
  virtual mac_cell_result_notifier& get_cell_result_notifier() = 0;

  /// Configures the MAC cell slot handler to the given one.
  virtual void set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler) = 0;

  /// Configures the MAC cell RACH handler to the given one.
  virtual void set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler) = 0;

  /// Configures the MAC cell PDU handler to the given one.
  virtual void set_cell_pdu_handler(mac_pdu_handler& handler) = 0;

  /// Configures the MAC cell CRC handler to the given one.
  virtual void set_cell_crc_handler(mac_cell_control_information_handler& handler) = 0;
};

} // namespace fapi_adaptor
} // namespace ocudu
