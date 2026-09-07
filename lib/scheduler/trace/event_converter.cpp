// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "event_converter.h"
#include "../config/cell_configuration.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/bwp/bwp_configuration.h"
#include "ocudu/ran/bwp/bwp_id.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/ran/pdcch/coreset.h"
#include "ocudu/ran/precoding/precoding_matrix_indicator.h"
#include "ocudu/ran/pucch/pucch_constants.h"
#include "ocudu/ran/slot_pdu_capacity_constants.h"
#include "ocudu/scheduler/input/uci_inputs.h"
#include "ocudu/scheduler/result/csi_rs_info.h"
#include "ocudu/scheduler/result/pdcch_info.h"
#include "ocudu/scheduler/result/pdsch_info.h"
#include "ocudu/scheduler/result/prach_info.h"
#include "ocudu/scheduler/result/sched_result.h"
#include "ocudu/scheduler/result/srs_info.h"
#include "ocudu/scheduler/scheduler_rach_handler.h"

using namespace ocudu;
using namespace schedtrace;

const coreset_configuration& schedtrace::get_stub_coreset_cfg(unsigned cs_id)
{
  static const auto stubs = [] {
    std::array<coreset_configuration, MAX_NOF_CORESETS> arr;
    arr[0] = coreset_configuration{nr_band::n3,
                                   subcarrier_spacing::kHz30,
                                   subcarrier_spacing::kHz30,
                                   coreset0_index{0},
                                   ssb_subcarrier_offset{0},
                                   ssb_offset_to_pointA{100},
                                   0};
    for (unsigned i = 1; i < MAX_NOF_CORESETS; ++i) {
      arr[i] = coreset_configuration(to_coreset_id(i),
                                     freq_resource_bitmap{},
                                     1,
                                     std::nullopt,
                                     coreset_configuration::precoder_granularity_type::same_as_reg_bundle);
    }
    return arr;
  }();
  return stubs[cs_id];
}

namespace {

/// Union field of a built RbAlloc: the member type and its offset.
struct rb_alloc_offset {
  fbs::RbAlloc              type;
  flatbuffers::Offset<void> value;
};

} // namespace

/// Builds the RbAlloc union member for the given resource allocation.
static rb_alloc_offset convert_rbs_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                         const vrb_alloc&                rbs,
                                         flatbuffers::Optional<uint16_t> vrb_second_hop_start = flatbuffers::nullopt)
{
  if (rbs.is_type1()) {
    const vrb_interval& vrbs = rbs.type1();
    return {fbs::RbAlloc::VrbAlloc,
            fbs::CreateVrbAlloc(fbb, vrbs.start(), static_cast<uint16_t>(vrbs.length()), vrb_second_hop_start).Union()};
  }
  const rbg_bitmap& rbgs = rbs.type0();
  return {fbs::RbAlloc::RbgAlloc,
          fbs::CreateRbgAlloc(fbb, static_cast<uint8_t>(rbgs.size()), static_cast<uint32_t>(rbgs.to_uint64())).Union()};
}

/// Applies a flatbuffer RbAlloc union to a native resource allocation.
template <typename RbsTable>
static void convert_fb_to_rbs(vrb_alloc& rbs, const RbsTable& ev)
{
  switch (ev.rbs_type()) {
    case fbs::RbAlloc::VrbAlloc: {
      const auto& vrb = *ev.rbs_as_VrbAlloc();
      rbs             = vrb_interval{vrb.vrb_start(), static_cast<unsigned>(vrb.vrb_start()) + vrb.vrb_length()};
    } break;
    case fbs::RbAlloc::RbgAlloc: {
      const auto& rbg_alloc = *ev.rbs_as_RbgAlloc();
      rbg_bitmap  bm(rbg_alloc.nof_rbgs());
      bm.from_uint64(static_cast<uint64_t>(rbg_alloc.bitmap()));
      rbs = bm;
    } break;
    default:
      break;
  }
}

/// Builds the single codeword of a broadcast/RA/paging PDSCH (zeros when absent).
static fbs::PdschCodeword get_codeword_fields(const pdsch_information& pcfg)
{
  // Note: new_data is always false for these channels.
  if (pcfg.codewords.empty()) {
    return {0, 0, 0, false};
  }
  const auto& cw = pcfg.codewords[0];
  return {cw.rv_index, cw.mcs_index.value(), static_cast<uint32_t>(cw.tb_size_bytes.value()), false};
}

/// Builds the single codeword of a broadcast/RA/paging PDSCH from its flatbuffer fields.
static pdsch_codeword make_codeword(const fbs::PdschCodeword& ev)
{
  pdsch_codeword cw{};
  cw.rv_index      = ev.rv_index();
  cw.mcs_index     = sch_mcs_index{ev.mcs_index()};
  cw.tb_size_bytes = units::bytes{ev.tb_size_bytes()};
  return cw;
}

template <typename Table>
static ofdm_symbol_range make_symbols(const Table& ev)
{
  return ofdm_symbol_range{ev.sym_start(), static_cast<uint8_t>(ev.sym_start() + ev.sym_length())};
}

flatbuffers::Offset<fbs::CellStartEvent> schedtrace::convert_cell_cfg_to_fb(flatbuffers::FlatBufferBuilder&  fbb,
                                                                            const ocudu::cell_configuration& cell_cfg)
{
  schedtrace::cell_configuration trace_cfg;
  trace_cfg.cell_index                   = cell_cfg.cell_index;
  trace_cfg.init_ul_bwp                  = cell_cfg.init_bwp.ul.cfg();
  trace_cfg.init_dl_bwp                  = cell_cfg.init_bwp.dl.cfg();
  const cell_pucch_res_config& pucch_res = cell_cfg.bwp_res[to_bwp_id(0)].ul().pucch;
  for (const pucch_resource& res : pucch_res.common) {
    trace_cfg.pucch_resources.push_back(res);
  }
  for (const pucch_resource& res : pucch_res.dedicated) {
    trace_cfg.pucch_resources.push_back(res);
  }
  return convert_cell_cfg_to_fb(fbb, trace_cfg);
}

flatbuffers::Offset<fbs::CellStartEvent>
schedtrace::convert_cell_cfg_to_fb(flatbuffers::FlatBufferBuilder& fbb, const schedtrace::cell_configuration& cell_cfg)
{
  const fbs::BwpConfiguration ul_bwp = convert_bwp_cfg_to_fb(cell_cfg.init_ul_bwp);
  const fbs::BwpConfiguration dl_bwp = convert_bwp_cfg_to_fb(cell_cfg.init_dl_bwp);

  std::vector<flatbuffers::Offset<fbs::PucchResource>> pucch_res_offs;
  pucch_res_offs.reserve(cell_cfg.pucch_resources.size());
  for (const pucch_resource& res : cell_cfg.pucch_resources) {
    pucch_res_offs.push_back(convert_pucch_resource_to_fb(fbb, res));
  }
  const auto pucch_res_vec = fbb.CreateVector(pucch_res_offs);

  return fbs::CreateCellStartEvent(fbb, static_cast<uint16_t>(cell_cfg.cell_index), &ul_bwp, &dl_bwp, pucch_res_vec);
}

void schedtrace::convert_fb_to_cell_cfg(cell_configuration& cell_cfg, const fbs::CellStartEvent& start_event)
{
  cell_cfg.cell_index = static_cast<du_cell_index_t>(start_event.pci());
  if (start_event.init_dl_bwp() != nullptr) {
    convert_fb_to_bwp_cfg(cell_cfg.init_dl_bwp, *start_event.init_dl_bwp());
  }
  if (start_event.init_ul_bwp() != nullptr) {
    convert_fb_to_bwp_cfg(cell_cfg.init_ul_bwp, *start_event.init_ul_bwp());
  }

  // The PUCCH resource list contains first the common resources, followed by the dedicated ones. The resource ID is
  // derived from the position of the resource in the list.
  cell_cfg.pucch_resources.clear();
  if (start_event.pucch_resources() != nullptr) {
    unsigned cell_res_idx = 0;
    for (const fbs::PucchResource* fb_res : *start_event.pucch_resources()) {
      pucch_res_id_t res_id =
          cell_res_idx < pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES
              ? pucch_res_id_t::make_cmn(cell_res_idx)
              : pucch_res_id_t::make_ded(cell_res_idx - pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES, 0);
      pucch_resource res;
      convert_fb_to_pucch_resource(res, *fb_res, res_id);
      cell_cfg.pucch_resources.push_back(res);
      ++cell_res_idx;
    }
  }
}

