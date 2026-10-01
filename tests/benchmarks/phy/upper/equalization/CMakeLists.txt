# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(channel_equalizer_benchmark channel_equalizer_benchmark.cpp)
target_link_libraries(channel_equalizer_benchmark ocuduvec ocudulog ocudu_channel_equalizer)
add_test(channel_equalizer_benchmark channel_equalizer_benchmark -s -R 1 -T ZF)
add_test(channel_equalizer_benchmark channel_equalizer_benchmark -s -R 1 -T MMSE)
