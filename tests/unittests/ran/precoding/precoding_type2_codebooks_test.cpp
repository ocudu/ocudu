// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/adt/format.h"
#include "ocudu/ran/precoding/precoding_codebook_configuration.h"
#include "ocudu/ran/precoding/precoding_codebook_helpers.h"
#include "ocudu/ran/precoding/precoding_codebooks.h"
#include <gtest/gtest.h>
#include <vector>

using namespace ocudu;

/// Tolerance for floating-point arithmetics equality comparisons.
static constexpr float TOLERANCE_FLOATING_POINT = 1e-5;

/// Assert that two float-based complex values are equal.
#define ASSERT_CF_EQ(val1, val2)                                                                                       \
  do {                                                                                                                 \
    ASSERT_NEAR((val1).real(), (val2).real(), TOLERANCE_FLOATING_POINT);                                               \
    ASSERT_NEAR((val1).imag(), (val2).imag(), TOLERANCE_FLOATING_POINT);                                               \
  } while (false)

/// \brief Calculates the layer to antenna port weights for a single beam.
///
/// The port coefficients follow \f$\nu _{l,m}\f$ (and \f$\tilde{\nu}_{l,m}\f$ for four layer weights) as per TS38.214
/// Section 5.2.2.2.1. For the Type II codebook, \c l and \c m are the oversampled beam indices \f$m_1 = O_1 n_1 +
/// q_1\f$ and \f$m_2 = O_2 n_2 + q_2\f$.
///
/// \param N1            Parameter \f$N_1\f$.
/// \param N2            Parameter \f$N_2\f$.
/// \param O1            Parameter \f$O_1\f$.
/// \param O2            Parameter \f$O_2\f$.
/// \param l             First-dimension (oversampled) beam index \f$m_1\f$.
/// \param m             Second-dimension (oversampled) beam index \f$m_2\f$.
/// \param layer_weights Layer weights for each of the beams.
/// \return A vector containing the layer to port coefficients.
/// \remark An assertion is triggered if the number of layer weights is other than 2 or 4.
static std::vector<cf_t> get_layer_port_weights(unsigned                      N1,
                                                unsigned                      N2,
                                                unsigned                      O1,
                                                unsigned                      O2,
                                                unsigned                      l,
                                                unsigned                      m,
                                                const static_vector<cf_t, 4>& layer_weights)
{
  ocudu_assert((layer_weights.size() == 2) || (layer_weights.size() == 4),
               "Invalid number of layer weights (i.e., {}). The layer weights must be 2 or 4.",
               layer_weights);

  std::vector<cf_t> coefficients;
  coefficients.reserve(N1 * N2 * 2);

  for (const cf_t& layer_weight : layer_weights) {
    for (unsigned i = 0, end_i = 2 * N1 / layer_weights.size(); i != end_i; ++i) {
      for (unsigned j = 0, end_j = N2; j != end_j; ++j) {
        cf_t u_m   = std::polar(1.0f, TWOPI * static_cast<float>(m * j) / static_cast<float>(N2 * O2));
        cf_t v_l_m = u_m * std::polar(1.0f, TWOPI * static_cast<float>(l * i) / static_cast<float>(N1 * O1));

        coefficients.emplace_back(layer_weight * v_l_m);
      }
    }
  }

  return coefficients;
}

