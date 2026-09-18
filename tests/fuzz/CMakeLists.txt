# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

# ---------------------------------------------------------------------------
# Fuzzer sanitizer flags
#
# When $LIB_FUZZING_ENGINE is set (OSS-Fuzz build environment), coverage
# instrumentation is already injected via $CXXFLAGS and the engine library
# is provided externally.  Use the env var for linking and leave the compile
# flag empty to avoid conflicting with OSS-Fuzz's own instrumentation.
#
# For local / CI builds (AFL++ via afl-clang-fast++ or plain libFuzzer),
# -fsanitize=fuzzer embeds the driver and enables coverage instrumentation.
#
# ASAN / UBSan are appended when the project-level ENABLE_ASAN / ENABLE_UBSAN
# options are ON so that memory safety bugs are caught during fuzzing runs.
# ---------------------------------------------------------------------------
if(DEFINED ENV{LIB_FUZZING_ENGINE})
    set(FUZZ_SANITIZER_FLAGS_COMPILE "")
    set(FUZZ_SANITIZER_FLAGS_LINK    $ENV{LIB_FUZZING_ENGINE})
else()
    set(FUZZ_SANITIZER_FLAGS_COMPILE "-fsanitize=fuzzer")
    set(FUZZ_SANITIZER_FLAGS_LINK    "-fsanitize=fuzzer")
endif()

if (ENABLE_ASAN)
    string(APPEND FUZZ_SANITIZER_FLAGS_COMPILE ",address")
    string(APPEND FUZZ_SANITIZER_FLAGS_LINK    ",address")
endif ()

if (ENABLE_UBSAN)
    string(APPEND FUZZ_SANITIZER_FLAGS_COMPILE ",undefined")
    string(APPEND FUZZ_SANITIZER_FLAGS_LINK    ",undefined")
endif ()

message(STATUS "Fuzz sanitizer flags: ${FUZZ_SANITIZER_FLAGS_COMPILE}")

# Aggregate target building every fuzz binary. Each sub-directory registers its own binaries, so
# adding a layer does not require updating the build instructions.
add_custom_target(fuzz_targets)

# ---------------------------------------------------------------------------
# Test doubles
#
# The harnesses reuse the message and key builders that the unit tests are written against, so that
# a harness uses the same correct scaffolding as the tests. But tests/fuzz is configured with
# BUILD_TESTING=OFF, where tests/test_doubles is not added, so the libraries are defined here when
# they are missing. They are built exactly as tests/test_doubles defines them, UNITY_BUILD included:
# rrc_packed_test_messages.cpp and f1ap_test_messages.cpp call into their sibling translation unit
# unqualified and do not compile on their own.
# ---------------------------------------------------------------------------
if (NOT TARGET security_test_doubles)
    add_library(security_test_doubles ${CMAKE_SOURCE_DIR}/tests/test_doubles/security/security_test_keys.cpp)
    target_include_directories(security_test_doubles PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(security_test_doubles ocudu_support ocudulog ocudu_security)
endif ()

if (NOT TARGET test_rng)
    add_library(test_rng ${CMAKE_SOURCE_DIR}/tests/test_doubles/utils/test_rng.cpp)
    target_link_libraries(test_rng PRIVATE ocudu_support)
endif ()

if (NOT TARGET pdcp_test_doubles)
    add_library(pdcp_test_doubles ${CMAKE_SOURCE_DIR}/tests/test_doubles/pdcp/pdcp_pdu_generator.cpp)
    set_target_properties(pdcp_test_doubles PROPERTIES UNITY_BUILD ON)
    target_link_libraries(pdcp_test_doubles ocudu_support ocudulog)
endif ()

if (NOT TARGET rrc_test_doubles)
    add_library(rrc_test_doubles
        ${CMAKE_SOURCE_DIR}/tests/test_doubles/rrc/rrc_test_messages.cpp
        ${CMAKE_SOURCE_DIR}/tests/test_doubles/rrc/rrc_test_message_validators.cpp
        ${CMAKE_SOURCE_DIR}/tests/test_doubles/rrc/rrc_packed_test_messages.cpp)
    set_target_properties(rrc_test_doubles PROPERTIES UNITY_BUILD ON)
    target_include_directories(rrc_test_doubles PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(rrc_test_doubles rrc_nr_asn1 ocudu_rrc ocudu_support ocudulog)
endif ()

if (NOT TARGET e1ap_test_doubles)
    add_library(e1ap_test_doubles
        ${CMAKE_SOURCE_DIR}/tests/test_doubles/e1ap/e1ap_cu_cp_test_messages.cpp
        ${CMAKE_SOURCE_DIR}/tests/test_doubles/e1ap/e1ap_cu_up_test_messages.cpp
        ${CMAKE_SOURCE_DIR}/tests/test_doubles/e1ap/e1ap_test_message_validators.cpp)
    set_target_properties(e1ap_test_doubles PROPERTIES UNITY_BUILD ON)
    target_include_directories(e1ap_test_doubles PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(e1ap_test_doubles ocudu_support ocudulog ocudu_e1ap_common e1ap_asn1)
endif ()

if (NOT TARGET f1ap_test_doubles)
    add_library(f1ap_test_doubles
        ${CMAKE_SOURCE_DIR}/tests/test_doubles/f1ap/f1ap_test_message_validators.cpp
        ${CMAKE_SOURCE_DIR}/tests/test_doubles/f1ap/f1ap_test_messages.cpp)
    set_target_properties(f1ap_test_doubles PROPERTIES UNITY_BUILD ON)
    target_include_directories(f1ap_test_doubles PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(f1ap_test_doubles ocudu_support ocudulog f1ap_asn1 ocudu_f1ap_common rrc_test_doubles
                          pdcp_test_doubles test_rng)
endif ()

add_subdirectory(ofh)
add_subdirectory(ngap)
add_subdirectory(rrc)
