# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


find_package(SCTP REQUIRED)

add_library(ocudu_gateway
        sctp_socket.cpp
        sctp_network_gateway_common_impl.cpp
        sctp_network_client_impl.cpp
        sctp_network_server_impl.cpp
        sctp_dtls.cpp
        udp_network_gateway_impl.cpp
        sctp_network_client_factory.cpp
        sctp_network_server_factory.cpp
        udp_network_gateway_factory.cpp
)

if (OPENSSL_HAS_DTLS_SCTP)
	target_compile_definitions(ocudu_gateway PRIVATE OCUDU_HAVE_OPENSSL_DTLS=1)
	target_link_libraries(ocudu_gateway ocudu_support ocudulog OpenSSL::SSL OpenSSL::Crypto ${SCTP_LIBRARIES})
else ()
	target_link_libraries(ocudu_gateway ocudu_support ocudulog ${SCTP_LIBRARIES})
endif()
