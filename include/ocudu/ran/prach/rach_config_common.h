// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_integer.h"
#include "ocudu/ran/meas_units.h"
#include "ocudu/ran/prach/prach_configuration.h"
#include "ocudu/ran/prach/restricted_set_config.h"
#include "ocudu/ran/sch/sch_mcs.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <chrono>
#include <optional>
#include <vector>

namespace ocudu {

/// Maximum number of RA preambles used per occasion as per TS 38.331.
constexpr unsigned MAX_NOF_RA_PREAMBLES_PER_OCCASION = 64;

/// \remark See TS 38.331, RACH-ConfigGeneric.
struct rach_config_generic {
  /// Values: {0,...,255}.
  uint8_t prach_config_index;
  /// Msg2 RAR window length in #slots. Network configures a value < 10msec. Values: (1, 2, 4, 8, 10, 20, 40, 80).
  unsigned ra_resp_window;
  /// Number of PRACH occasions FDMed in one time instance as per TS38.211, clause 6.3.3.2.
  unsigned msg1_fdm = 1;
  /// Offset of lowest PRACH transmission occasion in frequency domain respective to PRB 0,
  /// as per TS38.211, clause 6.3.3.2. Possible values: {0,...,MAX_NOF_PRB - 1}.
  unsigned msg1_frequency_start;
  /// Zero-correlation zone configuration number as per TS38.331 "zeroCorrelationZoneConfig", used to derive N_{CS}.
  uint16_t zero_correlation_zone_config;
  /// \brief \c preambleReceivedTargetPower, part of \c RACH-ConfigGeneric, TS 38.331.
  /// Target power level at the network receiver side, in dBm. Only values multiple of 2 are valid.
  bounded_integer<int, -202, -60> preamble_rx_target_pw;
  /// Max number of RA preamble transmissions performed before declaring a failure. Values {3, 4, 5, 6, 7, 8, 10, 20,
  /// 50, 100, 200}.
  uint8_t preamble_trans_max = 7;
  /// Power ramping steps for PRACH. Values {0, 2, 4, 6}.
  uint8_t power_ramping_step_db = 4;

  bool operator==(const rach_config_generic& other) const
  {
    return prach_config_index == other.prach_config_index and ra_resp_window == other.ra_resp_window and
           msg1_fdm == other.msg1_fdm and msg1_frequency_start == other.msg1_frequency_start and
           zero_correlation_zone_config == other.zero_correlation_zone_config and
           preamble_rx_target_pw == other.preamble_rx_target_pw and preamble_trans_max == other.preamble_trans_max and
           power_ramping_step_db == other.power_ramping_step_db;
  }
};

/// Parameters to configure prioritized random access.
struct ra_prioritization {
  enum power_ramp_step_high_priority : uint8_t { db0 = 0, db2 = 2, db4 = 4, db6 = 6 };
  enum scaling_factor_bi { zero, dot25, dot5, dot75 };

  /// Power ramping step applied for the prioritized RA procedure.
  power_ramp_step_high_priority pwr_ramp_step_hi_prio;
  /// Scaling factor for the backoff indicator (BI) for the prioritized RA procedure.
  std::optional<scaling_factor_bi> scaling_bi;
};

/// Information relative to RA prioritization for slicing.
struct ra_prioritization_slice_info {
  std::vector<uint8_t> nsag_id_list;
  ra_prioritization    prio;
};

/// Parameters for 2-step RACH configuration as per TS 38.331, "RACH-ConfigCommonTwoStepRA-r16".
/// \note Current version assumes shared RACH occasions between 2-step and 4-step RACH.
struct rach_config_common_two_step {
  /// Configuration of MsgA PUSCH parameters which the UE uses for CB MsgA PUSCH transmission. See TS 38.331,
  /// "MsgA-PUSCH-Config".
  struct msgA_pusch_config {
    /// Time-domain offset in number of slots (based on the numerology of the UL BWP), with respect to the start of the
    /// PRACH slot. See TS 38.213, clause 8.1A. Values: {1, ..., 32}.
    uint8_t td_offset{1};
    /// Index to the PUSCH-TimeDomainAllocationResource of the TDRA table provided either in the PUSCH-ConfigCommon or
    /// in the default Table 6.1.2.1.1-2 in 38.214. The parameter k2 is ignored.
    uint8_t pusch_td_res_index{0};
    /// MCS value used for MsgA, taken from Table 6.1.4.1-1 for DFT-s-OFDM and Table 5.1.31.-1 for CP-OFDM in TS 38.214.
    sch_mcs_index mcs{0};
    /// Number of PRBs per PUSCH occasion. See TS 38.213, 8.1A. Values: {1, ..., 32}.
    uint8_t nof_prbs_per_msgA_po{3};
    /// Offset of lowest PUSCH occasion in frequency domain with respect to PRB 0. See TS 38.331,
    /// "frequencyStartMsgA-PUSCH" and TS 38.213, 8.1A.
    uint16_t prb_start = 0;
    /// The number of msgA PUSCH occasions FDMed in one time instance. See TS 38.331 "nrofMsgA-PO-FDM" and
    /// TS 38.213, 8.1A. Values: {1, 2, 4, 8}.
    uint8_t po_fdm = 1;
  };

