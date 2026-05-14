// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "xnap_repository.h"
#include "ocudu/cu_cp/cu_cp_types.h"
#include "ocudu/support/ocudu_assert.h"
#include "ocudu/xnap/xnap_factory.h"
#include <algorithm>

using namespace ocudu;
using namespace ocucp;

xnap_repository::xnap_repository(xnap_repository_config cfg_) :
  cfg(cfg_), logger(cfg.logger), xnc_task_sched(cfg.cu_cp.xnap.xnaps.size(), logger)
{
}

std::map<xnc_peer_index_t, xnap_interface*> xnap_repository::get_xnaps()
{
  std::map<xnc_peer_index_t, xnap_interface*> xnaps;
  for (auto& peer : xnap_db) {
    xnaps.emplace(peer.first, peer.second.xnap.get());
  }
  return xnaps;
}

xnap_interface* xnap_repository::add_xnap(xnc_peer_index_t                            xnc_index,
                                          const std::vector<transport_layer_address>& peer_addrs,
                                          const xnap_configuration&                   xnap_cfg)
{
  auto it = xnap_db.insert(std::make_pair(xnc_index, xnap_context{}));
  ocudu_assert(it.second, "Unable to insert XNAP in map");
  xnap_context& xnap_ctxt = it.first->second;
  xnap_ctxt.peer_addrs    = peer_addrs;
  xnap_ctxt.xnap_to_cu_cp_notifier.connect_cu_cp(cfg.cu_cp_notifier, xnc_index);

  std::unique_ptr<xnap_interface> xnap_entity = create_xnap(xnc_index,
                                                            xnap_cfg,
                                                            xnap_ctxt.xnap_to_cu_cp_notifier,
                                                            *cfg.cu_cp.services.timers,
                                                            *cfg.cu_cp.services.cu_cp_executor);
  if (xnap_entity == nullptr) {
    logger.error("Failed to create XNAP");
    xnap_db.erase(it.first);
    return nullptr;
  }

  xnap_ctxt.xnap = std::move(xnap_entity);

  return xnap_ctxt.xnap.get();
}

xnap_interface* xnap_repository::find_xnap(xnc_peer_index_t xnc_index)
{
  auto it = xnap_db.find(xnc_index);
  if (it == xnap_db.end()) {
    return nullptr;
  }
  return it->second.xnap.get();
}

xnc_peer_index_t xnap_repository::find_xnap(const transport_layer_address& peer_addr)
{
  auto it = std::find_if(
      xnap_db.begin(), xnap_db.end(), [&peer_addr](const std::pair<const xnc_peer_index_t, xnap_context>& xn) {
        return std::find(xn.second.peer_addrs.begin(), xn.second.peer_addrs.end(), peer_addr) !=
               xn.second.peer_addrs.end();
      });
  if (it == xnap_db.end()) {
    return xnc_peer_index_t::invalid;
  }
  return it->first;
}

xnap_interface* xnap_repository::find_xnap(const gnb_id_t& peer_gnb_id)
{
  for (const auto& xn : xnap_db) {
    if (xn.second.xnap->has_peer_gnb_id(peer_gnb_id)) {
      return xn.second.xnap.get();
    }
  }
  return nullptr;
}

std::optional<std::vector<transport_layer_address>> xnap_repository::get_peer_addrs(xnc_peer_index_t xnc_index) const
{
  auto it = xnap_db.find(xnc_index);
  if (it == xnap_db.end()) {
    return std::nullopt;
  }
  return it->second.peer_addrs;
}

void xnap_repository::connect_association(xnc_peer_index_t idx, std::unique_ptr<xnap_message_notifier> sender_notifier)
{
  auto it = xnap_db.find(idx);
  if (it == xnap_db.end()) {
    return;
  }
  xnap_context& ctx = it->second;
  ctx.xnap->set_tx_association_notifier(std::move(sender_notifier));
}

async_task<void> xnap_repository::remove_xnap(xnc_peer_index_t idx)
{
  ocudu_assert(idx != xnc_peer_index_t::invalid, "Invalid xnc_peer_index={}", idx);
  logger.debug("Removing XNAP {}...", idx);

  return launch_async([this, idx](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    // Remove XNAP
    if (xnap_db.find(idx) == xnap_db.end()) {
      logger.warning("Remove XNAP called for inexistent xnc_peer_index={}", idx);
      return;
    }

    // Stop XNAP activity, eliminating pending transactions for the XNAP and respective UEs.
    CORO_AWAIT(xnap_db.find(idx)->second.xnap->stop());

    // Remove XNAP
    xnap_db.erase(idx);
    logger.info("Removed XNAP {}", idx);

    CORO_RETURN();
  });
}
