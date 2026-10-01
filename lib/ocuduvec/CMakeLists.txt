# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set(SOURCES
        accumulate.cpp
        add.cpp
        bit.cpp
        clip.cpp
        compare.cpp
        conversion.cpp
        convolution.cpp
        division.cpp
        dot_prod.cpp
        modulus_square.cpp
        prod.cpp
        sc_prod.cpp
        subtract.cpp
        unwrap.cpp
)

add_library(ocuduvec STATIC ${SOURCES})
target_link_libraries(ocuduvec ocudulog)

add_to_exported_libs(ocuduvec)
