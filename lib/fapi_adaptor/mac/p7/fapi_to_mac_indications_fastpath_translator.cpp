// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "fapi_to_mac_indications_fastpath_translator.h"
#include "ocudu/fapi/p7/messages/crc_indication.h"
#include "ocudu/fapi/p7/messages/rach_indication.h"
#include "ocudu/fapi/p7/messages/rx_data_indication.h"
#include "ocudu/fapi/p7/messages/srs_indication.h"
#include "ocudu/fapi/p7/messages/uci_indication.h"
#include "ocudu/ocudulog/ocudulog.h"

using namespace ocudu;
using namespace fapi_adaptor;

namespace {

class mac_cell_rach_handler_dummy : public mac_cell_rach_handler
{
public:
  void handle_rach_indication(const mac_rach_indication& rach_ind) override {}

  std::optional<rnti_t>
  handle_msga_ccch_sdu(rnti_t ra_rnti, uint8_t rapid, slot_point sl_rx, const ue_con_res_id_t& con_res_id) override
  {
    return std::nullopt;
  }

  std::optional<ue_con_res_id_t> resolve_msga_con_res_id(rnti_t tc_rnti) override { return std::nullopt; }
};

class mac_pdu_handler_dummy : public mac_pdu_handler
{
public:
  void handle_rx_data_indication(mac_rx_data_indication pdu) override {}
};

class mac_cell_control_information_handler_dummy : public mac_cell_control_information_handler
{
public:
  void handle_crc(const mac_crc_indication_message& msg) override {}

  void handle_uci(const mac_uci_indication_message& msg) override {}

  void handle_srs(const mac_srs_indication_message& msg) override {}
};

} // namespace

/// This dummy object is passed to the constructor of the FAPI-to-MAC message translator as a placeholder for the
/// actual, cell-specific MAC RACH handler, which will be later set up through the \ref set_cell_rach_handler() method.
static mac_cell_rach_handler_dummy dummy_mac_rach_handler;

/// This dummy object is passed to the constructor of the FAPI-to-MAC message translator as a placeholder for the
/// actual, cell-specific MAC PDU handler, which will be later set up through the \ref set_cell_pdu_handler() method.
static mac_pdu_handler_dummy dummy_pdu_handler;

/// This dummy object is passed to the constructor of the FAPI-to-MAC message translator as a placeholder for the
/// actual, cell-specific MAC CRC handler, which will be later set up through the \ref set_cell_crc_handler() method.
static mac_cell_control_information_handler_dummy dummy_cell_control_handler;

/// Converts the given FAPI UCI SINR to dB as per SCF-222 v4.0 section 3.4.9.
static float to_uci_ul_sinr(int sinr)
{
  return static_cast<float>(sinr) * 0.002F;
}

/// Converts the given FAPI UCI RSSI to dB as per SCF-222 v4.0 section 3.4.9.
static float to_uci_ul_rssi(unsigned rssi)
{
  return static_cast<float>(rssi - 1280) * 0.1F;
}

/// Converts the given FAPI UCI RSRP to dBFS as per SCF-222 v4.0 section 3.4.9.
static float to_uci_ul_rsrp(uint16_t rsrp)
{
  return (static_cast<float>(rsrp) - 1280.0F) * 0.1F;
}

/// Converts the given FAPI UCI SINR to dB as per SCF-222 v4.0 section 3.4.9.
static std::optional<float> convert_fapi_to_mac_ul_sinr(int16_t fapi_ul_sinr)
{
  if (fapi_ul_sinr != std::numeric_limits<decltype(fapi_ul_sinr)>::min()) {
    return to_uci_ul_sinr(fapi_ul_sinr);
  }
  return std::nullopt;
}

/// Converts the given FAPI UCI RSRP to dBFS as per SCF-222 v4.0 section 3.4.9.
static std::optional<float> convert_fapi_to_mac_rsrp(uint16_t fapi_rsrp)
{
  if (fapi_rsrp != std::numeric_limits<decltype(fapi_rsrp)>::max()) {
    return to_uci_ul_rsrp(fapi_rsrp);
  }
  return std::nullopt;
}

fapi_to_mac_indications_fastpath_translator::fapi_to_mac_indications_fastpath_translator(
    unsigned                sector_id_,
    ocudulog::basic_logger& logger_) :
  sector_id(sector_id_),
  logger(logger_),
  rach_handler(&dummy_mac_rach_handler),
  pdu_handler(&dummy_pdu_handler),
  cell_control_handler(&dummy_cell_control_handler)

{
}

