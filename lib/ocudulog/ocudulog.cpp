// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ocudulog/ocudulog.h"
#include "formatters/json_formatter.h"
#include "ocudulog_instance.h"
#include "sinks/file_sink.h"
#include "sinks/syslog_sink.h"
#include "sinks/udp_sink.h"

using namespace ocudulog;

/// Returns a copy of the input string with any occurrences of the '#' character
/// removed.
static std::string remove_sharp_chars(const std::string& s)
{
  std::string result(s);
  result.erase(std::remove(result.begin(), result.end(), '#'), result.end());
  return result;
}

/// Generic argument function that fetches a log channel from the repository.
template <typename... Args>
static log_channel& fetch_log_channel_helper(const std::string& id, Args&&... args)
{
  return ocudulog_instance::get().get_channel_repo().emplace(
      std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(id, std::forward<Args>(args)...));
}

///
/// Log channel management function implementations.
///

log_channel* ocudulog::find_log_channel(const std::string& id)
{
  return ocudulog_instance::get().get_channel_repo().find(id);
}

log_channel& ocudulog::fetch_log_channel(const std::string& id)
{
  assert(!id.empty() && "Empty id string");

  std::string clean_id = remove_sharp_chars(id);

  if (auto* c = find_log_channel(clean_id)) {
    return *c;
  }

  ocudulog_instance& instance = ocudulog_instance::get();
  return fetch_log_channel_helper(clean_id, instance.get_default_sink(), instance.get_backend());
}

log_channel& ocudulog::fetch_log_channel(const std::string& id, log_channel_config config)
{
  assert(!id.empty() && "Empty id string");

  std::string clean_id = remove_sharp_chars(id);

  if (auto* c = find_log_channel(clean_id)) {
    return *c;
  }

  ocudulog_instance& instance = ocudulog_instance::get();
  return fetch_log_channel_helper(clean_id, instance.get_default_sink(), instance.get_backend(), std::move(config));
}

log_channel& ocudulog::fetch_log_channel(const std::string& id, sink& s, log_channel_config config)
{
  assert(!id.empty() && "Empty id string");

  std::string clean_id = remove_sharp_chars(id);

  if (auto* c = find_log_channel(clean_id)) {
    return *c;
  }

  ocudulog_instance& instance = ocudulog_instance::get();
  return fetch_log_channel_helper(clean_id, s, instance.get_backend(), std::move(config));
}

///
/// Formatter management functions.
///

void ocudulog::set_default_log_formatter(std::unique_ptr<log_formatter> f)
{
  ocudulog_instance::get().set_default_formatter(std::move(f));
}

std::unique_ptr<log_formatter> ocudulog::get_default_log_formatter()
{
  return ocudulog_instance::get().get_default_formatter();
}

std::unique_ptr<log_formatter> ocudulog::create_text_formatter()
{
  return std::unique_ptr<log_formatter>(new text_formatter);
}

std::unique_ptr<log_formatter> ocudulog::create_contextual_text_formatter()
{
  return std::unique_ptr<log_formatter>(new contextual_text_formatter);
}

std::unique_ptr<log_formatter> ocudulog::create_json_formatter()
{
  return std::unique_ptr<log_formatter>(new json_formatter);
}

///
/// Sink management function implementations.
///

void ocudulog::set_default_sink(sink& s)
{
  ocudulog_instance::get().set_default_sink(s);
}

sink& ocudulog::get_default_sink()
{
  return ocudulog_instance::get().get_default_sink();
}

sink* ocudulog::find_sink(const std::string& id)
{
  auto ptr = ocudulog_instance::get().get_sink_repo().find(id);
  return (ptr) ? ptr->get() : nullptr;
}

sink& ocudulog::fetch_stdout_sink(const std::string& id, std::unique_ptr<log_formatter> f)
{
  assert(!id.empty() && "Empty id string");

  if (auto* s = find_sink(id)) {
    return *s;
  }

  auto& s = ocudulog_instance::get().get_sink_repo().emplace(
      std::piecewise_construct,
      std::forward_as_tuple(id),
      std::forward_as_tuple(new stream_sink(sink_stream_type::stdout, std::move(f))));

  return *s;
}

sink& ocudulog::fetch_stderr_sink(const std::string& id, std::unique_ptr<log_formatter> f)
{
  assert(!id.empty() && "Empty id string");

  if (auto* s = find_sink(id)) {
    return *s;
  }

  auto& s = ocudulog_instance::get().get_sink_repo().emplace(
      std::piecewise_construct,
      std::forward_as_tuple(id),
      std::forward_as_tuple(new stream_sink(sink_stream_type::stderr, std::move(f))));

  return *s;
}

