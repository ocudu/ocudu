// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Polar code interface.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/span.h"
#include <cstdint>

namespace ocudu {

/// Labels for the internal bit interleaver.
enum class polar_code_ibil {
  /// Internal bit interleaver disabled.
  not_present,
  /// Internal bit interleaver enabled.
  present
};

/// \brief Polar code interface.
///
/// Defines a polar code based on message length, codeblock length, rate matching and, possibly, internal interleaver.
/// The resulting code is compatible with the specifications in TS38.212 Section 5.3.1.
///
/// Provides access to the defining parameters as well as to derived ones.
class polar_code
{
public:
  /// \brief Maximum rate-matched codeword length.
  static constexpr unsigned EMAX = 8192;

  /// \brief Maximum codeword length.
  static constexpr unsigned NMAX = 1024;

  /// \brief Base-2 logarithm of the maximum codeword length.
  static constexpr unsigned NMAX_LOG = 10U;

  /// \brief Base-2 logarithm of the maximum rate-matched codeword length.
  static constexpr unsigned eMAX = 13;

  virtual ~polar_code() = default;

  /// Returns the base-2 logarithm of the codeword length.
  virtual unsigned get_n() const = 0;

  /// Returns the codeword length.
  virtual unsigned get_N() const = 0;

  /// Returns the message (data bits and CRC) length.
  virtual unsigned get_K() const = 0;

  /// Returns the codeword length after rate matching.
  virtual unsigned get_E() const = 0;
  /// Returns the number of parity check bits.
  virtual unsigned get_nPC() const = 0;

  /// \brief Returns the message set.
  ///
  /// The message set is the set of indices corresponding to the position of message bits (data and CRC) at the encoder
  /// input.
  virtual const bounded_bitset<NMAX>& get_K_set() const = 0;

  /// \brief Returns the parity-check set.
  ///
  /// The parity-check set is the set of indices corresponding to the position of parity-check bits at the encoder
  /// input.
  virtual span<const uint16_t> get_PC_set() const = 0;

  /// \brief Returns the frozen set.
  ///
  /// The frozen set is the set of indices corresponding to the frozen bits at the encoder input.
  virtual const bounded_bitset<NMAX>& get_F_set() const = 0;

  /// Returns the permutation describing the block interleaver.
  virtual span<const uint16_t> get_blk_interleaver() const = 0;

  /// Returns the internal bit interleaver state.
  virtual polar_code_ibil get_ibil() const = 0;

  /// \brief Initializes the different index sets as needed by the subchannel allocation block and/or by the polar
  /// decoder.
  ///
  /// \param[in] K    Number of data + CRC bits.
  /// \param[in] E    Number of bits of the codeword after rate matching.
  /// \param[in] nMax Base-2 logarithm of the maximum codeword length before rate matching. Possible values are 9 (for
  ///                 downlink) and 10 (for uplink).
  /// \param[in] ibil Bit interleaver (set to not_present to disable).
  virtual void set(unsigned K, unsigned E, uint8_t nMax, polar_code_ibil ibil) = 0;
};

} // namespace ocudu
