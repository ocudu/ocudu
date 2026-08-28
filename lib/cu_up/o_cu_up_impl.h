// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_up/cu_up.h"
#include "ocudu/cu_up/cu_up_operation_controller.h"
#include "ocudu/cu_up/o_cu_up.h"
#include "ocudu/e2/e2.h"
#include <memory>

namespace ocudu::ocuup {

/// O-RAN CU-UP implementation.
class o_cu_up_impl : public o_cu_up, public cu_up_operation_controller
{
public:
  explicit o_cu_up_impl(std::unique_ptr<cu_up_interface> cu_up_) : cu_up(std::move(cu_up_))
  {
    ocudu_assert(cu_up, "Invalid CU-UP");
  }

  // See interface for documentation.
  cu_up_interface& get_cu_up() override { return *cu_up; }

  // See interface for documentation.
  cu_up_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  std::unique_ptr<cu_up_interface> cu_up;
};

/// O-RAN CU-UP implementation with E2.
class o_cu_up_with_e2_impl : public o_cu_up, public cu_up_operation_controller
{
public:
  o_cu_up_with_e2_impl(std::unique_ptr<e2_agent> e2agent_, std::unique_ptr<cu_up_interface> cu_up_) :
    e2agent(std::move(e2agent_)), cu_up(std::move(cu_up_))
  {
    ocudu_assert(e2agent, "Invalid E2 agent");
    ocudu_assert(cu_up, "Invalid CU-UP");
  }

  // See interface for documentation.
  cu_up_interface& get_cu_up() override { return *cu_up; }

  // See interface for documentation.
  cu_up_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  std::unique_ptr<e2_agent>        e2agent;
  std::unique_ptr<cu_up_interface> cu_up;
};

} // namespace ocudu::ocuup
