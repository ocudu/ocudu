# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(cu_cp)
add_subdirectory(du)
add_subdirectory(gateways)

add_library(ocudu_f1ap_common f1ap_common_messages.cpp asn1_helpers.cpp f1ap_asn1_packer.cpp log_helpers.cpp)
target_link_libraries(ocudu_f1ap_common f1ap_asn1 ocudu_ran)
