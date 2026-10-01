// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

class downlink_pdu_validator;
class downlink_processor_pool;
class resource_grid_pool;
class uplink_pdu_validator;
class uplink_request_processor;
class upper_phy_error_handler;
class upper_phy_error_notifier;
class upper_phy_metrics_collector;
class upper_phy_operation_controller;
class upper_phy_rx_results_notifier;
class upper_phy_rx_symbol_handler;
class upper_phy_timing_handler;
class upper_phy_timing_notifier;
class uplink_pdu_slot_repository_pool;

/// \brief Upper PHY interface.
///
/// This interface describes the upper PHY layer giving access to its gateways and event notifiers.
///
/// The downlink part of the upper PHY processes different PDU types such as PDCCH, PDSCH, NZI-CSI-RS and SSB PDUs and
/// then sends the resulting resource grid through the configured \c resource_grid_gateway.
///
/// The uplink part of the upper PHY notifies about the new incoming symbols, processing them to generate the uplink
/// PDUs such as PUSCH or PUCCH. It also notifies when a new slot starts.
class upper_phy
{
public:
  /// Default destructor.
  virtual ~upper_phy() = default;

  /// Returns the operation controller of this upper PHY.
  virtual upper_phy_operation_controller& get_operation_controller() = 0;

  /// Returns a reference to the receive symbol handler of this upper PHY.
  virtual upper_phy_rx_symbol_handler& get_rx_symbol_handler() = 0;

  /// Returns a reference to the timing handler of this upper PHY.
  virtual upper_phy_timing_handler& get_timing_handler() = 0;

  /// Returns a reference to the error handler of this upper PHY.
  virtual upper_phy_error_handler& get_error_handler() = 0;

  /// Returns the downlink processor pool of this upper PHY.
  virtual downlink_processor_pool& get_downlink_processor_pool() = 0;

  /// Returns the downlink resource grid pool of this upper PHY.
  virtual resource_grid_pool& get_downlink_resource_grid_pool() = 0;

  /// Returns the uplink request processor of this upper PHY.
  virtual uplink_request_processor& get_uplink_request_processor() = 0;

  /// Returns the uplink slot PDU repository of this upper PHY.
  virtual uplink_pdu_slot_repository_pool& get_uplink_pdu_slot_repository() = 0;

  /// Returns the metrics collector of this upper PHY or nullptr if upper PHY metrics are not enabled.
  virtual upper_phy_metrics_collector* get_metrics_collector() = 0;

  /// Returns the downlink PDU validator of this upper PHY.
  virtual const downlink_pdu_validator& get_downlink_pdu_validator() const = 0;

  /// Returns the uplink PDU validator of this upper PHY.
  virtual const uplink_pdu_validator& get_uplink_pdu_validator() const = 0;

  /// \brief Sets the upper PHY timing notifier for this upper PHY.
  ///
  /// \param[in] notifier Notifier assigned to this upper PHY.
  virtual void set_timing_notifier(upper_phy_timing_notifier& notifier) = 0;

  /// \brief Sets the receive result notifier for this upper PHY.
  ///
  /// \param[in] notifier Notifier assigned to this upper PHY.
  virtual void set_rx_results_notifier(upper_phy_rx_results_notifier& notifier) = 0;

  /// \brief Sets the error notifier for this upper PHY.
  ///
  /// \param[in] notifier Notifier assigned to this upper PHY.
  virtual void set_error_notifier(upper_phy_error_notifier& notifier) = 0;

  /// \brief Stops the upper PHY.
  ///
  /// It shall wait for pending tasks that have cross dependencies to finish their processing.
  virtual void stop() = 0;
};

} // namespace ocudu
