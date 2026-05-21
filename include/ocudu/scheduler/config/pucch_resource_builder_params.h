// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_integer.h"
#include "ocudu/adt/strong_type.h"
#include "ocudu/ran/frame_types.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "ocudu/ran/pucch/pucch_info.h"
#include "ocudu/ran/pucch/pucch_mapping.h"
#include <optional>
#include <variant>
#include <vector>

namespace ocudu {

/// Parameters for the generation of PUCCH Format 0 resources.
struct pucch_f0_params {
  using nof_symbols = bounded_integer<unsigned, pucch_constants::f0::MIN_NOF_SYMS, pucch_constants::f0::MAX_NOF_SYMS>;

  /// Number of OFDM symbols.
  nof_symbols nof_syms{pucch_constants::f0::MAX_NOF_SYMS};
  /// Whether to configure intraslot frequency hopping.
  bool intraslot_freq_hopping{false};

  bool operator==(const pucch_f0_params& other) const
  {
    return nof_syms == other.nof_syms and intraslot_freq_hopping == other.intraslot_freq_hopping;
  }
  bool operator!=(const pucch_f0_params& other) const { return not(*this == other); }
};

/// \brief Options for the number of Initial Cyclic Shifts that can be set for PUCCH Format 1.
///
/// Defines the number of different Initial Cyclic Shifts that can be used for PUCCH Format 1, as per \c PUCCH-format1,
/// in \c PUCCH-Config, TS 38.331. We assume the CS are evenly distributed, which means we can only have a divisor of 12
/// possible cyclic shifts.
enum class pucch_nof_cyclic_shifts { no_cyclic_shift = 1, two = 2, three = 3, four = 4, six = 6, twelve = 12 };

constexpr unsigned to_uint(pucch_nof_cyclic_shifts opt)
{
  return static_cast<unsigned>(opt);
}

/// Parameters for the generation of PUCCH Format 1 resources.
struct pucch_f1_params {
  using nof_symbols = bounded_integer<unsigned, pucch_constants::f1::MIN_NOF_SYMS, pucch_constants::f1::MAX_NOF_SYMS>;

  /// Number of OFDM symbols.
  nof_symbols nof_syms{pucch_constants::f1::MAX_NOF_SYMS};
  bool        intraslot_freq_hopping{false};
  /// Number of Initial Cyclic Shifts to use.
  pucch_nof_cyclic_shifts nof_cyc_shifts{pucch_nof_cyclic_shifts::no_cyclic_shift};
  /// Whether to configure the resources with different OCCs.
  bool occ_supported{false};

  bool operator==(const pucch_f1_params& other) const
  {
    return nof_syms == other.nof_syms and intraslot_freq_hopping == other.intraslot_freq_hopping and
           nof_cyc_shifts == other.nof_cyc_shifts and occ_supported == other.occ_supported;
  }
  bool operator!=(const pucch_f1_params& other) const { return not(*this == other); }
};

/// Parameters for the generation of PUCCH Format 2 resources.
struct pucch_f2_params {
  using nof_symbols = bounded_integer<unsigned, pucch_constants::f2::MIN_NOF_SYMS, pucch_constants::f2::MAX_NOF_SYMS>;
  using nof_rbs     = bounded_integer<unsigned, pucch_constants::f2::MIN_NOF_RBS, pucch_constants::f2::MAX_NOF_RBS>;

  /// \brief Number of OFDM symbols.
  ///
  /// \remark For intraslot-freq-hopping, \c nof_symbols must be set to 2.
  nof_symbols nof_syms{pucch_constants::f2::MAX_NOF_SYMS};
  /// Number of RBs.
  nof_rbs max_nof_rbs{pucch_constants::f2::MIN_NOF_RBS};
  /// Whether to configure intraslot frequency hopping.
  bool intraslot_freq_hopping{false};
  /// \brief Maximum payload in bits to be carried.
  ///
  /// \remark When this field is set, \c max_nof_rbs is ignored and the maximum number of RBs is computed according to
  ///         \ref get_pucch_format2_max_nof_prbs.
  std::optional<unsigned> max_payload_bits;
  /// Maximum allowed effective code rate.
  max_pucch_code_rate max_code_rate{max_pucch_code_rate::dot_25};

