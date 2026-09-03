// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/ocudu_assert.h"
#include <cstdio>
#include <random>

namespace ocudu {

/// This custom sink intercepts log messages to count error and warning log entries.
class log_sink_spy : public ocudulog::sink
{
public:
  explicit log_sink_spy(std::unique_ptr<ocudulog::log_formatter> f) :
    ocudulog::sink(std::move(f)), s(ocudulog::get_default_sink()), error_counter(0), warning_counter(0)
  {
  }

  /// Identifier of this custom sink.
  static const char* name() { return "log_sink_spy"; }

  /// Returns the number of log entries tagged as errors.
  unsigned get_error_counter() const
  {
    // Flush to make sure all entries have been processed by the backend.
    ocudulog::flush();
    return error_counter.load();
  }

  /// Returns the number of log entries tagged as warnings.
  unsigned get_warning_counter() const
  {
    // Flush to make sure all entries have been processed by the backend.
    ocudulog::flush();
    return warning_counter.load();
  }

  /// Resets the counters back to 0.
  void reset_counters()
  {
    // Flush to make sure all entries have been processed by the backend.
    ocudulog::flush();
    error_counter.store(0);
    warning_counter.store(0);
  }

  ocudulog::detail::error_string write(ocudulog::detail::memory_buffer buffer) override
  {
    std::string entry(buffer.data(), buffer.size());
    if (entry.find("[E]") != std::string::npos) {
      error_counter.fetch_add(1);
    } else if (entry.find("[W]") != std::string::npos) {
      warning_counter.fetch_add(1);
    }

    return s.write(buffer);
  }

  ocudulog::detail::error_string flush() override { return s.flush(); }

private:
  ocudulog::sink&       s;
  std::atomic<unsigned> error_counter;
  std::atomic<unsigned> warning_counter;
};

/// This custom sink intercepts log messages allowing users to check if a certain log entry has been generated.
/// Calling spy.has_message("something") will return true if any log entries generated so far contain the string
/// "something".
/// The log entries history can be cleared with reset so old entries can be discarded.
class log_sink_message_spy : public ocudulog::sink
{
public:
  explicit log_sink_message_spy(std::unique_ptr<ocudulog::log_formatter> f) :
    ocudulog::sink(std::move(f)), s(ocudulog::get_default_sink())
  {
  }

  /// Identifier of this custom sink.
  static const char* name() { return "log_sink_message_spy"; }

  /// Discards all registered log entries.
  void reset()
  {
    // Flush to make sure all entries have been processed by the backend.
    ocudulog::flush();
    entries.clear();
  }

  /// Returns true if the string in msg is found in the registered log entries.
  bool has_message(const std::string& msg) const
  {
    ocudulog::flush();
    return std::find_if(entries.cbegin(), entries.cend(), [&](const std::string& entry) {
             return entry.find(msg) != std::string::npos;
           }) != entries.cend();
  }

  ocudulog::detail::error_string write(ocudulog::detail::memory_buffer buffer) override
  {
    entries.emplace_back(buffer.data(), buffer.size());

    return s.write(buffer);
  }

  ocudulog::detail::error_string flush() override { return s.flush(); }

private:
  ocudulog::sink&          s;
  std::vector<std::string> entries;
};

/// Delimits beginning/ending of a test with the following console output:
/// ============= [Test <Name of the Test>] ===============
/// <test log>
/// =======================================================
class test_delimit_logger
{
  const size_t delimiter_length = 128;

public:
  template <typename... Args>
  explicit test_delimit_logger(const char* test_name_fmt, Args&&... args)
  {
    test_name               = fmt::format(test_name_fmt, std::forward<Args>(args)...);
    std::string name_str    = fmt::format("[ Test \"{}\" ]", test_name);
    double      nof_repeats = (delimiter_length - name_str.size()) / 2.0;
    fmt::print("{0:=>{1}}{2}{0:=>{3}}\n", "", (int)std::floor(nof_repeats), name_str, (int)std::ceil(nof_repeats));
  }

  test_delimit_logger(const test_delimit_logger&)            = delete;
  test_delimit_logger(test_delimit_logger&&)                 = delete;
  test_delimit_logger& operator=(const test_delimit_logger&) = delete;
  test_delimit_logger& operator=(test_delimit_logger&&)      = delete;