// Test the Type II precoding matrix generation for a transmission using one layer and four antenna ports.
TEST(precoding_codebooks_test, Type2_OneLayerFourPorts)
{
  // Antenna configuration parameters, corresponding to N1 = 2, N2 = 1 (four CSI-RS ports).
  static constexpr unsigned N1                = 2;
  static constexpr unsigned N2                = 1;
  static constexpr unsigned O1                = 4;
  static constexpr unsigned O2                = 1;
  static constexpr unsigned L                 = 2;
  static constexpr unsigned nof_layers        = 1;
  static constexpr unsigned nof_ports         = 2 * N1 * N2;
  static constexpr unsigned N_psk             = 4;
  static constexpr bool     subband_amplitude = false;

  pmi_codebook_typeII config{
      pmi_codebook_single_panel_config::two_one, L, pmi_codebook_typeII_phase_size::qpsk, subband_amplitude};

  // Get the codebook parameters range.
  pmi_typeII_param_ranges param_ranges = get_pmi_ranges_typeII(config, nof_layers);

  for (uint8_t i_1_1 = 0; i_1_1 != param_ranges.i_1_1; ++i_1_1) {
    for (uint16_t i_1_2 = 0; i_1_2 != param_ranges.i_1_2; ++i_1_2) {
      pmi_typeII pmi = {
          .config = config, .i_1_1 = i_1_1, .i_1_2 = i_1_2, .layers = {{0, {7, 4, 3, 2}, {0, 1, 2, 3}, {}}}};

      precoding_weight_matrix precoding = make_type2(pmi, nof_layers);
      ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
      ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

      // Decode the beam selection (q1, q2) and the L selected beams (n1, n2).
      pmi_typeII_beam_selection                                  sel   = get_typeII_beam_selection(i_1_1, O1, O2);
      static_vector<pmi_typeII_beam_group, max_nof_typeII_beams> beams = get_typeII_beam_groups(i_1_2, N1, N2, L);

      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        const pmi_typeII::layer_coefficients& lc = pmi.layers[i_layer];

        // Combining coefficients and the layer energy.
        static_vector<cf_t, 2 * max_nof_typeII_beams> coeffs(2 * L);
        float                                         energy = 0.0F;
        for (unsigned i_beam = 0; i_beam != 2 * L; ++i_beam) {
          float p1 = get_typeII_wideband_amplitude(lc.i_1_4[i_beam]);
          float p2 = subband_amplitude ? get_typeII_subband_amplitude(lc.i_2_2[i_beam]) : 1.0F;
          coeffs[i_beam] =
              (p1 * p2) * std::polar(1.0F, TWOPI * static_cast<float>(lc.i_2_1[i_beam]) / static_cast<float>(N_psk));
          energy += (p1 * p2) * (p1 * p2);
        }
        float scaling = 1.0F / std::sqrt(static_cast<float>(N1 * N2) * static_cast<float>(nof_layers) * energy);

        // Expected layer weights, sum of the per-beam weights scaled.
        std::vector<cf_t> expected(nof_ports, cf_t(0.0F, 0.0F));
        for (unsigned beam = 0; beam != L; ++beam) {
          unsigned          m1 = O1 * beams[beam].n1 + sel.q1;
          unsigned          m2 = O2 * beams[beam].n2 + sel.q2;
          std::vector<cf_t> beam_weights =
              get_layer_port_weights(N1, N2, O1, O2, m1, m2, {coeffs[beam] * scaling, coeffs[beam + L] * scaling});
          for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
            expected[i_port] += beam_weights[i_port];
          }
        }

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected[i_port]);
        }
      }
    }
  }
}

