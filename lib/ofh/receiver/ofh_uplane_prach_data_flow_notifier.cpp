// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ofh_uplane_prach_data_flow_notifier.h"
#include "ocudu/ofh/ofh_uplane_rx_symbol_notifier.h"

using namespace ocudu;
using namespace ofh;

void uplane_prach_data_flow_notifier::notify_prach(slot_point slot)
{
  expected<prach_context::prach_context_information> context =
      prach_context_repo->try_popping_complete_prach_buffer(slot);

  if (!context) {
    return;
  }

  const auto& ctx_value = context->context;

  notifier->on_new_prach_window_data(ctx_value, std::move(context->buffer));

  if (OCUDU_UNLIKELY(logger.debug.enabled())) {
    logger.debug("Notifying PRACH in slot '{}' for sector#{}", ctx_value.slot, ctx_value.sector);
  }
}
