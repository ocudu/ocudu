// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ofh/ethernet/ethernet_factories.h"
#include "ethernet_frame_builder_impl.h"
#include "ethernet_receiver_impl.h"
#include "ethernet_transmitter_impl.h"
#include "vlan_ethernet_frame_builder_impl.h"
#include "vlan_ethernet_frame_decoder_impl.h"

using namespace ocudu;
using namespace ether;

std::unique_ptr<transmitter> ocudu::ether::create_transmitter(const transmitter_config& config,
                                                              ocudulog::basic_logger&   logger)
{
  return std::make_unique<transmitter_impl>(config, logger);
}

std::unique_ptr<receiver>
ocudu::ether::create_receiver(const receiver_config& config, task_executor& executor, ocudulog::basic_logger& logger)
{
  return std::make_unique<receiver_impl>(config, executor, logger);
}

std::unique_ptr<frame_builder> ocudu::ether::create_vlan_frame_builder(const vlan_frame_params& eth_params)
{
  return std::make_unique<vlan_frame_builder_impl>(eth_params);
}

std::unique_ptr<frame_builder> ocudu::ether::create_frame_builder(const vlan_frame_params& eth_params)
{
  return std::make_unique<frame_builder_impl>(eth_params);
}

std::unique_ptr<vlan_frame_decoder> ocudu::ether::create_vlan_frame_decoder(ocudulog::basic_logger& logger,
                                                                            unsigned                sector_id)
{
  return std::make_unique<vlan_frame_decoder_impl>(logger, sector_id);
}
