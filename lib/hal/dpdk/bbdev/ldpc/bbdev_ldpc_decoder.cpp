// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/hal/dpdk/bbdev/ldpc/bbdev_ldpc_decoder.h"
#include "ocudu/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository.h"
#include <array>
#include <rte_bbdev.h>
#include <rte_memcpy.h>

using namespace ocudu;
using namespace dpdk;

/// Starting position of different redundancy versions (k0), as per TS38.212 Table 5.4.2.1-2.
static const std::array<unsigned, 4> shift_factor_bg1 = {0, 17, 33, 56};
static const std::array<unsigned, 4> shift_factor_bg2 = {0, 13, 25, 43};

static bool use_harq_memory(const bbdev_pusch_decoder_configuration& op_cfg)
{
  return (op_cfg.has_harq_memory && !op_cfg.force_local_harq);
}

void dpdk::set_ldpc_dec_bbdev_config(bbdev_pusch_decoder_configuration&                op_cfg,
                                     const ocudu::hal::hw_pusch_decoder_configuration& cfg)
{
  // Base graph.
  op_cfg.op->ldpc_dec.basegraph = static_cast<uint8_t>(cfg.base_graph_index);
  // Modulation.
  // Note that in terms of LDPC decoding, 'PI_2_BPSK' and 'BPSK' are the same.
  unsigned modulation     = static_cast<uint8_t>(cfg.modulation);
  op_cfg.op->ldpc_dec.q_m = modulation;
  if (modulation == 0) {
    op_cfg.op->ldpc_dec.q_m = 1;
  }
  // RV index.
  op_cfg.op->ldpc_dec.rv_index = static_cast<uint8_t>(cfg.rv);

  // CB mode is currently forced (TB mode is not yet supported by bbdev/ACC100).
  // CB size is in bytes.
  op_cfg.op->ldpc_dec.code_block_mode = 1;

  // Set the CB-specific parameters.
  op_cfg.op->ldpc_dec.cb_params.e = static_cast<uint32_t>(cfg.cw_length);

  // Lifting size.
  op_cfg.op->ldpc_dec.z_c = static_cast<uint16_t>(cfg.lifting_size);
  // Filler bits.
  op_cfg.op->ldpc_dec.n_filler = static_cast<uint16_t>(cfg.nof_filler_bits);
  // Length of the circular buffer in bits, as described in TS38.212 Section 5.4.2.1.
  op_cfg.op->ldpc_dec.n_cb = static_cast<uint16_t>(cfg.Ncb);
  // Maximum number of iterations to perform in decoding CB.
  op_cfg.op->ldpc_dec.iter_max = static_cast<uint8_t>(cfg.max_nof_ldpc_iterations);

  // Define the PDSCH decoder operation flags.
  // Note that certain operation aspects are assumed.
  uint32_t bitmask = 0;
  // Only CRC24B checking is supported by the hardware-accelerator.
  if (cfg.cb_crc_type == ocudu::hal::hw_dec_cb_crc_type::CRC24B) {
    // Codeblock CRC-24B checking is always implemented by the accelerator (set the bit).
    bitmask |= RTE_BBDEV_LDPC_CRC_TYPE_24B_CHECK;
    // Dropping of last CRC bits decoding output is always implemented in the accelerator (set the bit).
    bitmask |= RTE_BBDEV_LDPC_CRC_TYPE_24B_DROP;
  }
  // Bit-level de-interleaver is always implemented by the accelerator (clear the bit).
  bitmask &= ~RTE_BBDEV_LDPC_DEINTERLEAVER_BYPASS;
  // Input HARQ combining data is only needed in case of a retransmission.
  if (!cfg.new_data || (cfg.rv > 0)) {
    // HARQ combined input stream is enabled in the accelerator (set the bit).
    bitmask |= RTE_BBDEV_LDPC_HQ_COMBINE_IN_ENABLE;

    // Check if the soft-buffer is implemented in the accelerator or in the host.
    if (use_harq_memory(op_cfg)) {
      // HARQ input is provided from the accelerator's internal memory (set the bit).
      bitmask |= RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_IN_ENABLE;
    }
  }
  // HARQ combined output stream is always enabled in the accelerator (set the bit).
  bitmask |= RTE_BBDEV_LDPC_HQ_COMBINE_OUT_ENABLE;
  // Check if the soft-buffer is implemented in the accelerator or in the host.
  if (use_harq_memory(op_cfg)) {
    // HARQ output is provided to the accelerator's internal memory (set the bit).
    bitmask |= RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_OUT_ENABLE;
  }
  // LDPC decoder is always implemented by the accelerator (clear the bit).
  bitmask &= ~RTE_BBDEV_LDPC_DECODE_BYPASS;
  // If early stop is requested, enable it in the accerlerator (set the bit, otherwise clear it).
  if (cfg.use_early_stop) {
    bitmask |= RTE_BBDEV_LDPC_ITERATION_STOP_ENABLE;
  } else {
    bitmask &= ~RTE_BBDEV_LDPC_ITERATION_STOP_ENABLE;
  }
  op_cfg.op->ldpc_dec.op_flags = bitmask;
}

