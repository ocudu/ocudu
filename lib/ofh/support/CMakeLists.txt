# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(SOURCES beamforming_weights_repository.cpp)

add_library(ocudu_ofh_support STATIC ${SOURCES})
target_link_libraries(ocudu_ofh_support ocudu_ofh_compression ocudu_ran ocudu_instrumentation)
