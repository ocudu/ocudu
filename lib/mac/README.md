# MAC

The main responsibilities of the MAC are:

- Encoding and decoding of MAC PDUs sent and received over the FAPI interface.
- Scheduling of DL/UL grants for System Information, Paging, UE data (RLC PDUs + MAC CEs) and Random Access, via the embedded gNB scheduler.
- Demultiplexing and forwarding of decoded MAC Rx SDUs to their respective logical channels.
- Handling of received MAC CEs.
- PRACH handling and RNTI allocation.

## Architecture

![MAC Architecture](mac.png)

The MAC interacts with FAPI, the RLC and the DU control plane. It is built from the sub-components
shown above and exposes its functionality through abstract C++ interfaces (`include/ocudu/mac/`); the top-level
`mac` interface (`mac.h`) hands out the per-cell and global handlers via accessors such as
`get_slot_handler()`, `get_pdu_handler()`, `get_cell_manager()` and `get_ue_configurator()`. The
three peers map onto the figure's edges: the DU control plane configures the MAC, **SDUs are exchanged
with the RLC**, and **PDUs are exchanged over FAPI**.

### Sub-components

- **MAC Controller** (`mac_controller`) — translates DU configuration requests (add cell,
  add/reconfigure/remove UE) into commands for the other sub-components, applying them with minimal
  service disruption and without races (see [Configuration Lifecycle](#configuration-lifecycle)).
- **RACH Handler** (`rach_handler`) — allocates RNTIs for received PRACH preambles and associates each
  RNTI with a DU UE index (see [RNTI Management](#rnti-management)).
- **MAC UL Processor** (`mac_ul_processor`) — decodes received MAC PDUs and demultiplexes the SDUs and
  MAC CEs (see [UL Data Path](#ul-data-path)).
- **MAC DL Processor** (`mac_dl_processor`) — drives the scheduler and assembles the DL MAC PDUs (see
  [DL Data Path](#dl-data-path) and [Scheduler](#scheduler)).

### DU control plane interface

Configuration and UE lifecycle events, driven by the DU manager:

- **Configuration** — `mac_cell_manager` (`add_cell`/`remove_cell`, plus per-cell `start`/`stop`/
  `reconfigure`) and `mac_ue_configurator` (`handle_ue_create_request`/`..._reconfiguration_request`/
  `..._delete_request`, returned as `async_task`s).
- **UL-CCCH / contention resolution** — `mac_ul_ccch_notifier` (`on_ul_ccch_msg_received`,
  `on_crnti_ce_received`) reports a received Msg3 to the upper layers, triggering UE creation in the DU.

### RLC interface

User-plane SDUs, one bearer per logical channel, bound through `mac_logical_channel_config`:

- a `mac_sdu_tx_builder` supplies DL SDUs (`on_new_tx_sdu`) for MAC PDU assembly and reports each RLC bearer's buffer
  occupancy (`on_buffer_state_update`), which the MAC forwards to the scheduler, and
- a `mac_sdu_rx_notifier` (`on_new_sdu`) which the MAC uses to forward decoded UL SDUs to the RLC.

### FAPI interface

The MAC's lower-edge handlers are translated to and from real FAPI messages by the FAPI adaptor, which
lives outside `lib/mac`, so the MAC is written purely against these abstract handlers:

- **Inputs (from FAPI)** — `mac_cell_slot_handler` (`handle_slot_indication`, `handle_error_indication`),
  `mac_cell_rach_handler` (`handle_rach_indication`), `mac_cell_control_information_handler`
  (`handle_crc`/`handle_uci`/`handle_srs`), and `mac_pdu_handler` (`handle_rx_data_indication`).
- **Outputs (to FAPI)** — `mac_cell_result_notifier` (obtained from `mac_result_notifier::get_cell`):
  `on_new_downlink_scheduler_results`, `on_new_downlink_data`, `on_new_uplink_scheduler_results`,
  and `on_cell_results_completion` as the end-of-slot sentinel.

## Concurrency and Parallelism

The MAC does not own any threads of its own. Instead, every MAC task runs on a `task_executor`
provided at construction time, and the mapping of tasks to executors is what determines the MAC's
concurrency model. This lets the same MAC code run single-threaded in tests and simulators, or
spread across a worker pool in a real-time deployment, just by swapping the executor mapper.

Executors are supplied to the MAC through `mac_config` as three handles:

- `ctrl_exec` — a single, DU-wide control executor.
- `cell_exec_mapper` (`mac_cell_executor_mapper`) — per-cell executors.
- `ue_exec_mapper` (`mac_ue_executor_mapper`) — per-UE executors.

### External events and thread-safety

The MAC's public entry points are invoked from threads it does not control: slot indications, RACH
indications, CRC/UCI/SRS indications and received PDUs all arrive on FAPI threads, while
configuration requests arrive on the DU manager's thread. These callers touch no MAC or scheduler
state directly. Each entry point either:

- **dispatches the event onto a MAC executor** — e.g. the slot indication is enqueued onto the
  cell's `slot_ind_executor` and received UL PDUs onto the UE's `mac_ul_pdu_executor` — so that the
  actual handling runs serialized on the owning strand; or
- **forwards the event to the scheduler**, which manages its own thread-safety internally. CRC, UCI,
  SRS and error indications are passed straight into the scheduler from the calling thread; the
  scheduler buffers them in its own thread-safe queues and applies them later, in the cell's
  `slot_indication()` context.

This is the central invariant of the MAC's concurrency model: any state mutation must reach the
owning strand (or the scheduler's internal queues) before it touches shared state. A new entry point
that reads or writes MAC/scheduler state inline on the caller's thread, instead of hopping onto the
appropriate executor or deferring to the scheduler, introduces a data race.

### Serialization vs. parallelism

The MAC relies on **strands** (see `support/executors/strand_executor.h`) rather than locks to keep
shared state consistent. A strand guarantees that tasks dispatched to it run one-at-a-time, in a
serialized fashion, even when the strand is backed by a multi-threaded worker pool. Parallelism is
therefore achieved *between* strands, not within one: tasks on different strands may run on
different workers concurrently, while tasks on the same strand never overlap.

This yields three independent axes of parallelism:

- **Across cells** — each cell has its own strand, so different cells schedule and assemble their
  DL/UL grants in parallel.
- **Across UEs** — UEs are distributed over a bounded set of strands, so UL PDU processing for
  different UEs can proceed concurrently.
- **Control vs. data** — control-plane reconfiguration is funnelled onto its own serialized
  executor, decoupled from the per-cell real-time path.

Conversely, work that must not race is forced onto the *same* strand and thus serialized.

### Per-cell execution

Each cell exposes two executors via the `mac_cell_executor_mapper`:

- `slot_ind_executor(cell)` — high-priority path for the periodic slot indication coming from FAPI.
- `mac_cell_executor(cell)` — default path for other, non-slot cell tasks.

The slot indication is the heartbeat of the cell: `mac_cell_processor::handle_slot_indication()`
merely enqueues onto `slot_ind_executor`, and the actual work — invoking the **scheduler** for that
cell, assembling the DL/UL PDUs, and forwarding the result to FAPI — runs in that executor's
context. The scheduler therefore has no executor of its own; it is driven synchronously, once per
cell per slot, on the cell strand. Because each cell uses a distinct strand, scheduling for
different cells runs in parallel, while everything within a single cell is serialized.

The cell executors can be configured two ways (`du_high_executor_config::cell_executor_config`):

- **Dedicated worker per cell** — each cell gets its own thread, fully isolating cells.
- **Strand-based worker pool** — one strand per cell, all sharing a common worker pool. Slot
  indications use a higher strand priority than other cell tasks so they are never starved by
  lower-priority work.

### Per-UE execution

UE-specific work runs on the `mac_ue_executor_mapper`:

- `ctrl_executor(ue)` — UE state changes (creation, reconfiguration, removal); infrequent.
- `mac_ul_pdu_executor(ue)` — decoding of received MAC PDUs and demultiplexing of UL SDUs.

UEs are mapped onto a bounded pool of strands (policy `per_cell` or `round_robin`, capped by
`max_nof_strands`), trading off parallelism against memory. Within a UE's strand, control tasks take
priority over UL PDU handling, which in turn takes priority over DL PDU handling. Two UEs on
different strands process their UL PDUs concurrently; two UEs sharing a strand are serialized.

### Control-plane serialization

Configuration requests from the DU manager (add cell, add/reconfigure/remove UE) are handled by the
**MAC Controller** on the single DU-wide `ctrl_exec`, which is a serialized strand. Routing all
reconfiguration through one strand is what lets the MAC Controller apply changes without locks and
without racing against the per-cell real-time path — it hands off to and from the cell executors at
well-defined points (e.g. cell activation/deactivation) rather than mutating cell state directly.

### Summary

| Task | Executor | Scope | Serialized within | Runs in parallel across |
|------|----------|-------|--------------------|--------------------------|
| Slot indication, scheduling, DL PDU assembly | `slot_ind_executor` | per cell | a cell | cells |
| Other cell tasks (lower priority) | `mac_cell_executor` | per cell | a cell | cells |
| UL PDU decode / demux | `mac_ul_pdu_executor` | per UE | a UE's strand | UEs on different strands |
| UE control (add/reconfig/remove) | `ctrl_executor` | per UE | a UE's strand | UEs on different strands |
| MAC configuration (cells, UEs) | `ctrl_exec` | DU-wide | the whole MAC | nothing (fully serialized) |

## DL Data Path

The DL path is driven by the per-cell slot indication. `mac_cell_processor::handle_slot_indication_impl()`
runs once per cell per slot and:

1. Invokes the scheduler for that cell, obtaining a `sched_result`.
2. `assemble_dl_sched_request()` builds the `mac_dl_sched_result` — SSB (`ssb_helper`) and the encoded
   PDCCH DCIs (`encode_dci`) — and forwards it to FAPI via `on_new_downlink_scheduler_results`.
3. `assemble_dl_data_request()` builds the `mac_dl_data_result`, i.e. the actual MAC PDU payloads, and
   forwards it via `on_new_downlink_data`:
   - SIB / broadcast via `sib_pdu_assembler`,
   - RAR via `rar_pdu_assembler`,
   - UE DL-SCH via `dl_sch_pdu_assembler`,
   - paging via `paging_pdu_assembler`.
4. UL grants are forwarded via `on_new_uplink_scheduler_results`, and `on_cell_results_completion`
   closes the slot.

DL-SCH assembly (`dl_sch_pdu_assembler::assemble_newtx_pdu`) allocates a HARQ buffer and, for each
scheduled logical channel, pulls SDUs from RLC (`mac_sdu_tx_builder::on_new_tx_sdu`) and encodes the
subheader and payload; MAC CEs such as the UE Contention Resolution Identity and Timing Advance
Command are multiplexed in, and the transport block is padded out. Retransmissions
(`assemble_retx_pdu`) reuse the cached HARQ buffer rather than re-fetching SDUs. Once results are
handed to FAPI, `update_logical_channel_dl_buffer_states()` reads the new RLC buffer state
(`on_buffer_state_update`) and feeds it back to the scheduler (`handle_dl_buffer_state_update`).

## UL Data Path

Received PDUs enter through `mac_pdu_handler::handle_rx_data_indication()` (`mac_ul_processor`). For
each PDU the UE is resolved through the RNTI table and the work is dispatched onto that UE's
`mac_ul_pdu_executor`, where `pdu_rx_handler::handle_rx_pdu()` runs:

- The UL-SCH PDU is unpacked into subPDUs (`mac_ul_sch_pdu::unpack`).
- SDUs are forwarded to the logical channel's RLC bearer (`mac_sdu_rx_notifier::on_new_sdu`).
- MAC CEs are parsed and forwarded: BSR via `handle_ul_bsr_indication`, PHR via
  `handle_ul_phr_indication`, both consumed by the scheduler.
- **CCCH (Msg3 / SRB0)** — when no UE exists yet for the TC-RNTI, the UL-CCCH message is delivered to
  upper layers via `mac_ul_ccch_notifier::on_ul_ccch_msg_received`; the SDU itself is pushed once the
  DU has created the UE (`push_ul_ccch_msg`).
- **C-RNTI CE** — `handle_crnti_ce()` resolves the existing UE for the carried C-RNTI, re-dispatches
  the remaining subPDUs onto that UE's executor, and notifies both the scheduler
  (`handle_crnti_ce_indication`) and upper layers (`on_crnti_ce_received`) for contention resolution.

## Scheduler

The MAC contains the gNB scheduler but treats it as a self-contained subsystem behind
`ocudu_scheduler_adapter` (implementing `mac_scheduler_adapter`). The MAC *pushes* events to the
scheduler — `rach_indication`, CRC/UCI/SRS, BSR/PHR, DL buffer state, and UE add/reconfigure/remove —
and *queries* it once per cell per slot through `slot_indication()`, whose returned `sched_result`
drives the DL data path above. As described under Concurrency, the scheduler manages its own
thread-safety for indication ingest. Its internal design is documented separately in
[`lib/scheduler/README.md`](../scheduler/README.md).

## Configuration Lifecycle

Configuration is handled by the **MAC Controller** (`mac_controller`) on `ctrl_exec`. Adding a cell
registers it with the timing source, metrics, scheduler and DL handler; removal unwinds these in
reverse; cell start/stop map onto scheduler cell activation/deactivation.

UE add, reconfigure and remove are asynchronous procedures (`ue_creation_procedure`,
`ue_reconfiguration_procedure`, `mac_ue_removal_procedure`) implemented as coroutines that hop between
the control executor and the per-cell/per-UE executors, setting up or tearing down the DL and UL
contexts. UE creation is deferred until Msg3 (see the
[RA Procedure](procedures.md#ra-procedure)); if any step fails, the earlier ones are rolled back in reverse order.
Removal deletes the scheduler context first — so no further grants are produced — then the UL/DL
contexts, and finally the controller's UE entry.

## RNTI Management

`rnti_manager` allocates a TC-RNTI for each detected contention-based PRACH preamble, using an atomic
counter over the C-RNTI range seeded from `mac_expert_config::initial_crnti` (default `0x4601`);
contention-free (CFRA) preambles instead use their pre-assigned C-RNTI. On UE creation the TC-RNTI is
promoted to the UE's C-RNTI. The `rnti_value_table` maps RNTI ↔ `du_ue_index` as an array of atomics
and is read lock-free from the UL and scheduler paths (relaxed ordering); its correctness relies on
the higher-level executor and scheduler serialization described under Concurrency rather than on
per-access locking.

## Radio Link Failure Detection

The `rlf_detector` (owned by the scheduler adapter) tracks three per-UE atomic counters: consecutive
DL KOs (HARQ-ACK NACK/DTX), UL KOs (PUSCH CRC failures) and undecoded CSI (DTX). The thresholds come
from `mac_expert_cell_config` (`max_consecutive_dl_kos`, `max_consecutive_ul_kos`,
`max_consecutive_csi_dtx`, default 100). `handle_ack`/`handle_crc`/`handle_csi` — driven from the UCI
decoder and CRC handler — reset the relevant counter on success and increment it on failure; the first
time a counter reaches its threshold, an RLF is reported to the DU manager via
`mac_ue_radio_link_notifier::on_rlf_detected`, deferred onto `ctrl_exec`. A received C-RNTI MAC CE
resets all counters and invokes `on_crnti_ce_received`, cancelling a spurious RLF once the UE
re-synchronises.

## Procedures

Step-by-step descriptions of the MAC's runtime procedures (e.g. Random Access) are documented
separately in [`procedures.md`](procedures.md).
