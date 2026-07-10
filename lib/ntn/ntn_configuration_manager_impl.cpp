// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ntn_configuration_manager_impl.h"
#include "ntn_log_helpers.h"
#include "ntn_sat_switch_helpers.h"
#include "ntn_sib19_helpers.h"
#include "ocudu/ntn/ntn_doppler_compensation_handler.h"
#include "ocudu/ntn/ntn_sib19_update_handler.h"
#include "ocudu/ran/sib/system_info_config.h"
#include "ocudu/support/ocudu_assert.h"
#include "fmt/chrono.h"

using namespace ocudu;
using namespace ocudu_ntn;

/// \brief Compute current Doppler shift in Hz based on TA drift.
/// \param ta_common_drift_us_per_s Timing advance drift [us/s]
/// \param carrier_freq_hz Carrier frequency [Hz]
/// \return Doppler shift in Hz
static double compute_doppler_hz(double ta_common_drift_us_per_s, double carrier_freq_hz)
{
  return (ta_common_drift_us_per_s / 2.0) * 1e-6 * carrier_freq_hz;
}

/// \brief Doppler shift rate in Hz/s based on TA drift derivative.
/// \param ta_common_drift_variant_us_per_s2 Timing advance drift rate [us/s^2]
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
  if (cfg.ntn_cfg) {
    cfg.ntn_cfg->ntn_ul_sync_validity_dur = update.ntn_ul_sync_validity_duration;
    if (update.ta_info && update.ta_info->ta_common_offset) {
      cfg.ntn_cfg->ta_common_offset = update.ta_info->ta_common_offset;
    }
    if (update.reference_location) {
      cfg.ntn_cfg->reference_location = update.reference_location;
    }
    if (update.distance_threshold) {
      cfg.ntn_cfg->distance_threshold = update.distance_threshold;
    }
    if (update.t_service) {
      cfg.ntn_cfg->t_service = update.t_service;
    }
    if (update.polarization) {
      cfg.ntn_cfg->polarization = update.polarization;
    }
    if (update.ta_report) {
      cfg.ntn_cfg->ta_report = update.ta_report;
    }
    if (update.moving_ref_location) {
      cfg.ntn_cfg->moving_reference_location = update.moving_ref_location;
    }
    if (update.feeder_link_info) {
      cfg.ntn_cfg->feeder_link_info = update.feeder_link_info;
    }
  }
  if (update.ncells && cfg.ntn_cfg) {
    cfg.ncells.clear();
    for (const auto& ncell : *update.ncells) {
      auto& nc_cfg           = cfg.ncells.emplace_back();
      nc_cfg.satellite_index = cfg.ntn_cfg->satellite_index;
      nc_cfg.carrier_freq    = ncell.carrier_freq;
      nc_cfg.phys_cell_id    = ncell.phys_cell_id;
      if (ncell.ntn_cfg) {
        nc_cfg.cell_specific_koffset    = ncell.ntn_cfg->cell_specific_koffset;
        nc_cfg.ntn_ul_sync_validity_dur = ncell.ntn_cfg->ntn_ul_sync_validity_dur;
        nc_cfg.k_mac                    = ncell.ntn_cfg->k_mac;
        nc_cfg.polarization             = ncell.ntn_cfg->polarization;
        nc_cfg.ta_report                = ncell.ntn_cfg->ta_report;
      }
    }
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
}

