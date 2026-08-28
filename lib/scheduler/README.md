# MAC Scheduler

The MAC scheduler allocates radio resources — time, frequency, and control channels — across all active cells in a DU. It runs once per slot per cell and produces a `sched_result` that the MAC layer uses to generate DL assignments and UL grants.

## Component Hierarchy

```
scheduler_impl
├── sched_config_manager          # Validates and distributes cell/UE config
├── cell_scheduler  [per cell]    # All resources specific to one cell
│   ├── cell_metrics_handler      # Per-slot metric aggregation for this cell
│   ├── cell_resource_allocator   # Circular-buffer resource grid (16 slots)
│   ├── ssb_scheduler             # SSB bursts
│   ├── pdcch_resource_allocator  # PDCCH/DCI allocation
│   ├── si_scheduler              # SIB1 and SI messages
│   ├── csi_rs_scheduler          # CSI-RS
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
└── ue_scheduler  [per cell group]  # Shared UE state across a CA group
    ├── ue_repository               # UE contexts of the cell group (ue objects)
    ├── cell_group_event_manager    # Owns one event handler per cell of the group
    ├── ue_fallback_scheduler       # SRB0 / contention-resolution grants
    ├── inter_slice_scheduler       # Prioritizes slices for each slot
    └── intra_slice_scheduler       # PDSCH/PUSCH allocation within a slice
        ├── grant_params_selector   # MCS, PRBs, HARQ selection
        └── ue_cell_grid_allocator  # Writes grants into the resource grid
```

A `ue_scheduler` is shared across all cells in a cell group (Carrier Aggregation), and holds the state that spans a UE's carriers. Each `cell_scheduler` holds a `ue_cell_scheduler` handle, which is an RAII view into the shared `ue_scheduler`.

The `uci_scheduler` and the `srs_scheduler` are owned by the cell but driven from the UE scheduling step, as they have to run before any UL grant of the slot.

## Slot Processing Flow

`scheduler_impl::slot_indication(sl_tx, cell_index)` is the entry point. It calls `cell_scheduler::run_slot()`, which executes in this order:

1. **Reset resource grid** — clear stale allocations for this slot.
2. **Events** — process everything that arrived since the last slot (`cell_event_manager`).
3. **SSB** — schedule Synchronization Signal Blocks.
4. **CSI-RS** — schedule CSI-RS if due.
5. **SI** — schedule SIB1 and SI-message windows.
6. **PRACH** — schedule PRACH occasions.
7. **RA** — schedule random access (RA) grants (e.g. RAR and Msg3) for detected RACH preambles.
8. **Paging** — schedule paging PDCCH and PDSCH.
9. **UE scheduling** (`ue_cell_scheduler::run_slot`):
   1. Advance UE state machines (DRX, timing advance, HARQ timers).
   2. Schedule SR and CSI PUCCH opportunities (`uci_scheduler`).
   3. Schedule periodic and aperiodic SRS (`srs_scheduler`).
   4. Schedule configured grant PUSCH opportunities, if configured.
   5. Schedule SRB0 / fallback grants (`ue_fallback_scheduler`).
   6. Prioritize slices for this slot (`inter_slice_scheduler::slot_indication`).
   7. For each slice in priority order:
      - Schedule PDSCH retransmissions, then new transmissions.
      - Schedule PUSCH retransmissions, then new transmissions.
   8. Post-process allocations (finalize PUCCH/UCI state).
   9. Inject synthetic BSRs for the UEs that need a triggered UL grant.
10. **UCI indications** — match the UCI grants of the finished slot to their indications (`uci_indication_selector`).
11. **Logging and metrics** — flush the event log, the result log and the slot metrics.

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
`cell_group_event_manager` owns one implementation of it per cell of the group. The cell scheduler creates its
`ue_cell_scheduler` handle before its `cell_event_manager`, so the handler already exists when the manager is given the
reference.

The cell also aggregates the DL buffer occupancy updates that it receives for a bearer since the last slot, and relays
only the resulting update.

## Resource Grid

`cell_resource_allocator` is a circular ring buffer over `cell_slot_resource_allocator` entries — one per slot. Each entry contains:

