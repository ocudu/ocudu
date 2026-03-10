# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set_directory_properties(PROPERTIES LABELS "fuzz_ofh")

# ---------------------------------------------------------------------------
# OFH U-Plane decoder fuzzer
# ---------------------------------------------------------------------------
add_executable(ofh_uplane_decoder_fuzzer ofh_uplane_decoder_fuzzer.cpp)
target_include_directories(ofh_uplane_decoder_fuzzer PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ofh_uplane_decoder_fuzzer
    ocudu_ofh_message_serdes
    ocudu_support
    ocudulog
    ${FUZZ_SANITIZER_FLAGS_LINK}
)
target_compile_options(ofh_uplane_decoder_fuzzer PRIVATE ${FUZZ_SANITIZER_FLAGS_COMPILE})

# ---------------------------------------------------------------------------
# OFH eCPRI decoder fuzzer
# ---------------------------------------------------------------------------
add_executable(ofh_ecpri_decoder_fuzzer ofh_ecpri_decoder_fuzzer.cpp)
target_include_directories(ofh_ecpri_decoder_fuzzer PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ofh_ecpri_decoder_fuzzer
    ocudu_ofh_ecpri
    ocudu_support
    ocudulog
    ${FUZZ_SANITIZER_FLAGS_LINK}
)
target_compile_options(ofh_ecpri_decoder_fuzzer PRIVATE ${FUZZ_SANITIZER_FLAGS_COMPILE})

# ---------------------------------------------------------------------------
# OFH VLAN frame decoder fuzzer
# ---------------------------------------------------------------------------
add_executable(ofh_vlan_frame_decoder_fuzzer ofh_vlan_frame_decoder_fuzzer.cpp)
target_include_directories(ofh_vlan_frame_decoder_fuzzer PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ofh_vlan_frame_decoder_fuzzer
    ocudu_ofh_ethernet
    ocudu_support
    ocudulog
    ${FUZZ_SANITIZER_FLAGS_LINK}
)
target_compile_options(ofh_vlan_frame_decoder_fuzzer PRIVATE ${FUZZ_SANITIZER_FLAGS_COMPILE})