bool dpdk::set_ldpc_dec_bbdev_data(bbdev_pusch_decoder_configuration& op_cfg,
                                   ::rte_mbuf&                        in_mbuf_ptr,
                                   ::rte_mbuf&                        harq_in_mbuf_ptr,
                                   ::rte_mbuf&                        out_mbuf_ptr,
                                   ::rte_mbuf&                        harq_out_mbuf_ptr,
                                   span<const int8_t>                 data,
                                   span<const int8_t>                 soft_data,
                                   bool                               new_data,
                                   uint32_t                           soft_data_len,
                                   unsigned                           cb_index,
                                   unsigned                           absolute_cb_id,
                                   ocudulog::basic_logger&            logger)
{
  // Get an mbuf for the input data.
  ::rte_mbuf* m_head_in = &in_mbuf_ptr;
  if (m_head_in == nullptr) {
    logger.error("[bbdev] Not enough mbufs in the input ldpc decoder mbuf pool.");
    return false;
  }
  // Reset the input mbuf.
  m_head_in->data_len = 0;

  // Set the memory pointers for the input data, accounting for the offset of the current segment.
  op_cfg.op->ldpc_dec.input.data = m_head_in;
  // Note that a dedicated mbuf is used per CB.
  op_cfg.op->ldpc_dec.input.offset = 0;
  op_cfg.op->ldpc_dec.input.length = 0;

  // Get the mbuf data-start memory pointers.
  uint16_t cw_len     = data.size();
  char*    input_data = ::rte_pktmbuf_append(m_head_in, cw_len);
  if (input_data == nullptr) {
    logger.error("[bbdev] Couldn't append {} bytes to the ldpc decoder input mbuf.", cw_len);
    return false;
  }

  // Copy the CB data.
  // Note that a single copy operation is performed per CB. The copy assumes that the input data size is below the
  // requested mbuf size (by default set to the maximum supported value RTE_BBDEV_LDPC_E_MAX_MBUF).
  ::rte_memcpy(input_data, data.begin(), cw_len);
  op_cfg.op->ldpc_dec.input.length += cw_len;

  // Harq offset for the current CB (based on its unique absolute ID).
  unsigned harq_offset = ocudu::hal::HARQ_INCR.value() * absolute_cb_id;

  // Input HARQ combining data is only needed in case of a retransmission.
  if (!new_data || (op_cfg.op->ldpc_dec.rv_index > 0)) {
    // Copy of HARQ input data is only needed if the soft-buffer is implemented in the host.
    if (op_cfg.force_local_harq || !(op_cfg.has_harq_memory)) {
      // Get an mbuf for the input soft-data.
      ::rte_mbuf* m_head_soft_in = &harq_in_mbuf_ptr;
      if (m_head_soft_in == nullptr) {
        logger.error("[bbdev] Not enough mbufs in the harq input ldpc decoder "
                     "mbuf pool.");
        return false;
      }
      // Reset the input soft-data mbuf.
      m_head_soft_in->data_len = 0;

      // Set the memory pointers for the input soft-data.
      // When using the host memory the offset is provided by the input span pointers.
      uint32_t harq_output_len                       = op_cfg.op->ldpc_dec.harq_combined_output.length;
      op_cfg.op->ldpc_dec.harq_combined_input.data   = m_head_soft_in;
      op_cfg.op->ldpc_dec.harq_combined_input.offset = 0;
      op_cfg.op->ldpc_dec.harq_combined_input.length = 0;

      // Get the mbuf soft-data-start memory pointers.
      char* soft_input_data = ::rte_pktmbuf_append(m_head_soft_in, harq_output_len);
      if (soft_input_data == nullptr) {
        logger.error("[bbdev] Couldn't append {} bytes to the ldpc decoder "
                     "harq input mbuf.",
                     harq_output_len);
        return false;
      }

      // Copy the input soft-data located before the filler bits in the soft-buffer.
      // Note that the copy assumes that the input soft-data size is below the requested mbuf size (by default set to
      // the maximum supported value RTE_BBDEV_LDPC_E_MAX_MBUF).
      ::rte_memcpy(soft_input_data, soft_data.begin(), harq_output_len);
      op_cfg.op->ldpc_dec.harq_combined_input.length += harq_output_len;

      // Preload the accelerator HARQ memory and adjust the offset (only when forcing use of the host memory with an
      // accelerator embedding a dedicated HARQ-buffer). Note this will introduce non-negligible latency.
      if (op_cfg.has_harq_memory) {
        ::rte_bbdev_op_data saved_hc_in  = op_cfg.op->ldpc_dec.harq_combined_input;
        ::rte_bbdev_op_data saved_hc_out = op_cfg.op->ldpc_dec.harq_combined_output;
        uint32_t            saved_flags  = op_cfg.op->ldpc_dec.op_flags;
        op_cfg.op->ldpc_dec.op_flags =
            RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_LOOPBACK + RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_OUT_ENABLE;
        ::rte_bbdev_enqueue_ldpc_dec_ops(0, 0, &op_cfg.op, 1);
        int ret = 0;
        while (ret == 0) {
          ret = ::rte_bbdev_dequeue_ldpc_dec_ops(0, 0, &op_cfg.op, 1);
        }
        op_cfg.op->ldpc_dec.op_flags             = saved_flags;
        op_cfg.op->ldpc_dec.harq_combined_input  = saved_hc_in;
        op_cfg.op->ldpc_dec.harq_combined_output = saved_hc_out;
      }
      // When using the accelerator's HARQ memory only pointers need updating.
    } else {
      // Update the HARQ input memory offset and length.
      // When using the accelerator memory fixed CB offsets are employed.
      op_cfg.op->ldpc_dec.harq_combined_input.offset = harq_offset;
      op_cfg.op->ldpc_dec.harq_combined_input.length = soft_data_len;
    }
  }

  // Get an mbuf for the output data.
  ::rte_mbuf* m_head_out = &out_mbuf_ptr;
  if (m_head_out == nullptr) {
    logger.error("[bbdev] Not enough mbufs in the output ldpc decoder mbuf pool.");
    return false;
  }
  // Reset the output mbuf.
  m_head_out->data_len = 0;

  // Set the memory pointers for the output data, accounting for the offset of the current segment.
  op_cfg.op->ldpc_dec.hard_output.data = m_head_out;
  // Note that a dedicated mbuf is used per segment.
  op_cfg.op->ldpc_dec.hard_output.offset = 0;
  op_cfg.op->ldpc_dec.hard_output.length = 0;

  // Update the HARQ output memory offset and length.
  // Host memory (offset is provided by the input span pointers).
  if (op_cfg.force_local_harq || !(op_cfg.has_harq_memory)) {
    op_cfg.op->ldpc_dec.harq_combined_output.offset = 0;
    // Accelerator memory (fixed CB offsets are used).
  } else {
    op_cfg.op->ldpc_dec.harq_combined_output.offset = harq_offset;
  }
  op_cfg.op->ldpc_dec.harq_combined_output.length = 0;

  // Reading of HARQ output data is only needed if the soft-buffer is implemented in the host.
  if (op_cfg.force_local_harq || !(op_cfg.has_harq_memory)) {
    // Get an mbuf for the output soft-data.
    ::rte_mbuf* m_head_soft_out = &harq_out_mbuf_ptr;
    if (m_head_soft_out == nullptr) {
      logger.error("[bbdev] Not enough mbufs in the harq output ldpc decoder mbuf pool.");
      return false;
    }
    // Reset the HARQ output mbuf.
    m_head_soft_out->data_len = 0;

    // Set the memory pointers for the output data.
    op_cfg.op->ldpc_dec.harq_combined_output.data = m_head_soft_out;
  }

  return true;
}

