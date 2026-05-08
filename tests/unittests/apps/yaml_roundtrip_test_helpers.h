// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Shared schema-agnostic helpers for the YAML round-trip tests.
///
/// The tests are split per-app (one translation unit each) because some app config struct names collide
/// across apps (e.g., the gnb monolithic config defines its own internal `cu_up_appconfig` distinct from
/// the standalone CU-UP one), and including both leads to a redefinition.

#pragma once

#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <optional>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <yaml-cpp/yaml.h>

namespace ocudu::yaml_roundtrip_test {

// CLI11 accepts several string forms for booleans (true/false, enable/disable, yes/no, on/off, 1/0).
// The writer emits the canonical "true"/"false" form. Treat these as equivalent during leaf comparison so
// the test focuses on semantic preservation rather than string form.
inline std::optional<bool> as_bool_form(const std::string& s)
{
  if (s == "true" || s == "True" || s == "enable" || s == "yes" || s == "on" || s == "1") {
    return true;
  }
  if (s == "false" || s == "False" || s == "disable" || s == "no" || s == "off" || s == "0") {
    return false;
  }
  return std::nullopt;
}

inline std::optional<double> as_number(const std::string& s)
{
  if (s.empty()) {
    return std::nullopt;
  }
  try {
    std::size_t pos = 0;
    double      v   = std::stod(s, &pos);
    if (pos != s.size()) {
      return std::nullopt;
    }
    return v;
  } catch (...) {
    return std::nullopt;
  }
}

inline bool scalars_equivalent(const std::string& a, const std::string& b)
{
  if (a == b) {
    return true;
  }
  auto ba = as_bool_form(a);
  auto bb = as_bool_form(b);
  if (ba.has_value() && bb.has_value() && *ba == *bb) {
    return true;
  }
  // yaml-cpp elides trailing zeros when emitting floats with integer values (e.g. "1.0" -> "1"). Treat
  // numeric forms as equivalent when their floating-point values match within a small epsilon.
  auto na = as_number(a);
  auto nb = as_number(b);
  if (na.has_value() && nb.has_value()) {
    const double diff  = std::fabs(*na - *nb);
    const double scale = std::max({1.0, std::fabs(*na), std::fabs(*nb)});
    return diff < 1e-6 * scale;
  }
  return false;
}

inline std::string path_join(const std::string& base, const std::string& part)
{
  if (base.empty()) {
    return part;
  }
  if (!part.empty() && part.front() == '[') {
    return base + part;
  }
  return base + "." + part;
}

inline bool yaml_equal(const YAML::Node& a, const YAML::Node& b)
{
  if (a.Type() != b.Type()) {
    return false;
  }
  switch (a.Type()) {
    case YAML::NodeType::Null:
      return true;
    case YAML::NodeType::Scalar:
      return scalars_equivalent(a.Scalar(), b.Scalar());
    case YAML::NodeType::Sequence: {
      if (a.size() != b.size()) {
        return false;
      }
      for (std::size_t i = 0; i != a.size(); ++i) {
        if (!yaml_equal(a[i], b[i])) {
          return false;
        }
      }
      return true;
    }
    case YAML::NodeType::Map: {
      if (a.size() != b.size()) {
        return false;
      }
      for (const auto& kv : a) {
        const std::string key = kv.first.Scalar();
        if (!b[key]) {
          return false;
        }
        if (!yaml_equal(kv.second, b[key])) {
          return false;
        }
      }
      return true;
    }
    case YAML::NodeType::Undefined:
    default:
      return false;
  }
}

inline void
leaf_diff_impl(const YAML::Node& a, const YAML::Node& b, const std::string& path, std::vector<std::string>& out)
{
  if (!a.IsDefined()) {
    return;
  }
  if (!b.IsDefined()) {
    out.push_back(path + "  (missing in output)");
    return;
  }
  if (a.IsScalar()) {
    // CLI11 accepts a single scalar where a list is expected, and the writer normalises to a list. Treat
    // a scalar A and a 1-element sequence B with the same value as equivalent to keep the test focused on
    // semantic preservation rather than YAML representation.
    if (b.IsSequence() && b.size() == 1 && b[0].IsScalar() && scalars_equivalent(b[0].Scalar(), a.Scalar())) {
      return;
    }
    if (!b.IsScalar() || !scalars_equivalent(a.Scalar(), b.Scalar())) {
      std::ostringstream os;
      os << path << "  (input=" << a.Scalar() << ", output=";
      if (b.IsScalar()) {
        os << b.Scalar();
      } else {
        os << "<non-scalar>";
      }
      os << ")";
      out.push_back(os.str());
    }
    return;
  }
  if (a.IsSequence()) {
    if (!b.IsSequence()) {
      out.push_back(path + "  (input is sequence, output is not)");
      return;
    }
    const std::size_t n = std::min(a.size(), b.size());
    for (std::size_t i = 0; i != n; ++i) {
      leaf_diff_impl(a[i], b[i], path_join(path, "[" + std::to_string(i) + "]"), out);
    }
    if (a.size() > b.size()) {
      out.push_back(path + "  (input has " + std::to_string(a.size()) + " elements, output has " +
                    std::to_string(b.size()) + ")");
    }
    return;
  }
  if (a.IsMap()) {
    if (!b.IsMap()) {
      out.push_back(path + "  (input is map, output is not)");
      return;
    }
    for (const auto& kv : a) {
      const std::string key = kv.first.Scalar();
      leaf_diff_impl(kv.second, b[key], path_join(path, key), out);
    }
    return;
  }
}

inline std::vector<std::string> leaf_diff(const YAML::Node& a, const YAML::Node& b)
{
  std::vector<std::string> out;
  leaf_diff_impl(a, b, "", out);
  return out;
}

class temp_yaml_file
{
public:
  explicit temp_yaml_file(const std::string& contents)
  {
    char tmpl[] = "/tmp/ocudu_yaml_roundtrip_XXXXXX";
    int  fd     = ::mkstemp(tmpl);
    if (fd < 0) {
      throw std::runtime_error("failed to create temp file");
    }
    ::close(fd);
    path_ = std::string(tmpl) + ".yml";
    std::filesystem::rename(tmpl, path_);
    std::ofstream ofs(path_);
    ofs << contents;
  }
  ~temp_yaml_file()
  {
    std::error_code ec;
    std::filesystem::remove(path_, ec);
  }
  temp_yaml_file(const temp_yaml_file&)            = delete;
  temp_yaml_file& operator=(const temp_yaml_file&) = delete;

