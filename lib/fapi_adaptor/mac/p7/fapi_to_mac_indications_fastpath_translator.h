// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p7/p7_indications_notifier.h"
#include "ocudu/mac/mac_cell_control_information_handler.h"
#include "ocudu/mac/mac_cell_rach_handler.h"
#include "ocudu/mac/mac_pdu_handler.h"
#include "ocudu/ocudulog/logger.h"

namespace ocudu {
namespace fapi_adaptor {

/// \brief FAPI-to-MAC data message fastpath translator.
///
/// This class listens to FAPI P7 indications and translates them into the suitable data types
/// for the MAC layer.
class fapi_to_mac_indications_fastpath_translator : public fapi::p7_indications_notifier
{
public:
  fapi_to_mac_indications_fastpath_translator(unsigned sector_id_, ocudulog::basic_logger& logger_);

  // See interface for documentation.
  void on_rx_data_indication(const fapi::rx_data_indication& msg) override;

  // See interface for documentation.
  void on_crc_indication(const fapi::crc_indication& msg) override;

  // See interface for documentation.
  void on_uci_indication(const fapi::uci_indication& msg) override;

  // See interface for documentation.
  void on_srs_indication(const fapi::srs_indication& msg) override;

  // See interface for documentation.
  void on_rach_indication(const fapi::rach_indication& msg) override;

  /// Sets the given cell-specific MAC RACH handler. This handler will be used to receive new RACH notifications.
  void set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler);

  /// Sets the given cell-specific MAC PDU handler. This handler will be used to receive new PDU notifications.
  void set_cell_pdu_handler(mac_pdu_handler& handler);

  /// Sets the given cell-specific MAC CRC handler. This handler will be used to receive new CRC notifications.
  void set_cell_crc_handler(mac_cell_control_information_handler& handler);

private:
  const unsigned                        sector_id;
  ocudulog::basic_logger&               logger;
  mac_cell_rach_handler*                rach_handler;
  mac_pdu_handler*                      pdu_handler;
  mac_cell_control_information_handler* cell_control_handler;
};

} // namespace fapi_adaptor
} // namespace ocudu
