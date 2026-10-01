// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "phy_to_fapi_error_event_fastpath_translator.h"
#include "ocudu/fapi/common/error_indication_builder.h"

using namespace ocudu;
using namespace fapi_adaptor;

namespace {
class error_notifier_dummy : public fapi::error_indication_notifier
{
public:
  void on_error_indication(const fapi::error_indication& msg) override {}
};

} // namespace

static error_notifier_dummy dummy_notifier;

phy_to_fapi_error_event_fastpath_translator::phy_to_fapi_error_event_fastpath_translator() :
  error_notifier(&dummy_notifier)
{
}

void phy_to_fapi_error_event_fastpath_translator::on_late_downlink_message(ocudu::slot_point dl_frame_slot)
{
  error_notifier->on_error_indication(
      fapi::build_msg_error_indication(dl_frame_slot, fapi::message_type_id::dl_tti_request));
}

void phy_to_fapi_error_event_fastpath_translator::on_late_uplink_message(ocudu::slot_point ul_frame_slot)
{
  error_notifier->on_error_indication(
      fapi::build_msg_error_indication(ul_frame_slot, fapi::message_type_id::ul_tti_request));
}

void phy_to_fapi_error_event_fastpath_translator::on_late_prach_message(slot_point prach_msg_slot)
{
  // Do nothing.
}
