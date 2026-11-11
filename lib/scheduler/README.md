# MAC Scheduler

The MAC scheduler allocates radio resources — time, frequency, and control channels — across all active cells in a DU.
It runs once per slot per cell and produces a `sched_result` that the MAC layer uses to generate DL assignments and UL
grants.

## Component Hierarchy

```text
scheduler_impl
├── sched_config_manager          # Validates and distributes cell/UE config
├── cell_scheduler  [per cell]    # All resources specific to one cell
│   ├── cell_metrics_handler      # Per-slot metric aggregation for this cell
│   ├── cell_resource_allocator   # Circular-buffer resource grid
│   ├── cell_event_tracer         # schedtrace hook of the cell results
│   ├── ssb_scheduler             # SSB bursts
│   ├── pdcch_resource_allocator  # PDCCH/DCI allocation
│   ├── si_scheduler              # SIB1 and SI messages
│   ├── csi_rs_scheduler          # CSI-RS
│   ├── prs_scheduler             # DL-PRS
│   ├── ra_ue_repository          # UEs of the RA procedures of the cell
│   ├── ra_scheduler              # Random access response (RAR)
│   ├── prach_scheduler           # PRACH occasions
│   ├── pucch_allocator           # PUCCH resources (HARQ-ACK, SR, CSI)
│   ├── uci_allocator             # UCI scheduling (PUCCH or PUSCH mux)
│   ├── srs_allocator             # Sounding Reference Signal scheduling
│   ├── paging_scheduler          # Paging messages
│   ├── uci_scheduler             # Periodic SR and CSI scheduling
│   ├── srs_scheduler             # Periodic SRS management
│   ├── uci_indication_selector   # Matches UCI indications to their grants
│   ├── ue_cell_repository        # UE contexts of this cell (ue_cell objects)
│   ├── cell_event_manager        # Queues and dispatches every event of the cell
│   └── ue_cell_scheduler         # UE grants for this cell (via ue_scheduler)
└── ue_scheduler  [per cell group]        # Shared UE state across a CA group
    ├── ue_repository                     # UE contexts of the cell group (ue objects)
    ├── cell_group_event_manager          # Creates the event handler of each cell of the group
    └── cell_context  [per cell]          # Implements ue_cell_scheduler
        ├── cell_group_event_handler      # Applies the events of the cell to the cell group
        ├── configured_grant_scheduler    # Configured grant PUSCH opportunities
        ├── ue_fallback_scheduler         # SRB0 / contention-resolution grants
        ├── inter_slice_scheduler         # Prioritizes slices for each slot
        ├── intra_slice_scheduler         # PDSCH/PUSCH allocation within a slice
        │   ├── pdcch_cce_budget_tracker  # PDCCH CCE budget of the slot
        │   └── ue_cell_grid_allocator    # Writes grants into the resource grid (uses sched_helper)
        └── triggered_ul_grant_scheduler  # UL grants triggered by DL grants
```

A `ue_scheduler` is shared across all cells in a cell group (Carrier Aggregation), and holds the state that spans a UE's
carriers. Each `cell_scheduler` holds a `ue_cell_scheduler` handle, which is an RAII view into the shared
`ue_scheduler`. The handle is a `cell_context`, which holds the UE schedulers of that cell.

`sched_helper` (`ue_scheduling/grant_params_selector.h`) is a set of free functions, not a component. They select the
MCS, the PRBs and the HARQ of a grant.

The `uci_scheduler` and the `srs_scheduler` run after the schedulers of the common channels and before the UE scheduling
step, as they have to reserve their periodic opportunities before any UE grant of the slot.

## Slot Processing Flow

`scheduler_impl::slot_indication(sl_tx, cell_index)` is the entry point. It calls `cell_scheduler::run_slot()`, which
executes in this order:

1. **Skipped slots** — for each slot skipped since the last call, reset the resource grid and count the slot in the
   metrics.
2. **Reset resource grid** — clear stale allocations for this slot, and reset the PDCCH, PUCCH, UCI and SRS allocators.
3. **Events** — process everything that arrived since the last slot (`cell_event_manager`).
4. **SSB** — schedule Synchronization Signal Blocks.
5. **CSI-RS** — schedule CSI-RS if due.
6. **DL-PRS** — schedule the DL-PRS (`prs_scheduler`).
7. **SI** — schedule SIB1 and SI-message windows.
8. **PRACH** — schedule PRACH occasions.
9. **RA** — schedule random access (RA) grants (e.g. RAR and Msg3) for detected RACH preambles.
10. **Paging** — schedule paging PDCCH and PDSCH.
11. **Periodic UCI** — schedule the SR and CSI PUCCH opportunities (`uci_scheduler`).
12. **Periodic SRS** — schedule the periodic and aperiodic SRS (`srs_scheduler`).
13. **UE scheduling** (`ue_cell_scheduler::run_slot`) — see below.
14. **UCI indications** — match the UCI grants of the finished slot to their indications (`uci_indication_selector`).
15. **Logging and metrics** — flush the event log, the result log, the schedtrace hook and the slot metrics.

