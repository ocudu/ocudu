// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocudulog/ocudulog.h"

using namespace ocudulog;

static constexpr unsigned num_iterations = 4000000;

int main()
{
  auto& s       = ocudulog::fetch_file_sink("ocudulog_latency_benchmark.txt");
  auto& channel = ocudulog::fetch_log_channel("bench", s, {});

  ocudulog::init(ocudulog::backend_priority::high);

  auto begin = std::chrono::steady_clock::now();
  for (unsigned i = 0; i != num_iterations; ++i) {
    double d = i;
    channel("OCUDULOG backend benchmark: int: {}, double: {}", i, d);
  }
  ocudulog::flush();
  auto end = std::chrono::steady_clock::now();

  fmt::print("OCUDULOG Backend throughput Benchmark:\n"
             "  benchmark run for {} ms and logged {} times\n"
             "  {:.1f} million logs/sec",
             std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count(),
             num_iterations,
             num_iterations / std::chrono::duration_cast<std::chrono::duration<double>>(end - begin).count() / 1e6);

  return 0;
}
