// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/cu_cp/cu_cp_ntn_ref_time_store.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace std::chrono_literals;

namespace {

const nr_cell_identity ntn_nci_1 = nr_cell_identity::create(0x66c001).value();
const nr_cell_identity ntn_nci_2 = nr_cell_identity::create(0x66c002).value();
const nr_cell_identity tn_nci    = nr_cell_identity::create(0x66c0ff).value();

nr_cell_global_id_t make_cgi(nr_cell_identity nci)
{
  return {plmn_identity::test_value(), nci};
}

ocucp::cu_cp_ref_time_report make_report(uint32_t sfn, std::chrono::system_clock::time_point time)
{
  return {.ref_slot = slot_point{subcarrier_spacing::kHz15, sfn, 0}, .time = time, .is_local_clock = false};
}

class cu_cp_ntn_ref_time_store_test : public ::testing::Test
{
protected:
  const std::array<nr_cell_identity, 2> tracked_cells = {ntn_nci_1, ntn_nci_2};
  cu_cp_ntn_ref_time_store              store{tracked_cells};

  const std::chrono::system_clock::time_point t0{std::chrono::seconds{1'700'000'000}};
};

} // namespace

TEST_F(cu_cp_ntn_ref_time_store_test, tracked_cell_has_no_mapping_until_its_du_reports_one)
{
  ASSERT_FALSE(store.get_last_mapping(make_cgi(ntn_nci_1), subcarrier_spacing::kHz15).has_value());
}

TEST_F(cu_cp_ntn_ref_time_store_test, reported_mapping_is_returned_as_reported)
{
  const std::array<nr_cell_global_id_t, 1> served = {make_cgi(ntn_nci_1)};
  store.on_ref_time_info_report(served, make_report(100, t0));

  std::optional<ocudu_ntn::ntn_time_slot_mapping> mapping =
      store.get_last_mapping(make_cgi(ntn_nci_1), subcarrier_spacing::kHz15);
  ASSERT_TRUE(mapping.has_value());
  ASSERT_EQ(mapping->slot_tx, (slot_point{subcarrier_spacing::kHz15, 100, 0}));
  ASSERT_EQ(mapping->time_point, t0);
}

TEST_F(cu_cp_ntn_ref_time_store_test, report_applies_to_every_tracked_cell_served_by_the_du)
{
  const std::array<nr_cell_global_id_t, 2> served = {make_cgi(ntn_nci_1), make_cgi(ntn_nci_2)};
  store.on_ref_time_info_report(served, make_report(100, t0));

  for (const nr_cell_global_id_t& cgi : served) {
    std::optional<ocudu_ntn::ntn_time_slot_mapping> mapping = store.get_last_mapping(cgi, subcarrier_spacing::kHz15);
    ASSERT_TRUE(mapping.has_value());
    ASSERT_EQ(mapping->time_point, t0);
  }
}

TEST_F(cu_cp_ntn_ref_time_store_test, report_reaches_only_the_tracked_cells_the_du_serves)
{
  // The DU serves one of the two tracked NTN cells and a cell the store does not track.
  const std::array<nr_cell_global_id_t, 2> served = {make_cgi(ntn_nci_1), make_cgi(tn_nci)};
  store.on_ref_time_info_report(served, make_report(100, t0));

  ASSERT_TRUE(store.get_last_mapping(make_cgi(ntn_nci_1), subcarrier_spacing::kHz15).has_value());
  ASSERT_FALSE(store.get_last_mapping(make_cgi(ntn_nci_2), subcarrier_spacing::kHz15).has_value());
  ASSERT_FALSE(store.get_last_mapping(make_cgi(tn_nci), subcarrier_spacing::kHz15).has_value());
}

TEST_F(cu_cp_ntn_ref_time_store_test, latest_report_replaces_the_previous_one)
{
  const std::array<nr_cell_global_id_t, 1> served = {make_cgi(ntn_nci_1)};
  store.on_ref_time_info_report(served, make_report(100, t0));
  store.on_ref_time_info_report(served, make_report(200, t0 + 1s));

  std::optional<ocudu_ntn::ntn_time_slot_mapping> mapping =
      store.get_last_mapping(make_cgi(ntn_nci_1), subcarrier_spacing::kHz15);
  ASSERT_TRUE(mapping.has_value());
  ASSERT_EQ(mapping->slot_tx, (slot_point{subcarrier_spacing::kHz15, 200, 0}));
  ASSERT_EQ(mapping->time_point, t0 + 1s);
}
