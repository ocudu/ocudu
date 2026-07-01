// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_dl_ran_status_transfer_procedure.h"

using namespace ocudu;
using namespace ocudu::ocucp;
using namespace asn1::ngap;

ngap_dl_ran_status_transfer_procedure::ngap_dl_ran_status_transfer_procedure(ngap_ue_transaction_manager& ev_mng_,
                                                                             ngap_ue_logger&              logger_) :
  ev_mng(ev_mng_), logger(logger_)
{
}

void ngap_dl_ran_status_transfer_procedure::operator()(coro_context<async_task<expected<cu_cp_status_transfer>>>& ctx)
{
  CORO_BEGIN(ctx);
  logger.log_info("\"{}\" started...", name());
  transaction_sink.subscribe_to(ev_mng.dl_ran_status_transfer_outcome, std::chrono::milliseconds{5000});

  CORO_AWAIT(transaction_sink);

  if (transaction_sink.timeout_expired()) {
    logger.log_warning("\"{}\" timed out after {}ms", name(), 5000);
    CORO_EARLY_RETURN(make_unexpected(default_error_t{}));
  }

  if (not transaction_sink.successful()) {
    logger.log_warning("\"{}\" failed", name());
    CORO_EARLY_RETURN(make_unexpected(default_error_t{}));
  }

  if (not fill_ngap_dl_ran_status_transfer()) {
    logger.log_warning("\"{}\" failed", name());
    CORO_EARLY_RETURN(make_unexpected(default_error_t{}));
  }

  logger.log_info("\"{}\" finished successfully", name());
  CORO_RETURN(dl_ran_status_transfer);
}

bool ngap_dl_ran_status_transfer_procedure::fill_ngap_dl_ran_status_transfer()
{
  dl_ran_status_transfer.ue_index = ue_index;

  const asn1::ngap::dl_ran_status_transfer_s& dl_status_transfer = transaction_sink.response();

  const ran_status_transfer_transparent_container_s& transparent_cont =
      dl_status_transfer->ran_status_transfer_transparent_container;

  for (const asn1::ngap::drbs_subject_to_status_transfer_item_s& drb_item_asn1 :
       transparent_cont.drbs_subject_to_status_transfer_list) {
    cu_cp_drbs_subject_to_status_transfer_item drb_item;

    drb_item.drb_id = uint_to_drb_id(drb_item_asn1.drb_id);
    // Fill DL status
    if (drb_item_asn1.drb_status_dl.type() == drb_status_dl_c::types_opts::drb_status_dl12) {
      drb_item.drb_status_dl.sn_size      = pdcp_sn_size::size12bits;
      drb_item.drb_status_dl.dl_count.sn  = drb_item_asn1.drb_status_dl.drb_status_dl12().dl_count_value.pdcp_sn12;
      drb_item.drb_status_dl.dl_count.hfn = drb_item_asn1.drb_status_dl.drb_status_dl12().dl_count_value.hfn_pdcp_sn12;
    } else if (drb_item_asn1.drb_status_dl.type() == drb_status_dl_c::types_opts::drb_status_dl18) {
      drb_item.drb_status_dl.sn_size      = pdcp_sn_size::size18bits;
      drb_item.drb_status_dl.dl_count.sn  = drb_item_asn1.drb_status_dl.drb_status_dl18().dl_count_value.pdcp_sn18;
      drb_item.drb_status_dl.dl_count.hfn = drb_item_asn1.drb_status_dl.drb_status_dl18().dl_count_value.hfn_pdcp_sn18;
    } else {
      return false;
    }
    // Fill UL status.
    if (drb_item_asn1.drb_status_ul.type() == drb_status_ul_c::types_opts::drb_status_ul12) {
      drb_item.drb_status_ul.sn_size      = pdcp_sn_size::size12bits;
      drb_item.drb_status_ul.ul_count.sn  = drb_item_asn1.drb_status_ul.drb_status_ul12().ul_count_value.pdcp_sn12;
      drb_item.drb_status_ul.ul_count.hfn = drb_item_asn1.drb_status_ul.drb_status_ul12().ul_count_value.hfn_pdcp_sn12;
    } else if (drb_item_asn1.drb_status_ul.type() == drb_status_ul_c::types_opts::drb_status_ul18) {
      drb_item.drb_status_ul.sn_size      = pdcp_sn_size::size18bits;
      drb_item.drb_status_ul.ul_count.sn  = drb_item_asn1.drb_status_ul.drb_status_ul18().ul_count_value.pdcp_sn18;
      drb_item.drb_status_ul.ul_count.hfn = drb_item_asn1.drb_status_ul.drb_status_ul18().ul_count_value.hfn_pdcp_sn18;
    } else {
      return false;
    }
    dl_ran_status_transfer.drbs_subject_to_status_transfer_list.emplace(drb_item.drb_id, drb_item);
  }
  return true;
}
