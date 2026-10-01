// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1u/du/f1u_bearer.h"
#include "ocudu/f1u/du/f1u_config.h"
#include "ocudu/f1u/du/f1u_tx_pdu_notifier.h"
#include "ocudu/gtpu/gtpu_teid_pool.h"
#include "ocudu/ran/gnb_du_id.h"
#include "ocudu/ran/qos/five_qi.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/ran/s_nssai.h"
#include "ocudu/ran/up_transport_layer_info.h"
#include "ocudu/support/timers.h"

namespace ocudu::odu {
/// This class provides a notifier for the RX bearer
/// inside the DU F1-U gateway. This provides an adapter
/// to the NR-U bearer to pass SDUs into.
class f1u_du_gateway_bearer_rx_notifier
{
public:
  virtual ~f1u_du_gateway_bearer_rx_notifier() = default;

  virtual void on_new_pdu(nru_dl_message msg) = 0;
};

/// This class provides the interface for an F1-U GW bearer
class f1u_du_gateway_bearer : public odu::f1u_tx_pdu_notifier
{
  virtual void stop() = 0;

public:
  virtual expected<std::string> get_bind_address() const = 0;
};

/// This class will be used to provide the interfaces to
/// the DU to create and manage F1-U bearers.
class f1u_du_gateway : public odu::f1u_bearer_disconnector
{
public:
  f1u_du_gateway()                                 = default;
  ~f1u_du_gateway() override                       = default;
  f1u_du_gateway(const f1u_du_gateway&)            = default;
  f1u_du_gateway& operator=(const f1u_du_gateway&) = default;
  f1u_du_gateway(f1u_du_gateway&&)                 = default;
  f1u_du_gateway& operator=(f1u_du_gateway&&)      = default;

  virtual std::unique_ptr<f1u_du_gateway_bearer> create_du_bearer(uint32_t                       ue_index,
                                                                  drb_id_t                       drb_id,
                                                                  s_nssai_t                      s_nssai,
                                                                  five_qi_t                      five_qi,
                                                                  odu::f1u_config                config,
                                                                  const gtpu_teid_t&             dl_up_tnl_info,
                                                                  gtpu_teid_pool&                dl_teid_pool,
                                                                  const up_transport_layer_info& ul_up_tnl_info,
                                                                  odu::f1u_du_gateway_bearer_rx_notifier& du_rx,
                                                                  timer_factory                           timers,
                                                                  task_executor& ue_executor) = 0;

  virtual expected<std::string> get_du_bind_address(gnb_du_id_t gnb_du_id) const = 0;
};

/// This class will be used to provide the interfaces to
/// the CU-UP to create and manage F1-U bearers.
class f1u_du_udp_gateway : public f1u_du_gateway
{
public:
  f1u_du_udp_gateway()                                     = default;
  ~f1u_du_udp_gateway() override                           = default;
  f1u_du_udp_gateway(const f1u_du_udp_gateway&)            = default;
  f1u_du_udp_gateway& operator=(const f1u_du_udp_gateway&) = default;
  f1u_du_udp_gateway(f1u_du_udp_gateway&&)                 = default;
  f1u_du_udp_gateway& operator=(f1u_du_udp_gateway&&)      = default;

  virtual std::optional<uint16_t> get_bind_port() const = 0;
};

} // namespace ocudu::odu
