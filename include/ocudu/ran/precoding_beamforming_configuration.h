// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/interval.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/beamforming/beam_identifier.h"
#include "ocudu/ran/beamforming/beam_identifier_helpers.h"
#include "ocudu/ran/precoding/precoding_constants.h"
#include "ocudu/ran/precoding/precoding_weight_matrix.h"
#include "ocudu/ran/precoding_beamforming_composite.h"
#include "ocudu/ran/resource_block.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <cmath>

namespace ocudu {

/// \brief Precoding and beamforming configuration.
///
/// Describes the precoding and beamforming operation to apply to a certain physical channel. The grid is divided in
/// Physical Resource Block Groups (PRGs) of size \c prg_size. The lowest PRB in the lowest PRG coincides with CRB0
/// (PointA).
///
/// Each PRG contains a composite precoding configuration, that is a MIMO precoding matrix and a beam list:
/// - the MIMO precoding matrix determines how the transmission layers are mapped onto virtual ports, and it is applied
///   in the upper physical layer; and
/// - the beam list determines the beam that each of the virtual ports is transmitted on, and it is applied in the lower
///   physical layer.
///
/// The absence of MIMO precoding is described by the identity precoding matrix, and the absence of beamforming is
/// described by the beam list that selects the antenna ports.
class precoding_beamforming_configuration
{
public:
  /// Default constructor - constructs a precoding and beamforming configuration with no PRG.
  precoding_beamforming_configuration() = default;

  /// \brief Constructs a default precoding and beamforming configuration.
  ///
  /// With the default configuration,
  /// - no MIMO precoding is applied, i.e., the MIMO precoding matrix maps the layer \f$i\f$ onto the virtual port
  ///   \f$i\f$ and the transmission power is distributed among the layers; and
  /// - no beamforming is applied, i.e., the virtual port \f$i\f$ selects the antenna port \f$i\f$.
  ///
  /// \param[in] nof_layers_ Number of layers.
  /// \param[in] nof_beams_  Number of beams, that is the number of virtual ports.
  /// \param[in] nof_prg_    Number of PRGs.
  /// \param[in] prg_size_   Number of PRB that comprise a PRG.
  /// \remark An assertion is triggered if the number of layers exceeds \ref precoding_constants::MAX_NOF_LAYERS.
  /// \remark An assertion is triggered if the number of beams exceeds \ref precoding_constants::MAX_NOF_PORTS.
  /// \remark An assertion is triggered if the number of layers exceeds the number of beams.
  /// \remark An assertion is triggered if the number of PRGs exceeds \ref precoding_constants::MAX_NOF_PRG.
  /// \remark An assertion is triggered if the PRG size is less than \ref precoding_constants::MIN_PRG_SIZE.
  precoding_beamforming_configuration(unsigned nof_layers_,
                                      unsigned nof_beams_,
                                      unsigned nof_prg_,
                                      unsigned prg_size_) :
    prg_size(prg_size_), nof_prg(nof_prg_), nof_layers(nof_layers_), nof_beams(nof_beams_)
  {
    /// Range of valid number of layers.
    static constexpr interval<unsigned, true> nof_layers_range{1, precoding_constants::MAX_NOF_LAYERS};
    /// Range of valid number of beams.
    static constexpr interval<unsigned, true> nof_beams_range{1, precoding_constants::MAX_NOF_PORTS};
    /// Range of valid number of PRG.
    static constexpr interval<unsigned, true> nof_prg_range{1, precoding_constants::MAX_NOF_PRG};
    /// Range of valid number of PRG sizes.
    static constexpr interval<unsigned, true> prg_size_range{precoding_constants::MIN_PRG_SIZE, MAX_NOF_PRBS};

    ocudu_assert(nof_prg_range.contains(nof_prg),
                 "The number of PRG (i.e., {}) is out of the range {}.",
                 nof_prg,
                 nof_prg_range);
    ocudu_assert(
        prg_size_range.contains(prg_size), "The PRG size (i.e., {}) is out of the range {}.", prg_size, prg_size_range);
    ocudu_assert(nof_layers_range.contains(nof_layers),
                 "The number of layers (i.e., {}) is outside the range {}.",
                 nof_layers,
                 nof_layers_range);
    ocudu_assert(nof_beams_range.contains(nof_beams),
                 "The number of beams (i.e., {}) is outside the range {}.",
                 nof_beams,
                 nof_beams_range);
    ocudu_assert(nof_layers <= nof_beams,
                 "The number of layers (i.e., {}) exceeds the number of beams (i.e., {}).",
                 nof_layers,
                 nof_beams);

    // Fill the PRG list with the default configuration, that is neither MIMO precoding nor beamforming.
    data.resize(nof_prg, make_default_composite(nof_layers, nof_beams));
  }

