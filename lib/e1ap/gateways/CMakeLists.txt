# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ocudu_e1_gateway
        e1_local_connector_factory.cpp
        e1_network_client_factory.cpp
        e1_network_server_factory.cpp)
target_link_libraries(ocudu_e1_gateway ocudu_support ocudu_e1ap_common e1ap_asn1 ocudu_pcap)
