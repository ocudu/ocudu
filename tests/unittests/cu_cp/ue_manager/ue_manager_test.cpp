// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/unittests/cu_cp/test_helpers.h"
#include "ue_manager_test_helpers.h"
#include "ocudu/ran/plmn_identity.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

//////////////////////////////////////////////////////////////////////////////////////
/* DU UE                                                                            */
//////////////////////////////////////////////////////////////////////////////////////

/// Test allocation of multiple UE indexes.
TEST_F(ue_manager_test, when_multiple_ue_indexes_allocated_then_ue_indexes_valid)
{
  cu_cp_du_index_t du_index = cu_cp_du_index_t::min;

  // Reduce log level to avoid flooding the log.
  ue_mng_logger.set_level(ocudulog::basic_levels::warning);
  test_logger.set_level(ocudulog::basic_levels::warning);

  for (unsigned it = 0; it < cu_cp_cfg.admission.max_nof_ues; it++) {
    // Check that the ue index is valid.
    ASSERT_NE(ue_mng.add_ue(du_index), cu_cp_ue_index_t::invalid);
    ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  }
}

/// Test allocation of unsupported number of UE indexes.
TEST_F(ue_manager_test, when_more_than_max_ue_created_then_ue_is_not_servable)
{
  cu_cp_du_index_t du_index = cu_cp_du_index_t::min;

  // Reduce log level to avoid flooding the log.
  ue_mng_logger.set_level(ocudulog::basic_levels::warning);
  test_logger.set_level(ocudulog::basic_levels::warning);

  for (unsigned it = 0; it < cu_cp_cfg.admission.max_nof_ues; it++) {
    // Check that the ue index is valid
    cu_cp_ue_index_t ue_index = ue_mng.add_ue(du_index);
    ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
    ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  }

  // reset log level
  ue_mng_logger.set_level(ocudulog::basic_levels::debug);
  test_logger.set_level(ocudulog::basic_levels::debug);

  // Allocate additional ue index
  ASSERT_NE(ue_mng.add_ue(du_index), cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.ue_admission_limit_reached());
}

/// Test successful creation of a DU UE.
TEST_F(ue_manager_test, when_valid_du_context_added_at_creation_then_ue_added)
{
  cu_cp_du_index_t du_index    = cu_cp_du_index_t::min;
  rnti_t           rnti        = to_rnti(0x4601);
  du_cell_index_t  pcell_index = MIN_DU_CELL_INDEX;
  cu_cp_ue_index_t ue_index    = ue_mng.add_ue(du_index);
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.update_ue_context(ue_index, gnb_du_id_t::min, MIN_PCI, rnti, pcell_index));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  auto* ue = ue_mng.find_ue(ue_index);

  // Check that the UE has been created.
  ASSERT_NE(ue, nullptr);
  ASSERT_NE(ue_mng.find_ue(ue->get_ue_index()), nullptr);

  // Check that the UE index is valid.
  ASSERT_NE(ue->get_ue_index(), cu_cp_ue_index_t::invalid);

  // Check that the gNB-DU ID has been set.
  ASSERT_NE(ue->get_du_id(), gnb_du_id_t::invalid);

  // Check that the PCI has been set.
  ASSERT_EQ(ue->get_pci(), MIN_PCI);

  // Check that the RNTI has been set.
  ASSERT_EQ(ue->get_c_rnti(), rnti);

  // Check that the Pcell index has been set.
  ASSERT_EQ(ue->get_pcell_index(), pcell_index);

  // Check that the lookup by PCI and RNTI works.
  ASSERT_EQ(ue->get_ue_index(), ue_mng.get_ue_index(ue->get_pci(), ue->get_c_rnti()));

  // Check that the number of DU UEs is 1.
  ASSERT_EQ(ue_mng.get_nof_du_ues(du_index), 1U);
}

