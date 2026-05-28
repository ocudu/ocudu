// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ngap_repository.h"
#include "ue_manager/ue_manager_impl.h"
#include "ocudu/cu_cp/cu_configurator.h"
#include "ocudu/ran/guami.h"

namespace ocudu {

class cu_configurator_impl : public cu_configurator
{
public:
  cu_configurator_impl(ocucp::mobility_manager_cu_cp_notifier& mobility_notif_,
                       ocucp::du_processor_repository&         du_db_,
                       ocucp::ngap_repository&                 ngap_db_,
                       ocucp::ue_manager&                      ue_mng_) :
    mobility_notif(mobility_notif_), du_db(du_db_), ngap_db(ngap_db_), ue_mng(ue_mng_)
  {
  }

  /// Get UE index for a given tuple of (AMF-UE-NGAP-ID, GUAMI, GNB-CU-UE-F1AP-ID).
  cu_cp_ue_index_t get_ue_index(const ocucp::amf_ue_id_t&  amf_ue_id,
                                const guami_t&             guami,
                                const gnb_cu_ue_f1ap_id_t& gnb_cu_ue_f1ap_id) const override;

  /// Get index of the DU that has UE with a given GNB-CU-UE-F1AP-ID.
  cu_cp_du_index_t get_du_index(const cu_cp_ue_index_t& ue_index) const override;

  /// Get DU index for a given NR Cell Global ID.
  cu_cp_du_index_t get_du_index(const nr_cell_global_id_t& nr_cgi) const override;

  /// Get PCI for a given NR Cell Global ID.
  pci_t get_pci(const nr_cell_global_id_t& nr_cgi) const override;

  async_task<ocucp::cu_cp_intra_cu_handover_response>
  trigger_handover(const cu_cp_du_index_t&                       source_du_index,
                   const ocucp::cu_cp_intra_cu_handover_request& handover_req) override;

private:
  async_task<ocucp::cu_cp_intra_cu_handover_response> return_handover_response(bool ack);

  ocucp::mobility_manager_cu_cp_notifier& mobility_notif;
  ocucp::du_processor_repository&         du_db;
  ocucp::ngap_repository&                 ngap_db;
  ocucp::ue_manager&                      ue_mng;
};

} // namespace ocudu