// Test the Type II precoding matrix generation for a transmission using two layers and four antenna ports.
TEST(precoding_codebooks_test, Type2_TwoLayerFourPorts)
{
  // Antenna configuration parameters, corresponding to N1 = 2, N2 = 1 (four CSI-RS ports).
  static constexpr unsigned N1                = 2;
  static constexpr unsigned N2                = 1;
  static constexpr unsigned O1                = 4;
  static constexpr unsigned O2                = 1;
  static constexpr unsigned L                 = 2;
  static constexpr unsigned nof_layers        = 2;
  static constexpr unsigned nof_ports         = 2 * N1 * N2;
  static constexpr unsigned N_psk             = 8;
  static constexpr bool     subband_amplitude = true;

  pmi_codebook_typeII config{
      pmi_codebook_single_panel_config::two_one, L, pmi_codebook_typeII_phase_size::psk8, subband_amplitude};

  // Get the codebook parameters range.
  pmi_typeII_param_ranges param_ranges = get_pmi_ranges_typeII(config, nof_layers);

  for (uint8_t i_1_1 = 0; i_1_1 != param_ranges.i_1_1; ++i_1_1) {
    for (uint16_t i_1_2 = 0; i_1_2 != param_ranges.i_1_2; ++i_1_2) {
      pmi_typeII pmi = {
          .config = config,
          .i_1_1  = i_1_1,
          .i_1_2  = i_1_2,
          .layers = {{1, {5, 7, 4, 2}, {3, 0, 5, 6}, {0, 1, 1, 0}}, {2, {4, 6, 7, 3}, {1, 7, 0, 4}, {1, 0, 1, 1}}}};

      precoding_weight_matrix precoding = make_type2(pmi, nof_layers);
      ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
      ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

      // Decode the beam selection (q1, q2) and the L selected beams (n1, n2).
      pmi_typeII_beam_selection                                  sel   = get_typeII_beam_selection(i_1_1, O1, O2);
      static_vector<pmi_typeII_beam_group, max_nof_typeII_beams> beams = get_typeII_beam_groups(i_1_2, N1, N2, L);

      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        const pmi_typeII::layer_coefficients& lc = pmi.layers[i_layer];

        // Combining coefficients and the layer energy.
        static_vector<cf_t, 2 * max_nof_typeII_beams> coeffs(2 * L);
        float                                         energy = 0.0F;
        for (unsigned i_beam = 0; i_beam != 2 * L; ++i_beam) {
          float p1 = get_typeII_wideband_amplitude(lc.i_1_4[i_beam]);
          float p2 = subband_amplitude ? get_typeII_subband_amplitude(lc.i_2_2[i_beam]) : 1.0F;
          coeffs[i_beam] =
              (p1 * p2) * std::polar(1.0F, TWOPI * static_cast<float>(lc.i_2_1[i_beam]) / static_cast<float>(N_psk));
          energy += (p1 * p2) * (p1 * p2);
        }
        float scaling = 1.0F / std::sqrt(static_cast<float>(N1 * N2) * static_cast<float>(nof_layers) * energy);

        // Expected layer weights, sum of the per-beam weights scaled.
        std::vector<cf_t> expected(nof_ports, cf_t(0.0F, 0.0F));
        for (unsigned beam = 0; beam != L; ++beam) {
          unsigned          m1 = O1 * beams[beam].n1 + sel.q1;
          unsigned          m2 = O2 * beams[beam].n2 + sel.q2;
          std::vector<cf_t> beam_weights =
              get_layer_port_weights(N1, N2, O1, O2, m1, m2, {coeffs[beam] * scaling, coeffs[beam + L] * scaling});
          for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
            expected[i_port] += beam_weights[i_port];
          }
        }

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected[i_port]);
        }
      }
    }
  }
}

// Test the Type II precoding matrix generation for one layer and eight antenna ports with N1=4, N2=1.
TEST(precoding_codebooks_test, Type2_OneLayer_4x1)
{
  // Antenna configuration parameters, corresponding to N1 = 4, N2 = 1 (eight CSI-RS ports).
  static constexpr unsigned N1                = 4;
  static constexpr unsigned N2                = 1;
  static constexpr unsigned O1                = 4;
  static constexpr unsigned O2                = 1;
  static constexpr unsigned L                 = 4;
  static constexpr unsigned nof_layers        = 1;
  static constexpr unsigned nof_ports         = 2 * N1 * N2;
  static constexpr unsigned N_psk             = 8;
  static constexpr bool     subband_amplitude = false;

  pmi_codebook_typeII config{
      pmi_codebook_single_panel_config::four_one, L, pmi_codebook_typeII_phase_size::psk8, subband_amplitude};

  // Get the codebook parameters range.
  pmi_typeII_param_ranges param_ranges = get_pmi_ranges_typeII(config, nof_layers);

  for (uint8_t i_1_1 = 0; i_1_1 != param_ranges.i_1_1; ++i_1_1) {
    for (uint16_t i_1_2 = 0; i_1_2 != param_ranges.i_1_2; ++i_1_2) {
      pmi_typeII pmi = {.config = config,
                        .i_1_1  = i_1_1,
                        .i_1_2  = i_1_2,
                        .layers = {{2, {2, 4, 7, 1, 3, 5, 6, 2}, {1, 2, 0, 3, 4, 5, 6, 7}, {}}}};

      precoding_weight_matrix precoding = make_type2(pmi, nof_layers);
      ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
      ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

      // Decode the beam selection (q1, q2) and the L selected beams (n1, n2).
      pmi_typeII_beam_selection                                  sel   = get_typeII_beam_selection(i_1_1, O1, O2);
      static_vector<pmi_typeII_beam_group, max_nof_typeII_beams> beams = get_typeII_beam_groups(i_1_2, N1, N2, L);

      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        const pmi_typeII::layer_coefficients& lc = pmi.layers[i_layer];

        // Combining coefficients and the layer energy.
        static_vector<cf_t, 2 * max_nof_typeII_beams> coeffs(2 * L);
        float                                         energy = 0.0F;
        for (unsigned i_beam = 0; i_beam != 2 * L; ++i_beam) {
          float p1 = get_typeII_wideband_amplitude(lc.i_1_4[i_beam]);
          float p2 = subband_amplitude ? get_typeII_subband_amplitude(lc.i_2_2[i_beam]) : 1.0F;
          coeffs[i_beam] =
              (p1 * p2) * std::polar(1.0F, TWOPI * static_cast<float>(lc.i_2_1[i_beam]) / static_cast<float>(N_psk));
          energy += (p1 * p2) * (p1 * p2);
        }
        float scaling = 1.0F / std::sqrt(static_cast<float>(N1 * N2) * static_cast<float>(nof_layers) * energy);

        // Expected layer weights, sum of the per-beam weights scaled.
        std::vector<cf_t> expected(nof_ports, cf_t(0.0F, 0.0F));
        for (unsigned beam = 0; beam != L; ++beam) {
          unsigned          m1 = O1 * beams[beam].n1 + sel.q1;
          unsigned          m2 = O2 * beams[beam].n2 + sel.q2;
          std::vector<cf_t> beam_weights =
              get_layer_port_weights(N1, N2, O1, O2, m1, m2, {coeffs[beam] * scaling, coeffs[beam + L] * scaling});
          for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
            expected[i_port] += beam_weights[i_port];
          }
        }

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected[i_port]);
        }
      }
    }
  }
}

