// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../procedures/e2ap_indication_procedure.h"
#include "ocudu/asn1/e2ap/e2ap.h"
#include "ocudu/asn1/e2sm/e2sm_kpm_ies.h"
#include "ocudu/e2/e2.h"
#include "ocudu/e2/e2sm/e2sm_manager.h"
#include <map>

namespace ocudu {

class e2_subscription_manager_impl : public e2_subscription_manager
{
public:
  explicit e2_subscription_manager_impl(e2sm_manager& e2sm_mngr_);
  virtual ~e2_subscription_manager_impl() = default;

  void stop() override;

  /// \brief  Handles the subscription request message.
  /// \param[in] msg  The subscription request message.
  /// \return The subscription response message.
  e2_subscribe_reponse_message handle_subscription_setup(const asn1::e2ap::ric_sub_request_s& msg) override;

  /// \brief  Handles the subscription delete request message.
  /// \param[in] msg The subscription delete request message.
  /// \return The subscription delete response message.
  e2_subscribe_delete_response_message
  handle_subscription_delete(const asn1::e2ap::ric_sub_delete_request_s& msg) override;

  /// \brief  Starts the subscription procedure associated with the given ric instance id.
  /// \param[in] ric_request_id  The ric request id.
  /// \param[in] ran_func_id  The RAN function id.
  /// \param[in] ev_mng The event manager that will be used to start the subscription procedure.
  /// \param[in] tx_pdu_notifier The E2 message notifier.
  void start_subscription(const asn1::e2ap::ric_request_id_s& ric_request_id,
                          uint16_t                            ran_func_id,
                          e2_event_manager&                   ev_mng,
                          e2_message_notifier&                tx_pdu_notifier) override;

  /// @brief  Stops the subscription procedure associated with the given ric instance id.
  /// @param[in] ric_request_id  The ric request id.
  /// \param[in] ev_mng The event manager that will be used to end the subscription procedure.
  void stop_subscription(const asn1::e2ap::ric_request_id_s&         ric_request_id,
                         e2_event_manager&                           ev_mng,
                         const asn1::e2ap::ric_sub_delete_request_s& msg) override;

  /// \brief checks whether the given action is supported.
  /// \param[in] action The action to check.
  /// \param[in] ran_func_id The ran function id.
  /// \param[in] ric_request_id_s associated with the subscription.
  /// \return true if the action is supported, false otherwise.
  bool action_supported(const asn1::e2ap::ric_action_to_be_setup_item_s& action,
                        uint16_t                                         ran_func_id,
                        const asn1::e2ap::ric_request_id_s&              ric_request_id);

  /// \brief  Gets the subscription outcome based on the subscription.
  /// \param[in]  ran_func_id The ran function id.
  /// \param[out] outcome The subscription response message.
  /// \param[in]  subscription  The subscription to use.
  /// \param[in]  actions The actions that have been requested.
  void get_subscription_result(uint16_t                                          ran_func_id,
                               e2_subscribe_reponse_message&                     outcome,
                               e2_subscription_t&                                subscription,
                               const asn1::e2ap::ric_actions_to_be_setup_list_l& actions);

  /// \brief Adds an e2sm service to the list of services that can be used to unpack e2sm messages.
  /// \param[in] oid The oid of the e2sm service.
  /// \param[in] e2sm_packer The interface that will be used to unpack the e2sm messages for this service
  void add_e2sm_service(std::string oid, std::unique_ptr<e2sm_interface> e2sm_iface) override;

  /// \brief Gets the e2sm service interface associated with the given oid.
  /// \param[in] oid The oid of the e2sm service.
  /// \return The e2sm service interface associated with the given oid.
  e2sm_interface* get_e2sm_interface(std::string oid) override;

  /// @brief Adds a supported ran function oid.
  /// @param ran_func_id  The ran function id.
  /// @param oid  The oid of the e2sm service associated with the ran function.
  void add_ran_function_oid(uint16_t ran_func_id, std::string oid) override;

private:
  using e2_subscription_key_t = std::tuple<uint32_t, uint32_t>;
  std::map<e2_subscription_key_t, e2_subscription_t>     subscriptions;
  std::map<std::string, std::unique_ptr<e2sm_interface>> e2sm_iface_list;
  std::map<uint16_t, std::string>                        supported_ran_functions;
  e2sm_manager&                                          e2sm_mngr;
  ocudulog::basic_logger&                                logger;
};

} // namespace ocudu
