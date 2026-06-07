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
        "$(_pkg_ver curl)" "$(_pkg_ver apt-transport-https)" "$(_pkg_ver ca-certificates)" "$(_pkg_ver xz-utils)"
        "$(_pkg_ver python3-pip)" "$(_pkg_ver ninja-build)" "$(_pkg_ver g++)" "$(_pkg_ver build-essential)" "$(_pkg_ver pkg-config)" "$(_pkg_ver libnuma-dev)" "$(_pkg_ver libfdt-dev)" "$(_pkg_ver pciutils)"
    )
    local -a extra_pkgs=(
        "$(_pkg_ver libatomic1)" "$(_pkg_ver iproute2)"
    )
    local -a run_pkgs=(
        "$(_pkg_ver python3-pip)" "$(_pkg_ver libnuma-dev)" "$(_pkg_ver pciutils)" "$(_pkg_ver libfdt-dev)" "$(_pkg_ver libatomic1)" "$(_pkg_ver iproute2)"
    )
    local -a pip_build_pkgs=("$(_pip_ver meson)" "$(_pip_ver pyelftools)")
    local -a pip_run_pkgs=("$(_pip_ver pyelftools)")

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
        "$(_pkg_ver curl)" "$(_pkg_ver ca-certificates)" "$(_pkg_ver xz)" "$(_pkg_ver ninja-build)" "$(_pkg_ver make)" "$(_pkg_ver numactl-devel)" "$(_pkg_ver libfdt-devel)" "$(_pkg_ver pciutils)" "$(_pkg_ver python3-pyelftools)" "$(_pkg_ver meson)"
    )
    local -a extra_pkgs=(
        "$(_pkg_ver libatomic)" "$(_pkg_ver iproute)"
    )
    local -a run_pkgs=(
        "$(_pkg_ver numactl-libs)" "$(_pkg_ver pciutils)" "$(_pkg_ver libfdt)" "$(_pkg_ver libatomic)" "$(_pkg_ver iproute)"
    )
    local -a pip_build_pkgs=()
    local -a pip_run_pkgs=()

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

}

install_dpdk_dependencies_arch() {
    local mode="${1:?}"
    local -a pkgs=()
    local -a pip_pkgs=()

    local -a build_pkgs=(
        "$(_pkg_ver curl)" "$(_pkg_ver ca-certificates)" "$(_pkg_ver xz)"
        "$(_pkg_ver python-pip)" "$(_pkg_ver ninja)" "$(_pkg_ver base-devel)" "$(_pkg_ver pkgconf)" "$(_pkg_ver numactl)" "$(_pkg_ver dtc)" "$(_pkg_ver pciutils)"
    )
    local -a extra_pkgs=(
        "$(_pkg_ver iproute2)"
    )
    local -a run_pkgs=(
        "$(_pkg_ver python-pip)" "$(_pkg_ver numactl)" "$(_pkg_ver dtc)" "$(_pkg_ver pciutils)" "$(_pkg_ver iproute2)"
    )
    local -a pip_build_pkgs=("$(_pip_ver meson)" "$(_pip_ver pyelftools)")
    local -a pip_run_pkgs=("$(_pip_ver pyelftools)")

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
        pip3 install "${pip_pkgs[@]}" || pip3 install --break-system-packages "${pip_pkgs[@]}"
    fi
}

install_dpdk_dependencies_rhel() {
    local mode="${1:?}"
    local -a pkgs=()
    local -a pip_pkgs=()

    local -a build_pkgs=(
        "$(_pkg_ver curl)" "$(_pkg_ver ca-certificates)" "$(_pkg_ver xz)"
        "$(_pkg_ver python3-pip)" "$(_pkg_ver ninja-build)" "$(_pkg_ver gcc)" "$(_pkg_ver gcc-c++)" "$(_pkg_ver make)" "$(_pkg_ver pkgconf-pkg-config)" "$(_pkg_ver numactl-devel)" "$(_pkg_ver libfdt-devel)" "$(_pkg_ver pciutils)"
    )
    local -a extra_pkgs=(
        "$(_pkg_ver libatomic)" "$(_pkg_ver iproute)"
    )
    local -a run_pkgs=(
        "$(_pkg_ver python3-pip)" "$(_pkg_ver numactl-devel)" "$(_pkg_ver pciutils)" "$(_pkg_ver libfdt)" "$(_pkg_ver libatomic)" "$(_pkg_ver iproute)"
    )
    local -a pip_build_pkgs=("$(_pip_ver meson)" "$(_pip_ver pyelftools)")
    local -a pip_run_pkgs=("$(_pip_ver pyelftools)")

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