sink& ocudulog::fetch_file_sink(const std::string&             path,
                                size_t                         max_size,
                                bool                           mark_eof,
                                bool                           force_flush,
                                std::unique_ptr<log_formatter> f)
{
  assert(!path.empty() && "Empty path string");

  if (auto* s = find_sink(path)) {
    return *s;
  }

  //: TODO: GCC5 or lower versions emits an error if we use the new() expression
  // directly, use redundant piecewise_construct instead.
  auto& s = ocudulog_instance::get().get_sink_repo().emplace(
      std::piecewise_construct,
      std::forward_as_tuple(path),
      std::forward_as_tuple(new file_sink(path, max_size, mark_eof, force_flush, std::move(f))));

  return *s;
}

sink& ocudulog::fetch_syslog_sink(const std::string&             preamble_,
                                  syslog_local_type              log_local_,
                                  std::unique_ptr<log_formatter> f)
{
  std::string sink_id = preamble_ + std::to_string(static_cast<int>(log_local_));
  if (auto* s = find_sink(sink_id)) {
    return *s;
  }

  //: TODO: GCC5 or lower versions emits an error if we use the new() expression
  // directly, use redundant piecewise_construct instead.
  auto& s = ocudulog_instance::get().get_sink_repo().emplace(
      std::piecewise_construct,
      std::forward_as_tuple(sink_id),
      std::forward_as_tuple(new syslog_sink(std::move(f), preamble_, log_local_)));

  return *s;
}

sink& ocudulog::fetch_udp_sink(const std::string& remote_ip, unsigned port, std::unique_ptr<log_formatter> f)
{
  std::string id = remote_ip + ':' + std::to_string(port);
  if (auto* s = find_sink(id)) {
    return *s;
  }

  auto& s = ocudulog_instance::get().get_sink_repo().emplace(
      std::piecewise_construct,
      std::forward_as_tuple(id),
      std::forward_as_tuple(new udp_sink(remote_ip, port, std::move(f))));

  return *s;
}

bool ocudulog::install_custom_sink(const std::string& id, std::unique_ptr<sink> s)
{
  assert(!id.empty() && "Empty path string");

  sink* input_sink    = s.get();
  sink* returned_sink = ocudulog_instance::get().get_sink_repo().emplace(id, std::move(s)).get();

  // Successful insertion occurs when the returned object is the same one as the
  // input object.
  return input_sink == returned_sink;
}

///
/// Framework configuration and control function implementations.
///

void ocudulog::init(backend_priority priority)
{
  ocudulog_instance::get().get_backend().start(priority);
}

void ocudulog::flush()
{
  ocudulog_instance& instance = ocudulog_instance::get();

  // Nothing to do when the backend is not running yet.
  if (!instance.get_backend().is_running()) {
    return;
  }

  // The backend will set this shared variable when done.
  detail::shared_variable<bool> completion_flag(false);

  auto               sink_ptrs = instance.get_sink_repo().contents();
  std::vector<sink*> sinks;
  sinks.reserve(sink_ptrs.size());
  for (const auto& s : sink_ptrs) {
    sinks.push_back(s->get());
  }

  // Make sure the flush command gets into the backend, otherwise we will be stuck waiting forever for the command to
  // succeed.
  for (;;) {
    detail::log_entry cmd;
    cmd.metadata.store = nullptr;
    cmd.flush_cmd      = std::make_unique<detail::flush_backend_cmd>(detail::flush_backend_cmd{completion_flag, sinks});
    if (instance.get_backend().push(std::move(cmd))) {
      break;
    }
  }

  // Block the caller thread until we are signaled that the flush is completed.
  while (!completion_flag) {
    std::this_thread::sleep_for(std::chrono::microseconds(100));
  }
}

void ocudulog::set_error_handler(error_handler handler)
{
  ocudulog_instance::get().set_error_handler(std::move(handler));
}

///
/// Logger management function implementations.
///

detail::any* ocudulog::detail::find_logger(const std::string& id)
{
  return ocudulog_instance::get().get_logger_repo().find(id);
}

detail::any* ocudulog::detail::fetch_logger(const std::string& id, detail::any&& logger)
{
  assert(!id.empty() && "Empty id string");
  return &ocudulog_instance::get().get_logger_repo().emplace(id, std::move(logger));
}

/// Builds a logger name out of the id and tag.
static std::string build_logger_name(const std::string& id, char tag)
{
  return fmt::format("{}#{}", id, tag);
}

/// Fetches a logger with all its log channels.
static basic_logger& fetch_basic_logger_helper(const std::string& id, sink& s, bool should_print_context)
{
  static constexpr char basic_logger_chan_tags[] = {'E', 'W', 'I', 'D'};

  ocudulog_instance& instance = ocudulog_instance::get();

  // User created log channels cannot have ids with a # character, encode the
  // ids here with a # to ensure all channels are unique.

  log_channel& error =
      fetch_log_channel_helper(build_logger_name(id, basic_logger_chan_tags[0]),
                               s,
                               instance.get_backend(),
                               log_channel_config{id, basic_logger_chan_tags[0], should_print_context});
  log_channel& warning =
      fetch_log_channel_helper(build_logger_name(id, basic_logger_chan_tags[1]),
                               s,
                               instance.get_backend(),
                               log_channel_config{id, basic_logger_chan_tags[1], should_print_context});
  log_channel& info = fetch_log_channel_helper(build_logger_name(id, basic_logger_chan_tags[2]),
                                               s,
                                               instance.get_backend(),
                                               log_channel_config{id, basic_logger_chan_tags[2], should_print_context});
  log_channel& debug =
      fetch_log_channel_helper(build_logger_name(id, basic_logger_chan_tags[3]),
                               s,
                               instance.get_backend(),
                               log_channel_config{id, basic_logger_chan_tags[3], should_print_context});

  return fetch_logger<basic_logger>(id, error, warning, info, debug);
}

