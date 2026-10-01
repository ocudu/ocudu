// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "fapi_to_mac_error_indication_fastpath_translator.h"
#include "fapi_to_mac_indications_fastpath_translator.h"
#include "fapi_to_mac_slot_indication_fastpath_translator.h"
#include "mac_fapi_p7_sector_fastpath_adaptor_impl_config.h"
#include "mac_to_fapi_fastpath_translator.h"
#include "ocudu/fapi_adaptor/mac/operation_controller.h"
#include "ocudu/fapi_adaptor/mac/p7/mac_fapi_p7_sector_fastpath_adaptor.h"

namespace ocudu {
namespace fapi_adaptor {

/// \brief MAC-FAPI P7 bidirectional sector fastpath adaptor implementation.
class mac_fapi_p7_sector_fastpath_adaptor_impl : public mac_fapi_p7_sector_fastpath_adaptor, public operation_controller
{
public:
  /// Constructor for the MAC-FAPI bidirectional sector adaptor.
  mac_fapi_p7_sector_fastpath_adaptor_impl(const mac_fapi_p7_sector_fastpath_adaptor_config&     config,
                                           mac_fapi_p7_sector_fastpath_adaptor_impl_dependencies dependencies);

  // See interface for documentation.
  operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override { mac_translator.start(); }

  // See interface for documentation.
  void stop() override { mac_translator.stop(); }

  // See interface for documentation.
  fapi::p7_slot_indication_notifier& get_p7_slot_indication_notifier() override;

  // See interface for documentation.
  fapi::error_indication_notifier& get_error_indication_notifier() override;

  // See interface for documentation.
  fapi::p7_indications_notifier& get_p7_indications_notifier() override;

  // See interface for documentation.
  mac_cell_result_notifier& get_cell_result_notifier() override;

  // See interface for documentation.
  void set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler) override;

  // See interface for documentation.
  void set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler) override;

  // See interface for documentation.
  void set_cell_pdu_handler(mac_pdu_handler& handler) override;

  // See interface for documentation.
  void set_cell_crc_handler(mac_cell_control_information_handler& handler) override;

private:
  /// FAPI-to-MAC indications message translator.
  fapi_to_mac_indications_fastpath_translator fapi_data_translator;
  /// FAPI-to-MAC slot indications message translator.
  fapi_to_mac_slot_indication_fastpath_translator fapi_time_translator;
  /// FAPI-to-MAC error-specific message translator.
  fapi_to_mac_error_indication_fastpath_translator fapi_error_translator;
  /// MAC-to-FAPI translator.
  mac_to_fapi_fastpath_translator mac_translator;
};

} // namespace fapi_adaptor
} // namespace ocudu
