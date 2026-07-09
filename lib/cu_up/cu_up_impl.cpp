// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cu_up_impl.h"
#include "cu_up_manager_impl.h"
#include "ngu_session_manager_impl.h"
#include "routines/cu_up_setup_routine.h"
#include "ocudu/e1ap/cu_up/e1ap_cu_up_factory.h"
#include "ocudu/gtpu/gtpu_demux_factory.h"
#include "ocudu/gtpu/gtpu_echo_factory.h"
#include "ocudu/gtpu/gtpu_teid_pool_factory.h"
#include "ocudu/support/executors/execute_until_success.h"
#include <future>

using namespace ocudu;
using namespace ocuup;

static void assert_cu_up_dependencies_valid(const cu_up_dependencies& dependencies)
{
  ocudu_assert(dependencies.exec_mapper != nullptr, "Invalid CU-UP UE executor pool");
  ocudu_assert(not dependencies.e1_conn_clients.empty(), "Invalid E1 connection client(s)");
  ocudu_assert(dependencies.f1u_gateway != nullptr, "Invalid F1-U connector");
  ocudu_assert(not dependencies.ngu_gws.empty(), "Invalid N3 gateway list");
  for (auto* gw : dependencies.ngu_gws) {
    ocudu_assert(gw != nullptr, "Invalid N3 gateway");
  }
  for (auto* e1 : dependencies.e1_conn_clients) {
    ocudu_assert(e1 != nullptr, "Invalid E1 gateway");
  }
  ocudu_assert(dependencies.gtpu_pcap != nullptr, "Invalid GTP-U pcap");
}

static cu_up_manager_impl_config generate_cu_up_manager_impl_config(const cu_up_config& config)
{
  return {config.cu_up_id,
          config.cu_up_name,
          config.max_nof_ues,
          config.plmns,
          config.qos,
          config.n3_cfg,
          config.test_mode_cfg};
}

static cu_up_manager_impl_dependencies
generate_cu_up_manager_impl_dependencies(std::atomic<bool>&                                  stop_command,
                                         const cu_up_dependencies&                           dependencies,
                                         std::vector<std::reference_wrapper<e1ap_interface>> e1aps,
                                         gtpu_demux&                                         ngu_demux,
                                         ngu_session_manager&                                ngu_session_mngr,
                                         gtpu_teid_pool&                                     n3_teid_allocator,
                                         fifo_async_task_scheduler&                          main_ctrl_loop)
{
  return {stop_command,
          e1aps,
          ngu_demux,
          ngu_session_mngr,
          n3_teid_allocator,
          *dependencies.f1u_teid_allocator,
          *dependencies.exec_mapper,
          *dependencies.f1u_gateway,
          *dependencies.timers,
          *dependencies.gtpu_pcap,
          main_ctrl_loop};
}

