// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/e2sm/e2sm_kpm_ies.h"
#include "ocudu/e2/e2sm/e2sm_kpm.h"

namespace ocudu {

std::string         e2sm_kpm_label_2_str(e2sm_kpm_label_enum label);
std::string         e2sm_kpm_scope_2_str(e2sm_kpm_metric_level_enum level);
e2sm_kpm_label_enum asn1_label_2_enum(const asn1::e2sm::meas_label_s& meas_label);

// comparison operators
bool operator==(const asn1::e2sm::ue_id_c& lhs, const asn1::e2sm::ue_id_c& rhs);
bool operator!=(const asn1::e2sm::ue_id_c& lhs, const asn1::e2sm::ue_id_c& rhs);
bool operator<(const asn1::e2sm::ue_id_c& lhs, const asn1::e2sm::ue_id_c& rhs);
bool operator==(const asn1::e2sm::ue_id_gnb_du_s& lhs, const asn1::e2sm::ue_id_gnb_du_s& rhs);
bool operator<(const asn1::e2sm::ue_id_gnb_du_s& lhs, const asn1::e2sm::ue_id_gnb_du_s& rhs);

} // namespace ocudu
