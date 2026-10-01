// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/slotted_vector.h"
#include "ocudu/ran/bwp/bwp_configuration.h"
#include "ocudu/ran/pdcch/aggregation_level.h"
#include "ocudu/ran/pdcch/coreset.h"
#include "ocudu/ran/pdcch/pdcch_constants.h"
#include "ocudu/ran/pdcch/search_space.h"

namespace ocudu {

/// List of CRBs for a given PDCCH candidate.
using crb_index_list      = static_vector<uint16_t, pdcch_constants::MAX_NOF_RB_PDCCH>;
using crb_index_list_span = span<const uint16_t>;

/// Configuration parameters and derived parameters associated with a CORESET.
class sched_coreset_config
{
public:
  sched_coreset_config(const coreset_configuration&                                           cfg_,
                       const std::array<std::vector<crb_index_list>, NOF_AGGREGATION_LEVELS>& ncce_crbs_) :
    cfg_val(cfg_), ncce_crbs(ncce_crbs_)
  {
  }
  sched_coreset_config(pci_t pci, const bwp_configuration& bwp_cfg, const coreset_configuration& cs_cfg);

  /// CORESET identifier.
  coreset_id id() const { return cfg().get_id(); }

  /// Fetch CORESET configuration.
  const coreset_configuration& cfg() const { return cfg_val; }

  /// Fetch list of CRBs for a given PDCCH candidate.
  crb_index_list_span candidate_crbs(uint8_t start_ncce, aggregation_level aggr_lvl) const
  {
    ocudu_sanity_check(start_ncce % to_nof_cces(aggr_lvl) == 0,
                       "The provided ncce is not aligned to the aggregation level");
    const unsigned ncce_idx = start_ncce >> to_aggregation_level_index(aggr_lvl);
    return ncce_crbs[to_aggregation_level_index(aggr_lvl)][ncce_idx];
  }

  bool operator==(const coreset_configuration& cs_cfg) const { return cfg() == cs_cfg; }
  bool operator==(const sched_coreset_config& cs_cfg) const { return cfg() == cs_cfg.cfg(); }

private:
  /// Pointer to the CORESET configuration.
  coreset_configuration cfg_val;

  std::array<std::vector<crb_index_list>, NOF_AGGREGATION_LEVELS> ncce_crbs;
};

/// Configuration parameters and derived parameters associated with a SearchSpace.
class sched_search_space_config
{
public:
  sched_search_space_config(const sched_coreset_config& cs_cfg_, const search_space_configuration& ss_cfg_) :
    cs_cfg_ptr(&cs_cfg_), ss_cfg(ss_cfg_)
  {
  }

  const search_space_id             id() const { return ss_cfg.get_id(); }
  const search_space_configuration& cfg() const { return ss_cfg; }
  const sched_coreset_config&       cs() const { return *cs_cfg_ptr; }

private:
  const sched_coreset_config* cs_cfg_ptr = nullptr;
  search_space_configuration  ss_cfg;
};

/// \brief Configuration of a PDCCH in a given BWP.
/// It may be a PDCCH only composed by common or by common and dedicated parameters.
class sched_pdcch_config
{
public:
  sched_pdcch_config() = default;
  sched_pdcch_config(const slotted_id_vector<coreset_id, const sched_coreset_config*>&           coresets_,
                     const slotted_id_vector<search_space_id, const sched_search_space_config*>& search_spaces_,
                     const std::optional<search_space_id>& sib1_search_space_id     = std::nullopt,
                     const std::optional<search_space_id>& other_si_search_space_id = std::nullopt,
                     const std::optional<search_space_id>& paging_search_space_id   = std::nullopt,
                     const std::optional<search_space_id>& ra_search_space_id       = std::nullopt) :
    cs_list(coresets_),
    ss_list(search_spaces_),
    sib1_ss_id(sib1_search_space_id),
    other_si_ss_id(other_si_search_space_id),
    paging_ss_id(paging_search_space_id),
    ra_ss_id(ra_search_space_id)
  {
  }

  const slotted_id_vector<coreset_id, const sched_coreset_config*>&           coresets() const { return cs_list; }
  const slotted_id_vector<search_space_id, const sched_search_space_config*>& search_spaces() const { return ss_list; }

  const sched_search_space_config* sib1_search_space() const
  {
    return sib1_ss_id.has_value() ? ss_list[*sib1_ss_id] : nullptr;
  }
  const sched_search_space_config* other_si_search_space() const
  {
    return other_si_ss_id.has_value() ? ss_list[*other_si_ss_id] : nullptr;
  }
  const sched_search_space_config* paging_search_space() const
  {
    return paging_ss_id.has_value() ? ss_list[*paging_ss_id] : nullptr;
  }
  const sched_search_space_config* ra_search_space() const
  {
    return ra_ss_id.has_value() ? ss_list[*ra_ss_id] : nullptr;
  }

  bool operator==(const sched_pdcch_config& other) const
  {
    return cs_list == other.cs_list and ss_list == other.ss_list and sib1_ss_id == other.sib1_ss_id and
           other_si_ss_id == other.paging_ss_id and paging_ss_id == other.paging_ss_id and ra_ss_id == other.ra_ss_id;
  }

private:
  /// Coresets configured for a given BWP.
  slotted_id_vector<coreset_id, const sched_coreset_config*> cs_list;

  /// Coresets configured for a given BWP.
  slotted_id_vector<search_space_id, const sched_search_space_config*> ss_list;

  std::optional<search_space_id> sib1_ss_id;
  std::optional<search_space_id> other_si_ss_id;
  std::optional<search_space_id> paging_ss_id;
  std::optional<search_space_id> ra_ss_id;
};

} // namespace ocudu
