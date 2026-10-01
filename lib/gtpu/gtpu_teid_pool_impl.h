// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gtpu/gtpu_teid_pool.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/timers.h"
#include <mutex>

namespace ocudu {

struct teid_pool_item {
  bool         is_allocated = false;
  tick_point_t linger_timestamp{0};
};

class gtpu_teid_pool_impl final : public gtpu_teid_pool
{
public:
  explicit gtpu_teid_pool_impl(uint32_t       max_nof_teids_,
                               timer_duration teid_release_linger_time_,
                               timer_manager& timers_) :
    max_nof_teids(max_nof_teids_),
    teid_pool(max_nof_teids_),
    teid_release_linger_time(teid_release_linger_time_),
    timers(timers_),
    logger(ocudulog::fetch_basic_logger("GTPU"))
  {
  }

  [[nodiscard]] expected<gtpu_teid_t> request_teid() override
  {
    expected<gtpu_teid_t> teid = make_unexpected(default_error_t{});

    std::lock_guard<std::mutex> guard(pool_mutex);
    if (full_no_lock()) {
      return teid;
    }

    // Find a free teid
    bool     found   = false;
    uint16_t tmp_idx = next_teid_idx;
    for (uint16_t n = 0; n < max_nof_teids; n++) {
      tmp_idx = (next_teid_idx + n) % max_nof_teids;
      if (not teid_pool[tmp_idx].is_allocated) {
        teid_pool[tmp_idx].is_allocated = true;
        found                           = true;
        break;
      }
    }

    if (not found) {
      return teid;
    }
    next_teid_idx = (tmp_idx + 1) % max_nof_teids;
    teid          = gtpu_teid_t{tmp_idx + GTPU_TEID_MIN.value()};
    nof_teids++;
    if (is_teid_lingering_no_lock(teid_pool[tmp_idx])) {
      logger.warning("Reallocated lingering TEID. Consider increasing TEID pool. teid={} linger_time={}ms",
                     teid.value(),
                     teid_release_linger_time.count());
    }
    return teid;
  }

  [[nodiscard]] bool release_teid(gtpu_teid_t teid) override
  {
    std::lock_guard<std::mutex> guard(pool_mutex);
    uint32_t                    teid_idx = teid.value() - GTPU_TEID_MIN.value();
    if (not teid_pool[teid_idx].is_allocated) {
      logger.error("Trying to free non-allocated TEID. teid={}", teid);
      return false;
    }
    teid_pool[teid_idx].is_allocated     = false;
    teid_pool[teid_idx].linger_timestamp = timers.now() + teid_release_linger_time.count();
    nof_teids--;
    return true;
  }

  bool is_teid_lingering(gtpu_teid_t teid) override
  {
    std::lock_guard<std::mutex> guard(pool_mutex);
    uint32_t                    teid_idx = teid.value() - GTPU_TEID_MIN.value();
    return is_teid_lingering_no_lock(teid_pool[teid_idx]);
  }

  [[nodiscard]] bool full() override
  {
    std::lock_guard<std::mutex> guard(pool_mutex);
    return full_no_lock();
  }

  uint32_t get_max_nof_teids() override { return max_nof_teids; }

private:
  [[nodiscard]] bool full_no_lock() const { return nof_teids >= max_nof_teids; }

  bool is_teid_lingering_no_lock(const teid_pool_item teid_item) { return timers.now() < teid_item.linger_timestamp; }

  uint32_t       next_teid_idx = 0;
  uint32_t       nof_teids     = 0;
  const uint32_t max_nof_teids;

  std::mutex                  pool_mutex;
  std::vector<teid_pool_item> teid_pool;
  timer_duration              teid_release_linger_time;
  timer_manager&              timers;

  ocudulog::basic_logger& logger;
};
} // namespace ocudu
