#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

"""
gen_dependency_tree.py — Emit the #include graph of the OCUDU source tree as YAML.

Scans every source file under the project's source roots and records what each
one includes, resolving each directive against the including file's own
directory first and then the repo's fixed, unconditional include roots
(the ones CMakeLists.txt adds via a plain `include_directories()`, independent
of any ENABLE_* option: `include`, `external/fmt/include`, `external`).

Deliberately not compile_commands.json-driven: no single cmake configuration
enables every optional backend at once (some, like ARMPL and MKL, are
mutually-exclusive alternatives), so a compiler-flag-driven resolver would
only ever see whichever subset one particular build enabled. Resolving
against the fixed roots instead covers every file regardless of build
configuration, at the cost of not being compiler-exact for the
conditionally-added system search paths (UHD, DPDK, ZeroMQ, NUMA) — those are
third-party headers the caller is expected to allow-list by name rather than
resolve by path.

Output is a flat per-file adjacency map: each edge is the resolved
repo-relative path plus the line number of the #include that produced it, so
a consumer can re-read that exact line for anything (a raw include-path
convention check) this tool doesn't itself interpret. Unresolved directives
are kept separately, by line, rather than silently dropped.

Usage:
  python3 gen_dependency_tree.py [options]

Options:
  --repo <path>              Project root. Default: git toplevel of the cwd.
  --output <path>            Output file. Default: <repo>/ocudu_dependency_tree.yml.
  --roots <dir> [...]        Source roots to scan, repo-relative. Default:
                             every top-level directory holding sources, minus
                             the excludes.
  --exclude <pattern> [...]  Extra top-level names to skip (glob, matched
                             against the directory name).
  --quiet                    Suppress the summary line on stderr.

Exit codes:
  0  tree written
  2  bad input (unreadable repo, empty scan)
"""

from __future__ import annotations

import argparse
import fnmatch
import re
import subprocess
import sys
from pathlib import Path
from typing import NoReturn

try:
    import yaml
except ImportError:
    sys.stderr.write("error: PyYAML required: pip install pyyaml\n")
    sys.exit(2)

DUMPER = getattr(yaml, "CSafeDumper", yaml.SafeDumper)

SOURCE_SUFFIXES = {".h", ".hpp", ".hh", ".hxx", ".c", ".cc", ".cpp", ".cxx"}

# Top-level names never scanned for outgoing edges. Third-party and generated
# trees stay valid include *targets* — nobody is going to fix their includes,
# so recording their own dependencies is noise.
DEFAULT_EXCLUDES = (".*", "external", "build", "build*", "cmake-build*", "ccache*", "Testing")

# The include roots the build adds unconditionally: `include`, `external/fmt/include`
# and `external` globally (CMakeLists.txt:672-674), and the repo root itself, which
# nearly every lib/apps/tests CMakeLists.txt adds per-target via
# `target_include_directories(... PRIVATE ${CMAKE_SOURCE_DIR} ...)` — present
# regardless of which ENABLE_* options a given build turns on.
FIXED_INCLUDE_ROOTS = (".", "include", "external/fmt/include", "external")

INCLUDE_RE = re.compile(r'^[ \t]*#[ \t]*include[ \t]*([<"])([^">]+)[>"]')


def fail(msg: str) -> NoReturn:
    sys.stderr.write(f"error: {msg}\n")
    sys.exit(2)


def git_toplevel() -> Path | None:
    try:
        out = subprocess.run(
            ["git", "rev-parse", "--show-toplevel"],
            capture_output=True, text=True, check=True,
        ).stdout.strip()
    except (subprocess.CalledProcessError, FileNotFoundError):
        return None
    return Path(out) if out else None


def pick_roots(repo: Path, given: list[str] | None, excludes: list[str]) -> list[Path]:
    if given:
        roots = []
        for name in given:
            path = repo / name
            if not path.is_dir():
                fail(f"{path}: no such source root")
            roots.append(path)
        return roots

    roots = []
    for entry in sorted(repo.iterdir()):
        if not entry.is_dir():
            continue
        if any(fnmatch.fnmatch(entry.name, pat) for pat in excludes):
            continue
        roots.append(entry)
    return roots


def collect_sources(roots: list[Path]) -> list[Path]:
    files = []
    for root in roots:
        for path in root.rglob("*"):
            if path.suffix in SOURCE_SUFFIXES and path.is_file():
                files.append(path)
    return files


def nearest_target_dir(source_dir: Path, repo: Path) -> Path:
    """The directory of the nearest CMakeLists.txt at or above `source_dir`.

    A handful of lib/ submodules (e1ap/cu_cp, e1ap/cu_up, f1ap/cu, f1ap/du, ...)
    declare `target_include_directories(<target> PRIVATE ..)` in their own
    CMakeLists.txt, adding their *own* parent directory as a root for every
    source the target compiles, however deeply that source is nested under it.
    Finding the CMakeLists.txt that owns a source file — rather than just
    walking that file's own parent chain — is what makes that root apply
    uniformly the way the real build does.
    """
    current = source_dir
    while True:
        if (current / "CMakeLists.txt").is_file():
            return current
        if current == repo or current.parent == current:
            return source_dir
        current = current.parent


