# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(channel_precoder_benchmark channel_precoder_benchmark.cpp)
target_link_libraries(channel_precoder_benchmark ocudulog ocudu_channel_precoder)
add_test(channel_precoder_benchmark channel_precoder_benchmark -s -R 1)
