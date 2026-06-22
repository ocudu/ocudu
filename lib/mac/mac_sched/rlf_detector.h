// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/mac_config.h"
#include "ocudu/mac/mac_ue_configurator.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/support/ocudu_assert.h"
#include <array>
#include <atomic>

namespace ocudu {

/// \brief Metrics used as reference to detect/trigger RLF.
struct rlf_metrics {
  unsigned max_consecutive_dl_kos;
  unsigned max_consecutive_ul_kos;
  unsigned max_consecutive_csi_dtx;
};

/// \brief Detector of RLFs in the MAC, based on HARQ-ACK and CRC indications.
class rlf_detector
{
  // Each mac_rlf_cause indexes one KO counter dimension, so its values must stay contiguous from 0.
  static_assert(static_cast<size_t>(mac_rlf_cause::max_consecutive_harq_kos) == 0 and
                    static_cast<size_t>(mac_rlf_cause::max_consecutive_crc_kos) == 1 and
                    static_cast<size_t>(mac_rlf_cause::max_consecutive_csi_dtx) == 2,
                "mac_rlf_cause values must be contiguous from 0 to index the KO counters");

public:
  rlf_detector(const mac_expert_config& expert_cfg, task_executor& ctrl_exec_) :
    ctrl_exec(ctrl_exec_), logger(ocudulog::fetch_basic_logger("MAC"))
  {
    max_consecutive_kos.resize(expert_cfg.configs.size());
    for (auto mac_cfg_it = expert_cfg.configs.begin(); mac_cfg_it != expert_cfg.configs.end(); ++mac_cfg_it) {
      max_consecutive_kos[std::distance(expert_cfg.configs.begin(), mac_cfg_it)] =
          rlf_metrics{.max_consecutive_dl_kos  = mac_cfg_it->max_consecutive_dl_kos,
                      .max_consecutive_ul_kos  = mac_cfg_it->max_consecutive_ul_kos,
                      .max_consecutive_csi_dtx = mac_cfg_it->max_consecutive_csi_dtx};
    }
  }

  void add_ue(du_ue_index_t ue_index, mac_ue_radio_link_notifier& notifier)
  {
    ocudu_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", fmt::underlying(ue_index));

    // Reset UE counters. As they are not under use, we can use relaxed memory ordering.
    ues[ue_index].ko_counters[0].store(0, std::memory_order_relaxed);
    ues[ue_index].ko_counters[1].store(0, std::memory_order_relaxed);
    ues[ue_index].ko_counters[2].store(0, std::memory_order_relaxed);

    // Save notifier that is going to be used to report RLFs.
    ues[ue_index].notifier = &notifier;
  }

  void rem_ue(du_ue_index_t ue_index, du_cell_index_t cell_index)
  {
    ocudu_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", fmt::underlying(ue_index));

    // We store max+1, meaning that no NOK will reach counter==max, and trigger the notifier.
    auto& cell = max_consecutive_kos[cell_index];
    ues[ue_index].ko_counters[0].store(cell.max_consecutive_dl_kos + 1, std::memory_order_relaxed);
    ues[ue_index].ko_counters[1].store(cell.max_consecutive_ul_kos + 1, std::memory_order_relaxed);
    ues[ue_index].ko_counters[2].store(cell.max_consecutive_csi_dtx + 1, std::memory_order_relaxed);

    // Remove notifier.
    ues[ue_index].notifier = nullptr;
  }

  void handle_ack(du_ue_index_t ue_index, du_cell_index_t cell_index, bool ack)
  {
    handle_event(cell_index, ue_index, mac_rlf_cause::max_consecutive_harq_kos, ack);
  }

  void handle_crc(du_ue_index_t ue_index, du_cell_index_t cell_index, bool crc)
  {
    handle_event(cell_index, ue_index, mac_rlf_cause::max_consecutive_crc_kos, crc);
  }

  void handle_csi(du_ue_index_t ue_index, du_cell_index_t cell_index, bool csi_decoded)
  {
    handle_event(cell_index, ue_index, mac_rlf_cause::max_consecutive_csi_dtx, csi_decoded);
  }

  void handle_crnti_ce(du_ue_index_t ue_index)
  {
    // Note: The detection of a C-RNTI CE cancels any previous MAC-detected RLF.

    ues[ue_index].ko_counters[0].store(0, std::memory_order_relaxed);
    ues[ue_index].ko_counters[1].store(0, std::memory_order_relaxed);
    ues[ue_index].ko_counters[2].store(0, std::memory_order_relaxed);
    if (not ctrl_exec.defer([this, ue_index]() {
          auto* notifier = ues[ue_index].notifier;
          if (notifier != nullptr) {
            ues[ue_index].notifier->on_crnti_ce_received();
          }
        })) {
      logger.warning(
          "ue={}: Unable to report CRNTI CE to cancel any previous RLF detection. Cause: Task queue is full");
    }
  }

private:
  void handle_event(du_cell_index_t cell_index, du_ue_index_t ue_index, mac_rlf_cause cause, bool valid)
  {
    ocudu_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", ue_index);
    auto& u          = ues[ue_index];
    auto& ko_counter = u.ko_counters[(size_t)cause];

    if (valid) {
      // Reset counter back to zero.
      ko_counter.store(0, std::memory_order::memory_order_relaxed);
      return;
    }

    // Increment consecutive KOs.
    unsigned       current_count = ko_counter.fetch_add(1, std::memory_order::memory_order_relaxed) + 1;
    const unsigned max_kos       = get_max_kos(cell_index, cause);

    // Note: We use != instead of < to ensure only one notification is sent.
    if (current_count != max_kos) {
      return;
    }

    if (not ctrl_exec.defer([this, cell_index, ue_index, cause]() {
          auto* notifier = ues[ue_index].notifier;
          if (notifier == nullptr) {
            // UE likely removed while we were enqueueing this task. No need to notify RLF.
            return;
          }
          const unsigned ko_count = get_max_kos(cell_index, cause);
          const char*    ev_str   = cause == mac_rlf_cause::max_consecutive_harq_kos
                                        ? "HARQ-ACK KOs"
                                        : (cause == mac_rlf_cause::max_consecutive_crc_kos ? "CRC KOs" : "undecoded CSIs");
          logger.info("ue={}: RLF detected. Cause: {} consecutive {}", ue_index, ko_count, ev_str);

          // Notify upper layers.
          notifier->on_rlf_detected(cause);
        })) {
      logger.warning("ue={}: Failed to handle RLF detection. Cause: Task queue is full");
      ko_counter.store(0, std::memory_order_relaxed);
    }
  }

  unsigned get_max_kos(du_cell_index_t cell_index, mac_rlf_cause cause) const
  {
    const auto& cell = max_consecutive_kos[cell_index];
    return cause == mac_rlf_cause::max_consecutive_harq_kos
               ? cell.max_consecutive_dl_kos
               : (cause == mac_rlf_cause::max_consecutive_crc_kos ? cell.max_consecutive_ul_kos
                                                                  : cell.max_consecutive_csi_dtx);
  }

  // DL for index 0, UL for index 1, CSI for index 2 in each cell.
  static_vector<rlf_metrics, MAX_NOF_DU_CELLS> max_consecutive_kos;
  task_executor&                               ctrl_exec;
  ocudulog::basic_logger&                      logger;

  struct ue_context {
    // DL for index 0, UL for index 1, CSI for index 2.
    std::array<std::atomic<unsigned>, 3> ko_counters{};
    mac_ue_radio_link_notifier*          notifier = nullptr;
  };

  std::array<ue_context, MAX_NOF_DU_UES> ues;
};

} // namespace ocudu
