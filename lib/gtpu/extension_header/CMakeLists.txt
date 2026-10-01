# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set(SOURCES
  long_pdcp_pdu_number_packing.cpp
  pdcp_pdu_number_packing.cpp
)
add_library(ocudu_gtpu_extension_header STATIC ${SOURCES})
target_link_libraries(ocudu_gtpu_extension_header
  ocudu_nru
  ocudu_psup
  ocudu_support
  ocudu_gateway
)
