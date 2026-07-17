// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/du/du_high/du_manager/converters/asn1_ntn_config_helpers.h"
#include "lib/ntn/ntn_sib19_helpers.h"
#include "ocudu/asn1/rrc_nr/bcch_dl_sch_msg.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/pcap/mac_pcap.h"
#include "ocudu/ran/ntn.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/executors/task_worker.h"
#include <array>
#include <cstdio>
#include <gtest/gtest.h>
#include <memory>

using namespace ocudu;
using namespace ocudu_ntn;

namespace {

ntn_orbital_state make_reply(bool success)
{
  ntn_orbital_state reply{};
  reply.success        = success;
  reply.ephemeris_info = ecef_coordinates_t{};
  return reply;
}

/// Builds a neighbor cell config. Two configs built with the same arguments are, by construction, identical in
/// every field that ntn_cfg_would_be_identical() checks.
ntn_neighbor_cell_config make_ncell(unsigned                satellite_index,
                                    std::optional<unsigned> ntn_ul_sync_validity_dur = 30U,
                                    unsigned                k_mac                    = 2U,
                                    bool                    ta_report                = true)
{
  ntn_neighbor_cell_config cfg{};
  cfg.satellite_index          = satellite_index;
  cfg.ntn_ul_sync_validity_dur = ntn_ul_sync_validity_dur;
  cfg.cell_specific_koffset    = std::chrono::milliseconds{10};
  cfg.k_mac                    = k_mac;
  cfg.ta_report                = ta_report;
  return cfg;
}

ntn_cell_config make_cell_config(bool serving_is_ntn, unsigned serving_sync_dur = 5U)
{
  ntn_cell_config cell_cfg{};
  cell_cfg.si_sched = ntn_si_scheduling_info{/*si_msg_idx=*/0,
                                             /*si_period_rf=*/1,
                                             /*si_window_len_slots=*/1,
                                             /*si_window_position=*/0};
  if (serving_is_ntn) {
    ntn_serving_cell_config serving{};
    serving.satellite_index          = 0;
    serving.cell_specific_koffset    = std::chrono::milliseconds{0};
    serving.ntn_ul_sync_validity_dur = serving_sync_dur;
    cell_cfg.ntn_cfg                 = serving;
  }
  return cell_cfg;
}

ntn_sat_switch_config make_sat_switch(unsigned satellite_index, std::optional<unsigned> ntn_ul_sync_validity_dur = 30U)
{
  ntn_sat_switch_config cfg{};
  cfg.satellite_index          = satellite_index;
  cfg.ntn_ul_sync_validity_dur = ntn_ul_sync_validity_dur;
  cfg.cell_specific_koffset    = std::chrono::milliseconds{15};
  cfg.k_mac                    = 3U;
  cfg.ta_report                = true;
  return cfg;
}

const slot_point test_epoch_slot{0, 100};

} // namespace

TEST(sib19_ncells_test, first_entry_is_always_present_and_epoch_time_omitted_when_serving_is_ntn)
{
  ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/true);
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/1));

  std::vector<ntn_orbital_state> replies       = {make_reply(true)};
  ntn_orbital_state              serving_reply = make_reply(true);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, nullptr, replies);

  ASSERT_EQ(sib19.ncells.size(), 1);
  ASSERT_TRUE(sib19.ncells[0].ntn_cfg.has_value());
  // Matches the serving NTN cell's own epoch time (also test_epoch_slot), so it can be left absent.
  EXPECT_FALSE(sib19.ncells[0].ntn_cfg->epoch_time.has_value());
}

TEST(sib19_ncells_test, epoch_time_is_filled_when_serving_cell_is_tn)
{
  ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/false);
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/1));

  std::vector<ntn_orbital_state> replies       = {make_reply(true)};
  ntn_orbital_state              serving_reply = make_reply(true);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, nullptr, replies);

  ASSERT_EQ(sib19.ncells.size(), 1);
  ASSERT_TRUE(sib19.ncells[0].ntn_cfg.has_value());
  ASSERT_TRUE(sib19.ncells[0].ntn_cfg->epoch_time.has_value());
  EXPECT_EQ(sib19.ncells[0].ntn_cfg->epoch_time->sfn, test_epoch_slot.sfn());
  EXPECT_EQ(sib19.ncells[0].ntn_cfg->epoch_time->subframe_number, test_epoch_slot.subframe_index());
}

