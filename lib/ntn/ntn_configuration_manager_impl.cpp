// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ntn_configuration_manager_impl.h"
#include "ntn_sib19_helpers.h"
#include "ocudu/ntn/ntn_doppler_compensation_handler.h"
#include "ocudu/ntn/ntn_sib19_update_handler.h"
#include "ocudu/ran/sib/system_info_config.h"
#include "ocudu/support/format/delimited_formatter.h"
#include "fmt/chrono.h"

using namespace ocudu;
using namespace ocudu_ntn;

namespace {

/// Structure containing assistance information for formatting.
struct assistance_info_wrapper {
  slot_point                                              si_window_start;
  slot_point                                              si_window_end;
  slot_point                                              epoch_slot;
  std::chrono::system_clock::time_point                   epoch_time;
  std::optional<ta_info_t>                                ta_info;
  std::variant<ecef_coordinates_t, orbital_coordinates_t> ephemeris_info;
};

} // namespace

namespace fmt {

/// \brief Custom formatter for \c assistance_info_wrapper.
template <>
struct formatter<assistance_info_wrapper> {
  ocudu::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const assistance_info_wrapper& info, FormatContext& ctx) const
  {
    // Format SIB19 update information.
    helper.format_always(ctx, "si_window={}-{}", info.si_window_start, info.si_window_end);
    helper.format_always(ctx, "epoch_slot={}", info.epoch_slot);
    helper.format_always(ctx, "epoch_time={:%T}", info.epoch_time);

    // Format TA-info.
    if (info.ta_info.has_value()) {
      helper.format_if_verbose(ctx, "ta_common={:.3f}us", info.ta_info->ta_common);
      helper.format_if_verbose(ctx, "ta_common_drift={:.3f}us/s", info.ta_info->ta_common_drift);
      helper.format_if_verbose(ctx, "ta_common_drift_variant={:.3f}us/s2", info.ta_info->ta_common_drift_variant);
      helper.format_if_verbose(ctx, "ta_common_offset={:.3f}us", info.ta_info->ta_common_offset);
    }

    // Format ephemeris information.
    if (std::holds_alternative<ecef_coordinates_t>(info.ephemeris_info)) {
      const auto& ecef = std::get<ecef_coordinates_t>(info.ephemeris_info);
      helper.format_if_verbose(ctx, "ephemeris_type=ecef");
      helper.format_if_verbose(
          ctx, "pos_x={:.3f}m pos_y={:.3f}m pos_z={:.3f}m", ecef.position_x, ecef.position_y, ecef.position_z);
      helper.format_if_verbose(
          ctx, "vel_x={:.3f}m/s vel_y={:.3f}m/s vel_z={:.3f}m/s", ecef.velocity_vx, ecef.velocity_vy, ecef.velocity_vz);
    } else {
      const auto& orbital = std::get<orbital_coordinates_t>(info.ephemeris_info);
      helper.format_if_verbose(ctx, "ephemeris_type=orbital");
      helper.format_if_verbose(ctx, "semi_major_axis={:.3f}m", orbital.semi_major_axis);
      helper.format_if_verbose(ctx, "eccentricity={:.6f}", orbital.eccentricity);
      helper.format_if_verbose(ctx, "periapsis={:.3f}rad", orbital.periapsis);
      helper.format_if_verbose(ctx, "longitude={:.3f}rad", orbital.longitude);
      helper.format_if_verbose(ctx, "mean_anomaly={:.3f}rad", orbital.mean_anomaly);
      helper.format_if_verbose(ctx, "inclination={:.3f}rad", orbital.inclination);
    }

    return ctx.out();
  }
};

} // namespace fmt

/// \brief Compute current Doppler shift in Hz based on TA drift.
/// \param ta_common_drift_us_per_s Timing advance drift [µs/s]
/// \param carrier_freq_hz Carrier frequency [Hz]
/// \return Doppler shift in Hz
static double compute_doppler_hz(double ta_common_drift_us_per_s, double carrier_freq_hz)
{
  return (ta_common_drift_us_per_s / 2.0) * 1e-6 * carrier_freq_hz;
}

