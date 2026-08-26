// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/du/du_high/du_manager/converters/asn1_sys_info_packer.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/pcap/mac_pcap.h"
#include "ocudu/ran/tac.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

// Global variables for PCAP support, required by du_manager_converters.
bool             g_enable_pcap = false;
ocudu::mac_pcap* g_pcap        = nullptr;

/// \note Takes the result as an out-parameter so that the unpack can be checked with ASSERT_EQ. Wrapping it in
/// ocudu_assert would drop the call entirely in a build with asserts disabled.
static void pack_and_unpack_sib1(const du_cell_config& cell_cfg, asn1::rrc_nr::sib1_s& sib1)
{
  byte_buffer    buf = asn1_packer::pack_sib1(cell_cfg);
  asn1::cbit_ref bref{buf};
  ASSERT_EQ(sib1.unpack(bref), asn1::OCUDUASN_SUCCESS);
}

/// A cell broadcasting \c tac_list_size TACs, led by \c du_cell_config::tac.
static du_cell_config make_cell_config_with_tac_list(unsigned tac_list_size)
{
  du_cell_config cfg = config_helpers::make_default_du_cell_config();

  cfg.tac = 7;
  cfg.tac_list.push_back(cfg.tac);
  for (unsigned i = 1; i != tac_list_size; ++i) {
    cfg.tac_list.push_back(cfg.tac + i);
  }

  return cfg;
}

TEST(asn1_sib1_tac_list_test, terrestrial_cell_broadcasts_tracking_area_code_and_no_list)
{
  du_cell_config cell_cfg = config_helpers::make_default_du_cell_config();
  ASSERT_TRUE(cell_cfg.tac_list.empty()) << "A default cell must not broadcast a TAC list";

  asn1::rrc_nr::sib1_s sib1;
  pack_and_unpack_sib1(cell_cfg, sib1);

  ASSERT_EQ(sib1.cell_access_related_info.plmn_id_info_list.size(), 1);
  const auto& plmn_info = sib1.cell_access_related_info.plmn_id_info_list[0];

  EXPECT_TRUE(plmn_info.tac_present);
  EXPECT_EQ(plmn_info.tac.to_number(), cell_cfg.tac);
  EXPECT_FALSE(plmn_info.tracking_area_list_r17.is_present());
}

TEST(asn1_sib1_tac_list_test, ntn_cell_broadcasts_tracking_area_list_and_omits_tracking_area_code)
{
  du_cell_config cell_cfg = make_cell_config_with_tac_list(3);

  asn1::rrc_nr::sib1_s sib1;
  pack_and_unpack_sib1(cell_cfg, sib1);

  ASSERT_EQ(sib1.cell_access_related_info.plmn_id_info_list.size(), 1);
  const auto& plmn_info = sib1.cell_access_related_info.plmn_id_info_list[0];

  // TS 38.331: trackingAreaList and trackingAreaCode are mutually exclusive.
  EXPECT_FALSE(plmn_info.tac_present) << "trackingAreaCode must be absent when trackingAreaList is broadcast";

  ASSERT_TRUE(plmn_info.tracking_area_list_r17.is_present())
      << "trackingAreaList did not survive the pack/unpack round trip";
  ASSERT_EQ(plmn_info.tracking_area_list_r17->size(), cell_cfg.tac_list.size());
  for (unsigned i = 0, e = cell_cfg.tac_list.size(); i != e; ++i) {
    EXPECT_EQ((*plmn_info.tracking_area_list_r17)[i].to_number(), cell_cfg.tac_list[i]);
  }
}

TEST(asn1_sib1_tac_list_test, tracking_area_list_supports_the_maximum_number_of_tacs)
{
  du_cell_config cell_cfg = make_cell_config_with_tac_list(MAX_NOF_TACS_NTN);

  asn1::rrc_nr::sib1_s sib1;
  pack_and_unpack_sib1(cell_cfg, sib1);

  const auto& plmn_info = sib1.cell_access_related_info.plmn_id_info_list[0];
  ASSERT_TRUE(plmn_info.tracking_area_list_r17.is_present());
  EXPECT_EQ(plmn_info.tracking_area_list_r17->size(), MAX_NOF_TACS_NTN);
  EXPECT_FALSE(plmn_info.tac_present);
}
