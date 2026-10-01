# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(pdcch)
add_subdirectory(pdsch)
add_subdirectory(prach)
add_subdirectory(pucch)
add_subdirectory(pusch)
add_subdirectory(ssb)
add_subdirectory(uci)

add_library(ocudu_channel_processors INTERFACE)
target_link_libraries(ocudu_channel_processors INTERFACE
        ocudu_channel_coding
        ocudu_channel_modulation
        ocudu_channel_precoder
        ocudu_pdcch_processor
        ocudu_pdsch_processor
        ocudu_phy_support
        ocudu_prach_detector
        ocudu_pucch_demodulator
        ocudu_pucch_detector
        ocudu_pucch_processor
        ocudu_pusch_processor
        ocudu_sequence_generators
        ocudu_signal_processors
        ocudu_ssb_tx_processor
        ocudu_uci_decoder)

add_to_exported_libs(ocudu_channel_processors
        ocudu_pdcch_processor
        ocudu_pdsch_processor
        ocudu_prach_detector
        ocudu_pucch_demodulator
        ocudu_pucch_detector
        ocudu_pucch_processor
        ocudu_pusch_demodulator
        ocudu_pusch_processor
        ocudu_ssb_tx_processor
        ocudu_uci_decoder)
