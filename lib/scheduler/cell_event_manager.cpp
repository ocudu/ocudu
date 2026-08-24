// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_event_manager.h"
#include "common_scheduling/paging_scheduler.h"
#include "config/cell_configuration.h"
#include "ocudu/adt/mpmc_queue.h"
#include "ocudu/adt/unique_function.h"
#include "ocudu/support/memory_pool/bounded_object_pool.h"

using namespace ocudu;

/// \brief Queue of the events pending to be processed by a cell, and pools of their payloads.
///
/// Events are pushed from any executor and dispatched in the cell scheduler executor.
class ocudu::cell_event_dispatcher
{
  /// \brief Capacity of the queue of pending events.
  ///
  /// [Implementation defined] Sized for the events that can accumulate over the slots elapsed between two slot
  /// indications of this cell.
  static constexpr size_t EVENT_QUEUE_SIZE = 128;

  // [Implementation defined] Number of paging requests that can be in flight at any moment.
  static constexpr size_t PAGING_POOL_SIZE = 128;

  using paging_pool = bounded_object_pool<sched_paging_information>;

  /// Event enqueued and dispatched by this class.
  struct event_t {
    static constexpr size_t callback_capacity = 48;
    using callback_type                       = unique_function<void(), callback_capacity, true>;

    callback_type callback;
    const char*   ev_name = "invalid";

    event_t() = default;
    template <typename Callable>
    event_t(const char* ev_name_, Callable&& callable) : callback(std::forward<Callable>(callable)), ev_name(ev_name_)
    {
    }
  };

  using event_queue = concurrent_queue<event_t, concurrent_queue_policy::lockfree_mpmc>;

public:
  cell_event_dispatcher(const cell_configuration& cell_cfg_, ocudulog::basic_logger& logger_) :
    cell_cfg(cell_cfg_), logger(logger_), pending_pagings(PAGING_POOL_SIZE), pending_events(EVENT_QUEUE_SIZE)
  {
  }

  /// Allow events to be pushed.
  void start() { active.store(true, std::memory_order_release); }

  /// Stop accepting events and discard the pending ones.
  void stop()
  {
    active.store(false, std::memory_order_release);

    event_t ev;
    while (pending_events.try_pop(ev)) {
    }
  }

  /// Dispatch the pending events.
  void run()
  {
    event_t ev;
    while (pending_events.try_pop(ev)) {
      ev.callback();
    }
  }

  /// \brief Create an event payload managed by an object pool.
  /// \return \c nullptr if the respective pool is exhausted.
  template <typename PDUType>
  auto create_pdu(const PDUType& pdu)
  {
    auto* pool = std::get<bounded_object_pool<std::decay_t<PDUType>>*>(pools);
    auto  ret  = pool->get();
    if (ret != nullptr) {
      *ret = pdu;
    } else {
      logger.warning("cell={}: Discarding {} event. Cause: Payload pool is empty",
                     fmt::underlying(cell_cfg.cell_index),
                     pdu_type_name<std::decay_t<PDUType>>());
    }
    return ret;
  }

  /// Enqueue an event to be dispatched in the next slot indication of this cell.
  template <typename Callable>
  void push(const char* ev_name, Callable&& callable)
  {
    if (OCUDU_UNLIKELY(not active.load(std::memory_order_acquire))) {
      logger.warning(
          "cell={}: Discarding {} event. Cause: Cell is not active", fmt::underlying(cell_cfg.cell_index), ev_name);
      return;
    }

    if (not pending_events.try_push(event_t{ev_name, std::forward<Callable>(callable)})) {
      logger.warning(
          "cell={}: Discarding {} event. Cause: Event queue is full", fmt::underlying(cell_cfg.cell_index), ev_name);
    }
  }

private:
  /// Returns a human-readable name for an event payload type.
  template <typename PDUType>
  static constexpr const char* pdu_type_name()
  {
    if constexpr (std::is_same_v<PDUType, sched_paging_information>) {
      return "paging";
    } else {
      return "unknown";
    }
  }

  const cell_configuration& cell_cfg;
  ocudulog::basic_logger&   logger;

  paging_pool pending_pagings;

  std::tuple<paging_pool*> pools{&pending_pagings};

  event_queue pending_events;

  // Whether the cell is currently accepting events.
  std::atomic<bool> active{true};
};

cell_event_manager::cell_event_manager(const cell_configuration& cell_cfg,
                                       paging_scheduler&         pg_sch_,
                                       ocudulog::basic_logger&   logger) :
  pg_sch(pg_sch_), dispatcher(std::make_unique<cell_event_dispatcher>(cell_cfg, logger))
{
}

cell_event_manager::~cell_event_manager() = default;

void cell_event_manager::start()
{
  dispatcher->start();
}

void cell_event_manager::stop()
{
  dispatcher->stop();
}

void cell_event_manager::run_slot()
{
  dispatcher->run();
}

void cell_event_manager::handle_paging_information(const sched_paging_information& pi)
{
  auto pi_ptr = dispatcher->create_pdu(pi);
  if (pi_ptr == nullptr) {
    return;
  }

  dispatcher->push("paging", [this, pi_ptr = std::move(pi_ptr)]() { pg_sch.handle_paging_information(*pi_ptr); });
}
