// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "o_cu_up_impl.h"

using namespace ocudu;
using namespace ocuup;

void o_cu_up_impl::start()
{
  cu_up->start();
}

void o_cu_up_impl::stop()
{
  cu_up->stop();
}

void o_cu_up_with_e2_impl::start()
{
  cu_up->start();
  e2agent->start();
}

void o_cu_up_with_e2_impl::stop()
{
  e2agent->stop();
  cu_up->stop();
}
