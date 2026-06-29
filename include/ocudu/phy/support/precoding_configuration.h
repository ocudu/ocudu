// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/interval.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/adt/tensor.h"
#include "ocudu/ocuduvec/copy.h"
#include "ocudu/ran/precoding/precoding_constants.h"
#include "ocudu/ran/precoding/precoding_weight_matrix.h"
#include "ocudu/ran/resource_block.h"

namespace ocudu {

/// \brief Precoder configuration.
///
/// Describes the precoding pattern to apply to a certain physical channel. The grid is divided in Physical Resource
/// Block Groups (PRGs) of size \c prg_size. The lowest PRB in the lowest PRG coincides with CRB0 (PointA).
///
/// Each PRG contains a matrix of weights per layer and port. The precoding matrix dimensions (i.e., number of layers
/// and ports) is set at construction time.
class precoding_configuration
{
public:
  /// Default constructor - constructs a precoder configuration with no coefficients.
  precoding_configuration() = default;

  /// \brief Constructs a precoding configuration with the desired number of layers, ports and PRGs.
  /// \param[in] nof_layers_ Number of layers.
  /// \param[in] nof_ports_  Number of ports.
  /// \param[in] nof_prg_    Number of PRGs.
  /// \param[in] prg_size_   Number of PRB that comprise a PRG.
  /// \remark An assertion is triggered if the number of layers exceeds \ref precoding_constants::MAX_NOF_LAYERS.
  /// \remark An assertion is triggered if the number of ports exceeds \ref precoding_constants::MAX_NOF_PORTS.
  /// \remark An assertion is triggered if the number of PRGs exceeds \ref precoding_constants::MAX_NOF_PRG.
  /// \remark An assertion is triggered if the PRG size is less than \ref precoding_constants::MIN_PRG_SIZE.
  precoding_configuration(unsigned nof_layers_, unsigned nof_ports_, unsigned nof_prg_, unsigned prg_size_) :
    prg_size(prg_size_), nof_prg(nof_prg_), nof_layers(nof_layers_), nof_ports(nof_ports_)
  {
    /// Range of valid number of layers.
    static constexpr interval<unsigned, true> nof_layers_range{1, precoding_constants::MAX_NOF_LAYERS};
    /// Range of valid number of ports.
    static constexpr interval<unsigned, true> nof_ports_range{1, precoding_constants::MAX_NOF_PORTS};
    /// Range of valid number of PRG.
    static constexpr interval<unsigned, true> nof_prg_range{1, precoding_constants::MAX_NOF_PRG};
    /// Range of valid number of PRG sizes.
    static constexpr interval<unsigned, true> prg_size_range{precoding_constants::MIN_PRG_SIZE, MAX_NOF_PRBS};

    ocudu_assert(nof_prg_range.contains(nof_prg_),
                 "The number of PRG (i.e., {}) is out of the range {}.",
                 nof_prg_,
                 nof_prg_range);

    ocudu_assert(prg_size_range.contains(prg_size_),
                 "The PRG size (i.e., {}) is out of the range {}.",
                 prg_size_,
                 prg_size_range);
    ocudu_assert(nof_layers_range.contains(nof_layers),
                 "The number of layers (i.e., {}) is outside the range {}.",
                 nof_layers,
                 nof_layers_range);
    ocudu_assert(nof_ports_range.contains(nof_ports),
                 "The number of ports (i.e., {}) is outside the range {}.",
                 nof_ports,
                 nof_ports_range);

    // Construct precoding matrices.
    for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
      weights_list.push_back(precoding_weight_matrix(nof_layers, nof_ports));
    }
  }

