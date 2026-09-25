// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "../../ocudu_test_requirements.h"
#include "ocudu/adt/mutexed_mpmc_queue.h"
#include "ocudu/gateways/sctp_network_client_factory.h"
#include "ocudu/gateways/sctp_network_server_factory.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/support/executors/inline_task_executor.h"
#include "ocudu/support/executors/task_worker.h"
#include "ocudu/support/io/io_broker_factory.h"
#include <condition_variable>
#include <gtest/gtest.h>
#include <mutex>

using namespace ocudu;

namespace {

struct test_params {
  bool     dtls_enabled = false;
  unsigned nof_clients  = 1;
};

void PrintTo(const test_params& value, ::std::ostream* os)
{
  *os << fmt::format("nof_clients={}", value.nof_clients);
}

} // namespace

class base_sctp_network_link_test
{
public:
  base_sctp_network_link_test(test_params params) :
    logger([]() -> ocudulog::basic_logger& {
      ocudulog::init();
      return ocudulog::fetch_basic_logger("SCTP-GW");
    }()),
    server_broker(create_io_broker(ocudu::io_broker_type::epoll)),
    client_broker(create_io_broker(ocudu::io_broker_type::epoll)),
    assoc_factory(std::make_unique<server_assoc_handler_factory>(*this)),
    server_cfg([this, params]() {
      sctp_network_server_config cfg{{}, *server_broker, rx_executor, app_executor, *assoc_factory};
      cfg.sctp.if_name        = "SERVER";
      cfg.sctp.ppid           = NGAP_PPID;
      cfg.sctp.bind_addresses = {"127.0.0.1"};
      cfg.sctp.bind_port      = 0;
      if (params.dtls_enabled) {
        std::map<transport_layer_address, dtls_mode> mode_map = {};
        cfg.sctp.dtls_cfg                                     = {dtls_mode::server,
                                                                 "1",
                                                                 std::string(TEST_CERT_DIR) + "/link12.crt",
                                                                 std::string(TEST_CERT_DIR) + "/link12.key",
                                                                 std::string(TEST_CERT_DIR) + "/ca.crt",
                                                                 mode_map};
      }

      return cfg;
    }()),
    server(create_sctp_network_server(server_cfg))
  {
    report_fatal_error_if_not(server_broker != nullptr, "Failed to create IO broker");
    report_fatal_error_if_not(client_broker != nullptr, "Failed to create IO broker");
    report_fatal_error_if_not(server != nullptr, "Failed to create Server");

    report_fatal_error_if_not(server->listen(), "Failed to listen to connections");

    int server_port = server->get_listen_port().value();

    for (unsigned i = 0; i < params.nof_clients; i++) {
      // Create context instance.
      auto ret = client_associations.insert(std::make_pair(i, std::make_unique<sctp_client_association_context>()));
      report_fatal_error_if_not(ret.second, "Failed to insert Client Association");

      // Create client.
      sctp_network_client_config client_cfg{{}, *client_broker, rx_executor};
      client_cfg.sctp.if_name           = fmt::format("client{}", i);
      client_cfg.sctp.ppid              = NGAP_PPID;
      client_cfg.sctp.dest_name         = "server";
      client_cfg.sctp.connect_addresses = {server_cfg.sctp.bind_addresses[0]};
      client_cfg.sctp.connect_port      = server_port;
      if (params.dtls_enabled) {
        std::map<transport_layer_address, dtls_mode> mode_map = {};
        client_cfg.sctp.dtls_cfg                              = {dtls_mode::client,
                                                                 "2",
                                                                 std::string(TEST_CERT_DIR) + "/link21.crt",
                                                                 std::string(TEST_CERT_DIR) + "/link21.key",
                                                                 std::string(TEST_CERT_DIR) + "/ca.crt",
                                                                 mode_map};
        report_error_if_not(i == 0, "Only one client supported in unit tests for now");
      }
      ret.first->second->client = create_sctp_network_client(client_cfg);
      report_fatal_error_if_not(ret.first->second->client != nullptr, "Failed to create Client");
    }

    // Connect Clients.
    for (auto& assoc : client_associations) {
      assoc.second->client_sender = assoc.second->client->connect(create_client_receiver(assoc.first));
      report_fatal_error_if_not(assoc.second->client_sender != nullptr, "Failed to connect Client");
    }

    // Wait for associations to be made to the server.
    std::unique_lock<std::mutex> lock(assoc_creation_mutex);
    assoc_created_cvar.wait(lock, [this, params]() { return server_associations.size() == params.nof_clients; });

    logger.info("All UEs connected");
  }

protected:
  struct server_assoc_handler_factory : public sctp_network_association_factory {
    server_assoc_handler_factory(base_sctp_network_link_test& parent_) : parent(parent_) {}

    std::unique_ptr<sctp_association_sdu_notifier>
    create(std::unique_ptr<sctp_association_sdu_notifier> sctp_send_notifier, sctp_association_info assoc_info) override
    {
      // Note: Called from within io_broker execution context.

      class server_recv_notifier : public sctp_association_sdu_notifier
      {
      public:
        server_recv_notifier(sctp_server_association_context& parent_) : parent(parent_) {}

        bool on_new_sdu(byte_buffer sdu) override { return parent.recv_data.try_push(std::move(sdu)); }

      private:
        sctp_server_association_context& parent;
      };

      // Create new SCTP server association test context.
      auto assoc_ctxt           = std::make_unique<sctp_server_association_context>();
      assoc_ctxt->server_sender = std::move(sctp_send_notifier);
      auto receiver             = std::make_unique<server_recv_notifier>(*assoc_ctxt);

      {
        std::lock_guard<std::mutex> lock(parent.assoc_creation_mutex);
        auto ret = parent.server_associations.insert(std::make_pair(next_server_assoc_key++, std::move(assoc_ctxt)));
        report_fatal_error_if_not(ret.second, "Failed to create SCTP association handler");
        parent.assoc_created_cvar.notify_one();
      }

      return receiver;
    }

