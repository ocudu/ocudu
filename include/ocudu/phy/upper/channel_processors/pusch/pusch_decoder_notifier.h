// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

struct pusch_decoder_result;

/// PUSCH decoder interface to notify the decoding of a transport block.
class pusch_decoder_notifier
{
public:
  /// Default destructor.
  virtual ~pusch_decoder_notifier() = default;

  /// \brief Notifies the completion of a PUSCH transport block decoding.
  /// \param[in] result PUSCH decoder result.
  virtual void on_sch_data(const pusch_decoder_result& result) = 0;
};

} // namespace ocudu