  /// \brief Copy constructor.
  /// \param[in] other Precoding configuration to copy.
  precoding_configuration(const precoding_configuration& other) :
    prg_size(other.get_prg_size()),
    nof_prg(other.get_nof_prg()),
    nof_layers(other.get_nof_layers()),
    nof_ports(other.get_nof_ports())
  {
    // Copy each precoding matrix.
    for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
      weights_list.push_back(other.get_prg_coefficients(i_prg));
    }
  }

  /// \brief Overload assignment operator.
  /// \param[in] other Precoding configuration to copy.
  precoding_configuration& operator=(const precoding_configuration& other)
  {
    if (this == &other) {
      return *this;
    }

    prg_size   = other.get_prg_size();
    nof_prg    = other.get_nof_prg();
    nof_layers = other.get_nof_layers();
    nof_ports  = other.get_nof_ports();

    // Copy each precoding matrix.
    weights_list.resize(nof_prg);
    for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
      weights_list[i_prg] = other.get_prg_coefficients(i_prg);
    }

    return *this;
  }

  /// Creates a precoding configuration with a single PRG spanning the entire signal bandwidth.
  static precoding_configuration make_wideband(const precoding_weight_matrix& precoding_matrix)
  {
    // Create a precoding configuration with a single PRG spanning all the available BW.
    precoding_configuration config(
        precoding_matrix.get_nof_layers(), precoding_matrix.get_nof_ports(), 1, MAX_NOF_PRBS);

    // Set the PRG weights.
    config.set_prg_coefficients(precoding_matrix, 0);

    return config;
  }

  /// \brief Overload equality comparison operator.
  /// \param[in] other Precoding configuration to compare against.
  /// \return \c true if both precoding configurations are exactly the same, \c false otherwise.
  bool operator==(const precoding_configuration& other) const
  {
    if (get_nof_prg() != other.get_nof_prg()) {
      return false;
    }
    if (get_nof_layers() != other.get_nof_layers()) {
      return false;
    }
    if (get_nof_ports() != other.get_nof_ports()) {
      return false;
    }
    if (get_prg_size() != other.get_prg_size()) {
      return false;
    }

    for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
      if (get_prg_coefficients(i_prg) != other.get_prg_coefficients(i_prg)) {
        return false;
      }
    }
    return true;
  }

  /// Overload inequality comparison operator.
  bool operator!=(const precoding_configuration& other) const { return !(*this == other); }

  /// Gets the current number of layers.
  unsigned get_nof_layers() const { return nof_layers; }

  /// Gets the current number of ports.
  unsigned get_nof_ports() const { return nof_ports; }

  /// Gets the current number of PRG.
  unsigned get_nof_prg() const { return nof_prg; }

  /// Gets the current PRG size.
  unsigned get_prg_size() const { return prg_size; }

  /// \brief Sets a specific coefficient for a given layer, port and PRG.
  /// \param[in] coefficient Coefficient value.
  /// \param[in] i_layer     Layer identifier.
  /// \param[in] i_port      Port identifier.
  /// \param[in] i_prg       PRG identifier.
  /// \remark An assertion is triggered if the layer identifier is greater than or equal to get_nof_layers().
  /// \remark An assertion is triggered if the port identifier is greater than or equal to get_nof_ports().
  /// \remark An assertion is triggered if the PRG identifier is greater than or equal to get_nof_prg().
  void set_coefficient(cf_t coefficient, unsigned i_layer, unsigned i_port, unsigned i_prg)
  {
    ocudu_assert(i_layer < get_nof_layers(),
                 "The layer identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_layer,
                 get_nof_layers() - 1);
    ocudu_assert(i_port < get_nof_ports(),
                 "The ports identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_port,
                 get_nof_ports() - 1);
    ocudu_assert(i_prg < get_nof_prg(),
                 "The PRG identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_prg,
                 get_nof_prg() - 1);
    weights_list[i_prg].set_coefficient(coefficient, i_layer, i_port);
  }

  /// \brief Gets a specific coefficient for a given layer, port and PRG.
  /// \param[in] i_layer     Layer identifier.
  /// \param[in] i_port      Port identifier.
  /// \param[in] i_prg       PRG identifier.
  /// \remark An assertion is triggered if the layer identifier is greater than or equal to get_nof_layers().
  /// \remark An assertion is triggered if the port identifier is greater than or equal to get_nof_ports().
  /// \remark An assertion is triggered if the PRG identifier is greater than or equal to get_nof_prg().
  /// \return The specified coefficient.
  cf_t get_coefficient(unsigned i_layer, unsigned i_port, unsigned i_prg) const
  {
    ocudu_assert(i_layer < get_nof_layers(),
                 "The layer identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_layer,
                 get_nof_layers() - 1);
    ocudu_assert(i_port < get_nof_ports(),
                 "The ports identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_port,
                 get_nof_ports() - 1);
    ocudu_assert(i_prg < get_nof_prg(),
                 "The PRG identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_prg,
                 get_nof_prg() - 1);
    return weights_list[i_prg].get_coefficient(i_layer, i_port);
  }

  /// \brief Gets the precoding weights for a given PRG.
  ///
  /// \param[in] i_prg PRG identifier.
  /// \return The corresponding precoding weight matrix.
  /// \remark An assertion is triggered if the PRG identifier is greater than or equal to get_nof_prg().
  const precoding_weight_matrix& get_prg_coefficients(unsigned i_prg) const
  {
    ocudu_assert((i_prg < get_nof_prg()),
                 "The requested PRG, i.e., {}, must be smaller than the number of PRG, i.e., {}.",
                 i_prg,
                 get_nof_prg());

    return weights_list[i_prg];
  }

  /// \brief Sets the precoding weight matrix for a specified PRG.
  ///
  /// \param[in] prg_coefficients precoding coefficients in matrix form for the given PRG.
  /// \param[in] i_prg PRG identifier.
  /// \remark An assertion is triggered if the PRG identifier is greater than or equal to get_nof_prg().
  /// \remark An assertion is triggered if the dimensions of \c prg_coefficients and the precoding configuration do not
  /// match.
  void set_prg_coefficients(const precoding_weight_matrix& prg_coefficients, unsigned i_prg)
  {
    ocudu_assert((i_prg < get_nof_prg()),
                 "The PRG identifier, i.e., {}, must be smaller than the number of PRG, i.e., {}.",
                 i_prg,
                 get_nof_prg());

    ocudu_assert(prg_coefficients.get_nof_layers() == get_nof_layers(),
                 "The precoding matrix number of layers (i.e., {}) does not match the precoding configuration number "
                 "of layers (i.e., {}).",
                 prg_coefficients.get_nof_layers(),
                 get_nof_layers());
    ocudu_assert(prg_coefficients.get_nof_ports() == get_nof_ports(),
                 "The precoding matrix number of ports (i.e., {}) does not match the precoding configuration number "
                 "of ports (i.e., {}).",
                 prg_coefficients.get_nof_ports(),
                 get_nof_ports());

    weights_list[i_prg] = prg_coefficients;
  }

  void resize(unsigned nof_prg_, unsigned prg_size_)
  {
    /// Range of valid number of PRG.
    static constexpr interval<unsigned, true> nof_prg_range{1, precoding_constants::MAX_NOF_PRG};
    /// Range of valid number of PRG sizes.
    static constexpr interval<unsigned, true> prg_size_range{precoding_constants::MIN_PRG_SIZE, MAX_NOF_PRBS};

    ocudu_assert(nof_prg_range.contains(nof_prg_),
                 "The number of PRG (i.e., {}) is out of the range {}.",
                 nof_prg_,
                 nof_prg_range);

    ocudu_assert(prg_size_range.contains(prg_size_),
                 "The PRG size (i.e., {}) is out of the range {}.",
                 prg_size_,
                 prg_size_range);

    if (nof_prg_ <= get_nof_prg()) {
      // Shrink the list of precoding weights.
      weights_list.resize(nof_prg);
    } else {
      // Grow the list of matrices using the appropriate matrix dimensions. Append new matrices at the end of the list.
      for (unsigned i_prg = get_nof_prg(); i_prg != nof_prg_; ++i_prg) {
        weights_list.push_back(precoding_weight_matrix(get_nof_layers(), get_nof_ports()));
      }
    }

    // Update the dimensions.
    prg_size = prg_size_;
    nof_prg  = nof_prg_;
  }

  /// Scales all the weights by a scaling factor.
  precoding_configuration& operator*=(float scale)
  {
    for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
      weights_list[i_prg] *= scale;
    }
    return *this;
  }

  /// \brief Returns the slice of the precoding configuration defined by the specified layer indices and port indices.
  ///
  /// \param[in] layers Interval indicating the layers to include in the slice, it must be contiguous from start to
  /// stop.
  /// \param[in] ports  Interval indicating the ports to include in the slice, it must be contiguous from start to
  /// stop.
  /// \return The slice of the precoding configuration.
  /// \remarks An assertion is triggered if any input interval, layers or ports, is invalid, i.e., it is out of bounds
  /// for the current precoding configuration.
  precoding_configuration slice(interval<uint8_t> layers, interval<uint8_t> ports) const
  {
    ocudu_assert(layers.start() < nof_layers,
                 "The layer start index (i.e., {}) must be smaller than the number of layers (i.e., {}).",
                 layers.start(),
                 nof_layers);
    ocudu_assert(layers.stop() <= nof_layers,
                 "The layer stop index (i.e., {}) must not be larger than the number of layers (i.e., {}).",
                 layers.stop(),
                 nof_layers);
    ocudu_assert(ports.start() < nof_ports,
                 "The port start index (i.e., {}) must be smaller than the number of ports (i.e., {}).",
                 ports.start(),
                 nof_ports);
    ocudu_assert(ports.stop() <= nof_ports,
                 "The port stop index (i.e., {}) must not be larger than the number of ports (i.e., {}).",
                 ports.stop(),
                 nof_ports);

    // Slice of the precoding configuration.
    precoding_configuration slice_precoding(layers.length(), ports.length(), nof_prg, prg_size);

    // Iterate each PRG.
    for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
      // Original PRG.
      precoding_weight_matrix prg = get_prg_coefficients(i_prg);
      // Slice PRG.
      precoding_weight_matrix slice_prg = slice_precoding.get_prg_coefficients(i_prg);

      // Iterate all ports specified in the slice, extract the layer coefficients and copy them to the slice
      // precoding.
      unsigned first_port = ports.start();
      for (unsigned i_port = first_port, i_port_end = ports.stop(); i_port != i_port_end; ++i_port) {
        ocuduvec::copy(slice_prg.get_port_coefficients(i_port - first_port),
                       prg.get_port_coefficients(i_port).subspan(layers.start(), layers.length()));
      }

      slice_precoding.set_prg_coefficients(slice_prg, i_prg);
    }

    return slice_precoding;
  }

private:
  /// Number of physical resource blocks per PRG.
  unsigned prg_size = MAX_NOF_PRBS;
  /// Number of PRGs.
  unsigned nof_prg = 0;
  /// Number of layers.
  unsigned nof_layers = 0;
  /// Number of antenna ports.
  unsigned nof_ports = 0;

  /// Internal data storage.
  static_vector<precoding_weight_matrix, precoding_constants::MAX_NOF_PRG> weights_list;
};

} // namespace ocudu
