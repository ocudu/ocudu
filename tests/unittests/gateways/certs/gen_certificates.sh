#!/usr/bin/env bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set -euo pipefail

DAYS=3650

echo "Generating SCTP/DTLS test certificates in: $(pwd)"

# -------------------------------------------------------------------
# Generate the test Certificate Authority
# -------------------------------------------------------------------

echo "Generating CA..."

openssl genrsa \
    -out ca.key \
    2048

openssl req \
    -x509 \
    -new \
    -key ca.key \
    -sha256 \
    -days "$DAYS" \
    -out ca.crt \
    -subj "/C=XX/O=Test/CN=SCTP DTLS Test CA" \
    -addext "basicConstraints=critical,CA:TRUE" \
    -addext "keyUsage=critical,keyCertSign,cRLSign"

# -------------------------------------------------------------------
# Generate an endpoint key and certificate
#
# Usage:
#   generate_endpoint_cert <name>
# -------------------------------------------------------------------

generate_endpoint_cert()
{
    local name="$1"

    echo "Generating certificate for $name..."

    # Generate endpoint private key
    openssl genrsa \
        -out "${name}.key" \
        2048

    # Create a temporary certificate from the endpoint key.
    # The certificate is then signed by the test CA below.
    openssl x509 \
        -req \
        -in <(
            openssl req \
                -new \
                -key "${name}.key" \
                -subj "/C=XX/O=Test/CN=${name}"
        ) \
        -CA ca.crt \
        -CAkey ca.key \
        -CAcreateserial \
        -out "${name}.crt" \
        -days "$DAYS" \
        -sha256 \
        -extfile <(
            printf '%s\n' \
                "basicConstraints=critical,CA:FALSE" \
                "keyUsage=critical,digitalSignature,keyEncipherment" \
                "extendedKeyUsage=serverAuth,clientAuth" \
                "subjectAltName=DNS:${name},DNS:localhost,IP:127.0.0.1"
        )
}

# -------------------------------------------------------------------
# DTLS associations
# -------------------------------------------------------------------

generate_endpoint_cert link12
generate_endpoint_cert link21
generate_endpoint_cert link13
generate_endpoint_cert link31
generate_endpoint_cert link23
generate_endpoint_cert link32

# -------------------------------------------------------------------
# Verify certificates
# -------------------------------------------------------------------

echo
echo "Verifying certificates..."

for cert in link*.crt
do
    openssl verify \
        -CAfile ca.crt \
        "$cert"
done

echo "Done."
