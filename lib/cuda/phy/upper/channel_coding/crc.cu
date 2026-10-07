// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief 5G NR CRC Implementation
///
/// CUDA-accelerated CRC computation for 5G NR transport blocks and code blocks.
/// Implements CRC-24A, CRC-24B, and CRC-16 per 3GPP TS 38.212.

#include "ocudu/cuda/adt/cuda_error.h"
#include "ocudu/cuda/phy/upper/channel_coding/crc.h"

namespace ocudu {
namespace cuda {

// ============================================================================
// CRC Polynomial Definitions (from 3GPP TS 38.212)
// ============================================================================

// CRC-24A: g(D) = D^24 + D^23 + D^18 + D^17 + D^14 + D^11 + D^10 + D^7 + D^6 + D^5 + D^4 + D^3 + D + 1
#define CRC24A_POLY 0x864CFB

// CRC-24B: g(D) = D^24 + D^23 + D^6 + D^5 + D + 1
#define CRC24B_POLY 0x800063

// CRC-16: g(D) = D^16 + D^12 + D^5 + 1
#define CRC16_POLY 0x1021

// ============================================================================
// Device Lookup Tables for Fast CRC Computation
// ============================================================================

__device__ __constant__ uint32_t CRC24A_TABLE[256];
__device__ __constant__ uint32_t CRC24B_TABLE[256];
__device__ __constant__ uint16_t CRC16_TABLE[256];

// Host-side table generation
static uint32_t h_crc24a_table[256];
static uint32_t h_crc24b_table[256];
static uint16_t h_crc16_table[256];
static bool     tables_initialized = false;

// ============================================================================
// Slicing-by-4 Tables for Fast CRC Computation (TX path - MSB-first)
// ============================================================================
// These tables enable processing 4 bytes per iteration instead of 1
__device__ __constant__ uint32_t CRC24A_S0[256]; ///< Byte 0 (MSB) contribution
__device__ __constant__ uint32_t CRC24A_S1[256]; ///< Byte 1 contribution
__device__ __constant__ uint32_t CRC24A_S2[256]; ///< Byte 2 contribution
__device__ __constant__ uint32_t CRC24A_S3[256]; ///< Byte 3 (LSB) contribution
__device__ __constant__ uint32_t CRC24A_S8_0[256];
__device__ __constant__ uint32_t CRC24A_S8_1[256];
__device__ __constant__ uint32_t CRC24A_S8_2[256];
__device__ __constant__ uint32_t CRC24A_S8_3[256];
__device__ __constant__ uint32_t CRC24A_S8_4[256];
__device__ __constant__ uint32_t CRC24A_S8_5[256];
__device__ __constant__ uint32_t CRC24A_S8_6[256];
__device__ __constant__ uint32_t CRC24A_S8_7[256];

__device__ __constant__ uint32_t CRC24B_S0[256];
__device__ __constant__ uint32_t CRC24B_S1[256];
__device__ __constant__ uint32_t CRC24B_S2[256];
__device__ __constant__ uint32_t CRC24B_S3[256];

__device__ __constant__ uint32_t CRC16_S0[256];
__device__ __constant__ uint32_t CRC16_S1[256];
__device__ __constant__ uint32_t CRC16_S2[256];
__device__ __constant__ uint32_t CRC16_S3[256];

// Host-side slicing tables for TX
static uint32_t h_crc24a_s0[256], h_crc24a_s1[256], h_crc24a_s2[256], h_crc24a_s3[256];
static uint32_t h_crc24a_s8_0[256], h_crc24a_s8_1[256], h_crc24a_s8_2[256], h_crc24a_s8_3[256];
static uint32_t h_crc24a_s8_4[256], h_crc24a_s8_5[256], h_crc24a_s8_6[256], h_crc24a_s8_7[256];
static uint32_t h_crc24b_s0[256], h_crc24b_s1[256], h_crc24b_s2[256], h_crc24b_s3[256];
static uint32_t h_crc16_s0[256], h_crc16_s1[256], h_crc16_s2[256], h_crc16_s3[256];
static bool     slicing_tables_initialized = false;

// ============================================================================
// Table Generation Functions
// ============================================================================

static void generate_crc24_table(uint32_t* table, uint32_t poly)
{
  for (int i = 0; i < 256; i++) {
    uint32_t crc = (uint32_t)i << 16;
    for (int j = 0; j < 8; j++) {
      if (crc & 0x800000) {
        crc = (crc << 1) ^ poly;
      } else {
        crc <<= 1;
      }
    }
    table[i] = crc & 0xFFFFFF;
  }
}

static void generate_crc16_table(uint16_t* table, uint16_t poly)
{
  for (int i = 0; i < 256; i++) {
    uint16_t crc = (uint16_t)i << 8;
    for (int j = 0; j < 8; j++) {
      if (crc & 0x8000) {
        crc = (crc << 1) ^ poly;
      } else {
        crc <<= 1;
      }
    }
    table[i] = crc;
  }
}

static cuda_result init_crc_tables()
{
  if (tables_initialized)
    return {};

  generate_crc24_table(h_crc24a_table, CRC24A_POLY);
  generate_crc24_table(h_crc24b_table, CRC24B_POLY);
  generate_crc16_table(h_crc16_table, CRC16_POLY);

  // Copy to device constant memory
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24A_TABLE, h_crc24a_table, sizeof(h_crc24a_table)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24B_TABLE, h_crc24b_table, sizeof(h_crc24b_table)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC16_TABLE, h_crc16_table, sizeof(h_crc16_table)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }

  tables_initialized = true;
  // success
  return {};
}

/// \brief Generate CRC-24 slicing-by-4 tables for MSB-first processing
///
/// For standard CRC: crc = (crc << 8) ^ T[(crc >> 16) ^ byte]
/// Slicing-by-4 extends this to process 4 bytes at once.
static void generate_crc24_slicing_tables(uint32_t  poly,
                                          uint32_t* s0,
                                          uint32_t* s1,
                                          uint32_t* s2,
                                          uint32_t* s3,
                                          uint32_t* base_table)
{
  // S3 is the base table (same as standard byte-at-a-time table)
  for (int i = 0; i < 256; i++) {
    s3[i] = base_table[i];
  }

  // Generate S2, S1, S0 using the relation:
  // S[k-1][i] = (S[k][i] << 8) ^ S3[(S[k][i] >> 16) & 0xFF]
  for (int i = 0; i < 256; i++) {
    s2[i] = ((s3[i] << 8) ^ base_table[(s3[i] >> 16) & 0xFF]) & 0xFFFFFF;
  }
  for (int i = 0; i < 256; i++) {
    s1[i] = ((s2[i] << 8) ^ base_table[(s2[i] >> 16) & 0xFF]) & 0xFFFFFF;
  }
  for (int i = 0; i < 256; i++) {
    s0[i] = ((s1[i] << 8) ^ base_table[(s1[i] >> 16) & 0xFF]) & 0xFFFFFF;
  }
}

static void generate_crc24_slicing8_tables(uint32_t* base_table,
                                           uint32_t* s0,
                                           uint32_t* s1,
                                           uint32_t* s2,
                                           uint32_t* s3,
                                           uint32_t* s4,
                                           uint32_t* s5,
                                           uint32_t* s6,
                                           uint32_t* s7)
{
  for (int i = 0; i < 256; i++) {
    s7[i] = base_table[i];
  }
  for (int i = 0; i < 256; i++) {
    s6[i] = ((s7[i] << 8) ^ base_table[(s7[i] >> 16) & 0xFF]) & 0xFFFFFF;
  }
  for (int i = 0; i < 256; i++) {
    s5[i] = ((s6[i] << 8) ^ base_table[(s6[i] >> 16) & 0xFF]) & 0xFFFFFF;
  }
  for (int i = 0; i < 256; i++) {
    s4[i] = ((s5[i] << 8) ^ base_table[(s5[i] >> 16) & 0xFF]) & 0xFFFFFF;
  }
  for (int i = 0; i < 256; i++) {
    s3[i] = ((s4[i] << 8) ^ base_table[(s4[i] >> 16) & 0xFF]) & 0xFFFFFF;
  }
  for (int i = 0; i < 256; i++) {
    s2[i] = ((s3[i] << 8) ^ base_table[(s3[i] >> 16) & 0xFF]) & 0xFFFFFF;
  }
  for (int i = 0; i < 256; i++) {
    s1[i] = ((s2[i] << 8) ^ base_table[(s2[i] >> 16) & 0xFF]) & 0xFFFFFF;
  }
  for (int i = 0; i < 256; i++) {
    s0[i] = ((s1[i] << 8) ^ base_table[(s1[i] >> 16) & 0xFF]) & 0xFFFFFF;
  }
}

/// \brief Generate CRC-16 slicing-by-4 tables for MSB-first processing
static void generate_crc16_slicing_tables(uint16_t* base_table, uint32_t* s0, uint32_t* s1, uint32_t* s2, uint32_t* s3)
{
  // S3 is the base table
  for (int i = 0; i < 256; i++) {
    s3[i] = base_table[i];
  }

  // Generate S2, S1, S0
  for (int i = 0; i < 256; i++) {
    s2[i] = ((s3[i] << 8) ^ base_table[(s3[i] >> 8) & 0xFF]) & 0xFFFF;
  }
  for (int i = 0; i < 256; i++) {
    s1[i] = ((s2[i] << 8) ^ base_table[(s2[i] >> 8) & 0xFF]) & 0xFFFF;
  }
  for (int i = 0; i < 256; i++) {
    s0[i] = ((s1[i] << 8) ^ base_table[(s1[i] >> 8) & 0xFF]) & 0xFFFF;
  }
}

static cuda_result init_slicing_tables()
{
  if (slicing_tables_initialized)
    return {};

  // Ensure base tables are initialized first
  if (!init_crc_tables())
    return make_unexpected(std::string("The CRC tables could not be prepared"));

  // Generate slicing-by-4 tables
  generate_crc24_slicing_tables(CRC24A_POLY, h_crc24a_s0, h_crc24a_s1, h_crc24a_s2, h_crc24a_s3, h_crc24a_table);
  generate_crc24_slicing8_tables(h_crc24a_table,
                                 h_crc24a_s8_0,
                                 h_crc24a_s8_1,
                                 h_crc24a_s8_2,
                                 h_crc24a_s8_3,
                                 h_crc24a_s8_4,
                                 h_crc24a_s8_5,
                                 h_crc24a_s8_6,
                                 h_crc24a_s8_7);
  generate_crc24_slicing_tables(CRC24B_POLY, h_crc24b_s0, h_crc24b_s1, h_crc24b_s2, h_crc24b_s3, h_crc24b_table);
  generate_crc16_slicing_tables(h_crc16_table, h_crc16_s0, h_crc16_s1, h_crc16_s2, h_crc16_s3);

  // Copy to device constant memory
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24A_S0, h_crc24a_s0, sizeof(h_crc24a_s0)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24A_S1, h_crc24a_s1, sizeof(h_crc24a_s1)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24A_S2, h_crc24a_s2, sizeof(h_crc24a_s2)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24A_S3, h_crc24a_s3, sizeof(h_crc24a_s3)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24A_S8_0, h_crc24a_s8_0, sizeof(h_crc24a_s8_0)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24A_S8_1, h_crc24a_s8_1, sizeof(h_crc24a_s8_1)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24A_S8_2, h_crc24a_s8_2, sizeof(h_crc24a_s8_2)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24A_S8_3, h_crc24a_s8_3, sizeof(h_crc24a_s8_3)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24A_S8_4, h_crc24a_s8_4, sizeof(h_crc24a_s8_4)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24A_S8_5, h_crc24a_s8_5, sizeof(h_crc24a_s8_5)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24A_S8_6, h_crc24a_s8_6, sizeof(h_crc24a_s8_6)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24A_S8_7, h_crc24a_s8_7, sizeof(h_crc24a_s8_7)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }

  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24B_S0, h_crc24b_s0, sizeof(h_crc24b_s0)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24B_S1, h_crc24b_s1, sizeof(h_crc24b_s1)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24B_S2, h_crc24b_s2, sizeof(h_crc24b_s2)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(CRC24B_S3, h_crc24b_s3, sizeof(h_crc24b_s3)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }

