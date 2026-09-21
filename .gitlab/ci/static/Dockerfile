# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


ARG OS_VERSION=26.04
ARG PRETTIER_VERSION=3.9.8

# npm pulls modules that ship known CVEs, so it is kept out of the final image and only prettier is copied over.
FROM ubuntu:$OS_VERSION AS prettier
ARG PRETTIER_VERSION

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    ca-certificates \
    npm \
    && npm install -g --prefix /opt/prettier prettier@${PRETTIER_VERSION}

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
    nodejs \
    && rm -f /usr/bin/pebble \
    && apt-get autoremove \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

ENV PATH="/opt/prettier/bin:/root/.local/bin:${PATH}"
ENV PIP_NO_CACHE_DIR=1
RUN pipx install "reuse<7"

COPY --from=prettier /opt/prettier /opt/prettier

ADD hex2lowercase.sh \
    include-guards-check.sh \
    is-pristine-repo.sh \
    run-clang-format-diff.sh \
    run-prettier-diff.sh \
    update_headers.sh \
    annotate_headers.sh \
    /usr/local/bin/
