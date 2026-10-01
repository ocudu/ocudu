// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "o_du_high_metrics_notifier_proxy.h"
#include "ocudu/du/du_high/du_high.h"
#include "ocudu/du/du_high/du_manager/du_f1_setup_notifier.h"
#include "ocudu/du/du_high/o_du_high.h"
#include "ocudu/du/du_high/o_du_high_config.h"
#include "ocudu/du/du_operation_controller.h"
#include "ocudu/e2/e2.h"
#include "ocudu/fapi_adaptor/mac/mac_fapi_fastpath_adaptor.h"

namespace ocudu {

class mac_result_notifier;

namespace odu {

/// O-RAN DU high implementation dependencies.
struct o_du_high_impl_dependencies {
  ocudulog::basic_logger*                                  logger;
  std::unique_ptr<fapi_adaptor::mac_fapi_fastpath_adaptor> fapi_fastpath_adaptor;
  du_metrics_notifier*                                     metrics_notifier;
};

/// O-RAN DU high implementation.
class o_du_high_impl : public o_du_high, public du_operation_controller
{
public:
  o_du_high_impl(unsigned nof_cells_, o_du_high_impl_dependencies&& du_dependencies);

  // See interface for documentation.
  du_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  fapi_adaptor::mac_fapi_fastpath_adaptor& get_mac_fapi_fastpath_adaptor() override;

  // See interface for documentation.
  du_high& get_du_high() override;

  // See interface for documentation.
  void set_o_du_high_metrics_notifier(o_du_high_metrics_notifier& notifier) override;

  /// Sets the DU high to the given one.
  void set_du_high(std::unique_ptr<du_high> updated_du_high);

  /// Sets E2 components and takes ownership of both; keeps the E2 agent alive longer because the adapter depends on it.
  void set_e2_components(std::unique_ptr<e2_agent> agent, std::unique_ptr<du_f1_setup_complete_notifier> adapter);

  /// Returns the MAC result notifier of this O-RAN DU high.
  mac_result_notifier& get_mac_result_notifier() { return *du_high_result_notifier; }

  /// Returns the DU metrics notifier of this O-DU high implementation.
  du_metrics_notifier& get_du_metrics_notifier() { return metrics_notifier_poxy; }

private:
  const unsigned                                           nof_cells;
  ocudulog::basic_logger&                                  logger;
  o_du_high_metrics_notifier_proxy                         metrics_notifier_poxy;
  std::unique_ptr<fapi_adaptor::mac_fapi_fastpath_adaptor> fapi_fastpath_adaptor;
  std::unique_ptr<mac_result_notifier>                     du_high_result_notifier;
  std::unique_ptr<du_high>                                 du_hi;
  std::unique_ptr<e2_agent>                                e2agent;
  std::unique_ptr<du_f1_setup_complete_notifier>           f1_setup_e2_adapter;
};

} // namespace odu
} // namespace ocudu
