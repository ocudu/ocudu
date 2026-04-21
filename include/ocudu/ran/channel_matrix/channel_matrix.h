// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/adt/tensor.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/ocuduvec/sc_prod.h"
#include "ocudu/ocuduvec/subtract.h"
#include "ocudu/ocuduvec/zero.h"
#include "ocudu/support/ocudu_assert.h"
#include <cmath>
#include <complex>
#include <initializer_list>
#include <numeric>

namespace ocudu {

/// \brief Channel coefficient matrix.
///
/// The complex channel coefficients are arranged by i) receive ports and ii) transmit ports. This class is intended for
/// channel representations with a single coefficient per Tx–Rx path, without subcarrier granularity.
template <unsigned MaxNofTxPorts = 8, unsigned MaxNofRxPorts = 8>
class channel_matrix
{
public:
  /// Channel matrix dimensions.
  enum class dims : uint8_t { rx_port = 0, tx_port, all };

  /// Default constructor - constructs a channel matrix with no coefficients.
  channel_matrix() = default;

  /// \brief Constructs a channel matrix with the desired number of transmit and receive ports.
  ///
  /// The channel coefficients are initialized to zero.
  ///
  /// \param[in] nof_rx_ports Number of receive ports.
  /// \param[in] nof_tx_ports Number of transmit ports.
  /// \remark An assertion is triggered if the number of receive ports exceeds the maximum.
  /// \remark An assertion is triggered if the number of transmit ports exceeds the maximum.
  channel_matrix(unsigned nof_rx_ports, unsigned nof_tx_ports) : data({nof_rx_ports, nof_tx_ports})
  {
    ocudu_assert(nof_rx_ports <= MaxNofRxPorts,
                 "The number of receive ports (i.e., {}) exceeds the maximum (i.e., {}).",
                 nof_rx_ports,
                 MaxNofRxPorts);
    ocudu_assert(nof_tx_ports <= MaxNofTxPorts,
                 "The number of transmit ports (i.e., {}) exceeds the maximum (i.e., {}).",
                 nof_tx_ports,
                 MaxNofTxPorts);
    // Initialize coefficients to zero.
    ocuduvec::zero(data.get_data());
  }

  /// \brief Constructs a channel matrix with the desired number of receive and transmit ports.
  ///
  /// Creates a channel matrix with the specified dimensions, and sets its contents to the provided coefficients.
  ///
  /// \param[in] coefficients Channel coefficient list, arranged by i) receive port and ii) transmit port.
  /// \param[in] nof_rx_ports Number of receive ports.
  /// \param[in] nof_tx_ports Number of transmit ports.
  /// \remark An assertion is triggered if the number of receive ports exceeds the maximum.
  /// \remark An assertion is triggered if the number of transmit ports exceeds the maximum.
  channel_matrix(const std::initializer_list<cf_t>& coefficients, unsigned nof_rx_ports, unsigned nof_tx_ports) :
    channel_matrix(span<const cf_t>(coefficients.begin(), coefficients.end()), nof_rx_ports, nof_tx_ports)
  {
  }

  /// \brief Constructs a channel matrix with the desired number of receive and transmit ports.
  ///
  /// Creates a channel matrix with the specified dimensions, and sets its contents to the provided coefficients
  /// values.
  ///
  /// \param[in] coefficients Channel coefficient list, arranged by i) receive port and ii) transmit port.
  /// \param[in] nof_rx_ports Number of receive ports.
  /// \param[in] nof_tx_ports Number of transmit ports.
  /// \remark An assertion is triggered if the number of receive ports exceeds the maximum.
  /// \remark An assertion is triggered if the number of transmit ports exceeds the maximum.
  channel_matrix(span<const cf_t> coefficients, unsigned nof_rx_ports, unsigned nof_tx_ports) :
    data({nof_rx_ports, nof_tx_ports})
  {
    ocudu_assert(coefficients.size() == nof_rx_ports * nof_tx_ports,
                 "The number of coefficients, i.e., {}, does not match the specified matrix dimensions, i.e., {} "
                 "transmit ports, {} receive ports.",
                 coefficients.size(),
                 nof_tx_ports,
                 nof_rx_ports);

    ocudu_assert(nof_rx_ports <= MaxNofRxPorts,
                 "The number of receive ports (i.e., {}) exceeds the maximum (i.e., {}).",
                 nof_rx_ports,
                 MaxNofRxPorts);
    ocudu_assert(nof_tx_ports <= MaxNofTxPorts,
                 "The number of transmit ports (i.e., {}) exceeds the maximum (i.e., {}).",
                 nof_tx_ports,
                 MaxNofTxPorts);

    // Copy the weights into the tensor.
    ocuduvec::copy(data.get_data(), coefficients);
  }

