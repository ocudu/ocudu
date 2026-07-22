#!/usr/bin/env bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

# Entrypoint script for the OFH fuzz Docker container.
# Also suitable as a standalone CI script when AFL++ is installed on the host.
#
# Environment variables
# ---------------------
#   FUZZ_TIMEOUT_EACH   Seconds to run each fuzzer (default: 7200 = 2 h)
#   FUZZ_OUTPUT_DIR     Directory for AFL++ output (default: /findings)
#   FUZZ_CORPUS_DIR     Directory containing seed corpora (default: /corpus)
#   FUZZ_TARGETS        Space-separated list of targets to run (default: all)
#                       Valid values: ofh_uplane_decoder_fuzzer
#                                     ofh_ecpri_decoder_fuzzer
#                                     ofh_vlan_frame_decoder_fuzzer
#                                     ngap_pdu_decoder_fuzzer
#                                     ngap_cu_cp_fuzzer
#
# Exit status
# -----------
#   0  no crashes found
#   1  one or more crashes found (artifacts in $FUZZ_OUTPUT_DIR)

set -euo pipefail

TIMEOUT_EACH=${FUZZ_TIMEOUT_EACH:-7200}
OUTPUT_DIR=${FUZZ_OUTPUT_DIR:-/findings}
CORPUS_DIR=${FUZZ_CORPUS_DIR:-/corpus}

# All available targets mapped to their corpus sub-directory.
declare -A ALL_TARGETS=(
    ["ofh_uplane_decoder_fuzzer"]="uplane"
    ["ofh_ecpri_decoder_fuzzer"]="ecpri"
    ["ofh_vlan_frame_decoder_fuzzer"]="vlan"
    ["ngap_pdu_decoder_fuzzer"]="ngap"
    ["ngap_cu_cp_fuzzer"]="ngap_cu_cp"
)

# Optionally restrict which targets run.
if [[ -n "${FUZZ_TARGETS:-}" ]]; then
    declare -A TARGETS
    for t in ${FUZZ_TARGETS}; do
        if [[ -v ALL_TARGETS[$t] ]]; then
            TARGETS[$t]="${ALL_TARGETS[$t]}"
        else
            echo "WARNING: unknown target '${t}', skipping" >&2
        fi
    done
else
    declare -A TARGETS
    for k in "${!ALL_TARGETS[@]}"; do TARGETS[$k]="${ALL_TARGETS[$k]}"; done
fi

if [[ ${#TARGETS[@]} -eq 0 ]]; then
    echo "ERROR: no valid fuzz targets selected" >&2
    exit 1
fi

mkdir -p "${OUTPUT_DIR}"

# ---------------------------------------------------------------------------
# AFL++ tuning for container / CI environments
# ---------------------------------------------------------------------------

# Disable the ncurses status UI – output goes to the log file instead.
export AFL_NO_UI=1

# Skip CPU frequency scaling check (containers typically lack permission).
export AFL_SKIP_CPUFREQ=1

# Allow longer fork-server init (useful with ASAN instrumentation).
export AFL_FORKSRV_INIT_TMOUT=30000

# Fix core dump pattern for accurate crash detection.  Falls back gracefully
# if the container lacks the required privilege.
if echo core > /proc/sys/kernel/core_pattern 2>/dev/null; then
    echo "core_pattern set to 'core'"
else
    echo "WARNING: could not set core_pattern (run with --privileged for full crash detection)"
    export AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1
fi

# ---------------------------------------------------------------------------
# Banner
# ---------------------------------------------------------------------------
echo "============================================================"
echo " OFH fuzz run  $(date -u '+%Y-%m-%d %H:%M:%S UTC')"
printf "   targets        : %s\n" "${!TARGETS[*]}"
printf "   timeout/target : %ss\n" "${TIMEOUT_EACH}"
printf "   output dir     : %s\n" "${OUTPUT_DIR}"
printf "   corpus dir     : %s\n" "${CORPUS_DIR}"
echo "============================================================"

# ---------------------------------------------------------------------------
# Run fuzzers sequentially
# ---------------------------------------------------------------------------
CRASH_FOUND=0
TOTAL_EXECS=0

for binary in "${!TARGETS[@]}"; do
    subdir="${TARGETS[$binary]}"
    corpus="${CORPUS_DIR}/${subdir}"
    outdir="${OUTPUT_DIR}/${subdir}"
    logfile="${OUTPUT_DIR}/${binary}.log"

    mkdir -p "${outdir}"

    # Use resume mode if a prior queue exists so accumulated corpus carries
    # over between CI runs.  On first run (no prior output) start from the
    # seed corpus instead.
    if [[ -d "${outdir}/queue" ]]; then
        afl_input="-i -"
        mode="resume"
    else
        afl_input="-i ${corpus}"
        mode="fresh start"
    fi

    echo ""
    echo "--- [$(date -u '+%H:%M:%S')] Starting ${binary} ---"
    echo "    corpus : ${corpus}"
    echo "    output : ${outdir}"
    echo "    log    : ${logfile}"
    echo "    mode   : ${mode}"

    timeout "${TIMEOUT_EACH}" \
        afl-fuzz \
            ${afl_input} \
            -o "${outdir}" \
            -- "${binary}" @@ \
        2>&1 | tee "${logfile}" || true

    echo "--- [$(date -u '+%H:%M:%S')] ${binary} finished ---"

    # Collect stats from the fuzzer_stats file if present.
    stats_file="${outdir}/fuzzer_stats"
    if [[ -f "${stats_file}" ]]; then
        execs=$(grep "^execs_done" "${stats_file}" | awk '{print $3}')
        paths=$(grep "^corpus_count" "${stats_file}" | awk '{print $3}')
        echo "    execs: ${execs:-?}  corpus paths: ${paths:-?}"
        TOTAL_EXECS=$(( TOTAL_EXECS + ${execs:-0} ))
    fi

    # Check for crashes.
    if compgen -G "${outdir}/crashes/id:*" > /dev/null 2>&1; then
        echo "    CRASH found in ${binary} – see ${outdir}/crashes/" >&2
        CRASH_FOUND=1
    fi

    # Check for hangs.
    if compgen -G "${outdir}/hangs/id:*" > /dev/null 2>&1; then
        echo "    HANG found in ${binary} – see ${outdir}/hangs/" >&2
    fi
done

# ---------------------------------------------------------------------------
# Summary
# ---------------------------------------------------------------------------
echo ""
echo "============================================================"
printf " Total executions across all targets : %s\n" "${TOTAL_EXECS}"
if [[ "${CRASH_FOUND}" -eq 1 ]]; then
    echo " Result : CRASHES FOUND – inspect ${OUTPUT_DIR} for details"
    echo "============================================================"
    exit 1
else
    echo " Result : no crashes found"
    echo "============================================================"
    exit 0
fi
