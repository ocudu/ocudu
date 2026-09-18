# Fuzz Test Harnesses

Coverage-guided fuzz testing, targeting [AFL++](https://github.com/AFLplusplus/AFLplusplus). The harnesses use the
`LLVMFuzzerTestOneInput` interface so they also run unmodified under [libFuzzer](https://llvm.org/docs/LibFuzzer.html)
and are compatible with [OSS-Fuzz](https://github.com/google/oss-fuzz).

---

## Prerequisites

### Ubuntu / Debian

Install all required dependencies in one step:

```bash
sudo apt-get update
sudo apt-get install -y \
    afl++ \
    clang \
    llvm \
    llvm-dev \
    python3 \
    cmake \
    ninja-build
```

`afl++` pulls in `afl-clang-fast++`, `afl-fuzz`, `afl-cmin`, `afl-tmin`, `afl-whatsup`, and `afl-plot`. `llvm` /
`llvm-dev` provide `llvm-symbolizer`, which is needed to resolve symbol names in ASAN crash reports.

> **Version note:** The `afl++` package in Ubuntu 26.04 LTS ships AFL++ 4.33c. If you need a newer release, see the
> [AFL++ installation instructions](https://github.com/AFLplusplus/AFLplusplus/blob/stable/docs/INSTALL.md).

### Verify the installation

```bash
afl-fuzz --version
afl-clang-fast++ --version
llvm-symbolizer --version
```

---

## Building

The fuzz targets are gated behind the `ENABLE_FUZZTESTS` CMake option, which is **off by default** to avoid affecting
normal builds.

### Recommended configuration (AFL++ + AddressSanitizer)

```bash
mkdir build_fuzz && cd build_fuzz

CXX=afl-clang-fast++ CC=afl-clang-fast \
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_FUZZTESTS=ON \
    -DENABLE_ASAN=ON \
    -DBUILD_TESTING=OFF \
    -DENABLE_DPDK=OFF

make -j$(nproc) fuzz_targets

cd ..   # return to the repository root

```

`fuzz_targets` is an aggregate target that builds every fuzz binary, so no target list needs updating when a new harness
is added.

`ENABLE_ASAN=ON` adds `-fsanitize=address` on top of the fuzzer instrumentation, which is the recommended combination
for catching memory safety bugs.

> **Note:** `BUILD_TESTING=OFF` is optional but speeds up the build by skipping the full unit-test suite. The fuzz
> targets have no dependency on it.

### libFuzzer (alternative, no AFL++ required)

The same source files compile directly with libFuzzer if you do not have AFL++ installed. Replace the compiler wrappers
with plain `clang++`:

```bash
mkdir build_libfuzz && cd build_libfuzz

CXX=clang++ CC=clang \
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_FUZZTESTS=ON \
    -DENABLE_ASAN=ON \
    -DBUILD_TESTING=OFF

make -j$(nproc) fuzz_targets

cd ..   # return to the repository root

```

---

## Seed corpus

> All commands from this point on are run from the **repository root**.

The seed corpus can be generated using:

```bash
python3 tests/fuzz/ofh/gen_corpus.py
python3 tests/fuzz/ngap/gen_corpus.py
python3 tests/fuzz/rrc/gen_corpus.py
```

Each script writes binary seed files into the corpus sub-directories of its own layer.

### OSS-Fuzz zip format

Each script accepts an optional flag to write seeds as a zip file suitable for the `$OUT/<target>_seed_corpus.zip`
convention expected by OSS-Fuzz:

```bash
# NGAP: one zip per target
python3 tests/fuzz/ngap/gen_corpus.py \
    --zip $OUT/ngap_pdu_decoder_fuzzer_seed_corpus.zip \
    --zip-cu-cp $OUT/ngap_cu_cp_fuzzer_seed_corpus.zip

# OFH: one zip per target
python3 tests/fuzz/ofh/gen_corpus.py --zip-dir $OUT/

# RRC: one zip per target
python3 tests/fuzz/rrc/gen_corpus.py \
    --zip $OUT/rrc_ue_fuzzer_seed_corpus.zip \
    --zip-cu-cp $OUT/rrc_cu_cp_fuzzer_seed_corpus.zip
```

---

## Running

### Prepare output directories

```bash
# Default: findings/ inside the repository root
mkdir -p findings/uplane findings/ecpri findings/vlan findings/ngap findings/ngap_cu_cp findings/rrc_ue \
         findings/rrc_cu_cp

# Alternative: any absolute path
export FUZZ_OUTPUT_DIR=/tmp/fuzz_findings
mkdir -p $FUZZ_OUTPUT_DIR/uplane $FUZZ_OUTPUT_DIR/ecpri $FUZZ_OUTPUT_DIR/vlan \
         $FUZZ_OUTPUT_DIR/ngap $FUZZ_OUTPUT_DIR/ngap_cu_cp $FUZZ_OUTPUT_DIR/rrc_ue \
         $FUZZ_OUTPUT_DIR/rrc_cu_cp
```

`FUZZ_OUTPUT_DIR` is picked up automatically by `run_fuzzers.sh`. For manual `afl-fuzz` invocations, pass the path
directly via `-o`.

### OFH U-Plane decoder fuzzer

```bash
afl-fuzz \
    -i tests/fuzz/ofh/corpus/uplane \
    -o findings/uplane \
    -- ./build_fuzz/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer @@
```

### OFH eCPRI decoder fuzzer

```bash
afl-fuzz \
    -i tests/fuzz/ofh/corpus/ecpri \
    -o findings/ecpri \
    -- ./build_fuzz/tests/fuzz/ofh/ofh_ecpri_decoder_fuzzer @@
```

### OFH VLAN frame decoder fuzzer

```bash
afl-fuzz \
    -i tests/fuzz/ofh/corpus/vlan \
    -o findings/vlan \
    -- ./build_fuzz/tests/fuzz/ofh/ofh_vlan_frame_decoder_fuzzer @@
```

### NGAP PDU decoder fuzzer (ASN.1 only)

```bash
afl-fuzz \
    -i tests/fuzz/ngap/corpus/ngap \
    -o findings/ngap \
    -- ./build_fuzz/tests/fuzz/ngap/ngap_pdu_decoder_fuzzer @@
```

### NGAP full-stack CU-CP fuzzer

```bash
# Recommended: AFL_FAST_CAL for higher throughput (the CU-CP stays alive across iterations)
AFL_FAST_CAL=1 afl-fuzz \
    -i tests/fuzz/ngap/corpus/ngap_cu_cp \
    -o findings/ngap_cu_cp \
    -- ./build_fuzz/tests/fuzz/ngap/ngap_cu_cp_fuzzer @@
```

> **Note:** Use `AFL_FAST_CAL=1` (persistent mode) or libFuzzer for throughput; in standard fork mode each child re-runs
> the NG Setup handshake before processing its input.

Spins up a real CU-CP with stub AMF, CU-UP and DU peers attached and injects decoded NGAP messages into it, so that the
validators, procedure dispatcher and state machines all run.

A UE is brought up before each input, and the message is pointed at it by a rewrite of the AMF-UE-NGAP-ID and the
RAN-UE-NGAP-ID that the message carries. Both halves are necessary: most of NGAP is UE-associated, so without a UE, or
with one that the message does not name, the CU-CP rejects each input at the UE lookup before a procedure runs. Only
initiating messages are rewritten, because that is the direction the AMF sends.

#### Input format

The first byte is a control byte; the remaining bytes are the NGAP PDU.

| Bit | Meaning                                         |
| --- | ----------------------------------------------- |
| 0-1 | UE state reached before the message is injected |
| 2-7 | Unused                                          |

| Value | UE state                                                                      |
| ----- | ----------------------------------------------------------------------------- |
| 0     | UE created by an Initial UL RRC Message Transfer, awaiting `RRCSetupComplete` |
| 1     | `RRCSetupComplete` handled. SRB1 is up, AS security is not active             |
| 2, 3  | AS security activated on SRB1, UE capabilities transferred, SRB2 created      |

As with the RRC harness, the bring-up is checked once at startup and the harness aborts if it did not complete, so that
a UE that silently fails to come up cannot look like a healthy run.

### RRC UE uplink fuzzer

```bash
AFL_FAST_CAL=1 afl-fuzz \
    -i tests/fuzz/rrc/corpus/rrc_ue \
    -o findings/rrc_ue \
    -- ./build_fuzz/tests/fuzz/rrc/rrc_ue_fuzzer @@
```

This harness drives an `rrc_ue_impl` directly, with no CU-CP, PDCP or F1AP around it. RRC is the only CU-CP layer an
unauthenticated attacker reaches: `RRCSetupRequest`, `RRCSetupComplete` and `RRCReestablishmentRequest` are processed
before AS security is activated.

Injecting above PDCP is what makes the layer fuzzable at all. Once security is activated, PDCP verifies a MAC-I over the
RRC PDU, so a mutated payload is rejected before RRC ever sees it and the UE is released. Above PDCP the
`integrity_verified` flag becomes a fuzzer-controlled input bit, which is what reaches the TS 38.331 Annex B1 gating
branches in `rrc_ue_impl::handle_pdu()`: a protected message arriving unprotected, and an unprotected message arriving
protected.

#### Input format of the RRC UE uplink fuzzer

The first byte is a control byte; the remaining bytes are the RRC PDU.

| Bit | Meaning                                                     |
| --- | ----------------------------------------------------------- |
| 0   | Logical channel: 0 = UL-CCCH, 1 = UL-DCCH                   |
| 1   | `integrity_verified` flag passed to the UL-DCCH handler     |
| 2   | SRB: 0 = SRB1, 1 = SRB2                                     |
| 3-4 | UE state reached before the payload is injected (see below) |
| 5-7 | Unused                                                      |

| Value | UE state                                                                |
| ----- | ----------------------------------------------------------------------- |
| 0     | Fresh. No RRC connection; only UL-CCCH is meaningful                    |
| 1     | `RRCSetupRequest` handled, `RRCSetup` sent, awaiting `RRCSetupComplete` |
| 2     | `RRCSetupComplete` handled. SRB1 is up, AS security is not active       |
| 3     | AS security activated on SRB1 and SRB2 created                          |

The UE is rebuilt for every input, so a crash reproduces from its input file alone rather than depending on the inputs
the fuzzer happened to run before it. The canned messages used to reach each state come from
`tests/unittests/rrc/rrc_ue_test_helpers.h`.

Logs are routed to `/dev/null` at debug level rather than switched off: `log_rrc_message()` and the `to_json()` call in
`rrc_ue_impl::store_ue_capabilities()` walk the decoded, attacker-controlled ASN.1 structures, which makes them part of
the surface under test.

### Full-stack RRC / CU-CP fuzzer

```bash
AFL_FAST_CAL=1 afl-fuzz \
    -i tests/fuzz/rrc/corpus/rrc_cu_cp \
    -o findings/rrc_cu_cp \
    -- ./build_fuzz/tests/fuzz/rrc/rrc_cu_cp_fuzzer @@
```

Injects RRC messages through a complete CU-CP with stub AMF, CU-UP and DU peers attached, so that the F1AP, PDCP and
CU-CP layers around RRC are exercised on every input. The F1AP wrapper and the PDCP PDU are scaffolding the harness
builds; only the RRC message inside is mutated. Fuzzing the F1AP wrapper itself belongs in a target under
`tests/fuzz/f1ap`.

A UE is created and released for every input, so a crash reproduces from its input file alone. The UE pool is capped at
8, which turns a UE that fails to be released into an immediate failure to create the next one rather than a slow leak.

#### Input format of the full-stack fuzzer

The first byte is a control byte; the remaining bytes are the RRC message. It carries no `integrity_verified` bit,
unlike `rrc_ue_fuzzer`: here PDCP derives that from the MAC-I the harness computes.

| Bit | Meaning                                                     |
| --- | ----------------------------------------------------------- |
| 0   | Logical channel: 0 = UL-CCCH, 1 = UL-DCCH                   |
| 1   | SRB: 0 = SRB1, 1 = SRB2                                     |
| 2-3 | UE state reached before the message is injected (see below) |
| 4-7 | Unused                                                      |

| Value | UE state                                                                      |
| ----- | ----------------------------------------------------------------------------- |
| 0     | UE created by an Initial UL RRC Message Transfer, awaiting `RRCSetupComplete` |
| 1     | `RRCSetupComplete` handled. SRB1 is up, AS security is not active             |
| 2, 3  | AS security activated on SRB1, UE capabilities transferred, SRB2 created      |

#### Reaching the post-security states

Once AS security is active, the CU-CP's SRB entity verifies a MAC-I over every RRC PDU. A payload carrying a zero MAC
would be dropped on integrity failure and the UE released, so the fuzzer would never reach RRC at all.

The DU side therefore runs its own PDCP TX entities, keyed with the AS keys the CU-CP derives for the UE: the harness
rebuilds the security context from the K_gNB that the injected Initial Context Setup Request carries, using the same
algorithm preferences as the CU-CP configuration. Mutated payloads are then packed the way the UE packs them, MAC-I
included, and the CU-CP accepts them.

A wrong key, direction or algorithm would fail silently: every input dropped, the harness still reporting healthy
throughput while covering nothing past RRC Setup. To make that impossible to miss, the harness brings one UE all the way
up at startup and aborts if security did not activate.

#### Choosing between the two RRC targets

|                                  | `rrc_ue_fuzzer` | `rrc_cu_cp_fuzzer`                         |
| -------------------------------- | --------------- | ------------------------------------------ |
| Layers under test                | RRC UE only     | F1AP, CU-CP, PDCP, RRC                     |
| `integrity_verified` as an input | Yes             | No, derived from the MAC-I                 |
| Throughput (ASan, Debug)         | ~1000 exec/s    | ~450 exec/s connected, ~110 exec/s secured |
| Cross-layer bugs                 | Out of reach    | In reach                                   |

The secured state costs a full Initial Context Setup exchange per input, which is where the throughput difference
between the two states comes from.

### Running in parallel (recommended)

AFL++ scales linearly with additional CPU cores. Use one main instance (`-M`) and one or more secondary instances (`-S`)
pointing at the same output directory:

```bash
# Terminal 1 - main instance
afl-fuzz -M main \
    -i tests/fuzz/ofh/corpus/uplane \
    -o findings/uplane \
    -- ./build_fuzz/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer @@

# Terminal 2 - secondary instance
afl-fuzz -S worker1 \
    -i tests/fuzz/ofh/corpus/uplane \
    -o findings/uplane \
    -- ./build_fuzz/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer @@
```

In parallel mode, AFL++ writes each instance's findings to a named subdirectory: `findings/uplane/main/`,
`findings/uplane/worker1/`, etc.

### Monitoring progress

```bash
# Live dashboard for a running instance
afl-whatsup findings/uplane

# Plot data over time
afl-plot findings/uplane/main plot_dir && open plot_dir/index.html
```

---

## Triaging crashes

### Locating crash files

Crash inputs are written to `findings/<target>/crashes/` for single-instance runs and to
`findings/<target>/<instance>/crashes/` for parallel runs (e.g. `findings/uplane/main/crashes/`). To reproduce a crash:

```bash
./build_fuzz/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer \
    findings/uplane/crashes/id:000000,*
```

### Minimising a crash

Reduce a crashing input to its smallest reproducing form before filing a bug:

```bash
afl-tmin \
    -i findings/uplane/crashes/id:000000,* \
    -o minimised_crash \
    -- ./build_fuzz/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer @@
```

### Symbolising ASAN output

ASAN prints a full stack trace when the crash is reproduced. Pipe through `llvm-symbolizer` if symbol names are missing:

```bash
ASAN_SYMBOLIZER_PATH=$(which llvm-symbolizer) \
    ./build_fuzz/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer \
    findings/uplane/crashes/id:000000,*
```

### Converting a crash to a unit-test literal

`crash_to_hex.py` converts AFL++ crash/hang input files into formats that can be pasted directly into a C++ unit test:

```bash
# Print a C array literal for a single crash file
python3 tests/fuzz/crash_to_hex.py --format c \
    findings/uplane/crashes/id:000000,*

# Convert all crashes in a directory at once
python3 tests/fuzz/crash_to_hex.py --format c \
    findings/uplane/crashes/
```

Available formats: `hex` (default), `xxd` (annotated hex dump), `c` (C/C++ `uint8_t` array literal). Add the resulting
literal to the appropriate unit test under `tests/unittests/` to prevent regressions.

---

## Updating the corpus

After a long local run, merge the newly discovered inputs back into the seed corpus so future runs start with a richer
base:

```bash
# Merge and deduplicate queue entries into the seed corpus
afl-cmin \
    -i findings/uplane/queue \
    -o tests/fuzz/ofh/corpus/uplane \
    -- ./build_fuzz/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer @@
```

Commit the updated corpus alongside code changes. In CI the corpus accumulates automatically across weekly runs via
AFL++ resume mode — see [Corpus accumulation](#corpus-accumulation) below.

---

## Docker

The `Dockerfile` in `tests/fuzz/` defines a build-environment image containing only system dependencies (AFL++, Clang,
LLVM, cmake, and the mandatory project libraries). Fuzz targets are compiled at run time from the checked-out source, so
the image only needs to be rebuilt when its dependencies change — not on every code change.

### Building the image

```bash
docker build -f tests/fuzz/Dockerfile -t ocudu-fuzz-deps .
```

Bump `FUZZ_IMAGE_VERSION` in `.gitlab-ci.yml` after pushing a new image so the CI jobs pick it up.

### Running locally

Mount the repository and a findings directory into the container, then build and run inside it:

```bash
docker run --rm --privileged \
    -v "$(pwd):/src" \
    -v "$(pwd)/findings:/findings" \
    -w /src \
    ocudu-fuzz-deps bash -c "
        cmake -S . -B /build -GNinja \
            -DCMAKE_C_COMPILER=afl-clang-fast \
            -DCMAKE_CXX_COMPILER=afl-clang-fast++ \
            -DCMAKE_BUILD_TYPE=Debug \
            -DENABLE_FUZZTESTS=ON \
            -DENABLE_ASAN=ON \
            -DBUILD_TESTING=OFF \
            -DENABLE_UHD=OFF \
            -DENABLE_ZEROMQ=OFF \
            -DENABLE_FFTW=OFF \
            -DENABLE_MKL=OFF \
            -DENABLE_FFTZ=OFF \
            -DENABLE_ARMPL=OFF \
            -DENABLE_DPDK=OFF \
            -DENABLE_LIBNUMA=OFF \
            -DENABLE_BACKWARD=OFF \
            -DENABLE_WERROR=OFF
        ninja -C /build fuzz_targets
        mkdir -p /tmp/corpus/ngap_cu_cp
        cp -r tests/fuzz/ofh/corpus/* /tmp/corpus/
        cp -r tests/fuzz/ngap/corpus/* /tmp/corpus/
        cp tests/fuzz/ngap/corpus/ngap/* /tmp/corpus/ngap_cu_cp/
        PATH=/build/tests/fuzz/ofh:/build/tests/fuzz/ngap:\$PATH \
            FUZZ_OUTPUT_DIR=/findings \
            FUZZ_CORPUS_DIR=/tmp/corpus \
            tests/fuzz/run_fuzzers.sh
    "
```

`--privileged` allows setting `/proc/sys/kernel/core_pattern` for accurate crash detection.

### Output layout

After the run, `findings/` on the host contains:

```text
findings/
├── ofh_uplane_decoder_fuzzer.log   afl-fuzz stdout for this target
├── ofh_ecpri_decoder_fuzzer.log
├── ofh_vlan_frame_decoder_fuzzer.log
├── ngap_pdu_decoder_fuzzer.log
├── ngap_cu_cp_fuzzer.log
├── uplane/                         AFL++ output directory
│   ├── crashes/                    inputs that caused a crash
│   ├── hangs/                      inputs that caused a hang
│   ├── queue/                      all corpus entries found
│   └── fuzzer_stats                run statistics
├── ecpri/
├── vlan/
├── ngap/
└── ngap_cu_cp/
```

---

### Corpus accumulation

`run_fuzzers.sh` detects a prior queue automatically: if `findings/<target>/queue/` exists when a run starts, AFL++
resumes from it (`-i -`); otherwise it starts from the seed corpus (`-i <corpus>`). The GitLab CI cache restores
`findings/` at the start of each weekly job, so coverage accumulates across runs without any manual intervention.

---

## OSS-Fuzz

The build is compatible with [OSS-Fuzz](https://github.com/google/oss-fuzz). When `$LIB_FUZZING_ENGINE` is set,
`tests/fuzz/CMakeLists.txt` uses it as the link target instead of `-fsanitize=fuzzer`, and leaves compile flags empty so
OSS-Fuzz's own instrumentation (injected via `$CXXFLAGS`) takes precedence. Seed corpus zips can be generated with the
`--zip` / `--zip-dir` flags described in [Seed corpus](#seed-corpus).
