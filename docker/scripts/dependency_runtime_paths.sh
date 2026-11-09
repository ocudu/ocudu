# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

# Sets runtime paths for ROHC and the selected UHD, DPDK and ArmPL prefixes.
setup_dependency_runtime_paths() {
    local rohc_dir="$1"
    local uhd_dir="${2:-}"
    local dpdk_dir="${3:-}"
    local armpl_dir="${4:-}"
    local directory
    local -a directories=("${rohc_dir}/lib")
    if [[ -n "$uhd_dir" ]]; then
        directories+=("${uhd_dir}/lib" "${uhd_dir}/lib64")
    fi
    if [[ -n "$dpdk_dir" ]]; then
        directories+=("${dpdk_dir}/lib/aarch64-linux-gnu" "${dpdk_dir}/lib/x86_64-linux-gnu"
                      "${dpdk_dir}/lib" "${dpdk_dir}/lib64")
    fi
    if [[ -n "$armpl_dir" ]]; then
        directories+=("${armpl_dir}/lib")
    fi
    for directory in "${directories[@]}"; do
        case ":${LD_LIBRARY_PATH:-}:" in
            *":${directory}:"*) ;;
            *) LD_LIBRARY_PATH="${directory}${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" ;;
        esac
    done
    export LD_LIBRARY_PATH
}
