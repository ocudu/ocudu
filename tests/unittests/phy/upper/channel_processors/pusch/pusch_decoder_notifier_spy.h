// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pusch/pusch_decoder_notifier.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_decoder_result.h"
#include <vector>

namespace ocudu {

class pusch_decoder_notifier_spy : public pusch_decoder_notifier
{
public:
  void on_sch_data(const pusch_decoder_result& result) override { entries.emplace_back(result); }

  const std::vector<pusch_decoder_result>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  std::vector<pusch_decoder_result> entries;
};

} // namespace ocudu
