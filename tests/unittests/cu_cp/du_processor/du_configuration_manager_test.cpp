// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/cu_cp/du_processor/du_configuration_manager.h"
#include "tests/ocudu_test_requirements.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

static cu_cp_served_cell_info create_basic_served_cell_info(unsigned du_counter)
{
  cu_cp_served_cell_info cell_info;
  cell_info.nr_cgi.plmn_id = plmn_identity::test_value();
  cell_info.nr_cgi.nci     = nr_cell_identity::create({411, 22}, du_counter).value();
  cell_info.five_gs_tac    = 7;
  cell_info.nr_pci         = du_counter;
  cell_info.served_plmns   = {plmn_identity::test_value()};
  return cell_info;
}

static du_setup_request create_basic_du_setup_request(unsigned du_counter = 0)
{
  du_setup_request req;
  req.gnb_du_id         = int_to_gnb_du_id(du_counter);
  req.gnb_du_name       = fmt::format("odu{}", du_counter);
  auto& cell            = req.gnb_du_served_cells_list.emplace_back();
  cell.served_cell_info = create_basic_served_cell_info(du_counter);
  cell.gnb_du_sys_info.emplace();
  cell.gnb_du_sys_info->mib_msg  = byte_buffer::create({0x0, 0x1, 0x2}).value();
  cell.gnb_du_sys_info->sib1_msg = byte_buffer::create({0x3, 0x4, 0x5}).value();
  return req;
}

class du_configuration_manager_test : public ::testing::Test
{
public:
  du_configuration_manager_test() : du_cfg_mng(gnb_id, plmns) {}

  gnb_id_t                   gnb_id{411, 22};
  std::vector<plmn_identity> plmns = {plmn_identity::test_value()};
  du_configuration_manager   du_cfg_mng;
};

TEST_F(du_configuration_manager_test, when_instance_created_then_it_has_no_dus)
{
  ASSERT_EQ(du_cfg_mng.nof_dus(), 0);
}

TEST_F(du_configuration_manager_test, when_du_config_handler_is_created_then_it_has_no_context)
{
  auto du_cfg_updater = du_cfg_mng.create_du_handler();
  ASSERT_NE(du_cfg_updater, nullptr);
  ASSERT_FALSE(du_cfg_updater->has_context());
}

TEST_F(du_configuration_manager_test, when_du_is_setup_successfully_then_context_is_updated)
{
  auto du_cfg_updater = du_cfg_mng.create_du_handler();
  auto setup_req      = create_basic_du_setup_request();
  auto ret            = du_cfg_updater->handle_new_du_config(setup_req);
  ASSERT_TRUE(ret.has_value()) << "DU setup failed: " << ret.error().cause_str;
  ASSERT_EQ(du_cfg_mng.nof_dus(), 1);
}

TEST_F(du_configuration_manager_test, when_du_cfg_handler_goes_out_of_scope_then_du_config_is_removed)
{
  {
    auto du_cfg_updater = du_cfg_mng.create_du_handler();
    auto setup_req      = create_basic_du_setup_request();
    auto ret            = du_cfg_updater->handle_new_du_config(setup_req);
    ASSERT_EQ(du_cfg_mng.nof_dus(), 1);
  }
  ASSERT_EQ(du_cfg_mng.nof_dus(), 0);
}

TEST_F(du_configuration_manager_test, when_two_dus_have_valid_configs_then_the_two_dus_are_added)
{
  auto setup_req1 = create_basic_du_setup_request(0);
  auto setup_req2 = create_basic_du_setup_request(1);

  auto du_cfg_updater = du_cfg_mng.create_du_handler();
  auto ret            = du_cfg_updater->handle_new_du_config(setup_req1);
  ASSERT_TRUE(ret.has_value());

  auto du_cfg_updater2 = du_cfg_mng.create_du_handler();
  ret                  = du_cfg_updater2->handle_new_du_config(setup_req2);
  ASSERT_TRUE(ret.has_value());

  ASSERT_EQ(du_cfg_mng.nof_dus(), 2);
}

