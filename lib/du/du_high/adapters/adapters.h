// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_manager/du_manager.h"
#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/mac/mac.h"
#include "ocudu/mac/mac_metrics_notifier.h"

namespace ocudu {
namespace odu {

class du_manager_mac_event_indicator : public mac_ul_ccch_notifier, public mac_metrics_notifier
{
public:
  void connect(du_manager_mac_event_handler& du_mng_, du_manager_mac_metric_aggregator& metrics_handler_)
  {
    du_mng          = &du_mng_;
    metrics_handler = &metrics_handler_;
  }

  void on_ul_ccch_msg_received(const ul_ccch_indication_message& msg) override
  {
    du_mng->handle_ul_ccch_indication(msg);
  }

  void on_crnti_ce_received(const ul_crnti_ce_indication_message& msg) override
  {
    du_mng->handle_crnti_ce_indication(msg);
  }

  void on_new_metrics_report(const mac_metric_report& report) override
  {
    metrics_handler->aggregate_mac_metrics_report(report);
  }

private:
  du_manager_mac_event_handler*     du_mng          = nullptr;
  du_manager_mac_metric_aggregator* metrics_handler = nullptr;
};

class mac_f1ap_paging_handler : public f1ap_du_paging_notifier
{
public:
  void connect(mac_paging_information_handler& mac_) { mac = &mac_; }

  void on_paging_received(const paging_information& msg) override { mac->handle_paging_information(msg); }

private:
  mac_paging_information_handler* mac = nullptr;
};

} // namespace odu
} // namespace ocudu
