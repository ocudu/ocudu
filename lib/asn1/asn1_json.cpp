// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief JSON writer.

#include "ocudu/asn1/asn1_json.h"
#include <cstring>
#include <iterator>

namespace asn1 {

/*******************
    JsonWriter
*******************/

json_writer::json_writer() : ident(""), sep(NONE) {}

void json_writer::write_fieldname(const char* fieldname)
{
  static constexpr const char* septable[] = {",\n", "\n", ""};

  fmt::format_to(std::back_inserter(buffer), "{}{}", septable[sep], sep != NONE ? ident : "");
  if (strlen(fieldname) != 0) {
    fmt::format_to(std::back_inserter(buffer), "\"{}\": ", fieldname);
  }
  sep = NONE;
}

void json_writer::write_str(const char* fieldname, const char* value)
{
  write_fieldname(fieldname);
  fmt::format_to(std::back_inserter(buffer), "\"{}\"", value);
  sep = COMMA;
}

void json_writer::write_str(const char* fieldname, const std::string& value)
{
  write_str(fieldname, value.c_str());
}

void json_writer::write_str(const char* value)
{
  write_str("", value);
}

void json_writer::write_str(const std::string& value)
{
  write_str("", value);
}

void json_writer::write_int(const char* fieldname, int64_t value)
{
  write_fieldname(fieldname);
  fmt::format_to(std::back_inserter(buffer), "{}", value);
  sep = COMMA;
}
void json_writer::write_int(int64_t value)
{
  write_int("", value);
}
void json_writer::write_uint(const char* fieldname, uint64_t value)
{
  write_fieldname(fieldname);
  fmt::format_to(std::back_inserter(buffer), "{}", value);
  sep = COMMA;
}
void json_writer::write_uint(uint64_t value)
{
  write_uint("", value);
}

void json_writer::write_bool(const char* fieldname, bool value)
{
  write_fieldname(fieldname);
  fmt::format_to(std::back_inserter(buffer), "{}", value ? "true" : "false");
  sep = COMMA;
}
void json_writer::write_bool(bool value)
{
  write_bool("", value);
}

void json_writer::write_float(const char* fieldname, float value)
{
  write_fieldname(fieldname);
  fmt::format_to(std::back_inserter(buffer), "{}", value);
  sep = COMMA;
}

void json_writer::write_float(float value)
{
  write_float("", value);
}
void json_writer::write_null(const char* fieldname)
{
  write_fieldname(fieldname);
  fmt::format_to(std::back_inserter(buffer), "null");
  sep = COMMA;
}
void json_writer::write_null()
{
  write_null("");
}

void json_writer::start_obj(const char* fieldname)
{
  write_fieldname(fieldname);
  fmt::format_to(std::back_inserter(buffer), "{{");
  ident += "  ";
  sep = NEWLINE;
}
void json_writer::end_obj()
{
  ident.erase(ident.size() - 2, 2);
  fmt::format_to(std::back_inserter(buffer), "\n{}}}", ident);
  sep = COMMA;
}

void json_writer::start_array(const char* fieldname)
{
  write_fieldname(fieldname);
  fmt::format_to(std::back_inserter(buffer), "[");
  ident += "  ";
  sep = NEWLINE;
}
void json_writer::end_array()
{
  ident.erase(ident.size() - 2, 2);
  fmt::format_to(std::back_inserter(buffer), "\n{}]", ident);
  sep = COMMA;
}

std::string json_writer::to_string() const
{
  return std::string(buffer.data(), buffer.size());
}

} // namespace asn1
