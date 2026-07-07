// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/pdsch/pdsch_time_domain_resource.h"
#include "ocudu/ran/pusch/pusch_time_domain_resource.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"
#include "ocudu/scheduler/config/time_domain_resource_helper.h"
#include <optional>
#include <variant>

namespace ocudu {

/// Parameters for building TD resource info.
struct bwp_td_res_info_builder_params {
  struct auto_resources {
    /// Max duration in symbols of all CORESETs.
    uint8_t coreset_max_dur = 2;
    /// Minimum k1 value to consider when generating the list of k1 values.
    uint8_t min_k1 = 4;
    /// Minimum k2 value to consider when generating the list of PUSCH time-domain resource allocations.
    uint8_t min_k2 = 4;
    /// Maximum number of symbols an SRS resource can occupy at the end of the slot. Zero disables SRS-aware generation.
    uint8_t max_srs_symbols = 0;
    /// Number of symbols of a single SRS resource.
    uint8_t symbols_per_srs = 0;
  };
  struct explicit_resources {
    std::vector<pdsch_time_domain_resource_allocation> pdsch_td_res_list;
    std::vector<pusch_time_domain_resource_allocation> pusch_td_res_list;
    std::vector<uint8_t>                               k1_candidates;
  };

  /// Cyclic prefix used in the BWP.
  cyclic_prefix cp = cyclic_prefix::NORMAL;
  /// TDD configuration common to all UEs in the cell/BWP.
  std::optional<tdd_ul_dl_config_common>           tdd_cfg;
  std::variant<auto_resources, explicit_resources> params;
};

/// Descriptor of time-domain resources in a given BWP.
struct bwp_td_res_info {
  bwp_td_res_info() = default;

  /// Build dedicated BWP TD resource info from builder parameters.
  explicit bwp_td_res_info(const bwp_td_res_info_builder_params& params);

  /// Retrieve the list of available PUSCH time-domain resource allocations for the BWP.
  span<const pusch_time_domain_resource_allocation> get_pusch_td_res_list() const { return pusch_td_res_list; }

  /// Retrieve the list of k1 candidates for PDSCH-to-HARQ timing used with UE-dedicated DCI.
  span<const uint8_t> get_dedicated_k1_candidates() const { return dedicated_k1_candidates; }

  /// \brief Get the common (fallback) k1 candidates valid for a PDSCH transmitted in the given slot index.
  /// \remark The returned k1 values point to full UL slots where the corresponding HARQ-ACK can be sent.
  span<const uint8_t> get_common_pdsch_k1_candidates(unsigned pdsch_slot_index) const
  {
    return common_k1_per_slot[pdsch_slot_index % common_k1_per_slot.size()];
  }

  /// \brief Get the dedicated k1 candidates valid for a PDSCH transmitted in the given slot index.
  /// \remark The returned k1 values point to full UL slots where the corresponding HARQ-ACK can be sent.
  span<const uint8_t> get_dedicated_pdsch_k1_candidates(unsigned pdsch_slot_index) const
  {
    return dedicated_k1_per_slot[pdsch_slot_index % dedicated_k1_per_slot.size()];
  }

  /// \brief Get the list of PUSCH TD resource candidates for a given slot index.
  span<const pusch_time_domain_resource_allocation> get_pusch_td_res_list(unsigned pdcch_slot_index) const
  {
    return pusch_td_res_per_slot[pdcch_slot_index % pusch_td_res_per_slot.size()];
  }

  /// \brief Get the PDSCH TD resource candidates for a PDSCH scheduled by a PDCCH in the given slot index.
  /// \remark Each resource ends at the last DL symbol of the slot: the number of symbols per slot in a full DL slot,
  /// or the last DL symbol in a special slot.
  span<const pdsch_time_domain_resource_allocation> get_pdsch_td_res_list(unsigned pdcch_slot_index) const
  {
    return pdsch_td_res_per_slot[pdcch_slot_index % pdsch_td_res_per_slot.size()];
  }

private:
  /// \brief List of available PDSCH time-domain resource allocations for the BWP.
  std::vector<pdsch_time_domain_resource_allocation> pdsch_td_res_list;

  /// \brief List of available PUSCH time-domain resource allocations for the BWP.
  /// Max size is pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS.
  std::vector<pusch_time_domain_resource_allocation> pusch_td_res_list;

  /// List of k1 candidates for PDSCH-to-HARQ timing used with UE-dedicated DCI.
  static_vector<uint8_t, time_domain_resource_helper::MAX_K1_CANDIDATES> dedicated_k1_candidates;

  /// List of PUSCH TD resource candidates for each slot within the TDD period.
  /// Note: Only used when TDD is enabled.
  std::vector<std::vector<pusch_time_domain_resource_allocation>> pusch_td_res_per_slot;

  /// List of PDSCH TD resource candidates for a PDSCH scheduled in each slot within the TDD period.
  std::vector<std::vector<pdsch_time_domain_resource_allocation>> pdsch_td_res_per_slot;

  /// List of common (fallback) k1 candidates valid for a PDSCH transmitted in each slot within the TDD period.
  std::vector<static_vector<uint8_t, time_domain_resource_helper::MAX_K1_CANDIDATES>> common_k1_per_slot;

  /// List of dedicated k1 candidates valid for a PDSCH transmitted in each slot within the TDD period.
  /// Note: Only used when TDD is enabled.
  std::vector<static_vector<uint8_t, time_domain_resource_helper::MAX_K1_CANDIDATES>> dedicated_k1_per_slot;
};

} // namespace ocudu
