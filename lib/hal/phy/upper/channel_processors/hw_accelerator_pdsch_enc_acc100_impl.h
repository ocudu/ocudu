// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Hardware accelerated PDSCH encoder functions declaration.

#pragma once

#include "hw_accelerator_pdsch_enc_impl.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/hal/dpdk/bbdev/bbdev.h"
#include "ocudu/hal/dpdk/bbdev/bbdev_acc.h"
#include "ocudu/hal/dpdk/bbdev/bbdev_op_pool.h"
#include "ocudu/hal/dpdk/bbdev/ldpc/bbdev_ldpc_encoder.h"
#include "ocudu/hal/dpdk/mbuf_pool.h"
#include "ocudu/ran/sch/sch_constants.h"
#include <array>
#include <memory>

namespace ocudu {
namespace hal {

/// Class representing the ACC100 implementation of PDSCH encoding.
class hw_accelerator_pdsch_enc_acc100_impl : public hw_accelerator_pdsch_enc_impl
{
  /// Hardware-specific implementation of the reserve queue function.
  void hw_reserve_queue() override;
  /// Hardware-specific implementation of the free queue function.
  void hw_free_queue() override;
  /// Hardware-specific implementation of the enqueue_operation function.
  bool hw_enqueue(span<const uint8_t> data, unsigned cb_index) override;
  /// Hardware-specific implementation of the dequeue_operation function.
  bool hw_dequeue(span<uint8_t> data, span<uint8_t> packed_data, unsigned segment_index) override;
  /// Hardware-specific configuration function.
  void hw_config(const hw_pdsch_encoder_configuration& config, unsigned cb_index) override;
  /// Hardware-specific CB mode quering function.
  bool is_hw_cb_mode_supported() const override;
  /// Hardware-specific maximum supported buffer size quering function.
  unsigned get_hw_max_supported_buff_size() const override;

  /// Allocate the required resources from the bbdev-based hardware-accelerator.
  void allocate_resources();

  /// \brief Members specific to bbdev-accelerated LDPC encoder functions.

  /// Unique ID of the current hardware-accelerated function.
  unsigned id;

  /// Pointer to a bbdev-based hardware-accelerator.
  std::shared_ptr<ocudu::dpdk::bbdev_acc> bbdev_accelerator;

  /// Private member to store the ID of the bbdev-based hardware-accelerator.
  unsigned device_id;

  /// Private member to store the ID of the queue used by the hardware-accelerated LDPC encoder function.
  int queue_id;

  /// Indicates the number of encoding operations in the queue.
  unsigned nof_enqueued_op = 0;

  /// Operation pool used by the hardware-accelerated LDPC encoder.
  std::unique_ptr<ocudu::dpdk::bbdev_op_pool> op_pool;

  /// Structure specifying each hardware-accelerated encode operation.
  ::rte_bbdev_enc_op* op[MAX_NOF_SEGMENTS];

  /// Input-data mbuf pool used by the hardware-accelerated LDPC encoder.
  std::unique_ptr<ocudu::dpdk::mbuf_pool> in_mbuf_pool;

  /// Structure of input-data mbuf pointers.
  ::rte_mbuf* in_mbuf_vec[MAX_NOF_SEGMENTS];

  /// Output-data mbuf pool used by the hardware-accelerated LDPC encoder.
  std::unique_ptr<ocudu::dpdk::mbuf_pool> out_mbuf_pool;

  /// Structure of output-data mbuf pointers.
  ::rte_mbuf* out_mbuf_vec[MAX_NOF_SEGMENTS];

  /// \brief Common members to all hardware-accelerated LDPC encoder functions.

  /// Private member to store the configuration of the current operation.
  hw_pdsch_encoder_configuration enc_config;

  /// Array of bbdev-based PDSCH encoder configuration structures.
  std::array<dpdk::bbdev_pdsch_encoder_configuration, MAX_NOF_SEGMENTS> encoding_op;

  /// Private member to store the operation mode.
  bool cb_mode = false;

  /// Private member to store the maximum supported buffer size (in bytes).
  unsigned max_buffer_size;

  /// Private member to store the TB CRC (only for TB mode operation).
  static_vector<uint8_t, 3> tb_crc;

  /// Array flagging those encoding operations that will be dropped due to enqueueing errors.
  bounded_bitset<MAX_NOF_SEGMENTS> drop_op;

  /// Indicates if the accelerated function uses a dedicated hardware queue or needs to reserve one for each operation.
  bool dedicated_queue;

public:
  /// Constructor taking care of obtaining a bbdev-based hardware-accelerator queue and allocating the required
  /// resources.
  hw_accelerator_pdsch_enc_acc100_impl(std::shared_ptr<ocudu::dpdk::bbdev_acc> bbdev_accelerator_,
                                       bool                                    cb_mode_,
                                       unsigned                                max_buffer_size_,
                                       bool                                    dedicated_queue_) :
    bbdev_accelerator(std::move(bbdev_accelerator_)),
    cb_mode(cb_mode_),
    max_buffer_size(max_buffer_size_),
    dedicated_queue(dedicated_queue_)
  {
    id        = bbdev_accelerator->reserve_encoder();
    device_id = bbdev_accelerator->get_device_id();
    // Reserve a hardware queue in case of dedicated use.
    queue_id = -1;
    if (dedicated_queue) {
      hw_reserve_queue();
      ocudu_assert(queue_id >= 0, "No free RTE_BBDEV_OP_LDPC_ENC queues available.");
    }
    allocate_resources();
    drop_op.resize(MAX_NOF_SEGMENTS);
    drop_op.reset();

    // HAL logging.
    ocudulog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[acc100] new encoder: id={}.", id);
  }

  /// Destructor taking care of freeing the utilized resources.
  ~hw_accelerator_pdsch_enc_acc100_impl()
  {
    // Free the reserved hardware queue in case of dedicated use.
    dedicated_queue = false;
    hw_free_queue();

    // Bulk-release of mbuf resources.
    ::rte_pktmbuf_free_bulk(in_mbuf_vec, MAX_NOF_SEGMENTS);
    ::rte_pktmbuf_free_bulk(out_mbuf_vec, MAX_NOF_SEGMENTS);

    // HAL logging.
    ocudulog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[acc100] destroyed encoder: id={}.", id);
  }
};

} // namespace hal
} // namespace ocudu
