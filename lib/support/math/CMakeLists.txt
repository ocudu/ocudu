# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(SOURCES
        math_utils.cpp
        curve_fitting_find_max.cpp
)

add_library(ocudu_support_math STATIC ${SOURCES})
target_link_libraries(ocudu_support_math PRIVATE ocuduvec)

add_to_exported_libs(ocudu_support_math)
