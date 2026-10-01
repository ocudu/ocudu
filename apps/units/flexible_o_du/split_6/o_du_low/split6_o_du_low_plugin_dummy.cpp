// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split6_o_du_low_plugin_dummy.h"
#include "ocudu/adt/format.h"
#include "ocudu/fapi/common/error_indication_notifier.h"
#include "ocudu/fapi/p5/p5_responses_notifier.h"
#include "ocudu/fapi/p7/p7_indications_notifier.h"
#include "ocudu/fapi/p7/p7_slot_indication_notifier.h"
#include "ocudu/fapi_adaptor/mac/operation_controller.h"

using namespace ocudu;

namespace {

/// FAPI operation controller dummy implementation.
class operation_controller_dummy : public fapi_adaptor::operation_controller
{
public:
  void start() override {}
  void stop() override {}
};

/// FAPI error indication notifier dummy implementation.
class error_indication_notifier_dummy : public fapi::error_indication_notifier
{
public:
  void on_error_indication(const fapi::error_indication& msg) override {}
};

/// FAPI P7 indications notifier dummy implementation.
class p7_indications_notifier_dummy : public fapi::p7_indications_notifier
{
public:
  void on_rx_data_indication(const fapi::rx_data_indication& msg) override {}
  void on_crc_indication(const fapi::crc_indication& msg) override {}
  void on_uci_indication(const fapi::uci_indication& msg) override {}
  void on_srs_indication(const fapi::srs_indication& msg) override {}
  void on_rach_indication(const fapi::rach_indication& msg) override {}
};

/// FAPI P7 slot indication notifier dummy implementation.
class p7_slot_indication_notifier_dummy : public fapi::p7_slot_indication_notifier
{
public:
  void on_slot_indication(const fapi::slot_indication& msg) override {}
};

/// MAC-FAPI P7 sector adaptor dummy implementation.
class mac_fapi_p7_sector_adaptor_dummy : public fapi_adaptor::mac_fapi_p7_sector_adaptor
{
  p7_indications_notifier_dummy     dummy_p7_indications_notifier;
  p7_slot_indication_notifier_dummy dummy_p7_slot_indication_notifier;
  error_indication_notifier_dummy   dummy_error_notifier;

public:
  // See interface for documentation.
  fapi::p7_indications_notifier& get_p7_indications_notifier() override { return dummy_p7_indications_notifier; }

  // See interface for documentation.
  fapi::p7_slot_indication_notifier& get_p7_slot_indication_notifier() override
  {
    return dummy_p7_slot_indication_notifier;
  }

  // See interface for documentation.
  fapi::error_indication_notifier& get_error_indication_notifier() override { return dummy_error_notifier; }
};

/// MAC-FAPI P7 sector adaptor factory dummy implementation.
class mac_fapi_p7_sector_adaptor_factory_dummy : public fapi_adaptor::mac_fapi_p7_sector_adaptor_factory
{
  // See interface for documentation.
  std::unique_ptr<fapi_adaptor::mac_fapi_p7_sector_adaptor> create(const fapi::cell_configuration& fapi_cfg,
                                                                   fapi::p7_requests_gateway&      p7_gateway,
                                                                   fapi::p7_last_request_notifier& p7_last_req_notifier,
                                                                   ru_controller&                  ru_ctrl) override
  {
    return std::make_unique<mac_fapi_p7_sector_adaptor_dummy>();
  }
};

/// FAPI config message notifier dummy implementation.
class p5_notifier_dummy : public fapi::p5_responses_notifier
{
public:
  void on_param_response(const fapi::param_response& msg) override {}
  void on_config_response(const fapi::config_response& msg) override {}
  void on_stop_indication(const fapi::stop_indication& msg) override {}
};

/// MAC-FAPI P5 sector adaptor dummy implementation.
class mac_fapi_p5_sector_adaptor_dummy : public fapi_adaptor::mac_fapi_p5_sector_adaptor
{
  operation_controller_dummy      dummy_controller;
  p5_notifier_dummy               dummy_config_notifier;
  error_indication_notifier_dummy dummy_error_notifier;

public:
  // See interface for documentation.
  fapi::p5_responses_notifier& get_p5_responses_notifier() override { return dummy_config_notifier; }

  // See interface for documentation.
  fapi::error_indication_notifier& get_error_indication_notifier() override { return dummy_error_notifier; }

  // See interface for documentation.
  fapi_adaptor::operation_controller& get_operation_controller() override { return dummy_controller; }
};

} // namespace

std::unique_ptr<fapi_adaptor::mac_fapi_p5_sector_adaptor>
split6_o_du_low_plugin_dummy::create_fapi_p5_sector_adaptor(fapi::p5_requests_gateway& p5_gateway,
                                                            task_executor&             executor,
                                                            task_executor&             control_executor)
{
  return std::make_unique<mac_fapi_p5_sector_adaptor_dummy>();
}

std::unique_ptr<fapi_adaptor::mac_fapi_p7_sector_adaptor_factory>
split6_o_du_low_plugin_dummy::create_fapi_p7_sector_adaptor_factory(task_executor& executor,
                                                                    task_executor& control_executor)
{
  return std::make_unique<mac_fapi_p7_sector_adaptor_factory_dummy>();
}

#ifndef OCUDU_HAS_SPLIT6_ENTERPRISE
std::unique_ptr<split6_o_du_low_plugin> ocudu::create_split6_o_du_low_plugin(std::string_view app_name)
{
  return std::make_unique<split6_o_du_low_plugin_dummy>();
}
#endif
