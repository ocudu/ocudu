// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief SRS-based DOA estimator: implementation leveraging the Eigen library for linear algebra
/// > https://libeigen.gitlab.io/

#pragma once

#include "ocudu/phy/upper/signal_processors/srs/doa_estimator.h"
#include "ocudu/phy/upper/signal_processors/srs/doa_estimator_configuration.h"
#include <eigen3/Eigen/Eigenvalues>

namespace ocudu {

/// \brief Direction of Arrival (DOA) estimator for Sounding Reference Signals: Eigen-based implementation.
///
/// The estimation assumes the receive antennas are arranged as a Uniform Linear Array (ULA). The array geometry (number
/// of antennas and distance between elements) are specified at construction time, as well as the carrier frequency. The
/// estimator supports both single- and cross-polarized antennas; in the latter case, the number of antennas must be
/// even and the antenna ports are assumed to be indexed such that pairs of consecutive port indices correspond to two
/// collocated antennas with orthogonal polarizations.
///
/// The estimation builds on the well-known [MUSIC algorithm](https://en.wikipedia.org/wiki/MUSIC_(algorithm)) and is
/// implemented via the [Eigen library](https://libeigen.gitlab.io/) for linear algebra.
class doa_estimator_eigen : public doa_estimator
{
public:
  using data_matrix_type = tensor<2, cf_t>;

  /// Constructor: Configures the estimator and reserves internal memory.
  explicit doa_estimator_eigen(const doa_estimator_configuration& config_in);

  // See the doa_estimator interface for documentation.
  std::optional<doa_estimator_result> estimate(const data_matrix_type& data) override;

private:
  /// \brief Computes the MUSIC spectrum for single-polarization antenna arrays.
  /// \param[in] nof_noise_eigen       Number of noise eigenvalues.
  /// \param[in] angle                 Broadside angle (between \f$-\pi/2\f$ and \f$\pi/2\f$ radians) at which the
  /// spectrum is computed.
  /// \param[in] distance_over_lambda  Distance between antenna elements, normalized with respect to the wavelength.
  /// \return The computed MUSIC spectrum value.
  float compute_simple(unsigned nof_noise_eigen, float angle, float distance_over_lambda);

  /// \brief Computes the MUSIC spectrum for cross-polarization antenna arrays.
  /// \param[in] nof_noise_eigen       Number of noise eigenvalues.
  /// \param[in] angle                 Broadside angle (between \f$-\pi/2\f$ and \f$\pi/2\f$ radians) at which the
  /// spectrum is computed.
  /// \param[in] distance_over_lambda  Distance between antenna elements, normalized with respect to the wavelength.
  /// \return The computed MUSIC spectrum value.
  float compute_polarized(unsigned nof_noise_eigen, float angle, float distance_over_lambda);

  /// \brief Number of points at which the MUSIC spectrum is computed.
  ///
  /// The number 181 corresponds to sweeping the spectrum with 1-degree granularity.
  static constexpr unsigned nof_angles = 181;

  /// Estimator configuration.
  doa_estimator_configuration config;

  /// Eigendecomposition solver.
  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXcf> eigen_solver;
  /// Memory for the sample covariance matrix.
  Eigen::MatrixXcf sample_covariance;
  /// Common term of the DOA signature.
  Eigen::VectorXcf signature_base;
  /// Memory for the DOA signature at a given angle.
  Eigen::VectorXcf signature;
  /// Helper memory for intermediate computations.
  Eigen::VectorXcf noise_projection_buffer;
  /// Helper memory for intermediate computations.
  Eigen::VectorXcf noise_projection_buffer2;
  /// List of angles values.
  std::array<float, nof_angles> angle_list;
  /// List of computed spectrum values.
  std::array<float, nof_angles> spectrum;
};

} // namespace ocudu
