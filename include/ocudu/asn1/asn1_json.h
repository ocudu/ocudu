// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief JSON writer of the ASN.1 types.

#pragma once

#include "fmt/format.h"
#include <cstdint>
#include <string>

namespace asn1 {

using json_buffer = fmt::basic_memory_buffer<char, 2048>;

class json_writer
{
public:
  json_writer();
  void        write_fieldname(const char* fieldname);
  void        write_str(const char* fieldname, const std::string& value);
  void        write_str(const char* fieldname, const char* value);
  void        write_str(const char* value);
  void        write_str(const std::string& value);
  void        write_int(const char* fieldname, int64_t value);
  void        write_int(int64_t value);
  void        write_uint(const char* fieldname, uint64_t value);
  void        write_uint(uint64_t value);
  void        write_bool(const char* fieldname, bool value);
  void        write_bool(bool value);
  void        write_float(const char* fieldname, float value);
  void        write_float(float value);
  void        write_null(const char* fieldname);
  void        write_null();
  void        start_obj(const char* fieldname = "");
  void        end_obj();
  void        start_array(const char* fieldname = "");
  void        end_array();
  std::string to_string() const;

private:
  json_buffer buffer;
  std::string ident;
  enum separator_t { COMMA = 0, NEWLINE, NONE };
  separator_t sep;
};

} // namespace asn1