  if (cuda_result status =
          check_cuda_error(cudaMemcpyToSymbol(CRC16_S0, h_crc16_s0, sizeof(h_crc16_s0)), "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status =
          check_cuda_error(cudaMemcpyToSymbol(CRC16_S1, h_crc16_s1, sizeof(h_crc16_s1)), "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status =
          check_cuda_error(cudaMemcpyToSymbol(CRC16_S2, h_crc16_s2, sizeof(h_crc16_s2)), "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status =
          check_cuda_error(cudaMemcpyToSymbol(CRC16_S3, h_crc16_s3, sizeof(h_crc16_s3)), "upload the checksum tables");
      !status.has_value()) {
    return status;
  }

  slicing_tables_initialized = true;
  // success
  return {};
}

// ============================================================================
// Precomputed x^(8*2^i) powers for O(log n) CRC shifting (forward declarations)
// ============================================================================
__device__ __constant__ uint32_t CRC24A_X_POWERS[21];
__device__ __constant__ uint32_t CRC16_X_POWERS[21];
static uint32_t                  h_crc24a_x_powers[21];
static uint32_t                  h_crc16_x_powers[21];
static bool                      x_powers_initialized       = false;
static bool                      crc16_x_powers_initialized = false;

// Payload size from which the wider block pays for itself, measured on sm_89.
static constexpr int CRC24A_WARP256_MIN_BYTES = 32768;

// Host-side GF(2^24) multiplication for table generation
static uint32_t gf24_multiply_host(uint32_t a, uint32_t b)
{
  uint32_t result = 0;
  while (b) {
    if (b & 1)
      result ^= a;
    b >>= 1;
    if (a & 0x800000) {
      a = ((a << 1) ^ CRC24A_POLY) & 0xFFFFFF;
    } else {
      a = (a << 1) & 0xFFFFFF;
    }
  }
  return result;
}

static uint32_t gf16_multiply_host(uint32_t a, uint32_t b)
{
  uint32_t result = 0;
  while (b) {
    if (b & 1)
      result ^= a;
    b >>= 1;
    if (a & 0x8000) {
      a = ((a << 1) ^ CRC16_POLY) & 0xFFFF;
    } else {
      a = (a << 1) & 0xFFFF;
    }
  }
  return result;
}

// Initialize precomputed x^(8*2^i) powers for fast CRC shifting
static cuda_result init_x_powers_table()
{
  if (x_powers_initialized)
    return {};

  // x^8 = process 1 byte of zeros starting from CRC=1
  // More precisely, x^8 mod G(x) where G(x) is CRC24A polynomial
  // We compute this by shifting 1 through 8 zero bits
  uint32_t x_8 = 1;
  for (int i = 0; i < 8; i++) {
    if (x_8 & 0x800000) {
      x_8 = ((x_8 << 1) ^ CRC24A_POLY) & 0xFFFFFF;
    } else {
      x_8 = (x_8 << 1) & 0xFFFFFF;
    }
  }

  // Now compute x^(8*2^i) by repeated squaring
  // x^8
  h_crc24a_x_powers[0] = x_8;
  for (int i = 1; i < 21; i++) {
    // x^(8*2^i) = (x^(8*2^(i-1)))^2
    h_crc24a_x_powers[i] = gf24_multiply_host(h_crc24a_x_powers[i - 1], h_crc24a_x_powers[i - 1]);
  }

  // Copy to device constant memory
  if (cuda_result status =
          check_cuda_error(cudaMemcpyToSymbol(CRC24A_X_POWERS, h_crc24a_x_powers, sizeof(h_crc24a_x_powers)),
                           "upload the checksum tables");
      !status.has_value()) {
    return status;
  }

  x_powers_initialized = true;
  // success
  return {};
}

static void init_crc16_x_powers_table()
{
  if (crc16_x_powers_initialized)
    return;

  uint32_t x_8 = 1;
  for (int i = 0; i < 8; i++) {
    if (x_8 & 0x8000) {
      x_8 = ((x_8 << 1) ^ CRC16_POLY) & 0xFFFF;
    } else {
      x_8 = (x_8 << 1) & 0xFFFF;
    }
  }

  h_crc16_x_powers[0] = x_8;
  for (int i = 1; i < 21; i++) {
    h_crc16_x_powers[i] = gf16_multiply_host(h_crc16_x_powers[i - 1], h_crc16_x_powers[i - 1]);
  }

  cudaMemcpyToSymbol(CRC16_X_POWERS, h_crc16_x_powers, sizeof(h_crc16_x_powers));
  crc16_x_powers_initialized = true;
}

// ============================================================================
// CUDA Kernels
// ============================================================================

/// \brief SIMPLE SINGLE-THREAD CRC-24A kernel for small TBs
///
/// Optimized for tiny TBs (≤1056 bytes / 8448 bits) where the overhead of
/// warp-cooperative prefetching exceeds the benefit. Uses slicing-by-4
/// with a single thread for minimal kernel launch overhead.
///
/// Expected performance: ~15-20µs for 1KB TB (vs ~40µs for warp-coop kernel)
__global__ void __launch_bounds__(1, 1)
    crc24a_simple_kernel(const uint8_t* __restrict__ d_data,
                         uint32_t* __restrict__ d_crc,
                         int      num_bytes,
                         unsigned num_bits // Actual bit count for masking partial last byte
    )
{
  uint32_t crc = 0;

  // For non-byte-aligned data, use pure bit-by-bit processing
  // (mixing table and bit-by-bit is complex due to implicit x^24 factor in tables)
  if (num_bits % 8 != 0) {
    // Process all data bits, MSB first
    for (int bit_idx = 0; bit_idx < num_bits; bit_idx++) {
      int     byte_idx = bit_idx / 8;
      int     bit_pos  = 7 - (bit_idx % 8);
      uint8_t data_bit = (d_data[byte_idx] >> bit_pos) & 1;

      uint32_t msb = (crc >> 23) & 1;
      crc          = (crc << 1) | data_bit;
      if (msb) {
        // 0x864CFB
        crc ^= CRC24A_POLY;
      }
      crc &= 0xFFFFFF;
    }

    // Flush with 24 zero bits
    for (int i = 0; i < 24; i++) {
      uint32_t msb = (crc >> 23) & 1;
      crc          = crc << 1;
      if (msb) {
        crc ^= CRC24A_POLY;
      }
      crc &= 0xFFFFFF;
    }

    *d_crc = crc;
    return;
  }

  // Byte-aligned data: use fast table-based approach
  int i         = 0;
  int num_words = num_bytes / 4;

  // Process 4 bytes at a time using slicing-by-4
  for (int w = 0; w < num_words; w++) {
    uint8_t b0 = d_data[i++];
    uint8_t b1 = d_data[i++];
    uint8_t b2 = d_data[i++];
    uint8_t b3 = d_data[i++];

    uint8_t idx0 = ((crc >> 16) ^ b0) & 0xFF;
    uint8_t idx1 = ((crc >> 8) ^ b1) & 0xFF;
    uint8_t idx2 = (crc ^ b2) & 0xFF;

    crc = CRC24A_S0[idx0] ^ CRC24A_S1[idx1] ^ CRC24A_S2[idx2] ^ CRC24A_S3[b3];
  }

  // Process remaining bytes (byte-by-byte)
  while (i < num_bytes) {
    uint8_t byte  = d_data[i++];
    uint8_t index = ((crc >> 16) ^ byte) & 0xFF;
    crc           = (crc << 8) ^ CRC24A_TABLE[index];
  }

  *d_crc = crc & 0xFFFFFF;
}

/// \brief FUSED CRC-24A compute and attach kernel
///
/// Computes CRC-24A and attaches it directly to the TB buffer in one kernel.
/// Thread 0 computes CRC, broadcasts via shared memory, all 24 threads attach bits.
/// Saves 1 kernel launch + 1 memset vs separate compute/attach.
__global__ void crc24a_compute_and_attach_kernel(uint8_t* __restrict__ d_tb_with_crc, unsigned tb_size_bits)
{
  int                 tid = threadIdx.x;
  __shared__ uint32_t s_crc;

  // Thread 0 computes CRC
  if (tid == 0) {
    int      num_bytes = (tb_size_bits + 7) / 8;
    uint32_t crc       = 0;

    // For non-byte-aligned data, use pure bit-by-bit processing
    if (tb_size_bits % 8 != 0) {
      // Process all data bits, MSB first
      for (int bit_idx = 0; bit_idx < tb_size_bits; bit_idx++) {
        int     byte_idx = bit_idx / 8;
        int     bit_pos  = 7 - (bit_idx % 8);
        uint8_t data_bit = (d_tb_with_crc[byte_idx] >> bit_pos) & 1;

        uint32_t msb = (crc >> 23) & 1;
        crc          = (crc << 1) | data_bit;
        if (msb) {
          crc ^= CRC24A_POLY;
        }
        crc &= 0xFFFFFF;
      }

      // Flush with 24 zero bits
      for (int i = 0; i < 24; i++) {
        uint32_t msb = (crc >> 23) & 1;
        crc          = crc << 1;
        if (msb) {
          crc ^= CRC24A_POLY;
        }
        crc &= 0xFFFFFF;
      }

      s_crc = crc;
    } else {
      // Byte-aligned: use fast table-based approach
      int i         = 0;
      int num_words = num_bytes / 4;

      for (int w = 0; w < num_words; w++) {
        uint8_t b0 = d_tb_with_crc[i++];
        uint8_t b1 = d_tb_with_crc[i++];
        uint8_t b2 = d_tb_with_crc[i++];
        uint8_t b3 = d_tb_with_crc[i++];

        uint8_t idx0 = ((crc >> 16) ^ b0) & 0xFF;
        uint8_t idx1 = ((crc >> 8) ^ b1) & 0xFF;
        uint8_t idx2 = (crc ^ b2) & 0xFF;

        crc = CRC24A_S0[idx0] ^ CRC24A_S1[idx1] ^ CRC24A_S2[idx2] ^ CRC24A_S3[b3];
      }

      while (i < num_bytes) {
        uint8_t byte  = d_tb_with_crc[i++];
        uint8_t index = ((crc >> 16) ^ byte) & 0xFF;
        crc           = (crc << 8) ^ CRC24A_TABLE[index];
      }

      s_crc = crc & 0xFFFFFF;
    }
  }
  __syncthreads();

  // All 24 threads attach CRC bits in parallel
  if (tid >= 24)
    return;

  uint32_t crc_value = s_crc;

  // CRC bits stored MSB first (bit 0 = MSB of CRC)
  int     crc_bit_pos = 23 - tid;
  uint8_t bit         = (crc_value >> crc_bit_pos) & 1;

  // Position in output buffer
  int out_bit_idx = tb_size_bits + tid;
  int byte_idx    = out_bit_idx / 8;
  int bit_pos     = 7 - (out_bit_idx % 8);

  // Use atomic to safely set the bit (only 24 threads, minimal contention)
  atomicOr((uint32_t*)&d_tb_with_crc[byte_idx & ~3], (uint32_t)bit << (bit_pos + 8 * (byte_idx & 3)));
}

/// \brief FUSED CRC-16 compute and attach kernel
__global__ void crc16_compute_and_attach_kernel(uint8_t* __restrict__ d_tb_with_crc, unsigned tb_size_bits)
{
  int                 tid = threadIdx.x;
  __shared__ uint32_t s_crc;

  // Thread 0 computes CRC-16
  if (tid == 0) {
    int      num_bytes = (tb_size_bits + 7) / 8;
    uint32_t crc       = 0;

    // For non-byte-aligned data, use pure bit-by-bit processing
    if (tb_size_bits % 8 != 0) {
      // Process all data bits, MSB first
      for (int bit_idx = 0; bit_idx < tb_size_bits; bit_idx++) {
        int     byte_idx = bit_idx / 8;
        int     bit_pos  = 7 - (bit_idx % 8);
        uint8_t data_bit = (d_tb_with_crc[byte_idx] >> bit_pos) & 1;

        uint16_t msb = (crc >> 15) & 1;
        crc          = (crc << 1) | data_bit;
        if (msb) {
          crc ^= CRC16_POLY;
        }
        crc &= 0xFFFF;
      }

      // Flush with 16 zero bits
      for (int i = 0; i < 16; i++) {
        uint16_t msb = (crc >> 15) & 1;
        crc          = crc << 1;
        if (msb) {
          crc ^= CRC16_POLY;
        }
        crc &= 0xFFFF;
      }

      s_crc = crc;
    } else {
      // Byte-aligned: use fast table-based approach
      int i         = 0;
      int num_words = num_bytes / 4;

      for (int w = 0; w < num_words; w++) {
        uint8_t b0 = d_tb_with_crc[i++];
        uint8_t b1 = d_tb_with_crc[i++];
        uint8_t b2 = d_tb_with_crc[i++];
        uint8_t b3 = d_tb_with_crc[i++];

        uint8_t idx0 = ((crc >> 8) ^ b0) & 0xFF;
        uint8_t idx1 = (crc ^ b1) & 0xFF;
        uint8_t idx2 = b2;
        uint8_t idx3 = b3;

        crc = CRC16_S0[idx0] ^ CRC16_S1[idx1] ^ CRC16_S2[idx2] ^ CRC16_S3[idx3];
      }

      while (i < num_bytes) {
        uint8_t byte  = d_tb_with_crc[i++];
        uint8_t index = ((crc >> 8) ^ byte) & 0xFF;
        crc           = (crc << 8) ^ CRC16_TABLE[index];
      }

      s_crc = crc & 0xFFFF;
    }
  }
  __syncthreads();

  // All 16 threads attach CRC bits
  if (tid >= 16)
    return;

  uint32_t crc_value   = s_crc;
  int      crc_bit_pos = 15 - tid;
  uint8_t  bit         = (crc_value >> crc_bit_pos) & 1;

  int out_bit_idx = tb_size_bits + tid;
  int byte_idx    = out_bit_idx / 8;
  int bit_pos     = 7 - (out_bit_idx % 8);

  atomicOr((uint32_t*)&d_tb_with_crc[byte_idx & ~3], (uint32_t)bit << (bit_pos + 8 * (byte_idx & 3)));
}

/// \brief CRC-24B computation kernel using slicing-by-4 (optimized)
__global__ void crc24b_kernel(const uint8_t* __restrict__ d_data, uint32_t* __restrict__ d_crc, int num_bytes)
{
  if (threadIdx.x != 0 || blockIdx.x != 0)
    return;

  uint32_t crc = 0;
  int      i   = 0;

  // Process 4 bytes at a time using slicing-by-4
  int num_words = num_bytes / 4;
  for (int w = 0; w < num_words; w++) {
    uint8_t b0 = d_data[i++];
    uint8_t b1 = d_data[i++];
    uint8_t b2 = d_data[i++];
    uint8_t b3 = d_data[i++];

    uint8_t idx0 = ((crc >> 16) ^ b0) & 0xFF;
    uint8_t idx1 = ((crc >> 8) ^ b1) & 0xFF;
    uint8_t idx2 = (crc ^ b2) & 0xFF;
    uint8_t idx3 = b3;

    crc = CRC24B_S0[idx0] ^ CRC24B_S1[idx1] ^ CRC24B_S2[idx2] ^ CRC24B_S3[idx3];
  }

  // Process remaining bytes
  while (i < num_bytes) {
    uint8_t byte  = d_data[i++];
    uint8_t index = ((crc >> 16) ^ byte) & 0xFF;
    crc           = (crc << 8) ^ CRC24B_TABLE[index];
  }

  *d_crc = crc & 0xFFFFFF;
}

/// \brief CRC-16 computation kernel using slicing-by-4 (optimized)
__global__ void crc16_kernel(const uint8_t* __restrict__ d_data,
                             uint16_t* __restrict__ d_crc,
                             int      num_bytes,
                             unsigned num_bits // Actual bit count for masking partial last byte
)
{
  if (threadIdx.x != 0 || blockIdx.x != 0)
    return;

  // Use 32-bit for intermediate calculations
  uint32_t crc = 0;

  // For non-byte-aligned data, use pure bit-by-bit processing
  if (num_bits % 8 != 0) {
    // Process all data bits, MSB first
    for (int bit_idx = 0; bit_idx < num_bits; bit_idx++) {
      int     byte_idx = bit_idx / 8;
      int     bit_pos  = 7 - (bit_idx % 8);
      uint8_t data_bit = (d_data[byte_idx] >> bit_pos) & 1;

      uint16_t msb = (crc >> 15) & 1;
      crc          = (crc << 1) | data_bit;
      if (msb) {
        // 0x1021
        crc ^= CRC16_POLY;
      }
      crc &= 0xFFFF;
    }

    // Flush with 16 zero bits
    for (int i = 0; i < 16; i++) {
      uint16_t msb = (crc >> 15) & 1;
      crc          = crc << 1;
      if (msb) {
        crc ^= CRC16_POLY;
      }
      crc &= 0xFFFF;
    }

    *d_crc = (uint16_t)crc;
    return;
  }

  // Byte-aligned data: use fast table-based approach
  int i         = 0;
  int num_words = num_bytes / 4;

  // Process 4 bytes at a time using slicing-by-4
  for (int w = 0; w < num_words; w++) {
    uint8_t b0 = d_data[i++];
    uint8_t b1 = d_data[i++];
    uint8_t b2 = d_data[i++];
    uint8_t b3 = d_data[i++];

    uint8_t idx0 = ((crc >> 8) ^ b0) & 0xFF;
    uint8_t idx1 = (crc ^ b1) & 0xFF;
    uint8_t idx2 = b2;
    uint8_t idx3 = b3;

    crc = CRC16_S0[idx0] ^ CRC16_S1[idx1] ^ CRC16_S2[idx2] ^ CRC16_S3[idx3];
  }

  // Process remaining bytes (byte-by-byte)
  while (i < num_bytes) {
    uint8_t byte  = d_data[i++];
    uint8_t index = ((crc >> 8) ^ byte) & 0xFF;
    crc           = (crc << 8) ^ CRC16_TABLE[index];
  }

  *d_crc = (uint16_t)(crc & 0xFFFF);
}

__device__ __forceinline__ uint32_t gf16_multiply(uint32_t a, uint32_t b)
{
  uint32_t result = 0;

#pragma unroll
  for (int i = 0; i < 16; i++) {
    result ^= (b & 1) ? a : 0;
    b >>= 1;
    uint32_t msb = a & 0x8000;
    a            = (a << 1) & 0xFFFF;
    a ^= msb ? CRC16_POLY : 0;
  }

  return result & 0xFFFF;
}

__device__ __forceinline__ uint32_t crc16_shift_bytes_fast(uint32_t crc, int n_bytes)
{
  if (n_bytes == 0 || crc == 0)
    return crc;

  uint32_t x_power = 0;
  bool     first   = true;

  for (int i = 0; i < 21 && n_bytes > 0; i++) {
    if (n_bytes & 1) {
      if (first) {
        x_power = CRC16_X_POWERS[i];
        first   = false;
      } else {
        x_power = gf16_multiply(x_power, CRC16_X_POWERS[i]);
      }
    }
    n_bytes >>= 1;
  }

  return first ? crc : gf16_multiply(crc, x_power);
}

__global__ void __launch_bounds__(32, 1)
    crc16_warp_reduce_kernel(const uint8_t* __restrict__ d_data, uint16_t* __restrict__ d_crc, int total_bytes)
{
  const int lane_id = threadIdx.x & 31;

  int bytes_per_thread = ((total_bytes + 31) / 32 + 3) & ~3;
  int my_start         = lane_id * bytes_per_thread;
  int my_end           = min(my_start + bytes_per_thread, total_bytes);
  int my_bytes         = max(my_end - my_start, 0);

  uint32_t my_crc = 0;
  if (my_bytes > 0) {
    const uint8_t* my_data   = d_data + my_start;
    int            pos       = 0;
    int            num_words = my_bytes >> 2;

    for (int w = 0; w < num_words; ++w) {
      uint8_t b0 = my_data[pos];
      uint8_t b1 = my_data[pos + 1];
      uint8_t b2 = my_data[pos + 2];
      uint8_t b3 = my_data[pos + 3];
      pos += 4;

      uint8_t idx0 = ((my_crc >> 8) ^ b0) & 0xFF;
      uint8_t idx1 = (my_crc ^ b1) & 0xFF;
      my_crc       = CRC16_S0[idx0] ^ CRC16_S1[idx1] ^ CRC16_S2[b2] ^ CRC16_S3[b3];
    }

    while (pos < my_bytes) {
      uint8_t byte  = my_data[pos++];
      uint8_t index = ((my_crc >> 8) ^ byte) & 0xFF;
      my_crc        = ((my_crc << 8) ^ CRC16_TABLE[index]) & 0xFFFF;
    }
  }

#pragma unroll
  for (int offset = 1; offset < 32; offset <<= 1) {
    uint32_t right_crc   = __shfl_down_sync(0xFFFFFFFF, my_crc, offset);
    int      right_bytes = __shfl_down_sync(0xFFFFFFFF, my_bytes, offset);
    if ((lane_id & ((offset << 1) - 1)) == 0) {
      my_crc = crc16_shift_bytes_fast(my_crc, right_bytes) ^ right_crc;
      my_bytes += right_bytes;
    }
  }

  if (lane_id == 0) {
    *d_crc = static_cast<uint16_t>(my_crc & 0xFFFF);
  }
}

// ============================================================================
// MASSIVELY PARALLEL CRC24A IMPLEMENTATION
// ============================================================================
// Uses chunked processing with CRC shift-and-combine for parallel execution.
// Key insight: CRC(A || B) = shift(CRC(A), len(B)*8) XOR CRC(B)
// Where shift(crc, n) = crc * x^n mod G(x) in GF(2)
// ============================================================================

// Precomputed table for CRC24A shift by 2^i bytes (i = 0..20)
// shift_table[i] = polynomial to use for shifting CRC by 2^i bytes = 2^(i+3) bits
// Generated at init time using repeated squaring
__device__ __constant__ uint32_t CRC24A_SHIFT_TABLE[21]; ///< Shift by 1,2,4,8,...,1M bytes

/// \brief Multiply two elements in GF(2^24) modulo CRC24A polynomial
///
/// Computes (a * b) mod G(x) using Russian peasant multiplication.
/// FULLY UNROLLED for GPU performance - no data-dependent branches!
__device__ __forceinline__ uint32_t gf24_multiply(uint32_t a, uint32_t b)
{
  uint32_t result = 0;

// Unroll all 24 iterations - GPU hates data-dependent loops!
#pragma unroll
  for (int i = 0; i < 24; i++) {
    result ^= (b & 1) ? a : 0;
    b >>= 1;
    // a = a * x mod G(x)
    uint32_t msb = a & 0x800000;
    a            = (a << 1) & 0xFFFFFF;
    a ^= msb ? CRC24A_POLY : 0;
  }

  return result;
}

__device__ __forceinline__ uint32_t crc24a_x_power_bytes(int n_bytes)
{
  if (n_bytes == 0)
    return 1;

  uint32_t x_power = 0;
  bool     first   = true;

  for (int i = 0; i < 21 && n_bytes > 0; i++) {
    if (n_bytes & 1) {
      if (first) {
        x_power = CRC24A_X_POWERS[i];
        first   = false;
      } else {
        x_power = gf24_multiply(x_power, CRC24A_X_POWERS[i]);
      }
    }
    n_bytes >>= 1;
  }

  return first ? 1 : x_power;
}

/// \brief Compute CRC24A shift by n bytes using precomputed x^(8*2^i) powers
///
/// shift(crc, n*8 bits) = crc * x^(n*8) mod G(x)
///
/// Using binary decomposition: x^(n*8) = product of x^(8*2^i) for set bits in n
/// This is O(log n) multiplications instead of O(n) bit shifts!
__device__ __forceinline__ uint32_t crc24a_shift_bytes_fast(uint32_t crc, int n_bytes)
{
  if (n_bytes == 0 || crc == 0)
    return crc;

  return gf24_multiply(crc, crc24a_x_power_bytes(n_bytes));
}

/// \brief Warp-reduction CRC24A generator for large byte-aligned transport blocks
///
/// This mirrors the optimized PUSCH TB CRC check shape: 128 threads compute
/// contiguous byte ranges, then combine partial CRCs with shuffle reductions.
/// It avoids the two-kernel chunk+combine path and its final D2D copy.
template <bool WRITE_PARTIAL>
__global__ void __launch_bounds__(256, 1) crc24a_warp_reduce_kernel(const uint8_t* __restrict__ d_data,
                                                                    uint32_t* __restrict__ d_result,
                                                                    int* __restrict__ d_chunk_sizes,
                                                                    int total_bytes,
                                                                    int chunk_size)
{
  int block_bytes = total_bytes;
  if constexpr (WRITE_PARTIAL) {
    // Each block reduces one contiguous slice; the host combines the partials afterwards.
    int chunk_start = blockIdx.x * chunk_size;
    if (chunk_start >= total_bytes) {
      if (threadIdx.x == 0) {
        d_result[blockIdx.x]      = 0;
        d_chunk_sizes[blockIdx.x] = 0;
      }
      return;
    }
    d_data += chunk_start;
    block_bytes = min(chunk_size, total_bytes - chunk_start);
  }
  __shared__ uint32_t s_S0[256], s_S1[256], s_S2[256], s_S3[256];
  __shared__ uint32_t s_TBL[256];
  __shared__ uint32_t s_warp_crcs[8];
  __shared__ int      s_warp_bytes[8];

  const int tid       = threadIdx.x;
  const int warp_id   = tid >> 5;
  const int lane_id   = tid & 31;
  const int num_warps = blockDim.x >> 5;

#pragma unroll
  for (int i = tid; i < 256; i += blockDim.x) {
    s_S0[i]  = CRC24A_S0[i];
    s_S1[i]  = CRC24A_S1[i];
    s_S2[i]  = CRC24A_S2[i];
    s_S3[i]  = CRC24A_S3[i];
    s_TBL[i] = CRC24A_TABLE[i];
  }
  __syncthreads();

  int bytes_per_thread = ((block_bytes + blockDim.x - 1) / blockDim.x + 3) & ~3;
  int my_start         = tid * bytes_per_thread;
  int my_end           = min(my_start + bytes_per_thread, block_bytes);
  int my_bytes         = max(my_end - my_start, 0);

  uint32_t my_crc = 0;
  if (my_bytes > 0) {
    const uint8_t*  my_data   = d_data + my_start;
    const uint32_t* my_words  = reinterpret_cast<const uint32_t*>(my_data);
    int             pos       = 0;
    int             num_words = my_bytes >> 2;

    int w = 0;
    for (; w + 1 < num_words; w += 2) {
      uint32_t word = my_words[w];
      uint8_t  b0   = word & 0xFF;
      uint8_t  b1   = (word >> 8) & 0xFF;
      uint8_t  b2   = (word >> 16) & 0xFF;
      uint8_t  b3   = (word >> 24) & 0xFF;

      my_crc = s_S0[((my_crc >> 16) ^ b0) & 0xFF] ^ s_S1[((my_crc >> 8) ^ b1) & 0xFF] ^ s_S2[(my_crc ^ b2) & 0xFF] ^
               s_S3[b3];

      word = my_words[w + 1];
      b0   = word & 0xFF;
      b1   = (word >> 8) & 0xFF;
      b2   = (word >> 16) & 0xFF;
      b3   = (word >> 24) & 0xFF;

      my_crc = s_S0[((my_crc >> 16) ^ b0) & 0xFF] ^ s_S1[((my_crc >> 8) ^ b1) & 0xFF] ^ s_S2[(my_crc ^ b2) & 0xFF] ^
               s_S3[b3];
      pos += 8;
    }

    if (w < num_words) {
      uint32_t word = my_words[w];
      uint8_t  b0   = word & 0xFF;
      uint8_t  b1   = (word >> 8) & 0xFF;
      uint8_t  b2   = (word >> 16) & 0xFF;
      uint8_t  b3   = (word >> 24) & 0xFF;
      pos += 4;

      my_crc = s_S0[((my_crc >> 16) ^ b0) & 0xFF] ^ s_S1[((my_crc >> 8) ^ b1) & 0xFF] ^ s_S2[(my_crc ^ b2) & 0xFF] ^
               s_S3[b3];
    }

    while (pos < my_bytes) {
      uint8_t byte = my_data[pos++];
      my_crc       = ((my_crc << 8) ^ s_TBL[((my_crc >> 16) ^ byte) & 0xFF]) & 0xFFFFFF;
    }

    my_crc &= 0xFFFFFF;
  }

#pragma unroll
  for (int offset = 1; offset < 32; offset <<= 1) {
    uint32_t right_crc   = __shfl_down_sync(0xFFFFFFFF, my_crc, offset);
    int      right_bytes = __shfl_down_sync(0xFFFFFFFF, my_bytes, offset);
    if ((lane_id & ((offset << 1) - 1)) == 0) {
      my_crc = crc24a_shift_bytes_fast(my_crc, right_bytes) ^ right_crc;
      my_bytes += right_bytes;
    }
  }

  if (lane_id == 0) {
    s_warp_crcs[warp_id]  = my_crc;
    s_warp_bytes[warp_id] = my_bytes;
  }
  __syncthreads();

  if (warp_id == 0) {
    my_crc   = (lane_id < num_warps) ? s_warp_crcs[lane_id] : 0;
    my_bytes = (lane_id < num_warps) ? s_warp_bytes[lane_id] : 0;

#pragma unroll
    for (int offset = 1; offset < 8; offset <<= 1) {
      uint32_t right_crc   = __shfl_down_sync(0xFFFFFFFF, my_crc, offset);
      int      right_bytes = __shfl_down_sync(0xFFFFFFFF, my_bytes, offset);
      if (lane_id < num_warps && lane_id + offset < num_warps && (lane_id & ((offset << 1) - 1)) == 0) {
        my_crc = crc24a_shift_bytes_fast(my_crc, right_bytes) ^ right_crc;
        my_bytes += right_bytes;
      }
    }

    if (lane_id == 0) {
      if constexpr (WRITE_PARTIAL) {
        d_result[blockIdx.x]      = my_crc & 0xFFFFFF;
        d_chunk_sizes[blockIdx.x] = my_bytes;
      } else {
        *d_result = my_crc & 0xFFFFFF;
      }
    }
  }
}

__global__ void __launch_bounds__(256, 1) crc24a_warp_reduce_slice8_kernel(const uint8_t* __restrict__ d_data,
                                                                           uint32_t* __restrict__ d_result,
                                                                           int total_bytes)
{
  __shared__ uint32_t s_S0[256], s_S1[256], s_S2[256], s_S3[256];
  __shared__ uint32_t s_S4[256], s_S5[256], s_S6[256], s_S7[256];
  __shared__ uint32_t s_TBL[256];
  __shared__ uint32_t s_warp_crcs[8];
  __shared__ int      s_warp_bytes[8];

  const int tid       = threadIdx.x;
  const int warp_id   = tid >> 5;
  const int lane_id   = tid & 31;
  const int num_warps = blockDim.x >> 5;

#pragma unroll
  for (int i = tid; i < 256; i += blockDim.x) {
    s_S0[i]  = CRC24A_S8_0[i];
    s_S1[i]  = CRC24A_S8_1[i];
    s_S2[i]  = CRC24A_S8_2[i];
    s_S3[i]  = CRC24A_S8_3[i];
    s_S4[i]  = CRC24A_S8_4[i];
    s_S5[i]  = CRC24A_S8_5[i];
    s_S6[i]  = CRC24A_S8_6[i];
    s_S7[i]  = CRC24A_S8_7[i];
    s_TBL[i] = CRC24A_TABLE[i];
  }
  __syncthreads();

  int bytes_per_thread = ((total_bytes + blockDim.x - 1) / blockDim.x + 3) & ~3;
  int my_start         = tid * bytes_per_thread;
  int my_end           = min(my_start + bytes_per_thread, total_bytes);
  int my_bytes         = max(my_end - my_start, 0);

  uint32_t my_crc = 0;
  if (my_bytes > 0) {
    const uint8_t*  my_data   = d_data + my_start;
    const uint32_t* my_words  = reinterpret_cast<const uint32_t*>(my_data);
    int             pos       = 0;
    int             num_words = my_bytes >> 2;
    int             w         = 0;

    for (; w + 1 < num_words; w += 2) {
      uint32_t word0 = my_words[w];
      uint32_t word1 = my_words[w + 1];
      uint8_t  b0    = word0 & 0xFF;
      uint8_t  b1    = (word0 >> 8) & 0xFF;
      uint8_t  b2    = (word0 >> 16) & 0xFF;
      uint8_t  b3    = (word0 >> 24) & 0xFF;
      uint8_t  b4    = word1 & 0xFF;
      uint8_t  b5    = (word1 >> 8) & 0xFF;
      uint8_t  b6    = (word1 >> 16) & 0xFF;
      uint8_t  b7    = (word1 >> 24) & 0xFF;

      my_crc = s_S0[((my_crc >> 16) ^ b0) & 0xFF] ^ s_S1[((my_crc >> 8) ^ b1) & 0xFF] ^ s_S2[(my_crc ^ b2) & 0xFF] ^
               s_S3[b3] ^ s_S4[b4] ^ s_S5[b5] ^ s_S6[b6] ^ s_S7[b7];
      pos += 8;
    }

    if (w < num_words) {
      uint32_t word = my_words[w];
      uint8_t  b0   = word & 0xFF;
      uint8_t  b1   = (word >> 8) & 0xFF;
      uint8_t  b2   = (word >> 16) & 0xFF;
      uint8_t  b3   = (word >> 24) & 0xFF;
      my_crc = s_S4[((my_crc >> 16) ^ b0) & 0xFF] ^ s_S5[((my_crc >> 8) ^ b1) & 0xFF] ^ s_S6[(my_crc ^ b2) & 0xFF] ^
               s_S7[b3];
      pos += 4;
    }

    while (pos < my_bytes) {
      uint8_t byte = my_data[pos++];
      my_crc       = ((my_crc << 8) ^ s_TBL[((my_crc >> 16) ^ byte) & 0xFF]) & 0xFFFFFF;
    }

    my_crc &= 0xFFFFFF;
  }

#pragma unroll
  for (int offset = 1; offset < 32; offset <<= 1) {
    uint32_t right_crc   = __shfl_down_sync(0xFFFFFFFF, my_crc, offset);
    int      right_bytes = __shfl_down_sync(0xFFFFFFFF, my_bytes, offset);
    if ((lane_id & ((offset << 1) - 1)) == 0) {
      my_crc = crc24a_shift_bytes_fast(my_crc, right_bytes) ^ right_crc;
      my_bytes += right_bytes;
    }
  }

  if (lane_id == 0) {
    s_warp_crcs[warp_id]  = my_crc;
    s_warp_bytes[warp_id] = my_bytes;
  }
  __syncthreads();

  if (warp_id == 0) {
    my_crc   = (lane_id < num_warps) ? s_warp_crcs[lane_id] : 0;
    my_bytes = (lane_id < num_warps) ? s_warp_bytes[lane_id] : 0;

#pragma unroll
    for (int offset = 1; offset < 8; offset <<= 1) {
      uint32_t right_crc   = __shfl_down_sync(0xFFFFFFFF, my_crc, offset);
      int      right_bytes = __shfl_down_sync(0xFFFFFFFF, my_bytes, offset);
      if (lane_id < num_warps && lane_id + offset < num_warps && (lane_id & ((offset << 1) - 1)) == 0) {
        my_crc = crc24a_shift_bytes_fast(my_crc, right_bytes) ^ right_crc;
        my_bytes += right_bytes;
      }
    }

    if (lane_id == 0) {
      *d_result = my_crc & 0xFFFFFF;
    }
  }
}

/// \brief Warp-reduction CRC24A without per-call shared table staging.
///
/// This keeps the same byte-range split and CRC combine shape as
/// crc24a_warp_reduce_kernel, but reads CRC lookup tables directly from
/// constant memory. It is useful for mid-size TBs where the 5 KiB table copy
/// into shared memory can be comparable to the payload work.
__global__ void __launch_bounds__(256, 1) crc24a_warp_reduce_const_kernel(const uint8_t* __restrict__ d_data,
                                                                          uint32_t* __restrict__ d_result,
                                                                          int total_bytes)
{
  __shared__ uint32_t s_warp_crcs[8];
  __shared__ int      s_warp_bytes[8];

  const int tid       = threadIdx.x;
  const int warp_id   = tid >> 5;
  const int lane_id   = tid & 31;
  const int num_warps = blockDim.x >> 5;

  int bytes_per_thread = ((total_bytes + blockDim.x - 1) / blockDim.x + 3) & ~3;
  int my_start         = tid * bytes_per_thread;
  int my_end           = min(my_start + bytes_per_thread, total_bytes);
  int my_bytes         = max(my_end - my_start, 0);

  uint32_t my_crc = 0;
  if (my_bytes > 0) {
    const uint8_t*  my_data   = d_data + my_start;
    const uint32_t* my_words  = reinterpret_cast<const uint32_t*>(my_data);
    int             pos       = 0;
    int             num_words = my_bytes >> 2;

    for (int w = 0; w < num_words; ++w) {
      uint32_t word = my_words[w];
      uint8_t  b0   = word & 0xFF;
      uint8_t  b1   = (word >> 8) & 0xFF;
      uint8_t  b2   = (word >> 16) & 0xFF;
      uint8_t  b3   = (word >> 24) & 0xFF;
      pos += 4;

      my_crc = CRC24A_S0[((my_crc >> 16) ^ b0) & 0xFF] ^ CRC24A_S1[((my_crc >> 8) ^ b1) & 0xFF] ^
               CRC24A_S2[(my_crc ^ b2) & 0xFF] ^ CRC24A_S3[b3];
    }

    while (pos < my_bytes) {
      uint8_t byte = my_data[pos++];
      my_crc       = ((my_crc << 8) ^ CRC24A_TABLE[((my_crc >> 16) ^ byte) & 0xFF]) & 0xFFFFFF;
    }

    my_crc &= 0xFFFFFF;
  }

#pragma unroll
  for (int offset = 1; offset < 32; offset <<= 1) {
    uint32_t right_crc   = __shfl_down_sync(0xFFFFFFFF, my_crc, offset);
    int      right_bytes = __shfl_down_sync(0xFFFFFFFF, my_bytes, offset);
    if ((lane_id & ((offset << 1) - 1)) == 0) {
      my_crc = crc24a_shift_bytes_fast(my_crc, right_bytes) ^ right_crc;
      my_bytes += right_bytes;
    }
  }

  if (lane_id == 0) {
    s_warp_crcs[warp_id]  = my_crc;
    s_warp_bytes[warp_id] = my_bytes;
  }
  __syncthreads();

  if (warp_id == 0) {
    my_crc   = (lane_id < num_warps) ? s_warp_crcs[lane_id] : 0;
    my_bytes = (lane_id < num_warps) ? s_warp_bytes[lane_id] : 0;

#pragma unroll
    for (int offset = 1; offset < 8; offset <<= 1) {
      uint32_t right_crc   = __shfl_down_sync(0xFFFFFFFF, my_crc, offset);
      int      right_bytes = __shfl_down_sync(0xFFFFFFFF, my_bytes, offset);
      if (lane_id < num_warps && lane_id + offset < num_warps && (lane_id & ((offset << 1) - 1)) == 0) {
        my_crc = crc24a_shift_bytes_fast(my_crc, right_bytes) ^ right_crc;
        my_bytes += right_bytes;
      }
    }

    if (lane_id == 0) {
      *d_result = my_crc & 0xFFFFFF;
    }
  }
}

/// \brief ULTRA-FAST PARALLEL CRC24A - Single kernel with intra-block parallelism
///
/// Uses 8 threads per chunk, each computing CRC for 1/8 of the data.
/// Then combines within the block using the shift-and-combine formula.
///
/// For 90KB data with 8 chunks of ~11KB each:
/// - 8 blocks, 8 threads per block actively computing
/// - Each thread processes ~1.4KB
/// - Combine is fast (7 GF multiplications per block)
__global__ void __launch_bounds__(32, 8)
    crc24a_fast_parallel_kernel(const uint8_t* __restrict__ d_data, uint32_t* __restrict__ d_result, int total_bytes)
{
  // Use 8 chunks, one block handles entire CRC
  // 8 threads compute sub-chunk CRCs, thread 0 combines
  const int NUM_THREADS = 8;
  int       tid         = threadIdx.x;

  if (tid >= NUM_THREADS)
    return;

  // Divide data among threads
  int bytes_per_thread = (total_bytes + NUM_THREADS - 1) / NUM_THREADS;
  int my_start         = tid * bytes_per_thread;
  int my_end           = min(my_start + bytes_per_thread, total_bytes);
  int my_bytes         = (my_start < total_bytes) ? (my_end - my_start) : 0;

  // Each thread computes CRC for its portion
  uint32_t my_crc = 0;
  if (my_bytes > 0) {
    const uint8_t* my_data = d_data + my_start;
    int            i       = 0;

    // Process 4 bytes at a time using slicing-by-4
    int num_words = my_bytes / 4;
    for (int w = 0; w < num_words; w++) {
      uint8_t b0 = my_data[i++];
      uint8_t b1 = my_data[i++];
      uint8_t b2 = my_data[i++];
      uint8_t b3 = my_data[i++];

      uint8_t idx0 = ((my_crc >> 16) ^ b0) & 0xFF;
      uint8_t idx1 = ((my_crc >> 8) ^ b1) & 0xFF;
      uint8_t idx2 = (my_crc ^ b2) & 0xFF;
      my_crc       = CRC24A_S0[idx0] ^ CRC24A_S1[idx1] ^ CRC24A_S2[idx2] ^ CRC24A_S3[b3];
    }

    // Process remaining bytes
    while (i < my_bytes) {
      uint8_t byte  = my_data[i++];
      uint8_t index = ((my_crc >> 16) ^ byte) & 0xFF;
      my_crc        = (my_crc << 8) ^ CRC24A_TABLE[index];
    }
  }

  // Store partial CRCs and sizes in shared memory
  __shared__ uint32_t s_crcs[8];
  __shared__ int      s_sizes[8];
  s_crcs[tid]  = my_crc & 0xFFFFFF;
  s_sizes[tid] = my_bytes;
  __syncthreads();

  // Thread 0 combines all partial CRCs
  if (tid == 0) {
    // Compute suffix sums (bytes remaining after each thread's portion)
    int suffix_sums[8];
    suffix_sums[NUM_THREADS - 1] = 0;
    for (int i = NUM_THREADS - 2; i >= 0; i--) {
      suffix_sums[i] = suffix_sums[i + 1] + s_sizes[i + 1];
    }

    // Combine: final = XOR of shift(crc[i], suffix_sum[i]) for all i
    uint32_t final_crc = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
      if (s_sizes[i] > 0) {
        uint32_t shifted = crc24a_shift_bytes_fast(s_crcs[i], suffix_sums[i]);
        final_crc ^= shifted;
      }
    }

    *d_result = final_crc & 0xFFFFFF;
  }
}

/// \brief MASSIVELY PARALLEL CRC24A kernel - Phase 2: Combine chunk CRCs
///
/// Combines partial CRCs using: CRC(A||B) = shift(CRC(A), len(B)*8) XOR CRC(B)
/// Uses tree reduction for O(log N) depth.

__global__ void crc24a_combine_chunks_to_result_kernel(const uint32_t* __restrict__ d_chunk_crcs,
                                                       const int* __restrict__ d_chunk_sizes,
                                                       int num_chunks,
                                                       uint32_t* __restrict__ d_result)
{
  // One warp: lane i shifts its own chunk CRC by the byte count of all later chunks, then the
  // partials XOR-reduce. The GF(2) multiply chains run in parallel instead of serially per chunk.
  if (blockIdx.x != 0 || threadIdx.x >= 32 || num_chunks <= 0 || num_chunks > 32) {
    if (blockIdx.x == 0 && threadIdx.x == 0 && num_chunks > 32) {
      // Fallback for oversized chunk counts (not expected with the current chunk sizing).
      int suffix_sum = 0;
      for (int i = num_chunks - 1; i >= 1; --i) {
        suffix_sum += d_chunk_sizes[i];
      }
      uint32_t final_crc       = 0;
      int      remaining_bytes = suffix_sum;
      for (int i = 0; i < num_chunks; ++i) {
        final_crc ^= crc24a_shift_bytes_fast(d_chunk_crcs[i], remaining_bytes);
        if (i < num_chunks - 1) {
          remaining_bytes -= d_chunk_sizes[i + 1];
        }
      }
      *d_result = final_crc & 0xFFFFFF;
    }
    return;
  }

  const int lane    = threadIdx.x;
  uint32_t  partial = 0;
  if (lane < num_chunks) {
    int suffix_bytes = 0;
    for (int i = lane + 1; i < num_chunks; ++i) {
      suffix_bytes += d_chunk_sizes[i];
    }
    partial = crc24a_shift_bytes_fast(d_chunk_crcs[lane], suffix_bytes);
  }

#pragma unroll
  for (int offset = 16; offset > 0; offset >>= 1) {
    partial ^= __shfl_down_sync(0xFFFFFFFF, partial, offset);
  }

  if (lane == 0) {
    *d_result = partial & 0xFFFFFF;
  }
}

/// \brief Check CRC-24A using slicing-by-4 (returns 0 if CRC passes)
__global__ void check_crc24a_kernel(const uint8_t* __restrict__ d_data,
                                    int total_bytes, // Including CRC
                                    uint32_t* __restrict__ d_result)
{
  if (threadIdx.x != 0 || blockIdx.x != 0)
    return;

  uint32_t crc = 0;
  int      i   = 0;

  // Process 4 bytes at a time using slicing-by-4
  int num_words = total_bytes / 4;
  for (int w = 0; w < num_words; w++) {
    uint8_t b0 = d_data[i++];
    uint8_t b1 = d_data[i++];
    uint8_t b2 = d_data[i++];
    uint8_t b3 = d_data[i++];

    uint8_t idx0 = ((crc >> 16) ^ b0) & 0xFF;
    uint8_t idx1 = ((crc >> 8) ^ b1) & 0xFF;
    uint8_t idx2 = (crc ^ b2) & 0xFF;
    uint8_t idx3 = b3;

    crc = CRC24A_S0[idx0] ^ CRC24A_S1[idx1] ^ CRC24A_S2[idx2] ^ CRC24A_S3[idx3];
  }

  // Process remaining bytes
  while (i < total_bytes) {
    uint8_t byte  = d_data[i++];
    uint8_t index = ((crc >> 16) ^ byte) & 0xFF;
    crc           = (crc << 8) ^ CRC24A_TABLE[index];
  }

  // If CRC is correct, result should be 0
  *d_result = crc & 0xFFFFFF;
}

/// \brief Check CRC-24B using slicing-by-4 (returns 0 if CRC passes)
__global__ void check_crc24b_kernel(const uint8_t* __restrict__ d_data,
                                    int total_bytes, // Including CRC
                                    uint32_t* __restrict__ d_result)
{
  if (threadIdx.x != 0 || blockIdx.x != 0)
    return;

  uint32_t crc = 0;
  int      i   = 0;

  // Process 4 bytes at a time using slicing-by-4
  int num_words = total_bytes / 4;
  for (int w = 0; w < num_words; w++) {
    uint8_t b0 = d_data[i++];
    uint8_t b1 = d_data[i++];
    uint8_t b2 = d_data[i++];
    uint8_t b3 = d_data[i++];

    uint8_t idx0 = ((crc >> 16) ^ b0) & 0xFF;
    uint8_t idx1 = ((crc >> 8) ^ b1) & 0xFF;
    uint8_t idx2 = (crc ^ b2) & 0xFF;
    uint8_t idx3 = b3;

    crc = CRC24B_S0[idx0] ^ CRC24B_S1[idx1] ^ CRC24B_S2[idx2] ^ CRC24B_S3[idx3];
  }

  // Process remaining bytes
  while (i < total_bytes) {
    uint8_t byte  = d_data[i++];
    uint8_t index = ((crc >> 16) ^ byte) & 0xFF;
    crc           = (crc << 8) ^ CRC24B_TABLE[index];
  }

  // If CRC is correct, result should be 0
  *d_result = crc & 0xFFFFFF;
}

/// \brief Check CRC-16 using slicing-by-4 (returns 0 if CRC passes)
__global__ void check_crc16_kernel(const uint8_t* __restrict__ d_data,
                                   int total_bytes, // Including CRC
                                   uint16_t* __restrict__ d_result)
{
  if (threadIdx.x != 0 || blockIdx.x != 0)
    return;

  uint32_t crc = 0;
  int      i   = 0;

  // Process 4 bytes at a time using slicing-by-4
  int num_words = total_bytes / 4;
  for (int w = 0; w < num_words; w++) {
    uint8_t b0 = d_data[i++];
    uint8_t b1 = d_data[i++];
    uint8_t b2 = d_data[i++];
    uint8_t b3 = d_data[i++];

    uint8_t idx0 = ((crc >> 8) ^ b0) & 0xFF;
    uint8_t idx1 = (crc ^ b1) & 0xFF;
    uint8_t idx2 = b2;
    uint8_t idx3 = b3;

    crc = CRC16_S0[idx0] ^ CRC16_S1[idx1] ^ CRC16_S2[idx2] ^ CRC16_S3[idx3];
  }

  // Process remaining bytes
  while (i < total_bytes) {
    uint8_t byte  = d_data[i++];
    uint8_t index = ((crc >> 8) ^ byte) & 0xFF;
    crc           = (crc << 8) ^ CRC16_TABLE[index];
  }

  // If CRC is correct, result should be 0
  *d_result = (uint16_t)(crc & 0xFFFF);
}

// ============================================================================
// Batch parallel CRC-24A kernel
// ============================================================================

#define BATCH_CRC24A_CHUNKS 8
#define BATCH_CRC24A_WORKERS 4
#define BATCH_CRC24A_BLOCK_SIZE (BATCH_CRC24A_CHUNKS * BATCH_CRC24A_WORKERS)

__global__ void __launch_bounds__(BATCH_CRC24A_BLOCK_SIZE, 8) crc24a_batch_kernel(const uint8_t* __restrict__ d_tb_data,
                                                                                  uint32_t* __restrict__ d_crc_out,
                                                                                  int      tb_stride_bytes,
                                                                                  unsigned num_bits)
{
  const int tid        = threadIdx.x;
  const int tb_idx     = blockIdx.x;
  const int chunk_idx  = tid / BATCH_CRC24A_WORKERS;
  const int worker_idx = tid % BATCH_CRC24A_WORKERS;

  __shared__ uint32_t s_S0[256];
  __shared__ uint32_t s_S1[256];
  __shared__ uint32_t s_S2[256];
  __shared__ uint32_t s_S3[256];
  __shared__ uint32_t s_TABLE[256];

  for (int i = tid; i < 256; i += BATCH_CRC24A_BLOCK_SIZE) {
    s_S0[i]    = CRC24A_S0[i];
    s_S1[i]    = CRC24A_S1[i];
    s_S2[i]    = CRC24A_S2[i];
    s_S3[i]    = CRC24A_S3[i];
    s_TABLE[i] = CRC24A_TABLE[i];
  }
  __syncthreads();

  const uint8_t* tb             = d_tb_data + tb_idx * tb_stride_bytes;
  const int      num_full_bytes = num_bits / 8;
  const int      remaining_bits = num_bits % 8;

  const int chunk_size     = (num_full_bytes + BATCH_CRC24A_CHUNKS - 1) / BATCH_CRC24A_CHUNKS;
  const int chunk_start    = chunk_idx * chunk_size;
  const int chunk_end      = min(chunk_start + chunk_size, num_full_bytes);
  const int bytes_in_chunk = (chunk_start < num_full_bytes) ? (chunk_end - chunk_start) : 0;

  const int bytes_per_worker = (bytes_in_chunk + BATCH_CRC24A_WORKERS - 1) / BATCH_CRC24A_WORKERS;
  const int my_start         = chunk_start + worker_idx * bytes_per_worker;
  const int my_end           = min(my_start + bytes_per_worker, chunk_end);
  const int my_bytes         = (my_start < chunk_end) ? (my_end - my_start) : 0;

  uint32_t my_crc = 0;
  if (my_bytes > 0) {
    const uint8_t* my_data   = tb + my_start;
    int            i         = 0;
    const int      num_words = my_bytes / 4;

    for (int w = 0; w < num_words; ++w) {
      uint8_t b0 = my_data[i++];
      uint8_t b1 = my_data[i++];
      uint8_t b2 = my_data[i++];
      uint8_t b3 = my_data[i++];

      my_crc = s_S0[((my_crc >> 16) ^ b0) & 0xFF] ^ s_S1[((my_crc >> 8) ^ b1) & 0xFF] ^ s_S2[(my_crc ^ b2) & 0xFF] ^
               s_S3[b3];
    }

    while (i < my_bytes) {
      uint8_t byte = my_data[i++];
      my_crc       = ((my_crc << 8) ^ s_TABLE[((my_crc >> 16) ^ byte) & 0xFF]) & 0xFFFFFF;
    }
  }

  __shared__ uint32_t s_worker_crcs[BATCH_CRC24A_BLOCK_SIZE];
  __shared__ int      s_worker_sizes[BATCH_CRC24A_BLOCK_SIZE];
  __shared__ uint32_t s_chunk_crcs[BATCH_CRC24A_CHUNKS];
  __shared__ int      s_chunk_sizes[BATCH_CRC24A_CHUNKS];

  s_worker_crcs[tid]  = my_crc & 0xFFFFFF;
  s_worker_sizes[tid] = my_bytes;
  __syncthreads();

  if (worker_idx == 0) {
    const int base = chunk_idx * BATCH_CRC24A_WORKERS;
    int       suffix[BATCH_CRC24A_WORKERS];
    suffix[BATCH_CRC24A_WORKERS - 1] = 0;
    for (int i = BATCH_CRC24A_WORKERS - 2; i >= 0; --i) {
      suffix[i] = suffix[i + 1] + s_worker_sizes[base + i + 1];
    }

    uint32_t chunk_crc = 0;
    for (int i = 0; i < BATCH_CRC24A_WORKERS; ++i) {
      if (s_worker_sizes[base + i] > 0) {
        chunk_crc ^= crc24a_shift_bytes_fast(s_worker_crcs[base + i], suffix[i]);
      }
    }

    s_chunk_crcs[chunk_idx]  = chunk_crc & 0xFFFFFF;
    s_chunk_sizes[chunk_idx] = bytes_in_chunk;
  }
  __syncthreads();

  if (tid == 0) {
    int suffix[BATCH_CRC24A_CHUNKS];
    suffix[BATCH_CRC24A_CHUNKS - 1] = 0;
    for (int i = BATCH_CRC24A_CHUNKS - 2; i >= 0; --i) {
      suffix[i] = suffix[i + 1] + s_chunk_sizes[i + 1];
    }

    uint32_t crc = 0;
    for (int i = 0; i < BATCH_CRC24A_CHUNKS; ++i) {
      if (s_chunk_sizes[i] > 0) {
        crc ^= crc24a_shift_bytes_fast(s_chunk_crcs[i], suffix[i]);
      }
    }
    crc &= 0xFFFFFF;

    if (remaining_bits > 0) {
      uint8_t byte = tb[num_full_bytes];
      for (int b = 0; b < remaining_bits; ++b) {
        uint8_t  data_bit = (byte >> (7 - b)) & 1;
        uint32_t msb      = (crc >> 23) & 1;
        crc               = (crc << 1) | data_bit;
        if (msb) {
          crc ^= CRC24A_POLY;
        }
        crc &= 0xFFFFFF;
      }
      for (int b = 0; b < (8 - remaining_bits); ++b) {
        uint32_t msb = (crc >> 23) & 1;
        crc <<= 1;
        if (msb) {
          crc ^= CRC24A_POLY;
        }
        crc &= 0xFFFFFF;
      }
    }

    d_crc_out[tb_idx] = crc;
  }
}

// ============================================================================
// API Implementation
// ============================================================================

cuda_result crc24a_compute(uint32_t* d_crc, const uint8_t* d_data, unsigned num_bits, const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  if (!init_crc_tables())
    return make_unexpected(std::string("The CRC kernel reported a CUDA failure"));
  // Initialize slicing-by-4 tables for optimized compute
  if (!init_slicing_tables())
    return make_unexpected(std::string("The CRC kernel reported a CUDA failure"));

  int  num_bytes       = (num_bits + 7) / 8;
  bool is_byte_aligned = (num_bits % 8 == 0);

  // Threshold routing for optimal performance:
  // - Tiny TBs (≤256 bytes / 2048 bits): Simple single-thread kernel.
  // - Larger byte-aligned TBs: warp-reduction kernel.
  // NOTE: Non-byte-aligned TBs always use simple kernel for correctness.
  //
  // The older single-warp prefetch kernel is slower on current GPUs for
  // mid-size word-aligned PDSCH TBs because most CRC work is still serialized
  // through one lane. The warp-reduction kernel parallelizes the byte ranges
  // and is consistently faster for the latency-sensitive 5G PDSCH sizes.
  // Use simple kernel only for tiny byte-aligned TBs.
  const int SIMPLE_THRESHOLD = 256;
  // Non-byte-aligned TBs always use simple kernel (handles partial bytes correctly)
  if (num_bytes <= SIMPLE_THRESHOLD || !is_byte_aligned) {
    // Tiny data or non-byte-aligned TBs: simple single-thread kernel
    crc24a_simple_kernel<<<1, 1, 0, native_stream>>>(d_data, d_crc, num_bytes, num_bits);
  } else {
    // Byte-aligned data: use the PUSCH-style warp reduction shape.
    // Keep mid-size TBs at 128 threads; 100 MHz high-throughput TBs
    // have enough bytes to benefit from 256 threads in the same block.
    // This removes the separate chunk-combine launch and final D2D copy.
    // Initialize x^(8*2^i) powers for fast CRC combine
    init_x_powers_table();

    int block_size = (num_bytes >= CRC24A_WARP256_MIN_BYTES) ? 256 : 128;
    crc24a_warp_reduce_kernel<false><<<1, block_size, 0, native_stream>>>(d_data, d_crc, nullptr, num_bytes, 0);
  }

  return {};
}

cuda_result crc24a_compute_with_scratch(uint32_t*          d_crc,
                                        uint32_t*          d_chunk_crcs,
                                        int*               d_chunk_sizes,
                                        const uint8_t*     d_data,
                                        unsigned           num_bits,
                                        unsigned           max_chunks,
                                        unsigned           chunk_size_bytes,
                                        const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  if (!d_data || !d_crc || !d_chunk_crcs || !d_chunk_sizes || max_chunks <= 0 || chunk_size_bytes <= 0) {
    return make_unexpected(std::string("The CRC call carries an invalid configuration"));
  }

  if ((num_bits % 8) != 0) {
    return crc24a_compute(d_crc, d_data, num_bits, stream);
  }

  for (cuda_result status : {init_crc_tables(), init_slicing_tables(), init_x_powers_table()}) {
    if (!status.has_value()) {
      return status;
    }
  }

  int num_bytes  = num_bits / 8;
  int num_chunks = (num_bytes + chunk_size_bytes - 1) / chunk_size_bytes;
  if (num_chunks <= 1) {
    crc24a_warp_reduce_kernel<false><<<1, 128, 0, native_stream>>>(d_data, d_crc, nullptr, num_bytes, 0);
    return {};
  }
  if (num_chunks > max_chunks) {
    return make_unexpected(std::string("The CRC call carries an invalid configuration"));
  }

  crc24a_warp_reduce_kernel<true>
      <<<num_chunks, 256, 0, native_stream>>>(d_data, d_chunk_crcs, d_chunk_sizes, num_bytes, chunk_size_bytes);
  crc24a_combine_chunks_to_result_kernel<<<1, 32, 0, native_stream>>>(d_chunk_crcs, d_chunk_sizes, num_chunks, d_crc);

  return {};
}

cuda_result crc24a_compute_batch(uint32_t*          d_crc_out,
                                 const uint8_t*     d_tb_data,
                                 unsigned           num_tbs,
                                 unsigned           tb_size_bits,
                                 int                tb_stride_bytes,
                                 const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  if (!d_tb_data || !d_crc_out || num_tbs <= 0 || tb_size_bits <= 0 || tb_stride_bytes <= 0) {
    return make_unexpected(std::string("The CRC call carries an invalid configuration"));
  }

  for (cuda_result status : {init_crc_tables(), init_slicing_tables(), init_x_powers_table()}) {
    if (!status.has_value()) {
      return status;
    }
  }

  crc24a_batch_kernel<<<num_tbs, BATCH_CRC24A_BLOCK_SIZE, 0, native_stream>>>(
      d_tb_data, d_crc_out, tb_stride_bytes, tb_size_bits);

  if (cuda_result status = check_cuda_error(cudaPeekAtLastError(), "launch the checksum kernel"); !status.has_value()) {
    return status;
  }
  return {};
}

cuda_result crc24b_compute(uint32_t* d_crc, const uint8_t* d_data, unsigned num_bits, const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  for (cuda_result status : {init_crc_tables(), init_slicing_tables()}) {
    if (!status.has_value()) {
      return status;
    }
  }

  int num_bytes = (num_bits + 7) / 8;

  crc24b_kernel<<<1, 1, 0, native_stream>>>(d_data, d_crc, num_bytes);

  if (cuda_result status = check_cuda_error(cudaPeekAtLastError(), "launch the checksum kernel"); !status.has_value()) {
    return status;
  }
  return {};
}

cuda_result crc16_compute(uint16_t* d_crc, const uint8_t* d_data, unsigned num_bits, const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  for (cuda_result status : {init_crc_tables(), init_slicing_tables()}) {
    if (!status.has_value()) {
      return status;
    }
  }

  int num_bytes = (num_bits + 7) / 8;

  if ((num_bits % 8) == 0 && num_bytes > 128) {
    init_crc16_x_powers_table();
    crc16_warp_reduce_kernel<<<1, 32, 0, native_stream>>>(d_data, d_crc, num_bytes);
  } else {
    crc16_kernel<<<1, 1, 0, native_stream>>>(d_data, d_crc, num_bytes, num_bits);
  }

  if (cuda_result status = check_cuda_error(cudaPeekAtLastError(), "launch the checksum kernel"); !status.has_value()) {
    return status;
  }
  return {};
}

cuda_result crc24a_compute_and_attach(uint8_t* d_tb_with_crc, unsigned tb_size_bits, const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  for (cuda_result status : {init_crc_tables(), init_slicing_tables()}) {
    if (!status.has_value()) {
      return status;
    }
  }

  // Single kernel: compute CRC-24A + attach (saves 1 launch + 1 memset!)
  crc24a_compute_and_attach_kernel<<<1, 32, 0, native_stream>>>(d_tb_with_crc, tb_size_bits);

  if (cuda_result status = check_cuda_error(cudaPeekAtLastError(), "launch the checksum kernel"); !status.has_value()) {
    return status;
  }
  return {};
}

cuda_result crc16_compute_and_attach(uint8_t* d_tb_with_crc, unsigned tb_size_bits, const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  for (cuda_result status : {init_crc_tables(), init_slicing_tables()}) {
    if (!status.has_value()) {
      return status;
    }
  }

  // Single kernel: compute CRC-16 + attach
  crc16_compute_and_attach_kernel<<<1, 32, 0, native_stream>>>(d_tb_with_crc, tb_size_bits);

  if (cuda_result status = check_cuda_error(cudaPeekAtLastError(), "launch the checksum kernel"); !status.has_value()) {
    return status;
  }
  return {};
}

// Pre-allocated CRC check result buffers (avoid malloc/free in hot path!)
static uint32_t* s_d_crc_result           = nullptr;
static bool      s_crc_result_initialized = false;

static cuda_result init_crc_result_buffer()
{
  if (s_crc_result_initialized)
    return {};
  if (cuda_result status =
          check_cuda_error(cudaMalloc(&s_d_crc_result, sizeof(uint32_t)), "allocate device memory for the checksum");
      !status.has_value()) {
    return status;
  }
  s_crc_result_initialized = true;
  return {};
}

cuda_expected<bool> crc24a_check(const uint8_t* d_data, unsigned num_bits, const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  for (cuda_result status : {init_crc_tables(), init_slicing_tables(), init_crc_result_buffer()}) {
    if (!status.has_value()) {
      return make_unexpected(status.error());
    }
  }

  int total_bytes = (num_bits + 7) / 8;

  check_crc24a_kernel<<<1, 1, 0, native_stream>>>(d_data, total_bytes, s_d_crc_result);
  if (cuda_result status = check_cuda_error(cudaPeekAtLastError(), "launch the checksum kernel"); !status.has_value()) {
    return make_unexpected(status.error());
  }
  if (cuda_result status = check_cuda_error(cudaStreamSynchronize(native_stream), "wait for the checksum kernel");
      !status.has_value()) {
    return make_unexpected(status.error());
  }

  uint32_t result;
  if (cuda_result status = check_cuda_error(
          cudaMemcpy(&result, s_d_crc_result, sizeof(uint32_t), cudaMemcpyDeviceToHost), "copy the checksum");
      !status.has_value()) {
    return make_unexpected(status.error());
  }

  return result == 0;
}

cuda_expected<bool> crc24b_check(const uint8_t* d_data, unsigned num_bits, const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  for (cuda_result status : {init_crc_tables(), init_slicing_tables(), init_crc_result_buffer()}) {
    if (!status.has_value()) {
      return make_unexpected(status.error());
    }
  }

  int total_bytes = (num_bits + 7) / 8;

  check_crc24b_kernel<<<1, 1, 0, native_stream>>>(d_data, total_bytes, s_d_crc_result);
  if (cuda_result status = check_cuda_error(cudaPeekAtLastError(), "launch the checksum kernel"); !status.has_value()) {
    return make_unexpected(status.error());
  }
  if (cuda_result status = check_cuda_error(cudaStreamSynchronize(native_stream), "wait for the checksum kernel");
      !status.has_value()) {
    return make_unexpected(status.error());
  }

  uint32_t result;
  if (cuda_result status = check_cuda_error(
          cudaMemcpy(&result, s_d_crc_result, sizeof(uint32_t), cudaMemcpyDeviceToHost), "copy the checksum");
      !status.has_value()) {
    return make_unexpected(status.error());
  }

  return result == 0;
}

cuda_expected<bool> crc16_check(const uint8_t* d_data, unsigned num_bits, const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  for (cuda_result status : {init_crc_tables(), init_slicing_tables(), init_crc_result_buffer()}) {
    if (!status.has_value()) {
      return make_unexpected(status.error());
    }
  }

  int total_bytes = (num_bits + 7) / 8;

  uint16_t* d_result16 = reinterpret_cast<uint16_t*>(s_d_crc_result);

  check_crc16_kernel<<<1, 1, 0, native_stream>>>(d_data, total_bytes, d_result16);
  if (cuda_result status = check_cuda_error(cudaPeekAtLastError(), "launch the checksum kernel"); !status.has_value()) {
    return make_unexpected(status.error());
  }
  if (cuda_result status = check_cuda_error(cudaStreamSynchronize(native_stream), "wait for the checksum kernel");
      !status.has_value()) {
    return make_unexpected(status.error());
  }

  uint16_t result;
  if (cuda_result status = check_cuda_error(cudaMemcpy(&result, d_result16, sizeof(uint16_t), cudaMemcpyDeviceToHost),
                                            "copy the checksum");
      !status.has_value()) {
    return make_unexpected(status.error());
  }

  return result == 0;
}

// ============================================================================
// Batch CRC Check on Packed Decoder Output - Optimized Implementation
// ============================================================================
//
// Uses "Reflected CRC" algorithm to process LSB-first packed data directly
// without any bit reversal. This eliminates the expensive bit manipulation
// overhead of the standard approach.
//
// Key optimizations:
// 1. Reflected CRC polynomials - process LSB-first data natively
// 2. Word-at-a-time processing with slicing-by-4 tables
// 3. Direct uint32_t reads instead of byte extraction
// ============================================================================

// Reflected CRC polynomials (bit-reversed from standard polynomials)
// Standard CRC-24A: 0x864CFB -> Reflected: 0xDF3261
// Standard CRC-24B: 0x800063 -> Reflected: 0xC60001
// Standard CRC-16:  0x1021   -> Reflected: 0x8408

#define RCRC24A_POLY 0xDF3261
#define RCRC24B_POLY 0xC60001
#define RCRC16_POLY 0x8408

// Slicing-by-4 tables for word-at-a-time reflected CRC
// These allow processing 32 bits (4 bytes) per iteration
__device__ __constant__ uint32_t RCRC24A_T0[256]; ///< Byte 0 contribution
__device__ __constant__ uint32_t RCRC24A_T1[256]; ///< Byte 1 contribution
__device__ __constant__ uint32_t RCRC24A_T2[256]; ///< Byte 2 contribution
__device__ __constant__ uint32_t RCRC24A_T3[256]; ///< Byte 3 contribution

__device__ __constant__ uint32_t RCRC24B_T0[256];
__device__ __constant__ uint32_t RCRC24B_T1[256];
__device__ __constant__ uint32_t RCRC24B_T2[256];
__device__ __constant__ uint32_t RCRC24B_T3[256];
// Slicing-by-8 extension tables for CRC-24B (process 8 bytes per iteration)
__device__ __constant__ uint32_t RCRC24B_T4[256];
__device__ __constant__ uint32_t RCRC24B_T5[256];
__device__ __constant__ uint32_t RCRC24B_T6[256];
__device__ __constant__ uint32_t RCRC24B_T7[256];

__device__ __constant__ uint32_t RCRC16_T0[256];
__device__ __constant__ uint32_t RCRC16_T1[256];
__device__ __constant__ uint32_t RCRC16_T2[256];
__device__ __constant__ uint32_t RCRC16_T3[256];

// Host-side slicing tables
static uint32_t h_rcrc24a_t0[256], h_rcrc24a_t1[256], h_rcrc24a_t2[256], h_rcrc24a_t3[256];
static uint32_t h_rcrc24b_t0[256], h_rcrc24b_t1[256], h_rcrc24b_t2[256], h_rcrc24b_t3[256];
// Slicing-by-8 extension for CRC-24B
static uint32_t h_rcrc24b_t4[256], h_rcrc24b_t5[256], h_rcrc24b_t6[256], h_rcrc24b_t7[256];
static uint32_t h_rcrc16_t0[256], h_rcrc16_t1[256], h_rcrc16_t2[256], h_rcrc16_t3[256];
static bool     reflected_tables_initialized = false;

// ============================================================================
// Precomputed x^(8*2^i) powers for REFLECTED CRC-24B (parallel CRC combining)
// ============================================================================
// These enable O(log n) CRC shift operations for fast parallel combine.
// For reflected CRC, shift(crc, n_bytes) = crc * x^(n*8) mod G_reflected(x)
__device__ __constant__ uint32_t RCRC24B_X_POWERS[21];
static uint32_t                  h_rcrc24b_x_powers[21];
static bool                      rcrc24b_x_powers_initialized = false;

// Precomputed x^(8*2^i) powers for REFLECTED CRC-24A (parallel CRC combining)
__device__ __constant__ uint32_t RCRC24A_X_POWERS[21];
static uint32_t                  h_rcrc24a_x_powers[21];
static bool                      rcrc24a_x_powers_initialized = false;

// Host-side GF(2^24) multiplication for reflected polynomial (for table generation)
static uint32_t gf24_multiply_reflected_host(uint32_t a, uint32_t b, uint32_t poly)
{
  uint32_t result = 0;
  while (b) {
    if (b & 1)
      result ^= a;
    b >>= 1;
    // For reflected: shift right, XOR poly if LSB was set
    if (a & 1) {
      a = (a >> 1) ^ poly;
    } else {
      a >>= 1;
    }
  }
  return result & 0xFFFFFF;
}

// Initialize precomputed x^(8*2^i) powers for fast reflected CRC-24B shifting
static cuda_result init_rcrc24b_x_powers_table()
{
  if (rcrc24b_x_powers_initialized)
    return {};

  // For reflected CRC, x^8 is computed by shifting 1 through 8 zero bits (LSB first)
  uint32_t x_8 = 1;
  for (int i = 0; i < 8; i++) {
    if (x_8 & 1) {
      x_8 = (x_8 >> 1) ^ RCRC24B_POLY;
    } else {
      x_8 >>= 1;
    }
  }

  // Compute x^(8*2^i) by repeated squaring
  // x^8
  h_rcrc24b_x_powers[0] = x_8;
  for (int i = 1; i < 21; i++) {
    // x^(8*2^i) = (x^(8*2^(i-1)))^2
    h_rcrc24b_x_powers[i] =
        gf24_multiply_reflected_host(h_rcrc24b_x_powers[i - 1], h_rcrc24b_x_powers[i - 1], RCRC24B_POLY);
  }

  // Copy to device constant memory
  if (cuda_result status =
          check_cuda_error(cudaMemcpyToSymbol(RCRC24B_X_POWERS, h_rcrc24b_x_powers, sizeof(h_rcrc24b_x_powers)),
                           "upload the checksum tables");
      !status.has_value()) {
    return status;
  }

  rcrc24b_x_powers_initialized = true;
  // success
  return {};
}

// Initialize precomputed x^(8*2^i) powers for fast reflected CRC-24A shifting
static cuda_result init_rcrc24a_x_powers_table()
{
  if (rcrc24a_x_powers_initialized)
    return {};

  // For reflected CRC, x^8 is computed by shifting 1 through 8 zero bits (LSB first)
  uint32_t x_8 = 1;
  for (int i = 0; i < 8; i++) {
    if (x_8 & 1) {
      x_8 = (x_8 >> 1) ^ RCRC24A_POLY;
    } else {
      x_8 >>= 1;
    }
  }

  // Compute x^(8*2^i) by repeated squaring
  // x^8
  h_rcrc24a_x_powers[0] = x_8;
  for (int i = 1; i < 21; i++) {
    h_rcrc24a_x_powers[i] =
        gf24_multiply_reflected_host(h_rcrc24a_x_powers[i - 1], h_rcrc24a_x_powers[i - 1], RCRC24A_POLY);
  }

  // Copy to device constant memory
  if (cuda_result status =
          check_cuda_error(cudaMemcpyToSymbol(RCRC24A_X_POWERS, h_rcrc24a_x_powers, sizeof(h_rcrc24a_x_powers)),
                           "upload the checksum tables");
      !status.has_value()) {
    return status;
  }

  rcrc24a_x_powers_initialized = true;
  // success
  return {};
}

/// \brief Generate reflected CRC-24 slicing-by-4 tables
///
/// For reflected CRC: crc = (crc >> 8) ^ T[(crc ^ byte) & 0xFF]
/// Slicing-by-4 extends this to process 4 bytes at once.
static void generate_rcrc24_slicing_tables(uint32_t poly, uint32_t* t0, uint32_t* t1, uint32_t* t2, uint32_t* t3)
{
  // Generate base table (T0) - reflected CRC of single byte
  for (int i = 0; i < 256; i++) {
    uint32_t crc = i;
    for (int j = 0; j < 8; j++) {
      if (crc & 1) {
        crc = (crc >> 1) ^ poly;
      } else {
        crc >>= 1;
      }
    }
    t0[i] = crc & 0xFFFFFF;
  }

  // Generate T1, T2, T3 from T0 using the relation:
  // T[k+1][i] = (T[k][i] >> 8) ^ T0[T[k][i] & 0xFF]
  for (int i = 0; i < 256; i++) {
    t1[i] = (t0[i] >> 8) ^ t0[t0[i] & 0xFF];
    t1[i] &= 0xFFFFFF;
  }
  for (int i = 0; i < 256; i++) {
    t2[i] = (t1[i] >> 8) ^ t0[t1[i] & 0xFF];
    t2[i] &= 0xFFFFFF;
  }
  for (int i = 0; i < 256; i++) {
    t3[i] = (t2[i] >> 8) ^ t0[t2[i] & 0xFF];
    t3[i] &= 0xFFFFFF;
  }
}

/// \brief Generate reflected CRC-16 slicing-by-4 tables
static void generate_rcrc16_slicing_tables(uint32_t poly, uint32_t* t0, uint32_t* t1, uint32_t* t2, uint32_t* t3)
{
  // Generate base table (T0)
  for (int i = 0; i < 256; i++) {
    uint32_t crc = i;
    for (int j = 0; j < 8; j++) {
      if (crc & 1) {
        crc = (crc >> 1) ^ poly;
      } else {
        crc >>= 1;
      }
    }
    t0[i] = crc & 0xFFFF;
  }

  // Generate T1, T2, T3
  for (int i = 0; i < 256; i++) {
    t1[i] = (t0[i] >> 8) ^ t0[t0[i] & 0xFF];
    t1[i] &= 0xFFFF;
  }
  for (int i = 0; i < 256; i++) {
    t2[i] = (t1[i] >> 8) ^ t0[t1[i] & 0xFF];
    t2[i] &= 0xFFFF;
  }
  for (int i = 0; i < 256; i++) {
    t3[i] = (t2[i] >> 8) ^ t0[t2[i] & 0xFF];
    t3[i] &= 0xFFFF;
  }
}

static cuda_result init_reflected_crc_tables()
{
  if (reflected_tables_initialized)
    return {};

  // Generate slicing-by-4 tables for all CRC types
  generate_rcrc24_slicing_tables(RCRC24A_POLY, h_rcrc24a_t0, h_rcrc24a_t1, h_rcrc24a_t2, h_rcrc24a_t3);
  generate_rcrc24_slicing_tables(RCRC24B_POLY, h_rcrc24b_t0, h_rcrc24b_t1, h_rcrc24b_t2, h_rcrc24b_t3);
  generate_rcrc16_slicing_tables(RCRC16_POLY, h_rcrc16_t0, h_rcrc16_t1, h_rcrc16_t2, h_rcrc16_t3);

  // Generate slicing-by-8 extension tables for CRC-24B (T4-T7)
  // Continue the pattern: T[k+1][i] = (T[k][i] >> 8) ^ T0[T[k][i] & 0xFF]
  for (int i = 0; i < 256; i++) {
    h_rcrc24b_t4[i] = (h_rcrc24b_t3[i] >> 8) ^ h_rcrc24b_t0[h_rcrc24b_t3[i] & 0xFF];
    h_rcrc24b_t4[i] &= 0xFFFFFF;
  }
  for (int i = 0; i < 256; i++) {
    h_rcrc24b_t5[i] = (h_rcrc24b_t4[i] >> 8) ^ h_rcrc24b_t0[h_rcrc24b_t4[i] & 0xFF];
    h_rcrc24b_t5[i] &= 0xFFFFFF;
  }
  for (int i = 0; i < 256; i++) {
    h_rcrc24b_t6[i] = (h_rcrc24b_t5[i] >> 8) ^ h_rcrc24b_t0[h_rcrc24b_t5[i] & 0xFF];
    h_rcrc24b_t6[i] &= 0xFFFFFF;
  }
  for (int i = 0; i < 256; i++) {
    h_rcrc24b_t7[i] = (h_rcrc24b_t6[i] >> 8) ^ h_rcrc24b_t0[h_rcrc24b_t6[i] & 0xFF];
    h_rcrc24b_t7[i] &= 0xFFFFFF;
  }

  // Copy to device constant memory
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC24A_T0, h_rcrc24a_t0, sizeof(h_rcrc24a_t0)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC24A_T1, h_rcrc24a_t1, sizeof(h_rcrc24a_t1)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC24A_T2, h_rcrc24a_t2, sizeof(h_rcrc24a_t2)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC24A_T3, h_rcrc24a_t3, sizeof(h_rcrc24a_t3)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }

  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC24B_T0, h_rcrc24b_t0, sizeof(h_rcrc24b_t0)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC24B_T1, h_rcrc24b_t1, sizeof(h_rcrc24b_t1)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC24B_T2, h_rcrc24b_t2, sizeof(h_rcrc24b_t2)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC24B_T3, h_rcrc24b_t3, sizeof(h_rcrc24b_t3)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC24B_T4, h_rcrc24b_t4, sizeof(h_rcrc24b_t4)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC24B_T5, h_rcrc24b_t5, sizeof(h_rcrc24b_t5)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC24B_T6, h_rcrc24b_t6, sizeof(h_rcrc24b_t6)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC24B_T7, h_rcrc24b_t7, sizeof(h_rcrc24b_t7)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }

  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC16_T0, h_rcrc16_t0, sizeof(h_rcrc16_t0)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC16_T1, h_rcrc16_t1, sizeof(h_rcrc16_t1)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC16_T2, h_rcrc16_t2, sizeof(h_rcrc16_t2)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }
  if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(RCRC16_T3, h_rcrc16_t3, sizeof(h_rcrc16_t3)),
                                            "upload the checksum tables");
      !status.has_value()) {
    return status;
  }

  reflected_tables_initialized = true;
  // success
  return {};
}