/// Returns the start slot of the next SI window for the given SI scheduling info, strictly after cur_sl.
static slot_point get_next_si_win_start(const ntn_si_scheduling_info& si_sched, slot_point cur_sl)
{
  // 2> The concerned SI message is configured in the schedulingInfoList2.
  // 3> Determine the integer value x = (si-WindowPosition -1) * w, where w is
  // the si-WindowLength. See TS 38 331 V17.0.0.
  unsigned x = (si_sched.si_window_position - 1) * si_sched.si_window_len_slots;

  // 3> The SI-window starts at the slot #a, where a = x mod N, in the radio
  // frame for which SFN mod T = FLOOR(x/N), where T is the si-Periodicity of
  // the concerned SI message and N is the number of slots in a radio frame as
  // specified in TS 38.213.
  const unsigned N = cur_sl.nof_slots_per_frame();
  const unsigned T = si_sched.si_period_rf;
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
    // Enforced at config creation (see generate_ntn_configuration_manager_config).
    ocudu_assert(cell_config.si_sched.has_value() != cell_config.update_period.has_value(),
                 "Cell={:#x} must configure exactly one of SI scheduling info or update period",
                 cell_config.nr_cgi.nci);
    auto [it, inserted] = cells.try_emplace(cell_config.nr_cgi);
    if (!inserted) {
      logger.error("Duplicate cell={:#x} in NTN configuration, skipping", cell_config.nr_cgi.nci);
      continue;
    }
    auto& ctx = it->second;
    ctx.cell_cfg_queue.push(cell_config_snapshot{time_point{}, cell_config});

    if (auto derived_cfg = derive_post_switch_config(cell_config)) {
      if (!ctx.cell_cfg_queue.try_push(
              cell_config_snapshot{*cell_config.ntn_cfg->t_service, std::move(*derived_cfg)})) {
        logger.warning("Cell config queue full, cell={:#x}, dropping initial sat-switch snapshot",
                       cell_config.nr_cgi.nci);
      } else {
        logger.info("Sat-switch promotion scheduled, cell={:#x} serving satellite {} -> {} at t_service={:%T}",
                    cell_config.nr_cgi.nci,
                    cell_config.ntn_cfg->satellite_index,
                    cell_config.sat_switch->satellite_index,
                    *cell_config.ntn_cfg->t_service);
      }
    }

    // Create per-cell timer for the periodic update task, aligned to the SI period when SIB19 is scheduled.
    auto period_ms = cell_config.si_sched ? std::chrono::milliseconds(cell_config.si_sched->si_period_rf * 10)
                                          : *cell_config.update_period;
    ctx.timer      = timers.create_unique_timer(executor);
    ctx.timer.set(period_ms, [this, nr_cgi = cell_config.nr_cgi]() {
      // Check if cell context still exists before processing.
      auto ctx_it = cells.find(nr_cgi);
      if (ctx_it == cells.end()) {
        // Cell was removed, do not re-run timer.
        return;
      }

      auto cur_tp_sl = time_provider->get_last_mapping(nr_cgi, subcarrier_spacing::kHz15);
      if (cur_tp_sl and cur_tp_sl->slot_tx.valid()) {
        logger.debug("Run periodic config update task cell={:#x} slot={} time={:%T}",
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
    if (q[0].config.ntn_cfg && q[1].config.ntn_cfg &&
        q[0].config.ntn_cfg->satellite_index != q[1].config.ntn_cfg->satellite_index) {
      logger.info("Sat-switch promotion applied, cell={:#x} serving satellite {} -> {} time={:%T}",
                  q[1].config.nr_cgi.nci,
                  q[0].config.ntn_cfg->satellite_index,
                  q[1].config.ntn_cfg->satellite_index,
                  t);
    }
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
    logger.warning("Received NTN config update for unknown cell={:#x}", cell_req.nr_cgi.nci);
    return false;
  }

  logger.debug("Received config update cell={:#x} epoch_time={:%T} format={}",
               cell_req.nr_cgi.nci,
               cell_req.epoch_time,
               std::holds_alternative<ecef_coordinates_t>(cell_req.ephemeris_info) ? "ecef" : "orbital");

  auto& ctx = it->second;

  // Merge the update onto the most recent snapshot and enqueue for epoch-gated selection.
  ntn_cell_config merged_cfg = ctx.cell_cfg_queue[ctx.cell_cfg_queue.size() - 1].config;
  merge_cell_config_update(merged_cfg, cell_req);
  if (!ctx.cell_cfg_queue.try_push(cell_config_snapshot{cell_req.epoch_time, std::move(merged_cfg)})) {
    logger.warning("Cell config queue full, cell={:#x}, dropping update", cell_req.nr_cgi.nci);
    return false;
  }

  const ntn_cell_config& base_cfg = ctx.cell_cfg_queue[ctx.cell_cfg_queue.size() - 1].config;

  if (auto derived_cfg = derive_post_switch_config(base_cfg)) {
    if (!ctx.cell_cfg_queue.try_push(cell_config_snapshot{*base_cfg.ntn_cfg->t_service, std::move(*derived_cfg)})) {
      logger.warning("Cell config queue full, cell={:#x}, dropping derived post-switch snapshot", cell_req.nr_cgi.nci);
    } else {
      logger.info("Sat-switch promotion scheduled, cell={:#x} serving satellite {} -> {} at t_service={:%T}",
                  cell_req.nr_cgi.nci,
                  base_cfg.ntn_cfg->satellite_index,
                  base_cfg.sat_switch->satellite_index,
                  *base_cfg.ntn_cfg->t_service);
    }
  }

  if (!base_cfg.ntn_cfg) {
    logger.debug("Skipping serving satellite ephemeris update, TN cell={:#x}", cell_req.nr_cgi.nci);
    return true;
  }

  per_satellite_context* sat_ctx = find_satellite_context(base_cfg.ntn_cfg->satellite_index);
  if (sat_ctx == nullptr) {
    logger.warning("Satellite index {} not found, cell={:#x}", base_cfg.ntn_cfg->satellite_index, cell_req.nr_cgi.nci);
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
      logger.warning("Sat-switch satellite index {} not found, cell={:#x}",
                     base_cfg.sat_switch->satellite_index,
                     cell_req.nr_cgi.nci);
      return false;
    }
    if (sat_sw.epoch_timestamp && sat_sw.ntn_cfg.ephemeris_info) {
      if (!sat_sw_ctx->ocm.enqueue_ephemeris_info(
              ephemeris_info_update{*sat_sw.epoch_timestamp, *sat_sw.ntn_cfg.ephemeris_info})) {
        logger.warning("Failed to enqueue sat-switch ephemeris, cell={:#x}", cell_req.nr_cgi.nci);
        return false;
      }
      if (sat_sw.ntn_gateway_location) {
        ntn_gateway_location_info gw_location{
            *sat_sw.epoch_timestamp, std::nullopt, *sat_sw.ntn_gateway_location, std::nullopt};
        if (!sat_sw_ctx->ocm.enqueue_ntn_gw_location(gw_location)) {
          logger.warning("Failed to enqueue sat-switch gateway location, cell={:#x}", cell_req.nr_cgi.nci);
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
  if (not cell_cfg.ntn_cfg || not cell_cfg.ntn_cfg->feeder_link_info) {
    return false;
  }

  if (not cell_cfg.ntn_cfg->feeder_link_info->enable_doppler_compensation) {
    return false;
  }

  if (not doppler_handler) {
    return false;
  }

  const feeder_link_info_t& fl = *cell_cfg.ntn_cfg->feeder_link_info;

  // Send CFO and CFO drift to PHY.
  double doppler_dl      = compute_doppler_hz(ta_info.ta_common_drift, fl.dl_freq);
  double doppler_ul      = compute_doppler_hz(ta_info.ta_common_drift, fl.ul_freq);
  double doppler_dl_rate = compute_doppler_shift_rate_hz_per_s(ta_info.ta_common_drift_variant, fl.dl_freq);
  double doppler_ul_rate = compute_doppler_shift_rate_hz_per_s(ta_info.ta_common_drift_variant, fl.ul_freq);

  // Check if sector_id is configured, warn if missing.
  if (!cell_cfg.sector_id) {
    logger.warning("No sector_id configured, cell={:#x}, using default value 0 for Doppler compensation",
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

  logger.debug("Apply feeder link Doppler compensation cell={:#x} time={:%T} dl_doppler={:.1f}Hz "
               "dl_doppler_drift={:.1f}Hz/s ul_doppler={:.1f}Hz ul_doppler_drift={:.1f}Hz/s",
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
    logger.error("Timer fired for unknown cell={:#x}", nr_cgi.nci);
    return;
  }

  auto& ctx = it->second;

  const ntn_cell_config& cell_cfg = get_cell_config(ctx, tp);

  // Derive the epoch slot: from the SI windows when SIB19 is scheduled for this cell, otherwise the current slot.
  slot_point next_si_win_start;
  slot_point next_si_win_end;
  slot_point epoch_slot;
  if (cell_cfg.si_sched) {
    next_si_win_start = get_next_si_win_start(*cell_cfg.si_sched, sl);
    next_si_win_end   = next_si_win_start + cell_cfg.si_sched->si_window_len_slots;
    // If absent for the NTN serving cell, the epoch time is the end of SI window where this SIB19 is scheduled.
    epoch_slot = next_si_win_end + 1;
  } else {
    // No SIB19 broadcast: this feeds NTN-NeighbourCellInfo-r18, whose EpochTime is delivered asynchronously via
    // dedicated RRC (only on a UE-triggered reconfiguration). Per TS 38.331 the neighbour epoch is the SFN nearest to
    // the frame where the message is received, so anchor it to the current slot rather than an update_period ahead;
    // the update period drives only the regeneration cadence. Keeping it near "now" also stays well within the 10-bit
    // SFN wrap (+/-5.12 s) regardless of the configured period.
    epoch_slot = sl;
  }
  auto       slot_diff  = epoch_slot - sl;
  time_point epoch_time = tp + std::chrono::milliseconds(slot_diff);

  // Propagate each serving cell satellite using its own OCM.
  ntn_orbital_state serving_ntn_info;
  if (cell_cfg.ntn_cfg) {
    const ntn_serving_cell_config& ntn_cfg                  = *cell_cfg.ntn_cfg;
    const unsigned                 ntn_ul_sync_validity_dur = ntn_cfg.ntn_ul_sync_validity_dur;
    const bool                     serving_use_state_vector = ntn_cfg.use_state_vector.value_or(false);

    // Recompute epoch_time if an offset provided then with the offset.
    if (ntn_cfg.epoch_sfn_offset) {
      epoch_slot += *ntn_cfg.epoch_sfn_offset * sl.nof_slots_per_frame();
      slot_diff  = epoch_slot - sl;
      epoch_time = tp + std::chrono::milliseconds(slot_diff);
    }

    per_satellite_context* sat_ctx = find_satellite_context(ntn_cfg.satellite_index);
    if (sat_ctx == nullptr) {
      logger.error("Satellite index {} not found, cell={:#x}", ntn_cfg.satellite_index, nr_cgi.nci);
      return;
    }
    serving_ntn_info =
        compute_orbital_state(*sat_ctx, epoch_time, epoch_slot, ntn_ul_sync_validity_dur, serving_use_state_vector);

    if (not serving_ntn_info.success) {
      logger.warning(
          "Failed to generate propagated NTN config, cell={:#x} slot={} epoch={:%T}", nr_cgi.nci, sl, epoch_time);
      return;
    }

    if (ntn_cfg.feeder_link_info && !serving_ntn_info.ta_info) {
      logger.error("Feeder link is configured, cell={:#x}, but TA-info was not computed at slot={} epoch={:%T}",
                   nr_cgi.nci,
                   sl,
                   epoch_time);
      return;
    }
  }

  // Sat-switch/neighbor entries without their own validity duration fall back to the serving cell value, which the
  // UE applies when the field is absent (TS 38.331). In a TN cell there is no serving value; use the smallest one.
  const unsigned fallback_ul_sync_validity_dur = cell_cfg.ntn_cfg ? cell_cfg.ntn_cfg->ntn_ul_sync_validity_dur : 5;

  // Optionally propagate sat-switch target satellite using its own OCM.
  std::optional<ntn_orbital_state> sat_sw_ntn_info;
  if (cell_cfg.sat_switch) {
    per_satellite_context* sat_sw_ctx = find_satellite_context(cell_cfg.sat_switch->satellite_index);
    if (sat_sw_ctx == nullptr) {
      logger.warning(
          "Sat-switch satellite index {} not found, cell={:#x}", cell_cfg.sat_switch->satellite_index, nr_cgi.nci);
    } else {
      const unsigned ntn_ul_sync_validity_dur =
          cell_cfg.sat_switch->ntn_ul_sync_validity_dur.value_or(fallback_ul_sync_validity_dur);
      const bool sat_sw_use_state_vector = cell_cfg.sat_switch->use_state_vector.value_or(false);
      sat_sw_ntn_info =
          compute_orbital_state(*sat_sw_ctx, epoch_time, epoch_slot, ntn_ul_sync_validity_dur, sat_sw_use_state_vector);
      if (!sat_sw_ntn_info->success) {
        sat_sw_ntn_info.reset();
        logger.warning("Failed to generate sat-switch propagated config, cell={:#x}", nr_cgi.nci);
      }
    }
  }

  // Propagate each neighbor satellite using its own OCM.
  static_vector<ntn_orbital_state, MAX_NOF_NTN_NEIGHBORS> ncell_ntn_info(cell_cfg.ncells.size(), ntn_orbital_state{});
  for (size_t i = 0; i != cell_cfg.ncells.size(); ++i) {
    const auto&            nc         = cell_cfg.ncells[i];
    per_satellite_context* nc_sat_ctx = find_satellite_context(nc.satellite_index);
    if (nc_sat_ctx == nullptr) {
      logger.warning("Satellite index {} not found for neighbor, cell={:#x}", nc.satellite_index, nr_cgi.nci);
      continue;
    }
    const unsigned ntn_ul_sync_validity_dur = nc.ntn_ul_sync_validity_dur.value_or(fallback_ul_sync_validity_dur);
    const bool     nc_use_state_vector      = nc.use_state_vector.value_or(false);
    ncell_ntn_info[i] =
        compute_orbital_state(*nc_sat_ctx, epoch_time, epoch_slot, ntn_ul_sync_validity_dur, nc_use_state_vector);
    if (!ncell_ntn_info[i].success) {
      logger.warning("Failed to generate neighbor OCM, cell={:#x} sat_idx={}", nr_cgi.nci, nc.satellite_index);
    }
  }

  // Send SIB19 PDU to DU.
  if (sib19_pdu_update_handler and cell_cfg.si_sched) {
    if (OCUDU_UNLIKELY(logger.debug.enabled())) {
      if (cell_cfg.ntn_cfg) {
        assistance_info_wrapper serving_info{next_si_win_start,
                                             next_si_win_end,
                                             epoch_slot,
                                             epoch_time,
                                             serving_ntn_info.ta_info,
                                             serving_ntn_info.ephemeris_info};
        logger.debug("SIB19 msg update, NTN serving cell={:#x}: {}", nr_cgi.nci, serving_info);
      } else {
        logger.debug(
            "SIB19 msg update, TN cell={:#x}: si_window={}-{} epoch_slot={} epoch_time={:%T} nof_ntn_neighbors={}",
            nr_cgi.nci,
            next_si_win_start,
            next_si_win_end,
            epoch_slot,
            epoch_time,
            cell_cfg.ncells.size());
      }
    }

    ntn_sib19_update_request ntn_req;
    ntn_req.nr_cgi         = cell_cfg.nr_cgi;
    ntn_req.si_msg_idx     = cell_cfg.si_sched->si_msg_idx;
    ntn_req.sib_idx        = 19;
    ntn_req.slot           = next_si_win_start;
    ntn_req.si_slot_period = cell_cfg.si_sched->si_period_rf * next_si_win_start.nof_slots_per_frame();
    ntn_req.epoch_time     = epoch_time;

    ntn_req.sib19 = generate_sib19_info(cell_cfg,
                                        epoch_slot,
                                        serving_ntn_info,
                                        sat_sw_ntn_info.has_value() ? &*sat_sw_ntn_info : nullptr,
                                        ncell_ntn_info);

    ntn_req.si_valuetag_change = sib19_tracked_fields_changed(ctx.last_sib19, ntn_req.sib19);
    if (ntn_req.si_valuetag_change) {
      logger.debug("SIB19 tracked fields changed, cell={:#x}, triggering SIB1 value tag increment", nr_cgi.nci);
    }
    ctx.last_sib19 = ntn_req.sib19;

    sib19_pdu_update_handler->handle_sib19_msg_update(ntn_req);
  }

  // Send CFO compensation request to PHY.
  if (doppler_handler and serving_ntn_info.ta_info) {
    send_cfo_compensation_request(cell_cfg, epoch_time, *serving_ntn_info.ta_info);
  }
}