// Test the Type II precoding matrix generation for two layers and eight antenna ports with N1=4, N2=1.
TEST(precoding_codebooks_test, Type2_TwoLayer_4x1)
{
  // Antenna configuration parameters, corresponding to N1 = 4, N2 = 1 (eight CSI-RS ports).
  static constexpr unsigned N1                = 4;
  static constexpr unsigned N2                = 1;
  static constexpr unsigned O1                = 4;
  static constexpr unsigned O2                = 1;
  static constexpr unsigned L                 = 3;
  static constexpr unsigned nof_layers        = 2;
  static constexpr unsigned nof_ports         = 2 * N1 * N2;
  static constexpr unsigned N_psk             = 4;
  static constexpr bool     subband_amplitude = true;

  pmi_codebook_typeII config{
      pmi_codebook_single_panel_config::four_one, L, pmi_codebook_typeII_phase_size::qpsk, subband_amplitude};

  // Get the codebook parameters range.
  pmi_typeII_param_ranges param_ranges = get_pmi_ranges_typeII(config, nof_layers);

  for (uint8_t i_1_1 = 0; i_1_1 != param_ranges.i_1_1; ++i_1_1) {
    for (uint16_t i_1_2 = 0; i_1_2 != param_ranges.i_1_2; ++i_1_2) {
      pmi_typeII pmi = {.config = config,
                        .i_1_1  = i_1_1,
                        .i_1_2  = i_1_2,
                        .layers = {{0, {7, 5, 4, 3, 2, 1}, {0, 1, 2, 3, 1, 2}, {0, 1, 0, 1, 1, 0}},
                                   {4, {3, 4, 2, 5, 7, 6}, {2, 3, 1, 0, 3, 2}, {1, 0, 1, 1, 0, 1}}}};

      precoding_weight_matrix precoding = make_type2(pmi, nof_layers);
      ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
      ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

      // Decode the beam selection (q1, q2) and the L selected beams (n1, n2).
      pmi_typeII_beam_selection                                  sel   = get_typeII_beam_selection(i_1_1, O1, O2);
      static_vector<pmi_typeII_beam_group, max_nof_typeII_beams> beams = get_typeII_beam_groups(i_1_2, N1, N2, L);

      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        const pmi_typeII::layer_coefficients& lc = pmi.layers[i_layer];

        // Combining coefficients and the layer energy.
        static_vector<cf_t, 2 * max_nof_typeII_beams> coeffs(2 * L);
        float                                         energy = 0.0F;
        for (unsigned i_beam = 0; i_beam != 2 * L; ++i_beam) {
          float p1 = get_typeII_wideband_amplitude(lc.i_1_4[i_beam]);
          float p2 = subband_amplitude ? get_typeII_subband_amplitude(lc.i_2_2[i_beam]) : 1.0F;
          coeffs[i_beam] =
              (p1 * p2) * std::polar(1.0F, TWOPI * static_cast<float>(lc.i_2_1[i_beam]) / static_cast<float>(N_psk));
          energy += (p1 * p2) * (p1 * p2);
        }
        float scaling = 1.0F / std::sqrt(static_cast<float>(N1 * N2) * static_cast<float>(nof_layers) * energy);

        // Expected layer weights, sum of the per-beam weights scaled.
        std::vector<cf_t> expected(nof_ports, cf_t(0.0F, 0.0F));
        for (unsigned beam = 0; beam != L; ++beam) {
          unsigned          m1 = O1 * beams[beam].n1 + sel.q1;
          unsigned          m2 = O2 * beams[beam].n2 + sel.q2;
          std::vector<cf_t> beam_weights =
              get_layer_port_weights(N1, N2, O1, O2, m1, m2, {coeffs[beam] * scaling, coeffs[beam + L] * scaling});
          for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
            expected[i_port] += beam_weights[i_port];
          }
        }

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected[i_port]);
        }
      }
    }
  }
}

