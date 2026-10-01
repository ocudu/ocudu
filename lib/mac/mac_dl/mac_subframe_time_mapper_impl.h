// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/mac_cell_manager.h"
#include "ocudu/mac/mac_cell_slot_handler.h"
#include "ocudu/mac/mac_cell_timing_context.h"
#include "ocudu/mac/mac_subframe_time_mapper.h"
#include "ocudu/ocudulog/logger.h"

namespace ocudu {

/// \brief Lock-free atomic mapper that maintains a reference mapping between slot point and time point.
///
/// This class stores and retrieves mappings between System Frame Number (SFN), subframe index, and system
/// timestamp using a single 64-bit atomic variable, enabling efficient lock-free concurrent access.
///
/// The 64-bit value packs:
/// - Upper 14 bits: SFN (10 bits) + subframe index (4 bits)
/// - Lower 50 bits: Truncated timestamp (nanoseconds since epoch)
///
/// Updates occur only at subframe boundaries (subframe_slot_index == 0), providing a reference mapping every 1ms.
/// The full timestamp is recovered from the 50-bit truncated value using the current system time. Since the
/// truncated timestamp wraps every ~13 days and typical query latency is <1ms, reconstruction is always accurate.
class atomic_subframe_time_mapper
{
public:
  using time_point = std::chrono::time_point<std::chrono::system_clock>;

  atomic_subframe_time_mapper() = default;

  /// \brief Atomically stores the SFN, subframe index and the associated timestamp.
  /// Only stores mapping when the slot is at subframe boundary (subframe_slot_index == 0).
  /// \param slot The slot point containing SFN and slot index.
  /// \param time The current system time to be stored.
  /// \return true if the mapping was stored (first mapping for this subframe), false otherwise.
  bool store(slot_point slot, time_point time);

  /// \brief Loads the last stored SFN/subframe and timestamp atomically.
  /// \param numerology The numerology to use for slot point reconstruction.
  /// \param now The current system time, defaults to `std::chrono::system_clock::now()`.
  /// \return A structure containing the last stored SFN/subframe and timestamp.
  mac_slot_time_info load(unsigned numerology, time_point now = std::chrono::system_clock::now()) const;

private:
  /// Number of bits used to store the timestamp.
  static constexpr uint64_t timestamp_bits = 50;
  /// Number of bits used to store the Sub-Frame Index (4bits).
  static constexpr uint64_t sf_idx_bits = 4;
  /// Number of bits used to store the System Frame Number (10bits) and Sub-Frame Index (4bits).
  static constexpr uint64_t sfn_bits = 14;
  /// The period for timestamp wraparound (2^50 nanoseconds ≈ 13 days).
  static constexpr uint64_t period = 1ULL << timestamp_bits;
  /// Mask to extract the timestamp (lower 50 bits).
  static constexpr uint64_t timestamp_mask = (1ULL << timestamp_bits) - 1;
  /// Mask to extract the SFN (upper 14 bits).
  static constexpr uint64_t sfn_mask = ((1ULL << sfn_bits) - 1) << timestamp_bits;

  /// \brief Packs the SFN, subframe index and system time (after truncation) into a 64-bit value.
  /// \param slot The slot point containing SFN and slot index.
  /// \param time The current system time to be stored.
  /// \return A 64-bit integer representing the packed SFN/subframe and truncated timestamp.
  static uint64_t pack_sfn_time_mapping(slot_point slot, time_point time);

  /// \brief Extracts the SFN and subframe index from the packed 64-bit value.
  /// \param packed_mapping The packed SFN/subframe and truncated timestamp.
  /// \param numerology The numerology used for slot reconstruction.
  /// \return The reconstructed slot point.
  static slot_point load_sfn(uint64_t packed_mapping, unsigned numerology);

  /// \brief Reconstructs the full timestamp from the packed 64-bit value.
  /// Since only the lower 50 bits of the timestamp are stored, this function reconstructs the full timestamp by
  /// leveraging the fact that timestamps change slowly (i.e., wrap around roughly every 13 days). It adjusts the
  /// truncated timestamp based on the current time to obtain the correct value.
  /// \param packed_mapping The packed SFN/subframe and truncated timestamp.
  /// \param now The current system time to assist in reconstruction.
  /// \return The reconstructed full timestamp.
  static time_point load_time(uint64_t packed_mapping, time_point now);

  /// \brief Extracts the packed SFN and subframe index (without timestamp) from a packed mapping.
  static uint64_t extract_sfn_sf_idx(uint64_t packed_mapping);

  /// Atomic storage for the packed SFN/subframe and truncated timestamp.
  std::atomic<uint64_t> mapping = {0};
};

/// \brief Class that provides mapping between slot point and time point at subframe level.
/// This class provides a mechanism to track and convert between slot points (SFN, slot index) and time points (system
/// timestamps). It maintains an atomic reference SFN-Timestamp mapping updated every 1ms that is used for requested
/// conversion. Unlike mac_cell_time_mapper_impl, this class receives indications from all cells and only takes the
/// first mapping for each subframe.
class mac_subframe_time_mapper_impl final : public mac_subframe_time_mapper, public mac_slot_time_handler
{
  using usecs = std::chrono::microseconds;

public:
  mac_subframe_time_mapper_impl();

  /// \brief Handles a slot indication and updates the slot-to-time mapping.
  /// The mapping is stored only for the full sub-frames (i.e., every 1ms) and only if no mapping exists for that
  /// subframe yet.
  /// \param context The MAC cell timing context containing slot and time information.
  void handle_slot_indication(const mac_cell_timing_context& context) override;

  /// \brief Retrieves the last known slot point-time point mapping for the given subcarrier spacing.
  /// Note that only SFN-Timestamp are stored, hence it is updated every 1 ms.
  /// \param scs The subcarrier spacing to use for slot point reconstruction.
  std::optional<mac_slot_time_info> get_last_mapping(subcarrier_spacing scs) const override;

  /// Converts a given slot point to a corresponding time point.
  std::optional<time_point> get_time_point(slot_point slot) const override;

  /// Converts a given time point to a corresponding slot point for the given subcarrier spacing.
  std::optional<slot_point> get_slot_point(time_point time, subcarrier_spacing scs) const override;

private:
  ocudulog::basic_logger& logger;

  /// Atomic SFN-Timestamp mapping used as reference to compute any requested slot point-time point mapping.
  /// This is the core reference mapping that links a System Frame Number (SFN), subframe index and a timestamp.
  atomic_subframe_time_mapper cur_slot_time_mapping;

  /// Duration of a full subframe (1 millisecond).
  static constexpr usecs subframe_dur = usecs{1000};

  /// Duration of a full system frame (10 milliseconds).
  static constexpr usecs frame_dur = usecs{10000};
};

} // namespace ocudu