void fapi_to_mac_indications_fastpath_translator::on_rx_data_indication(const fapi::rx_data_indication& msg)
{
  mac_rx_data_indication indication;
  indication.sl_rx      = msg.slot;
  indication.cell_index = to_du_cell_index(sector_id);

  // PDUs that were not successfully decoded have zero length.
  if (!msg.pdu.transport_block.empty()) {
    if (auto pdu_buffer = byte_buffer::create(msg.pdu.transport_block); pdu_buffer.has_value()) {
      mac_rx_pdu& pdu = indication.pdus.emplace_back();
      pdu.harq_id     = msg.pdu.harq_id;
      pdu.rnti        = msg.pdu.rnti;
      pdu.rapid       = msg.pdu.rapid;
      pdu.pdu         = std::move(pdu_buffer.value());

      if (OCUDU_UNLIKELY(logger.debug.enabled())) {
        logger.debug("Sector#{}: {}", sector_id, msg);
      }

      pdu_handler->handle_rx_data_indication(std::move(indication));
    } else {
      logger.warning("Sector#{}: Unable to allocate memory for MAC RX PDU", sector_id);
    }
  }
}

void fapi_to_mac_indications_fastpath_translator::on_crc_indication(const fapi::crc_indication& msg)
{
  mac_crc_indication_message indication;
  indication.sl_rx = msg.slot;

  mac_crc_pdu& pdu = indication.crcs.emplace_back();

  pdu.harq_id             = msg.pdu.harq_id;
  pdu.rnti                = msg.pdu.rnti;
  pdu.rapid               = msg.pdu.rapid;
  pdu.tb_crc_success      = msg.pdu.tb_crc_status_ok;
  pdu.ul_sinr_dB          = convert_fapi_to_mac_ul_sinr(msg.pdu.ul_sinr_metric);
  pdu.ul_rsrp_dBFS        = convert_fapi_to_mac_rsrp(msg.pdu.rsrp);
  pdu.time_advance_offset = msg.pdu.timing_advance_offset;

  if (OCUDU_UNLIKELY(logger.debug.enabled())) {
    logger.debug("Sector#{}: {}", sector_id, msg);
  }

  cell_control_handler->handle_crc(indication);
}

/// Converts the given FAPI UCI RSSI to dB as per SCF-222 v4.0 section 3.4.9.
static std::optional<float> convert_fapi_to_mac_rssi(uint16_t fapi_rssi)
{
  if (fapi_rssi != std::numeric_limits<decltype(fapi_rssi)>::max()) {
    return to_uci_ul_rssi(fapi_rssi);
  }
  return std::nullopt;
}

/// Returns true if the UCI payload is valid given a FAPI detection status.
static bool is_fapi_uci_payload_valid(uci_pusch_or_pucch_f2_3_4_detection_status status)
{
  return (status == uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass ||
          status == uci_pusch_or_pucch_f2_3_4_detection_status::no_dtx);
}

static void convert_fapi_to_mac_pucch_f0_f1_uci_ind(mac_uci_pdu::pucch_f0_or_f1_type&     mac_pucch,
                                                    const fapi::uci_pucch_pdu_format_0_1& fapi_pucch)
{
  mac_pucch.ul_sinr_dB          = convert_fapi_to_mac_ul_sinr(fapi_pucch.ul_sinr_metric);
  mac_pucch.rssi_dBFS           = convert_fapi_to_mac_rssi(fapi_pucch.rssi);
  mac_pucch.rsrp_dBFS           = convert_fapi_to_mac_rsrp(fapi_pucch.rsrp);
  mac_pucch.time_advance_offset = fapi_pucch.timing_advance_offset;

  // Fill SR.
  if (fapi_pucch.sr.has_value()) {
    mac_pucch.sr_info.emplace().detected = fapi_pucch.sr->sr_detected;
  }

  // Fill HARQ.
  if (fapi_pucch.harq.has_value()) {
    mac_pucch.harq_info.emplace().harqs.assign(fapi_pucch.harq->harq_values.begin(),
                                               fapi_pucch.harq->harq_values.end());
  }
}

static void convert_fapi_to_mac_pusch_uci_ind(mac_uci_pdu::pusch_type& mac_pusch, const fapi::uci_pusch_pdu& fapi_pusch)
{
  mac_pusch.ul_sinr_dB          = convert_fapi_to_mac_ul_sinr(fapi_pusch.ul_sinr_metric);
  mac_pusch.rssi_dBFS           = convert_fapi_to_mac_rssi(fapi_pusch.rssi);
  mac_pusch.rsrp_dBFS           = convert_fapi_to_mac_rsrp(fapi_pusch.rsrp);
  mac_pusch.time_advance_offset = fapi_pusch.timing_advance_offset;

  // Fill HARQ.
  if (fapi_pusch.harq.has_value()) {
    if (fapi_pusch.harq->payload.empty()) {
      mac_pusch.harq_info.emplace(mac_uci_pdu::pusch_type::harq_information::create_undetected_harq_info(
          fapi_pusch.harq->expected_bit_length.value()));
    } else {
      mac_pusch.harq_info.emplace(
          mac_uci_pdu::pusch_type::harq_information::create_detected_harq_info(fapi_pusch.harq->payload));
    }
  }

  // Fill CSI Part 1.
  if (fapi_pusch.csi_part1.has_value()) {
    mac_uci_pdu::pusch_type::csi_information& csi = mac_pusch.csi_part1_info.emplace();
    csi.is_valid                                  = is_fapi_uci_payload_valid(fapi_pusch.csi_part1->detection_status);
    csi.payload                                   = fapi_pusch.csi_part1->payload;
  }

  // Fill CSI Part 2.
  if (fapi_pusch.csi_part2.has_value()) {
    mac_uci_pdu::pusch_type::csi_information& csi = mac_pusch.csi_part2_info.emplace();
    csi.is_valid                                  = is_fapi_uci_payload_valid(fapi_pusch.csi_part2->detection_status);
    csi.payload                                   = fapi_pusch.csi_part2->payload;
  }
}