  /// Copy constructor.
  channel_matrix(const channel_matrix& other) : data({other.get_nof_rx_ports(), other.get_nof_tx_ports()})
  {
    // Copy the weights into the tensor.
    ocuduvec::copy(data.get_data(), other.data.get_data());
  }

  /// \brief Overload assignment operator.
  /// \param[in] other Channel matrix to copy.
  channel_matrix& operator=(const channel_matrix& other)
  {
    if (this == &other) {
      return *this;
    }

    // Resize the tensor.
    resize(other.get_nof_rx_ports(), other.get_nof_tx_ports());
    // Copy the weights into the tensor.
    ocuduvec::copy(data.get_data(), other.data.get_data());
    return *this;
  }

  /// \brief Near equal comparison method.
  ///
  /// Checks whether two matrices are similar, that is whether the Frobenius distance of their normalized versions does
  /// not exceed the given tolerance.
  ///
  /// \param[in] other     Channel matrix to compare against.
  /// \param[in] tolerance Maximum distance between normalized matrices, in the range of [0, 2].
  /// \return \c true if the distance between the two channel matrices is lower than \c tolerance, \c false
  ///         otherwise.
  bool is_near(const channel_matrix& other, float tolerance) const
  {
    unsigned nof_rx_ports = get_nof_rx_ports();
    unsigned nof_tx_ports = get_nof_tx_ports();

    if (nof_rx_ports != other.get_nof_rx_ports()) {
      return false;
    }
    if (nof_tx_ports != other.get_nof_tx_ports()) {
      return false;
    }

    // Get both matrices normalized.
    channel_matrix left  = this->normalize();
    channel_matrix right = other.normalize();

    // Calculate the Frobenius distance between the normalized matrices.
    float frob_dist = (left - right).frobenius_norm();

    // Scaling factor used to bound the distance to [0,2].
    float norm_scale = 1.0f / std::sqrt(nof_tx_ports * nof_rx_ports);

    // Finally, ensure the distance is below the given tolerance.
    return (frob_dist * norm_scale) < tolerance;
  }

  /// Gets the current number of receive ports.
  unsigned get_nof_rx_ports() const { return data.get_dimension_size(dims::rx_port); }

  /// Gets the current number of transmit ports.
  unsigned get_nof_tx_ports() const { return data.get_dimension_size(dims::tx_port); }

  /// \brief Gets a channel coefficient from the matrix.
  ///
  /// \param[in] i_rx_port Receive port index.
  /// \param[in] i_tx_port Transmit port index.
  /// \return The channel coefficient for the given transmit and receive ports.
  cf_t get_coefficient(unsigned i_rx_port, unsigned i_tx_port) const
  {
    ocudu_assert(i_rx_port < get_nof_rx_ports(),
                 "The receive port index (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_rx_port,
                 get_nof_rx_ports() - 1);
    ocudu_assert(i_tx_port < get_nof_tx_ports(),
                 "The transmit port index (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_tx_port,
                 get_nof_tx_ports() - 1);
    return data[{i_rx_port, i_tx_port}];
  }

  /// \brief Sets a channel coefficient in the matrix to a specified value.
  ///
  /// \param[in] coefficient Channel coefficient to set.
  /// \param[in] i_rx_port   Receive port index.
  /// \param[in] i_tx_port   Transmit port index.
  void set_coefficient(cf_t coefficient, unsigned i_rx_port, unsigned i_tx_port)
  {
    ocudu_assert(i_rx_port < get_nof_rx_ports(),
                 "The receive port index (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_rx_port,
                 get_nof_rx_ports() - 1);
    ocudu_assert(i_tx_port < get_nof_tx_ports(),
                 "The transmit port index (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_tx_port,
                 get_nof_tx_ports() - 1);
    data[{i_rx_port, i_tx_port}] = coefficient;
  }

