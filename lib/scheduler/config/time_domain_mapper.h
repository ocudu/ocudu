// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/pdcch/dci_format.h"
#include "ocudu/ran/pdsch/pdsch_time_domain_resource.h"
#include "ocudu/ran/pusch/pusch_constants.h"
#include "ocudu/ran/pusch/pusch_time_domain_resource.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"
#include "ocudu/scheduler/config/time_domain_resource_helper.h"
#include <optional>
#include <variant>

namespace ocudu {

/// Parameters for building DL (PDSCH) TD resource info.
struct dl_time_domain_builder_params {
  struct auto_resources {
    /// Max duration in symbols of all CORESETs.
    uint8_t coreset_max_dur = 2;
  };
  struct explicit_resources {
    /// PDSCH TD resource list used with DCI format 1_0, derived from PDSCH-ConfigCommon.
    std::vector<pdsch_time_domain_resource_allocation> common_pdsch_td_res_list;
    /// \brief PDSCH TD resource list used with DCI format 1_1, derived from a UE's dedicated PDSCH-Config.
    /// \remark When empty, DCI format 1_1 falls back to \c common_pdsch_td_res_list.
    std::vector<pdsch_time_domain_resource_allocation> dedicated_pdsch_td_res_list;
  };

  /// Cyclic prefix used in the BWP.
  cyclic_prefix cp = cyclic_prefix::NORMAL;
  /// TDD configuration common to all UEs in the cell/BWP.
  std::optional<tdd_ul_dl_config_common>           tdd_cfg;
  std::variant<auto_resources, explicit_resources> params{auto_resources{}};
};

/// Descriptor of DL (PDSCH) time-domain resources in a given BWP.
struct dl_time_domain_mapper {
  dl_time_domain_mapper() = default;

  /// Build dedicated BWP PDSCH TD resource info from builder parameters.
  explicit dl_time_domain_mapper(const dl_time_domain_builder_params& params);

  /// Retrieve the common (fallback) PDSCH TD resources, used with DCI format 1_0.
  span<const pdsch_time_domain_resource_allocation> common_pdsch_td_resources() const
  {
    return common_pdsch_td_res_list;
  }

  /// \brief Retrieve the dedicated PDSCH TD resources, used with DCI format 1_1.
  /// \remark Falls back to \ref common_pdsch_td_resources() if no dedicated list was configured.
  span<const pdsch_time_domain_resource_allocation> dedicated_pdsch_td_resources() const
  {
    return dedicated_pdsch_td_res_list;
  }

  /// \brief Retrieve the list of available PDSCH time-domain resource allocations applicable for the given DCI DL
  /// format, as per TS38.214, clause 5.1.2.1.
  /// \remark Returns the common (fallback) resources for DCI format 1_0, and the dedicated ones otherwise.
  span<const pdsch_time_domain_resource_allocation> pdsch_td_resources(dci_dl_format dci_format) const
  {
    return dci_format == dci_dl_format::f1_0 ? common_pdsch_td_resources() : dedicated_pdsch_td_resources();
  }

  /// \brief Get the list of indices into \ref common_pdsch_td_resources() that are applicable PDSCH TD resource
  /// candidates for a PDSCH scheduled by a PDCCH in the given slot index.
  /// \remark Each resource ends at the last DL symbol of the slot: the number of symbols per slot in a full DL slot,
  /// or the last DL symbol in a special slot.
  span<const uint8_t> common_pdsch_td_res_indices(unsigned pdcch_slot_index) const
  {
    return common_pdsch_td_res_indices_per_slot[pdcch_slot_index % common_pdsch_td_res_indices_per_slot.size()];
  }

  /// \brief Get the list of indices into \ref dedicated_pdsch_td_resources() that are applicable PDSCH TD resource
  /// candidates for a PDSCH scheduled by a PDCCH in the given slot index.
  span<const uint8_t> dedicated_pdsch_td_res_indices(unsigned pdcch_slot_index) const
  {
    return dedicated_pdsch_td_res_indices_per_slot[pdcch_slot_index % dedicated_pdsch_td_res_indices_per_slot.size()];
  }

  /// \brief Get the list of indices into \ref pdsch_td_resources(dci_dl_format) const that are applicable PDSCH TD
  /// resource candidates for a PDSCH scheduled by a PDCCH in the given slot index, for the given DCI DL format.
  span<const uint8_t> pdsch_td_res_indices(dci_dl_format dci_format, unsigned pdcch_slot_index) const
  {
    return dci_format == dci_dl_format::f1_0 ? common_pdsch_td_res_indices(pdcch_slot_index)
                                             : dedicated_pdsch_td_res_indices(pdcch_slot_index);
  }

