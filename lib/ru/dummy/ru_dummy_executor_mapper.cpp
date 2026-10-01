// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ru/dummy/ru_dummy_executor_mapper.h"

using namespace ocudu;

namespace {

/// Implements an RU dummy executor mapper.
class ru_dummy_executor_mapper_impl : public ru_dummy_executor_mapper
{
public:
  /// Contructs the executor mapper with a single executor reference.
  ru_dummy_executor_mapper_impl(task_executor& executor_) : executor(executor_) {}

  // See interface for documentation.
  task_executor& common_executor() override { return executor; }

private:
  // Actual executor.
  task_executor& executor;
};

} // namespace

std::unique_ptr<ru_dummy_executor_mapper> ocudu::create_ru_dummy_executor_mapper(ocudu::task_executor& executor)
{
  return std::make_unique<ru_dummy_executor_mapper_impl>(executor);
}
