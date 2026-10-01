# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(HWACC_FACTORIES_DEFINITIONS "")

if (ENABLE_PUSCH_HWACC)
    list(APPEND HWACC_FACTORIES_DEFINITIONS ENABLE_PUSCH_HWACC)
endif ()

set(SOURCES 
  hw_accelerator_factories.cpp
  hw_accelerator_pusch_dec_impl.cpp
  hw_accelerator_pusch_dec_acc100_impl.cpp
  ext_harq_buffer_context_repository_factory.cpp
)

add_library(ocudu_hal_pusch STATIC ${SOURCES})

set_source_files_properties(hw_accelerator_factories.cpp PROPERTIES COMPILE_DEFINITIONS "${HWACC_FACTORIES_DEFINITIONS}")

target_link_libraries(ocudu_hal_pusch ocudu_hal_bbdev hal_dpdk ${DPDK_LIBRARIES} dl)
