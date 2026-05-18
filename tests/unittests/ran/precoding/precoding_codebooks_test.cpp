// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/precoding/precoding_codebook_configuration.h"
#include "ocudu/ran/precoding/precoding_codebooks.h"
#include <gtest/gtest.h>

using namespace ocudu;

/// Tolerance for floating-point arithmetics equality comparisons.
static constexpr float TOLERANCE_FLOATING_POINT = 1e-5;

/// Assert that two float-based complex values are equal.
#define ASSERT_CF_EQ(val1, val2)                                                                                       \
  do {                                                                                                                 \
    ASSERT_NEAR((val1).real(), (val2).real(), TOLERANCE_FLOATING_POINT);                                               \
    ASSERT_NEAR((val1).imag(), (val2).imag(), TOLERANCE_FLOATING_POINT);                                               \
  } while (false)

/// \brief Parameters that define beam weights for Type I single-panel codebook.
///
/// These parameters are used for deriving the values \f$\nu _{l,m}\f$ and \f$\tilde{\nu} _{l,m}\f$.
struct codebook_typeI_single_panel_beam {
  /// Parameter \f$N_1\f$.
  unsigned N1;
  /// Parameter \f$N_2\f$.
  unsigned N2;
  /// Parameter \f$O_1\f$.
  unsigned O1;
  /// Parameter \f$O_2\f$.
  unsigned O2;
  /// Parameter \f$l\f$.
  unsigned l;
  /// Parameter \f$m\f$.
  unsigned m;
};

/// \brief Calculates a layer to antenna port weights.
///
/// The formula for the coefficients layer to port coefficients are given by \f$\nu _{l,m}\f$ and
/// \f$\tilde{\nu}_{l,m}\f$ for \c layer_wights of size 2 and 4 respectively is TS38.214 Section 5.2.2.2.1.
///
/// \param beam_descr    Parameters that describe the beam creation.
/// \param layer_weights Layer weights for each of the beams.
/// \return A vector containing the layer to port coefficients.
/// \remark An assertion is triggered if the number of layer weights is other than 2 or 4.
static std::vector<cf_t> get_layer_port_weights(const codebook_typeI_single_panel_beam& beam_descr,
                                                const static_vector<cf_t, 4>&           layer_weights)
{
  ocudu_assert((layer_weights.size() == 2) || (layer_weights.size() == 4),
               "Invalid number of layer weights (i.e., {}). The layer weights must be 2 or 4.",
               layer_weights);

  unsigned N1 = beam_descr.N1;
  unsigned N2 = beam_descr.N2;
  unsigned O1 = beam_descr.O1;
  unsigned O2 = beam_descr.O2;
  unsigned m  = beam_descr.m;
  unsigned l  = beam_descr.l;

  std::vector<cf_t> coefficients;
  coefficients.reserve(beam_descr.N1 * beam_descr.N2 * 2);

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

// Test the precoding matrix generated for a one layer transmission using one antenna port.
TEST(precoding_codebooks_test, OneLayerOnePort)
{
  static constexpr unsigned max_nof_ports = precoding_constants::MAX_NOF_PORTS;
  for (unsigned nof_ports = 1; nof_ports != max_nof_ports; ++nof_ports) {
    for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
      precoding_weight_matrix precoding = make_one_layer_one_port(nof_ports, i_port);

      // Assert precoding matrix dimensions.
      ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
      ASSERT_EQ(precoding.get_nof_layers(), 1);

      // Assert precoding matrix coefficients.
      for (unsigned i_port_2 = 0; i_port_2 != nof_ports; ++i_port_2) {
        cf_t expected = (i_port_2 == i_port) ? cf_t(1.0f, 0.0f) : cf_t(0.0f, 0.0f);
        ASSERT_CF_EQ(expected, precoding.get_coefficient(0, i_port_2));
      }
    }
  }
}

// Test the precoding matrix generated for a one layer transmission using the maximum number of ports.
TEST(precoding_codebooks_test, OneLayerAllPorts)
{
  static constexpr unsigned max_nof_ports = precoding_constants::MAX_NOF_PORTS;
  for (unsigned nof_ports = 1; nof_ports != max_nof_ports; ++nof_ports) {
    precoding_weight_matrix precoding = make_one_layer_all_ports(nof_ports);

    // Assert precoding matrix dimensions.
    ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
    ASSERT_EQ(precoding.get_nof_layers(), 1);

    // Assert precoding matrix coefficients.
    for (unsigned i_port_2 = 0; i_port_2 != nof_ports; ++i_port_2) {
      cf_t expected = cf_t(1.0F / std::sqrt(static_cast<float>(nof_ports)), 0.0f);
      ASSERT_CF_EQ(expected, precoding.get_coefficient(0, i_port_2));
    }
  }
}

// Test the precoding matrix generated for the maximum number of transmission layers using the maximum number of ports,
// with an identity precoding matrix.
TEST(precoding_codebooks_test, Identity)
{
  static constexpr unsigned max_nof_layers = precoding_constants::MAX_NOF_LAYERS;
  for (unsigned nof_layers = 1; nof_layers != max_nof_layers; ++nof_layers) {
    precoding_weight_matrix precoding = make_identity(nof_layers);

    // Assert precoding matrix dimensions.
    ASSERT_EQ(precoding.get_nof_ports(), nof_layers);
    ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

    // Assert precoding matrix coefficients.
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      for (unsigned i_port = 0; i_port != nof_layers; ++i_port) {
        cf_t expected =
            (i_layer == i_port) ? cf_t(1.0f / std::sqrt(static_cast<float>(nof_layers)), 0.0f) : cf_t(0.0f, 0.0f);
        ASSERT_CF_EQ(expected, precoding.get_coefficient(i_layer, i_port));
      }
    }
  }
}

