# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

# ---------------------------------------------------------------------------
# Fuzzer sanitizer flags
#
# Each fuzzer binary is compiled with -fsanitize=fuzzer which:
#  - Embeds the AFL++ / libFuzzer driver (provides main() and the mutation
#    engine when using libFuzzer, or integrates with AFL++ LLVM mode when
#    compiling with afl-clang-fast++).
#  - Enables coverage-guided instrumentation for the fuzzer engine.
#
# ASAN is added when the project-level ENABLE_ASAN is ON so that memory
# safety bugs are caught during fuzzing runs.
# ---------------------------------------------------------------------------
set(FUZZ_SANITIZER_FLAGS_COMPILE "-fsanitize=fuzzer")
set(FUZZ_SANITIZER_FLAGS_LINK    "-fsanitize=fuzzer")

if (ENABLE_ASAN)
    string(APPEND FUZZ_SANITIZER_FLAGS_COMPILE ",address")
    string(APPEND FUZZ_SANITIZER_FLAGS_LINK    ",address")
endif ()

if (ENABLE_UBSAN)
    string(APPEND FUZZ_SANITIZER_FLAGS_COMPILE ",undefined")
    string(APPEND FUZZ_SANITIZER_FLAGS_LINK    ",undefined")
endif ()

message(STATUS "Fuzz sanitizer flags: ${FUZZ_SANITIZER_FLAGS_COMPILE}")

add_subdirectory(ofh)
