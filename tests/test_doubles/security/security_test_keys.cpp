// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "security_test_keys.h"
#include "ocudu/adt/byte_buffer.h"
#include <algorithm>

using namespace ocudu;

security::sec_key ocudu::test_helpers::make_sec_key(const std::string& hex_str)
{
  byte_buffer       key_buf = make_byte_buffer(hex_str).value();
  security::sec_key key     = {};
  std::copy(key_buf.begin(), key_buf.end(), key.begin());
  return key;
}

security::sec_128_key ocudu::test_helpers::make_sec_128_key(const std::string& hex_str)
{
  byte_buffer           key_buf = make_byte_buffer(hex_str).value();
  security::sec_128_key key     = {};
  std::copy(key_buf.begin(), key_buf.end(), key.begin());
  return key;
}
