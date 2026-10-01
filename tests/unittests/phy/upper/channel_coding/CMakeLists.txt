# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.



set_directory_properties(PROPERTIES LABELS "phy")

add_executable(crc_calculator_test crc_calculator_test.cpp)
target_link_libraries(crc_calculator_test ocudu_channel_coding gtest gtest_main)
add_test(crc_calculator_test crc_calculator_test)

add_subdirectory(polar)
add_subdirectory(ldpc)
add_subdirectory(short)