The UE scheduling runs one time per slot for the full cell group. The first cell of the group that reaches step 13 runs
it for all the cells of the group, and the other cells skip it. With more than one cell in the group, `cell_group_mutex`
protects it. The UE scheduling does these steps:

1. Advance UE state machines (DRX, timing advance, HARQ timers).
2. For each cell of the group:
   1. Schedule configured grant PUSCH opportunities, if configured.
   2. Schedule SRB0 / fallback grants (`ue_fallback_scheduler`).
   3. Prioritize slices for this slot (`inter_slice_scheduler::slot_indication`).
   4. Inject synthetic BSRs for the UEs that need a triggered UL grant in this slot (`triggered_ul_grant_scheduler`).
   5. Schedule the PDSCH of all the DL slice candidates in priority order, then the PUSCH of all the UL slice candidates
      in priority order. DL goes first, so that DCI format 0_1 gets the correct DAI value.
   6. Post-process allocations (finalize PUCCH/UCI state).
   7. Record the UEs that need a triggered UL grant, from the final DL grants of the slot.

## Event Handling

Everything that reaches the scheduler outside a slot indication — RACH, CRC, UCI and SRS indications, buffer status and
power headroom reports, MAC CEs, paging and SI requests, positioning requests, UE creation, reconfiguration and deletion
— is an event. Events arrive from other executors, so none of them are applied where they arrive:

1. **`cell_event_manager`** (`cell_event_manager.h`) queues the event in a lock-free MPMC queue, with the payloads that
   do not fit in the callback taken from a pool of their own. The queue holds every payload the pools can hand out, so
   an event type is only ever limited by its own pool. One queue per cell means the events of a cell keep their arrival
   order, whatever their type.
2. The queue is drained at the start of the cell slot indication, in the cell executor. An event that only touches the
   state of the cell is applied there.
3. An event that needs the state shared by a UE's carriers is handed to the `cell_group_event_handler` of the cell and
   applied synchronously. It reports back whether it applied the event, so that the cell logs it and accounts for it in
   its metrics only when it did.

`cell_group_event_handler` (`cell_group_event_handler.h`) groups the two interfaces that the cell calls:
`cell_group_ue_config_handler` for the UE configuration requests, and `cell_group_ue_indication_handler` for the UE
indications and for the outcomes that a cell derives for one of its UEs while processing its own events.
`cell_group_event_manager` creates one implementation of it per cell of the group, and the `cell_context` of that cell
owns it. The cell scheduler creates its `ue_cell_scheduler` handle before its `cell_event_manager`, so the handler
already exists when the manager is given the reference.

The cell also aggregates the DL buffer occupancy updates that it receives for a bearer since the last slot, and relays
only the resulting update.

## Resource Grid

`cell_resource_allocator` is a circular ring buffer over `cell_slot_resource_allocator` entries — one per slot. The ring
keeps 16 past slots (`RING_MAX_HISTORY_SIZE`) plus the maximum UL allocation delay, which includes the NTN k_offset.
Each entry contains:

- `sched_result` — the scheduling decisions (PDCCHs, PDSCHs, PUSCHs, etc.)
- A DL and UL `carrier_subslot_resource_grid` per SCS — a Symbol × CRB bitmap used for collision detection and
  availability checks.

Allocators receive a `cell_slot_resource_allocator&` and write into it directly. The ring buffer provides a lookahead
window so allocators can inspect and reserve resources in future slots (e.g., PUSCH scheduled K2 slots ahead of the
PDCCH slot).

## RAN Slicing

The scheduler supports multiple RAN slices, each configured with a `slice_rrm_policy_config` (min/max RB limits,
S-NSSAI). Two scheduling layers handle slicing:

**Inter-slice scheduler** (`slicing/inter_slice_scheduler.h`) — runs once per slot and produces a priority-ordered queue
of `ran_slice_candidate` objects for DL and UL. The priority uses these values, from the most significant to the least
significant:

1. The distance from the PDCCH slot to the PDSCH/PUSCH slot. A shorter distance has a higher priority, so that the
   grants of a UE stay in order.
2. Whether the slice has not reached its minimum RBs yet.
3. The configured priority of the slice.
4. The number of slots since the last allocation to the slice.
5. A round-robin value.

Slices can be created and removed while the cell runs, with `du_cell_slice_reconfig_request` (`policies_to_create` and
`policies_to_remove`), as a cell event like a change of RB limits. Only a slice created this way can be removed. A
created slice has no UEs until they are reconfigured, and takes the lowest free RAN slice ID. When a slice is removed,
its UEs move to the default DRB slice and keep their place in their other slices. The RAN slice IDs of the other slices
do not change, as a HARQ process keeps the RAN slice ID it was sent for. The HARQ retransmissions pending for the
removed slice are cancelled, so that a slice created later with the same ID does not get them.