TEST(du_configuration_manager_ntn_test, a_cell_broadcasting_a_single_tac_keeps_its_location_mapping)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2");

  // TS 38.300 sec. 16.14.3.1 leaves broadcasting several TACs optional, and TS 38.331 keeps the single TAC of such a
  // cell in trackingAreaCode, which leaves trackingAreaList empty. That cell still derives its own TAC, reported as
  // the single entry of the TAC List in NR NTN, and a Mapped Cell ID regardless of what it broadcasts, so the mapping
  // must survive.
  ntn_location_area area;
  area.tac        = 7;
  area.mapped_nci = nr_cell_identity::create(0x66c0f0).value();
  area.lat_min    = 50.0;
  area.lat_max    = 52.0;
  area.lon_min    = 14.0;
  area.lon_max    = 17.0;

  ntn_cell_location_mapping mapping;
  mapping.nci = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
  mapping.mapping.location_areas.push_back(area);

  du_configuration_manager du_cfg_mng{gnb_id_t{411, 22}, {plmn_identity::test_value()}, {mapping}};

  auto du_cfg_updater = du_cfg_mng.create_du_handler();
  auto setup_req      = create_basic_du_setup_request();
  auto ret            = du_cfg_updater->handle_new_du_config(setup_req);
  ASSERT_TRUE(ret.has_value()) << "DU setup failed: " << ret.error().cause_str;
  ASSERT_EQ(du_cfg_updater->get_context().served_cells.size(), 1);

  const du_cell_configuration& cell = du_cfg_updater->get_context().served_cells[0];
  ASSERT_TRUE(cell.tac_list.empty()) << "the cell must broadcast no trackingAreaList for this case";
  ASSERT_FALSE(cell.location_mapping.empty());
  ASSERT_EQ(cell.location_mapping.location_areas.size(), 1);
  EXPECT_EQ(cell.location_mapping.location_areas[0].tac, 7);
  // A Mapped Cell ID names a geographical area, TS 38.300 sec. 16.14.5, so it is reported whatever the cell
  // broadcasts.
  EXPECT_TRUE(cell.location_mapping.reports_mapped_cell_id(nr_cell_identity::create(0x66c0f0).value()));
}

TEST_F(du_configuration_manager_test, when_du_has_duplicate_du_id_then_setup_fails)
{
  auto setup_req1      = create_basic_du_setup_request(0);
  auto setup_req2      = create_basic_du_setup_request(1);
  setup_req2.gnb_du_id = setup_req1.gnb_du_id;

  auto du_cfg_updater = du_cfg_mng.create_du_handler();
  auto ret            = du_cfg_updater->handle_new_du_config(setup_req1);
  ASSERT_TRUE(ret.has_value());

  auto du_cfg_updater2 = du_cfg_mng.create_du_handler();
  ret                  = du_cfg_updater2->handle_new_du_config(setup_req2);
  ASSERT_FALSE(ret.has_value());

  ASSERT_EQ(du_cfg_mng.nof_dus(), 1);
  fmt::print("DU creation failed with error: {}\n", ret.error().cause_str);
}

TEST_F(du_configuration_manager_test, when_du_has_duplicate_nci_then_setup_fails)
{
  auto setup_req1 = create_basic_du_setup_request(0);
  auto setup_req2 = create_basic_du_setup_request(1);
  setup_req2.gnb_du_served_cells_list[0].served_cell_info.nr_cgi =
      setup_req1.gnb_du_served_cells_list[0].served_cell_info.nr_cgi;

  auto du_cfg_updater = du_cfg_mng.create_du_handler();
  auto ret            = du_cfg_updater->handle_new_du_config(setup_req1);
  ASSERT_TRUE(ret.has_value());

  auto du_cfg_updater2 = du_cfg_mng.create_du_handler();
  ret                  = du_cfg_updater2->handle_new_du_config(setup_req2);
  ASSERT_FALSE(ret.has_value());

  ASSERT_EQ(du_cfg_mng.nof_dus(), 1);
  fmt::print("DU creation failed with error: {}\n", ret.error().cause_str);
}

