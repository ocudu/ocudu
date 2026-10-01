// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "upper_phy_impl.h"
#include "upper_phy_rx_symbol_handler_printer_decorator.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/upper/upper_phy_operation_controller.h"
#include "ocudu/phy/upper/upper_phy_timing_notifier.h"

using namespace ocudu;

namespace {

/// Dummy implementation of an upper PHY timing notifier.
class upper_phy_timing_notifier_dummy : public upper_phy_timing_notifier
{
public:
  void on_tti_boundary(const upper_phy_timing_context& context) override {}
};

} // namespace

static upper_phy_timing_notifier_dummy notifier_dummy;

upper_phy_impl::upper_phy_impl(upper_phy_impl_config&& config) :
  logger(ocudulog::fetch_basic_logger("PHY", true)),
  metrics_collector(std::move(config.metrics_collector)),
  rx_buf_pool(std::move(config.rx_buf_pool)),
  dl_rg_pool(std::move(config.dl_rg_pool)),
  dl_processor_pool(std::move(config.dl_processor_pool)),
  ul_processor_pool(std::move(config.ul_processor_pool)),
  dl_pdu_validator(std::move(config.dl_pdu_validator)),
  ul_pdu_validator(std::move(config.ul_pdu_validator)),
  ul_request_processor(*config.rx_symbol_request_notifier,
                       config.prach_buffers,
                       ocudulog::fetch_basic_logger("PHY", true)),
  rx_results_notifier(std::move(config.rx_results_notifier)),
  rx_symbol_handler(std::move(config.rx_symbol_handler)),
  timing_handler(notifier_dummy),
  error_handler(ul_processor_pool->get_slot_processor_pool())
{
  ocudu_assert(dl_processor_pool, "Invalid downlink processor pool");
  ocudu_assert(dl_rg_pool, "Invalid downlink resource grid pool");
  ocudu_assert(ul_processor_pool, "Invalid uplink processor pool");
  ocudu_assert(rx_buf_pool, "Invalid receive buffer pool");
  ocudu_assert(dl_pdu_validator, "Invalid downlink PDU validator");
  ocudu_assert(ul_pdu_validator, "Invalid uplink PDU validator");
  ocudu_assert(rx_results_notifier, "Invalid receive results notifier");
  ocudu_assert(rx_symbol_handler, "Invalid Rx symbol handler");

  // Interpose the operation controller's timing proxy between the timing handler and the external
  // notifier, so the controller's start and stop operations control the propagation of slot boundary
  // events towards higher layers. The proxy is a stable object owned by the operation controller;
  // only the downstream notifier is connected later, in set_timing_notifier().
  timing_handler.set_upper_phy_notifier(operation_controller.get_timing_notifier_proxy());

  logger.set_level(config.log_level);
}

upper_phy_operation_controller& upper_phy_impl::get_operation_controller()
{
  return operation_controller;
}

upper_phy_error_handler& upper_phy_impl::get_error_handler()
{
  return error_handler;
}

upper_phy_rx_symbol_handler& upper_phy_impl::get_rx_symbol_handler()
{
  return *rx_symbol_handler;
}

upper_phy_timing_handler& upper_phy_impl::get_timing_handler()
{
  return timing_handler;
}

downlink_processor_pool& upper_phy_impl::get_downlink_processor_pool()
{
  return *dl_processor_pool;
}

resource_grid_pool& upper_phy_impl::get_downlink_resource_grid_pool()
{
  return *dl_rg_pool;
}

uplink_request_processor& upper_phy_impl::get_uplink_request_processor()
{
  return ul_request_processor;
}

uplink_pdu_slot_repository_pool& upper_phy_impl::get_uplink_pdu_slot_repository()
{
  return ul_processor_pool->get_slot_pdu_repository_pool();
}

upper_phy_metrics_collector* upper_phy_impl::get_metrics_collector()
{
  return metrics_collector ? metrics_collector.get() : nullptr;
}

void upper_phy_impl::set_error_notifier(upper_phy_error_notifier& notifier)
{
  error_handler.set_error_notifier(notifier);
}

void upper_phy_impl::set_timing_notifier(ocudu::upper_phy_timing_notifier& notifier)
{
  // The operation controller's timing proxy is already interposed in the timing handler (see the
  // constructor). Connect the external notifier that receives the forwarded slot boundary events.
  operation_controller.connect_timing_notifier(notifier);
}

void upper_phy_impl::set_rx_results_notifier(upper_phy_rx_results_notifier& notifier)
{
  rx_results_notifier->connect(notifier);
}

const uplink_pdu_validator& upper_phy_impl::get_uplink_pdu_validator() const
{
  return *ul_pdu_validator;
}

const downlink_pdu_validator& upper_phy_impl::get_downlink_pdu_validator() const
{
  return *dl_pdu_validator;
}

void upper_phy_impl::stop()
{
  rx_buf_pool->stop();
  dl_processor_pool->stop();
  ul_processor_pool->stop();
}
