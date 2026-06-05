# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set(SOURCES
        converters/reference_frame_converter.cpp
        converters/ephemeris_info_converter.cpp
        converters/coordinate_converter.cpp
        propagators/ordinary_differential_equation.cpp
        propagators/rk4_propagator.cpp
        propagators/keplerian_propagator.cpp
        orbit_ephemeris_info.cpp
        ntn_math_helpers.cpp
        ntn_assistance_info_generator.cpp
        ntn_configuration_manager_impl.cpp
        ntn_configuration_manager_factory.cpp
)

add_library(ocudu_ntn_lib STATIC ${SOURCES})

add_to_exported_libs(ocudu_ntn_lib)
