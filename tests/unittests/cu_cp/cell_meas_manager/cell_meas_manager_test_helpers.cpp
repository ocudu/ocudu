// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_meas_manager_test_helpers.h"
#include "lib/cu_cp/cell_meas_manager/cell_meas_manager_helpers.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"

using namespace ocudu;
using namespace ocucp;

namespace {
/// Helper to create cell configuration with common defaults
cell_meas_config make_cell_config(const gnb_id_t&    gnb_id,
                                  nr_cell_identity   nci,
                                  pci_t              pci,
                                  unsigned           ssb_arfcn,
                                  nr_band            band = nr_band::n78,
                                  subcarrier_spacing scs  = subcarrier_spacing::kHz30)
{
  cell_meas_config cfg;
  cfg.serving_cell_cfg.gnb_id_bit_length = gnb_id.bit_length;
  cfg.serving_cell_cfg.nci               = nci;
  cfg.serving_cell_cfg.pci               = pci;
  cfg.serving_cell_cfg.band              = band;
  cfg.serving_cell_cfg.ssb_arfcn         = ssb_arfcn;
  cfg.serving_cell_cfg.ssb_scs           = scs;

  rrc_ssb_mtc ssb_mtc;
  ssb_mtc.dur                                = 1;
  ssb_mtc.periodicity_and_offset.periodicity = rrc_periodicity_and_offset::periodicity_t::sf5;
  ssb_mtc.periodicity_and_offset.offset      = 0;
  cfg.serving_cell_cfg.ssb_mtc               = ssb_mtc;

  return cfg;
}
} // namespace

cell_meas_manager_test::cell_meas_manager_test() :
  cu_cp_cfg([this]() {
    cu_cp_configuration cucfg     = config_helpers::make_default_cu_cp_config();
    cucfg.services.timers         = &timers;
    cucfg.services.cu_cp_executor = &ctrl_worker;
    return cucfg;
  }())
{
  cu_cp_logger.set_level(ocudulog::basic_levels::debug);
  test_logger.set_level(ocudulog::basic_levels::debug);
  test_logger.set_hex_dump_max_size(32);
  ocudulog::init();
}

cell_meas_manager_test::~cell_meas_manager_test()
{
  // flush logger after each test
  ocudulog::flush();
}

void cell_meas_manager_test::create_empty_manager()
{
  cell_meas_manager_cfg cfg = {};
  manager                   = std::make_unique<cell_meas_manager>(cfg, mobility_manager, ue_mng);
  ASSERT_NE(manager, nullptr);
}

