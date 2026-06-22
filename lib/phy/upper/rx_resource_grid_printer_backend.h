// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocuduvec/conversion.h"
#include "ocudu/phy/support/prach_buffer.h"
#include "ocudu/phy/support/prach_buffer_context.h"
#include "ocudu/phy/support/resource_grid_reader.h"
#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/phy/upper/upper_phy_rx_symbol_handler.h"
#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/prach/prach_constants.h"
#include "ocudu/ran/prach/prach_preamble_information.h"
#include "ocudu/support/executors/task_worker.h"
#include <fstream>

namespace ocudu {

/// \brief Receive resource grid printer backend.
///
/// Writes received resource grids and PRACH samples from an RU into a binary file.
///
/// The backend operates in two phases:
///
/// 1. **Buffering**: The consumer-facing entry points handle_rx_symbol() and handle_rx_prach_window() store copies
///    of resource grids and PRACH buffers into per-sector, per-slot ring buffers (5-slot deep).
/// 2. **Writing**: Trigger entry points on_grid_trigger() and on_prach_trigger() flush the buffered data to disk.
///    Triggers are activated by intercepting the upper physical-layer results notifier interface via on_new_sector();
///    the notifiers are wrapped and owned by the backend.
///
/// All file I/O is deferred to an internal \c task_worker to avoid blocking the upper PHY thread. The
/// binary output format is raw interleaved \c cf_t samples: for resource grids the layout is port-major (iterate ports,
/// then symbols, then subcarriers); for PRACH it is port-major (iterate ports, then replicas, then time samples).
class rx_resource_grid_printer_backend
{
public:
  /// \brief Constructs the backend and opens the output binary file.
  ///
  /// The file is opened in binary truncate mode. If the log-level is insufficient (info not enabled), an error is
  /// logged and no symbols will be written. If the port range is invalid (end <= start), an error is also logged.
  ///
  /// \param[in] logger_         Logger instance.
  /// \param[in] filename        Path of the binary output file.
  /// \param[in] nof_rb          Number of resource blocks (determines temporary buffer size).
  /// \param[in] ul_print_ports  Inclusive range of uplink antenna ports to dump.
  rx_resource_grid_printer_backend(ocudulog::basic_logger& logger_,
                                   const std::string&      filename,
                                   unsigned                nof_rb,
                                   interval<unsigned>      ul_print_ports) :
    logger(logger_),
    worker(async_worker_name, async_worker_queue_size),
    temp_buffer(nof_rb * NOF_SUBCARRIERS_PER_RB),
    temp_prach_buffer(prach_constants::LONG_SEQUENCE_LENGTH),
    start_port(ul_print_ports.start()),
    end_port(ul_print_ports.stop())
  {
    if (!logger.info.enabled()) {
      logger.error("Receive symbol enabled but logger level not enabled. No symbols will be printed.");
      return;
    }

    file = std::ofstream(filename, std::ios_base::binary | std::ios_base::trunc);
    if (!file.is_open()) {
      logger.error("RX_SYMBOL: failed to open file {}.", filename);
    }

    if (end_port <= start_port) {
      logger.error("End port {} is not larger than start port {}.", end_port, start_port);
    }
  }

  /// \brief Registers a sector and transfers ownership of its results notifier.
  ///
  /// The notifier wraps the intercepted upper physical-layer receiver notifications and is held by the backend so it
  /// can intercept trigger signals. The sector vector is resized on demand.
  ///
  /// \param sector    Sector identifier.
  /// \param notifier  Pointer to the notifier that wraps the intercepted receiver notifier. Ownership is transferred.
  void on_new_sector(unsigned sector, std::unique_ptr<upper_phy_rx_results_notifier> notifier)
  {
    if (sector + 1 >= sectors.size()) {
      sectors.resize(sector + 1);
    }
    sectors[sector].notifiers.push_back(std::move(notifier));
  }

