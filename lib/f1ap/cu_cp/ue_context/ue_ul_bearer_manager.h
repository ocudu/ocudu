// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/ran/rnti.h"
#include <variant>

namespace ocudu::ocucp {

class f1ap_ul_ccch_notifier;
class f1ap_ul_dcch_notifier;

/// Handles the reception of an UL RRC PDU initiated by an UL RRC message transfer procedure.
class f1c_initial_ul_bearer_handler
{
public:
  virtual ~f1c_initial_ul_bearer_handler() = default;

  /// Handle the reception of an UL RRC PDU.
  virtual void handle_initial_ul_rrc_message(byte_buffer pdu, rnti_t c_rnti) = 0;
};

/// Handles the reception of an UL RRC PDU initiated by an UL RRC message transfer procedure.
class f1c_ul_bearer_handler
{
public:
  virtual ~f1c_ul_bearer_handler() = default;

  /// Handle the reception of an UL RRC PDU.
  virtual void handle_ul_rrc_message(byte_buffer pdu) = 0;
};

/// Manages the state of the uplink path for the Signaling Radio Bearers (SRBs) of an individual UE.
class ue_ul_bearer_manager
{
public:
  /// Activate SRB0.
  void activate_srb0(f1ap_ul_ccch_notifier& f1ap_srb0_notifier);

  /// Activate SRB1.
  void activate_srb1(f1ap_ul_dcch_notifier& f1ap_srb1_notifier);

  /// Activate SRB2.
  void activate_srb2(f1ap_ul_dcch_notifier& f1ap_srb2_notifier);

  /// Is SRB active.
  bool is_srb_active(srb_id_t srb_id)
  {
    if (srb_id == srb_id_t::srb0) {
      return f1c_initial_ul_bearer != nullptr;
    }
    return f1c_ul_bearers.contains(srb_id_to_uint(srb_id));
  }

  /// Returns a pointer to the SRB bearer handler, or nullptr if the SRB has not been activated.
  std::variant<f1c_initial_ul_bearer_handler*, f1c_ul_bearer_handler*> get_srb(srb_id_t srb_id)
  {
    if (srb_id == srb_id_t::srb0) {
      return f1c_initial_ul_bearer.get();
    }
    if (!f1c_ul_bearers.contains(srb_id_to_uint(srb_id))) {
      return (f1c_ul_bearer_handler*)nullptr;
    }
    return f1c_ul_bearers[srb_id_to_uint(srb_id)].get();
  }

private:
  std::unique_ptr<f1c_initial_ul_bearer_handler>                      f1c_initial_ul_bearer;
  slotted_array<std::unique_ptr<f1c_ul_bearer_handler>, MAX_NOF_SRBS> f1c_ul_bearers;
};

} // namespace ocudu::ocucp