void cell_meas_manager_test::create_default_manager(std::optional<unsigned> t312)
{
  cell_meas_manager_cfg cfg;

  // Add 2 cells - one being the neighbor of the other one
  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci1 = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci2 = nr_cell_identity::create(gnb_id, 1).value();

  cell_meas_config cell_cfg;
  cell_cfg.serving_cell_cfg.gnb_id_bit_length = gnb_id.bit_length;
  cell_cfg.serving_cell_cfg.nci               = nci1;
  cell_cfg.serving_cell_cfg.pci               = 1;
  cell_cfg.periodic_report_cfg_id             = uint_to_report_cfg_id(1);

  neighbor_cell_meas_config ncell_meas_cfg;
  ncell_meas_cfg.nci = nci2;
  ncell_meas_cfg.report_cfg_ids.push_back(uint_to_report_cfg_id(2));
  cell_cfg.ncells.push_back(ncell_meas_cfg);

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
  cfg.cells.emplace(cell_cfg.serving_cell_cfg.nci, cell_cfg);

  // Reuse config to setup config for next cell.
  cell_cfg.serving_cell_cfg.nci = nci2;

  cell_cfg.ncells.clear();
  ncell_meas_cfg.nci = nci1;
  ncell_meas_cfg.report_cfg_ids.clear();
  ncell_meas_cfg.report_cfg_ids.push_back(uint_to_report_cfg_id(2));
  cell_cfg.ncells.push_back(ncell_meas_cfg);

  cell_cfg.serving_cell_cfg.band.emplace()      = nr_band::n78;
  cell_cfg.serving_cell_cfg.ssb_arfcn.emplace() = 632128;
  cell_cfg.serving_cell_cfg.ssb_scs.emplace()   = subcarrier_spacing::kHz30;
  {
    rrc_ssb_mtc ssb_mtc;
    ssb_mtc.dur                                 = 1;
    ssb_mtc.periodicity_and_offset.periodicity  = rrc_periodicity_and_offset::periodicity_t::sf5;
    ssb_mtc.periodicity_and_offset.offset       = 0;
    cell_cfg.serving_cell_cfg.ssb_mtc.emplace() = ssb_mtc;
  }
  cfg.cells.emplace(cell_cfg.serving_cell_cfg.nci, cell_cfg);

  // Add periodic event.
  rrc_report_cfg_nr         periodic_report_cfg;
  rrc_periodical_report_cfg periodical_cfg;

  periodical_cfg.rs_type                = ocucp::rrc_nr_rs_type::ssb;
  periodical_cfg.report_interv          = 1024;
  periodical_cfg.report_amount          = -1;
  periodical_cfg.report_quant_cell.rsrp = true;
  periodical_cfg.report_quant_cell.rsrq = true;
  periodical_cfg.report_quant_cell.sinr = true;
  periodical_cfg.max_report_cells       = 4;

  periodic_report_cfg = periodical_cfg;
  cfg.report_config_ids.emplace(uint_to_report_cfg_id(1), periodic_report_cfg);

  // Add A3 event.
  rrc_report_cfg_nr     a3_report_cfg;
  rrc_event_trigger_cfg event_trigger_cfg = {};

  rrc_event_id event_a3;
  event_a3.id = rrc_event_id::event_id_t::a3;
  event_a3.meas_trigger_quant_thres_or_offset.emplace();
  event_a3.meas_trigger_quant_thres_or_offset.value().rsrp.emplace() = 6;
  event_a3.hysteresis                                                = 0;
  event_a3.time_to_trigger                                           = 100;
  event_a3.use_allowed_cell_list                                     = false;

  event_trigger_cfg.event_id = event_a3;

  event_trigger_cfg.rs_type                = ocucp::rrc_nr_rs_type::ssb;
  event_trigger_cfg.report_interv          = 1024;
  event_trigger_cfg.report_amount          = -1;
  event_trigger_cfg.report_quant_cell.rsrp = true;
  event_trigger_cfg.report_quant_cell.rsrq = true;
  event_trigger_cfg.report_quant_cell.sinr = true;
  event_trigger_cfg.max_report_cells       = 4;

  rrc_meas_report_quant report_quant_rs_idxes;
  report_quant_rs_idxes.rsrp              = true;
  report_quant_rs_idxes.rsrq              = true;
  report_quant_rs_idxes.sinr              = true;
  event_trigger_cfg.report_quant_rs_idxes = report_quant_rs_idxes;

  // Set T312 if provided.
  event_trigger_cfg.t312 = t312;

  a3_report_cfg = event_trigger_cfg;
  cfg.report_config_ids.emplace(uint_to_report_cfg_id(2), a3_report_cfg);

  manager = std::make_unique<cell_meas_manager>(cfg, mobility_manager, ue_mng);
  ASSERT_NE(manager, nullptr);
}

