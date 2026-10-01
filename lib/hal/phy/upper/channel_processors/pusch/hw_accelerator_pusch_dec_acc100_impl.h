// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Hardware accelerated PUSCH decoder functions declaration.

#pragma once

#include "hw_accelerator_pusch_dec_impl.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/hal/dpdk/bbdev/bbdev.h"
#include "ocudu/hal/dpdk/bbdev/bbdev_acc.h"
#include "ocudu/hal/dpdk/bbdev/bbdev_op_pool.h"
#include "ocudu/hal/dpdk/bbdev/ldpc/bbdev_ldpc_decoder.h"
#include "ocudu/hal/dpdk/mbuf_pool.h"
#include "ocudu/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository_factory.h"
#include "ocudu/ran/sch/sch_constants.h"
#include <array>
#include <memory>
#include <vector>

namespace ocudu {
namespace hal {

/// Class representing the ACC100 implementation of PUSCH decoding.
class hw_accelerator_pusch_dec_acc100_impl : public hw_accelerator_pusch_dec_impl
{
  /// Hardware-specific implementation of the reserve queue function.
  void hw_reserve_queue() override;
  /// Hardware-specific implementation of the free queue function.
  void hw_free_queue() override;
  /// Hardware-specific implementation of the enqueue_operation function.
  bool hw_enqueue(span<const int8_t> data, span<const int8_t> soft_data, unsigned cb_index) override;
  /// Hardware-specific implementation of the dequeue_operation function.
  bool hw_dequeue(span<uint8_t> data, span<int8_t> soft_data, unsigned segment_index) override;
  /// Hardware-specific configuration function.
  void hw_config(const hw_pusch_decoder_configuration& config, unsigned cb_index) override;
  /// Hardware-specific operation status outputs recovery function.
  void hw_read_outputs(hw_pusch_decoder_outputs& out, unsigned cb_index, unsigned absolute_cb_id) override;
  /// Hardware-specific HARQ buffer context freeing function.
  void hw_free_harq_context(unsigned absolute_cb_id) override;
  /// Hardware-specific external HARQ buffer checking function.
  bool hw_is_harq_external() const override;

  /// Allocate the required resources from the bbdev-based hardware-accelerator.
  void allocate_resources();

  /// \brief Members specific to bbdev-accelerated LDPC decoder functions.

  /// Unique ID of the current hardware-accelerated function.
  unsigned id;

  /// Pointer to a bbdev-based hardware-accelerator.
  std::shared_ptr<ocudu::dpdk::bbdev_acc> bbdev_accelerator;

  /// Private member to store the ID of the bbdev-based hardware-accelerator.
  unsigned device_id;

  /// Private member to store the ID of the queue used by the hardware-accelerated LDPC decoder function.
  int queue_id;

  /// Indicates the number of decoding operations in the queue.
  unsigned nof_enqueued_op = 0;

  /// Operation pool used by the hardware-accelerated LDPC decoder.
  std::unique_ptr<ocudu::dpdk::bbdev_op_pool> op_pool;

  /// Structure specifying each hardware-accelerated decode operation.
  ::rte_bbdev_dec_op* op[MAX_NOF_SEGMENTS];

  /// Input-data mbuf pool used by the hardware-accelerated LDPC decoder.
  std::unique_ptr<ocudu::dpdk::mbuf_pool> in_mbuf_pool;

  /// Structure of input-data mbuf pointers.
  ::rte_mbuf* in_mbuf_vec[MAX_NOF_SEGMENTS];

  /// HARQ input-data mbuf pool used by the hardware-accelerated LDPC decoder.
  std::unique_ptr<ocudu::dpdk::mbuf_pool> harq_in_mbuf_pool;

  /// Structure of HARQ input-data mbuf pointers.
  ::rte_mbuf* harq_in_mbuf_vec[MAX_NOF_SEGMENTS];

  /// Output-data mbuf pool used by the hardware-accelerated LDPC decoder.
  std::unique_ptr<ocudu::dpdk::mbuf_pool> out_mbuf_pool;

  /// Structure of output-data mbuf pointers.
  ::rte_mbuf* out_mbuf_vec[MAX_NOF_SEGMENTS];

