// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_meas_config_manager.h"
#include "du_ue_resource_config.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg_ies.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/csi_rs/csi_meas_config.h"
#include "ocudu/ran/csi_rs/csi_report_config.h"
#include "ocudu/ran/sr_configuration.h"
#include "ocudu/ran/ssb/ssb_properties.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <array>
#include <optional>
#include <vector>

using namespace ocudu;
using namespace odu;
using namespace asn1::rrc_nr;

static bool unpack_meas_cfg(meas_cfg_s& meas_cfg, const byte_buffer& container)
{
  asn1::cbit_ref bref{container};
  return meas_cfg.unpack(bref) == asn1::OCUDUASN_SUCCESS;
}

namespace {

// Default Measurement Gap Length for a given PCell SCS and SMTC duration.
meas_gap_length get_default_mgl(subcarrier_spacing scs, ssb_mtc_s::dur_opts::options dur)
{
  switch (dur) {
    case ssb_mtc_s::dur_opts::sf1:
      return scs != subcarrier_spacing::kHz15 ? meas_gap_length::ms1dot5 : meas_gap_length::ms3;
    case ssb_mtc_s::dur_opts::sf2:
      return meas_gap_length::ms3;
    case ssb_mtc_s::dur_opts::sf3:
      return meas_gap_length::ms4;
    case ssb_mtc_s::dur_opts::sf4:
    case ssb_mtc_s::dur_opts::sf5:
      return meas_gap_length::ms6;
    default:
      report_fatal_error("Invalid SSB MTC duration");
  }
}

std::pair<ssb_periodicity, unsigned> extract_smtc_period_offset(const ssb_mtc_s& smtc1)
{
  using opts = ssb_mtc_s::periodicity_and_offset_c_::types_opts;
  switch (smtc1.periodicity_and_offset.type().value) {
    case opts::sf5:
      return {ssb_periodicity::ms5, smtc1.periodicity_and_offset.sf5()};
    case opts::sf10:
      return {ssb_periodicity::ms10, smtc1.periodicity_and_offset.sf10()};
    case opts::sf20:
      return {ssb_periodicity::ms20, smtc1.periodicity_and_offset.sf20()};
    case opts::sf40:
      return {ssb_periodicity::ms40, smtc1.periodicity_and_offset.sf40()};
    case opts::sf80:
      return {ssb_periodicity::ms80, smtc1.periodicity_and_offset.sf80()};
    case opts::sf160:
      return {ssb_periodicity::ms160, smtc1.periodicity_and_offset.sf160()};
    default:
      report_fatal_error("Invalid SSB MTC periodicity_and_offset");
  }
}

enum class collision_check { strict, loose };

bool meas_gap_collides(const meas_gap_config&          gap,
                       subcarrier_spacing              scs,
                       span<const periodic_uci_config> ul_occasions,
                       collision_check                 mode)
{
  const unsigned mgrp_slots = static_cast<unsigned>(gap.mgrp) * get_nof_slots_per_subframe(scs);
  for (const auto& occ : ul_occasions) {
    if (mode == collision_check::strict) {
      // `strict` check collides when any periodic UL occasion repetition overlaps the measurement gap.
      for (unsigned slot = occ.offset_slots; slot < mgrp_slots; slot += occ.period_slots) {
        if (is_inside_meas_gap(gap, slot_point(scs, slot))) {
          return true;
        }
      }
    } else {
      // `loose` check collides when all periodic UL resource repetitions overlap the measurement gap.
      bool all_inside_meas_gap = true;
      for (unsigned slot = occ.offset_slots; slot < mgrp_slots; slot += occ.period_slots) {
        if (!is_inside_meas_gap(gap, slot_point(scs, slot))) {
          all_inside_meas_gap = false;
          break;
        }
      }
      if (all_inside_meas_gap) {
        return true;
      }
    }
  }
  return false;
}

} // namespace

