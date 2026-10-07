// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief CRC computation and checking on the GPU, as per TS 38.212, Section 5.1.
///
/// The transport block chain attaches a checksum when encoding and checks one when decoding, so
/// both halves of the chain call into here.

#pragma once

#include "ocudu/cuda/adt/cuda_error.h"
#include "ocudu/cuda/adt/cuda_stream.h"
#include <cstdint>
#include <cuda_runtime.h>

namespace ocudu {
namespace cuda {

/// Polynomial a batch check applies.
enum class crc_type { crc24a, crc24b, crc16 };

/// \brief Computes the CRC-24A checksum of a transport block.
///
/// \param[out] d_crc    Checksum, in device memory.
/// \param[in]  d_data   Data bits in device memory.
/// \param[in]  num_bits Number of data bits.
/// \param[in]  stream   Stream the work is queued on.
/// \return A successful result, or a description of the error.
cuda_result crc24a_compute(uint32_t* d_crc, const uint8_t* d_data, unsigned num_bits, const cuda_stream& stream);

/// \brief Computes the CRC-24A checksum of a transport block with caller-owned scratch.
///
/// Suits byte-aligned blocks large enough to split across chunks.
///
/// \param[out] d_crc             Checksum, in device memory.
/// \param[out] d_chunk_crcs      Scratch for the per-chunk checksums.
/// \param[out] d_chunk_sizes     Scratch for the per-chunk byte counts.
/// \param[in]  d_data            Data bits in device memory.
/// \param[in]  num_bits          Number of data bits.
/// \param[in]  max_chunks        Number of entries the scratch arrays hold.
/// \param[in]  chunk_size_bytes  Number of input bytes per chunk.
/// \param[in]  stream            Stream the work is queued on.
/// \return A successful result, or a description of the error.
cuda_result crc24a_compute_with_scratch(uint32_t*          d_crc,
                                        uint32_t*          d_chunk_crcs,
                                        int*               d_chunk_sizes,
                                        const uint8_t*     d_data,
                                        unsigned           num_bits,
                                        unsigned           max_chunks,
                                        unsigned           chunk_size_bytes,
                                        const cuda_stream& stream);

/// \brief Computes the CRC-24A checksum of every transport block in a batch.
///
/// \param[out] d_crcs       Checksums, in device memory, one per transport block.
/// \param[in]  d_tb_data    Transport blocks in device memory, one after another.
/// \param[in]  num_tbs      Number of transport blocks.
/// \param[in]  tb_size_bits Number of data bits in each transport block.
/// \param[in]  stream       Stream the work is queued on.
/// \return A successful result, or a description of the error.
cuda_result crc24a_compute_batch(uint32_t*          d_crcs,
                                 const uint8_t*     d_tb_data,
                                 unsigned           num_tbs,
                                 unsigned           tb_size_bits,
                                 const cuda_stream& stream);

/// \brief Computes the CRC-24B checksum of a code block.
///
/// \param[out] d_crc    Checksum, in device memory.
/// \param[in]  d_data   Data bits in device memory.
/// \param[in]  num_bits Number of data bits.
/// \param[in]  stream   Stream the work is queued on.
/// \return A successful result, or a description of the error.
cuda_result crc24b_compute(uint32_t* d_crc, const uint8_t* d_data, unsigned num_bits, const cuda_stream& stream);

/// \brief Computes the CRC-16 checksum of a small transport block.
///
/// \param[out] d_crc    Checksum, in device memory.
/// \param[in]  d_data   Data bits in device memory.
/// \param[in]  num_bits Number of data bits.
/// \param[in]  stream   Stream the work is queued on.
/// \return A successful result, or a description of the error.
cuda_result crc16_compute(uint16_t* d_crc, const uint8_t* d_data, unsigned num_bits, const cuda_stream& stream);

/// \brief Computes the CRC-24A checksum of a transport block and appends it.
///
/// \param[in,out] d_tb_with_crc Transport block in device memory, with room for the checksum.
/// \param[in]     tb_size_bits  Number of data bits, excluding the checksum.
/// \param[in]     stream        Stream the work is queued on.
/// \return A successful result, or a description of the error.
cuda_result crc24a_compute_and_attach(uint8_t* d_tb_with_crc, unsigned tb_size_bits, const cuda_stream& stream);

/// \brief Computes the CRC-16 checksum of a transport block and appends it.
///
/// \param[in,out] d_tb_with_crc Transport block in device memory, with room for the checksum.
/// \param[in]     tb_size_bits  Number of data bits, excluding the checksum.
/// \param[in]     stream        Stream the work is queued on.
/// \return A successful result, or a description of the error.
cuda_result crc16_compute_and_attach(uint8_t* d_tb_with_crc, unsigned tb_size_bits, const cuda_stream& stream);

/// \brief Checks the CRC-24A checksum carried by received data.
///
/// \param[in] d_data   Data bits followed by the checksum, in device memory.
/// \param[in] num_bits Number of bits, including the checksum.
/// \param[in] stream   Stream the work is queued on.
/// \return True when the checksum matches, false when it does not, or a description of the error.
cuda_expected<bool> crc24a_check(const uint8_t* d_data, unsigned num_bits, const cuda_stream& stream);

/// \brief Checks the CRC-24B checksum carried by a received code block.
///
/// \param[in] d_data   Data bits followed by the checksum, in device memory.
/// \param[in] num_bits Number of bits, including the checksum.
/// \param[in] stream   Stream the work is queued on.
/// \return True when the checksum matches, false when it does not, or a description of the error.
cuda_expected<bool> crc24b_check(const uint8_t* d_data, unsigned num_bits, const cuda_stream& stream);

/// \brief Checks the CRC-16 checksum carried by a received transport block.
///
/// \param[in] d_data   Data bits followed by the checksum, in device memory.
/// \param[in] num_bits Number of bits, including the checksum.
/// \param[in] stream   Stream the work is queued on.
/// \return True when the checksum matches, false when it does not, or a description of the error.
cuda_expected<bool> crc16_check(const uint8_t* d_data, unsigned num_bits, const cuda_stream& stream);

/// \brief Checks the checksum of every code block in a batch of packed decoder output.
///
/// The decoder packs its output least significant bit first. This converts to the most significant
/// bit first order the checksum is computed over.
///
/// \param[out] d_results     One entry per code block in device memory, non-zero when it matches.
/// \param[in]  d_packed_data Packed decoder output in device memory.
/// \param[in]  words_per_cb  Number of words each code block occupies.
/// \param[in]  d_bits_per_cb Bit count of each code block, including the checksum, in device memory.
/// \param[in]  num_cbs       Number of code blocks.
/// \param[in]  type          Polynomial to apply.
/// \param[in]  stream        Stream the work is queued on.
/// \return A successful result, or a description of the error.
cuda_result crc_check_batch_packed(int*               d_results,
                                   const uint32_t*    d_packed_data,
                                   unsigned           words_per_cb,
                                   const int*         d_bits_per_cb,
                                   unsigned           num_cbs,
                                   crc_type           type,
                                   const cuda_stream& stream);

/// \brief Checks the checksum of every code block in a batch that shares one bit count.
///
/// \param[out] d_results     One entry per code block in device memory, non-zero when it matches.
/// \param[in]  d_packed_data Packed decoder output in device memory.
/// \param[in]  words_per_cb  Number of words each code block occupies.
/// \param[in]  bits_per_cb   Bit count of every code block, including the checksum.
/// \param[in]  num_cbs       Number of code blocks.
/// \param[in]  type          Polynomial to apply.
/// \param[in]  stream        Stream the work is queued on.
/// \return A successful result, or a description of the error.
cuda_result crc_check_batch_packed_uniform(int*               d_results,
                                           const uint32_t*    d_packed_data,
                                           unsigned           words_per_cb,
                                           unsigned           bits_per_cb,
                                           unsigned           num_cbs,
                                           crc_type           type,
                                           const cuda_stream& stream);

/// \brief Fills the lookup tables the checksum kernels read from constant memory.
///
/// Calling this before a graph capture keeps the table upload out of the capture. Calling it more
/// than once has no further effect.
///
/// \return A successful result, or a description of the error.
cuda_result crc_init_tables();

} // namespace cuda
} // namespace ocudu