  ~test_delimit_logger()
  {
    ocudulog::flush();
    fmt::print("{:=>{}}\n", "", delimiter_length);
  }

private:
  std::string test_name;
};

/// Test object to verify correct move ctor/assignment logic
struct moveonly_test_object {
  moveonly_test_object() : val_ptr(std::make_unique<int>(object_count_impl()))
  {
    object_count_impl().fetch_add(1, std::memory_order_relaxed);
  }
  moveonly_test_object(int v) : val_ptr(std::make_unique<int>(v))
  {
    object_count_impl().fetch_add(1, std::memory_order_relaxed);
  }
  ~moveonly_test_object() { object_count_impl().fetch_sub(1, std::memory_order_relaxed); }
  moveonly_test_object(moveonly_test_object&& other) noexcept : val_ptr(std::move(other.val_ptr))
  {
    object_count_impl().fetch_add(1, std::memory_order_relaxed);
  }
  moveonly_test_object(const moveonly_test_object& other)            = delete;
  moveonly_test_object& operator=(moveonly_test_object&&) noexcept   = default;
  moveonly_test_object& operator=(const moveonly_test_object& other) = delete;

  bool has_value() const { return val_ptr != nullptr; }

  int value() const
  {
    ocudu_assert(has_value(), "Invalid access");
    return *val_ptr;
  }

  bool operator==(const moveonly_test_object& other) const
  {
    return has_value() == other.has_value() && (!has_value() || value() == other.value());
  }

  bool operator!=(const moveonly_test_object& other) const { return !(*this == other); }

  static size_t object_count() { return object_count_impl().load(std::memory_order_relaxed); }

private:
  static std::atomic<size_t>& object_count_impl()
  {
    static std::atomic<size_t> count{0};
    return count;
  }

  std::unique_ptr<int> val_ptr;
};

inline int format_as(const moveonly_test_object& obj)
{
  return obj.value();
}

/// Test object to verify correct copy ctor/assignment logic
struct copyonly_test_object {
  copyonly_test_object() : val(object_count_impl()) { object_count_impl()++; }
  explicit copyonly_test_object(int v) : val(v) { object_count_impl()++; }
  ~copyonly_test_object() { object_count_impl()--; }
  copyonly_test_object(const copyonly_test_object& other) noexcept : val(other.val) { object_count_impl()++; }
  copyonly_test_object(copyonly_test_object&& other)                    = delete;
  copyonly_test_object& operator=(const copyonly_test_object&) noexcept = default;
  copyonly_test_object& operator=(copyonly_test_object&&)               = delete;

  int value() const { return val; }

  bool operator==(const copyonly_test_object& other) const { return val == other.val; }

  bool operator!=(const copyonly_test_object& other) const { return val != other.val; }

  static size_t object_count() { return object_count_impl(); }

private:
  static size_t& object_count_impl()
  {
    static size_t count = 0;
    return count;
  }

  int val;
};

struct nondefault_ctor_test_object {
  nondefault_ctor_test_object(int val_) : val(val_) {}

  int value() const { return val; }

  bool operator==(const nondefault_ctor_test_object& other) const { return val == other.val; }

  bool operator!=(const nondefault_ctor_test_object& other) const { return val != other.val; }

private:
  int val;
};

/// \brief Helper macro to make sure that logs are flushed when ASSERT_TRUE fails.
#define FLUSH_AND_ASSERT_TRUE(val1)                                                                                    \
  if (not(val1)) {                                                                                                     \
    ocudulog::flush();                                                                                                 \
  }                                                                                                                    \
  ASSERT_TRUE((val1));

/// \brief Helper macro to make sure that logs are flushed when ASSERT_FALSE fails.
#define FLUSH_AND_ASSERT_FALSE(val1)                                                                                   \
  if ((val1)) {                                                                                                        \
    ocudulog::flush();                                                                                                 \
  }                                                                                                                    \
  ASSERT_FALSE((val1));

/// \brief Helper macro to make sure that logs are flushed when ASSERT_EQ fail.
#define FLUSH_AND_ASSERT_EQ(val1, val2)                                                                                \
  if (not((val1) == (val2))) {                                                                                         \
    ocudulog::flush();                                                                                                 \
  }                                                                                                                    \
  ASSERT_EQ((val1), (val2));

/// \brief Helper macro to make sure that logs are flushed when ASSERT_NE fail.
#define FLUSH_AND_ASSERT_NE(val1, val2)                                                                                \
  if (((val1) == (val2))) {                                                                                            \
    ocudulog::flush();                                                                                                 \
  }                                                                                                                    \
  ASSERT_NE((val1), (val2));

} // namespace ocudu
