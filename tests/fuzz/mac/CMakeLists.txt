# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set_directory_properties(PROPERTIES LABELS "fuzz|mac")

# ---------------------------------------------------------------------------
# MAC UL processor fuzzer
#
# Drives a mac_ul_processor with one Rx data indication per input, so that the MAC CE decoders run on the
# paths pdu_rx_handler takes for the attached UE, for a Msg3 and for a MsgA.
# ---------------------------------------------------------------------------
add_executable(mac_ul_processor_fuzzer mac_ul_processor_fuzzer.cpp)
target_include_directories(mac_ul_processor_fuzzer PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(mac_ul_processor_fuzzer
    ocudu_mac
    ocudu_support
    ocudulog
    ${FUZZ_SANITIZER_FLAGS_LINK}
)
target_compile_options(mac_ul_processor_fuzzer PRIVATE ${FUZZ_SANITIZER_FLAGS_COMPILE})

add_dependencies(fuzz_targets mac_ul_processor_fuzzer)

add_fuzz_corpus(mac_fuzz_corpus seeds.py)
