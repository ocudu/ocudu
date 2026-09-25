// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Tests that check the transmission of Paging messages by the DU-high class.

#include "lib/f1ap/f1ap_asn1_packer.h"
#include "tests/integrationtests/du_high/test_utils/du_high_env_simulator.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "tests/unittests/gateways/test_helpers.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"
#include "ocudu/asn1/rrc_nr/bcch_dl_sch_msg.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/f1ap/f1ap_message.h"
#include "ocudu/ran/pcch/paging_helper.h"

using namespace ocudu;
using namespace odu;
using namespace asn1::f1ap;

static bool is_edrx_enabled(const asn1::rrc_nr::sib1_s& sib1)
{
  if (not sib1.non_crit_ext_present or not sib1.non_crit_ext.non_crit_ext_present or
      not sib1.non_crit_ext.non_crit_ext.non_crit_ext_present) {
    return false;
  }
  if (not sib1.non_crit_ext.non_crit_ext.non_crit_ext.hyper_sfn_r17_present or
      not sib1.non_crit_ext.non_crit_ext.non_crit_ext.edrx_allowed_idle_r17_present) {
    return false;
  }
  return true;
}

static std::optional<uint16_t> get_hyper_sfn(const asn1::rrc_nr::sib1_s& sib1)
{
  if (not is_edrx_enabled(sib1)) {
    return false;
  }
  return sib1.non_crit_ext.non_crit_ext.non_crit_ext.hyper_sfn_r17.to_number();
}

static bool is_edrx_enabled(const byte_buffer& packed_sib1)
{
  asn1::rrc_nr::sib1_s sib1;
  {
    asn1::cbit_ref bref{packed_sib1};
    auto           unpack_result = sib1.unpack(bref);
    ocudu_assert(unpack_result == asn1::OCUDUASN_SUCCESS, "Failed to decode SIB1");
  }
  return is_edrx_enabled(sib1);
}

static std::optional<uint16_t> extract_bcch_dl_sch_msg_hyper_sfn(span<const uint8_t> pdu)
{
  asn1::rrc_nr::bcch_dl_sch_msg_s msg;
  {
    auto           buf = byte_buffer::create(pdu).value();
    asn1::cbit_ref bref{buf};
    auto           unpack_result = msg.unpack(bref);
    ocudu_assert(unpack_result == asn1::OCUDUASN_SUCCESS, "Failed to decode SIB1");
  }
  ocudu_assert(msg.msg.type().value == asn1::rrc_nr::bcch_dl_sch_msg_type_c::types_opts::c1,
               "Invalid BCCH-DL SCH type");
  ocudu_assert(msg.msg.c1().type().value == asn1::rrc_nr::bcch_dl_sch_msg_type_c::c1_c_::types_opts::sib_type1,
               "Unexpected type for SIB1");
  return get_hyper_sfn(msg.msg.c1().sib_type1());
}

class paging_tester : public du_high_env_simulator, public testing::Test
{};

static f1ap_message generate_paging_message(uint64_t five_g_tmsi, const nr_cell_global_id_t& nr_cgi)
{
  f1ap_message msg;
  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_PAGING);
  paging_s& paging = msg.pdu.init_msg().value.paging();

  // Add ue id idx value.
  // UE Identity Index value is defined as: UE_ID 5G-S-TMSI mod 1024  (see TS 38.304 section 7.1).
  paging->ue_id_idx_value.set_idx_len10().from_number(five_g_tmsi % 1024);

  // Add paging id.
  paging->paging_id.set_cn_ue_paging_id().set_five_g_s_tmsi().from_number(five_g_tmsi);

  // Add paging DRX.
  paging->paging_drx_present = true;
  paging->paging_drx         = paging_drx_opts::v32;

  // Add paging cell list.
  asn1::protocol_ie_single_container_s<asn1::f1ap::paging_cell_item_ies_o> asn1_paging_cell_item_container;
  auto& asn1_paging_cell_item = asn1_paging_cell_item_container->paging_cell_item();
  asn1_paging_cell_item.nr_cgi.nr_cell_id.from_number(nr_cgi.nci.value());
  asn1_paging_cell_item.nr_cgi.plmn_id = nr_cgi.plmn_id.to_bytes();
  paging->paging_cell_list.push_back(asn1_paging_cell_item_container);

  return msg;
}

