// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/result/sched_result.h"

namespace ocudu {

inline const pdcch_dl_information* find_ue_dl_pdcch(rnti_t rnti, const dl_sched_result& res)
{
  const auto* it = std::find_if(res.dl_pdcchs.begin(), res.dl_pdcchs.end(), [rnti](const pdcch_dl_information& pdcch) {
    return pdcch.ctx.rnti == rnti;
  });
  return it != res.dl_pdcchs.end() ? &*it : nullptr;
}

inline const pdcch_ul_information* find_ue_ul_pdcch(rnti_t rnti, const dl_sched_result& res)
{
  const auto* it = std::find_if(res.ul_pdcchs.begin(), res.ul_pdcchs.end(), [rnti](const pdcch_ul_information& pdcch) {
    return pdcch.ctx.rnti == rnti;
  });
  return it != res.ul_pdcchs.end() ? &*it : nullptr;
}

inline const rar_information* find_rar_pdsch(rnti_t ra_rnti, span<const rar_information> rars)
{
  const auto* it = std::find_if(
      rars.begin(), rars.end(), [ra_rnti](const rar_information& grant) { return grant.pdsch_cfg.rnti == ra_rnti; });
  return it != rars.end() ? &*it : nullptr;
}

inline const dl_msg_alloc* find_ue_pdsch(rnti_t rnti, span<const dl_msg_alloc> dlgrants)
{
  const auto* it = std::find_if(
      dlgrants.begin(), dlgrants.end(), [rnti](const dl_msg_alloc& pdsch) { return pdsch.pdsch_cfg.rnti == rnti; });
  return it != dlgrants.end() ? &*it : nullptr;
}

const dl_msg_alloc* find_ue_pdsch_with_lcid(rnti_t rnti, lcid_dl_sch_t lcid, span<const dl_msg_alloc> dlgrants);
const dl_msg_alloc* find_ue_pdsch_with_lcid(rnti_t rnti, lcid_t lcid, span<const dl_msg_alloc> dlgrants);

inline const ul_sched_info* find_ue_pusch(rnti_t rnti, span<const ul_sched_info> ulgrants)
{
  const auto* it = std::find_if(
      ulgrants.begin(), ulgrants.end(), [rnti](const auto& pusch) { return pusch.pusch_cfg.rnti == rnti; });
  return it != ulgrants.end() ? &*it : nullptr;
}

inline const ul_sched_info* find_ue_pusch(rnti_t rnti, const ul_sched_result& res)
{
  return find_ue_pusch(rnti, res.puschs);
}

inline const pucch_info* find_ue_pucch(rnti_t rnti, const span<const pucch_info> pucchs)
{
  const auto* it =
      std::find_if(pucchs.begin(), pucchs.end(), [rnti](const pucch_info& pucch) { return pucch.crnti == rnti; });
  return it != pucchs.end() ? &*it : nullptr;
}

inline const pucch_info* find_ue_pucch_with_harq_ack(rnti_t rnti, span<const pucch_info> pucchs)
{
  const pucch_info* pucch = find_ue_pucch(rnti, pucchs);
  if (pucch != nullptr and pucch->uci_bits.harq_ack_nof_bits > 0) {
    return pucch;
  }
  return nullptr;
}

inline const pucch_info* find_ue_pucch_with_sr(rnti_t rnti, span<const pucch_info> pucchs)
{
  const pucch_info* pucch = find_ue_pucch(rnti, pucchs);
  if (pucch != nullptr and pucch->uci_bits.sr_bits != sr_nof_bits::no_sr) {
    return pucch;
  }
  return nullptr;
}

inline const pucch_info* find_ue_pucch_with_csi(rnti_t rnti, span<const pucch_info> pucchs)
{
  const pucch_info* pucch = find_ue_pucch(rnti, pucchs);
  if (pucch != nullptr and pucch->uci_bits.csi_part1_nof_bits > 0) {
    return pucch;
  }
  return nullptr;
}

/// Find UCI (both PUCCH and PUSCH) containing CSI for the given RNTI.
const csi_report_configuration* find_ue_uci_with_csi(rnti_t rnti, const ul_sched_result& ul_res);

} // namespace ocudu
