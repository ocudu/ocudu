// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_meas_manager_test_helpers.h"
#include "lib/cu_cp/cell_meas_manager/cell_meas_manager_helpers.h"
#include "tests/ocudu_test_requirements.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/support/enum_utils.h"
#include <variant>

using namespace ocudu;
using namespace ocucp;

TEST_F(cell_meas_manager_test, when_empty_cell_config_is_used_validation_fails)
{
  cell_meas_config cell_cfg;
  ASSERT_FALSE(is_complete(cell_cfg.serving_cell_cfg));
}

TEST_F(cell_meas_manager_test, when_valid_cell_config_is_used_validation_succeeds)
{
  cell_meas_config cell_cfg;
  cell_cfg.serving_cell_cfg.nci                 = nr_cell_identity::create(0x19b0).value();
  cell_cfg.serving_cell_cfg.gnb_id_bit_length   = 32;
  cell_cfg.serving_cell_cfg.pci                 = 1;
  cell_cfg.serving_cell_cfg.band.emplace()      = nr_band::n78;
  cell_cfg.serving_cell_cfg.ssb_arfcn.emplace() = 632628;
  cell_cfg.serving_cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;
  rrc_ssb_mtc ssb_mtc;
  ssb_mtc.dur                                 = 1;
  ssb_mtc.periodicity_and_offset.periodicity  = rrc_periodicity_and_offset::periodicity_t::sf5;
  ssb_mtc.periodicity_and_offset.offset       = 0;
  cell_cfg.serving_cell_cfg.ssb_mtc.emplace() = ssb_mtc;
  ASSERT_TRUE(is_complete(cell_cfg.serving_cell_cfg));
}

TEST_F(cell_meas_manager_test, when_empty_config_is_used_validation_succeeds)
{
  cell_meas_manager_config cfg = {};
  ASSERT_TRUE(is_valid_configuration(cfg));
}

TEST_F(cell_meas_manager_test, when_periodic_report_cfg_id_is_unknown_validation_fails)
{
  cell_meas_manager_config cfg;

  cell_meas_config cell_cfg;
  cell_cfg.serving_cell_cfg.nci               = nr_cell_identity::create(0x19b0).value();
  cell_cfg.serving_cell_cfg.gnb_id_bit_length = 32;
  cell_cfg.periodic_report_cfg_id             = uint_to_report_cfg_id(1);
  cfg.cells.emplace(cell_cfg.serving_cell_cfg.nci, cell_cfg);

  // Note: cfg.report_config_ids does not contain report_cfg_id 1.
  ASSERT_FALSE(is_valid_configuration(cfg));
}

TEST_F(cell_meas_manager_test, when_neighbor_report_cfg_id_is_unknown_validation_fails)
{
  cell_meas_manager_config cfg;

  nr_cell_identity nci1 = nr_cell_identity::create(0x19b0).value();
  nr_cell_identity nci2 = nr_cell_identity::create(0x19b1).value();

  cell_meas_config cell_cfg;
  cell_cfg.serving_cell_cfg.nci               = nci1;
  cell_cfg.serving_cell_cfg.gnb_id_bit_length = 32;

  neighbor_cell_meas_config ncell_meas_cfg;
  ncell_meas_cfg.nci = nci2;
  ncell_meas_cfg.report_cfg_ids.push_back(uint_to_report_cfg_id(2));
  cell_cfg.ncells.push_back(ncell_meas_cfg);

  cfg.cells.emplace(nci1, cell_cfg);

  // Note: cfg.report_config_ids does not contain report_cfg_id 2.
  ASSERT_FALSE(is_valid_configuration(cfg));
}

TEST_F(cell_meas_manager_test, when_empty_config_is_used_then_no_neighbor_cells_are_available)
{
  create_empty_manager();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  nr_cell_identity            nci      = nr_cell_identity::create(0x19b0).value();
  std::optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(ue_index, nci);

  // Make sure meas_cfg is empty.
  verify_empty_meas_cfg(meas_cfg);
}

TEST_F(cell_meas_manager_test, when_serving_cell_not_found_no_neighbor_cells_are_available)
{
  create_default_manager();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  nr_cell_identity            nci      = nr_cell_identity::create(0x19b5).value();
  std::optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(ue_index, nci);

  // Make sure meas_cfg is empty.
  verify_empty_meas_cfg(meas_cfg);
}

TEST_F(cell_meas_manager_test, when_serving_cell_found_then_neighbor_cells_are_available)
{
  create_default_manager();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  for (unsigned nci_val = 0x19b0; nci_val < 0x19b2; ++nci_val) {
    std::optional<rrc_meas_cfg> meas_cfg =
        manager->get_measurement_config(ue_index, nr_cell_identity::create(nci_val).value());
    check_default_meas_cfg(meas_cfg, meas_obj_id_t::min);
    verify_meas_cfg(meas_cfg);
  }
}

TEST_F(cell_meas_manager_test, when_inexisting_cell_config_is_updated_then_config_is_added)
{
  create_default_manager();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  const nr_cell_identity nci = nr_cell_identity::create(0x19b1).value();

  // get current config
  std::optional<cell_meas_config> cell_cfg = manager->get_cell_config(nci);
  ASSERT_TRUE(cell_cfg.has_value());

  // update config for cell 3
  auto& cell_cfg_val                                = cell_cfg.value();
  cell_cfg_val.serving_cell_cfg.gnb_id_bit_length   = 32;
  cell_cfg_val.serving_cell_cfg.nci                 = nr_cell_identity::create(0x19b3).value();
  cell_cfg_val.serving_cell_cfg.band.emplace()      = nr_band::n78;
  cell_cfg_val.serving_cell_cfg.ssb_arfcn.emplace() = 632628;
  cell_cfg_val.serving_cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;

  // Make sure meas_cfg is created.
  std::optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(ue_index, nci);
  check_default_meas_cfg(meas_cfg, meas_obj_id_t::min);
  verify_meas_cfg(meas_cfg);
}

TEST_F(cell_meas_manager_test, when_incomplete_cell_config_is_updated_then_valid_meas_config_is_created)
{
  create_default_manager();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  const nr_cell_identity nci = nr_cell_identity::create(0x19b1).value();

  // get current config
  std::optional<cell_meas_config> cell_cfg = manager->get_cell_config(nci);
  ASSERT_TRUE(cell_cfg.has_value());

  // update config for cell 1
  auto& cell_cfg_val                                = cell_cfg.value();
  cell_cfg_val.serving_cell_cfg.band.emplace()      = nr_band::n78;
  cell_cfg_val.serving_cell_cfg.ssb_arfcn.emplace() = 632628;
  cell_cfg_val.serving_cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;

  // Make sure meas_cfg is created.
  std::optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(ue_index, nci);
  check_default_meas_cfg(meas_cfg, meas_obj_id_t::min);
  verify_meas_cfg(meas_cfg);
}

TEST_F(cell_meas_manager_test, when_empty_cell_config_is_used_then_meas_cfg_is_not_set)
{
  // Create a manager without ncells and without report config.
  create_manager_without_ncells_and_periodic_report();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  nr_cell_identity            nci      = nr_cell_identity::create(0x19b0).value();
  std::optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(ue_index, nci);

  // Make sure meas_cfg is empty.
  verify_empty_meas_cfg(meas_cfg);
}

