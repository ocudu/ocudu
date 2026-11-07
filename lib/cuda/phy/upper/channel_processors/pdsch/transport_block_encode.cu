// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Transport block encoding kernels.

#include "ocudu/cuda/adt/cuda_copy.h"
#include "ocudu/cuda/adt/cuda_error.h"
#include "ocudu/cuda/adt/cuda_event.h"
#include "ocudu/cuda/adt/device_vector.h"
#include "ocudu/cuda/phy/upper/channel_coding/crc.h"
#include "ocudu/cuda/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include "ocudu/cuda/phy/upper/channel_coding/ldpc/ldpc_rate_matching.h"
#include "ocudu/cuda/phy/upper/channel_processors/pdsch/transport_block_encode.h"
#include "ocudu/cuda/phy/upper/sequence_generators/scrambling.h"
#include <cstdio>
#include <cstring>
#include <cuda_fp16.h>
#include <memory>
#include <vector>

namespace ocudu {
namespace cuda {

// Shapes of the two base graphs, per TS 38.212, Section 5.3.2. The LDPC encoder carries the same
// numbers for its own kernels; they are repeated here rather than shared so that neither file
// reaches into the other's translation unit.
#define NR_LDPC_BG1_INFO_NODES 22
#define NR_LDPC_BG1_PARITY_NODES 46
#define NR_LDPC_BG1_UNPUNCTURED_VARS 66
#define NR_LDPC_BG2_INFO_NODES 10
#define NR_LDPC_BG2_PARITY_NODES 42
#define NR_LDPC_BG2_UNPUNCTURED_VARS 50

// Largest code block the standard defines, and the two checksum lengths it attaches.
#define NR_LDPC_MAX_CB_INFO_BITS 8448
#define NR_MAX_TBS_BITS 1277992
#define NR_TB_CRC24A_BITS 24
#define NR_CB_CRC24B_BITS 24
#define NR_TB_CRC16_BITS 16
#define NR_LDPC_MAX_CB_INFO_BITS_BG2 3840
#define NR_LDPC_MAX_LIFTING_SIZE 384

// Lifting sizes the standard defines, in increasing order, per TS 38.212, Table 5.3.2-1.
#define NR_LDPC_NUM_LIFTING_SIZES 51
static const int NR_LDPC_LIFTING_SIZES[NR_LDPC_NUM_LIFTING_SIZES] = {
    2,  3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  16,  18,  20,
    22, 24,  26,  28,  30,  32,  36,  40,  44,  48,  52,  56,  60,  64,  72,  80,  88,
    96, 104, 112, 120, 128, 144, 160, 176, 192, 208, 224, 240, 256, 288, 320, 352, 384};

// Shape of the code the transport block segments into, derived once per configuration.
struct nr_ldpc_config_t {
  int  base_graph;
  int  lifting_size;
  int  lifting_set_index;
  int  num_info_bits;
  int  num_parity_bits;
  int  num_filler_bits;
  int  num_codeword_bits;
  bool puncture;
  int  max_parity_nodes;
  int  redundancy_version;
};

// Shape of the transport block itself, as per TS 38.212, Section 5.2.2.
struct nr_tb_config_t {
  int              tb_size_bits;
  int              num_code_blocks;
  int              cb_size_bits;
  int              filler_bits;
  int              tb_crc_bits;
  int              cb_crc_bits;
  nr_ldpc_config_t ldpc_cfg;
};

// The shape a caller asks the encoder for, in the units the kernels work in.
struct tb_encoder_config_t {
  int      tb_size_bits;
  int      num_layers;
  int      modulation_order;
  int      num_allocated_res;
  int      redundancy_version;
  float    code_rate;
  uint16_t n_RNTI;
  uint16_t n_ID;
  uint8_t  q;
  bool     enable_scrambling;
  int      base_graph;
  int      num_code_blocks;
  int      lifting_size;
  int      nof_filler_bits;
  int      nof_short_segments;
  int      E_short;
  int      E_long;
  int      N_ref;
};

// Mapping from Z to set index (iLS)
static int get_lifting_set_index(int Z)
{
  // Set 0: Z = 2, 4, 8, 16, 32, 64, 128, 256
  // Set 1: Z = 3, 6, 12, 24, 48, 96, 192, 384
  // Set 2: Z = 5, 10, 20, 40, 80, 160, 320
  // Set 3: Z = 7, 14, 28, 56, 112, 224
  // Set 4: Z = 9, 18, 36, 72, 144, 288
  // Set 5: Z = 11, 22, 44, 88, 176, 352
  // Set 6: Z = 13, 26, 52, 104, 208
  // Set 7: Z = 15, 30, 60, 120, 240

  static const int set_bases[8] = {2, 3, 5, 7, 9, 11, 13, 15};

  for (int i = 0; i < 8; i++) {
    int base = set_bases[i];
    if (Z % base == 0) {
      int ratio = Z / base;
      // Check if ratio is power of 2
      if (ratio > 0 && (ratio & (ratio - 1)) == 0) {
        return i;
      }
    }
  }
  // Invalid Z
  return -1;
}

// ============================================================================
// Transport Block and LDPC Geometry
// ============================================================================

static int nr_ldpc_get_Kb(int tbs, int bg)
{
  if (bg == 1) {
    // 22
    return NR_LDPC_BG1_INFO_NODES;
  } else {
    // BG2: Per 3GPP TS 38.212, K_b is always 10 for BG2
    // The old implementation had incorrect TBS-dependent logic that caused
    // wrong lifting sizes for TBS values like 280 (would select Z=40 instead of Z=30)
    // 10
    return NR_LDPC_BG2_INFO_NODES;
  }
}

static int nr_ldpc_select_base_graph(int tbs, float rate)
{
  // Per TS 38.212 Section 7.2.2
  // BG2 is selected if any of these conditions are met:
  //   - A ≤ 292
  //   - A ≤ 3824 AND R ≤ 0.67
  //   - R ≤ 0.25
  // Note: Large TBS can still use BG2 with multiple code blocks!
  // Note: Use small epsilon for rate comparison to handle float precision
  //       (TBS is derived from target rate, so TBS/G may slightly exceed target)
  const float rate_epsilon = 0.001f;
  bool        cond1        = (tbs <= 292);
  bool        cond2        = (tbs <= 3824 && rate <= (0.67f + rate_epsilon));
  bool        cond3        = (rate <= (0.25f + rate_epsilon));
  int         bg           = (cond1 || cond2 || cond3) ? 2 : 1;
  return bg;
}

static int nr_ldpc_compute_lifting_size(int K, int Kb)
{
  // Find minimum Z such that K <= Kb * Z
  // Z must be from the valid lifting size set
  int min_Z = (K + Kb - 1) / Kb;

  for (int i = 0; i < NR_LDPC_NUM_LIFTING_SIZES; i++) {
    if (NR_LDPC_LIFTING_SIZES[i] >= min_Z) {
      return NR_LDPC_LIFTING_SIZES[i];
    }
  }
  return NR_LDPC_MAX_LIFTING_SIZE;
}

/// \brief Values the segmentation of the caller chose, for the encoder to derive the rest from.
///
/// A base graph, lifting size or code block count of zero asks the encoder to derive that value. A
/// negative filler count asks the same, so that a count of zero stays a count the caller gives.
struct nr_geometry_overrides {
  int base_graph;
  int lifting_size;
  int num_code_blocks;
  int num_filler_bits;
};

static cuda_result
nr_ldpc_init_config(nr_ldpc_config_t* cfg, int cb_info_bits, float rate, const nr_geometry_overrides& overrides)
{
  if (!cfg)
    return make_unexpected(std::string("The transport block encoding call carries an invalid configuration"));

  // cb_info_bits is the code block size including any CRC already attached
  // (TB CRC for single CB, or TB portion + CB CRC for multi-CB)
  // We do NOT add additional CRC here - that's handled at the TB layer

  // Select base graph based on the CB size
  cfg->base_graph = (overrides.base_graph > 0) ? overrides.base_graph : nr_ldpc_select_base_graph(cb_info_bits, rate);

  // Get Kb for lifting size calculation (variable: 6, 8, 9, or 10 for BG2)
  int Kb_for_Z = nr_ldpc_get_Kb(cb_info_bits, cfg->base_graph);

  // K is the code block size (already includes CRC from TB layer)
  int K = cb_info_bits;

  // Compute lifting size using the variable Kb
  cfg->lifting_size = (overrides.lifting_size > 0) ? overrides.lifting_size : nr_ldpc_compute_lifting_size(K, Kb_for_Z);
  cfg->lifting_set_index = get_lifting_set_index(cfg->lifting_size);

  if (cfg->lifting_set_index < 0) {
    return make_unexpected(std::string("The lifting size is not one the standard defines"));
  }

  // Per 3GPP TS 38.212, the LDPC encoder ALWAYS uses the full Kb columns:
  // - BG1: Kb = 22 (K_b = 22 systematic columns)
  // - BG2: Kb = 10 (K_b = 10 systematic columns)
  // The filler bits fill the gap between actual info bits and Kb * Z.
  int Kb_full = (cfg->base_graph == 1) ? NR_LDPC_BG1_INFO_NODES : NR_LDPC_BG2_INFO_NODES;

  // Information bits K' = Kb_full * Z (this is the message length for LDPC encoding)
  int K_prime = Kb_full * cfg->lifting_size;

  // Filler bits = K' - actual info bits. A count the caller gives is taken as given, zero
  // included. A code block that fills K' exactly leaves none, and reading that as a request to
  // derive one encodes a different length.
  cfg->num_filler_bits = (overrides.num_filler_bits >= 0) ? overrides.num_filler_bits : (K_prime - K);
  cfg->num_info_bits   = K_prime - cfg->num_filler_bits;

  // Number of parity bits (N - K)
  int N = (cfg->base_graph == 1) ? NR_LDPC_BG1_UNPUNCTURED_VARS * cfg->lifting_size
                                 : NR_LDPC_BG2_UNPUNCTURED_VARS * cfg->lifting_size;

  cfg->num_codeword_bits = N;
  cfg->num_parity_bits   = N - K_prime;

  // Default settings
  // Always puncture first 2*Z bits
  cfg->puncture           = true;
  cfg->max_parity_nodes   = (cfg->base_graph == 1) ? NR_LDPC_BG1_PARITY_NODES : NR_LDPC_BG2_PARITY_NODES;
  cfg->redundancy_version = 0;

  return {};
}

static cuda_result
nr_tb_init_config(nr_tb_config_t* tb_cfg, int tbs, float rate, const nr_geometry_overrides& overrides)
{
  if (!tb_cfg)
    return make_unexpected(std::string("The transport block encoding call carries an invalid configuration"));

  tb_cfg->tb_size_bits = tbs;

  // TB CRC size (16 or 24 bits)
  tb_cfg->tb_crc_bits = (tbs > 3824) ? NR_TB_CRC24A_BITS : NR_TB_CRC16_BITS;

  // B = A + L (TB + CRC)
  int B = tbs + tb_cfg->tb_crc_bits;

  // Maximum code block size depends on base graph
  int bg       = (overrides.base_graph > 0) ? overrides.base_graph : nr_ldpc_select_base_graph(tbs, rate);
  int K_cb_max = (bg == 1) ? NR_LDPC_MAX_CB_INFO_BITS : NR_LDPC_MAX_CB_INFO_BITS_BG2;

  // Code block segmentation
  int C = overrides.num_code_blocks;
  if (C <= 0) {
    // Add segment-level CRC
    int B_prime = B + 24;
    C           = (B <= K_cb_max) ? 1 : ((B_prime + K_cb_max - 1 - 24) / (K_cb_max - 24));
  }
  tb_cfg->num_code_blocks = C;

  if (C == 1) {
    // No CB CRC for single block
    tb_cfg->cb_crc_bits  = 0;
    tb_cfg->cb_size_bits = B;
  } else {
    // Multiple code blocks with CB CRC
    tb_cfg->cb_crc_bits = NR_CB_CRC24B_BITS;
    // Add segment-level CRC
    int B_prime = B + 24;

    // Per-CB size
    int total_cb_bits    = B_prime + (C - 1) * 24;
    tb_cfg->cb_size_bits = (total_cb_bits + C - 1) / C;
  }

  // Initialize LDPC config for code blocks
  // NOTE: LDPC encoder receives the full CB including CB CRC (per 3GPP TS 38.212).
  // The CB CRC is attached BEFORE LDPC encoding, so K = cb_size_bits (NOT cb_size_bits - cb_crc_bits).
  cuda_result status = nr_ldpc_init_config(&tb_cfg->ldpc_cfg, tb_cfg->cb_size_bits, rate, overrides);
  if (!status.has_value())
    return status;

  // A filler count the caller gives changes how many transport block bits a code block carries.
  tb_cfg->cb_size_bits = tb_cfg->ldpc_cfg.num_info_bits;
  tb_cfg->filler_bits  = tb_cfg->ldpc_cfg.num_filler_bits;

  return {};
}

/// Converts the base graph this file carries as an integer into the library's enumeration.
static ldpc_base_graph_type to_base_graph(int base_graph)
{
  return (base_graph == 1) ? ldpc_base_graph_type::BG1 : ldpc_base_graph_type::BG2;
}

/// \brief Builds the scrambling sequence initialiser, as per TS 38.211, Section 7.3.1.1.
///
/// \param[in] rnti           Radio network temporary identifier.
/// \param[in] codeword_index Codeword index, zero or one.
/// \param[in] n_id           Scrambling identity of the cell.
/// \return The value the Gold sequence generator starts from.
static uint32_t scrambling_c_init(uint16_t rnti, uint8_t codeword_index, uint16_t n_id)
{
  return (static_cast<uint32_t>(rnti) << 15U) + (static_cast<uint32_t>(codeword_index) << 14U) +
         static_cast<uint32_t>(n_id);
}

// Optional PDSCH diagnostics can be enabled through the OCUDU_PHY_CUDA_PDSCH_DIAGNOSTICS and
// OCUDU_PHY_CUDA_PDSCH_BIT_DIAGNOSTICS compile definitions.

// CRC-24A polynomial (MSB-first): D^24 + D^23 + D^18 + D^17 + D^14 + D^11 + D^10 + D^7 + D^6 + D^5 + D^4 + D^3 + D + 1
#define TB_CRC24A_POLY 0x864CFB
// CRC24B table for CB CRC computation (TX path)
// NOTE: Without separable compilation, extern __device__ doesn't work across .cu files.
// Define and initialize the table directly here.
// CRC24B polynomial: D^24 + D^23 + D^6 + D^5 + D + 1
// In MSB-first notation with leading 1: 0x1800063
// The leading 1 bit (D^24 term) is needed for the table-based algorithm
#define CRC24B_POLY 0x1800063
#define CRC24B_POLY_NO_MSB 0x800063
__device__ __constant__ uint32_t TB_CRC24B_TABLE[256];
__device__ __constant__ uint32_t TB_CRC24B_SLICE0[256];
__device__ __constant__ uint32_t TB_CRC24B_SLICE1[256];
__device__ __constant__ uint32_t TB_CRC24B_SLICE2[256];
__device__ __constant__ uint32_t TB_CRC24B_SLICE3[256];
__device__ __constant__ uint32_t TB_CRC24B_X_POWERS[21];
static bool                      tb_crc24b_initialized = false;
static cuda_result               init_tb_crc24b_table()
{
  if (tb_crc24b_initialized) {
    return {};
  }
  uint32_t h_table[256];
  uint32_t h_slice0[256];
  uint32_t h_slice1[256];
  uint32_t h_slice2[256];
  uint32_t h_slice3[256];
  uint32_t h_x_powers[21];
  for (int i = 0; i < 256; i++) {
    uint32_t crc = (uint32_t)i << 16;
    for (int j = 0; j < 8; j++) {
      if (crc & 0x800000) {
        crc = (crc << 1) ^ CRC24B_POLY;
      } else {
        crc <<= 1;
      }
    }
    h_table[i] = crc & 0xFFFFFF;
  }

  for (int i = 0; i < 256; i++) {
    uint32_t crc = h_table[i];
    h_slice3[i]  = crc;
    crc          = ((crc << 8) ^ h_table[(crc >> 16) & 0xFF]) & 0xFFFFFF;
    h_slice2[i]  = crc;
    crc          = ((crc << 8) ^ h_table[(crc >> 16) & 0xFF]) & 0xFFFFFF;
    h_slice1[i]  = crc;
    crc          = ((crc << 8) ^ h_table[(crc >> 16) & 0xFF]) & 0xFFFFFF;
    h_slice0[i]  = crc;
  }

  uint32_t x_8 = 1;
  for (int i = 0; i < 8; i++) {
    if (x_8 & 0x800000) {
      x_8 = ((x_8 << 1) ^ CRC24B_POLY_NO_MSB) & 0xFFFFFF;
    } else {
      x_8 = (x_8 << 1) & 0xFFFFFF;
    }
  }
  h_x_powers[0] = x_8;

  for (int i = 1; i < 21; i++) {
    uint32_t a      = h_x_powers[i - 1];
    uint32_t b      = h_x_powers[i - 1];
    uint32_t result = 0;
    while (b) {
      if (b & 1) {
        result ^= a;
      }
      b >>= 1;
      if (a & 0x800000) {
        a = ((a << 1) ^ CRC24B_POLY_NO_MSB) & 0xFFFFFF;
      } else {
        a = (a << 1) & 0xFFFFFF;
      }
    }
    h_x_powers[i] = result & 0xFFFFFF;
  }

  const struct {
    const void* symbol;
    const void* source;
    std::size_t size;
  } uploads[] = {
      {TB_CRC24B_TABLE, h_table, sizeof(h_table)},
      {TB_CRC24B_SLICE0, h_slice0, sizeof(h_slice0)},
      {TB_CRC24B_SLICE1, h_slice1, sizeof(h_slice1)},
      {TB_CRC24B_SLICE2, h_slice2, sizeof(h_slice2)},
      {TB_CRC24B_SLICE3, h_slice3, sizeof(h_slice3)},
      {TB_CRC24B_X_POWERS, h_x_powers, sizeof(h_x_powers)},
  };

  for (const auto& upload : uploads) {
    if (cuda_result status = check_cuda_error(cudaMemcpyToSymbol(upload.symbol, upload.source, upload.size),
                                              "upload the code block checksum tables");
        !status.has_value()) {
      return status;
    }
  }

  // The flag is only set once every table is on the device, so a failed upload is retried by the
  // next encode rather than leaving the kernels reading uninitialised memory.
  tb_crc24b_initialized = true;
  return {};
}

__device__ __forceinline__ uint32_t gf24_multiply_crc24b(uint32_t a, uint32_t b)
{
  uint32_t result = 0;

#pragma unroll
  for (int i = 0; i < 24; i++) {
    result ^= (b & 1) ? a : 0;
    b >>= 1;
    if (a & 0x800000) {
      a = ((a << 1) ^ CRC24B_POLY_NO_MSB) & 0xFFFFFF;
    } else {
      a = (a << 1) & 0xFFFFFF;
    }
  }

  return result & 0xFFFFFF;
}

__device__ __forceinline__ uint32_t crc24b_shift_bytes_fast(uint32_t crc, int n_bytes)
{
  if (n_bytes == 0 || crc == 0)
    return crc;

  uint32_t x_power = 0;
  bool     first   = true;

  for (int i = 0; i < 21 && n_bytes > 0; i++) {
    if (n_bytes & 1) {
      if (first) {
        x_power = TB_CRC24B_X_POWERS[i];
        first   = false;
      } else {
        x_power = gf24_multiply_crc24b(x_power, TB_CRC24B_X_POWERS[i]);
      }
    }
    n_bytes >>= 1;
  }

  return first ? crc : gf24_multiply_crc24b(crc, x_power);
}

__device__ __forceinline__ uint32_t crc24b_update_bytes_slicing4(uint32_t crc, const uint8_t* bytes, int num_bytes)
{
  int byte_idx = 0;
  for (; byte_idx + 4 <= num_bytes; byte_idx += 4) {
    uint8_t b0 = bytes[byte_idx];
    uint8_t b1 = bytes[byte_idx + 1];
    uint8_t b2 = bytes[byte_idx + 2];
    uint8_t b3 = bytes[byte_idx + 3];
    crc        = TB_CRC24B_SLICE0[((crc >> 16) ^ b0) & 0xFF] ^ TB_CRC24B_SLICE1[((crc >> 8) ^ b1) & 0xFF] ^
          TB_CRC24B_SLICE2[(crc ^ b2) & 0xFF] ^ TB_CRC24B_SLICE3[b3];
  }

  for (; byte_idx < num_bytes; byte_idx++) {
    uint8_t index = ((crc >> 16) ^ bytes[byte_idx]) & 0xFF;
    crc           = ((crc << 8) ^ TB_CRC24B_TABLE[index]) & 0xFFFFFF;
  }

  return crc & 0xFFFFFF;
}

__device__ __forceinline__ uint32_t crc24b_update_bytes_parallel_block(const uint8_t* bytes,
                                                                       int            num_bytes,
                                                                       uint32_t*      s_warp_crcs,
                                                                       int*           s_warp_bytes)
{
  const int tid     = threadIdx.x;
  const int lane_id = tid & 31;
  const int warp_id = tid >> 5;

  int bytes_per_thread = ((num_bytes + blockDim.x - 1) / blockDim.x + 3) & ~3;
  int my_start         = tid * bytes_per_thread;
  int my_end           = min(my_start + bytes_per_thread, num_bytes);
  int my_bytes         = max(my_end - my_start, 0);

  uint32_t my_crc = 0;
  if (my_bytes > 0) {
    my_crc = crc24b_update_bytes_slicing4(0, bytes + my_start, my_bytes);
  }

#pragma unroll
  for (int offset = 1; offset < 32; offset <<= 1) {
    uint32_t right_crc   = __shfl_down_sync(0xFFFFFFFF, my_crc, offset);
    int      right_bytes = __shfl_down_sync(0xFFFFFFFF, my_bytes, offset);
    if ((lane_id & ((offset << 1) - 1)) == 0) {
      my_crc = crc24b_shift_bytes_fast(my_crc, right_bytes) ^ right_crc;
      my_bytes += right_bytes;
    }
  }

  if (lane_id == 0) {
    s_warp_crcs[warp_id]  = my_crc;
    s_warp_bytes[warp_id] = my_bytes;
  }
  __syncthreads();

  constexpr int num_warps = 8;
  if (warp_id == 0 && lane_id < num_warps) {
    my_crc   = s_warp_crcs[lane_id];
    my_bytes = s_warp_bytes[lane_id];

#pragma unroll
    for (int offset = 1; offset < num_warps; offset <<= 1) {
      uint32_t right_crc   = __shfl_down_sync(0xFF, my_crc, offset);
      int      right_bytes = __shfl_down_sync(0xFF, my_bytes, offset);
      if ((lane_id & ((offset << 1) - 1)) == 0) {
        my_crc = crc24b_shift_bytes_fast(my_crc, right_bytes) ^ right_crc;
        my_bytes += right_bytes;
      }
    }
  }

  return (tid == 0) ? (my_crc & 0xFFFFFF) : 0;
}

// ============================================================================
// Transport Block Encoder Context
// ============================================================================

struct transport_block_encoder::impl {
  /// Operations on the encoder state, kept out of the interface.
  static cuda_expected<std::unique_ptr<impl>> encoder_create();
  static cuda_result                          encoder_configure(impl* handle, const tb_encoder_config_t* cfg);
  static cuda_result
             encoder_encode(impl* handle, const uint8_t* d_tb_input, uint8_t* d_output, const cuda_stream& stream);
  static int encoder_get_output_bits(impl* handle);
  static int encoder_get_num_code_blocks(impl* handle);
  static int encoder_get_base_graph(impl* handle);
  static int encoder_get_lifting_size(impl* handle);

