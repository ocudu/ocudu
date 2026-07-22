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

add_subdirectory(ofh)
add_subdirectory(ngap)
