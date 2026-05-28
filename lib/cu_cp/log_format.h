// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cu_cp_types.h"
#include "ocudu/ran/logical_channel/lcid.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/support/format/fmt_to_c_str.h"
#include "fmt/format.h"

namespace ocudu::ocucp {

struct ue_event_prefix {
  const char*           direction;
  cu_cp_ue_index_t      ue_index;
  rnti_t                rnti;
  cu_cp_du_cell_index_t cell_index;
  const char*           channel = nullptr;
  lcid_t                lcid;

  ue_event_prefix(const char*           dir_      = "CTRL",
                  cu_cp_ue_index_t      ue_index_ = cu_cp_ue_index_t::invalid,
                  rnti_t                rnti_     = rnti_t::INVALID_RNTI,
                  cu_cp_du_cell_index_t cell_idx_ = uint_to_cu_cp_du_cell_index(CU_CP_MAX_NOF_DU_CELLS),
                  const char*           channel_  = nullptr,
                  lcid_t                lcid_     = INVALID_LCID) :
    direction(dir_), ue_index(ue_index_), rnti(rnti_), cell_index(cell_idx_), channel(channel_), lcid(lcid_)
  {
  }

  ue_event_prefix& set_type(const char* t)
  {
    direction = t;
    return *this;
  }
  ue_event_prefix& set_channel(const char* ch_)
  {
    channel = ch_;
    return *this;
  }

  ue_event_prefix& operator|(rnti_t rnti_)
  {
    rnti = rnti_;
    return *this;
  }

  ue_event_prefix& operator|(cu_cp_ue_index_t ue_index_)
  {
    ue_index = ue_index_;
    return *this;
  }

  ue_event_prefix& operator|(cu_cp_du_cell_index_t cell_index_)
  {
    cell_index = cell_index_;
    return *this;
  }

  ue_event_prefix& operator|(lcid_t lcid_)
  {
    lcid = lcid_;
    return *this;
  }
};

inline void
log_proc_started(ocudulog::basic_logger& logger, cu_cp_ue_index_t ue_index, rnti_t rnti, const char* proc_name)
{
  logger.info("{}: \"{}\" started.", ue_event_prefix{"CTRL", ue_index, rnti}, proc_name);
}

inline void log_proc_started(ocudulog::basic_logger& logger, cu_cp_ue_index_t ue_index, const char* proc_name)
{
  logger.info("{}: \"{}\" started.", ue_event_prefix{"CTRL", ue_index}, proc_name);
}

inline void
log_proc_completed(ocudulog::basic_logger& logger, cu_cp_ue_index_t ue_index, rnti_t rnti, const char* proc_name)
{
  logger.info("{}: \"{}\" completed.", ue_event_prefix{"CTRL", ue_index, rnti}, proc_name);
}

template <typename... Args>
void log_proc_failure(ocudulog::basic_logger& logger,
                      cu_cp_ue_index_t        ue_index,
                      const char*             proc_name,
                      const char*             cause_fmt = "",
                      Args&&... args)
{
  fmt::memory_buffer fmtbuf;
  if (strcmp(cause_fmt, "") != 0) {
    fmt::format_to(fmtbuf, "Cause: ");
    fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  }
  logger.warning("{}: \"{}\" failed. {}", ue_event_prefix{"CTRL", ue_index}, proc_name, to_c_str(fmtbuf));
}

template <typename... Args>
void log_proc_failure(ocudulog::basic_logger& logger,
                      cu_cp_ue_index_t        ue_index,
                      rnti_t                  rnti,
                      const char*             proc_name,
                      const char*             cause_fmt = "",
                      Args&&... args)
{
  fmt::memory_buffer fmtbuf;
  if (strcmp(cause_fmt, "") != 0) {
    fmt::format_to(fmtbuf, "Cause: ");
    fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  }
  logger.warning("{}: \"{}\" failed. {}", ue_event_prefix{"CTRL", ue_index, rnti}, proc_name, to_c_str(fmtbuf));
}

template <typename... Args>
void log_proc_event(ocudulog::basic_logger& logger,
                    cu_cp_ue_index_t        ue_index,
                    const char*             proc_name,
                    const char*             cause_fmt,
                    Args&&... args)
{
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  logger.info("{}: {}", ue_event_prefix{"CTRL", ue_index}, to_c_str(fmtbuf));
}

template <typename... Args>
void log_ue_event(ocudulog::basic_logger& logger,
                  const ue_event_prefix&  ue_prefix,
                  const char*             cause_fmt,
                  Args&&... args)
{
  if (not logger.info.enabled()) {
    return;
  }
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  logger.info("{}: {}", ue_prefix, to_c_str(fmtbuf));
}

template <typename... Args>
void log_ue_proc_event(ocudulog::log_channel& log_ch,
                       const ue_event_prefix& ue_prefix,
                       const char*            proc_name,
                       const char*            cause_fmt,
                       Args&&... args)
{
  if (not log_ch.enabled()) {
    return;
  }
  fmt::memory_buffer fmtbuf;
  fmt::format_to(fmtbuf, "\"{}\" ", proc_name);
  fmt::format_to(fmtbuf, cause_fmt, std::forward<Args>(args)...);
  log_ch("{}: {}", ue_prefix, to_c_str(fmtbuf));
}

template <typename... Args>
void log_ul_pdu(ocudulog::basic_logger& logger,
                cu_cp_ue_index_t        ue_index,
                rnti_t                  rnti,
                cu_cp_du_cell_index_t   cell_index,
                const char*             ch,
                const char*             cause_fmt,
                Args&&... args)
{
  log_ue_event(logger, ue_event_prefix{"UL", ue_index, rnti, cell_index, ch}, cause_fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void log_ul_pdu(ocudulog::basic_logger& logger,
                rnti_t                  rnti,
                cu_cp_du_cell_index_t   cc,
                const char*             cause_fmt,
                Args&&... args)
{
  log_ue_event(
      logger, ue_event_prefix{"UL", cu_cp_ue_index_t::invalid, rnti, cc}, cause_fmt, std::forward<Args>(args)...);
}

} // namespace ocudu::ocucp

namespace fmt {

/// FMT formatter of slot_point type.
template <>
struct formatter<ocudu::ocucp::ue_event_prefix> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::ocucp::ue_event_prefix& ue_prefix, FormatContext& ctx) const
  {
    using namespace ocudu;
    auto ret = format_to(ctx.out(), "{:<4}", ue_prefix.direction);
    if (ue_prefix.ue_index != cu_cp_ue_index_t::invalid) {
      ret = format_to(ctx.out(), " ueId={}", ue_prefix.ue_index);
    } else {
      ret = format_to(ctx.out(), "{: <7}", "");
    }
    if (ue_prefix.rnti != ocudu::rnti_t::INVALID_RNTI) {
      ret = format_to(ctx.out(), " {}", ue_prefix.rnti);
    } else {
      ret = format_to(ctx.out(), " {: <6}", "");
    }
    if (ue_prefix.cell_index != ocudu::cu_cp_du_cell_index_t::invalid) {
      ret = format_to(ctx.out(), " cell={}", ue_prefix.cell_index);
    }
    if (ue_prefix.channel != nullptr) {
      ret = format_to(ctx.out(), " {}", ue_prefix.channel);
    }
    if (ue_prefix.lcid <= MAX_LCID) {
      ret = format_to(ctx.out(), " {}", ue_prefix.lcid);
    }
    return ret;
  }
};

} // namespace fmt
