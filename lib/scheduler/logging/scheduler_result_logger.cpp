// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "scheduler_result_logger.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/type_list_buffer.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/csi_report/csi_report_formatters.h"
#include "ocudu/scheduler/result/sched_result.h"
#include "ocudu/support/format/custom_formattable.h"
#include "ocudu/support/format/fmt_to_c_str.h"

using namespace ocudu;

static auto make_dl_dci_log_entry(const dci_dl_info& dci)
{
  bool                   is_formattable = true;
  uint8_t                h_id           = 0;
  bool                   ndi            = false;
  uint8_t                rv             = 0;
  uint8_t                mcs            = 0;
  uint8_t                pucch_res_id   = 0;
  std::optional<int8_t>  tpc_cmd;
  std::optional<uint8_t> dai;
  std::optional<bool>    vrb_prb;

  switch (dci.type()) {
    case dci_dl_rnti_config_type::c_rnti_f1_0: {
      const auto& dci1_0 = dci.as_c_rnti_f1_0();
      h_id               = dci1_0.harq_process_number;
      ndi                = dci1_0.new_data_indicator;
      rv                 = dci1_0.redundancy_version;
      mcs                = dci1_0.modulation_coding_scheme;
      pucch_res_id       = dci1_0.pucch_resource_indicator;
      tpc_cmd            = dci1_0.tpc_command;
    } break;
    case dci_dl_rnti_config_type::tc_rnti_f1_0: {
      const auto& dci1_0 = dci.as_tc_rnti_f1_0();
      h_id               = dci1_0.harq_process_number;
      ndi                = dci1_0.new_data_indicator;
      rv                 = dci1_0.redundancy_version;
      mcs                = dci1_0.modulation_coding_scheme;
      pucch_res_id       = dci1_0.pucch_resource_indicator;
    } break;
    case dci_dl_rnti_config_type::c_rnti_f1_1: {
      const auto& dci1_1 = dci.as_c_rnti_f1_1();
      h_id               = dci1_1.harq_process_number;
      ndi                = dci1_1.tb1_new_data_indicator;
      rv                 = dci1_1.tb1_redundancy_version;
      mcs                = dci1_1.tb1_modulation_coding_scheme;
      pucch_res_id       = dci1_1.pucch_resource_indicator;
      tpc_cmd            = dci1_1.tpc_command;
      vrb_prb            = dci1_1.vrb_prb_mapping;
      if (dci.as_c_rnti_f1_1().downlink_assignment_index.has_value()) {
        dai = dci.as_c_rnti_f1_1().downlink_assignment_index;
      }
    } break;
    default:
      is_formattable = false;
      break;
  }
  return make_formattable([is_formattable, h_id, ndi, rv, mcs, pucch_res_id, dai, tpc_cmd, vrb_prb](auto& ctx) {
    if (is_formattable) {
      fmt::format_to(
          ctx.out(), "dci: h_id={} ndi={} rv={} mcs={} res_ind={}", h_id, ndi ? 1 : 0, rv, mcs, pucch_res_id);
      if (tpc_cmd.has_value()) {
        fmt::format_to(ctx.out(), " tpc={}", *tpc_cmd);
      }
      if (dai.has_value()) {
        fmt::format_to(ctx.out(), " dai={}", dai.value());
      }
      if (vrb_prb.has_value()) {
        fmt::format_to(ctx.out(), " vrb_prb_map_used={}", vrb_prb.value() ? "yes" : "no");
      }
    }
    return ctx.out();
  });
}

static auto make_ul_dci_log_entry(const dci_ul_info& dci)
{
  uint8_t                h_id    = 0;
  bool                   ndi     = false;
  uint8_t                rv      = 0;
  uint8_t                mcs     = 0;
  int8_t                 tpc_cmd = 0;
  std::optional<uint8_t> dai;
  int8_t                 nof_layers = 1;
  int8_t                 ant        = 1;
  std::optional<uint8_t> csi_request;

  switch (dci.type()) {
    case dci_ul_rnti_config_type::c_rnti_f0_0: {
      const auto& dci0_0 = dci.as_c_rnti_f0_0();
      h_id               = dci0_0.harq_process_number;
      ndi                = dci0_0.new_data_indicator;
      rv                 = dci0_0.redundancy_version;
      mcs                = dci0_0.modulation_coding_scheme;
      tpc_cmd            = dci0_0.tpc_command;
    } break;
    case dci_ul_rnti_config_type::tc_rnti_f0_0: {
      const auto& dci0_0 = dci.as_tc_rnti_f0_0();
      rv                 = dci0_0.redundancy_version;
      mcs                = dci0_0.modulation_coding_scheme;
      h_id               = 0;
      ndi                = true;
      tpc_cmd            = dci0_0.tpc_command;
    } break;
    case dci_ul_rnti_config_type::c_rnti_f0_1: {
      const auto& dci0_1 = dci.as_c_rnti_f0_1();
      h_id               = dci0_1.harq_process_number;
      ndi                = dci0_1.new_data_indicator;
      rv                 = dci0_1.redundancy_version;
      mcs                = dci0_1.modulation_coding_scheme;
      tpc_cmd            = dci0_1.tpc_command;
      dai                = dci0_1.first_dl_assignment_index;
      nof_layers         = dci0_1.precoding_info_nof_layers;
      ant                = dci0_1.antenna_ports;
      csi_request        = dci0_1.csi_request;
    } break;
    default:
      report_fatal_error("Invalid UL DCI format");
  }

  return make_formattable([h_id, ndi, rv, mcs, tpc_cmd, dai, nof_layers, ant, csi_request](auto& ctx) {
    fmt::format_to(ctx.out(), "dci: h_id={} ndi={} rv={} mcs={} tpc={}", h_id, ndi ? 1 : 0, rv, mcs, tpc_cmd);
    if (dai.has_value()) {
      fmt::format_to(ctx.out(), " dai={} mimo={} ant={}", *dai, nof_layers, ant);
    }
    if (csi_request.has_value()) {
      fmt::format_to(ctx.out(), " csi_req={}", *csi_request);
    }
    return ctx.out();
  });
}

