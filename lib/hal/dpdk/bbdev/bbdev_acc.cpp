// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/hal/dpdk/bbdev/bbdev_acc.h"
#include "ocudu/hal/dpdk/bbdev/bbdev.h"

using namespace ocudu;
using namespace dpdk;

bbdev_acc::bbdev_acc(const bbdev_acc_configuration& cfg,
                     const ::rte_bbdev_info&        info_,
                     ocudulog::basic_logger&        logger_) :
  id(cfg.id),
  info(info_),
  nof_ldpc_enc_lcores(cfg.nof_ldpc_enc_lcores),
  nof_ldpc_dec_lcores(cfg.nof_ldpc_dec_lcores),
  nof_fft_lcores(cfg.nof_fft_lcores),
  available_ldpc_enc_queue(MAX_NOF_BBDEV_QUEUES),
  available_ldpc_dec_queue(MAX_NOF_BBDEV_QUEUES),
  available_fft_queue(MAX_NOF_BBDEV_QUEUES),
  msg_mbuf_size(cfg.msg_mbuf_size),
  rm_mbuf_size(cfg.rm_mbuf_size),
  nof_mbuf(cfg.nof_mbuf),
  logger(logger_)
{
  unsigned nof_vfs = nof_ldpc_enc_lcores + nof_ldpc_dec_lcores + nof_fft_lcores;
  ocudu_assert(nof_ldpc_enc_lcores <= MAX_NOF_BBDEV_VF_INSTANCES,
               "Requested {} LDPC encoder VFs but only {} are available.",
               nof_ldpc_enc_lcores,
               MAX_NOF_BBDEV_VF_INSTANCES);
  ocudu_assert(nof_ldpc_dec_lcores <= MAX_NOF_BBDEV_VF_INSTANCES,
               "Requested {} LDPC decoder VFs but only {} are available.",
               nof_ldpc_dec_lcores,
               MAX_NOF_BBDEV_VF_INSTANCES);
  ocudu_assert(nof_fft_lcores <= MAX_NOF_BBDEV_VF_INSTANCES,
               "Requested {} FFT VFs but only {} are available.",
               nof_fft_lcores,
               MAX_NOF_BBDEV_VF_INSTANCES);
  ocudu_assert(nof_vfs <= MAX_NOF_BBDEV_QUEUES,
               "Requested {} BBDEV VFs but only {} are available.",
               nof_vfs,
               MAX_NOF_BBDEV_QUEUES);

  nof_ldpc_enc_instances = 0;
  nof_ldpc_dec_instances = 0;
  // Hardware-accelerated LDPC encoder functions use queues 0:(nof_ldpc_enc_lcores - 1)
  for (unsigned qid = 0, lastq = nof_ldpc_enc_lcores; qid != lastq; qid++) {
    bool discard = available_ldpc_enc_queue.try_push(qid);
    (void)discard;
  }
  // Hardware-accelerated LDPC decoder functions use queues nof_ldpc_enc_lcores:(nof_ldpc_enc_lcores +
  // nof_ldpc_dec_lcores - 1)
  for (unsigned qid = nof_ldpc_enc_lcores, lastq = nof_ldpc_enc_lcores + nof_ldpc_dec_lcores; qid != lastq; qid++) {
    bool discard = available_ldpc_dec_queue.try_push(qid);
    (void)discard;
  }
  // Hardware-accelerated FFT functions use queues (nof_ldpc_enc_lcores + nof_ldpc_dec_lcores):(nof_ldpc_enc_lcores +
  // nof_ldpc_dec_lcores + nof_fft_lcores - 1)
  for (unsigned qid = nof_ldpc_enc_lcores + nof_ldpc_dec_lcores, lastq = nof_vfs; qid != lastq; qid++) {
    bool discard = available_fft_queue.try_push(qid);
    (void)discard;
  }
}

bbdev_acc::~bbdev_acc()
{
  // bbdev device stop procedure.
  ::bbdev_stop(id, logger);
}

int bbdev_acc::reserve_queue(::rte_bbdev_op_type op_type)
{
  int queue_id = -1;

  if (op_type == RTE_BBDEV_OP_LDPC_ENC) {
    // Try to get an available LDPC encoder queue.
    unsigned qid;
    if (available_ldpc_enc_queue.try_pop(qid)) {
      queue_id = qid;
    }
  } else if (op_type == RTE_BBDEV_OP_LDPC_DEC) {
    // Try to get an available LDPC decoder queue.
    unsigned qid;
    if (available_ldpc_dec_queue.try_pop(qid)) {
      queue_id = qid;
    }
  } else {
    // Try to get an available FFT queue.
    unsigned qid;
    if (available_fft_queue.try_pop(qid)) {
      queue_id = qid;
    }
  }

  return queue_id;
}

void bbdev_acc::free_queue(::rte_bbdev_op_type op_type, unsigned queue_id)
{
  if (op_type == RTE_BBDEV_OP_LDPC_ENC) {
    // Free a LDPC encoder queue.
    while (!available_ldpc_enc_queue.try_push(queue_id)) {
    }
  } else if (op_type == RTE_BBDEV_OP_LDPC_DEC) {
    // Free a LDPC decoder queue.
    while (!available_ldpc_dec_queue.try_push(queue_id)) {
    }
  } else {
    // Free a FFT queue.
    while (!available_fft_queue.try_push(queue_id)) {
    }
  }
}
