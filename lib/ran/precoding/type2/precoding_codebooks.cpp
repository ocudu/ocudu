// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/precoding/precoding_codebooks.h"
#include "ocudu/ran/precoding/precoding_codebook_helpers.h"
#include "ocudu/ran/precoding/precoding_constants.h"
#include "ocudu/support/math/math_utils.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>

using namespace ocudu;

/// Maximum number of layers supported by the Type II codebook.
static constexpr unsigned max_nof_typeII_layers = 2;

/// \brief Accumulates the contribution of a single Type II beam into a layer of the precoding weight matrix.
///
/// The beam is added to the two polarizations of the given layer, each scaled by its own combining coefficient. The
/// first half of the ports corresponds to the first polarization and the second half to the second polarization.
///
/// \param[in,out] result      Precoding weight matrix to accumulate into (must be zero-initialized beforehand).
/// \param[in]     panel       Antenna panel information.
/// \param[in]     i_layer     Layer index within the precoding weight matrix.
/// \param[in]     m1          Oversampled horizontal beam index \f$m_1 = O_1 n_1 + q_1\f$.
/// \param[in]     m2          Oversampled vertical beam index \f$m_2 = O_2 n_2 + q_2\f$.
/// \param[in]     coeff_pol0  Combining coefficient for the first polarization.
/// \param[in]     coeff_pol1  Combining coefficient for the second polarization.
static void add_beam_type2(precoding_weight_matrix&              result,
                           const pmi_codebook_single_panel_info& panel,
                           unsigned                              i_layer,
                           unsigned                              m1,
                           unsigned                              m2,
                           cf_t                                  coeff_pol0,
                           cf_t                                  coeff_pol1)
{
  // Phase increments that define the beam direction in the horizontal and vertical planes.
  cf_t phase_inc_h = std::polar(1.0F, TWOPI * (static_cast<float>(m1) / static_cast<float>(panel.o1 * panel.n1)));
  cf_t phase_inc_v = std::polar(1.0F, TWOPI * (static_cast<float>(m2) / static_cast<float>(panel.o2 * panel.n2)));

  // Number of ports per polarization, i.e. half the total number of ports.
  unsigned nof_ports_pol = result.get_nof_ports() / 2;

  cf_t     beam_coef_h = std::polar(1.0F, 0.0F);
  unsigned i_port      = 0;
  for (unsigned i_h = 0; i_h != panel.n1; ++i_h) {
    cf_t beam_coef_v = std::polar(1.0F, 0.0F);
    for (unsigned i_v = 0; i_v != panel.n2; ++i_v) {
      // Beam coefficient for this antenna element.
      cf_t v = beam_coef_h * beam_coef_v;

      // Accumulate the beam into both polarizations, each weighted by its combining coefficient.
      result.set_coefficient(result.get_coefficient(i_layer, i_port) + coeff_pol0 * v, i_layer, i_port);
      result.set_coefficient(
          result.get_coefficient(i_layer, nof_ports_pol + i_port) + coeff_pol1 * v, i_layer, nof_ports_pol + i_port);

      ++i_port;
      beam_coef_v *= phase_inc_v;
    }
    beam_coef_h *= phase_inc_h;
  }
}