// Test the precoding matrix generated for a transmission using one layer and two antenna ports.
TEST(precoding_codebooks_test, OneLayerTwoPorts)
{
  // List of precoding matrix coefficients for one layer and two antenna ports indexed by the codebook index, as per
  // TS 38.214 Table 5.2.2.2.1-1.
  static constexpr cf_t coefficients[4][2] = {{cf_t(1.0f, 0.0f), cf_t(1.0f, 0.0f)},
                                              {cf_t(1.0f, 0.0f), cf_t(0.0f, 1.0f)},
                                              {cf_t(1.0f, 0.0f), cf_t(-1.0f, 0.0f)},
                                              {cf_t(1.0f, 0.0f), cf_t(0.0f, -1.0f)}};

  static constexpr float norm_factor = M_SQRT1_2f;

  for (unsigned i_codebook = 0; i_codebook != 4; ++i_codebook) {
    precoding_weight_matrix precoding = make_one_layer_two_ports(i_codebook);

    // Assert precoding matrix dimensions.
    ASSERT_EQ(precoding.get_nof_ports(), 2);
    ASSERT_EQ(precoding.get_nof_layers(), 1);

    // Assert first port coefficient.
    ASSERT_CF_EQ(precoding.get_coefficient(0, 0), coefficients[i_codebook][0] * norm_factor);
    // Assert second port coefficient.
    ASSERT_CF_EQ(precoding.get_coefficient(0, 1), coefficients[i_codebook][1] * norm_factor);
  }
}

// Test the precoding matrix generated for a transmission using two layers and two antenna ports.
TEST(precoding_codebooks_test, TwoLayerTwoPorts)
{
  // List of precoding matrix coefficients for two layer and two antenna ports indexed by the codebook index and layer,
  // as per TS 38.214 Table 5.2.2.2.1-1.
  static constexpr cf_t coefficients[2][2][2] = {
      {{cf_t(1.0f, 0.0f), cf_t(1.0f, 0.0f)}, {cf_t(1.0f, 0.0f), cf_t(-1.0f, 0.0f)}},
      {{cf_t(1.0f, 0.0f), cf_t(0.0f, 1.0f)}, {cf_t(1.0f, 0.0f), cf_t(0.0f, -1.0f)}},
  };

  static constexpr float norm_factor = 0.5f;

  for (unsigned i_codebook = 0; i_codebook != 2; ++i_codebook) {
    precoding_weight_matrix precoding = make_two_layer_two_ports(i_codebook);

    // Assert precoding matrix dimensions.
    ASSERT_EQ(precoding.get_nof_ports(), 2);
    ASSERT_EQ(precoding.get_nof_layers(), 2);

    // Assert first layer coefficients.
    ASSERT_CF_EQ(precoding.get_coefficient(0, 0), coefficients[i_codebook][0][0] * norm_factor);
    ASSERT_CF_EQ(precoding.get_coefficient(0, 1), coefficients[i_codebook][0][1] * norm_factor);

    // Assert second layer coefficients.
    ASSERT_CF_EQ(precoding.get_coefficient(1, 0), coefficients[i_codebook][1][0] * norm_factor);
    ASSERT_CF_EQ(precoding.get_coefficient(1, 1), coefficients[i_codebook][1][1] * norm_factor);
  }
}

// Test the precoding matrix generation for a transmission using one layer and four Type1 Single-Panel Mode1 antenna
// ports.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_OneLayerFourPorts)
{
  // Antenna configuration parameters, as per TS 38.214 Section 5.2.2.2.1.
  static constexpr unsigned N1             = 2;
  static constexpr unsigned N2             = 1;
  static constexpr unsigned O1             = 4;
  static constexpr unsigned O2             = 1;
  static constexpr unsigned nof_beams      = O1 * N1;
  static constexpr unsigned nof_pol_shifts = 4;
  static constexpr unsigned nof_ports      = 2 * N1;
  static constexpr unsigned i_1_2          = 0;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_ports));

  // Type I Single-Panel configuration based, corresponding to N1 = 2, N2 = 1.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::two_one;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
      precoding_matrix_indicator pmi = pmi_typeI_single_panel{
          .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = std::nullopt, .i_1_3 = std::nullopt, .i_2 = i_2};

      precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 1);

      // Assert precoding matrix dimensions.
      ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
      ASSERT_EQ(precoding.get_nof_layers(), 1);

      // Phase offset between polarizations.
      cf_t phi = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));

      std::vector<cf_t> expected_port_weights =
          get_layer_port_weights({N1, N2, O1, O2, i_1_1, i_1_2}, {scaling, scaling * phi});

      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        ASSERT_CF_EQ(precoding.get_coefficient(0, i_port), expected_port_weights[i_port]);
      }
    }
  }
}

// Test the precoding matrix generation for a transmission using two layers and four Type1 Single-Panel Mode1 antenna
// ports.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_TwoLayerFourPorts)
{
  // Antenna configuration parameters, as per TS 38.214 Table 5.2.2.2.1-6.
  static constexpr unsigned N1             = 2;
  static constexpr unsigned N2             = 1;
  static constexpr unsigned O1             = 4;
  static constexpr unsigned O2             = 1;
  static constexpr unsigned nof_beams      = O1 * N1;
  static constexpr unsigned nof_layers     = 2;
  static constexpr unsigned nof_ports      = 2 * N1;
  static constexpr unsigned i_1_2          = 0;
  static constexpr unsigned nof_pol_shifts = 2;
  static constexpr unsigned nof_offsets    = 2;

  // Beam offset between layers, as per TS 38.214 Table 5.2.2.2.1-4 for N1=2 and N2=1.
  static constexpr std::array<unsigned, nof_offsets> k1 = {0, O1};
  static const float scaling                            = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration based, corresponding to N1 = 2, N2 = 1.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::two_one;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_1_3 = 0; i_1_3 != nof_offsets; ++i_1_3) {
      for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
        precoding_matrix_indicator pmi = pmi_typeI_single_panel{
            .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = std::nullopt, .i_1_3 = i_1_3, .i_2 = i_2};

        precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 2);

        // Assert precoding matrix dimensions.
        ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
        ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

        // Per-layer beam indices and polarization phases, as per TS 38.214 Table 5.2.2.2.1-6.
        // E.g., layer 0: beam l = i_1_1, phi = pi/2 * i_2.
        cf_t     phi           = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
        unsigned layer_beam[2] = {i_1_1, i_1_1 + k1[i_1_3]};
        cf_t     layer_pol[2]  = {phi, -phi};

        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          std::vector<cf_t> expected_port_weights = get_layer_port_weights({N1, N2, O1, O2, layer_beam[i_layer], i_1_2},
                                                                           {scaling, scaling * layer_pol[i_layer]});

          for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
            ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
          }
        }
      }
    }
  }
}

