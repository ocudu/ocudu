#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set -e

install_uhd_dependencies_debian_ubuntu() {
    local mode="${1:?}"
    local -x DEBIAN_FRONTEND=noninteractive
    local -a pkgs=()

    local -a build_pkgs=(
        curl apt-transport-https ca-certificates xz-utils
        cmake build-essential pkg-config
        libboost-all-dev libusb-1.0-0-dev
        python3-mako python3-numpy python3-setuptools python3-requests
    )
    local -a run_pkgs=(
        inetutils-tools libboost-all-dev libncurses5-dev libusb-1.0-0 libusb-1.0-0-dev
        libusb-dev python3-dev python3-requests
    )
    local -a extra_pkgs=(
        inetutils-tools libncurses5-dev libusb-1.0-0 libusb-1.0-0-dev
        libusb-dev python3-dev
    )
    local -a optional_pkgs=()

    case "$mode" in
        all)
            pkgs+=( "${build_pkgs[@]}" "${extra_pkgs[@]}" )
            optional_pkgs+=(cpufrequtils)
            ;;
        build)
            pkgs+=( "${build_pkgs[@]}" )
            ;;
        run)
            pkgs+=( "${run_pkgs[@]}" )
            optional_pkgs+=(cpufrequtils)
            ;;
        *)
            echo >&2 "Unsupported mode: $mode"
            exit 1
            ;;
    esac

    if ((${#pkgs[@]})); then
        apt-get update
        for pkg in "${optional_pkgs[@]}"; do
            if apt-cache policy "$pkg" | awk '$1 == "Candidate:" && $2 != "(none)" { found = 1 } END { exit !found }'; then
                pkgs+=("$pkg")
            fi
        done
        apt-get install -y --no-install-recommends "${pkgs[@]}"
        apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/*
    fi

    if [[ "$mode" == "all" || "$mode" == "run" ]]; then
        uhd_images_downloader
    fi
}

install_uhd_dependencies_fedora() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(
        curl ca-certificates xz
        cmake gcc gcc-c++ make pkgconf-pkg-config
        boost-devel libusb1-devel
        python3-mako python3-numpy python3-setuptools python3-requests
    )
    local -a run_pkgs=(
        boost-devel libusb1 libusb1-devel python3-devel python3-requests
    )
    local -a extra_pkgs=(
        boost-devel libusb1 libusb1-devel python3-devel
    )

    case "$mode" in
        all)
            pkgs+=( "${build_pkgs[@]}" "${extra_pkgs[@]}" )
            ;;
        build)
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

    if [[ "$mode" == "all" || "$mode" == "run" ]]; then
        uhd_images_downloader
    fi
}

install_uhd_dependencies_arch() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(
        curl ca-certificates xz
        cmake base-devel pkgconf
        boost boost-libs libusb
        python-mako python-numpy python-setuptools python-requests
    )
    local -a run_pkgs=(
        boost boost-libs libusb python python-requests
    )
    local -a extra_pkgs=(
        boost boost-libs libusb python
    )

    case "$mode" in
        all)
            pkgs+=( "${build_pkgs[@]}" "${extra_pkgs[@]}" )
            ;;
        build)
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

    if [[ "$mode" == "all" || "$mode" == "run" ]]; then
        uhd_images_downloader
    fi
}

main() {

    if [ $# != 0 ] && [ $# != 1 ]; then
        echo >&2 "Illegal number of parameters"
        echo >&2 "Run like this: \"./install_uhd_dependencies.sh [<mode>]\" where mode could be: build, run and all"
        echo >&2 "If mode is not specified, all dependencies will be installed"
        exit 1
    fi

    local mode="${1:-all}"

    # shellcheck source=/dev/null
    . /etc/os-release

    case "$ID" in
        debian|ubuntu)
            install_uhd_dependencies_debian_ubuntu "$mode"
            ;;
        fedora)
            install_uhd_dependencies_fedora "$mode"
            ;;
        arch)
            install_uhd_dependencies_arch "$mode"
            ;;
        *)
            echo "OS $ID not supported"
            exit 1
            ;;
    esac

}

main "$@"
