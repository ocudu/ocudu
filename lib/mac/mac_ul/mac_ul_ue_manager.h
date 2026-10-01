// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../rnti_value_table.h"
#include "ocudu/adt/slotted_vector.h"
#include "ocudu/mac/mac_sdu_handler.h"
#include "ocudu/mac/mac_ue_configurator.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/du_ue_list.h"

namespace ocudu {

using du_rnti_table = rnti_value_table<du_ue_index_t, du_ue_index_t::MAX_NOF_DU_UES>;

/// Stores MAC UL UE context. In particular, the UL PDU notifiers for each logical channel.
class mac_ul_ue_context
{
public:
  explicit mac_ul_ue_context(du_ue_index_t ue_index_, rnti_t rnti_) : ue_index(ue_index_), rnti(rnti_) {}

  const du_ue_index_t ue_index = MAX_NOF_DU_UES;
  const rnti_t        rnti     = rnti_t::INVALID_RNTI;

  /// Flag to indicate that a UE RRC configuration is pending.
  bool rrc_config_pending = false;

  /// List of UL PDU notification endpoints associated to UE's logical channels.
  slotted_vector<mac_sdu_rx_notifier*> ul_bearers;
};

/// Class that manages the creation/reconfiguration/deletion of UEs from the MAC UL
/// Note: The access to this class is thread-safe as long as the same UE index and RNTI % MAX_NOF_UES is not accessed
///       from different threads. This is in general assured by the DU manager.
class mac_ul_ue_manager
{
public:
  mac_ul_ue_manager(du_rnti_table& rnti_table_);

  /// Checks whether RNTI exists.
  bool contains_rnti(rnti_t rnti) const { return ue_db.contains(rnti_table[rnti]); }

  /// Adds UE in MAC UL UE repository
  bool add_ue(const mac_ue_create_request& request);

  /// Add bearers to existing UE.
  bool addmod_bearers(du_ue_index_t ue_index, const std::vector<mac_logical_channel_config>& ul_logical_channels);

  /// Remove bearers based on LCID.
  bool remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids);

  void remove_ue(du_ue_index_t ue_index);

  mac_ul_ue_context* find_ue(du_ue_index_t ue_index)
  {
    if (ue_db.contains(ue_index)) {
      return &ue_db[ue_index];
    }
    return nullptr;
  }

  void handle_ue_config_applied(du_ue_index_t ue_index);

private:
  /// Arguments of UE manager.
  ocudulog::basic_logger& logger;
  du_rnti_table&          rnti_table;

  /// MAC UL UE repository.
  slotted_array<mac_ul_ue_context, MAX_NOF_DU_UES, false> ue_db;
};

} // namespace ocudu
