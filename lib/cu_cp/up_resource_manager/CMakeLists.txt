# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
  up_resource_manager_impl.cpp
  up_resource_manager_helpers.cpp
)

add_library(ocudu_up_resource_manager STATIC ${SOURCES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
