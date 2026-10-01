// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "prach.h"
#include "ocudu/scheduler/result/prach_info.h"

using namespace ocudu;
using namespace fapi_adaptor;

void ocudu::fapi_adaptor::convert_prach_mac_to_fapi(fapi::ul_prach_pdu_builder& builder,
                                                    const prach_occasion_info&  mac_pdu)
{
  // NOTE: For long preambles the number of time-domain PRACH occasions parameter does not apply, so set it to 1 to be
  // compliant with the FAPI specification.
  unsigned nof_prach_occasions = is_long_preamble(mac_pdu.format) ? 1U : mac_pdu.nof_prach_occasions;

  builder
      .set_preamble_parameters(fapi::ul_prach_pdu::preambles_interval::start_and_len(mac_pdu.start_preamble_index,
                                                                                     mac_pdu.nof_preamble_indexes))
      .set_prach_parameters(nof_prach_occasions, mac_pdu.nof_cs, mac_pdu.format)
      .set_frequency_domain_parameters(mac_pdu.index_fd_ra, mac_pdu.nof_fd_ra)
      .set_time_domain_parameters(mac_pdu.start_symbol);
}