TEST(sib19_ncells_test, ntn_ul_sync_validity_dur_is_always_filled_when_serving_cell_is_tn)
{
  ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/false);
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/1, /*ntn_ul_sync_validity_dur=*/30U));

  std::vector<ntn_orbital_state> replies       = {make_reply(true)};
  ntn_orbital_state              serving_reply = make_reply(true);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, nullptr, replies);

  ASSERT_EQ(sib19.ncells.size(), 1);
  ASSERT_TRUE(sib19.ncells[0].ntn_cfg.has_value());
  ASSERT_TRUE(sib19.ncells[0].ntn_cfg->ntn_ul_sync_validity_dur.has_value())
      << "there is no serving cell value to fall back to when the serving cell is TN";
  EXPECT_EQ(*sib19.ncells[0].ntn_cfg->ntn_ul_sync_validity_dur, 30U);
}

TEST(sib19_ncells_test, ntn_ul_sync_validity_dur_omitted_only_when_it_matches_serving_cell)
{
  ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/true, /*serving_sync_dur=*/30U);
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/1, /*ntn_ul_sync_validity_dur=*/30U));
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/2, /*ntn_ul_sync_validity_dur=*/60U));

  std::vector<ntn_orbital_state> replies       = {make_reply(true), make_reply(true)};
  ntn_orbital_state              serving_reply = make_reply(true);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, nullptr, replies);

  ASSERT_EQ(sib19.ncells.size(), 2);
  ASSERT_TRUE(sib19.ncells[0].ntn_cfg.has_value());
  EXPECT_FALSE(sib19.ncells[0].ntn_cfg->ntn_ul_sync_validity_dur.has_value())
      << "matches the serving cell's own validity duration, so it can be left absent";

  ASSERT_TRUE(sib19.ncells[1].ntn_cfg.has_value());
  ASSERT_TRUE(sib19.ncells[1].ntn_cfg->ntn_ul_sync_validity_dur.has_value());
  EXPECT_EQ(*sib19.ncells[1].ntn_cfg->ntn_ul_sync_validity_dur, 60U);
}

TEST(sib19_ncells_test, second_entry_inherits_whole_block_when_identical_to_first)
{
  ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/false);
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/1));
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/1));

  std::vector<ntn_orbital_state> replies       = {make_reply(true), make_reply(true)};
  ntn_orbital_state              serving_reply = make_reply(true);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, nullptr, replies);

  ASSERT_EQ(sib19.ncells.size(), 2);
  EXPECT_TRUE(sib19.ncells[0].ntn_cfg.has_value());
  EXPECT_FALSE(sib19.ncells[1].ntn_cfg.has_value());
}

TEST(sib19_ncells_test, second_entry_explicit_when_any_static_field_differs_even_on_same_satellite)
{
  auto expect_both_explicit = [](const ntn_neighbor_cell_config& first, const ntn_neighbor_cell_config& second) {
    ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/false);
    cell_cfg.ncells.push_back(first);
    cell_cfg.ncells.push_back(second);

    std::vector<ntn_orbital_state> replies       = {make_reply(true), make_reply(true)};
    ntn_orbital_state              serving_reply = make_reply(true);

    sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, nullptr, replies);

    ASSERT_EQ(sib19.ncells.size(), 2);
    EXPECT_TRUE(sib19.ncells[0].ntn_cfg.has_value());
    EXPECT_TRUE(sib19.ncells[1].ntn_cfg.has_value())
        << "second entry must not inherit when a static field differs from the first, even on the same satellite";
  };

  const ntn_neighbor_cell_config base = make_ncell(/*satellite_index=*/1);

  {
    ntn_neighbor_cell_config other = base;
    other.ntn_ul_sync_validity_dur = 60U;
    expect_both_explicit(base, other);
  }
  {
    ntn_neighbor_cell_config other = base;
    other.cell_specific_koffset    = std::chrono::milliseconds{20};
    expect_both_explicit(base, other);
  }
  {
    ntn_neighbor_cell_config other = base;
    other.k_mac                    = 99U;
    expect_both_explicit(base, other);
  }
  {
    ntn_neighbor_cell_config other = base;
    other.ta_report                = false;
    expect_both_explicit(base, other);
  }
  {
    ntn_neighbor_cell_config other = base;
    other.polarization             = ntn_polarization_t{ntn_polarization_t::polarization_type::rhcp, std::nullopt};
    expect_both_explicit(base, other);
  }
}

