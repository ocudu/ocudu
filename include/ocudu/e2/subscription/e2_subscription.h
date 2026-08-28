// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/asn1_utils.h"
#include "ocudu/asn1/e2ap/e2ap.h"
#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2_event_manager.h"
#include "ocudu/e2/e2sm/e2sm.h"
#include "ocudu/support/async/eager_async_task.h"

namespace ocudu {

/// This defines the RIC action struct.
struct ric_action_t {
  byte_buffer                          action_definition;
  uint16_t                             ric_action_id;
  asn1::e2ap::ric_action_type_e        ric_action_type;
  std::unique_ptr<e2sm_report_service> report_service;
};

/// Here we define a subscription struct.
struct e2_subscription_info_t {
  asn1::e2ap::ric_request_id_s request_id;
  uint16_t                     ran_function_id;
  std::vector<ric_action_t>    action_list;
  uint64_t                     report_period;
};

struct e2_subscription_t {
  e2_subscription_info_t subscription_info;
  eager_async_task<void> indication_task;
};

class e2_subscription_proc
{
public:
  virtual ~e2_subscription_proc() = default;
  /// \brief Handle the incoming subscription message.
  virtual e2_subscribe_reponse_message handle_subscription_setup(const asn1::e2ap::ric_sub_request_s& msg) = 0;
  /// \brief Handle the incoming subscription delete message.
  virtual e2_subscribe_delete_response_message
  handle_subscription_delete(const asn1::e2ap::ric_sub_delete_request_s& msg) = 0;
  /// \brief start the subscription request
  virtual void start_subscription(const asn1::e2ap::ric_request_id_s& ric_request_id,
                                  uint16_t                            ran_func_id,
                                  e2_event_manager&                   ev_mng,
                                  e2_message_notifier&                tx_pdu_notifier) = 0;
  /// \brief void stop the subscription request
  virtual void stop_subscription(const asn1::e2ap::ric_request_id_s&         ric_request_id,
                                 e2_event_manager&                           ev_mng,
                                 const asn1::e2ap::ric_sub_delete_request_s& msg) = 0;
};

class e2_subscriber_mgmt
{
public:
  virtual ~e2_subscriber_mgmt() = default;
  /// \brief  adds e2sm service to the subscriber
  /// \param oid The oid of the e2sm service
  /// \param e2sm_iface pointer to the e2sm interface
  virtual void add_e2sm_service(std::string oid, std::unique_ptr<e2sm_interface> e2sm_iface) = 0;

  virtual e2sm_interface* get_e2sm_interface(const std::string oid) = 0;

  virtual void add_ran_function_oid(uint16_t ran_func_id, std::string oid) = 0;

  virtual void stop() = 0;
};

class e2_subscription_manager : public e2_subscription_proc, public e2_subscriber_mgmt
{
public:
  virtual ~e2_subscription_manager() = default;
};

} // namespace ocudu
