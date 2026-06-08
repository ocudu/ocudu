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
        local -a versioned_pkgs=()
        for pkg in "${pkgs[@]}"; do versioned_pkgs+=("$(_pkg_ver "$pkg")"); done
        apt-get update
        for pkg in "${optional_pkgs[@]}"; do
            if apt-cache policy "$pkg" | awk '$1 == "Candidate:" && $2 != "(none)" { found = 1 } END { exit !found }'; then
                versioned_pkgs+=("$pkg")
            fi
        done
        apt-get install -y --no-install-recommends "${versioned_pkgs[@]}"
        apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/*
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
        local -a versioned_pkgs=()
        for pkg in "${pkgs[@]}"; do versioned_pkgs+=("$(_pkg_ver "$pkg")"); done
        pacman -Syu --noconfirm "${versioned_pkgs[@]}"
        pacman -Scc --noconfirm
    fi

    if [[ "$mode" == "all" || "$mode" == "run" ]]; then
        uhd_images_downloader
    fi
}

install_uhd_dependencies_fedora() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(
        "$(_pkg_ver curl)" "$(_pkg_ver ca-certificates)" "$(_pkg_ver xz)" "$(_pkg_ver cmake)" "$(_pkg_ver make)" "$(_pkg_ver boost-devel)" "$(_pkg_ver libusb1-devel)"
        "$(_pkg_ver python3-mako)" "$(_pkg_ver python3-numpy)" "$(_pkg_ver python3-setuptools)" "$(_pkg_ver python3-requests)"
    )
    local -a run_pkgs=(
        "$(_pkg_ver boost-atomic)" "$(_pkg_ver boost-chrono)" "$(_pkg_ver boost-container)" "$(_pkg_ver boost-date-time)"
        "$(_pkg_ver boost-filesystem)" "$(_pkg_ver boost-serialization)" "$(_pkg_ver boost-thread)"
        "$(_pkg_ver libusb1)" "$(_pkg_ver python3-requests)"
    )
    local -a extra_pkgs=(
        "$(_pkg_ver kernel-tools)" "$(_pkg_ver iputils)" "$(_pkg_ver ncurses-devel)" "$(_pkg_ver libusb1-devel)" "$(_pkg_ver python3-devel)"
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

install_uhd_dependencies_rhel() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(
        ca-certificates xz
        cmake gcc gcc-c++ make pkgconf-pkg-config
        boost-devel libusbx-devel
        python3-mako python3-numpy python3-setuptools python3-requests
    )
    local -a run_pkgs=(
        boost-devel libusbx libusbx-devel python3-devel python3-requests
    )
    local -a extra_pkgs=(
        boost-devel libusbx libusbx-devel python3-devel
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
        local -a versioned_pkgs=()
        for pkg in "${pkgs[@]}"; do versioned_pkgs+=("$(_pkg_ver "$pkg")"); done
        dnf -y install "${versioned_pkgs[@]}"
        dnf clean all
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
        rhel)
            install_uhd_dependencies_rhel "$mode"
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
