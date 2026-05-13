// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "prach_detector_generic_thresholds.h"
#include "ocudu/adt/span.h"
#include "ocudu/adt/to_array.h"
#include "ocudu/ran/prach/prach_format_type.h"
#include "ocudu/ran/prach/prach_subcarrier_spacing.h"

using namespace ocudu;
using namespace detail;

/// Overloaded less-than comparison operator.
static bool operator<(const threshold_params& lhs, const threshold_params& rhs)
{
  if (lhs.nof_rx_ports < rhs.nof_rx_ports) {
    return true;
  }
  if (lhs.nof_rx_ports > rhs.nof_rx_ports) {
    return false;
  }

  // If we are here, nof_rx_ports is equal.
  if (lhs.scs < rhs.scs) {
    return true;
  }
  if (lhs.scs > rhs.scs) {
    return false;
  }

  // If we are here, nof_rx_ports and scs are equal.
  if (lhs.format < rhs.format) {
    return true;
  }
  if (lhs.format > rhs.format) {
    return false;
  }

  // If we are here, nof_rx_ports, scs and format are equal.
  if (lhs.zero_correlation_zone < rhs.zero_correlation_zone) {
    return true;
  }
  if (lhs.zero_correlation_zone > rhs.zero_correlation_zone) {
    return false;
  }

  return false;
}

/// Overloaded equal-to comparison operator.
static bool operator==(const threshold_params& lhs, const threshold_params& rhs)
{
  if (lhs.nof_rx_ports != rhs.nof_rx_ports) {
    return false;
  }

  // If we are here, nof_rx_ports is equal.
  if (lhs.scs != rhs.scs) {
    return false;
  }

  // If we are here, nof_rx_ports and scs are equal.
  if (lhs.format != rhs.format) {
    return false;
  }

  // If we are here, nof_rx_ports, scs and format are equal.
  if (lhs.zero_correlation_zone != rhs.zero_correlation_zone) {
    return false;
  }

  return true;
}

namespace {

/// Manages the mapping between PRACH configuration and the (threshold, margin) pairs.
class threshold_and_margin_finder
{
public:
  /// Mapping between PRACH configuration and threshold value.
  struct threshold_entry {
    /// Subset of PRACH configuration parameters affecting the threshold value.
    threshold_params configuration;
    /// Threshold value and search margin.
    threshold_and_margin_type threshold_and_margin;
    /// Threshold quality flag.
    threshold_flag flag;
  };

  /// Constructor: receives the list of pairings and ensures it is sorted.
  explicit threshold_and_margin_finder(span<const threshold_entry> in) :
    sorted_thresholds_and_margins(in.begin(), in.end())
  {
    std::sort(sorted_thresholds_and_margins.begin(),
              sorted_thresholds_and_margins.end(),
              [](const threshold_entry& a, const threshold_entry& b) { return (a.configuration < b.configuration); });
  }

  /// Retrieves the (threshold, margin) pair corresponding to the given configuration.
  threshold_and_margin_type get(const threshold_params& params) const
  {
    auto it =
        std::lower_bound(sorted_thresholds_and_margins.begin(),
                         sorted_thresholds_and_margins.end(),
                         params,
                         [](const threshold_entry& a, const threshold_params& b) { return (a.configuration < b); });

    if (it != sorted_thresholds_and_margins.end()) {
      return it->threshold_and_margin;
    }

    // todo(david): once all cases are covered, replace this by a ocudu_assert.
    if (is_long_preamble(params.format)) {
      return {/* threshold */ 2.0F, /* combine symbols */ false, /* margin */ 5};
    }
    return {/* threshold */ 0.3F, /* combine symbols */ false, /* margin */ 12};
  }

  /// Checks the quality flag of the threshold for the given configurations.
  threshold_flag check_flag(const threshold_params& params) const
  {
    auto it =
        std::lower_bound(sorted_thresholds_and_margins.begin(),
                         sorted_thresholds_and_margins.end(),
                         params,
                         [](const threshold_entry& a, const threshold_params& b) { return (a.configuration < b); });

    if ((it != sorted_thresholds_and_margins.end()) && (it->configuration == params)) {
      return it->flag;
    }
    return threshold_flag::red;
  }

private:
  /// Sorted list of thresholds.
  std::vector<threshold_entry> sorted_thresholds_and_margins;
};

using th_flag = threshold_flag;

} // namespace

