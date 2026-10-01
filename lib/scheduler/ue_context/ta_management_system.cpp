// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ta_management_system.h"
#include "logical_channel_system.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/math/math_utils.h"
#include <cmath>

using namespace ocudu;

/// Returns the TA command processing time k, in UL slots, as per TS 38.213, Section 4.2.
static unsigned get_ta_cmd_processing_slots(subcarrier_spacing ul_scs, subcarrier_spacing dl_scs)
{
  // N_1 with additional PDSCH DM-RS and N_2, of UE processing capability 1, at the minimum of the UL and DL SCS, as
  // per TS 38.213, Section 4.2 and TS 38.214, Tables 5.3-1 and 6.4-1. N_1,0 is 14.
  const subcarrier_spacing proc_scs   = std::min(ul_scs, dl_scs);
  unsigned                 n1_symbols = 0;
  unsigned                 n2_symbols = 0;
  switch (proc_scs) {
    case subcarrier_spacing::kHz15:
      n1_symbols = 14;
      n2_symbols = 10;
      break;
    case subcarrier_spacing::kHz30:
      n1_symbols = 13;
      n2_symbols = 12;
      break;
    case subcarrier_spacing::kHz60:
      n1_symbols = 20;
      n2_symbols = 23;
      break;
    case subcarrier_spacing::kHz120:
      n1_symbols = 24;
      n2_symbols = 36;
      break;
    default:
      report_fatal_error("Unsupported SCS={} for the TA command processing time", scs_to_khz(proc_scs));
  }

  // T_c = 1 / (480 kHz * 4096) and kappa = T_s / T_c = 64, as per TS 38.211, Section 4.1.
  static constexpr unsigned tc_per_ms = 480 * 4096;
  static constexpr unsigned kappa     = 64;
  const unsigned            proc_mu   = to_numerology_value(proc_scs);
  const unsigned            ul_mu     = to_numerology_value(ul_scs);
  // A normal cyclic prefix symbol spans (2048 + 144) * kappa * 2^-mu T_c, as per TS 38.211, Section 5.3.1. It is the
  // per symbol factor of T_proc,1 and T_proc,2, as per TS 38.214, Sections 5.3 and 6.4.
  const unsigned symbols_tc = (n1_symbols + n2_symbols) * (2048 + 144) * kappa / (1U << proc_mu);
  // N_TA,max from T_A = 3846, as per TS 38.213, Section 4.2. N_TA,max and N_slot use the UL SCS.
  const unsigned n_ta_max_tc = 3846 * 16 * kappa / (1U << ul_mu);

  return divide_ceil(get_nof_slots_per_subframe(ul_scs) * (symbols_tc + n_ta_max_tc + tc_per_ms / 2), tc_per_ms);
}

ta_management_system::ta_management_system(const scheduler_ta_control_config& ta_cfg_) :
  ta_cfg(ta_cfg_),
  logger(ocudulog::fetch_basic_logger("SCHED")),
  // Note: The window size needs to be strictly larger than the measurement period to avoid ambiguity. 4 is an
  // arbitrary value. The size is rounded up to a power of 2, which also keeps the wheel index continuous when the
  // wheel counter wraps around.
  time_wheel(ta_cfg.ta_cmd_offset_threshold >= 0 ? ta_cfg.measurement_period + 4 : 0)
{
  if (ta_cfg.ta_cmd_offset_threshold < 0) {
    // TA management disabled.
    return;
  }
  ocudu_assert(ta_cfg.measurement_period > 0, "Invalid measurement period");

  ues.reserve(MAX_NOF_DU_UES);
}

ue_ta_manager ta_management_system::add_ue(time_alignment_group::id_t         pcell_tag_id,
                                           subcarrier_spacing                 ul_scs,
                                           subcarrier_spacing                 dl_scs,
                                           ue_logical_channel_repository_view lc_ch_mgr,
                                           unsigned                           ntn_koffset)
{
  if (ta_cfg.ta_cmd_offset_threshold < 0) {
    // TA management disabled for this UE.
    return ue_ta_manager{*this, invalid_row_id};
  }

  // Create UE context.
  // The UE applies a TA command in UL slot n + k + 1 + 2^mu * K_offset, as per TS 38.213, Section 4.2.
  const unsigned ta_cmd_apply_delay = get_ta_cmd_processing_slots(ul_scs, dl_scs) + 1 + ntn_koffset;
  auto row_id = ues.insert(ue_ta_context{ul_scs, ta_cmd_apply_delay, std::move(lc_ch_mgr)}, wheel_list_node{});
  update_tags(row_id, std::array<time_alignment_group::id_t, 1>{pcell_tag_id});

  return ue_ta_manager{*this, row_id};
}

