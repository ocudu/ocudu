// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include "../support/pucch/pucch_default_resource.h"
#include "ocudu/adt/expected.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "ocudu/ran/pucch/pucch_info.h"
#include "ocudu/ran/pucch/pucch_mapping.h"
#include "ocudu/scheduler/config/pucch_resource_builder_params.h"
#include "ocudu/support/math/math_utils.h"
#include "ocudu/support/ocudu_assert.h"
#include "fmt/ranges.h"
#include <algorithm>
#include <variant>

using namespace ocudu;
using namespace config_helpers;

error_type<const char*> config_helpers::pucch_parameters_validator(const pucch_resource_builder_params& params,
                                                                   unsigned                             bwp_size_rbs)
{
  if (params.nof_cell_sr_resources == 0) {
    return make_unexpected("The number of PUCCH SR resources must be greater than zero.");
  }
  if (params.nof_cell_res_set_configs == 0) {
    return make_unexpected("The number of PUCCH resource set configurations must be greater than zero.");
  }

  const unsigned max_nof_symbols = params.max_nof_symbols.value();

  if (params.format_01() == pucch_format::FORMAT_0 and params.format_234() == pucch_format::FORMAT_2 and
      params.res_set_size > 6) {
    return make_unexpected("When using PUCCH Formats 0 and 2, resource set size cannot be greater than 6, as 2 "
                           "resources in each set are reserved.");
  }

  const unsigned nof_res_01 =
      params.nof_cell_sr_resources + params.nof_cell_res_set_configs * params.res_set_size.value();
  unsigned nof_rbs_01;
  if (std::holds_alternative<pucch_f0_params>(params.f0_or_f1_params)) {
    const auto& f0_params = std::get<pucch_f0_params>(params.f0_or_f1_params);
    if (f0_params.intraslot_freq_hopping and f0_params.nof_syms == 1) {
      return make_unexpected("Intra-slot frequency hopping for PUCCH Format 0 requires 2 symbols");
    }

    if (params.format_234() != pucch_format::FORMAT_2) {
      return make_unexpected("PUCCH Formats 3/4 not currently supported with PUCCH Format 0");
    }

    // We define a block as a set of resources of the same format aligned over the same starting PRB.
    const unsigned nof_f0_blocks = max_nof_symbols / f0_params.nof_syms.value();
    nof_rbs_01                   = divide_ceil(nof_res_01, nof_f0_blocks);
  } else {
    const auto& f1_params = std::get<pucch_f1_params>(params.f0_or_f1_params);
    if (f1_params.nof_syms > max_nof_symbols) {
      return make_unexpected("The number of symbols for PUCCH Format 1 exceeds the maximum number of symbols available "
                             "for PUCCH resources");
    }

    const unsigned nof_occ_codes = f1_params.occ_supported ? format1_symb_to_spreading_factor(f1_params.nof_syms) : 1;
    const unsigned nof_css       = to_uint(f1_params.nof_cyc_shifts);
    // We define a block as a set of resources of the same format aligned over the same starting PRB.
    const unsigned nof_f1_blocks = nof_occ_codes * nof_css * (max_nof_symbols / f1_params.nof_syms.value());
    nof_rbs_01                   = divide_ceil(nof_res_01, nof_f1_blocks);
  }
  // With intraslot_freq_hopping, the number of RBs is even. Round up to the nearest even number if it's odd.
  if (params.intraslot_freq_hopping_01() and (nof_rbs_01 & 1U) != 0) {
    nof_rbs_01 += 1;
  }

  const unsigned nof_res_234 =
      params.nof_cell_csi_resources + params.nof_cell_res_set_configs * params.res_set_size.value();
  unsigned nof_rbs_234 = 0;
  if (std::holds_alternative<pucch_f2_params>(params.f2_or_f3_or_f4_params)) {
    const auto& f2_params = std::get<pucch_f2_params>(params.f2_or_f3_or_f4_params);

    if (f2_params.intraslot_freq_hopping and f2_params.nof_syms == 1) {
      return make_unexpected("Intra-slot frequency hopping for PUCCH Format 2 requires 2 symbols");
    }

    const unsigned f2_max_rbs = f2_params.max_payload_bits.has_value()
                                    ? get_pucch_format2_max_nof_prbs(f2_params.max_payload_bits.value(),
                                                                     f2_params.nof_syms.value(),
                                                                     to_float(f2_params.max_code_rate))
                                    : f2_params.max_nof_rbs.value();
    if (f2_max_rbs > pucch_constants::f2::MAX_NOF_RBS) {
      return make_unexpected("The configured maximum number of RBs for PUCCH Format 2 exceeds the limit of 16");
    }

    // We define a block as a set of resources of the same format aligned over the same starting PRB.
    const unsigned nof_f2_blocks = params.max_nof_symbols.value() / f2_params.nof_syms.value();
    nof_rbs_234                  = divide_ceil(nof_res_234, nof_f2_blocks) * f2_max_rbs;
  } else if (std::holds_alternative<pucch_f3_params>(params.f2_or_f3_or_f4_params)) {
    const auto& f3_params = std::get<pucch_f3_params>(params.f2_or_f3_or_f4_params);

    if (f3_params.nof_syms.value() > max_nof_symbols) {
      return make_unexpected("The number of symbols for PUCCH Format 3 exceeds the maximum number of symbols available "
                             "for PUCCH resources");
    }

    const unsigned f3_max_rbs = f3_params.max_payload_bits.has_value()
                                    ? get_pucch_format3_max_nof_prbs(f3_params.max_payload_bits.value(),
                                                                     f3_params.nof_syms.value(),
                                                                     to_float(f3_params.max_code_rate),
                                                                     f3_params.intraslot_freq_hopping,
                                                                     f3_params.additional_dmrs,
                                                                     f3_params.pi2_bpsk)
                                    : f3_params.max_nof_rbs.value();
    if (f3_max_rbs > pucch_constants::f3::MAX_NOF_RBS) {
      return make_unexpected("The number of PRBs for PUCCH Format 3 exceeds the limit of 16");
    }

    // We define a block as a set of resources of the same format aligned over the same starting PRB.
    const unsigned nof_f3_blocks = max_nof_symbols / f3_params.nof_syms.value();
    nof_rbs_234                  = divide_ceil(nof_res_234, nof_f3_blocks) * f3_max_rbs;
  } else {
    const auto& f4_params = std::get<pucch_f4_params>(params.f2_or_f3_or_f4_params);

    const unsigned nof_occs = f4_params.occ_supported ? static_cast<unsigned>(f4_params.occ_length) : 1U;
    // We define a block as a set of resources of the same format aligned over the same starting PRB.
    const unsigned nof_f4_blocks = nof_occs * max_nof_symbols / f4_params.nof_syms.value();
    nof_rbs_234                  = divide_ceil(nof_res_234, nof_f4_blocks);
  }
  // With intraslot_freq_hopping, the number of RBs is even. Round up to the nearest even number if it's odd.
  if (params.intraslot_freq_hopping_234() and (nof_rbs_234 & 1U) != 0) {
    nof_rbs_234 += 1;
  }

  // Verify the number of RBs for the PUCCH resources does not exceed the BWP size.
  // [Implementation-defined] We do not allow the PUCCH resources to occupy more than 50% of the BWP. This is an
  // extreme case, and ideally the PUCCH configuration should result in a much lower PRBs usage.
  static constexpr float max_allowed_rb_usage = 0.5F;
  if (nof_rbs_01 + nof_rbs_234 >= max_allowed_rb_usage * bwp_size_rbs) {
    return make_unexpected("With the given parameters, the number of PRBs for PUCCH exceeds the 50% of the BWP PRBs");
  }

  if (params.harq_ack_rep.has_value()) {
    const auto& rep = params.harq_ack_rep.value();
    if (rep.factors_per_res.size() != params.res_set_size.value()) {
      return make_unexpected(
          "The number of PUCCH HARQ-ACK repetition factors must equal the HARQ-ACK resource set size.");
    }
    if (rep.sinr_thresholds.size() > 3) {
      return make_unexpected("The number of PUCCH HARQ-ACK repetition SINR thresholds cannot exceed 3.");
    }
    const auto max_factor =
        static_cast<unsigned>(*std::max_element(rep.factors_per_res.begin(), rep.factors_per_res.end()));
    const unsigned required_thresholds = log2_ceil(max_factor);
    if (rep.sinr_thresholds.size() < required_thresholds) {
      return make_unexpected("The number of PUCCH HARQ-ACK repetition SINR thresholds is insufficient for the "
                             "configured maximum repetition factor.");
    }
  }
  return {};
}

