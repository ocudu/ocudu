// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_test_helpers.h"

using namespace ocudu;
using namespace ocudu::test_helpers;

void mac_test_ue::add_bearer(lcid_t lcid)
{
  bearers.emplace(lcid);
  bearers[lcid].bearer.lcid      = lcid;
  bearers[lcid].bearer.ul_bearer = &bearers[lcid].ul_notifier;
  bearers[lcid].bearer.dl_bearer = &bearers[lcid].dl_notifier;
}

mac_ue_create_request mac_test_ue::make_ue_create_request()
{
  mac_ue_create_request msg = make_default_ue_creation_request();
  msg.ue_index              = ue_index;
  msg.crnti                 = rnti;
  for (const auto& b : bearers) {
    msg.bearers.push_back(b.bearer);
  }
  msg.ul_ccch_msg = nullptr;
  return msg;
}