/// Test successful creation of a DU UE.
TEST_F(ue_manager_test, when_du_context_valid_then_ue_updated)
{
  cu_cp_du_index_t du_index = cu_cp_du_index_t::min;

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(du_index);
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());

  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  rnti_t          rnti        = to_rnti(0x4601);
  du_cell_index_t pcell_index = MIN_DU_CELL_INDEX;
  ASSERT_TRUE(ue_mng.update_ue_context(ue_index, gnb_du_id_t::min, MIN_PCI, rnti, pcell_index));

  auto* ue = ue_mng.find_ue(ue_index);

  // Check that the UE has been created.
  ASSERT_NE(ue, nullptr);
  ASSERT_NE(ue_mng.find_ue(ue->get_ue_index()), nullptr);

  // Check that the UE index is valid.
  ASSERT_NE(ue->get_ue_index(), cu_cp_ue_index_t::invalid);

  // Check that the gNB-DU ID has been set.
  ASSERT_NE(ue->get_du_id(), gnb_du_id_t::invalid);

  // Check that the PCI has been set.
  ASSERT_EQ(ue->get_pci(), MIN_PCI);

  // Check that the RNTI has been set.
  ASSERT_EQ(ue->get_c_rnti(), rnti);

  // Check that the Pcell index has been set.
  ASSERT_EQ(ue->get_pcell_index(), pcell_index);

  // Check that the lookup by PCI and RNTI works.
  ASSERT_EQ(ue->get_ue_index(), ue_mng.get_ue_index(ue->get_pci(), ue->get_c_rnti()));

  // Check that the number of DU UEs is 1.
  ASSERT_EQ(ue_mng.get_nof_du_ues(du_index), 1U);
}

/// Test finding invalid UE index.
TEST_F(ue_manager_test, when_ue_index_invalid_then_ue_not_found)
{
  cu_cp_du_index_t du_index    = cu_cp_du_index_t::min;
  rnti_t           rnti        = to_rnti(0x4601);
  du_cell_index_t  pcell_index = MIN_DU_CELL_INDEX;
  cu_cp_ue_index_t ue_index    = ue_mng.add_ue(du_index);
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.update_ue_context(ue_index, gnb_du_id_t::min, MIN_PCI, rnti, pcell_index));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  auto* ue = ue_mng.find_ue(ue_index);

  // Check that the UE has been created.
  ASSERT_NE(ue, nullptr);

  // Check that ue with invalid UE index is not found.
  ASSERT_EQ(ue_mng.find_ue(cu_cp_ue_index_t::invalid), nullptr);
}

/// Test duplicate UE creation.
TEST_F(ue_manager_test, when_rnti_already_exits_then_ue_not_added)
{
  cu_cp_du_index_t du_index    = cu_cp_du_index_t::min;
  rnti_t           rnti        = to_rnti(0x4601);
  du_cell_index_t  pcell_index = MIN_DU_CELL_INDEX;
  cu_cp_ue_index_t ue_index    = ue_mng.add_ue(du_index);
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.update_ue_context(ue_index, gnb_du_id_t::min, MIN_PCI, rnti, pcell_index));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));

  // Check that the number of DU UEs is 1.
  ASSERT_EQ(ue_mng.get_nof_du_ues(du_index), 1U);

  // Check that the same UE cannot be added again.
  ASSERT_FALSE(ue_mng.update_ue_context(ue_index, gnb_du_id_t::min, MIN_PCI, rnti, pcell_index));

  // Check that the UE has not been added.
  ASSERT_EQ(ue_mng.get_nof_du_ues(du_index), 1U);
}

/// Test successful removal of a DU UE
TEST_F(ue_manager_test, when_ue_exists_then_removal_successful)
{
  cu_cp_du_index_t du_index    = cu_cp_du_index_t::min;
  rnti_t           rnti        = to_rnti(0x4601);
  du_cell_index_t  pcell_index = MIN_DU_CELL_INDEX;
  cu_cp_ue_index_t ue_index    = ue_mng.add_ue(du_index);
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.update_ue_context(ue_index, gnb_du_id_t::min, MIN_PCI, rnti, pcell_index));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  auto* ue = ue_mng.find_ue(ue_index);

  ue_mng.remove_ue(ue->get_ue_index());

  // Check that the UE has been removed.
  ASSERT_EQ(ue_mng.get_nof_du_ues(du_index), 0U);
}