  private:
    base_sctp_network_link_test& parent;
    unsigned                     next_server_assoc_key = 0;
  };

  struct sctp_client_association_context {
    // Data the client received.
    concurrent_queue<byte_buffer,
                     concurrent_queue_policy::locking_mpmc,
                     concurrent_queue_wait_policy::condition_variable>
        recv_data{1024};

    std::unique_ptr<sctp_network_client>           client;
    std::unique_ptr<sctp_association_sdu_notifier> client_sender;
  };

  struct sctp_server_association_context {
    // Data the server association received.
    concurrent_queue<byte_buffer,
                     concurrent_queue_policy::locking_mpmc,
                     concurrent_queue_wait_policy::condition_variable>
        recv_data{1024};

    std::unique_ptr<sctp_association_sdu_notifier> server_sender;
  };

  std::unique_ptr<sctp_association_sdu_notifier> create_client_receiver(unsigned key)
  {
    class client_recv_notifier : public sctp_association_sdu_notifier
    {
    public:
      client_recv_notifier(sctp_client_association_context& parent_) : parent(parent_) {}

      bool on_new_sdu(byte_buffer sdu) override { return parent.recv_data.try_push(std::move(sdu)); }

    private:
      sctp_client_association_context& parent;
    };

    return std::make_unique<client_recv_notifier>(*client_associations.at(key));
  }

  ocudulog::basic_logger&                           logger;
  inline_task_executor                              rx_executor;
  inline_task_executor                              app_executor;
  std::unique_ptr<io_broker>                        server_broker;
  std::unique_ptr<io_broker>                        client_broker;
  std::unique_ptr<sctp_network_association_factory> assoc_factory;
  sctp_network_server_config                        server_cfg;

  std::unique_ptr<sctp_network_server> server;

  std::map<unsigned, std::unique_ptr<sctp_server_association_context>> server_associations;
  std::map<unsigned, std::unique_ptr<sctp_client_association_context>> client_associations;

  std::mutex              assoc_creation_mutex;
  std::condition_variable assoc_created_cvar;
};

class sctp_network_link_test : public base_sctp_network_link_test, public ::testing::TestWithParam<test_params>
{
public:
  sctp_network_link_test() : base_sctp_network_link_test(GetParam()) { ::signal(SIGPIPE, SIG_IGN); }
  ~sctp_network_link_test() override
  {
    if (server) {
      server->stop();
    }
    ocudulog::flush();
  }
};

static byte_buffer create_data(unsigned start_val, unsigned nof_vals)
{
  std::vector<uint8_t> vec(nof_vals);
  for (unsigned i = 0; i != vec.size(); ++i) {
    vec[i] = start_val + i;
  }
  return byte_buffer::create(vec).value();
}

TEST_P(sctp_network_link_test, multi_client_recv_data)
{
#ifdef OCUDU_HAVE_OPENSSL_DTLS
  OCUDU_TEST_REQUIREMENTS("MVP-SEC-O-CU-01b", "MVP-SEC-O-CU-10b", "MVP-SEC-O-CU-09b");
#endif

  unsigned pdu_len = 10;

  // Send data from each server association sender.
  unsigned i = 0;
  for (auto& assoc : server_associations) {
    byte_buffer pdu = create_data(i * pdu_len, pdu_len);
    ASSERT_TRUE(assoc.second->server_sender->on_new_sdu(std::move(pdu)));
  }

  // Check data received by client.
  for (auto& assoc : client_associations) {
    byte_buffer pdu;
    ASSERT_TRUE(assoc.second->recv_data.pop_blocking(pdu));
    EXPECT_EQ(pdu.length(), pdu_len);
  }
}

TEST_P(sctp_network_link_test, multi_client_send_data)
{
#ifdef OCUDU_HAVE_OPENSSL_DTLS
  OCUDU_TEST_REQUIREMENTS("MVP-SEC-O-CU-01b", "MVP-SEC-O-CU-10b", "MVP-SEC-O-CU-09b");
#endif

  unsigned pdu_len = 10;

  // Send data from each client association.
  unsigned i = 0;
  for (auto& assoc : client_associations) {
    byte_buffer pdu = create_data(i * pdu_len, pdu_len);
    ASSERT_TRUE(assoc.second->client_sender->on_new_sdu(std::move(pdu)));
  }

  // Check data received by each server association.
  for (auto& assoc : server_associations) {
    byte_buffer pdu;
    ASSERT_TRUE(assoc.second->recv_data.pop_blocking(pdu));
    EXPECT_EQ(pdu.length(), pdu_len);
  }
}

#ifdef OCUDU_HAVE_OPENSSL_DTLS
INSTANTIATE_TEST_SUITE_P(sctp_multi_client_test,
                         sctp_network_link_test,
                         ::testing::Values(test_params{true, 1},
                                           test_params{false, 1},
                                           test_params{false, 4},
                                           test_params{false, 8},
                                           test_params{false, 32}));
#else
INSTANTIATE_TEST_SUITE_P(
    sctp_multi_client_test,
    sctp_network_link_test,
    ::testing::Values(test_params{false, 1}, test_params{false, 4}, test_params{false, 8}, test_params{false, 32}));
#endif
