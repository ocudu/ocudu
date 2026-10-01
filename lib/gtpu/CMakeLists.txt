# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_subdirectory(extension_header)

set(SOURCES
  gtpu_demux_impl.cpp
  gtpu_demux_factory.cpp
  gtpu_echo_factory.cpp
  gtpu_gateway.cpp
  gtpu_pdu.cpp
  gtpu_teid_pool_factory.cpp
  gtpu_tunnel_nru_factory.cpp
  gtpu_tunnel_pdcp_factory.cpp
  gtpu_tunnel_pdcp_rx_impl.cpp
  gtpu_tunnel_pdcp_tx_impl.cpp
  gtpu_tunnel_psup_factory.cpp
)
add_library(ocudu_gtpu STATIC ${SOURCES})
target_link_libraries(ocudu_gtpu
  ocudu_gateway
  ocudu_gtpu_extension_header
  ocudu_nru
  ocudu_psup
  ocudu_support
)