TEST_F(paging_tester, when_paging_message_is_received_its_relayed_to_ue)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BAS-16-1");

  static constexpr uint64_t five_g_tmsi = 0x01011066fef7;

  // Check F1 Setup.
  ASSERT_TRUE(test_helpers::is_f1_setup_request_valid(cu_notifier.f1ap_ul_msgs.rbegin()->second));
  auto& f1_setup_req = cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().value.f1_setup_request();
  ASSERT_TRUE(f1_setup_req->gnb_du_served_cells_list_present);
  auto& cell = f1_setup_req->gnb_du_served_cells_list[0].value().gnb_du_served_cells_item();
  ASSERT_TRUE(cell.gnb_du_sys_info_present);
  ASSERT_FALSE(is_edrx_enabled(cell.gnb_du_sys_info.sib1_msg));

  // Receive F1AP paging message.
  cu_notifier.f1ap_ul_msgs.clear();
  const auto& du_cell_cfg = this->du_high_cfg.ran.cells[0];
  this->du_hi->get_f1ap_pdu_handler().handle_message(generate_paging_message(five_g_tmsi, du_cell_cfg.nr_cgi));
  // Flag indicating whether UE is Paged or not.
  bool ue_is_paged{false};

  const unsigned MAX_COUNT = 50 * this->next_slot.nof_slots_per_frame();
  for (unsigned i = 0; i != MAX_COUNT; ++i) {
    this->run_slot();

    for (const auto& pg_grant : this->phy.cells[0].last_dl_res->dl_res->paging_grants) {
      const auto& pg_ue_it =
          std::find_if(pg_grant.paging_ue_list.begin(), pg_grant.paging_ue_list.end(), [](const paging_ue_info& ue) {
            return ue.paging_type_indicator == ocudu::paging_ue_info::cn_ue_paging_identity and
                   ue.paging_identity == five_g_tmsi;
          });
      if (pg_ue_it != pg_grant.paging_ue_list.end()) {
        ue_is_paged = true;
        break;
      }
    }
  }
  ASSERT_TRUE(ue_is_paged);
}

class edrx_paging_test : public du_high_env_simulator, public testing::Test
{
protected:
  edrx_paging_test() :
    du_high_env_simulator([]() {
      auto cfg                                          = create_du_high_configuration(du_high_env_sim_params{});
      cfg.ran.cells[0].ran.init_bwp.paging.edrx_enabled = true;
      return cfg;
    }())
  {
    OCUDU_TEST_REQUIREMENTS("DU-GEN-10");
  }
};

TEST_F(edrx_paging_test, when_edrx_enabled_then_sib1_contains_hyper_sfn)
{
  ASSERT_TRUE(test_helpers::is_f1_setup_request_valid(cu_notifier.f1ap_ul_msgs.rbegin()->second));
  auto& f1_setup_req = cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().value.f1_setup_request();
  ASSERT_TRUE(f1_setup_req->gnb_du_served_cells_list_present);
  auto& cell = f1_setup_req->gnb_du_served_cells_list[0].value().gnb_du_served_cells_item();
  ASSERT_TRUE(cell.gnb_du_sys_info_present);
  ASSERT_TRUE(is_edrx_enabled(cell.gnb_du_sys_info.sib1_msg));
}

