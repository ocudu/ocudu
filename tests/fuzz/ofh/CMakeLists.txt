# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set_directory_properties(PROPERTIES LABELS "fuzz|ofh")

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

# ---------------------------------------------------------------------------
# OFH receiver fuzzer
#
# Wires the receiver and the uplink request handler of a sector to shared context repositories, and
# replays uplink requests, received frames and OTA symbols, so that the receiver data flows run against
# the contexts stored by the uplink request handler.
# ---------------------------------------------------------------------------
add_executable(ofh_receiver_fuzzer ofh_receiver_fuzzer.cpp)
target_include_directories(ofh_receiver_fuzzer PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ofh_receiver_fuzzer
    ocudu_ofh
    ocudu_phy_support
    ocudu_support
    ocudulog
    ${FUZZ_SANITIZER_FLAGS_LINK}
)
target_compile_options(ofh_receiver_fuzzer PRIVATE ${FUZZ_SANITIZER_FLAGS_COMPILE})

add_dependencies(fuzz_targets
    ofh_uplane_decoder_fuzzer
    ofh_ecpri_decoder_fuzzer
    ofh_vlan_frame_decoder_fuzzer
    ofh_receiver_fuzzer
)

add_fuzz_corpus(ofh_fuzz_corpus seeds.py)
