// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Transport block encoding on the GPU, as per TS 38.212, Section 7.2.
///
/// The encoder runs the downlink shared channel chain for one transport block: the transport block
/// checksum, segmentation into code blocks with a checksum each, LDPC encoding, rate matching and
/// scrambling. The code blocks of a transport block are encoded in one launch per stage rather than
/// one launch per code block.

#pragma once

#include "ocudu/cuda/adt/cuda_error.h"
#include "ocudu/cuda/adt/cuda_stream.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc.h"
#include "ocudu/ran/sch/ldpc_base_graph.h"
#include "ocudu/ran/sch/modulation_scheme.h"
#include <cstdint>
#include <memory>
#include <optional>

namespace ocudu {
namespace cuda {

/// \brief Shape of the transport block a configured encoder accepts.
///
/// The segmentation fields repeat what TS 38.212, Section 5.2.2 derives from the transport block
/// size and the target code rate. They are not derived but given so that a caller that has
/// already segmented on the host encodes exactly the same code blocks here. Leaving one at zero
/// asks the encoder to derive it.
struct transport_block_encoder_config {
  /// Transport block size, in bits, before the checksum is attached.
  unsigned nof_tb_bits = 0;
  /// Number of transmission layers the codeword is mapped to.
  unsigned nof_layers = 1;
  /// Modulation the rate matcher packs its output for.
  modulation_scheme modulation = modulation_scheme::QPSK;
  /// Number of coded bits the grant allocates, named G in TS 38.212.
  unsigned nof_allocated_bits = 0;
  /// Redundancy version of this transmission.
  unsigned redundancy_version = 0;
  /// Target code rate the segmentation was derived from.
  float code_rate = 0.0F;

  /// Radio network temporary identifier the scrambling sequence is initialised from.
  uint16_t rnti = 0;
  /// Scrambling identity of the cell.
  uint16_t n_id = 0;
  /// Codeword index, zero or one.
  uint8_t codeword_index = 0;
  /// Scramble the rate matched bits, as the standard requires of a transmitter.
  bool enable_scrambling = true;

  /// Base graph the code blocks are encoded with, or absent to derive it.
  std::optional<ldpc_base_graph_type> base_graph;
  /// Number of code blocks the transport block segments into, or zero to derive it.
  unsigned nof_codeblocks = 0;
  /// Lifting size of the code, or absent to derive it.
  std::optional<ldpc::lifting_size_t> lifting_size;
  /// Number of filler bits the segmentation leaves, or zero to derive it.
  unsigned nof_filler_bits = 0;

  /// Number of code blocks that carry the shorter rate matched length, or zero to derive it.
  unsigned nof_short_codeblocks = 0;
  /// Rate matched length of a short code block, or zero to derive it.
  unsigned nof_short_bits = 0;
  /// Rate matched length of a long code block, or zero to derive it.
  unsigned nof_long_bits = 0;

  /// \brief Length the circular buffer is limited to, or zero for the whole of it.
  ///
  /// Limited-buffer rate matching, as per TS 38.212, Section 5.4.2.1, shortens the buffer the bit
  /// selection wraps around to N_ref. The retransmissions of a large transport block select the
  /// wrong bits without it.
  unsigned nof_buffer_bits = 0;
};

/// \brief Encodes a transport block into the bits the grant carries.
///
/// An instance holds the device memory the chain works in, so it is configured once for a shape and
/// reused across transport blocks of that shape.
class transport_block_encoder
{
public:
  /// \brief Creates an encoder.
  ///
  /// \return An encoder, or a description of the error.
  static cuda_expected<transport_block_encoder> create();

  /// Creates an encoder that holds no state, to be moved into.
  transport_block_encoder();

  ~transport_block_encoder();

  transport_block_encoder(const transport_block_encoder&)            = delete;
  transport_block_encoder& operator=(const transport_block_encoder&) = delete;
  transport_block_encoder(transport_block_encoder&& other) noexcept;
  transport_block_encoder& operator=(transport_block_encoder&& other) noexcept;

  /// Returns true if the encoder holds the state a decode needs.
  bool is_valid() const { return state != nullptr; }

  /// \brief Sets the shape the encoder accepts, reserving the device memory it needs.
  ///
  /// \param[in] config Shape of the transport block.
  /// \return A successful result, or a description of the error.
  cuda_result configure(const transport_block_encoder_config& config);

  /// \brief Encodes one transport block.
  ///
  /// \param[out] d_output   Rate matched bits, one per byte, of \ref get_nof_output_bits() entries.
  /// \param[in]  d_tb_input Transport block, one bit per byte, in device memory.
  /// \param[in]  stream     Stream the work is queued on.
  /// \return A successful result, or a description of the error.
  cuda_result encode(uint8_t* d_output, const uint8_t* d_tb_input, const cuda_stream& stream);

  /// Returns the number of bits an encode writes.
  unsigned get_nof_output_bits() const;

  /// Returns the number of code blocks the configured transport block segments into.
  unsigned get_nof_codeblocks() const;

  /// Returns the base graph the configured code blocks are encoded with.
  ldpc_base_graph_type get_base_graph() const;

  /// Returns the lifting size of the configured code.
  ldpc::lifting_size_t get_lifting_size() const;

private:
  struct impl;

  explicit transport_block_encoder(std::unique_ptr<impl> state_);

  std::unique_ptr<impl> state;
};

} // namespace cuda
} // namespace ocudu