/// \brief Doppler shift rate in Hz/s based on TA drift derivative.
/// \param ta_common_drift_variant_us_per_s2 Timing advance drift rate [µs/s²]
/// \param carrier_freq_hz Carrier frequency [Hz]
/// \return Doppler frequency rate (derivative) in Hz/s
static double compute_doppler_shift_rate_hz_per_s(double ta_common_drift_variant_us_per_s2, double carrier_freq_hz)
{
  return ta_common_drift_variant_us_per_s2 * 1e-6 * carrier_freq_hz;
}

static slot_point get_next_si_win_start(const ntn_cell_config& ntn_cell_cfg, slot_point cur_sl)
{
  // 2> The concerned SI message is configured in the schedulingInfoList2.
  // 3> Determine the integer value x = (si-WindowPosition -1) × w, where w is
  // the si-WindowLength. See TS 38 331 V17.0.0.
  unsigned x = (ntn_cell_cfg.si_window_position - 1) * ntn_cell_cfg.si_window_len_slots;

  // 3> The SI-window starts at the slot #a, where a = x mod N, in the radio
  // frame for which SFN mod T = FLOOR(x/N), where T is the si-Periodicity of
  // the concerned SI message and N is the number of slots in a radio frame as
  // specified in TS 38.213.
  const unsigned N = cur_sl.nof_slots_per_frame();
  const unsigned T = ntn_cell_cfg.si_period_rf;
  const unsigned a = x % N;

  // Compute the difference (delta) needed to reach the target slot reminders.
  unsigned sfn_delta  = (T + (x / N) - (cur_sl.sfn() % T)) % T;
  unsigned slot_delta = (N + a - cur_sl.slot_index()) % N;

  // If delta is zero, it means current_sfn already has the desired remainder.
  // Since we need new_sfn > current_sfn, we add one full period (T).
  if (sfn_delta == 0) {
    sfn_delta = T;
  }
  if (slot_delta) {
    sfn_delta -= 1;
  }
  return cur_sl + sfn_delta * N + slot_delta;
}

