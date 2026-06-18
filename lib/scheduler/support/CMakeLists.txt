# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(sched_support STATIC
        pdcch/pdcch_type0_helpers.cpp
        pdsch/pdsch_default_time_allocation.cpp
        pdsch/pdsch_dmrs_symbol_mask.cpp
        pdsch/pdsch_config_params.cpp
        pucch/pucch_collision.cpp
        pusch/pusch_default_time_allocation.cpp
        pusch/pusch_dmrs_symbol_mask.cpp
        paging_helpers.cpp
        mcs_tbs_calculator.cpp
        prbs_calculator.cpp
        mcs_calculator.cpp
        pdcch_aggregation_level_calculator.cpp
        sr_helper.cpp
        dci_builder.cpp
        sch_pdu_builder.cpp
        csi_report_helpers.cpp
        pusch_power_controller.cpp
        pucch_power_controller.cpp
        pucch/pucch_k1_helper.cpp)
