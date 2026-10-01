#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# for actual source and header files
find . -type f \( -name "*.c" -o -name "*.cc" -o -name "*.cpp" -o -name "*.h" -o -name "*.h.in" \) \( ! -path "*/bundled/*" ! -path "*/external/*" \) -exec sed -i 's/0x[0-9a-fA-F]*/\L&/g' {} \;
