// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// AFL++/libFuzzer harness for the MAC uplink receive path.
///
/// The harness drives a mac_ul_processor the way the PHY does, with one Rx data indication per input:
///
///   fuzz input -> mac_ul_processor -> pdu_rx_handler -> UL-SCH PDU decode -> MAC CE decode / SDU demux
///
/// Unpacking the UL-SCH PDU does not read the MAC CE payloads; pdu_rx_handler decodes them while dispatching each
/// subPDU, and when formatting the PDU for the log. The harness therefore goes through pdu_rx_handler rather than the
/// decoders alone, so that the decoders run on exactly the paths and inputs that the DU uses.
///
/// The input carries a control byte selecting the RNTI the PDU is received for, and whether the Msg3 path is followed
/// through to the creation of the UE. See tests/fuzz/README.md for the layout, build instructions and run commands.

#include "lib/mac/mac_ul/mac_scheduler_ce_info_handler.h"
#include "lib/mac/mac_ul/mac_ul_processor.h"
#include "lib/mac/rnti_manager.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/mac/mac_pdu_handler.h"
#include "ocudu/mac/mac_sdu_handler.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/pcap/mac_pcap.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/timers.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <optional>

using namespace ocudu;

namespace {

// ---------------------------------------------------------------------------
// Input encoding
// ---------------------------------------------------------------------------

/// Decoded control byte prefixed to every input.
struct input_header {
  /// Receive the PDU for an RNTI without a UE context, as for a Msg3, instead of for the attached UE.
  bool is_msg3;
  /// Receive the PDU as a MsgA PUSCH of a 2-step RACH, i.e. with a RA preamble index.
  bool is_msga;
  /// After a Msg3 carrying a UL-CCCH SDU, create the UE and hand its deferred MAC CEs back to MAC, as the DU does.
  bool create_ue_after_msg3;
};

/// Decode the control byte. See tests/fuzz/README.md for the bit layout.
input_header decode_header(uint8_t byte)
{
  return {.is_msg3 = (byte & 0x01U) != 0, .is_msga = (byte & 0x02U) != 0, .create_ue_after_msg3 = (byte & 0x04U) != 0};
}

// ---------------------------------------------------------------------------
// Test doubles
// ---------------------------------------------------------------------------

/// Runs every MAC task on the manual task worker of the harness.
class fuzz_ue_executor_mapper : public mac_ue_executor_mapper
{
public:
  explicit fuzz_ue_executor_mapper(task_executor& exec_) : exec(exec_) {}

  void           rebind_executors(du_ue_index_t /*ue_index*/, du_cell_index_t /*pcell_index*/) override {}
  task_executor& ctrl_executor(du_ue_index_t /*ue_index*/) override { return exec; }
  task_executor& mac_ul_pdu_executor(du_ue_index_t /*ue_index*/) override { return exec; }

private:
  task_executor& exec;
};

/// Stores the last UL-CCCH indication, so that its deferred Msg3 MAC CEs can be handed back to MAC.
class fuzz_ccch_notifier : public mac_ul_ccch_notifier
{
public:
  void on_ul_ccch_msg_received(const ul_ccch_indication_message& msg) override { last_ccch_ind = msg; }
  void on_crnti_ce_received(const ul_crnti_ce_indication_message& /*msg*/) override {}

  std::optional<ul_ccch_indication_message> last_ccch_ind;
};

/// Accepts every MAC CE indication. A MsgA CCCH SDU resolves to the RNTI it was received with.
class fuzz_sched_ce_handler : public mac_scheduler_ce_info_handler
{
public:
  void handle_ul_bsr_indication(const mac_bsr_ce_info& /*bsr*/) override {}
  void handle_ul_sched_command(const mac_ul_scheduling_command& /*sched_cmd*/) override {}
  void handle_dl_mac_ce_indication(const mac_ce_scheduling_command& /*mac_ce*/) override {}
  void handle_ul_phr_indication(const mac_phr_ce_info& /*phr*/) override {}
  void handle_ul_ta_report_indication(const mac_ta_report_ce_info& /*ta_report*/) override {}
  void handle_crnti_ce_indication(du_ue_index_t /*old_ue_index*/, du_cell_index_t /*cell_index*/) override {}
  void handle_cg_confirmation_indication(const mac_cg_confirmation_ce_info& /*cg_conf*/) override {}
  std::optional<rnti_t> handle_msga_ccch_sdu(du_cell_index_t /*cell_index*/,
                                             rnti_t ra_rnti,
                                             uint8_t /*rapid*/,
                                             slot_point /*sl_rx*/,
                                             const ue_con_res_id_t& /*con_res_id*/) override
  {
    return ra_rnti;
  }
};

/// Discards the UL SDUs demultiplexed to a bearer.
class fuzz_sdu_rx_notifier : public mac_sdu_rx_notifier
{
public:
  void on_new_sdu(byte_buffer_slice /*sdu*/) override {}
};

// ---------------------------------------------------------------------------
// MAC UL under test
// ---------------------------------------------------------------------------

constexpr du_cell_index_t cell_index = to_du_cell_index(0);
/// UE attached before every input. Its C-RNTI is the one of the C-RNTI MAC CE seeds.
constexpr du_ue_index_t attached_ue_index = to_du_ue_index(0);
constexpr rnti_t        attached_crnti    = to_rnti(0x4601);
/// UE created after a Msg3, from the TC-RNTI the Msg3 was received with.
constexpr du_ue_index_t msg3_ue_index = to_du_ue_index(1);
constexpr rnti_t        msg3_tc_rnti  = to_rnti(0x4602);
/// RA preamble index of a MsgA PUSCH reception.
constexpr uint8_t msga_rapid = 0;
/// Logical channels of a UE: SRB1, SRB2 and one DRB.
constexpr std::array<lcid_t, 3> ue_lcids = {LCID_SRB1, LCID_SRB2, LCID_MIN_DRB};

/// MAC UL processor and its dependencies, rebuilt for every input so that a crash reproduces from its input file
/// alone.
class fuzz_mac_ul
{
public:
  fuzz_mac_ul() { add_ue(attached_ue_index, attached_crnti); }