static bool validate_generated_list(const std::vector<pucch_resource>&   res_list,
                                    const pucch_resource_builder_params& params)
{
  auto           expected_res_by_format = std::array<unsigned, 5>{0, 0, 0, 0, 0};
  const unsigned expected_res_01 =
      params.nof_cell_sr_resources + params.nof_cell_res_set_configs * params.res_set_size.value();
  if (params.format_01() == pucch_format::FORMAT_0) {
    expected_res_by_format[0] = expected_res_01;
    if (params.format_234() == pucch_format::FORMAT_2) {
      expected_res_by_format[0] += params.nof_cell_csi_resources;
    }
  } else {
    expected_res_by_format[1] = expected_res_01;
  }
  const unsigned expected_res_234 =
      params.nof_cell_csi_resources + params.nof_cell_res_set_configs * params.res_set_size.value();
  switch (params.format_234()) {
    case pucch_format::FORMAT_2:
      expected_res_by_format[2] = expected_res_234;
      if (params.format_01() == pucch_format::FORMAT_0) {
        expected_res_by_format[2] += params.nof_cell_sr_resources;
      }
      break;
    case pucch_format::FORMAT_3:
      expected_res_by_format[3] = expected_res_234;
      break;
    case pucch_format::FORMAT_4:
      expected_res_by_format[4] = expected_res_234;
      break;
    default:
      ocudu_assertion_failure("Unexpected PUCCH format for F2/F3/F4 resources");
      break;
  }

  std::array<unsigned, 5> res_count_by_format{0, 0, 0, 0, 0};
  for (const auto& res : res_list) {
    if (res.res_id.ded().cell_res_id >= res_list.size()) {
      ocudu_assertion_failure("Invalid cell resource ID {} in the generated resource list",
                              res.res_id.ded().cell_res_id);
      return false;
    }

    switch (res.format()) {
      case pucch_format::FORMAT_0:
        ++res_count_by_format[0];
        break;
      case pucch_format::FORMAT_1:
        ++res_count_by_format[1];
        break;
      case pucch_format::FORMAT_2:
        ++res_count_by_format[2];
        break;
      case pucch_format::FORMAT_3:
        ++res_count_by_format[3];
        break;
      case pucch_format::FORMAT_4:
        ++res_count_by_format[4];
        break;
      default:
        ocudu_assertion_failure("Invalid PUCCH Format in the generated resource list");
        return false;
        break;
    }
  }
  if (expected_res_by_format != res_count_by_format) {
    ocudu_assertion_failure("The number of PUCCH resources by format in the generated list does not match the expected "
                            "one. Expected: {{{}}}, actual: {{{}}}",
                            fmt::join(expected_res_by_format, ", "),
                            fmt::join(res_count_by_format, ", "));
    return false;
  }
  return true;
}

