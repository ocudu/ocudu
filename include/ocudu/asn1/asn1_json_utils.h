// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "nlohmann/json.hpp"
#include "ocudu/asn1/asn1_utils.h"

namespace asn1 {

inline void to_json(nlohmann::json& out_j, const real_s& obj)
{
  out_j = obj.value;
}

inline void from_json(const nlohmann::json& in_j, real_s& obj)
{
  in_j.get_to(obj.value);
}

inline void to_json(nlohmann::ordered_json& out_j, const real_s& obj)
{
  out_j = obj.value;
}

inline void from_json(const nlohmann::ordered_json& in_j, real_s& obj)
{
  in_j.get_to(obj.value);
}

} // namespace asn1