  /// \brief Creates a configuration with a single PRG spanning the entire signal bandwidth.
  ///
  /// The configuration applies the MIMO precoding and the beamforming described by the composite.
  ///
  /// \param[in] composite Composite precoding configuration.
  /// \return The precoding and beamforming configuration.
  /// \remark An assertion is triggered if the number of beams does not match the number of MIMO precoding matrix ports.
  static precoding_beamforming_configuration make_wideband(const precoding_beamforming_composite& composite)
  {
    precoding_beamforming_configuration config(
        composite.mimo.get_nof_layers(), composite.mimo.get_nof_ports(), 1, MAX_NOF_PRBS);
    config.set_prg(composite, 0);

    return config;
  }

  /// \brief Creates a configuration with a single PRG spanning the entire signal bandwidth.
  ///
  /// The configuration applies MIMO precoding and beamforming.
  ///
  /// \param[in] mimo  MIMO precoding matrix, which maps the layers onto the beams.
  /// \param[in] beams Beam list, with one beam per virtual port - MIMO precoding matrix port.
  /// \return The precoding and beamforming configuration.
  /// \remark An assertion is triggered if the number of beams does not match the number of MIMO precoding matrix ports.
  static precoding_beamforming_configuration make_wideband(const precoding_weight_matrix& mimo,
                                                           const precoding_beam_list&     beams)
  {
    return make_wideband({mimo, beams});
  }

  /// \brief Creates a configuration with a single PRG spanning the entire signal bandwidth.
  ///
  /// The configuration applies MIMO precoding without beamforming, therefore the MIMO precoding matrix maps the layers
  /// onto the antenna ports.
  ///
  /// \param[in] mimo MIMO precoding matrix, which maps the layers onto the antenna ports.
  /// \return The precoding and beamforming configuration.
  static precoding_beamforming_configuration make_wideband(const precoding_weight_matrix& mimo)
  {
    return make_wideband({mimo, get_default_beam_list(mimo.get_nof_ports())});
  }

  /// \brief Creates a configuration with a single PRG spanning the entire signal bandwidth.
  ///
  /// The configuration applies beamforming without MIMO precoding, therefore each layer is mapped onto one beam.
  ///
  /// \param[in] beams Beam list, with one beam per layer.
  /// \return The precoding and beamforming configuration.
  static precoding_beamforming_configuration make_wideband(const precoding_beam_list& beams)
  {
    unsigned                        nof_beams = beams.size();
    precoding_beamforming_composite composite = make_default_composite(nof_beams, nof_beams);
    composite.beams                           = beams;

    return make_wideband(composite);
  }

  /// \brief Overload equality comparison operator.
  /// \param[in] other Precoding and beamforming configuration to compare against.
  /// \return \c true if both configurations are exactly the same, \c false otherwise.
  bool operator==(const precoding_beamforming_configuration& other) const
  {
    if (get_nof_prg() != other.get_nof_prg()) {
      return false;
    }
    if (get_nof_layers() != other.get_nof_layers()) {
      return false;
    }
    if (get_nof_beams() != other.get_nof_beams()) {
      return false;
    }
    if (get_prg_size() != other.get_prg_size()) {
      return false;
    }

    for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
      const precoding_beamforming_composite& composite       = get_prg(i_prg);
      const precoding_beamforming_composite& other_composite = other.get_prg(i_prg);

      if (composite.mimo != other_composite.mimo) {
        return false;
      }
      if (composite.beams != other_composite.beams) {
        return false;
      }
    }

    return true;
  }

  /// Overload inequality comparison operator.
  bool operator!=(const precoding_beamforming_configuration& other) const { return !(*this == other); }

  /// Gets the current number of layers.
  unsigned get_nof_layers() const { return nof_layers; }

  /// Gets the current number of beams, that is the number of virtual ports.
  unsigned get_nof_beams() const { return nof_beams; }

  /// Gets the current number of PRG.
  unsigned get_nof_prg() const { return nof_prg; }

  /// Gets the current PRG size.
  unsigned get_prg_size() const { return prg_size; }

