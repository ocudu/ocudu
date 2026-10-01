# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
            du_low_config_cli11_schema.cpp
            du_low_config_translator.cpp
            du_low_config_validator.cpp
            du_low_config_yaml_writer.cpp
            du_low_hal_factory.cpp
            o_du_low_unit_factory.cpp)

add_library(ocudu_o_du_low_unit_helpers STATIC ${SOURCES})
target_include_directories(ocudu_o_du_low_unit_helpers PRIVATE ${CMAKE_SOURCE_DIR})
set(DU_LOW_UNIT_HELPERS_LIBRARIES ocudu_upper_phy ocudu_cpu_affinities_helper ocudu_o_du_low ocudu_app_helpers)

# Hardware acceleration for both PUSCH and PDSCH is enabled by default when using DPDK.
if (DPDK_FOUND)
   set_source_files_properties(
           du_low_config_cli11_schema.cpp
           du_low_hal_factory.cpp
           PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND; HWACC_PDSCH_ENABLED; HWACC_PUSCH_ENABLED")
   list(APPEND DU_LOW_UNIT_HELPERS_LIBRARIES ocudu_hal_pusch
        ocudu_hal_pdsch
        ocudu_hal_bbdev)
endif (DPDK_FOUND)

target_link_libraries(ocudu_o_du_low_unit_helpers PUBLIC ${DU_LOW_UNIT_HELPERS_LIBRARIES})
