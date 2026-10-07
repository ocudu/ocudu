// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Gold sequence generation and scrambling on the GPU, as per TS 38.211, Section 5.2.1.
///
/// The sequence is advanced by multiplying the state of each shift register by a precomputed jump
/// matrix, 1 multiplication per set bit of the offset, instead of clocking the registers once for
/// every bit skipped. Generated sequences are held in a cache shared by every scrambler in the
/// process, keyed on the pair of the sequence initialiser and the offset, because a base station
/// asks for the same pair on every slot.

#include "ocudu/cuda/adt/cuda_error.h"
#include "ocudu/cuda/phy/upper/sequence_generators/scrambling.h"
#include <algorithm>
#include <array>
#include <cstdio>
#include <cuda_fp16.h>
#include <mutex>
#include <new>
#include <vector>

namespace ocudu {
namespace cuda {

// ============================================================================
// Constants
// ============================================================================

#define NC_SKIP 1600      // Pre-skip cycles for Gold sequence
#define LFSR_BITS 31      // LFSR state size
#define MAX_JUMP_POWER 24 // Supports up to 2^24 = 16M bit advancement

// ============================================================================
// LFSR Jump Tables (Matrix Exponentiation)
// ============================================================================
//
// For an LFSR, state transition is linear over GF(2):
//   new_state = M × old_state  (matrix multiply mod 2)
//
// To advance N steps: state_N = M^N × state_0
// Using binary exponentiation: pre-compute M^1, M^2, M^4, M^8, ...
// Then compose: M^N = M^(2^a) × M^(2^b) × ... for each set bit in N
//
// Each matrix row is stored as a 32-bit mask indicating which input bits
// XOR together to produce that output bit.
//
// Memory: 2 LFSRs × 25 powers × 31 rows × 4 bytes = 6.2 KB constant memory

__constant__ uint32_t d_x1_jump[MAX_JUMP_POWER + 1][LFSR_BITS];
__constant__ uint32_t d_x2_jump[MAX_JUMP_POWER + 1][LFSR_BITS];

// Flag to track if jump tables are initialized
static bool g_jump_tables_initialized = false;
static bool g_jump_tables_available   = true;

// ============================================================================
// Host: Matrix Operations for Jump Table Computation
// ============================================================================

/// \brief Multiply two 31×31 GF(2) matrices
/// Each matrix is 31 uint32_t values (rows), bits are columns
static void matrix_multiply_gf2(const uint32_t A[LFSR_BITS], const uint32_t B[LFSR_BITS], uint32_t C[LFSR_BITS])
{
  // First, transpose B for efficient column access
  uint32_t B_T[LFSR_BITS] = {0};
  for (int i = 0; i < LFSR_BITS; i++) {
    for (int j = 0; j < LFSR_BITS; j++) {
      if (B[j] & (1u << i)) {
        B_T[i] |= (1u << j);
      }
    }
  }

  // C[i][j] = popcount(A[i] & B_T[j]) mod 2
  for (int i = 0; i < LFSR_BITS; i++) {
    C[i] = 0;
    for (int j = 0; j < LFSR_BITS; j++) {
      uint32_t dot = A[i] & B_T[j];
      // Count bits, if odd then set bit j
      int count = __builtin_popcount(dot);
      if (count & 1) {
        C[i] |= (1u << j);
      }
    }
  }
}

/// \brief Initialize the base transition matrix for x1 LFSR
/// x1(n+31) = x1(n+3) XOR x1(n)
/// State stored as: bit i = x1(n+i)
/// After one step: new[i] = old[i+1] for i<30, new[30] = old[0] XOR old[3]
static void init_x1_base_matrix(uint32_t M[LFSR_BITS])
{
  for (int i = 0; i < LFSR_BITS; i++) {
    M[i] = 0;
  }
  // new[i] = old[i+1] for i = 0..29
  for (int i = 0; i < LFSR_BITS - 1; i++) {
    M[i] = 1u << (i + 1);
  }
  // new[30] = old[0] XOR old[3]
  M[LFSR_BITS - 1] = (1u << 0) | (1u << 3);
}

/// \brief Initialize the base transition matrix for x2 LFSR
/// x2(n+31) = x2(n+3) XOR x2(n+2) XOR x2(n+1) XOR x2(n)
static void init_x2_base_matrix(uint32_t M[LFSR_BITS])
{
  for (int i = 0; i < LFSR_BITS; i++) {
    M[i] = 0;
  }
  // new[i] = old[i+1] for i = 0..29
  for (int i = 0; i < LFSR_BITS - 1; i++) {
    M[i] = 1u << (i + 1);
  }
  // new[30] = old[0] XOR old[1] XOR old[2] XOR old[3]
  M[LFSR_BITS - 1] = (1u << 0) | (1u << 1) | (1u << 2) | (1u << 3);
}

/// \brief Compute and upload jump tables to GPU constant memory
/// Pre-computes M^(2^p) for p = 0..MAX_JUMP_POWER for both LFSRs
static cudaError_t initialize_jump_tables()
{
  if (g_jump_tables_initialized) {
    return cudaSuccess;
  }

  // CUDA runtime calls such as cudaSetDeviceFlags may leave a non-fatal
  // error in the per-thread runtime slot after a context is already active.
  // Clear it before the synchronous constant-memory uploads below so stale
  // state does not make scrambler initialization fail on otherwise usable
  // discrete GPUs.
  (void)cudaGetLastError();

  uint32_t h_x1_jump[MAX_JUMP_POWER + 1][LFSR_BITS];
  uint32_t h_x2_jump[MAX_JUMP_POWER + 1][LFSR_BITS];

  // Initialize base matrices (M^1)
  init_x1_base_matrix(h_x1_jump[0]);
  init_x2_base_matrix(h_x2_jump[0]);

  // Compute M^(2^p) = M^(2^(p-1)) × M^(2^(p-1)) for p = 1..MAX_JUMP_POWER
  for (int p = 1; p <= MAX_JUMP_POWER; p++) {
    matrix_multiply_gf2(h_x1_jump[p - 1], h_x1_jump[p - 1], h_x1_jump[p]);
    matrix_multiply_gf2(h_x2_jump[p - 1], h_x2_jump[p - 1], h_x2_jump[p]);
  }

  // Upload to constant memory
  cudaError_t err;
  err = cudaMemcpyToSymbol(d_x1_jump, h_x1_jump, sizeof(h_x1_jump));
  if (err != cudaSuccess) {
    (void)cudaGetLastError();
    g_jump_tables_available   = false;
    g_jump_tables_initialized = true;
    std::fprintf(stderr,
                 "[OCUDU PHY CUDA] LFSR jump-table upload failed (%s); using host-generated scrambling sequences\n",
                 cudaGetErrorString(err));
    // return success to enable the host fallback
    return cudaSuccess;
  }

  err = cudaMemcpyToSymbol(d_x2_jump, h_x2_jump, sizeof(h_x2_jump));
  if (err != cudaSuccess) {
    (void)cudaGetLastError();
    g_jump_tables_available   = false;
    g_jump_tables_initialized = true;
    std::fprintf(stderr,
                 "[OCUDU PHY CUDA] LFSR jump-table upload failed (%s); using host-generated scrambling sequences\n",
                 cudaGetErrorString(err));
    // return success to enable the host fallback
    return cudaSuccess;
  }

  g_jump_tables_available   = true;
  g_jump_tables_initialized = true;
  return cudaSuccess;
}

static inline uint32_t advance_x1_host(uint32_t x1)
{
  uint32_t new_bit = ((x1 >> 3) ^ x1) & 1;
  return (x1 >> 1) | (new_bit << 30);
}

static inline uint32_t advance_x2_host(uint32_t x2)
{
  uint32_t new_bit = ((x2 >> 3) ^ (x2 >> 2) ^ (x2 >> 1) ^ x2) & 1;
  return (x2 >> 1) | (new_bit << 30);
}

static void generate_gold_sequence_host(std::vector<uint32_t>& sequence, uint32_t c_init, int num_words, int bit_offset)
{
  uint32_t x1 = 1;
  uint32_t x2 = c_init & 0x7FFFFFFF;

  const int total_advance = NC_SKIP + bit_offset;
  for (int i = 0; i != total_advance; ++i) {
    x1 = advance_x1_host(x1);
    x2 = advance_x2_host(x2);
  }

  sequence.resize(num_words);
  for (int word_idx = 0; word_idx != num_words; ++word_idx) {
    uint32_t word = 0;
    for (int b = 0; b != 32; ++b) {
      uint32_t c_bit = (x1 ^ x2) & 1;
      word |= (c_bit << (31 - b));
      x1 = advance_x1_host(x1);
      x2 = advance_x2_host(x2);
    }
    sequence[word_idx] = word;
  }
}

// ============================================================================
// Device: Fast LFSR Advancement using Jump Tables
// ============================================================================

/// \brief Apply a pre-computed jump matrix to LFSR state
/// new_state[i] = popcount(state & jump[i]) mod 2
__device__ __forceinline__ uint32_t apply_jump_matrix(uint32_t state, const uint32_t jump[LFSR_BITS])
{
  uint32_t result = 0;
#pragma unroll
  for (int i = 0; i < LFSR_BITS; i++) {
    uint32_t masked = state & jump[i];
    // If odd number of bits set, set bit i in result
    if (__popc(masked) & 1) {
      result |= (1u << i);
    }
  }
  return result;
}

/// \brief Advance x1 LFSR by N steps using matrix exponentiation - O(log N)
__device__ uint32_t advance_x1_fast(uint32_t x1, int n)
{
  // Apply jump matrices for each set bit in n
  // Only check up to highest set bit (saves iterations for small n)
  while (n) {
    // Find lowest set bit (0-indexed)
    int p = __ffs(n) - 1;
    x1    = apply_jump_matrix(x1, d_x1_jump[p]);
    // Clear lowest set bit
    n &= (n - 1);
  }
  return x1;
}

/// \brief Advance x2 LFSR by N steps using matrix exponentiation - O(log N)
__device__ uint32_t advance_x2_fast(uint32_t x2, int n)
{
  // Apply jump matrices for each set bit in n
  // Only check up to highest set bit (saves iterations for small n)
  while (n) {
    // Find lowest set bit (0-indexed)
    int p = __ffs(n) - 1;
    x2    = apply_jump_matrix(x2, d_x2_jump[p]);
    // Clear lowest set bit
    n &= (n - 1);
  }
  return x2;
}

// ============================================================================
// Device: Legacy O(N) Functions (kept for small N where overhead > benefit)
// ============================================================================

/// \brief Advance LFSR x1 by one step
/// x1(n+31) = (x1(n+3) + x1(n)) mod 2
__device__ __forceinline__ uint32_t advance_x1(uint32_t x1)
{
  uint32_t new_bit = ((x1 >> 3) ^ x1) & 1;
  return (x1 >> 1) | (new_bit << 30);
}

/// \brief Advance LFSR x2 by one step
/// x2(n+31) = (x2(n+3) + x2(n+2) + x2(n+1) + x2(n)) mod 2
__device__ __forceinline__ uint32_t advance_x2(uint32_t x2)
{
  uint32_t new_bit = ((x2 >> 3) ^ (x2 >> 2) ^ (x2 >> 1) ^ x2) & 1;
  return (x2 >> 1) | (new_bit << 30);
}

// ============================================================================
// Kernels
// ============================================================================

/// \brief Generate Gold sequence in packed uint32_t format - FAST VERSION
///
/// Uses matrix exponentiation for O(log N) LFSR advancement instead of O(N).
/// Each thread generates 32 consecutive bits (one word).
///
/// IMPORTANT: The bit ordering must match the CPU's pseudo_random_generator_impl.cpp.
/// The CPU uses MSB-first ordering within bytes, with byte swapping for 64-bit writes.
/// To match this, we use MSB-first ordering: bit 0 goes to position 31 (MSB),
/// bit 1 goes to position 30, etc. This matches how the CPU extracts bits using
/// (c & (1U << 31U)) and shifts left.
///
/// \param d_sequence Output sequence buffer
/// \param c_init Initialization value for x2 LFSR
/// \param num_words Number of 32-bit words to generate
/// \param bit_offset Starting bit offset (skipped bits before generation)
__global__ void __launch_bounds__(256, 4)
    gold_sequence_generate_kernel(uint32_t* __restrict__ d_sequence, uint32_t c_init, int num_words, int bit_offset)
{
  int word_idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (word_idx >= num_words)
    return;

  // Initialize LFSRs
  // x1(0) = 1, x1(n) = 0 for n = 1,...,30
  uint32_t x1 = 1;
  // x2 initialized from c_init (31 bits)
  uint32_t x2 = c_init & 0x7FFFFFFF;

  // Advance by Nc + bit_offset + word_idx * 32 steps
  // Using O(log N) matrix exponentiation instead of O(N) loop!
  int total_advance = NC_SKIP + bit_offset + word_idx * 32;

  x1 = advance_x1_fast(x1, total_advance);
  x2 = advance_x2_fast(x2, total_advance);

  // Generate 32 bits for this word (still O(32) but unavoidable)
  // Use MSB-first bit ordering to match CPU's pseudo_random_generator_impl.cpp:
  // - CPU extracts bit n using (c & (1U << 31U)) then shifts left with (c << 1U)
  // - So bit 0 is at position 31 (MSB), bit 1 at position 30, etc.
  // This ensures GPU scrambling sequence matches CPU bit-for-bit.
  uint32_t word = 0;
#pragma unroll
  for (int b = 0; b < 32; b++) {
    // c(n) = (x1(n) XOR x2(n))
    uint32_t c_bit = (x1 ^ x2) & 1;
    // MSB-first: bit b maps to position (31 - b)
    word |= (c_bit << (31 - b));

    // Advance LFSRs by 1 step (fast for single step)
    x1 = advance_x1(x1);
    x2 = advance_x2(x2);
  }

  d_sequence[word_idx] = word;
}

/// \brief Scramble bits using XOR (TX path)
///
/// output[i] = input[i] XOR sequence[i]
/// Operates on packed uint32_t words for efficiency.
__global__ void scramble_bits_kernel(const uint32_t* __restrict__ d_input,
                                     const uint32_t* __restrict__ d_sequence,
                                     uint32_t* __restrict__ d_output,
                                     int num_words)
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx >= num_words)
    return;

