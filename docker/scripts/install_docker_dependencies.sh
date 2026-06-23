#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#
# Utilities used in container/runtime environments.
# Installs the OS-specific package list for the given mode.
#
# Run like this: ./install_docker_dependencies.sh [<mode>]
# E.g.: ./install_docker_dependencies.sh
# E.g.: ./install_docker_dependencies.sh build
# E.g.: ./install_docker_dependencies.sh run
#

set -e

# shellcheck source=common.sh
. "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/common.sh"

install_docker_dependencies_debian_ubuntu() {
    local mode="${1:?}"
    local -x DEBIAN_FRONTEND=noninteractive
    local -a pkgs=()

    local -a build_pkgs=(git ca-certificates)
    local -a run_pkgs=(curl tini)

    case "$mode" in
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

    local -a extra_pkgs=()
    if [[ "$mode" == "run" ]]; then
        apt-get update
        if apt-cache policy ntpdate | awk '$1 == "Candidate:" && $2 != "(none)" { found = 1 } END { exit !found }'; then
            extra_pkgs+=(ntpdate)
        else
            extra_pkgs+=(ntpsec-ntpdate)
        fi
    fi
    install_ubuntu_pkgs "${pkgs[@]}" "${extra_pkgs[@]}"
}

install_docker_dependencies_arch() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(git ca-certificates base-devel which)
    local -a run_pkgs=(curl ntp)

    case "$mode" in
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
}

install_docker_dependencies_fedora() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(git ca-certificates make gcc gcc-c++ pkgconf-pkg-config which)
    local -a run_pkgs=(curl chrony tini procps-ng)

    case "$mode" in
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

    install_fedora_pkgs "${pkgs[@]}"
}

install_docker_dependencies_rhel() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(git ca-certificates make gcc gcc-c++ pkgconf-pkg-config which)
    local -a run_pkgs=(chrony)

    case "$mode" in
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

    install_rhel_pkgs "${pkgs[@]}"
}

main() {
    if [ $# != 0 ] && [ $# != 1 ]; then
        echo >&2 "Illegal number of parameters"
        echo >&2 "Run like this: \"./install_docker_dependencies.sh [<mode>]\" where mode could be: build, run"
        echo >&2 "If mode is not specified, run dependencies will be installed"
        exit 1
    fi

    local mode="${1:-run}"

    # shellcheck source=/dev/null
    . /etc/os-release

    echo "== Installing Docker/runtime helper packages, mode $mode =="

    case "$ID" in
        debian|ubuntu)
            install_docker_dependencies_debian_ubuntu "$mode"
            ;;
        arch)
            install_docker_dependencies_arch "$mode"
            ;;
        rhel)
            install_docker_dependencies_rhel "$mode"
            ;;
        fedora)
            install_docker_dependencies_fedora "$mode"
            ;;
        *)
            echo "OS $ID not supported"
            exit 1
            ;;
    esac
}

main "$@"
