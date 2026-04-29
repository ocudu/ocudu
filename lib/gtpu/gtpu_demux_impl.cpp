// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "gtpu_demux_impl.h"
#include "gtpu_pdu.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <utility>

using namespace ocudu;

gtpu_demux_impl::gtpu_demux_impl(gtpu_demux_cfg_t               cfg_,
                                 gtpu_teid_lingering_interface& teid_linger_checker_,
                                 dlt_pcap&                      gtpu_pcap_) :
  cfg(std::move(cfg_)),
  teid_linger_checker(teid_linger_checker_),
  gtpu_pcap(gtpu_pcap_),
  gen(rd()),
  logger(ocudulog::fetch_basic_logger("GTPU"))
{
  logger.info("GTP-U demux. {}", cfg);
}

void gtpu_demux_impl::stop()
{
  stopped.store(true, std::memory_order_relaxed);
}

expected<std::unique_ptr<gtpu_demux_dispatch_queue>>
gtpu_demux_impl::add_tunnel(gtpu_teid_t                                  teid,
                            task_executor&                               tunnel_exec,
                            gtpu_tunnel_common_rx_upper_layer_interface* tunnel)
{
  auto dispacth_fn = [this, teid](span<gtpu_demux_pdu_ctx_t> pdus_span) {
    for (gtpu_demux_pdu_ctx_t& pdu_ctx : pdus_span) {
      handle_pdu_impl(teid, pdu_ctx);
    }
  };
  auto batched_queue = std::make_unique<gtpu_demux_dispatch_queue>(
      cfg.name, cfg.queue_size, tunnel_exec, logger, dispacth_fn, cfg.batch_size);

  std::lock_guard<std::mutex> guard(map_mutex);
  auto                        it = teid_to_tunnel.try_emplace(teid, gtpu_demux_tunnel_ctx_t{*batched_queue, tunnel});
  if (not it.second) {
    logger.error("Tunnel already exists. teid={}", teid);
    return make_unexpected(default_error_t{});
  }

  logger.info("Tunnel added. teid={}", teid);
  return batched_queue;
}

bool gtpu_demux_impl::remove_tunnel(gtpu_teid_t teid)
{
  std::lock_guard<std::mutex> guard(map_mutex);
  auto                        it = teid_to_tunnel.find(teid);
  if (it == teid_to_tunnel.end()) {
    logger.error("Tunnel not found. teid={}", teid);
    return false;
  }

  logger.info("Tunnel removed. teid={}", teid);
  teid_to_tunnel.erase(it);
  return true;
}

void gtpu_demux_impl::apply_test_teids(std::vector<gtpu_teid_t> teids)
{
  std::lock_guard<std::mutex> guard(map_mutex);
  test_teids = teids;
  report_error_if_not(not teids.empty(), "Configuring GTP-U demux test mode without TEIDs");
  dist = std::uniform_int_distribution<>{0, static_cast<int>(teids.size() - 1)};
}

void gtpu_demux_impl::handle_pdu(byte_buffer pdu, const sockaddr_storage& src_addr)
{
  if (stopped.load(std::memory_order_relaxed)) {
    return;
  }

  uint32_t read_teid = 0;
  if (not cfg.test_mode) {
    if (not gtpu_read_teid(read_teid, pdu, logger)) {
      logger.error("Failed to read TEID from GTP-U PDU. pdu_len={}", pdu.length());
      return;
    }
  }

  std::lock_guard<std::mutex> guard(map_mutex);

  gtpu_teid_t teid{read_teid};

  if (cfg.test_mode) {
    teid = test_teids[dist(gen)];
  }

  auto it = teid_to_tunnel.find(teid);
  if (it == teid_to_tunnel.end()) {
    // write the PDU to PCAP before dropping it.
    write_pcap(pdu);
    bool err_ind = false;
    if (teid.value() != 0 && tx_upper != nullptr) {
      if (not teid_linger_checker.is_teid_lingering(teid)) {
        send_error_indication(read_teid, src_addr);
        err_ind = true;
      }
    }
    logger.info("Dropped GTP-U PDU, tunnel not found. teid={} err_ind={}", teid, err_ind);
    return;
  }
  if (not it->second.batched_queue.try_push(gtpu_demux_pdu_ctx_t{std::move(pdu), src_addr})) {
    if (not cfg.warn_on_drop) {
      logger.info("Dropped GTP-U PDU, queue is full. teid={}", teid);
    } else {
      logger.warning("Dropped GTP-U PDU, queue is full. teid={}", teid);
    }
  }
}