  bool operator==(const pucch_f2_params& other) const
  {
    return nof_syms == other.nof_syms and max_nof_rbs == other.max_nof_rbs and
           intraslot_freq_hopping == other.intraslot_freq_hopping and max_payload_bits == other.max_payload_bits and
           max_code_rate == other.max_code_rate;
  }
  bool operator!=(const pucch_f2_params& other) const { return not(*this == other); }
};

/// Collects the parameters for PUCCH Format 3 that can be configured.
struct pucch_f3_params {
  using nof_symbols = bounded_integer<unsigned, pucch_constants::f3::MIN_NOF_SYMS, pucch_constants::f3::MAX_NOF_SYMS>;
  using nof_rbs     = bounded_integer<unsigned, pucch_constants::f3::MIN_NOF_RBS, pucch_constants::f3::MAX_NOF_RBS>;

  /// Number of OFDM symbols.
  nof_symbols nof_syms{pucch_constants::f3::MIN_NOF_SYMS};
  /// Number of RBs.
  nof_rbs max_nof_rbs{pucch_constants::f3::MIN_NOF_RBS};
  /// Whether to configure intraslot frequency hopping.
  bool intraslot_freq_hopping{false};
  /// \brief Maximum payload in bits to be carried.
  ///
  /// \remark When this field is set, \c max_nof_rbs is ignored and the maximum number of RBs is computed according to
  ///         \ref get_pucch_format3_max_nof_prbs.
  std::optional<unsigned> max_payload_bits;
  /// Maximum allowed effective code rate.
  max_pucch_code_rate max_code_rate{max_pucch_code_rate::dot_25};
  /// Whether to configure the resources with additional DM-RS symbols.
  bool additional_dmrs{false};
  /// Whether to configure the resources with pi/2-BPSK modulation.
  bool pi2_bpsk{false};

  bool operator==(const pucch_f3_params& other) const
  {
    return nof_syms == other.nof_syms and max_nof_rbs == other.max_nof_rbs and
           intraslot_freq_hopping == other.intraslot_freq_hopping and max_payload_bits == other.max_payload_bits and
           max_code_rate == other.max_code_rate and additional_dmrs == other.additional_dmrs and
           pi2_bpsk == other.pi2_bpsk;
  }
  bool operator!=(const pucch_f3_params& other) const { return not(*this == other); }
};

/// Collects the parameters for PUCCH Format 4 that can be configured.
struct pucch_f4_params {
  using nof_symbols = bounded_integer<unsigned, pucch_constants::f4::MIN_NOF_SYMS, pucch_constants::f4::MAX_NOF_SYMS>;

  /// Number of OFDM symbols.
  nof_symbols nof_syms{pucch_constants::f4::MAX_NOF_SYMS};
  /// Whether to configure intraslot frequency hopping.
  bool intraslot_freq_hopping{false};
  /// Maximum allowed effective code rate.
  max_pucch_code_rate max_code_rate{max_pucch_code_rate::dot_25};
  /// Whether to configure the resources with additional DM-RS symbols.
  bool additional_dmrs{false};
  /// Whether to configure the resources with pi/2-BPSK modulation.
  bool pi2_bpsk{false};
  /// Whether to configure the resources with different OCCs.
  bool occ_supported{false};
  /// OCC length to use for PUCCH Format 4 resources.
  pucch_f4_occ_len occ_length{pucch_f4_occ_len::n2};

