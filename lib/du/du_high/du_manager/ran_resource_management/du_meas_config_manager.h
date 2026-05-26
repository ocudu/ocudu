// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_ue_resource_config.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/span.h"
#include "ocudu/du/du_cell_config.h"
#include "ocudu/ocudulog/logger.h"

namespace asn1::rrc_nr {

struct ssb_mtc_s;

} // namespace asn1::rrc_nr

namespace ocudu::odu {

/// Single UL periodic resource occasion (SR or periodic CSI). Period and offset in PCell slots.
struct periodic_uci_config {
  unsigned period_slots;
  unsigned offset_slots;
};

/// Creates a measurement gap configuration based on the PCell SCS, the target SSB MTC, and any periodic UL occasions
/// (SR, periodic CSI) the gap should try to avoid.
///
/// The search prefers a strict no-collision config, falls back to a loose one that checks that at least every other
/// periodic UL occasion does not collide with measurement gap after MGRP is increased (doubled).
/// As a last resort returns a best-effort gap at the maximum MGRP with the SMTC offset.
meas_gap_config create_meas_gap(subcarrier_spacing              scs,
                                const asn1::rrc_nr::ssb_mtc_s&  smtc1,
                                span<const periodic_uci_config> ul_occasions);

/// Class that handles the measConfig of the UE.
///
/// In particular, this class determines the measGaps that the need to be set for the UE based on the measConfig
/// provided by the CU-CP for the UE.
class du_meas_config_manager
{
public:
  du_meas_config_manager(span<const du_cell_config> cell_cfg_list);

  /// Update UE config based on UE measConfig given by the CU-CP.
  void update(du_ue_resource_config& ue_cfg, const byte_buffer& meas_cfg);

private:
  span<const du_cell_config> cell_cfg_list;
  ocudulog::basic_logger&    logger;
};

} // namespace ocudu::odu
