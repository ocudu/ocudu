// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_ue_resource_config.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/span.h"
#include "ocudu/du/du_cell_config.h"
#include "ocudu/ocudulog/logger.h"
#include <chrono>

namespace asn1::rrc_nr {

struct ssb_mtc_s;

} // namespace asn1::rrc_nr

namespace ocudu {

struct ue_capability_summary;

namespace odu {

/// Single UL periodic resource occasion (SR or periodic CSI). Period and offset in PCell slots.
struct periodic_uci_config {
  unsigned period_slots;
  unsigned offset_slots;
};

/// Creates a measurement gap configuration based on the PCell SCS, the target SSB MTC, the gap patterns supported by
/// the UE, and any periodic UL occasions (SR, periodic CSI) the gap should try to avoid.
///
/// Only (MGL, MGRP) combinations that correspond to a gap pattern supported by the UE (TS 38.306 supportedGapPattern,
/// TS 38.133 Table 9.1.2-1) are considered. The search prefers the shortest MGL that still encloses the SMTC window and
/// a strict no-collision config, falling back to a loose one that checks that at least every other periodic UL occasion
/// does not collide with the measurement gap after MGRP is increased (doubled). As a last resort it returns a
/// best-effort gap at the largest supported MGRP aligned with the SMTC offset. When \c supported_patterns is left
/// default-constructed, only the mandatory gap patterns 0 and 1 are assumed to be supported.
/// \param ul_ta Uplink timing advance T_TA applied by the UEs of the cell, used to place the periodic UL occasions
/// on the timing grid the gap is anchored to (TS 38.211, Section 4.3.1). Absent for terrestrial cells.
///
/// \remark Known limitation: the offset is chosen for the \c ul_ta of the moment and never revisited, while in an
/// NTN cell T_TA sweeps several slots over a pass. A UL occasion left clear here can therefore drift into the gap
/// later in the pass. Only occasion availability suffers, never correctness, as \ref is_inside_ul_meas_gap
/// re-evaluates the window against the current T_TA on every scheduling decision.
meas_gap_config create_meas_gap(subcarrier_spacing                       scs,
                                const asn1::rrc_nr::ssb_mtc_s&           smtc1,
                                span<const periodic_uci_config>          ul_occasions,
                                std::optional<std::chrono::microseconds> ul_ta,
                                const supported_meas_gap_patterns&       supported_patterns = {});

/// Class that handles the measConfig of the UE.
///
/// In particular, this class determines the measGaps that the need to be set for the UE based on the measConfig
/// provided by the CU-CP for the UE.
class du_meas_config_manager
{
public:
  du_meas_config_manager(span<const du_cell_config> cell_cfg_list);

  /// Update UE config based on UE measConfig given by the CU-CP.
  ///
  /// \param ue_cfg UE resource configuration to update with the computed measurement gap.
  /// \param meas_cfg Packed measConfig provided by the CU-CP.
  /// \param ue_caps Decoded UE capabilities, used to extract the UE supported gap patterns.
  void update(du_ue_resource_config& ue_cfg, const byte_buffer& meas_cfg, const ue_capability_summary* ue_caps);

  /// Update UE config based on the UE LocationMeasurementInfo given by the CU-CP.
  ///
  /// \param ue_cfg UE resource configuration to update with the measurement gap of the PRS.
  /// \param location_meas_info Packed LocationMeasurementInfo provided by the CU-CP.
  /// \return false if no measurement gap can enclose the PRS of the location measurements.
  bool update_location_meas(du_ue_resource_config& ue_cfg, const byte_buffer& location_meas_info);

private:
  span<const du_cell_config> cell_cfg_list;
  ocudulog::basic_logger&    logger;
};

} // namespace odu
} // namespace ocudu
