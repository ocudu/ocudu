# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(pucch_processor_benchmark pucch_processor_benchmark.cpp)
set(PUCCH_PROCESSOR_LIBRARIES
        ocudu_pucch_processor
        ocudu_channel_equalizer
        ocudu_channel_processors
        ocudu_transform_precoding
        ocudulog
        ocuduvec)

target_link_libraries(pucch_processor_benchmark ${PUCCH_PROCESSOR_LIBRARIES})
add_test(pucch_processor_benchmark pucch_processor_benchmark -m silent -R 1 -B 1 -T 2)
