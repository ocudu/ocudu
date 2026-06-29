// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "write_replace_warning_routine.h"
#include "../du_processor/du_processor_repository.h"
#include "pws_sib_encoder.h"
#include "ocudu/support/async/when_all.h"

using namespace ocudu;
using namespace ocudu::ocucp;

cu_cp_write_replace_warning_routine::cu_cp_write_replace_warning_routine(
    const ngap_write_replace_warning_request& request_,
    du_processor_repository&                  du_db_,
    ocudulog::basic_logger&                   logger_) :
  request(request_), du_db(du_db_), logger(logger_)
{
}

void cu_cp_write_replace_warning_routine::operator()(coro_context<async_task<ngap_write_replace_warning_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.info("\"{}\" started...", name());

  response.msg_id     = request.msg_id;
  response.serial_num = request.serial_num;

  if (!build_f1ap_requests()) {
    logger.warning("\"{}\" failed. Cause: failed to pack F1AP Write-Replace Warning Request", name());
    CORO_EARLY_RETURN(response);
  }

  CORO_AWAIT_VALUE(du_results, when_all(build_du_tasks()));

  for (size_t i = 0; i < du_results.size(); ++i) {
    if (!du_results[i]) {
      logger.warning("\"{}\" write-replace warning delivery to du={} failed", name(), targeted_du_indexes[i]);
    }
  }

  logger.info("\"{}\" finished successfully", name());

  CORO_RETURN(response);
}

std::vector<async_task<bool>> cu_cp_write_replace_warning_routine::build_du_tasks()
{
  std::vector<async_task<bool>>    du_tasks;
  std::vector<nr_cell_global_id_t> matching_cells;

  for (const cu_cp_du_index_t du_index : du_db.get_du_processor_indexes()) {
    du_processor* du_proc = du_db.find_du_processor(du_index);
    if (du_proc == nullptr) {
      logger.warning("\"{}\" DU processor not found for index {}", name(), du_index);
      continue;
    }

    std::optional<std::vector<nr_cell_global_id_t>> cells_to_be_broadcast;
    if (cgi_filter.has_value()) {
      if (du_proc->get_context() == nullptr) {
        continue;
      }
      matching_cells.clear();
      for (const auto& cgi : *cgi_filter) {
        if (du_proc->has_cell(cgi)) {
          matching_cells.push_back(cgi);
        }
      }
      if (matching_cells.empty()) {
        continue;
      }
      cells_to_be_broadcast = matching_cells;
    }

    // Per-DU copy: each task runs concurrently with the others, so they must not share mutable request state.
    std::vector<f1ap_write_replace_warning_request> du_requests = f1ap_requests;
    for (auto& req : du_requests) {
      req.cells_to_be_broadcast = cells_to_be_broadcast;
    }

    targeted_du_indexes.push_back(du_index);
    du_tasks.push_back(launch_async([this,
                                     &du_proc = *du_proc,
                                     du_index,
                                     du_requests = std::move(du_requests),
                                     f1ap_resp   = f1ap_write_replace_warning_response{},
                                     success     = true,
                                     req_idx     = size_t{0}](coro_context<async_task<bool>>& ctx) mutable {
      CORO_BEGIN(ctx);

      for (; req_idx < du_requests.size(); ++req_idx) {
        CORO_AWAIT_VALUE(f1ap_resp,
                         du_proc.get_f1ap_handler().get_f1ap_warning_manager().handle_write_replace_warning_request(
                             du_requests[req_idx]));
        if (!f1ap_resp.success) {
          logger.warning("\"{}\" F1AP write-replace warning request to du={} failed", name(), du_index);
          success = false;
        }
      }

      CORO_RETURN(success);
    }));
  }

  return du_tasks;
}

bool cu_cp_write_replace_warning_routine::build_f1ap_requests()
{
  // Build one or two F1AP requests depending on the NGAP IEs present.
  // TS 38.413 section 8.9.1 / TS 38.331 section 6.3.1:
  //   warning_type present only                        -> SIB6 (ETWS primary)
  //   warning_type + data_coding_scheme + msg_contents -> SIB6 + SIB7 (ETWS primary + secondary)
  //   data_coding_scheme + msg_contents only           -> SIB8 (CMAS)

  if (request.warning_type.has_value()) {
    auto sib6_buf = encode_sib6(request.msg_id, request.serial_num, *request.warning_type);
    if (!sib6_buf) {
      logger.warning("\"{}\" failed to encode SIB6", name());
      return false;
    }
    f1ap_write_replace_warning_request sib6_req;
    sib6_req.pws_sys_info.sib_type    = 6;
    sib6_req.pws_sys_info.sib_msg     = std::move(*sib6_buf);
    sib6_req.repeat_period            = request.repeat_period;
    sib6_req.nof_broadcasts_requested = request.nof_broadcasts_requested;
    f1ap_requests.push_back(std::move(sib6_req));
  }

  // TS 38.413 section 9.3.1.37: Data Coding Scheme shall be present when Warning Message Contents is present.
  if (request.warning_msg_contents.has_value() && request.data_coding_scheme.has_value()) {
    const uint8_t sib_type = request.warning_type.has_value() ? 7U : 8U;
    const uint8_t dcs      = *request.data_coding_scheme;
    auto          sib_buf  = (sib_type == 7)
                                 ? encode_sib7(request.msg_id, request.serial_num, dcs, *request.warning_msg_contents)
                                 : encode_sib8(request.msg_id, request.serial_num, dcs, *request.warning_msg_contents);
    if (!sib_buf) {
      logger.warning("\"{}\" failed to encode SIB{}", name(), sib_type);
      return false;
    }
    f1ap_write_replace_warning_request seg_req;
    seg_req.pws_sys_info.sib_type    = sib_type;
    seg_req.pws_sys_info.sib_msg     = std::move(*sib_buf);
    seg_req.repeat_period            = request.repeat_period;
    seg_req.nof_broadcasts_requested = request.nof_broadcasts_requested;
    f1ap_requests.push_back(std::move(seg_req));
  }

  // When no optional IEs are present (mandatory-only request), send a minimal SIB8 to the DU
  // so the basic WRITE-REPLACE WARNING flow always results in at least one F1AP request.
  if (f1ap_requests.empty()) {
    f1ap_write_replace_warning_request fallback;
    fallback.pws_sys_info.sib_type    = 8;
    fallback.repeat_period            = request.repeat_period;
    fallback.nof_broadcasts_requested = request.nof_broadcasts_requested;
    f1ap_requests.push_back(std::move(fallback));
  }

  // Extract the NR CGI filter from the warning area list.
  // TAI and emergency-area-id variants are not mapped to a cell filter; all DU cells are targeted in those cases.
  if (request.warning_area_list.has_value()) {
    if (const auto* cgis = std::get_if<ngap_nr_cgi_list_for_warning>(&*request.warning_area_list)) {
      cgi_filter = *cgis;
    }
  }

  return true;
}
