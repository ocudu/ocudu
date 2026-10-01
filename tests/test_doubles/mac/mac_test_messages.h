// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/mac/mac_cell_control_information_handler.h"
#include "ocudu/mac/mac_cell_rach_handler.h"
#include "ocudu/mac/mac_pdu_handler.h"
#include "ocudu/ran/harq_id.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/subcarrier_spacing.h"

namespace ocudu {

struct pucch_info;
struct uci_info;
struct ul_sched_info;
struct srs_info;
struct prach_occasion_info;
class slot_point;

namespace test_helpers {

/// Creates a CCCH message with the given RNTI.
mac_rx_data_indication
create_ccch_message(slot_point sl_rx, rnti_t rnti, du_cell_index_t du_cell_index = to_du_cell_index(0));

mac_rx_data_indication create_pdu_with_sdu(slot_point sl_rx, rnti_t rnti, lcid_t lcid, uint32_t rlc_sn = 0);

/// Creates an MAC Rx data indication containing an RLC status PDU with an ACK for the given RLC SN.
mac_rx_data_indication create_pdu_with_rlc_status_ack(slot_point sl_rx, rnti_t rnti, lcid_t lcid, uint32_t ack_sn);

/// Adds a MAC subheader to a given MAC SDU and creates a MAC Rx PDU.
byte_buffer prepend_mac_subheader(lcid_t lcid, const byte_buffer& mac_sdu);

/// \brief Generate MAC UCI PDU from PUCCH information, with all HARQ-ACKs set to ACK, SRs set as not detected and CSI
/// set as 1s.
mac_uci_pdu create_uci_pdu(const pucch_info& pucch, bool detect_sr);

/// Generate MAC UCI PDU out of a PUSCH UCI PDU indication, with all HARQ-ACKs set to ACK and CSI set as 1s.
mac_uci_pdu create_uci_pdu(rnti_t rnti, const uci_info& pusch_uci);

mac_uci_indication_message create_uci_indication(slot_point sl_rx, span<const pucch_info> pucchs, bool detect_sr);

std::optional<mac_uci_indication_message> create_uci_indication(slot_point sl_rx, span<const ul_sched_info> puschs);

mac_crc_indication_message create_crc_indication(slot_point sl_rx, rnti_t rnti, harq_id_t h_id);

mac_crc_indication_message create_crc_indication(slot_point sl_rx, span<const ul_sched_info> puschs);

/// Angle of Arrival reported in the positioning SRS PDUs.
struct srs_positioning_aoa {
  /// Azimuth Angle of Arrival, in degrees.
  float azimuth_deg = 123.4F;
  /// Zenith Angle of Arrival, in degrees.
  float zenith_deg = 56.7F;
};

mac_srs_pdu create_srs_pdu(const srs_info& srs, const srs_positioning_aoa& aoa = {});

mac_srs_indication_message
create_srs_indication(slot_point sl_rx, span<const srs_info> srss, const srs_positioning_aoa& aoa = {});

/// Creates a RACH indication that reports one preamble detected in a scheduled PRACH occasion.
/// \param msg1_scs PRACH subcarrier spacing, ignored for long preamble formats.
mac_rach_indication create_rach_indication(slot_point                 prach_slot_rx,
                                           const prach_occasion_info& occasion,
                                           subcarrier_spacing         msg1_scs,
                                           unsigned                   preamble_id = 0);

} // namespace test_helpers
} // namespace ocudu