  tb_encoder_config_t config;
  modulation_scheme   modulation = modulation_scheme::QPSK;
  nr_tb_config_t      tb_cfg;

  ldpc_encoder      ldpc_enc;
  ldpc_rate_matcher rate_matcher;
  scrambler         scrambler_unit;

  // Device buffers, each reserved for the largest shape the backend accepts and kept for the
  // lifetime of the encoder, so that the slot path never allocates.
  // TB + CRC
  device_vector<uint8_t> d_tb_with_crc;
  // Segmented code blocks
  device_vector<uint32_t> d_cb_bits;
  // LDPC encoded
  device_vector<uint32_t> d_encoded_bits;
  // Rate matched output
  device_vector<uint32_t> d_rate_matched;
  // Bit interleaved (before scrambling)
  device_vector<uint32_t> d_interleaved;
  // Scrambled output
  device_vector<uint32_t> d_scrambled;
  // TB CRC
  device_vector<uint32_t> d_crc;
  // Scratch for multi-CTA TB CRC
  device_vector<uint32_t> d_crc_chunk_crcs;
  // Scratch for multi-CTA TB CRC
  device_vector<int> d_crc_chunk_sizes;

  // Secondary stream for parallel scrambler execution (Phase 2 optimization)
  cuda_stream scrambler_stream;
  // Event to sync scrambler completion
  cuda_event scrambler_done;