bool dpdk::enqueue_ldpc_dec_operation(bbdev_pusch_decoder_configuration& op_cfg,
                                      uint16_t                           num_dec_ops,
                                      uint16_t                           bbdev_id,
                                      uint16_t                           dec_queue_id,
                                      ocudulog::basic_logger&            logger)
{
  // Enqueue new operations on the hardware-accelerator LDPC decoder.
  uint16_t num_enq_dec_ops = ::rte_bbdev_enqueue_ldpc_dec_ops(bbdev_id, dec_queue_id, &op_cfg.op, num_dec_ops);
  if (num_enq_dec_ops <= 0) {
    logger.error("[bbdev] Couldn't enqueue new operations in the ldpc decoder. "
                 "The operation will be dropped: acc [queue={}].",
                 dec_queue_id);
    return false;
  }

  return true;
}

bool dpdk::dequeue_ldpc_dec_operation(bbdev_pusch_decoder_configuration& op_cfg,
                                      uint16_t                           num_dec_ops,
                                      uint16_t                           bbdev_id,
                                      uint16_t                           dec_queue_id,
                                      ocudulog::basic_logger&            logger)
{
  // Dequeue processed operations from the hardware-accelerator LDPC decoder.
  uint16_t num_deq_dec_ops = ::rte_bbdev_dequeue_ldpc_dec_ops(bbdev_id, dec_queue_id, &op_cfg.op, num_dec_ops);
  if (num_deq_dec_ops <= 0) {
    // logger.error("[bbdev] Couldn't dequeue new operations from the ldpc decoder.");
    return false;
  }

  return true;
}