static auto make_dl_pdcch_log_entry(const pdcch_dl_information& pdcch)
{
  return make_formattable([rnti     = pdcch.ctx.rnti,
                           dci_type = pdcch.dci.type(),
                           cs_id    = pdcch.ctx.coreset_cfg->get_id(),
                           ss_id    = pdcch.ctx.context.ss_id,
                           cces     = pdcch.ctx.cces,
                           dci_log  = make_dl_dci_log_entry(pdcch.dci)](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "\n- DL PDCCH: rnti={} type={} cs_id={} ss_id={} format={} cce={} al={} {}",
                   rnti,
                   dci_dl_rnti_config_rnti_type(dci_type),
                   fmt::underlying(cs_id),
                   fmt::underlying(ss_id),
                   dci_dl_rnti_config_format(dci_type),
                   cces.ncce,
                   to_nof_cces(cces.aggr_lvl),
                   dci_log);
    return ctx.out();
  });
}

static auto make_ul_pdcch_log_entry(const pdcch_ul_information& pdcch)
{
  return make_formattable([rnti     = pdcch.ctx.rnti,
                           dci_type = pdcch.dci.type(),
                           cid      = pdcch.ctx.coreset_cfg->get_id(),
                           ssid     = pdcch.ctx.context.ss_id,
                           cces     = pdcch.ctx.cces,
                           dci_log  = make_ul_dci_log_entry(pdcch.dci)](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "\n- UL PDCCH: rnti={} type={} cs_id={} ss_id={} format={} cce={} al={} {}",
                   rnti,
                   dci_ul_rnti_config_rnti_type(dci_type),
                   fmt::underlying(cid),
                   fmt::underlying(ssid),
                   dci_ul_rnti_config_format(dci_type),
                   cces.ncce,
                   to_nof_cces(cces.aggr_lvl),
                   dci_log);
    return ctx.out();
  });
}

static auto make_sib_info_log_entry(const sib_information& sib_info)
{
  return make_formattable([si_ind       = sib_info.si_indicator,
                           si_msg_index = sib_info.si_msg_index,
                           rbs          = sib_info.pdsch_cfg.rbs,
                           tbs          = sib_info.pdsch_cfg.codewords[0].tb_size_bytes](auto& ctx) {
    if (si_ind == sib_information::sib1) {
      return fmt::format_to(ctx.out(), "SIB1: rb={} tbs={}", rbs, tbs);
    }
    if (si_msg_index.has_value()) {
      // SI message index is zero-based internally.
      return fmt::format_to(ctx.out(), "SI-{}: rb={} tbs={}", static_cast<unsigned>(*si_msg_index) + 1U, rbs, tbs);
    }
    return fmt::format_to(ctx.out(), "SI-?: rb={} tbs={}", rbs, tbs);
  });
}

static auto make_rar_info_log_entry(const rar_information& rar_info)
{
  return make_formattable([rnti = rar_info.pdsch_cfg.rnti,
                           rb   = rar_info.pdsch_cfg.rbs,
                           tbs  = rar_info.pdsch_cfg.codewords[0].tb_size_bytes](auto& ctx) {
    return fmt::format_to(ctx.out(), "RAR: ra-rnti={} rb={} tbs={}", rnti, rb, tbs);
  });
}

