// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/e2ap/common.h"
#include "ocudu/asn1/asn1_ap_codec.h"

using namespace asn1;
using namespace asn1::e2ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// TriggeringMessage ::= ENUMERATED
static const char*           enum_names_trigger_msg_e_values[] = {"initiating-message",
                                                                  "successful-outcome",
                                                                  "unsuccessfull-outcome"};
const asn1::table::enum_desc trigger_msg_opts::desc            = {enum_names_trigger_msg_e_values, 3, "trigger_msg_e"};
