// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/dmrs/dmrs.h"
#include "ocudu/ran/harq_id.h"
#include "ocudu/ran/pdsch/pdsch_mcs.h"
#include "ocudu/ran/radio_link_monitoring.h"
#include "ocudu/ran/resource_allocation/vrb_to_prb.h"
#include "ocudu/ran/sr_configuration.h"
#include "ocudu/scheduler/config/cg_builder_params.h"
#include "ocudu/scheduler/config/csi_helper.h"
#include "ocudu/scheduler/config/pucch_resource_builder_params.h"
#include "ocudu/scheduler/config/serving_cell_config.h"
#include "ocudu/scheduler/config/srs_builder_params.h"
#include <optional>

namespace ocudu {

/// Parameters used to build the PDSCH config for UEs of a given cell and BWP.
/// \note These parameters may not represent the actual config values used by the UEs, as the latter may depend
/// on the UE capabilities.
struct pdsch_builder_params {
  /// Maximum number of DL HARQ processes available to UEs.
  /// \remark See TS 38.331, \c nrofHARQ-ProcessesForPDSCH.
  uint8_t max_harq_procs = 16;
  /// Optional maximum number of DL layers. If not set, DL antenna ports are used.
  std::optional<unsigned> max_nof_layers;
  /// Preferred MCS table to use for PDSCH.
  pdsch_mcs_table mcs_table = pdsch_mcs_table::qam256;
  /// Position for additional DM-RS in DL, see Tables 7.4.1.1.2-3 and 7.4.1.1.2-4 in TS 38.211.
  dmrs_additional_positions additional_positions{dmrs_additional_positions::pos2};
  /// VRB-to-PRB mapping type for PDSCH. The field vrb-ToPRB-Interleaver applies to DCI format 1_1.
  vrb_to_prb::mapping_type interleaving_bundle_size{vrb_to_prb::mapping_type::non_interleaved};
  /// See TS 38.331, \c downlinkHARQ-FeedbackDisabled.
  /// A bit set to 1 indicates HARQ processes with disabled DL HARQ feedback; a bit set to 0 indicate feedback enabled.
  harq_dl_feedback_disabled_mask dl_harq_feedback_disabled = harq_dl_feedback_disabled_mask(MAX_NOF_HARQS);
};

/// PUCCH parameters for a given BWP of a given DU cell.
struct pucch_builder_params {
  /// Minimum k1 supported by the BWP for both common and dedicated PUCCH. Possible values: {1, ..., 7}.
  /// [Implementation-defined] Even though the "dl-DataToUl-Ack" ranges from {1, ..., 15} as per TS 38.213, 9.1.2.1, we
  /// restrict min_k1 to be at most 7. The reason being that we need a k1 < 8 for common PUCCH allocations.
  uint8_t min_k1 = 4;
  /// SR periodicity to use for UE-dedicated SR resources.
  sr_periodicity sr_period = sr_periodicity::sl_40;
  /// Parameters used to generate the PUCCH resources of a cell.
  pucch_resource_builder_params resources;
};

/// PUSCH parameters for a given BWP of a given DU cell.
struct pusch_builder_params {
  /// Maximum number of HARQ processes. Values: {16, 32}.
  /// \remark See TS 38.331, \c nrofHARQ-ProcessesForPUSCH.
  uint8_t max_harq_procs = 16;
  /// \brief Minimum k2 value used in the generation of the UE PUSCH time-domain resources.
  /// Possible values: {1, ..., 7}.
  /// [Implementation-defined] The value of min_k2 should be equal or lower than min_k1. Otherwise, the scheduler is
  /// forced to pick higher k1 values, as it cannot allocate PUCCHs in slots where there is an PUSCH with an already
  /// assigned DAI.
  uint8_t min_k2 = 4;
  /// Indicates which MCS table the UE shall use for PUSCH.
  pusch_mcs_table mcs_table{pusch_mcs_table::qam64};
  /// PUSCH Maximum of transmission layers. Limits the maximum rank the UE is configured with. Values: {1, ..., 4}.
  uint8_t max_nof_layers = 1;
  /// Whether transform precoding is enabled in PUSCH.
  bool transform_precoding_enabled = false;
  /// Position for additional DM-RS in UL (see TS 38.211, clause 6.4.1.1.3). If the field is not set, the UE applies the
  /// value pos2.
  dmrs_additional_positions additional_positions{dmrs_additional_positions::pos2};
  /// UCI beta offsets (semi-static) for PUSCH.
  std::optional<uci_on_pusch::beta_offsets_semi_static> uci_beta_offsets;
  /// P0-PUSCH-AlphaSet alpha for PUSCH power control.
  std::optional<alpha> p0_pusch_alpha;
  /// A bit set to one identifies a HARQ process in modeA and a bit set to zero identifies a HARQ process in modeB.
  /// \remark See TS 38.331, \c uplinkHARQ-mode.
  harq_ul_mode_mask ul_harq_mode = ~harq_ul_mode_mask(MAX_NOF_HARQS);
  /// Maximum number of code-block-groups (CBGs) per TB. See TS 38.213, clause 9.1. Values: {2, 4, 6, 8}.
  std::optional<uint8_t> cbg_tx;
  /// \c xOverhead.
  x_overhead x_ov_head{x_overhead::not_set};
};

/// Random Access parameters for this BWP.
struct rach_builder_params {
  /// \brief Whether to enable contention-free random access (CFRA) for this BWP. If enabled, the number of RA preambles
  /// used for CBRA (see \c nof_cb_preambles_per_ssb) must be less than \c total_nof_ra_preambles.
  bool cfra_enabled = true;
};

/// Parameters used to configure paging in this BWP.
struct paging_builder_params {
  /// Whether eDRX paging is enabled.
  bool edrx_enabled = false;
};

/// Parameters used to setup Radio-link Monitoring in a given BWP.
struct du_rlm_params {
  /// Defines which resources (e.g, default, SSB, CSI-RS, SSB and CSI-RS) the UE should use for RLM.
  rlm_resource_type type = rlm_resource_type::default_type;
};

/// Parameters used to generate a BWP configuration.
struct bwp_builder_params {
  /// UE-dedicated PDCCH configuration used for DU-generated configs.
  std::optional<pdcch_config> pdcch_cfg;
  /// Parameters relative to the generation of the PDSCH configs.
  pdsch_builder_params pdsch;
  /// Parameters relative to the generation of the PUSCH configs.
  pusch_builder_params pusch;
  /// Parameters relative to the generation of the PUCCH configs.
  pucch_builder_params pucch;
  /// Parameters relative to the generation of the CSI Meas Config.
  std::optional<du_csi_params> csi;
  /// Parameters for SRS-Config generation.
  srs_builder_params srs_cfg;
  /// Parameters for Configured Grant (CG) PUSCH generation. When absent, no CG is configured.
  std::optional<cg_builder_params> cg_cfg;
  /// Parameters for Random Access in this BWP.
  rach_builder_params rach;
  /// Parameters relative to the generation of the paging configs.
  paging_builder_params paging;
  /// Parameters used to setup Radio-Link Monitoring.
  du_rlm_params rlm;
};

} // namespace ocudu
