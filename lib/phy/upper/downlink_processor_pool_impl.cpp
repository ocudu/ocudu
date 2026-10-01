// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "downlink_processor_pool_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/slot_point.h"

using namespace ocudu;

downlink_processor_pool_impl::downlink_processor_pool_impl(downlink_processor_pool_impl_config dl_processors)
{
  for (auto& proc : dl_processors.procs) {
    ocudu_assert(!proc.procs.empty(), "Cannot store an empty processor pool");
    processors.insert(proc.scs, std::move(proc.procs));
  }
}

downlink_processor_controller& downlink_processor_pool_impl::get_processor_controller(slot_point slot)
{
  ocudu_assert(slot.valid(), "Invalid slot ({}) when requesting a downlink processor", slot);
  return processors.get_processor(slot).get_controller();
}

void downlink_processor_pool_impl::stop()
{
  processors.stop();
}
