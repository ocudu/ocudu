// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "doa_estimator_eigen.h"
#include "ocudu/adt/format.h"

using namespace ocudu;

/// \brief Splits the given eigenvalues into noise and signal eigenvalues.
///
/// The eigenvalues are assumed in increasing order.
/// \return The number of eigenvalues associated to noise.
static unsigned find_noise_eigenvalues(span<const float> eig_values);

/// \brief Finds the peaks (relative maxima) in the spectrum array.
/// \param[out]   peaks_ix   Indices of the detected peaks, in decreasing order of the peak value.
/// \param[in]    spectrum   Spectrum values.
/// \param[in]    max_peaks  Maximum number of peaks to search.
/// \return The estimated number of peaks.
static unsigned find_peaks(span<unsigned> peak_ix, span<const float> spectrum, unsigned max_peaks);

doa_estimator_eigen::doa_estimator_eigen(const doa_estimator_configuration& config_in) :
  config(config_in),
  eigen_solver(config_in.nof_antennas),
  sample_covariance(config_in.nof_antennas, config_in.nof_antennas),
  signature(config_in.nof_antennas),
  noise_projection_buffer(config_in.nof_antennas),
  noise_projection_buffer2(config_in.nof_antennas)
{
  ocudu_assert(config.nof_antennas > 0, "Invalid number of antenna elements {}", config.nof_antennas);
  ocudu_assert(!config.cross_polarized || (config.nof_antennas % 2 == 0),
               "For cross-polarized antenna arrays, the number of antenna elements should be even - provided {}.",
               config.nof_antennas);
  ocudu_assert(config.antenna_distance_over_wavelength > 0,
               "Negative antenna distance {}.",
               config.antenna_distance_over_wavelength);

  // Fill the angle list to sweep the range -pi/2:pi/2 with the given granularity.
  std::generate(angle_list.begin(), angle_list.end(), [n = 0, g = 1.0F / (nof_angles - 1)]() mutable {
    return ((-0.5F + g * n++) * M_PI);
  });

  // Compute the base signature term for single- and cross-polarized antenna arrays.
  if (config.cross_polarized) {
    unsigned nof_elements = config_in.nof_antennas / 2;
    signature.resize(nof_elements);
    signature_base = cf_t(0.0F, -2.0F * M_PI) * Eigen::VectorXf::LinSpaced(nof_elements, 0, nof_elements - 1);
  } else {
    signature.resize(config_in.nof_antennas);
    signature_base =
        cf_t(0.0F, -2.0F * M_PI) * Eigen::VectorXf::LinSpaced(config_in.nof_antennas, 0, config_in.nof_antennas - 1);
  }
}

std::optional<doa_estimator_result> doa_estimator_eigen::estimate(const data_matrix_type& data)
{
  using namespace Eigen;

  unsigned                               nof_antennas = config.nof_antennas;
  data_matrix_type::dimensions_size_type data_size    = data.get_dimensions_size();
  unsigned                               srs_size     = data_size[0];
  ocudu_assert(data_size[1] == nof_antennas,
               "The number of data columns {} should be equal to the configured number of antennas {}.",
               data_size[1],
               nof_antennas);

  // Map the inputs as Eigen matrices.
  Map<const MatrixXcf> data_map(data.get_view<2>({}).data(), data_size[0], data_size[1]);

  // Compute the sample covariance matrix from the input data: covariance matrices are Hermitian (self-adjoint), so we
  // can work with the lower triangular part of it only.
  // Unfortunately, the single rank-k update
  //     sample_covariance.selfadjointView<Eigen::Lower>().rankUpdate(data_map.transpose());
  // seems to require MALLOC when the number of antennas is larger than 8 (likely because the required buffer exceeds
  // Eigen's stack allocation limit), so we implement it by accumulating the rank update for each row.
  sample_covariance.triangularView<Eigen::Lower>() = data_map.row(0).transpose() * data_map.row(0).conjugate();
  for (unsigned i_sample = 1; i_sample != srs_size; ++i_sample) {
    sample_covariance.selfadjointView<Eigen::Lower>().rankUpdate(data_map.row(i_sample).transpose());
  }

  sample_covariance.triangularView<Eigen::Lower>() *= (1.0F / srs_size);

  // Compute eigendecomposition of the sample covariance matrix.
  eigen_solver.compute(sample_covariance);

  // Early exit if the eigendecomposition wasn't successful.
  if (eigen_solver.info() != ComputationInfo::Success) {
    return std::nullopt;
  }

  span<const float> eigen_values(eigen_solver.eigenvalues().data(), nof_antennas);

  unsigned nof_noise_eigen = find_noise_eigenvalues(eigen_values);

  // Early exit if no signal eigenvalue.
  if (nof_noise_eigen == nof_antennas) {
    return std::nullopt;
  }

  // Eigen returns the eigenvectors in increasing order of the eigenvalues: the noise eigenvectors are thus the first
  // nof_noise_eigen ones.
  Ref<const MatrixXcf> noise_vectors = eigen_solver.eigenvectors().leftCols(nof_noise_eigen);

  float distance_over_lambda = config.antenna_distance_over_wavelength;

  // Brute-force search of impinging plane waves.
  for (unsigned i_angle = 0; i_angle != nof_angles; ++i_angle) {
    if (!config.cross_polarized) {
      spectrum[i_angle] = compute_simple(nof_noise_eigen, angle_list[i_angle], distance_over_lambda);
    } else {
      spectrum[i_angle] = compute_polarized(nof_noise_eigen, angle_list[i_angle], distance_over_lambda);
    }
  }

  std::array<unsigned, nof_angles> tmp;
  unsigned                         nof_peaks = find_peaks(tmp, spectrum, nof_antennas - nof_noise_eigen);

  doa_estimator_result results(nof_peaks);
  for (unsigned i_peak = 0; i_peak != nof_peaks; ++i_peak) {
    results.doa_components[i_peak] = {
        .broadside_angle_degrees = angle_list[tmp[i_peak]] * 180.0F / static_cast<float>(M_PI),
        .spectrum_strength       = spectrum[tmp[i_peak]],
    };
  }

  span<const cf_t> result(sample_covariance.data(), sample_covariance.size());

  return results;
}

