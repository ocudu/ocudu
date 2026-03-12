#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

# Convert AFL++ crash/hang inputs to hex for use in unit tests.
#
# Usage:
#   python3 tests/fuzz/crash_to_hex.py findings/ngap_cu_cp/crashes/id:000000,*
#   python3 tests/fuzz/crash_to_hex.py findings/ngap_cu_cp/crashes/
#   python3 tests/fuzz/crash_to_hex.py findings/          # recurse all subdirs

import argparse
import sys
from pathlib import Path


def format_c_array(name: str, data: bytes) -> str:
    """Format bytes as a C/C++ uint8_t array literal."""
    hex_bytes = ", ".join(f"0x{b:02x}" for b in data)
    return (
        f"// {len(data)} bytes\n"
        f"const uint8_t {name}[] = {{{hex_bytes}}};"
    )


def process_file(path: Path, fmt: str) -> None:
    data = path.read_bytes()
    print(f"=== {path} ({len(data)} bytes) ===")
    if fmt == "hex":
        print(data.hex())
    elif fmt == "xxd":
        for i in range(0, len(data), 16):
            chunk = data[i : i + 16]
            hex_part = " ".join(f"{b:02x}" for b in chunk)
            asc_part = "".join(chr(b) if 32 <= b < 127 else "." for b in chunk)
            print(f"  {i:04x}  {hex_part:<47}  {asc_part}")
    elif fmt == "c":
        varname = path.name.replace(":", "_").replace(",", "_").replace("-", "_")
        print(format_c_array(varname, data))
    print()


def collect_files(inputs: list[str]) -> list[Path]:
    files: list[Path] = []
    for raw in inputs:
        p = Path(raw)
        if p.is_dir():
            # Recurse: pick up crashes/ and hangs/ sub-directories.
            for child in sorted(p.rglob("id:*")):
                if child.is_file():
                    files.append(child)
        elif p.is_file():
            files.append(p)
        else:
            # Glob expansion already happened in the shell (e.g. id:000000,*)
            print(f"WARNING: {p} not found, skipping", file=sys.stderr)
    return files


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Convert AFL++ crash/hang inputs to hex or C array literals."
    )
    parser.add_argument(
        "inputs",
        nargs="+",
        metavar="FILE_OR_DIR",
        help="crash file(s) or directory containing id:* files",
    )
    parser.add_argument(
        "--format",
        choices=["hex", "xxd", "c"],
        default="hex",
        help=(
            "hex  – plain hex string (default)\n"
            "xxd  – annotated hex dump\n"
            "c    – C/C++ uint8_t array literal"
        ),
    )
    args = parser.parse_args()

    files = collect_files(args.inputs)
    if not files:
        print("No crash files found.", file=sys.stderr)
        sys.exit(1)

    for f in files:
        process_file(f, args.format)


if __name__ == "__main__":
    main()