static void convert_fapi_to_mac_pucch_f2_f3_f4_uci_ind(mac_uci_pdu::pucch_f2_or_f3_or_f4_type& mac_pucch,
                                                       const fapi::uci_pucch_pdu_format_2_3_4& fapi_pucch)
{
  mac_pucch.ul_sinr_dB          = convert_fapi_to_mac_ul_sinr(fapi_pucch.ul_sinr_metric);
  mac_pucch.rssi_dBFS           = convert_fapi_to_mac_rssi(fapi_pucch.rssi);
  mac_pucch.rsrp_dBFS           = convert_fapi_to_mac_rsrp(fapi_pucch.rsrp);
  mac_pucch.time_advance_offset = fapi_pucch.timing_advance_offset;

  // Fill SR.
  if (fapi_pucch.sr.has_value()) {
    mac_pucch.sr_info.emplace() = fapi_pucch.sr->sr_payload;
  }

  // Fill HARQ.
  if (fapi_pucch.harq.has_value()) {
    if (fapi_pucch.harq->payload.empty()) {
      mac_pucch.harq_info.emplace(mac_uci_pdu::pucch_f2_or_f3_or_f4_type::harq_information::create_undetected_harq_info(
          fapi_pucch.harq->expected_bit_length.value()));
    } else {
      mac_pucch.harq_info.emplace(mac_uci_pdu::pucch_f2_or_f3_or_f4_type::harq_information::create_detected_harq_info(
          fapi_pucch.harq->payload));
    }
  }

  // Fill CSI Part 1.
  if (fapi_pucch.csi_part1.has_value()) {
    mac_uci_pdu::pucch_f2_or_f3_or_f4_type::csi_information& csi = mac_pucch.csi_part1_info.emplace();
    csi.is_valid = is_fapi_uci_payload_valid(fapi_pucch.csi_part1->detection_status);
    csi.payload  = fapi_pucch.csi_part1->payload;
  }

  // Fill CSI Part 2.
  if (fapi_pucch.csi_part2.has_value()) {
    mac_uci_pdu::pucch_f2_or_f3_or_f4_type::csi_information& csi = mac_pucch.csi_part2_info.emplace();
    csi.is_valid = is_fapi_uci_payload_valid(fapi_pucch.csi_part2->detection_status);
    csi.payload  = fapi_pucch.csi_part2->payload;
  }
}

void fapi_to_mac_indications_fastpath_translator::on_uci_indication(const fapi::uci_indication& msg)
{
  mac_uci_indication_message mac_msg;
  mac_msg.sl_rx = msg.slot;

  if (const auto* uci_pusch = std::get_if<fapi::uci_pusch_pdu>(&msg.pdu)) {
    mac_uci_pdu& mac_pdu = mac_msg.ucis.emplace_back();
    mac_pdu.rnti         = uci_pusch->rnti;
    mac_uci_pdu::pusch_type pusch;
    convert_fapi_to_mac_pusch_uci_ind(pusch, *uci_pusch);
    mac_pdu.pdu = pusch;
  } else if (const auto* uci_pusch_format_0_1 = std::get_if<fapi::uci_pucch_pdu_format_0_1>(&msg.pdu)) {
    mac_uci_pdu& mac_pdu = mac_msg.ucis.emplace_back();
    mac_pdu.rnti         = uci_pusch_format_0_1->rnti;
    mac_uci_pdu::pucch_f0_or_f1_type pucch;
    convert_fapi_to_mac_pucch_f0_f1_uci_ind(pucch, *uci_pusch_format_0_1);
    mac_pdu.pdu = pucch;
  } else if (const auto* uci_pusch_format_2_3_4 = std::get_if<fapi::uci_pucch_pdu_format_2_3_4>(&msg.pdu)) {
    mac_uci_pdu& mac_pdu = mac_msg.ucis.emplace_back();
    mac_pdu.rnti         = uci_pusch_format_2_3_4->rnti;
    mac_uci_pdu::pucch_f2_or_f3_or_f4_type pucch;
    convert_fapi_to_mac_pucch_f2_f3_f4_uci_ind(pucch, *uci_pusch_format_2_3_4);
    mac_pdu.pdu = pucch;
  }

  if (OCUDU_UNLIKELY(logger.debug.enabled())) {
    logger.debug("Sector#{}: {}", sector_id, msg);
  }

  cell_control_handler->handle_uci(mac_msg);
}

