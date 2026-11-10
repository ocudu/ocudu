// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/gateways/udp_network_gateway_factory.h"
#include <algorithm>
#include <arpa/inet.h>
#include <thread>

namespace ocudu {

/// Submission deadlines for a nonzero target packet rate.
class udp_benchmark_pacer
{
public:
  udp_benchmark_pacer(unsigned rate_, std::chrono::steady_clock::time_point start) : rate(rate_), deadline(start) {}

  std::chrono::steady_clock::time_point next_deadline(unsigned nof_pdus, std::chrono::steady_clock::time_point now)
  {
    const auto interval = std::chrono::duration<double>(static_cast<double>(nof_pdus) / rate);
    // Discard overdue work rather than replaying missed bursts.
    deadline = std::max(deadline + std::chrono::duration_cast<std::chrono::steady_clock::duration>(interval), now);
    return deadline;
  }

private:
  unsigned                              rate;
  std::chrono::steady_clock::time_point deadline;
};

inline void send_udp_benchmark_pdus(udp_network_gateway&    gateway,
                                    const byte_buffer&      pdu,
                                    const sockaddr_storage& dest,
                                    uint64_t                nof_pdus,
                                    unsigned                rate,
                                    unsigned                burst_size)
{
  if (burst_size == 0) {
    report_error("Burst size must be greater than zero");
  }
  if (rate == 0) {
    for (uint64_t i = 0; i != nof_pdus; ++i) {
      gateway.handle_pdu(pdu.copy(), dest);
    }
    return;
  }
  udp_benchmark_pacer pacer{rate, std::chrono::steady_clock::now()};
  while (nof_pdus != 0) {
    const unsigned count = std::min<uint64_t>(burst_size, nof_pdus);
    for (unsigned i = 0; i != count; ++i) {
      gateway.handle_pdu(pdu.copy(), dest);
    }
    nof_pdus -= count;
    // Include the final burst's interval in the measured offered-load window.
    std::this_thread::sleep_until(pacer.next_deadline(count, std::chrono::steady_clock::now()));
  }
}

class dummy_network_gateway_data_notifier_with_src_addr : public network_gateway_data_notifier_with_src_addr
{
public:
  dummy_network_gateway_data_notifier_with_src_addr(unsigned slow_inter_rx_us_, uint64_t max_pdus_) :
    slow_inter_rx_us(slow_inter_rx_us_), max_pdus(max_pdus_)
  {
  }

  void on_new_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) override
  {
    if (n_pdus >= max_pdus) {
      // test is finished stop counting
      return;
    }
    rx_bytes += pdu.length();
    n_pdus++;

    auto t_now = std::chrono::steady_clock::now();
    if (!first) {
      auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t_now - t_last);
      if (duration < t_min) {
        t_min = duration;
      }
      if (duration > t_max) {
        t_max = duration;
      }
      t_sum += duration;
      if (duration.count() > slow_inter_rx_us) {
        ++nof_slow_intervals;
      }
    } else {
      first   = false;
      t_start = t_now;
    }

    t_last = t_now;
    if (n_pdus == max_pdus) {
      t_end = std::chrono::steady_clock::now();
    }
  }

  uint64_t get_rx_bytes() const { return rx_bytes; }
  uint64_t get_n_pdus() const { return n_pdus; }
  uint64_t get_nof_slow_intervals() const { return nof_slow_intervals; }

  std::chrono::microseconds get_t_min() { return t_min; }
  std::chrono::microseconds get_t_max() { return t_max; }
  std::chrono::microseconds get_t_sum() { return t_sum; }
  std::chrono::microseconds get_t_rx()
  {
    if (n_pdus < 2) {
      return std::chrono::microseconds::zero();
    }
    if (n_pdus >= max_pdus) {
      return std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start);
    }
    return std::chrono::duration_cast<std::chrono::microseconds>(t_last - t_start);
  }

private:
  unsigned slow_inter_rx_us;
  bool     first = true;

  uint64_t rx_bytes = 0;
  uint64_t n_pdus   = 0;
  uint64_t max_pdus;
  uint64_t nof_slow_intervals = 0;

  std::chrono::steady_clock::time_point t_last = std::chrono::steady_clock::now();
  std::chrono::microseconds             t_min  = std::chrono::microseconds::max();
  std::chrono::microseconds             t_max  = std::chrono::microseconds::min();
  std::chrono::microseconds             t_sum  = std::chrono::microseconds::zero();

  std::chrono::steady_clock::time_point t_start;
  std::chrono::steady_clock::time_point t_end;
};

inline byte_buffer make_tx_byte_buffer(uint32_t length)
{
  byte_buffer pdu{};
  for (uint32_t i = 0; i < length; ++i) {
    if (not pdu.append((uint8_t)i)) {
      pdu.clear();
      break;
    }
  }
  return pdu;
}

inline sockaddr_storage to_sockaddr_storage(const std::string& dest_addr, uint16_t port)
{
  in_addr          inaddr_v4    = {};
  in6_addr         inaddr_v6    = {};
  sockaddr_storage addr_storage = {};

  if (inet_pton(AF_INET, dest_addr.c_str(), &inaddr_v4) == 1) {
    sockaddr_in* tmp = (sockaddr_in*)&addr_storage;
    tmp->sin_family  = AF_INET;
    tmp->sin_addr    = inaddr_v4;
    tmp->sin_port    = htons(port);
  } else if (inet_pton(AF_INET6, dest_addr.c_str(), &inaddr_v6) == 1) {
    sockaddr_in6* tmp = (sockaddr_in6*)&addr_storage;
    tmp->sin6_family  = AF_INET6;
    tmp->sin6_addr    = inaddr_v6;
    tmp->sin6_port    = htons(port);
  }
  return addr_storage;
}

} // namespace ocudu