void gtpu_demux_impl::write_pcap(const byte_buffer& pdu)
{
  if (gtpu_pcap.is_write_enabled()) {
    auto pdu_copy = pdu.deep_copy();
    if (not pdu_copy.has_value()) {
      logger.warning("Unable to deep copy PDU for PCAP writer");
    } else {
      gtpu_pcap.push_pdu(std::move(pdu_copy.value()));
    }
  }
}

void gtpu_demux_impl::handle_pdu_impl(gtpu_teid_t teid, gtpu_demux_pdu_ctx_t pdu_ctx)
{
  if (stopped.load(std::memory_order_relaxed)) {
    return;
  }
  write_pcap(pdu_ctx.pdu);
  logger.debug(
      pdu_ctx.pdu.begin(), pdu_ctx.pdu.end(), "Forwarding PDU. pdu_len={} teid={}", pdu_ctx.pdu.length(), teid);

  gtpu_tunnel_common_rx_upper_layer_interface* tunnel = nullptr;
  {
    // Get GTP-U tunnel.
    // We lookup the tunnel again, as the tunnel could have been removed between the time PDU processing was enqueued
    // and the time we actually run the task.
    std::lock_guard<std::mutex> guard(map_mutex);
    auto                        it = teid_to_tunnel.find(teid);
    if (it == teid_to_tunnel.end()) {
      logger.info("Dropped GTP-U PDU, tunnel not found. teid={}", teid);
      return;
    }
    tunnel = it->second.tunnel;
  }
  // Forward entire PDU to the tunnel.
  // As removal happens in the same thread as handling the PDU, we no longer need the lock.
  tunnel->handle_pdu(std::move(pdu_ctx.pdu), pdu_ctx.src_addr);
}

void gtpu_demux_impl::set_error_indication_tx(gtpu_tunnel_common_tx_upper_layer_notifier& tx_upper_,
                                              const std::string&                          local_addr_)
{
  tx_upper = &tx_upper_;

  // Pre-compute the peer address IE from the local address string.
  struct in_addr  addr4 = {};
  struct in6_addr addr6 = {};
  if (inet_pton(AF_INET, local_addr_.c_str(), &addr4) == 1) {
    gtpu_ie_gtpu_peer_address::ipv4_addr_t ipv4 = {};
    std::memcpy(ipv4.data(), &addr4, 4);
    ei_peer_addr.gtpu_peer_address = ipv4;
  } else if (inet_pton(AF_INET6, local_addr_.c_str(), &addr6) == 1) {
    gtpu_ie_gtpu_peer_address::ipv6_addr_t ipv6 = {};
    std::memcpy(ipv6.data(), &addr6, 16);
    ei_peer_addr.gtpu_peer_address = ipv6;
  } else {
    logger.error("Invalid local address for error indication. addr={}", local_addr_);
    tx_upper = nullptr;
    return;
  }
  logger.debug("Error indication TX configured. local_addr={}", local_addr_);
}

void gtpu_demux_impl::send_error_indication(uint32_t teid, const sockaddr_storage& src_addr)
{
  if (stopped.load(std::memory_order_relaxed)) {
    return;
  }

  byte_buffer buf;

  // Write mandatory IEs: TEID-I and GTP-U Peer Address
  gtpu_ie_teid_i ie_teid = {};
  ie_teid.teid_i         = teid;
  if (!gtpu_write_ie_teid_i(buf, ie_teid, ei_logger)) {
    logger.error("Failed to write IE TEID-I for error indication. teid={:#x}", teid);
    return;
  }

  if (!gtpu_write_ie_gtpu_peer_address(buf, ei_peer_addr, ei_logger)) {
    logger.error("Failed to write IE GTP-U peer address for error indication.");
    return;
  }

  // Build GTP-U header
  gtpu_header hdr         = {};
  hdr.flags.version       = GTPU_FLAGS_VERSION_V1;
  hdr.flags.protocol_type = GTPU_FLAGS_GTP_PROTOCOL;
  hdr.flags.ext_hdr       = false;
  hdr.flags.seq_number    = true;
  hdr.message_type        = GTPU_MSG_ERROR_INDICATION;
  hdr.length              = 0;
  hdr.teid                = GTPU_PATH_MANAGEMENT_TEID;
  hdr.seq_number          = ei_sn_next++;

  if (!gtpu_write_header(buf, hdr, ei_logger)) {
    logger.error("Failed to write GTP-U header for error indication. teid={:#x}", teid);
    return;
  }

  logger.info("TX error indication. teid={:#x} pdu_len={}", teid, buf.length());
  write_pcap(buf);
  tx_upper->on_new_pdu(std::move(buf), src_addr);
}
