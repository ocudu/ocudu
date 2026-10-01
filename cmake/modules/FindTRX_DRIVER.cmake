# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(TRX_DRIVER_HEADER trx_driver.h)

if (NOT DEFINED TRX_DRIVER_DIR)
    message(SEND_ERROR "Missing ${TRX_DRIVER_HEADER} directory variable. Rerun cmake with -DTRX_DRIVER_DIR=path_to_directory")
endif (NOT DEFINED TRX_DRIVER_DIR)

find_file(TRX_DRIVER_HEADER_PATH
        NAMES ${TRX_DRIVER_HEADER}
        HINTS ${TRX_DRIVER_DIR}
        PATHS ${TRX_DRIVER_DIR}
        REQUIRED
        )

message(STATUS "Found ${TRX_DRIVER_HEADER} in TRX_DRIVER_DIR=${TRX_DRIVER_HEADER_PATH}")
