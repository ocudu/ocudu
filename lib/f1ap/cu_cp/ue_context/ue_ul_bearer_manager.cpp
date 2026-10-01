// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_ul_bearer_manager.h"
#include "ocudu/adt/format.h"
#include "ocudu/f1ap/cu_cp/f1ap_cu.h"

using namespace ocudu;
using namespace ocucp;

namespace {

/// Handles uplink PDUs that are transported in the CCCH.
class f1c_srb0_ul_handler : public f1c_initial_ul_bearer_handler
{
public:
  explicit f1c_srb0_ul_handler(f1ap_ul_ccch_notifier& ul_ccch_notifier_) : ul_ccch_notifier(ul_ccch_notifier_) {}

  // See interface for the documentation.
  void handle_initial_ul_rrc_message(byte_buffer pdu, rnti_t c_rnti) override
  {
    ul_ccch_notifier.on_ul_ccch_pdu(std::move(pdu), c_rnti);
  }

private:
  f1ap_ul_ccch_notifier& ul_ccch_notifier;
};

/// Handles uplink PDUs that are transported in the DCCH.
class f1c_other_cu_bearer : public f1c_ul_bearer_handler
{
public:
  explicit f1c_other_cu_bearer(f1ap_ul_dcch_notifier& ul_dcch_notifier_) : ul_dcch_notifier(ul_dcch_notifier_) {}

  // See interface for the documentation.
  void handle_ul_rrc_message(byte_buffer pdu) override { ul_dcch_notifier.on_ul_dcch_pdu(std::move(pdu)); }

private:
  f1ap_ul_dcch_notifier& ul_dcch_notifier;
};

} // namespace

void ue_ul_bearer_manager::activate_srb0(f1ap_ul_ccch_notifier& f1ap_srb0_notifier)
{
  f1c_initial_ul_bearer = std::make_unique<f1c_srb0_ul_handler>(f1ap_srb0_notifier);
}

void ue_ul_bearer_manager::activate_srb1(f1ap_ul_dcch_notifier& f1ap_srb1_notifier)
{
  f1c_ul_bearers.emplace(srb_id_to_uint(srb_id_t::srb1), std::make_unique<f1c_other_cu_bearer>(f1ap_srb1_notifier));
}

void ue_ul_bearer_manager::activate_srb2(f1ap_ul_dcch_notifier& f1ap_srb2_notifier)
{
  f1c_ul_bearers.emplace(srb_id_to_uint(srb_id_t::srb2), std::make_unique<f1c_other_cu_bearer>(f1ap_srb2_notifier));
}