/// Test creation of multiple DU UEs.
TEST_F(ue_manager_test, when_multiple_ues_added_then_ues_exist)
{
  cu_cp_du_index_t du_index    = cu_cp_du_index_t::min;
  du_cell_index_t  pcell_index = MIN_DU_CELL_INDEX;

  // Reduce log level to avoid flooding the log.
  ue_mng_logger.set_level(ocudulog::basic_levels::warning);
  test_logger.set_level(ocudulog::basic_levels::warning);

  for (unsigned it = to_value(rnti_t::MIN_CRNTI);
       it < unsigned(to_value(rnti_t::MIN_CRNTI) + cu_cp_cfg.admission.max_nof_ues);
       it++) {
    rnti_t           rnti     = to_rnti(it);
    cu_cp_ue_index_t ue_index = ue_mng.add_ue(du_index);
    ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
    ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
    ASSERT_TRUE(ue_mng.update_ue_context(ue_index, gnb_du_id_t::min, MIN_PCI, rnti, pcell_index));
    ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
    auto* ue = ue_mng.find_ue(ue_index);

    // Check that the UE has been created.
    ASSERT_NE(ue, nullptr);
    ASSERT_NE(ue_mng.find_ue(ue->get_ue_index()), nullptr);

    // Check that the UE index is valid.
    ASSERT_NE(ue->get_ue_index(), cu_cp_ue_index_t::invalid);

    // Check that the PCI has been set.
    ASSERT_EQ(ue->get_pci(), MIN_PCI);

    // Check that the RNTI has been set.
    ASSERT_EQ(ue->get_c_rnti(), rnti);

    // Check that the lookup by PCI and  RNTI works.
    ASSERT_EQ(ue->get_ue_index(), ue_mng.get_ue_index(ue->get_pci(), ue->get_c_rnti()));

    // Check that the number of DU UEs is increased.
    ASSERT_EQ(ue_mng.get_nof_du_ues(du_index), it - to_value(rnti_t::MIN_CRNTI) + 1);
  }

  // Reset log level.
  ue_mng_logger.set_level(ocudulog::basic_levels::debug);

  // Check that the maximum number of DU UEs has been reached.
  ASSERT_EQ(ue_mng.get_nof_du_ues(du_index), cu_cp_cfg.admission.max_nof_ues);
}

/// Test creation of an unservable UE once the admission limit is reached.
TEST_F(ue_manager_test, when_more_than_max_ues_added_then_ue_created_but_not_servable)
{
  cu_cp_du_index_t du_index    = cu_cp_du_index_t::min;
  du_cell_index_t  pcell_index = MIN_DU_CELL_INDEX;

  // Reduce log level to avoid flooding the log.
  ue_mng_logger.set_level(ocudulog::basic_levels::warning);
  test_logger.set_level(ocudulog::basic_levels::warning);

  for (unsigned it = to_value(rnti_t::MIN_CRNTI);
       it < unsigned(to_value(rnti_t::MIN_CRNTI) + cu_cp_cfg.admission.max_nof_ues);
       it++) {
    rnti_t           rnti     = to_rnti(it);
    cu_cp_ue_index_t ue_index = ue_mng.add_ue(du_index);
    ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
    ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
    ASSERT_TRUE(ue_mng.update_ue_context(ue_index, gnb_du_id_t::min, MIN_PCI, rnti, pcell_index));
    ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
    auto* ue = ue_mng.find_ue(ue_index);

    // Check that the UE has been created.
    ASSERT_NE(ue, nullptr);
    ASSERT_NE(ue_mng.find_ue(ue->get_ue_index()), nullptr);

    // Check that the UE index is valid.
    ASSERT_NE(ue->get_ue_index(), cu_cp_ue_index_t::invalid);

    // Check that the PCI has been set.
    ASSERT_EQ(ue->get_pci(), MIN_PCI);

    // Check that the RNTI has been set.
    ASSERT_EQ(ue->get_c_rnti(), rnti);

    // Check that the lookup by PCI and RNTI works.
    ASSERT_EQ(ue->get_ue_index(), ue_mng.get_ue_index(ue->get_pci(), ue->get_c_rnti()));

    // Check that the number of DU UEs is increased.
    ASSERT_EQ(ue_mng.get_nof_du_ues(du_index), it - to_value(rnti_t::MIN_CRNTI) + 1);
  }

  // Reset log level.
  ue_mng_logger.set_level(ocudulog::basic_levels::debug);
  test_logger.set_level(ocudulog::basic_levels::debug);

  // Check that the maximum number of DU UEs has been reached.
  ASSERT_EQ(ue_mng.get_nof_du_ues(du_index), cu_cp_cfg.admission.max_nof_ues);

  cu_cp_ue_index_t ue_index = ue_mng.add_ue(du_index);
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.ue_admission_limit_reached());

  // Check that the UE context has been created even though the UE is not servable.
  ASSERT_EQ(ue_mng.get_nof_du_ues(du_index), cu_cp_cfg.admission.max_nof_ues + 1);
}

