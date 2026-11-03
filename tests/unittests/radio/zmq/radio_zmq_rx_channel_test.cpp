// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_zmq_rx_channel.h"
#include "radio_zmq_rx_channel_fsm.h"
#include "ocudu/radio/radio_event_notifier.h"
#include "ocudu/support/executors/task_worker.h"
#include <algorithm>
#include <chrono>
#include <future>
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <zmq.h>

using namespace ocudu;

namespace {

/// Radio event notifier that ignores every event.
class radio_notifier_spy : public radio_event_notifier
{
public:
  void on_radio_rt_event(const event_description& description) override {}
};

class RadioZmqRxChannelFixture : public ::testing::Test
{
protected:
  /// Samples carried by each response.
  static constexpr unsigned block_size = 16;
  /// Maximum time the test waits for the channel to talk to a peer, longer than the channel wait timeout.
  static constexpr int peer_timeout_ms = 5000;
  /// Time without a peer that exceeds more than two channel wait timeouts.
  static constexpr std::chrono::milliseconds absent_peer_time{2500};

  void SetUp() override
  {
    context = ::zmq_ctx_new();
    ASSERT_NE(context, nullptr);

    // Reserve a free TCP port, so that peers can come and go on the same endpoint.
    void* probe = ::zmq_socket(context, ZMQ_REP);
    ASSERT_EQ(::zmq_bind(probe, "tcp://127.0.0.1:*"), 0);
    char   last_endpoint[256];
    size_t size = sizeof(last_endpoint);
    ASSERT_EQ(::zmq_getsockopt(probe, ZMQ_LAST_ENDPOINT, last_endpoint, &size), 0);
    endpoint = last_endpoint;
    close_socket(probe);
  }

  void TearDown() override
  {
    // A failed assertion leaves the channel running and the peers open, and the context cannot terminate with them.
    if (channel != nullptr) {
      channel->stop();
    }
    channel.reset();
    worker.stop();
    for (void* peer : peers) {
      close_socket(peer);
    }
    ::zmq_ctx_term(context);
  }

  static void close_socket(void* sock)
  {
    int linger = 0;
    ::zmq_setsockopt(sock, ZMQ_LINGER, &linger, sizeof(linger));
    ::zmq_close(sock);
  }

  /// Closes a peer created by the test, so that the peer is not closed again at the end of the test.
  void close_peer(void* sock)
  {
    peers.erase(std::remove(peers.begin(), peers.end(), sock), peers.end());
    close_socket(sock);
  }

