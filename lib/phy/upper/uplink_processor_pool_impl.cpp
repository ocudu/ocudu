// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "uplink_processor_pool_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/slot_point.h"

using namespace ocudu;

uplink_processor_pool_impl::uplink_processor_pool_impl(uplink_processor_pool_impl_config ul_processors) :
  default_processor(std::move(ul_processors.default_processor))
{
  ocudu_assert(default_processor, "Invalid default processor");
  for (auto& proc : ul_processors.procs) {
    ocudu_assert(!proc.procs.empty(), "Cannot store an empty processor pool");
    processors.insert(proc.scs, std::move(proc.procs));
  }
}

void uplink_processor_pool_impl::stop()
{
  processors.stop();
}

uplink_slot_processor& uplink_processor_pool_impl::get_slot_processor(slot_point slot)
{
  ocudu_assert(slot.valid(), "Invalid slot ({}) when requesting an uplink processor", slot);
  uplink_processor* processor = assigned_processors[slot.system_slot()].load(std::memory_order_acquire);
  if (processor) {
    return processor->get_slot_processor(slot);
  }
  return default_processor->get_slot_processor(slot);
}

unique_uplink_pdu_slot_repository uplink_processor_pool_impl::get_pdu_slot_repository(slot_point slot)
{
  ocudu_assert(slot.valid(), "Invalid slot ({}) when requesting an uplink processor", slot);

  // Get processor.
  uplink_processor& processor = processors.get_processor(slot);

  // Assign the processor to the circular array, where it will later be retrieved by the receiver symbol handler.
  assigned_processors[slot.system_slot()].store(&processor, std::memory_order_release);

  return processor.get_pdu_slot_repository(slot);
}
