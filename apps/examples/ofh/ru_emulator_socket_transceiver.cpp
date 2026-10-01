// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ru_emulator_transceiver.h"
#include "ocudu/ofh/ethernet/ethernet_factories.h"
#include "ocudu/support/error_handling.h"

using namespace ocudu;
using namespace ether;

ru_emu_socket_receiver::ru_emu_socket_receiver(ocudulog::basic_logger&   logger_,
                                               ocudu::task_executor&     executor_,
                                               const transmitter_config& config)
{
  receiver = create_receiver({config.interface, config.is_promiscuous_mode_enabled, false}, executor_, logger_);
  ocudu_assert(receiver, "RU emulator failed to initialize Ethernet receiver");
}

ru_emu_socket_transmitter::ru_emu_socket_transmitter(ocudulog::basic_logger& logger_, const transmitter_config& config)
{
  transmitter = create_transmitter(config, logger_);
  ocudu_assert(transmitter, "RU emulator failed to initialize Ethernet transmitter");
}

void ru_emu_socket_receiver::start(ether::frame_notifier& notifier_)
{
  receiver->get_operation_controller().start(notifier_);
}

void ru_emu_socket_receiver::stop()
{
  receiver->get_operation_controller().stop();
}

void ru_emu_socket_transmitter::send(span<span<const uint8_t>> frames)
{
  transmitter->send(frames);
}

std::unique_ptr<ru_emulator_transceiver> ocudu::ru_emu_create_socket_transceiver(ocudulog::basic_logger&   logger,
                                                                                 task_executor&            executor,
                                                                                 const transmitter_config& config)
{
  return std::make_unique<ru_emulator_transceiver>(std::make_unique<ru_emu_socket_receiver>(logger, executor, config),
                                                   std::make_unique<ru_emu_socket_transmitter>(logger, config));
}
