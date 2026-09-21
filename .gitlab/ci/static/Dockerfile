# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


ARG OS_VERSION=26.04
ARG PRETTIER_VERSION=3.9.8
ARG MARKDOWNLINT_VERSION=0.23.3

# npm pulls modules that ship known CVEs, so it is kept out of the final image and only the tools are copied over.
FROM ubuntu:$OS_VERSION AS node-tools
ARG PRETTIER_VERSION
ARG MARKDOWNLINT_VERSION

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    ca-certificates \
    npm \
    && npm install -g --prefix /opt/node-tools \
    prettier@${PRETTIER_VERSION} \
    markdownlint-cli2@${MARKDOWNLINT_VERSION}

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

ENV PATH="/opt/node-tools/bin:/root/.local/bin:${PATH}"
ENV PIP_NO_CACHE_DIR=1
RUN pipx install "reuse<7"

COPY --from=node-tools /opt/node-tools /opt/node-tools

ADD hex2lowercase.sh \
    include-guards-check.sh \
    is-pristine-repo.sh \
    run-clang-format-diff.sh \
    run-markdownlint.sh \
    run-prettier-diff.sh \
    update_headers.sh \
    annotate_headers.sh \
    /usr/local/bin/
