# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


add_subdirectory(mac)
add_subdirectory(phy)

set(PRECODING_SOURCES
        precoding_codebook_mapper.cpp
        precoding_codebook_repository.cpp
        precoding_codebook_generator.cpp
        )

set(UCI_SOURCES
        uci_part2_correspondence_generator.cpp
        uci_part2_correspondence_mapper.cpp
        uci_part2_correspondence_repository.cpp
        )

add_library(ocudu_fapi_precoding_matrix_tools STATIC ${PRECODING_SOURCES})
target_link_libraries(ocudu_fapi_precoding_matrix_tools ocudu_ran ocudulog)

add_library(ocudu_fapi_uci_part2_tools STATIC ${UCI_SOURCES})
target_link_libraries(ocudu_fapi_uci_part2_tools ocudu_ran ocudulog)
