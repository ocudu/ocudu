// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rlc_stress_test_traffic.h"
#include "ocudu/pdcp/pdcp_tx.h"

using namespace ocudu;

void stress_traffic_sink::on_new_sdu(byte_buffer pdu, bool integrity_verified)
{
  logger.log_info(pdu.begin(), pdu.end(), "Received PDU ({} B). verified={}", pdu.length(), integrity_verified);
}

void stress_traffic_source::send_pdu()
{
  byte_buffer sdu;

  // random or fixed SDU size
  if (args.sdu_size < 1) {
    sdu_size = int_dist(rgen);
  } else {
    sdu_size = args.sdu_size;
  }

  for (uint32_t i = 0; i < sdu_size; i++) {
    report_fatal_error_if_not(sdu.append(payload), "Failed to allocate SDU");
    payload++;
  }

  logger.log_info(sdu.begin(), sdu.end(), "Sending SDU ({} B)", sdu.length());
  pdcp_tx_upper->handle_sdu(std::move(sdu));
}
