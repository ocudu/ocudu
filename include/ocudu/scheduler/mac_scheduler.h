// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/scheduler_configurator.h"
#include "ocudu/scheduler/scheduler_dl_buffer_state_indication_handler.h"
#include "ocudu/scheduler/scheduler_feedback_handler.h"
#include "ocudu/scheduler/scheduler_paging_handler.h"
#include "ocudu/scheduler/scheduler_positioning_handler.h"
#include "ocudu/scheduler/scheduler_rach_handler.h"
#include "ocudu/scheduler/scheduler_slot_handler.h"
#include "ocudu/scheduler/scheduler_sys_info_handler.h"

namespace ocudu {

class mac_scheduler : public scheduler_configurator,
                      public scheduler_ue_configurator,
                      public scheduler_rach_handler,
                      public scheduler_feedback_handler,
                      public scheduler_slot_handler,
                      public scheduler_dl_buffer_state_indication_handler,
                      public scheduler_paging_handler,
                      public scheduler_sys_info_handler,
                      public scheduler_positioning_handler
{
public:
  virtual ~mac_scheduler() = default;
};

} // namespace ocudu
