# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

ARG VERSION=25.04
ARG OS_NAME=ubuntu
FROM $OS_NAME:$VERSION

RUN TZ=Europe/Madrid && \
    ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && \
    echo $TZ > /etc/timezone && \
    apt-get update && \
    (apt-get remove --purge -y pebble; rm -f /usr/bin/pebble) && \
    apt-get install -y --no-install-recommends \
    git gdb \
    clang llvm lld mold \
    ccache gcovr valgrind ninja-build \
    python3-dev python3-venv && \
    (apt-get install -y --no-install-recommends libclang-rt-dev || true) && \
    apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/*

ADD install_dependencies.sh common.sh builder.sh changed_tests.py add_junit_properties.py get_available_cores.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/install_dependencies.sh /usr/local/bin/builder.sh /usr/local/bin/changed_tests.py /usr/local/bin/add_junit_properties.py /usr/local/bin/get_available_cores.sh && \
    /usr/local/bin/install_dependencies.sh && \
    apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* && \
    python3 -m venv /usr/local/builder_tools && \
    /usr/local/builder_tools/bin/pip install --no-cache-dir --upgrade pip "setuptools>=78.1.1" && \
    /usr/local/builder_tools/bin/pip install --no-cache-dir "pandas<3" "psutil"

# TEMPORARY, remove once ubuntu 25.10 ships a rebuilt libgtest-dev.
#
# libgtest-dev 1.17.0-1 was built while Ubuntu's gcc-15 defaulted to -Wa,-gsframe (enabled
# in 15.2.0-1ubuntu1, reverted in 15.2.0-4ubuntu2). Its .sframe sections relocate into
# COMDAT .text that the linker discards once one of our TUs supplies the same inline gtest
# function, which ld.lld rejects and GNU ld tolerates (llvm/llvm-project#156667):
#
#   ld.lld: error: relocation refers to a discarded section: .text._ZN7testing4Test5SetupEv
#
# 26.04 is unaffected because it carries a rebuilt package, 1.17.0-1build1, which has no
# .sframe. No such rebuild exists for 25.10, so we do it ourselves from the sources the
# package ships, using this image's post-revert compiler. Pinned to ubuntu 25.10 and
# 1.17.0-1, plus an actual .sframe check.
RUN set -eu; \
    [ -r /etc/os-release ] && . /etc/os-release || true; \
    gtest_a="$(find /usr/lib -name libgtest.a -print -quit)"; \
    gtest_ver="$(dpkg-query -W -f='${Version}' libgtest-dev 2>/dev/null || true)"; \
    has_sframe=no; \
    if [ -n "${gtest_a}" ] && readelf -SW "${gtest_a}" | grep -q '\.sframe'; then has_sframe=yes; fi; \
    if [ "${ID:-}:${VERSION_ID:-}" = "ubuntu:25.10" ] && [ "${gtest_ver}" = "1.17.0-1" ] && [ "${has_sframe}" = yes ]; then \
        [ -d /usr/src/gtest ] || { echo "ERROR: libgtest-dev ${gtest_ver} needs the .sframe rebuild but /usr/src/gtest is gone"; exit 1; }; \
        cmake -S /usr/src/gtest -B /tmp/gtest-build -DCMAKE_BUILD_TYPE=Release; \
        cmake --build /tmp/gtest-build --parallel; \
        find /tmp/gtest-build -name 'libgtest*.a' -exec cp {} "$(dirname "${gtest_a}")/" ';'; \
        rm -rf /tmp/gtest-build; \
        if readelf -SW "${gtest_a}" | grep -q '\.sframe'; then echo "ERROR: .sframe still present after rebuild"; exit 1; fi; \
        echo "Rebuilt gtest ${gtest_ver} without .sframe"; \
    else \
        echo "Skipping gtest rebuild: ${ID:-?}:${VERSION_ID:-?} libgtest-dev=${gtest_ver:-none} sframe=${has_sframe}"; \
    fi

ADD lib /opt/lib
ADD rohc /opt/rohc
ADD uhd /opt/uhd
ADD dpdk /opt/dpdk

WORKDIR /workdir
