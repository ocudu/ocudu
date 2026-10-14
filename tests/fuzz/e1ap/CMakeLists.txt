# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set_directory_properties(PROPERTIES LABELS "fuzz|e1ap")

add_executable(e1ap_pdu_decoder_fuzzer e1ap_pdu_decoder_fuzzer.cpp)
target_include_directories(e1ap_pdu_decoder_fuzzer PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(e1ap_pdu_decoder_fuzzer
    ocudu_e1ap_common
    ocudu_support
    ocudulog
    ${FUZZ_SANITIZER_FLAGS_LINK}
)
target_compile_options(e1ap_pdu_decoder_fuzzer PRIVATE ${FUZZ_SANITIZER_FLAGS_COMPILE})

add_dependencies(fuzz_targets e1ap_pdu_decoder_fuzzer)