  // Required buffer sizes for current config
  size_t tb_with_crc_size;
  size_t cb_bits_size;
  size_t encoded_bits_size;
  size_t rate_matched_size;
  // Total output words
  size_t output_words;

  // Per-CB rate matching (for variable CB lengths per 3GPP TS 38.212 Section 5.4.2.1)
  // Number of CBs with short E
  int nof_short_segments;
  // Rate-matched length for short CBs (floor)
  int E_short;
  // Rate-matched length for long CBs (ceil)
  int E_long;

  /// When true, configure must never cudaMalloc/cudaFree (resident RT lane mode).
  bool rt_locked = false;
};

// ============================================================================
// Transport Block Decoder Context
// ============================================================================

__global__ void __launch_bounds__(256, 4)
    fused_tb_segment_crc_kernel(const uint8_t* __restrict__ d_tb_input,
                                const uint32_t* __restrict__ d_tb_crc,
                                uint32_t* __restrict__ d_cb_output,
                                int tb_size_bits,
                                int tb_crc_bits,
                                int cb_size_bits,
                                int cb_crc_bits,
                                int num_cbs,
                                int cb_stride_words,
                                int num_filler_bits // F - filler bits per CB from LDPC config
    )
{
  int cb_idx = blockIdx.x;
  if (cb_idx >= num_cbs)
    return;

  // Shared memory for TB CRC bytes (up to 3 bytes for CRC24)
  __shared__ uint8_t  s_tb_crc[4];
  __shared__ uint32_t s_cb_crc_warp_crcs[8];
  __shared__ int      s_cb_crc_warp_bytes[8];

  // Thread 0 loads TB CRC into shared memory
  if (threadIdx.x == 0) {
    uint32_t crc_val = *d_tb_crc;
    if (tb_crc_bits == 24) {
      s_tb_crc[0] = (crc_val >> 16) & 0xFF;
      s_tb_crc[1] = (crc_val >> 8) & 0xFF;
      s_tb_crc[2] = crc_val & 0xFF;
    } else { // 16 bits
      s_tb_crc[0] = (crc_val >> 8) & 0xFF;
      s_tb_crc[1] = crc_val & 0xFF;
    }
  }
  __syncthreads();

  // Calculate this CB's portion of TB
  // Use LDPC-derived segment size (matches CPU segmenter behavior):
  // - K = cb_size_bits (total LDPC input bits = Kb * Z)
  // - K' = K - F (actual info bits per CB including CB CRC)
  // - cb_info_bits = K' - CB_CRC (data bits from TB per CB)
  //
  // For multi-CB last CB: cb_info_bits_last = cb_info_bits - TB_CRC (TB CRC handled separately)
  // For single-CB (cb_crc_bits == 0): include TB CRC in CB data (no subtraction)
  // Actual info bits per CB (incl CB CRC)
  int K_prime = cb_size_bits - num_filler_bits;
  // Data bits from TB per CB
  int cb_info_bits = K_prime - cb_crc_bits;

  // All CBs (including last) copy cb_info_bits of data
  // The data comes from the combined TB+TB_CRC stream
  int this_cb_data_bits = cb_info_bits;

  // Compute cb_start_bit by summing previous CBs' data bits
  int cb_start_bit = cb_idx * cb_info_bits;

  // Output pointer for this CB
  uint32_t* cb_ptr = d_cb_output + cb_idx * cb_stride_words;

  bool byte_aligned_fast_path = ((cb_start_bit | this_cb_data_bits | tb_size_bits | tb_crc_bits) & 7) == 0;

  // Phase 1: Segment TB data into this CB (parallel across threads).
  // Common PDSCH transport blocks are byte-aligned, so preserve byte order
  // directly instead of rebuilding every output word bit-by-bit.
  if (byte_aligned_fast_path) {
    int cb_start_byte = cb_start_bit >> 3;
    int data_bytes    = this_cb_data_bits >> 3;
    int tb_bytes      = tb_size_bits >> 3;

    for (int word_idx = threadIdx.x; word_idx < cb_stride_words; word_idx += blockDim.x) {
      int      byte_base = word_idx << 2;
      uint32_t word      = 0;

#pragma unroll
      for (int byte = 0; byte < 4; ++byte) {
        int     local_byte = byte_base + byte;
        uint8_t value      = 0;

        if (local_byte < data_bytes) {
          int tb_byte_idx = cb_start_byte + local_byte;
          if (tb_byte_idx < tb_bytes) {
            value = d_tb_input[tb_byte_idx];
          } else if (tb_byte_idx < tb_bytes + (tb_crc_bits >> 3)) {
            value = s_tb_crc[tb_byte_idx - tb_bytes];
          }
        }

        word |= static_cast<uint32_t>(value) << (8 * byte);
      }

      cb_ptr[word_idx] = word;
    }
  } else {
    for (int word_idx = threadIdx.x; word_idx < cb_stride_words; word_idx += blockDim.x) {
      uint32_t word = 0;

      // Pack 32 bits into this word
      int base_bit = word_idx * 32;
      for (int b = 0; b < 32; b++) {
        int bit_idx = base_bit + b;
        // Past CB size = filler region
        if (bit_idx >= cb_size_bits)
          break;

        // Only copy data for data region (0 to this_cb_data_bits-1)
        // CB CRC goes at position this_cb_data_bits
        // Filler region after CB CRC stays 0
        if (bit_idx < this_cb_data_bits) {
          int     tb_bit_idx = cb_start_bit + bit_idx;
          uint8_t bit_val    = 0;

          // Read from TB data or TB CRC depending on position
          // TB CRC is distributed across CBs for both single-CB and multi-CB cases
          if (tb_bit_idx < tb_size_bits) {
            // Read from TB data
            int byte_idx = tb_bit_idx / 8;
            int bit_pos  = tb_bit_idx % 8;
            bit_val      = (d_tb_input[byte_idx] >> (7 - bit_pos)) & 1;
          } else if (tb_bit_idx < tb_size_bits + tb_crc_bits) {
            // Read from TB CRC (bits after TB data, distributed across CBs)
            int crc_bit_idx = tb_bit_idx - tb_size_bits;
            // TB CRC is stored MSB-first in s_tb_crc (byte 0 = MSB)
            int crc_byte_idx = crc_bit_idx / 8;
            int crc_bit_pos  = crc_bit_idx % 8;
            bit_val          = (s_tb_crc[crc_byte_idx] >> (7 - crc_bit_pos)) & 1;
          }
          // else: past TB+CRC = 0 (shouldn't happen for valid segmentation)

          // MSB-first within each byte
          int byte_in_word = b / 8;
          int bit_in_byte  = 7 - (b % 8);
          int out_bit_pos  = byte_in_word * 8 + bit_in_byte;
          word |= (uint32_t)bit_val << out_bit_pos;
        }
        // CB CRC region and filler bits are 0 (implicit)
      }

      cb_ptr[word_idx] = word;
    }
  }

  // Sync before CB CRC computation
  __syncthreads();

  // Phase 2: Compute and attach CB CRC24B. All threads compute the
  // full-byte CRC region; thread 0 handles partial-byte tails and writes.
  if (cb_crc_bits > 0) {
    // Compute CRC over the data bits we actually copied (this_cb_data_bits)
    // CB CRC is placed at position this_cb_data_bits
    int info_bits  = this_cb_data_bits;
    int info_bytes = info_bits / 8;

    // Compute CRC24B over info bits
    uint8_t* cb_bytes = (uint8_t*)cb_ptr;
    uint32_t crc = crc24b_update_bytes_parallel_block(cb_bytes, info_bytes, s_cb_crc_warp_crcs, s_cb_crc_warp_bytes);
    __syncthreads();

    int remainder_bits = info_bits % 8;
    if (threadIdx.x == 0) {
      if (remainder_bits != 0) {
        uint8_t byte_val = cb_bytes[info_bytes] & (0xFF << (8 - remainder_bits));
        uint8_t index    = ((crc >> 16) ^ byte_val) & 0xFF;
        crc              = ((crc << 8) ^ TB_CRC24B_TABLE[index]) & 0xFFFFFF;
      }

      // Handle non-byte-aligned input: reverse CRC bits
      // This compensates for the zero-padding of the partial last byte
      if (remainder_bits != 0) {
        int reverse_bits = 8 - remainder_bits;
        for (int m = 0; m < reverse_bits; m++) {
          if (crc & 1) {
            crc = (crc ^ CRC24B_POLY) >> 1;
          } else {
            crc = crc >> 1;
          }
        }
        crc &= 0xFFFFFF;
      }

      // Store CRC24B at the end of info bits
      int crc_byte_start = info_bits / 8;
      int crc_bit_offset = info_bits % 8;

      if (crc_bit_offset == 0) {
        // CRC starts at byte boundary
        cb_bytes[crc_byte_start]     = (crc >> 16) & 0xFF;
        cb_bytes[crc_byte_start + 1] = (crc >> 8) & 0xFF;
        cb_bytes[crc_byte_start + 2] = crc & 0xFF;
      } else {
        // CRC starts mid-byte (rare case)
        for (int i = 0; i < 24; i++) {
          int     bit_pos      = info_bits + i;
          int     byte_idx_out = bit_pos / 8;
          int     bit_in_byte  = 7 - (bit_pos % 8);
          uint8_t crc_bit      = (crc >> (23 - i)) & 1;
          if (crc_bit) {
            cb_bytes[byte_idx_out] |= (1 << bit_in_byte);
          }
        }
      }
    }
  }
}

// ============================================================================
// FULLY-FUSED TB CRC + Segment Kernel (eliminates separate CRC kernel)
// ============================================================================

/// \brief De-segment code blocks back to transport block
__global__ void bits_to_bytes_kernel(const uint32_t* __restrict__ d_bits, uint8_t* __restrict__ d_bytes, int num_bytes)
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx >= num_bytes)
    return;