  /// \brief Get the index into \ref pdsch_td_resources(dci_dl_format) const of the best-matching PDSCH TD resource
  /// candidate for a PDCCH in \c pdcch_slot, whose k0 leads to \c pdsch_slot and whose symbols are fully contained
  /// within \c usable_symbols. "Best" means the candidate with the largest \c symbols.length() among those that
  /// qualify; ties keep the first one encountered.
  /// \return The matching index, or \c std::nullopt if no candidate qualifies.
  std::optional<uint8_t> find_pdsch_td_res_index(dci_dl_format     dci_format,
                                                 slot_point        pdcch_slot,
                                                 slot_point        pdsch_slot,
                                                 ofdm_symbol_range usable_symbols) const;

private:
  /// \brief Common (fallback) PDSCH time-domain resource allocations for the BWP, used with DCI format 1_0.
  std::vector<pdsch_time_domain_resource_allocation> common_pdsch_td_res_list;

  /// \brief Dedicated PDSCH time-domain resource allocations for the BWP, used with DCI format 1_1. Falls back to
  /// \c common_pdsch_td_res_list if no dedicated list was configured.
  std::vector<pdsch_time_domain_resource_allocation> dedicated_pdsch_td_res_list;

  /// List of indices into \c common_pdsch_td_res_list applicable for a PDSCH scheduled in each slot within the TDD
  /// period.
  std::vector<std::vector<uint8_t>> common_pdsch_td_res_indices_per_slot;

  /// List of indices into \c dedicated_pdsch_td_res_list applicable for a PDSCH scheduled in each slot within the
  /// TDD period.
  std::vector<std::vector<uint8_t>> dedicated_pdsch_td_res_indices_per_slot;
};

/// Parameters for building UL (PUSCH and PUCCH dl-DataToUL-ACK / k1) TD resource info.
struct ul_time_domain_builder_params {
  struct pusch_auto_resources {
    /// Minimum k2 value to consider when generating the list of PUSCH time-domain resource allocations.
    uint8_t min_k2 = 4;
    /// Maximum number of symbols an SRS resource can occupy at the end of the slot. Zero disables SRS-aware generation.
    uint8_t max_srs_symbols = 0;
    /// Number of symbols of a single SRS resource.
    uint8_t symbols_per_srs = 0;
  };
  struct pusch_explicit_resources {
    std::vector<pusch_time_domain_resource_allocation> pusch_td_res_list;
  };
  struct pucch_auto_resources {
    /// Minimum k1 value to consider when generating the list of k1 values.
    uint8_t min_k1 = 4;
  };
  struct pucch_explicit_resources {
    std::vector<uint8_t> k1_candidates;
  };

  /// SCS used in the BWP.
  subcarrier_spacing scs = subcarrier_spacing::kHz15;
  /// Cyclic prefix used in the BWP.
  cyclic_prefix cp = cyclic_prefix::NORMAL;
  /// TDD configuration common to all UEs in the cell/BWP.
  std::optional<tdd_ul_dl_config_common>                       tdd_cfg;
  std::variant<pusch_auto_resources, pusch_explicit_resources> pusch_params{pusch_auto_resources{}};
  std::variant<pucch_auto_resources, pucch_explicit_resources> pucch_params{pucch_auto_resources{}};
};

/// Descriptor of UL (PUSCH and PUCCH dl-DataToUL-ACK / k1) time-domain resources in a given BWP.
struct ul_time_domain_mapper {
  ul_time_domain_mapper() = default;

  /// Build dedicated BWP UL TD resource info from builder parameters.
  explicit ul_time_domain_mapper(const ul_time_domain_builder_params& params);

  /// Minimum k1 used to derive k1 candidates.
  uint8_t min_k1() const { return min_k1_val; }

  /// Retrieve the list of available PUSCH time-domain resource allocations for the BWP.
  span<const pusch_time_domain_resource_allocation> pusch_td_resources() const { return pusch_td_res_list; }

  /// \brief Get the list of indices into \ref pusch_td_resources() that are applicable PUSCH TD resource candidates for
  /// a given PDCCH slot index.
  span<const uint8_t> pusch_td_res_indices(unsigned pdcch_slot_index) const
  {
    return pusch_td_res_indices_per_slot[pdcch_slot_index % pusch_td_res_indices_per_slot.size()];
  }

  /// \brief Get the index into \ref pusch_td_resources() of the best-matching PUSCH TD resource candidate for a PDCCH
  /// in \c pdcch_slot, whose k2 (plus \c ntn_cs_koffset) leads to \c pusch_slot and whose symbols are fully contained
  /// within \c usable_symbols. "Best" means the candidate with the largest \c symbols.length() among those that
  /// qualify; ties keep the first one encountered.
  /// \param retx_symbols For a retransmission, the number of symbols used by the original transmission; only
  /// candidates with a matching \c symbols.length() qualify. Empty for a new transmission.
  /// \return The matching index, or \c std::nullopt if no candidate qualifies.
  std::optional<uint8_t> find_pusch_td_res_index(slot_point             pdcch_slot,
                                                 slot_point             pusch_slot,
                                                 ofdm_symbol_range      usable_symbols,
                                                 unsigned               ntn_cs_koffset,
                                                 std::optional<uint8_t> retx_symbols = std::nullopt) const;

