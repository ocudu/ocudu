// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ssb_assembler.h"
#include "ocudu/mac/mac_cell_result.h"
#include "ocudu/ran/ssb/ssb_mapping.h"
#include "ocudu/scheduler/result/pdsch_info.h"

using namespace ocudu;

ssb_assembler::ssb_assembler(const mac_cell_creation_request& cell_cfg) :
  pci(cell_cfg.pci),
  ssb_cfg(cell_cfg.ssb_cfg),
  pdcch_config_sib1((cell_cfg.cs0_index.value() << 4U) + cell_cfg.ss0_index.value()),
  dmrs_typeA_pos(cell_cfg.sched_req.ran.dmrs_typeA_pos),
  cell_barred(cell_cfg.cell_barred),
  intra_freq_reselection(cell_cfg.intra_freq_reselection),
  ssb_case(band_helper::get_ssb_pattern(cell_cfg.dl_carrier.band, ssb_cfg.scs)),
  L_max(ssb_get_L_max(ssb_cfg.scs, cell_cfg.dl_carrier.arfcn_f_ref, cell_cfg.dl_carrier.band))
{
}

void ssb_assembler::assemble_ssb(dl_ssb_pdu& ssb_pdu, const ssb_information& ssb_info)
{
  ssb_pdu.pci               = pci;
  ssb_pdu.pss_to_sss_epre   = ssb_cfg.pss_to_sss_epre;
  ssb_pdu.ssb_index         = ssb_info.ssb_index;
  ssb_pdu.scs               = ssb_cfg.scs;
  ssb_pdu.subcarrier_offset = ssb_cfg.k_ssb;
  ssb_pdu.offset_to_pointA  = ssb_cfg.offset_to_point_A;
  ssb_pdu.ssb_case          = ssb_case;
  ssb_pdu.L_max             = L_max;

  // Fields required for PBCH payload/MIB generation.
  ssb_pdu.mib_data.cell_barred            = cell_barred;
  ssb_pdu.mib_data.intra_freq_reselection = intra_freq_reselection;
  ssb_pdu.mib_data.dmrs_typeA_pos         = dmrs_typeA_pos;
  ssb_pdu.mib_data.pdcch_config_sib1      = pdcch_config_sib1;
}
