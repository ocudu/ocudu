// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "tests/test_doubles/scheduler/scheduler_result_finder.h"
#include "ocudu/scheduler/scheduler_feedback_handler.h"

namespace ocudu {

inline const pdcch_dl_information* find_ue_dl_pdcch(rnti_t rnti, const sched_result& res)
{
  return find_ue_dl_pdcch(rnti, res.dl);
}

inline const pdcch_ul_information* find_ue_ul_pdcch(rnti_t rnti, const sched_result& res)
{
  return find_ue_ul_pdcch(rnti, res.dl);
}

inline const ul_sched_info* find_ue_pusch(rnti_t rnti, const sched_result& res)
{
  return find_ue_pusch(rnti, res.ul.puschs);
}

inline const ul_sched_info* find_ue_pusch_with_harq_ack(rnti_t rnti, const sched_result& res)
{
  const ul_sched_info* pusch = find_ue_pusch(rnti, res);
  if (pusch != nullptr) {
    if (pusch->uci.has_value() and pusch->uci->harq.has_value() and pusch->uci->harq->harq_ack_nof_bits > 0) {
      return pusch;
    }
  }
  return nullptr;
}

inline const dl_msg_alloc* find_ue_pdsch(rnti_t rnti, const sched_result& res)
{
  return find_ue_pdsch(rnti, res.dl.ue_grants);
}

inline const dl_msg_alloc* find_ue_pdsch_with_lcid(rnti_t rnti, lcid_dl_sch_t lcid, const sched_result& res)
{
  const dl_msg_alloc* pdsch = find_ue_pdsch(rnti, res.dl.ue_grants);
  if (pdsch == nullptr) {
    return nullptr;
  }
  for (unsigned tb = 0, tb_sz = pdsch->tb_list.size(); tb != tb_sz; ++tb) {
    if (std::any_of(pdsch->tb_list[tb].lc_chs_to_sched.begin(),
                    pdsch->tb_list[tb].lc_chs_to_sched.end(),
                    [lcid](const auto& lc) { return lc.lcid == lcid; })) {
      return pdsch;
    }
  }
  return nullptr;
}

inline const pucch_info* find_ue_pucch(rnti_t rnti, const sched_result& res)
{
  return find_ue_pucch(rnti, res.ul.pucchs.unsorted());
}

inline const pucch_info* find_ue_pucch_with_harq_ack(rnti_t rnti, const sched_result& res)
{
  return find_ue_pucch_with_harq_ack(rnti, res.ul.pucchs.unsorted());
}

inline uci_indication::uci_pdu create_uci_pdu_with_harq_ack(du_ue_index_t ue_index, const pucch_info& pucch_pdu)
{
  uci_indication::uci_pdu pdu;
  pdu.crnti    = pucch_pdu.crnti;
  pdu.ue_index = ue_index;
  switch (pucch_pdu.format()) {
    case pucch_format::FORMAT_1: {
      uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu f0_1_pdu{};
      f0_1_pdu.harqs.resize(pucch_pdu.uci_bits.harq_ack_nof_bits, mac_harq_ack_report_status::ack);
      pdu.pdu = f0_1_pdu;
    } break;
    case pucch_format::FORMAT_2: {
      uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu f2_pdu{};
      f2_pdu.harqs.resize(pucch_pdu.uci_bits.harq_ack_nof_bits, mac_harq_ack_report_status::ack);
      pdu.pdu = f2_pdu;
    } break;
    default:
      ocudu_terminate("Format {} not supported", fmt::underlying(pucch_pdu.format()));
      break;
  }
  return pdu;
}

} // namespace ocudu
