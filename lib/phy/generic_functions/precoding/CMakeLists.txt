# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(channel_precoder_sources
        channel_precoder_impl.cpp
        channel_precoder_generic.cpp
        precoding_factories.cpp)

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    list(APPEND channel_precoder_sources
            channel_precoder_avx2.cpp
            channel_precoder_avx512.cpp)
    set_source_files_properties(channel_precoder_avx2.cpp PROPERTIES COMPILE_OPTIONS "-mavx2;-mfma;")
    set_source_files_properties(channel_precoder_avx512.cpp PROPERTIES COMPILE_OPTIONS "-mavx512f;-mavx512bw")
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    list(APPEND channel_precoder_sources channel_precoder_neon.cpp)
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")


add_library(ocudu_channel_precoder STATIC ${channel_precoder_sources})

add_to_exported_libs(ocudu_channel_precoder)
