// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/e2/e2sm/e2sm_kpm/e2sm_kpm_cu_meas_provider_impl.h"
#include "lib/e2/e2sm/e2sm_kpm/e2sm_kpm_du_meas_provider_impl.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/ran/du_types.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <map>

using namespace ocudu;
using namespace asn1::e2sm;

static span<const e2sm_kpm_metric_t> e2sm_kpm_28_552_metrics = get_e2sm_kpm_28_552_metrics();
static span<const e2sm_kpm_metric_t> e2sm_kpm_oran_metrics   = get_e2sm_kpm_oran_metrics();

bool get_metric_definition(std::string metric_name, e2sm_kpm_metric_t& e2sm_kpm_metric_def)
{
  auto name_matches = [&metric_name](const e2sm_kpm_metric_t& x) {
    return (x.name == metric_name.c_str() or x.name == metric_name);
  };

  const auto* it = std::find_if(e2sm_kpm_28_552_metrics.begin(), e2sm_kpm_28_552_metrics.end(), name_matches);
  if (it != e2sm_kpm_28_552_metrics.end()) {
    e2sm_kpm_metric_def = *it;
    return true;
  }

  it = std::find_if(e2sm_kpm_oran_metrics.begin(), e2sm_kpm_oran_metrics.end(), name_matches);
  if (it != e2sm_kpm_oran_metrics.end()) {
    e2sm_kpm_metric_def = *it;
    return true;
  }

  return false;
}

static rlc_metrics generate_non_zero_rlc_metrics(uint32_t ue_idx, uint32_t bearer_id)
{
  rlc_metrics rlc_metric;
  rlc_metric.metrics_period        = std::chrono::milliseconds(1000);
  rlc_metric.ue_index              = static_cast<du_ue_index_t>(ue_idx);
  rlc_metric.rb_id                 = rb_id_t(drb_id_t(bearer_id));
  rlc_metric.rx.mode_specific      = rlc_am_rx_metrics{};
  rlc_metric.rx.num_pdus           = 5;
  rlc_metric.rx.num_pdu_bytes      = rlc_metric.rx.num_pdus * 1000;
  rlc_metric.rx.num_sdus           = 5;
  rlc_metric.rx.num_sdu_bytes      = rlc_metric.rx.num_sdus * 1000;
  rlc_metric.rx.num_lost_pdus      = 1;
  rlc_metric.rx.num_malformed_pdus = 1;
  rlc_metric.rx.sdu_latency_us     = 1000;

  rlc_metric.tx.tx_high.num_sdus                     = 10;
  rlc_metric.tx.tx_high.num_sdu_bytes                = rlc_metric.tx.tx_high.num_sdus * 1000;
  rlc_metric.tx.tx_high.num_dropped_sdus             = 1;
  rlc_metric.tx.tx_high.num_discarded_sdus           = 1;
  rlc_metric.tx.tx_high.num_discard_failures         = 1;
  rlc_metric.tx.tx_low.sum_sdu_latency_us            = 1000;
  rlc_metric.tx.tx_low.num_of_pulled_sdus            = 1;
  rlc_metric.tx.tx_low.num_pdus_no_segmentation      = 10;
  rlc_metric.tx.tx_low.num_pdu_bytes_no_segmentation = rlc_metric.tx.tx_low.num_pdus_no_segmentation * 1000;

  rlc_metric.tx.tx_low.mode_specific = rlc_am_tx_metrics_lower{};
  auto& am                           = std::get<rlc_am_tx_metrics_lower>(rlc_metric.tx.tx_low.mode_specific);
  am.num_pdus_with_segmentation      = 2;
  am.num_pdu_bytes_with_segmentation = am.num_pdus_with_segmentation * 1000;

  return rlc_metric;
}

static scheduler_cell_metrics generate_non_zero_sched_metrics()
{
  scheduler_cell_metrics sched_metric;
  sched_metric.nof_prbs              = 52;
  sched_metric.nof_dl_slots          = 14;
  sched_metric.nof_ul_slots          = 14;
  sched_metric.total_prach_preambles = 10;

  scheduler_ue_metrics ue_metrics;
  ue_metrics.ue_index = to_du_ue_index(0);
  ue_metrics.pci      = 1;
  ue_metrics.rnti     = static_cast<rnti_t>(0x1000 + 1);
  // Mean per-slot usage (total / nof_slots) must stay below nof_prbs, or RRU.PrbAvail* is legitimately 0.
  ue_metrics.tot_pdsch_prbs_used = 20 * sched_metric.nof_dl_slots;
  ue_metrics.tot_pusch_prbs_used = 20 * sched_metric.nof_ul_slots;
  ue_metrics.avg_crc_delay_ms    = 100;
  ue_metrics.pusch_snr_db        = 10;
  for (auto i = 0; i < 10; i++) {
    ue_metrics.cqi_stats.update(i);
  }
  sched_metric.ue_metrics.push_back(ue_metrics);

  return sched_metric;
}

