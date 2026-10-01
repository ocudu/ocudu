# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set_directory_properties(PROPERTIES LABELS "ofh")

add_executable(ofh_compression_benchmark ofh_compression_benchmark.cpp)
target_link_libraries(ofh_compression_benchmark ocudulog ocudu_ofh_compression)
