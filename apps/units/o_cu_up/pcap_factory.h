// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/worker_manager/pcap_executor_mapper.h"
#include "apps/units/o_cu_up/o_cu_up_unit_config.h"
#include "ocudu/pcap/dlt_pcap.h"
#include "ocudu/support/signal_observer.h"

namespace ocudu {

struct o_cu_up_dlt_pcaps {
  std::unique_ptr<dlt_pcap> ngu;
  std::unique_ptr<dlt_pcap> f1u;
  std::unique_ptr<dlt_pcap> e1ap;
  std::unique_ptr<dlt_pcap> e2ap;

  std::unique_ptr<signal_observer> ngu_sig_handler;
  std::unique_ptr<signal_observer> f1u_sig_handler;
  std::unique_ptr<signal_observer> e1ap_sig_handler;
  std::unique_ptr<signal_observer> e2ap_sig_handler;

  /// \brief Close (and flush) the PCAPs without destroying the objects.
  void close()
  {
    if (ngu) {
      ngu->close();
    }
    if (f1u) {
      f1u->close();
    }
    if (e1ap) {
      e1ap->close();
    }
    if (e2ap) {
      e2ap->close();
    }
  }

  /// \brief Destroy (close and flush) the PCAPs.
  void reset()
  {
    ngu_sig_handler.reset();
    f1u_sig_handler.reset();
    e1ap_sig_handler.reset();
    e2ap_sig_handler.reset();

    ngu.reset();
    f1u.reset();
    e1ap.reset();
    e2ap.reset();
  }
};

/// Creates the DLT PCAPs of the O-RAN CU-UP.
inline o_cu_up_dlt_pcaps create_o_cu_up_dlt_pcaps(const o_cu_up_unit_config&  unit_cfg,
                                                  cu_up_pcap_executor_mapper& exec_mapper,
                                                  signal_dispatcher&          signal_source)
{
  o_cu_up_dlt_pcaps pcaps;

  const auto& cu_pcaps  = unit_cfg.cu_up_cfg.pcap_cfg;
  pcaps.ngu             = cu_pcaps.ngu.enabled ? create_gtpu_pcap(cu_pcaps.ngu.filename, exec_mapper.get_ngu_executor())
                                               : create_null_dlt_pcap();
  pcaps.ngu_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.ngu->flush(); });

  pcaps.f1u             = cu_pcaps.f1u.enabled ? create_gtpu_pcap(cu_pcaps.f1u.filename, exec_mapper.get_f1u_executor())
                                               : create_null_dlt_pcap();
  pcaps.f1u_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.f1u->flush(); });

  pcaps.e1ap = cu_pcaps.e1ap.enabled ? create_e1ap_pcap(cu_pcaps.e1ap.filename, exec_mapper.get_e1ap_executor())
                                     : create_null_dlt_pcap();
  pcaps.e1ap_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.e1ap->flush(); });

  pcaps.e2ap             = unit_cfg.e2_cfg.pcaps.enabled
                               ? create_e2ap_pcap(unit_cfg.e2_cfg.pcaps.filename, exec_mapper.get_e2ap_executor())
                               : create_null_dlt_pcap();
  pcaps.e2ap_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.e2ap->flush(); });

  return pcaps;
}

} // namespace ocudu
