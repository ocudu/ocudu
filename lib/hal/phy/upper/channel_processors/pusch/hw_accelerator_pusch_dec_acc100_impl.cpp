// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "hw_accelerator_pusch_dec_acc100_impl.h"
#include "ocudu/hal/dpdk/bbdev/bbdev_op_pool_factory.h"
#include "ocudu/hal/dpdk/mbuf_pool_factory.h"
#include "fmt/format.h"

using namespace ocudu;
using namespace hal;

void hw_accelerator_pusch_dec_acc100_impl::allocate_resources()
{
  int socket_id = bbdev_accelerator->get_socket_id();

  // Create bbdev op pools for the accelerated LDPC decoder operations.
  // op pools require unique names.
  std::string op_pool_name = fmt::format("dec_op_pool_{}_{}", device_id, id);
  op_pool                  = ::dpdk::create_bbdev_op_pool(
      op_pool_name.c_str(), RTE_BBDEV_OP_LDPC_DEC, MAX_NOF_SEGMENTS, socket_id, bbdev_accelerator->get_logger());

  // Bulk-reserve LDPC decoding operation resources.
  ::rte_bbdev_dec_op_alloc_bulk(op_pool->get_pool(), op, MAX_NOF_SEGMENTS);

  // Save the bbdev-related operation pointers into the configuration aray.
  for (unsigned i = 0; i != MAX_NOF_SEGMENTS; ++i) {
    decoding_op[i].op = op[i];
  }

  // Create new mbuf pools for both input and output data for the hardware-accelerated LDPC decoder.
  // Note that the buffers are sized taking into account that only CB mode is supported by the decoder.
  ::dpdk::mempool_config rm_mpool_cfg;
  unsigned               nof_mbuf = bbdev_accelerator->get_nof_mbuf();
  rm_mpool_cfg.mbuf_data_size     = bbdev_accelerator->get_rm_mbuf_size().value();
  rm_mpool_cfg.n_mbuf             = nof_mbuf;
  ::dpdk::mempool_config msg_mpool_cfg;
  msg_mpool_cfg.mbuf_data_size = bbdev_accelerator->get_msg_mbuf_size().value();
  msg_mpool_cfg.n_mbuf         = nof_mbuf;

  // mbuf pools require unique names.
  std::string mbuf_pool_name = fmt::format("dec_in_mbuf_pool_{}_{}", device_id, id);
  in_mbuf_pool =
      ::dpdk::create_mbuf_pool(mbuf_pool_name.c_str(), socket_id, rm_mpool_cfg, bbdev_accelerator->get_logger());

  // Bulk-reserve input-data mbuf resources.
  ::rte_pktmbuf_alloc_bulk(in_mbuf_pool->get_pool(), in_mbuf_vec, MAX_NOF_SEGMENTS);

  mbuf_pool_name = fmt::format("harq_in_mbuf_pool_{}_{}", device_id, id);
  harq_in_mbuf_pool =
      ::dpdk::create_mbuf_pool(mbuf_pool_name.c_str(), socket_id, rm_mpool_cfg, bbdev_accelerator->get_logger());

  // Bulk-reserve HARQ input-data mbuf resources.
  ::rte_pktmbuf_alloc_bulk(harq_in_mbuf_pool->get_pool(), harq_in_mbuf_vec, MAX_NOF_SEGMENTS);

  mbuf_pool_name = fmt::format("dec_out_mbuf_pool_{}_{}", device_id, id);
  out_mbuf_pool =
      ::dpdk::create_mbuf_pool(mbuf_pool_name.c_str(), socket_id, msg_mpool_cfg, bbdev_accelerator->get_logger());

  // Bulk-reserve output-data mbuf resources.
  ::rte_pktmbuf_alloc_bulk(out_mbuf_pool->get_pool(), out_mbuf_vec, MAX_NOF_SEGMENTS);

  mbuf_pool_name = fmt::format("harq_out_mbuf_pool_{}_{}", device_id, id);
  harq_out_mbuf_pool =
      ::dpdk::create_mbuf_pool(mbuf_pool_name.c_str(), socket_id, rm_mpool_cfg, bbdev_accelerator->get_logger());

  // Bulk-reserve HARQ output-data mbuf resources.
  ::rte_pktmbuf_alloc_bulk(harq_out_mbuf_pool->get_pool(), harq_out_mbuf_vec, MAX_NOF_SEGMENTS);
}