void ta_management_system::rem_ue(soa::row_id ue_id)
{
  // Remove UE from the wheel if present.
  auto& ue_node = ues.at<ue_component::wheel_next_node>(ue_id);
  if (ue_node.wheel_meas_pos.has_value()) {
    auto&       wheel_head = time_wheel[ue_node.wheel_meas_pos.value()].head;
    soa::row_id node       = wheel_head;
    // Iterate through linked list to find and remove UE.
    for (soa::row_id prev = invalid_row_id; node != invalid_row_id;) {
      if (node == ue_id) {
        if (prev == invalid_row_id) {
          // Head node.
          wheel_head = ue_node.next;
        } else {
          auto& prev_next = ues.at<ue_component::wheel_next_node>(prev);
          prev_next.next  = ue_node.next;
        }
        break;
      }
      prev = node;
      node = ues.at<ue_component::wheel_next_node>(node).next;
    }
    ocudu_assert(node != invalid_row_id, "UE not found in time wheel");
  }

  // Remove UE from repository.
  ues.erase(ue_id);
}

void ta_management_system::slot_indication(slot_point sl_tx)
{
  if (ta_cfg.ta_cmd_offset_threshold < 0) {
    // TA management disabled.
    return;
  }

  // Select a time wheel position based on the current index.
  auto& wheel_head = time_wheel[next_wheel_index].head;
  ++next_wheel_index;

  // Consider that all UEs in the wheel slot have completed their measurements.
  for (soa::row_id ue_id = wheel_head; ue_id != invalid_row_id;) {
    // Handle pending TA command.
    ue_ta_context& u = ues.at<ue_component::context>(ue_id);
    handle_ue_ta_cmds(u);

    // Remove UE from the wheel linked list and reset its wheel state.
    wheel_list_node& ue_node = ues.at<ue_component::wheel_next_node>(ue_id);
    ue_id                    = ue_node.next;
    ue_node                  = {};
  }

  // Reset wheel slot head.
  wheel_head = invalid_row_id;
}

void ta_management_system::handle_ue_ta_cmds(ue_ta_context& u)
{
  for (unsigned tag_idx = 0; tag_idx != u.n_ta_reports.size(); ++tag_idx) {
    tag_measurement& ta_meas = u.n_ta_reports[tag_idx];
    if (ta_meas.window_count_samples == 0) {
      // No samples collected within the measurement window for this TAG.
      continue;
    }

    bool ta_cmd_sent = false;
    if (abs(ta_meas.last_t_a - ta_cmd_offset_zero) >= ta_cfg.ta_cmd_offset_threshold) {
      // This TAG has a pending TA CMD to be sent.
      // Note: We need to check the threshold again here, because it could happen that a UE was added to the wheel, but
      // later last_t_a was updated with a value below the threshold.

      // Send Timing Advance Command to UE.
      const time_alignment_group::id_t tag_id = ta_meas.tag_id;
      const unsigned                   ta_cmd = std::max(0, ta_meas.last_t_a);
      u.lc_ch_mgr.handle_mac_ce_indication(
          {.ce_lcid = lcid_dl_sch_t::TA_CMD, .ce_payload = ta_cmd_ce_payload{tag_id, ta_cmd}});
      ta_cmd_sent = true;
    }

    // Reset stored measurements within the measurement window.
    ta_meas.window_count_samples = 0;
    ta_meas.window_sum_samples   = 0;
    ta_meas.last_t_a             = ta_cmd_offset_zero;
    ta_meas.forbid_period_start  = std::nullopt;
    if (ta_cmd_sent) {
      ta_meas.forbid_period_start = next_wheel_index;
    }
  }
}

void ta_management_system::update_tags(soa::row_id ue_id, span<const time_alignment_group::id_t> tag_ids)
{
  // TODO: Avoid losing all history.
  auto& ue_ctxt = ues.at<ue_component::context>(ue_id);
  ue_ctxt.n_ta_reports.resize(tag_ids.size());
  for (unsigned i = 0, e = ue_ctxt.n_ta_reports.size(); i != e; ++i) {
    ue_ctxt.n_ta_reports[i].tag_id = tag_ids[i];
    ue_ctxt.n_ta_reports[i].n_ta_diff_averager.reset();
    ue_ctxt.n_ta_reports[i].n_ta_diff_sq_averager.reset();
    ue_ctxt.n_ta_reports[i].count_until_outlier_detection = 0;
    ue_ctxt.n_ta_reports[i].window_sum_samples            = 0;
    ue_ctxt.n_ta_reports[i].window_count_samples          = 0;
    ue_ctxt.n_ta_reports[i].last_t_a                      = ta_cmd_offset_zero;
  }
}

unsigned ta_management_system::compute_new_t_a(int64_t n_ta_diff, subcarrier_spacing ul_scs) const
{
  const float ta_offset = static_cast<float>(ta_cmd_offset_zero) - ta_cfg.target;
  return static_cast<unsigned>(std::round(
      static_cast<float>(n_ta_diff * get_nof_slots_per_subframe(ul_scs)) / static_cast<float>(16U * 64U) + ta_offset));
}

int64_t ta_management_system::compute_avg_n_ta_difference(const tag_measurement& report)
{
  return report.window_count_samples > 0 ? report.window_sum_samples / static_cast<int64_t>(report.window_count_samples)
                                         : ta_cmd_offset_zero;
}