void cell_meas_manager_test::create_manager_with_incomplete_cells_and_periodic_report_at_target_cell()
{
  cell_meas_manager_cfg cfg;

  // Add 2 cells - one being the neighbor of the other one
  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci1 = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci2 = nr_cell_identity::create(gnb_id, 1).value();

  cell_meas_config cell_cfg;
  cell_cfg.serving_cell_cfg.gnb_id_bit_length = gnb_id.bit_length;
  cell_cfg.serving_cell_cfg.nci               = nci1;
  cell_cfg.serving_cell_cfg.pci               = 1;

  neighbor_cell_meas_config ncell_meas_cfg;
  ncell_meas_cfg.nci = nci2;
  ncell_meas_cfg.report_cfg_ids.push_back(uint_to_report_cfg_id(2));
  cell_cfg.ncells.push_back(ncell_meas_cfg);
  cfg.cells.emplace(cell_cfg.serving_cell_cfg.nci, cell_cfg);

  // Reuse config to setup config for next cell.
  cell_cfg.serving_cell_cfg.nci   = nci2;
  cell_cfg.periodic_report_cfg_id = uint_to_report_cfg_id(1);

  cell_cfg.ncells.clear();
  ncell_meas_cfg.nci = nci1;
  ncell_meas_cfg.report_cfg_ids.clear();
  ncell_meas_cfg.report_cfg_ids.push_back(uint_to_report_cfg_id(2));
  cell_cfg.ncells.push_back(ncell_meas_cfg);
  cfg.cells.emplace(cell_cfg.serving_cell_cfg.nci, cell_cfg);

  // Add periodic event.
  rrc_report_cfg_nr         periodic_report_cfg;
  rrc_periodical_report_cfg periodical_cfg;

  periodical_cfg.rs_type                = ocucp::rrc_nr_rs_type::ssb;
  periodical_cfg.report_interv          = 1024;
  periodical_cfg.report_amount          = -1;
  periodical_cfg.report_quant_cell.rsrp = true;
  periodical_cfg.report_quant_cell.rsrq = true;
  periodical_cfg.report_quant_cell.sinr = true;
  periodical_cfg.max_report_cells       = 4;

  periodic_report_cfg = periodical_cfg;
  cfg.report_config_ids.emplace(uint_to_report_cfg_id(1), periodic_report_cfg);

  // Add A3 event.
  rrc_report_cfg_nr     a3_report_cfg;
  rrc_event_trigger_cfg event_trigger_cfg = {};

  rrc_event_id event_a3;
  event_a3.id = rrc_event_id::event_id_t::a3;
  event_a3.meas_trigger_quant_thres_or_offset.emplace();
  event_a3.meas_trigger_quant_thres_or_offset.value().rsrp.emplace() = 6;
  event_a3.hysteresis                                                = 0;
  event_a3.time_to_trigger                                           = 100;
  event_a3.use_allowed_cell_list                                     = false;

  event_trigger_cfg.event_id = event_a3;

  event_trigger_cfg.rs_type                = ocucp::rrc_nr_rs_type::ssb;
  event_trigger_cfg.report_interv          = 1024;
  event_trigger_cfg.report_amount          = -1;
  event_trigger_cfg.report_quant_cell.rsrp = true;
  event_trigger_cfg.report_quant_cell.rsrq = true;
  event_trigger_cfg.report_quant_cell.sinr = true;
  event_trigger_cfg.max_report_cells       = 4;

  rrc_meas_report_quant report_quant_rs_idxes;
  report_quant_rs_idxes.rsrp              = true;
  report_quant_rs_idxes.rsrq              = true;
  report_quant_rs_idxes.sinr              = true;
  event_trigger_cfg.report_quant_rs_idxes = report_quant_rs_idxes;

  a3_report_cfg = event_trigger_cfg;
  cfg.report_config_ids.emplace(uint_to_report_cfg_id(2), a3_report_cfg);

  manager = std::make_unique<cell_meas_manager>(cfg, mobility_manager, ue_mng);
  ASSERT_NE(manager, nullptr);
}

