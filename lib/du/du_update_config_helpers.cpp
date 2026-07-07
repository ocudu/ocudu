// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/du/du_update_config_helpers.h"
#include "ocudu/scheduler/config/pucch_resource_generator.h"
#include "ocudu/scheduler/config/sched_cell_config_helpers.h"
#include "ocudu/scheduler/config/srs_builder_params.h"

using namespace ocudu;
using namespace config_helpers;

/// Helper function that, for a given PUCCH resource, returns the (inner) interval of PRBs that are not used for the
/// PUCCH resource. We define "inner interval" as the interval of PRBs spanning through the center of the BWP. This
/// function assumes that (i) the PUCCH resources are located in 2 separate blocks, at both external sides of the BWP,
/// and that (ii) a PUCCH resource can be at the same time on both of these sides (i.e., inter-slot frequency hopping).
static prb_interval find_pucch_inner_prbs(const pucch_resource& res, unsigned bwp_size)
{
  // Return true if the given PRB is on the BWP's left side (i.e., if the PRB index is less than the BWP's size measured
  // in PRBs).
  auto is_on_bwp_left_side = [bwp_size](unsigned prb) { return prb < bwp_size / 2; };
  // Return true if the given PRB is on the BWP's right side (approx., if the PRB index is more than half the BWP's size
  // measured in PRBs).
  // NOTE: for odd bwp_size and the for central PRB, both is_on_bwp_right_side() and is_on_bwp_left_side() are false.
  auto is_on_bwp_right_side = [bwp_size](unsigned prb) { return prb >= bwp_size - bwp_size / 2; };

  unsigned max_rb_idx_on_left_side  = 0;
  unsigned min_rb_idx_on_right_side = bwp_size;

  auto update_limits = [&](prb_interval hop) {
    if (is_on_bwp_left_side(hop.stop())) {
      max_rb_idx_on_left_side = std::max(hop.stop(), max_rb_idx_on_left_side);
    }
    if (is_on_bwp_right_side(hop.start())) {
      min_rb_idx_on_right_side = std::min(hop.start(), min_rb_idx_on_right_side);
    }
  };
  const prb_interval prbs = res.prbs();
  update_limits(prbs);
  if (res.second_hop_prb.has_value()) {
    update_limits(prb_interval::start_and_len(*res.second_hop_prb, prbs.length()));
  }

  return prb_interval{max_rb_idx_on_left_side, min_rb_idx_on_right_side};
}

prb_interval config_helpers::find_largest_prb_interval_without_pucch(const pucch_resource_builder_params& user_params,
                                                                     unsigned                             bwp_size)
{
  // Compute the cell PUCCH resource list, depending on which parameter that has been passed.
  const std::vector<pucch_resource>& res_list = generate_cell_pucch_res_list(user_params, bwp_size);

  prb_interval prb_without_pucch = {0, bwp_size};

  for (const auto& pucch_res : res_list) {
    prb_interval inner_prbs = find_pucch_inner_prbs(pucch_res, bwp_size);
    prb_without_pucch.set(std::max(prb_without_pucch.start(), inner_prbs.start()),
                          std::min(prb_without_pucch.stop(), inner_prbs.stop()));
  }
  return prb_without_pucch;
}

unsigned config_helpers::compute_prach_frequency_start(const pucch_resource_builder_params& user_params,
                                                       unsigned                             bwp_size,
                                                       bool                                 is_long_prach)
{
  // This is to preserve a guardband between the PUCCH and PRACH.
  const unsigned pucch_to_prach_guardband = is_long_prach ? 0U : 3U;
  return find_largest_prb_interval_without_pucch(user_params, bwp_size).start() + pucch_to_prach_guardband;
}

void config_helpers::compute_nof_sr_csi_pucch_res(pucch_resource_builder_params& user_params,
                                                  unsigned                       max_pucch_grants_per_slot,
                                                  float                          sr_period_msec,
                                                  std::optional<unsigned>        csi_period_msec)
{
  // [Implementation-defined] In the following, we compute the estimated number of PUCCH resources that are needed for
  // SR and CSI; we assume we cannot allocate more than max_pucch_grants_per_slot - 1U (1 is reserved for HARQ-ACK)
  // overall SR and CSI per slot, and the required resources are weighted based on CSI and SR period, respectively
  // (i.e., if the SR period is half of the CSI's, we allocate twice the resources to SR).
  // If the CSI is not enabled, we only allocate resources for SR.

  const unsigned max_pucch_grants_per_sr_csi = max_pucch_grants_per_slot - 1U;

  if (csi_period_msec.has_value()) {
    const unsigned required_nof_sr_resources =
        std::ceil(static_cast<double>(max_pucch_grants_per_sr_csi * csi_period_msec.value()) /
                  (static_cast<double>(sr_period_msec) + static_cast<double>(csi_period_msec.value())));

    user_params.nof_cell_sr_resources = std::min(required_nof_sr_resources, user_params.nof_cell_sr_resources);

    const unsigned required_nof_csi_resources =
        std::ceil(static_cast<double>(max_pucch_grants_per_sr_csi * sr_period_msec) /
                  (static_cast<double>(sr_period_msec) + static_cast<double>(csi_period_msec.value())));

    user_params.nof_cell_csi_resources = std::min(required_nof_csi_resources, user_params.nof_cell_csi_resources);
  } else {
    user_params.nof_cell_sr_resources  = std::min(max_pucch_grants_per_sr_csi, user_params.nof_cell_sr_resources);
    user_params.nof_cell_csi_resources = 0;
  }
}

bounded_integer<unsigned, 1, 14>
config_helpers::compute_max_nof_pucch_symbols(const srs_builder_params& user_srs_params)
{
  // [Implementation-defined] In the following, we compute the maximum number of PUCCH symbols that can be used in a
  // slot based on the PUCCH and SRS configurations. The maximum number of PUCCH symbols is computed so that PUCCH and
  // SRS resources occupy all symbols in a slot and in such a way that they do not overlap each other.
  return user_srs_params.srs_type_enabled != srs_type::disabled
             ? NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - user_srs_params.max_nof_symbols.value()
             : NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
}