  /// Creates a socket of the given type bound to the test endpoint. It is closed at the end of the test if still open.
  void* create_peer(int type)
  {
    void* sock = ::zmq_socket(context, type);
    EXPECT_NE(sock, nullptr);
    peers.push_back(sock);
    int timeout = peer_timeout_ms;
    ::zmq_setsockopt(sock, ZMQ_RCVTIMEO, &timeout, sizeof(timeout));

    // The endpoint of a closed peer can take a moment to be released.
    for (unsigned i = 0; i != 50; ++i) {
      if (::zmq_bind(sock, endpoint.c_str()) == 0) {
        return sock;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    ADD_FAILURE() << "Failed to bind " << endpoint;
    return sock;
  }

  /// Creates and starts a channel that requests samples from the test endpoint.
  void start_channel()
  {
    radio_zmq_rx_channel::channel_description config = {};
    config.socket_type                               = ZMQ_REQ;
    config.address                                   = endpoint;
    config.channel_id_str                            = "radio_zmq_rx_channel_test";
    config.log_level                                 = ocudulog::basic_levels::warning;
    config.buffer_size                               = 1024;

    channel = std::make_unique<radio_zmq_rx_channel>(context, config, notifier, *executor);
    ASSERT_TRUE(channel->is_successful());
    channel->start();
  }

  /// Receives one multipart message and returns whether it arrived before the timeout.
  static bool receive_message(void* sock, int flags = 0)
  {
    bool received = false;
    int  more     = 1;
    while (more != 0) {
      zmq_msg_t msg;
      ::zmq_msg_init(&msg);
      if (::zmq_msg_recv(&msg, sock, flags) < 0) {
        ::zmq_msg_close(&msg);
        return received;
      }
      received    = true;
      size_t size = sizeof(more);
      ::zmq_getsockopt(sock, ZMQ_RCVMORE, &more, &size);
      ::zmq_msg_close(&msg);
    }
    return received;
  }

  void*                                 context = nullptr;
  std::vector<void*>                    peers;
  std::string                           endpoint;
  radio_notifier_spy                    notifier;
  task_worker                           worker{"zmq_rx_test", 16};
  std::unique_ptr<task_executor>        executor = make_task_executor_ptr(worker);
  std::unique_ptr<radio_zmq_rx_channel> channel;
};

} // namespace

TEST_F(RadioZmqRxChannelFixture, recovers_when_the_peer_dies_without_replying)
{
  // The first peer receives the request and dies without replying.
  void* first_peer = create_peer(ZMQ_REP);
  start_channel();
  uint8_t request = 0;
  ASSERT_EQ(::zmq_recv(first_peer, &request, sizeof(request), 0), 1);
  close_peer(first_peer);

  // A new peer on the same endpoint must receive a new request and be able to stream.
  void* second_peer = create_peer(ZMQ_REP);
  ASSERT_EQ(::zmq_recv(second_peer, &request, sizeof(request), 0), 1) << "The channel did not send a new request.";

  std::vector<cf_t> block(block_size, cf_t(1.0F, -2.0F));
  ASSERT_EQ(::zmq_send(second_peer, block.data(), block.size() * sizeof(cf_t), 0),
            static_cast<int>(block.size() * sizeof(cf_t)));

  std::vector<cf_t> samples(block_size);
  channel->receive(samples);
  EXPECT_EQ(samples, block);

  channel->stop();
  close_peer(second_peer);
}

TEST_F(RadioZmqRxChannelFixture, does_not_queue_requests_while_the_peer_is_absent)
{
  // The channel keeps trying to send requests while no peer is connected.
  start_channel();
  std::this_thread::sleep_for(absent_peer_time);

  // Only the request sent after the peer connects is delivered. The channel does not send a second one before its wait
  // timeout, so the peer is polled for less than that.
  void* peer = create_peer(ZMQ_ROUTER);
  ASSERT_TRUE(receive_message(peer));
  unsigned nof_requests = 1;
  auto     deadline     = std::chrono::steady_clock::now() + std::chrono::milliseconds(500);
  while (std::chrono::steady_clock::now() < deadline) {
    if (receive_message(peer, ZMQ_DONTWAIT)) {
      ++nof_requests;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  EXPECT_EQ(nof_requests, 1);

  channel->stop();
  close_peer(peer);
}

TEST_F(RadioZmqRxChannelFixture, stops_while_the_peer_is_absent)
{
  start_channel();
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  std::promise<void> stopped;
  std::future<void>  is_stopped = stopped.get_future();
  std::thread        stopper([this, &stopped]() {
    channel->stop();
    stopped.set_value();
  });
  bool               in_time = is_stopped.wait_for(std::chrono::seconds(3)) == std::future_status::ready;

  // A late peer releases a channel that is blocked sending, so that the test does not hang when the stop is late.
  void* peer = nullptr;
  if (!in_time) {
    peer = create_peer(ZMQ_REP);
  }
  stopper.join();
  if (peer != nullptr) {
    close_peer(peer);
  }
  EXPECT_TRUE(in_time) << "The channel did not stop while waiting for a peer.";
}

TEST_F(RadioZmqRxChannelFixture, slow_peer_does_not_lose_blocks)
{
  void* peer         = create_peer(ZMQ_REP);
  int   peer_wait_ms = 500;
  ::zmq_setsockopt(peer, ZMQ_RCVTIMEO, &peer_wait_ms, sizeof(peer_wait_ms));
  start_channel();

  std::thread peer_thread([peer]() {
    uint8_t request = 0;
    for (unsigned i = 0; i != 6; ++i) {
      if (::zmq_recv(peer, &request, sizeof(request), 0) != 1) {
        return;
      }
      // Only the first reply comes after the channel wait timeout.
      if (i == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
      }
      std::vector<cf_t> block(block_size, cf_t(float(i), 0.0F));
      ::zmq_send(peer, block.data(), block.size() * sizeof(cf_t), 0);
    }
  });

  std::vector<float> got;
  for (unsigned i = 0; i != 4; ++i) {
    std::vector<cf_t> samples(block_size);
    channel->receive(samples);
    got.push_back(samples[0].real());
  }
  EXPECT_EQ(got, (std::vector<float>{0, 1, 2, 3}));

  channel->stop();
  peer_thread.join();
  close_peer(peer);
}

TEST_F(RadioZmqRxChannelFixture, keeps_the_reply_of_a_peer_that_leaves_after_replying)
{
  void* first_peer = create_peer(ZMQ_REP);
  start_channel();
  uint8_t request = 0;
  ASSERT_EQ(::zmq_recv(first_peer, &request, sizeof(request), 0), 1);

  // Park the channel task, so that the reply and the disconnection are both waiting when it resumes.
  auto                     parked = std::make_shared<std::promise<void>>();
  std::promise<void>       release;
  std::shared_future<void> released = release.get_future().share();
  ASSERT_TRUE(executor->defer([parked, released]() {
    parked->set_value();
    released.wait_for(std::chrono::seconds(10));
  }));
  ASSERT_EQ(parked->get_future().wait_for(std::chrono::seconds(5)), std::future_status::ready);

  // The first peer replies and leaves. It lingers on close to deliver the reply before disconnecting.
  std::vector<cf_t> first_block(block_size, cf_t(1.0F, 0.0F));
  ASSERT_EQ(::zmq_send(first_peer, first_block.data(), first_block.size() * sizeof(cf_t), 0),
            static_cast<int>(first_block.size() * sizeof(cf_t)));
  int linger = 1000;
  ::zmq_setsockopt(first_peer, ZMQ_LINGER, &linger, sizeof(linger));
  peers.erase(std::remove(peers.begin(), peers.end(), first_peer), peers.end());
  ::zmq_close(first_peer);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  release.set_value();

  // A second peer receives the following request.
  void* second_peer = create_peer(ZMQ_REP);
  ASSERT_EQ(::zmq_recv(second_peer, &request, sizeof(request), 0), 1) << "The channel did not send a new request.";
  std::vector<cf_t> second_block(block_size, cf_t(2.0F, 0.0F));
  ASSERT_EQ(::zmq_send(second_peer, second_block.data(), second_block.size() * sizeof(cf_t), 0),
            static_cast<int>(second_block.size() * sizeof(cf_t)));

  // No block is lost.
  std::vector<cf_t> samples(block_size);
  channel->receive(samples);
  ASSERT_EQ(samples, first_block);
  channel->receive(samples);
  EXPECT_EQ(samples, second_block);

  channel->stop();
  close_peer(second_peer);
}

TEST(RadioZmqRxChannelFsmTest, ignores_a_disconnection_while_no_response_is_pending)
{
  radio_zmq_rx_channel_fsm fsm;
  fsm.init_successful();

  // The next request is queued and delivered to the next peer, so the disconnection does not carry over to it.
  fsm.on_disconnected();
  ASSERT_FALSE(fsm.has_disconnected());
  fsm.request_sent();
  ASSERT_TRUE(fsm.has_pending_response());
  ASSERT_FALSE(fsm.has_disconnected());
}

TEST(RadioZmqRxChannelFsmTest, requests_again_after_the_socket_is_reopened)
{
  radio_zmq_rx_channel_fsm fsm;
  fsm.init_successful();
  fsm.request_sent();

  // The reply of a lost peer can still be received until the socket is replaced.
  fsm.on_disconnected();
  ASSERT_TRUE(fsm.has_disconnected());
  ASSERT_TRUE(fsm.has_pending_response());

  fsm.on_reopen_socket();
  ASSERT_FALSE(fsm.has_disconnected());
  ASSERT_FALSE(fsm.has_pending_response());
  ASSERT_TRUE(fsm.is_running());
}

TEST(RadioZmqRxChannelFsmTest, a_received_response_clears_the_disconnection)
{
  radio_zmq_rx_channel_fsm fsm;
  fsm.init_successful();
  fsm.request_sent();

  // The peer replied before leaving, so the response is not lost.
  fsm.on_disconnected();
  fsm.data_received();
  ASSERT_FALSE(fsm.has_disconnected());
  ASSERT_FALSE(fsm.has_pending_response());
}

TEST(RadioZmqRxChannelFsmTest, stops_while_the_peer_is_lost)
{
  radio_zmq_rx_channel_fsm fsm;
  fsm.init_successful();
  fsm.request_sent();
  fsm.on_disconnected();

  fsm.stop();
  ASSERT_FALSE(fsm.is_running());
  ASSERT_FALSE(fsm.has_disconnected());
}
