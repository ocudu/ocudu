// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/mac_cell_result.h"
#include "ocudu/scheduler/result/sched_result_grid.h"
#include <memory>

namespace ocudu {
namespace unittests {

/// Builds and returns a valid CORESET#0 configuration, which is always interleaved.
coreset_configuration build_valid_coreset0_configuration();

/// Builds and returns a valid CORESET configuration with a non-zero identifier, optionally with interleaved mapping.
coreset_configuration build_valid_non_coreset0_configuration(bool interleaved);

/// Helper struct to manage the pointer life cycle of a PDU that carries a PDSCH.
template <typename Pdu>
struct pdsch_pdu_test_helper {
  std::unique_ptr<bwp_configuration>     bwp_cfg;
  std::unique_ptr<coreset_configuration> coreset_cfg;
  Pdu                                    pdu;
};

using sib_information_test_helper      = pdsch_pdu_test_helper<sib_information>;
using rar_information_test_helper      = pdsch_pdu_test_helper<rar_information>;
using dl_paging_allocation_test_helper = pdsch_pdu_test_helper<dl_paging_allocation>;
using dl_msg_allocation_test_helper    = pdsch_pdu_test_helper<dl_msg_alloc>;

/// Builds and returns a valid SIB1 information PDU.
sib_information_test_helper build_valid_sib1_information_pdu();

/// Builds and returns a valid RAR information PDU.
rar_information_test_helper build_valid_rar_information_pdu();

/// Builds and returns a valid DL paging PDU.
dl_paging_allocation_test_helper build_valid_dl_paging_pdu();

/// Builds and returns a valid DL message allocation PDU.
dl_msg_allocation_test_helper build_valid_dl_msg_alloc_pdu();

/// Builds and returns a valid MAC SSB PDU.
dl_ssb_pdu build_valid_dl_ssb_pdu();

/// Helper struct to manage the pointer life cycle of a MAC DL scheduling result.
struct mac_dl_sched_result_test_helper {
  std::unique_ptr<std::array<bwp_configuration, 2>>                          bwp_cfg;
  std::unique_ptr<std::array<bwp_configuration, MAX_DL_PDCCH_PDUS_PER_SLOT>> bwp_cfg_pdcch;
  std::unique_ptr<std::array<coreset_configuration, 2>>                      coreset_cfg;
  sib_information_test_helper                                                sib1;
  rar_information_test_helper                                                rar;
  dl_paging_allocation_test_helper                                           dl_paging;
  dl_msg_allocation_test_helper                                              dl_msg_alloc;
  /// Grid that holds the result, so that its pooled PDUs come from a pool.
  std::unique_ptr<sched_result_grid> grid         = std::make_unique<sched_result_grid>(sched_result_grid_config{1});
  sched_slot_dl_result_builder       builder      = grid->get_builder(0).dl();
  dl_sched_result&                   sched_result = grid->slot_storage(0).dl;
  mac_dl_sched_result                result;
};

/// Helper struct to manage the pointer life cycle of a MAC DL data result.
struct mac_dl_data_result_test_helper {
  std::unique_ptr<std::array<uint8_t, 1>> data;
  mac_dl_data_result                      result;
};

/// Builds and returns a valid MAC data result with a single SIB1 PDU.
mac_dl_data_result_test_helper build_valid_mac_data_result();

/// Builds and returns a valid MAC data result with the maximum number of PDUs of each kind.
mac_dl_data_result_test_helper build_valid_mac_data_result_with_all_supported_pdu();

/// Builds and returns a valid PRACH occasion.
prach_occasion_info build_valid_prach_occasion();

/// Builds and returns a valid DL-PRS PDU.
prs_info build_valid_prs_pdu();

/// Helper struct to manage the pointer life cycle of a UL scheduling info.
struct ul_sched_info_test_helper {
  std::unique_ptr<bwp_configuration> bwp_cfg;
  ul_sched_info                      info;
};

/// Builds and returns a valid PUSCH PDU.
ul_sched_info_test_helper build_valid_pusch_pdu();

/// Helper struct to manage the pointer life cycle of an SRS info.
struct srs_info_helper {
  std::unique_ptr<bwp_configuration> bwp_cfg;
  srs_info                           pdu;
};

/// Builds and returns a valid SRS PDU.
srs_info_helper build_valid_srs_pdu();

/// Helper struct to manage the pointer life cycle of a PUCCH info.
struct pucch_info_test_helper {
  std::unique_ptr<bwp_configuration> bwp_cfg;
  std::unique_ptr<pucch_resource>    res;
  pucch_info                         info;
};

/// Builds and returns a valid PUCCH format 0 PDU.
pucch_info_test_helper build_valid_pucch_format_0_pdu();

/// Builds and returns a valid PUCCH format 1 PDU.
pucch_info_test_helper build_valid_pucch_format_1_pdu();

/// Builds and returns a valid PUCCH format 2 PDU.
pucch_info_test_helper build_valid_pucch_format_2_pdu();

/// Builds and returns a valid PUCCH format 3 PDU.
pucch_info_test_helper build_valid_pucch_format_3_pdu();

/// Builds and returns a valid PUCCH format 4 PDU.
pucch_info_test_helper build_valid_pucch_format_4_pdu();

/// Builds and returns a valid MAC DL sched result.
mac_dl_sched_result_test_helper build_valid_mac_dl_sched_result();

/// Builds and returns a valid MAC DL sched result with all the PDUs filled.
mac_dl_sched_result_test_helper build_valid_mac_dl_sched_result_with_all_supported_pdus();

/// Helper struct to manage the pointer life cycle of a MAC UL scheduling result.
struct mac_ul_sched_result_test_helper {
  pucch_info_test_helper    pucch_f1;
  pucch_info_test_helper    pucch_f2;
  ul_sched_info_test_helper pusch;
  srs_info_helper           srs;
  /// Grid that holds the result, so that its pooled PDUs come from a pool.
  std::unique_ptr<sched_result_grid> grid         = std::make_unique<sched_result_grid>(sched_result_grid_config{1});
  sched_slot_ul_result_builder       builder      = grid->get_builder(0).ul();
  ul_sched_result&                   sched_result = grid->slot_storage(0).ul;
  mac_ul_sched_result                result;
};

/// Builds and returns a valid MAC UL sched result.
mac_ul_sched_result_test_helper build_valid_mac_ul_sched_result();

/// Builds and returns a valid MAC UL sched result with all PDUs filled.
mac_ul_sched_result_test_helper build_valid_mac_ul_sched_result_with_all_supported_pdus();

} // namespace unittests
} // namespace ocudu
