// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ntn_configuration_manager_impl.h"
#include "ntn_log_helpers.h"
#include "ntn_sib19_helpers.h"
#include "ocudu/ntn/ntn_doppler_compensation_handler.h"
#include "ocudu/ntn/ntn_sib19_update_handler.h"
#include "ocudu/ran/sib/system_info_config.h"
#include "fmt/chrono.h"

using namespace ocudu;
using namespace ocudu_ntn;

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

/// \brief Merges a sparse cell config update into a full cell config snapshot.
///
/// ntn_cell_config_update_info uses optional fields as a patch: only the fields present in the update are applied;
/// absent fields leave the existing values in cfg unchanged. This allows O&M to send partial updates, e.g. only a
/// new feeder_link_info without repeating unchanged parameters.
static void merge_cell_config_update(ntn_cell_config& cfg, const ntn_cell_config_update_info& update)
{
  cfg.ntn_cfg.ntn_ul_sync_validity_dur = update.ntn_ul_sync_validity_duration;
  if (update.ta_info && update.ta_info->ta_common_offset) {
    cfg.ntn_cfg.ta_common_offset = update.ta_info->ta_common_offset;
  }
  if (update.reference_location) {
    cfg.ntn_cfg.reference_location = update.reference_location;
  }
  if (update.distance_threshold) {
    cfg.ntn_cfg.distance_threshold = update.distance_threshold;
  }
  if (update.t_service) {
    cfg.ntn_cfg.t_service = update.t_service;
  }
  if (update.polarization) {
    cfg.ntn_cfg.polarization = update.polarization;
  }
  if (update.ta_report) {
    cfg.ntn_cfg.ta_report = update.ta_report;
  }
  if (update.ncells) {
    cfg.ncells.clear();
    for (const auto& ncell : *update.ncells) {
      ntn_neighbor_cell_config nc_cfg{cfg.satellite_index, ncell.carrier_freq, ncell.phys_cell_id};
      if (ncell.ntn_cfg) {
        nc_cfg.cell_specific_koffset    = ncell.ntn_cfg->cell_specific_koffset;
        nc_cfg.ntn_ul_sync_validity_dur = ncell.ntn_cfg->ntn_ul_sync_validity_dur;
        nc_cfg.k_mac                    = ncell.ntn_cfg->k_mac;
        nc_cfg.polarization             = ncell.ntn_cfg->polarization;
        nc_cfg.ta_report                = ncell.ntn_cfg->ta_report;
      }
      cfg.ncells.push_back(nc_cfg);
    }
  }
  if (update.moving_ref_location) {
    cfg.ntn_cfg.moving_reference_location = update.moving_ref_location;
  }
  if (update.sat_switch_with_resync) {
    const auto& sat_sw  = *update.sat_switch_with_resync;
    unsigned    sat_idx = cfg.sat_switch ? cfg.sat_switch->satellite_index : 0;
    cfg.sat_switch      = ntn_sat_switch_config{sat_idx,
                                           sat_sw.t_service_start,
                                           sat_sw.ssb_time_offset_sf,
                                           sat_sw.ntn_cfg.ntn_ul_sync_validity_dur,
                                           sat_sw.ntn_cfg.cell_specific_koffset,
                                           sat_sw.ntn_cfg.k_mac,
                                           sat_sw.ntn_cfg.polarization,
                                           sat_sw.ntn_cfg.ta_report};
  } else {
    cfg.sat_switch.reset();
  }
  if (update.feeder_link_info) {
    cfg.ntn_cfg.feeder_link_info = update.feeder_link_info;
  }
}

