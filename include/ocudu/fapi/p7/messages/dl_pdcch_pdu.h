// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/messages/tx_precoding_and_beamforming_pdu.h"
#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/pdcch/aggregation_level.h"
#include "ocudu/ran/pdcch/dci_packing.h"
#include "ocudu/ran/pdcch/pdcch_context.h"
#include "ocudu/ran/resource_allocation/ofdm_symbol_range.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/subcarrier_spacing.h"

namespace ocudu {
namespace fapi {

/// Downlink DCI PDU configuration.
struct dl_dci_pdu {
  /// PDCCH PDU profile SSS power parameters.
  struct power_profile_sss {
    float dmrs_power_offset_db;
    float data_power_offset_db;
  };

  /// PDCCH PDU profile NR power parameters.
  struct power_profile_nr {
    int8_t power_control_offset_ss_db;
  };

  rnti_t                                            rnti;
  uint16_t                                          nid_pdcch_data;
  uint16_t                                          nid_pdcch_dmrs;
  uint16_t                                          nrnti_pdcch_data;
  uint8_t                                           cce_index;
  aggregation_level                                 dci_aggregation_level;
  tx_precoding_and_beamforming_pdu                  precoding_and_beamforming;
  std::variant<power_profile_nr, power_profile_sss> power_config;
  dci_payload                                       payload;
  /// Vendor specific parameters.
  std::optional<pdcch_context> context;
};

/// Downlink PDCCH PDU information.
struct dl_pdcch_pdu {
  /// Holds the parameters that are needed for PDUs transmitted in CORESET 0.
  struct mapping_coreset_0 {
    coreset_configuration::interleaved_mapping_type interleaved;
  };

  /// Holds the parameters that are needed for PDUs with interleaving.
  struct mapping_interleaved {
    coreset_configuration::interleaved_mapping_type interleaved;
  };

  /// Holds the parameters that are needed for PDUs with no interleaving.
  struct mapping_non_interleaved {
    uint8_t reg_bundle_sz;
  };

  crb_interval                                                                  coreset_bwp;
  subcarrier_spacing                                                            scs;
  cyclic_prefix                                                                 cp;
  ofdm_symbol_range                                                             symbols;
  freq_resource_bitmap                                                          freq_domain_resource;
  std::variant<mapping_coreset_0, mapping_interleaved, mapping_non_interleaved> mapping;
  coreset_configuration::precoder_granularity_type                              precoder_granularity;
  dl_dci_pdu                                                                    dl_dci;
};

} // namespace fapi
} // namespace ocudu

namespace fmt {
template <>
struct formatter<ocudu::fapi::dl_dci_pdu> {
private:
  /// Converts the given power offset to dB as per SCF-222 v4.0 section 3.4.8.
  static float to_power_sss_ul_dci(float power_offset) { return power_offset * 0.001; }

public:
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::dl_dci_pdu& pdu, FormatContext& ctx) const
  {
    fmt::format_to(ctx.out(),
                   "DCI rnti={} nid_pdcch_data={} nid_pdcch_dmrs={} nrnti_pdcch_data={} cce_index={} "
                   "dci_aggregation_level={} payload_size={}",
                   pdu.rnti,
                   pdu.nid_pdcch_data,
                   pdu.nid_pdcch_dmrs,
                   pdu.nrnti_pdcch_data,
                   pdu.cce_index,
                   fmt::underlying(pdu.dci_aggregation_level),
                   pdu.payload.size());

    if (const auto* power_profile_nr_in_pdu =
            std::get_if<ocudu::fapi::dl_dci_pdu::power_profile_nr>(&pdu.power_config)) {
      fmt::format_to(ctx.out(),
                     " Power configuration profile NR: power_control_offset_ss_db={}",
                     power_profile_nr_in_pdu->power_control_offset_ss_db);
    } else if (const auto* power_profile_sss_in_pdu =
                   std::get_if<ocudu::fapi::dl_dci_pdu::power_profile_sss>(&pdu.power_config)) {
      fmt::format_to(ctx.out(),
                     " Power configuration profile SSS: dmrs_power_offset_db={} data_power_offset_db={}",
                     to_power_sss_ul_dci(power_profile_sss_in_pdu->dmrs_power_offset_db),
                     to_power_sss_ul_dci(power_profile_sss_in_pdu->data_power_offset_db));
    }

    if (pdu.context.has_value()) {
      fmt::format_to(ctx.out(), " with context");
    }

    return ctx.out();
  }
};

template <>
struct formatter<ocudu::fapi::dl_pdcch_pdu> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const ocudu::fapi::dl_pdcch_pdu& pdu, FormatContext& ctx) const
  {
    format_to(ctx.out(),
              "\n\t- PDCCH bwp={} scs={} cp{} symb={} freq_domain_resource={} precoder_granularity={} {}",
              pdu.coreset_bwp,
              to_string(pdu.scs),
              pdu.cp.to_string(),
              pdu.symbols,
              pdu.freq_domain_resource,
              underlying(pdu.precoder_granularity),
              pdu.dl_dci);

    if (const auto* dci_coreset_0 = std::get_if<ocudu::fapi::dl_pdcch_pdu::mapping_coreset_0>(&pdu.mapping)) {
      format_to(ctx.out(),
                " CORESET0 reg_bundle_sz={} interleaver_sz={} shift_index={}",
                dci_coreset_0->interleaved.reg_bundle_sz,
                dci_coreset_0->interleaved.interleaver_sz,
                dci_coreset_0->interleaved.shift_index);

    } else if (const auto* dci_interleaved =
                   std::get_if<ocudu::fapi::dl_pdcch_pdu::mapping_interleaved>(&pdu.mapping)) {
      format_to(ctx.out(),
                " INTERLEAVED reg_bundle_sz={} interleaver_sz={} shift_index={}",
                dci_interleaved->interleaved.reg_bundle_sz,
                dci_interleaved->interleaved.interleaver_sz,
                dci_interleaved->interleaved.shift_index);

    } else if (const auto* dci_non_interleaved =
                   std::get_if<ocudu::fapi::dl_pdcch_pdu::mapping_non_interleaved>(&pdu.mapping)) {
      format_to(ctx.out(), " NON INTERLEAVED reg_bundle_sz={}", dci_non_interleaved->reg_bundle_sz);
    }

    return ctx.out();
  }
};
} // namespace fmt
