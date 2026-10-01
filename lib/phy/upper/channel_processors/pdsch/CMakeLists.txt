# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ocudu_pdsch_processor STATIC
        factories.cpp
        pdsch_block_processor_impl.cpp
        pdsch_block_processor_hw_impl.cpp
        pdsch_encoder_hw_impl.cpp
        pdsch_encoder_impl.cpp
        pdsch_modulator_impl.cpp
        pdsch_processor_flexible_impl.cpp
        pdsch_processor_impl.cpp
        pdsch_processor_validator_impl.cpp)

target_link_libraries(ocudu_pdsch_processor
        ocudu_channel_modulation
        ocudu_phy_support
        ocudu_ran
        ocudu_upper_phy_support)