// Test the Type II precoding matrix generation for one layer and eight antenna ports with N1=2, N2=2.
TEST(precoding_codebooks_test, Type2_OneLayer_2x2)
{
  // Antenna configuration parameters, corresponding to N1 = 2, N2 = 2 (eight CSI-RS ports).
  static constexpr unsigned N1                = 2;
  static constexpr unsigned N2                = 2;
  static constexpr unsigned O1                = 4;
  static constexpr unsigned O2                = 4;
  static constexpr unsigned L                 = 2;
  static constexpr unsigned nof_layers        = 1;
  static constexpr unsigned nof_ports         = 2 * N1 * N2;
  static constexpr unsigned N_psk             = 8;
  static constexpr bool     subband_amplitude = true;

  pmi_codebook_typeII config{
      pmi_codebook_single_panel_config::two_two, L, pmi_codebook_typeII_phase_size::psk8, subband_amplitude};

  // Get the codebook parameters range.
  pmi_typeII_param_ranges param_ranges = get_pmi_ranges_typeII(config, nof_layers);

  for (uint8_t i_1_1 = 0; i_1_1 != param_ranges.i_1_1; ++i_1_1) {
    for (uint16_t i_1_2 = 0; i_1_2 != param_ranges.i_1_2; ++i_1_2) {
      pmi_typeII pmi = {
          .config = config, .i_1_1 = i_1_1, .i_1_2 = i_1_2, .layers = {{1, {5, 7, 3, 6}, {3, 0, 5, 2}, {0, 1, 1, 0}}}};

      precoding_weight_matrix precoding = make_type2(pmi, nof_layers);
      ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
      ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

      // Decode the beam selection (q1, q2) and the L selected beams (n1, n2).
      pmi_typeII_beam_selection                                  sel   = get_typeII_beam_selection(i_1_1, O1, O2);
      static_vector<pmi_typeII_beam_group, max_nof_typeII_beams> beams = get_typeII_beam_groups(i_1_2, N1, N2, L);

      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        const pmi_typeII::layer_coefficients& lc = pmi.layers[i_layer];

        // Combining coefficients and the layer energy.
        static_vector<cf_t, 2 * max_nof_typeII_beams> coeffs(2 * L);
        float                                         energy = 0.0F;
        for (unsigned i_beam = 0; i_beam != 2 * L; ++i_beam) {
          float p1 = get_typeII_wideband_amplitude(lc.i_1_4[i_beam]);
          float p2 = subband_amplitude ? get_typeII_subband_amplitude(lc.i_2_2[i_beam]) : 1.0F;
          coeffs[i_beam] =
              (p1 * p2) * std::polar(1.0F, TWOPI * static_cast<float>(lc.i_2_1[i_beam]) / static_cast<float>(N_psk));
          energy += (p1 * p2) * (p1 * p2);
        }
        float scaling = 1.0F / std::sqrt(static_cast<float>(N1 * N2) * static_cast<float>(nof_layers) * energy);

        // Expected layer weights, sum of the per-beam weights scaled.
        std::vector<cf_t> expected(nof_ports, cf_t(0.0F, 0.0F));
        for (unsigned beam = 0; beam != L; ++beam) {
          unsigned          m1 = O1 * beams[beam].n1 + sel.q1;
          unsigned          m2 = O2 * beams[beam].n2 + sel.q2;
          std::vector<cf_t> beam_weights =
              get_layer_port_weights(N1, N2, O1, O2, m1, m2, {coeffs[beam] * scaling, coeffs[beam + L] * scaling});
          for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
            expected[i_port] += beam_weights[i_port];
          }
        }

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected[i_port]);
        }
      }
    }
  }
}

