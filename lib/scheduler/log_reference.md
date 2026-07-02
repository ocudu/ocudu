# Scheduler Log Reference

The MAC scheduler writes to the `SCHED` logger. Two independent loggers produce its output, both defined in `logging/`:

- **Result logger** (`scheduler_result_logger`) — one `Slot decisions` line per scheduled slot, describing the grants written into the `sched_result` (PDCCH, PDSCH, PUSCH, PUCCH, SRS, ...).
- **Event logger** (`scheduler_event_logger`) — one `Processed slot events` line per slot in which UE/cell events were processed (CRC, HARQ-ACK, BSR, UE creation, PRACH, ...).

Both are per-cell and tag every line with `pci=<physical cell id>`.

## Verbosity

The output is controlled by the level of the `SCHED` logger:

| Level | Result logger | Event logger |
|-------|---------------|--------------|
| `info`  | Summary line + compact one-line entries (`log_info`) | Only events with an info formatter: PRACH, RACH indication, ErrorIndication, cell creation |
| `debug` | Summary line + one detailed entry per line (`log_debug`) | All events, one detailed entry per line |
| below `info` | disabled | disabled |

`debug` is a superset of `info`: it logs more entry types and more fields per entry.

### `log_broadcast`

The result logger is constructed with a `log_broadcast` flag. When `false`, broadcast/common allocations (SSB, SIB, CSI-RS, PRACH occasions, and `SI-RNTI` PDCCH) are omitted. This is normally enabled for a single cell only, so recurring broadcast allocations do not flood the log across all cells.

## Log line anatomy

```
2026-07-02T09:29:21.835770 [SCHED   ] [D] [     1.5] Slot decisions pci=0 t=0us (1 PDSCH, ...):
└─ timestamp                 └─ logger  │    │        └─ message
                                        │    └─ slot as <SFN>.<slot-in-frame>
                                        └─ level: [I]=info, [D]=debug
```

The `[   SFN.slot]` field is the transmit slot the decisions apply to. Event lines carry the slot in which the events were processed.

---

## Result logger — `Slot decisions`

### Summary line

A slot is logged only if it carries something worth reporting: a UE DL/UL grant, RAR, paging, a non-`SI-RNTI` PDCCH, (with `log_broadcast`) a broadcast allocation, or any failed allocation attempt. Empty slots are skipped.

**info:**
```
Slot decisions pci=0 t=0us (1 PDSCH, 1 PUSCH, 1 PUCCH): <entries...>
```

**debug** (adds attempted-but-failed allocation counters):
```
Slot decisions pci=1 t=0us (1 PDSCH, 0 PUSCHs, 0 PUCCHs, 0 attempted PDCCHs, 0 attempted UCIs): <entries...>
```

| Field | Meaning |
|-------|---------|
| `t=<n>us` | Scheduler decision latency for this slot, microseconds |
| `N PDSCH` | Number of PDSCH grants (UE grants + RAR + paging, + SIBs when `log_broadcast`) |
| `N PUSCH` | Number of PUSCH grants |
| `N PUCCH` | Number of PUCCH grants |
| `N attempted PDCCH` | Failed DL+UL PDCCH allocation attempts (`debug` only) |
| `N attempted UCI` | Failed UCI allocation attempts (`debug` only) |

At **info** level the entries follow the summary on the *same line*, comma-separated. At **debug** level each entry is on its own line prefixed with `- `.

### info-level entries (compact, comma-separated)

| Prefix | Format |
|--------|--------|
| `SIB1` / `SI-<n>` | `SIB1: rb=<prbs> tbs=<bytes>` |
| `RAR` | `RAR: ra-rnti=<rnti> rb=<prbs> tbs=<bytes>` |
| `DL` | `DL: ue=<idx> c-rnti=<rnti> h_id=<harq> ss_id=<ss> rb=<prbs> k1=<k1> newtx=<0/1> rv=<rv> tbs=<bytes>` — on new transmissions appends ` ri=<layers> dl_bo=<buffer-occupancy>` |
| `UL` | `UL: ue=<idx> rnti=<rnti> h_id=<harq> ss_id=<ss> rb=<prbs> newtx=<0/1> rv=<rv> tbs=<bytes>` — then ` k2=<k2>`, or ` msg3_delay=<n>` for a Msg3 grant (invalid UE index, first tx) |
| `PG` | `PG: rb=<prbs> tbs=<bytes> ues: <cn\|ran>-pg-id=<0x..>, ...` |

