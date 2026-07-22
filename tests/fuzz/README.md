# Fuzz Test Harnesses

Coverage-guided fuzz testing for the OFH packet-parsing stack and the NGAP/CU-CP
receive path, targeting [AFL++](https://github.com/AFLplusplus/AFLplusplus). The
harnesses use the `LLVMFuzzerTestOneInput` interface so they also run unmodified
under [libFuzzer](https://llvm.org/docs/LibFuzzer.html) and are compatible with
[OSS-Fuzz](https://github.com/google/oss-fuzz).

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
AFL++ installed.  Replace the compiler wrappers with plain `clang++`:

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
```

Each script writes binary seed files into the corpus sub-directories of its
own layer.

### OSS-Fuzz zip format

Both scripts accept an optional flag to write seeds as a zip file suitable
for the `$OUT/<target>_seed_corpus.zip` convention expected by OSS-Fuzz:

```bash
# NGAP: one zip covers both NGAP targets
python3 tests/fuzz/ngap/gen_corpus.py \
    --zip $OUT/ngap_pdu_decoder_fuzzer_seed_corpus.zip
cp $OUT/ngap_pdu_decoder_fuzzer_seed_corpus.zip \
   $OUT/ngap_cu_cp_fuzzer_seed_corpus.zip

# OFH: one zip per target
python3 tests/fuzz/ofh/gen_corpus.py --zip-dir $OUT/
```

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
afl-fuzz \
    -i tests/fuzz/ngap/corpus/ngap \
    -o findings/ngap \
    -- ./build_fuzz/tests/fuzz/ngap/ngap_pdu_decoder_fuzzer @@
```

### NGAP full-stack CU-CP fuzzer

```bash
# Recommended: AFL_FAST_CAL for higher throughput (CU-CP stays alive across iterations)
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

In parallel mode, AFL++ writes each instance's findings to a named
subdirectory: `findings/uplane/main/`, `findings/uplane/worker1/`, etc.

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

Crash inputs are written to `findings/<target>/crashes/` for single-instance
runs and to `findings/<target>/<instance>/crashes/` for parallel runs (e.g.
`findings/uplane/main/crashes/`).  To reproduce a crash:

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

ASAN prints a full stack trace when the crash is reproduced.  Pipe through
`llvm-symbolizer` if symbol names are missing:

```bash
ASAN_SYMBOLIZER_PATH=$(which llvm-symbolizer) \
    ./build_fuzz/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer \
    findings/uplane/crashes/id:000000,*
```

### Converting a crash to a unit-test literal

`crash_to_hex.py` converts AFL++ crash/hang input files into formats that can
be pasted directly into a C++ unit test:

```bash
# Print a C array literal for a single crash file
python3 tests/fuzz/crash_to_hex.py --format c \
    findings/uplane/crashes/id:000000,*

# Convert all crashes in a directory at once
python3 tests/fuzz/crash_to_hex.py --format c \
    findings/uplane/crashes/
```

Available formats: `hex` (default), `xxd` (annotated hex dump), `c` (C/C++
`uint8_t` array literal).  Add the resulting literal to the appropriate unit
test under `tests/unittests/` to prevent regressions.

---

## Updating the corpus

After a long local run, merge the newly discovered inputs back into the seed
corpus so future runs start with a richer base:

```bash
# Merge and deduplicate queue entries into the seed corpus
afl-cmin \
    -i findings/uplane/queue \
    -o tests/fuzz/ofh/corpus/uplane \
    -- ./build_fuzz/tests/fuzz/ofh/ofh_uplane_decoder_fuzzer @@
```

Commit the updated corpus alongside code changes.  In CI the corpus
accumulates automatically across weekly runs via AFL++ resume mode — see
[CI integration](#ci-integration) below.

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

1. Installs all system dependencies (AFL++, Clang, LLVM, cmake, and the
   mandatory project libraries).
2. Configures a minimal CMake build with `afl-clang-fast++` and ASAN, with
   all optional subsystems disabled.
3. Compiles all five fuzz binaries.
4. Generates the seed corpus via both `gen_corpus.py` scripts.
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

By default all five fuzzers run for **2 hours each**.  Override with
environment variables:

| Variable | Default | Description |
|---|---|---|
| `FUZZ_TIMEOUT_EACH` | `7200` | Seconds to run each fuzzer |
| `FUZZ_OUTPUT_DIR` | `/findings` | AFL++ output root inside the container |
| `FUZZ_CORPUS_DIR` | `/corpus` | Seed corpus root inside the container |
| `FUZZ_TARGETS` | _(all five)_ | Space-separated list of targets to run |

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

## CI integration

Two jobs are defined in `.gitlab-ci.yml` and run on a weekly scheduled
pipeline with the description **"Weekly fuzz"**:

- **`build-fuzz-image`** — builds the fuzz Docker image and pushes it to the
  project registry.
- **`fuzz`** — pulls the image and runs all targets.  The `findings/`
  directory is cached between pipeline runs so `run_fuzzers.sh` resumes from
  the accumulated AFL++ queue rather than restarting from the seed corpus each
  week.

To activate: create a [GitLab scheduled pipeline](https://docs.gitlab.com/ee/ci/pipelines/schedules.html)
on the default branch with the description `Weekly fuzz`.

### Relevant `.gitlab-ci.yml` excerpt

```yaml
build-fuzz-image:
  stage: .post
  image: docker:latest
  services:
    - docker:dind
  rules:
    - if: '$CI_PIPELINE_SOURCE == "schedule" && $CI_PIPELINE_SCHEDULE_DESCRIPTION =~ /Weekly fuzz/'
  script:
    - docker login -u $CI_REGISTRY_USER -p $CI_REGISTRY_PASSWORD $CI_REGISTRY
    - docker build -f tests/fuzz/Dockerfile -t $CI_REGISTRY_IMAGE/fuzz:latest .
    - docker push $CI_REGISTRY_IMAGE/fuzz:latest
  dependencies: []

fuzz:
  stage: .post
  image: $CI_REGISTRY_IMAGE/fuzz:latest
  needs: [build-fuzz-image]
  rules:
    - if: '$CI_PIPELINE_SOURCE == "schedule" && $CI_PIPELINE_SCHEDULE_DESCRIPTION =~ /Weekly fuzz/'
  variables:
    FUZZ_OUTPUT_DIR: $CI_PROJECT_DIR/findings
    FUZZ_TIMEOUT_EACH: "7200"
  cache:
    key: fuzz-corpus-$CI_DEFAULT_BRANCH
    paths:
      - findings/
  script:
    - /usr/local/bin/run_fuzzers.sh
  artifacts:
    when: always
    paths:
      - findings/
    expire_in: 4 weeks
  dependencies: []
```

### Corpus accumulation

`run_fuzzers.sh` detects a prior queue automatically: if
`findings/<target>/queue/` exists when a run starts, AFL++ resumes from it
(`-i -`); otherwise it starts from the seed corpus (`-i <corpus>`).  The
GitLab CI cache restores `findings/` at the start of each weekly job, so
coverage accumulates across runs without any manual intervention.

---

## OSS-Fuzz

The build is compatible with [OSS-Fuzz](https://github.com/google/oss-fuzz).
When `$LIB_FUZZING_ENGINE` is set, `tests/fuzz/CMakeLists.txt` uses it as the
link target instead of `-fsanitize=fuzzer`, and leaves compile flags empty so
OSS-Fuzz's own instrumentation (injected via `$CXXFLAGS`) takes precedence.
Seed corpus zips can be generated with the `--zip` / `--zip-dir` flags
described in [Seed corpus](#seed-corpus).