flatbuffers::Offset<fbs::PucchResource> schedtrace::convert_pucch_resource_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                                 const pucch_resource&           res)
{
  fbs::PucchFormat          format_type = fbs::PucchFormat::NONE;
  flatbuffers::Offset<void> format;
  if (const auto* f0 = std::get_if<pucch_resource::f0_config>(&res.format_params)) {
    format_type = fbs::PucchFormat::PucchF0;
    format      = fbs::CreatePucchF0(fbb, static_cast<uint8_t>(f0->initial_cyclic_shift)).Union();
  } else if (const auto* f1 = std::get_if<pucch_resource::f1_config>(&res.format_params)) {
    format_type = fbs::PucchFormat::PucchF1;
    format      = fbs::CreatePucchF1(
                 fbb, static_cast<uint8_t>(f1->initial_cyclic_shift), static_cast<uint8_t>(f1->time_domain_occ))
                 .Union();
  } else if (const auto* f2 = std::get_if<pucch_resource::f2_config>(&res.format_params)) {
    format_type = fbs::PucchFormat::PucchF2;
    format      = fbs::CreatePucchF2(fbb, static_cast<uint8_t>(f2->nof_prbs)).Union();
  } else if (const auto* f3 = std::get_if<pucch_resource::f3_config>(&res.format_params)) {
    format_type = fbs::PucchFormat::PucchF3;
    format = fbs::CreatePucchF3(fbb, static_cast<uint8_t>(f3->nof_prbs), f3->pi_2_bpsk, f3->additional_dmrs).Union();
  } else if (const auto* f4 = std::get_if<pucch_resource::f4_config>(&res.format_params)) {
    format_type = fbs::PucchFormat::PucchF4;
    format      = fbs::CreatePucchF4(fbb,
                                static_cast<uint8_t>(f4->occ_index),
                                static_cast<uint8_t>(f4->occ_length),
                                f4->pi_2_bpsk,
                                f4->additional_dmrs)
                 .Union();
  }

  const auto second_hop =
      res.second_hop_prb.has_value() ? flatbuffers::Optional<uint16_t>{*res.second_hop_prb} : flatbuffers::nullopt;
  return fbs::CreatePucchResource(fbb,
                                  format_type,
                                  format,
                                  static_cast<uint16_t>(res.starting_prb),
                                  second_hop,
                                  static_cast<uint8_t>(res.syms.start()),
                                  static_cast<uint8_t>(res.syms.length()),
                                  static_cast<uint8_t>(res.rep_factor));
}

void schedtrace::convert_fb_to_pucch_resource(pucch_resource&           res,
                                              const fbs::PucchResource& fb,
                                              const pucch_res_id_t&     res_id)
{
  res.res_id       = res_id;
  res.starting_prb = fb.prb_start();
  res.second_hop_prb =
      fb.prb_second_hop_start().has_value() ? std::optional<uint16_t>{fb.prb_second_hop_start().value()} : std::nullopt;
  res.syms       = ofdm_symbol_range{fb.sym_start(), static_cast<uint8_t>(fb.sym_start() + fb.sym_length())};
  res.rep_factor = static_cast<pucch_repetition_factor>(fb.rep_factor());

  switch (fb.format_type()) {
    case fbs::PucchFormat::PucchF0:
      res.format_params.emplace<pucch_resource::f0_config>(
          pucch_resource::f0_config{fb.format_as_PucchF0()->initial_cyclic_shift()});
      break;
    case fbs::PucchFormat::PucchF1:
      res.format_params.emplace<pucch_resource::f1_config>(pucch_resource::f1_config{
          fb.format_as_PucchF1()->initial_cyclic_shift(), fb.format_as_PucchF1()->time_domain_occ()});
      break;
    case fbs::PucchFormat::PucchF2:
      res.format_params.emplace<pucch_resource::f2_config>(
          pucch_resource::f2_config{fb.format_as_PucchF2()->nof_prbs()});
      break;
    case fbs::PucchFormat::PucchF3:
      res.format_params.emplace<pucch_resource::f3_config>(
          pucch_resource::f3_config{fb.format_as_PucchF3()->nof_prbs(),
                                    fb.format_as_PucchF3()->pi_2_bpsk(),
                                    fb.format_as_PucchF3()->additional_dmrs()});
      break;
    case fbs::PucchFormat::PucchF4:
      res.format_params.emplace<pucch_resource::f4_config>(
          pucch_resource::f4_config{static_cast<pucch_f4_occ_idx>(fb.format_as_PucchF4()->occ_index()),
                                    static_cast<pucch_f4_occ_len>(fb.format_as_PucchF4()->occ_length()),
                                    fb.format_as_PucchF4()->pi_2_bpsk(),
                                    fb.format_as_PucchF4()->additional_dmrs()});
      break;
    default:
      break;
  }
}

fbs::BwpConfiguration schedtrace::convert_bwp_cfg_to_fb(const bwp_configuration& bwp)
{
  return fbs::BwpConfiguration(static_cast<fbs::CyclicPrefix>(bwp.cp.value),
                               static_cast<fbs::SubcarrierSpacing>(bwp.scs),
                               bwp.crbs.start(),
                               bwp.crbs.length());
}

void schedtrace::convert_fb_to_bwp_cfg(bwp_configuration& bwp, const fbs::BwpConfiguration& fb)
{
  bwp.cp   = fb.cp() == fbs::CyclicPrefix::Normal ? cyclic_prefix::NORMAL : cyclic_prefix::EXTENDED;
  bwp.scs  = static_cast<subcarrier_spacing>(fb.scs());
  bwp.crbs = crb_interval::start_and_len(fb.crb_start(), fb.crb_length());
}

flatbuffers::Offset<fbs::Pucch> schedtrace::convert_pucch_info_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                     const pucch_info&               pucch)
{
  uint16_t res_idx = 0;
  if (pucch.res->res_id.is_cmn()) {
    res_idx = static_cast<uint16_t>(pucch.res->res_id.cmn().r_pucch);
  } else {
    res_idx = static_cast<uint16_t>(pucch_constants::MAX_NOF_CELL_COMMON_PUCCH_RESOURCES +
                                    pucch.res->res_id.ded().cell_res_id);
  }
  fbs::PucchTxParams        params_type = fbs::PucchTxParams::NONE;
  flatbuffers::Offset<void> params;
  if (const auto* f0 = std::get_if<pucch_info::f0_config>(&pucch.format_params)) {
    params_type = fbs::PucchTxParams::PucchTxF0;
    params      = fbs::CreatePucchTxF0(fbb, static_cast<uint8_t>(f0->group_hopping), f0->n_id_hopping).Union();
  } else if (const auto* f1 = std::get_if<pucch_info::f1_config>(&pucch.format_params)) {
    params_type = fbs::PucchTxParams::PucchTxF1;
    params      = fbs::CreatePucchTxF1(fbb, static_cast<uint8_t>(f1->group_hopping), f1->n_id_hopping).Union();
  } else if (const auto* f2 = std::get_if<pucch_info::f2_config>(&pucch.format_params)) {
    params_type = fbs::PucchTxParams::PucchTxF2;
    params      = fbs::CreatePucchTxF2(fbb, f2->n_id_scrambling, f2->n_id_0_scrambling, f2->nof_prbs).Union();
  } else if (const auto* f3 = std::get_if<pucch_info::f3_config>(&pucch.format_params)) {
    params_type = fbs::PucchTxParams::PucchTxF3;
    params      = fbs::CreatePucchTxF3(fbb,
                                  static_cast<uint8_t>(f3->group_hopping),
                                  f3->n_id_hopping,
                                  f3->n_id_scrambling,
                                  f3->n_id_0_scrambling,
                                  f3->nof_prbs)
                 .Union();
  } else if (const auto* f4 = std::get_if<pucch_info::f4_config>(&pucch.format_params)) {
    params_type = fbs::PucchTxParams::PucchTxF4;
    params =
        fbs::CreatePucchTxF4(
            fbb, static_cast<uint8_t>(f4->group_hopping), f4->n_id_hopping, f4->n_id_scrambling, f4->n_id_0_scrambling)
            .Union();
  }
  flatbuffers::Offset<fbs::PucchRepetition> repetition;
  if (pucch.repetition.has_value()) {
    repetition = fbs::CreatePucchRepetition(
        fbb, pucch.repetition->anchor_slot.count(), static_cast<uint8_t>(pucch.repetition->position));
  }
  return fbs::CreatePucch(fbb,
                          to_value(pucch.crnti),
                          res_idx,
                          params_type,
                          params,
                          pucch.uci_bits.harq_ack_nof_bits,
                          static_cast<uint8_t>(sr_nof_bits_to_uint(pucch.uci_bits.sr_bits)),
                          pucch.uci_bits.csi_part1_nof_bits,
                          repetition);
}

void schedtrace::convert_fb_to_pucch_info(pucch_info&                pucch,
                                          const fbs::Pucch&          fb,
                                          const bwp_configuration*   ul_bwp,
                                          span<const pucch_resource> pucch_resources)
{
  pucch.crnti   = to_rnti(fb.rnti());
  pucch.bwp_cfg = ul_bwp;
  ocudu_assert(fb.res_idx() < pucch_resources.size(),
               "PUCCH resource index {} exceeds the size of the cell PUCCH resource list ({})",
               fb.res_idx(),
               pucch_resources.size());
  pucch.res                         = &pucch_resources[fb.res_idx()];
  pucch.uci_bits.harq_ack_nof_bits  = fb.nof_bits_harq_ack();
  pucch.uci_bits.sr_bits            = static_cast<sr_nof_bits>(fb.nof_bits_sr());
  pucch.uci_bits.csi_part1_nof_bits = fb.nof_bits_csi1();
  switch (fb.format_params_type()) {
    case fbs::PucchTxParams::PucchTxF0: {
      const auto* fb_f0 = fb.format_params_as_PucchTxF0();
      auto&       f0    = pucch.format_params.emplace<pucch_info::f0_config>();
      f0.group_hopping  = static_cast<pucch_group_hopping>(fb_f0->group_hopping());
      f0.n_id_hopping   = fb_f0->n_id_hopping();
    } break;
    case fbs::PucchTxParams::PucchTxF1: {
      const auto* fb_f1 = fb.format_params_as_PucchTxF1();
      auto&       f1    = pucch.format_params.emplace<pucch_info::f1_config>();
      f1.group_hopping  = static_cast<pucch_group_hopping>(fb_f1->group_hopping());
      f1.n_id_hopping   = fb_f1->n_id_hopping();
    } break;
    case fbs::PucchTxParams::PucchTxF2: {
      const auto* fb_f2    = fb.format_params_as_PucchTxF2();
      auto&       f2       = pucch.format_params.emplace<pucch_info::f2_config>();
      f2.n_id_scrambling   = fb_f2->n_id_scrambling();
      f2.n_id_0_scrambling = fb_f2->n_id_0_scrambling();
      f2.nof_prbs          = fb_f2->nof_prbs();
    } break;
    case fbs::PucchTxParams::PucchTxF3: {
      const auto* fb_f3    = fb.format_params_as_PucchTxF3();
      auto&       f3       = pucch.format_params.emplace<pucch_info::f3_config>();
      f3.group_hopping     = static_cast<pucch_group_hopping>(fb_f3->group_hopping());
      f3.n_id_hopping      = fb_f3->n_id_hopping();
      f3.n_id_scrambling   = fb_f3->n_id_scrambling();
      f3.n_id_0_scrambling = fb_f3->n_id_0_scrambling();
      f3.nof_prbs          = fb_f3->nof_prbs();
    } break;
    case fbs::PucchTxParams::PucchTxF4: {
      const auto* fb_f4    = fb.format_params_as_PucchTxF4();
      auto&       f4       = pucch.format_params.emplace<pucch_info::f4_config>();
      f4.group_hopping     = static_cast<pucch_group_hopping>(fb_f4->group_hopping());
      f4.n_id_hopping      = fb_f4->n_id_hopping();
      f4.n_id_scrambling   = fb_f4->n_id_scrambling();
      f4.n_id_0_scrambling = fb_f4->n_id_0_scrambling();
    } break;
    default:
      pucch.set_format(pucch.res->format());
      break;
  }
  if (const auto* fb_rep = fb.repetition()) {
    pucch.repetition = pucch_info::repetition_info{slot_point(ul_bwp->scs, fb_rep->anchor_slot()),
                                                   static_cast<pucch_repetition_tx_slot>(fb_rep->position())};
  }
}