static auto make_ue_dl_msg_info_log_entry(const dl_msg_alloc& ue_msg)
{
  return make_formattable([ue_idx   = ue_msg.context.ue_index,
                           rnti     = ue_msg.pdsch_cfg.rnti,
                           rb       = ue_msg.pdsch_cfg.rbs,
                           h_id     = ue_msg.pdsch_cfg.harq_id,
                           ss_id    = ue_msg.context.ss_id,
                           k1       = ue_msg.context.k1,
                           new_data = ue_msg.pdsch_cfg.codewords[0].new_data,
                           rv       = ue_msg.pdsch_cfg.codewords[0].rv_index,
                           tbs      = ue_msg.pdsch_cfg.codewords[0].tb_size_bytes,
                           ri       = ue_msg.pdsch_cfg.nof_layers,
                           dl_bo    = ue_msg.context.buffer_occupancy](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "DL: ue={} c-rnti={} h_id={} ss_id={} rb={} k1={} newtx={} rv={} tbs={}",
                   fmt::underlying(ue_idx),
                   rnti,
                   fmt::underlying(h_id),
                   fmt::underlying(ss_id),
                   rb,
                   k1,
                   new_data,
                   rv,
                   tbs);
    if (new_data) {
      fmt::format_to(ctx.out(), " ri={} dl_bo={}", ri, dl_bo);
    }
    return ctx.out();
  });
}

static auto make_ue_ul_msg_info_log_entry(const ul_sched_info& ue_msg)
{
  return make_formattable([ue_idx     = ue_msg.context.ue_index,
                           rnti       = ue_msg.pusch_cfg.rnti,
                           rb         = ue_msg.pusch_cfg.rbs,
                           h_id       = ue_msg.pusch_cfg.harq_id,
                           ss_id      = ue_msg.context.ss_id,
                           k2         = ue_msg.context.k2,
                           new_data   = ue_msg.pusch_cfg.new_data,
                           rv         = ue_msg.pusch_cfg.rv_index,
                           tbs        = ue_msg.pusch_cfg.tb_size_bytes,
                           nof_retx   = ue_msg.context.nof_retxs,
                           msg3_delay = ue_msg.context.msg3_delay](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "UL: ue={} rnti={} h_id={} ss_id={} rb={} newtx={} rv={} tbs={}",
                   fmt::underlying(ue_idx),
                   rnti,
                   fmt::underlying(h_id),
                   fmt::underlying(ss_id),
                   rb,
                   new_data,
                   rv,
                   tbs);
    if (ue_idx == INVALID_DU_UE_INDEX and nof_retx == 0 and msg3_delay.has_value()) {
      fmt::format_to(ctx.out(), " msg3_delay={}", msg3_delay.value());
    } else {
      fmt::format_to(ctx.out(), " k2={}", k2);
    }
    return ctx.out();
  });
}

static auto make_paging_info_log_entry(const dl_paging_allocation& pg_info)
{
  return make_formattable([rb             = pg_info.pdsch_cfg.rbs,
                           tbs            = pg_info.pdsch_cfg.codewords[0].tb_size_bytes,
                           paging_ue_list = pg_info.paging_ue_list](auto& ctx) {
    fmt::format_to(ctx.out(), "PG: rb={} tbs={}", rb, tbs);
    for (const paging_ue_info& ue : paging_ue_list) {
      fmt::format_to(ctx.out(),
                     "{}{}-pg-id={:#x}",
                     (&ue == &paging_ue_list.front()) ? " ues: " : ", ",
                     ue.paging_type_indicator == paging_ue_info::paging_identity_type::cn_ue_paging_identity ? "cn"
                                                                                                             : "ran",
                     ue.paging_identity);
    }
    return ctx.out();
  });
}

static auto make_info_log_entry(const sched_result& result, bool log_broadcast)
{
  using sib_entry_t    = decltype(make_sib_info_log_entry(std::declval<sib_information>()));
  using rar_entry_t    = decltype(make_rar_info_log_entry(std::declval<rar_information>()));
  using ue_dl_entry_t  = decltype(make_ue_dl_msg_info_log_entry(std::declval<dl_msg_alloc>()));
  using ue_ul_entry_t  = decltype(make_ue_ul_msg_info_log_entry(std::declval<ul_sched_info>()));
  using paging_entry_t = decltype(make_paging_info_log_entry(std::declval<dl_paging_allocation>()));

  auto entries = type_list_buffer_stream<sib_entry_t, rar_entry_t, ue_dl_entry_t, ue_ul_entry_t, paging_entry_t>::make(
                     get_default_fallback_byte_buffer_segment_pool())
                     .value();

  if (log_broadcast) {
    for (const auto& sib : result.dl.bc.sibs) {
      entries.push(make_sib_info_log_entry(sib));
    }
  }
  for (const auto& rar : result.dl.rar_grants) {
    entries.push(make_rar_info_log_entry(rar));
  }
  for (const auto& ue_msg : result.dl.ue_grants) {
    entries.push(make_ue_dl_msg_info_log_entry(ue_msg));
  }
  for (const auto& ue_msg : result.ul.puschs) {
    entries.push(make_ue_ul_msg_info_log_entry(ue_msg));
  }
  for (const auto& pg : result.dl.paging_grants) {
    entries.push(make_paging_info_log_entry(pg));
  }

  return make_formattable([entries = std::move(entries)](auto& ctx) {
    bool first = true;
    entries.for_each([&ctx, &first](const auto& entry) {
      if (!first) {
        fmt::format_to(ctx.out(), ", ");
      }
      fmt::format_to(ctx.out(), "{}", entry);
      first = false;
    });
    return ctx.out();
  });
}