/// Returns the start slot of the next SI window for the given cell config, strictly after cur_sl.
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

  // Create per-satellite contexts and seed their generators.
  for (const auto& sat_config : config.satellites) {
    auto [sat_it, inserted] = satellite_contexts.try_emplace(sat_config.satellite_index, sat_config.propagator_type);
    if (!inserted) {
      logger.error("Duplicate satellite_index {} in NTN configuration, skipping", sat_config.satellite_index);
      continue;
    }
    auto& sat_ctx = sat_it->second;
    sat_ctx.ocm.set_ta_info_override(sat_config.ta_info);

    if (sat_config.epoch_timestamp) {
      sat_ctx.ocm.enqueue_ephemeris_info(ephemeris_info_update{*sat_config.epoch_timestamp, sat_config.ephemeris_info});

      if (sat_config.ntn_gateway_location) {
        ntn_gateway_location_info gw_location{
            *sat_config.epoch_timestamp, std::nullopt, *sat_config.ntn_gateway_location, std::nullopt};
        sat_ctx.ocm.enqueue_ntn_gw_location(gw_location);
      }
    }
  }

  for (const auto& cell_config : config.cells) {
    auto [it, inserted] = cells.try_emplace(cell_config.nr_cgi);
    if (!inserted) {
      logger.error("Duplicate nr_cgi {} in NTN configuration, skipping", cell_config.nr_cgi.nci);
      continue;
    }
    auto& ctx = it->second;
    ctx.cell_cfg_queue.push(cell_config_snapshot{time_point{}, cell_config});

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
      if (cur_tp_sl and cur_tp_sl->slot_tx.valid()) {
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

const ntn_cell_config& ntn_configuration_manager_impl::get_cell_config(per_cell_context& ctx, time_point t) const
{
  auto& q = ctx.cell_cfg_queue;
  while (q.size() > 1 && t >= q[1].epoch_time) {
    q.pop();
  }
  return q[0].config;
}

ntn_configuration_manager_impl::per_satellite_context*
ntn_configuration_manager_impl::find_satellite_context(unsigned satellite_index)
{
  auto it = satellite_contexts.find(satellite_index);
  return it != satellite_contexts.end() ? &it->second : nullptr;
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

  // Merge the update onto the most recent snapshot and enqueue for epoch-gated selection.
  ntn_cell_config merged_cfg = ctx.cell_cfg_queue[ctx.cell_cfg_queue.size() - 1].config;
  merge_cell_config_update(merged_cfg, cell_req);
  if (!ctx.cell_cfg_queue.try_push(cell_config_snapshot{cell_req.epoch_time, std::move(merged_cfg)})) {
    logger.warning("Cell config queue full for cell {}, dropping update", cell_req.nr_cgi.nci);
    return false;
  }

  const ntn_cell_config& base_cfg = ctx.cell_cfg_queue[ctx.cell_cfg_queue.size() - 1].config;

  per_satellite_context* sat_ctx = find_satellite_context(base_cfg.satellite_index);
  if (sat_ctx == nullptr) {
    logger.warning("Satellite index {} not found for cell {}", base_cfg.satellite_index, cell_req.nr_cgi.nci);
    return false;
  }

  if (!sat_ctx->ocm.enqueue_ephemeris_info(ephemeris_info_update{cell_req.epoch_time, cell_req.ephemeris_info})) {
    return false;
  }

  if (cell_req.ntn_gateway_location) {
    ntn_gateway_location_info gw_location{
        cell_req.epoch_time, std::nullopt, *cell_req.ntn_gateway_location, std::nullopt};
    if (not sat_ctx->ocm.enqueue_ntn_gw_location(gw_location)) {
      return false;
    }
  }

  // Enqueue sat-switch ephemeris and gateway immediately (time-gated by sat_switch.epoch_timestamp).
  if (cell_req.sat_switch_with_resync && base_cfg.sat_switch) {
    const sat_switch_with_resync_t& sat_sw     = *cell_req.sat_switch_with_resync;
    per_satellite_context*          sat_sw_ctx = find_satellite_context(base_cfg.sat_switch->satellite_index);
    if (sat_sw_ctx == nullptr) {
      logger.warning("Sat-switch satellite index {} not found for cell {}",
                     base_cfg.sat_switch->satellite_index,
                     cell_req.nr_cgi.nci);
      return false;
    }
    if (sat_sw.epoch_timestamp && sat_sw.ntn_cfg.ephemeris_info) {
      if (!sat_sw_ctx->ocm.enqueue_ephemeris_info(
              ephemeris_info_update{*sat_sw.epoch_timestamp, *sat_sw.ntn_cfg.ephemeris_info})) {
        logger.warning("Failed to enqueue sat-switch ephemeris for cell {}", cell_req.nr_cgi.nci);
        return false;
      }
      if (sat_sw.ntn_gateway_location) {
        ntn_gateway_location_info gw_location{
            *sat_sw.epoch_timestamp, std::nullopt, *sat_sw.ntn_gateway_location, std::nullopt};
        if (!sat_sw_ctx->ocm.enqueue_ntn_gw_location(gw_location)) {
          logger.warning("Failed to enqueue sat-switch gateway location for cell {}", cell_req.nr_cgi.nci);
          return false;
        }
      }
    }
  }

  return true;
}

bool ntn_configuration_manager_impl::send_cfo_compensation_request(const ntn_cell_config& cell_cfg,
                                                                   time_point             doppler_update_time,
                                                                   const ta_info_t&       ta_info)
{
  if (not cell_cfg.ntn_cfg.feeder_link_info) {
    return false;
  }

  if (not cell_cfg.ntn_cfg.feeder_link_info->enable_doppler_compensation) {
    return false;
  }

  if (not doppler_handler) {
    return false;
  }

  const feeder_link_info_t& fl = *cell_cfg.ntn_cfg.feeder_link_info;

  // Send CFO and CFO drift to PHY.
  double doppler_dl      = compute_doppler_hz(ta_info.ta_common_drift, fl.dl_freq);
  double doppler_ul      = compute_doppler_hz(ta_info.ta_common_drift, fl.ul_freq);
  double doppler_dl_rate = compute_doppler_shift_rate_hz_per_s(ta_info.ta_common_drift_variant, fl.dl_freq);
  double doppler_ul_rate = compute_doppler_shift_rate_hz_per_s(ta_info.ta_common_drift_variant, fl.ul_freq);

  // Check if sector_id is configured, warn if missing.
  if (!cell_cfg.sector_id) {
    logger.warning("Cell {} has no sector_id configured, using default value 0 for Doppler compensation",
                   cell_cfg.nr_cgi.nci);
  }
  unsigned sector_id = cell_cfg.sector_id.value_or(0);

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
               cell_cfg.nr_cgi.nci,
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

ntn_orbital_state ntn_configuration_manager_impl::compute_orbital_state(per_satellite_context& sat,
                                                                        time_point             epoch_time,
                                                                        slot_point             epoch_slot,
                                                                        unsigned               ntn_ul_sync_validity_dur,
                                                                        bool                   use_state_vector) const
{
  if (sat.last_result && sat.last_result->epoch_slot == epoch_slot &&
      std::chrono::abs(sat.last_result->epoch_time - epoch_time) < std::chrono::milliseconds(5) &&
      sat.last_result->ntn_ul_sync_validity_dur == ntn_ul_sync_validity_dur &&
      sat.last_result->use_state_vector == use_state_vector) {
    return sat.last_result->state;
  }
  ntn_orbital_state result = sat.ocm.compute_orbital_state(epoch_time, ntn_ul_sync_validity_dur, use_state_vector);
  if (result.success) {
    sat.last_result = {epoch_time, epoch_slot, ntn_ul_sync_validity_dur, use_state_vector, result};
  }
  return result;
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

  const ntn_cell_config& cell_cfg          = get_cell_config(ctx, tp);
  slot_point             next_si_win_start = get_next_si_win_start(cell_cfg, sl);
  slot_point             next_si_win_end   = next_si_win_start + cell_cfg.si_window_len_slots;
  // If absent for the NTN serving cell, the epoch time is the end of SI window where this SIB19 is scheduled.
  slot_point epoch_slot = next_si_win_end + 1;
  // or if an offset provided then with the offset
  epoch_slot += cell_cfg.ntn_cfg.epoch_sfn_offset.value_or(0) * next_si_win_start.nof_slots_per_frame();
  auto       slot_diff  = epoch_slot - sl;
  time_point epoch_time = tp + std::chrono::milliseconds(slot_diff);

  per_satellite_context* sat_ctx = find_satellite_context(cell_cfg.satellite_index);
  if (sat_ctx == nullptr) {
    logger.error("Satellite index {} not found for cell {}", cell_cfg.satellite_index, nr_cgi.nci);
    return;
  }

  unsigned ntn_ul_sync_validity_dur = cell_cfg.ntn_cfg.ntn_ul_sync_validity_dur.value_or(5);
  bool     use_state_vector         = cell_cfg.ntn_cfg.use_state_vector.value_or(true);

  ntn_orbital_state serving_ntn_info =
      compute_orbital_state(*sat_ctx, epoch_time, epoch_slot, ntn_ul_sync_validity_dur, use_state_vector);

  if (not serving_ntn_info.success) {
    logger.warning(
        "Failed to generate propagated NTN config for cell {} at slot={}, epoch={:%T}", nr_cgi.nci, sl, epoch_time);
    return;
  }

  if (cell_cfg.ntn_cfg.feeder_link_info && !serving_ntn_info.ta_info) {
    logger.error("Feeder link is configured for cell {} but TA-info was not computed at slot={}, epoch={:%T}",
                 nr_cgi.nci,
                 sl,
                 epoch_time);
    return;
  }

  // Optionally propagate sat-switch target satellite using its own OCM.
  std::optional<ntn_orbital_state> sat_sw_ntn_info;
  if (cell_cfg.sat_switch) {
    per_satellite_context* sat_sw_ctx = find_satellite_context(cell_cfg.sat_switch->satellite_index);
    if (sat_sw_ctx == nullptr) {
      logger.warning(
          "Sat-switch satellite index {} not found for cell {}", cell_cfg.sat_switch->satellite_index, nr_cgi.nci);
    } else {
      sat_sw_ntn_info =
          compute_orbital_state(*sat_sw_ctx, epoch_time, epoch_slot, ntn_ul_sync_validity_dur, use_state_vector);
      if (!sat_sw_ntn_info->success) {
        sat_sw_ntn_info.reset();
        logger.warning("Failed to generate sat-switch propagated config for cell {}.", nr_cgi.nci);
      }
    }
  }

  // Propagate each neighbor satellite using its own OCM.
  static_vector<ntn_orbital_state, MAX_NOF_NTN_NEIGHBORS> ncell_ntn_info(cell_cfg.ncells.size(), ntn_orbital_state{});
  for (size_t i = 0; i != cell_cfg.ncells.size(); ++i) {
    const auto&            nc         = cell_cfg.ncells[i];
    per_satellite_context* nc_sat_ctx = find_satellite_context(nc.satellite_index);
    if (nc_sat_ctx == nullptr) {
      logger.warning("Satellite index {} not found for neighbor of cell {}", nc.satellite_index, nr_cgi.nci);
      continue;
    }
    ncell_ntn_info[i] =
        compute_orbital_state(*nc_sat_ctx, epoch_time, epoch_slot, ntn_ul_sync_validity_dur, use_state_vector);
    if (!ncell_ntn_info[i].success) {
      logger.warning("Failed to generate neighbor OCM for cell {}, sat_idx={}", nr_cgi.nci, nc.satellite_index);
    }
  }

  // Send SIB19 PDU to DU.
  if (sib19_pdu_update_handler) {
    if (OCUDU_UNLIKELY(logger.debug.enabled())) {
      assistance_info_wrapper assistance_info{next_si_win_start,
                                              next_si_win_end,
                                              epoch_slot,
                                              epoch_time,
                                              serving_ntn_info.ta_info,
                                              serving_ntn_info.ephemeris_info};
      logger.debug("SIB19 msg update for cell {}: {}", nr_cgi.nci, assistance_info);
    }

    ntn_sib19_update_request ntn_req;
    ntn_req.nr_cgi         = cell_cfg.nr_cgi;
    ntn_req.si_msg_idx     = cell_cfg.si_msg_idx;
    ntn_req.sib_idx        = 19;
    ntn_req.slot           = next_si_win_start;
    ntn_req.si_slot_period = cell_cfg.si_period_rf * next_si_win_start.nof_slots_per_frame();
    ntn_req.epoch_time     = epoch_time;

    ntn_req.sib19 = generate_sib19_info(cell_cfg,
                                        epoch_slot,
                                        serving_ntn_info,
                                        sat_sw_ntn_info.has_value() ? &*sat_sw_ntn_info : nullptr,
                                        ncell_ntn_info);

    ntn_req.si_valuetag_change = sib19_tracked_fields_changed(ctx.last_sib19, ntn_req.sib19);
    if (ntn_req.si_valuetag_change) {
      logger.debug("SIB19 tracked fields changed for cell {} - triggering SIB1 value tag increment", nr_cgi.nci);
    }
    ctx.last_sib19 = ntn_req.sib19;

    sib19_pdu_update_handler->handle_sib19_msg_update(ntn_req);
  }

  // Send CFO compensation request to PHY.
  if (serving_ntn_info.ta_info && cell_cfg.ntn_cfg.feeder_link_info) {
    send_cfo_compensation_request(cell_cfg, epoch_time, *serving_ntn_info.ta_info);
  }
}