cuda_result crc_init_tables()
{
  // The byte-at-a-time tables, the slicing tables each direction reads, and the powers of x that
  // let a batch check fold several code blocks at once.
  for (cuda_result status : {init_crc_tables(),
                             init_slicing_tables(),
                             init_reflected_crc_tables(),
                             init_x_powers_table(),
                             init_rcrc24a_x_powers_table(),
                             init_rcrc24b_x_powers_table()}) {
    if (!status.has_value()) {
      return status;
    }
  }

  return {};
}

/// \brief Compute reflected CRC-24 using slicing-by-4
///
/// Processes 32 bits (one uint32_t word) per iteration.
/// No bit reversal needed - works directly on LSB-first packed decoder output.
///
/// \param data Pointer to packed uint32_t data (LSB-first bit ordering)
/// \param num_words Number of complete uint32_t words
/// \param remaining_bits Remaining bits after complete words (0-31)
/// \param T0,T1,T2,T3 Slicing tables for the CRC polynomial
/// \return CRC value (24 bits for CRC-24, 16 bits for CRC-16)
__device__ inline uint32_t compute_rcrc24_sliced(const uint32_t* data,
                                                 int             num_words,
                                                 int             remaining_bits,
                                                 const uint32_t* T0,
                                                 const uint32_t* T1,
                                                 const uint32_t* T2,
                                                 const uint32_t* T3)
{
  uint32_t crc = 0;

  // Process complete 32-bit words using slicing-by-4
  // Each iteration: crc ^= word, then apply 4-byte slicing
  for (int i = 0; i < num_words; i++) {
    uint32_t word = data[i];
    crc ^= word;

    // Slicing-by-4: process all 4 bytes in parallel via table lookups
    crc = T3[(crc >> 0) & 0xFF] ^ T2[(crc >> 8) & 0xFF] ^ T1[(crc >> 16) & 0xFF] ^ T0[(crc >> 24) & 0xFF];
  }

  // Process remaining bits (if any) byte-at-a-time
  if (remaining_bits > 0) {
    uint32_t last_word       = data[num_words];
    int      remaining_bytes = (remaining_bits + 7) / 8;

    for (int b = 0; b < remaining_bytes; b++) {
      uint8_t byte = (last_word >> (b * 8)) & 0xFF;
      crc          = (crc >> 8) ^ T0[(crc ^ byte) & 0xFF];
    }
  }

  return crc & 0xFFFFFF;
}