// Test the precoding matrix generation for a transmission using three layers and four Type1 Single-Panel antenna ports.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_ThreeLayerFourPorts)
{
  // Antenna configuration parameters, as per TS 38.214 Table 5.2.2.2.1-7.
  static constexpr unsigned N1             = 2;
  static constexpr unsigned N2             = 1;
  static constexpr unsigned O1             = 4;
  static constexpr unsigned O2             = 1;
  static constexpr unsigned nof_beams      = O1 * N1;
  static constexpr unsigned nof_layers     = 3;
  static constexpr unsigned nof_ports      = 2 * N1;
  static constexpr unsigned i_1_2          = 0;
  static constexpr unsigned nof_pol_shifts = 2;
  // Beam offset between layers, as per TS 38.214 Table 5.2.2.2.1-4.
  static constexpr unsigned k1      = O1;
  static const float        scaling = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration based, corresponding to N1 = 2, N2 = 1.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::two_one;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
      precoding_matrix_indicator pmi = pmi_typeI_single_panel{
          .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = std::nullopt, .i_1_3 = std::nullopt, .i_2 = i_2};

      precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 3);

      // Assert precoding matrix dimensions.
      ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
      ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

      // Per-layer beam indices and polarization phases, as per TS 38.214 Table 5.2.2.2.1-7.
      // E.g., layer 0: beam l = i_1_1, phi = pi/2 * i_2.
      cf_t     phi           = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
      unsigned layer_beam[3] = {i_1_1, i_1_1 + k1, i_1_1};
      cf_t     layer_pol[3]  = {phi, phi, -phi};

      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        std::vector<cf_t> expected_port_weights = get_layer_port_weights({N1, N2, O1, O2, layer_beam[i_layer], i_1_2},
                                                                         {scaling, scaling * layer_pol[i_layer]});

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
        }
      }
    }
  }
}

// Test the precoding matrix generation for a transmission using four layers and four Type1 Single-Panel antenna ports.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_FourLayerFourPorts)
{
  // Antenna configuration parameters, as per TS 38.214 Table 5.2.2.2.1-8.
  static constexpr unsigned N1             = 2;
  static constexpr unsigned N2             = 1;
  static constexpr unsigned O1             = 4;
  static constexpr unsigned O2             = 1;
  static constexpr unsigned nof_beams      = O1 * N1;
  static constexpr unsigned nof_layers     = 4;
  static constexpr unsigned nof_ports      = 2 * N1;
  static constexpr unsigned i_1_2          = 0;
  static constexpr unsigned nof_pol_shifts = 2;
  // Beam offset between layers, as per TS 38.214 Table 5.2.2.2.1-4.
  static constexpr unsigned k1      = O1;
  static const float        scaling = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration based, corresponding to N1 = 2, N2 = 1.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::two_one;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
      precoding_matrix_indicator pmi = pmi_typeI_single_panel{
          .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = std::nullopt, .i_1_3 = std::nullopt, .i_2 = i_2};

      precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 4);

      // Assert precoding matrix dimensions.
      ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
      ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

      // Per-layer beam indices and polarization phases, as per TS 38.214 Table 5.2.2.2.1-8.
      // E.g., layer 0: beam l = i_1_1, phi = pi/2 * i_2.
      cf_t     phi           = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
      unsigned layer_beam[4] = {i_1_1, i_1_1 + k1, i_1_1, i_1_1 + k1};
      cf_t     layer_pol[4]  = {phi, phi, -phi, -phi};

      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        std::vector<cf_t> expected_port_weights = get_layer_port_weights({N1, N2, O1, O2, layer_beam[i_layer], i_1_2},
                                                                         {scaling, scaling * layer_pol[i_layer]});

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
        }
      }
    }
  }
}

// Test the precoding matrix generation for one layer and eight Type1 Single-Panel Mode1 antenna ports with N1=4, N2=1.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_OneLayer_4x1)
{
  // Antenna configuration: N1=4, N2=1, as per TS 38.214 Table 5.2.2.2.1-5.
  static constexpr unsigned N1             = 4;
  static constexpr unsigned N2             = 1;
  static constexpr unsigned O1             = 4;
  static constexpr unsigned O2             = 1;
  static constexpr unsigned nof_beams      = O1 * N1;
  static constexpr unsigned nof_ports      = 2 * N1;
  static constexpr unsigned i_1_2          = 0;
  static constexpr unsigned nof_pol_shifts = 4;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 4, N2 = 1.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::four_one;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
      precoding_matrix_indicator pmi = pmi_typeI_single_panel{
          .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = std::nullopt, .i_1_3 = std::nullopt, .i_2 = i_2};
      precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 1);

      ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
      ASSERT_EQ(precoding.get_nof_layers(), 1);

      // Layer 0: beam l = i_1_1, phi = pi/2 * i_2.
      cf_t phi = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));

      std::vector<cf_t> expected_port_weights =
          get_layer_port_weights({N1, N2, O1, O2, i_1_1, i_1_2}, {scaling, scaling * phi});

      for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
        ASSERT_CF_EQ(precoding.get_coefficient(0, i_port), expected_port_weights[i_port]);
      }
    }
  }
}

// Test the precoding matrix generation for two layers and eight Type1 Single-Panel Mode1 antenna ports with N1=4, N2=1.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_TwoLayer_4x1)
{
  // Antenna configuration: N1=4, N2=1, as per TS 38.214 Table 5.2.2.2.1-6.
  static constexpr unsigned N1             = 4;
  static constexpr unsigned N2             = 1;
  static constexpr unsigned O1             = 4;
  static constexpr unsigned O2             = 1;
  static constexpr unsigned nof_beams      = O1 * N1;
  static constexpr unsigned nof_layers     = 2;
  static constexpr unsigned nof_ports      = 2 * N1;
  static constexpr unsigned i_1_2          = 0;
  static constexpr unsigned nof_pol_shifts = 2;
  static constexpr unsigned nof_offsets    = 4;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 4, N2 = 1.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::four_one;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_1_3 = 0; i_1_3 != nof_offsets; ++i_1_3) {
      for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
        precoding_matrix_indicator pmi = pmi_typeI_single_panel{
            .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = std::nullopt, .i_1_3 = i_1_3, .i_2 = i_2};
        precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 2);

        ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
        ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

        // Per-layer configuration, as per TS 38.214 Table 5.2.2.2.1-6.
        // E.g., layer 0: beam l = i_1_1, phi = pi/2 * i_2.
        unsigned k1            = i_1_3 * O1;
        cf_t     phi           = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
        unsigned layer_beam[2] = {i_1_1, i_1_1 + k1};
        cf_t     layer_pol[2]  = {phi, -phi};

        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          std::vector<cf_t> expected_port_weights = get_layer_port_weights({N1, N2, O1, O2, layer_beam[i_layer], i_1_2},
                                                                           {scaling, scaling * layer_pol[i_layer]});

          for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
            ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
          }
        }
      }
    }
  }
}