  d_output[idx] = d_input[idx] ^ d_sequence[idx];
}

/// \brief Scramble bits in-place using XOR (TX path)
__global__ void
scramble_bits_inplace_kernel(uint32_t* __restrict__ d_bits, const uint32_t* __restrict__ d_sequence, int num_words)
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx >= num_words)
    return;

  d_bits[idx] ^= d_sequence[idx];
}

/// \brief Descramble soft LLRs (RX path)
///
/// For soft decoding, flip sign of LLR where scrambling bit is 1.
/// output[i] = (sequence[i] == 1) ? -input[i] : input[i]
///
/// Uses MSB-first bit indexing to match gold_sequence_generate_kernel:
/// - Gold sequence stores bit b at position (31 - b) (MSB-first within words)
/// - To get sequence bit i, access word i/32, bit position (31 - i%32)
__global__ void descramble_llr_kernel(const float* __restrict__ d_input_llrs,
                                      const uint32_t* __restrict__ d_sequence,
                                      float* __restrict__ d_output_llrs,
                                      int num_bits)
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx >= num_bits)
    return;

  // MSB-first bit indexing to match gold sequence generator
  int      word_idx = idx / 32;
  int      bit_pos  = 31 - (idx % 32);
  uint32_t seq_bit  = (d_sequence[word_idx] >> bit_pos) & 1;

  float llr          = d_input_llrs[idx];
  d_output_llrs[idx] = seq_bit ? -llr : llr;
}

