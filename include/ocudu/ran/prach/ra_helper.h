// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <array>
#include <chrono>

namespace ocudu {

namespace ra_helper {

/// \brief Processing delta (\f$\Delta\f$), as per TS 38.214, Table 6.1.2.1.1-5, indexed by numerology.
/// Used both for Msg3 PUSCH timing (TS 38.213, Section 8.3) and for the successRAR PUCCH HARQ-ACK timing
/// (TS 38.213, Section 8.2A).
/// \param[in] scs Subcarrier spacing.
/// \return Delta, in number of slots.
inline uint8_t get_pusch_delay_delta(subcarrier_spacing scs)
{
  // In TS 38.214, Table 6.1.2.1.1-5, Delta is only defined for SCS within [kHz15, kHz120].
  ocudu_sanity_check(to_numerology_value(scs) <= to_numerology_value(subcarrier_spacing::kHz120),
                     "Subcarrier spacing not supported for Delta lookup");

  // The array represents Table 6.1.2.1.1-5, in TS 38.214.
  static constexpr std::array<uint8_t, 4> DELTAS{2, 3, 4, 6};
  return DELTAS[to_numerology_value(scs)];
}

/// Get Msg3 Delay when PDCCH SCS is the same as PUSCH SCS, as per TS 38.214.
/// \param[in] pdcch_and_pusch_scs SCS used by PDCCH and PUSCH.
/// \param[in] k2 delay used for a given PUSCH.
/// \return Msg3 delay in number of slots.
inline uint8_t get_msg3_delay(subcarrier_spacing pdcch_and_pusch_scs, uint8_t k2)
{
  // The MSG3 slot is defined as MSG3_slot = floor( n * (2^(mu_PUSCH) ) / (2^(mu_PDCCH) ) ) + k2 + Delta.
  // Given the assumption mu_PUSCH == mu_PDCCH, MSG3_delay simplifies to MSG3_delay =  k2 + Delta
  // [TS 38.214, Section 6.1.2.1 and 6.1.2.1.1].
  return k2 + get_pusch_delay_delta(pdcch_and_pusch_scs);
}

/// \brief Computes the RA-RNTI based on PRACH parameters, as per TS 38.321, Section 5.1.3.
/// \param[in] slot_index Index of the first slot of the PRACH occasion in a system frame. Values {0,...,79}.
/// \param[in] symbol_index Index of the first OFDM symbol of the first PRACH occasion. Values {0,...,13}.
/// \param[in] frequency_index Index of the PRACH occation in the frequency domain. Values {0,...,7}.
/// \param[in] is_sul true is this is SUL carrier, false otherwise.
/// \return The value for RA-RNTI.
inline rnti_t get_ra_rnti(unsigned slot_index, unsigned symbol_index, unsigned frequency_index, bool is_sul = false)
{
  // RA-RNTI = 1 + s_id + 14 × t_id + 14 × 80 × f_id + 14 × 80 × 8 × ul_carrier_id.
  // s_id = index of the first OFDM symbol of the (first, for short formats) PRACH occasion (0 <= s_id < 14).
  // t_id = index of the first slot of the PRACH occasion in a system frame (0 <= t_id < 80); the numerology of
  // reference for t_id is 15kHz for long PRACH Formats, regardless of the SCS common; whereas, for short PRACH formats,
  // it coincides with SCS common (this can be inferred from Section 5.1.3, TS 38.321, and from Section 5.3.2,
  // TS 38.211).
  // f_id = index of the PRACH occation in the freq domain (0 <= f_id < 8).
  // ul_carrier_id = 0 for NUL and 1 for SUL carrier.
  const uint16_t ra_rnti =
      1U + symbol_index + 14U * slot_index + 14U * 80U * frequency_index + (14U * 80U * 8U * (is_sul ? 1U : 0U));
  return to_rnti(ra_rnti);
}

/// \brief Computes the MSGB-RNTI based on PRACH parameters, as per TS 38.321, Section 5.1.4.
inline rnti_t get_msgb_rnti(unsigned slot_index, unsigned symbol_index, unsigned frequency_index, bool is_sul = false)
{
  // MSGB-RNTI = 1 + s_id + 14 × t_id + 14 × 80 × f_id + 14 × 80 × 8 × ul_carrier_id + 14 × 80 × 8 × 2.
  const uint16_t msgb_rnti = 1U + symbol_index + 14U * slot_index + 14U * 80U * frequency_index +
                             (14U * 80U * 8U * (is_sul ? 1U : 0U)) + 14 * 80 * 8 * 2;
  return to_rnti(msgb_rnti);
}

/// Checks whether the RA-RNTI is within its possible bounds as per TS 38.321, 5.1.3.
inline bool is_valid_ra_rnti(rnti_t ra_rnti)
{
  return ra_rnti >= rnti_t::MIN_RA_RNTI and ra_rnti <= rnti_t::MAX_RA_RNTI;
}

/// Checks whether the MSGB-RNTI is within its possible bounds as per TS 38.321, 5.1.4.
inline bool is_valid_msgb_rnti(rnti_t msgb_rnti)
{
  return msgb_rnti >= rnti_t::MIN_MSGB_RNTI and msgb_rnti <= rnti_t::MAX_MSGB_RNTI;
}

/// \brief Valid Backoff Indicator (BI) duration values, as per TS 38.321, Table 7.2-1.
inline span<const std::chrono::milliseconds> get_backoff_indicator_values()
{
  static constexpr std::array<std::chrono::milliseconds, 14> values = {std::chrono::milliseconds{5},
                                                                       std::chrono::milliseconds{10},
                                                                       std::chrono::milliseconds{20},
                                                                       std::chrono::milliseconds{30},
                                                                       std::chrono::milliseconds{40},
                                                                       std::chrono::milliseconds{60},
                                                                       std::chrono::milliseconds{80},
                                                                       std::chrono::milliseconds{120},
                                                                       std::chrono::milliseconds{160},
                                                                       std::chrono::milliseconds{240},
                                                                       std::chrono::milliseconds{320},
                                                                       std::chrono::milliseconds{480},
                                                                       std::chrono::milliseconds{960},
                                                                       std::chrono::milliseconds{1920}};
  return values;
}

} // namespace ra_helper

} // namespace ocudu
