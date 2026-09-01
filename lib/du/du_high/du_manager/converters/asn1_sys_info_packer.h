// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/du/du_cell_config.h"
#include "ocudu/mac/cell_configuration.h"
#include <string>
#include <vector>

namespace ocudu {

struct sib19_info;

namespace odu {

namespace asn1_packer {

/// \brief Derive and pack cell MIB based on DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \return byte buffer with packed cell MIB.
byte_buffer pack_mib(const du_cell_config& du_cfg);

/// \brief Derive and pack cell SIB1 based on DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \param[out] json_string String where the RRC ASN.1 SIB1 is stored in json format. If nullptr, no conversion takes
/// place.
/// \return byte buffer with packed cell SIB1.
byte_buffer pack_sib1(const du_cell_config& du_cfg, std::string* js_str = nullptr);

/// \brief Derive and pack cell SIB19.
/// \param[in] sib19_params content of SIB19 msg.
///\param[out] json_string String where the RRC ASN.1 SIB1 is stored in json format. If nullptr, no conversion takes
/// place.
/// \return byte buffer with packed cell SIB19.
byte_buffer pack_sib19(const sib19_info& sib19_params, std::string* js_str = nullptr);

/// \brief Pack all cell BCCH-DL-SCH messages (SIB1 + SI messages) from DU cell configuration.
/// \param[in] du_cfg DU Cell Configuration.
/// \param[out] bcch_dl_sch_json_msgs Optional list of BCCH-DL-SCH messages serialized as JSON (one entry per returned
/// message). If nullptr, no conversion takes place.
/// \return A list of buffers with packed cell BCCH-DL-SCH message. First buffer is SIB1, the rest are the SI messages
/// of the normal operation. The ones carrying a warning are packed by \c pack_pws_si_messages.
std::vector<bcch_dl_sch_payload_type>
pack_all_bcch_dl_sch_msgs(const du_cell_config& du_cfg, std::vector<std::string>* bcch_dl_sch_json_msgs = nullptr);

/// \brief Pack the content that each SI message carrying a warning broadcasts from the cell start.
/// \param[in] du_cfg DU Cell Configuration.
/// \return One entry per \c si_scheduling_info_config::pws_si_messages entry, empty if no content is configured for it.
std::vector<bcch_dl_sch_payload_type> pack_pws_si_messages(const du_cell_config& du_cfg);

} // namespace asn1_packer
} // namespace odu
} // namespace ocudu
