#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

"""
Check that the include directives respect the architecture rules.

Reads the resolved #include graph written by gen_dependency_tree.py rather
than guessing a target's module from the literal include text: every edge
here points at a real file, so a target's module is read off its actual
location instead of inferred from how it happened to be spelled.

That also collapses two textual heuristics the previous, text-only version of
this check needed and this one does not:

  - It used to skip any include starting with `..`, on the assumption that a
    relative-parent include always stays inside the same module. Real
    resolution makes that assumption unnecessary: the target's own module is
    known outright, so a `..`-spelled include that happens to cross a module
    boundary is now caught, and one that does not is allowed on the same
    "target module == source module" basis as any other same-module include
    (which every module's rule set already permits — it always lists itself).
  - It only checked non-`ocudu/`-prefixed includes from lib/ files when they
    still looked module-qualified (contained a `/`), to avoid mistaking a
    same-directory `#include "foo.h"` for a module reference. Real resolution
    makes the distinction directly: a same-directory include resolves inside
    the source's own module and is allowed for the same reason as above.

Everything else — the per-module allow-list, the submodule and
filename-prefix carve-outs, the specific-file exceptions, the public-header
relative-include ban — is unchanged from the original.
"""

from __future__ import annotations

import argparse
import sys

try:
    import yaml
except ImportError:
    sys.stderr.write("error: PyYAML required: pip install pyyaml\n")
    sys.exit(2)

LOADER = getattr(yaml, "CSafeLoader", yaml.SafeLoader)
from pathlib import Path

# Third-party folders always allowed regardless of source module.
EXTERNAL_ALLOWED = {
    'Backward', 'cameron314', 'CLI', 'cmake-sbom', 'fmt',
    'nlohmann', 'rigtorp', 'TartanLlama', 'uWebSockets',
}