- `sched_result` — the scheduling decisions (PDCCHs, PDSCHs, PUSCHs, etc.)
- A DL and UL `carrier_subslot_resource_grid` per SCS — a Symbol × CRB bitmap used for collision detection and availability checks.

Allocators receive a `cell_slot_resource_allocator&` and write into it directly. The ring buffer provides a lookahead window so allocators can inspect and reserve resources in future slots (e.g., PUSCH scheduled K2 slots ahead of the PDCCH slot).

## RAN Slicing

The scheduler supports multiple RAN slices, each configured with a `slice_rrm_policy_config` (min/max RB limits, S-NSSAI). Two scheduling layers handle slicing:

**Inter-slice scheduler** (`slicing/inter_slice_scheduler.h`) — runs once per slot and produces a priority-ordered queue of `ran_slice_candidate` objects for DL and UL. Priority is computed from slice SLA parameters, recent utilization, and RB limits.

**Intra-slice scheduler** (`ue_scheduling/intra_slice_scheduler.h`) — consumes one slice candidate at a time and allocates PDSCH/PUSCH grants for UEs in that slice. Within a slice, UE ordering is determined by the slice's `scheduler_policy`.

### Scheduling Policies

`scheduler_policy` (`policy/scheduler_policy.h`) is the pluggable per-slice algorithm interface:

- `compute_ue_dl_priorities()` / `compute_ue_ul_priorities()` — assign a `ue_sched_priority` to each UE candidate.
- `save_dl_newtx_grants()` / `save_ul_newtx_grants()` — called after allocation to update internal state.
- `add_ue()` / `rem_ue()` — track UE membership.

Available implementations (in `policy/`) include Round-Robin with QoS weighting and time-domain fair scheduling.

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
- Fallback mode flag (set on repeated HARQ failures; cleared by MAC).

The `ue` objects live in the `ue_repository` of the cell group; the `ue_cell` objects live in the
`ue_cell_repository` of the cell that owns them, which the cell scheduler owns.

## Configuration Management

`sched_config_manager` (`config/sched_config_manager.h`) is the single point of entry for all configuration:

- **Cell config** — validates `sched_cell_configuration_request_message` and builds a `cell_configuration` object used throughout the scheduler.
- **UE config** — validates `sched_ue_creation_request_message` / `sched_ue_reconfiguration_message`, builds a thread-safe config snapshot, and emits a `ue_config_update_event` that the UE PCell queues like any other event.

Configuration changes are never applied mid-slot; they are queued as events and applied at the start of the next `run_slot()` call, in their arrival order relative to the indications of the same UE.

## Logging

The scheduler writes to the `SCHED` logger via two loggers in `logging/`: a result logger (`Slot decisions` lines) and an event logger (`Processed slot events` lines). See [log_reference.md](log_reference.md) for the full format of every log line and field.

## Directory Layout

```
lib/scheduler/
├── scheduler_impl.{h,cpp}      # mac_scheduler implementation
├── scheduler_factory.cpp       # create_scheduler() factory
├── cell_scheduler.{h,cpp}      # Per-cell orchestrator
├── cell/                       # Resource grid, HARQ entities, PRB tracking
├── common_scheduling/          # SSB, SI, CSI-RS, RA, PRACH, paging schedulers
├── config/                     # Cell/UE configuration management and validation
├── logging/                    # Event logger, result logger, metrics handler ([log_reference.md](log_reference.md))
├── pdcch_scheduling/           # PDCCH/DCI allocation
├── policy/                     # Scheduling policy interface and implementations
├── pucch_scheduling/           # PUCCH resource allocation
├── slicing/                    # RAN slice instances and inter-slice scheduler
├── srs/                        # SRS resource allocation
├── support/                    # MCS tables, BWP helpers, DMRS, power control
├── uci_scheduling/             # UCI allocation (PUCCH and PUSCH mux)
├── ue_context/                 # UE and UE-cell state objects
└── ue_scheduling/              # UE grant scheduler, fallback, intra-slice allocator
```

The event handling spans both levels: `cell_event_manager.{h,cpp}` and the interfaces in
`cell_group_event_handler.h` sit at the top level, next to `cell_scheduler`, and
`ue_scheduling/cell_group_event_manager.{h,cpp}` holds the cell-group side.