  const std::string& path() const { return path_; }

private:
  std::string path_;
};

inline std::string read_file(const std::string& path)
{
  std::ifstream     ifs(path);
  std::stringstream ss;
  ss << ifs.rdbuf();
  return ss.str();
}

template <typename Loader>
void assert_roundtrip(const std::string& yaml_text, Loader loader, const std::string& label)
{
  YAML::Node a = YAML::Load(yaml_text);
  YAML::Node b;
  try {
    b = loader(yaml_text);
  } catch (const std::exception& e) {
    FAIL() << "first parse failed for " << label << ": " << e.what();
  }

  std::vector<std::string> diffs = leaf_diff(a, b);
  if (!diffs.empty()) {
    std::ostringstream os;
    os << "Leaf-preservation failed for " << label << " (" << diffs.size() << " diff(s)):\n";
    for (const auto& d : diffs) {
      os << "  - " << d << "\n";
    }
    FAIL() << os.str();
  }

  const std::string b_text = YAML::Dump(b);
  YAML::Node        c;
  try {
    c = loader(b_text);
  } catch (const std::exception& e) {
    FAIL() << "second parse (fixed-point) failed for " << label << ": " << e.what();
  }
  EXPECT_TRUE(yaml_equal(b, c)) << "Fixed-point B == C failed for " << label;
}

} // namespace ocudu::yaml_roundtrip_test
