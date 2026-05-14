// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/slotted_vector.h"
#include "ocudu/mac/mac_cell_rach_handler.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/rnti.h"

namespace ocudu {

class scheduler_rach_handler;
class rnti_manager;
class mac_rach_handler;
struct sched_cell_configuration_request_message;

/// Handler of RACH indications for a given cell in the MAC.
class mac_cell_rach_handler_impl final : public mac_cell_rach_handler
{
public:
  mac_cell_rach_handler_impl(mac_rach_handler& parent_, const sched_cell_configuration_request_message& sched_cfg_);

  /// Handle detected RACH indication.
  void handle_rach_indication(const mac_rach_indication& rach_ind) override;

  /// Reserve Contention-free RACH preamble for a given UE.
  [[nodiscard]] bool handle_cfra_allocation(uint8_t preamble_idx, du_ue_index_t ue_idx, rnti_t crnti);

  /// Deallocate Contention-free RACH preamble that was previously given to a UE.
  void handle_cfra_deallocation(du_ue_index_t ue_idx);

private:
  // Map of preamble ID to entry in \c preambles vector.
  unsigned get_cfra_index(unsigned ra_preamble_id) const;

  mac_rach_handler&     parent;
  const du_cell_index_t cell_index;
  interval<unsigned>    cfra_preambles;

  std::vector<std::atomic<rnti_t>> preambles;
};

/// Handler of RACH indications for multiple cells.
class mac_rach_handler
{
public:
  mac_rach_handler(scheduler_rach_handler& sched_, rnti_manager& rnti_mng_, ocudulog::basic_logger& logger_);

  /// Create new handler of RACH indications for a cell.
  mac_cell_rach_handler_impl& add_cell(const sched_cell_configuration_request_message& sched_cfg);

  void rem_cell(du_cell_index_t cell_index);

private:
  friend class mac_cell_rach_handler_impl;

  struct cfra_ue_context {
    du_cell_index_t cell_idx;
    uint8_t         preamble_id;
  };

  scheduler_rach_handler& sched;
  rnti_manager&           rnti_mng;
  ocudulog::basic_logger& logger;

  slotted_id_vector<du_cell_index_t, std::unique_ptr<mac_cell_rach_handler_impl>> cell_map;
  std::vector<cfra_ue_context>                                                    ue_map;
};

} // namespace ocudu
