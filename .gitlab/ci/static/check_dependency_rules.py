#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

"""
check_dependency_rules.py — Check a dependency tree against forbidden-include rules.

Reads the adjacency map written by gen_dependency_tree.py plus a rules file, and
reports every include edge a rule forbids. Complements include_directives_check.py:
that one enforces a single per-module allow-list; this one expresses rules the
allow-list has no vocabulary for — peer isolation between two sibling directories,
transitive reachability, and layering rules keyed on the *kind* of file (public
vs. private, production vs. test/app-wiring) rather than on module identity.

Two rule kinds:

  forbidden-edge (default)  `from` may not include `to`. Both accept a string
                            or a list of glob patterns.
  peer-isolation            For every pair of distinct directories matching
                            `peers`, files under one may not include files
                            under the other.

Globs are repo-relative; `*` does not cross `/`, `**` does.

The whole tree is always evaluated. `--changed-files` does not narrow the
evaluation, only the report: matching violations become findings, the rest are
summarised as pre-existing, and the exit code then reflects the changed files
alone.

Usage:
  python3 check_dependency_rules.py [options]

Options:
  --tree <path>                Dependency tree JSON from gen_dependency_tree.py.
                               Default: ./ocudu_dependency_tree.json.
  --rules <path>                Rules JSON. Default: ocudu_dependency_rules.json
                               beside this script.
  --changed-files <path> [..]  Repo-relative paths under review.
  --changed-files-from <path>  Read those paths from a file, one per line;
                               `-` reads stdin.
  --json                       Emit findings as JSON instead of text.

Exit codes:
  0  no violations (in the changed set, when one is given)
  1  violations found
  2  the ruleset or its inputs are broken (stale pattern, duplicate id,
     missing tree)
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from collections import deque
from pathlib import Path
from typing import NoReturn

RULE_KINDS = ("forbidden-edge", "peer-isolation")
COMMON_KEYS = {"id", "kind", "reason", "transitive", "exempt"}
KIND_KEYS = {
    "forbidden-edge": {"from", "to"},
    "peer-isolation": {"peers"},
}

errors: list[str] = []
warnings: list[str] = []


def fail(msg: str) -> NoReturn:
    sys.stderr.write(f"error: {msg}\n")
    sys.exit(2)


def glob_to_regex(pattern: str) -> re.Pattern:
    """Translate a path glob to a regex where `*` stops at `/` and `**` does not."""
    out = []
    i = 0
    while i < len(pattern):
        char = pattern[i]
        if char == "*":
            if pattern.startswith("**/", i):
                out.append("(?:.*/)?")
                i += 3
                continue
            if pattern.startswith("**", i):
                out.append(".*")
                i += 2
                continue
            out.append("[^/]*")
        elif char == "?":
            out.append("[^/]")
        else:
            out.append(re.escape(char))
        i += 1
    return re.compile("".join(out) + r"\Z")


class Matcher:
    """One or more globs, with the compiled patterns kept for error messages."""

    def __init__(self, patterns: list[str]):
        self.patterns = patterns
        self._regexes = [glob_to_regex(p) for p in patterns]

    def matches(self, path: str) -> bool:
        return any(rx.match(path) for rx in self._regexes)

    def unmatched(self, paths) -> list[str]:
        """Patterns that match nothing — a rule nobody can violate."""
        dead = []
        for pattern, rx in zip(self.patterns, self._regexes):
            if not any(rx.match(p) for p in paths):
                dead.append(pattern)
        return dead


def as_list(value, field: str, rule_id: str) -> list[str]:
    if isinstance(value, str):
        return [value]
    if isinstance(value, list) and all(isinstance(v, str) for v in value):
        return list(value)
    errors.append(f"rule '{rule_id}': '{field}' must be a string or a list of strings")
    return []


def load_tree(path: Path) -> dict:
    if not path.is_file():
        fail(
            f"{path}: no dependency tree. Generate it first:\n"
            f"  python3 gen_dependency_tree.py"
        )
    try:
        doc = json.loads(path.read_text())
    except (OSError, json.JSONDecodeError) as exc:
        fail(f"{path}: {exc}")
    if not isinstance(doc, dict) or not isinstance(doc.get("files"), dict):
        fail(f"{path}: not a dependency tree (missing a `files` mapping)")
    return doc


def load_rules(path: Path) -> list[dict]:
    if not path.is_file():
        fail(f"{path}: no such rules file")
    try:
        doc = json.loads(path.read_text())
    except (OSError, json.JSONDecodeError) as exc:
        fail(f"{path}: {exc}")
    if not isinstance(doc, dict):
        fail(f"{path}: top level must be an object with `version` and `rules`")
    if doc.get("version") != 1:
        fail(f"{path}: unsupported rules version {doc.get('version')!r} (expected 1)")
    rules = doc.get("rules")
    if not isinstance(rules, list) or not rules:
        fail(f"{path}: `rules` must be a non-empty list")
    return rules


def validate_rules(rules: list[dict], paths: set[str], dirs: set[str]) -> list[dict]:
    """Reject a ruleset that cannot do its job, rather than reporting a false green."""
    seen_ids: set[str] = set()
    prepared = []
    for index, rule in enumerate(rules):
        if not isinstance(rule, dict):
            errors.append(f"rule #{index + 1}: must be a mapping")
            continue
        rule_id = rule.get("id")
        if not isinstance(rule_id, str) or not rule_id:
            errors.append(f"rule #{index + 1}: missing `id`")
            continue
        if rule_id in seen_ids:
            errors.append(f"rule '{rule_id}': duplicate id")
            continue
        seen_ids.add(rule_id)

        kind = rule.get("kind", "forbidden-edge")
        if kind not in RULE_KINDS:
            errors.append(f"rule '{rule_id}': unknown kind '{kind}' (expected one of {', '.join(RULE_KINDS)})")
            continue
        reason = rule.get("reason")
        if not isinstance(reason, str) or not reason.strip():
            errors.append(f"rule '{rule_id}': missing `reason`")
            continue

        allowed = COMMON_KEYS | KIND_KEYS[kind]
        unknown = sorted(set(rule) - allowed)
        if unknown:
            errors.append(
                f"rule '{rule_id}': unknown key(s) {', '.join(unknown)} "
                f"(allowed for kind '{kind}': {', '.join(sorted(allowed))})"
            )
            continue

        prepared_rule = {
            "id": rule_id,
            "kind": kind,
            "reason": " ".join(reason.split()),
            "transitive": bool(rule.get("transitive", False)),
            "exempt": [],
        }

        if kind == "forbidden-edge":
            src = Matcher(as_list(rule.get("from"), "from", rule_id))
            dst = Matcher(as_list(rule.get("to"), "to", rule_id))
            for field, matcher in (("from", src), ("to", dst)):
                for dead in matcher.unmatched(paths):
                    errors.append(
                        f"rule '{rule_id}': '{field}' pattern '{dead}' matches no files "
                        f"— stale rule after a rename?"
                    )
            prepared_rule["from"] = src
            prepared_rule["to"] = dst
        else:
            peers = Matcher(as_list(rule.get("peers"), "peers", rule_id))
            for dead in peers.unmatched(dirs):
                errors.append(
                    f"rule '{rule_id}': 'peers' pattern '{dead}' matches no directories "
                    f"— stale rule after a rename?"
                )
            prepared_rule["peers"] = peers

        if kind == "peer-isolation" and prepared_rule["transitive"]:
            errors.append(
                f"rule '{rule_id}': `transitive` is not supported for kind 'peer-isolation'"
            )
            continue

        exempt = rule.get("exempt", [])
        if exempt is None:
            exempt = []
        if not isinstance(exempt, list):
            errors.append(f"rule '{rule_id}': `exempt` must be a list")
            continue
        for entry in exempt:
            if not isinstance(entry, dict) or set(entry) - {"from", "to", "reason"}:
                errors.append(f"rule '{rule_id}': each `exempt` entry needs from/to/reason only")
                continue
            src_path, dst_path = entry.get("from"), entry.get("to")
            if not isinstance(src_path, str) or not isinstance(dst_path, str):
                errors.append(f"rule '{rule_id}': `exempt` from/to must be exact paths")
                continue
            if not isinstance(entry.get("reason"), str) or not entry["reason"].strip():
                errors.append(f"rule '{rule_id}': exemption {src_path} -> {dst_path} needs a `reason`")
                continue
            for path in (src_path, dst_path):
                if path not in paths:
                    warnings.append(
                        f"rule '{rule_id}': exemption references '{path}', which is not in the "
                        f"tree — remove it?"
                    )
            prepared_rule["exempt"].append(
                {"from": src_path, "to": dst_path, "reason": entry["reason"], "used": False}
            )
        prepared.append(prepared_rule)
    return prepared


def ancestor_dirs(path: str) -> list[str]:
    parts = path.split("/")[:-1]
    return ["/".join(parts[: i + 1]) for i in range(len(parts))]


def peer_of(path: str, peers: Matcher) -> str | None:
    for directory in ancestor_dirs(path):
        if peers.matches(directory):
            return directory
    return None


def shortest_chain(start: str, edges: dict[str, list[str]], dst: Matcher) -> list[str] | None:
    """BFS for the shortest include chain from `start` to any `dst` match.

    Shortest, because the author can only act on the first edge and a shorter
    chain makes that edge easier to see.
    """
    prev: dict[str, str | None] = {start: None}
    queue = deque([start])
    while queue:
        node = queue.popleft()
        for nxt in edges.get(node, ()):
            if nxt in prev:
                continue
            prev[nxt] = node
            if dst.matches(nxt):
                chain = [nxt]
                walk = node
                while walk is not None:
                    chain.append(walk)
                    walk = prev[walk]
                chain.reverse()
                return chain
            queue.append(nxt)
    return None


def find_include_line(files: dict, source: str, target: str) -> int:
    """Line of the #include in `source` that produced the edge to `target`.

    The tree already carries this per edge (gen_dependency_tree.py records it
    at generation time), so — unlike the original version of this script —
    there is no need to re-read and re-scan the source file here.
    """
    entries = files.get(source, {}).get("includes", [])
    for entry in entries:
        if entry["target"] == target:
            return entry["line"]
    return entries[0]["line"] if entries else 1


def violations_for(rule: dict, edges: dict[str, list[str]]) -> list[dict]:
    found = []
    if rule["kind"] == "forbidden-edge":
        src, dst = rule["from"], rule["to"]
        for source in edges:
            if not src.matches(source):
                continue
            if rule["transitive"]:
                chain = shortest_chain(source, edges, dst)
                if chain:
                    found.append({"file": source, "to": chain[-1], "chain": chain})
                continue
            for target in edges[source]:
                if dst.matches(target):
                    found.append({"file": source, "to": target, "chain": [source, target]})
    else:
        peers = rule["peers"]
        for source, targets in edges.items():
            source_peer = peer_of(source, peers)
            if source_peer is None:
                continue
            for target in targets:
                target_peer = peer_of(target, peers)
                if target_peer is None or target_peer == source_peer:
                    continue
                found.append({"file": source, "to": target, "chain": [source, target]})
    return found


def main() -> int:
    parser = argparse.ArgumentParser(add_help=True, description=__doc__)
    parser.add_argument("--tree")
    parser.add_argument("--rules")
    parser.add_argument("--changed-files", nargs="+", default=[])
    parser.add_argument("--changed-files-from")
    parser.add_argument("--json", action="store_true")
    args = parser.parse_args()

    script_dir = Path(__file__).resolve().parent
    rules_path = Path(args.rules) if args.rules else script_dir / "ocudu_dependency_rules.json"
    tree_path = Path(args.tree) if args.tree else Path.cwd() / "ocudu_dependency_tree.json"

    doc = load_tree(tree_path)
    meta = doc.get("meta") or {}

    files = doc["files"]
    edges = {path: [inc["target"] for inc in entry.get("includes") or []] for path, entry in files.items()}
    # Include-only targets (external/, third-party) are never scanned, so they
    # appear as edge targets and never as keys. Rules may legitimately name
    # them, so staleness validation has to see them too.
    known_paths = set(files).union(*edges.values()) if edges else set(files)
    # Directories are derived from file paths, so `peers` validation sees exactly
    # the directories the tree actually covers.
    known_dirs = {d for path in known_paths for d in ancestor_dirs(path)}

    rules = validate_rules(load_rules(rules_path), known_paths, known_dirs)
    if errors:
        for line in errors:
            sys.stderr.write(f"error: {line}\n")
        return 2

    changed: set[str] = set(args.changed_files)
    if args.changed_files_from:
        if args.changed_files_from == "-":
            raw = sys.stdin.read()
        else:
            source = Path(args.changed_files_from)
            if not source.is_file():
                fail(f"{source}: no such file")
            raw = source.read_text()
        changed.update(line.strip() for line in raw.splitlines() if line.strip())
    scoped = bool(changed)

    findings: list[dict] = []
    for rule in rules:
        for hit in violations_for(rule, edges):
            exemption = next(
                (e for e in rule["exempt"] if e["from"] == hit["file"] and e["to"] == hit["to"]),
                None,
            )
            if exemption:
                exemption["used"] = True
                continue
            anchor = hit["chain"][1] if len(hit["chain"]) > 1 else hit["to"]
            findings.append({
                "rule_id": rule["id"],
                "kind": rule["kind"],
                "file": hit["file"],
                "line": find_include_line(files, hit["file"], anchor),
                "from": hit["file"],
                "to": hit["to"],
                "chain": hit["chain"],
                "reason": rule["reason"],
                "in_changed_set": hit["file"] in changed,
            })

    for rule in rules:
        for exemption in rule["exempt"]:
            if not exemption["used"]:
                warnings.append(
                    f"rule '{rule['id']}': exemption {exemption['from']} -> {exemption['to']} "
                    f"no longer matches a violation — remove it?"
                )

    reported = [f for f in findings if f["in_changed_set"]] if scoped else findings
    pre_existing = len(findings) - len(reported) if scoped else 0

    if args.json:
        json.dump(
            {
                "tree": tree_path.as_posix(),
                "rules": rules_path.as_posix(),
                "rule_count": len(rules),
                "file_count": meta.get("file_count", len(files)),
                "edge_count": meta.get("edge_count"),
                "unresolved_count": meta.get("unresolved_count"),
                "scoped": scoped,
                "pre_existing_count": pre_existing,
                "warnings": warnings,
                "findings": reported,
            },
            sys.stdout, indent=2, sort_keys=False,
        )
        sys.stdout.write("\n")
    else:
        for finding in sorted(reported, key=lambda f: (f["rule_id"], f["file"], f["line"])):
            sys.stdout.write(
                f"{finding['file']}:{finding['line']} — {finding['rule_id']} — "
                f"{' -> '.join(finding['chain'])}\n"
            )
        for line in warnings:
            sys.stdout.write(f"warning: {line}\n")
        scope = "changed files" if scoped else "whole tree"
        sys.stdout.write(
            f"\n{len(reported)} violation(s) in {scope}; {len(rules)} rule(s) over "
            f"{meta.get('file_count', len(files))} files, {meta.get('edge_count')} edges, "
            f"{meta.get('unresolved_count')} unresolved.\n"
        )
        if scoped and pre_existing:
            sys.stdout.write(f"{pre_existing} pre-existing violation(s) elsewhere (not in this diff).\n")

    return 1 if reported else 0


if __name__ == "__main__":
    sys.exit(main())
