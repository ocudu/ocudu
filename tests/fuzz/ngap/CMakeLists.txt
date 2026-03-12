# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set_directory_properties(PROPERTIES LABELS "fuzz_ngap")

# ---------------------------------------------------------------------------
# NGAP PDU decoder fuzzer
# ---------------------------------------------------------------------------
add_executable(ngap_pdu_decoder_fuzzer ngap_pdu_decoder_fuzzer.cpp)
target_include_directories(ngap_pdu_decoder_fuzzer PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ngap_pdu_decoder_fuzzer
    ngap_asn1
    ocudu_support
    ocudulog
    ${FUZZ_SANITIZER_FLAGS_LINK}
)
target_compile_options(ngap_pdu_decoder_fuzzer PRIVATE ${FUZZ_SANITIZER_FLAGS_COMPILE})