TEST_F(cell_meas_manager_test, when_old_meas_config_is_provided_reused_ids_are_not_removed)
{
  create_default_manager();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  const nr_cell_identity initial_nci = nr_cell_identity::create(0x19b0).value();

  // Make sure meas_cfg is created (no previous meas config provided)
  std::optional<rrc_meas_cfg> initial_meas_cfg = manager->get_measurement_config(ue_index, initial_nci);
  check_default_meas_cfg(initial_meas_cfg, meas_obj_id_t::min);
  verify_meas_cfg(initial_meas_cfg);

  const nr_cell_identity      target_nci      = nr_cell_identity::create(0x19b1).value();
  std::optional<rrc_meas_cfg> target_meas_cfg = manager->get_measurement_config(ue_index, target_nci, initial_meas_cfg);

  // Ids the new config sets up again are modified in place, so they must not be removed first.
  const auto& rem_meas_objs   = target_meas_cfg.value().meas_obj_to_rem_list;
  const auto& rem_report_cfgs = target_meas_cfg.value().report_cfg_to_rem_list;
  const auto& rem_meas_ids    = target_meas_cfg.value().meas_id_to_rem_list;
  for (const auto& meas_obj : target_meas_cfg.value().meas_obj_to_add_mod_list) {
    ASSERT_EQ(std::find(rem_meas_objs.begin(), rem_meas_objs.end(), meas_obj.meas_obj_id), rem_meas_objs.end())
        << "measObjectId " << to_underlying(meas_obj.meas_obj_id) << " is removed and added back";
  }
  for (const auto& report_cfg : target_meas_cfg.value().report_cfg_to_add_mod_list) {
    ASSERT_EQ(std::find(rem_report_cfgs.begin(), rem_report_cfgs.end(), report_cfg.report_cfg_id),
              rem_report_cfgs.end())
        << "reportConfigId " << to_underlying(report_cfg.report_cfg_id) << " is removed and added back";
  }
  for (const auto& meas_id : target_meas_cfg.value().meas_id_to_add_mod_list) {
    ASSERT_EQ(std::find(rem_meas_ids.begin(), rem_meas_ids.end(), meas_id.meas_id), rem_meas_ids.end())
        << "measId " << to_underlying(meas_id.meas_id) << " is removed and added back";
  }

  // Every id is reused here, so nothing is left to remove.
  ASSERT_TRUE(rem_meas_objs.empty());
  ASSERT_TRUE(rem_report_cfgs.empty());
  ASSERT_TRUE(rem_meas_ids.empty());

  // The new config should reuse the IDs again.
  check_default_meas_cfg(target_meas_cfg, meas_obj_id_t::min);
  verify_meas_cfg(target_meas_cfg);
}

TEST_F(cell_meas_manager_test, when_only_event_based_reports_configured_then_meas_objects_are_created)
{
  create_manager_with_incomplete_cells_and_periodic_report_at_target_cell();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  const nr_cell_identity initial_nci = nr_cell_identity::create(0x19b0).value();
  const nr_cell_identity target_nci  = nr_cell_identity::create(0x19b1).value();

  // Make sure no meas_cfg is created (incomplete cell config)
  ASSERT_FALSE(manager->get_measurement_config(ue_index, initial_nci).has_value());
  ASSERT_FALSE(manager->get_measurement_config(ue_index, target_nci).has_value());

  serving_cell_meas_config serving_cell_cfg;
  serving_cell_cfg.gnb_id_bit_length   = 32;
  serving_cell_cfg.nci                 = initial_nci;
  serving_cell_cfg.pci                 = 1;
  serving_cell_cfg.band.emplace()      = nr_band::n78;
  serving_cell_cfg.ssb_arfcn.emplace() = 632628;
  serving_cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;
  {
    rrc_ssb_mtc ssb_mtc;
    ssb_mtc.dur                                = 1;
    ssb_mtc.periodicity_and_offset.periodicity = rrc_periodicity_and_offset::periodicity_t::sf5;
    ssb_mtc.periodicity_and_offset.offset      = 0;
    serving_cell_cfg.ssb_mtc.emplace()         = ssb_mtc;
  }

  // Update cell config for cell 1
  ASSERT_TRUE(manager->update_cell_config(initial_nci, serving_cell_cfg));

  // Update cell config for cell 2
  serving_cell_cfg.nci = target_nci;
  ASSERT_TRUE(manager->update_cell_config(target_nci, serving_cell_cfg));

  // Make sure meas_cfg is created and contains measurement objects to add mod
  std::optional<rrc_meas_cfg> initial_meas_cfg = manager->get_measurement_config(ue_index, initial_nci);
  ASSERT_TRUE(initial_meas_cfg.has_value());
  ASSERT_EQ(initial_meas_cfg.value().meas_obj_to_add_mod_list.size(), 1);
  ASSERT_TRUE(initial_meas_cfg.value().meas_obj_to_add_mod_list.begin()->meas_obj_nr.has_value());
  ASSERT_EQ(initial_meas_cfg.value().meas_obj_to_add_mod_list.begin()->meas_obj_nr.value().ssb_freq,
            serving_cell_cfg.ssb_arfcn);
  ASSERT_EQ(initial_meas_cfg.value().report_cfg_to_add_mod_list.size(), 1);

  std::optional<rrc_meas_cfg> target_meas_cfg = manager->get_measurement_config(ue_index, target_nci, initial_meas_cfg);
  ASSERT_TRUE(target_meas_cfg.has_value());
  ASSERT_EQ(target_meas_cfg.value().meas_obj_to_add_mod_list.size(), 1);
  ASSERT_TRUE(target_meas_cfg.value().meas_obj_to_add_mod_list.begin()->meas_obj_nr.has_value());
  ASSERT_EQ(target_meas_cfg.value().meas_obj_to_add_mod_list.begin()->meas_obj_nr.value().ssb_freq,
            serving_cell_cfg.ssb_arfcn);
  ASSERT_EQ(target_meas_cfg.value().report_cfg_to_add_mod_list.size(), 2);
}

TEST_F(cell_meas_manager_test, when_serving_cell_has_no_periodic_report_then_serving_meas_obj_is_still_generated)
{
  // Inter-frequency setup: serving cell (632628) without periodic report, neighbor (633000) with A3 report.
  create_manager_inter_freq_without_periodic_report();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  const nr_cell_identity      serving_nci = nr_cell_identity::create(gnb_id_t{0x19b, 32}, 0).value();
  std::optional<rrc_meas_cfg> meas_cfg    = manager->get_measurement_config(ue_index, serving_nci);
  ASSERT_TRUE(meas_cfg.has_value());

  // Both serving and neighbor frequencies must have a measurement object, even though only the neighbor has a report.
  ASSERT_EQ(meas_cfg.value().meas_obj_to_add_mod_list.size(), 2);
  auto has_ssb_freq = [&](uint32_t arfcn) {
    return std::any_of(meas_cfg.value().meas_obj_to_add_mod_list.begin(),
                       meas_cfg.value().meas_obj_to_add_mod_list.end(),
                       [arfcn](const rrc_meas_obj_to_add_mod& mo) {
                         return mo.meas_obj_nr.has_value() && mo.meas_obj_nr->ssb_freq == arfcn;
                       });
  };
  ASSERT_TRUE(has_ssb_freq(632628)) << "Serving cell measurement object must be present for servingCellMO reference";
  ASSERT_TRUE(has_ssb_freq(633000)) << "Neighbor cell measurement object missing";

  // Only the neighbor's A3 report (and its meas id) is configured: the serving cell MO carries no report on its own.
  ASSERT_EQ(meas_cfg.value().report_cfg_to_add_mod_list.size(), 1);
  ASSERT_EQ(meas_cfg.value().meas_id_to_add_mod_list.size(), 1);
  verify_meas_cfg(meas_cfg);
}

