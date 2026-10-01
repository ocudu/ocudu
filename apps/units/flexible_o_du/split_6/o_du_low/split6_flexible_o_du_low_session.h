// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "split6_o_du_low_metrics_collector_impl.h"
#include "ocudu/du/du_low/o_du_low.h"
#include "ocudu/fapi_adaptor/mac/p7/mac_fapi_p7_sector_adaptor.h"
#include "ocudu/ru/ru.h"
#include "ocudu/ru/ru_adapters.h"
#include <memory>

namespace ocudu {

class split6_flexible_o_du_low_metrics_notifier;

/// \brief Split 6 flexible O-DU low session.
///
/// This session glues the FAPI slot messages adaptor, the O-DU low and the RU in a single object, managing
/// the lifetime of the objects related to an active cell. When a cell starts, this object must be created, and when it
/// is stopped, this object can safely be destroyed.
class split6_flexible_o_du_low_session
{
  static constexpr unsigned NOF_CELLS_SUPPORTED = 1U;

public:
  split6_flexible_o_du_low_session(split6_flexible_o_du_low_metrics_notifier* notifier_,
                                   std::chrono::milliseconds                  report_period_) :
    report_period(report_period_),
    notifier(notifier_),
    ru_ul_adapt(NOF_CELLS_SUPPORTED),
    ru_timing_adapt(NOF_CELLS_SUPPORTED),
    ru_error_adapt(NOF_CELLS_SUPPORTED)
  {
  }

  ~split6_flexible_o_du_low_session();

  /// Sets the dependencies to the given one
  void set_dependencies(std::unique_ptr<fapi_adaptor::mac_fapi_p7_sector_adaptor> slot_msg_adaptor,
                        std::unique_ptr<odu::o_du_low>                            du,
                        std::unique_ptr<radio_unit>                               radio,
                        unique_timer                                              timer);

  /// Getters to the adaptors.
  upper_phy_ru_ul_adapter&         get_upper_ru_ul_adapter() { return ru_ul_adapt; }
  upper_phy_ru_timing_adapter&     get_upper_ru_timing_adapter() { return ru_timing_adapt; }
  upper_phy_ru_error_adapter&      get_upper_ru_error_adapter() { return ru_error_adapt; }
  upper_phy_ru_dl_rg_adapter&      get_upper_ru_dl_rg_adapter() { return ru_dl_rg_adapt; }
  upper_phy_ru_ul_request_adapter& get_upper_ru_ul_request_adapter() { return ru_ul_request_adapt; }

private:
  std::chrono::milliseconds                                 report_period{0};
  split6_flexible_o_du_low_metrics_notifier*                notifier;
  upper_phy_ru_ul_adapter                                   ru_ul_adapt;
  upper_phy_ru_timing_adapter                               ru_timing_adapt;
  upper_phy_ru_error_adapter                                ru_error_adapt;
  std::unique_ptr<radio_unit>                               ru;
  std::unique_ptr<odu::o_du_low>                            odu_low;
  std::unique_ptr<fapi_adaptor::mac_fapi_p7_sector_adaptor> mac_p7_adaptor;
  upper_phy_ru_dl_rg_adapter                                ru_dl_rg_adapt;
  upper_phy_ru_ul_request_adapter                           ru_ul_request_adapt;
  split6_o_du_low_metrics_collector_impl                    metrics_collector;
};

} // namespace ocudu
