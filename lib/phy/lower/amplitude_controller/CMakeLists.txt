# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ocudu_lower_phy_amplitude_controller STATIC
        amplitude_controller_clipping_impl.cpp
        amplitude_controller_scaling_impl.cpp
        amplitude_controller_factories.cpp)
target_link_libraries(ocudu_lower_phy_amplitude_controller ocuduvec)
