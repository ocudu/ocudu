#!/bin/sh

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set -eu

fail()
{
    echo "SBOM validation failed: $*" >&2
    exit 1
}

[ "$#" -ge 2 ] || fail "usage: $0 <spdx-file> <expected-file>..."

sbom_file=$1
shift

[ -s "${sbom_file}" ] || fail "${sbom_file} is missing or empty"

if grep -Fq '${SBOM_' "${sbom_file}"; then
    fail "unexpanded SBOM template token found"
fi

awk '
    /^DocumentNamespace:/ {
        count++
        value = substr($0, index($0, ":") + 2)
        if (value !~ /^https?:\/\/[^[:space:]]+$/) {
            invalid = 1
        }
    }
    END { exit !(count == 1 && !invalid) }
' "${sbom_file}" || fail "DocumentNamespace must be one concrete HTTP(S) URI"

awk '
    /^PackageVerificationCode:/ {
        count++
        value = substr($0, index($0, ":") + 2)
        if (length(value) != 40 || value !~ /^[[:xdigit:]]+$/) {
            invalid = 1
        }
    }
    END { exit !(count == 1 && !invalid) }
' "${sbom_file}" || fail "PackageVerificationCode must be one 40-character hexadecimal value"

awk '
    /^PackageName:/ {
        count++
        value = substr($0, index($0, ":") + 1)
        gsub(/^[[:space:]]+|[[:space:]]+$/, "", value)
        if (value == "" || value == "NONE" || value == "NOASSERTION") {
            invalid = 1
        }
    }
    END { exit !(count > 0 && !invalid) }
' "${sbom_file}" || fail "PackageName is missing or malformed"

validation_dir=$(mktemp -d)
trap 'rm -rf "${validation_dir}"' EXIT HUP INT TERM
expected_names="${validation_dir}/expected-files"
actual_entries="${validation_dir}/actual-entries"
actual_names="${validation_dir}/actual-files"

: > "${expected_names}"
for expected_file in "$@"; do
    normalized_file=${expected_file#./}
    normalized_file=${normalized_file#/}
    [ -n "${normalized_file}" ] || fail "expected filename is empty"
    printf '%s\n' "${normalized_file}" >> "${expected_names}"
done
LC_ALL=C sort -o "${expected_names}" "${expected_names}"
if [ -n "$(uniq -d "${expected_names}")" ]; then
    fail "expected file inventory contains duplicates"
fi

awk -F ': ' '
    function emit_file() {
        if (filename != "") {
            if (spdxid == "" || checksum == "") {
                exit 2
            }
            sub(/^\.\//, "", filename)
            print filename "\t" spdxid "\t" checksum
        }
        filename = ""
        spdxid = ""
        checksum = ""
    }
    /^FileName: / {
        emit_file()
        filename = substr($0, index($0, ":") + 2)
        next
    }
    filename != "" && /^SPDXID: / {
        spdxid = $2
        next
    }
    filename != "" && /^FileChecksum: SHA1: / {
        checksum = $3
        next
    }
    /^$/ { emit_file() }
    END { emit_file() }
' "${sbom_file}" > "${actual_entries}" || fail "File entry is missing an SPDXID or SHA1 checksum"

[ -s "${actual_entries}" ] || fail "no File entries found"
if ! awk -F '\t' 'length($3) != 40 || $3 !~ /^[[:xdigit:]]+$/ { exit 1 }' "${actual_entries}"; then
    fail "FileChecksum must contain a 40-character SHA1 value"
fi

cut -f 1 "${actual_entries}" | LC_ALL=C sort > "${actual_names}"
if [ -n "$(uniq -d "${actual_names}")" ]; then
    fail "File inventory contains duplicate filenames"
fi
if [ -n "$(cut -f 2 "${actual_entries}" | LC_ALL=C sort | uniq -d)" ]; then
    fail "File inventory contains duplicate SPDXIDs"
fi
expected_inventory=$(cat "${expected_names}")
actual_inventory=$(cat "${actual_names}")
if [ "${expected_inventory}" != "${actual_inventory}" ]; then
    echo "Expected SBOM files:" >&2
    sed 's/^/  /' "${expected_names}" >&2
    echo "Actual SBOM files:" >&2
    sed 's/^/  /' "${actual_names}" >&2
    fail "incorrect File inventory"
fi

described_package=$(awk '
    /^Relationship: SPDXRef-DOCUMENT DESCRIBES / {
        count++
        package = $4
    }
    END {
        if (count == 1) {
            print package
        }
    }
' "${sbom_file}")
[ -n "${described_package}" ] || fail "exactly one document DESCRIBES relationship is required"

awk -F '\t' -v package="${described_package}" '
    NR == FNR {
        file_ids[$2] = 0
        next
    }
    /^Relationship:/ {
        field_count = split($0, fields, /[[:space:]]+/)
        if (fields[2] != package || fields[3] != "CONTAINS") {
            next
        }
        if (!(fields[4] in file_ids) || field_count != 4) {
            invalid = 1
        } else {
            file_ids[fields[4]]++
        }
    }
    END {
        for (id in file_ids) {
            if (file_ids[id] != 1) {
                invalid = 1
            }
        }
        exit invalid
    }
' "${actual_entries}" "${sbom_file}" || fail "each File SPDXID must have exactly one package-to-file CONTAINS relationship"

if [ -n "${ATTESTATION_IMAGE:-}" ]; then
    attestation_root=${ATTESTATION_FILE_ROOT:-/usr/local}
    while IFS="$(printf '\t')" read -r filename _ expected_sha1; do
        image_file="${validation_dir}/image-file"
        regctl image get-file "${ATTESTATION_IMAGE}" "${attestation_root}/${filename}" > "${image_file}" ||
            fail "cannot read ${attestation_root}/${filename} from ${ATTESTATION_IMAGE}"
        actual_sha1=$(sha1sum "${image_file}" | awk '{print $1}')
        [ "${actual_sha1}" = "${expected_sha1}" ] ||
            fail "checksum mismatch for ${attestation_root}/${filename}"
    done < "${actual_entries}"
fi

echo "Validated ${sbom_file}: $(wc -l < "${actual_names}") file(s)"
