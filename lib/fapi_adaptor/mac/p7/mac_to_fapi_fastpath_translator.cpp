// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_to_fapi_fastpath_translator.h"
#include "pdu_translators/csi_rs.h"
#include "pdu_translators/pdcch.h"
#include "pdu_translators/pdsch.h"
#include "pdu_translators/prach.h"
#include "pdu_translators/prs.h"
#include "pdu_translators/pucch.h"
#include "pdu_translators/pusch.h"
#include "pdu_translators/srs.h"
#include "pdu_translators/ssb.h"
#include "ocudu/fapi/p7/builders/dl_tti_request_builder.h"
#include "ocudu/fapi/p7/builders/tx_data_request_builder.h"
#include "ocudu/fapi/p7/builders/ul_dci_request_builder.h"
#include "ocudu/fapi/p7/builders/ul_tti_request_builder.h"
#include "ocudu/fapi/p7/p7_last_request_notifier.h"
#include "ocudu/fapi/p7/p7_requests_gateway.h"
#include "ocudu/scheduler/result/sched_result.h"

using namespace ocudu;
using namespace fapi_adaptor;

mac_to_fapi_fastpath_translator::mac_to_fapi_fastpath_translator(
    const mac_to_fapi_fastpath_translator_config& config,
    mac_to_fapi_fastpath_translator_dependencies  dependencies) :
  cell_nof_prbs(config.cell_nof_prbs),
  sector_id(config.sector_id),
  p7_gateway(dependencies.p7_gateway),
  p7_last_req_notifier(dependencies.p7_last_req_notifier),
  pm_mapper(std::move(dependencies.pm_mapper)),
  part2_mapper(std::move(dependencies.part2_mapper)),
  fapi_logger(dependencies.fapi_logger)
{
  ocudu_assert(pm_mapper, "Invalid precoding codebook mapper");
  ocudu_assert(part2_mapper, "Invalid Part2 mapper");
}

void mac_to_fapi_fastpath_translator::start()
{
  // Make sure the stop manager is reset.
  stop_manager.reset();
}

void mac_to_fapi_fastpath_translator::stop()
{
  stop_manager.stop();
}

/// Adds a PDCCH PDU to the given builder.
template <typename builder_type, typename pdu_type>
static void add_pdcch_pdus_to_builder(builder_type&                    builder,
                                      span<const pdu_type>             pdcch_info,
                                      span<const dci_payload>          payloads,
                                      const precoding_codebook_mapper& pm_mapper,
                                      unsigned                         cell_nof_prbs)
{
  static_assert(std::is_same_v<builder_type, fapi::dl_tti_request_builder> ||
                    std::is_same_v<builder_type, fapi::ul_dci_request_builder>,
                "Unsupported builder type. Must be DL TTI or UL DCI builder.");

  ocudu_assert(pdcch_info.size() == payloads.size(), "Size mismatch");

  for (unsigned i = 0, e = pdcch_info.size(); i != e; ++i) {
    const coreset_configuration& coreset_cfg = *pdcch_info[i].ctx.coreset_cfg;

    ocudu_assert(!(coreset_cfg.get_id() == to_coreset_id(0) && !coreset_cfg.interleaved_mapping().has_value()),
                 "CORESET0 must always be interleaved");

    fapi::dl_pdcch_pdu_builder pdcch_builder = builder.add_pdcch_pdu();
    convert_pdcch_mac_to_fapi(pdcch_builder, pdcch_info[i].ctx, payloads[i], pm_mapper, cell_nof_prbs);
  }
}

static void add_ssb_pdus_to_dl_request(fapi::dl_tti_request_builder& builder,
                                       span<const dl_ssb_pdu>        ssb_pdus,
                                       slot_point                    slot,
                                       unsigned                      cell_nof_prbs)
{
  for (const auto& pdu : ssb_pdus) {
    fapi::dl_ssb_pdu_builder ssb_builder = builder.add_ssb_pdu();
    convert_ssb_mac_to_fapi(ssb_builder, pdu, slot, cell_nof_prbs);
  }
}

static void add_csi_rs_pdus_to_dl_request(fapi::dl_tti_request_builder& builder, span<const csi_rs_info> csi_rs_list)
{
  for (const auto& pdu : csi_rs_list) {
    convert_csi_rs_mac_to_fapi(builder, pdu);
  }
}

