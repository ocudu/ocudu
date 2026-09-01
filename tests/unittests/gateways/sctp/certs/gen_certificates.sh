#!/usr/bin/env bash
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
# DTLS association 1
# -------------------------------------------------------------------

generate_endpoint_cert link1-a
generate_endpoint_cert link1-b

# -------------------------------------------------------------------
# DTLS association 2
# -------------------------------------------------------------------

generate_endpoint_cert link2-a
generate_endpoint_cert link2-b

# -------------------------------------------------------------------
# DTLS association 3
# -------------------------------------------------------------------

generate_endpoint_cert link3-a
generate_endpoint_cert link3-b

# -------------------------------------------------------------------
# Verify certificates
# -------------------------------------------------------------------

echo
echo "Verifying certificates..."

for cert in \
    link1-a.crt \
    link1-b.crt \
    link2-a.crt \
    link2-b.crt \
    link3-a.crt \
    link3-b.crt
do
    openssl verify \
        -CAfile ca.crt \
        "$cert"
done

echo
echo "Generated files:"
echo
ls -1 \
    ca.crt \
    ca.key \
    link1-a.crt link1-a.key \
    link1-b.crt link1-b.key \
    link2-a.crt link2-a.key \
    link2-b.crt link2-b.key \
    link3-a.crt link3-a.key \
    link3-b.crt link3-b.key

echo
echo "Done."
