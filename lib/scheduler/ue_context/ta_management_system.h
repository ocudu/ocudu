// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "logical_channel_system.h"
#include "ocudu/adt/circular_vector.h"
#include "ocudu/adt/soa_table.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/ran/time_alignment_config.h"
#include "ocudu/scheduler/config/scheduler_expert_config.h"
#include "ocudu/support/math/exponential_averager.h"

namespace ocudu {

class ue_ta_manager;

class ta_management_system
{
  /// Maximum number of TAG measurements supported per UE.
  static constexpr size_t      MAX_NOF_TAG_MEASUREMENTS = 4;
  static constexpr soa::row_id invalid_row_id{std::numeric_limits<uint32_t>::max()};

public:
  explicit ta_management_system(const scheduler_ta_control_config& ta_cfg_);

  /// \brief Adds a new UE to the TA management system.
  /// \param pcell_tag_id TAG of the UE PCell.
  /// \param ul_scs UE UL BWP SCS.
  /// \param lc_ch_mgr_ UE logical channels, which carry its TA commands.
  /// \param ntn_koffset K_offset in slots, zero outside NTN, as per TS 38.213, Section 4.2.
  ue_ta_manager add_ue(time_alignment_group::id_t         pcell_tag_id,
                       subcarrier_spacing                 ul_scs,
                       ue_logical_channel_repository_view lc_ch_mgr_,
                       unsigned                           ntn_koffset = 0);

  /// \brief Handles Timing Advance adaptation related tasks at slot indication.
  void slot_indication(slot_point sl_tx);

  /// Number of active UEs in the TA management system.
  unsigned nof_active_ues() const { return static_cast<unsigned>(ues.size()); }

private:
  friend class ue_ta_manager;
  /// Decay factor for N_TA difference exponential averaging.
  static constexpr double n_ta_diff_avg_decay = 0.01;
  /// TA command offset for a zero value.
  static constexpr int ta_cmd_offset_zero = 31;

  struct tag_measurement {
    /// TAG ID associated to this measurement.
    time_alignment_group::id_t tag_id;
    /// \brief Exponential average of N_TA differences for outlier detection.
    /// This average does not get reset on each measurement.
    exp_average_fast_start<double> n_ta_diff_averager{n_ta_diff_avg_decay};
    /// \brief Exponential average of squared N_TA differences for outlier detection.
    /// This average does not get reset on each measurement.
    exp_average_fast_start<double> n_ta_diff_sq_averager{n_ta_diff_avg_decay};
    /// Number of samples collected until outlier detection is enabled.
    uint32_t count_until_outlier_detection = 0;
    /// Time count at which forbid period started. If absent, forbid period is not active.
    std::optional<unsigned> forbid_period_start;
    /// Number of samples within the current measurement window.
    uint32_t window_count_samples = 0;
    /// Sum of samples within the current measurement window.
    int64_t window_sum_samples = 0;
    /// Last computed Timing Advance Command value.
    int last_t_a = ta_cmd_offset_zero;
  };

  struct ue_ta_context {
    /// Uplink subcarrier spacing of the UE.
    subcarrier_spacing ul_scs;
    /// Slots K_offset delays a TA command, as per TS 38.213, Section 4.2. Earlier N_TA updates are stale.
    unsigned ta_cmd_apply_delay = 0;
    /// Logical channel manager for the UE.
    ue_logical_channel_repository_view lc_ch_mgr;
    /// List of N_TA update (N_TA_new - N_TA_old value in T_C units) measurements maintained per Timing Advance Group.
    /// The array index corresponds to TAG ID. And, the corresponding array value (i.e. vector) holds N_TA update
    /// measurements for that TAG ID.
    static_vector<tag_measurement, MAX_NOF_TAG_MEASUREMENTS> n_ta_reports;
  };

  struct wheel_list_node {
    /// Position in the time wheel.
    std::optional<unsigned> wheel_meas_pos;
    /// Time at which the UE started the measurement period.
    std::optional<unsigned> meas_start_time;
    /// Next node in the linked list of a given time wheel slot.
    soa::row_id next = invalid_row_id;
  };