class dummy_e2_du_metrics_notifier : public e2_du_metrics_notifier, public e2_du_metrics_interface
{
public:
  void report_metrics(const scheduler_cell_metrics& metrics) override
  {
    if (e2_meas_provider) {
      e2_meas_provider->report_metrics(metrics);
    }
  }

  void report_metrics(const rlc_metrics& metrics) override
  {
    if (e2_meas_provider) {
      e2_meas_provider->report_metrics(metrics);
    }
  }

  void connect_e2_du_meas_provider(std::unique_ptr<e2_du_metrics_notifier> meas_provider) override {}

  void connect_e2_du_meas_provider(e2_du_metrics_notifier* meas_provider) { e2_meas_provider = meas_provider; }

private:
  e2_du_metrics_notifier* e2_meas_provider = nullptr;
};

class dummy_e2_cu_metrics_notifier : public e2_cu_metrics_notifier, public e2_cu_metrics_interface
{
public:
  void report_metrics(const pdcp_metrics_container& metrics) override
  {
    if (e2_meas_provider) {
      e2_meas_provider->report_metrics(metrics);
    }
  }

  void report_metrics(const ocuup::f1u_metrics_container& metrics) override
  {
    if (e2_meas_provider) {
      e2_meas_provider->report_metrics(metrics);
    }
  }

  void report_metrics(const cu_cp_metrics_report& metrics) override
  {
    if (e2_meas_provider) {
      e2_meas_provider->report_metrics(metrics);
    }
  }

  void connect_e2_cu_meas_provider(std::unique_ptr<e2_cu_metrics_notifier> meas_provider) override {}

  void connect_e2_cu_meas_provider(e2_cu_metrics_notifier* meas_provider) { e2_meas_provider = meas_provider; }

private:
  e2_cu_metrics_notifier* e2_meas_provider = nullptr;
};

class e2sm_kpm_meas_provider_metrics_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();
    f1ap_ue_id_mapper = std::make_unique<dummy_f1ap_ue_id_translator>();
    du_meas_provider  = std::make_unique<e2sm_kpm_du_meas_provider_impl>(*f1ap_ue_id_mapper);
    metrics           = std::make_unique<dummy_e2_du_metrics_notifier>();
    metrics->connect_e2_du_meas_provider(du_meas_provider.get());
  }

  void TearDown() override
  {
    // Flush logger after each test.
    ocudulog::flush();
  }

  std::unique_ptr<dummy_e2_du_metrics_notifier>   metrics;
  std::unique_ptr<dummy_f1ap_ue_id_translator>    f1ap_ue_id_mapper;
  std::unique_ptr<e2sm_kpm_du_meas_provider_impl> du_meas_provider;
  ocudulog::basic_logger&                         test_logger = ocudulog::fetch_basic_logger("TEST");
};

TEST_F(e2sm_kpm_meas_provider_metrics_test, e2sm_kpm_supported_metrics_are_supported)
{
  e2sm_kpm_metric_level_enum metric_level;
  meas_type_c                meas_type;
  meas_label_s               meas_label;
  meas_label.no_label_present         = true;
  meas_label.no_label                 = meas_label_s::no_label_e_::true_value;
  bool                     cell_scope = false;
  std::vector<std::string> supported_metrics;
  bool                     metric_supported = false;

  // E2-NODE-LEVEL metrics
  metric_level      = E2_NODE_LEVEL;
  supported_metrics = du_meas_provider->get_supported_metric_names(metric_level);
  for (auto& metric : supported_metrics) {
    meas_type.set_meas_name().from_string(metric);
    metric_supported = du_meas_provider->is_metric_supported(meas_type, meas_label, metric_level, cell_scope);
    ASSERT_TRUE(metric_supported) << "Metric: " << metric << " Level: " << e2sm_kpm_scope_2_str(metric_level)
                                  << " returned as supported but not supported ";
  }

  // UE-LEVEL metrics
  metric_level      = UE_LEVEL;
  supported_metrics = du_meas_provider->get_supported_metric_names(metric_level);
  for (auto& metric : supported_metrics) {
    meas_type.set_meas_name().from_string(metric);
    metric_supported = du_meas_provider->is_metric_supported(meas_type, meas_label, metric_level, cell_scope);
    ASSERT_TRUE(metric_supported) << "Metric: " << metric << " Level: " << e2sm_kpm_scope_2_str(metric_level)
                                  << " returned as supported but not supported ";
  }
}