void fapi_to_mac_indications_fastpath_translator::on_srs_indication(const fapi::srs_indication& msg)
{
  mac_srs_indication_message mac_msg;
  mac_msg.sl_rx = msg.slot;

  mac_srs_pdu& mac_pdu        = mac_msg.srss.emplace_back();
  mac_pdu.rnti                = msg.pdu.rnti;
  mac_pdu.time_advance_offset = msg.pdu.timing_advance_offset;

  if (msg.pdu.matrix) {
    mac_pdu.report = mac_srs_pdu::normalized_channel_iq_matrix{*msg.pdu.matrix};
  }

  if (msg.pdu.positioning) {
    mac_pdu.report = mac_srs_pdu::positioning_report{msg.pdu.positioning->ul_relative_toa, msg.pdu.positioning->rsrp};
  }

  if (OCUDU_UNLIKELY(logger.debug.enabled())) {
    logger.debug("Sector#{}: {}", sector_id, msg);
  }

  cell_control_handler->handle_srs(mac_msg);
}

/// Converts the given FAPI RACH occasion RSSI to dB as per SCF-222 v4.0 section 3.4.11.
static float to_prach_rssi_dB(int fapi_rssi)
{
  return (fapi_rssi - 140000) * 0.001F;
}

/// Converts the given FAPI RACH occasion RSSI to dB as per SCF-222 v4.0 section 3.4.11.
static std::optional<float> convert_fapi_to_mac_rssi_dB(uint32_t fapi_rssi)
{
  if (fapi_rssi != std::numeric_limits<decltype(fapi_rssi)>::max()) {
    return to_prach_rssi_dB(fapi_rssi);
  }
  return std::nullopt;
}

/// Converts the given FAPI RACH preamble power to dB as per SCF-222 v4.0 section 3.4.11.
static float to_prach_preamble_power_dB(int fapi_power)
{
  return static_cast<float>(fapi_power - 140000) * 0.001F;
}

/// Converts the given FAPI RACH preamble power to dB as per SCF-222 v4.0 section 3.4.11.
static std::optional<float> convert_fapi_to_mac_preamble_power_dB(uint32_t fapi_power)
{
  if (fapi_power != std::numeric_limits<decltype(fapi_power)>::max()) {
    return to_prach_preamble_power_dB(fapi_power);
  }
  return std::nullopt;
}

void fapi_to_mac_indications_fastpath_translator::on_rach_indication(const fapi::rach_indication& msg)
{
  mac_rach_indication indication;
  indication.slot_rx = msg.slot;

  mac_rach_indication::rach_occasion& occas = indication.occasions.emplace_back();
  occas.frequency_index                     = msg.pdu.ra_index;
  occas.slot_index                          = msg.pdu.slot_index;
  occas.start_symbol                        = msg.pdu.symbol_index;
  occas.rssi_dBFS                           = convert_fapi_to_mac_rssi_dB(msg.pdu.avg_rssi);

  for (const auto& preamble : msg.pdu.preambles) {
    if (!preamble.timing_advance_offset) {
      logger.warning("RACH indication for slot {}: Empty timing advance for slot index {}, ignoring PDU",
                     msg.slot,
                     msg.pdu.slot_index);
      continue;
    }

    mac_rach_indication::rach_preamble& mac_pream = occas.preambles.emplace_back();
    mac_pream.index                               = preamble.preamble_index;
    mac_pream.pwr_dBFS                            = convert_fapi_to_mac_preamble_power_dB(preamble.preamble_pwr);
    mac_pream.time_advance                        = preamble.timing_advance_offset.value();
  }

  if (OCUDU_UNLIKELY(logger.debug.enabled())) {
    logger.debug("Sector#{}: {}", sector_id, msg);
  }

  rach_handler->handle_rach_indication(indication);
}

void fapi_to_mac_indications_fastpath_translator::set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler)
{
  rach_handler = &mac_rach_handler;
}

void fapi_to_mac_indications_fastpath_translator::set_cell_pdu_handler(mac_pdu_handler& handler)
{
  pdu_handler = &handler;
}

void fapi_to_mac_indications_fastpath_translator::set_cell_crc_handler(mac_cell_control_information_handler& handler)
{
  cell_control_handler = &handler;
}
