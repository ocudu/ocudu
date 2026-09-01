// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_cell_manager.h"
#include "converters/asn1_sys_info_packer.h"
#include "converters/scheduler_configuration_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/du/du_cell_config_validation.h"
#include "ocudu/du/du_high/du_manager/du_configurator.h"
#include "ocudu/mac/mac_cell_manager.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/support/async/async_no_op_task.h"
#include "ocudu/support/async/async_timer.h"
#include "ocudu/support/enum_utils.h"

using namespace ocudu;
using namespace odu;

du_cell_manager::du_cell_manager(const du_manager_params& cfg_) :
  cfg(cfg_), logger(ocudulog::fetch_basic_logger("DU-MNG"))
{
}

/// Size of the largest segment of an SI message, 0 bytes if it carries no content.
static units::bytes largest_segment_len(const bcch_dl_sch_payload_type& si_msg)
{
  size_t len = 0;
  for (const byte_buffer& segment : si_msg) {
    len = std::max(len, segment.length());
  }
  return units::bytes{static_cast<unsigned>(len)};
}

static void fill_si_scheduler_config(si_scheduling_config&                si_sched_cfg,
                                     const du_cell_config&                cell_cfg,
                                     const byte_buffer&                   sib1,
                                     span<const bcch_dl_sch_payload_type> si_messages,
                                     span<const bcch_dl_sch_payload_type> pws_si_messages)
{
  const units::bytes                           sib1_len = units::bytes{static_cast<unsigned>(sib1.length())};
  static_vector<units::bytes, MAX_SI_MESSAGES> si_payload_sizes;
  for (const auto& si_msg : si_messages) {
    size_t si_msg_len = si_msg.front().length();
    // If the SI message has multiple segments, check that all segments have the same length.
    if (si_msg.size() > 1) {
      if (!std::all_of(si_msg.begin(), si_msg.end(), [si_msg_len](const byte_buffer& si_msg_) {
            return si_msg_.length() == si_msg_len;
          })) {
        report_error("All segments of an SI message must have the same length.");
      }
    }
    si_payload_sizes.emplace_back(units::bytes{static_cast<unsigned>(si_msg_len)});
  }
  // A warning message may end in a shorter segment, so its grants are sized off the largest one.
  static_vector<units::bytes, MAX_PWS_SI_MESSAGES> pws_payload_sizes;
  for (const auto& pws_si_msg : pws_si_messages) {
    pws_payload_sizes.emplace_back(largest_segment_len(pws_si_msg));
  }
  si_sched_cfg = make_si_scheduling_info_config(cell_cfg, sib1_len, si_payload_sizes, pws_payload_sizes);
}

void du_cell_manager::add_cell(const du_cell_config& cell_cfg)
{
  // Verify that DU cell configuration is valid. Abort application otherwise.
  auto ret = is_du_cell_config_valid(cell_cfg);
  if (not ret.has_value()) {
    report_error("ERROR: Invalid DU Cell Configuration. Cause: {}.\n", ret.error());
  }

  // Generate system information.
  std::vector<bcch_dl_sch_payload_type> bcch_msgs = asn1_packer::pack_all_bcch_dl_sch_msgs(cell_cfg);

  ocudu_assert(bcch_msgs[0].size() == 1, "SIB-1 cannot be segmented");
  const byte_buffer& sib1 = bcch_msgs[0].front();

  span<const bcch_dl_sch_payload_type> si_messages =
      span<const bcch_dl_sch_payload_type>(bcch_msgs).last(bcch_msgs.size() - 1);

  // Save config.
  du_cell_context& cell = *cells.emplace_back(std::make_unique<du_cell_context>());
  cell.cfg              = cell_cfg;
  cell.state            = du_cell_context::state_t::inactive;
  cell.live_barred      = cell_cfg.cell_barred;
  cell.si_cfg.sib1      = sib1.copy();
  cell.si_cfg.si_messages.assign(si_messages.begin(), si_messages.end());
  std::vector<bcch_dl_sch_payload_type> pws_msgs = asn1_packer::pack_pws_si_messages(cell_cfg);
  cell.si_cfg.pws_si_messages.assign(pws_msgs.begin(), pws_msgs.end());
  cell.si_cfg.sib1_contains_hypersfn = cell_cfg.ran.init_bwp.paging.edrx_enabled;

  // Generate Scheduler SI scheduling config.
  fill_si_scheduler_config(cell.si_cfg.si_sched_cfg, cell_cfg, sib1, si_messages, cell.si_cfg.pws_si_messages);
}