void cell_meas_manager_test::create_manager_without_ncells_and_periodic_report()
{
  cell_meas_manager_cfg cfg;

  // Add serving cell
  gnb_id_t         gnb_id{411, 32};
  cell_meas_config cell_cfg;
  cell_cfg.serving_cell_cfg.gnb_id_bit_length = gnb_id.bit_length;
  cell_cfg.serving_cell_cfg.nci               = nr_cell_identity::create(gnb_id, 0).value();

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
  cfg.cells.emplace(cell_cfg.serving_cell_cfg.nci, cell_cfg);

  // Add A3 event.
  rrc_report_cfg_nr     a3_report_cfg;
  rrc_event_trigger_cfg event_trigger_cfg = {};

  rrc_event_id event_a3;
  event_a3.id = rrc_event_id::event_id_t::a3;
  event_a3.meas_trigger_quant_thres_or_offset.emplace();
  event_a3.meas_trigger_quant_thres_or_offset.value().rsrp.emplace() = 6;
  event_a3.hysteresis                                                = 0;
  event_a3.time_to_trigger                                           = 100;
  event_a3.use_allowed_cell_list                                     = false;

  event_trigger_cfg.event_id = event_a3;

  event_trigger_cfg.rs_type                = ocucp::rrc_nr_rs_type::ssb;
  event_trigger_cfg.report_interv          = 1024;
  event_trigger_cfg.report_amount          = -1;
  event_trigger_cfg.report_quant_cell.rsrp = true;
  event_trigger_cfg.report_quant_cell.rsrq = true;
  event_trigger_cfg.report_quant_cell.sinr = true;
  event_trigger_cfg.max_report_cells       = 4;

  rrc_meas_report_quant report_quant_rs_idxes;
  report_quant_rs_idxes.rsrp              = true;
  report_quant_rs_idxes.rsrq              = true;
  report_quant_rs_idxes.sinr              = true;
  event_trigger_cfg.report_quant_rs_idxes = report_quant_rs_idxes;

  a3_report_cfg = event_trigger_cfg = {};
  cfg.report_config_ids.emplace(uint_to_report_cfg_id(1), a3_report_cfg);

  manager = std::make_unique<cell_meas_manager>(cfg, mobility_manager, ue_mng);
  ASSERT_NE(manager, nullptr);
}

void cell_meas_manager_test::create_manager_inter_freq_without_periodic_report()
{
  cell_meas_manager_cfg cfg;

  // Serving cell and neighbor on different frequencies, with only an event-based (A3) report on the neighbor and
  // no periodic report on the serving cell (inter-frequency handover setup).
  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_serving = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci_target  = nr_cell_identity::create(gnb_id, 1).value();

  cell_meas_config serving_cfg = make_cell_config(gnb_id, nci_serving, 1, 632628);

  neighbor_cell_meas_config ncell;
  ncell.nci = nci_target;
  ncell.report_cfg_ids.push_back(uint_to_report_cfg_id(1));
  serving_cfg.ncells.push_back(ncell);

  cfg.cells.emplace(nci_serving, serving_cfg);
  cfg.cells.emplace(nci_target, make_cell_config(gnb_id, nci_target, 2, 633000));

  // Add A3 event report config for the neighbor.
  rrc_report_cfg_nr     a3_report_cfg;
  rrc_event_trigger_cfg event_trigger_cfg = {};
  rrc_event_id          event_a3;
  event_a3.id = rrc_event_id::event_id_t::a3;
  event_a3.meas_trigger_quant_thres_or_offset.emplace();
  event_a3.meas_trigger_quant_thres_or_offset.value().rsrp.emplace() = 6;
  event_a3.hysteresis                                                = 0;
  event_a3.time_to_trigger                                           = 100;
  event_a3.use_allowed_cell_list                                     = false;
  event_trigger_cfg.event_id                                         = event_a3;
  event_trigger_cfg.rs_type                                          = ocucp::rrc_nr_rs_type::ssb;
  event_trigger_cfg.report_interv                                    = 1024;
  event_trigger_cfg.report_amount                                    = -1;
  event_trigger_cfg.report_quant_cell.rsrp                           = true;
  event_trigger_cfg.max_report_cells                                 = 4;
  a3_report_cfg                                                      = event_trigger_cfg;
  cfg.report_config_ids.emplace(uint_to_report_cfg_id(1), a3_report_cfg);

  manager = std::make_unique<cell_meas_manager>(cfg, mobility_manager, ue_mng);
  ASSERT_NE(manager, nullptr);
}

void cell_meas_manager_test::check_default_meas_cfg(const std::optional<rrc_meas_cfg>& meas_cfg,
                                                    meas_obj_id_t                      meas_obj_id)
{
  ASSERT_TRUE(meas_cfg.has_value());
  ASSERT_EQ(meas_cfg.value().meas_obj_to_add_mod_list.size(), 2);
  ASSERT_EQ((unsigned)meas_cfg.value().meas_obj_to_add_mod_list.begin()->meas_obj_id, meas_obj_id_to_uint(meas_obj_id));
  // TODO: Add checks for more values
}