/// \brief Compute reflected CRC-16 using slicing-by-4
__device__ inline uint32_t compute_rcrc16_sliced(const uint32_t* data,
                                                 int             num_words,
                                                 int             remaining_bits,
                                                 const uint32_t* T0,
                                                 const uint32_t* T1,
                                                 const uint32_t* T2,
                                                 const uint32_t* T3)
{
  uint32_t crc = 0;

  for (int i = 0; i < num_words; i++) {
    uint32_t word = data[i];
    crc ^= word;

    crc = T3[(crc >> 0) & 0xFF] ^ T2[(crc >> 8) & 0xFF] ^ T1[(crc >> 16) & 0xFF] ^ T0[(crc >> 24) & 0xFF];
  }

  if (remaining_bits > 0) {
    uint32_t last_word       = data[num_words];
    int      remaining_bytes = (remaining_bits + 7) / 8;

    for (int b = 0; b < remaining_bytes; b++) {
      uint8_t byte = (last_word >> (b * 8)) & 0xFF;
      crc          = (crc >> 8) ^ T0[(crc ^ byte) & 0xFF];
    }
  }

  return crc & 0xFFFF;
}

/// \brief GF(2^24) multiplication for reflected CRC-24B polynomial (device version)
///
/// Computes (a * b) mod G_reflected(x) using Russian peasant multiplication.
/// Fully unrolled for GPU performance.
__device__ __forceinline__ uint32_t gf24_multiply_rcrc24b(uint32_t a, uint32_t b)
{
  uint32_t result = 0;

#pragma unroll
  for (int i = 0; i < 24; i++) {
    result ^= (b & 1) ? a : 0;
    b >>= 1;
    // For reflected: shift right, XOR poly if LSB was set
    uint32_t lsb = a & 1;
    a >>= 1;
    a ^= lsb ? RCRC24B_POLY : 0;
  }

  return result & 0xFFFFFF;
}

