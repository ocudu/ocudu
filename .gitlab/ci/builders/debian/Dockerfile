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
    ccache gcovr valgrind \
    python3-dev python3-venv && \
    (apt-get install -y --no-install-recommends libclang-rt-dev || true) && \
    apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/*

ADD install_dependencies.sh common.sh builder.sh changed_tests.py get_available_cores.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/install_dependencies.sh /usr/local/bin/builder.sh /usr/local/bin/changed_tests.py /usr/local/bin/get_available_cores.sh && \
    /usr/local/bin/install_dependencies.sh && \
    apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* && \
    python3 -m venv /usr/local/builder_tools && \
    /usr/local/builder_tools/bin/pip install --no-cache-dir "pandas<3" "psutil"

ADD lib /opt/lib
ADD rohc /opt/rohc
ADD uhd /opt/uhd
ADD dpdk /opt/dpdk

WORKDIR /workdir
