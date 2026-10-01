// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "../support/uplink_cplane_context_repository.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ofh/serdes/ofh_message_decoder_properties.h"
#include <algorithm>

namespace ocudu {
namespace ofh {

/// Returns true if an OFDM symbol index is valid for the given uplink Control-Plane context.
constexpr bool is_symbol_index_in_cplane_valid(const ul_cplane_context& cp_context, unsigned start_symbol)
{
  return (start_symbol >= cp_context.start_symbol) &&
         (start_symbol < (cp_context.start_symbol + cp_context.nof_symbols));
}

/// Returns true if all of the Open Fronthaul User-Plane per section PRB fields are valid, otherwise returns false.
inline bool are_uplane_prb_fields_valid(const uplane_message_decoder_results& results,
                                        const ul_cplane_context&              cp_context,
                                        unsigned                              sector_id,
                                        ocudulog::basic_logger&               logger)
{
  return std::none_of(
      results.sections.begin(),
      results.sections.end(),
      [cp_context, sector_id, &logger](const uplane_section_params& up_section) {
        if (OCUDU_UNLIKELY(up_section.start_prb > MAX_NOF_PRBS - 1)) {
          logger.info(
              "Sector#{}: dropped received Open Fronthaul User-Plane packet as the first PRB index '{}' is not valid",
              sector_id,
              up_section.start_prb);

          return true;
        }

        if (OCUDU_UNLIKELY(up_section.start_prb + up_section.nof_prbs > MAX_NOF_PRBS)) {
          logger.info(
              "Sector#{}: dropped received Open Fronthaul User-Plane packet as the last PRB index '{}' is not valid",
              sector_id,
              up_section.start_prb + up_section.nof_prbs);

          return true;
        }

        if (OCUDU_UNLIKELY(!up_section.is_every_rb_used)) {
          logger.info("Sector#{}: dropped received Open Fronthaul User-Plane packet as 'every other resource block is "
                      "used' mode is not supported",
                      sector_id);

          return true;
        }

        if (OCUDU_UNLIKELY(!up_section.use_current_symbol_number)) {
          logger.info("Sector#{}: dropped received Open Fronthaul User-Plane packet as 'increment the current symbol "
                      "number and use that' mode is not supported",
                      sector_id);

          return true;
        }

        bool is_up_section_not_found_in_cp_section =
            up_section.start_prb < cp_context.prb_start ||
            (up_section.start_prb + up_section.nof_prbs) > (cp_context.prb_start + cp_context.nof_prb);

        if (OCUDU_UNLIKELY(is_up_section_not_found_in_cp_section)) {
          logger.info("Sector#{}: dropped received Open Fronthaul User-Plane packet as PRB index range '{}:{}' does "
                      "not match the expected range '{}:{}'",
                      sector_id,
                      up_section.start_prb,
                      up_section.nof_prbs,
                      cp_context.prb_start,
                      cp_context.nof_prb);
        }
        return is_up_section_not_found_in_cp_section;
      });
}

} // namespace ofh
} // namespace ocudu
