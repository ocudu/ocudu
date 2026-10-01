// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/format.h"
#include "ocudu/fapi/p7/builders/ul_tti_request_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(ul_tti_request_builder, valid_basic_parameters_passes)
{
  auto     scs        = subcarrier_spacing::kHz30;
  unsigned sfn        = 599;
  unsigned slot_index = 13;
  auto     slot       = slot_point(scs, sfn, slot_index);

  ul_tti_request         msg;
  ul_tti_request_builder builder(msg);

  builder.set_slot(slot);

  ASSERT_EQ(slot, msg.slot);
  ASSERT_TRUE(msg.pdus.empty());
}

TEST(ul_tti_request_builder, add_pucch_f0_pdu_passes)
{
  ul_tti_request         msg;
  ul_tti_request_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());

  rnti_t       rnti   = to_rnti(3);
  pucch_format format = pucch_format::FORMAT_0;

  builder.add_pucch_pdu(rnti, format);

  ASSERT_EQ(1U, msg.pdus.size());
  const auto& pdu       = msg.pdus.back();
  const auto* pucch_pdu = std::get_if<ul_pucch_pdu>(&pdu.pdu);
  ASSERT_TRUE(pucch_pdu);
  ASSERT_EQ(rnti, pucch_pdu->rnti);
  ASSERT_EQ(0, pucch_pdu->handle);
  ASSERT_TRUE(std::holds_alternative<ul_pucch_pdu_format_0>(pucch_pdu->format));
}

TEST(ul_tti_request_builder, add_pucch_f1_pdu_passes)
{
  ul_tti_request         msg;
  ul_tti_request_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());

  rnti_t       rnti   = to_rnti(3);
  pucch_format format = pucch_format::FORMAT_1;

  builder.add_pucch_pdu(rnti, format);

  ASSERT_EQ(1U, msg.pdus.size());
  const auto& pdu       = msg.pdus.back();
  const auto* pucch_pdu = std::get_if<ul_pucch_pdu>(&pdu.pdu);
  ASSERT_TRUE(pucch_pdu);
  ASSERT_EQ(rnti, pucch_pdu->rnti);
  ASSERT_EQ(0, pucch_pdu->handle);
  ASSERT_TRUE(std::holds_alternative<ul_pucch_pdu_format_1>(pucch_pdu->format));
}

TEST(ul_tti_request_builder, add_pucch_f2_pdu_passes)
{
  ul_tti_request         msg;
  ul_tti_request_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());

  rnti_t       rnti   = to_rnti(3);
  pucch_format format = pucch_format::FORMAT_2;

  builder.add_pucch_pdu(rnti, format);

  ASSERT_EQ(1U, msg.pdus.size());
  const auto& pdu       = msg.pdus.back();
  const auto* pucch_pdu = std::get_if<ul_pucch_pdu>(&pdu.pdu);
  ASSERT_TRUE(pucch_pdu);
  ASSERT_EQ(rnti, pucch_pdu->rnti);
  ASSERT_EQ(0, pucch_pdu->handle);
  ASSERT_TRUE(std::holds_alternative<ul_pucch_pdu_format_2>(pucch_pdu->format));
}

TEST(ul_tti_request_builder, add_pucch_f3_pdu_passes)
{
  ul_tti_request         msg;
  ul_tti_request_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());

  rnti_t       rnti   = to_rnti(3);
  pucch_format format = pucch_format::FORMAT_3;

  builder.add_pucch_pdu(rnti, format);

  ASSERT_EQ(1U, msg.pdus.size());
  const auto& pdu       = msg.pdus.back();
  const auto* pucch_pdu = std::get_if<ul_pucch_pdu>(&pdu.pdu);
  ASSERT_TRUE(pucch_pdu);
  ASSERT_EQ(rnti, pucch_pdu->rnti);
  ASSERT_EQ(0, pucch_pdu->handle);
  ASSERT_TRUE(std::holds_alternative<ul_pucch_pdu_format_3>(pucch_pdu->format));
}

TEST(ul_tti_request_builder, add_pucch_f4_pdu_passes)
{
  ul_tti_request         msg;
  ul_tti_request_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());

  rnti_t       rnti   = to_rnti(3);
  pucch_format format = pucch_format::FORMAT_4;

  builder.add_pucch_pdu(rnti, format);

  ASSERT_EQ(1U, msg.pdus.size());
  const auto& pdu       = msg.pdus.back();
  const auto* pucch_pdu = std::get_if<ul_pucch_pdu>(&pdu.pdu);
  ASSERT_TRUE(pucch_pdu);
  ASSERT_EQ(rnti, pucch_pdu->rnti);
  ASSERT_EQ(0, pucch_pdu->handle);
  ASSERT_TRUE(std::holds_alternative<ul_pucch_pdu_format_4>(pucch_pdu->format));
}

TEST(ul_tti_request_builder, add_pusch_pdu_passes)
{
  ul_tti_request         msg;
  ul_tti_request_builder builder(msg);

  ASSERT_TRUE(msg.pdus.empty());

  rnti_t rnti = to_rnti(3);

  builder.add_pusch_pdu(rnti);

  ASSERT_EQ(1U, msg.pdus.size());
  const auto& pdu       = msg.pdus.back();
  const auto* pusch_pdu = std::get_if<ul_pusch_pdu>(&pdu.pdu);
  ASSERT_TRUE(pusch_pdu);
  ASSERT_EQ(rnti, pusch_pdu->rnti);
  ASSERT_EQ(0, pusch_pdu->handle);
}