flatbuffers::Offset<fbs::Pusch> schedtrace::convert_pusch_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                const ul_sched_info&            pusch)
{
  const auto&           pcfg       = pusch.pusch_cfg;
  const auto            second_hop = pcfg.rbs.is_type1() && pcfg.intra_slot_freq_hopping
                                         ? flatbuffers::Optional<uint16_t>{static_cast<uint16_t>(pcfg.pusch_second_hop_prb)}
                                         : flatbuffers::nullopt;
  const rb_alloc_offset rbs        = convert_rbs_to_fb(fbb, pcfg.rbs, second_hop);
  return fbs::CreatePusch(fbb,
                          to_value(pcfg.rnti),
                          rbs.type,
                          rbs.value,
                          static_cast<uint8_t>(pcfg.symbols.start()),
                          static_cast<uint8_t>(pcfg.symbols.length()),
                          pcfg.rv_index,
                          pcfg.nof_layers,
                          static_cast<uint8_t>(pcfg.harq_id),
                          static_cast<uint32_t>(pcfg.tb_size_bytes.value()),
                          static_cast<uint16_t>(pusch.context.ue_index),
                          pusch.context.nof_retxs,
                          pusch.context.k2);
}

void schedtrace::convert_fb_to_pusch(ul_sched_info& pusch, const fbs::Pusch& fb, const bwp_configuration* ul_bwp)
{
  pusch.pusch_cfg.bwp_cfg = ul_bwp;
  pusch.pusch_cfg.rnti    = to_rnti(fb.rnti());
  convert_fb_to_rbs(pusch.pusch_cfg.rbs, fb);
  if (fb.rbs_type() == fbs::RbAlloc::VrbAlloc) {
    const auto second_hop                   = fb.rbs_as_VrbAlloc()->vrb_second_hop_start();
    pusch.pusch_cfg.intra_slot_freq_hopping = second_hop.has_value();
    pusch.pusch_cfg.pusch_second_hop_prb    = second_hop.value_or(0);
  }
  pusch.pusch_cfg.symbols       = make_symbols(fb);
  pusch.pusch_cfg.rv_index      = fb.rv_index();
  pusch.pusch_cfg.nof_layers    = fb.nof_layers();
  pusch.pusch_cfg.harq_id       = to_harq_id(fb.harq_id());
  pusch.pusch_cfg.tb_size_bytes = units::bytes{fb.tb_size_bytes()};
  pusch.context.ue_index        = to_du_ue_index(fb.ue_index());
  pusch.context.nof_retxs       = fb.nof_retxs();
  pusch.context.k2              = fb.k2();
}

flatbuffers::Offset<fbs::Pdsch> schedtrace::convert_pdsch_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                const dl_msg_alloc&             pdsch)
{
  const auto&           pcfg = pdsch.pdsch_cfg;
  const rb_alloc_offset rbs  = convert_rbs_to_fb(fbb, pcfg.rbs);

  std::array<fbs::PdschCodeword, MAX_CODEWORDS_PER_PDSCH> cws;
  unsigned                                                nof_cws = 0;
  for (const auto& cw : pcfg.codewords) {
    cws[nof_cws++] = fbs::PdschCodeword(
        cw.rv_index, cw.mcs_index.value(), static_cast<uint32_t>(cw.tb_size_bytes.value()), cw.new_data);
  }
  const auto cws_vec = nof_cws == 0 ? 0 : fbb.CreateVectorOfStructs(cws.data(), nof_cws);

  return fbs::CreatePdsch(fbb,
                          to_value(pcfg.rnti),
                          rbs.type,
                          rbs.value,
                          static_cast<uint8_t>(pcfg.symbols.start()),
                          static_cast<uint8_t>(pcfg.symbols.length()),
                          cws_vec,
                          static_cast<uint8_t>(pcfg.harq_id),
                          static_cast<uint16_t>(pdsch.context.ue_index),
                          pdsch.context.nof_retxs,
                          pdsch.context.k1);
}

void schedtrace::convert_fb_to_pdsch(dl_msg_alloc& pdsch, const fbs::Pdsch& fb, const bwp_configuration* dl_bwp)
{
  pdsch.pdsch_cfg.bwp_cfg = dl_bwp;
  // coreset_cfg is not part of the wire format -- fbs::Pdsch carries no coreset index to look it up by.
  pdsch.pdsch_cfg.coreset_cfg = nullptr;
  pdsch.pdsch_cfg.rnti        = to_rnti(fb.rnti());
  convert_fb_to_rbs(pdsch.pdsch_cfg.rbs, fb);
  pdsch.pdsch_cfg.symbols = make_symbols(fb);
  if (fb.codewords() != nullptr) {
    for (const fbs::PdschCodeword* cw_ev : *fb.codewords()) {
      pdsch_codeword cw;
      cw.rv_index      = cw_ev->rv_index();
      cw.mcs_index     = sch_mcs_index{cw_ev->mcs_index()};
      cw.tb_size_bytes = units::bytes{cw_ev->tb_size_bytes()};
      cw.new_data      = cw_ev->new_data();
      pdsch.pdsch_cfg.codewords.push_back(cw);
    }
  }
  pdsch.pdsch_cfg.harq_id = to_harq_id(fb.harq_id());
  pdsch.context.ue_index  = to_du_ue_index(fb.ue_index());
  pdsch.context.nof_retxs = fb.nof_retxs();
  pdsch.context.k1        = fb.k1();
}

flatbuffers::Offset<fbs::SibPdsch> schedtrace::convert_sib_pdsch_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                       const sib_information&          sib)
{
  const auto&              pcfg = sib.pdsch_cfg;
  const rb_alloc_offset    rbs  = convert_rbs_to_fb(fbb, pcfg.rbs);
  const fbs::PdschCodeword cw   = get_codeword_fields(pcfg);
  return fbs::CreateSibPdsch(fbb,
                             sib.si_indicator == sib_information::sib1,
                             rbs.type,
                             rbs.value,
                             static_cast<uint8_t>(pcfg.symbols.start()),
                             static_cast<uint8_t>(pcfg.symbols.length()),
                             &cw);
}

void schedtrace::convert_fb_to_sib_pdsch(sib_information& sib, const fbs::SibPdsch& fb, const bwp_configuration* dl_bwp)
{
  sib.si_indicator = fb.is_sib1() ? sib_information::sib1 : sib_information::other_si;
  auto& pcfg       = sib.pdsch_cfg;
  pcfg.bwp_cfg     = dl_bwp;
  // coreset_cfg is not part of the wire format -- fbs::SibPdsch carries no coreset index to look it up by.
  pcfg.coreset_cfg = nullptr;
  convert_fb_to_rbs(pcfg.rbs, fb);
  pcfg.symbols = make_symbols(fb);
  pcfg.codewords.push_back(make_codeword(*fb.codeword()));
}

flatbuffers::Offset<fbs::RarPdsch> schedtrace::convert_rar_pdsch_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                       const rar_information&          rar)
{
  const auto&              pcfg = rar.pdsch_cfg;
  const rb_alloc_offset    rbs  = convert_rbs_to_fb(fbb, pcfg.rbs);
  const fbs::PdschCodeword cw   = get_codeword_fields(pcfg);

  std::array<fbs::RarUlGrant, MAX_GRANTS_PER_RAR> grants;
  unsigned                                        nof_grants = 0;
  for (const auto& grant : rar.grants) {
    uint8_t harq_feedback_timing_indicator = 0;
    uint8_t pucch_resource_indicator       = 0;
    if (const auto* two_step = std::get_if<rar_ul_grant::two_step_success_info>(&grant.type)) {
      harq_feedback_timing_indicator = two_step->harq_feedback_timing_indicator;
      pucch_resource_indicator       = two_step->pucch_resource_indicator;
    }
    grants[nof_grants++] = fbs::RarUlGrant(to_value(grant.temp_crnti),
                                           grant.rapid,
                                           static_cast<uint16_t>(grant.ta),
                                           static_cast<uint8_t>(grant.time_resource_assignment),
                                           grant.freq_hop_flag,
                                           grant.freq_resource_assignment,
                                           static_cast<uint8_t>(grant.mcs.value()),
                                           grant.tpc,
                                           grant.csi_req,
                                           static_cast<uint8_t>(grant.type.index()),
                                           harq_feedback_timing_indicator,
                                           pucch_resource_indicator);
  }
  const auto grants_vec = nof_grants != 0 ? fbb.CreateVectorOfStructs(grants.data(), nof_grants) : 0;

  return fbs::CreateRarPdsch(fbb,
                             to_value(pcfg.rnti),
                             rbs.type,
                             rbs.value,
                             static_cast<uint8_t>(pcfg.symbols.start()),
                             static_cast<uint8_t>(pcfg.symbols.length()),
                             &cw,
                             grants_vec);
}

