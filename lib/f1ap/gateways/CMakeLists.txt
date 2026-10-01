# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ocudu_f1c_gateway
        f1c_network_client_factory.cpp
        f1c_network_server_factory.cpp
        f1c_local_connector_factory.cpp)
target_include_directories(ocudu_f1c_gateway PRIVATE ..)
target_link_libraries(ocudu_f1c_gateway f1ap_asn1 ocudu_support ocudu_pcap)