// Test the Type II precoding matrix generation for two layers and eight antenna ports with N1=2, N2=2.
TEST(precoding_codebooks_test, Type2_TwoLayer_2x2)
{
  // Antenna configuration parameters, corresponding to N1 = 2, N2 = 2 (eight CSI-RS ports).
  static constexpr unsigned N1                = 2;
  static constexpr unsigned N2                = 2;
  static constexpr unsigned O1                = 4;
  static constexpr unsigned O2                = 4;
  static constexpr unsigned L                 = 4;
  static constexpr unsigned nof_layers        = 2;
  static constexpr unsigned nof_ports         = 2 * N1 * N2;
  static constexpr unsigned N_psk             = 4;
  static constexpr bool     subband_amplitude = false;

  pmi_codebook_typeII config{
      pmi_codebook_single_panel_config::two_two, L, pmi_codebook_typeII_phase_size::qpsk, subband_amplitude};

  // Get the codebook parameters range.
  pmi_typeII_param_ranges param_ranges = get_pmi_ranges_typeII(config, nof_layers);

  for (uint8_t i_1_1 = 0; i_1_1 != param_ranges.i_1_1; ++i_1_1) {
    for (uint16_t i_1_2 = 0; i_1_2 != param_ranges.i_1_2; ++i_1_2) {
      pmi_typeII pmi = {.config = config,
                        .i_1_1  = i_1_1,
                        .i_1_2  = i_1_2,
                        .layers = {{2, {4, 5, 7, 1, 3, 6, 2, 4}, {1, 2, 0, 3, 2, 1, 3, 0}, {}},
                                   {5, {3, 2, 4, 6, 5, 7, 1, 2}, {0, 3, 2, 1, 3, 0, 2, 1}, {}}}};

      precoding_weight_matrix precoding = make_type2(pmi, nof_layers);
      ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
      ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

      // Decode the beam selection (q1, q2) and the L selected beams (n1, n2).
      pmi_typeII_beam_selection                                  sel   = get_typeII_beam_selection(i_1_1, O1, O2);
      static_vector<pmi_typeII_beam_group, max_nof_typeII_beams> beams = get_typeII_beam_groups(i_1_2, N1, N2, L);

      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        const pmi_typeII::layer_coefficients& lc = pmi.layers[i_layer];

        // Combining coefficients and the layer energy.
        static_vector<cf_t, 2 * max_nof_typeII_beams> coeffs(2 * L);
        float                                         energy = 0.0F;
        for (unsigned i_beam = 0; i_beam != 2 * L; ++i_beam) {
          float p1 = get_typeII_wideband_amplitude(lc.i_1_4[i_beam]);
          float p2 = subband_amplitude ? get_typeII_subband_amplitude(lc.i_2_2[i_beam]) : 1.0F;
          coeffs[i_beam] =
              (p1 * p2) * std::polar(1.0F, TWOPI * static_cast<float>(lc.i_2_1[i_beam]) / static_cast<float>(N_psk));
          energy += (p1 * p2) * (p1 * p2);
        }
        float scaling = 1.0F / std::sqrt(static_cast<float>(N1 * N2) * static_cast<float>(nof_layers) * energy);

        // Expected layer weights, sum of the per-beam weights scaled.
        std::vector<cf_t> expected(nof_ports, cf_t(0.0F, 0.0F));
        for (unsigned beam = 0; beam != L; ++beam) {
          unsigned          m1 = O1 * beams[beam].n1 + sel.q1;
          unsigned          m2 = O2 * beams[beam].n2 + sel.q2;
          std::vector<cf_t> beam_weights =
              get_layer_port_weights(N1, N2, O1, O2, m1, m2, {coeffs[beam] * scaling, coeffs[beam + L] * scaling});
          for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
            expected[i_port] += beam_weights[i_port];
          }
        }

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected[i_port]);
        }
      }
    }
  }
}
