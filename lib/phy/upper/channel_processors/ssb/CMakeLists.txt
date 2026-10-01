# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ocudu_ssb_tx_processor STATIC
        factories.cpp
        pbch_encoder_impl.cpp
        pbch_modulator_impl.cpp
        ssb_processor_impl.cpp)
target_link_libraries(ocudu_ssb_tx_processor
        ocudu_sequence_generators
        ocudu_signal_processors)