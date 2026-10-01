// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "lib/ocudulog/sinks/syslog_sink.h"
#include "test_dummies.h"
#include "testing_helpers.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <fstream>
#include <sstream>

using namespace ocudulog;

static bool find_string_infile(std::string filename, std::string pattern)
{
  std::ifstream file(filename);
  std::string   line;
  bool          found = false;

  if (file.is_open()) {
    while (std::getline(file, line)) {
      if (line.find(pattern) != std::string::npos) { // WILL SEARCH 2015-1113 in file
        found = true;
      }
    }
  } else {
    printf("WARNING: Could not open file %s", filename.c_str());
  }
  return found;
}

static bool syslog_basic_test()
{
  syslog_sink syslog_sink(get_default_log_formatter());

  // Build a 1000 byte entry.
  std::string entry(1000, 'a');

  syslog_sink.write(detail::memory_buffer(entry));

  syslog_sink.flush();

  ASSERT_EQ(find_string_infile("/var/log/syslog", entry), true);
  return true;
}

int main()
{
  TEST_FUNCTION(syslog_basic_test);
  return 0;
}
