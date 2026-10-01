# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(SOURCES 
  bbdev.cpp
  bbdev_acc.cpp
  bbdev_acc_factory.cpp
  bbdev_op_pool_factory.cpp
  ldpc/bbdev_ldpc_encoder.cpp
  ldpc/bbdev_ldpc_decoder.cpp
)

add_library(ocudu_hal_bbdev STATIC ${SOURCES})
target_compile_options(ocudu_hal_bbdev PRIVATE ${DPDK_CFLAGS})
target_link_libraries(ocudu_hal_bbdev ocudulog ${DPDK_LIBRARIES} dl)
