#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

"""
Copy the requirement tags recorded by the tests onto the matching CTest JUnit entries.

Each test process writes its own gtest XML report (see cmake/scripts/gtest_xml_launcher.sh), in which
a test that called OCUDU_TEST_REQUIREMENTS carries a "requirements" property. CTest stays the
authority on pass, fail, crash and timeout; this only adds the metadata its JUnit output cannot carry.

Usage:
  merge_gtest_requirements.py <xunit.xml> --gtest-xml-dir <dir> [--strict]
"""

import argparse
import sys
import xml.etree.ElementTree as ET
from collections import defaultdict
from pathlib import Path

PROPERTY_NAME = "requirements"


def _requirements_of(testcase) -> set[str]:
    """Read the requirement ids off a gtest <testcase>, as a property or as a legacy attribute."""
    raw = testcase.get(PROPERTY_NAME, "")

    props = testcase.find("properties")
    if props is not None:
        for prop in props.findall("property"):
            if prop.get("name") == PROPERTY_NAME:
                raw = prop.get("value", "")

    return {value.strip() for value in raw.split(";") if value.strip()}


def collect(gtest_xml_dir: Path) -> tuple[dict[str, set[str]], dict[str, set[str]], dict[str, str]]:
    """Return ({"<suite>.<case>": ids}, {"<binary>": ids}, {"<suite>.<case>": binary}) from the folder."""
    by_case: dict[str, set[str]] = defaultdict(set)
    by_binary: dict[str, set[str]] = defaultdict(set)
    case_binary: dict[str, str] = {}

    for path in sorted(gtest_xml_dir.glob("*.xml")):
        # The launcher names the report "<binary>.<pid>.<timestamp>.xml". A report is missing or
        # truncated when the test crashed; ctest has already recorded that as a failure.
        binary = path.name.split(".")[0]
        try:
            root = ET.parse(path).getroot()
        except ET.ParseError as err:
            print(f"WARNING: {path}: {err}", file=sys.stderr)
            continue

        for testcase in root.iter("testcase"):
            ids = _requirements_of(testcase)
            if not ids:
                continue
            suite = testcase.get("classname", "")
            name = testcase.get("name", "")
            by_case[f"{suite}.{name}"] |= ids
            by_binary[binary] |= ids
            case_binary[f"{suite}.{name}"] = binary

    return by_case, by_binary, case_binary


def apply(junit_xml: Path, by_case: dict[str, set[str]], by_binary: dict[str, set[str]]) -> set[str]:
    """Add a requirements property to the matching entries. Returns the test ids that matched."""
    tree = ET.parse(junit_xml)
    matched: set[str] = set()

    for testcase in tree.getroot().iter("testcase"):
        name = testcase.get("name", "")

        # A target registered with gtest_discover_tests has one ctest entry per case, named after it.
        # A target registered as a single entry is named after the binary, and collects everything its
        # tests recorded.
        ids = by_case.get(name) or by_binary.get(name)
        if not ids:
            continue

        props = testcase.find("properties")
        if props is None:
            props = ET.SubElement(testcase, "properties")
        ET.SubElement(props, "property", {"name": PROPERTY_NAME, "value": ";".join(sorted(ids))})
        matched.add(name)

    tree.write(junit_xml, encoding="utf-8", xml_declaration=True)
    return matched


def main() -> int:
    """Merge the requirement tags of the gtest reports into a CTest JUnit file, in place."""
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("junit_xml", type=Path, help="CTest JUnit XML file to modify in-place")
    parser.add_argument("--gtest-xml-dir", type=Path, required=True, help="Folder holding the gtest XML reports")
    parser.add_argument("--strict", action="store_true", help="Fail if a recorded requirement reached no entry")
    args = parser.parse_args()

    if not args.gtest_xml_dir.is_dir():
        parser.error(f"Not a directory: {args.gtest_xml_dir}")

    by_case, by_binary, case_binary = collect(args.gtest_xml_dir)
    if not by_case:
        print("WARNING: no requirement tags found. Was OCUDU_GTEST_XML_DIR set?", file=sys.stderr)
        return 1 if args.strict else 0

    matched = apply(args.junit_xml, by_case, by_binary)

    # A tag that reaches no entry would leave the requirement looking untested, so it is worth saying
    # out loud rather than letting the report show a silent gap. A case is covered either by its own
    # ctest entry or by the whole-binary entry it ran under.
    orphans = {test for test in by_case if test not in matched and case_binary[test] not in matched}
    for test in sorted(orphans):
        print(f"WARNING: {test} recorded {sorted(by_case[test])} but matched no ctest entry", file=sys.stderr)

    covered = sorted({req for test, ids in by_case.items() if test not in orphans for req in ids})
    print(f"{len(matched)} ctest entries tagged, {len(covered)} requirements covered: {', '.join(covered)}")

    return 1 if orphans and args.strict else 0


if __name__ == "__main__":
    sys.exit(main())