void hw_accelerator_pusch_dec_acc100_impl::hw_reserve_queue()
{
  // Verify that no hardware-queue is reserved already.
  if (queue_id < 0) {
    int qid = -1;
    do {
      qid = bbdev_accelerator->reserve_queue(RTE_BBDEV_OP_LDPC_DEC);
    } while (qid < 0 && !dedicated_queue);
    queue_id = qid;

    // HAL logging.
    ocudulog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[acc100] decoder id={}: reserved queue={}.", id, queue_id);
  }
}

void hw_accelerator_pusch_dec_acc100_impl::hw_free_queue()
{
  // Free the queue in case of non-dedicated use or upon object destruction.
  if (!dedicated_queue) {
    bbdev_accelerator->free_queue(RTE_BBDEV_OP_LDPC_DEC, queue_id);

    // HAL logging.
    ocudulog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[acc100] decoder id={}: freed queue={}.", id, queue_id);

    queue_id = -1;
  }
}

void hw_accelerator_pusch_dec_acc100_impl::hw_config(const hw_pusch_decoder_configuration& config, unsigned cb_index)
{
  // Save configuration.
  dec_config = config;

  // Reset the operation dropping control (in case of unexpected behaviour).
  if (drop_op.test(cb_index)) {
    drop_op.reset(cb_index);
  }

  // Get the HARQ buffer context entry of the CB or create a new one.
  harq_context_entries[cb_index] = harq_buffer_context->get(config.absolute_cb_id, config.new_data);

  // Set the bbdev-based accelerator configuration.
  decoding_op[cb_index].has_harq_memory  = has_harq_memory;
  decoding_op[cb_index].force_local_harq = force_local_harq;
  ::dpdk::set_ldpc_dec_bbdev_config(decoding_op[cb_index], config);
}

bool hw_accelerator_pusch_dec_acc100_impl::hw_enqueue(span<const int8_t> data,
                                                      span<const int8_t> soft_data,
                                                      unsigned           cb_index)
{
  bool     enqueued      = false;
  unsigned soft_data_len = harq_context_entries[cb_index]->soft_data_len;
  bool soft_data_len_ok  = dec_config.new_data || ((!dec_config.new_data || (dec_config.rv > 0)) && soft_data_len > 0);
  bool force_soft_data_len = dec_config.new_data && (dec_config.rv > 0) && (soft_data_len == 0);

  // For rv > 0, the ACC100 expects a non-zero input HARQ length.
  // Note: a zero input HARQ length will generate an error in 'acc100_fcw_ld_fill' in the 'rte_acc100_pmd' driver.
  if (force_soft_data_len) {
    soft_data_len = ::dpdk::compute_softdata_len(op[cb_index]->ldpc_dec);
  }

  // Verify that the queue is not already full before trying to enqueue.
  if (nof_enqueued_op < ::dpdk::MAX_NOF_OP_IN_QUEUE && soft_data_len_ok) {
    // Set the memory-pointers for the accelerator and provide the input data and related soft-combining inputs.
    ::rte_mbuf* in_mbuf_ptr       = in_mbuf_vec[cb_index];
    ::rte_mbuf* harq_in_mbuf_ptr  = harq_in_mbuf_vec[cb_index];
    ::rte_mbuf* out_mbuf_ptr      = out_mbuf_vec[cb_index];
    ::rte_mbuf* harq_out_mbuf_ptr = harq_out_mbuf_vec[cb_index];

    // Configure the decoder operation.
    ::dpdk::set_ldpc_dec_bbdev_data(decoding_op[cb_index],
                                    *in_mbuf_ptr,
                                    *harq_in_mbuf_ptr,
                                    *out_mbuf_ptr,
                                    *harq_out_mbuf_ptr,
                                    data,
                                    soft_data,
                                    dec_config.new_data,
                                    soft_data_len,
                                    cb_index,
                                    dec_config.absolute_cb_id,
                                    bbdev_accelerator->get_logger());

    // HAL logging.
    ocudulog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[bbdev] decoder enqueueing: cb={}, harq [new_data={}, sof_len={}{}, id={}], "
                "LDPC [bg={}, mod={}, seg={}, rv={}, cw={}, lifting={}, Ncb={}, filler={}, crc_len={}], acc [queue={}]",
                cb_index,
                dec_config.new_data,
                soft_data_len,
                force_soft_data_len ? " (forced)" : "",
                dec_config.absolute_cb_id,
                (unsigned)dec_config.base_graph_index,
                (unsigned)dec_config.modulation,
                dec_config.nof_segments,
                dec_config.rv,
                dec_config.cw_length,
                dec_config.lifting_size,
                dec_config.Ncb,
                dec_config.nof_filler_bits,
                dec_config.cb_crc_len,
                queue_id);

    // Enqueue the LDPC decoding operation.
    enqueued = ::dpdk::enqueue_ldpc_dec_operation(
        decoding_op[cb_index], 1, device_id, static_cast<uint16_t>(queue_id), bbdev_accelerator->get_logger());

    // Update the enqueued task counter.
    if (enqueued) {
      ++nof_enqueued_op;
      // Drop the operation due to an enqueueing error.
    } else {
      drop_op.set(cb_index);
      enqueued = true;
    }
  } else if (!soft_data_len_ok) {
    drop_op.set(cb_index);
    enqueued = true;
  }

  return enqueued;
}

