// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ntn_ru_doppler_adapter.h"
#include "ocudu/ru/ru_controller.h"
#include "fmt/chrono.h"

using namespace ocudu;

ntn_ru_doppler_adapter::ntn_ru_doppler_adapter() : logger(ocudulog::fetch_basic_logger("RU")) {}

bool ntn_ru_doppler_adapter::handle_dl_doppler_compensation(const ocudu_ntn::doppler_compensation_request& request)
{
  ru_controller* controller = ru_ctrl.load(std::memory_order_acquire);
  if (controller == nullptr) {
    return false;
  }

  ru_cfo_controller* cfo_ctrl = controller->get_cfo_controller();
  if (cfo_ctrl == nullptr) {
    logger.warning("NTN: CFO controller not available, cannot apply DL Doppler compensation");
    return false;
  }

  cfo_compensation_request cfo_reqs;
  cfo_reqs.cfo_hz          = request.cfo_hz;
  cfo_reqs.cfo_drift_hz_s  = request.cfo_drift_hz_s;
  cfo_reqs.start_timestamp = request.start_timestamp;

  // Apply the pre-calculated DL Doppler compensation values to TX.
  cfo_ctrl->set_tx_cfo(request.sector_id, cfo_reqs);

  logger.debug("NTN: Apply DL Doppler compensation: {:.1f} Hz (drift: {:.1f} Hz/s) at {:%T}",
               cfo_reqs.cfo_hz,
               cfo_reqs.cfo_drift_hz_s,
               cfo_reqs.start_timestamp.value());

  return true;
}

bool ntn_ru_doppler_adapter::handle_ul_doppler_compensation(const ocudu_ntn::doppler_compensation_request& request)
{
  ru_controller* controller = ru_ctrl.load(std::memory_order_acquire);
  if (controller == nullptr) {
    return false;
  }

  ru_cfo_controller* cfo_ctrl = controller->get_cfo_controller();
  if (cfo_ctrl == nullptr) {
    logger.warning("NTN: CFO controller not available, cannot apply UL Doppler compensation");
    return false;
  }

  cfo_compensation_request cfo_reqs;
  cfo_reqs.cfo_hz          = request.cfo_hz;
  cfo_reqs.cfo_drift_hz_s  = request.cfo_drift_hz_s;
  cfo_reqs.start_timestamp = request.start_timestamp;

  // Apply the pre-calculated UL Doppler compensation values to RX.
  cfo_ctrl->set_rx_cfo(request.sector_id, cfo_reqs);

  logger.debug("NTN: Apply UL Doppler compensation: {:.1f} Hz (drift: {:.1f} Hz/s) at {:%T}",
               cfo_reqs.cfo_hz,
               cfo_reqs.cfo_drift_hz_s,
               cfo_reqs.start_timestamp.value());

  return true;
}
