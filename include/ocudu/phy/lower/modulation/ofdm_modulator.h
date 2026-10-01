// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/ran/cyclic_prefix.h"

namespace ocudu {

class resource_grid_reader;

/// Setup configuration parameters.
struct ofdm_modulator_configuration {
  /// Indicates the numeorology of the OFDM modulator.
  unsigned numerology;
  /// Indicates the resource grid bandwidth in resource blocks.
  unsigned bw_rb;
  /// Indicates the DFT size.
  unsigned dft_size;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Scaling factor at the DFT output.
  float scale;
  /// Carrier center frequency in Hertz.
  double center_freq_Hz;
};

/// \brief Describes an OFDM modulator that modulates at symbol granularity.
///
/// \remark Performs OFDM modulation as per TS38.211 Section 5.3.1 OFDM baseband signal generation for all channels
/// except PRACH.
/// \remark In addition to modulation, it applies phase compensation as per TS38.211 Section 5.4 Modulation and
/// upconversion
class ofdm_symbol_modulator
{
public:
  /// Default destructor.
  virtual ~ofdm_symbol_modulator() = default;

  /// \brief Gets a symbol size including cyclic prefix.
  /// \param[in] symbol_index Indicates the symbol index within the subframe.
  /// \return The number of samples for the given symbol index.
  virtual unsigned get_symbol_size(unsigned symbol_index) const = 0;

  /// \brief Sets the center frequency.
  ///
  /// The implementation of this method must be thread safe and the new center frequency takes effect in the next call
  /// to modulate().
  ///
  /// \param[in] center_frequency_Hz Given center frequency in Hertz.
  virtual void set_center_frequency(double center_frequency_Hz) = 0;

  /// \brief Modulates an OFDM signal with symbol granularity.
  /// \param[out] output      Time domain modulated signal.
  /// \param[in] grid         Input as frequency-domain signal corresponding to one slot.
  /// \param[in] port_weights Beamforming weights.
  /// \param[in] i_symbol_sf  Symbol index within the subframe to modulate.
  /// \note The input size must be consistent with the configured bandwidth.
  /// \note The output size must be equal to the the symbol size.
  /// \note The number of port weights must be equal to the number of ports contained by the resource grid.
  virtual void modulate(span<cf_t>                  output,
                        const resource_grid_reader& grid,
                        span<const cf_t>            port_weights,
                        unsigned                    i_symbol_sf) = 0;
};

/// \brief Describes an OFDM modulator with slot granularity.
///
/// \remark Performs OFDM modulation as per TS38.211 Section 5.3.1 OFDM baseband signal generation for all channels
/// except PRACH.
/// \remark In addition to modulation, it applies phase compensation as per TS38.211 Section 5.4 Modulation and
/// upconversion.
class ofdm_slot_modulator
{
public:
  /// Default destructor.
  virtual ~ofdm_slot_modulator() = default;

  /// \brief Gets a slot size.
  /// \param[in] slot_index Indicates the slot index within the subframe.
  /// \return The number of samples for the given slot index.
  virtual unsigned get_slot_size(unsigned slot_index) const = 0;

  /// \brief Sets the center frequency.
  ///
  /// The implementation of this method must be thread safe and the new center frequency takes effect in the next call
  /// to modulate().
  ///
  /// \param[in] center_frequency_Hz Given center frequency in Hertz.
  virtual void set_center_frequency(double center_frequency_Hz) = 0;

  /// \brief Modulates an OFDM signal with slot granularity.
  ///
  /// The resource grid symbols in each port are combined according to the provided port weights before applying the
  /// OFDM modulation.
  ///
  /// \param[out] output       Time time domain modulated signal destination.
  /// \param[in]  grid          Input as frequency-domain signal corresponding to one slot.
  /// \param[in]  port_weights  Port weights.
  /// \param[in]  i_symbol_sf   Slot index within the subframe to modulate.
  /// \note The input size must be consistent with the configured bandwidth.
  /// \note The output size must be equal to the slot size.
  /// \note The number of port weights must be equal to the number of ports contained by the resource grid.
  virtual void modulate(span<cf_t>                  output,
                        const resource_grid_reader& grid,
                        span<const cf_t>            port_weights,
                        unsigned                    i_symbol_sf) = 0;
};

} // namespace ocudu