meas_gap_config
odu::create_meas_gap(subcarrier_spacing scs, const ssb_mtc_s& smtc1, span<const periodic_uci_config> ul_occasions)
{
  const meas_gap_length mgl              = get_default_mgl(scs, smtc1.dur);
  const auto            smtc_po          = extract_smtc_period_offset(smtc1);
  const unsigned        smtc_period_ms   = static_cast<unsigned>(smtc_po.first);
  const unsigned        smtc_offset_ms   = smtc_po.second;
  const unsigned        smtc_duration_ms = smtc1.dur.to_number();
  const unsigned        slots_per_ms     = get_nof_slots_per_subframe(scs);

  // Maximum integer ms we can slide the measGap left of an SMTC start while still fully enclosing the
  // SMTC window.
  const int offset_slack_ms = static_cast<int>(meas_gap_length_to_msec(mgl)) - static_cast<int>(smtc_duration_ms);
  ocudu_assert(offset_slack_ms >= 0,
               "MGL must be >= SMTC duration (got MGL={}ms, SMTC duration={}ms)",
               meas_gap_length_to_msec(mgl),
               smtc_duration_ms);

  // Measurement Gap Repetition Period must be at least as long as the SMTC period and all UL occasion periods
  // to ensure that one MGRP contains at least one SSB, SR and periodic CSI occasion we have to align it with.
  unsigned min_mgrp_ms = smtc_period_ms;
  for (const auto& occ : ul_occasions) {
    ocudu_assert(occ.period_slots > 0, "Periodic UL occasion must have a non-zero period");
    ocudu_assert(occ.offset_slots < occ.period_slots,
                 "Periodic UL occasion offset ({}) must be less than its period ({})",
                 occ.offset_slots,
                 occ.period_slots);
    const unsigned occ_period_ms = (occ.period_slots + slots_per_ms - 1) / slots_per_ms;
    min_mgrp_ms                  = std::max(min_mgrp_ms, occ_period_ms);
  }

  static constexpr std::array<meas_gap_repetition_period, 4> mgrp_candidates = {meas_gap_repetition_period::ms20,
                                                                                meas_gap_repetition_period::ms40,
                                                                                meas_gap_repetition_period::ms80,
                                                                                meas_gap_repetition_period::ms160};

  // Searches for a gap offset within MGRP that fully encloses some SMTC window and passes the
  // collision check. Each SMTC repetition offers `offset_slack_ms + 1` candidate offsets — sliding
  // the gap left by 0..slack ms keeps the SMTC fully inside the gap.
  auto find_non_colliding_offset = [&](meas_gap_repetition_period mgrp,
                                       collision_check            mode) -> std::optional<meas_gap_config> {
    const unsigned mgrp_ms = static_cast<unsigned>(mgrp);
    // All SSB repetitions in a given MGRP.
    for (unsigned smtc_start_ms = smtc_offset_ms; smtc_start_ms < mgrp_ms; smtc_start_ms += smtc_period_ms) {
      // All measGap offsets that still allow to fully catch the SSB.
      for (int shift_ms = 0; shift_ms <= offset_slack_ms; ++shift_ms) {
        const unsigned        gap_offset_ms = (smtc_start_ms + mgrp_ms - shift_ms) % mgrp_ms;
        const meas_gap_config candidate{gap_offset_ms, mgl, mgrp};
        if (!meas_gap_collides(candidate, scs, ul_occasions, mode)) {
          return candidate;
        }
      }
    }
    return std::nullopt;
  };

  // Search for strictly non-colliding measGap offset for all possible MGRP values.
  for (auto mgrp : mgrp_candidates) {
    if (static_cast<unsigned>(mgrp) < min_mgrp_ms) {
      continue;
    }
    if (auto candidate = find_non_colliding_offset(mgrp, collision_check::strict)) {
      return *candidate;
    }
    break;
  }

  // Search for non-colliding measGap offset that allows all periodic UL occasions to have at least one non-colliding
  // instance in a given MGRP.
  for (auto mgrp : mgrp_candidates) {
    if (static_cast<unsigned>(mgrp) < min_mgrp_ms) {
      continue;
    }
    if (auto candidate = find_non_colliding_offset(mgrp, collision_check::loose)) {
      return *candidate;
    }
  }

  // Best-effort fallback: largest MGRP with the offset equal to the SMTC offset.
  // The gap will align with the SMTC but some UL occasions still collide.
  return meas_gap_config{smtc_offset_ms, mgl, meas_gap_repetition_period::ms160};
}

