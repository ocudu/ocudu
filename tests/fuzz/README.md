# Fuzz Test Harnesses

Coverage-guided fuzz testing, targeting [AFL++](https://github.com/AFLplusplus/AFLplusplus). The harnesses use the `LLVMFuzzerTestOneInput` interface so they also run unmodified under [libFuzzer](https://llvm.org/docs/LibFuzzer.html).

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

`afl++` pulls in `afl-clang-fast++`, `afl-fuzz`, `afl-cmin`, `afl-tmin`,
`afl-whatsup`, and `afl-plot`.  `llvm` / `llvm-dev` provide
`llvm-symbolizer`, which is needed to resolve symbol names in ASAN crash
reports.

> **Version note:** The `afl++` package in Ubuntu 26.04 LTS ships AFL++
> 4.33c. If you need a newer release, see the
> [AFL++ installation instructions](https://github.com/AFLplusplus/AFLplusplus/blob/stable/docs/INSTALL.md).

### Verify the installation

```bash
afl-fuzz --version
afl-clang-fast++ --version
llvm-symbolizer --version
```

---

## Building

The fuzz targets are gated behind the `ENABLE_FUZZTESTS` CMake option, which
is **off by default** to avoid affecting normal builds.

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

`fuzz_targets` is an aggregate target that builds every fuzz binary, so no
target list needs updating when a new harness is added.

`ENABLE_ASAN=ON` adds `-fsanitize=address` on top of the fuzzer
instrumentation, which is the recommended combination for catching memory
safety bugs.

> **Note:** `BUILD_TESTING=OFF` is optional but speeds up the build by
> skipping the full unit-test suite.  The fuzz targets have no dependency on
> it.

### libFuzzer (alternative, no AFL++ required)

The same source files compile directly with libFuzzer if you do not have
AFL++ installed.  Replace the compiler wrappers with plain `clang++` and add
`-fsanitize=fuzzer` manually:

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

## Generating the seed corpus

> All commands from this point on are run from the **repository root**.

The seed corpus is committed to the repository under
`tests/fuzz/<layer>/corpus/` and is derived directly from the unit-test
vectors.  If you need to regenerate it (e.g. after adding new unit tests):

```bash
python3 tests/fuzz/ofh/gen_corpus.py
python3 tests/fuzz/ngap/gen_corpus.py
```

Each script writes binary seed files into the corpus sub-directories of its
own layer.

---

## Running

### Prepare output directories

```bash
# Default: findings/ inside the repository root
mkdir -p findings/uplane findings/ecpri findings/vlan findings/ngap findings/ngap_cu_cp

# Alternative: any absolute path
export FUZZ_OUTPUT_DIR=/tmp/fuzz_findings
mkdir -p $FUZZ_OUTPUT_DIR/uplane $FUZZ_OUTPUT_DIR/ecpri $FUZZ_OUTPUT_DIR/vlan \
         $FUZZ_OUTPUT_DIR/ngap $FUZZ_OUTPUT_DIR/ngap_cu_cp
```

`FUZZ_OUTPUT_DIR` is picked up automatically by `run_fuzzers.sh`.  For manual
`afl-fuzz` invocations, pass the path directly via `-o`.

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
python3 tests/fuzz/ngap/gen_corpus.py   # generate seeds (only needed once)
mkdir -p findings/ngap
afl-fuzz \
    -i tests/fuzz/ngap/corpus/ngap \
    -o findings/ngap \
    -- ./build_fuzz/tests/fuzz/ngap/ngap_pdu_decoder_fuzzer @@
```

### NGAP full-stack CU-CP fuzzer

```bash
mkdir -p findings/ngap_cu_cp
# Recommended: persistent mode for higher throughput (CU-CP stays alive across iterations)
AFL_FAST_CAL=1 afl-fuzz \
    -i tests/fuzz/ngap/corpus/ngap \
    -o findings/ngap_cu_cp \
    -- ./build_fuzz/tests/fuzz/ngap/ngap_cu_cp_fuzzer @@
```

> **Note:** Use `AFL_FAST_CAL=1` (persistent mode) or libFuzzer for throughput; in standard fork
> mode each child re-runs the NG Setup handshake before processing its input.

#### Test double architecture

| Component | Role |
|---|---|
| `fuzz_amf` | `n2_connection_client` stub; `push_tx_pdu()` injects a decoded `ngap_message` into the CU-CP, `try_pop_rx_pdu()` drains responses sent by the CU-CP |
| `fuzz_xnc_gateway` | No-op `xnc_connection_gateway`; the XN-C interface is not under test |
| `task_worker` | Background thread that executes CU-CP tasks |
| `timer_manager` | Driven from the fuzzer main thread via `tick()` to cover timer-expiry code paths |

### Running in parallel (recommended)

AFL++ scales linearly with additional CPU cores.  Use one main instance
(`-M`) and one or more secondary instances (`-S`) pointing at the same output
directory:

```bash
# Terminal 1 – main instance
afl-fuzz -M main \
    -i tests/fuzz/ofh/corpus/uplane \
    -o findings/uplane \
    -- ./build_fuzz/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer @@

# Terminal 2 – secondary instance
afl-fuzz -S worker1 \
    -i tests/fuzz/ofh/corpus/uplane \
    -o findings/uplane \
    -- ./build_fuzz/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer @@
```

### Monitoring progress

```bash
# Live dashboard for a running instance
afl-whatsup findings/uplane

# Plot data over time
afl-plot findings/uplane/main plot_dir && open plot_dir/index.html
```

---

## Triaging crashes

Crashes are written to `findings/<target>/main/crashes/`.  To reproduce a
crash outside of AFL++:

```bash
./build_fuzz/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer \
    findings/uplane/main/crashes/id:000000,*
```

To minimise a crashing input to its smallest reproducing form:

```bash
afl-tmin \
    -i findings/uplane/main/crashes/id:000000,* \
    -o minimised_crash \
    -- ./build_fuzz/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer @@
```

ASAN will print a full stack trace and memory error report when the crash is
reproduced.  Pipe through `llvm-symbolizer` if symbol names are missing:

```bash
ASAN_SYMBOLIZER_PATH=$(which llvm-symbolizer) \
    ./build_fuzz/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer \
    findings/uplane/main/crashes/id:000000,*
```

---

## Updating the corpus

After a fuzzing run, merge the newly discovered inputs back into the seed
corpus so future runs start with a richer base:

```bash
# Merge all queue entries into the seed corpus (deduplicates automatically)
afl-cmin \
    -i findings/uplane/main/queue \
    -o tests/fuzz/ofh/corpus/uplane \
    -- ./build_fuzz/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer @@
```

Commit the updated corpus alongside code changes.

---

## Docker

A `Dockerfile` and `run_fuzzers.sh` entrypoint are provided in `tests/fuzz/`
so the entire build-and-fuzz workflow runs in an isolated, reproducible
container without any local tool installation beyond Docker itself.

### Building the image

Run from the **repository root** (the build context must contain the full
source tree):

```bash
docker build -f tests/fuzz/Dockerfile -t ocudu-fuzz .
```

What the build does:

1. Installs all system dependencies (AFL++, Clang, LLVM, cmake, the three
   mandatory project libraries).
2. Configures a minimal CMake build with `afl-clang-fast++` and ASAN, with
   all optional subsystems disabled.
3. Compiles only the three OFH fuzz binaries.
4. Generates the seed corpus via `gen_corpus.py`.
5. Sets `run_fuzzers.sh` as the container entrypoint.

### Running the container

Bind-mount a host directory to `/findings` so AFL++ output is written there
and persists after the container exits:

```bash
mkdir -p findings
docker run --rm \
    -v "$(pwd)/findings:/findings" \
    ocudu-fuzz
```

By default all three fuzzers run for **2 hours each**.  Override with
environment variables:

| Variable | Default | Description |
|---|---|---|
| `FUZZ_TIMEOUT_EACH` | `7200` | Seconds to run each fuzzer |
| `FUZZ_OUTPUT_DIR` | `/findings` | AFL++ output root inside the container |
| `FUZZ_CORPUS_DIR` | `/corpus` | Seed corpus root inside the container |
| `FUZZ_TARGETS` | _(all three)_ | Space-separated list of targets to run |

```bash
# Run only the U-Plane fuzzer for 30 minutes
docker run --rm \
    -v "$(pwd)/findings:/findings" \
    -e FUZZ_TIMEOUT_EACH=1800 \
    -e FUZZ_TARGETS="ofh_uplane_decoder_fuzzer" \
    ocudu-fuzz
```

For better crash detection, run with `--privileged` so the container can set
`/proc/sys/kernel/core_pattern`:

```bash
docker run --rm --privileged \
    -v "$(pwd)/findings:/findings" \
    ocudu-fuzz
```

### Output layout

After the run, `findings/` on the host contains:

```
findings/
├── ofh_uplane_decoder_fuzzer.log   afl-fuzz stdout for this target
├── ofh_ecpri_decoder_fuzzer.log
├── ofh_vlan_frame_decoder_fuzzer.log
├── uplane/                         AFL++ output directory
│   ├── crashes/                    inputs that caused a crash
│   ├── hangs/                      inputs that caused a hang
│   ├── queue/                      all corpus entries found
│   └── fuzzer_stats                run statistics
├── ecpri/
└── vlan/
```

---

## CI integration

### Using the Docker image in GitLab CI

The recommended approach for CI is to build the Docker image once (e.g. in a
scheduled pipeline or when the fuzz sources change) and then reference it in
a dedicated weekly fuzz job.

```yaml
# .gitlab-ci.yml excerpt

variables:
  FUZZ_IMAGE: $CI_REGISTRY_IMAGE/ocudu-fuzz:latest

# Build and push the fuzz image when the fuzz sources change.
build-fuzz-image:
  stage: build
  image: docker:latest
  services:
    - docker:dind
  rules:
    - changes:
        - tests/fuzz/**/*
        - lib/ofh/**/*
        - lib/asn1/ngap/**/*
        - lib/ngap/**/*
  script:
    - docker login -u $CI_REGISTRY_USER -p $CI_REGISTRY_PASSWORD $CI_REGISTRY
    - docker build -f tests/fuzz/Dockerfile -t $FUZZ_IMAGE .
    - docker push $FUZZ_IMAGE

# Weekly scheduled fuzz run – ~6 h total (2 h per target).
fuzz-ofh:
  stage: test
  image: $FUZZ_IMAGE
  rules:
    - if: $CI_PIPELINE_SOURCE == "schedule"
  variables:
    FUZZ_TIMEOUT_EACH: "7200"
    FUZZ_OUTPUT_DIR: "$CI_PROJECT_DIR/findings"
  script:
    - /usr/local/bin/run_fuzzers.sh
  artifacts:
    when: always          # collect findings even when the job fails (crash found)
    paths:
      - findings/
    expire_in: 4 weeks
```

> **Tip:** Create a [GitLab scheduled pipeline](https://docs.gitlab.com/ee/ci/pipelines/schedules.html)
> to trigger `fuzz-ofh` once a week automatically.

### Running `run_fuzzers.sh` directly on the host

If AFL++ is already installed, `run_fuzzers.sh` can be used without Docker.
Set `FUZZ_CORPUS_DIR` to point at the in-tree corpus and build the targets
first:

```bash
# Build
cmake -B build_fuzz -DENABLE_FUZZTESTS=ON -DENABLE_ASAN=ON \
    -DCMAKE_CXX_COMPILER=afl-clang-fast++ -DCMAKE_BUILD_TYPE=Debug .
cmake --build build_fuzz --target \
    ofh_uplane_decoder_fuzzer ofh_ecpri_decoder_fuzzer ofh_vlan_frame_decoder_fuzzer \
    ngap_pdu_decoder_fuzzer ngap_cu_cp_fuzzer

# Copy binaries to PATH
sudo cp build_fuzz/tests/fuzz/ofh/ofh_*_fuzzer /usr/local/bin/

# Run
FUZZ_CORPUS_DIR=tests/fuzz/ofh/corpus \
FUZZ_OUTPUT_DIR=findings \
FUZZ_TIMEOUT_EACH=7200 \
    tests/fuzz/run_fuzzers.sh
```
