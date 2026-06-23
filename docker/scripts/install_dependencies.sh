#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#
# This script will install ocudu dependencies
#
# Run like this: ./install_dependencies.sh [<mode>]
# E.g.: ./install_dependencies.sh
# E.g.: ./install_dependencies.sh build
# E.g.: ./install_dependencies.sh run
# E.g.: ./install_dependencies.sh extra
#

set -e

# shellcheck source=common.sh
. "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/common.sh"

install_dependencies_debian_ubuntu() {
    local mode="${1:?}"
    local -x DEBIAN_FRONTEND=noninteractive
    local -a pkgs=()
    local ARCH=""

    local -a build_pkgs=(
        cmake make gcc g++ pkg-config
        libfftw3-dev libmbedtls-dev libsctp-dev libyaml-cpp-dev libgtest-dev
    )
    local -a run_pkgs=(
        libfftw3-dev libmbedtls-dev libsctp-dev libyaml-cpp-dev libgtest-dev libcap2-bin
    )
    local -a extra_pkgs=(
        libzmq3-dev libuhd-dev uhd-host libboost-program-options-dev libdpdk-dev libelf-dev libdwarf-dev libdw-dev capnproto libcapnp-dev
    )

    case "$mode" in
        all)
            # run_pkgs ⊆ build_pkgs; "all" is build + extra
            pkgs+=( "${build_pkgs[@]}" "${extra_pkgs[@]}" )
            ;;
        build)
            pkgs+=( "${build_pkgs[@]}" )
            ;;
        run)
            pkgs+=( "${run_pkgs[@]}" )
            ;;
        extra)
            pkgs+=( "${extra_pkgs[@]}" )
            ;;
        *)
            echo >&2 "Unsupported mode: $mode"
            exit 1
            ;;
    esac

    if [[ "$mode" == "all" || "$mode" == "extra" ]]; then
        ARCH=$(uname -m)
        if [[ "$ARCH" == "x86_64" ]]; then
            pkgs+=(gpg gpg-agent wget)
        else
            pkgs+=(wget environment-modules)
        fi
    fi

    if ((${#pkgs[@]})); then
        install_ubuntu_pkgs "${pkgs[@]}"
    fi

    if [[ "$mode" == "all" || "$mode" == "extra" ]]; then
        if [[ "$ARCH" == "x86_64" ]]; then
            wget -O- https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB | gpg --dearmor | tee /usr/share/keyrings/oneapi-archive-keyring.gpg > /dev/null
            echo "deb [trusted=yes] https://apt.repos.intel.com/oneapi all main" | tee /etc/apt/sources.list.d/oneAPI.list
            apt-get update
            apt-get install -y --no-install-recommends \
                intel-oneapi-mkl-core-devel-2025.0 libomp-dev
        else
            pushd /tmp
            wget https://developer.arm.com/-/cdn-downloads/permalink/Arm-Performance-Libraries/Version_24.10/arm-performance-libraries_24.10_deb_gcc.tar
            tar -xf arm-performance-libraries_24.10_deb_gcc.tar
            rm -f arm-performance-libraries_24.10_deb_gcc.tar
            cd arm-performance-libraries_24.10_deb/
            ./arm-performance-libraries_24.10_deb.sh --accept
            popd
            rm -Rf /tmp/arm-performance-libraries_24.10_deb/
            # shellcheck source=/dev/null
            source /usr/share/modules/init/bash
            export MODULEPATH=$MODULEPATH:/opt/arm/modulefiles
            module avail
            module load armpl/24.10.0_gcc
        fi
    fi

    _ubuntu_apt_cleanup
}

install_dependencies_fedora() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(
        cmake make libatomic fftw-devel lksctp-tools-devel yaml-cpp-devel mbedtls-devel gtest-devel
    )
    local -a run_pkgs=(
        fftw-libs-single lksctp-tools yaml-cpp mbedtls libcap
    )
    local -a extra_pkgs=(
        boost-devel capnproto capnproto-devel cppzmq-devel dpdk-devel elfutils-devel elfutils-libelf-devel
        libdwarf-devel libusb1-devel numactl-devel zeromq-devel
    )

    case "$mode" in
        all)
            # run_pkgs ⊆ build_pkgs; "all" is build + extra
            pkgs+=( "${build_pkgs[@]}" "${extra_pkgs[@]}" )
            ;;
        build)
            pkgs+=( "${build_pkgs[@]}" )
            ;;
        run)
            pkgs+=( "${run_pkgs[@]}" )
            ;;
        extra)
            pkgs+=( "${extra_pkgs[@]}" )
            ;;
        *)
            echo >&2 "Unsupported mode: $mode"
            exit 1
            ;;
    esac

    install_fedora_pkgs "${pkgs[@]}"
}

