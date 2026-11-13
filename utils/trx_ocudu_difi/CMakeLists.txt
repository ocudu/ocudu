# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# Find TRX header
find_package(TRX_DRIVER MODULE REQUIRED)

# The sample conversion uses SSE2 intrinsics.
if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    message(SEND_ERROR "The DIFI TRX driver requires an x86-64 host.")
endif ()

# Add shared library. The driver is self-contained, so it links no ocudu library.
add_library(trx_ocudu_difi SHARED trx_ocudu_difi.cpp)
target_include_directories(trx_ocudu_difi PRIVATE ${TRX_DRIVER_DIR})
target_link_libraries(trx_ocudu_difi PRIVATE m)

# Add dynamic library test.
add_executable(trx_ocudu_difi_test trx_ocudu_difi_test.cpp)
target_link_libraries(trx_ocudu_difi_test dl)
target_include_directories(trx_ocudu_difi_test PRIVATE ${TRX_DRIVER_DIR})
add_test(trx_ocudu_difi_test trx_ocudu_difi_test ./libtrx_ocudu_difi.so)

# Make sure libtrx_ocudu_difi is compiled when trx_ocudu_difi_test is the only target.
add_dependencies(trx_ocudu_difi_test trx_ocudu_difi)
