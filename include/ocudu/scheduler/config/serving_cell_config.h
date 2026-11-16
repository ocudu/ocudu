// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/configured_grant/cg_configuration.h"
#include "ocudu/ran/csi_rs/csi_meas_config.h"
#include "ocudu/ran/dmrs/dmrs_downlink_config.h"
#include "ocudu/ran/pdcch/downlink_preemption.h"
#include "ocudu/ran/pdsch/pdsch_mcs.h"
#include "ocudu/ran/pdsch/pdsch_prb_bundling.h"
#include "ocudu/ran/pdsch/pdsch_rate_match_pattern.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/pucch/pucch_tpc.h"
#include "ocudu/ran/pucch/srs_tpc.h"
#include "ocudu/ran/pusch/pusch_configuration.h"
#include "ocudu/ran/pusch/pusch_tpc.h"
#include "ocudu/ran/radio_link_monitoring.h"
#include "ocudu/ran/resource_allocation/vrb_to_prb.h"
#include "ocudu/ran/serv_cell_index.h"
#include "ocudu/ran/srs/srs_configuration.h"
#include "ocudu/ran/tci.h"
#include "ocudu/scheduler/config/bwp_configuration.h"
#include "ocudu/scheduler/result/resource_block_group.h"

namespace ocudu {

/// Converts a uint into a \ref serv_cell_index.
inline serv_cell_index_t uint_to_serv_cell_index(unsigned idx)
{
  return idx < MAX_NOF_SERVING_CELLS ? static_cast<serv_cell_index_t>(idx)
                                     : static_cast<serv_cell_index_t>(MAX_NOF_SERVING_CELLS);
}

/// "PDCCH-Config" - UE-dedicated PDCCH configuration as per TS 38.331.
struct pdcch_config {
  /// List of CORESETs to be used by the UE. In case of CORESET Id overlaps with commonControlResourceSet,
  /// the CORESET in this list takes precedence. The network configures at most 3 CORESETs per BWP per cell (including
  /// UE-specific and common CORESETs).
  std::vector<coreset_configuration> coresets;
  /// List of SearchSpaces to be used by the UE. The network configures at most 10 Search Spaces per BWP per cell
  /// (including UE-specific and common Search Spaces).
  std::vector<search_space_configuration> search_spaces;
  /// Configuration of downlink preemption indications to be monitored in this cell.
  std::optional<downlink_preemption> dl_preemption;
  /// TPC Commands Configuration to configure UE for extracting TPC commands for PUSCH from a group-TPC messages on DCI.
  std::optional<pusch_tpc_command_config> pusch_tpc_cmd_cfg;
  /// TPC Commands Configuration to configure UE for extracting TPC commands for PUCCH from a group-TPC messages on DCI.
  std::optional<pucch_tpc_command_config> pucch_tpc_cmd_cfg;
  /// TPC Commands Configuration to configure UE for extracting TPC commands for SRS from a group-TPC messages on DCI.
  std::optional<srs_tpc_command_config> srs_tpc_cmd_cfg;
  // TODO: add remaining fields.

  bool operator==(const pdcch_config& rhs) const
  {
    return coresets == rhs.coresets && search_spaces == rhs.search_spaces && dl_preemption == rhs.dl_preemption &&
           pusch_tpc_cmd_cfg == rhs.pusch_tpc_cmd_cfg && pucch_tpc_cmd_cfg == rhs.pucch_tpc_cmd_cfg &&
           srs_tpc_cmd_cfg == rhs.srs_tpc_cmd_cfg;
  }
  bool operator!=(const pdcch_config& rhs) const { return !(rhs == *this); }
};

/// "PDSCH-Config" - UE-dedicated PDSCH Configuration as per TS 38.331.
struct pdsch_config {
  /// \brief Resource allocation type of to DCI format 1_1.
  /// \remark See TS 38.214, clause 5.1.2.2.
  enum class resource_allocation { resource_allocation_type_0, resource_allocation_type_1, dynamic_switch };

  /// \brief Aggregation factor.
  /// \remark See TS 38.214, clause 5.1.2.1.
  enum class pdsch_aggregation_factor { n2, n4, n8 };

  enum class max_no_codeword_scheduled_by_dci { n1, n2 };

  /// \brief HARQ process number for DCI Format 1_1.
  /// \remark See TS 38.212, Section 7.3.1.2.2 Format 1_1.
  enum class harq_process_num_dci_1_1_size { n4 = 4, n5 = 5 };

  /// \brief HARQ process number for DCI Format 1_2.
  /// \remark See TS 38.212, Section 7.3.1.2.3 Format 1_2.
  enum class harq_process_num_dci_1_2_size { n0 = 0, n1 = 1, n2 = 2, n3 = 3, n4 = 4, n5 = 5 };

