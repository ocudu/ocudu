# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        du_low_executor_mapper.cpp
        du_low_factory.cpp
        du_low_impl.cpp
        du_low_metrics_collector_impl.cpp
        o_du_low_factory.cpp
        o_du_low_impl.cpp
        o_du_low_metrics_collector_impl.cpp)

add_library(ocudu_o_du_low STATIC ${SOURCES})
set(DU_LOW_LIBRARIES ocudu_phy_fapi_fastpath_adaptor ocudu_upper_phy)

# Hardware acceleration for both PUSCH and PDSCH is enabled by default when using DPDK.
if (DPDK_FOUND)
   set_source_files_properties(${SOURCES} PROPERTIES COMPILE_DEFINITIONS "DPDK_FOUND; HWACC_PDSCH_ENABLED; HWACC_PUSCH_ENABLED")
   list(APPEND DU_LOW_LIBRARIES ocudu_hal_pusch
        ocudu_hal_pdsch
        ocudu_hal_bbdev)
endif (DPDK_FOUND)
target_link_libraries(ocudu_o_du_low PUBLIC ${DU_LOW_LIBRARIES})
