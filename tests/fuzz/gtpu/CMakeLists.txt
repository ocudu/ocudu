# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set_directory_properties(PROPERTIES LABELS "fuzz|gtpu")

add_executable(gtpu_pdu_fuzzer gtpu_pdu_fuzzer.cpp)
target_include_directories(gtpu_pdu_fuzzer PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(gtpu_pdu_fuzzer
    ocudu_gtpu
    ocudu_support
    ocudulog
    ${FUZZ_SANITIZER_FLAGS_LINK}
)
target_compile_options(gtpu_pdu_fuzzer PRIVATE ${FUZZ_SANITIZER_FLAGS_COMPILE})

add_dependencies(fuzz_targets gtpu_pdu_fuzzer)
