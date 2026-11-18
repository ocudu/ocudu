// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/mpmc_queue.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/phy/upper/channel_processors/pusch/formatters.h"
#include "ocudu/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"
#include <algorithm>

namespace ocudu {

/// PUSCH processor wrapper. It appends its identifier into the free list when the processing is finished.
class pusch_processor_wrapper : public pusch_processor, private pusch_processor_result_notifier
{
public:
  /// Pool of PUSCH processor wrappers.
  using pool = bounded_unique_object_pool<pusch_processor_wrapper>;

  /// Creates a pusch processor wrapper from another pusch processor.
  explicit pusch_processor_wrapper(std::unique_ptr<pusch_processor> processor_) : processor(std::move(processor_))
  {
    ocudu_assert(processor, "Invalid pusch processor.");
  }

  /// Creates a PUSCH processor wrapper from another PUSCH processor wrapper.
  pusch_processor_wrapper(pusch_processor_wrapper&& other) :
    unique_pool_ptr(std::move(other.unique_pool_ptr)), notifier(other.notifier), processor(std::move(other.processor))
  {
    other.notifier = nullptr;
  }

  // See pusch_processor interface for documentation.
  void process(pusch_processor_result_notifier& notifier_,
               const resource_grid_reader&      grid,
               const common_parameters&         common,
               span<ue_specific_parameters>     ues) override
  {
    // Save original notifier.
    [[maybe_unused]] pusch_processor_result_notifier* prev_notifier = std::exchange(notifier, &notifier_);
    ocudu_assert(prev_notifier == nullptr, "PUSCH processor is in use.");

    // Each UE notifies its data.
    nof_pending_sch = ues.size();

    // Process.
    processor->process(*this, grid, common, ues);
  }

  /// Sets the unique pointer that will be released upon the completion of the PUSCH processing.
  void set_unique_ptr(pool::ptr unique_token_) { unique_pool_ptr = std::move(unique_token_); }

private:
  // See pusch_processor_result_notifier for documentation.
  void on_uci(const pusch_processor_result_control& uci) override
  {
    ocudu_assert(notifier != nullptr, "Invalid notifier.");
    notifier->on_uci(uci);
  }

  // See pusch_processor_result_notifier for documentation.
  void on_sch(const pusch_processor_result_data& sch) override
  {
    unsigned prev_nof_pending_sch = nof_pending_sch.fetch_sub(1);
    ocudu_assert(prev_nof_pending_sch != 0, "All the UEs have already notified their data.");

    // Forward the data of the UE if other UEs are pending.
    if (prev_nof_pending_sch != 1) {
      ocudu_assert(notifier != nullptr, "Invalid notifier.");
      notifier->on_sch(sch);
      return;
    }

    // Notify the completion of the processing.
    pusch_processor_result_notifier* current_notifier = std::exchange(notifier, nullptr);
    ocudu_assert(current_notifier != nullptr, "Invalid notifier.");
    current_notifier->on_sch(sch);

    // Return the PUSCH processor identifier to the free list.
    unique_pool_ptr = nullptr;
  }

  /// Processor identifier within the pool.
  pool::ptr unique_pool_ptr;
  /// Current PUSCH processor notifier.
  pusch_processor_result_notifier* notifier = nullptr;
  /// Number of UEs that have not notified their data yet.
  std::atomic<unsigned> nof_pending_sch = 0;
  /// Wrapped PUSCH processor.
  std::unique_ptr<pusch_processor> processor;
};

/// \brief PUSCH processor pool
///
/// It contains PUSCH processors that are asynchronously executed. The processing of a PUSCH transmission is dropped if
/// there are no free PUSCH processors available.
///
class pusch_processor_pool : public pusch_processor
{
public:
  /// Alias for the common UCI processor pool. The processors of this pool operate synchronously from the thread that
  /// calls processing.
  using uci_processor_pool = bounded_unique_object_pool<pusch_processor>;