void schedtrace::convert_fb_to_rar_pdsch(rar_information& rar, const fbs::RarPdsch& fb, const bwp_configuration* dl_bwp)
{
  auto& pcfg   = rar.pdsch_cfg;
  pcfg.bwp_cfg = dl_bwp;
  // coreset_cfg is not part of the wire format -- fbs::RarPdsch carries no coreset index to look it up by.
  pcfg.coreset_cfg = nullptr;
  pcfg.rnti        = to_rnti(fb.rnti());
  convert_fb_to_rbs(pcfg.rbs, fb);
  pcfg.symbols = make_symbols(fb);
  pcfg.codewords.push_back(make_codeword(*fb.codeword()));
  if (fb.grants() != nullptr) {
    for (const fbs::RarUlGrant* grant_ev : *fb.grants()) {
      rar_ul_grant grant;
      grant.temp_crnti               = to_rnti(grant_ev->temp_crnti());
      grant.rapid                    = grant_ev->rapid();
      grant.ta                       = grant_ev->ta();
      grant.time_resource_assignment = grant_ev->time_resource_assignment();
      grant.freq_hop_flag            = grant_ev->freq_hop_flag();
      grant.freq_resource_assignment = grant_ev->freq_resource_assignment();
      grant.mcs                      = sch_mcs_index{grant_ev->mcs()};
      grant.tpc                      = grant_ev->tpc();
      grant.csi_req                  = grant_ev->csi_req();
      switch (grant_ev->grant_type()) {
        case 1: {
          rar_ul_grant::two_step_success_info info;
          info.harq_feedback_timing_indicator = grant_ev->harq_feedback_timing_indicator();
          info.pucch_resource_indicator       = grant_ev->pucch_resource_indicator();
          grant.type                          = info;
        } break;
        case 2:
          grant.type = rar_ul_grant::two_step_fallback_info{};
          break;
        default:
          grant.type = rar_ul_grant::four_step_info{};
          break;
      }
      rar.grants.push_back(grant);
    }
  }
}

flatbuffers::Offset<fbs::PagingPdsch> schedtrace::convert_paging_pdsch_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                             const dl_paging_allocation&     paging)
{
  const auto&              pcfg = paging.pdsch_cfg;
  const rb_alloc_offset    rbs  = convert_rbs_to_fb(fbb, pcfg.rbs);
  const fbs::PdschCodeword cw   = get_codeword_fields(pcfg);

  std::array<fbs::PagingUe, MAX_PAGING_RECORDS_PER_PAGING_PDU> ues;
  unsigned                                                     nof_ues = 0;
  for (const auto& ue : paging.paging_ue_list) {
    ues[nof_ues++] =
        fbs::PagingUe(ue.paging_identity, ue.paging_type_indicator == paging_ue_info::cn_ue_paging_identity);
  }
  const auto ues_vec = nof_ues != 0 ? fbb.CreateVectorOfStructs(ues.data(), nof_ues) : 0;

  return fbs::CreatePagingPdsch(fbb,
                                rbs.type,
                                rbs.value,
                                static_cast<uint8_t>(pcfg.symbols.start()),
                                static_cast<uint8_t>(pcfg.symbols.length()),
                                &cw,
                                ues_vec);
}

void schedtrace::convert_fb_to_paging_pdsch(dl_paging_allocation&    paging,
                                            const fbs::PagingPdsch&  fb,
                                            const bwp_configuration* dl_bwp)
{
  auto& pcfg   = paging.pdsch_cfg;
  pcfg.bwp_cfg = dl_bwp;
  // coreset_cfg is not part of the wire format -- fbs::PagingPdsch carries no coreset index to look it up by.
  pcfg.coreset_cfg = nullptr;
  convert_fb_to_rbs(pcfg.rbs, fb);
  pcfg.symbols = make_symbols(fb);
  pcfg.codewords.push_back(make_codeword(*fb.codeword()));
  if (fb.paging_ues() != nullptr) {
    for (const fbs::PagingUe* ue_ev : *fb.paging_ues()) {
      paging_ue_info ue;
      ue.paging_type_indicator =
          ue_ev->is_cn_paging() ? paging_ue_info::cn_ue_paging_identity : paging_ue_info::ran_ue_paging_identity;
      ue.paging_identity = ue_ev->paging_identity();
      paging.paging_ue_list.push_back(ue);
    }
  }
}

fbs::Srs schedtrace::convert_srs_to_fb(const srs_info& srs)
{
  return {to_value(srs.crnti),
          srs.nof_antenna_ports,
          static_cast<uint8_t>(srs.symbols.start()),
          static_cast<uint8_t>(srs.symbols.length()),
          static_cast<uint8_t>(srs.nof_repetitions),
          srs.config_index,
          srs.sequence_id,
          srs.bw_index,
          static_cast<uint8_t>(srs.tx_comb),
          srs.comb_offset,
          srs.cyclic_shift,
          srs.freq_position,
          srs.freq_shift,
          srs.freq_hopping,
          static_cast<uint8_t>(srs.group_or_seq_hopping),
          static_cast<uint8_t>(srs.resource_type),
          static_cast<uint16_t>(srs.t_srs_period),
          srs.t_offset,
          srs.normalized_channel_iq_matrix_requested,
          srs.positioning_report_requested};
}

void schedtrace::convert_fb_to_srs(srs_info& srs, const fbs::Srs& fb, const bwp_configuration* ul_bwp)
{
  srs.bwp_cfg                                = ul_bwp;
  srs.crnti                                  = to_rnti(fb.rnti());
  srs.nof_antenna_ports                      = fb.nof_antenna_ports();
  srs.symbols                                = make_symbols(fb);
  srs.nof_repetitions                        = static_cast<srs_nof_symbols>(fb.nof_repetitions());
  srs.config_index                           = fb.config_index();
  srs.sequence_id                            = fb.sequence_id();
  srs.bw_index                               = fb.bw_index();
  srs.tx_comb                                = static_cast<tx_comb_size>(fb.tx_comb());
  srs.comb_offset                            = fb.comb_offset();
  srs.cyclic_shift                           = fb.cyclic_shift();
  srs.freq_position                          = fb.freq_position();
  srs.freq_shift                             = fb.freq_shift();
  srs.freq_hopping                           = fb.freq_hopping();
  srs.group_or_seq_hopping                   = static_cast<srs_group_or_sequence_hopping>(fb.group_or_seq_hopping());
  srs.resource_type                          = static_cast<srs_resource_type>(fb.resource_type());
  srs.t_srs_period                           = static_cast<srs_periodicity>(fb.t_srs_period());
  srs.t_offset                               = fb.t_offset();
  srs.normalized_channel_iq_matrix_requested = fb.normalized_channel_iq_matrix_req();
  srs.positioning_report_requested           = fb.positioning_report_req();
}

fbs::CsiRs schedtrace::convert_csi_rs_to_fb(const csi_rs_info& csi)
{
  // to_uint64() asserts on an empty bitset.
  const uint16_t freq_domain = csi.freq_domain.size() == 0 ? 0 : static_cast<uint16_t>(csi.freq_domain.to_uint64());
  return {csi.crbs.start(),
          static_cast<uint16_t>(csi.crbs.length()),
          static_cast<uint8_t>(csi.type),
          csi.row,
          freq_domain,
          static_cast<uint8_t>(csi.freq_domain.size()),
          csi.symbol0,
          csi.symbol1,
          static_cast<uint8_t>(csi.cdm_type),
          static_cast<uint8_t>(csi.freq_density),
          csi.scrambling_id,
          csi.power_ctrl_offset,
          csi.power_ctrl_offset_ss};
}

void schedtrace::convert_fb_to_csi_rs(csi_rs_info& csi, const fbs::CsiRs& fb, const bwp_configuration* dl_bwp)
{
  csi.bwp_cfg     = dl_bwp;
  csi.crbs        = crb_interval{fb.crb_start(), fb.crb_start() + fb.crb_length()};
  csi.type        = static_cast<csi_rs_type>(fb.type());
  csi.row         = fb.row();
  csi.freq_domain = bounded_bitset<12, false>(fb.freq_domain_nof_bits());
  // from_uint64() asserts on an empty bitset.
  if (fb.freq_domain_nof_bits() != 0) {
    csi.freq_domain.from_uint64(static_cast<uint64_t>(fb.freq_domain()));
  }
  csi.symbol0              = fb.symbol0();
  csi.symbol1              = fb.symbol1();
  csi.cdm_type             = static_cast<csi_rs_cdm_type>(fb.cdm_type());
  csi.freq_density         = static_cast<csi_rs_freq_density_type>(fb.freq_density());
  csi.scrambling_id        = fb.scrambling_id();
  csi.power_ctrl_offset    = fb.power_ctrl_offset();
  csi.power_ctrl_offset_ss = fb.power_ctrl_offset_ss();
}

fbs::Ssb schedtrace::convert_ssb_to_fb(const ssb_information& ssb)
{
  return {ssb.ssb_index,
          ssb.crbs.start(),
          static_cast<uint16_t>(ssb.crbs.length()),
          static_cast<uint8_t>(ssb.symbols.start()),
          static_cast<uint8_t>(ssb.symbols.length())};
}