/// \brief Fast CRC-24B shift by n bytes using precomputed x^(8*2^i) powers (reflected)
///
/// shift(crc, n*8 bits) = crc * x^(n*8) mod G_reflected(x)
/// O(log n) multiplications instead of O(n) bit shifts!
__device__ __forceinline__ uint32_t rcrc24b_shift_bytes_fast(uint32_t crc, int n_bytes)
{
  if (n_bytes == 0 || crc == 0)
    return crc;

  // Compute x^(n*8) mod G(x) using precomputed powers
  uint32_t x_power = 0;
  bool     first   = true;

  for (int i = 0; i < 21 && n_bytes > 0; i++) {
    if (n_bytes & 1) {
      if (first) {
        x_power = RCRC24B_X_POWERS[i];
        first   = false;
      } else {
        x_power = gf24_multiply_rcrc24b(x_power, RCRC24B_X_POWERS[i]);
      }
    }
    n_bytes >>= 1;
  }

  // Multiply crc by x_power
  // n_bytes was 0
  if (first)
    return crc;
  return gf24_multiply_rcrc24b(crc, x_power);
}

/// \brief GF(2^24) multiplication for reflected CRC-24A polynomial (device version)
__device__ __forceinline__ uint32_t gf24_multiply_rcrc24a(uint32_t a, uint32_t b)
{
  uint32_t result = 0;

#pragma unroll
  for (int i = 0; i < 24; i++) {
    result ^= (b & 1) ? a : 0;
    b >>= 1;
    uint32_t lsb = a & 1;
    a >>= 1;
    a ^= lsb ? RCRC24A_POLY : 0;
  }

  return result & 0xFFFFFF;
}