expected<du_cell_reconfig_result>
du_cell_manager::handle_cell_reconf_request(const du_cell_param_config_request& req) const
{
  if (!req.nr_cgi.has_value()) {
    logger.warning("DU Cell Reconfiguration request without NR CGI is not supported");
    return make_unexpected(default_error_t{});
  }

  du_cell_index_t cell_index = get_cell_index(req.nr_cgi.value());
  if (cell_index == INVALID_DU_CELL_INDEX) {
    logger.warning("Discarding cell {} changes. Cause: No cell with the provided CGI was found",
                   req.nr_cgi.value().nci);
    return make_unexpected(default_error_t{});
  }
  auto& cell = *cells[cell_index];

  du_cell_config& cell_cfg   = cell.cfg;
  bool            si_updated = false;

  if (req.ssb_pwr_mod.has_value() and req.ssb_pwr_mod.value() != cell_cfg.ran.ssb_cfg.ssb_block_power) {
    // SSB power changed.
    cell_cfg.ran.ssb_cfg.ssb_block_power = req.ssb_pwr_mod.value();
    si_updated                           = true;
  }

  // Update SIB info in cell config if provided.
  if (req.new_sys_info.has_value()) {
    // Ensure si_config exists.
    if (not cell_cfg.si.si_config.has_value()) {
      logger.warning("Cell {} has no SI config, cannot update SIB", cell_index);
    } else {
      sib_type type = get_sib_info_type(req.new_sys_info.value());

      // Find existing entry for this SIB type.
      auto sib_it = std::find_if(cell_cfg.si.si_config->sibs.begin(),
                                 cell_cfg.si.si_config->sibs.end(),
                                 [type](const sib_type_info& sib) { return get_sib_info_type(sib.content) == type; });

      if (sib_it != cell_cfg.si.si_config->sibs.end()) {
        sib_it->content = req.new_sys_info.value();
        // Increment value_tag with wrapping (5-bit field: 0-31).
        sib_it->value_tag = (sib_it->value_tag.value() + 1) % 32;
        si_updated        = true;
        logger.info("Updated SIB{} in cell {} config, new value_tag={}",
                    static_cast<int>(type),
                    cell_index,
                    sib_it->value_tag.value());
      } else {
        logger.warning("Requested SIB{} update in cell {}, but entry not found.", static_cast<int>(type), cell_index);
      }
    }
  }

  const unsigned nof_prbs = band_helper::get_n_rbs_from_bw(cell_cfg.ran.dl_carrier.carrier_bw,
                                                           cell_cfg.ran.dl_cfg_common.init_dl_bwp.generic_params.scs,
                                                           band_helper::get_freq_range(cell_cfg.ran.dl_carrier.band));

  du_cell_reconfig_result result;
  result.slice_reconf_req.emplace();
  for (const auto& rrm_policy_ratio : req.rrm_policy_ratio_list) {
    if (not(rrm_policy_ratio.minimum_ratio.has_value() or rrm_policy_ratio.maximum_ratio.has_value())) {
      continue;
    }

    for (const auto& policy_member : rrm_policy_ratio.policy_members_list) {
      bool found = false;
      for (auto& policy_cfg : cell_cfg.rrm_policy_members) {
        if (policy_cfg.rrc_member == policy_member) {
          found = true;
          // Update the policy member configuration.
          unsigned min_prb_ratio = rrm_policy_ratio.minimum_ratio.value_or(0);
          unsigned max_prb_ratio = rrm_policy_ratio.maximum_ratio.value_or(100);

          min_prb_ratio = std::clamp(min_prb_ratio, static_cast<unsigned>(0), static_cast<unsigned>(100));
          max_prb_ratio = std::clamp(max_prb_ratio, static_cast<unsigned>(0), static_cast<unsigned>(100));

          const unsigned min_prb = static_cast<int>((1.0 * min_prb_ratio / 100) * nof_prbs);
          const unsigned max_prb = static_cast<int>((1.0 * max_prb_ratio / 100) * nof_prbs);

          if (min_prb > max_prb) {
            logger.warning(
                "Invalid min/max PRB policy ratio for {} in cell {}: min_prb={} > max_prb={}. Skipping update.",
                policy_member,
                cell_index,
                min_prb,
                max_prb);
            break;
          }

          if ((policy_cfg.rbs.min() != min_prb) or (policy_cfg.rbs.max() != max_prb)) {
            // Policy configuration has been updated.
            result.slice_reconf_req->rrm_policies.push_back(
                du_cell_slice_reconfig_request::rrm_policy_config{policy_member, {min_prb, max_prb}});
          }

          policy_cfg.rbs = {min_prb, max_prb};
          break;
        }
      }
      if (not found) {
        logger.warning("No RRM policy member found for {} in cell {}", policy_member, cell_index);
      }

      if (result.slice_reconf_req->rrm_policies.full()) {
        logger.warning("RRM policy update list is full. Discarding further updates for cell {}", cell_index);
        break;
      }
    }
  }

  if (si_updated) {
    if (req.new_sys_info.has_value()) {
      // Other SIB msg was updated, repack ALL SIBs (SIB1 + SI messages).
      logger.info("Repacking all BCCH-DL-SCH messages for cell {} (SIB update)", cell_index);
      std::vector<bcch_dl_sch_payload_type> bcch_msgs = asn1_packer::pack_all_bcch_dl_sch_msgs(cell_cfg);

      ocudu_assert(bcch_msgs[0].size() == 1, "SIB-1 cannot be segmented");
      cell.si_cfg.sib1 = bcch_msgs[0].front().copy();

      span<const bcch_dl_sch_payload_type> si_messages =
          span<const bcch_dl_sch_payload_type>(bcch_msgs).last(bcch_msgs.size() - 1);
      cell.si_cfg.si_messages.assign(si_messages.begin(), si_messages.end());

      std::vector<bcch_dl_sch_payload_type> pws_msgs = asn1_packer::pack_pws_si_messages(cell_cfg);
      cell.si_cfg.pws_si_messages.assign(pws_msgs.begin(), pws_msgs.end());
    } else {
      // Only SSB power changed, repack only SIB1.
      cell.si_cfg.sib1 = asn1_packer::pack_sib1(cell_cfg);
    }

    // Update SI scheduling config. The SI version is owned by the MAC.
    fill_si_scheduler_config(
        cell.si_cfg.si_sched_cfg, cell_cfg, cell.si_cfg.sib1, cell.si_cfg.si_messages, cell.si_cfg.pws_si_messages);
  }

  result.cell_index           = cell_index;
  result.cu_notif_required    = si_updated;
  result.sched_notif_required = si_updated;
  if (result.slice_reconf_req->rrm_policies.empty()) {
    // No RRM policy changes.
    result.slice_reconf_req.reset();
  }
  return result;
}

