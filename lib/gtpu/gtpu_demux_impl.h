// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "gtpu_pdu.h"
#include "gtpu_tunnel_logger.h"
#include "ocudu/gtpu/gtpu_demux.h"
#include "ocudu/gtpu/gtpu_teid_pool.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/pcap/dlt_pcap.h"
#include "ocudu/support/executors/task_executor.h"
#include "ocudu/support/rate_limiting/lockfree_token_bucket.h"
#include "fmt/format.h"
#include <mutex>
#include <random>
#include <unordered_map>

namespace ocudu {

struct gtpu_demux_tunnel_ctx_t {
  gtpu_demux_dispatch_queue&                   batched_queue;
  gtpu_tunnel_common_rx_upper_layer_interface* tunnel;
};

class gtpu_demux_impl final : public gtpu_demux
{
public:
  explicit gtpu_demux_impl(gtpu_demux_cfg_t               cfg_,
                           gtpu_teid_lingering_interface& teid_linger_checker_,
                           dlt_pcap&                      gtpu_pcap_,
                           lockfree_token_bucket*         rate_limiter_ = nullptr);
  ~gtpu_demux_impl() override = default;

  // gtpu_demux_rx_upper_layer_interface
  void handle_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) override; // Will be run from IO executor.

  // gtpu_demux_ctrl
  expected<std::unique_ptr<gtpu_demux_dispatch_queue>>
       add_tunnel(gtpu_teid_t                                  teid,
                  task_executor&                               tunnel_exec,
                  gtpu_tunnel_common_rx_upper_layer_interface* tunnel) override;
  bool remove_tunnel(gtpu_teid_t teid) override;

  void apply_test_teids(std::vector<gtpu_teid_t> teids) override;

  void stop() override;

  void set_error_indication_tx(gtpu_tunnel_common_tx_upper_layer_notifier& tx_upper,
                               const std::string&                          local_addr) override;

private:
  void send_error_indication(uint32_t teid, const sockaddr_storage& src_addr);

  /// \brief Write PDU to PCAP (if activated).
  ///
  /// This function writes a PDU to the PCAP if enabled.
  /// Only if PCAP is enabled, a deep-copy of the PDU is forwarded to the PCAP writer.
  ///
  /// \param pdu The PDU to be written to PCAP as reference.
  void write_pcap(const byte_buffer& pdu);

  // Actual demuxing, to be run in CU-UP executor.
  void handle_pdu_impl(gtpu_teid_t teid, gtpu_demux_pdu_ctx_t pdu_ctx);

  const gtpu_demux_cfg_t         cfg;
  gtpu_teid_lingering_interface& teid_linger_checker;
  dlt_pcap&                      gtpu_pcap;
  std::atomic<bool>              stopped = false;

  // The map is modified by accessed the io_broker (to get the right executor)
  // and the modified by UE executors when setting up/tearing down.
  std::mutex                                                                   map_mutex;
  std::unordered_map<gtpu_teid_t, gtpu_demux_tunnel_ctx_t, gtpu_teid_hasher_t> teid_to_tunnel;

  // Rate limiting. Lockfree, as to support parallel tunnel executors.
  lockfree_token_bucket* rate_limiter;

  // TEID(s) used for test mode operation and helpers
  // to randomly pick TEIDs from the available values.
  std::vector<gtpu_teid_t>        test_teids;
  std::random_device              rd;
  std::default_random_engine      gen;
  std::uniform_int_distribution<> dist;

  ocudulog::basic_logger& logger;

  // Error Indication TX support
  gtpu_tunnel_common_tx_upper_layer_notifier* tx_upper     = nullptr;
  gtpu_ie_gtpu_peer_address                   ei_peer_addr = {};
  uint16_t                                    ei_sn_next   = 0;
  gtpu_tunnel_logger ei_logger{"GTPU", {gtpu_tunnel_log_prefix{{}, GTPU_PATH_MANAGEMENT_TEID, "UL"}}};
};

} // namespace ocudu

namespace fmt {
// GTP-U demux config formatter
template <>
struct formatter<ocudu::gtpu_demux_cfg_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(ocudu::gtpu_demux_cfg_t cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "name={} queue_size={} batch_size={} warn_on_drop={} test_mode={}",
                     cfg.name,
                     cfg.queue_size,
                     cfg.batch_size,
                     cfg.warn_on_drop,
                     cfg.test_mode);
  }
};
} // namespace fmt
