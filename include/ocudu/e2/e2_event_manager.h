// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/e2ap/e2ap.h"
#include "ocudu/support/async/protocol_transaction_manager.h"
#include <map>

namespace ocudu {

class e2_event_manager
{
public:
  protocol_transaction_event_source<asn1::e2ap::e2setup_resp_s, asn1::e2ap::e2setup_fail_s>       e2_setup_outcome;
  protocol_transaction_event_source<asn1::e2ap::e2_removal_resp_s, asn1::e2ap::e2_removal_fail_s> e2_removal_outcome;

  std::map<std::tuple<uint32_t, uint32_t>,
           std::unique_ptr<protocol_transaction_event_source<asn1::e2ap::ric_sub_delete_request_s>>>
      sub_del_reqs;

  void add_sub_del_req(const asn1::e2ap::ric_request_id_s& ric_request_id, timer_factory timer)
  {
    sub_del_reqs[{ric_request_id.ric_requestor_id, ric_request_id.ric_instance_id}] =
        std::make_unique<protocol_transaction_event_source<asn1::e2ap::ric_sub_delete_request_s>>(timer);
  }
  explicit e2_event_manager(timer_factory timers) : e2_setup_outcome(timers), e2_removal_outcome(timers) {}
};

} // namespace ocudu