async_task<bool> du_cell_manager::start(du_cell_index_t cell_index) const
{
  return launch_async([this, cell_index](coro_context<async_task<bool>>& ctx) {
    CORO_BEGIN(ctx);
    if (!has_cell(cell_index)) {
      logger.warning("cell={}: Start called for a cell that does not exist.", cell_index);
      CORO_EARLY_RETURN(false);
    }
    if (cells[cell_index]->state != du_cell_context::state_t::inactive) {
      logger.warning("cell={}: Start called for an already active cell.", cell_index);
      CORO_EARLY_RETURN(false);
    }

    // On restart, the live MIB cellBarred flag may have been left set to barred by a prior bar-first cell stop.
    // Restore the configured value *before* starting the MAC cell, so the first SSB built once the cell goes
    // active already advertises the operator-configured cellBarred instead of briefly re-airing the stale
    // barred flag. Skipped when the live flag already matches the configured value (first start, or a restart
    // with no runtime bar in between): the restore is an extra awaited hop to the cell executor on the cell
    // (re)activation path, and a redundant one only delays the cell going active. This runs on a stopped cell:
    // reconfigure() only hops to the cell executor to store the flag, it does not depend on the cell being
    // active.
    if (cells[cell_index]->live_barred != cells[cell_index]->cfg.cell_barred) {
      CORO_AWAIT(set_cell_barred(cell_index, cells[cell_index]->cfg.cell_barred));
    }

    // Start cell in the MAC.
    CORO_AWAIT(cfg.mac.mgr.get_cell_manager().get_cell_controller(cell_index).start());

    cells[cell_index]->state = du_cell_context::state_t::active;

    CORO_RETURN(true);
  });
}

async_task<void> du_cell_manager::set_cell_barred(du_cell_index_t cell_index, bool barred) const
{
  mac_cell_reconfig_request mac_req;
  mac_req.cell_barred_mod.emplace(barred);

  return launch_async([this, cell_index, barred, mac_req](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    if (!has_cell(cell_index)) {
      logger.warning("cell={}: set_cell_barred called for a cell that does not exist.", fmt::underlying(cell_index));
      CORO_EARLY_RETURN();
    }

    CORO_AWAIT(cfg.mac.mgr.get_cell_manager().get_cell_controller(cell_index).reconfigure(mac_req));

    cells[cell_index]->live_barred = barred;

    logger.info("cell={}: MIB cellBarred set to {}", fmt::underlying(cell_index), barred);

    CORO_RETURN();
  });
}

