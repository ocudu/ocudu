# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


add_executable(bounded_bitset_count_benchmark bounded_bitset_count_benchmark.cpp)
target_link_libraries(bounded_bitset_count_benchmark ocudulog)
add_test(bounded_bitset_count_benchmark bounded_bitset_count_benchmark -R 100)

add_executable(bounded_bitset_range_benchmark bounded_bitset_range_benchmark.cpp)
target_link_libraries(bounded_bitset_range_benchmark ocudulog)
add_test(bounded_bitset_range_benchmark bounded_bitset_range_benchmark -R 100)