du_meas_config_manager::du_meas_config_manager(span<const du_cell_config> cell_cfg_list_) :
  cell_cfg_list(cell_cfg_list_), logger(ocudulog::fetch_basic_logger("DU-MNG"))
{
}

// Collects SR and periodic-CSI occasions from the UE's PCell serving cell config.
static std::vector<periodic_uci_config> collect_ul_occasions(const ue_cell_config& pcell_ue_cfg)
{
  std::vector<periodic_uci_config> out;

  if (pcell_ue_cfg.serv_cell_cfg.ul_config.has_value() &&
      pcell_ue_cfg.serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.has_value()) {
    for (const auto& sr : pcell_ue_cfg.serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg->sr_res_list) {
      out.push_back({sr_periodicity_to_slot(sr.period), sr.offset});
    }
  }

  if (pcell_ue_cfg.serv_cell_cfg.csi_meas_cfg.has_value()) {
    for (const auto& rep : pcell_ue_cfg.serv_cell_cfg.csi_meas_cfg->csi_report_cfg_list) {
      const auto* pucch_rep =
          std::get_if<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(&rep.report_cfg_type);
      if (pucch_rep != nullptr &&
          pucch_rep->report_type ==
              csi_report_config::periodic_or_semi_persistent_report_on_pucch::report_type_t::periodic) {
        out.push_back({csi_report_periodicity_to_uint(pucch_rep->report_slot_period), pucch_rep->report_slot_offset});
      }
    }
  }

  return out;
}

void du_meas_config_manager::update(du_ue_resource_config& ue_cfg, const byte_buffer& packed_meas_cfg)
{
  if (packed_meas_cfg.empty()) {
    return;
  }

  meas_cfg_s meas_cfg;
  if (not unpack_meas_cfg(meas_cfg, packed_meas_cfg)) {
    logger.error("Failed to unpack meas config. Discarding it...");
    return;
  }

  const ue_cell_config& pcell_ue_cfg = ue_cfg.cell_group.cells.at(SERVING_PCELL_IDX);
  const du_cell_config& pcell_common = cell_cfg_list[pcell_ue_cfg.serv_cell_cfg.cell_index];

  const auto ul_occasions = collect_ul_occasions(pcell_ue_cfg);

  for (const auto& asn1measobj : meas_cfg.meas_obj_to_add_mod_list) {
    if (asn1measobj.meas_obj.type().value != meas_obj_to_add_mod_s::meas_obj_c_::types_opts::meas_obj_nr) {
      logger.warning("Ignoring measObject of type {}. Cause: Unsupported", asn1measobj.meas_obj.type().to_string());
      continue;
    }
    const auto& asn1nr = asn1measobj.meas_obj.meas_obj_nr();

    if (not asn1nr.ssb_freq_present or not asn1nr.smtc1_present) {
      logger.info("Ignoring measObject of type {}. Cause: Lack of a SSB frequency or SMTC1 config",
                  asn1measobj.meas_obj.type().to_string());
      continue;
    }

    if (asn1nr.ssb_freq == pcell_common.ran.dl_cfg_common.freq_info_dl.absolute_frequency_ssb) {
      // Same frequency. No need for measGap.
      continue;
    }

    ue_cfg.meas_gap =
        create_meas_gap(pcell_common.ran.dl_cfg_common.init_dl_bwp.generic_params.scs, asn1nr.smtc1, ul_occasions);
  }
}