TEST_F(du_configuration_manager_test, when_du_has_different_plmn_then_setup_fails)
{
  auto setup_req                                                        = create_basic_du_setup_request();
  setup_req.gnb_du_served_cells_list[0].served_cell_info.nr_cgi.plmn_id = plmn_identity::parse("00102").value();

  auto du_cfg_updater = du_cfg_mng.create_du_handler();
  auto ret            = du_cfg_updater->handle_new_du_config(setup_req);
  ASSERT_FALSE(ret.has_value());

  ASSERT_EQ(du_cfg_mng.nof_dus(), 0);
  fmt::print("DU creation failed with error: {}\n", ret.error().cause_str);
}

/// Builds a served cell, optionally reporting \c mapped_nci as the Mapped Cell ID of its single area.
static du_cell_configuration create_cell(uint64_t nci, std::optional<uint64_t> mapped_nci = std::nullopt)
{
  du_cell_configuration cell;
  cell.cgi.plmn_id = plmn_identity::test_value();
  cell.cgi.nci     = nr_cell_identity::create(nci).value();
  cell.tac         = 7;
  if (mapped_nci.has_value()) {
    ntn_location_area area;
    area.tac        = 7;
    area.mapped_nci = nr_cell_identity::create(mapped_nci.value()).value();
    area.lat_min    = 50.0;
    area.lat_max    = 52.0;
    area.lon_min    = 14.0;
    area.lon_max    = 17.0;
    cell.location_mapping.location_areas.push_back(area);
  }
  return cell;
}

static nr_cell_global_id_t make_cgi(uint64_t nci)
{
  return {plmn_identity::test_value(), nr_cell_identity::create(nci).value()};
}

TEST(du_configuration_context_test, cell_is_found_by_its_own_cgi)
{
  du_configuration_context ctxt;
  ctxt.served_cells.push_back(create_cell(0x66c000, 0x66c001));

  const std::vector<const du_cell_configuration*> cells = ctxt.find_cells_by_reported_cgi(make_cgi(0x66c000));
  ASSERT_EQ(cells.size(), 1);
  EXPECT_EQ(cells[0]->cgi.nci.value(), 0x66c000);
}

TEST(du_configuration_context_test, cell_is_found_by_a_mapped_cell_id_it_reports)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2");

  du_configuration_context ctxt;
  ctxt.served_cells.push_back(create_cell(0x66c000, 0x66c001));

  // The core names the cell by the identity the gNB reported for it, TS 38.300 sec. 16.14.5.
  const std::vector<const du_cell_configuration*> cells = ctxt.find_cells_by_reported_cgi(make_cgi(0x66c001));
  ASSERT_EQ(cells.size(), 1);
  EXPECT_EQ(cells[0]->cgi.nci.value(), 0x66c000) << "the cell must be returned under its own identity";
}

TEST(du_configuration_context_test, a_cell_is_found_by_every_mapped_cell_id_its_areas_name)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2");

  // TS 38.300 sec. 16.14.5 NOTE 2 lets Mapped Cell IDs name different geographical areas, so the areas of one cell may
  // name several. A warning area naming any of them has to reach the cell.
  du_configuration_context ctxt;
  du_cell_configuration    cell = create_cell(0x66c000, 0x66c001);
  ntn_location_area        second_area;
  second_area.tac        = 8;
  second_area.mapped_nci = nr_cell_identity::create(0x66c002).value();
  second_area.lat_min    = 52.0;
  second_area.lat_max    = 54.0;
  second_area.lon_min    = 14.0;
  second_area.lon_max    = 17.0;
  cell.location_mapping.location_areas.push_back(second_area);
  ctxt.served_cells.push_back(cell);

  const std::vector<const du_cell_configuration*> first = ctxt.find_cells_by_reported_cgi(make_cgi(0x66c001));
  ASSERT_EQ(first.size(), 1);
  EXPECT_EQ(first[0]->cgi.nci.value(), 0x66c000);

  const std::vector<const du_cell_configuration*> second = ctxt.find_cells_by_reported_cgi(make_cgi(0x66c002));
  ASSERT_EQ(second.size(), 1);
  EXPECT_EQ(second[0]->cgi.nci.value(), 0x66c000) << "the second area names the cell just as the first does";
}

