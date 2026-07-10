// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/logical_channel/lcid.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/slot_pdu_capacity_constants.h"
#include "ocudu/ran/slot_point.h"
#include <optional>

namespace ocudu {

/// HARQ process ID value.
using harq_pid_value = uint8_t;

/// MAC Received PDU that comprises the list of PDUs of Rx_Data.indication.
struct mac_rx_pdu {
  rnti_t         rnti;
  harq_pid_value harq_id;
  byte_buffer    pdu;
  /// Index of the RA preamble that originated this PDU, set only for MsgA PUSCH receptions.
  std::optional<uint8_t> rapid;
};
using mac_rx_pdu_list = static_vector<mac_rx_pdu, MAX_PUSCH_PDUS_PER_SLOT>;

/// Rx_Data.indication Message.
struct mac_rx_data_indication {
  slot_point      sl_rx;
  du_cell_index_t cell_index;
  mac_rx_pdu_list pdus;
};

/// Interface used to push Rx Data indications to L2.
class mac_pdu_handler
{
public:
  virtual ~mac_pdu_handler() = default;

  /// This function is called whenever the MAC receives a Rx_Data.indication
  virtual void handle_rx_data_indication(mac_rx_data_indication pdu) = 0;
};

/// Used to indicate UL CCCH message arrival.
struct ul_ccch_indication_message {
  du_cell_index_t cell_index;
  rnti_t          tc_rnti;
  slot_point      slot_rx;
  byte_buffer     subpdu;
};

/// Used to indicate Msg3 C-RNTI CE detection.
struct ul_crnti_ce_indication_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
};

/// Methods used by MAC to notify events.
class mac_ul_ccch_notifier
{
public:
  virtual ~mac_ul_ccch_notifier()                                              = default;
  virtual void on_ul_ccch_msg_received(const ul_ccch_indication_message& msg)  = 0;
  virtual void on_crnti_ce_received(const ul_crnti_ce_indication_message& msg) = 0;
};

} // namespace ocudu