TEST_F(e2sm_kpm_meas_provider_metrics_test, e2sm_kpm_return_e2_level_metric_with_no_measurements)
{
  // metrics that return no_value when no measurements are present. Specifically, they should not return 0
  std::vector<std::string> no_value_metrics = {
      "DRB.AirIfDelayUl", "DRB.RlcSduDelayDl", "DRB.RlcDelayUl", "DRB.RlcPacketDropRateDl", "DRB.RlcSduDelayDl"};

  // E2-NODE-LEVEL metrics have to be always returned, even if 0 or NAN
  e2sm_kpm_metric_level_enum       metric_level = E2_NODE_LEVEL;
  meas_type_c                      meas_type;
  std::optional<asn1::e2sm::cgi_c> cell_global_id = {};
  e2sm_kpm_metric_t                e2sm_kpm_metric_definition;

  label_info_list_l label_info_list;
  label_info_item_s label_info_item           = {};
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_e_::true_value;
  label_info_list.push_back(label_info_item);
  std::vector<meas_record_item_c> meas_records_items;

  std::vector<std::string> supported_metrics = du_meas_provider->get_supported_metric_names(metric_level);
  for (auto& metric : supported_metrics) {
    meas_type.set_meas_name().from_string(metric);

    meas_records_items.clear();
    du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records_items);

    ASSERT_TRUE(meas_records_items.size() == 1)
        << "Metric: " << metric << " Level: " << e2sm_kpm_scope_2_str(metric_level)
        << " returned no measurements (size=" << meas_records_items.size() << ")";

    // Check if metric should return no_value when no measurements are present
    bool found = std::any_of(
        no_value_metrics.begin(), no_value_metrics.end(), [&metric](const std::string& s) { return s == metric; });
    if (found) {
      ASSERT_EQ(meas_records_items[0].type(), meas_record_item_c::types::no_value)
          << "Metric: " << metric << " Level: " << e2sm_kpm_scope_2_str(metric_level)
          << " expected to return no_value when no measurements available.";
      continue;
    }
    if (get_metric_definition(metric, e2sm_kpm_metric_definition)) {
      if (e2sm_kpm_metric_definition.data_type == e2sm_kpm_metric_dtype_t::INTEGER) {
        ASSERT_EQ(meas_records_items[0].type().value, meas_record_item_c::types::integer)
            << "Metric: " << e2sm_kpm_metric_definition.name.c_str() << " should return record of the integer type.";
      } else {
        // e2sm_kpm_metric_dtype_t::REAL
        ASSERT_EQ(meas_records_items[0].type().value, meas_record_item_c::types::real)
            << "Metric: " << e2sm_kpm_metric_definition.name.c_str() << " should return record of the real type.";
      }
    }

    switch (meas_records_items[0].type()) {
      case meas_record_item_c::types::integer:
        ASSERT_EQ(meas_records_items[0].integer(), 0)
            << "Metric: " << metric << " Level: " << e2sm_kpm_scope_2_str(metric_level);
        break;
      case meas_record_item_c::types::real:
        ASSERT_FLOAT_EQ(meas_records_items[0].real().value, 0.0)
            << "Metric: " << metric << " Level: " << e2sm_kpm_scope_2_str(metric_level);
        break;
      default:
        printf("%s type: %i\n", metric.c_str(), meas_records_items[0].type().value);
        ASSERT_TRUE(false) << "Metric: " << metric << " Level: " << e2sm_kpm_scope_2_str(metric_level)
                           << " returned a record with wrong type.";
        break;
    }
  }
}

