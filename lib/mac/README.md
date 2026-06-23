# MAC

The main responsibilities of the MAC are:

- Encoding and decoding of MAC PDUs that are sent/received to the PHY via the FAPI interface
- Contains the gNB scheduler, which allocates DL/UL grants for System Information, Paging, UE data (RLC PDUs + MAC CEs) and Random Access handling.
- Demultiplexing and forwarding of decoded MAC Rx SDUs to their respective logical channel.
- Handle received MAC CEs
- PRACH handling and RNTI allocation

## Implementation

![MAC Architecture](mac.png)

The MAC is divided into the following sub-components:

- **MAC Controller:**
    Translates DU configuration requests into configuration commands that can be understood by the remaining MAC sub-components. The MAC controller ensures that the other
    components are configured with minimal service disruption in terms of traffic latency and avoiding any race conditions. The configuration commands that the DU manager
    sends to the MAC controller include the addition of new DU cells and addition/reconfiguration/removal of UEs. This is implemented in the `mac_controller` class.

- **RACH Handler:**
    Manages the allocation of RNTIs for the received PRACH preambles and association of reach RNTI to a DU UE Index. Which is implemented in the `rach_handler` class.

- **MAC UL Processor:**
    Decodes the received MAC PDUs and forwards the resulting MAC SDUs to their respective logical channels using the DEMUX component and forwards the UL Buffer Status
    Reports to the Scheduler. This is implemented in the `mac_ul_processor` class.

- **MAC DL processor:**
    Manages the MAC scheduler. This is implemented in the `mac_dl_processor` class.

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
indications, CRC/UCI/SRS indications and received PDUs all arrive on PHY/FAPI threads, while
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

- `slot_ind_executor(cell)` — high-priority path for the periodic slot indication coming from the
  PHY/FAPI.
- `mac_cell_executor(cell)` — default path for other, non-slot cell tasks.

The slot indication is the heartbeat of the cell: `mac_cell_processor::handle_slot_indication()`
merely enqueues onto `slot_ind_executor`, and the actual work — invoking the **scheduler** for that
cell, assembling the DL/UL PDUs, and forwarding the result to the PHY — runs in that executor's
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

## RA Procedure

The Random Access procedure is handled primarily by the DU MAC in the following steps:

1. The RACH handler manages the allocation of temporary RNTIs (TC-RNTIs) for each of the detected PRACH preambles in a given slot, and forwarding of these preambles and TC-RNTIs to the scheduler.
2. The scheduler is then responsible for allocating the RAR and Msg3 grant for each detected PRACH preamble.
3. The MAC DL processor has the role of encoding the MAC DL PDUs sent to the PHY.

This leads to the following messaging flow graph:

![RA Procedure](ra.png)

It is worth noting that the creation of a new UE in the DU is deferred until Msg3 is received. This design has the following advantages:

- UE Resources are not allocated unnecessarily for the cases when a phantom PRACH is detected, the UE fails to detect the RAR, or the Msg3 is not correctly decoded by the gNB.
- UE Resources are allocated after the RAR window has passed. With this design, any latency in the UE resource allocation thus won’t affect the ability of the scheduler to timely schedule RARs. Notice that the RAR window in NR can be relatively short, depending on the type of service provided.
