// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "fapi_to_mac_error_indication_fastpath_translator.h"
#include "ocudu/fapi/common/error_indication.h"
#include "ocudu/mac/mac_cell_slot_handler.h"

using namespace ocudu;
using namespace fapi_adaptor;

namespace {

class mac_cell_slot_handler_dummy : public mac_cell_slot_handler
{
public:
  void handle_slot_indication(const mac_cell_timing_context& context) override {}
  void handle_error_indication(slot_point sl_tx, error_event event) override {}
  void handle_stop_indication() override {}
};

} // namespace

/// This dummy object is passed to the constructor of the FAPI-to-MAC translator as a placeholder for the actual,
/// cell-specific MAC slot handler, which will be later set up through the \ref set_cell_slot_handler() method.
static mac_cell_slot_handler_dummy mac_dummy_handler;

fapi_to_mac_error_indication_fastpath_translator::fapi_to_mac_error_indication_fastpath_translator(
    unsigned                sector_id_,
    ocudulog::basic_logger& logger_) :
  sector_id(sector_id_), mac_slot_handler(&mac_dummy_handler), logger(logger_)
{
}

/// Converts a FAPI error indication error code to the corresponding MAC specific error event.
static mac_cell_slot_handler::error_event to_error_event(fapi::error_code_id code, fapi::message_type_id id)
{
  mac_cell_slot_handler::error_event mac_event{};

  switch (code) {
    case fapi::error_code_id::out_of_sync:
    case fapi::error_code_id::msg_slot_err:
      // DL_TTI.request OR UL_TTI.request.
      if (id == fapi::message_type_id::dl_tti_request) {
        mac_event.pdsch_discarded = true;
        mac_event.pdcch_discarded = true;
      } else {
        mac_event.pusch_and_pucch_discarded = true;
      }
      break;
    case fapi::error_code_id::msg_invalid_sfn:
      // UL_DCI.request OR Tx_Data.request.
      if (id == fapi::message_type_id::ul_dci_request) {
        mac_event.pdcch_discarded = true;
      } else {
        mac_event.pdsch_discarded = true;
      }
      break;
    case fapi::error_code_id::msg_ul_dci_err:
      // UL_DCI.request only.
      mac_event.pdcch_discarded = true;
      break;
    case fapi::error_code_id::msg_tx_err:
      // Tx_Data.request only.
      mac_event.pdsch_discarded = true;
      break;
    default:
      break;
  }

  return mac_event;
}

void fapi_to_mac_error_indication_fastpath_translator::on_error_indication(const fapi::error_indication& msg)
{
  if (!msg.slot) {
    logger.warning("Invalid slot point when receiving an error indication.");
    return;
  }

  if (OCUDU_UNLIKELY(logger.debug.enabled())) {
    logger.debug("Sector#{}: {}", sector_id, msg);
  }

  mac_slot_handler->handle_error_indication(msg.slot.value(), to_error_event(msg.error_code, msg.message_id));
}