/// \brief Fast CRC-24A shift by n bytes using precomputed x^(8*2^i) powers (reflected)
__device__ __forceinline__ uint32_t rcrc24a_shift_bytes_fast(uint32_t crc, int n_bytes)
{
  if (n_bytes == 0 || crc == 0)
    return crc;

  uint32_t x_power = 0;
  bool     first   = true;

  for (int i = 0; i < 21 && n_bytes > 0; i++) {
    if (n_bytes & 1) {
      if (first) {
        x_power = RCRC24A_X_POWERS[i];
        first   = false;
      } else {
        x_power = gf24_multiply_rcrc24a(x_power, RCRC24A_X_POWERS[i]);
      }
    }
    n_bytes >>= 1;
  }

  if (first)
    return crc;
  return gf24_multiply_rcrc24a(crc, x_power);
}

/// \brief FAST PARALLEL Batch CRC-24B check kernel (non-uniform bits version)
///
/// Uses 8 threads per codeblock for intra-CB parallelism.
/// Each codeblock can have a different bit count (read from d_bits_per_cb array).
__global__ void __launch_bounds__(8, 32)
    crc24b_check_batch_packed_parallel_nonuniform_kernel(const uint32_t* __restrict__ d_packed_data,
                                                         unsigned words_per_cb,
                                                         const int* __restrict__ d_bits_per_cb,
                                                         unsigned num_cbs,
                                                         int* __restrict__ d_results)
{
  const int NUM_WORKERS = 8;
  int       cb_idx      = blockIdx.x;
  int       tid         = threadIdx.x;

  if (cb_idx >= num_cbs)
    return;
  if (tid >= NUM_WORKERS)
    return;

  const uint32_t* cb_data = d_packed_data + cb_idx * words_per_cb;
  // Read per-CB bit count
  unsigned bits_per_cb        = d_bits_per_cb[cb_idx];
  int      num_complete_words = bits_per_cb / 32;
  int      remaining_bits     = bits_per_cb % 32;

  // Divide words among threads
  int words_per_worker = (num_complete_words + NUM_WORKERS - 1) / NUM_WORKERS;
  int my_start_word    = tid * words_per_worker;
  int my_end_word      = min(my_start_word + words_per_worker, num_complete_words);
  int my_words         = (my_start_word < num_complete_words) ? (my_end_word - my_start_word) : 0;

  // Each thread computes CRC for its portion using slicing-by-4
  uint32_t my_crc = 0;
  if (my_words > 0) {
    const uint32_t* my_data = cb_data + my_start_word;

    for (int i = 0; i < my_words; i++) {
      uint32_t word = my_data[i];
      my_crc ^= word;

      // Slicing-by-4
      my_crc = RCRC24B_T3[(my_crc >> 0) & 0xFF] ^ RCRC24B_T2[(my_crc >> 8) & 0xFF] ^ RCRC24B_T1[(my_crc >> 16) & 0xFF] ^
               RCRC24B_T0[(my_crc >> 24) & 0xFF];
    }
  }

  // Handle remaining bits (only last worker handles this)
  if (tid == NUM_WORKERS - 1 && remaining_bits > 0 && my_end_word == num_complete_words) {
    uint32_t last_word       = cb_data[num_complete_words];
    int      remaining_bytes = (remaining_bits + 7) / 8;

    for (int b = 0; b < remaining_bytes; b++) {
      uint8_t byte = (last_word >> (b * 8)) & 0xFF;
      my_crc       = (my_crc >> 8) ^ RCRC24B_T0[(my_crc ^ byte) & 0xFF];
    }
  }

  // Store partial CRCs and byte counts in shared memory
  __shared__ uint32_t s_crcs[8];
  __shared__ int      s_bytes[8];

  int my_bytes = my_words * 4;
  if (tid == NUM_WORKERS - 1 && remaining_bits > 0) {
    my_bytes += (remaining_bits + 7) / 8;
  }

  s_crcs[tid]  = my_crc & 0xFFFFFF;
  s_bytes[tid] = my_bytes;
  __syncthreads();

  // Thread 0 combines all partial CRCs
  if (tid == 0) {
    // Compute suffix sums (bytes remaining after each thread's portion)
    int suffix_sums[8];
    suffix_sums[NUM_WORKERS - 1] = 0;
    for (int i = NUM_WORKERS - 2; i >= 0; i--) {
      suffix_sums[i] = suffix_sums[i + 1] + s_bytes[i + 1];
    }

    // Combine: final = XOR of shift(crc[i], suffix_sum[i]) for all i
    uint32_t final_crc = 0;
    for (int i = 0; i < NUM_WORKERS; i++) {
      if (s_bytes[i] > 0) {
        uint32_t shifted = rcrc24b_shift_bytes_fast(s_crcs[i], suffix_sums[i]);
        final_crc ^= shifted;
      }
    }

    // CRC passes if result is 0
    d_results[cb_idx] = (final_crc == 0) ? 1 : 0;
  }
}

