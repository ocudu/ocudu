# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(SOURCES
        backend_pcap_writer.cpp
        dlt_pcap_impl.cpp
        mac_pcap_impl.cpp
        pcap_file_writer.cpp
        rlc_pcap_impl.cpp
        pcap_rlc_pdu_context.cpp
)

add_library(ocudu_pcap STATIC ${SOURCES})
