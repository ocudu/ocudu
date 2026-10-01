// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ru_ofh_timing_notifier_impl.h"
#include "ocudu/ru/ru_timing_notifier.h"
#include <chrono>

using namespace ocudu;

ru_ofh_timing_notifier_impl::ru_ofh_timing_notifier_impl(unsigned            nof_slot_offset_du_ru_,
                                                         unsigned            nof_symbols_per_slot,
                                                         subcarrier_spacing  scs,
                                                         ru_timing_notifier& timing_notifier_) :
  nof_slot_offset_du_ru(nof_slot_offset_du_ru_),
  nof_slots_offset_du_ru_ns(nof_slot_offset_du_ru * 1000000 / slot_point(scs, 0).nof_slots_per_subframe()),
  half_slot_symbol(nof_symbols_per_slot / 2U - 1U),
  full_slot_symbol(nof_symbols_per_slot - 1U),
  timing_notifier(timing_notifier_)
{
}

void ru_ofh_timing_notifier_impl::on_new_symbol(const ofh::slot_symbol_point_context& symbol_point_context)
{
  if (symbol_point_context.symbol_point.get_symbol_index() == half_slot_symbol) {
    timing_notifier.on_ul_half_slot_boundary(symbol_point_context.symbol_point.get_slot());
  }

  if (symbol_point_context.symbol_point.get_symbol_index() == full_slot_symbol) {
    timing_notifier.on_ul_full_slot_boundary(symbol_point_context.symbol_point.get_slot());
  }

  // New slots start on symbol index 0.
  if (symbol_point_context.symbol_point.get_symbol_index() == 0) {
    timing_notifier.on_tti_boundary(tti_boundary_context{
        .slot = slot_point_extended(symbol_point_context.symbol_point.get_slot(), symbol_point_context.hfn) +
                nof_slot_offset_du_ru,
        .time_point = symbol_point_context.time_point + nof_slots_offset_du_ru_ns});
  }
}
