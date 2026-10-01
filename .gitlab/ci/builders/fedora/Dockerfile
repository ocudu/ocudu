# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

ARG VERSION=39
ARG OS_NAME=fedora
FROM $OS_NAME:$VERSION

RUN TZ=Europe/Madrid && \
    ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && \
    echo $TZ > /etc/timezone && \
    dnf update -y && \
    dnf install -y \
    which git \
    clang llvm lld mold \
    ccache gcovr valgrind ninja-build \
    python3-devel python3-virtualenv && \
    dnf install -y libstdc++-static && \
    dnf autoremove -y && dnf clean all && rm -rf /var/cache/dnf

ADD install_dependencies.sh common.sh builder.sh changed_tests.py add_junit_properties.py get_available_cores.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/install_dependencies.sh /usr/local/bin/builder.sh /usr/local/bin/changed_tests.py /usr/local/bin/add_junit_properties.py /usr/local/bin/get_available_cores.sh && \
    /usr/local/bin/install_dependencies.sh && \
    dnf autoremove -y && dnf clean all && rm -rf /var/cache/dnf && \
    python3 -m venv /usr/local/builder_tools && \
    /usr/local/builder_tools/bin/pip install "pandas<3" "psutil"

WORKDIR /workdir
