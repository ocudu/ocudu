# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(ldpc_encoder_benchmark ldpc_encoder_benchmark.cpp)
target_link_libraries(ldpc_encoder_benchmark ocudu_channel_coding ocudulog)
add_test(ldpc_encoder_benchmark ldpc_encoder_benchmark -s -R 1 -T generic)

add_executable(ldpc_decoder_benchmark ldpc_decoder_benchmark.cpp)
target_link_libraries(ldpc_decoder_benchmark ocudu_channel_coding ocudulog)
add_test(ldpc_decoder_benchmark ldpc_decoder_benchmark -s -R 1 -I 2 -T generic)