/// Test inactive and i-rnti handling.
TEST_F(ue_manager_test, when_ue_is_set_inactive_then_i_rnti_returned)
{
  cu_cp_du_index_t du_index    = cu_cp_du_index_t::min;
  rnti_t           rnti        = to_rnti(0x4601);
  du_cell_index_t  pcell_index = MIN_DU_CELL_INDEX;
  cu_cp_ue_index_t ue_index    = ue_mng.add_ue(du_index);
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.update_ue_context(ue_index, gnb_du_id_t::min, MIN_PCI, rnti, pcell_index));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  auto* ue = ue_mng.find_ue(ue_index);

  dummy_rrc_ue rrc_ue;
  ue->set_rrc_ue(rrc_ue);

  std::optional<i_rntis_t> i_rntis = ue_mng.set_inactive(ue->get_ue_index());
  ASSERT_TRUE(i_rntis.has_value());

  // Check that the UE has not been removed.
  ASSERT_EQ(ue_mng.get_nof_du_ues(du_index), 1U);
}

TEST_F(ue_manager_test, when_ue_is_set_inactive_then_its_found_by_i_rnti)
{
  cu_cp_du_index_t du_index    = cu_cp_du_index_t::min;
  rnti_t           rnti        = to_rnti(0x4601);
  du_cell_index_t  pcell_index = MIN_DU_CELL_INDEX;
  cu_cp_ue_index_t ue_index    = ue_mng.add_ue(du_index);
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_FALSE(ue_mng.ue_admission_limit_reached());
  ASSERT_TRUE(ue_mng.update_ue_context(ue_index, gnb_du_id_t::min, MIN_PCI, rnti, pcell_index));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  auto* ue = ue_mng.find_ue(ue_index);

  dummy_rrc_ue rrc_ue;
  ue->set_rrc_ue(rrc_ue);

  std::optional<i_rntis_t> i_rntis = ue_mng.set_inactive(ue->get_ue_index());
  ASSERT_TRUE(i_rntis.has_value());
  ASSERT_TRUE(i_rntis->short_i_rnti.value() < i_rntis->short_i_rnti.max());
  ASSERT_TRUE(i_rntis->full_i_rnti.value() < i_rntis->full_i_rnti.max());

  // Find by full-i-rnti.
  ASSERT_NE(ue_mng.get_ue_index(i_rntis->full_i_rnti), cu_cp_ue_index_t::invalid);

  // Find by short-i-rnti.
  ASSERT_NE(ue_mng.get_ue_index(i_rntis->short_i_rnti), cu_cp_ue_index_t::invalid);
}

/// Test that removing a UE while it is RRC_INACTIVE (e.g. RNA update or RAN paging timeout, instead of a
/// successful RRC Resume) also cleans up its I-RNTI lookup entries, instead of leaking them.
TEST_F(ue_manager_test, when_inactive_ue_is_removed_then_i_rnti_lookups_are_cleared)
{
  cu_cp_du_index_t du_index    = cu_cp_du_index_t::min;
  rnti_t           rnti        = to_rnti(0x4601);
  du_cell_index_t  pcell_index = MIN_DU_CELL_INDEX;
  cu_cp_ue_index_t ue_index    = ue_mng.add_ue(du_index);
  ASSERT_NE(ue_index, cu_cp_ue_index_t::invalid);
  ASSERT_TRUE(ue_mng.update_ue_context(ue_index, gnb_du_id_t::min, MIN_PCI, rnti, pcell_index));
  ASSERT_TRUE(ue_mng.set_plmn(ue_index, plmn_identity::test_value()));
  auto* ue = ue_mng.find_ue(ue_index);

  dummy_rrc_ue rrc_ue;
  ue->set_rrc_ue(rrc_ue);

  std::optional<i_rntis_t> i_rntis = ue_mng.set_inactive(ue_index);
  ASSERT_TRUE(i_rntis.has_value());
  ASSERT_NE(ue_mng.get_ue_index(i_rntis->full_i_rnti), cu_cp_ue_index_t::invalid);
  ASSERT_NE(ue_mng.get_ue_index(i_rntis->short_i_rnti), cu_cp_ue_index_t::invalid);

  // UE never resumes (set_active() is never called) and is released directly, e.g. due to RNA update timeout.
  ue_mng.remove_ue(ue_index);

  // The I-RNTI lookup entries must not outlive the UE they pointed to.
  ASSERT_EQ(ue_mng.get_ue_index(i_rntis->full_i_rnti), cu_cp_ue_index_t::invalid);
  ASSERT_EQ(ue_mng.get_ue_index(i_rntis->short_i_rnti), cu_cp_ue_index_t::invalid);
}