TEST(sib19_ncells_test, run_of_identical_entries_fully_compresses_not_just_alternate_entries)
{
  ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/false);
  for (unsigned i = 0; i != 4; ++i) {
    cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/9));
  }

  std::vector<ntn_orbital_state> replies(4, make_reply(true));
  ntn_orbital_state              serving_reply = make_reply(true);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, nullptr, replies);

  ASSERT_EQ(sib19.ncells.size(), 4);
  EXPECT_TRUE(sib19.ncells[0].ntn_cfg.has_value());
  EXPECT_FALSE(sib19.ncells[1].ntn_cfg.has_value());
  EXPECT_FALSE(sib19.ncells[2].ntn_cfg.has_value());
  EXPECT_FALSE(sib19.ncells[3].ntn_cfg.has_value());
}

TEST(sib19_ncells_test, entry_with_failed_ocm_lookup_is_dropped_and_first_remaining_entry_is_still_mandatory)
{
  ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/false);
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/1)); // OCM fails for this one.
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/2));

  std::vector<ntn_orbital_state> replies       = {make_reply(false), make_reply(true)};
  ntn_orbital_state              serving_reply = make_reply(true);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, nullptr, replies);

  ASSERT_EQ(sib19.ncells.size(), 1);
  EXPECT_TRUE(sib19.ncells[0].ntn_cfg.has_value());
}

TEST(sib19_ncells_test, failed_ocm_lookup_in_the_middle_does_not_break_the_inheritance_chain)
{
  ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/false);
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/5));
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/6)); // OCM fails for this one, dropped.
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/5)); // Same config as the first surviving entry.

  std::vector<ntn_orbital_state> replies       = {make_reply(true), make_reply(false), make_reply(true)};
  ntn_orbital_state              serving_reply = make_reply(true);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, nullptr, replies);

  ASSERT_EQ(sib19.ncells.size(), 2);
  EXPECT_TRUE(sib19.ncells[0].ntn_cfg.has_value());
  EXPECT_FALSE(sib19.ncells[1].ntn_cfg.has_value())
      << "third raw entry becomes the second broadcast entry and should inherit from the first";
}

TEST(sib19_ncells_test, ext_list_entry_is_explicit_when_same_position_base_entry_omitted_its_ntn_cfg)
{
  ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/false);
  // Six identical entries: base list positions 0-3 and ext list positions 4-5.
  for (unsigned i = 0; i != 6; ++i) {
    cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/7));
  }

  std::vector<ntn_orbital_state> replies(6, make_reply(true));
  ntn_orbital_state              serving_reply = make_reply(true);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, nullptr, replies);

  ASSERT_EQ(sib19.ncells.size(), 6);
  EXPECT_TRUE(sib19.ncells[0].ntn_cfg.has_value());
  EXPECT_FALSE(sib19.ncells[1].ntn_cfg.has_value());
  EXPECT_FALSE(sib19.ncells[2].ntn_cfg.has_value());
  EXPECT_FALSE(sib19.ncells[3].ntn_cfg.has_value());
  EXPECT_FALSE(sib19.ncells[4].ntn_cfg.has_value())
      << "same-position base entry (position 0) provides an explicit ntn-Config, so ext position 4 can inherit";
  EXPECT_TRUE(sib19.ncells[5].ntn_cfg.has_value())
      << "same-position base entry (position 1) omitted its ntn-Config, so ext position 5 has nothing to inherit";
}