/// \brief Descramble soft LLRs in-place (RX path)
__global__ void
descramble_llr_inplace_kernel(float* __restrict__ d_llrs, const uint32_t* __restrict__ d_sequence, int num_bits)
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx >= num_bits)
    return;

  // MSB-first bit indexing to match gold sequence generator
  int      word_idx = idx / 32;
  int      bit_pos  = 31 - (idx % 32);
  uint32_t seq_bit  = (d_sequence[word_idx] >> bit_pos) & 1;

  if (seq_bit) {
    d_llrs[idx] = -d_llrs[idx];
  }
}

/// \brief Descramble half-precision (fp16) LLRs in-place (RX path)
///
/// For fp16, flip sign by XORing the sign bit (bit 15) directly.
/// This is more efficient than negation for fp16.
__global__ void
descramble_llr_half_inplace_kernel(__half* __restrict__ d_llrs, const uint32_t* __restrict__ d_sequence, int num_bits)
{
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx >= num_bits)
    return;

  // MSB-first bit indexing to match gold sequence generator
  int      word_idx = idx / 32;
  int      bit_pos  = 31 - (idx % 32);
  uint32_t seq_bit  = (d_sequence[word_idx] >> bit_pos) & 1;

  if (seq_bit) {
    // Flip sign bit of fp16 value (bit 15)
    unsigned short bits = __half_as_ushort(d_llrs[idx]);
    d_llrs[idx]         = __ushort_as_half(bits ^ 0x8000);
  }
}