/// \brief Batch CRC-24B check kernel with uniform bit count (optimized path)
__global__ void crc24b_check_batch_packed_uniform_kernel(const uint32_t* __restrict__ d_packed_data,
                                                         unsigned words_per_cb,
                                                         unsigned bits_per_cb,
                                                         unsigned num_cbs,
                                                         int* __restrict__ d_results)
{
  int cb_idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (cb_idx >= num_cbs)
    return;

  const uint32_t* cb_data            = d_packed_data + cb_idx * words_per_cb;
  int             num_complete_words = bits_per_cb / 32;
  int             remaining_bits     = bits_per_cb % 32;

  uint32_t crc = compute_rcrc24_sliced(
      cb_data, num_complete_words, remaining_bits, RCRC24B_T0, RCRC24B_T1, RCRC24B_T2, RCRC24B_T3);

  d_results[cb_idx] = (crc == 0) ? 1 : 0;
}

/// \brief Batch CRC-24A check kernel using optimized reflected slicing-by-4
__global__ void crc24a_check_batch_packed_kernel(const uint32_t* __restrict__ d_packed_data,
                                                 unsigned words_per_cb,
                                                 const int* __restrict__ d_bits_per_cb,
                                                 unsigned num_cbs,
                                                 int* __restrict__ d_results)
{
  int cb_idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (cb_idx >= num_cbs)
    return;

  const uint32_t* cb_data            = d_packed_data + cb_idx * words_per_cb;
  unsigned        num_bits           = d_bits_per_cb[cb_idx];
  int             num_complete_words = num_bits / 32;
  int             remaining_bits     = num_bits % 32;

  uint32_t crc = compute_rcrc24_sliced(
      cb_data, num_complete_words, remaining_bits, RCRC24A_T0, RCRC24A_T1, RCRC24A_T2, RCRC24A_T3);

  d_results[cb_idx] = (crc == 0) ? 1 : 0;
}

