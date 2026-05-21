// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pucch_allocator_helpers.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/ran/pucch/pucch_info.h"
#include "ocudu/scheduler/config/pucch_resource_builder_params.h"

using namespace ocudu;

pucch_existing_pdus_handler::pucch_existing_pdus_handler(rnti_t                               crnti,
                                                         span<pucch_info>                     pucchs,
                                                         const pucch_resource_builder_params& res_params)
{
  pdu_id = 0;

  for (auto& pucch : pucchs) {
    if (pucch.crnti == crnti and pucch.pdu_context.res_id) {
      pucch.pdu_context.id = MAX_PUCCH_PDUS_PER_SLOT;
      ++pdus_cnt;

      if (pucch.uci_bits.harq_ack_nof_bits != 0U) {
        if (pucch.format() == pucch_format::FORMAT_1 and
            pucch.pdu_context.res_id->ded().ue_res_id == res_params.get_sr_ue_res_idx()) {
          // For Format 1, the SR resource can carry HARQ-ACK.
          sr_pdu = &pucch;
        } else {
          harq_pdu = &pucch;
        }
      } else {
        if (pucch.pdu_context.res_id->ded().ue_res_id == res_params.get_sr_ue_res_idx()) {
          sr_pdu = &pucch;
        } else if (pucch.pdu_context.res_id->ded().ue_res_id == res_params.get_csi_ue_res_idx()) {
          csi_pdu = &pucch;
        } else {
          ocudu_assertion_failure("Unexpected PUCCH resource carrying SR/CSI only");
        }
      }
    }
  }
}

pucch_info* pucch_existing_pdus_handler::get_next_pdu(static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucchs)
{
  if (is_empty()) {
    ocudu_assert(not pucchs.full(), "PUCCH grants list is full");
    auto* new_pdu           = &pucchs.emplace_back();
    new_pdu->pdu_context.id = pdu_id++;
    return new_pdu;
  }
  pucch_info* ret_grant = nullptr;
  if (csi_pdu != nullptr) {
    ret_grant                 = csi_pdu;
    ret_grant->pdu_context.id = pdu_id++;
    --pdus_cnt;
  } else if (sr_pdu != nullptr) {
    ret_grant                 = sr_pdu;
    ret_grant->pdu_context.id = pdu_id++;
    --pdus_cnt;
  } else if (harq_pdu != nullptr) {
    ret_grant                 = harq_pdu;
    ret_grant->pdu_context.id = pdu_id++;
    --pdus_cnt;
  }
  // NOTE: this cannot be nullptr, otherwise the function would have exited at the previous return.
  return ret_grant;
}

void pucch_existing_pdus_handler::remove_unused_pdus(static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucchs,
                                                     rnti_t                                              rnti) const
{
  if (pdus_cnt == 0) {
    return;
  }
  auto* it = pucchs.begin();
  while (it != pucchs.end()) {
    if (it->crnti == rnti and it->pdu_context.res_id and it->pdu_context.id >= MAX_PUCCH_PDUS_PER_SLOT) {
      it = pucchs.erase(it);
    } else {
      ++it;
    }
  }
}

void pucch_existing_pdus_handler::update_sr_pdu_bits(sr_nof_bits sr_bits, unsigned harq_ack_bits)
{
  if (sr_pdu == nullptr) {
    return;
  }
  ocudu_assert(sr_pdu->format() == pucch_format::FORMAT_0 or sr_pdu->format() == pucch_format::FORMAT_1,
               "Only PUCCH Formats 0 or 1 can be used for SR grant");

  sr_pdu->uci_bits.sr_bits           = sr_bits;
  sr_pdu->uci_bits.harq_ack_nof_bits = harq_ack_bits;
  sr_pdu->pdu_context.id             = pdu_id++;

  // Once the grant is updated, set the pointer to null, as we don't want to process this again.
  sr_pdu = nullptr;
  --pdus_cnt;
}

void pucch_existing_pdus_handler::update_csi_pdu_bits(unsigned csi_part1_bits, sr_nof_bits sr_bits)
{
  ocudu_assert(csi_pdu->format() == pucch_format::FORMAT_2 or csi_pdu->format() == pucch_format::FORMAT_3 or
                   csi_pdu->format() == pucch_format::FORMAT_4,
               "Only PUCCH Formats 2, 3 and 4 can be used for CSI grant");

  csi_pdu->uci_bits.csi_part1_nof_bits = csi_part1_bits;
  csi_pdu->uci_bits.sr_bits            = sr_bits;
  csi_pdu->pdu_context.id              = pdu_id++;

  // Once the grant is updated, set the pointer to null, as we don't want to process this again.
  csi_pdu = nullptr;
  --pdus_cnt;
}

void pucch_existing_pdus_handler::update_harq_pdu_bits(unsigned                             harq_ack_bits,
                                                       sr_nof_bits                          sr_bits,
                                                       unsigned                             csi_part1_bits,
                                                       const pucch_resource_builder_params& res_params,
                                                       const pucch_resource&                pucch_res_cfg)
{
  const prb_interval res_prbs = pucch_res_cfg.prbs();
  switch (harq_pdu->format()) {
    case pucch_format::FORMAT_2: {
      harq_pdu->uci_bits.csi_part1_nof_bits = csi_part1_bits;
      // After updating the UCI bits, we need to recompute the number of PRBs for PUCCH format 2, as per TS 38.213,
      // Section 9.2.5.2.
      const unsigned nof_prbs =
          get_pucch_format2_nof_prbs(harq_ack_bits + sr_nof_bits_to_uint(sr_bits) + csi_part1_bits,
                                     res_prbs.length(),
                                     pucch_res_cfg.syms.length(),
                                     to_max_code_rate_float(res_params.max_code_rate_234()));
      harq_pdu->resources.prbs           = {res_prbs.start(), res_prbs.start() + nof_prbs};
      harq_pdu->resources.second_hop_prb = pucch_res_cfg.second_hop_prb;
    } break;
    case pucch_format::FORMAT_3: {
      harq_pdu->uci_bits.csi_part1_nof_bits = csi_part1_bits;
      // After updating the UCI bits, we need to recompute the number of PRBs for PUCCH format 3, as per TS 38.213,
      // Section 9.2.5.2.
      const auto&    f3_cfg = std::get<pucch_resource::f3_config>(pucch_res_cfg.format_params);
      const unsigned nof_prbs =
          get_pucch_format3_nof_prbs(harq_ack_bits + sr_nof_bits_to_uint(sr_bits) + csi_part1_bits,
                                     res_prbs.length(),
                                     pucch_res_cfg.syms.length(),
                                     to_max_code_rate_float(res_params.max_code_rate_234()),
                                     pucch_res_cfg.second_hop_prb.has_value(),
                                     f3_cfg.additional_dmrs,
                                     f3_cfg.pi_2_bpsk);
      harq_pdu->resources.prbs           = {res_prbs.start(), res_prbs.start() + nof_prbs};
      harq_pdu->resources.second_hop_prb = pucch_res_cfg.second_hop_prb;
    } break;
    case pucch_format::FORMAT_4: {
      harq_pdu->uci_bits.csi_part1_nof_bits = csi_part1_bits;
    } break;
    default:
      break;
  }
  harq_pdu->uci_bits.harq_ack_nof_bits = harq_ack_bits;
  harq_pdu->uci_bits.sr_bits           = sr_bits;
  harq_pdu->pdu_context.id             = pdu_id++;

  // Once the grant is updated, set the pointer to null, as we don't want to process this again.
  harq_pdu = nullptr;
  --pdus_cnt;
}
