// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Tests LDPC rate dematching (TS38.212 Section 5.4.2) against a reference golden implementation.
///
/// The golden_rate_dematch reference function implements the inverse of rate matching:
/// deinterleaving (Section 5.4.2.2) + placement from circular buffer (Section 5.4.2.1).
///
/// For each parameter set the test:
///   1. Generates random LLRs of buffer_length.
///   2. Feeds those LLRs through the real dematcher.
///   3. Compares against golden_rate_dematch output.

#include "ocudu/adt/to_array.h"
#include "ocudu/ocuduvec/compare.h"
#include "ocudu/ocuduvec/fill.h"
#include "ocudu/phy/upper/channel_coding/channel_coding_factories.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc.h"
#include "ocudu/phy/upper/channel_coding/ldpc/ldpc_rate_dematcher.h"
#include "ocudu/phy/upper/codeblock_metadata.h"
#include "ocudu/ran/sch/modulation_scheme.h"
#include "ocudu/ran/sch/sch_segmentation.h"
#include "ocudu/support/cpu_features.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

namespace ocudu {
std::ostream& operator<<(std::ostream& os, span<const log_likelihood_ratio> data)
{
  fmt::print(os, "{}", data);
  return os;
}

static bool operator==(span<const log_likelihood_ratio> left, span<const log_likelihood_ratio> right)
{
  return ocuduvec::equal(left, right);
}
} // namespace ocudu

/// Base-graph constants (mirrored from ldpc_graph_impl.h for test isolation).
constexpr unsigned BG1_N_SHORT = 66;
constexpr unsigned BG1_N_FULL  = 68;
constexpr unsigned BG1_M       = 46;
constexpr unsigned BG1_K       = BG1_N_FULL - BG1_M; // 22

constexpr unsigned BG2_N_SHORT = 50;
constexpr unsigned BG2_N_FULL  = 52;
constexpr unsigned BG2_M       = 42;
constexpr unsigned BG2_K       = BG2_N_FULL - BG2_M; // 10

static constexpr std::array<double, 4> shift_factor_bg1 = {0, 17, 33, 56};
static constexpr std::array<double, 4> shift_factor_bg2 = {0, 13, 25, 43};

/// Gets the full encoded codeblock length.
static unsigned get_full_codeblock_length(ldpc_base_graph_type base_graph, ldpc::lifting_size_t lifting_size)
{
  unsigned BG_N = (base_graph == ldpc_base_graph_type::BG1) ? BG1_N_SHORT : BG2_N_SHORT;
  return static_cast<unsigned>(lifting_size) * BG_N;
}

/// Test parameters
struct rate_dematcher_test_parameters {
  std::string          implementation;
  ldpc_base_graph_type base_graph;
  ldpc::lifting_size_t lifting_size;
  unsigned             rv;
  modulation_scheme    modulation;
  unsigned             Nref;
  unsigned             nof_filler_bits;
  unsigned             nof_rate_matched_bits;
  bool                 new_data;
};

std::ostream& operator<<(std::ostream& os, const rate_dematcher_test_parameters& tp)
{
  return os << fmt::format("{} BG{} LS{} RV{} {} Nref{} fill{} E{} new_data={}",
                           tp.implementation,
                           fmt::underlying(tp.base_graph),
                           fmt::underlying(tp.lifting_size),
                           tp.rv,
                           to_string(tp.modulation),
                           tp.Nref,
                           tp.nof_filler_bits,
                           tp.nof_rate_matched_bits,
                           tp.new_data);
}

/// Test fixture

