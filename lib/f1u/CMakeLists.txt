# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ocudu_f1u_cu_up
  cu_up/f1u_bearer_impl.cpp
  cu_up/f1u_bearer_factory.cpp
  cu_up/f1u_metrics_aggregator.cpp
)

add_library(ocudu_f1u_du
  du/f1u_bearer_impl.cpp
  du/f1u_bearer_factory.cpp
)

# Local connector
add_library(ocudu_f1u_connector local_connector/f1u_local_connector.cpp)
target_link_libraries(ocudu_f1u_connector ocudu_f1u_cu_up)

#Split connector
add_library(ocudu_f1u_split_connector split_connector/f1u_session_manager_impl.cpp split_connector/f1u_session_manager_factory.cpp )

add_library(ocudu_f1u_cu_up_split_connector cu_up/split_connector/f1u_split_connector_factory.cpp cu_up/split_connector/f1u_split_connector.cpp )
target_link_libraries(ocudu_f1u_cu_up_split_connector ocudu_f1u_split_connector ocudu_f1u_cu_up ocudu_gtpu)

add_library(ocudu_f1u_du_split_connector du/split_connector/f1u_split_connector.cpp du/split_connector/f1u_split_connector_factory.cpp)
target_link_libraries(ocudu_f1u_du_split_connector ocudu_f1u_split_connector ocudu_f1u_du ocudu_gtpu)