void cell_meas_manager_test::verify_meas_cfg(const std::optional<rrc_meas_cfg>& meas_cfg)
{
  // Performs sanity check on the config without making any assumptions.
  ASSERT_TRUE(meas_cfg.has_value());

  std::vector<meas_id_t> used_meas_ids;

  // Make sure sure all elements referenced in MeasIds exist.
  for (const auto& meas_item : meas_cfg.value().meas_id_to_add_mod_list) {
    // Check ID is not already used.
    ASSERT_EQ(std::find(used_meas_ids.begin(), used_meas_ids.end(), meas_item.meas_id), used_meas_ids.end());
    used_meas_ids.push_back(meas_item.meas_id);

    // Report config must be valid/present in meas_cfg.
    auto report_it = std::find_if(
        begin(meas_cfg.value().report_cfg_to_add_mod_list),
        end(meas_cfg.value().report_cfg_to_add_mod_list),
        [meas_item](const rrc_report_cfg_to_add_mod& x) { return x.report_cfg_id == meas_item.report_cfg_id; });
    ASSERT_NE(report_it, meas_cfg.value().report_cfg_to_add_mod_list.end());

    auto meas_obj_it =
        std::find_if(begin(meas_cfg.value().meas_obj_to_add_mod_list),
                     end(meas_cfg.value().meas_obj_to_add_mod_list),
                     [meas_item](const rrc_meas_obj_to_add_mod& x) { return x.meas_obj_id == meas_item.meas_obj_id; });
    ASSERT_NE(meas_obj_it, meas_cfg.value().meas_obj_to_add_mod_list.end());

    // check meas object is unique
    for (const auto& meas_obj : meas_cfg.value().meas_obj_to_add_mod_list) {
      for (const auto& meas_obj_2 : meas_cfg.value().meas_obj_to_add_mod_list) {
        if (meas_obj.meas_obj_id == meas_obj_2.meas_obj_id) {
          continue;
        }
        if (meas_obj.meas_obj_nr.has_value() && meas_obj_2.meas_obj_nr.has_value()) {
          ASSERT_FALSE(is_duplicate(meas_obj.meas_obj_nr.value(), meas_obj_2.meas_obj_nr.value()));
        }
      }
    }
  }
}

void cell_meas_manager_test::verify_empty_meas_cfg(const std::optional<rrc_meas_cfg>& meas_cfg)
{
  ASSERT_FALSE(meas_cfg.has_value());
}

void cell_meas_manager_test::create_cho_manager_single_frequency()
{
  cell_meas_manager_cfg cfg;

  // Add serving cell and 2 target cells on the SAME frequency
  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_serving = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci_target1 = nr_cell_identity::create(gnb_id, 1).value();
  nr_cell_identity nci_target2 = nr_cell_identity::create(gnb_id, 2).value();

  // Serving cell configuration
  cell_meas_config serving_cfg = make_cell_config(gnb_id, nci_serving, 1, 632628);

  // Add target cells as neighbors with CHO conditional triggers
  neighbor_cell_meas_config ncell1;
  ncell1.nci = nci_target1;
  ncell1.report_cfg_ids.push_back(uint_to_report_cfg_id(1)); // CHO conditional trigger
  serving_cfg.ncells.push_back(ncell1);

  neighbor_cell_meas_config ncell2;
  ncell2.nci = nci_target2;
  ncell2.report_cfg_ids.push_back(uint_to_report_cfg_id(1)); // CHO conditional trigger
  serving_cfg.ncells.push_back(ncell2);

  cfg.cells.emplace(nci_serving, serving_cfg);

  // Target cell configurations (same frequency)
  cfg.cells.emplace(nci_target1, make_cell_config(gnb_id, nci_target1, 2, 632628));
  cfg.cells.emplace(nci_target2, make_cell_config(gnb_id, nci_target2, 3, 632628));

  // Add CHO conditional trigger (A3 event)
  rrc_report_cfg_nr    cho_trigger_cfg;
  rrc_cond_trigger_cfg cond_trigger;
  rrc_event_id         cond_event_a3;
  cond_event_a3.id = rrc_event_id::event_id_t::a3;
  cond_event_a3.meas_trigger_quant_thres_or_offset.emplace();
  cond_event_a3.meas_trigger_quant_thres_or_offset.value().rsrp = 6;
  cond_event_a3.hysteresis                                      = 0;
  cond_event_a3.time_to_trigger                                 = 100;
  cond_trigger.cond_event_id                                    = cond_event_a3;
  cond_trigger.rs_type                                          = rrc_nr_rs_type::ssb;
  cho_trigger_cfg                                               = cond_trigger;
  cfg.report_config_ids.emplace(uint_to_report_cfg_id(1), cho_trigger_cfg);

  manager = std::make_unique<cell_meas_manager>(cfg, mobility_manager, ue_mng);
  ASSERT_NE(manager, nullptr);
}

