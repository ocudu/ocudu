#!/bin/sh

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

# Test launcher that gives each test process its own gtest XML report, so that the requirement tags
# the tests record can be merged into the ctest JUnit output afterwards.
#
# Does nothing unless OCUDU_GTEST_XML_DIR is set, and passes the request through the environment
# rather than the command line, so that a test which does not link gtest is unaffected.

if [ -n "${OCUDU_GTEST_XML_DIR}" ]; then
    # The file name carries the binary, which is how a whole-binary ctest entry is matched later. The
    # pid and the timestamp keep parallel ctest jobs from overwriting each other: pointing gtest at a
    # directory instead would let it pick the name, and it does so with a racy existence check.
    GTEST_OUTPUT="xml:${OCUDU_GTEST_XML_DIR}/$(basename "$1").$$.$(date +%s%N).xml"
    export GTEST_OUTPUT
fi

exec "$@"