TEST(du_configuration_context_test, a_cell_without_a_mapping_is_found_by_its_own_cgi_alone)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2");

  du_configuration_context ctxt;
  ctxt.served_cells.push_back(create_cell(0x66c000));

  EXPECT_EQ(ctxt.find_cells_by_reported_cgi(make_cgi(0x66c000)).size(), 1);
  EXPECT_TRUE(ctxt.find_cells_by_reported_cgi(make_cgi(0x66c001)).empty());
}

TEST(du_configuration_context_test, a_mapped_cell_id_of_another_plmn_finds_no_cell)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2");

  du_configuration_context ctxt;
  ctxt.served_cells.push_back(create_cell(0x66c000, 0x66c001));

  const nr_cell_global_id_t foreign{plmn_identity::parse("00102").value(), nr_cell_identity::create(0x66c001).value()};
  EXPECT_TRUE(ctxt.find_cells_by_reported_cgi(foreign).empty());
}

TEST(du_configuration_context_test, an_unknown_identity_finds_no_cell)
{
  du_configuration_context ctxt;
  ctxt.served_cells.push_back(create_cell(0x66c000, 0x66c001));

  EXPECT_TRUE(ctxt.find_cells_by_reported_cgi(make_cgi(0x66c009)).empty());
}

TEST(du_configuration_context_test, every_cell_covering_a_mapped_cell_id_is_found)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2");

  // TS 38.300 sec. 16.14.5 leaves the mapping between a Mapped Cell ID and its geographical area to configuration, so
  // more than one cell may cover the area and report the same identity. A warning area naming it has to reach all of
  // them, not the first one alone.
  du_configuration_context ctxt;
  ctxt.served_cells.push_back(create_cell(0x66c000, 0x66c0ff));
  ctxt.served_cells.push_back(create_cell(0x66c001, 0x66c0ff));

  const std::vector<const du_cell_configuration*> cells = ctxt.find_cells_by_reported_cgi(make_cgi(0x66c0ff));
  ASSERT_EQ(cells.size(), 2);
  EXPECT_EQ(cells[0]->cgi.nci.value(), 0x66c000);
  EXPECT_EQ(cells[1]->cgi.nci.value(), 0x66c001);
}

TEST(du_configuration_context_test, a_mapped_cell_id_equal_to_the_uu_cell_id_of_another_cell_finds_both)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-LOC-2");

  // Nothing keeps a Mapped Cell ID clear of the Uu Cell ID of a served cell: it names an area agreed between RAN and
  // core, and TS 38.300 sec. 16.14.5 NOTE 3 even allows special values for it. Stopping at the cell whose Uu Cell ID
  // matches would leave the cell covering the area unserved.
  du_configuration_context ctxt;
  ctxt.served_cells.push_back(create_cell(0x66c000));
  ctxt.served_cells.push_back(create_cell(0x66c001, 0x66c000));

  const std::vector<const du_cell_configuration*> cells = ctxt.find_cells_by_reported_cgi(make_cgi(0x66c000));
  ASSERT_EQ(cells.size(), 2);
  EXPECT_EQ(cells[0]->cgi.nci.value(), 0x66c000) << "the cell the identity names directly";
  EXPECT_EQ(cells[1]->cgi.nci.value(), 0x66c001) << "the cell reporting it as its Mapped Cell ID";
}
