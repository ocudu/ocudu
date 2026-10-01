// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Definition of the specific functions used by the bbdeb-based hardware-accelerated LDPC decoder
/// implementations.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <rte_bbdev_op.h>

namespace ocudu {
namespace dpdk {

/// Configuration parameters of a bbdev-based PUSCH decoder operation.
struct bbdev_pusch_decoder_configuration {
  /// Pointer to a bbdev decoder operation configuration structure.
  ::rte_bbdev_dec_op* op;
  /// Indicates if the accelerator embeds a dedicated HARQ memory.
  bool has_harq_memory;
  /// Flag forced use of the host memory to implement the soft-buffer.
  bool force_local_harq;
};

/// \brief Sets the contents of the bbdev configuration structure for the hardware-accelerated LPDC decoder operations.
///
/// \param[out] op_cfg bbdev-based PUSCH decoder operation configuration structure.
/// \param[in]  cfg PUSCH decoder configuration parameter-structure.
void set_ldpc_dec_bbdev_config(bbdev_pusch_decoder_configuration&                op_cfg,
                               const ocudu::hal::hw_pusch_decoder_configuration& cfg);

/// \brief Sets the contents of the bbdev input mbuffer structures for the hardware-accelerated LPDC decoder operations.
///
/// \param[out] op_cfg            bbdev-based PUSCH decoder operation configuration structure.
/// \param[in]  in_mbuf_ptr       Input data mbuf pointer for the hardware-accelerated LDPC decoder operations.
/// \param[in]  harq_in_mbuf_ptr  Input soft-data mbuf pointer for the hardware-accelerated LDPC decoder operations.
/// \param[in]  out_mbuf_ptr      Output data mbuf pointer for the hardware-accelerated LDPC decoder operations.
/// \param[in]  harq_out_mbuf_ptr Output soft-data mbuf pointer for the hardware-accelerated LDPC decoder operations.
/// \param[in]  data              The input data to the hardware-accelerated LDPC decoder operations.
/// \param[in]  soft_data         The input soft-data to the hardware-accelerated LDPC decoder operations.
/// \param[in]  new_data          Indicates if the data is new or a retransmission.
/// \param[in]  soft_data_len     Length of the soft-combining input data.
/// \param[in]  cb_index          Index of the CB (within the TB).
/// \param[in]  absolute_cb_id    Unique ID of associated to the CB in the soft-buffer and HARQ ACK context entry.
/// \param[in]  logger            OCUDU logger.
bool set_ldpc_dec_bbdev_data(bbdev_pusch_decoder_configuration& op_cfg,
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
                             ocudulog::basic_logger&            logger);

/// \brief Enqueues new operations in the hardware-accelerated LDPC decoder.
///
/// \param[out] op_cfg       bbdev-based PUSCH decoder operation configuration structure.
/// \param[in]  num_dec_ops  Number of decoder operations to be enqueued.
/// \param[in]  id           bbdev ID of the hardware-accelerator.
/// \param[in]  dec_queue_id bbdev ID of the queue used by the hardrware-accelerator.
/// \param[in]  logger       OCUDU logger.
bool enqueue_ldpc_dec_operation(bbdev_pusch_decoder_configuration& op_cfg,
                                uint16_t                           num_dec_ops,
                                uint16_t                           id,
                                uint16_t                           dec_queue_id,
                                ocudulog::basic_logger&            logger);

/// \brief Dequeues processed operations from the hardware-accelerated LDPC decoder.
///
/// \param[out] op_cfg       bbdev-based PUSCH decoder operation configuration structure.
/// \param[in]  num_dec_ops  Number of decoder operations to be dequeued.
/// \param[in]  id           bbdev ID of the hardware-accelerator.
/// \param[in]  dec_queue_id bbdev ID of the queue used by the hardrware-accelerator.
/// \param[in]  logger       OCUDU logger.
bool dequeue_ldpc_dec_operation(bbdev_pusch_decoder_configuration& op_cfg,
                                uint16_t                           num_dec_ops,
                                uint16_t                           id,
                                uint16_t                           dec_queue_id,
                                ocudulog::basic_logger&            logger);

/// \brief Reads the contents of the bbdev output mbuffer structures from the hardware-accelerated LPDC decoder
/// operations.
///
/// \param[out] data      The output data from the hardware-accelerated LDPC decoder operations.
/// \param[out] soft_data The output soft-data from the hardware-accelerated LDPC decoder operations.
/// \param[in]  op_cfg    bbdev-based PUSCH decoder operation configuration structure.
/// \return Length of the soft-combining output data.
uint32_t
read_ldpc_dec_bbdev_data(span<uint8_t> data, span<int8_t> soft_data, bbdev_pusch_decoder_configuration& op_cfg);

/// \brief Computes the output soft-data length to be retrieved from the HARQ buffer, based on the minimum circular
/// buffer size required to perform rate matching for a given code block.
///
/// \param[in] dec_cfg bbdev LDPC decoder configuration structure.
/// \return output soft-data length.
uint16_t compute_softdata_len(const ::rte_bbdev_op_ldpc_dec& dec_cfg);

} // namespace dpdk
} // namespace ocudu
