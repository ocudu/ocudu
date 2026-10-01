// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "gtpu_tunnel_pdcp_rx_impl.h"

using namespace ocudu;

gtpu_tunnel_pdcp_rx_impl::gtpu_tunnel_pdcp_rx_impl(cu_up_ue_index_t                                    ue_index,
                                                   gtpu_tunnel_pdcp_config::gtpu_tunnel_pdcp_rx_config cfg_,
                                                   gtpu_tunnel_pdcp_rx_lower_layer_notifier&           rx_lower_) :
  gtpu_tunnel_base_rx(gtpu_tunnel_log_prefix{cfg_.lif, ue_index, cfg_.local_teid, "DL"}),
  cfg(cfg_),
  lower_dn(rx_lower_),
  pdcp_pdu_number_packer(logger.get_basic_logger()),
  long_pdcp_pdu_number_packer(logger.get_basic_logger())
{
  logger.log_info("GTP-U PDCP RX configured. {}", cfg);
  ocudu_assert(
      cfg_.lif == gtpu_logical_interface::xnu, "GTP-U PDCP RX node not correctly initialized. lif={}", cfg_.lif);
  ocudu_assert(cfg_.pdcp_sn_len == pdcp_sn_size::size12bits || cfg_.pdcp_sn_len == pdcp_sn_size::size18bits,
               "GTP-U PDCP RX node not correctly initialized. pdcp_sn_len={}",
               cfg_.pdcp_sn_len);
}

void gtpu_tunnel_pdcp_rx_impl::stop()
{
  stopped = true;
}

void gtpu_tunnel_pdcp_rx_impl::handle_pdu(gtpu_dissected_pdu&& pdu, const sockaddr_storage& src_addr)
{
  if (stopped) {
    return;
  }

  size_t      pdu_len              = pdu.buf.length();
  gtpu_teid_t teid                 = pdu.hdr.teid;
  uint32_t    pdcp_pdu_number      = 0;
  bool        have_pdcp_pdu_number = false;
  for (auto ext_hdr : pdu.hdr.ext_list) {
    switch (ext_hdr.extension_header_type) {
      case gtpu_extension_header_type::pdcp_pdu_number:
        if (cfg.pdcp_sn_len != pdcp_sn_size::size12bits) {
          logger.log_warning("Ignoring 12-bit PDCP PDU number. pdcp_sn_len={}", cfg.pdcp_sn_len);
          break;
        }
        if (!have_pdcp_pdu_number) {
          have_pdcp_pdu_number = pdcp_pdu_number_packer.unpack(pdcp_pdu_number, ext_hdr.container);
          if (!have_pdcp_pdu_number) {
            logger.log_error("Failed to unpack PDCP PDU number. pdu_len={}", pdu_len);
          }
        } else {
          logger.log_warning("Ignoring multiple PDCP PDU numbers. pdu_len={}", pdu_len);
        }
        break;
      case gtpu_extension_header_type::long_pdcp_pdu_number:
      case gtpu_extension_header_type::long_pdcp_pdu_number_legacy:
        if (cfg.pdcp_sn_len != pdcp_sn_size::size18bits) {
          logger.log_warning("Ignoring 18-bit long PDCP PDU number. pdcp_sn_len={}", cfg.pdcp_sn_len);
          break;
        }
        if (!have_pdcp_pdu_number) {
          have_pdcp_pdu_number = long_pdcp_pdu_number_packer.unpack(pdcp_pdu_number, ext_hdr.container);
          if (!have_pdcp_pdu_number) {
            logger.log_error("Failed to unpack long PDCP PDU number. pdu_len={}", pdu_len);
          }
        } else {
          logger.log_warning("Ignoring multiple long PDCP PDU numbers. pdu_len={}", pdu_len);
        }
        break;
      default:
        logger.log_warning("Ignoring unexpected extension header at Xn-U interface. type={} pdu_len={}",
                           ext_hdr.extension_header_type,
                           pdu_len);
    }
  }
  if (!have_pdcp_pdu_number) {
    logger.log_warning("Incomplete PDU at Xn-U interface: missing PDCP PDU number. pdu_len={} teid={}", pdu_len, teid);
    // TS 38.300 Sec. 9.2.3.2.3: The SN of forwarded PDCP SDUs is carried in the "PDCP PDU number"
    // field of the GTP-U extension header.
    return;
  }

  logger.log_debug(pdu.buf.begin(), pdu.buf.end(), "RX PDU. pdu_len={}", pdu_len);

  byte_buffer            rx_sdu      = gtpu_extract_msg(std::move(pdu)); // header is invalidated after extraction.
  gtpu_pdcp_rx_tpdu_info rx_sdu_info = {std::move(rx_sdu), pdcp_pdu_number};
  deliver_sdu(rx_sdu_info);
}

void gtpu_tunnel_pdcp_rx_impl::deliver_sdu(gtpu_pdcp_rx_tpdu_info& sdu_info)
{
  logger.log_info(sdu_info.tpdu.begin(),
                  sdu_info.tpdu.end(),
                  "RX SDU. sdu_len={} pdcp_pdu_num={}",
                  sdu_info.tpdu.length(),
                  sdu_info.pdcp_pdu_number);
  lower_dn.on_new_sdu(std::move(sdu_info.tpdu), sdu_info.pdcp_pdu_number);
}
