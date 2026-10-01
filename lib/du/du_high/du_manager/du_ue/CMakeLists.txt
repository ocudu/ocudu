# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(du_ue du_bearer.cpp du_ue_adapters.cpp du_ue_bearer_manager.cpp du_ue_cond_mobility_manager.cpp du_ue_controller_impl.cpp du_ue_manager.cpp)
target_link_libraries(du_ue ocudu_du_manager_procedures ocudu_f1u_du ocudu_gtpu)
