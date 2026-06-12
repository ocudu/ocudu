// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/carrier_configuration.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/pdcch/coreset.h"
#include "ocudu/ran/ssb/ssb_configuration.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include "ocudu/scheduler/scheduler_sys_info_handler.h"

namespace ocudu {

/// Type that can hold multiple versions of the payload for segmented messages.
using bcch_dl_sch_payload_type = std::vector<byte_buffer>;

/// System Information signalled by the cell.
struct mac_cell_sys_info_config {
  /// SIB1 payload.
  byte_buffer sib1;
  /// SI messages provided by the cell and which are part of the SIB1 SI-SchedConfig.
  static_vector<bcch_dl_sch_payload_type, MAX_SI_MESSAGES> si_messages;
  /// SI scheduling configuration to provide to MAC scheduler.
  si_scheduling_update_request si_sched_cfg;
  /// Whether HyperSFN is stored in SIB1 and needs to be updated periodically.
  bool sib1_contains_hypersfn = false;
};

/// Request to create Cell in MAC and Scheduler.
struct mac_cell_creation_request {
  du_cell_index_t cell_index;
  pci_t           pci;

  /// MIB cellBarred field (TS 38.331): true=barred, false=notBarred.
  bool cell_barred = false;
  /// MIB intraFreqReselection field (TS 38.331): true=allowed, false=notAllowed.
  bool intra_freq_reselection = true;

  /// subcarrierSpacing for common, used for initial access and broadcast message.
  subcarrier_spacing scs_common;

  carrier_configuration dl_carrier;
  carrier_configuration ul_carrier;
  ssb_configuration     ssb_cfg;
  coreset0_index        cs0_index;
  search_space0_index   ss0_index;

  /// Information used by MAC to dimension resources for the given cell.
  unsigned max_harq_buffers = MAX_NOF_HARQS * MAX_NOF_DU_UES_PER_CELL;

  /// Embedded scheduler cell configuration request.
  sched_cell_configuration_request_message sched_req;

  /// Cell-specific encoded system information.
  mac_cell_sys_info_config sys_info;
};

} // namespace ocudu