static auto make_ssb_debug_log_entry(const ssb_information& ssb_info)
{
  return make_formattable([ssb_idx = ssb_info.ssb_index, crbs = ssb_info.crbs, symbs = ssb_info.symbols](auto& ctx) {
    return fmt::format_to(ctx.out(), "\n- SSB: ssbIdx={} crbs={} symb={}", ssb_idx, crbs, symbs);
  });
}

template <typename ItemType, typename ListItemFormatter, std::size_t N>
static auto
format_each(const static_vector<ItemType, N>& list, const ListItemFormatter& item_formatter, bool enabled = true)
{
  static_vector<decltype(item_formatter(std::declval<ItemType>())), N> out;
  if (enabled) {
    for (const ItemType& item : list) {
      out.push_back(item_formatter(item));
    }
  }
  return out;
}

static auto make_csi_rs_log_entry(const csi_rs_info& csi_rs)
{
  return make_formattable([type          = csi_rs.type,
                           crbs          = csi_rs.crbs,
                           row           = csi_rs.row,
                           freq_domain   = csi_rs.freq_domain,
                           symbol0       = csi_rs.symbol0,
                           cdm_type      = csi_rs.cdm_type,
                           freq_density  = csi_rs.freq_density,
                           scrambling_id = csi_rs.scrambling_id](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "\n- CSI-RS: type={} crbs={} row={} freq={} symb0={} cdm_type={} freq_density={}",
                   type == csi_rs_type::CSI_RS_NZP ? "nzp" : "zp",
                   crbs,
                   row,
                   freq_domain,
                   symbol0,
                   to_string(cdm_type),
                   to_string(freq_density));
    if (type == csi_rs_type::CSI_RS_NZP) {
      fmt::format_to(ctx.out(), " scramb_id={}", scrambling_id);
    }
    return ctx.out();
  });
}

static auto make_sib_debug_log_entry(const sib_information& sib_info)
{
  return make_formattable([si_ind       = sib_info.si_indicator,
                           si_msg_index = sib_info.si_msg_index,
                           rbs          = sib_info.pdsch_cfg.rbs,
                           symbols      = sib_info.pdsch_cfg.symbols,
                           tbs          = sib_info.pdsch_cfg.codewords[0].tb_size_bytes,
                           mcs          = sib_info.pdsch_cfg.codewords[0].mcs_index,
                           rv           = sib_info.pdsch_cfg.codewords[0].rv_index](auto& ctx) {
    if (si_ind == sib_information::sib1) {
      return fmt::format_to(ctx.out(), "\n- SIB1 PDSCH: rb={} symb={} tbs={} mcs={} rv={}", rbs, symbols, tbs, mcs, rv);
    }
    if (si_msg_index.has_value()) {
      // SI message index is zero-based internally.
      return fmt::format_to(ctx.out(),
                            "\n- SI-{} PDSCH: rb={} symb={} tbs={} mcs={} rv={}",
                            static_cast<unsigned>(*si_msg_index) + 1U,
                            rbs,
                            symbols,
                            tbs,
                            mcs,
                            rv);
    }
    return fmt::format_to(ctx.out(), "\n- SI-? PDSCH: rb={} symb={} tbs={} mcs={} rv={}", rbs, symbols, tbs, mcs, rv);
  });
}

static auto make_rar_debug_log_entry(const rar_information& rar_info)
{
  auto make_rar_grant_debug_entry = [](const rar_ul_grant& grant) {
    const auto* ts     = std::get_if<rar_ul_grant::two_step_info>(&grant.type);
    const auto  result = ts != nullptr ? std::optional<bool>{ts->is_success} : std::nullopt;
    return make_formattable(
        [tcrnti = grant.temp_crnti, rapid = grant.rapid, ta = grant.ta, td = grant.time_resource_assignment, result](
            auto& ctx) {
          fmt::format_to(ctx.out(), "tc-rnti={}: rapid={} ta={} time_res={}", tcrnti, rapid, ta, td);
          if (result.has_value()) {
            fmt::format_to(ctx.out(), " result={}", *result ? "successRAR" : "fallbackRAR");
          }
          return ctx.out();
        });
  };

  return make_formattable([rnti       = rar_info.pdsch_cfg.rnti,
                           rb         = rar_info.pdsch_cfg.rbs,
                           symbols    = rar_info.pdsch_cfg.symbols,
                           tbs        = rar_info.pdsch_cfg.codewords[0].tb_size_bytes,
                           mcs        = rar_info.pdsch_cfg.codewords[0].mcs_index,
                           rv         = rar_info.pdsch_cfg.codewords[0].rv_index,
                           nof_grants = rar_info.grants.size(),
                           grants     = format_each(rar_info.grants, make_rar_grant_debug_entry)](auto& ctx) {
    return fmt::format_to(ctx.out(),
                          "\n- RAR PDSCH: ra-rnti={} rb={} symb={} tbs={} mcs={} rv={} grants ({}): {}",
                          rnti,
                          rb,
                          symbols,
                          tbs,
                          mcs,
                          rv,
                          nof_grants,
                          grants);
  });
}

