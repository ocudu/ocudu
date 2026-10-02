// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "vkFFT.h"
#include "ocudu/cuda/adt/cuda_copy.h"
#include "ocudu/cuda/adt/cuda_event.h"
#include "ocudu/cuda/adt/device_vector.h"
#include "ocudu/cuda/prach/prach_detector_backend.h"
#include <cstring>
#include <cuComplex.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <vector>

using namespace ocudu;
using namespace ocudu::cuda;

namespace {

/// Number of threads per block used by both detector kernels. A multiple of the warp size, so the
/// reductions below can assume full warps.
constexpr int THREADS = 256;

/// Largest number of port and symbol batches a single root sequence may be searched over. It bounds
/// the per-block shared memory holding one reference power per batch.
constexpr int MAX_BATCHES_PER_SEQUENCE = 128;

/// Number of candidate slots, one per preamble.
constexpr int MAX_CANDIDATES = static_cast<int>(prach_constants::MAX_NUM_PREAMBLES);

/// \brief Geometry as consumed by the kernels.
///
/// The public geometry uses unsigned values, while the kernels index with signed arithmetic that has
/// to stay negative in places, so the values are converted once on the host.
struct kernel_config {
  int   sequence_length;
  int   dft_size;
  int   nof_rx_ports;
  int   nof_symbols;
  int   nof_sequences;
  int   sequence_start;
  int   nof_shifts;
  int   n_cs;
  int   win_width;
  int   win_margin;
  int   max_delay_samples;
  int   start_preamble_index;
  int   nof_preamble_indices;
  int   combine_symbols;
  int   input_symbol_stride;
  int   input_port_stride;
  float threshold;
};

/// Candidate as written by the search kernel, with the validity flag the host filters on.
struct device_candidate {
  int   valid;
  int   preamble_index;
  int   delay_samples;
  float detection_metric;
  float preamble_power;
};

/// \brief Results of one detection.
///
/// Keeping the candidate list and the RSSI accumulator contiguous allows a single readback
/// per detection instead of one per member.
struct result_block {
  device_candidate candidates[MAX_CANDIDATES];
  float            rssi;
};

} // namespace

static __device__ __forceinline__ float bf16_to_float(uint16_t v)
{
  union {
    uint32_t u;
    float    f;
  } conv;
  conv.u = static_cast<uint32_t>(v) << 16;
  return conv.f;
}

static __device__ __forceinline__ cuFloatComplex cbf16_to_cfloat(uint32_t packed)
{
  return make_cuFloatComplex(bf16_to_float(static_cast<uint16_t>(packed & 0xffffu)),
                             bf16_to_float(static_cast<uint16_t>(packed >> 16)));
}

static __device__ __forceinline__ cuFloatComplex cmul_conj(cuFloatComplex x, cuFloatComplex y)
{
  return make_cuFloatComplex(x.x * y.x + x.y * y.y, x.y * y.x - x.x * y.y);
}

static __device__ __forceinline__ float cabs2(cuFloatComplex x)
{
  return x.x * x.x + x.y * x.y;
}

static __device__ __forceinline__ float warp_reduce_sum(float value)
{
  // Full-mask warp reduction, valid because THREADS is a multiple of the warp size.
  value += __shfl_down_sync(0xffffffffu, value, 16);
  value += __shfl_down_sync(0xffffffffu, value, 8);
  value += __shfl_down_sync(0xffffffffu, value, 4);
  value += __shfl_down_sync(0xffffffffu, value, 2);
  value += __shfl_down_sync(0xffffffffu, value, 1);
  return value;
}

/// Reduces the largest metric of a warp together with the power and delay it was measured at.
static __device__ __forceinline__ void warp_reduce_max_metric(float& metric, float& power, int& delay)
{
  for (int offset = 16; offset != 0; offset >>= 1) {
    float other_metric = __shfl_down_sync(0xffffffffu, metric, offset);
    float other_power  = __shfl_down_sync(0xffffffffu, power, offset);
    int   other_delay  = __shfl_down_sync(0xffffffffu, delay, offset);
    if (other_metric > metric) {
      metric = other_metric;
      power  = other_power;
      delay  = other_delay;
    }
  }
}