TEST(sib19_ncells_test, ext_list_entry_inherits_from_same_position_base_entry_not_the_previous_entry)
{
  ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/false);
  // Four distinct base-list entries (positions 0-3).
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/1, 30U, /*k_mac=*/1U));
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/2, 30U, /*k_mac=*/2U));
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/3, 30U, /*k_mac=*/3U));
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/4, 30U, /*k_mac=*/4U));
  // First ext-list entry (position 4): identical to base position 0, NOT to base position 3 (the previous entry).
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/1, 30U, /*k_mac=*/1U));

  std::vector<ntn_orbital_state> replies(5, make_reply(true));
  ntn_orbital_state              serving_reply = make_reply(true);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, nullptr, replies);

  ASSERT_EQ(sib19.ncells.size(), 5);
  for (size_t i = 0; i != 4; ++i) {
    EXPECT_TRUE(sib19.ncells[i].ntn_cfg.has_value());
  }
  EXPECT_FALSE(sib19.ncells[4].ntn_cfg.has_value())
      << "ext entry matches base position 0 and must inherit from it, even though it differs from base position 3";
}

TEST(sib19_ncells_test, ext_list_entry_is_explicit_when_it_only_matches_the_previous_entry_not_the_same_position)
{
  ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/false);
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/1, 30U, /*k_mac=*/1U));
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/2, 30U, /*k_mac=*/2U));
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/3, 30U, /*k_mac=*/3U));
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/4, 30U, /*k_mac=*/4U));
  // First ext-list entry (position 4): identical to base position 3 (the previous raw entry), NOT to base position 0.
  cell_cfg.ncells.push_back(make_ncell(/*satellite_index=*/4, 30U, /*k_mac=*/4U));

  std::vector<ntn_orbital_state> replies(5, make_reply(true));
  ntn_orbital_state              serving_reply = make_reply(true);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, nullptr, replies);

  ASSERT_EQ(sib19.ncells.size(), 5);
  EXPECT_TRUE(sib19.ncells[4].ntn_cfg.has_value())
      << "ext entry must not inherit from the previous entry; it only compares against the same-position base entry";
}

TEST(sib19_sat_switch_test, not_broadcast_when_reply_is_null)
{
  ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/true);
  cell_cfg.sat_switch      = make_sat_switch(/*satellite_index=*/1);

  std::vector<ntn_orbital_state> replies;
  ntn_orbital_state              serving_reply = make_reply(true);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, /*sat_sw_reply=*/nullptr, replies);

  EXPECT_FALSE(sib19.sat_switch_with_resync.has_value());
}

TEST(sib19_sat_switch_test, not_broadcast_when_ocm_lookup_failed)
{
  ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/true);
  cell_cfg.sat_switch      = make_sat_switch(/*satellite_index=*/1);

  std::vector<ntn_orbital_state> replies;
  ntn_orbital_state              serving_reply = make_reply(true);
  ntn_orbital_state              sat_sw_reply  = make_reply(false);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, &sat_sw_reply, replies);

  EXPECT_FALSE(sib19.sat_switch_with_resync.has_value())
      << "ntn-Config is mandatory within SatSwitchWithReSync, so it must never be broadcast empty";
}

TEST(sib19_sat_switch_test, epoch_time_and_validity_dur_omitted_when_they_match_the_serving_ntn_cell)
{
  ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/true, /*serving_sync_dur=*/30U);
  cell_cfg.sat_switch      = make_sat_switch(/*satellite_index=*/1, /*ntn_ul_sync_validity_dur=*/30U);

  std::vector<ntn_orbital_state> replies;
  ntn_orbital_state              serving_reply = make_reply(true);
  ntn_orbital_state              sat_sw_reply  = make_reply(true);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, &sat_sw_reply, replies);

  ASSERT_TRUE(sib19.sat_switch_with_resync.has_value());
  EXPECT_FALSE(sib19.sat_switch_with_resync->ntn_cfg.epoch_time.has_value());
  EXPECT_FALSE(sib19.sat_switch_with_resync->ntn_cfg.ntn_ul_sync_validity_dur.has_value());
}

