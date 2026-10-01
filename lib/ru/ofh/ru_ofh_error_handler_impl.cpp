// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ru_ofh_error_handler_impl.h"

using namespace ocudu;

void ru_ofh_error_handler_impl::on_late_downlink_message(const ofh::error_context& context)
{
  ru_error_context ru_err_context;
  ru_err_context.slot   = context.slot;
  ru_err_context.sector = context.sector;

  error_notifier.on_late_downlink_message(ru_err_context);
}

void ru_ofh_error_handler_impl::on_late_uplink_message(const ofh::error_context& context)
{
  ru_error_context ru_err_context;
  ru_err_context.slot   = context.slot;
  ru_err_context.sector = context.sector;

  error_notifier.on_late_uplink_message(ru_err_context);
}

void ru_ofh_error_handler_impl::on_late_prach_message(const ofh::error_context& context)
{
  ru_error_context ru_err_context;
  ru_err_context.slot   = context.slot;
  ru_err_context.sector = context.sector;

  error_notifier.on_late_prach_message(ru_err_context);
}