ntn_configuration_manager_impl::ntn_configuration_manager_impl(const ntn_configuration_manager_config& config,
                                                               ntn_configuration_manager_dependencies  dependencies) :
  logger(ocudulog::fetch_basic_logger("NTN")),
  cfg(config),
  sib19_pdu_update_handler(std::move(dependencies.sib19_msg_update_handler)),
  time_provider(std::move(dependencies.time_provider)),
  doppler_handler(std::move(dependencies.doppler_handler)),
  timers(dependencies.timers),
  executor(dependencies.executor)
{
  if (config.cells.empty()) {
    logger.error("NTN configuration manager initialized with empty cells vector");
    return;
  }

  for (const auto& cell_config : config.cells) {
    auto [it, inserted] = cells.try_emplace(cell_config.nr_cgi);
    if (!inserted) {
      logger.error("Duplicate nr_cgi {} in NTN configuration, skipping", cell_config.nr_cgi.nci);
      continue;
    }
    auto& ctx    = it->second;
    ctx.cell_cfg = cell_config;
    // Check if sat_switch field is enabled.
    ctx.sat_switch_enabled = cell_config.assistance_info.sat_switch_with_resync.has_value();

    if (cell_config.assistance_info.epoch_timestamp.has_value()) {
      ephemeris_info_update ephemeris_info;
      ephemeris_info.epoch_time     = *cell_config.assistance_info.epoch_timestamp;
      ephemeris_info.ephemeris_info = cell_config.assistance_info.ephemeris_info;
      ctx.ntn_info_generator.enqueue_ephemeris_info(ephemeris_info);

      if (cell_config.assistance_info.ntn_gateway_location.has_value()) {
        ntn_gateway_location_info gw_location;
        gw_location.service_start_time             = ephemeris_info.epoch_time;
        gw_location.ntn_gateway_location.latitude  = cell_config.assistance_info.ntn_gateway_location->latitude;
        gw_location.ntn_gateway_location.longitude = cell_config.assistance_info.ntn_gateway_location->longitude;
        gw_location.ntn_gateway_location.altitude  = cell_config.assistance_info.ntn_gateway_location->altitude;
        ctx.ntn_info_generator.enqueue_ntn_gw_location(gw_location);
      }
    }

    if (ctx.sat_switch_enabled) {
      const sat_switch_with_resync_t& sat_sw = *cell_config.assistance_info.sat_switch_with_resync;
      if (sat_sw.epoch_timestamp.has_value() && sat_sw.ntn_cfg.ephemeris_info.has_value()) {
        ephemeris_info_update sat_ephemeris_update;
        sat_ephemeris_update.epoch_time     = *sat_sw.epoch_timestamp;
        sat_ephemeris_update.ephemeris_info = *sat_sw.ntn_cfg.ephemeris_info;
        if (!ctx.sat_switch_info_generator.enqueue_ephemeris_info(sat_ephemeris_update)) {
          logger.warning("Failed to enqueue sat-switch ephemeris for cell {}", cell_config.nr_cgi.nci);
        }
        if (sat_sw.ntn_gateway_location.has_value()) {
          ntn_gateway_location_info sat_gw_location;
          sat_gw_location.service_start_time             = *sat_sw.epoch_timestamp;
          sat_gw_location.ntn_gateway_location.latitude  = sat_sw.ntn_gateway_location->latitude;
          sat_gw_location.ntn_gateway_location.longitude = sat_sw.ntn_gateway_location->longitude;
          sat_gw_location.ntn_gateway_location.altitude  = sat_sw.ntn_gateway_location->altitude;
          if (!ctx.sat_switch_info_generator.enqueue_ntn_gw_location(sat_gw_location)) {
            logger.warning("Failed to enqueue sat-switch gateway location for cell {}", cell_config.nr_cgi.nci);
          }
        }
      }
    }

    // Create per-cell timer for SIB19 updating task.
    auto si_period_ms = cell_config.si_period_rf * 10;
    ctx.timer         = timers.create_unique_timer(executor);
    ctx.timer.set(std::chrono::milliseconds(si_period_ms), [this, nr_cgi = cell_config.nr_cgi](timer_id_t tid) {
      // Check if cell context still exists before processing.
      auto ctx_it = cells.find(nr_cgi);
      if (ctx_it == cells.end()) {
        // Cell was removed, do not re-run timer.
        return;
      }

      auto cur_tp_sl = time_provider->get_last_mapping(subcarrier_spacing::kHz15);
      if (cur_tp_sl.has_value() and cur_tp_sl->slot_tx.valid()) {
        logger.debug("Run periodic config update task for cell {} at slot={}, time={:%T}",
                     nr_cgi.nci,
                     cur_tp_sl->slot_tx,
                     cur_tp_sl->time_point);
        periodic_ntn_config_update_task(nr_cgi, cur_tp_sl->time_point, cur_tp_sl->slot_tx);
      }

      ctx_it->second.timer.run();
    });
  }

  // Start all timers.
  for (auto& [cgi, ctx] : cells) {
    ctx.timer.run();
  }
}

ntn_config_update_result ntn_configuration_manager_impl::handle_ntn_config_update(const ntn_config_update_info& req)
{
  ntn_config_update_result result;

  if (req.cells.empty()) {
    logger.warning("Received empty NTN config update request");
    return result;
  }

  for (const auto& cell_req : req.cells) {
    if (handle_ntn_cell_config_update(cell_req)) {
      result.succeeded.push_back(cell_req.nr_cgi);
    } else {
      result.failed.push_back(cell_req.nr_cgi);
    }
  }

  return result;
}