# Allowed modules per source module.
ALLOWED_INCLUDES = {
    'adt': {'adt', 'ocudulog', 'support'},
    'asn1': {'adt', 'asn1', 'ocudulog', 'support'},
    'cu_cp': {'adt', 'asn1', 'cu_cp', 'e1ap/common', 'e1ap/cu_cp', 'f1ap/cu_cp', 'gateways', 'ngap', 'nrppa',
              'ocudulog', 'pdcp', 'ran', 'rrc', 'sdap', 'security', 'support', 'xnap'},
    'cu_up': {'adt', 'cu_up', 'e1ap', 'e2', 'f1u', 'gtpu', 'nru', 'ocudulog', 'pcap', 'pdcp', 'ran', 'rohc',
              'sdap', 'support'},
    'du': {'adt', 'asn1/f1ap', 'du', 'f1ap/du', 'f1u', 'mac', 'ntn', 'ocudulog', 'pcap', 'phy', 'ran', 'rlc',
           'scheduler', 'support'},
    'du/du_high': {'adt', 'asn1', 'du', 'du/du_high', 'f1ap', 'f1u', 'gtpu', 'mac', 'ntn', 'ocudulog', 'pcap', 'ran',
                   'rlc', 'scheduler', 'support'},
    'du/du_low': {'adt', 'du', 'du/du_low', 'ocudulog', 'pcap', 'phy', 'ran', 'support'},
    'e1ap': {'adt', 'asn1/e1ap', 'e1ap', 'gateways', 'ocudulog', 'pcap', 'ran', 'security', 'support'},
    'e2': {'adt', 'asn1/e2', 'asn1/e2ap', 'asn1/e2sm', 'cu_cp', 'du/du_high', 'e2', 'f1ap', 'f1u', 'gateways',
           'ocudulog', 'pcap', 'pdcp', 'ran', 'rlc', 'scheduler', 'security', 'support'},
    'f1ap': {'adt', 'asn1/f1ap', 'f1ap', 'gateways', 'pcap', 'ocudulog', 'ran', 'support'},
    'fapi': {'adt', 'fapi', 'ocudulog', 'ocuduvec', 'ran', 'support'},
    'fapi_adaptor': {'adt', 'fapi', 'fapi_adaptor', 'ocudulog', 'ocuduvec', 'ran', 'support'},
    'fapi_adaptor/mac': {'adt', 'fapi', 'fapi_adaptor', 'fapi_adaptor/mac', 'mac', 'ocudulog', 'ocuduvec', 'ran',
                         'scheduler', 'support'},
    'fapi_adaptor/phy': {'adt', 'fapi', 'fapi_adaptor', 'fapi_adaptor/phy', 'ocudulog', 'ocuduvec', 'phy', 'ran',
                         'support'},
    'gateways': {'adt', 'gateways', 'ocudulog', 'support'},
    'gtpu': {'adt', 'gateways', 'gtpu', 'nru', 'ocudulog', 'pcap', 'psup', 'ran', 'support'},
    'hal': {'adt', 'hal', 'ocudulog', 'ocuduvec', 'ran', 'support'},
    'instrumentation': {'adt', 'instrumentation', 'ocudulog', 'ran', 'support', 'tracy'},
    'mac': {'adt', 'mac', 'ocudulog', 'pcap', 'ran', 'scheduler', 'support'},
    'ngap': {'adt', 'asn1/ngap', 'gateways', 'pcap', 'ngap', 'ocudulog', 'ran', 'security', 'support'},
    'nrppa': {'adt', 'asn1/nrppa', 'nrppa', 'ocudulog', 'ran', 'support'},
    'nru': {'adt', 'nru', 'ocudulog', 'ran', 'support'},
    'ntn': {'adt', 'ntn', 'ocudulog', 'ran', 'support'},
    'ocudulog': {'ocudulog'},
    'ocuduvec': {'adt', 'ocudulog', 'ocuduvec', 'support'},
    'ofh': {'adt', 'ocudulog', 'ocuduvec', 'ofh', 'phy', 'ran', 'support'},
    'pcap': {'adt', 'ocudulog', 'ocuduvec', 'pcap', 'ran', 'rlc', 'support'},
    'pdcp': {'adt', 'ocudulog', 'pdcp', 'ran', 'rohc', 'security', 'support'},
    'phy': {'adt', 'gateways/baseband', 'ocudulog', 'ocuduvec', 'phy', 'ran', 'support'},
    'phy/lower': {'adt', 'gateways/baseband', 'hal', 'ocudulog', 'ocuduvec', 'phy', 'phy/lower', 'ran', 'support'},
    'phy/upper': {'adt', 'hal', 'ocudulog', 'ocuduvec', 'phy', 'phy/upper', 'ran', 'support'},
    'psup': {'adt', 'ocudulog', 'psup', 'ran', 'support'},
    'radio': {'adt', 'gateways', 'ocudulog', 'ocuduvec', 'radio', 'ran', 'support'},
    'ran': {'adt', 'ocudulog', 'ocuduvec', 'ran', 'support'},
    'rlc': {'adt', 'ocudulog', 'pcap', 'ran', 'rlc', 'support'},
    'rohc': {'adt', 'ocudulog', 'ran', 'rohc', 'support'},
    'rrc': {'adt', 'asn1/rrc', 'asn1/rrc_nr', 'ocudulog', 'ran', 'rrc', 'security', 'support'},
    'ru': {'adt', 'instrumentation', 'ntn', 'ocudulog', 'ocuduvec', 'ofh', 'phy/support', 'phy/upper', 'radio', 'ran',
           'ru', 'support'},
    'ru/dummy': {'adt', 'instrumentation', 'ntn', 'ocudulog', 'ocuduvec', 'phy/support', 'radio', 'ran', 'ru',
                 'support'},
    'ru/ofh': {'adt', 'gateways/baseband', 'instrumentation', 'ntn', 'ocudulog', 'ocuduvec', 'ofh', 'phy', 'radio',
               'ran', 'ru', 'support'},
    'ru/sdr': {'adt', 'gateways/baseband', 'instrumentation', 'ntn', 'ocudulog', 'ocuduvec', 'phy', 'radio', 'ran',
               'ru', 'sdr', 'support'},
    'scheduler': {'adt', 'ntn', 'ocudulog', 'ran', 'scheduler', 'support'},
    'sdap': {'adt', 'ocudulog', 'ran', 'sdap', 'support'},
    'security': {'adt', 'asn1', 'ocudulog', 'ocuduvec', 'ran', 'security', 'support'},
    'support': {'adt', 'ocudulog', 'ocuduvec', 'support'},
    'xnap': {'adt', 'asn1/xnap', 'gateways', 'ocudulog', 'pcap', 'ran', 'security', 'support', 'xnap'},
}

