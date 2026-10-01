// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "log_helpers.h"
#include "xnap_asn1_utils.h"
#include "ocudu/support/format/custom_formattable.h"

using namespace ocudu;

namespace fmt {

template <>
struct formatter<asn1::xnap::xn_ap_pdu_c> : public basic_parser {
  template <typename FormatContext>
  auto format(const asn1::xnap::xn_ap_pdu_c& p, FormatContext& ctx) const
  {
    asn1::json_writer js;
    p.to_json(js);
    return fmt::format_to(ctx.out(), "{}", js.to_string());
  }
};

} // namespace fmt

void ocudu::ocucp::log_xnap_pdu(ocudulog::basic_logger&        logger,
                                bool                           json_log,
                                bool                           is_rx,
                                const asn1::xnap::xn_ap_pdu_c& pdu)
{
  if (not logger.info.enabled()) {
    return;
  }

  // Custom formattable object whose formatting function will run in the log backend.
  auto pdu_log_entry = make_formattable([is_rx, msg_name = asn1_utils::get_message_type_str(pdu)](auto& ctx) {
    return fmt::format_to(ctx.out(), "{} PDU: {}", is_rx ? "Rx" : "Tx", msg_name);
  });

  if (json_log) {
    logger.info("{}\n{}", pdu_log_entry, pdu);
  } else {
    logger.info("{}", pdu_log_entry);
  }
}
