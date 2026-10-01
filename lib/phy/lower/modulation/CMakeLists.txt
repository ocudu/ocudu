# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ocudu_lower_phy_modulation STATIC
        modulation_factories.cpp
        ofdm_modulator_impl.cpp
        ofdm_demodulator_impl.cpp
        ofdm_prach_demodulator_impl.cpp)
target_link_libraries(ocudu_lower_phy_modulation ocuduvec ocudu_ran ocudu_support)
