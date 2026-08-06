// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "radio_difi_baseband_gateway.h"
#include "ocudu/radio/radio_session.h"
#include "ocudu/support/executors/task_executor.h"
#include <memory>
#include <vector>

namespace ocudu {

/// Describes a DIFI radio session.
class radio_session_difi_impl : public radio_session, public radio_management_plane
{
  /// Radio session logger.
  ocudulog::basic_logger& logger;
  /// One baseband gateway per configured stream.
  std::vector<std::unique_ptr<radio_difi_baseband_gateway>> bb_gateways;
  /// Set to true when the session has been constructed without errors.
  bool successful = false;

public:
  /// \brief Constructs a DIFI radio session; call is_successful() before use.
  ///
  /// \p notifier receives runtime events, and \p async_task_executor processes them.
  radio_session_difi_impl(const radio_configuration::radio& config,
                          task_executor&                    async_task_executor,
                          radio_event_notifier&             notifier);

  /// Default destructor — stops all streams before teardown.
  ~radio_session_difi_impl() override = default;

  /// Returns true if the session was successfully constructed.
  bool is_successful() const { return successful; }

  // See radio_session for documentation.
  radio_management_plane& get_management_plane() override { return *this; }

  // See radio_session for documentation.
  baseband_gateway& get_baseband_gateway(unsigned stream_id) override;

  // See radio_session for documentation.
  baseband_gateway_timestamp read_current_time() override;

  // See radio_session for documentation.
  void start(baseband_gateway_timestamp init_time) override;

  // See radio_session for documentation.
  void stop() override;

  // See radio_management_plane for documentation.
  bool set_tx_gain(unsigned port_id, double gain_dB) override;

  // See radio_management_plane for documentation.
  bool set_rx_gain(unsigned port_id, double gain_dB) override;

  // See radio_management_plane for documentation.
  bool set_tx_freq(unsigned stream_id, double center_freq_Hz) override;

  // See radio_management_plane for documentation.
  bool set_rx_freq(unsigned stream_id, double center_freq_Hz) override;
};

} // namespace ocudu
