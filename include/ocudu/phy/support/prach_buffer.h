// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"

namespace ocudu {

/// \brief PRACH buffer interface.
///
/// Provides access to frequency-domain PRACH sequences.
///
/// A buffer for storing long PRACH sequences (\f$L_{RA}=839\f$), for a maximum of 4 OFDM symbols, can be created with
/// the factory function create_prach_buffer_long(). A buffer for storing short PRACH sequences (\f$L_{RA}=129\f$), for
/// a maximum of 12 OFDM symbols, can be created with the factory function \c create_prach_buffer_short().
class prach_buffer
{
public:
  /// Default destructor.
  virtual ~prach_buffer() = default;

  /// Gets the maximum number of ports.
  virtual unsigned get_max_nof_ports() const = 0;

  /// Gets the maximum number of time domain occasions.
  virtual unsigned get_max_nof_td_occasions() const = 0;

  /// Gets the maximum number of frequency domain occasions.
  virtual unsigned get_max_nof_fd_occasions() const = 0;

  /// Gets the maximum number of symbols.
  virtual unsigned get_max_nof_symbols() const = 0;

  /// Gets the sequence length.
  virtual unsigned get_sequence_length() const = 0;

  /// \brief Gets a read-write PRACH symbol for a given port, occasion and symbol.
  ///
  /// \param[in] i_port        Port identifier.
  /// \param[in] i_td_occasion Time-domain occasion.
  /// \param[in] i_fd_occasion Frequency-domain occasion.
  /// \param[in] i_symbol      Symbol index within the occasion.
  /// \return A read-write view of a PRACH OFDM symbol.
  virtual span<cbf16_t>
  get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) = 0;

  /// \brief Gets a read-only PRACH symbol for a given port, occasion, and symbol.
  ///
  /// \param[in] i_port        Port identifier.
  /// \param[in] i_td_occasion Time-domain occasion.
  /// \param[in] i_fd_occasion Frequency-domain occasion.
  /// \param[in] i_symbol      Symbol index within the occasion.
  /// \return A read-only view of a PRACH OFDM symbol.
  virtual span<const cbf16_t>
  get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) const = 0;

  /// \brief Gets a read-only view of the whole buffer.
  ///
  /// The samples of every port, occasion and symbol, laid out so that \ref get_symbol_offset()
  /// gives the position of each symbol within them.
  ///
  /// \return A read-only span containing the entire data of the buffer.
  virtual span<const cbf16_t> get_buffer() const = 0;

  /// \brief Gets the position of one PRACH symbol within the buffer.
  ///
  /// \param[in] i_port        Port identifier.
  /// \param[in] i_td_occasion Time-domain occasion.
  /// \param[in] i_fd_occasion Frequency-domain occasion.
  /// \param[in] i_symbol      Symbol index within the occasion.
  /// \return The offset of the first sample of that symbol, counted in samples from the start of
  /// the span returned by \ref get_buffer().
  virtual unsigned
  get_symbol_offset(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) const = 0;
};

} // namespace ocudu