/// \brief Maps the received samples onto the inverse DFT input, correlating them with the roots.
///
/// The kernel also accumulates the signal power (RSSI). Every root sequence reads the same received
// samples, so only the blocks handling root sequence 0 accumulate. Letting all of them accumulate
// would count each sample once per root sequence.
static __global__ void kernel_prepare_idft(const uint32_t* __restrict__ input,
                                           const cuFloatComplex* __restrict__ roots,
                                           cuFloatComplex* __restrict__ idft,
                                           float* __restrict__ rssi,
                                           kernel_config cfg,
                                           int           batches_per_sequence)
{
  __shared__ float rssi_partial[THREADS];

  int local_sequence    = blockIdx.y;
  int batch_in_sequence = blockIdx.x;
  int freq_bin          = threadIdx.x + blockIdx.z * blockDim.x;
  if (local_sequence >= cfg.nof_sequences || batch_in_sequence >= batches_per_sequence) {
    return;
  }

  int port   = 0;
  int symbol = 0;
  if (cfg.combine_symbols) {
    port = batch_in_sequence;
  } else {
    port   = batch_in_sequence / cfg.nof_symbols;
    symbol = batch_in_sequence - port * cfg.nof_symbols;
  }

  int half    = cfg.sequence_length / 2;
  int root_re = -1;
  if (freq_bin < cfg.sequence_length - half) {
    root_re = freq_bin + half;
  } else if (freq_bin >= cfg.dft_size - half && freq_bin < cfg.dft_size) {
    root_re = freq_bin - (cfg.dft_size - half);
  }

  cuFloatComplex sample   = make_cuFloatComplex(0.0f, 0.0f);
  float          rssi_sum = 0.0f;
  if (root_re >= 0) {
    if (cfg.combine_symbols) {
      for (int i_symbol = 0; i_symbol != cfg.nof_symbols; ++i_symbol) {
        uint32_t       packed = input[port * cfg.input_port_stride + i_symbol * cfg.input_symbol_stride + root_re];
        cuFloatComplex v      = cbf16_to_cfloat(packed);
        sample.x += v.x;
        sample.y += v.y;
        rssi_sum += cabs2(v);
      }
    } else {
      sample   = cbf16_to_cfloat(input[port * cfg.input_port_stride + symbol * cfg.input_symbol_stride + root_re]);
      rssi_sum = cabs2(sample);
    }
  }

  if (local_sequence == 0) {
    // Warp reduction followed by one shared step across warps, which needs fewer barriers than a
    // shared memory tree.
    float warp_sum = warp_reduce_sum(rssi_sum);
    if ((threadIdx.x & 31) == 0) {
      rssi_partial[threadIdx.x >> 5] = warp_sum;
    }
    __syncthreads();
    if (threadIdx.x < 32) {
      constexpr int nof_warps = THREADS / 32;
      float         block_sum = (threadIdx.x < nof_warps) ? rssi_partial[threadIdx.x] : 0.0f;
      block_sum               = warp_reduce_sum(block_sum);
      if (threadIdx.x == 0) {
        atomicAdd(rssi, block_sum);
      }
    }
  }

  int batch = local_sequence * batches_per_sequence + batch_in_sequence;
  if (root_re < 0) {
    if (freq_bin < cfg.dft_size) {
      idft[batch * cfg.dft_size + freq_bin] = make_cuFloatComplex(0.0f, 0.0f);
    }
    return;
  }

  idft[batch * cfg.dft_size + freq_bin] = cmul_conj(sample, roots[local_sequence * cfg.sequence_length + root_re]);
}

