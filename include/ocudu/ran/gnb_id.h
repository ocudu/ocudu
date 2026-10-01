// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

struct gnb_id_t {
  uint32_t id;         ///< gNodeB identifier.
  uint8_t  bit_length; ///< Length of gNB identity in bits. Values {22,...,32}.

  bool operator==(const gnb_id_t& rhs) const { return id == rhs.id && bit_length == rhs.bit_length; }
  bool operator!=(const gnb_id_t& rhs) const { return id != rhs.id || bit_length != rhs.bit_length; }
};

} // namespace ocudu