TEST_F(cell_meas_manager_test, when_invalid_cell_config_update_received_then_config_is_not_updated)
{
  create_manager_with_incomplete_cells_and_periodic_report_at_target_cell();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  const nr_cell_identity initial_nci = nr_cell_identity::create(0x19b0).value();
  const nr_cell_identity target_nci  = nr_cell_identity::create(0x19b1).value();

  // Make sure no meas_cfg is created (incomplete cell config)
  ASSERT_FALSE(manager->get_measurement_config(ue_index, initial_nci).has_value());
  ASSERT_FALSE(manager->get_measurement_config(ue_index, target_nci).has_value());

  serving_cell_meas_config serving_cell_cfg;
  serving_cell_cfg.gnb_id_bit_length = 32;
  serving_cell_cfg.nci               = initial_nci;
  serving_cell_cfg.pci               = 1;
  serving_cell_cfg.band              = nr_band::n78;
  serving_cell_cfg.ssb_arfcn         = 632628;
  serving_cell_cfg.ssb_scs           = subcarrier_spacing::kHz30;
  {
    rrc_ssb_mtc ssb_mtc;
    ssb_mtc.dur                                = 1;
    ssb_mtc.periodicity_and_offset.periodicity = rrc_periodicity_and_offset::periodicity_t::sf5;
    ssb_mtc.periodicity_and_offset.offset      = 0;
    serving_cell_cfg.ssb_mtc                   = ssb_mtc;
  }

  // Update cell config for cell 1
  ASSERT_TRUE(manager->update_cell_config(initial_nci, serving_cell_cfg));

  // Update cell config for cell 2 with different scs for same ssb_freq
  serving_cell_cfg.nci     = target_nci;
  serving_cell_cfg.ssb_scs = subcarrier_spacing::kHz15;

  ASSERT_FALSE(manager->update_cell_config(target_nci, serving_cell_cfg));

  // Make sure meas_cfg for cell 1 only contains the serving cell measurement object
  std::optional<rrc_meas_cfg> initial_meas_cfg = manager->get_measurement_config(ue_index, initial_nci);
  ASSERT_TRUE(initial_meas_cfg.has_value());
  ASSERT_EQ(initial_meas_cfg.value().meas_obj_to_add_mod_list.size(), 1);
  ASSERT_TRUE(initial_meas_cfg.value().meas_obj_to_add_mod_list.begin()->meas_obj_nr.has_value());
  ASSERT_EQ(initial_meas_cfg.value().meas_obj_to_add_mod_list.begin()->meas_obj_nr.value().ssb_freq,
            serving_cell_cfg.ssb_arfcn);
  ASSERT_TRUE(initial_meas_cfg.value().report_cfg_to_add_mod_list.empty());

  // The target cell's config is incomplete, so nothing can be measured from it: the UE's current
  // measurements are removed instead of being silently kept.
  std::optional<rrc_meas_cfg> target_meas_cfg = manager->get_measurement_config(ue_index, target_nci, initial_meas_cfg);
  ASSERT_TRUE(target_meas_cfg.has_value());
  ASSERT_TRUE(target_meas_cfg.value().meas_obj_to_add_mod_list.empty());
  ASSERT_FALSE(target_meas_cfg.value().meas_obj_to_rem_list.empty());
}

TEST_F(cell_meas_manager_test, when_t312_is_configured_then_meas_obj_has_t312_and_report_cfg_has_t312)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-14");

  create_default_manager(100);

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  nr_cell_identity nci = nr_cell_identity::create(0x19b0).value();

  std::optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(ue_index, nci);
  ASSERT_TRUE(meas_cfg.has_value());
  verify_meas_cfg(meas_cfg);

  // Find the event-triggered report config.
  const auto report_it = std::find_if(
      meas_cfg.value().report_cfg_to_add_mod_list.begin(),
      meas_cfg.value().report_cfg_to_add_mod_list.end(),
      [](const rrc_report_cfg_to_add_mod& r) { return std::get_if<rrc_event_trigger_cfg>(&r.report_cfg) != nullptr; });
  ASSERT_NE(report_it, meas_cfg.value().report_cfg_to_add_mod_list.end());

  // Verify report config carries t312.
  const auto* event_triggered = std::get_if<rrc_event_trigger_cfg>(&report_it->report_cfg);
  ASSERT_NE(event_triggered, nullptr);
  ASSERT_TRUE(event_triggered->t312.has_value());
  ASSERT_EQ(event_triggered->t312.value(), 100);

  // Find the meas_id that links the event-triggered report to a meas object.
  const auto meas_id_it =
      std::find_if(meas_cfg.value().meas_id_to_add_mod_list.begin(),
                   meas_cfg.value().meas_id_to_add_mod_list.end(),
                   [&](const rrc_meas_id_to_add_mod& m) { return m.report_cfg_id == report_it->report_cfg_id; });
  ASSERT_NE(meas_id_it, meas_cfg.value().meas_id_to_add_mod_list.end());

  // Find the linked measurement object and verify t312 is propagated.
  const auto meas_obj_it =
      std::find_if(meas_cfg.value().meas_obj_to_add_mod_list.begin(),
                   meas_cfg.value().meas_obj_to_add_mod_list.end(),
                   [&](const rrc_meas_obj_to_add_mod& obj) { return obj.meas_obj_id == meas_id_it->meas_obj_id; });
  ASSERT_NE(meas_obj_it, meas_cfg.value().meas_obj_to_add_mod_list.end());
  ASSERT_TRUE(meas_obj_it->meas_obj_nr.has_value());
  ASSERT_TRUE(meas_obj_it->meas_obj_nr.value().t312.has_value());
  ASSERT_EQ(meas_obj_it->meas_obj_nr.value().t312.value(), 100);
}

// ===================== CHO Measurement Config Tests =====================

TEST_F(cell_meas_manager_test, cho_single_frequency_generates_correct_nci_to_meas_id_mapping)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-15");

  create_cho_manager_single_frequency();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  attach_rrc_ue(ue_index);

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_serving = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci_target1 = nr_cell_identity::create(gnb_id, 1).value();
  nr_cell_identity nci_target2 = nr_cell_identity::create(gnb_id, 2).value();

  // Get CHO measurement config for both target candidates
  std::vector<pci_t> candidate_pcis = {2, 3}; // PCIs of target cells
  auto cho_result = manager->get_measurement_config(ue_index, nci_serving, std::nullopt, true, candidate_pcis);

  ASSERT_TRUE(cho_result.has_value());
  ASSERT_EQ(cho_result->meas_obj_to_add_mod_list.size(), 1);
  ASSERT_EQ(cho_result->meas_id_to_add_mod_list.size(), 1);

  // Verify NCI-to-measId mapping was generated
  ASSERT_FALSE(cho_result->nci_to_meas_ids.empty());

  // Single frequency: all targets should map to same frequency's measIds
  // Both target cells should have entries in the mapping
  ASSERT_TRUE(cho_result->nci_to_meas_ids.find(nci_target1) != cho_result->nci_to_meas_ids.end() &&
              cho_result->nci_to_meas_ids.find(nci_target2) != cho_result->nci_to_meas_ids.end());
}