static void add_prs_pdus_to_dl_request(fapi::dl_tti_request_builder&    builder,
                                       span<const prs_info>             prs_list,
                                       const precoding_codebook_mapper& pm_mapper,
                                       unsigned                         cell_nof_prbs)
{
  for (const auto& pdu : prs_list) {
    convert_prs_mac_to_fapi(builder, pdu, pm_mapper, cell_nof_prbs);
  }
}

static void add_pdsch_pdus_to_dl_request(fapi::dl_tti_request_builder&    builder,
                                         span<const sib_information>      sibs,
                                         span<const rar_information>      rars,
                                         span<const dl_msg_alloc>         ue_grants,
                                         span<const dl_paging_allocation> paging,
                                         unsigned                         nof_csi_pdus,
                                         const precoding_codebook_mapper& pm_mapper,
                                         unsigned                         cell_nof_prbs)
{
  for (const auto& pdu : sibs) {
    fapi::dl_pdsch_pdu_builder pdsch_builder = builder.add_pdsch_pdu();
    convert_pdsch_mac_to_fapi(pdsch_builder, pdu, nof_csi_pdus, pm_mapper, cell_nof_prbs);
  }

  for (const auto& pdu : rars) {
    fapi::dl_pdsch_pdu_builder pdsch_builder = builder.add_pdsch_pdu();
    convert_pdsch_mac_to_fapi(pdsch_builder, pdu, nof_csi_pdus, pm_mapper, cell_nof_prbs);
  }

  for (const auto& pdu : ue_grants) {
    fapi::dl_pdsch_pdu_builder pdsch_builder = builder.add_pdsch_pdu();
    convert_pdsch_mac_to_fapi(pdsch_builder, pdu, nof_csi_pdus, pm_mapper, cell_nof_prbs);
  }

  for (const auto& pdu : paging) {
    fapi::dl_pdsch_pdu_builder pdsch_builder = builder.add_pdsch_pdu();
    convert_pdsch_mac_to_fapi(pdsch_builder, pdu, nof_csi_pdus, pm_mapper, cell_nof_prbs);
  }
}

void mac_to_fapi_fastpath_translator::on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res)
{
  auto token = stop_manager.get_token();
  // Do not process results when the translator is not running.
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  stop_token = std::move(token);
  fapi::dl_tti_request         msg;
  fapi::dl_tti_request_builder builder(msg);

  builder.set_slot(dl_res.slot);

  // Add PDCCH PDUs to the DL_TTI.request message.
  add_pdcch_pdus_to_builder(builder,
                            span<const pdcch_dl_information>(dl_res.dl_res->dl_pdcchs),
                            dl_res.dl_pdcch_pdus,
                            *pm_mapper,
                            cell_nof_prbs);

  // Add SSB PDUs to the DL_TTI.request message.
  add_ssb_pdus_to_dl_request(builder, dl_res.ssb_pdus, dl_res.slot, cell_nof_prbs);

  // Add CSI-RS PDUs to the DL_TTI.request message.
  add_csi_rs_pdus_to_dl_request(builder, dl_res.dl_res->csi_rs);

  // Add DL-PRS PDUs to the DL_TTI.request message.
  add_prs_pdus_to_dl_request(builder, dl_res.dl_res->prs, *pm_mapper, cell_nof_prbs);

  // Add PDSCH PDUs to the DL_TTI.request message.
  add_pdsch_pdus_to_dl_request(builder,
                               dl_res.dl_res->bc.sibs,
                               dl_res.dl_res->rar_grants,
                               dl_res.dl_res->ue_grants,
                               dl_res.dl_res->paging_grants,
                               dl_res.dl_res->csi_rs.size(),
                               *pm_mapper,
                               cell_nof_prbs);

  if (OCUDU_UNLIKELY(fapi_logger.debug.enabled())) {
    fapi_logger.debug("Sector#{}: {}", sector_id, msg);
  }

  // Send the message.
  p7_gateway.send_dl_tti_request(msg);

  handle_ul_dci_request(dl_res.dl_res->ul_pdcchs, dl_res.ul_pdcch_pdus, dl_res.slot);
}

