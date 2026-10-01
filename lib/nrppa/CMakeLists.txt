# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
        nrppa_factory.cpp
        nrppa_helper.cpp
        nrppa_impl.cpp
        du_context/nrppa_du_context.cpp
        meas_context/nrppa_meas_context.cpp
        procedures/e_cid_measurement_initiation_procedure.cpp
        procedures/e_cid_measurement_termination_procedure.cpp
        procedures/measurement_procedure.cpp
        procedures/trp_information_exchange_procedure.cpp
        procedures/positioning_information_exchange_procedure.cpp
        procedures/positioning_activation_procedure.cpp
        ue_context/nrppa_ue_context.cpp
        )

add_library(ocudu_nrppa STATIC ${SOURCES})

target_link_libraries(ocudu_nrppa
	ocudu_cu_cp
	nrppa_asn1
	ocudu_support)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
