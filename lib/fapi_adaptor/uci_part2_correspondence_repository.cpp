// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi_adaptor/uci_part2_correspondence_repository.h"

using namespace ocudu;
using namespace fapi_adaptor;

span<const uint16_t> uci_part2_correspondence_repository::get_uci_part2_correspondence(unsigned index) const
{
  ocudu_assert(index < repo_map.size(),
               "Invalid UCI Part2 correspondence index, index value is '{}' while repository size has '{}' entries",
               index,
               repo_map.size());

  return repo_map[index];
}