basic_logger& ocudulog::fetch_basic_logger(const std::string& id, bool should_print_context)
{
  assert(!id.empty() && "Empty id string");

  if (auto* logger = find_logger<basic_logger>(id)) {
    return *logger;
  }

  return fetch_basic_logger_helper(id, ocudulog_instance::get().get_default_sink(), should_print_context);
}

basic_logger& ocudulog::fetch_basic_logger(const std::string& id, sink& s, bool should_print_context)
{
  assert(!id.empty() && "Empty id string");

  if (auto* logger = find_logger<basic_logger>(id)) {
    return *logger;
  }

  return fetch_basic_logger_helper(id, s, should_print_context);
}

///
/// Deprecated functions to be removed.
///

/// Creates and registers a log channel. Returns a pointer to the newly created
/// channel on success, otherwise nullptr.
static log_channel* create_and_register_log_channel(const std::string& id, sink& s)
{
  assert(!id.empty() && "Empty id string");

  ocudulog_instance& instance = ocudulog_instance::get();

  auto& p = instance.get_channel_repo().emplace(
      std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(id, s, instance.get_backend()));

  return &p;
}

static log_channel* create_and_register_log_channel(const std::string& id, log_channel_config config, sink& s)
{
  assert(!id.empty() && "Empty id string");

  ocudulog_instance& instance = ocudulog_instance::get();

  auto& p =
      instance.get_channel_repo().emplace(std::piecewise_construct,
                                          std::forward_as_tuple(id),
                                          std::forward_as_tuple(id, s, instance.get_backend(), std::move(config)));

  return &p;
}

/// Returns true if the input string contains a sharp character, otherwise
/// returns false.
static bool contains_sharp_char(const std::string& s)
{
  return s.find('#') != std::string::npos;
}

log_channel* ocudulog::create_log_channel(const std::string& id, sink& s)
{
  if (contains_sharp_char(id)) {
    return nullptr;
  }

  return create_and_register_log_channel(id, s);
}

sink* ocudulog::create_stdout_sink(const std::string& name)
{
  return ocudulog_instance::get().get_sink_repo().find("stdout")->get();
}

sink* ocudulog::create_stderr_sink(const std::string& name)
{
  return ocudulog_instance::get().get_sink_repo().find("stderr")->get();
}

sink* ocudulog::create_file_sink(const std::string& path, size_t max_size, bool mark_eof)
{
  //: TODO: GCC5 or lower versions emits an error if we use the new() expression
  // directly, use redundant piecewise_construct instead.
  return ocudulog_instance::get()
      .get_sink_repo()
      .emplace(std::piecewise_construct,
               std::forward_as_tuple(path),
               std::forward_as_tuple(new file_sink(
                   path, max_size, mark_eof, false, std::unique_ptr<log_formatter>(new contextual_text_formatter))))
      .get();
}

basic_logger* ocudulog::create_basic_logger(const std::string& id, sink& s, bool should_print_context)
{
  assert(!id.empty() && "Empty id string");

  static constexpr char basic_logger_chan_tags[] = {'E', 'W', 'I', 'D'};

  auto& logger_repo = ocudulog_instance::get().get_logger_repo();

  // Nothing to do when the logger already exists.
  if (logger_repo.find(id)) {
    return nullptr;
  }

  // User created log channels cannot have ids with a # character, encode the
  // ids here with a # to ensure all channel creations will be successful
  // without any id clashes.

  log_channel* error = create_and_register_log_channel(
      build_logger_name(id, basic_logger_chan_tags[0]), {id, basic_logger_chan_tags[0], should_print_context}, s);
  assert(error && "Could not create channel");
  log_channel* warning = create_and_register_log_channel(
      build_logger_name(id, basic_logger_chan_tags[1]), {id, basic_logger_chan_tags[1], should_print_context}, s);
  assert(warning && "Could not create channel");
  log_channel* info = create_and_register_log_channel(
      build_logger_name(id, basic_logger_chan_tags[2]), {id, basic_logger_chan_tags[2], should_print_context}, s);
  assert(info && "Could not create channel");
  log_channel* debug = create_and_register_log_channel(
      build_logger_name(id, basic_logger_chan_tags[3]), {id, basic_logger_chan_tags[3], should_print_context}, s);
  assert(debug && "Could not create channel");

  return create_logger<basic_logger>(id, *error, *warning, *info, *debug);
}