  bool operator==(const pucch_f4_params& other) const
  {
    return nof_syms == other.nof_syms and intraslot_freq_hopping == other.intraslot_freq_hopping and
           max_code_rate == other.max_code_rate and additional_dmrs == other.additional_dmrs and
           pi2_bpsk == other.pi2_bpsk and occ_supported == other.occ_supported and occ_length == other.occ_length;
  }
  bool operator!=(const pucch_f4_params& other) const { return not(*this == other); }
};

struct pucch_resource_set_config_id_tag;
/// PUCCH Resource Set configuration ID. Range: 0 to \ref pucch_resource_builder_params::nof_cell_res_set_configs - 1.
using pucch_resource_set_config_id =
    strong_type<uint8_t, struct pucch_res_set_cfg_id_tag, strong_equality, strong_increment_decrement>;
struct pucch_sr_resource_id_tag;
/// ID for a PUCCH resource used for SR. Range: 0 to \ref pucch_resource_builder_params::nof_cell_sr_resources - 1.
using pucch_sr_resource_id =
    strong_type<uint8_t, struct pucch_sr_resource_id_tag, strong_equality, strong_increment_decrement>;
struct pucch_csi_resource_id_tag;
/// ID for a PUCCH resource used for CSI. Range: 0 to \ref pucch_resource_builder_params::nof_cell_sr_resources - 1.
using pucch_csi_resource_id =
    strong_type<uint8_t, struct pucch_csi_resource_id_tag, strong_equality, strong_increment_decrement>;

/// PUCCH HARQ-ACK repetition configuration for a cell.
struct pucch_harq_ack_rep_params {
  /// SINR thresholds (in dB) used to map UE SINR to PUCCH HARQ-ACK repetition factor. Up to 3 entries, in order:
  /// [max SINR for n2, max SINR for n4, max SINR for n8].
  std::vector<float> sinr_thresholds;
  /// Repetition factor configured for each PUCCH resource within a HARQ-ACK PUCCH resource set.
  /// Size equals \ref pucch_resource_builder_params::res_set_size.
  std::vector<pucch_repetition_factor> factors_per_res;
};

/// \brief Parameters for PUCCH configuration.
///
/// Defines the parameters that are used for the PUCCH configuration builder. These parameters are used to define the
/// number of PUCCH resources, as well as the PUCCH format-specific parameters.
///
/// [Implementation-defined] The cell PUCCH resource list will contain all resources in the cell ordered by type:
///   | HARQ Resource Set ID 0 | SR | HARQ Resource Set ID 1 | CSI | (SR_F2) | (CSI_F0) |
/// [Implementation-defined] The UE PUCCH resource list will contain the resources in the following order:
///   | HARQ Resource Set ID 0 | SR | HARQ Resource Set ID 1 | CSI | (SR_F2) | (CSI_F0) |
/// Where SR_F2 and CSI_F0 only exist when using Format 0 and Format 2 together.
struct pucch_resource_builder_params {
  /// Number of PUCCH resources for SR configured per UE.
  static constexpr unsigned nof_sr_res_per_ue = 1U;
  /// Number of PUCCH resources for CSI configured per UE (when periodic CSI reporting is configured).
  static constexpr unsigned nof_csi_res_per_ue = 1U;

  static constexpr unsigned max_res_set_size = 8;
  using resource_set_size                    = bounded_integer<unsigned, 1, max_res_set_size>;

  /// Number of resources to use for both Resource Set ID 0 and Resource Set ID 1. Both sets always have the same size.
  /// \remark For F0+F2, this doesn't include the extra resources (SR/CSI_F0 in Set 0, SR_F2/CSI in Set 1).
  resource_set_size res_set_size = 6;
  /// \brief Number of separate PUCCH resource set configurations for HARQ-ACK reporting that are available in a cell.
  ///
  /// \remark Each resource set configuration defines different resources for Resource Set ID 0 and 1.
  /// \remark UEs will be distributed possibly over different configurations. The more configurations, the fewer UEs
  ///         will have to share the same set, reducing the chance that UEs won't be allocated PUCCH due to lack of
  ///         resources. However, the usage of PUCCH-dedicated REs will be proportional to the number of sets.
  unsigned nof_cell_res_set_configs = 1;
  /// \brief Defines how many PUCCH F0/F1 resources should be dedicated for SR at cell level.
  /// Each UE will be allocated 1 resource for SR.
  unsigned nof_cell_sr_resources = 2;
  /// \brief Defines how many PUCCH F2/F3/F4 resources should be dedicated for CSI at cell level.
  /// Each UE will be allocated 1 resource for CSI.
  unsigned nof_cell_csi_resources = 1;
  /// \brief Parameters for the generation of PUCCH Format 0 or Format 1 resources.
  ///
  /// \remark Having \c pucch_f1_params first forces the variant to use the Format 1 in the default constructor.
  std::variant<pucch_f1_params, pucch_f0_params> f0_or_f1_params;
  /// Parameters for the generation of PUCCH Format 2, Format 3 or Format 4 resources.
  std::variant<pucch_f2_params, pucch_f3_params, pucch_f4_params> f2_or_f3_or_f4_params;
  /// Maximum number of symbols per UL slot dedicated for PUCCH.
  /// \remark In case of Sounding Reference Signals (SRS) being used, the number of symbols should be reduced so that
  ///         the PUCCH resources do not overlap in symbols with the SRS resources.
  /// \remark This parameter should be computed by the GNB and not exposed to the user configuration interface.
  bounded_integer<unsigned, 1, 14> max_nof_symbols = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
  /// PUCCH HARQ-ACK repetition configuration. If not set, HARQ-ACK repetition is disabled.
  std::optional<pucch_harq_ack_rep_params> harq_ack_rep;

