// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/ssb/pbch_encoder.h"
#include "ocudu/phy/upper/channel_processors/ssb/pbch_modulator.h"
#include "ocudu/phy/upper/channel_processors/ssb/ssb_processor.h"
#include "ocudu/phy/upper/signal_processors/ssb/dmrs_pbch_processor.h"
#include "ocudu/phy/upper/signal_processors/ssb/pss_processor.h"
#include "ocudu/phy/upper/signal_processors/ssb/sss_processor.h"
#include "ocudu/support/math/math_utils.h"

namespace ocudu {

struct ssb_processor_config {
  std::unique_ptr<pbch_encoder>        encoder;
  std::unique_ptr<pbch_modulator>      modulator;
  std::unique_ptr<dmrs_pbch_processor> dmrs;
  std::unique_ptr<pss_processor>       pss;
  std::unique_ptr<sss_processor>       sss;
};

/// Implements a parameter validator for \ref ssb_processor_impl.
class ssb_processor_validator_impl : public ssb_pdu_validator
{
public:
  // See interface for documentation.
  error_type<std::string> is_valid(const ssb_processor::pdu_t& pdu, unsigned cell_bandwith_prbs) const override
  {
    // Maximum SSB index allowed for SSB pattern A, B, C.
    static constexpr unsigned ssb_pattern_FR1_max_ssb_idx = 8;

    // Maximum SSB index allowed for SSB pattern D and E.
    static constexpr unsigned ssb_pattern_FR2_max_ssb_idx = 64;

    const frequency_range    fr      = to_frequency_range(pdu.pattern_case);
    const subcarrier_spacing ssb_scs = to_subcarrier_spacing(pdu.pattern_case);

    // Check that the SSB index is valid for the selected pattern in frequency range FR2.
    if ((fr == frequency_range::FR2) && (pdu.ssb_idx >= ssb_pattern_FR2_max_ssb_idx)) {
      return make_unexpected(fmt::format("SSB index is out of range for SSB pattern {} (i.e., {} vs {}).",
                                         to_string(pdu.pattern_case),
                                         pdu.ssb_idx,
                                         ssb_pattern_FR2_max_ssb_idx));
    }

    // Check that the SSB index is valid for the selected pattern in frequency range FR1.
    if ((fr == frequency_range::FR1) && (pdu.ssb_idx >= ssb_pattern_FR1_max_ssb_idx)) {
      return make_unexpected(fmt::format("SSB index is out of range for SSB pattern {} (i.e., {} vs {}).",
                                         to_string(pdu.pattern_case),
                                         pdu.ssb_idx,
                                         ssb_pattern_FR1_max_ssb_idx));
    }

    // Check that SSB subcarrier spacing and common subcarrier spacing are valid for the selected frequency range.
    if (((fr == frequency_range::FR1) && (ssb_scs > subcarrier_spacing::kHz30)) ||
        ((fr == frequency_range::FR2) && (ssb_scs <= subcarrier_spacing::kHz60))) {
      return make_unexpected(fmt::format("The SSB SCS (i.e., {} kHz) is not supported in FR{}.",
                                         scs_to_khz(ssb_scs),
                                         (fr == frequency_range::FR1) ? 1 : 2));
    }

    if ((pdu.common_scs == subcarrier_spacing::kHz240) || !is_scs_valid(pdu.common_scs, fr)) {
      return make_unexpected(fmt::format("The common SCS (i.e., {} kHz) is not supported in FR{}.",
                                         scs_to_khz(pdu.common_scs),
                                         (fr == frequency_range::FR1) ? 1 : 2));
    }

    // Check that the offset to PointA in PRBs is within the valid range.
    if (!pdu.offset_to_pointA.valid()) {
      return make_unexpected(fmt::format(
          "Invalid offset to PointA (i.e., {}, max {})", pdu.offset_to_pointA.value(), ssb_offset_to_pointA::max()));
    }

    // Subcarrier spacing used for SSB subcarrier offset.
    unsigned kssb_scs_kHz = scs_to_khz((fr == frequency_range::FR1) ? subcarrier_spacing::kHz15 : pdu.common_scs);
    // Subcarrier spacing used for offset to PointA.
    unsigned pointA_scs_kHz =
        scs_to_khz((fr == frequency_range::FR1) ? subcarrier_spacing::kHz15 : subcarrier_spacing::kHz60);

    // Subcarrier 0 of the common resource block containing the SSB (N_CRB_SSB) must coincide with the subcarrier 0 of a
    // common resource block based on common subcarrier spacing, i.e., the CRB containing the SSB must be
    // aligned with the common subcarrier spacing.
    bool enforce_even = (pdu.common_scs == subcarrier_spacing::kHz30) || (pdu.common_scs == subcarrier_spacing::kHz120);
    if (enforce_even && (pdu.offset_to_pointA.value() % 2 != 0)) {
      return make_unexpected(fmt::format("With common SCS {}kHz, offset to PointA must be even (i.e., {}).",
                                         scs_to_khz(pdu.common_scs),
                                         pdu.offset_to_pointA.value()));
    }

    // The SSB subcarrier offset (k_ssb) maximum value is such that the first SS/PBCH resource block overlaps with the
    // previous CRB, i.e., the CRB based on common subcarrier spacing which is aligned with subcarrier 0 of N_CRB_SSB.
    unsigned k_ssb_max = (fr == frequency_range::FR1)
                             ? (NOF_SUBCARRIERS_PER_RB * pow2(to_numerology_value(pdu.common_scs)) - 1)
                             : (NOF_SUBCARRIERS_PER_RB - 1);
    if (pdu.subcarrier_offset.value() > k_ssb_max) {
      return make_unexpected(fmt::format("With common SCS {}kHz, the maximum SSB subcarrier offset is {} (i.e., {}).",
                                         k_ssb_max,
                                         pdu.subcarrier_offset.value()));
    }

    // SSB first subcarriers in common subcarrier spacing;
    unsigned ssb_start_subc = (pdu.offset_to_pointA.value() * NOF_SUBCARRIERS_PER_RB * pointA_scs_kHz +
                               pdu.subcarrier_offset.value() * kssb_scs_kHz) /
                              scs_to_khz(pdu.common_scs);
    // SSB bandwidth expressed in SSB SCS number of subcarriers.
    unsigned ssb_bw_subc = NOF_SSB_PRBS * NOF_SUBCARRIERS_PER_RB;

    // If using mixed numerology, use the SCS ratio to convert the SSB bandwidth to common SCS subcarriers.
    if (OCUDU_UNLIKELY(ssb_scs != pdu.common_scs)) {
      float scs_ratio = static_cast<float>(scs_to_khz(ssb_scs)) / static_cast<float>(scs_to_khz(pdu.common_scs));
      ssb_bw_subc *= scs_ratio;
    }

    // The combination of the offset to PointA and the SSB subcarrier offset must be such that the SSB fits inside the
    // carrier bandwidth. The carrier bandwidth is expressed in units of PRBs based on common subcarrier spacing. As
    // such, compute the subcarrier based on common SCS at which the SSB ends. This assumes the resource grid
    // start is set at Point A.
    if (ssb_start_subc + ssb_bw_subc > cell_bandwith_prbs * NOF_SUBCARRIERS_PER_RB) {
      return make_unexpected(
          fmt::format("Invalid SSB location: The SSB block spans outside the carrier bandwidth (i.e., {} PRBs).",
                      cell_bandwith_prbs));
    }
    return default_success_t();
  }
};

/// SSB processor implementation.
class ssb_processor_impl : public ssb_processor
{
  std::unique_ptr<pbch_encoder>        encoder;
  std::unique_ptr<pbch_modulator>      modulator;
  std::unique_ptr<dmrs_pbch_processor> dmrs;
  std::unique_ptr<pss_processor>       pss;
  std::unique_ptr<sss_processor>       sss;

public:
  ssb_processor_impl(ssb_processor_config config) :
    encoder(std::move(config.encoder)),
    modulator(std::move(config.modulator)),
    dmrs(std::move(config.dmrs)),
    pss(std::move(config.pss)),
    sss(std::move(config.sss))
  {
    // Do nothing
  }

  void process(resource_grid_writer& grid, const pdu_t& pdu) override;
};

} // namespace ocudu
