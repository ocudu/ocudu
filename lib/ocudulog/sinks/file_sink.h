// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "file_utils.h"
#include "ocudu/ocudulog/sink.h"

namespace ocudulog {

/// This sink implementation writes to files. Includes the optional feature of
/// file rotation: a new file is created when file size exceeds an established
/// threshold.
class file_sink : public sink
{
public:
  file_sink(std::string name, size_t max_size_, bool mark_eof_, bool force_flush_, std::unique_ptr<log_formatter> f) :
    sink(std::move(f)),
    max_size((max_size_ == 0) ? 0 : std::max<size_t>(max_size_, 4 * 1024)),
    mark_eof(mark_eof_),
    force_flush(force_flush_),
    base_filename(std::move(name))
  {
  }

  file_sink(const file_sink& other)            = delete;
  file_sink& operator=(const file_sink& other) = delete;
  ~file_sink() { handle_eof(); }

  detail::error_string write(detail::memory_buffer buffer) override
  {
    // Create a new file the first time we hit this method.
    if (is_first_write()) {
      assert(!handler && "No handler should be created yet");
      if (auto err_str = create_file()) {
        return err_str;
      }
    }

    // Do not bother doing any work when the file was closed on a previous
    // error.
    if (!handler) {
      return {};
    }

    if (auto err_str = handle_rotation(buffer.size())) {
      return err_str;
    }

    if (force_flush) {
      flush();
    }

    return handler.write(buffer);
  }

  detail::error_string write_error(detail::memory_buffer input_buffer) override { return write(input_buffer); }

  detail::error_string flush() override { return handler.flush(); }

protected:
  /// Returns the current file index.
  uint32_t get_file_index() const { return file_index; }

private:
  /// Returns true when the sink has never written data to a file, otherwise
  /// returns false.
  bool is_first_write() const { return file_index == 0; }

  /// Creates a new file and increments the file index counter.
  detail::error_string create_file()
  {
    return handler.create(file_utils::build_filename_with_index(base_filename, file_index++));
  }

  /// Handles the file rotation feature when it is activated.
  /// NOTE: The file handler must be valid.
  detail::error_string handle_rotation(size_t size)
  {
    assert(handler && "Expected a valid file handle");
    current_size += size;
    if (max_size && current_size >= max_size) {
      // TODO: consider handle_eof() here
      current_size = size;
      return create_file();
    }
    return {};
  }

  /// Prints the \c eof_msg to the file if the feature is activated
  void handle_eof()
  {
    if (mark_eof) {
      write(detail::memory_buffer{eof_msg});
    }
  }

private:
  const size_t      max_size;
  const bool        mark_eof;
  const bool        force_flush;
  const std::string base_filename;
  file_utils::file  handler;
  size_t            current_size = 0;
  uint32_t          file_index   = 0;

  const std::string eof_msg{"(END)"};
};

} // namespace ocudulog