install_dependencies_arch() {
    local mode="${1:?}"
    local -a pkgs=()

    local -a build_pkgs=(
        cmake fftw mbedtls yaml-cpp lksctp-tools gtest pkgconf
    )
    local -a run_pkgs=(
        fftw mbedtls yaml-cpp lksctp-tools gtest libcap
    )
    local -a extra_pkgs=(
        zeromq libuhd boost dpdk libelf libdwarf elfutils capnproto
    )

    case "$mode" in
        all)
            # run_pkgs ⊆ build_pkgs; "all" is build + extra
            pkgs+=( "${build_pkgs[@]}" "${extra_pkgs[@]}" )
            ;;
        build)
            pkgs+=( "${build_pkgs[@]}" )
            ;;
        run)
            pkgs+=( "${run_pkgs[@]}" )
            ;;
        extra)
            pkgs+=( "${extra_pkgs[@]}" )
            ;;
        *)
            echo >&2 "Unsupported mode: $mode"
            exit 1
            ;;
    esac

    install_arch_pkgs "${pkgs[@]}"
}

install_dependencies_rhel() {
    local mode="${1:?}"
    local -a pkgs=()

    # gcc-toolset-11/12 are RHEL subscription-only and redundant on UBI9/RHEL9
    # where GCC 11 is the default compiler. libatomic replaces gcc-toolset-12-libatomic-devel.
    # yaml-cpp-devel and mbedtls-devel come from EPEL (installed below).
    local -a build_pkgs=(
        cmake fftw-devel lksctp-tools-devel
        gcc gcc-c++ libatomic
        yaml-cpp-devel mbedtls-devel
    )
    local -a run_pkgs=(
        fftw-devel lksctp-tools
        libatomic libcap
        yaml-cpp-devel mbedtls-devel
    )
    local -a extra_pkgs=(
        cppzmq-devel libusbx-devel boost-devel numactl-devel capnproto capnproto-devel
    )

    case "$mode" in
        all)
            # run_pkgs ⊆ build_pkgs; "all" is build + extra
            pkgs+=( "${build_pkgs[@]}" "${extra_pkgs[@]}" )
            ;;
        build)
            pkgs+=( "${build_pkgs[@]}" )
            ;;
        run)
            pkgs+=( "${run_pkgs[@]}" )
            ;;
        extra)
            pkgs+=( "${extra_pkgs[@]}" )
            ;;
        *)
            echo >&2 "Unsupported mode: $mode"
            exit 1
            ;;
    esac

    dnf -y install https://dl.fedoraproject.org/pub/epel/epel-release-latest-9.noarch.rpm
    install_rhel_pkgs "${pkgs[@]}"
}

main() {

    if [ $# != 0 ] && [ $# != 1 ]; then
        echo >&2 "Illegal number of parameters"
        echo >&2 "Run like this: \"./install_dependencies.sh [<mode>]\" where mode could be: build, run and extra"
        echo >&2 "If mode is not specified, all dependencies will be installed"
        exit 1
    fi

    local mode="${1:-all}"

    # shellcheck source=/dev/null
    . /etc/os-release

    echo "== Installing OCUDU dependencies, mode $mode =="

    case "$ID" in
        debian|ubuntu)
            install_dependencies_debian_ubuntu "$mode"
            ;;
        arch)
            install_dependencies_arch "$mode"
            ;;
        rhel)
            install_dependencies_rhel "$mode"
            ;;
        fedora)
            install_dependencies_fedora "$mode"
            ;;
        *)
            echo "OS $ID not supported"
            exit 1
            ;;
    esac

}

main "$@"