class Resolver:
    """Resolves include directives to repo-relative paths, same-directory first."""

    def __init__(self, repo: Path, include_roots: list[Path]):
        self.repo = repo
        self.include_roots = include_roots
        self._cache: dict[tuple[str, str], str | None] = {}
        self._target_dir_cache: dict[Path, Path] = {}

    def _to_repo_path(self, path: Path) -> str:
        resolved = path.resolve()
        try:
            return resolved.relative_to(self.repo).as_posix()
        except ValueError:
            return resolved.as_posix()

    def _candidate_roots(self, source_dir: Path) -> list[Path]:
        target_dir = self._target_dir_cache.get(source_dir)
        if target_dir is None:
            target_dir = nearest_target_dir(source_dir, self.repo)
            self._target_dir_cache[source_dir] = target_dir
        # Two more common idioms besides same-directory, both scoped to the
        # CMakeLists.txt that owns this source: `target_include_directories(t
        # PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})` (the target's own dir — e.g.
        # lib/e2) and `target_include_directories(t PRIVATE ..)` (its parent —
        # e.g. lib/e1ap for a target declared in lib/e1ap/cu_cp).
        roots = [source_dir]
        for extra in (target_dir, target_dir.parent):
            if extra not in roots:
                roots.append(extra)
        roots.extend(self.include_roots)
        return roots

    def resolve(self, source: Path, target: str) -> str | None:
        key = (str(source.parent), target)
        if key in self._cache:
            return self._cache[key]
        result = None
        for base in self._candidate_roots(source.parent):
            candidate = base / target
            if candidate.is_file():
                result = self._to_repo_path(candidate)
                break
        self._cache[key] = result
        return result


def build_tree(repo: Path, files: list[Path], resolver: Resolver) -> tuple[dict, int, int]:
    tree: dict[str, dict] = {}
    edge_count = 0
    unresolved_count = 0
    for path in files:
        try:
            text = path.read_text(errors="replace")
        except OSError:
            continue
        includes: dict[str, tuple[int, bool]] = {}
        unresolved: dict[str, tuple[int, bool]] = {}
        for lineno, line in enumerate(text.splitlines(), start=1):
            match = INCLUDE_RE.match(line)
            if not match:
                continue
            quoted = match.group(1) == '"'
            target = match.group(2)
            resolved = resolver.resolve(path, target)
            if resolved is None:
                unresolved.setdefault(target, (lineno, quoted))
            else:
                includes.setdefault(resolved, (lineno, quoted))
        entry: dict = {
            "includes": [
                {"target": t, "line": l, "quoted": q} for t, (l, q) in sorted(includes.items())
            ],
        }
        if unresolved:
            # `quoted` distinguishes a real gap (a quoted, project-relative include
            # that resolves nowhere) from a system/toolchain header pulled in via
            # `<...>`, which is expected to never resolve against the repo.
            entry["unresolved"] = [
                {"text": t, "line": l, "quoted": q} for t, (l, q) in sorted(unresolved.items())
            ]
        tree[path.resolve().relative_to(repo).as_posix()] = entry
        edge_count += len(includes)
        unresolved_count += len(unresolved)
    return tree, edge_count, unresolved_count


def main() -> int:
    parser = argparse.ArgumentParser(add_help=True, description=__doc__)
    parser.add_argument("--repo", help="project root; default: git toplevel of the cwd")
    parser.add_argument("--output", help="output file; default: <repo>/ocudu_dependency_tree.yml")
    parser.add_argument(
        "--roots", nargs="+",
        help="source roots to scan, repo-relative; default: every top-level directory "
             "holding sources, minus the excludes",
    )
    parser.add_argument(
        "--exclude", nargs="+", default=[],
        help="extra top-level names to skip (glob, matched against the directory name)",
    )
    parser.add_argument("--quiet", action="store_true", help="suppress the summary line on stderr")
    args = parser.parse_args()

    repo = Path(args.repo).resolve() if args.repo else git_toplevel()
    if repo is None:
        fail("not inside a git repository; pass --repo <path>")
    if not repo.is_dir():
        fail(f"{repo}: no such directory")

    output = Path(args.output) if args.output else repo / "ocudu_dependency_tree.yml"

    excludes = list(DEFAULT_EXCLUDES) + list(args.exclude)
    roots = pick_roots(repo, args.roots, excludes)
    files = collect_sources(roots)
    if not files:
        fail(f"no sources found under {repo} (roots: {[r.name for r in roots]})")

    scanned_roots = sorted({
        f.resolve().relative_to(repo).parts[0] for f in files
    })

    include_roots = [repo / r for r in FIXED_INCLUDE_ROOTS if (repo / r).is_dir()]
    resolver = Resolver(repo, include_roots)
    tree, edge_count, unresolved_count = build_tree(repo, files, resolver)

    doc = {
        "meta": {
            "repo_root": repo.as_posix(),
            "include_roots": [r.relative_to(repo).as_posix() for r in include_roots],
            "roots": scanned_roots,
            "file_count": len(tree),
            "edge_count": edge_count,
            "unresolved_count": unresolved_count,
        },
        "files": tree,
    }

    output.parent.mkdir(parents=True, exist_ok=True)
    with open(output, "w", encoding="utf-8") as handle:
        handle.write("# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited\n")
        handle.write("# SPDX-License-Identifier: BSD-3-Clause-Open-MPI\n")
        handle.write("# Generated by gen_dependency_tree.py — do not edit.\n")
        yaml.dump(
            doc, handle, Dumper=DUMPER,
            sort_keys=True, default_flow_style=False, width=4096, allow_unicode=True,
        )

    if not args.quiet:
        sys.stderr.write(
            f"{output}: {len(tree)} files, {edge_count} edges, "
            f"{unresolved_count} unresolved\n"
        )
    return 0


if __name__ == "__main__":
    sys.exit(main())