  uint8_t byte = 0;
  for (int b = 0; b < 8; b++) {
    int     bit_idx  = idx * 8 + b;
    int     word_idx = bit_idx / 32;
    int     word_bit = bit_idx % 32;
    uint8_t bit      = (d_bits[word_idx] >> word_bit) & 1;
    byte |= bit << (7 - b);
  }

  d_bytes[idx] = byte;
}

/// \brief Clear unused bits in the last byte of a buffer (for non-byte-aligned TBS)
#ifdef OCUDU_PHY_CUDA_PDSCH_DIAGNOSTICS
/// \brief Diagnostic kernel to print CB boundary values.
__global__ void pdsch_cb_boundary_diagnostics_kernel(const uint32_t* __restrict__ d_rate_matched,
                                                     const uint32_t* __restrict__ d_scramble_seq,
                                                     int nof_short,
                                                     int E_short,
                                                     int E_long,
                                                     int words_per_cb,
                                                     int Q_m,
                                                     int total_bits)
{
  if (threadIdx.x != 0 || blockIdx.x != 0)
    return;

  // Print values around the CB boundary.
  int short_cb_total_bits = nof_short * E_short;
  printf("[GPU CB Boundary Diagnostics] short_cb_total_bits=%d, words_per_cb=%d\n", short_cb_total_bits, words_per_cb);

  for (int out_bit_idx = short_cb_total_bits - 5; out_bit_idx < short_cb_total_bits + 5 && out_bit_idx < total_bits;
       out_bit_idx++) {
    if (out_bit_idx < 0)
      continue;

    // Step 1: Find CB and bit position
    int cb_idx, bit_in_cb, bits_per_cb;
    if (out_bit_idx < short_cb_total_bits) {
      cb_idx      = out_bit_idx / E_short;
      bit_in_cb   = out_bit_idx % E_short;
      bits_per_cb = E_short;
    } else {
      int offset  = out_bit_idx - short_cb_total_bits;
      cb_idx      = nof_short + offset / E_long;
      bit_in_cb   = offset % E_long;
      bits_per_cb = E_long;
    }

    // Interleaving
    int K            = bits_per_cb / Q_m;
    int j            = bit_in_cb / Q_m;
    int i            = bit_in_cb % Q_m;
    int in_bit_in_cb = i * K + j;

    // Read from rate matched buffer (MSB-first)
    int      in_word         = cb_idx * words_per_cb + in_bit_in_cb / 32;
    int      in_bit_in_word  = in_bit_in_cb % 32;
    int      in_byte_in_word = in_bit_in_word / 8;
    int      in_bit_in_byte  = 7 - (in_bit_in_word % 8);
    int      in_bit_pos      = in_byte_in_word * 8 + in_bit_in_byte;
    uint32_t rm_bit          = (d_rate_matched[in_word] >> in_bit_pos) & 1u;

    // Scrambling bit (MSB-first to match gold_sequence_generate_kernel)
    int scr_word_idx = out_bit_idx / 32;
    // MSB-first: bit 0 at position 31
    int      scr_bit_pos = 31 - (out_bit_idx % 32);
    uint32_t scr_bit     = (d_scramble_seq[scr_word_idx] >> scr_bit_pos) & 1u;

    printf("  out[%d]: cb=%d, bit_in_cb=%d, K=%d, in_bit=%d, in_word=%d, rm=%d, scr=%d, final=%d\n",
           out_bit_idx,
           cb_idx,
           bit_in_cb,
           K,
           in_bit_in_cb,
           in_word,
           rm_bit,
           scr_bit,
           rm_bit ^ scr_bit);
  }
}
#endif

/// \brief FUSED kernel: interleave + scramble + pack_to_bytes in ONE pass!
///
/// This replaces 3 separate kernel launches with 1, eliminating ~30-40µs of
/// kernel launch overhead. Each thread processes 4 output bytes (32 bits = 1 word).
///
/// Input: d_rate_matched (per-CB word-aligned layout)
/// Output: d_output_bytes (packed scrambled interleaved bytes)
///
/// Supports variable per-CB E values per 3GPP TS 38.212 Section 5.4.2.1:
///   - CBs 0 to nof_short-1 have E_short bits
///   - CBs nof_short to num_cbs-1 have E_long bits
__global__ void
interleave_scramble_pack_kernel(const uint32_t* __restrict__ d_rate_matched, // Input: rate matched bits (per-CB layout)
                                const uint32_t* __restrict__ d_scramble_seq, // Scrambling sequence
                                uint8_t* __restrict__ d_output_bytes,        // Output: packed bytes
                                int nof_short,                               // Number of CBs with E_short
                                int E_short,                                 // Rate-matched length for short CBs
                                int E_long,                                  // Rate-matched length for long CBs
                                int words_per_cb, // Words per CB in input (based on E_long for uniform stride)
                                int num_cbs,
                                int Q_m,        // Modulation order (for interleaving)
                                int total_bits, // Total output bits
                                int num_bytes   // Total output bytes
)
{
  // Each thread processes 4 bytes (32 bits) for better efficiency
  int word_idx  = blockIdx.x * blockDim.x + threadIdx.x;
  int base_byte = word_idx * 4;
  if (base_byte >= num_bytes)
    return;

  // Precompute boundary between short and long CBs
  int short_cb_total_bits = nof_short * E_short;

  // Process 4 bytes (32 bits)
  uint8_t bytes[4] = {0, 0, 0, 0};

  for (int byte_in_word = 0; byte_in_word < 4 && (base_byte + byte_in_word) < num_bytes; byte_in_word++) {
    uint8_t byte     = 0;
    int     byte_idx = base_byte + byte_in_word;

    for (int b = 0; b < 8; b++) {
      int out_bit_idx = byte_idx * 8 + b;
      if (out_bit_idx >= total_bits)
        break;

      // Step 1: Find which CB and bit position (handling variable E per CB)
      int cb_idx;
      int bit_in_cb;
      int bits_per_cb;

      if (out_bit_idx < short_cb_total_bits) {
        // In the short CB region
        cb_idx      = out_bit_idx / E_short;
        bit_in_cb   = out_bit_idx % E_short;
        bits_per_cb = E_short;
      } else {
        // In the long CB region
        int offset  = out_bit_idx - short_cb_total_bits;
        cb_idx      = nof_short + offset / E_long;
        bit_in_cb   = offset % E_long;
        bits_per_cb = E_long;
      }

      // Apply bit interleaving per 3GPP TS 38.212 Section 5.4.2.2
      // Number of symbols for this CB
      int K            = bits_per_cb / Q_m;
      int j            = bit_in_cb / Q_m;
      int i            = bit_in_cb % Q_m;
      int in_bit_in_cb = i * K + j;

      // Read from rate matched buffer (MSB-first within each byte)
      int in_word         = cb_idx * words_per_cb + in_bit_in_cb / 32;
      int in_bit_in_word  = in_bit_in_cb % 32;
      int in_byte_in_word = in_bit_in_word / 8;
      // MSB-first within byte
      int      in_bit_in_byte = 7 - (in_bit_in_word % 8);
      int      in_bit_pos     = in_byte_in_word * 8 + in_bit_in_byte;
      uint32_t bit            = (d_rate_matched[in_word] >> in_bit_pos) & 1u;

      // Step 2: Apply scrambling based on OUTPUT position (after interleaving)
      // Per 3GPP, scrambling is applied AFTER bit interleaving.
      // The scrambling sequence is continuous across all CBs.
      // Read scrambling bit (MSB-first to match gold_sequence_generate_kernel)
      int scr_word_idx = out_bit_idx / 32;
      // MSB-first: bit 0 at position 31
      int      scr_bit_pos = 31 - (out_bit_idx % 32);
      uint32_t scr_bit     = (d_scramble_seq[scr_word_idx] >> scr_bit_pos) & 1u;
      bit ^= scr_bit;

      // Step 3: Pack into output byte
      // Use same bit ordering as fallback path (interleave_and_compact + bits_to_bytes)
      // which produces: output bit i at byte bit (7 - i%8) in the intermediate word,
      // then bits_to_bytes reads word bit j and puts at byte bit (7-j%8).
      // Net effect: output bit i → byte bit (i%8) (LSB-first within byte)
      byte |= (uint8_t)bit << b;

#ifdef OCUDU_PHY_CUDA_PDSCH_BIT_DIAGNOSTICS
      // Diagnostics: trace specific bit positions around symbol 104 for QPSK.
      if (out_bit_idx >= 208 && out_bit_idx <= 215) {
        printf("[GPU BIT DIAG] out_bit[%d]: cb=%d, bit_in_cb=%d, K=%d, j=%d, i=%d, in_bit=%d | "
               "rm_bit=%d (word %d, pos %d), scr_bit=%d, final=%d, byte_in_word=%d, b=%d, byte_pos=%d\n",
               out_bit_idx,
               cb_idx,
               bit_in_cb,
               K,
               j,
               i,
               in_bit_in_cb,
               (d_rate_matched[in_word] >> in_bit_pos) & 1u,
               in_word,
               in_bit_pos,
               scr_bit,
               bit,
               byte_in_word,
               b,
               7 - b);
      }
#endif
    }
    bytes[byte_in_word] = byte;
  }

  // Write output bytes
  for (int i = 0; i < 4 && (base_byte + i) < num_bytes; i++) {
    d_output_bytes[base_byte + i] = bytes[i];
  }
}

