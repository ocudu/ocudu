# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_library(ocudu_f1_app_helpers STATIC f1_gateway_helpers.cpp)
target_include_directories(ocudu_f1_app_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ocudu_f1_app_helpers
                      PRIVATE ocudu_f1c_gateway
                              ocudu_gtpu
                              ocudu_f1u_cu_up_split_connector
                              ocudu_f1u_du_split_connector)
