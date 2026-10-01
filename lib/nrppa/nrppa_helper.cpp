// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "nrppa_helper.h"
#include "ocudu/adt/format.h"

using namespace ocudu;
using namespace ocucp;

void ocudu::ocucp::log_nrppa_message(ocudulog::basic_logger&          logger,
                                     const direction_t                dir,
                                     byte_buffer_view                 pdu,
                                     const asn1::nrppa::nr_ppa_pdu_c& nrppa_pdu)
{
  std::string type_str = nrppa_pdu.type().to_string();
  if (logger.debug.enabled()) {
    switch (nrppa_pdu.type()) {
      case asn1::nrppa::nr_ppa_pdu_c::types_opts::init_msg:
        type_str += fmt::format(".{}", nrppa_pdu.init_msg().value.type().to_string());
        break;
      case asn1::nrppa::nr_ppa_pdu_c::types_opts::successful_outcome:
        type_str += fmt::format(".{}", nrppa_pdu.successful_outcome().value.type().to_string());
        break;
      case asn1::nrppa::nr_ppa_pdu_c::types_opts::unsuccessful_outcome:
        type_str += fmt::format(".{}", nrppa_pdu.unsuccessful_outcome().value.type().to_string());
        break;
      default:
        break;
    }

    asn1::json_writer js;
    nrppa_pdu.to_json(js);
    logger.debug(pdu.begin(), pdu.end(), "{} {} ({} B)", (dir == Rx) ? "Rx" : "Tx", type_str, pdu.length());
    logger.debug("Containerized {}: {}", type_str, js.to_string());
  } else if (logger.info.enabled()) {
    std::vector<uint8_t> bytes{pdu.begin(), pdu.end()};
    logger.info(pdu.begin(), pdu.end(), "{}", type_str);
  }
}
