# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


ARG OS_VERSION=26.04
FROM ubuntu:$OS_VERSION AS builder

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Madrid

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    clang-format \
    git \
    colordiff \
    apt-transport-https \
    ca-certificates \
    gawk \
    yamllint \
    file \
    pipx \
    python3-yaml \
    && rm -f /usr/bin/pebble \
    && apt-get autoremove \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

ENV PATH="/root/.local/bin:${PATH}"
ENV PIP_NO_CACHE_DIR=1
RUN pipx install "reuse<7"

ADD hex2lowercase.sh \
    include-guards-check.sh \
    is-pristine-repo.sh \
    run-clang-format-diff.sh \
    update_headers.sh \
    annotate_headers.sh \
    /usr/local/bin/