async_task<void> du_cell_manager::set_cell_barred_and_wait(du_cell_index_t cell_index) const
{
  if (!has_cell(cell_index)) {
    logger.warning("cell={}: set_cell_barred_and_wait called for a cell that does not exist.",
                   fmt::underlying(cell_index));
    return launch_no_op_task();
  }

  // If the cell is already barred (e.g. the CU barred it via a previous gNB-CU Configuration Update carrying
  // the Cells to be Barred List), skip the re-bar but still hold the settling window: the tracked state only
  // records that the MIB flag was applied at the MAC, not that a barred SSB has been transmitted, and the CU
  // may bar and deactivate in immediate succession (even within one configuration update). Holding the window
  // guarantees the barred MIB airs at least once before the stop that follows this call halts SSB.
  const bool already_barred = is_cell_barred(cell_index);
  if (already_barred) {
    logger.debug("cell={}: cell already barred. Skipping re-bar and holding the settling window.",
                 fmt::underlying(cell_index));
  }

  // Derive the settling window from the cell's configured SSB period: the barred MIB only needs to reach the
  // air before released/idle UEs reselect, so hold a couple of SSB periods to guarantee it is transmitted at
  // least once with margin. Meant to run concurrently with the UE drain, so it adds no latency in the common
  // case.
  const unsigned                  ssb_period_ms = to_underlying(get_cell_cfg(cell_index).ran.ssb_cfg.ssb_period);
  const std::chrono::milliseconds bar_settling_window{2 * ssb_period_ms};
  unique_timer                    settling_timer = cfg.services.timers.create_unique_timer(cfg.services.du_mng_exec);

  return launch_async(
      [this, cell_index, already_barred, bar_settling_window, settling_timer = std::move(settling_timer)](
          coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        if (!already_barred) {
          CORO_AWAIT(set_cell_barred(cell_index, true));
        }

        CORO_AWAIT(async_wait_for(settling_timer, bar_settling_window));

        CORO_RETURN();
      });
}

async_task<void> du_cell_manager::stop(du_cell_index_t cell_index) const
{
  return launch_async([this, cell_index](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    if (!has_cell(cell_index)) {
      logger.warning("cell={}: Stop called for a cell that does not exist.", cell_index);
      CORO_EARLY_RETURN();
    }
    if (cells[cell_index]->state == du_cell_context::state_t::inactive) {
      // Ignore.
      CORO_EARLY_RETURN();
    }
    cells[cell_index]->state = du_cell_context::state_t::inactive;

    // Stop cell in the MAC.
    CORO_AWAIT(cfg.mac.mgr.get_cell_manager().get_cell_controller(cell_index).stop());

    CORO_RETURN();
  });
}

async_task<void> du_cell_manager::stop_all() const
{
  return launch_async([this, i = 0U](coro_context<async_task<void>>& ctx) mutable {
    CORO_BEGIN(ctx);

    for (; i != cells.size(); ++i) {
      if (cells[i] != nullptr and cells[i]->state == du_cell_context::state_t::active) {
        cells[i]->state = du_cell_context::state_t::inactive;

        CORO_AWAIT(cfg.mac.mgr.get_cell_manager().get_cell_controller(to_du_cell_index(i)).stop());
      }
    }

    CORO_RETURN();
  });
}

void du_cell_manager::remove_all_cells()
{
  for (unsigned i = 0; i != cells.size(); ++i) {
    ocudu_assert(cells[i] != nullptr, "Cell {} is null", i);
    ocudu_assert(cells[i]->state != du_cell_context::state_t::active, "Cell {} is still active", i);
    cfg.mac.mgr.get_cell_manager().remove_cell(to_du_cell_index(i));
  }
  cells.clear();
}

du_cell_index_t du_cell_manager::get_cell_index(nr_cell_global_id_t nr_cgi) const
{
  du_cell_index_t cell_index = du_cell_index_t::INVALID_DU_CELL_INDEX;
  for (unsigned i = 0, e = nof_cells(); i != e; ++i) {
    const du_cell_config& cell_it = get_cell_cfg(to_du_cell_index(i));
    if (cell_it.nr_cgi == nr_cgi) {
      cell_index = to_du_cell_index(i);
      break;
    }
  }
  return cell_index;
}

du_cell_index_t du_cell_manager::get_cell_index(pci_t pci) const
{
  du_cell_index_t cell_index = du_cell_index_t::INVALID_DU_CELL_INDEX;
  for (unsigned i = 0, e = nof_cells(); i != e; ++i) {
    const du_cell_config& cell_it = get_cell_cfg(to_du_cell_index(i));
    if (cell_it.ran.pci == pci) {
      cell_index = to_du_cell_index(i);
      break;
    }
  }
  return cell_index;
}