cu_up::cu_up(const cu_up_config& config_, const cu_up_dependencies& dependencies) :
  cfg(config_),
  ctrl_executor(dependencies.exec_mapper->ctrl_executor()),
  timers(*dependencies.timers),
  e1_setup_notifier(dependencies.e1_setup_notifier),
  main_ctrl_loop(128)
{
  assert_cu_up_dependencies_valid(dependencies);

  /// > Create and connect upper layers

  // Create N3 TEID allocator
  gtpu_allocator_creation_request n3_alloc_msg = {.max_nof_teids            = cfg.max_nof_ues * MAX_NOF_PDU_SESSIONS,
                                                  .teid_release_linger_time = cfg.n3_cfg.gtpu_teid_release_linger_time,
                                                  .timers                   = timers};

  n3_teid_allocator = create_gtpu_allocator(n3_alloc_msg);

  // Create N3 GTP-U demux
  gtpu_demux_creation_request demux_msg = {};
  demux_msg.cfg.name                    = "NG-U-DEMUX";
  demux_msg.cfg.warn_on_drop            = cfg.n3_cfg.warn_on_drop;
  demux_msg.cfg.queue_size              = cfg.n3_cfg.gtpu_queue_size;
  demux_msg.cfg.batch_size              = cfg.n3_cfg.gtpu_batch_size;
  demux_msg.cfg.test_mode               = cfg.test_mode_cfg.enabled;
  demux_msg.teid_linger_checker         = n3_teid_allocator.get();
  demux_msg.gtpu_pcap                   = dependencies.gtpu_pcap;
  demux_msg.rate_limiter                = n3_limiter.has_value() ? &n3_limiter.value() : nullptr;
  ngu_demux                             = create_gtpu_demux(demux_msg);

  echo_exec_mapper = dependencies.exec_mapper->create_ue_executor_mapper();
  report_error_if_not(echo_exec_mapper != nullptr, "Could not create CU-UP executor for control TEID");

  // Create N3 GTP-U echo and register it at demux
  gtpu_echo_creation_message ngu_echo_msg                      = {};
  ngu_echo_msg.gtpu_pcap                                       = dependencies.gtpu_pcap;
  ngu_echo_msg.tx_upper                                        = &gtpu_gw_adapter;
  ngu_echo                                                     = create_gtpu_echo(ngu_echo_msg);
  expected<std::unique_ptr<gtpu_demux_dispatch_queue>> batch_q = ngu_demux->add_tunnel(
      GTPU_PATH_MANAGEMENT_TEID, echo_exec_mapper->dl_pdu_executor(), ngu_echo->get_rx_upper_layer_interface());
  report_error_if_not(batch_q.has_value(), "Could not create GTP-U echo tunnel.");
  echo_batched_queue = std::move(batch_q.value());

  // Connect GTP-U DEMUX to adapter.
  gw_data_gtpu_demux_adapter.connect_gtpu_demux(*ngu_demux);

  // Establish new NG-U session and connect the instantiated session to the GTP-U DEMUX adapter, so that the latter
  // is called when new NG-U DL PDUs are received.
  for (gtpu_gateway* gw : dependencies.ngu_gws) {
    std::unique_ptr<gtpu_tnl_pdu_session> ngu_session = gw->create(gw_data_gtpu_demux_adapter);
    if (ngu_session == nullptr) {
      report_error("Unable to allocate the required NG-U network resources");
    }
    ngu_sessions.push_back(std::move(ngu_session));
  }
  ngu_session_mngr = std::make_unique<ngu_session_manager_impl>(ngu_sessions);

  // Connect GTPU GW adapter to NG-U session in order to send UL PDUs.
  // We use the first UDP GW for UL.
  gtpu_gw_adapter.connect_network_gateway(*ngu_sessions[0]);

  // Configure GTP-U Error Indication TX on the demux.
  {
    std::string n3_bind_addr;
    if (ngu_sessions[0]->get_bind_address(n3_bind_addr)) {
      ngu_demux->set_error_indication_tx(gtpu_gw_adapter, n3_bind_addr);
    }
  }

  /// > Create E1AP(s).
  e1ap_cu_up_mng_adapters.reserve(dependencies.e1_conn_clients.size());
  std::vector<std::reference_wrapper<e1ap_interface>> e1ap_refs;
  for (uint16_t e1_index = 0; e1_index < dependencies.e1_conn_clients.size(); e1_index++) {
    auto* e1_gw = dependencies.e1_conn_clients[e1_index];
    e1ap_cu_up_mng_adapters.emplace_back();
    e1ap_cu_up_manager_adapter&     e1ap_cu_up_mng_adapter = e1ap_cu_up_mng_adapters.back();
    std::unique_ptr<e1ap_interface> e1ap                   = create_e1ap(cu_up_e1_index_t{e1_index},
                                                       cfg.e1ap,
                                                       *e1_gw,
                                                       e1ap_cu_up_mng_adapter,
                                                       *dependencies.timers,
                                                       dependencies.exec_mapper->ctrl_executor());
    e1ap_refs.emplace_back(*e1ap);
    e1aps.push_back(std::move(e1ap));
  }

  /// > Create CU-UP manager
  cu_up_mng = std::make_unique<cu_up_manager_impl>(
      generate_cu_up_manager_impl_config(cfg),
      generate_cu_up_manager_impl_dependencies(
          stop_command, dependencies, e1ap_refs, *ngu_demux, *ngu_session_mngr, *n3_teid_allocator, main_ctrl_loop));

  /// > Connect E1AP(s) to CU-UP manager.
  for (auto& e1ap_cu_up_mng_adapter : e1ap_cu_up_mng_adapters) {
    e1ap_cu_up_mng_adapter.connect_cu_up_manager(*cu_up_mng);
  }

  // Start statistics report timer
  if (cfg.statistics_report_period.count() > 0) {
    statistics_report_timer = dependencies.timers->create_unique_timer(dependencies.exec_mapper->ctrl_executor());
    statistics_report_timer.set(cfg.statistics_report_period, [this]() { on_statistics_report_timer_expired(); });
    statistics_report_timer.run();
  }
}

cu_up::~cu_up()
{
  stop();
}

