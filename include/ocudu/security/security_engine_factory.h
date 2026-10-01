// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/security/security.h"
#include "ocudu/security/security_engine.h"
#include <memory>

/// This factory header file depends on the security engine interfaces (see above include list). It is kept separate as
/// clients of the Security engine interfaces dont need to call factory methods.
namespace ocudu {

struct security_engine_creation_message {
  security::sec_128_as_config  sec_cfg;
  uint8_t                      bearer_id;
  security::security_direction direction;
  security::integrity_enabled  integrity_enabled;
  security::ciphering_enabled  ciphering_enabled;
};

/// Creates an instance of a TX security engine.
std::unique_ptr<security::security_engine_tx> create_security_engine_tx(const security_engine_creation_message& msg);
/// Creates an instance of a RX security engine.
std::unique_ptr<security::security_engine_rx> create_security_engine_rx(const security_engine_creation_message& msg);

} // namespace ocudu
