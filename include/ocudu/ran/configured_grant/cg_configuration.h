// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/dmrs/dmrs_uplink_config.h"
#include "ocudu/ran/pusch/pusch_configuration.h"
#include "ocudu/ran/resource_allocation/resource_allocation_frequency.h"
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
    sl5120 = 5120,
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
    /// Frequency domain resource allocation for this configured grant.
    /// Currently only Type 1 (contiguous VRBs) is supported; the variant eases future addition of Type 0.
    /// \remark Maps to "Frequency domain resource assignment" as per TS 38.212, Section 7.3.1.1.2.
    std::variant<ra_frequency_type1_configuration> freq_domain_res;
    /// DMRS antenna port index and associated parameters. Values {0,...,31}.
    uint8_t antenna_port;
    /// DMRS sequence initialization value. Values {0, 1}.
    std::optional<uint8_t> dmrs_seq_initialization;
    /// Precoding matrix indicator and number of layers, encoded jointly. Values {0,...,63}.
    uint8_t precoding_and_nof_layers;
    /// SRS resource indicator index for non-codebook-based transmission. Values {0,...,15}.
    //  NOTE: SRS indicator not used in CG yet.
    //  TODO: Remove "static constexpr" once freq. hopping will be supported.
    static constexpr std::optional<uint8_t> srs_resource_indicator = std::nullopt;
    /// MCS for this Configured Grant. Values {0,...,31}.
    /// Values {28,...,31} are not supported in Rel.15.
    uint8_t mcs;
    /// Frequency hopping offset in PRBs. Values {1,...,274}.
    /// \remark See TS 38.214, clause 6.3.
    //  NOTE: Freq. hopping not supported yet.
    //  TODO: Remove "static constexpr" once freq. hopping will be supported.
    static constexpr std::optional<uint16_t> frequency_hopping_offset = std::nullopt;
    /// Index into the path loss reference RS set configured in PUSCH-PowerControl. Values {0,...,3}.
    //  NOTE: Only one element in the set supported.
    //  TODO: Remove "static constexpr" for future refactors.
    static constexpr uint8_t pathloss_ref_index = 0;

    bool operator==(const rrc_configured_ul_grant& rhs) const
    {
      return time_domain_offset == rhs.time_domain_offset && time_domain_allocation == rhs.time_domain_allocation &&
             freq_domain_res == rhs.freq_domain_res && antenna_port == rhs.antenna_port &&
             dmrs_seq_initialization == rhs.dmrs_seq_initialization &&
             precoding_and_nof_layers == rhs.precoding_and_nof_layers && mcs == rhs.mcs;
    }
    bool operator!=(const rrc_configured_ul_grant& rhs) const { return !(rhs == *this); }
  };

  /// Frequency hopping mode. When absent, frequency hopping is disabled.
  //  NOTE: Freq. hopping not supported yet. Remove "static constexpr" once freq. hopping will be supported.
  static constexpr freq_hopping frequency_hopping = freq_hopping::disabled;
  /// DMRS configuration for CG PUSCH transmissions.
  dmrs_uplink_config cg_dmrs_cfg;
  /// MCS table override for CP-OFDM CG transmissions.
  pusch_mcs_table mcs_table;
  /// Enables/disables transform precoder for CG.
  /// If not set, \c msg3-transformPrecoder in \c RACH-ConfigCommon applies.
  std::optional<bool> trans_precoder;
  /// Defines the MCS table to be used with transform precoder.
  //  NOTE: trans. precorder not supported for CG. Remove "static constexpr" once trans. precorder will be supported.
  static constexpr pusch_mcs_table mcs_table_transform_precoder = pusch_mcs_table::qam64;
  /// UCI-on-PUSCH beta offset configuration.
  /// \remark We reuse the same struct as for PUSCH-Config, although \ref alpha_scaling_opt is not used for Configured
  /// Grant.
  uci_on_pusch uci_on_pusch_cfg;
  /// Frequency domain resource allocation type.
  //  NOTE: Freq. alloc. type 0 not supported. Remove "static constexpr" once type 0 will be supported.
  static constexpr res_allocation res_alloc = res_allocation::type_1;
  /// Sets RBG size configuration 2 for PUSCH's RBG size; otherwise, UE defaults to RBG size configuration 1.
  /// \remark Applicable only when \c res_alloc is \c resourceAllocationType2.
  //  NOTE: Only RBG size configuration 1 supported. Remove "static constexpr" once type 2 will be supported.
  static constexpr bool enable_rbg_size_cfg_2 = false;
  /// Sets uplink power control closed loop n1; else, UE defaults to n0.
  //  NOTE: Feature not supported. Remove "static constexpr" this will be supported.
  static constexpr bool enable_pwr_ctrl_loop_n1 = false;
  /// P0-PUSCH-Alpha set index for power control. Values {0,...,29}.
  //  NOTE: Only MIN_P0_PUSCH_ALPHASET_ID supported. Remove "static constexpr" in future refactors.
  static constexpr p0_pusch_alphaset_id p0_pusch_alpha = MIN_P0_PUSCH_ALPHASET_ID;
  /// Number of HARQ processes for CG transmissions. Values {1,...,16}.
  uint8_t nof_harq_processes;
  /// Number of PUSCH repetitions per CG occasion and their RV sequence.
  static constexpr repetitions_t rep =
      cg_configuration::repetitions_t{.rep_k = rep_k_t::n1, .rv_seq = rep_k_rv::s1_0231};
  /// Periodicity of the CG resource.
  periodicity_t periodicity;
  /// Duration (in multiple of periodicity) of the configured-grant timer. Values {1,...,64}.
  //  NOTE: Value statically set. Remove "static constexpr" once interface allows setting this.
  static constexpr uint8_t configured_grant_timer = 4U;
  /// RRC-level resource grant (Type 1 CG). When absent, the grant is activated via DCI (Type 2 CG).
  /// \remark Type 2 CG is not currently supported.
  std::optional<rrc_configured_ul_grant> rrc_configured_ul_grant_cfg;
  bool                                   operator==(const cg_configuration& rhs) const
  {
    return cg_dmrs_cfg == rhs.cg_dmrs_cfg && mcs_table == rhs.mcs_table && trans_precoder == rhs.trans_precoder &&
           uci_on_pusch_cfg == rhs.uci_on_pusch_cfg && nof_harq_processes == rhs.nof_harq_processes &&
           periodicity == rhs.periodicity && rrc_configured_ul_grant_cfg == rhs.rrc_configured_ul_grant_cfg;
  }
  bool operator!=(const cg_configuration& rhs) const { return !(rhs == *this); }
};

} // namespace ocudu
