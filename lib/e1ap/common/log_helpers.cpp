// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "log_helpers.h"
#include "../common/e1ap_asn1_utils.h"
#include "ocudu/support/format/custom_formattable.h"
#include "ocudu/support/format/fmt_basic_parser.h"

using namespace ocudu;

namespace fmt {

template <>
struct formatter<asn1::e1ap::e1ap_pdu_c> : public basic_parser {
  template <typename FormatContext>
  auto format(const asn1::e1ap::e1ap_pdu_c& p, FormatContext& ctx) const
  {
    asn1::json_writer js;
    p.to_json(js);
    return fmt::format_to(ctx.out(), "{}", js.to_string());
  }
};

} // namespace fmt

static void log_common_message(ocudulog::basic_logger& logger,
                               bool                    is_rx,
                               uint8_t                 transaction_id,
                               const e1ap_message&     e1ap_msg,
                               bool                    json_enabled)
{
  const char* msg_name = get_message_type_str(e1ap_msg.pdu);
  const char* rx_str   = is_rx ? "Rx" : "Tx";

  if (json_enabled) {
    logger.info("{} PDU tid={}: {}\n{}", rx_str, transaction_id, msg_name, e1ap_msg.pdu);
  } else {
    logger.info("{} PDU tid={}: {}", rx_str, transaction_id, msg_name);
  }
}

static void log_common_message(e1ap_logger&        logger,
                               bool                is_rx,
                               uint8_t             transaction_id,
                               const e1ap_message& e1ap_msg,
                               bool                json_enabled)
{
  const char* msg_name = get_message_type_str(e1ap_msg.pdu);
  const char* rx_str   = is_rx ? "Rx" : "Tx";

  if (json_enabled) {
    logger.log_info("{} PDU tid={} {}\n{}", rx_str, transaction_id, msg_name, e1ap_msg.pdu);
  } else {
    logger.log_info("{} PDU tid={} {}", rx_str, transaction_id, msg_name);
  }
}

template <typename UeIndex>
void ocudu::log_e1ap_pdu(ocudulog::basic_logger&       logger,
                         bool                          is_rx,
                         const std::optional<UeIndex>& ue_id,
                         const e1ap_message&           e1ap_msg,
                         bool                          json_enabled)
{
  if (not logger.info.enabled()) {
    return;
  }

  // Determine if it is a UE-dedicated message or common message.
  std::optional<uint8_t> transaction_id = get_transaction_id(e1ap_msg.pdu);
  if (transaction_id.has_value()) {
    log_common_message(logger, is_rx, transaction_id.value(), e1ap_msg, json_enabled);
    return;
  }

  std::optional<gnb_cu_cp_ue_e1ap_id_t> cp_ue_id = get_gnb_cu_cp_ue_e1ap_id(e1ap_msg.pdu);
  std::optional<gnb_cu_up_ue_e1ap_id_t> up_ue_id = get_gnb_cu_up_ue_e1ap_id(e1ap_msg.pdu);
  const char*                           msg_name = get_message_type_str(e1ap_msg.pdu);

  // Create PDU formatter that runs in log backend.
  // Note: msg_name is a string literal and therefore it is ok to pass by pointer.
  auto pdu_description = make_formattable([is_rx, cp_ue_id, up_ue_id, ue_id, msg_name = msg_name](auto& ctx) {
    return fmt::format_to(ctx.out(),
                          "{} PDU{}{}{}: {}",
                          is_rx ? "Rx" : "Tx",
                          add_prefix_if_set(" ue=", ue_id),
                          add_prefix_if_set(" cu_cp_ue=", cp_ue_id),
                          add_prefix_if_set(" cu_up_ue=", up_ue_id),
                          msg_name);
  });

  if (json_enabled) {
    logger.info("{}\n{}", pdu_description, e1ap_msg.pdu);
  } else {
    logger.info("{}", pdu_description);
  }
}

template <typename UeIndex>
void ocudu::log_e1ap_pdu(e1ap_logger&                  logger,
                         bool                          is_rx,
                         const std::optional<UeIndex>& ue_id,
                         const e1ap_message&           e1ap_msg,
                         bool                          json_enabled)
{
  if (not logger.get_basic_logger().info.enabled()) {
    return;
  }

  // Determine if it is a UE-dedicated message or common message.
  std::optional<uint8_t> transaction_id = get_transaction_id(e1ap_msg.pdu);
  if (transaction_id.has_value()) {
    log_common_message(logger, is_rx, transaction_id.value(), e1ap_msg, json_enabled);
    return;
  }

  std::optional<gnb_cu_cp_ue_e1ap_id_t> cp_ue_id = get_gnb_cu_cp_ue_e1ap_id(e1ap_msg.pdu);
  std::optional<gnb_cu_up_ue_e1ap_id_t> up_ue_id = get_gnb_cu_up_ue_e1ap_id(e1ap_msg.pdu);
  const char*                           msg_name = get_message_type_str(e1ap_msg.pdu);

  // Create PDU formatter that runs in log backend.
  // Note: msg_name is a string literal and therefore it is ok to pass by pointer.
  auto pdu_description = make_formattable([is_rx, cp_ue_id, up_ue_id, ue_id, msg_name = msg_name](auto& ctx) {
    return fmt::format_to(ctx.out(),
                          "{} PDU{}{}{} {}",
                          is_rx ? "Rx" : "Tx",
                          add_prefix_if_set(" ue=", ue_id),
                          add_prefix_if_set(" cu_cp_ue=", cp_ue_id),
                          add_prefix_if_set(" cu_up_ue=", up_ue_id),
                          msg_name);
  });

  if (json_enabled) {
    logger.log_info("{}\n{}", pdu_description, e1ap_msg.pdu);
  } else {
    logger.log_info("{}", pdu_description);
  }
}

template void ocudu::log_e1ap_pdu<cu_cp_ue_index_t>(ocudulog::basic_logger&                logger,
                                                    bool                                   is_rx,
                                                    const std::optional<cu_cp_ue_index_t>& ue_id,
                                                    const e1ap_message&                    e1ap_msg,
                                                    bool                                   json_enabled);
template void ocudu::log_e1ap_pdu<cu_up_ue_index_t>(ocudulog::basic_logger&                logger,
                                                    bool                                   is_rx,
                                                    const std::optional<cu_up_ue_index_t>& ue_id,
                                                    const e1ap_message&                    e1ap_msg,
                                                    bool                                   json_enabled);

template void ocudu::log_e1ap_pdu<cu_up_ue_index_t>(e1ap_logger&                           logger,
                                                    bool                                   is_rx,
                                                    const std::optional<cu_up_ue_index_t>& ue_id,
                                                    const e1ap_message&                    e1ap_msg,
                                                    bool                                   json_enabled);
