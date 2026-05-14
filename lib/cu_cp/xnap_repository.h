// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "adapters/xnap_adapters.h"
#include "task_schedulers/xnap_task_scheduler.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/cu_cp/cu_cp_types.h"
#include "ocudu/support/io/transport_layer_address.h"
#include "ocudu/xnap/xnap.h"
#include "ocudu/xnap/xnap_configuration.h"
#include "ocudu/xnap/xnap_message_notifier.h"

namespace ocudu::ocucp {

struct cu_cp_configuration;

struct xnap_repository_config {
  const cu_cp_configuration& cu_cp;
  cu_cp_xnap_handler&        cu_cp_notifier;
  ocudulog::basic_logger&    logger;
};

class xnap_repository
{
public:
  explicit xnap_repository(xnap_repository_config cfg_);

  /// \brief Adds an XNAP object to the CU-CP.
  /// \param[in] xnc_index Index of the XN-C peer to which the XNAP object will be connected.
  /// \param[in] peer_addrs Addresses of the XN-C peer (multiple for SCTP multihoming).
  /// \param[in] xnap_cfg Configuration of the XNAP object to be added.
  /// \return A pointer to the interface of the added XNAP object if it was successfully created, a nullptr otherwise.
  xnap_interface* add_xnap(xnc_peer_index_t                            xnc_index,
                           const std::vector<transport_layer_address>& peer_addrs,
                           const xnap_configuration&                   xnap_cfg);

  /// \brief Find an XNAP object in the repository.
  /// \param[in] xnc_index Index of the XN-C peer to which the XNAP object is connected.
  /// \return A pointer to the interface of the added XNAP object if it was successfully found, a nullptr otherwise.
  xnap_interface* find_xnap(xnc_peer_index_t xnc_index);

  /// \brief Find an XNAP object in the repository.
  /// \param[in] peer_addr Address of the XN-C peer to which the XNAP object is connected.
  /// The lookup matches if the address reported by SCTP COMM_UP equals any of the peer's configured addresses.
  /// \return The index of the XN-C peer if the XNAP object was found, xnc_peer_index_t::invalid otherwise.
  xnc_peer_index_t find_xnap(const transport_layer_address& peer_addr);

  /// \brief Find an XNAP object in the repository.
  /// \param[in] peer_gnb_id GNB ID of the XN-C peer to which the XNAP object is connected.
  /// \return A pointer to the interface of the added XNAP object if it was successfully found, a nullptr otherwise.
  xnap_interface* find_xnap(const gnb_id_t& peer_gnb_id);

  /// \brief Get the peer addresses of an XNAP connection.
  /// \param[in] xnc_index Index of the XN-C peer in the XNAP repository.
  /// \return Addresses of the XN-C peer (multiple if SCTP multihoming is used), or std::nullopt if not found.
  std::optional<std::vector<transport_layer_address>> get_peer_addrs(xnc_peer_index_t xnc_index) const;

  /// \brief Get the all XNAP interfaces.
  std::map<xnc_peer_index_t, xnap_interface*> get_xnaps();

  xnap_task_scheduler& get_xnap_task_scheduler() { return xnc_task_sched; }

  /// Number of XNAPs managed by the CU-CP.
  size_t get_nof_xnaps() const { return xnap_db.size(); }

  void connect_association(xnc_peer_index_t idx, std::unique_ptr<xnap_message_notifier> sender_notifier);

  async_task<void> remove_xnap(xnc_peer_index_t idx);

private:
  struct xnap_context {
    // Peer addresses (multiple for SCTP multihoming).
    std::vector<transport_layer_address> peer_addrs;
    // CU-CP handler of XNAP events.
    xnap_cu_cp_adapter xnap_to_cu_cp_notifier;

    std::unique_ptr<xnap_interface> xnap;
  };

  xnap_repository_config  cfg;
  ocudulog::basic_logger& logger;

  xnap_task_scheduler xnc_task_sched;

  std::map<xnc_peer_index_t, xnap_context> xnap_db;
};

} // namespace ocudu::ocucp
