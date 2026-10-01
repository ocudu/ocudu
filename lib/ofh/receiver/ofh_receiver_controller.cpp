// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ofh_receiver_controller.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace ofh;

void receiver_controller::start()
{
  msg_receiver.get_operation_controller().start();
  window_handler.start();
}

void receiver_controller::stop()
{
  window_handler.stop();
  msg_receiver.get_operation_controller().stop();
}