namespace {

/// Keeps track of the current state of the resource generation.
struct res_gen_state {
  unsigned prb_low_off    = 0;
  unsigned prb_high_off   = 0;
  unsigned start_sym      = 0;
  unsigned mux_idx        = 0;
  bool     high_start_prb = false;
};

/// Responsible for generating PUCCH resources of a given format according to the current state, and advancing the state
/// for the next resource.
class res_gen_cursor
{
public:
  res_gen_cursor(res_gen_state& state_,
                 unsigned       nof_prbs_,
                 unsigned       nof_syms_,
                 unsigned       mux_capacity_,
                 unsigned       max_nof_syms_,
                 bool           intraslot_freq_hop_) :
    nof_prbs(nof_prbs_),
    nof_syms(nof_syms_),
    mux_capacity(mux_capacity_),
    max_nof_syms(max_nof_syms_),
    intraslot_freq_hop(intraslot_freq_hop_),
    state(state_)
  {
  }

  /// Get the PUCCH resource according to the current state.
  pucch_resource get(unsigned bwp_size_rbs) const
  {
    pucch_resource res{};
    // Resource ID will be set later, assign an invalid value to avoid uninitialized variable warnings.
    static constexpr unsigned invalid_res_id = std::numeric_limits<unsigned>::max();
    res.res_id                               = pucch_res_id_t::make_ded(invalid_res_id, invalid_res_id);
    res.syms = {static_cast<uint8_t>(state.start_sym), static_cast<uint8_t>(state.start_sym + nof_syms)};
    const unsigned starting_prb =
        state.high_start_prb ? bwp_size_rbs - state.prb_high_off - nof_prbs : state.prb_low_off;
    res.starting_prb = starting_prb;
    if (intraslot_freq_hop) {
      res.second_hop_prb.emplace(state.high_start_prb ? state.prb_low_off
                                                      : bwp_size_rbs - state.prb_high_off - nof_prbs);
    }
    return res;
  }