/// \brief Compact per-CB word-aligned bits into a continuous stream
///
/// Takes bits from multiple CBs with word-aligned storage and packs them
/// into a continuous bit stream.
///
/// Supports variable per-CB E values per 3GPP TS 38.212 Section 5.4.2.1:
///   - CBs 0 to nof_short-1 have E_short bits
///   - CBs nof_short to num_cbs-1 have E_long bits
///
/// Input layout:
///   CB 0: words_per_cb words (bits 0 to E_short/E_long-1 valid)
///   CB 1: words_per_cb words (bits 0 to E_short/E_long-1 valid)
///   ...
///
/// Output layout:
///   Continuous bit stream: CB0[0..E0-1], CB1[0..E1-1], ...
__global__ void compact_cb_bits_kernel(const uint32_t* __restrict__ d_input,
                                       uint32_t* __restrict__ d_output,
                                       int nof_short,    // Number of CBs with E_short
                                       int E_short,      // Rate-matched length for short CBs
                                       int E_long,       // Rate-matched length for long CBs
                                       int words_per_cb, // Words per CB in input (includes padding)
                                       int num_cbs,
                                       int total_bits // Total output bits
)
{
  int out_bit_idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (out_bit_idx >= total_bits)
    return;

  // Precompute boundary between short and long CBs
  int short_cb_total_bits = nof_short * E_short;

  // Find which CB and bit position (handling variable E per CB)
  int cb_idx;
  int bit_in_cb;

  if (out_bit_idx < short_cb_total_bits) {
    // In the short CB region
    cb_idx    = out_bit_idx / E_short;
    bit_in_cb = out_bit_idx % E_short;
  } else {
    // In the long CB region
    int offset = out_bit_idx - short_cb_total_bits;
    cb_idx     = nof_short + offset / E_long;
    bit_in_cb  = offset % E_long;
  }

  if (cb_idx >= num_cbs)
    return;

  // Read from input (word-aligned per CB, MSB-first within each byte)
  int in_word         = cb_idx * words_per_cb + bit_in_cb / 32;
  int in_bit_in_word  = bit_in_cb % 32;
  int in_byte_in_word = in_bit_in_word / 8;
  // MSB-first within byte
  int      in_bit_in_byte = 7 - (in_bit_in_word % 8);
  int      in_bit         = in_byte_in_word * 8 + in_bit_in_byte;
  uint32_t bit            = (d_input[in_word] >> in_bit) & 1u;

  // Write to output (continuous stream, MSB-first within each byte)
  int out_word         = out_bit_idx / 32;
  int out_bit_in_word  = out_bit_idx % 32;
  int out_byte_in_word = out_bit_in_word / 8;
  // MSB-first within byte
  int out_bit_in_byte = 7 - (out_bit_in_word % 8);
  int out_bit         = out_byte_in_word * 8 + out_bit_in_byte;
  atomicOr(&d_output[out_word], bit << out_bit);
}

/// \brief Interleave and compact per-CB bits in one pass
///
/// Per 3GPP TS 38.212 Section 5.4.2.2:
///   e(j*Qm + i) = f(i*K + j)  for i=0..Qm-1, j=0..K-1
/// where K = E/Qm (number of symbols per CB).
///
/// This kernel reads from per-CB word-aligned rate-matched data,
/// applies the interleaving formula per CB, and writes to a
/// continuous output stream (all CBs concatenated without padding).
///
/// Supports variable per-CB E values per 3GPP TS 38.212 Section 5.4.2.1:
///   - CBs 0 to nof_short-1 have E_short bits
///   - CBs nof_short to num_cbs-1 have E_long bits
__global__ void interleave_and_compact_kernel(const uint32_t* __restrict__ d_input,
                                              uint32_t* __restrict__ d_output,
                                              int nof_short,    // Number of CBs with E_short
                                              int E_short,      // Rate-matched length for short CBs
                                              int E_long,       // Rate-matched length for long CBs
                                              int words_per_cb, // Words per CB in input (includes padding)
                                              int num_cbs,
                                              int Q_m,       // Modulation order
                                              int total_bits // Total output bits
)
{
  int out_bit_idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (out_bit_idx >= total_bits)
    return;

  // Precompute boundary between short and long CBs
  int short_cb_total_bits = nof_short * E_short;

  // Find which CB and bit position (handling variable E per CB)
  int cb_idx;
  int bit_in_cb;
  int bits_per_cb;

  if (out_bit_idx < short_cb_total_bits) {
    // In the short CB region
    cb_idx      = out_bit_idx / E_short;
    bit_in_cb   = out_bit_idx % E_short;
    bits_per_cb = E_short;
  } else {
    // In the long CB region
    int offset  = out_bit_idx - short_cb_total_bits;
    cb_idx      = nof_short + offset / E_long;
    bit_in_cb   = offset % E_long;
    bits_per_cb = E_long;
  }

  if (cb_idx >= num_cbs)
    return;

  // Per 3GPP TS 38.212 Section 5.4.2.2:
  // Output position j*Qm + i reads from input position i*K + j
  // Number of symbols per CB
  int K = bits_per_cb / Q_m;
  // Symbol index
  int j = bit_in_cb / Q_m;
  // Bit index within symbol
  int i = bit_in_cb % Q_m;
  // Input position within CB
  int in_bit_in_cb = i * K + j;

  // Read from input (word-aligned per CB, MSB-first within each byte)
  int in_word         = cb_idx * words_per_cb + in_bit_in_cb / 32;
  int in_bit_in_word  = in_bit_in_cb % 32;
  int in_byte_in_word = in_bit_in_word / 8;
  // MSB-first within byte
  int      in_bit_in_byte = 7 - (in_bit_in_word % 8);
  int      in_bit         = in_byte_in_word * 8 + in_bit_in_byte;
  uint32_t bit            = (d_input[in_word] >> in_bit) & 1u;

  // Write to output (continuous stream, MSB-first within each byte)
  int out_word         = out_bit_idx / 32;
  int out_bit_in_word  = out_bit_idx % 32;
  int out_byte_in_word = out_bit_in_word / 8;
  // MSB-first within byte
  int out_bit_in_byte = 7 - (out_bit_in_word % 8);
  int out_bit         = out_byte_in_word * 8 + out_bit_in_byte;
  atomicOr(&d_output[out_word], bit << out_bit);
}

/// \brief Clear unused bits in the last data byte when TBS is not byte-aligned
///
/// For CRC correctness, unused bits must be zeros (not garbage from input).
/// CRC is computed over rounded-up bytes, so bits beyond TBS affect the CRC value.

// Maximum sizes for pre-allocation (avoids any runtime cudaMalloc/cudaFree)
// Based on the maximum NR TBS and coded-bit allocation supported by this backend:
// - Max TBS: NR_MAX_TBS_BITS (~1.28Mbits); reserve 200KB with CRC and headroom
// - Max CBs: ceil((A + TB CRC) / (K_cb - CB CRC)) for BG1
// - Max LDPC input: Kb=22 * Z=384 = 8448 bits/CB = 264 words/CB
// - Max LDPC output: the encoder writes all 68 BG1 columns, 68 * Z = 26112 bits/CB = 816 words/CB
// - Max rate matched: G up to 2Mbits plus per-CB word-rounding slack
static constexpr size_t MAX_TB_WITH_CRC_BYTES = 200000;
// 16 KiB slices keep each 256-thread block at ~64 bytes/thread while the serial GF-combine
// stays cheap (<=13 chunks for the largest TB).
static constexpr int PDSCH_CRC24A_MULTI_CTA_CHUNK_BYTES = 16384;
static constexpr int MAX_CRC24A_CHUNKS =
    (MAX_TB_WITH_CRC_BYTES + PDSCH_CRC24A_MULTI_CTA_CHUNK_BYTES - 1) / PDSCH_CRC24A_MULTI_CTA_CHUNK_BYTES;
static constexpr size_t MAX_PDSCH_CODEBLOCKS =
    (NR_MAX_TBS_BITS + NR_TB_CRC24A_BITS + (NR_LDPC_MAX_CB_INFO_BITS - NR_CB_CRC24B_BITS) - 1) /
    (NR_LDPC_MAX_CB_INFO_BITS - NR_CB_CRC24B_BITS);
// ~256KB (2M bits / 32)
static constexpr size_t MAX_OUTPUT_WORDS      = 65536;
static constexpr size_t MAX_CB_BITS_SIZE      = MAX_PDSCH_CODEBLOCKS * 264 * sizeof(uint32_t);
static constexpr size_t MAX_ENCODED_BITS_SIZE = MAX_PDSCH_CODEBLOCKS * 816 * sizeof(uint32_t);
static constexpr size_t MAX_RATE_MATCHED_SIZE = (MAX_OUTPUT_WORDS + MAX_PDSCH_CODEBLOCKS) * sizeof(uint32_t);

