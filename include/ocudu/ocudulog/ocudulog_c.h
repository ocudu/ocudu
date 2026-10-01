// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Common types.
 */
typedef int                                 ocudulog_bool;
typedef struct ocudulog_opaque_sink         ocudulog_sink;
typedef struct ocudulog_opaque_log_channel  ocudulog_log_channel;
typedef struct ocudulog_opaque_basic_logger ocudulog_logger;

/**
 * This function initializes the logging framework. It must be called before
 * any log entry is generated.
 * NOTE: Calling this function more than once has no side effects.
 */
void ocudulog_init(void);

/**
 * Installs the specified sink to be used as the default one by new log
 * channels and loggers.
 * The initial default sink writes to stdout.
 */
void ocudulog_set_default_sink(ocudulog_sink* s);

/**
 * Returns the instance of the default sink being used.
 */
ocudulog_sink* ocudulog_get_default_sink(void);

/**
 * Returns an instance of a log_channel with the specified id that writes to
 * the default sink using the default log channel configuration.
 * NOTE: Any '#' characters in the id will get removed.
 */
ocudulog_log_channel* ocudulog_fetch_log_channel(const char* id);

/**
 * Finds a log channel with the specified id string in the repository. On
 * success returns a pointer to the requested log channel, otherwise NULL.
 */
ocudulog_log_channel* ocudulog_find_log_channel(const char* id);

/**
 * Controls whether the specified channel accepts incoming log entries.
 */
void ocudulog_set_log_channel_enabled(ocudulog_log_channel* channel, ocudulog_bool enabled);

/**
 * Returns 1 if the specified channel is accepting incoming log entries,
 * otherwise 0.
 */
ocudulog_bool ocudulog_is_log_channel_enabled(ocudulog_log_channel* channel);

/**
 * Returns the id string of the specified channel.
 */
const char* ocudulog_get_log_channel_id(ocudulog_log_channel* channel);

/**
 * Logs the provided log entry using the specified log channel. When the channel
 * is disabled the log entry wil be discarded.
 * NOTE: Only printf style formatting is supported when using the C API.
 */
void ocudulog_log(ocudulog_log_channel* channel, const char* fmt, ...);

/**
 * Returns an instance of a basic logger (see basic_logger type) with the
 * specified id string. All logger channels will write into the default sink.
 */
ocudulog_logger* ocudulog_fetch_default_logger(const char* id);

/**
 * Finds a logger with the specified id string in the repository. On success
 * returns a pointer to the requested log channel, otherwise NULL.
 */
ocudulog_logger* ocudulog_find_default_logger(const char* id);

/**
 * These functions log the provided log entry using the specified logger.
 * Entries are automatically discarded depending on the configured level of the
 * logger.
 * NOTE: Only printf style formatting is supported when using the C API.
 */
void ocudulog_debug(ocudulog_logger* log, const char* fmt, ...);
void ocudulog_info(ocudulog_logger* log, const char* fmt, ...);
void ocudulog_warning(ocudulog_logger* log, const char* fmt, ...);
void ocudulog_error(ocudulog_logger* log, const char* fmt, ...);

/**
 * Returns the id string of the specified logger.
 */
const char* ocudulog_get_logger_id(ocudulog_logger* log);

typedef enum {
  ocudulog_lvl_none,    /**< disable logger */
  ocudulog_lvl_error,   /**< error logging level */
  ocudulog_lvl_warning, /**< warning logging level */
  ocudulog_lvl_info,    /**< info logging level */
  ocudulog_lvl_debug    /**< debug logging level */
} ocudulog_log_levels;

/**
 * Sets the logging level into the specified logger.
 */
void ocudulog_set_logger_level(ocudulog_logger* log, ocudulog_log_levels lvl);

/**
 * Finds a sink with the specified id string in the repository. On
 * success returns a pointer to the requested sink, otherwise NULL.
 */
ocudulog_sink* ocudulog_find_sink(const char* id);

/**
 * Returns an instance of a sink that writes to the stdout stream.
 */
ocudulog_sink* ocudulog_fetch_stdout_sink(void);

/**
 * Returns an instance of a sink that writes to the stderr stream.
 */
ocudulog_sink* ocudulog_fetch_stderr_sink(void);

/**
 * Returns an instance of a sink that writes into a file in the specified path.
 * Specifying a max_size value different to zero will make the sink create a
 * new file each time the current file exceeds this value. The units of
 * max_size are bytes.
 * Setting force_flush to true will flush the sink after every write.
 * NOTE: Any '#' characters in the id will get removed.
 */
ocudulog_sink* ocudulog_fetch_file_sink(const char* path, size_t max_size, ocudulog_bool force_flush);

#ifdef __cplusplus
}
#endif