void cu_up::start()
{
  logger.info("CU-UP starting...");

  std::unique_lock<std::mutex> lock(mutex);
  if (std::exchange(running, true)) {
    logger.warning("CU-UP already started. Ignoring start request");
    return;
  }

  std::promise<void> p;
  std::future<void>  fut = p.get_future();

  bool connected = false;
  if (not ctrl_executor.execute([this, &p, &connected]() {
        main_ctrl_loop.schedule(
            [this, &p, &connected, e1ap = e1aps.end()](coro_context<async_task<void>>& ctx) mutable {
              CORO_BEGIN(ctx);

              // Connect to CU-CP and send E1 Setup Request and await for E1 setup response.
              for (e1ap = e1aps.begin(); e1ap != e1aps.end(); ++e1ap) {
                CORO_AWAIT_VALUE(connected,
                                 launch_async<cu_up_setup_routine>(
                                     cfg.cu_up_id, cfg.cu_up_name, cfg.plmns, **e1ap, e1_setup_notifier));
              }

              if (cfg.test_mode_cfg.enabled) {
                logger.info("enabling test mode...");
                CORO_AWAIT(cu_up_mng->enable_test_mode());
                logger.info("test mode enabled");
              }

              // Signal start() caller thread that the operation is complete.
              p.set_value();

              CORO_RETURN();
            });
      })) {
    report_fatal_error("Unable to initiate CU-UP setup routine");
  }

  // Block waiting for CU-UP setup to complete.
  fut.wait();
  if (not connected) {
    report_error("CU-UP failed to connect to CU-CP");
  }
  logger.info("CU-UP started successfully");
}

void cu_up::stop()
{
  std::unique_lock<std::mutex> lock(mutex);
  if (not running) {
    return;
  }

  logger.debug("CU-UP stopping...");

  std::condition_variable cvar;

  auto stop_cu_up_main_loop = [this, &cvar]() mutable {
    // Dispatch coroutine to stop CU-UP.
    main_ctrl_loop.schedule(
        launch_async([this, &cvar, e1ap = e1aps.end()](coro_context<async_task<void>>& ctx) mutable {
          CORO_BEGIN(ctx);

          if (not running) {
            // Already stopped.
            CORO_EARLY_RETURN();
          }

          // Run E1 Release Procedure.
          for (e1ap = e1aps.begin(); e1ap != e1aps.end(); ++e1ap) {
            CORO_AWAIT((*e1ap)->handle_cu_up_e1ap_release_request());
          }

          // CU-UP stops listening to new GTPU Rx PDUs and stops pushing UL PDUs.
          CORO_AWAIT(handle_stop_command());

          // We defer main ctrl loop stop to let the current coroutine complete successfully.
          defer_until_success(ctrl_executor, timers, [this, &cvar]() {
            // Stop main control loop and communicate back with the caller thread.
            auto main_loop = main_ctrl_loop.request_stop();

            std::lock_guard<std::mutex> lock2(mutex);
            running = false;
            cvar.notify_all();
          });

          CORO_RETURN();
        }));
  };

  // Dispatch task to stop CU-UP main loop.
  stop_command = true;
  defer_until_success(ctrl_executor, timers, stop_cu_up_main_loop);

  // Wait until the all tasks of the main loop are completed and main loop has stopped.
  cvar.wait(lock, [this]() { return not running; });

  // CU-UP stops listening to new GTPU Rx PDUs.
  ngu_sessions.clear();

  logger.info("CU-UP stopped successfully");
}

async_task<void> cu_up::handle_stop_command()
{
  // Stop statistics report timer.
  statistics_report_timer.stop();

  gtpu_gw_adapter.disconnect();

  for (auto& e1ap_cu_up_mng_adapter : e1ap_cu_up_mng_adapters) {
    e1ap_cu_up_mng_adapter.disconnect();
  }
  // Do not disconnect GTP-U Demux as it is being concurrently accessed from the thread pool.
  // It will be safely stopped from inside the CU-UP manager.

  return launch_async([this](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    // Stop CU-UP manager and remove all UEs.
    CORO_AWAIT(cu_up_mng->stop());

    CORO_RETURN();
  });
}

void cu_up::on_statistics_report_timer_expired()
{
  // Log statistics
  // TODO sum E1AP statistics.
  logger.debug("num_e1ap_ues={} num_cu_up_ues={}", e1aps[0]->get_nof_ues(), cu_up_mng->get_nof_ues());

  // Restart timer
  statistics_report_timer.set(cfg.statistics_report_period, [this]() { on_statistics_report_timer_expired(); });
  statistics_report_timer.run();
}
