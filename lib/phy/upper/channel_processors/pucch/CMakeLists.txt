# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ocudu_pucch_processor STATIC factories.cpp pucch_processor_impl.cpp)

add_library(ocudu_pucch_detector STATIC
        factories.cpp
        pucch_detector_format0.cpp
        pucch_detector_format1.cpp)

add_library(ocudu_pucch_demodulator STATIC
        factories.cpp
        pucch_demodulator_format2.cpp
        pucch_demodulator_format3.cpp
        pucch_demodulator_format4.cpp)