  /// \brief Advance the state for the next resource.
  ///
  /// Aiming to:
  /// - Minimize the time-frequency resources used by PUCCH.
  /// - Distribute the resources equally on both ends of the BWP.
  void next()
  {
    if (not advance_mux()) {
      return;
    }

    // Multiplexing capacity for this time-freq block is reached, move to the next time-freq block.
    if (intraslot_freq_hop) {
      advance_time_freq_hop();
    } else {
      advance_time_freq_no_hop();
    }
  }

  // Prepare the state for the use of a different cursor (from a different PUCCH format).
  void close()
  {
    // Increase the RB offsets to ensure the next cursor starts from a different set of PRBs.
    // TODO: This leaves a gap of unused symbols in the current PRB offsets.
    if (intraslot_freq_hop) {
      if (state.mux_idx == 0 and state.start_sym == 0 and not state.high_start_prb) {
        // The current RBs are totally unused, so we can keep the same PRB offsets for the next cursor.
        return;
      }

      state.high_start_prb = false;
      state.prb_low_off += nof_prbs;
      state.prb_high_off += nof_prbs;
    } else {
      if (state.mux_idx == 0 and state.start_sym == 0) {
        // The current RBs are totally unused, so we can keep the same PRB offsets for the next cursor.
        return;
      }

      state.start_sym = 0;
      if (state.high_start_prb) {
        state.prb_high_off += nof_prbs;
      } else {
        state.prb_low_off += nof_prbs;
      }
    }
    state.mux_idx = 0;
  }

private:
  const unsigned nof_prbs;
  const unsigned nof_syms;
  const unsigned mux_capacity;
  const unsigned max_nof_syms;
  const bool     intraslot_freq_hop;

  res_gen_state& state;

  bool advance_mux()
  {
    ++state.mux_idx;
    if (state.mux_idx != mux_capacity) {
      return false;
    }
    state.mux_idx = 0;
    return true;
  }

  void advance_time_freq_hop()
  {
    // With intra-slot frequency hopping enabled, each resource uses both the low and high PRBs.
    // Therefore, we toggle \c high_start_prb before incrementing \c start_sym to minimize symbol usage.
    state.high_start_prb = not state.high_start_prb;
    if (state.high_start_prb) {
      return;
    }

    state.start_sym += nof_syms;
    if (state.start_sym + nof_syms <= max_nof_syms) {
      return;
    }
    state.start_sym = 0;

    state.prb_low_off += nof_prbs;
    state.prb_high_off += nof_prbs;
  }

  void advance_time_freq_no_hop()
  {
    // Without intra-slot frequency hopping, each resource only uses either the low or high PRBs.
    // Therefore, we increment \c start_sym before toggling \c high_start_prb to minimize PRB usage.
    state.start_sym += nof_syms;
    if (state.start_sym + nof_syms <= max_nof_syms) {
      return;
    }
    state.start_sym = 0;

    if (state.high_start_prb) {
      state.prb_high_off += nof_prbs;
    } else {
      state.prb_low_off += nof_prbs;
    }
    state.high_start_prb = not state.high_start_prb;
  }
};

} // namespace
  //

std::array<pucch_resource, pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES>
config_helpers::generate_cell_common_pucch_res_list(unsigned pucch_res_common, unsigned bwp_size_rbs)
{
  const pucch_default_resource default_res = get_pucch_default_resource(pucch_res_common, bwp_size_rbs);
  const unsigned               nof_cs      = default_res.cs_indexes.size();

  std::array<pucch_resource, pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES> out;
  for (unsigned r_pucch = 0; r_pucch != out.size(); ++r_pucch) {
    const auto     prbs = get_pucch_default_prb_index(r_pucch, default_res.rb_bwp_offset, nof_cs, bwp_size_rbs);
    const uint8_t  cs   = default_res.cs_indexes[get_pucch_default_cyclic_shift(r_pucch, nof_cs)];
    pucch_resource res{};
    res.res_id         = pucch_res_id_t::make_cmn(r_pucch);
    res.starting_prb   = prbs.first;
    res.second_hop_prb = prbs.second;
    res.syms           = ofdm_symbol_range::start_and_len(default_res.first_symbol_index, default_res.nof_symbols);
    res.rep_factor     = pucch_repetition_factor::n1;
    switch (default_res.format) {
      case pucch_format::FORMAT_0:
        res.format_params = pucch_resource::f0_config{.initial_cyclic_shift = cs};
        break;
      case pucch_format::FORMAT_1:
        res.format_params = pucch_resource::f1_config{.initial_cyclic_shift = cs, .time_domain_occ = 0};
        break;
      default:
        ocudu_assertion_failure("Unexpected PUCCH format in default common table.");
        break;
    }
    out[r_pucch] = res;
  }
  return out;
}

