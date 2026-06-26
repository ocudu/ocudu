# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
  rlc_am_pdu.cpp
  rlc_bearer_metrics_collector.cpp
  rlc_factory.cpp
  rlc_tx_am_entity.cpp
  rlc_tx_tm_entity.cpp
  rlc_tx_um_entity.cpp
  rlc_rx_am_entity.cpp
  rlc_rx_tm_entity.cpp
  rlc_rx_um_entity.cpp
  rlc_window_seg_pools.cpp
)

add_library(ocudu_rlc STATIC ${SOURCES})
target_link_libraries(ocudu_rlc ocudu_instrumentation ocudu_support ocudu_pcap)