// Test the precoding matrix generation for three layers and eight Type1 Single-Panel antenna ports with N1=4, N2=1.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_ThreeLayer_4x1)
{
  // Antenna configuration: N1=4, N2=1, as per TS 38.214 Table 5.2.2.2.1-7.
  static constexpr unsigned N1             = 4;
  static constexpr unsigned N2             = 1;
  static constexpr unsigned O1             = 4;
  static constexpr unsigned O2             = 1;
  static constexpr unsigned nof_beams      = O1 * N1;
  static constexpr unsigned nof_layers     = 3;
  static constexpr unsigned nof_ports      = 2 * N1;
  static constexpr unsigned i_1_2          = 0;
  static constexpr unsigned nof_pol_shifts = 2;
  static constexpr unsigned nof_offsets    = 3;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 4, N2 = 1.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::four_one;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_1_3 = 0; i_1_3 != nof_offsets; ++i_1_3) {
      for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
        precoding_matrix_indicator pmi = pmi_typeI_single_panel{
            .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = std::nullopt, .i_1_3 = i_1_3, .i_2 = i_2};
        precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 3);

        ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
        ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

        // Per-layer configuration, as per TS 38.214 Table 5.2.2.2.1-7.
        // E.g., layer 0: beam l = i_1_1, phi = pi/2 * i_2.
        unsigned k1            = (i_1_3 + 1) * O1;
        cf_t     phi           = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
        unsigned layer_beam[3] = {i_1_1, i_1_1 + k1, i_1_1};
        cf_t     layer_pol[3]  = {phi, phi, -phi};

        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          std::vector<cf_t> expected_port_weights = get_layer_port_weights({N1, N2, O1, O2, layer_beam[i_layer], i_1_2},
                                                                           {scaling, scaling * layer_pol[i_layer]});

          for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
            ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
          }
        }
      }
    }
  }
}

// Test the precoding matrix generation for four layers and eight Type1 Single-Panel antenna ports with N1=4, N2=1.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_FourLayer_4x1)
{
  // Antenna configuration: N1=4, N2=1, as per TS 38.214 Table 5.2.2.2.1-8.
  static constexpr unsigned N1             = 4;
  static constexpr unsigned N2             = 1;
  static constexpr unsigned O1             = 4;
  static constexpr unsigned O2             = 1;
  static constexpr unsigned nof_beams      = O1 * N1;
  static constexpr unsigned nof_layers     = 4;
  static constexpr unsigned nof_ports      = 2 * N1;
  static constexpr unsigned i_1_2          = 0;
  static constexpr unsigned nof_pol_shifts = 2;
  static constexpr unsigned nof_offsets    = 3;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 4, N2 = 1.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::four_one;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_1_3 = 0; i_1_3 != nof_offsets; ++i_1_3) {
      for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
        precoding_matrix_indicator pmi = pmi_typeI_single_panel{
            .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = std::nullopt, .i_1_3 = i_1_3, .i_2 = i_2};
        precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 4);

        ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
        ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

        // Per-layer configuration, as per TS 38.214 Table 5.2.2.2.1-8.
        // E.g., layer 0: beam l = i_1_1, phi = pi/2 * i_2.
        unsigned k1            = (i_1_3 + 1) * O1;
        cf_t     phi           = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
        unsigned layer_beam[4] = {i_1_1, i_1_1 + k1, i_1_1, i_1_1 + k1};
        cf_t     layer_pol[4]  = {phi, phi, -phi, -phi};

        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          std::vector<cf_t> expected_port_weights = get_layer_port_weights({N1, N2, O1, O2, layer_beam[i_layer], i_1_2},
                                                                           {scaling, scaling * layer_pol[i_layer]});

          for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
            ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
          }
        }
      }
    }
  }
}

// Test the precoding matrix generation for five layers and eight Type1 Single-Panel antenna ports with N1=4, N2=1.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_FiveLayer_4x1)
{
  // Antenna configuration: N1=4, N2=1, as per TS 38.214 Table 5.2.2.2.1-9.
  static constexpr unsigned N1             = 4;
  static constexpr unsigned N2             = 1;
  static constexpr unsigned O1             = 4;
  static constexpr unsigned O2             = 1;
  static constexpr unsigned nof_beams      = O1 * N1;
  static constexpr unsigned nof_layers     = 5;
  static constexpr unsigned nof_ports      = 2 * N1;
  static constexpr unsigned i_1_2          = 0;
  static constexpr unsigned nof_pol_shifts = 2;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 4, N2 = 1.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::four_one;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
      precoding_matrix_indicator pmi = pmi_typeI_single_panel{
          .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = std::nullopt, .i_1_3 = std::nullopt, .i_2 = i_2};
      precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 5);

      ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
      ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

      // Per-layer configuration, as per TS 38.214 Table 5.2.2.2.1-9.
      // E.g., layer 0: beam l = i_1_1, phi = pi/2 * i_2.
      cf_t     phi           = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
      unsigned layer_beam[5] = {i_1_1, i_1_1, i_1_1 + O1, i_1_1 + O1, i_1_1 + 2 * O1};
      cf_t     layer_pol[5]  = {phi, -phi, 1.0f, -1.0f, 1.0f};

      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        std::vector<cf_t> expected_port_weights = get_layer_port_weights({N1, N2, O1, O2, layer_beam[i_layer], i_1_2},
                                                                         {scaling, scaling * layer_pol[i_layer]});

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
        }
      }
    }
  }
}