bool ntn_configuration_manager_impl::handle_ntn_cell_config_update(const ntn_cell_config_update_info& cell_req)
{
  auto it = cells.find(cell_req.nr_cgi);
  if (it == cells.end()) {
    logger.warning("Received NTN config update for unknown cell: {}", cell_req.nr_cgi.nci);
    return false;
  }

  logger.debug("Received config update for cell {} - epoch time={:%T}, format={}",
               cell_req.nr_cgi.nci,
               cell_req.epoch_time,
               std::holds_alternative<ecef_coordinates_t>(cell_req.ephemeris_info) ? "ecef" : "orbital");

  auto& ctx = it->second;

  // Enqueue the update for epoch-gated application of all cell config fields.
  if (!ctx.deferred_cell_cfg_queue.try_push(cell_req)) {
    logger.warning("Deferred cell config queue full for cell {}, dropping update", cell_req.nr_cgi.nci);
    return false;
  }

  // Enqueue sat-switch ephemeris and gateway immediately (time-gated by sat_switch.epoch_timestamp).
  if (cell_req.sat_switch_with_resync.has_value()) {
    const sat_switch_with_resync_t& sat_sw = *cell_req.sat_switch_with_resync;
    if (sat_sw.epoch_timestamp.has_value() && sat_sw.ntn_cfg.ephemeris_info.has_value()) {
      ephemeris_info_update sat_ephemeris_update;
      sat_ephemeris_update.epoch_time     = *sat_sw.epoch_timestamp;
      sat_ephemeris_update.ephemeris_info = *sat_sw.ntn_cfg.ephemeris_info;
      if (!ctx.sat_switch_info_generator.enqueue_ephemeris_info(sat_ephemeris_update)) {
        logger.warning("Failed to enqueue sat-switch ephemeris for cell {}", cell_req.nr_cgi.nci);
        return false;
      }
      if (sat_sw.ntn_gateway_location.has_value()) {
        ntn_gateway_location_info sat_gw_location;
        sat_gw_location.service_start_time             = *sat_sw.epoch_timestamp;
        sat_gw_location.ntn_gateway_location.latitude  = sat_sw.ntn_gateway_location->latitude;
        sat_gw_location.ntn_gateway_location.longitude = sat_sw.ntn_gateway_location->longitude;
        sat_gw_location.ntn_gateway_location.altitude  = sat_sw.ntn_gateway_location->altitude;
        if (!ctx.sat_switch_info_generator.enqueue_ntn_gw_location(sat_gw_location)) {
          logger.warning("Failed to enqueue sat-switch gateway location for cell {}", cell_req.nr_cgi.nci);
          return false;
        }
      }
    }
  }

  // If provided, send NTN gateway location info to SIB19 NTN config generator.
  if (cell_req.ntn_gateway_location.has_value()) {
    ntn_gateway_location_info gw_location;
    gw_location.service_start_time             = cell_req.epoch_time;
    gw_location.ntn_gateway_location.latitude  = cell_req.ntn_gateway_location->latitude;
    gw_location.ntn_gateway_location.longitude = cell_req.ntn_gateway_location->longitude;
    gw_location.ntn_gateway_location.altitude  = cell_req.ntn_gateway_location->altitude;
    if (not ctx.ntn_info_generator.enqueue_ntn_gw_location(gw_location)) {
      return false;
    }
  }

  // Send Ephemeris Info to SIB19 NTN config generator.
  ephemeris_info_update ephemeris_info;
  ephemeris_info.epoch_time     = cell_req.epoch_time;
  ephemeris_info.ephemeris_info = cell_req.ephemeris_info;
  return ctx.ntn_info_generator.enqueue_ephemeris_info(ephemeris_info);
}

void ntn_configuration_manager_impl::apply_deferred_cell_config_update(per_cell_context&                  ctx,
                                                                       const ntn_cell_config_update_info& update)
{
  ctx.cell_cfg.assistance_info.ntn_ul_sync_validity_dur = update.ntn_ul_sync_validity_duration;
  if (update.ta_info.has_value()) {
    ctx.cell_cfg.assistance_info.ta_info = update.ta_info;
  }
  if (update.feeder_link_info.has_value()) {
    ctx.cell_cfg.assistance_info.feeder_link_info = update.feeder_link_info;
  }
  if (update.reference_location.has_value()) {
    ctx.cell_cfg.assistance_info.reference_location = update.reference_location;
  }
  if (update.distance_threshold.has_value()) {
    ctx.cell_cfg.assistance_info.distance_threshold = update.distance_threshold;
  }
  if (update.t_service.has_value()) {
    ctx.cell_cfg.assistance_info.t_service = update.t_service;
  }
  if (update.polarization.has_value()) {
    ctx.cell_cfg.assistance_info.polarization = update.polarization;
  }
  if (update.ta_report.has_value()) {
    ctx.cell_cfg.assistance_info.ta_report = update.ta_report;
  }
  if (update.ncells.has_value()) {
    ctx.cell_cfg.assistance_info.ncells.clear();
    for (const auto& ncell : *update.ncells) {
      ctx.cell_cfg.assistance_info.ncells.push_back(ncell);
    }
  }
  if (update.moving_ref_location.has_value()) {
    ctx.cell_cfg.assistance_info.moving_reference_location = update.moving_ref_location;
  }
  if (update.sat_switch_with_resync.has_value()) {
    ctx.cell_cfg.assistance_info.sat_switch_with_resync = update.sat_switch_with_resync;
    ctx.sat_switch_enabled                              = true;
  } else {
    ctx.cell_cfg.assistance_info.sat_switch_with_resync.reset();
    ctx.sat_switch_enabled = false;
  }
}