cuda_expected<std::unique_ptr<transport_block_encoder::impl>> transport_block_encoder::impl::encoder_create()
{
  std::unique_ptr<impl> ctx = std::make_unique<impl>();

  // Create the components the chain runs through.
  cuda_expected<ldpc_encoder> created_encoder = ldpc_encoder::create();
  if (!created_encoder.has_value()) {
    return make_unexpected(created_encoder.error());
  }
  ctx->ldpc_enc = std::move(created_encoder.value());

  cuda_expected<ldpc_rate_matcher> created_matcher = ldpc_rate_matcher::create();
  if (!created_matcher.has_value()) {
    return make_unexpected(created_matcher.error());
  }
  ctx->rate_matcher = std::move(created_matcher.value());

  cuda_expected<scrambler> created_scrambler = scrambler::create();
  if (!created_scrambler.has_value()) {
    return make_unexpected(created_scrambler.error());
  }
  ctx->scrambler_unit = std::move(created_scrambler.value());

  cuda_expected<cuda_stream> created_stream = cuda_stream::create();
  if (!created_stream.has_value()) {
    return make_unexpected(created_stream.error());
  }
  ctx->scrambler_stream = std::move(created_stream.value());

  // The scrambling sequence is produced on a stream of its own, and the main stream waits on this
  // event before it reads the sequence.
  cuda_expected<cuda_event> created_event = cuda_event::create();
  if (!created_event.has_value()) {
    return make_unexpected(created_event.error());
  }
  ctx->scrambler_done = std::move(created_event.value());

  // Reserve every device buffer for the largest shape the backend accepts, so that neither
  // configure() nor encode() allocates on a slot thread.
  cuda_expected<device_vector<uint32_t>> created_crc = device_vector<uint32_t>::create(1);
  if (!created_crc.has_value()) {
    return make_unexpected(created_crc.error());
  }
  ctx->d_crc = std::move(created_crc.value());

  cuda_expected<device_vector<uint32_t>> created_chunk_crcs = device_vector<uint32_t>::create(MAX_CRC24A_CHUNKS);
  if (!created_chunk_crcs.has_value()) {
    return make_unexpected(created_chunk_crcs.error());
  }
  ctx->d_crc_chunk_crcs = std::move(created_chunk_crcs.value());

  cuda_expected<device_vector<int>> created_chunk_sizes = device_vector<int>::create(MAX_CRC24A_CHUNKS);
  if (!created_chunk_sizes.has_value()) {
    return make_unexpected(created_chunk_sizes.error());
  }
  ctx->d_crc_chunk_sizes = std::move(created_chunk_sizes.value());

  cuda_expected<device_vector<uint8_t>> created_tb_with_crc = device_vector<uint8_t>::create(MAX_TB_WITH_CRC_BYTES);
  if (!created_tb_with_crc.has_value()) {
    return make_unexpected(created_tb_with_crc.error());
  }
  ctx->d_tb_with_crc = std::move(created_tb_with_crc.value());

  cuda_expected<device_vector<uint32_t>> created_cb_bits =
      device_vector<uint32_t>::create(MAX_CB_BITS_SIZE / sizeof(uint32_t));
  if (!created_cb_bits.has_value()) {
    return make_unexpected(created_cb_bits.error());
  }
  ctx->d_cb_bits = std::move(created_cb_bits.value());

  cuda_expected<device_vector<uint32_t>> created_encoded_bits =
      device_vector<uint32_t>::create(MAX_ENCODED_BITS_SIZE / sizeof(uint32_t));
  if (!created_encoded_bits.has_value()) {
    return make_unexpected(created_encoded_bits.error());
  }
  ctx->d_encoded_bits = std::move(created_encoded_bits.value());

  cuda_expected<device_vector<uint32_t>> created_rate_matched =
      device_vector<uint32_t>::create(MAX_RATE_MATCHED_SIZE / sizeof(uint32_t));
  if (!created_rate_matched.has_value()) {
    return make_unexpected(created_rate_matched.error());
  }
  ctx->d_rate_matched = std::move(created_rate_matched.value());

  cuda_expected<device_vector<uint32_t>> created_interleaved = device_vector<uint32_t>::create(MAX_OUTPUT_WORDS);
  if (!created_interleaved.has_value()) {
    return make_unexpected(created_interleaved.error());
  }
  ctx->d_interleaved = std::move(created_interleaved.value());

  cuda_expected<device_vector<uint32_t>> created_scrambled = device_vector<uint32_t>::create(MAX_OUTPUT_WORDS);
  if (!created_scrambled.has_value()) {
    return make_unexpected(created_scrambled.error());
  }
  ctx->d_scrambled = std::move(created_scrambled.value());

  return ctx;
}

cuda_result transport_block_encoder::impl::encoder_configure(transport_block_encoder::impl* handle,
                                                             const tb_encoder_config_t*     cfg)
{
  if (!handle || !cfg)
    return make_unexpected(std::string("The transport block encoding call carries an invalid configuration"));

  handle->config = *cfg;

  // The caller gives what its own segmentation chose, and the encoder derives the rest from it.
  // Patching the derived values after they are computed leaves a mixed geometry whenever the
  // caller gives some of them and not others.
  nr_geometry_overrides overrides = {.base_graph      = cfg->base_graph,
                                     .lifting_size    = cfg->lifting_size,
                                     .num_code_blocks = cfg->num_code_blocks,
                                     .num_filler_bits = cfg->nof_filler_bits};

  // Initialize TB configuration
  cuda_result status = nr_tb_init_config(&handle->tb_cfg, cfg->tb_size_bits, cfg->code_rate, overrides);
  if (!status.has_value())
    return status;

#ifdef OCUDU_PHY_CUDA_PDSCH_DIAGNOSTICS
  printf("[TB Encoder] geometry: BG=%d, Z=%d, K=%d, K'=%d, F=%d, C=%d, N=%d\n",
         handle->tb_cfg.ldpc_cfg.base_graph,
         handle->tb_cfg.ldpc_cfg.lifting_size,
         handle->tb_cfg.ldpc_cfg.num_info_bits,
         handle->tb_cfg.ldpc_cfg.num_info_bits + handle->tb_cfg.ldpc_cfg.num_filler_bits,
         handle->tb_cfg.ldpc_cfg.num_filler_bits,
         handle->tb_cfg.num_code_blocks,
         handle->tb_cfg.ldpc_cfg.num_codeword_bits);
#endif

  // Configure LDPC encoder (fast encoder)
  status = handle->ldpc_enc.configure(to_base_graph(handle->tb_cfg.ldpc_cfg.base_graph),
                                      static_cast<ldpc::lifting_size_t>(handle->tb_cfg.ldpc_cfg.lifting_size));
  if (!status.has_value())
    return status;

  // Calculate per-CB rate-matched lengths per 3GPP TS 38.212 Section 5.4.2.1
  // Use CPU-provided values if available for multi-CB consistency
  if (cfg->E_short > 0 && cfg->E_long > 0) {
    // Use values from CPU segmenter for exact consistency
    handle->nof_short_segments = cfg->nof_short_segments;
    handle->E_short            = cfg->E_short;
    handle->E_long             = cfg->E_long;
  } else {
    // Compute internally (fallback for standalone use)
    int G                 = cfg->num_allocated_res;
    int Q_m               = cfg->modulation_order;
    int nof_layers        = cfg->num_layers;
    int C                 = handle->tb_cfg.num_code_blocks;
    int total_symbols     = G / Q_m;
    int symbols_per_layer = total_symbols / nof_layers;

    // nof_short_segments = C - (symbols_per_layer % C)
    // Short CBs use floor, remaining use ceiling
    handle->nof_short_segments = C - (symbols_per_layer % C);
    int symbols_short          = symbols_per_layer / C;
    // ceiling
    int symbols_long = (symbols_per_layer + C - 1) / C;
    handle->E_short  = symbols_short * nof_layers * Q_m;
    handle->E_long   = symbols_long * nof_layers * Q_m;
  }

  // Configure rate matcher with the longer E (all CBs must fit in this buffer)
  // We'll handle variable lengths in rate matching and interleaving
  // The rate matcher is sized for the longest code block; the shorter ones reconfigure it.
  codeblock_metadata rm_cfg          = {};
  rm_cfg.tb_common.base_graph        = to_base_graph(handle->tb_cfg.ldpc_cfg.base_graph);
  rm_cfg.tb_common.lifting_size      = static_cast<ldpc::lifting_size_t>(handle->tb_cfg.ldpc_cfg.lifting_size);
  rm_cfg.tb_common.rv                = static_cast<unsigned>(cfg->redundancy_version);
  rm_cfg.tb_common.mod               = handle->modulation;
  rm_cfg.cb_specific.rm_length       = static_cast<unsigned>(handle->E_long);
  rm_cfg.cb_specific.nof_filler_bits = handle->tb_cfg.ldpc_cfg.num_filler_bits;
  rm_cfg.tb_common.Nref              = static_cast<unsigned>(handle->config.N_ref);
  status                             = handle->rate_matcher.configure(rm_cfg);
  if (!status.has_value())
    return status;

  // Configure scrambler (if enabled)
  if (cfg->enable_scrambling) {
    handle->scrambler_unit.init(scrambling_c_init(cfg->n_RNTI, cfg->q, cfg->n_ID));
  }

  // Validate sizes fit the max preallocation from encoder_create(). Never grow here —
  // resident RT lanes must not cudaMalloc on the slot path.
  int tb_crc_bits       = handle->tb_cfg.tb_crc_bits;
  int tb_with_crc_bytes = (cfg->tb_size_bits + tb_crc_bits + 7) / 8;

  int ldpc_input_bits    = static_cast<int>(handle->ldpc_enc.get_input_bits());
  int cb_words           = (ldpc_input_bits + 31) / 32;
  int encoded_words      = static_cast<int>(handle->ldpc_enc.get_output_words());
  int rm_words           = (handle->E_long + 31) / 32;
  int total_output_words = (cfg->num_allocated_res + 31) / 32;

  handle->tb_with_crc_size  = tb_with_crc_bytes;
  handle->cb_bits_size      = handle->tb_cfg.num_code_blocks * cb_words * sizeof(uint32_t);
  handle->encoded_bits_size = handle->tb_cfg.num_code_blocks * encoded_words * sizeof(uint32_t);
  handle->rate_matched_size = handle->tb_cfg.num_code_blocks * rm_words * sizeof(uint32_t);
  handle->output_words      = total_output_words;
  int required_crc_chunks   = (static_cast<int>((cfg->tb_size_bits + 7) / 8) + PDSCH_CRC24A_MULTI_CTA_CHUNK_BYTES - 1) /
                            PDSCH_CRC24A_MULTI_CTA_CHUNK_BYTES;

  if (static_cast<size_t>(handle->tb_with_crc_size) > handle->d_tb_with_crc.size_bytes() ||
      static_cast<size_t>(required_crc_chunks) > handle->d_crc_chunk_crcs.size() ||
      handle->cb_bits_size > handle->d_cb_bits.size_bytes() ||
      handle->encoded_bits_size > handle->d_encoded_bits.size_bytes() ||
      handle->rate_matched_size > handle->d_rate_matched.size_bytes() ||
      static_cast<size_t>(total_output_words) > handle->d_interleaved.size()) {
    // Shape exceeds max preallocation (or create() did not finish). Never grow in RT.
    return make_unexpected(std::string("The transport block encoding call carries an invalid configuration"));
  }

  return {};
}