precoding_weight_matrix ocudu::make_type2(const precoding_matrix_indicator& pmi, unsigned nof_layers)
{
  ocudu_assert((nof_layers > 0) && (nof_layers <= max_nof_typeII_layers),
               "The Type II codebook supports one or two layers, requested {}.",
               nof_layers);

  // Get the underlying Type II PMI type from the parameter PMI.
  const auto* type2_pmi = std::get_if<pmi_typeII>(&pmi);
  ocudu_assert(type2_pmi != nullptr, "The precoding matrix indicator (PMI) must be of type Type II.");

  const pmi_codebook_typeII&            config = type2_pmi->config;
  const pmi_codebook_single_panel_info& panel  = get_single_panel_info(config.n1_n2);

  unsigned L         = config.nof_beams.value();
  unsigned n1n2      = panel.n1 * panel.n2;
  unsigned nof_ports = 2 * n1n2;
  unsigned N_psk     = static_cast<unsigned>(config.phase_alphabet_size);

  ocudu_assert(nof_ports <= precoding_constants::MAX_NOF_PORTS,
               "The given number of ports (i.e., {}) exceeds the maximum supported number of ports (i.e., {}).",
               nof_ports,
               precoding_constants::MAX_NOF_PORTS);

  ocudu_assert(type2_pmi->layers.size() == nof_layers,
               "The number of layer coefficient sets (i.e., {}) does not match the number of layers (i.e., {}).",
               type2_pmi->layers.size(),
               nof_layers);

  // Decode the wideband beam selection and the L selected beam groups.
  pmi_typeII_beam_selection beam_selection = get_typeII_beam_selection(type2_pmi->i_1_1, panel.o1, panel.o2);
  static_vector<pmi_typeII_beam_group, max_nof_typeII_beams> beam_groups =
      get_typeII_beam_groups(type2_pmi->i_1_2, panel.n1, panel.n2, L);

  precoding_weight_matrix result(nof_layers, nof_ports);

  for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
    const pmi_typeII::layer_coefficients& coefficients = type2_pmi->layers[i_layer];

    // Validate the reported per-layer coefficient sizes, L beams by two polarizations.
    ocudu_assert(coefficients.i_1_4.size() == 2 * L,
                 "The number of wideband amplitude indices (i.e., {}) must be 2*L (i.e., {}).",
                 coefficients.i_1_4.size(),
                 2 * L);
    ocudu_assert(coefficients.i_2_1.size() == 2 * L,
                 "The number of phase indices (i.e., {}) must be 2*L (i.e., {}).",
                 coefficients.i_2_1.size(),
                 2 * L);
    ocudu_assert(config.subband_amplitude ? (coefficients.i_2_2.size() == 2 * L) : coefficients.i_2_2.empty(),
                 "The number of subband amplitude indices (i.e., {}) is inconsistent with subbandAmplitude={}.",
                 coefficients.i_2_2.size(),
                 config.subband_amplitude);
    ocudu_assert(coefficients.i_1_3 < 2 * L,
                 "The strongest-coefficient index i_1_3 (i.e., {}) is out of range (i.e., {}).",
                 coefficients.i_1_3,
                 2 * L);
    // The strongest coefficient must carry the maximum wideband amplitude among the reported coefficients.
    ocudu_assert(coefficients.i_1_4[coefficients.i_1_3] ==
                     *std::max_element(coefficients.i_1_4.begin(), coefficients.i_1_4.end()),
                 "The strongest beam i_1_3 (i.e., {}) must have the maximum wideband amplitude index.",
                 coefficients.i_1_3);

    // Compute the 2L combining coefficients and the layer energy used for the per-layer normalization.
    static_vector<cf_t, 2 * max_nof_typeII_beams> coeffs(2 * L);
    float                                         energy = 0.0F;
    for (unsigned c = 0; c != 2 * L; ++c) {
      // Wideband amplitude, as per TS38.214 Table 5.2.2.2.3-2.
      float p1 = get_typeII_wideband_amplitude(coefficients.i_1_4[c]);
      // Subband amplitude, as per TS38.214 Table 5.2.2.2.3-3. When subband amplitude is disabled, k2 = 1 (p2 = 1).
      float p2 = config.subband_amplitude ? get_typeII_subband_amplitude(coefficients.i_2_2[c]) : 1.0F;
      // Phase offset per beam and polarization, as per TS38.214 Section 5.2.2.2.3.
      cf_t phi = std::polar(1.0F, TWOPI * static_cast<float>(coefficients.i_2_1[c]) / static_cast<float>(N_psk));

      coeffs[c] = (p1 * p2) * phi;
      energy += (p1 * p2) * (p1 * p2);
    }
    ocudu_assert(energy > 0.0F, "The layer coefficient energy must be strictly positive.");

    // Accumulate the L beams into the layer.
    for (unsigned i = 0; i != L; ++i) {
      unsigned m1 = panel.o1 * beam_groups[i].n1 + beam_selection.q1;
      unsigned m2 = panel.o2 * beam_groups[i].n2 + beam_selection.q2;
      add_beam_type2(result, panel, i_layer, m1, m2, coeffs[i], coeffs[i + L]);
    }

    // Per-layer coefficient normalization, as per TS38.214 Table 5.2.2.2.3-5.
    float layer_scaling = 1.0F / std::sqrt(energy);
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      result.set_coefficient(result.get_coefficient(i_layer, i_port) * layer_scaling, i_layer, i_port);
    }
  }

  // Common normalization factor for all layers.
  result *= 1.0F / std::sqrt(static_cast<float>(n1n2) * static_cast<float>(nof_layers));

  return result;
}