# Folders within a module that are treated as distinct sub-modules.
# Maps module -> {subfolder_name -> ALLOWED_INCLUDES key}.
_SUB_MODULES: dict[str, dict[str, str]] = {
    'du': {'du_high': 'du/du_high', 'du_low': 'du/du_low'},
    'fapi_adaptor': {'mac': 'fapi_adaptor/mac', 'phy': 'fapi_adaptor/phy'},
    'phy': {'lower': 'phy/lower', 'upper': 'phy/upper'},
    'ru': {'dummy': 'ru/dummy', 'ofh': 'ru/ofh', 'sdr': 'ru/sdr'},
}

# Extra allowed modules for files whose name starts with a given prefix.
# Maps module -> [(filename_prefix, {extra_allowed_modules})].
_FILENAME_PREFIX_EXTRAS: dict[str, list[tuple[str, set[str]]]] = {
    'du': [('o_du', {'fapi', 'fapi_adaptor', 'hal'})],
    'du/du_high': [('o_du_high', {'e2', 'fapi', 'fapi_adaptor', 'hal'})],
    'du/du_low': [('o_du_low', {'fapi', 'fapi_adaptor', 'hal'})],
    'cu_cp': [('o_cu_cp', {'e2'})],
}

# Extra allowed especific files for a given module.
_SPECIFIC_ALLOWED_FILES: dict[str, list[str]] = {
    'cu_cp': ['ocudu/f1ap/f1ap_ue_id_types.h', 'ocudu/f1ap/f1ap_message.h'],
    'e2': ['ocudu/asn1/asn1_utils.h', 'ocudu/asn1/asn1_ap_utils.h'],
    'e1ap': ['ocudu/asn1/asn1_utils.h'],
    'f1ap': ['ocudu/asn1/asn1_utils.h'],
    'fapi_adaptor/phy': ['ocudu/instrumentation/traces/critical_traces.h', 'ocudu/instrumentation/traces/du_traces.h'],
    'ngap': ['ocudu/asn1/asn1_utils.h'],
    'nrppa': ['ocudu/asn1/asn1_utils.h'],
    'mac': ['ocudu/instrumentation/traces/du_traces.h', 'ocudu/instrumentation/traces/up_traces.h'],
    'ofh': ['ocudu/instrumentation/traces/ofh_traces.h'],
    'pdcp': ['ocudu/instrumentation/traces/up_traces.h', 'ocudu/instrumentation/traces/tracy_profiler.h'],
    'phy': ['ocudu/instrumentation/traces/du_traces.h'],
    'phy/lower': ['ocudu/instrumentation/traces/ru_traces.h', 'ocudu/instrumentation/traces/critical_traces.h'],
    'phy/upper': ['ocudu/instrumentation/traces/du_traces.h'],
    'rlc': ['ocudu/asn1/asn1_utils.h', 'ocudu/instrumentation/traces/du_traces.h',
            'ocudu/instrumentation/traces/up_traces.h'],
    'rrc': ['ocudu/asn1/asn1_utils.h'],
    'xnap': ['ocudu/asn1/asn1_utils.h']
}


def is_in_include_tree(parts: tuple[str, ...]) -> bool:
    """Return True if a repo-relative path lives under an include/ directory."""
    return 'include' in parts


def get_module(parts: tuple[str, ...]) -> str | None:
    """Return the module name for a repo-relative path under include/ or lib/."""
    for i, part in enumerate(parts):
        if part in ('include', 'lib'):
            if i + 1 >= len(parts):
                return None
            # ocudu is part of the path for include/ocudu/<module>/... but not for lib/<module>/...
            next_part = parts[i + 1]
            module_idx = i + 2 if next_part == 'ocudu' else i + 1

            if module_idx >= len(parts):
                return None

            module = parts[module_idx]

            sub_modules = _SUB_MODULES.get(module, {})
            if sub_modules and module_idx + 1 < len(parts):
                candidate = parts[module_idx + 1]
                if candidate in sub_modules:
                    return sub_modules[candidate]

            return module

    return None


def classify_target(target: str) -> str | None:
    """Return the module a *resolved* include target belongs to.

    Unlike get_module(), this reads two levels below include/ocudu/, lib/ or
    external/ when present (e.g. 'f1ap/cu_cp', 'cameron314'), so a module's
    allow-list can name a specific submodule as a target even where the
    owning-file classification above only ever needs the coarser name.
    """
    parts = Path(target).parts
    if not parts:
        return None
    if parts[0] == 'external':
        return parts[1] if len(parts) >= 2 else None
    if parts[0] in ('include', 'lib'):
        rest = parts[1:]
        if rest and rest[0] == 'ocudu':
            rest = rest[1:]
        if len(rest) >= 3:
            return f'{rest[0]}/{rest[1]}'
        if len(rest) >= 2:
            return rest[0]
        return None
    return None


