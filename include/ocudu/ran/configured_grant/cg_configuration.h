// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/dmrs/dmrs_uplink_config.h"
#include "ocudu/ran/pusch/pusch_configuration.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/uci/uci_configuration.h"
#include <optional>
#include <variant>

namespace ocudu {

/// \brief Configured Grant (CG) configuration for Type 1 and Type 2 uplink transmissions.
/// \remark See TS 38.331, "ConfiguredGrantConfig".
struct cg_configuration {
  /// \brief Frequency hopping scheme for CG PUSCH.
  /// \remark See TS 38.331, "ConfiguredGrantConfig" field \c frequencyHopping.
  enum class freq_hopping { disabled, intra_slot, inter_slot };

  /// \brief Resource allocation type for CG PUSCH.
  /// \remark See TS 38.331, "ConfiguredGrantConfig" field \c resourceAllocation.
  enum class res_allocation { type_0, type_1, dynamic_switch };

  /// \brief Number of PUSCH repetitions (repK) for CG transmissions.
  /// \remark See TS 38.331, "ConfiguredGrantConfig" field \c repK.
  enum class rep_k_t { n1, n2, n4, n8 };

  /// \brief RV sequence for CG PUSCH repetitions.
  ///
  /// Values correspond to:
  ///  - s1_0231: RV sequence {0, 2, 3, 1}.
  ///  - s2_0303: RV sequence {0, 3, 0, 3}.
  ///  - s3_0000: RV sequence {0, 0, 0, 0}.
  ///
  /// \remark See TS 38.331, "ConfiguredGrantConfig" field \c repK-RV.
  enum class rep_k_rv { s1_0231, s2_0303, s3_0000 };

  /// \brief Periodicity of the Configured Grant resource, in slots, as per available values in TS 38.331,
  /// "ConfiguredGrantConfig" field \c periodicity.
  /// \remark Sub-slot periodicities are not yet supported.
  /// \remark For 12-symbol slots, \ref sl1024 and sl5120 are not supported.
  enum class periodicity_t {
    /// Normal CP (14 symbols/slot).
    sl1    = 1,
    sl2    = 2,
    sl4    = 4,
    sl5    = 5,
    sl8    = 8,
    sl10   = 10,
    sl16   = 16,
    sl20   = 20,
    sl32   = 32,
    sl40   = 40,
    sl64   = 64,
    sl80   = 80,
    sl128  = 128,
    sl160  = 160,
    sl256  = 256,
    sl320  = 320,
    sl512  = 512,
    sl640  = 640,
    sl1024 = 1024,
    sl1280 = 1280,
    sl2560 = 2560,
    sl5120 = 5160,
  };

  struct repetitions_t {
    /// Number of PUSCH repetitions per CG occasion.
    rep_k_t rep_k = rep_k_t::n1;
    /// RV sequence for the repetitions. When absent, the UE uses the sequence {0, 2, 3, 1}.
    rep_k_rv rv_seq = rep_k_rv::s1_0231;

    bool operator==(const repetitions_t& rhs) const { return rep_k == rhs.rep_k && rv_seq == rhs.rv_seq; }
    bool operator!=(const repetitions_t& rhs) const { return !(rhs == *this); }
  };

  /// \brief RRC-configured uplink grant parameters (Type 1 CG).
  ///
  /// When present, this field provides the full resource grant configured by RRC, without requiring a DCI activation
  /// (i.e., Type 1 CG). When absent, the grant must be activated via DCI (Type 2 CG).
  ///
  /// \remark See TS 38.331, "ConfiguredGrantConfig" field \c rrc-ConfiguredUplinkGrant.
  struct rrc_configured_ul_grant {
    /// Slot offset within the CG period at which the grant starts. Values {0,...,5119}.
    uint16_t time_domain_offset;
    /// Index into the PUSCH time-domain resource allocation table. Values {0,...,15}.
    uint8_t time_domain_allocation;
    /// VRBs for this configured grant, which map to "Frequency domain resource assignment", as per TS 38.212,
    /// Section 7.3.1.1.2 for the bit-field definition.
    vrb_interval vrbs;
    /// DMRS antenna port index and associated parameters. Values {0,...,31}.
    uint8_t antenna_port;
    /// DMRS sequence initialization value. Values {0, 1}.
    std::optional<uint8_t> dmrs_seq_initialization;
    /// Precoding matrix indicator and number of layers, encoded jointly. Values {0,...,63}.
    uint8_t precoding_and_nof_layers;
    /// SRS resource indicator index for non-codebook-based transmission. Values {0,...,15}.
    std::optional<uint8_t> srs_resource_indicator;
    /// MCS for this Configured Grant. Values {0,...,31}.
    /// Values {28,...,31} are not supported in Rel.15.
    uint8_t mcs;
    /// Frequency hopping offset in PRBs. Values {1,...,274}.
    /// \remark See TS 38.214, clause 6.3.
    std::optional<uint16_t> frequency_hopping_offset;
    /// Index into the path loss reference RS set configured in PUSCH-PowerControl. Values {0,...,3}.
    uint8_t pathloss_ref_index;

