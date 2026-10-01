// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/support/rb_allocation.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/resource_allocation/vrb_to_prb.h"

using namespace ocudu;

rb_allocation ocudu::rb_allocation::make_type0(const vrb_bitmap&                               vrb_bitmap,
                                               const std::optional<vrb_to_prb::configuration>& vrb_to_prb_config)
{
  rb_allocation result;

  result.vrb_mask = vrb_bitmap;
  if (vrb_to_prb_config.has_value()) {
    result.vrb_to_prb_config = *vrb_to_prb_config;
  }

  return result;
}

rb_allocation ocudu::rb_allocation::make_type1(unsigned                                        rb_start,
                                               unsigned                                        rb_count,
                                               const std::optional<vrb_to_prb::configuration>& vrb_to_prb_config)
{
  rb_allocation result;

  result.vrb_mask = vrb_bitmap(rb_start + rb_count);
  result.vrb_mask.fill(rb_start, rb_start + rb_count);
  if (vrb_to_prb_config.has_value()) {
    result.vrb_to_prb_config = *vrb_to_prb_config;
  }

  return result;
}

rb_allocation ocudu::rb_allocation::make_custom(std::initializer_list<const unsigned>           vrb_indexes,
                                                const std::optional<vrb_to_prb::configuration>& vrb_to_prb_config)
{
  rb_allocation result;

  // Find the maximum VRB index.
  unsigned rb_end = *std::max_element(vrb_indexes.begin(), vrb_indexes.end());

  // Allocate the VRB mask so the maximum index fits.
  result.vrb_mask = vrb_bitmap(rb_end + 1);
  for (unsigned vrb_index : vrb_indexes) {
    result.vrb_mask.set(vrb_index, true);
  }
  if (vrb_to_prb_config.has_value()) {
    result.vrb_to_prb_config = *vrb_to_prb_config;
  }

  return result;
}

crb_bitmap rb_allocation::get_crb_mask(unsigned bwp_start_rb, unsigned bwp_size_rb) const
{
  ocudu_assert(is_bwp_valid(bwp_start_rb, bwp_size_rb),
               "Invalid BWP configuration {}:{} for a VRB mask of size {}.",
               bwp_start_rb,
               bwp_size_rb,
               vrb_mask.size());

  if (vrb_to_prb_config.is_interleaved()) {
    vrb_to_prb::interleaved_mapping map(vrb_to_prb_config);
    return map.vrb_to_crb(bwp_start_rb, bwp_size_rb, vrb_mask);
  }
  vrb_to_prb::non_interleaved_mapping map(vrb_to_prb_config);
  return map.vrb_to_crb(bwp_start_rb, bwp_size_rb, vrb_mask);
}

static_vector<uint16_t, MAX_NOF_PRBS> rb_allocation::get_crb_indices(unsigned bwp_start_rb, unsigned bwp_size_rb) const
{
  ocudu_assert(is_bwp_valid(bwp_start_rb, bwp_size_rb),
               "Invalid BWP configuration {}:{} for a VRB mask of size {}.",
               bwp_start_rb,
               bwp_size_rb,
               vrb_mask.size());

  if (vrb_to_prb_config.is_interleaved()) {
    vrb_to_prb::interleaved_mapping map(vrb_to_prb_config);
    return map.vrb_to_crb_indices(bwp_start_rb, bwp_size_rb, vrb_mask);
  }
  vrb_to_prb::non_interleaved_mapping map(vrb_to_prb_config);
  return map.vrb_to_crb_indices(bwp_start_rb, bwp_size_rb, vrb_mask);
}