void mac_to_fapi_fastpath_translator::on_new_downlink_data(const mac_dl_data_result& dl_data)
{
  auto token = stop_manager.get_token();
  // Do not process results when the translator is not running.
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  stop_token = std::move(token);

  ocudu_assert(!dl_data.si_pdus.empty() || !dl_data.rar_pdus.empty() || !dl_data.ue_pdus.empty() ||
                   !dl_data.paging_pdus.empty(),
               "Received a mac_dl_data_result object with zero payloads");

  fapi::tx_data_request         msg;
  fapi::tx_data_request_builder builder(msg);

  builder.set_slot(dl_data.slot);

  // Make sure PDUs are added to the builder in the same order as for the DL_TTI.request message.
  unsigned fapi_pdu_index = 0;

  // Add SIB1 PDUs to the Tx_Data.request message.
  for (const auto& pdu : dl_data.si_pdus) {
    builder.add_pdu(fapi_pdu_index, pdu.cw_index, pdu.pdu);
    if (pdu.cw_index == 0) {
      ++fapi_pdu_index;
    }
  }

  // Add RAR PDUs to the Tx_Data.request message.
  for (const auto& pdu : dl_data.rar_pdus) {
    builder.add_pdu(fapi_pdu_index, pdu.cw_index, pdu.pdu);
    if (pdu.cw_index == 0) {
      ++fapi_pdu_index;
    }
  }

  // Add UE specific PDUs to the Tx_Data.request message.
  for (const auto& pdu : dl_data.ue_pdus) {
    builder.add_pdu(fapi_pdu_index, pdu.cw_index, pdu.pdu);
    if (pdu.cw_index == 0) {
      ++fapi_pdu_index;
    }
  }

  // Add Paging PDU to the Tx_Data.request message.
  for (const auto& pdu : dl_data.paging_pdus) {
    builder.add_pdu(fapi_pdu_index, pdu.cw_index, pdu.pdu);
    if (pdu.cw_index == 0) {
      ++fapi_pdu_index;
    }
  }

  if (OCUDU_UNLIKELY(fapi_logger.debug.enabled())) {
    fapi_logger.debug("Sector#{}: {}", sector_id, msg);
  }

  // Send the message.
  p7_gateway.send_tx_data_request(msg);
}

void mac_to_fapi_fastpath_translator::on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res)
{
  auto token = stop_manager.get_token();
  // Do not process results when the translator is not running.
  if (OCUDU_UNLIKELY(token.is_stop_requested())) {
    return;
  }

  stop_token = std::move(token);

  fapi::ul_tti_request         msg;
  fapi::ul_tti_request_builder builder(msg);

  builder.set_slot(ul_res.slot);

  // Add PRACH PDUs to the UL_TTI.request message.
  for (const auto& pdu : ul_res.ul_res->prachs) {
    fapi::ul_prach_pdu_builder pdu_builder = builder.add_prach_pdu();
    convert_prach_mac_to_fapi(pdu_builder, pdu);
  }

  // Add PUSCH PDUs to the UL_TTI.request message.
  for (const auto& pdu : ul_res.ul_res->puschs) {
    fapi::ul_pusch_pdu_builder pdu_builder = builder.add_pusch_pdu();
    convert_pusch_mac_to_fapi(pdu_builder, pdu, *part2_mapper);
  }

  for (const auto& pdu : ul_res.ul_res->pucchs) {
    fapi::ul_pucch_pdu_builder pdu_builder = builder.add_pucch_pdu(pdu.format());
    convert_pucch_mac_to_fapi(pdu_builder, pdu);
  }

  for (const auto& pdu : ul_res.ul_res->srss) {
    fapi::ul_srs_pdu_builder pdu_builder = builder.add_srs_pdu();
    convert_srs_mac_to_fapi(pdu_builder, pdu);
  }

  if (OCUDU_UNLIKELY(fapi_logger.debug.enabled())) {
    fapi_logger.debug("Sector#{}: {}", sector_id, msg);
  }

  // Send the message.
  p7_gateway.send_ul_tti_request(msg);
}

void mac_to_fapi_fastpath_translator::handle_ul_dci_request(span<const pdcch_ul_information> pdcch_info,
                                                            span<const dci_payload>          payloads,
                                                            slot_point                       slot)
{
  // This message is optional, do not send it empty.
  if (pdcch_info.empty()) {
    return;
  }

  fapi::ul_dci_request         msg;
  fapi::ul_dci_request_builder builder(msg);

  builder.set_slot(slot);

  add_pdcch_pdus_to_builder(builder, pdcch_info, payloads, *pm_mapper, cell_nof_prbs);

  if (OCUDU_UNLIKELY(fapi_logger.debug.enabled())) {
    fapi_logger.debug("Sector#{}: {}", sector_id, msg);
  }

  // Send the message.
  p7_gateway.send_ul_dci_request(msg);
}

void mac_to_fapi_fastpath_translator::on_cell_results_completion(slot_point slot)
{
  p7_last_req_notifier.on_last_message(slot);

  // Reset the stop token.
  stop_token.reset();
}
