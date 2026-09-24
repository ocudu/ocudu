// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ofh_data_flow_uplane_decoding_metrics_collector.h"
#include "ocudu/ofh/receiver/ofh_receiver_metrics.h"

namespace ocudu {
namespace ofh {

/// Open Fronthaul message receiver metrics collector.
class message_receiver_metrics_collector
{
public:
  message_receiver_metrics_collector(bool                                          is_enabled_,
                                     data_flow_message_decoding_metrics_collector& df_uplink_metrics_collector_,
                                     data_flow_message_decoding_metrics_collector& df_prach_metrics_collector_) :
    is_enabled(is_enabled_),
    df_uplink_metrics_collector(df_uplink_metrics_collector_),
    df_prach_metrics_collector(df_prach_metrics_collector_)
  {
  }

  /// Returns true if the metrics collector is enabled, false otherwise.
  bool enabled() const { return is_enabled; }

  /// Updates the PRACH message decoding statistics given the execution time taken by processing a received message.
  void update_prach_stats(std::chrono::nanoseconds exec_latency)
  {
    df_prach_metrics_collector.update_stats(exec_latency);
  }

  /// Updates the uplink data message decoding statistics given the execution time taken by processing a received
  /// message.
  void update_uplink_stats(std::chrono::nanoseconds exec_latency)
  {
    df_uplink_metrics_collector.update_stats(exec_latency);
  }

  /// Increases by the given value the number of messages with a sequence identifier from the future.
  void update_future_seq_id_messages(unsigned value)
  {
    nof_future_seq_id_messages.fetch_add(value, std::memory_order_relaxed);
  }

  /// Increases the number of messages with a sequence identifier from the past by one.
  void increase_past_seq_id_messages() { nof_past_seq_id_messages.fetch_add(1, std::memory_order_relaxed); }

  /// Increases the number of corrupted messages detected by the eCPRI decoder.
  void increase_ecpri_corrupted_messages() { ecpri_nof_corrupted_messages.fetch_add(1, std::memory_order_relaxed); }

  /// Collects message receiver performance metrics.
  void collect_metrics(message_decoding_performance_metrics& metrics)
  {
    if (!enabled()) {
      return;
    }

    df_uplink_metrics_collector.collect_metrics(metrics.data_processing_metrics);
    df_prach_metrics_collector.collect_metrics(metrics.prach_processing_metrics);
    metrics.ecpri_metrics.nof_past_seq_id_messages = nof_past_seq_id_messages.exchange(0, std::memory_order_relaxed);
    metrics.ecpri_metrics.nof_future_seq_id_messages =
        nof_future_seq_id_messages.exchange(0, std::memory_order_relaxed);
    metrics.ecpri_metrics.nof_corrupted_messages = ecpri_nof_corrupted_messages.exchange(0, std::memory_order_relaxed);
  }

private:
  const bool                                    is_enabled;
  data_flow_message_decoding_metrics_collector& df_uplink_metrics_collector;
  data_flow_message_decoding_metrics_collector& df_prach_metrics_collector;
  /// \brief Number of sequence IDs skipped when received OFH messages jump ahead of the expected sequence ID.
  ///
  /// Messages delivered out of order also skip sequence identifiers - those might be received later on. Thus the number
  /// of messages lost by the RU/Transport can be obtained by subtracting the number of messages with a sequence
  /// identifier from the past, see \c nof_past_seq_id_messages.
  std::atomic<uint64_t> nof_future_seq_id_messages = {0};
  /// \brief Number of received OFH messages with a sequence identifier from the past.
  ///
  /// A message carries a sequence identifier from the past when its value is lower than the expected one, which
  /// happens when the message is delivered out of order or duplicated.
  std::atomic<uint64_t> nof_past_seq_id_messages = {0};
  /// Number of corrupted messages detected by the eCPRI decoder.
  std::atomic<uint64_t> ecpri_nof_corrupted_messages = {0};
};

} // namespace ofh
} // namespace ocudu
