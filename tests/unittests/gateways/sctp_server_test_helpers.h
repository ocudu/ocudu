// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "sctp_test_helpers.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/gateways/sctp_network_server.h"
#include <arpa/inet.h>
#include <gtest/gtest.h>

namespace ocudu {

class association_factory : public sctp_network_association_factory
{
public:
  byte_buffer last_sdu;

  class dummy_sctp_recv_notifier : public sctp_association_sdu_notifier
  {
  public:
    dummy_sctp_recv_notifier(association_factory& parent_, sctp_association_sdu_notifier* send_notifier_) :
      parent(parent_), send_notifier(send_notifier_)
    {
    }

    ~dummy_sctp_recv_notifier() override
    {
      for (auto& sender : parent.association_senders) {
        if (sender.get() == send_notifier) {
          parent.association_senders.erase(parent.association_senders.begin());
          break;
        }
      }
      parent.association_destroyed = true;
    }

    bool on_new_sdu(byte_buffer sdu) override
    {
      parent.last_sdu = std::move(sdu);
      return true;
    }

  private:
    association_factory&           parent;
    sctp_association_sdu_notifier* send_notifier;
  };

  std::vector<std::unique_ptr<sctp_association_sdu_notifier>> association_senders;
  bool                                                        association_created   = false;
  bool                                                        association_destroyed = false;

  std::unique_ptr<sctp_association_sdu_notifier> create(std::unique_ptr<sctp_association_sdu_notifier> send_notifier,
                                                        sctp_association_info assoc_info) override
  {
    association_created = true;
    association_senders.push_back(std::move(send_notifier));
    return std::make_unique<dummy_sctp_recv_notifier>(*this, association_senders.back().get());
  }

  unsigned association_count() const { return association_senders.size(); }
};

} // namespace ocudu