  /// \brief Triggers writing the resource grid for a given sector and slot.
  ///
  /// Matches the requested slot against the buffered grid entry. If the slot matches, reads the antenna-port range
  /// [start_port, end_port) for every OFDM symbol in the slot, and appends the raw \c cf_t samples to the binary file.
  /// The write is offloaded to the internal task worker. A warning is logged when the task queue is full.
  ///
  /// \param sector  Sector identifier.
  /// \param slot    Slot point (SFN + slot index) identifying the resource grid to write.
  void on_grid_trigger(unsigned sector, slot_point slot)
  {
    // Skip if the sector is invalid.
    if (sector >= sectors.size()) {
      return;
    }

    // Queue trigger.
    if (not worker.push_task([this, sector, slot]() {
          // Select resource grid/slot entry.
          slot_entry<shared_resource_grid>& slot_rg =
              sectors[sector].grid_entries[slot.system_slot() % nof_slots_timeout];

          // Skip if the slot is not matched.
          if (slot_rg.slot != slot) {
            return;
          }

          // Exchange the grid with an invalid one. Skip further steps if it is invalid.
          shared_resource_grid grid = std::exchange(slot_rg.resource, {});
          if (!grid) {
            return;
          }

          // Obtain reference to the resource grid reader.
          const resource_grid_reader& rg_reader = grid->get_reader();

          // Save the resource grid.
          for (unsigned i_port = start_port; i_port != end_port; ++i_port) {
            for (unsigned symbol_idx = 0; symbol_idx != nof_symbols; ++symbol_idx) {
              rg_reader.get(temp_buffer, i_port, symbol_idx, 0);
              file.write(reinterpret_cast<const char*>(temp_buffer.data()), temp_buffer.size() * sizeof(cf_t));
            }
          }

          // Log the resource grid information.
          unsigned nof_complex_floats = temp_buffer.size() * nof_symbols * (end_port - start_port);
          logger.info(slot.sfn(),
                      slot.slot_index(),
                      "RX_SYMBOL: sector={} offset={} size={}",
                      sector,
                      file_offset,
                      nof_complex_floats);

          // Advance file offset.
          file_offset += nof_complex_floats;
        })) {
      logger.warning(
          slot.sfn(), slot.slot_index(), "RX_SYMBOL: Failed to write UL samples. Cause: task worker queue is full");
    }
  }

  /// \brief Triggers writing PRACH samples for a given sector and PRACH buffer context.
  ///
  /// Looks up the buffered PRACH buffer matching the given slot. If found, converts every PRACH symbol from \c cbf16_t
  /// to \c cf_t and appends all replicas and ports to the binary file. The write is offloaded to the internal task
  /// worker. A warning is logged when the sector is invalid, the buffer is missing, or the task queue is full.
  ///
  /// \param sector   Sector identifier.
  /// \param context  PRACH buffer context containing sector, slot, format, and PUSCH SCS.
  void on_prach_trigger(unsigned sector, const prach_buffer_context& context)
  {
    // Skip if the sector identifier is invalid.
    if (context.sector >= sectors.size()) {
      logger.warning(
          context.slot.sfn(), context.slot.slot_index(), "RX_PRACH: Failed to write PRACH samples. Invalid sector.");
      return;
    }

    // Retrieve preamble information.
    prach_preamble_information prach_info;
    if (is_long_preamble(context.format)) {
      prach_info = get_prach_preamble_long_info(context.format);
    } else {
      prach_info = get_prach_preamble_short_info(
          context.format, to_ra_subcarrier_spacing(context.pusch_scs), /*last_occasion=*/false);
    }
    unsigned nof_replicas = prach_info.nof_symbols;

    // Queue trigger.
    if (not worker.push_task([this, sector, slot = context.slot, nof_replicas]() {
          // Select resource grid/slot entry.
          slot_entry<shared_prach_buffer>& slot_rg =
              sectors[sector].prach_entries[slot.system_slot() % nof_slots_timeout];

          // Skip if the slot is not matched.
          if (slot_rg.slot != slot) {
            logger.warning(slot.sfn(), slot.slot_index(), "RX_PRACH: Failed to write PRACH samples. Buffer not found.");
            return;
          }

          // Exchange the PRACH buffer with an invalid one. Skip further steps if the buffer was already cleared.
          shared_prach_buffer prach_buff = std::exchange(slot_rg.resource, {});
          if (!prach_buff) {
            return;
          }

          // Save IQ samples for each of the PRACH ports.
          unsigned prach_start        = 0;
          unsigned prach_stop         = prach_buff->get_max_nof_ports();
          unsigned nof_complex_floats = prach_buff->get_sequence_length() * nof_replicas * (prach_stop - prach_start);
          for (unsigned i_port = prach_start; i_port != prach_stop; ++i_port) {
            for (unsigned i_replica = 0; i_replica != nof_replicas; ++i_replica) {
              // Select view of the replica.
              span<const cbf16_t> samples = prach_buff->get_symbol(i_port, 0, 0, i_replica);

              // Convert samples to complex float.
              span<cf_t> samples_cf = span<cf_t>(temp_prach_buffer).first(samples.size());
              ocuduvec::convert(samples_cf, samples);

              // Write file.
              file.write(reinterpret_cast<const char*>(samples_cf.data()), samples.size() * sizeof(cf_t));
            }
          }

          // Log the resource grid information.
          logger.info(slot.sfn(),
                      slot.slot_index(),
                      "RX_PRACH: sector={} offset={} size={}",
                      sector,
                      file_offset,
                      nof_complex_floats);

          // Advance file offset.
          file_offset += nof_complex_floats;
        })) {
      logger.warning(context.slot.sfn(),
                     context.slot.slot_index(),
                     "RX_PRACH: Failed to write PRACH samples. Cause: task worker queue is full");
    }
  }