static auto make_ue_dl_msg_debug_log_entry(const dl_msg_alloc& ue_grant)
{
  return make_formattable([ue_grant](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "\n- UE PDSCH: ue={} c-rnti={} h_id={} rb={} symb={} tbs={} mcs={} rv={} nrtx={} k1={}",
                   fmt::underlying(ue_grant.context.ue_index),
                   ue_grant.pdsch_cfg.rnti,
                   fmt::underlying(ue_grant.pdsch_cfg.harq_id),
                   ue_grant.pdsch_cfg.rbs,
                   ue_grant.pdsch_cfg.symbols,
                   ue_grant.pdsch_cfg.codewords[0].tb_size_bytes,
                   ue_grant.pdsch_cfg.codewords[0].mcs_index,
                   ue_grant.pdsch_cfg.codewords[0].rv_index,
                   ue_grant.context.nof_retxs,
                   ue_grant.context.k1);
    if (ue_grant.pdsch_cfg.precoding.has_value() and not ue_grant.pdsch_cfg.precoding.value().prg_infos.empty()) {
      const auto& prg_type = ue_grant.pdsch_cfg.precoding->prg_infos[0];
      fmt::format_to(ctx.out(), " ri={} {}", ue_grant.pdsch_cfg.nof_layers, precoding_matrix_indicator{prg_type});
    }
    if (ue_grant.pdsch_cfg.codewords[0].new_data) {
      fmt::format_to(ctx.out(), " dl_bo={}", ue_grant.context.buffer_occupancy);
    }
    if (ue_grant.context.olla_offset.has_value()) {
      fmt::format_to(ctx.out(), " olla={:.3}", *ue_grant.context.olla_offset);
    }
    if (not ue_grant.tb_list.empty()) {
      for (const dl_msg_lc_info& lc : ue_grant.tb_list[0].lc_chs_to_sched) {
        fmt::format_to(ctx.out(),
                       "{}lcid={}: size={}",
                       (&lc == &ue_grant.tb_list[0].lc_chs_to_sched.front()) ? " grants: " : ", ",
                       lc.lcid,
                       lc.sched_bytes);
      }
    }
    return ctx.out();
  });
}

static auto make_paging_debug_log_entry(const dl_paging_allocation& pg)
{
  return make_formattable([pg](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "\n- PCCH: rb={} symb={} tbs={} mcs={} rv={}",
                   pg.pdsch_cfg.rbs,
                   pg.pdsch_cfg.symbols,
                   pg.pdsch_cfg.codewords[0].tb_size_bytes,
                   pg.pdsch_cfg.codewords[0].mcs_index,
                   pg.pdsch_cfg.codewords[0].rv_index);

    for (const paging_ue_info& ue : pg.paging_ue_list) {
      fmt::format_to(ctx.out(),
                     "{}{}-pg-id={:#x}",
                     (&ue == &pg.paging_ue_list.front()) ? " ues: " : ", ",
                     ue.paging_type_indicator == paging_ue_info::paging_identity_type::cn_ue_paging_identity ? "cn"
                                                                                                             : "ran",
                     ue.paging_identity);
    }
    return ctx.out();
  });
}

