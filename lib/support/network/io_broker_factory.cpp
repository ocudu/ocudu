// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/io/io_broker_factory.h"
#include "io_broker_epoll.h"

using namespace ocudu;

std::unique_ptr<io_broker> ocudu::create_io_broker(io_broker_type type, const io_broker_config& config)
{
  switch (type) {
    case io_broker_type::epoll:
      return std::make_unique<io_broker_epoll>(config);
    default:
      ocudu_terminate("IO broker type not supported");
  }
  return nullptr;
}