  /// Get the PUCCH format used for Resource Set ID 0 and SR resources.
  pucch_format format_01() const
  {
    return std::holds_alternative<pucch_f0_params>(f0_or_f1_params) ? pucch_format::FORMAT_0 : pucch_format::FORMAT_1;
  }

  /// Get the PUCCH format used for Resource Set ID 1 and CSI resources.
  pucch_format format_234() const
  {
    if (std::holds_alternative<pucch_f2_params>(f2_or_f3_or_f4_params)) {
      return pucch_format::FORMAT_2;
    }
    if (std::holds_alternative<pucch_f3_params>(f2_or_f3_or_f4_params)) {
      return pucch_format::FORMAT_3;
    }
    return pucch_format::FORMAT_4;
  }

  // \brief Get the ID of a given Resource Set ID 0/1 resource.
  //
  // \param res_set_id The Resource Set ID (0 or 1).
  // \param res_set_cfg_id The resource set configuration index.
  // \param pri the index of the resource within the resource set (PUCCH Resource Indicator).
  // \return The ID of the PUCCH resource.
  template <unsigned ResourceSetId>
  pucch_res_id_t harq_res_id(pucch_resource_set_config_id res_set_cfg_id, unsigned pri) const
  {
    static_assert(ResourceSetId == 0 or ResourceSetId == 1, "Only Resource Sets ID 0 and 1 are supported");
    if constexpr (ResourceSetId == 0) {
      ocudu_assert(res_set_cfg_id.value() < nof_cell_res_set_configs,
                   "Resource set config index={} exceeds configured number of resource set configs={}",
                   res_set_cfg_id.value(),
                   nof_cell_res_set_configs);
      ocudu_assert(pri < res_set_size.value(),
                   "Resource index={} exceeds configured resource set size={}",
                   pri,
                   res_set_size.value());
      return pucch_res_id_t::make_ded(res_set_cfg_id.value() * res_set_size.value() + pri, pri);
    }
    ocudu_assert(res_set_cfg_id.value() < nof_cell_res_set_configs,
                 "Resource set config index={} exceeds configured number of resource set configs={}",
                 res_set_cfg_id.value(),
                 nof_cell_res_set_configs);
    ocudu_assert(pri < res_set_size.value(),
                 "Resource index={} exceeds configured resource set size={}",
                 pri,
                 res_set_size.value());
    return pucch_res_id_t::make_ded(nof_cell_res_set_configs * res_set_size.value() + nof_cell_sr_resources +
                                        res_set_cfg_id.value() * res_set_size.value() + pri,
                                    res_set_size.value() + nof_sr_res_per_ue + pri);
  }

  // \brief Get the ID of a given PUCCH resource for SR.
  //
  // \param sr_res_id The SR PUCCH resource configuration index.
  // \return The ID of the PUCCH resource.
  pucch_res_id_t sr_res_id(pucch_sr_resource_id sr_res_id) const
  {
    ocudu_assert(sr_res_id.value() < nof_cell_sr_resources,
                 "SR resource index={} exceeds configured number of SR resources={}",
                 sr_res_id.value(),
                 nof_cell_sr_resources);
    return pucch_res_id_t::make_ded(nof_cell_res_set_configs * res_set_size.value() + sr_res_id.value(),
                                    res_set_size.value());
  }