bool ntn_configuration_manager_impl::send_cfo_compensation_request(const per_cell_context& ctx,
                                                                   time_point              doppler_update_time,
                                                                   const ta_info_t&        ta_info)
{
  if (not ctx.cell_cfg.assistance_info.feeder_link_info.has_value()) {
    return false;
  }

  if (not ctx.cell_cfg.assistance_info.feeder_link_info->enable_doppler_compensation) {
    return false;
  }

  if (not doppler_handler) {
    return false;
  }

  // Send CFO and CFO drift to PHY.
  double doppler_dl =
      compute_doppler_hz(ta_info.ta_common_drift, ctx.cell_cfg.assistance_info.feeder_link_info->dl_freq);
  double doppler_ul =
      compute_doppler_hz(ta_info.ta_common_drift, ctx.cell_cfg.assistance_info.feeder_link_info->ul_freq);
  double doppler_dl_rate = compute_doppler_shift_rate_hz_per_s(ta_info.ta_common_drift_variant,
                                                               ctx.cell_cfg.assistance_info.feeder_link_info->dl_freq);
  double doppler_ul_rate = compute_doppler_shift_rate_hz_per_s(ta_info.ta_common_drift_variant,
                                                               ctx.cell_cfg.assistance_info.feeder_link_info->ul_freq);

  // Check if sector_id is configured, warn if missing.
  if (!ctx.cell_cfg.sector_id.has_value()) {
    logger.warning("Cell {} has no sector_id configured, using default value 0 for Doppler compensation",
                   ctx.cell_cfg.nr_cgi.nci);
  }
  unsigned sector_id = ctx.cell_cfg.sector_id.value_or(0);

  doppler_compensation_request dl_cfo_reqs;
  dl_cfo_reqs.sector_id       = sector_id;
  dl_cfo_reqs.cfo_hz          = doppler_dl;
  dl_cfo_reqs.cfo_drift_hz_s  = doppler_dl_rate;
  dl_cfo_reqs.start_timestamp = doppler_update_time;

  doppler_compensation_request ul_cfo_reqs;
  ul_cfo_reqs.sector_id       = sector_id;
  ul_cfo_reqs.cfo_hz          = doppler_ul;
  ul_cfo_reqs.cfo_drift_hz_s  = doppler_ul_rate;
  ul_cfo_reqs.start_timestamp = doppler_update_time;

  logger.debug("Apply feeder link Doppler compensation for cell {} at time={:%T}, dl_doppler={:.1f}Hz, "
               "dl_doppler_drift={:.1f}Hz/s, ul_doppler={:.1f}Hz, ul_doppler_drift={:.1f}Hz/s",
               ctx.cell_cfg.nr_cgi.nci,
               doppler_update_time,
               doppler_dl,
               doppler_dl_rate,
               doppler_ul,
               doppler_ul_rate);

  // Send DL and UL requests separately through the interface.
  doppler_handler->handle_dl_doppler_compensation(dl_cfo_reqs);
  doppler_handler->handle_ul_doppler_compensation(ul_cfo_reqs);

  return true;
}

