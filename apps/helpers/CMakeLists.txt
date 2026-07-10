# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.



add_subdirectory(e2)
add_subdirectory(f1u)
add_subdirectory(hal)
add_subdirectory(logger)
add_subdirectory(metrics)
add_subdirectory(network)
add_subdirectory(tracing)

add_library(ocudu_app_helpers INTERFACE)
target_link_libraries(ocudu_app_helpers INTERFACE  ocudu_e2_app_helpers
                                                    ocudu_f1u_app_helpers
                                                    ocudu_hal_app_helpers
                                                    ocudu_logger_app_helpers
                                                    ocudu_metrics_app_helpers
                                                    ocudu_network_app_helpers
                                                    ocudu_tracing_app_helpers)

# Reusable NTN config helpers shared across app units. Added after ocudu_app_helpers since it links against it.
add_subdirectory(ntn)
