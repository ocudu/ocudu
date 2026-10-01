# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(radio_util_sample radio_util_sample.cpp)
target_link_libraries(radio_util_sample ocudu_radio ocudulog ocudu_support)

add_executable(rx_power_analyzer rx_power_analyzer.cpp)
target_link_libraries(rx_power_analyzer ocudu_radio ocudulog ocudu_support ocuduvec)
