// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/bounded_integer.h"
#include "ocudu/ran/nr_band.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/pdcch/pdcch_constants.h"
#include "ocudu/ran/resource_allocation/rb_interval.h"
#include "ocudu/ran/ssb/ssb_properties.h"
#include <optional>
#include <utility>

namespace ocudu {
class ssb_offset_to_pointA;
class ssb_subcarrier_offset;

/// Max number of CORESETs per BWP per cell (including UE-specific and common CORESETs).
/// \remark See TS 38.331, "PDCCH-Config".
constexpr size_t MAX_NOF_CORESETS_PER_BWP = 3;

/// Describes a frequency resource bitmap for CORESET allocation. Each resource represents a group of 6 PRBs.
/// \remark See TS 38.331, "ControlResourceSet"
using freq_resource_bitmap = bounded_bitset<pdcch_constants::MAX_NOF_FREQ_RESOURCES, true>;

/// CORESET#0 index of Table 13-{1, ..., 15}, TS 38.213.
using coreset0_index = bounded_integer<uint8_t, 0, 15>;

/// \brief CORESET identifier. This value is UE-specific, which means that a UE can have up to
/// "maxNrofControlResourceSets" CORESETS configured.
/// \remark See TS 38.331, "maxNrofControlResourceSets".
enum coreset_id : uint8_t { MAX_CORESET_ID = 11, MAX_NOF_CORESETS = 12 };

constexpr coreset_id to_coreset_id(unsigned cs_id)
{
  return static_cast<coreset_id>(cs_id);
}

/// CORESET configuration.
/// \remark See TS 38.331, "ControlResourceSet".
struct coreset_configuration {
  struct interleaved_mapping_type {
    /// Values: (2, 3, 6).
    uint8_t reg_bundle_sz;
    /// Values: (2, 3, 6).
    uint8_t interleaver_sz;
    /// Values: (0..MAX_NOF_PRBS-1).
    uint16_t shift_index;

    bool operator==(const interleaved_mapping_type& rhs) const
    {
      return reg_bundle_sz == rhs.reg_bundle_sz and interleaver_sz == rhs.interleaver_sz and
             shift_index == rhs.shift_index;
    }
  };
  enum class precoder_granularity_type { same_as_reg_bundle, all_contiguous_rbs };

  coreset_configuration() = default;

  /// Constructor for CORESET#0.
  explicit coreset_configuration(nr_band                 band_,
                                 subcarrier_spacing      ssb_scs_,
                                 subcarrier_spacing      pdcch_scs_,
                                 coreset0_index          coreset0_index_,
                                 ssb_subcarrier_offset   k_ssb_,
                                 ssb_offset_to_pointA    offset_to_point_a_,
                                 pci_t                   pci_,
                                 std::optional<unsigned> pdcch_dmrs_scrambling_id_ = std::nullopt);

  /// Constructor for non-CORESET#0.
  explicit coreset_configuration(coreset_id                              id_,
                                 freq_resource_bitmap                    freq_domain_resources_,
                                 unsigned                                duration_,
                                 std::optional<interleaved_mapping_type> interleaved_,
                                 precoder_granularity_type               precoder_granurality_,
                                 std::optional<unsigned>                 pdcch_dmrs_scrambling_id_ = std::nullopt);

  /// \brief Returns CORESET id.
  coreset_id get_id() const { return id; }

  /// \brief Returns CORESET#0 table index, if this configuration corresponds to CORESET#0.
  const std::optional<coreset0_index>& get_coreset0_index() const { return coreset0_idx; }

  /// \brief Returns CORESET duration in number of symbols.
  unsigned duration() const { return dur; }

  /// \brief Returns interleaving configuration, if enabled.
  const std::optional<interleaved_mapping_type>& interleaved_mapping() const { return interleaved; }

  /// \brief Returns precoder granularity.
  precoder_granularity_type get_precoder_granularity() const { return precoder_granurality; }

  /// \brief Returns optional PDCCH DMRS scrambling ID.
  const std::optional<unsigned>& get_pdcch_dmrs_scrambling_id() const { return pdcch_dmrs_scrambling_id; }

  /// \brief Sets duration for coresets other than CORESET#0.
  void set_non_coreset0_duration(uint8_t duration_)
  {
    ocudu_assert(id != to_coreset_id(0), "Duration of CORESET#0 cannot be manually set");
    ocudu_assert(duration_ <= pdcch_constants::MAX_CORESET_DURATION, "Invalid CORESET duration");
    dur = duration_;
  }

