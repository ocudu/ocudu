// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

/// \file Conversion between native scheduler types and flatbuffer tables.
///
/// The convert_*_to_fb direction runs on the producer (RT) thread, so it is malloc-free: tables are built bottom-up
/// into the caller-provided builder, using fixed-capacity scratch storage for the intermediate offsets. The
/// convert_fb_to_* direction uses the zero-copy accessors and is only used by offline consumers (schedlog) and tests.

#include "cell_configuration.h"
#include "fbs/cell_start_event_generated.h"
#include "fbs/slot_decision_generated.h"
#include "fbs/slot_input_generated.h"
#include "flatbuffers/flatbuffer_builder.h"
#include "ocudu/adt/span.h"
#include "ocudu/ran/subcarrier_spacing.h"

namespace ocudu {

class cell_configuration;
struct sched_result;
struct bwp_configuration;
struct pucch_info;
struct rach_indication_message;
struct harq_ack_event;
struct sr_event;
struct csi_report_event;
struct ul_sched_info;
struct dl_msg_alloc;
struct sib_information;
struct rar_information;
struct dl_paging_allocation;
struct srs_info;
struct csi_rs_info;
struct failed_alloc_attempts;
struct ssb_information;
struct prach_occasion_info;
struct pdcch_dl_information;
struct pdcch_ul_information;
struct coreset_configuration;

namespace schedtrace {

/// Convert a cell configuration to the respective flatbuffer table.
flatbuffers::Offset<fbs::CellStartEvent> convert_cell_cfg_to_fb(flatbuffers::FlatBufferBuilder&  fbb,
                                                                const ocudu::cell_configuration& cell_cfg);

/// Convert a trace-side cell configuration to the respective flatbuffer table.
flatbuffers::Offset<fbs::CellStartEvent> convert_cell_cfg_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                const cell_configuration&       cell_cfg);

/// Convert a flatbuffer start event into the original cell configuration.
void convert_fb_to_cell_cfg(cell_configuration& cell_cfg, const fbs::CellStartEvent& start_event);

/// Convert a PUCCH resource to the respective flatbuffer table.
flatbuffers::Offset<fbs::PucchResource> convert_pucch_resource_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                     const pucch_resource&           res);

/// Convert a flatbuffer PUCCH resource into the original PUCCH resource.
void convert_fb_to_pucch_resource(pucch_resource& res, const fbs::PucchResource& fb, const pucch_res_id_t& res_id);

/// Convert a BWP configuration to the respective flatbuffer struct.
fbs::BwpConfiguration convert_bwp_cfg_to_fb(const bwp_configuration& bwp);

/// Convert a flatbuffer BWP configuration into the original BWP configuration.
void convert_fb_to_bwp_cfg(bwp_configuration& bwp, const fbs::BwpConfiguration& fb);

/// Convert a PUCCH allocation to the respective flatbuffer table.
flatbuffers::Offset<fbs::Pucch> convert_pucch_info_to_fb(flatbuffers::FlatBufferBuilder& fbb, const pucch_info& pucch);

/// \brief Convert a flatbuffer PUCCH into the original PUCCH allocation.
///
/// \p ul_bwp and \p pucch_resources provide the out-of-band cell context the PUCCH points into.
void convert_fb_to_pucch_info(pucch_info&                pucch,
                              const fbs::Pucch&          fb,
                              const bwp_configuration*   ul_bwp,
                              span<const pucch_resource> pucch_resources);

/// Convert a PUSCH allocation to the respective flatbuffer table.
flatbuffers::Offset<fbs::Pusch> convert_pusch_to_fb(flatbuffers::FlatBufferBuilder& fbb, const ul_sched_info& pusch);

/// \brief Convert a flatbuffer PUSCH into the original PUSCH allocation.
///
/// \p ul_bwp provides the out-of-band cell context the PUSCH points into.
void convert_fb_to_pusch(ul_sched_info& pusch, const fbs::Pusch& fb, const bwp_configuration* ul_bwp);