/// \brief Searches the correlation window of every cyclic shift for the strongest delay.
///
/// One block searches one preamble, so the candidate slots are written without contention.
static __global__ void kernel_find_candidates(const cuFloatComplex* __restrict__ idft,
                                              device_candidate* __restrict__ candidates,
                                              kernel_config cfg,
                                              int           batches_per_sequence)
{
  __shared__ float best_metric[THREADS];
  __shared__ float best_power[THREADS];
  __shared__ float reference_power[MAX_BATCHES_PER_SEQUENCE];
  __shared__ float reduction_scratch[THREADS];
  __shared__ int   best_delay[THREADS];

  int local_sequence = blockIdx.y;
  int sequence       = cfg.sequence_start + local_sequence;
  int window         = blockIdx.x;
  int preamble_index = sequence * cfg.nof_shifts + window;
  int out_index      = preamble_index;

  float local_metric = -1.0f;
  float local_power  = 0.0f;
  int   local_delay  = 0;

  if ((preamble_index < MAX_CANDIDATES) && (preamble_index >= cfg.start_preamble_index) &&
      (preamble_index < cfg.start_preamble_index + cfg.nof_preamble_indices)) {
    // The window placement follows the generic detector so that both report the same delay:
    //   window_start = (dft_size - (n_cs * window * dft_size) / sequence_length) % dft_size
    // and a delay d maps to the sample at (window_start + d) % dft_size.
    int   window_start = (cfg.dft_size - ((cfg.n_cs * window * cfg.dft_size) / cfg.sequence_length)) % cfg.dft_size;
    float mod_scale    = 1.0f / static_cast<float>(cfg.dft_size * cfg.sequence_length);
    float window_scale = static_cast<float>(cfg.dft_size) / static_cast<float>(cfg.sequence_length);
    int   ref_start    = (window_start + cfg.dft_size - cfg.win_margin) % cfg.dft_size;
    int   ref_len      = 2 * cfg.win_margin + cfg.win_width;

    for (int b = 0; b != batches_per_sequence; ++b) {
      const cuFloatComplex* corr    = idft + (local_sequence * batches_per_sequence + b) * cfg.dft_size;
      float                 partial = 0.0f;
      for (int i = threadIdx.x; i < ref_len; i += blockDim.x) {
        int idx = (ref_start + i) % cfg.dft_size;
        partial += cabs2(corr[idx]) * mod_scale;
      }

      float warp_sum = warp_reduce_sum(partial);
      if ((threadIdx.x & 31) == 0) {
        reduction_scratch[threadIdx.x >> 5] = warp_sum;
      }
      __syncthreads();
      if (threadIdx.x < 32) {
        constexpr int nof_warps = THREADS / 32;
        float         block_sum = (threadIdx.x < nof_warps) ? reduction_scratch[threadIdx.x] : 0.0f;
        block_sum               = warp_reduce_sum(block_sum);
        if (threadIdx.x == 0) {
          reference_power[b] = block_sum;
        }
      }
      __syncthreads();
    }

    for (int delay = threadIdx.x; delay < cfg.win_width; delay += blockDim.x) {
      float numerator   = 0.0f;
      float denominator = 0.0f;

      for (int b = 0; b != batches_per_sequence; ++b) {
        const cuFloatComplex* corr       = idft + (local_sequence * batches_per_sequence + b) * cfg.dft_size;
        int                   sample_idx = window_start + delay;
        if (sample_idx >= cfg.dft_size) {
          sample_idx -= cfg.dft_size;
        }

        float window_power = cabs2(corr[sample_idx]) * mod_scale * window_scale;
        // The noise estimate mirrors the generic detector. The floor is an explicit comparison
        // against the smallest normal float rather than isnormal() which on the device flags almost
        // every difference as subnormal and collapses the denominator, inflating the metric.
        float           diff    = reference_power[b] - window_power;
        constexpr float flt_min = 1.1754943508222875e-38f;
        if (!isfinite(diff) || diff == 0.0f || fabsf(diff) < flt_min) {
          diff = 1e-9f;
        }
        numerator += window_power;
        denominator += diff;
      }

      denominator = fabsf(denominator);
      if (!isfinite(denominator) || denominator <= 0.0f) {
        denominator = 1e-9f;
      }
      float metric = numerator / denominator;
      if (metric > local_metric) {
        local_metric = metric;
        local_power  = numerator;
        local_delay  = delay;
      }
    }
  }

  warp_reduce_max_metric(local_metric, local_power, local_delay);
  if ((threadIdx.x & 31) == 0) {
    best_metric[threadIdx.x >> 5] = local_metric;
    best_power[threadIdx.x >> 5]  = local_power;
    best_delay[threadIdx.x >> 5]  = local_delay;
  }
  __syncthreads();

  if (threadIdx.x < 32) {
    constexpr int nof_warps = THREADS / 32;
    float         m         = (threadIdx.x < nof_warps) ? best_metric[threadIdx.x] : -1.0f;
    float         p         = (threadIdx.x < nof_warps) ? best_power[threadIdx.x] : 0.0f;
    int           d         = (threadIdx.x < nof_warps) ? best_delay[threadIdx.x] : 0;
    warp_reduce_max_metric(m, p, d);
    if (threadIdx.x == 0) {
      best_metric[0] = m;
      best_power[0]  = p;
      best_delay[0]  = d;
    }
  }
  __syncthreads();

  if (threadIdx.x == 0 && out_index < MAX_CANDIDATES) {
    device_candidate out{};
    float            max_delay_gate = static_cast<float>(cfg.max_delay_samples) * 0.8f;
    if (best_metric[0] > cfg.threshold && static_cast<float>(best_delay[0]) < max_delay_gate) {
      int power_norm = cfg.nof_rx_ports * cfg.sequence_length * cfg.nof_symbols;
      if (cfg.combine_symbols) {
        power_norm *= cfg.nof_symbols;
      }
      out.valid            = 1;
      out.preamble_index   = preamble_index;
      out.delay_samples    = best_delay[0];
      out.detection_metric = best_metric[0] / cfg.threshold;
      out.preamble_power   = best_power[0] / static_cast<float>(power_norm);
    }
    candidates[out_index] = out;
  }
}