// ============================================================================
// Sequence state
// ============================================================================

/// \brief Sequence a scrambler currently holds.
///
/// The buffer is either owned by this scrambler or borrowed from the process-wide cache, which is
/// what \c from_cache records.
struct sequence_state {
  uint32_t* d_sequence           = nullptr;
  size_t    sequence_alloc_words = 0;
  int       sequence_length_bits = 0;
  int       sequence_offset      = 0;
  bool      sequence_generated   = false;
  bool      sequence_from_cache  = false;
};

// Process-wide gold-sequence cache (device buffers live for process lifetime).
// Hot paths re-request the same (c_init, offset) keys across slots/handles; caching
// avoids repeated Gold-sequence kernel launches and device reallocations.
namespace {

constexpr int GOLD_SEQ_CACHE_ENTRIES = 128;
// Match transport_block MAX_OUTPUT_WORDS (+1 pad for kernel word_idx+1 reads).
constexpr size_t GOLD_SEQ_DEFAULT_WORDS = 65536 + 1;

struct gold_seq_cache_entry {
  uint32_t c_init = 0;
  int      offset = 0;
  // valid generated words
  int       num_words   = 0;
  uint32_t* d_sequence  = nullptr;
  size_t    alloc_words = 0;
  uint64_t  last_use    = 0;
  bool      valid       = false;
  // recorded after generation completes
  cudaEvent_t ready_event = nullptr;
};

std::mutex                                               g_gold_seq_cache_mutex;
std::array<gold_seq_cache_entry, GOLD_SEQ_CACHE_ENTRIES> g_gold_seq_cache{};
uint64_t                                                 g_gold_seq_cache_use_counter = 0;

// Device buffers detached from a cache slot (on grow or LRU eviction) that a consumer
// kernel enqueued by another thread may still be reading. The buffer is handed to
// callers as a borrow and consumed asynchronously on their stream, so freeing or
// overwriting it inline would be a device-side use-after-free / data corruption.
// Detached buffers are parked here and reclaimed in bulk only once the device is known
// idle. Steady-state working sets that fit in the cache never detach a buffer, so this
// stays empty; it only grows under sustained churn of >GOLD_SEQ_CACHE_ENTRIES keys.
std::vector<uint32_t*> g_gold_seq_retired;
constexpr size_t       GOLD_SEQ_RETIRE_RECLAIM_THRESHOLD = 64;

// Park a buffer that has been detached from its slot but may still be in flight.
// Caller must hold g_gold_seq_cache_mutex.
void retire_gold_seq_buffer(uint32_t* buf)
{
  if (buf == nullptr)
    return;

  g_gold_seq_retired.push_back(buf);
  if (g_gold_seq_retired.size() < GOLD_SEQ_RETIRE_RECLAIM_THRESHOLD)
    return;

  // A device-wide sync guarantees every previously enqueued consumer kernel has
  // completed, so no retired buffer can still be read. This only runs on the rare
  // churn path (never in steady state), so the sync cost is amortized to nothing.
  if (cudaDeviceSynchronize() != cudaSuccess) {
    (void)cudaGetLastError();
    // leave buffers parked; retry reclaim on a later retirement
    return;
  }

  for (uint32_t* retired : g_gold_seq_retired)
    cudaFree(retired);

  g_gold_seq_retired.clear();
}

// Ensure a cache entry has a device buffer of at least `num_words` words (grow-only).
bool ensure_cache_entry_capacity(gold_seq_cache_entry& entry, int num_words)
{
  if (num_words <= 0) {
    return false;
  }
  if (entry.alloc_words >= static_cast<size_t>(num_words) && entry.d_sequence != nullptr) {
    return true;
  }
  // Grow: allocate a new buffer, copy any existing content, free old. Only on miss path.
  uint32_t*    new_buf = nullptr;
  const size_t alloc_words =
      std::max(static_cast<size_t>(num_words), entry.alloc_words > 0 ? entry.alloc_words * 2 : GOLD_SEQ_DEFAULT_WORDS);
  if (cudaMalloc(&new_buf, alloc_words * sizeof(uint32_t)) != cudaSuccess) {
    return false;
  }
  if (entry.d_sequence != nullptr && entry.num_words > 0) {
    (void)cudaMemcpy(
        new_buf, entry.d_sequence, static_cast<size_t>(entry.num_words) * sizeof(uint32_t), cudaMemcpyDeviceToDevice);
    // The old buffer may still be borrowed by an in-flight consumer; park it
    // instead of freeing it now (see retire_gold_seq_buffer).
    retire_gold_seq_buffer(entry.d_sequence);
  } else if (entry.d_sequence != nullptr) {
    retire_gold_seq_buffer(entry.d_sequence);
  }
  entry.d_sequence  = new_buf;
  entry.alloc_words = alloc_words;
  return true;
}

// Look up an existing valid cache entry. Caller must hold g_gold_seq_cache_mutex.
gold_seq_cache_entry* find_gold_seq_cache_entry(uint32_t c_init, int offset, int num_words)
{
  for (auto& entry : g_gold_seq_cache) {
    if (entry.valid && entry.c_init == c_init && entry.offset == offset && entry.num_words >= num_words &&
        entry.d_sequence != nullptr) {
      entry.last_use = ++g_gold_seq_cache_use_counter;
      return &entry;
    }
  }
  return nullptr;
}

// Reserve an entry for (c_init, offset). Caller must hold the mutex.
gold_seq_cache_entry* reserve_gold_seq_cache_entry(uint32_t c_init, int offset)
{
  // Prefer empty/invalid slot.
  for (auto& entry : g_gold_seq_cache) {
    if (!entry.valid) {
      entry.c_init    = c_init;
      entry.offset    = offset;
      entry.num_words = 0;
      // becomes true after generation
      entry.valid    = false;
      entry.last_use = ++g_gold_seq_cache_use_counter;
      return &entry;
    }
  }
  // Prefer exact key match to grow in place.
  for (auto& entry : g_gold_seq_cache) {
    if (entry.c_init == c_init && entry.offset == offset) {
      entry.last_use = ++g_gold_seq_cache_use_counter;
      return &entry;
    }
  }
  // LRU eviction. The victim still holds a different key's sequence, and a consumer
  // kernel from another thread may still be reading that buffer, so it cannot be
  // reused in place (that would overwrite live data). Detach and park the buffer,
  // then let ensure_cache_entry_capacity allocate a fresh one for the new key.
  gold_seq_cache_entry* victim = &g_gold_seq_cache[0];
  for (auto& entry : g_gold_seq_cache) {
    if (entry.last_use < victim->last_use) {
      victim = &entry;
    }
  }
  retire_gold_seq_buffer(victim->d_sequence);
  victim->d_sequence  = nullptr;
  victim->alloc_words = 0;
  victim->c_init      = c_init;
  victim->offset      = offset;
  victim->num_words   = 0;
  victim->valid       = false;
  victim->last_use    = ++g_gold_seq_cache_use_counter;
  return victim;
}

// Generate (or fetch) sequence for (c_init, offset) with at least num_words.
// On success, ctx points at the shared cache buffer (borrowed).
// Caller's stream is made to wait on the entry's ready event when needed (no global sync).
cuda_result
gold_seq_get_or_generate(sequence_state* ctx, uint32_t c_init, int offset, int num_words, cudaStream_t stream)
{
  if ((ctx == nullptr) || (num_words <= 0) || (offset < 0)) {
    return make_unexpected(std::string("The scrambling sequence request carries an invalid configuration"));
  }

  uint32_t*   d_seq       = nullptr;
  size_t      alloc_words = 0;
  cudaEvent_t ready       = nullptr;

  {
    std::lock_guard<std::mutex> lock(g_gold_seq_cache_mutex);

    gold_seq_cache_entry* entry = find_gold_seq_cache_entry(c_init, offset, num_words);
    if (entry == nullptr) {
      // Miss: reserve a slot and generate under the same lock. Keeping reserve, generation and
      // publish in a single critical section prevents another thread from re-grabbing this still
      // pending slot and overwriting the buffer about to be handed out as a borrow. Generation was
      // already serialised under this mutex, so this costs no further concurrency.
      entry = reserve_gold_seq_cache_entry(c_init, offset);
      if ((entry == nullptr) || !ensure_cache_entry_capacity(*entry, num_words)) {
        return make_unexpected(std::string("The scrambling sequence cache could not allocate a buffer"));
      }
      if (entry->ready_event == nullptr) {
        cuda_result status =
            check_cuda_error(static_cast<int>(cudaEventCreateWithFlags(&entry->ready_event, cudaEventDisableTiming)),
                             "scrambling sequence event creation");
        if (!status.has_value()) {
          return status;
        }
      }
      if (g_jump_tables_available) {
        const int block_size = 256;
        const int num_blocks = (num_words + block_size - 1) / block_size;
        gold_sequence_generate_kernel<<<num_blocks, block_size, 0, stream>>>(
            entry->d_sequence, c_init, num_words, offset);

        cuda_result status = check_last_cuda_error("scrambling sequence generation");
        if (!status.has_value()) {
          return status;
        }

        status =
            check_cuda_error(static_cast<int>(cudaEventRecord(entry->ready_event, (stream != nullptr) ? stream : 0)),
                             "scrambling sequence event record");
        if (!status.has_value()) {
          return status;
        }
      } else {
        // The jump tables did not reach constant memory, so the sequence is built on the host and
        // uploaded instead.
        std::vector<uint32_t> h_sequence;
        generate_gold_sequence_host(h_sequence, c_init, num_words, offset);

        cuda_result status =
            check_cuda_error(static_cast<int>(cudaMemcpy(entry->d_sequence,
                                                         h_sequence.data(),
                                                         static_cast<size_t>(num_words) * sizeof(uint32_t),
                                                         cudaMemcpyHostToDevice)),
                             "scrambling sequence upload");
        if (!status.has_value()) {
          return status;
        }

        status = check_cuda_error(static_cast<int>(cudaEventRecord(entry->ready_event, 0)),
                                  "scrambling sequence event record");
        if (!status.has_value()) {
          return status;
        }
      }
      entry->c_init    = c_init;
      entry->offset    = offset;
      entry->num_words = num_words;
      entry->valid     = true;
    }

    d_seq       = entry->d_sequence;
    alloc_words = entry->alloc_words;
    ready       = entry->ready_event;
  }

  if (d_seq == nullptr) {
    return make_unexpected(std::string("The scrambling sequence cache could not allocate a buffer"));
  }

  // Make the consumer stream wait on the generation rather than synchronising the whole device.
  if ((ready != nullptr) && (stream != nullptr)) {
    cuda_result status =
        check_cuda_error(static_cast<int>(cudaStreamWaitEvent(stream, ready, 0)), "waiting on the scrambling sequence");
    if (!status.has_value()) {
      return status;
    }
  } else if (ready != nullptr) {
    cuda_result status =
        check_cuda_error(static_cast<int>(cudaEventSynchronize(ready)), "waiting on the scrambling sequence");
    if (!status.has_value()) {
      return status;
    }
  }

  ctx->d_sequence           = d_seq;
  ctx->sequence_alloc_words = alloc_words;
  ctx->sequence_length_bits = num_words * 32;
  ctx->sequence_offset      = offset;
  ctx->sequence_generated   = true;
  ctx->sequence_from_cache  = true;

  return {};
}

} // namespace