  // \brief Get the ID of a given PUCCH resource for CSI.
  //
  // \param csi_res_id The CSI PUCCH resource configuration index.
  // \return The ID of the PUCCH resource.
  pucch_res_id_t csi_res_id(pucch_csi_resource_id csi_res_id) const
  {
    ocudu_assert(nof_cell_csi_resources != 0 and csi_res_id.value() < nof_cell_csi_resources,
                 "CSI resource index={} exceeds configured number of CSI resources={}",
                 csi_res_id.value(),
                 nof_cell_csi_resources);
    return pucch_res_id_t::make_ded(nof_cell_res_set_configs * res_set_size.value() + nof_cell_sr_resources +
                                        nof_cell_res_set_configs * res_set_size.value() + csi_res_id.value(),
                                    res_set_size.value() + nof_sr_res_per_ue + res_set_size.value());
  }

  /// \brief Get the ID of the SR_F2 resource associated to a given SR resource.
  ///
  /// \param sr_res_id The SR PUCCH resource configuration index.
  /// \return The ID of the PUCCH resource.
  pucch_res_id_t sr_f2_res_id(pucch_sr_resource_id sr_res_id) const
  {
    ocudu_assert(format_01() == pucch_format::FORMAT_0 and format_234() == pucch_format::FORMAT_2,
                 "SR_F2 resource is only present in the F0+F2 case");
    return pucch_res_id_t::make_ded(nof_cell_res_set_configs * res_set_size.value() + nof_cell_sr_resources +
                                        nof_cell_res_set_configs * res_set_size.value() + nof_cell_csi_resources +
                                        sr_res_id.value(),
                                    res_set_size.value() + nof_sr_res_per_ue + res_set_size.value() +
                                        (nof_cell_csi_resources != 0 ? nof_csi_res_per_ue : 0U));
  }

  /// \brief Get the ID of the CSI_F0 resource associated to a given CSI resource.
  ///
  /// \param csi_res_id The CSI PUCCH resource configuration index.
  /// \return The ID of the PUCCH resource.
  pucch_res_id_t csi_f0_res_id(pucch_csi_resource_id csi_res_id) const
  {
    ocudu_assert(format_01() == pucch_format::FORMAT_0 and format_234() == pucch_format::FORMAT_2,
                 "CSI_F0 resource is only present in the F0+F2 case");
    return pucch_res_id_t::make_ded(nof_cell_res_set_configs * res_set_size.value() + nof_cell_sr_resources +
                                        nof_cell_res_set_configs * res_set_size.value() + nof_cell_csi_resources +
                                        nof_cell_sr_resources + csi_res_id.value(),
                                    res_set_size.value() + nof_sr_res_per_ue + res_set_size.value() +
                                        nof_csi_res_per_ue + 1U);
  }

  /// Get the number of symbols configured for the Format 0 or 1 resources.
  unsigned nof_syms_01() const
  {
    if (std::holds_alternative<pucch_f0_params>(f0_or_f1_params)) {
      return std::get<pucch_f0_params>(f0_or_f1_params).nof_syms.value();
    }
    return std::get<pucch_f1_params>(f0_or_f1_params).nof_syms.value();
  }
  /// Get the number of symbols configured for the Format 2, 3 or 4 resources.
  unsigned nof_syms_234() const
  {
    if (std::holds_alternative<pucch_f2_params>(f2_or_f3_or_f4_params)) {
      return std::get<pucch_f2_params>(f2_or_f3_or_f4_params).nof_syms.value();
    }
    if (std::holds_alternative<pucch_f3_params>(f2_or_f3_or_f4_params)) {
      return std::get<pucch_f3_params>(f2_or_f3_or_f4_params).nof_syms.value();
    }
    return std::get<pucch_f4_params>(f2_or_f3_or_f4_params).nof_syms.value();
  }

  /// Get the number of RBs configured for the Format 0 or 1 resources.
  unsigned nof_prbs_234() const
  {
    if (std::holds_alternative<pucch_f2_params>(f2_or_f3_or_f4_params)) {
      return std::get<pucch_f2_params>(f2_or_f3_or_f4_params).max_nof_rbs.value();
    }
    if (std::holds_alternative<pucch_f3_params>(f2_or_f3_or_f4_params)) {
      return std::get<pucch_f3_params>(f2_or_f3_or_f4_params).max_nof_rbs.value();
    }
    return pucch_constants::f4::NOF_RBS;
  }

