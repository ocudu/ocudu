# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set_directory_properties(PROPERTIES LABELS "fuzz|f1ap")

add_executable(f1ap_pdu_decoder_fuzzer f1ap_pdu_decoder_fuzzer.cpp)
target_include_directories(f1ap_pdu_decoder_fuzzer PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(f1ap_pdu_decoder_fuzzer
    ocudu_f1ap_common
    ocudu_support
    ocudulog
    ${FUZZ_SANITIZER_FLAGS_LINK}
)
target_compile_options(f1ap_pdu_decoder_fuzzer PRIVATE ${FUZZ_SANITIZER_FLAGS_COMPILE})

add_dependencies(fuzz_targets f1ap_pdu_decoder_fuzzer)

add_fuzz_corpus(f1ap_fuzz_corpus seeds.py)