uint32_t
dpdk::read_ldpc_dec_bbdev_data(span<uint8_t> data, span<int8_t> soft_data, bbdev_pusch_decoder_configuration& op_cfg)
{
  uint32_t new_soft_data_len = 0;

  ::rte_mbuf* m_head_out = op_cfg.op->ldpc_dec.hard_output.data;
  uint16_t    dec_len    = rte_pktmbuf_data_len(m_head_out);
  // Read the CB data.
  // Note that a single read operation is performed per CB. The read assumes that the output data size is below the
  // requested mbuf size (by default set to the maximum supported value RTE_BBDEV_LDPC_E_MAX_MBUF).
  uint16_t offset   = 0; // Note that a dedicated mbuf is used per segment.
  uint8_t* dec_data = rte_pktmbuf_mtod_offset(m_head_out, uint8_t*, offset);

  // Recover the decoded data.
  ::rte_memcpy(data.data(), dec_data, dec_len);

  // Update the harq length for this CB.
  new_soft_data_len = op_cfg.op->ldpc_dec.harq_combined_output.length;

  // Only if the soft-buffer is implemented in the host then the output soft-data needs to be read from the accelerator
  // HARQ memory.
  if (op_cfg.force_local_harq || !(op_cfg.has_harq_memory)) {
    // Push back the data from the accelerator HARQ memory (only when forcing use of the host memory with an accelerator
    // embedding a dedicated HARQ-buffer). Note this will introduce non-negligible latency.
    if (op_cfg.has_harq_memory) {
      int      saved_status     = op_cfg.op->status;
      uint8_t  saved_iter_count = op_cfg.op->ldpc_dec.iter_count;
      uint32_t saved_flags      = op_cfg.op->ldpc_dec.op_flags;
      op_cfg.op->ldpc_dec.op_flags =
          RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_LOOPBACK + RTE_BBDEV_LDPC_INTERNAL_HARQ_MEMORY_IN_ENABLE;
      op_cfg.op->ldpc_dec.harq_combined_input.length = new_soft_data_len;
      ::rte_bbdev_enqueue_ldpc_dec_ops(0, 0, &op_cfg.op, 1);
      int ret = 0;
      while (ret == 0) {
        ret = ::rte_bbdev_dequeue_ldpc_dec_ops(0, 0, &op_cfg.op, 1);
      }
      op_cfg.op->ldpc_dec.op_flags   = saved_flags;
      op_cfg.op->status              = saved_status;
      op_cfg.op->ldpc_dec.iter_count = saved_iter_count;
    }

    // Retrieve the pointer to the mbuf used for the output soft-data.
    ::rte_mbuf* m_head_soft_out = op_cfg.op->ldpc_dec.harq_combined_output.data;
    // Note that the read assumes that the decoded soft-data size is below the requested mbuf size (by default set to
    // the maximum supported value RTE_BBDEV_LDPC_E_MAX_MBUF).
    uint16_t soft_offset   = 0;
    int8_t*  dec_soft_data = rte_pktmbuf_mtod_offset(m_head_soft_out, int8_t*, soft_offset);

    // Copy the output soft-data in the soft-buffer.
    ::rte_memcpy(soft_data.data(), dec_soft_data, new_soft_data_len);
  }

  return new_soft_data_len;
}

