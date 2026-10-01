# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(ocudu_e1ap_common
        e1ap_asn1_packer.cpp
        e1ap_common_messages.cpp
        log_helpers.cpp
        )
target_link_libraries(ocudu_e1ap_common e1ap_asn1)