/// \brief Converts prach_detector_geometry into kernel_config.
///
/// kernel_config holds the same values as signed integers. The kernels mark a frequency bin that
/// carries no root sample with -1, and an unsigned index cannot represent that.
///
/// input_symbol_stride and input_port_stride are not part of prach_detector_geometry. The caller
/// uploads the samples contiguously, so two consecutive symbols of one port are sequence_length
/// elements apart, and two consecutive ports are nof_symbols * sequence_length elements apart.
static kernel_config make_kernel_config(const prach_detector_geometry& geometry)
{
  kernel_config cfg;
  cfg.sequence_length      = static_cast<int>(geometry.sequence_length);
  cfg.dft_size             = static_cast<int>(geometry.dft_size);
  cfg.nof_rx_ports         = static_cast<int>(geometry.nof_rx_ports);
  cfg.nof_symbols          = static_cast<int>(geometry.nof_symbols);
  cfg.nof_sequences        = static_cast<int>(geometry.nof_sequences);
  cfg.sequence_start       = static_cast<int>(geometry.sequence_start);
  cfg.nof_shifts           = static_cast<int>(geometry.nof_shifts);
  cfg.n_cs                 = static_cast<int>(geometry.n_cs);
  cfg.win_width            = static_cast<int>(geometry.win_width);
  cfg.win_margin           = static_cast<int>(geometry.win_margin);
  cfg.max_delay_samples    = static_cast<int>(geometry.max_delay_samples);
  cfg.start_preamble_index = static_cast<int>(geometry.start_preamble_index);
  cfg.nof_preamble_indices = static_cast<int>(geometry.nof_preamble_indices);
  cfg.combine_symbols      = geometry.combine_symbols ? 1 : 0;
  cfg.input_symbol_stride  = cfg.sequence_length;
  cfg.input_port_stride    = cfg.nof_symbols * cfg.sequence_length;
  cfg.threshold            = geometry.threshold;
  return cfg;
}