// Test the precoding matrix generation for six layers and eight Type1 Single-Panel antenna ports with N1=4, N2=1.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_SixLayer_4x1)
{
  // Antenna configuration: N1=4, N2=1, as per TS 38.214 Table 5.2.2.2.1-10.
  static constexpr unsigned N1             = 4;
  static constexpr unsigned N2             = 1;
  static constexpr unsigned O1             = 4;
  static constexpr unsigned O2             = 1;
  static constexpr unsigned nof_beams      = O1 * N1;
  static constexpr unsigned nof_layers     = 6;
  static constexpr unsigned nof_ports      = 2 * N1;
  static constexpr unsigned i_1_2          = 0;
  static constexpr unsigned nof_pol_shifts = 2;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 4, N2 = 1.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::four_one;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
      precoding_matrix_indicator pmi = pmi_typeI_single_panel{
          .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = std::nullopt, .i_1_3 = std::nullopt, .i_2 = i_2};
      precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 6);

      ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
      ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

      // Per-layer configuration, as per TS 38.214 Table 5.2.2.2.1-10.
      // E.g., layer 0: beam l = i_1_1,          phi = pi/2 * i_2.
      cf_t     phi           = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
      unsigned layer_beam[6] = {i_1_1, i_1_1, i_1_1 + O1, i_1_1 + O1, i_1_1 + 2 * O1, i_1_1 + 2 * O1};
      cf_t     layer_pol[6]  = {phi, -phi, phi, -phi, 1.0f, -1.0f};

      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        std::vector<cf_t> expected_port_weights = get_layer_port_weights({N1, N2, O1, O2, layer_beam[i_layer], i_1_2},
                                                                         {scaling, scaling * layer_pol[i_layer]});

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
        }
      }
    }
  }
}

// Test the precoding matrix generation for seven layers and eight Type1 Single-Panel antenna ports with N1=4, N2=1.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_SevenLayer_4x1)
{
  // Antenna configuration: N1=4, N2=1, as per TS 38.214 Table 5.2.2.2.1-11.
  static constexpr unsigned N1             = 4;
  static constexpr unsigned N2             = 1;
  static constexpr unsigned O1             = 4;
  static constexpr unsigned O2             = 1;
  static constexpr unsigned nof_beams      = O1 * N1 / 2;
  static constexpr unsigned nof_layers     = 7;
  static constexpr unsigned nof_ports      = 2 * N1;
  static constexpr unsigned i_1_2          = 0;
  static constexpr unsigned nof_pol_shifts = 2;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 4, N2 = 1.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::four_one;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
      precoding_matrix_indicator pmi = pmi_typeI_single_panel{
          .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = std::nullopt, .i_1_3 = std::nullopt, .i_2 = i_2};
      precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 7);

      ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
      ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

      // Per-layer configuration, as per TS 38.214 Table 5.2.2.2.1-11.
      // E.g., layer 0: beam l = i_1_1, phi = pi/2 * i_2.
      cf_t     phi           = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
      unsigned layer_beam[7] = {
          i_1_1, i_1_1, i_1_1 + O1, i_1_1 + 2 * O1, i_1_1 + 2 * O1, i_1_1 + 3 * O1, i_1_1 + 3 * O1};
      cf_t layer_pol[7] = {phi, -phi, phi, 1.0f, -1.0f, 1.0f, -1.0f};

      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        std::vector<cf_t> expected_port_weights = get_layer_port_weights({N1, N2, O1, O2, layer_beam[i_layer], i_1_2},
                                                                         {scaling, scaling * layer_pol[i_layer]});

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
        }
      }
    }
  }
}

// Test the precoding matrix generation for eight layers and eight Type1 Single-Panel antenna ports with N1=4, N2=1.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_EightLayer_4x1)
{
  // Antenna configuration: N1=4, N2=1, as per TS 38.214 Table 5.2.2.2.1-12.
  static constexpr unsigned N1             = 4;
  static constexpr unsigned N2             = 1;
  static constexpr unsigned O1             = 4;
  static constexpr unsigned O2             = 1;
  static constexpr unsigned nof_beams      = O1 * N1 / 2;
  static constexpr unsigned nof_layers     = 8;
  static constexpr unsigned nof_ports      = 2 * N1;
  static constexpr unsigned i_1_2          = 0;
  static constexpr unsigned nof_pol_shifts = 2;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 4, N2 = 1.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::four_one;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
      precoding_matrix_indicator pmi = pmi_typeI_single_panel{
          .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = std::nullopt, .i_1_3 = std::nullopt, .i_2 = i_2};
      precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 8);

      ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
      ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

      // Per-layer configuration, as per TS 38.214 Table 5.2.2.2.1-12.
      // E.g., layer 0: beam l = i_1_1, phi = pi/2 * i_2.
      cf_t     phi           = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
      unsigned layer_beam[8] = {
          i_1_1, i_1_1, i_1_1 + O1, i_1_1 + O1, i_1_1 + 2 * O1, i_1_1 + 2 * O1, i_1_1 + 3 * O1, i_1_1 + 3 * O1};
      cf_t layer_pol[8] = {phi, -phi, phi, -phi, 1.0f, -1.0f, 1.0f, -1.0f};

      for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
        std::vector<cf_t> expected_port_weights = get_layer_port_weights({N1, N2, O1, O2, layer_beam[i_layer], i_1_2},
                                                                         {scaling, scaling * layer_pol[i_layer]});

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
        }
      }
    }
  }
}

// Test the precoding matrix generation for one layer and eight Type1 Single-Panel Mode1 antenna ports with N1=2, N2=2.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_OneLayer_2x2)
{
  // Antenna configuration: N1=2, N2=2, O1=O2=4, as per TS 38.214 Table 5.2.2.2.1-5.
  static constexpr unsigned N1             = 2;
  static constexpr unsigned N2             = 2;
  static constexpr unsigned O              = 4;
  static constexpr unsigned nof_beams      = O * N1;
  static constexpr unsigned nof_ports      = 2 * N1 * N2;
  static constexpr unsigned nof_pol_shifts = 4;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 2, N2 = 2.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::two_two;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_1_2 = 0; i_1_2 != nof_beams; ++i_1_2) {
      for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
        precoding_matrix_indicator pmi = pmi_typeI_single_panel{
            .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = i_1_2, .i_1_3 = std::nullopt, .i_2 = i_2};
        precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 1);

        ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
        ASSERT_EQ(precoding.get_nof_layers(), 1);

        // Layer 0: beam_h = i_1_1, beam_v = i_1_2, phi = pi/2 * i_2.
        cf_t phi = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));

        std::vector<cf_t> expected_port_weights =
            get_layer_port_weights({N1, N2, O, O, i_1_1, i_1_2}, {scaling, scaling * phi});

        for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
          ASSERT_CF_EQ(precoding.get_coefficient(0, i_port), expected_port_weights[i_port]);
        }
      }
    }
  }
}