**Intra-slice scheduler** (`ue_scheduling/intra_slice_scheduler.h`) — consumes one slice candidate at a time and
allocates PDSCH/PUSCH grants for UEs in that slice. Within a slice, UE ordering is determined by the slice's
`scheduler_policy`.

### Scheduling Policies

`scheduler_policy` (`policy/scheduler_policy.h`) is the pluggable per-slice algorithm interface:

- `compute_ue_dl_priorities()` / `compute_ue_ul_priorities()` — assign a `ue_sched_priority` to each UE candidate.
- `save_dl_newtx_grants()` / `save_ul_newtx_grants()` — called after allocation to update internal state.
- `add_ue()` / `rem_ue()` — track UE membership.

The implementations are in `policy/`:

- `scheduler_time_rr` — round-robin.
- `scheduler_time_qos` — QoS-aware proportional fair.

## UE Context

UE state is split into two levels:

**`ue`** (in `ue_context/`) — per-UE, cell-group-wide state:

- Logical channel repository and DL/UL buffer occupancy.
- DRX controller and timing advance manager (shared across cells).
- Link to all serving `ue_cell` objects.

**`ue_cell`** (in `ue_context/`) — per-UE, per-cell state:

- Active BWP configuration and HARQ entities.
- Channel state manager (CQI, RI, PMI from UCI indications).
- Link adaptation controller (translates CQI/SINR to MCS).
- Fallback mode (`is_in_fallback_mode()`), only for the PCell. It comes from the RRC configuration state and the
  contention resolution state of the PCell.

The `ue` objects live in the `ue_repository` of the cell group; the `ue_cell` objects live in the `ue_cell_repository`
of the cell that owns them, which the cell scheduler owns.

## Configuration Management

`sched_config_manager` (`config/sched_config_manager.h`) is the single point of entry for all configuration:

- **Cell config** — validates `sched_cell_configuration_request_message` and builds a `cell_configuration` object used
  throughout the scheduler.
- **UE config** — validates `sched_ue_creation_request_message` / `sched_ue_reconfiguration_message`, builds a
  thread-safe config snapshot, and emits a `ue_config_update_event` that the UE PCell queues like any other event.

UE configuration changes are never applied mid-slot; they are queued as events and applied at the start of the next
`run_slot()` call, in their arrival order relative to the indications of the same UE.

Two cell configuration changes are different. `sched_config_manager` writes them to `cell_configuration` directly, in
the thread of the caller:

- The slice RB limits (`update_cell`). The inter-slice scheduler also gets the change as a cell event.
- The NTN UL timing advance (`update_ntn_ul_ta`).

A long refactor changes the cell and UE configuration structures step by step. See
[config_refactor.md](config_refactor.md) for the rules that new configuration code follows.

## Logging

The scheduler writes to the `SCHED` logger via two loggers in `logging/`: a result logger (`Slot decisions` lines) and
an event logger (`Processed slot events` lines). See [log_reference.md](log_reference.md) for the full format of every
log line and field.

## Directory Layout

```text
lib/scheduler/
├── scheduler_impl.{h,cpp}      # mac_scheduler implementation
├── scheduler_factory.cpp       # create_scheduler() factory
├── cell_scheduler.{h,cpp}      # Per-cell orchestrator
├── cell/                       # Resource grid, HARQ entities, PRB tracking
├── common_scheduling/          # SSB, SI, CSI-RS, RA, PRACH, paging schedulers
├── config/                     # Cell/UE configuration management and validation
├── configured_grant/           # Configured grant PUSCH scheduler
├── logging/                    # Event logger, result logger, metrics handler, schedtrace hook ([log_reference.md](log_reference.md))
├── pdcch_scheduling/           # PDCCH/DCI allocation
├── policy/                     # Scheduling policy interface and implementations
├── pucch_scheduling/           # PUCCH resource allocation
├── rrm/                        # PUCCH, SRS and configured grant resource managers
├── slicing/                    # RAN slice instances, slice UE repository and inter-slice scheduler
├── srs/                        # SRS resource allocation and periodic SRS scheduler
├── support/                    # MCS tables, BWP helpers, DMRS, power control
├── trace/                      # schedtrace event tracing and conversion
├── uci_scheduling/             # UCI allocation (PUCCH and PUSCH mux), periodic UCI scheduler, UCI indication selector
├── ue_context/                 # UE and UE-cell state objects
└── ue_scheduling/              # UE grant scheduler, fallback, intra-slice allocator
```

The event handling spans both levels: `cell_event_manager.{h,cpp}` and the interfaces in `cell_group_event_handler.h`
sit at the top level, next to `cell_scheduler`, and `ue_scheduling/cell_group_event_manager.{h,cpp}` holds the
cell-group side.