/// Returns a description of what makes the geometry unusable, or an empty string if the geometry is valid.
static std::string validate(const prach_detector_geometry& geometry, span<const uint32_t> input, span<const cf_t> roots)
{
  if ((geometry.sequence_length == 0) || (geometry.dft_size < geometry.sequence_length)) {
    return "Invalid PRACH sequence length or inverse DFT size";
  }
  if ((geometry.nof_rx_ports == 0) || (geometry.nof_symbols == 0) || (geometry.nof_sequences == 0) ||
      (geometry.nof_shifts == 0)) {
    return "The PRACH geometry searches no ports, symbols, sequences or shifts";
  }
  if (geometry.sequence_start + geometry.nof_sequences > prach_constants::MAX_NUM_PREAMBLES) {
    return "The PRACH sequence range exceeds the number of preambles";
  }
  if ((geometry.win_width == 0) || (geometry.threshold <= 0.0F)) {
    return "Invalid PRACH search window or detection threshold";
  }

  unsigned batches_per_sequence = geometry.nof_rx_ports * (geometry.combine_symbols ? 1U : geometry.nof_symbols);
  if (batches_per_sequence > static_cast<unsigned>(MAX_BATCHES_PER_SEQUENCE)) {
    return "The PRACH geometry combines more port and symbol batches than the detector supports";
  }

  if (input.size() < static_cast<size_t>(geometry.nof_rx_ports) * geometry.nof_symbols * geometry.sequence_length) {
    return "The PRACH input is smaller than the geometry requires";
  }
  if (roots.size() < static_cast<size_t>(geometry.nof_sequences) * geometry.sequence_length) {
    return "The PRACH root sequences are shorter than the geometry requires";
  }

  return {};
}

/// \brief Device state of one detector.
///
/// The blocks are grown to the largest geometry seen so far and kept for the lifetime of the
/// detector, so that a steady stream of detections of the same shape performs no allocation.
class prach_detector_backend::impl
{
public:
  ~impl()
  {
    if (vkfft_valid) {
      deleteVkFFT(&vkfft_app);
    }
  }

  /// Takes ownership of the event the detections are timed with.
  void set_done_event(cuda_event event) { done_event = std::move(event); }

  cuda_result detect(prach_detector_output&         output,
                     span<const uint32_t>           input,
                     span<const cf_t>               roots,
                     const prach_detector_geometry& geometry,
                     const cuda_stream&             stream);

private:
  /// Reallocates a block when it is too small for the number of elements requested.
  template <typename T>
  cuda_result ensure_capacity(device_vector<T>& block, size_t nof_elements)
  {
    if (block.size() >= nof_elements) {
      return {};
    }

    cuda_expected<device_vector<T>> allocated = device_vector<T>::create(nof_elements);
    if (!allocated.has_value()) {
      return make_unexpected(allocated.error());
    }
    block = std::move(allocated.value());
    return {};
  }

  /// Builds, or rebuilds, the inverse DFT plan when the transform shape or the stream changes.
  cuda_result ensure_plan(int dft_size, int batch, const cuda_stream& stream);

  device_vector<uint32_t>       d_input;
  device_vector<cuFloatComplex> d_roots;
  device_vector<cuFloatComplex> d_idft;
  device_vector<result_block>   d_results;
  result_block                  h_results = {};

  VkFFTApplication vkfft_app         = {};
  CUdevice         vkfft_device      = 0;
  ::cudaStream_t   vkfft_stream      = nullptr;
  void*            vkfft_buffer      = nullptr;
  pfUINT           vkfft_buffer_size = 0;
  int              vkfft_dft_size    = 0;
  int              vkfft_batch       = 0;
  bool             vkfft_valid       = false;

  /// Shape of the root sequences currently held on the device so that an unchanged set is not
  /// uploaded again.
  size_t cached_roots_size = 0;

  /// Contents of the root sequences currently held on the device.
  std::vector<cf_t> cached_roots;

  /// \brief Event marking the end of one detection.
  ///
  /// Held for the lifetime of the backend: creating an event allocates, and a detection runs on a
  /// real-time thread where allocation is not admissible.
  cuda_event done_event;
};