TEST(sib19_sat_switch_test, ntn_ul_sync_validity_dur_filled_when_it_differs_from_the_serving_ntn_cell)
{
  ntn_cell_config cell_cfg = make_cell_config(/*serving_is_ntn=*/true, /*serving_sync_dur=*/30U);
  cell_cfg.sat_switch      = make_sat_switch(/*satellite_index=*/1, /*ntn_ul_sync_validity_dur=*/60U);

  std::vector<ntn_orbital_state> replies;
  ntn_orbital_state              serving_reply = make_reply(true);
  ntn_orbital_state              sat_sw_reply  = make_reply(true);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, &sat_sw_reply, replies);

  ASSERT_TRUE(sib19.sat_switch_with_resync.has_value());
  ASSERT_TRUE(sib19.sat_switch_with_resync->ntn_cfg.ntn_ul_sync_validity_dur.has_value());
  EXPECT_EQ(*sib19.sat_switch_with_resync->ntn_cfg.ntn_ul_sync_validity_dur, 60U);
}

TEST(sib19_sat_switch_test, fields_left_unset_in_sat_switch_config_are_broadcast_with_the_serving_cell_values)
{
  ntn_cell_config cell_cfg                = make_cell_config(/*serving_is_ntn=*/true, /*serving_sync_dur=*/30U);
  cell_cfg.ntn_cfg->cell_specific_koffset = std::chrono::milliseconds{25};
  cell_cfg.ntn_cfg->k_mac                 = 2U;
  cell_cfg.ntn_cfg->ta_report             = true;

  // Sat-switch config with koffset, k_mac, polarization and ta_report left unset.
  ntn_sat_switch_config sw{};
  sw.satellite_index          = 1;
  sw.ntn_ul_sync_validity_dur = 30U;
  cell_cfg.sat_switch         = sw;

  std::vector<ntn_orbital_state> replies;
  ntn_orbital_state              serving_reply = make_reply(true);
  ntn_orbital_state              sat_sw_reply  = make_reply(true);

  sib19_info sib19 = generate_sib19_info(cell_cfg, test_epoch_slot, serving_reply, &sat_sw_reply, replies);

  ASSERT_TRUE(sib19.sat_switch_with_resync.has_value());
  const auto& sw_ntn_cfg = sib19.sat_switch_with_resync->ntn_cfg;
  EXPECT_EQ(sw_ntn_cfg.cell_specific_koffset, std::chrono::milliseconds{25})
      << "an absent koffset makes the UE assume 0, not the serving value the promotion inherits";
  EXPECT_EQ(sw_ntn_cfg.k_mac, 2U);
  EXPECT_EQ(sw_ntn_cfg.ta_report, true);
  EXPECT_FALSE(sw_ntn_cfg.polarization.has_value()) << "absent in both sat-switch and serving config stays absent";
}

