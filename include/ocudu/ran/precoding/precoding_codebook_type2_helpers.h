// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/precoding/precoding_codebook_configuration.h"
#include "ocudu/ran/precoding/precoding_codebook_properties.h"
#include "ocudu/support/math/pow2_utils.h"
#include "ocudu/support/ocudu_assert.h"
#include <cstdint>

/// \file
/// \brief Precoding Matrix Indicator (PMI) extended information structures and helper functions for the
/// Type II codebook.
///
/// This file contains structures that extend the information from the Type II PMI codebook.

#include "ocudu/ran/precoding/precoding_weight_matrix.h"

namespace ocudu {

/// Maximum number of Type II spatial beams \f$L\f$.
static constexpr unsigned max_nof_typeII_beams = 4;

/// Maximum number of layers of the Type II codebook, as per TS38.214 Section 5.2.2.2.3.
static constexpr unsigned max_nof_typeII_layers = 2;

/// \brief Precoding matrix of a Type II codebook.
///
/// TS38.214 Section 5.2.2.2.3 limits the rank of the codebook to two, so the matrix holds fewer layers than
/// \ref precoding_weight_matrix.
using typeII_precoding_weight_matrix = precoding_weight_matrix_base<max_nof_typeII_layers>;

/// Maximum number of Type II combining coefficients per layer, i.e. \f$2L\f$.
static constexpr unsigned max_nof_typeII_coefficients = 2 * max_nof_typeII_beams;

/// \brief Bit-width of each of the Type II wideband amplitude indices \f$k^{(1)}_{l,i}\f$.
///
/// The \f$2L - 1\f$ indices other than the strongest one are reported with three bits each, as per TS38.212
/// Table 6.3.2.1.2-1 and TS38.214 Table 5.2.2.2.3-2.
static constexpr unsigned nof_typeII_wideband_amplitude_bits = 3;

/// \brief Bit-width of the Type II phase coefficients reported with a QPSK alphabet.
///
/// When the subband amplitude reporting is enabled, the \f$M_l - \min(M_l, K^{(2)})\f$ weakest non-zero coefficients
/// of a layer report their phase as \f$c_{l,i} \in \{0, 1, 2, 3\}\f$, whereas the \f$\min(M_l, K^{(2)})\f$
/// strongest ones use the full \f$N_{PSK}\f$ alphabet, as per TS38.214 Section 5.2.2.2.3.
static constexpr unsigned nof_typeII_qpsk_phase_bits = 2;

/// Number of non-zero wideband amplitude coefficients \f$M_l\f$ for each of the reported layers.
using typeII_nof_amplitudes = static_vector<uint8_t, max_nof_typeII_layers>;

/// Type II beam selection \f$(q_1, q_2)\f$ within the selected beam group.
struct pmi_typeII_beam_selection {
  /// First-dimension beam selection. Valid values are {0, ..., O1 - 1}.
  unsigned q1;
  /// Second-dimension beam selection. Valid values are {0, ..., O2 - 1}.
  unsigned q2;
};

/// \brief Decodes the Type II PMI parameter \f$i_{1,1}\f$ into \f$(q_1, q_2)\f$.
///
/// TS38.214 Section 5.2.2.2.3 defines \f$i_{1,1} = [q_1 q_2]\f$. As per TS38.212 CSI field mapping, the two components
/// are packed as adjacent bit subfields with \f$q_1\f$ in the high-order bits and \f$q_2\f$ in the low-order bits.
///
/// \param[in] i_1_1 Reported PMI parameter \f$i_{1,1}\f$.
/// \param[in] o1    First-dimension oversampling factor \f$O_1\f$.
/// \param[in] o2    Second-dimension oversampling factor \f$O_2\f$.
/// \return The decoded beam selection \f$(q_1, q_2)\f$.
pmi_typeII_beam_selection get_typeII_beam_selection(unsigned i_1_1, unsigned o1, unsigned o2);

/// Type II beam group, containing \f$O1 * O2\f$ oversampled beams. Each group is identified by its two-dimensional beam
/// group indices \f$(n_1, n_2)\f$.
struct pmi_typeII_beam_group {
  /// First-dimension beam index. Valid values are {0, ..., N_1 - 1}.
  unsigned n1;
  /// Second-dimension beam index. Valid values are {0, ..., N_2 - 1}.
  unsigned n2;
};

/// \brief Decodes the Type II beam group selection index i_1_2 into the L selected beam groups.
///
/// Each returned beam group is given by its two-dimensional indices \f$(n_1^{(i)}, n_2^{(i)})\f$. The decoder uses the
/// algorithm defined in TS38.214 Section 5.2.2.2.3 (Table 5.2.2.2.3-1).
///
/// \param[in] i_1_2     Reported beam group selection index.
/// \param[in] n1        Number of first-dimension beams, N1.
/// \param[in] n2        Number of second-dimension beams, N2.
/// \param[in] nof_beams Number of selected beams, L.
/// \return The L selected beam groups, as (n_1, n_2) pairs.
/// \remark An assertion is triggered if \c nof_beams exceeds \ref max_nof_typeII_beams or \f$N_1 * N_2\f$, or if \c
/// i_1_2 is out of range.
static_vector<pmi_typeII_beam_group, max_nof_typeII_beams>
get_typeII_beam_groups(unsigned i_1_2, unsigned n1, unsigned n2, unsigned nof_beams);

/// \brief Type II wideband amplitude coefficient \f$p^{(1)}\f$ from index \f$k^{(1)}\f$, as per TS38.214
/// Table 5.2.2.2.3-2.
///
/// \param[in] k1 Wideband amplitude index, \f$k^{(1)}\f$.
/// \return The amplitude coefficient, \f$p^{(1)}\f$.
float get_typeII_wideband_amplitude(unsigned k1);

/// \brief Type II subband amplitude coefficient \f$p^{(2)}\f$ from index \f$k^{(2)}\f$, as per TS38.214
/// Table 5.2.2.2.3-3.
///
/// \param[in] k2 Subband amplitude index, \f$k^{(2)}\f$.
/// \return The amplitude coefficient, \f$p^{(2)}\f$.
float get_typeII_subband_amplitude(unsigned k2);

/// \brief Gets the number of full resolution subband coefficients \f$K^{(2)}\f$, as per TS38.214 Table 5.2.2.2.3-4.
///
/// \param[in] nof_beams Number of combined beams, \f$L\f$.
/// \return The number of coefficients that are reported with full resolution when the subband amplitude reporting is
/// enabled.
inline unsigned get_typeII_nof_full_res_coefficients(unsigned nof_beams)
{
  ocudu_assert(
      (nof_beams >= 2) && (nof_beams <= max_nof_typeII_beams), "Invalid number of beams (i.e., {}).", nof_beams);
  return (nof_beams == max_nof_typeII_beams) ? 6 : 4;
}

/// \brief Sorts the reported Type II combining coefficients from strongest to weakest.
///
/// Only the coefficients with a non-zero wideband amplitude other than the strongest one are considered, hence the
/// number of returned indices is \f$M_l - 1\f$. As per TS38.214 Section 5.2.2.2.3, coefficients with identical wideband
/// amplitudes are sorted with increasing coefficient index.
///
/// \param[in] wideband_amplitudes   Wideband amplitude indices \f$k^{(1)}_{l,i}\f$, one per beam and polarization.
/// \param[in] strongest_coefficient Strongest coefficient index \f$i_{1,3,l}\f$.
/// \return The coefficient indices in decreasing wideband amplitude order.
static_vector<uint8_t, max_nof_typeII_coefficients>
get_typeII_coefficient_strength_order(span<const uint8_t> wideband_amplitudes, unsigned strongest_coefficient);

/// \brief Precoding Matrix Indicator (PMI) parameter bit-widths for the Type II codebook.
///
/// Unused or fixed values for the given configuration are set to zero.
struct pmi_typeII_param_sizes {
  /// Parameter \f$i_{1,1}\f$ bit-width.
  unsigned i_1_1;
  /// Parameter \f$i_{1,2}\f$ bit-width.
  unsigned i_1_2;
  /// Parameter \f$i_{1,3,1}\f$ bit-width.
  unsigned i_1_3_1;
  /// Parameter \f$i_{1,3,2}\f$ bit-width.
  unsigned i_1_3_2;
  /// Parameter \f$i_{1,4,1}\f$ bit-width.
  unsigned i_1_4_1;
  /// Parameter \f$i_{1,4,2}\f$ bit-width.
  unsigned i_1_4_2;
  /// Parameter \f$i_{2,1,1}\f$ bit-width.
  unsigned i_2_1_1;
  /// Parameter \f$i_{2,1,2}\f$ bit-width.
  unsigned i_2_1_2;
  /// Parameter \f$i_{2,2,1}\f$ bit-width.
  unsigned i_2_2_1;
  /// Parameter \f$i_{2,2,2}\f$ bit-width.
  unsigned i_2_2_2;
};

/// \brief Gets the PMI parameter sizes for the Type II codebook configuration as per TS38.212 Table 6.3.2.1.2-1.
///
/// \param[in] codebook       Type II codebook configuration.
/// \param[in] nof_amplitudes Number of non-zero wideband amplitude coefficients \f$M_l\f$, one entry per reported
///                           layer.
/// \remark An assertion is triggered if the number of layers exceeds \ref max_nof_typeII_layers, or if any of the
/// number of non-zero wideband amplitude coefficients is out of the range {1, ..., 2L}.
pmi_typeII_param_sizes get_pmi_sizes_typeII(const pmi_codebook_typeII&   codebook,
                                            const typeII_nof_amplitudes& nof_amplitudes);

/// \brief Get the number of possible beam groups combinations based on the number of beams and the antenna panel
/// layout.
///
/// A beam group is the set of \f$O_1 * O_2\f$ oversampled beams sharing the same base index \f$(n_1, n_2)\f$. See \ref
/// pmi_typeII_beam_group.
unsigned get_typeII_nof_total_beam_groups(const pmi_codebook_typeII& codebook);

/// \brief Precoding Matrix Indicator (PMI) parameter ranges for Type II codebooks.
///
/// Each of the values give the number of possible values for each of the parameters. The ranges are exclusive, meaning
/// that the fields start at zero.
struct pmi_typeII_param_ranges {
  /// Parameter \f$i_{1,1}\f$.
  unsigned i_1_1;
  /// Parameter \f$i_{1,2}\f$.
  unsigned i_1_2;
  /// Parameter \f$i_{3,1}\f$.
  unsigned i_1_3_1;
  /// Parameter \f$i_{3,2}\f$.
  unsigned i_1_3_2;
  /// Parameter \f$i_{4,1}\f$.
  unsigned i_1_4_1;
  /// Parameter \f$i_{4,2}\f$.
  unsigned i_1_4_2;
  /// Parameter \f$i_{2,1,1}\f$.
  unsigned i_2_1_1;
  /// Parameter \f$i_{2,1,2}\f$.
  unsigned i_2_1_2;
  /// Parameter \f$i_{2,2,1}\f$.
  unsigned i_2_2_1;
  /// Parameter \f$i_{2,2,2}\f$.
  unsigned i_2_2_2;
};

/// \brief Gets PMI parameter ranges for \e Type-II codebook configuration as per TS38.214 Section 5.2.2.2.3.
///
/// The range for each PMI parameter returned by this function is defined as in an exclusive range. Hence, each PMI
/// range value indicates the number of possible values for the corresponding PMI parameter for the given panel
/// topology.
pmi_typeII_param_ranges get_pmi_ranges_typeII(const pmi_codebook_typeII& panel, uint8_t ri);

} // namespace ocudu