void schedtrace::convert_fb_to_ssb(ssb_information& ssb, const fbs::Ssb& fb)
{
  ssb.ssb_index = fb.ssb_index();
  ssb.crbs      = crb_interval{fb.crb_start(), fb.crb_start() + fb.crb_length()};
  ssb.symbols   = ofdm_symbol_range{fb.sym_start(), static_cast<uint8_t>(fb.sym_start() + fb.sym_length())};
}

fbs::DlPdcch schedtrace::convert_dl_pdcch_to_fb(const pdcch_dl_information& pdcch)
{
  // Only the DCI fields the wire format carries are extracted; a field absent from the active DCI type stays 0.
  uint8_t harq_id       = 0;
  bool    ndi           = false;
  uint8_t rv            = 0;
  uint8_t mcs           = 0;
  uint8_t pucch_res_ind = 0;
  switch (pdcch.dci.type()) {
    case dci_dl_rnti_config_type::si_f1_0: {
      const auto& dci = pdcch.dci.as_si_rnti_f1_0();
      rv              = static_cast<uint8_t>(dci.redundancy_version);
      mcs             = static_cast<uint8_t>(dci.modulation_coding_scheme);
    } break;
    case dci_dl_rnti_config_type::ra_f1_0: {
      mcs = static_cast<uint8_t>(pdcch.dci.as_ra_rnti_f1_0().modulation_coding_scheme);
    } break;
    case dci_dl_rnti_config_type::p_rnti_f1_0: {
      mcs = static_cast<uint8_t>(pdcch.dci.as_p_rnti_f1_0().modulation_coding_scheme);
    } break;
    case dci_dl_rnti_config_type::c_rnti_f1_0: {
      const auto& dci = pdcch.dci.as_c_rnti_f1_0();
      harq_id         = static_cast<uint8_t>(dci.harq_process_number);
      ndi             = dci.new_data_indicator;
      rv              = static_cast<uint8_t>(dci.redundancy_version);
      mcs             = static_cast<uint8_t>(dci.modulation_coding_scheme);
      pucch_res_ind   = static_cast<uint8_t>(dci.pucch_resource_indicator);
    } break;
    case dci_dl_rnti_config_type::tc_rnti_f1_0: {
      const auto& dci = pdcch.dci.as_tc_rnti_f1_0();
      harq_id         = static_cast<uint8_t>(dci.harq_process_number);
      ndi             = dci.new_data_indicator;
      rv              = static_cast<uint8_t>(dci.redundancy_version);
      mcs             = static_cast<uint8_t>(dci.modulation_coding_scheme);
      pucch_res_ind   = static_cast<uint8_t>(dci.pucch_resource_indicator);
    } break;
    case dci_dl_rnti_config_type::c_rnti_f1_1: {
      const auto& dci = pdcch.dci.as_c_rnti_f1_1();
      harq_id         = static_cast<uint8_t>(dci.harq_process_number);
      ndi             = dci.tb1_new_data_indicator;
      rv              = static_cast<uint8_t>(dci.tb1_redundancy_version);
      mcs             = static_cast<uint8_t>(dci.tb1_modulation_coding_scheme);
      pucch_res_ind   = static_cast<uint8_t>(dci.pucch_resource_indicator);
    } break;
  }
  return {to_value(pdcch.ctx.rnti),
          static_cast<uint8_t>(pdcch.dci.type()),
          static_cast<uint8_t>(pdcch.ctx.coreset_cfg->get_id()),
          static_cast<uint8_t>(pdcch.ctx.context.ss_id),
          static_cast<uint8_t>(pdcch.ctx.cces.ncce),
          static_cast<uint8_t>(pdcch.ctx.cces.aggr_lvl),
          harq_id,
          ndi,
          rv,
          mcs,
          pucch_res_ind};
}

void schedtrace::convert_fb_to_dl_pdcch(pdcch_dl_information& pdcch, const fbs::DlPdcch& fb)
{
  pdcch.ctx.coreset_cfg   = &get_stub_coreset_cfg(fb.coreset_id());
  pdcch.ctx.rnti          = to_rnti(fb.rnti());
  pdcch.ctx.context.ss_id = to_search_space_id(fb.ss_id());
  pdcch.ctx.cces.ncce     = fb.cce();
  pdcch.ctx.cces.aggr_lvl = static_cast<aggregation_level>(fb.aggr_lvl());
  switch (static_cast<dci_dl_rnti_config_type>(fb.dci_type())) {
    case dci_dl_rnti_config_type::si_f1_0: {
      auto& dci_cfg                    = pdcch.dci.set_si_rnti_f1_0();
      dci_cfg.redundancy_version       = fb.rv();
      dci_cfg.modulation_coding_scheme = fb.mcs();
    } break;
    case dci_dl_rnti_config_type::ra_f1_0: {
      pdcch.dci.set_ra_rnti_f1_0().modulation_coding_scheme = fb.mcs();
    } break;
    case dci_dl_rnti_config_type::p_rnti_f1_0: {
      pdcch.dci.set_p_rnti_f1_0().modulation_coding_scheme = fb.mcs();
    } break;
    case dci_dl_rnti_config_type::c_rnti_f1_0: {
      auto& dci_cfg                    = pdcch.dci.set_c_rnti_f1_0();
      dci_cfg.harq_process_number      = fb.harq_id();
      dci_cfg.new_data_indicator       = fb.ndi();
      dci_cfg.redundancy_version       = fb.rv();
      dci_cfg.modulation_coding_scheme = fb.mcs();
      dci_cfg.pucch_resource_indicator = fb.pucch_res_ind();
    } break;
    case dci_dl_rnti_config_type::tc_rnti_f1_0: {
      auto& dci_cfg                    = pdcch.dci.set_tc_rnti_f1_0();
      dci_cfg.harq_process_number      = fb.harq_id();
      dci_cfg.new_data_indicator       = fb.ndi();
      dci_cfg.redundancy_version       = fb.rv();
      dci_cfg.modulation_coding_scheme = fb.mcs();
      dci_cfg.pucch_resource_indicator = fb.pucch_res_ind();
    } break;
    case dci_dl_rnti_config_type::c_rnti_f1_1: {
      auto& dci_cfg                        = pdcch.dci.set_c_rnti_f1_1();
      dci_cfg.harq_process_number          = fb.harq_id();
      dci_cfg.tb1_new_data_indicator       = fb.ndi();
      dci_cfg.tb1_redundancy_version       = fb.rv();
      dci_cfg.tb1_modulation_coding_scheme = fb.mcs();
      dci_cfg.pucch_resource_indicator     = fb.pucch_res_ind();
    } break;
  }
}

fbs::UlPdcch schedtrace::convert_ul_pdcch_to_fb(const pdcch_ul_information& pdcch)
{
  // Only the DCI fields the wire format carries are extracted; a field absent from the active DCI type stays 0.
  uint8_t harq_id = 0;
  bool    ndi     = false;
  uint8_t rv      = 0;
  uint8_t mcs     = 0;
  switch (pdcch.dci.type()) {
    case dci_ul_rnti_config_type::tc_rnti_f0_0: {
      const auto& dci = pdcch.dci.as_tc_rnti_f0_0();
      rv              = static_cast<uint8_t>(dci.redundancy_version);
      mcs             = static_cast<uint8_t>(dci.modulation_coding_scheme);
    } break;
    case dci_ul_rnti_config_type::c_rnti_f0_0: {
      const auto& dci = pdcch.dci.as_c_rnti_f0_0();
      harq_id         = static_cast<uint8_t>(dci.harq_process_number);
      ndi             = dci.new_data_indicator;
      rv              = static_cast<uint8_t>(dci.redundancy_version);
      mcs             = static_cast<uint8_t>(dci.modulation_coding_scheme);
    } break;
    case dci_ul_rnti_config_type::c_rnti_f0_1: {
      const auto& dci = pdcch.dci.as_c_rnti_f0_1();
      harq_id         = static_cast<uint8_t>(dci.harq_process_number);
      ndi             = dci.new_data_indicator;
      rv              = static_cast<uint8_t>(dci.redundancy_version);
      mcs             = static_cast<uint8_t>(dci.modulation_coding_scheme);
    } break;
  }
  return {to_value(pdcch.ctx.rnti),
          static_cast<uint8_t>(pdcch.dci.type()),
          static_cast<uint8_t>(pdcch.ctx.coreset_cfg->get_id()),
          static_cast<uint8_t>(pdcch.ctx.context.ss_id),
          static_cast<uint8_t>(pdcch.ctx.cces.ncce),
          static_cast<uint8_t>(pdcch.ctx.cces.aggr_lvl),
          harq_id,
          ndi,
          rv,
          mcs};
}

void schedtrace::convert_fb_to_ul_pdcch(pdcch_ul_information& pdcch, const fbs::UlPdcch& fb)
{
  pdcch.ctx.coreset_cfg   = &get_stub_coreset_cfg(fb.coreset_id());
  pdcch.ctx.rnti          = to_rnti(fb.rnti());
  pdcch.ctx.context.ss_id = to_search_space_id(fb.ss_id());
  pdcch.ctx.cces.ncce     = fb.cce();
  pdcch.ctx.cces.aggr_lvl = static_cast<aggregation_level>(fb.aggr_lvl());
  switch (static_cast<dci_ul_rnti_config_type>(fb.dci_type())) {
    case dci_ul_rnti_config_type::tc_rnti_f0_0: {
      auto& dci_cfg                    = pdcch.dci.set_tc_rnti_f0_0();
      dci_cfg.redundancy_version       = fb.rv();
      dci_cfg.modulation_coding_scheme = fb.mcs();
    } break;
    case dci_ul_rnti_config_type::c_rnti_f0_0: {
      auto& dci_cfg                    = pdcch.dci.set_c_rnti_f0_0();
      dci_cfg.harq_process_number      = fb.harq_id();
      dci_cfg.new_data_indicator       = fb.ndi();
      dci_cfg.redundancy_version       = fb.rv();
      dci_cfg.modulation_coding_scheme = fb.mcs();
    } break;
    case dci_ul_rnti_config_type::c_rnti_f0_1: {
      auto& dci_cfg                    = pdcch.dci.set_c_rnti_f0_1();
      dci_cfg.harq_process_number      = fb.harq_id();
      dci_cfg.new_data_indicator       = fb.ndi();
      dci_cfg.redundancy_version       = fb.rv();
      dci_cfg.modulation_coding_scheme = fb.mcs();
    } break;
  }
}