namespace {

// ===== SIB19 PCAP generation =====
// These globals and helpers let the test emit a Wireshark-loadable MAC-NR PCAP holding fully populated SIB19
// messages. Pass `--enable_pcap` to write the file; without it the test still packs and round-trips every SIB19.
bool             g_enable_pcap = false;
ocudu::mac_pcap* g_pcap        = nullptr;

/// Builds a fully populated ntn_config. When \p use_orbital is true the ephemeris is expressed as ECI orbital
/// parameters, otherwise as an ECEF position/velocity state vector.
ntn_config make_full_ntn_config(bool use_orbital)
{
  ntn_config cfg;
  cfg.epoch_time.emplace();
  cfg.epoch_time->sfn             = 100;
  cfg.epoch_time->subframe_number = 5;
  cfg.ntn_ul_sync_validity_dur    = 60U;
  cfg.cell_specific_koffset.emplace(std::chrono::milliseconds(260));
  cfg.k_mac = 512U;

  cfg.ta_info.emplace();
  cfg.ta_info->ta_common               = 500.0;
  cfg.ta_info->ta_common_drift         = 0.5;
  cfg.ta_info->ta_common_drift_variant = 0.01;
  cfg.ta_info->ta_common_offset        = 50.0;

  cfg.polarization.emplace();
  cfg.polarization->dl = ntn_polarization_t::polarization_type::rhcp;
  cfg.polarization->ul = ntn_polarization_t::polarization_type::lhcp;

  if (use_orbital) {
    orbital_coordinates_t orb{};
    orb.semi_major_axis = 6900000.0;
    orb.eccentricity    = 0.001;
    orb.periapsis       = 1.0;
    orb.longitude       = 2.0;
    orb.mean_anomaly    = 3.0;
    orb.inclination     = 0.9;
    cfg.ephemeris_info  = orb;
  } else {
    ecef_coordinates_t ecef{};
    ecef.position_x    = 1300.0;
    ecef.position_y    = 2600.0;
    ecef.position_z    = 3900.0;
    ecef.velocity_vx   = 0.26;
    ecef.velocity_vy   = 0.52;
    ecef.velocity_vz   = 0.78;
    cfg.ephemeris_info = ecef;
  }

  cfg.ta_report = true;
  return cfg;
}

/// Builds a SIB19 with every field populated, varying the serving ephemeris format and the (possibly edge-case)
/// reference locations. The sat-switch target uses the opposite ephemeris format to exercise both encoders at once.
sib19_info make_full_sib19(bool serving_orbital, geodetic_coordinates_t ref_loc, geodetic_coordinates_t moving_loc)
{
  sib19_info sib19;
  sib19.ntn_cfg             = make_full_ntn_config(serving_orbital);
  sib19.t_service           = std::chrono::system_clock::time_point(std::chrono::milliseconds(1000000000));
  sib19.ref_location        = ref_loc;
  sib19.distance_thres      = 10000; // 10 km
  sib19.moving_ref_location = moving_loc;

  sib19.coverage_enhancements.emplace();
  sib19.coverage_enhancements->nof_msg4_harq_ack_rep    = 4;
  sib19.coverage_enhancements->rsrp_thres_msg4_harq_ack = 60;

  sib19.sat_switch_with_resync.emplace();
  sib19.sat_switch_with_resync->ntn_cfg = make_full_ntn_config(!serving_orbital);
  sib19.sat_switch_with_resync->t_service_start =
      std::chrono::system_clock::time_point(std::chrono::milliseconds(123456789));
  sib19.sat_switch_with_resync->ssb_time_offset_sf.emplace(50);

  // Two neighbor cells, one carrying its own explicit NTN config.
  neighbor_ntn_cell ncell0;
  ncell0.phys_cell_id = 100;
  ncell0.carrier_freq = arfcn_t{650000};
  ncell0.ntn_cfg      = make_full_ntn_config(serving_orbital);
  sib19.ncells.push_back(ncell0);

  neighbor_ntn_cell ncell1;
  ncell1.phys_cell_id = 200;
  ncell1.carrier_freq = arfcn_t{651000};
  sib19.ncells.push_back(ncell1);

  return sib19;
}

/// Packs a sib19_info into a BCCH-DL-SCH SystemInformation message, exactly as it is broadcast over the air.
byte_buffer pack_sib19_bcch_dl_sch(const sib19_info& sib19)
{
  asn1::rrc_nr::bcch_dl_sch_msg_s si_msg;
  asn1::rrc_nr::sys_info_ies_s&   si_ies = si_msg.msg.set_c1().set_sys_info().crit_exts.set_sys_info();
  si_ies.sib_type_and_info.resize(1);
  si_ies.sib_type_and_info[0].set_sib19_v1700() = odu::make_asn1_rrc_cell_sib19(sib19);

  byte_buffer         buf;
  asn1::bit_ref       bref{buf};
  asn1::OCUDUASN_CODE ret = si_msg.pack(bref);
  ocudu_assert(ret == asn1::OCUDUASN_SUCCESS, "Failed to pack BCCH-DL-SCH SIB19");
  return buf;
}

/// Writes an encoded BCCH-DL-SCH buffer to the PCAP under SI-RNTI at the given SFN/subframe.
void write_sib19_to_pcap(const byte_buffer& bcch_buf, unsigned sfn, unsigned subframe)
{
  if (!g_enable_pcap || g_pcap == nullptr) {
    return;
  }
  mac_nr_context_info context = {};
  context.radioType           = PCAP_FDD_RADIO;
  context.direction           = PCAP_DIRECTION_DOWNLINK;
  context.rntiType            = PCAP_SI_RNTI;
  context.rnti                = 0xffff; // SI-RNTI
  context.system_frame_number = static_cast<uint16_t>(sfn);
  context.sub_frame_number    = static_cast<uint8_t>(subframe);
  context.length              = static_cast<uint16_t>(bcch_buf.length());
  g_pcap->push_pdu(context, bcch_buf.deep_copy().value());
}

} // namespace

