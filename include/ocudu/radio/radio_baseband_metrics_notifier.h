// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

namespace ocudu {

struct radio_baseband_metrics;

/// Radio interface used to notify metrics.
class radio_baseband_metrics_notifier
{
public:
  /// Default destructor.
  virtual ~radio_baseband_metrics_notifier() = default;

  /// \brief Notifies a new transmit radio call measurement.
  ///
  /// \param[in] metrics Measurements of the transmitted radio call.
  virtual void on_new_transmit_metrics(const radio_baseband_metrics& metrics) = 0;

  /// \brief Notifies a new receive radio call measurement.
  ///
  /// \param[in] metrics Measurements of the received radio call.
  virtual void on_new_receive_metrics(const radio_baseband_metrics& metrics) = 0;
};

} // namespace ocudu
