// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "common/e2ap_asn1_packer.h"
#include "lib/e2/e2sm/e2sm_kpm/e2sm_kpm_asn1_packer.h"
#include "ocudu/gateways/sctp_network_server_factory.h"
#include "ocudu/pcap/dlt_pcap.h"
#include "ocudu/support/error_handling.h"

namespace ocudu {

class dlt_pcap;
class io_broker;

struct ric_sctp_gateway_config {
  /// SCTP configuration.
  sctp_network_gateway_config sctp;
  /// IO broker responsible for handling SCTP Rx data and notifications.
  io_broker& broker;
  /// Execution context used to process received SCTP packets.
  task_executor& io_rx_executor;
  /// CU-CP control executor to process SCTP notifications deffered back from io_broker executor.
  task_executor& ctrl_exec;
  /// PCAP writer for the E2AP messages.
  dlt_pcap& pcap;
  /// Sniffer that receives a copy of a received E2 message.
  e2_message_notifier* rx_sniffer;
};

class ric_e2_handler
{
public:
  virtual ~ric_e2_handler() = default;
  virtual std::unique_ptr<e2_message_notifier>
  handle_new_du_connection(std::unique_ptr<e2_message_notifier> e2_tx_pdu_notifier) = 0;
};

/// Connection server responsible for handling new E2 connection.
class e2_connection_server
{
public:
  virtual ~e2_connection_server() = default;

  /// Attach a RIC handler to the E2 connection server.
  virtual void attach_ric(ric_e2_handler& ric_) = 0;

  /// Stop the E2 connection server.
  virtual void stop() = 0;

  /// Get port on which the E2 Server is listening for new connections.
  ///
  /// This method is useful in testing, where we don't want to use a specific port.
  /// \return The port number on which the E2 Server is listening for new connections.
  virtual std::optional<uint16_t> get_listen_port() const = 0;
};

/// Creates a RIC Gateway server that listens for incoming SCTP connections, packs/unpacks E2AP PDUs and forwards
/// them to the GW/RIC E2AP handler.
std::unique_ptr<e2_connection_server> create_e2_gateway_server(const ric_sctp_gateway_config& params);

struct e2_agent_repository {
  std::map<unsigned, std::unique_ptr<e2_message_notifier>> db;
};

class e2_agent_connection_manager : public ric_e2_handler
{
public:
  e2_agent_connection_manager(e2_agent_repository& e2_agents_);

  /// Handle a new connection from E2 Agent.
  std::unique_ptr<e2_message_notifier>
  handle_new_du_connection(std::unique_ptr<e2_message_notifier> e2_tx_pdu_notifier) override;

private:
  class e2_gw_to_ric_pdu_adapter;
  unsigned get_next_e2_agent_index();

  ocudulog::basic_logger& logger;
  e2_agent_repository&    e2_agents;
};

class near_rt_ric
{
public:
  near_rt_ric();

  ric_e2_handler& get_ric_e2_handler() { return e2_agent_mng; }

  /// RIC sends msg to E2 Agent
  void send_msg(unsigned e2_agent_idx, const e2_message& msg);

private:
  ocudulog::basic_logger&     logger;
  e2_agent_connection_manager e2_agent_mng;
  e2_agent_repository         e2_agents;
};

} // namespace ocudu
