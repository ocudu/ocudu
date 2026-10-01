// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/builders/ul_pucch_format_0_pdu_builder.h"
#include "ocudu/fapi/p7/builders/ul_pucch_format_1_pdu_builder.h"
#include "ocudu/fapi/p7/builders/ul_pucch_format_2_pdu_builder.h"
#include "ocudu/fapi/p7/builders/ul_pucch_format_3_pdu_builder.h"
#include "ocudu/fapi/p7/builders/ul_pucch_format_4_pdu_builder.h"
#include "ocudu/fapi/p7/messages/ul_pucch_pdu.h"

namespace ocudu {
namespace fapi {

/// PUCCH PDU builder that helps to fill in the parameters specified in SCF-222 v4.0 section 3.4.3.3.
class ul_pucch_pdu_builder
{
  ul_pucch_pdu& pdu;

public:
  explicit ul_pucch_pdu_builder(ul_pucch_pdu& pdu_) : pdu(pdu_) {}

  /// \brief Sets the PUCCH PDU UE specific parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU.
  ul_pucch_pdu_builder& set_ue_specific_parameters(rnti_t rnti)
  {
    pdu.rnti = rnti;

    return *this;
  }

  /// \brief Gets the builder helper instance to build a PUCCH format 0 PDU.
  ul_pucch_format_0_pdu_builder get_pucch_format_0_builder()
  {
    ocudu_assert(pdu.format.index() == 0 || std::holds_alternative<ul_pucch_pdu_format_0>(pdu.format),
                 "Builder of format 0 already called with a different format: {}",
                 pdu.format.index());

    auto& format = pdu.format.emplace<ul_pucch_pdu_format_0>();
    return ul_pucch_format_0_pdu_builder(format);
  }

  /// \brief Gets the builder helper instance to build a PUCCH format 1 PDU.
  ul_pucch_format_1_pdu_builder get_pucch_format_1_builder()
  {
    ocudu_assert(pdu.format.index() == 0 || std::holds_alternative<ul_pucch_pdu_format_1>(pdu.format),
                 "Builder of format 1 already called with a different format: {}",
                 pdu.format.index());
    auto& format = pdu.format.emplace<ul_pucch_pdu_format_1>();
    return ul_pucch_format_1_pdu_builder(format);
  }

  /// \brief Gets the builder helper instance to build a PUCCH format 2 PDU.
  ul_pucch_format_2_pdu_builder get_pucch_format_2_builder()
  {
    ocudu_assert(pdu.format.index() == 0 || std::holds_alternative<ul_pucch_pdu_format_2>(pdu.format),
                 "Builder of format 2 already called with a different format: {}",
                 pdu.format.index());

    auto& format = pdu.format.emplace<ul_pucch_pdu_format_2>();
    return ul_pucch_format_2_pdu_builder(format);
  }

  /// \brief Gets the builder helper instance to build a PUCCH format 3 PDU.
  ul_pucch_format_3_pdu_builder get_pucch_format_3_builder()
  {
    ocudu_assert(pdu.format.index() == 0 || std::holds_alternative<ul_pucch_pdu_format_3>(pdu.format),
                 "Builder of format 3 already called with a different format: {}",
                 pdu.format.index());

    auto& format = pdu.format.emplace<ul_pucch_pdu_format_3>();
    return ul_pucch_format_3_pdu_builder(format);
  }

  /// \brief Gets the builder helper instance to build a PUCCH format 4 PDU.
  ul_pucch_format_4_pdu_builder get_pucch_format_4_builder()
  {
    ocudu_assert(pdu.format.index() == 0 || std::holds_alternative<ul_pucch_pdu_format_4>(pdu.format),
                 "Builder of format 4 already called with a different format: {}",
                 pdu.format.index());

    auto& format = pdu.format.emplace<ul_pucch_pdu_format_4>();
    return ul_pucch_format_4_pdu_builder(format);
  }

  /// \brief Sets the PUCCH PDU BWP parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU.
  ul_pucch_pdu_builder& set_bwp_parameters(crb_interval bwp, subcarrier_spacing scs, cyclic_prefix cp)
  {
    pdu.bwp = bwp;
    pdu.scs = scs;
    pdu.cp  = cp;

    return *this;
  }

  /// \brief Sets the PUCCH PDU frequency allocation parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU.
  ul_pucch_pdu_builder& set_frequency_allocation_parameters(prb_interval prbs)
  {
    pdu.prbs = prbs;

    return *this;
  }

  /// \brief Sets the PUCCH PDU time allocation parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU.
  ul_pucch_pdu_builder& set_time_allocation_parameters(ofdm_symbol_range symbols)
  {
    pdu.symbols = symbols;

    return *this;
  }

  /// \brief Sets the PUCCH PDU hopping information parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU.
  ul_pucch_pdu_builder& set_hopping_information_parameters(std::optional<uint16_t> second_hop_prb)
  {
    pdu.second_hop_prb = second_hop_prb;

    return *this;
  }

  /// \brief Sets the PUCCH PDU multi-slot TX indicator and returns a reference to the builder.
  ///
  /// This parameter is specified in SCF-222 v4.0 section 3.4.3.3 in table PUCCH PDU, common to all PUCCH formats.
  ul_pucch_pdu_builder& set_multi_slot_tx_indicator(pucch_repetition_tx_slot indicator)
  {
    pdu.multi_slot_tx_indicator = indicator;

    return *this;
  }
};

} // namespace fapi
} // namespace ocudu