TEST(sib19_pcap_test, generate_full_sib19_pcap_with_edge_case_locations)
{
  struct sib19_variant {
    const char*            label;
    bool                   serving_orbital;
    geodetic_coordinates_t ref_location;
    geodetic_coordinates_t moving_ref_location;
  };

  // Edge-case reference locations: origin (equator/prime-meridian), both poles, both antimeridians, near-limit
  // coordinates and typical mid-latitude points. Each is emitted once with an ECEF state vector and once with ECI
  // orbital ephemeris, so both ephemeris encoders are covered.
  const std::array<sib19_variant, 8> variants = {{
      {"ecef_origin_0_0", false, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
      {"orbital_origin_0_0", true, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}},
      {"ecef_north_pole", false, {90.0, 0.0, 0.0}, {89.9999, 179.9999, 0.0}},
      {"orbital_south_pole", true, {-90.0, 0.0, 0.0}, {-89.9999, -179.9999, 0.0}},
      {"ecef_antimeridian_pos", false, {0.0, 180.0, 0.0}, {12.34, 179.5, 0.0}},
      {"orbital_antimeridian_neg", true, {0.0, -180.0, 0.0}, {-12.34, -179.5, 0.0}},
      {"ecef_mid_latitude", false, {37.7749, -122.4194, 0.0}, {-45.5, 12.3, 0.0}},
      {"orbital_mid_latitude", true, {48.135, 11.582, 0.0}, {51.5074, -0.1278, 0.0}},
  }};

  unsigned sfn = 0;
  for (const auto& v : variants) {
    SCOPED_TRACE(v.label);
    sib19_info  sib19    = make_full_sib19(v.serving_orbital, v.ref_location, v.moving_ref_location);
    byte_buffer bcch_buf = pack_sib19_bcch_dl_sch(sib19);
    ASSERT_FALSE(bcch_buf.empty());

    // Each message must round-trip through the ASN.1 decoder, i.e. be a valid BCCH-DL-SCH SystemInformation PDU.
    asn1::cbit_ref                  bref(bcch_buf);
    asn1::rrc_nr::bcch_dl_sch_msg_s decoded;
    ASSERT_EQ(decoded.unpack(bref), asn1::OCUDUASN_SUCCESS);
    ASSERT_TRUE(decoded.msg.c1().type() == asn1::rrc_nr::bcch_dl_sch_msg_type_c::c1_c_::types::sys_info);

    write_sib19_to_pcap(bcch_buf, sfn, /*subframe=*/5);
    sfn += 10;
  }

  if (g_enable_pcap && g_pcap != nullptr) {
    printf("\n=== %zu full SIB19 messages written to /tmp/ntn_sib19_helpers.pcap ===\n", variants.size());
    printf("Open in Wireshark (>= 4.6.3) and filter with: mac-nr.rnti == 0xffff\n");
  }
}

int main(int argc, char** argv)
{
  // Check for '--enable_pcap' cmd line argument; do not use getopt as it interferes with gtest.
  for (int i = 1; i < argc; ++i) {
    if (std::string(argv[i]) == "--enable_pcap") {
      g_enable_pcap = true;
    }
  }

  ocudulog::init();

  std::unique_ptr<task_worker>          pcap_worker;
  std::unique_ptr<task_worker_executor> pcap_exec;
  std::unique_ptr<mac_pcap>             pcap_writer;
  if (g_enable_pcap) {
    pcap_worker = std::make_unique<task_worker>("pcap_worker", 128);
    pcap_exec   = std::make_unique<task_worker_executor>(*pcap_worker);
    pcap_writer = create_mac_pcap("/tmp/ntn_sib19_helpers.pcap", mac_pcap_type::udp, *pcap_exec);
    g_pcap      = pcap_writer.get();
    printf("\n=== PCAP enabled: /tmp/ntn_sib19_helpers.pcap ===\n\n");
  }

  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();

  if (pcap_writer) {
    pcap_writer->close();
    pcap_writer.reset();
  }
  if (pcap_worker) {
    pcap_worker->wait_pending_tasks();
    pcap_worker->stop();
  }

  return result;
}
