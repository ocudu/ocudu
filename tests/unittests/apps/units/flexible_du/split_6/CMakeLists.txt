# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set_directory_properties(PROPERTIES LABELS "split_6;apps")

add_executable(slot_point_extender_adaptor_test
        slot_point_extender_adaptor_test.cpp
        ${CMAKE_SOURCE_DIR}/apps/units/flexible_o_du/split_6/o_du_high/slot_point_extender_adaptor.cpp)
target_include_directories(slot_point_extender_adaptor_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(slot_point_extender_adaptor_test ocudu_support ocudu_gtest_main gtest)
add_test(slot_point_extender_adaptor_test slot_point_extender_adaptor_test)