cuda_result transport_block_encoder::impl::encoder_encode(transport_block_encoder::impl* handle,
                                                          const uint8_t*                 d_tb_input,
                                                          uint8_t*                       d_output,
                                                          const cuda_stream&             stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());
  if (!handle || !d_tb_input || !d_output) {
    return make_unexpected(std::string("The transport block encoding call carries an invalid configuration"));
  }

  // Launch the scrambling sequence early because it is independent of TB data
  // and LDPC output. The main stream waits on it immediately before use.

  int ldpc_input_bits = static_cast<int>(handle->ldpc_enc.get_input_bits());
  int cb_words        = (ldpc_input_bits + 31) / 32;
  int total_bits      = handle->config.num_allocated_res;
  int Q_m             = handle->config.modulation_order;

  // Initialize CB CRC24B table (once, on first use)
  if (handle->tb_cfg.cb_crc_bits > 0) {
    if (cuda_result status = init_tb_crc24b_table(); !status.has_value()) {
      return status;
    }
  }

  // =========================================================================
  // PARALLEL SCRAMBLER: Start sequence generation early on separate stream
  // =========================================================================
  // The scrambling sequence depends only on the initialiser set at configure time,
  // not on the actual TB data or LDPC output. We can generate it in parallel with
  // TB CRC, segmentation, and LDPC encoding.
  const uint32_t* d_scramble_seq     = nullptr;
  bool            scrambler_launched = false;
  if (handle->config.enable_scrambling && Q_m >= 2) {
    // Launch scrambler on secondary stream - will run in parallel with main pipeline
    cuda_result scr_status = handle->scrambler_unit.generate(total_bits, handle->scrambler_stream);
    if (scr_status.has_value()) {
      d_scramble_seq = handle->scrambler_unit.sequence();
      // The main stream waits on this event before it reads the sequence. A record that does not
      // take leaves scrambler_launched false, and the sequence is generated again on the main
      // stream, which needs no event.
      scrambler_launched = handle->scrambler_done.record(handle->scrambler_stream).has_value();
    }
  }

  // =========================================================================
  // Step 1: Compute TB CRC separately (more reliable than fused kernel)
  // Step 2: Segment TB + attach CB CRC
  // =========================================================================
  // Compute TB CRC
  cuda_result crc_status =
      (handle->tb_cfg.tb_crc_bits == 24)
          ? crc24a_compute(handle->d_crc.data(), d_tb_input, handle->config.tb_size_bits, stream)
          : crc16_compute(
                reinterpret_cast<uint16_t*>(handle->d_crc.data()), d_tb_input, handle->config.tb_size_bits, stream);
  if (!crc_status.has_value()) {
    return crc_status;
  }

  // Segment TB + attach CRCs
  fused_tb_segment_crc_kernel<<<handle->tb_cfg.num_code_blocks, 256, 0, native_stream>>>(
      d_tb_input,
      handle->d_crc.data(),
      handle->d_cb_bits.data(),
      handle->config.tb_size_bits,
      handle->tb_cfg.tb_crc_bits,
      ldpc_input_bits, // K = Kb * Z (LDPC block size)
      handle->tb_cfg.cb_crc_bits,
      handle->tb_cfg.num_code_blocks,
      cb_words,
      handle->tb_cfg.ldpc_cfg.num_filler_bits);

  if (cuda_result status = check_cuda_error(cudaPeekAtLastError(), "launch the segmentation kernel");
      !status.has_value()) {
    return status;
  }

#ifdef OCUDU_PHY_CUDA_PDSCH_DIAGNOSTICS
  {
    (void)stream.synchronize();
    uint32_t h_crc = 0;
    (void)copy_to_host(span<uint32_t>(&h_crc, 1), span<const uint32_t>(handle->d_crc).first(1));
    fprintf(stderr, "[GPU PDSCH] TB CRC = 0x%06x (separate compute)\n", h_crc & 0x00FFFFFF);

    std::vector<uint32_t> cb_raw(cb_words * handle->tb_cfg.num_code_blocks);
    (void)copy_to_host(span<uint32_t>(cb_raw), span<const uint32_t>(handle->d_cb_bits).first(cb_raw.size()));
    for (int cb = 0; cb < handle->tb_cfg.num_code_blocks; cb++) {
      fprintf(stderr, "[GPU PDSCH] CB%d data (K*Z=%d bits): first 32 bytes = ", cb, ldpc_input_bits);
      uint8_t* byte_ptr = (uint8_t*)(cb_raw.data() + cb * cb_words);
      for (int b = 0; b < 32; b++) {
        fprintf(stderr, "%02x ", byte_ptr[b]);
      }
      fprintf(stderr, "\n");

      // Print CRC region (TB ends at tb_size_bits, CRC is next 24 bits)
      int crc_byte_start = handle->config.tb_size_bits / 8;
      fprintf(stderr, "[GPU PDSCH] CB%d CRC region (bytes %d-%d): ", cb, crc_byte_start, crc_byte_start + 3);
      for (int b = crc_byte_start; b < crc_byte_start + 4 && b < cb_words * 4; b++) {
        fprintf(stderr, "%02x ", byte_ptr[b]);
      }
      fprintf(stderr, " (expected: %02x %02x %02x)\n", (h_crc >> 16) & 0xFF, (h_crc >> 8) & 0xFF, h_crc & 0xFF);
    }
  }
#endif

  // Step 3: LDPC encoding.
  int         encoded_words = static_cast<int>(handle->ldpc_enc.get_output_words());
  cuda_result encode_status = handle->ldpc_enc.encode(
      handle->d_encoded_bits.data(), handle->d_cb_bits.data(), handle->tb_cfg.num_code_blocks, stream);
  if (!encode_status.has_value()) {
    return encode_status;
  }

#ifdef OCUDU_PHY_CUDA_PDSCH_DIAGNOSTICS
  // Diagnostics: print first few LDPC encoded bytes per CB before rate matching.
  {
    (void)stream.synchronize();
    std::vector<uint32_t> enc_diagnostics(handle->tb_cfg.num_code_blocks * encoded_words);
    (void)copy_to_host(span<uint32_t>(enc_diagnostics),
                       span<const uint32_t>(handle->d_encoded_bits).first(enc_diagnostics.size()));
    for (int cb = 0; cb < handle->tb_cfg.num_code_blocks; cb++) {
      fprintf(stderr, "[GPU PDSCH] CB%d LDPC encoded (N=%d words): first 8 bytes = ", cb, encoded_words);
      uint8_t* byte_ptr = (uint8_t*)(enc_diagnostics.data() + cb * encoded_words);
      for (int b = 0; b < 8; b++) {
        fprintf(stderr, "%02x ", byte_ptr[b]);
      }
      fprintf(stderr, "\n");
    }
  }
#endif

  // Step 4: Rate matching with per-CB E values (3GPP TS 38.212 Section 5.4.2.1)
  // Short CBs get E_short, long CBs get E_long
  int num_cbs   = handle->tb_cfg.num_code_blocks;
  int nof_short = handle->nof_short_segments;
  int E_short   = handle->E_short;
  int E_long    = handle->E_long;
  // Uniform stride based on max E
  int rm_words = (E_long + 31) / 32;

#ifdef OCUDU_PHY_CUDA_PDSCH_DIAGNOSTICS
  fprintf(stderr,
          "[GPU PDSCH] RM: num_cbs=%d, nof_short=%d, E_short=%d, E_long=%d, rm_words=%d\n",
          num_cbs,
          nof_short,
          E_short,
          E_long,
          rm_words);
#endif

  // Clear output buffer (needed when E_short < E_long)
  span<uint32_t> d_rate_matched = span<uint32_t>(handle->d_rate_matched).first(num_cbs * rm_words);
  if (cuda_result status = device_zero_async(d_rate_matched, stream); !status.has_value()) {
    return status;
  }

  if (nof_short == num_cbs) {
    // All CBs are short - configure with E_short and single batch call
    codeblock_metadata rm_cfg_short          = {};
    rm_cfg_short.tb_common.base_graph        = to_base_graph(handle->tb_cfg.ldpc_cfg.base_graph);
    rm_cfg_short.tb_common.lifting_size      = static_cast<ldpc::lifting_size_t>(handle->tb_cfg.ldpc_cfg.lifting_size);
    rm_cfg_short.tb_common.rv                = handle->config.redundancy_version;
    rm_cfg_short.tb_common.mod               = handle->modulation;
    rm_cfg_short.cb_specific.rm_length       = static_cast<unsigned>(E_short);
    rm_cfg_short.cb_specific.nof_filler_bits = handle->tb_cfg.ldpc_cfg.num_filler_bits;
    rm_cfg_short.tb_common.Nref              = static_cast<unsigned>(handle->config.N_ref);
    if (cuda_result status = handle->rate_matcher.configure(rm_cfg_short); !status.has_value()) {
      return status;
    }
    // Note: for all-short case, E_words == rm_words, so use standard batch call
    if (cuda_result status =
            handle->rate_matcher.rate_match(d_rate_matched.data(), handle->d_encoded_bits.data(), num_cbs, 0, stream);
        !status.has_value()) {
      return status;
    }
  } else if (nof_short == 0) {
    // All CBs are long - already configured with E_long, single batch call
    if (cuda_result status =
            handle->rate_matcher.rate_match(d_rate_matched.data(), handle->d_encoded_bits.data(), num_cbs, 0, stream);
        !status.has_value()) {
      return status;
    }
  } else {
    // Mixed: rate match short CBs first, then long CBs
    // IMPORTANT: Use rm_words stride for uniform buffer layout
    // Configure for short CBs
    codeblock_metadata rm_cfg_short          = {};
    rm_cfg_short.tb_common.base_graph        = to_base_graph(handle->tb_cfg.ldpc_cfg.base_graph);
    rm_cfg_short.tb_common.lifting_size      = static_cast<ldpc::lifting_size_t>(handle->tb_cfg.ldpc_cfg.lifting_size);
    rm_cfg_short.tb_common.rv                = handle->config.redundancy_version;
    rm_cfg_short.tb_common.mod               = handle->modulation;
    rm_cfg_short.cb_specific.rm_length       = static_cast<unsigned>(E_short);
    rm_cfg_short.cb_specific.nof_filler_bits = handle->tb_cfg.ldpc_cfg.num_filler_bits;
    rm_cfg_short.tb_common.Nref              = static_cast<unsigned>(handle->config.N_ref);
    if (cuda_result status = handle->rate_matcher.configure(rm_cfg_short); !status.has_value()) {
      return status;
    }
    if (cuda_result status = handle->rate_matcher.rate_match(
            d_rate_matched.data(), handle->d_encoded_bits.data(), nof_short, rm_words, stream);
        !status.has_value()) {
      return status;
    }

    // Configure for long CBs
    codeblock_metadata rm_cfg_long          = {};
    rm_cfg_long.tb_common.base_graph        = to_base_graph(handle->tb_cfg.ldpc_cfg.base_graph);
    rm_cfg_long.tb_common.lifting_size      = static_cast<ldpc::lifting_size_t>(handle->tb_cfg.ldpc_cfg.lifting_size);
    rm_cfg_long.tb_common.rv                = handle->config.redundancy_version;
    rm_cfg_long.tb_common.mod               = handle->modulation;
    rm_cfg_long.cb_specific.rm_length       = static_cast<unsigned>(E_long);
    rm_cfg_long.cb_specific.nof_filler_bits = handle->tb_cfg.ldpc_cfg.num_filler_bits;
    rm_cfg_long.tb_common.Nref              = static_cast<unsigned>(handle->config.N_ref);
    if (cuda_result status = handle->rate_matcher.configure(rm_cfg_long); !status.has_value()) {
      return status;
    }
    span<uint32_t> d_long_half = d_rate_matched.subspan(nof_short * rm_words, (num_cbs - nof_short) * rm_words);
    if (cuda_result status = handle->rate_matcher.rate_match(d_long_half.data(),
                                                             handle->d_encoded_bits.data() + nof_short * encoded_words,
                                                             num_cbs - nof_short,
                                                             rm_words,
                                                             stream);
        !status.has_value()) {
      return status;
    }
  }

#ifdef OCUDU_PHY_CUDA_PDSCH_DIAGNOSTICS
  // Diagnostics: print first few rate-matched bits per CB.
  {
    (void)stream.synchronize();
    std::vector<uint32_t> rm_diagnostics(num_cbs * rm_words);
    (void)copy_to_host(span<uint32_t>(rm_diagnostics), span<const uint32_t>(d_rate_matched));
    for (int cb = 0; cb < num_cbs; cb++) {
      int E_cb = (cb < nof_short) ? E_short : E_long;
      fprintf(stderr, "[GPU PDSCH] CB%d rate matched (E=%d): first 8 bytes = ", cb, E_cb);
      uint8_t* byte_ptr = (uint8_t*)(rm_diagnostics.data() + cb * rm_words);
      for (int b = 0; b < 8; b++) {
        fprintf(stderr, "%02x ", byte_ptr[b]);
      }
      fprintf(stderr, "\n");
    }
  }
