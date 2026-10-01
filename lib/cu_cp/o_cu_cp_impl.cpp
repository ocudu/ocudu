// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "o_cu_cp_impl.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace ocucp;

o_cu_cp_impl::o_cu_cp_impl(std::unique_ptr<cu_cp> cu_cp_) : cu_impl(std::move(cu_cp_))
{
  ocudu_assert(cu_impl, "Invalid CU-CP");
}

void o_cu_cp_impl::start()
{
  cu_impl->start();
}

void o_cu_cp_impl::stop()
{
  cu_impl->stop();
}

o_cu_cp_with_e2_impl::o_cu_cp_with_e2_impl(std::unique_ptr<e2_agent>                         e2agent_,
                                           std::unique_ptr<cu_cp>                            cu_cp_,
                                           std::unique_ptr<cu_cp_ng_setup_complete_notifier> ng_setup_adapter_) :
  o_cu_cp_impl(std::move(cu_cp_)), ng_setup_e2_adapter(std::move(ng_setup_adapter_)), e2agent(std::move(e2agent_))
{
  ocudu_assert(e2agent, "Invalid E2 agent");
  ocudu_assert(ng_setup_e2_adapter, "Invalid NG setup E2 adapter");
}

void o_cu_cp_with_e2_impl::start()
{
  o_cu_cp_impl::start();
  e2agent->start();
}

void o_cu_cp_with_e2_impl::stop()
{
  e2agent->stop();
  o_cu_cp_impl::stop();
}