static auto make_pusch_debug_log_entry(const ul_sched_info& ul_info)
{
  return make_formattable([ul_info](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "\n- UE PUSCH: ue={} {}c-rnti={} h_id={} rb={} symb={} tbs={} rv={} nrtx={} nof_layers={}",
                   fmt::underlying(ul_info.context.ue_index),
                   ul_info.context.ue_index == INVALID_DU_UE_INDEX ? "t" : "",
                   ul_info.pusch_cfg.rnti,
                   fmt::underlying(ul_info.pusch_cfg.harq_id),
                   ul_info.pusch_cfg.rbs,
                   ul_info.pusch_cfg.symbols,
                   ul_info.pusch_cfg.tb_size_bytes,
                   ul_info.pusch_cfg.rv_index,
                   ul_info.context.nof_retxs,
                   ul_info.pusch_cfg.nof_layers);
    if (ul_info.context.olla_offset.has_value()) {
      fmt::format_to(ctx.out(), " olla={:.3}", ul_info.context.olla_offset.value());
    }
    if (ul_info.context.ue_index == INVALID_DU_UE_INDEX and ul_info.context.nof_retxs == 0 and
        ul_info.context.msg3_delay.has_value()) {
      fmt::format_to(ctx.out(), " msg3_delay={}", ul_info.context.msg3_delay.value());
    } else {
      fmt::format_to(ctx.out(), " k2={}", ul_info.context.k2);
    }

    if (ul_info.uci.has_value()) {
      fmt::format_to(
          ctx.out(),
          " uci: harq_bits={} csi-1_bits={} csi-2_present={}",
          ul_info.uci.value().harq.has_value() ? ul_info.uci.value().harq.value().harq_ack_nof_bits : 0U,
          ul_info.uci.value().csi.has_value() ? ul_info.uci.value().csi.value().csi_part1_nof_bits : 0U,
          ul_info.uci.value().csi.has_value() && ul_info.uci.value().csi.value().beta_offset_csi_2.has_value() ? "Yes"
                                                                                                               : "No");
    }

    return ctx.out();
  });
}

static auto make_pucch_debug_log_entry(const pucch_info& pucch)
{
  // Capture res by value and pre-compute grant_prbs() to avoid a dangling pointer: the pucch_resource
  // objects live in cell config vectors that may be freed before the async logging thread formats this entry.
  return make_formattable([pucch, res = *pucch.res, grant_prbs = pucch.grant_prbs()](auto& ctx) {
    switch (pucch.format()) {
      case pucch_format::FORMAT_0: {
        format_to(ctx.out(), "\n- PUCCH: c-rnti={} format=0 prb={}", pucch.crnti, grant_prbs);
        if (res.second_hop_prb.has_value()) {
          format_to(ctx.out(), " prb2={}", *res.second_hop_prb);
        }
        format_to(ctx.out(),
                  " symb={} uci: harq_bits={} sr={}",
                  res.syms,
                  pucch.uci_bits.harq_ack_nof_bits,
                  fmt::underlying(pucch.uci_bits.sr_bits));
      } break;
      case pucch_format::FORMAT_1: {
        const auto& f1 = std::get<pucch_resource::f1_config>(res.format_params);
        format_to(ctx.out(), "\n- PUCCH: c-rnti={} format=1 prb={}", pucch.crnti, grant_prbs);
        if (res.second_hop_prb.has_value()) {
          format_to(ctx.out(), " prb2={}", *res.second_hop_prb);
        }
        format_to(ctx.out(),
                  " symb={} cs={} occ={} uci: harq_bits={} sr={}",
                  res.syms,
                  f1.initial_cyclic_shift,
                  f1.time_domain_occ,
                  pucch.uci_bits.harq_ack_nof_bits,
                  fmt::underlying(pucch.uci_bits.sr_bits));

      } break;
      case pucch_format::FORMAT_2: {
        format_to(ctx.out(), "\n- PUCCH: c-rnti={} format=2 prb={}", pucch.crnti, grant_prbs);
        if (res.second_hop_prb.has_value()) {
          format_to(ctx.out(), " prb2={}", *res.second_hop_prb);
        }
        format_to(ctx.out(),
                  " symb={} uci: harq_bits={} sr={} csi-1_bits={}",
                  res.syms,
                  pucch.uci_bits.harq_ack_nof_bits,
                  fmt::underlying(pucch.uci_bits.sr_bits),
                  pucch.uci_bits.csi_part1_nof_bits);

      } break;
      case pucch_format::FORMAT_3: {
        format_to(ctx.out(), "\n- PUCCH: c-rnti={} format=3 prb={}", pucch.crnti, grant_prbs);
        if (res.second_hop_prb.has_value()) {
          format_to(ctx.out(), " prb2={}", *res.second_hop_prb);
        }
        format_to(ctx.out(),
                  " symb={} uci: harq_bits={} sr={} csi-1_bits={}",
                  res.syms,
                  pucch.uci_bits.harq_ack_nof_bits,
                  fmt::underlying(pucch.uci_bits.sr_bits),
                  pucch.uci_bits.csi_part1_nof_bits);
      } break;
      case pucch_format::FORMAT_4: {
        const auto& f4 = std::get<pucch_resource::f4_config>(res.format_params);
        format_to(ctx.out(), "\n- PUCCH: c-rnti={} format=4 prb={}", pucch.crnti, grant_prbs);
        if (res.second_hop_prb.has_value()) {
          format_to(ctx.out(), " prb2={}", *res.second_hop_prb);
        }
        format_to(ctx.out(),
                  " symb={} occ={}/{} uci: harq_bits={} sr={} csi-1_bits={}",
                  res.syms,
                  fmt::underlying(f4.occ_index),
                  fmt::underlying(f4.occ_length),
                  pucch.uci_bits.harq_ack_nof_bits,
                  fmt::underlying(pucch.uci_bits.sr_bits),
                  pucch.uci_bits.csi_part1_nof_bits);
      } break;
      default:
        ocudu_assertion_failure("Invalid PUCCH format");
    }
    return ctx.out();
  });
}