TEST_F(cell_meas_manager_test, cho_multi_frequency_generates_separate_meas_ids_per_nci)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-15");

  create_cho_manager_multi_frequency();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  attach_rrc_ue(ue_index);

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_serving = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci_target1 = nr_cell_identity::create(gnb_id, 1).value();
  nr_cell_identity nci_target2 = nr_cell_identity::create(gnb_id, 2).value();

  // Get CHO measurement config for both target candidates on different frequencies
  std::vector<pci_t> candidate_pcis = {2, 3};
  auto cho_result = manager->get_measurement_config(ue_index, nci_serving, std::nullopt, true, candidate_pcis);

  ASSERT_TRUE(cho_result.has_value());

  // Multi-frequency: 2 target frequencies + 1 serving cell frequency = 3 measurement objects.
  ASSERT_EQ(cho_result->meas_obj_to_add_mod_list.size(), 3);

  // Verify NCI-to-measId mapping contains both target NCIs
  ASSERT_TRUE(cho_result->nci_to_meas_ids.find(nci_target1) != cho_result->nci_to_meas_ids.end() &&
              cho_result->nci_to_meas_ids.find(nci_target2) != cho_result->nci_to_meas_ids.end());

  // Verify each NCI has its own distinct measIds (since they're on different frequencies)
  if (cho_result->nci_to_meas_ids.size() >= 2) {
    auto it1 = cho_result->nci_to_meas_ids.begin();
    auto it2 = std::next(it1);

    // Each target should have at least 1 measId
    ASSERT_FALSE(it1->second.empty());
    ASSERT_FALSE(it2->second.empty());
  }
}

TEST_F(cell_meas_manager_test, cho_multi_trigger_creates_cross_product_meas_ids)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-15");

  create_cho_manager_multi_trigger();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  attach_rrc_ue(ue_index);

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_serving = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci_target  = nr_cell_identity::create(gnb_id, 1).value();

  // Get CHO measurement config with multiple conditional triggers
  std::vector<pci_t> candidate_pcis = {2};
  auto cho_result = manager->get_measurement_config(ue_index, nci_serving, std::nullopt, true, candidate_pcis);

  ASSERT_TRUE(cho_result.has_value());

  // Multi-trigger: should create cross-product of MOs × triggers
  // 1 frequency × 2 triggers = 2 measIds for the target
  ASSERT_GE(cho_result->meas_id_to_add_mod_list.size(), 2);

  // Verify target NCI is in the mapping
  ASSERT_TRUE(cho_result->nci_to_meas_ids.find(nci_target) != cho_result->nci_to_meas_ids.end());

  // The target should have 2 measIds (one for each conditional trigger)
  if (cho_result->nci_to_meas_ids.find(nci_target) != cho_result->nci_to_meas_ids.end()) {
    const auto& meas_ids = cho_result->nci_to_meas_ids.at(nci_target);
    ASSERT_GE(meas_ids.size(), 2) << "Expected at least 2 measIds for target with 2 conditional triggers";
  }
}

TEST_F(cell_meas_manager_test, cho_empty_candidate_list_includes_all_neighbors)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-15");

  create_cho_manager_single_frequency();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  attach_rrc_ue(ue_index);

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_serving = nr_cell_identity::create(gnb_id, 0).value();

  // Empty candidate list means no PCI filter — all configured neighbors are included.
  std::vector<pci_t> candidate_pcis;
  auto cho_result = manager->get_measurement_config(ue_index, nci_serving, std::nullopt, true, candidate_pcis);

  ASSERT_TRUE(cho_result.has_value());
}

TEST_F(cell_meas_manager_test, cho_invalid_candidate_pci_filters_correctly)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-15");

  create_cho_manager_single_frequency();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_serving = nr_cell_identity::create(gnb_id, 0).value();

  // Request CHO config with invalid/unknown PCI
  std::vector<pci_t> candidate_pcis = {999}; // Non-existent PCI
  auto cho_result = manager->get_measurement_config(ue_index, nci_serving, std::nullopt, true, candidate_pcis);

  // Should return nullopt when no valid candidates exist
  ASSERT_FALSE(cho_result.has_value());
}

TEST_F(cell_meas_manager_test, cho_a5_inter_frequency_includes_serving_cell_meas_obj)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-15");

  create_cho_manager_a5_inter_frequency();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  attach_rrc_ue(ue_index);

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_serving = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci_target  = nr_cell_identity::create(gnb_id, 1).value();

  std::vector<pci_t> candidate_pcis = {2};
  auto cho_result = manager->get_measurement_config(ue_index, nci_serving, std::nullopt, true, candidate_pcis);

  ASSERT_TRUE(cho_result.has_value());

  // Serving (632628) and target (633000) are on different frequencies: both must have measurement objects.
  ASSERT_EQ(cho_result->meas_obj_to_add_mod_list.size(), 2);

  auto has_ssb_freq = [&](uint32_t arfcn) {
    return std::any_of(cho_result->meas_obj_to_add_mod_list.begin(),
                       cho_result->meas_obj_to_add_mod_list.end(),
                       [arfcn](const rrc_meas_obj_to_add_mod& mo) {
                         return mo.meas_obj_nr.has_value() && mo.meas_obj_nr->ssb_freq.has_value() &&
                                mo.meas_obj_nr->ssb_freq.value() == arfcn;
                       });
  };
  ASSERT_TRUE(has_ssb_freq(632628)) << "Serving cell measurement object must be present for A5 threshold1 evaluation";
  ASSERT_TRUE(has_ssb_freq(633000)) << "Target cell measurement object missing";

  // Target NCI must have measurement IDs for condExecutionCond assignment.
  ASSERT_NE(cho_result->nci_to_meas_ids.find(nci_target), cho_result->nci_to_meas_ids.end());
  ASSERT_FALSE(cho_result->nci_to_meas_ids.at(nci_target).empty());

  // Serving NCI must not appear as a CHO candidate target.
  ASSERT_EQ(cho_result->nci_to_meas_ids.find(nci_serving), cho_result->nci_to_meas_ids.end());
}

// ===================== NTN Neighbour Cell Info Tests =====================

static rrc_ntn_neighbour_cell_info make_test_ntn_neighbour_info()
{
  rrc_ntn_neighbour_cell_info info;
  info.epoch_time.sfn             = 100;
  info.epoch_time.subframe_number = 5;

  ecef_coordinates_t ecef;
  ecef.position_x  = -3621225.25;
  ecef.position_y  = -5839350.24;
  ecef.position_z  = 101120.52;
  ecef.velocity_vx = 3498.87;
  ecef.velocity_vy = -2055.89;
  ecef.velocity_vz = 6104.62;
  info.ephemeris   = ecef;

  info.ref_location = reference_location{12.3, 45.6};
  return info;
}

TEST_F(cell_meas_manager_test, when_no_ntn_neighbour_info_then_meas_config_has_no_cells_to_add_mod)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-1");

  create_default_manager();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci1 = nr_cell_identity::create(gnb_id, 0).value();

  std::optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(ue_index, nci1);
  ASSERT_TRUE(meas_cfg.has_value());
  for (const auto& meas_obj : meas_cfg->meas_obj_to_add_mod_list) {
    ASSERT_TRUE(meas_obj.meas_obj_nr.has_value());
    EXPECT_TRUE(meas_obj.meas_obj_nr->cells_to_add_mod_list.empty());
  }
}

