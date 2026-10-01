// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Definition of the specific functions used by the bbdeb-based hardware-accelerated LDPC encoder
/// implementations.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <rte_bbdev_op.h>

namespace ocudu {
namespace dpdk {

/// Configuration parameters of a bbdev-based PDSCH encoder operation.
struct bbdev_pdsch_encoder_configuration {
  /// Pointer to a bbdev encoder operation configuration structure.
  ::rte_bbdev_enc_op* op;
  /// Operation mode of the PDSCH encoder (CB = true, TB = false [default]).
  bool cb_mode = true;
  // Request to unpack the accelerator output data (enabled by default).
  bool do_unpack = true;
};

/// \brief Sets the contents of the bbdev configuration structure for the hardware-accelerated LPDC encoder operations.
///
/// \param[out] op_cfg bbdev-based PDSCH encoder operation configuration structure.
/// \param[in]  cfg    PDSCH encoder configuration parameter-structure.
void set_ldpc_enc_bbdev_config(bbdev_pdsch_encoder_configuration&                op_cfg,
                               const ocudu::hal::hw_pdsch_encoder_configuration& cfg);

/// \brief Sets the contents of the bbdev input mbuffer structures for the hardware-accelerated LPDC encoder operations.
///
/// \param[out] op_cfg       bbdev-based PDSCH encoder operation configuration structure.
/// \param[in]  in_mbuf_ptr  Input data mbuf pointer for the hardware-accelerated LDPC encoder operations.
/// \param[in]  out_mbuf_ptr Output data mbuf pointer for the hardware-accelerated LDPC encoder operations.
/// \param[in]  data         The input data to the hardware-accelerated LDPC encoder operations.
/// \param[in]  tb_crc       The TB CRC computed from 'data'. Only required in TB mode.
/// \param[in]  logger       OCUDU logger.
bool set_ldpc_enc_bbdev_data(bbdev_pdsch_encoder_configuration& op_cfg,
                             ::rte_mbuf&                        in_mbuf_ptr,
                             ::rte_mbuf&                        out_mbuf_ptr,
                             span<const uint8_t>                data,
                             span<const uint8_t>                tb_crc,
                             ocudulog::basic_logger&            logger);

/// \brief Enqueues new operations in the hardware-accelerated LDPC encoder.
///
/// \param[out] op_cfg       bbdev-based PDSCH encoder operation configuration structure.
/// \param[in]  num_enc_ops  Number of encoder operations to be enqueued.
/// \param[in]  id           bbdev ID of the hardware-accelerator.
/// \param[in]  enc_queue_id bbdev ID of the queue used by the hardrware-accelerator.
/// \param[in]  logger       OCUDU logger.
bool enqueue_ldpc_enc_operation(bbdev_pdsch_encoder_configuration& op_cfg,
                                uint16_t                           num_enc_ops,
                                uint16_t                           id,
                                uint16_t                           enc_queue_id,
                                ocudulog::basic_logger&            logger);

/// \brief Dequeues processed operations from the hardware-accelerated LDPC encoder.
///
/// \param[out] op_cfg       bbdev-based PDSCH encoder operation configuration structure.
/// \param[in]  num_enc_ops  Number of encoder operations to be dequeued.
/// \param[in]  id           bbdev ID of the hardware-accelerator.
/// \param[in]  enc_queue_id bbdev ID of the queue used by the hardrware-accelerator.
/// \param[in]  logger       OCUDU logger.
bool dequeue_ldpc_enc_operation(bbdev_pdsch_encoder_configuration& op_cfg,
                                uint16_t                           num_enc_ops,
                                uint16_t                           id,
                                uint16_t                           enc_queue_id,
                                ocudulog::basic_logger&            logger);

/// \brief Reads the contents of the bbdev output mbuffer structures from the hardware-accelerated LPDC encoder
/// operations.
///
/// \param[out] data        The output data from the hardware-accelerated LDPC encoder operations.
/// \param[out] packed_data Temporary storage for the packed outputs from the hardware-accelerator.
/// \param[in]  op_cfg      bbdev-based PDSCH encoder operation configuration structure.
void read_ldpc_enc_bbdev_data(span<uint8_t> data, span<uint8_t> packed_data, bbdev_pdsch_encoder_configuration& op_cfg);

} // namespace dpdk
} // namespace ocudu