TEST_F(e2sm_kpm_meas_provider_metrics_test, e2sm_kpm_return_e2_level_metric_with_with_measurements)
{
  // E2-NODE-LEVEL metrics have to be always returned, even if 0 or NAN
  e2sm_kpm_metric_level_enum       metric_level = E2_NODE_LEVEL;
  meas_type_c                      meas_type;
  std::optional<asn1::e2sm::cgi_c> cell_global_id = {};
  e2sm_kpm_metric_t                e2sm_kpm_metric_definition;

  label_info_list_l label_info_list;
  label_info_item_s label_info_item           = {};
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_e_::true_value;
  label_info_list.push_back(label_info_item);
  std::vector<meas_record_item_c> meas_records_items;

  // Fill e2sm-kpm measurements provider with RLC and SCHED metrics.
  // Generate dummy metrics that will generate non-zero e2sm-kpm metric records.
  rlc_metrics rlc_metric = generate_non_zero_rlc_metrics(0, 1);
  metrics->report_metrics(rlc_metric);
  scheduler_cell_metrics sched_metrics = generate_non_zero_sched_metrics();
  metrics->report_metrics(sched_metrics);

  std::vector<std::string> supported_metrics = du_meas_provider->get_supported_metric_names(metric_level);
  for (auto& metric : supported_metrics) {
    meas_type.set_meas_name().from_string(metric);

    meas_records_items.clear();
    du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records_items);

    ASSERT_TRUE(meas_records_items.size() == 1)
        << "Metric: " << metric << " Level: " << e2sm_kpm_scope_2_str(metric_level)
        << " returned no measurements (size=" << meas_records_items.size() << ")";

    if (get_metric_definition(metric, e2sm_kpm_metric_definition)) {
      if (e2sm_kpm_metric_definition.data_type == e2sm_kpm_metric_dtype_t::INTEGER) {
        ASSERT_EQ(meas_records_items[0].type().value, meas_record_item_c::types::integer)
            << "Metric: " << e2sm_kpm_metric_definition.name.c_str() << " should return record of the integer type.";
      } else {
        // e2sm_kpm_metric_dtype_t::REAL
        ASSERT_EQ(meas_records_items[0].type().value, meas_record_item_c::types::real)
            << "Metric: " << e2sm_kpm_metric_definition.name.c_str() << " should return record of the real type.";
      }
    }

    switch (meas_records_items[0].type()) {
      case meas_record_item_c::types::integer:
        ASSERT_NE(meas_records_items[0].integer(), 0)
            << "Metric: " << metric << " Level: " << e2sm_kpm_scope_2_str(metric_level);
        break;
      case meas_record_item_c::types::real:
        ASSERT_NE(meas_records_items[0].real().value, 0.0)
            << "Metric: " << metric << " Level: " << e2sm_kpm_scope_2_str(metric_level);
        break;
      default:
        printf("%s type: %i\n", metric.c_str(), meas_records_items[0].type().value);
        ASSERT_TRUE(false) << "Metric: " << metric << " Level: " << e2sm_kpm_scope_2_str(metric_level)
                           << " returned a record with wrong type.";
        break;
    }
  }
}

