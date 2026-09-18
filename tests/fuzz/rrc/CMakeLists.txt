# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set_directory_properties(PROPERTIES LABELS "fuzz|rrc")

# ---------------------------------------------------------------------------
# RRC UE uplink fuzzer
#
# Drives an rrc_ue_impl directly, without a CU-CP, PDCP or F1AP around it, so that the fuzzer
# controls the integrity_verified flag and reaches the TS 38.331 Annex B1 gating branches.
# ---------------------------------------------------------------------------
add_executable(rrc_ue_fuzzer rrc_ue_fuzzer.cpp)
target_include_directories(rrc_ue_fuzzer PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(rrc_ue_fuzzer
    rrc_test_doubles
    security_test_doubles
    ocudu_rrc
    ocudu_cu_cp
    ocudu_support
    ocudulog
    ${FUZZ_SANITIZER_FLAGS_LINK}
)
target_compile_options(rrc_ue_fuzzer PRIVATE ${FUZZ_SANITIZER_FLAGS_COMPILE})

add_dependencies(fuzz_targets rrc_ue_fuzzer)

# ---------------------------------------------------------------------------
# Full-stack RRC / CU-CP fuzzer
#
# Injects RRC messages through a complete CU-CP, so that the F1AP, PDCP and CU-CP layers around RRC
# are exercised on every input.
#
# ---------------------------------------------------------------------------
add_executable(rrc_cu_cp_fuzzer
    rrc_cu_cp_fuzzer.cpp
    ${CMAKE_SOURCE_DIR}/tests/unittests/ngap/ngap_test_messages.cpp
)
target_include_directories(rrc_cu_cp_fuzzer PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(rrc_cu_cp_fuzzer
    e1ap_test_doubles
    f1ap_test_doubles
    rrc_test_doubles
    ocudu_cu_cp
    ocudu_rrc
    ocudu_f1ap_common
    f1ap_asn1
    ngap_asn1
    e1ap_asn1
    rrc_nr_asn1
    ocudu_support
    ocudulog
    ${FUZZ_SANITIZER_FLAGS_LINK}
)
target_compile_options(rrc_cu_cp_fuzzer PRIVATE ${FUZZ_SANITIZER_FLAGS_COMPILE})

add_dependencies(fuzz_targets rrc_cu_cp_fuzzer)