/// \brief Batch CRC-24A check kernel with uniform bit count (serial, 1 thread per CB)
__global__ void crc24a_check_batch_packed_uniform_kernel(const uint32_t* __restrict__ d_packed_data,
                                                         unsigned words_per_cb,
                                                         unsigned bits_per_cb,
                                                         unsigned num_cbs,
                                                         int* __restrict__ d_results)
{
  int cb_idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (cb_idx >= num_cbs)
    return;

  const uint32_t* cb_data            = d_packed_data + cb_idx * words_per_cb;
  int             num_complete_words = bits_per_cb / 32;
  int             remaining_bits     = bits_per_cb % 32;

  uint32_t crc = compute_rcrc24_sliced(
      cb_data, num_complete_words, remaining_bits, RCRC24A_T0, RCRC24A_T1, RCRC24A_T2, RCRC24A_T3);

  d_results[cb_idx] = (crc == 0) ? 1 : 0;
}

/// \brief Batch CRC-16 check kernel using optimized reflected slicing-by-4
__global__ void crc16_check_batch_packed_kernel(const uint32_t* __restrict__ d_packed_data,
                                                unsigned words_per_cb,
                                                const int* __restrict__ d_bits_per_cb,
                                                unsigned num_cbs,
                                                int* __restrict__ d_results)
{
  int cb_idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (cb_idx >= num_cbs)
    return;

  const uint32_t* cb_data            = d_packed_data + cb_idx * words_per_cb;
  unsigned        num_bits           = d_bits_per_cb[cb_idx];
  int             num_complete_words = num_bits / 32;
  int             remaining_bits     = num_bits % 32;

  uint32_t crc =
      compute_rcrc16_sliced(cb_data, num_complete_words, remaining_bits, RCRC16_T0, RCRC16_T1, RCRC16_T2, RCRC16_T3);

  d_results[cb_idx] = (crc == 0) ? 1 : 0;
}

/// \brief Batch CRC-16 check kernel with uniform bit count
__global__ void crc16_check_batch_packed_uniform_kernel(const uint32_t* __restrict__ d_packed_data,
                                                        unsigned words_per_cb,
                                                        unsigned bits_per_cb,
                                                        unsigned num_cbs,
                                                        int* __restrict__ d_results)
{
  int cb_idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (cb_idx >= num_cbs)
    return;

  const uint32_t* cb_data            = d_packed_data + cb_idx * words_per_cb;
  int             num_complete_words = bits_per_cb / 32;
  int             remaining_bits     = bits_per_cb % 32;

  uint32_t crc =
      compute_rcrc16_sliced(cb_data, num_complete_words, remaining_bits, RCRC16_T0, RCRC16_T1, RCRC16_T2, RCRC16_T3);

  d_results[cb_idx] = (crc == 0) ? 1 : 0;
}

cuda_result crc_check_batch_packed(int*               d_results,
                                   const uint32_t*    d_packed_data,
                                   unsigned           words_per_cb,
                                   const int*         d_bits_per_cb,
                                   unsigned           num_cbs,
                                   crc_type           type,
                                   const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  if (!init_reflected_crc_tables())
    return make_unexpected(std::string("The CRC kernel reported a CUDA failure"));

  switch (type) {
    case crc_type::crc24a: {
      int threads_per_block = 64;
      int num_blocks        = (num_cbs + threads_per_block - 1) / threads_per_block;
      crc24a_check_batch_packed_kernel<<<num_blocks, threads_per_block, 0, native_stream>>>(
          d_packed_data, words_per_cb, d_bits_per_cb, num_cbs, d_results);
      break;
    }
    case crc_type::crc24b: {
      // Use parallel kernel with 8 threads per codeblock for intra-CB parallelism
      init_rcrc24b_x_powers_table();
      crc24b_check_batch_packed_parallel_nonuniform_kernel<<<num_cbs, 8, 0, native_stream>>>(
          d_packed_data, words_per_cb, d_bits_per_cb, num_cbs, d_results);
      break;
    }
    case crc_type::crc16: {
      int threads_per_block = 64;
      int num_blocks        = (num_cbs + threads_per_block - 1) / threads_per_block;
      crc16_check_batch_packed_kernel<<<num_blocks, threads_per_block, 0, native_stream>>>(
          d_packed_data, words_per_cb, d_bits_per_cb, num_cbs, d_results);
      break;
    }
    default:
      return make_unexpected(std::string("The CRC call carries an invalid configuration"));
  }

  if (cuda_result status = check_cuda_error(cudaPeekAtLastError(), "launch the checksum kernel"); !status.has_value()) {
    return status;
  }
  return {};
}

cuda_result crc_check_batch_packed_uniform(int*               d_results,
                                           const uint32_t*    d_packed_data,
                                           unsigned           words_per_cb,
                                           unsigned           bits_per_cb,
                                           unsigned           num_cbs,
                                           crc_type           type,
                                           const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  if (!init_reflected_crc_tables())
    return make_unexpected(std::string("The CRC kernel reported a CUDA failure"));

  switch (type) {
    case crc_type::crc24a: {
      // Use serial kernel - the parallel kernel has a combining bug
      int threads_per_block = 64;
      int num_blocks        = (num_cbs + threads_per_block - 1) / threads_per_block;
      crc24a_check_batch_packed_uniform_kernel<<<num_blocks, threads_per_block, 0, native_stream>>>(
          d_packed_data, words_per_cb, bits_per_cb, num_cbs, d_results);
      break;
    }
    case crc_type::crc24b: {
      // Use serial kernel - the parallel kernel has a combining bug
      int threads_per_block = 64;
      int num_blocks        = (num_cbs + threads_per_block - 1) / threads_per_block;
      crc24b_check_batch_packed_uniform_kernel<<<num_blocks, threads_per_block, 0, native_stream>>>(
          d_packed_data, words_per_cb, bits_per_cb, num_cbs, d_results);
      break;
    }
    case crc_type::crc16: {
      int threads_per_block = 64;
      int num_blocks        = (num_cbs + threads_per_block - 1) / threads_per_block;
      crc16_check_batch_packed_uniform_kernel<<<num_blocks, threads_per_block, 0, native_stream>>>(
          d_packed_data, words_per_cb, bits_per_cb, num_cbs, d_results);
      break;
    }
    default:
      return make_unexpected(std::string("The CRC call carries an invalid configuration"));
  }

  if (cuda_result status = check_cuda_error(cudaPeekAtLastError(), "launch the checksum kernel"); !status.has_value()) {
    return status;
  }
  return {};
}

} // namespace cuda
} // namespace ocudu