// ============================================================================
// Scrambler
// ============================================================================

/// State a scrambler carries between calls.
struct scrambler::impl {
  /// Value the sequence is initialised from, as per TS 38.211, Section 5.2.1.
  uint32_t c_init = 0;
  /// Bit the next generated sequence starts at.
  int current_offset = 0;
  /// Sequence generated so far, if any.
  sequence_state sequence;
};

/// Number of threads a scrambling kernel runs per block.
static constexpr unsigned block_size = 256;

/// Number of blocks that covers the given number of items.
static unsigned blocks_for(unsigned nof_items)
{
  return (nof_items + block_size - 1) / block_size;
}

cuda_expected<scrambler> scrambler::create()
{
  cuda_result status = check_cuda_error(static_cast<int>(initialize_jump_tables()), "scrambler jump table upload");
  if (!status.has_value()) {
    return make_unexpected(status.error());
  }

  std::unique_ptr<impl> created(new (std::nothrow) impl());
  if (created == nullptr) {
    return make_unexpected(std::string("The scrambler state could not be allocated"));
  }

  // Give the cache a default sized buffer now, so the first slot encoded does not pay for a cold
  // allocation. The sequence itself waits until the initialiser is known.
  {
    std::lock_guard<std::mutex> lock(g_gold_seq_cache_mutex);
    for (gold_seq_cache_entry& entry : g_gold_seq_cache) {
      if (entry.d_sequence == nullptr) {
        (void)ensure_cache_entry_capacity(entry, static_cast<int>(GOLD_SEQ_DEFAULT_WORDS));
        break;
      }
    }
  }

  return scrambler(std::move(created));
}

