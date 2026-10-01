# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

ARG VERSION=base-20251019.0.436919
FROM archlinux:$VERSION

ADD install_dependencies.sh common.sh /usr/local/bin
RUN chmod +x /usr/local/bin/install_dependencies.sh

# mbedtls3 is namespaced out of the default pkg-config search path.
ENV PKG_CONFIG_PATH=/usr/lib/mbedtls3/pkgconfig

RUN TZ=Europe/Madrid && ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ >/etc/timezone
RUN pacman-key --init \
  && pacman -Sy --noconfirm archlinux-keyring \
  && pacman-key --populate archlinux \
  && pacman -Syu --noconfirm
RUN install_dependencies.sh && pacman -Syu --noconfirm \
  git \
  base-devel \
  which \
  clang lld mold \
  ccache valgrind ninja \
  python-pip
RUN pip install --break-system-packages gcovr==5.0

ADD builder.sh get_available_cores.sh /usr/local/bin
RUN chmod +x /usr/local/bin/builder.sh /usr/local/bin/get_available_cores.sh

RUN python -m venv /usr/local/builder_tools
RUN /usr/local/builder_tools/bin/pip install "pandas < 3" "psutil < 6"
ADD changed_tests.py add_junit_properties.py /usr/local/bin/
RUN chmod +x /usr/local/bin/changed_tests.py /usr/local/bin/add_junit_properties.py

WORKDIR /workdir
