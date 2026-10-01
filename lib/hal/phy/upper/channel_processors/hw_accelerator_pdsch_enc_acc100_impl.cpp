// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "hw_accelerator_pdsch_enc_acc100_impl.h"
#include "ocudu/hal/dpdk/bbdev/bbdev_op_pool_factory.h"
#include "ocudu/hal/dpdk/mbuf_pool_factory.h"
#include "fmt/format.h"

using namespace ocudu;
using namespace hal;

void hw_accelerator_pdsch_enc_acc100_impl::allocate_resources()
{
  int socket_id = bbdev_accelerator->get_socket_id();

  // Create bbdev op pools for the accelerated LDPC encoder operations.
  // op pools require unique names.
  std::string op_pool_name = fmt::format("enc_op_pool_{}_{}", device_id, id);
  op_pool                  = ::dpdk::create_bbdev_op_pool(
      op_pool_name.c_str(), RTE_BBDEV_OP_LDPC_ENC, MAX_NOF_SEGMENTS, socket_id, bbdev_accelerator->get_logger());

  // Bulk-reserve LDPC encoding operation resources.
  ::rte_bbdev_enc_op_alloc_bulk(op_pool->get_pool(), op, MAX_NOF_SEGMENTS);

  // Save the bbdev-related operation pointers into the configuration aray.
  for (unsigned i = 0; i != MAX_NOF_SEGMENTS; ++i) {
    encoding_op[i].op = op[i];
  }

  // Create new mbuf pools for both input and output data for the hardware-accelerated LDPC encoder.
  // Note that a predefined headroom length is added on top of the size required for the data in the mbufs. Also, the
  // set mbuf length represents the maximum supported TB size (also accounting for the TB CRC length) when using TB
  // mode.
  ::dpdk::mempool_config msg_mpool_cfg;
  unsigned               nof_mbuf = bbdev_accelerator->get_nof_mbuf();
  msg_mpool_cfg.mbuf_data_size    = bbdev_accelerator->get_msg_mbuf_size().value() + RTE_PKTMBUF_HEADROOM;
  msg_mpool_cfg.n_mbuf            = nof_mbuf;
  ::dpdk::mempool_config rm_mpool_cfg;
  rm_mpool_cfg.mbuf_data_size = bbdev_accelerator->get_rm_mbuf_size().value() + RTE_PKTMBUF_HEADROOM;
  rm_mpool_cfg.n_mbuf         = nof_mbuf;

  // mbuf pools require unique names.
  std::string mbuf_pool_name = fmt::format("enc_in_mbuf_pool_{}_{}", device_id, id);
  in_mbuf_pool =
      ::dpdk::create_mbuf_pool(mbuf_pool_name.c_str(), socket_id, msg_mpool_cfg, bbdev_accelerator->get_logger());

  // Bulk-reserve input-data mbuf resources.
  ::rte_pktmbuf_alloc_bulk(in_mbuf_pool->get_pool(), in_mbuf_vec, MAX_NOF_SEGMENTS);

  mbuf_pool_name = fmt::format("enc_out_mbuf_pool_{}_{}", device_id, id);
  out_mbuf_pool =
      ::dpdk::create_mbuf_pool(mbuf_pool_name.c_str(), socket_id, rm_mpool_cfg, bbdev_accelerator->get_logger());

  // Bulk-reserve output-data mbuf resources.
  ::rte_pktmbuf_alloc_bulk(out_mbuf_pool->get_pool(), out_mbuf_vec, MAX_NOF_SEGMENTS);
}

void hw_accelerator_pdsch_enc_acc100_impl::hw_reserve_queue()
{
  // Verify that no hardware-queue is reserved already.
  if (queue_id < 0) {
    int qid = -1;
    do {
      qid = bbdev_accelerator->reserve_queue(RTE_BBDEV_OP_LDPC_ENC);
    } while (qid < 0 && !dedicated_queue);
    queue_id = qid;

    // HAL logging.
    ocudulog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[acc100] encoder id={}: reserved queue={}.", id, queue_id);
  }
}

void hw_accelerator_pdsch_enc_acc100_impl::hw_free_queue()
{
  // Free the queue in case of non-dedicated use or upon object destruction.
  if (!dedicated_queue) {
    bbdev_accelerator->free_queue(RTE_BBDEV_OP_LDPC_ENC, queue_id);

    // HAL logging.
    ocudulog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[acc100] encoder id={}: freed queue={}.", id, queue_id);

    queue_id = -1;
  }
}

