# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(modulation_chain_benchmark modulation_chain_benchmark.cpp)
target_link_libraries(modulation_chain_benchmark ocudu_channel_modulation ocuduvec ocudulog)
add_test(modulation_chain_benchmark modulation_chain_benchmark -s -R 1)
