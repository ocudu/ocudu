// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ru/dummy/ru_dummy_factory.h"
#include "ru_dummy_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/ru/ru.h"
#include <memory>

using namespace ocudu;

std::unique_ptr<radio_unit> ocudu::create_dummy_ru(const ru_dummy_configuration& config,
                                                   ru_dummy_dependencies&        dependencies)
{
  return std::make_unique<ru_dummy_impl>(config, dependencies);
}
