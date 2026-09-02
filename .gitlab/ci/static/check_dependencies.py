#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

"""
check_dependencies.py — Entrypoint for every #include dependency check.

Generates one dependency tree with gen_dependency_tree.py, then runs both
checks against it: include_directives_check.py (the per-module allow-list)
and check_dependency_rules.py (peer isolation, transitive reachability, and
layering rules the allow-list can't express). One entrypoint, one tree, one
CI job — the checks themselves stay separate scripts because they read the
tree differently and are independently useful to run on their own.

Usage:
  python3 check_dependencies.py [options]

Options:
  --repo <path>    Project root. Default: git toplevel of the cwd.
  --rules <path>   Rules YAML for check_dependency_rules.py. Default:
                   ocudu_dependency_rules.yml beside this script.

Exit codes:
  0  no violations from either check
  1  violations found by either check
  2  either check's ruleset or inputs are broken (stale pattern, duplicate
     id, missing tree)
"""

from __future__ import annotations

import argparse
import subprocess
import sys
import tempfile
from pathlib import Path

SCRIPT_DIR = Path(__file__).resolve().parent


def run(*args: str) -> int:
    result = subprocess.run([sys.executable, "-u", *args])
    return result.returncode


def main() -> int:
    parser = argparse.ArgumentParser(add_help=True, description=__doc__)
    parser.add_argument("--repo")
    parser.add_argument("--rules")
    args = parser.parse_args()

    repo_args = ["--repo", args.repo] if args.repo else []
    rules_args = ["--rules", args.rules] if args.rules else []

    with tempfile.TemporaryDirectory() as tmp:
        tree = str(Path(tmp) / "ocudu_dependency_tree.yml")

        gen_code = run(str(SCRIPT_DIR / "gen_dependency_tree.py"), *repo_args, "--output", tree)
        if gen_code != 0:
            return gen_code

        print("### include-directives (per-module allow-list) ###", flush=True)
        allow_list_code = run(
            str(SCRIPT_DIR / "include_directives_check.py"), "--tree", tree, *repo_args,
        )

        print("\n### dependency-rules (peer isolation, layering, transitive reachability) ###", flush=True)
        rules_code = run(
            str(SCRIPT_DIR / "check_dependency_rules.py"), "--tree", tree, *rules_args,
        )

    if 2 in (allow_list_code, rules_code):
        return 2
    return 1 if (allow_list_code or rules_code) else 0


if __name__ == "__main__":
    sys.exit(main())
