// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pusch.h"
#include "ocudu/fapi_adaptor/uci_part2_correspondence_mapper.h"
#include "ocudu/ran/sch/sch_constants.h"
#include "ocudu/scheduler/result/pusch_info.h"

using namespace ocudu;
using namespace fapi_adaptor;

/// Fill the optional UCI parameters.
static void fill_optional_uci_parameters(fapi::ul_pusch_pdu_builder&      builder,
                                         const std::optional<uci_info>&   uci,
                                         uci_part2_correspondence_mapper& part2_mapper)
{
  if (!uci) {
    return;
  }

  units::bits       harq_ack_bit{0U};
  units::bits       csi_part1_bit{0U};
  alpha_scaling_opt alpha_scaling = uci->alpha;
  uint8_t           beta_offset_harq_ack{0U};
  uint8_t           beta_offset_csi1{0U};
  uint8_t           beta_offset_csi2{0U};

  if (uci->harq) {
    const uci_info::harq_info& harq = *uci->harq;
    harq_ack_bit                    = units::bits(harq.harq_ack_nof_bits);
    beta_offset_harq_ack            = harq.beta_offset_harq_ack;
  }

  if (uci->csi) {
    const uci_info::csi_info& csi = *uci->csi;
    csi_part1_bit                 = units::bits(csi.csi_part1_nof_bits);
    beta_offset_csi1              = csi.beta_offset_csi_1;

    // No CSI Part2 for a single antenna.
    if (csi.beta_offset_csi_2 && !std::holds_alternative<pmi_codebook_one_port>(csi.csi_rep_cfg.pmi_codebook)) {
      span<const uci_part2_correspondence_information> uci_correspondence = part2_mapper.map(csi.csi_rep_cfg);
      if (!uci_correspondence.empty()) {
        beta_offset_csi2 = *csi.beta_offset_csi_2;

        // Build UCI Part2 correspondence.
        for (const auto& part2 : uci_correspondence) {
          builder.add_uci_part1_part2_correspondence(
              part2.part1_param_offsets, part2.part1_param_sizes, part2.part2_map_index);
        }
      }
    }
  }

  builder.add_optional_pusch_uci(
      harq_ack_bit, csi_part1_bit, alpha_scaling, beta_offset_harq_ack, beta_offset_csi1, beta_offset_csi2);
}

void ocudu::fapi_adaptor::convert_pusch_mac_to_fapi(fapi::ul_pusch_pdu_builder&      builder,
                                                    const ul_sched_info&             mac_pdu,
                                                    uci_part2_correspondence_mapper& part2_mapper)
{
  const pusch_information& pusch_pdu = mac_pdu.pusch_cfg;
  builder.set_ue_specific_parameters(pusch_pdu.rnti);

  const bwp_configuration& bwp_cfg = *pusch_pdu.bwp_cfg;
  builder.set_bwp_parameters(bwp_cfg.crbs, bwp_cfg.scs, bwp_cfg.cp);

  builder.set_information_parameters(pusch_pdu.mcs_descr.target_code_rate,
                                     pusch_pdu.mcs_descr.modulation,
                                     pusch_pdu.mcs_index.value(),
                                     pusch_pdu.mcs_table,
                                     pusch_pdu.n_id,
                                     pusch_pdu.nof_layers);

  const dmrs_information& dmrs_cfg = pusch_pdu.dmrs;
  if (pusch_pdu.transform_precoding) {
    builder.set_transform_precoding_enabled_parameters(dmrs_cfg.dmrs_scrambling_id);
  } else {
    builder.set_transform_precoding_disabled_parameters(dmrs_cfg.num_dmrs_cdm_grps_no_data);
  }
  builder.set_dmrs_parameters(
      dmrs_cfg.dmrs_symb_pos, dmrs_cfg.config_type, pusch_pdu.pusch_dmrs_id, dmrs_cfg.n_scid, dmrs_cfg.dmrs_ports);

  const vrb_alloc& rbs = pusch_pdu.rbs;

  report_error_if_not(!rbs.is_type0(), "PUSCH resource type allocation type 0 is not supported");

  builder.set_time_allocation_parameters(pusch_pdu.symbols);

  // Sending data through PUSCH is optional, but for now, the MAC does not signal this, use the TB size to catch it.
  ocudu_assert(pusch_pdu.tb_size_bytes.value(), "Transport block of null size");

  // Add PUSCH Data.
  builder.add_optional_pusch_data(
      pusch_pdu.rv_index, pusch_pdu.harq_id, pusch_pdu.new_data, units::bytes{pusch_pdu.tb_size_bytes});

  // NOTE: MAC uses the value of the target code rate x[1024], as per TS38.214, Section 6.1.4.1, Table 6.1.4.1-1.
  const vrb_interval& vrbs               = rbs.type1();
  float               R                  = pusch_pdu.mcs_descr.get_normalised_target_code_rate();
  const units::bytes  tb_size_lbrm_bytes = tbs_lbrm_default;
  builder.set_frequency_allocation_parameters(vrbs,
                                              pusch_pdu.tx_direct_current_location,
                                              get_ldpc_base_graph(R, units::bytes{pusch_pdu.tb_size_bytes}.to_bits()),
                                              tb_size_lbrm_bytes);

  // Fill the UCI parameters.
  fill_optional_uci_parameters(builder, mac_pdu.uci, part2_mapper);

  if (mac_pdu.context.rapid.has_value()) {
    builder.add_optional_rapid(*mac_pdu.context.rapid);
  }

  // Set PUSCH HARQ identifier for logging.
  builder.set_harq_id(pusch_pdu.harq_id);
}
