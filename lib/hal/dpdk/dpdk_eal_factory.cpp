// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/hal/dpdk/dpdk_eal_factory.h"
#include "dpdk.h"
#include <sstream>
#include <vector>

using namespace ocudu;
using namespace dpdk;

/// Splits the input string into a vector of substrings separated by space characters.
static std::vector<std::string> split_string_by_space(const std::string& input)
{
  std::vector<std::string> strings;

  std::istringstream ss(input);
  while (ss.good()) {
    std::string substr;
    std::getline(ss, substr, ' ');

    if (!substr.empty()) {
      strings.push_back(std::move(substr));
    }
  }

  return strings;
}

std::unique_ptr<dpdk_eal>
ocudu::dpdk::create_dpdk_eal(const std::string& args, ocudulog::basic_logger& logger, bool enable_pdump_init)
{
  auto               strings = split_string_by_space(args);
  std::vector<char*> argv;
  for (const auto& s : strings) {
    argv.push_back(const_cast<char*>(s.c_str()));
  }

  // EAL initialization.
  if (!::eal_init(argv.size(), argv.data(), logger)) {
    return nullptr;
  }

#if !defined(DPDK_PDUMP_AVAILABLE)
  if (enable_pdump_init) {
    logger.warning("dpdk: pdump support was requested by the configuration, but it is not supported by this build as "
                   "the DPDK pdump library could not be resolved; ignoring the request");
  }
#endif

  return std::make_unique<dpdk_eal>(logger, enable_pdump_init);
}
