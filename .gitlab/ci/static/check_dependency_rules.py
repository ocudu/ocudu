#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

"""
check_dependency_rules.py — Check a dependency tree against dependency rules.

Reads the adjacency map written by gen_dependency_tree.py plus a rules file, and
reports every include edge a rule forbids.

Four rule kinds:

  forbidden-edge            `from` may not include `to`. Both accept a string
                            or a list of glob patterns.
  allowed-edge              The inverse: `from` may *only* include `allow`
                            (plus the top-level `always_allowed` list) —
                            anything else is a violation. `from_exclude`
                            carves out a subset of `from` handled by a more
                            specific rule instead (e.g. a submodule).
                            `extra_allow_for_prefix` widens `allow` further
                            for files whose name starts with a given prefix,
                            without introducing a second rule that could
                            independently pass or fail the same file.
  peer-isolation            For every pair of distinct directories matching
                            `peers`, files under one may not include files
                            under the other.
  no-relative-includes      Files matching `from` may not use a `..`
                            component in their #include text.

`from`, `to`, `allow`, `from_exclude`, and `always_allowed` entries are
repo-relative globs (`*` does not cross `/`, `**` does).

The whole tree is always evaluated. `--changed-files` does not narrow the
evaluation, only the report: matching violations become findings, the rest are
summarised as pre-existing, and the exit code then reflects the changed files
alone.

Usage:
  python3 check_dependency_rules.py [options]

Options:
  --tree <path>                Dependency tree YAML from gen_dependency_tree.py.
                               Default: ./ocudu_dependency_tree.yml.
  --rules <path>                Rules YAML. Default: ocudu_dependency_rules.yml
                               beside this script.
  --repo <path>                Project root, for re-reading a line to check for
                               a relative include. Default: the tree's
                               meta.repo_root.
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

try:
    import yaml
except ImportError:
    sys.stderr.write("error: PyYAML required: pip install pyyaml\n")
    sys.exit(2)

LOADER = getattr(yaml, "CSafeLoader", yaml.SafeLoader)

RULE_KINDS = ("forbidden-edge", "allowed-edge", "peer-isolation", "no-relative-includes")
COMMON_KEYS = {"id", "kind", "reason", "transitive", "exempt"}
KIND_KEYS = {
    "forbidden-edge": {"from", "to"},
    "allowed-edge": {"from", "from_exclude", "allow", "extra_allow_for_prefix"},
    "peer-isolation": {"peers"},
    "no-relative-includes": {"from"},
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
        doc = yaml.load(path.read_text(), Loader=LOADER)
    except (OSError, yaml.YAMLError) as exc:
        fail(f"{path}: {exc}")
    if not isinstance(doc, dict) or not isinstance(doc.get("files"), dict):
        fail(f"{path}: not a dependency tree (missing a `files` mapping)")
    return doc


def load_rules(path: Path) -> dict:
    if not path.is_file():
        fail(f"{path}: no such rules file")
    try:
        doc = yaml.load(path.read_text(), Loader=LOADER)
    except (OSError, yaml.YAMLError) as exc:
        fail(f"{path}: {exc}")
    if not isinstance(doc, dict):
        fail(f"{path}: top level must be a mapping with `version` and `rules`")
    if doc.get("version") != 1:
        fail(f"{path}: unsupported rules version {doc.get('version')!r} (expected 1)")
    rules = doc.get("rules")
    if not isinstance(rules, list) or not rules:
        fail(f"{path}: `rules` must be a non-empty list")
    return doc


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

        kind = rule.get("kind")
        if kind is None:
            errors.append(f"rule '{rule_id}': missing `kind` (expected one of {', '.join(RULE_KINDS)})")
            continue
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

        def file_matcher(field: str) -> "Matcher":
            matcher = Matcher(as_list(rule.get(field), field, rule_id))
            for dead in matcher.unmatched(paths):
                errors.append(
                    f"rule '{rule_id}': '{field}' pattern '{dead}' matches no files "
                    f"— stale rule after a rename?"
                )
            return matcher

        if kind == "forbidden-edge":
            prepared_rule["from"] = file_matcher("from")
            prepared_rule["to"] = file_matcher("to")
        elif kind == "allowed-edge":
            prepared_rule["from"] = file_matcher("from")
            prepared_rule["from_exclude"] = (
                file_matcher("from_exclude") if rule.get("from_exclude") is not None else Matcher([])
            )
            prepared_rule["allow"] = file_matcher("allow")
            extras = rule.get("extra_allow_for_prefix", []) or []
            if not isinstance(extras, list):
                errors.append(f"rule '{rule_id}': `extra_allow_for_prefix` must be a list")
                extras = []
            prepared_extras = []
            for entry in extras:
                if not isinstance(entry, dict) or set(entry) != {"prefix", "allow"}:
                    errors.append(f"rule '{rule_id}': each `extra_allow_for_prefix` entry needs exactly prefix/allow")
                    continue
                prefix = entry.get("prefix")
                if not isinstance(prefix, str) or not prefix:
                    errors.append(f"rule '{rule_id}': `extra_allow_for_prefix` entry missing a string `prefix`")
                    continue
                field_name = f"extra_allow_for_prefix[{prefix}].allow"
                allow_matcher = Matcher(as_list(entry.get("allow"), field_name, rule_id))
                for dead in allow_matcher.unmatched(paths):
                    errors.append(
                        f"rule '{rule_id}': '{field_name}' pattern '{dead}' matches no files "
                        f"— stale rule after a rename?"
                    )
                prepared_extras.append((prefix, allow_matcher))
            prepared_rule["extra_allow_for_prefix"] = prepared_extras
        elif kind == "peer-isolation":
            peers = Matcher(as_list(rule.get("peers"), "peers", rule_id))
            for dead in peers.unmatched(dirs):
                errors.append(
                    f"rule '{rule_id}': 'peers' pattern '{dead}' matches no directories "
                    f"— stale rule after a rename?"
                )
            prepared_rule["peers"] = peers
        else:  # no-relative-includes
            prepared_rule["from"] = file_matcher("from")

        if kind in ("peer-isolation", "allowed-edge", "no-relative-includes") and prepared_rule["transitive"]:
            errors.append(
                f"rule '{rule_id}': `transitive` is not supported for kind '{kind}'"
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


def is_relative_include(repo: Path, source: str, line: int) -> bool:
    """True if the #include on this line of `source` uses a `..` component."""
    try:
        text = (repo / source).read_text(errors="replace").splitlines()[line - 1]
    except (OSError, IndexError):
        return False
    stripped = text.split("include", 1)
    if len(stripped) < 2:
        return False
    quote = stripped[1].strip().lstrip("#").strip()
    for opener, closer in (('"', '"'), ("<", ">")):
        if quote.startswith(opener):
            end = quote.find(closer, 1)
            if end != -1:
                return ".." in Path(quote[1:end]).parts
    return False