fbs::Prach schedtrace::convert_prach_to_fb(const prach_occasion_info& prach)
{
  return {prach.pci,
          static_cast<uint8_t>(prach.format),
          prach.nof_prach_occasions,
          prach.index_fd_ra,
          prach.start_symbol,
          prach.nof_cs,
          prach.nof_fd_ra,
          prach.start_preamble_index,
          prach.nof_preamble_indexes};
}

void schedtrace::convert_fb_to_prach(prach_occasion_info& prach, const fbs::Prach& fb)
{
  prach.pci                  = fb.pci();
  prach.format               = static_cast<prach_format_type>(fb.format());
  prach.nof_prach_occasions  = fb.nof_occasions();
  prach.index_fd_ra          = fb.index_fd_ra();
  prach.start_symbol         = fb.start_symbol();
  prach.nof_cs               = fb.nof_cs();
  prach.nof_fd_ra            = fb.nof_fd_ra();
  prach.start_preamble_index = fb.start_preamble_index();
  prach.nof_preamble_indexes = fb.nof_preamble_indexes();
}

fbs::FailedAttempts schedtrace::convert_failed_attempts_to_fb(const failed_alloc_attempts& failed_attempts)
{
  return {failed_attempts.dl_pdcch,
          failed_attempts.ul_pdcch,
          failed_attempts.common_dl_pdcch,
          failed_attempts.common_ul_pdcch,
          failed_attempts.uci,
          failed_attempts.fallback_uci_allocs};
}

void schedtrace::convert_fb_to_failed_attempts(failed_alloc_attempts& failed_attempts, const fbs::FailedAttempts& fb)
{
  failed_attempts.dl_pdcch            = fb.dl_pdcch();
  failed_attempts.ul_pdcch            = fb.ul_pdcch();
  failed_attempts.common_dl_pdcch     = fb.common_dl_pdcch();
  failed_attempts.common_ul_pdcch     = fb.common_ul_pdcch();
  failed_attempts.uci                 = fb.uci();
  failed_attempts.fallback_uci_allocs = fb.fallback_uci_allocs();
}

flatbuffers::Offset<fbs::SlotDecision> schedtrace::convert_decision_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                          const sched_result&             result)
{
  static_vector<flatbuffers::Offset<fbs::SibPdsch>, MAX_SI_PDUS_PER_SLOT> sib_offs;
  for (const auto& sib : result.dl.bc.sibs) {
    sib_offs.push_back(convert_sib_pdsch_to_fb(fbb, sib));
  }

  static_vector<flatbuffers::Offset<fbs::RarPdsch>, MAX_RAR_PDUS_PER_SLOT> rar_offs;
  for (const auto& rar : result.dl.rar_grants) {
    rar_offs.push_back(convert_rar_pdsch_to_fb(fbb, rar));
  }

  static_vector<flatbuffers::Offset<fbs::PagingPdsch>, MAX_PAGING_PDUS_PER_SLOT> paging_offs;
  for (const auto& pg : result.dl.paging_grants) {
    paging_offs.push_back(convert_paging_pdsch_to_fb(fbb, pg));
  }

  static_vector<flatbuffers::Offset<fbs::Pdsch>, MAX_UE_PDUS_PER_SLOT> pdsch_offs;
  for (const auto& pdsch_alloc : result.dl.ue_grants) {
    pdsch_offs.push_back(convert_pdsch_to_fb(fbb, pdsch_alloc));
  }

  static_vector<flatbuffers::Offset<fbs::Pusch>, MAX_PUSCH_PDUS_PER_SLOT> pusch_offs;
  for (const auto& pusch : result.ul.puschs) {
    pusch_offs.push_back(convert_pusch_to_fb(fbb, pusch));
  }

  static_vector<flatbuffers::Offset<fbs::Pucch>, MAX_PUCCH_PDUS_PER_SLOT> pucch_offs;
  for (const auto& pucch : result.ul.pucchs) {
    pucch_offs.push_back(convert_pucch_info_to_fb(fbb, pucch));
  }

  std::array<fbs::DlPdcch, MAX_DL_PDCCH_PDUS_PER_SLOT> dl_pdcchs;
  unsigned                                             nof_dl_pdcchs = 0;
  for (const auto& pdcch : result.dl.dl_pdcchs) {
    dl_pdcchs[nof_dl_pdcchs++] = convert_dl_pdcch_to_fb(pdcch);
  }

  std::array<fbs::UlPdcch, MAX_UL_PDCCH_PDUS_PER_SLOT> ul_pdcchs;
  unsigned                                             nof_ul_pdcchs = 0;
  for (const auto& pdcch : result.dl.ul_pdcchs) {
    ul_pdcchs[nof_ul_pdcchs++] = convert_ul_pdcch_to_fb(pdcch);
  }

  std::array<fbs::Ssb, MAX_SSB_PER_SLOT> ssbs;
  unsigned                               nof_ssbs = 0;
  for (const auto& ssb : result.dl.bc.ssb_info) {
    ssbs[nof_ssbs++] = convert_ssb_to_fb(ssb);
  }

  std::array<fbs::CsiRs, MAX_CSI_RS_PDUS_PER_SLOT> csi_rss;
  unsigned                                         nof_csi_rss = 0;
  for (const auto& csi : result.dl.csi_rs) {
    csi_rss[nof_csi_rss++] = convert_csi_rs_to_fb(csi);
  }

  std::array<fbs::Prach, MAX_PRACH_OCCASIONS_PER_SLOT> prachs;
  unsigned                                             nof_prachs = 0;
  for (const auto& prach : result.ul.prachs) {
    prachs[nof_prachs++] = convert_prach_to_fb(prach);
  }

  std::array<fbs::Srs, MAX_SRS_PDUS_PER_SLOT> srss;
  unsigned                                    nof_srss = 0;
  for (const auto& srs : result.ul.srss) {
    srss[nof_srss++] = convert_srs_to_fb(srs);
  }

  const bool any_failed_attempts = result.failed_attempts.dl_pdcch != 0 || result.failed_attempts.ul_pdcch != 0 ||
                                   result.failed_attempts.common_dl_pdcch != 0 ||
                                   result.failed_attempts.common_ul_pdcch != 0 || result.failed_attempts.uci != 0 ||
                                   result.failed_attempts.fallback_uci_allocs != 0;
  const fbs::FailedAttempts failed_attempts = convert_failed_attempts_to_fb(result.failed_attempts);

  // All child tables are built; create the vectors and then the decision table.
  const auto dl_pdcchs_vec = nof_dl_pdcchs == 0 ? 0 : fbb.CreateVectorOfStructs(dl_pdcchs.data(), nof_dl_pdcchs);
  const auto ul_pdcchs_vec = nof_ul_pdcchs == 0 ? 0 : fbb.CreateVectorOfStructs(ul_pdcchs.data(), nof_ul_pdcchs);
  const auto sibs_vec      = sib_offs.empty() ? 0 : fbb.CreateVector(sib_offs.data(), sib_offs.size());
  const auto rars_vec      = rar_offs.empty() ? 0 : fbb.CreateVector(rar_offs.data(), rar_offs.size());
  const auto pagings_vec   = paging_offs.empty() ? 0 : fbb.CreateVector(paging_offs.data(), paging_offs.size());
  const auto pdschs_vec    = pdsch_offs.empty() ? 0 : fbb.CreateVector(pdsch_offs.data(), pdsch_offs.size());
  const auto puschs_vec    = pusch_offs.empty() ? 0 : fbb.CreateVector(pusch_offs.data(), pusch_offs.size());
  const auto pucchs_vec    = pucch_offs.empty() ? 0 : fbb.CreateVector(pucch_offs.data(), pucch_offs.size());
  const auto ssbs_vec      = nof_ssbs == 0 ? 0 : fbb.CreateVectorOfStructs(ssbs.data(), nof_ssbs);
  const auto csi_rs_vec    = nof_csi_rss == 0 ? 0 : fbb.CreateVectorOfStructs(csi_rss.data(), nof_csi_rss);
  const auto prachs_vec    = nof_prachs == 0 ? 0 : fbb.CreateVectorOfStructs(prachs.data(), nof_prachs);
  const auto srss_vec      = nof_srss == 0 ? 0 : fbb.CreateVectorOfStructs(srss.data(), nof_srss);

  return fbs::CreateSlotDecision(fbb,
                                 dl_pdcchs_vec,
                                 ul_pdcchs_vec,
                                 sibs_vec,
                                 rars_vec,
                                 pagings_vec,
                                 pdschs_vec,
                                 puschs_vec,
                                 pucchs_vec,
                                 ssbs_vec,
                                 csi_rs_vec,
                                 prachs_vec,
                                 srss_vec,
                                 any_failed_attempts ? &failed_attempts : nullptr);
}