TEST_F(cell_meas_manager_test, when_ntn_neighbour_info_updated_then_meas_config_contains_it)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-1");

  create_default_manager();

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci1 = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci2 = nr_cell_identity::create(gnb_id, 1).value();

  std::vector<rrc_ntn_neighbour_cell_info_item> items = {{nci2, make_test_ntn_neighbour_info()}};
  ASSERT_TRUE(manager->update_ntn_neighbour_info(nci1, items));

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  std::optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(ue_index, nci1);
  ASSERT_TRUE(meas_cfg.has_value());

  // The neighbour cell must be added to cells_to_add_mod_list with the NTN info attached.
  unsigned nof_ntn_cells = 0;
  for (const auto& meas_obj : meas_cfg->meas_obj_to_add_mod_list) {
    ASSERT_TRUE(meas_obj.meas_obj_nr.has_value());
    for (const auto& cell : meas_obj.meas_obj_nr->cells_to_add_mod_list) {
      ASSERT_TRUE(cell.ntn_neighbour_info.has_value());
      EXPECT_EQ(cell.ntn_neighbour_info->epoch_time.sfn, 100U);
      EXPECT_EQ(cell.ntn_neighbour_info->epoch_time.subframe_number, 5U);
      EXPECT_TRUE(std::holds_alternative<ecef_coordinates_t>(cell.ntn_neighbour_info->ephemeris));
      ASSERT_TRUE(cell.ntn_neighbour_info->ref_location.has_value());
      EXPECT_DOUBLE_EQ(cell.ntn_neighbour_info->ref_location->latitude, 12.3);
      EXPECT_DOUBLE_EQ(cell.ntn_neighbour_info->ref_location->longitude, 45.6);
      ++nof_ntn_cells;
    }
  }
  EXPECT_EQ(nof_ntn_cells, 1U);
}

TEST_F(cell_meas_manager_test, when_ntn_update_refers_to_unknown_serving_cell_then_update_fails)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-1");

  create_default_manager();

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity unknown_nci = nr_cell_identity::create(gnb_id, 5).value();
  nr_cell_identity nci2        = nr_cell_identity::create(gnb_id, 1).value();

  std::vector<rrc_ntn_neighbour_cell_info_item> items = {{nci2, make_test_ntn_neighbour_info()}};
  ASSERT_FALSE(manager->update_ntn_neighbour_info(unknown_nci, items));
}

TEST_F(cell_meas_manager_test, when_ntn_update_refers_to_unknown_neighbour_then_update_fails)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-1");

  create_default_manager();

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci1        = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity unknown_nci = nr_cell_identity::create(gnb_id, 5).value();

  std::vector<rrc_ntn_neighbour_cell_info_item> items = {{unknown_nci, make_test_ntn_neighbour_info()}};
  ASSERT_FALSE(manager->update_ntn_neighbour_info(nci1, items));
}

TEST_F(cell_meas_manager_test, when_cho_meas_config_requested_then_ntn_neighbour_info_is_included)
{
  OCUDU_TEST_REQUIREMENTS("CU-NTN-MOB-1", "MVP-FUNC-MOB-15");

  create_cho_manager_single_frequency();

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_serving = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci_target1 = nr_cell_identity::create(gnb_id, 1).value();

  std::vector<rrc_ntn_neighbour_cell_info_item> items = {{nci_target1, make_test_ntn_neighbour_info()}};
  ASSERT_TRUE(manager->update_ntn_neighbour_info(nci_serving, items));

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  attach_rrc_ue(ue_index);

  std::vector<pci_t> candidate_pcis = {2, 3}; // PCIs of target cells 1 and 2
  auto cho_result = manager->get_measurement_config(ue_index, nci_serving, std::nullopt, true, candidate_pcis);
  ASSERT_TRUE(cho_result.has_value());

  // Target 1 (pci=2) carries the NTN info, target 2 (pci=3) does not.
  unsigned nof_cells = 0;
  for (const auto& meas_obj : cho_result->meas_obj_to_add_mod_list) {
    ASSERT_TRUE(meas_obj.meas_obj_nr.has_value());
    for (const auto& cell : meas_obj.meas_obj_nr->cells_to_add_mod_list) {
      if (cell.pci == 2) {
        ASSERT_TRUE(cell.ntn_neighbour_info.has_value());
        EXPECT_EQ(cell.ntn_neighbour_info->epoch_time.sfn, 100U);
      } else {
        EXPECT_FALSE(cell.ntn_neighbour_info.has_value());
      }
      ++nof_cells;
    }
  }
  EXPECT_GT(nof_cells, 0U);
}

TEST_F(cell_meas_manager_test, when_reused_meas_object_drops_a_cell_then_the_cell_is_removed)
{
  // Old config: measurement object 1 covers pci=5.
  rrc_meas_cfg            old_cfg;
  rrc_meas_obj_to_add_mod old_obj;
  old_obj.meas_obj_id = uint_to_meas_obj_id(1);
  old_obj.meas_obj_nr.emplace();
  rrc_cells_to_add_mod stale_cell;
  stale_cell.pci = 5;
  old_obj.meas_obj_nr.value().cells_to_add_mod_list.push_back(stale_cell);
  old_cfg.meas_obj_to_add_mod_list.push_back(old_obj);

  // New config: same measurement object, without that cell.
  rrc_meas_cfg            new_cfg;
  rrc_meas_obj_to_add_mod new_obj;
  new_obj.meas_obj_id = uint_to_meas_obj_id(1);
  new_obj.meas_obj_nr.emplace();
  new_cfg.meas_obj_to_add_mod_list.push_back(new_obj);
  add_old_meas_config_to_rem_list(old_cfg, new_cfg);

  prune_redundant_rem_list_entries(old_cfg, new_cfg);

  ASSERT_TRUE(new_cfg.meas_obj_to_rem_list.empty());
  const auto& cells_to_rem = new_cfg.meas_obj_to_add_mod_list.at(0).meas_obj_nr.value().cells_to_rem_list;
  ASSERT_EQ(cells_to_rem.size(), 1);
  ASSERT_EQ(cells_to_rem.at(0), 5);
}

TEST_F(cell_meas_manager_test, when_new_meas_config_is_smaller_then_the_dropped_ids_are_removed)
{
  // Old config: two of everything.
  rrc_meas_cfg old_cfg;
  for (uint8_t id : {1, 2}) {
    rrc_meas_obj_to_add_mod meas_obj;
    meas_obj.meas_obj_id = uint_to_meas_obj_id(id);
    meas_obj.meas_obj_nr.emplace();
    old_cfg.meas_obj_to_add_mod_list.push_back(meas_obj);
    old_cfg.report_cfg_to_add_mod_list.push_back({uint_to_report_cfg_id(id), {}});
    old_cfg.meas_id_to_add_mod_list.push_back(
        {uint_to_meas_id(id), uint_to_meas_obj_id(id), uint_to_report_cfg_id(id)});
  }

  // New config: only the first of each is set up again.
  rrc_meas_cfg            new_cfg;
  rrc_meas_obj_to_add_mod meas_obj;
  meas_obj.meas_obj_id = uint_to_meas_obj_id(1);
  meas_obj.meas_obj_nr.emplace();
  new_cfg.meas_obj_to_add_mod_list.push_back(meas_obj);
  new_cfg.report_cfg_to_add_mod_list.push_back({uint_to_report_cfg_id(1), {}});
  new_cfg.meas_id_to_add_mod_list.push_back({uint_to_meas_id(1), uint_to_meas_obj_id(1), uint_to_report_cfg_id(1)});
  add_old_meas_config_to_rem_list(old_cfg, new_cfg);

  prune_redundant_rem_list_entries(old_cfg, new_cfg);

  // The ids that are gone must still be removed, the reused ones must not.
  ASSERT_EQ(new_cfg.meas_obj_to_rem_list, std::vector<meas_obj_id_t>{uint_to_meas_obj_id(2)});
  ASSERT_EQ(new_cfg.report_cfg_to_rem_list, std::vector<report_cfg_id_t>{uint_to_report_cfg_id(2)});
  ASSERT_EQ(new_cfg.meas_id_to_rem_list, std::vector<meas_id_t>{uint_to_meas_id(2)});
}