class RateDematcherFixture : public ::testing::TestWithParam<rate_dematcher_test_parameters>
{
public:
  // Create factories just once.
  static void SetUpTestSuite()
  {
    if (!factory_generic) {
      factory_generic = create_ldpc_rate_dematcher_factory_sw("generic");
      report_error_if_not(factory_generic, "Failed to create generic factory.");
    }
    if (!factory_avx2) {
      factory_avx2 = create_ldpc_rate_dematcher_factory_sw("avx2");
      report_error_if_not(factory_avx2, "Failed to create avx2 factory.");
    }
    if (!factory_avx512) {
      factory_avx512 = create_ldpc_rate_dematcher_factory_sw("avx512");
      report_error_if_not(factory_avx512, "Failed to create avx512 factory.");
    }
    if (!factory_neon) {
      factory_neon = create_ldpc_rate_dematcher_factory_sw("neon");
      report_error_if_not(factory_neon, "Failed to create neon factory.");
    }
  }

protected:
  void SetUp() override
  {
    const auto& tp = GetParam();

    std::shared_ptr<ldpc_rate_dematcher_factory> factory;
    if (tp.implementation == "generic") {
      factory = factory_generic;
    } else if (tp.implementation == "avx2") {
      factory = factory_avx2;
    } else if (tp.implementation == "avx512") {
      factory = factory_avx512;
    } else if (tp.implementation == "neon") {
      factory = factory_neon;
    }
    if (!factory) {
      GTEST_SKIP() << fmt::format("Factory not available: {}", tp.implementation);
    }

    dematcher = factory->create();
#ifdef __x86_64__
    // For x86-64 architectures, the AVX2 implementation is not available if the CPU does not support AVX2.
    if ((tp.implementation == "avx2") && !cpu_supports_feature(cpu_feature::avx2)) {
      ASSERT_FALSE(dematcher);
      GTEST_SKIP();
    }

    // For x86-64 architectures, the AVX512 implementation is not available if some CPU features are not enabled.
    if ((tp.implementation == "avx512") &&
        (!cpu_supports_feature(cpu_feature::avx512f) || !cpu_supports_feature(cpu_feature::avx512bw) ||
         !cpu_supports_feature(cpu_feature::avx512vbmi))) {
      ASSERT_FALSE(dematcher);
      GTEST_SKIP();
    }
#endif // __x86_64__
    ASSERT_NE(dematcher, nullptr) << fmt::format("Could not create LDPC dematcher ({})", tp.implementation);

    // Build metadata.
    metadata.tb_common.base_graph        = tp.base_graph;
    metadata.tb_common.lifting_size      = tp.lifting_size;
    metadata.tb_common.rv                = tp.rv;
    metadata.tb_common.mod               = tp.modulation;
    metadata.tb_common.Nref              = tp.Nref;
    metadata.cb_specific.full_length     = get_full_codeblock_length(tp.base_graph, tp.lifting_size);
    metadata.cb_specific.nof_filler_bits = tp.nof_filler_bits;
  }

  /// Generate a vector of random soft bits (LLRs) of the given size.
  static std::vector<log_likelihood_ratio> generate_soft_bits(unsigned size)
  {
    std::vector<log_likelihood_ratio> out;
    out.reserve(size);
    std::generate_n(std::back_inserter(out), size, []() { return soft_bit_dist(rgen); });
    return out;
  }

