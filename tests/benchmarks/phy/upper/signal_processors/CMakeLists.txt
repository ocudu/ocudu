# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(dmrs_pdsch_processor_benchmark dmrs_pdsch_processor_benchmark.cpp)
target_link_libraries(dmrs_pdsch_processor_benchmark
        ocuduvec
        ocudulog
        ocudu_dmrs_pdsch
        ocudu_phy_support
        ocudu_sequence_generators
        ocudu_channel_precoder)
add_test(dmrs_pdsch_processor_benchmark dmrs_pdsch_processor_benchmark -D -s -R 1)


add_executable(srs_estimator_benchmark srs_estimator_benchmark.cpp)
target_link_libraries(srs_estimator_benchmark
        ocuduvec
        ocudulog
        ocudu_channel_precoder
        ocudu_srs_estimator
        ocudu_phy_support
        ocudu_dft
        ocudu_sequence_generators)
add_test(srs_estimator_benchmark srs_estimator_benchmark -D -s -R 1)