cuda_result prach_detector_backend::impl::ensure_plan(int dft_size, int batch, const cuda_stream& stream)
{
  ::cudaStream_t native = static_cast<::cudaStream_t>(stream.native());
  if (vkfft_valid && (vkfft_dft_size == dft_size) && (vkfft_batch == batch) && (vkfft_stream == native) &&
      (vkfft_buffer == d_idft.data())) {
    return {};
  }

  if (vkfft_valid) {
    deleteVkFFT(&vkfft_app);
    std::memset(&vkfft_app, 0, sizeof(vkfft_app));
    vkfft_valid = false;
  }

  int         device = 0;
  cuda_result result = check_cuda_error(::cudaGetDevice(&device), "query the current CUDA device");
  if (!result.has_value()) {
    return result;
  }
  if (cuDeviceGet(&vkfft_device, device) != CUDA_SUCCESS) {
    return make_unexpected(std::string("Failed to obtain the CUDA driver handle of the device"));
  }

  vkfft_stream      = native;
  vkfft_buffer      = d_idft.data();
  vkfft_buffer_size = static_cast<pfUINT>(static_cast<size_t>(batch) * dft_size * sizeof(cuFloatComplex));

  VkFFTConfiguration configuration = {};
  configuration.FFTdim             = 1;
  configuration.size[0]            = static_cast<pfUINT>(dft_size);
  configuration.device             = &vkfft_device;
  configuration.stream             = &vkfft_stream;
  configuration.num_streams        = 1;
  configuration.numberBatches      = batch;
  configuration.bufferSize         = &vkfft_buffer_size;
  configuration.buffer             = &vkfft_buffer;
  // An inverse-only plan matches the generic detector, which correlates with an inverse DFT.
  configuration.normalize              = 0;
  configuration.makeInversePlanOnly    = 1;
  configuration.disableReorderFourStep = 1;
  configuration.useLUT                 = 1;
  configuration.aimThreads             = (dft_size >= 512) ? 256 : 128;

  if (initializeVkFFT(&vkfft_app, configuration) != VKFFT_SUCCESS) {
    std::memset(&vkfft_app, 0, sizeof(vkfft_app));
    return make_unexpected(std::string("Failed to build the PRACH inverse DFT plan"));
  }

  vkfft_dft_size = dft_size;
  vkfft_batch    = batch;
  vkfft_valid    = true;
  return {};
}

cuda_result prach_detector_backend::impl::detect(prach_detector_output&         output,
                                                 span<const uint32_t>           input,
                                                 span<const cf_t>               roots,
                                                 const prach_detector_geometry& geometry,
                                                 const cuda_stream&             stream)
{
  std::string invalid = validate(geometry, input, roots);
  if (!invalid.empty()) {
    return make_unexpected(invalid);
  }

  kernel_config cfg                  = make_kernel_config(geometry);
  int           batches_per_sequence = cfg.nof_rx_ports * (cfg.combine_symbols ? 1 : cfg.nof_symbols);
  int           total_batches        = batches_per_sequence * cfg.nof_sequences;

  size_t input_size = static_cast<size_t>(cfg.nof_rx_ports) * cfg.nof_symbols * cfg.sequence_length;
  size_t roots_size = static_cast<size_t>(cfg.nof_sequences) * cfg.sequence_length;
  size_t idft_size  = static_cast<size_t>(total_batches) * cfg.dft_size;

  cuda_result result = ensure_capacity(d_input, input_size);
  if (!result.has_value()) {
    return result;
  }
  result = ensure_capacity(d_roots, roots_size);
  if (!result.has_value()) {
    return result;
  }
  result = ensure_capacity(d_idft, idft_size);
  if (!result.has_value()) {
    return result;
  }
  result = ensure_capacity(d_results, 1);
  if (!result.has_value()) {
    return result;
  }

  result = copy_to_device_async(d_input, input.first(input_size), stream);
  if (!result.has_value()) {
    return result;
  }

  // The root sequences only change when the PRACH configuration does, which is far less often than
  // once per occasion, so an unchanged set is left on the device.
  span<const cf_t> active_roots = roots.first(roots_size);
  if ((cached_roots_size != roots_size) ||
      !std::equal(active_roots.begin(), active_roots.end(), cached_roots.begin())) {
    result = copy_to_device_async(
        d_roots,
        span<const cuFloatComplex>(reinterpret_cast<const cuFloatComplex*>(active_roots.data()), roots_size),
        stream);
    if (!result.has_value()) {
      return result;
    }
    cached_roots.assign(active_roots.begin(), active_roots.end());
    cached_roots_size = roots_size;
  }

  result = ensure_plan(cfg.dft_size, total_batches, stream);
  if (!result.has_value()) {
    return result;
  }

  ::cudaStream_t native = static_cast<::cudaStream_t>(stream.native());

  // Every candidate slot outside the searched range keeps a stale validity flag unless the block is
  // cleared, so the clear covers the whole block rather than only the RSSI.
  result = device_zero_async(d_results, stream);
  if (!result.has_value()) {
    return result;
  }

  dim3 prepare_grid(batches_per_sequence, cfg.nof_sequences, (cfg.dft_size + THREADS - 1) / THREADS);
  kernel_prepare_idft<<<prepare_grid, THREADS, 0, native>>>(
      d_input.data(), d_roots.data(), d_idft.data(), &d_results.data()->rssi, cfg, batches_per_sequence);
  result = check_last_cuda_error("launch the PRACH correlation kernel");
  if (!result.has_value()) {
    return result;
  }

  VkFFTLaunchParams launch_params = {};
  launch_params.buffer            = &vkfft_buffer;
  if (VkFFTAppend(&vkfft_app, 1, &launch_params) != VKFFT_SUCCESS) {
    return make_unexpected(std::string("Failed to enqueue the PRACH inverse DFT"));
  }

  dim3 candidate_grid(cfg.nof_shifts, cfg.nof_sequences, 1);
  kernel_find_candidates<<<candidate_grid, THREADS, 0, native>>>(
      d_idft.data(), d_results.data()->candidates, cfg, batches_per_sequence);
  result = check_last_cuda_error("launch the PRACH search kernel");
  if (!result.has_value()) {
    return result;
  }

  result = copy_to_host_async(span<result_block>(&h_results, 1), d_results, stream);
  if (!result.has_value()) {
    return result;
  }

  // An event is cheaper to wait on than the whole stream for a path this short. It belongs to the
  // backend rather than to the call, because creating one allocates and a detection runs on a
  // real-time thread.
  result = done_event.record(stream);
  if (!result.has_value()) {
    return result;
  }
  result = done_event.synchronize();
  if (!result.has_value()) {
    return result;
  }

  output.candidates.clear();
  output.rssi = h_results.rssi / static_cast<float>(cfg.nof_rx_ports * cfg.nof_symbols * cfg.sequence_length);

  unsigned last_preamble = geometry.start_preamble_index + geometry.nof_preamble_indices;
  for (int i = 0; i != MAX_CANDIDATES; ++i) {
    const device_candidate& candidate = h_results.candidates[i];
    if (candidate.valid == 0) {
      continue;
    }
    unsigned preamble_index = static_cast<unsigned>(candidate.preamble_index);
    if ((preamble_index < geometry.start_preamble_index) || (preamble_index >= last_preamble)) {
      continue;
    }

    prach_detector_candidate& reported = output.candidates.emplace_back();
    reported.preamble_index            = preamble_index;
    reported.delay_samples             = static_cast<unsigned>(candidate.delay_samples);
    reported.detection_metric          = candidate.detection_metric;
    reported.preamble_power            = candidate.preamble_power;
  }

  return {};
}

