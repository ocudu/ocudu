// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "o_du_high_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/fapi_adaptor/mac/mac_fapi_sector_fastpath_adaptor.h"
#include "ocudu/fapi_adaptor/mac/p7/mac_fapi_p7_sector_fastpath_adaptor.h"
#include "ocudu/mac/mac_cell_result.h"

using namespace ocudu;
using namespace odu;

namespace {
/// Dummy implementation of the mac_result_notifier.
class phy_dummy : public mac_result_notifier
{
  std::vector<std::reference_wrapper<mac_cell_result_notifier>> cells;

public:
  explicit phy_dummy(std::vector<std::reference_wrapper<mac_cell_result_notifier>> cells_) : cells(std::move(cells_)) {}

  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override
  {
    return cells[static_cast<unsigned>(cell_index)].get();
  }
};

} // namespace

o_du_high_impl::o_du_high_impl(unsigned nof_cells_, o_du_high_impl_dependencies&& du_dependencies) :
  nof_cells(nof_cells_),
  logger(*du_dependencies.logger),
  metrics_notifier_poxy(du_dependencies.metrics_notifier),
  fapi_fastpath_adaptor(std::move(du_dependencies.fapi_fastpath_adaptor)),
  du_high_result_notifier([](fapi_adaptor::mac_fapi_fastpath_adaptor& fapi_adaptor, unsigned num_cells) {
    std::vector<std::reference_wrapper<mac_cell_result_notifier>> cells;
    for (unsigned i = 0; i != num_cells; ++i) {
      cells.push_back(std::ref(fapi_adaptor.get_sector_adaptor(i).get_p7_sector_adaptor().get_cell_result_notifier()));
    }

    return std::make_unique<phy_dummy>(std::move(cells));
  }(*fapi_fastpath_adaptor, nof_cells))
{
  ocudu_assert(fapi_fastpath_adaptor, "Invalid FAPI MAC adaptor");
}

void o_du_high_impl::start()
{
  ocudu_assert(du_hi, "Invalid DU high");

  logger.info("Starting DU...");
  du_hi->start();

  if (e2agent) {
    e2agent->start();
  }

  // Configure the FAPI -> DU interface.
  for (unsigned i = 0; i != nof_cells; ++i) {
    du_cell_index_t cell_id        = to_du_cell_index(i);
    auto&           sector_adaptor = fapi_fastpath_adaptor->get_sector_adaptor(i).get_p7_sector_adaptor();
    sector_adaptor.set_cell_slot_handler(du_hi->get_slot_handler(cell_id));
    sector_adaptor.set_cell_rach_handler(du_hi->get_rach_handler(cell_id));
    sector_adaptor.set_cell_pdu_handler(du_hi->get_pdu_handler());
    sector_adaptor.set_cell_crc_handler(du_hi->get_control_info_handler(cell_id));
  }

  logger.info("DU started successfully");
}

void o_du_high_impl::stop()
{
  ocudu_assert(du_hi, "Invalid DU high");

  logger.info("Stopping DU...");
  if (e2agent) {
    e2agent->stop();
  }

  du_hi->stop();
  logger.info("DU stopped successfully");
}

fapi_adaptor::mac_fapi_fastpath_adaptor& o_du_high_impl::get_mac_fapi_fastpath_adaptor()
{
  return *fapi_fastpath_adaptor;
}

du_high& o_du_high_impl::get_du_high()
{
  return *du_hi;
}

void o_du_high_impl::set_o_du_high_metrics_notifier(o_du_high_metrics_notifier& notifier)
{
  metrics_notifier_poxy.set_o_du_high_metrics_notifier(notifier);
}

void o_du_high_impl::set_du_high(std::unique_ptr<du_high> updated_du_high)
{
  du_hi = std::move(updated_du_high);
  ocudu_assert(du_hi, "Invalid DU high");
}

void o_du_high_impl::set_e2_components(std::unique_ptr<e2_agent>                      agent,
                                       std::unique_ptr<du_f1_setup_complete_notifier> adapter)
{
  e2agent             = std::move(agent);
  f1_setup_e2_adapter = std::move(adapter);

  ocudu_assert(e2agent, "Invalid E2 agent");
  ocudu_assert(f1_setup_e2_adapter, "Invalid F1 setup E2 adapter");
}
