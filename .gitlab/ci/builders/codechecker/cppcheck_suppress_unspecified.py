#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

"""
Remove unspecified checks from cppcheck generated results
"""

import os
import sys
from xml.etree import ElementTree

UNSPECIFIED_CHECK_NAME_ARRAY = (
    "cppcheck-templateRecursion",
    "cppcheck-syntaxError",
    "cppcheck-internalAstError",
)
CPPCHECK_RESULT_EXT = ".plist"


def _parse_file(filename: str):
    changed = False
    tree = ElementTree.parse(filename)
    diagnostic_array = tree.getroot().find("dict").find("array")  # type: ignore
    for entry in diagnostic_array.findall("dict"):  # type: ignore
        for string_item in entry.findall("string"):
            if string_item.text in UNSPECIFIED_CHECK_NAME_ARRAY:
                diagnostic_array.remove(entry)  # type: ignore
                changed = True
                break
    if changed:
        tree.write(filename, xml_declaration=True)


def main():
    """
    Remove unspecified checks from cppcheck generated results
    """
    folder = os.path.abspath(sys.argv[1])
    for root, _, files in os.walk(folder):
        for filename in files:
            if filename.endswith(CPPCHECK_RESULT_EXT):
                abs_filename = os.path.join(root, filename)
                _parse_file(abs_filename)


if __name__ == "__main__":
    main()
