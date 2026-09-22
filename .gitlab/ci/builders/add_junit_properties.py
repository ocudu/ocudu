#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

"""
Add the CI properties, and the requirement tags the tests recorded, to a CTest JUnit XML file.

Every <testsuite> element gets a <properties> block naming the commit and the job. When
--gtest-xml-dir is given, the requirement identifiers that the tests recorded with
OCUDU_TEST_REQUIREMENTS are also copied onto the matching <testcase> elements: each test process
writes its own gtest XML report (see cmake/scripts/gtest_xml_launcher.sh), and the ids travel from
there. CTest stays the authority on pass, fail, crash and timeout; this only adds metadata its JUnit
output cannot carry.

Usage:
  add_junit_properties.py <xunit.xml> [--commit <hash>] [--gtest-xml-dir <dir>] [--strict]

If --commit is omitted, the value is read from the OCUDU_COMMIT environment variable.
"""

import argparse
import os
import re
import sys
import xml.etree.ElementTree as ET
from collections import defaultdict
from pathlib import Path

REQUIREMENTS = "requirements"

# CTest escapes "<" in test output, so a testcase element cannot contain its own end tag.
TESTCASE_RE = re.compile(r"(<testcase\b[^>]*?/>)|(<testcase\b[^>]*>)(.*?)(</testcase>)", re.DOTALL)


def _requirements_of(testcase) -> set[str]:
    """Read the requirement ids off a gtest <testcase>, as a property or as a legacy attribute."""
    raw = testcase.get(REQUIREMENTS, "")

    props = testcase.find("properties")
    if props is not None:
        for prop in props.findall("property"):
            if prop.get("name") == REQUIREMENTS:
                raw = prop.get("value", "")

    return {value.strip() for value in raw.split(";") if value.strip()}


def collect_requirements(gtest_xml_dir: Path) -> tuple[dict, dict, dict]:
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
            key = f"{testcase.get('classname', '')}.{testcase.get('name', '')}"
            by_case[key] |= ids
            by_binary[binary] |= ids
            case_binary[key] = binary

    return by_case, by_binary, case_binary


def _inject_property(body: str, ids: set[str]) -> str:
    """Add a requirements property to the <properties> block of one testcase body."""
    prop = f'<property name="{REQUIREMENTS}" value="{";".join(sorted(ids))}"/>'

    # CTest writes a <properties> element on every testcase, empty when the test carries no label.
    if "<properties/>" in body:
        return body.replace("<properties/>", f"<properties>{prop}</properties>", 1)
    if "<properties>" in body:
        return body.replace("<properties>", f"<properties>{prop}", 1)
    return body + f"<properties>{prop}</properties>"


def add_requirements(content: str, by_case: dict, by_binary: dict) -> tuple[str, set[str]]:
    """Copy the recorded ids onto the matching testcases. Returns the patched text and what matched."""
    matched: set[str] = set()

    def patch(match: re.Match[str]) -> str:
        self_closed, open_tag, body, close_tag = match.groups()
        tag = self_closed or open_tag
        name_match = re.search(r'name="([^"]*)"', tag)
        name = name_match.group(1) if name_match else ""

        # A target registered with gtest_discover_tests has one ctest entry per case, named after it.
        # A target registered as a single entry is named after the binary, and collects everything its
        # tests recorded.
        ids = by_case.get(name) or by_binary.get(name)
        if not ids:
            return match.group(0)

        matched.add(name)
        if self_closed:
            prop = f'<property name="{REQUIREMENTS}" value="{";".join(sorted(ids))}"/>'
            return f"{self_closed[:-2]}><properties>{prop}</properties></testcase>"
        return f"{open_tag}{_inject_property(body, ids)}{close_tag}"

    return TESTCASE_RE.sub(patch, content), matched


def add_ci_properties(content: str, args: argparse.Namespace) -> str:
    """Add a <properties> block naming the commit and the job to every <testsuite> element."""
    properties = [("ocudu_commit", args.commit), ("test_commit", args.test_commit), ("url", args.url)]
    prop_block = (
        "<properties>" + "".join(f'<property name="{k}" value="{v}"/>' for k, v in properties) + "</properties>"
    )

    def patch_testsuite(m):
        tag = m.group(0)
        if args.suite_name:
            # Anchored, so that the trailing "name=" of hostname= is not renamed along with it.
            tag = re.sub(r'(?<![\w-])name="[^"]*"', f'name="{args.suite_name}"', tag, count=1)
        return tag + prop_block

    return re.sub(r"(<testsuite\b[^>]*>)", patch_testsuite, content)


def main() -> int:
    """Add the CI properties, and optionally the recorded requirement tags, to a CTest JUnit file."""
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("junit_xml", type=Path, help="Path to the JUnit XML file to modify in-place")
    parser.add_argument(
        "--suite-name", default=os.environ.get("CI_JOB_NAME", ""), help="Override testsuite name attribute"
    )
    parser.add_argument("--commit", default=os.environ.get("OCUDU_COMMIT", ""), help="OCUDU commit hash")
    parser.add_argument("--test-commit", default=os.environ.get("CI_COMMIT_SHA", ""), help="CI commit hash")
    parser.add_argument("--url", default=os.environ.get("CI_JOB_URL", ""), help="CI job URL")
    parser.add_argument(
        "--gtest-xml-dir",
        type=Path,
        default=os.environ.get("OCUDU_GTEST_XML_DIR") or None,
        help="Folder holding the per-process gtest XML reports carrying the requirement tags",
    )
    parser.add_argument("--strict", action="store_true", help="Fail if a recorded requirement reached no entry")
    args = parser.parse_args()

    content = args.junit_xml.read_text(encoding="utf-8")
    content = add_ci_properties(content, args)

    ret = 0
    if args.gtest_xml_dir:
        if not args.gtest_xml_dir.is_dir():
            parser.error(f"Not a directory: {args.gtest_xml_dir}")

        by_case, by_binary, case_binary = collect_requirements(args.gtest_xml_dir)
        if not by_case:
            print("WARNING: no requirement tags found. Was OCUDU_GTEST_XML_DIR set for the test run?", file=sys.stderr)
            ret = 1 if args.strict else 0
        else:
            content, matched = add_requirements(content, by_case, by_binary)

            # A tag that reaches no entry would leave the requirement looking untested, so it is worth
            # saying out loud rather than letting the report show a silent gap. A case is covered
            # either by its own ctest entry or by the whole-binary entry it ran under.
            orphans = {t for t in by_case if t not in matched and case_binary[t] not in matched}
            for test in sorted(orphans):
                print(f"WARNING: {test} recorded {sorted(by_case[test])} but matched no ctest entry", file=sys.stderr)

            covered = sorted({req for t, ids in by_case.items() if t not in orphans for req in ids})
            print(f"{len(matched)} ctest entries tagged, {len(covered)} requirements covered: {', '.join(covered)}")
            ret = 1 if orphans and args.strict else 0

    args.junit_xml.write_text(content, encoding="utf-8")
    return ret


if __name__ == "__main__":
    sys.exit(main())
