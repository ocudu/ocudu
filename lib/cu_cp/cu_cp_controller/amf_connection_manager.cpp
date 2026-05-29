// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "amf_connection_manager.h"
#include "../cu_cp_impl_interface.h"
#include "../routines/amf_connection_removal_routine.h"
#include "../routines/amf_connection_setup_routine.h"
#include "../routines/amf_reconnection_routine.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/support/synchronization/baton.h"
#include <chrono>
#include <thread>

using namespace ocudu;
using namespace ocucp;

amf_connection_manager::amf_connection_manager(ngap_repository&                  ngaps_,
                                               cu_cp_amf_reconnection_handler&   cu_cp_notifier_,
                                               timer_manager&                    timers_,
                                               task_executor&                    cu_cp_exec_,
                                               async_task_scheduler&             common_task_sched_,
                                               cu_cp_ng_setup_complete_notifier* ng_setup_notifier_) :
  ngaps(ngaps_),
  cu_cp_notifier(cu_cp_notifier_),
  timers(timers_),
  cu_cp_exec(cu_cp_exec_),
  common_task_sched(common_task_sched_),
  logger(ocudulog::fetch_basic_logger("CU-CP")),
  ng_setup_notifier(ng_setup_notifier_)
{
}

void amf_connection_manager::connect_to_amf(std::promise<bool>* completion_signal)
{
  // Schedules setup routine to be executed in sequence with other CU-CP procedures.
  common_task_sched.schedule(
      launch_async([this, success = false, p = completion_signal](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        // Launch procedure to initiate AMF connection.
        CORO_AWAIT_VALUE(success, start_amf_connection_setup(ngaps, amfs_connected, ng_setup_notifier));

        // Signal through the promise the result of the connection setup.
        if (p != nullptr) {
          p->set_value(success);
        }

        CORO_RETURN();
      }));
}

async_task<void> amf_connection_manager::disconnect_amf()
{
  if (ngaps.get_ngaps().empty() or amfs_connected.empty()) {
    return launch_async([](coro_context<async_task<void>>& ctx) {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  return start_amf_connection_removal(ngaps, amfs_connected);
}

void amf_connection_manager::handle_amf_connection_loss(cu_cp_amf_index_t amf_index)
{
  amfs_connected.erase(amf_index);
}

void amf_connection_manager::reconnect_to_amf(cu_cp_amf_index_t         amf_index,
                                              ue_manager*               ue_mng,
                                              std::chrono::milliseconds amf_reconnection_retry_time)
{
  if (ngaps.find_ngap(amf_index) == nullptr) {
    logger.debug("AMF index {} for reconnection not found", amf_index);
    return;
  }

  ngaps.get_ngap_task_scheduler().handle_amf_async_task(
      amf_index,
      launch_async([this, amf_index, success = bool{false}, ue_mng, amf_reconnection_retry_time](
                       coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        CORO_AWAIT_VALUE(success,
                         start_amf_reconnection(*ngaps.find_ngap(amf_index),
                                                timer_factory{timers, cu_cp_exec},
                                                amf_reconnection_retry_time));

        if (success) {
          // Update PLMN lookups in NGAP repository after successful reconnection.
          ngaps.update_plmn_lookup(amf_index);
          ue_mng->remove_blocked_plmns(ngaps.find_ngap(amf_index)->get_ngap_context().get_supported_plmns());
          amfs_connected.emplace(amf_index, true);
          // Notrify CU-CP about the successful reconnection.
          cu_cp_notifier.handle_amf_reconnection(amf_index);
        } else {
          logger.info("Failed to reconnect to AMF index {}", amf_index);
        }

        CORO_RETURN();
      }));
}

void amf_connection_manager::stop()
{
  if (stopped) {
    return;
  }

  baton               stop_baton;
  scoped_baton_sender signal_stop{stop_baton};

  // Stop and delete AMF connections.
  while (not cu_cp_exec.defer([this, signal_stop = std::move(signal_stop)]() mutable {
    common_task_sched.schedule(
        launch_async([this, signal_stop = std::move(signal_stop)](coro_context<async_task<void>>& ctx) mutable {
          CORO_BEGIN(ctx);
          // Disconnect AMF connection.
          CORO_AWAIT(disconnect_amf());

          // AMF disconnection successfully finished.
          // Dispatch main async task loop destruction via defer so that the current coroutine ends successfully.
          while (not cu_cp_exec.defer([signal_stop = std::move(signal_stop)]() mutable { signal_stop.post(); })) {
            logger.warning("Unable to stop AMF Manager. Retrying...");
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
          }

          CORO_RETURN();
        }));
  })) {
    logger.warning("Failed to dispatch AMF stop task. Retrying...");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Wait for AMF stop to complete.
  stop_baton.wait();

  stopped = true;
}

bool amf_connection_manager::is_amf_connected(plmn_identity plmn) const
{
  cu_cp_amf_index_t amf_index = plmn_to_amf_index(plmn);
  if (amf_index == cu_cp_amf_index_t::invalid) {
    return false;
  }

  return is_amf_connected(amf_index);
}

bool amf_connection_manager::is_amf_connected(cu_cp_amf_index_t amf_index) const
{
  const auto& amf_connected = amfs_connected.find(amf_index);
  if (amf_connected == amfs_connected.end()) {
    return false;
  }

  return amf_connected->second.load(std::memory_order_relaxed);
}

void amf_connection_manager::handle_connection_setup_result(cu_cp_amf_index_t amf_index, bool success)
{
  // Update AMF connection handler state.
  amfs_connected.emplace(amf_index, success);
}

cu_cp_amf_index_t amf_connection_manager::plmn_to_amf_index(plmn_identity plmn) const
{
  for (const auto& [amf_index, ngap] : ngaps.get_ngaps()) {
    for (auto& supported_plmn : ngap->get_ngap_context().get_supported_plmns()) {
      if (plmn == supported_plmn) {
        return amf_index;
      }
    }
  }

  return cu_cp_amf_index_t::invalid;
}