  /// Identifier used to initialize data scrambling (c_init) for PDSCH. If the field is absent, the UE applies the PCI.
  /// See TS 38.331, \e dataScramblingIdentityPDSCH, and TS 38.211, 7.3.1.1. Values: {0,...,1023}.
  std::optional<uint16_t> data_scrambling_id_pdsch;
  /// DMRS configuration for PDSCH transmissions using PDSCH mapping type A (chosen dynamically via
  /// PDSCH-TimeDomainResourceAllocation).
  std::optional<dmrs_downlink_config> pdsch_mapping_type_a_dmrs;
  /// DMRS configuration for PDSCH transmissions using PDSCH mapping type B (chosen dynamically via
  /// PDSCH-TimeDomainResourceAllocation).
  std::optional<dmrs_downlink_config> pdsch_mapping_type_b_dmrs;
  /// VRB-to-PRB mapping type for PDSCH. The field vrb-ToPRB-Interleaver applies to DCI format 1_1.
  vrb_to_prb::mapping_type vrb_to_prb_interleaving = vrb_to_prb::mapping_type::non_interleaved;
  /// A list of Transmission Configuration Indicator (TCI) states indicating a transmission configuration which includes
  /// QCL-relationships between the DL RSs in one RS set and the PDSCH DMRS ports (see TS 38.214, clause 5.1.5).
  static_vector<tci_state, MAX_NOF_TCI_STATES> tci_states;
  /// Configuration of resource allocation type 0 and resource allocation type 1 for non-fallback DCI.
  resource_allocation res_alloc = resource_allocation::resource_allocation_type_1;
  /// Number of repetitions for data. When the field is absent the UE applies the value 1.
  std::optional<pdsch_aggregation_factor> aggr_factor;
  /// \brief PDSCH time domain resource allocations, either the legacy \e pdsch-TimeDomainAllocationList or (when
  /// repetitions enabled
  // the Rel-16 \e pdsch-TimeDomainAllocationList-r16, which replaces the legacy and
  /// common lists for DCI format 1_1. Size: (0..maxNrofDL-Allocations=16).
  std::vector<pdsch_time_domain_resource_allocation> pdsch_td_alloc_list;
  /// \brief Enables the Rel-16 slot-based repetition scheme, \e repetitionSchemeConfig-r16 with \e slotBased-r16.
  /// Required by the UE to apply \e repetitionNumber-r16 of the Rel-16 TDRA list. With a single TCI state indicated in
  /// the DCI, the TCI mapping is not used and is signalled as cyclic.
  bool slot_based_repetition_enabled = false;
  /// Resources patterns which the UE should rate match PDSCH around. The UE rate matches around the union of all
  /// resources indicated in the rate match patterns. Rate match patterns defined here on cell level apply only to PDSCH
  /// of the same numerology. See 38.214, clause 5.1.4,1.
  static_vector<rate_match_pattern, MAX_NOF_RATE_MATCH_PATTERNS> rate_match_pattrn;

  /// Selection between config 1 and config 2 for RBG size for PDSCH. The UE ignores this field if resourceAllocation is
  /// set to resourceAllocationType1.
  rbg_size rbg_sz = rbg_size::config1;
  /// Indicates which MCS table the UE shall use for PDSCH. (see TS 38.214 [19], clause 5.1.3.1). The field mcs-Table
  /// applies to DCI format 1_0 and DCI format 1_1.
  pdsch_mcs_table mcs_table{pdsch_mcs_table::qam64};
  /// Maximum number of code words a single DCI may schedule. This changes the number of MCS/RV/NDI bits in the DCI
  /// message from 1 to 2. See TS 38.331, "maxNrofCodeWordsScheduledByDCI".
  bool is_max_cw_sched_by_dci_is_two{false};
  /// Indicates the PRB bundle type and bundle size(s). If dynamic is chosen, the actual bundleSizeSet1 or
  /// bundleSizeSet2 to use is indicated via DCI. Constraints on bundleSize(Set) setting depending on
  /// vrb-ToPRB-Interleaver and rbg-Size settings are described in TS 38.214, clause 5.1.2.3. If a bundleSize(Set)
  /// value is absent, the UE applies the value n2. The field prb-BundlingType applies to DCI format 1_1.
  prb_bundling prb_bndlg;
  /// \brief List of zp-CSI-RS-Resources. Maximum size: 32.
  std::vector<zp_csi_rs_resource> zp_csi_rs_res_list;
  /// A set of periodically occurring ZP-CSI-RS-Resources. The network uses the ZP-CSI-RSResourceSetId=0 for this set.
  std::optional<zp_csi_rs_resource_set> p_zp_csi_rs_res;

