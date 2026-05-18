#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set -e

install_dpdk_dependencies_debian_ubuntu() {
    local mode="${1:?}"
    local -x DEBIAN_FRONTEND=noninteractive
    local -a pkgs=()
    local -a pip_pkgs=()

    local -a build_pkgs=(
        curl apt-transport-https ca-certificates xz-utils
        python3-pip ninja-build g++ build-essential pkg-config libnuma-dev libfdt-dev pciutils
    )
    local -a extra_pkgs=(
        libatomic1 iproute2
    )
    local -a run_pkgs=(
        python3-pip libnuma-dev pciutils libfdt-dev libatomic1 iproute2
    )
    local -a pip_build_pkgs=(meson pyelftools)
    local -a pip_run_pkgs=(pyelftools)

    case "$mode" in
        all)
            pkgs+=( "${build_pkgs[@]}" "${extra_pkgs[@]}" )
            pip_pkgs+=( "${pip_build_pkgs[@]}" )
            ;;
        build)
            pkgs+=( "${build_pkgs[@]}" )
            pip_pkgs+=( "${pip_build_pkgs[@]}" )
            ;;
        run)
            pkgs+=( "${run_pkgs[@]}" )
            pip_pkgs+=( "${pip_run_pkgs[@]}" )
            ;;
        *)
            echo >&2 "Unsupported mode: $mode"
            exit 1
            ;;
    esac

    if ((${#pkgs[@]})); then
        apt-get update
        apt-get install -y --no-install-recommends "${pkgs[@]}"
        apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/*
    fi

    if ((${#pip_pkgs[@]})); then
        pip3 install "${pip_pkgs[@]}" || pip3 install --break-system-packages "${pip_pkgs[@]}"
    fi
}

install_dpdk_dependencies_fedora() {
    local mode="${1:?}"
    local -a pkgs=()
    local -a pip_pkgs=()

    local -a build_pkgs=(
        curl ca-certificates xz
        python3-pip ninja-build gcc gcc-c++ make pkgconf-pkg-config numactl-devel libfdt-devel pciutils
    )
    local -a extra_pkgs=(
        libatomic iproute
    )
    local -a run_pkgs=(
        python3-pip numactl-devel pciutils libfdt libatomic iproute
    )
    local -a pip_build_pkgs=(meson pyelftools)
    local -a pip_run_pkgs=(pyelftools)

    case "$mode" in
        all)
            pkgs+=( "${build_pkgs[@]}" "${extra_pkgs[@]}" )
            pip_pkgs+=( "${pip_build_pkgs[@]}" )
            ;;
        build)
            pkgs+=( "${build_pkgs[@]}" )
            pip_pkgs+=( "${pip_build_pkgs[@]}" )
            ;;
        run)
            pkgs+=( "${run_pkgs[@]}" )
            pip_pkgs+=( "${pip_run_pkgs[@]}" )
            ;;
        *)
            echo >&2 "Unsupported mode: $mode"
            exit 1
            ;;
    esac

    if ((${#pkgs[@]})); then
        dnf -y install "${pkgs[@]}"
        dnf clean all
    fi

    if ((${#pip_pkgs[@]})); then
        pip3 install "${pip_pkgs[@]}" --break-system-packages
    fi
}

install_dpdk_dependencies_arch() {
    local mode="${1:?}"
    local -a pkgs=()
    local -a pip_pkgs=()

    local -a build_pkgs=(
        curl ca-certificates xz
        python-pip ninja base-devel pkgconf numactl dtc pciutils
    )
    local -a extra_pkgs=(
        iproute2
    )
    local -a run_pkgs=(
        python-pip numactl dtc pciutils iproute2
    )
    local -a pip_build_pkgs=(meson pyelftools)
    local -a pip_run_pkgs=(pyelftools)

    case "$mode" in
        all)
            pkgs+=( "${build_pkgs[@]}" "${extra_pkgs[@]}" )
            pip_pkgs+=( "${pip_build_pkgs[@]}" )
            ;;
        build)
            pkgs+=( "${build_pkgs[@]}" )
            pip_pkgs+=( "${pip_build_pkgs[@]}" )
            ;;
        run)
            pkgs+=( "${run_pkgs[@]}" )
            pip_pkgs+=( "${pip_run_pkgs[@]}" )
            ;;
        *)
            echo >&2 "Unsupported mode: $mode"
            exit 1
            ;;
    esac

    if ((${#pkgs[@]})); then
        pacman -Syu --noconfirm "${pkgs[@]}"
        pacman -Scc --noconfirm
    fi

    if ((${#pip_pkgs[@]})); then
        pip3 install "${pip_pkgs[@]}" --break-system-packages
    fi
}

main() {

    if [ $# != 0 ] && [ $# != 1 ]; then
        echo >&2 "Illegal number of parameters"
        echo >&2 "Run like this: \"./install_dpdk_dependencies.sh [<mode>]\" where mode could be: build, run and all"
        echo >&2 "If mode is not specified, all dependencies will be installed"
        exit 1
    fi

    local mode="${1:-all}"

    # shellcheck source=/dev/null
    . /etc/os-release

    case "$ID" in
        debian|ubuntu)
            install_dpdk_dependencies_debian_ubuntu "$mode"
            ;;
        fedora)
            install_dpdk_dependencies_fedora "$mode"
            ;;
        arch)
            install_dpdk_dependencies_arch "$mode"
            ;;
        *)
            echo "OS $ID not supported"
            exit 1
            ;;
    esac

}

main "$@"
