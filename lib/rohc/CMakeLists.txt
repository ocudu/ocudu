# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set(SOURCES
  rohc_factory.cpp
  rohc_support.cpp
)

add_library(ocudu_rohc STATIC ${SOURCES})

set(OCUDU_ROHC_DEFINITIONS "")
set(OCUDU_ROHC_LIBRARIES "")

if(ROHC_FOUND)
  add_subdirectory(rohc_lib)
  list(APPEND OCUDU_ROHC_DEFINITIONS -DENABLE_ROHC_LIB)
  list(APPEND OCUDU_ROHC_LIBRARIES ocudu_rohc_lib)
endif()

add_definitions(${OCUDU_ROHC_DEFINITIONS})

target_link_libraries(ocudu_rohc ${OCUDU_ROHC_LIBRARIES})
