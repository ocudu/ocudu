# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES pdcp_factory.cpp pdcp_entity_tx.cpp pdcp_entity_rx.cpp pdcp_tx_window.cpp pdcp_pdu.cpp pdcp_metrics_aggregator.cpp)

add_library(ocudu_pdcp STATIC ${SOURCES})
target_link_libraries(ocudu_pdcp ocudu_rohc ocudu_security ocudu_instrumentation ocudu_support)
