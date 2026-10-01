// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "phy_to_fapi_time_event_fastpath_translator.h"
#include "fapi_to_phy_fastpath_translator.h"
#include "ocudu/adt/format.h"
#include "ocudu/fapi/p7/builders/slot_indication_builder.h"
#include "ocudu/phy/upper/upper_phy_timing_context.h"

using namespace ocudu;
using namespace fapi_adaptor;

namespace {

class p7_slot_indication_notifier_dummy : public fapi::p7_slot_indication_notifier
{
public:
  void on_slot_indication(const fapi::slot_indication& msg) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the PHY-to-FAPI time event translator as a placeholder for the
/// actual time-specific notifier, which will be later set up through the \ref set_p7_slot_indication_notifier() method.
static p7_slot_indication_notifier_dummy dummy_p7_slot_notifier;

phy_to_fapi_time_event_fastpath_translator::phy_to_fapi_time_event_fastpath_translator(
    fapi_to_phy_fastpath_translator& translator_) :
  translator(translator_), slot_indication_notifier(&dummy_p7_slot_notifier)
{
}

void phy_to_fapi_time_event_fastpath_translator::on_tti_boundary(const upper_phy_timing_context& context)
{
  translator.handle_new_slot(context.slot.without_hyper_sfn());

  // Delivering the slot.indication message must always be the last step.
  slot_indication_notifier->on_slot_indication(fapi::build_slot_indication(context.slot, context.time_point));
}