  /// \brief Sets interleaving parameters for coresets other than CORESET#0.
  void set_non_coreset0_interleaved(std::optional<interleaved_mapping_type> interleaved_)
  {
    ocudu_assert(id != to_coreset_id(0), "Cannot manually set interlaved mapping of CORESET#0");
    interleaved = std::move(interleaved_);
  }

  /// \brief Sets precoder granularity for coresets other than CORESET#0.
  void set_non_coreset0_precoder_granularity(precoder_granularity_type precoder_granularity_)
  {
    ocudu_assert(id != to_coreset_id(0), "Cannot manually set precoder granularity of CORESET#0");
    precoder_granurality = precoder_granularity_;
  }

  /// \brief Sets PDCCH DMRS scrambling ID for coresets other than CORESET#0.
  void set_non_coreset0_pdcch_dmrs_scrambling_id(std::optional<unsigned> pdcch_dmrs_scrambling_id_)
  {
    ocudu_assert(id != to_coreset_id(0), "Cannot manually set DMRS scrambling ID of CORESET#0");
    pdcch_dmrs_scrambling_id = pdcch_dmrs_scrambling_id_;
  }

  /// Retrieve the (contiguous) CRBs of the CORESET, in case of CORESET#0.
  const crb_interval& coreset0_crbs() const
  {
    ocudu_assert(id == to_coreset_id(0), "Invalid access to CORESET#0 RBs for a coresetId>0");
    return coreset0_rbs;
  }

  void set_freq_domain_resources(freq_resource_bitmap res_bitmap)
  {
    ocudu_assert(id != to_coreset_id(0), "Cannot manually set frequency domain resource bitmap for CORESET#0");
    other_coreset_freq_resources = res_bitmap;
  }
  const freq_resource_bitmap& freq_domain_resources() const
  {
    ocudu_assert(id != to_coreset_id(0), "Invalid access to CORESET#0 frequency resource bitmap");
    return other_coreset_freq_resources;
  }

  bool operator==(const coreset_configuration& rhs) const
  {
    if (std::tie(id, dur, interleaved, precoder_granurality, pdcch_dmrs_scrambling_id) !=
        std::tie(rhs.id, rhs.dur, rhs.interleaved, rhs.precoder_granurality, rhs.pdcch_dmrs_scrambling_id)) {
      return false;
    }
    if (id == to_coreset_id(0)) {
      return (coreset0_crbs() == rhs.coreset0_crbs());
    }
    return (freq_domain_resources() == rhs.freq_domain_resources());
  }

  /// Computes \f$N_{CCE}\f$, as per TS 38.213, Section 9.2.1, which is the number of CCEs in a CORESET of a PDCCH
  /// reception with DCI format 1_0 or DCI format 1_1.
  unsigned get_nof_cces() const
  {
    // For \c frequencyDomainResources, TS 38.331, each 1 in the bitset corresponds to 6 RBs.
    unsigned nof_rbs = id == to_coreset_id(0)
                           ? coreset0_rbs.length()
                           : (other_coreset_freq_resources.count() * pdcch_constants::NOF_RB_PER_FREQ_RESOURCE);
    // A CCE corresponds to 6 REGs, where a REG is 1 RB x 1 symbol.
    return (nof_rbs * dur) / pdcch_constants::NOF_REG_PER_CCE;
  }

  /// Computes the lowest RB used by the CORESET.
  unsigned get_coreset_start_crb() const
  {
    static constexpr unsigned NOF_RBS_PER_GROUP = 6U;
    if (id == to_coreset_id(0)) {
      return coreset0_crbs().start();
    }
    const uint64_t lowest_bit = other_coreset_freq_resources.find_lowest(0, other_coreset_freq_resources.size());
    return lowest_bit * NOF_RBS_PER_GROUP;
  }

private:
  coreset_id id = to_coreset_id(1);
  /// Duration in number of symbols. Values: (1..maxCORESETDuration).
  uint8_t                                 dur = 0;
  std::optional<interleaved_mapping_type> interleaved;
  precoder_granularity_type               precoder_granurality = precoder_granularity_type::same_as_reg_bundle;
  /// PDCCH DMRS scrambling initialization. Values: (0..65535).
  std::optional<unsigned> pdcch_dmrs_scrambling_id;

  crb_interval                  coreset0_rbs;
  std::optional<coreset0_index> coreset0_idx;
  freq_resource_bitmap          other_coreset_freq_resources;
};

} // namespace ocudu
