// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ru/ofh/ru_ofh_executor_mapper.h"
#include "ocudu/adt/mpmc_queue.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/ofh/ofh_constants.h"
#include "ocudu/ru/ofh/ru_ofh_executor_mapper_factory.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/executors/strand_executor.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/ocudu_assert.h"
#include <algorithm>
#include <cmath>

using namespace ocudu;

namespace {

/// Open Fronthaul sector executor mapper implementation.
class ru_ofh_sector_executor_mapper_impl : public ofh::ofh_sector_executor_mapper
{
  /// Size of each per-eAxC strand task queue. Limit it to 8 slots (a strand receives a single task per slot).
  static constexpr unsigned strand_queue_size = 8u;

  /// Storage type for the strand executors allocated for configured eAxCs.
  using eaxc_task_strand_map =
      slotted_id_table<unsigned, std::unique_ptr<task_executor>, ofh::MAX_SUPPORTED_EAXC_ID_VALUE>;

public:
  ru_ofh_sector_executor_mapper_impl(unsigned                       sector_id_,
                                     task_executor&                 txrx_exec_,
                                     task_executor&                 downlink_exec,
                                     std::unique_ptr<task_executor> uplink_exec_,
                                     const std::vector<unsigned>&   eaxcs) :
    sector_id(sector_id_), txrx_exec(txrx_exec_), uplink_exec(std::move(uplink_exec_))
  {
    ocudu_assert(uplink_exec, "Invalid OFH uplink executor");

    // The sector mapper owns one serialization strand per eAxC.
    // All messages of a given eAxC are processed in enqueue order (i.e. slot order), so that the eCPRI sequence
    // identifiers are generated in the same order, while different eAxCs are processed concurrently in the
    // underlying thread pool.
    //
    // The serialization strands are grouped by the type of the task they are aimed for: DL C-Plane, DL U-Plane and UL
    // C-Plane messages generation.
    for (auto eaxc : eaxcs) {
      dl_cp_strands.emplace(
          eaxc, make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(downlink_exec, strand_queue_size));
      ocudu_assert(dl_cp_strands[eaxc], "Failed to create a strand for downlink C-Plane tasks execution");
    }

    for (auto eaxc : eaxcs) {
      dl_up_strands.emplace(
          eaxc, make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(downlink_exec, strand_queue_size));
      ocudu_assert(dl_up_strands[eaxc], "Failed to create a strand for downlink U-Plane tasks execution");
    }

    // Uplink and PRACH C-Plane messages are not dispatched per eAxC, so the tasks are serialized through a single
    // strand. We still need a strand because it is possible that requests for two neighboring slots are processed in
    // parallel by the thread pool, and thus the eCPRI sequence identifiers might get mixed between slots.
    ul_cp_strand = make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(downlink_exec, strand_queue_size);
    ocudu_assert(ul_cp_strand, "Failed to create a strand for uplink C-Plane tasks execution");
  }

  // See interface for documentation.
  task_executor& get_txrx_executor() const override { return txrx_exec; }

  // See interface for documentation.
  task_executor& get_dl_cp_executor(unsigned eaxc) const override
  {
    ocudu_assert(dl_cp_strands.contains(eaxc) && dl_cp_strands[eaxc] != nullptr,
                 "No OFH executor found for sector '{}' eAxC '{}' downlink C-Plane processing",
                 sector_id,
                 eaxc);

    return *dl_cp_strands[eaxc];
  }

  // See interface for documentation.
  task_executor& get_dl_up_executor(unsigned eaxc) const override
  {
    ocudu_assert(dl_up_strands.contains(eaxc) && dl_up_strands[eaxc] != nullptr,
                 "No OFH executor found for sector '{}' eAxC '{}' downlink U-Plane processing",
                 sector_id,
                 eaxc);

    return *dl_up_strands[eaxc];
  }

  task_executor& get_ul_cp_executor() const override { return *ul_cp_strand; }

  // See interface for documentation.
  task_executor& get_uplink_executor() const override { return *uplink_exec; }

private:
  unsigned                       sector_id;
  task_executor&                 txrx_exec;
  std::unique_ptr<task_executor> uplink_exec;
  eaxc_task_strand_map           dl_cp_strands;
  eaxc_task_strand_map           dl_up_strands;
  std::unique_ptr<task_executor> ul_cp_strand;
};

/// Open Fronthaul RU executor mapper implementation managing executor mappers of the configured sectors.
class ru_ofh_executor_mapper_impl : public ru_ofh_executor_mapper
{
  /// Default queue size.
  static constexpr unsigned default_queue_size = 2048;

public:
  explicit ru_ofh_executor_mapper_impl(const ru_ofh_executor_mapper_config& config) :
    timing_exec(config.timing_executor)
  {
    report_error_if_not(config.downlink_executor, "Invalid Downlink executor");
    report_error_if_not(config.uplink_executor, "Invalid Uplink executor");
    report_error_if_not(config.timing_executor, "Invalid Timing executor");

    report_error_if_not(!config.txrx_executors.empty(), "TXRX executors for OFH must not be empty");
    report_error_if_not(std::all_of(config.txrx_executors.begin(),
                                    config.txrx_executors.end(),
                                    [](auto& exec) { return exec != nullptr; }),
                        "TXRX executors are not initialized properly");

    // Number of OFH sectors served by a single executor for transmitter and receiver tasks.
    unsigned nof_txrx_threads = config.txrx_executors.size();
    unsigned nof_sectors      = config.dl_eaxc_per_sector.size();
    unsigned nof_sectors_per_txrx_thread =
        (nof_sectors > nof_txrx_threads) ? static_cast<unsigned>(std::ceil(nof_sectors / float(nof_txrx_threads))) : 1;

    // Reserve up-front to avoid reallocating the sector mappers, which hold reference members and are not reassignable.
    sector_mappers.reserve(nof_sectors);
    for (unsigned i = 0; i != nof_sectors; ++i) {
      sector_mappers.emplace_back(
          i,
          *config.txrx_executors[i / nof_sectors_per_txrx_thread],
          *config.downlink_executor,
          make_task_strand_ptr<concurrent_queue_policy::lockfree_mpmc>(*config.uplink_executor, default_queue_size),
          config.dl_eaxc_per_sector[i]);
    }
  }

  // See interface for documentation.
  ofh::ofh_sector_executor_mapper& get_sector_mapper(unsigned cell_index) override
  {
    ocudu_assert(cell_index < sector_mappers.size(),
                 "The cell index {} exceeds the number of cells {}",
                 cell_index,
                 sector_mappers.size());
    return sector_mappers[cell_index];
  }

  // See interface for documentation.
  task_executor& timing_executor() override { return *timing_exec; }

private:
  task_executor*                                  timing_exec;
  std::vector<ru_ofh_sector_executor_mapper_impl> sector_mappers;
};

} // namespace

std::unique_ptr<ru_ofh_executor_mapper>
ocudu::create_ofh_ru_executor_mapper(const ru_ofh_executor_mapper_config& config)
{
  return std::make_unique<ru_ofh_executor_mapper_impl>(config);
}
