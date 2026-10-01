#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

"""Fail-closed validators for the CodeChecker wrapper (static-analyzer.sh).

Subcommands, each reading a file the wrapper produced:

  manifest <unique_compile_commands.json>
      Report whether the analysis scheduled any action. CodeChecker writes this file only when it has work to do.
        0  at least one action was scheduled (a non-empty JSON list)
        2  no action was scheduled (file absent or an empty list)
        1  file present but unreadable or malformed
      A 0%-coverage run is a failure by default; only a caller that passes --allow-no-actions may accept exit 2, and
      exit 1 stays fatal for everyone because corruption is never a legitimate "nothing to do".

  metadata <metadata.json> <unique_compile_commands.json> <expected_analyzer> <allow_ctu_skip>
      Exit 0 if the requested analyzer accounted for every scheduled action, 1 otherwise (reason on stderr). Requires
      metadata action_num == manifest length (both are len(the same actions list), so a mismatch means the two files
      describe different runs), exactly one record for the requested analyzer, no failures, and 0 < successful <=
      action_num. successful must equal action_num unless allow_ctu_skip is "1" (set by the wrapper only for the actual
      clangsa --ctu + skiplist path, where CodeChecker counts skip-listed actions in action_num but does not run them).

  report <code-quality-report.json>
      Validate the GitLab Code Quality report and look for major-or-higher findings. Disjoint exit codes that skip 1, so
      an unhandled exception (Python exit 1) can never be mistaken for a "valid, fatal" report and published:
        0  valid, no fatal findings
        10 valid, at least one major/critical/blocker finding
        20 report does not match the GitLab Code Quality schema
        30 validator internal error (fail closed)
"""

import json
import os
import sys

EXIT_CLEAN = 0
EXIT_NO_ACTIONS = 2
EXIT_FATAL = 10
EXIT_SCHEMA_INVALID = 20
EXIT_INTERNAL_ERROR = 30


def _load(path):
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def _nonneg_int(value, what):
    if not isinstance(value, int) or isinstance(value, bool) or value < 0:
        raise ValueError("non-negative-integer %s expected, got %r" % (what, value))
    return value


def cmd_manifest(path):
    # An absent file and an empty list both mean "nothing was scheduled"; malformed content is corruption, not silence.
    try:
        data = _load(path)
    except OSError:
        return EXIT_NO_ACTIONS
    except ValueError:
        return 1
    if not isinstance(data, list):
        return 1
    return EXIT_CLEAN if data else EXIT_NO_ACTIONS


def cmd_metadata(meta_path, manifest_path, expected, allow_ctu_skip):
    def die(msg):
        print("ERROR: incomplete CodeChecker analysis: " + msg, file=sys.stderr)
        return 1

    if not expected:
        return die("no expected analyzer given; cannot verify completeness")
    # The manifest and metadata action_num both come from the same CodeChecker `actions` list
    # (unique_compile_commands.json is json.dump(actions); action_num is len(actions)), so they must agree. Reading the
    # manifest here proves the metadata describes THIS run instead of validating the two files independently.
    try:
        manifest = _load(manifest_path)
    except (OSError, ValueError) as error:
        return die("action manifest not readable as JSON (%s)" % error)
    if not isinstance(manifest, list) or not manifest:
        return die("action manifest is empty or not a list")
    action_count = len(manifest)

    try:
        meta = _load(meta_path)
    except (OSError, ValueError) as error:
        return die("metadata.json not readable as JSON (%s)" % error)

    tools = meta.get("tools")
    if not isinstance(tools, list) or not tools:
        return die("metadata.tools is not a non-empty list")

    records = []  # analyzer_statistics for each tools[] entry that ran the requested analyzer
    try:
        for tool in tools:
            if not isinstance(tool, dict):
                return die("metadata.tools entry is not an object")
            action_num = _nonneg_int(tool.get("action_num"), "action_num")
            if action_num != action_count:
                return die("metadata action_num=%d but the manifest lists %d actions" % (action_num, action_count))
            analyzers = tool.get("analyzers")
            if not isinstance(analyzers, dict):
                return die("metadata.tools entry has no analyzers object")
            if expected in analyzers:
                rec = analyzers[expected]
                stats = rec.get("analyzer_statistics") if isinstance(rec, dict) else None
                if not isinstance(stats, dict):
                    return die("%s record has no analyzer_statistics object" % expected)
                records.append(stats)

        if len(records) != 1:
            return die("expected exactly one %r analyzer record, found %d" % (expected, len(records)))

        stats = records[0]
        successful = _nonneg_int(stats.get("successful"), "successful")
        failed = _nonneg_int(stats.get("failed"), "failed")
    except ValueError as error:
        return die(str(error))

    if failed != 0:
        return die("%s reported %d failed action(s)" % (expected, failed))
    if successful == 0:
        return die("%s executed zero actions" % expected)
    if successful > action_count:
        return die("%s reported %d successes for %d scheduled actions" % (expected, successful, action_count))
    # Completeness: normally every scheduled action runs, so successful == action_count. clangsa --ctu is the exception:
    # it keeps the skip-listed actions in action_num for CTU pre-analysis but the analysis itself skips them, so
    # successful < action_count is healthy there. allow_ctu_skip is set by the wrapper only when the invocation is
    # actually clangsa with --ctu and a skip list; every other run must account for every scheduled action.
    if not allow_ctu_skip and successful != action_count:
        return die("%s executed %d of %d scheduled actions" % (expected, successful, action_count))
    return 0