def is_relative_include(repo: Path, source: str, line: int) -> bool:
    """True if the #include on this line of `source` uses a `..` component."""
    try:
        text = (repo / source).read_text(errors='replace').splitlines()[line - 1]
    except (OSError, IndexError):
        return False
    stripped = text.split('include', 1)
    if len(stripped) < 2:
        return False
    quote = stripped[1].strip().lstrip('#').strip()
    for opener, closer in (('"', '"'), ('<', '>')):
        if quote.startswith(opener):
            end = quote.find(closer, 1)
            if end != -1:
                return '..' in Path(quote[1:end]).parts
    return False


def check_file(rel_path: str, entry: dict, repo: Path) -> tuple[list[str], list[str]]:
    """Return (violations, warnings) for a single file already in the tree."""
    parts = Path(rel_path).parts
    module = get_module(parts)
    if module is None:
        return [], []

    allowed = ALLOWED_INCLUDES.get(module)
    if allowed is None:
        return [], []

    extras_list = _FILENAME_PREFIX_EXTRAS.get(module, [])
    if extras_list:
        filename = Path(rel_path).name
        for prefix, extras in extras_list:
            if filename.startswith(prefix):
                allowed = allowed | extras
                break

    in_include = is_in_include_tree(parts)
    violations: list[str] = []
    warnings: list[str] = []

    for edge in entry.get('includes', []):
        target, lineno = edge['target'], edge['line']
        if in_include and is_relative_include(repo, rel_path, lineno):
            violations.append(
                f"{rel_path}:{lineno}: [{module}] forbidden relative include "
                f"in include/ tree -> {target}"
            )
            continue
        inc_module = classify_target(target)
        if inc_module is None or inc_module in allowed or inc_module in EXTERNAL_ALLOWED:
            continue
        top_module = inc_module.split('/')[0]
        if top_module in allowed or top_module in EXTERNAL_ALLOWED:
            continue
        canonical = target.split('include/', 1)[-1] if 'include/' in target else target
        if canonical in _SPECIFIC_ALLOWED_FILES.get(module, []):
            continue
        violations.append(
            f"{rel_path}:{lineno}: [{module}] forbidden include of "
            f"'{inc_module}' -> {target}"
        )

    for unresolved in entry.get('unresolved', []):
        if not unresolved.get('quoted', True):
            # A `<...>` include that resolves nowhere is a system/toolchain header,
            # never a project one — out of scope, same as the original text-only check.
            continue
        text, lineno = unresolved['text'], unresolved['line']
        warnings.append(f"{rel_path}:{lineno}: [{module}] unresolved include -> \"{text}\"")

    return violations, warnings


def load_tree(path: Path) -> dict:
    if not path.is_file():
        fail(
            f"{path}: no dependency tree. Generate it first:\n"
            f"  python3 gen_dependency_tree.py"
        )
    try:
        return yaml.load(path.read_text(), Loader=LOADER)
    except (OSError, yaml.YAMLError) as exc:
        fail(f"{path}: {exc}")


def fail(msg: str) -> None:
    print(f"error: {msg}", file=sys.stderr)
    sys.exit(2)


def main() -> int:
    parser = argparse.ArgumentParser(add_help=True, description=__doc__)
    parser.add_argument('--tree', required=True, help='dependency tree YAML from gen_dependency_tree.py')
    parser.add_argument('--repo', help='project root, for re-reading a line to check for a relative include')
    args = parser.parse_args()

    doc = load_tree(Path(args.tree))
    meta = doc.get('meta') or {}
    repo = Path(args.repo).resolve() if args.repo else Path(meta.get('repo_root', '.'))

    violations: list[str] = []
    warnings: list[str] = []
    for rel_path, entry in sorted(doc.get('files', {}).items()):
        v, w = check_file(rel_path, entry, repo)
        violations.extend(v)
        warnings.extend(w)

    for w in warnings:
        print(w)
    for v in violations:
        print(v)

    if warnings:
        print(f"{len(warnings)} unresolved include(s) — not counted as violations.")
    print(f"{len(violations)} include directive violation(s) found.")
    return 1 if violations else 0


if __name__ == '__main__':
    sys.exit(main())
