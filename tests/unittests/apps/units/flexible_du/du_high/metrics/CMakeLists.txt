# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(du_high_scheduler_cell_metrics_consumer_stdout_test du_high_scheduler_cell_metrics_consumer_stdout_test.cpp)
target_link_libraries(du_high_scheduler_cell_metrics_consumer_stdout_test ocudu_support ocudu_du_high_unit_metrics_helpers ocudu_app_services)
add_test(du_high_scheduler_cell_metrics_consumer_stdout_test du_high_scheduler_cell_metrics_consumer_stdout_test)
target_include_directories(du_high_scheduler_cell_metrics_consumer_stdout_test PRIVATE ${CMAKE_SOURCE_DIR})