scrambler::scrambler() = default;

scrambler::scrambler(std::unique_ptr<impl> state_) : state(std::move(state_)) {}

scrambler::scrambler(scrambler&& other) noexcept = default;

scrambler& scrambler::operator=(scrambler&& other) noexcept = default;

scrambler::~scrambler()
{
  // A buffer the cache owns outlives every scrambler that borrowed it.
  if ((state != nullptr) && (state->sequence.d_sequence != nullptr) && !state->sequence.sequence_from_cache) {
    (void)cudaFree(state->sequence.d_sequence);
  }
}

bool scrambler::is_valid() const
{
  return state != nullptr;
}

void scrambler::init(uint32_t c_init)
{
  // Re-initialising with the value already in place keeps the sequence, which is what lets a
  // scrambler reused across slots hit the cache.
  if (state->c_init != c_init) {
    state->c_init                      = c_init;
    state->sequence.sequence_generated = false;
  }

  state->current_offset = 0;
}

uint32_t scrambler::get_init() const
{
  return state->c_init;
}

void scrambler::advance(unsigned count)
{
  state->current_offset += static_cast<int>(count);
  state->sequence.sequence_generated = false;
}

void scrambler::set_offset(unsigned offset)
{
  state->current_offset              = static_cast<int>(offset);
  state->sequence.sequence_generated = false;
}