// Test the precoding matrix generation for two layers and eight Type1 Single-Panel Mode1 antenna ports with N1=2, N2=2.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_TwoLayer_2x2)
{
  // Antenna configuration: N1=2, N2=2, O1=O2=4, as per TS 38.214 Table 5.2.2.2.1-6.
  static constexpr unsigned N1             = 2;
  static constexpr unsigned N2             = 2;
  static constexpr unsigned O              = 4;
  static constexpr unsigned nof_beams      = O * N1;
  static constexpr unsigned nof_layers     = 2;
  static constexpr unsigned nof_ports      = 2 * N1 * N2;
  static constexpr unsigned nof_pol_shifts = 2;
  static constexpr unsigned nof_offsets    = 4;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 2, N2 = 2.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::two_two;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_1_2 = 0; i_1_2 != nof_beams; ++i_1_2) {
      for (unsigned i_1_3 = 0; i_1_3 != nof_offsets; ++i_1_3) {
        for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
          precoding_matrix_indicator pmi = pmi_typeI_single_panel{
              .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = i_1_2, .i_1_3 = i_1_3, .i_2 = i_2};
          precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 2);

          ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
          ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

          // Per-layer configuration, as per TS 38.214 Table 5.2.2.2.1-6.
          // E.g., layer 0: beam_h = i_1_1, beam_v = i_1_2, phi = pi/2 * i_2.
          unsigned k1              = (i_1_3 % 2 == 0) ? 0 : O;
          unsigned k2              = (i_1_3 < 2) ? 0 : O;
          cf_t     phi             = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
          unsigned layer_beam_h[2] = {i_1_1, i_1_1 + k1};
          unsigned layer_beam_v[2] = {i_1_2, i_1_2 + k2};
          cf_t     layer_pol[2]    = {phi, -phi};

          for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
            std::vector<cf_t> expected_port_weights = get_layer_port_weights(
                {N1, N2, O, O, layer_beam_h[i_layer], layer_beam_v[i_layer]}, {scaling, scaling * layer_pol[i_layer]});

            for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
              ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
            }
          }
        }
      }
    }
  }
}

// Test the precoding matrix generation for three layers and eight Type1 Single-Panel antenna ports with N1=2, N2=2.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_ThreeLayer_2x2)
{
  // Antenna configuration: N1=2, N2=2, O1=O2=4, as per TS 38.214 Table 5.2.2.2.1-7.
  static constexpr unsigned N1             = 2;
  static constexpr unsigned N2             = 2;
  static constexpr unsigned O              = 4;
  static constexpr unsigned nof_beams      = O * N1;
  static constexpr unsigned nof_layers     = 3;
  static constexpr unsigned nof_ports      = 2 * N1 * N2;
  static constexpr unsigned nof_pol_shifts = 2;
  static constexpr unsigned nof_offsets    = 3;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 2, N2 = 2.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::two_two;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_1_2 = 0; i_1_2 != nof_beams; ++i_1_2) {
      for (unsigned i_1_3 = 0; i_1_3 != nof_offsets; ++i_1_3) {
        for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
          precoding_matrix_indicator pmi = pmi_typeI_single_panel{
              .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = i_1_2, .i_1_3 = i_1_3, .i_2 = i_2};
          precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 3);

          ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
          ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

          // Per-layer configuration, as per TS 38.214 Table 5.2.2.2.1-7.
          // E.g., layer 0: beam_h = i_1_1, beam_v = i_1_2, phi = pi/2 * i_2.
          unsigned k1              = (i_1_3 == 1) ? 0 : O;
          unsigned k2              = (i_1_3 == 0) ? 0 : O;
          cf_t     phi             = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
          unsigned layer_beam_h[3] = {i_1_1, i_1_1 + k1, i_1_1};
          unsigned layer_beam_v[3] = {i_1_2, i_1_2 + k2, i_1_2};
          cf_t     layer_pol[3]    = {phi, phi, -phi};

          for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
            std::vector<cf_t> expected_port_weights = get_layer_port_weights(
                {N1, N2, O, O, layer_beam_h[i_layer], layer_beam_v[i_layer]}, {scaling, scaling * layer_pol[i_layer]});

            for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
              ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
            }
          }
        }
      }
    }
  }
}

// Test the precoding matrix generation for four layers and eight Type1 Single-Panel antenna ports with N1=2, N2=2.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_FourLayer_2x2)
{
  // Antenna configuration: N1=2, N2=2, O1=O2=4, as per TS 38.214 Table 5.2.2.2.1-8.
  static constexpr unsigned N1             = 2;
  static constexpr unsigned N2             = 2;
  static constexpr unsigned O              = 4;
  static constexpr unsigned nof_beams      = O * N1;
  static constexpr unsigned nof_layers     = 4;
  static constexpr unsigned nof_ports      = 2 * N1 * N2;
  static constexpr unsigned nof_pol_shifts = 2;
  static constexpr unsigned nof_offsets    = 3;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 2, N2 = 2.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::two_two;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_1_2 = 0; i_1_2 != nof_beams; ++i_1_2) {
      for (unsigned i_1_3 = 0; i_1_3 != nof_offsets; ++i_1_3) {
        for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
          precoding_matrix_indicator pmi = pmi_typeI_single_panel{
              .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = i_1_2, .i_1_3 = i_1_3, .i_2 = i_2};
          precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 4);

          ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
          ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

          // Per-layer configuration, as per TS 38.214 Table 5.2.2.2.1-8.
          // E.g., layer 0: beam_h = i_1_1, beam_v = i_1_2, phi = pi/2 * i_2.
          unsigned k1              = (i_1_3 == 1) ? 0 : O;
          unsigned k2              = (i_1_3 == 0) ? 0 : O;
          cf_t     phi             = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
          unsigned layer_beam_h[4] = {i_1_1, i_1_1 + k1, i_1_1, i_1_1 + k1};
          unsigned layer_beam_v[4] = {i_1_2, i_1_2 + k2, i_1_2, i_1_2 + k2};
          cf_t     layer_pol[4]    = {phi, phi, -phi, -phi};

          for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
            std::vector<cf_t> expected_port_weights = get_layer_port_weights(
                {N1, N2, O, O, layer_beam_h[i_layer], layer_beam_v[i_layer]}, {scaling, scaling * layer_pol[i_layer]});

            for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
              ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
            }
          }
        }
      }
    }
  }
}