static auto make_srs_debug_log_entry(const srs_info& srs)
{
  return make_formattable([srs](auto& ctx) {
    fmt::format_to(
        ctx.out(),
        "\n- SRS: c-rnti={} symb={} tx-comb=(n{} o={} cs={}) c_srs={} f_sh={} seq_id={} requests=[ch_mtx={} pos={}]",
        srs.crnti,
        srs.symbols,
        static_cast<unsigned>(srs.tx_comb),
        srs.comb_offset,
        srs.cyclic_shift,
        srs.config_index,
        srs.freq_shift,
        srs.sequence_id,
        srs.normalized_channel_iq_matrix_requested ? "yes" : "no",
        srs.positioning_report_requested ? "yes" : "no");
    return ctx.out();
  });
}

static auto make_prach_debug_log_entry(const prach_occasion_info& prach)
{
  return make_formattable([prach](auto& ctx) {
    fmt::format_to(ctx.out(),
                   "\n- PRACH: pci={} format={} nof_occasions={} nof_preambles={}",
                   prach.pci,
                   to_string(prach.format),
                   prach.nof_prach_occasions,
                   prach.nof_preamble_indexes);
    return ctx.out();
  });
}

static auto make_debug_log_entry(const sched_result& result, bool log_broadcast)
{
  using ssb_entry_t      = decltype(make_ssb_debug_log_entry(std::declval<ssb_information>()));
  using dl_pdcch_entry_t = decltype(make_dl_pdcch_log_entry(std::declval<pdcch_dl_information>()));
  using ul_pdcch_entry_t = decltype(make_ul_pdcch_log_entry(std::declval<pdcch_ul_information>()));
  using csi_rs_entry_t   = decltype(make_csi_rs_log_entry(std::declval<csi_rs_info>()));
  using sib_entry_t      = decltype(make_sib_debug_log_entry(std::declval<sib_information>()));
  using rar_entry_t      = decltype(make_rar_debug_log_entry(std::declval<rar_information>()));
  using ue_grant_entry_t = decltype(make_ue_dl_msg_debug_log_entry(std::declval<dl_msg_alloc>()));
  using paging_entry_t   = decltype(make_paging_debug_log_entry(std::declval<dl_paging_allocation>()));
  using pusch_entry_t    = decltype(make_pusch_debug_log_entry(std::declval<ul_sched_info>()));
  using pucch_entry_t    = decltype(make_pucch_debug_log_entry(std::declval<pucch_info>()));
  using srs_entry_t      = decltype(make_srs_debug_log_entry(std::declval<srs_info>()));
  using prach_entry_t    = decltype(make_prach_debug_log_entry(std::declval<prach_occasion_info>()));

  auto entries = type_list_buffer_stream<ssb_entry_t,
                                         dl_pdcch_entry_t,
                                         ul_pdcch_entry_t,
                                         csi_rs_entry_t,
                                         sib_entry_t,
                                         rar_entry_t,
                                         ue_grant_entry_t,
                                         paging_entry_t,
                                         pusch_entry_t,
                                         pucch_entry_t,
                                         srs_entry_t,
                                         prach_entry_t>::make(get_default_fallback_byte_buffer_segment_pool())
                     .value();

  if (log_broadcast) {
    for (const auto& ssb : result.dl.bc.ssb_info) {
      entries.push(make_ssb_debug_log_entry(ssb));
    }
    for (const auto& csi : result.dl.csi_rs) {
      entries.push(make_csi_rs_log_entry(csi));
    }
    for (const auto& sib : result.dl.bc.sibs) {
      entries.push(make_sib_debug_log_entry(sib));
    }
  }
  for (const auto& pdcch : result.dl.dl_pdcchs) {
    if (log_broadcast or pdcch.ctx.rnti != rnti_t::SI_RNTI) {
      entries.push(make_dl_pdcch_log_entry(pdcch));
    }
  }
  for (const auto& pdcch : result.dl.ul_pdcchs) {
    entries.push(make_ul_pdcch_log_entry(pdcch));
  }
  for (const auto& rar : result.dl.rar_grants) {
    entries.push(make_rar_debug_log_entry(rar));
  }
  for (const auto& grant : result.dl.ue_grants) {
    entries.push(make_ue_dl_msg_debug_log_entry(grant));
  }
  for (const auto& pg : result.dl.paging_grants) {
    entries.push(make_paging_debug_log_entry(pg));
  }
  for (const auto& pusch : result.ul.puschs) {
    entries.push(make_pusch_debug_log_entry(pusch));
  }
  for (const auto& pucch : result.ul.pucchs) {
    entries.push(make_pucch_debug_log_entry(pucch));
  }
  for (const auto& srs : result.ul.srss) {
    entries.push(make_srs_debug_log_entry(srs));
  }
  if (log_broadcast) {
    for (const auto& prach : result.ul.prachs) {
      entries.push(make_prach_debug_log_entry(prach));
    }
  }

  return make_formattable([entries = std::move(entries)](auto& ctx) {
    entries.for_each([&ctx](const auto& entry) { fmt::format_to(ctx.out(), "{}", entry); });
    return ctx.out();
  });
}