  /// \brief Gets the composite precoding configuration of a given PRG.
  /// \param[in] i_prg PRG identifier.
  /// \return The corresponding MIMO precoding matrix and beam list.
  /// \remark An assertion is triggered if the PRG identifier is greater than or equal to get_nof_prg().
  const precoding_beamforming_composite& get_prg(unsigned i_prg) const
  {
    ocudu_assert(i_prg < get_nof_prg(),
                 "The PRG identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_prg,
                 get_nof_prg() - 1);
    return data[i_prg];
  }

  /// \brief Sets the composite precoding configuration of a given PRG.
  /// \param[in] composite MIMO precoding matrix and beam list.
  /// \param[in] i_prg     PRG identifier.
  /// \remark An assertion is triggered if the PRG identifier is greater than or equal to get_nof_prg().
  /// \remark An assertion is triggered if the composite dimensions do not match the configuration dimensions.
  /// \remark An assertion is triggered if any of the beam identifiers is not valid.
  void set_prg(const precoding_beamforming_composite& composite, unsigned i_prg)
  {
    ocudu_assert(i_prg < get_nof_prg(),
                 "The PRG identifier (i.e., {}) exceeds the maximum (i.e., {}).",
                 i_prg,
                 get_nof_prg() - 1);
    ocudu_assert(composite.mimo.get_nof_layers() == get_nof_layers(),
                 "The MIMO precoding matrix number of layers (i.e., {}) does not match the configuration number of "
                 "layers (i.e., {}).",
                 composite.mimo.get_nof_layers(),
                 get_nof_layers());
    ocudu_assert(composite.mimo.get_nof_ports() == get_nof_beams(),
                 "The MIMO precoding matrix number of ports (i.e., {}) does not match the configuration number of "
                 "beams (i.e., {}).",
                 composite.mimo.get_nof_ports(),
                 get_nof_beams());
    ocudu_assert(composite.beams.size() == get_nof_beams(),
                 "The number of beams (i.e., {}) does not match the configuration number of beams (i.e., {}).",
                 composite.beams.size(),
                 get_nof_beams());
    ocudu_assert(std::all_of(composite.beams.begin(), composite.beams.end(), is_beam_id_valid),
                 "The beam list contains beams that are not set.");

    data[i_prg] = composite;
  }

  /// \brief Resizes the number of PRG and the PRG size.
  ///
  /// The configuration of the PRG that are appended is the default one, that is neither MIMO precoding nor
  /// beamforming.
  ///
  /// \param[in] nof_prg_  Number of PRGs.
  /// \param[in] prg_size_ Number of PRB that comprise a PRG.
  /// \remark An assertion is triggered if the number of PRGs exceeds \ref precoding_constants::MAX_NOF_PRG.
  /// \remark An assertion is triggered if the PRG size is less than \ref precoding_constants::MIN_PRG_SIZE.
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

    data.resize(nof_prg_, make_default_composite(nof_layers, nof_beams));

    // Update the dimensions.
    prg_size = prg_size_;
    nof_prg  = nof_prg_;
  }

  /// Scales all the MIMO precoding weights by a scaling factor.
  precoding_beamforming_configuration& operator*=(float scale)
  {
    for (unsigned i_prg = 0; i_prg != nof_prg; ++i_prg) {
      data[i_prg].mimo *= scale;
    }

    return *this;
  }

private:
  /// \brief Creates the default composite precoding configuration.
  ///
  /// The MIMO precoding matrix maps the layer \f$i\f$ onto the virtual port \f$i\f$, distributing the transmission
  /// power among the layers, and the beam list selects the first antenna ports. For a square MIMO precoding matrix, it
  /// is equal to the matrix given by \ref make_identity.
  ///
  /// \param[in] nof_layers Number of layers.
  /// \param[in] nof_beams  Number of beams.
  /// \return The default composite precoding configuration.
  static precoding_beamforming_composite make_default_composite(unsigned nof_layers, unsigned nof_beams)
  {
    precoding_weight_matrix mimo(nof_layers, nof_beams);

    // Distribute the transmission power among the layers.
    cf_t weight = 1.0F / std::sqrt(static_cast<float>(nof_layers));
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      mimo.set_coefficient(weight, i_layer, i_layer);
    }

    return {mimo, get_default_beam_list(nof_beams)};
  }

  /// Number of physical resource blocks per PRG.
  unsigned prg_size = MAX_NOF_PRBS;
  /// Number of PRGs.
  unsigned nof_prg = 0;
  /// Number of layers.
  unsigned nof_layers = 0;
  /// Number of beams, that is the number of virtual ports.
  unsigned nof_beams = 0;

  /// Internal data storage.
  static_vector<precoding_beamforming_composite, precoding_constants::MAX_NOF_PRG> data;
};

} // namespace ocudu
