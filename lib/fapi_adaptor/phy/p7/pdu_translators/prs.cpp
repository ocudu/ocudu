// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "prs.h"
#include "ocudu/adt/format.h"
#include "ocudu/fapi_adaptor/precoding_codebook_repository.h"
#include "ocudu/phy/upper/signal_processors/prs/prs_generator_configuration.h"

using namespace ocudu;
using namespace fapi_adaptor;

void ocudu::fapi_adaptor::convert_prs_fapi_to_phy(prs_generator_configuration&         generator_config,
                                                  const fapi::dl_prs_pdu&              fapi_pdu,
                                                  slot_point                           slot,
                                                  const precoding_codebook_repository& pm_repo)
{
  generator_config.slot         = slot;
  generator_config.cp           = fapi_pdu.cp;
  generator_config.n_id_prs     = fapi_pdu.nid_prs;
  generator_config.comb_size    = fapi_pdu.comb_size;
  generator_config.comb_offset  = fapi_pdu.comb_offset;
  generator_config.duration     = fapi_pdu.num_symbols;
  generator_config.start_symbol = fapi_pdu.first_symbol;
  generator_config.prb_start    = fapi_pdu.crbs.start();
  generator_config.freq_alloc   = interval<uint16_t>::start_and_len(fapi_pdu.crbs.start(), fapi_pdu.crbs.length());
  generator_config.power_offset_dB =
      fapi_pdu.prs_power_offset_db.has_value() ? fapi_pdu.prs_power_offset_db.value() : 0.f;
  generator_config.precoding_and_beamforming = precoding_beamforming_configuration::make_wideband(
      pm_repo.get_precoding_config(fapi_pdu.precoding_and_beamforming.prg.pm_index));
}