    bool operator==(const rrc_configured_ul_grant& rhs) const
    {
      return time_domain_offset == rhs.time_domain_offset && time_domain_allocation == rhs.time_domain_allocation &&
             vrbs == rhs.vrbs && antenna_port == rhs.antenna_port &&
             dmrs_seq_initialization == rhs.dmrs_seq_initialization &&
             precoding_and_nof_layers == rhs.precoding_and_nof_layers &&
             srs_resource_indicator == rhs.srs_resource_indicator && mcs == rhs.mcs &&
             frequency_hopping_offset == rhs.frequency_hopping_offset && pathloss_ref_index == rhs.pathloss_ref_index;
    }
    bool operator!=(const rrc_configured_ul_grant& rhs) const { return !(rhs == *this); }
  };

  /// \c CS-RNTI of the UE, common to all CellGroupConfig cells.
  /// \remark CS-RNTI is present in \c PhysicalCellGroupConfig, TS 38.331, not in ConfiguredGrantConfig.
  rnti_t cs_rnti;
  /// Frequency hopping mode. When absent, frequency hopping is disabled.
  freq_hopping frequency_hopping;
  /// DMRS configuration for CG PUSCH transmissions.
  dmrs_uplink_config cg_dmrs_cfg;
  /// MCS table override for CP-OFDM CG transmissions.
  pusch_mcs_table mcs_table;
  /// Enables/disables transform precoder for CG.
  /// If not set, \c msg3-transformPrecoder in \c RACH-ConfigCommon applies.
  std::optional<bool> trans_precoder;
  /// Defines the MCS table to be used with transform precoder.
  pusch_mcs_table mcs_table_transform_precoder;
  /// UCI-on-PUSCH beta offset configuration.
  /// \remark We reuse the same struct as for PUSCH-Config, although \ref alpha_scaling_opt is not used for Configured
  /// Grant.
  uci_on_pusch uci_on_pusch_cfg;
  /// Frequency domain resource allocation type.
  res_allocation res_alloc;
  /// Sets RBG size configuration 2 for PUSCH's RBG size; otherwise, UE defaults to RBG size configuration 1.
  /// \remark Applicable only when \c res_alloc is \c resourceAllocationType2.
  bool enable_rbg_size_cfg_2;
  /// Sets uplink power control closed loop n1; else, UE defaults to n0.
  bool enable_pwr_ctrl_loop_n1;
  /// P0-PUSCH-Alpha set index for power control. Values {0,...,29}.
  p0_pusch_alphaset_id p0_pusch_alpha;
  /// Number of HARQ processes for CG transmissions. Values {1,...,16}.
  uint8_t nof_harq_processes;
  /// Number of PUSCH repetitions per CG occasion and their RV sequence.
  repetitions_t rep;
  /// Periodicity of the CG resource.
  periodicity_t periodicity;
  /// Duration (in multiple of periodicity) of the configured-grant timer. Values {1,...,64}.
  uint8_t configured_grant_timer;
  /// RRC-level resource grant (Type 1 CG). When absent, the grant is activated via DCI (Type 2 CG).
  /// \remark Type 2 CG is not currently supported.
  std::optional<rrc_configured_ul_grant> rrc_configured_ul_grant_cfg;
  /// Number of PRBs in the UL BWP this configured grant belongs to.
  /// \remark Not a protocol-level field; required for RIV computation when encoding \c frequencyDomainAllocation.
  unsigned bwp_nof_prbs = 0;

  bool operator==(const cg_configuration& rhs) const
  {
    return cs_rnti == rhs.cs_rnti && frequency_hopping == rhs.frequency_hopping && cg_dmrs_cfg == rhs.cg_dmrs_cfg &&
           mcs_table == rhs.mcs_table && trans_precoder == rhs.trans_precoder &&
           mcs_table_transform_precoder == rhs.mcs_table_transform_precoder &&
           uci_on_pusch_cfg == rhs.uci_on_pusch_cfg && res_alloc == rhs.res_alloc &&
           enable_rbg_size_cfg_2 == rhs.enable_rbg_size_cfg_2 &&
           enable_pwr_ctrl_loop_n1 == rhs.enable_pwr_ctrl_loop_n1 && p0_pusch_alpha == rhs.p0_pusch_alpha &&
           nof_harq_processes == rhs.nof_harq_processes && rep == rhs.rep && periodicity == rhs.periodicity &&
           configured_grant_timer == rhs.configured_grant_timer &&
           rrc_configured_ul_grant_cfg == rhs.rrc_configured_ul_grant_cfg;
  }
  bool operator!=(const cg_configuration& rhs) const { return !(rhs == *this); }
};

} // namespace ocudu