  /// Retrieve the list of k1 candidates for PDSCH-to-HARQ timing used with UE-dedicated DCI.
  span<const uint8_t> dedicated_k1_candidates() const { return dedicated_k1_list; }

  /// \brief Retrieve the k1 candidates for PDSCH-to-HARQ timing appropriate for the given DCI DL format, as per
  /// TS 38.213, clause 9.2.3.
  ///
  /// - For DCI format 1_0, the PDSCH-to-HARQ-timing-indicator field values map to {min_k1, ..., 8}.
  /// - For DCI format 1_1 (if present), they map to the values configured via dl-DataToUL-ACK (Table 9.2.3-1).
  /// \remark Returns the common (fallback) candidates for DCI format 1_0, and the dedicated ones otherwise.
  /// \param dci_format         DCI format that will schedule the PDSCH whose HARQ-ACK is being placed.
  span<const uint8_t> k1_candidates(dci_dl_format dci_format) const
  {
    return dci_format == dci_dl_format::f1_0 ? common_k1_candidates() : dedicated_k1_candidates();
  }

  /// \brief Retrieve the common (fallback) k1 candidates for PDSCH-to-HARQ timing, as per TS38.213, 9.1.2.1.
  /// \remark Unlike \ref common_k1_candidates(unsigned) const, this is not filtered per slot.
  span<const uint8_t> common_k1_candidates() const { return common_k1_list; }

  /// \brief Get the common (fallback) k1 candidates valid for a PDSCH transmitted in the given slot index.
  /// \remark The returned k1 values point to full UL slots where the corresponding HARQ-ACK can be sent.
  span<const uint8_t> common_k1_candidates(unsigned pdsch_slot_index) const
  {
    return common_k1_per_slot[pdsch_slot_index % common_k1_per_slot.size()];
  }

  /// \brief Get the dedicated k1 candidates valid for a PDSCH transmitted in the given slot index.
  /// \remark The returned k1 values point to full UL slots where the corresponding HARQ-ACK can be sent.
  span<const uint8_t> dedicated_k1_candidates(unsigned pdsch_slot_index) const
  {
    return dedicated_k1_per_slot[pdsch_slot_index % dedicated_k1_per_slot.size()];
  }

  /// \brief Retrieve the k1 candidates for PDSCH-to-HARQ timing appropriate for the given DCI DL format in the given
  /// slot index.
  /// \remark Returns the common (fallback) candidates for DCI format 1_0, and the dedicated ones otherwise.
  span<const uint8_t> k1_candidates(dci_dl_format dci_format, unsigned pdsch_slot_index) const
  {
    return dci_format == dci_dl_format::f1_0 ? common_k1_candidates(pdsch_slot_index)
                                             : dedicated_k1_candidates(pdsch_slot_index);
  }

private:
  /// Minimum k1 value used to derive candidates.
  uint8_t min_k1_val = 1;

  /// \brief List of available PUSCH time-domain resource allocations for the BWP.
  /// Max size is pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS.
  std::vector<pusch_time_domain_resource_allocation> pusch_td_res_list;

  /// List of indices into \c pusch_td_res_list applicable for each slot within the TDD period.
  /// Note: Only used when TDD is enabled.
  std::vector<static_vector<uint8_t, pusch_constants::MAX_NOF_PUSCH_TD_RES_ALLOCS>> pusch_td_res_indices_per_slot;

  /// List of k1 candidates for PDSCH-to-HARQ timing used with UE-dedicated DCI.
  static_vector<uint8_t, pucch_td_helper::MAX_K1_CANDIDATES> dedicated_k1_list;

  /// List of common (fallback) k1 candidates for PDSCH-to-HARQ timing, as per TS38.213, 9.1.2.1.
  span<const uint8_t> common_k1_list;

  /// List of common (fallback) k1 candidates valid for a PDSCH transmitted in each slot within the TDD period.
  std::vector<static_vector<uint8_t, pucch_td_helper::MAX_K1_CANDIDATES>> common_k1_per_slot;

  /// List of dedicated k1 candidates valid for a PDSCH transmitted in each slot within the TDD period.
  /// Note: Only used when TDD is enabled.
  std::vector<static_vector<uint8_t, pucch_td_helper::MAX_K1_CANDIDATES>> dedicated_k1_per_slot;
};

} // namespace ocudu