  /// Each slot in the time wheel.
  struct time_wheel_slot {
    /// Linked list head of UEs scheduled to complete their measurement at this time wheel slot.
    soa::row_id head = invalid_row_id;
  };

  /// Remove a UE from the TA management system.
  void rem_ue(soa::row_id ue_id);

  /// Handle all pending TA commands for a given UE.
  void handle_ue_ta_cmds(ue_ta_context& u);

  void handle_ul_n_ta_update_indication(soa::row_id                ue_id,
                                        time_alignment_group::id_t tag_id,
                                        int64_t                    n_ta_diff_,
                                        float                      ul_sinr);

  /// \brief Computes the average of N_TA update measurements.
  static int64_t compute_avg_n_ta_difference(const tag_measurement& ta_meas);

  /// \brief Computes new Timing Advance Command value (T_A) as per TS 38.213, clause 4.2.
  /// \return Timing Advance Command value. Values [0,...,63].
  unsigned compute_new_t_a(int64_t n_ta_diff, subcarrier_spacing ul_scs) const;

  void update_tags(soa::row_id ue_id, span<const time_alignment_group::id_t> tag_ids);

  const scheduler_ta_control_config ta_cfg;
  ocudulog::basic_logger&           logger;

  enum class ue_component { context, wheel_next_node };
  soa::table<ue_component, ue_ta_context, wheel_list_node> ues;

  /// \brief Time wheel for UEs.
  /// Each entry index corresponds to a slot offset within a window of size prohibit+measurement period.
  /// Each entry points to a linked list of UE row IDs, which are scheduled to complete their measurement.
  circular_vector<time_wheel_slot, true> time_wheel;

  /// Current index in the time wheel.
  unsigned next_wheel_index = 0;
};

/// Handle to the TA manager of a single UE.
class ue_ta_manager
{
  ue_ta_manager(ta_management_system& parent_, soa::row_id ue_id_) : parent(&parent_), ue_id(ue_id_) {}

public:
  ue_ta_manager()                     = default;
  ue_ta_manager(const ue_ta_manager&) = delete;
  ue_ta_manager(ue_ta_manager&& other) noexcept :
    parent(std::exchange(other.parent, nullptr)),
    ue_id(std::exchange(other.ue_id, ta_management_system::invalid_row_id))
  {
  }
  ue_ta_manager& operator=(const ue_ta_manager&) = delete;
  ue_ta_manager& operator=(ue_ta_manager&& other) noexcept
  {
    reset();
    parent = std::exchange(other.parent, nullptr);
    ue_id  = std::exchange(other.ue_id, ta_management_system::invalid_row_id);
    return *this;
  }
  ~ue_ta_manager();

  /// \brief Resets the TA manager, disabling TA management for the UE.
  void reset();

  /// Whether the TA management is active for the UE.
  bool active() const { return parent != nullptr and ue_id != ta_management_system::invalid_row_id; }

  /// \brief Updates the list of configured TAG IDs for the UE.
  void update_tags(span<const time_alignment_group::id_t> tag_ids)
  {
    ocudu_sanity_check(parent != nullptr, "TA manager is not in valid state");
    if (active()) {
      parent->update_tags(ue_id, tag_ids);
    }
  }

  /// \brief Handles N_TA update indication.
  void handle_ul_n_ta_update_indication(time_alignment_group::id_t tag_id, int64_t n_ta_diff_, float ul_sinr)
  {
    ocudu_sanity_check(parent != nullptr, "TA manager is not in valid state");
    if (active()) {
      parent->handle_ul_n_ta_update_indication(ue_id, tag_id, n_ta_diff_, ul_sinr);
    }
  }

private:
  friend class ta_management_system;

  ta_management_system* parent = nullptr;
  soa::row_id           ue_id{ta_management_system::invalid_row_id};
};

} // namespace ocudu