  /// Get the configured maximum number of Format 0 or 1 resources to be multiplexed over the same PRBs and symbols.
  unsigned mux_capacity_01() const
  {
    // [Implementation-defined] We don't set different ICS for Format 0 resources.
    if (std::holds_alternative<pucch_f0_params>(f0_or_f1_params)) {
      return 1U;
    }

    const auto& f1_params = std::get<pucch_f1_params>(f0_or_f1_params);
    return to_uint(f1_params.nof_cyc_shifts) * (f1_params.occ_supported ? pucch_constants::f1::NOF_TD_OCC : 1U);
  }
  /// Get the configured maximum number of Format 2, 3 or 4 resources to be multiplexed over the same PRBs and symbols.
  unsigned mux_capacity_234() const
  {
    // PUCCH Formats 2 and 3 do not have multiplexing capabilities.
    if (not std::holds_alternative<pucch_f4_params>(f2_or_f3_or_f4_params)) {
      return 1U;
    }

    const auto& f4_params = std::get<pucch_f4_params>(f2_or_f3_or_f4_params);
    return f4_params.occ_supported ? static_cast<unsigned>(f4_params.occ_length) : 1U;
  }

  /// Get whether intraslot frequency hopping is configured for the Format 0 or 1 resources.
  bool intraslot_freq_hopping_01() const
  {
    if (std::holds_alternative<pucch_f0_params>(f0_or_f1_params)) {
      return std::get<pucch_f0_params>(f0_or_f1_params).intraslot_freq_hopping;
    }
    return std::get<pucch_f1_params>(f0_or_f1_params).intraslot_freq_hopping;
  }
  /// Get whether intraslot frequency hopping is configured for the Format 2, 3 or 4 resources.
  bool intraslot_freq_hopping_234() const
  {
    if (std::holds_alternative<pucch_f2_params>(f2_or_f3_or_f4_params)) {
      return std::get<pucch_f2_params>(f2_or_f3_or_f4_params).intraslot_freq_hopping;
    }
    if (std::holds_alternative<pucch_f3_params>(f2_or_f3_or_f4_params)) {
      return std::get<pucch_f3_params>(f2_or_f3_or_f4_params).intraslot_freq_hopping;
    }
    return std::get<pucch_f4_params>(f2_or_f3_or_f4_params).intraslot_freq_hopping;
  }

  /// Get the maximum effective code rate that can be achieved with the PUCCH Format 2, 3 or 4 resources.
  max_pucch_code_rate max_code_rate_234() const
  {
    if (std::holds_alternative<pucch_f2_params>(f2_or_f3_or_f4_params)) {
      return std::get<pucch_f2_params>(f2_or_f3_or_f4_params).max_code_rate;
    }
    if (std::holds_alternative<pucch_f3_params>(f2_or_f3_or_f4_params)) {
      return std::get<pucch_f3_params>(f2_or_f3_or_f4_params).max_code_rate;
    }
    return std::get<pucch_f4_params>(f2_or_f3_or_f4_params).max_code_rate;
  }

  /// Get the maximum number of UCI bits that can be carried by the PUCCH Format 2, 3 or 4 resources.
  unsigned max_payload_234() const
  {
    if (std::holds_alternative<pucch_f2_params>(f2_or_f3_or_f4_params)) {
      const auto& f2_params = std::get<pucch_f2_params>(f2_or_f3_or_f4_params);
      return get_pucch_format2_max_payload(
          f2_params.max_nof_rbs.value(), f2_params.nof_syms.value(), to_float(f2_params.max_code_rate));
    }
    if (std::holds_alternative<pucch_f3_params>(f2_or_f3_or_f4_params)) {
      const auto& f3_params = std::get<pucch_f3_params>(f2_or_f3_or_f4_params);
      return get_pucch_format3_max_payload(f3_params.max_nof_rbs.value(),
                                           f3_params.nof_syms.value(),
                                           to_float(f3_params.max_code_rate),
                                           f3_params.intraslot_freq_hopping,
                                           f3_params.additional_dmrs,
                                           f3_params.pi2_bpsk);
    }
    const auto& f4_params = std::get<pucch_f4_params>(f2_or_f3_or_f4_params);
    return get_pucch_format4_max_payload(f4_params.nof_syms.value(),
                                         to_float(f4_params.max_code_rate),
                                         f4_params.intraslot_freq_hopping,
                                         f4_params.additional_dmrs,
                                         f4_params.pi2_bpsk,
                                         f4_params.occ_length);
  }
};

} // namespace ocudu
