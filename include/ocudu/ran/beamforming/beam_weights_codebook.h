// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/tensor.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/ran/beamforming/beam_identifier.h"

namespace ocudu {

/// \brief Beam weights codebook.
///
/// This codebook associates the beamforming weights with beams.
///
/// The complex beam coefficients are arranged by i) beams and ii) antennas. This class is intended for
/// beam coefficient representations with a single coefficient per beam and antenna, without subcarrier granularity.
class beam_weights_codebook
{
public:
  /// \brief Default constructor - constructs beam weights codebook with no coefficients.
  ///
  /// This method is protected to avoid generating an invalid codebook by default.
  beam_weights_codebook() = default;

  /// \brief Constructs a beam weights codebook with the desired number of beams and antennas.
  ///
  /// The beam weights are initialized to zero.
  ///
  /// \param[in] nof_beams    Number of beams.
  /// \param[in] nof_antennas Number of antennas.
  beam_weights_codebook(unsigned nof_beams, unsigned nof_antennas) : data({nof_beams, nof_antennas})
  {
    // Initialize coefficients to zero.
    ocuduvec::zero(data.get_data());
  }

  /// Beam weights codebook dimensions.
  enum class dims : uint8_t { beam = 0, antenna, all };

  /// \brief Constructs a beam weight codebook with the desired number of beams and antennas.
  ///
  /// Creates a beam weights codebook with the specified dimensions, and sets its contents to the provided coefficients.
  ///
  /// \param[in] coefficients Beam coefficient list, arranged by i) beams and ii) antenna port.
  /// \param[in] nof_beams    Number of beams.
  /// \param[in] nof_antennas Number of antennas.
  /// \remark An assertion is triggered if the number of receive ports exceeds the maximum.
  /// \remark An assertion is triggered if the number of transmit ports exceeds the maximum.
  beam_weights_codebook(const std::initializer_list<cf_t>& coefficients, unsigned nof_beams, unsigned nof_antennas) :
    beam_weights_codebook(span(coefficients.begin(), coefficients.end()), nof_beams, nof_antennas)
  {
  }

  /// \brief Constructs a beam weights codebook with the desired number of beams and antennas.
  ///
  /// Creates a beam weights codebook with the specified dimensions, and sets its contents to the provided coefficients.
  ///
  /// \param[in] coefficients Beam coefficient list, arranged by i) beams and ii) antenna port.
  /// \param[in] nof_beams    Number of beams.
  /// \param[in] nof_antennas Number of antennas.
  /// \remark An assertion is triggered if the number of receive ports exceeds the maximum.
  /// \remark An assertion is triggered if the number of transmit ports exceeds the maximum.
  beam_weights_codebook(span<const cf_t> coefficients, unsigned nof_beams, unsigned nof_antennas) :
    data({nof_beams, nof_antennas})
  {
    ocudu_assert(coefficients.size() == nof_beams * nof_antennas,
                 "The number of coefficients, i.e., {}, does not match the specified matrix dimensions, i.e., {} "
                 "antennas, {} beams.",
                 coefficients.size(),
                 nof_antennas,
                 nof_beams);

    // Copy the weights into the tensor.
    ocuduvec::copy(data.get_data(), coefficients);
  }

  /// Copy constructor.
  beam_weights_codebook(const beam_weights_codebook& other) : data({other.get_nof_beams(), other.get_nof_antennas()})
  {
    // Copy the weights into the tensor.
    ocuduvec::copy(data.get_data(), other.data.get_data());
  }

  /// \brief Overload assignment operator.
  /// \param[in] other Beam weight codebook to copy.
  beam_weights_codebook& operator=(const beam_weights_codebook& other)
  {
    if (this == &other) {
      return *this;
    }

    // Resize the tensor.
    data.resize({other.get_nof_beams(), other.get_nof_antennas()});
    // Copy the weights into the tensor.
    ocuduvec::copy(data.get_data(), other.data.get_data());
    return *this;
  }

  /// Gets the current number of beams.
  unsigned get_nof_beams() const { return data.get_dimension_size(dims::beam); }

  /// Gets the current number of transmit ports.
  unsigned get_nof_antennas() const { return data.get_dimension_size(dims::antenna); }

  /// \brief Gets a channel coefficient from the matrix.
  ///
  /// \param[in] beam_id     Beam identifier.
  /// \param[in] i_antenna   Antenna port index.
  /// \return The channel coefficient for the given transmit and receive ports.
  cf_t get_coefficient(beam_identifier beam_id, unsigned i_antenna) const
  {
    unsigned i_beam = to_uint(beam_id);
    ocudu_assert(i_beam < get_nof_beams(),
                 "The beam index (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_beam,
                 get_nof_beams() - 1);
    ocudu_assert(i_antenna < get_nof_antennas(),
                 "The antenna index (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_antenna,
                 get_nof_antennas() - 1);
    return data[{i_beam, i_antenna}];
  }

  /// \brief Sets a channel coefficient in the matrix to a specified value.
  ///
  /// \param[in] coefficient Channel coefficient to set.
  /// \param[in] beam_id     Beam identifier.
  /// \param[in] i_antenna   Antenna port index.
  void set_coefficient(cf_t coefficient, beam_identifier beam_id, unsigned i_antenna)
  {
    unsigned i_beam = to_uint(beam_id);
    ocudu_assert(i_beam < get_nof_beams(),
                 "The beam index (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_beam,
                 get_nof_beams() - 1);
    ocudu_assert(i_antenna < get_nof_antennas(),
                 "The antenna index (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_antenna,
                 get_nof_antennas() - 1);
    data[{i_beam, i_antenna}] = coefficient;
  }

  /// \brief  Gets a view of the coefficients associated with a given antenna index.
  /// \return A view of the coefficients for the contribution of each beam to the given antenna.
  /// \remark An assertion is triggered if the antenna index exceeds the maximum.
  span<const cf_t> get_antenna_coefficients(unsigned i_antenna)
  {
    ocudu_assert(i_antenna < get_nof_antennas(),
                 "The antenna index (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_antenna,
                 get_nof_antennas() - 1);
    return data.get_view({i_antenna});
  }

  /// \brief Gets a vector of the coefficients associated with a given beam identifier.
  /// \tparam MaxNofAntennas Maximum number of antennas.
  /// \param[in] beam_id Given beam identifier.
  /// \return A vector of the coefficients for the contribution of each antenna to the given beam.
  /// \remark An assertion is triggered if the maximum number of antennas is less than the number of antennas.
  /// \remark An assertion is triggered if the given beam identifier exceeds the maximum.
  template <unsigned MaxNofAntennas>
  static_vector<cf_t, MaxNofAntennas> get_beam_coefficients(beam_identifier beam_id) const
  {
    unsigned i_beam = to_uint(beam_id);
    ocudu_assert(i_beam < get_nof_beams(),
                 "The beam index (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_beam,
                 get_nof_beams() - 1);

    unsigned nof_antennas = get_nof_antennas();
    ocudu_assert(nof_antennas <= MaxNofAntennas,
                 "The number of antennas (i.e., {}) exceeds the maximum (i.e., {}).",
                 nof_antennas,
                 MaxNofAntennas);

    static_vector<cf_t, MaxNofAntennas> beam_coefficients(nof_antennas);
    for (unsigned i_antenna = 0; i_antenna != nof_antennas; ++i_antenna) {
      beam_coefficients[i_antenna] = data[{i_beam, i_antenna}];
    }

    return beam_coefficients;
  }

private:
  /// Internal data storage.
  dynamic_tensor<static_cast<unsigned>(dims::all), cf_t, dims> data;
};

} // namespace ocudu
