# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        channel_equalizer_generic_impl.cpp
        equalization_factories.cpp)

add_library(ocudu_channel_equalizer STATIC ${SOURCES})
target_link_libraries(ocudu_channel_equalizer ocuduvec)

add_to_exported_libs(ocudu_channel_equalizer)
