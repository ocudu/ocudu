# SPDX-FileCopyrightText: Copyright (C) 2021-2026 DeepSig Inc
# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# Adds a CUDA test. It includes setting a device lock and appending a CUDA label.
macro(ADD_CUDA_TEST TEST_NAME)
    # Add the test as normal.
    add_test(${TEST_NAME} ${ARGN})

    # The CUDA runtime rejects allocations with cudaErrorDevicesUnavailable while several exclusive
    # device contexts are alive, which surfaces as intermittent failures that look like defects in
    # the accelerated code. This lock lets ctest keep running the rest of the suite in parallel while
    # serialising the tests that drive the GPU.
    set_tests_properties(${TEST_NAME} PROPERTIES RESOURCE_LOCK ocudu_cuda_device)

    # Tag test as CUDA. Setting the property replaces the labels the directory applies, so the
    # directory ones are read back and kept.
    get_directory_property(OCUDU_DIRECTORY_LABELS LABELS)
    if (OCUDU_DIRECTORY_LABELS)
        set_tests_properties(${TEST_NAME} PROPERTIES LABELS "${OCUDU_DIRECTORY_LABELS};cuda")
    else ()
        set_tests_properties(${TEST_NAME} PROPERTIES LABELS "cuda")
    endif ()
endmacro(ADD_CUDA_TEST TEST_NAME)
