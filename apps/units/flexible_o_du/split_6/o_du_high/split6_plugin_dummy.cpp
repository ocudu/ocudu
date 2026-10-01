// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "split6_plugin_dummy.h"
#include "ocudu/du/du_high/du_high_configuration.h"
#include "ocudu/fapi/p5/p5_requests_gateway.h"
#include "ocudu/fapi/p7/p7_last_request_notifier.h"
#include "ocudu/fapi/p7/p7_requests_gateway.h"
#include "ocudu/fapi_adaptor/phy/p5/phy_fapi_p5_sector_adaptor.h"
#include "ocudu/fapi_adaptor/phy/p7/phy_fapi_p7_sector_adaptor.h"
#include "ocudu/fapi_adaptor/phy/phy_fapi_adaptor.h"

using namespace ocudu;

namespace ocudu::fapi {
struct dl_tti_request;
struct tx_data_request;
struct ul_dci_request;
struct ul_tti_request;
} // namespace ocudu::fapi

namespace {

/// Dummy FAPI adaptor implementation.
class fapi_adaptor_dummy : public fapi_adaptor::phy_fapi_adaptor,
                           fapi_adaptor::phy_fapi_sector_adaptor,
                           fapi_adaptor::phy_fapi_p7_sector_adaptor,
                           fapi_adaptor::phy_fapi_p5_sector_adaptor
{
  /// Dummy P5 gateway implementation.
  class p5_gateway_dummy : public fapi::p5_requests_gateway
  {
  public:
    void send_param_request(const fapi::param_request& msg) override {}
    void send_config_request(const fapi::config_request& msg) override {}
    void send_start_request(const fapi::start_request& msg) override {}
    void send_stop_request(const fapi::stop_request& msg) override {}
  };

  /// Dummy P7 requests gateway implementation.
  class p7_gateway_dummy : public fapi::p7_requests_gateway
  {
  public:
    // See interface for documentation.
    void send_dl_tti_request(const fapi::dl_tti_request& msg) override {}
    // See interface for documentation.
    void send_ul_tti_request(const fapi::ul_tti_request& msg) override {}
    // See interface for documentation.
    void send_ul_dci_request(const fapi::ul_dci_request& msg) override {}
    // See interface for documentation.
    void send_tx_data_request(const fapi::tx_data_request& msg) override {}
  };

  /// Dummy P7 last request notifier implementation.
  class p7_last_request_notifier_dummy : public fapi::p7_last_request_notifier
  {
  public:
    // See interface for documentation.
    void on_last_message(slot_point slot) override {}
  };

  p7_gateway_dummy               p7_gateway;
  p7_last_request_notifier_dummy p7_last_req_notifier;
  p5_gateway_dummy               p5_gateway;

public:
  // See interface for documentation.
  void start() override {}

  // See interface for documentation.
  void stop() override {}

  // See interface for documentation.
  phy_fapi_p7_sector_adaptor& get_p7_sector_adaptor() override { return *this; }

  // See interface for documentation.
  phy_fapi_p5_sector_adaptor& get_p5_sector_adaptor() override { return *this; }

  // See interface for documentation.
  phy_fapi_sector_adaptor& get_sector_adaptor(unsigned cell_id) override { return *this; }

  // See interface for documentation.
  fapi::p5_requests_gateway& get_p5_requests_gateway() override { return p5_gateway; }

  // See interface for documentation.
  void set_error_indication_notifier(fapi::error_indication_notifier& fapi_error_notifier) override {}

  // See interface for documentation.
  void set_p5_responses_notifier(fapi::p5_responses_notifier& p5_notifier) override {}

  // See interface for documentation.
  fapi::p7_requests_gateway& get_p7_requests_gateway() override { return p7_gateway; }

  // See interface for documentation.
  fapi::p7_last_request_notifier& get_p7_last_request_notifier() override { return p7_last_req_notifier; }

  // See interface for documentation.
  void set_p7_slot_indication_notifier(fapi::p7_slot_indication_notifier& notifier_) override {}

  // See interface for documentation.
  void set_p7_indications_notifier(fapi::p7_indications_notifier& notifier_) override {}
};

} // namespace

std::unique_ptr<fapi_adaptor::phy_fapi_adaptor>
split6_plugin_dummy::create_fapi_adaptor(const fapi_adaptor::split6_o_du_low_fapi_adaptor_configuration& fapi_cfg,
                                         const o_du_unit_dependencies&                                   dependencies)
{
  return std::make_unique<fapi_adaptor_dummy>();
}

#ifndef OCUDU_HAS_ENTERPRISE
std::unique_ptr<split6_plugin> ocudu::create_split6_plugin(std::string_view app_name)
{
  return std::make_unique<split6_plugin_dummy>();
}
#endif
