# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

FROM ubuntu:24.04

RUN TZ=Europe/Madrid && \
    ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && \
    echo $TZ > /etc/timezone && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
    git gdb \
    clang-20 libclang-rt-20-dev llvm-20 lld mold \
    ccache && \
    apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/* && \
    update-alternatives --install /usr/bin/clang clang /usr/bin/clang-20 100 && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-20 100

ADD install_dependencies.sh common.sh builder.sh get_available_cores.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/install_dependencies.sh /usr/local/bin/builder.sh /usr/local/bin/get_available_cores.sh && \
    /usr/local/bin/install_dependencies.sh && \
    apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

ADD lib /opt/lib
ADD rohc /opt/rohc
ADD uhd /opt/uhd
ADD dpdk /opt/dpdk

WORKDIR /workdir
