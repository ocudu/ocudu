# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


include_directories(../../..)

add_executable(worker_pool_benchmark worker_pool_benchmark.cpp)
target_link_libraries(worker_pool_benchmark ocudu_support ocudulog)
add_test(worker_pool_benchmark worker_pool_benchmark)
set_tests_properties(worker_pool_benchmark PROPERTIES LABELS "tsan")