/// Convert a UE PDSCH allocation to the respective flatbuffer table.
flatbuffers::Offset<fbs::Pdsch> convert_pdsch_to_fb(flatbuffers::FlatBufferBuilder& fbb, const dl_msg_alloc& pdsch);

/// \brief Convert a flatbuffer PDSCH into the original UE PDSCH allocation.
///
/// \p dl_bwp provides the out-of-band cell context the PDSCH points into.
void convert_fb_to_pdsch(dl_msg_alloc& pdsch, const fbs::Pdsch& fb, const bwp_configuration* dl_bwp);

/// Convert a SIB PDSCH allocation to the respective flatbuffer table.
flatbuffers::Offset<fbs::SibPdsch> convert_sib_pdsch_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                           const sib_information&          sib);

/// \brief Convert a flatbuffer SIB PDSCH into the original SIB PDSCH allocation.
///
/// \p dl_bwp provides the out-of-band cell context the SIB PDSCH points into.
void convert_fb_to_sib_pdsch(sib_information& sib, const fbs::SibPdsch& fb, const bwp_configuration* dl_bwp);

/// Convert a RAR PDSCH allocation to the respective flatbuffer table.
flatbuffers::Offset<fbs::RarPdsch> convert_rar_pdsch_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                           const rar_information&          rar);

/// \brief Convert a flatbuffer RAR PDSCH into the original RAR PDSCH allocation.
///
/// \p dl_bwp provides the out-of-band cell context the RAR PDSCH points into.
void convert_fb_to_rar_pdsch(rar_information& rar, const fbs::RarPdsch& fb, const bwp_configuration* dl_bwp);

/// Convert a paging PDSCH allocation to the respective flatbuffer table.
flatbuffers::Offset<fbs::PagingPdsch> convert_paging_pdsch_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                 const dl_paging_allocation&     paging);

/// \brief Convert a flatbuffer paging PDSCH into the original paging PDSCH allocation.
///
/// \p dl_bwp provides the out-of-band cell context the paging PDSCH points into.
void convert_fb_to_paging_pdsch(dl_paging_allocation&    paging,
                                const fbs::PagingPdsch&  fb,
                                const bwp_configuration* dl_bwp);

/// Convert an SRS allocation to the respective flatbuffer struct.
fbs::Srs convert_srs_to_fb(const srs_info& srs);

/// \brief Convert a flatbuffer SRS into the original SRS allocation.
///
/// \p ul_bwp provides the out-of-band cell context the SRS points into.
void convert_fb_to_srs(srs_info& srs, const fbs::Srs& fb, const bwp_configuration* ul_bwp);

/// Convert a CSI-RS allocation to the respective flatbuffer struct.
fbs::CsiRs convert_csi_rs_to_fb(const csi_rs_info& csi);

/// \brief Convert a flatbuffer CSI-RS into the original CSI-RS allocation.
///
/// \p dl_bwp provides the out-of-band cell context the CSI-RS points into.
void convert_fb_to_csi_rs(csi_rs_info& csi, const fbs::CsiRs& fb, const bwp_configuration* dl_bwp);

/// Convert an SSB allocation to the respective flatbuffer struct.
fbs::Ssb convert_ssb_to_fb(const ssb_information& ssb);

/// Convert a flatbuffer SSB into the original SSB allocation.
void convert_fb_to_ssb(ssb_information& ssb, const fbs::Ssb& fb);

/// \brief Returns a stub coreset_configuration for the given coreset ID.
///
/// The wire format carries only the coreset ID, but consumers dereference \c dci_context_information::coreset_cfg (the
/// scheduler_result_logger reads \c coreset_cfg->get_id()), so deserializing a PDCCH resolves the ID to one of these
/// stubs. Exposed so that tests can build a PDCCH the decoders can reproduce exactly.
const coreset_configuration& get_stub_coreset_cfg(unsigned cs_id);

