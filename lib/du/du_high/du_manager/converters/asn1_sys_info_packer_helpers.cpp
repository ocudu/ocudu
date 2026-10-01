// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "asn1_sys_info_packer_helpers.h"

using namespace ocudu;

asn1::rrc_nr::sib2_s::cell_resel_info_common_s_::q_hyst_opts::options
asn1_utils::make_asn1_rrc_q_hyst(const q_hyst_t& q_hyst)
{
  using namespace asn1::rrc_nr;
  switch (q_hyst) {
    case ocudu::q_hyst_t::db0:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db0;
    case ocudu::q_hyst_t::db1:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db1;
    case ocudu::q_hyst_t::db2:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db2;
    case ocudu::q_hyst_t::db3:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db3;
    case ocudu::q_hyst_t::db4:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db4;
    case ocudu::q_hyst_t::db5:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db5;
    case ocudu::q_hyst_t::db6:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db6;
    case ocudu::q_hyst_t::db8:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db8;
    case ocudu::q_hyst_t::db10:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db10;
    case ocudu::q_hyst_t::db12:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db12;
    case ocudu::q_hyst_t::db14:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db14;
    case ocudu::q_hyst_t::db16:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db16;
    case ocudu::q_hyst_t::db18:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db18;
    case ocudu::q_hyst_t::db20:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db20;
    case ocudu::q_hyst_t::db22:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db22;
    case ocudu::q_hyst_t::db24:
      return sib2_s::cell_resel_info_common_s_::q_hyst_opts::db24;
    default:
      report_fatal_error("Invalid Qhyst value");
  }
}

asn1::rrc_nr::q_offset_range_opts::options
asn1_utils::make_asn1_rrc_q_offset_range(const q_offset_range_t& q_offset_range)
{
  switch (q_offset_range) {
    case ocudu::q_offset_range_t::db_24:
      return asn1::rrc_nr::q_offset_range_opts::db_neg24;
    case ocudu::q_offset_range_t::db_22:
      return asn1::rrc_nr::q_offset_range_opts::db_neg22;
    case ocudu::q_offset_range_t::db_20:
      return asn1::rrc_nr::q_offset_range_opts::db_neg20;
    case ocudu::q_offset_range_t::db_18:
      return asn1::rrc_nr::q_offset_range_opts::db_neg18;
    case ocudu::q_offset_range_t::db_16:
      return asn1::rrc_nr::q_offset_range_opts::db_neg16;
    case ocudu::q_offset_range_t::db_14:
      return asn1::rrc_nr::q_offset_range_opts::db_neg14;
    case ocudu::q_offset_range_t::db_12:
      return asn1::rrc_nr::q_offset_range_opts::db_neg12;
    case ocudu::q_offset_range_t::db_10:
      return asn1::rrc_nr::q_offset_range_opts::db_neg10;
    case ocudu::q_offset_range_t::db_8:
      return asn1::rrc_nr::q_offset_range_opts::db_neg8;
    case ocudu::q_offset_range_t::db_6:
      return asn1::rrc_nr::q_offset_range_opts::db_neg6;
    case ocudu::q_offset_range_t::db_5:
      return asn1::rrc_nr::q_offset_range_opts::db_neg5;
    case ocudu::q_offset_range_t::db_4:
      return asn1::rrc_nr::q_offset_range_opts::db_neg4;
    case ocudu::q_offset_range_t::db_3:
      return asn1::rrc_nr::q_offset_range_opts::db_neg3;
    case ocudu::q_offset_range_t::db_2:
      return asn1::rrc_nr::q_offset_range_opts::db_neg2;
    case ocudu::q_offset_range_t::db_1:
      return asn1::rrc_nr::q_offset_range_opts::db_neg1;
    case ocudu::q_offset_range_t::db0:
      return asn1::rrc_nr::q_offset_range_opts::db0;
    case ocudu::q_offset_range_t::db1:
      return asn1::rrc_nr::q_offset_range_opts::db1;
    case ocudu::q_offset_range_t::db2:
      return asn1::rrc_nr::q_offset_range_opts::db2;
    case ocudu::q_offset_range_t::db3:
      return asn1::rrc_nr::q_offset_range_opts::db3;
    case ocudu::q_offset_range_t::db4:
      return asn1::rrc_nr::q_offset_range_opts::db4;
    case ocudu::q_offset_range_t::db5:
      return asn1::rrc_nr::q_offset_range_opts::db5;
    case ocudu::q_offset_range_t::db6:
      return asn1::rrc_nr::q_offset_range_opts::db6;
    case ocudu::q_offset_range_t::db8:
      return asn1::rrc_nr::q_offset_range_opts::db8;
    case ocudu::q_offset_range_t::db10:
      return asn1::rrc_nr::q_offset_range_opts::db10;
    case ocudu::q_offset_range_t::db12:
      return asn1::rrc_nr::q_offset_range_opts::db12;
    case ocudu::q_offset_range_t::db14:
      return asn1::rrc_nr::q_offset_range_opts::db14;
    case ocudu::q_offset_range_t::db16:
      return asn1::rrc_nr::q_offset_range_opts::db16;
    case ocudu::q_offset_range_t::db18:
      return asn1::rrc_nr::q_offset_range_opts::db18;
    case ocudu::q_offset_range_t::db20:
      return asn1::rrc_nr::q_offset_range_opts::db20;
    case ocudu::q_offset_range_t::db22:
      return asn1::rrc_nr::q_offset_range_opts::db22;
    case ocudu::q_offset_range_t::db24:
      return asn1::rrc_nr::q_offset_range_opts::db24;
    default:
      report_fatal_error("Invalid QoffsetRange value");
  }
}