void ntn_configuration_manager_impl::periodic_ntn_config_update_task(const nr_cell_global_id_t& nr_cgi,
                                                                     time_point                 tp,
                                                                     slot_point                 sl)
{
  auto it = cells.find(nr_cgi);
  if (it == cells.end()) {
    logger.error("Timer fired for unknown cell: {}", nr_cgi.nci);
    return;
  }

  auto& ctx = it->second;

  slot_point next_si_win_start = get_next_si_win_start(ctx.cell_cfg, sl);
  slot_point next_si_win_end   = next_si_win_start + ctx.cell_cfg.si_window_len_slots;
  // If absent for the NTN serving cell, the epoch time is the end of SI window where this SIB19 is scheduled.
  slot_point epoch_slot = next_si_win_end + 1;
  // or if an offset provided then with the offset
  epoch_slot += ctx.cell_cfg.assistance_info.epoch_sfn_offset.value_or(0) * next_si_win_start.nof_slots_per_frame();
  auto       slot_diff  = epoch_slot - sl;
  time_point epoch_time = tp + std::chrono::milliseconds(slot_diff);

  // Drain all deferred cell config updates whose epoch_time has been reached.
  while (!ctx.deferred_cell_cfg_queue.empty() && (*ctx.deferred_cell_cfg_queue.begin()).epoch_time <= epoch_time) {
    apply_deferred_cell_config_update(ctx, *ctx.deferred_cell_cfg_queue.begin());
    ctx.deferred_cell_cfg_queue.pop();
  }

  sib19_ntn_configs_request request;
  request.use_state_vector    = *ctx.cell_cfg.assistance_info.use_state_vector;
  request.feeder_link_present = ctx.cell_cfg.assistance_info.feeder_link_info.has_value() &&
                                ctx.cell_cfg.assistance_info.ntn_gateway_location.has_value();
  request.epoch_time               = epoch_time;
  request.epoch_slot               = epoch_slot;
  request.ntn_ul_sync_validity_dur = ctx.cell_cfg.assistance_info.ntn_ul_sync_validity_dur.value_or(5);

  sib19_ntn_configs_reply reply = ctx.ntn_info_generator.generate_ntn_config(request);

  if (not reply.success) {
    logger.warning(
        "Failed to generate SIB19 NTN config for cell {} at slot={}, epoch={:%T}", nr_cgi.nci, sl, epoch_time);
    return;
  }

  // Send SIB19 PDU to DU.
  if (sib19_pdu_update_handler) {
    if (OCUDU_UNLIKELY(logger.debug.enabled())) {
      assistance_info_wrapper assistance_info{
          next_si_win_start, next_si_win_end, epoch_slot, epoch_time, reply.ta_info, reply.ephemeris_info};
      logger.debug("SIB19 msg update for cell {}: {}", nr_cgi.nci, assistance_info);
    }

    ntn_sib19_update_request ntn_req;
    ntn_req.nr_cgi         = ctx.cell_cfg.nr_cgi;
    ntn_req.si_msg_idx     = ctx.cell_cfg.si_msg_idx;
    ntn_req.sib_idx        = 19;
    ntn_req.slot           = next_si_win_start;
    ntn_req.si_slot_period = ctx.cell_cfg.si_period_rf * next_si_win_start.nof_slots_per_frame();
    ntn_req.epoch_time     = epoch_time;

    sib19_info& sib19         = ntn_req.sib19;
    sib19.ref_location        = ctx.cell_cfg.assistance_info.reference_location;
    sib19.distance_thres      = ctx.cell_cfg.assistance_info.distance_threshold;
    sib19.t_service           = ctx.cell_cfg.assistance_info.t_service;
    sib19.ncells              = ctx.cell_cfg.assistance_info.ncells;
    sib19.moving_ref_location = ctx.cell_cfg.assistance_info.moving_reference_location;
    if (ctx.sat_switch_enabled) {
      sib19.sat_switch_with_resync = ctx.cell_cfg.assistance_info.sat_switch_with_resync;
    } else {
      sib19.sat_switch_with_resync.reset();
    }
    sib19.ntn_cfg.emplace();
    sib19.ntn_cfg->cell_specific_koffset = ctx.cell_cfg.assistance_info.cell_specific_koffset;
    sib19.ntn_cfg->polarization          = ctx.cell_cfg.assistance_info.polarization;
    sib19.ntn_cfg->ta_report             = ctx.cell_cfg.assistance_info.ta_report;
    sib19.ntn_cfg->k_mac                 = ctx.cell_cfg.assistance_info.k_mac;
    sib19.ntn_cfg->epoch_time.emplace();
    sib19.ntn_cfg->epoch_time->sfn             = reply.epoch_slot.sfn();
    sib19.ntn_cfg->epoch_time->subframe_number = reply.epoch_slot.subframe_index();
    sib19.ntn_cfg->ephemeris_info              = reply.ephemeris_info;
    sib19.ntn_cfg->ta_info                     = reply.ta_info;
    sib19.ntn_cfg->ntn_ul_sync_validity_dur    = reply.ntn_ul_sync_validity_dur;

    if (ctx.cell_cfg.assistance_info.ta_info.has_value() and ctx.cell_cfg.assistance_info.ta_info->ta_common_offset) {
      if (!sib19.ntn_cfg->ta_info.has_value()) {
        sib19.ntn_cfg->ta_info.emplace();
      }
      sib19.ntn_cfg->ta_info->ta_common_offset = ctx.cell_cfg.assistance_info.ta_info->ta_common_offset;
    }

    if (ctx.sat_switch_enabled && sib19.sat_switch_with_resync.has_value() &&
        sib19.sat_switch_with_resync->epoch_timestamp.has_value() &&
        sib19.sat_switch_with_resync->ntn_cfg.ephemeris_info.has_value()) {
      auto&                     sat_sw = *sib19.sat_switch_with_resync;
      sib19_ntn_configs_request sat_req;
      sat_req.use_state_vector    = std::holds_alternative<ecef_coordinates_t>(*sat_sw.ntn_cfg.ephemeris_info);
      sat_req.feeder_link_present = sat_sw.ntn_gateway_location.has_value();
      sat_req.epoch_time          = epoch_time;
      sat_req.epoch_slot          = epoch_slot;
      sat_req.ntn_ul_sync_validity_dur =
          sat_sw.ntn_cfg.ntn_ul_sync_validity_dur.value_or(request.ntn_ul_sync_validity_dur);

      sib19_ntn_configs_reply sat_reply = ctx.sat_switch_info_generator.generate_ntn_config(sat_req);
      if (sat_reply.success) {
        sat_sw.ntn_cfg.epoch_time.emplace();
        sat_sw.ntn_cfg.epoch_time->sfn             = sat_reply.epoch_slot.sfn();
        sat_sw.ntn_cfg.epoch_time->subframe_number = sat_reply.epoch_slot.subframe_index();
        sat_sw.ntn_cfg.ephemeris_info              = sat_reply.ephemeris_info;
        sat_sw.ntn_cfg.ta_info                     = sat_reply.ta_info;
        sat_sw.ntn_cfg.ntn_ul_sync_validity_dur    = sat_reply.ntn_ul_sync_validity_dur;
      } else {
        logger.warning("Failed to generate sat-switch propagated config for cell {}. Keeping static sat-switch config.",
                       nr_cgi.nci);
      }
    }

    // If neighbors are present in SIB19, populate the first neighbor NTN config
    // with the serving-cell NTN config parameters. Specifically, neighboring cell use the same satellite.
    if (!sib19.ncells.empty()) {
      if (!sib19.ncells[0].ntn_cfg.has_value()) {
        sib19.ncells[0].ntn_cfg.emplace();
      }
      sib19.ncells[0].ntn_cfg->ephemeris_info = sib19.ntn_cfg->ephemeris_info;
      sib19.ncells[0].ntn_cfg->ta_info        = sib19.ntn_cfg->ta_info;
    }

    ntn_req.si_valuetag_change = sib19_tracked_fields_changed(ctx.last_sib19, ntn_req.sib19);
    if (ntn_req.si_valuetag_change) {
      logger.debug("SIB19 tracked fields changed for cell {} - triggering SIB1 value tag increment", nr_cgi.nci);
    }
    ctx.last_sib19 = ntn_req.sib19;

    sib19_pdu_update_handler->handle_sib19_msg_update(ntn_req);
  }

  // Send CFO compensation request to PHY.
  if (reply.ta_info.has_value()) {
    send_cfo_compensation_request(ctx, epoch_time, *reply.ta_info);
  }
}