TEST_F(cell_meas_manager_test, when_measurement_report_references_unknown_cell_then_report_is_ignored)
{
  create_default_manager();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  // Forge a measurement context referencing a cell absent from the configuration, as left behind by a
  // configuration change racing an in-flight measurement report.
  meas_context_t stale_ctxt;
  stale_ctxt.report_cfg_id     = uint_to_report_cfg_id(2);
  stale_ctxt.gnb_id_bit_length = 32;
  stale_ctxt.nci               = nr_cell_identity::create(gnb_id_t{0x19b, 32}, 5).value();
  stale_ctxt.pci               = 1;

  auto&     ue_meas_ctxt  = ue_mng.get_measurement_context(ue_index);
  meas_id_t stale_meas_id = ue_meas_ctxt.allocate_meas_id();
  ue_meas_ctxt.meas_id_to_meas_context.emplace(stale_meas_id, stale_ctxt);

  rrc_meas_results results;
  results.meas_id = stale_meas_id;

  // The report must be dropped without notifying the mobility manager (and without terminating the process).
  manager->report_measurement(ue_index, results);
  EXPECT_EQ(mobility_manager.nof_notifications, 0U);
}

TEST_F(cell_meas_manager_test, when_neighbor_relation_references_unknown_cell_then_it_is_skipped)
{
  // Build a config whose serving cell lists a neighbor with no cell entry of its own (a dangling relation,
  // which construction-time validation does not reject).
  cell_meas_manager_config cfg;
  gnb_id_t                 gnb_id{0x19b, 32};
  nr_cell_identity         serving_nci = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity         unknown_nci = nr_cell_identity::create(gnb_id, 9).value();

  cell_meas_config cell_cfg;
  cell_cfg.serving_cell_cfg.gnb_id_bit_length   = gnb_id.bit_length;
  cell_cfg.serving_cell_cfg.nci                 = serving_nci;
  cell_cfg.serving_cell_cfg.pci                 = 1;
  cell_cfg.periodic_report_cfg_id               = uint_to_report_cfg_id(1);
  cell_cfg.serving_cell_cfg.band.emplace()      = nr_band::n78;
  cell_cfg.serving_cell_cfg.ssb_arfcn.emplace() = 632628;
  cell_cfg.serving_cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;
  {
    rrc_ssb_mtc ssb_mtc;
    ssb_mtc.dur                                 = 1;
    ssb_mtc.periodicity_and_offset.periodicity  = rrc_periodicity_and_offset::periodicity_t::sf5;
    ssb_mtc.periodicity_and_offset.offset       = 0;
    cell_cfg.serving_cell_cfg.ssb_mtc.emplace() = ssb_mtc;
  }
  neighbor_cell_meas_config dangling_ncell;
  dangling_ncell.nci = unknown_nci;
  dangling_ncell.report_cfg_ids.push_back(uint_to_report_cfg_id(1));
  cell_cfg.ncells.push_back(dangling_ncell);
  cfg.cells.emplace(serving_nci, cell_cfg);

  rrc_periodical_report_cfg periodical_cfg;
  periodical_cfg.rs_type                = ocucp::rrc_nr_rs_type::ssb;
  periodical_cfg.report_interv          = 1024;
  periodical_cfg.report_amount          = -1;
  periodical_cfg.report_quant_cell.rsrp = true;
  periodical_cfg.report_quant_cell.rsrq = true;
  periodical_cfg.report_quant_cell.sinr = true;
  periodical_cfg.max_report_cells       = 4;
  cfg.report_config_ids.emplace(uint_to_report_cfg_id(1), rrc_report_cfg_nr{periodical_cfg});

  manager = std::make_unique<cell_meas_manager>(
      cfg,
      cell_meas_manager_dependencies{.mobility_mng_notifier = mobility_manager,
                                     .ue_mng                = ue_mng,
                                     .logger                = ocudulog::fetch_basic_logger("CU-CP")});
  ASSERT_NE(manager, nullptr);

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  // Measurement config generation must skip the dangling neighbor and still produce the serving-cell part.
  std::optional<rrc_meas_cfg> meas_cfg = manager->get_measurement_config(ue_index, serving_nci);
  ASSERT_TRUE(meas_cfg.has_value());
  ASSERT_FALSE(meas_cfg.value().meas_obj_to_add_mod_list.empty());
}

TEST_F(cell_meas_manager_test, when_cell_config_is_updated_again_then_cho_config_has_no_duplicate_cells)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-15");

  create_cho_manager_single_frequency();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  attach_rrc_ue(ue_index);

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_serving = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci_target  = nr_cell_identity::create(gnb_id, 1).value();

  // Re-apply the target cell's own configuration, as a DU re-attach does.
  std::optional<cell_meas_config> target_cfg = manager->get_cell_config(nci_target);
  ASSERT_TRUE(target_cfg.has_value());
  ASSERT_TRUE(manager->update_cell_config(nci_target, target_cfg.value().serving_cell_cfg));

  // The CHO measurement config must list each candidate cell exactly once.
  std::vector<pci_t> candidate_pcis = {2, 3};
  auto cho_result = manager->get_measurement_config(ue_index, nci_serving, std::nullopt, true, candidate_pcis);
  ASSERT_TRUE(cho_result.has_value());

  std::map<pci_t, unsigned> pci_counts;
  for (const auto& meas_obj : cho_result->meas_obj_to_add_mod_list) {
    for (const auto& cell : meas_obj.meas_obj_nr->cells_to_add_mod_list) {
      pci_counts[cell.pci]++;
    }
  }
  for (const auto& [pci, count] : pci_counts) {
    EXPECT_EQ(count, 1U) << "pci=" << pci << " listed " << count << " times after a repeated cell update";
  }
}

namespace {

/// Build a complete serving-cell config on the default manager's serving frequency (632628, kHz30, sf5).
serving_cell_meas_config make_complete_serving_cell_cfg(nr_cell_identity nci, pci_t pci)
{
  serving_cell_meas_config serv_cfg;
  serv_cfg.gnb_id_bit_length   = 32;
  serv_cfg.nci                 = nci;
  serv_cfg.pci                 = pci;
  serv_cfg.band.emplace()      = nr_band::n78;
  serv_cfg.ssb_arfcn.emplace() = 632628;
  serv_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;
  rrc_ssb_mtc ssb_mtc;
  ssb_mtc.dur                                = 1;
  ssb_mtc.periodicity_and_offset.periodicity = rrc_periodicity_and_offset::periodicity_t::sf5;
  ssb_mtc.periodicity_and_offset.offset      = 0;
  serv_cfg.ssb_mtc.emplace()                 = ssb_mtc;
  return serv_cfg;
}

} // namespace

TEST_F(cell_meas_manager_test, when_neighbor_is_added_at_runtime_then_meas_config_contains_it)
{
  create_default_manager();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity serving_nci = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity new_nci     = nr_cell_identity::create(gnb_id, 3).value();

  std::optional<rrc_meas_cfg> before = manager->get_measurement_config(ue_index, serving_nci);
  ASSERT_TRUE(before.has_value());
  const size_t nof_meas_ids_before = before.value().meas_id_to_add_mod_list.size();

  // Add a new cell and a relation towards it at runtime.
  ASSERT_TRUE(manager->update_cell_config(new_nci, make_complete_serving_cell_cfg(new_nci, 7)));
  ASSERT_TRUE(manager->add_or_update_neighbor(serving_nci, new_nci, {uint_to_report_cfg_id(2)}));

  // The next measurement config regeneration picks the new neighbor up.
  std::optional<rrc_meas_cfg> after = manager->get_measurement_config(ue_index, serving_nci);
  ASSERT_TRUE(after.has_value());
  EXPECT_GT(after.value().meas_id_to_add_mod_list.size(), nof_meas_ids_before);
  std::vector<pci_t> neighbor_pcis = manager->get_neighbor_pcis(serving_nci);
  EXPECT_NE(std::find(neighbor_pcis.begin(), neighbor_pcis.end(), 7), neighbor_pcis.end());
}

