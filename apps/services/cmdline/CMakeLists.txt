# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


# Do not build by default.
set_directory_properties(PROPERTIES EXCLUDE_FROM_ALL TRUE)

set(SOURCES
        cmdline_command_dispatcher.cpp
        stdout_metrics_command.cpp)

add_library(ocudu_cmdline_app_service STATIC ${SOURCES})
target_include_directories(ocudu_cmdline_app_service PRIVATE ${CMAKE_SOURCE_DIR})