### debug-level entries (one per line, `- ` prefix)

**Control channels:**
```
- DL PDCCH: rnti=0x4601 type=c-rnti cs_id=1 ss_id=2 format=1_1 cce=8 al=2 dci: h_id=0 ndi=1 rv=0 mcs=3 res_ind=0 tpc=1 dai=0
- UL PDCCH: rnti=0x4601 type=c-rnti cs_id=1 ss_id=2 format=0_1 cce=10 al=2 dci: h_id=0 ndi=1 rv=0 mcs=9 tpc=1 dai=0 mimo=0 ant=2
```
| Field | Meaning |
|-------|---------|
| `type` | RNTI type (`c-rnti`, `tc-rnti`, `si-rnti`, ...) |
| `cs_id` / `ss_id` | CORESET id / SearchSpace id |
| `format` | DCI format (`1_0`, `1_1`, `0_0`, `0_1`) |
| `cce` / `al` | Starting CCE index / aggregation level (number of CCEs) |
| `dci: ...` | Decoded DCI fields — `h_id` HARQ process, `ndi`, `rv`, `mcs`. DL adds `res_ind` (PUCCH resource indicator) and optional `tpc`/`dai`/`vrb_prb_map_used`. UL adds `tpc` and, for format 0_1, `dai`/`mimo` (layers)/`ant`/`csi_req` |

**DL shared channel:**
```
- SSB: ssbIdx=0 crbs=[5..26) symb=[2..6)
- CSI-RS: type=nzp crbs=[0..52) row=1 freq=0010 symb0=4 cdm_type=no_CDM freq_density=three scramb_id=1
- SIB1 PDSCH: rb=[0..4) symb=[2..14) tbs=120 mcs=5 rv=0
- RAR PDSCH: ra-rnti=0x2 rb=[0..8) symb=[1..14) tbs=32 mcs=0 rv=0 grants (1): tc-rnti=0x4601: rapid=5 ta=12 time_res=0
- UE PDSCH: ue=0 c-rnti=0x4601 h_id=0 rb=[0..25) symb=[1..14) tbs=309 mcs=9 rv=0 nrtx=0 k1=4 ri=1 <pmi> dl_bo=1024 olla=0.03 grants: lcid=1: size=53, lcid=4: size=256
- PCCH: rb=[0..4) symb=[2..14) tbs=40 mcs=5 rv=0 ues: cn-pg-id=0x1234
```
- `UE PDSCH`: `nrtx` = retransmission count; `ri`/PMI printed only when precoding present; `dl_bo` (DL buffer occupancy) only on new data; `olla` (outer-loop link-adaptation offset) when set; per-logical-channel `grants:` list when a TB is built. RAR `result=successRAR|fallbackRAR` appears for 2-step grants.

**UL shared channel and control:**
```
- UE PUSCH: ue=0 c-rnti=0x4601 h_id=0 rb=[1..52) symb=[0..14) tbs=1121 rv=0 nrtx=0 nof_layers=1 olla=0 k2=4 uci: harq_bits=1 csi-1_bits=0 csi-2_present=No
- PUCCH: c-rnti=0x4601 format=1 prb=[0..1) prb2=50 symb=[0..14) cs=0 occ=0 uci: harq_bits=1 sr=0
- SRS: c-rnti=0x4601 symb=[8..10) tx-comb=(n2 o=0 cs=0) c_srs=0 f_sh=0 seq_id=0 requests=[ch_mtx=yes pos=no]
- PRACH: pci=1 format=0 nof_occasions=1 nof_preambles=64
```
- `UE PUSCH`: a leading `t` before `c-rnti` marks a temporary-C-RNTI (Msg3) grant; `k2` is replaced by `msg3_delay` for the initial Msg3. `uci:` block present when UCI is multiplexed on PUSCH.
- `PUCCH`: `prb2` shown when frequency hopping; format-specific fields — `cs`/`occ` (F1), `occ=<idx>/<len>` (F4); `csi-1_bits` shown for F2/F3/F4.