/// Unsorted list of thresholds.
static constexpr auto all_threshold_and_margins = to_array<threshold_and_margin_finder::threshold_entry>({
    // clang-format off
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 0}, {0.147F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 1}, {1.000F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 2}, {0.874F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 3}, {0.774F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 4}, {0.640F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 5}, {0.551F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 6}, {0.449F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 7}, {0.388F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 8}, {0.321F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 9}, {0.255F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 10}, {0.205F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 11}, {0.167F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 12}, {0.147F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 13}, {0.148F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 14}, {0.148F, /* combine symbols */ true, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 15}, {0.148F, /* combine symbols */ true, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 0}, {0.085F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 1}, {0.522F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 2}, {0.451F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 3}, {0.404F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 4}, {0.344F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 5}, {0.301F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 6}, {0.249F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 7}, {0.219F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 8}, {0.180F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 9}, {0.146F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 10}, {0.118F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 11}, {0.098F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 12}, {0.085F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 13}, {0.086F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 14}, {0.085F, /* combine symbols */ true, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 15}, {0.085F, /* combine symbols */ true, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 0}, {0.053F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 1}, {0.307F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 2}, {0.269F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 3}, {0.240F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 4}, {0.207F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 5}, {0.180F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 6}, {0.149F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 7}, {0.131F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 8}, {0.112F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 9}, {0.090F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 10}, {0.072F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 11}, {0.060F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 12}, {0.052F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 13}, {0.052F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 14}, {0.052F, /* combine symbols */ true, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 15}, {0.053F, /* combine symbols */ true, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 0}, {0.025F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 1}, {1.000F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 2}, {0.869F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 3}, {0.781F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 4}, {0.636F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 5}, {0.548F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 6}, {0.455F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 7}, {0.387F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 8}, {0.328F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 9}, {0.256F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 10}, {0.205F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 11}, {0.169F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 12}, {0.134F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 13}, {0.097F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 14}, {0.060F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 15}, {0.041F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 0}, {0.014F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 1}, {0.510F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 2}, {0.459F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 3}, {0.409F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 4}, {0.341F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 5}, {0.299F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 6}, {0.250F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 7}, {0.213F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 8}, {0.183F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 9}, {0.145F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 10}, {0.118F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 11}, {0.097F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 12}, {0.078F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 13}, {0.057F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 14}, {0.035F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 15}, {0.024F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 0}, {0.009F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 1}, {0.304F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 2}, {0.269F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 3}, {0.242F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 4}, {0.206F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 5}, {0.180F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 6}, {0.150F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 7}, {0.132F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 8}, {0.112F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 9}, {0.090F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 10}, {0.072F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 11}, {0.060F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 12}, {0.048F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 13}, {0.035F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 14}, {0.022F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 15}, {0.015F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 0}, {0.036F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 1}, {0.304F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 2}, {0.273F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 3}, {0.241F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 4}, {0.206F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 5}, {0.181F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 6}, {0.152F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 7}, {0.131F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 8}, {0.112F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 9}, {0.090F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 10}, {0.072F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 11}, {0.060F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 12}, {0.048F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 13}, {0.036F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 14}, {0.037F, /* combine symbols */ false, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 15}, {0.037F, /* combine symbols */ false, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 0}, {0.025F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 1}, {0.197F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 2}, {0.177F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 3}, {0.159F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 4}, {0.137F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 5}, {0.120F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 6}, {0.100F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 7}, {0.087F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 8}, {0.074F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 9}, {0.060F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 10}, {0.048F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 11}, {0.040F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 12}, {0.032F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 13}, {0.025F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 14}, {0.025F, /* combine symbols */ false, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 15}, {0.025F, /* combine symbols */ false, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 0}, {0.018F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 1}, {0.140F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 2}, {0.126F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 3}, {0.113F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 4}, {0.097F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 5}, {0.085F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 6}, {0.071F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 7}, {0.062F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 8}, {0.053F, /* combine symbols */ false, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 9}, {0.043F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 10}, {0.035F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 11}, {0.029F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 12}, {0.023F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 13}, {0.018F, /* combine symbols */ false, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 14}, {0.018F, /* combine symbols */ false, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 15}, {0.018F, /* combine symbols */ false, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 0}, {0.040F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 1}, {1.010F, /* combine symbols */ true, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 2}, {0.543F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 3}, {0.436F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 4}, {0.387F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 5}, {0.363F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 6}, {0.309F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 7}, {0.276F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 8}, {0.237F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 9}, {0.201F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 10}, {0.170F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 11}, {0.133F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 12}, {0.117F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 13}, {0.079F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 14}, {0.059F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 15}, {0.041F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 0}, {0.023F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 1}, {0.514F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 2}, {0.298F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 3}, {0.242F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 4}, {0.217F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 5}, {0.199F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 6}, {0.175F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 7}, {0.156F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 8}, {0.137F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 9}, {0.118F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 10}, {0.097F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 11}, {0.077F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 12}, {0.067F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 13}, {0.046F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 14}, {0.035F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 15}, {0.024F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 0}, {0.015F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 1}, {0.303F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 2}, {0.178F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 3}, {0.147F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 4}, {0.131F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 5}, {0.124F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 6}, {0.107F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 7}, {0.095F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 8}, {0.084F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 9}, {0.072F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 10}, {0.060F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 11}, {0.048F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 12}, {0.042F, /* combine symbols */ true, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 13}, {0.029F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 14}, {0.022F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz5, prach_format_type::three, /* ZCZ */ 15}, {0.015F, /* combine symbols */ true, 5}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 0}, {0.610F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 1}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 2}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 3}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 4}, {0.995F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 5}, {0.876F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 6}, {0.787F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 7}, {0.769F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 8}, {0.690F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 9}, {0.637F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 10}, {0.600F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 11}, {0.607F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 12}, {0.604F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 13}, {0.597F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 14}, {0.603F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 15}, {0.598F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 0}, {0.328F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 1}, {0.692F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 2}, {0.625F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 3}, {0.555F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 4}, {0.507F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 5}, {0.461F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 6}, {0.423F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 7}, {0.409F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 8}, {0.371F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 9}, {0.344F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 10}, {0.323F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 11}, {0.326F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 12}, {0.323F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 13}, {0.324F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 14}, {0.326F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 15}, {0.326F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 0}, {0.195F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 1}, {0.406F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 2}, {0.367F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 3}, {0.328F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 4}, {0.303F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 5}, {0.271F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 6}, {0.246F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 7}, {0.243F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 8}, {0.224F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 9}, {0.206F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 10}, {0.196F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 11}, {0.197F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 12}, {0.196F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 13}, {0.196F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 14}, {0.198F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 15}, {0.196F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 0}, {0.605F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 1}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 2}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 3}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 4}, {0.980F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 5}, {0.879F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 6}, {0.785F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 7}, {0.760F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 8}, {0.692F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 9}, {0.631F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 10}, {0.598F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 11}, {0.602F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 12}, {0.611F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 13}, {0.607F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 14}, {0.605F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 15}, {0.607F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 0}, {0.322F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 1}, {0.686F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 2}, {0.617F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 3}, {0.554F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 4}, {0.516F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 5}, {0.460F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 6}, {0.416F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 7}, {0.406F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 8}, {0.374F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 9}, {0.342F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 10}, {0.323F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 11}, {0.325F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 12}, {0.326F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 13}, {0.326F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 14}, {0.328F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 15}, {0.321F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 0}, {0.195F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 1}, {0.401F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 2}, {0.358F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 3}, {0.325F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 4}, {0.302F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 5}, {0.271F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 6}, {0.244F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 7}, {0.241F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 8}, {0.222F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 9}, {0.204F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 10}, {0.194F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 11}, {0.195F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 12}, {0.195F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 13}, {0.195F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 14}, {0.196F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 15}, {0.195F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 0}, {0.605F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 1}, {1.000F, /* combine symbols */ true, 12}, th_flag::red}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 2}, {1.000F, /* combine symbols */ true, 12}, th_flag::red}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 3}, {1.000F, /* combine symbols */ true, 12}, th_flag::red}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 4}, {0.980F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 5}, {0.879F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 6}, {0.785F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 7}, {0.760F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 8}, {0.692F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 9}, {0.631F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 10}, {0.598F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 11}, {0.602F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 12}, {0.611F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 13}, {0.607F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 14}, {0.605F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 15}, {0.607F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 0}, {0.322F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 1}, {0.686F, /* combine symbols */ true, 12}, th_flag::red}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 2}, {0.617F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 3}, {0.554F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 4}, {0.516F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 5}, {0.460F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 6}, {0.416F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 7}, {0.406F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 8}, {0.374F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 9}, {0.342F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 10}, {0.323F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 11}, {0.325F, /* combine symbols */ true, 12}, th_flag::green}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 12}, {0.326F, /* combine symbols */ true, 12}, th_flag::green}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 13}, {0.326F, /* combine symbols */ true, 12}, th_flag::green}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 14}, {0.324F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 15}, {0.321F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 0}, {0.195F, /* combine symbols */ true, 12}, th_flag::green}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 1}, {0.401F, /* combine symbols */ true, 12}, th_flag::red}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 2}, {0.358F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 3}, {0.325F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 4}, {0.302F, /* combine symbols */ true, 12}, th_flag::green}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 5}, {0.271F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 6}, {0.244F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 7}, {0.241F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 8}, {0.222F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 9}, {0.204F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 10}, {0.194F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 11}, {0.195F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 12}, {0.195F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 13}, {0.195F, /* combine symbols */ true, 12}, th_flag::green}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 14}, {0.195F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::A1, /* ZCZ */ 15}, {0.195F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 0}, {0.352F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 1}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 2}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 3}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 4}, {0.987F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 5}, {0.868F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 6}, {0.789F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 7}, {0.762F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 8}, {0.696F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 9}, {0.633F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 10}, {0.602F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 11}, {0.516F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 12}, {0.461F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 13}, {0.388F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 14}, {0.350F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 15}, {0.353F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 0}, {0.192F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 1}, {0.688F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 2}, {0.625F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 3}, {0.568F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 4}, {0.516F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 5}, {0.460F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 6}, {0.416F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 7}, {0.407F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 8}, {0.372F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 9}, {0.342F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 10}, {0.324F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 11}, {0.281F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 12}, {0.255F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 13}, {0.218F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 14}, {0.193F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 15}, {0.192F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 0}, {0.119F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 1}, {0.402F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 2}, {0.363F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 3}, {0.326F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 4}, {0.304F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 5}, {0.273F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 6}, {0.248F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 7}, {0.243F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 8}, {0.224F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 9}, {0.206F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 10}, {0.195F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 11}, {0.172F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 12}, {0.154F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 13}, {0.131F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 14}, {0.118F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 15}, {0.119F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 0}, {0.352F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 1}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 2}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 3}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 4}, {0.993F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 5}, {0.863F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 6}, {0.786F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 7}, {0.776F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 8}, {0.699F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 9}, {0.633F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 10}, {0.607F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 11}, {0.518F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 12}, {0.464F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 13}, {0.392F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 14}, {0.349F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 15}, {0.353F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 0}, {0.194F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 1}, {0.694F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 2}, {0.611F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 3}, {0.550F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 4}, {0.510F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 5}, {0.452F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 6}, {0.411F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 7}, {0.400F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 8}, {0.367F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 9}, {0.340F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 10}, {0.323F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 11}, {0.282F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 12}, {0.256F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 13}, {0.212F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 14}, {0.193F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 15}, {0.193F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 0}, {0.118F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 1}, {0.404F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 2}, {0.362F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 3}, {0.328F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 4}, {0.301F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 5}, {0.272F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 6}, {0.247F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 7}, {0.242F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 8}, {0.222F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 9}, {0.205F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 10}, {0.195F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 11}, {0.170F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 12}, {0.155F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 13}, {0.131F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 14}, {0.119F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 15}, {0.119F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 0}, {0.249F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 1}, {1.380F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 2}, {1.238F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 3}, {1.072F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 4}, {0.973F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 5}, {0.870F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 6}, {0.781F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 7}, {0.766F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 8}, {0.696F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 9}, {0.643F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 10}, {0.593F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 11}, {0.522F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 12}, {0.465F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 13}, {0.388F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 14}, {0.304F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 15}, {0.247F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 0}, {0.142F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 1}, {0.676F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 2}, {0.620F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 3}, {0.548F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 4}, {0.507F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 5}, {0.450F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 6}, {0.414F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 7}, {0.409F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 8}, {0.375F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 9}, {0.345F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 10}, {0.326F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 11}, {0.283F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 12}, {0.256F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 13}, {0.216F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 14}, {0.170F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 15}, {0.140F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 0}, {0.086F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 1}, {0.403F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 2}, {0.360F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 3}, {0.322F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 4}, {0.297F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 5}, {0.267F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 6}, {0.244F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 7}, {0.241F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 8}, {0.220F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 9}, {0.205F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 10}, {0.195F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 11}, {0.172F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 12}, {0.154F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 13}, {0.131F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 14}, {0.104F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A3, /* ZCZ */ 15}, {0.086F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 0}, {0.251F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 1}, {1.359F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 2}, {1.224F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 3}, {1.069F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 4}, {0.986F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 5}, {0.868F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 6}, {0.783F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 7}, {0.770F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 8}, {0.705F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 9}, {0.637F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 10}, {0.611F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 11}, {0.524F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 12}, {0.468F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 13}, {0.391F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 14}, {0.302F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 15}, {0.249F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 0}, {0.143F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 1}, {0.696F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 2}, {0.623F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 3}, {0.549F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 4}, {0.506F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 5}, {0.453F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 6}, {0.414F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 7}, {0.405F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 8}, {0.372F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 9}, {0.342F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 10}, {0.325F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 11}, {0.283F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 12}, {0.253F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 13}, {0.215F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 14}, {0.171F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 15}, {0.142F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 0}, {0.086F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 1}, {0.399F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 2}, {0.360F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 3}, {0.324F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 4}, {0.297F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 5}, {0.273F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 6}, {0.248F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 7}, {0.243F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 8}, {0.224F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 9}, {0.208F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 10}, {0.196F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 11}, {0.172F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 12}, {0.155F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 13}, {0.131F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 14}, {0.104F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A3, /* ZCZ */ 15}, {0.087F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 0}, {0.234F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 1}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 2}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 3}, {1.000F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 4}, {0.995F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 5}, {0.876F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 6}, {0.789F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 7}, {0.763F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 8}, {0.707F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 9}, {0.646F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 10}, {0.603F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 11}, {0.517F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 12}, {0.465F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 13}, {0.388F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 14}, {0.304F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 15}, {0.232F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 0}, {0.134F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 1}, {0.687F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 2}, {0.616F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 3}, {0.553F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 4}, {0.510F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 5}, {0.456F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 6}, {0.408F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 7}, {0.402F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 8}, {0.370F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 9}, {0.348F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 10}, {0.326F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 11}, {0.289F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 12}, {0.252F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 13}, {0.217F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 14}, {0.173F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 15}, {0.133F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 0}, {0.080F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 1}, {0.401F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 2}, {0.360F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 3}, {0.324F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 4}, {0.298F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 5}, {0.270F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 6}, {0.246F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 7}, {0.242F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 8}, {0.223F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 9}, {0.205F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 10}, {0.193F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 11}, {0.171F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 12}, {0.153F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 13}, {0.130F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 14}, {0.104F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 15}, {0.080F, /* combine symbols */ true, 12}, th_flag::orange},
    // The following commented value is the calibrated one - however, we noticed that in synthetic environments with no
    // noise/interference, the resulting false-alarm probability is a bit too high. We increase a bit the threshold.
    // {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 0}, {0.229F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 0}, {0.458F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 1}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 2}, {1.000F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 3}, {1.000F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 4}, {0.986F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 5}, {0.883F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 6}, {0.783F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 7}, {0.770F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 8}, {0.700F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 9}, {0.637F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 10}, {0.599F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 11}, {0.516F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 12}, {0.464F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 13}, {0.385F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 14}, {0.304F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 15}, {0.232F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 0}, {0.131F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 1}, {0.677F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 2}, {0.622F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 3}, {0.553F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 4}, {0.511F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 5}, {0.458F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 6}, {0.414F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 7}, {0.409F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 8}, {0.374F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 9}, {0.345F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 10}, {0.325F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 11}, {0.283F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 12}, {0.255F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 13}, {0.213F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 14}, {0.171F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 15}, {0.132F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 0}, {0.081F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 1}, {0.404F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 2}, {0.363F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 3}, {0.322F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 4}, {0.298F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 5}, {0.268F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 6}, {0.246F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 7}, {0.241F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 8}, {0.219F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 9}, {0.205F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 10}, {0.197F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 11}, {0.171F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 12}, {0.155F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 13}, {0.131F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 14}, {0.105F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 15}, {0.081F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 0}, {0.044F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 1}, {0.217F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 2}, {0.193F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 3}, {0.172F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 4}, {0.159F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 5}, {0.145F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 6}, {0.131F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 7}, {0.129F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 8}, {0.119F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 9}, {0.110F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 10}, {0.104F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 11}, {0.092F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 12}, {0.083F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 13}, {0.071F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 14}, {0.056F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 15}, {0.044F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 0}, {0.033F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 1}, {0.162F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 2}, {0.145F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 3}, {0.129F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 4}, {0.119F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 5}, {0.108F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 6}, {0.098F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 7}, {0.096F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 8}, {0.088F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 9}, {0.082F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 10}, {0.078F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 11}, {0.068F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 12}, {0.062F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 13}, {0.053F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 14}, {0.042F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 15}, {0.033F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 0}, {0.081F, /* combine symbols */ true, 12}, th_flag::green}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 1}, {0.404F, /* combine symbols */ true, 12}, th_flag::red}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 2}, {0.363F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 3}, {0.322F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 4}, {0.298F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 5}, {0.268F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 6}, {0.246F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 7}, {0.241F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 8}, {0.219F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 9}, {0.205F, /* combine symbols */ true, 12}, th_flag::orange}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 10}, {0.197F, /* combine symbols */ true, 12}, th_flag::green}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 11}, {0.171F, /* combine symbols */ true, 12}, th_flag::green}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 12}, {0.155F, /* combine symbols */ true, 12}, th_flag::green}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 13}, {0.131F, /* combine symbols */ true, 12}, th_flag::green}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 14}, {0.105F, /* combine symbols */ true, 12}, th_flag::green}, // provisional
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz120, prach_format_type::B4, /* ZCZ */ 15}, {0.081F, /* combine symbols */ true, 12}, th_flag::green}, // provisional
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 0}, {0.181F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 1}, {1.379F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 2}, {1.232F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 3}, {1.082F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 4}, {0.986F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 5}, {0.890F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 6}, {0.792F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 7}, {0.769F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 8}, {0.706F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 9}, {0.643F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 10}, {0.607F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 11}, {0.524F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 12}, {0.479F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 13}, {0.390F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 14}, {0.305F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 15}, {0.211F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 0}, {0.103F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 1}, {0.693F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 2}, {0.630F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 3}, {0.556F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 4}, {0.516F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 5}, {0.455F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 6}, {0.416F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 7}, {0.400F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 8}, {0.369F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 9}, {0.344F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 10}, {0.323F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 11}, {0.280F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 12}, {0.254F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 13}, {0.217F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 14}, {0.170F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 15}, {0.121F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 0}, {0.064F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 1}, {0.404F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 2}, {0.368F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 3}, {0.326F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 4}, {0.301F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 5}, {0.271F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 6}, {0.248F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 7}, {0.241F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 8}, {0.223F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 9}, {0.208F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 10}, {0.195F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 11}, {0.172F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 12}, {0.156F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 13}, {0.132F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 14}, {0.105F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C0, /* ZCZ */ 15}, {0.075F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 0}, {0.180F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 1}, {1.363F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 2}, {1.231F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 3}, {1.081F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 4}, {0.980F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 5}, {0.865F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 6}, {0.784F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 7}, {0.768F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 8}, {0.696F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 9}, {0.643F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 10}, {0.600F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 11}, {0.521F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 12}, {0.465F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 13}, {0.385F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 14}, {0.304F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 15}, {0.213F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 0}, {0.104F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 1}, {0.686F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 2}, {0.620F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 3}, {0.555F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 4}, {0.510F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 5}, {0.460F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 6}, {0.420F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 7}, {0.406F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 8}, {0.370F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 9}, {0.344F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 10}, {0.326F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 11}, {0.283F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 12}, {0.258F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 13}, {0.215F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 14}, {0.171F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 15}, {0.122F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 0}, {0.064F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 1}, {0.403F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 2}, {0.361F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 3}, {0.321F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 4}, {0.300F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 5}, {0.271F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 6}, {0.248F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 7}, {0.242F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 8}, {0.223F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 9}, {0.205F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 10}, {0.195F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 11}, {0.171F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 12}, {0.155F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 13}, {0.130F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 14}, {0.105F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C0, /* ZCZ */ 15}, {0.075F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 0}, {0.183F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 1}, {1.438F, /* combine symbols */ false, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 2}, {1.268F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 3}, {1.111F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 4}, {1.010F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 5}, {0.895F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 6}, {0.808F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 7}, {0.787F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 8}, {0.711F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 9}, {0.651F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 10}, {0.613F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 11}, {0.531F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 12}, {0.472F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 13}, {0.395F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 14}, {0.308F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 15}, {0.217F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 0}, {0.105F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 1}, {0.713F, /* combine symbols */ false, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 2}, {0.633F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 3}, {0.565F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 4}, {0.517F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 5}, {0.463F, /* combine symbols */ false, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 6}, {0.422F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 7}, {0.413F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 8}, {0.378F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 9}, {0.347F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 10}, {0.329F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 11}, {0.288F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 12}, {0.258F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 13}, {0.218F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 14}, {0.172F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz120, prach_format_type::C0, /* ZCZ */ 15}, {0.123F, /* combine symbols */ false, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 0}, {0.115F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 1}, {1.358F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 2}, {1.236F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 3}, {1.074F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 4}, {0.972F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 5}, {0.874F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 6}, {0.778F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 7}, {0.768F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 8}, {0.694F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 9}, {0.641F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 10}, {0.612F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 11}, {0.521F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 12}, {0.467F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 13}, {0.388F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 14}, {0.306F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 15}, {0.216F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 0}, {0.067F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 1}, {0.695F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 2}, {0.616F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 3}, {0.558F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 4}, {0.511F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 5}, {0.455F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 6}, {0.419F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 7}, {0.408F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 8}, {0.372F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 9}, {0.337F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 10}, {0.322F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 11}, {0.283F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 12}, {0.258F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 13}, {0.215F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 14}, {0.171F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 15}, {0.121F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 0}, {0.041F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 1}, {0.402F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 2}, {0.365F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 3}, {0.326F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 4}, {0.302F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 5}, {0.274F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 6}, {0.248F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 7}, {0.243F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 8}, {0.223F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 9}, {0.206F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 10}, {0.197F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 11}, {0.173F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 12}, {0.156F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 13}, {0.132F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 14}, {0.104F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::C2, /* ZCZ */ 15}, {0.075F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 0}, {0.115F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 1}, {1.351F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 2}, {1.234F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 3}, {1.092F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 4}, {0.993F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 5}, {0.869F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 6}, {0.778F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 7}, {0.764F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 8}, {0.691F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 9}, {0.639F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 10}, {0.606F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 11}, {0.522F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 12}, {0.468F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 13}, {0.392F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 14}, {0.304F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 15}, {0.214F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 0}, {0.065F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 1}, {0.689F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 2}, {0.612F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 3}, {0.548F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 4}, {0.507F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 5}, {0.457F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 6}, {0.414F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 7}, {0.402F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 8}, {0.371F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 9}, {0.343F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 10}, {0.324F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 11}, {0.283F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 12}, {0.255F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 13}, {0.216F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 14}, {0.169F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 15}, {0.121F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 0}, {0.041F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 1}, {0.402F, /* combine symbols */ true, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 2}, {0.364F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 3}, {0.320F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 4}, {0.298F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 5}, {0.272F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 6}, {0.247F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 7}, {0.240F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 8}, {0.221F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 9}, {0.205F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 10}, {0.195F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 11}, {0.171F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 12}, {0.154F, /* combine symbols */ true, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 13}, {0.131F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 14}, {0.105F, /* combine symbols */ true, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::C2, /* ZCZ */ 15}, {0.075F, /* combine symbols */ true, 12}, th_flag::green},
    // clang-format on
});

static const threshold_and_margin_finder threshold_and_margin_table(all_threshold_and_margins);

threshold_and_margin_type ocudu::detail::get_threshold_and_margin(const threshold_params& params)
{
  return threshold_and_margin_table.get(params);
}

threshold_flag ocudu::detail::get_threshold_flag(const threshold_params& params)
{
  return threshold_and_margin_table.check_flag(params);
}