  /// Creates a PUSCH processor pool from a list of processors. Ownership is transferred to the pool.
  pusch_processor_pool(span<std::unique_ptr<pusch_processor_wrapper>> processors_,
                       std::shared_ptr<uci_processor_pool>            uci_processors_) :
    logger(ocudulog::fetch_basic_logger("PHY")), processors(processors_), uci_processors(uci_processors_)
  {
  }

  // See interface for documentation.
  void process(pusch_processor_result_notifier& notifier,
               const resource_grid_reader&      grid,
               const common_parameters&         common,
               span<ue_specific_parameters>     ues) override
  {
    // Get a processor and process normally.
    auto unique_processor = processors.get();
    if (unique_processor) {
      // Save reference to the processor.
      pusch_processor& processor = *unique_processor;

      // Set unique pointer, it will be returned to the pool when the processing is completed.
      unique_processor->set_unique_ptr(std::move(unique_processor));

      // Process PUSCH.
      processor.process(notifier, grid, common, ues);
      return;
    }

    // Determine all PUSCH are for data (UL-SCH). It will be false if at least one UE contains UCI.
    bool only_ul_sch = std::none_of(ues.begin(), ues.end(), [](const ue_specific_parameters& ue) {
      return (ue.grant.uci.nof_harq_ack > 0) || (ue.grant.uci.nof_csi_part1 > 0);
    });

    // Discard UL-SCH if only data is allocated.
    if (only_ul_sch) {
      for (auto& ue : ues) {
        logger.warning(common.slot.sfn(),
                       common.slot.slot_index(),
                       "PUSCH processing queue is full. Dropping PUSCH {:s} for UE {:s}.",
                       common,
                       ue.grant);
        unlock_rm_buffer_and_notify_discarded_sch(notifier, ue);
      }
      return;
    }

    // Process PUSCH using the UCI processors. Same processing, it ignores UL-SCH.
    uci_processor_pool::ptr uci_processor = uci_processors->get();
    if (uci_processor) {
      logger.warning(
          common.slot.sfn(), common.slot.slot_index(), "PUSCH processing queue is full. Processing UCI {:s}.", common);
      uci_processor->process(notifier, grid, common, ues);
      return;
    }

    // Failed to get a UCI processor, report discarded UCI and UL-SCH.
    for (auto& ue : ues) {
      logger.warning(common.slot.sfn(),
                     common.slot.slot_index(),
                     "PUSCH processing queue is full. Failed processing UCI {:s} for UE {:s}.",
                     common,
                     ue.grant);

      // Notify the UCI before the data, as the data notification completes the transmission.
      if ((ue.grant.uci.nof_harq_ack > 0) || (ue.grant.uci.nof_csi_part1 > 0)) {
        notifier.on_uci(
            {.context   = ue.grant.context,
             .harq_ack  = {.payload = uci_payload_type(ue.grant.uci.nof_harq_ack), .status = uci_status::invalid},
             .csi_part1 = {.payload = uci_payload_type(ue.grant.uci.nof_csi_part1), .status = uci_status::invalid},
             .csi_part2 = {},
             .csi       = {}});
      }

      unlock_rm_buffer_and_notify_discarded_sch(notifier, ue);
    }
  }

private:
  /// Unlocks rate matching buffer and reports discarded data results for the UEs that carry shared channel data.
  static void unlock_rm_buffer_and_notify_discarded_sch(pusch_processor_result_notifier& notifier,
                                                        ue_specific_parameters&          ue)
  {
    if (ue.rm_buffer.is_valid()) {
      ue.rm_buffer.unlock();
    }
    if (ue.grant.codeword.has_value()) {
      notifier.on_sch({.context = ue.grant.context,
                       .data    = {.tb_crc_ok = false, .nof_codeblocks_total = 0, .ldpc_decoder_stats = {}},
                       .csi     = {}});
    }
  }

  /// Physical layer logger.
  ocudulog::basic_logger& logger;
  /// Actual PUSCH processor pool.
  pusch_processor_wrapper::pool processors;
  /// UCI processor pool, used only the normal processor pool runs out of processors or the PUSCH transmission only
  /// contains UCI.
  std::shared_ptr<uci_processor_pool> uci_processors;
};

} // namespace ocudu
