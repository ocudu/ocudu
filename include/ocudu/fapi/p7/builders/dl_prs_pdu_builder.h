// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/fapi/p7/builders/tx_precoding_and_beamforming_pdu_builder.h"
#include "ocudu/fapi/p7/messages/dl_prs_pdu.h"

namespace ocudu {
namespace fapi {

/// PRS PDU builder that helps to fill in the parameters specified in SCF-222 v8.0 section 3.4.2.4a.
class dl_prs_pdu_builder
{
  dl_prs_pdu& pdu;

public:
  explicit dl_prs_pdu_builder(dl_prs_pdu& pdu_) : pdu(pdu_) {}

  /// \brief Sets the PRS PDU BWP parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v8.0 section 3.4.2.4a in table PRS PDU.
  dl_prs_pdu_builder& set_bwp_parameters(subcarrier_spacing scs, cyclic_prefix cp)
  {
    pdu.scs = scs;
    pdu.cp  = cp;

    return *this;
  }

  /// \brief Sets the PRS PDU N_ID parameter and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v8.0 section 3.4.2.4a in table PRS PDU.
  dl_prs_pdu_builder& set_n_id(unsigned n_id)
  {
    pdu.nid_prs = n_id;

    return *this;
  }

  /// \brief Sets the PRS PDU symbol parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v8.0 section 3.4.2.4a in table PRS PDU.
  dl_prs_pdu_builder& set_symbol_parameters(prs_num_symbols nof_symbols, unsigned first_symbol)
  {
    pdu.num_symbols  = nof_symbols;
    pdu.first_symbol = first_symbol;

    return *this;
  }

  /// \brief Sets the PRS PDU RB parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v8.0 section 3.4.2.4a in table PRS PDU.
  dl_prs_pdu_builder& set_rb_parameters(crb_interval crbs)
  {
    pdu.crbs = crbs;

    return *this;
  }

  /// \brief Sets the PRS PDU power offset parameter and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v8.0 section 3.4.2.4a in table PRS PDU.
  dl_prs_pdu_builder& set_power_offset(std::optional<float> power_offset)
  {
    pdu.prs_power_offset_db = power_offset;

    return *this;
  }

  /// \brief Sets the PRS PDU transmission comb parameters and returns a reference to the builder.
  ///
  /// These parameters are specified in SCF-222 v8.0 section 3.4.2.4a in table PRS PDU.
  dl_prs_pdu_builder& set_comb_parameters(prs_comb_size comb_size, unsigned comb_offset)
  {
    pdu.comb_size   = comb_size;
    pdu.comb_offset = comb_offset;

    return *this;
  }

  /// \brief Returns a transmission precoding and beamforming PDU builder of this PRS PDU.
  ///
  /// These parameters are specified in SCF-222 v8.0 section 3.4.2.4a in table PRS PDU.
  tx_precoding_and_beamforming_pdu_builder get_tx_precoding_and_beamforming_pdu_builder()
  {
    tx_precoding_and_beamforming_pdu_builder builder(pdu.precoding_and_beamforming);

    return builder;
  }
};

} // namespace fapi
} // namespace ocudu