/// \brief Determines whether a sample is an outlier based on Welford's algorithm.
static bool is_outlier(double sample, double mean, double sq_mean, double thres)
{
  double var = sq_mean - mean * mean;
  // small numerical errors can lead to negative variance.
  var            = var < 0.0 ? 0.0 : var;
  double std_dev = std::sqrt(var);

  if (std_dev == 0) {
    // There is no spread, so no outlier detection possible. Accept all samples.
    return false;
  }

  // [Implementation-defined] Welford's algorithm z-threshold. Adjust this threshold as needed.
  // There is spread, so we can apply the outlier detection algorithm.
  return std::abs(sample - mean) > thres * std_dev;
}

void ta_management_system::handle_ul_n_ta_update_indication(soa::row_id                ue_id,
                                                            time_alignment_group::id_t tag_id,
                                                            int64_t                    n_ta_diff_,
                                                            float                      ul_sinr)
{
  if (ul_sinr <= ta_cfg.update_measurement_ul_sinr_threshold) {
    // [Implementation-defined] Discard measurement due to low UL SINR.
    // NOTE: From the testing with COTS UE its observed that N_TA update measurements with UL SINR less than 10 dB were
    // majorly outliers.
    return;
  }

  // Find respective TAG-ID measurement context.
  ue_ta_context& u  = ues.at<ue_component::context>(ue_id);
  auto*          it = std::find_if(
      u.n_ta_reports.begin(), u.n_ta_reports.end(), [tag_id](const auto& meas) { return meas.tag_id == tag_id; });
  if (it == u.n_ta_reports.end()) {
    logger.warning("Discarding TA report. Cause: TAG Id {} is not configured", tag_id.value());
    return;
  }
  tag_measurement& tag_meas = *it;

  // Check if the TAG measurement is within the forbid period.
  if (tag_meas.forbid_period_start.has_value()) {
    if (next_wheel_index - tag_meas.forbid_period_start.value() <
        ta_cfg.measurement_prohibit_period + u.ta_cmd_apply_delay) {
      // Within forbid period, discard measurement.
      return;
    }
    tag_meas.forbid_period_start = std::nullopt;
  }

  // If enabled, decide whether to filter out outlier using Welford's algorithm and using an exponential average.
  if (ta_cfg.outlier_detection_zscore_threshold > 0.0F) {
    // Update exponential averages for outlier detection.
    tag_meas.n_ta_diff_averager.push(n_ta_diff_);
    tag_meas.n_ta_diff_sq_averager.push(n_ta_diff_ * n_ta_diff_);

    static constexpr unsigned min_samples_for_outlier_detection = 10;
    if (tag_meas.count_until_outlier_detection < min_samples_for_outlier_detection) {
      // Note: for small number of samples, outlier detection is not performed.
      ++tag_meas.count_until_outlier_detection;
    } else if (is_outlier(n_ta_diff_,
                          tag_meas.n_ta_diff_averager.average(),
                          tag_meas.n_ta_diff_sq_averager.average(),
                          ta_cfg.outlier_detection_zscore_threshold)) {
      // Outlier detected, discard measurement.
      return;
    }
  }

  // Passed outlier detection. Update statistics for this TAG measurement.
  tag_meas.window_sum_samples += n_ta_diff_;
  ++tag_meas.window_count_samples;
  tag_meas.last_t_a = compute_new_t_a(compute_avg_n_ta_difference(tag_meas), u.ul_scs);

  // If UE has already pending TA CMDs, no need to add it again to the time wheel.
  wheel_list_node& ue_node = ues.at<ue_component::wheel_next_node>(ue_id);
  if (ue_node.wheel_meas_pos.has_value()) {
    // UE is already in the time wheel.
    return;
  }

  if (not ue_node.meas_start_time.has_value()) {
    // First measurement in the measurement window for this UE. Set the measurement start time.
    ue_node.meas_start_time = next_wheel_index - 1;
  }

  // Send Timing Advance command only if the offset is equal to or greater than the threshold.
  // The new Timing Advance Command is a value ranging from [0,...,63] as per TS 38.213, clause 4.2. Hence, we
  // need to subtract a value of 31 (as per equation in the same clause) to get the change in Timing Advance
  // Command.
  const bool new_t_a_passed = std::abs(tag_meas.last_t_a - ta_cmd_offset_zero) >= ta_cfg.ta_cmd_offset_threshold;
  if (new_t_a_passed) {
    // When the new TA command passes the threshold and the UE is not already in the time wheel, we add it.
    // For that reason, we push this UE to the time wheel.
    unsigned offset        = (ue_node.meas_start_time.value() + ta_cfg.measurement_period);
    ue_node.wheel_meas_pos = offset;
    auto& wheel_head       = time_wheel[offset].head;
    ue_node.next           = wheel_head;
    wheel_head             = ue_id;
  }
}

ue_ta_manager::~ue_ta_manager()
{
  reset();
}

void ue_ta_manager::reset()
{
  if (active()) {
    parent->rem_ue(ue_id);
    parent = nullptr;
    ue_id  = ta_management_system::invalid_row_id;
  }
}
