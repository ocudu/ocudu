// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "o_du_low_impl.h"
#include "ocudu/fapi_adaptor/phy/p7/phy_fapi_p7_sector_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/phy/phy_fapi_sector_fastpath_adaptor.h"
#include "ocudu/phy/upper/upper_phy.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace odu;

o_du_low_impl::o_du_low_impl(std::unique_ptr<du_low>                                  du_lo_,
                             std::unique_ptr<fapi_adaptor::phy_fapi_fastpath_adaptor> fapi_adaptor_,
                             unsigned                                                 nof_cells) :
  du_lo(std::move(du_lo_)),
  fapi_fastpath_adaptor(std::move(fapi_adaptor_)),
  metrics_collector(du_lo->get_metrics_collector())
{
  ocudu_assert(du_lo, "Invalid DU low");
  ocudu_assert(fapi_fastpath_adaptor, "Invalid PHY-FAPI adapter");

  for (unsigned i = 0; i != nof_cells; ++i) {
    upper_phy&                                      upper          = du_lo->get_upper_phy(i);
    fapi_adaptor::phy_fapi_sector_fastpath_adaptor& sector_adaptor = fapi_fastpath_adaptor->get_sector_adaptor(i);

    upper.set_rx_results_notifier(sector_adaptor.get_p7_sector_adaptor().get_rx_results_notifier());
    upper.set_timing_notifier(sector_adaptor.get_p7_sector_adaptor().get_timing_notifier());
    upper.set_error_notifier(sector_adaptor.get_p7_sector_adaptor().get_error_notifier());
  }
}

du_low& o_du_low_impl::get_du_low()
{
  return *du_lo;
}

fapi_adaptor::phy_fapi_fastpath_adaptor& o_du_low_impl::get_phy_fapi_fastpath_adaptor()
{
  return *fapi_fastpath_adaptor;
}

o_du_low_metrics_collector_impl* o_du_low_impl::get_metrics_collector()
{
  return metrics_collector.enabled() ? &metrics_collector : nullptr;
}

void o_du_low_impl::start()
{
  // Nothing to do as the FAPI adaptor and DU low are stateless.
}

void o_du_low_impl::stop()
{
  ocudu_assert(du_lo, "Invalid DU low object");
  du_lo->get_operation_controller().stop();
}