unsigned scrambler::get_offset() const
{
  return static_cast<unsigned>(state->current_offset);
}

const uint32_t* scrambler::sequence() const
{
  return state->sequence.sequence_generated ? state->sequence.d_sequence : nullptr;
}

cuda_result scrambler::generate(unsigned nof_bits, const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  if (nof_bits == 0) {
    return make_unexpected(std::string("The scrambling sequence request carries an invalid configuration"));
  }

  // The sequence in hand serves if it is long enough and starts where the next call expects.
  if (state->sequence.sequence_generated && (state->sequence.sequence_length_bits >= static_cast<int>(nof_bits)) &&
      (state->sequence.sequence_offset == state->current_offset) && (state->sequence.d_sequence != nullptr)) {
    return {};
  }

  // A kernel reading a bit span that crosses a word boundary reads 1 word past the last, so the
  // buffer carries a padding word.
  int nof_words = static_cast<int>((nof_bits + 31) / 32) + 1;

  // Let go of a privately owned buffer before borrowing one from the cache.
  if ((state->sequence.d_sequence != nullptr) && !state->sequence.sequence_from_cache) {
    (void)cudaFree(state->sequence.d_sequence);
    state->sequence.d_sequence           = nullptr;
    state->sequence.sequence_alloc_words = 0;
  }

  return gold_seq_get_or_generate(&state->sequence, state->c_init, state->current_offset, nof_words, native_stream);
}