// Test the precoding matrix generation for five layers and eight Type1 Single-Panel antenna ports with N1=2, N2=2.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_FiveLayer_2x2)
{
  // Antenna configuration: N1=2, N2=2, O1=O2=4, as per TS 38.214 Table 5.2.2.2.1-9.
  static constexpr unsigned N1             = 2;
  static constexpr unsigned N2             = 2;
  static constexpr unsigned O              = 4;
  static constexpr unsigned nof_beams      = O * N1;
  static constexpr unsigned nof_layers     = 5;
  static constexpr unsigned nof_ports      = 2 * N1 * N2;
  static constexpr unsigned nof_pol_shifts = 2;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 2, N2 = 2.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::two_two;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_1_2 = 0; i_1_2 != nof_beams; ++i_1_2) {
      for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
        precoding_matrix_indicator pmi = pmi_typeI_single_panel{
            .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = i_1_2, .i_1_3 = std::nullopt, .i_2 = i_2};
        precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 5);

        ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
        ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

        // Per-layer configuration, as per TS 38.214 Table 5.2.2.2.1-9.
        // E.g., layer 0: beam_h = i_1_1, beam_v = i_1_2, phi = pi/2 * i_2.
        cf_t     phi             = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
        unsigned layer_beam_h[5] = {i_1_1, i_1_1, i_1_1 + O, i_1_1 + O, i_1_1 + O};
        unsigned layer_beam_v[5] = {i_1_2, i_1_2, i_1_2, i_1_2, i_1_2 + O};
        cf_t     layer_pol[5]    = {phi, -phi, 1.0f, -1.0f, 1.0f};

        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          std::vector<cf_t> expected_port_weights = get_layer_port_weights(
              {N1, N2, O, O, layer_beam_h[i_layer], layer_beam_v[i_layer]}, {scaling, scaling * layer_pol[i_layer]});

          for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
            ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
          }
        }
      }
    }
  }
}

// Test the precoding matrix generation for six layers and eight Type1 Single-Panel antenna ports with N1=2, N2=2.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_SixLayer_2x2)
{
  // Antenna configuration: N1=2, N2=2, O1=O2=4, as per TS 38.214 Table 5.2.2.2.1-10.
  static constexpr unsigned N1             = 2;
  static constexpr unsigned N2             = 2;
  static constexpr unsigned O              = 4;
  static constexpr unsigned nof_beams      = O * N1;
  static constexpr unsigned nof_layers     = 6;
  static constexpr unsigned nof_ports      = 2 * N1 * N2;
  static constexpr unsigned nof_pol_shifts = 2;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 2, N2 = 2.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::two_two;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_1_2 = 0; i_1_2 != nof_beams; ++i_1_2) {
      for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
        precoding_matrix_indicator pmi = pmi_typeI_single_panel{
            .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = i_1_2, .i_1_3 = std::nullopt, .i_2 = i_2};
        precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 6);

        ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
        ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

        // Per-layer configuration, as per TS 38.214 Table 5.2.2.2.1-10.
        // E.g., layer 0: beam_h = i_1_1, beam_v = i_1_2, phi = pi/2 * i_2.
        cf_t     phi             = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
        unsigned layer_beam_h[6] = {i_1_1, i_1_1, i_1_1 + O, i_1_1 + O, i_1_1 + O, i_1_1 + O};
        unsigned layer_beam_v[6] = {i_1_2, i_1_2, i_1_2, i_1_2, i_1_2 + O, i_1_2 + O};
        cf_t     layer_pol[6]    = {phi, -phi, phi, -phi, 1.0f, -1.0f};

        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          std::vector<cf_t> expected_port_weights = get_layer_port_weights(
              {N1, N2, O, O, layer_beam_h[i_layer], layer_beam_v[i_layer]}, {scaling, scaling * layer_pol[i_layer]});

          for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
            ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
          }
        }
      }
    }
  }
}

// Test the precoding matrix generation for seven layers and eight Type1 Single-Panel antenna ports with N1=2, N2=2.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_SevenLayer_2x2)
{
  // Antenna configuration: N1=2, N2=2, O1=O2=4, as per TS 38.214 Table 5.2.2.2.1-11.
  static constexpr unsigned N1             = 2;
  static constexpr unsigned N2             = 2;
  static constexpr unsigned O              = 4;
  static constexpr unsigned nof_beams      = O * N1;
  static constexpr unsigned nof_layers     = 7;
  static constexpr unsigned nof_ports      = 2 * N1 * N2;
  static constexpr unsigned nof_pol_shifts = 2;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 2, N2 = 2.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::two_two;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_1_2 = 0; i_1_2 != nof_beams; ++i_1_2) {
      for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
        precoding_matrix_indicator pmi = pmi_typeI_single_panel{
            .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = i_1_2, .i_1_3 = std::nullopt, .i_2 = i_2};
        precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 7);

        ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
        ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

        // Per-layer configuration, as per TS 38.214 Table 5.2.2.2.1-11.
        // E.g., layer 0: beam_h = i_1_1, beam_v = i_1_2, phi = pi/2 * i_2.
        cf_t     phi             = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
        unsigned layer_beam_h[7] = {i_1_1, i_1_1, i_1_1 + O, i_1_1, i_1_1, i_1_1 + O, i_1_1 + O};
        unsigned layer_beam_v[7] = {i_1_2, i_1_2, i_1_2, i_1_2 + O, i_1_2 + O, i_1_2 + O, i_1_2 + O};
        cf_t     layer_pol[7]    = {phi, -phi, phi, 1.0f, -1.0f, 1.0f, -1.0f};

        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          std::vector<cf_t> expected_port_weights = get_layer_port_weights(
              {N1, N2, O, O, layer_beam_h[i_layer], layer_beam_v[i_layer]}, {scaling, scaling * layer_pol[i_layer]});

          for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
            ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
          }
        }
      }
    }
  }
}