bool hw_accelerator_pusch_dec_acc100_impl::hw_dequeue(span<uint8_t> data,
                                                      span<int8_t>  soft_data,
                                                      unsigned      segment_index)
{
  bool dropped  = drop_op.test(segment_index);
  bool dequeued = false;

  // Verify that the queue is not already emtpy and that the operation has not been dropped before trying to dequeue.
  if (nof_enqueued_op > 0 && !dropped) {
    // Dequeue processed operations from the hardware-accelerated LDPC decoder.
    dequeued = ::dpdk::dequeue_ldpc_dec_operation(decoding_op[segment_index],
                                                  1,
                                                  device_id,
                                                  static_cast<uint16_t>(queue_id),
                                                  bbdev_accelerator->get_logger()); // TBD: single operation dequeued.

    // Check if there are new results available from the hardware accelerator.
    if (dequeued) {
      // Read the accelerator output data and related soft-combining outputs, while updating the HARQ context.
      harq_context_entries[segment_index]->soft_data_len =
          ::dpdk::read_ldpc_dec_bbdev_data(data, soft_data, decoding_op[segment_index]);

      // Update the enqueued task counter.
      --nof_enqueued_op;
    }
  } else if (dropped) {
    dequeued = true;
  }

  return dequeued;
}

void hw_accelerator_pusch_dec_acc100_impl::hw_read_outputs(hw_pusch_decoder_outputs& out,
                                                           unsigned                  cb_index,
                                                           unsigned                  absolute_cb_id)
{
  // Save outputs.
  dec_out = out;

  ocudulog::basic_logger& logger = bbdev_accelerator->get_logger();

  // Operation dropped due to encoding errors.
  if (drop_op.test(cb_index)) {
    out.CRC_pass            = false;
    out.nof_ldpc_iterations = dec_config.max_nof_ldpc_iterations;
    drop_op.reset(cb_index);

    // HAL logging.
    logger.info("[bbdev] decoder skipped: cb={}, harq [new_data={}, sof_len={}, id={}], acc [queue={}]",
                cb_index,
                dec_config.new_data,
                harq_context_entries[cb_index]->soft_data_len,
                absolute_cb_id,
                queue_id);

    return;
  }

  // Check the CRC and number of utilized ldpc decoding iterations.
  int ldpc_dec_status     = op[cb_index]->status;
  out.CRC_pass            = !(ldpc_dec_status & (1 << RTE_BBDEV_CRC_ERROR));
  out.nof_ldpc_iterations = op[cb_index]->ldpc_dec.iter_count;

  // HAL logging.
  logger.info("[bbdev] decoder dequeued: cb={}, harq [new_data={}, sof_len={}, id={}], LDPC "
              "[crc_pass={}, "
              "ldpc_iter={}], acc [queue={}]",
              cb_index,
              dec_config.new_data,
              harq_context_entries[cb_index]->soft_data_len,
              absolute_cb_id,
              (unsigned)dec_config.cb_crc_type == 1 ? (int)out.CRC_pass : -1,
              out.nof_ldpc_iterations,
              queue_id);
}

void hw_accelerator_pusch_dec_acc100_impl::hw_free_harq_context(unsigned absolute_cb_id)
{
  harq_buffer_context->free(absolute_cb_id);
}

bool hw_accelerator_pusch_dec_acc100_impl::hw_is_harq_external() const
{
  return (has_harq_memory && !force_local_harq);
}