void schedtrace::convert_fb_to_decision(sched_result&             result,
                                        const fbs::SlotDecision&  decision,
                                        const cell_configuration& cell_cfg)
{
  if (decision.sib_pdschs() != nullptr) {
    for (const fbs::SibPdsch* sib_ev : *decision.sib_pdschs()) {
      sib_information sib{};
      convert_fb_to_sib_pdsch(sib, *sib_ev, &cell_cfg.init_dl_bwp);
      result.dl.bc.sibs.push_back(sib);
    }
  }

  if (decision.rar_pdschs() != nullptr) {
    for (const fbs::RarPdsch* rar_ev : *decision.rar_pdschs()) {
      rar_information rar{};
      convert_fb_to_rar_pdsch(rar, *rar_ev, &cell_cfg.init_dl_bwp);
      result.dl.rar_grants.push_back(rar);
    }
  }

  if (decision.paging_pdschs() != nullptr) {
    for (const fbs::PagingPdsch* pg_ev : *decision.paging_pdschs()) {
      dl_paging_allocation pg{};
      convert_fb_to_paging_pdsch(pg, *pg_ev, &cell_cfg.init_dl_bwp);
      result.dl.paging_grants.push_back(pg);
    }
  }

  if (decision.ue_pdschs() != nullptr) {
    for (const fbs::Pdsch* pdsch_ev : *decision.ue_pdschs()) {
      dl_msg_alloc alloc{};
      convert_fb_to_pdsch(alloc, *pdsch_ev, &cell_cfg.init_dl_bwp);
      result.dl.ue_grants.push_back(alloc);
    }
  }

  if (decision.puschs() != nullptr) {
    for (const fbs::Pusch* pusch_ev : *decision.puschs()) {
      ul_sched_info alloc{};
      convert_fb_to_pusch(alloc, *pusch_ev, &cell_cfg.init_ul_bwp);
      result.ul.puschs.push_back(alloc);
    }
  }

  if (decision.pucchs() != nullptr) {
    for (const fbs::Pucch* pucch_ev : *decision.pucchs()) {
      pucch_info alloc{};
      convert_fb_to_pucch_info(alloc, *pucch_ev, &cell_cfg.init_ul_bwp, cell_cfg.pucch_resources);
      result.ul.pucchs.emplace(alloc);
    }
  }

  if (decision.dl_pdcchs() != nullptr) {
    for (const fbs::DlPdcch* pdcch_ev : *decision.dl_pdcchs()) {
      pdcch_dl_information pdcch{};
      convert_fb_to_dl_pdcch(pdcch, *pdcch_ev);
      result.dl.dl_pdcchs.push_back(pdcch);
    }
  }

  if (decision.ul_pdcchs() != nullptr) {
    for (const fbs::UlPdcch* pdcch_ev : *decision.ul_pdcchs()) {
      pdcch_ul_information pdcch{};
      convert_fb_to_ul_pdcch(pdcch, *pdcch_ev);
      result.dl.ul_pdcchs.push_back(pdcch);
    }
  }

  if (decision.failed_attempts() != nullptr) {
    convert_fb_to_failed_attempts(result.failed_attempts, *decision.failed_attempts());
  }

  if (decision.ssbs() != nullptr) {
    for (const fbs::Ssb* ssb_ev : *decision.ssbs()) {
      ssb_information ssb;
      convert_fb_to_ssb(ssb, *ssb_ev);
      result.dl.bc.ssb_info.push_back(ssb);
    }
  }

  if (decision.csi_rs() != nullptr) {
    for (const fbs::CsiRs* csi_ev : *decision.csi_rs()) {
      csi_rs_info csi;
      convert_fb_to_csi_rs(csi, *csi_ev, &cell_cfg.init_dl_bwp);
      result.dl.csi_rs.push_back(csi);
    }
  }

  if (decision.prachs() != nullptr) {
    for (const fbs::Prach* prach_ev : *decision.prachs()) {
      prach_occasion_info prach;
      convert_fb_to_prach(prach, *prach_ev);
      result.ul.prachs.push_back(prach);
    }
  }

  if (decision.srss() != nullptr) {
    for (const fbs::Srs* srs_ev : *decision.srss()) {
      srs_info srs;
      convert_fb_to_srs(srs, *srs_ev, &cell_cfg.init_ul_bwp);
      result.ul.srss.push_back(srs);
    }
  }
}

flatbuffers::Offset<fbs::RachIndication>
schedtrace::convert_rach_indication_to_fb(flatbuffers::FlatBufferBuilder& fbb, const rach_indication_message& rach_ind)
{
  static_vector<flatbuffers::Offset<fbs::RachOccasion>, MAX_PRACH_OCCASIONS_PER_SLOT> occ_offs;
  for (const auto& occ_msg : rach_ind.occasions) {
    std::array<fbs::RachPreamble, MAX_PREAMBLES_PER_PRACH_OCCASION> preambles;
    unsigned                                                        nof_preambles = 0;
    for (const auto& preamble_msg : occ_msg.preambles) {
      preambles[nof_preambles++] = fbs::RachPreamble(
          preamble_msg.preamble_id, to_value(preamble_msg.tc_rnti), preamble_msg.time_advance.to_Tc());
    }
    const auto preambles_vec = nof_preambles == 0 ? 0 : fbb.CreateVectorOfStructs(preambles.data(), nof_preambles);
    occ_offs.push_back(fbs::CreateRachOccasion(fbb, occ_msg.start_symbol, occ_msg.frequency_index, preambles_vec));
  }
  const auto occasions_vec = occ_offs.empty() ? 0 : fbb.CreateVector(occ_offs.data(), occ_offs.size());
  return fbs::CreateRachIndication(fbb, rach_ind.slot_rx.count(), occasions_vec);
}

void schedtrace::convert_fb_to_rach_indication(rach_indication_message&   rach_ind,
                                               const fbs::RachIndication& input,
                                               subcarrier_spacing         scs,
                                               du_cell_index_t            cell_index)
{
  rach_ind.cell_index = cell_index;
  rach_ind.slot_rx    = slot_point(scs, input.slot_rx());
  if (input.occasions() == nullptr) {
    return;
  }
  for (const fbs::RachOccasion* occ : *input.occasions()) {
    rach_indication_message::occasion occ_msg;
    occ_msg.start_symbol    = occ->start_symbol();
    occ_msg.frequency_index = occ->freq_idx();
    if (occ->preambles() != nullptr) {
      for (const fbs::RachPreamble* preamble : *occ->preambles()) {
        rach_indication_message::preamble preamble_msg;
        preamble_msg.preamble_id  = preamble->preamble_id();
        preamble_msg.tc_rnti      = to_rnti(preamble->tc_rnti());
        preamble_msg.time_advance = phy_time_unit::from_units_of_Tc(preamble->time_advance());
        occ_msg.preambles.push_back(preamble_msg);
      }
    }
    rach_ind.occasions.push_back(occ_msg);
  }
}

flatbuffers::Offset<fbs::HarqAckEvent> schedtrace::convert_harq_ack_event_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                                const harq_ack_event&           event)
{
  return fbs::CreateHarqAckEvent(fbb,
                                 event.sl_ack_rx.count(),
                                 static_cast<uint16_t>(event.ue_index),
                                 to_value(event.rnti),
                                 static_cast<uint8_t>(event.h_id),
                                 static_cast<fbs::HarqAckReportStatus>(event.ack),
                                 static_cast<uint32_t>(event.tbs.value()));
}

void schedtrace::convert_fb_to_harq_ack_event(harq_ack_event&          event,
                                              const fbs::HarqAckEvent& input,
                                              subcarrier_spacing       scs,
                                              du_cell_index_t          cell_index)
{
  event.cell_index = cell_index;
  event.sl_ack_rx  = slot_point(scs, input.slot_rx());
  event.ue_index   = to_du_ue_index(input.ue_idx());
  event.rnti       = to_rnti(input.rnti());
  event.h_id       = to_harq_id(input.harq_id());
  event.ack        = static_cast<mac_harq_ack_report_status>(input.status());
  event.tbs        = units::bytes{input.tbs()};
}

flatbuffers::Offset<fbs::SrEvent> schedtrace::convert_sr_event_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                     const sr_event&                 event)
{
  return fbs::CreateSrEvent(fbb, 0, static_cast<uint16_t>(event.ue_index), to_value(event.rnti));
}

void schedtrace::convert_fb_to_sr_event(sr_event& event, const fbs::SrEvent& input)
{
  event.ue_index = to_du_ue_index(input.ue_idx());
  event.rnti     = to_rnti(input.rnti());
}