cuda_expected<prach_detector_backend> prach_detector_backend::create()
{
  if (!is_prach_detector_backend_available()) {
    return make_unexpected(std::string("No CUDA device is available for the PRACH detector"));
  }

  auto pimpl = std::make_unique<impl>();

  cuda_expected<cuda_event> done = cuda_event::create();
  if (!done.has_value()) {
    return make_unexpected(done.error());
  }
  pimpl->set_done_event(std::move(done.value()));

  return prach_detector_backend(std::move(pimpl));
}

prach_detector_backend::prach_detector_backend(std::unique_ptr<impl> impl_) : pimpl(std::move(impl_)) {}

prach_detector_backend::~prach_detector_backend() = default;

prach_detector_backend::prach_detector_backend(prach_detector_backend&& other) noexcept = default;

prach_detector_backend& prach_detector_backend::operator=(prach_detector_backend&& other) noexcept = default;

cuda_result prach_detector_backend::detect(prach_detector_output&         output,
                                           span<const uint32_t>           input,
                                           span<const cf_t>               roots,
                                           const prach_detector_geometry& geometry,
                                           const cuda_stream&             stream)
{
  return pimpl->detect(output, input, roots, geometry, stream);
}

bool ocudu::cuda::is_prach_detector_backend_available()
{
  int nof_devices = 0;
  if ((::cudaGetDeviceCount(&nof_devices) != cudaSuccess) || (nof_devices == 0)) {
    // The query leaves the error latched on the context, which would surface at an unrelated call.
    (void)::cudaGetLastError();
    return false;
  }
  return true;
}
