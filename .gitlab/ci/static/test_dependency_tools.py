#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

"""Tests for gen_dependency_tree.py and check_dependency_rules.py.

Runs both scripts against a synthetic C++ project laid out like OCUDU
(include/<proj>/<subsystem> public headers, lib/<subsystem> private sources),
so include resolution, rule evaluation, exemptions and exit codes are
exercised end to end rather than against the live OCUDU tree, whose contents
change independently.

Adapted from the test suite explored in ocudu_tools!17
(tests/dependency/test_dependency_tools.py): that generator resolved via
compile_commands.json, so its project fixture built one and its generator
tests covered compile_commands.json discovery. This one resolves via fixed,
build-independent roots instead (see gen_dependency_tree.py's own docstring
for why), so the fixture needs no build directory at all, and the generator
tests instead cover the fixed-roots and CMakeLists.txt-owned-directory
resolution this version actually does. The checker tests are a direct port:
check_dependency_rules.py's own rule-evaluation logic (glob matching,
transitive BFS, peer isolation, exemptions, changed-files scoping) is
unchanged from !17, YAML tree/rules format included.

Usage:
    python3 test_dependency_tools.py
"""

from __future__ import annotations

import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

import yaml

TEST_DIR = Path(__file__).resolve().parent
GEN = TEST_DIR / "gen_dependency_tree.py"
CHECK = TEST_DIR / "check_dependency_rules.py"
SEED_RULES = TEST_DIR / "ocudu_dependency_rules.yml"

MAC_RULE = {
    "version": 1,
    "rules": [{
        "id": "mac-must-not-depend-on-du",
        "from": "lib/mac/**",
        "to": "include/proj/du/**",
        "reason": "MAC sits below the DU manager.",
    }],
}


def write(root: Path, rel: str, text: str) -> Path:
    path = root / rel
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text)
    return path


def write_yaml(root: Path, rel: str, doc: dict) -> Path:
    return write(root, rel, yaml.safe_dump(doc, sort_keys=False))


def make_project(root: Path, mac_impl_extra: str = "", mac_header_extra: str = "") -> None:
    """A minimal project whose includes cover every resolution path this
    generator supports.

    `proj/...` only resolves through the fixed `include/` root;
    `mac_config.h` only resolves relative to the including file;
    `<vector>` resolves nowhere.
    """
    write(root, "include/proj/du/du_manager.h", "#pragma once\n#include <memory>\n")
    write(root, "include/proj/ran/rnti.h", "#pragma once\n#include <cstdint>\n")
    write(
        root, "include/proj/mac/mac.h",
        '#pragma once\n#include "proj/ran/rnti.h"\n' + mac_header_extra,
    )
    write(
        root, "lib/mac/mac_impl.cpp",
        '#include "proj/mac/mac.h"\n'
        '#include "mac_config.h"\n'
        "#include <vector>\n" + mac_impl_extra,
    )
    write(root, "lib/mac/mac_config.h", '#pragma once\n#include "proj/ran/rnti.h"\n')
    write(root, "lib/e2/e2_impl.cpp", '#include "proj/ran/rnti.h"\n')


def run(script: Path, *args: str) -> subprocess.CompletedProcess:
    return subprocess.run(
        [sys.executable, str(script), *args], capture_output=True, text=True,
    )


def gen(root: Path, *args: str) -> Path:
    """Run the generator against `root`, asserting success, and return the tree path."""
    out = root / "tree.yml"
    result = run(GEN, "--repo", str(root), "--output", str(out), "--quiet", *args)
    assert result.returncode == 0, result.stderr
    return out


