# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


# Do not build by default.
set_directory_properties(PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(app_execution_metrics)
add_subdirectory(app_resource_usage)
add_subdirectory(buffer_pool)
add_subdirectory(cmdline)
add_subdirectory(metrics)
add_subdirectory(remote_control)
add_subdirectory(worker_manager)

add_library(ocudu_app_services INTERFACE)
target_link_libraries(ocudu_app_services INTERFACE
        ocudu_exec_metrics_app_service
        ocudu_buffer_pool_app_service
        ocudu_cmdline_app_service
        ocudu_metrics_app_service
        ocudu_remote_control_app_service
        ocudu_resource_usage_app_service
        ocudu_worker_manager_app_service)
