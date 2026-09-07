// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/scheduler/trace/trace_to_log.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/error_handling.h"
#include "CLI/CLI11.hpp"
#include <cstdlib>
#include <fstream>
#include <string>

using namespace ocudu;
using namespace schedtrace;

int main(int argc, char* argv[])
{
  CLI::App app{"schedlog: convert a schedtrace binary file to a human-readable log"};

  std::string input_path;
  std::string output_path = "stdout";
  app.add_option("-i", input_path, "Input schedtrace binary file")->required();
  app.add_option("-o", output_path, "Output text log file")->capture_default_str();

  CLI11_PARSE(app, argc, argv);

  // Open input binary file.
  std::ifstream f(input_path, std::ios::binary);
  if (not f.is_open()) {
    fmt::print(stderr, "Failed to open input file: {}\n", input_path);
    return EXIT_FAILURE;
  }

  // Set up logging to the output file.
  ocudulog::sink* log_sink =
      (output_path == "stdout") ? ocudulog::create_stdout_sink() : ocudulog::create_file_sink(output_path);
  if (log_sink == nullptr) {
    report_error("Could not create application log sink.\n");
  }
  ocudulog::set_default_sink(*log_sink);
  ocudulog::init();
  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("SCHED", true);
  logger.set_level(ocudulog::basic_levels::debug);
  logger.set_context(0, 0);

  // Read bin file and produce log file.
  if (not trace_to_log(f, logger)) {
    return EXIT_FAILURE;
  }

  ocudulog::flush();
  return EXIT_SUCCESS;
}
