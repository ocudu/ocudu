// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/complex.h"
#include "ocudu/adt/format.h"
#include "ocudu/adt/span.h"
#include <array>
#include <gtest/gtest.h>

using namespace ocudu;

TEST(span_formatter_test, int_comma)
{
  std::array<int, 5> data = {0, 1, 2, 3, 4};
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer), "{:n}", span<int>(data));

  std::string formatted_string = to_string(buffer);
  std::string expected_string  = "0, 1, 2, 3, 4";
  ASSERT_EQ(formatted_string, expected_string);
  ASSERT_EQ(span<int>(data), span<int>(data));
}

TEST(span_formatter_test, u8_dec)
{
  std::array<uint8_t, 5> data = {0, 1, 2, 3, 4};
  fmt::memory_buffer     buffer;
  fmt::format_to(std::back_inserter(buffer), "{}", span<uint8_t>(data));

  std::string formatted_string = to_string(buffer);
  std::string expected_string  = "[0, 1, 2, 3, 4]";
  ASSERT_EQ(formatted_string, expected_string);
  ASSERT_EQ(span<uint8_t>(data), span<uint8_t>(data));
}

TEST(span_formatter_test, u8_hex)
{
  std::array<uint8_t, 5> data = {0, 1, 2, 3, 4};
  fmt::memory_buffer     buffer;
  fmt::format_to(std::back_inserter(buffer), "{::0>2x}", span<uint8_t>(data));

  std::string formatted_string = to_string(buffer);
  std::string expected_string  = "[00, 01, 02, 03, 04]";
  ASSERT_EQ(formatted_string, expected_string);
  ASSERT_EQ(span<uint8_t>(data), span<uint8_t>(data));
}

TEST(span_formatter_test, cf_long)
{
  using namespace std::complex_literals;

  std::array<cf_t, 5> data = {0.0if, 1.0if, 2.0if, 3.0if, 4.0if};
  fmt::memory_buffer  buffer;
  fmt::format_to(std::back_inserter(buffer), "{}", span<cf_t>(data));

  std::string formatted_string = to_string(buffer);
  std::string expected_string  = "[+0.000000+0.000000j, +0.000000+1.000000j, +0.000000+2.000000j, "
                                 "+0.000000+3.000000j, +0.000000+4.000000j]";
  ASSERT_EQ(formatted_string, expected_string);
  ASSERT_EQ(span<cf_t>(data), span<cf_t>(data));
}

TEST(span_formatter_test, cf_short)
{
  using namespace std::complex_literals;

  std::array<cf_t, 5> data = {0.0if, 1.0if, 2.0if, 3.0if, 4.0if};
  fmt::memory_buffer  buffer;
  fmt::format_to(std::back_inserter(buffer), "{::+.1f}", span<cf_t>(data));

  std::string formatted_string = to_string(buffer);
  std::string expected_string  = "[+0.0+0.0j, +0.0+1.0j, +0.0+2.0j, +0.0+3.0j, +0.0+4.0j]";
  ASSERT_EQ(formatted_string, expected_string);
  ASSERT_EQ(span<cf_t>(data), span<cf_t>(data));
}
