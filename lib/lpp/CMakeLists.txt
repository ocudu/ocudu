# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_library(ocudu_lpp_lib STATIC reference_location.cpp)
target_link_libraries(ocudu_lpp_lib lpp_asn1)

add_to_exported_libs(ocudu_lpp_lib)