  /// Reverts 3GPP TS38.212 §5.4.2 rate matching (deinterleaving + placement).
  static std::vector<log_likelihood_ratio> generate_expected_output(span<const log_likelihood_ratio> rm_buffer_in,
                                                                    span<const log_likelihood_ratio> rate_matched_llrs,
                                                                    bool                             new_data,
                                                                    const codeblock_metadata&        cfg)
  {
    const auto& tb_common = cfg.tb_common;
    const auto& cb_spec   = cfg.cb_specific;

    unsigned nof_rm_bits      = rate_matched_llrs.size();
    unsigned rv               = tb_common.rv;
    unsigned modulation_order = get_bits_per_symbol(tb_common.mod);
    unsigned lifting_size     = static_cast<unsigned>(tb_common.lifting_size);
    unsigned fill_cb_length   = cb_spec.full_length;
    unsigned rm_buffer_size   = tb_common.Nref;
    unsigned nof_filler_bits  = cb_spec.nof_filler_bits;

    // Select limited buffer size.
    rm_buffer_size = std::min(rm_buffer_size > 0 ? rm_buffer_size : ldpc::MAX_CODEBLOCK_SIZE, fill_cb_length);

    span<const double> shift_factor;
    if (fill_cb_length % BG1_N_SHORT == 0) {
      shift_factor = shift_factor_bg1;
    } else {
      shift_factor = shift_factor_bg2;
    }

    unsigned BG_K = (tb_common.base_graph == ldpc_base_graph_type::BG1) ? BG1_K : BG2_K;

    unsigned shift_k0 = static_cast<unsigned>(std::floor((shift_factor[rv] * static_cast<double>(rm_buffer_size)) /
                                                         static_cast<double>(fill_cb_length))) *
                        lifting_size;

    unsigned nof_systematic = (BG_K - 2) * lifting_size;
    unsigned nof_info_bits  = nof_systematic - nof_filler_bits;

    // Reverse the interleaving.
    std::vector<log_likelihood_ratio> deinterleaved(nof_rm_bits);
    unsigned                          K = nof_rm_bits / modulation_order;
    for (unsigned i = 0, in_index = 0; i != K; ++i) {
      for (unsigned j = 0; j != modulation_order; ++j) {
        deinterleaved[j * K + i] = rate_matched_llrs[in_index++];
      }
    }

    // Place LLRs into codeblock positions.
    std::vector output(rm_buffer_in.begin(), rm_buffer_in.end());

    // Fill output vector with zeros if marked as a new transmission.
    if (new_data) {
      ocuduvec::fill(output, 0);
    }

    // Set filler bits to +inf (fixed logical zero).
    ocuduvec::fill(span<log_likelihood_ratio>(output).subspan(nof_info_bits, nof_filler_bits),
                   log_likelihood_ratio::infinity());

    // Always combine (rate match buffer is zeroed on new data, so first pass copy = zero + value = value).
    // Place bits from shift_k0, wrapping around buffer_length, skipping filler.
    for (unsigned i = 0, pos = shift_k0; i != nof_rm_bits; ++i) {
      // Advance circular buffer position if it is in the filler bits region.
      if ((pos >= nof_info_bits) && (pos < nof_systematic)) {
        pos = nof_systematic;
      }

      // Place bits before filler (systematic region).
      output[pos] += deinterleaved[i];

      // Advance circular buffer position.
      pos = (pos + 1) % rm_buffer_size;
    }

    return output;
  }

  static std::shared_ptr<ldpc_rate_dematcher_factory> factory_generic;
  static std::shared_ptr<ldpc_rate_dematcher_factory> factory_avx2;
  static std::shared_ptr<ldpc_rate_dematcher_factory> factory_avx512;
  static std::shared_ptr<ldpc_rate_dematcher_factory> factory_neon;
  static std::mt19937                                 rgen;
  static std::uniform_int_distribution<int>           soft_bit_dist;

  std::unique_ptr<ldpc_rate_dematcher> dematcher;
  codeblock_metadata                   metadata;
};

std::mt19937                       RateDematcherFixture::rgen{0};
std::uniform_int_distribution<int> RateDematcherFixture::soft_bit_dist{static_cast<int>(log_likelihood_ratio::min()),
                                                                       static_cast<int>(log_likelihood_ratio::max())};
std::shared_ptr<ldpc_rate_dematcher_factory> RateDematcherFixture::factory_generic = nullptr;
std::shared_ptr<ldpc_rate_dematcher_factory> RateDematcherFixture::factory_avx2    = nullptr;
std::shared_ptr<ldpc_rate_dematcher_factory> RateDematcherFixture::factory_avx512  = nullptr;
std::shared_ptr<ldpc_rate_dematcher_factory> RateDematcherFixture::factory_neon    = nullptr;

TEST_P(RateDematcherFixture, RateDematchAgainstReference)
{
  const auto& tp = GetParam();

  // Generate random input and rate match buffer soft bits.
  std::vector<log_likelihood_ratio> input = generate_soft_bits(tp.nof_rate_matched_bits);
  std::vector<log_likelihood_ratio> rm_buffer =
      generate_soft_bits(get_full_codeblock_length(tp.base_graph, tp.lifting_size));

  // Generate expected RM buffer after the rate matching.
  std::vector<log_likelihood_ratio> expected_rm_buffer =
      generate_expected_output(rm_buffer, input, tp.new_data, metadata);

  // Run the rate dematcher implementation.
  dematcher->rate_dematch(rm_buffer, span<const log_likelihood_ratio>(input), tp.new_data, metadata);

  // Compare actual dematcher output against the reference dematch output.
  ASSERT_EQ(span<const log_likelihood_ratio>(rm_buffer), span<const log_likelihood_ratio>(expected_rm_buffer));
}