/// \brief Convert a DL PDCCH allocation to the respective flatbuffer struct.
///
/// The wire format carries a summary of the DCI payload (see fbs::DlPdcch), not its full contents: of the active DCI
/// type, only the HARQ process, NDI, RV, MCS and PUCCH resource indicator are kept, and only for the types that
/// define them.
fbs::DlPdcch convert_dl_pdcch_to_fb(const pdcch_dl_information& pdcch);

/// Convert a flatbuffer DL PDCCH into the original DL PDCCH allocation, resolving the coreset ID to a stub coreset.
void convert_fb_to_dl_pdcch(pdcch_dl_information& pdcch, const fbs::DlPdcch& fb);

/// \brief Convert a UL PDCCH allocation to the respective flatbuffer struct.
///
/// As with the DL, the wire format carries only a summary of the DCI payload (see fbs::UlPdcch).
fbs::UlPdcch convert_ul_pdcch_to_fb(const pdcch_ul_information& pdcch);

/// Convert a flatbuffer UL PDCCH into the original UL PDCCH allocation, resolving the coreset ID to a stub coreset.
void convert_fb_to_ul_pdcch(pdcch_ul_information& pdcch, const fbs::UlPdcch& fb);

/// Convert a PRACH occasion to the respective flatbuffer struct.
fbs::Prach convert_prach_to_fb(const prach_occasion_info& prach);

/// Convert a flatbuffer PRACH into the original PRACH occasion.
void convert_fb_to_prach(prach_occasion_info& prach, const fbs::Prach& fb);

/// Convert a failed allocation attempts struct to the respective flatbuffer struct.
fbs::FailedAttempts convert_failed_attempts_to_fb(const failed_alloc_attempts& failed_attempts);

/// Convert a flatbuffer failed allocation attempts struct into the original failed allocation attempts struct.
void convert_fb_to_failed_attempts(failed_alloc_attempts& failed_attempts, const fbs::FailedAttempts& fb);

/// Convert a scheduler decision to the respective flatbuffer table.
flatbuffers::Offset<fbs::SlotDecision> convert_decision_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                              const sched_result&             result);

/// Convert a flatbuffer SlotDecision into the original scheduler decision.
void convert_fb_to_decision(sched_result&             result,
                            const fbs::SlotDecision&  decision,
                            const cell_configuration& cell_cfg);

/// Convert a RACH indication to the respective flatbuffer table.
flatbuffers::Offset<fbs::RachIndication> convert_rach_indication_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                       const rach_indication_message&  rach_ind);

/// Convert a flatbuffer RACH indication into the original RACH indication.
void convert_fb_to_rach_indication(rach_indication_message&   rach_ind,
                                   const fbs::RachIndication& input,
                                   subcarrier_spacing         scs,
                                   du_cell_index_t            cell_index);

/// Convert a HARQ ACK event to the respective flatbuffer table.
flatbuffers::Offset<fbs::HarqAckEvent> convert_harq_ack_event_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                    const harq_ack_event&           event);

/// Convert a flatbuffer HARQ ACK event into the original HARQ ACK event.
void convert_fb_to_harq_ack_event(harq_ack_event&          event,
                                  const fbs::HarqAckEvent& input,
                                  subcarrier_spacing       scs,
                                  du_cell_index_t          cell_index);

/// Convert an SR event to the respective flatbuffer table.
flatbuffers::Offset<fbs::SrEvent> convert_sr_event_to_fb(flatbuffers::FlatBufferBuilder& fbb, const sr_event& event);

/// Convert a flatbuffer SR event into the original SR event.
void convert_fb_to_sr_event(sr_event& event, const fbs::SrEvent& input);

/// Convert a CSI report event to the respective flatbuffer table.
flatbuffers::Offset<fbs::CsiReportEvent> convert_csi_report_event_to_fb(flatbuffers::FlatBufferBuilder& fbb,
                                                                        const csi_report_event&         event);

/// Convert a flatbuffer CSI report event into the original CSI report event.
void convert_fb_to_csi_report_event(csi_report_event& event, const fbs::CsiReportEvent& input, subcarrier_spacing scs);

} // namespace schedtrace
} // namespace ocudu
