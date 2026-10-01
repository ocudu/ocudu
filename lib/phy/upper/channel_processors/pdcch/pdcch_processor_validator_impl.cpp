// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcch_processor_validator_impl.h"
#include "ocudu/adt/format.h"

using namespace ocudu;

error_type<std::string> pdcch_processor_validator_impl::is_valid(const pdcch_processor::pdu_t& pdu) const

{
  static constexpr interval<unsigned, true> coreset_duration_range(1, pdcch_constants::MAX_CORESET_DURATION);
  if (!coreset_duration_range.contains(pdu.coreset.duration)) {
    return make_unexpected(fmt::format(
        "The CORESET duration (i.e., {}) is out of the range {}-", pdu.coreset.duration, coreset_duration_range));
  }

  unsigned slot_duration = get_nsymb_per_slot(pdu.cp);
  if (pdu.coreset.start_symbol_index + pdu.coreset.duration > slot_duration) {
    return make_unexpected(fmt::format("The CORESET start symbol index (i.e., {}) plus the duration (i.e., {}) "
                                       "exceeds the slot duration (i.e., {}).",
                                       pdu.coreset.start_symbol_index,
                                       pdu.coreset.duration,
                                       slot_duration));
  }

  bool use_interleaver_mapping =
      (pdu.coreset.cce_to_reg_mapping == pdcch_processor::cce_to_reg_mapping_type::INTERLEAVED);
  if (use_interleaver_mapping &&
      (((pdu.coreset.duration == 3) && (pdu.coreset.reg_bundle_size != 3)) ||
       ((pdu.coreset.duration != 3) && (pdu.coreset.reg_bundle_size != 2))) &&
      (pdu.coreset.reg_bundle_size != 6)) {
    return make_unexpected(fmt::format("Invalid REG bundle size (i.e., {}) for CORESET duration of {}.",
                                       pdu.coreset.reg_bundle_size,
                                       pdu.coreset.duration));
  }

  if (use_interleaver_mapping && (pdu.coreset.interleaver_size != 2) && (pdu.coreset.interleaver_size != 3) &&
      (pdu.coreset.interleaver_size != 6)) {
    return make_unexpected(fmt::format("Invalid interleaver size (i.e., {}).", pdu.coreset.interleaver_size));
  }

  if ((pdu.dci.dci_aggregation_level != aggregation_level::n1) &&
      (pdu.dci.dci_aggregation_level != aggregation_level::n2) &&
      (pdu.dci.dci_aggregation_level != aggregation_level::n4) &&
      (pdu.dci.dci_aggregation_level != aggregation_level::n8) &&
      (pdu.dci.dci_aggregation_level != aggregation_level::n16)) {
    return make_unexpected(
        fmt::format("Invalid aggregation level (i.e., {}).", to_nof_cces(pdu.dci.dci_aggregation_level)));
  }

  unsigned nof_coreset_cce = pdu.coreset.frequency_resources.count() * pdu.coreset.duration;
  if (pdu.dci.cce_index + to_nof_cces(pdu.dci.dci_aggregation_level) > nof_coreset_cce) {
    return make_unexpected(fmt::format(
        "The CCE index (i.e., {}) plus the aggregation level (i.e., {}) exceeds CORESET capacity (i.e., {}).",
        pdu.dci.cce_index,
        to_nof_cces(pdu.dci.dci_aggregation_level),
        nof_coreset_cce));
  }

  if (pdu.dci.payload.empty()) {
    return make_unexpected("Empty payload.");
  }

  if (pdu.dci.precoding_and_beamforming.get_nof_layers() != 1) {
    return make_unexpected(fmt::format("Precoding number of layers (i.e., {}) must be one.",
                                       pdu.dci.precoding_and_beamforming.get_nof_layers()));
  }

  return default_success_t();
}
