#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set -e

# shellcheck source=common.sh
. "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/common.sh"

install_rohc_dependencies_debian_ubuntu() {
    local mode="${1:?}"
    local -x DEBIAN_FRONTEND=noninteractive
    local -a pkgs=()

    local -a build_pkgs=(
        curl ca-certificates build-essential xz-utils gzip autotools-dev automake libtool libpcap-dev libcmocka-dev
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

    install_ubuntu_pkgs "${pkgs[@]}"
}

install_rohc_dependencies_fedora() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(
        curl ca-certificates gcc gcc-c++ make which xz tar gzip
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

    install_rpm_pkgs "${pkgs[@]}"

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

install_rohc_dependencies_centos() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(
        curl ca-certificates gcc gcc-c++ make which xz gzip
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

    install_rpm_pkgs "${pkgs[@]}"

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

install_rohc_dependencies_redhat() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(
        curl-minimal ca-certificates gcc gcc-c++ make which xz gzip
        autoconf automake libtool libpcap-devel libcmocka-devel
    )
    build_pkgs+=(tar diffutils)
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

    install_redhat_pkgs "${pkgs[@]}"

    # RPM autotools ship versioned binaries (e.g. aclocal-1.16) but ROHC autogen.sh
    # expects unversioned aclocal/automake on PATH; link the first match when missing.
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
        curl ca-certificates base-devel which xz gzip autoconf automake libtool libpcap cmocka
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

    install_arch_pkgs "${pkgs[@]}"
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
        # ID=rhel: UBI 10 / RHEL 10 container images (not Fedora or CentOS Stream).
        rhel)
            install_rohc_dependencies_redhat "$mode"
            ;;
        centos)
            install_rohc_dependencies_centos "$mode"
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
