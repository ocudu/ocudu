# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        ru_dummy_executor_mapper.cpp
        ru_dummy_factory.cpp
        ru_dummy_impl.cpp
        ru_dummy_metrics_collector.cpp)

add_library(ocudu_ru_dummy STATIC ${SOURCES})
target_link_libraries(ocudu_ru_dummy ocudulog)