TEST_F(cell_meas_manager_test, when_cell_is_set_again_then_its_neighbor_relations_are_kept)
{
  create_default_manager();

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci1 = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci2 = nr_cell_identity::create(gnb_id, 1).value();

  // Set the second cell again with a new PCI, as a runtime update of an external cell does.
  ASSERT_TRUE(manager->update_cell_config(nci2, make_complete_serving_cell_cfg(nci2, 9)));

  // Only the cell's own parameters change: the relations of both cells and the periodic report are kept.
  std::optional<cell_meas_config> cell1 = manager->get_cell_config(nci1);
  ASSERT_TRUE(cell1.has_value());
  ASSERT_EQ(cell1->ncells.size(), 1U);
  EXPECT_EQ(cell1->ncells[0].nci, nci2);
  EXPECT_EQ(cell1->ncells[0].report_cfg_ids, std::vector<report_cfg_id_t>{uint_to_report_cfg_id(2)});

  std::optional<cell_meas_config> cell2 = manager->get_cell_config(nci2);
  ASSERT_TRUE(cell2.has_value());
  EXPECT_EQ(cell2->serving_cell_cfg.pci, 9);
  ASSERT_EQ(cell2->ncells.size(), 1U);
  EXPECT_EQ(cell2->ncells[0].nci, nci1);
  EXPECT_EQ(cell2->periodic_report_cfg_id, uint_to_report_cfg_id(1));

  std::vector<pci_t> neighbor_pcis = manager->get_neighbor_pcis(nci1);
  EXPECT_NE(std::find(neighbor_pcis.begin(), neighbor_pcis.end(), 9), neighbor_pcis.end());
}

TEST_F(cell_meas_manager_test, when_cell_is_set_without_its_radio_parameters_then_they_are_cleared)
{
  create_default_manager();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity serving_nci = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity new_nci     = nr_cell_identity::create(gnb_id, 3).value();

  std::optional<rrc_meas_cfg> before = manager->get_measurement_config(ue_index, serving_nci);
  ASSERT_TRUE(before.has_value());
  const size_t nof_meas_ids_before = before.value().meas_id_to_add_mod_list.size();

  // A complete external cell with a relation towards it is measured.
  ASSERT_TRUE(manager->update_cell_config(new_nci, make_complete_serving_cell_cfg(new_nci, 7)));
  ASSERT_TRUE(manager->add_or_update_neighbor(serving_nci, new_nci, {uint_to_report_cfg_id(2)}));
  std::optional<rrc_meas_cfg> with_cell = manager->get_measurement_config(ue_index, serving_nci);
  ASSERT_TRUE(with_cell.has_value());
  ASSERT_GT(with_cell.value().meas_id_to_add_mod_list.size(), nof_meas_ids_before);

  // Set the cell again with its identity only. The update replaces the cell's parameters as a whole, so the
  // radio parameters left out are cleared rather than kept from the previous configuration.
  serving_cell_meas_config identity_only;
  identity_only.gnb_id_bit_length = gnb_id.bit_length;
  identity_only.nci               = new_nci;
  identity_only.pci               = 7;
  ASSERT_TRUE(manager->update_cell_config(new_nci, identity_only));

  std::optional<cell_meas_config> cell_cfg = manager->get_cell_config(new_nci);
  ASSERT_TRUE(cell_cfg.has_value());
  EXPECT_FALSE(cell_cfg->serving_cell_cfg.ssb_arfcn.has_value());
  EXPECT_FALSE(cell_cfg->serving_cell_cfg.band.has_value());
  EXPECT_FALSE(cell_cfg->serving_cell_cfg.ssb_scs.has_value());
  EXPECT_FALSE(cell_cfg->serving_cell_cfg.ssb_mtc.has_value());
  // The relation towards it is kept, but without SSB parameters the cell is no longer measured.
  ASSERT_EQ(manager->get_cell_config(serving_nci)->ncells.size(), 2U);
  std::optional<rrc_meas_cfg> without_params = manager->get_measurement_config(ue_index, serving_nci);
  ASSERT_TRUE(without_params.has_value());
  EXPECT_EQ(without_params.value().meas_id_to_add_mod_list.size(), nof_meas_ids_before);
}

TEST_F(cell_meas_manager_test, when_neighbor_relation_with_unknown_cell_is_added_then_it_fails)
{
  create_default_manager();

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity serving_nci = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity unknown_nci = nr_cell_identity::create(gnb_id, 9).value();

  EXPECT_FALSE(manager->add_or_update_neighbor(serving_nci, unknown_nci, {uint_to_report_cfg_id(2)}));
  EXPECT_FALSE(manager->add_or_update_neighbor(unknown_nci, serving_nci, {uint_to_report_cfg_id(2)}));
  EXPECT_FALSE(manager->add_or_update_neighbor(serving_nci, serving_nci, {uint_to_report_cfg_id(2)}));
}

TEST_F(cell_meas_manager_test, when_neighbor_relation_with_periodical_report_is_added_then_it_fails)
{
  create_default_manager();

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity serving_nci  = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity neighbor_nci = nr_cell_identity::create(gnb_id, 1).value();

  // Report config 1 is periodical: not allowed on a neighbor relation. Unknown ids are not allowed either.
  EXPECT_FALSE(manager->add_or_update_neighbor(serving_nci, neighbor_nci, {uint_to_report_cfg_id(1)}));
  EXPECT_FALSE(manager->add_or_update_neighbor(serving_nci, neighbor_nci, {uint_to_report_cfg_id(60)}));
}

TEST_F(cell_meas_manager_test, when_neighbor_is_removed_then_meas_config_drops_it)
{
  create_default_manager();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity serving_nci  = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity neighbor_nci = nr_cell_identity::create(gnb_id, 1).value();

  std::optional<rrc_meas_cfg> before = manager->get_measurement_config(ue_index, serving_nci);
  ASSERT_TRUE(before.has_value());
  const size_t nof_meas_objs_before = before.value().meas_obj_to_add_mod_list.size();

  ASSERT_TRUE(manager->remove_neighbor(serving_nci, neighbor_nci));
  // Removing it again fails: the relation is gone.
  EXPECT_FALSE(manager->remove_neighbor(serving_nci, neighbor_nci));

  // The neighbor's frequency drops out of the regenerated measurement config.
  std::optional<rrc_meas_cfg> after = manager->get_measurement_config(ue_index, serving_nci);
  ASSERT_TRUE(after.has_value());
  EXPECT_LT(after.value().meas_obj_to_add_mod_list.size(), nof_meas_objs_before);
  EXPECT_TRUE(manager->get_neighbor_pcis(serving_nci).empty());
}