  /// HARQ output-data mbuf pool used by the hardware-accelerated LDPC decoder.
  std::unique_ptr<ocudu::dpdk::mbuf_pool> harq_out_mbuf_pool;

  /// Structure of HARQ output-data mbuf pointers.
  ::rte_mbuf* harq_out_mbuf_vec[MAX_NOF_SEGMENTS];

  /// \brief Common members to all hardware-accelerated LDPC decoder functions.

  /// Private member to store the configuration of the current operation.
  hw_pusch_decoder_configuration dec_config;

  /// Array of bbdev-based PUSCH decoder configuration structures.
  std::array<dpdk::bbdev_pusch_decoder_configuration, MAX_NOF_SEGMENTS> decoding_op;

  /// Context of the external HARQ buffer.
  std::shared_ptr<ext_harq_buffer_context_repository> harq_buffer_context;

  /// Array flagging those decoding operations that will be dropped due to enqueueing errors.
  bounded_bitset<MAX_NOF_SEGMENTS> drop_op;

  /// Private member to store the outputs of the current operation.
  hw_pusch_decoder_outputs dec_out;

  /// HARQ context repository entry for the current decoding operation.
  std::vector<ext_harq_buffer_context_entry*> harq_context_entries;

  /// Indicates if the hardware-accelerator embeds a dedicated HARQ memory.
  bool has_harq_memory;

  /// Private member to flag forced use of the host memory to implement the soft-buffer.
  bool force_local_harq;

  /// Indicates if the accelerated function uses a dedicated hardware queue or needs to reserve one for each operation.
  bool dedicated_queue;

public:
  /// Constructor taking care of obtaining a bbdev-based hardware-accelerator queue and allocating the required
  /// resources.
  hw_accelerator_pusch_dec_acc100_impl(std::shared_ptr<ocudu::dpdk::bbdev_acc>             bbdev_accelerator_,
                                       std::shared_ptr<ext_harq_buffer_context_repository> harq_buffer_context_,
                                       bool                                                force_local_harq_,
                                       bool                                                dedicated_queue_) :
    bbdev_accelerator(std::move(bbdev_accelerator_)),
    harq_buffer_context(std::move(harq_buffer_context_)),
    force_local_harq(force_local_harq_),
    dedicated_queue(dedicated_queue_)
  {
    id        = bbdev_accelerator->reserve_decoder();
    device_id = bbdev_accelerator->get_device_id();
    // Reserve a hardware queue in case of dedicated use.
    queue_id = -1;
    if (dedicated_queue) {
      hw_reserve_queue();
      ocudu_assert(queue_id >= 0, "No free RTE_BBDEV_OP_LDPC_DEC queues available.");
    }
    allocate_resources();
    drop_op.resize(MAX_NOF_SEGMENTS);
    drop_op.reset();
    harq_context_entries.reserve(MAX_NOF_SEGMENTS);

    // Check if the accelerator embeds a dedicated HARQ memory.
    has_harq_memory = true;
    if (bbdev_accelerator->get_harq_buff_size_bytes() == 0) {
      has_harq_memory = false;
    }

    // HAL logging.
    ocudulog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[acc100] new decoder: id={}.", id);
  }

  /// Destructor taking care of freeing the utilized resources.
  ~hw_accelerator_pusch_dec_acc100_impl()
  {
    // Free the reserved hardware queue in case of dedicated use.
    dedicated_queue = false;
    hw_free_queue();

    // Bulk-release of mbuf resources.
    ::rte_pktmbuf_free_bulk(in_mbuf_vec, MAX_NOF_SEGMENTS);
    ::rte_pktmbuf_free_bulk(harq_in_mbuf_vec, MAX_NOF_SEGMENTS);
    ::rte_pktmbuf_free_bulk(out_mbuf_vec, MAX_NOF_SEGMENTS);
    ::rte_pktmbuf_free_bulk(harq_out_mbuf_vec, MAX_NOF_SEGMENTS);

    // HAL logging.
    ocudulog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[acc100] destroyed decoder: id={}.", id);
  }
};

} // namespace hal
} // namespace ocudu
