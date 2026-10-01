# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_subdirectory(du_high)
add_subdirectory(du_low)

set(SOURCES du_cell_config_validation.cpp du_update_config_helpers.cpp)
add_library(ocudu_du_config_validators STATIC ${SOURCES})
target_link_libraries(ocudu_du_config_validators sched_config rrc_nr_asn1)

add_library(ocudu_o_du STATIC
        o_du_impl.cpp
        o_du_factory.cpp)

target_link_libraries(ocudu_o_du PUBLIC ocudu_o_du_high ocudu_o_du_low)

# Hardware acceleration for both PUSCH and PDSCH is enabled by default when using DPDK.
if (DPDK_FOUND)
   set_source_files_properties(o_du_factory.cpp o_du_impl.cpp PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND; HWACC_PDSCH_ENABLED; HWACC_PUSCH_ENABLED")
   list(APPEND DU_LOW_LIBRARIES ocudu_hal_pusch
           ocudu_hal_pdsch
           ocudu_hal_bbdev)
endif (DPDK_FOUND)