def cmd_report(path):
    # Test-only fault injection: force the internal-error path to prove any unexpected exception fails closed as 30
    # (never a silent success or Python's exit 1). Unset in production; if ever set it only makes the gate fail closed.
    if os.environ.get("CC_TEST_FORCE_INTERNAL_ERROR"):
        raise RuntimeError("forced validator error (test hook)")

    def invalid(message):
        print("ERROR: invalid Code Quality report: " + message, file=sys.stderr)
        raise SystemExit(EXIT_SCHEMA_INVALID)

    def reject_constant(value):
        # json.load() accepts NaN/Infinity/-Infinity by default; the GitLab Code Quality parser and RFC 8259 do not.
        raise ValueError("non-standard JSON constant %r" % value)

    try:
        with open(path, encoding="utf-8") as report_file:
            findings = json.load(report_file, parse_constant=reject_constant)
    except (OSError, ValueError) as error:
        invalid("not readable as JSON (%s)" % error)

    if not isinstance(findings, list):
        invalid("root is not a JSON array")

    allowed = {"info", "minor", "major", "critical", "blocker"}
    fatal = {"major", "critical", "blocker"}
    has_fatal = False

    for index, finding in enumerate(findings):
        if not isinstance(finding, dict):
            invalid("finding %d is not an object" % index)
        for field in ("description", "check_name", "fingerprint"):
            if not isinstance(finding.get(field), str) or not finding[field]:
                invalid("finding %d has an invalid %s" % (index, field))
        severity = finding.get("severity")
        # isinstance guard first: a non-string severity (list, object, number, bool, null) is not a valid GitLab
        # severity, and "severity not in allowed" would raise TypeError on an unhashable list/dict.
        if not isinstance(severity, str) or severity not in allowed:
            invalid("finding %d has an invalid severity %r" % (index, severity))
        location = finding.get("location")
        if not isinstance(location, dict):
            invalid("finding %d has no location object" % index)
        path_value = location.get("path")
        if not isinstance(path_value, str) or not path_value:
            invalid("finding %d has an invalid location.path" % index)
        # GitLab requires a repo-relative path not prefixed with "./".
        if path_value.startswith("/") or path_value.startswith("./") or ".." in path_value.split("/"):
            invalid("finding %d has a non-repo-relative location.path %r" % (index, path_value))
        lines = location.get("lines")
        begin = lines.get("begin") if isinstance(lines, dict) else None
        if begin is None:
            positions = location.get("positions")
            begin_pos = positions.get("begin") if isinstance(positions, dict) else None
            begin = begin_pos.get("line") if isinstance(begin_pos, dict) else None
        if not isinstance(begin, int) or isinstance(begin, bool) or begin < 1:
            invalid("finding %d has an invalid begin line" % index)
        if severity in fatal:
            has_fatal = True

    raise SystemExit(EXIT_FATAL if has_fatal else EXIT_CLEAN)


def main(argv):
    if len(argv) < 2:
        print(
            "usage: validate_analysis.py manifest <manifest> | "
            "metadata <metadata> <manifest> <analyzer> <allow_ctu_skip> | report <report>",
            file=sys.stderr,
        )
        return EXIT_INTERNAL_ERROR
    sub = argv[1]
    try:
        if sub == "manifest":
            return cmd_manifest(argv[2])
        if sub == "metadata":
            expected = argv[4] if len(argv) > 4 else os.environ.get("ANALYZER", "")
            allow_ctu_skip = len(argv) > 5 and argv[5] == "1"
            return cmd_metadata(argv[2], argv[3], expected, allow_ctu_skip)
        if sub == "report":
            cmd_report(argv[2])  # raises SystemExit with the tri-state code
            return EXIT_INTERNAL_ERROR  # unreachable
        print("ERROR: unknown subcommand %r" % sub, file=sys.stderr)
        return EXIT_INTERNAL_ERROR
    except SystemExit:
        raise
    except BaseException as error:  # fail closed: any unexpected error is an internal error, never a silent success
        print("ERROR: validate_analysis internal error: %r" % error, file=sys.stderr)
        return EXIT_INTERNAL_ERROR


if __name__ == "__main__":
    sys.exit(main(sys.argv))
