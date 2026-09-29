#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set -e

# shellcheck source=common.sh
. "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/common.sh"

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

    # UHD binaries only link these Boost and USB runtime libraries. The image
    # downloader needs Python and requests, not headers. Boost package names
    # include the ABI version, which differs between Ubuntu releases.
    case "${ID:-}:${VERSION_ID:-}" in
        ubuntu:24.04)
            run_pkgs=(
                inetutils-tools
                libboost-chrono1.83.0t64 libboost-filesystem1.83.0
                libboost-program-options1.83.0 libboost-serialization1.83.0
                libboost-thread1.83.0 libusb-1.0-0
                python3 python3-requests
            )
            ;;
        ubuntu:26.04)
            run_pkgs=(
                inetutils-tools
                libboost-chrono1.90.0 libboost-filesystem1.90.0
                libboost-program-options1.90.0 libboost-serialization1.90.0
                libboost-thread1.90.0 libusb-1.0-0
                python3 python3-requests
            )
            ;;
    esac
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
        local -a install_pkgs=( "${pkgs[@]}" )
        for pkg in "${optional_pkgs[@]}"; do
            if apt-cache policy "$pkg" | awk '$1 == "Candidate:" && $2 != "(none)" { found = 1 } END { exit !found }'; then
                install_pkgs+=("$pkg")
            fi
        done
        install_ubuntu_pkgs "${install_pkgs[@]}"
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

    install_arch_pkgs "${pkgs[@]}"

    if [[ "$mode" == "all" || "$mode" == "run" ]]; then
        uhd_images_downloader
    fi
}

# Shared package lists for Fedora, CentOS Stream, and UBI 10 / RHEL 10.
install_uhd_dependencies_rpm() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(
        curl-minimal ca-certificates xz tar gzip cmake gcc gcc-c++ make boost-devel libusb1-devel
        python3-mako python3-numpy python3-setuptools python3-requests
    )
    local -a run_pkgs=(
        boost-atomic boost-chrono boost-container boost-date-time
        boost-filesystem boost-serialization boost-thread
        libusb1 python3-requests
    )
    local -a extra_pkgs=(
        kernel-tools iputils ncurses-devel libusb1-devel python3-devel
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

    # ID=rhel (UBI 10 images): CRB/EPEL via install_redhat_pkgs; Fedora/CentOS use install_rpm_pkgs.
    if [[ "${ID:-}" == rhel ]]; then
        install_redhat_pkgs "${pkgs[@]}"
    else
        install_rpm_pkgs "${pkgs[@]}"
    fi

    if [[ "$mode" == "all" || "$mode" == "run" ]]; then
        if command -v uhd_images_downloader >/dev/null 2>&1; then
            uhd_images_downloader
        fi
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
        # rhel: UBI 10 / RHEL 10 images; fedora and centos use their own /etc/os-release IDs.
        fedora|centos|rhel)
            install_uhd_dependencies_rpm "$mode"
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