TEST_F(e2sm_kpm_meas_provider_metrics_test, e2sm_kpm_prb_metrics_with_zero_slots_do_not_crash)
{
  // Regression test: nof_dl_slots=0 / nof_ul_slots=0 with UEs present triggered SIGFPE.
  scheduler_cell_metrics sched_metrics;
  sched_metrics.nof_prbs     = 52;
  sched_metrics.nof_dl_slots = 0;
  sched_metrics.nof_ul_slots = 0;
  scheduler_ue_metrics ue_metrics;
  ue_metrics.ue_index            = to_du_ue_index(0);
  ue_metrics.tot_pdsch_prbs_used = 100;
  ue_metrics.tot_pusch_prbs_used = 100;
  sched_metrics.ue_metrics.push_back(ue_metrics);
  metrics->report_metrics(sched_metrics);

  label_info_list_l label_info_list;
  label_info_item_s label_info_item           = {};
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_e_::true_value;
  label_info_list.push_back(label_info_item);

  const std::optional<asn1::e2sm::cgi_c> cell_global_id = {};
  meas_type_c                            meas_type;
  std::vector<meas_record_item_c>        meas_records;

  // With zero slots mean PRBs used = 0, so PrbAvail = nof_prbs and PrbUsed/PrbTot = 0.
  meas_type.set_meas_name().from_string("RRU.PrbAvailDl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].integer(), 52u);
  meas_records.clear();

  meas_type.set_meas_name().from_string("RRU.PrbAvailUl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].integer(), 52u);
  meas_records.clear();

  meas_type.set_meas_name().from_string("RRU.PrbUsedDl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].integer(), 0u);
  meas_records.clear();

  meas_type.set_meas_name().from_string("RRU.PrbUsedUl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].integer(), 0u);
  meas_records.clear();

  meas_type.set_meas_name().from_string("RRU.PrbTotDl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].integer(), 0u);
  meas_records.clear();

  meas_type.set_meas_name().from_string("RRU.PrbTotUl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].integer(), 0u);
}

TEST_F(e2sm_kpm_meas_provider_metrics_test, e2sm_kpm_prb_perc_metrics_with_zero_cell_prbs_return_zero)
{
  // Regression test: nof_prbs=0 triggered SIGFPE in PrbTot* percentage calculation.
  scheduler_cell_metrics sched_metrics;
  sched_metrics.nof_prbs     = 0;
  sched_metrics.nof_dl_slots = 14;
  sched_metrics.nof_ul_slots = 14;
  scheduler_ue_metrics ue_metrics;
  ue_metrics.ue_index            = to_du_ue_index(0);
  ue_metrics.tot_pdsch_prbs_used = 100;
  ue_metrics.tot_pusch_prbs_used = 100;
  sched_metrics.ue_metrics.push_back(ue_metrics);
  metrics->report_metrics(sched_metrics);

  label_info_list_l label_info_list;
  label_info_item_s label_info_item           = {};
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_e_::true_value;
  label_info_list.push_back(label_info_item);

  const std::optional<asn1::e2sm::cgi_c> cell_global_id = {};
  meas_type_c                            meas_type;
  std::vector<meas_record_item_c>        meas_records;

  meas_type.set_meas_name().from_string("RRU.PrbTotDl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].integer(), 0u);
  meas_records.clear();

  meas_type.set_meas_name().from_string("RRU.PrbTotUl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].integer(), 0u);
}

TEST_F(e2sm_kpm_meas_provider_metrics_test, e2sm_kpm_prb_avail_metrics_without_ues_return_cell_prbs)
{
  // Regression test: PrbAvailDl/Ul reported 0 while no UE was attached, instead of the full cell PRB count.
  scheduler_cell_metrics sched_metrics;
  sched_metrics.nof_prbs     = 52;
  sched_metrics.nof_dl_slots = 14;
  sched_metrics.nof_ul_slots = 14;
  metrics->report_metrics(sched_metrics);

  label_info_list_l label_info_list;
  label_info_item_s label_info_item           = {};
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_e_::true_value;
  label_info_list.push_back(label_info_item);

  const std::optional<asn1::e2sm::cgi_c> cell_global_id = {};
  meas_type_c                            meas_type;
  std::vector<meas_record_item_c>        meas_records;

  // The whole cell bandwidth is available when no UE consumes PRBs.
  meas_type.set_meas_name().from_string("RRU.PrbAvailDl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].integer(), 52u);
  meas_records.clear();

  meas_type.set_meas_name().from_string("RRU.PrbAvailUl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].integer(), 52u);
  meas_records.clear();

  // Nothing is used, so the used/total counters stay at zero.
  meas_type.set_meas_name().from_string("RRU.PrbUsedDl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].integer(), 0u);
  meas_records.clear();

  meas_type.set_meas_name().from_string("RRU.PrbUsedUl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].integer(), 0u);
}

TEST_F(e2sm_kpm_meas_provider_metrics_test, e2sm_kpm_prb_avail_metrics_do_not_underflow)
{
  // Regression test: a mean PRB usage above the cell PRB count wrapped around in unsigned arithmetic.
  scheduler_cell_metrics sched_metrics;
  sched_metrics.nof_prbs     = 52;
  sched_metrics.nof_dl_slots = 1;
  sched_metrics.nof_ul_slots = 1;
  scheduler_ue_metrics ue_metrics;
  ue_metrics.ue_index            = to_du_ue_index(0);
  ue_metrics.tot_pdsch_prbs_used = 100;
  ue_metrics.tot_pusch_prbs_used = 100;
  sched_metrics.ue_metrics.push_back(ue_metrics);
  metrics->report_metrics(sched_metrics);

  label_info_list_l label_info_list;
  label_info_item_s label_info_item           = {};
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_e_::true_value;
  label_info_list.push_back(label_info_item);

  const std::optional<asn1::e2sm::cgi_c> cell_global_id = {};
  meas_type_c                            meas_type;
  std::vector<meas_record_item_c>        meas_records;

  meas_type.set_meas_name().from_string("RRU.PrbAvailDl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].integer(), 0);
  meas_records.clear();

  meas_type.set_meas_name().from_string("RRU.PrbAvailUl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].integer(), 0);
}

TEST_F(e2sm_kpm_meas_provider_metrics_test, e2sm_kpm_drb_latency_with_zero_sdus_returns_no_value)
{
  // Regression test: non-zero latency sum with zero SDU count produced NaN/Inf (bad guard).
  rlc_metrics rlc_metric                  = generate_non_zero_rlc_metrics(0, 1);
  rlc_metric.tx.tx_high.num_sdus          = 0;
  rlc_metric.tx.tx_low.sum_sdu_latency_us = 1000;
  rlc_metric.rx.num_sdus                  = 0;
  rlc_metric.rx.sdu_latency_us            = 1000;
  metrics->report_metrics(rlc_metric);

  label_info_list_l label_info_list;
  label_info_item_s label_info_item           = {};
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_e_::true_value;
  label_info_list.push_back(label_info_item);

  ue_id_c        ue_id;
  ue_id_gnb_du_s ueid_gnb_du{};
  ueid_gnb_du.gnb_cu_ue_f1ap_id = 0;
  ueid_gnb_du.ran_ue_id_present = false;
  ue_id.set_gnb_du_ue_id()      = ueid_gnb_du;
  std::vector<ue_id_c> ues      = {ue_id};

  const std::optional<asn1::e2sm::cgi_c> cell_global_id = {};
  meas_type_c                            meas_type;
  std::vector<meas_record_item_c>        meas_records;

  meas_type.set_meas_name().from_string("DRB.RlcSduDelayDl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, ues, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].type(), meas_record_item_c::types::no_value);
  meas_records.clear();

  meas_type.set_meas_name().from_string("DRB.RlcDelayUl");
  du_meas_provider->get_meas_data(meas_type, label_info_list, ues, cell_global_id, meas_records);
  ASSERT_EQ(meas_records[0].type(), meas_record_item_c::types::no_value);
}

TEST_F(e2sm_kpm_meas_provider_metrics_test, e2sm_kpm_drb_throughput_preserves_fractional_values)
{
  rlc_metrics rlc_metric                             = generate_non_zero_rlc_metrics(0, 1);
  rlc_metric.metrics_period                          = std::chrono::seconds(2);
  rlc_metric.tx.tx_low.num_pdu_bytes_no_segmentation = 100;
  std::get<rlc_am_tx_metrics_lower>(rlc_metric.tx.tx_low.mode_specific).num_pdu_bytes_with_segmentation = 0;
  rlc_metric.rx.num_pdu_bytes                                                                           = 100;
  metrics->report_metrics(rlc_metric);

  label_info_list_l label_info_list;
  label_info_item_s label_info_item           = {};
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_e_::true_value;
  label_info_list.push_back(label_info_item);

  ue_id_c        ue_id;
  ue_id_gnb_du_s ueid_gnb_du{};
  ueid_gnb_du.gnb_cu_ue_f1ap_id = 0;
  ueid_gnb_du.ran_ue_id_present = false;
  ue_id.set_gnb_du_ue_id()      = ueid_gnb_du;

  const std::optional<asn1::e2sm::cgi_c> cell_global_id = {};
  meas_type_c                            meas_type;
  std::vector<meas_record_item_c>        meas_records;

  for (const char* metric_name : {"DRB.UEThpDl", "DRB.UEThpUl"}) {
    meas_type.set_meas_name().from_string(metric_name);
    for (const std::vector<ue_id_c>& ues : {std::vector<ue_id_c>{}, std::vector<ue_id_c>{ue_id}}) {
      meas_records.clear();
      ASSERT_TRUE(du_meas_provider->get_meas_data(meas_type, label_info_list, ues, cell_global_id, meas_records));
      ASSERT_EQ(meas_records.size(), 1);
      ASSERT_EQ(meas_records[0].type(), meas_record_item_c::types::real);
      EXPECT_FLOAT_EQ(meas_records[0].real().value, 0.4F);
    }
  }
}

TEST_F(e2sm_kpm_meas_provider_metrics_test, e2sm_kpm_drb_latency_does_not_overflow_signed_integer)
{
  rlc_metrics rlc_metric                  = generate_non_zero_rlc_metrics(0, 1);
  rlc_metric.tx.tx_low.sum_sdu_latency_us = 3000000000U;
  rlc_metric.tx.tx_low.num_of_pulled_sdus = 3000000U;
  rlc_metric.tx.tx_high.num_sdus          = 3000000U;
  rlc_metric.rx.sdu_latency_us            = 3000000000U;
  rlc_metric.rx.num_sdus                  = 3000000U;
  metrics->report_metrics(rlc_metric);

  label_info_list_l label_info_list;
  label_info_item_s label_info_item           = {};
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_e_::true_value;
  label_info_list.push_back(label_info_item);

  ue_id_c        ue_id;
  ue_id_gnb_du_s ueid_gnb_du{};
  ueid_gnb_du.gnb_cu_ue_f1ap_id = 0;
  ueid_gnb_du.ran_ue_id_present = false;
  ue_id.set_gnb_du_ue_id()      = ueid_gnb_du;

  const std::optional<asn1::e2sm::cgi_c> cell_global_id = {};
  meas_type_c                            meas_type;
  std::vector<meas_record_item_c>        meas_records;

  for (const char* metric_name : {"DRB.RlcSduDelayDl", "DRB.RlcDelayUl"}) {
    meas_type.set_meas_name().from_string(metric_name);
    for (const std::vector<ue_id_c>& ues : {std::vector<ue_id_c>{}, std::vector<ue_id_c>{ue_id}}) {
      meas_records.clear();
      ASSERT_TRUE(du_meas_provider->get_meas_data(meas_type, label_info_list, ues, cell_global_id, meas_records));
      ASSERT_EQ(meas_records.size(), 1);
      ASSERT_EQ(meas_records[0].type(), meas_record_item_c::types::real);
      EXPECT_DOUBLE_EQ(meas_records[0].real().value, 10.0);
    }
  }
}

class e2sm_kpm_cu_cp_meas_provider_metrics_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();
    cu_cp_meas_provider = std::make_unique<e2sm_kpm_cu_cp_meas_provider_impl>();
    metrics             = std::make_unique<dummy_e2_cu_metrics_notifier>();
    metrics->connect_e2_cu_meas_provider(cu_cp_meas_provider.get());
  }

  void TearDown() override
  {
    // Flush logger after each test.
    ocudulog::flush();
  }

  std::unique_ptr<dummy_e2_cu_metrics_notifier>      metrics;
  std::unique_ptr<e2sm_kpm_cu_cp_meas_provider_impl> cu_cp_meas_provider;
};

TEST_F(e2sm_kpm_cu_cp_meas_provider_metrics_test, e2sm_kpm_cu_cp_supported_metrics_are_present)
{
  std::vector<std::string> expected_metrics = {"RRC.ConnEstabAtt",
                                               "RRC.ConnEstabSucc",
                                               "RRC.ConnEstabFailCause.NetworkReject",
                                               "UECNTX.ConnEstabAtt",
                                               "UECNTX.ConnEstabSucc",
                                               "RRC.ReEstabAtt",
                                               "RRC.ReEstabSuccWithUeContext",
                                               "RRC.ConnMean",
                                               "RRC.ConnMax"};

  auto supported_metrics = cu_cp_meas_provider->get_supported_metric_names(E2_NODE_LEVEL);
  for (const auto& metric : expected_metrics) {
    auto it = std::find(supported_metrics.begin(), supported_metrics.end(), metric);
    ASSERT_TRUE(it != supported_metrics.end()) << "Missing supported metric: " << metric;
  }
}

TEST_F(e2sm_kpm_cu_cp_meas_provider_metrics_test, e2sm_kpm_cu_cp_returns_zero_without_metrics_report)
{
  label_info_list_l label_info_list;
  label_info_item_s label_info_item           = {};
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_e_::true_value;
  label_info_list.push_back(label_info_item);

  meas_type_c                      meas_type;
  std::vector<meas_record_item_c>  meas_records_items;
  std::optional<asn1::e2sm::cgi_c> cell_global_id = {};

  auto supported_metrics = cu_cp_meas_provider->get_supported_metric_names(E2_NODE_LEVEL);
  for (const auto& metric : supported_metrics) {
    meas_type.set_meas_name().from_string(metric);
    meas_records_items.clear();

    ASSERT_TRUE(cu_cp_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records_items));
    ASSERT_EQ(meas_records_items.size(), 1);
    ASSERT_EQ(meas_records_items[0].type(), meas_record_item_c::types::integer);
    ASSERT_EQ(meas_records_items[0].integer(), 0);
  }
}

TEST_F(e2sm_kpm_cu_cp_meas_provider_metrics_test, e2sm_kpm_cu_cp_returns_expected_rrc_metrics)
{
  cu_cp_metrics_report report;
  report.dus.resize(2);

  report.dus[0].rrc_metrics.mean_nof_rrc_connections = 10;
  report.dus[0].rrc_metrics.max_nof_rrc_connections  = 12;
  report.dus[0].rrc_metrics.attempted_rrc_connection_establishments.increase(establishment_cause_t::emergency);
  report.dus[0].rrc_metrics.attempted_rrc_connection_establishments.increase(establishment_cause_t::high_prio_access);
  report.dus[0].rrc_metrics.successful_rrc_connection_establishments.increase(establishment_cause_t::emergency);
  report.dus[0].rrc_metrics.attempted_rrc_connection_reestablishments                  = 4;
  report.dus[0].rrc_metrics.successful_rrc_connection_reestablishments_with_ue_context = 3;

  report.dus[1].rrc_metrics.mean_nof_rrc_connections = 7;
  report.dus[1].rrc_metrics.max_nof_rrc_connections  = 20;
  report.dus[1].rrc_metrics.attempted_rrc_connection_establishments.increase(establishment_cause_t::mt_access);
  report.dus[1].rrc_metrics.successful_rrc_connection_establishments.increase(establishment_cause_t::mt_access);
  report.dus[1].rrc_metrics.successful_rrc_connection_establishments.increase(establishment_cause_t::mo_sig);
  for (unsigned i = 0; i != 5; ++i) {
    report.dus[1].rrc_metrics.failed_rrc_connection_establishments.increase(establishment_fail_cause_t::network_reject);
  }
  report.dus[1].rrc_metrics.attempted_rrc_connection_reestablishments                  = 6;
  report.dus[1].rrc_metrics.successful_rrc_connection_reestablishments_with_ue_context = 1;

  report.ngaps.resize(2);
  report.ngaps[0].metrics.nof_ue_associated_logical_ng_connection_establishment_attempts  = 4;
  report.ngaps[0].metrics.nof_ue_associated_logical_ng_connection_establishment_successes = 3;
  report.ngaps[1].metrics.nof_ue_associated_logical_ng_connection_establishment_attempts  = 2;
  report.ngaps[1].metrics.nof_ue_associated_logical_ng_connection_establishment_successes = 1;

  metrics->report_metrics(report);

  const std::map<std::string, int64_t> expected_values = {{"RRC.ConnEstabAtt", 3},
                                                          {"RRC.ConnEstabSucc", 3},
                                                          {"RRC.ConnEstabFailCause.NetworkReject", 5},
                                                          {"UECNTX.ConnEstabAtt", 6},
                                                          {"UECNTX.ConnEstabSucc", 4},
                                                          {"RRC.ReEstabAtt", 10},
                                                          {"RRC.ReEstabSuccWithUeContext", 4},
                                                          {"RRC.ConnMean", 17},
                                                          {"RRC.ConnMax", 20}};

  label_info_list_l label_info_list;
  label_info_item_s label_info_item           = {};
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_e_::true_value;
  label_info_list.push_back(label_info_item);

  std::vector<meas_record_item_c>  meas_records_items;
  std::optional<asn1::e2sm::cgi_c> cell_global_id = {};
  meas_type_c                      meas_type;

  for (const auto& metric : expected_values) {
    meas_type.set_meas_name().from_string(metric.first);
    meas_records_items.clear();

    ASSERT_TRUE(cu_cp_meas_provider->get_meas_data(meas_type, label_info_list, {}, cell_global_id, meas_records_items));
    ASSERT_EQ(meas_records_items.size(), 1);
    ASSERT_EQ(meas_records_items[0].type(), meas_record_item_c::types::integer);
    ASSERT_EQ(meas_records_items[0].integer(), metric.second) << "Unexpected value for metric " << metric.first;
  }
}
