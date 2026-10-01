// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/ran/pci.h"
#include "ocudu/ran/sib/cell_reselection.h"

namespace ocudu::asn1_utils {

/// Convert Qhyst value to ASN.1 RRC representation.
asn1::rrc_nr::sib2_s::cell_resel_info_common_s_::q_hyst_opts::options make_asn1_rrc_q_hyst(const q_hyst_t& q_hyst);

/// Convert QoffsetRange value to ASN.1 RRC representation.
asn1::rrc_nr::q_offset_range_opts::options make_asn1_rrc_q_offset_range(const q_offset_range_t& q_offset_range);

/// Convert PCI range to ASN.1 RRC representation.
asn1::rrc_nr::pci_range_s make_asn1_rrc_pci_range(const pci_range_t& pci_range);

/// Convert allowed measurement bandwidth to ASN.1 RRC representation.
asn1::rrc_nr::eutra_allowed_meas_bw_opts::options
make_asn1_rrc_allowed_meas_bw(const eutra_allowed_meas_bandwidth_t& allowed_beas_bw);

} // namespace ocudu::asn1_utils