TEST_F(cell_meas_manager_test, when_cell_is_removed_then_relations_and_meas_objects_are_dropped)
{
  create_default_manager();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity serving_nci = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity removed_nci = nr_cell_identity::create(gnb_id, 1).value();

  std::optional<rrc_meas_cfg> before = manager->get_measurement_config(ue_index, serving_nci);
  ASSERT_TRUE(before.has_value());
  const size_t nof_meas_objs_before = before.value().meas_obj_to_add_mod_list.size();

  ASSERT_TRUE(manager->remove_cell_config(removed_nci));
  EXPECT_FALSE(manager->remove_cell_config(removed_nci));
  EXPECT_FALSE(manager->get_cell_config(removed_nci).has_value());

  // The incoming relation was cascaded away and the removed cell's frequency lost its measurement object.
  EXPECT_TRUE(manager->get_neighbor_pcis(serving_nci).empty());
  std::optional<rrc_meas_cfg> after = manager->get_measurement_config(ue_index, serving_nci);
  ASSERT_TRUE(after.has_value());
  EXPECT_LT(after.value().meas_obj_to_add_mod_list.size(), nof_meas_objs_before);
}

TEST_F(cell_meas_manager_test, when_removed_cell_is_reported_then_report_is_ignored)
{
  create_default_manager();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity serving_nci = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity removed_nci = nr_cell_identity::create(gnb_id, 1).value();

  // Give the UE a real measurement config, so its contexts reference the neighbor.
  ASSERT_TRUE(manager->get_measurement_config(ue_index, serving_nci).has_value());
  auto&     ue_meas_ctxt  = ue_mng.get_measurement_context(ue_index);
  meas_id_t neigh_meas_id = meas_id_t::invalid;
  for (const auto& [meas_id, ctxt] : ue_meas_ctxt.meas_id_to_meas_context) {
    if (ctxt.nci == removed_nci) {
      neigh_meas_id = meas_id;
      break;
    }
  }
  ASSERT_NE(neigh_meas_id, meas_id_t::invalid) << "expected a measurement context referencing the neighbor";

  // Remove the cell, then let the stale in-flight report arrive.
  ASSERT_TRUE(manager->remove_cell_config(removed_nci));
  rrc_meas_results results;
  results.meas_id = neigh_meas_id;
  manager->report_measurement(ue_index, results);
  EXPECT_EQ(mobility_manager.nof_notifications, 0U);
}

TEST_F(cell_meas_manager_test, when_referenced_report_config_is_removed_then_it_fails)
{
  create_default_manager();

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_a = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci_b = nr_cell_identity::create(gnb_id, 1).value();

  // Report config 2 is referenced by both relations; report config 1 by the serving cell's periodic report.
  EXPECT_FALSE(manager->remove_report_config(uint_to_report_cfg_id(2)));
  EXPECT_FALSE(manager->remove_report_config(uint_to_report_cfg_id(1)));

  ASSERT_TRUE(manager->remove_neighbor(nci_a, nci_b));
  ASSERT_TRUE(manager->remove_neighbor(nci_b, nci_a));
  EXPECT_TRUE(manager->remove_report_config(uint_to_report_cfg_id(2)));

  // Both cells of the default manager use report config 1 as their periodic report.
  ASSERT_TRUE(manager->set_periodic_report_config(nci_a, std::nullopt));
  EXPECT_FALSE(manager->remove_report_config(uint_to_report_cfg_id(1)));
  ASSERT_TRUE(manager->set_periodic_report_config(nci_b, std::nullopt));
  EXPECT_TRUE(manager->remove_report_config(uint_to_report_cfg_id(1)));

  // Removing an unknown id fails.
  EXPECT_FALSE(manager->remove_report_config(uint_to_report_cfg_id(60)));
}

TEST_F(cell_meas_manager_test, when_report_config_type_conflicts_with_references_then_update_fails)
{
  create_default_manager();

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_a = nr_cell_identity::create(gnb_id, 0).value();

  // The serving-cell periodic report cannot point at an event-triggered config.
  EXPECT_FALSE(manager->set_periodic_report_config(nci_a, uint_to_report_cfg_id(2)));
  EXPECT_FALSE(manager->set_periodic_report_config(nci_a, uint_to_report_cfg_id(60)));

  // Config 2 is referenced by neighbor relations: it cannot become periodical.
  rrc_periodical_report_cfg periodical_cfg;
  periodical_cfg.rs_type                = ocucp::rrc_nr_rs_type::ssb;
  periodical_cfg.report_interv          = 1024;
  periodical_cfg.report_amount          = -1;
  periodical_cfg.report_quant_cell.rsrp = true;
  periodical_cfg.max_report_cells       = 4;
  EXPECT_FALSE(manager->add_or_update_report_config(uint_to_report_cfg_id(2), rrc_report_cfg_nr{periodical_cfg}));

  // Config 1 is the serving cell's periodic report: it cannot become event-triggered.
  rrc_event_trigger_cfg event_cfg = {};
  rrc_event_id          event_a3;
  event_a3.id                                                          = rrc_event_id::event_id_t::a3;
  event_a3.meas_trigger_quant_thres_or_offset.emplace().rsrp.emplace() = 6;
  event_cfg.event_id                                                   = event_a3;
  EXPECT_FALSE(manager->add_or_update_report_config(uint_to_report_cfg_id(1), rrc_report_cfg_nr{event_cfg}));

  // A fresh id is accepted.
  EXPECT_TRUE(manager->add_or_update_report_config(uint_to_report_cfg_id(10), rrc_report_cfg_nr{periodical_cfg}));
  EXPECT_TRUE(manager->set_periodic_report_config(nci_a, uint_to_report_cfg_id(10)));
}

TEST_F(cell_meas_manager_test, when_nothing_remains_to_measure_then_removal_only_config_is_generated)
{
  create_default_manager();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_a = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci_b = nr_cell_identity::create(gnb_id, 1).value();

  std::optional<rrc_meas_cfg> current = manager->get_measurement_config(ue_index, nci_a);
  ASSERT_TRUE(current.has_value());

  // Strip the serving cell of everything it could measure or report.
  ASSERT_TRUE(manager->remove_neighbor(nci_a, nci_b));
  ASSERT_TRUE(manager->set_periodic_report_config(nci_a, std::nullopt));

  // With the UE's current config provided, a removal-only config is generated.
  std::optional<rrc_meas_cfg> rem_cfg = manager->get_measurement_config(ue_index, nci_a, current);
  ASSERT_TRUE(rem_cfg.has_value());
  EXPECT_TRUE(rem_cfg.value().meas_obj_to_add_mod_list.empty());
  EXPECT_TRUE(rem_cfg.value().meas_id_to_add_mod_list.empty());
  EXPECT_EQ(rem_cfg.value().meas_obj_to_rem_list.size(), current.value().meas_obj_to_add_mod_list.size());
  EXPECT_EQ(rem_cfg.value().meas_id_to_rem_list.size(), current.value().meas_id_to_add_mod_list.size());

  // Without a current config there is nothing to remove.
  EXPECT_FALSE(manager->get_measurement_config(ue_index, nci_a).has_value());
}

TEST_F(cell_meas_manager_test, when_serving_cell_is_removed_then_removal_only_config_is_generated)
{
  create_default_manager();

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(uint_to_cu_cp_du_index(0));
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_a = nr_cell_identity::create(gnb_id, 0).value();

  std::optional<rrc_meas_cfg> current = manager->get_measurement_config(ue_index, nci_a);
  ASSERT_TRUE(current.has_value());

  ASSERT_TRUE(manager->remove_cell_config(nci_a));

  std::optional<rrc_meas_cfg> rem_cfg = manager->get_measurement_config(ue_index, nci_a, current);
  ASSERT_TRUE(rem_cfg.has_value());
  EXPECT_TRUE(rem_cfg.value().meas_obj_to_add_mod_list.empty());
  EXPECT_FALSE(rem_cfg.value().meas_obj_to_rem_list.empty());
}