cuda_result scrambler::reserve(unsigned max_bits)
{
  if (max_bits == 0) {
    return make_unexpected(std::string("The scrambling sequence request carries an invalid configuration"));
  }

  int nof_words = static_cast<int>((max_bits + 31) / 32) + 1;

  std::lock_guard<std::mutex> lock(g_gold_seq_cache_mutex);
  for (gold_seq_cache_entry& entry : g_gold_seq_cache) {
    if (entry.d_sequence == nullptr) {
      if (!ensure_cache_entry_capacity(entry, nof_words)) {
        return make_unexpected(std::string("The scrambling sequence cache could not allocate a buffer"));
      }
      return {};
    }
  }

  return {};
}

cuda_result scrambler::ensure_sequence(unsigned nof_bits, const cuda_stream& stream)
{
  if (!state->sequence.sequence_generated || (state->sequence.sequence_length_bits < static_cast<int>(nof_bits)) ||
      (state->sequence.sequence_offset != state->current_offset)) {
    return generate(nof_bits, stream);
  }

  return {};
}

cuda_result
scrambler::apply_xor(uint32_t* d_output, const uint32_t* d_input, unsigned nof_bits, const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  if ((d_input == nullptr) || (d_output == nullptr) || (nof_bits == 0)) {
    return make_unexpected(std::string("The scrambling call carries an invalid configuration"));
  }

  cuda_result status = ensure_sequence(nof_bits, stream);
  if (!status.has_value()) {
    return status;
  }

  unsigned nof_words = (nof_bits + 31) / 32;
  scramble_bits_kernel<<<blocks_for(nof_words), block_size, 0, native_stream>>>(
      d_input, state->sequence.d_sequence, d_output, static_cast<int>(nof_words));

  return check_last_cuda_error("bit scrambling");
}

cuda_result scrambler::apply_xor(uint32_t* d_bits, unsigned nof_bits, const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  if ((d_bits == nullptr) || (nof_bits == 0)) {
    return make_unexpected(std::string("The scrambling call carries an invalid configuration"));
  }

  cuda_result status = ensure_sequence(nof_bits, stream);
  if (!status.has_value()) {
    return status;
  }

  unsigned nof_words = (nof_bits + 31) / 32;
  scramble_bits_inplace_kernel<<<blocks_for(nof_words), block_size, 0, native_stream>>>(
      d_bits, state->sequence.d_sequence, static_cast<int>(nof_words));

  return check_last_cuda_error("bit scrambling in place");
}

cuda_result scrambler::apply_xor(float* d_output, const float* d_input, unsigned nof_bits, const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  if ((d_input == nullptr) || (d_output == nullptr) || (nof_bits == 0)) {
    return make_unexpected(std::string("The descrambling call carries an invalid configuration"));
  }

  cuda_result status = ensure_sequence(nof_bits, stream);
  if (!status.has_value()) {
    return status;
  }

  descramble_llr_kernel<<<blocks_for(nof_bits), block_size, 0, native_stream>>>(
      d_input, state->sequence.d_sequence, d_output, static_cast<int>(nof_bits));

  return check_last_cuda_error("descrambling of log likelihood ratios");
}

cuda_result scrambler::apply_xor(float* d_llrs, unsigned nof_bits, const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  if ((d_llrs == nullptr) || (nof_bits == 0)) {
    return make_unexpected(std::string("The descrambling call carries an invalid configuration"));
  }

  cuda_result status = ensure_sequence(nof_bits, stream);
  if (!status.has_value()) {
    return status;
  }

  descramble_llr_inplace_kernel<<<blocks_for(nof_bits), block_size, 0, native_stream>>>(
      d_llrs, state->sequence.d_sequence, static_cast<int>(nof_bits));

  return check_last_cuda_error("descrambling of log likelihood ratios in place");
}

cuda_result scrambler::apply_xor(__half* d_llrs, unsigned nof_bits, const cuda_stream& stream)
{
  ::cudaStream_t native_stream = static_cast<::cudaStream_t>(stream.native());

  if ((d_llrs == nullptr) || (nof_bits == 0)) {
    return make_unexpected(std::string("The descrambling call carries an invalid configuration"));
  }

  cuda_result status = ensure_sequence(nof_bits, stream);
  if (!status.has_value()) {
    return status;
  }

  descramble_llr_half_inplace_kernel<<<blocks_for(nof_bits), block_size, 0, native_stream>>>(
      d_llrs, state->sequence.d_sequence, static_cast<int>(nof_bits));

  return check_last_cuda_error("descrambling of half precision log likelihood ratios in place");
}

} // namespace cuda
} // namespace ocudu
