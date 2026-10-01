// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pusch/pusch_decoder_result.h"
#include "ocudu/phy/upper/channel_processors/uci/uci_status.h"
#include "ocudu/phy/upper/channel_state_information.h"
#include "ocudu/ran/uci/uci_payload_type.h"

namespace ocudu {

/// Collects the results of UCI field decoding.
struct pusch_uci_field {
  uci_payload_type payload;
  uci_status       status;

  /// Clears the contents of the field.
  void clear()
  {
    payload.resize(0);
    status = uci_status::unknown;
  }
};

/// Groups the PUSCH processor UL-SCH data decode results.
struct pusch_processor_result_data {
  /// SCH decoding information.
  pusch_decoder_result data;
  /// Channel state information.
  channel_state_information csi;
};

/// Groups the PUSCH processor UL-SCH control decode results.
struct pusch_processor_result_control {
  /// \brief Decoded HARQ-ACK information bits.
  /// \remark Leave empty if no HARQ-ACK information bits are multiplexed with the UL-SCH.
  pusch_uci_field harq_ack;
  /// \brief Decoded CSI Part 1 information bits.
  /// \remark Leave empty if no CSI Part 1 information bits are multiplexed with the UL-SCH.
  pusch_uci_field csi_part1;
  /// \brief Decoded CSI Part 2 information bits.
  /// \remark Leave empty if no CSI Part 2 information bits are multiplexed with the UL-SCH.
  pusch_uci_field csi_part2;
  /// Channel state information.
  channel_state_information csi;
};

/// \brief PUSCH processor result notifier.
///
/// The PUSCH processor uses this interface for notifying results.
class pusch_processor_result_notifier
{
public:
  /// Default destructor.
  virtual ~pusch_processor_result_notifier() = default;

  /// \brief Notifies the detection of Uplink Control Information (UCI) on PUSCH.
  ///
  /// The UCI result is always notified after on_csi(), if the \ref pusch_processor::uci_description contains any
  /// payload bits. Otherwise, it is not notified.
  ///
  /// \param[in] uci Actual UCI decoding results.
  virtual void on_uci(const pusch_processor_result_control& uci) = 0;

  /// \brief Notifies the shared channel decoding.
  ///
  /// The SCH result is always notified after on_csi(), if the \ref pusch_processor::pdu_t::codeword is
  /// available.
  ///
  /// \param[in] sch Actual SCH decoding results.
  virtual void on_sch(const pusch_processor_result_data& sch) = 0;
};

} // namespace ocudu
