# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(pusch)

set(HWACC_FACTORIES_DEFINITIONS "")

if (ENABLE_PDSCH_HWACC)
    list(APPEND HWACC_FACTORIES_DEFINITIONS ENABLE_PDSCH_HWACC)
endif ()

set(SOURCES 
  hw_accelerator_factories.cpp
  hw_accelerator_pdsch_enc_impl.cpp
  hw_accelerator_pdsch_enc_acc100_impl.cpp
)

add_library(ocudu_hal_pdsch STATIC ${SOURCES})

set_source_files_properties(hw_accelerator_factories.cpp PROPERTIES COMPILE_DEFINITIONS "${HWACC_FACTORIES_DEFINITIONS}")

target_link_libraries(ocudu_hal_pdsch ocudu_hal_bbdev hal_dpdk ${DPDK_LIBRARIES} dl)