float doa_estimator_eigen::compute_simple(unsigned nof_noise_eigen, float angle, float distance_over_lambda)
{
  using namespace Eigen;

  signature = (signature_base * distance_over_lambda * std::sin(angle)).array().exp();

  Ref<const MatrixXcf> noise_vectors    = eigen_solver.eigenvectors().leftCols(nof_noise_eigen);
  Ref<VectorXcf>       noise_projection = noise_projection_buffer.head(nof_noise_eigen);

  // Compute projection of the signature onto the noise space. Unfortunately, the Eigen expression
  //     noise_vectors.adjoint() * signature;
  // seems to require MALLOC (likely because noise_vectors is a block of a bigger matrix), so we do it one column at a
  // time.
  for (unsigned i_col = 0; i_col != nof_noise_eigen; ++i_col) {
    noise_projection(i_col) = noise_vectors.col(i_col).adjoint() * signature;
  }

  return 1.0F / noise_projection.squaredNorm();
}

float doa_estimator_eigen::compute_polarized(unsigned nof_noise_eigen, float angle, float distance_over_lambda)
{
  using namespace Eigen;

  Ref<const MatrixXcf> noise_vectors = eigen_solver.eigenvectors().leftCols(nof_noise_eigen);

  Ref<VectorXcf> component_h = noise_projection_buffer.head(nof_noise_eigen);
  Ref<VectorXcf> component_v = noise_projection_buffer2.head(nof_noise_eigen);

  signature = (signature_base * distance_over_lambda * std::sin(angle)).array().exp();

  // Compute projection of the signature onto the noise space for the two polarization components. Unfortunately, the
  // Eigen expression
  //     noise_vectors.adjoint() * signature;
  // seems to require MALLOC (likely because noise_vectors is a block of a bigger matrix), so we do it one column at a
  // time.
  for (unsigned i_col = 0; i_col != nof_noise_eigen; ++i_col) {
    component_h(i_col) = noise_vectors.col(i_col)(seq(0, indexing::last, 2)).adjoint() * signature;
    component_v(i_col) = noise_vectors.col(i_col)(seq(1, indexing::last, 2)).adjoint() * signature;
  }

  float component_h_norm2 = component_h.squaredNorm();
  float component_v_norm2 = component_v.squaredNorm();
  cf_t  component_cross   = component_h.adjoint() * component_v;

  // Compute the minimum eigenvalue of the matrix
  //   [component_h_norm2        component_cross
  //    component_cross*        component_v_norm2].
  float diff_norm  = component_h_norm2 - component_v_norm2;
  float lambda_min = component_h_norm2 + component_v_norm2;
  lambda_min -= std::sqrt(diff_norm * diff_norm + 4 * std::norm(component_cross));

  return 2.0F / lambda_min;
}

unsigned find_noise_eigenvalues(span<const float> eig_values)
{
  unsigned n_values = eig_values.size();

  float running_sum = eig_values[0];

  // Eigenvalues are given in increasing order. We set the boundary between noise and signal eigenvalues where the
  // eigenvalue is larger than 1.5 times the average values of the previous ones.
  unsigned i_value = 1;
  while ((i_value != n_values) && (running_sum * 1.5F >= eig_values[i_value] * i_value)) {
    running_sum += eig_values[i_value++];
  }

  return i_value;
}

unsigned find_peaks(span<unsigned> peak_ix, span<const float> spectrum, unsigned max_peaks)
{
  unsigned spectrum_size = spectrum.size();
  ocudu_assert(max_peaks <= spectrum_size,
               "The maximum number of peaks {} must be lower than the spectrum size {}.",
               max_peaks,
               spectrum.size());
  ocudu_assert(peak_ix.size() == spectrum_size, "peak_ix and spectrum must have the same size.");

  unsigned i_peak = 0;

  // A point is a relative maximum if it's larger than its two neighbor points.
  // We neglect the endpoints, since they only have one neighbor to compare to.
  for (unsigned i_spectrum = 1, end_spectrum = spectrum_size - 1; i_spectrum != end_spectrum; ++i_spectrum) {
    if ((spectrum[i_spectrum] > spectrum[i_spectrum - 1]) && (spectrum[i_spectrum] > spectrum[i_spectrum + 1])) {
      peak_ix[i_peak++] = i_spectrum;
    }
  }

  // Sort the peaks.
  std::stable_sort(peak_ix.begin(), peak_ix.begin() + i_peak, [spectrum](unsigned n, unsigned m) {
    return spectrum[n] > spectrum[m];
  });

  return std::min(i_peak, max_peaks);
}
