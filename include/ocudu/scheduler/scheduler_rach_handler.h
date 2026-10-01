// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/phy_time_unit.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/slot_pdu_capacity_constants.h"
#include "ocudu/ran/slot_point.h"
#include <optional>

namespace ocudu {

/// RACH indication Message. It contains all the RACHs detected in a given slot and cell.
struct rach_indication_message {
  du_cell_index_t cell_index;
  slot_point      slot_rx;

  struct preamble {
    /// Preamble logical index. Values: {0,...,63}.
    uint8_t preamble_id;
    /// Allocated TC-RNTI, for Contention-based RACH, or C-RNTI, for Contention-free RACH.
    rnti_t        tc_rnti;
    phy_time_unit time_advance;
    /// Average SNR value in dB, if available.
    std::optional<float> snr_dB;
  };

  struct occasion {
    /// Index of the first OFDM Symbol where RACH was detected. Values: {0,...,13}.
    uint8_t start_symbol;
    /// \brief Index of the first slot of the PRACH occasion in a system frame, as reported by the PHY.
    ///
    /// The t_id of the RA-RNTI and MsgB-RNTI (TS 38.321 Sections 5.1.3 and 5.1.3a), counted in slots of the PRACH
    /// subcarrier spacing. Values: {0,...,79}.
    uint8_t slot_index;
    /// Frequency domain occasion index. Values: {0,...,7}.
    uint8_t                                                   frequency_index;
    static_vector<preamble, MAX_PREAMBLES_PER_PRACH_OCCASION> preambles;
  };

  static_vector<occasion, MAX_PRACH_OCCASIONS_PER_SLOT> occasions;
};

class scheduler_rach_handler
{
public:
  virtual ~scheduler_rach_handler() = default;

  /// \brief Handle RACH indication message.
  virtual void handle_rach_indication(const rach_indication_message& msg) = 0;
};

} // namespace ocudu