  /// Receive the given MAC PDU as the PHY does, and run every task it triggers.
  void handle_rx_pdu(rnti_t rnti, std::optional<uint8_t> rapid, span<const uint8_t> pdu_bytes)
  {
    mac_rx_data_indication rx_ind{slot_rx, cell_index, {}};
    rx_ind.pdus.push_back(mac_rx_pdu{.rnti = rnti, .harq_id = 0, .pdu = {}, .rapid = rapid});
    if (!rx_ind.pdus.back().pdu.append(pdu_bytes)) {
      return;
    }
    mac_ul.handle_rx_data_indication(rx_ind);
    run_pending_tasks();
  }

  /// Create the UE of a Msg3 and hand its deferred MAC CEs back to MAC, as the DU does once the UL-CCCH SDU is handled.
  void create_ue_after_msg3()
  {
    if (!ccch_notifier.last_ccch_ind.has_value()) {
      return;
    }
    const rnti_t tc_rnti = ccch_notifier.last_ccch_ind->tc_rnti;
    if (rnti_mng.has_rnti(tc_rnti) || !add_ue(msg3_ue_index, tc_rnti)) {
      return;
    }
    (void)mac_ul.flush_ul_ccch_msg(
        msg3_ue_index, cell_index, slot_rx, byte_buffer{}, std::move(ccch_notifier.last_ccch_ind->msg3_mac_ces));
    run_pending_tasks();
  }

private:
  bool add_ue(du_ue_index_t ue_index, rnti_t crnti)
  {
    rnti_mng.add_ue(crnti, ue_index);

    mac_ue_create_request req{};
    req.cell_index = cell_index;
    req.ue_index   = ue_index;
    req.crnti      = crnti;
    for (lcid_t lcid : ue_lcids) {
      req.bearers.push_back(mac_logical_channel_config{.lcid = lcid, .ul_bearer = &sdu_notifier, .dl_bearer = nullptr});
    }

    async_task<bool>         t = mac_ul.add_ue(req);
    lazy_task_launcher<bool> launcher(t);
    run_pending_tasks();
    return t.ready() && t.get();
  }

  void run_pending_tasks()
  {
    while (task_exec.has_pending_tasks()) {
      task_exec.try_run_next();
    }
  }

  const slot_point        slot_rx{subcarrier_spacing::kHz30, 0};
  manual_task_worker      task_exec{128};
  fuzz_ue_executor_mapper ue_exec_mapper{task_exec};
  fuzz_ccch_notifier      ccch_notifier;
  fuzz_sched_ce_handler   sched;
  fuzz_sdu_rx_notifier    sdu_notifier;
  rnti_manager            rnti_mng;
  null_mac_pcap           pcap;
  timer_manager           timers;
  mac_ul_processor mac_ul{mac_ul_config{task_exec, ue_exec_mapper, ccch_notifier, sched, rnti_mng, pcap, timers}};
};

std::once_flag init_flag;

void ensure_logging()
{
  std::call_once(init_flag, []() {
    // Started here rather than in LLVMFuzzerInitialize so that the backend thread is created after AFL++ forks.
    ocudulog::init();
    // Construct the byte_buffer pools before registering the atexit handler below, so that they are destroyed after it
    // has drained the log entries holding byte_buffers.
    (void)get_default_byte_buffer_segment_pool();
    (void)get_default_fallback_byte_buffer_segment_pool();

    // Drain pending log entries while the byte_buffer pool their arguments point into is still alive.
    std::atexit([]() { ocudulog::flush(); });
  });
}

} // namespace

// ---------------------------------------------------------------------------
// LLVMFuzzer interface
// ---------------------------------------------------------------------------

/// Route logs to /dev/null at debug level before any forking occurs.
///
/// The level stays at debug so that the log formatting code runs: pdu_rx_handler formats the decoded subPDUs at info
/// level, and the subPDU formatter decodes the MAC CEs it prints, which makes it part of the surface under test.
extern "C" int LLVMFuzzerInitialize(int* /*argc*/, char*** /*argv*/)
{
  if (ocudulog::sink* null_sink = ocudulog::create_file_sink("/dev/null"); null_sink != nullptr) {
    ocudulog::set_default_sink(*null_sink);
  }
  ocudulog::fetch_basic_logger("MAC").set_level(ocudulog::basic_levels::debug);
  return 0;
}

/// Entry point called by AFL++ / libFuzzer for each mutated input.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  // The first byte selects the RNTI and the Msg3 handling; the rest is the MAC PDU.
  if (size < 2) {
    return 0;
  }
  ensure_logging();

  const input_header hdr = decode_header(data[0]);

  fuzz_mac_ul mac;
  mac.handle_rx_pdu(hdr.is_msg3 ? msg3_tc_rnti : attached_crnti,
                    hdr.is_msga ? std::optional<uint8_t>{msga_rapid} : std::nullopt,
                    span<const uint8_t>(data + 1, size - 1));
  if (hdr.is_msg3 && hdr.create_ue_after_msg3) {
    mac.create_ue_after_msg3();
  }

  return 0;
}
