// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdxch_processor_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/instrumentation/traces/critical_traces.h"
#include "ocudu/phy/support/resource_grid_reader.h"

using namespace ocudu;

void pdxch_processor_impl::connect(pdxch_processor_notifier& notifier_)
{
  notifier = &notifier_;
}

pdxch_processor_request_handler& pdxch_processor_impl::get_request_handler()
{
  return *this;
}

pdxch_processor_baseband& pdxch_processor_impl::get_baseband()
{
  return *this;
}

lower_phy_center_freq_controller& pdxch_processor_impl::get_center_freq_control()
{
  return *this;
}

void pdxch_processor_impl::handle_request(const shared_resource_grid& grid, const resource_grid_context& context)
{
  // Ignore request if the processor has stopped.
  if (stopped.load(std::memory_order_relaxed)) {
    return;
  }

  ocudu_assert(notifier != nullptr, "Notifier has not been connected.");

  // Ignore grid if it is empty.
  if (grid.get_reader().is_empty()) {
    return;
  }

  // Obtain baseband buffer.
  baseband_gateway_buffer_ptr buffer = bb_buffers.get();
  if (!buffer) {
    logger.error(context.slot.sfn(), context.slot.slot_index(), "Insufficient number of buffers.");
    return;
  }

  // Try modulating the request. It can be the modulator is busy with a previous transmission.
  bool success = get_modulator(context.slot).handle_request(std::move(buffer), grid, context);

  // If there was a request with a resource grid, then notify a late event with the context of the discarded request.
  if (!success) {
    logger.error(context.slot.sfn(), context.slot.slot_index(), "The modulator is busy.");
    general_critical_tracer << instant_trace_event{
        "modulator_busy", instant_trace_event::cpu_scope::thread, instant_trace_event::event_criticality::severe};
  }
}

void pdxch_processor_impl::on_modulation_completion(baseband_gateway_buffer_ptr  buffer,
                                                    const resource_grid_context& context)
{
  // Write modulation result in the request buffer.
  auto request = requests.exchange({context.slot, std::move(buffer)});

  // Check if the previous entry in the request buffer is not empty.
  if (request.resource) {
    notifier->on_pdxch_request_late(resource_grid_context{.slot = request.slot, .sector = context.sector});
    general_critical_tracer << instant_trace_event{"on_pdxch_request_late",
                                                   instant_trace_event::cpu_scope::thread,
                                                   instant_trace_event::event_criticality::severe};
  }
}

bool pdxch_processor_impl::set_carrier_center_frequency(double carrier_center_frequency_Hz)
{
  common_ofdm_modulator->set_center_frequency(carrier_center_frequency_Hz);
  return true;
}

baseband_gateway_buffer_ptr pdxch_processor_impl::process_slot(slot_context context)
{
  // Exchange an empty request with the current slot with a stored request.
  auto request = requests.exchange({.slot = context.slot, .resource = {}});

  // If the request buffer is invalid, the request is empty.
  if (!request.resource) {
    return {};
  }

  // If the slot of the request does not match the current slot, then notify a late event.
  if (context.slot != request.slot) {
    notifier->on_pdxch_request_late(resource_grid_context{.slot = request.slot, .sector = context.sector});
    return {};
  }

  // Return the result.
  return std::move(request.resource);
}