TEST_F(edrx_paging_test, when_edrx_enabled_then_hypersfn_is_updated_in_sib1)
{
  const unsigned MAX_COUNT = 4 * 16 * this->next_slot.nof_slots_per_frame();

  unsigned sib1_count = 0;
  for (unsigned i = 0; i != MAX_COUNT; ++i) {
    this->run_slot();

    if (this->phy.cells[0].last_dl_res.has_value() and not this->phy.cells[0].last_dl_res->dl_res->bc.sibs.empty()) {
      auto& sibs = this->phy.cells[0].last_dl_res->dl_res->bc.sibs;
      for (const auto& sib : sibs) {
        if (sib.si_indicator == sib_information::sib1) {
          sib1_count++;
          unsigned idx = &sib - sibs.data();
          ASSERT_TRUE(this->phy.cells[0].last_dl_data.has_value());
          ASSERT_LT(idx, this->phy.cells[0].last_dl_data->si_pdus.size());
          span<const uint8_t> pdu       = this->phy.cells[0].last_dl_data->si_pdus[idx].pdu.get_buffer();
          auto                hyper_sfn = extract_bcch_dl_sch_msg_hyper_sfn(pdu);
          ASSERT_TRUE(hyper_sfn.has_value()) << "eDRX not enabled in SIB1";
          auto cur_hypersfn = (this->next_slot - 1).hyper_sfn();
          ASSERT_EQ(cur_hypersfn, *hyper_sfn)
              << fmt::format("HyperSFNs don't match ({} != {})", cur_hypersfn, *hyper_sfn);
        }
      }
    }
  }

  ASSERT_EQ(sib1_count, 4);
}

TEST_F(edrx_paging_test, when_f1_edrx_paging_is_received_then_it_is_sent_to_lower_layers)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BAS-16-1");

  const uint64_t five_g_tmsi = test_rng::uniform_int<uint64_t>() & mask_lsb_ones<uint64_t>(48);
  // Note: We set a Paging Time Window that is almost the length of the full eDRX cycle, to minimize the duration
  /// of this test.
  const hyper_frames edrx_cycle{2};
  /// PTW length == paging_win_coeff * 1.28sec
  const unsigned     paging_win_coeff = 15;
  const radio_frames ptw_len          = paging_win_coeff * radio_frames{128};

  // Receive F1AP paging message.
  // Note: We set the PTW close enough to current slot, to minimize the duration of the test.
  cu_notifier.f1ap_ul_msgs.clear();
  const auto&  du_cell_cfg            = this->du_high_cfg.ran.cells[0];
  f1ap_message msg                    = generate_paging_message(five_g_tmsi, du_cell_cfg.nr_cgi);
  auto&        paging                 = msg.pdu.init_msg().value.paging();
  paging->nr_paginge_drx_info_present = true;
  bool success                        = asn1::float_number_to_enum(
      paging->nr_paginge_drx_info.nrpaging_e_drx_cycle_idle, static_cast<double>(edrx_cycle.count()), 0.001);
  ocudu_assert(success, "Invalid conversion");
  paging->nr_paginge_drx_info.nrpaging_time_win_present = true;
  success = asn1::number_to_enum(paging->nr_paginge_drx_info.nrpaging_time_win, paging_win_coeff);
  ocudu_assert(success, "Invalid conversion");
  this->du_hi->get_f1ap_pdu_handler().handle_message(msg);

  // Maximum time that can be observed without paging (accounts for the eDRX PTW, DRX SFN, and the paging scheduler's
  // look-ahead offset).
  const radio_frames forbid_edrx = edrx_cycle - ptw_len;
  const unsigned     MAX_SLOT_COUNT =
      (forbid_edrx.count() + paging->paging_drx.to_number() + 1) * next_slot.nof_slots_per_frame();
  bool found = false;
  for (unsigned i = 0; i != MAX_SLOT_COUNT and not found; ++i) {
    this->run_slot();
    if (this->phy.cells[0].last_dl_res.has_value() and
        not this->phy.cells[0].last_dl_res->dl_res->paging_grants.empty()) {
      for (const auto& grant : this->phy.cells[0].last_dl_res->dl_res->paging_grants) {
        ASSERT_FALSE(grant.paging_ue_list.empty());
        for (const auto& ue_pg : grant.paging_ue_list) {
          ASSERT_EQ(ue_pg.paging_type_indicator, paging_ue_info::cn_ue_paging_identity);
          ASSERT_EQ(ue_pg.paging_identity, five_g_tmsi);
          found = true;
          break;
        }
      }
    }
  }
  ASSERT_TRUE(found);
}
