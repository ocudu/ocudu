// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pdsch/factories.h"
#include "ocudu/phy/upper/channel_processors/pusch/factories.h"
#include "ocudu/phy/upper/equalization/channel_equalizer_algorithm_type.h"
#include "ocudu/phy/upper/signal_processors/channel_estimator/port_channel_estimator_parameters.h"

namespace ocudu {
class task_executor;

/// \brief Creates a PDSCH processor factory.
/// \param[in] executor        Asynchronous executor.
/// \param[in] max_nof_threads Maximum number of concurrent threads.
/// \param[in] eal_arguments   String containing the EAL arguments to be used in case of hardware-acceleration.
/// \param[in] pxsch_type      Type of PxSCH implementation to be tested.
/// \return A PDSCH processor factory if it is successful.
std::shared_ptr<pdsch_processor_factory> create_sw_pdsch_processor_factory(task_executor&     executor,
                                                                           unsigned           max_nof_threads,
                                                                           const std::string& eal_arguments,
                                                                           const std::string& pxsch_type);

/// \brief Creates a PUSCH processor factory.
/// \param[in] executor                  Asynchornous executor.
/// \param[in] max_nof_threads           Maximum number of concurrent threads.
/// \param[in] nof_ldpc_iterations       Maximum number of LDPC decoder iterations.
/// \param[in] pxsch_type                Type of PxSCH implementation to be tested.
/// \param[in] dec_enable_early_stop     Set to true to enable LDPC decoder early stop.
/// \param[in] td_interpolation_strategy Channel estimator time-domain interpolation strategy.
/// \param[in] fd_smoothing_strategy     Channel estimator frequency-domain smoothing strategy.
/// \return A PUSCH processor factory if it is successful.
std::shared_ptr<pusch_processor_factory>
create_sw_pusch_processor_factory(task_executor&                                   executor,
                                  unsigned                                         max_nof_threads,
                                  unsigned                                         nof_ldpc_iterations,
                                  bool                                             dec_enable_early_stop,
                                  const std::string&                               pxsch_type,
                                  port_channel_estimator_td_interpolation_strategy td_interpolation_strategy,
                                  port_channel_estimator_fd_smoothing_strategy     fd_smoothing_strategy,
                                  channel_equalizer_algorithm_type                 equalizer_algorithm_type);

} // namespace ocudu
