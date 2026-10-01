// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/hal/dpdk/bbdev/bbdev_acc.h"
#include "ocudu/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc.h"
#include "ocudu/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc_factory.h"
#include <memory>

namespace ocudu {
namespace hal {

/// Configuration parameters of the bbdev-based PDSCH encoder hardware-accelerator factory.
struct bbdev_hwacc_pdsch_enc_factory_configuration {
  /// Accelerator type.
  std::string acc_type;
  /// Interfacing to a bbdev-based hardware-accelerator.
  std::shared_ptr<ocudu::dpdk::bbdev_acc> bbdev_accelerator;
  /// Defines if the PDSCH encoder operates in CB mode (true) or TB mode (false).
  bool cb_mode = false;
  /// Defines the maximum supported TB size in bytes (CB mode will be forced for larger TBs).
  /// Only used in TB mode.
  unsigned max_tb_size;
  /// Indicates if the accelerated function uses a dedicated hardware queue or needs to reserve one for each operation.
  bool dedicated_queue;
};

/// \brief Creates a bbdev-based PDSCH encoder hardware accelerator factory.
///
/// \param[in] cfg    Configuration parameters of the bbdev-based PDSCH encoder hardware accelerator.
/// \return A valid PDSCH encoder hardware accelerator factory on success, otherwise \c nullptr.
std::shared_ptr<hw_accelerator_pdsch_enc_factory>
create_bbdev_pdsch_enc_acc_factory(const bbdev_hwacc_pdsch_enc_factory_configuration& accelerator_config);

} // namespace hal
} // namespace ocudu
