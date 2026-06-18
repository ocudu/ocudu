// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../rnti_value_table.h"
#include "rlf_detector.h"
#include "ocudu/mac/mac_cell_control_information_handler.h"
#include "ocudu/ran/csi_report/csi_report_configuration.h"
#include "ocudu/scheduler/scheduler_configurator.h"
#include "ocudu/scheduler/scheduler_feedback_handler.h"
#include <atomic>

namespace ocudu {

struct ul_sched_info;
struct pucch_info;

using du_rnti_table = rnti_value_table<du_ue_index_t, du_ue_index_t::INVALID_DU_UE_INDEX>;

class uci_cell_decoder
{
public:
  uci_cell_decoder(const sched_cell_configuration_request_message& cell_cfg,
                   const du_rnti_table&                            rnti_table_,
                   rlf_detector&                                   rlf_hdlr_);

  /// \brief Store information relative to expected UCIs to be decoded.
  void
  store_uci(slot_point uci_sl, span<const pucch_info> scheduled_pucchs, span<const ul_sched_info> scheduled_puschs);

  /// \brief Decode received MAC UCI indication and convert it to scheduler UCI indication.
  uci_indication decode_uci(const mac_uci_indication_message& msg);

private:
  struct uci_context {
    rnti_t                   rnti = rnti_t::INVALID_RNTI;
    csi_report_configuration csi_rep_cfg;
  };

  /// \brief Expected UCI reports for a single slot in the ring buffer.
  /// \note Only one writer accesses exclusively each slot entry, but multiple readers access it concurrently with
  /// each other.
  /// \note When the writer catches up with the reader(s), an overflow is warned and data is discarded.
  struct slot_ucis_entry {
    /// Access guard: 0 == idle, < 0 == writer holds it, > 0 == number of readers currently holding it.
    std::atomic<int32_t> access_guard{0};
    /// Slot whose expected UCIs are currently stored in this entry.
    slot_point slot;
    /// UCIs pending indication for the stored slot.
    static_vector<uci_context, MAX_PUCCH_PDUS_PER_SLOT + MAX_PUSCH_PDUS_PER_SLOT> ucis;

    /// Tries to acquire shared (reader) access. Fails if the writer currently holds the entry.
    bool try_acquire_read()
    {
      int32_t cur = access_guard.load(std::memory_order_acquire);
      while (cur >= 0) {
        // We keep trying when it is only readers accessing the entry.
        if (access_guard.compare_exchange_weak(cur, cur + 1, std::memory_order_acquire, std::memory_order_relaxed)) {
          return true;
        }
      }
      return false;
    }
    void release_read() { access_guard.fetch_sub(1, std::memory_order_release); }

    /// Tries to acquire exclusive (writer) access. Fails if any reader or writer currently holds the entry.
    bool try_acquire_write()
    {
      int32_t expected = 0;
      return access_guard.compare_exchange_strong(expected, -1, std::memory_order_acquire, std::memory_order_relaxed);
    }
    void release_write() { access_guard.store(0, std::memory_order_release); }
  };

  size_t to_grid_index(slot_point slot) const { return slot.count() % expected_uci_report_grid.size(); }

  const du_rnti_table&    rnti_table;
  du_cell_index_t         cell_index;
  bool                    aperiodic_csi_report;
  rlf_detector&           rlf_handler;
  ocudulog::basic_logger& logger;

  std::vector<slot_ucis_entry> expected_uci_report_grid;
};

} // namespace ocudu