/// Test case generation function.
static std::vector<rate_dematcher_test_parameters> generate_cases()
{
  // Modulation list.
  static constexpr auto modulations = to_array<modulation_scheme>({modulation_scheme::BPSK,
                                                                   modulation_scheme::QPSK,
                                                                   modulation_scheme::QAM16,
                                                                   modulation_scheme::QAM64,
                                                                   modulation_scheme::QAM256});

  // Implementation list.
  static constexpr auto implementations = to_array<const char*>({
#ifdef __x86_64__
      "generic",
      "avx2",
      "avx512"
#endif // __x86_64__
#ifdef __aarch64__
      "generic",
      "neon"
#endif // __aarch64__
  });

  // List of base graphs.
  static constexpr auto base_graphs =
      to_array<ldpc_base_graph_type>({ldpc_base_graph_type::BG1, ldpc_base_graph_type::BG2});

  // Lifting sizes valid for both BG1 (K=22, max=8448) and BG2 (K=10, max=3840).
  static constexpr auto lifting_sizes = to_array<ldpc::lifting_size_t>(
      {ldpc::LS2, ldpc::LS16, ldpc::LS32, ldpc::LS64, ldpc::LS96, ldpc::LS160, ldpc::LS256, ldpc::LS384});

  static constexpr auto redundancy_versions = to_array<unsigned>({0, 1, 2, 3});

  // Limited buffer lengths. 0 means unlimited.
  static constexpr auto limited_buffer_lengths = to_array<float>({0.0, 0.75});

  // List of filler bits.
  static constexpr auto nof_filler_bits_list = to_array<unsigned>({0, 2, 4, 8});

  // New data values.
  static constexpr auto new_data_values = to_array<bool>({true, false});

  // Random generator and distributions.
  std::mt19937                            rgen;
  std::uniform_int_distribution<unsigned> filler_bits_dist(0, nof_filler_bits_list.size() - 1);
  std::uniform_real_distribution<float>   code_rate_dist(0.1f, 0.95f);
  std::uniform_int_distribution<unsigned> modulation_dist(0, modulations.size() - 1);

  std::vector<rate_dematcher_test_parameters> out;
  for (const char* implementation : implementations) {
    for (ldpc_base_graph_type base_graph : base_graphs) {
      for (ldpc::lifting_size_t lifting_size : lifting_sizes) {
        for (unsigned rv : redundancy_versions) {
          for (float rm_buffer_size_ratio : limited_buffer_lengths) {
            for (bool new_data : new_data_values) {
              // Select random parameters.
              modulation_scheme modulation      = modulations[modulation_dist(rgen)];
              unsigned          nof_filler_bits = nof_filler_bits_list[filler_bits_dist(rgen)];
              float             code_rate       = code_rate_dist(rgen);

              // Calculate the total number of systematic bits.
              unsigned BG_K_val       = (base_graph == ldpc_base_graph_type::BG1) ? BG1_K : BG2_K;
              unsigned nof_systematic = (BG_K_val - 2) * static_cast<unsigned>(lifting_size);

              // Calculate the rate matching buffer size.
              unsigned rm_buffer_size =
                  static_cast<unsigned>(rm_buffer_size_ratio * get_full_codeblock_length(base_graph, lifting_size));

              // Derive the number of rate matched bits from the random code rate.
              unsigned nof_info_bits    = nof_systematic - nof_filler_bits;
              unsigned modulation_order = get_bits_per_symbol(modulation);
              unsigned nof_rate_matched =
                  static_cast<unsigned>(std::floor(static_cast<float>(nof_info_bits) / code_rate / modulation_order)) *
                  modulation_order;

              // Skip test case if invalid.
              if (nof_rate_matched == 0) {
                continue;
              }

              out.push_back({implementation,
                             base_graph,
                             lifting_size,
                             rv,
                             modulation,
                             rm_buffer_size,
                             nof_filler_bits,
                             nof_rate_matched,
                             new_data});
            }
          }
        }
      }
    }
  }

  return out;
}

static std::vector<rate_dematcher_test_parameters> all_tests = generate_cases();

INSTANTIATE_TEST_SUITE_P(LDPC, RateDematcherFixture, ::testing::ValuesIn(all_tests));