scheduler_result_logger::scheduler_result_logger(bool log_broadcast_, pci_t pci_) :
  logger(ocudulog::fetch_basic_logger("SCHED")),
  log_broadcast(log_broadcast_),
  enabled(logger.info.enabled()),
  pci(pci_)
{
}

void scheduler_result_logger::log_debug(const sched_result& result, std::chrono::microseconds decision_latency)
{
  const bool broadcast_is_empty = result.dl.bc.ssb_info.empty() and result.dl.bc.sibs.empty() and
                                  result.dl.csi_rs.empty() and result.ul.prachs.empty();

  const bool non_broadcast_is_empty =
      std::none_of(result.dl.dl_pdcchs.begin(),
                   result.dl.dl_pdcchs.end(),
                   [](const pdcch_dl_information& pdcch) { return pdcch.ctx.rnti != rnti_t::SI_RNTI; }) and
      result.dl.ul_pdcchs.empty() and result.dl.paging_grants.empty() and result.dl.rar_grants.empty() and
      result.dl.ue_grants.empty() and result.ul.puschs.empty() and result.ul.pucchs.empty() and result.ul.srss.empty();

  const bool failed_attempts = result.failed_attempts.pdcch + result.failed_attempts.uci > 0;
  const bool slot_is_logged =
      (log_broadcast and not broadcast_is_empty) or not non_broadcast_is_empty or failed_attempts;

  if (slot_is_logged) {
    unsigned nof_pdschs = result.dl.paging_grants.size() + result.dl.rar_grants.size() + result.dl.ue_grants.size();
    if (log_broadcast) {
      nof_pdschs += result.dl.bc.sibs.size();
    }
    const unsigned nof_puschs       = result.ul.puschs.size();
    const unsigned nof_pucchs       = result.ul.pucchs.size();
    const unsigned nof_failed_pdcch = result.failed_attempts.pdcch;
    const unsigned nof_failed_uci   = result.failed_attempts.uci;
    logger.debug("Slot decisions pci={} t={}us ({} PDSCH{}, {} PUSCH{}, {} PUCCH{}, {} attempted PDCCH{}, {} attempted "
                 "UCI{}):{}",
                 pci,
                 decision_latency.count(),
                 nof_pdschs,
                 nof_pdschs == 1 ? "" : "s",
                 nof_puschs,
                 nof_puschs == 1 ? "" : "s",
                 nof_pucchs,
                 nof_pucchs == 1 ? "" : "s",
                 nof_failed_pdcch,
                 nof_failed_pdcch == 1 ? "" : "s",
                 nof_failed_uci,
                 nof_failed_uci == 1 ? "" : "s",
                 make_debug_log_entry(result, log_broadcast));
  }
}

void scheduler_result_logger::log_info(const sched_result& result, std::chrono::microseconds decision_latency)
{
  const bool failed_attempts = result.failed_attempts.pdcch + result.failed_attempts.uci > 0;
  bool       slot_is_logged  = (log_broadcast and not result.dl.bc.sibs.empty()) or not result.dl.rar_grants.empty() or
                        not result.dl.ue_grants.empty() or not result.ul.puschs.empty() or
                        not result.dl.paging_grants.empty() or failed_attempts;

  if (slot_is_logged) {
    unsigned nof_pdschs = result.dl.paging_grants.size() + result.dl.rar_grants.size() + result.dl.ue_grants.size();
    if (log_broadcast) {
      nof_pdschs += result.dl.bc.sibs.size();
    }
    const unsigned nof_puschs = result.ul.puschs.size();
    const unsigned nof_pucchs = result.ul.pucchs.size();
    logger.info("Slot decisions pci={} t={}us ({} PDSCH{}, {} PUSCH{}, {} PUCCH{}): {}",
                pci,
                decision_latency.count(),
                nof_pdschs,
                nof_pdschs == 1 ? "" : "s",
                nof_puschs,
                nof_puschs == 1 ? "" : "s",
                nof_pucchs,
                nof_pucchs == 1 ? "" : "s",
                make_info_log_entry(result, log_broadcast));
  }
}

void scheduler_result_logger::on_scheduler_result(const sched_result&       result,
                                                  std::chrono::microseconds decision_latency)
{
  if (not enabled) {
    return;
  }
  if (logger.debug.enabled()) {
    log_debug(result, decision_latency);
  } else {
    log_info(result, decision_latency);
  }
}
