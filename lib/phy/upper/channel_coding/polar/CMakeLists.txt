# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ocudu_polar STATIC
        polar_allocator_impl.cpp
        polar_code_impl.cpp
        polar_deallocator_impl.cpp
        polar_decoder_impl.cpp
        polar_encoder_impl.cpp
        polar_interleaver_impl.cpp
        polar_rate_dematcher_impl.cpp
        polar_rate_matcher_impl.cpp
        )
target_link_libraries(ocudu_polar log_likelihood_ratio ocudulog)