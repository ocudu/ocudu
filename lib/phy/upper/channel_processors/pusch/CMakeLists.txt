# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ocudu_pusch_demodulator STATIC
        pusch_demodulator_impl.cpp
        demodulator_factories.cpp)

add_library(ocudu_pusch_processor STATIC
        processor_factories.cpp
        pusch_codeblock_decoder.cpp
        pusch_decoder_empty_impl.cpp
        pusch_decoder_hw_impl.cpp
        pusch_decoder_impl.cpp
        pusch_processor_impl.cpp
        pusch_processor_validator_impl.cpp
        ulsch_demultiplex_impl.cpp)
set(OCUDU_PUSCH_LIBS ocudu_upper_phy_support ocudu_ran ocudu_pusch_demodulator)

target_link_libraries(ocudu_pusch_processor ${OCUDU_PUSCH_LIBS})