#endif

  // Steps 5-7: fused interleave + scramble + pack_to_bytes.
  int output_bytes = (total_bits + 7) / 8;
  int block_size   = 256;

  if (handle->config.enable_scrambling && Q_m >= 2) {
    // The scrambler was launched earlier on scrambler_stream in parallel with
    // TB CRC, segmentation, and LDPC encoding. Now we need to wait for it.
    if (scrambler_launched) {
      // Wait for scrambler stream to complete before using the sequence
      if (cuda_result status = stream.wait_event(handle->scrambler_done); !status.has_value()) {
        return status;
      }
    } else {
      // Fallback: generate scrambling sequence synchronously if parallel launch failed
      cuda_result scr_status = handle->scrambler_unit.generate(total_bits, stream);
      if (!scr_status.has_value()) {
        return scr_status;
      }
      d_scramble_seq = handle->scrambler_unit.sequence();
    }

    // Each thread processes 4 bytes (1 word = 32 bits)
    int num_words       = (output_bytes + 3) / 4;
    int num_word_blocks = (num_words + block_size - 1) / block_size;
    interleave_scramble_pack_kernel<<<num_word_blocks, block_size, 0, native_stream>>>(
        d_rate_matched.data(),
        d_scramble_seq,
        d_output,
        nof_short, // Number of short CBs
        E_short,   // Bits per short CB
        E_long,    // Bits per long CB
        rm_words,  // words per CB in input (uniform stride)
        num_cbs,
        Q_m,
        total_bits,
        output_bytes);

    if (cuda_result status = check_cuda_error(cudaPeekAtLastError(), "launch the interleave and scramble kernel");
        !status.has_value()) {
      return status;
    }

#ifdef OCUDU_PHY_CUDA_PDSCH_DIAGNOSTICS
    // Run diagnostic kernel for multi-CB boundary tracing.
    if (num_cbs > 1) {
      (void)stream.synchronize();
      pdsch_cb_boundary_diagnostics_kernel<<<1, 1, 0, native_stream>>>(
          d_rate_matched.data(), d_scramble_seq, nof_short, E_short, E_long, rm_words, Q_m, total_bits);
      (void)stream.synchronize();
    }
#endif

#ifdef OCUDU_PHY_CUDA_PDSCH_DIAGNOSTICS
    // Diagnostics: print scrambling sequence and output bytes.
    {
      (void)stream.synchronize();
      std::vector<uint32_t> scr_diagnostics(4);
      (void)copy_to_host(span<uint32_t>(scr_diagnostics), span<const uint32_t>(d_scramble_seq, scr_diagnostics.size()));
      fprintf(stderr,
              "[GPU PDSCH] Scramble seq (first 4 words): %08x %08x %08x %08x\n",
              scr_diagnostics[0],
              scr_diagnostics[1],
              scr_diagnostics[2],
              scr_diagnostics[3]);

      std::vector<uint8_t> out_diagnostics(std::min(32, output_bytes));
      (void)copy_to_host(span<uint8_t>(out_diagnostics), span<const uint8_t>(d_output, out_diagnostics.size()));
      fprintf(stderr, "[GPU PDSCH] Output bytes (first %zu): ", out_diagnostics.size());
      for (size_t i = 0; i < out_diagnostics.size(); i++) {
        fprintf(stderr, "%02x ", out_diagnostics[i]);
      }
      fprintf(stderr, "\n");

      // For multi-CB, also print bytes around the CB boundary
      if (num_cbs > 1 && E_short > 0) {
        // First byte of CB1 region
        int cb_boundary_byte = E_short / 8;
        if (cb_boundary_byte + 8 <= output_bytes) {
          std::vector<uint8_t> boundary_diagnostics(16);
          (void)copy_to_host(span<uint8_t>(boundary_diagnostics),
                             span<const uint8_t>(d_output + cb_boundary_byte - 8, boundary_diagnostics.size()));
          fprintf(stderr, "[GPU PDSCH] Bytes at CB boundary (byte %d): ", cb_boundary_byte);
          for (int i = 0; i < 16; i++) {
            fprintf(stderr, "%02x ", boundary_diagnostics[i]);
          }
          fprintf(stderr, "\n");
        }
      }
    }
#endif
  } else {
    // FALLBACK: Separate kernels for edge cases (BPSK or no scrambling)
    int            total_words   = (total_bits + 31) / 32;
    span<uint32_t> d_interleaved = span<uint32_t>(handle->d_interleaved).first(total_words);
    if (cuda_result status = device_zero_async(d_interleaved, stream); !status.has_value()) {
      return status;
    }

    int num_thread_blocks = (total_bits + block_size - 1) / block_size;
    if (Q_m >= 2) {
      interleave_and_compact_kernel<<<num_thread_blocks, block_size, 0, native_stream>>>(
          d_rate_matched.data(), d_interleaved.data(), nof_short, E_short, E_long, rm_words, num_cbs, Q_m, total_bits);
    } else {
      compact_cb_bits_kernel<<<num_thread_blocks, block_size, 0, native_stream>>>(
          d_rate_matched.data(), d_interleaved.data(), nof_short, E_short, E_long, rm_words, num_cbs, total_bits);
    }

    if (cuda_result status = check_cuda_error(cudaPeekAtLastError(), "launch the interleave kernel");
        !status.has_value()) {
      return status;
    }

    uint32_t* d_final_output = d_interleaved.data();
    if (handle->config.enable_scrambling) {
      cuda_result scr_status =
          handle->scrambler_unit.apply_xor(handle->d_scrambled.data(), d_interleaved.data(), total_bits, stream);
      if (!scr_status.has_value())
        return scr_status;
      d_final_output = handle->d_scrambled.data();
    }

    int num_byte_blocks = (output_bytes + block_size - 1) / block_size;
    bits_to_bytes_kernel<<<num_byte_blocks, block_size, 0, native_stream>>>(d_final_output, d_output, output_bytes);

    if (cuda_result status = check_cuda_error(cudaPeekAtLastError(), "launch the bit packing kernel");
        !status.has_value()) {
      return status;
    }
  }

  return {};
}

int transport_block_encoder::impl::encoder_get_output_bits(transport_block_encoder::impl* handle)
{
  return handle ? handle->config.num_allocated_res : 0;
}

int transport_block_encoder::impl::encoder_get_num_code_blocks(transport_block_encoder::impl* handle)
{
  return handle ? handle->tb_cfg.num_code_blocks : 0;
}

int transport_block_encoder::impl::encoder_get_base_graph(transport_block_encoder::impl* handle)
{
  return handle ? handle->tb_cfg.ldpc_cfg.base_graph : 0;
}

int transport_block_encoder::impl::encoder_get_lifting_size(transport_block_encoder::impl* handle)
{
  return handle ? handle->tb_cfg.ldpc_cfg.lifting_size : 0;
}

// ============================================================================
// Encoder
// ============================================================================

cuda_expected<transport_block_encoder> transport_block_encoder::create()
{
  cuda_expected<std::unique_ptr<impl>> created_state = impl::encoder_create();
  if (!created_state.has_value()) {
    return make_unexpected(created_state.error());
  }

  return transport_block_encoder(std::move(created_state.value()));
}

transport_block_encoder::transport_block_encoder() = default;

transport_block_encoder::transport_block_encoder(std::unique_ptr<impl> state_) : state(std::move(state_)) {}

transport_block_encoder::transport_block_encoder(transport_block_encoder&& other) noexcept = default;

transport_block_encoder& transport_block_encoder::operator=(transport_block_encoder&& other) noexcept = default;

transport_block_encoder::~transport_block_encoder() = default;

cuda_result transport_block_encoder::configure(const transport_block_encoder_config& config)
{
  if (state == nullptr) {
    return make_unexpected(std::string("The transport block encoder holds no state"));
  }

  tb_encoder_config_t cfg = {};
  cfg.tb_size_bits        = static_cast<int>(config.nof_tb_bits);
  cfg.num_layers          = static_cast<int>(config.nof_layers);
  cfg.modulation_order    = static_cast<int>(get_bits_per_symbol(config.modulation));
  cfg.num_allocated_res   = static_cast<int>(config.nof_allocated_bits);
  cfg.redundancy_version  = static_cast<int>(config.redundancy_version);
  cfg.code_rate           = config.code_rate;
  cfg.n_RNTI              = config.rnti;
  cfg.n_ID                = config.n_id;
  cfg.q                   = config.codeword_index;
  cfg.enable_scrambling   = config.enable_scrambling;
  // Zero leaves the segmentation to derive it, which is what an absent value asks for.
  cfg.base_graph =
      config.base_graph.has_value() ? ((config.base_graph.value() == ldpc_base_graph_type::BG1) ? 1 : 2) : 0;
  cfg.num_code_blocks    = static_cast<int>(config.nof_codeblocks);
  cfg.lifting_size       = config.lifting_size.has_value() ? static_cast<int>(config.lifting_size.value()) : 0;
  cfg.nof_filler_bits    = config.nof_filler_bits.has_value() ? static_cast<int>(config.nof_filler_bits.value()) : -1;
  cfg.nof_short_segments = static_cast<int>(config.nof_short_codeblocks);
  cfg.E_short            = static_cast<int>(config.nof_short_bits);
  cfg.E_long             = static_cast<int>(config.nof_long_bits);
  cfg.N_ref              = static_cast<int>(config.nof_buffer_bits);

  // The encoder keeps the modulation itself, so the rate matcher can be told which one it packs for.
  state->modulation = config.modulation;

  return impl::encoder_configure(state.get(), &cfg);
}

cuda_result transport_block_encoder::encode(uint8_t* d_output, const uint8_t* d_tb_input, const cuda_stream& stream)
{
  if (state == nullptr) {
    return make_unexpected(std::string("The transport block encoder holds no state"));
  }

  return impl::encoder_encode(state.get(), d_tb_input, d_output, stream);
}

unsigned transport_block_encoder::get_nof_output_bits() const
{
  return (state == nullptr) ? 0U : static_cast<unsigned>(impl::encoder_get_output_bits(state.get()));
}

unsigned transport_block_encoder::get_nof_codeblocks() const
{
  return (state == nullptr) ? 0U : static_cast<unsigned>(impl::encoder_get_num_code_blocks(state.get()));
}

ldpc_base_graph_type transport_block_encoder::get_base_graph() const
{
  return ((state != nullptr) && (impl::encoder_get_base_graph(state.get()) == 1)) ? ldpc_base_graph_type::BG1
                                                                                  : ldpc_base_graph_type::BG2;
}

ldpc::lifting_size_t transport_block_encoder::get_lifting_size() const
{
  if (state == nullptr) {
    return ldpc::LS2;
  }

  return static_cast<ldpc::lifting_size_t>(impl::encoder_get_lifting_size(state.get()));
}

} // namespace cuda
} // namespace ocudu
