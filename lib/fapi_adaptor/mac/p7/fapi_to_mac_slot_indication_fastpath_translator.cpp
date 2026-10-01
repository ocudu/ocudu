// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "fapi_to_mac_slot_indication_fastpath_translator.h"
#include "ocudu/fapi/p7/messages/slot_indication.h"
#include "ocudu/mac/mac_cell_slot_handler.h"
#include "ocudu/mac/mac_cell_timing_context.h"

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

fapi_to_mac_slot_indication_fastpath_translator::fapi_to_mac_slot_indication_fastpath_translator(
    unsigned                sector_id_,
    mac_cell_slot_handler&  fapi_slot_handler_,
    ocudulog::basic_logger& logger_) :
  sector_id(sector_id_), fapi_slot_handler(fapi_slot_handler_), mac_slot_handler(&mac_dummy_handler), logger(logger_)
{
}

void fapi_to_mac_slot_indication_fastpath_translator::on_slot_indication(const fapi::slot_indication& msg)
{
  if (OCUDU_UNLIKELY(logger.debug.enabled())) {
    logger.debug("Sector#{}: {}", sector_id, msg);
  }

  mac_cell_timing_context context{.sl_tx = msg.slot, .time_point = msg.time_point};
  fapi_slot_handler.handle_slot_indication(context);
  mac_slot_handler->handle_slot_indication(context);
}
