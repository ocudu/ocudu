// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/phy_time_unit.h"
#include "ocudu/ran/prach/prach_format_type.h"
#include "ocudu/ran/prach/prach_subcarrier_spacing.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"

namespace ocudu {

struct prach_configuration;

/// \brief Collects PRACH preamble information parameters.
///
/// The parameters are used and described in TS38.211 Section 6.3.3.1.
struct prach_preamble_information {
  /// Sequence length in frequency domain, parameter \f$L_{RA}\f$.
  unsigned sequence_length;
  /// Parameter \f$\Delta f^{RA}\f$.
  prach_subcarrier_spacing scs;
  /// \brief Number of OFDM symbols.
  ///
  /// It is given by the first multiplier that appears in the \f$N_u\f$ column, in TS38.211 Tables 6.3.3.1-1 and
  /// 6.3.3.1-2.
  unsigned nof_symbols;
  /// Parameter \f$N_{CP}^{RA}\f$. Expressed in units of the reference symbol time \f$\kappa\f$.
  phy_time_unit cp_length;
  /// Flag: true if the preamble supports the restricted sets A and B.
  bool support_restricted_sets;

  /// Parameter \f$N_u\f$, expressed in units of the reference symbol time \f$\kappa\f$.
  phy_time_unit symbol_length() const
  {
    return phy_time_unit::from_seconds(static_cast<double>(nof_symbols) / static_cast<double>(ra_scs_to_Hz(scs)));
  }
};

/// \brief Get long PRACH preamble information as per TS38.211 Table 6.3.3.1-1.
///
/// An assertion is triggered if the PRACH preamble format is short.
///
/// \param[in] format PRACH preamble format.
/// \return PRACH preamble information.
prach_preamble_information get_prach_preamble_long_info(prach_format_type format);

/// \brief Get short PRACH preamble information as per TS38.211 Table 6.3.3.1-2.
///
/// An assertion is triggered if:
/// - the PRACH preamble format is long, or
/// - the random access subcarrier spacing does not satisfy \f$\Delta f ^\textup{RA} = 15 \cdot 2^\mu\f$ where \f$\mu
/// \in \{0, 1, 2, 3\}\f$.
///
/// \param[in] format        PRACH preamble format.
/// \param[in] ra_scs        Random access subcarrier spacing \f$\Delta f ^\textup{RA}\f$.
/// \param[in] last_occasion Set to true if \f$n_\textup{t}^\textup{RA}=N_\textup{t}^\textup{RA,slot}-1\f$
/// \return PRACH preamble information.
prach_preamble_information
get_prach_preamble_short_info(prach_format_type format, prach_subcarrier_spacing ra_scs, bool last_occasion);

/// \brief Gets the PRACH window duration.
/// \param[in] format             PRACH preamble format.
/// \param[in] pusch_scs          Uplink resource grid subcarrier spacing. Determines the numerology \f$\mu\f$.
/// \param[in] start_symbol_index OFDM symbol index within the slot that marks the start of the acquisition window for
///                               the first time-domain PRACH occasion.
/// \param[in] nof_td_occasions   Number of time-domain occasions within a PRACH window.
/// \return The PRACH window duration from the beginning of the slot to the end of the last time-domain occasion.
/// \note The RA subcarrier spacing for short preambles is assumed to be equal to the uplink resource grid subcarrier
///       spacing.
phy_time_unit get_prach_window_duration(prach_format_type  format,
                                        subcarrier_spacing pusch_scs,
                                        unsigned           start_symbol_index,
                                        unsigned           nof_td_occasions);

/// \brief Collects PRACH preamble duration information.
struct prach_symbols_slots_duration {
  /// Duration of the PRACH Preamble in slots, with reference to the PUSCH SCS.
  unsigned prach_length_slots;
  /// PRACH starting slot within the subframe, with reference to the PUSCH SCS. For 15kHz PUSCH SCS, values: {0}; for
  /// 30kHz PUSCH SCS, values: {0, 1}
  unsigned start_slot_pusch_scs;
  /// PRACH duration in symbols, with reference to the PUSCH SCS.
  unsigned nof_symbols;
  /// PRACH starting symbol within the slot, with reference to the PUSCH SCS.
  unsigned start_symbol_pusch_scs;
};

/// \brief Compute PRACH preamble duration information.
///
/// \param[in] prach_cfg PRACH preamble configuration.
/// \param[in] pusch_scs PUSCH SCS that is used as a reference for symbol and slot unit.
/// \return PRACH preamble duration information.
prach_symbols_slots_duration get_prach_duration_info(const prach_configuration& prach_cfg,
                                                     subcarrier_spacing         pusch_scs);

/// \brief Gets the OFDM symbols that a given slot of a PRACH burst occupies.
///
/// A short-preamble burst repeats the same symbols in every slot it spans (1 or 2 slots). A long-preamble burst
/// runs continuously from its starting symbol in the first slot of the burst to the end of the preamble in the last
/// one, filling every slot in between.
///
/// \param[in] duration_info  PRACH preamble duration info, as returned by \ref get_prach_duration_info.
/// \param[in] long_preamble  Whether the PRACH format is long, as per \ref is_long_preamble(prach_format_type).
/// \param[in] burst_slot_idx Index, within the burst, of the slot whose symbols are being queried. Values:
///                           {0, ..., duration_info.prach_length_slots - 1}.
/// \return The OFDM symbols the given slot occupies, as [start, stop), in the slot's own numerology.
ofdm_symbol_range get_prach_burst_slot_symbols(const prach_symbols_slots_duration& duration_info,
                                               bool                                long_preamble,
                                               unsigned                            burst_slot_idx);

} // namespace ocudu
