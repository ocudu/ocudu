#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set -e

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
        apt-get update
        apt-get install -y --no-install-recommends "${pkgs[@]}"
        apt-get clean && rm -rf /var/lib/apt/lists/*
    fi
}

install_rohc_dependencies_fedora() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(
        curl ca-certificates gcc gcc-c++ make which xz autoconf automake libtool libpcap-devel libcmocka-devel
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
        dnf -y install "${pkgs[@]}"
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
        pacman -Syu --noconfirm "${pkgs[@]}"
        pacman -Scc --noconfirm
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