asn1::rrc_nr::pci_range_s asn1_utils::make_asn1_rrc_pci_range(const pci_range_t& pci_range)
{
  asn1::rrc_nr::pci_range_s asn1_range;
  asn1_range.start = pci_range.start;
  switch (pci_range.size) {
    case pci_range_t::range_t::n1:
      return asn1_range;
    case pci_range_t::range_t::n4:
      asn1_range.range.value = asn1::rrc_nr::pci_range_s::range_opts::n4;
      break;
    case pci_range_t::range_t::n8:
      asn1_range.range.value = asn1::rrc_nr::pci_range_s::range_opts::n8;
      break;
    case pci_range_t::range_t::n12:
      asn1_range.range.value = asn1::rrc_nr::pci_range_s::range_opts::n12;
      break;
    case pci_range_t::range_t::n16:
      asn1_range.range.value = asn1::rrc_nr::pci_range_s::range_opts::n16;
      break;
    case pci_range_t::range_t::n24:
      asn1_range.range.value = asn1::rrc_nr::pci_range_s::range_opts::n24;
      break;
    case pci_range_t::range_t::n32:
      asn1_range.range.value = asn1::rrc_nr::pci_range_s::range_opts::n32;
      break;
    case pci_range_t::range_t::n48:
      asn1_range.range.value = asn1::rrc_nr::pci_range_s::range_opts::n48;
      break;
    case pci_range_t::range_t::n64:
      asn1_range.range.value = asn1::rrc_nr::pci_range_s::range_opts::n64;
      break;
    case pci_range_t::range_t::n84:
      asn1_range.range.value = asn1::rrc_nr::pci_range_s::range_opts::n84;
      break;
    case pci_range_t::range_t::n96:
      asn1_range.range.value = asn1::rrc_nr::pci_range_s::range_opts::n96;
      break;
    case pci_range_t::range_t::n128:
      asn1_range.range.value = asn1::rrc_nr::pci_range_s::range_opts::n128;
      break;
    case pci_range_t::range_t::n168:
      asn1_range.range.value = asn1::rrc_nr::pci_range_s::range_opts::n168;
      break;
    case pci_range_t::range_t::n252:
      asn1_range.range.value = asn1::rrc_nr::pci_range_s::range_opts::n252;
      break;
    case pci_range_t::range_t::n504:
      asn1_range.range.value = asn1::rrc_nr::pci_range_s::range_opts::n504;
      break;
    case pci_range_t::range_t::n1008:
      asn1_range.range.value = asn1::rrc_nr::pci_range_s::range_opts::n1008;
      break;
    default:
      report_fatal_error("Invalid PCI range size");
  }
  asn1_range.range_present = true;
  return asn1_range;
}

asn1::rrc_nr::eutra_allowed_meas_bw_opts::options
asn1_utils::make_asn1_rrc_allowed_meas_bw(const eutra_allowed_meas_bandwidth_t& allowed_beas_bw)
{
  switch (allowed_beas_bw) {
    case eutra_allowed_meas_bandwidth_t::mbw6:
      return asn1::rrc_nr::eutra_allowed_meas_bw_opts::mbw6;
    case eutra_allowed_meas_bandwidth_t::mbw15:
      return asn1::rrc_nr::eutra_allowed_meas_bw_opts::mbw15;
    case eutra_allowed_meas_bandwidth_t::mbw25:
      return asn1::rrc_nr::eutra_allowed_meas_bw_opts::mbw25;
    case eutra_allowed_meas_bandwidth_t::mbw50:
      return asn1::rrc_nr::eutra_allowed_meas_bw_opts::mbw50;
    case eutra_allowed_meas_bandwidth_t::mbw75:
      return asn1::rrc_nr::eutra_allowed_meas_bw_opts::mbw75;
    case eutra_allowed_meas_bandwidth_t::mbw100:
      return asn1::rrc_nr::eutra_allowed_meas_bw_opts::mbw100;
    default:
      report_fatal_error("Invalid allowed measurement bandwidth value");
  }
}
