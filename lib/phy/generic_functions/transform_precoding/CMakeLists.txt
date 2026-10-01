# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(transform_precoding_sources
        transform_precoder_dft_impl.cpp
        transform_precoding_factories.cpp)

add_library(ocudu_transform_precoding STATIC ${transform_precoding_sources})

add_to_exported_libs(ocudu_transform_precoding)