def violations_for(
    rule: dict, edges: dict[str, list[str]], files: dict, always_allowed: "Matcher", repo: Path,
) -> list[dict]:
    found = []
    kind = rule["kind"]
    if kind == "forbidden-edge":
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
    elif kind == "allowed-edge":
        src, exclude, allow = rule["from"], rule["from_exclude"], rule["allow"]
        for source in edges:
            if not src.matches(source) or exclude.matches(source):
                continue
            extra = next(
                (m for prefix, m in rule["extra_allow_for_prefix"] if Path(source).name.startswith(prefix)),
                None,
            )
            for target in edges[source]:
                if allow.matches(target) or always_allowed.matches(target):
                    continue
                if extra is not None and extra.matches(target):
                    continue
                found.append({"file": source, "to": target, "chain": [source, target]})
    elif kind == "no-relative-includes":
        src = rule["from"]
        for source in edges:
            if not src.matches(source):
                continue
            for inc in files.get(source, {}).get("includes", []):
                if is_relative_include(repo, source, inc["line"]):
                    found.append({"file": source, "to": inc["target"], "chain": [source, inc["target"]]})
    else:  # peer-isolation
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
    parser.add_argument("--repo", help="project root, for re-reading a line to check for a relative include")
    parser.add_argument("--changed-files", nargs="+", default=[])
    parser.add_argument("--changed-files-from")
    parser.add_argument("--json", action="store_true")
    args = parser.parse_args()

    script_dir = Path(__file__).resolve().parent
    rules_path = Path(args.rules) if args.rules else script_dir / "ocudu_dependency_rules.yml"
    tree_path = Path(args.tree) if args.tree else Path.cwd() / "ocudu_dependency_tree.yml"

    doc = load_tree(tree_path)
    meta = doc.get("meta") or {}
    repo = Path(args.repo).resolve() if args.repo else Path(meta.get("repo_root", "."))

    files = doc["files"]
    edges = {path: [inc["target"] for inc in entry.get("includes") or []] for path, entry in files.items()}
    # Include-only targets (external/, third-party) are never scanned, so they
    # appear as edge targets and never as keys. Rules may legitimately name
    # them, so staleness validation has to see them too.
    known_paths = set(files).union(*edges.values()) if edges else set(files)
    # Directories are derived from file paths, so `peers` validation sees exactly
    # the directories the tree actually covers.
    known_dirs = {d for path in known_paths for d in ancestor_dirs(path)}

    rules_doc = load_rules(rules_path)
    rules = validate_rules(rules_doc.get("rules", []), known_paths, known_dirs)
    always_allowed = Matcher(as_list(rules_doc.get("always_allowed", []), "always_allowed", "<top-level>"))
    for dead in always_allowed.unmatched(known_paths):
        errors.append(f"'always_allowed' pattern '{dead}' matches no files — stale rule after a rename?")
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
        for hit in violations_for(rule, edges, files, always_allowed, repo):
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
