#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# Fails if std::chrono::high_resolution_clock is used in the C/C++ sources.
#
# high_resolution_clock is an alias of either system_clock or steady_clock depending on the standard library
# implementation, so its semantics are not portable. Use steady_clock to measure durations and intervals, and
# system_clock for wall clock time.

files=$(git ls-files -- \
    '*.c' '*.cc' '*.c++' '*.cxx' '*.cl' '*.cpp' '*.h' '*.hh' '*.hpp' '*.h.in' \
    ':(exclude)*/bundled/*' ':(exclude)*/external/*')

if [ -z "${files}" ]; then
  echo "::error::No source files found, is this a repository checkout?"
  exit 1
fi

matches=$(echo "${files}" | tr '\n' '\0' | xargs -0 grep -Hn 'high_resolution_clock')

if [ -n "${matches}" ]; then
  echo "${matches}"
  echo "::error::high_resolution_clock is not allowed, use steady_clock for durations and system_clock for wall clock time"
  exit 1
fi

echo "No usage of std::chrono::high_resolution_clock found."