  /// \brief Number of contention-based (CB) preambles used for 2-step RA from the non-CBRA 4-step type preambles
  /// associated with each SSB for RO shared with 4-step RA.
  /// This value should not exceed the number of preambles per SSB minus the number of CB preambles for 4-step RA.
  /// See TS 38.331, "msgA-CB-PreamblesPerSSB-PerSharedRO". Values: {1, ..., 60}.
  uint8_t cb_preambles_per_ssb_per_shared_ro{4};
  /// Value in dBm above which the UE selects 2-step RA over 4-step RA. See "msgA-RSRP-Threshold-r16".
  rsrp_range msgA_rsrp_thres{-100};
  /// MsgB response window in slots. It cannot represent a duration larger than 40msec.
  /// See TS 38.331 "msgB-ResponseWindow-r16" and TS 38.321, 5.1.1. Values: {1, 2, 4, 8, 10, 20, 40, 80, 160, 320}.
  uint16_t msgB_response_window_slots = 40;
  /// MsgA PUSCH config.
  msgA_pusch_config pusch;
};

/// Used to specify the cell-specific random-access parameters as per TS 38.331, "RACH-ConfigCommon".
struct rach_config_common {
  rach_config_generic rach_cfg_generic;
  /// Total number of preambles used for contention based and contention free RA. Values: (1..64).
  unsigned total_nof_ra_preambles = MAX_NOF_RA_PREAMBLES_PER_OCCASION;
  /// Maximum time for the Contention Resolution. Values: {8, 16, 24, 32, 40, 48, 56, 64}.
  std::chrono::milliseconds ra_con_res_timer{64};
  /// PRACH Root Sequence Index can be of 2 types, as per \c prach-RootSequenceIndex, \c RACH-ConfigCommon, TS 38.331.
  /// We use \c true for l839, while \c false for l139.
  bool is_prach_root_seq_index_l839;
  /// PRACH root sequence index. Values: (1..839).
  /// \remark See TS 38.211, clause 6.3.3.1.
  unsigned prach_root_seq_index;
  /// \brief Subcarrier spacing of PRACH as per TS38.331, "RACH-ConfigCommon". If invalid, the UE applies the SCS as
  /// derived from the prach-ConfigurationIndex in RACH-ConfigGeneric as per TS38.211 Tables 6.3.3.1-[1-3].
  subcarrier_spacing    msg1_scs;
  restricted_set_config restricted_set;
  /// Enables the transform precoder for Msg3 transmission according to clause 6.1.3 of TS 38.214.
  bool msg3_transform_precoder = false;
  /// Indicates the number of SSBs per RACH occasion (L1 parameter 'SSB-per-rach-occasion'). See TS 38.331, \c
  /// ssb-perRACH-OccasionAndCB-PreamblesPerSSB.
  /// Value 1/8 corresponds to one SSB associated with 8 RACH occasions and so on so forth.
  ssb_per_rach_occasions nof_ssb_per_ro = ssb_per_rach_occasions::one;
  /// Indicates the number of Contention Based preambles per SSB (L1 parameter 'CB-preambles-per-SSB'). See TS 38.331,
  /// \c ssb-perRACH-OccasionAndCB-PreamblesPerSSB.
  /// \remark Values of \c cb_preambles_per_ssb depends on value of \c ssb_per_ro.
  uint8_t nof_cb_preambles_per_ssb = 64;
  /// List of slice-specific RACH configurations.
  std::vector<ra_prioritization_slice_info> ra_prio_slice_info_list;
  /// 2-step RACH configuration (Rel-16). Present only when 2-step RACH is enabled.
  std::optional<rach_config_common_two_step> two_step_rach_cfg;

  bool operator==(const rach_config_common& other) const
  {
    return rach_cfg_generic == other.rach_cfg_generic and total_nof_ra_preambles == other.total_nof_ra_preambles and
           ra_con_res_timer == other.ra_con_res_timer and
           is_prach_root_seq_index_l839 == other.is_prach_root_seq_index_l839 and
           prach_root_seq_index == other.prach_root_seq_index and msg1_scs == other.msg1_scs and
           restricted_set == other.restricted_set and msg3_transform_precoder == other.msg3_transform_precoder and
           nof_ssb_per_ro == other.nof_ssb_per_ro and nof_cb_preambles_per_ssb == other.nof_cb_preambles_per_ssb;
  }
};

namespace ra_helper {

/// Determines range of RA preambles associated with CFRA.
inline interval<unsigned> get_cfra_preambles(const rach_config_common& rach_cfg)
{
  const auto nof_cb_preambles = rach_cfg.nof_cb_preambles_per_ssb;
  const auto nof_msga_preambles =
      rach_cfg.two_step_rach_cfg.has_value() ? rach_cfg.two_step_rach_cfg->cb_preambles_per_ssb_per_shared_ro : 0U;

  // Preambles above the CB (4-step) and MsgA (2-step CB) ranges are reserved for CFRA.
  const unsigned cfra_start = nof_cb_preambles + nof_msga_preambles;
  return {cfra_start, rach_cfg.total_nof_ra_preambles};
}

} // namespace ra_helper

} // namespace ocudu