void cell_meas_manager_test::create_cho_manager_multi_frequency()
{
  cell_meas_manager_cfg cfg;

  // Add serving cell and 2 target cells on DIFFERENT frequencies
  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_serving = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci_target1 = nr_cell_identity::create(gnb_id, 1).value();
  nr_cell_identity nci_target2 = nr_cell_identity::create(gnb_id, 2).value();

  // Serving cell configuration
  cell_meas_config serving_cfg = make_cell_config(gnb_id, nci_serving, 1, 632628);

  // Add target cells as neighbors with CHO conditional triggers
  neighbor_cell_meas_config ncell1;
  ncell1.nci = nci_target1;
  ncell1.report_cfg_ids.push_back(uint_to_report_cfg_id(1)); // CHO conditional trigger
  serving_cfg.ncells.push_back(ncell1);

  neighbor_cell_meas_config ncell2;
  ncell2.nci = nci_target2;
  ncell2.report_cfg_ids.push_back(uint_to_report_cfg_id(1)); // CHO conditional trigger
  serving_cfg.ncells.push_back(ncell2);

  cfg.cells.emplace(nci_serving, serving_cfg);

  // Target cell configurations (different frequencies)
  cfg.cells.emplace(nci_target1, make_cell_config(gnb_id, nci_target1, 2, 633000));
  cfg.cells.emplace(nci_target2, make_cell_config(gnb_id, nci_target2, 3, 634000));

  // Add CHO conditional trigger (A3 event)
  rrc_report_cfg_nr    cho_trigger_cfg;
  rrc_cond_trigger_cfg cond_trigger;
  rrc_event_id         cond_event_a3;
  cond_event_a3.id = rrc_event_id::event_id_t::a3;
  cond_event_a3.meas_trigger_quant_thres_or_offset.emplace();
  cond_event_a3.meas_trigger_quant_thres_or_offset.value().rsrp = 6;
  cond_event_a3.hysteresis                                      = 0;
  cond_event_a3.time_to_trigger                                 = 100;
  cond_trigger.cond_event_id                                    = cond_event_a3;
  cond_trigger.rs_type                                          = rrc_nr_rs_type::ssb;
  cho_trigger_cfg                                               = cond_trigger;
  cfg.report_config_ids.emplace(uint_to_report_cfg_id(1), cho_trigger_cfg);

  manager = std::make_unique<cell_meas_manager>(cfg, mobility_manager, ue_mng);
  ASSERT_NE(manager, nullptr);
}

