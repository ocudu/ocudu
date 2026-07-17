// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/mac/ue_con_res_id.h"
#include "ocudu/ran/phy_time_unit.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/slot_pdu_capacity_constants.h"
#include "ocudu/ran/slot_point.h"
#include <optional>

namespace ocudu {

/// Describes a RACH indication.
struct mac_rach_indication {
  /// Describes the detection of a single preamble.
  struct rach_preamble {
    /// Index of the detected preamble. Possible values are {0, ..., 63}.
    unsigned index;
    /// Timing advance between the observed arrival time (for the considered UE) and the reference uplink time.
    phy_time_unit time_advance;
    /// Preamble received power in dBFS.
    std::optional<float> pwr_dBFS;
    /// Average SNR value in dB.
    std::optional<float> snr_dB;
  };

  /// Describes a single RACH occasion.
  struct rach_occasion {
    /// OFDM symbol index within the slot that marks the start of the acquisition window for the first time-domain PRACH
    /// occasion.
    unsigned start_symbol;
    /// The index of first slot of the PRACH TD occasion in a system frame.
    unsigned slot_index;
    /// The index of the received PRACH frequency domain occasion.
    unsigned frequency_index;
    /// Average value of RSSI in dBFS.
    std::optional<float> rssi_dBFS;
    /// List of detected preambles in this RACH occasion.
    static_vector<rach_preamble, MAX_PREAMBLES_PER_PRACH_OCCASION> preambles;
  };

  /// Slot point corresponding to the reception of this indication.
  slot_point slot_rx;
  /// List of RACH occasions carried in this indication.
  static_vector<rach_occasion, MAX_PRACH_OCCASIONS_PER_SLOT> occasions;
};

/// Interface used to handle RACH indications specific to a cell.
class mac_cell_rach_handler
{
public:
  virtual ~mac_cell_rach_handler() = default;

  /// \brief Handles incoming RACH indication from the L1.
  ///
  /// \param rach_ind Received RACH indication.
  virtual void handle_rach_indication(const mac_rach_indication& rach_ind) = 0;

  /// \brief Resolves the TC-RNTI allocated to a 2-step RACH (MsgA) PUSCH transmission, given its RA-RNTI and RAPID.
  /// If resolved, also registers the UE ConResId decoded from the same CCCH SDU, so it is
  /// later encoded in the successRAR MAC subPDU when this preamble's MsgB is scheduled.
  /// \return The TC-RNTI, if a matching, non-expired entry exists; std::nullopt otherwise.
  virtual std::optional<rnti_t>
  handle_msga_ccch_sdu(rnti_t ra_rnti, uint8_t rapid, slot_point sl_rx, const ue_con_res_id_t& con_res_id) = 0;

  /// \brief Resolves and consumes the UE ConResId registered for a TC-RNTI.
  /// \param tc_rnti TC-RNTI (echoed as the successRAR's C-RNTI) that the entry was registered with.
  /// \return The ConResId, if a matching entry exists; std::nullopt otherwise.
  virtual std::optional<ue_con_res_id_t> resolve_msga_con_res_id(rnti_t tc_rnti) = 0;
};

} // namespace ocudu
