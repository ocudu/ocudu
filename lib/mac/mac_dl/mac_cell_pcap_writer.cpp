// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_cell_pcap_writer.h"
#include "ocudu/mac/mac_cell_result.h"
#include "ocudu/pcap/mac_pcap.h"
#include "ocudu/scheduler/result/sched_result.h"
#include <algorithm>
#include <limits>

using namespace ocudu;

mac_cell_pcap_writer::mac_cell_pcap_writer(mac_pcap&                                pcap_,
                                           bool                                     is_tdd,
                                           span<const si_message_scheduling_config> si_messages) :
  pcap(pcap_),
  radio_type(is_tdd ? PCAP_TDD_RADIO : PCAP_FDD_RADIO),
  sib1_dumped_version(std::numeric_limits<unsigned>::max())
{
  ocudu_assert(si_messages.size() <= MAX_SI_MESSAGES, "Invalid number of SI messages");
  for (const si_message_scheduling_config& si_msg : si_messages) {
    si_msgs.push_back(si_msg_context{si_msg.sibs, std::numeric_limits<unsigned>::max()});
  }
}

void mac_cell_pcap_writer::write_slot_result(slot_point                sl_tx,
                                             const sched_result&       sl_res,
                                             const mac_dl_data_result& dl_res)
{
  if (not pcap.is_write_enabled() or sl_res.dl.nof_dl_symbols == 0) {
    return;
  }

  write_si_pdus(sl_tx, sl_res, dl_res);
  write_rar_pdus(sl_tx, sl_res, dl_res);
  write_paging_pdus(sl_tx, sl_res, dl_res);
  write_ue_pdus(sl_tx, sl_res, dl_res);
}

void mac_cell_pcap_writer::write_si_pdus(slot_point sl_tx, const sched_result& sl_res, const mac_dl_data_result& dl_res)
{
  for (unsigned i = 0, e = dl_res.si_pdus.size(); i != e; ++i) {
    const sib_information& dl_alloc = sl_res.dl.bc.sibs[i];

    // A PWS (ETWS/CMAS) SI-message cycles through multiple content segments and repetitions without bumping "version"
    // (which only tracks SI scheduling info updates), so it bypasses the version-based dedup, which would wrongly
    // suppress genuinely different payloads.
    if (not dl_alloc.sibs.is_pws()) {
      unsigned* dumped_version = &sib1_dumped_version;
      if (dl_alloc.si_indicator != sib_information::sib1) {
        const auto si_msg_it = std::find_if(si_msgs.begin(), si_msgs.end(), [&dl_alloc](const si_msg_context& si_msg) {
          return si_msg.sibs == dl_alloc.sibs;
        });
        ocudu_assert(si_msg_it != si_msgs.end(), "The SI message this grant carries is not one of the cell");
        if (si_msg_it == si_msgs.end()) {
          continue;
        }
        dumped_version = &si_msg_it->dumped_version;
      }
      if (*dumped_version == dl_alloc.version) {
        continue;
      }
      *dumped_version = dl_alloc.version;
    }

    const mac_dl_data_result::dl_pdu& si_pdu  = dl_res.si_pdus[i];
    mac_nr_context_info               context = {};
    context.radioType                         = radio_type;
    context.direction                         = PCAP_DIRECTION_DOWNLINK;
    context.rntiType                          = PCAP_SI_RNTI;
    context.rnti                              = to_underlying(dl_alloc.pdsch_cfg.rnti);
    context.system_frame_number               = sl_tx.sfn();
    context.sub_frame_number                  = sl_tx.subframe_index();
    context.length                            = si_pdu.pdu.get_buffer().size();
    pcap.push_pdu(context, si_pdu.pdu.get_buffer());
  }
}

void mac_cell_pcap_writer::write_rar_pdus(slot_point                sl_tx,
                                          const sched_result&       sl_res,
                                          const mac_dl_data_result& dl_res)
{
  for (unsigned i = 0, e = dl_res.rar_pdus.size(); i != e; ++i) {
    const mac_dl_data_result::dl_pdu& rar_pdu  = dl_res.rar_pdus[i];
    const rar_information&            dl_alloc = sl_res.dl.rar_grants[i];
    mac_nr_context_info               context  = {};
    context.radioType                          = radio_type;
    context.direction                          = PCAP_DIRECTION_DOWNLINK;
    context.rntiType                           = PCAP_RA_RNTI;
    context.rnti                               = to_underlying(dl_alloc.pdsch_cfg.rnti);
    context.system_frame_number                = sl_tx.sfn();
    context.sub_frame_number                   = sl_tx.subframe_index();
    context.length                             = rar_pdu.pdu.get_buffer().size();
    pcap.push_pdu(context, rar_pdu.pdu.get_buffer());
  }
}

void mac_cell_pcap_writer::write_paging_pdus(slot_point                sl_tx,
                                             const sched_result&       sl_res,
                                             const mac_dl_data_result& dl_res)
{
  for (unsigned i = 0, e = dl_res.paging_pdus.size(); i != e; ++i) {
    const mac_dl_data_result::dl_pdu& pg_pdu   = dl_res.paging_pdus[i];
    const dl_paging_allocation&       dl_alloc = sl_res.dl.paging_grants[i];
    mac_nr_context_info               context  = {};
    context.radioType                          = radio_type;
    context.direction                          = PCAP_DIRECTION_DOWNLINK;
    context.rntiType                           = PCAP_P_RNTI;
    context.rnti                               = to_underlying(dl_alloc.pdsch_cfg.rnti);
    context.system_frame_number                = sl_tx.sfn();
    context.sub_frame_number                   = sl_tx.subframe_index();
    context.length                             = pg_pdu.pdu.get_buffer().size();
    pcap.push_pdu(context, pg_pdu.pdu.get_buffer());
  }
}

void mac_cell_pcap_writer::write_ue_pdus(slot_point sl_tx, const sched_result& sl_res, const mac_dl_data_result& dl_res)
{
  unsigned pdu_idx = 0;
  for (const dl_msg_alloc& dl_alloc : sl_res.dl.ue_grants) {
    for (unsigned cw_idx = 0, sz = dl_alloc.pdsch_cfg.codewords.size(); cw_idx != sz; ++cw_idx, ++pdu_idx) {
      const mac_dl_data_result::dl_pdu& ue_pdu = dl_res.ue_pdus[pdu_idx];
      if (not dl_alloc.pdsch_cfg.codewords[cw_idx].new_data) {
        continue;
      }
      mac_nr_context_info context = {};
      context.radioType           = radio_type;
      context.direction           = PCAP_DIRECTION_DOWNLINK;
      context.rntiType            = PCAP_C_RNTI;
      context.rnti                = to_underlying(dl_alloc.pdsch_cfg.rnti);
      context.ueid                = dl_alloc.context.ue_index == du_ue_index_t::INVALID_DU_UE_INDEX
                                        ? du_ue_index_t::INVALID_DU_UE_INDEX
                                        : dl_alloc.context.ue_index + 1;
      context.harqid              = dl_alloc.pdsch_cfg.harq_id;
      context.system_frame_number = sl_tx.sfn();
      context.sub_frame_number    = sl_tx.subframe_index();
      context.length              = ue_pdu.pdu.get_buffer().size();
      pcap.push_pdu(context, ue_pdu.pdu.get_buffer());
    }
  }
}
