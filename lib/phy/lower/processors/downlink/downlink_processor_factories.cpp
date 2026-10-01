// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/lower/processors/downlink/downlink_processor_factories.h"
#include "downlink_processor_impl.h"
#include "ocudu/phy/lower/processors/downlink/pdxch/pdxch_processor_factories.h"

using namespace ocudu;

namespace {

class lower_phy_downlink_processor_factory_sw : public lower_phy_downlink_processor_factory
{
public:
  lower_phy_downlink_processor_factory_sw(std::shared_ptr<pdxch_processor_factory> pdxch_proc_factory_) :
    pdxch_proc_factory(std::move(pdxch_proc_factory_))
  {
    ocudu_assert(pdxch_proc_factory, "Invalid PDxCH processor factory.");
  }

  std::unique_ptr<lower_phy_downlink_processor> create(const downlink_processor_configuration& config,
                                                       task_executor& modulation_executor) override
  {
    pdxch_processor_configuration pdxch_proc_config = {.cp              = config.cp,
                                                       .scs             = config.scs,
                                                       .srate           = config.rate,
                                                       .bandwidth_rb    = config.bandwidth_prb,
                                                       .center_freq_Hz  = config.center_frequency_Hz,
                                                       .tx_ant_topology = config.tx_ant_topology};

    downlink_processor_baseband_configuration baseband_config = {
        .sector_id               = config.sector_id,
        .scs                     = config.scs,
        .cp                      = config.cp,
        .rate                    = config.rate,
        .nof_tx_ports            = get_total_nof_ports(config.tx_ant_topology),
        .nof_slot_tti_in_advance = config.nof_slot_tti_in_advance};

    return std::make_unique<downlink_processor_impl>(pdxch_proc_factory->create(pdxch_proc_config, modulation_executor),
                                                     baseband_config);
  }

private:
  std::shared_ptr<pdxch_processor_factory> pdxch_proc_factory;
};

} // namespace

std::shared_ptr<lower_phy_downlink_processor_factory>
ocudu::create_downlink_processor_factory_sw(std::shared_ptr<pdxch_processor_factory> pdxch_proc_factory)
{
  return std::make_shared<lower_phy_downlink_processor_factory_sw>(std::move(pdxch_proc_factory));
}
