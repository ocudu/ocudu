// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Gold sequence generation and scrambling on the GPU, as per TS 38.211, Section 5.2.1.

#pragma once

#include "ocudu/cuda/adt/cuda_error.h"
#include "ocudu/cuda/adt/cuda_stream.h"
#include <cstdint>
#include <cuda_fp16.h>
#include <cuda_runtime.h>
#include <memory>

namespace ocudu {
namespace cuda {

/// \brief Generates the pseudo-random sequence of TS 38.211, Section 5.2.1, and applies it on the
/// device.
///
/// A scrambler holds the sequence it last generated, so a caller that asks for the same
/// initialiser and offset again does not pay for a second generation. The sequence itself lives in
/// a cache shared by every scrambler in the process, which a base station asking for the same slot
/// parameters on every sector reuses.
///
/// The entry points taking device pointers queue their work on a stream and do not synchronise, so
/// a caller reads the output only after the stream has run.
class scrambler
{
public:
  /// \brief Creates a scrambler.
  ///
  /// The first call uploads the jump matrices that advancing the sequence reads from constant
  /// memory, so it costs more than the calls that follow.
  ///
  /// \return A scrambler, or a description of the error.
  static cuda_expected<scrambler> create();

  /// Creates an empty scrambler that holds no state.
  scrambler();

  ~scrambler();

  scrambler(const scrambler&)            = delete;
  scrambler& operator=(const scrambler&) = delete;

  scrambler(scrambler&& other) noexcept;
  scrambler& operator=(scrambler&& other) noexcept;

  /// Returns true when the scrambler holds state, false when it was default constructed or moved
  /// from.
  bool is_valid() const;

  /// \brief Initialises the sequence and rewinds the offset to 0.
  ///
  /// The value is the \f$c_{init}\f$ of TS 38.211, Section 5.2.1. The channel processor computes
  /// it, because the formula differs between the channels.
  void init(uint32_t c_init);

  /// Returns the value the sequence is initialised from.
  uint32_t get_init() const;

  /// Advances the offset the next sequence starts at by \c count bits.
  void advance(unsigned count);

  /// Sets the offset the next sequence starts at, counted from the start of the sequence.
  void set_offset(unsigned offset);

  /// Returns the offset the next sequence starts at.
  unsigned get_offset() const;

  /// \brief Generates the sequence at the current offset.
  ///
  /// Generating before a graph capture keeps the generation out of the capture.
  ///
  /// \param[in] nof_bits Number of bits of sequence to generate.
  /// \param[in] stream   Stream the work is queued on.
  /// \return A successful result, or a description of the error.
  cuda_result generate(unsigned nof_bits, const cuda_stream& stream);

  /// \brief Reserves room in the cache for a sequence of up to \c max_bits.
  ///
  /// Reserving before a graph capture keeps the allocation out of the capture.
  ///
  /// \param[in] max_bits Largest sequence the caller will ask for.
  /// \return A successful result, or a description of the error.
  cuda_result reserve(unsigned max_bits);

  /// \brief Returns a device pointer to the packed sequence.
  ///
  /// The pointer is borrowed and stays valid while this scrambler holds the sequence.
  ///
  /// \return The sequence, or nullptr when none is generated.
  const uint32_t* sequence() const;

  /// \brief Applies the sequence to packed bits.
  ///
  /// Generates the sequence first if the one in hand is too short or starts elsewhere.
  ///
  /// \param[out] d_output Scrambled bits, in device memory. Must not overlap \c d_input.
  /// \param[in]  d_input  Bits to scramble, in device memory.
  /// \param[in]  nof_bits Number of bits to scramble.
  /// \param[in]  stream   Stream the work is queued on.
  /// \return A successful result, or a description of the error.
  cuda_result apply_xor(uint32_t* d_output, const uint32_t* d_input, unsigned nof_bits, const cuda_stream& stream);

  /// \brief Applies the sequence to packed bits, in place.
  ///
  /// \param[in,out] d_bits   Bits to scramble, in device memory.
  /// \param[in]     nof_bits Number of bits to scramble.
  /// \param[in]     stream   Stream the work is queued on.
  /// \return A successful result, or a description of the error.
  cuda_result apply_xor(uint32_t* d_bits, unsigned nof_bits, const cuda_stream& stream);

  /// \brief Applies the sequence to log likelihood ratios, flipping the sign where the sequence
  /// carries a 1.
  ///
  /// \param[out] d_output Descrambled ratios, in device memory. Must not overlap \c d_input.
  /// \param[in]  d_input  Ratios to descramble, in device memory.
  /// \param[in]  nof_bits Number of ratios to descramble.
  /// \param[in]  stream   Stream the work is queued on.
  /// \return A successful result, or a description of the error.
  cuda_result apply_xor(float* d_output, const float* d_input, unsigned nof_bits, const cuda_stream& stream);

  /// \brief Applies the sequence to log likelihood ratios, in place.
  ///
  /// \param[in,out] d_llrs   Ratios to descramble, in device memory.
  /// \param[in]     nof_bits Number of ratios to descramble.
  /// \param[in]     stream   Stream the work is queued on.
  /// \return A successful result, or a description of the error.
  cuda_result apply_xor(float* d_llrs, unsigned nof_bits, const cuda_stream& stream);

  /// \brief Applies the sequence to half precision log likelihood ratios, in place.
  ///
  /// The sign is flipped in the half precision representation itself, which costs less than a
  /// negation.
  ///
  /// \param[in,out] d_llrs   Ratios to descramble, in device memory.
  /// \param[in]     nof_bits Number of ratios to descramble.
  /// \param[in]     stream   Stream the work is queued on.
  /// \return A successful result, or a description of the error.
  cuda_result apply_xor(__half* d_llrs, unsigned nof_bits, const cuda_stream& stream);

private:
  struct impl;

  explicit scrambler(std::unique_ptr<impl> state_);

  /// Generates the sequence when the one in hand is too short or starts elsewhere.
  cuda_result ensure_sequence(unsigned nof_bits, const cuda_stream& stream);

  std::unique_ptr<impl> state;
};

} // namespace cuda
} // namespace ocudu