class GeneratorTest(unittest.TestCase):
    def test_resolves_every_include_kind(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            make_project(root)
            tree = yaml.safe_load(gen(root).read_text())

            entry = tree["files"]["lib/mac/mac_impl.cpp"]
            targets = {inc["target"] for inc in entry["includes"]}
            # proj/mac/mac.h resolves via the fixed include/ root,
            # mac_config.h relative to the source.
            self.assertEqual(targets, {"include/proj/mac/mac.h", "lib/mac/mac_config.h"})
            unresolved = entry["unresolved"]
            self.assertEqual(len(unresolved), 1)
            self.assertEqual(unresolved[0]["text"], "vector")
            self.assertFalse(unresolved[0]["quoted"])

            self.assertEqual(tree["meta"]["repo_root"], root.as_posix())
            self.assertEqual(sorted(tree["meta"]["roots"]), ["include", "lib"])
            self.assertEqual(tree["meta"]["file_count"], 6)

    def test_quoted_unresolved_include_is_flagged_quoted(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            make_project(root, mac_impl_extra='#include "proj/mac/missing.h"\n')
            tree = yaml.safe_load(gen(root).read_text())
            unresolved = tree["files"]["lib/mac/mac_impl.cpp"]["unresolved"]
            quoted_missing = [u for u in unresolved if u["text"] == "proj/mac/missing.h"]
            self.assertEqual(len(quoted_missing), 1)
            self.assertTrue(quoted_missing[0]["quoted"])

    def test_output_is_deterministic(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            make_project(root)
            out = gen(root)
            first = out.read_bytes()
            gen(root)
            self.assertEqual(first, out.read_bytes())

    def test_build_like_dirs_are_not_scanned(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            make_project(root)
            write(root, "build/generated.h", '#include "proj/ran/rnti.h"\n')
            tree = yaml.safe_load(gen(root).read_text())
            self.assertNotIn("build/generated.h", tree["files"])

    def test_external_fixed_roots_resolve(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            make_project(root)
            write(root, "external/fmt/include/fmt/format.h", "#pragma once\n")
            write(root, "external/cameron314/concurrentqueue.h", "#pragma once\n")
            write(
                root, "lib/mac/mac_impl.cpp",
                '#include "proj/mac/mac.h"\n'
                '#include "mac_config.h"\n'
                "#include <vector>\n"
                '#include "fmt/format.h"\n'
                '#include <cameron314/concurrentqueue.h>\n',
            )
            tree = yaml.safe_load(gen(root).read_text())
            targets = {inc["target"] for inc in tree["files"]["lib/mac/mac_impl.cpp"]["includes"]}
            self.assertIn("external/fmt/include/fmt/format.h", targets)
            self.assertIn("external/cameron314/concurrentqueue.h", targets)

    def test_repo_root_relative_include_resolves(self):
        """apps/ and tests/ include each other by full repo-relative path
        (target_include_directories(... ${CMAKE_SOURCE_DIR})), not just
        relative to include/."""
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            make_project(root)
            write(root, "apps/cu/helper.h", "#pragma once\n")
            write(root, "apps/cu/cu.cpp", '#include "apps/cu/helper.h"\n')
            tree = yaml.safe_load(gen(root).read_text())
            targets = {inc["target"] for inc in tree["files"]["apps/cu/cu.cpp"]["includes"]}
            self.assertEqual(targets, {"apps/cu/helper.h"})

    def test_cmakelists_owned_directory_is_a_root_self_and_parent(self):
        """Mirrors the two idioms found in ocudu's own lib/ CMakeLists.txt:
        target_include_directories(t PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}) (the
        target's own directory) and PRIVATE .. (its parent), both scoped to
        whichever directory owns the nearest CMakeLists.txt — not to the
        including file's own, possibly more deeply nested, directory."""
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            make_project(root)
            write(root, "lib/e2/CMakeLists.txt", "# fixture target\n")
            write(root, "lib/e2/procedures/setup.h", "#pragma once\n")
            write(
                root, "lib/e2/common/e2_impl.cpp",
                '#include "procedures/setup.h"\n',  # self: lib/e2 is the include root
            )
            write(root, "lib/f1ap/cu_cp/CMakeLists.txt", "# fixture target\n")
            write(root, "lib/f1ap/common.h", "#pragma once\n")
            write(
                root, "lib/f1ap/cu_cp/impl.cpp",
                '#include "common.h"\n',  # parent (..): lib/f1ap is the include root
            )
            tree = yaml.safe_load(gen(root).read_text())
            e2_targets = {inc["target"] for inc in tree["files"]["lib/e2/common/e2_impl.cpp"]["includes"]}
            self.assertIn("lib/e2/procedures/setup.h", e2_targets)
            f1ap_targets = {inc["target"] for inc in tree["files"]["lib/f1ap/cu_cp/impl.cpp"]["includes"]}
            self.assertIn("lib/f1ap/common.h", f1ap_targets)


class CheckerTest(unittest.TestCase):
    def build(self, root: Path, rules: dict, **kwargs) -> tuple[Path, Path]:
        make_project(root, **kwargs)
        tree = gen(root)
        rules_path = write_yaml(root, "rules.yml", rules)
        return tree, rules_path

    def check(self, tree: Path, rules: Path, *args: str) -> subprocess.CompletedProcess:
        return run(CHECK, "--tree", str(tree), "--rules", str(rules), *args)

    def json_check(self, tree: Path, rules: Path, *args: str) -> tuple[dict, int]:
        result = self.check(tree, rules, "--json", *args)
        self.assertIn(result.returncode, (0, 1), result.stderr)
        return json.loads(result.stdout), result.returncode

    def test_clean_tree_exits_0(self):
        with tempfile.TemporaryDirectory() as tmp:
            tree, rules = self.build(Path(tmp), MAC_RULE)
            payload, code = self.json_check(tree, rules)
            self.assertEqual(code, 0)
            self.assertEqual(payload["findings"], [])
            self.assertEqual(payload["rule_count"], 1)

    def test_direct_violation_is_reported_with_line(self):
        with tempfile.TemporaryDirectory() as tmp:
            tree, rules = self.build(
                Path(tmp), MAC_RULE,
                mac_impl_extra='#include "proj/du/du_manager.h"\n',
            )
            payload, code = self.json_check(tree, rules)
            self.assertEqual(code, 1)
            self.assertEqual(len(payload["findings"]), 1)
            finding = payload["findings"][0]
            self.assertEqual(finding["rule_id"], "mac-must-not-depend-on-du")
            self.assertEqual(finding["file"], "lib/mac/mac_impl.cpp")
            self.assertEqual(finding["to"], "include/proj/du/du_manager.h")
            # The offending directive is the 4th line of mac_impl.cpp.
            self.assertEqual(finding["line"], 4)
            self.assertEqual(finding["kind"], "forbidden-edge")

    def test_relative_include_violation_anchors_on_its_own_line(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            rules = {
                "version": 1,
                "rules": [{
                    "id": "no-mac-config",
                    "from": "lib/mac/mac_impl.cpp",
                    "to": "lib/mac/mac_config.h",
                    "reason": "Contrived rule pinning a relative include.",
                }],
            }
            tree, rules_path = self.build(root, rules)
            payload, code = self.json_check(tree, rules_path)
            self.assertEqual(code, 1)
            self.assertEqual(payload["findings"][0]["line"], 2)

    def test_transitive_rule_reports_shortest_chain(self):
        with tempfile.TemporaryDirectory() as tmp:
            rules = {
                "version": 1,
                "rules": [{
                    "id": "mac-must-not-reach-du",
                    "from": "lib/mac/**",
                    "to": "include/proj/du/**",
                    "transitive": True,
                    "reason": "MAC must not reach DU headers, even indirectly.",
                }],
            }
            tree, rules_path = self.build(
                Path(tmp), rules,
                mac_header_extra='#include "proj/du/du_manager.h"\n',
            )
            payload, code = self.json_check(tree, rules_path)
            self.assertEqual(code, 1)
            finding = payload["findings"][0]
            self.assertEqual(finding["chain"], [
                "lib/mac/mac_impl.cpp",
                "include/proj/mac/mac.h",
                "include/proj/du/du_manager.h",
            ])
            # Anchored on the edge the from-side file owns, not the deep one.
            self.assertEqual(finding["line"], 1)

    def test_direct_rule_ignores_indirect_reach(self):
        with tempfile.TemporaryDirectory() as tmp:
            tree, rules = self.build(
                Path(tmp), MAC_RULE,
                mac_header_extra='#include "proj/du/du_manager.h"\n',
            )
            payload, code = self.json_check(tree, rules)
            self.assertEqual(code, 0)
            self.assertEqual(payload["findings"], [])

    def test_peer_isolation_flags_sibling_subsystem(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            rules = {
                "version": 1,
                "rules": [{
                    "id": "peers-are-private",
                    "kind": "peer-isolation",
                    "peers": "lib/*",
                    "reason": "Subsystems talk through public headers only.",
                }],
            }
            make_project(root)
            write(root, "lib/e2/e2_impl.cpp", '#include "../mac/mac_config.h"\n')
            tree = gen(root)
            rules_path = write_yaml(root, "rules.yml", rules)
            payload, code = self.json_check(tree, rules_path)
            self.assertEqual(code, 1)
            finding = payload["findings"][0]
            self.assertEqual(finding["file"], "lib/e2/e2_impl.cpp")
            self.assertEqual(finding["to"], "lib/mac/mac_config.h")
            self.assertEqual(finding["kind"], "peer-isolation")

    def test_peer_isolation_allows_same_subsystem(self):
        with tempfile.TemporaryDirectory() as tmp:
            rules = {
                "version": 1,
                "rules": [{
                    "id": "peers-are-private",
                    "kind": "peer-isolation",
                    "peers": "lib/*",
                    "reason": "Subsystems talk through public headers only.",
                }],
            }
            tree, rules_path = self.build(Path(tmp), rules)
            # lib/mac/mac_impl.cpp includes lib/mac/mac_config.h, a same-peer edge.
            payload, code = self.json_check(tree, rules_path)
            self.assertEqual(code, 0)
            self.assertEqual(payload["findings"], [])

    def test_exemption_suppresses_violation(self):
        with tempfile.TemporaryDirectory() as tmp:
            rules = {
                "version": 1,
                "rules": [{
                    "id": "mac-must-not-depend-on-du",
                    "from": "lib/mac/**",
                    "to": "include/proj/du/**",
                    "reason": "MAC sits below the DU manager.",
                    "exempt": [{
                        "from": "lib/mac/mac_impl.cpp",
                        "to": "include/proj/du/du_manager.h",
                        "reason": "tracked in PROJ-1234",
                    }],
                }],
            }
            tree, rules_path = self.build(
                Path(tmp), rules,
                mac_impl_extra='#include "proj/du/du_manager.h"\n',
            )
            payload, code = self.json_check(tree, rules_path)
            self.assertEqual(code, 0)
            self.assertEqual(payload["findings"], [])
            self.assertEqual(payload["warnings"], [])

    def test_unused_exemption_warns(self):
        with tempfile.TemporaryDirectory() as tmp:
            rules = {
                "version": 1,
                "rules": [{
                    "id": "mac-must-not-depend-on-du",
                    "from": "lib/mac/**",
                    "to": "include/proj/du/**",
                    "reason": "MAC sits below the DU manager.",
                    "exempt": [{
                        "from": "lib/mac/mac_impl.cpp",
                        "to": "include/proj/du/du_manager.h",
                        "reason": "tracked in PROJ-1234",
                    }],
                }],
            }
            tree, rules_path = self.build(Path(tmp), rules)
            payload, code = self.json_check(tree, rules_path)
            self.assertEqual(code, 0)
            self.assertEqual(len(payload["warnings"]), 1)
            self.assertIn("no longer matches", payload["warnings"][0])

    def test_changed_files_partition(self):
        with tempfile.TemporaryDirectory() as tmp:
            tree, rules = self.build(
                Path(tmp), MAC_RULE,
                mac_impl_extra='#include "proj/du/du_manager.h"\n',
            )
            payload, code = self.json_check(tree, rules, "--changed-files", "lib/e2/e2_impl.cpp")
            self.assertEqual(code, 0)
            self.assertEqual(payload["findings"], [])
            self.assertEqual(payload["pre_existing_count"], 1)
            self.assertTrue(payload["scoped"])

            payload, code = self.json_check(
                tree, rules, "--changed-files", "lib/mac/mac_impl.cpp",
            )
            self.assertEqual(code, 1)
            self.assertEqual(len(payload["findings"]), 1)
            self.assertEqual(payload["pre_existing_count"], 0)

    def test_changed_files_from_stdin(self):
        with tempfile.TemporaryDirectory() as tmp:
            tree, rules = self.build(
                Path(tmp), MAC_RULE,
                mac_impl_extra='#include "proj/du/du_manager.h"\n',
            )
            result = subprocess.run(
                [sys.executable, str(CHECK), "--tree", str(tree), "--rules", str(rules),
                 "--json", "--changed-files-from", "-"],
                input="lib/mac/mac_impl.cpp\n", capture_output=True, text=True,
            )
            self.assertEqual(result.returncode, 1)
            self.assertEqual(len(json.loads(result.stdout)["findings"]), 1)


class IncludeOnlyTargetTest(unittest.TestCase):
    """Third-party trees (external/) are edge targets but are never scanned
    for their own edges, so they appear in the tree only on the right-hand
    side. Rules must still be able to name them."""

    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.root = Path(self._tmp.name)
        make_project(self.root)
        write(self.root, "external/fmt/include/fmt/format.h", "#pragma once\n")
        write(
            self.root, "include/proj/mac/mac.h",
            '#pragma once\n#include "proj/ran/rnti.h"\n#include "fmt/format.h"\n',
        )
        self.tree = gen(self.root)

    def tearDown(self):
        self._tmp.cleanup()

    def test_external_is_a_target_but_not_a_key(self):
        tree = yaml.safe_load(self.tree.read_text())
        targets = {inc["target"] for inc in tree["files"]["include/proj/mac/mac.h"]["includes"]}
        self.assertIn("external/fmt/include/fmt/format.h", targets)
        self.assertNotIn("external/fmt/include/fmt/format.h", tree["files"])

    def test_rule_targeting_external_is_not_stale(self):
        rules = write_yaml(self.root, "rules.yml", {
            "version": 1,
            "rules": [{
                "id": "public-headers-must-not-include-external",
                "from": "include/**",
                "to": "external/**",
                "reason": "Public headers must not expose third-party types.",
            }],
        })
        result = run(CHECK, "--tree", str(self.tree), "--rules", str(rules), "--json")
        self.assertEqual(result.returncode, 1, result.stderr)
        findings = json.loads(result.stdout)["findings"]
        self.assertEqual(len(findings), 1)
        self.assertEqual(findings[0]["to"], "external/fmt/include/fmt/format.h")
        self.assertEqual(findings[0]["file"], "include/proj/mac/mac.h")
        self.assertEqual(findings[0]["line"], 3)


class RulesetValidationTest(unittest.TestCase):
    def prepare(self, root: Path, rules: dict) -> tuple[Path, Path]:
        make_project(root)
        tree = gen(root)
        return tree, write_yaml(root, "rules.yml", rules)

    def expect_broken(self, rules: dict, needle: str) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            tree, rules_path = self.prepare(Path(tmp), rules)
            result = run(CHECK, "--tree", str(tree), "--rules", str(rules_path))
            self.assertEqual(result.returncode, 2, result.stdout)
            self.assertIn(needle, result.stderr)

    def test_stale_pattern_is_an_error(self):
        self.expect_broken(
            {"version": 1, "rules": [{
                "id": "renamed-away",
                "from": "lib/mac/**",
                "to": "include/proj/du_manager/**",
                "reason": "The target directory was renamed, so this rule enforces nothing.",
            }]},
            "matches no files",
        )

    def test_stale_peers_pattern_is_an_error(self):
        self.expect_broken(
            {"version": 1, "rules": [{
                "id": "no-such-peers",
                "kind": "peer-isolation",
                "peers": "components/*",
                "reason": "There is no components/ directory.",
            }]},
            "matches no directories",
        )

    def test_duplicate_id_is_an_error(self):
        self.expect_broken(
            {"version": 1, "rules": [
                {"id": "same", "from": "lib/mac/**", "to": "include/proj/du/**", "reason": "First."},
                {"id": "same", "from": "lib/e2/**", "to": "include/proj/du/**", "reason": "Second."},
            ]},
            "duplicate id",
        )

    def test_unknown_key_is_an_error(self):
        self.expect_broken(
            {"version": 1, "rules": [{
                "id": "typo",
                "form": "lib/mac/**",
                "to": "include/proj/du/**",
                "reason": "`form` is a typo for `from`.",
            }]},
            "unknown key",
        )

    def test_missing_reason_is_an_error(self):
        self.expect_broken(
            {"version": 1, "rules": [{
                "id": "no-reason",
                "from": "lib/mac/**",
                "to": "include/proj/du/**",
            }]},
            "missing `reason`",
        )

    def test_unsupported_version_is_an_error(self):
        self.expect_broken(
            {"version": 2, "rules": [{
                "id": "whatever",
                "from": "lib/mac/**",
                "to": "include/proj/du/**",
                "reason": "Version 2 does not exist.",
            }]},
            "unsupported rules version",
        )

    def test_transitive_peer_isolation_is_rejected(self):
        self.expect_broken(
            {"version": 1, "rules": [{
                "id": "peers-transitive",
                "kind": "peer-isolation",
                "peers": "lib/*",
                "transitive": True,
                "reason": "Transitive peer isolation is not defined.",
            }]},
            "not supported for kind 'peer-isolation'",
        )

    def test_missing_tree_exits_2(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            rules_path = write_yaml(root, "rules.yml", MAC_RULE)
            result = run(CHECK, "--tree", str(root / "nope.yml"), "--rules", str(rules_path))
            self.assertEqual(result.returncode, 2)
            self.assertIn("gen_dependency_tree.py", result.stderr)


class GlobSemanticsTest(unittest.TestCase):
    def test_single_star_does_not_cross_slash(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            make_project(root)
            write(root, "lib/mac/detail/inner.cpp", '#include "proj/du/du_manager.h"\n')
            tree = gen(root)
            # `lib/mac/*.cpp` must not reach lib/mac/detail/inner.cpp.
            rules_path = write_yaml(root, "rules.yml", {
                "version": 1, "rules": [{
                    "id": "shallow-only",
                    "from": "lib/mac/*.cpp",
                    "to": "include/proj/du/**",
                    "reason": "Only direct children of lib/mac are in scope.",
                }],
            })
            result = run(CHECK, "--tree", str(tree), "--rules", str(rules_path), "--json")
            self.assertEqual(result.returncode, 0, result.stdout)

            rules_path = write_yaml(root, "rules_deep.yml", {
                "version": 1, "rules": [{
                    "id": "deep-too",
                    "from": "lib/mac/**",
                    "to": "include/proj/du/**",
                    "reason": "Every file under lib/mac is in scope.",
                }],
            })
            result = run(CHECK, "--tree", str(tree), "--rules", str(rules_path), "--json")
            self.assertEqual(result.returncode, 1)
            findings = json.loads(result.stdout)["findings"]
            self.assertEqual([f["file"] for f in findings], ["lib/mac/detail/inner.cpp"])


class SeedRulesTest(unittest.TestCase):
    def test_seed_ruleset_parses_and_is_self_consistent(self):
        doc = yaml.safe_load(SEED_RULES.read_text())
        self.assertEqual(doc["version"], 1)
        ids = [rule["id"] for rule in doc["rules"]]
        self.assertEqual(len(ids), len(set(ids)))
        for rule in doc["rules"]:
            self.assertTrue(rule.get("reason", "").strip(), rule["id"])
            if rule.get("kind") == "peer-isolation":
                self.assertIn("peers", rule)
            else:
                self.assertIn("from", rule)
                self.assertIn("to", rule)


if __name__ == "__main__":
    unittest.main(verbosity=2)
