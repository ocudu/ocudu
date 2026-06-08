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

install_rohc_dependencies_debian_ubuntu() {
    local mode="${1:?}"
    local -x DEBIAN_FRONTEND=noninteractive
    local -a pkgs=()

    local -a build_pkgs=(
        curl ca-certificates build-essential xz-utils autotools-dev automake libtool libpcap-dev libcmocka-dev
    )
    local -a run_pkgs=()

    case "$mode" in
        all|build)
            pkgs+=( "${build_pkgs[@]}" )
            ;;
        run)
            pkgs+=( "${run_pkgs[@]}" )
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
        apt-get clean && rm -rf /var/lib/apt/lists/*
    fi
}

install_rohc_dependencies_fedora() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(
        curl ca-certificates gcc gcc-c++ make which xz
        autoconf automake libtool libpcap-devel libcmocka-devel
    )
    local -a run_pkgs=()

    case "$mode" in
        all|build)
            pkgs+=( "${build_pkgs[@]}" )
            ;;
        run)
            pkgs+=( "${run_pkgs[@]}" )
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

    if [[ "$mode" != "run" ]]; then
        local tool ver
        for tool in aclocal automake; do
            if ! command -v "${tool}" >/dev/null 2>&1; then
                ver=$(compgen -G "/usr/bin/${tool}-*" | head -1)
                if [[ -n "$ver" ]]; then
                    ln -sf "${ver}" "/usr/bin/${tool}"
                fi
            fi
        done
    fi
}

install_rohc_dependencies_arch() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(
        curl ca-certificates base-devel which xz autoconf automake libtool libpcap cmocka
    )
    local -a run_pkgs=()

    case "$mode" in
        all|build)
            pkgs+=( "${build_pkgs[@]}" )
            ;;
        run)
            pkgs+=( "${run_pkgs[@]}" )
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
}

install_rohc_dependencies_rhel() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(
        ca-certificates gcc gcc-c++ make which xz autoconf automake libtool libpcap-devel libcmocka-devel
    )
    local -a run_pkgs=()

    case "$mode" in
        all|build)
            pkgs+=( "${build_pkgs[@]}" )
            ;;
        run)
            pkgs+=( "${run_pkgs[@]}" )
            ;;
        *)
            echo >&2 "Unsupported mode: $mode"
            exit 1
            ;;
    esac

    # Some deps require RHEL CRB (subscription-only)
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

    if [[ "$mode" != "run" ]]; then
        if ! command -v aclocal >/dev/null 2>&1 && command -v aclocal-1.18 >/dev/null 2>&1; then
            ln -sf "$(command -v aclocal-1.18)" /usr/bin/aclocal
        fi
        if ! command -v automake >/dev/null 2>&1 && command -v automake-1.18 >/dev/null 2>&1; then
            ln -sf "$(command -v automake-1.18)" /usr/bin/automake
        fi
    fi
}

main() {

    if [ $# != 0 ] && [ $# != 1 ]; then
        echo >&2 "Illegal number of parameters"
        echo >&2 "Run like this: \"./install_rohc_dependencies.sh [<mode>]\" where mode could be: build, run and all"
        echo >&2 "If mode is not specified, all dependencies will be installed"
        exit 1
    fi

    local mode="${1:-all}"

    # shellcheck source=/dev/null
    . /etc/os-release

    case "$ID" in
        debian|ubuntu)
            install_rohc_dependencies_debian_ubuntu "$mode"
            ;;
        fedora)
            install_rohc_dependencies_fedora "$mode"
            ;;
        rhel)
            install_rohc_dependencies_rhel "$mode"
            ;;
        arch)
            install_rohc_dependencies_arch "$mode"
            ;;
        *)
            echo "OS $ID not supported"
            exit 1
            ;;
    esac

}

main "$@"
