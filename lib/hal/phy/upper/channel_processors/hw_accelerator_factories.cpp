// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/hal/phy/upper/channel_processors/hw_accelerator_factories.h"
#include "hw_accelerator_pdsch_enc_acc100_impl.h"
#include "hw_accelerator_pdsch_enc_impl.h"

using namespace ocudu;
using namespace hal;

namespace {

class hw_accelerator_pdsch_enc_factory_bbdev : public hw_accelerator_pdsch_enc_factory
{
  /// Accelerator type.
  std::string acc_type;
  /// Interfacing to a bbdev-based hardware-accelerator.
  std::shared_ptr<ocudu::dpdk::bbdev_acc> bbdev_accelerator;
  /// Operation mode of the PDSCH encoder (CB = true, TB = false [default]).
  bool cb_mode = false;
  /// Maximum supported TB size in bytes (used to size the mbufs).
  unsigned max_tb_size;
  /// Indicates if the accelerated function uses a dedicated hardware queue or needs to reserve one for each operation.
  bool dedicated_queue = true;

public:
  explicit hw_accelerator_pdsch_enc_factory_bbdev(
      const bbdev_hwacc_pdsch_enc_factory_configuration& accelerator_config) :
    acc_type(accelerator_config.acc_type),
    bbdev_accelerator(std::move(accelerator_config.bbdev_accelerator)),
    cb_mode(accelerator_config.cb_mode),
    max_tb_size(accelerator_config.max_tb_size),
    dedicated_queue(accelerator_config.dedicated_queue)
  {
  }

  std::unique_ptr<hw_accelerator_pdsch_enc> create() override
  {
    if (acc_type == "acc100" || acc_type == "acc200" || acc_type == "vrb1") {
      return std::make_unique<hw_accelerator_pdsch_enc_acc100_impl>(
          bbdev_accelerator, cb_mode, max_tb_size, dedicated_queue);
    }
    // Handle other accelerator types here.
    report_error("[HAL] unsupported hardware-accelerator type {}, supported values are 'acc100', 'acc200' and 'vrb1'",
                 acc_type);
    return {};
  }
};

} // namespace

std::shared_ptr<hw_accelerator_pdsch_enc_factory>
ocudu::hal::create_bbdev_pdsch_enc_acc_factory(const bbdev_hwacc_pdsch_enc_factory_configuration& accelerator_config)
{
  return std::make_shared<hw_accelerator_pdsch_enc_factory_bbdev>(accelerator_config);
}
