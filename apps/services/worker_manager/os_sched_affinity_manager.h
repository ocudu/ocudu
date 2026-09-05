// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/enum_utils.h"
#include "ocudu/support/executors/unique_thread.h"

namespace ocudu {

/// Types of CPU affinity masks.
enum class sched_affinity_mask_types : unsigned { ru, main, last };

/// Thread pinning policy to a CPU affinity mask.
enum class sched_affinity_mask_policy {
  /// Each thread is pinned to a single CPU in a mask in RR fashion.
  round_robin = 0,
  /// All threads are sharing the same affinity mask.
  mask = 1,
  last
};

/// Converts the given affinity mask policy into a string.
inline std::string to_string(sched_affinity_mask_policy policy)
{
  switch (policy) {
    case sched_affinity_mask_policy::mask:
      return "mask";
    case sched_affinity_mask_policy::round_robin:
      return "round-robin";
    default:
      ocudu_assert(0, "Invalid affinity mask policy");
      break;
  }
  return {};
}

/// Converts the given sting into an affinity mask policy or returns last if it could not convert it.
inline sched_affinity_mask_policy to_affinity_mask_policy(const std::string& value)
{
  if (value == "round-robin") {
    return sched_affinity_mask_policy::round_robin;
  }
  if (value == "mask") {
    return sched_affinity_mask_policy::mask;
  }

  return sched_affinity_mask_policy::last;
}

/// Converts and returns the given value to an affinity mask type.
inline sched_affinity_mask_types to_affinity_mask_type(unsigned value)
{
  return static_cast<sched_affinity_mask_types>(value);
}

/// CPU affinity configuration.
struct os_sched_affinity_config {
  /// Affinity type.
  sched_affinity_mask_types type = sched_affinity_mask_types::last;
  /// Affinity mask.
  os_sched_affinity_bitmask mask;
  /// Thread pinning policy.
  sched_affinity_mask_policy pinning_policy = sched_affinity_mask_policy::last;
};

/// \brief Scheduler affinity mask manager.
///
/// Manages the CPU affinity mask for the application. It calculates and returns a mask using on a round-robin algorithm
/// on a configured mask.
class os_sched_affinity_manager
{
  /// Interface of the class managing affinity mask assignments depending on the policy.
  class affinity_entry
  {
  public:
    explicit affinity_entry(const os_sched_affinity_bitmask& mask_) : mask(mask_) {}
    virtual ~affinity_entry() = default;

    virtual os_sched_affinity_bitmask calculate_mask() = 0;

  protected:
    os_sched_affinity_bitmask mask;
  };

  /// Helper class that manages the mask and implements the round-robin algorithm.
  class affinity_entry_rr : public affinity_entry
  {
  public:
    explicit affinity_entry_rr(const os_sched_affinity_bitmask& mask_) : affinity_entry(mask_) {}

    /// Calculates and returns a bitmask using a round-robin algorithm over the configured mask. If no CPU is enabled in
    /// the configured mask, it returns the default mask.
    os_sched_affinity_bitmask calculate_mask() override
    {
      // No CPU is selected.
      if (mask.none()) {
        return {};
      }

      int pos = -1;

      while (pos < 0) {
        pos = mask.find_lowest(last_pos_used, mask.size());

        last_pos_used = (pos < 0) ? 0 : pos + 1;
      }

      return os_sched_affinity_bitmask(mask.size(), pos);
    }

  private:
    unsigned last_pos_used = 0;
  };

  /// Helper class that manages the mask and return user-defined mask.
  class affinity_entry_mask : public affinity_entry
  {
  public:
    explicit affinity_entry_mask(const os_sched_affinity_bitmask& mask_) : affinity_entry(mask_) {}

    os_sched_affinity_bitmask calculate_mask() override { return mask; }
  };

public:
  os_sched_affinity_manager(const std::vector<os_sched_affinity_config>& entries) :
    cpu_masks(to_underlying(sched_affinity_mask_types::last))
  {
    for (const auto& entry : entries) {
      ocudu_assert(entry.type != sched_affinity_mask_types::last, "Invalid type '{}'", to_underlying(entry.type));
      switch (entry.pinning_policy) {
        case sched_affinity_mask_policy::round_robin:
          cpu_masks[to_underlying(entry.type)] = std::make_unique<affinity_entry_rr>(entry.mask);
          break;
        case sched_affinity_mask_policy::mask:
          cpu_masks[to_underlying(entry.type)] = std::make_unique<affinity_entry_mask>(entry.mask);
          break;
        default:
          cpu_masks[to_underlying(entry.type)] = nullptr;
      }
    }
  }

  /// Calculate and returns the affinity mask for the given type.
  os_sched_affinity_bitmask calcute_affinity_mask(sched_affinity_mask_types type) const
  {
    ocudu_assert(type != sched_affinity_mask_types::last, "Invalid type '{}'", to_underlying(type));
    ocudu_assert(cpu_masks[to_underlying(type)], "Invalid manager for type '{}'", to_underlying(type));

    return cpu_masks[to_underlying(type)]->calculate_mask();
  }

private:
  std::vector<std::unique_ptr<affinity_entry>> cpu_masks;
};

} // namespace ocudu
