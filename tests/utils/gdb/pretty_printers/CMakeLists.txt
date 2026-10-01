# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set_directory_properties(PROPERTIES LABELS "pretty_printers")

add_executable(pretty_printer_bf16_test pretty_printer_bf16_test.cpp)
add_test(NAME pretty_printer_bf16_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_bf16_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = 1.2265625'"
)

add_executable(pretty_printer_bounded_bitset_test pretty_printer_bounded_bitset_test.cpp)
target_link_libraries(pretty_printer_bounded_bitset_test PRIVATE fmt)
add_test(NAME pretty_printer_bounded_bitset_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_bounded_bitset_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = bounded_bitset of length 4, capacity 5 = 0100'"
)

add_executable(pretty_printer_cbf16_test pretty_printer_cbf16_test.cpp)
add_test(NAME pretty_printer_cbf16_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_cbf16_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = 1.2265625 + 4.5625i'"
)

add_executable(pretty_printer_llr_test pretty_printer_llr_test.cpp)
target_link_libraries(pretty_printer_llr_test PRIVATE fmt)
add_test(NAME pretty_printer_llr_finite_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_llr_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = 33'"
)
add_test(NAME pretty_printer_llr_infinite_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_llr_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'b = 127'"
)

add_executable(pretty_printer_slotted_array_test pretty_printer_slotted_array_test.cpp)
target_link_libraries(pretty_printer_slotted_array_test PRIVATE fmt)
add_test(NAME pretty_printer_slotted_array_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_slotted_array_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = slotted_array of 2 elements, capacity 5 = {\\[1\\] = 3, \\[3\\] = -5}'"
)

add_executable(pretty_printer_slotted_vector_test pretty_printer_slotted_vector_test.cpp)
add_test(NAME pretty_printer_slotted_vector_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_slotted_vector_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = slotted_vector of 2 elements = {\\[1\\] = 3, \\[3\\] = -5}'"
)

add_executable(pretty_printer_static_vector_test pretty_printer_static_vector_test.cpp)
target_link_libraries(pretty_printer_static_vector_test PRIVATE fmt)
add_test(NAME pretty_printer_static_vector_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_static_vector_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = static_vector of length 3, capacity 5 = {0, 1, 2}'"
)

add_executable(pretty_printer_strong_type_test pretty_printer_strong_type_test.cpp)
add_test(NAME pretty_printer_strong_type_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_strong_type_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = 42'")
add_test(NAME pretty_printer_strong_type_derived_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_strong_type_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'b = 8'")

add_executable(pretty_printer_span_test pretty_printer_span_test.cpp)
add_test(NAME pretty_printer_span_nonempty_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_span_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = span of length 3 = {10, 20, 30}'")
add_test(NAME pretty_printer_span_empty_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_span_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'b = span of length 0'")

add_executable(pretty_printer_byte_buffer_test pretty_printer_byte_buffer_test.cpp)
target_link_libraries(pretty_printer_byte_buffer_test PRIVATE ocudu_support)
add_test(NAME pretty_printer_byte_buffer_nonempty_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_byte_buffer_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = byte_buffer of length 3 = \\[01 a2 f3\\]'")
add_test(NAME pretty_printer_byte_buffer_empty_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_byte_buffer_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'b = byte_buffer of length 0'")
add_test(NAME pretty_printer_byte_buffer_view_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_byte_buffer_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'c = byte_buffer_view of length 3 = \\[01 a2 f3\\]'")
add_test(NAME pretty_printer_byte_buffer_slice_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_byte_buffer_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'd = byte_buffer_slice of length 3 = \\[01 a2 f3\\]'")

add_executable(pretty_printer_tl_expected_test pretty_printer_tl_expected_test.cpp)
add_test(NAME pretty_printer_tl_expected_value_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_tl_expected_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = expected (has value) = {5}'"
)
add_test(NAME pretty_printer_tl_expected_error_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_tl_expected_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'b = expected (has error) = {my_error::err1}'"
)

add_executable(pretty_printer_tiny_optional_test pretty_printer_tiny_optional_test.cpp)
add_test(NAME pretty_printer_tiny_optional_empty_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_tiny_optional_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'c = tiny_optional (empty)'"
)
add_test(NAME pretty_printer_tiny_optional_alternative_empty_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_tiny_optional_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'd = tiny_optional (empty)'"
)
add_test(NAME pretty_printer_tiny_optional_present_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_tiny_optional_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'a = tiny_optional (present) = {std::unique_ptr<int> = {get() = 0x[0-9a-f]\\{12\\}}}'"
)
add_test(NAME pretty_printer_tiny_optional_alternative_present_test
         COMMAND sh -c "gdb $<TARGET_FILE:pretty_printer_tiny_optional_test> -x ${CMAKE_CURRENT_SOURCE_DIR}/pretty_printer_gdb_commands.txt --batch | grep 'b = tiny_optional (present) = {3.4000001}'"
)

add_custom_target(pretty_printer_tests)
add_dependencies(pretty_printer_tests
        pretty_printer_bf16_test
        pretty_printer_bounded_bitset_test
        pretty_printer_byte_buffer_test
        pretty_printer_cbf16_test
        pretty_printer_llr_test
        pretty_printer_slotted_array_test
        pretty_printer_slotted_vector_test
        pretty_printer_span_test
        pretty_printer_static_vector_test
        pretty_printer_strong_type_test
        pretty_printer_tiny_optional_test
        pretty_printer_tl_expected_test
)