  /// The UE specific HARQ Process number field size in DCI Format 1_1.
  /// 5 bits if higher layer parameter harq-ProcessNumberSizeDCI-1-1 is configured; otherwise 4 bits.
  std::optional<harq_process_num_dci_1_1_size> harq_process_num_size_dci_1_1{harq_process_num_dci_1_1_size::n4};

  /// The UE specific HARQ Process number field size in DCI Format 1_2.
  /// 0, 1, 2, 3, 4 or 5 bits determined by higher layer parameter harq-ProcessNumberSizeDCI-1-2-v1700 if configured.
  /// otherwise 0, 1, 2, 3 or 4 bits determined by higher layer parameter harq-ProcessNumberSizeDCI-1-2.
  std::optional<harq_process_num_dci_1_2_size> harq_process_num_size_dci_1_2{harq_process_num_dci_1_2_size::n4};

  bool operator==(const pdsch_config& rhs) const
  {
    return data_scrambling_id_pdsch == rhs.data_scrambling_id_pdsch &&
           pdsch_mapping_type_a_dmrs == rhs.pdsch_mapping_type_a_dmrs &&
           pdsch_mapping_type_b_dmrs == rhs.pdsch_mapping_type_b_dmrs &&
           vrb_to_prb_interleaving == rhs.vrb_to_prb_interleaving && tci_states == rhs.tci_states &&
           res_alloc == rhs.res_alloc && aggr_factor == rhs.aggr_factor &&
           pdsch_td_alloc_list == rhs.pdsch_td_alloc_list &&
           slot_based_repetition_enabled == rhs.slot_based_repetition_enabled &&
           rate_match_pattrn == rhs.rate_match_pattrn && rbg_sz == rhs.rbg_sz && mcs_table == rhs.mcs_table &&
           is_max_cw_sched_by_dci_is_two == rhs.is_max_cw_sched_by_dci_is_two && prb_bndlg == rhs.prb_bndlg &&
           zp_csi_rs_res_list == rhs.zp_csi_rs_res_list and p_zp_csi_rs_res == rhs.p_zp_csi_rs_res &&
           harq_process_num_size_dci_1_1 == rhs.harq_process_num_size_dci_1_1 &&
           harq_process_num_size_dci_1_2 == rhs.harq_process_num_size_dci_1_2;
  }
  bool operator!=(const pdsch_config& rhs) const { return !(rhs == *this); }
};

/// "BWP-DownlinkDedicated" as per TS 38.331.
struct bwp_downlink_dedicated {
  std::optional<pdcch_config>                 pdcch_cfg;
  std::optional<pdsch_config>                 pdsch_cfg;
  std::optional<radio_link_monitoring_config> rlm_cfg;
  // TODO: Remaining

  bool operator==(const bwp_downlink_dedicated& other) const
  {
    return pdcch_cfg == other.pdcch_cfg and pdsch_cfg == other.pdsch_cfg and rlm_cfg == other.rlm_cfg;
  }

  bool operator!=(const bwp_downlink_dedicated& other) const { return !(other == *this); }
};

/// Used to configure the dedicated UE-specific parameters of an UL BWP.
/// \remark See TS 38.331, BWP-UplinkDedicated.
struct bwp_uplink_dedicated {
  std::optional<pusch_config>     pusch_cfg;
  std::optional<cg_configuration> cg_cfg;
  std::optional<srs_config>       srs_cfg;

  bool operator==(const bwp_uplink_dedicated& other) const
  {
    return pusch_cfg == other.pusch_cfg and srs_cfg == other.srs_cfg and cg_cfg == other.cg_cfg;
  }
};

/// Uplink configuration, as per \c UplinkConfig, in \c ServingCellConfig, TS 38.331.
struct uplink_config {
  bwp_uplink_dedicated init_ul_bwp;
  // TODO: add remaining fields.

  bool operator==(const uplink_config& other) const { return init_ul_bwp == other.init_ul_bwp; }
};

/// \c ServingCellConfig, as per TS 38.331.
struct serving_cell_config {
  /// DU-specific cell identifier.
  du_cell_index_t cell_index;
  /// Initial Downlink BWP.
  bwp_downlink_dedicated init_dl_bwp;
  /// \c uplinkConfig, containing the UL configuration.
  std::optional<uplink_config> ul_config;
  /// \c CSI-MeasConfig.
  std::optional<csi_meas_config> csi_meas_cfg;

  bool operator==(const serving_cell_config& other) const
  {
    return cell_index == other.cell_index and init_dl_bwp == other.init_dl_bwp and ul_config == other.ul_config and
           csi_meas_cfg == other.csi_meas_cfg;
  }
};

} // namespace ocudu
