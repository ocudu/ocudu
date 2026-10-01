# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(SOURCES
        ecpri_factories.cpp
        ecpri_packet_builder_impl.cpp
        ecpri_packet_decoder_impl.cpp)

add_library(ocudu_ofh_ecpri STATIC ${SOURCES})
target_link_libraries(ocudu_ofh_ecpri ocudulog)