void hw_accelerator_pdsch_enc_acc100_impl::hw_config(const hw_pdsch_encoder_configuration& config, unsigned cb_index)
{
  // Save configuration.
  enc_config = config;

  // Set the bbdev-based accelerator configuration.
  encoding_op[cb_index].cb_mode   = enc_config.cb_mode;
  encoding_op[cb_index].do_unpack = enc_config.do_unpack;
  ::dpdk::set_ldpc_enc_bbdev_config(encoding_op[cb_index], config);

  // See if we need to attach the TB CRC (it is not supported by the accelerator).
  if (config.attach_tb_crc) {
    if (config.nof_tb_crc_bits == 16) {
      tb_crc.resize(2);
    }
    tb_crc = config.tb_crc;
  } else {
    tb_crc.resize(0);
  }

  // Reset the operation dropping control.
  if (cb_index == 0) {
    drop_op.reset();
  }
}

bool hw_accelerator_pdsch_enc_acc100_impl::hw_enqueue(span<const uint8_t> data, unsigned cb_index)
{
  bool enqueued = false;

  // Verify that the queue is not already full before trying to enqueue.
  if (nof_enqueued_op < ocudu::dpdk::MAX_NOF_OP_IN_QUEUE) {
    // Set the memory-pointers for the accelerator and provide the input data.
    ::rte_mbuf* in_mbuf_ptr  = in_mbuf_vec[cb_index];
    ::rte_mbuf* out_mbuf_ptr = out_mbuf_vec[cb_index];

    // Set up the input and output data of the accelerated operation.
    ::dpdk::set_ldpc_enc_bbdev_data(
        encoding_op[cb_index], *in_mbuf_ptr, *out_mbuf_ptr, data, tb_crc, bbdev_accelerator->get_logger());

    // HAL logging.
    ocudulog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[bbdev] encoder enqueueing: cb={}, LDPC [bg={}, mod={}, tb={}, tb_crc={}, seg={}, short_seg={}, "
                "rv={}, cwa={}, cwb={}, lifting={}, Ncb={}, filler={}, rm={}], acc [cb_mode={}, queue={}]",
                cb_index,
                (unsigned)enc_config.base_graph_index,
                (unsigned)enc_config.modulation,
                enc_config.nof_tb_bits,
                enc_config.nof_tb_crc_bits,
                enc_config.nof_segments,
                enc_config.nof_short_segments,
                enc_config.rv,
                enc_config.cw_length_a,
                enc_config.cw_length_b,
                enc_config.lifting_size,
                enc_config.Ncb,
                enc_config.nof_filler_bits,
                enc_config.rm_length,
                enc_config.cb_mode,
                queue_id);

    // Enqueue the LDPC encoding operation.
    enqueued = ::dpdk::enqueue_ldpc_enc_operation(encoding_op[cb_index],
                                                  1,
                                                  device_id,
                                                  static_cast<uint16_t>(queue_id),
                                                  bbdev_accelerator->get_logger()); // TBD: single operation enqueued.

    // Update the enqueued task counter.
    if (enqueued) {
      ++nof_enqueued_op;
      // Drop the operation due to an enqueueing error.
    } else {
      drop_op.set(cb_index);
      enqueued = true;
    }
  }

  return enqueued;
}

bool hw_accelerator_pdsch_enc_acc100_impl::hw_dequeue(span<uint8_t> data,
                                                      span<uint8_t> packed_data,
                                                      unsigned      segment_index)
{
  bool dropped  = drop_op.test(segment_index);
  bool dequeued = false;

  // Verify that the queue is not already emtpy and that the operation has not been dropped before trying to dequeue.
  if (nof_enqueued_op > 0 && !dropped) {
    // Dequeue processed operations from the hardware-accelerated LDPC encoder.
    dequeued = ::dpdk::dequeue_ldpc_enc_operation(encoding_op[segment_index],
                                                  1,
                                                  device_id,
                                                  static_cast<uint16_t>(queue_id),
                                                  bbdev_accelerator->get_logger()); // TBD: single operation dequeued.

    // Read the returned results (if any).
    if (dequeued) {
      // Read the accelerator output data.
      ::dpdk::read_ldpc_enc_bbdev_data(data, packed_data, encoding_op[segment_index]);

      // Update the enqueued task counter.
      --nof_enqueued_op;

      // HAL logging.
      ocudulog::basic_logger& logger = bbdev_accelerator->get_logger();
      logger.info(
          "[bbdev] encoder dequeued: cb={}, acc [cb_mode={}, queue={}]", segment_index, enc_config.cb_mode, queue_id);
    }
  } else if (dropped) {
    drop_op.reset(segment_index);
    dequeued = true;

    // HAL logging.
    ocudulog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info(
        "[bbdev] encoder skipped: cb={}, acc [cb_mode={}, queue={}]", segment_index, enc_config.cb_mode, queue_id);
  }

  return dequeued;
}

bool hw_accelerator_pdsch_enc_acc100_impl::is_hw_cb_mode_supported() const
{
  return cb_mode;
}

unsigned hw_accelerator_pdsch_enc_acc100_impl::get_hw_max_supported_buff_size() const
{
  return max_buffer_size;
}
