// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_meas_manager_test_helpers.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/plmn_identity.h"
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

TEST_F(cell_meas_manager_test, when_old_meas_config_is_provided_old_ids_are_removed)
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

  // Make sure initial IDs are release again.
  ASSERT_EQ(target_meas_cfg.value().meas_obj_to_rem_list.at(0),
            initial_meas_cfg.value().meas_obj_to_add_mod_list.at(0).meas_obj_id);

  ASSERT_EQ(target_meas_cfg.value().meas_id_to_rem_list.at(0),
            initial_meas_cfg.value().meas_id_to_add_mod_list.at(0).meas_id);

  ASSERT_EQ(target_meas_cfg.value().report_cfg_to_rem_list.at(0),
            initial_meas_cfg.value().report_cfg_to_add_mod_list.at(0).report_cfg_id);

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

  std::optional<rrc_meas_cfg> target_meas_cfg = manager->get_measurement_config(ue_index, target_nci, initial_meas_cfg);
  ASSERT_FALSE(target_meas_cfg.has_value());
}

TEST_F(cell_meas_manager_test, when_t312_is_configured_then_meas_obj_has_t312_and_report_cfg_has_t312)
{
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
  create_default_manager();

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity unknown_nci = nr_cell_identity::create(gnb_id, 5).value();
  nr_cell_identity nci2        = nr_cell_identity::create(gnb_id, 1).value();

  std::vector<rrc_ntn_neighbour_cell_info_item> items = {{nci2, make_test_ntn_neighbour_info()}};
  ASSERT_FALSE(manager->update_ntn_neighbour_info(unknown_nci, items));
}

TEST_F(cell_meas_manager_test, when_ntn_update_refers_to_unknown_neighbour_then_update_fails)
{
  create_default_manager();

  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci1        = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity unknown_nci = nr_cell_identity::create(gnb_id, 5).value();

  std::vector<rrc_ntn_neighbour_cell_info_item> items = {{unknown_nci, make_test_ntn_neighbour_info()}};
  ASSERT_FALSE(manager->update_ntn_neighbour_info(nci1, items));
}

TEST_F(cell_meas_manager_test, when_cho_meas_config_requested_then_ntn_neighbour_info_is_included)
{
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
