# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

ARG CODECHECKER_VERSION=6.26.1

FROM ubuntu:22.04 AS codecheck_builder
ARG CODECHECKER_VERSION
RUN DEBIAN_FRONTEND=noninteractive TZ=Europe/Madrid \ 
  apt-get update && apt-get install -y --no-install-recommends \
  git python3-pip python3-dev gcc\
  && apt-get autoremove && apt-get clean && rm -rf /var/lib/apt/lists/*
RUN pip install codechecker==${CODECHECKER_VERSION} 

FROM ubuntu:22.04
ARG CODECHECKER_VERSION
RUN DEBIAN_FRONTEND=noninteractive TZ=Europe/Madrid \ 
  apt-get update && apt-get install -y --no-install-recommends \
  python3-pip clang clang-tidy cppcheck \
  cmake make ccache \
  && apt-get autoremove && apt-get clean && rm -rf /var/lib/apt/lists/*
COPY --from=codecheck_builder /root/.cache/pip/wheels /root/.cache/pip/wheels
RUN pip install codechecker==${CODECHECKER_VERSION} 

ADD codechecker/static-analyzer.sh codechecker/cppcheck_suppress_unspecified.py /usr/local/bin/
RUN chmod +x /usr/local/bin/static-analyzer.sh /usr/local/bin/cppcheck_suppress_unspecified.py

ADD install_dependencies.sh common.sh builder.sh get_available_cores.sh /usr/local/bin/
RUN chmod +x /usr/local/bin/install_dependencies.sh /usr/local/bin/builder.sh /usr/local/bin/get_available_cores.sh

RUN DEBIAN_FRONTEND=noninteractive apt-get update && install_dependencies.sh \
  && apt-get update && apt-get install -y --no-install-recommends git \
  && apt-get autoremove && apt-get clean && rm -rf /var/lib/apt/lists/*