namespace {

/// Union field of a built PmiValue: the member type and its offset.
struct pmi_offset {
  fbs::PmiValue             type = fbs::PmiValue::NONE;
  flatbuffers::Offset<void> value;
};

/// Converts an optional native integer to its optional wire counterpart.
template <typename WireType, typename T>
flatbuffers::Optional<WireType> to_fb_optional(const std::optional<T>& value)
{
  if (!value.has_value()) {
    return flatbuffers::nullopt;
  }
  if constexpr (std::is_integral_v<T>) {
    return flatbuffers::Optional<WireType>{static_cast<WireType>(*value)};
  } else {
    // bounded_integer and other wrappers around an integer.
    return flatbuffers::Optional<WireType>{static_cast<WireType>(value->value())};
  }
}

/// Converts a list of integer-like values to a vector of \c uint8_t. Kept empty-safe: an empty input still yields a
/// present (zero-length) vector, so an empty reported list is not confused with an absent one.
template <typename Container>
flatbuffers::Offset<flatbuffers::Vector<uint8_t>> to_fb_byte_vector(flatbuffers::FlatBufferBuilder& fbb,
                                                                    const Container&                values)
{
  static_vector<uint8_t, csi_max_nof_subbands> bytes;
  for (const auto& value : values) {
    if constexpr (std::is_integral_v<std::decay_t<decltype(value)>>) {
      bytes.push_back(static_cast<uint8_t>(value));
    } else {
      bytes.push_back(static_cast<uint8_t>(value.value()));
    }
  }
  return fbb.CreateVector(bytes.data(), bytes.size());
}

/// Reads back a vector of \c uint8_t into a list of integer-like values.
template <typename Container>
void from_fb_byte_vector(Container& out, const flatbuffers::Vector<uint8_t>& values)
{
  out.clear();
  for (uint8_t value : values) {
    out.push_back(typename Container::value_type{value});
  }
}

pmi_offset convert_pmi_to_fb(flatbuffers::FlatBufferBuilder& fbb, const precoding_matrix_indicator& pmi)
{
  if (std::holds_alternative<std::monostate>(pmi)) {
    return {fbs::PmiValue::PmiUnknown, fbs::CreatePmiUnknown(fbb).Union()};
  }
  if (const auto* pmi_2_port = std::get_if<pmi_two_antenna_port>(&pmi)) {
    return {fbs::PmiValue::PmiTwoAntennaPorts, fbs::CreatePmiTwoAntennaPorts(fbb, pmi_2_port->pmi).Union()};
  }
  if (const auto* pmi_4_port = std::get_if<pmi_typeI_single_panel>(&pmi)) {
    return {fbs::PmiValue::PmiTypeISinglePanel,
            fbs::CreatePmiTypeISinglePanel(fbb,
                                           static_cast<uint8_t>(pmi_4_port->panel_config.n1_n2),
                                           static_cast<uint8_t>(pmi_4_port->panel_config.mode),
                                           pmi_4_port->i_1_1,
                                           to_fb_optional<uint8_t>(pmi_4_port->i_1_2),
                                           to_fb_optional<uint8_t>(pmi_4_port->i_1_3),
                                           pmi_4_port->i_2)
                .Union()};
  }

  const auto&                                                pmi_typeII_val = std::get<pmi_typeII>(pmi);
  static_vector<flatbuffers::Offset<fbs::PmiTypeIILayer>, 2> layer_offs;
  for (const pmi_typeII::layer_coefficients& layer : pmi_typeII_val.layers) {
    const auto i_1_4 = to_fb_byte_vector(fbb, layer.i_1_4);
    const auto i_2_1 = to_fb_byte_vector(fbb, layer.i_2_1);
    const auto i_2_2 = to_fb_byte_vector(fbb, layer.i_2_2);
    layer_offs.push_back(fbs::CreatePmiTypeIILayer(fbb, layer.i_1_3, i_1_4, i_2_1, i_2_2));
  }
  const auto layers_vec = fbb.CreateVector(layer_offs.data(), layer_offs.size());

  return {fbs::PmiValue::PmiTypeII,
          fbs::CreatePmiTypeII(fbb,
                               static_cast<uint8_t>(pmi_typeII_val.config.n1_n2),
                               pmi_typeII_val.config.nof_beams.value(),
                               static_cast<uint8_t>(pmi_typeII_val.config.phase_alphabet_size),
                               pmi_typeII_val.config.subband_amplitude,
                               pmi_typeII_val.i_1_1,
                               pmi_typeII_val.i_1_2,
                               layers_vec)
              .Union()};
}

std::optional<precoding_matrix_indicator> convert_fb_to_pmi(const fbs::CsiReportEvent& input)
{
  switch (input.pmi_type()) {
    case fbs::PmiValue::PmiUnknown:
      return precoding_matrix_indicator{std::monostate{}};
    case fbs::PmiValue::PmiTwoAntennaPorts:
      return precoding_matrix_indicator{pmi_two_antenna_port{.pmi = input.pmi_as_PmiTwoAntennaPorts()->pmi()}};
    case fbs::PmiValue::PmiTypeISinglePanel: {
      const fbs::PmiTypeISinglePanel& pmi_4_port = *input.pmi_as_PmiTypeISinglePanel();
      return precoding_matrix_indicator{pmi_typeI_single_panel{
          .panel_config = {.n1_n2 = static_cast<pmi_codebook_single_panel_config>(pmi_4_port.n1_n2()),
                           .mode  = static_cast<pmi_codebook_typeI_mode>(pmi_4_port.mode())},
          .i_1_1        = pmi_4_port.i_1_1(),
          .i_1_2 = pmi_4_port.i_1_2().has_value() ? std::optional<uint8_t>{pmi_4_port.i_1_2().value()} : std::nullopt,
          .i_1_3 = pmi_4_port.i_1_3().has_value() ? std::optional<uint8_t>{pmi_4_port.i_1_3().value()} : std::nullopt,
          .i_2   = pmi_4_port.i_2()}};
    }
    case fbs::PmiValue::PmiTypeII: {
      const fbs::PmiTypeII& fb_typeII = *input.pmi_as_PmiTypeII();
      pmi_typeII            result{.config = {.n1_n2 = static_cast<pmi_codebook_single_panel_config>(fb_typeII.n1_n2()),
                                              .nof_beams = bounded_integer<unsigned, 2, 4>{fb_typeII.nof_beams()},
                                              .phase_alphabet_size =
                                                  static_cast<pmi_codebook_typeII_phase_size>(fb_typeII.phase_alphabet_size()),
                                              .subband_amplitude = fb_typeII.subband_amplitude()},
                                   .i_1_1  = fb_typeII.i_1_1(),
                                   .i_1_2  = fb_typeII.i_1_2()};
      if (fb_typeII.layers() != nullptr) {
        for (const fbs::PmiTypeIILayer* fb_layer : *fb_typeII.layers()) {
          pmi_typeII::layer_coefficients layer{.i_1_3 = fb_layer->i_1_3()};
          from_fb_byte_vector(layer.i_1_4, *fb_layer->i_1_4());
          from_fb_byte_vector(layer.i_2_1, *fb_layer->i_2_1());
          from_fb_byte_vector(layer.i_2_2, *fb_layer->i_2_2());
          result.layers.push_back(layer);
        }
      }
      return precoding_matrix_indicator{result};
    }
    default:
      return std::nullopt;
  }
}

} // namespace

flatbuffers::Offset<fbs::CsiReportEvent> schedtrace::convert_csi_report_event_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                                    const csi_report_event& event)
{
  const csi_report_data& csi = event.csi;

  const pmi_offset pmi = csi.pmi.has_value() ? convert_pmi_to_fb(fbb, *csi.pmi) : pmi_offset{};

  const auto cri_vec  = to_fb_byte_vector(fbb, csi.cri);
  const auto rsrp_vec = fbb.CreateVector(csi.rsrp_dBm.data(), csi.rsrp_dBm.size());
  const auto first_tb_subband_diff_cqi_vec =
      csi.first_tb_subband_diff_cqi.has_value() ? to_fb_byte_vector(fbb, *csi.first_tb_subband_diff_cqi) : 0;
  const auto second_tb_subband_diff_cqi_vec =
      csi.second_tb_subband_diff_cqi.has_value() ? to_fb_byte_vector(fbb, *csi.second_tb_subband_diff_cqi) : 0;

  return fbs::CreateCsiReportEvent(fbb,
                                   event.sl_rx.count(),
                                   static_cast<uint16_t>(event.ue_index),
                                   to_value(event.rnti),
                                   cri_vec,
                                   rsrp_vec,
                                   to_fb_optional<uint8_t>(csi.ri),
                                   to_fb_optional<uint8_t>(csi.li),
                                   pmi.type,
                                   pmi.value,
                                   to_fb_optional<uint8_t>(csi.first_tb_wideband_cqi),
                                   to_fb_optional<uint8_t>(csi.second_tb_wideband_cqi),
                                   first_tb_subband_diff_cqi_vec,
                                   second_tb_subband_diff_cqi_vec,
                                   csi.valid);
}

void schedtrace::convert_fb_to_csi_report_event(csi_report_event&          event,
                                                const fbs::CsiReportEvent& input,
                                                subcarrier_spacing         scs)
{
  event.sl_rx    = slot_point(scs, input.slot_rx());
  event.ue_index = to_du_ue_index(input.ue_idx());
  event.rnti     = to_rnti(input.rnti());

  event.csi            = csi_report_data{};
  csi_report_data& csi = event.csi;
  if (input.cri() != nullptr) {
    from_fb_byte_vector(csi.cri, *input.cri());
  }
  if (input.rsrp_dbm() != nullptr) {
    csi.rsrp_dBm.assign(input.rsrp_dbm()->begin(), input.rsrp_dbm()->end());
  }
  if (input.ri().has_value()) {
    csi.ri = csi_report_data::ri_type{input.ri().value()};
  }
  if (input.li().has_value()) {
    csi.li = csi_report_data::li_type{input.li().value()};
  }
  csi.pmi = convert_fb_to_pmi(input);
  if (input.first_tb_wb_cqi().has_value()) {
    csi.first_tb_wideband_cqi = csi_report_data::wideband_cqi_type{input.first_tb_wb_cqi().value()};
  }
  if (input.second_tb_wb_cqi().has_value()) {
    csi.second_tb_wideband_cqi = csi_report_data::wideband_cqi_type{input.second_tb_wb_cqi().value()};
  }
  if (input.first_tb_subband_diff_cqi() != nullptr) {
    from_fb_byte_vector(csi.first_tb_subband_diff_cqi.emplace(), *input.first_tb_subband_diff_cqi());
  }
  if (input.second_tb_subband_diff_cqi() != nullptr) {
    from_fb_byte_vector(csi.second_tb_subband_diff_cqi.emplace(), *input.second_tb_subband_diff_cqi());
  }
  csi.valid = input.valid();
}
