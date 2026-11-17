#!/usr/bin/env bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

# Creates or updates a subscriber of the running Open5GS container, subscribed to the given slice. The container is not
# restarted. The slice has to be one of the s_nssai of the AMF in open5gs-5gc.yml for the UE to register.
#
# Usage: ./set_ue_slice.sh IMSI SST [SD]
#   ./set_ue_slice.sh 001010123456780 1 111111                       # SST 1, SD 111111
#   ./set_ue_slice.sh 001010123456780 2                              # SST 2, no SD
#   IP_ALLOC=10.45.2.2 ./set_ue_slice.sh 001010123456781 1 222222    # a second UE, with its own IP
#
# KEY, OPC, AMF, QCI, IP_ALLOC and CONTAINER can be set in the environment. The whole subscriber is written, not only
# its slice, so a UE that has other values than the defaults needs the same KEY, OPC, AMF, QCI and IP_ALLOC. The IP is
# static because Open5GS rejects the registration of a subscriber with an empty IP. UEs attached at the same time need
# different IMSI and IP_ALLOC.
set -euo pipefail

IMSI="${1:?Usage: $0 IMSI SST [SD]}"
SST="${2:?Usage: $0 IMSI SST [SD]}"
SD="${3:-}"

KEY="${KEY:-00112233445566778899aabbccddeeff}"
OPC="${OPC:-63bfa50ee6523365ff14c1f45f88737d}"
AMF="${AMF:-8000}"
QCI="${QCI:-9}"
IP_ALLOC="${IP_ALLOC:-10.45.1.2}"
CONTAINER="${CONTAINER:-open5gs_5gc}"

SUBSCRIBER_DATA="${IMSI},${KEY},opc,${OPC},${AMF},${QCI},${IP_ALLOC},${SST}${SD:+,${SD}}"

# The core reads the database that MONGODB_IP of the container points to. The default is applied inside the container,
# so a failure of docker exec stops the script instead of selecting the default.
MONGODB_IP="$(docker exec "$CONTAINER" sh -c 'echo "${MONGODB_IP:-127.0.0.1}"')"

docker exec -w /open5gs "$CONTAINER" python3 add_users.py --mongodb "$MONGODB_IP" \
  --subscriber_data "$SUBSCRIBER_DATA"