std::vector<pucch_resource> config_helpers::generate_cell_pucch_res_list(const pucch_resource_builder_params& params,
                                                                         unsigned bwp_size_rbs)
{
  auto outcome = pucch_parameters_validator(params, bwp_size_rbs);
  if (not outcome.has_value()) {
    ocudu_assertion_failure("The cell list could not be generated due to: {}", outcome.error());
    return {};
  }

  const unsigned nof_res_01 =
      params.nof_cell_sr_resources + params.nof_cell_res_set_configs * params.res_set_size.value();
  const unsigned nof_res_234 =
      params.nof_cell_csi_resources + params.nof_cell_res_set_configs * params.res_set_size.value();
  const unsigned nof_res = nof_res_01 + nof_res_234;
  const bool using_02 = params.format_01() == pucch_format::FORMAT_0 and params.format_234() == pucch_format::FORMAT_2;

  std::vector<pucch_resource> resources;
  // For F0+F2, the list contains extra resources for CSI_F0 and SR_F2.
  resources.reserve(nof_res + (using_02 ? params.nof_cell_sr_resources + params.nof_cell_csi_resources : 0U));
  res_gen_state state;

  // Generate F0/F1 resources at the BWP edges.
  // Note: PUCCH Format 0 and 1 always take one PRB.
  static constexpr unsigned nof_prbs_01           = 1U;
  const unsigned            nof_syms_01           = params.nof_syms_01();
  const unsigned            mux_capacity_01       = params.mux_capacity_01();
  const bool                intraslot_freq_hop_01 = params.intraslot_freq_hopping_01();
  res_gen_cursor            cur01(
      state, nof_prbs_01, nof_syms_01, mux_capacity_01, params.max_nof_symbols.value(), intraslot_freq_hop_01);
  for (unsigned i = 0; i != nof_res_01; ++i, cur01.next()) {
    pucch_resource res = cur01.get(bwp_size_rbs);
    if (params.format_01() == pucch_format::FORMAT_0) {
      res.format_params.emplace<pucch_resource::f0_config>(pucch_resource::f0_config{.initial_cyclic_shift = 0U});
    } else {
      // [Implementation-defined] For PUCCH Format 1, the map mux_idx -> (CS, OCC) is defined as follows:
      // - mux_idx = 0, 1, 2, ...                  => (CS=0, OCC=0), (CS=cs_step, OCC=0), (CS=2*cs_step, OCC=0), ...
      // - mux_idx = nof_css, nof_css+1, ...       => (CS=0, OCC=1), (CS=cs_step, OCC=1), (CS=2*cs_step, OCC=1), ...
      // - mux_idx = 2*nof_css, (2*nof_css)+1, ... => (CS=0, OCC=2), (CS=cs_step, OCC=2), (CS=2*cs_step, OCC=2), ...
      // Where cs_step = 12 / nof_css. Note that 12 is divisible by nof_css.
      const auto&               f1_params   = std::get<pucch_f1_params>(params.f0_or_f1_params);
      static constexpr unsigned max_nof_css = to_uint(pucch_nof_cyclic_shifts::twelve);
      const unsigned            cs_step     = max_nof_css / to_uint(f1_params.nof_cyc_shifts);
      res.format_params.emplace<pucch_resource::f1_config>(pucch_resource::f1_config{
          .initial_cyclic_shift = static_cast<uint8_t>((state.mux_idx * cs_step) % max_nof_css),
          .time_domain_occ      = static_cast<uint8_t>(state.mux_idx / to_uint(f1_params.nof_cyc_shifts)),
      });
    }
    resources.emplace_back(res);
  }
  cur01.close();

  const unsigned nof_prbs_234           = params.nof_prbs_234();
  const unsigned nof_syms_234           = params.nof_syms_234();
  const unsigned mux_capacity_234       = params.mux_capacity_234();
  const bool     intraslot_freq_hop_234 = params.intraslot_freq_hopping_234();
  res_gen_cursor cur234(
      state, nof_prbs_234, nof_syms_234, mux_capacity_234, params.max_nof_symbols.value(), intraslot_freq_hop_234);
  for (unsigned i = 0; i != nof_res_234; ++i, cur234.next()) {
    pucch_resource res = cur234.get(bwp_size_rbs);
    if (params.format_234() == pucch_format::FORMAT_2) {
      res.format_params.emplace<pucch_resource::f2_config>(
          pucch_resource::f2_config{.nof_prbs = static_cast<uint8_t>(nof_prbs_234)});
    } else if (params.format_234() == pucch_format::FORMAT_3) {
      const auto& f3_params = std::get<pucch_f3_params>(params.f2_or_f3_or_f4_params);
      res.format_params.emplace<pucch_resource::f3_config>(
          pucch_resource::f3_config{.nof_prbs        = static_cast<uint8_t>(nof_prbs_234),
                                    .pi_2_bpsk       = f3_params.pi2_bpsk,
                                    .additional_dmrs = f3_params.additional_dmrs});
    } else {
      const auto& f4_params = std::get<pucch_f4_params>(params.f2_or_f3_or_f4_params);
      res.format_params.emplace<pucch_resource::f4_config>(
          pucch_resource::f4_config{.occ_index       = static_cast<pucch_f4_occ_idx>(state.mux_idx),
                                    .occ_length      = f4_params.occ_length,
                                    .pi_2_bpsk       = f4_params.pi2_bpsk,
                                    .additional_dmrs = f4_params.additional_dmrs});
    }
    resources.emplace_back(res);
  }

  for (unsigned res_set_cfg_id = 0; res_set_cfg_id != params.nof_cell_res_set_configs; ++res_set_cfg_id) {
    for (unsigned pri = 0; pri != params.res_set_size.value(); ++pri) {
      pucch_res_id_t res_id         = params.harq_res_id<0>(pucch_resource_set_config_id(res_set_cfg_id), pri);
      const unsigned cell_res_id    = res_id.ded().cell_res_id;
      resources[cell_res_id].res_id = res_id;
      if (params.harq_ack_rep.has_value()) {
        resources[cell_res_id].rep_factor = params.harq_ack_rep->factors_per_res[pri];
      }
    }
    for (unsigned pri = 0; pri != params.res_set_size.value(); ++pri) {
      pucch_res_id_t res_id         = params.harq_res_id<1>(pucch_resource_set_config_id(res_set_cfg_id), pri);
      unsigned       cell_res_id    = res_id.ded().cell_res_id;
      resources[cell_res_id].res_id = res_id;
      if (params.harq_ack_rep.has_value()) {
        resources[cell_res_id].rep_factor = params.harq_ack_rep->factors_per_res[pri];
      }
    }
  }
  for (unsigned sr_res_id = 0; sr_res_id != params.nof_cell_sr_resources; ++sr_res_id) {
    pucch_res_id_t res_id      = params.sr_res_id(pucch_sr_resource_id(sr_res_id));
    unsigned       cell_res_id = res_id.ded().cell_res_id;
    auto&          sr_res      = resources[cell_res_id];
    sr_res.res_id              = res_id;
    if (using_02) {
      // Add SR_F2 resource.
      pucch_res_id_t res_id_sr_f2 = params.sr_f2_res_id(pucch_sr_resource_id(sr_res_id));
      resources.emplace_back(pucch_resource{.res_id       = res_id_sr_f2,
                                            .starting_prb = sr_res.starting_prb,
                                            // Must overlap in symbols with the SR resource.
                                            .syms           = sr_res.syms,
                                            .second_hop_prb = sr_res.second_hop_prb,
                                            .format_params  = pucch_resource::f2_config{.nof_prbs = 1U}});
    }
  }
  for (unsigned csi_res_id = 0; csi_res_id != params.nof_cell_csi_resources; ++csi_res_id) {
    pucch_res_id_t res_id      = params.csi_res_id(pucch_csi_resource_id(csi_res_id));
    unsigned       cell_res_id = res_id.ded().cell_res_id;
    auto&          csi_res     = resources[cell_res_id];
    csi_res.res_id             = res_id;
    if (using_02) {
      // Add CSI_F0 resource.
      pucch_res_id_t res_id_csi_f0 = params.csi_f0_res_id(pucch_csi_resource_id(csi_res_id));
      resources.emplace_back(pucch_resource{.res_id       = res_id_csi_f0,
                                            .starting_prb = csi_res.starting_prb,
                                            // Must overlap in symbols with the CSI resource.
                                            .syms           = csi_res.syms,
                                            .second_hop_prb = csi_res.second_hop_prb,
                                            .format_params  = pucch_resource::f0_config{.initial_cyclic_shift = 0U}});
    }
  }

  if (not validate_generated_list(resources, params)) {
    return {};
  }

  return resources;
}
