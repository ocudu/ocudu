# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_executable(pdxch_modulator_benchmark pdxch_modulator_benchmark.cpp)
target_include_directories(pdxch_modulator_benchmark PRIVATE ${CMAKE_SOURCE_DIR}/lib/phy/lower/processors/downlink/pdxch)
target_link_libraries(pdxch_modulator_benchmark
        ocudu_lower_phy_modulation
        ocudu_lower_phy_amplitude_controller
        ocudu_phy_support
        ocudu_dft
        ocudu_ran
        ocudu_instrumentation
        ocudu_support
        ocuduvec
        ocudulog)
add_test(pdxch_modulator_benchmark pdxch_modulator_benchmark -R 10 -T 2)