  /// Scales all the coefficients by a real scaling factor.
  channel_matrix& operator*=(float scale)
  {
    ocuduvec::sc_prod(data.get_data(), data.get_data(), scale);
    return *this;
  }

  /// Scales all the coefficients by a complex scaling factor.
  channel_matrix& operator*=(cf_t scale)
  {
    ocuduvec::sc_prod(data.get_data(), data.get_data(), scale);
    return *this;
  }

  /// Calculates the Frobenius norm of the channel matrix.
  float frobenius_norm() const
  {
    span<const cf_t> raw_data = data.get_data();

    float square_sum =
        std::accumulate(raw_data.begin(), raw_data.end(), 0.0F, [](float sum, cf_t in) { return sum + std::norm(in); });

    return std::sqrt(square_sum);
  }

  /// \brief Subtracts two channel matrices.
  /// \param[in] other The matrix to subtract.
  /// \return The difference matrix.
  /// \remark An assertion is triggered if the dimensions are not equal.
  channel_matrix operator-(const channel_matrix& other) const
  {
    unsigned nof_tx_ports = get_nof_tx_ports();
    unsigned nof_rx_ports = get_nof_rx_ports();

    ocudu_assert(nof_tx_ports == other.get_nof_tx_ports(), "The number of transmit ports is not equal.");
    ocudu_assert(nof_rx_ports == other.get_nof_rx_ports(), "The number of receive ports is not equal.");

    channel_matrix   result(nof_rx_ports, nof_tx_ports);
    span<cf_t>       result_data = result.data.get_data();
    span<const cf_t> raw_data    = data.get_data();
    span<const cf_t> other_data  = other.data.get_data();

    ocuduvec::subtract(result_data, raw_data, other_data);

    return result;
  }

  /// \brief Normalizes the channel matrix.
  ///
  /// Applies a scaling factor to all the channel matrix coefficients. The scaling magnitude is equal to the square
  /// root of the number of coefficients divided by the Frobenius norm. The scaling argument is the opposite of the
  /// first coefficient argument.
  ///
  /// \return A normalized channel matrix with purely-real first coefficient and Frobenius norm equal to the square root
  /// of the number of elements.
  channel_matrix normalize() const
  {
    unsigned nof_rx_ports = get_nof_rx_ports();
    unsigned nof_tx_ports = get_nof_tx_ports();

    float argument  = -std::arg(get_coefficient(0, 0));
    float amplitude = std::sqrt(nof_rx_ports * nof_tx_ports) / frobenius_norm();
    cf_t  scaling   = std::polar(amplitude, argument);

    channel_matrix result = *this;
    result *= scaling;
    return result;
  }

private:
  /// \brief Resizes the number of coefficients to a desired number of receive and transmit ports.
  /// \param[in] nof_rx_ports Number of receive ports.
  /// \param[in] nof_tx_ports Number of transmit ports.
  /// \remark An assertion is triggered if the number of receive ports exceeds the maximum.
  /// \remark An assertion is triggered if the number of transmit ports exceeds the maximum.
  void resize(unsigned nof_rx_ports, unsigned nof_tx_ports)
  {
    ocudu_assert(nof_rx_ports <= MaxNofRxPorts,
                 "The number of receive ports (i.e., {}) exceeds the maximum (i.e., {}).",
                 nof_rx_ports,
                 MaxNofRxPorts);
    ocudu_assert(nof_tx_ports <= MaxNofTxPorts,
                 "The number of transmit ports (i.e., {}) exceeds the maximum (i.e., {}).",
                 nof_tx_ports,
                 MaxNofTxPorts);

    data.resize({nof_rx_ports, nof_tx_ports});
  }

  /// Internal data storage.
  static_tensor<static_cast<unsigned>(dims::all), cf_t, MaxNofTxPorts * MaxNofRxPorts, dims> data;
};

} // namespace ocudu
