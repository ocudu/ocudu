// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ran/csi_rs/csi_meas_config.h"
#include "ocudu/ran/csi_rs/csi_report_config.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/pdsch/pdsch_mcs.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"
#include <array>
#include <optional>

namespace ocudu {

/// Configurable CSI parameters in a given DU cell.
struct du_csi_params {
  /// Symbol index within the slot assigned to CSI-RS for channel measurement.
  unsigned cm_csi_ofdm_symbol_index = 4;
  /// Symbol index within the slot assigned to ZP-CSI-RS.
  unsigned zp_csi_ofdm_symbol_index = 8;
  /// \brief Symbol indices within the slot assigned to CSI-RS for tracking resources.
  ///
  /// The possible sets of values are given in TS38.214 Section 5.1.6.1.1. It must be set to {4, 8, 4, 8}, {5, 9, 5, 9}
  /// or {6, 10, 6, 10}.
  std::array<unsigned, 4> tracking_csi_ofdm_symbol_indices = {4, 8, 4, 8};
  /// \brief Period of the CSI-RS resources.
  ///
  /// The UE expects a period of \f$2^\mu\{10, 20, 40, 80\}\f$ slots.
  csi_resource_periodicity csi_rs_period = csi_resource_periodicity::slots80;
  /// Slot offset for measurement CSI-RS resources. Note: Should avoid collisions with SSB and SIB1.
  unsigned meas_csi_slot_offset = 2;
  /// Slot offset of the first CSI-RS resource used for tracking. Note: Should avoid collisions with SSB and SIB1.
  unsigned tracking_csi_slot_offset = 12;
  /// Slot offset for zp-CSI-RS resources. Note: Should avoid collisions with SSB and SIB1.
  unsigned zp_csi_slot_offset = 2;
  /// Report slot offset for periodic CSI reports.
  std::optional<unsigned> csi_report_slot_offset = 9;
  /// Whether to configure aperiodic CSI reports.
  bool enable_aperiodic_report = false;
  /// Power offset of PDSCH RE to NZP CSI-RS RE. Value in dB {-8,...,15}.
  int8_t pwr_ctrl_offset = 0;
};

namespace csi_helper {

/// Extension of \c du_csi_params with other cell parameters involved in the CSI Meas Config generation.
struct csi_meas_config_builder_params {
  /// PCI of the cell that will determine the scrambling.
  pci_t pci;
  /// \brief Number of RBs used for the CSI-RS. The csi config generators will find the closest number of RBs to
  /// the one provided that is a multiple of 4 as required by the TS 38.331, "CSI-FrequencyOccupation".
  unsigned nof_rbs;
  /// Number of ports set for the CSI-RS.
  unsigned nof_ports = 1;
  /// Maximum number of DL layers.
  unsigned max_nof_layers = 1;
  /// PDSCH MCS table used to select the CSI CQI table.
  pdsch_mcs_table mcs_table = pdsch_mcs_table::qam64;
  /// DU-level CSI parameters.
  du_csi_params csi_params;
};

/// \brief Compute default CSI-RS signalling period to use, while constrained by TS38.214, 5.1.6.1.1.
csi_resource_periodicity get_max_csi_rs_period(subcarrier_spacing pdsch_scs);

/// \brief Checks whether a specified CSI-RS period is valid for a given TDD pattern.
///
/// The CSI-RS period is valid if:
/// - it is multiple of the TDD period; and
/// - it is one that the UE expects it for the CSI-RS for tracking.
[[nodiscard]] bool is_csi_rs_period_valid(csi_resource_periodicity       csi_rs_period,
                                          const tdd_ul_dl_config_common& tdd_cfg);

/// Checks whether the given SR and CSI periods+offsets will result in SR and CSI being scheduled together in some slot.
[[nodiscard]] bool are_sr_and_csi_pucchs_scheduled_together(unsigned sr_period,
                                                            unsigned sr_offset,
                                                            unsigned csi_period,
                                                            unsigned csi_offset);

/// \brief Searches for a valid CSI-RS periodicity, while constrained by TDD pattern periodicity.
std::optional<csi_resource_periodicity> find_valid_csi_rs_period(const tdd_ul_dl_config_common& tdd_cfg);

/// \brief Search for valid CSI-RS slot offsets for measurement, tracking and interference management.
///
/// \param csi_params [in/out] Parameters used to generate CSI Meas Config. This function assumes that the CSI-RS
/// period, is already set.
/// \param meas_csi_slot_offset [in] Slot offset for measurement CSI-RS resources. If passed as empty, a new value is
/// derived. If passed as non-empty, the function will check whether the value is valid.
/// \param tracking_csi_slot_offset [in] Slot offset for tracking CSI-RS resources. If passed as empty, a new value is
/// derived. If passed as non-empty, the function will check whether the value is valid.
/// \param zp_csi_slot_offset [in] Slot offset for IM CSI-RS resources. If passed as empty, a new value is derived.
/// If passed as non-empty, the function will check whether the value is valid.
/// \param tdd_cfg [in] TDD pattern.
/// \param max_csi_symbol_index [in] Maximum CSI symbol among those used for CSI-RS.
/// \param ssb_period_ms [in] SSB period in ms.
/// \param ssb_slot_offsets [in] Slot offsets (within one SSB period) that carry SSB transmissions. All listed slots
/// are excluded from CSI-RS placement.
[[nodiscard]] bool derive_valid_csi_rs_slot_offsets(du_csi_params&                 csi_params,
                                                    const std::optional<unsigned>& meas_csi_slot_offset,
                                                    const std::optional<unsigned>& tracking_csi_slot_offset,
                                                    const std::optional<unsigned>& zp_csi_slot_offset,
                                                    const tdd_ul_dl_config_common& tdd_cfg,
                                                    unsigned                       max_csi_symbol_index,
                                                    unsigned                       ssb_period_ms,
                                                    span<const unsigned>           ssb_slot_offsets);

/// \brief Generate list of zp-CSI-RS Resources.
std::vector<zp_csi_rs_resource> make_periodic_zp_csi_rs_resource_list(const csi_meas_config_builder_params& params);

/// \brief Generate set of periodic zp-CSI-RS Resources.
zp_csi_rs_resource_set make_periodic_zp_csi_rs_resource_set(const csi_meas_config_builder_params& params);

/// \brief Generate wideband NZP-CSI-RS Resource List, composed by nzp-CSI-RS resources for tracking and channel
/// measurements.
std::vector<nzp_csi_rs_resource> make_nzp_csi_rs_resource_list(const csi_meas_config_builder_params& params);

/// \brief Generate CSI-MeasConfig.
csi_meas_config make_csi_meas_config(const csi_meas_config_builder_params&                     params,
                                     const std::vector<pusch_time_domain_resource_allocation>& pusch_td_alloc_list);

} // namespace csi_helper
} // namespace ocudu
