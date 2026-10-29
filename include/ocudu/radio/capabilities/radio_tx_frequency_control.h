// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {

/// Radio capability that allows modifying the transmission center frequency in runtime.
class radio_tx_frequency_control
{
public:
  virtual ~radio_tx_frequency_control() = default;

  /// \brief Set the transmission center frequency of all ports related to a stream.
  /// \param[in] stream_id      Radio stream identifier.
  /// \param[in] center_freq_Hz New center frequency in Hertz.
  /// \return True if the operation is successful. Otherwise, false.
  virtual bool set_tx_freq(unsigned stream_id, double center_freq_Hz) = 0;
};

} // namespace ocudu
