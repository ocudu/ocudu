// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/hal/dpdk/bbdev/bbdev.h"
#include "fmt/format.h"

using namespace ocudu;
using namespace dpdk;

expected<::rte_bbdev_info> dpdk::bbdev_start(const bbdev_acc_configuration& cfg, ocudulog::basic_logger& logger)
{
  ::rte_bbdev_info info = {};

  // Get information on the utilized bbdev hardware-accelerator.
  ::rte_bbdev_info_get(cfg.id, &info);

  // Setup the required queues.
  unsigned nof_queues = cfg.nof_ldpc_enc_lcores + cfg.nof_ldpc_dec_lcores + cfg.nof_fft_lcores;
  if (::rte_bbdev_setup_queues(cfg.id, nof_queues, info.socket_id) < 0) {
    logger.error("[bbdev] queues for device {} not setup properly (requested: ldpc_enc={}, ldpc_dec={}, fft={}, "
                 "available: ldpc_enc={}, ldpc_dec={}, fft={}).",
                 cfg.id,
                 cfg.nof_ldpc_enc_lcores,
                 cfg.nof_ldpc_dec_lcores,
                 cfg.nof_fft_lcores,
                 info.drv.num_queues[RTE_BBDEV_OP_LDPC_ENC],
                 info.drv.num_queues[RTE_BBDEV_OP_LDPC_DEC],
                 info.drv.num_queues[RTE_BBDEV_OP_FFT]);
    return make_unexpected(default_error_t{});
  }

  // Basic checking of hardware-accelerator capabilities.
  const ::rte_bbdev_op_cap* op_cap           = info.drv.capabilities;
  bool                      ldpc_enc_capable = false;
  bool                      ldpc_dec_capable = false;
  bool                      fft_capable      = false;

  while (op_cap->type != RTE_BBDEV_OP_NONE) {
    if (op_cap->type == RTE_BBDEV_OP_LDPC_ENC) {
      ldpc_enc_capable = true;
    } else if (op_cap->type == RTE_BBDEV_OP_LDPC_DEC) {
      ldpc_dec_capable = true;
    } else if (op_cap->type == RTE_BBDEV_OP_FFT) {
      fft_capable = true;
    }
    ++op_cap;
  }
  if ((cfg.nof_ldpc_enc_lcores > 0 && !ldpc_enc_capable) || (cfg.nof_ldpc_dec_lcores > 0 && !ldpc_dec_capable) ||
      (cfg.nof_fft_lcores > 0 && !fft_capable)) {
    logger.error("[bbdev] device {} does not provide the requested acceleration functions.", cfg.id);
    return make_unexpected(default_error_t{});
  }

  // Enable interrupts (only for ACC100). Not possible when using 'vfio-pci' if OFH is using DPDK as well.
  std::string bbdev_driver = info.drv.driver_name;
  if (bbdev_driver.find("acc100") != std::string::npos) {
    int err = ::rte_bbdev_intr_enable(cfg.id);
    if (err < 0) {
      std::string bbdev_vf_error = "";
      if (bbdev_driver.find("vf") != std::string::npos) {
        bbdev_vf_error = fmt::format(" To enable interrupts with {} use '--vfio-intr=msi'. If OFH is using DPDK "
                                     "'--log-level=error' is advised instead.",
                                     bbdev_driver);
      }
      logger.warning(
          "[bbdev] failed to enable interrupts for device {}: '{}'.{}", cfg.id, strerror(err), bbdev_vf_error);
    }
  }

  // Configure the queues (only those required).
  // The type of accelerated function assigned to a queue is following a known pattern: LDPC encoder, LDPC decoder and
  // FFT.
  ::rte_bbdev_queue_conf queue_conf = {0};
  unsigned               queue_id   = 0;
  queue_conf.socket                 = info.socket_id;
  queue_conf.queue_size             = info.drv.queue_size_lim;
  if (cfg.nof_ldpc_enc_lcores > 0) {
    // Queue configuration starts from the highest priority level supported.
    queue_conf.priority = 0;
    queue_conf.op_type  = RTE_BBDEV_OP_LDPC_ENC;
    for (unsigned qid = 0, lastq = cfg.nof_ldpc_enc_lcores; qid != lastq; ++qid) {
      // A maximum of 16 queues is supported per priority level.
      if (qid > 0 && qid % 16 == 0) {
        ++queue_conf.priority;
      }
      if (::rte_bbdev_queue_configure(cfg.id, queue_id, &queue_conf) < 0) {
        logger.error("[bbdev] device {} queue {} (ldpc encoder) not configured properly (priority = {}).",
                     cfg.id,
                     queue_id,
                     queue_conf.priority);
        return make_unexpected(default_error_t{});
      }
      ++queue_id;
    }
  }
  if (cfg.nof_ldpc_dec_lcores > 0) {
    // Queue configuration starts from the highest priority level supported.
    queue_conf.priority = 0;
    queue_conf.op_type  = RTE_BBDEV_OP_LDPC_DEC;
    for (unsigned qid = 0, lastq = cfg.nof_ldpc_dec_lcores; qid != lastq; ++qid) {
      // A maximum of 16 queues is supported per priority level.
      if (qid > 0 && qid % 16 == 0) {
        ++queue_conf.priority;
      }
      if (::rte_bbdev_queue_configure(cfg.id, queue_id, &queue_conf) < 0) {
        logger.error("[bbdev] device {} queue {} (ldpc decoder) not configured properly (priority = {}).",
                     cfg.id,
                     queue_id,
                     queue_conf.priority);
        return make_unexpected(default_error_t{});
      }
      ++queue_id;
    }
  }
  if (cfg.nof_fft_lcores > 0) {
    // Queue configuration starts from the highest priority level supported.
    queue_conf.priority = 0;
    queue_conf.op_type  = RTE_BBDEV_OP_FFT;
    for (unsigned qid = 0, lastq = cfg.nof_fft_lcores; qid != lastq; ++qid) {
      // A maximum of 16 queues is supported per priority level.
      if (qid > 0 && qid % 16 == 0) {
        ++queue_conf.priority;
      }
      if (::rte_bbdev_queue_configure(cfg.id, queue_id, &queue_conf) < 0) {
        logger.error("[bbdev] device {} queue {} (fft) not configured properly (priority = {}).",
                     cfg.id,
                     queue_id,
                     queue_conf.priority);
        return make_unexpected(default_error_t{});
      }
      ++queue_id;
    }
  }

  if (::rte_bbdev_start(cfg.id) < 0) {
    logger.error("[bbdev] device {} not started.", cfg.id);
    return make_unexpected(default_error_t{});
  }

  return info;
}

bool dpdk::bbdev_stop(unsigned dev_id, ocudulog::basic_logger& logger)
{
  if (::rte_bbdev_close(dev_id) < 0) {
    logger.error("[bbdev] device {} not closed.", dev_id);
    return false;
  }

  return true;
}

::rte_mempool* dpdk::create_op_pool(const char*             pool_name,
                                    ::rte_bbdev_op_type     op_type,
                                    uint16_t                nof_elements,
                                    int                     socket,
                                    ocudulog::basic_logger& logger)

{
  static constexpr unsigned NB_MBUF = 2048;

  ::rte_mempool* op_pool = ::rte_bbdev_op_pool_create(pool_name, op_type, NB_MBUF, nof_elements, socket);

  if (op_pool == nullptr) {
    logger.error("dpdk: create_op_pool '{}' failed", pool_name);
  }

  return op_pool;
}
