#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set -e

# shellcheck source=common.sh
. "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/common.sh"

install_dpdk_dependencies_debian_ubuntu() {
    local mode="${1:?}"
    local -x DEBIAN_FRONTEND=noninteractive
    local -a pkgs=()
    local -a pip_pkgs=()

    local -a build_pkgs=(
        curl apt-transport-https ca-certificates xz-utils
        python3-pip ninja-build g++ build-essential pkg-config libnuma-dev libfdt-dev pciutils
        libibverbs-dev
    )
    local -a extra_pkgs=(
        libatomic1 iproute2
    )
    local -a run_pkgs=(
        python3-pip libnuma-dev pciutils libfdt-dev libatomic1 iproute2
        libibverbs1 ibverbs-providers
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

    install_ubuntu_pkgs "${pkgs[@]}"

    install_pip_pkgs "${pip_pkgs[@]}"
}

install_dpdk_dependencies_fedora() {
    local mode="${1:?}"
    local -a pkgs=()
    local -a pip_pkgs=()

    local -a build_pkgs=(
        curl ca-certificates xz
        python3-pip ninja-build gcc gcc-c++ make pkgconf-pkg-config numactl-devel libfdt-devel pciutils
        rdma-core-devel
    )
    local -a extra_pkgs=(
        libatomic iproute
    )
    local -a run_pkgs=(
        python3-pip numactl-libs pciutils libfdt libatomic iproute
        libibverbs
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

    install_fedora_pkgs "${pkgs[@]}"
    install_pip_pkgs --break-system-packages "${pip_pkgs[@]}"
}

install_dpdk_dependencies_arch() {
    local mode="${1:?}"
    local -a pkgs=()
    local -a pip_pkgs=()

    local -a build_pkgs=(
        curl ca-certificates xz
        python-pip ninja base-devel pkgconf numactl dtc pciutils
        rdma-core
    )
    local -a extra_pkgs=(
        iproute2
    )
    local -a run_pkgs=(
        python-pip numactl dtc pciutils iproute2
        rdma-core
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

    install_arch_pkgs "${pkgs[@]}"
    install_pip_pkgs --break-system-packages "${pip_pkgs[@]}"
}

install_dpdk_dependencies_rhel() {
    local mode="${1:?}"
    local -a pkgs=()
    local -a pip_pkgs=()

    local -a build_pkgs=(
        ca-certificates xz
        python3-pip ninja-build gcc gcc-c++ make pkgconf-pkg-config numactl-devel libfdt-devel pciutils
        rdma-core-devel
    )
    local -a extra_pkgs=(
        libatomic iproute
    )
    local -a run_pkgs=(
        python3-pip numactl-libs pciutils libfdt libatomic iproute
        libibverbs
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

    # libfdt-devel is in RHEL CRB (subscription-only)
    if [[ "$mode" != "run" ]]; then
        dnf -y install dnf-plugins-core
        dnf config-manager --enable "codeready-builder-for-rhel-9-$(uname -m)-rpms" 2>/dev/null || true
    fi

    install_rhel_pkgs "${pkgs[@]}"
    install_pip_pkgs "${pip_pkgs[@]}"
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
        rhel)
            install_dpdk_dependencies_rhel "$mode"
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