uint16_t dpdk::compute_softdata_len(const ::rte_bbdev_op_ldpc_dec& dec_cfg)
{
  uint16_t k0            = 0;
  uint16_t n             = 0;
  uint16_t parity_offset = 0;
  uint16_t full_len      = 0;
  uint16_t rm_out_len    = 0;
  uint8_t  max_rv_ix     = dec_cfg.rv_index == 1 ? 3 : dec_cfg.rv_index;

  // Compute k0 (TS38.212 Table 5.4.2.1-2).
  n = (dec_cfg.basegraph == 1 ? 66 : 50) * dec_cfg.z_c;
  if (dec_cfg.n_cb == n) {
    k0 = (dec_cfg.basegraph == 1 ? shift_factor_bg1[max_rv_ix] : shift_factor_bg2[max_rv_ix]) * dec_cfg.z_c;
  } else {
    k0 = (((dec_cfg.basegraph == 1 ? shift_factor_bg1[max_rv_ix] : shift_factor_bg2[max_rv_ix]) * dec_cfg.n_cb) / n) *
         dec_cfg.z_c;
  }

  // Compute the rate-matching output and HARQ output lenghts.
  parity_offset = (dec_cfg.basegraph == 1 ? 20 : 8) * dec_cfg.z_c - dec_cfg.n_filler;
  full_len      = k0 + dec_cfg.cb_params.e + ((k0 > parity_offset) ? 0 : dec_cfg.n_filler);
  rm_out_len    = RTE_MIN(full_len, dec_cfg.n_cb);

  return ((rm_out_len + dec_cfg.z_c - 1) / dec_cfg.z_c) * dec_cfg.z_c;
}
