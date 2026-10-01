// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/hal/dpdk/bbdev/bbdev_acc.h"
#include "ocudu/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository.h"
#include "ocudu/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec.h"
#include "ocudu/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec_factory.h"
#include <memory>

namespace ocudu {
namespace hal {

/// Configuration parameters of the bbdev-based PUSCH decoder hardware-accelerator factory.
struct bbdev_hwacc_pusch_dec_factory_configuration {
  /// Accelerator type.
  std::string acc_type;
  /// Interfacing to a bbdev-based hardware-accelerator.
  std::shared_ptr<ocudu::dpdk::bbdev_acc> bbdev_accelerator;
  /// Interfacing to an external HARQ buffer context repository.
  std::shared_ptr<ext_harq_buffer_context_repository> harq_buffer_context;
  /// Forces using the host memory to implement the soft-buffer.
  bool force_local_harq = false;
  /// Indicates if the accelerated function uses a dedicated hardware queue or needs to reserve one for each operation.
  bool dedicated_queue = true;
};

/// \brief Creates a bbdev-based PUSCH decoder hardware accelerator factory.
///
/// \param[in] cfg    Configuration parameters of the bbdev-based PUSCH decoder hardware accelerator.
/// \return A valid PUSCH decoder hardware accelerator factory on success, otherwise \c nullptr.
std::shared_ptr<hw_accelerator_pusch_dec_factory>
create_bbdev_pusch_dec_acc_factory(const bbdev_hwacc_pusch_dec_factory_configuration& accelerator_config);

} // namespace hal
} // namespace ocudu