  /// \brief Handles a received OFDM symbol and buffers the resource grid.
  ///
  /// This method is the consumer-facing entry point — it is called by the upper physical layer when the last symbol
  /// of a slot has been received. Only on the final symbol (i.e. when \c symbol equals <em>nof_symbols - 1</em> does it
  /// store a copy of the resource grid in the per-sector, per-slot ring buffer for later writing. The actual file I/O
  /// is deferred to the task worker and triggered via on_grid_trigger().
  ///
  /// \param context  Context containing sector, slot, and symbol index.
  /// \param grid     Shared resource grid for the received slot.
  void handle_rx_symbol(const upper_phy_rx_symbol_context& context, const shared_resource_grid& grid)
  {
    // Early return if the number of symbols does not reach the configured one or the file is not open.
    if ((context.symbol != (nof_symbols - 1)) || !file.is_open()) {
      return;
    }

    // Skip if the sector identifier is invalid.
    if (context.sector >= sectors.size()) {
      logger.warning(
          context.slot.sfn(), context.slot.slot_index(), "RX_SYMBOL: Failed to write UL samples. Invalid sector.");
      return;
    }

    // Queue write request.
    if (not worker.push_task([this, context, rg = grid.copy()]() mutable {
          slot_entry<shared_resource_grid>& entry =
              sectors[context.sector].grid_entries[context.slot.system_slot() % nof_slots_timeout];
          entry.slot     = context.slot;
          entry.resource = std::move(rg);
        })) {
      logger.warning(context.slot.sfn(),
                     context.slot.slot_index(),
                     "RX_SYMBOL: Failed to write UL entry. Cause: task worker queue is full");
    }
  }

  /// \brief Buffers a received PRACH window (occasion) for later file writing.
  ///
  /// Stores the PRACH buffer in the per-sector, per-slot ring buffer. The actual file I/O is deferred to the task
  /// worker and triggered via on_prach_trigger().
  ///
  /// \param context  PRACH buffer context containing sector and slot information.
  /// \param buffer   Shared PRACH buffer owned by the caller before this call.
  void handle_rx_prach_window(const prach_buffer_context& context, shared_prach_buffer buffer)
  {
    // Skip if the sector is invalid.
    if (context.sector >= sectors.size()) {
      logger.warning(
          context.slot.sfn(), context.slot.slot_index(), "RX_PRACH: Failed to keep PRACH buffer. Invalid sector.");
      return;
    }

    // Queue write request.
    if (!worker.push_task(
            [this, slot = context.slot, sector = context.sector, prach_buff = std::move(buffer)]() mutable {
              slot_entry<shared_prach_buffer>& entry =
                  sectors[sector].prach_entries[slot.system_slot() % nof_slots_timeout];
              entry.slot     = slot;
              entry.resource = std::move(prach_buff);
            })) {
      logger.warning(context.slot.sfn(),
                     context.slot.slot_index(),
                     "RX_PRACH: Failed to save PRACH entry. Cause: task worker queue is full");
    }
  }

private:
  /// Asynchronous dedicated worker name.
  static constexpr const char* async_worker_name = "rx_symb_print";
  /// Asynchronous dedicated worker queue size.
  static constexpr size_t async_worker_queue_size = 128;
  /// Expected OFDM symbol count per slot (\p MAX_NSYMB_PER_SLOT).
  static constexpr unsigned nof_symbols = MAX_NSYMB_PER_SLOT;
  /// \brief Number of slot entries retained in the per-sector ring buffers.
  ///
  /// Provides a 5-slot history window so that trigger requests arriving slightly out of order can still find their
  /// buffered data.
  static constexpr unsigned nof_slots_timeout = 5;

  /// \brief Stores a slot-identifying key alongside its associated resource (grid or PRACH buffer).
  template <typename ResourceType>
  struct slot_entry {
    slot_point   slot;
    ResourceType resource;
  };

  /// Per-sector state: notifier list and slot-indexed resource buffers.
  struct sector_repository {
    std::vector<std::unique_ptr<upper_phy_rx_results_notifier>>     notifiers;
    std::array<slot_entry<shared_prach_buffer>, nof_slots_timeout>  prach_entries;
    std::array<slot_entry<shared_resource_grid>, nof_slots_timeout> grid_entries;
  };

  /// Current byte offset in the output binary file (used for logging).
  size_t file_offset = 0;
  /// Logger instance.
  ocudulog::basic_logger& logger;
  /// Binary output file stream.
  std::ofstream file;
  /// Background task worker (4 threads) for deferred I/O.
  task_worker worker;
  /// Scratch buffer: one resource block × subcarriers in complex-float format.
  std::vector<cf_t> temp_buffer;
  /// Scratch buffer: PRACH long-sequence length in complex-float format.
  std::vector<cf_t> temp_prach_buffer;
  /// Per-sector state (notifiers, grid, PRACH ring buffers).
  std::vector<sector_repository> sectors;
  /// First antenna port to dump (inclusive).
  unsigned start_port;
  /// Last antenna port to dump (exclusive).
  unsigned end_port;
};

} // namespace ocudu