### Interval notation

`rb`, `symb`, `crbs` are printed as half-open intervals `[start..stop)` — e.g. `rb=[1..52)` is 51 PRBs starting at PRB 1; `symb=[0..14)` is all 14 OFDM symbols.

---

## Event logger — `Processed slot events`

Emitted once per slot in which one or more UE/cell events were processed.

**info** (space-separated, only events with an info formatter):
```
Processed slot events pci=1: prach(ra-rnti=0x2 preamble=5 tc-rnti=0x4601), RACH Ind slot_rx=100.3, ErrorIndication slot=100.5
```

**debug** (one event per line):
```
Processed slot events pci=1:
- UE creation: ue=0 rnti=0x4601
- CRC: ue=0 rnti=0x4601 rx_slot=401.9 h_id=0 crc=true sinr=100dB
- HARQ-ACK: ue=0 rnti=0x4601 slot_rx=401.5 h_id=0 ack=1 tbs=720
- BSR: ue=0 rnti=0x4601 type="Short BSR" report={2: 700} pending_bytes=706
- CSI: ue=0 rnti=0x4601: slot_rx=408.9 cqi=15
- RLC Buffer State: ue=0 lcid=4 pending_bytes=2400
```

### Event types

| Event | Levels | Notes |
|-------|--------|-------|
| Cell creation | info + debug | Emitted once at logger construction |
| PRACH | info + debug | Detected preamble → RA-RNTI/MsgB-RNTI, `tc-rnti`; debug adds `preamble`, `temp_crnti`, `ta_cmd` |
| RACH ind | info + debug | debug lists each occasion: `preamble`, `tc-rnti`, `freq_idx`, `start_symbol`, `TA` |
| ErrorIndication | info + debug | debug lists which channels were discarded (PDCCH / PDSCH / PUSCH+PUCCH) |
| UE creation / reconfiguration / removal | debug | `ue=<idx> rnti=<rnti>` |
| UE dedicated config applied / UE deactivation | debug | `ue=<idx> rnti=<rnti>` |
| SR | debug | Scheduling request |
| CSI | debug | `cqi` / `ri` / `pmi` when present, else `invalid` |
| BSR | debug | `type` (Short/Long/...), per-LCG `report={...}`, `pending_bytes` |
| HARQ-ACK | debug | `ack` status (0/1/2); `tbs` shown on ACK |
| CRC | debug | `crc=true/false`, `sinr=<n>dB` or `sinr=N/A` |
| MAC CE | debug | `ue=<idx> lcid=<lcid>` |
| RLC Buffer State | debug | `ue=<idx> lcid=<lcid> pending_bytes=<n>` |
| PHR | debug | `ph=<n>dB`, optional `p_cmax=<n>dBm` |
| SRS | debug | optional `tpmi_info=[...]` |
| Slice Reconfig | debug | `cell=<idx>` |

---

## Common field glossary

| Field | Meaning |
|-------|---------|
| `pci` | Physical cell id |
| `ue` | DU UE index (internal); `INVALID`/`t`-prefix denotes a temporary (pre-attach) context |
| `c-rnti` / `rnti` | UE C-RNTI |
| `tc-rnti` | Temporary C-RNTI (RA / Msg3 / Msg4) |
| `ra-rnti` / `msgb-rnti` | RA-RNTI (4-step) / MsgB-RNTI (2-step) derived from the PRACH occasion |
| `h_id` | HARQ process id |
| `ndi` / `rv` | New-data indicator / redundancy version |
| `mcs` | Modulation and coding scheme index |
| `tbs` | Transport block size, bytes |
| `rb` | Allocated PRBs, half-open interval |
| `symb` | Allocated OFDM symbols, half-open interval |
| `k1` | PDSCH-to-HARQ-ACK slot offset |
| `k2` | PDCCH/DCI-to-PUSCH slot offset |
| `nrtx` / `nof_retxs` | HARQ retransmission count |
| `dl_bo` | DL buffer occupancy (pending bytes) |
| `olla` | Outer-loop link-adaptation MCS offset |
| `ss_id` / `cs_id` | SearchSpace id / CORESET id |