// Test the precoding matrix generation for eight layers and eight Type1 Single-Panel antenna ports with N1=2, N2=2.
TEST(precoding_codebooks_test, Type1SinglePanelMode1_EightLayer_2x2)
{
  // Antenna configuration: N1=2, N2=2, O1=O2=4, as per TS 38.214 Table 5.2.2.2.1-12.
  static constexpr unsigned N1             = 2;
  static constexpr unsigned N2             = 2;
  static constexpr unsigned O              = 4;
  static constexpr unsigned nof_beams      = O * N1;
  static constexpr unsigned nof_layers     = 8;
  static constexpr unsigned nof_ports      = 2 * N1 * N2;
  static constexpr unsigned nof_pol_shifts = 2;
  static const float        scaling        = 1.0F / std::sqrt(static_cast<float>(nof_layers * nof_ports));

  // Type I Single-Panel configuration corresponding to N1 = 2, N2 = 2.
  pmi_codebook_single_panel_config panel_config = pmi_codebook_single_panel_config::two_two;

  for (unsigned i_1_1 = 0; i_1_1 != nof_beams; ++i_1_1) {
    for (unsigned i_1_2 = 0; i_1_2 != nof_beams; ++i_1_2) {
      for (unsigned i_2 = 0; i_2 != nof_pol_shifts; ++i_2) {
        precoding_matrix_indicator pmi = pmi_typeI_single_panel{
            .panel_config = panel_config, .i_1_1 = i_1_1, .i_1_2 = i_1_2, .i_1_3 = std::nullopt, .i_2 = i_2};
        precoding_weight_matrix precoding = make_type1_sp_mode1(pmi, 8);

        ASSERT_EQ(precoding.get_nof_ports(), nof_ports);
        ASSERT_EQ(precoding.get_nof_layers(), nof_layers);

        // Per-layer configuration, as per TS 38.214 Table 5.2.2.2.1-12.
        // E.g., layer 0: beam_h = i_1_1, beam_v = i_1_2, phi = pi/2 * i_2.
        cf_t     phi             = std::polar(1.0f, static_cast<float>(M_PI_2) * static_cast<float>(i_2));
        unsigned layer_beam_h[8] = {i_1_1, i_1_1, i_1_1 + O, i_1_1 + O, i_1_1, i_1_1, i_1_1 + O, i_1_1 + O};
        unsigned layer_beam_v[8] = {i_1_2, i_1_2, i_1_2, i_1_2, i_1_2 + O, i_1_2 + O, i_1_2 + O, i_1_2 + O};
        cf_t     layer_pol[8]    = {phi, -phi, phi, -phi, 1.0f, -1.0f, 1.0f, -1.0f};

        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          std::vector<cf_t> expected_port_weights = get_layer_port_weights(
              {N1, N2, O, O, layer_beam_h[i_layer], layer_beam_v[i_layer]}, {scaling, scaling * layer_pol[i_layer]});

          for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
            ASSERT_CF_EQ(precoding.get_coefficient(i_layer, i_port), expected_port_weights[i_port]);
          }
        }
      }
    }
  }
}

#ifdef ASSERTS_ENABLED
TEST(precoding_codebooks_test, TypeISinglePanelMode1_UnsupportedCases)
{
  // Use thread safe death test.
  ::testing::GTEST_FLAG(death_test_style) = "threadsafe";

  // Create an example PMI with a Type I Single-Panel configuration corresponding to N1 = 4, N2 = 2, which is
  // unsupported.
  ASSERT_DEATH(make_type1_sp_mode1(pmi_typeI_single_panel{.panel_config = pmi_codebook_single_panel_config::four_two,
                                                          .i_1_1        = 0,
                                                          .i_1_2        = 0,
                                                          .i_1_3        = std::nullopt,
                                                          .i_2          = 0},
                                   4),
               R"(The given number of ports \(i\.e\., 16\) is out of the range of supported values \(1\.\.8\).)");

  // Try to generate a precoding for a higher number of layers than ports. Check that an assert is triggered, and it is
  // actually the one expected.
  ASSERT_DEATH(
      make_type1_sp_mode1(pmi_typeI_single_panel{.panel_config = pmi_codebook_single_panel_config::two_one,
                                                 .i_1_1        = 0,
                                                 .i_1_2        = 0,
                                                 .i_1_3        = std::nullopt,
                                                 .i_2          = 0},
                          8),
      R"(The given number of layers \(i\.e\., 8\) for the given number of ports \(i\.e\., 4) is out of the range of supported values \(1\.\.4\).)");

  // Try to generate a precoding for a 2D panel configuration (N1 = 2, N2 = 2) without the required i_1_2. Check that an
  // assert is triggered, and it is actually the one expected.
  ASSERT_DEATH(make_type1_sp_mode1(pmi_typeI_single_panel{.panel_config = pmi_codebook_single_panel_config::two_two,
                                                          .i_1_1        = 0,
                                                          .i_1_2        = std::nullopt,
                                                          .i_1_3        = std::nullopt,
                                                          .i_2          = 0},
                                   5),
               R"(Missing parameter i_1_2 for a 2D antenna panel distribution \(i\.e\., N2 = 2\)\.)");

  // For a 3 or 4 layer transmission using 4 antenna ports (N1 = 2, N2 = 1), the beam offset selector can only take the
  // zero value. Check that an assert is triggered, and it is actually the one expected.
  ASSERT_DEATH(
      make_type1_sp_mode1(pmi_typeI_single_panel{.panel_config = pmi_codebook_single_panel_config::two_one,
                                                 .i_1_1        = 0,
                                                 .i_1_2        = std::nullopt,
                                                 .i_1_3        = 1,
                                                 .i_2          = 0},
                          3),
      R"(For a 3 layer transmission using 4 antenna ports \(N1 = 2, N2 = 1\), the beam offset selector \(i_1_3\) can only take the zero value\.)");
}
#endif
