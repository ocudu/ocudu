#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set -e

# Return "name=version" if PKG_VERSIONS contains an entry for the given package name,
# otherwise return the bare name. PKG_VERSIONS is a space-separated list of "name=version" pairs.
_pkg_ver() {
    local name="$1"
    local pair
    for pair in $PKG_VERSIONS; do
        case "$pair" in
            "${name}="*) echo "${pair}"; return ;;
        esac
    done
    echo "$name"
}

# Return "name==version" if PKG_VERSIONS contains an entry for the given pip package name,
# otherwise return the bare name. Uses pip's == version pin syntax.
_pip_ver() {
    local name="$1"
    local pair
    for pair in $PKG_VERSIONS; do
        case "$pair" in
            "${name}="*) echo "${name}==${pair#*=}"; return ;;
        esac
    done
    echo "$name"
}

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
        local -a versioned_pkgs=()
        for pkg in "${pkgs[@]}"; do versioned_pkgs+=("$(_pkg_ver "$pkg")"); done
        apt-get update
        apt-get install -y --no-install-recommends "${versioned_pkgs[@]}"
        apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/*
    fi

    if ((${#pip_pkgs[@]})); then
        local -a versioned_pip_pkgs=()
        for pkg in "${pip_pkgs[@]}"; do versioned_pip_pkgs+=("$(_pip_ver "$pkg")"); done
        pip3 install "${versioned_pip_pkgs[@]}" || pip3 install --break-system-packages "${versioned_pip_pkgs[@]}"
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
        python3-pip numactl-libs pciutils libfdt libatomic iproute
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
        local -a versioned_pkgs=()
        for pkg in "${pkgs[@]}"; do versioned_pkgs+=("$(_pkg_ver "$pkg")"); done
        dnf -y install "${versioned_pkgs[@]}"
        dnf clean all
    fi

    if ((${#pip_pkgs[@]})); then
        local -a versioned_pip_pkgs=()
        for pkg in "${pip_pkgs[@]}"; do versioned_pip_pkgs+=("$(_pip_ver "$pkg")"); done
        pip3 install "${versioned_pip_pkgs[@]}" --break-system-packages
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
        local -a versioned_pkgs=()
        for pkg in "${pkgs[@]}"; do versioned_pkgs+=("$(_pkg_ver "$pkg")"); done
        pacman -Syu --noconfirm "${versioned_pkgs[@]}"
        pacman -Scc --noconfirm
    fi

    if ((${#pip_pkgs[@]})); then
        local -a versioned_pip_pkgs=()
        for pkg in "${pip_pkgs[@]}"; do versioned_pip_pkgs+=("$(_pip_ver "$pkg")"); done
        pip3 install "${versioned_pip_pkgs[@]}" --break-system-packages
    fi
}

install_dpdk_dependencies_rhel() {
    local mode="${1:?}"
    local -a pkgs=()
    local -a pip_pkgs=()

    local -a build_pkgs=(
        ca-certificates xz
        python3-pip ninja-build gcc gcc-c++ make pkgconf-pkg-config numactl-devel libfdt-devel pciutils
    )
    local -a extra_pkgs=(
        libatomic iproute
    )
    local -a run_pkgs=(
        python3-pip numactl-libs pciutils libfdt libatomic iproute
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

    if ((${#pkgs[@]})); then
        local -a versioned_pkgs=()
        for pkg in "${pkgs[@]}"; do versioned_pkgs+=("$(_pkg_ver "$pkg")"); done
        dnf -y install "${versioned_pkgs[@]}"
        dnf clean all
    fi

    if ((${#pip_pkgs[@]})); then
        local -a versioned_pip_pkgs=()
        for pkg in "${pip_pkgs[@]}"; do versioned_pip_pkgs+=("$(_pip_ver "$pkg")"); done
        pip3 install "${versioned_pip_pkgs[@]}"
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