void cell_meas_manager_test::create_cho_manager_multi_trigger()
{
  cell_meas_manager_cfg cfg;

  // Add serving cell and 1 target cell with MULTIPLE conditional triggers
  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_serving = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci_target  = nr_cell_identity::create(gnb_id, 1).value();

  // Serving cell configuration
  cell_meas_config serving_cfg = make_cell_config(gnb_id, nci_serving, 1, 632628);

  // Add target cell with TWO conditional triggers
  neighbor_cell_meas_config ncell;
  ncell.nci = nci_target;
  ncell.report_cfg_ids.push_back(uint_to_report_cfg_id(1)); // CHO trigger A3
  ncell.report_cfg_ids.push_back(uint_to_report_cfg_id(2)); // CHO trigger A5
  serving_cfg.ncells.push_back(ncell);

  cfg.cells.emplace(nci_serving, serving_cfg);

  // Target cell configuration
  cfg.cells.emplace(nci_target, make_cell_config(gnb_id, nci_target, 2, 633000));

  // Add CHO conditional trigger A3
  rrc_report_cfg_nr    cho_trigger_a3;
  rrc_cond_trigger_cfg cond_trigger_a3;
  rrc_event_id         cond_event_a3;
  cond_event_a3.id = rrc_event_id::event_id_t::a3;
  cond_event_a3.meas_trigger_quant_thres_or_offset.emplace();
  cond_event_a3.meas_trigger_quant_thres_or_offset.value().rsrp = 6;
  cond_event_a3.hysteresis                                      = 0;
  cond_event_a3.time_to_trigger                                 = 100;
  cond_trigger_a3.cond_event_id                                 = cond_event_a3;
  cond_trigger_a3.rs_type                                       = rrc_nr_rs_type::ssb;
  cho_trigger_a3                                                = cond_trigger_a3;
  cfg.report_config_ids.emplace(uint_to_report_cfg_id(1), cho_trigger_a3);

  // Add CHO conditional trigger A5
  rrc_report_cfg_nr    cho_trigger_a5;
  rrc_cond_trigger_cfg cond_trigger_a5;
  rrc_event_id         cond_event_a5;
  cond_event_a5.id = rrc_event_id::event_id_t::a5;
  cond_event_a5.meas_trigger_quant_thres_or_offset.emplace();
  cond_event_a5.meas_trigger_quant_thres_or_offset.value().rsrp = 10;
  cond_event_a5.meas_trigger_quant_thres_2.emplace();
  cond_event_a5.meas_trigger_quant_thres_2.value().rsrp = 20;
  cond_event_a5.hysteresis                              = 0;
  cond_event_a5.time_to_trigger                         = 100;
  cond_trigger_a5.cond_event_id                         = cond_event_a5;
  cond_trigger_a5.rs_type                               = rrc_nr_rs_type::ssb;
  cho_trigger_a5                                        = cond_trigger_a5;
  cfg.report_config_ids.emplace(uint_to_report_cfg_id(2), cho_trigger_a5);

  manager = std::make_unique<cell_meas_manager>(cfg, mobility_manager, ue_mng);
  ASSERT_NE(manager, nullptr);
}

void cell_meas_manager_test::create_cho_manager_a5_inter_frequency()
{
  cell_meas_manager_cfg cfg;

  // Serving cell and target cell on DIFFERENT frequencies to exercise inter-frequency A5 CHO.
  gnb_id_t         gnb_id{0x19b, 32};
  nr_cell_identity nci_serving = nr_cell_identity::create(gnb_id, 0).value();
  nr_cell_identity nci_target  = nr_cell_identity::create(gnb_id, 1).value();

  cell_meas_config serving_cfg = make_cell_config(gnb_id, nci_serving, 1, 632628);

  neighbor_cell_meas_config ncell;
  ncell.nci = nci_target;
  ncell.report_cfg_ids.push_back(uint_to_report_cfg_id(1));
  serving_cfg.ncells.push_back(ncell);

  cfg.cells.emplace(nci_serving, serving_cfg);
  cfg.cells.emplace(nci_target, make_cell_config(gnb_id, nci_target, 2, 633000));

  rrc_report_cfg_nr    cho_trigger_cfg;
  rrc_cond_trigger_cfg cond_trigger;
  rrc_event_id         cond_event_a5;
  cond_event_a5.id = rrc_event_id::event_id_t::a5;
  cond_event_a5.meas_trigger_quant_thres_or_offset.emplace();
  cond_event_a5.meas_trigger_quant_thres_or_offset.value().rsrp = 10;
  cond_event_a5.meas_trigger_quant_thres_2.emplace();
  cond_event_a5.meas_trigger_quant_thres_2.value().rsrp = 20;
  cond_event_a5.hysteresis                              = 0;
  cond_event_a5.time_to_trigger                         = 100;
  cond_trigger.cond_event_id                            = cond_event_a5;
  cond_trigger.rs_type                                  = rrc_nr_rs_type::ssb;
  cho_trigger_cfg                                       = cond_trigger;
  cfg.report_config_ids.emplace(uint_to_report_cfg_id(1), cho_trigger_cfg);

  manager = std::make_unique<cell_meas_manager>(cfg, mobility_manager, ue_mng);
  ASSERT_NE(manager, nullptr);
}
